#include <iostream>
#include <cassert>
#include <iomanip>
#include "../include/json.hpp"
using json = sjson::json;

#define ASSERT(arg) assert(arg)

int main()
{
    json obj = {
        {"test", 233},
        {"hello", "world"},
        {"obj", {
            {"000", 123}
            }
        }
    };

    std::cout << std::setw(4) << obj << "\n";


    using ObjectType = std::map<std::string, json>;
    using KEY_TYPE = decltype("test");
    constexpr auto is_cont = std::is_constructible<typename ObjectType::key_type, KEY_TYPE>::value;
    auto v_test = obj["test"];
    ASSERT(v_test.get<int>() == 233);


    auto v_obj = obj["obj"];
    auto v_000 = v_obj["000"];
    ASSERT(v_000.get<int>() == 123);

    json j0 = nullptr;
    json j1 = {"pi", 3.14};
    json j2 = "json test";
    json j3 = 233;
    json j4 = 1.414;
    json j5 = true;


    double v40 = j4;
    auto v4 = static_cast<double>(j4);
    ASSERT(v4 == 1.414);

    auto v3 = j3.get<int>();
    ASSERT(v3 == 233);


    auto j10 = json::object({"test", 233});
    auto j11 = json::array({0, 1, 2, 3});

    std::cout << "test end...\n";
    std::cin.get();
    return 0;
}