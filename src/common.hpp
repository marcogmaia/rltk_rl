#pragma once

struct position_t {
    int x;
    int y;

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
};
