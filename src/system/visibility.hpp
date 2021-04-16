#pragma once
#include <functional>
#include <unordered_set>
#include "entt/entt.hpp"
#include "utils/geometry.hpp"

namespace radl {
void visibility_system(entt::registry& reg, entt::entity& ent);


namespace visibility_private {

/*
 * Benchmark results from example 6 (on my desktop):
 * - Using line_func, it averages 0.3625 uS per line.
 * - Using line_func_cancellable, it averages 0.25 uS per line.
 * - Using a naieve float based slope, it averages 0.2 uS per line.
 */

template <typename location_t_, typename navigator_t>
void internal_2d_sweep(const location_t_& position, const int& range,
                       const std::function<void(location_t_)>& set_visible,
                       const std::function<bool(location_t_)>& is_transparent,
                       const std::pair<int, int>& offset) {
    bool blocked      = false;
    const int start_x = navigator_t::get_x(position);
    const int start_y = navigator_t::get_y(position);
    const int end_x   = start_x + offset.first;
    const int end_y   = start_y + offset.second;

    auto func = [&blocked, &is_transparent, &set_visible, &range, &start_x,
                 &start_y](int X, int Y) {
        if(blocked) {
            return false;
        }
        float distance = distance2d(start_x, start_y, X, Y);
        if(distance <= range) {
            location_t_ pos = navigator_t::get_xy(X, Y);
            if(!blocked) {
                set_visible(pos);
            }
            if(!is_transparent(pos)) {
                blocked = true;
            }
        }
        return true;
    };
    line_func_cancellable(start_x, start_y, end_x, end_y, func);
}
}  // namespace visibility_private


/**
 * @brief Simple all-direction visibility sweep in 2 dimensions. This requires
 * that your location_t utilize an x and y component. *
 * @tparam location_t
 * @tparam navigator_t just like for path finding. It must support
 * get_x, get_y, and get_xy.
 * @param position where you are sweeping from.
 * @param range the number of tiles you can traverse.
 * @param set_visible a callback (such as bool set_visible(location_t & loc)) to
 * say "this is visible"
 * @param is_transparent a callback to ask your map if you can see
 * through a tile.
 */
template <typename location_t, typename navigator_t>
void visibility_sweep_2d(const location_t& position, const int& range,
                         std::function<void(location_t)> set_visible,
                         std::function<bool(location_t)> is_transparent) {
    // You can always see yourself
    set_visible(position);

    // Box-sweep
    for(int i = 0 - range; i < range; ++i) {
        visibility_private::internal_2d_sweep<location_t, navigator_t>(
            position, range, set_visible, is_transparent,
            std::make_pair(i, 0 - range));
        visibility_private::internal_2d_sweep<location_t, navigator_t>(
            position, range, set_visible, is_transparent,
            std::make_pair(i, range));
        visibility_private::internal_2d_sweep<location_t, navigator_t>(
            position, range, set_visible, is_transparent,
            std::make_pair(0 - range, i));
        visibility_private::internal_2d_sweep<location_t, navigator_t>(
            position, range, set_visible, is_transparent,
            std::make_pair(range, i));
    }
}


// TODO better field of view based on the bracket lib
template <typename location_t, typename navigator_t>
std::unordered_set<location_t>
field_of_view_set(location_t center, uint32_t range,
                  std::function<bool(location_t)> is_transparent) {}


}  // namespace radl
