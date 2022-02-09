#ifndef PAINTER_H
#define PAINTER_H

#include <e172/math/vector.h>
#include <e172/time/elapsedtimer.h>

#include "surface.h"

class painter {
    e172::ElapsedTimer m_timer;
public:
    painter();

    void paint(pixel_primitives::bitmap& btmp);
};

#endif // PAINTER_H
