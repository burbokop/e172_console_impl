#ifndef PNG_READER_H
#define PNG_READER_H

#include <istream>

#include "pixelprimitives.h"

struct png_reader {
    class png_decoding_exception : public std::exception {
        const char* m_what;
    public:
        png_decoding_exception(const char* what) : m_what(what) {}
        const char *what() const noexcept { return m_what; }
    };

    static pixel_primitives::bitmap read(std::istream &stream);
};

#endif // PNG_READER_H
