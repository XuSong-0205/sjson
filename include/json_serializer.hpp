#ifndef JSON_SERIALIZER_HPP
#define JSON_SERIALIZER_HPP

#include <type_traits>  // make_unsigned
#include <ostream>      // basic_ostream
#include <string>       // basic_string
#include <array>        // array


namespace sjson
{


/**
 * output_adapter base
 */
template<typename CharT>
struct output_adapter
{
    virtual ~output_adapter() = default;
    virtual write(const CharT ch) = 0;
    virtual write(const CharT* str, std::size_t len) = 0;
};


template<typename CharT>
struct output_stream_adapter : public output_adapter<CharT>
{
    explicit output_stream_adapter(std::basic_ostream<CharT>& stream)noexcept
        : os(stream) 
    { }


    virtual write(const CharT ch)override
    {
        os.put(ch);
    }

    virtual write(const CharT* str, std::size_t len)override
    {
        os.wirte(str, static_cast<std::streamsize>(len));
    }

private:
    std::basic_ostream<CharT>& os;
};


template<typename StringT, typename CharT = typename StringT::value_type>
struct output_string_adapter : public output_adapter<CharT>
{
    explicit output_string_adapter(StringT& s)noexcept
        : str(s)
    { }

    virtual write(const CharT ch)
    {
        str.push_back(ch);
    }

    virtual write(const CharT* str, std::size_t len)
    {
        str.append(str, len);
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
    using string_t          = typename BasicJsonType::string_t;;
    using number_integer_t  = typename BasicJsonType::number_integer_t;
    using number_float_t    = typename BasicJsonType::number_float_t;
    using boolean_t         = typename BasicJsonType::boolean_t;

public:
    json_serializer(output_adapter<char_type>& out_ad, const char_type indent_char)noexcept
        : oa(out_ad), indent(indent_char), indent_string(32, indent_char)
    { }

    void dump(const BasicJsonType& json, 
              const bool pretty_print,
              const unsigned int indent_step,
              const unsigned int current_indent = 0)
    {


    }


    void dump_integer(number_integer_t num)
    {
        if (num == 0)
        {
            oa.write("0", 1);
            return;
        }

        auto uval = static_cast<typename std::make_unsigned<number_integer_t>::type>(0);
        if (num < 0)
        {
            uval = static_cast<decltype(uval)>(0) - num;
        }

        auto iter = number_buffer.rbegin();
        *(iter++) = '\0';

        while (uval)
        {
            *(iter++) = static_cast<char_type>('0' + uval % 10);
            uval /= 10;
        }
        
        if (num < 0)
        {
            *iter = '-';
        }

        oa.write(&(*iter), static_cast<std::size_t>(number_buffer.end() - iter - 1));
    }

    void dump_float(number_float_t num)
    {
        auto&& str = std::to_string(num);
        oa.write(str.c_str(), str.size());
    }

    void dump_string(const string_t& str)
    {
        
    }


private:
    output_adapter<char_type>&  oa;
    char_type                   indent;
    string_t                    indent_string;
    std::array<char, 64>        number_buffer{ };
};



}


#endif // JSON_SERIALIZER_HPP