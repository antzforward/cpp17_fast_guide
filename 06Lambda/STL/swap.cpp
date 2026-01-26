#include <algorithm>
#include <iostream>
/*-T 必须符合可复制构造 (CopyConstructible) 与可复制赋值 (CopyAssignable) (C++11 前)可移动构造 (MoveConstructible) 与可移动赋值 (MoveAssignable) (C++11 起)的要求
-T2 必须满足可交换 (Swappable) 。*/ 
namespace Ns
{
    class A
    {
        int id {};
 
        friend void swap(A& lhs, A& rhs)
        {
            std::cout << "swap(" << lhs << ", " << rhs << ")\n";
            std::swap(lhs.id, rhs.id);
        }
 
        friend std::ostream& operator<< (std::ostream& os, A const& a)
        {
            return os << "A::id=" << a.id;
        }
 
    public:
        A(int i) : id{i} {}
        A(A const&) = delete;
        A& operator = (A const&) = delete;
    };
}
 
int main()
{
    int a = 5, b = 3;
    std::cout << a << ' ' << b << '\n';
    std::swap(a, b);
    std::cout << a << ' ' << b << '\n';
 
    Ns::A p{6}, q{9};
    std::cout << p << ' ' << q << '\n';
//  std::swap(p, q);  // 错误，不满足类型要求
    swap(p, q);       // OK：实参依赖查找找到了合适的友元 swap
    std::cout << p << ' ' << q << '\n';
}