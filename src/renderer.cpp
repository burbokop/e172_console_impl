#include "renderer.h"

namespace e172::impl::console {

Renderer::Renderer(std::ostream &output)
    : m_writer(Writer(output))
{}

bool Renderer::update()
{
    m_writer.write_frame();
    return true;
}

void Renderer::fill(uint32_t color)
{
    pixel_primitives::fill_area(m_writer.bitmap(), 0, 0, m_writer.bitmap().width, m_writer.bitmap().height, color);
}

void Renderer::drawPixel(const e172::Vector<double> &point, e172::Color color)
{
    pixel_primitives::pixel(m_writer.bitmap(), point.x(), point.y()) = color;
}

void Renderer::drawLine(const e172::Vector<double> &point0,
                        const e172::Vector<double> &point1,
                        uint32_t color)
{
    pixel_primitives::draw_line(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
}

void Renderer::drawRect(const e172::Vector<double> &point0,
                        const e172::Vector<double> &point1,
                        uint32_t color,
                        const e172::ShapeFormat &format)
{
    if(format.fill()) {
        pixel_primitives::fill_area(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
    } else {
        pixel_primitives::draw_rect(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
    }
}

void Renderer::drawSquare(const e172::Vector<double> &point, int radius, uint32_t color)
{
    pixel_primitives::draw_square(m_writer.bitmap(), point.x(), point.y(), radius, color);
}

void Renderer::drawCircle(const e172::Vector<double> &point, int radius, uint32_t color)
{
    pixel_primitives::draw_circle(m_writer.bitmap(), point.x(), point.y(), radius, color);
}

void Renderer::drawImage(const e172::Image &image,
                         const e172::Vector<double> &position,
                         double angle,
                         double zoom)
{
    if(imageProvider(image) == provider()) {
        pixel_primitives::blit_transformed(
                    m_writer.bitmap(),
                    imageData<pixel_primitives::bitmap>(image),
                    std::complex<double>(std::cos(angle), std::sin(angle)),
                    zoom,
                    position.x(),
                    position.y()
                    );
    }
}

void Renderer::modifyBitmap(const std::function<void(e172::Color *)> &modifier)
{
    modifier(m_writer.bitmap().matrix);
}

void Renderer::setFullscreen(bool value)
{
    m_writer.set_auto_resize(value);
}

void Renderer::setResolution(e172::Vector<double> value)
{
    m_writer.set_frame_size(value.x(), value.y());
}

e172::Vector<double> Renderer::resolution() const
{
    return e172::Vector<double>(m_writer.bitmap().width, m_writer.bitmap().height);
}

e172::Vector<double> Renderer::screenSize() const
{
    const auto siz = Writer::output_stream_size(m_writer.output(), m_writer.symbol_wh_fraction());
    return e172::Vector<double>(siz.first, siz.second);
}

} // namespace e172::impl::console
