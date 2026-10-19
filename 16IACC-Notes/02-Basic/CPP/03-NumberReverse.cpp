// cl /EHsc /std:c++20 03-NumberReverse.cpp
#include <iostream>
#include <format>
using namespace std;
int main(){
    int n;
    cin>>n;
    int o=0;
    while(n){
        auto i = n%10;
        o = o * 10 + i;
        n /= 10;
    }
    cout<<format("{}\n",o);
    return 0;
}