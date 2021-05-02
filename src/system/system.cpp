#include <ranges>
#include <algorithm>

#include "spdlog/spdlog.h"

#include "system/ai.hpp"
#include "system/system.hpp"
#include "system/camera.hpp"
#include "system/factories.hpp"

#include "component/component.hpp"
#include "system/game_state.hpp"
#include "system/spawner.hpp"
#include "core/gui/gui.hpp"
#include "core/engine.hpp"

#include "system/map/dijkstra_map.hpp"
namespace radl::system {


void system_visibility() {
    fov_update();
}

void system_ai() {
    const auto& gstate = reg.ctx<game_state_t>();
    auto v_enemies     = reg.view<enemy_t, viewshed_t>();
    if(gstate == game_state_t::ENEMY_TURN) {
        const auto& player_pos = reg.get<position_t>(player);
        for(auto [e_ent, e_enemy, e_vshed] : v_enemies.each()) {
            // if player is visible
            if(e_vshed.visible_coordinates.contains(player_pos)) {
                e_enemy.memory                    = player_pos;
                e_enemy.remember_turns            = e_vshed.range * 2;
                e_enemy.remembers_target_position = true;
                // TODO an enemy needs to remember a path
                // every time an enemy sees the player, they construct the path
                // and store it in his memory
                // if the player is not adjacente it takes the next step in the
                // path
                ai_enemy_dijkstra_map(e_ent);
            }
            else if(e_enemy.remember_turns > 0) {
                --e_enemy.remember_turns;
                if(e_enemy.remember_turns <= 0) {
                    e_enemy.remembers_target_position = false;
                }
                ai_enemy_dijkstra_map(e_ent);
            }
            else {
                random_walk(reg, e_ent, reg.get<position_t>(e_ent));
            }
        }
    } else if(gstate == game_state_t::PLAYER_TURN) {
        // player AI ?
    }
}

void system_melee_combat() {
    auto ents_melee = reg.view<combat_stats_t, name_t, wants_to_melee_t>(
        entt::exclude<dead_t>);

    for(auto [ent, atker_stats, being, want_to_melee] : ents_melee.each()) {
        auto [target_stats, target_being]
            = reg.get<combat_stats_t, name_t>(want_to_melee.target);
        auto final_damage = atker_stats.power - target_stats.defense;
        final_damage      = std::max(final_damage, 0);
        new_damage(reg, want_to_melee.target, final_damage);
        particle_create(particle_type_t::ATTACK, 200.0,
                        reg.get<position_t>(want_to_melee.target));

        if(final_damage > 0) {
            color_t dmg_color = (ent == player) ? RED : LIGHT_RED;
            log_add_entry(reg, dmg_color, "{} hits {}, for {} hp.", being.name,
                          target_being.name, final_damage);
        } else {
            log_add_entry(reg, WHITE, "{} is unable to hit {}.", being.name,
                          target_being.name);
        }
        reg.remove<wants_to_melee_t>(ent);
    }
}

void system_destroy_deads() {
    const auto& gstate = reg.ctx<game_state_t>();
    auto deads         = reg.view<renderable_t, dead_t, position_t>();
    deads.each([&](entity ent, renderable_t& rend, dead_t& dead,
                   position_t& dead_pos) {
        rend.vchar   = {'%', DARK_RED, DARKEST_RED};
        rend.z_level = z_level_t::DEAD;
        reg.remove_if_exists<blocks_t>(ent);
        if(gstate != game_state_t::ENEMY_TURN) {
            return;
        }
        --dead.decompose_turns;
        if(dead.decompose_turns <= 0) {
            // remove from map
            auto& map = get_map();
            map[dead_pos].entities_here.remove(ent);
            // query_alive_entities_near_player();
            reg.destroy(ent);
        }
    });
}

void system_walk() {
    auto wanting_to_walk
        = reg.view<want_to_walk_t, viewshed_t>(entt::exclude<dead_t>);
    auto& map = get_map();

    auto fwalk = [&](auto ent) -> void {
        // want_to_walk_t& want_walk = wanting_to_walk.get<want_to_walk_t>(ent);
        auto [want_walk, vshed] = wanting_to_walk.get(ent);
        reg.remove<want_to_walk_t>(ent);

        auto can_walk_to
            = !is_occupied(want_walk.to) && map[want_walk.to].props.walkable;
        if(!can_walk_to) {
            return;
        }

        // if can walk
        auto& map = get_map();
        map.at(want_walk.from).remove_entity(ent);
        map.at(want_walk.to).insert_entity(ent);

        reg.replace<position_t>(ent, want_walk.to);
        vshed.dirty                  = true;
        reg.ctx<DijkstraMap>().dirty = true;
        reg.ctx<camera_t>().dirty    = true;
    };

    std::ranges::for_each(wanting_to_walk, fwalk);
}

void system_damage() {
    auto& map = get_map();

    auto sufferers
        = reg.view<name_t, suffer_damage_t, combat_stats_t, position_t>(
            entt::exclude<dead_t>);

    for(auto [ent, being, damage, stats, e_pos] : sufferers.each()) {
        auto total_damage
            = std::accumulate(damage.amount.cbegin(), damage.amount.cend(), 0);
        stats.hp -= total_damage;
        damage.amount.clear();

        spdlog::debug("{} suffers {} damage", being.name, total_damage);
        if(total_damage > 0) {
            auto& tile  = map.at(e_pos);
            tile.status = tile_status_t::BLOODIED;
            // recomputes the map only if any damage occurred
            reg.ctx<DijkstraMap>().dirty = true;
        }

        if(stats.hp <= 0) {
            reg.emplace<dead_t>(ent);
            auto& e_inventory = reg.get<inventory_t>(ent);
            spdlog::debug("{} drops {} items.", being.name,
                          e_inventory.items.size());
            auto& tile = map.at(e_pos);

            for(auto [item_id, item_bucket] : e_inventory.items) {
                auto& e_item = item_bucket.back();
                reg.emplace<position_t>(e_item, e_pos);
            }

            std::ranges::copy(e_inventory.get_items(),
                              std::back_inserter(tile.entities_here));

            reg.remove<suffer_damage_t>(ent);
            auto* name = &reg.get<name_t>(ent);
            name->name = name->dead_name;
            reg.remove<combat_stats_t>(ent);
        }
    }
}

void quaff_potion(entity who, entity pot) {
    auto& e_stats  = reg.get<combat_stats_t>(who);
    auto& potion   = reg.get<drinkable_effects_t>(pot);
    auto prev_heal = e_stats.hp;
    e_stats.hp += potion.healing;
    e_stats.hp = std::min(e_stats.max_hp, e_stats.hp);
    // log entry
    auto final_healing = e_stats.hp - prev_heal;
    particle_create(particle_type_t::HEALING, 200.0, reg.get<position_t>(who));
    log_add_entry(reg, LIGHT_GREEN, "healed for {} hp.", final_healing);
}

void system_item_use() {
    auto v_use = reg.view<wants_to_use_t, inventory_t>();
    for(auto [e_who, euse, einv] : v_use.each()) {
        if(!einv.contains(euse.what)) {
            continue;
        }
        // inventory contains item
        auto item = einv.get_item(euse.what);
        if(item.in_pack) {
            // use item
            switch(item.type) {
            case item_type_t::POTION: {
                if(item.characteristics.drinkable) {
                    quaff_potion(e_who, euse.what);
                }
            } break;
            default: break;
            }
            // remove item
            einv.remove_item(euse.what);
            reg.remove<wants_to_use_t>(e_who);
            // destroy entity
            reg.destroy(euse.what);
        }
    }
}


void system_item_collection() {
    auto v_pick_item
        = reg.view<wants_to_pickup_item_t, inventory_t, position_t>();

    for(auto [ent, epick, einv, epos] : v_pick_item.each()) {
        einv.add_item(epick.item);
        get_map().at(epos).remove_entity(epick.item);
        reg.remove<position_t>(epick.item);
        reg.remove<wants_to_pickup_item_t>(ent);
        auto& item   = reg.get<item_t>(epick.item);
        item.in_pack = true;
    }
}

void system_item_drop() {
    auto vdrop = reg.view<wants_to_drop_t, inventory_t, position_t>();
    for(auto [ent, edrop, einv, epos] : vdrop.each()) {
        if(!einv.contains(edrop.what)) {
            continue;
        }
        einv.get_item(edrop.what).in_pack = false;
        einv.remove_item(edrop.what);
        reg.remove<wants_to_drop_t>(ent);
        // put on the map and give a position component
        get_map().at(epos).insert_entity(edrop.what);
        reg.emplace<position_t>(edrop.what, epos);
    }
}

// I can use an event dispatcher and connect this function to its sink
void system_particle(double elapsed_time) {
    const auto& v_particles = reg.view<particle_t>();
    for(auto [ent, e_particle] : v_particles.each()) {
        e_particle.lifetime_ms -= elapsed_time;
        if(e_particle.lifetime_ms <= 0) {
            // destroy the entity, since the entity purpose is only to
            // display the particle effect
            reg.destroy(ent);
        }
        reg.ctx<camera_t>().dirty = true;
    }
}

void system_map() {
    auto& pos = reg.get<position_t>(player);
    reg.ctx<DijkstraMap>().compute(std::vector<position_t>{pos});
}

void system_render(double duration_ms) {
    static const auto& gstate      = reg.ctx<game_state_t>();
    reg.ctx<camera_t>().frame_time = duration_ms;
    switch(gstate) {
    case game_state_t::SHOW_INVENTORY:
    case game_state_t::SHOW_INVENTORY_DROP:
    case game_state_t::AWAITING_INPUT: {
        // render
        if(reg.valid(player) && reg.all_of<player_t>(player)) {
            system::camera();
            gui::render_gui();
        }
    }
    default: {
    } break;
    }
}

void system_camera() {
    auto& camera    = reg.ctx<camera_t>();
    camera.width    = engine::console->term_width;
    camera.height   = engine::console->term_height;
    camera.position = reg.get<position_t>(player);
    // gui::render_gui();
}

void restart_game_state() {
    reg.unset<Map>();
    reg.unset<game_log_t>();
    reg.clear();
}

constexpr int map_width  = 1024 / 16;
constexpr int map_height = 768 / 16;

void pre_run() {
    // initialize everything
    // create the map
    reg.set<Map>();
    auto& map = get_map();
    map.init(rect_t{0, 0, map_width, map_height});
    create_random_rooms(map);
    make_corridors_between_rooms(map);
    auto player_start_pos = map.rooms[0].center();
    player                = reg.create();
    factory::player_factory(player, player_start_pos,
                            vchar_t{'@', YELLOW, BLACK});
    add_enemies();

    game_state_init();

    spdlog::info("entities created: {}", reg.alive());
    system::systems_run();
}

game_state_t game_state_inventory_show() {
    // ! Ou faria associação com o reg
    using gui::item_menu_result_t;
    auto [menu_res, ent] = gui::render_inventory_use();
    auto& inv_ui         = *term(gui::UI_INVENTORY_POPUP);

    switch(menu_res) {
    case item_menu_result_t::CANCEL: {
        inv_ui.clear();
        return game_state_t::AWAITING_INPUT;
    } break;
    case item_menu_result_t::NO_RESPONSE: break;
    case item_menu_result_t::SELECTED: {
        inv_ui.clear();
        reg.emplace<wants_to_use_t>(player, ent);
        return game_state_t::PLAYER_TURN;
    } break;
    }
    return game_state_t::SHOW_INVENTORY;
}

game_state_t game_state_inventory_drop() {
    using gui::item_menu_result_t;
    auto [menu_res, ent] = gui::render_inventory_drop();
    auto& inv_ui         = *term(gui::UI_INVENTORY_POPUP);

    switch(menu_res) {
    case item_menu_result_t::CANCEL: {
        inv_ui.clear();
        return game_state_t::AWAITING_INPUT;
    } break;
    case item_menu_result_t::NO_RESPONSE: break;
    case item_menu_result_t::SELECTED: {
        inv_ui.clear();
        reg.emplace<wants_to_drop_t>(player, ent);
        return game_state_t::PLAYER_TURN;
    } break;
    }
    return game_state_t::SHOW_INVENTORY_DROP;
}

// I need a conventional state machine
void system_game_state([[maybe_unused]] double elapsed_time) {
    auto& game_state = reg.ctx<game_state_t>();

    switch(game_state) {
    case game_state_t::PRE_RUN: {
        pre_run();
        game_state = game_state_t::AWAITING_INPUT;
    } break;

    case game_state_t::AWAITING_INPUT: {
        // game_state = player_input();
        // TODO modify this
        system_player();
    } break;

    case game_state_t::SHOW_INVENTORY: {
        game_state = game_state_inventory_show();
    } break;

    case game_state_t::SHOW_INVENTORY_DROP: {
        game_state = game_state_inventory_drop();
    } break;

    case game_state_t::PLAYER_TURN: {
        system::systems_run();
        game_state = game_state_t::ENEMY_TURN;
    } break;

    case game_state_t::ENEMY_TURN: {
        // ## if player is dead then restart game
        system::systems_run();

        if(reg.all_of<dead_t>(player)) {
            game_state = game_state_t::DEFEAT;
        } else {
            game_state = game_state_t::AWAITING_INPUT;
        }
    } break;

    case game_state_t::DEFEAT: {
        restart_game_state();
        game_state = game_state_t::PRE_RUN;
    } break;

    default: break;
    }
}

// later we can group the components and run the groups in parallel
void systems_run() {
    system_visibility();
    system_ai();
    system_melee_combat();
    system_damage();
    system_item_collection();
    system_item_drop();
    system_item_use();
    system_destroy_deads();
    system_walk();
    system_map();
    system_camera();
}

void player_system(const sf::Event& ev) {
    auto dpos = get_delta_pos(ev);
    spdlog::debug("player dpos: ({}, {})", dpos.first, dpos.second);
}

void phase_mouse_cursor(double elapsed_time) {
    static double alpha_cursor = 0.0;
    static double time_frame   = 0.0;
    static bool increasing     = true;
    time_frame += elapsed_time * 0.42;

    if(time_frame >= 1.0) {
        auto inc = static_cast<int>(time_frame);
        time_frame -= inc;
        if(increasing) {
            alpha_cursor += inc;
        } else {
            alpha_cursor -= inc;
        }
        if(alpha_cursor >= 255) {
            increasing   = false;
            alpha_cursor = 255;
        }
        if(alpha_cursor <= 0) {
            increasing   = true;
            alpha_cursor = 0;
        }
    }
    term(gui::UI_MOUSE)->set_alpha(static_cast<int>(alpha_cursor));
}


void init_systems() {
    reg.set<camera_t>();
    engine::engine.dispatcher.sink<double>().connect<&system_render>();
    engine::engine.dispatcher.sink<double>().connect<&system_particle>();
    engine::engine.dispatcher.sink<double>().connect<&phase_mouse_cursor>();
    engine::engine.dispatcher.sink<double>().connect<&system_game_state>();
}


}  // namespace radl::system
