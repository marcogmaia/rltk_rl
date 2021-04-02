#include "factories.hpp"
#include "system/camera.hpp"

#include "engine.hpp"


namespace radl {

namespace engine::factory {

using namespace world;

void player_factory(entt::entity ent, const position_t& pos,
                    const vchar_t& vch) {
    reg.emplace<viewshed_t>(ent, 16);
    reg.emplace<movable_t>(ent);
    reg.emplace<renderable_t>(ent, renderable_t{vch});
    reg.emplace<position_t>(ent, pos);
    reg.emplace<blocks_t>(ent);
    reg.emplace<player_t>(ent);
}


void enemy_factory(const position_t& pos, vchar_t vch) {
    auto& map  = reg.ctx<Map>();
    auto& tile = Map::get_tile(reg, pos);
    auto enemy = reg.create();
    reg.emplace<renderable_t>(enemy, vch);
    reg.emplace<position_t>(enemy, pos);
    reg.emplace<destructible_t>(enemy);
    reg.emplace<blocks_t>(enemy);
    reg.emplace<enemy_t>(enemy);
    tile.entities.push_back(enemy);
}


}  // namespace engine::factory

}  // namespace radl
