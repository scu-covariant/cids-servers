#include <unistd.h>

#include "Mirror.cpp"

int main()
{
    static const char* crush_log = "../crush.txt";

    Mirror mir;
    try
    {
        mir.start();
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
