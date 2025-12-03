////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Vladislav Trifochkin
//
// This file is part of `common-lib`.
//
// Changelog:
//      2025.12.02 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/term.hpp"
#include "pfs/fmt.hpp"

void tabulize (std::string const & name, pfs::term::colorizer colrzr)
{
    // pfs::term::colorizer::reset();
    // colrzr.style() is a hack to correctly output text with the alignment using fmt::print()
    // Otherwise, fmt takes control characters into account when calculating the length of the string.
    fmt::println("{}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6}"
        , pfs::term::colorizer{colrzr}.regular().style(), name
        , pfs::term::colorizer{colrzr}.regular().bright().style(), name
        , pfs::term::colorizer{colrzr}.bold().style(), name
        , pfs::term::colorizer{colrzr}.bold().bright().style(), name
        , pfs::term::colorizer{colrzr}.italic().style(), name
        , pfs::term::colorizer{colrzr}.italic().bright().style(), name
        , pfs::term::colorizer{colrzr}.italic().bold().style(), name
        , pfs::term::colorizer{colrzr}.italic().bold().bright().style(), name
    );
}

void tabulize_underline (std::string const & name, pfs::term::colorizer colrzr)
{
    fmt::println("{}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6}"
        , pfs::term::colorizer{colrzr}.regular().underline().style(), name
        , pfs::term::colorizer{colrzr}.regular().bright().underline().style(), name
        , pfs::term::colorizer{colrzr}.bold().underline().style(), name
        , pfs::term::colorizer{colrzr}.bold().bright().underline().style(), name
        , pfs::term::colorizer{colrzr}.italic().underline().style(), name
        , pfs::term::colorizer{colrzr}.italic().bright().underline().style(), name
        , pfs::term::colorizer{colrzr}.italic().bold().underline().style(), name
        , pfs::term::colorizer{colrzr}.italic().bold().bright().underline().style(), name
    );
}

void tabulize_strike (std::string const & name, pfs::term::colorizer colrzr)
{
    fmt::println("{}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6} {}{:6}"
        , pfs::term::colorizer{colrzr}.regular().strike().style(), name
        , pfs::term::colorizer{colrzr}.regular().bright().strike().style(), name
        , pfs::term::colorizer{colrzr}.bold().strike().style(), name
        , pfs::term::colorizer{colrzr}.bold().bright().strike().style(), name
        , pfs::term::colorizer{colrzr}.italic().strike().style(), name
        , pfs::term::colorizer{colrzr}.italic().bright().strike().style(), name
        , pfs::term::colorizer{colrzr}.italic().bold().strike().style(), name
        , pfs::term::colorizer{colrzr}.italic().bold().bright().strike().style(), name
    );
}

int main ()
{
    using colrzr_t = pfs::term::colorizer;

    if (pfs::term::color_term()) {
        fmt::println("{} {} {} {}\n"
            , colrzr_t{}.cyan().text("The")
            , colrzr_t{}.green().text("terminal")
            , colrzr_t{}.blue().text("supports")
            , colrzr_t{}.purple().text("colors"));

        pfs::term::colorizer::reset();
    } else {
        fmt::println("The terminal does not support colors\n");
    }

    fmt::println("italic bold bright --------------------------------+");
    fmt::println("italic bold --------------------------------+      |");
    fmt::println("italic bright -----------------------+      |      |");
    fmt::println("italic -----------------------+      |      |      |");
    fmt::println("bold bright -----------+      |      |      |      |");
    fmt::println("bold -----------+      |      |      |      |      |");
    fmt::println("regular bright  |      |      |      |      |      |");
    fmt::println("regular  |      |      |      |      |      |      |");
    fmt::println("  |      |      |      |      |      |      |      |");
    fmt::println("  v      v      v      v      v      v      v      v");
    tabulize("black" , colrzr_t{}.black());
    tabulize("red"   , colrzr_t{}.red());
    tabulize("green" , colrzr_t{}.green());
    tabulize("yellow", colrzr_t{}.yellow());
    tabulize("blue"  , colrzr_t{}.blue());
    tabulize("purple", colrzr_t{}.purple());
    tabulize("cyan"  , colrzr_t{}.cyan());
    tabulize("white" , colrzr_t{}.white());

    tabulize_underline("black" , colrzr_t{}.black());
    tabulize_underline("red"   , colrzr_t{}.red());
    tabulize_underline("green" , colrzr_t{}.green());
    tabulize_underline("yellow", colrzr_t{}.yellow());
    tabulize_underline("blue"  , colrzr_t{}.blue());
    tabulize_underline("purple", colrzr_t{}.purple());
    tabulize_underline("cyan"  , colrzr_t{}.cyan());
    tabulize_underline("white" , colrzr_t{}.white());

    tabulize_strike("black" , colrzr_t{}.black());
    tabulize_strike("red"   , colrzr_t{}.red());
    tabulize_strike("green" , colrzr_t{}.green());
    tabulize_strike("yellow", colrzr_t{}.yellow());
    tabulize_strike("blue"  , colrzr_t{}.blue());
    tabulize_strike("purple", colrzr_t{}.purple());
    tabulize_strike("cyan"  , colrzr_t{}.cyan());
    tabulize_strike("white" , colrzr_t{}.white());

    pfs::term::colorizer::reset();

    return 0;
}
