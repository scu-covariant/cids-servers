#pragma once

#include <string>
//#include <rapidjson>

struct Lesson final {
    std::string kch_{""};  //课程号
    int kxh_{0}; //课序号
    std::string kcm_{""}; //课程名
    std::string jsxm_{""}; //教师姓名
    std::string jxdd_{""}; //教学地点


//    JsonValue get_Event();

    Lesson(std::string kch, unsigned short kxh, std::string kcm, std::string jsxm, std::string jxdd) :
            kch_(kch), kxh_(kxh), kcm_(kcm), jsxm_(jsxm), jxdd_(jxdd) {}

    Lesson() {}

};