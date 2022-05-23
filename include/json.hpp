#ifndef JSON_HPP
#define JSON_HPP

#include <map>      // map
#include <vector>   // vector
#include <string>   // string
#include <cstdint>  // int64_t
#include <cstddef>  // nullptr_t, ptrdiff_t, size_t
#include "json_basic.hpp"


namespace sjson
{

template<
        template<typename K, typename V, typename... Args> class ObjectType = std::map,
        template<typename T, typename... Args> class ArrayType = std::vector,
        class StringType = std::string,
        class IntegerType = std::int64_t,
        class FloatType = double,
        class BooleanType = bool,
        template<typename T, typename SFINAE = void> class JsonSerializer = json_serializer
        >
class basic_json;



using json = basic_json<>;




} // namespace sjson

#endif // JSON_HPP