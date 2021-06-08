#pragma once

#include <string>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/reader.h>
#include <rapidjson/document.h>

#include <muduo/base/Timestamp.h>

#include <unordered_map>
#include <map>
#include <nanodbc/nanodbc.h>

#include <mirror_head>
#include <common_head>


namespace db_control {
    using Cli::Uuid;

//    void check_week_and_day(nanodbc::connection conn, scu_date &date);

    /*
     * param CCmap uuid->ClassMes
     * param skjc 上课节次
     *
     */
    //暂停使用
//    void
//    query_lessons(nanodbc::connection conn, size_t skjc, bool two_lesson, std::unordered_map<Uuid, ClassMes> &CCmap,
//                  scu_date *date, Uuid id = -1);


    void query_background(nanodbc::connection &conn, std::vector<BackGround>& backgrounds);

    std::string find_pic(nanodbc::connection &conn, Cli::Uuid id);

    std::deque<Message> find_mes(nanodbc::connection &conn, Cli::Uuid id, const std::string &stamp);

    Lesson find_lesson(nanodbc::connection &conn, Cli::Uuid id, int skjc, const scu_date &date);

    int find_campus(nanodbc::connection &conn, Cli::Uuid id);

    scu_date update_calendar(nanodbc::connection &conn, int year, int month, int date);
}


namespace json_control {
    extern std::string no_need_update;

    void getJson(rapidjson::Writer <rapidjson::StringBuffer> &writer, const Message &mes);

    void getJson(rapidjson::Writer <rapidjson::StringBuffer> &writer, const Lesson &les);

    std::string getJsonInfo(const ClassMes &mes, const std::string &timestamp);

    void getIdAndStamp(const std::string& src, Cli::Uuid &id, std::string& stamp);
}//End namespace of json_control

namespace codec{
    void downPic(const std::string& path, const std::string &code);

}