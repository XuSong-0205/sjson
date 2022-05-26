#ifndef JSON_PARSE_HPP
#define JSON_PARSE_HPP

#include <cstdio>       // FILE
#include <cctype>       // isdigit
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

        // skip current char
        read_next();

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
        if (current != '\"')
        {
            return token_type::parse_error;
        }

        string_buffer.clear();
        while (true)
        {
            const auto ch = read_next();
            switch (ch)
            {
            case char_traits::eof():
            {
                return token_type::end_of_input;
            }

            case '\"':
            {
                read_next();
                return token_type::value_string;
            }

            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1A:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
            case 0x1F:
            {
                return token_type::parse_error;
            }

            case '\\':
            {
                switch (read_next())
                {
                case '\"':
                    string_buffer.push_back('\"');
                    break;

                case '\\':
                    string_buffer.push_back('\\');
                    break;

                case '/':
                    string_buffer.push_back('/');
                    break;

                case 'b':
                    string_buffer.push_back('\b');
                    break;

                case 'f':
                    string_buffer.push_back('\f');
                    break;
                    
                case 'n':
                    string_buffer.push_back('\n');
                    break;
                    
                case 'r':
                    string_buffer.push_back('\r');
                    break;
                    
                case 't':
                    string_buffer.push_back('\t');
                    break;

                case 'u':
                {
                    const auto code = get_escaped_code();
                    if (code == -1)
                    {
                        return token_type::parse_error;
                    }

                    string_buffer.push_back(char_traits::to_char_type(code));
                    break;
                }

                default:
                {
                    // invalid escaped char
                    return token_type::parse_error;
                }
                }

                break;
            }

            default:
            {
                string_buffer.push_back(char_traits::to_char_type(ch));
            }
            }
        }
    }

    int32_t get_escaped_code()
    {
        int32_t byte = 0;
        for (const auto factor : { 12, 8, 4, 0 })
        {
            const auto n = read_next();
            if ('0' <= n && n <= '9')
            {
                byte |= (n - '0') << factor;
            }
            else if ('A' <= n && n <= 'F')
            {
                byte |= (n - 'A' + 10) << factor;
            }
            else if ('a' <= n && n <= 'f')
            {
                byte |= (n - 'a' + 10) << factor;
            }
            else
            {
                return -1;
            }
        }

        return byte;
    }

    token_type scan_number()
    {
        is_negative = false;
        number_integer = static_cast<number_integer_t>(0);
        number_float = static_cast<number_float_t>(0.0);

        if (current == '-')
        {
            return scan_negative();
        }

        if (current == '0')
        {
            return scan_zero();
        }

        return scan_integer();
    }

    token_type scan_negative()
    {
        if (current == '-')
        {
            is_negative = true;
            read_next();
            
            return scan_integer();
        }

        return token_type::parse_error;
    }

    token_type scan_zero()
    {
        if (current == '0')
        {
            if (read_next() == '.')
            {
                return scan_float();
            }
            else
            {
                return token_type::value_integer;
            }
        }

        return token_type::parse_error;
    }

    token_type scan_integer()
    {
        if (std::isdigit(current))
        {
            number_integer = static_cast<number_integer_t>(current - '0');

            while (true)
            {
                const auto ch = read_next();
                if (ch == '.')
                {
                    number_float = number_integer;
                    number_integer = 0;
                    
                    return scan_float();
                }

                if (ch == 'e' || ch == 'E')
                {
                    number_float = number_integer;
                    number_integer = 0;
                    
                    return scan_exponent();
                }

                if (std::isdigit(ch))
                {
                    number_integer = number_integer * 10 + (ch - '0');
                }
                else
                {
                    break;
                }
            }

            return token_type::value_integer;
        }

        return token_type::parse_error;
    }

    token_type scan_float()
    {
        if (current != '.')
        {
            return token_type::parse_error;
        }

        
        if (std::isdigit(read_next()))
        {
            number_float_t fraction = static_cast<number_float_t>(0.1);
            number_float += static_cast<number_float_t>(current - '0') * fraction;

            while (true)
            {
                const auto ch = read_next();
                if (ch == 'e' || ch == 'E')
                {
                    return scan_exponent();
                }

                if (std::isdigit(ch))
                {
                    fraction *= static_cast<number_float_t>(0.1);
                    number_float += static_cast<number_float_t>(ch - '0') * fraction;
                }
                else
                {
                    break;
                }
            }

            return token_type::value_float;
        }

        return token_type::parse_error;
    }

    token_type scan_exponent()
    {
        if (current != 'e' && current != 'E')
        {
            return token_type::parse_error;
        }

        read_next();

        if ((std::isdigit(current) && current != '0') || (current == '-') || (current == '+'))
        {
            number_float_t base = 10;
            if (current == '+')
            {
                read_next();
            }
            else if (current == '-')
            {
                base = static_cast<number_float_t>(0.1);
                read_next();
            }

            uint32_t exponect = static_cast<uint32_t>(current - '0');
            while (std::isdigit(read_next()))
            {
                exponect = (exponect * 10) + static_cast<uint32_t>(current - '0');
            }

            number_float_t power = 1;
            for (; exponect; exponect >>= 1, base *= base)
            {
                if (exponect & 1)
                {
                    power *= base;
                }
            }

            number_float *= power;
            return token_type::value_float;
        }

        return token_type::parse_error;
    }


    number_integer_t token_to_integer()const
    {
        return is_negative ? -number_integer : number_integer;
    }

    number_float_t token_to_float()const
    {
        return is_negative ? -number_float : number_float;
    }

    string_t token_to_string()const
    {
        return string_buffer;
    }


private:
    input_adapter<char_type>&   adapter;
    int_type                    current = char_traits::eof();
    bool                        is_negative = false;
    number_integer_t            number_integer = 0;
    number_float_t              number_float = 0.0;
    string_t                    string_buffer;
};





//
// json_parser
//

template <typename BasicJsonType>
class json_parser
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

public:
    json_parser(input_adapter<char_type>& ia)
        : lexer(ia), last_token(token_type::uninitialized) { }

    BasicJsonType parse()
    {
        auto json = parse_value();
        if (get_token() != token_type::end_of_input)
        {
            throw json_parse_error("unexpected token, expect end");
        }

        return json;
    }

private:
    token_type get_token()
    {
        last_token = lexer.scan();
        return last_token;
    }

    BasicJsonType parse_value(bool get_next = true)
    {
        token_type token = last_token;
        if (get_next)
        {
            token = get_token();
        }

        switch (token)
        {
        case token_type::literal_null:
            return nullptr;

        case token_type::literal_true:
            return true;

        case token_type::literal_false:
            return false;

        case token_type::value_integer:
            return lexer.token_to_integer();

        case token_type::value_float:
            return lexer.token_to_float();

        case token_type::value_string:
            return lexer.token_to_string();

        case token_type::begin_object:
        {
            auto json = BasicJsonType(value_t::object);
            while (true)
            {
                // {}, parse a empty object
                if (get_token() == token_type::end_object)
                {
                    break;
                }

                // parse key
                if (last_token != token_type::value_string)
                {
                    break;
                }

                // read key
                string_t key = lexer.token_to_string();
                if (get_token() != token_type::name_separator)
                {
                    break;
                }

                // read value, insert pair
                json.m_value.m_data.object->emplace(std::make_pair(key, parse_value()));

                // read ','
                if (get_token() != token_type::value_separator)
                {
                    break;
                }
            }

            if (last_token != token_type::end_object)
            {
                throw json_parse_error("unexpected token in object");
            }

            return json;
        }

        case token_type::begin_array:
        {
            auto json = BasicJsonType(value_t::array);
            while (true)
            {
                // [], parse a empty array
                if (get_token() == token_type::end_array)
                {
                    break;
                }

                json.m_value.m_data.array->push_back(parse_value(false));

                // read ','
                if (get_token() != token_type::value_separator)
                {
                    break;
                }
            }

            if (last_token != token_type::end_array)
            {
                throw json_parse_error("unexpected token in array");
            }

            return json;
        }

        default:
            throw json_parse_error("unexpected token");
        }
    }

    
private:
    json_lexer<BasicJsonType>   lexer;
    token_type                  last_token;
};


    
}

#endif  // JSON_PARSE_HPP