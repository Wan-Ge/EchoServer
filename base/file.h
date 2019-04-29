//
// Created by 程俊豪 on 2019/3/21.
//

#ifndef KLIB_FILE_H
#define KLIB_FILE_H

#include "base/noncopyable.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string>

#include <boost/static_assert.hpp>
namespace klib
{
    namespace file
    {
        //缓冲区大小， 读文件和写文件的缓冲区大小皆可
        const int kBufferSize = 128*1024;
        class AppendFile : private klib::noncopyable
        {
        public:
            explicit AppendFile(const char* baseName) : nbytes(0), fp(NULL)
            {
                //fopen 普通文件默认是全缓冲
                fp = ::fopen(baseName, "ae");  // O_APPEND & O_CLOEXEC
                assert(fp != NULL);
                BOOST_STATIC_ASSERT(sizeof(buffer) == kBufferSize);
                ::setbuffer(fp, buffer, kBufferSize);
            }
            ~AppendFile();

            void append(const char* ptr, int len);
            uint64_t writeBytes() const { return nbytes; }
            void fflush()
            {
                ::fflush(fp);
            }
        private:
            size_t write(const char* ptr, int len);
            FILE* fp;
            char buffer[kBufferSize];
            uint64_t nbytes;
        };


        //读小文件缓存为128kb
        class ReadSmallFile : private klib::noncopyable
        {
        public:
                ReadSmallFile(const char* baseName);
                ~ReadSmallFile();

                // return errno
                int readToString(int maxSize,
                                std::string* str,
                                int64_t* fileSize ,
                                int64_t* modifyTime,
                                int64_t* createTime);

                int readToBuffer(int *maxSize);
                const char* getBuffer() const { return buffer; }

        private:
            char buffer[kBufferSize];
            int err;
            int fd;
        };

        //
        int readFile(const char* baseName,
                     int maxSize,
                     std::string* str,
                     int64_t* filesize = NULL,
                     int64_t* modifytime = NULL,
                     int64_t* createtime = NULL);
//        {
//            ReadSmallFile file(baseName);
//            return file.readToString(maxSize, str, filesize, modifytime, createtime);
//        }

    } // end namespace file
}// end namespace klib


#endif //KLIB_FILE_H
