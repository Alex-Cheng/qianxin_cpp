# 零成本抽象——C++的设计哲学

## 封面

现代C++的发展道路

从哲学到实践

第三篇：RAII —— 让资源管理自动、确定、零泄漏

一门语言的长青之道

---

## 第 1 页｜RAII 的意义

- 为了实现零成本抽象，C++ 放弃了垃圾回收（GC）。
- 那么，在没有 GC 的前提下，如何仍然可靠地管理资源？
- C++ 给出的答案是：RAII（Resource Acquisition Is Initialization）。

---

## 第 2 页｜RAII 的基本原理

- 栈对象承担资源的所有权（Ownership）
- 资源获取 → 构造函数  
- 资源释放 → 析构函数  
- 作用域决定生命周期 - 左花括号时构造，右花括号时析构

**核心机制：**  
> Ownership除了所有权，还意味着责任。
> 当程序执行离开栈对象的作用域（通俗的说就是遇到了右花括号 } ）时，对象被自动且确定地析构。

---

## 第 3 页｜所有权 / 责任

- 独占所有权，可转让不可复制
- 共享所有权，可复制但通常需要计数器
- 独占所有权常见
  
**核心机制：**

> Ownership 描述的不是“谁能用资源”，
> 而是“谁对资源的生命周期负责”。

---

## 第 4 页｜标准库里的 RAII

典型例子：

- `std::lock_guard`（锁）
- `std::unique_ptr` / `std::shared_ptr`（内存与所有权）
- `std::ifstream / ofstream`（文件）
- `std::jthread`（线程，C++20）

**核心结论：**  
> 标准库广泛采用 RAII，
> 将易错的资源管理，封装为安全的作用域对象。

---

## 第 5 页｜RAII 与异常安全

- 异常路径是资源泄漏最常见、也最隐蔽的来源  
- 传统做法需要覆盖所有返回与异常路径  
- **RAII 的保证：异常不会跳过析构**  
- 无需 `try / finally`

**核心结论：**  
> RAII 让异常路径与正常路径，  
> 遵循同一套资源释放规则。

---

## 第 6 页｜RAII 与异常安全（试金石）

- 异常路径是资源泄漏的主要来源  
- RAII 的保证：异常不会跳过析构  
- 无需 `try / finally`，无需 GC  

**核心结论：**  
> RAII 让异常不再是资源安全的“特殊情况”。

---

第 x 页|其他编程语言保证资源安全的方法以及与RAII方式的对比

```csharp
FileStream fs = null;
try
{
    fs = new FileStream("test.txt", FileMode.Open);
    // 使用fs
    // ...
}
finally
{
    if (fs != null)
    {
        fs.Close();   // 一定会执行
    }
}
```

对应的C++的RAII方式更简洁。
```c++
std::ifstream ifs("test.txt");
if (ifs) {
    // 使用 ifs
    // ...
}
```

如果是C风格的文件读写，可以这样。
```c++
auto deleter = [](FILE* f) {
    if (f) std::fclose(f);
};

std::unique_ptr<FILE, decltype(deleter)> fs(
    std::fopen("test.txt", "r"),
    deleter
);

// 使用 fs
```

---

## 第 7 页|工程中使用RAII

- 尽量不要使用裸new，而是选择std::make_unique()
- std::unique_ptr性能上与裸指针T*基本持平
- 特殊情况下，使用std::make_shared()
- 内存之外的资源，用定制了deleter的std::unique_ptr或者std::shared_ptr管理
- 必须的情况，再自己实现RAII类

---

## 第 8 页|应用实例 - ObjectARX的资源释放

resbuf 是 AutoCAD 用来表示“变长、异构数据列表”的动态链表结构，通常由 ObjectARX 在堆上分配，必须显式释放。
以前的做法是手工释放：

``` 
int main()
{
    resbuf* rb = nullptr;

    try {
        rb = acutBuildList(
            RTSHORT, 1,
            RTSTR, _T("Hello"),
            RTNONE
        );

        // 使用 rb
        // 任何这里抛出的异常，都必须被兜住
        // ...

        if (/*提前退出条件*/) {
            acutRelRb(rb);
            rb = nullptr;
            return;
        }
    }
    catch (...) {
        if (rb) {
            acutRelRb(rb);   // 手动释放（异常路径）
            rb = nullptr;
        }
        // 处理异常
    }

    // 正常路径也必须释放
    acutRelRb(rb);
    rb = nullptr;
    return 0;
}
```

--- 

## 第 9 页 | 使用RAII的做法 - 定制deleter的unique_ptr

```cpp
int main()
{
    // 定制deleter
    auto deleter = [](resbuf* p) {
        if (p) acutRelRb(p);
    };

    // 不管如何离开，resbuf都会被释放
    std::unique_ptr<resbuf, decltype(deleter)> rb(
        acutBuildList(
            RTSHORT, 1,
            RTSTR, _T("Hello"),
            RTNONE
        ),
        deleter
    );

    try {
        if (/*提前退出条件*/) {
            return 0;
        }
    }
    catch (...) {
        // 处理异常
    }

    // 正常退出路径
    return 0;
}
```

--- 

## 使用RAII的做法 - 定制RAII类

定制一个RAII类，封装资源管理，独占所有权方式。可以移动不可拷贝。

```cpp
class RbHolder {
public:
    explicit RbHolder(resbuf* p) : rb(p) {}
    ~RbHolder() {
        if (rb) acutRelRb(rb);
    }

    resbuf* get() const { return rb; }

    RbHolder(const RbHolder&) = delete;
    RbHolder& operator=(RbHolder&&) = delete;

    RbHolder(RbHolder&& other) noexcept : rb(other.rb) {
        other.rb = nullptr;
    }

    RbHolder& operator=(RbHolder&& other) noexcept {
        if (this != &other) {
            rb = other.rb;
            other.rb = nullptr;
        }
        return *this;
    }

private:
    resbuf* rb;
};

int main()
{
    // 不管如何离开，resbuf都会被释放
    RbHolder rb(acutBuildList(
        RTSHORT, 1,
        RTSTR, _T("Hello"),
        RTNONE
    ));

    try {
        if (/*提前退出条件*/) {
            return 0;
        }
    }
    catch (...) {
        // 处理异常
    }

    // 正常退出路径
    return 0;

}

```

---

## 第 8 页｜RAII 价值总结

- ✅ 确定性的资源释放  
- ✅ 天然的异常安全  
- ✅ 代码可组合、可维护  
- ✅ 与现代 C++（移动语义、智能指针）天然契合  

**核心结论：**  
> 在使用 RAII 的前提下，
> 资源安全不再依赖人的小心翼翼，
> 而是交由编译器来保障。

---

## 总结

RAII 不只是技巧，  
而是 C++ 实现资源（内存、文件句柄、连接等）不泄露的核心保障。

下一篇：模板元编程与constexpr——在编译期打造速度

