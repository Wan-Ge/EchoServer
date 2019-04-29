//
// Created by 程俊豪 on 2019/3/21.
//

#include "base/logFile.h"
#include "base/file.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

using namespace klib;


LogFile::LogFile(const std::string &baseName_,
                 int64_t rollSize_,
                 bool threadSafe_,
                 int flushInterval_,
                 int checkFileEveryN_)
:baseName(baseName_),
 rollSize(rollSize_),
 flushInterval(flushInterval_),
 checkFileEveryN(checkFileEveryN_),
 count(0),
 mutex(threadSafe_ ? new MutexLock() : NULL),
 startTime(0),
 lastFlush(0),
 lastRoll(0)
{
    assert(baseName.find('/') == std::string::npos);
    rollFile();
}

void LogFile::append(const char *logPtr, int len)
{
    if(mutex)
    {
        klib::MutexLockGuard lockGuard(*mutex);
        append_unlocked(logPtr, len);
    } else{
        append_unlocked(logPtr, len);
    }
}

void LogFile::flush()
{
    if(mutex)
    {
        klib::MutexLockGuard lockGuard(*mutex);
        file->fflush();
    } else{
        file->fflush();
    }
}

bool LogFile::rollFile()
{
    time_t  now = 0;
    std::string logName = getLogFileName(baseName, &now);
    time_t tmpStart = now / kRollPerSeconds * kRollPerSeconds;
    if(now > lastRoll)
    {
        startTime = now;
        lastFlush = now;
        lastRoll = now;
        file.reset(new klib::file::AppendFile(logName.c_str()));
        return true;
    }
    return false;
}

void LogFile::append_unlocked(const char *logPtr, int len)
{
    file->append(logPtr, len);
    if(file->writeBytes() > rollSize)
    {
        rollFile();
    } else{
        ++count;
        time_t now = ::time(NULL);
        if(now - lastRoll >= kRollPerSeconds)  // 文件超过了一天需要重新rollFile 建立新的log文件
        {
            rollFile();
        }
        else if(now - lastFlush > flushInterval){
            lastFlush = now;
            file->fflush();
        }
    }
}

std::string LogFile::getLogFileName(const std::string &baseName_, time_t *now_)
{
    //日志文件格式为 baseName_ + .YYYYMMDD-HHMMSS.pid.log
    std::string fileName = baseName_;
    char timebuf[32];
    ::memset(timebuf, 0, 32);
    *now_ = ::time(NULL);
    struct tm tmtime;
    ::gmtime_r(now_, &tmtime);
    ::strftime(timebuf, 31,".%Y%m%d-%H%M%S.", &tmtime);
    fileName.append(timebuf);
    char pidbuf[32];
    memset(pidbuf, 0, 32);
    ::snprintf(pidbuf, 31, "%d.log", ::getpid());
    fileName.append(pidbuf);
    return fileName;
}