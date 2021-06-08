#pragma once

#include "CliDescript.hpp"

struct Message {
    std::string title;
    std::string text;
    size_t expireTime;//seconds
};

namespace std {
    template<>
    struct hash<Message> : public __hash_base<size_t, Message> {
        size_t operator()(const Message &rhs) const noexcept    //这个const noexpect一定要写上去
        {
            return ((std::hash<std::string>()(rhs.title) << 1) ^
                    (std::hash<std::string>()(rhs.text) << 1) ^
                    (std::hash<size_t>()(rhs.expireTime))); //当然，可以使用其他的方式来组合这个哈希值,
            //这里是cppreference里面的例子，产生的数够乱就行。
        }
    };
}