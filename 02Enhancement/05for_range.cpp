#include <iostream>
#include <vector>
using namespace std;

int main(){
	int arr[] = {1,2,3,4,5,6};
	cout<<"int arr[] = {1,2,3,4,5,6};: "<<"[";
	for(auto &x : arr){
		cout<<x<<",";
	}
	cout<<"]"<<endl;
	
	vector<int> vec(5,6);
	cout<<"vector<int> vec(5,6);: "<<"[";
	for(auto itr = vec.begin(); itr != vec.end(); ++itr ){
		cout<<*itr<<",";
	}
	cout<<"]"<<endl;
}