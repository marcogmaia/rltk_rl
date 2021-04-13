#pragma once
#include <cmath>
#include <fmt/format.h>
#include "utils/rng.hpp"
#include "component/position.hpp"


namespace radl::component {

struct rect_t {
    int x1;
    int y1;
    int x2;
    int y2;

    [[nodiscard]] inline bool intersect(const rect_t& rhs) const {
        return x1 <= rhs.x2 && x2 >= rhs.x1 && y1 <= rhs.y2 && y2 >= rhs.y1;
    }

    [[nodiscard]] inline bool contains(const position_t& rhs) const {
        const auto& [x, y] = rhs;
        return (x1 <= x && x < x2) && (y1 <= y && y < y2);
    }

    [[nodiscard]] inline position_t center() const {
        return position_t{(x1 + x2) / 2, (y1 + y2) / 2};
    }

    [[nodiscard]] inline position_t random_pos() const {
        int rand_x = rng::rng.range(x1, x2 - 1);
        int rand_y = rng::rng.range(y1, y2 - 1);
        return position_t{rand_x, rand_y};
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

inline rect_t rect_create(const position_t& p1, const position_t& p2) {
    return rect_t{p1.first, p1.second, p2.first, p2.second};
}

// inline rect_t rect_create(position_t p1, position_t p2) {
//     return rect_t{p1.first, p1.second, p2.first, p2.second};
// }

// inline rect_t rect_create(position_t&& p1, position_t&& p2) {
//     return rect_t{p1.first, p1.second, p2.first, p2.second};
// }

}  // namespace radl
