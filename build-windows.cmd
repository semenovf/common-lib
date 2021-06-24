::-------------------------------------------------------------------------------
:: Copyright (c) 2021 Vladislav Trifochkin
::
:: Unified build script for Windows
::
:: Changelog:
::      2021.06.22 Initial version
::-------------------------------------------------------------------------------

@echo off

:: ENABLEDELAYEDEXPANSION need to work such things properly:
::      set "CMAKE_OPTIONS="!CMAKE_OPTIONS! ..."
::      set "BUILD_DIR=!BUILD_DIR! ..." 

setlocal ENABLEDELAYEDEXPANSION

if "%BUILD_GENERATOR%" == "" (
    @echo Detecting build generator ...

    @rem See https://en.wikipedia.org/wiki/Microsoft_Visual_Studio
    @rem and https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html
    if "%VisualStudioVersion%" == "" (
        @echo ERROR: script must be run inside Visual Studio Command Prompt >&2
        exit /b 1
    ) else if "%VisualStudioVersion:~0,3%" == "16." (
        set "BUILD_GENERATOR=Visual Studio 16 2019"
    ) else if "%VisualStudioVersion:~0,3%" == "15." (
        set "BUILD_GENERATOR=Visual Studio 15 2017"
    ) else if "%VisualStudioVersion:~0,3%" == "14." (
        set "BUILD_GENERATOR=Visual Studio 14 2015"
    ) else if "%VisualStudioVersion:~0,3%" == "12." (
        set "BUILD_GENERATOR=Visual Studio 12 2013"
    ) else if "%VisualStudioVersion:~0,3%" == "11." (
        set "BUILD_GENERATOR=Visual Studio 11 2012"
    ) else if "%VisualStudioVersion:~0,3%" == "10." (
        set "BUILD_GENERATOR=Visual Studio 10 2010"
    ) else if "%VisualStudioVersion:~0,2%" == "9." (
        set "BUILD_GENERATOR=Visual Studio 9 2008"
    ) else if "%VisualStudioVersion:~0,2%" == "8." (
        set "BUILD_GENERATOR=Visual Studio 8 2005"
    ) else (
        @echo "ERROR: unable to detect build generator, set it manually" >&2
        exit /b 1
    )
)

@echo Build generator: %BUILD_GENERATOR%

if /i not "%CMAKE_VERBOSE_MAKEFILE%" == "off" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DCMAKE_VERBOSE_MAKEFILE=ON"
)

if /i "%BUILD_STRICT%" == "on" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBUILD_STRICT=ON"
)

if /i "%BUILD_DEMO%" == "on" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBUILD_DEMO=ON"
)

if not "%CXX_STANDARD%" == "" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DCMAKE_CXX_STANDARD=%CXX_STANDARD%"
)

if not "%C_COMPILER%" == "" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DCMAKE_C_COMPILER=%C_COMPILER%"
)

if not "%CXX_COMPILER%" == "" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DCMAKE_CXX_COMPILER=%CXX_COMPILER%"
)

if "%BUILD_TYPE%" == "" (
    set "BUILD_TYPE=Debug"
)

set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DCMAKE_BUILD_TYPE=%BUILD_TYPE%"

if not "%ENABLE_COVERAGE%" == "" (
    if /i "%ENABLE_COVERAGE%" == "on" (
        set "ENABLE_COVERAGE=ON"
    ) else (
        set ENABLE_COVERAGE=
    )
)

if not "%ENABLE_COVERAGE%" == "" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DENABLE_COVERAGE=%ENABLE_COVERAGE%"
)

if "%BUILD_DIR%" == "" (
    set "BUILD_DIR=builds\%BUILD_GENERATOR%"

    if not "%CXX_STANDARD%" == "" (
        set "BUILD_DIR=!BUILD_DIR!.%CXX_STANDARD%" 
    )

    if not "%ENABLE_COVERAGE%" == "" (
        set "BUILD_DIR=!BUILD_DIR!.coverage" 
    )
)

::
:: We are inside source directory
:: 
if exist .git (
    if "%ENABLE_COVERAGE%" == "" (
        set "SOURCE_DIR=%cd%"
    )
    cd ..
)

if "%SOURCE_DIR%" == "" (
    if exist src\.git (
        set "SOURCE_DIR=%cd%\src"
    ) else (
        echo ERROR: SOURCE_DIR must be specified >&2
        exit /b 1
    )
)

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

cd "%BUILD_DIR%" ^
    && cmake -G "%BUILD_GENERATOR%" %CMAKE_OPTIONS% "%SOURCE_DIR%" ^
    && cmake --build .

if "%BUILD_TESTS%" == "ON" ctest
if "%ENABLE_COVERAGE%" == "ON" cmake --build . --target Coverage

endlocal

exit /b 0