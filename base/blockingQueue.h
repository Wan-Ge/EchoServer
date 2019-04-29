#ifndef KLIB_BLOCKINGQUEUE_H
#define KLIB_BLOCKINGQUEUE_H

#include "base/condition.h"
#include "base/mutex.h"


#include <deque>
namespace klib
{
    template<class _Ty>
    class BlockingQueue : private klib::noncopyable
    {
    public:
        BlockingQueue() : queue(), mutex(),notEmpty(mutex)
        {

        }

        void put(const _Ty& t)
        {
            klib::MutexLockGuard lock(mutex);
            queue.push_back(t);
            notEmpty.notify();
        }


        _Ty get()
        {
            klib::MutexLockGuard lock(mutex);
            while(queue.empty())
            {
                notEmpty.wait();
            }
            _Ty t(queue.front());
            queue.pop_front();
            return t;
        }

        size_t size()
        {
            klib::MutexLockGuard lock(mutex);
            return queue.size();
        }
        ~BlockingQueue(){

        }
    private:
        std::deque<_Ty> queue;
        MutexLock mutex;
        Condition notEmpty;
    };
}

#endif