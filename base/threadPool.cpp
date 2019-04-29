//
// Created by 程俊豪 on 2019/3/12.
//

#include "base/threadPool.h"
#include "base/thread.h"
#include "base/exception.h"
#include <base/logging.h>

#include <boost/bind.hpp>
#include <assert.h>

#include <string>

using namespace klib;

ThreadPool::ThreadPool(int threadNum, const std::string &threadPoolName)
: isRunning(false),
  mutexLock(),
  notFull(mutexLock),
  notEmpty(mutexLock),
  name(threadPoolName),
  taskQueue(),
  maxTaskQueueSize(0),
  threadCount(threadNum),
  idleThreads(),
  blockThreads()
{
}

ThreadPool::~ThreadPool()
{
    if (isRunning)
    {
        stop();
    }
}

void ThreadPool::execute()
{
    assert(threads.empty());
    isRunning = true;
    threads.reserve(threadCount);
    for(int i = 0; i < threadCount; ++i)
    {
        char ch[32];
        ::memset(ch, 0, sizeof(32)) ;
        snprintf(ch, 32, "%d", i + 1);
        Thread* thread = new Thread(boost::bind(&ThreadPool::executeInThread, this), name + ch);
        threads.push_back(thread);
        threads[i].start();
    }
}

void ThreadPool::stop()
{
    assert(isRunning);
    {
        klib::MutexLockGuard lock(mutexLock);
        isRunning = false;
        notEmpty.notifyAll();
    }

    for (boost::ptr_vector<klib::Thread>::iterator it = threads.begin();
            it != threads.end(); ++it)
    {
        it->join();
    }

}

bool ThreadPool::isFull()
{
    mutexLock.isLockedByThisThread();
    return maxTaskQueueSize <= taskQueue.size();
}

void ThreadPool::put(const boost::function<void()> &task)
{
    if (threads.empty())
    {
        task();
    } else{
        klib::MutexLockGuard lock(mutexLock);
        while (isFull())
        {
            blockThreads.increment();
            notFull.wait();
            blockThreads.decrement();
        }
        taskQueue.push(task);
    //    if (idleThreads.get())
            notEmpty.notify();
    }
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
void ThreadPool::put(const boost::function<void()> &&task)
{
    if (threads.empty())
    {
        task();
    } else{
        klib::MutexLockGuard lock(mutexLock);
        while (isFull())
        {
            blockThreads.increment();
            notFull.wait();
            blockThreads.decrement();
        }
        taskQueue.push(std::move(task));
        //if (idleThreads.get())
            notEmpty.notify();
    }
}
#endif

void ThreadPool::setTaskQueueSize(size_t maxSize)
{
    maxTaskQueueSize = maxSize;
}

boost::function<void()> ThreadPool::get()
{
    klib::MutexLockGuard lock(mutexLock);
    while (taskQueue.empty() && isRunning)
    {
        idleThreads.increment();
        notEmpty.wait();
        idleThreads.decrement();
    }
    Task task;
    if (!taskQueue.empty())
    {
        task = taskQueue.front();
        taskQueue.pop();
        //if (blockThreads.get())
            notFull.notify();
    }

    return task;
}


void ThreadPool::executeInThread()
{
    try {
        LOG_INFO << "123";
        while (isRunning)
        {
            Task task = get();
            if(task)
            {
                task();
            }
        }
    }
    catch (const klib::Exception e)
    {

        LOG_SYSFATAL << "klib::Exception caught in threadpool (name = " << name << ") \n"
                     << "this reason : " << e.what() << "\n"
                     << "this stack : " << e.stackTrace();

    }
    catch (const std::exception e)
    {
        LOG_SYSFATAL << "std::exception caught in threadpool (name = " << name << ") \n"
                     << "this reason : " << e.what();
    }
    catch (...)
    {
        LOG_SYSERR << "unknown exception caught in threadpool(name = " << name<< ")";
        throw ;
    }

}