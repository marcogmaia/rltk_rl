#include "factories.hpp"
#include "system/camera.hpp"


namespace radl {

void player_factory(entt::registry& reg, entt::entity ent,
                    const position_t& pos, const radl::vchar_t& vch) {
    using namespace world;
    // FIXME only if marked as Player trigger this
    reg.on_construct<position_t>().connect<&camera_update>();
    reg.on_update<position_t>().connect<&camera_update>();

    // reg.on_construct<position_t>().connect<&update_viewshed>();
    // reg.on_update<position_t>().connect<&update_viewshed>();

    reg.emplace<Player>(ent);
    reg.emplace<viewshed_t>(ent);
    reg.emplace<Movable>(ent);
    reg.emplace<Renderable>(ent, Renderable{vch});
    reg.emplace<position_t>(ent, pos);
    // TODO observer ?
}

}  // namespace radl