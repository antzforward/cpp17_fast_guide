#include <iostream>
#include <functional>
//重点哦，基本上作为广播使用了。
class MyClass {
public:
    void memberFunction(int data) {
        std::cout << "Value: " << data << std::endl;
    }
};

class MulticastDelegate {
private:
    int nextId = 0;
    std::unordered_map<int, std::function<void()>> idToFunc;

public:
    int add(const std::function<void()>& func) {
        int id = nextId++;
        idToFunc[id] = func;
        return id;
    }

    void remove(int id) {
        auto it = idToFunc.find(id);
        if (it != idToFunc.end()) {
            idToFunc.erase(it);
        }
    }

    void invoke() {
        for (const auto& pair : idToFunc) {
            pair.second();
        }
    }
};

void hello() {
    std::cout << "Hello, ";
}

void world() {
    std::cout << "world!\n";
}

void foo(int a,int b, int c){
	std::cout <<a<<','<<b<<','<<c<<std::endl;
}

int main() {
    MyClass myObject;
    
    // 我们想在回调中使用成员函数并传入一个固定值
    auto boundFunc = std::bind(&MyClass::memberFunction, &myObject, 42);
    auto bindFunc2 = std::bind(foo, 1,2,3);//例子不好，std::placeholders::_1 表示函数调用的第一个参数，是bind之后的函数
    MulticastDelegate del;
    
    int idHello = del.add(hello); // 注册 hello 函数
    int idWorld = del.add(world); // 注册 world 函数
    int bound = del.add( boundFunc );//注册memberFunction
    int idBind = del.add(bindFunc2);

    del.invoke(); // 输出: Hello, world!

    del.remove(idHello); // 移除 hello 函数
    del.invoke(); // 输出: world!
    
    del.remove(bound);
    del.invoke();

    return 0;
}