#pragma once
#include <cmath>
#include <fmt/format.h>
#include "../component/position.hpp"


namespace radl {

struct rect_t {
    int x1;
    int y1;
    int x2;
    int y2;

    [[nodiscard]] inline bool intersect(const rect_t& rhs) const {
        return x1 <= rhs.x2 && x2 >= rhs.x1 && y1 <= rhs.y2 && y2 >= rhs.y1;
    }

    [[nodiscard]] inline position_t center() const {
        return position_t{(x1 + x2) / 2, (y1 + y2) / 2};
    }

    [[nodiscard]] inline size_t area() const {
        int w = std::abs(x2 - x1);
        int h = std::abs(y2 - y1);
        return w * h;
    }

    [[nodiscard]] inline int width() const {
        return std::abs(x2 - x1);
    }

    [[nodiscard]] inline int height() const {
        return std::abs(y2 - y1);
    }

    [[nodiscard]] inline std::string to_string() const {
        return fmt::format("[({}, {}), ({}, {})]", x1, y1, x2, y2);
    }

    [[nodiscard]] inline bool is_inside(const rect_t& rhs) const {
        return x1 > rhs.x1 && x2 < rhs.x2 && y1 > rhs.y1 && y2 < rhs.y2;
    }
};

// inline constexpr int width(const rect_t& rect) {
//     auto& p1 = rect.p1;
//     auto& p2 = rect.p1;
//     return std::abs(p2.x - p1.x);
// }

// inline constexpr int height(const rect_t& rect) {
//     auto& p1 = rect.p1;
//     auto& p2 = rect.p1;
//     return std::abs(p2.y - p1.y);
// }

}  // namespace radl
