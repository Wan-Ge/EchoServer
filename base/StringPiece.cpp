//
// Created by 程俊豪 on 2019/2/23.
//

#include "base/StringPiece.h"
namespace klib
{

    StringPiece::StringPiece(StringPiece x, int pos, int len) :
        ptr_(ptr_ + pos),
        length_(std::min(len, x.length_ - pos))
    {

    }
    StringPiece::StringPiece(StringPiece x, int pos) : ptr_(ptr_ + pos), length_(x.length_ - pos)
    {

    }
    void StringPiece::CopyToString(string* target)  const
    {
        target->append(ptr_, length_);
    }
    void StringPiece::AppendToString(string* target) const
    {
        target->assign(ptr_, length_);
    }
}