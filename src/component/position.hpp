#pragma once

#include <cmath>
#include <utility>
#include <functional>

#include "entt/entity/registry.hpp"
#include "utils/geometry.hpp"

namespace radl::component {

template <typename T>
std::pair<T, T> operator+(const std::pair<T, T>& left,
                          const std::pair<T, T>& right) {
    return {left.first + right.first, left.second + right.second};
}

template <typename T>
std::pair<T, T> operator-(const std::pair<T, T>& left,
                          const std::pair<T, T>& right) {
    return {left.first - right.first, left.second - right.second};
}

using position_t = std::pair<int, int>;

inline position_t get_position_from_entity(entt::registry& reg,
                                           entt::entity ent) {
    auto pos = reg.get<position_t>(ent);
    return pos;
}

inline double distance_manhattan(const position_t& pos1,
                                 const position_t& pos2) {
    auto [x1, y1] = pos1;
    auto [x2, y2] = pos2;
    return distance2d_manhattan(x1, y1, x2, y2);
}

inline double distance_pythagoras(const position_t& pos1,
                                  const position_t& pos2) {
    auto [x1, y1] = pos1;
    auto [x2, y2] = pos2;
    return distance2d(x1, y1, x2, y2);
}

struct position_t_2 {
    int x = 0;
    int y = 0;

    position_t_2 operator-(const position_t_2& right) {
        return {x - right.x, y - right.y};
    }

    position_t_2 operator+(const position_t_2& right) {
        return {x + right.x, y + right.y};
    }

    inline bool operator==(const position_t_2& rhs) {
        return x == rhs.x && y == rhs.y;
    }

    // double distance_to(const position_t &rhs) {

    // }
};

struct PosHasher {
    inline size_t operator()(const position_t& pos) const {
        return std::hash<int>()(pos.first)
               ^ ((std::hash<int>()(pos.second) << 1) >> 1);
    }
};

}  // namespace radl::component
