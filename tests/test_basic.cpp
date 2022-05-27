#include "test.h"

int main()
{
    json j0 = nullptr;
    json j1 = { {"pi", 3.14} };
    json j2 = "json test";
    json j3 = 233;
    json j4 = 1.414;
    json j5 = true;
    auto j6 = json::object({ "test", 233 });
    auto j7 = json::array({ 0, 1, 2, 3 });

    j7.push_back(4);
    j7.emplace_back(5);
    j7.push_back(j1);
    j7.push_back(json("back"));

    std::cout << color::F_BLUE << j6 << "\n" << color::CLEAR_F;
    std::cout << color::F_CYAN << j7 << "\n" << color::CLEAR_F;

    j7.pop_back();
    std::cout << color::F_CYAN << j7 << "\n" << color::CLEAR_F;

    
    json j8 = {
        { "c", 3.14 },
        { "obj", {
            { "key1", "val1" },
            { "key2", "val2" },
            }
        }
    };

    
    std::cout << j8 << "\n";
    std::cout << "j8.contains(\"obj\"): " << std::boolalpha << j8.contains("obj") << "\n";

    
    std::cout << color::F_GREEN << std::setw(4) << j6 << "\n" << color::CLEAR_F;
    std::cout << color::F_BLUE << j7.dump(0) << "\n" << color::CLEAR_F;

    return 0;
}