#pragma once
#include <compare>
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

    position_t center() {
        return position_t{x + w / 2, y + h / 2};
    }

    // int max_index() {
    //     // auto index = x + y * w;
    //     auto maxidx = w * h - 1;
    //     return maxidx;
    // }

    // template <typename C, typename F>
    // void fill(C& collection, F& func, int gw, int gh) {
    //     for(int xi = x; xi < (x + w); ++xi) {
    //         for(int yi = y; yi < (y + h); ++yi) {
    //             func(collection[xi + yi * gw]);
    //         }
    //     }
    // }
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