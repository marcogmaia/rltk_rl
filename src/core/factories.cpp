#include "factories.hpp"
#include "system/camera.hpp"

#include "engine.hpp"


namespace radl {

namespace engine::factory {

using namespace world;

static void add_ent_to_map(entity ent, const position_t& dst_post) {
    auto& map = reg.ctx<world::Map>();
    map[dst_post].entities_here.push_front(ent);
}

void player_factory(entt::entity ent, const position_t& pos,
                    const vchar_t& vch) {
    reg.emplace<viewshed_t>(ent, 16);
    // reg.emplace<blocks_t>(ent);
    reg.emplace<renderable_t>(ent, renderable_t{vch});
    reg.emplace<position_t>(ent, pos);
    reg.emplace<destructible_t>(ent);
    reg.emplace<being_t>(ent, "Player");
    reg.emplace<player_t>(ent);

    add_ent_to_map(ent, pos);
}


void enemy_factory(const position_t& pos, vchar_t vch, std::string_view name) {
    auto enemy = reg.create();
    reg.emplace<renderable_t>(enemy, vch);
    reg.emplace<viewshed_t>(enemy, 12);
    reg.emplace<position_t>(enemy, pos);
    reg.emplace<destructible_t>(enemy);
    // reg.emplace<blocks_t>(enemy);
    reg.emplace<being_t>(enemy, std::string(name));

    add_ent_to_map(enemy, pos);
}


}  // namespace engine::factory

}  // namespace radl
