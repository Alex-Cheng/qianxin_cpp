#include <type_traits>
#include <iostream>
#include <typeinfo>
#include <functional>
#include <string>

template<typename T>
void print_type() {
    std::cout << "   类型: " << typeid(T).name() << "\n";
}

void print_separator() {
    std::cout << "----------------------------------------\n";
}

template<typename T>
class SensorSample {
public:
    using RawType = std::remove_cvref_t<T>;
    
    SensorSample(T value) : data_(value) {}

    SensorSample() = default;
    
    void print() const {
        std::cout << "   存储的数据类型: " << typeid(RawType).name() << "\n";
    }
    
private:
    RawType data_;
};

int main() {
    std::cout << "=== 类型变换：修改类型 ===\n\n";
    
    std::cout << "1. std::remove_const\n";
    std::cout << "   原始类型: const int\n";
    std::cout << "   变换后: ";
    print_type<std::remove_const_t<const int>>();
    print_separator();
    
    std::cout << "2. std::remove_reference\n";
    std::cout << "   原始类型: int&\n";
    std::cout << "   变换后: ";
    print_type<std::remove_reference_t<int&>>();
    std::cout << "   原始类型: int&&\n";
    std::cout << "   变换后: ";
    print_type<std::remove_reference_t<int&&>>();
    print_separator();
    
    std::cout << "3. std::remove_cvref (C++20)\n";
    std::cout << "   原始类型: const int&\n";
    std::cout << "   变换后: ";
    print_type<std::remove_cvref_t<const int&>>();
    std::cout << "   原始类型: volatile double&&\n";
    std::cout << "   变换后: ";
    print_type<std::remove_cvref_t<volatile double&&>>();
    print_separator();
    
    std::cout << "4. std::add_pointer\n";
    std::cout << "   原始类型: int\n";
    std::cout << "   变换后: ";
    print_type<std::add_pointer_t<int>>();
    std::cout << "   原始类型: int&\n";
    std::cout << "   变换后: ";
    print_type<std::add_pointer_t<int&>>();
    print_separator();
    
    std::cout << "5. std::add_lvalue_reference\n";
    std::cout << "   原始类型: int\n";
    std::cout << "   变换后: ";
    print_type<std::add_lvalue_reference_t<int>>();
    print_separator();
    
    std::cout << "6. std::decay\n";
    std::cout << "   原始类型: const int[5]\n";
    std::cout << "   变换后: ";
    print_type<std::decay_t<const int[5]>>();
    std::cout << "   原始类型: int&\n";
    std::cout << "   变换后: ";
    print_type<std::decay_t<int&>>();
    print_separator();
    
    std::cout << "7. 实战：统一模板参数类型\n";
    
    // 传入 int 右值，T 推导为 int
    SensorSample s1(42);

    const double d = 3.5;
    // 传入 const double 左值，T 推导为 const double，再经 std::remove_cvref_t 萃取得 double
    SensorSample s2(d);
    
    const double& dref = d;
    // 传入 const double& 左值引用，T 推导为 const double&，再经 std::remove_cvref_t 萃取得 double
    SensorSample s3(dref);

    
    s1.print();
    s2.print();
    s3.print();
    print_separator();
    
    std::cout << "\n类型变换让我们在编译期\"修改\"类型！\n";
    
    return 0;
}
