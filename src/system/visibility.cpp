// #include <functional>
// #include "visibility.hpp"
// #include "utils/geometry.hpp"
// #include "component/viewshed.hpp"
// #include "map.hpp"


// namespace radl::world {

// // void visibility_system(entt::registry& reg, entt::entity& ent) {
// //     auto& vshed = reg.get<viewshed_t>(ent);
// //     auto& map   = reg.get<Map>(ent);
// // }

// }  // namespace radl::world


// namespace radl {
// }  // namespace radl


// /**
//  * @brief Simple all-direction visibility sweep in 2 dimensions. This requires
//  * that your location_t utilize an x and y component.
//  * @tparam location_t_
//  * @tparam navigator_t
//  * @param position where you are sweeping from.
//  * @param range the number of tiles you can traverse.
//  * @param set_visible a callback (such as bool set_visible(location_t & loc))
//  * to say "this is visible"
//  * @param is_transparent a callback to ask your map if you can see
//  * through a tile.
//  */
// template <class location_t_, class navigator_t>
// void visibility_sweep_2d(const location_t_& position, const int& range,
//                          std::function<void(location_t_)> set_visible,
//                          std::function<bool(location_t_)> is_transparent) {
//     // You can always see yourself
//     set_visible(position);

//     // Box-sweep
//     for(int i = 0 - range; i < range; ++i) {
//         visibility_private::internal_2d_sweep<location_t_, navigator_t>(
//             position, range, set_visible, is_transparent,
//             std::make_pair(i, 0 - range));
//         visibility_private::internal_2d_sweep<location_t_, navigator_t>(
//             position, range, set_visible, is_transparent,
//             std::make_pair(i, range));
//         visibility_private::internal_2d_sweep<location_t_, navigator_t>(
//             position, range, set_visible, is_transparent,
//             std::make_pair(0 - range, i));
//         visibility_private::internal_2d_sweep<location_t_, navigator_t>(
//             position, range, set_visible, is_transparent,
//             std::make_pair(range, i));
//     }
// }
