#pragma once

#include <vector>
#include <string>
#include <fmt/format.h>
#include "utils/colors.hpp"
#include "entt/entity/registry.hpp"

namespace radl::component {

struct log_entry_t {
    std::string log;
    color_t fg = WHITE;
    color_t bg = BLACK;
};

struct game_log_t {
    std::vector<log_entry_t> entries;
};

template <typename... Args>
void log_add_entry(entt::registry& reg, const color_t& color,
                   const std::string& fmt, Args... args) {
    log_entry_t entry = {
        .log = fmt::format(fmt, args...),
        .fg  = color,
    };
    reg.ctx<game_log_t>().entries.push_back(entry);
}

}  // namespace radl::component
