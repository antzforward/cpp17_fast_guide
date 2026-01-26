#include <iostream>

void foo(char * );
void foo( int );

int main(){
	if( NULL == (void*)0) std::cout<<"NULL == 0"<<std::endl;
	else std::cout<<"NULL != 0"<<std::endl;
	
	foo(0);
	foo(NULL);
	foo(nullptr);
	return 0;
}

void foo(char* ch){
	std::cout<<"call foo(char* ch)"<<std::endl;
}

void foo(int i){
	std::cout<<"call foo(int i)"<<std::endl;
}