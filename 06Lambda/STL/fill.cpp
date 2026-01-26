#include <algorithm>
#include <iostream>
#include <vector>
#include <iomanip>
 
int main()
{
    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
 
    std::fill(v.begin(), v.end(), -1);
	std::copy(v.begin(), v.end(),std::ostream_iterator<int>(std::cout, " "));
    std::cout<<'\n';
    
    std::fill_n(v.begin(), 5, 1);
    std::for_each(v.begin(), v.end(), [](const int& n) {
        std::cout <<std::setw(2)<<n << " ";
    });
    std::cout << "\n";
    
    
    std::vector<int> a(10,-1);
    std::copy(a.begin(), a.end(),std::ostream_iterator<int>(std::cout, " "));
    std::cout<<'\n';
}