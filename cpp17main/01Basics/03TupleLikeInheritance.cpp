#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <type_traits>
#include "../TestMacro.h"

// 在ECS中，结构体继承的形式进行解析的过程
// 基础组件
struct Transform {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    
    // 支持结构化绑定的成员函数
    template<std::size_t I>
    auto& get() & {
        static_assert(I < 3);
        if constexpr (I == 0) return x;
        else if constexpr (I == 1) return y;
        else return z;
    }
    
    template<std::size_t I>
    const auto& get() const& {
        static_assert(I < 3);
        if constexpr (I == 0) return x;
        else if constexpr (I == 1) return y;
        else return z;
    }
    
    template<std::size_t I>
    auto&& get() && {
        static_assert(I < 3);
        if constexpr (I == 0) return std::move(x);
        else if constexpr (I == 1) return std::move(y);
        else return std::move(z);
    }
};

// 必须在 std 命名空间中专用化
namespace std {
    template<> struct tuple_size<Transform> 
        : std::integral_constant<size_t, 3> {};
    //统一类型就可以一句表示完成
    template<size_t I> struct tuple_element<I, Transform> { using type = float; };
}

// 物理组件
struct Physics : Transform {
    float velocity_x = 0.0f, velocity_y = 0.0f, velocity_z = 0.0f;
    float mass = 1.0f;
    
    // 支持结构化绑定
    template<std::size_t I>
    auto& get() & {
        static_assert(I < 7);
        if constexpr (I < 3) {
            // 委托给基类的get
            return static_cast<Transform*>(this)->template get<I>();
        } else if constexpr (I == 3) return velocity_x;
        else if constexpr (I == 4) return velocity_y;
        else if constexpr (I == 5) return velocity_z;
        else return mass;
    }
    
    template<std::size_t I>
    const auto& get() const& {
        static_assert(I < 7);
        if constexpr (I < 3) {
            return static_cast<const Transform*>(this)->template get<I>();
        } else if constexpr (I == 3) return velocity_x;
        else if constexpr (I == 4) return velocity_y;
        else if constexpr (I == 5) return velocity_z;
        else return mass;
    }
    
    template<std::size_t I>
    auto&& get() && {
        static_assert(I < 7);
        if constexpr (I < 3) {
            return std::move(static_cast<Transform*>(this)->template get<I>());
        } else if constexpr (I == 3) return std::move(velocity_x);
        else if constexpr (I == 4) return std::move(velocity_y);
        else if constexpr (I == 5) return std::move(velocity_z);
        else return std::move(mass);
    }
};

namespace std {
    template<> struct tuple_size<Physics> 
        : std::integral_constant<size_t, 7> {};
    
    
    // 如果某个序号的不是float，在前面声明，比如第二个是字符串
    //template<> struct tuple_element<1, Physics> { using type = std::string; };
    template<size_t I> struct tuple_element<I, Physics> { using type = float; };
}

// 渲染组件
struct Renderable : Transform {
    std::string texture = "default.png";
    float color_r = 1.0f, color_g = 1.0f, color_b = 1.0f, color_a = 1.0f;
    
    template<std::size_t I>
    decltype(auto) get() & {
        static_assert(I < 8);
        if constexpr (I < 3) {
            return static_cast<Transform*>(this)->template get<I>();
        } else if constexpr (I == 3) return texture;
        else if constexpr (I == 4) return color_r;
        else if constexpr (I == 5) return color_g;
        else if constexpr (I == 6) return color_b;
        else return color_a;
    }
    
    template<std::size_t I>
    decltype(auto) get() const& {
        static_assert(I < 8);
        if constexpr (I < 3) {
            return static_cast<const Transform*>(this)->template get<I>();
        } else if constexpr (I == 3) return texture;
        else if constexpr (I == 4) return color_r;
        else if constexpr (I == 5) return color_g;
        else if constexpr (I == 6) return color_b;
        else return color_a;
    }
    
    template<std::size_t I>
    decltype(auto) get() && {
        static_assert(I < 8);
        if constexpr (I < 3) {
            return std::move(static_cast<Transform*>(this)->template get<I>());
        } else if constexpr (I == 3) return std::move(texture);
        else if constexpr (I == 4) return std::move(color_r);
        else if constexpr (I == 5) return std::move(color_g);
        else if constexpr (I == 6) return std::move(color_b);
        else return std::move(color_a);
    }
};

namespace std {
    template<> struct tuple_size<Renderable> 
        : std::integral_constant<size_t, 8> {};
    template<> struct tuple_element<3, Renderable> { using type = std::string; };
    template<std::size_t I> struct tuple_element<I, Renderable>{ using type = float; };
}

// 非成员get函数
template<std::size_t I, typename T>
decltype(auto) get(T&& obj) {
    return std::forward<T>(obj).template get<I>();
}


int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);  
	{
		 // 测试 Transform
		Transform t{2.0f, 3.0f, 4.0f};
		auto [x, y, z] = t;
		std::cout << "Transform: (" << x << ", " << y << ", " << z << ")\n";
		
		// 测试 Physics
		Physics p;
		p.x = 1.0f; p.y = 2.0f; p.z = 3.0f;
		p.velocity_x = 0.5f;
		p.mass = 10.0f;
		
		auto [px, py, pz, vx, vy, vz, m] = p;
		std::cout << "\nPhysics:\n";
		std::cout << "  Position: (" << px << ", " << py << ", " << pz << ")\n";
		std::cout << "  Velocity: (" << vx << ", " << vy << ", " << vz << ")\n";
		std::cout << "  Mass: " << m << "\n";
		
		// 通过get访问特定成员
		std::cout << "\nUsing get<>:\n";
		std::cout << "  Position X: " << get<0>(p) << "\n";
		std::cout << "  Mass: " << get<6>(p) << "\n";
		
		// 修改值
		get<0>(p) = 10.0f;
		get<6>(p) = 20.0f;
		std::cout << "  Modified Position X: " << p.x << "\n";
		std::cout << "  Modified Mass: " << p.mass << "\n";
		
		// 测试 Renderable
		Renderable r;
		r.x = 5.0f;
		r.texture = "player.png";
		r.color_r = 0.8f;
		
		auto [rx, ry, rz, tex, cr, cg, cb, ca] = r;
		std::cout << "\nRenderable:\n";
		std::cout << "  Position: (" << rx << ", " << ry << ", " << rz << ")\n";
		std::cout << "  Texture: " << tex << "\n";
		std::cout << "  Color: (" << cr << ", " << cg << ", " << cb << ", " << ca << ")\n";
	}
    return 0;
}
//cl /EHsc /std:c++17 03TupleLikeInheritance.cpp /link psapi.lib 