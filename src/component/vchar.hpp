#pragma once
#include <cinttypes>

#include "rltk/rltk.hpp"
#include "component/color_t.hpp"


namespace radl::component {

namespace glyph {
// items
constexpr uint32_t POTION = 173;

constexpr uint32_t BLOCK1 = 176;
constexpr uint32_t BLOCK2 = 177;
constexpr uint32_t BLOCK3 = 178;

constexpr uint32_t LIST_MID        = 195;
constexpr uint32_t HORIZONTAL_LINE = 196;

constexpr uint32_t SOLID1 = 219;

constexpr uint32_t CDOT = 250;

}  // namespace glyph

using vchar_t = rltk::vchar;

}  // namespace radl::component
