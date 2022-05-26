#include <iostream>
#include <cassert>
#include <iomanip>
#include <vector>
#include "utils\color.hpp"
#include "..\include\json.hpp"
using json = sjson::json;


#define ASSERT(arg) assert(arg)



int main()
{
    json obj = {
        {"test", 233},
        {"hello", "world"},
        {"is_ok", true},
        {"obj", {
            {"000", 123}
            }
        },
        {"array", {
            0, 1, 1, 2, 3, 5, 8
        }}
    };

    std::cout << color::F_GREEN << std::setw(4) << obj << "\n" << color::CLEAR_F;
    std::cout << color::F_BLUE << obj.dump(0) << "\n" << color::CLEAR_F;

    for (const auto& val : obj)
    {
        std::cout << val << " ";
    }
    std::cout << "\n";

    
    for (auto it = obj.cbegin(), end = obj.cend(); it != end; ++it)
    {
        std::cout << it.key() << ": " << it.value() << ", ";
    }


    ASSERT(obj.at("test").get<int>() == 233);
    ASSERT(obj["obj"].as_object().at("000").get<int>() == 123);



    json j0 = nullptr;
    json j1 = {"pi", 3.14};
    json j2 = "json test";
    json j3 = 233;
    json j4 = 1.414;
    json j5 = true;

    ASSERT(j0.get<std::nullptr_t>() == nullptr);
    ASSERT(static_cast<double>(j4) == 1.414);
    ASSERT(j3.get<int>() == 233);


    auto j10 = json::object({"test", 233});
    auto j11 = json::array({0, 1, 2, 3});


    std::cout << "\ntest end...\n";
    //std::cin.get();
    return 0;
}