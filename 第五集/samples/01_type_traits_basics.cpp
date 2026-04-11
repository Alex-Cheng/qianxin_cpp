#include <type_traits>
#include <iostream>

int main() {
    // 基本类型检测
    static_assert(std::is_integral_v<int>);
    static_assert(!std::is_integral_v<double>);
    static_assert(std::is_pointer_v<int*>);
    static_assert(!std::is_pointer_v<int>);

    // 类型比较
    static_assert(std::is_same_v<int, int>);
    static_assert(!std::is_same_v<int, unsigned int>);

    // 布尔常量
    constexpr bool is_int = std::is_integral_v<int>;
    constexpr bool is_float = std::is_floating_point_v<double>;

    std::cout << "is_int: " << is_int << "\n";
    std::cout << "is_float: " << is_float << "\n";

    return 0;
}
