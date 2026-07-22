#include <iostream>
#include <vector>
#include <string>
#include <ranges>

namespace rv = std::views;

void print_separator() {
    std::cout << "----------------------------------------\n";
}

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::cout << "=== Range 适配器管道组合 ===\n\n";

    // 1. 基础链条
    std::cout << "1. 基础链条 filter + transform + take\n";
    auto r1 = v
        | rv::filter([](int x) { return x > 3; })
        | rv::transform([](int x) { return x * 2; })
        | rv::take(3);
    std::cout << "   结果: ";
    for (int x : r1) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 2. drop + take 组合（窗口操作）
    std::cout << "2. drop(3) + take(4)：取第4~7个元素\n";
    auto r2 = v | rv::drop(3) | rv::take(4);
    std::cout << "   结果: ";
    for (int x : r2) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 3. 多步骤数据清洗流水线
    std::cout << "3. 数据处理流水线\n";
    auto pipeline = v
        | rv::filter([](int x) { return x % 2 == 1; })       // 选奇数
        | rv::transform([](int x) { return x * 10; })         // 放大10倍
        | rv::filter([](int x) { return x < 80; })            // 只要小于80的
        | rv::reverse;                                         // 反转
    std::cout << "   结果: ";
    for (int x : pipeline) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 4. 字符串处理
    std::cout << "4. 字符串管道\n";
    std::string text = "Hello C++20 Ranges World";
    auto words = text
        | rv::split(' ')
        | rv::transform([](auto&& word) {
            std::string w;
            for (char c : word) w += c;
            return w;
        })
        | rv::filter([](const std::string& w) { return w.size() > 3; });
    std::cout << "   长度>3的单词: ";
    for (const auto& w : words) std::cout << w << " ";
    std::cout << "\n";
    print_separator();

    // 5. 管道可以复用
    std::cout << "5. 管道复用\n";
    std::vector<int> v2 = {100, 200, 300, 400, 500};
    auto same_pipeline = v2
        | rv::take(3)
        | rv::transform([](int x) { return x / 10; });
    std::cout << "   同一管道: ";
    for (int x : same_pipeline) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
