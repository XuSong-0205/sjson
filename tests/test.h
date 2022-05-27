#ifndef TEST_H
#define TEST_H


#include <iostream>
#include <cassert>
#include <iomanip>
#include <string>
#include "utils\color.hpp"
#include "..\include\json.hpp"
using json = sjson::json;


#define JSON_ASSERT(x)   assert(x)



const std::string& data_path()
{
    static std::string data_path;

    if (data_path.empty())
    {
        data_path = __FILE__;
        
        auto pos = data_path.rfind('\\');
        if (pos == std::string::npos)
        {
            pos = data_path.rfind('/');
        }
        
        if (pos == std::string::npos)
        {
            data_path = ".\\data\\";
        }
        else
        {
            data_path = data_path.substr(0, pos);
            data_path += "\\data\\";
        }
    }

    return data_path;
}


#endif // TEST_H