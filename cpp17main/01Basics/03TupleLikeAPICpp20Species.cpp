#include <iostream>
#include <string>
#include <utility>
#include <type_traits>

class Customer2 {
private:
    std::string first;
    std::string last;
    long val;
public:
    // 构造函数修正
    Customer2(std::string f, std::string l, long v)
        : first{std::move(f)}, last{std::move(l)}, val{v}  // 修正: last{std::move(l)}
    {
    }
    
    // 使用 C++20 的 operator<=> 进行结构化绑定支持
    template<std::size_t I>
    decltype(auto) get() & {
        if constexpr (I == 0) return (first);
        else if constexpr (I == 1) return (last);
        else if constexpr (I == 2) return (val);
    }
    
    template<std::size_t I>
    decltype(auto) get() const& {
        if constexpr (I == 0) return (first);
        else if constexpr (I == 1) return (last);
        else if constexpr (I == 2) return (val);
    }
    
    template<std::size_t I>
    decltype(auto) get() && {
        if constexpr (I == 0) return std::move(first);
        else if constexpr (I == 1) return std::move(last);
        else if constexpr (I == 2) return val;
    }
};

// 这些特化必须在 std 命名空间中，且不能在类内部
namespace std {
    template<> struct tuple_size<Customer2> 
        : integral_constant<size_t, 3> {};
    
    template<size_t I> struct tuple_element<I, Customer2>;
    
    template<> struct tuple_element<0, Customer2> 
        { using type = std::string; };
    
    template<> struct tuple_element<1, Customer2> 
        { using type = std::string; };
    
    template<> struct tuple_element<2, Customer2> 
        { using type = long; };
}

// 非成员 get 函数
template<std::size_t I>
decltype(auto) get(Customer2& c) {
    return c.template get<I>();
}

template<std::size_t I>
decltype(auto) get(const Customer2& c) {
    return c.template get<I>();
}

template<std::size_t I>
decltype(auto) get(Customer2&& c) {
    return std::move(c).template get<I>();
}

int main() {
    // 测试常量引用
    const Customer2 constCustomer{"John", "Doe", 100};
    
    // 结构化绑定
    auto [first1, last1, val1] = constCustomer;
    std::cout << "Const binding: " << first1 << " " << last1 << " " << val1 << "\n";
    
    // 通过 get<> 访问
    std::cout << "Get<0>: " << get<0>(constCustomer) << "\n";
    std::cout << "Get<1>: " << get<1>(constCustomer) << "\n";
    std::cout << "Get<2>: " << get<2>(constCustomer) << "\n";
    
    // 测试非常量引用
    Customer2 customer{"Jane", "Smith", 200};
    
    auto [first2, last2, val2] = customer;
    std::cout << "\nNon-const binding: " << first2 << " " << last2 << " " << val2 << "\n";
    
    // 修改值
    get<0>(customer) = "Janet";
    get<2>(customer) = 300;
    
    std::cout << "After modification:\n";
    std::cout << "First: " << get<0>(customer) << "\n";
    std::cout << "Value: " << get<2>(customer) << "\n";
    
    // 测试右值引用
    auto [first3, last3, val3] = Customer2{"Temp", "User", 400};
    std::cout << "\nRvalue binding: " << first3 << " " << last3 << " " << val3 << "\n";
    
    return 0;
}
//cl /EHsc /std:c++20 03TupleLikeAPICpp20Species.cpp /link psapi.lib