#pragma once

#include <string>
#include <unordered_set>
//暂时以string代表课程信息

//classinfo中应该有一个bool量表示是否发送过, 发送过置否, 每次更新置真

struct ClassPlace final{
    std::string jsdd_;  //教室地点
    unsigned jsszxqh; //教室所在校区号
};

struct ClassInfo final{
    std::string jsdd_; //教师地点
    std::unordered_set<lesson> lessons_;  //课程信息
    bool status_; //true->已发送
};
