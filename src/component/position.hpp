#pragma once

#include <cmath>
#include <utility>

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

struct PosHasher {
    inline size_t operator()(const position_t& pos) const {
        return std::hash<int>()(pos.first)
               ^ ((std::hash<int>()(pos.second) << 1) >> 1);
    }
};

}  // namespace radl::component
