//
// Created by 程俊豪 on 2019/3/19.
//

#ifndef KLIB_WEAKCALLBACK_H
#define KLIB_WEAKCALLBACK_H

#include <functional>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace klib
{

    // 需要C++ 11 以上的版本支持
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    template<typename Class, typename... Args>
    class WeakCallback
    {
    public:
        WeakCallback(const boost::shared_ptr<Class>& sharedPtr,
                const std::function<void(Class*, Args ...)> & func)
                : weakPtr(sharedPtr),
                  function(func)
        {

        }

        void operator() (Args&&...  args) const
        {
            boost::shared_ptr<Class> ptr(weakPtr.lock());
            if(ptr)
            {
                function(ptr.get(), std::forward<Args>(args)...);
            }
        }

    private:
        boost::weak_ptr<Class> weakPtr;
        const std::function<void(Class*, Args...)> function;
    };

    template <typename Class, typename... Args>
    WeakCallback<Class, Args...>  makeWeakCallback(const boost::shared_ptr<Class>& sharedPtr,
                                                    void (Class::*function)(Args...))
    {
        return WeakCallback<Class, Args...>(sharedPtr, function);
    }

    template <typename Class, typename... Args>
    WeakCallback<Class, Args...>  makeWeakCallback(const boost::shared_ptr<Class>& sharedPtr,
                                                   void (Class::*function)(Args...) const)
    {
        return WeakCallback<Class, Args...>(sharedPtr, function);
    }
#else

    // 老版本C++ 支持 C++ 98
    template<typename Class>
    class WeakCallback
    {
    public:
        WeakCallback(const boost::shared_ptr<Class>& sharedPtr,
                    const boost::function<void(Class*)> func)
        : weakPtr(sharedPtr),
          function(func)
        {

        }

        void operator()() const
        {
            boost::shared_ptr<Class> ptr(weakPtr.lock());
            if(ptr)
            {
                function(ptr.get());
            }
        }
    private:
        const boost::weak_ptr<Class> weakPtr;
        const boost::function<void(Class*)> function;
    };

    template<typename Class>
    WeakCallback<Class> makeWeakCallback(const boost::shared_ptr<Class>& ptr,
                                         void (Class::*function)())
    {
        return WeakCallback<Class>(ptr, function);
    }

    template<typename Class>
    WeakCallback<Class> makeWeakCallback(const boost::shared_ptr<Class>& ptr,
                                         void (Class::*function)() const)
    {
        return WeakCallback<Class>(ptr, function);
    }
#endif
}

#endif //KLIB_WEAKCALLBACK_H
