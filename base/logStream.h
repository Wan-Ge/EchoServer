//
// Created by 程俊豪 on 2019/3/13.
//

#ifndef KLIB_LOGSTREAM_H
#define KLIB_LOGSTREAM_H

#include "base/StringPiece.h"
#include "base/noncopyable.h"

#include <string.h>
#include <string>
#include <assert.h>

#include <iostream>
namespace klib
{
    namespace detail
    {
        const int kSmallBufferSize = 4096;
        const int kBigBufferSize = 4096 * 1024; // 表示4MB

        template <int BUFFERSIZE>
        class FixedBuffer : private klib::noncopyable
        {
        public:
            FixedBuffer() : cur(buffer)
            {
                setCookie(cookieStart);
            }
            ~FixedBuffer() {setCookie(cookieEnd);}
            void append(const char* ptr, size_t len)
            {
                if(static_cast<size_t>(remained()) > len)
                {
//                    std::cout <<"debug\n";
//                    std::cout << cur << " " << len << "\n";
                    memcpy(cur,  ptr, len);
                    cur += len;
                }
            }

            const char* data() const
            {
                return buffer;
            }

            char* current()
            {
                return cur;
            }

            void add(size_t len)
            {
                cur += len;
            }


            void reset()
            {
                cur = buffer;
            }

            void zero()
            {
                ::bzero(buffer, BUFFERSIZE);
            }


            int length() const
            {
                return static_cast<int>(cur - buffer);
            }
            int remained() const
            {
                return static_cast<int>(end() - cur);
            }

            std::string toString() const
            {
                return std::string(buffer, length());
            }

            klib::StringPiece toStringPiece() const
            {
                return klib::StringPiece(buffer, length());
            }


            void setCookie(void(* cookie_)())
            {
                cookie = cookie_;
            }
//
//            const char* debug()
//            {
//                *cur = 0;
//                return buffer;
//            }
        private:
            static void cookieStart();
            static void cookieEnd();
            const char* end() const
            {
                return buffer + BUFFERSIZE;
            }
            char buffer[BUFFERSIZE];
            char* cur;
            void (*cookie)();
        };
    }


    class LogStream : public klib::noncopyable
    {
        typedef LogStream self;
    public:
        typedef detail::FixedBuffer<detail::kSmallBufferSize> Buffer;
        self& operator << (bool x)
        {
            buf.append( x ? "1" : "0", 1);
            return *this;
        }

        self& operator<< (short);
        self& operator<< (unsigned  short);
        self& operator<< (int);
        self& operator<< (unsigned int);
        self& operator<< (long );
        self& operator<< (unsigned long);
        self& operator<< (long long);
        self& operator<< (unsigned long long);

        self& operator<< (const void*);

        self& operator<< (float v)
        {
            *this << static_cast<double >(v);
            return *this;
        }

        self& operator<< (double);
        self& operator<< (long double);

        self& operator<< (char);
        self& operator<< (const  char*);
        self& operator<< (const unsigned char*);
        self& operator<< (const std::string& str);
        self& operator<< (const StringPiece& stringPiece);
        self& operator<< (const Buffer& buffer);


        const Buffer& buffer() const { return buf; }
        void reset() { buf.reset(); }
        void append(const char* ptr, size_t len){ buf.append(ptr, len); }
    private:
        void staticCheck();
        template <typename _Ty>
        void formatIngeterAndLog(_Ty t);
        Buffer buf;
        static const int kMaxNumericSize = 32;
    };


    class Fmt{
    public:
        template <typename _Ty>
        Fmt(const char* fmt, _Ty t);
        const char* data() const { return ptr;}
        int length() const { return len; }
    private:
        char ptr[32];
        int len;
    };

    inline LogStream& operator << (LogStream& log, const Fmt& fmt)
    {
        log.append(fmt.data(), fmt.length());
        return  log;
    }

}

#endif //KLIB_LOGSTREAM_H
