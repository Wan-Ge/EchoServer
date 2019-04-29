#include "base/countDownLatch.h"

namespace klib
{
    CountDownLatch::CountDownLatch(const int maxCount) : mutex(), cond(mutex), count(maxCount)
    {

    }

    void CountDownLatch::wait()
    {
        klib::MutexLockGuard lock(mutex);
        while(count > 0){
            cond.wait();
        }
        assert(0 == count);
    }

    void CountDownLatch::countDown()
    {
        klib::MutexLockGuard lock(mutex);
        --count;
        if(0 == count)
        {
            cond.notifyAll();
        }
    }

    int CountDownLatch::getCount() const
    {
        klib::MutexLockGuard lock(mutex);
        return count;
    }

    CountDownLatch::~CountDownLatch()
    {

    }
}