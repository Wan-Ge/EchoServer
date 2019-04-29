//
// Created by 程俊豪 on 2019/3/5.
//

#ifndef KLIB_THREADLOCAL_H
#define KLIB_THREADLOCAL_H

//以下主要是对线程私有数据 pthread_key_t 的封装
//以面向对象的思想管理全局变量，可以有效解决变量的线程安全问题
//不过针对基础变量或者 非POD类型， 我们还是建议使用 thread 修饰
// @auther 程俊豪

#include "base/noncopyable.h"
#include "base/mutex.h"

#include <pthread.h>

namespace klib
{

    template<class _Ty>
    class ThreadLocal : private klib::noncopyable
    {
    public:
        ThreadLocal()
        {
            M_CHECH(pthread_key_create(&key, &ThreadLocal::destructor));
        }
        ~ThreadLocal()
        {
            M_CHECH(pthread_key_delete(key));
        }

        _Ty& value()
        {
            _Ty* ptr = NULL;
            ptr = static_cast<_Ty*>(pthread_getspecific(key));

            if(!ptr)
            {
                ptr = new _Ty();
                M_CHECH(pthread_setspecific(key, ptr));
            }
            return *ptr;
        }
    private:
        static void destructor(void *arg)
        {
            // 此方案可以用来检测 该_Ty 是否完整
            // 因为typedef char type_must_be_complete[xxx] 表示定义一个固定大小的数组
            // 若 _Ty 是不完整类型那么在运行时错误转化为编译器错误， 编译器会检查 char[-1] 将不会通过编译

            // 置于什么是不完整类型 那就是只有声明没有定义， 但是 C++ 标准允许delete 删除指向不完全类型的指针，所以正式因为
            //编译器的这种拓展将未定义的行为转化成了编译器错误，有助于早日发现bug

            // 置于为什么要有(void) sizeof(type_must_be_complete) 这段代码， 主要是为了防止编译器优化
            // 因为如果系统检测到type_must_be_complete 未被使用的话可能就会被优化掉，而这句代码的目的就是为了告诉编译器
            // typedef 出来的类型 type_must_be_complete 是有用，不能被优化。 若type_must_be_complete被优化掉了， 那么
            // 检测在编译器检测 _Ty 是否是完整类型将会没有警告
            _Ty *ptr = static_cast<_Ty*>(arg);
            typedef char type_must_be_complete[sizeof(_Ty) ? 1 : -1];
            type_must_be_complete preventingCompilerOptimization;
            (void)preventingCompilerOptimization;
            delete ptr;
        }
        pthread_key_t key;
    };
}


#endif //KLIB_THREADLOCAL_H
