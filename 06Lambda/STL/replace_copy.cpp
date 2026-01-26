#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
 
int main()
{
    std::vector<int> v{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    std::copy(v.begin(),v.end(), std::ostream_iterator<int>(std::cout, " ") );
    std::cout<<'\n';
    
    //感觉是先replace if 再copy的写法。
    std::replace_copy_if(v.begin(), v.end(),
                         std::ostream_iterator<int>(std::cout, " "),
                         [](int n){ return n < 5; }, 99);
    std::cout << '\n';
}