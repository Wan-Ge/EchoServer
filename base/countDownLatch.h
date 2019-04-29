#ifndef KLIB_COUNTDOWNLATCH_H
#define KLIB_COUNTDOWNLATCH_H

#include "base/mutex.h"
#include "base/condition.h"
#include "base/noncopyable.h"


namespace klib
{
    class  CountDownLatch : private klib::noncopyable
    {
    public:
        explicit CountDownLatch(const int maxCount) ;

        void wait();
        void countDown();
        int getCount() const;


        ~CountDownLatch();
    private:
        mutable MutexLock mutex;
        Condition cond;
        int count;
    };
}

#endif