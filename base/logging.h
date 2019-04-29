//
// Created by 程俊豪 on 2019/3/15.
//

#ifndef KLIB_LOG_H
#define KLIB_LOG_H

#include "base/logStream.h"
#include "base/timeStamp.h"
#include "base/timeZone.h"

namespace klib
{


    class Logger
    {
    public:
        enum LogLevel
        {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            LOGLEVELNUM
        };

        Logger(const char* file, int line);
        Logger(const char* file, int line, LogLevel level);
        Logger(const char* file, int line, LogLevel level, const char* func);
        Logger(const char* file, int line, bool isAbort);
        ~Logger();

        LogStream& stream() { return impl.logStream; }
        static LogLevel getLogLevel();
        static void setLogLevel(LogLevel logLevel);

        typedef void (*OutputFunc)(const char* msg, int len);
        typedef void (*FlushFunc)();
        static void setOutputFunc(OutputFunc);
        static void setFlushFunc(FlushFunc);
        static void setTimeZone(const TimeZone& zone);
    private:
        class LoggerImpl
        {
        public:
            typedef Logger::LogLevel LogLevel;
            LoggerImpl(LogLevel level1, int err, const char* basename1, int line1);
            void formatTime();
            void finish();
            LogLevel level;
            TimeStamp time;
            LogStream logStream;
            int line;
            const char* basename;
        };
        LoggerImpl impl;
    };

    /// 日志的级别默认为 INFO 级别， g_logLevel变量请查看 logging.cpp文件
    extern Logger::LogLevel g_logLevel;
    inline Logger::LogLevel Logger::getLogLevel()
    {
        return g_logLevel;
    }

#define LOG_TRACE if(klib::Logger::getLogLevel() <= klib::Logger::TRACE) \
  klib::Logger(__FILE__, __LINE__, klib::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if(klib::Logger::getLogLevel() <= klib::Logger::DEBUG) \
    klib::Logger(__FILE__, __LINE__, klib::Logger::DEBUG, __func__).stream()

#define LOG_INFO  if(klib::Logger::getLogLevel() <= klib::Logger::INFO) \
    klib::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN klib::Logger(__FILE__, __LINE__, klib::Logger::WARN).stream()
#define LOG_ERROR klib::Logger(__FILE__, __LINE__, klib::Logger::ERROR).stream()
#define LOG_FATAL klib::Logger(__FILE__, __LINE__, klib::Logger::FATAL).stream()
#define LOG_SYSERR klib::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL klib::Logger(__FILE__, __LINE__, true).stream()






    const char* strerror_t(int err);
}


#endif //KLIB_LOG_H
