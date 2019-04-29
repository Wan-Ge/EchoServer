#include "base/asyncLogging.h"
#include "base/timeStamp.h"
#include "base/logFile.h"


using namespace klib;

AysncLogging::AysncLogging(const std::string &baseName_,
        int64_t rollSize_,
        int flushInterval_):
        flushInterval(flushInterval_),
        running(false),
        baseName(baseName_),
        rollSize(rollSize_),
        thread(boost::bind(&AysncLogging::threadFunc, this), "logging"),
        countDownLatch(1),
        mutex(),
        cond(mutex),
        curBufferPtr(new Buffer),
        nextBufferPtr(new Buffer),
        bufferVector()
{
    curBufferPtr->zero();
    nextBufferPtr->zero();
    bufferVector.reserve(16);
}

AysncLogging::~AysncLogging()
{
    if(running)
    {
        stop();
    }

//    fprintf(stdout, "AysncLogging::~AysncLogging()\n");
}

void AysncLogging::start()
{
    assert(running == false);
    running = true;
    thread.start();
    countDownLatch.wait();
}

void AysncLogging::stop()
{
    assert(running == true);
    running = false;
    cond.notifyAll();
    thread.join();
}

void AysncLogging::append(const char *logPtr, int len)
{
    klib::MutexLockGuard lockGuard(mutex);
    if(curBufferPtr->remained() > len)
    {
        curBufferPtr->append(logPtr, len);
    } else{
        // 这里使用的是双缓冲技术，
        bufferVector.push_back(curBufferPtr.release());
        if(nextBufferPtr)
        {
            curBufferPtr = boost::ptr_container::move(nextBufferPtr);
        } else{
            curBufferPtr.reset(new Buffer);
        }

        curBufferPtr->append(logPtr, len);
        cond.notify();
    }
}

void AysncLogging::threadFunc()
{
    assert(running == true);
    countDownLatch.countDown();
    BufferPtr newCurBufferPtr(new Buffer);
    BufferPtr newNextBufferPtr(new Buffer);
    BufferVector buffersWrite;
    LogFile logFile(baseName, rollSize, false);
    newCurBufferPtr->zero();
    newNextBufferPtr->zero();
    buffersWrite.reserve(16);
    while (running)
    {
        assert(newCurBufferPtr && newCurBufferPtr->length() == 0);
        assert(newNextBufferPtr && newNextBufferPtr->length() == 0);
        assert(buffersWrite.empty());
        {
            klib::MutexLockGuard lockGuard(mutex);
            // 若缓存为没有日志数据， 那么 IO 线程等待
            if(bufferVector.empty())
            {
                cond.timedWait(flushInterval);
            };
            if(!nextBufferPtr)
            {
                nextBufferPtr = boost::ptr_container::move(newNextBufferPtr);
            }

            bufferVector.push_back(curBufferPtr.release());
            curBufferPtr = boost::ptr_container::move(newCurBufferPtr);
            buffersWrite.swap(bufferVector);
        }

        assert(!buffersWrite.empty());

        if (buffersWrite.size() > 25)
        {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
                     TimeStamp::now().toString().c_str(),
                     buffersWrite.size()-2);
            fputs(buf, stderr);
            logFile.append(buf, static_cast<int>(strlen(buf)));
            buffersWrite.erase(buffersWrite.begin()+2, buffersWrite.end());
        }


        for(unsigned int i = 0; i < buffersWrite.size(); ++i)
        {
            logFile.append(buffersWrite[i].data(), buffersWrite[i].length());
        }

        if(buffersWrite.size() > 2)
        {
            buffersWrite.resize(2);
        }

        // 将 从缓冲中 取出两个缓冲重新填充 备用双缓冲里面去， 可以减少
        // 重新 new buffer 的开销提高效率
        if(!newCurBufferPtr)
        {
            assert(!buffersWrite.empty());
            newCurBufferPtr = buffersWrite.pop_back();
            newCurBufferPtr->reset();
        }

        if(!newNextBufferPtr)
        {
            assert(!buffersWrite.empty());
            newNextBufferPtr = buffersWrite.pop_back();
            newNextBufferPtr->reset();
        }

        logFile.flush();
        buffersWrite.clear();
    }

}