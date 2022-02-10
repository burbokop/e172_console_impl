#include "colorizer.h"

#include <e172/math/math.h>

// "\033[38;2;255;100;0mTRUECOLOR\033[0m\n"



std::string ansi_colorizer::beginSeq(uint32_t argb) const {
    const auto& r = std::uint8_t(argb >> 16);
    const auto& g = std::uint8_t(argb >> 8);
    const auto& b = std::uint8_t(argb);
    if(r == g && r == b) {
        return std::string();
    }
    if(g == 0 && b == 0) {
        return mapping[Red].code;
    }
    if(r == 0 && b == 0) {
        return mapping[Green].code;
    }
    if(r == 0 && g == 0) {
        return mapping[Blue].code;
    }

    std::int64_t minDelta = std::numeric_limits<std::int64_t>::max();
    std::string code;
    for(std::size_t i = 0; i < mappingSize; ++i) {
        const auto delta = std::abs(int64_t(mapping[i].rgb) - int64_t(argb & 0x00ffffff));
        if(delta < minDelta) {
            minDelta = delta;
            code = mapping[i].code;
        }
    }
    return code;
}

std::string ansi_colorizer::endSeq() const {
    return ansi_colorizer::reset;
}

ansi_true_colorizer::ansi_true_colorizer(uint8_t deterioration)
    : m_deterioration(deterioration) {}

std::string ansi_true_colorizer::beginSeq(uint32_t argb) const {
    const auto& r = std::to_string((std::uint8_t(argb >> 16) / m_deterioration) * m_deterioration);
    const auto& g = std::to_string((std::uint8_t(argb >> 8) / m_deterioration) * m_deterioration);
    const auto& b = std::to_string((std::uint8_t(argb >> 0) / m_deterioration) * m_deterioration);

    //const auto& r = std::to_string(std::uint8_t(argb >> 16));
    //const auto& g = std::to_string(std::uint8_t(argb >>  8));
    //const auto& b = std::to_string(std::uint8_t(argb >>  0));

    return "\033[38;2;" + r + ";" + g + ";" + b + "m";
}

std::string ansi_true_colorizer::endSeq() const {
    return ansi_true_colorizer::reset;
}

