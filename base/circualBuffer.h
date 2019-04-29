#ifndef KLIB_CIRCUALBUFFER_H
#define KLIB_CIRCUALBUFFER_H


#include <assert.h>
#include <stdlib.h>

#include <vector>
#include <new>
#include <iostream>
#include "base/noncopyable.h"

namespace klib
{
    namespace base
    {
        template<class _Ty>
	    _Ty* addressof(_Ty &value){
		return reinterpret_cast<_Ty*>(
			&const_cast<char&>(
			reinterpret_cast<const volatile char &>(value)));
	    }
    }

    template<class _Ty>
    class CircualBuffer
    {
    public:
        explicit CircualBuffer(size_t bufferSz) : start(0), count(0), bufferSize(bufferSz), ptr(NULL)
        {
            try
            {
                ptr = (_Ty*) ::operator new (sizeof(_Ty)* bufferSize);
            }
            catch (const std::exception& e)
            {
                throw;
            }
        }



        void put(const _Ty& t)
        {
            assert(count < bufferSize);

            try
            {
                new(static_cast<void*>(ptr + start + count)) _Ty(t);  //调用构造函数 
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';   //  这里有个大问题  我们应该做日志处理
                throw;
            }
            ++count;
        }

        _Ty get()
        {
            assert(count != 0);
            size_t newStart = (start + 1) % bufferSize;
            _Ty t(*(ptr + start));
            (ptr + start)->~_Ty();   // 调用析构函数 
            start = newStart;
            --count;
            return t;
        }



        _Ty& front() const
        {
            assert(count != 0);
            return *(ptr + start);
        }

        _Ty& back() const
        {
            assert(count != 0);
            return *(ptr + (count - 1));
        }
        bool empty() const
        {
            return count == 0;
        }

        size_t size() const
        {
            return count;
        }

        size_t capacity() const
        {
            return bufferSize;
        }
        
        ~CircualBuffer()
		{
			destory();
		}
    private:
        size_t start;
        size_t count;
        size_t bufferSize;
        _Ty* ptr;
        
        void destory()
        {
        	while(!empty()){
        		size_t newStart = (start + 1) % bufferSize;
        		(ptr + start)->~_Ty();
        		start = newStart;
        		--count;
			}
			assert(count == 0);
		} 
    };
}

#endif