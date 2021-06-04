#pragma once

#include <string>
#include <unordered_set>
#include <deque>

#include "Lesson.hpp"
#include "CliDescript.hpp"
#include "Message.hpp"

struct ClassMes final{
    //ClassPlace classPlace_; //教室地点 暂不做内容发送
    Lesson lesson_1;  //课程信息
    Lesson lesson_2;
    std::string image_url;
    std::deque<Message> messages;
};
