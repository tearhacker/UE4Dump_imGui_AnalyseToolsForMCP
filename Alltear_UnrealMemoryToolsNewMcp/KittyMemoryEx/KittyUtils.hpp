#pragma once

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <cstring>
#include <cstdint>
#include <cstdarg>

#include <string>
#include <sstream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <vector>
#include <utility>
#include <map>
#include <random>
#include <functional>

#include <elf.h>
#ifdef __LP64__
#define KT_ELFCLASS_BITS 64
#define KT_ELF_EICLASS 2
#define KT_ElfW(x) Elf64_##x
#define KT_ELFW(x) ELF64_##x
#else
#define KT_ELFCLASS_BITS 32
#define KT_ELF_EICLASS 1
#define KT_ElfW(x) Elf32_##x
#define KT_ELFW(x) ELF32_##x
#endif
#define KT_ELF_ST_BIND(val) (((unsigned char) (val)) >> 4)
#define KT_ELF_ST_TYPE(val)	((val) & 0xf)
#define KT_ELF_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))
#define KT_ELF_ST_VISIBILITY(o)	((o) & 0x03)

#define KT_PAGE_SIZE (sysconf(_SC_PAGE_SIZE))
#define KT_PAGE_START(x) (uintptr_t(x) & ~(KT_PAGE_SIZE - 1))
#define KT_PAGE_END(x) (KT_PAGE_START(uintptr_t(x) + KT_PAGE_SIZE - 1))
#define KT_PAGE_OFFSET(x) (uintptr_t(x) - KT_PAGE_START(x))
#define KT_PAGE_LEN(x) (size_t(KT_PAGE_SIZE - KT_PAGE_OFFSET(x)))

#if defined(__ANDROID__) && defined(kUSE_LOGCAT)

#include <android/log.h>
#define KITTY_LOG_TAG "KittyMemoryEx"

#ifdef kITTYMEMORY_DEBUG
#define KITTY_LOGD(fmt, ...) ((void)__android_log_print(ANDROID_LOG_DEBUG, KITTY_LOG_TAG, fmt, ##__VA_ARGS__))
#else
#define KITTY_LOGD(fmt, ...) \
    do                       \
    {                        \
    } while (0)
#endif

#define KITTY_LOGI(fmt, ...) ((void)__android_log_print(ANDROID_LOG_INFO, KITTY_LOG_TAG, fmt, ##__VA_ARGS__))
#define KITTY_LOGE(fmt, ...) ((void)__android_log_print(ANDROID_LOG_ERROR, KITTY_LOG_TAG, fmt, ##__VA_ARGS__))
#define KITTY_LOGW(fmt, ...) ((void)__android_log_print(ANDROID_LOG_WARN, KITTY_LOG_TAG, fmt, ##__VA_ARGS__))

#else

#ifdef kITTYMEMORY_DEBUG
#define KITTY_LOGD(fmt, ...) printf("D: " fmt "\n", ##__VA_ARGS__)
#else
#define KITTY_LOGD(fmt, ...) \
    do                       \
    {                        \
    } while (0)
#endif

#define KITTY_LOGI(fmt, ...) printf("I: " fmt "\n", ##__VA_ARGS__)
#define KITTY_LOGE(fmt, ...) printf("E: " fmt "\n", ##__VA_ARGS__)
#define KITTY_LOGW(fmt, ...) printf("W: " fmt "\n", ##__VA_ARGS__)

#endif

#define KT_EINTR_RETRY(exp) ({         \
    __typeof__(exp) _rc;                   \
    do {                                   \
        _rc = (exp);                       \
    } while (_rc == -1 && errno == EINTR); \
    _rc; })

struct zip_entry_info_t
{
    std::string name;
    uintptr_t offset;
    ssize_t index;
    uint32_t crc32;
    bool is_dir;
    size_t size, comp_size, uncomp_size;

    zip_entry_info_t() : offset(0), index(0), crc32(0), is_dir(false), size(0), comp_size(0), uncomp_size(0)
    {
    }
    zip_entry_info_t(uint32_t crc32, bool is_dir, std::string name, uintptr_t offset, ssize_t index, size_t size, size_t comp_size, size_t uncomp_size) : offset(offset), index(index), crc32(crc32), is_dir(is_dir), size(size), comp_size(comp_size), uncomp_size(uncomp_size)
    {
    }
};

namespace KittyUtils
{

#ifdef __ANDROID__
    std::string getExternalStorage();
    int getAndroidVersion();
    int getAndroidSDK();
#endif

    std::string fileNameFromPath(const std::string &filePath);
    std::string fileDirectory(const std::string &filePath);
    std::string fileExtension(const std::string &filePath);

    namespace String
    {
        static inline bool StartsWith(const std::string &str, const std::string &str2)
        {
            return str.length() >= str2.length() && str.compare(0, str2.length(), str2) == 0;
        }
        
        static inline bool Contains(const std::string &str, const std::string &str2)
        {
            return str.length() >= str2.length() && str.find(str2) != std::string::npos;
        }
        
        static inline bool EndsWith(const std::string &str, const std::string &str2)
        {
            return str.length() >= str2.length() && str.compare(str.length() - str2.length(), str2.length(), str2) == 0;
        }
        
        void Trim(std::string &str);

        bool ValidateHex(std::string &hex);

        std::string Fmt(const char *fmt, ...);

        std::string Random(size_t length);
    } // namespace String

    template <typename T>
    T randInt(T min, T max)
    {
        using param_type = typename std::uniform_int_distribution<T>::param_type;

        thread_local static std::mt19937 gen{std::random_device{}()};
        thread_local static std::uniform_int_distribution<T> dist;

        return dist(gen, param_type{min, max});
    }

    template <typename T>
    std::string data2Hex(const T &data)
    {
        const auto *byteData = reinterpret_cast<const unsigned char *>(&data);
        std::stringstream hexStringStream;

        hexStringStream << std::hex << std::setfill('0');
        for (size_t index = 0; index < sizeof(T); ++index)
            hexStringStream << std::setw(2) << static_cast<int>(byteData[index]);

        return hexStringStream.str();
    }

    std::string data2Hex(const void *data, const size_t dataLength);
    void dataFromHex(const std::string &in, void *data);

    template <size_t rowSize = 8, bool showASCII = true>
    std::string HexDump(const void *address, size_t len)
    {
        if (!address || len == 0 || rowSize == 0)
            return "";

        const unsigned char *data = static_cast<const unsigned char *>(address);

        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');

        size_t i, j;

        for (i = 0; i < len; i += rowSize)
        {
            // offset
            ss << std::setw(8) << i << ": ";

            // row bytes
            for (j = 0; (j < rowSize) && ((i + j) < len); j++)
                ss << std::setw(2) << static_cast<unsigned int>(data[i + j]) << " ";

            // fill row empty space
            for (; j < rowSize; j++)
                ss << "   ";

            // ASCII
            if (showASCII)
            {
                ss << " ";

                for (j = 0; (j < rowSize) && ((i + j) < len); j++)
                {
                    if (std::isprint(data[i + j]))
                        ss << data[i + j];
                    else
                        ss << '.';
                }
            }

            if (i + rowSize < len)
                ss << std::endl;
        }

        return ss.str();
    }

    namespace Zip
    {
        zip_entry_info_t GetEntryInfoAtOffset(const std::string &zipPath, uintptr_t offset);

        std::pair<void*, size_t> MMapEntryAtOffset(const std::string &zipPath, uintptr_t offset);
    }

}