//
// Created by 程俊豪 on 2019/3/5.
//

#include "base/exception.h"

#include <execinfo.h>   // linux 下用來追踪堆栈的函数
#include <stdio.h>
#include <stdlib.h>

using namespace klib;
using namespace std;

Exception::Exception(const char *msg) : message(msg)
{
    stackInfo.reserve(1024);
    fillInStackTrace();
}

Exception::Exception(const string &str) : message(str)
{
    stackInfo.reserve(1024);
    fillInStackTrace();
}

Exception::~Exception() throw()
{

}

void Exception::fillInStackTrace()
{
    // 本代码主要是借鉴了  https://www.gnu.org/software/libc/manual/html_node/Backtraces.html
    // 通过该函数我们可以追踪当前栈抛出异常后的堆栈的相关错误信息
    // 有助于定位错误
    // 关于 backtrace 函数：该函数用与获取当前线程的调用堆栈,获取的信息将会被存放在buffer中,它是一个指针列表
    //                注意：某些编译器的有关选项对获取正确的调用栈有影响，另外内联函数没有堆栈架构，删除框架指针无法正确解析其中内容
    //
    // 和 backtrace_symbols 函数：backtrace函数获取的信息转化为一个字符串数组.
    //                      注意：函数的返回值是  malloc 申请的空间，最后必须 free 释放掉
    size_t size = 0;
    void *array[300];
    char **strings = NULL;
    size_t i = 0;

    size = ::backtrace(array, 300);
    strings = ::backtrace_symbols(array, size);
    if(strings)
    {
        for(i = 0; i < size; ++i)
        {
            stackInfo.append(strings[i]);
            stackInfo.append("\n");
        }
        free(strings);
    }

}

const char* Exception::what() const throw()
{
    return message.c_str();
}

const char* Exception::stackTrace() const throw()
{
    return stackInfo.c_str();
}