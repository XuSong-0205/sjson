#ifndef JSON_BASIC_HPP
#define JSON_BASIC_HPP

#include <iostream>
#include <algorithm>
#include <iterator>
#include <initializer_list>
#include "json_utils.hpp"
#include "json_value.hpp"
#include "json_parser.hpp"
#include "json_iterator.hpp"
#include "json_serializer.hpp"
#include "json_exception.hpp"

namespace sjson
{

BASIC_JSON_TEMPLATE_DECLARATION
class basic_json
{
public:
    friend class json_serializer<basic_json>;
    friend class iterator_impl<basic_json>;
    friend class iterator_impl<const basic_json>;
    friend class json_parser<basic_json>;

    
public:
    using value_type        = basic_json;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using char_type         = typename StringType::value_type;
    using initializer_list  = std::initializer_list<basic_json>;

    using iterator                  = iterator_impl<basic_json>;
    using const_iterator            = iterator_impl<const basic_json>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

public:
    using object_t          = ObjectType<StringType, basic_json>;
    using array_t           = ArrayType<basic_json>;
    using string_t          = StringType;
    using number_integer_t  = NumberIntegerType;
    using number_float_t    = NumberFloatType;
    using boolean_t         = BooleanType;


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


    basic_json(initializer_list init_list)
    {
        const bool is_all_object = std::all_of(init_list.begin(), init_list.end(), [](const basic_json& json){
            return (json.is_array() && json.size() == 2 && json[0].is_string());
        });

        if (is_all_object)
        {
            m_value = json_value<basic_json>(value_t::object);
            for (const basic_json& json : init_list)
            {
                m_value.m_data.object->emplace(std::move(*(json[0].m_value.m_data.string)), std::move(json[1]));
            }
        }
        else
        {
            array(init_list).swap(*this);
        }
    }


public:
    static basic_json object(initializer_list init_list)
    {
        if (init_list.size() != 2 || !(init_list.begin()->is_string()))
        {
            throw json_invalid_key("can't create object from initializer_list");
        }

        basic_json obj(value_t::object);
        obj.m_value.m_data.object->emplace( std::move(*(init_list.begin()->m_value.m_data.string)), 
                                            std::move(*(init_list.begin() + 1)));
        return obj;
    }

    static basic_json array(initializer_list init_list)
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
    template<typename ValueType>
    ValueType get()const
    {
        static_assert(std::is_default_constructible<ValueType>::value, 
                        "types must be DefaultConstructible when used with get()");

        ValueType value;
        m_value.get(value);
        return value;
    }


    const object_t& as_object()const
    {
        if (!is_object())
        {
            throw json_type_error("json value type must be object");
        }

        return *m_value.m_data.object;
    }

    const array_t& as_array()const
    {
        if (!is_array())
        {
            throw json_type_error("json value type must be array");
        }

        return *m_value.m_data.array;
    }

    const string_t& as_string()const
    {
        if (!is_string())
        {
            throw json_type_error("json value type must be string");
        }

        return *m_value.m_data.string;
    }

    number_integer_t as_int()const
    {
        if (is_integer())
        {
            return m_value.m_data.number_integer;
        }

        if (is_float())
        {
            return static_cast<number_integer_t>(m_value.m_data.number_float);
        }

        throw json_type_error("json value type must be number");
    }

    number_float_t as_float()const
    {
        if (is_integer())
        {
            return static_cast<number_float_t>(m_value.m_data.number_integer);
        }

        if (is_float())
        {
            return m_value.m_data.number_float;
        }

        throw json_type_error("json value type must be number");
    }

    boolean_t as_bool()const
    {
        switch (type())
        {
        case value_t::null:
            return false;
        
        case value_t::object:
        case value_t::array:
        case value_t::string:
            return empty();
        
        case value_t::number_integer:
            return m_value.m_data.number_integer != 0;

        case value_t::number_float:
            return m_value.m_data.number_float != 0.0;

        case value_t::boolean:
            return m_value.m_data.boolean;

        default:
            return false;
        }
    }


public:
    basic_json& operator[](size_type index)
    {
        if (is_null())
        {
            m_value = json_value<basic_json>(value_t::array);
        }

        if (!is_array())
        {
            throw json_invalid_key("operator[] called on a non-array object");
        }

        auto array = m_value.m_data.array;
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

        auto array = m_value.m_data.array;
        if (index >= array->size())
        {
            throw std::out_of_range("json operator[] index out of range");
        }

        return (*array)[index];
    }

    template<typename KEY_TYPE,
            typename std::enable_if<std::is_constructible<typename object_t::key_type, KEY_TYPE>::value, int>::type = 0>
    basic_json& operator[](const KEY_TYPE& key)
    {
        if (is_null())
        {
            m_value = json_value<basic_json>(value_t::object);
        }

        if (!is_object())
        {
            throw json_invalid_key("json operator[] called on a non-object type");
        }

        return (*m_value.m_data.object)[key];
    }

    template<typename KEY_TYPE,
            typename std::enable_if<std::is_constructible<typename object_t::key_type, KEY_TYPE>::value, int>::type = 0>
    const basic_json& operator[](const KEY_TYPE& key)const
    {
        if (!is_object())
        {
            throw json_invalid_key("json operator[] called on a non-object type");
        }

        auto iter = m_value.m_data.object->find(key);
        if (iter == m_value.m_data.object->end())
        {
            throw json_invalid_key("json operator[] key out of range");
        }

        return iter->second;
    }


    basic_json& at(size_type index)
    {
        if (!is_array())
        {
            throw json_invalid_key("json at called on a non-array object");
        }

        auto array = m_value.m_data.array;
        if (index >= array->size())
        {
            throw std::out_of_range("json index out of range");
        }

        return (*array)[index];
    }

    const basic_json& at(size_type index)const
    {
        return static_cast<basic_json*>(this)->at(index);
    }

    template<typename KEY_TYPE,
            typename std::enable_if<std::is_constructible<typename object_t::key_type, KEY_TYPE>::value, int>::type = 0>
    basic_json& at(const KEY_TYPE& key)
    {
        if (!is_object())
        {
            throw json_invalid_key("json at called on a non-object type");
        }

        auto iter = m_value.m_data.object->find(key);
        if (iter == m_value.m_data.object->end())
        {
            throw json_invalid_key("json at key out of range");
        }

        return iter->second;
    }

    template<typename KEY_TYPE,
            typename std::enable_if<std::is_constructible<typename object_t::key_type, KEY_TYPE>::value, int>::type = 0>
    const basic_json& at(const KEY_TYPE& key)const
    {
        return static_cast<basic_json*>(this)->at(key);
    }


    // explicitly convert functions
    template<typename Ty>
    operator Ty()const
    {
        return get<Ty>();
    }


public:
    iterator begin()
    {
        iterator iter(this);
        iter.set_begin();
        return iter;
    }

    const_iterator begin()const
    {
        return cbegin();
    }

    const_iterator cbegin()const
    {
        const_iterator iter(this);
        iter.set_begin();
        return iter;
    }

    iterator end()
    {
        iterator iter(this);
        iter.set_end();
        return iter;
    }

    const_iterator end()const
    {
        return cend();
    }

    const_iterator cend()const
    {
        const_iterator iter(this);
        iter.set_end();
        return iter;
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin()const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin()const
    {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend()
    {
        reverse_iterator(being());
    }

    const_reverse_iterator rend()const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend()const
    {
        return const_reverse_iterator(cbegin());
    }


    // dump functions
public:

    friend std::basic_ostream<char_type>& operator<<(std::basic_ostream<char_type>& os, const basic_json& json)
    {
        const auto indent_step = (os.width() > 0 ? os.width() : 0);
        os.width(0);

        stream_output_adapter<char_type> stream_adapter(os);
        json.dump(stream_adapter, static_cast<unsigned int>(indent_step), os.fill());
        return os;
    }


    string_t dump(
        const unsigned int indent = 0,
        const char_type indent_char = ' ')const
    {
        string_t result;
        string_output_adapter<string_t> string_output(result);
        dump(string_output, indent, indent_char);
        return result;
    }


    void dump(
        output_adapter<char_type>& oa,
        const unsigned int indent = 0,
        const char_type indent_char = ' ')const
    {
        json_serializer<basic_json>(oa, indent_char).dump(*this, indent);
    }


    // parse function
public:
    friend operator>>(std::basic_istream<char_type>& is, basic_json& json)
    {
        stream_input_adapter<char_type> adapter(is);
        json = parse(adapter);
        return json;
    }

    static basic_json parse(const string_t& str)
    {
        string_input_adapter<string_t> adapter(str);
        return parse(adapter);
    }

    static basic_json parse(const char_type* str)
    {
        buffer_input_adapter<char_type> adapter(str);
        return parse(adapter);
    }

    static basic_json parse(std::FILE* file)
    {
        file_input_adapter<char_type> adapter(file);
        return parse(adapter);
    }


private:
    static basic_json parse(input_adapter<char_type>& adapter)
    {
        return json_parser<basic_json>(adapter).parse();
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

            case value_t::string:
                return m_value.m_data.string->size();

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