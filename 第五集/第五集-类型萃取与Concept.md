# 类型萃取与 Concept——在编译期掌控类型

## 封面

现代C++的发展道路

从哲学到实践

第五篇：类型萃取与Concept——在编译期掌控类型

一门语言的长青之道

---

## 第 1 页｜为什么需要"看见"类型？

**先问一个问题：为什么要在编译期"看见"类型？**

- 模板实例化需要类型的相关信息
  比如需要知道 T 的大小和对齐，才能正确分配内存。
- 编译期类型约束检查
  在编译期直接拒绝不符合要求的类型，不用等到运行时再报错，而且错误信息尽量直观准确。例如，一个模板函数需要类型参数 T 支持加减乘除算术运算，如果给的类型是 `字符串`，编译期就应该报错。
- 根据类型生成不同的分支代码
  根据类型特征，选择生成不同的专用代码，以实现编译期分支分发与静态多态，运行时零开销。
- 编译期优化
  根据类型特征选择最优方式，比如是否给定的类型 T 可以通过直接内存拷贝批量复制，而不用逐元素调用拷贝构造函数。

**核心问题：**

> 如何在编译期检测一个类型的"特征"——比如它是不是整数、能不能直接通过内存拷贝复制元素、是不是某个类的派生类？

这就是类型萃取（Type Traits）的核心价值之一。

---

## 第 2 页｜为什么需要在编译期"修改"类型？

再问一个问题：为什么需要在编译期变换类型，或者简单的说，为什么要在编译期“修改”现有的类型？

- 修正类型
  经常有需要把类型修整成需要的形式。比如把模板收到的 `T`、`T&`、`T&&`、`const T&` 等类型实参，修正成“裸类型” `T`，以避免由于类型不统一导致的代码逻辑混乱。

- 提取类型
  从复合类型中提取组成部分。比如，从容器类型中提取容器中元素类型、从函数中提取返回类型、从指针提取指向类型等。

- 修饰类型
  基于已有类型 T 构造出 T 的指针、数组、引用等派生类型。

**核心问题：**

> 如何在编译期变换类型——比如去掉 const、去掉引用、加上指针、提取嵌套类型？

这就是类型萃取（Type Traits）的核心价值之二。

---

## 第 3 页｜什么是类型萃取（Type Traits）？

### 类型萃取的概念

为了解决上述问题，引入了 **类型萃取（Type Traits）** 的概念，核心是在编译期"看见"类型的特征，根据这些特征做出决策，或者对已有类型进行变换。它有三个核心能力：**检测**（这个类型是不是整数？能不能拷贝？是不是某个类的派生类？）、**查询**（这个类型的大小是多少？对齐要求是什么？）、**变换**（去掉 const、去掉引用、加上指针、提取嵌套类型）。

---

## 第 4 页｜C++ 标准库的类型萃取组件

C++ 标准库在 `<type_traits>` 头文件中提供了一整套模板工具，实现了类型萃取的思想。常用工具包括：

| 萃取 | 含义 | 示例 |
|------|------|------|
| `std::is_integral<T>` | T 是否是整型 | `is_integral<int>` → true |
| `std::is_floating_point<T>` | T 是否是浮点型 | `is_floating_point<double>` → true |
| `std::is_pointer<T>` | T 是否是指针 | `is_pointer<int*>` → true |
| `std::is_same<T, U>` | T 和 U 是否相同 | `is_same<int, int>` → true |
| `std::remove_const<T>` | 去掉 T 的 const | `remove_const<const int>` → int |
| `std::remove_reference<T>` | 去掉 T 的引用 | `remove_reference<int&>` → int |
| `std::remove_cvref<T>` | 同时去掉 const / volatile 和引用 | `remove_cvref<const int&>` → int |
| `std::add_pointer<T>` | 给 T 加上指针 | `add_pointer<int>` → int* |

**代码示例：**

```cpp
#include <type_traits>
#include <iostream>

// 用 type traits + if constexpr 根据类型做不同处理
template <typename T>
void process(T value) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "按照整数处理";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "按照浮点数处理";
    } else {
        std::cout << "按照其他类型处理";
    }
}

int main() {
    std::cout << process(10)   << "\n";   // 20
    std::cout << process(3.14) << "\n";   // 6.28
}
```

这个例子展示了 type traits 最常用的场景：**配合 `if constexpr` 在模板里根据类型走不同编译期分支**。

---

## 第 5 页｜类型萃取的实现原理

用 **模板偏特化** 和 **SFINAE** 来获取类型特征，例如 `is_integral` 的实现：

```cpp
// 简化版：枚举所有整数类型
template<typename T>
struct is_integral : std::false_type {};
template<>
struct is_integral<char> : std::true_type {};
template<>
// ... 枚举所有整数类型，int、long、long long、unsigned int 等。
// 使用萃取
static_assert(is_integral<int>::value);
```

事实上，最初标准库也是通过这种方式实现的类型萃取。而后来为了追求更好的编译性能，用了一些**intrinsic**（编译器内建指令，比如`__is_integral(_Tp)`）替代，以避免使用模板实例化带来的开销。

我们开发者可以通过同样的方式去灵活实现自己想要的类型萃取。

---

## 第 6 页｜类型萃取的使用

有了类型萃取这一强大的工具，我们就可以在编译期做很多有用的工作。也是一个比较实际的例子。

**例一：编译期根据类型特征选择最合适的分支代码**

```cpp
template <typename T>
void serialize(T& obj) {
    if constexpr (std::is_trivially_copyable_v<T>) {
        // T 可以直接 memcpy，无需逐字节序列化
        std::memcpy(buffer, &obj, sizeof(T));
    } else {
        // T 有自定义析构或拷贝构造，必须逐字段序列化
        obj.serialize_to(buffer);
    }
}
```

编译期根据类型选择分支的另外一个好处是消除了运行时的判断，在编译好的代码中是没有这个 `if` 判断的。

**例二：**
```cpp
template <typename T>
std::enable_if_t<std::is_arithmetic_v<T>, void>
process(T value) {
    std::cout << "此函数模板仅支持可以进行算术运算的类型: " << value << "\n";
}
```

---

## 第 7 页｜Concept 登场——模板约束的现代化

**痛点回顾：** 在 Concept 出现之前，需要用 SFINAE 和 enable_if 等手段实现模板元编程中的类型约束，写法非常晦涩：

```cpp
// 古老的 SFINAE 写法
template <typename T>
std::enable_if_t<std::is_arithmetic_v<T>, void>
process(T value) { ... }
```

**问题：**
原本函数的返回类型就是简单的 void ，现在还要加上一段 enable_if 代码，看起来就很不直观。而且这种方式下产生的错误信息，经常是一大堆模板展开时的错误信息，并没有指向出问题的地方，相当于是大量噪音，干扰了代码修正工作。

**C++20 的解决方案：Concept**

> Concept 是一种**命名了的编译期谓词**，
> 用来约束模板参数必须满足的条件。

---

## 第 8 页｜使用 Concept

### 使用标准库 Concept 约束模板

C++20 标准库提供了大量现成的 Concept，如 `std::integral`、`std::floating_point`、`std::same_as` 等。直接用它们约束模板，三种语法都可以：

```cpp
// 语法1：concept 作为类型约束（最简洁）
template <std::integral T>
T double_value(T x) {
    return x * 2;
}

// 语法2：用 requires 子句
template <typename T>
    requires std::integral<T>
T double_value(T x) {
    return x * 2;
}

// 语法3：尾部 requires
template <typename T>
T triple(T x) requires std::integral<T> {
    return x * 3;
}
```

我最喜欢的还是第1种语法，一般情况下用第1种语法就行了。

**核心结论：**

> 代码意图一目了然。

---

## 第 9 页｜定义 Concept

标准库 Concept 覆盖了常见场景，但特定需求还需要自己定义。自定义 Concept 有两种方式：

### 方式一：组合 type traits

```cpp
template <typename T>
concept Numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;
```

### 方式二：requires 表达式

`requires` 表达式内部可以写**多项检查**，全部满足才通过：

```cpp
template <typename T>
concept Regular = requires(T a, T b, T c) {
    T{a};    // 拷贝构造
    a = b;   // 拷贝赋值
    a == b;  // 相等比较
    a != b;  // 不相等比较
};
```

---

## 第 10 页｜实战：智能选择算法

### 问题
有些容器只支持仅向前遍历，而有些容器支持随机访问。如何根据容器能力自动选择最优排序算法？

### 解决方案：Concepts + Subsumption

```cpp
#include <concepts>
#include <iostream>
#include <vector>
#include <list>

// 前向迭代器版本（适用于 list）
// 使用插入排序/归并排序
template<std::forward_iterator Iter>
void sort(Iter first, Iter last) {
    std::cout << "forward_iterator sort (insertion/merge)" << std::endl;
}

// 随机访问迭代器版本（适用于 vector）
// 使用快速排序/堆排序
template<std::random_access_iterator Iter>
void sort(Iter first, Iter last) {
    std::cout << "random_access sort (quick/heap)" << std::endl;
}

int main() {
    std::list<int> l = {3, 1, 2};
    std::vector<int> v = {3, 1, 2};
    
    sort(l.begin(), l.end());  // 自动选择 forward 版本
    sort(v.begin(), v.end());  // 自动选择 random_access 版本
}
```

### 效果
- **一套接口**：统一调用 `sort()`
- **自动适配**：编译器根据迭代器能力选择最优实现
- **零运行时开销**：纯编译期决策

---

## 第 11 页｜Concept 的蕴含关系——约束越严格，匹配越优先

Concept 的引入，让我们有了更智能的代码选择的方法。

### Subsumption（蕴含关系）

**核心概念**：约束严格程度决定调用哪个版本

**给定如下蕴含关系**：
```
signed_integral ⊂ integral ⊂ arithmetic
```

编译器在编译以下代码的时候，会按照蕴含关系的从严格到宽松的优先级，选择合适的函数模板。

```cpp
template<std::integral T>        // 所有整数
void process(T);

template<std::signed_integral T> // 有符号整数（更严格）
void process(T);

process(42);   // 调用 signed_integral 版本（更严格）
process(42u);  // 调用 integral 版本（唯一匹配）
```

---

## 第 12 页｜总结

### 类型萃取的贡献

- ✅ 让编译期"看见"类型的特性
- ✅ 为模板选择不同的处理逻辑
- ✅ 对类型进行抽取、变换
- ✅ 实现零开销静态多态

### Concept 的贡献

- ✅ 让模板约束从隐式变显式
- ✅ 大幅改善错误信息质量
- ✅ 约束即文档，提升代码可读性
- ✅ C++20 最重要的新特性之一

### 核心结论

> 类型萃取在编译期获得类型特征，抽取、变换类型，
> Concept 在编译期约束模板参数。
> 两者合在一起，让 C++ 在编译期拥有类型掌控能力——
> 不需要耗费运行期的一个字节、一个时钟周期。
>
> 这正是零开销抽象哲学的实践。

---

### 下期预告

**C++20 Ranges —— 用管道思维打开编程新模式**
我们将探讨 C++20 Ranges 如何改造标准库算法，让管道式编程风格与现代 C++ 完美融合。

---

GitHub 示例代码：https://github.com/Alex-Cheng/qianxin_cpp/

谢谢大家。
