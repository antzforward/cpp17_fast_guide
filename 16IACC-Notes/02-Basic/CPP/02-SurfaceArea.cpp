// cl /EHsc /std:c++20 02-SurfaceArea.cpp
#include <cmath>
#include <format>
#include <iostream>
using namespace std;
int main(){
    const double pi = acos(-1.0);
    double  r, h;
    if(!(cin>>r>>h))
        return 1;
    double s1 = pi*r*r;
    double s2 = 2*pi*r*h;
    auto s = s1+s2;
    cout<<format("Area = {:.3f}\n",s);
    return 0;
}