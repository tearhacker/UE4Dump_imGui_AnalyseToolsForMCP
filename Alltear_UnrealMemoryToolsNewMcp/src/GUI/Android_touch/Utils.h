#ifndef GAMEHELPER_UTILS_H
#define GAMEHELPER_UTILS_H

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/system_properties.h>
#include <ctime>
#include <string>
#include <jni.h>
#include <random>
#include <algorithm>
//#include "Log.h"

inline bool isStartWith(const std::string &str, const char *check) {
    return (str.rfind(check, 0) == 0);
}

inline bool isEqual(char *s1, const char *s2) {
    return (strcmp(s1, s2) == 0);
}

inline bool isEqual(const std::string &s1, const char *check) {
    std::string s2(check);
    return (s1 == s2);
}

inline bool isEqual(const std::string &s1, const std::string &s2) {
    return (s1 == s2);
}

inline bool isContain(const std::string &str, const std::string &check) {
    size_t found = str.find(check);
    return (found != std::string::npos);
}

inline void trimStr(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
}

inline std::string getSystemProperty(const char *name) {
    char tmp[PROP_VALUE_MAX]{0};
    __system_property_get(name, tmp);
    return tmp;
}

inline bool getLocalLanguageIsCN() {
    std::string strs[] = {
            getSystemProperty("persist.sys.locale"),
            getSystemProperty("ro.product.locale"),
            getSystemProperty("persist.sys.country"),
            getSystemProperty("persist.sys.language"),
    };
    return std::any_of(std::begin(strs), std::end(strs), [](const std::string &str) {
        return isContain(str, "zh") || isContain(str, "CN");
    });
}

inline int getAndroidSDKLevel() {
    static int var = 0;
    if (var > 0)
        return var;

    var = std::stoi(getSystemProperty("ro.build.version.sdk"));
    return var;
}

inline int getAndroidVersion() {
    static int var = 0;
    if (var > 0)
        return var;

    var = std::stoi(getSystemProperty("ro.build.version.release"));
    return var;
}

inline size_t ReadFile(const char *path, void *buff, size_t size) {
    int fd = open(path, O_RDONLY);
    if (fd > 0) {
        auto len = read(fd, buff, size);
        close(fd);
        return len;
    }
    return 0;
}

inline std::string ReadFile(const std::string &path) {
    std::string str;
    int fd = open(path.c_str(), O_RDONLY);
    if (fd > 0) {
        char buff[4096]{0};
        ssize_t len;
        while ((len = read(fd, buff, sizeof(buff))) > 0) {
            str.append(buff, len);
        }
        close(fd);
    }
    return str;
}

inline size_t WriteFile(const char *path, void *buff, size_t size) {
    int fd = open(path, O_WRONLY | O_CREAT, 0666);
    if (fd > 0) {
        auto len = write(fd, buff, size);
        close(fd);
        return len;
    }
    return 0;
}

inline size_t WriteFile(const std::string &path, const std::string &str) {
    return WriteFile(path.c_str(), (void *) str.c_str(), str.size());
}

inline void genRandomString(char *string, int length) {
    int flag, i;
    srand((unsigned) time(NULL) + length);
    for (i = 0; i < length - 1; i++) {
        flag = rand() % 3;
        switch (flag) {
            case 0:
                string[i] = 'A' + rand() % 26;
                break;
            case 1:
                string[i] = 'a' + rand() % 26;
                break;
            case 2:
                string[i] = '0' + rand() % 10;
                break;
            default:
                string[i] = 'x';
                break;
        }
    }
    string[length - 1] = '\0';
}

inline std::string getRandomString(int length) {
    std::string str;
    srand((unsigned) time(NULL) + length);
    for (int i = 0; i < length - 1; i++) {
        switch (rand() % 3) {
            case 0:
                str += 'A' + rand() % 26;
                break;
            case 1:
                str += 'a' + rand() % 26;
                break;
            case 2:
                str += '0' + rand() % 10;
                break;
            default:
                str += 'x';
                break;
        }
    }
    return str;
}

inline void genRandomStringLetter(char *string, int length) {
    int flag, i;
    srand((unsigned) time(NULL) + length);
    for (i = 0; i < length - 1; i++) {
        flag = rand() % 3;
        switch (flag) {
            case 0:
                string[i] = 'A' + rand() % 26;
                break;
            case 1:
                string[i] = 'a' + rand() % 26;
                break;
            default:
                string[i] = 'x';
                break;
        }
    }
    string[length - 1] = '\0';
}

inline std::string getRandomStringLetter(int length) {
    std::string str;
    int flag, i;
    srand((unsigned) time(NULL) + length);
    for (i = 0; i < length - 1; i++) {
        flag = rand() % 3;
        switch (flag) {
            case 0:
                str += 'A' + rand() % 26;
                break;
            case 1:
                str += 'a' + rand() % 26;
                break;
            default:
                str += 'x';
                break;
        }
    }
    return str;
}


inline uint8_t *FindBytes(const uint8_t *dest, size_t dest_n, const uint8_t *src, size_t src_n) {
    for (int i = 0; i < dest_n - src_n; ++i) {
        size_t j = 0;
        while (j < src_n && dest[i + j] == src[j]) {
            j++;
        }
        if (j == src_n) {
            return (uint8_t *) (dest + i);
        }
    }
    return nullptr;
}

inline bool isFileExist(const char *name) {
    if (access(name, F_OK) == 0) {
        return true;
    }
    return false;
}

inline bool isAlphaNumericString(const char *str) {
    while (*str) {
        if (!isalnum(*str)) {
            return false;
        }
        str++;
    }
    return true;
}


inline std::string getJString(JNIEnv *env, jstring jstr) {
    if (jstr == nullptr || env == nullptr) {
        return {};
    }
    const char *cstr = env->GetStringUTFChars(jstr, nullptr);
    std::string str(cstr);
    env->ReleaseStringUTFChars(jstr, cstr);
    return str;
}

inline int getRandomNumber(int minNumber, int maxNumber) {
    std::random_device rd;  // 随机设备，用于生成种子
    std::mt19937 gen(rd());  // Mersenne Twister 随机数引擎，可以替换为其他引擎
    std::uniform_int_distribution<int> distribution(minNumber, maxNumber);
    return distribution(gen);
}

inline std::vector<std::string> split(const std::string &s, const char &delim = ' ') {
    std::vector<std::string> tokens;
    size_t lastPos = s.find_first_not_of(delim, 0);
    size_t pos = s.find(delim, lastPos);
    while (lastPos != std::string::npos) {
        tokens.emplace_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delim, pos);
        pos = s.find(delim, lastPos);
    }
    return tokens;
}

inline std::string fileNameFromPath(const std::string &filePath) {
    std::string filename;
    const size_t last_slash_idx = filePath.find_last_of("/\\");
    if (std::string::npos != last_slash_idx)
        filename = filePath.substr(last_slash_idx + 1);
    return filename;
}

inline std::string fileDirectory(const std::string &filePath) {
    std::string directory;
    const size_t last_slash_idx = filePath.find_last_of("/\\");
    if (std::string::npos != last_slash_idx)
        directory = filePath.substr(0, last_slash_idx);
    return directory;
}

inline std::string fileExtension(const std::string &filePath) {
    std::string ext;
    const size_t last_slash_idx = filePath.find_last_of(".");
    if (std::string::npos != last_slash_idx)
        ext = filePath.substr(last_slash_idx + 1);
    return ext;
}

inline void Trim(std::string &str) {
    // https://www.techiedelight.com/remove-whitespaces-string-cpp/
    str.erase(std::remove_if(str.begin(), str.end(), [](char c) {
                  return (c == ' ' || c == '\n' || c == '\r' ||
                          c == '\t' || c == '\v' || c == '\f');
              }),
              str.end());
}

inline bool ValidateHex(std::string &hex) {
    if (hex.empty())
        return false;

    if (hex.compare(0, 2, "0x") == 0)
        hex.erase(0, 2);

    Trim(hex); // first remove spaces

    if (hex.length() < 2 || hex.length() % 2 != 0)
        return false;

    for (size_t i = 0; i < hex.length(); i++) {
        if (!std::isxdigit((unsigned char) hex[i]))
            return false;
    }

    return true;
}


#endif