/**
 * 第四集代码示例 6：编译期字符串格式化
 * 展示如何在编译期处理和格式化字符串与位置信息
 * 需要 C++23 支持
 */

#include <iostream>
#include <array>

// fixed_string: 用作非类型模板参数的字符串包装
template<std::size_t N>
struct fixed_string {
    char data[N];
    constexpr fixed_string(const char (&str)[N]) {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = str[i];
        }
    }
    constexpr std::size_t size() const { return N - 1; } // 不包括 '\0'
};

// 编译期计算整数位数
consteval int count_digits(int value) {
    if (value == 0) return 1;
    if (value < 0) value = -value;
    int count = 0;
    while (value > 0) {
        count++;
        value /= 10;
    }
    return count;
}

// 编译期整数转字符数组
template<int Value>
consteval auto int_to_array() {
    constexpr bool negative = Value < 0;
    constexpr int abs_value = negative ? -Value : Value;
    constexpr int digits = count_digits(abs_value);
    constexpr int size = digits + (negative ? 1 : 0);
    
    std::array<char, size + 1> result{};
    
    int pos = size - 1;
    int v = abs_value;
    while (v > 0) {
        result[pos--] = '0' + (v % 10);
        v /= 10;
    }
    
    if (negative) result[0] = '-';
    if (abs_value == 0) result[0] = '0';
    result[size] = '\0';
    
    return result;
}

// 编译期字符串拼接：msg + " - " + file + ":" + line
template<fixed_string Msg, fixed_string File, int Line>
consteval auto log_with_location() {
    constexpr auto line_str = int_to_array<Line>();
    constexpr std::size_t total_len = Msg.size() + 3 + File.size() + 1 + line_str.size() - 1;
    
    std::array<char, total_len + 1> result{};
    std::size_t pos = 0;
    
    // 拷贝 msg
    for (std::size_t i = 0; i < Msg.size(); ++i) {
        result[pos++] = Msg.data[i];
    }
    
    // 添加 " - "
    result[pos++] = ' ';
    result[pos++] = '-';
    result[pos++] = ' ';
    
    // 拷贝 file
    for (std::size_t i = 0; i < File.size(); ++i) {
        result[pos++] = File.data[i];
    }
    
    // 添加 ":"
    result[pos++] = ':';
    
    // 拷贝 line
    for (std::size_t i = 0; i < line_str.size() - 1; ++i) {
        result[pos++] = line_str[i];
    }
    
    result[pos] = '\0';
    return result;
}

// 使用宏包装，在编译期格式化位置信息
#define LOG_LOCATION(msg) \
    log_with_location<msg, __FILE__, __LINE__>()

int main() {
    std::cout << "=== 第四集示例 6：编译期字符串格式化 ===" << std::endl;
    std::cout << std::endl;
    
    // 演示整数转换
    constexpr auto num1 = int_to_array<12345>();
    constexpr auto num2 = int_to_array<-999>();
    constexpr auto num3 = int_to_array<0>();
    
    std::cout << "编译期整数转换:\n";
    std::cout << "  12345 -> " << num1.data() << '\n';
    std::cout << "  -999  -> " << num2.data() << '\n';
    std::cout << "  0     -> " << num3.data() << '\n';
    std::cout << std::endl;
    
    // 演示完整的日志格式化
    constexpr auto log1 = LOG_LOCATION("Hello, World!");
    constexpr auto log2 = LOG_LOCATION("Another message");
    constexpr auto log3 = LOG_LOCATION("Testing compile-time formatting");
    
    std::cout << "编译期日志格式化:\n";
    std::cout << "  " << log1.data() << '\n';
    std::cout << "  " << log2.data() << '\n';
    std::cout << "  " << log3.data() << '\n';
    
    std::cout << std::endl;
    return 0;
}

/*
 * 编译期字符串格式化的关键技术：
 * 
 * 1. 使用 fixed_string 作为非类型模板参数：
 *    - C++20 允许类类型作为 NTTP，但必须是"结构化类型"
 *    - fixed_string 包装字符串字面量，所有成员 public
 *    - 替代 std::string_view（不能做 NTTP）
 * 
 * 2. 使用 std::array<char, N> 替代 std::string：
 *    - std::array 是字面类型，可以在编译期使用
 *    - 固定大小，栈上分配，无堆操作
 *    - std::string 在 consteval 中不可用（涉及动态分配）
 * 
 * 3. consteval 函数确保编译期执行：
 *    - int_to_array<Value>() 在编译期将整数转为字符数组
 *    - log_with_location() 在编译期拼接字符串
 *    - 配合模板参数实现完全编译期计算
 * 
 * 4. 模板参数传递编译期常量：
 *    - 使用非类型模板参数 (如 template<int Value>)
 *    - 配合 __LINE__ 宏获取编译期行号
 *    - __FILE__ 自动转为 fixed_string<N>
 * 
 * 5. 实际价值：
 *    - 零运行时开销：所有字符串操作在编译期完成
 *    - 类型安全：编译期检查所有操作
 *    - 适用场景：日志系统、断言、调试信息
 * 
 * 编译命令：
 * /usr/local/opt/llvm@18/bin/clang++ -std=c++23 06_compile_time_string_format.cpp -o 06_compile_time_string_format
 */
