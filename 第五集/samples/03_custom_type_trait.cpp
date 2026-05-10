#include <type_traits>
#include <iostream>
#include <vector>
#include <string>

// 演示自定义类型萃取

void print_separator() {
    std::cout << "----------------------------------------\n";
}

// 1. 自定义 is_pointer 萃取（简化版）
template<typename T>
struct is_pointer_custom : std::false_type {};

template<typename T>
struct is_pointer_custom<T*> : std::true_type {};

template<typename T>
struct is_pointer_custom<T* const> : std::true_type {};

template<typename T>
inline constexpr bool is_pointer_custom_v = is_pointer_custom<T>::value;

// 2. 自定义 remove_const_ref 萃取
template<typename T>
struct remove_const_ref {
    using type = T;
};

template<typename T>
struct remove_const_ref<const T> {
    using type = T;
};

template<typename T>
struct remove_const_ref<T&> {
    using type = T;
};

template<typename T>
struct remove_const_ref<const T&> {
    using type = T;
};

template<typename T>
using remove_const_ref_t = typename remove_const_ref<T>::type;

// 3. 自定义 is_container 萃取
template<typename T, typename = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<T, std::void_t<
    typename T::value_type,
    typename T::iterator,
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end())
>> : std::true_type {};

template<typename T>
inline constexpr bool is_container_v = is_container<T>::value;

int main() {
    std::cout << "=== 自定义类型萃取 ===\n\n";
    
    // 1. 测试自定义 is_pointer
    std::cout << "1. 自定义 is_pointer_custom\n";
    std::cout << "   int*: " << is_pointer_custom_v<int*> << "\n";
    std::cout << "   int: " << is_pointer_custom_v<int> << "\n";
    std::cout << "   char*: " << is_pointer_custom_v<char*> << "\n";
    print_separator();
    
    // 2. 测试自定义 remove_const_ref
    std::cout << "2. 自定义 remove_const_ref\n";
    std::cout << "   const int& -> ";
    std::cout << std::is_same_v<remove_const_ref_t<const int&>, int> << "\n";
    std::cout << "   int& -> ";
    std::cout << std::is_same_v<remove_const_ref_t<int&>, int> << "\n";
    std::cout << "   const int -> ";
    std::cout << std::is_same_v<remove_const_ref_t<const int>, int> << "\n";
    print_separator();
    
    // 3. 测试自定义 is_container
    std::cout << "3. 自定义 is_container\n";
    std::cout << "   std::vector<int>: " << is_container_v<std::vector<int>> << "\n";
    std::cout << "   std::string: " << is_container_v<std::string> << "\n";
    std::cout << "   int: " << is_container_v<int> << "\n";
    print_separator();
    
    // 4. 实战：使用自定义萃取
    std::cout << "4. 实战：根据是否是容器做不同处理\n";
    
    auto process = [](auto& value) {
        if constexpr (is_container_v<decltype(value)>) {
            std::cout << "   容器大小: " << value.size() << "\n";
        } else {
            std::cout << "   非容器类型\n";
        }
    };
    
    std::vector<double> v {1.1, 2.2, 3.3};
    int x = 42;
    
    process(v);
    process(x);
    print_separator();
    
    std::cout << "\n自定义类型萃取让我们检测任意类型特征！\n";
    
    return 0;
}
