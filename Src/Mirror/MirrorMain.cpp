#include <unistd.h>

#include <mirror_head>

int main()
{
    static const char* crush_log = "../crush.txt";

    Mirror mir;
    try
    {
        mir.start();
    }
    catch(const std::exception& e)
    {
       LOG_WARN << "some error happens  " << e.what() << '\n';
    }
}
