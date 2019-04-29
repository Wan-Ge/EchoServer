#ifndef KLIB_MUTEX_H
#define KLIB_MUTEX_H

#include <pthread.h>
#include <assert.h>

#include "base/noncopyable.h"
#include "base/currentThread.h"

#if  __cplusplus >= 201103L

#define M_CHECH(x) ({decltype(x) retV = (x) ; \
                    assert(retV == 0); (void) retV;})
#else

#define M_CHECH(x) ({__typeof__ (x) retV = (x) ;    \
                    assert(retV == 0); (void) retV;})

#endif

namespace klib
{
    class MutexLock: private klib::noncopyable
    {
    public:
        MutexLock():holder(0)
        {
            M_CHECH(pthread_mutex_init(&mutex, NULL));
        }

        bool isLockedByThisThread() const
        {
            return holder == currentThread::gettid();
        }

        void assertThread() const 
        {
            assert(isLockedByThisThread());
        }

        void lock()
        {

            M_CHECH(pthread_mutex_lock(&mutex));
            assignHolder();
        }

        void unlock()
        {
            unAssignHolder();
            M_CHECH(pthread_mutex_unlock(&mutex));
        }


        pthread_mutex_t* getMutex() 
        {
            return &mutex;
        }

        ~MutexLock()
        {
            assert(holder == 0);
            M_CHECH(pthread_mutex_destroy(&mutex));
        }
    private:
        pthread_mutex_t mutex;
        pid_t holder;

        void assignHolder()
        {
            holder = currentThread::gettid();
        } 

        void unAssignHolder()
        {
            holder = 0;
        }


        
        friend class Condition;
        class UnAssignGuard : public klib::noncopyable
        {
        public:
            explicit UnAssignGuard(MutexLock& mutex) : self(mutex)
            {
                self.unAssignHolder();
            }

            ~UnAssignGuard()
            {
                self.unAssignHolder();
            }
        private:
            MutexLock &self;
        };
    };


    class MutexLockGuard : public klib::noncopyable
    {
    public:
        explicit MutexLockGuard(MutexLock& mutex):mutexLock(mutex)
        {
            mutexLock.lock();
        }

        ~MutexLockGuard()
        {
            mutexLock.unlock();
        }
    private:
        MutexLock& mutexLock;
    };



}

#define MutexLockGuard(x) error "miss the MutexLockGuard object"





#endif //KLIB_MUTEX_H
