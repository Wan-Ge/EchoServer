//
// Created by 程俊豪 on 2019/3/13.
//


#include "base/logStream.h"

#include <algorithm>
#include <iostream>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>



namespace klib
{
    namespace detail
    {
        const char digits[] = "9876543210123456789";
        BOOST_STATIC_ASSERT(sizeof(digits) == 20);
        const char* zero = digits + 9;
        const char digitsHex[] = "0123456789ABCDEF";
        BOOST_STATIC_ASSERT(sizeof(digitsHex) == 17);


        template<typename _Ty>
        size_t integerToStr(char buf[], _Ty t) {
            _Ty value = t;
            char *ptr = buf;
            do {
                int remainder = static_cast<int>(value % 10);
                value /= 10;
                *ptr++ = zero[remainder];
            } while (value != 0);
            if (t < 0) {
                *ptr++ = '-';
            }

            *ptr = '\0';
            std::reverse(buf, ptr);
            return ptr - buf;
        }

        size_t integerToHex(char buf[], uintptr_t t) {
            uintptr_t value = t;
            char *ptr = buf;
            do {
                int remainder = static_cast<int>(value % 16);
                value /= 16;
                *ptr++ = digitsHex[remainder];
            } while (value != 0);
            assert(value == 0);
            *ptr = '\0';
            std::reverse(buf, ptr);
            return ptr - buf;
        }
        template class FixedBuffer<kSmallBufferSize>;
        template class FixedBuffer<kBigBufferSize>;
    }
}

using namespace klib;
using namespace klib::detail;
template <int BUFFERSIZE>
void FixedBuffer<BUFFERSIZE>::cookieStart()
{

}

template <int BUFFERSIZE>
void FixedBuffer<BUFFERSIZE>::cookieEnd()
{

}

template <typename _Ty>
void LogStream::formatIngeterAndLog(_Ty t)
{
    if(buf.remained() >= kMaxNumericSize)
    {
        size_t len = integerToStr(buf.current(), t);
        buf.add(len);
    }
}

LogStream& LogStream::operator<<(short num)
{
    *this << static_cast<int>(num);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short num)
{
    *this << static_cast<unsigned int>(num);
    return *this;
}

LogStream& LogStream::operator<<(int num)
{
    formatIngeterAndLog(num);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int num)
{
    formatIngeterAndLog(num);
    return *this;
}

LogStream& LogStream::operator<<(long num)
{
    formatIngeterAndLog(num);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long num)
{
    formatIngeterAndLog(num);
    return *this;
}

LogStream& LogStream::operator<<(long long num)
{
    formatIngeterAndLog(num);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long num)
{
    formatIngeterAndLog(num);
    return *this;
}

LogStream& LogStream::operator<<(const void* ptr)
{
    uintptr_t uintptr = reinterpret_cast<uintptr_t >(ptr);
    if(kMaxNumericSize <= buf.remained())
    {
        char ch[32];
        ch[0] = '0';
        ch[1] = 'x';
        int len = detail::integerToHex(ch + 2, uintptr);
        buf.append(ch, len + 2);
    }
    return *this;
}


LogStream& LogStream::operator<<(double num)
{
    if(buf.remained() >= kMaxNumericSize)
    {
        int len = snprintf(buf.current(), kMaxNumericSize, "%.15g", num);
        buf.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double num)
{
    if(buf.remained() >= kMaxNumericSize)
    {
        int len = snprintf(buf.current(), kMaxNumericSize, "%.15g", num);
        buf.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(char num)
{
    buf.append(&num, 1);
    return *this;
}

LogStream& LogStream::operator<<(const char* ptr)
{

    if(ptr){
        buf.append(ptr, strlen(ptr));
    } else{
        buf.append("(null)", 6);
    }
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* ptr)
{
    *this << reinterpret_cast<const char*>(ptr);
    return *this;
}

LogStream& LogStream::operator<<(const std::string& str)
{
    buf.append(str.c_str(), str.length());
    return *this;
}

LogStream& LogStream::operator<<(const StringPiece& stringPiece)
{
    buf.append(stringPiece.data(), stringPiece.length());
    return *this;
}

LogStream& LogStream::operator<<(const Buffer& buffer)
{
    buf.append(buffer.data(), buffer.length());
    return *this;
}

void LogStream::staticCheck()
{
    BOOST_STATIC_ASSERT(kMaxNumericSize - 10 > std::numeric_limits<double>::digits10);
    BOOST_STATIC_ASSERT(kMaxNumericSize - 10 > std::numeric_limits<long double>::digits10);
    BOOST_STATIC_ASSERT(kMaxNumericSize - 10 > std::numeric_limits<long>::digits10);
    BOOST_STATIC_ASSERT(kMaxNumericSize - 10 > std::numeric_limits<long long>::digits10);
}




template <typename _Ty>
Fmt::Fmt(const char *fmt, _Ty t)
{
    BOOST_STATIC_ASSERT(boost::is_arithmetic<_Ty>::value == true);
    len = snprintf(ptr, sizeof(ptr), fmt, t);
    assert(static_cast<size_t>(len) < sizeof(ptr));
}


template Fmt::Fmt(const char* fmt, char);
template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);
template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);