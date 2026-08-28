#!/usr/bin/env python3
"""Dependency-free stdio MCP bridge for the Zygisk IL2CPP command socket."""

from __future__ import annotations

import argparse
import json
import math
import os
import socket
import struct
import subprocess
import sys
import threading
from dataclasses import dataclass
from typing import Any, Callable


SERVER_NAME = "zygisk-il2cpp-mcp"
SERVER_VERSION = "2.0.0"
LATEST_PROTOCOL = "2025-11-25"
SUPPORTED_PROTOCOLS = {
    "2024-11-05",
    "2025-03-26",
    "2025-06-18",
    "2025-11-25",
}
MAX_HEADER_BYTES = 16 * 1024
MAX_BODY_BYTES = 4 * 1024 * 1024


class BridgeError(RuntimeError):
    """A connection, protocol, or hook-call failure suitable for an MCP result."""


def _single_line(value: Any, field: str, *, allow_pipe: bool = True) -> str:
    text = str(value)
    if "\n" in text or "\r" in text or "\x00" in text:
        raise BridgeError(f"{field} must be a single line")
    if not allow_pipe and "|" in text:
        raise BridgeError(f"{field} cannot contain '|'")
    return text


@dataclass
class ConnectionConfig:
    host: str = "127.0.0.1"
    port: int = 27184
    timeout: float = 5.0
    auto_adb_forward: bool = True
    adb_path: str = "adb"
    adb_serial: str | None = None

    def validate(self) -> None:
        if not self.host:
            raise BridgeError("host cannot be empty")
        if not 1 <= self.port <= 65535:
            raise BridgeError("port must be between 1 and 65535")
        if not 0.1 <= self.timeout <= 300:
            raise BridgeError("timeout must be between 0.1 and 300 seconds")


class HookSocketClient:
    """Client for the native `OK <length>\n<body>` command protocol."""

    def __init__(self, config: ConnectionConfig):
        self.config = config

    @staticmethod
    def _recv_line(sock: socket.socket) -> str:
        data = bytearray()
        while len(data) < MAX_HEADER_BYTES:
            chunk = sock.recv(1)
            if not chunk:
                raise BridgeError("hook service closed the connection")
            if chunk == b"\n":
                return data.decode("utf-8", errors="strict")
            data.extend(chunk)
        raise BridgeError("hook service returned an oversized response header")

    @staticmethod
    def _recv_exact(sock: socket.socket, length: int) -> bytes:
        if length < 0 or length > MAX_BODY_BYTES:
            raise BridgeError("hook service returned an invalid response length")
        data = bytearray()
        while len(data) < length:
            chunk = sock.recv(length - len(data))
            if not chunk:
                raise BridgeError("hook service closed before the response completed")
            data.extend(chunk)
        return bytes(data)

    def call(self, command: str, *, timeout: float | None = None, retry_forward: bool = True) -> str:
        command = _single_line(command, "command").strip()
        if not command:
            raise BridgeError("command cannot be empty")
        if len(command.encode("utf-8")) > 64 * 1024:
            raise BridgeError("command is too long")

        try:
            with socket.create_connection(
                (self.config.host, self.config.port),
                timeout=timeout or self.config.timeout,
            ) as sock:
                sock.settimeout(timeout or self.config.timeout)
                sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                sock.sendall(command.encode("utf-8") + b"\n")
                header = self._recv_line(sock)
                if header.startswith("ERR "):
                    raise BridgeError(header[4:].strip() or "hook call failed")
                if not header.startswith("OK "):
                    raise BridgeError(f"unexpected hook response: {header!r}")
                try:
                    body_length = int(header[3:].strip())
                except ValueError as exc:
                    raise BridgeError(f"invalid hook response header: {header!r}") from exc
                return self._recv_exact(sock, body_length).decode("utf-8", errors="strict").rstrip("\n")
        except BridgeError:
            raise
        except OSError as exc:
            if retry_forward and self._can_auto_forward():
                self._adb_forward()
                return self.call(command, timeout=timeout, retry_forward=False)
            raise BridgeError(
                f"cannot connect to hook service at {self.config.host}:{self.config.port}: {exc}"
            ) from exc

    def _can_auto_forward(self) -> bool:
        return self.config.auto_adb_forward and self.config.host in {"127.0.0.1", "localhost", "::1"}

    def _adb_forward(self) -> None:
        args = [self.config.adb_path]
        if self.config.adb_serial:
            args.extend(["-s", self.config.adb_serial])
        args.extend(["forward", f"tcp:{self.config.port}", f"tcp:{self.config.port}"])
        creation_flags = subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0
        try:
            result = subprocess.run(
                args,
                capture_output=True,
                text=True,
                timeout=15,
                check=False,
                creationflags=creation_flags,
            )
        except (OSError, subprocess.SubprocessError) as exc:
            raise BridgeError(f"adb forward failed: {exc}") from exc
        if result.returncode != 0:
            detail = (result.stderr or result.stdout or "unknown adb error").strip()
            raise BridgeError(f"adb forward failed: {detail}")


EMPTY_SCHEMA = {"type": "object", "additionalProperties": False}

TOOLS: list[dict[str, Any]] = [
    {
        "name": "ping",
        "title": "Check hook connection",
        "description": "Check whether the target game hook service is reachable.",
        "inputSchema": EMPTY_SCHEMA,
        "annotations": {"readOnlyHint": True, "openWorldHint": False},
    },
    {
        "name": "connection_info",
        "title": "Get connection settings",
        "description": "Return the current host, port, timeout, and ADB forwarding settings.",
        "inputSchema": EMPTY_SCHEMA,
        "annotations": {"readOnlyHint": True, "openWorldHint": False},
    },
    {
        "name": "configure_connection",
        "title": "Configure hook connection",
        "description": "Change the hook service connection for later MCP calls.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "host": {"type": "string", "description": "Hook service host."},
                "port": {"type": "integer", "minimum": 1, "maximum": 65535},
                "timeout": {"type": "number", "minimum": 0.1, "maximum": 300},
                "auto_adb_forward": {"type": "boolean"},
                "adb_serial": {"type": ["string", "null"]},
            },
            "additionalProperties": False,
        },
        "annotations": {"openWorldHint": False},
    },
    {
        "name": "get_clipboard",
        "title": "Read game clipboard",
        "description": "Read clipboard text through the injected IL2CPP hook.",
        "inputSchema": EMPTY_SCHEMA,
        "annotations": {"readOnlyHint": True, "openWorldHint": False},
    },
    {
        "name": "mcp_toast_status",
        "title": "Get MCP Toast status",
        "description": "Return whether in-app JNI Toast notifications for MCP calls are enabled.",
        "inputSchema": EMPTY_SCHEMA,
        "annotations": {"readOnlyHint": True, "openWorldHint": False},
    },
    {
        "name": "mcp_toast_set_enabled",
        "title": "Enable or disable MCP Toast",
        "description": "Control automatic in-app JNI Toast notifications that show MCP tool calls.",
        "inputSchema": {
            "type": "object",
            "properties": {"enabled": {"type": "boolean"}},
            "required": ["enabled"],
            "additionalProperties": False,
        },
        "annotations": {"openWorldHint": False},
    },
    {
        "name": "mcp_toast_show",
        "title": "Show an in-app Toast",
        "description": "Immediately show custom text inside the target app through JNI, independent of the automatic switch.",
        "inputSchema": {
            "type": "object",
            "properties": {"message": {"type": "string", "maxLength": 1000}},
            "required": ["message"],
            "additionalProperties": False,
        },
        "annotations": {"openWorldHint": False},
    },
    {
        "name": "show_input_box",
        "title": "Show Unity input box",
        "description": "Open the hooked Unity TouchScreenKeyboard without waiting for completion.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "title": {"type": "string", "default": "输入"},
                "hint": {"type": "string", "default": "请输入内容"},
                "prefill": {"type": "string", "default": ""},
            },
            "additionalProperties": False,
        },
        "annotations": {"openWorldHint": False},
    },
    {
        "name": "wait_input",
        "title": "Wait for Unity input",
        "description": "Wait for the currently open Unity input box and return its result.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "timeout_ms": {"type": "integer", "minimum": 100, "maximum": 300000, "default": 120000},
            },
            "additionalProperties": False,
        },
        "annotations": {"readOnlyHint": True, "openWorldHint": False},
    },
    {
        "name": "input_and_wait",
        "title": "Show input box and wait",
        "description": "Open the Unity input box, wait for completion, and return the entered text.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "title": {"type": "string", "default": "输入"},
                "hint": {"type": "string", "default": "请输入内容"},
                "prefill": {"type": "string", "default": ""},
                "timeout_ms": {"type": "integer", "minimum": 100, "maximum": 300000, "default": 120000},
            },
            "additionalProperties": False,
        },
        "annotations": {"openWorldHint": False},
    },
    {
        "name": "push_input_result",
        "title": "Push input result",
        "description": "Push a result into the hook input state, primarily for automation and testing.",
        "inputSchema": {
            "type": "object",
            "properties": {"text": {"type": "string"}},
            "required": ["text"],
            "additionalProperties": False,
        },
        "annotations": {"openWorldHint": False},
    },
    {
        "name": "raw_hook_call",
        "title": "Call raw hook command",
        "description": "Send any single-line native socket command, including commands added by future hooks.",
        "inputSchema": {
            "type": "object",
            "properties": {
                "command": {"type": "string", "description": "A single native command line without CR/LF."},
                "timeout": {"type": "number", "minimum": 0.1, "maximum": 300},
            },
            "required": ["command"],
            "additionalProperties": False,
        },
        "annotations": {"openWorldHint": False},
    },
]

METHOD_LOOKUP_PROPERTIES: dict[str, Any] = {
    "image": {"type": "string", "description": "IL2CPP image name, for example Assembly-CSharp.dll."},
    "namespace": {"type": "string", "description": "Exact namespace; use an empty string for the global namespace."},
    "class_name": {"type": "string", "description": "Exact class name."},
    "method": {"type": "string", "description": "Exact method name."},
    "parameter_count": {"type": "integer", "minimum": -1, "default": -1},
}

RETURN_KIND_SCHEMA = {
    "type": "string",
    "enum": ["void", "bool", "i64", "u64", "ptr", "f32", "f64"],
    "description": "Native ABI return kind used by the generated Dobby replacement stub.",
}

MEMORY_VALUE_FORMATS: dict[str, str] = {
    "bool": "B",
    "i8": "b",
    "u8": "B",
    "i16": "h",
    "u16": "H",
    "i32": "i",
    "u32": "I",
    "i64": "q",
    "u64": "Q",
    "f32": "f",
    "f64": "d",
    "ptr32": "I",
    "ptr64": "Q",
}

MEMORY_VALUE_KIND_SCHEMA = {
    "type": "string",
    "enum": list(MEMORY_VALUE_FORMATS),
    "description": "Little-endian value type. Choose ptr32 or ptr64 to match the target process ABI.",
}

MEMORY_REGION_TYPES = [
    "anonymous", "heap", "stack", "app_code", "system_code",
    "app_data", "ashmem", "java", "other",
]

MEMORY_REGION_TYPES_SCHEMA = {
    "type": "array",
    "items": {"type": "string", "enum": MEMORY_REGION_TYPES},
    "uniqueItems": True,
    "minItems": 1,
    "description": "One or more /proc/self/maps region classes. Omit to scan every readable class.",
}

TOOLS.extend(
    [
        {
            "name": "il2cpp_status",
            "title": "Get IL2CPP runtime status",
            "description": "Initialize/attach to IL2CPP and return its base and domain addresses.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "il2cpp_dump_file",
            "title": "Dump IL2CPP metadata to app storage",
            "description": "Write fields, properties, methods, addresses, and RVAs directly to files/zygisk_il2cpp_mcp/il2cpp_dump.cs in the target app private directory. Returns success only; dump data is never sent through MCP.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "il2cpp_list_images",
            "title": "List IL2CPP images",
            "description": "List loaded IL2CPP assembly image names.",
            "inputSchema": {
                "type": "object",
                "properties": {"limit": {"type": "integer", "minimum": 1, "maximum": 5000, "default": 256}},
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "il2cpp_list_classes",
            "title": "List IL2CPP classes",
            "description": "List or filter classes in an IL2CPP image.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "image": METHOD_LOOKUP_PROPERTIES["image"],
                    "namespace_filter": {"type": "string", "default": ""},
                    "name_filter": {"type": "string", "default": ""},
                    "limit": {"type": "integer", "minimum": 1, "maximum": 5000, "default": 200},
                },
                "required": ["image"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "il2cpp_list_methods",
            "title": "List IL2CPP methods",
            "description": "List methods, native addresses, RVAs, parameter types, and return types for a class.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "image": METHOD_LOOKUP_PROPERTIES["image"],
                    "namespace": METHOD_LOOKUP_PROPERTIES["namespace"],
                    "class_name": METHOD_LOOKUP_PROPERTIES["class_name"],
                    "name_filter": {"type": "string", "default": ""},
                    "limit": {"type": "integer", "minimum": 1, "maximum": 5000, "default": 200},
                },
                "required": ["image", "namespace", "class_name"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "il2cpp_find_method",
            "title": "Resolve IL2CPP method",
            "description": "Resolve an IL2CPP method to metadata, native address, and RVA.",
            "inputSchema": {
                "type": "object",
                "properties": METHOD_LOOKUP_PROPERTIES,
                "required": ["image", "namespace", "class_name", "method"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "il2cpp_invoke",
            "title": "Invoke IL2CPP method",
            "description": "Invoke a static or addressed instance IL2CPP method through il2cpp_runtime_invoke. Supports primitive, string, null, pointer, and enum arguments. Enums accept an underlying integer, a member name such as Walking or RoleSyncState.Walking, or an explicit {\"enum\": \"RoleSyncState.Walking\"} value.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    **METHOD_LOOKUP_PROPERTIES,
                    "instance_address": {"type": "string", "default": "0x0"},
                    "arguments": {
                        "type": "array",
                        "items": {
                            "anyOf": [
                                {"type": ["string", "number", "boolean", "null"]},
                                {
                                    "type": "object",
                                    "properties": {
                                        "enum": {
                                            "type": ["string", "integer"],
                                            "description": "Enum member name (optionally Type.Member), flags joined by |, or its underlying integer value.",
                                        }
                                    },
                                    "required": ["enum"],
                                    "additionalProperties": False,
                                },
                            ]
                        },
                        "maxItems": 64,
                        "default": [],
                    },
                },
                "required": ["image", "namespace", "class_name", "method"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "il2cpp_hook",
            "title": "Hook IL2CPP method",
            "description": "Resolve an IL2CPP method and install a Dobby hook to an explicit native replacement address; returns the original trampoline address.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    **METHOD_LOOKUP_PROPERTIES,
                    "replacement_address": {"type": "string"},
                },
                "required": ["image", "namespace", "class_name", "method", "replacement_address"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "il2cpp_hook_return",
            "title": "Hook IL2CPP method return",
            "description": "Resolve an IL2CPP method and install a Dobby replacement that returns a fixed ABI value.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    **METHOD_LOOKUP_PROPERTIES,
                    "return_kind": RETURN_KIND_SCHEMA,
                    "value": {"type": ["string", "number", "boolean", "null"]},
                },
                "required": ["image", "namespace", "class_name", "method", "return_kind"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "il2cpp_unhook",
            "title": "Remove IL2CPP method hook",
            "description": "Resolve an IL2CPP method and restore it with DobbyDestroy.",
            "inputSchema": {
                "type": "object",
                "properties": METHOD_LOOKUP_PROPERTIES,
                "required": ["image", "namespace", "class_name", "method"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "memory_backend_status",
            "title": "Get memory backend status",
            "description": "Return the WebUI-selected memory read/write backend and its lazy probe state. This does not initialize a kernel driver.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_read",
            "title": "Read target-process memory",
            "description": "Read 1 to 65536 bytes from a fully readable mapped range in the target process. Returns lowercase hexadecimal bytes.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "address": {"type": "string", "description": "Absolute target-process address, normally hexadecimal."},
                    "size": {"type": "integer", "minimum": 1, "maximum": 65536},
                },
                "required": ["address", "size"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_write",
            "title": "Write target-process memory",
            "description": "Write 1 to 65536 hexadecimal bytes to a fully readable and writable mapped range in the target process. Returns the previous bytes and verifies the write. Use dobby_patch_code for executable code pages.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "address": {"type": "string", "description": "Absolute target-process address, normally hexadecimal."},
                    "hex_bytes": {
                        "type": "string",
                        "minLength": 2,
                        "maxLength": 131072,
                        "pattern": "^[0-9A-Fa-f]+$",
                    },
                },
                "required": ["address", "hex_bytes"],
                "additionalProperties": False,
            },
            "annotations": {"destructiveHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_read_value",
            "title": "Read a typed memory value",
            "description": "Read one little-endian boolean, integer, floating-point, or 32/64-bit pointer value from target-process memory.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "address": {"type": "string"},
                    "value_type": MEMORY_VALUE_KIND_SCHEMA,
                },
                "required": ["address", "value_type"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_write_value",
            "title": "Write a typed memory value",
            "description": "Encode and write one little-endian boolean, integer, floating-point, or 32/64-bit pointer value. Returns the previous typed value.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "address": {"type": "string"},
                    "value_type": MEMORY_VALUE_KIND_SCHEMA,
                    "value": {"type": ["string", "number", "boolean"]},
                },
                "required": ["address", "value_type", "value"],
                "additionalProperties": False,
            },
            "annotations": {"destructiveHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_list_modules",
            "title": "List loaded native modules",
            "description": "List executable modules in the target process with path, occurrence index, start/end, load bias, size, and region count. Duplicate basenames receive occurrence values starting at 1.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "name_filter": {"type": "string", "default": ""},
                    "limit": {"type": "integer", "minimum": 1, "maximum": 4096, "default": 256},
                },
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_find_module",
            "title": "Find a loaded module instance",
            "description": "Resolve an exact module basename or full path and its 1-based occurrence. Returns module start/end and every mapped region.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "module_name": {"type": "string"},
                    "occurrence": {"type": "integer", "minimum": 1, "maximum": 4096, "default": 1},
                },
                "required": ["module_name"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_address_info",
            "title": "Locate an address in memory maps",
            "description": "Return the exact mapped region containing an address and, when applicable, the owning module, module occurrence, and relative offsets.",
            "inputSchema": {
                "type": "object",
                "properties": {"address": {"type": "string"}},
                "required": ["address"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_search",
            "title": "Search target-process memory",
            "description": "Search a module instance or explicit address range for a byte pattern. Supports byte and nibble wildcards such as '48 8B ?? A?' and creates a filterable search session.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "pattern": {"type": "string", "description": "Hexadecimal byte pattern; ? is a wildcard nibble."},
                    "module_name": {"type": "string", "description": "Exact basename or full path. Mutually exclusive with explicit range."},
                    "occurrence": {"type": "integer", "minimum": 1, "maximum": 4096, "default": 1},
                    "start_address": {"type": "string"},
                    "end_address": {"type": "string"},
                    "max_results": {"type": "integer", "minimum": 1, "maximum": 10000, "default": 1024},
                    "alignment": {"type": "integer", "minimum": 1, "maximum": 4096, "default": 1},
                    "memory_types": MEMORY_REGION_TYPES_SCHEMA,
                },
                "required": ["pattern"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_search_value",
            "title": "Search for a typed memory value",
            "description": "Encode a little-endian typed value and search a module instance or explicit range. Returns a filterable search session.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "value_type": MEMORY_VALUE_KIND_SCHEMA,
                    "value": {"type": ["string", "number", "boolean"]},
                    "module_name": {"type": "string"},
                    "occurrence": {"type": "integer", "minimum": 1, "maximum": 4096, "default": 1},
                    "start_address": {"type": "string"},
                    "end_address": {"type": "string"},
                    "max_results": {"type": "integer", "minimum": 1, "maximum": 10000, "default": 1024},
                    "alignment": {"type": "integer", "minimum": 1, "maximum": 4096, "description": "Defaults to the encoded value size."},
                    "memory_types": MEMORY_REGION_TYPES_SCHEMA,
                },
                "required": ["value_type", "value"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_search_exact",
            "title": "Exact-search multiple value types",
            "description": "Search the same value as multiple selected integer/floating/pointer encodings. Each encoding creates an independent filterable session.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "value_types": {"type": "array", "items": MEMORY_VALUE_KIND_SCHEMA, "minItems": 1, "uniqueItems": True},
                    "value": {"type": ["string", "number", "boolean"]},
                    "module_name": {"type": "string"},
                    "occurrence": {"type": "integer", "minimum": 1, "maximum": 4096, "default": 1},
                    "start_address": {"type": "string"},
                    "end_address": {"type": "string"},
                    "max_results": {"type": "integer", "minimum": 1, "maximum": 10000, "default": 1024},
                    "alignment": {"type": "integer", "minimum": 1, "maximum": 4096},
                    "memory_types": MEMORY_REGION_TYPES_SCHEMA,
                },
                "required": ["value_types", "value"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_search_fuzzy",
            "title": "Start a fuzzy/unknown-value search",
            "description": "Snapshot aligned values without requiring an initial exact value, then narrow the session with changed, unchanged, increased, decreased, equals, or not-equals filters.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "value_size": {"type": "integer", "enum": [1, 2, 4, 8]},
                    "module_name": {"type": "string"},
                    "occurrence": {"type": "integer", "minimum": 1, "maximum": 4096, "default": 1},
                    "start_address": {"type": "string"},
                    "end_address": {"type": "string"},
                    "max_results": {"type": "integer", "minimum": 1, "maximum": 10000, "default": 10000},
                    "alignment": {"type": "integer", "minimum": 1, "maximum": 4096},
                    "memory_types": MEMORY_REGION_TYPES_SCHEMA,
                },
                "required": ["value_size"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_filter",
            "title": "Filter memory search results",
            "description": "Narrow a search session by equals/not_equals pattern, changed/unchanged snapshot, or unsigned little-endian increased/decreased comparison. Retained snapshots update after each filter.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "session_id": {"type": "integer", "minimum": 1},
                    "mode": {"type": "string", "enum": ["equals", "not_equals", "changed", "unchanged", "increased", "decreased"]},
                    "pattern": {"type": "string", "description": "Required for equals/not_equals; byte length must equal the original search pattern."},
                },
                "required": ["session_id", "mode"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_filter_value",
            "title": "Filter search results by typed value",
            "description": "Narrow a search session to typed values equal or not equal to the supplied value.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "session_id": {"type": "integer", "minimum": 1},
                    "mode": {"type": "string", "enum": ["equals", "not_equals"]},
                    "value_type": MEMORY_VALUE_KIND_SCHEMA,
                    "value": {"type": ["string", "number", "boolean"]},
                },
                "required": ["session_id", "mode", "value_type", "value"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_search_results",
            "title": "Page memory-search results",
            "description": "Return a bounded page of addresses and current snapshot bytes from a search session.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "session_id": {"type": "integer", "minimum": 1},
                    "offset": {"type": "integer", "minimum": 0, "maximum": 10000, "default": 0},
                    "limit": {"type": "integer", "minimum": 1, "maximum": 1000, "default": 100},
                },
                "required": ["session_id"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "memory_search_clear",
            "title": "Clear a memory search session",
            "description": "Release one native memory-search session and its saved snapshots.",
            "inputSchema": {
                "type": "object",
                "properties": {"session_id": {"type": "integer", "minimum": 1}},
                "required": ["session_id"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "dobby_version",
            "title": "Get Dobby version",
            "description": "Return the embedded Dobby runtime version.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "dobby_resolve_symbol",
            "title": "Resolve native symbol",
            "description": "Resolve a native symbol using DobbySymbolResolver.",
            "inputSchema": {
                "type": "object",
                "properties": {"image": {"type": "string", "default": ""}, "symbol": {"type": "string"}},
                "required": ["symbol"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "dobby_hook",
            "title": "Install raw Dobby hook",
            "description": "Call DobbyHook with explicit target and replacement addresses; returns the original trampoline address.",
            "inputSchema": {
                "type": "object",
                "properties": {"target_address": {"type": "string"}, "replacement_address": {"type": "string"}},
                "required": ["target_address", "replacement_address"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "dobby_hook_return",
            "title": "Install fixed-return Dobby hook",
            "description": "Install a generated replacement stub at a raw address with a fixed return value.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "target_address": {"type": "string"},
                    "return_kind": RETURN_KIND_SCHEMA,
                    "value": {"type": ["string", "number", "boolean", "null"]},
                },
                "required": ["target_address", "return_kind"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "dobby_instrument",
            "title": "Instrument native address",
            "description": "Install DobbyInstrument at an address and count executions.",
            "inputSchema": {
                "type": "object",
                "properties": {"address": {"type": "string"}},
                "required": ["address"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "dobby_trace_get",
            "title": "Read instrumentation count",
            "description": "Return the execution count for a Dobby-instrumented address.",
            "inputSchema": {
                "type": "object",
                "properties": {"address": {"type": "string"}},
                "required": ["address"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "dobby_patch_code",
            "title": "Patch native code",
            "description": "Apply hexadecimal machine-code bytes with DobbyCodePatch (maximum 4096 bytes).",
            "inputSchema": {
                "type": "object",
                "properties": {"address": {"type": "string"}, "hex_bytes": {"type": "string", "pattern": "^[0-9A-Fa-f]+$"}},
                "required": ["address", "hex_bytes"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "dobby_destroy",
            "title": "Destroy Dobby hook",
            "description": "Restore a hook, instrumentation point, or tracked patch using DobbyDestroy.",
            "inputSchema": {
                "type": "object",
                "properties": {"target_address": {"type": "string"}},
                "required": ["target_address"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "dobby_list_hooks",
            "title": "List active Dobby hooks",
            "description": "List fixed-return hooks, raw hooks, and instrumentation counters installed through this bridge.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "debug_help",
            "title": "Explain native debug commands",
            "description": "List custom native bridge commands or return usage and an explanation for one command.",
            "inputSchema": {
                "type": "object",
                "properties": {"command": {"type": "string", "description": "Optional native command such as ASM_PATCH or LUA_EXEC."}},
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "runtime_capabilities",
            "title": "Get optional runtime capabilities",
            "description": "Report memory backend, LuaJIT, assembly, breakpoint, Dobby, and IL2CPP availability without eagerly loading optional subsystems.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "lua_status",
            "title": "Get LuaJIT status",
            "description": "Report LuaJIT availability and VM state without creating the VM.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "lua_execute",
            "title": "Execute LuaJIT script",
            "description": "Lazily create the embedded LuaJIT VM and execute a script with safe memory, module, call, Dobby hook, and instrumentation APIs.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "script": {"type": "string", "maxLength": 262144},
                    "timeout": {"type": "number", "minimum": 0.1, "maximum": 300, "default": 30},
                },
                "required": ["script"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "lua_reset",
            "title": "Reset LuaJIT VM",
            "description": "Remove hooks owned by Lua and replace the LuaJIT VM with a clean lazy state.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "lua_logs",
            "title": "Read LuaJIT callback logs",
            "description": "Read persistent output from scripts, hook callbacks, and main-thread call closures, optionally clearing it after reading.",
            "inputSchema": {
                "type": "object",
                "properties": {"clear": {"type": "boolean", "default": True}},
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "assembly_status",
            "title": "Get assembly engine status",
            "description": "Report whether the ARM64 AsmJit/Capstone/Dobby assembly engine is available.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "assembly_assemble",
            "title": "Assemble ARM64 instruction",
            "description": "Assemble one AArch64 textual instruction into machine-code bytes without changing memory.",
            "inputSchema": {
                "type": "object",
                "properties": {"instruction": {"type": "string", "maxLength": 1024}},
                "required": ["instruction"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "assembly_disassemble",
            "title": "Disassemble ARM64 memory",
            "description": "Read through the original system path and disassemble an executable ARM64 memory range with Capstone.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "address": {"type": "string"},
                    "size": {"type": "integer", "minimum": 1, "maximum": 4096, "default": 64},
                    "max_instructions": {"type": "integer", "minimum": 1, "maximum": 256, "default": 16},
                },
                "required": ["address"],
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "assembly_patch",
            "title": "Assemble and patch ARM64 code",
            "description": "Assemble one AArch64 instruction and patch an executable address with DobbyCodePatch.",
            "inputSchema": {
                "type": "object",
                "properties": {"address": {"type": "string"}, "instruction": {"type": "string", "maxLength": 1024}},
                "required": ["address", "instruction"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "breakpoint_status",
            "title": "Get hardware breakpoint status",
            "description": "Report ARM64 perf hardware breakpoint support and lazy initialization state.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "breakpoint_set",
            "title": "Set hardware breakpoint/watchpoint",
            "description": "Set a non-stopping execution, read, write, or read/write ARM64 perf hardware breakpoint for all current and future target threads.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "address": {"type": "string"},
                    "type": {"type": "string", "enum": ["x", "r", "w", "rw"]},
                    "length": {"type": "integer", "enum": [1, 2, 4, 8], "default": 4},
                },
                "required": ["address", "type"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "breakpoint_list",
            "title": "List hardware breakpoints",
            "description": "List active hardware breakpoints and watchpoints.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "breakpoint_hits",
            "title": "Read breakpoint hits",
            "description": "Page captured breakpoint hits with thread id and ARM64 registers; omit address to include all breakpoints.",
            "inputSchema": {
                "type": "object",
                "properties": {
                    "address": {"type": "string"},
                    "offset": {"type": "integer", "minimum": 0, "maximum": 1000000, "default": 0},
                    "limit": {"type": "integer", "minimum": 1, "maximum": 512, "default": 100},
                },
                "additionalProperties": False,
            },
            "annotations": {"readOnlyHint": True, "openWorldHint": False},
        },
        {
            "name": "breakpoint_clear",
            "title": "Clear hardware breakpoint",
            "description": "Clear a hardware breakpoint/watchpoint from every target thread.",
            "inputSchema": {
                "type": "object",
                "properties": {"address": {"type": "string"}},
                "required": ["address"],
                "additionalProperties": False,
            },
            "annotations": {"openWorldHint": False},
        },
        {
            "name": "breakpoint_clear_all",
            "title": "Clear all hardware breakpoints",
            "description": "Clear every hardware breakpoint/watchpoint while leaving all other runtime features active.",
            "inputSchema": EMPTY_SCHEMA,
            "annotations": {"openWorldHint": False},
        },
    ]
)


class ToolDispatcher:
    def __init__(self, config: ConnectionConfig):
        config.validate()
        self.config = config
        self._lock = threading.RLock()

    def _client(self) -> HookSocketClient:
        return HookSocketClient(self.config)

    def _info(self) -> dict[str, Any]:
        return {
            "host": self.config.host,
            "port": self.config.port,
            "timeout": self.config.timeout,
            "auto_adb_forward": self.config.auto_adb_forward,
            "adb_serial": self.config.adb_serial,
        }

    def call(self, name: str, arguments: dict[str, Any]) -> dict[str, Any]:
        methods: dict[str, Callable[[dict[str, Any]], dict[str, Any]]] = {
            "ping": self.ping,
            "connection_info": self.connection_info,
            "configure_connection": self.configure_connection,
            "get_clipboard": self.get_clipboard,
            "show_input_box": self.show_input_box,
            "wait_input": self.wait_input,
            "input_and_wait": self.input_and_wait,
            "push_input_result": self.push_input_result,
            "raw_hook_call": self.raw_hook_call,
            "mcp_toast_status": self.mcp_toast_status,
            "mcp_toast_set_enabled": self.mcp_toast_set_enabled,
            "mcp_toast_show": self.mcp_toast_show,
            "il2cpp_status": self.il2cpp_status,
            "il2cpp_dump_file": self.il2cpp_dump_file,
            "il2cpp_list_images": self.il2cpp_list_images,
            "il2cpp_list_classes": self.il2cpp_list_classes,
            "il2cpp_list_methods": self.il2cpp_list_methods,
            "il2cpp_find_method": self.il2cpp_find_method,
            "il2cpp_invoke": self.il2cpp_invoke,
            "il2cpp_hook": self.il2cpp_hook,
            "il2cpp_hook_return": self.il2cpp_hook_return,
            "il2cpp_unhook": self.il2cpp_unhook,
            "memory_backend_status": self.memory_backend_status,
            "memory_read": self.memory_read,
            "memory_write": self.memory_write,
            "memory_read_value": self.memory_read_value,
            "memory_write_value": self.memory_write_value,
            "memory_list_modules": self.memory_list_modules,
            "memory_find_module": self.memory_find_module,
            "memory_address_info": self.memory_address_info,
            "memory_search": self.memory_search,
            "memory_search_value": self.memory_search_value,
            "memory_search_exact": self.memory_search_exact,
            "memory_search_fuzzy": self.memory_search_fuzzy,
            "memory_filter": self.memory_filter,
            "memory_filter_value": self.memory_filter_value,
            "memory_search_results": self.memory_search_results,
            "memory_search_clear": self.memory_search_clear,
            "dobby_version": self.dobby_version,
            "dobby_resolve_symbol": self.dobby_resolve_symbol,
            "dobby_hook": self.dobby_hook,
            "dobby_hook_return": self.dobby_hook_return,
            "dobby_instrument": self.dobby_instrument,
            "dobby_trace_get": self.dobby_trace_get,
            "dobby_patch_code": self.dobby_patch_code,
            "dobby_destroy": self.dobby_destroy,
            "dobby_list_hooks": self.dobby_list_hooks,
            "debug_help": self.debug_help,
            "runtime_capabilities": self.runtime_capabilities,
            "lua_status": self.lua_status,
            "lua_execute": self.lua_execute,
            "lua_reset": self.lua_reset,
            "lua_logs": self.lua_logs,
            "assembly_status": self.assembly_status,
            "assembly_assemble": self.assembly_assemble,
            "assembly_disassemble": self.assembly_disassemble,
            "assembly_patch": self.assembly_patch,
            "breakpoint_status": self.breakpoint_status,
            "breakpoint_set": self.breakpoint_set,
            "breakpoint_list": self.breakpoint_list,
            "breakpoint_hits": self.breakpoint_hits,
            "breakpoint_clear": self.breakpoint_clear,
            "breakpoint_clear_all": self.breakpoint_clear_all,
        }
        method = methods.get(name)
        if method is None:
            raise BridgeError(f"unknown tool: {name}")
        if not isinstance(arguments, dict):
            raise BridgeError("tool arguments must be an object")
        with self._lock:
            self._notify_mcp_call(name, arguments)
            return method(arguments)

    def _notify_mcp_call(self, name: str, arguments: dict[str, Any]) -> None:
        if name in {"connection_info", "configure_connection", "mcp_toast_set_enabled", "mcp_toast_show"}:
            return
        message = json.dumps(
            {"tool": name, "arguments": arguments},
            ensure_ascii=False,
            separators=(",", ":"),
        )
        if len(message) > 1000:
            message = message[:997] + "..."
        try:
            self._client().call(f"UI_TOAST_NOTIFY {self._hex_text(message)}")
        except BridgeError:
            # Toast reporting must never prevent the requested MCP operation.
            pass

    def ping(self, _: dict[str, Any]) -> dict[str, Any]:
        response = self._client().call("PING")
        if response != "PONG":
            raise BridgeError(f"unexpected ping response: {response!r}")
        return {"connected": True, **self._info(), "response": response}

    def connection_info(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._info()

    def configure_connection(self, args: dict[str, Any]) -> dict[str, Any]:
        candidate = ConnectionConfig(
            host=self.config.host,
            port=self.config.port,
            timeout=self.config.timeout,
            auto_adb_forward=self.config.auto_adb_forward,
            adb_path=self.config.adb_path,
            adb_serial=self.config.adb_serial,
        )
        if "host" in args:
            if not isinstance(args["host"], str):
                raise BridgeError("host must be a string")
            candidate.host = args["host"]
        if "port" in args:
            if isinstance(args["port"], bool):
                raise BridgeError("port must be an integer")
            candidate.port = int(args["port"])
        if "timeout" in args:
            if isinstance(args["timeout"], bool):
                raise BridgeError("timeout must be a number")
            candidate.timeout = float(args["timeout"])
        if "auto_adb_forward" in args:
            if not isinstance(args["auto_adb_forward"], bool):
                raise BridgeError("auto_adb_forward must be a boolean")
            candidate.auto_adb_forward = args["auto_adb_forward"]
        if "adb_serial" in args:
            serial = args["adb_serial"]
            if serial is not None and not isinstance(serial, str):
                raise BridgeError("adb_serial must be a string or null")
            candidate.adb_serial = None if serial is None or serial.strip() == "" else serial
        candidate.validate()
        self.config = candidate
        return self._info()

    def get_clipboard(self, _: dict[str, Any]) -> dict[str, Any]:
        return {"text": self._client().call("GET_CLIPBOARD")}

    def mcp_toast_status(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("UI_TOAST_STATUS")

    def mcp_toast_set_enabled(self, args: dict[str, Any]) -> dict[str, Any]:
        enabled = args.get("enabled")
        if not isinstance(enabled, bool):
            raise BridgeError("enabled must be a boolean")
        return self._json_call(f"UI_TOAST_SET {'true' if enabled else 'false'}")

    def mcp_toast_show(self, args: dict[str, Any]) -> dict[str, Any]:
        message = self._required_text(args, "message")
        if not message:
            raise BridgeError("message cannot be empty")
        if len(message) > 1000:
            raise BridgeError("message cannot exceed 1000 characters")
        return self._json_call(f"UI_TOAST_SHOW {self._hex_text(message)}")

    @staticmethod
    def _input_args(args: dict[str, Any]) -> tuple[str, str, str]:
        title = _single_line(args.get("title", "输入"), "title", allow_pipe=False)
        hint = _single_line(args.get("hint", "请输入内容"), "hint", allow_pipe=False)
        prefill = _single_line(args.get("prefill", ""), "prefill", allow_pipe=False)
        return title, hint, prefill

    def show_input_box(self, args: dict[str, Any]) -> dict[str, Any]:
        title, hint, prefill = self._input_args(args)
        response = self._client().call(f"SHOW_INPUT_BOX {title}|{hint}|{prefill}")
        return {"shown": response == "INPUT_BOX_SHOWN", "response": response}

    def wait_input(self, args: dict[str, Any]) -> dict[str, Any]:
        timeout_ms = int(args.get("timeout_ms", 120000))
        if not 100 <= timeout_ms <= 300000:
            raise BridgeError("timeout_ms must be between 100 and 300000")
        text = self._client().call(f"WAIT_INPUT {timeout_ms}", timeout=timeout_ms / 1000 + 5)
        return {"text": text}

    def input_and_wait(self, args: dict[str, Any]) -> dict[str, Any]:
        shown = self.show_input_box(args)
        if not shown["shown"]:
            raise BridgeError(f"input box did not open: {shown['response']}")
        return self.wait_input(args)

    def push_input_result(self, args: dict[str, Any]) -> dict[str, Any]:
        text = _single_line(args.get("text", ""), "text")
        response = self._client().call(f"INPUT_RESULT {text}")
        return {"accepted": response == "OK", "response": response}

    def raw_hook_call(self, args: dict[str, Any]) -> dict[str, Any]:
        command = _single_line(args.get("command", ""), "command").strip()
        timeout = float(args.get("timeout", self.config.timeout))
        if not 0.1 <= timeout <= 300:
            raise BridgeError("timeout must be between 0.1 and 300 seconds")
        return {"response": self._client().call(command, timeout=timeout)}

    @staticmethod
    def _hex_text(value: Any) -> str:
        encoded = str(value).encode("utf-8").hex()
        return encoded or "-"

    @staticmethod
    def _required_text(args: dict[str, Any], name: str) -> str:
        if name not in args:
            raise BridgeError(f"missing required argument: {name}")
        if not isinstance(args[name], str):
            raise BridgeError(f"{name} must be a string")
        return _single_line(args[name], name)

    @staticmethod
    def _address(value: Any, name: str) -> str:
        text = _single_line(value, name).strip()
        try:
            parsed = int(text, 0)
        except ValueError as exc:
            raise BridgeError(f"{name} must be a hexadecimal or decimal address") from exc
        if parsed < 0 or parsed > (1 << 64) - 1:
            raise BridgeError(f"{name} is out of range")
        return hex(parsed)

    def _json_call(self, command: str, *, timeout: float | None = None) -> dict[str, Any]:
        response = self._client().call(command, timeout=timeout)
        try:
            value = json.loads(response)
        except json.JSONDecodeError as exc:
            raise BridgeError(f"native bridge returned invalid JSON for {command.split()[0]}: {response!r}") from exc
        if not isinstance(value, dict):
            raise BridgeError("native bridge returned a non-object JSON result")
        return value

    def _lookup_words(self, args: dict[str, Any]) -> list[str]:
        return [
            self._hex_text(self._required_text(args, "image")),
            self._hex_text(self._required_text(args, "namespace")),
            self._hex_text(self._required_text(args, "class_name")),
            self._hex_text(self._required_text(args, "method")),
            str(int(args.get("parameter_count", -1))),
        ]

    @staticmethod
    def _return_value(args: dict[str, Any]) -> str:
        kind = str(args.get("return_kind", ""))
        value = args.get("value")
        if kind == "void":
            return "0"
        if kind == "bool":
            if isinstance(value, bool):
                return "true" if value else "false"
            text = str(value).lower()
            if text in {"true", "1"}:
                return "true"
            if text in {"false", "0"}:
                return "false"
            raise BridgeError("bool hook value must be true or false")
        if value is None:
            raise BridgeError(f"value is required for return kind {kind}")
        return _single_line(value, "value")

    @classmethod
    def _invoke_token(cls, value: Any) -> str:
        if isinstance(value, dict):
            if set(value) != {"enum"}:
                raise BridgeError("enum argument object must contain only the enum field")
            value = value["enum"]
            if isinstance(value, bool) or not isinstance(value, (str, int)):
                raise BridgeError("enum argument must be a member name or integer value")
        if value is None:
            return "z"
        if isinstance(value, bool):
            return "b1" if value else "b0"
        if isinstance(value, (int, float)):
            return "n" + str(value).encode("ascii").hex()
        return "s" + str(value).encode("utf-8").hex()

    def il2cpp_status(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("IL2CPP_STATUS")

    def il2cpp_dump_file(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("IL2CPP_DUMP_FILE", timeout=300.0)

    def il2cpp_list_images(self, args: dict[str, Any]) -> dict[str, Any]:
        limit = int(args.get("limit", 256))
        if not 1 <= limit <= 5000:
            raise BridgeError("limit must be between 1 and 5000")
        return self._json_call(f"IL2CPP_IMAGES {limit}")

    def il2cpp_list_classes(self, args: dict[str, Any]) -> dict[str, Any]:
        image = self._hex_text(self._required_text(args, "image"))
        namespace_filter = self._hex_text(_single_line(args.get("namespace_filter", ""), "namespace_filter"))
        name_filter = self._hex_text(_single_line(args.get("name_filter", ""), "name_filter"))
        limit = int(args.get("limit", 200))
        if not 1 <= limit <= 5000:
            raise BridgeError("limit must be between 1 and 5000")
        return self._json_call(f"IL2CPP_CLASSES {image} {namespace_filter} {name_filter} {limit}")

    def il2cpp_list_methods(self, args: dict[str, Any]) -> dict[str, Any]:
        image = self._hex_text(self._required_text(args, "image"))
        namespace = self._hex_text(self._required_text(args, "namespace"))
        class_name = self._hex_text(self._required_text(args, "class_name"))
        name_filter = self._hex_text(_single_line(args.get("name_filter", ""), "name_filter"))
        limit = int(args.get("limit", 200))
        if not 1 <= limit <= 5000:
            raise BridgeError("limit must be between 1 and 5000")
        return self._json_call(f"IL2CPP_METHODS {image} {namespace} {class_name} {name_filter} {limit}")

    def il2cpp_find_method(self, args: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("IL2CPP_FIND_METHOD " + " ".join(self._lookup_words(args)))

    def il2cpp_invoke(self, args: dict[str, Any]) -> dict[str, Any]:
        words = self._lookup_words(args)
        instance = self._address(args.get("instance_address", "0x0"), "instance_address")
        values = args.get("arguments", [])
        if not isinstance(values, list) or len(values) > 64:
            raise BridgeError("arguments must be an array with at most 64 values")
        encoded = [self._invoke_token(value) for value in values]
        command = "IL2CPP_INVOKE " + " ".join(words + [instance, str(len(encoded)), *encoded])
        return self._json_call(command, timeout=max(self.config.timeout, 30.0))

    def il2cpp_hook(self, args: dict[str, Any]) -> dict[str, Any]:
        words = self._lookup_words(args)
        replacement = self._address(
            self._required_text(args, "replacement_address"), "replacement_address"
        )
        return self._json_call("IL2CPP_HOOK " + " ".join(words + [replacement]))

    def il2cpp_hook_return(self, args: dict[str, Any]) -> dict[str, Any]:
        words = self._lookup_words(args)
        kind = self._required_text(args, "return_kind")
        value = self._return_value(args)
        return self._json_call("IL2CPP_HOOK_RETURN " + " ".join(words + [kind, value]))

    def il2cpp_unhook(self, args: dict[str, Any]) -> dict[str, Any]:
        method = self.il2cpp_find_method(args)
        address = method.get("address")
        if not isinstance(address, str):
            raise BridgeError("resolved method did not contain an address")
        return self._json_call(f"DOBBY_DESTROY {self._address(address, 'address')}")

    def memory_read(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        raw_size = args.get("size")
        if isinstance(raw_size, bool):
            raise BridgeError("size must be an integer between 1 and 65536")
        try:
            size = int(raw_size)
        except (TypeError, ValueError):
            raise BridgeError("size must be an integer between 1 and 65536") from None
        if not 1 <= size <= 65536:
            raise BridgeError("size must be an integer between 1 and 65536")
        return self._json_call(f"MEMORY_READ {address} {size}")

    def memory_backend_status(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("MEMORY_BACKEND_STATUS")

    def memory_write(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        hex_bytes = self._required_text(args, "hex_bytes").strip()
        if (
            not hex_bytes
            or len(hex_bytes) % 2
            or len(hex_bytes) > 131072
            or any(character not in "0123456789abcdefABCDEF" for character in hex_bytes)
        ):
            raise BridgeError("hex_bytes must contain 1 to 65536 complete bytes")
        return self._json_call(f"MEMORY_WRITE {address} {hex_bytes}")

    @staticmethod
    def _memory_value_format(value_type: Any) -> tuple[str, int]:
        kind = _single_line(value_type, "value_type")
        format_code = MEMORY_VALUE_FORMATS.get(kind)
        if format_code is None:
            raise BridgeError(f"unsupported memory value_type: {kind}")
        return kind, struct.calcsize("<" + format_code)

    @classmethod
    def _decode_memory_value(cls, value_type: Any, hex_bytes: str) -> Any:
        kind, size = cls._memory_value_format(value_type)
        try:
            data = bytes.fromhex(hex_bytes)
        except ValueError:
            raise BridgeError("native bridge returned invalid hexadecimal memory data") from None
        if len(data) != size:
            raise BridgeError(f"native bridge returned {len(data)} bytes for {kind}, expected {size}")
        value = struct.unpack("<" + MEMORY_VALUE_FORMATS[kind], data)[0]
        if kind == "bool":
            return bool(value)
        if kind.startswith("ptr"):
            return f"0x{value:x}"
        return value

    @classmethod
    def _encode_memory_value(cls, value_type: Any, value: Any) -> tuple[str, Any]:
        kind, _ = cls._memory_value_format(value_type)
        if kind == "bool":
            if isinstance(value, bool):
                parsed: Any = value
            elif value in (0, 1, "0", "1"):
                parsed = bool(int(value))
            else:
                raise BridgeError("bool memory value must be true, false, 0, or 1")
        elif kind in {"f32", "f64"}:
            if isinstance(value, bool):
                raise BridgeError(f"{kind} memory value must be numeric")
            try:
                parsed = float(value)
            except (TypeError, ValueError):
                raise BridgeError(f"{kind} memory value must be numeric") from None
            if not math.isfinite(parsed):
                raise BridgeError(f"{kind} memory value must be finite")
        else:
            if isinstance(value, bool):
                raise BridgeError(f"{kind} memory value must be an integer")
            try:
                if isinstance(value, str):
                    parsed = int(value, 0)
                elif isinstance(value, int):
                    parsed = value
                elif isinstance(value, float) and value.is_integer():
                    parsed = int(value)
                else:
                    raise ValueError
            except (TypeError, ValueError):
                raise BridgeError(f"{kind} memory value must be an integer or integer string") from None
        try:
            encoded = struct.pack("<" + MEMORY_VALUE_FORMATS[kind], parsed)
        except (struct.error, OverflowError):
            raise BridgeError(f"memory value is outside the range of {kind}") from None
        return encoded.hex(), cls._decode_memory_value(kind, encoded.hex())

    def memory_read_value(self, args: dict[str, Any]) -> dict[str, Any]:
        kind, size = self._memory_value_format(args.get("value_type"))
        result = self.memory_read({"address": args.get("address"), "size": size})
        hex_bytes = result.get("hex")
        if not isinstance(hex_bytes, str):
            raise BridgeError("native memory read did not return hexadecimal data")
        return {
            **result,
            "value_type": kind,
            "value": self._decode_memory_value(kind, hex_bytes),
        }

    def memory_write_value(self, args: dict[str, Any]) -> dict[str, Any]:
        kind, _ = self._memory_value_format(args.get("value_type"))
        hex_bytes, normalized_value = self._encode_memory_value(kind, args.get("value"))
        result = self.memory_write({"address": args.get("address"), "hex_bytes": hex_bytes})
        previous_hex = result.get("previous_hex")
        if not isinstance(previous_hex, str):
            raise BridgeError("native memory write did not return previous hexadecimal data")
        return {
            **result,
            "value_type": kind,
            "value": normalized_value,
            "previous_value": self._decode_memory_value(kind, previous_hex),
        }

    @staticmethod
    def _bounded_integer(value: Any, name: str, minimum: int, maximum: int) -> int:
        if isinstance(value, bool) or (isinstance(value, float) and not value.is_integer()):
            raise BridgeError(f"{name} must be an integer between {minimum} and {maximum}")
        try:
            parsed = int(value)
        except (TypeError, ValueError):
            raise BridgeError(f"{name} must be an integer between {minimum} and {maximum}") from None
        if not minimum <= parsed <= maximum:
            raise BridgeError(f"{name} must be an integer between {minimum} and {maximum}")
        return parsed

    def memory_list_modules(self, args: dict[str, Any]) -> dict[str, Any]:
        name_filter = _single_line(args.get("name_filter", ""), "name_filter")
        limit = self._bounded_integer(args.get("limit", 256), "limit", 1, 4096)
        return self._json_call(f"MEMORY_MODULES {self._hex_text(name_filter)} {limit}")

    def memory_find_module(self, args: dict[str, Any]) -> dict[str, Any]:
        module_name = self._required_text(args, "module_name")
        if not module_name:
            raise BridgeError("module_name cannot be empty")
        occurrence = self._bounded_integer(args.get("occurrence", 1), "occurrence", 1, 4096)
        return self._json_call(
            f"MEMORY_MODULE_FIND {self._hex_text(module_name)} {occurrence}"
        )

    def memory_address_info(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        return self._json_call(f"MEMORY_ADDRESS_INFO {address}")

    @staticmethod
    def _memory_search_pattern(value: Any) -> str:
        if not isinstance(value, str):
            raise BridgeError("pattern must be a hexadecimal string")
        compact = "".join(value.split())
        if (
            not compact
            or len(compact) % 2
            or len(compact) > 512
            or any(character not in "0123456789abcdefABCDEF?" for character in compact)
        ):
            raise BridgeError("pattern must contain 1 to 256 hexadecimal/wildcard bytes")
        return compact

    def _memory_search_range(self, args: dict[str, Any]) -> tuple[str, str]:
        has_module = "module_name" in args
        has_start = "start_address" in args
        has_end = "end_address" in args
        if has_module:
            if has_start or has_end:
                raise BridgeError("module_name cannot be combined with start_address/end_address")
            module = self.memory_find_module(
                {
                    "module_name": args.get("module_name"),
                    "occurrence": args.get("occurrence", 1),
                }
            )
            start = module.get("start")
            end = module.get("end")
            if not isinstance(start, str) or not isinstance(end, str):
                raise BridgeError("module lookup did not return a valid address range")
            return start, end
        if not has_start or not has_end:
            raise BridgeError("provide module_name or both start_address and end_address")
        return (
            self._address(args.get("start_address"), "start_address"),
            self._address(args.get("end_address"), "end_address"),
        )

    @staticmethod
    def _memory_region_types(args: dict[str, Any]) -> str:
        raw = args.get("memory_types")
        if raw is None:
            return "all"
        if not isinstance(raw, list) or not raw:
            raise BridgeError("memory_types must be a non-empty array when provided")
        selected: list[str] = []
        for value in raw:
            if not isinstance(value, str) or value not in MEMORY_REGION_TYPES:
                raise BridgeError(f"unsupported memory type: {value!r}")
            if value not in selected:
                selected.append(value)
        return ",".join(selected)

    def memory_search(self, args: dict[str, Any]) -> dict[str, Any]:
        pattern = self._memory_search_pattern(args.get("pattern"))
        start, end = self._memory_search_range(args)
        max_results = self._bounded_integer(
            args.get("max_results", 1024), "max_results", 1, 10000
        )
        alignment = self._bounded_integer(args.get("alignment", 1), "alignment", 1, 4096)
        memory_types = self._memory_region_types(args)
        return self._json_call(
            f"MEMORY_SEARCH {start} {end} {self._hex_text(pattern)} {max_results} "
            f"{alignment} {self._hex_text(memory_types)}",
            timeout=max(self.config.timeout, 300.0),
        )

    def memory_search_value(self, args: dict[str, Any]) -> dict[str, Any]:
        kind, value_size = self._memory_value_format(args.get("value_type"))
        pattern, normalized_value = self._encode_memory_value(kind, args.get("value"))
        search_args = {
            key: value
            for key, value in args.items()
            if key not in {"value_type", "value"}
        }
        search_args["pattern"] = pattern
        search_args.setdefault("alignment", value_size)
        result = self.memory_search(search_args)
        return {**result, "value_type": kind, "value": normalized_value}

    def memory_search_exact(self, args: dict[str, Any]) -> dict[str, Any]:
        value_types = args.get("value_types")
        if not isinstance(value_types, list) or not value_types:
            raise BridgeError("value_types must be a non-empty array")
        if len(value_types) > len(MEMORY_VALUE_FORMATS):
            raise BridgeError("too many value_types")
        searches: list[dict[str, Any]] = []
        seen: set[str] = set()
        common = {key: value for key, value in args.items() if key not in {"value_types", "value"}}
        for raw_kind in value_types:
            kind, _ = self._memory_value_format(raw_kind)
            if kind in seen:
                continue
            seen.add(kind)
            searches.append(self.memory_search_value({
                **common,
                "value_type": kind,
                "value": args.get("value"),
            }))
        return {"exact": True, "searches": searches, "search_count": len(searches)}

    def memory_search_fuzzy(self, args: dict[str, Any]) -> dict[str, Any]:
        value_size = self._bounded_integer(args.get("value_size"), "value_size", 1, 8)
        if value_size not in {1, 2, 4, 8}:
            raise BridgeError("value_size must be 1, 2, 4, or 8")
        start, end = self._memory_search_range(args)
        max_results = self._bounded_integer(
            args.get("max_results", 10000), "max_results", 1, 10000
        )
        alignment = self._bounded_integer(
            args.get("alignment", value_size), "alignment", 1, 4096
        )
        memory_types = self._memory_region_types(args)
        return self._json_call(
            f"MEMORY_SEARCH_FUZZY {start} {end} {value_size} {max_results} "
            f"{alignment} {self._hex_text(memory_types)}",
            timeout=max(self.config.timeout, 300.0),
        )

    def memory_filter(self, args: dict[str, Any]) -> dict[str, Any]:
        session_id = self._bounded_integer(
            args.get("session_id"), "session_id", 1, (1 << 64) - 1
        )
        mode = _single_line(args.get("mode", ""), "mode")
        allowed = {"equals", "not_equals", "changed", "unchanged", "increased", "decreased"}
        if mode not in allowed:
            raise BridgeError("unsupported memory filter mode")
        if mode in {"equals", "not_equals"}:
            pattern = self._memory_search_pattern(args.get("pattern"))
        else:
            if "pattern" in args:
                raise BridgeError(f"pattern is not used by {mode} filtering")
            pattern = ""
        return self._json_call(
            f"MEMORY_FILTER {session_id} {mode} {self._hex_text(pattern)}",
            timeout=max(self.config.timeout, 60.0),
        )

    def memory_filter_value(self, args: dict[str, Any]) -> dict[str, Any]:
        mode = _single_line(args.get("mode", ""), "mode")
        if mode not in {"equals", "not_equals"}:
            raise BridgeError("typed value filtering supports equals or not_equals")
        kind, _ = self._memory_value_format(args.get("value_type"))
        pattern, normalized_value = self._encode_memory_value(kind, args.get("value"))
        result = self.memory_filter(
            {"session_id": args.get("session_id"), "mode": mode, "pattern": pattern}
        )
        return {**result, "value_type": kind, "value": normalized_value}

    def memory_search_clear(self, args: dict[str, Any]) -> dict[str, Any]:
        session_id = self._bounded_integer(
            args.get("session_id"), "session_id", 1, (1 << 64) - 1
        )
        return self._json_call(f"MEMORY_SEARCH_CLEAR {session_id}")

    def memory_search_results(self, args: dict[str, Any]) -> dict[str, Any]:
        session_id = self._bounded_integer(
            args.get("session_id"), "session_id", 1, (1 << 64) - 1
        )
        offset = self._bounded_integer(args.get("offset", 0), "offset", 0, 10000)
        limit = self._bounded_integer(args.get("limit", 100), "limit", 1, 1000)
        return self._json_call(f"MEMORY_SEARCH_RESULTS {session_id} {offset} {limit}")

    def dobby_version(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("DOBBY_VERSION")

    def dobby_resolve_symbol(self, args: dict[str, Any]) -> dict[str, Any]:
        image = self._hex_text(_single_line(args.get("image", ""), "image"))
        symbol = self._hex_text(self._required_text(args, "symbol"))
        return self._json_call(f"DOBBY_RESOLVE {image} {symbol}")

    def dobby_hook(self, args: dict[str, Any]) -> dict[str, Any]:
        target = self._address(self._required_text(args, "target_address"), "target_address")
        replacement = self._address(self._required_text(args, "replacement_address"), "replacement_address")
        return self._json_call(f"DOBBY_HOOK {target} {replacement}")

    def dobby_hook_return(self, args: dict[str, Any]) -> dict[str, Any]:
        target = self._address(self._required_text(args, "target_address"), "target_address")
        kind = self._required_text(args, "return_kind")
        value = self._return_value(args)
        return self._json_call(f"DOBBY_HOOK_RETURN {target} {kind} {value}")

    def dobby_instrument(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        return self._json_call(f"DOBBY_INSTRUMENT {address}")

    def dobby_trace_get(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        return self._json_call(f"DOBBY_TRACE_GET {address}")

    def dobby_patch_code(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        hex_bytes = self._required_text(args, "hex_bytes").strip()
        if (
            not hex_bytes
            or len(hex_bytes) % 2
            or len(hex_bytes) > 8192
            or any(character not in "0123456789abcdefABCDEF" for character in hex_bytes)
        ):
            raise BridgeError("hex_bytes must contain 1 to 4096 complete bytes")
        return self._json_call(f"DOBBY_PATCH {address} {hex_bytes}")

    def dobby_destroy(self, args: dict[str, Any]) -> dict[str, Any]:
        target = self._address(self._required_text(args, "target_address"), "target_address")
        return self._json_call(f"DOBBY_DESTROY {target}")

    def dobby_list_hooks(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("DOBBY_LIST_HOOKS")

    def debug_help(self, args: dict[str, Any]) -> dict[str, Any]:
        command = _single_line(args.get("command", ""), "command").strip().upper()
        aliases = {
            "MEMORY_SEARCH_FUZZY": "MEMORY_SEARCH_FUZZY",
            "MEMORY_SEARCH_RESULTS": "MEMORY_SEARCH_RESULTS",
            "MEMORY_FILTER": "MEMORY_FILTER",
            "LUA_STATUS": "LUA_STATUS",
            "LUA_EXECUTE": "LUA_EXEC",
            "LUA_LOGS": "LUA_LOGS",
            "LUA_RESET": "LUA_RESET",
            "ASSEMBLY_STATUS": "ASM_STATUS",
            "ASSEMBLY_ASSEMBLE": "ASM_ASSEMBLE",
            "ASSEMBLY_DISASSEMBLE": "ASM_DISASSEMBLE",
            "ASSEMBLY_PATCH": "ASM_PATCH",
            "BREAKPOINT_STATUS": "BREAKPOINT_STATUS",
            "BREAKPOINT_SET": "BREAKPOINT_SET",
            "BREAKPOINT_LIST": "BREAKPOINT_LIST",
            "BREAKPOINT_HITS": "BREAKPOINT_HITS",
            "BREAKPOINT_CLEAR": "BREAKPOINT_CLEAR",
            "BREAKPOINT_CLEAR_ALL": "BREAKPOINT_CLEAR_ALL",
            "RUNTIME_CAPABILITIES": "CAPABILITIES",
            "DEBUG_HELP": "HELP",
        }
        command = aliases.get(command, command)
        return self._json_call("HELP" + (f" {command}" if command else ""))

    def runtime_capabilities(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("CAPABILITIES")

    def lua_status(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("LUA_STATUS")

    def lua_execute(self, args: dict[str, Any]) -> dict[str, Any]:
        if "script" not in args or not isinstance(args["script"], str):
            raise BridgeError("script must be a string")
        script = args["script"]
        if "\x00" in script:
            raise BridgeError("script cannot contain NUL bytes")
        if len(script.encode("utf-8")) > 256 * 1024:
            raise BridgeError("script exceeds 256 KiB")
        timeout = float(args.get("timeout", 30))
        if not 0.1 <= timeout <= 300:
            raise BridgeError("timeout must be between 0.1 and 300 seconds")
        return self._json_call(f"LUA_EXEC {self._hex_text(script)}", timeout=timeout)

    def lua_reset(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("LUA_RESET")

    def lua_logs(self, args: dict[str, Any]) -> dict[str, Any]:
        clear = args.get("clear", True)
        if not isinstance(clear, bool):
            raise BridgeError("clear must be a boolean")
        return self._json_call(f"LUA_LOGS {1 if clear else 0}")

    def assembly_status(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("ASM_STATUS")

    def assembly_assemble(self, args: dict[str, Any]) -> dict[str, Any]:
        instruction = self._required_text(args, "instruction").strip()
        if not instruction or len(instruction.encode("utf-8")) > 1024:
            raise BridgeError("instruction must contain 1 to 1024 UTF-8 bytes")
        return self._json_call(f"ASM_ASSEMBLE {self._hex_text(instruction)}")

    def assembly_disassemble(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        size = self._bounded_integer(args.get("size", 64), "size", 1, 4096)
        maximum = self._bounded_integer(
            args.get("max_instructions", 16), "max_instructions", 1, 256
        )
        return self._json_call(f"ASM_DISASSEMBLE {address} {size} {maximum}")

    def assembly_patch(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        instruction = self._required_text(args, "instruction").strip()
        if not instruction or len(instruction.encode("utf-8")) > 1024:
            raise BridgeError("instruction must contain 1 to 1024 UTF-8 bytes")
        return self._json_call(f"ASM_PATCH {address} {self._hex_text(instruction)}")

    def breakpoint_status(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("BREAKPOINT_STATUS")

    def breakpoint_set(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        kind = self._required_text(args, "type")
        if kind not in {"x", "r", "w", "rw"}:
            raise BridgeError("type must be x, r, w, or rw")
        length = self._bounded_integer(args.get("length", 4), "length", 1, 8)
        if length not in {1, 2, 4, 8}:
            raise BridgeError("length must be 1, 2, 4, or 8")
        return self._json_call(f"BREAKPOINT_SET {address} {kind} {length}")

    def breakpoint_list(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("BREAKPOINT_LIST")

    def breakpoint_hits(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(args.get("address", "0"), "address")
        offset = self._bounded_integer(args.get("offset", 0), "offset", 0, 1000000)
        limit = self._bounded_integer(args.get("limit", 100), "limit", 1, 512)
        return self._json_call(f"BREAKPOINT_HITS {address} {offset} {limit}")

    def breakpoint_clear(self, args: dict[str, Any]) -> dict[str, Any]:
        address = self._address(self._required_text(args, "address"), "address")
        return self._json_call(f"BREAKPOINT_CLEAR {address}")

    def breakpoint_clear_all(self, _: dict[str, Any]) -> dict[str, Any]:
        return self._json_call("BREAKPOINT_CLEAR_ALL")


class McpServer:
    def __init__(self, dispatcher: ToolDispatcher):
        self.dispatcher = dispatcher

    @staticmethod
    def _success(request_id: Any, result: Any) -> dict[str, Any]:
        return {"jsonrpc": "2.0", "id": request_id, "result": result}

    @staticmethod
    def _error(request_id: Any, code: int, message: str, data: Any = None) -> dict[str, Any]:
        error: dict[str, Any] = {"code": code, "message": message}
        if data is not None:
            error["data"] = data
        return {"jsonrpc": "2.0", "id": request_id, "error": error}

    @staticmethod
    def _tool_result(value: dict[str, Any], is_error: bool = False) -> dict[str, Any]:
        text = value.get("error") if is_error else json.dumps(value, ensure_ascii=False, separators=(",", ":"))
        result: dict[str, Any] = {
            "content": [{"type": "text", "text": str(text)}],
            "isError": is_error,
        }
        if not is_error:
            result["structuredContent"] = value
        return result

    def handle(self, message: dict[str, Any]) -> dict[str, Any] | None:
        if not isinstance(message, dict) or message.get("jsonrpc") != "2.0":
            return self._error(message.get("id") if isinstance(message, dict) else None, -32600, "Invalid Request")
        method = message.get("method")
        request_id = message.get("id")
        is_notification = "id" not in message

        if is_notification:
            return None
        if method == "initialize":
            params = message.get("params") or {}
            if not isinstance(params, dict):
                return self._error(request_id, -32602, "Invalid params")
            requested = params.get("protocolVersion")
            protocol = requested if requested in SUPPORTED_PROTOCOLS else LATEST_PROTOCOL
            return self._success(
                request_id,
                {
                    "protocolVersion": protocol,
                    "capabilities": {"tools": {"listChanged": False}},
                    "serverInfo": {
                        "name": SERVER_NAME,
                        "title": "Zygisk Runtime MCP Bridge",
                        "version": SERVER_VERSION,
                        "description": "Controls IL2CPP, native memory, LuaJIT, Dobby, assembly, and breakpoints in an injected Android process.",
                    },
                    "instructions": "Use ping first. The target game must be running; ADB forwarding is automatic by default.",
                },
            )
        if method == "ping":
            return self._success(request_id, {})
        if method == "tools/list":
            return self._success(request_id, {"tools": TOOLS})
        if method == "tools/call":
            params = message.get("params") or {}
            if not isinstance(params, dict):
                return self._error(request_id, -32602, "Invalid params")
            try:
                name = params.get("name", "")
                if not isinstance(name, str):
                    raise BridgeError("tool name must be a string")
                arguments = params.get("arguments") or {}
                value = self.dispatcher.call(name, arguments)
                return self._success(request_id, self._tool_result(value))
            except Exception as exc:
                return self._success(request_id, self._tool_result({"error": str(exc)}, is_error=True))
        return self._error(request_id, -32601, f"Method not found: {method}")

    def run_stdio(self) -> None:
        for raw_line in sys.stdin.buffer:
            if not raw_line.strip():
                continue
            try:
                message = json.loads(raw_line)
            except (json.JSONDecodeError, UnicodeDecodeError) as exc:
                response = self._error(None, -32700, "Parse error", str(exc))
            else:
                response = self.handle(message)
            if response is not None:
                payload = json.dumps(response, ensure_ascii=False, separators=(",", ":"))
                sys.stdout.buffer.write(payload.encode("utf-8") + b"\n")
                sys.stdout.buffer.flush()


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="MCP bridge for Zygisk IL2CPP hooks")
    parser.add_argument("--host", default=os.environ.get("ZYGISK_MCP_HOST", "127.0.0.1"))
    parser.add_argument("--port", type=int, default=int(os.environ.get("ZYGISK_MCP_PORT", "27184")))
    parser.add_argument("--timeout", type=float, default=float(os.environ.get("ZYGISK_MCP_TIMEOUT", "5")))
    parser.add_argument("--adb", default=os.environ.get("ADB", "adb"), help="ADB executable path")
    parser.add_argument("--serial", default=os.environ.get("ANDROID_SERIAL"), help="ADB device serial")
    parser.add_argument("--no-adb-forward", action="store_true", help="Do not create adb forward automatically")
    parser.add_argument(
        "--direct",
        action="store_true",
        help="Run on the target Android machine and connect directly; disables ADB forwarding",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    config = ConnectionConfig(
        host=args.host,
        port=args.port,
        timeout=args.timeout,
        auto_adb_forward=not (args.no_adb_forward or args.direct),
        adb_path=args.adb,
        adb_serial=args.serial,
    )
    try:
        config.validate()
    except BridgeError as exc:
        print(f"Configuration error: {exc}", file=sys.stderr)
        return 2
    McpServer(ToolDispatcher(config)).run_stdio()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
