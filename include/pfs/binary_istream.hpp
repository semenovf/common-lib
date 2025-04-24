////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023-2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2023.03.22 Initial version (as original binary_istream).
//      2024.05.07 Initial version (named as binary_istream_nt).
//      2025.01.23 Renamed into binary_istream.
//                 Refactored.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "endian.hpp"
#include "numeric_cast.hpp"
#include "string_view.hpp"
#include <cstdint>
#include <algorithm>
#include <functional>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

namespace pfs {

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
    binary_istream & operator >> (T & v)
    {
        unpack(*this, v);
        return *this;
    }

    template <std::size_t N>
    binary_istream & operator >> (std::array<char, N> & v)
    {
        unpack(*this, v);
        return *this;
    }

    template <typename SizeType>
    binary_istream & operator >> (std::pair<std::string *, SizeType *> && v) // && here
    {
        string_view tmp;
        unpack(*this, tmp, *v.second);

        if (_state == status_enum::good)
            *v.first = to_string(tmp);

        return *this;
    }

    template <typename Char, typename SizeType>
    binary_istream & operator >> (std::pair<std::vector<Char> *, SizeType *> & v)
    {
        unpack(*this, *v.first, *v.second);
        return *this;
    }

    template <typename SizeType>
    binary_istream & operator >> (std::pair<SizeType *, std::string *> && v) // && here
    {
        unpack(*this, *v.first);

        if (_state == status_enum::good) {
            string_view tmp;

            unpack(*this, tmp, *v.first);

            if (_state == status_enum::good)
                *v.second = to_string(tmp);
        }

        return *this;
    }

    template <typename Char, typename SizeType>
    binary_istream & operator >> (std::pair<SizeType *, std::vector<Char> *> && v)
    {
        unpack(*this, *v.first);
        unpack(*this, *v.second, static_cast<size_type>(*v.first));
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
    template <typename T>
    friend typename std::enable_if<std::is_same<typename std::decay<T>::type, bool>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        if (in._state == status_enum::good) {
            if (in._p + sizeof(std::int8_t) <= in._end) {
                auto p = reinterpret_cast<std::int8_t const *>(in._p);
                v = static_cast<bool>(*p);
                in._p += sizeof(std::int8_t);
            } else {
                in._state = status_enum::out_of_bound;
            }
        }
    }

    template <typename T>
    friend typename std::enable_if<std::is_integral<T>::value && !std::is_same<typename std::decay<T>::type, bool>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        if (in._state == status_enum::good) {
            if (in._p + sizeof(T) <= in._end) {
                T const * p = reinterpret_cast<T const *>(in._p);
                v = Endianess == endian::network ? to_native_order(*p) : *p;
                in._p += sizeof(T);
            } else {
                in._state = status_enum::out_of_bound;
            }
        }
    }

    template <typename T>
    friend typename std::enable_if<std::is_enum<T>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        typename std::underlying_type<T>::type tmp;
        unpack(in, tmp);

        if (in._state == status_enum::good)
            v = static_cast<T>(tmp);
    }

    friend void unpack (binary_istream & in, float & v)
    {
        union { float f; std::uint32_t d; } x;
        unpack(in, x.d);

        if (in._state == status_enum::good)
            v = x.f;
    }

    /**
     */
    friend void unpack (binary_istream & in, double & v)
    {
        union { double f; std::uint64_t d; } x;
        unpack(in, x.d);

        if (in._state == status_enum::good)
            v = x.f;
    }

    /**
     * Reads byte sequence into string view. Lifetime of the string view must be less or equals to
     * the stream's lifetime
     */
    friend void unpack (binary_istream & in, string_view & v, size_type n)
    {
        if (n == 0)
            return;

        if (in._state != status_enum::good)
            return;

        auto sz = in.available();

        if (sz < n) {
            in._state = status_enum::out_of_bound;
            return;
        }

        v = string_view(in._p, n);
        in._p += n;
    }

    template <typename Char>
    static void unpack_helper (binary_istream & in, std::vector<Char> & v, size_type n)
    {
        if (n == 0)
            return;

        if (in._state != status_enum::good)
            return;

        auto sz = in.available();

        if (sz < n) {
            in._state = status_enum::out_of_bound;
            return;
        }

        v.resize(n);
        std::memcpy(reinterpret_cast<Char *>(v.data()), in._p, n);
        in._p += n;
    }

    friend void unpack (binary_istream & in, std::vector<char> & v, size_type n)
    {
        binary_istream<Endianess>::unpack_helper(in, v, n);
    }

    friend void unpack (binary_istream & in, std::vector<std::uint8_t> & v, size_type n)
    {
        binary_istream<Endianess>::unpack_helper(in, v, n);
    }

    /**
     */
    template <std::size_t N>
    friend void unpack (binary_istream & in, std::array<char, N> & v)
    {
        if (in._state != status_enum::good)
            return;

        auto sz = in.available();

        if (sz < N) {
            in._state = status_enum::out_of_bound;
            return;
        }

        std::copy(in._p, in._p + N, v.data());
        in._p += N;
    }
};

} // namespace pfs
