#!/usr/bin/env bash
# =============================================================================
# 谦信CPP 第五集配套示例 - 编译运行脚本
# 使用方法: ./build_run.sh
# 支持 macOS (clang++) / Linux (g++)
# =============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# --- 颜色定义 ---
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# --- 检测编译器 ---
detect_compiler() {
    if command -v clang++ >/dev/null 2>&1; then
        CXX=clang++
    elif command -v g++ >/dev/null 2>&1; then
        CXX=g++
    elif command -v c++ >/dev/null 2>&1; then
        CXX=c++
    else
        echo -e "${RED}错误: 未找到 C++ 编译器 (clang++ / g++ / c++)${NC}"
        exit 1
    fi

    VERSION=$($CXX --version 2>/dev/null | head -1)
    echo -e "${CYAN}检测到编译器: ${BOLD}$CXX${NC} $VERSION"

    # 检查 C++20 支持
    if ! $CXX -std=c++20 -E -x c++ - < /dev/null >/dev/null 2>&1; then
        echo -e "${RED}错误: 当前编译器不支持 C++20${NC}"
        exit 1
    fi
    echo -e "${GREEN}C++20 支持: OK${NC}"
}

# --- 编译单个文件 ---
compile_file() {
    local src="$1"
    local bin="$2"
    local name=$(basename "$src" .cpp)

    echo -n "  编译 $name ... "
    if $CXX -std=c++20 -Wall -Wextra -O2 -o "$bin" "$src" 2>/dev/null; then
        echo -e "${GREEN}✓${NC}"
        return 0
    else
        echo -e "${RED}✗${NC}"
        echo -e "    ${YELLOW}编译出错 (clang/g++ 输出):${NC}"
        $CXX -std=c++20 -Wall -Wextra -O2 -o "$bin" "$src" 2>&1 | head -10 | sed 's/^/    /'
        return 1
    fi
}

# --- 运行单个可执行文件 ---
run_file() {
    local bin="$1"
    local name=$(basename "$bin")
    echo -e "  ${CYAN}[运行] $name${NC}"
    if [[ "$name" == "01_type_traits_basics" ]] || [[ "$name" == "03_custom_type_trait" ]] || \
       [[ "$name" == "04_requires_expression" ]] || [[ "$name" == "06_custom_concept" ]] || \
       [[ "$name" == "07_combination" ]]; then
        "$bin" 2>&1 | sed 's/^/    /'
    else
        "$bin" 2>/dev/null | sed 's/^/    /' || "$bin" 2>&1 | sed 's/^/    /'
    fi
}

# --- 主流程 ---
main() {
    echo ""
    echo -e "${BOLD}${CYAN}============================================${NC}"
    echo -e "${BOLD}${CYAN}  谦信CPP · 第五集  编译运行${NC}"
    echo -e "${BOLD}${CYAN}  类型萃取与 Concept${NC}"
    echo -e "${BOLD}${CYAN}============================================${NC}"
    echo ""

    detect_compiler

    # 所有示例文件
    EXAMPLES=(
        "01_type_traits_basics:类型萃取基础"
        "02_type_transformations:类型变换"
        "03_custom_type_trait:手写类型萃取"
        "04_requires_expression:requires表达式"
        "05_standard_concepts:标准库Concept"
        "06_custom_concept:自定义Concept"
        "07_combination:组合拳实战"
    )

    # 清理旧的构建产物
    echo ""
    echo -e "${YELLOW}--- 清理旧产物 ---${NC}"
    for item in "${EXAMPLES[@]}"; do
        name="${item%%:*}"
        rm -f "$SCRIPT_DIR/$name"
    done
    echo "  清理完成"

    # 编译
    echo ""
    echo -e "${YELLOW}--- 编译 (C++20) ---${NC}"
    FAILED=0
    for item in "${EXAMPLES[@]}"; do
        src="$item"
        name="${src%:*}"
        compile_file "$SCRIPT_DIR/${name}.cpp" "$SCRIPT_DIR/$name" || ((FAILED++))
    done

    if [[ $FAILED -gt 0 ]]; then
        echo ""
        echo -e "${RED}编译失败 ($FAILED 个文件)${NC}"
        exit 1
    fi

    echo -e "${GREEN}全部编译成功${NC}"

    # 运行
    echo ""
    echo -e "${YELLOW}--- 运行 ---${NC}"
    PASSED=0
    FAILED_RUN=0
    for item in "${EXAMPLES[@]}"; do
        name="${item%%:*}"
        desc="${item##*:}"
        echo ""
        echo -e "${BOLD}  ★ $desc ($name)${NC}"
        if run_file "$SCRIPT_DIR/$name"; then
            ((PASSED++))
        else
            ((FAILED_RUN++))
            echo -e "    ${RED}运行出错${NC}"
        fi
    done

    # 总结
    echo ""
    echo -e "${CYAN}============================================${NC}"
    echo -e "  编译: ${GREEN}全部成功${NC} | 运行: ${GREEN}${PASSED} 通过${NC}" \
        $([ $FAILED_RUN -gt 0 ] && echo "| ${RED}${FAILED_RUN} 失败${NC}")
    echo -e "${CYAN}============================================${NC}"
    echo ""
    echo -e "  源码: ${SCRIPT_DIR}/*.cpp"
    echo -e "  产物: ${SCRIPT_DIR}/01_type_traits_basics ~ 07_combination"
    echo ""
}

main "$@"
