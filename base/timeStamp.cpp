//
// Created by 程俊豪 on 2019/2/28.
//
#include <sys/time.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <boost/static_assert.hpp>

#include "base/timeStamp.h"

namespace klib {

// assert 是在运行的时候动态检查错误
// 而 BOOST_STATIC_ASSERT是对 static_assert的进一步实现
// static_assert 相对assert 的优势就是在编译器就会检查值是否错误
    BOOST_STATIC_ASSERT(sizeof(TimeStamp) == sizeof(int64_t));

    TimeStamp TimeStamp::now() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t tempMicroUnixSeconds = static_cast<int64_t>(tv.tv_sec) * TimeStamp::kMicorSecondsPerSecond + tv.tv_usec;
        return TimeStamp(tempMicroUnixSeconds);
    }


    std::string TimeStamp::toString() const {
        char buf[32] = {0};
        memset((void *) buf, 0, 32);
        int64_t tmpSeconds = microUnixSeconds / kMicorSecondsPerSecond;
        int64_t tmpMicroSeconds = microUnixSeconds % kMicorSecondsPerSecond;
        snprintf(buf, 31, "%"
        PRId64
        ".%06d"
        PRId64
        "", tmpSeconds, tmpMicroSeconds);
        return std::string(buf);
    }


    std::string TimeStamp::toFormatString(bool showmicroSecondFlag) const {
        char buf[64];
        struct tm tm_time;
        memset(buf, 0, 64);
        time_t sec = static_cast<time_t >(microUnixSeconds / kMicorSecondsPerSecond);
        int64_t tmpMicroSeconds = microUnixSeconds % kMicorSecondsPerSecond;
        //可重入函数与gmtime相比的优点就是gmtime函数返回的结果是静态的结果
        // 如果没有将其中的结果写入栈空间之前其他线程也调用了gmtime函数其结果将会改变
        // 使用 gmtime_r 函数可以将结果返回用户空间
        gmtime_r(&sec, &tm_time);
        if(showmicroSecondFlag)
        {
            snprintf(buf, 63, "%4d-%02d-%02d %02d:%02d:%02d.%06d",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1,
                    tm_time.tm_mday, tm_time.tm_hour,
                    tm_time.tm_min, tm_time.tm_sec,
                    tmpMicroSeconds);
        }else{
            snprintf(buf, 63, "%4d-%02d-%02d %02d:%02d:%02d",
                     tm_time.tm_year + 1900, tm_time.tm_mon + 1,
                     tm_time.tm_mday, tm_time.tm_hour,
                     tm_time.tm_min, tm_time.tm_sec);
        }

        return std::string(buf);
    }

}