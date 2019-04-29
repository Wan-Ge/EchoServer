//
// Created by 程俊豪 on 2019/2/27.
//

#ifndef KLIB_TIMESTAMP_H
#define KLIB_TIMESTAMP_H

#include <stdint.h>
#include <time.h>

#include <string>
#include <boost/operators.hpp>

#include "base/noncopyable.h"

namespace klib
{
    // 类TimeStamp 以UTC(unix 元年)为基准，及1970年1月1日为标准进行转化，
    class TimeStamp : public boost::equality_comparable<TimeStamp>,
                      public boost::less_than_comparable<TimeStamp>
    {
    public:
        TimeStamp() : microUnixSeconds(0){}

        //构造函数，用一个int64_t构造当前类
        //@parram microUnixSecondsArg
        explicit TimeStamp(int64_t microUnixSecondsArg) : microUnixSeconds(microUnixSecondsArg){}

        void swap(TimeStamp& rhs)
        {
            std::swap(this->microUnixSeconds, rhs.microUnixSeconds);
        }

        std::string toString() const;
        std::string toFormatString(bool showmicroSecondFlag = true) const;

        bool valid() const
        {
            return microUnixSeconds > 0;
        }

        //获取微妙
        int64_t getMicroUnixSeconds() const
        {
            return microUnixSeconds;
        }

//        关于time_t 的底层数据类型有不同的方式
//        在 vs 中查看time.h的源代码可以查看 其数据类型为int64_t类型
//        在linux 下查看为 long int 类型， 当然在不同的Linux 版本当中对其数据类型的定义也会不同
//        所以我们要使用static_cast 进行转化
        time_t getUnixSeconds() const
        {
            return static_cast<time_t>(microUnixSeconds / kMicorSecondsPerSecond);
        }

        static TimeStamp now();
        static TimeStamp invalid()
        {
            return TimeStamp();
        }

        static TimeStamp fromUnixTime(const time_t t)
        {
            fromUnixTime(t, 0);
        }

        static TimeStamp fromUnixTime(const time_t t, int microseconds)
        {
            return TimeStamp(static_cast<int64_t >(t) * kMicorSecondsPerSecond + microseconds);
        }





        const static int kMicorSecondsPerSecond = 1000000;

    private:
        int64_t microUnixSeconds;
    };

    inline bool operator < (const TimeStamp& t1, const TimeStamp& t2)
    {
        return t1.getMicroUnixSeconds() < t2.getMicroUnixSeconds();
    }

    inline bool operator == (const TimeStamp& t1, const TimeStamp& t2)
    {
        return t1.getMicroUnixSeconds() == t2.getMicroUnixSeconds();
    }

    /// 求t1 和 t2的时间之差 返回的时间是秒
    /// \param t1
    /// \param t2
    /// \return 返回秒
    inline double timeDifference(const TimeStamp& t1, const TimeStamp& t2)
    {
        int64_t diff = t1.getMicroUnixSeconds() - t2.getMicroUnixSeconds();
        return static_cast<double >(diff) / TimeStamp::kMicorSecondsPerSecond;
    }

    inline TimeStamp addTime(const TimeStamp& t1, double seconds)
    {
        int64_t total = t1.getMicroUnixSeconds() + seconds +TimeStamp::kMicorSecondsPerSecond;
        return TimeStamp(total);
    }
}
#endif //KLIB_TIMESTAMP_H
