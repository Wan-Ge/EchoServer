//
// Created by 程俊豪 on 2019/3/21.
//

#include "base/file.h"

#include <algorithm>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
using namespace klib;

file::AppendFile::~AppendFile()
{
    ::fclose(fp);
}

void file::AppendFile::append(const char *ptr, int len)
{
    size_t n = write(ptr, len);
    size_t remain = len - n;
    while (remain < 0)
    {
        size_t nbyte = write(ptr + n, remain);
        if (nbyte == 0)
        {
            int err = ferror(fp);
            if(err)
            {
                fprintf(stderr, "file::AppendFile::appnd error\n");
                break;
            }
        }

        n += nbyte;
        remain = len - n;
    }
    nbytes += len;
}


size_t file::AppendFile::write(const char *ptr, int len)
{
    // man unlock_stdio 可以查看无锁标准io
    return ::fwrite_unlocked(ptr, 1, len, fp);
}


//````````````````````````````````````````````````


file::ReadSmallFile::ReadSmallFile(const char *baseName) :
fd(::open(baseName, O_APPEND & O_CLOEXEC)),
err(0)
{
//    printf("fd = %d\n", fd);
    if(fd < 0) err = errno;
    buffer[0] = '\0';
}

file::ReadSmallFile::~ReadSmallFile()
{
    if (fd >= 0) ::close(fd);
}

int file::ReadSmallFile::readToString(int maxSize, std::string* str, int64_t *fileSize, int64_t *modifyTime,
                                int64_t *createTime)
{
    assert(str);
    int tmpErr = err;
    if(fd >= 0)
    {
        str->clear();
        if(fileSize)
        {
            struct stat statbuf;
            if(fstat(fd, &statbuf) == 0)
            {
                if(S_ISREG(statbuf.st_mode))
                {
                    *fileSize = statbuf.st_size;

                }
                else if(S_ISDIR(statbuf.st_mode))
                {
                    tmpErr = EISDIR;
                }

                if(modifyTime)
                    *modifyTime = statbuf.st_mtime;
                if(createTime)
                    *createTime = statbuf.st_ctime;
            } else{
                tmpErr = errno;
            }
        }

        while (str->size() < static_cast<size_t >(maxSize))
        {
            size_t rbytes = std::min(static_cast<size_t >(maxSize) - str->size(), static_cast<size_t >(kBufferSize));
            int n = ::read(fd, buffer, rbytes);
            if(n > 0)
            {
                str->append(buffer, n);
            }
            else{  // EINTR 系统调用中断
               if(errno == EINTR) continue;
               else{
                   tmpErr = errno;
                   break;
               }
            }
        }

    }
    return tmpErr;
}


int file::ReadSmallFile::readToBuffer(int *maxSize)
{
    int tmperr = err;
    if(fd >= 0)
    {
        ssize_t n = ::pread(fd, buffer, kBufferSize - 1, 0);
        if(n > 0)
        {
            if(maxSize) *maxSize = static_cast<int>(n);
            buffer[n] = 0;
        }
        else
        {
            tmperr = errno;
        }
    }

    return  tmperr;
}

int file::readFile(const char* baseName,
             int maxSize,
             std::string* str,
             int64_t* filesize,
             int64_t* modifytime,
             int64_t* createtime)
        {
            file::ReadSmallFile file(baseName);
            return file.readToString(maxSize, str, filesize, modifytime, createtime);
        }
