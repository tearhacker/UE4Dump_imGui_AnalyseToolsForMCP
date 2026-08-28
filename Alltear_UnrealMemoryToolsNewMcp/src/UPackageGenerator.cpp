#include "UPackageGenerator.hpp"

#include "UE/UEMemory.hpp"
using namespace UEMemory;

#include "AutoFix/NativeFunctionResolver.hpp"
#include "AutoFix/StructLayout.hpp"
#include "AutoFix/VirtualFunctionResolver.hpp"

#include <unordered_map>
#include <unordered_set>

void UE_UPackage::GenerateBitPadding(std::vector<Member> &members, uint32_t offset, uint8_t bitOffset, uint8_t size)
{
    Member padding;
    padding.Type = "uint8_t";
    padding.Name = fmt::format("BitPad_0x{:X}_{} : {}", offset, bitOffset, size);
    padding.Offset = offset;
    padding.Size = 1;
    members.push_back(padding);
}

void UE_UPackage::GeneratePadding(std::vector<Member> &members, uint32_t offset, uint32_t size)
{
    Member padding;
    padding.Type = "uint8_t";
    padding.Name = fmt::format("Pad_0x{:X}[0x{:X}]", offset, size);
    padding.Offset = offset;
    padding.Size = size;
    members.push_back(padding);
}

void UE_UPackage::FillPadding(const UE_UStruct &object, std::vector<Member> &members, uint32_t &offset, uint8_t &bitOffset, uint32_t end)
{
    (void)object;

    if (bitOffset && bitOffset < 8)
    {
        UE_UPackage::GenerateBitPadding(members, offset, bitOffset, 8 - bitOffset);
        bitOffset = 0;
        offset++;
    }

    if (offset != end)
    {
        GeneratePadding(members, offset, end - offset);
        offset = end;
    }
}

void UE_UPackage::GenerateFunction(const UE_UFunction &fn, Function *out)
{
    out->Name = fn.GetName();
    out->FullName = fn.GetFullName();
    out->EFlags = fn.GetFunctionEFlags();
    out->Flags = fn.GetFunctionFlags();
    out->NumParams = fn.GetNumParams();
    out->ParamSize = fn.GetParamSize();
    out->Func = fn.GetFunc();
    out->OwnerAddress = reinterpret_cast<uintptr_t>(fn.GetOuter().GetAddress());

    auto generateParam = [&](IProperty *prop)
    {
        if (!prop)
            return;

        const auto flags = prop->GetPropertyFlags();
        const auto type = prop->GetType().second;
        const auto name = prop->GetName();

        // dump-7 的做法是：函数 ChildProperties 链里的字段默认都视作参数，
        // 只用 ReturnParm/OutParm/ConstParm 去修饰，而不是强依赖 CPF_Parm。
        if ((flags & CPF_ReturnParm) || name == "ReturnValue")
        {
            out->CppName = type + " " + fn.GetName();
            return;
        }

        if (type.empty() || name.empty())
            return;

        if (!out->Params.empty())
            out->Params += ", ";

        if (flags & CPF_ConstParm)
            out->Params += "const ";

        if (prop->GetArrayDim() > 1)
        {
            out->Params += fmt::format("{}* {}", type, name);
            return;
        }

        out->Params += type;
        if (flags & CPF_OutParm)
            out->Params += "&";
        out->Params += " " + name;
    };

    bool usedChildProperties = false;
    for (auto prop = fn.GetChildProperties().Cast<UE_FProperty>(); prop; prop = prop.GetNext().Cast<UE_FProperty>())
    {
        usedChildProperties = true;
        auto propInterface = prop.GetInterface();
        generateParam(&propInterface);
    }

    // UE4.25+/UE5 优先使用 ChildProperties(FField)；仅在旧链路下回退到 Children(UField)。
    if (!usedChildProperties)
    {
        for (auto prop = fn.GetChildren().Cast<UE_UProperty>(); prop; prop = prop.GetNext().Cast<UE_UProperty>())
        {
            auto propInterface = prop.GetInterface();
            generateParam(&propInterface);
        }
    }
    if (out->Params.size())
    {
        // no-op: params are now appended with delimiter-aware logic
    }

    if (out->CppName.size() == 0)
    {
        out->CppName = "void " + fn.GetName();
    }
}

void UE_UPackage::GenerateStruct(const UE_UStruct &object, std::vector<Struct> &arr)
{
    Struct s;
    s.Name = object.GetName();
    s.FullName = object.GetFullName();
    const auto layoutFields = AutoFixStructLayout::GetSortedFields(object);
    const bool useAutoLayout = !layoutFields.empty();
    const auto &layoutInfo = useAutoLayout ? AutoFixStructLayout::GetStructLayoutInfo(object) : AutoFixStructLayout::StructLayoutInfo{};

    s.CppName = "struct ";
    if (useAutoLayout && layoutInfo.UseExplicitAlignment && layoutInfo.Alignment > 1)
        s.CppName += fmt::format("alignas(0x{:X}) ", layoutInfo.Alignment);
    s.CppName += object.GetCppName();

    s.Inherited = useAutoLayout ? std::max(0, AutoFixStructLayout::GetOwnMemberStart(object)) : 0;
    s.Size = useAutoLayout ? std::max(0, AutoFixStructLayout::GetDisplayStructSize(object)) : object.GetSize();
    s.Alignment = useAutoLayout ? std::max(1, layoutInfo.Alignment) : 1;
    s.UseExplicitAlignment = useAutoLayout && layoutInfo.UseExplicitAlignment && layoutInfo.Alignment > 1;
    s.UsePack = useAutoLayout && layoutInfo.HasReusedTrailingPadding;

    if (s.Size == 0)
    {
        arr.push_back(s);
        return;
    }

    auto super = object.GetSuper();
    if (super)
    {
        s.CppName += " : ";
        s.CppName += super.GetCppName();
        if (!useAutoLayout)
            s.Inherited = super.GetSize();
    }

    uint32_t offset = s.Inherited;
    uint8_t bitOffset = 0;

    auto generateMember = [&](IProperty *prop, Member *m)
    {
        auto arrDim = prop->GetArrayDim();
        m->Size = prop->GetSize() * arrDim;
        if (m->Size == 0)
        {
            return;
        }  // this shouldn't be zero

        auto type = prop->GetType();
        m->Type = type.second;
        m->Name = prop->GetName();
        m->Offset = prop->GetOffset();

        if (m->Offset > offset)
        {
            UE_UPackage::FillPadding(object, s.Members, offset, bitOffset, m->Offset);
        }
        if (type.first == UEPropertyType::BoolProperty && *(uint32_t *)type.second.data() != 'loob')
        {
            auto boolProp = prop;
            auto mask = boolProp->GetFieldMask();
            uint8_t zeros = 0, ones = 0;
            while (mask & ~1)
            {
                mask >>= 1;
                zeros++;
            }
            while (mask & 1)
            {
                mask >>= 1;
                ones++;
            }
            if (zeros > bitOffset)
            {
                UE_UPackage::GenerateBitPadding(s.Members, offset, bitOffset, zeros - bitOffset);
                bitOffset = zeros;
            }
            m->Name += fmt::format(" : {}", ones);
            bitOffset += ones;

            if (bitOffset == 8)
            {
                offset++;
                bitOffset = 0;
            }

            m->extra = fmt::format("Mask(0x{:X})", boolProp->GetFieldMask());
        }
        else
        {
            if (arrDim > 1)
            {
                m->Name += fmt::format("[0x{:X}]", arrDim);
            }

            offset += m->Size;
        }
    };

    auto generateLayoutMember = [&](const AutoFixStructLayout::FieldLayoutInfo &fieldInfo)
    {
        Member m;
        auto propInterface = fieldInfo.Field.GetInterface();
        m.Size = fieldInfo.TotalSize;
        if (m.Size == 0)
            return;

        auto type = propInterface.GetType();
        m.Type = type.second;
        m.Name = fieldInfo.Field.GetName();
        m.Offset = fieldInfo.Offset;

        if (m.Offset > offset)
            UE_UPackage::FillPadding(object, s.Members, offset, bitOffset, m.Offset);

        if (fieldInfo.IsBitfield && type.first == UEPropertyType::BoolProperty && *(uint32_t *)type.second.data() != 'loob')
        {
            auto mask = fieldInfo.FieldMask;
            uint8_t zeros = 0, ones = 0;
            while (mask & ~1)
            {
                mask >>= 1;
                zeros++;
            }
            while (mask & 1)
            {
                mask >>= 1;
                ones++;
            }
            if (zeros > bitOffset)
            {
                UE_UPackage::GenerateBitPadding(s.Members, m.Offset, bitOffset, zeros - bitOffset);
                bitOffset = zeros;
            }
            m.Name += fmt::format(" : {}", ones);
            bitOffset += ones;

            if (bitOffset == 8)
            {
                offset = m.Offset + fieldInfo.ElementSize;
                bitOffset = 0;
            }

            m.extra = fmt::format("Mask(0x{:X})", fieldInfo.FieldMask);
        }
        else
        {
            if (fieldInfo.ArrayDim > 1)
                m.Name += fmt::format("[0x{:X}]", fieldInfo.ArrayDim);
            offset = m.Offset + m.Size;
        }

        s.Members.push_back(std::move(m));
    };

    if (useAutoLayout)
    {
        for (const auto &fieldInfo : layoutFields)
            generateLayoutMember(fieldInfo);
    }
    else
    {
        for (auto prop = object.GetChildProperties().Cast<UE_FProperty>(); prop; prop = prop.GetNext().Cast<UE_FProperty>())
        {
            Member m;
            auto propInterface = prop.GetInterface();
            generateMember(&propInterface, &m);
            s.Members.push_back(m);
        }

        for (auto child = object.GetChildren(); child; child = child.GetNext())
        {
            if (child.IsA<UE_UFunction>())
            {
                auto fn = child.Cast<UE_UFunction>();
                Function f;
                GenerateFunction(fn, &f);
                s.Functions.push_back(f);
            }
            else if (child.IsA<UE_UProperty>())
            {
                auto prop = child.Cast<UE_UProperty>();
                Member m;
                auto propInterface = prop.GetInterface();
                generateMember(&propInterface, &m);
                s.Members.push_back(m);
            }
        }
    }

    if (useAutoLayout)
    {
        for (auto child = object.GetChildren(); child; child = child.GetNext())
        {
            if (!child.IsA<UE_UFunction>())
                continue;
            auto fn = child.Cast<UE_UFunction>();
            Function f;
            GenerateFunction(fn, &f);
            s.Functions.push_back(f);
        }
    }
    if (s.Size > offset)
    {
        UE_UPackage::FillPadding(object, s.Members, offset, bitOffset, s.Size);
    }

    arr.push_back(s);
}

void UE_UPackage::GenerateEnum(const UE_UEnum &object, std::vector<Enum> &arr)
{
    Enum e;
    e.FullName = object.GetFullName();

    uint64_t nameSize = GetPtrAlignedOf(UEWrappers::GetUEVars()->GetOffsets()->FName.Size);
    uint64_t pairSize = nameSize + sizeof(int64_t);

    auto names = object.GetNames();
    uint64_t max = 0;

    for (int32_t i = 0; i < names.Num(); i++)
    {
        auto pair = names.GetData() + i * pairSize;
        auto name = UE_FName(pair);
        auto str = name.GetName();
        auto pos = str.find_last_of(':');
        if (pos != std::string::npos)
            str = str.substr(pos + 1);

        auto value = vm_rpm_ptr<uint64_t>(pair + nameSize);
        if (value > max)
            max = value;

        e.Members.emplace_back(str, value);
    }

    // enum values should be in ascending order
    auto isUninitializedEnum = [](Enum &e) -> bool
    {
        if (e.Members.size() > 1)
        {
            for (size_t i = 1; i < e.Members.size(); ++i)
            {
                if (e.Members[i].second <= e.Members[i - 1].second)
                    return true;
            }
        }
        return false;
    };

    if (isUninitializedEnum(e))
    {
        max = e.Members.size();
        for (size_t i = 0; i < e.Members.size(); ++i)
        {
            e.Members[i].second = i;
        }
    }

    const char *type = nullptr;

    if (max > GetMaxOfType<uint32_t>())
        type = " : uint64_t";
    else if (max > GetMaxOfType<uint16_t>())
        type = " : uint32_t";
    else if (max > GetMaxOfType<uint8_t>())
        type = " : uint16_t";
    else
        type = " : uint8_t";

    e.CppName = "enum class " + object.GetName() + type;

    if (e.Members.size())
    {
        arr.push_back(e);
    }
}

void UE_UPackage::AppendStructsToBuffer(std::vector<Struct> &arr, BufferFmt *pBufFmt)
{
    for (auto &s : arr)
    {
        if (s.UsePack)
            pBufFmt->append("#pragma pack(push, 0x1)\n");

        pBufFmt->append("// Object: {}\n// Size: 0x{:X} (Inherited: 0x{:X})\n{}\n{{",
                        s.FullName, s.Size, s.Inherited, s.CppName);

        if (s.Members.size())
        {
            for (auto &m : s.Members)
            {
                pBufFmt->append("\n\t{} {}; // 0x{:X}(0x{:X})", m.Type, m.Name, m.Offset, m.Size);
                if (!m.extra.empty())
                {
                    pBufFmt->append(", {}", m.extra);
                }
            }
        }
        if (s.Functions.size())
        {
            if (s.Members.size())
                pBufFmt->append("\n");

            for (auto &f : s.Functions)
            {
                uintptr_t baseAddr = UEWrappers::GetUEVars()->GetBaseAddress();
                void *funcOffset = f.Func ? (void *)(f.Func - baseAddr) : nullptr;
                std::string vtComment;
                if (f.Func)
                {
                    const auto nativeInfo = AutoFixNativeFunctions::ResolveNativeFunctionInfo(f.OwnerAddress, (uintptr_t)f.Func, f.EFlags);
                    uintptr_t slotOff = nativeInfo.VTableOffset;
                    uintptr_t realOff = nativeInfo.RealOffset ? nativeInfo.RealOffset : nativeInfo.FuncOffset;
                    if (slotOff)
                    {
                        int vtIdx = AutoFixVTable::OffsetToIndex(slotOff);
                        if (vtIdx >= 0)
                            vtComment = fmt::format(" // VTableIndex: {} (Offset: 0x{:X}, Real: 0x{:X})", vtIdx, slotOff, realOff);
                        else
                            vtComment = fmt::format(" // VTableSlotOffset: 0x{:X} (Real: 0x{:X})", slotOff, realOff);
                    }
                }
                pBufFmt->append("\n\n\t// Object: {}\n\t// Flags: [{}]\n\t// Offset: {}\n\t// Params: [ Num({}) Size(0x{:X}) ]\n\t{}({});{}", f.FullName, f.Flags, funcOffset, f.NumParams, f.ParamSize, f.CppName, f.Params, vtComment);
            }
        }
        pBufFmt->append("\n}};\n");
        if (s.UsePack)
            pBufFmt->append("#pragma pack(pop)\n");
        pBufFmt->append("\n");
    }
}

void UE_UPackage::AppendEnumsToBuffer(std::vector<Enum> &arr, BufferFmt *pBufFmt)
{
    for (auto &e : arr)
    {
        pBufFmt->append("// Object: {}\n{}\n{{", e.FullName, e.CppName);

        size_t lastIdx = e.Members.size() - 1;
        for (size_t i = 0; i < lastIdx; i++)
        {
            auto &m = e.Members.at(i);
            pBufFmt->append("\n\t{} = {},", m.first, m.second);
        }

        auto &m = e.Members.at(lastIdx);
        pBufFmt->append("\n\t{} = {}", m.first, m.second);

        pBufFmt->append("\n}};\n\n");
    }
}

void UE_UPackage::Process()
{
    auto &objects = Package->second;

    std::unordered_map<uint8_t *, std::vector<UE_UFunction>> functionsByOuter;
    for (auto &object : objects)
    {
        if (!object || !object.IsA<UE_UFunction>())
            continue;

        UE_UObject outer = object.GetOuter();
        if (!outer)
            continue;

        functionsByOuter[outer.GetAddress()].push_back(object.Cast<UE_UFunction>());
    }

    auto mergeOuterFunctions = [&](const UE_UStruct &owner, Struct &dst)
    {
        auto it = functionsByOuter.find(owner.GetAddress());
        if (it == functionsByOuter.end())
            return;

        std::unordered_set<std::string> seenFullNames;
        for (const auto &fn : dst.Functions)
        {
            if (!fn.FullName.empty())
                seenFullNames.insert(fn.FullName);
        }

        for (const auto &fnObj : it->second)
        {
            Function f;
            GenerateFunction(fnObj, &f);
            if (f.FullName.empty())
                continue;
            if (!seenFullNames.insert(f.FullName).second)
                continue;
            dst.Functions.push_back(std::move(f));
        }
    };

    for (auto &object : objects)
    {
        if (object.IsA<UE_UClass>())
        {
            GenerateStruct(object.Cast<UE_UStruct>(), Classes);
            mergeOuterFunctions(object.Cast<UE_UStruct>(), Classes.back());
        }
        else if (object.IsA<UE_UScriptStruct>())
        {
            GenerateStruct(object.Cast<UE_UStruct>(), Structures);
            mergeOuterFunctions(object.Cast<UE_UStruct>(), Structures.back());
        }
        else if (object.IsA<UE_UEnum>())
        {
            GenerateEnum(object.Cast<UE_UEnum>(), Enums);
        }
    }
}

bool UE_UPackage::AppendToBuffer(BufferFmt *pBufFmt)
{
    if (!pBufFmt)
        return false;

    if (!Classes.size() && !Structures.size() && !Enums.size())
        return false;

    pBufFmt->append("// Package: {}\n// Enums: {}\n// Structs: {}\n// Classes: {}\n\n",
                    GetObject().GetName(), Enums.size(), Structures.size(), Classes.size());

    if (Enums.size())
    {
        UE_UPackage::AppendEnumsToBuffer(Enums, pBufFmt);
    }

    if (Structures.size())
    {
        UE_UPackage::AppendStructsToBuffer(Structures, pBufFmt);
    }

    if (Classes.size())
    {
        UE_UPackage::AppendStructsToBuffer(Classes, pBufFmt);
    }

    return true;
}

bool UE_UPackage::AppendToBuffers(BufferFmt *enumsBufFmt, BufferFmt *structsBufFmt, BufferFmt *classesBufFmt)
{
    if (!enumsBufFmt || !structsBufFmt || !classesBufFmt)
        return false;

    if (!Classes.size() && !Structures.size() && !Enums.size())
        return false;

    if (Enums.size())
    {
        enumsBufFmt->append("// Package: {}\n// Enums: {}\n\n", GetObject().GetName(), Enums.size());
        UE_UPackage::AppendEnumsToBuffer(Enums, enumsBufFmt);
    }

    if (Structures.size())
    {
        structsBufFmt->append("// Package: {}\n// Structs: {}\n\n", GetObject().GetName(), Structures.size());
        UE_UPackage::AppendStructsToBuffer(Structures, structsBufFmt);
    }

    if (Classes.size())
    {
        classesBufFmt->append("// Package: {}\n// Classes: {}\n\n", GetObject().GetName(), Classes.size());
        UE_UPackage::AppendStructsToBuffer(Classes, classesBufFmt);
    }

    return true;
}

static std::string MakeCppIdentifier(const std::string &raw)
{
    std::string out;
    out.reserve(raw.size());
    for (char c : raw)
    {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '_')
            out.push_back(c);
        else
            out.push_back('_');
    }
    if (!out.empty() && out[0] >= '0' && out[0] <= '9')
        out.insert(out.begin(), '_');
    if (out.empty())
        out = "_";
    return out;
}

void UE_UPackage::AppendOffsetsToBuffer(std::vector<Struct> &arr, BufferFmt *pBufFmt)
{
    for (auto &s : arr)
    {
        std::string ident = MakeCppIdentifier(s.Name);
        pBufFmt->append("// {}  Size: 0x{:X} (Inherited: 0x{:X})\nnamespace {}\n{{\n",
                        s.FullName, s.Size, s.Inherited, ident);
        pBufFmt->append("\tconstexpr ::std::uintptr_t __Size      = 0x{:X};\n", s.Size);
        pBufFmt->append("\tconstexpr ::std::uintptr_t __Inherited = 0x{:X};\n", s.Inherited);

        if (!s.Members.empty())
            pBufFmt->append("\n");

        for (auto &m : s.Members)
        {
            std::string mid = MakeCppIdentifier(m.Name);
            pBufFmt->append("\tconstexpr ::std::uintptr_t {} = 0x{:X}; // {} (Size: 0x{:X})\n",
                            mid, m.Offset, m.Type, m.Size);
        }

        if (!s.Functions.empty())
        {
            pBufFmt->append("\n\tnamespace Functions\n\t{{\n");
            uintptr_t baseAddr = UEWrappers::GetUEVars()->GetBaseAddress();
            for (auto &f : s.Functions)
            {
                std::string fid = MakeCppIdentifier(f.Name);
                uintptr_t funcOff = (f.Func && baseAddr && f.Func >= baseAddr) ? (f.Func - baseAddr) : 0;
                pBufFmt->append("\t\tconstexpr ::std::uintptr_t {} = 0x{:X}; // {}\n",
                                fid, funcOff, f.FullName);
            }
            pBufFmt->append("\t}}\n");
        }

        pBufFmt->append("}}\n\n");
    }
}

bool UE_UPackage::AppendOffsetsToBuffer(BufferFmt *offsetBufFmt)
{
    if (!offsetBufFmt)
        return false;

    if (!Classes.size() && !Structures.size())
        return false;

    if (Structures.size())
    {
        offsetBufFmt->append("// Package: {}\n// Structs: {}\n\n", GetObject().GetName(), Structures.size());
        UE_UPackage::AppendOffsetsToBuffer(Structures, offsetBufFmt);
    }

    if (Classes.size())
    {
        offsetBufFmt->append("// Package: {}\n// Classes: {}\n\n", GetObject().GetName(), Classes.size());
        UE_UPackage::AppendOffsetsToBuffer(Classes, offsetBufFmt);
    }

    return true;
}
