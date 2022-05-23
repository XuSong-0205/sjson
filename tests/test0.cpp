#include <iostream>
#include "../include/json.hpp"
using json = sjson::json;

int main()
{
    json obj = {
        {
            "user", {
                { "id", 10 },
                { "name", "sjson" },
                { "version", "0.1.0" }
            }
        }
    };

    auto j1 = json::object({"test", 233});


    return 0;
}