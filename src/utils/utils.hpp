#pragma once

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

template <typename C>
auto longest_string_in_collection(const C& collection) {
    auto max_elem = std::max_element(collection.begin(), collection.end(),
                                     [](auto& lhs, auto& rhs) {
                                         return lhs.size() < rhs.size();
                                     });
    return max_elem;
}

}  // namespace radl
