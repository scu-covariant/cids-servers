#include <cppcodec/base64_rfc4648.hpp>
#include <fstream>
#include <vector>

namespace codec {
    using base64 = cppcodec::base64_rfc4648;

    void downPic(const std::string &code, const std::string &path) {
        //打开文件
        std::ofstream ofs(path, std::ios::trunc | std::ios::binary);
        //异常处理
        if (!ofs) {
            exit(-1);
        }
        //解码
        std::vector<uint8_t> buff = base64::decode(code);
        //写入文件
        ofs.write(reinterpret_cast<const char *>(buff.data()), buff.size());
        //关闭文件
        ofs.close();
    }

}