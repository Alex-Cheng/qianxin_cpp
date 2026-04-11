#include <concepts>
#include <iostream>

template <typename T>
    requires std::integral<T>
T count_bits(T value) {
    int count = 0;
    while (value) {
        count += value & 1;
        value >>= 1;
    }
    return count;
}

template <typename T>
    requires std::floating_point<T>
T safe_sqrt(T x) {
    return x >= 0 ? x : -x;
}

int main() {
    std::cout << "count_bits(15):   " << count_bits(15) << "\n";
    std::cout << "safe_sqrt(-4.0):  " << safe_sqrt(-4.0) << "\n";

    static_assert(std::integral<int>);
    static_assert(!std::integral<double>);
    static_assert(std::floating_point<double>);

    return 0;
}
