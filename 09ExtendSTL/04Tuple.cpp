#include <tuple>
#include <iostream>
#include <string>
#include <variant>
#include <stdexcept>

auto get_student(int id)
{
	// 返回类型被推断为 std::tuple<double, char, std::string>
	if (id == 0)
		return std::make_tuple(3.8, 'A', "张三三少爷的剑");
	if (id == 1)
		return std::make_tuple(2.9, 'C', "李四");
	if (id == 2)
		return std::make_tuple(1.7, 'D', "王五");
	return std::make_tuple(0.0, 'D', "null");
}
// 递归打印函数
template<typename Tuple, std::size_t... Is>
void printTuple(const Tuple& t, std::index_sequence<Is...>) {
    ((std::cout << (Is == 0 ? "" : ", ") << std::get<Is>(t)), ...);
}

// 用户接口函数
template<typename... Args>
void printTuple(const std::tuple<Args...>& t) {
    printTuple(t, std::make_index_sequence<sizeof...(Args)>());
    std::cout<<std::endl;
}

//尝试动态获得tuple元素的写法，嗯，确实没有提供这个功能
// Function to convert a tuple to a variant
template<typename... Ts>
struct tuple_to_variant;

template<typename T, typename... Ts>
struct tuple_to_variant<std::tuple<T, Ts...>> {
    using type = std::variant<T, Ts...>;
};

// Helper function to get a variant from the tuple at runtime index
template<size_t I = 0, typename... Tp>
typename std::enable_if<I == sizeof...(Tp), void>::type
get_by_index(int, const std::tuple<Tp...>&, typename tuple_to_variant<std::tuple<Tp...>>::type&) {
    throw std::out_of_range("Index out of range");
}

template<size_t I = 0, typename... Tp>
typename std::enable_if<I < sizeof...(Tp), void>::type
get_by_index(int index, const std::tuple<Tp...>& tup, typename tuple_to_variant<std::tuple<Tp...>>::type& var) {
    if (index == I) {
        var = std::get<I>(tup);
    } else {
        get_by_index<I + 1, Tp...>(index, tup, var);
    }
}

template<typename... Tp>
typename tuple_to_variant<std::tuple<Tp...>>::type get_from_tuple(const std::tuple<Tp...>& tup, int index) {
    typename tuple_to_variant<std::tuple<Tp...>>::type var;
    get_by_index(index, tup, var);
    return var;
}

int main()
{
	auto student = get_student(0);
	std::cout << "ID: 0, "
		<< "GPA: " << std::get<0>(student) << ", " //get<n> n必须为编译器常量
		<< "成绩: " << std::get<1>(student) << ", "
		<< "姓名: " << std::get<2>(student) << '\n';
	std::cout<<"student 包含元素数量:\t"<<std::tuple_size<decltype(student)>::value<<std::endl;
	std::cout<<"student 内存占用:\t"<<sizeof student<<std::endl;
	double gpa;
	char grade;
	std::string name;
	// 元组进行拆包
	std::tie(gpa, grade, name) = get_student(1);
	std::cout << "ID: 1, "
		<< "GPA: " << gpa << ", "
		<< "成绩: " << grade << ", "
		<< "姓名: " << name << '\n';
		
	const auto& [x,y,z] = get_student(2);//注意解构带来的move的问题。悬垂引用的问题。
	std::cout << "ID: 3, "
		<< "GPA: " << x << ", "
		<< "成绩: " << y << ", "
		<< "姓名: " << z << '\n';
		
	//改动解构的时候需要知道成员数量，或者全解构的问题。没有糖可以用。
	//const auto& [id, ] = student;或const auto& [id] = student;不行，
	auto myTuple = std::make_tuple("Big Boss",1, 4.5, "Hello","World");
	//使用initializier list的模式也就是{}推导不出auto的。使用make_tuple就好多了
    printTuple(myTuple); // 输出: 1, 4.5, Hello
    
    //getAtIndex 的测试
    std::tuple<int, double, std::string> t = {10, 3.14, "Hello"};
	auto v0 = get_from_tuple(t, 0); // v0 holds int
	auto v1 = get_from_tuple(t, 1); // v1 holds double
	auto v2 = get_from_tuple(t, 2); // v2 holds string

	// Visit and print the value
	std::visit([](const auto& val) { std::cout << val << std::endl; }, v0);
	std::visit([](const auto& val) { std::cout << val << std::endl; }, v1);
	std::visit([](const auto& val) { std::cout << val << std::endl; }, v2);

	try {
	  auto v3 = get_from_tuple(t, 3); // This will throw an exception
	} catch(const std::exception& e) {
	  std::cerr << "Error: " << e.what() << std::endl;
	}
	
	std::tuple<std::string,int,double> t2 = {"Big Boss",1, 4.5};
	//测试以下mytuple的情况。
	for(int i=0; i<std::tuple_size<decltype(t2)>::value;i++){
		std::cout<<"mytuple 按照顺序访问： "<<i<<" : ";
		auto value = get_from_tuple(t2, i);//这个实现有问题，如果类型重复的话，就报错了。
		std::visit([](const auto& val) { std::cout << val << std::endl; },value) ;
	}
}
