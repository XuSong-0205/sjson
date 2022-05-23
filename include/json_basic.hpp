#ifndef JSON_BASIC_HPP
#define JSON_BASIC_HPP

#include <iostream>
#include <algorithm>
#include <initializer_list>
#include "json_utils.hpp"
#include "json_value.hpp"
#include "json_serializer.hpp"
#include "json_exception.hpp"

namespace sjson
{

BASIC_JSON_TEMPLATE_DECLARATION
class basic_json
{
public:
    using value_type        = basic_json;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using char_type         = typename StringType::value_type;
    using initializer_list  = std::initializer_list<basic_json>;


    /// object type
    using object_t  = ObjectType<StringType, basic_json>;

    /// array type
    using array_t   = ArrayType<basic_json>;

    /// string type
    using string_t  = StringType;

    /// int type
    using number_integer_t = NumberIntegerType;

    /// float type
    using number_float_t = NumberFloatType;

    /// bool type
    using boolean_t = BooleanType;


public:
    basic_json() = default;

    basic_json(const basic_json& other): m_value(other.m_value) { }

    basic_json(basic_json&& other)noexcept: m_value(std::move(other.m_value)) { }

    basic_json& operator=(const basic_json& other)
    {
        if (this != &other)
        {
            m_value = other.m_value;
        }

        return *this;
    }

    basic_json& operator=(basic_json&& other)noexcept
    {
        if (this != &other)
        {
            m_value = std::move(other.m_value);
        }

        return *this;
    }

    ~basic_json() = default;


    basic_json(const value_t value_type): m_value(value_type) { }

    basic_json(std::nullptr_t): m_value(nullptr) { }

    basic_json(const object_t& obj): m_value(obj) { }

    basic_json(object_t&& obj): m_value(std::move(obj)) { }

    basic_json(const array_t& arr): m_value(arr) { }

    basic_json(array_t&& arr): m_value(std::move(arr)) { }

    basic_json(const string_t& str): m_value(str) { }

    basic_json(string_t&& str): m_value(std::move(str)) { }

    basic_json(const char_type* str): m_value(str) { }

    basic_json(const number_integer_t num): m_value(num) { }

    basic_json(const number_float_t num): m_value(num) { }

    basic_json(const boolean_t val): m_value(val) { }


    template<typename Integer, 
            typename std::enable_if<std::is_integral<Integer>::value, int>::type = 0>
    basic_json(const Integer num): m_value(static_cast<number_integer_t>(num)) { }


    template<typename Floating,
            typename std::enable_if<std::is_floating_point<Floating>::value, int>::type = 0>
    basic_json(const Floating num): m_value(static_cast<number_float_t>(num)) { }


    basic_json(const initializer_list& init_list)
    {
        const bool is_an_object = std::all_of(init_list.begin(), init_list.end(), [](const basic_json& json){
            return (json.is_array() && json.size() == 2 && json[0].is_string());
        });

        if (is_an_object)
        {
            m_value = json_value<basic_json>(value_t::object);
            for (const basic_json& json : init_list)
            {
                m_value.m_data.object->emplace(*(json[0].m_value.m_data.string), json[1]);
            }
        }
        else
        {
            m_value = json_value<basic_json>(value_t::array);
            m_value.m_data.array->reserve(init_list.size());
            m_value.m_data.array->assign(init_list.begin(), init_list.end());
        }
    }


public:
    static basic_json object(const initializer_list& init_list)
    {
        if (init_list.size() != 2 || !(init_list.begin()->is_string()))
        {
            throw json_invalid_key("can't create object from initializer_list");
        }

        basic_json obj(value_t::object);
        obj.m_value.m_data.object->emplace(*(init_list.begin()->m_value.m_data.string), *(init_list.begin() + 1));
        return obj;
    }

    static basic_json array(const initializer_list& init_list)
    {
        basic_json arr(value_t::array);
        if (init_list.size())
        {
            arr.m_value.m_data.array->reserve(init_list.size());
            arr.m_value.m_data.array->assign(init_list.begin(), init_list.end());
        }
        return arr;
    }


public:
    basic_json& operator[](size_type index)
    {
        if (is_null())
        {
            m_value = json_value(value_t::array);
        }

        if (!is_array())
        {
            throw json_invalid_key("operator[] called on a non-array object");
        }

        const auto array = m_value.m_data.array;
        if (index >= array->size())
        {
            array->insert(array->end(), index - array->size() + 1, basic_json());
        }

        return (*array)[index];
    }

    const basic_json& operator[](size_type index)const
    {
        if (!is_array())
        {
            throw json_invalid_key("json operator[] called on a non-array object");
        }

        const auto array = m_value.m_data.array;
        if (index >= array->size())
        {
            throw std::out_of_range("json operator[] index out of range");
        }

        return (*array)[index];
    }


public:

    void swap(basic_json& other)noexcept    
    { 
        m_value.swap(other.m_value);  
    }

    value_t type()const noexcept
    {
        return m_value.m_type;
    }

    string_t type_name()const noexcept
    {
        switch (type())
        {
            case value_t::null:
                return string_t("null");

            case value_t::object:
                return string_t("object");

            case value_t::array:
                return string_t("array");

            case value_t::string:
                return string_t("string");

            case value_t::number_integer:
            case value_t::number_float:
                return string_t("number");

            case value_t::boolean:
                return string_t("boolean");

            default:
                return string_t("unknown");
        }
    }

    size_type size()const noexcept
    {
        switch (type())
        {
            case value_t::null:
                return 0;

            case value_t::object:
                return m_value.m_data.object->size();

            case value_t::array:
                return m_value.m_data.array->size();

            default:
                return 1;
        }
    }

    bool empty()const noexcept
    {
        return size() == 0;
    }




public:
    bool is_null()const noexcept    { return m_value.m_type == value_t::null;           }

    bool is_object()const noexcept  { return m_value.m_type == value_t::object;         }

    bool is_array()const noexcept   { return m_value.m_type == value_t::array;          }
    
    bool is_string()const noexcept  { return m_value.m_type == value_t::string;         }

    bool is_integer()const noexcept { return m_value.m_type == value_t::number_integer; }

    bool is_float()const noexcept   { return m_value.m_type == value_t::number_float;   }

    bool is_number()const noexcept  { return is_integer() || is_float();                }

    bool is_bool()const noexcept    { return m_value.m_type == value_t::boolean;        }



private:
    json_value<basic_json>  m_value;

};









} // namespace sjson





#endif  // JSON_BASIC_HPP