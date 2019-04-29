//
// Created by 程俊豪 on 2019/3/12.
//

#ifndef KLIB_THREADPOOL_H
#define KLIB_THREADPOOL_H

#include "base/noncopyable.h"
#include "base/mutex.h"
#include "base/condition.h"
#include "base/thread.h"
#include "base/atomic.h"

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/function.hpp>

#include <queue>
namespace klib
{
    class ThreadPool : private klib::noncopyable
    {
    public:
        typedef boost::function<void()> Task;

        ThreadPool(int threadNum, const std::string& threadPoolName = "ThreadPool");
        ~ThreadPool();
        void execute();
        void stop();

        void shutDown();
        void put(const Task& task);

#ifdef __GXX_EXPERIMENTAL_CXX0X__
        void put(const Task&& task);
#endif

        const std::string& getThreadPoolName() const { return name; }
        size_t getTaskQueueSize() const { return taskQueue.size(); }

        void setTaskQueueSize(size_t maxSize);
    private:
        Task get();
        void executeInThread();
        bool isFull();

        bool isRunning;
        mutable MutexLock mutexLock;
        Condition notFull;
        Condition notEmpty;
        std::string name;
        std::queue<Task> taskQueue;
        boost::ptr_vector<klib::Thread> threads;

        size_t maxTaskQueueSize;
        const int threadCount;

        AtomicInt32 idleThreads; // 表示线程池中的空闲线程数
        AtomicInt32 blockThreads; // 表示在put 过程中堵塞线程数
    };
}
#endif //KLIB_THREADPOOL_H
