/**
 * @defgroup 01Basics 基础语法示例
 * @brief C++17 基础语法相关的示例代码集合
 */

/**
 * @file 01UnpackStruct.cpp
 * @ingroup 01Basics
 * @brief C++17 结构化绑定(Structured Binding) — 结构体解包详解
 * @details 本示例演示 C++17 结构化绑定语法在结构体上的各种用法，
 *          包括值绑定、引用绑定、const 引用绑定、移动语义等。
 */

#include <vector>
#include <string>
#include <iomanip>
#include "../TestMacro.h"

/**
 * @brief 示例结构体，包含 int、string、double 三个成员
 *
 * 用于演示结构化绑定对聚合类型和非聚合类型（有用户构造函数）的支持。
 * 提供了委托构造函数链，允许从不同数量的参数构造。
 */
struct MyStruct{
	int i;          ///< 整型成员
	std::string s;  ///< 字符串成员
	double c;       ///< 浮点数成员

	/**
	 * @brief 三参数构造函数
	 * @param intV    整型值
	 * @param strV    字符串值
	 * @param doubleV 双精度浮点值
	 */
	MyStruct(int intV,std::string strV,double doubleV){
		i=intV;
		s = strV;
		c = doubleV;
	}

	/**
	 * @brief 两参数构造函数，c 默认为 π
	 * @param intV 整型值
	 * @param strV 字符串值
	 * @note 委托构造到三参数版本，默认 c = 3.1415926535
	 */
	MyStruct(int intV,std::string strV):MyStruct(intV,strV,3.1415926535)
	{
	}

	/**
	 * @brief 单参数构造函数，s 默认为 "Empty Boy!"
	 * @param intV 整型值
	 */
	MyStruct(int intV):MyStruct(intV,"Empty Boy!"){
	}

	/** @brief 默认构造函数，所有成员使用默认值 (0, "Empty Boy!", π) */
	MyStruct():MyStruct(0){}
};

/**
 * @brief 另一个用于演示的结构体
 *
 * 包含四个不同类型的成员，展示结构化绑定对多成员结构体的支持。
 */
struct OtherStruct{
	int id;             ///< 标识符
	int value;          ///< 整型值
	double other;       ///< 浮点值
	std::string otherInfo; ///< 描述信息

	/**
	 * @brief 打印结构体的所有成员
	 */
	void PrintOtherStruct(){
		std::cout<<"id："<<id<<"		a："<<value<<"		b："<<std::setprecision(12)<<other<<" hint: "<<otherInfo<<std::endl;
	}
};

/**
 * @brief 返回一个 MyStruct 实例的辅助函数
 * @return MyStruct{42, "Hello World", 0.001}
 */
MyStruct getStruct(){
	return MyStruct{42,"Hello World",0.001};
}

/**
 * @brief 主函数 — 演示 C++17 结构化绑定的多种用法
 *
 * 依次演示以下场景：
 * -# 基本值绑定：将结构体成员解包为独立变量
 * -# 临时对象绑定：直接对临时构造的对象进行解包
 * -# 值绑定的独立性：修改解包变量不影响原结构体
 * -# 引用绑定：使用 auto& 解包，修改会同步到原结构体
 * -# const 引用绑定：只读访问，但可通过原对象间接修改
 * -# 移动语义绑定：使用 auto&& 解包并转移资源
 * -# 值解包与移动：使用 auto 解包配合 std::move 转移成员
 *
 * @return 0 正常退出
 *
 * @note 编译命令: cl /EHsc /std:c++17 01UnpackStruct.cpp /link psapi.lib
 */
int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);

	///CODE:struct MyStruct{
	///CODE:	int i;
	///CODE:	std::string s;
	///CODE:	double c;
	///CODE: }
	///CODE:MyStruct ms;
	///Not:这种写法已经会出现编译错误了，因为语法跟initializer list太一致了，出现二义性。
	///Code:auto [u2, v2] {ms};
	///Note:下面这个形式还是可以的。
	///CODE：auto [u3, v3] (ms);
	///Note：通常解析的语法
	///Code：auto [u,v,w]=ms;

	/**
	 * @name 示例1：基本值绑定
	 * @brief 使用 auto 将结构体成员解包为独立的局部变量
	 * @details 解包后的变量是原成员的副本，修改不会影响原结构体。
	 *          类似 Python 中的元组解包: `u, v, w = struct_values`
	 * @{
	 */
	{
		MyStruct ms;
		auto [u,v,w]=ms;
		std::cout<<"MyStruct.i："<<u<<"		MyStruct.s："<<v<<"		MyStruct.c："<<std::setprecision(12)<<w<<std::endl;
	}
	///Note: python中近似写
	///Code(Python): p=(42,"Bad Boy!")
	///Code(Python): u1,v1=p

	/**
	 * @brief 直接对临时对象进行结构化绑定
	 * @details 使用花括号初始化临时 MyStruct 并立即解包。
	 */
	{
		auto [u,v,w]= MyStruct{42,"Bad Boy!"};
		std::cout<<"MyStruct.i："<<u<<"		MyStruct.s："<<v<<"		MyStruct.c："<<std::setprecision(12)<<w<<std::endl;
	}
	/** @} */

	///NOTE：共用解析临时变量,实际上不行的,不能复用临时变量
	///auto [u1,v1,w1]= (ms);
	///NOTE：修改解析后的内容，看是否影响struct自己
	///auto [u2,v2,w2] = ms2;
	///u2=13;v2="Fixed strz";w2 = 1.23456789;
	///std::cout<<"MyStruct.i："<<ms2.i<<"		MyStruct.s："<<ms2.s<<"		MyStruct.c："<<std::setprecision(12)<<ms2.c<<std::endl;
	///MyStruct.i：42          MyStruct.s：Hello World         MyStruct.c：0.001

	/**
	 * @name 示例2：值绑定的独立性验证
	 * @brief 验证值绑定后修改解包变量不会影响原结构体
	 * @details getStruct() 返回临时对象被 auto 接收为值副本，
	 *          解包后的 u2, v2, w2 是独立副本，修改它们不会改变 ms2 的成员。
	 * @{
	 */
	{
		auto ms2 = getStruct();
		auto [u2,v2,w2] = ms2;
		std::cout<<"MyStruct.i："<<u2<<"		MyStruct.s："<<v2<<"		MyStruct.c："<<std::setprecision(12)<<w2<<std::endl;
		u2=13;
		v2="Fixed strz";
		w2 = 1.23456789;
		std::cout<<"MyStruct.i："<<ms2.i<<"		MyStruct.s："<<ms2.s<<"		MyStruct.c："<<std::setprecision(12)<<ms2.c<<std::endl;
	}
	/** @} */

	///NOTE：用引用分解的方式,如果要是imutable的用const auto&的形式
	///auto& [u3,v3,w3] = ms2;
	///u3=13;v3="Fixed strz";w3 = 1.23456789;
	///std::cout<<"MyStruct.i："<<ms2.i<<"		MyStruct.s："<<ms2.s<<"		MyStruct.c："<<std::setprecision(12)<<ms2.c<<std::endl;
	///MyStruct.i：13          MyStruct.s：Fixed strz          MyStruct.c：1.23456789

	/**
	 * @name 示例3：引用绑定与 const 引用绑定
	 * @brief 使用 auto& 解包后修改变量会同步到原结构体
	 *
	 * - 使用 @c auto& 绑定：解包变量是原成员的引用，修改会反映到原结构体。
	 * - 使用 @c const auto& 绑定：解包变量是只读引用，不能直接赋值，
	 *   但仍然可以通过原结构体修改成员，只读引用会看到最新的值。
	 *
	 * @note C++ 不允许混合模式（部分引用、部分值），如 @c auto [u5, &v5, w5] 编译不通过。
	 * @{
	 */
	{
		auto ms2 = getStruct();

		/** @brief 引用绑定 — 修改解包变量直接影响原结构体 */
		auto& [u3,v3,w3] = ms2;
		std::cout<<"MyStruct.i："<<u3<<"		MyStruct.s："<<v3<<"		MyStruct.c："<<std::setprecision(12)<<w3<<std::endl;
		u3=13;
		v3="Fixed strz";
		w3 = 1.23456789;
		std::cout<<"MyStruct.i："<<ms2.i<<"		MyStruct.s："<<ms2.s<<"		MyStruct.c："<<std::setprecision(12)<<ms2.c<<std::endl;

		/** @brief const 引用绑定 — 不能直接赋值，但可通过原对象间接修改 */
		const auto& [u4,v4,w4] = ms2;
		std::cout<<"MyStruct.i："<<u4<<"		MyStruct.s："<<v4<<"		MyStruct.c："<<std::setprecision(12)<<w4<<std::endl;

		// const auto 变量不能赋值。
		//v4="fix again";
		// 但是指向的结构体还是可以赋值修改的
		ms2.s="fix again";
		std::cout<<"MyStruct.i："<<u4<<"		MyStruct.s："<<v4<<"		MyStruct.c："<<std::setprecision(12)<<w4<<std::endl;
		//MyStruct.i：13          MyStruct.s：fix again           MyStruct.c：1.23456789
	}
	/** @} */

	/// c++不容许下面这种混合模式，部分引用，部分赋值，编译不通过
	//auto [u5,&v5,w5] = ms2;
	//std::cout<<"MyStruct.i："<<u5<<"		MyStruct.s："<<v5<<"		MyStruct.c："<<std::setprecision(12)<<w5<<std::endl;

	/**
	 * @name 示例4：移动语义绑定
	 * @brief 使用 auto&& 配合 std::move 进行右值引用绑定
	 * @details 解包变量持有原结构体成员的右值引用，可以从中移动资源（如 string），
	 *          移动后原结构体对应成员处于有效但未指定的状态。
	 * @{
	 */
	{
		//move语义
		auto ms = MyStruct{41};
		auto&& [u,v,w]=std::move(ms);
		std::cout<<"MyStruct.i："<<u<<"		MyStruct.s："<<v<<"		MyStruct.c："<<std::setprecision(12)<<w<<std::endl;
		//MyStruct.i：41          MyStruct.s：Empty Boy!          MyStruct.c：3.1415926535
		auto s = std::move(v);
		v = "Lara";
		std::cout<<"MyStruct.s："<<ms.s<<"		unpack v："<<v << "		move s:"<<s<<std::endl;
		//MyStruct.s：Lara                unpack v：Lara          move s:Empty Boy!
	}
	/** @} */

	/**
	 * @name 示例5：值解包与移动
	 * @brief 使用 auto（非引用）配合 std::move 将成员直接移动到解包变量
	 * @details 使用 @c auto 而非 @c auto&& 接收时，解包变量是独立的新对象，
	 *          原结构体的 string 成员被移动后变为空字符串。
	 * @{
	 */
	{
		// unpack 接受体不是auto &&,是auto，代表直接完成接受
		auto ms = MyStruct{41};
		auto [u,v,w]=std::move(ms);
		std::cout<<"MyStruct.i："<<ms.i<<"		MyStruct.s："<<ms.s<<"		MyStruct.c："<<std::setprecision(12)<<ms.c<<std::endl;
		//MyStruct.i：41          MyStruct.s：            MyStruct.c：3.1415926535
		std::cout<<"MyStruct.i："<<u<<"		MyStruct.s："<<v<<"		MyStruct.c："<<std::setprecision(12)<<w<<std::endl;
		//MyStruct.i：41          MyStruct.s：Empty Boy!          MyStruct.c：3.1415926535
	}
	/** @} */

    return 0;
}
//cl /EHsc /std:c++17 01UnpackStruct.cpp /link psapi.lib
