//
// Created by 程俊豪 on 2019/3/9.
//

#ifndef KLIB_ATOMIC_H
#define KLIB_ATOMIC_H

#include "base/noncopyable.h"

#include <stdint.h>

// Built-in functions for atomic memory access（用于原子存储器访问的内置函数）
// gcc 允许任意长度为 1 2 4 8 字节的整数标量或者指针类型，竞选原子修改
// 而参考因特尔给出的定义是只允许使用的类型是 int、 long 、 long long以及对应的无符号类型
//参考资料https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html


namespace klib
{
    // thread safe
    namespace detail
    {
        template<class _Ty>
        class AtomicInteger : private klib::noncopyable
                {
        public:
            AtomicInteger() : atomicNum() {}

            explicit AtomicInteger(const _Ty& t) : atomicNum(t) {}


            _Ty get()
            {
                return __sync_val_compare_and_swap(&atomicNum, 0, 0);
            }


            /**
            bool __sync_bool_compare_and_swap (类型 *ptr, 类型 oldval 类型 newval, ...)
            类型 __sync_val_compare_and_swap (类型 *ptr, 类型 oldval 类型 newval, ...)
            这些内置函数执行原子比较和交换。也就是说，如果*ptr的当前值是oldval，则将newval写入 *ptr。
            如果比较成功并且写入了newval，则“bool”版本返回true 。“val”版本在操作之前返回*ptr的内容。
            */
            _Ty getAndAdd(_Ty t)
            {
                // __sync_fetch_and_add (类型 *ptr, 类型 value, ...)
                return __sync_fetch_and_add(&atomicNum, t);
            }

            _Ty addAndGet(_Ty t)
            {
                // __sync_add_and_fetch (类型 *ptr, 类型 value, ...)
                return __sync_add_and_fetch(&atomicNum, t);
            }

            _Ty getAndSub(_Ty t)
            {
                // __sync_fetch_and_sub (类型 *ptr, 类型 value, ...)
                return __sync_fetch_and_sub(&atomicNum, t);
            }

            _Ty subAndGet(_Ty t) {
                //__sync_sub_and_fetch (类型 *ptr, 类型 value, ...)
                return __sync_sub_and_fetch(&atomicNum, t);
            }

            /**

            类型 __sync_fetch_and_or (类型 *ptr, 类型 value, ...)
            类型 __sync_fetch_and_and (类型 *ptr, 类型 value, ...)
            类型 __sync_fetch_and_xor (类型 *ptr, 类型 value, ...)
            类型 __sync_fetch_and_nand (类型 *ptr, 类型 value, ...)
            这些内置函数执行名称建议的操作，并返回先前在内存中的值。那是，
                      {tmp = * ptr; * ptr op = value; 返回tmp; }
                      {tmp = * ptr; * ptr = ~tmp＆value; 返回tmp; } // nand


            类型 __sync_or_and_fetch (类型 *ptr, 类型 value, ...)
            类型 __sync_and_and_fetch (类型 *ptr, 类型 value, ...)
            类型 __sync_xor_and_fetch (类型 *ptr, 类型 value, ...)
            类型 __sync_nand_and_fetch (类型 *ptr, 类型 value, ...)
            这些内置函数执行名称建议的操作，并返回新值。那是，
                  {* ptr op = value; return * ptr; }
                  {* ptr =?* ptr＆value; return * ptr; } // nand
            */





            _Ty incrementAndGet()
            {
                return addAndGet(1);
            }

            _Ty getAndIncrement()
            {
                return getAndAdd(1);
            }

            _Ty decrementAndGet()
            {
                return subAndGet(-1);
            }

            _Ty getAndDecrement()
            {
                return getAndSub(-1);
            }

            void increment()
            {
                incrementAndGet();
            }

            void decrement()
            {
                decrementAndGet();
            }

            void add(_Ty t)
            {
                getAndAdd(t);
            }

            void sub(_Ty t)
            {
                getAndSub(t);
            }

            _Ty getAndSet(_Ty t)
            {
                //__sync_lock_test_and_set (类型 *ptr, 类型 value, ...)
                return __sync_lock_test_and_set(&atomicNum, t);
            }

        private:
            volatile _Ty atomicNum;
        };
    }

    typedef detail::AtomicInteger<int32_t>  AtomicInt32;
    typedef detail::AtomicInteger<int64_t > AtomicInt64;


}

#endif //KLIB_ATOMIC_H
