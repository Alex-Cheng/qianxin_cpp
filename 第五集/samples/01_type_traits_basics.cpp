#include <type_traits>
#include <iostream>
#include <string>

// 演示类型萃取的基本用法：检测类型属性

void print_separator() {
    std::cout << "----------------------------------------\n";
}

int main() {
    std::cout << "=== 类型萃取基础：检测类型属性 ===\n\n";
    
    // 1. is_integral - 判断是否是整型
    std::cout << "1. std::is_integral\n";
    std::cout << "   int: " << std::is_integral_v<int> << "\n";
    std::cout << "   double: " << std::is_integral_v<double> << "\n";
    std::cout << "   char: " << std::is_integral_v<char> << "\n";
    print_separator();
    
    // 2. is_floating_point - 判断是否是浮点型
    std::cout << "2. std::is_floating_point\n";
    std::cout << "   float: " << std::is_floating_point_v<float> << "\n";
    std::cout << "   double: " << std::is_floating_point_v<double> << "\n";
    std::cout << "   int: " << std::is_floating_point_v<int> << "\n";
    print_separator();
    
    // 3. is_pointer - 判断是否是指针
    std::cout << "3. std::is_pointer\n";
    std::cout << "   int*: " << std::is_pointer_v<int*> << "\n";
    std::cout << "   int: " << std::is_pointer_v<int> << "\n";
    std::cout << "   char*: " << std::is_pointer_v<char*> << "\n";
    print_separator();
    
    // 4. is_same - 判断两个类型是否相同
    std::cout << "4. std::is_same\n";
    std::cout << "   int, int: " << std::is_same_v<int, int> << "\n";
    std::cout << "   int, double: " << std::is_same_v<int, double> << "\n";
    std::cout << "   int, const int: " << std::is_same_v<int, const int> << "\n";
    print_separator();
    
    // 5. is_class - 判断是否是类/结构体
    struct MyStruct {};
    std::cout << "5. std::is_class\n";
    std::cout << "   MyStruct: " << std::is_class_v<MyStruct> << "\n";
    std::cout << "   int: " << std::is_class_v<int> << "\n";
    std::cout << "   std::string: " << std::is_class_v<std::string> << "\n";
    print_separator();
    
    // 6. 实战：根据类型做不同处理
    std::cout << "6. 实战：if constexpr + type traits\n";
    
    auto process = [](auto value) {
        if constexpr (std::is_integral_v<decltype(value)>) {
            std::cout << "   处理整数: " << value << " -> " << value * 2 << "\n";
        } else if constexpr (std::is_floating_point_v<decltype(value)>) {
            std::cout << "   处理浮点数: " << value << " -> " << value * 2.0 << "\n";
        } else {
            std::cout << "   其他类型\n";
        }
    };
    
    process(42);
    process(3.14);
    process("hello");
    print_separator();
    
    std::cout << "\n类型萃取让我们在编译期\"看见\"类型的属性！\n";
    
    return 0;
}
