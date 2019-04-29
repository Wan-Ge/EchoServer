#ifndef KLIB_NONCOPYABLE_H
#define KLIB_NONCOPYABLE_H

namespace klib
{


    class noncopyable
    {
    protected:
    // 判断是否支持 C++11
    #if __cplusplus >= 201103L     
        noncopyable() = default;
        ~noncopyable() = default;
    #else
         noncopyable() {}
         ~noncopyable() {}
    #endif



    #if  __cplusplus >= 201103L
        noncopyable(const noncopyable& ) = delete;
        noncopyable& operator=(const noncopyable& ) = delete;
    #else
    private:
        noncopyable(const noncopyable& );
        noncopyable& operator=(const noncopyable& );
    #endif
    };

}


#endif  //KLIB_NONCOPYABLE_H