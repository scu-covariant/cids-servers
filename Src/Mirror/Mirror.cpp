#include <mirror_head>

//如果最后一节课.置空字符串+置零
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

void Mirror::init() {


    auto judge_lesson_order = [this](const std::vector <scu_time> &time_table) -> size_t {
        scu_time now{scu_time::toScutime(muduo::Timestamp::now())};
        if (now < time_table[0])
            return 0;
        for (size_t i = 1; i < time_table.size(); i++) {
            if (now < time_table[i])
                return i;
        }
        return 12;
    };

    //读取配置文件
    static std::string config_file_path = this->config_file_path;

    static std::ifstream in{config_file_path, std::ios::in};

    if (!in.is_open())
        in.open(config_file_path, std::ios::in);

    std::string line{};

    while (in.good()) {
        std::getline(in, line, '\n');
        if (line.size() < 2 || line.front() == '/')
            continue;
        else if (line.find("mir_heart") != std::string::npos)
            telemeter::setting->mir_heart_beat_interval = std::stoi(
                    std::string(line.begin() + line.find_first_of('[') + 1, line.begin() +line.find_first_of(']')));
        else if (line.find("mir_load") != std::string::npos)
            telemeter::setting->mir_load_report_interval = std::stoi(
                    std::string(line.begin() +line.find_first_of('[') + 1,line.begin() + line.find_first_of(']')));
        else if (line.find("img_path") != std::string::npos)
            telemeter::setting->img_path = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
        else if (line.find("db_source") != std::string::npos)
            telemeter::setting->db_source = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
        else if (line.find("db_user") != std::string::npos)
            telemeter::setting->db_user = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
        else if (line.find("db_password") != std::string::npos)
            telemeter::setting->db_password = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
        else if (line.find("center_ip") != std::string::npos)
            telemeter::setting->center_ip = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
        else if (line.find("this_ip") != std::string::npos)
            telemeter::setting->this_ip = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
        else if (line.find("update_pic_interval") != std::string::npos)
            telemeter::setting->update_pic_interval = std::stoi(
                    std::string(line.begin() +line.find_first_of('[') + 1,line.begin() + line.find_first_of(']')));
        else if (line.find("utc_time") != std::string::npos)
            telemeter::setting->utc_time = std::stoi(std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']')));
        else if (line.find("update_calendar_time") != std::string::npos)
            telemeter::setting->update_calendar_time = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
        else if (line.find("clear_clients_time") != std::string::npos)
            telemeter::setting->clear_clients_time = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
        else if (line.find("course_interval") != std::string::npos)
            telemeter::setting->course_interval = std::stoi(
                    std::string(line.begin() +line.find_first_of('[') + 1,line.begin() + line.find_first_of(']')));
    }
    //初始化数据库
    this->conn = nanodbc::connection(telemeter::setting->db_source,telemeter::setting->db_user,telemeter::setting->db_password);

    //更新section
    this->wj_section = judge_lesson_order(this->wj_Time);
    this->ja_section = judge_lesson_order(this->ja_Time);

    //更新日期
    muduo::Timestamp now(muduo::Timestamp::now());
    std::string formattedTime = now.toFormattedString(false);
    //获取年
    int year = std::stoi(formattedTime.substr(0, 4));
    //获取月
    int month = std::stoi(formattedTime.substr(4, 2));
    //获取日
    int date = std::stoi(formattedTime.substr(6, 2));
    //更新数据
    scu_date scudate = db_control::update_calendar(this->conn, year, month, date);
    //更新至原子量
    this->day = scudate.day;
    this->week = scudate.week;

    //下载图片缓存



    std::vector <BackGround> backgrounds;
    db_control::query_background(conn, backgrounds);
    //数据

    //循环
    for (auto iter = backgrounds.begin(); iter != backgrounds.end(); iter++) {
        //解码  +  储存图片
        //路径
        std::string path = telemeter::setting->img_path + "/" + std::to_string(iter->PicId) + ".jpg";


        codec::downPic(path, iter->PicCode);
    }


};

void Mirror::start() {



    //初始化配置文件及各数据
    this->init();

    LOG_INFO << "init success";
    //任务队列最大数量
    pool_.setMaxQueueSize(30);
    //用std::hard_ware_concurrency()确定硬件支持的最大并发数量, 该数量 * 2为此程序运行时占用线程数
    pool_.start(static_cast<int>(2 * std::thread::hardware_concurrency()));
    auto thd_start = [&](const char *info) {
        std::clog << "thread start: " << info << std::endl;
    };

    //发送负载情况(Task2)
    this->pool_.run([this]() { update_data_info(); });
    thd_start("update data info");
//
//    //更新数据缓存(Task3)
//    this->pool_.run([this]() { update_clis_data(); });
//    thd_start("update clients data");

    //读取配置文件(Task4)
    this->pool_.run([this]() { read_config(); });
    thd_start("read config");


    this->pool_.run([this]() { clear_cli_data(); });
    thd_start("clear cli data");

    this->pool_.run([this]() { update_course_section(); });
    thd_start("update_course_section");

    this->pool_.run([this]() { update_school_calendar(); });
    thd_start("update school calendar");

    this->pool_.run([this]() { update_pictures_info(); });
    thd_start("update_pictures_info");

    //TCP相关(Task6)
    this->listen_cli_beat();
    thd_start("listen client beat");
    //输出日志
}


void Mirror::update_data_info() {

    //        发送‘心跳包’
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        std::cerr << "sock error" << std::endl;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
//    设置发送端口
    servaddr.sin_port = htons(port::kMIR_BEAT_);
//    设置发送ip
/*修改:若发送失败,则沿用之前的ip*/
    servaddr.sin_addr.s_addr = inet_addr((telemeter::setting->center_ip).c_str());

    //    更新间隔
    size_t time = 0;
    //    发送消息
    char send_buf[8] = {0};
//    定义计算服务器负载情况的函数
    auto cal_ser_load = [&]() -> uint8_t {
        return '0';
    };

//    定义计算内存负载情况的函数
    auto cal_mem_load = [&]() -> uint8_t {
        std::ifstream in;
        std::string filename = "";
        return '0';
    };

//    定义计算网络负载情况的函数
    auto cal_net_load = [&]() -> uint8_t {
        return '0';
    };

    auto get_packet = [&]() {

        IP this_ip(telemeter::setting->this_ip);

        //calculate ServerLoad
        unsigned char serverLoad = cal_ser_load();

        //        DtlLoadState.serve_load = serverLoad;
        //calculate MemoryLoad
        unsigned char memLoad = cal_mem_load();
//        DtlLoadState.mem_load = memLoad;
        //calculate NetworkLoad
        unsigned char netLoad = cal_net_load();
//        DtlLoadState.net_load_ = netLoad;

//this_ip
        std::vector<unsigned char> ret;
        ret.push_back(this_ip.seg0);
        ret.push_back(this_ip.seg1);
        ret.push_back(this_ip.seg2);
        ret.push_back(this_ip.seg3);
        ret.push_back(serverLoad);  //服务器负载
        ret.push_back(memLoad);  //内存负载
        ret.push_back(netLoad);   //网络负载
        ret.push_back('\0');
        return ret;
    };

    //重置缓冲
    memset(send_buf, 0, sizeof(send_buf));

    //获取负载情况
    std::vector<unsigned char> info = get_packet();

    //更新负载情况
    for (int i = 0; i < 7; i++)
        send_buf[i] = info[i];

    //最后位填充0
    send_buf[7] = '\0';

    //重置time
    time = 1;


//线程主循环
    while (true) {

        if (time == telemeter::setting->mir_load_report_interval) {
            //to_string(8byte) = 4+1+1+1+1 true/false
            //重置缓冲
            memset(send_buf, 0, sizeof(send_buf));
            //获取负载情况
            std::vector<unsigned char> info = get_packet();
            //更新负载情况
            for (int i = 0; i < 7; i++)
                send_buf[i] = info[i];
            //最后位填充0
            send_buf[7] = '\0';
            //重置time
            time = 1;

        }

//    发送缓存
        sendto(sock, send_buf, protocol::kMIR_BEAT_PACSIZE_, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
        // 统计发送心跳包次数
        time++;
        //心跳包频率
        sleep(telemeter::setting->mir_heart_beat_interval);
    }
}
//定时拉取数据库信息, 更新时间戳

//读取配置文件
void Mirror::read_config() {


//    后可以考虑使用json格式
    static std::string config_file_path = this->config_file_path;
    static std::ifstream in{config_file_path, std::ios::in};
    Setting set{};
    size_t next_read_interval{0};

    auto read_data = [&]() {
        if (!in.is_open())
            in.open(config_file_path, std::ios::in);
        std::string line{};
        while (in.good()) {
            std::getline(in, line, '\n');
            if (line.size() < 2 || line.front() == '/')
                continue;
            else if (line.find("next_read") != std::string::npos)
                next_read_interval = std::stoi(std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']')));
            else if (line.find("mir_heart") != std::string::npos)
                set.mir_heart_beat_interval = std::stoi(
                        std::string(line.begin() +line.find_first_of('[') + 1,line.begin() + line.find_first_of(']')));
            else if (line.find("mir_load") != std::string::npos)
                set.mir_load_report_interval = std::stoi(
                        std::string(line.begin() +line.find_first_of('[') + 1,line.begin() + line.find_first_of(']')));
            else if (line.find("img_path") != std::string::npos)
                set.img_path = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
            else if (line.find("db_source") != std::string::npos)
                set.db_source = std::string(line.begin() +line.find_first_of('[') + 1,line.begin() + line.find_first_of(']'));
            else if (line.find("db_user") != std::string::npos)
                set.db_user = std::string(line.begin() +line.find_first_of('[') + 1,line.begin() + line.find_first_of(']'));
            else if (line.find("db_password") != std::string::npos)
                set.db_password = std::string(line.begin() +line.find_first_of('[') + 1,line.begin() + line.find_first_of(']'));
            else if (line.find("center_ip") != std::string::npos)
                set.center_ip = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
            else if (line.find("this_ip") != std::string::npos)
                telemeter::setting->this_ip = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
            else if (line.find("update_pic_interval") != std::string::npos)
                set.update_pic_interval = std::stoi(std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']')));
            else if (line.find("utc_time: ") != std::string::npos)
                set.utc_time = std::stoi(std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']')));
            else if (line.find("update_calendar_time:") != std::string::npos)
                set.update_calendar_time = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
            else if (line.find("clear_clients_time:") != std::string::npos)
                set.clear_clients_time = std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']'));
            else if (line.find("course_interval:") != std::string::npos)
                set.course_interval = std::stoi(std::string(line.begin() +line.find_first_of('[') + 1, line.begin() +line.find_first_of(']')));

        }
    };


    auto swap = [](Setting *src, Setting *dst) {
        Setting *tmp = dst;
        dst = src;
        src = tmp;
    };

    //主任务
    while (true) {
        //读取配置文件
        read_data();

        //复制到配置副本中
        *telemeter::setting_copy = set;

        //交换主副配置指针
        swap(telemeter::setting_copy, telemeter::setting);  //atomic operation

        //挂起该进程
        sleep(next_read_interval);
    }
}

//清除mirror->client树信息
void Mirror::clear_cli_data() {

    auto get_seconds_from_now = [&](const scu_time &time) {
        scu_time nowTime = scu_time::toScutime(muduo::Timestamp::now());
        return time <= nowTime ? 0 : (time - nowTime).toSeconds();
    };

    while (true) {

        //加锁
        Class2Campus.clear();

        cli_info_.clear();

        //配置文件读取upload,time
        std::string clear_time = telemeter::setting->clear_clients_time;
        int index = clear_time.find_last_of(":");
        //获取小时
        int hour = std::stoi(clear_time.substr(0, index));
        //获取分钟
        int minute = std::stoi(clear_time.substr(index + 1));

        sleep(get_seconds_from_now(scu_time(hour, minute)));
        //每日24时更新
    }
}

//定时 更新课程节次信息,包括江安与望江
//更新course间隔
void Mirror::update_course_section() {

    auto judge_lesson_order = [this](const std::vector <scu_time> &time_table) -> size_t {
        scu_time now{scu_time::toScutime(muduo::Timestamp::now())};
        if (now < time_table[0])
            return 0;
        for (size_t i = 1; i < time_table.size(); i++) {
            if (now < time_table[i])
                return i;
        }
        return 12;
    };


    while (true) {
        wj_section = judge_lesson_order(this->wj_Time);
        ja_section = judge_lesson_order(this->ja_Time);

        sleep(telemeter::setting->course_interval);
    }
}

//定时 更新校历信息
void Mirror::update_school_calendar() {
    auto get_seconds_from_now = [&](const scu_time &time) {
        scu_time nowTime = scu_time::toScutime(muduo::Timestamp::now());
        return time <= nowTime ? 0 : (time - nowTime).toSeconds();
    };
    while (true) {
        //获取当前时间
        muduo::Timestamp now(muduo::Timestamp::now());
        std::string formattedTime = now.toFormattedString(false);
        //获取年
        int year = std::stoi(formattedTime.substr(0, 4));
        //获取月
        int month = std::stoi(formattedTime.substr(4, 2));
        //获取日
        int date = std::stoi(formattedTime.substr(6, 2));
        //更新数据
        scu_date scudate = db_control::update_calendar(this->conn, year, month, date);
        //更新至原子量
        day = scudate.day;
        week = scudate.week;
        //配置文件读取upload,time

        std::string update_time = telemeter::setting->update_calendar_time;
        int index = update_time.find_last_of(":");
        //获取小时
        int hour = std::stoi(update_time.substr(0, index));
        //获取分钟
        int minute = std::stoi(update_time.substr(index + 1));

        sleep(get_seconds_from_now(scu_time(hour, minute)));
        //每日24时更新
    }
}

//待修改配置文件
void Mirror::update_pictures_info() {

    scu_time nowtime = scu_time::toScutime(muduo::Timestamp::now());
    //从6/12/18点开始更新
    if (nowtime < scu_time(6, 0))
        sleep((scu_time(6, 0) - nowtime).toSeconds());
    else if (nowtime < scu_time(12, 0))
        sleep((scu_time(12, 0) - nowtime).toSeconds());
    else if (nowtime < scu_time(18, 0))
        sleep((scu_time(18, 0) - nowtime).toSeconds());
    else
        sleep((scu_time(24, 0) - nowtime).toSeconds());

    while (true) {
        //查询数据库

        std::vector <BackGround> backgrounds;

        db_control::query_background(conn, backgrounds);
        //图片信息与
        //数据
        //循环
        for (auto iter = backgrounds.begin(); iter != backgrounds.end(); iter++) {
            //解码  +  储存图片
            //路径
            std::string path = telemeter::setting->img_path + "/" + std::to_string(iter->PicId) + ".jpg";

            codec::downPic(path, iter->PicCode);
        }

        sleep(telemeter::setting->update_pic_interval * 60 * 60);
    }

}

//多并发
void Mirror::listen_cli_beat() {
    muduo::net::EventLoop loop_;
    muduo::net::TcpServer server_(&loop_, muduo::net::InetAddress(port::kCLI_BEAT_), "");
    //set server.onConnect 连接函数
    server_.setMessageCallback(std::bind(&Mirror::on_message, this, _1, _2, _3));
    server_.start();
    loop_.loop();
}

//更新参数, 如Scu_date与scu_time->skjc


//待修改配置时间
void Mirror::on_message(const muduo::net::TcpConnectionPtr &connectionPtr,
                        muduo::net::Buffer *msg,
                        muduo::Timestamp time) {
    std::string src = msg->retrieveAllAsString();
    Cli::Uuid id;
    std::string timeStamp;
    json_control::getIdAndStamp(src, id, timeStamp);
    OldHash old;
    //查找到旧数据的哈希值
    if (this->cli_info_.find(id) != this->cli_info_.end()) {
        old = this->cli_info_[id];
    } else {
        std::deque <size_t> msgs;
        msgs.push_back(0);
        OldHash temp_hush;
        temp_hush.url_ = 0;
        temp_hush.event_ = 0;
        temp_hush.next_event_ = 0;
        temp_hush.msgs_ = msgs;
        this->cli_info_.insert(std::pair<Cli::Uuid, OldHash>(id, temp_hush));
        old = this->cli_info_[id];
    }

    //去数据库中查询新数据
    std::string now_stamp = muduo::Timestamp::now().toString();
    //分析当前的时间戳
    unsigned long stamp = std::stoul(now_stamp);
    stamp -= telemeter::setting->utc_time * 60 * 60;//(UTC+8h)    //误差时间

    int course_section;
    if (Class2Campus.find(id) == Class2Campus.end()) {
        CliDes new_des;
        new_des.id = id;
        new_des.campus = db_control::find_campus(this->conn, id);
        new_des.stamp = now_stamp;
        Class2Campus.insert(std::pair<Cli::Uuid, CliDes>(id, new_des));
    }

    if (Class2Campus[id].campus == 3) { //江安校区
        course_section = this->ja_section;
    } else {
        course_section = this->wj_section;
    }

    bool next_event = true;
    if (course_section == 4 || course_section == 9 || course_section == 12)
        next_event = false;


    std::string pic_url = "http://" + telemeter::setting->this_ip + ":" + std::to_string(port::kMIR_HTTP_) + telemeter::setting->img_path + "/" + db_control::find_pic(this->conn, id) + ".jpg";

    std::deque <Message> msgs = db_control::find_mes(this->conn, id, std::to_string(stamp)); //统一utc时间

    Lesson lesson_1 = db_control::find_lesson(this->conn, id, course_section, scu_date(this->week, this->day));
    Lesson lesson_2;
    if (next_event) {
        lesson_2 = db_control::find_lesson(this->conn, id, course_section + 1, scu_date(this->week, this->day));
    }
    OldHash new_hash;
    new_hash.url_ = std::hash<std::string>()(pic_url);
    new_hash.event_ = std::hash<Lesson>()(lesson_1);
    new_hash.next_event_ = std::hash<Lesson>()(lesson_2);
    std::deque <size_t> msgs_hash;
    for (auto iter = msgs.begin(); iter != msgs.end(); iter++) {
        msgs_hash.push_back(std::hash<Message>()(*iter));
    }

    if (new_hash == old) {
        connectionPtr->send(json_control::no_need_update);

    }else {
        cli_info_[id] = new_hash;
        ClassMes sendInfo;
        sendInfo.image_url = pic_url;
        sendInfo.messages = msgs;
        sendInfo.lesson_1 = lesson_1;
        sendInfo.lesson_2 = lesson_2;
        std::string info = json_control::getJsonInfo(sendInfo, now_stamp);
        connectionPtr->send(info);
    }

}


//暂停该函数
/*
void Mirror::update_clis_data() {
    //cli-classinfo缓存
    std::unordered_map <Cli::Uuid, ClassMes> clis_data;

//    校园日历
    scu_date sD{1, 1};
    scu_date sD_1{1, 1};
    scu_date *scuDate = &sD;
    scu_date *scuDate_copy = &sD_1;

    scu_date scuDate_cache{};

    scu_time nowTime{0, 0};

    auto swap_scu_date_ptr = [&]() {
        scu_date *tmp = scuDate;
        scuDate_copy = scuDate;
        scuDate = tmp;
    };

//写当日date(其实不会出现冲突.但为以后消费者模型做准备工作)
    auto update_scu_date = [&]() {
//更新当日上课信息
        db_control::check_week_and_day(conn, scuDate_cache);
//复制至副本
        memcpy(scuDate_copy, &scuDate_cache, sizeof(scu_date));
//交换指针
        swap_scu_date_ptr();
    };

//    判断时间
    auto at_middle = [&](const scu_time &lTime, const scu_time &rTime) {
        return (lTime <= nowTime) && (nowTime < rTime);
    };

    auto get_seconds_from_now = [&](const scu_time &time) {
        nowTime = scu_time::toScutime(muduo::Timestamp::now());
        return time <= nowTime ? 0 : (time - nowTime).toSeconds();
    };

    auto query_scu_time = [&](int &xqh, int &jc) -> scu_time {
//        获取迭代器,遍历时间表

        auto iter1 = this->wj_Time.begin();
        auto iter2 = this->ja_Time.begin();
//        上课节次,用于返回
        int skjc = 0;

        nowTime = scu_time::toScutime(muduo::Timestamp::now());//获取当前时间,进行分析

        for (; iter1 != wj_Time.end(); iter1++, iter2++) {
            skjc++;
            jc = skjc;
            if (skjc <= 4) {
                if (nowTime <= *iter1) {
                    xqh = 1;
                    return scu_time(0, 15);
                } else {
                    if (nowTime <= *iter2) {
                        xqh = 3;
                        if (skjc == 4)
                            return scu_time(2, 40);
                        else
                            return scu_time(0, 40);
                    } else {
                        continue;
                    }
                }
            } else {
                if (nowTime <= *iter2) {
                    xqh = 3;
                    return scu_time(0, 10);
                } else {
                    if (nowTime <= *iter1) {
                        xqh = 1;
                        if (skjc == 9) //晚餐
                            return scu_time(1, 30);
                        else if (skjc == 7)
                            return scu_time(0, 55);
                        else if (skjc == 12)
                            return scu_time(0, 0);
                        else
                            return scu_time(0, 45);
                    } else {
                        continue;
                    }
                }
            }
        }
        return scu_time::toScutime(muduo::Timestamp::now());
    };


//    自动计算当前时间
    auto query_clis_data = [&, this]() {
        std::unordered_map <Cli::Uuid, ClassMes> clis_data_cache_;
        bool two_lessons = true;
        int xqh = 0;  //校区号
        int jc = 0;    //时间

        auto update_uC_map = [](const std::unordered_map <Cli::Uuid, ClassMes> &data_map,
                                std::unordered_map <Cli::Uuid, ClassMes> *origin_map,
                                std::unordered_map <Cli::Uuid, ClassMes> *copy_map) {
            //赋值
            *copy_map = data_map;
//            交换指针
            auto tmp = copy_map;
            copy_map = origin_map;
            origin_map = tmp;
        };


        //获取应当更新的校区号与节次
        scu_time wait_time = query_scu_time(xqh, jc);

        if (jc == 4 || jc == 9 || jc == 12)
            two_lessons = false;
        size_t wait_second = wait_time.toSeconds();
        if (xqh == 1) {
//       更新值
//       参数: 连接参数, 这节课节次,查询两节课信息, 要查询的uuid集合
            clis_data_cache_ = *(this->wj_clis_data_);
//         参数: 连接参数, 这节课节次,查询两节课信息, 要查询的uuid集合
            db_control::query_lessons(conn, jc, two_lessons, clis_data_cache_, scuDate);
//            获取当前时间戳
            muduo::Timestamp nTime = muduo::Timestamp::now();
//            更新时间戳
            update_timestamp(nTime, wj_timestamp_, wj_timestamp_copy);
            //       交换指针

            update_uC_map(clis_data_cache_, this->wj_clis_data_, this->wj_clis_data_copy);
        } else if (xqh == 3) {
//         更新值
            clis_data_cache_ = *ja_clis_data_;
//         参数: 连接参数, 这节课节次,查询两节课信息, 要查询的uuid集合
            db_control::query_lessons(conn, jc, two_lessons, clis_data_cache_, scuDate);
//       获取当前时间戳
            muduo::Timestamp nTime = muduo::Timestamp::now();
//        更新时间戳
            update_timestamp(nTime, ja_timestamp_, ja_timestamp_copy);
//       交换指针
            update_uC_map(clis_data_cache_, this->ja_clis_data_, this->ja_clis_data_copy);
        }

        return wait_second;
    };

//    mirror开机时,初始化当时时间
    update_scu_date();
    muduo::Timestamp nTime = muduo::Timestamp::now();
    nowTime = scu_time::toScutime(nTime);


//    进入任务线程阶段
    while (true) {
//        挂起. 7.55唤醒
        sleep(get_seconds_from_now(scu_time(7, 55)));

        while (true) {
            nowTime = scu_time::toScutime(muduo::Timestamp::now());//记录当前时间
            size_t sleepSeconds = 0;
            sleepSeconds = query_clis_data();   //更新数据,在函数体内自动计算时间

            if (sleepSeconds == 0)
                break;
            //休眠至下一次更新
            sleep(sleepSeconds);
        }
        sleep(get_seconds_from_now(scu_time(24, 0)) ); //沉睡至24点
//        24点更新当日上课信息
        update_scu_date();
    }


}
*/


/*
void Mirror::update_timestamp(const muduo::Timestamp &dataStamp, muduo::Timestamp *origin_campus_stamp,
                              muduo::Timestamp *copy_campus_stamp) {

    //    赋值
    *copy_campus_stamp = dataStamp;

//    交换指针
    auto swap = [](muduo::Timestamp *origin, muduo::Timestamp *copy) {
        muduo::Timestamp *tmp = origin;
        origin = copy;
        copy = tmp;
    };
    swap(origin_campus_stamp, copy_campus_stamp);
}
*/