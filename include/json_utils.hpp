#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP

#include <type_traits>  // enable_if false_type true_type

namespace sjson
{

template<
        template<typename K, typename V, typename... Args> class ObjectType = std::map,
        template<typename T, typename... Args> class ArrayType = std::vector,
        class StringType = std::string,
        class IntegerType = std::int64_t,
        class FloatType = double,
        class BooleanType = bool
        >
class basic_json;



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



/**
struct Person
{
    std::string name_;
    int age_;

    Person(const std::string& name, int age)
        : name_(name), age_(age)
    {
    }
};

namespace sjson
{
template<>
struct json_bind<Person>
{
    void to_json(json& j, const Person& v)
    {
        j["name"] = v.name_;
        j["age"] = v.age_;
    }

    void from_json(const json& j, Person& v)
    {
        v.name_ = j["name"].get<std::string>();
        v.age_ = j["age"].get<int>();
    }
};
}

*/

template<
    typename Ty,
    typename BasicJsonType = basic_json<>,
    typename = typename std::enable_if<is_basic_json<BasicJsonType>::value>::type
>
struct json_bind;



template<typename... T>
struct make_void
{
    using type = void;
};

template<typename... T>
using void_t = typename make_void<T...>::type;



template<typename, typename BasicJsonType = basic_json<>, typename = void>
struct has_to_json
    : std::false_type
{
    
};

template<typename Ty, typename BasicJsonType>
struct has_to_json<Ty, BasicJsonType, 
    void_t<decltype(json_bind<Ty, BasicJsonType>().to_json(std::declval<BasicJsonType&>(), std::declval<const Ty&>()))>
>
    : std::true_type
{
    
};



template<typename, typename BasicJsonType = basic_json<>, typename = void>
struct has_from_json
    : std::false_type
{
    
};

template<typename Ty, typename BasicJsonType>
struct has_from_json<Ty, BasicJsonType, 
    void_t<decltype(json_bind<Ty, BasicJsonType>().from_json(std::declval<const BasicJsonType&>(), std::declval<Ty&>()))
    >
>
    : std::true_type
{
    
};





} // namespace sjson

#endif  // JSON_UTILS_HPP