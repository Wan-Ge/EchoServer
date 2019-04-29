//
// Created by 程俊豪 on 2019/3/5.
//

#ifndef KLIB_THREAD_H
#define KLIB_THREAD_H

#include "base/noncopyable.h"
#include "base/countDownLatch.h"
#include "base/atomic.h"

#include <pthread.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace klib
{
    class Thread : private klib::noncopyable
    {
    public:
        typedef boost::function<void()> ThreadFunc;
        Thread(const ThreadFunc& func, const std::string& name = std::string());

#ifdef __GXX_EXPERIMENTAL_CXX0X__
        Thread(const ThreadFunc&& func, const std::string& name = std::string());
#endif
        ~Thread();
        void start();
        int join();
        bool isStart() const;
        bool isJoin() const;
        pid_t gettid() const ;
        void detach();

        const std::string& name() const
        {
            return name_;
        }



        static int threadNumCreated();
    private:
        void setDefaultName();


        bool started;
        bool joined;
        bool detached;
        pthread_t pthreadId;
        pid_t tid; //线程在内核中的id
        ThreadFunc func;




        std::string name_;
        CountDownLatch latch;


        static AtomicInt32 numCreated;
    };
}

#endif //KLIB_THREAD_H
