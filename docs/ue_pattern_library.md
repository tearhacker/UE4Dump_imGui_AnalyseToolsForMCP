# UE 引擎函数 pattern 库

从 AndUEDumper 的 28 个 game profile 中自动抽取。

> **这些 pattern 不是「游戏特征码」，是「UE 引擎函数序言」。**
> 其字节序列由「UE 版本 + 编译器」决定，**跨游戏可迁移**。

## 一、按版本统计

| UE 版本 | 游戏数 | 原始 pattern | 去重后 |
|---|---|---|---|
| **UE4_23_24** | 3 | 18 | **6** |
| **UE4_25_27** | 17 | 130 | **18** |
| **UE5_00_02** | 3 | 34 | **14** |
| **UE5_03** | 2 | 20 | **14** |

## 二、各版本游戏来源

- **UE4_23_24**（3）：Dislyte, OdinValhalla, RealBoxing2
- **UE4_25_27**（17）：ArenaBreakout, ArkUltimate, Auroria, BlackClover, Case2, Century, DeltaForce, Farlight, HelloNeighbor, Injustice2, LineageW, MortalKombat, PES, RLSideswipe, Torchlight, TowerFantasy, WutheringWaves
- **UE5_00_02**（3）：HelloNeighborND, KingArthur, NightCrows
- **UE5_03**（2）：BabyYellow, RooftopsParkour

## 二·五、策略四：符号定位（Symbol-based）

> 部分 UE 发行版保留了符号，可直接解析，**比 pattern 更快更稳**。
> 这是 pattern 快路径之外的第四条路，优先级应高于 pattern。

| UE 版本 | 符号名 | 用途 | 需 DecodeADRL | 来源 |
|---|---|---|---|---|
| UE4_00_17 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | Lineage2 |
| UE4_00_17 | `_ZN5FName16GetIsInitializedEv` | GNames / 名字表 | 否（直接即地址） | Lineage2 |
| UE4_20 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | BladeSoul |
| UE4_20 | `_ZN5FName16GetIsInitializedEv` | GNames / 名字表 | 否（直接即地址） | BladeSoul |
| UE4_22 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | SFG2 |
| UE4_22 | `GFNameTableForDebuggerVisualizers_MT` | GNames / 名字表 | 否（直接即地址） | SFG2 |
| UE4_23_24 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | Dislyte |
| UE4_23_24 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | Dislyte |
| UE4_23_24 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | OdinValhalla |
| UE4_23_24 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | OdinValhalla |
| UE4_23_24 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | RealBoxing2 |
| UE4_23_24 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | RealBoxing2 |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | ArkUltimate |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | ArkUltimate |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | Auroria |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | Auroria |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | BlackClover |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | BlackClover |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | Case2 |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | Case2 |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | Century |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | Century |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | HelloNeighbor |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | HelloNeighbor |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | Injustice2 |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | Injustice2 |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | LineageW |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | LineageW |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | MortalKombat |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | MortalKombat |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | TowerFantasy |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | TowerFantasy |
| UE4_25_27 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | WutheringWaves |
| UE4_25_27 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | WutheringWaves |
| UE5_00_02 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | HelloNeighborND |
| UE5_00_02 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | HelloNeighborND |
| UE5_03 | `GUObjectArray` | GUObjectArray | 否（直接即地址） | BabyYellow |
| UE5_03 | `GNameBlocksDebug` | GNames / 名字表 | 否（直接即地址） | BabyYellow |

## 三、Pattern 明细

### UE4_23_24

#### `GetGUObjectArrayPtr` — GUObjectArray（6 条）

| # | Pattern | step | 命中来源 |
|---|---|---|---|
| 1 | `69 3E 40 B9 1F 01 09 6B ? ? ? 54 E1 03 13 AA ? ? ? ? F4 4F ? A9 ? ? ? ? ? ? ? 91` | 24 | Dislyte, OdinValhalla, RealBoxing2 |
| 2 | `91 E1 03 ? AA E0 03 08 AA E2 03 1F 2A` | -7 | Dislyte, OdinValhalla, RealBoxing2 |
| 3 | `96 DF 02 17 ? ? ? ? 54 ? ? ? ? ? ? ? 91 E1 03 13 AA` | 9 | Dislyte, OdinValhalla, RealBoxing2 |
| 4 | `9F E5 00 ? 00 E3 FF ? 40 E3 ? ? A0 E1` | -2 | Dislyte, OdinValhalla, RealBoxing2 |
| 5 | `B4 21 0C 40 B9 ? ? ? ? ? ? ? 91` | 5 | Dislyte, OdinValhalla, RealBoxing2 |
| 6 | `F4 03 01 2A ? 00 00 34 ? ? ? ? ? ? ? ? ? ? 00 54 ? 00 00 14 ? ? ? ? ? ? ? 91` | 24 | Dislyte, OdinValhalla, RealBoxing2 |

### UE4_25_27

#### `GetGUObjectArrayPtr` — GUObjectArray（10 条）

| # | Pattern | step | 命中来源 |
|---|---|---|---|
| 1 | `69 3E 40 B9 1F 01 09 6B ? ? ? 54 E1 03 13 AA ? ? ? ? F4 4F ? A9 ? ? ? ? ? ? ? 91` | 24 | ArenaBreakout, ArkUltimate, Auroria, BlackClover, Case2, ... |
| 2 | `96 DF 02 17 ? ? ? ? 54 ? ? ? ? ? ? ? 91 E1 03 13 AA` | 9 | ArenaBreakout, ArkUltimate, Auroria, BlackClover, Case2, ... |
| 3 | `9F E5 00 ? 00 E3 FF ? 40 E3 ? ? A0 E1` | -2 | ArenaBreakout, ArkUltimate, Auroria, BlackClover, Case2, ... |
| 4 | `B4 21 0C 40 B9 ? ? ? ? ? ? ? 91` | 5 | ArenaBreakout, ArkUltimate, Auroria, BlackClover, Case2, ... |
| 5 | `F4 03 01 2A ? 00 00 34 ? ? ? ? ? ? ? ? ? ? 00 54 ? 00 00 14 ? ? ? ? ? ? ? 91` | 24 | ArenaBreakout, ArkUltimate, Auroria, BlackClover, Case2, ... |
| 6 | `91 E1 03 ? AA E0 03 08 AA E2 03 1F 2A` | -7 | ArenaBreakout, ArkUltimate, Auroria, BlackClover, Case2, ... |
| 7 | `91 E1 03 14 AA ? ? ? 95 ? ? ? 36 ? ? ? B9` | -7 | DeltaForce |
| 8 | `91 E1 03 15 AA ? ? ? 95 ? ? ? 36 ? ? ? B9 ? ? ? 52 ? ? ? B0 ? ? ? F9 09 01 09 0B ? ? ? 71 ? ? ? 1A ? ? ? 13 ? ? ? 12 08 01 09 4B ? ? ? 52 ? ? ? F8 1F 20 03 D5 08 29 29 9B ? ? ? B9 ? ? ? 72 ? ? ? 54 ? ? ? F9` | -7 | DeltaForce |
| 9 | `91 F6 03 01 AA E1 03 14 AA ? ? ? 39` | -7 | DeltaForce |
| 10 | `? ? ? 90 ? ? ? 52 09 01 09 0B ? ? ? 71 ? ? ? F9 ? ? ? 1A ? ? ? 13 ? ? ? 12 08 01 09 4B ? ? ? 52 ? ? ? F9 ? ? ? F8 1F 20 03 D5 08 29 29 9B ? ? ? 39 ? ? ? 37 F8 03 00 AA` | 0 | ArenaBreakout |

#### `GetNamesPtr` — GNames / 名字表（8 条）

| # | Pattern | step | 命中来源 |
|---|---|---|---|
| 1 | `02 ? 91 C8 00 00 37 ? ? ? ? ? ? ? 91` | 7 | Farlight, PES, RLSideswipe, Torchlight |
| 2 | `39 C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97 ? 00 80 52 ? ? ? 39` | 5 | Farlight, PES, RLSideswipe, Torchlight |
| 3 | `C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97` | 4 | Farlight, PES, RLSideswipe, Torchlight |
| 4 | `C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97 ? 00 80 52` | 4 | Farlight, PES, RLSideswipe, Torchlight |
| 5 | `F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ? ? ? ? ? ? ? ? A8 02 ? 39` | 24 | Farlight, PES, RLSideswipe, Torchlight |
| 6 | `F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ? ? ? ? A8 02 ? 39` | 36 | Farlight, PES, RLSideswipe, Torchlight |
| 7 | `FD 7B 01 A9 FD 43 00 91 ? ? ? ? 89 ? ? 39 F3 03 08 AA C9 00 00 37 ? ? ? ? ? ? ? 91` | 24 | Farlight, PES, RLSideswipe, Torchlight |
| 8 | `F8 C8 ? ? 39 C8 00 00 37 ? ? ? ? ? ? ? 91` | 9 | Farlight, PES, Torchlight |

### UE5_00_02

#### `GetGUObjectArrayPtr` — GUObjectArray（6 条）

| # | Pattern | step | 命中来源 |
|---|---|---|---|
| 1 | `69 3E 40 B9 1F 01 09 6B ? ? ? 54 E1 03 13 AA ? ? ? ? F4 4F ? A9 ? ? ? ? ? ? ? 91` | 24 | HelloNeighborND, KingArthur, NightCrows |
| 2 | `91 E1 03 ? AA E0 03 08 AA E2 03 1F 2A` | -7 | HelloNeighborND, KingArthur, NightCrows |
| 3 | `96 DF 02 17 ? ? ? ? 54 ? ? ? ? ? ? ? 91 E1 03 13 AA` | 9 | HelloNeighborND, KingArthur, NightCrows |
| 4 | `9F E5 00 ? 00 E3 FF ? 40 E3 ? ? A0 E1` | -2 | HelloNeighborND, KingArthur, NightCrows |
| 5 | `B4 21 0C 40 B9 ? ? ? ? ? ? ? 91` | 5 | HelloNeighborND, KingArthur, NightCrows |
| 6 | `F4 03 01 2A ? 00 00 34 ? ? ? ? ? ? ? ? ? ? 00 54 ? 00 00 14 ? ? ? ? ? ? ? 91` | 24 | HelloNeighborND, KingArthur, NightCrows |

#### `GetNamesPtr` — GNames / 名字表（8 条）

| # | Pattern | step | 命中来源 |
|---|---|---|---|
| 1 | `02 ? 91 C8 00 00 37 ? ? ? ? ? ? ? 91` | 7 | KingArthur, NightCrows |
| 2 | `39 C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97 ? 00 80 52 ? ? ? 39` | 5 | KingArthur, NightCrows |
| 3 | `C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97` | 4 | KingArthur, NightCrows |
| 4 | `C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97 ? 00 80 52` | 4 | KingArthur, NightCrows |
| 5 | `F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ? ? ? ? ? ? ? ? A8 02 ? 39` | 24 | KingArthur, NightCrows |
| 6 | `F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ? ? ? ? A8 02 ? 39` | 36 | KingArthur, NightCrows |
| 7 | `F8 C8 ? ? 39 C8 00 00 37 ? ? ? ? ? ? ? 91` | 9 | KingArthur, NightCrows |
| 8 | `FD 7B 01 A9 FD 43 00 91 ? ? ? ? 89 ? ? 39 F3 03 08 AA C9 00 00 37 ? ? ? ? ? ? ? 91` | 24 | KingArthur, NightCrows |

### UE5_03

#### `GetGUObjectArrayPtr` — GUObjectArray（6 条）

| # | Pattern | step | 命中来源 |
|---|---|---|---|
| 1 | `69 3E 40 B9 1F 01 09 6B ? ? ? 54 E1 03 13 AA ? ? ? ? F4 4F ? A9 ? ? ? ? ? ? ? 91` | 24 | BabyYellow, RooftopsParkour |
| 2 | `91 E1 03 ? AA E0 03 08 AA E2 03 1F 2A` | -7 | BabyYellow, RooftopsParkour |
| 3 | `96 DF 02 17 ? ? ? ? 54 ? ? ? ? ? ? ? 91 E1 03 13 AA` | 9 | BabyYellow, RooftopsParkour |
| 4 | `9F E5 00 ? 00 E3 FF ? 40 E3 ? ? A0 E1` | -2 | BabyYellow, RooftopsParkour |
| 5 | `B4 21 0C 40 B9 ? ? ? ? ? ? ? 91` | 5 | BabyYellow, RooftopsParkour |
| 6 | `F4 03 01 2A ? 00 00 34 ? ? ? ? ? ? ? ? ? ? 00 54 ? 00 00 14 ? ? ? ? ? ? ? 91` | 24 | BabyYellow, RooftopsParkour |

#### `GetNamesPtr` — GNames / 名字表（8 条）

| # | Pattern | step | 命中来源 |
|---|---|---|---|
| 1 | `02 ? 91 C8 00 00 37 ? ? ? ? ? ? ? 91` | 7 | RooftopsParkour |
| 2 | `39 C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97 ? 00 80 52 ? ? ? 39` | 5 | RooftopsParkour |
| 3 | `C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97` | 4 | RooftopsParkour |
| 4 | `C8 00 00 37 ? ? ? ? ? ? ? 91 ? ? ? 97 ? 00 80 52` | 4 | RooftopsParkour |
| 5 | `F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ? ? ? ? ? ? ? ? A8 02 ? 39` | 24 | RooftopsParkour |
| 6 | `F4 4F 01 A9 FD 7B 02 A9 FD 83 00 91 ? ? ? ? A8 02 ? 39` | 36 | RooftopsParkour |
| 7 | `F8 C8 ? ? 39 C8 00 00 37 ? ? ? ? ? ? ? 91` | 9 | RooftopsParkour |
| 8 | `FD 7B 01 A9 FD 43 00 91 ? ? ? ? 89 ? ? 39 F3 03 08 AA C9 00 00 37 ? ? ? ? ? ? ? 91` | 24 | RooftopsParkour |
