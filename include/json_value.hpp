#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <cstdint>  // uint8_t
#include <cstddef>  // nullptr_t
#include <utility>  // forward
#include "json_exception.hpp"


namespace sjson
{


enum class value_t : std::uint8_t
{
    null,
    object,
    array,
    string,
    number_integer,
    number_float,
    boolean
};



template<typename BasicJsonType>
class json_value
{
private:
    friend BasicJsonType;

public:
    using char_type         = typename BasicJsonType::char_type;
    using object_t          = typename BasicJsonType::object_t;
    using array_t           = typename BasicJsonType::array_t;
    using string_t          = typename BasicJsonType::string_t;;
    using number_integer_t  = typename BasicJsonType::number_integer_t;
    using number_float_t    = typename BasicJsonType::number_float_t;
    using boolean_t         = typename BasicJsonType::boolean_t;;

public:
    json_value()
    {
        m_type = value_t::null;
        m_data.object = nullptr;
    }

    json_value(std::nullptr_t)
    {
        m_type = value_t::null;
        m_data.object = nullptr;
    }

    json_value(const object_t& obj)
    {
        m_type = value_t::object;
        m_data.object = create<object_t>(obj);
    }

    json_value(object_t&& obj)
    {
        m_type = value_t::object;
        m_data.object = create<object_t>(std::move(obj));
    }

    json_value(const array_t& arr)
    {
        m_type = value_t::array;
        m_data.array = create<array_t>(arr);
    }

    json_value(array_t&& arr)
    {
        m_type = value_t::array;
        m_data.array = create<array_t>(std::move(arr));
    }

    json_value(const string_t& str)
    {
        m_type = value_t::string;
        m_data.string = create<string_t>(str);
    }

    json_value(string_t&& str)
    {
        m_type = value_t::string;
        m_data.string = create<string_t>(std::move(str));
    }

    json_value(const char_type* str)
    {
        m_type = value_t::string;
        m_data.string = create<string_t>(str);
    }

    json_value(const number_integer_t num)
    {
        m_type = value_t::number_integer;
        m_data.number_integer = num;
    }

    json_value(const number_float_t num)
    {
        m_type = value_t::number_float;
        m_data.number_float = num;
    }

    json_value(const boolean_t val)
    {
        m_type = value_t::boolean;
        m_data.boolean = val;
    }

    json_value(const value_t value_type)
    {
        m_type = value_type;
        switch (value_type)
        {
            case value_t::null:
                m_data.object = nullptr;
                break;

            case value_t::object:
                m_data.object = create<object_t>();
                break;

            case value_t::array:
                m_data.array = create<array_t>();
                break;

            case value_t::string:
                m_data.string = create<string_t>();
                break;

            case value_t::number_integer:
                m_data.number_integer = number_integer_t(0);
                break;

            case value_t::number_float:
                m_data.number_float = number_float_t(0.0);
                break;

            case value_t::boolean:
                m_data.boolean = boolean_t(false);
                break;
        }
    }


    json_value(const json_value& other)
    {
        m_type = other.m_type;
        switch (other.m_type)
        {
            case value_t::null:
                m_data.object = nullptr;
                break;

            case value_t::object:
                m_data.object = create<object_t>(*other.m_data.object);
                break;

            case value_t::array:
                m_data.array = create<array_t>(*other.m_data.array);
                break;

            case value_t::string:
                m_data.string = create<string_t>(*other.m_data.string);
                break;

            case value_t::number_integer:
                m_data.number_integer = other.m_data.number_integer;
                break;

            case value_t::number_float:
                m_data.number_float = other.m_data.number_float;
                break;

            case value_t::boolean:
                m_data.boolean = other.m_data.boolean;
                break;
        }
    }
    
    json_value(json_value&& other)noexcept
    {
        m_type = other.m_type;
        m_data = other.m_data;
        other.m_type = value_t::null;
        other.m_data.object = nullptr;
    }
    
    json_value& operator=(const json_value& other)
    {
        if (this != &other)
        {
            json_value(other).swap(*this);
        }

        return *this;
    }

    json_value& operator=(json_value&& other)noexcept
    {
        if (this != &other)
        {
            clear();
            other.swap(*this);
        }

        return *this;
    }

    ~json_value()
    {
        clear();
    }



    void swap(json_value& other)noexcept
    {
        std::swap(m_type, other.m_type);
        std::swap(m_data, other.m_data);
    }


    void clear()
    {
        switch (m_type)
        {
            case value_t::object:
            case value_t::array:
            case value_t::string:
                delete m_data.object;
                m_data.object = nullptr;
                m_type = value_t::null;
                break;
            
            default:
                m_data.object = nullptr;
                m_type = value_t::null;
                break;
        }
    }


    template<typename Ty, typename... Args>
    Ty* create(Args&&... args)
    {
        return new Ty(std::forward<Args>(args)...);
    }


    bool operator==(const json_value& other)const noexcept
    {
        if (m_type == other.m_type)
        {
            switch (m_type)
            {
                case value_t::null:
                    return true;
                
                case value_t::object:
                    return *m_data.object == *other.m_data.object;

                case value_t::array:
                    return *m_data.array == *other.m_data.array;

                case value_t::string:
                    return *m_data.string == *other.m_data.string;

                case value_t::number_integer:
                    return m_data.number_integer == other.m_data.number_integer;

                case value_t::number_float:
                    return m_data.number_float == other.m_data.number_float;

                case value_t::boolean:
                    return m_data.boolean == other.m_data.boolean;

                default:
                    return false;
            }
        }
        else if ((m_type == value_t::number_integer) && (other.m_type == value_t::number_float))
        {
            return static_cast<number_float_t>(m_data.number_integer) == m_data.number_float;
        }
        else if ((m_type == value_t::number_float) && (other.m_type == value_t::number_integer))
        {
            return static_cast<number_float_t>(other.m_data.number_integer) == m_data.number_float;
        }

        return false;
    }


private:
    value_t   m_type;
    union
    {
        object_t*           object;
        array_t*            array;
        string_t*           string;
        number_integer_t    number_integer;
        number_float_t      number_float;
        boolean_t           boolean;
    }   m_data;

};


} // namespace sjson

#endif  // JSON_VALUE_HPP