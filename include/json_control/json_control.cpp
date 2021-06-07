#pragma once

#include "../mirror/ClassMes.hpp"
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace json_control{

void getJson(rapidjson::Writer<rapidjson::StringBuffer>&writer, const Message& mes){
    //开启对象
    writer.StartObject();
    //title
    writer.Key("Title");
    writer.String(mes.title.c_str());

    //text
    writer.Key("text");
    writer.String(mes.text.c_str())

    //expiretime
    writer.Key("expireTime");
    writer.String(mes.expireTime.c_str());

    //结束对象
    writer.EndObject();
}

void getJson(rapidjson::Writer<rapidjson::StringBuffer>&writer, const lesson &les){
    //开始对象
    writer.StartObject();

    //课程号
    writer.Key("kch");
    writer.String(les.kch_.c_str());

    //课序号
    writer.Key("kxh");
    writer.Int(les.kxh_.c_str())

    //课程名
    writer.Key("kcm");
    writer.String(les.kcm_.c_str());

    //教师姓名
    writer.Key("jsxm");
    writer.String(les.jsxm_.c_str());

    //教学地点
    writer.Key("jxdd");
    writer.String(les.jxdd_.c_str());

    //结束对象
    writer.EndObject();
}
void getJsonInfo(const ClassMes& mes, const muduo::Timestamp& timestamp) {
    rapidjson::StringBuffer strBuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
    //声明strBuf与writer对象

    //开启json对象
    writer.StartObject();

    //时间戳
    writer.Key("time");
    writer.String(timestamp.toString().c_str());

    //图像的url
    writer.Key("image_url");
    writer.String(mes.image_url.c_str());

    //消息队列
    writer.Key("message");
    //开启数组
    writer.StartArray();
    for (auto iter = mes.messages.begin(); iter != mes.messages.end(); iter++) {
        getJson(writer, *iter);
    }
    //结束数组
    writer.EndArray();

    //第一节课信息
    writer.Key("event");

    getJson(writer, mes.lesson_1);

    //第二节课信息
    writer.Key("next_event");

    getJson(writer, mes.lesson_2);

    //需要更新
    writer.Key("needUpdate");
    writer.Bool(true);

    //结束json对象
    writer.EndObject();

    std::string json = strBuf.GetString();

    return json;
}

}//End namespace of json_control
