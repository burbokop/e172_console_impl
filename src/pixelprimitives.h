#pragma once

#include <array>
#include <complex>
#include <cstdint>
#include <e172/graphics/color.h>
#include <type_traits>

namespace e172::impl::console::pixel_primitives {

struct bitmap {
    std::uint32_t *matrix = nullptr;
    std::size_t width = 0;
    std::size_t height = 0;
    std::uint32_t garbage_pixel = 0;
    operator std::uint32_t*() { return matrix; }
    operator const std::uint32_t*() const { return matrix; }
};

template<std::size_t s>
typename std::enable_if<s != 0, e172::Color>::type aver_argb(const std::array<e172::Color, s> &arr)
{
    std::int64_t sum_a = 0;
    std::int64_t sum_r = 0;
    std::int64_t sum_g = 0;
    std::int64_t sum_b = 0;
    std::size_t i = 0;
    for(; i < s; ++i) {
        sum_a += (arr[i] >> 24) & 0xff;
        sum_r += (arr[i] >> 16) & 0xff;
        sum_g += (arr[i] >>  8) & 0xff;
        sum_b += (arr[i] >>  0) & 0xff;
    }
    return (sum_a / i) << 24 | (sum_r / i) << 16 | (sum_g / i) << 8 | (sum_b / i) << 0;
}

inline e172::Color &pixel(bitmap &btmp, std::size_t x, std::size_t y)
{
    if (btmp && x < btmp.width && y < btmp.height) return btmp[(y * btmp.width) + x];
    return btmp.garbage_pixel;
}

inline const e172::Color &pixel(const bitmap &btmp, std::size_t x, std::size_t y)
{
    if (btmp && x < btmp.width && y < btmp.height) return btmp[(y * btmp.width) + x];
    return btmp.garbage_pixel;
}

inline const e172::Color &pixel(const bitmap &btmp,
                                std::size_t x,
                                std::size_t y,
                                const e172::Color &garbage_pixel)
{
    if (btmp && x < btmp.width && y < btmp.height) return btmp[(y * btmp.width) + x];
    return garbage_pixel;
}

void draw_line(bitmap &btmp,
               std::int64_t point0_x,
               std::int64_t point0_y,
               std::int64_t point1_x,
               std::int64_t point1_y,
               e172::Color argb);

inline void draw_vertical_line(
    bitmap &btmp, std::size_t point_x, std::size_t point_y, std::size_t len, e172::Color argb)
{
    for (std::size_t i = 0; i < len; i++)
        pixel(btmp, point_x, point_y + i) = argb;
}

inline void draw_horizontal_line(
    bitmap &btmp, std::size_t point_x, std::size_t point_y, std::size_t len, e172::Color argb)
{
    for (std::size_t i = 0; i < len; i++)
        pixel(btmp, point_x + i, point_y) = argb;
}

void draw_square(
    bitmap &btmp, std::size_t center_x, std::size_t center_y, std::size_t radius, e172::Color argb);

void fill_square(
    bitmap &btmp, std::size_t center_x, std::size_t center_y, std::size_t radius, e172::Color argb);

void draw_rect(bitmap &btmp,
               std::size_t point0_x,
               std::size_t point0_y,
               std::size_t point1_x,
               std::size_t point1_y,
               e172::Color argb);

void fill_area(bitmap &btmp,
               std::size_t point0_x,
               std::size_t point0_y,
               std::size_t point1_x,
               std::size_t point1_y,
               e172::Color argb);

void draw_circle(
    bitmap &btmp, std::size_t center_x, std::size_t center_y, std::size_t radius, e172::Color argb);

void draw_grid(bitmap &btmp,
               std::int64_t point0_x,
               std::int64_t point0_y,
               std::int64_t point1_x,
               std::int64_t point1_y,
               std::int64_t interval,
               e172::Color argb);

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

void blit_transformed(bitmap &dst_btmp,
                      const bitmap &src_btmp,
                      const std::complex<double> &rotor,
                      const double scaler,
                      const std::size_t center_x,
                      const std::size_t center_y);

inline void blit_transformed(
        bitmap &dst_btmp,
        const bitmap &src_btmp,
        const std::complex<double> &rotor,
        const double scaler
        ) { blit_transformed(dst_btmp, src_btmp, rotor, scaler, src_btmp.width / 2, src_btmp.height / 2); }

inline void blit_rotated(
        bitmap &dst_btmp,
        const bitmap &src_btmp,
        const std::complex<double> &rotor,
        std::size_t center_x,
        std::size_t center_y
        ) { blit_transformed(dst_btmp, src_btmp, rotor, 1, center_x, center_y); }

inline void blit_rotated(
        bitmap &dst_btmp,
        const bitmap &src_btmp,
        const std::complex<double> &rotor
        ) { blit_rotated(dst_btmp, src_btmp, rotor, src_btmp.width / 2, src_btmp.height / 2); }

}; // namespace e172::impl::console::pixel_primitives
