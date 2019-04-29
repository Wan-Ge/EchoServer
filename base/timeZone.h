//
// Created by 程俊豪 on 2019/3/4.
//

#ifndef KLIB_TIMEZONE_H
#define KLIB_TIMEZONE_H

#include <boost/shared_ptr.hpp>

#include <time.h>

#include "base/noncopyable.h"

namespace klib
{
    class TimeZone
    {
    public:
        TimeZone(int eastUtc, const char* zoneFile);
        TimeZone(){ }
        ~TimeZone(){ }

        bool valid() const
        {
            return static_cast<bool> (p_data);
        }

        struct tm toLocalTime(time_t) const ;
        time_t fromLocalTime(const struct tm&) const;

        //gmtime(3)
        static struct tm toUtcTime(time_t, bool yday = false);
        static time_t fromUtcTime(const struct tm&);
        static time_t  fromUtcTime(int year, int month, int day, int hour, int minute, int second);
        struct Data;
    private:
        boost::shared_ptr<struct Data> p_data;
    };
}

#endif //KLIB_TIMEZONE_H
