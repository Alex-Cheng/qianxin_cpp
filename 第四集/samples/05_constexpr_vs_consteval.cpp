/**
 * 第四集代码示例 5：constexpr vs consteval
 * C++20 引入 consteval，保证函数只在编译期执行
 */

#include <iostream>
#include <array>

// ============================================
// constexpr：可以编译期也可以运行时
// ============================================

constexpr int square_constexpr(int n) {
    return n * n;
}

// ============================================
// consteval：强制只能编译期（C++20）
// ============================================

#if __cplusplus >= 202002L  // C++20 或更高

consteval int square_consteval(int n) {
    return n * n;
}

// 编译期配置验证
consteval bool validate_config(int buffer_size, int max_size) {
    if (buffer_size <= 0) {
        throw "Buffer size must be positive!";
    }
    if (buffer_size > max_size) {
        throw "Buffer size exceeds maximum!";
    }
    return true;
}

// 使用 consteval 生成编译期字符串哈希
consteval unsigned int hash_compile_time(const char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = hash * 31 + (*str++);
    }
    return hash;
}

#endif

// ============================================
// 对比测试
// ============================================

void test_constexpr() {
    std::cout << "=== constexpr 测试 ===" << std::endl;
    
    // 1. 编译期调用（用编译期常量）
    constexpr int a = square_constexpr(10);
    std::cout << "编译期: square_constexpr(10) = " << a << std::endl;
    
    // 2. 运行时调用（用运行时变量）
    int x = 10;
    int b = square_constexpr(x);  // 运行时执行
    std::cout << "运行时: square_constexpr(x) = " << b << std::endl;
    
    // 3. 可以同时使用
    std::cout << "constexpr 既可以编译期，也可以运行时" << std::endl;
}

#if __cplusplus >= 202002L

void test_consteval() {
    std::cout << "\n=== consteval 测试 (C++20) ===" << std::endl;
    
    // 1. 编译期调用 - 正确
    constexpr int a = square_consteval(10);
    std::cout << "编译期: square_consteval(10) = " << a << std::endl;
    
    // 2. 运行时调用 - 编译错误！
    // int x = 10;
    // int b = square_consteval(x);  // Error: 不能在运行时调用
    
    std::cout << "consteval 只能在编译期调用" << std::endl;
    
    // 配置验证示例
    constexpr bool valid = validate_config(1024, 2048);
    std::cout << "\n配置验证通过: " << valid << std::endl;
    
    // 下面这行会在编译期报错（取消注释试试）
    // constexpr bool invalid = validate_config(3000, 2048);  // Error!
    
    // 编译期字符串哈希
    constexpr unsigned int h1 = hash_compile_time("GET");
    constexpr unsigned int h2 = hash_compile_time("POST");
    constexpr unsigned int h3 = hash_compile_time("DELETE");
    
    std::cout << "\n编译期字符串哈希：" << std::endl;
    std::cout << "hash(\"GET\")    = " << h1 << std::endl;
    std::cout << "hash(\"POST\")   = " << h2 << std::endl;
    std::cout << "hash(\"DELETE\") = " << h3 << std::endl;
}

// ============================================
// 实用示例：编译期 switch 优化
// ============================================

// 使用 consteval 生成枚举到字符串的映射
enum class HttpMethod { GET, POST, PUT, DELETE };

consteval const char* method_to_string(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:    return "GET";
        case HttpMethod::POST:   return "POST";
        case HttpMethod::PUT:    return "PUT";
        case HttpMethod::DELETE: return "DELETE";
    }
    return "UNKNOWN";
}

void test_method_mapping() {
    std::cout << "\n=== HTTP Method 映射（编译期）===" << std::endl;
    
    constexpr const char* m1 = method_to_string(HttpMethod::GET);
    constexpr const char* m2 = method_to_string(HttpMethod::POST);
    
    std::cout << "HttpMethod::GET    → " << m1 << std::endl;
    std::cout << "HttpMethod::POST   → " << m2 << std::endl;
}

// ============================================
// consteval 的典型应用：编译期单位转换
// ============================================

consteval double km_to_meters(double km) {
    return km * 1000.0;
}

consteval double hours_to_seconds(double hours) {
    return hours * 3600.0;
}

void test_unit_conversion() {
    std::cout << "\n=== 编译期单位转换 ===" << std::endl;
    
    constexpr double distance = km_to_meters(5.5);
    constexpr double time = hours_to_seconds(2.0);
    
    std::cout << "5.5 km = " << distance << " meters" << std::endl;
    std::cout << "2.0 hours = " << time << " seconds" << std::endl;
    
    // 这些常量在编译期就已经转换完成
    // 运行时代码直接使用 5500.0 和 7200.0
}

#endif

// ============================================
// 对比总结
// ============================================

void comparison_summary() {
    std::cout << "\n=== constexpr vs consteval 对比 ===" << std::endl;
    std::cout << "\nconstexpr:" << std::endl;
    std::cout << "  ✓ 可以编译期调用（用编译期常量时）" << std::endl;
    std::cout << "  ✓ 可以运行时调用（用运行时变量时）" << std::endl;
    std::cout << "  ✓ 灵活，适用范围广" << std::endl;
    
#if __cplusplus >= 202002L
    std::cout << "\nconsteval (C++20):" << std::endl;
    std::cout << "  ✓ 只能编译期调用" << std::endl;
    std::cout << "  ✓ 强制保证编译期执行" << std::endl;
    std::cout << "  ✓ 适用于配置验证、代码生成等必须编译期的场景" << std::endl;
#else
    std::cout << "\n注意：consteval 需要 C++20 或更高版本" << std::endl;
#endif
}

// ============================================
// 主函数
// ============================================

int main() {
    test_constexpr();
    
#if __cplusplus >= 202002L
    test_consteval();
    test_method_mapping();
    test_unit_conversion();
#else
    std::cout << "\n注意：部分示例需要 C++20 支持" << std::endl;
    std::cout << "当前编译器版本不支持 consteval" << std::endl;
#endif
    
    comparison_summary();
    
    std::cout << "\n关键点：" << std::endl;
    std::cout << "1. constexpr = 可编译期也可运行时" << std::endl;
    std::cout << "2. consteval = 强制只能编译期" << std::endl;
    std::cout << "3. consteval 用于必须在编译期确定的场景" << std::endl;
    std::cout << "4. 编译期保证从约定变成了强制" << std::endl;
    
    return 0;
}

/*
 * consteval 的典型应用场景：
 * 1. 配置验证 - 编译期检查配置参数
 * 2. 代码生成 - 生成查找表、哈希值等
 * 3. 单位转换 - 物理单位的编译期转换
 * 4. 字符串处理 - 编译期字符串哈希、解析
 * 5. 静态断言辅助 - 提供更好的错误信息
 * 
 * 编译命令：
 * C++17: g++ -std=c++17 05_constexpr_vs_consteval.cpp -o constexpr_test
 * C++20: g++ -std=c++20 05_constexpr_vs_consteval.cpp -o consteval_test
 */
