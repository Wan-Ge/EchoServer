#ifndef KLIB_CONDITION_H
#define KLIB_CONDITION_H


#include <pthread.h>


#include "base/mutex.h"
#include "base/noncopyable.h"

#include <time.h>
#include <sys/time.h>
namespace klib 
{
    
    class Condition : private klib::noncopyable
    {
    public:
        explicit Condition(MutexLock& mutex) : mutexLock(mutex)
        {
            M_CHECH(pthread_cond_init(&cond, NULL));
        }

        int timedWait(double seconds)
        {
            int64_t nanosecond = static_cast<int64_t >(kNanosecondPerSecond * seconds);
            struct timeval unixTimeval;
            struct timespec unixTimespec;
            // 有的 unix 系统无法精确到纳秒
#ifdef CLOCK_MONOTONIC_COARSE
            clock_gettime(CLOCK_REALTIME, &unixTimespec);
#else
            gettimeofday(&unixTimeval, NULL);
            unixTimespec.tv_sec = unixTimeval.tv_sec;
            unixTimespec.tv_nsec = unixTimeval.tv_usec * 1000;
#endif

            unixTimespec.tv_sec += static_cast<int64_t>((nanosecond + unixTimespec.tv_nsec) / kNanosecondPerSecond);
            unixTimespec.tv_nsec = static_cast<long>((nanosecond + unixTimespec.tv_nsec) % kNanosecondPerSecond);

            //调用pthread_cond_timedwait 后mutex 属于解锁状态所以需要使持锁 的 id 等于0
            klib::MutexLock::UnAssignGuard ug(mutexLock);
            return pthread_cond_timedwait(&cond, mutexLock.getMutex(), &unixTimespec);
        }

        void wait()
        {
            MutexLock::UnAssignGuard unAssignHolder(mutexLock);
            M_CHECH(pthread_cond_wait(&cond, mutexLock.getMutex()));
        }
        void notify()
        {
            M_CHECH(pthread_cond_signal(&cond));
        }
        void notifyAll()
        {
            M_CHECH(pthread_cond_broadcast(&cond));
        }


        ~Condition(){
            M_CHECH(pthread_cond_destroy(&cond));
        }
    private:
        static const int64_t kNanosecondPerSecond = 1000*1000*1000;
        MutexLock& mutexLock;
        pthread_cond_t cond;
    };
}



#endif
