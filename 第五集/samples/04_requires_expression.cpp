#include <type_traits>
#include <vector>
#include <iostream>

template <typename T>
concept Addable = requires(T a, T b) {
    a + b;
};

template <typename T>
concept Comparable = requires(T a, T b) {
    a < b;
    a == b;
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

int main() {
    static_assert(Addable<int>);
    static_assert(!Addable<int*>);
    static_assert(Comparable<int>);
    static_assert(Arithmetic<int>);
    static_assert(Container<std::vector<int>>);

    std::cout << "All static_assert passed.\n";
    std::cout << "Addable<int>:     " << Addable<int> << "\n";
    std::cout << "Addable<int*>:    " << Addable<int*> << "\n";
    std::cout << "Arithmetic<int>:  " << Arithmetic<int> << "\n";
    std::cout << "Container<vector<int>>: " << Container<std::vector<int>> << "\n";
    return 0;
}
