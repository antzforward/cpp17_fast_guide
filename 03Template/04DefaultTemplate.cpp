#include <iostream>
using namespace std;
template<typename T=int, typename U=int>//默认指向了int，但是一旦要填，就要填完，不如默认函数参数那样方便。
auto add( T x, U y)->decltype(x+y){
	return x + y;
}

int main(){
	cout<<add( 10, 11)<<endl;
	cout<<add<int,float>( 10, 11.5f)<<endl;
	cout<<add<float,float>( 10.2f, 11.05f)<<endl;
}