#ifndef JSON_EXCEPTION_HPP
#define JSON_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace sjson
{
    
class json_exception : public std::runtime_error
{
public:
    explicit json_exception(const char* msg)
        : std::runtime_error(msg) { }

    explicit json_exception(const std::string& msg)
        : std::runtime_error(msg) { }
};


class json_type_error : public json_exception
{
public:
    explicit json_type_error(const char* msg)
        : json_exception(msg) { }

    explicit json_type_error(const std::string& msg)
        : json_exception(msg) { }
};


class json_invalid_key : public json_exception
{
public:
    explicit json_invalid_key(const char* msg)
        : json_exception(msg) { }

    explicit json_invalid_key(const std::string& msg)
        : json_exception(msg) { }
};


class json_invalid_iterator : public json_exception
{
public:
    explicit json_invalid_iterator(const char* msg)
        : json_exception(msg) { }

    explicit json_invalid_iterator(const std::string& msg)
        : json_exception(msg) { }
};


class json_parse_error : public json_exception
{
public:
    explicit json_parse_error(const char* msg)
        : json_exception(msg) { }

    explicit json_parse_error(const std::string& msg)
        : json_exception(msg) { }
};


} // namespace sjson

#endif // JSON_EXCEPTION_HPP