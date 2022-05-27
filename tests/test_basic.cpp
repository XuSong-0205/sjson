#include "test.h"

int main()
{
    json j0 = nullptr;
    json j1 = {"pi", 3.14};
    json j2 = "json test";
    json j3 = 233;
    json j4 = 1.414;
    json j5 = true;

    JSON_ASSERT(j0.get<std::nullptr_t>() == nullptr);
    JSON_ASSERT(static_cast<double>(j4) == 1.414);
    JSON_ASSERT(j3.get<int>() == 233);


    auto j10 = json::object({ "test", 233 });
    auto j11 = json::array({ 0, 1, 2, 3 });

    std::cout << color::F_GREEN << std::setw(4) << j10 << "\n" << color::CLEAR_F;
    std::cout << color::F_BLUE << j11.dump(0) << "\n" << color::CLEAR_F;

    return 0;
}