#include <iostream>
#include <string>

// 定义一个名为Weekday的枚举类
enum class Weekday {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

// 函数用于将Weekday转换成对应的字符串
std::string weekdayToString(Weekday day) {
    switch (day) {
        case Weekday::Monday:    return "Monday";
        case Weekday::Tuesday:   return "Tuesday";
        case Weekday::Wednesday: return "Wednesday";
        case Weekday::Thursday:  return "Thursday";
        case Weekday::Friday:    return "Friday";
        case Weekday::Saturday:  return "Saturday";
        case Weekday::Sunday:    return "Sunday";
        default:                 return "[Unknown]";
    }
}
enum class new_enum : unsigned int {
	value1,
	value2,
	value3 = 100,
	value4 = 100
};

template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>
::value, std::ostream>::type& stream, const T& e)
{
	return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

int main() {
    Weekday today = Weekday::Friday;

    std::cout << "Today is " << weekdayToString(today) << "." << std::endl;

    // 使用static_cast进行显式转换
    int dayIndex = static_cast<int>(today);
    std::cout << "Day index is " << dayIndex << "." << std::endl;
	
	if (new_enum::value3 == new_enum::value4) {
		// 会输出
		std::cout << "new_enum::value3 == new_enum::value4" << std::endl;
	}
	std::cout << new_enum::value3 << std::endl;
    return 0;
}