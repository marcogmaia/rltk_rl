#pragma once

#include "component/camera.hpp"
#include "component/component.hpp"
#include "entt/entity/registry.hpp"

namespace radl::engine {
extern rltk::virtual_terminal* console;
}  // namespace engine

namespace radl::system {

/**
 * @brief update the player camera and render order
 *
 * @param ent
 */
// void camera_update(entt::entity ent);

void camera();

// extern std::unique_ptr<std::vector<entt::entity>> active_entities_near_player;

// auto render_pos = [&](int xr, int yr) -> position_t {
//     return position_t{xr - xci, yr - yci};
// };

// template <typename... Ts>
// void render_components(int px, int py, int offx, int offy, Ts... comps) {
//     auto view = reg.view<comps...>();
//     for(auto ent : view) {
//         auto& pos  = view.get<position_t>(ent);
//         auto& rend = view.get<renderable_t>(ent);
//     }
// }

}  // namespace radl
