// #include <iostream>
// #include <fmt/format.h>
// #include "destructible.hpp"
// #include "entity.hpp"
// #include "engine.hpp"

// destructible_t::destructible_t(float max_hp, float defense, const char* corpse_name)
//     : max_hp(max_hp)
//     , hp(max_hp)
//     , defense(defense)
//     , corpse_name(corpse_name) {}

// float destructible_t::take_damage(Entity* owner, float damage) {
//     auto final_damage = damage - defense;
//     if(final_damage < 0.f) {
//         final_damage = 0.f;
//     }
//     hp -= final_damage;
//     if(owner == engine::player) {
//         engine::gui.message(TCODColor::darkRed, "You lose {} hp.\n",
//                             static_cast<int>(final_damage));
//     }
//     else {
//         engine::gui.message(TCODColor::lightRed, "{} loses {} hp.\n",
//                             owner->name, static_cast<int>(final_damage));
//     }
//     if(hp <= 0.f) {
//         die(owner);
//     }
//     return final_damage;
// }

// void destructible_t::die(Entity* owner) {
//     if(owner == engine::player) {
//         engine::gui.message(TCODColor::darkestRed, "You died.\n", owner->name);
//     }
//     else {
//         engine::gui.message(TCODColor::darkestRed, "{} dies.\n", owner->name);
//     }
//     owner->ch     = '%';
//     owner->color  = TCODColor::darkRed;
//     owner->name   = corpse_name;
//     owner->blocks = false;

//     // make sure the corpse is sent to the back of the rendering queue
// }

// DestructibleEnemy::DestructibleEnemy(float max_hp, float defense,
//                                      const char* corpse_name)
//     : destructible_t(max_hp, defense, corpse_name) {}

// void DestructibleEnemy::die(Entity* owner) {
//     destructible_t::die(owner);
// }

// DestructiblePlayer::DestructiblePlayer(float max_hp, float defense,
//                                        const char* corpse_name)
//     : destructible_t(max_hp, defense, corpse_name) {}

// void DestructiblePlayer::die(Entity* owner) {
//     destructible_t::die(owner);
// }
