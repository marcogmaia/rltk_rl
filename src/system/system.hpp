#pragma once

#include "system/visibility.hpp"
#include "system/player_action.hpp"

namespace radl::system {

void systems_run();

void systems_player();

// XXX debugando
void player_system(const sf::Event &ev) ;

}  // namespace radl::system
