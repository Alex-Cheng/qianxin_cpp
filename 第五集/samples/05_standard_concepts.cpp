#include <concepts>
#include <iostream>
#include <string>
#include <vector>
#include <list>

// 演示标准库预定义 Concepts

void print_separator() {
    std::cout << "----------------------------------------\n";
}

int main() {
    std::cout << "=== 标准库预定义 Concepts ===\n\n";
    
    // 1. same_as - 判断类型相同
    std::cout << "1. std::same_as\n";
    std::cout << "   int == int: " << std::same_as<int, int> << "\n";
    std::cout << "   int == double: " << std::same_as<int, double> << "\n";
    std::cout << "   int == const int: " << std::same_as<int, const int> << "\n";
    print_separator();
    
    // 2. integral - 整型
    std::cout << "2. std::integral\n";
    std::cout << "   int: " << std::integral<int> << "\n";
    std::cout << "   char: " << std::integral<char> << "\n";
    std::cout << "   double: " << std::integral<double> << "\n";
    std::cout << "   bool: " << std::integral<bool> << "\n";
    print_separator();
    
    // 3. floating_point - 浮点型
    std::cout << "3. std::floating_point\n";
    std::cout << "   float: " << std::floating_point<float> << "\n";
    std::cout << "   double: " << std::floating_point<double> << "\n";
    std::cout << "   int: " << std::floating_point<int> << "\n";
    print_separator();
    
    // 4. derived_from - 派生关系
    struct Base {};
    struct Derived : Base {};
    
    std::cout << "4. std::derived_from\n";
    std::cout << "   Derived 派生自 Base: " << std::derived_from<Derived, Base> << "\n";
    std::cout << "   Base 派生自 Derived: " << std::derived_from<Base, Derived> << "\n";
    print_separator();
    
    // 5. movable / copyable
    std::cout << "5. std::movable / std::copyable\n";
    std::cout << "   int 是 movable: " << std::movable<int> << "\n";
    std::cout << "   int 是 copyable: " << std::copyable<int> << "\n";
    std::cout << "   std::string 是 movable: " << std::movable<std::string> << "\n";
    std::cout << "   std::string 是 copyable: " << std::copyable<std::string> << "\n";
    print_separator();
    
    // 6. default_initializable
    std::cout << "6. std::default_initializable\n";
    std::cout << "   int: " << std::default_initializable<int> << "\n";
    std::cout << "   std::string: " << std::default_initializable<std::string> << "\n";
    std::cout << "   std::vector<int>: " << std::default_initializable<std::vector<int>> << "\n";
    print_separator();
    
    // 7. invocable
    std::cout << "7. std::invocable\n";
    auto lambda = [](int x) { return x * 2; };
    std::cout << "   lambda(int): " << std::invocable<decltype(lambda), int> << "\n";
    std::cout << "   lambda(std::string): " << std::invocable<decltype(lambda), std::string> << "\n";
    print_separator();
    
    // 8. 迭代器 Concepts
    std::cout << "8. 迭代器 Concepts\n";
    std::cout << "   std::vector<int>::iterator 是 forward_iterator: " 
              << std::forward_iterator<std::vector<int>::iterator> << "\n";
    std::cout << "   std::vector<int>::iterator 是 random_access_iterator: " 
              << std::random_access_iterator<std::vector<int>::iterator> << "\n";
    std::cout << "   std::list<int>::iterator 是 forward_iterator: " 
              << std::forward_iterator<std::list<int>::iterator> << "\n";
    std::cout << "   std::list<int>::iterator 是 random_access_iterator: " 
              << std::random_access_iterator<std::list<int>::iterator> << "\n";
    print_separator();
    
    // 9. 实战：使用标准 Concepts 约束函数
    std::cout << "9. 实战：使用标准 Concepts 约束函数\n";
    
    auto double_value = [](std::integral auto x) {
        return x * 2;
    };
    
    auto half_value = [](std::floating_point auto x) {
        return x / 2.0;
    };
    
    std::cout << "   double_value(42): " << double_value(42) << "\n";
    std::cout << "   half_value(3.14): " << half_value(3.14) << "\n";
    // double_value(3.14);  // 编译错误：double 不满足 integral
    print_separator();
    
    std::cout << "\n标准库 Concepts 提供了开箱即用的类型约束！\n";
    
    return 0;
}
