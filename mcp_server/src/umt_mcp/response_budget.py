"""Deterministic response budgeting shared by tools and tests."""
from __future__ import annotations

import json
import math
from typing import Any

MIN_BUDGET_TOKENS = 128
MAX_BUDGET_TOKENS = 3800
DEFAULT_BUDGET_TOKENS = 800

def estimated_tokens(value: Any) -> int:
    raw = json.dumps(value, ensure_ascii=False, separators=(",", ":")).encode("utf-8")
    return max(1, math.ceil(len(raw) / 4))

def normalize_budget(value: int | None) -> int:
    if value is None:
        return DEFAULT_BUDGET_TOKENS
    return max(MIN_BUDGET_TOKENS, min(MAX_BUDGET_TOKENS, int(value)))

def envelope(data: Any, *, budget_tokens: int | None = None, brief: bool = False,
             fields: list[str] | None = None) -> dict[str, Any]:
    budget = normalize_budget(budget_tokens)
    projected = data
    omitted: list[str] = []
    if fields and isinstance(data, dict):
        projected = {k: data[k] for k in fields if k in data}
        omitted = [k for k in data if k not in projected]
    estimate = estimated_tokens(projected)
    truncated = estimate > budget
    if truncated and isinstance(projected, dict):
        projected = {k: v for k, v in projected.items() if k not in {"description", "content", "raw", "contextHex"}}
        estimate = estimated_tokens(projected)
        omitted.extend(["description", "content", "raw", "contextHex"])
    return {"schemaVersion": "1.1", "status": "ok", "brief": brief,
            "estimatedTokens": estimate, "budgetTokens": budget,
            "truncated": truncated or bool(omitted), "omittedFields": sorted(set(omitted)),
            "nextCursor": projected.get("nextCursor") if isinstance(projected, dict) else None,
            "data": projected}
