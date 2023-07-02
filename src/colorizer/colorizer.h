#pragma once

#include <cstdint>
#include <string>

namespace e172::impl::console {

class Colorizer
{
public:
    virtual std::string beginSeq(std::uint32_t argb) const = 0;
    virtual std::string endSeq() const = 0;
    virtual ~Colorizer() {}
};

class AnsiColorizer : public Colorizer
{
public:

    struct color_mapping {
        const char* code;
        uint32_t rgb;
    };

    static inline const color_mapping mapping[] = {
        /* Black          */ { "\033[30m", 0x000000 },
        /* Red            */ { "\033[31m", 0x880000 },
        /* Green          */ { "\033[32m", 0x008800 },
        /* Yellow         */ { "\033[33m", 0x888800 },
        /* Blue           */ { "\033[34m", 0x000088 },
        /* Magenta        */ { "\033[35m", 0x880088 },
        /* Cyan           */ { "\033[36m", 0x008888 },
        /* White          */ { "\033[37m", 0xaaaaaa },
        /* Bright Black   */ { "\033[90m", 0x888888 },
        /* Bright Red     */ { "\033[91m", 0xff0000 },
        /* Bright Green   */ { "\033[92m", 0x00ff00 },
        /* Bright Yellow  */ { "\033[93m", 0xffff00 },
        /* Bright Blue    */ { "\033[94m", 0x0000ff },
        /* Bright Magenta */ { "\033[95m", 0xff00ff },
        /* Bright Cyan    */ { "\033[96m", 0x00ffff },
        /* Bright White   */ { "\033[97m", 0xffffff },
    };


    //static inline const color_mapping mapping[] = {
    //    /* Blue	      */ { "\033[34m", 0x0000ff },
    //    /* Green	  */ { "\033[32m", 0x00ff00 },
    //    /* Cyan	      */ { "\033[36m", 0x00ffff },
    //    /* Red	      */ { "\033[31m", 0xff0000 },
    //    /* Magenta    */ { "\033[35m", 0xff00ff },
    //    /* Orange	  */ { "\033[33m", 0xff8800 },
    //};


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

    // colorizer interface
public:
    virtual std::string beginSeq(std::uint32_t argb) const override;
    virtual std::string endSeq() const override;
};

class ansi_true_colorizer : public Colorizer
{
    std::uint8_t m_deterioration;
public:
    static inline const char* reset = "\033[0m";

    ansi_true_colorizer(std::uint8_t deterioration = 1);

    // colorizer interface
public:
    virtual std::string beginSeq(std::uint32_t argb) const override;
    virtual std::string endSeq() const override;
};

} // namespace e172::impl::console
