#include <ranges>
#include <algorithm>

#include "system/system.hpp"

#include "component/component.hpp"
#include "core/engine.hpp"

namespace radl::system {

namespace {

using engine::game_state_t;
using engine::reg;
using namespace world;

}  // namespace

void system_visibility() {
    // can be multithread
    fov_update();
}

void system_active_universe() {
    query_alive_entities_near_player();
    system_visibility();
}

void map_indexer() {}

void system_ai() {
    const auto& gstate = engine::reg.ctx<game_state_t>();
    if(gstate == game_state_t::ENEMY_TURN) {
        ai_enemy();
    }
    else if(gstate == game_state_t::PLAYER_TURN) {
    }
}

void system_melee_combat() {
    auto ents_melee = reg.view<combat_stats_t, being_t, wants_to_melee_t>(
        entt::exclude<dead_t>);

    for(auto [ent, atker_stats, being, want_to_melee] : ents_melee.each()) {
        auto [target_stats, target_being]
            = reg.get<combat_stats_t, being_t>(want_to_melee.target);
        auto final_damage = atker_stats.power - target_stats.defense;
        final_damage      = std::max(final_damage, 0);
        new_damage(want_to_melee.target, final_damage);

        std::string log_entry;
        if(final_damage > 0) {
            log_entry = fmt::format("{} hits {}, for {} hp", being.name,
                                    target_being.name, final_damage);
        }
        else {
            log_entry = fmt::format("{} is unable to hit {}", being.name,
                                    target_being.name);
        }
        engine::get_game_log().entries.push_back(log_entry);

        reg.remove<wants_to_melee_t>(ent);
    }
}

void system_destroy_deads() {
    const auto& gstate = reg.ctx<game_state_t>();
    if(gstate != game_state_t::ENEMY_TURN) {
        return;
    }
    auto deads = reg.view<renderable_t, dead_t, position_t>();
    deads.each([&](entity ent, renderable_t& rend, dead_t& dead,
                   position_t& dead_pos) {
        rend.vchar   = {'%', DARK_RED, DARKEST_RED};
        rend.z_level = z_level_t::DEAD;
        reg.remove_if_exists<blocks_t>(ent);
        --dead.decompose_turns;
        if(dead.decompose_turns <= 0) {
            // remove from map
            auto& map = engine::get_map();
            map[dead_pos].entities_here.remove(ent);
            query_alive_entities_near_player();
            reg.destroy(ent);
        }
    });

    // reg.destroy(deads.begin(), deads.end());
}

void system_walk() {
    auto wanting_to_walk
        = reg.view<want_to_walk_t, viewshed_t>(entt::exclude<dead_t>);
    auto& map = engine::get_map();

    auto fwalk = [&](auto ent) -> void {
        // want_to_walk_t& want_walk = wanting_to_walk.get<want_to_walk_t>(ent);
        auto [want_walk, vshed] = wanting_to_walk.get(ent);
        reg.remove<want_to_walk_t>(ent);

        auto& ents_tile_from = map[want_walk.from].entities_here;
        auto& ents_tile_to   = map[want_walk.to].entities_here;
        auto can_walk_to     = !is_occupied(want_walk.to)
                           && map[want_walk.to].props.walkable;
        if(!can_walk_to) {
            return;
        }
        // if can walk
        if(auto remove_pos = std::ranges::find(ents_tile_from, ent);
           remove_pos != ents_tile_from.end()) {
            ents_tile_from.erase(remove_pos);
        }
        ents_tile_to.push_back(ent);
        reg.replace<position_t>(ent, want_walk.to);
        vshed.dirty = true;
    };

    std::for_each(wanting_to_walk.begin(), wanting_to_walk.end(), fwalk);
}


void system_damage() {
    auto& map = engine::get_map();

    auto sufferers
        = reg.view<being_t, suffer_damage_t, combat_stats_t, position_t>(
            entt::exclude<dead_t>);

    for(auto [ent, being, damage, stats, e_pos] : sufferers.each()) {
        auto total_damage
            = std::accumulate(damage.amount.cbegin(), damage.amount.cend(), 0);
        stats.hp -= total_damage;
        damage.amount.clear();

        spdlog::debug("{} suffers {} damage", being.name, total_damage);
        if(total_damage > 0) {
            auto& tile  = map.at(e_pos);
            // tile.status = tile_status_t::BLOODIED;
        }

        if(stats.hp <= 0) {
            reg.emplace<dead_t>(ent);

            auto& e_inventory = reg.get<inventory_t>(ent);
            spdlog::debug("{} drops {} items.", being.name,
                          e_inventory.items.size());
            auto& tile = map.at(e_pos);

            for(auto& e_item : e_inventory.items) {
                reg.emplace<position_t>(e_item, e_pos);
            }

            std::copy(e_inventory.items.begin(), e_inventory.items.end(),
                      std::back_inserter(tile.entities_here));
            reg.remove<inventory_t>(ent);
        }
    }
}

// later we can group the components and run the groups in parallel
void systems_run() {
    // system_visibility();
    system_active_universe();

    system_ai();
    system_melee_combat();
    system_damage();
    system_destroy_deads();
    system_walk();
}


void player_system(const sf::Event& ev) {
    // using engine::event_dispatcher;
    auto dpos = get_delta_pos(ev);
    spdlog::debug("player dpos: ({}, {})", dpos.first, dpos.second);
}

void init_player() {
    engine::event_dispatcher.sink<sf::Event>().connect<&player_system>();
}

}  // namespace radl::system
