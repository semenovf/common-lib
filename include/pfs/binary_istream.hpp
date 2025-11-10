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
#include "string_view.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>
#include <stack>
#include <utility>
#include <vector>

PFS__NAMESPACE_BEGIN

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
    friend typename std::enable_if<
        std::is_integral<typename std::decay<T>::type>::value
            || std::is_floating_point<typename std::decay<T>::type>::value, void>::type
    unpack (binary_istream & in, T & v)
    {
        if (in._state == status_enum::good) {
            if (in._p + sizeof(T) <= in._end) {
                in._p += unpack_unsafe<Endianess>(in._p, v);
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

    template <typename SizeType>
    friend void unpack (binary_istream & in, std::pair<char *, SizeType> v)
    {
        string_view tmp;
        in.unpack_helper(tmp, numeric_cast<std::size_t>(v.second));

        if (in._state == status_enum::good)
            std::copy(tmp.begin(), tmp.end(), v.first);
    }

    template <typename SizeType>
    friend void unpack (binary_istream & in, std::pair<std::string *, SizeType> v)
    {
        string_view tmp;
        in.unpack_helper(tmp, numeric_cast<std::size_t>(v.second));

        if (in._state == status_enum::good)
            *v.first = to_string(tmp);
    }

    template <typename SizeType>
    friend void unpack (binary_istream & in, std::pair<string_view *, SizeType> v)
    {
        in.unpack_helper(*v.first, numeric_cast<std::size_t>(v.second));
    }

    template <typename Char, typename SizeType>
    friend void unpack (binary_istream & in, std::pair<std::vector<Char> *, SizeType> v)
    {
        in.unpack_helper(*v.first, numeric_cast<std::size_t>(v.second));
    }

    template <typename Char, std::size_t N>
    friend void unpack (binary_istream & in, std::array<Char, N> & v)
    {
        in.unpack_helper(v);
    }
};

PFS__NAMESPACE_END
