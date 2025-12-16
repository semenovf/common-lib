////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.12.02 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "getenv.hpp"
#include "string_view.hpp"
#include "utility.hpp"
#include <cstdint>
#include <cstdio>
#include <string>

#if _MSC_VER
#   include "windows.hpp"
#endif

PFS__NAMESPACE_BEGIN
namespace term {

inline bool color_term ()
{
#if _MSC_VER
    return false; // Unsupported yet
#else
    auto term_env = pfs::getenv("TERM");
    auto success = term_env && (term_env->find("256") != std::string::npos);

    if (!success) {
        auto color_term_env = pfs::getenv("COLORTERM");
        success = color_term_env && (color_term_env->find("truecolor") != std::string::npos);
    }

    return success;
#endif
}

class colorizer final
{
    enum class color: std::uint8_t { black, red, green, yellow, blue, purple, cyan, white };
    enum font_style: std::uint8_t {
          regular_style = 0
        , bold_style = 1 << 1
        , italic_style = 1 << 2
        , underline_style = 1 << 3
        , strike_style = 1 << 4
    };

#if _MSC_VER
    // Unsupported yet
#else
    static constexpr int base_regular_color = 30;
    static constexpr int base_bright_color = 90;
    static constexpr char regular_flag   = '0';
    static constexpr char bold_flag      = '1';
    static constexpr char italic_flag    = '3';
    static constexpr char underline_flag = '4';
    static constexpr char strike_flag    = '9';
#endif

private:
    bool _is_color_term {false};
    color _color {color::black};
    std::uint8_t _font_style {font_style::regular_style};

#if _MSC_VER
#else
    int _color_base{base_regular_color};
#endif

public:
    colorizer (bool is_color_term = color_term())
        : _is_color_term(is_color_term)
    {}

public:
    colorizer & black ()  { return reset(color::black); }
    colorizer & red ()    { return reset(color::red); }
    colorizer & green ()  { return reset(color::green); }
    colorizer & yellow () { return reset(color::yellow); }
    colorizer & blue ()   { return reset(color::blue); }
    colorizer & purple () { return reset(color::purple); }
    colorizer & cyan ()   { return reset(color::cyan); }
    colorizer & white ()  { return reset(color::white); }

    /**
     * Set bright (high intensive) color.
     */
    colorizer & bright ()
    {
#if _MSC_VER
#else
        _color_base = base_bright_color;
#endif
        return *this;
    }

    /**
     * Sets regular font style.
     */
    colorizer & regular ()
    {
        _font_style = font_style::regular_style;
        return *this;
    }

    /**
     * Sets bold font style.
     */
    colorizer & bold ()
    {
        _font_style |= font_style::bold_style;
        return *this;
    }

    /**
     * Sets italic font style.
     */
    colorizer & italic ()
    {
        _font_style |= font_style::italic_style;
        return *this;
    }

    colorizer & underline ()
    {
        _font_style |= font_style::underline_style;
        return *this;
    }

    colorizer & strike ()
    {
        _font_style |= font_style::strike_style;
        return *this;
    }

    std::string style () const
    {
#if _MSC_VER
        // Unsupported yet
        return std::string{};
#else
        if (!_is_color_term)
            return std::string{};

        std::string result;

        int i = 0;
        char prefix[7 + 4 + 4 + 4 + 1];

        // 7 characters
        prefix[i++] = '\x1b';
        prefix[i++] = '[';
        prefix[i++] = '0';
        prefix[i++] = ';';
        prefix[i++] = '0' + (_color_base + to_underlying(_color)) / 10;
        prefix[i++] = '0' + (_color_base + to_underlying(_color)) % 10;
        prefix[i++] = 'm';

        // 4 characters
        if (_font_style & bold_style) {
            prefix[i++] = '\x1b';
            prefix[i++] = '[';
            prefix[i++] = bold_flag;
            prefix[i++] = 'm';
        }

        // 4 characters
        if (_font_style & italic_style) {
            prefix[i++] = '\x1b';
            prefix[i++] = '[';
            prefix[i++] = italic_flag;
            prefix[i++] = 'm';
        }

        // 4 characters
        if (_font_style & strike_style) {
            prefix[i++] = '\x1b';
            prefix[i++] = '[';
            prefix[i++] = strike_flag;
            prefix[i++] = 'm';
        } else if (_font_style & underline_style) {
            prefix[i++] = '\x1b';
            prefix[i++] = '[';
            prefix[i++] = underline_flag;
            prefix[i++] = 'm';
        }

        // 1 character
        prefix[i++] = '\x0';

        result.append(prefix);
        return result;
#endif
    }

    /**
     * Returns text prefixed with an ANSI escape sequence.
     */
    std::string text (string_view s) const
    {
        std::string result {style()};
        result.append(s.data(), s.size());
        return result;
    }

    /**
     * Returns text prefixed with an ANSI escape sequence and suffixed with a reset sequence.
     */
    std::string textr (string_view s) const
    {
        std::string result {style()};
        result.append(s.data(), s.size());

#if _MSC_VER
#else
        if (_is_color_term)
            result.append("\x1b[0m", 4); // Reset sequence
#endif
        return result;
    }

public:
    /**
     * Resets all attributes.
     */
    static void reset ()
    {
#if _MSC_VER
#else
        std::printf("\x1b[0m");
#endif
    }

private:
    colorizer & reset (color c)
    {
#if _MSC_VER
#else
        _color_base = base_regular_color;
#endif
        _color = c;
        return regular();
    }
};

} //namespace term
PFS__NAMESPACE_END
