@echo off
chcp 65001 >nul 2>&1
REM =============================================================================
REM 谦信CPP 第五集配套示例 - 编译运行脚本 (Windows)
REM 使用方法: 双击 build_run.bat 或在 cmd 中运行
REM 依赖: Visual Studio Developer Command Prompt 或 MinGW/MSYS2 环境
REM =============================================================================

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

REM --- 颜色 (Windows cmd 有限支持) ---
set "GREEN=[92m"
set "RED=[91m"
set "YELLOW=[93m"
set "CYAN=[96m"
set "BOLD=[1m"
set "NC=[0m"

call :color_echo "%CYAN%%BOLD%============================================%NC%"
call :color_echo "%CYAN%%BOLD%  谦信CPP · 第五集  编译运行%NC%"
call :color_echo "%CYAN%%BOLD%  类型萃取与 Concept%NC%"
call :color_echo "%CYAN%%BOLD%============================================%NC%"
echo.

REM --- 检测编译器 ---
call :detect_compiler
if errorlevel 1 (
    call :color_echo "%RED%错误: 未找到 C++ 编译器 (cl / g++ / clang++)%NC%"
    pause
    exit /b 1
)

REM --- 查找源代码文件 ---
set "EXAMPLES=01_type_traits_basics 02_type_transformations 03_custom_type_trait 04_requires_expression 05_standard_concepts 06_custom_concept 07_combination"

REM --- 清理旧产物 ---
call :color_echo "%YELLOW%--- 清理旧产物 ---%NC%"
for %%F in (%EXAMPLES%) do (
    if exist "%%F.exe" del /f /q "%%F.exe" >nul 2>&1
    if exist "%%F" (del /f /q "%%F" >nul 2>&1)
)
echo   清理完成
echo.

REM --- 编译 ---
call :color_echo "%YELLOW%--- 编译 (C++20) ---%NC%"
set "FAILED_COUNT=0"
set "PASSED_COUNT=0"

for %%F in (%EXAMPLES%) do (
    set "NAME=%%F"
    set "SRC=%%F.cpp"
    set "OUT=%%F.exe"

    call :color_echo_n "  编译 !NAME! ... "

    if not exist "!SRC!" (
        call :color_echo "%RED%✗ (源文件不存在: !SRC!)%NC%"
        set /a FAILED_COUNT+=1
        goto :next_compile
    )

    REM 尝试 MSVC (cl) 或 MinGW (g++)
    if defined MSVC_PRESENT (
        cl /std:c++20 /EHsc /W4 /O2 /Fe:!OUT! !SRC! >nul 2>&1
    ) else (
        g++ -std=c++20 -Wall -Wextra -O2 -o "!OUT!" "!SRC!" >nul 2>&1
    )

    if errorlevel 1 (
        call :color_echo "%RED%✗%NC%"
        echo.
        call :color_echo "    %YELLOW%编译出错详情:%NC%"
        if defined MSVC_PRESENT (
            cl /std:c++20 /EHsc /W4 /O2 /Fe:!OUT! !SRC! 2>&1 | findstr /n "." | findstr "^1:^2:^3:^4:^5:^6:^7:^8:^9:10:"
        ) else (
            g++ -std=c++20 -Wall -Wextra -O2 -o "!OUT!" "!SRC!" 2>&1 | head /n 10
        )
        echo.
        set /a FAILED_COUNT+=1
    ) else (
        call :color_echo "%GREEN%✓%NC%"
        set /a PASSED_COUNT+=1
    )

    :next_compile
)

echo.
if %FAILED_COUNT% gtr 0 (
    call :color_echo "%RED%编译失败 (%FAILED_COUNT% 个文件)%NC%"
) else (
    call :color_echo "%GREEN%全部编译成功%NC%"
)

REM --- 运行 ---
call :color_echo "%YELLOW%--- 运行 ---%NC%"
echo.

set "DESCRIPTIONS[01_type_traits_basics]=类型萃取基础"
set "DESCRIPTIONS[02_type_transformations]=类型变换"
set "DESCRIPTIONS[03_custom_type_trait]=手写类型萃取"
set "DESCRIPTIONS[04_requires_expression]=requires表达式"
set "DESCRIPTIONS[05_standard_concepts]=标准库Concept"
set "DESCRIPTIONS[06_custom_concept]=自定义Concept"
set "DESCRIPTIONS[07_combination]=组合拳实战"

for %%F in (%EXAMPLES%) do (
    if exist "%%F.exe" (
        call :color_echo "  %CYAN%[运行] %%F%NC%"
        echo   ----------------------------------------
        %%F.exe
        echo.
    ) else (
        call :color_echo "  %RED%[跳过] %%F.exe (未编译)%NC%"
    )
)

REM --- 总结 ---
echo.
call :color_echo "%CYAN%============================================%NC%"
call :color_echo "  编译: %GREEN%%PASSED_COUNT% 成功%NC% %FAILED_COUNT% 失败"
call :color_echo "%CYAN%============================================%NC%"
echo.
call :color_echo "  源码: %SCRIPT_DIR%*.cpp"
call :color_echo "  产物: %SCRIPT_DIR%01_type_traits_basics.exe ~ 07_combination.exe"
echo.
echo  按任意键退出 ...
pause >nul
exit /b 0


REM =============================================================================
REM :color_echo  - 打印带颜色的文本 (有限支持)
REM :color_echo_n - 不换行版本
REM =============================================================================
:color_echo
    echo %~1
    exit /b 0

:color_echo_n
    <nul set /p "=%~1"
    exit /b 0

REM =============================================================================
REM :detect_compiler - 检测 C++ 编译器
REM =============================================================================
:detect_compiler
    REM 先尝试 MSVC (Visual Studio)
    where cl >nul 2>&1
    if not errorlevel 1 (
        for /f "delims=" %%v in ('cl /? 2^|find "版本" 2^>nul') do set "MSVC_VERSION=%%v"
        call :color_echo "  检测到编译器: %GREEN%MSVC (cl)%NC% !MSVC_VERSION!"
        set "MSVC_PRESENT=1"
        exit /b 0
    )

    REM 尝试 MinGW / MSYS2 (g++)
    where g++ >nul 2>&1
    if not errorlevel 1 (
        for /f "delims=" %%v in ('g++ --version 2^|findstr "g++"') do set "GXX_VERSION=%%v"
        call :color_echo "  检测到编译器: %GREEN%MinGW (g++)%NC%"
        call :color_echo "  !GXX_VERSION!"
        exit /b 0
    )

    REM 尝试 Clang (clang++)
    where clang++ >nul 2>&1
    if not errorlevel 1 (
        for /f "delims=" %%v in ('clang++ --version 2^|findstr "clang"') do set "CLANG_VERSION=%%v"
        call :color_echo "  检测到编译器: %GREEN%Clang (clang++)%NC%"
        call :color_echo "  !CLANG_VERSION!"
        exit /b 0
    )

    exit /b 1
