::-------------------------------------------------------------------------------
:: Copyright (c) 2021,2022 Vladislav Trifochkin
::
:: Unified build script for Windows
::
:: Changelog:
::      2021.06.22 Initial version.
::      2021.11.25 Updated.
::      2022.07.06 Added CTEST_OPTIONS.
::      2022.07.14 Added BUILD_JOBS variable.
::                 Added BUILD_VERBOSITY variable.
::-------------------------------------------------------------------------------

@echo off

:: ENABLEDELAYEDEXPANSION need to work such things properly:
::      set "CMAKE_OPTIONS="!CMAKE_OPTIONS! ..."
::      set "BUILD_DIR=!BUILD_DIR! ..." 

setlocal ENABLEDELAYEDEXPANSION

set "CMAKE_OPTIONS=!CMAKE_OPTIONS!"
set "CTEST_OPTIONS=!CTEST_OPTIONS!"

if "%PROJECT_OPT_PREFIX%" == "" (
    @echo ERROR: PROJECT_OPT_PREFIX is mandatory >&2
    exit /b 1
)

if "%BUILD_JOBS%" == "" set BUILD_JOBS = %NUMBER_OF_PROCESSORS%

:: Valid values for MSBuild verbosity option.
:: q[uiet], m[inimal], n[ormal] (default), d[etailed], and diag[nostic].
if "%BUILD_VERBOSITY%" == "" set BUILD_VERBOSITY=minimal

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

if /i "%CTEST_VERBOSE%" == "on" (
    set "CTEST_OPTIONS=--verbose !CTEST_OPTIONS!"
)

if /i "%BUILD_STRICT%" == "on" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%BUILD_STRICT=ON"
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

if /i "%BUILD_TESTS%" == "on" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%BUILD_TESTS=ON"
)

if /i "%BUILD_DEMO%" == "on" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%BUILD_DEMO=ON"
)

if /i "%ENABLE_COVERAGE%" == "on" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%ENABLE_COVERAGE=ON"
)

if "%BUILD_DIR%" == "" (
    set "BUILD_DIR=builds\%PROJECT_OPT_PREFIX%%BUILD_GENERATOR%"

    if not "%CXX_STANDARD%" == "" (
        set "BUILD_DIR=!BUILD_DIR!.cxx%CXX_STANDARD%" 
    )

    if not "%ENABLE_COVERAGE%" == "" (
        set "BUILD_DIR=!BUILD_DIR!.coverage" 
    )
)

if not "%BUILD_DIR_SUFFIX%" == "" (
    set "BUILD_DIR=!BUILD_DIR!%BUILD_DIR_SUFFIX%" 
)

::
:: We are inside source directory
:: 
if exist .git (
    if "%ENABLE_COVERAGE%" == "" (
        set "SOURCE_DIR=%cd%"
    )
    set "BUILD_DIR=..\!BUILD_DIR!" 
)

if "%SOURCE_DIR%" == "" (
    :: We are inside subdirectory (usually from scripts directory)
    if exist ..\.git (
        set "SOURCE_DIR=%cd%\.."
        set "BUILD_DIR=..\..\!BUILD_DIR!" 
    ) else (
        echo ERROR: SOURCE_DIR must be specified >&2
        exit /b 1
    )
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

if %ERRORLEVEL% == 0 (
    cd "%BUILD_DIR%" ^
        && cmake -G "%BUILD_GENERATOR%" %CMAKE_OPTIONS% "%SOURCE_DIR%" ^
        && cmake --build . -j %BUILD_JOBS% -- -verbosity:%BUILD_VERBOSITY%
)

if %ERRORLEVEL% == 0 (
    if "%BUILD_TESTS%" == "ON" ctest %CTEST_OPTIONS% -C %BUILD_TYPE%
)

if %ERRORLEVEL% == 0 (
    if "%ENABLE_COVERAGE%" == "ON" (
	cmake --build . --target Coverage -j %BUILD_JOBS% -- -verbosity:%BUILD_VERBOSITY%
    )
)

endlocal

exit /b 0
