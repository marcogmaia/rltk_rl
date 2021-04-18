#include "component/renderable.hpp"


namespace radl::component {

const std::string& get_name(registry& reg, entity ent) {
    auto& name = reg.get<name_t>(ent);
    return name.name;
}

}  // namespace radl::component
