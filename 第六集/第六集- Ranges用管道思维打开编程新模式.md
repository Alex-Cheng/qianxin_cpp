# C++20 Ranges —— 用管道思维打开编程新模式

## 引言

首先问个问题，如何编写代码完成以下任务？

> 给定数据集，按照如下过程处理：过滤出偶数 → 每个数平方 → 取前 3 个 → 倒序输出？

传统的写法：

```cpp
std::vector<int> data{1, 3, 2, 8, 7, 4, 6, 5};

std::vector<int> temp_even;
temp_even.reserve(data.size());

//  Step 1: 过滤偶数（用迭代器而不是范围for）
for (std::vector<int>::iterator it = data.begin(); it != data.end(); ++it) {
    if (*it % 2 == 0) {
        temp_even.push_back(*it);
    }
}

std::vector<int> temp_squared;
temp_squared.reserve(temp_even.size());

// Step 2: 求平方
for (std::vector<int>::iterator it = temp_even.begin(); 
     it != temp_even.end(); ++it) {
    temp_squared.push_back((*it) * (*it));
}

std::vector<int> temp_first_three;

// Step 3: 取前3个（手动检查边界）
if (temp_squared.size() >= 3) {
    temp_first_three.push_back(temp_squared[0]);
    temp_first_three.push_back(temp_squared[1]);
    temp_first_three.push_back(temp_squared[2]);
} else {
    for (unsigned int i = 0; i < temp_squared.size(); ++i) {
        temp_first_three.push_back(temp_squared[i]);
    }
}

std::vector<int> result;

// Step 4: 倒序（不用 std::reverse）
for (int i = static_cast<int>(temp_first_three.size()) - 1; i >= 0; --i) {
    result.push_back(temp_first_three[i]);
}

// 输出结果
for (std::vector<int>::iterator it = result.begin(); it != result.end(); ++it) {
    std::cout << *it << " ";  // 输出: 16 64 4
}
```

审视上面这段代码：为了完成"过滤偶数 → 求平方 → 取前3个 → 倒序"这个简单任务，我们写了 4 个临时容器、多次循环，计算逻辑也比较零散。每个操作步骤的中间结果的临时保存导致频繁分配内存、拷贝数据，性能大打折扣。

在以前，我们经常不得不写出这样的代码，但是有了 C++20 Ranges 之后，同样的逻辑可以用声明式管道一气呵成，就像如下代码：

```cpp
std::vector<int> data{1, 3, 2, 8, 7, 4, 6, 5};
auto result = data
    | std::views::filter([](int x) { return x % 2 == 0; })  // 过滤偶数
    | std::views::transform([](int x) { return x * x; })     // 求平方
    | std::views::take(3)                                     // 取前 3 个
    | std::views::reverse;                                    // 倒序
for (int x : result)
    std::cout << x << " ";  // 输出: 16 64 4
```

这段代码的特点非常鲜明——看起来很像一节节管道相连：**简洁直观、没有临时容器（零拷贝）、单次循环遍历、声明式风格**。这就是 Ranges 带来的编程新模式。

## C++20 的新特性 - Ranges 简介

C++20 Ranges 将函数式编程中的 **管道模式** 引入标准库，通过 `|` 运算符串联 `filter`、`transform`、`take` 等范围适配器，开发者可以用声明式风格组装惰性求值的数据处理流水线——**简洁直观、零拷贝、单次遍历、按需计算**。

本文从管道模式的设计思想出发，覆盖 Ranges 核心组件（Concept、适配器、视图、Projection），并且通过实测展现了在适当场景下对性能的显著提升。随后从跨语言视角（Shell、Python、Rust 等）阐述管道模式的通用设计思想，说明这一范式并非 C++ 独创，而是现代编程语言的共同选择。最后讨论悬垂引用、const 语义等常见陷阱与编程实践注意事项。读完本文，你将获得一种新的技能——在数据处理场景中，用声明式管道替代指令式循环。

## Ranges 详解

### 要点

#### 1. `std::ranges::range` 概念（Concept）

`range` 概念约束某类型为 range，即它提供了 begin 迭代器和 end 哨兵。所有标准容器（`vector`、`list`、`map` 等）天然就是 range。

这体现了"鸭子类型"（Duck Typing）的思想：不关心对象的继承关系，只关心它的 **表现行为** ——能获取 begin 迭代器和 end 哨兵，符合这样的行为的，就是 range。

> 鸭子类型：如果看到一只鸟走起来像鸭子、游泳起来像鸭子、叫起来也像鸭子，那么这只鸟就可以被称为鸭子。

`range` 概念（在之前的视频中介绍过）让函数模板可以清晰约束"我接受的是一个范围range"。

#### 2. 视图（View）概念

`view` 概念约束某 range 为 view，即其拷贝、移动、赋值操作具有常数时间复杂度（O(1)）。这意味着 view 必须是轻量的——这也是为什么 `std::vector` 不是 view（拷贝它是 O(n)），而 `std::string_view` 是（拷贝它只复制一个指针和一个长度）。

> **View = 数据上的一副"眼镜"** —— 你不移动数据，只是换了一副镜片去看同一个数据集，看到的形状（过滤、变换、反转）不一样，但客观存在的数据并不会改变。

大多数标准 view 还具备以下实用特征：
- **惰性求值（Lazy）**：遍历时才计算，之前只是声明
- **不拥有数据（Non-owning）**：只是对源 range 的"观察"，源销毁则 view 失效

#### 3. 范围适配器（Range Adaptor）和范围工厂（Range Factory）

范围适配器是一个函数对象，通过管道 `|` 与 range 组合后产生视图（view）。以下代码中，`filter` 和 `transform` 就是两个适配器：

```cpp
std::vector<int> v{1,2,3,4,5,6};
auto result = v | std::views::filter([](int x) { return x % 2 == 0; })
                | std::views::transform([](int x) { return x * x; });
```

适配器链上的所有操作都是惰性的——只有在最终遍历 `result` 时，数据才会依次流经 `filter` → `transform`。可以用一个现实类比来理解：电源适配器将 220V 交流电变为 5V 直流电（做了变换），供电站的电流仍是 220V 交流电（不改变输入源），不接电器时电流不流动（惰性）。范围适配器同样如此：对数据做变换，不改变上游源数据，不遍历则不执行。

作为经常用的两个范围适配器，`filter` 通过传入谓词（比如一个函数对象）定制过滤条件，`transform` 通过传入函数定制变换规则。适配器的"可定制化"正体现于此：同一个适配器，传入不同的自定义逻辑，产生完全不同的行为。

范围工厂则不需要输入 range，自己"无中生有"产生数据序列。`std::views::iota` 是最典型的工厂——`iota(1)` 生成 1, 2, 3, ... 的等差序列，配合适配器即可构成完整的数据处理流水线。

以下是常用的标准库适配器和工厂：

| 类型 | 名称 | 功能 |
|------|------|------|
| 适配器 | `views::filter` | 过滤元素 |
| 适配器 | `views::transform` | 映射变换 |
| 适配器 | `views::take` / `views::drop` | 取前/跳过前 N 个 |
| 适配器 | `views::reverse` | 反转 |
| 适配器 | `views::split` / `views::join` | 拆分/合并 |
| 工厂 | `views::iota` | 生成等差序列 |
| 工厂 | `views::single` | 单元素视图 |

#### 4. `std::ranges::algorithm` —— Range 版算法

所有 STL 算法的 ranges 版本，可直接接受 range 而不需要传 `begin()`/`end()` 迭代器对：

```cpp
std::ranges::sort(v);           // C++20 ranges 版本
auto it = std::ranges::find(v, 42);
```

此外，所有 ranges 算法还附带一个语法糖：**投影（Projection）** 参数。对自定义类型排序时不再需要手写 lambda：

```cpp
// 传统写法：手写 lambda 比较 age
std::sort(people.begin(), people.end(),
    [](const Person& a, const Person& b) { return a.age < b.age; });

// Ranges + Projection：第二个参数是比较器，第三个是投影
std::ranges::sort(people, {}, &Person::age);
```

`{}` 表示默认比较器（`std::less`），投影参数告诉算法"比较前先取什么字段"。它将 **"取什么"** 与 **"怎么比"** 解耦，`min_element`、`max_element` 等所有 ranges 算法都支持这一语法。

---

## 示例代码

本文中的所有示例代码可在以下仓库中找到：

> **GitHub:** [Alex-Cheng/qianxin_cpp](https://github.com/Alex-Cheng/qianxin_cpp)
>
> 示例目录：`第六集/samples/`

---

## 管道模式（Pipeline Pattern）

Ranges 背后的设计思想是**管道模式**——将复杂的数据处理流程拆解为一组独立的、可组合的阶段，数据依次流经每个阶段，每个阶段只完成单一变换。

### 本质

管道模式并不是 C++ 独创。它是 **函数式编程中"函数组合"与"不可变数据流"思想在数据处理领域的具体应用**：

- **函数组合（Function Composition）**：`h(x) = f(g(x))` 的泛化，多个小函数拼接成一个大函数。管道中的 `| filter | transform` 本质上就是函数组合。
- **不可变数据流（Immutable Data Flow）**：数据流经每个阶段时不被原地修改，每个阶段产出新结果。这消除了副作用，让每个阶段可独立测试和推理。

### 跨语言实践

几乎所有主流语言都有管道模式的影子：

| 语言 | 实现形式 | 示例 |
|------|---------|------|
| C++20 | Ranges | `v \| std::views::filter(f) \| std::views::transform(g)` |
| Java | Stream API | `stream.filter(f).map(g).collect(toList())` |
| C# | LINQ | `list.Where(f).Select(g).ToList()` |
| Rust | Iterator Combinators | `iter.filter(f).map(g).collect()` |
| JavaScript | 函数式组合 / RxJS | `compose(g, f)(data)` |
| Python | itertools 生成器 | `(x*x for x in data if x%2==0)` |
| Go | Channel 管道 | `ch1 → ch2 → ch3` |
| Ruby | 集合链式调用 | `arr.select{|x|...}.map{|x|...}` |
| Elixir | `|>` 管道操作符 | `data |> f() |> g()` |
| Unix Shell | 命令行管道 | `cat file \| grep "err" \| wc -l` |

### 四要素

管道模式由四个核心要素构成：

1. **数据源（Source）** — 产生初始数据流的起点（range）
2. **管道（Pipe）** — 连接各阶段的通道（`|` 操作符）
3. **阶段/过滤器（Stage/Filter）** — 每个阶段对数据做单一变换（filter、transform、take 等）
4. **汇（Sink）** — 数据流的终点，收集最终结果（遍历消费）

以上四要素，在 C++ Ranges 中都有内容与之对应。

---

## Ranges 的两大优势

### 1. 声明式编程——让代码"自注释"

适配器链让复杂的过滤/变换逻辑可以声明式地一层层组装，而不是堆叠嵌套循环、临时容器和散落的 if 判断。这是编程理念上的升级：

```cpp
// 指令式（Imperative）——怎么干
std::vector<int> temp;
for (auto& x : data)
    if (x > 0) temp.push_back(x * 2);
std::vector<int> result;
for (size_t i = 0; i < std::min(temp.size(), 5u); ++i)
    result.push_back(temp[i]);

// 声明式（Declarative）——干什么
auto result = data
    | std::views::filter([](int x) { return x > 0; })
    | std::views::transform([](int x) { return x * 2; })
    | std::views::take(5);
```

声明式代码直接映射到数据流图——从上到下一遍就能理解完整的处理链，而不是在循环体和条件判断中追踪状态变化。阅读者看到的不是"怎么实现的"，而是"想表达什么"。

### 2. 性能——零额外开销的数据处理

Ranges 的性能优势不是来自算法本身更快，而是源于两个架构级特征：

**视图组合无中间容器。** 管道串联的每一步只是产出一个轻量 view 对象——不分配、不拷贝数据。这与惰性求值是两回事：前者管"怎么存"（view 本身就不存数据），后者管"何时算"（遍历时才执行）：

```cpp
// 传统方式：每次变换都分配新容器
auto temp1 = filter(data, pred);   // 分配容器 A
auto temp2 = transform(temp1, f);  // 分配容器 B
auto temp3 = take(temp2, n);       // 分配容器 C

// Ranges 方式：零额外分配
auto result = data | std::views::filter(pred) | std::views::transform(f) | std::views::take(n);
// 遍历时：取出一个元素 → 判断过滤 → 变换 → 计数 → 送出/跳过
```

对于百万级元素的数据集，省去中间容器的内存分配和拷贝，性能差异是数量级的。

**单次遍历代替多次遍历。** 传统方式每个阶段独立遍历一次数据。Ranges 管道中，所有阶段按元素粒度交织执行——每个元素一次走完整条管道：

```
遍历循环（仅一次）
  元素 → filter通过? → transform变换 → take计数 → 下个元素
```

**实测数据：** 在一千万条 double 上执行 `filter → transform → take` 操作链：

| 指标 | 传统写法 | Ranges 管道 |
|------|---------|-------------|
| 耗时 | **603 ms** | **55 ms** |
| 中间容器 | 3 个（约 1450 万个 double） | **0 个** |
| 遍历次数 | 3 次 | **1 次** |
| 加速比 | — | **11.0×** |

数据量越大，零拷贝的优势越明显。编译期内联展开保证了零运行时开销——这是典型的 C++ 零成本抽象。

另外，C++23 的 `std::ranges::to<T>` 打通了最后一公里：视图物化回容器只需一行代码，惰性加工和持久化存储无缝衔接：

```cpp
auto v = data | filter(...) | transform(...) | std::ranges::to<std::vector<int>>();
```

> 完整 benchmark 源码见 `work/第六集/samples/07_performance.cpp`

---

## 常见陷阱与注意事项

Ranges 虽好，但有几个坑需要清楚：

### 1. 悬垂引用（Dangling）

视图不拥有数据——它只是对源 range 的"观察"。源 range 销毁后，视图立即变为悬垂引用，再遍历就是未定义行为：

```cpp
auto dangerous() {
    std::vector<int> v{1, 2, 3, 4, 5};
    return v | std::views::filter([](int x) { return x % 2 == 0; });
    // 危险！v 随函数返回被析构，返回的 view 悬垂!
}
```

**对策**：如果需要将结果传出作用域，在函数内用 `std::ranges::to<T>()` 物化到容器后再返回。

### 2. 视图的 const 语义陷阱

某些视图（如 `filter_view`）的 `begin()` 需要内部缓存以满足 amortized O(1) 的要求。如果给视图加上 `const`，`begin()` 会被禁止修改缓存，导致无法编译或返回错误的迭代器类型：

```cpp
const auto view = data | std::views::filter(pred);
// view.begin() 可能编译失败（取决于具体视图类型）
```

**对策**：非必要不对视图加 `const`。如果需要只读语义，对源数据加 `const`，而非对视图。

### 3. 输入 Range 的一次性特性

基于 input_iterator 的视图（如读取网络流的 `istream_view`）只能遍历一次。一旦消费完，视图就"空"了：

```cpp
auto v = std::views::istream<int>(std::cin) | std::views::take(5);
for (int x : v) std::cout << x << " ";  // 正常
for (int x : v) std::cout << x << " ";  // 空循环！
```

**对策**：如果需要对数据做多次遍历，先用 `std::ranges::to<T>()` 物化到容器中。

---

## 总结

C++20 Ranges 带来的不只是一套新 API，而是一种思维方式的转变：

- **从"怎么做"到"做什么"**：从指令式循环转向声明式管道
- **从容器思维到视图思维**：数据不动，只变换观看角度
- **从立即计算到惰性求值**：需要时才计算，按需消耗资源
- **管道模式**：这一思想跨越语言边界，已经成为现代编程中的通用范式

Ranges 让你用数据流代替控制流，用意图代替步骤。
