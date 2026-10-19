// cl /EHsc /std:c++23preview /permissive- 02-Branch.cpp
#include <algorithm>
#include <format>
#include <array>
#include <string>
#include <iostream>
#include <linq.hpp>
#include <array>
using namespace std;

int main(int argc, char* argv[]) {
    int problem = 0;
    int branch = 0;
    if (argc >= 3) {
        problem = std::stoi(argv[1]);
        branch = std::stoi(argv[2]);
    }
    if( problem == 0 || branch == 0){
        cout<<R"(
        问题列表
            1、水仙花数
            2、韩信点兵
            3、子序列的和
            4、分数化小数
            5、排列
        不同写法：
            1、暴力解法
            2、STL的API使用
            3、算法最优的写法
    )";
        return 0;
    }
    switch(problem){
        case 1:{
            constexpr int cube[10] = {0, 1, 8, 27, 64, 125, 216, 343, 512, 729};
            //水仙花数:100~999,若三位数ABC=A^3+B^3+C^3,称之为水仙花数
            if(branch == 1 ){
                for(int i=100;i<=999;i++){
                    int a=i/100;
                    int b =(i / 10) % 10;
                    int c = i % 10;
                    if(cube[a] + cube[b] + cube[c] == i ){
                        cout<<format("{}\n",i);
                    }
                }
            }else if (branch == 2) {
                namespace vs = linq::views;
                auto is_narcissistic = [&cube](int i) {
                    int a = i / 100, b = (i / 10) % 10, c = i % 10;
                    return cube[a] + cube[b] + cube[c] == i;
                };

                // 管道式：生成 100..999 → 过滤 → 输出
                vs::iota(100, 1000)
                    | vs::filter(is_narcissistic)
                    | linq::foreach([](int i) { cout << format("{}\n", i); });
            }else if (branch == 3) {
                
                for (int a = 1; a <= 9; a++)
                    for (int b = 0; b <= 9; b++)
                        for (int c = 0; c <= 9; c++) {
                            int i = a * 100 + b * 10 + c;
                            if (cube[a] + cube[b] + cube[c] == i)   // 查表，零乘法
                                cout << format("{}\n", i);
                        }
            }

            break;
        };
        case 2:{
            int a,b,c;
            if (argc >= 6) {
                a = std::stoi(argv[3]);
                b = std::stoi(argv[4]);
                c = std::stoi(argv[5]);
            }else {
                cout<<"参数过少 需要输入 a，b，c"<<endl;
                return -1;
            }
            //方法1 就是 在范围是[10,100)
            // 找到第一个 满足 a = n mod 3,b=n mode 5, c = n mode 7
            if(branch == 1 ){
                int i = 10;
                for(; i<100;i++){
                    if((a == i%3) && (b == i%5) && (c == i%7) ){
                        cout<<format("{}\n",i);
                        return 0;
                    }
                }
                cout<<"No answer";
                if(i>=100) return -1;
            }
            // 写for循环的linq的写法了。 算是方法2
            else if(branch == 2 ){
                namespace vs = linq::views;
                auto matches = [a,b,c](int i){
                    return (a == i%3) && (b == i%5) && (c == i%7);
                };
                // 与方法1相同：只看 [10, 100)。命中第一个就停，区间里没有则返回 -1。
                bool found = false;
                vs::iota(10, 100)
                    | vs::filter(matches)
                    | vs::take(1)
                    | linq::foreach([&found](int i) {
                        cout << format("{}\n", i);
                        found = true;
                    });
                if( !found ) cout<<"No answer";
                return found ? 0 : -1;
                
            }
            // 方法3 
            //韩信点兵，其实是个数论的问题 对应一个数n，它 有a = n mod 3,b=n mode 5, c = n mode 7
            //由于 3,5,7 互质，所以有暴力解法就是
            // 假设n=a，然后 n=n+3(i>=1)，直到达到 这个n mode 5 = b
            // 然后就是 n=n+15i（i>=1） 直到达到 n mode 7 = c 当然有这个n的范围是[10,100)
            // 这就是个。
            else if(branch == 3 ){
                int n = a;
                for(int i=1; n<100;i++){
                    if( n%5 == b )break;
                    n += 3;
                }
                for(int i=1; n< 100;i++){
                    if( n%7 == c )break;
                    n += 15;
                }
                if(n>=100){ 
                    cout<<"No answer";
                    return -1;
                }else{
                    cout<<format("{}\n",n);
                    return 0;
                }
                
            }
            // 方法4 先求出最小公倍数 105，用中国剩余定理来做，有公式 n=(a*70+21*b+15*c) %105
            // a是对3的余数，则它的系数是 105/3 b是5的余数，系数是 105/5，c是7的余数，它的实数是105/7
            else if(branch == 4){
                int n = (70 * a + 21 * b + 15 * c) % 105;
                if( n>= 10 && n<100 ){
                    cout<<format("{}\n",n);
                    return 0;
                }else{
                    cout<<"No answer";
                    return -1;
                }
                
            }
        }  
        case 3:{
            //输入两个正整数n<m<10^6,输出1/n^2+1/(n+1)^2+...+1/m^2. 保留5位小数
            //重点是中间值$n^2,数值是10^12,而2^32=4294967296,10位十进制，保存不够，这里就不能用int32
            //同样对应的分数也不能用float，用double吧
            int n=0,m=0;
            if(argc >= 5){
                n = std::stoi(argv[3]);
                m = std::stoi(argv[4]);
            }
            if( n <= 0 || m <= 0 || n>=m || m>1'000'000) //不符合条件 直接返回，这里不是核心
            {
                cout<<format("input:{},{},{},{}\n",problem,branch,n,m);
               return -1;     
            }
                
            if( branch == 1 ){
                //暴力做法 就是每次都计算对应的中间值，然后求分式 相加，正常输出
                auto ret = 0.0;
                auto mid = 0LL;
                auto dmid = 0.0;
                for(int i=n;i<=m;i++){
                    mid = i*i;
                    dmid = 1.0/mid;
                    ret +=dmid;
                }
                cout<<format("{:.5f}\n",ret);
                return 0;
            }
            else if(branch == 2){
                // range-v3：iota 右端不含，所以写到 m + 1，和上面的 i<=m 对齐。
                // x*x 先转成 long long，避免 m 接近 10^6 时 int 乘法溢出。
                namespace vs = linq::views;
                double ret = vs::iota(n, m + 1)
                    | vs::transform([](int x) {
                        return 1.0 / (static_cast<long long>(x) * x);
                    })
                    | linq::fold(0.0, std::plus<>());
                cout<<format("{:.5f}\n",ret);
                return 0;
            }
            else if(branch == 3 ){
                //最优的写法,嗯 我看了AI的解析，确实是累加变成定积分的模式，然后就是积分形式的减法
                //但是这个定积分是连续的，这个求的是离散的，感觉这种数值算法，都可以单独写一个文章了。
                //幂次乘法也有优化的空间。这里就不做了
                /**
                第 189 行可以做成与区间长度无关的算法。和没有初等闭合式，但可以写成两条无穷尾巴的差，
                尾巴再用欧拉-麦克劳林展开。
                ∑ₖ₌ₙᵐ1/k²=T(n)−T(m+1),    T(x)=∑ₖ₌ₓ^∞1/k²
                x 稍大时，T(x) 只用几项有理式：
                T(x)≈1/x+1/2x²+1/6x³−1/30x⁵+1/42x⁷−1/30x⁹+5/66x¹¹

                系数来自欧拉-麦克劳林：先是 ∫ₓ^∞ x⁻² dx=1/x，再加端点的一半 1/(2x²)，
                后面是伯努利数 B₂=1/6、B₄=−1/30、B₆=1/42 乘上 1/x² 的奇数阶导数。
                我用它和逐项相加对过：n 从 1 到 10⁶ 的几种区间，误差约 10⁻¹⁴，保留 5 位小数一致。
                写的时候按这个分支走：

                • m−n 很短（比如不到 20）时，第 169 行的循环已经够快，展开式省不下时间。
                • n<20 时展开不准。先把 n 到 19 逐项加完，剩下的用 T(20)−T(m+1)。
                • n≥ 20 时直接 T(n)−T(m+1)，一共十几次乘除。
                • 幂次用连乘 1/x, (1/x)², (1/x)³…，比一次算出 x¹¹ 再去除更稳。

                能不能提前停，看的是剩余整段的和。递减正项有上界 T(k)<1/(k−1)。单项 1/k² 已经小于 10⁻⁵ 时，后面那一长串加起来仍可能进到小数点后第 4 位。
                */
                cout<<format("离散算法的基础题，很复杂\n");
                return 0;
            }
            else if(branch == 4){
                //linq 下的最优写法，尽力可为吧。同上。
                return 0;
            }
        } 
        case 4:{
            //输入正整数a,b,c,输出a/b的小数形式，精确到小数后c位，a，b<-10^6,c<=100
            int a,b,c;
            if (argc >= 6) {
                a = std::stoi(argv[3]);
                b = std::stoi(argv[4]);
                c = std::stoi(argv[5]);
            }else {
                cout<<"参数过少 需要输入 a，b，c"<<endl;
                return -1;
            }
            if( branch == 1 ){
                //思路就是，第一次除法得到商，比如 a/b=n...d,然后执行c次，每次都是 d=d*10，a=d
                //如此循环即可。
                //FIX BUG，四舍五入可能出现累进一位，比如0.999995 累进之后就到整数位置了。
                // 看来最后还是要保存到vector里面的吧
                int n = a/b;
                cout<<format("{}.",n);
                int odd = a - n*b;
                for(int i=0;i<c;i++){
                    a = odd * 10 ;
                    n =  a/b;
                    odd = a - n*b;
                    if( i== c -1 ){
                        //最后一位要四舍五入了
                        if(odd >= int(b*0.5))
                            n++;
                        cout<<format("{}",n);
                    }else
                        cout<<format("{}",n);
                }
                
                return 0;
            }else if(branch == 2){
                //linq还是有的，就是要保留一个固定array才行吧。
                namespace vs = linq::views;
                int quot = a/b;
                int rem = a%b;
                struct Step{int rem; string digits;};
                Step acc = vs::iota(0,c)
                |linq::fold(Step{rem,{}},[b](Step s, int){
                    int cur = s.rem * 10 ;
                    s.digits.push_back(char('0'+cur/b));
                    s.rem = cur % b;
                    return s;
                });
                //四舍五入留在管道外，用来做展示效果的内容。所以前面多一位的。
                //返回的acc保留了最后一位的剩余项，考虑能否上位。要考虑连续几个9，就可能直接进位到整数的形式。
                //【我的暴力计算过程这点没考虑对。】
                if (acc.rem * 2 >= b) {
                    int i = (int)acc.digits.size() - 1;
                    for (; i >= 0 && acc.digits[i] == '9'; --i)
                        acc.digits[i] = '0';
                    if (i >= 0)
                        acc.digits[i]++;
                    else
                        quot++;
                }
                cout << format("{}.{}\n", quot, acc.digits);
                return 0;
            }else if( branch == 3){
                //考虑到范围太大了，尤其是c，100个十进制的数字，直接转换做除法也不行的
                //好像找不到好方法来。
            }
        }
        case 5:{
            //1,2,3..,9 组成3位数 abc,def 和ghi，每个数字恰好使用一次
            //要求abc:def:ghi=1:2:3,按照“abc def ghi”的格式输出所有解，每行一个解
            if(branch == 1)
            {
                //这里要有set这个结构就很简单了，九个变量，九个循环套在一起
                //然后构成的数字看是否满足要求,一共就9^9次
                //abc
                for(int a=1;a<=9;a++)
                for(int b=1;b<=9;b++)
                for(int c=1;c<=9;c++)
                //def
                for(int d=1;d<=9;d++)
                for(int e=1;e<=9;e++)
                for(int f=1;f<=9;f++)
                //ghi
                for(int g=1;g<=9;g++)
                for(int h=1;h<=9;h++)
                for(int i=1;i<=9;i++){
                    if(a==b||a==c||a==d||a==e||a==f||a==g||a==h||a==i||
                             b==c||b==d||b==e||b==f||b==g||b==h||b==i||
                                   c==d||c==e||c==f||c==g||c==h||c==i||
                                         d==e||d==f||d==g||d==h||d==i||
                                               e==f||e==g||e==h||e==i||
                                                     f==g||f==h||f==i||
                                                           g==h||g==i||
                                                                 h==i
                    )
                        continue;
                    int num1=a*100+b*10+c;
                    int num2 = d*100 + e*10 + f;
                    int num3 = g*100 + h*10 + i;
                    if( (num1*2 == num2) &&( num1 * 3 == num3 ))
                        cout<<format("{} {} {}\n",num1,num2,num3);
                }
                return 0;

            }
            else if( branch == 2 ){
                //用linq的形式写
                namespace vs = linq::views;
                auto solutions =
                vs::iota(100,334)
                | vs::transform([](int abc){
                        return array<int,3>{abc, abc*2, abc*3};
                    })
                |vs::filter([](const auto& numbers) {
                    auto digits =
                    std::to_string(numbers[0]) +
                    std::to_string(numbers[1]) +
                    std::to_string(numbers[2]);

                    // 排序后恰好等于123456789，
                    // 就说明没有0，且每个数字恰好出现一次。
                    std::sort(digits.begin(), digits.end());
                    return digits == "123456789";
                });
                for (auto [abc, def, ghi] : solutions) {
                    cout<<format("{} {} {}\n",abc,def,ghi);
                }
            }else if(branch == 3){
                // abc:def:ghi = 1:2:3，后两个数由 abc 决定，只枚举 abc。
                // 三者都是三位数：100 ≤ abc ≤ 333。
                // 1..9 的数字和是 45 ≡ 0 (mod 9)，
                // abc+2abc+3abc = 6abc ≡ 0 (mod 9)，所以 abc 必须是 3 的倍数。
                // 100 以上第一个 3 的倍数是 102，步长 3 扫到 333。出现 0 或重复数字就丢掉。
                for (int abc = 102; abc <= 333; abc += 3) {
                    int def = abc * 2;
                    int ghi = abc * 3;
                    int mask = 0;//9bit表示9个不同的数字。
                    bool ok = true;
                    int nums[3] = {abc, def, ghi};
                    for (int v : nums) {
                        for (int k = 0; k < 3; ++k) {
                            int d = v % 10;
                            if (d == 0 || (mask & (1 << d))) {
                                ok = false;
                                break;
                            }
                            mask |= 1 << d;
                            v /= 10;
                        }
                        if (!ok) break;
                    }
                    if (ok)
                        cout << format("{} {} {}\n", abc, def, ghi);
                }
                return 0;
            }
        }
        default:break;
    }
    return 0;
}