#ifndef KLIB_BOUNDEDBLOCKINGQUEUE_H
#define KLIB_BOUNDEDBLOCKINGQUEUE_H

#include "base/circualBuffer.h"
#include "base/condition.h"
#include "base/mutex.h"
#include "base/noncopyable.h"


namespace klib
{
    template<class _Ty>
    class BounedBlockingQueue : private klib::noncopyable
    {
    public:
        explicit BounedBlockingQueue(const size_t bufferSize) : mutex(), notEmpty(mutex), notFull(mutex), buffer(bufferSize)
        {

        }

        void put(const _Ty& t)
        {
            klib::MutexLockGuard lock(mutex);
            while(buffer.full())
            {
                notFull.wait();
            }
            assert(!buffer.full());
            buffer.put(t);
            notEmpty.notify();
        }

        _Ty get()
        {
            klib::MutexLockGuard lock(mutex);
            while(buffer.empty())
            {
                notEmpty.wait();
            }
            assert(!buffer.empty());
            _Ty t(buffer.get());
            notFull.notify();
            return t;
        }

        size_t size() const
        {
            klib::MutexLockGuard lock(mutex);
            return buffer.size();
        }

        bool full() const
        {
            klib::MutexLockGuard lock(mutex);
            return buffer.full();
        }

        bool empty() const
        {
            klib::MutexLockGuard lock(mutex);
            return buffer.empty();
        }

        size_t capacity() const
        {
            klib::MutexLockGuard lock(mutex);
            return buffer.capacity()
            ;
        }
    private:
        MutexLock mutex;
        Condition notEmpty;
        Condition notFull;
        klib::CircualBuffer<_Ty> buffer;
    };
}

#endif