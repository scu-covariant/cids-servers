//
// Created by XM on 2021/5/20.
//

#pragma once

#include <string>

namespace Cli{
    using Uuid = unsigned int;
}

struct CliDes{
    Cli::Uuid id;
    size_t campus;
    std::string stamp;
};

struct ClassPlace final{
    std::string jxdd_;  //教室地点
    unsigned int jsszxqh; //教室所在校区号
};
