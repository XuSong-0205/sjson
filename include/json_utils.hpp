#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP


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





} // namespace sjson

#endif  // JSON_UTILS_HPP