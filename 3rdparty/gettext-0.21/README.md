1. Precompiled libraries: [gettext 0.21 and iconv 1.16 - Binaries for Windows](https://mlocati.github.io/articles/gettext-iconv-windows.html)
2. `libintl.dll` copied from `bin/libintl-8.dll`.
3. `libiconv-2.dll` copied from `bin/libiconv-2.dll`.
4. `libintl.h` created from `gettext-runtime/intl/libgnuintl.in.h` from `gettext-0.21` source library.
5. Generate `libintl.lib` and `libiconv-2.lib` using instructions from next section

# Generating `NAME.lib` from `NAME.dll` file

1. Generate `NAME.exports` file by command `dumpbin /EXPORTS NAME.dll > NAME.exports`.
2. Create `NAME.def` file by copying and editing data from `NAME.exports`. See example below.
```txt
LIBRARY libintl
EXPORTS
    bind_textdomain_codeset
    bindtextdomain
    dcgettext
    dcngettext
    dgettext
    dngettext
    gettext
```
3. Generate `NAME.lib` file for `x64` platform by command `lib /def:NAME.def /machine:x64 /out:NAME.lib`.
