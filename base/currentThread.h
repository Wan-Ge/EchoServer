#ifndef KLIB_CURRENTTHREAD_H
#define KLIB_CURRENTTHREAD_H


#include <stdint.h>   

#include <stdio.h>
// 
#define likely(x) __builtin_expect(!!(x), 1)   //  x  很有可能为真
#define unlikely(x) __builtin_expect(!!(x), 0)  // x  很有可能为假


namespace klib
{
    namespace  currentThread
    {
        extern __thread int cachedTid;  // 线程在内核中的id， 由内核维护
        extern __thread  char tidString[32];  //logging
        extern __thread int tidStringLength;  // logging
        extern __thread const char* p_threadName;
        void cacheTid();

        inline int gettid()
        {
            if(likely(cachedTid == 0))
            {
                cacheTid();
            }

            return cachedTid;
        }

        inline const char*  threadName() 
        {
            return p_threadName;
        }

        inline const char* getTidString()
        {
            return tidString;
        }

        inline int getTidStringLength()
        {
            return tidStringLength;
        }
    }
}


#endif  //KLIB_CURRENTTHREAD_H