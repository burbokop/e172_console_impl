#include "consolerenderer.h"

ConsoleRenderer::ConsoleRenderer(std::ostream &output)
    : m_writer(console_writer(output)) {}

bool ConsoleRenderer::update() {
    m_writer.write_frame();
    return true;
}

void ConsoleRenderer::fill(uint32_t color) {
    pixel_primitives::fill_area(m_writer.bitmap(), 0, 0, m_writer.bitmap().width, m_writer.bitmap().height, color);
}

void ConsoleRenderer::drawPixel(const e172::Vector &point, uint32_t color) {
    pixel_primitives::pixel(m_writer.bitmap(), point.x(), point.y()) = color;
}

void ConsoleRenderer::drawLine(const e172::Vector &point0, const e172::Vector &point1, uint32_t color) {
    pixel_primitives::draw_line(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
}

void ConsoleRenderer::drawRect(const e172::Vector &point0, const e172::Vector &point1, uint32_t color, const e172::ShapeFormat &format) {
    if(format.fill()) {
        pixel_primitives::fill_area(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
    } else {
        pixel_primitives::draw_rect(m_writer.bitmap(), point0.x(), point0.y(), point1.x(), point1.y(), color);
    }
}

void ConsoleRenderer::drawSquare(const e172::Vector &point, int radius, uint32_t color) {
    pixel_primitives::draw_square(m_writer.bitmap(), point.x(), point.y(), radius, color);
}

void ConsoleRenderer::drawCircle(const e172::Vector &point, int radius, uint32_t color) {
    pixel_primitives::draw_circle(m_writer.bitmap(), point.x(), point.y(), radius, color);
}

void ConsoleRenderer::drawImage(const e172::Image &image, const e172::Vector &position, double angle, double zoom) {
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

e172::Color *ConsoleRenderer::bitmap() const { return m_writer.bitmap().matrix; }

void ConsoleRenderer::setFullscreen(bool value) { m_writer.set_auto_resize(value); }

void ConsoleRenderer::setResolution(e172::Vector value) {
    m_writer.set_frame_size(value.x(), value.y());
}

e172::Vector ConsoleRenderer::resolution() const {
    return e172::Vector(m_writer.bitmap().width, m_writer.bitmap().height);
}

e172::Vector ConsoleRenderer::screenSize() const {
    const auto siz = console_writer::output_stream_size(m_writer.output(), m_writer.symbol_wh_fraction());
    return e172::Vector(siz.first, siz.second);
}

