#!/bin/bash
# 谦信CPP 第五集配套示例 - 编译运行脚本

cd "$(dirname "$0")"

echo "=== 编译 ==="
cmake -B build -DCMAKE_CXX_STANDARD=20
cmake --build build

echo ""
echo "=== 运行 ==="
for exe in build/01_type_traits_basics build/02_type_transformations \
           build/03_custom_type_trait build/04_requires_expression \
           build/05_standard_concepts build/06_custom_concept \
           build/07_combination; do
    echo "--- $(basename $exe) ---"
    "$exe"
done
