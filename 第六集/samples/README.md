# 第六集配套代码示例

> C++20 Ranges —— 用管道思维打开编程新模式
> GitHub: https://github.com/Alex-Cheng/qianxin_cpp/

---

## 目录结构

```
samples/
├── 01_range_basics.cpp         # Range 基础：filter, transform, take
├── 02_range_adaptors.cpp       # Range 适配器管道组合
├── 03_view_laziness.cpp        # View 惰性求值演示
├── 04_range_factories.cpp      # Range 工厂：iota 凭空创造序列
├── 05_projections.cpp          # Projection 投影演示
├── 06_comprehensive.cpp        # 综合实战：学生成绩分析系统
├── 07_performance.cpp          # 性能对比：零拷贝 vs 传统多容器
├── CMakeLists.txt              # 构建配置
├── build_run.sh                # macOS/Linux 一键运行
└── build_run.bat               # Windows 一键运行
```

---

## 运行方式

### macOS / Linux
```bash
cd samples
chmod +x build_run.sh
./build_run.sh
```

### Windows
```
双击 build_run.bat
```

### 手动构建
```bash
cd samples
cmake -B build -DCMAKE_CXX_STANDARD=20
cmake --build build
./build/01_range_basics
```

> 需要 CMake 3.16+ 和支持 C++20 的编译器（GCC 10+，Clang 13+，MSVC 2019 16.10+）。

---

## 示例说明

### 01_range_basics.cpp — Range 基础
展示 Ranges 最核心的用法：filter、transform、take、drop、reverse，以及 `std::ranges::sort` 直接传容器的便捷写法。对比传统迭代器对方式。

### 02_range_adaptors.cpp — 管道组合
展示管道操作符 `|` 的组合能力。多步数据清洗流水线、drop+take 窗口操作、字符串 split+filter、管道的复用。体现"声明式编程"的思维。

### 03_view_laziness.cpp — 惰性求值
通过计数器深入演示 View 的惰性求值机制：定义时不计算、遍历时才按需求值、提前退出不做多余工作。帮助理解 Ranges 的"零开销"原理。

### 04_range_factories.cpp — 凭空创造序列
展示 `std::views::iota` 工厂的多种用法：有界/无界序列、生成平方数、生成 Fibonacci 数列、带索引的变换、与外部数据组合。

### 05_projections.cpp — 投影
展示 Projection 的各种场景：按成员字段排序、按名字长度排序、min/max_element 配合投影、按绝对值排序。说明 Projection 如何将"取什么"与"怎么比"解耦。

### 06_comprehensive.cpp — 综合实战
一个完整的学生成绩分析系统：筛选优秀学生、各科排名 TOP3、计算平均分、挂科预警、成绩分布直方图、查找最高/最低分。综合运用 filter、transform、join、sort、Projection 等 Ranges 全家桶。

### 07_performance.cpp — 性能对比：零拷贝 vs 传统多容器
用 1 千万条数据对比两种写法。传统方式需要 3 次独立遍历 + 3 个中间 vector（~2000 万个 double 拷贝）；Ranges 管道模式在同一轮遍历中交织执行 filter → transform → take，零中间容器、零元素拷贝。数据量越大差距越明显。
