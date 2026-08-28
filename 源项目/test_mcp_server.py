from __future__ import annotations

import json
import socket
import threading
import unittest

from mcp.mcp_server import BridgeError, ConnectionConfig, HookSocketClient, McpServer, ToolDispatcher


class OneShotHookServer:
    def __init__(self, response: str):
        self.response = response
        self.command = ""
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind(("127.0.0.1", 0))
        self.socket.listen(1)
        self.port = self.socket.getsockname()[1]
        self.thread = threading.Thread(target=self._serve, daemon=True)

    def _serve(self) -> None:
        connection, _ = self.socket.accept()
        with connection:
            data = bytearray()
            while not data.endswith(b"\n"):
                data.extend(connection.recv(1024))
            self.command = data.decode("utf-8").rstrip("\r\n")
            body = self.response.encode("utf-8")
            connection.sendall(f"OK {len(body)}\n".encode("ascii") + body)
        self.socket.close()

    def __enter__(self) -> "OneShotHookServer":
        self.thread.start()
        return self

    def __exit__(self, *_: object) -> None:
        self.thread.join(timeout=2)


class HookSocketClientTests(unittest.TestCase):
    def test_command_protocol(self) -> None:
        with OneShotHookServer("PONG\n") as server:
            config = ConnectionConfig(port=server.port, auto_adb_forward=False)
            response = HookSocketClient(config).call("PING")
        self.assertEqual("PING", server.command)
        self.assertEqual("PONG", response)


class McpServerTests(unittest.TestCase):
    def setUp(self) -> None:
        config = ConnectionConfig(auto_adb_forward=False)
        self.server = McpServer(ToolDispatcher(config))

    def test_initialize_negotiates_latest_protocol(self) -> None:
        response = self.server.handle(
            {
                "jsonrpc": "2.0",
                "id": 1,
                "method": "initialize",
                "params": {"protocolVersion": "unknown"},
            }
        )
        self.assertEqual("2025-11-25", response["result"]["protocolVersion"])

    def test_lists_runtime_hook_tools(self) -> None:
        response = self.server.handle({"jsonrpc": "2.0", "id": 2, "method": "tools/list", "params": {}})
        names = {tool["name"] for tool in response["result"]["tools"]}
        self.assertIn("raw_hook_call", names)
        self.assertIn("il2cpp_hook", names)
        self.assertIn("il2cpp_hook_return", names)
        self.assertIn("il2cpp_dump_file", names)
        self.assertIn("memory_read", names)
        self.assertIn("memory_write", names)
        self.assertIn("memory_read_value", names)
        self.assertIn("memory_write_value", names)
        self.assertIn("dobby_hook", names)
        self.assertIn("dobby_patch_code", names)
        self.assertIn("mcp_toast_set_enabled", names)
        self.assertIn("mcp_toast_show", names)

        invoke_tool = next(tool for tool in response["result"]["tools"] if tool["name"] == "il2cpp_invoke")
        argument_variants = invoke_tool["inputSchema"]["properties"]["arguments"]["items"]["anyOf"]
        self.assertTrue(any(variant.get("type") == "object" for variant in argument_variants))

    def test_encodes_explicit_enum_arguments(self) -> None:
        member = "RoleSyncState.Walking"
        self.assertEqual("s" + member.encode("utf-8").hex(), ToolDispatcher._invoke_token({"enum": member}))
        self.assertEqual("n31", ToolDispatcher._invoke_token({"enum": 1}))

        with self.assertRaises(BridgeError):
            ToolDispatcher._invoke_token({"enum": 1.5})
        with self.assertRaises(BridgeError):
            ToolDispatcher._invoke_token({"enum": 1, "type": "RoleSyncState"})

    def test_memory_read_and_write_commands(self) -> None:
        with OneShotHookServer('{"address":"0x1000","size":2,"hex":"01ff"}') as server:
            dispatcher = ToolDispatcher(ConnectionConfig(port=server.port, auto_adb_forward=False))
            result = dispatcher.memory_read({"address": "0x1000", "size": 2})
        self.assertEqual("MEMORY_READ 0x1000 2", server.command)
        self.assertEqual("01ff", result["hex"])

        response = '{"written":true,"verified":true,"address":"0x1000","size":2,"previous_hex":"0000"}'
        with OneShotHookServer(response) as server:
            dispatcher = ToolDispatcher(ConnectionConfig(port=server.port, auto_adb_forward=False))
            result = dispatcher.memory_write({"address": "0x1000", "hex_bytes": "01FF"})
        self.assertEqual("MEMORY_WRITE 0x1000 01FF", server.command)
        self.assertTrue(result["verified"])
        self.assertEqual("0000", result["previous_hex"])

        dispatcher = ToolDispatcher(ConnectionConfig(auto_adb_forward=False))
        with self.assertRaises(BridgeError):
            dispatcher.memory_read({"address": "0x1000", "size": 0})
        with self.assertRaises(BridgeError):
            dispatcher.memory_write({"address": "0x1000", "hex_bytes": "123"})

    def test_typed_memory_values(self) -> None:
        encoded, normalized = ToolDispatcher._encode_memory_value("i32", "0x12345678")
        self.assertEqual("78563412", encoded)
        self.assertEqual(0x12345678, normalized)
        self.assertEqual("0x12345678", ToolDispatcher._decode_memory_value("ptr32", encoded))

        with OneShotHookServer('{"address":"0x2000","size":4,"hex":"78563412"}') as server:
            dispatcher = ToolDispatcher(ConnectionConfig(port=server.port, auto_adb_forward=False))
            result = dispatcher.memory_read_value(
                {"address": "0x2000", "value_type": "i32"}
            )
        self.assertEqual("MEMORY_READ 0x2000 4", server.command)
        self.assertEqual(0x12345678, result["value"])

        response = '{"written":true,"verified":true,"address":"0x2000","size":4,"previous_hex":"00000000"}'
        with OneShotHookServer(response) as server:
            dispatcher = ToolDispatcher(ConnectionConfig(port=server.port, auto_adb_forward=False))
            result = dispatcher.memory_write_value(
                {"address": "0x2000", "value_type": "f32", "value": 1.5}
            )
        self.assertEqual("MEMORY_WRITE 0x2000 0000c03f", server.command)
        self.assertEqual(0.0, result["previous_value"])

        with self.assertRaises(BridgeError):
            ToolDispatcher._encode_memory_value("u8", 256)

    def test_connection_info_has_structured_content(self) -> None:
        response = self.server.handle(
            {
                "jsonrpc": "2.0",
                "id": 3,
                "method": "tools/call",
                "params": {"name": "connection_info", "arguments": {}},
            }
        )
        result = response["result"]
        self.assertFalse(result["isError"])
        self.assertEqual(27184, result["structuredContent"]["port"])
        json.loads(result["content"][0]["text"])


if __name__ == "__main__":
    unittest.main()
