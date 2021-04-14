#include "component/renderable.hpp"

namespace radl::component {
const std::string& get_name(entt::entity ent) {
    auto& name = engine::reg.get<name_t>(ent);
    return name.name;
}
}  // namespace radl::component
