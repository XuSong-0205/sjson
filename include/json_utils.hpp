#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP

#include <type_traits>  // enable_if false_type true_type
#include <iostream>     // basic_ostream basic_istream

namespace sjson
{

namespace detail
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



template<class StringType = std::string>
using str_json = basic_json<std::map, std::vector, StringType, std::int64_t, double, bool>;




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



// 
// void_t
// 
template<typename... T>
struct make_void
{
    using type = void;
};

template<typename... T>
using void_t = typename make_void<T...>::type;



// 
// is_basic_json
//
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


} // namespace detail



// Example:
// struct Person
// {
// private:
//     friend sjson::json_bind<Person>;
// private:
//     std::string name_;
//     int age_;
// public:
//     Person(const std::string& name, int age)
//         : name_(name), age_(age)
//     {
//     }
// };
//
// namespace sjson
// {
// template<>
// struct json_bind<Person>
// {
//     void to_json(json& j, const Person& v)
//     {
//         j["name"] = v.name_;
//         j["age"] = v.age_;
//     }
//
//     void from_json(const json& j, Person& v)
//     {
//         v.name_ = j["name"].get<std::string>();
//         v.age_ = j["age"].get<int>();
//     }
// };
// }


// 
// json_bind
// 
template<
    typename Ty,
    typename BasicJsonType = detail::basic_json<>,
    typename std::enable_if<detail::is_basic_json<BasicJsonType>::value, int>::type = 0
>
struct json_bind
{
};




namespace detail
{

// 
// has_to_json
// 
template<typename, typename BasicJsonType = basic_json<>, typename = void>
struct has_to_json
    : std::false_type
{
};

template<typename Ty, typename BasicJsonType>
struct has_to_json<Ty, BasicJsonType, 
    void_t<decltype(json_bind<Ty, BasicJsonType>().to_json(std::declval<BasicJsonType&>(), std::declval<const Ty&>()))>>
    : std::true_type
{
};


// 
// has_from_json
// 
template<typename, typename BasicJsonType = basic_json<>, typename = void>
struct has_from_json
    : std::false_type
{
};

template<typename Ty, typename BasicJsonType>
struct has_from_json<Ty, BasicJsonType, 
    void_t<decltype(json_bind<Ty, BasicJsonType>().from_json(std::declval<const BasicJsonType&>(), std::declval<Ty&>()))>>
    : std::true_type
{
};


} // namespace detail



// 
// to_json
// 
template<typename Ty, typename BasicJsonType = detail::basic_json<>, 
    typename std::enable_if<detail::has_to_json<Ty, BasicJsonType>::value, int>::type = 0>
inline void to_json(BasicJsonType& json, const Ty& val)
{
    json_bind<Ty, BasicJsonType>().to_json(json, val);
}

// 
// from_json
// 
template<typename Ty, typename BasicJsonType = detail::basic_json<>,
    typename std::enable_if<detail::has_from_json<Ty, BasicJsonType>::value, int>::type = 0>
inline void from_json(const BasicJsonType& json, Ty& val)
{
    json_bind<Ty, BasicJsonType>().from_json(json, val);
}



// 
// json& << const Ty&
// 
template<typename Ty, typename BasicJsonType = detail::basic_json<>, 
    typename std::enable_if<detail::has_to_json<Ty, BasicJsonType>::value, int>::type = 0>
inline BasicJsonType& operator<<(BasicJsonType& json, const Ty& val)
{
    to_json(json, val);
    return json;
}

// 
// const json& >> Ty&
// 
template<typename Ty, typename BasicJsonType = detail::basic_json<>,
    typename std::enable_if<detail::has_from_json<Ty, BasicJsonType>::value, int>::type = 0>
inline const BasicJsonType& operator>>(const BasicJsonType& json, Ty& val)
{
    from_json(json, val);
    return json;
}



namespace detail 
{

// 
// read_json_wrapper
// 
template<typename Ty, typename BasicJsonType = basic_json<>,
    typename std::enable_if<has_to_json<Ty, BasicJsonType>::value, int>::type = 0>
struct read_json_wrapper
{
    using char_type = typename BasicJsonType::char_type;
    
    read_json_wrapper(const Ty& val)noexcept : read_value(val) { }

    friend std::basic_ostream<char_type>& operator<<(std::basic_ostream<char_type>& os, const read_json_wrapper& wrapper)
    {
        BasicJsonType json;
        to_json(json, wrapper.read_value);
        return os << json;
    }

private:
    const Ty& read_value;
};


// 
// write_json_wrapper
// 
template<typename Ty, typename BasicJsonType = basic_json<>,
    typename std::enable_if<has_to_json<Ty, BasicJsonType>::value && 
                            has_from_json<Ty, BasicJsonType>::value, int>::type = 0>
struct write_json_wrapper : public read_json_wrapper<Ty, BasicJsonType>
{
    using char_type = typename BasicJsonType::char_type;

    write_json_wrapper(Ty& val)noexcept : read_json_wrapper<Ty, BasicJsonType>(val), write_value(val) { }

    friend std::basic_istream<char_type>& operator>>(std::basic_istream<char_type>& os, write_json_wrapper&& wrapper)
    {
        BasicJsonType json;
        os >> json;
        from_json(json, wrapper.write_value);
        return os;
    }

private:
    Ty& write_value;
};


} // namespace detail


// 
// json_wrap
// 
template<typename Ty, typename BasicJsonType = detail::basic_json<>>
inline detail::read_json_wrapper<Ty, BasicJsonType> json_wrap(const Ty& val)
{
    return detail::read_json_wrapper<Ty, BasicJsonType>(val);
}

template<typename Ty, typename BasicJsonType = detail::basic_json<>>
inline detail::write_json_wrapper<Ty, BasicJsonType> json_wrap(Ty& val)
{
    return detail::write_json_wrapper<Ty, BasicJsonType>(val);
}



} // namespace sjson


#endif  // JSON_UTILS_HPP