# 第四集：模板元编程与 constexpr —— 在编译期打造速度

## 📚 内容概述

本集展示 C++ 如何通过编译期计算实现"零成本抽象"的核心理念。

## 📁 文件说明

### 文档
- **第四集-模板元编程与constexpr.md** - 完整的 PPT 内容大纲（14页）
- **internal/第四篇ppt提示词.md** - Gamma 生成 PPT 的提示词
- **internal/第四集-模板元编程与constexpr口播.md** - 视频口播脚本

### 代码示例（samples/ 目录）
1. **01_template_vs_constexpr.cpp**
   - 对比模板元编程与 constexpr 两种编译期计算方式
   - 展示同一问题的不同实现

2. **02_if_constexpr.cpp**
   - C++17 的 `if constexpr` 编译期分支
   - 类型相关的编译期分派

3. **03_compile_time_lookup_table.cpp**
   - 编译期生成查找表（平方表、sin 查找表）
   - 性能对比：运行时 sin() vs 查找表（2.5x+ 加速）
   - 编译期字符串哈希

4. **04_sfinae_example.cpp**
   - SFINAE（Substitution Failure Is Not An Error）示例
   - 使用 enable_if 实现类型相关的函数重载
   - 成员函数检测

5. **05_constexpr_vs_consteval.cpp**
   - 对比 constexpr 与 C++20 的 consteval
   - 配置验证、单位转换等应用场景

## 🔧 编译与运行

### 使用 CMake（推荐）

```bash
# 进入 samples 目录
cd samples

# 配置项目
cmake .

# 编译所有示例
make

# 运行示例
./01_template_vs_constexpr
./02_if_constexpr
./03_compile_time_lookup_table
./04_sfinae_example
./05_constexpr_vs_consteval
```

### 手动编译

```bash
# 进入 samples 目录
cd samples

# C++17 示例
g++ -std=c++17 -O2 01_template_vs_constexpr.cpp -o example1
g++ -std=c++17 -O2 02_if_constexpr.cpp -o example2
g++ -std=c++17 -O2 03_compile_time_lookup_table.cpp -o example3
g++ -std=c++17 -O2 04_sfinae_example.cpp -o example4

# C++20 示例（需要支持 C++20 的编译器）
g++ -std=c++20 -O2 05_constexpr_vs_consteval.cpp -o example5
```

## 🎯 学习要点

### 1. 编译期计算的意义
- 把计算从运行时搬到编译期
- 运行时零开销
- 用编译时间换运行时速度

### 2. 两大工具
- **模板元编程**：强大但难写（C++98 时代）
- **constexpr**：实用且易读（现代 C++ 首选）

### 3. 关键特性
- `constexpr` 函数：可编译期也可运行时
- `if constexpr`：编译期分支选择
- SFINAE：模板元编程的条件分支技术
- `consteval`：强制编译期执行（C++20）

### 4. 实际应用
- 编译期查找表
- 编译期字符串哈希
- 配置验证
- 单位转换
- 类型检查

## 📊 性能对比

运行 `03_compile_time_lookup_table` 可以看到：
- **Sin 计算**：查找表比运行时 sin() 快 2.5x+
- **平方计算**：编译期查找表 vs 运行时计算
- **字符串哈希**：编译期完成，运行时零开销

在密集计算场景下（特别是三角函数），查找表可以带来显著的性能提升。

## 🔍 查看编译期计算的证据

### 查看汇编代码

```bash
# 生成汇编代码
g++ -std=c++17 -O2 -S 01_template_vs_constexpr.cpp

# 查看生成的 .s 文件
# 你会发现所有 constexpr 计算的结果都直接是常量
```

### 使用 Compiler Explorer

访问 https://godbolt.org/
- 粘贴示例代码
- 选择 C++17 或 C++20
- 开启优化（-O2）
- 观察生成的汇编代码中没有计算指令
