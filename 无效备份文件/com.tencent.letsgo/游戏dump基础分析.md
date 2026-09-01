# 元梦之星 Dump 基础分析

> 游戏包名: `com.tencent.letsgo`  
> 引擎: Unreal Engine 4.26  
> SDK 来源: Dumper-7 全量导出, 路径 `4.26.2-0+++UE4+Release-4.26-LetsGo/CppSDK/SDK/`  
> 文档日期: 2026-08-06

---

## 1. 引擎底层指针链

### 1.1 GWorld 与自身实体

整个游戏的世界入口从模块基址偏移读取, 所有功能模块均依赖此指针链。

```
GWorld = [g_ModuleBase + 0x11FCAC30]
PersistentLevel = [GWorld + 0x30]
Actors.TArray = Level + 0x98  (Data: ptr, Num: int32, Max: int32)
```

自身角色实体的完整指针链 (6 级解引用):

```
Base + 0x11FCAC30  →  GWorld
  + 0x1F0          →  OwningGameInstance
    + 0x38          →  LocalPlayers[0]
      + 0x00          →  PlayerController
        + 0x30          →  Pawn
          + 0x300         →  selfEntity (ABaseMoeGameCharacter)
```

关键组件偏移 (相对 selfEntity):

| 组件 | 偏移 | 类型 |
|------|------|------|
| MovementComponent | +0x6F0 | UCharacterMovementComponent |
| CharAttributeComponent | +0x6F8 | UMoeCharAttributeComponent |
| FAttributeFloat.Value | +0x74 (相对属性对象) | float |
| FAttributeBool.Value | +0x51 (相对属性对象) | bool |

### 1.2 GName 原生内存解析

UE4 4.22+ 使用 FNamePool 管理 FName 到字符串的映射。元梦之星的精确参数如下:

| 参数 | 值 | 说明 |
|------|-----|------|
| Stride | 2 | offset 需乘以 Stride |
| BlocksBit | 16 | blockIdx = nameIndex >> 16 |
| BlocksOff | 0x40 | Blocks 数组在 Pool 中的偏移 |
| FNameEntry Header | uint16 | bIsWide:1 \| ProbeHash:5 \| Len:10 |
| 字符串偏移 | +2 | Header 之后 |

解析流程:

```
nameIndex → blockIdx = nameIndex >> 16
          → offset = nameIndex & 0xFFFF
          → block = [NamePool + 0x40 + blockIdx * 8]
          → entryAddr = block + offset * 2
          → hdr = read_uint16(entryAddr)
          → len = hdr >> 6
          → isWide = hdr & 1
          → string = read_bytes(entryAddr + 2, len)
```

校准方法: 读取 GUObjectArray[1] (对象名应为 "Object"), 验证解析结果正确。GUObjectArray 为分块数组, 每块 65536 个元素, FUObjectItem 大小 0x18 字节。

### 1.3 类名读取

```
GetClassName(objectAddr):
  cls = [objectAddr + 0x10]        // ClassPrivate
  nameIndex = read_int32(cls + 0x18)  // NamePrivate.ComparisonIndex
  return ResolveFName(nameIndex)
```

注意: 类名必须从 UClass 自身的 NamePrivate 读取, 不能回退到对象实例的 NamePrivate, 后者是实例名而非类名。

---

## 2. 探灵实录 PVE 模式 (Feature_ChestPVE)

### 2.1 模式概述

探灵实录是元梦之星的 PVE 玩法模式, 对应 SDK 模块 `Feature_ChestPVE`。该模式下所有 BP 类以 `BP_ChestPVE_` 为前缀, 涵盖宝箱、钥匙、道具、怪物、撤离点等游戏对象。

### 2.2 Actor 类名匹配规则

由于地图差异和动态生成, 类名无法精确匹配, 统一采用子串匹配策略:

| 目标 | 类名前缀 | 关键子串 | 排除条件 |
|------|----------|----------|----------|
| 撤离点 | BP_ChestPVE_ | Exit / Elevator / Evacuat / Escap / Extract | - |
| 怪物 | BP_ChestPVE_Monster_ | - | - |
| 道具 | - | Prop | Fishing (钓鱼有独立功能) |
| 物品 | BP_ChestPVE_SupplyItem_ | - | - |
| 宝箱 | BP_ChestPVE_ | BigBox / Box / Chest | - |

---

## 3. 道具系统 (AMoePropActorBase)

### 3.1 类继承关系

```
AActor
  └─ AMoePlaceableActor
       └─ AMoePropActorBase          (MoeGameCore, Size: 0x12D0)
            └─ AMoeAbilityPropBase   (Feature_ChestPVE)
```

### 3.2 核心字段与偏移

| 字段 | 偏移 | 类型 | 属性标记 | 用途 |
|------|------|------|----------|------|
| PropType | 0x0AC8 | EPropType (uint8) | Edit, DisableEditOnInstance | 道具类型枚举 |
| PropTypeId | 0x0ACC | int32 | Edit, DisableEditOnInstance | 道具类型 ID |
| bInfiniteUse | 0x0C21 | bool | Edit, DisableEditOnInstance | 无限使用 |
| UseTimes | 0x0C24 | int32 | Edit, DisableEditOnInstance | 使用次数 |
| InitialCD | 0x0C2C | float | Edit, DisableEditOnInstance | 初始冷却 |
| UseCoolTime | 0x0C34 | float | Edit, DisableEditOnInstance | 使用冷却时间 |
| InitEnergy | 0x0C3C | float | Edit, DisableEditOnInstance | 初始能量 |
| UseEnergyCost | 0x0C40 | float | Edit, DisableEditOnInstance | 单次使用能量消耗 |
| EnergyAddPerSecond | 0x0C44 | float | Edit, DisableEditOnInstance | 每秒能量回复 |
| MaxEnergyStackNum | 0x0C48 | int32 | Edit, DisableEditOnInstance | 最大能量层数 |
| ChangeOtherHPValue | 0x0C8C | float | Edit, DisableEditOnInstance | 对他人 HP 修改值 (伤害) |
| ExistTime | 0x0C94 | float | Edit, BlueprintVisible | 存在时间 |
| DestroyAfterHit | 0x0C98 | bool | Edit, BlueprintVisible | 命中后销毁 |
| bShouldAffectSelf | 0x0C99 | bool | Edit | 影响自身 |
| bShouldAffectTeammate | 0x0C9A | bool | Edit | 影响队友 |
| UseEffects | 0x0EF8 | TArray\<UMoeEffect*\> | Edit, ExportObject | 使用时触发的效果列表 |
| PropTrigger | 0x1040 | USphereComponent* | Edit, ExportObject, Protected | 触发碰撞球组件 |
| PropMesh | 0x1048 | UStaticMeshComponent* | Edit, ExportObject, Protected | 道具网格 |
| CurrentState | 0x1090 | EMoePropState (uint8) | BlueprintVisible, Protected | 当前道具状态 |
| CurrentEnergy | 0x1094 | float | ZeroConstructor, Protected | 当前能量 |
| PropConfigId | 0x11D0 | int32 | Net, RepNotify, Protected | 道具配置 ID |
| PropRuntimeId | 0x11D4 | int32 | Net, RepNotify, Protected | 道具运行时 ID |
| UGCCoolTime | 0x1270 | float | Net, RepNotify, Protected | UGC 冷却时间 |
| UGCDamageValue | 0x1274 | int32 | Net, RepNotify, Protected | UGC 伤害值 |

### 3.3 PropTrigger 碰撞球

PropTrigger 是 USphereComponent 指针, 其 SphereRadius 字段控制碰撞检测半径:

```
PropTrigger = [PropActor + 0x1040]
SphereRadius = read_float(PropTrigger + 0x0538)
```

修改 SphereRadius 可扩大道具的碰撞检测范围, 实现更大范围的 AOE 效果。

### 3.4 道具使用与伤害机制

道具使用时触发 UseEffects 数组中的 UMoeEffect 对象。其中伤害类效果为 `UMoeTakeDamageEffect`, 继承自 `UMoeEffect`:

```
UMoeEffect (Size: 0x00F8, base 0x0028)
  └─ UMoeTakeDamageEffect (Size: 0x01A8, base 0x00F8)
       ├─ HitResult           @ 0x010C  (FHitResult, 0x8C bytes)
       ├─ PropWeaponType      @ 0x0198  (ESGWeaponType, uint8)
       ├─ MatchingWeaponType  @ 0x0199  (EMoePropDamageType, uint8)
       ├─ ChangeOtherHPValue  @ 0x019C  (float, 实际伤害效果值)
       └─ DamageCauser        @ 0x01A0  (ABaseMoeGameCharacter*)
```

伤害施加路径: 道具使用 → UseEffects 遍历 → UMoeTakeDamageEffect.ChangeOtherHPValue → ANPCBase::TakePropDamage(Damage, ImpactLocation) → 怪物扣血。

### 3.5 道具持有与装备

角色通过 `UMoePropManageComponent` 管理道具槽位:

| 字段 | 偏移 | 类型 | 说明 |
|------|------|------|------|
| CurrentProps | +0x0128 | TMap\<EMoePropTargetSlot, AMoePropActorBase*\> | 当前各槽位道具 |
| CurrentCacheProps | +0x0178 | TMap\<EMoePropTargetSlot, FMoePropArray\> | 缓存道具 |
| CurrentPropInfos | +0x01C8 | TArray\<FCharacterPropInfo\> | 道具信息 (Net, RepNotify) |

关键函数:
- `GetCurrentProp()` - 获取当前手持道具
- `GetCurrentAbilityProp()` - 获取当前能力道具
- `AddProp(AMoePropActorBase*)` - 添加道具
- `UsePropImp(FManipulatorInput, AMoePropActorBase*, EMoePropTargetSlot)` - 使用道具
- `UsePropOnServer(FManipulatorInput, AMoePropActorBase*, EMoePropTargetSlot)` - 服务器端使用

### 3.6 道具修改策略

对道具的内存修改采用三层策略:

| 层级 | 字段 | 偏移 | 修改方式 | PVE 有效性 |
|------|------|------|----------|-----------|
| 第 1 层 | PropActor.ChangeOtherHPValue | 0x0C8C | 原始值 * 倍率 | 道具配置值, 可能被 Effect 覆盖 |
| 第 2 层 | UGCDamageValue | 0x1274 | 原始值 * 倍率 | PVE 下服务器校验弱, 实际生效 |
| 第 3 层 | UMoeTakeDamageEffect.ChangeOtherHPValue | 0x019C | 原始值 * 倍率 | 实际伤害效果对象, 直接参与伤害计算 |

原始值跟踪: 每个道具 Actor 地址对应一个 PropOrigValues 记录, 包含各字段的原始值和已修改的 Effect 对象列表。恢复时遍历记录逐一写回。

### 3.7 电锯道具

电锯的类名为 `BP_ChestPVE_SupplyProp_ChainSaw_C`, 属于 AMoePropActorBase 子类。电锯不走 AWeapon 武器系统, 通过 `AMoePropSpawnerBase` 生成, 拾取后通过 UMoePropManageComponent 管理槽位。修改 AWeapon 字段对电锯无效, 必须直接修改 PropActor 上的字段。

---

## 4. 武器系统 (AWeaponBase)

### 4.1 核心字段

| 字段 | 偏移 | 类型 | 用途 |
|------|------|------|------|
| bEnableSpeedDec | 0x0508 | bool | 是否启用持武器减速 |
| speedDecPercent | 0x050C | float | 减速百分比 |
| AttackColliderExtent | 0x0554 | FVector | 攻击碰撞箱范围 |
| MaxLockTargetRange | 0x0604 | float | 最大锁定距离 |
| MaxLockTargetAngle | 0x0608 | float | 最大锁定角度 |

武器系统与道具系统独立。枪械类武器走 AWeapon 系统, 近战道具 (电锯、炸弹等) 走 AMoePropActorBase 系统。GetCurrentWeapon() 读取的是武器库存, 不包含道具。

### 4.2 服务器校验

| 字段 | 客户端修改 | 服务器校验 | PVE 模式 |
|------|-----------|-----------|---------|
| AttackColliderExtent | 可修改 | 弱校验 | 生效 |
| MaxLockTargetRange | 可修改 | 弱校验 | 生效 |
| bEnableSpeedDec | 可修改 | 无校验 | 生效 |
| UGCDamageValue | 可修改 | RepNotify 但 PVE 弱校验 | 生效 |

---

## 5. 怪物系统 (ANPCBase)

### 5.1 核心结构

```
AActor
  └─ APawn
       └─ ACharacter
            └─ ABaseMoeGameCharacter
                 └─ ANPCBase              (MoeGameCore)
                      └─ AChestMonsterBase (Feature_ChestPVE)
                           └─ AChestPVEGameCharacter
```

### 5.2 关键字段与偏移

| 字段 | 偏移 | 类型 | 说明 |
|------|------|------|------|
| AIParam | 0x05A0 | 指针 | AI 参数组件 |
| AIParam.BehaveMode | 0x05A0+0x14 | uint8 | 行为状态 (网络复制) |
| AIParam.monsterUid | 0x05A0+0x08 | int64 | 怪物 UID |
| NPCAttributeComponent | 0x0620 | 指针 | 属性组件 |
| IsAlive.Value | 0x0620+0x0971 | bool | 存活状态 |
| MonsterLevel | 0x0638 | int32 | 怪物等级 |
| MonsterID | 0x0640 | int32 | 怪物类型 ID |
| ExecutionerMonster | 0x0D18 | 指针 | 处刑怪物引用 (AChestPVEGameCharacter) |
| IsInBeGrabed | 0x0CD0 | bool | 是否被抓取 (处刑状态) |

### 5.3 行为状态枚举

怪物行为状态从 `ANPCBase.AIParam.BehaveMode` 读取, 网络复制保证实时准确性:

| 状态值 | 名称 | 颜色 | 威胁等级 |
|--------|------|------|---------|
| 3 | 待机 | 浅黄 (ImColor 255,235,59) | 低 |
| 4 | 搜索中 | 黄 (ImColor 255,193,7) | 低 |
| 6 | 追逐中 | 鲜红 (ImColor 244,67,54) | 高 |
| 8 | 攻击中 | 橙红 (ImColor 255,87,34) | 高 |
| 9 | 眩晕 | 蓝紫 (ImColor 103,58,183) | 低 |
| 18 | 死亡 | 灰半透明 (ImColor 158,158,158,128) | 无 |

高威胁状态 (追击/攻击/蓄力) 在怪物位置上方显示红色警告点。

### 5.4 怪物属性组件

`UMoeMonsterAttributeComponent` 包含怪物的基础属性:

| 属性 | 说明 |
|------|------|
| SightRadius | 视野范围 |
| LoseSightRadius | 丢失视野范围 |
| sightAngle | 视野角度 |
| MaxAttackRange | 最大攻击距离 |
| ChaseSpeed | 追击速度 |
| PatrolSpeed | 巡逻速度 |
| EscapeSpeed | 逃跑速度 |
| DashSpeed | 冲刺速度 |
| DashDuration | 冲刺持续时间 |
| DashPrepareTime | 冲刺准备时间 |

### 5.5 处刑预警

处刑状态通过两个字段判断:
- `AChestPVEGameCharacter.ExecutionerMonster` (0x0D18) - 正在执行处刑的怪物
- `AChestPVEGameCharacter.IsInBeGrabed` (0x0CD0) - 角色是否被抓取

当 IsInBeGrabed 为 true 且 ExecutionerMonster 有效时, 触发处刑预警。

---

## 6. 伤害系统

### 6.1 伤害类型

| 来源 | 函数 | 说明 |
|------|------|------|
| 道具 | ANPCBase::TakePropDamage(float Damage, FVector ImpactLocation) | 道具伤害入口 |
| 武器 | AWeaponBase::CanDoDamage(AActor* Target) | 武器伤害判定 |
| 属性 | UMoeCharAttributeComponent | 属性伤害计算 |
| 效果 | UMoeTakeDamageEffect | 伤害效果对象 |
| 处理器 | UMoeTakeDamageEffectHandler | 伤害效果处理器 |

### 6.2 UMoeTakeDamageEffect

伤害效果对象是道具伤害的核心。每个道具的 UseEffects 数组中可能包含多个 Effect 对象, 其中类名含 "Damage" 的即为伤害效果。

Effect 类名匹配规则: 读取 Effect 对象的 ClassPrivate → NamePrivate → ResolveFName, 检查结果是否包含 "Damage" 或 "TakeDamage" 子串。

### 6.3 伤害计算

```
CalDamageRatioByAttrAdvantage(SrcType, TargetType, DamageRatio)
  → 计算属性克制伤害倍率
```

属性克制影响最终伤害, 但基础伤害值由 ChangeOtherHPValue (Effect 中 0x019C 或 PropActor 中 0x0C8C) 决定。

---

## 7. 透视与 ESP 系统

### 7.1 Actor 遍历框架

所有透视功能共用同一 Actor 遍历框架:

```
GWorld = [Base + 0x11FCAC30]
Level = [GWorld + 0x30]
Actors.Data = [Level + 0x98]
Actors.Num = read_int32(Level + 0x98 + 0x8)

for i in 0..Num:
    entity = [Actors.Data + i * 8]
    clsName = GetClassName(entity)
    → 按类名子串匹配分类
```

### 7.2 物品类型映射

物品透视按类名子串匹配分配类型 ID, 已映射的物品类型超过 40 种。部分关键映射:

| 类型 ID | 中文名 | 类名特征 |
|---------|--------|---------|
| 0 | 未分类 | 无匹配 |
| 43 | 撤离电梯 | 含 Exit/Elevator/Evacuat/Escap/Extract |
| 44 | 探灵怪物 | 含 BP_ChestPVE_Monster_ |

### 7.3 撤离点透视

- 匹配方式: 类名以 `BP_ChestPVE_` 开头, 且包含 Exit/Elevator/Evacuat/Escap/Extract 关键字
- 渲染颜色: 亮金色 `ImColor(255, 200, 0, 255)`, 双层描边
- 扫描距离: 启用时自动扩展到 300 米
- 类名缓存: 每 10 秒刷新一次, 确保新实例被检测到
- 独立 UI 开关, 金色文字标签

### 7.4 怪物透视/雷达

- 匹配方式: 类名以 `BP_ChestPVE_Monster_` 开头
- 扫描距离: 50-500 米, 默认 300 米 (可调)
- 缓存容量: 128 个条目
- 显示格式: `怪物名 [行为状态] Lv.等级 [距离m]`
- 纯只读功能, 不修改任何游戏内存
- 左上角实时显示怪物数量
- 高威胁怪物 (追击/攻击/蓄力) 显示红色警告点
- 怪物名称通过 `GetMonsterDisplayName` 函数获取, 失败时回退到类名子串

---

## 8. 已实现功能模块

### 8.1 基础功能

| 功能 | 实现方式 | 关键偏移 |
|------|---------|---------|
| 移速倍率 | MovementComponent.MaxWalkSpeed * 倍率 | selfEntity+0x6F0 |
| 跳跃倍率 | MovementComponent.JumpZVelocity * 倍率 | selfEntity+0x6F0 |
| 踏空飞行 | MovementComponent.GravityScale / AirControl | selfEntity+0x6F0 |
| 穿墙 | CollisionResponse 修改 | selfEntity RootComponent |
| 移动增强 | 6 个 checkbox: 低重力/空中控制/快速启动/防摔落/惯性滑行/无限冲刺 | - |

### 8.2 战斗功能

| 功能 | 实现方式 | 说明 |
|------|---------|------|
| 无敌 | 技能无敌 + SP 模式无敌 | 免疫所有伤害和效果 |
| 免伤 | 受伤比率 = 0.05 | 95% 免伤 |
| 强化抗眩晕 | 眩晕时间 = 0.05 秒 + 阈值 10 倍 | 比普通抗眩晕更彻底 |
| 霸体 | 不受击 + 抗眩晕 + 免疫推动 | 组合功能 |
| 锁血 | HP 锁定为 MaxHP | 非极端值 |
| 自动回血 | HP 低于 50% 渐进恢复 | 每帧恢复 5% MaxHP |
| 快速射击 | 射速倍率 | - |
| 无限弹药 | 弹药量锁定 | - |
| 伤害增强 | 伤害倍率 | - |

### 8.3 PVE 武器增强

| 功能 | 字段 | 偏移 | 修改方式 |
|------|------|------|---------|
| AOE 范围 | AttackColliderExtent | 0x0554 | 原始值 * 倍率 |
| 锁定扩大 | MaxLockTargetRange | 0x0604 | 原始值 * 3 |
| 锁定扩大 | MaxLockTargetAngle | 0x0608 | 原始值 * 3 |
| 持武不减速 | bEnableSpeedDec | 0x0508 | 设为 false |
| 持武不减速 | speedDecPercent | 0x050C | 设为 0 |

### 8.4 PVE 道具增强

| 功能 | 修改字段 | 说明 |
|------|---------|------|
| 道具加伤 | ChangeOtherHPValue + UGCDamageValue + UseEffects 伤害 Effect | 三层修改, 原始值 * 倍率 |
| 道具范围 | PropTrigger.SphereRadius | 原始值 * 倍率, 上限 2000 |
| 道具快攻 | UseCoolTime * 0.5 + UseEnergyCost = 0 | 冷却减半 + 无能量消耗 |

道具遍历: 每 3 秒刷新缓存, 匹配所有类名含 "Prop" 的 Actor (排除 Fishing), 最多缓存 32 个, 原始值跟踪最多 16 个。

### 8.5 星世界跳关

通过遍历 `GWorld->PersistentLevel->Actors` 查找 `LevelRepInfoActor`, 读取关卡实例 ID 和完成状态, 支持自动跳关。

---

## 9. 属性系统 (UMoeCharAttributeComponent)

### 9.1 FAttributeFloat 结构

属性值通过 FAttributeFloat 封装, Value 字段在属性对象偏移 +0x74 处:

```
CharAttributeComponent = [selfEntity + 0x6F8]
AttributeFloat.Value = [AttributeFloat + 0x74]
```

### 9.2 FAttributeBool 结构

布尔属性通过 FAttributeBool 封装, Value 字段在属性对象偏移 +0x51 处:

```
AttributeBool.Value = [AttributeBool + 0x51]
```

### 9.3 常用属性

部分属性可直接通过 selfEntity 偏移读写 (DirectBool), 无需通过属性组件间接访问。

---

## 10. 注意事项

### 10.1 安全性原则

- 所有修改值使用原始值乘以倍率, 不使用极端固定值 (如 99999, 0.0)
- 恢复功能使用跟踪的原始值, 不使用硬编码默认值
- 怪物透视为纯只读功能, 不修改任何游戏内存
- 道具修改每帧使用记录的原始值乘以倍率, 避免指数增长

### 10.2 网络复制字段

标记为 `Net` 或 `RepNotify` 的字段会被服务器同步。在 PvP 模式下, 服务器校验严格, 客户端修改会被纠正。但在 PVE 探灵实录模式下, 服务器校验较弱, 部分网络字段 (如 UGCDamageValue) 的修改可以生效。

### 10.3 缓存与刷新

| 缓存对象 | 刷新间隔 | 容量上限 |
|---------|---------|---------|
| 道具列表 | 3 秒 | 32 个 |
| 道具原始值 | 随道具列表 | 16 个 |
| 怪物列表 | 每帧 | 128 个 |
| 撤离点类名 | 10 秒 | - |
| GName | 首次解析后永久缓存 | 无上限 |

### 10.4 地址有效性校验

所有指针读取前进行范围校验: `0x10000000 <= addr < 0x10000000000`。超出范围的地址视为无效, 跳过处理。Actor 数量校验: `0 < Num < 4096`。

### 10.5 道具系统 vs 武器系统

电锯、炸弹、飞刀等近战/投掷道具属于 `AMoePropActorBase`, 不属于 `AWeapon` 武器系统。修改 AWeapon 的字段 (如 AttackColliderExtent) 对这些道具无效。必须通过遍历 Actor 列表找到对应的 PropActor, 直接修改其字段。
