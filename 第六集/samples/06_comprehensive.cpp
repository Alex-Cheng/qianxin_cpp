#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <ranges>
#include <algorithm>
#include <numeric>

namespace rv = std::views;

void print_separator() {
    std::cout << "----------------------------------------\n";
}

struct Student {
    std::string name;
    int math;
    int english;
    int cs;
};

int main() {
    std::cout << "=== Ranges 综合实战：学生成绩分析系统 ===\n\n";

    std::vector<Student> students = {
        {"Alice", 85, 92, 95},
        {"Bob", 72, 68, 91},
        {"Charlie", 90, 88, 85},
        {"Diana", 65, 75, 70},
        {"Eve", 95, 96, 98},
        {"Frank", 58, 62, 55},
        {"Grace", 88, 85, 90},
        {"Hank", 45, 50, 48},
    };

    // ========== 1. 计算平均分并筛选优秀学生 ==========
    std::cout << "1. 优秀学生（总分 ≥ 240 且 无不及格科目）\n";
    auto excellent = students
        | rv::filter([](const Student& s) {
            int total = s.math + s.english + s.cs;
            return total >= 240 && s.math >= 60 && s.english >= 60 && s.cs >= 60;
        })
        | rv::transform([](const Student& s) {
            return s.name + ": 总分"
                + std::to_string(s.math + s.english + s.cs)
                + " (数" + std::to_string(s.math)
                + " 英" + std::to_string(s.english)
                + " CS" + std::to_string(s.cs) + ")";
        });
    for (const auto& info : excellent) {
        std::cout << "   " << info << "\n";
    }
    print_separator();

    // ========== 2. 按数学成绩排序并输出前 3 ==========
    std::cout << "2. 数学成绩 TOP 3\n";
    auto ranked = students;
    std::ranges::sort(ranked, std::greater{}, &Student::math);
    for (const auto& s : ranked | rv::take(3)) {
        std::cout << "   " << s.name << ": 数学 " << s.math << "\n";
    }
    print_separator();

    // ========== 3. 统计各科平均分 ==========
    std::cout << "3. 各科平均分\n";
    auto avg = [](const auto& students, auto proj) {
        auto scores = students | rv::transform(proj);
        double sum = std::accumulate(scores.begin(), scores.end(), 0.0);
        return sum / std::ranges::distance(students);
    };

    std::cout << "   数学平均: " << avg(students, &Student::math) << "\n";
    std::cout << "   英语平均: " << avg(students, &Student::english) << "\n";
    std::cout << "   CS平均:   " << avg(students, &Student::cs) << "\n";
    print_separator();

    // ========== 4. 挂科预警（任一科 < 60） ==========
    std::cout << "4. 挂科预警\n";
    auto at_risk = students | rv::filter([](const Student& s) {
        return s.math < 60 || s.english < 60 || s.cs < 60;
    });
    bool found = false;
    for (const auto& s : at_risk) {
        std::cout << "   ⚠ " << s.name << ": 数" << s.math
                  << " 英" << s.english << " CS" << s.cs << "\n";
        found = true;
    }
    if (!found) std::cout << "   ✓ 无人挂科\n";
    print_separator();

    // ========== 5. 成绩分布统计（用 iota 生成分数段） ==========
    std::cout << "5. 成绩分布（10分一档）\n";
    std::map<int, int> distribution;
    for (const auto& s : students) {
        distribution[s.math / 10 * 10]++;
        distribution[s.english / 10 * 10]++;
        distribution[s.cs / 10 * 10]++;
    }
    for (int bucket = 100; bucket >= 40; bucket -= 10) {
        int count = distribution[bucket];
        std::cout << "   " << bucket << "-" << bucket + 9 << "分: ";
        for (int i = 0; i < count; ++i) std::cout << "█";
        std::cout << " (" << count << "人)\n";
    }
    print_separator();

    // ========== 6. 查找最高/最低总分 ==========
    std::cout << "6. 总分统计\n";
    auto best = std::ranges::max_element(students, {},
        [](const Student& s) { return s.math + s.english + s.cs; });
    auto worst = std::ranges::min_element(students, {},
        [](const Student& s) { return s.math + s.english + s.cs; });
    std::cout << "   最高: " << best->name << " ("
              << best->math + best->english + best->cs << "分)\n";
    std::cout << "   最低: " << worst->name << " ("
              << worst->math + worst->english + worst->cs << "分)\n";

    return 0;
}
