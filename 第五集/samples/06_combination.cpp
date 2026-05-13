#include <concepts>
#include <type_traits>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

// 演示类型萃取 + Concept 的组合使用

void print_separator() {
    std::cout << "----------------------------------------\n";
}

// 1. 泛型数值处理
template<typename T>
    requires std::is_arithmetic_v<T>
T safe_divide(T a, T b) {
    if (b == 0) {
        std::cout << "   警告：除数为零\n";
        return T{};
    }
    return a / b;
}

// 2. 类型安全的容器操作
template<typename T>
concept HasBeginEnd = requires(T c) {
    c.begin();
    c.end();
};

template<typename T>
concept HasValueType = requires { typename T::value_type; };

template<typename T>
    requires HasValueType<T> && std::copyable<typename T::value_type> && HasBeginEnd<T>
void safe_copy(const T& src, T& dst) {
    std::copy(src.begin(), src.end(), std::back_inserter(dst));
    std::cout << "   安全拷贝完成\n";
}

// 3. 智能算法选择：根据迭代器能力选择最优实现
template<std::forward_iterator Iter>
void my_sort(Iter first, Iter last) {
    std::cout << "   使用前向迭代器排序（插入排序/归并排序）\n";
}

template<std::random_access_iterator Iter>
void my_sort(Iter first, Iter last) {
    std::cout << "   使用随机访问迭代器排序（快速排序/堆排序）\n";
}

// 4. Concept 层次：根据类型能力选择实现
template<std::integral T>
T process(T value) {
    std::cout << "   处理整数: " << value << " -> " << value * 2 << "\n";
    return value * 2;
}

template<std::signed_integral T>
T process(T value) {
    std::cout << "   处理有符号整数: " << value << " -> " << value * 3 << "\n";
    return value * 3;
}

// 5. 类型萃取 + 编译期判断
template <typename T>
void smart_process(T value) {
    using U = std::remove_cvref_t<T>;

    if constexpr (std::integral<U>) {
        std::cout << "   整数处理: " << value << "\n";
    } else if constexpr (std::floating_point<U>) {
        std::cout << "   浮点数处理: " << value << "\n";
    } else if constexpr (std::is_same_v<U, std::string>) {
        std::cout << "   字符串处理: " << value << "\n";
    } else {
        std::cout << "   其他类型处理\n";
    }
}

int main() {
    std::cout << "=== 类型萃取 + Concept 组合实战 ===\n\n";
    
    // 1. 泛型数值处理
    std::cout << "1. 泛型数值处理\n";
    std::cout << "   safe_divide(10, 3): " << safe_divide(10, 3) << "\n";
    std::cout << "   safe_divide(10.0, 3.0): " << safe_divide(10.0, 3.0) << "\n";
    safe_divide(10, 0);  // 警告
    print_separator();
    
    // 2. 类型安全的容器操作
    std::cout << "2. 类型安全的容器操作\n";
    std::vector<int> src = {1, 2, 3};
    std::vector<int> dst;
    safe_copy(src, dst);
    print_separator();
    
    // 3. 智能算法选择
    std::cout << "3. 智能算法选择\n";
    std::vector<int> v = {3, 1, 2};
    std::list<int> l = {3, 1, 2};
    
    my_sort(v.begin(), v.end());  // 随机访问迭代器
    my_sort(l.begin(), l.end());  // 前向迭代器
    print_separator();
    
    // 4. Concept 智能选择模板
    std::cout << "4. Concept 智能选择模板\n";
    process(42);   // 调用 signed_integral 版本（更严格）
    process(42u);  // 调用 integral 版本（唯一匹配）
    print_separator();
    
    // 5. 综合示例：类型萃取 + Concept（函数模板）
    std::cout << "5. 综合示例\n";
    const double x = 42.23;
    smart_process(x);
    volatile int y = 100;
    smart_process(y);
    smart_process(std::string("hello"));
    print_separator();
    
    std::cout << "\n类型萃取 + Concept = 完整的编译期类型掌控能力！\n";
    
    return 0;
}
