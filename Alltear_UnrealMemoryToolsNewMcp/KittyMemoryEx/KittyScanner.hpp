#pragma once

#include "KittyUtils.hpp"
#include "KittyMemoryEx.hpp"
#include "KittyMemOp.hpp"

#include <unordered_map>
#include <functional>
#include <utility>

class KittyScannerMgr
{
private:
    IKittyMemOp *_pMem;

public:
    KittyScannerMgr() : _pMem(nullptr) {}
    KittyScannerMgr(IKittyMemOp *pMem) : _pMem(pMem) {}

    /**
     * Search for bytes within a memory range and return all results
     *
     * @param start: search start address
     * @param end: search end address
     * @param bytes: bytes to search
     * @param mask: bytes mask x/?
     *
     * @return vector list of all found bytes addresses
     */
    std::vector<uintptr_t> findBytesAll(const uintptr_t start, const uintptr_t end, const char *bytes, const std::string &mask) const;

    /**
     * Search for bytes within a memory range and return first result
     *
     * @param start: search start address
     * @param end: search end address
     * @param bytes: bytes to search
     * @param mask: bytes mask x/?
     *
     * @return first found bytes address
     */
    uintptr_t findBytesFirst(const uintptr_t start, const uintptr_t end, const char *bytes, const std::string &mask) const;

    /**
     * Search for hex within a memory range and return all results
     *
     * @param start: search start address
     * @param end: search end address
     * @param hex: hex to search
     * @param mask: hex mask x/?
     *
     * @return vector list of all found hex addresses
     */
    std::vector<uintptr_t> findHexAll(const uintptr_t start, const uintptr_t end, std::string hex, const std::string &mask) const;

    /**
     * Search for hex within a memory range and return first result
     *
     * @param start: search start address
     * @param end: search end address
     * @param hex: hex to search
     * @param mask: hex mask x/?
     *
     * @return first found hex address
     */
    uintptr_t findHexFirst(const uintptr_t start, const uintptr_t end, std::string hex, const std::string &mask) const;

    /**
     * Search for ida pattern within a memory range and return all results
     *
     * @param start: search start address
     * @param end: search end address
     * @param pattern: hex bytes and wildcard "?" ( FF DD ? 99 CC ? 00 )
     *
     * @return vector list of all found pattern addresses
     */
    std::vector<uintptr_t> findIdaPatternAll(const uintptr_t start, const uintptr_t end, const std::string &pattern);

    /**
     * Search for ida pattern within a memory range and return first result
     *
     * @param start: search start address
     * @param end: search end address
     * @param pattern: hex bytes and wildcard "?" ( FF DD ? 99 CC ? 00 )
     *
     * @return first found pattern address
     */
    uintptr_t findIdaPatternFirst(const uintptr_t start, const uintptr_t end, const std::string& pattern);

    /**
     * Search for data within a memory range and return all results
     *
     * @param start: search start address
     * @param end: search end address
     * @param data: data to search
     * @param size: data size
     *
     * @return vector list of all found data addresses
     */
    std::vector<uintptr_t> findDataAll(const uintptr_t start, const uintptr_t end, const void *data, size_t size) const;

    /**
     * Search for data within a memory range and return first result
     *
     * @param start: search start address
     * @param end: search end address
     * @param data: data to search
     * @param size: data size
     *
     * @return first found data address
     */
    uintptr_t findDataFirst(const uintptr_t start, const uintptr_t end, const void *data, size_t size) const;
};

#ifdef __ANDROID__
#define kSOINFO_BUFFER_SZ 0x200
struct soinfo_info_t
{
    uintptr_t base = 0;
    size_t size = 0;
    uintptr_t phdr = 0;
    size_t phnum = 0;
    uintptr_t dyn = 0;
    uintptr_t strtab = 0;
    uintptr_t symtab = 0;
    size_t strsz = 0;
    uintptr_t bias = 0;
    uintptr_t next = 0;
    std::string path;
    std::string realpath;
};
#endif

class ElfScanner
{
    friend class ElfScannerMgr;

protected:
    IKittyMemOp *_pMem;
    uintptr_t _elfBase;
    KT_ElfW(Ehdr) _ehdr;
    uintptr_t _phdr;
    std::vector<KT_ElfW(Phdr)> _phdrs;
    int _loads;
    uintptr_t _loadBias, _loadSize;
    uintptr_t _dynamic;
    std::vector<KT_ElfW(Dyn)> _dynamics;
    uintptr_t _stringTable, _symbolTable;
    size_t _strsz, _syment;
    bool _headerless;
    KittyMemoryEx::ProcMap _base_segment;
    std::vector<KittyMemoryEx::ProcMap> _segments;
    std::vector<KittyMemoryEx::ProcMap> _bss_segments;
    std::string _filepath;
    std::string _realpath;
    bool _symbols_init;
    bool _dsymbols_init;
    std::unordered_map<std::string, uintptr_t> _symbolsMap;
    std::unordered_map<std::string, uintptr_t> _dsymbolsMap;

public:
    ElfScanner() : _pMem(nullptr), _elfBase(0), _phdr(0), _loads(0), _loadBias(0), _loadSize(0),
                   _dynamic(0), _stringTable(0), _symbolTable(0), _strsz(0), _syment(0), _headerless(false), _symbols_init(false), _dsymbols_init(false) {}
    ElfScanner(IKittyMemOp *pMem, uintptr_t elfBase);

#ifdef __ANDROID__
    ElfScanner(IKittyMemOp *pMem, const soinfo_info_t &soinfo);
#endif

    inline bool isValid() const
    {
        return _elfBase && _loadSize && _phdr && _dynamic && _loadBias && _stringTable && _symbolTable;
    }

    inline bool isHeaderless() const { return _headerless; }

    inline uintptr_t base() const { return _elfBase; }

    inline uintptr_t end() const { return _elfBase + _loadSize; }

    inline KT_ElfW(Ehdr) header() const { return _ehdr; }

    inline uintptr_t phdr() const { return _phdr; }

    inline std::vector<KT_ElfW(Phdr)> programHeaders() const { return _phdrs; }

    inline int loads() const { return _loads; }

    inline uintptr_t loadBias() const { return _loadBias; }

    inline uintptr_t loadSize() const { return _loadSize; }

    inline uintptr_t dynamic() const { return _dynamic; }

    inline std::vector<KT_ElfW(Dyn)> dynamics() const { return _dynamics; }

    inline uintptr_t stringTable() const { return _stringTable; }

    inline uintptr_t symbolTable() const { return _symbolTable; }

    inline size_t stringTableSize() const { return _strsz; }

    inline size_t symbolEntrySize() const { return _syment; }

    // dynamic symbols from DT_SYMTAB
    std::unordered_map<std::string, uintptr_t> symbols();

    // debug symbols from SHT_SYMTAB on disk
    std::unordered_map<std::string, uintptr_t> dsymbols();

    uintptr_t findSymbol(const std::string &symbolName);
    uintptr_t findDebugSymbol(const std::string &symbolName);

    inline KittyMemoryEx::ProcMap baseSegment() const { return _base_segment; }

    inline std::vector<KittyMemoryEx::ProcMap> segments() const { return _segments; }

    inline std::vector<KittyMemoryEx::ProcMap> bssSegments() const { return _bss_segments; }

    inline std::string filePath() const { return _filepath; }
    inline std::string realPath() const { return _realpath; }
    inline bool isZipped() const { return _base_segment.offset != 0; }
};

class ElfScannerMgr
{
private:
    IKittyMemOp *_pMem;

public:
    ElfScannerMgr() : _pMem(nullptr) {}
    ElfScannerMgr(IKittyMemOp *pMem) : _pMem(pMem) {}

    inline ElfScanner createWithBase(uintptr_t elfBase) const
    {
        return !_pMem ? ElfScanner() : ElfScanner(_pMem, elfBase);
    }
    inline ElfScanner createWithMap(const KittyMemoryEx::ProcMap &map) const
    {
        return !_pMem ? ElfScanner() : ElfScanner(_pMem, map.startAddress);
    }
#ifdef __ANDROID__
    inline ElfScanner createWithSoInfo(const soinfo_info_t &soinfo) const
    {
        return !_pMem ? ElfScanner() : ElfScanner(_pMem, soinfo);
    }
#endif
};

#ifdef __ANDROID__
class LinkerScanner : public ElfScanner
{
private:
struct
    {
        uintptr_t solist;
        uintptr_t somain;
        uintptr_t sonext;
    } _linker_syms;
    struct
    {
        uintptr_t base;
        uintptr_t size;
        uintptr_t phdr;
        uintptr_t phnum;
        uintptr_t dyn;
        uintptr_t strtab;
        uintptr_t symtab;
        uintptr_t strsz;
        uintptr_t bias;
        uintptr_t next;
    } _soinfo_offsets;

public:
    LinkerScanner() : ElfScanner()
    {
        memset(&_linker_syms, 0, sizeof(_linker_syms));
        memset(&_soinfo_offsets, 0, sizeof(_soinfo_offsets));
    }

    LinkerScanner(IKittyMemOp *pMem, uintptr_t linkerBase);

    inline ElfScanner *asELF() const { return (ElfScanner*)this; }

    inline uintptr_t solist() const
    {
        if (!isValid()) return 0;

        uintptr_t value = 0;
        return _pMem->Read(_linker_syms.solist, &value, sizeof(uintptr_t)) == sizeof(uintptr_t) ? value : 0;
    }

    inline uintptr_t somain() const
    {
        if (!isValid()) return 0;

        uintptr_t value = 0;
        return _pMem->Read(_linker_syms.somain, &value, sizeof(uintptr_t)) == sizeof(uintptr_t) ? value : 0;
    }

    inline uintptr_t sonext() const
    {
        if (!isValid()) return 0;

        uintptr_t value = 0;
        return _pMem->Read(_linker_syms.sonext, &value, sizeof(uintptr_t)) == sizeof(uintptr_t) ? value : 0;
    }

    inline soinfo_info_t GetSoMainInfo() const
    {
        if (!isValid()) return {};

        return GetInfoFromSoInfo_(somain(), KittyMemoryEx::getAllMaps(_pMem->processID()));
    }

    inline soinfo_info_t GetSoNextInfo() const
    {
        if (!isValid()) return {};

        return GetInfoFromSoInfo_(sonext(), KittyMemoryEx::getAllMaps(_pMem->processID()));
    }

    std::vector<soinfo_info_t> GetSoList() const;

private:
    soinfo_info_t GetInfoFromSoInfo_(uintptr_t si, const std::vector<KittyMemoryEx::ProcMap> &maps) const;
};
#endif