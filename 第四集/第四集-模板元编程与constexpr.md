# 零成本抽象——C++的设计哲学

## 封面

现代C++的发展道路

从哲学到实践

第四篇：模板元编程与constexpr——在编译期打造极致速度

一门语言的效率之道

---

## 第 1 页｜为什么要在编译期计算

- 运行时的每一次计算都要消耗 CPU 时间
- 如果结果在编译期就能确定，为什么要等到运行时？
- C++ 的哲学：能在编译期做的，就不要拖到运行时
- 编译期提前计算，减轻运行时负担

这是"零成本抽象"的直接体现

**核心理念：**  
> 把计算从运行时搬到编译期，  

---

## 第 2 页｜什么是模板元编程？

**模板元编程（Template Metaprogramming）** 是利用 C++ 模板的实例化与特化机制，在编译期完成计算、条件分支、递归展开以及代码生成的技术。

**实现机制：**

1. **通过表达式实现计算**
   - 利用模板参数进行数值和类型运算

2. **通过模板递归实例化实现循环**
   - 递归展开模板，实现编译期迭代循环

3. **通过 SFINAE 实现分支/选择**
   - SFINAE: Substitution Failure Is Not An Error
   - 在模板实例化时进行条件选择

**核心结论：**  
> 模板元编程在理论上是图灵完备的。

---

## 第 3 页｜SFINAE：编译期条件选择

SFINAE（Substitution Failure Is Not An Error）让编译器在模板实例化时进行条件选择：

```cpp
#include <type_traits>

// 只对整数类型启用
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
process(T value) {
    return value * 2;
}

// 只对浮点类型启用
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
process(T value) {
    return value * 3.14;
}

int main() {
    auto a = process(10);      // 调用整数版本，返回 20
    auto b = process(3.0);     // 调用浮点版本，返回 9.42
    // auto c = process("hi");  // 编译错误：没有匹配的函数
}
```

**核心机制：**
- 模板替换失败不会导致编译错误，只是该候选被排除
- 编译器选择唯一匹配的模板
- 实现编译期的类型分支

**核心结论：**  
> SFINAE 是模板元编程实现条件分支的核心技术，  
> 但语法晦涩，后来被 `if constexpr` 和 `concept` 简化。

---

## 第 4 页｜模板元编程：经典示例

编译期计算阶乘：

```cpp
// 模板元编程方式（C++98）
template <int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static constexpr int value = 1;
};

// 使用
constexpr int result = Factorial<5>::value;  // 编译期得到 120
```

**特点：**

- 运行时 `result` 直接是常量 `120`，没有任何计算开销

---

## 第 5 页｜constexpr：现代方式

同样的阶乘，用 constexpr 实现：

```cpp
// constexpr 方式（C++11+）
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// 使用
constexpr int result = factorial(5);  // 编译期得到 120
```

**优势：**
- 语法就像普通函数，易读易写
- 既可以编译期调用，也可以运行时调用
- 随着C++的演进，能力越来越大

---

## 第 6 页｜constexpr 的演化

C++ 标准持续增强 constexpr 的能力：

| 版本 | 新增能力 |
|------|----------|
| **C++11** | constexpr 函数（单一 return 语句） |
| **C++14** | 支持循环、分支、局部变量 |
| **C++17** | constexpr lambda、if constexpr |
| **C++20** | constexpr 虚函数、动态内存分配（new/delete） |
| **C++23** | constexpr 函数中可以使用 static 和 thread_local 变量 |

**核心结论：**  
> constexpr 的能力边界在不断扩展，  
> 越来越多的运行时代码可以通过它搬到编译期。

---

## 第 7 页｜实战示例：编译期字符串处理

计算编译期字符串长度：

```cpp
constexpr size_t str_length(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    return len;
}

// 编译期计算
constexpr size_t len = str_length("Hello, C++");  // 编译期得到 10

// 可以用于数组大小
char buffer[str_length("Hello") + 1];  // buffer[6]
```

---

## 第 8 页｜实战示例：编译期查找表

生成编译期的平方查找表：

```cpp
template <size_t N>
constexpr auto make_square_table() {
    std::array<int, N> table{};
    for (size_t i = 0; i < N; ++i) {
        table[i] = i * i;
    }
    return table;
}

// 编译期生成查找表
constexpr auto squares = make_square_table<100>();

// 运行时直接查表，零计算开销
int main() {
    int result = squares[42];  // 直接读取 1764，不需要计算
}
```

**工程价值：**
- 复杂的计算逻辑在编译期完成
- 运行时只需要读取常量数据
- 典型的“空间换时间”、“编译期换运行时”优化

---

## 第 9 页｜if constexpr：编译期分支

C++17 引入 `if constexpr`，实现编译期条件编译：

```cpp
template <typename T>
auto process(T value) {
    if constexpr (std::is_integral_v<T>) {
        // 整数类型的处理
        return value * 2;
    } else if constexpr (std::is_floating_point_v<T>) {
        // 浮点类型的处理
        return value * 3.14;
    } else {
        // 其他类型
        return value;
    }
}
```

**核心机制：**
- 编译器根据模板参数在编译期选择分支
- 编译后的代码没有if分支，分支在现代CPU中很伤性能
- 未选中的分支不会被实例化，也不会进入最终代码
- 实现了零成本的分支

---

## 第 10 页｜consteval：强制编译期执行

`constexpr` 并不强制编译期执行，只是“建议”编译期执行。
如果参数不是编译期常量，`constexpr` 函数会在运行时执行。

C++20 引入 `consteval`，**强制保证**函数只在编译期执行：

```cpp
// constexpr：允许编译期也允许运行时
constexpr int square_constexpr(int n) {
    return n * n;
}

// consteval：强制只能编译期（C++20）
consteval int square_consteval(int n) {
    return n * n;
}

// 编译期调用 - 两者都正确
constexpr int a = square_constexpr(10);  // OK
constexpr int b = square_consteval(10);  // OK

// 运行时调用
int x = 5;
int c = square_constexpr(x);  // OK：运行时执行
int d = square_consteval(x);  // Error：不能在运行时调用 consteval 函数
```

**使用场景：**
- 配置验证（编译期检查配置合法性）
- 代码生成（编译期生成查找表、哈希值）
- 静态断言辅助

**核心结论：**  
> constexpr = “可以”编译期执行（但不强制），  
> consteval = “必须”编译期执行（强制保证）。

---

## 第 11 页｜模板元编程的现代应用

虽然 constexpr 更易用，但模板元编程在某些场景仍不可替代：

1. **类型萃取（type_traits）**
   ```cpp
   template <typename T>
   using remove_const_ref_t = std::remove_const_t<std::remove_reference_t<T>>;
   ```

2. **静态多态（CRTP）**

3. **其他奇妙、晦涩且有创造性的应用**

**核心结论：**  
> constexpr 主要处理“值”的编译期计算和逻辑；
> 模板元编程主要处理“类型”的萃取、变换与约束；
> concept（未来介绍） 让模板元编程更直观。

---

## 第 12 页｜性能对比：运行时 vs 编译期

| 方式 | 编译时间 | 运行时间 | 二进制大小 |
|------|----------|----------|-----------|
| **运行时计算** | 快 | 慢（需要重复计算） | 小 |
| **编译期计算** | 慢（编译器做了计算） | 快（直接用常量） | 稍大 |

---

## 第 14 页｜总结与展望

### 本篇总结：编译期计算的价值

编译期计算，是用**编译时间和代码空间，换取运行时速度和静态检查**

模板元编程在现代C++之前，就打开了编译期计算的大门，但是实现方式晦涩。constexpr 让编译期计算更加简单直接。
模板元编程当前用于类型萃取，但是同样实现方式晦涩一些，未来要介绍的 “概念concept”，会进一步简化。

---

### 下一篇预告
**模板元编程类型萃取与 concept——在编译期掌控类型**

我们将深入探讨 C++ 模板元编程中的类型萃取技术（type traits），以及 C++20 引入的 Concept 如何让类型约束变得更加简单直接。让我们在编译期掌控类型。

谢谢大家，一些代码示例，可以在我这个GitHub仓库里看到。

---
