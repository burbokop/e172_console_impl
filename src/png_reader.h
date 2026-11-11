// Copyright 2023 Borys Boiko

#pragma once

#include "pixelprimitives.h"
#include <istream>

namespace e172::impl::console::png {

class PNGDecodingException : public std::exception {
public:
    PNGDecodingException(const char* what)
        : m_what(what)
    {
    }
    const char* what() const noexcept { return m_what; }

private:
    const char* m_what;
};

pixel_primitives::bitmap read(std::istream& stream);

} // namespace e172::impl::console::png
