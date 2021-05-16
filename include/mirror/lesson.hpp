#pragma once
#include <string>
//#include <rapidjson>

struct lesson final{
    std::string kch_;  //课程号
    int kxh_; //课序号
    std::string kcm_; //课程名
    std::string jsxm_; //教师姓名
    std::string jxdd_; //教学地点


//    JsonValue get_Event();

    lesson( std::string kch, unsigned short kxh, std::string kcm, std::string jsxm,std::string jxdd):
            kch_(kch), kxh_(kxh), kcm_(kcm), jsxm_(jsxm), jxdd_(jxdd) {}

    friend std::ostream& operator<<(std::ostream& out, const lesson& l){
        out << "kch:  " << l.kch_ << ", kxh:  " << l.kxh_ << ", kcm:  " << l.kcm_ << ", jsxm:  " << l.jsxm_ << ", jxdd:  " << l.jxdd_;
        return out;
    }
};