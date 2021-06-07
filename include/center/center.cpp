#include "center.h"

void center::start()
{
    //假设udp namespace 下的 recv和send已经正常
    //任务队列最大数量
    pool_.setMaxQueueSize(30);
    pool_.start(_permanent_thread_num_ + 5);    //这里默认四核八线程, 也可以用std::hard_ware_concurrency()确定

    //提交定时读取配置文件的任务,监听cli登录, mir心跳的任务
    this->pool_.run( [this](){  read_config();  });
    this->pool_.run( [this](){  wait_cli_login();   });
    this->pool_.run( [this](){  listen_mir_beat();  });
    //this->pool.run( [this](){ clear_mirs_data();  });

    //可能需要设置一段时间内进行日志输出的文件和文件路径
    
}

void center::read_config()
{
    //或者放到center中作为一个变量
    static std::string config_file_path;
    //或者
    //static std::fstream config_file;

    //可以使用如下形式的定义
    //static auto read_data_infile/copy/swap = [=](...){ ... };

    while(true)
    {
        Setting set_in_file{};
    
        //读取配置文件
        //read_data_in(config_file_path, set_in_file);

        //复制到配置副本中
        //copy(set_in_file, *setting_copy);

        //交换主副配置指针
        //swap(setting_copy, setting);  //atomic operation
    
        //sleep(set_in_file.next_read_intrerval);
    }
}

void center::wait_cli_login()
{
    //用于接收数据的缓冲区
    //char* buffer[xxx_len];
    //flush(buffer)
    //current_available_mir
    
    while(true)
    {
        //udp::recv(port, buffer ...);

        //按照协议格式进行解析, 并且得到ip等其他信息
        //auto [cli_id, login_count, state] = msg[...]
        
        //登录或者重新连接
        // if(!cookie_.contains(cli_id))
        // {
        //     //重新连接
        //     if(all_cli_.contains(cli_id))
        //     {
        //         correct_info(mirs_data_, cli_id);
        //     }
        //     else  //首次链接
        //     {
        //         record_info(mirs_data, cli_id);
        //     }
        //     cookie_.cache_login_info(cli_id);
        //     dblog(cli_login_event, cli_id);
        // }
        // else
        // {
        //     //延迟到达不做处理
        //     continue;
        // }
        // flush(buffer);
    }
}

