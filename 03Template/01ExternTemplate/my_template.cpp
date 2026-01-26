// my_template.cpp

#include "my_template.h"

// 显式实例化定义
template class MyTemplate<int>;

// 或者为了避免链接时的重复实例化，在其他源文件中声明外部模板
//extern template class MyTemplate<int>;