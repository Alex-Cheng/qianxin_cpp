/**
 * 第四集代码示例 2：if constexpr - 编译期类型分派
 * 展示如何根据类型在编译期选择不同的处理逻辑
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>

// ============================================
// 使用 if constexpr 实现类型相关的处理
// ============================================

template <typename T>
auto process(T value) {
    if constexpr (std::is_integral_v<T>) {
        // 整数类型：乘以 2
        std::cout << "处理整数: ";
        return value * 2;
    } 
    else if constexpr (std::is_floating_point_v<T>) {
        // 浮点类型：乘以 π
        std::cout << "处理浮点数: ";
        return value * 3.14159;
    } 
    else if constexpr (std::is_same_v<T, std::string>) {
        // 字符串类型：重复两次
        std::cout << "处理字符串: ";
        return value + value;
    }
    else {
        // 其他类型：直接返回
        std::cout << "未知类型: ";
        return value;
    }
}

// ============================================
// 对比：没有 if constexpr 的传统做法（函数重载）
// ============================================

// 需要为每种类型写一个重载函数
int process_old(int value) {
    std::cout << "处理整数(重载): ";
    return value * 2;
}

double process_old(double value) {
    std::cout << "处理浮点数(重载): ";
    return value * 3.14159;
}

std::string process_old(const std::string& value) {
    std::cout << "处理字符串(重载): ";
    return value + value;
}

// ============================================
// 更复杂的示例：编译期选择算法
// ============================================

template <typename Container>
void print_container(const Container& c) {
    if constexpr (std::is_same_v<Container, std::string>) {
        // 字符串：直接打印
        std::cout << "字符串: \"" << c << "\"" << std::endl;
    }
    else {
        // 其他容器：遍历打印
        std::cout << "容器: [";
        bool first = true;
        for (const auto& item : c) {
            if (!first) std::cout << ", ";
            std::cout << item;
            first = false;
        }
        std::cout << "]" << std::endl;
    }
}

// ============================================
// 测试
// ============================================

int main() {
    std::cout << "=== if constexpr 示例 ===" << std::endl;
    
    // 整数
    auto r1 = process(42);
    std::cout << r1 << std::endl;
    
    // 浮点数
    auto r2 = process(3.0);
    std::cout << r2 << std::endl;
    
    // 字符串
    auto r3 = process(std::string("Hello"));
    std::cout << r3 << std::endl;
    
    std::cout << "\n=== 传统重载方式 ===" << std::endl;
    
    std::cout << process_old(42) << std::endl;
    std::cout << process_old(3.0) << std::endl;
    std::cout << process_old(std::string("Hello")) << std::endl;
    
    std::cout << "\n=== 容器打印示例 ===" << std::endl;
    
    print_container(std::string("C++"));
    print_container(std::vector<int>{1, 2, 3, 4, 5});
    
    return 0;
}

/*
 * if constexpr 的关键优势：
 * 1. 编译期就确定走哪个分支，未选中的分支不会被实例化
 * 2. 可以在同一个函数模板里处理不同类型，避免写多个重载
 * 3. 未选中的分支不需要编译通过（可以包含该类型不支持的操作）
 * 
 * 编译命令：
 * g++ -std=c++17 02_if_constexpr.cpp -o if_constexpr
 */
