#pragma once

// 轻量 ARM64(AArch64) 反汇编器（设备端 DISASSEMBLE 命令的地基）。
//
// 覆盖范围（UE 内存分析高频指令，占调试场景绝大多数）：
//   - PC 相对：ADR / ADRP（定位 GOT / 字符串 / 函数基地址最关键）
//   - 常量装载：MOVZ / MOVN / MOVK（sf 决定 32/64 位）
//   - 算术立即数：ADD / SUB（immediate，含 ADRP+ADD 页内偏移组合）
//   - 加载字面量：LDR (literal, PC-relative)
//   - 控制流：B / BL / BLR / RET / BR / B.cond / CBZ / CBNZ / TBZ / TBNZ
//   - 系统/杂项：NOP / BRK / HINT
//
// 设计取舍：项目未集成 capstone/unicorn/Zydis 等反汇编库，为避免引入重型第三方
// 依赖与离线编译风险，采用自研聚焦解码器。未识别指令（如 SIMD / 浮点 / 加密扩展
// 等）统一返回 "???"，不会崩溃。后续若需 100% 指令覆盖，可改接 capstone 并替换
// DisassembleArm64 的实现，调用方接口保持不变。
//
// 调用约定：insn 须为从目标地址按小端读取的 32 位机器码（设备为小端 ARM，
// memcpy 到 uint32_t 即得正确值，无需字节序转换）。

#include <cstdint>
#include <string>

namespace UmtMcp
{
// 给定 PC（指令虚拟地址）与 32 位指令机器码，返回一行可读汇编文本。
// 无法识别时返回 "???"。
std::string DisassembleArm64(uint64_t pc, uint32_t insn);

}  // namespace UmtMcp
