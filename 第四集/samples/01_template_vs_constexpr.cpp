/**
 * 第四集代码示例 1：模板元编程 vs constexpr
 * 展示同一个问题（编译期阶乘）的两种实现方式
 */

#include <iostream>

// ============================================
// 方式 1：模板元编程（C++98 风格）
// ============================================

template <int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

// 模板特化：递归终止条件
template <>
struct Factorial<0> {
    static constexpr int value = 1;
};

// ============================================
// 方式 2：constexpr 函数（C++11+ 风格）
// ============================================

constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// C++14 风格：使用循环
constexpr int factorial_loop(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// ============================================
// 测试与对比
// ============================================

int main() {
    // 模板元编程方式
    constexpr int r1 = Factorial<5>::value;
    std::cout << "模板元编程: 5! = " << r1 << std::endl;
    
    // constexpr 递归方式
    constexpr int r2 = factorial(5);
    std::cout << "constexpr 递归: 5! = " << r2 << std::endl;
    
    // constexpr 循环方式
    constexpr int r3 = factorial_loop(5);
    std::cout << "constexpr 循环: 5! = " << r3 << std::endl;
    
    // 关键点：这些都在编译期计算完成
    // 汇编代码里直接是常量 120，没有任何计算指令
    
    std::cout << "\n所有结果都在编译期计算完成！" << std::endl;
    std::cout << "运行时代码只是直接使用常量 120。" << std::endl;
    
    return 0;
}

/*
 * 编译并查看汇编代码：
 * g++ -std=c++14 -O2 -S 01_template_vs_constexpr.cpp
 * 
 * 你会发现生成的汇编中，r1、r2、r3 都直接是常量 120，
 * 没有任何乘法或递归调用的指令。
 */
