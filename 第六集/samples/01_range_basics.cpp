#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>

namespace rv = std::views;

void print_separator() {
    std::cout << "----------------------------------------\n";
}

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 1. 传统写法：迭代器对
    std::cout << "=== 1. 传统写法 ===\n";
    std::vector<int> result;
    std::copy_if(v.begin(), v.end(), std::back_inserter(result),
        [](int x) { return x % 2 == 0; });
    std::cout << "copy_if 结果: ";
    for (int x : result) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 2. Ranges 写法：filter
    std::cout << "=== 2. Ranges filter ===\n";
    auto even = v | rv::filter([](int x) { return x % 2 == 0; });
    std::cout << "filter 偶数: ";
    for (int x : even) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 3. Ranges 写法：transform
    std::cout << "=== 3. Ranges transform ===\n";
    auto squares = v | rv::transform([](int x) { return x * x; });
    std::cout << "transform 平方: ";
    for (int x : squares) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 4. 组合：filter + transform
    std::cout << "=== 4. 组合 filter + transform ===\n";
    auto even_squares = v
        | rv::filter([](int x) { return x % 2 == 0; })
        | rv::transform([](int x) { return x * x; });
    std::cout << "偶数平方: ";
    for (int x : even_squares) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 5. take / drop / reverse
    std::cout << "=== 5. take / drop / reverse ===\n";
    auto first3 = v | rv::take(3);
    std::cout << "take(3): ";
    for (int x : first3) std::cout << x << " ";
    std::cout << "\n";

    auto skip3 = v | rv::drop(3);
    std::cout << "drop(3): ";
    for (int x : skip3) std::cout << x << " ";
    std::cout << "\n";

    auto rev = v | rv::reverse;
    std::cout << "reverse: ";
    for (int x : rev) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 6. View 是惰性的
    std::cout << "=== 6. View 惰性求值 ===\n";
    int count = 0;
    auto lazy = v
        | rv::filter([&](int x) { ++count; return x % 2 == 0; })
        | rv::transform([&](int x) { ++count; return x * x; })
        | rv::take(2);
    std::cout << "定义 View 后 count = " << count << " (0表示未计算)\n";
    for (int x : lazy) {
        std::cout << "  产出: " << x << "\n";
    }
    std::cout << "遍历后 count = " << count << " (才触发计算)\n";
    print_separator();

    // 7. std::ranges::sort 直接传容器
    std::cout << "=== 7. std::ranges::sort ===\n";
    std::vector<int> to_sort = {5, 3, 1, 4, 2};
    std::ranges::sort(to_sort);
    std::cout << "排序后: ";
    for (int x : to_sort) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
