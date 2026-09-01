# 项目长期记忆（UE4 ImGui / UMT MCP）

## 网络与连接约定（2026-09-01 落地）

- 设备端 `CommandServer` 默认只监听 `127.0.0.1:35515`（安全红线）。非回环 bind 地址
  需由 `/sdcard/UnrealMemoryTools/mcp_bind.conf`（一行，如 `192.168.1.23`）显式指定，默认不放宽。
- PC 侧 `server.py` 三种模式：① adb 隧道（默认）② `--no-adb --host <局域网IP>` 直连
  ③ 同机直连（手机端 Operit）用 `--no-adb --host 127.0.0.1`。
- `bridge.py` 的 `UmtBridge.__init__` 的 host 默认参数必须写成 `host: str | None = None`，
  再函数体内取 `config.HOST` —— 写成 `host: str = config.HOST` 会在 import 期冻结，导致 `--host` 失效。
- Operit 与 UMT 同机同 network namespace，`127.0.0.1:35515` 直连可达，不需要 ADB（adb forward 是跨机隧道）。

## 构建（设备端）

- NDK 27 + VS2026 CMake/Ninja：`build/CMakeCache.txt` 已记录路径。
  增量编译：`cmake -S . -B build -DNDK_PATH=...` 后 `cmake --build build`。
  查产物新代码用 NDK 的 `llvm-strings.exe`。
- ⚠️ 已知前置编译阻塞（与本仓库 MCP 工作无关）：`executable.cpp:4268` 调
  `ANativeWindowCreator::ProcessMirrorDisplay()`，但 `ANativeWindowCreator.h` 工作树已删除该成员。
  属 mirror/virtual display 子系统，修复它不在 MCP 网络任务范围内。

## GetGUObjectArrayPtr 陷阱（2026-09-01 修复记录）

- **不要对只读数据用 `isPtrWritable`**：`TUObjectArray.Objects` 和 `ObjObjects_Objects` 只需
  可读（`isPtrReadable`），游戏常把 chunk 指针表放在 `.rodata`。误用 `isPtrWritable` 会静默
  丢弃有效候选，导致 "通用方式搜索 GUObject 失败"。
- 验证槽位对象的内存可读性已有 `isPtrReadable(firstObj)` 兜底，不会被 bogus executable page 骗过。
