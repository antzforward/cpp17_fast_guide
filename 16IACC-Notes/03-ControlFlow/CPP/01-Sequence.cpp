// cl /EHsc /std:c++20 01-Sequence.cpp
#include <format>
#include <array> 
#include <iostream>
using namespace std;
//这里从大的结构就是从前到执行，对应的应该是fwd形式
//但是也有从结果推到因的过程，对应的是bwd
//但是bwd 一定要用阈值检查的，所以会写成while do的形式。
// 串行是非常普遍的形式。
// 这里主要展示代码的临时变量定义要接近使用区域的情况吧
// 其他内容也没啥好展示的。
int main(){
    //下面模拟一个左右对称的聊天内容吧
    cout<<"Good moring,everybody"<<endl;
    cout<<"Good moring,sir"<<endl;
    cout<<"Today, we will learn a new lesson:Sequence!"<<endl;
    cout<<"Please open the book,turn to Page 54."<<endl;
    cout<<"..."<<endl;
    cout<<"..."<<endl;
    cout<<"Alarm click...."<<endl;
    cout<<"Everybody,this class is closed,goodby"<<endl;
    return 0;
}