#include <mirror.hpp>


void Mirror::start() {
    //任务队列最大数量
    pool_.setMaxQueueSize(30);
    pool_.start(_permanent_thread_num_ + 5;)


    //提交任务

    this->pool_.run([this]() { read_config(); });
    //读取配置文件
    this->pool_.run([this]() { update_data_info(); });
    //初始化数据库
    this->pool_.run([this]() { update_data_info(); });
    //更新数据缓存
    this->pool_.run([this]() { listen_cli_beat(); });
    //建立cli链接

    //输出日志
}

void Mirror::read_config() {
//json格式
    static std::string config_file_path{"CenterConfig.txt"};
    static std::ifstream in{config_file_path, std::ios::in};
    Setting set{};
    memset(&set, 0, sizeof(Setting));
    uint next_read_interval{0};

    auto read_data = [&]() {
        if (!in.is_open())
            in.open(config_file_path, std::ios::in);
        std::string line{};
        while (in.good()) {
            std::getline(in, line, '\n');
            if (line.size() < 2 || line.front() == '/')
                continue;
            else if (line.find("next_read") != std::string::npos)
                next_read_interval = std::stoi(std::string(line.find_first_of('[') + 1, line.find_first_of(']')));
            else if (line.find("mir_heart") != std::string::npos)
                set.mir_load_record_interval_ = std::stoi(
                        std::string(line.find_first_of('[') + 1, line.find_first_of(']')));
            else if (line.find("mir_load") != std::string::npos)
                set.mir_dblog_interval_ = std::stoi(std::string(line.find_first_of('[') + 1, line.find_first_of(']')));
            else if (line.find("cli_class") != std::string::npos)
                set.mir_max_disbeat_time_ = std::stoi(
                        std::string(line.find_first_of('[') + 1, line.find_first_of(']')));
        }
    };

    auto swap = [](Setting *src, Setting *dst) {
        Setting *tmp = dst;
        dst = src;
        src = tmp;
    };

    while (true) {
        //读取配置文件
        read_data();

        //复制到配置副本中
        memcpy(telemeter::setting_copy, &set, sizeof(Setting));

        //交换主副配置指针
        swap(telemeter::setting_copy, telemeter::setting);  //atomic operation

        //挂起该进程
        sleep(next_read_interval * 1000);
    }
}

void Mirror::update_data_info() {
//    发送ip地址
    IP center_ip{0};
    IP this_ip{0};
//    定义计算服务器负载情况的函数
    auto cal_ser_load = [&]() -> uint8_t {
        return 0;
    };

//    定义计算内存负载情况的函数
    auto cal_mem_load = [&]() -> uint8_t {
        return 0;
    };

//    定义计算网络负载情况的函数
    auto cal_net_load = [&]() -> uint8_t {
        return 0;
    };

    auto get_packet = [&](IP, DetailLoadState){

    };

    DetailLoadState dtlLoadState{};
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
//    设置发送端口
    servaddr.sin_port = htons(myport);
//    设置发送ip
    servaddr.sin_addr.s_addr = inet_addr(serverip);

//    发送缓存
    char* packet = "\0";

//    更新间隔
    size_t time = 60;
//    发送的息
    char send_buf[8] = {0};

//线程主循环
    while (true) {

        if (time == mir_load_report_interval) {
        this_ip;
        //calculate ServerLoad
        uint8_t serverLoad = cal_ser_load();
        dtlLoadState.serve_load = serverLoad;
        //calculate MemoryLoad
        uint8_t memLoad = cal_mem_load();
        dtlLoadState.mem_load = memLoad;
        //calculate NetworkLoad
        uint8_t netLoad = cal_net_load();
        dtlLoadState.net_load_ = netLoad;

        //to_string(8byte) = 4+1+1+1+1 true/false
            get_packet(this_ip, dtlLoadState);
        time = 1;
    }

        if(time == 1){
            get_packet(this_ip, dtlLoadState);
        }
//        发送‘心跳包’
        sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

        //        重置缓冲
        memset(sendbuf, 0, sizeof(sendbuf));
//
        sleep(setting.mir_heart_beat_interval * 1000)；

        time++;
    }
}

//多并发
void Mirror::listen_cli_beat() {
    //set server.onConnect 连接函数

    //set server.onMessage 接收消息函数(回复消息)

    //set server

    while (true) {
        server_;
        //接收消息

        //获取time(为null则加入cli_tree)
//
//        if(第一次Content) {
//            返回第一次Content
//        }else{
//            分析时间戳
//            if(版本相同){
//                返回不需要更新
//            }else{  //版本不同
//                获取Content
//                返回Content
//            }
//        }



    }
}

//监听center发送的udp包
void Mirror::listen_mes() {
// udp
    while (true) {
        //server.start
    }
}

//清除mirror->client树信息
void Mirror::clear_cli_data() {
    while (true) {
        //sleep()24h
    }
}

void Mirror::update_clis_data() {
//    db_config 初始化
    const std::string dsn = "";
    const std::string name = "";
    const std::string passwd = "";
    nanodbc::connection conn = nanodbc::connection(dsn, name, passwd);

    while (true) {
        for(size_t i = 1; i <= 4; i++) {
            db_control::update_lesson_info(conn, this->cli_lesson_, i);
            sleep(class_interval * 60 * 1000)
        }

        time_sleep_until();//直到每日13：00

        for(size_t i = 5; i <= 9; i++){
            db_control::update_lesson_info(conn, this->cli_lesson_, i);
            sleep(class_interval * 60 * 1000)
        }

        time_sleep_until();//直到每日19：00

        for(size_t i = 10; i <= 12; i++){
            db_control::update_lesson_info(conn, this->cli_lesson_, i);
            sleep(class_interval * 60 * 1000)
        }

        time_sleep_until(); //直到每日7:30
    }
}

