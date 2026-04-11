# 类型萃取与 Concept——在编译期掌控类型

## 封面

现代C++的发展道路

从哲学到实践

第五篇：类型萃取与 Concept——在编译期掌控类型

一门语言的类型系统哲学

---

## 第 1 页｜为什么需要"看见"类型？

**先问一个问题：为什么要在编译期"看见"类型？**

- 泛型代码（模板）需要对类型作出反应
- 不同类型往往需要不同的处理逻辑
- 编译器在实例化模板时，需要判断类型满足什么条件
- 类型信息还可以被用来做编译期优化

**核心问题：**

> 如何在编译期检测一个类型的"特征"——比如它是不是整数、能不能拷贝、是不是某个类的派生类？

这就是类型萃取（Type Traits）的核心价值。

---

## 第 2 页｜什么是类型萃取（Type Traits）？

**类型萃取（Type Traits）** 是 C++ 标准库提供的一套模板工具，用来在编译期**检测、查询、变换**类型。

标准库在 `<type_traits>` 头文件中提供了大量预定义类型萃取：

| 萃取 | 含义 | 示例 |
|------|------|------|
| `std::is_integral<T>` | T 是否是整型 | `is_integral<int>` → true |
| `std::is_floating_point<T>` | T 是否是浮点型 | `is_floating_point<double>` → true |
| `std::is_pointer<T>` | T 是否是指针 | `is_pointer<int*>` → true |
| `std::is_same<T, U>` | T 和 U 是否相同 | `is_same<int, int>` → true |
| `std::remove_const<T>` | 去掉 T 的 const | `remove_const<const int>` → int |
| `std::remove_reference<T>` | 去掉 T 的引用 | `remove_reference<int&>` → int |
| `add_pointer<T>` | 给 T 加上指针 | `add_pointer<int>` → int* |

**核心结论：**

> 类型萃取让我们在编译期"看见"类型的属性，并据此选择代码路径。

---

## 第 3 页｜类型萃取的底层机制

类型萃取的底层实现依赖两个关键技术：

### 3.1 模板偏特化（Partial Specialization）

```cpp
// 主模板：默认是 false
template <typename T>
struct IsPointer : std::false_type {};

// 偏特化：当 T 是指针类型时，偏特化版本匹配，value = true
template <typename T>
struct IsPointer<T*> : std::true_type {};
```

编译器在实例化 `IsPointer<int*>` 时：
1. 尝试主模板 → 匹配失败
2. 尝试偏特化 `IsPointer<T*>` → 匹配成功
3. 使用偏特化版本，得到 `value = true`

### 3.2 SFINAE（复习）

```cpp
template <typename T, typename = void>
struct HasValueType : std::false_type {};

template <typename T>
struct HasValueType<T, std::void_t<typename T::value_type>> : std::true_type {};
```

`std::void_t` 是 C++17 引入的技巧：如果 `typename T::value_type` 合法，`void_t` 展开为 `void`；否则 SFINAE 机制让这个特化被跳过。

**核心结论：**

> 模板偏特化 + SFINAE = 类型萃取的基础设施。

---

## 第 4 页｜实战：手写一个类型萃取

**目标：检测类型 T 是否是"类"（class / struct）**

```cpp
// 用标准库的 is_class
template <typename T>
void process(T value) {
    if constexpr (std::is_class_v<T>) {
        // 只有 T 是 class/struct 时这段代码才会被编译
        // ...
    }
}
```

**目标：实现一个通用的"去掉 const 和引用"的萃取**

```cpp
template <typename T>
struct RemoveConstRef {
    using type = T;  // 默认不变
};

template <typename T>
struct RemoveConstRef<const T> {
    using type = T;  // 去掉 const
};

template <typename T>
struct RemoveConstRef<T&> {
    using type = T;  // 去掉引用
};

template <typename T>
struct RemoveConstRef<const T&> {
    using type = T;  // 同时去掉 const 和引用
};

// 标准库写法（更简洁）
// template <typename T>
// using RemoveConstRef_t = std::remove_const_t<std::remove_reference_t<T>>;
```

**核心结论：**

> 类型萃取的实质，是对类型进行"模式匹配"，
> 用偏特化枚举所有我们关心的类型情况。

---

## 第 5 页｜类型变换（Type Transformations）

类型萃取不仅是"检测"，还能对类型做"变换"：

| 萃取 | 作用 | 示例 |
|------|------|------|
| `std::remove_const<T>` | 去掉顶层 const | `const int` → `int` |
| `std::add_const<T>` | 加上 const | `int` → `const int` |
| `std::remove_reference<T>` | 去掉引用 | `int&` → `int` |
| `std::add_lvalue_reference<T>` | 加上左值引用 | `int` → `int&` |
| `std::decay<T>` | 按值传递规则变换 | `int&` → `int`，`const char[N]` → `const char*` |
| `std::conditional<B, T, F>` | 条件选择类型 | `conditional<true, int, double>` → `int` |

**实战：实现一个"要么返回 int，要么返回 double"的类型选择**

```cpp
template <bool B>
using NumericType = std::conditional<B, int, double>::type;

using A = NumericType<true>;   // int
using B = NumericType<false>; // double
```

---

## 第 6 页｜类型萃取的工程价值

类型萃取在工程中最重要的两个价值：

### 6.1 泛型代码的差异化处理

```cpp
template <typename Container>
void serialize(const Container& c) {
    // 如果容器有 reserve 方法，预先分配空间
    if constexpr (requires { c.reserve(0); }) {
        c.reserve(c.size());
    }
    // 否则跳过
}
```

### 6.2 类型安全的条件编译

```cpp
template <typename T>
T add(T a, T b) {
    static_assert(std::is_arithmetic_v<T>,
        "T must be arithmetic type (int, float, double, etc.)");
    return a + b;
}
```

**核心结论：**

> 类型萃取让模板代码能够在编译期"看见"类型的身份，
> 从而做出正确的类型安全决策。

---

## 第 7 页｜Concept 登场——模板约束的现代化

**痛点回顾：** 模板元编程写类型约束，用 SFINAE 和 enable_if，写法非常晦涩：

```cpp
// 古老的 SFINAE 写法
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
process(T value) {
    return value * 2;
}
```

**问题：**
- 返回类型声明里塞了一堆逻辑，可读性极差
- 约束条件与函数逻辑混在一起
- 错误信息是一大堆模板展开的噪音

**C++20 的解决方案：Concept

> Concept 是一种**命名了的编译期谓词**，
> 用来约束模板参数必须满足的条件。

---

## 第 8 页｜Concept 的基本语法

### 定义 Concept

```cpp
// 用 requires 子句定义概念
template <typename T>
concept Numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

// 或者更丰富
template <typename T>
concept Addable = requires(T a, T b) {
    a + b;  // T 必须支持 + 运算
    a - b;  // T 必须支持 - 运算
};
```

### 使用 Concept 约束模板

```cpp
// 语法1：concept name 作为类型约束
template <Numeric T>
T double_value(T x) {
    return x * 2;
}

// 语法2：用 requires 子句
template <typename T>
    requires Numeric<T>
T double_value(T x) {
    return x * 2;
}

// 语法3：C++17 风格的简短约束
template <Numeric T>
T triple(T x) requires Numeric<T> {
    return x * 3;
}
```

**核心结论：**

> Concept 让模板约束从"隐式魔法"变成"显式声明"，
> 代码意图一目了然。

---

## 第 9 页｜requires 表达式详解

`requires` 表达式有两种用法：

### 9.1 requires 子句（放在 template<> 后面）

```cpp
template <typename T>
    requires std::integral<T>
T factorial(T n) { /* ... */ }
```

### 9.2 requires 表达式（作为布尔常量）

```cpp
template <typename T>
concept HasPlus = requires(T a, T b) {
    a + b;        // 语法检查：T 能做 + 运算
    a - b;        // 语法检查：T 能做 - 运算
    a == b;       // 语法检查：T 能做 == 比较
};
```

`requires` 表达式内部可以写**多项检查**：

```cpp
template <typename T>
concept Regular = requires(T a, T b, T c) {
    // 拷贝构造
    T{a};
    // 拷贝赋值
    a = b;
    // 默认构造（如果不是标量类型）
    // 相等比较
    a == b;
    // 不相等比较
    a != b;
};
```

---

## 第 10 页｜标准库预定义 Concept（C++20）

C++20 标准库提供了一套开箱即用的 Concept：

| Concept | 含义 |
|---------|------|
| `std::same_as<T, U>` | T 和 U 是同一类型 |
| `std::integral<T>` | T 是整型 |
| `std::floating_point<T>` | T 是浮点型 |
| `std::derived_from<T, B>` | T 派生自 B |
| `std::movable<T>` | T 可以移动 |
| `std::copyable<T>` | T 可以拷贝 |
| `std::default_initializable<T>` | T 可以默认构造 |
| `std::semiregular<T>` | T 是半规则类型（默认构造 + 可拷贝 + 可赋值） |
| `std::regular<T>` | T 是规则类型（半规则 + 可相等比较） |
| `std::invocable<F, Args...>` | F 可以用 Args 调用 |

---

## 第 11 页｜Concept 的工程价值：让编译器说话

### 价值1：错误信息极大改善

```cpp
template <Numeric T>
T safe_divide(T a, T b) {
    return a / b;
}

int main() {
    std::string s = "hello";
    safe_divide(s, s);  // 编译器直接告诉你：
                        // "s 违反了 Numeric 约束"
                        // 而不是一堆模板错误
}
```

### 价值2：约束即文档

```cpp
// 这行代码本身就是最好的注释
template <std::integral T>
T count_bits(T value) { /* ... */ }
```

> 任何人看到这个函数签名，就知道 T 必须是整数类型。
> 不需要额外的注释说明。

### 价值3：编译期早期检查

```cpp
// 如果传入了不满足条件的类型
// 编译器在实例化时就立刻报错
// 而不是等到运行时报错
```

---

## 第 12 页｜实战：综合示例

**设计一个函数：接受两个参数，支持 + 和 < 运算，返回较大者**

```cpp
// 用 Concept 约束
template <typename T>
concept AddableComparable = requires(T a, T b) {
    a + b;
    a < b;
};

template <AddableComparable T>
T max_value(T a, T b) {
    return (a + b + (a < b ? b : a) - (a < b ? a : b)) / 2;
    // 更直观写法：
    // return (a < b) ? b : a;
}
```

**配合 if constexpr 与 Concept 做分支**

```cpp
template <typename T>
void process(T value) {
    if constexpr (std::integral<T>) {
        // 整数路径
        value *= 2;
    } else if constexpr (std::floating_point<T>) {
        // 浮点路径
        value *= 2.0;
    } else {
        // 未知类型
        static_assert(std::floating_point<T> || std::integral<T>,
            "Unsupported type");
    }
}
```

---

## 第 13 页｜Concept 的底层原理

Concept 的实现仍然依赖模板偏特化，但进行了封装：

```cpp
// 这两行是等价的（简化理解）
template <typename T>
concept Numeric = std::is_integral_v<T> || std::floating_point_v<T>;

// 编译器内部展开大致等价于：
template <typename T>
struct Numeric_impl : std::bool_constant<
    std::is_integral_v<T> || std::floating_point_v<T>
> {};
template <typename T>
constexpr bool Numeric = Numeric_impl<T>::value;
```

**关键点：**
- Concept 是**编译期的布尔常量**
- 编译器在实例化模板前先检查 Concept 约束
- SFINAE 机制在 Concept 时代依然存在，只是被包装了

---

## 第 14 页｜Concept 的边界与陷阱

### 边界：Concept 不能做什么

- **不能递归**：`concept Foo = Foo<T>` 是非法的
- **不能有默认模板参数**
- **不能做运行时判断**（编译期求值）

### 常见陷阱

**陷阱1：顶层 const 的问题**

```cpp
// std::same_as<int, const int> 值为 false
// 因为顶层 const 被视为不同

// 正确做法：
template <typename T>
concept Addable = std::integral<T> || std::floating_point<T>;
```

**陷阱2：隐式转换**

```cpp
// requires 检查严格的语法可用性
// 不考虑隐式转换
template <typename T>
concept HasPlus = requires(T a, T b) {
    a + b;  // 必须是 a + b 直接合法，不允许先转换
};

// 如果 a + b 需要隐式转换，这里会失败
```

---

## 第 15 页｜类型萃取 + Concept：现代 C++ 的组合拳

两者配合使用，各司其职：

| 场景 | 用类型萃取 | 用 Concept |
|------|-----------|-----------|
| 查询类型属性 | `is_integral_v<T>` | - |
| 约束模板参数 | - | `std::integral<T>` |
| 类型变换 | `remove_const_t<T>` | - |
| 条件编译 | `if constexpr` | Concept 约束 |
| 静态断言 | `static_assert` | Concept |

**最佳实践：**

```cpp
// 1. 用 Concept 做接口约束（"这个类型能做什么"）
template <typename T>
    requires std::movable<T>
class SmartWrapper {
    T data;
public:
    // 2. 用类型萃取做内部实现（"如何处理这个类型"）
    using value_type = std::remove_cvref_t<T>;
    // ...
};
```

---

## 第 16 页｜总结

### 类型萃取的贡献

- ✅ 让编译期"看见"类型的属性
- ✅ 为模板代码提供类型安全的差异化逻辑
- ✅ 支撑了标准库 `<type_traits>` 的基础设施

### Concept 的贡献

- ✅ 让模板约束从隐式变显式
- ✅ 大幅改善错误信息质量
- ✅ 约束即文档，提升代码可读性
- ✅ C++20 最重要的新特性之一

### 核心结论

> **类型萃取告诉你"类型是什么"，**
> **Concept 告诉你"类型能做什么"。**
> **两者合在一起，让 C++ 在编译期拥有完整的类型掌控能力。**

---

### 下期预告

**C++20 Ranges 与算法革命——用组合式思维写代码**

我们将探讨 C++20 Ranges 如何彻底改造标准库算法，让函数式编程风格与现代 C++ 完美融合。

---

GitHub 示例代码：https://github.com/Alex-Cheng/qianxin_cpp/

谢谢大家。
