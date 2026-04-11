#include <concepts>
#include <type_traits>
#include <iostream>
#include <vector>

template <typename T>
concept Numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <typename T>
concept Addable = requires(T a, T b) {
    a + b;
    a - b;
};

template <typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
};

template <typename T>
concept Arithmetic = Addable<T> && Comparable<T>;

template <typename C>
concept Container = requires(C c) {
    typename C::value_type;
    { c.begin() };
    { c.end() };
    { c.size() } -> std::convertible_to<std::size_t>;
};

template <Numeric T>
T square(T x) { return x * x; }

template <Arithmetic T>
T max_value(T a, T b) { return (a < b) ? b : a; }

template <Container C>
typename C::size_type count_elements(const C& c) { return c.size(); }

int main() {
    std::cout << "square(5):           " << square(5) << "\n";
    std::cout << "square(3.14):        " << square(3.14) << "\n";
    std::cout << "max_value(3, 7):     " << max_value(3, 7) << "\n";
    std::cout << "max_value(1.1, 2.2): " << max_value(1.1, 2.2) << "\n";
    std::cout << "count_elements(vec): " << count_elements(std::vector<int>{1,2,3}) << "\n";
    return 0;
}
