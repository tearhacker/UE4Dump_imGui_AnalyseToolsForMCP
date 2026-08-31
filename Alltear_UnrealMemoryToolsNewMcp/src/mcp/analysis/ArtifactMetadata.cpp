#include "ArtifactMetadata.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <elf.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <vector>

namespace UmtMcp::Analysis
{
namespace
{
constexpr std::array<uint32_t, 64> kSha256 = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

uint32_t RotateRight(uint32_t value, int bits)
{
    return (value >> bits) | (value << (32 - bits));
}

class Sha256
{
public:
    void Update(const uint8_t *data, size_t size)
    {
        totalBytes_ += size;
        while (size > 0)
        {
            const size_t take = std::min(size, block_.size() - blockSize_);
            std::memcpy(block_.data() + blockSize_, data, take);
            blockSize_ += take;
            data += take;
            size -= take;
            if (blockSize_ == block_.size())
            {
                Transform(block_.data());
                blockSize_ = 0;
            }
        }
    }

    std::string Final()
    {
        const uint64_t totalBits = totalBytes_ * 8;
        std::array<uint8_t, 128> padding{};
        padding[0] = 0x80;
        const size_t paddingSize = blockSize_ < 56 ? 56 - blockSize_ : 120 - blockSize_;
        Update(padding.data(), paddingSize);
        std::array<uint8_t, 8> length{};
        for (int i = 0; i < 8; ++i)
            length[7 - i] = static_cast<uint8_t>(totalBits >> (i * 8));
        Update(length.data(), length.size());

        std::ostringstream out;
        out << std::hex << std::setfill('0');
        for (uint32_t value : state_) out << std::setw(8) << value;
        return out.str();
    }

private:
    void Transform(const uint8_t *data)
    {
        std::array<uint32_t, 64> words{};
        for (size_t i = 0; i < 16; ++i)
            words[i] = (static_cast<uint32_t>(data[i * 4]) << 24) |
                       (static_cast<uint32_t>(data[i * 4 + 1]) << 16) |
                       (static_cast<uint32_t>(data[i * 4 + 2]) << 8) |
                       static_cast<uint32_t>(data[i * 4 + 3]);
        for (size_t i = 16; i < words.size(); ++i)
        {
            const uint32_t s0 = RotateRight(words[i - 15], 7) ^ RotateRight(words[i - 15], 18) ^ (words[i - 15] >> 3);
            const uint32_t s1 = RotateRight(words[i - 2], 17) ^ RotateRight(words[i - 2], 19) ^ (words[i - 2] >> 10);
            words[i] = words[i - 16] + s0 + words[i - 7] + s1;
        }
        uint32_t a = state_[0], b = state_[1], c = state_[2], d = state_[3];
        uint32_t e = state_[4], f = state_[5], g = state_[6], h = state_[7];
        for (size_t i = 0; i < words.size(); ++i)
        {
            const uint32_t s1 = RotateRight(e, 6) ^ RotateRight(e, 11) ^ RotateRight(e, 25);
            const uint32_t choice = (e & f) ^ (~e & g);
            const uint32_t temp1 = h + s1 + choice + kSha256[i] + words[i];
            const uint32_t s0 = RotateRight(a, 2) ^ RotateRight(a, 13) ^ RotateRight(a, 22);
            const uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
            const uint32_t temp2 = s0 + majority;
            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }
        state_[0] += a; state_[1] += b; state_[2] += c; state_[3] += d;
        state_[4] += e; state_[5] += f; state_[6] += g; state_[7] += h;
    }

    std::array<uint32_t, 8> state_ = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    std::array<uint8_t, 64> block_{};
    size_t blockSize_ = 0;
    uint64_t totalBytes_ = 0;
};

std::string HashFile(const std::string &path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) return {};
    Sha256 hash;
    std::array<uint8_t, 1024 * 1024> buffer{};
    while (input)
    {
        input.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        const std::streamsize got = input.gcount();
        if (got > 0) hash.Update(buffer.data(), static_cast<size_t>(got));
    }
    return input.eof() ? hash.Final() : std::string();
}

size_t Align4(size_t value)
{
    return (value + 3) & ~size_t(3);
}

std::string ReadBuildId(const std::string &path)
{
    std::ifstream input(path, std::ios::binary);
    Elf64_Ehdr header{};
    if (!input.read(reinterpret_cast<char *>(&header), sizeof(header)) ||
        std::memcmp(header.e_ident, ELFMAG, SELFMAG) != 0 ||
        header.e_ident[EI_CLASS] != ELFCLASS64 || header.e_ident[EI_DATA] != ELFDATA2LSB)
        return {};
    if (header.e_phentsize != sizeof(Elf64_Phdr) || header.e_phnum > 1024) return {};
    for (uint16_t i = 0; i < header.e_phnum; ++i)
    {
        Elf64_Phdr program{};
        input.clear();
        input.seekg(static_cast<std::streamoff>(header.e_phoff) +
                    static_cast<std::streamoff>(i) * sizeof(program));
        if (!input.read(reinterpret_cast<char *>(&program), sizeof(program))) return {};
        if (program.p_type != PT_NOTE || program.p_filesz == 0 || program.p_filesz > 4 * 1024 * 1024) continue;
        std::vector<uint8_t> notes(static_cast<size_t>(program.p_filesz));
        input.clear();
        input.seekg(static_cast<std::streamoff>(program.p_offset));
        if (!input.read(reinterpret_cast<char *>(notes.data()), notes.size())) continue;
        size_t cursor = 0;
        while (cursor + sizeof(Elf64_Nhdr) <= notes.size())
        {
            Elf64_Nhdr note{};
            std::memcpy(&note, notes.data() + cursor, sizeof(note));
            cursor += sizeof(note);
            const size_t nameSize = Align4(note.n_namesz);
            const size_t descSize = Align4(note.n_descsz);
            if (nameSize > notes.size() - cursor || descSize > notes.size() - cursor - nameSize) break;
            const uint8_t *name = notes.data() + cursor;
            const uint8_t *description = name + nameSize;
            if (note.n_type == NT_GNU_BUILD_ID && note.n_namesz >= 3 &&
                std::memcmp(name, "GNU", 3) == 0)
            {
                std::ostringstream out;
                out << std::hex << std::setfill('0');
                for (uint32_t j = 0; j < note.n_descsz; ++j)
                    out << std::setw(2) << static_cast<unsigned>(description[j]);
                return out.str();
            }
            cursor += nameSize + descSize;
        }
    }
    return {};
}
}

nlohmann::json InspectArtifactFile(const std::string &path, const std::string &source)
{
    struct stat info{};
    if (::stat(path.c_str(), &info) != 0)
        return {{"devicePath", path}, {"source", source}, {"exists", false}};
    const std::string sha256 = HashFile(path);
    const std::string buildId = ReadBuildId(path);
    return {{"devicePath", path}, {"source", source}, {"exists", true},
            {"sizeBytes", static_cast<uint64_t>(info.st_size)},
            {"sha256", sha256.empty() ? nlohmann::json(nullptr) : nlohmann::json(sha256)},
            {"buildId", buildId.empty() ? nlohmann::json(nullptr) : nlohmann::json(buildId)}};
}
}
