#ifndef PIXELPRIMITIVES_H
#define PIXELPRIMITIVES_H

#include <complex>
#include <cstdint>

namespace pixel_primitives {

struct bitmap {
    std::uint32_t *matrix = nullptr;
    std::size_t width = 0;
    std::size_t height = 0;
    operator std::uint32_t*() { return matrix; }
    operator const std::uint32_t*() const { return matrix; }
};

constexpr std::uint32_t argb(std::uint8_t a, std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    return a << 24 | r << 16 | g << 8 | b << 0;
}

constexpr std::uint32_t rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    return argb(0xff, r, g, b);
}

std::uint32_t blend_argb(std::uint32_t top, std::uint32_t bottom);

inline std::uint32_t garbage_pixel = 0;

inline std::uint32_t &pixel(bitmap &btmp, std::size_t x, std::size_t y) {
    if (btmp && x < btmp.width && y < btmp.height) return btmp[(y * btmp.width) + x];
    return garbage_pixel;
}

inline const std::uint32_t &pixel(const bitmap &btmp, std::size_t x, std::size_t y) {
    if (btmp && x < btmp.width && y < btmp.height) return btmp[(y * btmp.width) + x];
    return garbage_pixel;
}

void draw_line(
        bitmap &btmp,
        std::size_t point0_x,
        std::size_t point0_y,
        std::size_t point1_x,
        std::size_t point1_y,
        std::uint32_t argb
        );

inline void draw_vertical_line(
        bitmap &btmp,
        std::size_t point_x,
        std::size_t point_y,
        std::size_t len,
        std::uint32_t argb
        ) {
    for (int i = 0; i < len; i++) pixel(btmp, point_x , point_y + i) = argb;
}

inline void draw_horizontal_line(
        bitmap &btmp,
        std::size_t point_x,
        std::size_t point_y,
        std::size_t len,
        std::uint32_t argb
        ) {
    for (int i = 0; i < len; i++) pixel(btmp, point_x + i , point_y) = argb;
}

void draw_square(bitmap &btmp,
        std::size_t point_x,
        std::size_t point_y,
        std::size_t radius,
        std::uint32_t argb
        );

void fill_square(
        bitmap &btmp,
        std::size_t point_x,
        std::size_t point_y,
        std::size_t len,
        std::uint32_t argb
        );

void draw_rect(
        bitmap &btmp,
        std::size_t point0_x,
        std::size_t point0_y,
        std::size_t point1_x,
        std::size_t point1_y,
        std::uint32_t argb
        );

void fill_area(
        bitmap &btmp,
        std::size_t point0_x,
        std::size_t point0_y,
        std::size_t point1_x,
        std::size_t point1_y,
        std::uint32_t argb
        );

void draw_circle(
        bitmap &btmp,
        std::size_t center_x,
        std::size_t center_y,
        std::size_t radius,
        std::uint32_t argb
        );

void draw_grid(
        bitmap &btmp,
        std::size_t point0_x,
        std::size_t point0_y,
        std::size_t point1_x,
        std::size_t point1_y,
        std::size_t interval,
        std::uint32_t argb
        );

void copy_flipped(bitmap &dst_btmp, const bitmap &src_btmp, bool x_flip, bool y_flip);

inline void copy(bitmap &dst_btmp, const bitmap &src_btmp) {
    copy_flipped(dst_btmp, src_btmp, false, false);
}


bitmap cut_out(const bitmap &src_btmp, int x, int y, int w, int h);


void blit(
        bitmap &dst_btmp,
        const bitmap &src_btmp,
        std::size_t offset_x,
        std::size_t offset_y,
        std::size_t w,
        std::size_t h
        );

inline void blit(bitmap &dst_btmp, const bitmap &src_btmp, std::size_t offset_x, std::size_t offset_y) {
    blit(dst_btmp, src_btmp, offset_x, offset_y, src_btmp.width, src_btmp.height);
}

void rotate(bitmap &dst_btmp, const bitmap &src_btmp, const std::complex<double> &rotor);

};

#endif // PIXELPRIMITIVES_H
