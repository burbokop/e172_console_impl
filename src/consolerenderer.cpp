#include "consolerenderer.h"

ConsoleRenderer::ConsoleRenderer(std::ostream &output)
    : m_surface(surface(output)) {}

bool ConsoleRenderer::update() {
    m_surface.update();
    return true;
}

void ConsoleRenderer::setDepth(int64_t depth) {

}

void ConsoleRenderer::fill(uint32_t color) {

}

void ConsoleRenderer::drawPixel(const e172::Vector &point, uint32_t color) {

}

void ConsoleRenderer::drawLine(const e172::Vector &point0, const e172::Vector &point1, uint32_t color) {

}

void ConsoleRenderer::drawRect(const e172::Vector &point0, const e172::Vector &point1, uint32_t color, const e172::ShapeFormat &format) {

}

void ConsoleRenderer::drawSquare(const e172::Vector &point, int radius, uint32_t color) {

}

void ConsoleRenderer::drawCircle(const e172::Vector &point, int radius, uint32_t color) {

}

void ConsoleRenderer::setResolution(e172::Vector value) {
    m_surface.set_size(value.x(), value.y());
}

e172::Vector ConsoleRenderer::resolution() const {
    return e172::Vector(m_surface.bitmap().width, m_surface.bitmap().height);
}

e172::Vector ConsoleRenderer::screenSize() const {
    const auto siz = m_surface.console_size(STDOUT_FILENO);
    return e172::Vector(siz.first, siz.second);
}

