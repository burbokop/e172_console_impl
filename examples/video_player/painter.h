#pragma once

#include "../../src/surface.h"
#include <e172/math/vector.h>
#include <e172/time/elapsedtimer.h>

namespace e172::impl::console::video_player {

class painter {
    e172::ElapsedTimer m_timer;
public:
    painter();

    void paint(pixel_primitives::bitmap& btmp);
};

} // namespace e172::impl::console::video_player
