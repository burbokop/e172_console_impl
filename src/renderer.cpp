#include "renderer.h"

namespace e172::impl::console {

Renderer::Renderer(Private, std::ostream &output, const Style &style)
    : m_writer(Writer(output, style))
{}

bool Renderer::update()
{
    m_writer.writeFrame();
    return true;
}

void Renderer::fill(Color color)
{
    pixel_primitives::fill_area(m_writer.bitmap(), 0, 0, m_writer.bitmap().width, m_writer.bitmap().height, color);
}

void Renderer::drawPixel(const e172::Vector<double> &point, e172::Color color)
{
    pixel_primitives::pixel(m_writer.bitmap(), point.x(), point.y()) = color;
}

void Renderer::drawLine(const e172::Vector<double> &point0,
                        const e172::Vector<double> &point1,
                        Color color)
{
    pixel_primitives::draw_line(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
}

void Renderer::drawRect(const e172::Vector<double> &point0,
                        const e172::Vector<double> &point1,
                        Color color,
                        const e172::ShapeFormat &format)
{
    if(format.fill()) {
        pixel_primitives::fill_area(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
    } else {
        pixel_primitives::draw_rect(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
    }
}

void Renderer::drawSquare(const e172::Vector<double> &center, double radius, Color color)
{
    pixel_primitives::draw_square(m_writer.bitmap(), center.x(), center.y(), radius, color);
}

void Renderer::drawCircle(const e172::Vector<double> &center, double radius, Color color)
{
    pixel_primitives::draw_circle(m_writer.bitmap(), center.x(), center.y(), radius, color);
}

void Renderer::drawImage(const e172::Image &image,
                         const e172::Vector<double> &center,
                         double angle,
                         double zoom)
{
    if(imageProvider(image) == provider()) {
        pixel_primitives::blit_transformed(m_writer.bitmap(),
                                           imageData<pixel_primitives::bitmap>(image),
                                           std::complex<double>(std::cos(angle), std::sin(angle)),
                                           zoom,
                                           center.x(),
                                           center.y());
    }
}

void Renderer::modifyBitmap(const std::function<void(e172::Color *)> &modifier)
{
    modifier(m_writer.bitmap().matrix);
}

void Renderer::setFullscreen(bool value)
{
    m_writer.setAutoResize(value);
}

void Renderer::setResolution(const e172::Vector<std::uint32_t> &value)
{
    m_writer.setFrameSize(value.x(), value.y());
}

e172::Vector<std::uint32_t> Renderer::resolution() const
{
    return e172::Vector<std::uint32_t>(m_writer.bitmap().width, m_writer.bitmap().height);
}

} // namespace e172::impl::console
