//
// Created by 程俊豪 on 2019/2/23.
//

#ifndef KLIB_SINGLETON_H
#define KLIB_SINGLETON_H

#include <pthread.h>
#include <assert.h>

#include "base/noncopyable.h"

namespace klib
{



    template<class _Ty>
    class Singleton : private klib::noncopyable
    {
    public:

        static _Ty* instance()
        {
            pthread_once(&once, &(Singleton::init));
         //   assert(interface != NULL);
            return interface;
        }
    private:
        Singleton();
        ~Singleton();

        static  void init() throw()
        {
            assert(interface == NULL);
            interface = new _Ty();
        }





    private:
        static pthread_once_t once;
        static _Ty* interface;
    };

    template<class _Ty>
    pthread_once_t Singleton<_Ty>::once = PTHREAD_ONCE_INIT;

    template<class _Ty>
    _Ty* Singleton<_Ty>::interface = NULL;

}

#endif //KLIB_SINGLETON_H
