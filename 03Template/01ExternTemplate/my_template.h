// my_template.h

#ifndef MY_TEMPLATE_H
#define MY_TEMPLATE_H
#include <string>
#include "my_fake.h"
template <typename T>
class MyTemplate {
public:
	inline static std::string msg{"OK"};
    void doSomething() {
        // 实现细节...
    }
    inline static My_Structure mstruct;
};

#endif // MY_TEMPLATE_H