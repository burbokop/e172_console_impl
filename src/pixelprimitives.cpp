#include "pixelprimitives.h"

#include <algorithm>
#include <math.h>

namespace e172::impl::console::pixel_primitives {

void draw_line(bitmap &btmp,
               std::size_t point0_x,
               std::size_t point0_y,
               std::size_t point1_x,
               std::size_t point1_y,
               uint32_t argb)
{
    std::int64_t d, dL, dU, dx, dy, temp;
    dy = point1_y - point0_y;
    if (dy < 0) {
        temp = point1_x;
        point1_x = point0_x;
        point0_x = temp;
        temp = point1_y;
        point1_y = point0_y;
        point0_y = temp;
    }
    dx = point1_x - point0_x;
    dy = point1_y - point0_y;
    if (dx >= 0) {
        if (dy > dx) {
            d = dy - 2 * dx;
            dL = -2 * dx;
            dU = 2 * dy - 2 * dx;
            for(std::int64_t x = point0_x, y = point0_y; y <= point1_y; y++) {
                pixel(btmp, x, y) = argb;
                if (d >= 1) {
                    d += dL;
                } else {
                    x++;
                    d += dU;
                }
            }
        } else {
            d = 2 * dy - dx;
            dL = 2 * dy;
            dU = 2 * dy - 2 * dx;
            for(std::int64_t x = point0_x, y = point0_y; x <= point1_x; x++) {
                pixel(btmp, x, y) = argb;
                if (d <= 0) {
                    d += dL;
                } else {
                    y++;
                    d += dU;
                }
            }
        }
    } else if (dx < 0) {
        if (dy > -dx) {
            d = dy - 2 * -dx;
            dL = -2 * -dx;
            dU = 2 * dy - 2 * -dx;
            for (std::int64_t x = point0_x, y = point0_y; y <= point1_y; y++) {
                pixel(btmp, x, y) = argb;
                if (d >= 1) {
                    d += dL;
                } else {
                    --x;
                    d += dU;
                }
            }
        } else {
            d = 2 * dy + dx;
            dL = 2 * dy;
            dU = 2 * dy - 2 * -dx;
            for (std::int64_t x = point0_x, y = point0_y; x >= point1_x; --x) {
                pixel(btmp, x, y) = argb;
                if (d <= 0) {
                    d += dL;
                } else {
                    y++;
                    d += dU;
                }
            }
        }
    }
}

void draw_square(
    bitmap &btmp, std::size_t point_x, std::size_t point_y, std::size_t radius, uint32_t argb)
{
    const std::size_t len = radius * 2;
    for(std::size_t i = 0; i < len; i++) {
        pixel(btmp, point_x + i, point_y) = argb;
        pixel(btmp, point_x + i, point_y + len) = argb;
        pixel(btmp, point_x, point_y + i) = argb;
        pixel(btmp, point_x + len, point_y + i) = argb;
    }
    pixel(btmp, point_x + len, point_y + len) = argb;
}

void fill_square(
    bitmap &btmp, std::size_t point_x, std::size_t point_y, std::size_t len, uint32_t argb)
{
    for (std::size_t i = 0; i < len; i++) {
        for (std::size_t j = 0; j < len; j++) {
            pixel(btmp, point_x + j, point_y + i) = argb;
        }
    }
}

void draw_rect(bitmap &btmp,
               std::size_t point0_x,
               std::size_t point0_y,
               std::size_t point1_x,
               std::size_t point1_y,
               uint32_t argb)
{
    const auto& min_x = std::min(point1_x, point0_x);
    const auto& max_x = std::max(point1_x, point0_x);
    const auto& min_y = std::min(point1_y, point0_y);
    const auto& max_y = std::max(point1_y, point0_y);

    std::int64_t dx = max_x - min_x, dy = max_y - min_y;
    if (dx >= 0) {
        for (std::int64_t i = 0; i <= dx; i++) {
            pixel(btmp, min_x + i, min_y) = argb;
            pixel(btmp, min_x + i, min_y + dy) = argb;
        }
    } else if (dx < 0) {
        for (std::int64_t i = 0; i >= dx; i--) {
            pixel(btmp, min_x + i, min_y) = argb;
            pixel(btmp, min_x + i, min_y + dy) = argb;
        }
    }

    if (dy >= 0) {
        for (std::int64_t i = 0; i <= dy; i++) {
            pixel(btmp, min_x, min_y + i) = argb;
            pixel(btmp, min_x + dx, min_y + i) = argb;
        }
    } else if (dy < 0) {
        for (std::int64_t i = 0; i >= dy; i--) {
            pixel(btmp, min_x, min_y + i) = argb;
            pixel(btmp, min_x + dx, min_y + i) = argb;
        }
    }
}

void fill_area(bitmap &btmp,
               std::size_t point0_x,
               std::size_t point0_y,
               std::size_t point1_x,
               std::size_t point1_y,
               uint32_t argb)
{
    std::int64_t dx = point1_x - point0_x, dy = point1_y - point0_y;
    if (dx >= 0) {
        if (dy >= 0) {
            for (std::int64_t  i = 0; i < dy; i++) {
                for (std::int64_t  j = 0; j < dx; j++) {
                    pixel(btmp, point0_x + j, point0_y + i) = argb;
                }
            }
        } else {
            for (std::int64_t  i = 0; i >= dy; i--) {
                for (std::int64_t  j = 0; j < dx; j++) {
                    pixel(btmp, point0_x + j, point0_y + i) = argb;
                }
            }
        }
    } else {
        if (dy >= 0) {
            for (std::int64_t  i = 0; i < dy; i++) {
                for (std::int64_t  j = 0; j >= dx; j--) {
                    pixel(btmp, point0_x + j, point0_y + i) = argb;
                }
            }
        } else {
            for (std::int64_t  i = 0; i >= dy; i--) {
                for (std::int64_t  j = 0; j >= dx; j--) {
                    pixel(btmp, point0_x + j, point0_y + i) = argb;
                }
            }
        }
    }
}

void draw_circle(
    bitmap &btmp, std::size_t center_x, std::size_t center_y, std::size_t radius, uint32_t argb)
{
    std::int64_t i2 = std::numeric_limits<std::int64_t>::max();
    for(std::int64_t i = 0; i + 1 < i2; i++) {
        i2 = static_cast<std::int64_t>(std::sqrt(radius * radius - i * i));
        pixel(btmp, center_x + i, center_y + i2) = argb;
        pixel(btmp, center_x - i, center_y - i2) = argb;
        pixel(btmp, center_x + i, center_y - i2) = argb;
        pixel(btmp, center_x - i, center_y + i2) = argb;

        pixel(btmp, center_x + i2, center_y + i) = argb;
        pixel(btmp, center_x - i2, center_y - i) = argb;
        pixel(btmp, center_x - i2, center_y + i) = argb;
        pixel(btmp, center_x + i2, center_y - i) = argb;
    }
}

void draw_grid(bitmap &btmp,
               std::size_t point0_x,
               std::size_t point0_y,
               std::size_t point1_x,
               std::size_t point1_y,
               std::size_t interval,
               uint32_t argb)
{
    for (std::int64_t i = 0; i < (point1_x - point0_x) / interval; i++) {
        for (std::int64_t j = 0; j < point1_y - point0_y; j++) {
            pixel(btmp, point0_x + (i * interval), point0_y + j) = argb;
        }
    }
    for (std::int64_t i = 0; i < (point1_y - point0_y) / interval; i++) {
        for (std::int64_t j = 0; j < point1_x - point0_x; j++) {
            pixel(btmp, point0_x + j, point0_y + (i * interval)) = argb;
        }
    }
}

void copy_flipped(bitmap &dst_btmp, const bitmap &src_btmp, bool x_flip, bool y_flip)
{
    if(x_flip && y_flip) {
        for (std::size_t y = 0; y < src_btmp.height; ++y) {
            for (std::size_t x = 0; x < src_btmp.width; ++x) {
                pixel(dst_btmp, src_btmp.width - x, src_btmp.height - y) = pixel(src_btmp, x, y);
            }
        }
    } else if(x_flip) {
        for (std::size_t y = 0; y < src_btmp.height; ++y) {
            for (std::size_t x = 0; x < src_btmp.width; ++x) {
                pixel(dst_btmp, src_btmp.width - x, y) = pixel(src_btmp, x, y);
            }
        }
    } else if(y_flip) {
        for (std::size_t y = 0; y < src_btmp.height; ++y) {
            for (std::size_t x = 0; x < src_btmp.width; ++x) {
                pixel(dst_btmp, x, src_btmp.height - y) = pixel(src_btmp, x, y);
            }
        }
    } else {
        for (std::size_t y = 0; y < src_btmp.height; ++y) {
            for (std::size_t x = 0; x < src_btmp.width; ++x) {
                pixel(dst_btmp, x, y) = pixel(src_btmp, x, y);
            }
        }
    }
}

std::uint32_t blend_argb(std::uint32_t top, std::uint32_t bottom)
{
    const auto ba = uint8_t((top >>  0) & 0x000000ff);
    const auto ga = uint8_t((top >>  8) & 0x000000ff);
    const auto ra = uint8_t((top >> 16) & 0x000000ff);
    const auto aa = uint8_t((top >> 24) & 0x000000ff);

    const auto bb = uint8_t((bottom >>  0) & 0x000000ff);
    const auto gb = uint8_t((bottom >>  8) & 0x000000ff);
    const auto rb = uint8_t((bottom >> 16) & 0x000000ff);
    const auto ab = uint8_t((bottom >> 24) & 0x000000ff);

    const uint32_t b_out = uint32_t((ba * aa / 255) + (bb * ab * (255 - aa) / (255 * 255))) << 0;
    const uint32_t g_out = uint32_t((ga * aa / 255) + (gb * ab * (255 - aa) / (255 * 255))) << 8;
    const uint32_t r_out = uint32_t((ra * aa / 255) + (rb * ab * (255 - aa) / (255 * 255))) << 16;
    const uint32_t a_out = uint32_t(aa + (ab * (255 - aa) / 255)) << 24;

    return a_out | r_out | g_out | b_out;
}

void blit(bitmap &dst_btmp,
          const bitmap &src_btmp,
          std::size_t offset_x,
          std::size_t offset_y,
          std::size_t w,
          std::size_t h)
{
    //w = std::min(w, src_btmp.width);
    //h = std::min(h, src_btmp.height);
    //w = std::min(w, dst_btmp.width);
    //h = std::min(h, dst_btmp.height);

    for (std::size_t y = 0; y < h; ++y) {
        for (std::size_t x = 0; x < w; ++x) {
            auto& bottom = pixel(dst_btmp, x + offset_x, y + offset_y);
            bottom = blend_argb(pixel(src_btmp, x, y), bottom);
        }
    }
}

void blit_transformed(bitmap &dst_btmp,
                      const bitmap &src_btmp,
                      const std::complex<double> &rotor,
                      const double scaler,
                      std::size_t center_x,
                      std::size_t center_y)
{
    const auto& lt = rotor * std::complex<double>(-double(src_btmp.width) / 2 * scaler, -double(src_btmp.height) / 2 * scaler);
    const auto& rt = rotor * std::complex<double>( double(src_btmp.width) / 2 * scaler, -double(src_btmp.height) / 2 * scaler);
    const auto& lb = rotor * std::complex<double>(-double(src_btmp.width) / 2 * scaler,  double(src_btmp.height) / 2 * scaler);
    const auto& rb = rotor * std::complex<double>( double(src_btmp.width) / 2 * scaler,  double(src_btmp.height) / 2 * scaler);
    const auto real_arr = { lt.real(), rt.real(), lb.real(), rb.real() };
    const auto imgn_arr = { lt.imag(), rt.imag(), lb.imag(), rb.imag() };
    const auto min_x = std::min_element(real_arr.begin(), real_arr.end());
    const auto max_x = std::max_element(real_arr.begin(), real_arr.end());
    const auto min_y = std::min_element(imgn_arr.begin(), imgn_arr.end());
    const auto max_y = std::max_element(imgn_arr.begin(), imgn_arr.end());
    if(!min_x || !min_y || !max_x || !max_y) {
        return;
    }

    const std::uint32_t gp = 0x0;
    const auto inv_rotor = (std::complex<double>(1, 0) / rotor);
    for (std::int32_t y = *min_y; y < *max_y; ++y) {
        for (std::int32_t x = *min_x; x < *max_x; ++x) {
            const auto& rotated_position = inv_rotor * std::complex<double>(x, y);
            const auto sx = rotated_position.real() / scaler + src_btmp.width / 2;
            const auto sy = rotated_position.imag() / scaler + src_btmp.height / 2;

            std::uint32_t src0 = pixel(src_btmp, sx,     sy,     gp);
            std::uint32_t src1 = pixel(src_btmp, sx + 1, sy,     gp);
            std::uint32_t src2 = pixel(src_btmp, sx    , sy + 1, gp);
            std::uint32_t src3 = pixel(src_btmp, sx + 1, sy + 1, gp);

            std::uint32_t result_src = aver_argb<4>({ src0, src1, src2, src3 });

            auto& bottom = pixel(dst_btmp, x + center_x, y + center_x);

            bottom = blend_argb(result_src, bottom) ;
            //pixel(dst_btmp, x + center_x, y + center_x) = src4;
        }
    }

    draw_circle(dst_btmp, center_x, center_x, 8, 0xff00ff00);

    draw_circle(dst_btmp, lt.real() + center_x, lt.imag() + center_y, 4, 0xff00ff00);
    draw_circle(dst_btmp, rt.real() + center_x, rt.imag() + center_y, 4, 0xff00ff00);
    draw_circle(dst_btmp, lb.real() + center_x, lb.imag() + center_y, 4, 0xff00ff00);
    draw_circle(dst_btmp, rb.real() + center_x, rb.imag() + center_y, 4, 0xff00ff00);


    draw_rect(
                dst_btmp,
                *min_x + center_x,
                *min_y + center_y,
                *max_x + center_x,
                *max_y + center_y,
                0xff0000ff
                );
}

} // namespace e172::impl::console::pixel_primitives
