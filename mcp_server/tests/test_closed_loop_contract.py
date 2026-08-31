"""Probe 失败恢复闭环的 PC/设备字段契约。"""

from __future__ import annotations

import re
from pathlib import Path
from typing import Any

import pytest

from src.umt_mcp import protocol as proto
from src.umt_mcp import tools


@pytest.fixture
def captured_calls(monkeypatch: pytest.MonkeyPatch) -> list[tuple[str, dict[str, Any]]]:
    calls: list[tuple[str, dict[str, Any]]] = []

    def fake_call(cmd: str, payload: dict[str, Any], **_: Any) -> dict[str, Any]:
        calls.append((cmd, payload))
        if cmd in {"SCAN_PATTERN", "SEARCH_MEMORY"}:
            return {"sessionId": "search-1", "mapRevision": "123:abc", "hits": []}
        if cmd in {"SCAN_GNAMES", "SCAN_OBJECTS"}:
            return {"sessionId": "candidate-1", "mapRevision": "123:abc", "candidates": []}
        if cmd == "DUMP_UNREAL_LIBRARY":
            return {"jobId": "job-1", "started": True}
        if cmd == "GET_DUMP_STATUS":
            return {"jobs": [{"jobId": "job-2", "type": "find_references",
                               "status": "succeeded",
                               "result": {"sessionId": "references-2",
                                          "mapRevision": "123:def"}}]}
        return {"ok": True}

    monkeypatch.setattr(tools.br, "call", fake_call)
    tools._RUNTIME_STATE.update(pid=None, processStartTime=None, mapRevision=None,
                                searchSessions=[], candidateSessions=[], jobs=[])
    return calls


def test_tool_budget_and_new_commands() -> None:
    assert len(tools.TOOLS) == 47
    assert len(tools.DEVICE_COMMANDS) == 47
    assert {"SEARCH_MEMORY", "FIND_REFERENCES"} <= tools.DEVICE_COMMANDS
    assert tools.self_check() == []


def test_python_command_set_matches_device_registry() -> None:
    repo_root = Path(__file__).resolve().parents[2]
    device_source = (
        repo_root / "Alltear_UnrealMemoryToolsNewMcp" / "src" / "executable.cpp"
    ).read_text(encoding="utf-8")
    registered = frozenset(re.findall(r'Register\(\s*"([A-Z_]+)"', device_source))
    assert registered == tools.DEVICE_COMMANDS


def test_scan_pattern_range_and_paging_are_forwarded(captured_calls) -> None:
    tools.scan_pattern(
        pattern="7f 45 4c 46",
        start="0x1000",
        end="0x2000",
        map_ids=None,
        segment_permissions=["r-x"],
        max_results=25,
    )
    cmd, payload = captured_calls[-1]
    assert cmd == "SCAN_PATTERN"
    assert payload["start"] == "0x1000"
    assert payload["end"] == "0x2000"
    assert payload["segmentPermissions"] == ["r-x"]
    assert "mapIds" not in payload

    tools.scan_pattern(pattern="AA BB", module="libUE4.so", async_mode=True)
    assert captured_calls[-1] == (
        "SCAN_PATTERN",
        {"pattern": "AA BB", "module": "libUE4.so", "async": True},
    )


def test_search_and_reference_parameters_are_forwarded(captured_calls) -> None:
    tools.search_memory(
        query_type="POINTER",
        query="0x71000000",
        module="libUE4.so",
        permissions=["rw-"],
        context_before=8,
        context_after=16,
    )
    assert captured_calls[-1] == (
        "SEARCH_MEMORY",
        {
            "queryType": "POINTER",
            "query": "0x71000000",
            "module": "libUE4.so",
            "permissions": ["rw-"],
            "contextBefore": 8,
            "contextAfter": 16,
        },
    )

    tools.find_references(
        target="0x71000000",
        module="libUE4.so",
        kinds=["POINTER", "ADRP_ADD", "ADRP_LDR", "LITERAL_LOAD"],
        include_disassembly=True,
    )
    cmd, payload = captured_calls[-1]
    assert cmd == "FIND_REFERENCES"
    assert payload["kinds"] == ["POINTER", "ADRP_ADD", "ADRP_LDR", "LITERAL_LOAD"]
    assert payload["includeDisassembly"] is True

    tools.find_references(session_id="references-1", cursor="20", max_results=20)
    assert captured_calls[-1] == (
        "FIND_REFERENCES",
        {"sessionId": "references-1", "cursor": "20", "maxResults": 20},
    )


def test_module_and_candidate_paging_are_forwarded(captured_calls) -> None:
    tools.list_modules(include_anonymous=True, cursor="20",
                       anonymous_cursor="40", limit=20)
    assert captured_calls[-1] == (
        "LIST_MODULES",
        {"includeAnonymous": True, "cursor": "20",
         "anonymousCursor": "40", "limit": 20},
    )

    tools.scan_gnames(session_id="names-1", cursor="20", limit=20)
    assert captured_calls[-1] == (
        "SCAN_GNAMES",
        {"sessionId": "names-1", "cursor": "20", "limit": 20},
    )

    tools.scan_objects(session_id="objects-1", cursor="20", limit=20)
    assert captured_calls[-1] == (
        "SCAN_OBJECTS",
        {"sessionId": "objects-1", "cursor": "20", "limit": 20},
    )

    tools.locate_engine_globals(async_mode=True)
    assert captured_calls[-1] == ("LOCATE_ENGINE_GLOBALS", {"async": True})


def test_candidate_mode_and_structured_override_are_forwarded(captured_calls) -> None:
    name_layout = {
        "kind": "FNamePool", "stride": 2, "blocksBit": 16,
        "blocksOff": 64, "headerOff": 0, "lengthShift": 6,
    }
    tools.scan_gnames(
        source="CANDIDATE",
        region="ELF_SEGMENTS",
        anchor_names=["None", "ByteProperty"],
        layouts=[name_layout],
    )
    cmd, payload = captured_calls[-1]
    assert cmd == "SCAN_GNAMES"
    assert payload["source"] == "CANDIDATE"
    assert payload["layouts"] == [name_layout]

    override = {
        "namesPtr": {
            "address": "0x71001000",
            "semantics": "POOL_BASE",
            "layout": name_layout,
            "sessionId": "candidate-1",
            "candidateId": 0,
        }
    }
    tools.apply_probe_overrides(override, pid=123, process_start_time="456",
                                map_revision="123:abc")
    cmd, payload = captured_calls[-1]
    assert cmd == "APPLY_PROBE_OVERRIDES"
    assert payload == {"overrides": override, "pid": 123,
                       "processStartTime": "456", "mapRevision": "123:abc"}


def test_runtime_state_tracks_revision_sessions_jobs_and_resets(captured_calls) -> None:
    tools.search_memory(query_type="ASCII", query="None", module="libUE4.so")
    tools.scan_gnames(source="CANDIDATE", region="ELF_SEGMENTS")
    tools.dump_unreal_library(source="MEMORY")
    state = tools.runtime_state()
    assert state == {
        "pid": None,
        "processStartTime": None,
        "mapRevision": "123:abc",
        "searchSessions": ["search-1"],
        "candidateSessions": ["candidate-1"],
        "jobs": ["job-1"],
    }

    tools.select_process(package="com.example.game")
    assert tools.runtime_state() == {
        "pid": None,
        "processStartTime": None,
        "mapRevision": None,
        "searchSessions": [],
        "candidateSessions": [],
        "jobs": [],
    }


def test_runtime_state_observes_completed_async_job_result(captured_calls) -> None:
    tools.get_dump_status()
    state = tools.runtime_state()
    assert state["mapRevision"] == "123:def"
    assert state["searchSessions"] == ["references-2"]


@pytest.mark.parametrize("code", [proto.E_MAP_STALE, proto.E_SESSION_STALE])
def test_stale_errors_remain_execution_layer(code: str) -> None:
    with pytest.raises(proto.UmtExecutionError) as exc:
        proto.raise_for_error(code, "stale")
    assert exc.value.code == code
