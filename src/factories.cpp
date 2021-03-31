#include "factories.hpp"
#include "system/camera.hpp"


namespace radl {

void player_factory(entt::registry& reg, entt::entity ent,
                    const position_t& pos, const radl::vchar_t& vch) {
    using namespace world;
    reg.emplace<player_t>(ent);
    reg.emplace<viewshed_t>(ent, 16);
    reg.emplace<movable_t>(ent);
    reg.emplace<renderable_t>(ent, renderable_t{vch});
    reg.emplace<position_t>(ent, pos);
    // TODO observer ? why ?
}

}  // namespace radl