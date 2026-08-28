#pragma once

#include <cstdarg>
#include <cstdio>

#ifndef kEXECUTABLE
#include <android/log.h>
#define LOG_TAG "UEDump3r"
#endif

namespace Logger
{
    using SinkFn = void (*)(char level, const char *message);

    inline SinkFn &GetSink()
    {
        static SinkFn sink = nullptr;
        return sink;
    }

    inline void SetSink(SinkFn sink)
    {
        GetSink() = sink;
    }

    inline void ForwardToSink(char level, const char *message)
    {
        if (auto sink = GetSink())
            sink(level, message);
    }

#ifndef kEXECUTABLE
    inline int ToAndroidPriority(char level)
    {
        switch (level)
        {
            case 'V': return ANDROID_LOG_VERBOSE;
            case 'D': return ANDROID_LOG_DEBUG;
            case 'I': return ANDROID_LOG_INFO;
            case 'W': return ANDROID_LOG_WARN;
            case 'E': return ANDROID_LOG_ERROR;
            case 'F': return ANDROID_LOG_FATAL;
            default: return ANDROID_LOG_DEFAULT;
        }
    }
#endif

    inline void Log(char level, const char *fmt, ...)
    {
        char buffer[4096] = {0};

        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

#ifdef kEXECUTABLE
        std::printf("%c: %s\n", level, buffer);
#else
        __android_log_print(ToAndroidPriority(level), LOG_TAG, "%s", buffer);
#endif

        ForwardToSink(level, buffer);
    }
}  // namespace Logger

#define LOGV(fmt, ...) ::Logger::Log('V', fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) ::Logger::Log('I', fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) ::Logger::Log('W', fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) ::Logger::Log('E', fmt, ##__VA_ARGS__)
#define LOGF(fmt, ...) ::Logger::Log('F', fmt, ##__VA_ARGS__)

#ifndef NDEBUG
#define LOGD(fmt, ...) ::Logger::Log('D', fmt, ##__VA_ARGS__)
#else
#define LOGD(...) do {} while (false)
#endif
