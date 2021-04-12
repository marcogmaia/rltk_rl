#include <memory>

#include "spdlog/spdlog.h"

#include "component/component.hpp"

#include "core/engine.hpp"
#include "core/factories.hpp"
#include "core/gui.hpp"

#include "system/camera.hpp"
#include "system/player_action.hpp"
#include "system/system.hpp"

#include "utils/rng.hpp"


namespace radl::engine {

using namespace world;
using namespace rltk::colors;
std::deque<sf::Event> event_queue;

entt::registry reg;
entt::entity player;

rltk::virtual_terminal* console;

entt::dispatcher event_dispatcher{};
entt::sigh<sf::Event()> ev_signal{};
entt::sink ev_sink{ev_signal};

constexpr int width  = 1024 / 16;
constexpr int height = 768 / 16;

namespace {

using rltk::gui;
using rltk::term;

}  // namespace

static void rltk_init() {
    constexpr auto font_file = "../assets";
    rltk::init(rltk::config_advanced(font_file, width * 16, height * 16,
                                     "Maia Roguelike learning"));

    // XXX fix this to sync with one variable
}


namespace {


void add_enemy(const position_t& pos) {
    if(!is_occupied(pos)) {
        entity e_ent;
        auto chance = rng::rng.range(1, 3);
        if(chance == 1) {
            e_ent = factory::enemy_factory(pos, vchar_t{'g', DARK_GREEN, BLACK},
                                           "Goblin");
        }
        else {
            e_ent = factory::enemy_factory(pos, vchar_t{'O', GREEN, BLACK},
                                           "Orc");
        }
        auto chance_to_have_item
            = rng::rng.range(1, 10) == 1;  // 1 chance in 10
        if(chance_to_have_item) {
            auto& e_inventory = reg.get<inventory_t>(e_ent);
            e_inventory.items.push_back(healing_potion(true));
        }
    }
}

void add_enemies() {
    constexpr uint32_t max_enemies_pex_room = 5;
    using rng::rng;

    const auto& w_map = engine::get_map();
    for(const auto& room : w_map.rooms) {
        auto num_enemies = rng.range(0, max_enemies_pex_room);
        for(int i = 0; i < num_enemies; ++i) {
            int rand_x          = rng.range(room.x1, room.x2 - 1);
            int rand_y          = rng.range(room.y1, room.y2 - 1);
            position_t rand_pos = {rand_x, rand_y};
            if(!is_occupied(rand_pos)) {
                add_enemy(rand_pos);
            }
        }
    }
}


}  // namespace

void init() {
#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    event_dispatcher.sink<sf::Event>().connect<&system::player_system>();
#endif
    spdlog::info("Initializing engine.");
    rltk_init();
    gui::init();
    reg.set<game_state_t>(game_state_t::PRE_RUN);
    // event_sink.connect<&process_input>();
}

/**
 * @brief Get the map from the register context
 *
 * @warning call this function only after setting the map on the register
 *
 * @return the map from the register context
 */
world::Map& get_map() {
    return reg.ctx<world::Map>();
}

namespace {

[[maybe_unused]] void enemy_spawner() {
    auto& map        = engine::get_map();
    int random_index = rng::rng.range(0, map.rooms.size() - 1);
    auto random_pos  = map.rooms.at(random_index).random_pos();
    add_enemy(random_pos);
}

}  // namespace


component::game_log_t& get_game_log() {
    return reg.ctx<component::game_log_t>();
}

void game_state_system([[maybe_unused]] double elapsed_time) {
    auto& game_state = reg.ctx<game_state_t>();


    switch(game_state) {
    case game_state_t::PRE_RUN: {
        // initialize everything
        reg.set<Map>();
        reg.set<component::game_log_t>();
        auto& map = get_map();
        map.init(rect_t{0, 0, width * 4, height * 4});
        create_random_rooms(map);
        make_corridors_between_rooms(map);
        auto player_start_pos = map.rooms[0].center();
        player                = reg.create();
        factory::player_factory(player, player_start_pos,
                                vchar_t{'@', YELLOW, BLACK});
        add_enemies();
        spdlog::info("entities created: {}", reg.alive());
        system::systems_run();
        game_state = game_state_t::AWAITING_INPUT;
    } break;

    case game_state_t::AWAITING_INPUT: {
        render();
        game_state = player_input();
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
        }
        else {
            game_state = game_state_t::AWAITING_INPUT;
        }
    } break;

    case game_state_t::DEFEAT: {
        terminate();
        game_state = game_state_t::PRE_RUN;
    } break;

    default:
        break;
    }
}

void update(double elapsed_time) {
    engine::event_dispatcher.update();
    game_state_system(elapsed_time);
}

void render() {
    if(reg.valid(player)) {
        camera_update(player);
        gui::render_gui();
    }
}

void terminate() {
    reg.unset<world::Map>();
    reg.unset<component::game_log_t>();
    reg.clear();
}

position_t get_position_from_entity(entity ent) {
    auto pos = reg.get<position_t>(ent);
    return pos;
}

}  // namespace radl::engine
