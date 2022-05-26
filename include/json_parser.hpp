#ifndef JSON_PARSE_HPP
#define JSON_PARSE_HPP

#include <cstdio>       // FILE
#include <cctype>       // isdight
#include <ios>          // basic_istream, basic_streambuf
#include <type_traits>  // char_traits
#include "json_exception.hpp"

namespace sjson
{

//
// input_adapter
//

template<typename CharT>
struct input_adapter
{
    using char_type     = CharT;
    using char_traits   = std::char_traits<char_type>;
    using int_type      = typename char_traits::int_type;

    virtual ~input_adapter() = default;
    virtual int_type get_char() = 0;
};


template<typename CharT>
struct file_input_adapter : public input_adapter<CharT>
{
    using char_type     = typename input_adapter<CharT>::char_type;
    using char_traits   = typename input_adapter<CharT>::char_traits;
    using int_type      = typename input_adapter<CharT>::int_type;

    file_input_adapter(std::FILE* file_) : file(file_) { }

    virtual int_type get_char()override
    {
        return std::fgetc(file);
    }

private:
    std::FILE* file;
};


template<typename CharT>
struct stream_input_adapter : public input_adapter<CharT>
{
    using char_type     = typename input_adapter<CharT>::char_type;
    using char_traits   = typename input_adapter<CharT>::char_traits;
    using int_type      = typename input_adapter<CharT>::int_type;

    stream_input_adapter(std::basic_istream<char_type>& is) : stream(is), streambuf(*is.rdbuf()) { }

    virtual int_type get_char()override
    {
        auto ch = streambuf.sbumpc();
        if (ch == char_traits::eof())
        {
            stream.clear(stream.rdstate() | std::ios::eofbit);
        }

        return ch;
    }

private:
    std::basic_istream<char_type>&      stream;
    std::basic_streambuf<char_type>&    streambuf;
};


template<typename StringT, typename CharT = typename StringT::value_type>
struct string_input_adapter : public input_adapter<CharT>
{
    using char_type     = typename input_adapter<CharT>::char_type;
    using char_traits   = typename input_adapter<CharT>::char_traits;
    using int_type      = typename input_adapter<CharT>::int_type;
    using size_type     = typename StringT::size_type;

    string_input_adapter(const StringT& s) : str(s), index(0) { }

    virtual int_type get_char()override
    {
        if (index == str.size())
        {
            return char_traits::eof();
        }

        return str[index++];
    }


private:
    const StringT&  str;
    size_type       index;
};


template<typename CharT>
struct buffer_input_adapter : public input_adapter<CharT>
{
    using char_type     = typename input_adapter<CharT>::char_type;
    using char_traits   = typename input_adapter<CharT>::char_traits;
    using int_type      = typename input_adapter<CharT>::int_type;
    using size_type     = std::size_t;

    buffer_input_adapter(const char_type* s) : str(s), index(0) { }

    virtual int_type get_char()override
    {
        if (str[index] == '\0')
        {
            return char_traits::eof();
        }

        return str[index++];
    }


private:
    const char_type*    str;
    size_type           index; 
};



//
// json_lexer
//

enum class token_type
{
    uninitialized,

    literal_null,
    literal_true,
    literal_false,

    value_string,
    value_integer,
    value_float,

    begin_object,
    end_object,

    begin_array,
    end_array,

    name_separator,
    value_separator,

    parse_error,

    end_of_input
};



template<typename BasicJsonType>
class json_lexer
{
public:
    using object_t          = typename BasicJsonType::object_t;
    using array_t           = typename BasicJsonType::array_t;
    using string_t          = typename BasicJsonType::string_t;
    using number_integer_t  = typename BasicJsonType::number_integer_t;
    using number_float_t    = typename BasicJsonType::number_float_t;
    using boolean_t         = typename BasicJsonType::boolean_t;
    using char_type         = typename BasicJsonType::char_type;
    using char_traits       = std::char_traits<char_type>;
    using int_type          = typename char_traits::int_type;

public:
    json_lexer(input_adapter<char_type>& input_adapter) 
        : adapter(input_adapter) 
    { 
        read_next();
    }


    int_type read_next()
    {
        current = adapter.get_char();
        return current;
    }

    void skip_spaces()
    {
        while (current == ' '  ||
               current == '\r' ||
               current == '\t' ||
               current == '\n')
        {
            read_next();
        }
    }

    token_type scan()
    {
        skip_spaces();

        token_type result = token_type::uninitialized;
        switch (current)
        {
        case '{':
            result = token_type::begin_object;
            break;
            
        case '}':
            result = token_type::end_object;
            break;

        case '[':
            result = token_type::begin_array;
            break;

        case ']':
            result = token_type::end_array;
            break;

        case ':':
            result = token_type::name_separator;
            break;

        case ',':
            result = token_type::value_separator;
            break;

        case 'n':
            return scan_literal("null", token_type::literal_null);

        case 't':
            return scan_literal("true", token_type::literal_true);

        case 'f':
            return scan_literal("false", token_type::literal_false);

        case '\"':
            return scan_string();

        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return scan_number();

        case '\0':
        case char_traits::eof():
            return token_type::end_of_input;

        default:
            return token_type::parse_error;
        }

        skip_spaces();

        return result;
    }


    token_type scan_literal(const char_type* str, token_type result)
    {
        for (std::size_t i = 0; str[i] != '\0'; ++i)
        {
            if (str[i] != char_traits::to_char_type(current))
            {
                return token_type::parse_error;
            }

            read_next();
        }

        return result;
    }

    token_type scan_string()
    {
        
    }

    int32_t get_escaped_code()
    {

    }

    token_type scan_number()
    {

    }

    token_type scan_integer()
    {

    }

    token_type scan_float()
    {

    }

    token_type scan_negative()
    {

    }

    token_type scan_zero()
    {

    }

    token_type scan_exponent()
    {

    }


    number_integer_t token_to_integer()const
    {

    }

    number_float_t token_to_float()const
    {

    }

    string_t token_to_string()const
    {

    }


private:
    input_adapter<char_type>&   adapter;
    int_type                    current = char_traits::eof();
    bool                        is_negative = false;
    number_integer_t            value_integer = 0;
    number_float_t              value_float = 0.0;
    string_t                    string_buffer;
};





//
// json_parser
//

template <typename BasicJsonType>
class json_parser
{
public:
    using char_type         = typename BasicJsonType::char_type;
    using object_t          = typename BasicJsonType::object_t;
    using array_t           = typename BasicJsonType::array_t;
    using string_t          = typename BasicJsonType::string_t;;
    using number_integer_t  = typename BasicJsonType::number_integer_t;
    using number_float_t    = typename BasicJsonType::number_float_t;
    using boolean_t         = typename BasicJsonType::boolean_t;
};


    
}

#endif  // JSON_PARSE_HPP