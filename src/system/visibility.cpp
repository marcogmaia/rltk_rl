#include "visibility.hpp"
#include "map.hpp"
#include "component/viewshed.hpp"


namespace radl::world {

void visibility_system(entt::registry& reg, entt::entity& ent) {
    auto& vshed = reg.get<viewshed_t>(ent);
    auto& map   = reg.get<Map>(ent);
}

}  // namespace radl::world