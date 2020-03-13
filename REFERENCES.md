## References

### C++

1. [Prototypes of member access operators](http://en.cppreference.com/w/cpp/language/operator_member_access)
2. [C++17 features](http://www.bfilipek.com/2017/07/cpp17-in-details-attributes.html)
3. [How I Declare My class And Why](http://howardhinnant.github.io/classdecl.html)
4. [Changes between C++11 and C++14](https://isocpp.org/files/papers/p1319r0.html)
5. [Changes between C++14 and C++17 DIS](https://isocpp.org/files/papers/p0636r0.html)

### Containers

1. [A faster std::vector](http://andreoffringa.org/?q=uvector)
2. [Immutable containers](https://sinusoid.es/immer/containers.html)

### Templates

1. [Partial Specialization and Tags](https://vector-of-bool.github.io/2017/08/12/partial-specializations.html)

### Idioms

1. [CRTP - Curiously recurring template pattern](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
2. [Barton–Nackman trick](https://en.wikipedia.org/wiki/Barton%E2%80%93Nackman_trick)

### std::chrono

1. [Compose function to measure function call times](https://a4z.bitbucket.io/blog/2017/07/11/Compose-function-to-measure-function-call-times.html)

### std::iterator

1. [Iterators are also part of the C++ zero cost abstractions](https://blog.galowicz.de/2017/07/02/order2_iterator/)

### Smart pointer (std::shared_ptr, std::unique_ptr)

1. [Using C++11’s Smart Pointers](http://umich.edu/~eecs381/handouts/C++11_smart_ptrs.pdf)

### rvalue, value categories

1. [C++ Rvalue References Explained](http://thbecker.net/articles/rvalue_references/section_01.html)
2. [Value Categories in C++17](https://medium.com/@barryrevzin/value-categories-in-c-17-f56ae54bccbe)
3. [Understanding the meaning of lvalues and rvalues in C++](https://www.internalpointers.com/post/understanding-meaning-lvalues-and-rvalues-c)
4. [Pass by Reference vs Pass by Value in C++](https://codeofconnor.wordpress.com/2017/09/12/pass-by-reference-vs-pass-by-value-in-cpp/)
5. [One-Shot Learning of C++ r-value, &&, and Move](https://hackernoon.com/one-shot-learning-of-c-r-value-and-move-27e5d6bcec3b)

### Return Value Optimization (RVO), Named RVO (NRVO) and Copy-Elision

1. [Return Value Optimization](http://shaharmike.com/cpp/rvo/)

### STL

1. [STL & Generic Programming: Writing Your Own Iterators](http://www.drdobbs.com/stl-generic-programming-writing-your-ow/184401417)

### Exceptions, error handling

1. [Exception handling in constructors](https://medium.com/cpp-station/exception-handling-in-constructors-26bf4c811b46)
2. [Your own error code](https://akrzemi1.wordpress.com/2017/07/12/your-own-error-code/)
3. [Your own error condition](https://akrzemi1.wordpress.com/2017/08/12/your-own-error-condition/)
4. [Using error codes effectively](https://akrzemi1.wordpress.com/2017/09/04/using-error-codes-effectively/)

### 32/64-bits

1. [Перенос Linux-приложений на 64-разрядные системы](http://www.ibm.com/developerworks/ru/library/l-port64/)
2. [64-Bit Transition Guide for Cocoa Touch. Major 64-Bit Changes](https://developer.apple.com/library/ios/documentation/General/Conceptual/CocoaTouch64BitGuide/Major64-BitChanges/Major64-BitChanges.html)
3. [Модель данных](http://www.viva64.com/ru/t/0012/)

### Unicode
1. [UTF-8, UTF-16, UTF-32 & BOM](http://unicode.org/faq/utf_bom.html)
2. [RFC-2781. UTF-16, an encoding of ISO 10646](https://www.ietf.org/rfc/rfc2781.txt)
3. [UTF-8 Everywhere](http://utf8everywhere.org/)

### Compilers

1. [Predefined macros. Visual Studio 2010 ](https://msdn.microsoft.com/ru-ru/library/b0084kay%28v=vs.100%29.aspx)

### Generic<Programming>, Alignment

1. [Generic<Programming>: Typelists and Applications](http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2002/alexandr/alexandr.htm)
2. [Generic<Programming>: Discriminated Unions (I)](http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2004/alexandr/alexandr.htm)
3. [Generic<Programming>: Discriminated Unions (II)](http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2006/alexandr/alexandr.htm)
4. [Determining the alignment of C/C++ structures in relation to its members](http://stackoverflow.com/questions/364483/determining-the-alignment-of-c-c-structures-in-relation-to-its-members)
5. [Implementing alignof](http://www.wambold.com/Martin/writings/alignof.html)
6. [Style Case Study #3: Construction Unions (Herb Sutter)](http://www.gotw.ca/gotw/085.htm)
7. [std::vector of Aligned Elements](http://ofekshilon.com/2010/05/05/stdvector-of-aligned-elements/)
8. [Особенности выравнивания данных в структурах](http://precious-cpp.blogspot.ru/2010/09/blog-post.html)

### Floating point parsing / arithmetic

1. [How to Define NaN (Not-a-Number) on Windows](http://tdistler.com/2011/03/24/how-to-define-nan-not-a-number-on-windows)
2. [How to use nan and inf in C?](http://stackoverflow.com/questions/1923837/how-to-use-nan-and-inf-in-c)
3. [IEEE 754 - Standard binary arithmetic float](http://www.softelectro.ru/teoriy_en.html)
4. [Comparing Floating Point Numbers, 2012 Edition](https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/)
5. [How do I handle byte order differences when reading/writing floating-point types in C?](https://stackoverflow.com/questions/14954088/how-do-i-handle-byte-order-differences-when-reading-writing-floating-point-types/14955046#14955046)
6. [Fast float parsing in practice](https://lemire.me/blog/2020/03/10/fast-float-parsing-in-practice/) ([Reddit](https://www.reddit.com/r/cpp/comments/fgcufa/fast_float_parsing_in_practice/))

### Endianness

1. [Endianness](http://en.wikipedia.org/wiki/Endianness#Floating-point_and_endianness)
2. [Understanding Big and Little Endian Byte Order](http://betterexplained.com/articles/understanding-big-and-little-endian-byte-order/)
3. [Writing endian-independent code in C](http://www.ibm.com/developerworks/aix/library/au-endianc/)
4. [Endianness: Big and Little Endian Byte Order (with list of Processor Endianness)](http://www.yolinux.com/TUTORIALS/Endian-Byte-Order.html)
5. Something about endianness at ["Serialization—How to Pack Data"](http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#serialization)

### Pimpl

1. [The Fast Pimpl Idiom](http://www.gotw.ca/gotw/028.htm)
2. [Why every C++ developer should know about the pimpl idiom pattern](https://tonka2013.wordpress.com/2013/08/31/why-every-c-developer-should-know-about-the-pimpl-idiom-pattern/)

### SFINAE

1. [Compiler Tricks: SFINAE and nice messages](https://gracicot.github.io/tricks/2017/07/01/deleted-function-diagnostic.html)

### Functor

1. [Functors in C++ - Part I](https://mayankj08.github.io/2017/07/02/Functors-In-C++/)
2. [Functors in C++ - Part II](https://mayankj08.github.io/2017/07/06/Functors-In-C++-II/)

### Undefined Behavior (UB)

1. [Undefined Behavior in 2017](https://blog.regehr.org/archives/1520)

### Logger libraries

1. [Сравнение библиотек логирования](https://habrahabr.ru/post/313686/)
2. [blackhole - Yet another logging library](https://github.com/3Hren/blackhole)
3. [plog -  Portable, simple and extensible C++ logging library](https://github.com/SergiusTheBest/plog)

### Data structures

1. [C++ implementation of a fast **hash map** and **hash set** using robin hood hashing](https://github.com/Tessil/robin-map)

### Database, ORM

1. [SQLite ORM](https://github.com/fnc12/sqlite_orm)
2. [SQLAPI++ Library](http://www.sqlapi.com/)
3. [SOCI - The C++ Database Access Library](https://github.com/SOCI/soci)
4. [Wt::Dbo](https://www.webtoolkit.eu/wt/doc/tutorial/dbo.html)
5. [sqlite3](https://www.sqlite.org/docs.html)

### CLI

1. [Docopt](http://docopt.org/)
2. [Optional](https://mulholland.xyz/docs/optional/index.html)
3. [cxxopts - Lightweight C++ command line option parser](https://github.com/jarro2783/cxxopts)
4. [argagg - A simple C++11 command line argument parser](https://github.com/vietjtnguyen/argagg)
5. [Boost.Program_options](https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html)
6. [Clara - A simple to use, composable, command line parser for C++ 11 and beyond](https://github.com/catchorg/Clara)

### File system

1. [Filesystem library (cppreference.com)](http://en.cppreference.com/w/cpp/experimental/fs)

### Regex

1. [Regular expressions library (cppreference.com)](http://en.cppreference.com/w/cpp/regex)
2. [Regex (cplusplus.com)](http://www.cplusplus.com/reference/regex/)

### Reflective enums

1. [wise_enum](https://github.com/quicknir/wise_enum)
2. [Better Enums](https://github.com/aantron/better-enums)
3. [Meta Enum - Static reflection on enums in C++17](https://github.com/therocode/meta_enum)

### JSON

1. [JSON-RPC 2.0 Specification](https://www.jsonrpc.org/specification)

### HTTP / RTPS

1. [Hypertext Transfer Protocol -- HTTP/1.0](https://tools.ietf.org/html/rfc1945)
2. [Hypertext Transfer Protocol -- HTTP/1.1](https://tools.ietf.org/html/rfc2616)
3. [Real Time Streaming Protocol (RTSP)](https://tools.ietf.org/html/rfc2326)
4. [Real-Time Streaming Protocol Version 2.0](https://tools.ietf.org/html/rfc7826)
5. [HTTP Made Really Easy](https://www.jmarshall.com/easy/http/)
6. [PicoHTTPParser](https://github.com/h2o/picohttpparser)
7. [nghttp2 - HTTP/2 C Library](https://github.com/nghttp2/nghttp2)
8. [The C++ Network Library Project](https://cpp-netlib.org)
9. [Microsoft C++ REST SDK](https://github.com/Microsoft/cpprestsdk)
10.[Yet Another HTTP Library](https://github.com/cmouse/yahttp)

### Software development principles

1. [DRY](https://en.wikipedia.org/wiki/Don%27t_repeat_yourself) (Don't repeat yourself)
2. [SOLID (1)](https://en.wikipedia.org/wiki/SOLID)
3. [SOLID (2)](https://blog.bitsrc.io/solid-principles-every-developer-should-know-b3bfa96bb688)
4. [FIRST](https://addyosmani.com/first/)

### Timer

1. [Hashed and Hierarchical Timing Wheels: Data Structures for the Efficient Implementation of a Timer Facility](http://www.cs.columbia.edu/~nahum/w6998/papers/sosp87-timing-wheels.pdf)

### Misc

1. [Embedding resource files in a C++ program binary on Linux/*NIX](http://www.atwillys.de/content/cc/embedding-resource-files-in-a-c-plus-plus-program-binary-on-linux-unix/?lang=en)
2. [Embedding a File in an Executable, aka Hello World, Version 5967](http://www.linuxjournal.com/content/embedding-file-executable-aka-hello-world-version-5967)

### Memory allocators

1. [tcmalloc](https://github.com/gperftools/gperftools)
2. [jemalloc](http://jemalloc.net)
3. [mimalloc](https://github.com/microsoft/mimalloc)
