#include "test.h"
#include <fstream>
#include <chrono>
using namespace std::chrono;

const auto get_now = [](){
    return time_point_cast<microseconds>(steady_clock::now());
};


json create_json()
{
    return {
        {"test", 233},
        {"hello", "world"},
        {"is_ok", true},
        {"obj", {
            {"000", 123}
            }
        },
        {"array", {
            0, 1, 1, 2, 3, 5, 8
            }
        },
        {"测试", {
            123456789101112, 
            3.14159265358, 
            true, 
            "ok", 
            "可以", 
            "😀", 
            nullptr
            }
        }
    };
}


int main()
{
    auto t0 = get_now();
    constexpr int count = 1000;
    for (int i = 0; i < count; ++i)
    {
        auto temp = create_json();
    }
    auto t1 = get_now();
    std::cout << color::F_GREEN << "create json " << count 
              << " count, time speed " << (t1 - t0).count() << " us\n" << color::CLEAR_F;


    json obj = create_json();

    std::cout << color::F_GREEN << std::setw(4) << obj << "\n" << color::CLEAR_F;
    std::cout << color::F_BLUE << obj.dump(0) << "\n" << color::CLEAR_F;
    std::ofstream(data_path() + "temp0.json") << std::setw(4) << obj << "\n";

    return 0;
}