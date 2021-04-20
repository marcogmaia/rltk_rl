#pragma once
#include <random>

#include "rltk/rltk.hpp"

namespace radl::rng {


class random_number_generator {
private:
    int g_seed;
    std::mt19937_64 rng;

public:
    random_number_generator();
    explicit random_number_generator(const int seed);
    explicit random_number_generator(const std::string& seed);

    int roll_dice(const int& n, const int& d);
    int range(const int& min, const int& max);

    /**
     * @brief Picks a random value in range [first - last] and puts in "output"
     *
     * @tparam InIter
     * @tparam OutIter
     * @param first
     * @param last
     * @param output
     * @return true
     * @return false
     */
    template <typename InIter, typename OutIter>
    bool random_choice(InIter first, InIter last, OutIter output) {
        // if empty
        if(first == last) {
            return false;
        }
        std::sample(first, last, output, 1, this->rng);
        return true;
    }

    uint_fast64_t initial_seed;
};

extern random_number_generator rng;

}  // namespace radl::rng
