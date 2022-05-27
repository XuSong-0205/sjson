#include "test.h"
#include <fstream>
#include <cstdio>


int main()
{    
    const char* str0 = "{\"测试1\": {\"测试2\": [123456789101112, 3.14159265358, true, \"ok\", \"可以\", \"😀\", null]}}";
    std::cout << color::F_CYAN << json::parse(str0) << color::CLEAR_F << "\n";

    std::string str1(str0);
    std::cout << color::F_PURPLE << json::parse(str0) << color::CLEAR_F << "\n";


    json obj;
    std::ifstream ifile(data_path() + "temp1.json");
    if (ifile)
    {
        ifile >> obj;
        std::cout << color::F_RED << obj << color::CLEAR_F << "\n";
    }

    std::FILE* file = std::fopen((data_path() + "temp1.json").c_str(), "r");
    if (file != nullptr)
    {
        std::cout << color::F_GREEN << json::parse(file) << color::CLEAR_F << "\n";
    }


    return 0;
}