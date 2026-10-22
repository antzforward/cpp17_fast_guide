#pragma once
// 学习用入口。正式项目去掉 start_vs_cmd.bat 里的 INCLUDE，并删除 third_party/range-v3 即可。
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/functional/pipeable.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/take_while.hpp>
#include <range/v3/view/generate.hpp>
#include <utility>

namespace linq {
namespace views = ranges::views;

// range-v3 的 views 可以接在 | 后面；for_each 算法本身不是管道。
// 这里用它提供的 make_pipeable，把 for_each 收成 LINQ 最后一段。
template <class F>
auto foreach(F f) {
    return ranges::make_pipeable([f = std::move(f)](auto&& rng) {
        ranges::for_each(static_cast<decltype(rng)>(rng), f);
    });
}

// 对应 LINQ 的 Aggregate / C++ 的 fold。init 是初值。
template <class T, class BinOp>
auto fold(T init, BinOp op) {
    return ranges::make_pipeable([init, op](auto&& rng) {
        return ranges::accumulate(static_cast<decltype(rng)>(rng), init, op);
    });
}
}
