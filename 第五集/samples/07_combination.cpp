#include <type_traits>
#include <concepts>
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <typeinfo>

// Part 1: Concept 做接口约束
template <typename T>
concept AddableComparable = requires(T a, T b) {
    a + b;
    a < b;
};

template <AddableComparable T>
T max_value(T a, T b) { return (a < b) ? b : a; }

// Part 2: 类型萃取做内部实现
template <typename Container>
auto serialize(const Container& c) {
    if constexpr (requires { c.reserve(0); }) {
        std::vector<typename Container::value_type> out;
        out.reserve(c.size());
        for (const auto& item : c) out.push_back(item);
        return out;
    } else {
        return std::vector<typename Container::value_type>(c.begin(), c.end());
    }
}

// Part 3: 条件编译分支
template <typename T>
auto process(T value) {
    if constexpr (std::integral<T>) {
        return value * 2;
    } else if constexpr (std::floating_point<T>) {
        return value * 2.0;
    } else if constexpr (requires { typename T::value_type; }) {
        return static_cast<typename T::size_type>(value.size()) * 2;
    } else {
        return value;
    }
}

// Part 4: 静态断言门禁
template <typename T>
T safe_add(T a, T b) {
    static_assert(std::is_arithmetic_v<T>,
        "safe_add only supports arithmetic types");
    return a + b;
}

int main() {
    std::cout << "max_value(3, 7):     " << max_value(3, 7) << "\n";
    std::cout << "max_value(1.1, 2.2): " << max_value(1.1, 2.2) << "\n";

    std::vector<int> v = {1, 2, 3};
    auto s = serialize(v);
    std::cout << "serialized size:    " << s.size() << "\n";

    std::cout << "process(10):        " << process(10) << "\n";
    std::cout << "process(3.0):        " << process(3.0) << "\n";
    std::cout << "process(vector):     " << process(v) << "\n";

    std::cout << "safe_add(1, 2):      " << safe_add(1, 2) << "\n";

    return 0;
}
