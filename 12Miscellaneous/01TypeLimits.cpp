#include <iostream>
#include <limits>

int main() {
    std::cout << "The maximum value of int: " << std::numeric_limits<int>::max() << std::endl;
    std::cout << "The minimum value of int: " << std::numeric_limits<int>::min() << std::endl;
    std::cout << "The maximum value of unsigned int: " << std::numeric_limits<unsigned int>::max() << std::endl;

    // 对于byte，通常可以用 char 类型来表示
    std::cout << "The number of bits in a byte: " << std::numeric_limits<unsigned char>::digits << std::endl;
    std::cout << "The maximum value of an unsigned byte (unsigned char): " << static_cast<int>(std::numeric_limits<unsigned char>::max()) << std::endl;
    std::cout << "The maximum value of a signed byte (signed char): " << static_cast<int>(std::numeric_limits<signed char>::max()) << std::endl;
    std::cout << "The minimum value of a signed byte (signed char): " << static_cast<int>(std::numeric_limits<signed char>::min()) << std::endl;
	
	//针对新加的long long int
	using int_64t=long long int;
	using uint_64t = unsigned long long int;
	std::cout << "The maximum value of int_64t: " << std::numeric_limits<int_64t>::max() << std::endl;
    std::cout << "The minimum value of int_64t: " << std::numeric_limits<int_64t>::min() << std::endl;
    std::cout << "The number of bits in a int_64t: " << std::numeric_limits<int_64t>::digits << std::endl;
    
    std::cout << "The maximum value of uint_64t: " << std::numeric_limits<uint_64t>::max() << std::endl;
    std::cout << "The minimum value of uint_64t: " << std::numeric_limits<uint_64t>::min() << std::endl;
    std::cout << "The number of bits in a uint_64t: " << std::numeric_limits<uint_64t>::digits << std::endl;
    
    return 0;
}