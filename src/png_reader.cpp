#include "png_reader.h"

#include <png.h>
#include <bsd/stdio.h>

namespace e172::impl::console::png {

pixel_primitives::bitmap read(std::istream &stream)
{
    auto fp = fropen(
                &stream,
                []/* readfn  */(void *c, char * buf, int siz) -> int {
                    return reinterpret_cast<std::istream*>(c)
                    ->readsome(buf, siz);
                }
                );

    std::uint8_t header[8]; // 8 is the maximum size that can be checked

    if (!fp)
        throw PNGDecodingException("stream could not be opened for reading");

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        throw PNGDecodingException("decoding png failed. stream data is not recognized as a PNG");

    /* initialize stuff */
    const auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        throw PNGDecodingException("decoding png failed. png_create_read_struct failed");

    const auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        throw PNGDecodingException("decoding png failed. png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        throw PNGDecodingException("decoding png failed. error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    const auto width = png_get_image_width(png_ptr, info_ptr);
    const auto height = png_get_image_height(png_ptr, info_ptr);
    const auto color_type = png_get_color_type(png_ptr, info_ptr);
    const auto bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    const auto number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
        throw PNGDecodingException("decoding png failed. error during read_image");

    const auto row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (std::size_t y = 0; y < height; y++)
        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

    png_read_image(png_ptr, row_pointers);

    pixel_primitives::bitmap result = { new uint32_t[width * height], width, height };

    for (std::size_t y = 0; y < height; y++) {
        png_byte* row = row_pointers[y];
        for (std::size_t x = 0; x < width; x++) {
            png_byte* ptr = &(row[x*4]);
            std::uint32_t argb = (ptr[3] << 24) | (ptr[0] << 16) | (ptr[1] << 8) | (ptr[2] << 0);
            pixel_primitives::pixel(result, x, y) = argb;
        }
        free(row);
    }
    free(row_pointers);
    return result;
}

} // namespace e172::impl::console::png
