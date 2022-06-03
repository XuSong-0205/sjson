#ifndef JSON_HPP
#define JSON_HPP

#include <map>      // map
#include <vector>   // vector
#include <string>   // string
#include <cstdint>  // int64_t
#include <cstddef>  // nullptr_t, ptrdiff_t, size_t
#include "json_basic.hpp"


namespace sjson
{

using json      = detail::basic_json<>;
using wjson     = detail::str_json<::std::wstring>;     // todo
using u16json   = detail::str_json<::std::u16string>;   // todo
using u32json   = detail::str_json<::std::u32string>;   // todo



// 
// operator""__json()
// 
inline json operator ""_json(const char* str, size_t)
{
    return json::parse(str);
}



} // namespace sjson



namespace std
{

// 
// hash<json>
// 
template<>
struct hash<::sjson::json>
{
    std::size_t operator()(const ::sjson::json& json)const 
    {
        return std::hash<::sjson::json::string_t>()(json.dump());
    }
};


// 
// swap<json>
// 
template<>
void swap<::sjson::json>(::sjson::json& lhs, ::sjson::json& rhs)noexcept
{
    lhs.swap(rhs);
}


} // namespace std


#endif // JSON_HPP