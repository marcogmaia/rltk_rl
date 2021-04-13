#include <fmt/format.h>

#include "factories.hpp"
#include "system/camera.hpp"

#include "component/component.hpp"
#include "core/engine.hpp"


namespace radl {

namespace engine::factory {

/**
 * @brief pushes entity id to tile position's entities
 *
 * @param ent
 * @param dst_post
 */
static void add_ent_to_map(entity ent, const position_t& dst_post) {
    auto& map = engine::get_map();
    map[dst_post].entities_here.push_back(ent);
}

void player_factory(entt::entity ent, const position_t& pos,
                    const vchar_t& vch) {
    reg.emplace<viewshed_t>(ent, 16);
    reg.emplace<inventory_t>(ent);
    reg.emplace<renderable_t>(ent, renderable_t{
                                       vch,
                                       z_level_t::BEING,
                                   });
    reg.emplace<position_t>(ent, pos);
    reg.emplace<combat_stats_t>(ent, combat_stats_t{
                                         .max_hp  = 100,
                                         .hp      = 100,
                                         .defense = 5,
                                         .power   = 50,
                                     });
    reg.emplace<name_t>(ent, "Player");
    reg.emplace<blocks_t>(ent);
    reg.emplace<player_t>(ent);
    add_ent_to_map(ent, pos);
}


entity enemy_factory(const position_t& pos, vchar_t vch, const char* name) {
    auto ent = reg.create();
    reg.emplace<renderable_t>(ent, renderable_t{
                                       vch,
                                       z_level_t::BEING,
                                   });
    reg.emplace<viewshed_t>(ent, 12);
    reg.emplace<position_t>(ent, pos);
    reg.emplace<combat_stats_t>(ent, combat_stats_t{
                                         .max_hp  = 50,
                                         .hp      = 50,
                                         .defense = 5,
                                         .power   = 6,
                                     });
    reg.emplace<name_t>(ent, name);
    reg.emplace<blocks_t>(ent);
    reg.emplace<inventory_t>(ent);
    reg.emplace<enemy_t>(ent);
    add_ent_to_map(ent, pos);
    return ent;
}

entity item_factory(const char* item_name, item_t item, vchar_t vch) {
    auto ent = reg.create();
    reg.emplace<name_t>(ent, item_name);
    reg.emplace<item_t>(ent, item);
    reg.emplace<renderable_t>(ent, renderable_t{
                                       vch,
                                       z_level_t::ITEM,
                                   });
    return ent;
}


}  // namespace engine::factory

}  // namespace radl
