#include "factories.hpp"
#include "system/camera.hpp"

#include "engine.hpp"

#include <fmt/format.h>

namespace radl {

namespace engine::factory {

using namespace world;

/**
 * @brief pushes entity id to tile position's entities
 *
 * @param ent
 * @param dst_post
 */
static void add_ent_to_map(entity ent, const position_t& dst_post) {
    auto& map = reg.ctx<world::Map>();
    map[dst_post].entities_here.push_back(ent);
}

void player_factory(entt::entity ent, const position_t& pos,
                    const vchar_t& vch) {
    reg.emplace<viewshed_t>(ent, 16);
    reg.emplace<renderable_t>(ent, renderable_t{vch});
    reg.emplace<position_t>(ent, pos);
    reg.emplace<destructible_t>(ent, destructible_t{
                                         .max_hp      = 100,
                                         .hp          = 100,
                                         .defense     = 5,
                                         .corpse_name = "ded",
                                     });
    reg.emplace<attacker_t>(ent, attacker_t{
                                     .power = 10,
                                 });
    reg.emplace<being_t>(ent, "Player");
    reg.emplace<blocks_t>(ent);
    reg.emplace<player_t>(ent);

    add_ent_to_map(ent, pos);
}


void enemy_factory(const position_t& pos, vchar_t vch, std::string_view name) {
    auto ent = reg.create();
    reg.emplace<renderable_t>(ent, vch);
    reg.emplace<viewshed_t>(ent, 12);
    reg.emplace<position_t>(ent, pos);
    reg.emplace<destructible_t>(ent,
                                destructible_t{
                                    .max_hp      = 30,
                                    .hp          = 30,
                                    .defense     = 1,
                                    .corpse_name = fmt::format("ded {}", name),
                                });
    reg.emplace<attacker_t>(ent, attacker_t{
                                     .power = 1,
                                 });
    reg.emplace<being_t>(ent, std::string(name));
    reg.emplace<blocks_t>(ent);
    add_ent_to_map(ent, pos);
}


}  // namespace engine::factory

}  // namespace radl
