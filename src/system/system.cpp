#include <ranges>
#include <algorithm>

#include "system/system.hpp"

#include "component/component.hpp"
#include "core/engine.hpp"

namespace radl::system {

namespace {

using engine::game_state_t;
using engine::reg;

}  // namespace

void system_visibility() {
    // can be multithread
    fov_update();
}

void system_active_universe() {
    query_alive_entities_near_player();
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
    auto ents_melee = reg.view<combat_stats_t, name_t, wants_to_melee_t>(
        entt::exclude<dead_t>);

    for(auto [ent, atker_stats, being, want_to_melee] : ents_melee.each()) {
        auto [target_stats, target_being]
            = reg.get<combat_stats_t, name_t>(want_to_melee.target);
        auto final_damage = atker_stats.power - target_stats.defense;
        final_damage      = std::max(final_damage, 0);
        new_damage(want_to_melee.target, final_damage);

        log_entry_t log_entry;
        if(final_damage > 0) {
            log_entry.log = fmt::format("{} hits {}, for {} hp", being.name,
                                        target_being.name, final_damage);
        }
        else {
            log_entry.log = fmt::format("{} is unable to hit {}", being.name,
                                        target_being.name);
        }
        engine::get_game_log().entries.push_back(log_entry);

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
            auto& map = engine::get_map();
            map[dead_pos].entities_here.remove(ent);
            query_alive_entities_near_player();
            reg.destroy(ent);
        }
    });
}

void system_walk() {
    auto wanting_to_walk
        = reg.view<want_to_walk_t, viewshed_t>(entt::exclude<dead_t>);
    auto& map = engine::get_map();

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
        auto& map = engine::get_map();
        map.at(want_walk.from).remove_entity(ent);
        map.at(want_walk.to).insert_entity(ent);

        reg.replace<position_t>(ent, want_walk.to);
        vshed.dirty = true;
    };

    std::ranges::for_each(wanting_to_walk, fwalk);
}

void system_item_collection() {
    auto v_pick_item = reg.view<wants_to_pickup_item_t, position_t>();
    v_pick_item.each(
        [](auto ent, wants_to_pickup_item_t& want_pick, position_t& pos_item) {
            const auto& ent_item = want_pick.item;
            // const auto& ent      = want_pick.picked_by;
            add_to_inventory(ent, ent_item);
            engine::get_map().at(pos_item).remove_entity(ent_item);
#ifdef DEBUG
            const auto& being = reg.get<name_t>(ent);
            spdlog::debug("item picked up by {}", being.name);
#endif
            reg.remove<position_t>(ent_item);
            reg.remove<wants_to_pickup_item_t>(ent);
        });
}

void system_damage() {
    auto& map = engine::get_map();

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
            reg.remove<suffer_damage_t>(ent);
            reg.remove<name_t>(ent);
            reg.remove<combat_stats_t>(ent);
        }
    }
}

void system_item_use() {
    auto v_use = reg.view<wants_to_use_t>();

    for(auto [ent, use] : v_use.each()) {
        auto [e_what]   = use;
        auto* inventory = reg.try_get<inventory_t>(ent);
        if(!inventory) {
            continue;
        }
        if(!inventory->contains(e_what)) {
            continue;
        }

        auto item = inventory->get_item(e_what);
        if(item.in_pack) {
            // use item
            switch(item.type) {
            case item_type_t::POTION: {
                auto& e_stats  = reg.get<combat_stats_t>(ent);
                auto& potion   = reg.get<item_potion_t>(e_what);
                auto prev_heal = e_stats.hp;
                e_stats.hp += potion.healing;
                e_stats.hp = std::min(e_stats.max_hp, e_stats.hp);
                // log entry
                auto final_healing = e_stats.hp - prev_heal;
                log_entry_t log_entry;
                log_entry.log = fmt::format("healed for {} hp.", final_healing);
                log_entry.fg  = LIGHT_GREEN;
                engine::get_game_log().entries.push_back(log_entry);
            } break;
            default:
                break;
            }
            // remove item
            inventory->remove_first(e_what);
            reg.remove<wants_to_use_t>(ent);
            // destroy entity
            reg.destroy(e_what);
        }
    }
}

// later we can group the components and run the groups in parallel
void systems_run() {
    system_active_universe();
    system_visibility();
    system_ai();
    system_melee_combat();
    system_damage();
    system_walk();
    system_item_collection();
    system_item_use();
    system_destroy_deads();
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
