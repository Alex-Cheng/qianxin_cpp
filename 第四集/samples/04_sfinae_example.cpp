/**
 * 第四集代码示例 4：SFINAE - 编译期条件选择
 * 展示 SFINAE (Substitution Failure Is Not An Error) 的基本用法
 * 这是模板元编程实现条件分支的核心技术
 */

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>

// ============================================
// SFINAE 基础示例：enable_if
// ============================================

// 只对整数类型启用
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
process(T value) {
    std::cout << "处理整数: ";
    return value * 2;
}

// 只对浮点类型启用
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
process(T value) {
    std::cout << "处理浮点数: ";
    return value * 3.14;
}

// ============================================
// SFINAE 实战：根据类型选择不同实现
// ============================================

// 为有 size() 方法的类型启用
template <typename T>
auto get_size(const T& container) 
    -> decltype(container.size(), size_t()) {
    std::cout << "使用 size() 方法: ";
    return container.size();
}

// 为数组类型启用
template <typename T, size_t N>
constexpr size_t get_size(const T (&array)[N]) {
    std::cout << "使用数组大小: ";
    return N;
}

// ============================================
// 检测成员函数是否存在（SFINAE 高级应用）
// ============================================

// 检测类型是否有 begin() 方法
template <typename T>
class has_begin_method {
    // 如果 T 有 begin() 方法，这个版本会被选中
    template <typename U>
    static auto test(int) -> decltype(std::declval<U>().begin(), std::true_type());
    
    // 否则选择这个版本
    template <typename U>
    static std::false_type test(...);
    
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

// ============================================
// 使用 SFINAE 实现通用打印函数
// ============================================

// 为可迭代容器启用（有 begin/end）
template <typename T>
typename std::enable_if<has_begin_method<T>::value, void>::type
print_value(const T& container) {
    std::cout << "[";
    bool first = true;
    for (const auto& item : container) {
        if (!first) std::cout << ", ";
        std::cout << item;
        first = false;
    }
    std::cout << "]";
}

// 为非容器类型启用
template <typename T>
typename std::enable_if<!has_begin_method<T>::value, void>::type
print_value(const T& value) {
    std::cout << value;
}

// ============================================
// 对比：if constexpr 简化 SFINAE
// ============================================

#if __cplusplus >= 201703L
// C++17 的 if constexpr 让代码更简洁
template <typename T>
void print_value_modern(const T& value) {
    if constexpr (has_begin_method<T>::value) {
        // 可迭代容器
        std::cout << "[";
        bool first = true;
        for (const auto& item : value) {
            if (!first) std::cout << ", ";
            std::cout << item;
            first = false;
        }
        std::cout << "]";
    } else {
        // 普通值
        std::cout << value;
    }
}
#endif

// ============================================
// 测试
// ============================================

int main() {
    std::cout << "=== SFINAE 基础示例 ===" << std::endl;
    
    // 整数类型调用整数版本
    auto r1 = process(10);
    std::cout << r1 << std::endl;
    
    // 浮点类型调用浮点版本
    auto r2 = process(3.0);
    std::cout << r2 << std::endl;
    
    // 如果传入不支持的类型（如字符串），会编译错误
    // auto r3 = process("hello");  // Error: 没有匹配的函数
    
    std::cout << "\n=== get_size 示例 ===" << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout << get_size(vec) << std::endl;
    
    int arr[] = {1, 2, 3};
    std::cout << get_size(arr) << std::endl;
    
    std::string str = "Hello";
    std::cout << get_size(str) << std::endl;
    
    std::cout << "\n=== 类型特征检测 ===" << std::endl;
    
    std::cout << "vector<int> 有 begin() 方法: " 
              << has_begin_method<std::vector<int>>::value << std::endl;
    std::cout << "int 有 begin() 方法: " 
              << has_begin_method<int>::value << std::endl;
    std::cout << "string 有 begin() 方法: " 
              << has_begin_method<std::string>::value << std::endl;
    
    std::cout << "\n=== 通用打印函数 ===" << std::endl;
    
    std::vector<int> v = {1, 2, 3, 4};
    std::cout << "vector: ";
    print_value(v);
    std::cout << std::endl;
    
    std::cout << "int: ";
    print_value(42);
    std::cout << std::endl;
    
    std::cout << "string: ";
    print_value(std::string("Hello"));
    std::cout << std::endl;
    
#if __cplusplus >= 201703L
    std::cout << "\n=== if constexpr 简化版本 (C++17) ===" << std::endl;
    
    std::cout << "vector: ";
    print_value_modern(v);
    std::cout << std::endl;
    
    std::cout << "int: ";
    print_value_modern(42);
    std::cout << std::endl;
#endif
    
    std::cout << "\n关键点：" << std::endl;
    std::cout << "1. SFINAE 是模板元编程实现条件分支的核心技术" << std::endl;
    std::cout << "2. 模板替换失败不会导致编译错误，只是该候选被排除" << std::endl;
    std::cout << "3. 语法比较晦涩，C++17 的 if constexpr 更简洁" << std::endl;
    std::cout << "4. C++20 的 Concept 让类型约束更加直观" << std::endl;
    
    return 0;
}

/*
 * SFINAE 的核心机制：
 * - "Substitution Failure Is Not An Error"
 * - 模板参数替换失败时，编译器不报错，而是尝试其他重载
 * - 用于实现编译期的类型分支和条件选择
 * 
 * 演进历史：
 * C++98/11: SFINAE + enable_if (语法晦涩)
 * C++17:    if constexpr (简化了很多场景)
 * C++20:    Concept (最直观的类型约束)
 * 
 * 编译命令：
 * C++14: g++ -std=c++14 04_sfinae_example.cpp -o sfinae
 * C++17: g++ -std=c++17 04_sfinae_example.cpp -o sfinae
 */
