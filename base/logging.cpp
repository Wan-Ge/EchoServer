//
// Created by 程俊豪 on 2019/3/15.
//

#include "base/logging.h"
#include "base/currentThread.h"
#include "base/timeZone.h"
#include "base/timeStamp.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>



namespace klib
{
    __thread char t_errnoBuf[512];
    __thread char t_time[64];
    __thread time_t t_lastSecond;
    const char* strerror_t(int err)
    {
        strerror_r(err, t_errnoBuf, sizeof(t_errnoBuf));
        return t_errnoBuf;
    }

    //默认初始化为INFO日志级别
    Logger::LogLevel initLogLevel()
    {
        if(::getenv("KLIB_LOG_TRACE"))
            return Logger::TRACE;
        else if(::getenv("KLIB_LOG_DEBUG"))
            return Logger::DEBUG;
        else
            return Logger::INFO;
    }

    Logger::LogLevel g_logLevel = initLogLevel();

    const char* LogLevelName[Logger::LOGLEVELNUM] =
    {
            "TRACE ",
            "DEBUG ",
            "INFO  ",
            "WARN  ",
            "ERROR ",
            "FATAL ",
    };

    void defaultOutput(const char* msg, int len)
    {
        fwrite(msg, 1, len, stdout);
    }

    void defaultFlush()
    {
        fflush(stdout);
    }


    class StringSlice{
    public:
        StringSlice(const char* p, size_t length): ptr(p), len(length)
        { assert(strlen(ptr) == len); }

        const char* ptr;
        size_t len;
    };

    inline LogStream& operator << (LogStream& os, StringSlice stringSlice)
    {
        os.append(stringSlice.ptr, stringSlice.len);
        return os;
    }




    Logger::OutputFunc g_outputFunc = defaultOutput;
    Logger::FlushFunc g_flushFunc = defaultFlush;
    TimeZone g_logTimezone;
}

using namespace klib;

Logger::LoggerImpl::LoggerImpl(klib::Logger::LogLevel level1, int err, const char *basename1, int line1)
 :  level(level1),
    time(TimeStamp::now()),
    logStream(),
    line(line1),
    basename(basename1)

{
    formatTime();
    currentThread::gettid();
    logStream << StringSlice(currentThread::getTidString(), currentThread::getTidStringLength());
    logStream << StringSlice(LogLevelName[level], 6);
    if(err)
    {
        logStream << strerror_t(err) <<"errno=" << err << ")";
    }
}

void Logger::LoggerImpl::formatTime()
{
    int64_t microUnixSeconds = time.getMicroUnixSeconds();
    time_t seconds = time.getUnixSeconds();
    int microSeconds = static_cast<int>(microUnixSeconds % TimeStamp::kMicorSecondsPerSecond);

    if(seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        struct tm tm_time;
        memset(&tm_time, 0, sizeof(tm_time));
        if(g_logTimezone.valid())
        {
            tm_time = g_logTimezone.toLocalTime(seconds);
        }else{
            ::gmtime_r(&seconds, &tm_time);
        }
//        int len = snprintf(t_time, sizeof(t_time), "%s", time.toFormatString(false).c_str());
        int len =  snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
                            tm_time.tm_year + 1900, tm_time.tm_mon + 1,
                            tm_time.tm_mday, tm_time.tm_hour,
                            tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17);
        (void)len;
    }

    if(g_logTimezone.valid())
    {
        Fmt fmt(".%06d ", microSeconds);
        assert(fmt.length() == 8);
        logStream << StringSlice(t_time, 17) <<StringSlice(fmt.data(), 8);
    } else{
        Fmt fmt(".%06dZ ", microSeconds);
        assert(fmt.length() == 9);
        logStream << StringSlice(t_time, 17) <<StringSlice(fmt.data(), 9);
    }

}

void Logger::LoggerImpl::finish()
{
    logStream << " - " << basename << ':' << line << '\n';
}

Logger::Logger(const char *file, int line) : impl(INFO, 0, file, line)
{

}

Logger::Logger(const char *file, int line, klib::Logger::LogLevel level, const char *func)
: impl(level, 0, file, line)
{
//    fprintf(stdout, "line-- = %d\n", line);
    impl.logStream << func <<"(" <<line << ") ";
}

Logger::Logger(const char *file, int line, klib::Logger::LogLevel level)
:impl(level, 0, file, line)
{

}
Logger::Logger(const char *file, int line, bool isAbort)
: impl(isAbort ? FATAL : ERROR, 0, file, line)
{

}

Logger::~Logger()
{
    impl.finish();
    const LogStream::Buffer& buffer(stream().buffer());
    g_outputFunc(buffer.data(), buffer.length());
    if(impl.level == FATAL)
    {
        g_flushFunc();
        abort();
    }
}

void Logger::setLogLevel(klib::Logger::LogLevel logLevel)
{
    g_logLevel = logLevel;
}

void Logger::setFlushFunc(klib::Logger::FlushFunc flushFunc)
{
    g_flushFunc = flushFunc;
}

void Logger::setOutputFunc(klib::Logger::OutputFunc outputFunc)
{
    g_outputFunc = outputFunc;
}

void Logger::setTimeZone(const klib::TimeZone &zone)
{
    g_logTimezone = zone;
}