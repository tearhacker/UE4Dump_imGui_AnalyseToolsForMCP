#include "MemoryHelpers.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>

namespace UmtMcp
{
namespace
{
// 字符串是否仅由十六进制数字组成
bool IsPureHex(const std::string &s)
{
    if (s.empty())
        return false;
    return s.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos;
}

// 是否含 a-f/A-F（用于区分「纯十进制数字串」与「十六进制地址串」）
bool HasHexLetter(const std::string &s)
{
    return s.find_first_of("abcdefABCDEF") != std::string::npos;
}
}  // namespace

bool ParseAddress(const std::string &s, uintptr_t &out)
{
    if (s.empty())
        return false;

    std::string t = s;
    // 去掉 "0x" / "0X" 前缀
    if (t.size() >= 2 && t[0] == '0' && (t[1] == 'x' || t[1] == 'X'))
        t = t.substr(2);
    if (t.empty())
        return false;

    // 含字母 → 按十六进制；否则按十进制（地址通常带 0x 或纯 hex，纯数字十进制兜底）
    const int base = HasHexLetter(t) ? 16 : 10;
    try
    {
        out = static_cast<uintptr_t>(std::stoull(t, nullptr, base));
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool ParseOffset(const std::string &s, ParsedOffset &out)
{
    out = ParsedOffset{};
    if (s.empty())
        return false;

    std::string t = s;

    // 1) 解析数组下标 [n]
    const size_t br = t.find('[');
    if (br != std::string::npos)
    {
        const size_t brEnd = t.find(']', br);
        if (brEnd == std::string::npos)
            return false;  // 括号不配对
        const std::string idxStr = t.substr(br + 1, brEnd - br - 1);
        try
        {
            out.arrayIndex = std::stoll(idxStr, nullptr, 10);
        }
        catch (...)
        {
            return false;
        }
        t = t.substr(0, br);  // 去掉 "[n]" 部分
    }

    // 2) 去掉前导 '+'（"-8" 的负号需保留，stoll 可解析）
    if (!t.empty() && t[0] == '+')
        t = t.substr(1);
    if (t.empty())
        return false;

    const int base = HasHexLetter(t) ? 16 : 10;
    try
    {
        out.offset = std::stoll(t, nullptr, base);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

std::string FormatAddress(uintptr_t addr)
{
    char buf[32];
    std::snprintf(buf, sizeof(buf), "0x%llx", static_cast<unsigned long long>(addr));
    return std::string(buf);
}

std::string BytesToHex(const uint8_t *data, size_t len)
{
    if (!data || len == 0)
        return "";
    static const char *kHexDigits = "0123456789ABCDEF";
    std::string out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; ++i)
    {
        out.push_back(kHexDigits[data[i] >> 4]);
        out.push_back(kHexDigits[data[i] & 0x0F]);
    }
    return out;
}

size_t ValueTypeSize(const std::string &type)
{
    if (type == "bool" || type == "i8" || type == "u8")
        return 1;
    if (type == "i16" || type == "u16")
        return 2;
    if (type == "i32" || type == "u32" || type == "f32" || type == "ptr32")
        return 4;
    if (type == "i64" || type == "u64" || type == "f64" || type == "ptr64")
        return 8;
    return 0;
}

std::string WstringToUtf8(const std::wstring &ws)
{
    std::string out;
    out.reserve(ws.size() * 3);
    for (const wchar_t wc : ws)
    {
        const uint32_t cp = static_cast<uint32_t>(wc);
        if (cp <= 0x7F)
        {
            out.push_back(static_cast<char>(cp));
        }
        else if (cp <= 0x7FF)
        {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0xFFFF)
        {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else
        {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }
    return out;
}

std::string FormatPermissions(bool readable, bool writeable, bool executable)
{
    std::string s;
    s.push_back(readable ? 'r' : '-');
    s.push_back(writeable ? 'w' : '-');
    s.push_back(executable ? 'x' : '-');
    return s;
}

// 单 hex 字符 → 数值；非法返回 -1
int HexDigit(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool HexToBytes(const std::string &s, std::vector<uint8_t> &out)
{
    out.clear();
    std::string t = s;
    // 去掉可能出现的 "0x"/"0X" 字节前缀（逐处）
    size_t pos = 0;
    std::string stripped;
    stripped.reserve(t.size());
    while (pos < t.size())
    {
        if (t[pos] == '0' && (pos + 1 < t.size()) && (t[pos + 1] == 'x' || t[pos + 1] == 'X'))
        {
            pos += 2;
            continue;
        }
        stripped.push_back(t[pos]);
        ++pos;
    }
    // 去空格
    std::string compact;
    for (char c : stripped)
        if (!isspace((unsigned char)c))
            compact.push_back(c);
    if (compact.empty() || (compact.size() % 2) != 0)
        return false;
    for (size_t i = 0; i < compact.size(); i += 2)
    {
        const int hi = HexDigit(compact[i]);
        const int lo = HexDigit(compact[i + 1]);
        if (hi < 0 || lo < 0)
            return false;
        out.push_back(static_cast<uint8_t>((hi << 4) | lo));
    }
    return true;
}

std::string NormalizeIdaPattern(const std::string &pattern)
{
    std::string s = pattern;
    // "??" → "?"
    size_t idx;
    while ((idx = s.find("??")) != std::string::npos)
        s.replace(idx, 2, "?");
    // 转大写
    for (char &c : s)
        c = static_cast<char>(::toupper((unsigned char)c));
    // 压缩空白为单空格，去首尾空格
    std::string out;
    bool prevSpace = false;
    for (char c : s)
    {
        if (isspace((unsigned char)c))
        {
            if (!prevSpace && !out.empty())
            {
                out.push_back(' ');
                prevSpace = true;
            }
        }
        else
        {
            out.push_back(c);
            prevSpace = false;
        }
    }
    while (!out.empty() && out.back() == ' ')
        out.pop_back();
    return out;
}

}  // namespace UmtMcp
