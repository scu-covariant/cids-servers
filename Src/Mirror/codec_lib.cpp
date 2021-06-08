#include <cppcodec/base64_rfc4648.hpp>
#include <fstream>
#include <vector>

namespace codec {
    using base64 = cppcodec::base64_rfc4648;

    void downPic( const std::string &path,const std::string &code) {
        //打开文件
        std::ofstream ofs(path, std::ios::trunc | std::ios::binary);
        //异常处理
        if (!ofs) {
            ofs.open(path, std::ios::trunc | std::ios::binary);
        }
        //解码
        std::vector<uint8_t> buff = base64::decode(code);
        //写入文件
        ofs.write(reinterpret_cast<const char *>(buff.data()), buff.size());
        //关闭文件
        ofs.close();
    }

}