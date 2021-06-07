#include <unistd.h>

#include <center_head>

int main()
{
    static const char* crush_log = "../crush.txt";

    center ctr;
    try
    {
        ctr.start();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << '\n';
        ctr.log_info(e, crush_log);
    }
}
