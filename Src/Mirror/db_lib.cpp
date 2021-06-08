#include <mirror_head>
#include <common_head>

namespace db_control {

    using Cli::Uuid;

    /*
    void check_week_and_day(nanodbc::connection conn, scu_date &date) {
        if (date.day == 7) {
            date.week += 1;
            date.day = 1;
        } else {
            date.day += 1;
        }
    }
*/
    /*
     * param CCmap uuid->ClassMes
     * param skjc 上课节次
     *
     */
/*暂停使用
    void
    query_lessons(nanodbc::connection conn, size_t skjc, bool two_lesson, std::unordered_map <Uuid, ClassMes> &CCmap,
                  scu_date *date, Uuid id) {
        size_t week = date->week; //第几周
        size_t day = date->day; //星期几
        bool new_client = false;
        std::string find_stmt{};

        std::string uuid_stmt{};

        if (id < 0) {
            auto iter = CCmap.begin();
            uuid_stmt += "( U.uuid = " + std::to_string(iter->first);
            for (auto iter = CCmap.begin(); iter != CCmap.end(); iter++) {
                uuid_stmt += " OR\n U.uuid = ";
                uuid_stmt += std::to_string(iter->first);
            }
            uuid_stmt += ") AND\n";
        } else {
            uuid_stmt = std::string("U.uuid = " + std::to_string(id));
            new_client = true;
        }
        auto get_find_stmt = [&](size_t skJc) {
            find_stmt = std::string(std::string("SELECT\n"
                                                "U.uuid AS uuid,\n"
                                                "P.kch AS `kch`,\n"
                                                "P.kxh AS `kxh`,\n"
                                                "K.kcm AS `kcm`,\n"
                                                "P.jsxm AS `jsxm`,\n"
                                                "P.jxdd AS `jdxx`,\n"
                                                "p.jsszxqh AS `jsszxqh`\n"
                                                "FROM\n"
                                                "uuid_jxdd AS U\n"
                                                "INNER JOIN pksj AS P ON U.jxdd = P.jxdd AND U.jsszxqh = P.jsszxqh\n"
                                                "INNER JOIN kcb AS K ON P.kch = K.kch\n"
                                                "WHERE\n")
                                    + uuid_stmt +
                                    "P.sksj <= " + std::to_string(skJc) + " AND\n"
                                                                          "P.sksj+P.cxjc\t >= " + std::to_string(skJc) +
                                    " AND\n" +
                                    "P.skxq = " + std::to_string(day) + " AND\n"
                                    + "SUBSTR(qsz," +
                                    std::to_string(week) + ",1)=\'1\'");
        };

        auto update_map = [&](size_t jc, bool first, bool new_cli = false) {
            get_find_stmt(skjc);
            try {
                nanodbc::result find_row = nanodbc::execute(conn, find_stmt);
                for (; find_row.next();) {
                    int uid = find_row.get<int>(0);

                    if (new_cli) {
                        int xqh = find_row.get<int>(6);
                        std::string dd = find_row.get<std::string>(5);
                        ClassPlace cp;
                        cp.jxdd_ = dd;
                        cp.jsszxqh = xqh;
                        ClassMes ci;
                        ci.classPlace_ = cp;
                        CCmap.insert(std::make_pair(id, ci));
                    }

                    if (CCmap.find(uid) != CCmap.end()) {
                        Lesson les = Lesson(find_row.get<std::string>(1), find_row.get<int>(2),
                                            find_row.get<std::string>(3), find_row.get<std::string>(4),
                                            find_row.get<std::string>(5));
                        if (first)
                            CCmap.at(uid).lesson_1 = les;
                        else
                            CCmap.at(uid).lesson_2 = les;
                    } else {
                        continue;
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl; //更换为日志报错
            }
        };

//            更新当前节次
        update_map(skjc, true, new_client);
        if (two_lesson) {
//                更新下一节次
            update_map(skjc + 1, false, new_client);
        }

    }
*/
    void query_background(nanodbc::connection &conn, std::vector <BackGround> &backgrounds) {
        //查询所有
        std::string sel_stmt = "SELECT * FROM backgrounds";
        //执行语句
        nanodbc::result find_row = nanodbc::execute(conn, sel_stmt);

        for (; find_row.next();) {
            int id = find_row.get<int>(0);
            std::string code = find_row.get<std::string>(1);
            std::string des = find_row.get<std::string>(2);

            backgrounds.emplace_back(id, code, des);
        }

    }

    std::string find_pic(nanodbc::connection &conn, Cli::Uuid id) {
        nanodbc::statement statement(conn);

        nanodbc::prepare(statement, std::string("SELECT background FROM places WHERE UUID = ?"));

        statement.bind(0, &id);

        nanodbc::result row = nanodbc::execute(statement);

        while (row.next()) {
            return row.get<std::string>(0);
        }
        return "";
    }

    std::deque <Message> find_mes(nanodbc::connection &conn, Cli::Uuid id, const std::string &stamp) {
        //准备结果
        std::deque <Message> res;

        nanodbc::statement statement(conn);

        nanodbc::prepare(statement,
                         std::string("SELECT title, expiretime, text FROM msg WHERE uuid = ? AND starttime = ? ;"));

        statement.bind(0, &id);
        statement.bind(1, stamp.c_str());

        nanodbc::result row = nanodbc::execute(statement);

        while (row.next()) {
            Message mes;
            mes.title = row.get<std::string>(0);
            mes.expireTime = row.get<size_t>(1);
            mes.text = row.get<std::string>(2);
            res.push_back(mes);
        }

        return res;
    }

    Lesson find_lesson(nanodbc::connection &conn, Cli::Uuid id, int skjc, const scu_date &date) {
        //准备结果
        Lesson ret;

        std::string teacher_name;

        nanodbc::statement statement(conn);

        nanodbc::prepare(statement,
                         std::string("SELECT\n"
                                     "U.UUID AS uuid,\n"
                                     "P.kch AS kch,\n"
                                     "P.kxh AS kxh,\n"
                                     "K.kcm AS kcm,\n"
                                     "P.jsxm AS jsxm,\n"
                                     "P.jxdd AS jxdd,\n"
                                     "P.jsszxqh AS jsszxqh\n"
                                     "FROM\n"
                                     "places AS U\n"
                                     "INNER JOIN pksj AS P ON U.place = P.jxdd \n"
                                     "INNER JOIN kcb AS K ON P.kch = K.kch\n"
                                     "WHERE U.UUID = ?\n"
                                     "AND \n"
                                     "P.sksj <= ? AND\n"
                                     "P.sksj + P.cxjc > ? AND\n"
                                     "P.skxq = ? AND\n"
                                     "SUBSTR(qsz,?,1)=1"));

        statement.bind(0, &id);
        statement.bind(1, &skjc);
        statement.bind(2, &date.day);
        statement.bind(3, &date.day);
        statement.bind(4, &date.week);


        nanodbc::result row = nanodbc::execute(statement);

        for (int i = 0; row.next(); i++) {

            if (i != 0) {
                teacher_name += ",";
            }
            ret.kch_ = row.get<std::string>(1);
            ret.kxh_ = row.get<int>(2);
            ret.kcm_ = row.get<std::string>(3);
            teacher_name += row.get<std::string>(4);
            ret.jxdd_ = row.get<std::string>(5);
        }
        ret.jsxm_ = teacher_name;

        return ret;
    }

    int find_campus(nanodbc::connection &conn, Cli::Uuid id) {

        int ret;

        nanodbc::statement statement(conn);

        nanodbc::prepare(statement, std::string("SELECT \n"
                                                "distinct P.jsszxqh AS campus \n"
                                                "FROM pksj AS P\n"
                                                "INNER JOIN places AS PL ON P.jxdd = PL.place\n"
                                                "WHERE PL.UUID = ?"));

        statement.bind(0, &id);

        nanodbc::result row = nanodbc::execute(statement);

        while (row.next()) {
            ret = row.get<int>(0);
        }

        return ret;
    }

    scu_date update_calendar(nanodbc::connection &conn, int year, int month, int date) {

        int week;
        int day;

        nanodbc::statement statement(conn);

        nanodbc::prepare(statement, std::string("SELECT \n"
                                                "now_week,\n"
                                                "now_day\n"
                                                "FROM\n"
                                                "calendar\n"
                                                "WHERE\n"
                                                "year = ? AND\n"
                                                "month = ? AND\n"
                                                "date = ?"));

        statement.bind(0, &year);
        statement.bind(1, &month);
        statement.bind(2, &date);

        nanodbc::result row = nanodbc::execute(statement);

        while (row.next()) {
            week = row.get<int>(0);
            day = row.get<int>(1);
        }

        return scu_date(week,day);
    }


}
