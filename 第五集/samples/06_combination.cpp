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

// 1. 智能算法选择：根据迭代器能力选择最优实现
template<std::forward_iterator Iter>
void my_sort(Iter first, Iter last) {
    std::cout << "   使用前向迭代器排序（插入排序/归并排序）\n";
    // 简化实现：实际应该实现具体算法
}

template<std::random_access_iterator Iter>
void my_sort(Iter first, Iter last) {
    std::cout << "   使用随机访问迭代器排序（快速排序/堆排序）\n";
    // 简化实现：实际应该实现具体算法
}

// 2. 类型安全的容器操作
template<typename Container>
    requires std::copyable<Container> && 
             requires(Container c) { c.begin(); c.end(); }
void safe_copy(const Container& src, Container& dst) {
    dst = src;
    std::cout << "   安全拷贝完成\n";
}

// 3. 泛型数值处理
template<typename T>
    requires std::is_arithmetic_v<T>
T safe_divide(T a, T b) {
    if (b == 0) {
        std::cout << "   警告：除数为零\n";
        return T{};
    }
    return a / b;
}

// 4. 类型萃取 + Concept：智能序列化
template<typename T>
    requires std::is_trivially_copyable_v<T>
void fast_serialize(const T& obj, std::ostream& os) {
    os.write(reinterpret_cast<const char*>(&obj), sizeof(T));
    std::cout << "   快速序列化（memcpy）\n";
}

template<typename T>
    requires (!std::is_trivially_copyable_v<T>) && 
             requires(T t, std::ostream& os) { os << t; }
void serialize(const T& obj, std::ostream& os) {
    os << obj;
    std::cout << "   常规序列化\n";
}

// 5. Concept 层次：根据类型能力选择实现
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

int main() {
    std::cout << "=== 类型萃取 + Concept 组合实战 ===\n\n";
    
    // 1. 智能算法选择
    std::cout << "1. 智能算法选择\n";
    std::vector<int> v = {3, 1, 2};
    std::list<int> l = {3, 1, 2};
    
    my_sort(v.begin(), v.end());  // 随机访问迭代器
    my_sort(l.begin(), l.end());  // 前向迭代器
    print_separator();
    
    // 2. 类型安全的容器操作
    std::cout << "2. 类型安全的容器操作\n";
    std::vector<int> src = {1, 2, 3};
    std::vector<int> dst;
    safe_copy(src, dst);
    print_separator();
    
    // 3. 泛型数值处理
    std::cout << "3. 泛型数值处理\n";
    std::cout << "   safe_divide(10, 3): " << safe_divide(10, 3) << "\n";
    std::cout << "   safe_divide(10.0, 3.0): " << safe_divide(10.0, 3.0) << "\n";
    safe_divide(10, 0);  // 警告
    print_separator();
    
    // 4. 智能序列化
    std::cout << "4. 智能序列化\n";
    int trivial_value = 42;
    std::string non_trivial_value = "hello";
    
    fast_serialize(trivial_value, std::cout);  // 快速序列化
    serialize(non_trivial_value, std::cout);   // 常规序列化
    print_separator();
    
    // 5. Concept 智能选择模板
    std::cout << "5. Concept 智能选择模板\n";
    process(42);   // 调用 signed_integral 版本（更严格）
    process(42u);  // 调用 integral 版本（唯一匹配）
    print_separator();
    
    // 6. 综合示例：类型萃取 + Concept
    std::cout << "6. 综合示例\n";
    
    auto smart_process = [](auto value) {
        using T = std::remove_cvref_t<decltype(value)>;
        
        if constexpr (std::is_integral_v<T>) {
            std::cout << "   整数处理: " << value << "\n";
        } else if constexpr (std::is_floating_point_v<T>) {
            std::cout << "   浮点数处理: " << value << "\n";
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "   字符串处理: " << value << "\n";
        } else {
            std::cout << "   其他类型处理\n";
        }
    };
    
    smart_process(42);
    smart_process(3.14);
    smart_process(std::string("hello"));
    print_separator();
    
    std::cout << "\n类型萃取 + Concept = 完整的编译期类型掌控能力！\n";
    
    return 0;
}
