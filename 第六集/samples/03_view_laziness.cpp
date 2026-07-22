#include <iostream>
#include <vector>
#include <ranges>

namespace rv = std::views;

void print_separator() {
    std::cout << "----------------------------------------\n";
}

// 一个会产生副作用的 transform，用来观察调用时机
int transform_with_log(int x, int& call_count) {
    ++call_count;
    return x * x;
}

int main() {
    std::cout << "=== View 惰性求值演示 ===\n\n";

    int transform_calls = 0;
    int filter_calls = 0;

    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};

    // 定义 View 管道 —— 此时什么都不发生
    std::cout << "1. 定义 View 管道\n";
    auto view = v
        | rv::filter([&](int x) {
            ++filter_calls;
            return x % 2 == 0;
        })
        | rv::transform([&](int x) {
            ++transform_calls;
            return x * x;
        })
        | rv::take(2);

    std::cout << "   定义 View 后:\n";
    std::cout << "   filter 被调用了 " << filter_calls << " 次\n";
    std::cout << "   transform 被调用了 " << transform_calls << " 次\n";
    std::cout << "   → 定义 View 时不触发任何计算！\n\n";

    // 真正遍历时才触发计算
    std::cout << "2. 开始遍历 View\n";
    for (int x : view) {
        std::cout << "   消费: " << x;
        std::cout << " | filter调用: " << filter_calls;
        std::cout << " | transform调用: " << transform_calls << "\n";
    }
    std::cout << "\n   遍历结束后:\n";
    std::cout << "   filter 被调用了 " << filter_calls << " 次\n";
    std::cout << "   transform 被调用了 " << transform_calls << " 次\n";
    print_separator();

    // 每次遍历都重新计算（View 不缓存结果）
    std::cout << "3. 每次遍历都重新计算\n";
    std::cout << "   第二次遍历: ";
    int count = 0;
    for (int x : view) {
        ++count;
        std::cout << x << " ";
    }
    std::cout << "\n   输出了 " << count << " 个元素\n";
    std::cout << "   filter又被调用了，总共: " << filter_calls << " 次\n";
    std::cout << "   transform也被调用，总共: " << transform_calls << " 次\n";
    print_separator();

    // 提前终止也不会做多余计算
    std::cout << "4. 提前返回——View 不会做多余计算\n";
    int early_count = 0;
    for (int x : view) {
        if (++early_count >= 1) break;  // 只取一个就退出
        std::cout << x << " ";
    }
    std::cout << "   只消费了 " << early_count << " 个就退出\n";
    std::cout << "   → View 没有多余的求值\n";

    return 0;
}
