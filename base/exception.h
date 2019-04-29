//
// Created by 程俊豪 on 2019/3/4.
//

#ifndef KLIB_EXCEPTION_H
#define KLIB_EXCEPTION_H



#include <exception>
#include <string>

namespace klib
{
    class Exception : public std::exception
    {
    public:
        explicit Exception(const char* msg);
        explicit Exception(const std::string& str);
        virtual ~Exception() throw();
        virtual const char* what() const throw();
        const char* stackTrace() const throw();

    private:
        void fillInStackTrace();
        std::string message;
        std::string stackInfo;
    };
}

#endif //KLIB_EXCEPTION_H
