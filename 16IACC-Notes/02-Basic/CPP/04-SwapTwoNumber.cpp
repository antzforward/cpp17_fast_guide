// cl /EHsc /std:c++20 04-SwapTwoNumber.cpp
#include <format>
#include <iostream>
using namespace  std;
int main(){
    int a,b;
    cin>>a>>b;
    //简单形式，不用实际性的操作。
    cout<<format("{} {}\n",b,a);
    a = a + b; //注意不能越界的。新的a不能超过int的定义域的。
    b = a - b; // to a
    a = a - b; // to b
    cout<<format("{} {}\n",a,b);
    return 0;
}