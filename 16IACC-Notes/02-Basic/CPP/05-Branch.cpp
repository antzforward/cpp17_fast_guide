// cl /EHsc /std:c++20 05-Branch.cpp
#include <format>
#include <iostream>
using namespace std;
int main(){
    int n,m;//n animals, m feet
    cin>>n>>m;
    int b=(m-2*n)/2;//rabbit number
    int a = n - b;//chicks number
    // 注意逻辑判断的优先级 如果写成2*a+4*b != m 就错了
    // 先逻辑判断，得到1，然后2*a+1 一定不等 0
    // 还要注意 逻辑运算符的短路运算符的作用。
    if( a<=0 || b <= 0 || (2*a+4*b) != m ) 
    //if( m%2 == 1 || a<0 || b < 0 )
    {
        cout<<"No answer\n";
    }else{
        cout << format("{} {}\n",a, b);
    }
    return 0;
}