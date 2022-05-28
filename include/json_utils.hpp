#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP

#include <type_traits>  // enable_if false_type true_type

namespace sjson
{
    
#define BASIC_JSON_TEMPLATE_DECLARATION                                 \
template<                                                               \
        template<typename, typename, typename...> class ObjectType,     \
        template<typename, typename...> class ArrayType,                \
        class StringType,                                               \
        class NumberIntegerType,                                        \
        class NumberFloatType,                                          \
        class BooleanType                                               \
        > 


#define BASIC_JSON_TEMPLATE_ARGS                        \
    ObjectType, ArrayType, StringType,                  \
    NumberIntegerType, NumberFloatType, BooleanType



template<typename...>
struct is_basic_json
    : std::false_type
{
    
};


BASIC_JSON_TEMPLATE_DECLARATION
struct is_basic_json<basic_json<BASIC_JSON_TEMPLATE_ARGS>>
    : std::true_type
{
    
};




} // namespace sjson

#endif  // JSON_UTILS_HPP