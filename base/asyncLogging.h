//
// Created by 程俊豪 on 2019/3/19.
//

#ifndef KLIB_ASYNCLOG_H
#define KLIB_ASYNCLOG_H

#include "base/noncopyable.h"
#include "base/blockingQueue.h"
#include "base/boundedBlockingQueue.h"
#include "base/countDownLatch.h"
#include "base/mutex.h"
#include "base/thread.h"
#include "base/logStream.h"

#include <stdint.h>
#include <time.h>
#include <string>

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace klib
{
    class AysncLogging : private klib::noncopyable
    {
    public:
        AysncLogging(const std::string& baseName_,
                     int64_t rollSize_,
                     int flushInterval_ = 3);
        ~AysncLogging();

        void append(const char* logPtr, int len);
        void append(const std::string& str) { append(str.c_str(), static_cast<int>(str.length())); }
        void stop();
        void start();


    private:
        //声明， 可以阻止编译器合成函数。 这里必须加因为在ptr_container当中编译会不过
        AysncLogging(const AysncLogging&);
        void operator=(const AysncLogging&);

        void threadFunc();
        typedef klib::detail::FixedBuffer<klib::detail::kBigBufferSize> Buffer;
        typedef boost::ptr_vector<Buffer> BufferVector;
        typedef BufferVector::auto_type BufferPtr;

        const int flushInterval; // flush 到时间间隔
        bool running;
        std::string baseName;  //基础名字， 可以用来构建日志文件的名称
        const int64_t rollSize;  // 表时一个日志文件的最大大小
        klib::Thread thread;
        klib::CountDownLatch countDownLatch;
        klib::MutexLock mutex;
        klib::Condition cond;

        // 双缓冲指针
        BufferPtr curBufferPtr;
        BufferPtr nextBufferPtr;

        //缓冲池
        BufferVector bufferVector;
    };
}
#endif //KLIB_ASYNCLOG_H
