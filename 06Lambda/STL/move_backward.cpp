#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>
 
using container = std::vector<std::string>;//使用alias的方式很方便。

/*移动来自范围 [first, last) 的元素到终于 d_last 的另一范围。以逆序移动元素（首先复制末元素），但保持其相对顺序。
若 d_last 在 [first, last) 内则行为未定义。该情况下必须用 std::move 代替 std::move_backward。*/ 
void print(std::string_view comment, const container& src, const container& dst = {})
{
    auto prn = [](std::string_view name, const container& cont)
    {
        std::cout << name;
        for (const auto &s : cont)
            std::cout << (s.empty() ? "∙" : s.data()) << ' ';
        std::cout << '\n';
    };
    std::cout << comment << '\n';
    prn("src: ", src);
    if (dst.empty())
        return;
    prn("dst: ", dst);
}

//移动重叠的范围时，std::move 适合向左移动（目标范围的起始在源范围外）时，而 std::move_backward 适合向右移动（目标范围的结尾在源范围外）时。 
int main()
{
    container src {"foo", "bar", "baz"};
    container dst {"qux", "quux", "quuz", "corge"};
    print("无重叠的情况；move_backward 之前：", src, dst);
    std::move_backward(src.begin(), src.end(), dst.end());//第三个参数是目标的结尾。实际上都是替为，等同于replace了吧。
    print("之后：", src, dst);
 
    src = {"snap", "crackle", "pop", "lock", "drop"};
    print("有重叠的情况；move_backward 之前：", src);
    std::move_backward(src.begin(), std::next(src.begin(), 3), src.end());
    print("之后：", src);
}
//跟所有带backward一样，这个是防止内存段覆盖的。都是指向相同的内存区域。如果s，d 完全不在一起，就无所谓了。