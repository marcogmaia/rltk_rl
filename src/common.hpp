#pragma once

#include <compare>

#include "libtcod.hpp"

struct position_t {
    int x;
    int y;


    position_t(int a, int b)
        : x(a)
        , y(b){};
    // copy constructor
    position_t(const position_t& rhs) = default;

    position_t operator/(int div) {
        auto pt = position_t{x / div, y / div};
        return pt;
    }

    position_t operator+(const position_t& rhs) {
        auto pt = position_t{(x + rhs.x), (y + rhs.y)};
        return pt;
    }

    position_t operator-(const position_t& rhs) {
        auto pt = position_t{(x - rhs.x), (y - rhs.y)};
        return pt;
    }

    auto operator<=>(const position_t& rhs) const = default;

    bool operator==(const position_t& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const position_t& rhs) const {
        return !(*this == rhs);
    }
};

struct rect_t {
    int x;
    int y;
    int w;
    int h;

    rect_t() = default;

    rect_t(int xi, int yi, int wi, int hi)
        : x(xi)
        , y(yi)
        , w(wi)
        , h(hi) {}

    position_t center() const {
        return position_t{x + w / 2, y + h / 2};
    }

    position_t random_pos() const {
        auto* rng = TCODRandom::getInstance();
        // treating the exceptional cases
        auto rw = std::abs(w);
        auto rh = std::abs(h);
        if(rw == 0 || rh == 0) {
            rw = rh = 1;
        }
        // now we can get a valid position
        auto rx = rng->getInt(x, x + rw - 1);
        auto ry = rng->getInt(y, y + rh - 1);
        return position_t{rx, ry};
    }
};

template <typename Fw_iter, typename Func>
void adjacent_pair(Fw_iter first, Fw_iter last, Func f) {
    if(first != last) {
        Fw_iter trailer = first;
        ++first;
        for(; first != last; ++first) {
            f(*trailer, *first);
        }
    }
}