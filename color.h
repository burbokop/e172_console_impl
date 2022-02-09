#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

struct color {
    static constexpr const char gradient[] = " .:!/r(l1Z4H9W8$@";

    struct color_mapping {
        const char* code;
        uint32_t rgb;
    };

    static inline const color_mapping mapping[] = {
        /* Blue	      */ { "\033[34m", 0x0000ff },
        /* Green	  */ { "\033[32m", 0x00ff00 },
        /* Cyan	      */ { "\033[36m", 0x00ffff },
        /* Red	      */ { "\033[31m", 0xff0000 },
        /* Magenta    */ { "\033[35m", 0xff00ff },
        /* Orange	  */ { "\033[33m", 0xff8800 },
    };

    enum Mapping {
        Blue = 0,
        Green,
        Cyan,
        Red,
        Magenta,
        Orange
    };

    static constexpr std::size_t mappingSize = sizeof (mapping) / sizeof (mapping[0]);
    static inline const char* reset = "\033[0m";

    static const char* argbToColorCode(std::uint32_t argb);

    static char argbToChar(std::uint32_t argb);
    static char brightnessToChar(std::uint8_t brightness);
};

#endif // COLOR_H
