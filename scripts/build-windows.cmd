::-------------------------------------------------------------------------------
:: Copyright (c) 2021-2024 Vladislav Trifochkin
::
:: Unified build script for Windows
::
:: Changelog:
::      2021.06.22 Initial version.
::      2021.11.25 Updated.
::      2022.07.06 Added CTEST_OPTIONS.
::      2022.07.14 Added BUILD_JOBS variable.
::                 Added BUILD_VERBOSITY variable.
::      2022.11.06 Added `Visual Studio 17 2022` generator.
::      2024.04.06 Refactored according to `build-linux.sh`.
::      2024.08.30 Fixed run tests after building them.
::      2024.09.01 Changed BUILD_DIR generation.
::      2024.09.02 Added BUILD_BASENAME for BUILD_DIR generation instead of BUILD_GENERATOR.
::      2024.09.03 Fixed BUILD_DIR_SUFFIX usage.
::                 Minimum supported MSVC version is Visual Studio 16 2019.
::-------------------------------------------------------------------------------

@echo off

:: ENABLEDELAYEDEXPANSION need to work such things properly:
::      set "CMAKE_OPTIONS="!CMAKE_OPTIONS! ..."
::      set "BUILD_DIR=!BUILD_DIR! ..." 

setlocal ENABLEDELAYEDEXPANSION

set "CMAKE_OPTIONS=!CMAKE_OPTIONS!"
set "CTEST_OPTIONS=!CTEST_OPTIONS!"

:: See https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/for
::for /F "delims=" %%i in (%filepath%) do set dirname=%%~dpi 
::for /F "delims=" %%i in (%filepath%) do set filename=%%~nxi
::for /F "delims=" %%i in (%filepath%) do set basename=%%~n
::for /F "delims=" %%i in (%filepath%) do set drive=%%~di
::for /F "delims=" %%i in (%filepath%) do set filepath=%%~pi
::for /F "delims=" %%i in (%filepath%) do set fileextension=%%~xi
::for /F "delims=" %%i in (%filepath%) do set filename=%%~ni

set SCRIPT_ABS_PATH=%~0
set PROJECT_FOLDER=%~dp0
pushd .
cd %PROJECT_FOLDER%\..
set PROJECT_FOLDER=%CD%
for %%F in (.) do set PROJECT_FOLDER_NAME=%%~nxF
popd

echo ** Script absolute path: %SCRIPT_ABS_PATH%
echo ** Project folder: %PROJECT_FOLDER%
echo ** Project folder name: %PROJECT_FOLDER_NAME%

if "%BUILD_GENERATOR%" == "" (
    @echo Detecting build generator ...

    @rem See https://en.wikipedia.org/wiki/Microsoft_Visual_Studio
    @rem and https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html
    if "%VisualStudioVersion%" == "" (
        @echo ERROR: script must be run inside Visual Studio Command Prompt >&2
        exit /b 1
    ) else if "%VisualStudioVersion:~0,3%" == "17." (
        set "BUILD_GENERATOR=Visual Studio 17 2022"
        set "BUILD_BASENAME=MSVC2022"
    ) else if "%VisualStudioVersion:~0,3%" == "16." (
        set "BUILD_GENERATOR=Visual Studio 16 2019"
        set "BUILD_BASENAME=MSVC2019"
@rem    ) else if "%VisualStudioVersion:~0,3%" == "15." (
@rem        set "BUILD_GENERATOR=Visual Studio 15 2017"
@rem        set "BUILD_BASENAME=MSVC2017"
@rem    ) else if "%VisualStudioVersion:~0,3%" == "14." (
@rem        set "BUILD_GENERATOR=Visual Studio 14 2015"
@rem    ) else if "%VisualStudioVersion:~0,3%" == "12." (
@rem        set "BUILD_GENERATOR=Visual Studio 12 2013"
@rem    ) else if "%VisualStudioVersion:~0,3%" == "11." (
@rem        set "BUILD_GENERATOR=Visual Studio 11 2012"
@rem    ) else if "%VisualStudioVersion:~0,3%" == "10." (
@rem        set "BUILD_GENERATOR=Visual Studio 10 2010"
@rem    ) else if "%VisualStudioVersion:~0,2%" == "9." (
@rem        set "BUILD_GENERATOR=Visual Studio 9 2008"
@rem    ) else if "%VisualStudioVersion:~0,2%" == "8." (
@rem        set "BUILD_GENERATOR=Visual Studio 8 2005"
    ) else (
        @echo "ERROR: unsupported build generator, set it manually without any guarantees" >&2
        exit /b 1
    )
)

set ARCH=%Platform%
if "%ARCH%" == "" set ARCH=x64

@echo Build generator: %BUILD_GENERATOR%
@echo Platform       : %ARCH%

if "%PROJECT_OPT_PREFIX%" == "" (
    if not exist "%PROJECT_FOLDER%\PREFIX" (
        @echo ERROR: PREFIX file must be in the project folder. >&2
        exit /b 1
    )

    for /f "delims=" %%X in (%PROJECT_FOLDER%\PREFIX) do set PROJECT_OPT_PREFIX=%%X
    rem set /P PROJECT_OPT_PREFIX=<"%PROJECT_FOLDER%\PREFIX"
)

echo ** Option prefix name: %PROJECT_OPT_PREFIX%

if "%PROJECT_NAME%" == "" set PROJECT_NAME=%PROJECT_FOLDER_NAME%
if "%BUILD_JOBS%" == "" set BUILD_JOBS=%NUMBER_OF_PROCESSORS%

:: Valid values for MSBuild verbosity option.
:: q[uiet], m[inimal], n[ormal] (default), d[etailed], and diag[nostic].

if "%BUILD_VERBOSITY%" == "" set BUILD_VERBOSITY=quiet

if /i "%BUILD_STRICT%" == "off" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%BUILD_STRICT=OFF"
) else (
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

if "%BUILD_TYPE%" == "" set "BUILD_TYPE=Debug"

set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DCMAKE_BUILD_TYPE=%BUILD_TYPE%"

if /i "%BUILD_TESTS%" == "off" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%BUILD_TESTS=OFF"
) else (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%BUILD_TESTS=ON"
    set "BUILD_TESTS=ON"
)

if /i "%CTEST_VERBOSE%" == "on" (
    set "CTEST_OPTIONS=--verbose !CTEST_OPTIONS!"
)

if /i "%BUILD_DEMO%" == "off" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%BUILD_DEMO=OFF"
) else (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%BUILD_DEMO=ON"
)

if /i "%ENABLE_COVERAGE%" == "on" (
    set "CMAKE_OPTIONS=!CMAKE_OPTIONS! -D%PROJECT_OPT_PREFIX%ENABLE_COVERAGE=ON"
)

if "%BUILD_DIR%" == "" (
    set "BUILD_DIR=builds\%PROJECT_OPT_PREFIX%%BUILD_BASENAME%"

    if not "%CXX_STANDARD%" == "" set "BUILD_DIR=!BUILD_DIR!.cxx%CXX_STANDARD%"
    if not "%ENABLE_COVERAGE%" == "" set "BUILD_DIR=!BUILD_DIR!.coverage"
)

if not "%BUILD_DIR_SUFFIX%" == "" set "BUILD_DIR=!BUILD_DIR!%BUILD_DIR_SUFFIX%"

::
:: We are inside source directory
:: 
if exist .git set IS_INSIDE_SRC_DIR=TRUE
if exist LICENSE set IS_INSIDE_SRC_DIR=TRUE
if exist PREFIX set IS_INSIDE_SRC_DIR=TRUE
if exist CMakeLists.txt set IS_INSIDE_SRC_DIR=TRUE

if "%IS_INSIDE_SRC_DIR%" == "TRUE" (
    if "%SOURCE_DIR%" == "" (set SOURCE_DIR=%cd%)
    set "BUILD_DIR=..\!BUILD_DIR!"
)

if "%SOURCE_DIR%" == "" (
    :: We are inside subdirectory (usually from scripts directory)
    if exist ..\.git set IS_INSIDE_SRC_SUBDIR=TRUE
    if exist ..\LICENSE set IS_INSIDE_SRC_SUBDIR=TRUE
    if exist ..\PREFIX set IS_INSIDE_SRC_SUBDIR=TRUE
    if exist ..\CMakeLists.txt set IS_INSIDE_SRC_SUBDIR=TRUE

    if "!IS_INSIDE_SRC_SUBDIR!" == "TRUE" (
        set "SOURCE_DIR=%cd%\.."
        set "BUILD_DIR=..\..\!BUILD_DIR!" 
    ) else (
        echo ERROR: SOURCE_DIR must be specified >&2
        exit /b 1
    )
)

echo ** SOURCE_DIR="%SOURCE_DIR%"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

if %ERRORLEVEL% == 0 (
    cd "%BUILD_DIR%" ^
        && cmake -G "%BUILD_GENERATOR%" -A %ARCH% %CMAKE_OPTIONS% "%SOURCE_DIR%" ^
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
