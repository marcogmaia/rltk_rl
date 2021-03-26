#pragma once

#include <cmath>

namespace radl {

// Position component
struct position_t {
    int x;
    int y;
};

constexpr position_t operator+(const position_t lhs, const position_t rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

constexpr position_t& operator+=(position_t& lhs, const position_t rhs) {
    return lhs = lhs + rhs;
}

constexpr position_t operator-(const position_t lhs, const position_t rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

constexpr position_t operator*(const position_t lhs, const int rhs) {
    return {lhs.x * rhs, lhs.y * rhs};
}

constexpr bool operator==(const position_t lhs, const position_t rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr bool operator!=(const position_t lhs, const position_t rhs) {
    return !(lhs == rhs);
}

inline int distance2(const position_t a, const position_t b) {
    const int dx = a.x - b.x;
    const int dy = a.y - b.y;
    return dx * dx + dy * dy;
}

inline float distance(const position_t a, const position_t b) {
    return sqrtf32(distance2(a, b));
}

}  // namespace radl
