#pragma once
#include <cinttypes>

#include "rltk/rltk.hpp"
#include "component/color_t.hpp"


namespace radl {

using vchar_t = rltk::vchar;

/*
 * Represents a character on a virtual terminal.
 */
// struct vchar_t {
//     int glyph;
//     TCODColor foreground;
//     TCODColor background;

//     vchar_t() {}

//     vchar_t(const uint32_t& g, const TCODColor& f, const TCODColor& b)
//         : glyph(g)
//         , foreground(f)
//         , background(b) {}

//     vchar_t(const int& g, const TCODColor& f, const TCODColor& b)
//         : glyph(static_cast<uint32_t>(g))
//         , foreground(f)
//         , background(b) {}

//     vchar_t(const uint32_t& gly, const uint8_t& fr, const uint8_t& fg,
//             const uint8_t& fb, const uint8_t& br, const uint8_t& bg,
//             const uint8_t& bb)
//         : glyph(gly)
//         , foreground(TCODColor{fr, fg, fb})
//         , background(TCODColor{br, bg, bb}) {}


//     template <class Archive>
//     void serialize(Archive& archive) {
//         archive(glyph, foreground,
//                 background);  // serialize things by passing them to the
//                 archive
//     }
// };

}  // namespace radl
