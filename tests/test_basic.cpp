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
    j7.push_back(5);
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

    j8["arr"] = json::array({ 0, 2, 4 });
    j8["arr"].push_back(j8["arr"]);
    j8["arr"].push_back({ "pi", 3.14 });
    auto iter = j8.at("obj").find("key2");
    j8["obj"].erase(iter);
    std::cout << color::F_PURPLE << j8 << color::CLEAR_F << "\n";

    
    j0.insert("j0", 0);
    j0.insert("j00", 1);

    std::cout << color::F_RED << j0 << "\n" << color::CLEAR_F;
    std::cout << color::F_GREEN << j6 << "\n" << color::CLEAR_F;
    std::cout << color::F_BLUE << j7 << "\n" << color::CLEAR_F;

    using sjson::operator""_json;
    auto j9 = "{\"j9\": \"test\", \"arr\": [1,3,5,7,9]}"_json;
    std::cout << color::F_YELLOW << j9 << "\n" << color::CLEAR_F;




    return 0;
}