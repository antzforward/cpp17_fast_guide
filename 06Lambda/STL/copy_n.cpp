#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

//1) 复制始于 first 的范围中恰好 count 个值到始于 result 的范围。正式而言，对于每个整数 0 ≤ i < count，实施 *(result + i) = *(first + i)。范围的重叠在形式上是允许的，但是会导致结果的顺序不可预测。 
int main()
{
    std::string in {"1234567890"};
    std::string out;
 
    std::copy_n(in.begin(), 4, std::back_inserter(out));
    std::cout << out << '\n';
 
    std::vector<int> v_in(128);
    std::iota(v_in.begin(), v_in.end(), 1);//从1...128
    std::cout << "to_vector 包含：";
    std::copy(v_in.begin(), v_in.end(),
              std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
       
    std::vector<int> v_out(v_in.size());
 
    std::copy_n(v_in.cbegin(), 100, v_out.begin());
    std::cout << std::accumulate(v_out.begin(), v_out.end(), 0) << '\n';
}