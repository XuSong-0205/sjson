#include "test.h"

int main()
{
    json j0 = 0;
    json j1 = nullptr;
    json j2 = -0.1;
    json j3 = "中文测试";
    json j4 = json::array({ 0, 1, 1, 2, 3, 5, 8 });
    json j5 = json::object({ "obj", {{"hello", "sjson"}} });

    JSON_ASSERT(j0.get<int>() == 0);
    JSON_ASSERT(j1.get<decltype(nullptr)>() == nullptr);
    JSON_ASSERT(j2.get<double>() == -0.1);
    JSON_ASSERT(j3.get<std::string>() == "中文测试");
    JSON_ASSERT(j4.as_array().back().get<int>() == 8);
    JSON_ASSERT(j5.at("obj").at("hello").get<std::string>() == "sjson");

    JSON_ASSERT(j0 == 0);
    JSON_ASSERT(nullptr == j1);
    JSON_ASSERT(j2 == -0.1);
    JSON_ASSERT(j3 == "中文测试");
    JSON_ASSERT(j4.as_array().back() == 8);
    JSON_ASSERT(j5.at("obj").at("hello") == "sjson");

    JSON_ASSERT(static_cast<int>(j0) == 0);
    JSON_ASSERT(nullptr == (std::nullptr_t)j1);
    JSON_ASSERT(double(j2) == -0.1);
    JSON_ASSERT(static_cast<std::string>(j3) == "中文测试");
    

    std::cout << color::F_GREEN << j4 << "\n" << color::CLEAR_F;
    std::cout << color::F_BLUE << j5 << "\n" << color::CLEAR_F;

    return 0;
}