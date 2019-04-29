//
// Created by 程俊豪 on 2019/3/4.
//

#include "base/timeZone.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>
#include <string>

namespace klib
{
    namespace detail
    {
        struct Transformation{
            time_t gmt_time;
            time_t local_time;
            int localtimeIndex;
            Transformation(time_t gmttime, time_t localtime, int localtimeIdx)
                    :gmt_time(gmttime), local_time(localtime), localtimeIndex(localtimeIdx)
            {

            }
        };

        struct Compare{
            bool flag;

            Compare(bool f) : flag(f)
            { }

            bool operator()(const Transformation& lhs, const Transformation& rhs) const
            {
                if(flag)
                    return lhs.gmt_time < rhs.gmt_time;
                else
                    return lhs.local_time < rhs.local_time;
            }

            bool equal(const Transformation& lhs, const Transformation& rhs) const
            {
                if(flag)
                    return lhs.gmt_time == rhs.gmt_time;
                else
                    return lhs.local_time == rhs.local_time;
            }
        };

        struct LocalTime{
            time_t gmtime_offset;
            bool isDst;
            int arrIndex;
            LocalTime(time_t offset, bool dst, int index)
                    : gmtime_offset(offset), isDst(dst), arrIndex(index)
            { }

        };

        // 将秒转化为 时分秒
        inline void secondToHMS(unsigned int seconds, struct tm* hms)
        {
            hms->tm_sec = seconds % 60;
            unsigned int tmp_minutes = (seconds - hms->tm_sec) / 60;
            hms->tm_min = tmp_minutes % 60;
            hms->tm_hour = tmp_minutes / 60;
        }

    }

    const unsigned int kSecondsPerDay = 24 * 3600;
}

using namespace klib;
using namespace std;

struct TimeZone::Data
{
    vector<detail::Transformation> transformations;
    vector<detail::LocalTime> localtimes;
    vector<string> names;
    string timezoneName;
};


TimeZone::TimeZone(int eastUtc, const char *name) : p_data(new Data())
{
    p_data->localtimes.push_back(detail::LocalTime(eastUtc, false, 0));
    p_data->timezoneName = name;
}

struct tm TimeZone::toLocalTime(time_t secondsSinceEpoch) const
{
    struct tm localtime;
    memset(&localtime, 0, sizeof(localtime));
    assert(p_data != NULL);
    const Data& data(*p_data);
    const detail::LocalTime* p_localtime = data.localtimes.empty() ? NULL :&(data.localtimes.front());

    if(p_localtime)
    {
        time_t localSeconds = secondsSinceEpoch + p_localtime->gmtime_offset;
        ::gmtime_r(&localSeconds, &localtime);
        localtime.tm_isdst = p_localtime->isDst;
        localtime.tm_gmtoff = p_localtime->gmtime_offset;
        localtime.tm_zone = data.timezoneName.c_str();
    }

    return localtime;
}

time_t TimeZone::fromLocalTime(const struct tm &tmtime) const
{
    assert(p_data != NULL);
    const Data& data(*p_data);
    struct tm tmpTmtime = tmtime;
    time_t seconds = ::timegm(&tmpTmtime);
    const detail::LocalTime* p_localtime = data.localtimes.empty() ? NULL : &(data.localtimes.front());
    if (tmtime.tm_isdst)
    {
        struct tm tryTm = toLocalTime(seconds - p_localtime->gmtime_offset);
        if(!tryTm.tm_isdst && tryTm.tm_hour == tmtime.tm_hour
                           && tryTm.tm_min == tmtime.tm_min)
        {
            seconds -= 3600;
        }
    }

    return seconds - p_localtime->gmtime_offset;
}


struct tm TimeZone::toUtcTime(time_t, bool yday)
{

}

time_t TimeZone::fromUtcTime(const struct tm &)
{

}

time_t TimeZone::fromUtcTime(int year, int month, int day, int hour, int minute, int second)
{

}







