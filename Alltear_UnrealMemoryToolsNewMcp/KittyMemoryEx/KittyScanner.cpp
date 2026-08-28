#include "KittyScanner.hpp"
#include "KittyMemoryEx.hpp"
#include "zip/zip.h"

// refs
// https://github.com/learn-more/findpattern-bench

static bool compare(const char *data, const char *pattern, const char *mask)
{
    for (; *mask; ++mask, ++data, ++pattern)
    {
        if (*mask == 'x' && *data != *pattern)
            return false;
    }
    return !*mask;
}

static uintptr_t findInRange(const uintptr_t start, const uintptr_t end,
                             const char *pattern, const std::string &mask)
{
    const size_t scan_size = mask.length();

    if (scan_size < 1 || ((start + scan_size) > end))
        return 0;

    const size_t length = end - start;

    for (size_t i = 0; i < length; ++i)
    {
        const uintptr_t current_end = start + i + scan_size;
        if (current_end > end)
            break;

        if (!compare(reinterpret_cast<const char *>(start + i), pattern, mask.c_str()))
            continue;

        return start + i;
    }
    return 0;
}

std::vector<uintptr_t> KittyScannerMgr::findBytesAll(const uintptr_t start, const uintptr_t end,
                                                     const char *bytes, const std::string &mask) const
{
    std::vector<uintptr_t> local_list;

    if (!_pMem || start >= end || !bytes || mask.empty())
        return local_list;

    std::vector<char> buf(end - start, 0);
    if (!_pMem->Read(start, &buf[0], buf.size()))
    {
        KITTY_LOGE("findBytesAll: failed to read into buffer.");
        return local_list;
    }

    uintptr_t curr_search_address = (uintptr_t)&buf[0];
    const size_t scan_size = mask.length();
    do
    {
        if (!local_list.empty())
            curr_search_address = local_list.back() + scan_size;

        uintptr_t found = findInRange(curr_search_address, (uintptr_t(&buf[0]) + buf.size()), bytes, mask);
        if (!found)
            break;

        local_list.push_back(found);
    } while (true);

    if (local_list.empty())
        return local_list;

    std::vector<uintptr_t> remote_list;
    for (auto &it : local_list)
    {
        remote_list.push_back((it - (uintptr_t(&buf[0]))) + start);
    }

    return remote_list;
}

uintptr_t KittyScannerMgr::findBytesFirst(const uintptr_t start, const uintptr_t end, const char *bytes, const std::string &mask) const
{
    if (!_pMem || start >= end || !bytes || mask.empty())
        return 0;

    std::vector<char> buf(end - start, 0);
    if (!_pMem->Read(start, &buf[0], buf.size()))
    {
        KITTY_LOGE("findBytesFirst: failed to read into buffer.");
        return 0;
    }

    uintptr_t local = findInRange((uintptr_t)&buf[0], (uintptr_t(&buf[0]) + buf.size()), bytes, mask);
    if (local)
        return (local - (uintptr_t(&buf[0]))) + start;

    return 0;
}

std::vector<uintptr_t> KittyScannerMgr::findHexAll(const uintptr_t start, const uintptr_t end, std::string hex, const std::string &mask) const
{
    std::vector<uintptr_t> list;

    if (!_pMem || start >= end || mask.empty() || !KittyUtils::String::ValidateHex(hex))
        return list;

    const size_t scan_size = mask.length();
    if ((hex.length() / 2) != scan_size)
        return list;

    std::vector<char> pattern(scan_size);
    KittyUtils::dataFromHex(hex, &pattern[0]);

    list = findBytesAll(start, end, pattern.data(), mask);
    return list;
}

uintptr_t KittyScannerMgr::findHexFirst(const uintptr_t start, const uintptr_t end, std::string hex, const std::string &mask) const
{
    if (!_pMem || start >= end || mask.empty() || !KittyUtils::String::ValidateHex(hex))
        return 0;

    const size_t scan_size = mask.length();
    if ((hex.length() / 2) != scan_size)
        return 0;

    std::vector<char> pattern(scan_size);
    KittyUtils::dataFromHex(hex, &pattern[0]);

    return findBytesFirst(start, end, pattern.data(), mask);
}

std::vector<uintptr_t> KittyScannerMgr::findIdaPatternAll(const uintptr_t start, const uintptr_t end, const std::string &pattern)
{
    std::vector<uintptr_t> list;

    if (!_pMem || start >= end)
        return list;

    std::string mask;
    std::vector<char> bytes;

    const size_t pattren_len = pattern.length();
    for (std::size_t i = 0; i < pattren_len; i++)
    {
        if (pattern[i] == ' ') continue;

        if (pattern[i] == '?')
        {
            bytes.push_back(0);
            mask += '?';
        }
        else if (pattren_len > i + 1 && std::isxdigit(pattern[i]) && std::isxdigit(pattern[i + 1]))
        {
            bytes.push_back(std::stoi(pattern.substr(i++, 2), nullptr, 16));
            mask += 'x';
        }
    }

    if (bytes.empty() || mask.empty() || bytes.size() != mask.size())
        return list;

    list = findBytesAll(start, end, bytes.data(), mask);
    return list;
}

uintptr_t KittyScannerMgr::findIdaPatternFirst(const uintptr_t start, const uintptr_t end, const std::string &pattern)
{
    if (!_pMem || start >= end)
        return 0;

    std::string mask;
    std::vector<char> bytes;

    const size_t pattren_len = pattern.length();
    for (std::size_t i = 0; i < pattren_len; i++)
    {
        if (pattern[i] == ' ') continue;

        if (pattern[i] == '?')
        {
            bytes.push_back(0);
            mask += '?';
        }
        else if (pattren_len > i + 1 && std::isxdigit(pattern[i]) && std::isxdigit(pattern[i + 1]))
        {
            bytes.push_back(std::stoi(pattern.substr(i++, 2), nullptr, 16));
            mask += 'x';
        }
    }

    if (bytes.empty() || mask.empty() || bytes.size() != mask.size())
        return 0;

    return findBytesFirst(start, end, bytes.data(), mask);
}

std::vector<uintptr_t> KittyScannerMgr::findDataAll(const uintptr_t start, const uintptr_t end, const void *data, size_t size) const
{
    std::vector<uintptr_t> list;

    if (!_pMem || start >= end || !data || size < 1)
        return list;

    std::string mask(size, 'x');

    list = findBytesAll(start, end, (const char *)data, mask);
    return list;
}

uintptr_t KittyScannerMgr::findDataFirst(const uintptr_t start, const uintptr_t end, const void *data, size_t size) const
{
    if (!_pMem || start >= end || !data || size < 1)
        return 0;

    std::string mask(size, 'x');

    return findBytesFirst(start, end, (const char *)data, mask);
}

/* ======================= ElfScanner ======================= */

// refs https://gist.github.com/resilar/24bb92087aaec5649c9a2afc0b4350c8

ElfScanner::ElfScanner(IKittyMemOp *pMem, uintptr_t elfBase)
{
    _pMem = nullptr;
    _elfBase = 0;
    _ehdr = {};
    _phdr = 0;
    _loads = 0;
    _loadBias = 0;
    _loadSize = 0;
    _dynamic = 0;
    _stringTable = 0;
    _symbolTable = 0;
    _strsz = 0;
    _syment = 0;
    _headerless = false;
    _symbols_init = false;
    _dsymbols_init = false;

    if (!pMem || !elfBase)
        return;

    _pMem = pMem;
    _elfBase = elfBase;

    // read ELF header
    if (!_pMem->Read(elfBase, &_ehdr, sizeof(_ehdr)))
    {
        KITTY_LOGD("ElfScanner: failed to read ELF (%p) header.", (void *)elfBase);
        return;
    }

    // verify ELF header
    if (memcmp(_ehdr.e_ident, "\177ELF", 4) != 0)
    {
        KITTY_LOGD("ElfScanner: (%p) is not a valid ELF.", (void *)elfBase);
        return;
    }

    // check ELF bit
    if (_ehdr.e_ident[EI_CLASS] != KT_ELF_EICLASS)
    {
        KITTY_LOGD("ElfScanner: ELF class mismatch (%p).", (void *)elfBase);
        return;
    }

    // check common header values
    if (!_ehdr.e_phnum || !_ehdr.e_phentsize || !_ehdr.e_shnum || !_ehdr.e_shentsize)
    {
        KITTY_LOGD("ElfScanner: Invalid header values (%p).", (void *)elfBase);
        return;
    }

    _phdr = elfBase + _ehdr.e_phoff;

    // read all program headers
    std::vector<char> phdrs_buf(_ehdr.e_phnum * _ehdr.e_phentsize);
    if (!_pMem->Read(_phdr, &phdrs_buf[0], phdrs_buf.size()))
    {
        KITTY_LOGD("ElfScanner: failed to read ELF (%p) program headers.", (void *)elfBase);
        return;
    }

    // find load bias
    uintptr_t min_vaddr = UINTPTR_MAX, max_vaddr = 0;
    uintptr_t load_vaddr = 0, load_memsz = 0, load_filesz = 0;
    for (KT_ElfW(Half) i = 0; i < _ehdr.e_phnum; i++)
    {
        KT_ElfW(Phdr) phdr_entry = {};
        memcpy(&phdr_entry, phdrs_buf.data() + (i * _ehdr.e_phentsize), _ehdr.e_phentsize);
        _phdrs.push_back(phdr_entry);

        if (phdr_entry.p_type == PT_LOAD)
        {
            _loads++;

            load_vaddr = phdr_entry.p_vaddr;
            load_memsz = phdr_entry.p_memsz;
            load_filesz = phdr_entry.p_filesz;

            if (phdr_entry.p_vaddr < min_vaddr)
                min_vaddr = phdr_entry.p_vaddr;

            if (phdr_entry.p_vaddr + phdr_entry.p_memsz > max_vaddr)
                max_vaddr = phdr_entry.p_vaddr + phdr_entry.p_memsz;
        }
    }

    if (!_loads)
    {
        KITTY_LOGD("ElfScanner: No loads entry for ELF (%p).", (void *)elfBase);
        return;
    }

    if (!max_vaddr)
    {
        KITTY_LOGD("ElfScanner: failed to find load size for ELF (%p).", (void *)elfBase);
        return;
    }

    min_vaddr = KT_PAGE_START(min_vaddr);
    max_vaddr = KT_PAGE_END(max_vaddr);

    _loadBias = elfBase - min_vaddr;
    _loadSize = max_vaddr - min_vaddr;

    uintptr_t seg_start = load_vaddr + _loadBias;
    uintptr_t seg_mem_end = KT_PAGE_END((seg_start + load_memsz));
    uintptr_t seg_file_end = KT_PAGE_END((seg_start + load_filesz));
    uintptr_t bss_start = 0, bss_end = 0;
    if (seg_mem_end > seg_file_end)
    {
        bss_start = seg_file_end;
        bss_end = seg_mem_end;
    }

    // read all dynamics
    for (auto &phdr : _phdrs)
    {
        if (phdr.p_type == PT_DYNAMIC)
        {
            _dynamic = _loadBias + phdr.p_vaddr;
            std::vector<KT_ElfW(Dyn)> dyn_buff(phdr.p_memsz / sizeof(KT_ElfW(Dyn)));
            if (!_pMem->Read(_dynamic, &dyn_buff[0], phdr.p_memsz))
            {
                KITTY_LOGD("ElfScanner: failed to read dynamic for ELF (%p).", (void *)elfBase);
                break;
            }

            for (auto &dyn : dyn_buff)
            {
                if (dyn.d_tag == DT_NULL)
                    break;

                // set required dynamics for symbol lookup
                switch (dyn.d_tag)
                {
                    // mandatory
                case DT_STRTAB:  // string table
                    _stringTable = dyn.d_un.d_ptr;
                    break;
                    // mandatory
                case DT_SYMTAB:  // symbol table
                    _symbolTable = dyn.d_un.d_ptr;
                    break;
                    // mandatory
                case DT_STRSZ:  // string table size
                    _strsz = dyn.d_un.d_val;
                    break;
                    // mandatory
                case DT_SYMENT:  // symbol entry size
                    _syment = dyn.d_un.d_val;
                    break;
                default:
                    break;
                }

                _dynamics.push_back(dyn);
            }
        }
    }

    // check required dynamics for symbol lookup
    if (!_elfBase || !_loadSize || !_phdr || !_dynamic || !_loadBias || !_stringTable || !_symbolTable)
    {
        KITTY_LOGD("ElfScanner: Failed to require dynamics for symbol lookup.");
        KITTY_LOGD("ElfScanner: elfBase: %p | bias: %p | phdr: %p | dyn: %p | strtab=%p | symtab=%p | strsz=%p | syment=%p",
                   (void *)_elfBase, (void *)_loadBias, (void *)_phdr, (void *)_dynamic, (void *)_stringTable, (void *)_symbolTable, (void *)_strsz, (void *)_syment);
        return;
    }

    auto fix_table_address = [&](uintptr_t &table_addr)
    {
        if (table_addr && table_addr < _loadBias)
            table_addr += _loadBias;
    };

    fix_table_address(_stringTable);
    fix_table_address(_symbolTable);

    if (_syment == 0)
    {
        _syment = sizeof(KT_ElfW(Sym));
    }

    if (_loadSize)
    {
        auto p_maps = KittyMemoryEx::getAllMaps(_pMem->processID());
        for (auto &it : p_maps)
        {
            if (it.startAddress >= _elfBase && it.endAddress <= (_elfBase + _loadSize))
            {
                _segments.push_back(it);
            }

            if (it.endAddress >= (_elfBase + _loadSize))
                break;
        }

        if (!_segments.empty())
        {
            _base_segment = _segments.front();
            _filepath = _base_segment.pathname;
            _realpath = _base_segment.pathname;
            if (!_base_segment.pathname.empty() && _base_segment.offset != 0)
            {
                std::string inZipPath = KittyUtils::Zip::GetEntryInfoAtOffset(_base_segment.pathname, _base_segment.offset).name;
                if (!inZipPath.empty())
                {
                    _realpath += '!';
                    _realpath += inZipPath;
                }
            }

            if (bss_start && bss_end)
            {
                for (const auto &it : _segments)
                {
                    if (it.startAddress >= bss_start && it.endAddress <= bss_end)
                    {
                        _bss_segments.push_back(it);
                    }
                }
            }

            if (_bss_segments.empty())
            {
                for (const auto &it : _segments)
                {
                    if (it.pathname == "[anon:.bss]")
                    {
                        _bss_segments.push_back(it);
                    }
                }
            }
        }
    }
}

#ifdef __ANDROID__
ElfScanner::ElfScanner(IKittyMemOp *pMem, const soinfo_info_t &soinfo)
{
    _pMem = nullptr;
    _elfBase = 0;
    _ehdr = {};
    _phdr = 0;
    _loads = 0;
    _loadBias = 0;
    _loadSize = 0;
    _dynamic = 0;
    _stringTable = 0;
    _symbolTable = 0;
    _strsz = 0;
    _syment = 0;
    _headerless = false;
    _symbols_init = false;
    _dsymbols_init = false;

    if (!pMem)
        return;

    _pMem = pMem;
    _elfBase = soinfo.base;
    _phdr = soinfo.phdr;
    _loadBias = soinfo.bias;
    _loadSize = soinfo.size;
    _dynamic = soinfo.dyn;
    _stringTable = soinfo.strtab;
    _symbolTable = soinfo.symtab;
    _strsz = soinfo.strsz;
    _syment = sizeof(KT_ElfW(Sym));
    _filepath = soinfo.path;
    _realpath = soinfo.realpath;

    bool isLinker = KittyUtils::String::EndsWith(soinfo.path, "/linker") || KittyUtils::String::EndsWith(soinfo.path, "/linker64");
    if ((!isLinker && (_elfBase == 0 || _loadSize == 0)) ||  _loadBias == 0 || _phdr == 0 || _dynamic == 0 || _stringTable == 0 || _symbolTable == 0)
    {
        KITTY_LOGD("ElfScanner: Failed to require dynamics for symbol lookup");
        KITTY_LOGD("ElfScanner: elfBase: %p | bias: %p | phdr: %p | dyn: %p | strtab=%p | symtab=%p | strsz=%p | syment=%p",
                   (void *)_elfBase, (void *)_loadBias, (void *)_phdr, (void *)_dynamic, (void *)_stringTable, (void *)_symbolTable, (void *)_strsz, (void *)_syment);
        return;
    }

    auto p_maps = KittyMemoryEx::getAllMaps(_pMem->processID());

    // fix for linker
    if (_elfBase == 0)
    {
        _elfBase = KittyMemoryEx::getAddressMap(p_maps, soinfo.bias).startAddress;
    }

    uintptr_t bss_start = 0, bss_end = 0;

    do
    {
        // read ELF header
        if (!_pMem->Read(_elfBase, &_ehdr, sizeof(_ehdr)))
        {
            _headerless = true;
            KITTY_LOGD("ElfScanner: failed to read ELF header for soinfo(%p).", (void *)_elfBase);
            break;
        }

        // verify ELF header
        if (memcmp(_ehdr.e_ident, "\177ELF", 4) != 0)
        {
            // maybe protected like pubgm and farlight
            _headerless = true;
            KITTY_LOGD("ElfScanner: soinfo(%p) is not a valid ELF.", (void *)_elfBase);
            break;
        }

        // check ELF bit
        if (_ehdr.e_ident[EI_CLASS] != KT_ELF_EICLASS)
        {
            KITTY_LOGD("ElfScanner: ELF class mismatch (%p).", (void *)_elfBase);
            break;
        }

        // check common header values
        if (!_ehdr.e_phnum || !_ehdr.e_phentsize || !_ehdr.e_shnum || !_ehdr.e_shentsize)
        {
            KITTY_LOGD("ElfScanner: Invalid header values (%p).", (void *)_elfBase);
            break;
        }

        // read all program headers
        std::vector<char> phdrs_buf(_ehdr.e_phnum * _ehdr.e_phentsize);
        if (!_pMem->Read(_phdr, &phdrs_buf[0], phdrs_buf.size()))
        {
            KITTY_LOGD("ElfScanner: failed to read ELF (%p) program headers.", (void *)_elfBase);
            break;
        }

        uintptr_t min_vaddr = UINTPTR_MAX, max_vaddr = 0;
        uintptr_t load_vaddr = 0, load_memsz = 0, load_filesz = 0;
        for (KT_ElfW(Half) i = 0; i < _ehdr.e_phnum; i++)
        {
            KT_ElfW(Phdr) phdr_entry = {};
            memcpy(&phdr_entry, phdrs_buf.data() + (i * _ehdr.e_phentsize), _ehdr.e_phentsize);
            _phdrs.push_back(phdr_entry);

            if (phdr_entry.p_type == PT_LOAD)
            {
                _loads++;

                load_vaddr = phdr_entry.p_vaddr;
                load_memsz = phdr_entry.p_memsz;
                load_filesz = phdr_entry.p_filesz;

                if (phdr_entry.p_vaddr < min_vaddr)
                    min_vaddr = phdr_entry.p_vaddr;

                if (phdr_entry.p_vaddr + phdr_entry.p_memsz > max_vaddr)
                    max_vaddr = phdr_entry.p_vaddr + phdr_entry.p_memsz;
            }
        }

        if (!_loads)
        {
            KITTY_LOGD("ElfScanner: No loads entry for ELF (%p).", (void *)_elfBase);
            break;
        }

        if (!max_vaddr)
        {
            KITTY_LOGD("ElfScanner: failed to find load size for ELF (%p).", (void *)_elfBase);
            break;
        }

        min_vaddr = KT_PAGE_START(min_vaddr);
        max_vaddr = KT_PAGE_END(max_vaddr);

        // fix for linker
        if (_loadSize == 0)
        {
            _loadSize = max_vaddr - min_vaddr;
        }

        uintptr_t seg_start = load_vaddr + _loadBias;
        uintptr_t seg_mem_end = KT_PAGE_END((seg_start + load_memsz));
        uintptr_t seg_file_end = KT_PAGE_END((seg_start + load_filesz));
        if (seg_mem_end > seg_file_end)
        {
            bss_start = seg_file_end;
            bss_end = seg_mem_end;
        }

        // read all dynamics
        for (auto &phdr : _phdrs)
        {
            if (phdr.p_type == PT_DYNAMIC)
            {
                std::vector<KT_ElfW(Dyn)> dyn_buff(phdr.p_memsz / sizeof(KT_ElfW(Dyn)));
                if (!_pMem->Read(_dynamic, &dyn_buff[0], phdr.p_memsz))
                {
                    KITTY_LOGD("ElfScanner: failed to read dynamic for ELF (%p).", (void *)_elfBase);
                    break;
                }

                for (auto &dyn : dyn_buff)
                {
                    if (dyn.d_tag == DT_NULL)
                        break;

                    switch (dyn.d_tag)
                    {
                    case DT_STRSZ:
                        _strsz = dyn.d_un.d_val;
                        break;
                    case DT_SYMENT:
                        _syment = dyn.d_un.d_val;
                        break;
                    default:
                        break;
                    }

                    _dynamics.push_back(dyn);
                }
            }
        }
    } while (false);

    if (_loadSize)
    {
        for (auto &it : p_maps)
        {
            if (it.startAddress >= _elfBase && it.endAddress <= (_elfBase + _loadSize))
            {
                _segments.push_back(it);
            }

            if (it.endAddress >= (_elfBase + _loadSize))
                break;
        }

        if (!_segments.empty())
        {
            _base_segment = _segments.front();

            if (bss_start && bss_end)
            {
                for (const auto &it : _segments)
                {
                    if (it.startAddress >= bss_start && it.endAddress <= bss_end)
                    {
                        _bss_segments.push_back(it);
                    }
                }
            }

            if (_bss_segments.empty())
            {
                for (const auto &it : _segments)
                {
                    if (it.pathname == "[anon:.bss]")
                    {
                        _bss_segments.push_back(it);
                    }
                }
            }
        }
    }

    if (!_elfBase || !_loadSize || !_phdr || !_dynamic || !_loadBias || !_stringTable || !_symbolTable)
    {
        KITTY_LOGD("ElfScanner: Failed to require dynamics for symbol lookup.");
        KITTY_LOGD("ElfScanner: elfBase: %p | bias: %p | phdr: %p | dyn: %p | strtab=%p | symtab=%p | strsz=%p | syment=%p",
                   (void *)_elfBase, (void *)_loadBias, (void *)_phdr, (void *)_dynamic, (void *)_stringTable, (void *)_symbolTable, (void *)_strsz, (void *)_syment);
    }
}
#endif

std::unordered_map<std::string, uintptr_t> ElfScanner::symbols()
{
    if (!_symbols_init && _loadBias && _stringTable && _symbolTable && _strsz && _syment)
    {
        _symbols_init = true;

        auto get_sym_address = [&](const KT_ElfW(Sym) * sym_ent) -> uintptr_t
        {
            return sym_ent->st_value < _loadBias ? _loadBias + sym_ent->st_value : sym_ent->st_value;
        };

        size_t symtab_sz = ((_stringTable > _symbolTable) ? (_stringTable - _symbolTable) : (_symbolTable - _stringTable));
        std::vector<char> symtab_buff(symtab_sz, 0);
        std::vector<char> strtab_buff(_strsz, 0);

        if (_pMem->Read(_symbolTable, symtab_buff.data(), symtab_buff.size()) && _pMem->Read(_stringTable, strtab_buff.data(), strtab_buff.size()))
        {
            uintptr_t sym_start = uintptr_t(symtab_buff.data());
            uintptr_t sym_end = uintptr_t(symtab_buff.data() + symtab_buff.size());
            uintptr_t sym_str_end = uintptr_t(strtab_buff.data() + strtab_buff.size());
            for (auto sym_entry = sym_start; (sym_entry + _syment) < sym_end; sym_entry += _syment)
            {
                const KT_ElfW(Sym) *curr_sym = reinterpret_cast<KT_ElfW(Sym) *>(sym_entry);

                if (curr_sym->st_name >= _strsz)
                    break;

                if (intptr_t(curr_sym->st_name) <= 0 || intptr_t(curr_sym->st_value) <= 0 || intptr_t(curr_sym->st_size) <= 0)
                    continue;

                if (KT_ELF_ST_TYPE(curr_sym->st_info) != STT_OBJECT && KT_ELF_ST_TYPE(curr_sym->st_info) != STT_FUNC)
                    continue;

                uintptr_t sym_str_addr = uintptr_t(strtab_buff.data() + curr_sym->st_name);
                if (!sym_str_addr || sym_str_addr >= sym_str_end)
                    continue;

                std::string sym_str = std::string(reinterpret_cast<const char *>(sym_str_addr));
                if (!sym_str.empty() && sym_str.data())
                    _symbolsMap[sym_str] = get_sym_address(curr_sym);
            }
        }
    }

    return _symbolsMap;
}

std::unordered_map<std::string, uintptr_t> ElfScanner::dsymbols()
{
    if (!_dsymbols_init && _loadBias && !_filepath.empty())
    {
        _dsymbols_init = true;

        auto get_sym_address = [&](const KT_ElfW(Sym) * sym_ent) -> uintptr_t
        {
            return sym_ent->st_value < _loadBias ? _loadBias + sym_ent->st_value : sym_ent->st_value;
        };

        std::pair<void*, size_t> mmap_info = {nullptr, 0};
        if (isZipped())
        {
            mmap_info = KittyUtils::Zip::MMapEntryAtOffset(_filepath, _base_segment.offset);
        }
        else
        {
            KittyIOFile elfFile(_filepath, O_RDONLY);
            if (!elfFile.Open())
            {
                KITTY_LOGD("Failed to open file <%s> err(%s)", _filepath.c_str(), elfFile.lastStrError().c_str());
                return _dsymbolsMap;
            }
            size_t elfSize = elfFile.Stat().st_size;
            if (elfSize <= 0)
            {
                elfFile.Close();
                KITTY_LOGD("stat failed for <%s>", _filepath.c_str());
                return _dsymbolsMap;
            }
            mmap_info.first = mmap(nullptr, elfSize, PROT_READ, MAP_PRIVATE, elfFile.FD(), 0);
            mmap_info.second = elfSize;
            elfFile.Close();
        }

        if (mmap_info.second == 0 || !mmap_info.first || mmap_info.first == ((void *)-1))
        {
            KITTY_LOGD("Failed to mmap <%s>", realPath().c_str());
            return _dsymbolsMap;
        }

        auto cleanup = [&]
        { munmap(mmap_info.first, mmap_info.second); };

        KT_ElfW(Ehdr) *ehdr = static_cast<KT_ElfW(Ehdr) *>(mmap_info.first);

        if (memcmp(ehdr->e_ident, "\177ELF", 4) != 0)
        {
            KITTY_LOGD("<%s> is not a valid ELF", realPath().c_str());
            cleanup();
            return _dsymbolsMap;
        }

        if (ehdr->e_phoff == 0 || ehdr->e_phentsize == 0 || ehdr->e_phnum == 0 || ehdr->e_phoff + ehdr->e_phnum * sizeof(KT_ElfW(Phdr)) > mmap_info.second)
        {
            KITTY_LOGD("Invalid program header table in <%s>", filePath().c_str());
            cleanup();
            return _dsymbolsMap;
        }

        if (ehdr->e_shoff == 0 || ehdr->e_shentsize == 0 || ehdr->e_shnum == 0 || ehdr->e_shoff + ehdr->e_shnum * sizeof(KT_ElfW(Shdr)) > mmap_info.second)
        {
            KITTY_LOGD("Invalid section header table in <%s>", filePath().c_str());
            cleanup();
            return _dsymbolsMap;
        }

        const KT_ElfW(Shdr) *shdr = reinterpret_cast<KT_ElfW(Shdr) *>(static_cast<char *>(mmap_info.first) + ehdr->e_shoff);
        const KT_ElfW(Shdr) *shstrtab_shdr = shdr + ehdr->e_shstrndx;
        const char *sectionstr = reinterpret_cast<char *>(static_cast<char *>(mmap_info.first) + shstrtab_shdr->sh_offset);
        for (uint16_t i = 0; i < ehdr->e_shnum; ++i)
        {
            if (shdr[i].sh_type != SHT_SYMTAB)
                continue;

            std::string section_name = std::string(reinterpret_cast<const char *>(sectionstr + shdr[i].sh_name));
            if (section_name.compare(".symtab") != 0)
                continue;

            if ((shdr[i].sh_offset + shdr[i].sh_size) > mmap_info.second || shdr[i].sh_link >= ehdr->e_shnum ||
                (shdr[shdr[i].sh_link].sh_offset + shdr[shdr[i].sh_link].sh_size) > mmap_info.second)
                continue;

            const KT_ElfW(Sym) *symtab = reinterpret_cast<KT_ElfW(Sym) *>(static_cast<char *>(mmap_info.first) + shdr[i].sh_offset);
            const size_t symCount = shdr[i].sh_size / shdr[i].sh_entsize;
            const KT_ElfW(Shdr) *strtabShdr = &shdr[shdr[i].sh_link];
            const char *strtab = reinterpret_cast<char *>(static_cast<char *>(mmap_info.first) + strtabShdr->sh_offset);

            for (size_t j = 0; j < symCount; ++j)
            {
                const KT_ElfW(Sym) *curr_sym = &symtab[j];
                if (!curr_sym || curr_sym->st_name >= strtabShdr->sh_size)
                    continue;

                if (intptr_t(curr_sym->st_value) <= 0 || intptr_t(curr_sym->st_size) <= 0)
                    continue;

                if (KT_ELF_ST_TYPE(curr_sym->st_info) != STT_OBJECT && KT_ELF_ST_TYPE(curr_sym->st_info) != STT_FUNC)
                    continue;

                std::string sym_str = std::string(reinterpret_cast<const char *>(strtab + curr_sym->st_name));
                if (!sym_str.empty() && sym_str.data())
                    _dsymbolsMap[sym_str] = get_sym_address(curr_sym);
            }
        }
        cleanup();
    }
    return _dsymbolsMap;
}

uintptr_t ElfScanner::findSymbol(const std::string &symbolName)
{
    const auto &syms = symbols();
    auto it = syms.find(symbolName);
    return it != syms.end() ? it->second : 0;
}

uintptr_t ElfScanner::findDebugSymbol(const std::string &symbolName)
{
    const auto &syms = dsymbols();
    auto it = syms.find(symbolName);
    return it != syms.end() ? it->second : 0;
}

#ifdef __ANDROID__
LinkerScanner::LinkerScanner(IKittyMemOp *pMem, uintptr_t linkerBase) : ElfScanner(pMem, linkerBase)
{
    memset(&_linker_syms, 0, sizeof(_linker_syms));
    memset(&_soinfo_offsets, 0, sizeof(_soinfo_offsets));

    if (!pMem || !isValid()) return;

    for (const auto &sym : dsymbols())
    {
        if (KittyUtils::String::StartsWith(sym.first, "__dl__ZL6solist"))
        {
            _linker_syms.solist = sym.second;
            continue;
        }
        if (KittyUtils::String::StartsWith(sym.first, "__dl__ZL6somain"))
        {
            _linker_syms.somain = sym.second;
            continue;
        }
        if (KittyUtils::String::StartsWith(sym.first, "__dl__ZL6sonext"))
        {
            _linker_syms.sonext = sym.second;
            continue;
        }
        if (_linker_syms.solist && _linker_syms.somain && _linker_syms.sonext)
            break;
    }

    std::vector<char> solist_buf(kSOINFO_BUFFER_SZ, 0);
    pMem->Read(solist(), solist_buf.data(), kSOINFO_BUFFER_SZ);

    std::vector<char> si_buf(kSOINFO_BUFFER_SZ, 0);
    uintptr_t somain_ptr = somain();
    pMem->Read(somain_ptr ? somain_ptr : sonext(), si_buf.data(), kSOINFO_BUFFER_SZ);

    ElfScanner si_elf{};
    for (size_t i = 0; i < si_buf.size(); i += sizeof(uintptr_t))
    {
        si_elf = ElfScanner(pMem, *(uintptr_t *)&si_buf[i]);
        if (si_elf.isValid())
        {
            _soinfo_offsets.base = i;
            break;
        }
    }

    for (size_t i = 0; i < si_buf.size(); i += sizeof(uintptr_t))
    {
        uintptr_t value = *(uintptr_t *)&si_buf[i];

        if (value == si_elf.phdr())
            _soinfo_offsets.phdr = i;
        if (value == si_elf.programHeaders().size())
            _soinfo_offsets.phnum = i;
        else if (value == si_elf.loadSize())
            _soinfo_offsets.size = i;
        else if (value == si_elf.dynamic())
            _soinfo_offsets.dyn = i;
        else if (value == si_elf.stringTable())
            _soinfo_offsets.strtab = i;
        else if (value == si_elf.symbolTable())
            _soinfo_offsets.symtab = i;
        else if (value == si_elf.loadBias() && i != _soinfo_offsets.base)
            _soinfo_offsets.bias = i;
        else if (value == si_elf.stringTableSize())
            _soinfo_offsets.strsz = i;
    }

    for (size_t i = 0; i < solist_buf.size(); i += sizeof(uintptr_t))
    {
        uintptr_t value = *(uintptr_t *)&solist_buf[i];

        uintptr_t tmp_base = 0;
        pMem->Read(value + _soinfo_offsets.base, &tmp_base, sizeof(uintptr_t));
        auto tmp_elf = ElfScanner(pMem, tmp_base);
        if (tmp_elf.isValid())
        {
            uintptr_t tmp_size = 0;
            pMem->Read(value + _soinfo_offsets.size, &tmp_size, sizeof(uintptr_t));
            if (tmp_elf.loadSize() == tmp_size)
            {
                _soinfo_offsets.next = i;
                break;
            }
        }
    }

    KITTY_LOGD("soinfo_base(%zx) | soinfo_size(%zx)", _soinfo_offsets.base, _soinfo_offsets.size);
    KITTY_LOGD("soinfo_phdr(%zx, %zx) | soinfo_dyn(%zx)", _soinfo_offsets.phdr, _soinfo_offsets.phnum, _soinfo_offsets.dyn);
    KITTY_LOGD("soinfo_strtab(%zx, %zx) | soinfo_symtab(%zx)", _soinfo_offsets.strtab, _soinfo_offsets.strsz, _soinfo_offsets.symtab);
    KITTY_LOGD("soinfo_bias(%zx) | soinfo_sonext(%zx)", _soinfo_offsets.bias, _soinfo_offsets.next);
}

std::vector<soinfo_info_t> LinkerScanner::GetSoList() const
{
    std::vector<soinfo_info_t> infos{};

    if (!isValid()) return infos;

    auto maps = KittyMemoryEx::getAllMaps(_pMem->processID());
    uintptr_t si = solist();
    while (si)
    {
        soinfo_info_t info = GetInfoFromSoInfo_(si, maps);
        if (!info.bias) break;

        infos.push_back(info);

        if (_pMem->Read(si + _soinfo_offsets.next, &si, sizeof(uintptr_t)) != sizeof(uintptr_t))
            break;
    }
    return infos;
}

soinfo_info_t LinkerScanner::GetInfoFromSoInfo_(uintptr_t si, const std::vector<KittyMemoryEx::ProcMap> &maps) const
{
    soinfo_info_t info{};

    thread_local static std::vector<char> si_buf(kSOINFO_BUFFER_SZ);
    memset(si_buf.data(), 0, si_buf.size());
    if (!_pMem->Read(si, si_buf.data(), kSOINFO_BUFFER_SZ))
        return info;

    auto si_map = KittyMemoryEx::getAddressMap(maps, *(uintptr_t *)&si_buf[_soinfo_offsets.bias]);
    if (si_map.isValid())
    {
        info.base = *(uintptr_t *)&si_buf[_soinfo_offsets.base];
        info.size = *(uintptr_t *)&si_buf[_soinfo_offsets.size];
        info.phdr = *(uintptr_t *)&si_buf[_soinfo_offsets.phdr];
        info.phnum = *(uintptr_t *)&si_buf[_soinfo_offsets.phnum];
        info.dyn = *(uintptr_t *)&si_buf[_soinfo_offsets.dyn];
        info.strtab = *(uintptr_t *)&si_buf[_soinfo_offsets.strtab];
        info.symtab = *(uintptr_t *)&si_buf[_soinfo_offsets.symtab];
        info.strsz = *(uintptr_t *)&si_buf[_soinfo_offsets.strsz];
        info.bias = *(uintptr_t *)&si_buf[_soinfo_offsets.bias];
        info.next = *(uintptr_t *)&si_buf[_soinfo_offsets.next];
        info.path = si_map.pathname;
        info.realpath = si_map.pathname;
        if (si_map.offset != 0)
        {
            std::string inZipPath = KittyUtils::Zip::GetEntryInfoAtOffset(si_map.pathname, si_map.offset).name;
            if (!inZipPath.empty())
            {
                info.realpath += '!';
                info.realpath += inZipPath;
            }
        }
    }

    return info;
}
#endif