#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <cstdint>  // uint8_t
#include <cstddef>  // size_t
#include "json_utils.hpp"

namespace sjson
{

enum class json_type : std::uint8_t
{
    null,
    object,
    array,
    string,
    boolean,
    number_integer,
    number_float,
};



    

    
}

#endif  // JSON_VALUE_H