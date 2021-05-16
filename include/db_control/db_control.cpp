#pragma once

#include <nanodbc/nanodbc.h>
#include <lesson>
#include <map>

namespace db_control{

    /*
     * param CCmap uuid->ClassInfo
     * param skjc 上课节次
     *
     */
    void update_lesson_info(std::map<std::string, lesson> &CCmap, unsigned skjc){

        std::string date_stmt = std::string("select new_week, new_day from calendar");
        std::string find_stmt = std::string("");
        std::string week = "5";
        std::string day = "4";
        try{
//            获取上课周与星期,及学校日历,主要涉及到补课需求
            nanodbc::result date_row = nanodbc::execute(conn, date_stmt);
            week = date_row.get<std::string>(0);
            day = date_row.get<std::string>(1);

            //补全find_stmt语句
            find_stmt = std::string("SELECT\n"
                                    "U.uuid AS uuid,\n"
                                    "P.kch AS `kch`,\n"
                                    "P.kxh AS `kxh`,\n"
                                    "K.kcm AS `kcm`,\n"
                                    "P.jsxm AS `jsxm`,\n"
                                    "P.jxdd AS `jdxx`,\n"
                                    "FROM\n"
                                    "uuid_jxdd AS U\n"
                                    "INNER JOIN pksj AS P ON U.jxdd = P.jxdd AND U.jsszxqh = P.jsszxqh\n"
                                    "INNER JOIN kcb AS K ON P.kch = K.kch\n"
                                    "WHERE\n"
                                    "P.sksj <= " + skjc.c_str() + " AND\n"
                                    "P.sksj+P.cxjc\t >= " + skjc.c_str() +" AND\n"
                                    "P.skxq = " + day.c_str() + " AND\n"
                                     "SUBSTR(qsz," + week.c_str() + ",1)=\'1\'");
            //查询结果find_stmt
            nanodbc::result find_row = nanodbc::execute(conn, find_stmt);

            for(; find_row.next(); ){
                std::string uuid = find_row.get<std::string>(0);
                lesson lesson = lesson(find_row.get<std::string>(1),find_row.get<int>(2),find_row.get<std::string>(3),find_row.get<std::string>(4),find_row.get<std::string>(5))

                CCmap.insert(std::make_pair(uuid, lesson));
            }

        }catch(const std::exception& e){
//         输出至日志
        }
    }
}