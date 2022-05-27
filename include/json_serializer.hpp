#ifndef JSON_SERIALIZER_HPP
#define JSON_SERIALIZER_HPP

#include <cstdio>       // sprintf
#include <type_traits>  // make_unsigned
#include <ostream>      // basic_ostream
#include <sstream>      // ostringstream
#include <string>       // basic_string
#include <array>        // array
#include "json_value.hpp"

namespace sjson
{


/**
 * output_adapter base
 */
template<typename CharT>
struct output_adapter
{
    virtual ~output_adapter() = default;
    virtual void write(const CharT ch) = 0;
    virtual void write(const CharT* str, std::size_t len) = 0;
    virtual void write(const CharT* str)
    {
        using char_traits = std::char_traits<CharT>;
        write(str, static_cast<std::size_t>(char_traits::length(str)));
    }
};


template<typename CharT>
struct stream_output_adapter : public output_adapter<CharT>
{
    explicit stream_output_adapter(std::basic_ostream<CharT>& stream)noexcept
        : os(stream) 
    { }


    virtual void write(const CharT ch)override
    {
        os.put(ch);
    }

    virtual void write(const CharT* str, std::size_t len)override
    {
        os.write(str, static_cast<std::streamsize>(len));
    }

private:
    std::basic_ostream<CharT>& os;
};


template<typename StringT, typename CharT = typename StringT::value_type>
struct string_output_adapter : public output_adapter<CharT>
{
    explicit string_output_adapter(StringT& s)noexcept
        : str(s)
    { }

    virtual void write(const CharT ch)override
    {
        str.push_back(ch);
    }

    virtual void write(const CharT* s, std::size_t len)override
    {
        str.append(s, len);
    }

private:
    StringT& str;
};



template<typename BasicJsonType>
class json_serializer
{
public:
    using char_type         = typename BasicJsonType::char_type;
    using object_t          = typename BasicJsonType::object_t;
    using array_t           = typename BasicJsonType::array_t;
    using string_t          = typename BasicJsonType::string_t;
    using number_integer_t  = typename BasicJsonType::number_integer_t;
    using number_float_t    = typename BasicJsonType::number_float_t;
    using boolean_t         = typename BasicJsonType::boolean_t;

public:
    json_serializer(output_adapter<char_type>& out_ad, const char_type ind_char)noexcept
        : oa(out_ad), indent_char(ind_char), indent_string(32, ind_char)
    { }

    
    void dump(const BasicJsonType& json,
              const unsigned int indent_step,
              const unsigned int current_indent = 0)
    {
        switch (json.type())
        {
        case value_t::null:
        {
            oa.write("null");
            return;
        }

        case value_t::object:
        {
            auto& object = *json.m_value.m_data.object;
            if (object.empty())
            {
                oa.write("{}");
                return;
            }

            if (indent_step > 0)
            {
                oa.write("{\n");

                const auto new_indent = current_indent + indent_step;
                if (indent_string.size() < new_indent)
                {
                    indent_string.resize(indent_string.size() * 2, indent_char);
                }

                auto iter = object.cbegin();
                const auto size = object.size();
                for (std::size_t i = 0; i < size; ++i, ++iter)
                {
                    oa.write(indent_string.c_str(), new_indent);
                    oa.write('\"');
                    oa.write(iter->first.c_str(), iter->first.size());
                    oa.write("\":");
                    if (indent_step > 0) oa.write(' ');
                    dump(iter->second, indent_step, new_indent);

                    // not last element
                    if (i != size - 1)
                        oa.write(",\n");
                }

                oa.write('\n');
                oa.write(indent_string.c_str(), current_indent);
                oa.write('}');
            }
            else
            {
                oa.write('{');

                auto iter = object.cbegin();
                const auto size = object.size();
                for (std::size_t i = 0; i < size; ++i, ++iter)
                {
                    oa.write('\"');
                    oa.write(iter->first.c_str(), iter->first.size());
                    oa.write("\":");
                    if (indent_step > 0) oa.write(' ');
                    dump(iter->second, indent_step, current_indent);

                    // not last element
                    if (i != size - 1)
                        oa.write(',');
                }

                oa.write('}');
            }

            return;
        }

        case value_t::array:
        {
            auto& array = *json.m_value.m_data.array;
            if (array.empty())
            {
                oa.write("[]");
                return;
            }

            if (indent_step > 0)
            {
                oa.write("[\n");

                const auto new_indent = current_indent + indent_step;
                if (indent_string.size() < new_indent)
                {
                    indent_string.resize(indent_string.size() * 2, indent_char);
                }

                auto iter = array.cbegin();
                const auto size = array.size();
                for (std::size_t i = 0; i < size; ++i, ++iter)
                {
                    oa.write(indent_string.c_str(), new_indent);
                    dump(*iter, indent_step, new_indent);

                    // not last element
                    if (i != size - 1)
                        oa.write(",\n");
                }

                oa.write('\n');
                oa.write(indent_string.c_str(), current_indent);
                oa.write(']');
            }
            else
            {
                oa.write('[');
                
                auto iter = array.cbegin();
                const auto size = array.size();
                for (std::size_t i = 0; i < size; ++i, ++iter)
                {
                    dump(*iter, indent_step, current_indent);

                    // not last element
                    if (i != size - 1)
                        oa.write(',');
                }

                oa.write(']');
            }
            
            return;
        }

        case value_t::string:
        {
            oa.write('\"');
            dump_string(*json.m_value.m_data.string);
            oa.write('\"');
            return;
        }

        case value_t::number_integer:
        {
            dump_integer(json.m_value.m_data.number_integer);
            return;
        }

        case value_t::number_float:
        {
            dump_float(json.m_value.m_data.number_float);
            return;
        }

        case value_t::boolean:
        {
            if (json.m_value.m_data.boolean)
            {
                oa.write("true");
            }
            else
            {
                oa.write("false");
            }
            return;
        }

        }
    }


    void dump_integer(number_integer_t num)
    {
        if (num == 0)
        {
            oa.write('0');
            return;
        }

        auto uval = static_cast<typename std::make_unsigned<number_integer_t>::type>(0);
        if (num < 0)
        {
            uval = static_cast<decltype(uval)>(0) - num;
        }
        else
        {
            uval = num;
        }

        auto iter = number_buffer.rbegin();
        *iter = '\0';

        while (uval)
        {
            *(++iter) = static_cast<char_type>('0' + uval % 10);
            uval /= 10;
        }
        
        if (num < 0)
        {
            *(++iter) = '-';
        }

        oa.write(&(*iter), static_cast<std::size_t>(iter - number_buffer.rbegin()));
    }

    void dump_float(number_float_t num)
    {
        std::ostringstream oss;
        oss << std::setprecision(sizeof(number_float_t) == 8 ? 15 : 7) << num;
        auto&& str = oss.str();
        oa.write(str.c_str(), str.size());
    }

    void dump_string(const string_t& str)
    {
        std::size_t index = 0;
        for (const auto& ch : str)
        {
            switch (ch)
            {
                case '\b':
                case '\f':
                case '\n':
                case '\r':
                case '\t':
                case '\\':
                case '\"':
                {
                    string_buffer[index++] = '\\';
                    string_buffer[index++] = ch;
                    break;
                }

                default:
                {
                    const auto code = static_cast<uint32_t>(ch);
                    if (code < 0x1f)
                    {
                        // escape control characters (0x00..0x1F)
                        std::snprintf(string_buffer.data() + index, 7, "\\u%04X", uint16_t(code));
                        index += 6;
                    }
                    else
                    {
                        string_buffer[index++] = ch;
                    }
                }
            }

            if (string_buffer.size() - index < 7)
            {
                oa.write(string_buffer.data(), index);
                index = 0;
            }
        }

        if (index > 0)
        {
            oa.write(string_buffer.data(), index);
            index = 0;
        }
    }


private:
    output_adapter<char_type>&  oa;
    char_type                   indent_char;
    string_t                    indent_string;
    std::array<char, 21>        number_buffer{ };
    std::array<char, 512>       string_buffer{ };
};



}


#endif // JSON_SERIALIZER_HPP