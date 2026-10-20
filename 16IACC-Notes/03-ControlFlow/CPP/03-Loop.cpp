// cl /EHsc /std:c++23preview /permissive- 03-Loop.cpp 
#include <algorithm>
#include <format>
#include <array> 
#include <string>
#include <linq.hpp>
#include <cmath>
#include <numbers>
#include <iostream>
using namespace std;
int main(int argc, char* argv[]) {
    int problem = 0;
    int branch = 0;
    if (argc >= 3) {
        problem = std::stoi(argv[1]);
        branch = std::stoi(argv[2]);
    }
    if( problem == 0 || branch == 0){
        cout<<format(R"(
        问题列表
            1、aabb的完全平方数
            2、求pi/4的近似值
            3、求 e的近似值
            4、阶乘之和
            5、欧几里得除法的形式
        不同写法：
            1、暴力解法
            2、STL的API使用
            3、算法最优的写法
        )");
        return 0;
    }
    if( problem == 1 ){
        //1、找到表现形式为aabb的完全平方数
        if(branch == 1 ){
          //暴力法：就是遍历形式      
          for(int a=1;a<=9;a++){
            for(int b=0;b<=9;b++){
                int n = a*1100+b*11;
                int m = std::sqrt(n);
                if( m * m == n ){
                    cout<<format("{},",n);
                }
            }
          }
          cout<<format("\n");
          return 0;
        }else if( branch == 2 ){
            //相互正交的就用笛卡尔正交的形式，比如上面的a，b实际上是无关的
            namespace vs = linq::views;
            vs::cartesian_product(vs::iota(1,10),
            vs::iota(0,10))
            | vs::transform([](auto ab){
                auto [a,b] = ab;
                return a*1100+b*11;
            })
            |vs::filter([](int n){
                int m = (int)std::sqrt(n);
                return m*m == n;
            })
            |linq::foreach([](int n){
                cout<<format("{},",n);
            });
            cout<<format("\n");
            
        }else if(branch > 2 ){
            //真正的优化形式 考虑最快
            //aabb=1100a+11b=11(100a+b)=11(99a+(a+b))
            //至少要 a+b mod 11 = 0,由 a \in[1,9] b\in[ 0,9]
            // 所以只有一种情况 a+b=11 代入上面可以知道
            // 11(99a+(a+b)) = 11*11*（9a+1）可以知道（9a+1）是整数平方
            // a \in[1,9]，则（9a+1）\in {10,19,28,37,46,55,64,73,82}
            // 可以知道a=7，从而b=4，直接输出 7744 就好了 且只有一个解
            cout<<format("{}\n",7744);
        }
    } else if(problem == 2){
        // 求\dfrac{\pi}4的近似值
        double epslion = 1e-6;
        if(branch == 1){
            //书上的例子吧，用的是莱布尼茨级数（最简单，收敛极慢）
            double  s=0;
            int     i=0;
            do{
                double term = 1.0/(2.0*i + 1.0);
                s += i%2?-term:term;
                i++;
                if( term < epslion )break;
            }while(1);
            cout<<format("{:.15f}\n",s );

        }else if( branch == 2){
            // 还是指向linq写法吧
            //不想先求终点，就用没有右端的 iota(0)，它会一直数下去，
            // 再用 take_while 代替 break。take_while 在条件失败的那一项之前停下，
            // 而你的循环是先加再判断，所以小于 epslion 的那一项还要留下。
            // 这个头文件 linq.hpp 里没有，要单独加上 <range/v3/view/take_while.hpp>。
            namespace vs = linq::views;
            bool passed = false;
            double s = vs::iota(0)
            |vs::transform([](int k){
                double term = 1.0/(2.0*k + 1.0);
                return k%2 ?-term:term;
            })
            |vs::take_while([&](double term){
                if(passed) return false;
                if(std::abs(term)<epslion) passed = true;
                return true;
            })
            |linq::fold(0.0,std::plus<>());
            cout<<format("{:.15f}\n",s );

        }//注意下面的代码并不是优化的，只是为了展示算法部分
        else if( branch == 3){
            //梅钦类公式，用arctan 组合的形式，但是arctan 也要算啊。
            //梅钦公式（arctan用泰勒级数)
            auto arctan_taylor=[](double x, int N){
                namespace vs = linq::views;
                //arctan(x)泰勒展开 |x| ≤1
                return vs::iota(0,N)
                |vs::transform([x](int k){
                    double term = std::pow(x,2*k+1)/(2*k+1.0);
                    return k%2?-term:term;
                })
                |linq::fold(0.0,std::plus<>());
            };
            const int N=20;
            double s = 4*arctan_taylor(1.0/5,N)- arctan_taylor(1.0/239,N);
            cout<<format("{:.15f}\n",s );
        }else if(branch == 4){
            //数值积分的形式，可以用梯形法、辛普森积分，里面提到数值离散近似而已。
            auto simpson_pi4 = [](int n){
                if( n%2 !=  0)
                    n+=1;
                auto a = 0.0;
                auto b = 1.0;
                auto h = (b-a)/n;
                auto s = 1.0/(1+a*a)+1.0/(1+b*b);
                for(int k=1;k<n;k++){
                    auto x = a + k*h;
                    auto fx = 1.0/(1+x*x);
                    if( k% 2 == 1){
                        s += 4*fx;
                    }else{
                        s += 2*fx;
                    }
                }
                return h*s/3.0;
            };
            double s = simpson_pi4(200);
            cout<<format("{:.15f}\n",s );
        }else if( branch == 5 ){
            // 与第 130 行同一条辛普森公式。k = 0..n，权重是 1,4,2,4,...,2,4,1。
            auto simpson_pi4 = [](int n) {
                if (n % 2 != 0)
                    n += 1;
                auto a = 0.0;
                auto b = 1.0;
                auto h = (b - a) / n;
                namespace vs = linq::views;
                auto s = vs::iota(0, n + 1)
                    | vs::transform([=](int k) {
                        auto x = a + k * h;
                        auto fx = 1.0 / (1.0 + x * x);
                        auto w = (k == 0 || k == n) ? 1.0 : (k % 2 ? 4.0 : 2.0);
                        return w * fx;
                    })
                    | linq::fold(0.0, std::plus<>());
                return h * s / 3.0;
            };
            double s = simpson_pi4(200);
            cout << format("{:.15f}\n", s);
        }else if( branch == 5 ){
            // 不考虑那么多精度，直接反函数求解 \dfrac{\arccos(-1)}{4} 的形式
            cout<<format("{:.15f}\n",std::acos(-1.0)*0.25);
        }else{
            //默认值保留了，直接读出来看看
            cout<<format("{:.15f}\n",std::numbers::pi*0.25);
        }
    }else if(problem == 3){
        if(branch == 1){
           //直接从numbers的常数库里面查看一下
           cout<<format("{:.15f}\n",std::numbers::e);     
        }else if( branch == 2){
            //考虑泰勒级数，就是麻烦一点，要知道级数表达式形式
            //e^x= \sum_0^{\infty}\dfrac{x^n}{n!}令x=1 得到 e=\sum_0^{\infty}\dfrac1{n!}
            auto e_taylor=[](int N){
                auto e = 0.0;
                auto factor = 1.0;
                for(int i=0;i<N;i++){
                    if( i > 0 ) factor *= i;
                    e += 1.0/factor;
                }
                return e;
            };
            cout<<format("{:.15f}\n",e_taylor(20));        
        }else if( branch == 3){
            //注意：误差非常的大，哪怕是100'000也误差很大
            //用极限收敛的方式，这个比较慢，用来展示
            // e =\lim_{n\to\infty}(1+\dfrac1n)^n
            auto e_limit=[](int N){
                return pow(1.0+1.0/N,N);
            };
            cout<<format("{:.15f}\n",e_limit(100000));   
        }else if( branch == 4){
            //辛普森积分法，确实辛普森法要好好学
            // e=\int_0^1 e^x dx + 1
            auto simpson =[](std::function<double(double)>f,double a, double b, int N){
                auto h = (b-a)/N;
                auto s = f(a)+f(b);
                for(int i=1;i<N;i++){
                    auto x = a+i*h;
                    if( i%2 == 1){
                        s += 4*f(x);
                    }else{
                        s += 2*f(x);
                    }
                }
                return s*h/3.0;
            };
            auto e_simpson=[&](int N){
                auto f = [](double x){
                    return exp(x);
                };
                auto integral = simpson(f,0.0,1.0,N);
                return 1 + integral;
            };
            cout<<format("{:.15f}\n",e_simpson(1000));   
        }else if( branch == 5){
            // 第 186 行的递推：factor 从 1 开始，第 i 项乘上 i，再加 1/factor。
            // exclusive_scan 先给出 0!,1!,2!,...，再取倒数相加。
            auto e_taylor = [](int N) {
                namespace vs = linq::views;
                return vs::iota(1, N + 1)
                    | vs::exclusive_scan(1.0, std::multiplies<>())
                    | vs::transform([](double factor) { return 1.0 / factor; })
                    | linq::fold(0.0, std::plus<>());
            };
            cout << format("{:.15f}\n", e_taylor(20));
        }
        //求 e的近似值，
        // 也有用反函数形式求的？
        // 用展式来求。
    }else if(problem == 4){
        //阶乘之和，要考虑accumulate的值超过范围的问题。
    }else if(problem == 5){
        //欧几里得除法的形式
    }
    
}