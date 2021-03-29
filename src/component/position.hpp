#pragma once

#include <cmath>
#include <utility>

namespace radl {

template <typename T>
std::pair<T, T> operator+(const std::pair<T, T>& left,
                          const std::pair<T, T>& right) {
    return {left.first + right.first, left.second + right.second};
}

using position_t = std::pair<int, int>;

// Position component
// struct position_t {
//     int x;
//     int y;
// };
//
// constexpr position_t
// operator+(const position_t lhs, const position_t rhs) {
//     return {lhs.first + rhs.first, lhs.second + rhs.second};
// }

// constexpr position_t& operator+=(position_t& lhs, const position_t rhs) {
//     return lhs = lhs + rhs;
// }

// constexpr position_t operator-(const position_t lhs, const position_t rhs) {
//     return {lhs.first - rhs.first, lhs.second - rhs.second};
// }

// constexpr position_t operator*(const position_t lhs, const int rhs) {
//     return {lhs.first * rhs, lhs.second * rhs};
// }

// constexpr bool operator==(const position_t lhs, const position_t rhs) {
//     return lhs.first == rhs.first && lhs.second == rhs.second;
// }

// constexpr bool operator!=(const position_t lhs, const position_t rhs) {
//     return !(lhs == rhs);
// }

// inline int distance2(const position_t a, const position_t b) {
//     const int dx = a.first - b.first;
//     const int dy = a.second - b.second;
//     return dx * dx + dy * dy;
// }

// inline float distance(const position_t a, const position_t b) {
//     return sqrtf32(distance2(a, b));
// }

}  // namespace radl
