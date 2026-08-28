#include "KittyMemoryMgr.hpp"
#include "zip/zip.h"

bool KittyMemoryMgr::initialize(pid_t pid, EKittyMemOP eMemOp, bool initMemPatch)
{
    _init = false;
    _pid = pid;

    if (_pid <= 0)
    {
        KITTY_LOGE("KittyMemoryMgr: Invalid arg (pid).");
        return false;
    }

    _process_name = KittyMemoryEx::getProcessName(_pid);

    if (_pMemOp.get())
        _pMemOp.reset();

    _eMemOp = eMemOp;
    switch (eMemOp)
    {
    case EK_MEM_OP_SYSCALL:
        _pMemOp = std::make_unique<KittyMemSys>();
        break;
    case EK_MEM_OP_IO:
        _pMemOp = std::make_unique<KittyMemIO>();
        break;
    default:
        KITTY_LOGE("KittyMemoryMgr: Unknown memory operation.");
        return false;
    }

    if (!_pMemOp->init(_pid))
    {
        KITTY_LOGE("KittyMemoryMgr: Couldn't initialize memory operation.");
        return false;
    }

    _init = true;

    // patching mem only avaialabe for IO operation
    if (initMemPatch)
    {
        if (eMemOp == EK_MEM_OP_IO)
        {
            memPatch = MemoryPatchMgr(_pMemOp.get());
            memBackup = MemoryBackupMgr(_pMemOp.get());
        }
        else
        {
            if (_pMemOpPatch.get())
                _pMemOpPatch.reset();

            _pMemOpPatch = std::make_unique<KittyMemIO>();
            if (_pMemOpPatch->init(pid))
            {
                memPatch = MemoryPatchMgr(_pMemOpPatch.get());
                memBackup = MemoryBackupMgr(_pMemOpPatch.get());
            }
            else
            {
                KITTY_LOGW("KittyMemoryMgr: Couldn't initialize IO memory operation for memory patching.");
            }
        }
    }

    memScanner = KittyScannerMgr(_pMemOp.get());
    elfScanner = ElfScannerMgr(_pMemOp.get());

#ifdef __ANDROID__
#ifdef __LP64__
    linkerScanner = LinkerScanner(_pMemOp.get(), findMemElf("/linker64").base());
#else
    linkerScanner = LinkerScanner(_pMemOp.get(), findMemElf("/linker").base());
#endif
#endif

#ifdef __ANDROID__
    // refs https://fadeevab.com/shared-library-injection-on-android-8/
    uintptr_t defaultCaller = findMemElf("libRS.so").base();
#else
    uintptr_t defaultCaller = 0;
#endif
    trace = KittyTraceMgr(_pMemOp.get(), defaultCaller);

    return true;
}

size_t KittyMemoryMgr::readMem(uintptr_t address, void *buffer, size_t len) const
{
    if (!isMemValid() || !buffer || !len)
        return 0;

    return _pMemOp->Read(address, buffer, len);
}

size_t KittyMemoryMgr::writeMem(uintptr_t address, void *buffer, size_t len) const
{
    if (!isMemValid() || !buffer || !len)
        return 0;

    return _pMemOp->Write(address, buffer, len);
}

std::string KittyMemoryMgr::readMemStr(uintptr_t address, size_t maxLen) const
{
    if (!isMemValid() || !address || !maxLen)
        return "";

    return _pMemOp->ReadStr(address, maxLen);
}

bool KittyMemoryMgr::writeMemStr(uintptr_t address, std::string str) const
{
    if (!isMemValid() || !address || str.empty())
        return false;

    return _pMemOp->WriteStr(address, str);
}

bool KittyMemoryMgr::isValidELF(uintptr_t elfBase) const
{
    if (!isMemValid() || !elfBase)
        return false;

    char magic[4] = {0};
    return readMem(elfBase, magic, sizeof(magic)) && memcmp(magic, "\177ELF", 4) == 0;
}

ElfScanner KittyMemoryMgr::findMemElf(const std::string &elfName) const
{
    ElfScanner ret{};

    if (!isMemValid() || elfName.empty())
        return ret;

    // sometimes an ELF has two loads
    // the one we should use is the one with more segments than other

    std::vector<ElfScanner> elfs;

    auto maps = KittyMemoryEx::getMapsContain(_pid, elfName);
    for (auto &it : maps)
    {
        if (!isValidELF(it.startAddress))
            continue;

        auto elf = elfScanner.createWithMap(it);
        if (elf.isValid())
            elfs.push_back(elf);
    }

    if (elfs.empty())
        return ret;

    ret = elfs.front();

    if (elfs.size() == 1)
        return ret;

    int nMostSegments = 0;
    for (auto &it : elfs)
    {
        int numSegments = it.segments().size();
        if (numSegments > nMostSegments)
        {
            ret = it;
            nMostSegments = numSegments;
        }
    }

    return ret;
}

ElfScanner KittyMemoryMgr::findMemElfInZip(const std::string& zip, const std::string& elfName) const
{
    // Comparing ELF data offset in zip to the mapped memory offset

    ElfScanner ret{};

    if (!isMemValid() || elfName.empty())
        return ret;

    auto maps = KittyMemoryEx::getMapsEndWith(_pid, zip);
    if (maps.empty())
        return ret;

    auto map = maps.front();

    struct zip_t* z = zip_open(map.pathname.c_str(), 0, 'r');
    if (!z) return ret;

    bool found = false;
    int i, n = zip_entries_total(z);
    for (i = 0; i < n; ++i)
    {
        zip_entry_openbyindex(z, i);
        {
            std::string name = zip_entry_name(z);
            if (KittyUtils::String::EndsWith(name, elfName))
            {
                unsigned long long data_offset = zip_entry_data_offset(z);
                for (auto& it : maps)
                {
                    if (it.inode == map.inode && it.offset == data_offset)
                    {
                        ret = elfScanner.createWithMap(it);
                        found = true;
                        break;
                    }
                }
            }
        }
        zip_entry_close(z);
        if (found) break;
    }

    zip_close(z);

    return ret;
}

#ifdef __ANDROID__
    ElfScanner KittyMemoryMgr::findMemElfFromLinker(const std::string &elfName) const
    {
        if (linkerScanner.isValid())
        {
            const auto solistInfo = linkerScanner.GetSoList();
            for (const auto &it : solistInfo)
            {
                if (KittyUtils::String::EndsWith(it.realpath, elfName))
                {
                    return elfScanner.createWithSoInfo(it);
                }
            }
        }
        return {};
    }
#endif

ElfScanner KittyMemoryMgr::findMemElfProgram() const
{
    if (!isMemValid())
        return {};

    std::string path = KittyUtils::String::Fmt("/proc/%d/exe", _pid);
    char exePath[0xff] = {0};
    int ret = int(readlink(path.c_str(), exePath, 0xff));
    if (ret == -1)
    {
        int err = errno;
        KITTY_LOGE("Failed to readlink \"%s\", error(%d): %s.", path.c_str(), err, strerror(err));
        return {};
    }
    return findMemElf(exePath);
}

uintptr_t KittyMemoryMgr::findRemoteOfSymbol(const local_symbol_t &local_sym) const
{
    if (!isMemValid() || !local_sym.name || !local_sym.address)
        return 0;

    ElfScanner r_lib{};
    ProcMap l_lib{};

    l_lib = KittyMemoryEx::getAddressMap(getpid(), local_sym.address);
    if (l_lib.isValid())
        r_lib = findMemElf(l_lib.pathname);

    if (!r_lib.isValid())
    {
        KITTY_LOGE("KittyInjector: Failed to find %s, remote lib not found.", local_sym.name);
        return 0;
    }
    
    uintptr_t remote_address = r_lib.findSymbol(local_sym.name);
    
    // fallback
    if (!remote_address)
        remote_address = local_sym.address - l_lib.startAddress + r_lib.base();

    return remote_address;
}

bool KittyMemoryMgr::dumpMemRange(uintptr_t start, uintptr_t end, const std::string &destination) const
{
    if (!isMemValid())
        return false;

    if (start >= end)
    {
        KITTY_LOGE("dumpMemRange: start(%p) is equal or greater than end(%p).", (void *)start, (void *)end);
        return false;
    }

    KittyIOFile srcFile(KittyUtils::String::Fmt("/proc/%d/mem", _pid), O_RDONLY);
    if (!srcFile.Open())
    {
        KITTY_LOGE("dumpMemRange: Couldn't open mem file %s, error=%s", srcFile.Path().c_str(), srcFile.lastStrError().c_str());
        return false;
    }

    return srcFile.writeToFile(start, end - start, destination);
}

bool KittyMemoryMgr::dumpMemFile(const std::string &memFile, const std::string &destination) const
{
    if (!isMemValid() || memFile.empty() || destination.empty())
        return false;

    auto fileMaps = KittyMemoryEx::getMapsEndWith(_pid, memFile);
    if (fileMaps.empty())
        return false;

    auto firstMap = fileMaps.front();
    fileMaps.erase(fileMaps.begin());

    uintptr_t lastEnd = firstMap.endAddress;
    if (fileMaps.size() > 1)
    {
        for (auto &it : fileMaps)
        {
            if (firstMap.inode != it.inode || it.startAddress != lastEnd)
                break;

            lastEnd = it.endAddress;
        }
    }

    return dumpMemRange(firstMap.startAddress, lastEnd, destination);
}

bool KittyMemoryMgr::dumpMemELF(const ElfScanner &elf, const std::string &destination) const
{
    if (!isMemValid() || !elf.base() || !elf.loadSize())
        return false;

    return dumpMemRange(elf.base(), elf.end(), destination);
}