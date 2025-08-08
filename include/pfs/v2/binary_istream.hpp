////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.08.07 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../error.hpp"
#include "../endian.hpp"
#include "../namespace.hpp"
#include "../numeric_cast.hpp"
#include "../string_view.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>
#include <stack>
#include <utility>
#include <vector>

PFS__NAMESPACE_BEGIN

namespace v2 {

/**
 * No-throw version of @c binary_istream class.
 */
template <endian Endianess = endian::native>
class binary_istream
{
public:
    using size_type = std::uint32_t;

    enum status_enum {
          good
        , out_of_bound
        , corrupted
    };

private:
    char const * _p {nullptr};
    char const * _end {nullptr};
    status_enum _state {status_enum::good};
    std::stack<std::pair<status_enum, char const *>> _state_stack;

public:
    binary_istream (char const * begin, char const * end)
        : _p(begin)
        , _end(end)
    {
        if (_p == nullptr || _end == nullptr)
            throw error {make_error_code(std::errc::invalid_argument)};

        if (_p > _end)
            throw error {make_error_code(std::errc::invalid_argument)};
    }

    binary_istream (char const * begin, std::size_t size)
        : _p(begin)
        , _end(begin + numeric_cast<size_type>(size))
    {
        if (_p == nullptr)
            throw error {make_error_code(std::errc::invalid_argument)};
    }

    binary_istream (binary_istream && other) noexcept
        : _p(other._p)
        , _end(other._end)
        , _state(other._state)
        , _state_stack(std::move(other._state_stack))
    {
        other._p = nullptr;
        other._end = nullptr;
        other._state = status_enum::good;
    }

    binary_istream & operator = (binary_istream && other) noexcept
    {
        if (this != & other) {
            using std::swap;

            binary_istream tmp {std::move(other)};
            std::swap(_p, tmp._p);
            std::swap(_end, tmp._end);
            std::swap(_state, tmp._state);
        }

        return *this;
    }

    binary_istream (binary_istream const &) = delete;
    binary_istream & operator = (binary_istream const &) = delete;

    char const * begin () const noexcept
    {
        return _p;
    }

    bool empty () const noexcept
    {
        return _p == _end;
    }

    bool is_good () const noexcept
    {
        return _state == good;
    }

    operator bool () const noexcept
    {
        return !this->empty() && _state == good;
    }

    size_type available () const noexcept
    {
        return numeric_cast<size_type>(_end - _p);
    }

    /**
     * Skips @a nbytes in stream.
     */
    void skip (size_type nbytes)
    {
        if (_state == status_enum::good) {
            if (this->_p + nbytes <= this->_end) {
                this->_p += nbytes;
            } else {
                _state = status_enum::out_of_bound;
            }
        }
    }

    template <typename T>
    binary_istream & operator >> (T && v)
    {
        unpack(std::forward<T>(v));
        return *this;
    }

    void start_transaction ()
    {
        _state_stack.push(std::make_pair(_state, _p));
    }

    bool commit_transaction ()
    {
        if (_state == status_enum::good) {
            _state_stack.pop();
            return true;
        }

        _state = _state_stack.top().first;
        _p = _state_stack.top().second;
        _state_stack.pop();

        return false;
    }

private:
    /**
     * Reads byte sequence into string view. Lifetime of the string view must be less or equals to
     * the stream's lifetime
     */
    void unpack_helper (string_view & v, size_type n)
    {
        if (n == 0)
            return;

        if (_state != status_enum::good)
            return;

        auto sz = available();

        if (sz < n) {
            _state = status_enum::out_of_bound;
            return;
        }

        v = string_view(_p, n);
        _p += n;
    }

    template <typename Char>
    void unpack_helper (std::vector<Char> & v, std::size_t n)
    {
        if (n == 0)
            return;

        if (_state != status_enum::good)
            return;

        auto sz = available();

        if (sz < n) {
            _state = status_enum::out_of_bound;
            return;
        }

        auto off = v.size();
        v.resize(off + n);
        std::copy(reinterpret_cast<Char const *>(_p), reinterpret_cast<Char const *>(_p) + n, v.data() + off);
        _p += n;
    }

    template <typename Char, std::size_t N>
    void unpack_helper (std::array<Char, N> & v)
    {
        if (_state != status_enum::good)
            return;

        auto sz = available();

        if (sz < N) {
            _state = status_enum::out_of_bound;
            return;
        }

        std::copy(reinterpret_cast<Char const *>(_p), reinterpret_cast<Char const *>(_p) + N, v.data());
        _p += N;
    }

    template <typename T>
    typename std::enable_if<
        std::is_integral<typename std::decay<T>::type>::value
            || std::is_floating_point<typename std::decay<T>::type>::value, void>::type
    unpack (T & v)
    {
        if (_state == status_enum::good) {
            if (_p + sizeof(T) <= _end) {
                _p += unpack_unsafe<Endianess>(_p, v);
            } else {
                _state = status_enum::out_of_bound;
            }
        }
    }

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value, void>::type
    unpack (T & v)
    {
        typename std::underlying_type<T>::type tmp;
        unpack(tmp);

        if (_state == status_enum::good)
            v = static_cast<T>(tmp);
    }

    void unpack (std::pair<char *, std::size_t> v)
    {
        string_view tmp;
        unpack_helper(tmp, v.second);

        if (_state == status_enum::good)
            std::copy(tmp.begin(), tmp.end(), v.first);
    }

    void unpack (std::pair<std::string *, std::size_t> v)
    {
        string_view tmp;
        unpack_helper(tmp, v.second);

        if (_state == status_enum::good)
            *v.first = to_string(tmp);
    }

    void unpack (std::pair<string_view *, std::size_t> v)
    {
        unpack_helper(*v.first, v.second);
    }

    template <typename Char>
    void unpack (std::pair<std::vector<Char> *, std::size_t> v)
    {
        unpack_helper(*v.first, v.second);
    }

    // This method required when used literal value for size:
    // in >> std::make_pair(& vec, 6);
    template <typename Char>
    void unpack (std::pair<std::vector<Char> *, int> v)
    {
        unpack_helper(*v.first, numeric_cast<std::size_t>(v.second));
    }

    template <typename Char, std::size_t N>
    void unpack (std::array<Char, N> & v)
    {
        unpack_helper(v);
    }
};

} // namespace v2

PFS__NAMESPACE_END
