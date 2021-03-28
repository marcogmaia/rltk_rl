
#pragma once
#include <concepts>
#include <iterator>

namespace radl {

template <typename F_Iter, typename F>
void adjacent_pairs(F_Iter begin, F_Iter end, F func) {
    if(begin == end) {
        return;
    }
    auto trailer = begin;
    ++begin;
    if(begin == end) {
        return;
    }
    for(; begin != end; ++trailer, ++begin) {
        func(*trailer, *begin);
    }
}

}  // namespace radl