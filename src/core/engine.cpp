#include <memory>

#include "spdlog/spdlog.h"


#include "core/engine.hpp"
#include "system/spawner.hpp"

#include "system/game_state.hpp"

#include "utils/rng.hpp"

#include "gsl/gsl_util"


// TODO remove this
#include "core/gui/gui.hpp"

namespace radl::engine {

Engine engine;

using namespace rltk::colors;

rltk::virtual_terminal* console;

namespace {

using rltk::gui;
using rltk::term;

}  // namespace

}
