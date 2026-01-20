/**
 * 第四集代码示例 3：编译期查找表
 * 展示如何在编译期生成查找表，运行时零开销使用
 */

#include <iostream>
#include <array>
#include <chrono>
#include <cmath>

// ============================================
// 编译期生成平方查找表
// ============================================

template <size_t N>
constexpr auto make_square_table() {
    std::array<int, N> table{};
    for (size_t i = 0; i < N; ++i) {
        table[i] = i * i;
    }
    return table;
}

// 编译期生成查找表
constexpr auto squares = make_square_table<1000>();

// ============================================
// 编译期生成三角函数查找表（性能关键场景）
// ============================================

// 编译期计算 sin 的泰勒级数近似
constexpr double sin_taylor(double x) {
    // 将角度归一化到 [-PI, PI]
    constexpr double PI = 3.14159265358979323846;
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    
    // 泰勒级数：sin(x) = x - x³/3! + x⁵/5! - x⁷/7! + ...
    double result = x;
    double term = x;
    for (int i = 1; i <= 10; ++i) {
        term *= -x * x / ((2 * i) * (2 * i + 1));
        result += term;
    }
    return result;
}

template <size_t N>
constexpr auto make_sin_table() {
    std::array<double, N> table{};
    constexpr double PI = 3.14159265358979323846;
    for (size_t i = 0; i < N; ++i) {
        double angle = (2.0 * PI * i) / N;
        table[i] = sin_taylor(angle);
    }
    return table;
}

// 编译期生成 sin 查找表（360度，每度一个值）
constexpr auto sin_table = make_sin_table<360>();

// ============================================
// 性能对比：运行时计算 vs 编译期查找表
// ============================================

// 运行时计算平方
int square_runtime(int n) {
    return n * n;
}

// 使用编译期查找表
int square_lookup(int n) {
    return squares[n];
}

// 运行时计算 sin（使用标准库）
double sin_runtime(int degree) {
    constexpr double PI = 3.14159265358979323846;
    double radian = degree * PI / 180.0;
    return std::sin(radian);
}

// 使用编译期 sin 查找表
double sin_lookup(int degree) {
    return sin_table[degree % 360];
}

// ============================================
// 性能测试
// ============================================

void benchmark() {
    const int iterations = 1000000000;
    
    // sin 计算性能对比
    std::cout << "=== sin 计算性能对比（" << iterations << " 次调用）===" << std::endl;
    
    // 测试运行时 sin 计算
    auto start3 = std::chrono::high_resolution_clock::now();
    volatile double sum3 = 0;
    for (int i = 0; i < iterations; ++i) {
        sum3 += sin_runtime(i % 360);
    }
    auto end3 = std::chrono::high_resolution_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(end3 - start3);
    
    // 测试 sin 查找表
    auto start4 = std::chrono::high_resolution_clock::now();
    volatile double sum4 = 0;
    for (int i = 0; i < iterations; ++i) {
        sum4 += sin_lookup(i % 360);
    }
    auto end4 = std::chrono::high_resolution_clock::now();
    auto duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(end4 - start4);
    
    std::cout << "运行时 sin(): " << duration3.count() << " ms" << std::endl;
    std::cout << "sin 查找表:   " << duration4.count() << " ms" << std::endl;
    std::cout << "加速比:       " << (double)duration3.count() / duration4.count() << "x" << std::endl;
}

// ============================================
// 更实用的示例：编译期字符串哈希
// ============================================

constexpr size_t hash_string(const char* str) {
    size_t hash = 5381;
    while (*str) {
        hash = ((hash << 5) + hash) + (*str++);  // hash * 33 + c
    }
    return hash;
}

// ============================================
// 平方查找表演示
// ============================================

void square_table_demo() {
    std::cout << "\n=== 编译期平方查找表演示 ===" << std::endl;
    
    // 显示一些关键数字的平方值
    std::cout << "squares[0]   = " << squares[0] << std::endl;
    std::cout << "squares[10]  = " << squares[10] << std::endl;
    std::cout << "squares[42]  = " << squares[42] << std::endl;
    std::cout << "squares[100] = " << squares[100] << std::endl;
    std::cout << "squares[256] = " << squares[256] << std::endl;
    std::cout << "squares[999] = " << squares[999] << std::endl;
    
    std::cout << "\n所有平方值在编译期计算完成！" << std::endl;
    std::cout << "运行时只需要查表，零计算开销。" << std::endl;
}

// ============================================
// sin 查找表演示
// ============================================

void sin_table_demo() {
    std::cout << "\n=== 编译期 sin 查找表演示 ===" << std::endl;
    
    // 显示一些关键角度的 sin 值
    std::cout << "sin(0°)   = " << sin_table[0] << std::endl;
    std::cout << "sin(30°)  = " << sin_table[30] << std::endl;
    std::cout << "sin(45°)  = " << sin_table[45] << std::endl;
    std::cout << "sin(60°)  = " << sin_table[60] << std::endl;
    std::cout << "sin(90°)  = " << sin_table[90] << std::endl;
    std::cout << "sin(180°) = " << sin_table[180] << std::endl;
    std::cout << "sin(270°) = " << sin_table[270] << std::endl;
    
    std::cout << "\n所有 sin 值在编译期计算完成！" << std::endl;
    std::cout << "运行时只需要查表，零计算开销。" << std::endl;
}

// 使用编译期哈希实现快速字符串比较
void string_hash_demo() {
    constexpr size_t hash1 = hash_string("Hello");
    constexpr size_t hash2 = hash_string("World");
    constexpr size_t hash3 = hash_string("Hello");
    
    std::cout << "\n=== 编译期字符串哈希 ===" << std::endl;
    std::cout << "hash(\"Hello\") = " << hash1 << std::endl;
    std::cout << "hash(\"World\") = " << hash2 << std::endl;
    std::cout << "hash(\"Hello\") = " << hash3 << std::endl;
    
    // 编译期比较
    static_assert(hash1 == hash3, "相同字符串应该有相同的哈希值");
    static_assert(hash1 != hash2, "不同字符串应该有不同的哈希值");
    
    std::cout << "所有哈希值在编译期计算完成！" << std::endl;
}

// ============================================
// 测试
// ============================================

int main() {
    std::cout << "=== 编译期查找表示例 ===" << std::endl;
    
    // 测试查找表
    std::cout << "squares[42] = " << squares[42] << std::endl;
    std::cout << "squares[99] = " << squares[99] << std::endl;
    
    // 性能对比
    std::cout << std::endl;
    benchmark();
    
    // 平方查找表示例
    square_table_demo();
        
    // 字符串哈希示例
    string_hash_demo();
    
    std::cout << "\n关键点：" << std::endl;
    std::cout << "1. 查找表在编译期生成，存储在只读数据段" << std::endl;
    std::cout << "2. 运行时只需要读取内存，不需要任何计算" << std::endl;
    std::cout << "3. 这是典型的'用空间换时间'优化" << std::endl;
    
    return 0;
}

/*
 * 编译期查找表的应用场景：
 * 1. 数学函数表（sin/cos/sqrt 等）
 * 2. 字符编码转换表
 * 3. CRC/哈希校验表
 * 4. 游戏中的物理参数表
 * 5. 配置常量验证
 * 
 * 编译命令：
 * g++ -std=c++17 -O2 03_compile_time_lookup_table.cpp -o lookup_table
 */
