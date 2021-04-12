#pragma once

#include <vector>
#include <string>
#include "utils/colors.hpp"

namespace radl::component {

struct log_entry_t {
    std::string log;
    color_t fg = WHITE;
    color_t bg = BLACK;
};

struct game_log_t {
    std::vector<log_entry_t> entries;
};

}  // namespace radl::component
