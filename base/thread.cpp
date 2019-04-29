#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <assert.h>
#include <string>

#include "base/currentThread.h"
#include "base/thread.h"
#include "base/exception.h"
#include "base/countDownLatch.h"


namespace klib
{
    namespace currentThread
    {
        __thread int cachedTid = 0;
        __thread const char* p_threadName = "unknown";
        __thread  char tidString[32];  //logging
        __thread int tidStringLength;  // logging

    }

    namespace detail
    {
        pid_t gettid()
        {
            return static_cast<pid_t>(::syscall(SYS_gettid));
        }

        struct ThreadData
        {
            typedef klib::Thread::ThreadFunc ThreadFunc;
            ThreadFunc func_;
            std::string name_;
            pid_t* tid_;
            CountDownLatch* latch_;

            ThreadData(const ThreadFunc &func,
                    const std::string& name,
                    pid_t * tid,
                    CountDownLatch* latch)
                    : func_(func), name_(name), tid_(tid), latch_(latch)
            {

            }

            void run()
            {
                //再次初始化成员变量
                *tid_ = klib::currentThread::gettid();
                tid_ = NULL;   // 在运行 run 的时候代表线程已经开始运行， 初始化线程类的tid 后， 将指针指向空， 更有安全性
                latch_->countDown();

                // 确认线程名
                /**
                 * 因为在 Linux 当中， 每一个线程 都有一个task_struct 结构，
                 * Linux 管理进程都是在管理 task_struct 结构， 如果是多线程则是管理一个多个 task_struct 结构的进程
                 * 在 task_struct 当中 comm[TASK_COMM_LEN] 成员保存了线程名，  TASK_COMM_LEN = 16
                 * 所以调用prctl 函数的时候， 字符串的值不能超过15, 否则会被阶段
                 * */
                klib::currentThread::p_threadName  = name_.empty() ? "klibThread" : name_.c_str();
                ::prctl(PR_SET_NAME, (unsigned long)klib::currentThread::p_threadName);

                // 调用func_ 函数  切记处理异常
                try {
                    func_();
                    klib::currentThread::p_threadName = " finished";
                }catch(const klib::Exception e)
                {
                    klib::currentThread::p_threadName = "crashed";
                    fprintf(stderr, "Catching exception in thread %s\n", name_.c_str());
                    fprintf(stderr, "Reason: %s\n", e.what());
                    fprintf(stderr, "Stack trace: ", e.stackTrace());
                    abort();

                    /**
                     * 对于异常如果没有对应的 catch 处理
                     * 那么将会导致 系统调用abort() 处理
                     **/
                }
                catch(const std::exception e)
                {
                    klib::currentThread::p_threadName = "crashed";
                    fprintf(stderr, "Catching exception in thread %s\n", name_.c_str());
                    fprintf(stderr, "Reason: %s\n", e.what());
                    abort();
                }catch (...)
                {
                    klib::currentThread::p_threadName = "crashed";
                    fprintf(stderr, "Catching exception in thread %s\n", name_.c_str());
                    throw;
                }
            }


        };


        // 传入线程当中的函数
        void* startThread(void* arg)
        {
            ThreadData* data = static_cast<ThreadData*>(arg);
            data->run();
            delete data;
            return NULL;
        }
    } // end namespace detail
}










void klib::currentThread::cacheTid()
{
    if(0 == cachedTid)
    {
        cachedTid = detail::gettid();
        tidStringLength = snprintf(tidString, sizeof(tidString), "%5d ", cachedTid);
    }
}




using namespace klib;

AtomicInt32 Thread::numCreated;   // default value(0)

int Thread::threadNumCreated()
{
    return numCreated.get();
}

Thread::Thread(const boost::function<void()> &func, const std::string &name)
:   started(false),
    joined(false),
    detached(false),
    pthreadId(0),
    tid(0),
    func(func),
    name_(name),
    latch(1)
{

}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
Thread::Thread(const boost::function<void()> &&func, const std::string &name)
:   started(false),
    joined(false),
    detached(false),
    pthreadId(0),
    tid(0),
    func(std::move(func)),
    name_(name),
    latch(1)
{

}

#endif

void Thread::setDefaultName()
{
    int order = numCreated.incrementAndGet();
    if(name_.empty())
    {
        char buf[64];
        snprintf(buf, 63, "Thread%d", order);
        name_.assign(buf);
    }
}

void Thread::start()
{
    assert(!joined);
    assert(!started);
    assert(!detached);
    started = true;
    detail::ThreadData* threadData = new detail::ThreadData(func, name_, &tid, &latch);

    if(pthread_create(&pthreadId, NULL, (detail::startThread), threadData))
    {   // 线程创建失败
        started = false;
        delete threadData;
        // 日志处理代码
    } else{
        latch.wait();
        assert(tid > 0);
    }
}


int Thread::join()
{
    assert(started == true);
    assert(joined == false);
    joined = true;
    return pthread_join(pthreadId, NULL);
}

void Thread::detach()
{
    assert(started);
    assert(!joined);
    assert(!detached);
    detached = true;
    M_CHECH(pthread_detach(pthreadId));
}

 bool Thread::isStart() const
 {
     return started;
 }

 bool Thread::isJoin() const
 {
     return joined;
 }

 pid_t Thread::gettid() const
 {
     return tid;
 }

 Thread::~Thread()
 {

    if (!detached && started && !joined)
    {
        detach();
    }
 }
