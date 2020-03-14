#pragma once

namespace pfs {

template <typename T, typename U = T>
class compare_op
{
public:
    friend bool operator == (T const & a, U const & b)
    {
        return a.compare(b) == 0;
    }

    friend bool operator < (T const & a, U const & b)
    {
        return a.compare(b) < 0;
    }

    friend bool operator == (U const & a, T const & b)
    {
        return b.compare(a) == 0;
    }

    friend bool operator < (U const & a, T const & b)
    {
        return b.compare(a) > 0;
    }
};

template <typename T>
class compare_op<T,T>
{
public:
    friend bool operator == (T const & a, T const & b)
    {
        return a.compare(b) == 0;
    }

    friend bool operator < (T const & a, T const & b)
    {
        return a.compare(b) < 0;
    }
};

//
// Applicable inside pfs namespace
//

template <typename T, typename U>
inline bool operator != (T const & a, U const & b)
{
    return !(a == b);
}

template <typename T, typename U>
inline bool operator <= (T const & a, U const & b)
{
    return !(b < a);
}

template <typename T, typename U>
inline bool operator > (T const & a, U const & b)
{
    return b < a;
}

template <typename T, typename U>
inline bool operator >= (T const & a, U const & b)
{
    return !(a < b);
}

} // namespace pfs
