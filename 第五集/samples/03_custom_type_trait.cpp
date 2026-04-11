#include <type_traits>
#include <vector>
#include <iostream>

// 检测是否为指针
template <typename T>
struct IsPointer : std::false_type {};

template <typename T>
struct IsPointer<T*> : std::true_type {};

// 检测是否有 value_type 成员
template <typename T, typename = void>
struct HasValueType : std::false_type {};

template <typename T>
struct HasValueType<T, std::void_t<typename T::value_type>> : std::true_type {};

// 检测是否支持 + 运算
template <typename T, typename = void>
struct HasPlus : std::false_type {};

template <typename T>
struct HasPlus<T, std::void_t<decltype(std::declval<T>() + std::declval<T>())>>
    : std::true_type {};

int main() {
    std::cout << "IsPointer<int*>:  " << IsPointer<int*>::value << "\n";
    std::cout << "IsPointer<int>:   " << IsPointer<int>::value << "\n";
    std::cout << "HasValueType<vector<int>>: " << HasValueType<std::vector<int>>::value << "\n";
    std::cout << "HasValueType<int>:        " << HasValueType<int>::value << "\n";
    std::cout << "HasPlus<int>:     " << HasPlus<int>::value << "\n";
    std::cout << "HasPlus<int*>:    " << HasPlus<int*>::value << "\n";
    return 0;
}
