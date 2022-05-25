#ifndef JSON_ITERATOR_HPP
#define JSON_ITERATOR_HPP

#include <cstddef>
#include <iterator>
#include "json_value.hpp"
#include "json_exception.hpp"

namespace sjson
{

class primitive_iterator
{
private:
    using difference_type = std::ptrdiff_t;
    difference_type m_it;

    static constexpr difference_type begin_value = 0;
    static constexpr difference_type end_value = begin_value + 1;


public:
    constexpr explicit primitive_iterator(difference_type it = begin_value) : m_it(it) { }

    void set_begin() noexcept                   { m_it = begin_value;           }
    void set_end() noexcept                     { m_it = end_value;             }

    constexpr bool is_begin()const noexcept     { return m_it == begin_value;   }
    constexpr bool is_end()const noexcept       { return m_it == end_value;     }

    primitive_iterator& operator++()noexcept
    { 
        ++m_it; 
        return *this;    
    }

    primitive_iterator operator++(int)noexcept
    {
        auto old = *this;
        ++(*this);
        return old;
    }

    primitive_iterator& operator--()noexcept
    { 
        --m_it;
        return *this;    
    }

    primitive_iterator operator--(int)noexcept
    {
        auto old = *this;
        --(*this);
        return old;
    }

    primitive_iterator operator+(difference_type off)const noexcept
    {
        auto res = *this;
        res += off;
        return res;
    }

    primitive_iterator operator-(difference_type off)const noexcept
    {
        auto res = *this;
        res -= off;
        return res;
    }

    primitive_iterator& operator+=(difference_type off)noexcept
    {
        m_it += off;
        return *this;
    }

    primitive_iterator& operator-=(difference_type off)noexcept
    {
        m_it -= off;
        return *this;
    }


    friend bool operator==(primitive_iterator lhs, primitive_iterator rhs) noexcept {  return lhs.m_it == rhs.m_it; }
    friend bool operator!=(primitive_iterator lhs, primitive_iterator rhs) noexcept {  return lhs.m_it != rhs.m_it; }
    friend bool operator< (primitive_iterator lhs, primitive_iterator rhs) noexcept {  return lhs.m_it <  rhs.m_it; }
    friend bool operator<=(primitive_iterator lhs, primitive_iterator rhs) noexcept {  return lhs.m_it <= rhs.m_it; }
    friend bool operator> (primitive_iterator lhs, primitive_iterator rhs) noexcept {  return lhs.m_it >  rhs.m_it; }
    friend bool operator>=(primitive_iterator lhs, primitive_iterator rhs) noexcept {  return lhs.m_it >= rhs.m_it; }

};


template<typename BasicJsonType>
class internal_iterator
{
public:
    typename BasicJsonType::object_t::iterator  object_iter{ };
    typename BasicJsonType::array_t::iterator   array_iter{ };
    primitive_iterator                          original_iter{ };  // for other types
};


template<typename BasicJsonType>
class iterator_impl
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
    using value_type        = BasicJsonType;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_pointer     = const value_type*;
    using const_reference   = const value_type&;

public:
    explicit iterator_impl(pointer json) : m_json(json) { }


    const_pointer operator->()const
    {
        check_data();
        check_iterator();

        switch (m_json->type())
        {
        case value_t::object:
            return &(m_iter.object_iter->second);
        case value_t::array:
            return &(*m_iter.array_iter);
        default:
            return m_json;
        }
    }

    pointer operator->()
    {
        return const_cast<pointer>(const_cast<const iterator_impl*>(this)->operator->());
    }

    reference operator*()
    {
        return *(operator->());
    }

    const_reference operator*()const
    {
        return *(operator->());
    }


    const typename object_t::key_type& key()const
    {
        check_data();
        check_iterator();
        if (!m_json->is_object())
        {
            throw json_invalid_iterator("cannot use key() with non-object type");
        }

        return m_iter.object_iter->first;
    }

    const_reference value()const
    {
        return operator*();
    }

    void set_begin()
    {
        check_data();

        switch (m_json->type())
        {
        case value_t::object:
        {
            m_iter.object_iter = m_json->m_value.m_data.object->begin();
            break;
        }

        case value_t::array:
        {
            m_iter.array_iter = m_json->m_value.m_data.array->begin();
            break;
        }

        default:
        {
            m_iter.original_iter.set_begin();
            break;
        }
        }
    }

    void set_end()
    {
        check_data();

        switch (m_json->type())
        {
        case value_t::object:
        {
            m_iter.object_iter = m_json->m_value.m_data.object->end();
            break;
        }

        case value_t::array:
        {
            m_iter.array_iter = m_json->m_value.m_data.array->end();
            break;
        }

        default:
        {
            m_iter.original_iter.set_end();
            break;
        }
        }
    }


    iterator_impl& operator++()
    {
        check_data();
        
        switch (m_json->type())
        {
        case value_t::object:
        {
            std::advance(m_iter.object_iter, 1);
            break;
        }

        case value_t::array:
        {
            std::advance(m_iter.array_iter, 1);
            break;
        }

        default:
        {
            ++m_iter.original_iter;
            break;
        }
        }

        return *this;
    }

    iterator_impl operator++(int)
    {
        auto old = *this;
        ++(*this);
        return old;
    }

    iterator_impl& operator--()
    {
        check_data();
        
        switch (m_json->type())
        {
        case value_t::object:
        {
            std::advance(m_iter.object_iter, -1);
            break;
        }

        case value_t::array:
        {
            std::advance(m_iter.array_iter, -1);
            break;
        }

        default:
        {
            --m_iter.original_iter;
            break;
        }
        }
    }

    iterator_impl operator--(int)
    {
        auto old = *this;
        --(*this);
        return old;
    }

    iterator_impl operator+(difference_type off)
    {
        auto res = *this;
        res += off;
        return res;
    }

    iterator_impl operator-(difference_type off)
    {
        auto res = *this;
        res -= off;
        return res;
    }

    iterator_impl& operator+=(difference_type off)
    {
        check_data();
        
        switch (m_json->type())
        {
        case value_t::object:
        {
            throw json_invalid_iterator("cannot use offsets with object type");
            break;
        }

        case value_t::array:
        {
            std::advance(m_iter.array_iter, off);
            break;
        }

        default:
        {
            m_iter.original_iter += off;
            break;
        }
        }
    }

    iterator_impl& operator-=(difference_type off)
    {
        operator+=(-off);
    }

    bool operator==(const iterator_impl& other)
    {
        if (m_json != other.m_json)
        {
            return false;
        }

        if (m_json == nullptr)
        {
            throw json_invalid_iterator("json data is nullptr");
        }

        switch (m_json->type())
        {
        case value_t::object:
        {
            return m_iter.object_iter == other.m_iter.object_iter;
        }

        case value_t::array:
        {
            return m_iter.array_iter == other.m_iter.array_iter;
        }

        default:
        {
            return m_iter.original_iter == other.m_iter.original_iter;
        }
        }
    }

    bool operator!=(const iterator_impl& other)
    {
        return !(*this == other);
    }

    bool operator<(const iterator_impl& other)
    {
        check_data();
        other.check_data();

        if (m_json != other.m_json)
        {
            throw json_invalid_iterator("cannot compare iterators of different objects");
        }

        switch (m_json->type())
        {
        case value_t::object:
        {
            throw json_invalid_iterator("cannot compare iterators with object type");
        }

        case value_t::array:
        {
            return m_iter.array_iter < other.m_iter.array_iter;
        }

        default:
        {
            return m_iter.original_iter < other.m_iter.original_iter;
        }
        }
    }

    bool operator<=(const iterator_impl& other)
    {
        return !(other < *this);
    }

    bool operator>(const iterator_impl& other)
    {
        return other < *this;
    }

    bool operator>=(const iterator_impl& other)
    {
        return !(*this < other);
    }


private:
    void check_data()const
    {
        if (m_json == nullptr)
        {
            throw json_invalid_iterator("iterator contains an empty object");
        }
    }

    void check_iterator()const
    {
        switch (m_json->type())
        {
        case value_t::object:
        {
            if (m_iter.object_iter == m_json->m_value.m_data.object->end())
            {
                throw std::out_of_range("iterator out of range");
            }
            break;
        }

        case value_t::array:
        {
            if (m_iter.array_iter == m_json->m_value.m_data.array->end())
            {
                throw std::out_of_range("iterator out of range");
            }
            break;
        }

        default:
        {
            if (m_iter.original_iter.is_end())
            {
                throw std::out_of_range("iterator out of range");
            }
            break;
        }
        }
    }

private:
    pointer                             m_json;
    internal_iterator<BasicJsonType>    m_iter;
};

    
}

#endif  // JSON_ITERATOR_HPP