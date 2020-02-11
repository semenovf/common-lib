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
    char const *    filename;
    unsigned char * text;
    unsigned int    len;
    unsigned int    nchars;
};

static test_data data[] = {
      { "cyrillic"  , "unicode/utf8/data/cyrillic.txt"  , cyrillic_txt  , cyrillic_txt_len   ,   66 }
    , { "gothic"    , "unicode/utf8/data/gothic.txt"    , gothic_txt    , gothic_txt_len     ,   83 }
    , { "greek"     , "unicode/utf8/data/greek.txt"     , greek_txt     , greek_txt_len      ,  204 }
    , { "mideng"    , "unicode/utf8/data/mideng.txt"    , mideng_txt    , mideng_txt_len     ,  272 }
    , { "midger"    , "unicode/utf8/data/midger.txt"    , midger_txt    , midger_txt_len     ,  353 }
    , { "multilang" , "unicode/utf8/data/multilang.txt" , multilang_txt , multilang_txt_len  , 9175 }
    , { "rune"      , "unicode/utf8/data/rune.txt"      , rune_txt      , rune_txt_len       ,  145 }
    , { "vietnamese", "unicode/utf8/data/vietnamese.txt", vietnamese_txt, vietnamese_txt_len ,   43 }
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
