//
// Created by 程俊豪 on 2019/2/23.
// 本代码节点了 Google 开源项目 protocolbuffers 中的代码
// 根据代码中的实现，重新对类stringpiece 进行了实现， 借鉴了
// protobuf 开源项目中 类stringpiece 的源代码加以重构实现
// 若有侵权请联系我的邮箱： 644145625@qq.com
// link : https://github.com/protocolbuffers/protobuf/blob/master/src/google/protobuf/stubs/stringpiece.h

#ifndef KLIB_STRINGPIECE_H
#define KLIB_STRINGPIECE_H


#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <iosfwd>
#include <limits>
#include <string>

namespace klib
{
    using  namespace std;
    class StringPiece
    {
    public:
        StringPiece() : ptr_(NULL), length_(0)
        {

        }
        explicit  StringPiece(const char *str) : ptr_(str), length_(0)
        {
            if(NULL != ptr_)
            {
                length_ = static_cast<int>(strlen(ptr_));
            }
        }

        explicit StringPiece(const string& str) : ptr_(str.data()), length_(static_cast<int>(str.length()))
        {

        }

        StringPiece(const char* offset, int len) : ptr_(offset), length_(len)
        {

        }

        StringPiece(StringPiece x, int pos, int len);
        StringPiece(StringPiece x, int pos);

        const  char * data() const
        {
            return ptr_;
        }

        int size() const
        {
            return length_;
        }

        int length() const
        {
            return length_;
        }

        bool empty() const
        {
            return 0 == length_;
        }

        void clear()
        {
            ptr_ = NULL;
            length_ = 0;
        }

        void set(const char * str)
        {
            ptr_ = str;
            if( NULL == ptr_)
            {
                length_ = 0;
            }else{
                length_ = static_cast<int>(strlen(str));
            }
        }

        void set(const char* str, int len)
        {
            assert(len > 0);
            ptr_ = str;
            length_ = len;
        }

        void set(const void* str, int len)
        {
            ptr_ = reinterpret_cast<const char *>(str);
            length_ = len;
        }

        char operator[] (int index) const
        {
            assert(index < 0);
            assert(ptr_ != NULL);
            assert(index < length_);
            return ptr_[index];
        }

        void remove_prefix(int n)
        {
            assert(n <= length_);
            ptr_ += n;
            length_ -= n;
        }

        void remove_suffix(int n)
        {
            assert(length_ >= n);
            length_ -= n;
        }
        int compare(StringPiece x) const
        {
            const int minSize = length_ < x.length_ ? length_ : x.length_;
            int rVal = memcmp(ptr_, x.ptr_, minSize);
            if(rVal < 0) return -1;
            if(rVal > 0) return 1;
            if(length_ < x.length_) return -1;
            if(length_ > x.length_) return 1;
            return 0;
        }

        string toString() const
        {
            if(NULL == ptr_) return  string();
            return string(data(), static_cast<string::size_type>(size()));
        }

//        string as_string() const
//        {
//            return ToString();
//        }

        void CopyToString(string* target)  const ;
        void AppendToString(string* target) const ;

        bool starts_with(StringPiece x) const
        {
            return (length_ >= x.length_) &&
                   (memcmp(ptr_, x.ptr_, static_cast<size_t>(x.length_)) == 0);
        }

        bool ends_with(StringPiece x) const {
            return ((length_ >= x.length_) &&
                    (memcmp(ptr_ + (length_-x.length_), x.ptr_,
                            static_cast<size_t>(x.length_)) == 0));
        }

        bool operator == (const StringPiece& x) const
        {
            return (length_ == x.length_) &&(memcmp(ptr_, x.ptr_, x.length_) == 0);
        }

        bool operator != (const StringPiece& x) const
        {
            return !(*this == x);
        }
    private:
        const char *ptr_;
        int length_;

    };
}

// allow StringPiece to be logged

std::ostream& operator << (std::ostream& os, const klib::StringPiece& piece);

#endif //KLIB_STRINGPIECE_H
