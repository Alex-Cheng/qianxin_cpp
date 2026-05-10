#include <concepts>
#include <iostream>
#include <string>
#include <vector>

// 1) 基于 requires 表达式定义 concept
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

// 2) 组合已有 concept
template<typename T>
concept Number = std::integral<T> || std::floating_point<T>;

// 3) 要求类型支持 size() 并返回可转换为 size_t 的值
template<typename T>
concept HasSize = requires(T t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};

// 4) 使用多个要求构建更高阶 concept
template<typename T>
concept ContainerWithIndex = requires(T c, std::size_t i) {
    { c.size() } -> std::convertible_to<std::size_t>;
    { c[i] };
};

// 5) 约束模板参数的函数模板
template<Addable T>
T add(T a, T b) {
    return a + b;
}

// 6) 缩写函数模板（abbreviated template）
auto add_numbers(Number auto a, Number auto b) {
    return a + b;
}

// 7) requires 子句约束函数模板
template<typename T>
requires HasSize<T>
void print_size(const T& value) {
    std::cout << "size = " << value.size() << '\n';
}

// 8) 结合 concept 作为返回值约束
template<ContainerWithIndex C>  // 等同于 template<typename C> requires ContainerWithIndex<C>
auto first_element(C& container) {
    return container[0];
}

int main() {
    std::cout << "add(int,int) = " << add(1, 2) << '\n';
    std::cout << "add_numbers(double,double) = " << add_numbers(1.5, 2.5) << '\n';

    std::string text = "Hello";
    print_size(text);

    std::vector<int> values = {10, 20, 30};
    print_size(values);
    std::cout << "first element = " << first_element(values) << '\n';

    // 下面的调用如果取消注释，将导致编译失败：
    // add(std::string("a"), std::string("b"));   // Addable 限制要求结果可转换为 T
    // print_size(42);                              // 42 没有 size()
    // add_numbers("x", "y");                     // Number 只允许 integral 或 floating_point

    return 0;
}
