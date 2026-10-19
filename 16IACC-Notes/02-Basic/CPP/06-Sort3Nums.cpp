// cl /EHsc /std:c++20 06-Sort3Nums.cpp
#include <algorithm>
#include <format>
#include <array> 
#include <iostream>
using namespace std;
#define BRUNCH 3
//这里这里我想用到就是简洁语句，因为是3个数字 就不用array来sort了。
int main(){
    int a,b,c;
    cin>>a>>b>>c;
    //先写个最常规的，if-else的情况
    #if BRUNCH == 1
    if( a < b){
        if( b<c) cout<<format("{} {} {}",a,b,c);
        else {
            if(a<c) cout<<format("{} {} {}",a,c,b);
            else cout<<format("{} {} {}",c,a,b);
        }
    }else{
        if( a<c) cout<<format("{} {} {}",b,a,c);
        else {
            if(b<c) cout<<format("{} {} {}",b,c,a);
            else cout<<format("{} {} {}",c,b,a);
        }
    }
    #elif BRUNCH == 2
    //这里直接重复计算就好了,<algorithm>
    // 因为用了取余的方式，如果总和超过限制，就出问题了。
    auto s = a+b+c;
    auto top = max(max(a,b),c);
    auto buttom = min(min(a,b),c);
    auto mid = s - top - buttom;
    cout<<format("{} {} {}",buttom,mid,top);

    #elif BRUNCH == 3
    //这里用个array来处理了
    array<int,3> arr= {a,b,c};
    sort(arr.begin(),arr.end());
    cout<<format("{} {} {}\n",arr[0],arr[1],arr[2]);
    //用for in 也可以的
    for_each(arr.begin(), arr.end(), [](const int n) { cout << n << ' '; });
    #endif
    return 0;
}