#include <algorithm>
#include "rng.hpp"

namespace radl::rng {

random_number_generator rng("Marco A. Maia");

random_number_generator::random_number_generator() {
    initial_seed = time(nullptr);
    rng          = std::mt19937_64(initial_seed);
}


random_number_generator::random_number_generator(const std::string& seed) {
    auto hsseed = std::hash<std::string>{}(seed);
    rng         = std::mt19937_64(hsseed);
}

// int random_number_generator::roll_dice(const int& n, const int& d) {
//     int total = 0;
//     // for(int i = 0; i < n; ++i) {
//     // total += ((fastrand() % d) + 1);
//     // }

//     return total;
// }

int random_number_generator::range(const int& min, const int& max) {
  
    std::uniform_int_distribution<int> udist(min, max);

    return udist(this->rng);
}

}  // namespace radl::rng
