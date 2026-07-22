#include <iostream>
#include <vector>
#include <string>
#include <ranges>
#include <algorithm>

namespace rv = std::views;

void print_separator() {
    std::cout << "----------------------------------------\n";
}

struct Person {
    std::string name;
    int age;
};

int main() {
    std::cout << "=== Projection 投影演示 ===\n\n";

    std::vector<Person> people = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35},
        {"Diana", 28},
        {"Eve", 22},
    };

    // 1. 传统写法：手写 lambda
    std::cout << "1. 传统排序（手写 lambda 比较 age）\n";
    std::vector<Person> p1 = people;
    std::sort(p1.begin(), p1.end(),
        [](const Person& a, const Person& b) { return a.age < b.age; });
    for (const auto& p : p1) {
        std::cout << "   " << p.name << " (" << p.age << ")\n";
    }
    print_separator();

    // 2. Ranges + Projection：按 age 排序
    std::cout << "2. Ranges + Projection：按 age 排序\n";
    std::vector<Person> p2 = people;
    std::ranges::sort(p2, {}, &Person::age);
    for (const auto& p : p2) {
        std::cout << "   " << p.name << " (" << p.age << ")\n";
    }
    print_separator();

    // 3. 按 name 长度排序
    std::cout << "3. Projection：按 name 长度排序\n";
    std::vector<Person> p3 = people;
    std::ranges::sort(p3, {}, [](const Person& p) { return p.name.size(); });
    for (const auto& p : p3) {
        std::cout << "   " << p.name << " (" << p.age << ") len=" << p.name.size() << "\n";
    }
    print_separator();

    // 4. min_element / max_element + Projection
    std::cout << "4. min_element / max_element + Projection\n";
    auto youngest = std::ranges::min_element(people, {}, &Person::age);
    auto oldest = std::ranges::max_element(people, {}, &Person::age);
    std::cout << "   最年轻: " << youngest->name << " (" << youngest->age << ")\n";
    std::cout << "   最年长: " << oldest->name << " (" << oldest->age << ")\n";
    print_separator();

    // 5. 普通数据也适用：按绝对值排序
    std::cout << "5. 普通数据 + Projection：按绝对值排序\n";
    std::vector<int> nums = {-5, 3, -1, 4, -2};
    auto sorted_nums = nums;
    std::ranges::sort(sorted_nums, {}, [](int x) { return std::abs(x); });
    std::cout << "    原始: ";
    for (int x : nums) std::cout << x << " ";
    std::cout << "\n    按绝对值排序: ";
    for (int x : sorted_nums) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 6. Projection 不修改原始数据
    std::cout << "6. Projection 不修改元素本身\n";
    auto people_display = people;
    std::ranges::sort(people_display, {}, &Person::age);
    for (const auto& p : people_display) {
        std::cout << "   " << p.name << ": " << p.age << " (age未变)\n";
    }

    return 0;
}
