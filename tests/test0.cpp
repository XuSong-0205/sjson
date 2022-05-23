#include <iostream>
#include "../include/json.hpp"
using json = sjson::json;

int main()
{
    json j = {
        {"test", 233},
        {"hello", "world"},
        {"obj", {
            "000", 123
            }
        }
    };

    auto j1 = json::object({"test", 233});


    return 0;
}