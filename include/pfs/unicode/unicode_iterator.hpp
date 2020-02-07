#pragma once
#include <string>
#include <pfs/unicode/u8_iterator.hpp>
#include <pfs/unicode/u16_iterator.hpp>
//#include <pfs/unicode/utf32_iterator.hpp>
#include <pfs/limits.hpp>
#include <pfs/type_traits.hpp>

namespace pfs {
namespace unicode {

template <typename CodePointIter>
struct unicode_iterator_traits;

template <typename CodePointIter, typename Pointer>
struct unicode_iterator_limits
{
//    typedef Pointer pointer;
//    typedef typename remove_pointer<pointer>::type value_type;
    static CodePointIter max () { return CodePointIter(reinterpret_cast<Pointer>(numeric_limits<uintptr_t>::max())); }
    static CodePointIter min () { return CodePointIter(reinterpret_cast<Pointer>(numeric_limits<uintptr_t>::min())); }
};

template <typename Pointer>
struct unicode_iterator_limits<Pointer, Pointer>
{
//    typedef Pointer pointer;
//    typedef typename remove_pointer<pointer>::type value_type;
    static Pointer max () { return reinterpret_cast<Pointer>(numeric_limits<uintptr_t>::max()); }
    static Pointer min () { return reinterpret_cast<Pointer>(numeric_limits<uintptr_t>::min()); }
};

template <>
struct unicode_iterator_traits<unsigned char *>
    :  unicode_iterator_limits<unsigned char *, unsigned char *>
{
    typedef utf8_iterator<unsigned char *> iterator;
    typedef u8_output_iterator<unsigned char *> output_iterator;
    typedef u8_input_iterator<unsigned char *> input_iterator;
};

template <>
struct unicode_iterator_traits<unsigned char const *>
    :  unicode_iterator_limits<unsigned char const *, unsigned char const *>
{
    typedef utf8_iterator<unsigned char const *> iterator;
    typedef u8_input_iterator<unsigned char const *> input_iterator;
};

template <>
struct unicode_iterator_traits<char *>
    :  unicode_iterator_limits<char *, char *>
{
    typedef utf8_iterator<char *> iterator;
    typedef u8_output_iterator<char *> output_iterator;
    typedef u8_input_iterator<char *> input_iterator;
};

template <>
struct unicode_iterator_traits<char const *>
    :  unicode_iterator_limits<char const *, char const *>
{
    typedef utf8_iterator<char const *> iterator;
    typedef u8_input_iterator<char const *> input_iterator;
};

#if PFS_CC_MSC

template <>
struct unicode_iterator_traits<unsigned wchar_t *>
    :  unicode_iterator_limits<unsigned wchar_t *, unsigned wchar_t *>
{
    typedef utf16_iterator<unsigned wchar_t *> iterator;
    typedef u8_output_iterator<unsigned wchar_t *> output_iterator;
    typedef u8_input_iterator<unsigned wchar_t *> input_iterator;
};

template <>
struct unicode_iterator_traits<unsigned wchar_t const *>
    :  unicode_iterator_limits<unsigned wchar_t const *, unsigned wchar_t const *>
{
    typedef utf16_iterator<unsigned wchar_t const *> iterator;
    typedef u8_input_iterator<unsigned *> input_iterator;
};

template <>
struct unicode_iterator_traits<wchar_t *>
    :  unicode_iterator_limits<wchar_t *, wchar_t *>
{
    typedef utf16_iterator<wchar_t *> iterator;
    typedef u8_output_iterator<wchar_t *> output_iterator;
    typedef u8_input_iterator<wchar_t *> input_iterator;
};

template <>
struct unicode_iterator_traits<wchar_t const *>
    :  unicode_iterator_limits<wchar_t const *, wchar_t const *>

{
    typedef utf16_iterator<wchar_t const *> iterator;
    typedef u8_input_iterator<wchar_t const *> input_iterator;
};

template <>
struct unicode_iterator_traits<std::wstring::iterator>
    :  unicode_iterator_limits<std::wstring::iterator, wchar_t *>
{
    typedef utf16_iterator<std::wstring::iterator> iterator;
    typedef u8_output_iterator<std::wstring::iterator> output_iterator;
    typedef u8_input_iterator<std::wstring::iterator> input_iterator;
};

template <>
struct unicode_iterator_traits<std::wstring::const_iterator>
    :  unicode_iterator_limits<std::wstring::const_iterator, wchar_t const*>
{
    typedef utf16_iterator<std::wstring::const_iterator> iterator;
    typedef u8_input_iterator<std::wstring::const_iterator> input_iterator;
};

#endif

}}
