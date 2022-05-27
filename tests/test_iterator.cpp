#include "test.h"

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

    for (const auto& val : obj)
    {
        std::cout << val << " ";
    }
    std::cout << "\n";
    
    for (auto it = obj.cbegin(), end = obj.cend(); it != end; ++it)
    {
        std::cout << color::F_GREEN << it.key() << color::CLEAR_F << ": " 
                  << color::F_BLUE << it.value() << color::CLEAR_F << "\n";
    }
    std::cout << "\n";

    return 0;
}