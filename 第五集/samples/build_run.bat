@echo off
chcp 65001 >nul

cd /d "%~dp0"

echo === 编译 ===
cmake -B build -DCMAKE_CXX_STANDARD=20
cmake --build build

echo.
echo === 运行 ===
for %%F in (01_type_traits_basics 02_type_transformations 03_custom_type_trait 04_requires_expression 05_standard_concepts 06_custom_concept 07_combination) do (
    echo --- %%F ---
    build\%%F.exe
)
pause
