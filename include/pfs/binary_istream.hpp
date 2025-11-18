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
//      2025.08.07 v2 is default implementation now.
//      2025.08.12 Moved from `v2` namespace.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
#include "endian.hpp"
#include "namespace.hpp"
#include "numeric_cast.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>
#include <stack>
#include <utility>
#include <vector>

PFS__NAMESPACE_BEGIN

template <typename Container>
void append_bytes (Container & c, char const * data, std::size_t n);

/**
 * No-throw version of @c binary_istream class.
 */
template <endian Endianess = endian::native>
class binary_istream
{
    using view_type = std::pair<char const *, std::size_t>;

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

    binary_istream (binary_istream const &) = delete;
    binary_istream (binary_istream && other) = delete;
    binary_istream & operator = (binary_istream && other) = delete;
    binary_istream & operator = (binary_istream const &) = delete;

    char const * begin () const noexcept
    {
        return _p;
    }

    bool at_end () const noexcept
    {
        return _p == _end;
    }

    /**
     * Synonym to at_end()
     */
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
        unpack(*this, std::forward<T>(v));
        return *this;
    }

    /**
     * Reads @a n bytes from stream and copy them into @a data.
     */
    binary_istream & read (char * data, std::size_t n)
    {
        auto view = read(n);

        // Error
        if (view.first == nullptr)
            return *this;

        std::copy(view.first, view.first + n, data);
        return *this;
    }

    /**
     * Reads @a n bytes from stream and append them into container @a c.
     */
    template <typename Container>
    binary_istream & read (Container & c, std::size_t n)
    {
        auto view = read(n);

        // Error
        if (view.first == nullptr)
            return *this;

        append_bytes<Container>(c, view.first, view.second);
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

    void rollback_transaction ()
    {
        if (!_state_stack.empty()) {
            _state = _state_stack.top().first;
            _p = _state_stack.top().second;
            _state_stack.pop();
        }
    }

private:
    /**
     * Reads byte sequence into string view. Lifetime of the string view must be less or equals to
     * the stream's lifetime
     */
    view_type read (size_type n)
    {
        if (_state != status_enum::good)
            return view_type(nullptr, 0);

        if (n == 0)
            return view_type(nullptr, 0);

        auto sz = available();

        if (sz < n) {
            _state = status_enum::out_of_bound;
            return view_type(nullptr, 0);
        }

        auto result = view_type(_p, n);
        _p += n;
        return result;
    }

    template <typename T>
    friend typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        auto view = in.read(sizeof(T));

        // Error
        if (view.first == nullptr)
            return;

        T const * p = reinterpret_cast<T const *>(view.first);
        v = Endianess == endian::network ? to_native_order(*p) : *p;
    }

    template <typename T>
    friend typename std::enable_if<std::is_enum<T>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        typename std::underlying_type<T>::type tmp;
        unpack(in, tmp);

        if (in.is_good())
            v = static_cast<T>(tmp);
    }

    friend void unpack (binary_istream & in, float & v)
    {
        static_assert(sizeof(float) == sizeof(std::uint32_t)
            , "Float and std::uint32_t are expected to have the same size.");
        union { float f; std::uint32_t d; } x;
        unpack(in, x.d);

        if (in.is_good())
            v = x.f;
    }

    friend void unpack (binary_istream & in, double & v)
    {
        static_assert(sizeof(double) == sizeof(std::uint64_t)
            , "Double and std::uint64_t are expected to have the same size.");
        union { double f; std::uint64_t d; } x;
        unpack(in, x.d);

        if (in.is_good())
            v = x.f;
    }

    template <typename Char, std::size_t N>
    friend void unpack (binary_istream & in, std::array<Char, N> & v)
    {
        in.read(reinterpret_cast<char *>(v.data()), v.size());
    }
};

template <>
inline void append_bytes<std::string> (std::string & c, char const * bytes, std::size_t n)
{
    c.append(bytes, n);
}

template <>
inline void append_bytes<std::vector<char>> (std::vector<char> & c, char const * bytes, std::size_t n)
{
    c.insert(c.end(), bytes, bytes + n);
}

PFS__NAMESPACE_END
