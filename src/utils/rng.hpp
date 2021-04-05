#pragma once

#include "rltk/rltk.hpp"

namespace radl::rng {


class random_number_generator {
public:
    random_number_generator();
    explicit random_number_generator(const int seed);
    explicit random_number_generator(const std::string& seed);

    int roll_dice(const int& n, const int& d);
    int range(const int& min, const int& max);
    int initial_seed;

private:
    int fastrand();
    int g_seed;
};

extern random_number_generator rng;

}  // namespace radl::rng
