#pragma once
#include <cmath>
#include "position.hpp"

namespace radl {

struct rect_t {
    position_t p1;
    position_t p2;
};

inline constexpr int width(const rect_t& rect) {
    auto& p1 = rect.p1;
    auto& p2 = rect.p1;
    return std::abs(p2.x - p1.x);
}

inline constexpr int height(const rect_t& rect) {
    auto& p1 = rect.p1;
    auto& p2 = rect.p1;
    return std::abs(p2.y - p1.y);
}

}  // namespace radl
