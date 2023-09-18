#pragma once

#ifndef UTF_SUBDIR
#   error "UTF subdirectory must be defined"
#endif

extern "C" unsigned char cyrillic1_txt[];
extern "C" unsigned int  cyrillic1_txt_len;
extern "C" unsigned char cyrillic2_txt[];
extern "C" unsigned int  cyrillic2_txt_len;
extern "C" unsigned char cyrillic_txt[];
extern "C" unsigned int  cyrillic_txt_len;
extern "C" unsigned char greek_txt[];
extern "C" unsigned int  greek_txt_len;
extern "C" unsigned char gothic_txt[];
extern "C" unsigned int  gothic_txt_len;
extern "C" unsigned char mideng_txt[];
extern "C" unsigned int  mideng_txt_len;
extern "C" unsigned char midger_txt[];
extern "C" unsigned int  midger_txt_len;
extern "C" unsigned char multilang_txt[];
extern "C" unsigned int  multilang_txt_len;
extern "C" unsigned char rune_txt[];
extern "C" unsigned int  rune_txt_len;
extern "C" unsigned char vietnamese_txt[];
extern "C" unsigned int  vietnamese_txt_len;

struct test_data {
    char const *    name;
    char const *    filename;
    unsigned char * text;
    unsigned int    len;
    unsigned int    nchars;
};

static test_data data[] = {
      { "cyrillic1" , "unicode/" UTF_SUBDIR "/data/cyrillic1.txt" , cyrillic1_txt , cyrillic1_txt_len  ,    1 }
    , { "cyrillic2" , "unicode/" UTF_SUBDIR "/data/cyrillic2.txt" , cyrillic2_txt , cyrillic2_txt_len  ,    2 }
    , { "cyrillic"  , "unicode/" UTF_SUBDIR "/data/cyrillic.txt"  , cyrillic_txt  , cyrillic_txt_len   ,   66 }
    , { "gothic"    , "unicode/" UTF_SUBDIR "/data/gothic.txt"    , gothic_txt    , gothic_txt_len     ,   83 }
    , { "greek"     , "unicode/" UTF_SUBDIR "/data/greek.txt"     , greek_txt     , greek_txt_len      ,  204 }
    , { "mideng"    , "unicode/" UTF_SUBDIR "/data/mideng.txt"    , mideng_txt    , mideng_txt_len     ,  272 }
    , { "midger"    , "unicode/" UTF_SUBDIR "/data/midger.txt"    , midger_txt    , midger_txt_len     ,  353 }
    , { "multilang" , "unicode/" UTF_SUBDIR "/data/multilang.txt" , multilang_txt , multilang_txt_len  , 9175 }
    , { "rune"      , "unicode/" UTF_SUBDIR "/data/rune.txt"      , rune_txt      , rune_txt_len       ,  145 }
    , { "vietnamese", "unicode/" UTF_SUBDIR "/data/vietnamese.txt", vietnamese_txt, vietnamese_txt_len ,   43 }
};

template <typename Iterator>
Iterator iter_cast (unsigned char *);

template <typename Iterator>
Iterator iter_cast (uint16_t *);

template <>
inline unsigned char * iter_cast<unsigned char *> (unsigned char * it)
{
    return it;
}

template <>
inline const unsigned char * iter_cast<const unsigned char *> (unsigned char * it)
{
    return it;
}

template <>
inline char * iter_cast<char *> (unsigned char * it)
{
    return reinterpret_cast<char *>(it);
}

template <>
inline const char * iter_cast<const char *> (unsigned char * it)
{
    return reinterpret_cast<const char *>(it);
}

// FIXME std::string::iterator() constructor need 2 args: pointer and owner, but nullptr is not valid value
template <>
inline std::string::iterator iter_cast<std::string::iterator> (unsigned char * it)
{
#if _MSC_VER
    return std::string::iterator(reinterpret_cast<char *>(it), nullptr); // <=== FIXME nullptr is not valid value here
#else    
    return std::string::iterator(reinterpret_cast<char *>(it));
#endif
}

template <>
inline std::string::const_iterator iter_cast<std::string::const_iterator> (unsigned char * it)
{
#if _MSC_VER
    return std::string::const_iterator(reinterpret_cast<char *>(it), nullptr); // <=== FIXME nullptr is not valid value here
#else
    return std::string::const_iterator(reinterpret_cast<char *>(it));
#endif    
}

template <>
inline std::uint16_t * iter_cast<std::uint16_t *> (std::uint16_t * it)
{
    return it;
}

template <>
inline std::uint16_t const * iter_cast<std::uint16_t const *> (std::uint16_t * it)
{
    return reinterpret_cast<std::uint16_t const *>(it);
}

//
template <>
inline std::int16_t * iter_cast<std::int16_t *> (std::uint16_t * it)
{
    return reinterpret_cast<int16_t *>(it);
}

template <>
inline std::int16_t const * iter_cast<std::int16_t const *> (std::uint16_t * it)
{
    return reinterpret_cast<std::int16_t const *>(it);
}

template <>
inline std::basic_string<std::int16_t>::iterator iter_cast<std::basic_string<std::int16_t>::iterator> (std::uint16_t * it)
{
#if _MSC_VER
    return std::basic_string<std::int16_t>::iterator(reinterpret_cast<std::int16_t*>(it), nullptr);
#else
    return std::basic_string<std::int16_t>::iterator(reinterpret_cast<std::int16_t *>(it));
#endif
}

template <>
inline std::basic_string<std::int16_t>::const_iterator iter_cast<std::basic_string<std::int16_t>::const_iterator> (std::uint16_t * it)
{
#if _MSC_VER
    return std::basic_string<std::int16_t>::const_iterator(reinterpret_cast<std::int16_t*>(it), nullptr);
#else
    return std::basic_string<std::int16_t>::const_iterator(reinterpret_cast<std::int16_t *>(it));
#endif
}

template <>
inline std::basic_string<std::uint16_t>::iterator iter_cast<std::basic_string<std::uint16_t>::iterator> (std::uint16_t * it)
{
#if _MSC_VER
    return std::basic_string<std::uint16_t>::iterator(it, nullptr);
#else
    return std::basic_string<std::uint16_t>::iterator(it);
#endif
}

template <>
inline std::basic_string<std::uint16_t>::const_iterator iter_cast<std::basic_string<std::uint16_t>::const_iterator> (std::uint16_t * it)
{
#if _MSC_VER
    return std::basic_string<std::uint16_t>::const_iterator(it, nullptr);
#else
    return std::basic_string<std::uint16_t>::const_iterator(it);
#endif
}
