#include <algorithm>
#include "rng.hpp"

namespace radl::rng {

random_number_generator rng;


int random_number_generator::fastrand() {
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

random_number_generator::random_number_generator() {
    initial_seed = time(nullptr);
    g_seed       = initial_seed;
}

random_number_generator::random_number_generator(const int seed) {
    initial_seed = seed;
    g_seed       = initial_seed;
}

random_number_generator::random_number_generator(const std::string seed) {
    std::hash<std::string> hash_func;
    initial_seed = hash_func(seed);
    g_seed       = initial_seed;
}

int random_number_generator::roll_dice(const int& n, const int& d) {
    int total = 0;
    for(int i = 0; i < n; ++i) {
        total += ((fastrand() % d) + 1);
    }
    return total;
}

int random_number_generator::range(const int& min, const int& max) {
    auto m     = std::min(min, max);
    auto range = std::abs(max - min);
    auto ret   = (fastrand() % (range + 1)) + m;
    return ret;
}

}  // namespace radl::rng
