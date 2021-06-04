#pragma once

#include <string>

struct  BackGround{
    int PicId;
    std::string PicCode;
    std::string PicDes;

    BackGround(int id, const std::string& code,const std::string &des): PicId(id), PicCode(code), PicDes(des){};
};