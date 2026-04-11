# 第五集配套代码示例

> 类型萃取与 Concept —— 在编译期掌控类型
> GitHub: https://github.com/Alex-Cheng/qianxin_cpp/

---

## 目录结构

```
samples/
├── 01_type_traits_basics.cpp     # 类型萃取基础
├── 02_type_transformations.cpp  # 类型变换
├── 03_custom_type_trait.cpp      # 手写类型萃取
├── 04_requires_expression.cpp    # requires 表达式
├── 05_standard_concepts.cpp     # 标准库 Concept
├── 06_custom_concept.cpp         # 自定义 Concept
├── 07_combination.cpp            # 类型萃取 + Concept 组合拳
└── CMakeLists.txt                # 构建配置
```

---

## 示例代码

### 01_type_traits_basics.cpp — 类型萃取基础

```cpp
#include <type_traits>
#include <iostream>

int main() {
    // 基本类型检测
    static_assert(std::is_integral_v<int>);
    static_assert(!std::is_integral_v<double>);
    static_assert(std::is_pointer_v<int*>);
    static_assert(!std::is_pointer_v<int>);

    // 类型比较
    static_assert(std::is_same_v<int, int>);
    static_assert(!std::is_same_v<int, unsigned int>);

    // 布尔常量
    constexpr bool is_int = std::is_integral_v<int>;
    constexpr bool is_float = std::is_floating_point_v<double>;

    std::cout << "is_int: " << is_int << "\n";
    std::cout << "is_float: " << is_float << "\n";

    return 0;
}
```

### 02_type_transformations.cpp — 类型变换

```cpp
#include <type_traits>
#include <typeinfo>
#include <iostream>

int main() {
    // 去 const / 去引用
    using T1 = std::remove_const<const int>::type;       // int
    using T2 = std::remove_reference<int&>::type;      // int
    using T3 = std::remove_reference<const int&>::type; // int

    // 组合使用
    using T4 = std::remove_const<std::remove_reference<const int&>::type>::type; // int

    // 标准库别名版本（更简洁）
    using T5 = std::remove_const_t<const int>;           // int
    using T6 = std::remove_reference_t<int&>;            // int
    using T7 = std::remove_cvref_t<const int&>;           // int (C++20)

    // decay：按值传递规则变换
    using T8  = std::decay_t<int>;          // int
    using T9  = std::decay_t<int&>;         // int
    using T10 = std::decay_t<const int*>;   // const int*
    // 注意：数组 decay 为指针，函数 decay 为函数指针

    // conditional：条件选择类型
    using T11 = std::conditional_t<true, int, double>;   // int
    using T12 = std::conditional_t<false, int, double>;  // double

    // add_pointer：添加指针
    using T13 = std::add_pointer_t<int>;    // int*
    using T14 = std::add_pointer_t<int&>;   // int*（引用先 decay）

    std::cout << "Type transformations demo done.\n";
    return 0;
}
```

### 03_custom_type_trait.cpp — 手写类型萃取

```cpp
#include <type_traits>
#include <iostream>

// ========== 示例1：检测是否为指针 ==========
template <typename T>
struct IsPointer : std::false_type {};

template <typename T>
struct IsPointer<T*> : std::true_type {};

static_assert(IsPointer<int*>::value == true);
static_assert(IsPointer<int>::value == false);

// ========== 示例2：检测是否有 value_type 成员 ==========
template <typename T, typename = void>
struct HasValueType : std::false_type {};

template <typename T>
struct HasValueType<T, std::void_t<typename T::value_type>> : std::true_type {};

// 验证：vector 有 value_type，int 没有
static_assert(HasValueType<std::vector<int>>::value == true);
static_assert(HasValueType<int>::value == false);

// ========== 示例3：检测是否支持 + 运算 ==========
template <typename T, typename = void>
struct HasPlus : std::false_type {};

template <typename T>
struct HasPlus<T, std::void_t<decltype(std::declval<T>() + std::declval<T>())>>
    : std::true_type {};

static_assert(HasPlus<int>::value == true);
static_assert(HasPlus<double>::value == true);
static_assert(HasPlus<std::string>::value == true);
static_assert(HasPlus<int*>::value == false); // 指针不支持 +

// ========== 示例4：综合——可迭代性检测 ==========
template <typename T>
struct IsIterable : std::bool_constant<
    requires(T t) {
        std::begin(t);
        std::end(t);
    }
> {};

static_assert(IsIterable<std::vector<int>>::value == true);
static_assert(IsIterable<int[]>::value == true);
static_assert(IsIterable<int>::value == false);

int main() {
    std::cout << "IsPointer<int*>: " << IsPointer<int*>::value << "\n";
    std::cout << "IsPointer<int>:   " << IsPointer<int>::value << "\n";
    std::cout << "HasPlus<int>:     " << HasPlus<int>::value << "\n";
    std::cout << "HasPlus<int*>:    " << HasPlus<int*>::value << "\n";
    std::cout << "IsIterable<vector<int>>: " << IsIterable<std::vector<int>>::value << "\n";
    std::cout << "IsIterable<int>:        " << IsIterable<int>::value << "\n";
    return 0;
}
```

### 04_requires_expression.cpp — requires 表达式

```cpp
#include <type_traits>
#include <iostream>
#include <vector>
#include <list>

// ========== requires 表达式的基本用法 ==========
template <typename T>
concept Addable = requires(T a, T b) {
    a + b;
};

template <typename T>
concept Comparable = requires(T a, T b) {
    a < b;
    a == b;
};

// ========== 检测更复杂的操作 ==========
template <typename T>
concept PushBackable = requires(T t) {
    t.push_back(*t.begin());
};

template <typename T>
concept EmplaceConstructible = requires(T* p) {
    new (p) T;
};

// ========== 组合多个 requires ==========
template <typename T>
concept Arithmetic = Addable<T> && Comparable<T>;

// ========== requires 列表中可以包含类型约束 ==========
template <typename T>
concept VectorLike = requires(T t) {
    typename T::value_type;   // 必须有 value_type
    typename T::iterator;    // 必须有 iterator
    { t.size() } -> std::convertible_to<std::size_t>;  // size() 返回可转 size_t 的类型
    { t.begin() };
    { t.end() };
};

int main() {
    static_assert(Addable<int>);
    static_assert(!Addable<int*>);
    static_assert(Comparable<int>);
    static_assert(Arithmetic<int>);
    static_assert(PushBackable<std::vector<int>>);
    static_assert(!PushBackable<std::list<int>>);
    static_assert(VectorLike<std::vector<int>>);

    std::cout << "All static_assert passed.\n";
    return 0;
}
```

### 05_standard_concepts.cpp — 标准库 Concept

```cpp
#include <concepts>
#include <type_traits>
#include <iostream>
#include <memory>

// 标准库 concept 的使用示例

template <typename T>
    requires std::integral<T>
T count_bits(T value) {
    int count = 0;
    while (value) {
        count += value & 1;
        value >>= 1;
    }
    return count;
}

template <typename T>
    requires std::floating_point<T>
T safe_sqrt(T x) {
    return x >= 0 ? x : -x;
}

template <typename T>
    requires std::movable<T>
class SmartPtr {
    T* ptr_;
public:
    explicit SmartPtr(T* p) : ptr_(p) {}
    ~SmartPtr() { delete ptr_; }
    SmartPtr(SmartPtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    T& operator*() const { return *ptr_; }
};

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
};

int main() {
    std::cout << "count_bits(15): " << count_bits(15) << "\n";        // 4
    std::cout << "safe_sqrt(-4.0): " << safe_sqrt(-4.0) << "\n";       // 4.0

    SmartPtr<int> sp(new int(42));
    std::cout << "SmartPtr value: " << *sp << "\n";

    // 编译期验证
    static_assert(std::integral<int>);
    static_assert(!std::integral<double>);
    static_assert(std::movable<int>);
    static_assert(!std::movable<NonCopyable>);

    return 0;
}
```

### 06_custom_concept.cpp — 自定义 Concept

```cpp
#include <concepts>
#include <type_traits>
#include <iostream>
#include <string>

// ========== 自定义 Concept 示例 ==========

// 1. 基于 std::is_xxx_v 的简单 concept
template <typename T>
concept Numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

// 2. 基于 requires 表达式的精确描述
template <typename T>
concept Addable = requires(T a, T b) {
    a + b;
    a - b;
};

template <typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
};

// 3. 组合 concept
template <typename T>
concept Arithmetic = Addable<T> && Comparable<T>;

// 4. 带有返回值约束的 concept
template <typename T>
concept HasAbs = requires(T t) {
    { std::abs(t) } -> std::same_as<T>;
};

// 5. 检测容器特性
template <typename C>
concept Container = requires(C c) {
    typename C::value_type;
    typename C::iterator;
    { c.begin() } -> std::same_as<typename C::iterator>;
    { c.end() }   -> std::same_as<typename C::iterator>;
    { c.size() }  -> std::convertible_to<std::size_t>;
};

// ========== 使用自定义 Concept ==========

template <Numeric T>
T square(T x) { return x * x; }

template <Addable T>
T sum(T a, T b) { return a + b; }

template <Arithmetic T>
T max_value(T a, T b) { return (a < b) ? b : a; }

template <Container C>
typename C::size_type count_elements(const C& c) {
    return c.size();
}

int main() {
    std::cout << "square(5):        " << square(5) << "\n";
    std::cout << "square(3.14):     " << square(3.14) << "\n";
    std::cout << "sum(1, 2):        " << sum(1, 2) << "\n";
    std::cout << "sum(1.5, 2.5):    " << sum(1.5, 2.5) << "\n";
    std::cout << "max_value(3, 7):  " << max_value(3, 7) << "\n";
    std::cout << "max_value(1.1, 2.2): " << max_value(1.1, 2.2) << "\n";
    std::cout << "count_elements(vec): " << count_elements(std::vector<int>{1,2,3}) << "\n";

    return 0;
}
```

### 07_combination.cpp — 类型萃取 + Concept 组合拳

```cpp
#include <type_traits>
#include <concepts>
#include <iostream>
#include <vector>
#include <array>
#include <string>

// ========== Part 1: Concept 做接口约束 ==========

template <typename T>
concept AddableComparable = requires(T a, T b) {
    a + b;
    a < b;
};

template <AddableComparable T>
T max_value(T a, T b) {
    return (a < b) ? b : a;
}

// ========== Part 2: 类型萃取做内部实现 ==========

template <typename Container>
auto serialize(const Container& c) {
    // 用类型萃取检测容器是否有 reserve 方法
    if constexpr (requires { c.reserve(0); }) {
        // 有 reserve，提前分配
        std::vector<typename Container::value_type> out;
        out.reserve(c.size());
        for (const auto& item : c) {
            out.push_back(item);
        }
        return out;
    } else {
        // 没有 reserve，直接拷贝
        return std::vector<typename Container::value_type>(c.begin(), c.end());
    }
}

// ========== Part 3: 条件编译分支 ==========

template <typename T>
auto process(T value) {
    if constexpr (std::integral<T>) {
        return value * 2;
    } else if constexpr (std::floating_point<T>) {
        return value * 2.0;
    } else if constexpr (requires { typename T::value_type; }) {
        // 如果是容器类型
        return static_cast<typename T::size_type>(value.size()) * 2;
    } else {
        static_assert(std::integral<T> || std::floating_point<T>
            || requires { typename T::value_type; },
            "Unsupported type");
        return value;
    }
}

// ========== Part 4: 静态断言门禁 ==========

template <typename T>
T safe_add(T a, T b) {
    static_assert(std::is_arithmetic_v<T>,
        "safe_add only supports arithmetic types (int, float, double, etc.)");
    return a + b;
}

// ========== Part 5: 类型萃取的智能实现选择 ==========

template <typename T>
struct Serializer {
    // 使用类型萃取在编译期选择序列化策略
    static std::string serialize(const T& value) {
        if constexpr (std::is_same_v<T, int>) {
            return "int:" + std::to_string(value);
        } else if constexpr (std::is_same_v<T, double>) {
            return "double:" + std::to_string(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "string:" + value;
        } else {
            return "unknown";
        }
    }
};

int main() {
    // Part 1
    std::cout << "max_value(3, 7):  " << max_value(3, 7) << "\n";
    std::cout << "max_value(1.1, 2.2): " << max_value(1.1, 2.2) << "\n";

    // Part 2
    std::vector<int> v = {1, 2, 3};
    auto s1 = serialize(v);  // 使用 reserve 分支
    std::cout << "serialized vector size: " << s1.size() << "\n";

    std::array<int, 3> a = {1, 2, 3};
    auto s2 = serialize(a);  // 使用普通拷贝分支
    std::cout << "serialized array size: " << s2.size() << "\n";

    // Part 3
    std::cout << "process(10):   " << process(10) << "\n";   // 20
    std::cout << "process(3.0):   " << process(3.0) << "\n"; // 6.0
    std::cout << "process(vec):   " << process(v) << "\n";  // 6 (size*2)

    // Part 4
    std::cout << "safe_add(1, 2): " << safe_add(1, 2) << "\n";

    // Part 5
    std::cout << Serializer<int>::serialize(42) << "\n";
    std::cout << Serializer<double>::serialize(3.14) << "\n";
    std::cout << Serializer<std::string>::serialize("hello") << "\n";

    return 0;
}
```
