#!/usr/bin/env python3
"""从 AndUEDumper 的 game profile 中抽取 IDA pattern，按 UE 版本归类去重。

产出：
  1) ue_pattern_library.json  —— 机器可读，供 scanPattern 使用
  2) ue_pattern_library.md    —— 人可读的报告

用法：
  python extract_patterns.py <AndUEDumper UEGameProfiles 目录> <输出目录>
"""
from __future__ import annotations

import json
import re
import sys
from collections import defaultdict
from pathlib import Path

PROFILE_RE = re.compile(r"class\s+(\w+Profile)\s*:\s*public\s+IGameProfile")

# UE_DefaultOffsets::UE4_25_27 / UE5_03 ...
VERSION_RE = re.compile(r"UE_DefaultOffsets::(UE[45]_[0-9_]+)")

# {"91 E1 03 ? AA", -7},  或带注释
PATTERN_RE = re.compile(
    r'\{\s*"([0-9A-Fa-f?\s]+)"\s*,\s*(-?0x[0-9A-Fa-f]+|-?\d+)\s*\}'
)

# 捕获某个函数体内的 pattern 块
FUNC_RE = re.compile(
    r"uintptr_t\s+(Get\w+)\s*\(\s*\)\s*const\s*override\s*\{(.*?)\n\s*\}",
    re.DOTALL,
)

# 第四种策略：符号定位（部分 UE 版本/发行保留了符号）
SYMBOL_RE = re.compile(r'findSymbol\(\s*"([^"]+)"\s*\)')
DECODE_ADRL_RE = re.compile(r"DecodeADRL")

APPID_RE = re.compile(r"GetAppIDs\s*\(\s*\)\s*const\s*override\s*\{(.*?)\}", re.DOTALL)
STR_RE = re.compile(r'"([^"]+)"')


def parse_step(raw: str) -> int:
    raw = raw.strip()
    try:
        return int(raw, 0)
    except ValueError:
        return 0


def norm_pattern(p: str) -> str:
    """归一化：大写、单空格分隔。"""
    return " ".join(p.upper().split())


def parse_profile(path: Path) -> dict | None:
    text = path.read_text(encoding="utf-8", errors="ignore")

    m = PROFILE_RE.search(text)
    cls = m.group(1) if m else path.stem

    vm = VERSION_RE.search(text)
    version = vm.group(1) if vm else "UNKNOWN"

    appids: list[str] = []
    am = APPID_RE.search(text)
    if am:
        body = am.group(1)
        # 去掉 return / { } ; 等噪声
        appids = [s for s in STR_RE.findall(body) if "." in s]

    entries: dict[str, list[dict]] = {}
    symbols: dict[str, list[dict]] = {}
    for fm in FUNC_RE.finditer(text):
        fname = fm.group(1)
        body = fm.group(2)

        # 策略四：符号定位
        syms = []
        seen_sym = set()
        for sm in SYMBOL_RE.finditer(body):
            name = sm.group(1)
            if name in seen_sym:
                continue
            seen_sym.add(name)
            syms.append(
                {
                    "symbol": name,
                    "decodeAdrl": bool(DECODE_ADRL_RE.search(body)),
                }
            )
        if syms:
            symbols[fname] = syms

        found = []
        seen = set()
        for pm in PATTERN_RE.finditer(body):
            pat = norm_pattern(pm.group(1))
            step = parse_step(pm.group(2))
            key = (pat, step)
            if key in seen:
                continue
            seen.add(key)
            found.append({"pattern": pat, "step": step})
        if found:
            entries[fname] = found

    if not entries and not symbols:
        return None

    return {
        "file": path.name,
        "class": cls,
        "ueVersion": version,
        "appIds": appids,
        "patterns": entries,
        "symbols": symbols,
    }


def main() -> int:
    if len(sys.argv) < 3:
        print(__doc__)
        return 2

    src = Path(sys.argv[1])
    out = Path(sys.argv[2])
    out.mkdir(parents=True, exist_ok=True)

    profiles = []
    for f in sorted(src.glob("*.hpp")):
        p = parse_profile(f)
        if p:
            profiles.append(p)

    # ---- 按版本聚合 + 全局去重 ----
    by_version: dict[str, dict[str, list[dict]]] = defaultdict(
        lambda: defaultdict(list)
    )
    for prof in profiles:
        ver = prof["ueVersion"]
        for func, items in prof["patterns"].items():
            for it in items:
                by_version[ver][func].append(
                    {
                        "pattern": it["pattern"],
                        "step": it["step"],
                        "from": prof["file"].replace(".hpp", ""),
                    }
                )

    # 每个版本内去重：同一 (pattern, step) 只留一条，但记录来源数
    library: dict[str, dict[str, list[dict]]] = {}
    stats: list[dict] = []
    for ver in sorted(by_version):
        library[ver] = {}
        ver_total = 0
        ver_raw = 0
        for func in sorted(by_version[ver]):
            merged: dict[tuple, dict] = {}
            for it in by_version[ver][func]:
                ver_raw += 1
                key = (it["pattern"], it["step"])
                if key not in merged:
                    merged[key] = {
                        "pattern": it["pattern"],
                        "step": it["step"],
                        "sources": [],
                    }
                if it["from"] not in merged[key]["sources"]:
                    merged[key]["sources"].append(it["from"])
            lst = sorted(
                merged.values(),
                key=lambda x: (-len(x["sources"]), x["pattern"]),
            )
            library[ver][func] = lst
            ver_total += len(lst)
        games = sorted({p["file"].replace(".hpp", "") for p in profiles
                        if p["ueVersion"] == ver})
        stats.append(
            {
                "ueVersion": ver,
                "games": games,
                "gameCount": len(games),
                "rawPatterns": ver_raw,
                "dedupedPatterns": ver_total,
            }
        )

    # ---- 符号策略聚合 ----
    sym_library: dict[str, list[dict]] = defaultdict(list)
    for prof in profiles:
        ver = prof["ueVersion"]
        for func, items in prof.get("symbols", {}).items():
            for it in items:
                sym_library[ver].append(
                    {
                        "symbol": it["symbol"],
                        "function": func,
                        "decodeAdrl": it["decodeAdrl"],
                        "from": prof["file"].replace(".hpp", ""),
                    }
                )
    sym_library = {k: v for k, v in sorted(sym_library.items())}

    (out / "ue_pattern_library.json").write_text(
        json.dumps(
            {"generatedFrom": str(src), "profiles": profiles,
             "statistics": stats, "library": library,
             "symbolStrategies": sym_library},
            ensure_ascii=False,
            indent=2,
        ),
        encoding="utf-8",
    )

    # ---- 人可读报告 ----
    lines = [
        "# UE 引擎函数 pattern 库",
        "",
        f"从 AndUEDumper 的 {len(profiles)} 个 game profile 中自动抽取。",
        "",
        "> **这些 pattern 不是「游戏特征码」，是「UE 引擎函数序言」。**",
        "> 其字节序列由「UE 版本 + 编译器」决定，**跨游戏可迁移**。",
        "",
        "## 一、按版本统计",
        "",
        "| UE 版本 | 游戏数 | 原始 pattern | 去重后 |",
        "|---|---|---|---|",
    ]
    for s in stats:
        lines.append(
            f"| **{s['ueVersion']}** | {s['gameCount']} | "
            f"{s['rawPatterns']} | **{s['dedupedPatterns']}** |"
        )

    lines += ["", "## 二、各版本游戏来源", ""]
    for s in stats:
        lines.append(f"- **{s['ueVersion']}**（{s['gameCount']}）："
                     f"{', '.join(s['games'])}")

    if sym_library:
        lines += [
            "",
            "## 二·五、策略四：符号定位（Symbol-based）",
            "",
            "> 部分 UE 发行版保留了符号，可直接解析，**比 pattern 更快更稳**。",
            "> 这是 pattern 快路径之外的第四条路，优先级应高于 pattern。",
            "",
            "| UE 版本 | 符号名 | 用途 | 需 DecodeADRL | 来源 |",
            "|---|---|---|---|---|",
        ]
        for ver in sorted(sym_library):
            for it in sym_library[ver]:
                kind = ("GUObjectArray" if "GUObject" in it["function"]
                        else "GNames / 名字表")
                lines.append(
                    f"| {ver} | `{it['symbol']}` | {kind} | "
                    f"{'是' if it['decodeAdrl'] else '否（直接即地址）'} | "
                    f"{it['from']} |"
                )

    lines += ["", "## 三、Pattern 明细", ""]
    for ver in sorted(library):
        lines.append(f"### {ver}")
        lines.append("")
        for func in sorted(library[ver]):
            lst = library[ver][func]
            kind = "GUObjectArray" if "GUObject" in func else "GNames / 名字表"
            lines.append(f"#### `{func}` — {kind}（{len(lst)} 条）")
            lines.append("")
            lines.append("| # | Pattern | step | 命中来源 |")
            lines.append("|---|---|---|---|")
            for i, it in enumerate(lst, 1):
                srcs = ", ".join(it["sources"])
                if len(srcs) > 60:
                    srcs = srcs[:57] + "..."
                lines.append(
                    f"| {i} | `{it['pattern']}` | {it['step']} | {srcs} |"
                )
            lines.append("")

    (out / "ue_pattern_library.md").write_text(
        "\n".join(lines), encoding="utf-8"
    )

    print(f"profiles parsed : {len(profiles)}")
    for s in stats:
        print(f"  {s['ueVersion']:<12} games={s['gameCount']:<3} "
              f"raw={s['rawPatterns']:<4} dedup={s['dedupedPatterns']}")
    print(f"\n-> {out / 'ue_pattern_library.json'}")
    print(f"-> {out / 'ue_pattern_library.md'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
