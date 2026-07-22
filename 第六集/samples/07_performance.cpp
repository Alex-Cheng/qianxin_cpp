#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cstdlib>

namespace rv = std::views;
using Clock = std::chrono::high_resolution_clock;
using Ms = std::chrono::duration<double, std::milli>;

void print_separator() {
    std::cout << "----------------------------------------\n";
}

// 监控内存峰值（平台相关，macOS/Linux 用 task_info / /proc）
size_t current_memory_kb() {
#if defined(__APPLE__)
    // macOS: 通过 task_info 获取 resident size
    // 简化起见，跳过实现细节，使用占位
    return 0;
#elif defined(__linux__)
    // Linux: 读取 /proc/self/status 中的 VmRSS
    FILE* f = fopen("/proc/self/status", "r");
    if (!f) return 0;
    size_t kb = 0;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "VmRSS: %zu", &kb) == 1) break;
    }
    fclose(f);
    return kb;
#else
    return 0;
#endif
}

int main() {
    std::cout << "=== Ranges 性能优势：对比传统写法 ===\n\n";

    // ============ 场景设置 ============
    // 模拟股票交易数据处理：浮点价格 → 过滤 → 换算 → 取前 N
    constexpr size_t TOTAL = 10'000'000;   // 1千万条
    constexpr size_t TAKE_N = 500'000;      // 最终取前 50 万个

    std::cout << "数据规模: " << TOTAL / 1'000'000 << " 百万条\n";
    std::cout << "最终输出: " << TAKE_N / 1'000 << " 千条\n";
    std::cout << "操作链: filter >0 → transform×1.08 → take\n\n";

    // 生成测试数据（模拟股票价格波动）
    std::vector<double> data(TOTAL);
    for (size_t i = 0; i < TOTAL; ++i) {
        // 约 70% 正数，30% 负数（模拟无效数据）
        data[i] = (i % 10 < 7) ? (i % 1000) * 0.01 + 10.0 : -1.0;
    }

    print_separator();

    // ============ 传统写法 ============
    std::cout << "【传统写法】多次遍历 + 中间容器\n\n";

    auto mem_before = current_memory_kb();

    auto t1 = Clock::now();

    // 步骤1：筛选 >0 的（第一次遍历，分配容器 A）
    std::vector<double> filtered;
    filtered.reserve(TOTAL);
    for (size_t i = 0; i < TOTAL; ++i) {
        if (data[i] > 0)
            filtered.push_back(data[i]);
    }

    // 步骤2：价格 ×1.08（第二次遍历，分配容器 B）
    std::vector<double> transformed;
    transformed.reserve(filtered.size());
    for (size_t i = 0; i < filtered.size(); ++i) {
        transformed.push_back(filtered[i] * 1.08);
    }

    // 步骤3：取前 TAKE_N（第三次遍历，分配容器 C）
    size_t limit = std::min(transformed.size(), TAKE_N);
    std::vector<double> result_traditional;
    result_traditional.reserve(limit);
    for (size_t i = 0; i < limit; ++i) {
        result_traditional.push_back(transformed[i]);
    }

    auto t2 = Clock::now();

    auto mem_after = current_memory_kb();

    auto time_traditional = std::chrono::duration_cast<Ms>(t2 - t1);
    std::cout << "  filter 阶段: " << filtered.size() << " 个元素\n";
    std::cout << "  transform 阶段: " << transformed.size() << " 个元素\n";
    std::cout << "  take 阶段: " << result_traditional.size() << " 个元素\n";
    std::cout << "  耗时: " << std::fixed << std::setprecision(2)
              << time_traditional.count() << " ms\n";
    std::cout << "  中间容器: 3 个 (共 " << filtered.size() << " + "
              << transformed.size() << " + " << result_traditional.size()
              << " = ~" << (filtered.size() + transformed.size() + result_traditional.size())
              << " 个元素)\n";
    std::cout << "  遍历次数: 3 次\n";

    // 验证结果
    double sum_traditional = 0;
    for (auto x : result_traditional) sum_traditional += x;
    std::cout << "  校验和: " << std::fixed << std::setprecision(2) << sum_traditional << "\n";

    print_separator();

    // ============ Ranges 写法 ============
    std::cout << "【Ranges 写法】管道模式 —— 零中间容器，单次遍历\n\n";

    auto t3 = Clock::now();

    auto view = data
        | rv::filter([](double x) { return x > 0; })
        | rv::transform([](double x) { return x * 1.08; })
        | rv::take(TAKE_N);

    // 惰性求值：遍历时才真正执行，所有操作在单次遍历中交织完成
    double sum_ranges = 0;
    size_t count_ranges = 0;
    for (double x : view) {
        sum_ranges += x;
        ++count_ranges;
    }

    auto t4 = Clock::now();

    auto time_ranges = std::chrono::duration_cast<Ms>(t4 - t3);
    std::cout << "  输出元素: " << count_ranges << " 个\n";
    std::cout << "  耗时: " << std::fixed << std::setprecision(2)
              << time_ranges.count() << " ms\n";
    std::cout << "  中间容器: 0 个\n";
    std::cout << "  遍历次数: 1 次\n";
    std::cout << "  校验和: " << std::fixed << std::setprecision(2) << sum_ranges << "\n";

    print_separator();

    // ============ 对比总结 ============
    std::cout << "【性能对比】\n";
    double speedup = time_traditional.count() / time_ranges.count();
    std::cout << "  Ranges 比传统写法快  " << std::fixed << std::setprecision(1)
              << speedup << " 倍\n\n";

    std::cout << "差异根源：\n";
    std::cout << "  传统: filter → 分配容器 → transform → 分配容器 → take → 分配容器\n";
    std::cout << "        3 次独立遍历 + 3 个中间容器（~"
              << (filtered.size() + transformed.size() + result_traditional.size())
              << " 个 double 拷贝）\n\n";
    std::cout << "  Ranges: 每个元素沿管道一次性走完 filter→transform→take\n";
    std::cout << "        0 个中间容器，0 次元素拷贝，1 次遍历\n";

    // 数据越大，差距越明显
    std::cout << "\n提示：数据量越大，Ranges 的零拷贝优势越明显。\n";
    std::cout << "可以尝试 TOTAL = 100'000'000（1亿条）观察更大差距。\n";

    return 0;
}
