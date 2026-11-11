// Copyright 2023 Borys Boiko

#include "png_reader.h"

#include <cassert>
#include <cstdio>
#include <png.h>

namespace e172::impl::console::png {

pixel_primitives::bitmap read(std::istream& stream)
{
    if (!stream) {
        throw PNGDecodingException("stream could not be opened for reading");
    }

    std::uint8_t header[8]; // 8 is the maximum size that can be checked
    stream.read(reinterpret_cast<char*>(header), 8);
    const auto bytesRead = stream.gcount();
    assert(bytesRead == 8);
    if (bytesRead != 8) {
        throw PNGDecodingException("decoding png failed. Expected to read png header of 8 bytes.");
    }

    if (png_sig_cmp(header, 0, 8))
        throw PNGDecodingException("decoding png failed. stream data is not recognized as a PNG");

    /* initialize stuff */
    auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        throw PNGDecodingException("decoding png failed. png_create_read_struct failed");

    auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        throw PNGDecodingException("decoding png failed. png_create_info_struct failed");
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        throw PNGDecodingException("decoding png failed. error during init_io");
    }

    png_set_read_fn(png_ptr, &stream, [](png_structp png, png_bytep data, size_t length) {
        auto* is = static_cast<std::istream*>(png_get_io_ptr(png));
        is->read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(length));
        if (is->gcount() != static_cast<std::streamsize>(length)) {
            png_error(png, "Read error from std::istream");
        }
    });
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    const auto width = png_get_image_width(png_ptr, info_ptr);
    const auto height = png_get_image_height(png_ptr, info_ptr);
    const auto color_type = png_get_color_type(png_ptr, info_ptr);
    const auto bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    const auto number_of_passes = png_set_interlace_handling(png_ptr);

    (void)color_type;
    (void)bit_depth;
    (void)number_of_passes;

    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        throw PNGDecodingException("decoding png failed. error during read_image");
    }

    const auto row_pointers = reinterpret_cast<png_bytep*>(std::malloc(sizeof(png_bytep) * height));
    for (std::size_t y = 0; y < height; y++) {
        row_pointers[y] = reinterpret_cast<png_byte*>(std::malloc(png_get_rowbytes(png_ptr, info_ptr)));
    }

    png_read_image(png_ptr, row_pointers);

    pixel_primitives::bitmap result = { new uint32_t[width * height], width, height };

    for (std::size_t y = 0; y < height; y++) {
        png_byte* row = row_pointers[y];
        for (std::size_t x = 0; x < width; x++) {
            png_byte* ptr = &(row[x * 4]);
            std::uint32_t argb = (ptr[3] << 24) | (ptr[0] << 16) | (ptr[1] << 8) | (ptr[2] << 0);
            pixel_primitives::pixel(result, x, y) = argb;
        }
        std::free(row);
    }

    std::free(row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return result;
}

} // namespace e172::impl::console::png
