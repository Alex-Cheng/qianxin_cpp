#include <iostream>
#include <ranges>
#include <vector>

namespace rv = std::views;

void print_separator() {
    std::cout << "----------------------------------------\n";
}

int main() {
    std::cout << "=== Range 工厂：凭空创造序列 ===\n\n";

    // 1. iota：生成数字序列
    std::cout << "1. iota(1, 10)：生成 1 到 9\n   ";
    for (int x : rv::iota(1, 10)) std::cout << x << " ";
    std::cout << "\n\n";

    // 2. iota 无界 —— 无限序列！
    std::cout << "2. iota(0) 无限 + take(10)\n   ";
    auto infinite = rv::iota(0) | rv::take(10);
    for (int x : infinite) std::cout << x << " ";
    std::cout << "\n\n";

    // 3. iota 生成平方数序列
    std::cout << "3. iota 生成前 10 个平方数\n   ";
    auto squares = rv::iota(1)
        | rv::transform([](int x) { return x * x; })
        | rv::take(10);
    for (int x : squares) std::cout << x << " ";
    std::cout << "\n\n";

    // 4. iota 生成 Fibonacci 数列（前 15 个）
    std::cout << "4. iota 生成前 15 个 Fibonacci 数\n   ";
    auto fibonacci = rv::iota(0, 15)
        | rv::transform([a = 0LL, b = 1LL](int) mutable {
            long long result = a;
            long long next = a + b;
            a = b;
            b = next;
            return result;
        });
    for (long long x : fibonacci) std::cout << x << " ";
    std::cout << "\n\n";

    // 5. iota 做"带索引的变换"
    std::cout << "5. iota 做带索引的变换：f(i) = i*i + 2*i + 1\n   ";
    auto formula = rv::iota(0, 10)
        | rv::transform([](int i) { return i * i + 2 * i + 1; });
    for (int x : formula) std::cout << x << " ";
    std::cout << "\n";
    print_separator();

    // 6. 与现有容器组合
    std::cout << "6. 与容器组合：用外部数据生成序列\n";
    std::vector<int> weights = {2, 5, 3, 8, 1};
    auto weighted = rv::iota(0)
        | rv::transform([&](int i) { return weights[i] * 10; })
        | rv::take(weights.size());
    std::cout << "   权重 * 10: ";
    for (int x : weighted) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
