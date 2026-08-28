local data_str = [[
.fa-0::before {
  content: "\30"; }

.fa-1::before {
  content: "\31"; }

.fa-google-pay:before {
  content: "\e079"; }

.fa-intercom:before {
  content: "\f7af"; }

.fa-zhihu:before {
  content: "\f63f"; }

.fa-korvue:before {
  content: "\f42f"; }
]]




local function tohex(val)
  return string.format("%X", val)
end
-- 函数：将 Unicode 字符编码转换为 UTF-8 字节序列
-- 参数：codepoint - Unicode 字符的码点
-- 返回：UTF-8 字节序列
local function unicodeToBytes(codepoint)
    if codepoint <= 0x7F then
        return string.char(codepoint)
    elseif codepoint <= 0x7FF then
        return string.char(0xC0 | (codepoint >> 6), 0x80 | (codepoint & 0x3F))
    else
        return string.char(0xE0 | (codepoint >> 12), 0x80 | ((codepoint >> 6) & 0x3F), 0x80 | (codepoint & 0x3F))
    end
end


function my_split(s, separator)
    local parts = {}
    local start = 1
    local index = string.find(s, separator, start, true) -- 第四个参数 'true' 表示使用模式匹配模式
    while index do
        table.insert(parts, string.sub(s, start, index-1))
        start = index + #separator
        index = string.find(s, separator, start, true)
    end
    table.insert(parts, string.sub(s, start))
    return parts
end

function replaceAndUpperCase(str)
    -- 将`.`和`-`替换为`_`
    str = string.gsub(str, "[.-]", "_")
    -- 将所有小写字母替换为大写
    str = string.upper(str)
    return str
end


local g = my_split(data_str, "\n")
--print(#g)


local ICON_MIN_FA = 99999999;
local ICON_MAX_FA = 0;
local h_path = "/sdcard/gui_icon.h"
local r_file = io.open(h_path, 'r')
if (r_file ~= nil) then
    os.remove(h_path)  
    r_file:close()
end

local file_h = io.open(h_path, 'a')
file_h:write(string.format("#pragma once\n"))
file_h:write(string.format("\n\n"))
for i = 1, #g do
    local startPos, endPos = string.find(g[i], "::before")
    if (startPos == nil and endPos == nil) then
        startPos, endPos = string.find(g[i], ":before")    
    end
    if (startPos ~= nil and endPos ~= nil) then
        local res = string.sub(g[i], 2, startPos-1)
        --print(res)
        startPos, endPos = string.find(g[i+1], "content: \"\\")
        if (startPos ~= nil and endPos ~= nil) then
            local startPos_2, endPos_2 = string.find(g[i+1], "\";")
            local hex_str = string.sub(g[i+1], endPos+1, startPos_2-1)

            local decimal_num = tonumber(hex_str, 16)
            --print(decimal_num)
            
            local byteSequence = unicodeToBytes(decimal_num)
 
            local hexString = ""
            for i = 1, #byteSequence do
                hexString = hexString .. string.format("\\x%02x", string.byte(byteSequence, i))
            end
            if (decimal_num < ICON_MIN_FA) then
                ICON_MIN_FA = decimal_num            
            end
            if (decimal_num > ICON_MAX_FA) then
                ICON_MAX_FA = decimal_num
            end
            local U = string.format("ICON_%s \"%s\"    // U+%s", replaceAndUpperCase(res), hexString, hex_str)
            file_h:write(string.format("#define %s\n", U))
        --]]
            --print(res)
        
        end
        
    end

end



print("#define ICON_MIN_FA 0x" .. tohex(ICON_MIN_FA) )
print("#define ICON_MAX_FA 0x" .. tohex(ICON_MAX_FA) )
file_h:close()     









--[[
-- Unicode 字符编码 \uf044 对应的码点是 0xf044
local codepoint = 0xf6ad

-- 将 Unicode 字符编码转换为字节序列
local byteSequence = unicodeToBytes(codepoint)

-- 输出字节序列的十六进制表示
local hexString = ""
for i = 1, #byteSequence do
    hexString = hexString .. string.format("\\x%02x", string.byte(byteSequence, i))
end
print("Byte sequence: " .. hexString)
--]]