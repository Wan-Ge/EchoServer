//
// Created by 程俊豪 on 2019/3/21.
//

#ifndef KLIB_LOGFILE_H
#define KLIB_LOGFILE_H


#include "base/noncopyable.h"
#include "base/mutex.h"
#include "base/file.h"

#include <time.h>
#include <stdint.h>
#include <string>

#include <boost/scoped_ptr.hpp>

namespace klib
{
    class LogFile : private klib::noncopyable
    {
    public:
        LogFile(const std::string& baseName_,
                int64_t rollSize_,
                bool threadSafe_ = true,
                int flushInterval_ = 3,
                int checkFileEveryN_ = 1000);
        ~LogFile(){}

        void append(const char* logPtr, int len);
        void flush();
        bool rollFile();
    private:

        void append_unlocked(const char* logPtr, int len);

        static std::string getLogFileName(const std::string& baseName_, time_t* now_);

        const std::string baseName;
        const int64_t rollSize;
        const int flushInterval;  // flush 时间间隔
        const int checkFileEveryN; // 表示每log 多少条信息就 check 文件
        int count;  // 表示日志目录有多少条信息

        boost::scoped_ptr<MutexLock> mutex;
        boost::scoped_ptr<klib::file::AppendFile> file;
        time_t startTime;
        time_t lastFlush;
        time_t lastRoll;

        static const time_t kRollPerSeconds = 24*3600;
    };
}


#endif //KLIB_LOGFILE_H
