#pragma once
#include <string>
#include "entt/entity/fwd.hpp"
#include "rltk/rltk.hpp"
#include "vchar.hpp"
#include "component/common.hpp"

// To render anything we need to know the position, colors and console
// the position we get directly from the position_t struct of the entity

namespace radl::component {

enum class z_level_t {
    GROUND = 0,
    DEAD,
    ITEM,
    BEING,
    PLAYER,
    MAX,
};

struct renderable_t {
    vchar_t vchar;
    z_level_t z_level = z_level_t::GROUND;
};

struct name_t {
    std::string name;
    std::string dead_name;
};

const std::string& get_name(registry& reg, entt::entity ent);

}  // namespace radl::component
