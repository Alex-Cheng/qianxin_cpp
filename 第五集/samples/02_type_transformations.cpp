#include <type_traits>
#include <iostream>

int main() {
    // 去 const / 去引用
    using T1 = std::remove_const_t<const int>;       // int
    using T2 = std::remove_reference_t<int&>;        // int
    using T3 = std::remove_cvref_t<const int&>;      // int (C++20)

    // decay：按值传递规则变换
    using T4 = std::decay_t<int>;          // int
    using T5 = std::decay_t<int&>;         // int
    using T6 = std::decay_t<const int*>;   // const int*

    // conditional：条件选择类型
    using T7 = std::conditional_t<true, int, double>;   // int
    using T8 = std::conditional_t<false, int, double>;   // double

    // add_pointer：添加指针
    using T9 = std::add_pointer_t<int>;    // int*

    std::cout << "Type transformations demo done.\n";
    return 0;
}
