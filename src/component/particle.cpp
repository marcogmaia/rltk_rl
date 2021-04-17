#include "component/particle.hpp"
#include "component/vchar.hpp"
#include "component/renderable.hpp"
#include "component/colors.hpp"

namespace radl::component {

void create_particle(particle_type_t type, double duration_ms,
                     const position_t& pos) {
    vchar_t particle_vch;
    switch(type) {
    case particle_type_t::ATTACK: {
        particle_vch.glyph      = glyph::ATTACK;
        particle_vch.foreground = RED;
        // particle_vch.foreground = colors::
    } break;
    case particle_type_t::HEALING: {
        particle_vch.glyph      = glyph::HEALING;
        particle_vch.foreground = GREEN;
    } break;
    }
    auto ent = reg.create();
    reg.emplace<position_t>(ent, pos);
    reg.emplace<renderable_t>(ent, renderable_t{particle_vch});
    reg.emplace<particle_t>(ent, duration_ms);
}

}  // namespace radl::component
