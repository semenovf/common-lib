#pragma once

extern unsigned char cyrillic_txt[];
extern unsigned int  cyrillic_txt_len;
extern unsigned char greek_txt[];
extern unsigned int  greek_txt_len;
extern unsigned char gothic_txt[];
extern unsigned int  gothic_txt_len;
extern unsigned char mideng_txt[];
extern unsigned int  mideng_txt_len;
extern unsigned char midger_txt[];
extern unsigned int  midger_txt_len;
extern unsigned char multilang_txt[];
extern unsigned int  multilang_txt_len;
extern unsigned char rune_txt[];
extern unsigned int  rune_txt_len;
extern unsigned char vietnamese_txt[];
extern unsigned int  vietnamese_txt_len;

struct test_data {
    char const *    name;
    unsigned char * text;
    unsigned int    len;
    unsigned int    nchars;
};

static test_data data[] = {
      { "cyrillic"  , cyrillic_txt  , cyrillic_txt_len   ,   66 }
    , { "gothic"    , gothic_txt    , gothic_txt_len     ,   83 }
    , { "greek"     , greek_txt     , greek_txt_len      ,  204 }
    , { "mideng"    , mideng_txt    , mideng_txt_len     ,  272 }
    , { "midger"    , midger_txt    , midger_txt_len     ,  353 }
    , { "multilang" , multilang_txt , multilang_txt_len  , 9175 }
    , { "rune"      , rune_txt      , rune_txt_len       ,  145 }
    , { "vietnamese", vietnamese_txt, vietnamese_txt_len ,   43 }
};

template <typename Iterator>
Iterator iter_cast (unsigned char *);

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

template <>
inline std::string::iterator iter_cast<std::string::iterator> (unsigned char * it)
{
    return std::string::iterator(reinterpret_cast<char *>(it));
}

template <>
inline std::string::const_iterator iter_cast<std::string::const_iterator> (unsigned char * it)
{
    return std::string::const_iterator(reinterpret_cast<char *>(it));
}
