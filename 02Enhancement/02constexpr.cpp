//const的，会在constexpr中表现出来
#include <iostream>
#include <random>

constexpr int fibonacci( const int n ) {
	return n==1 || n==2 ?1:fibonacci(n-1) + fibonacci(n-2);
}

int main(){
	int sumIdx = 0,sum=0;
	for(int i=5; i<10;i++)//这是核心，输入参数是常量，从而上面的过程也是
	{
		//std::cout<<"fibonacci("<<i<<"): "<<fibonacci(i)<<std::endl;
		sumIdx += i;
		sum += fibonacci(i);
	}
	
	std::random_device rd; // 用来获得种子值
    std::mt19937 gen(rd()); // Mersenne Twister引擎
    // 定义一个范围为1-6的均匀分布
    std::uniform_int_distribution<> dis(5, 10);

	int sumIdx2 = 0,sum2=0;
    // 生成随机数
    for (int i = 5; i < 10; ++i) {
		auto p = dis(gen);
		sumIdx2 += p;
		sum2 += fibonacci(p);
        //std::cout<<"fibonacci("<<p<<"): "<<fibonacci(p)<<std::endl;
    }
    std::cout<<sumIdx2-sumIdx<<sum2-sum<<std::endl;
    return 0;
}
//编译未优化的asm文件，并不链接的方式 cl /EHsc /FA /c 02constexpr.cpp