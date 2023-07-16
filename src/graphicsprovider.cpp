#include "graphicsprovider.h"

#include <fstream>
#include "png_reader.h"

namespace e172::impl::console {

e172::Image GraphicsProvider::imageFromBitmap(const pixel_primitives::bitmap &btmp) const
{
    return imageFromData(new e172::Image::Handle<pixel_primitives::bitmap>(btmp),
                         btmp.width,
                         btmp.height);
}

GraphicsProvider::GraphicsProvider(std::ostream &output, const Style &style)
    : m_output(output)
    , m_style(style)
{}

std::shared_ptr<AbstractRenderer> GraphicsProvider::createRenderer(
    const std::string &, const Vector<std::uint32_t> &) const
{
    const auto renderer = std::make_shared<Renderer>(Renderer::Private{}, m_output, m_style);
    installParentToRenderer(*renderer);
    return renderer;
}

e172::Image GraphicsProvider::loadImage(const std::string &path) const
{
    std::ifstream ifile(path, std::ios::in);
    const auto &btmp = png::read(ifile);
    ifile.close();
    return imageFromBitmap(btmp);
}

e172::Image GraphicsProvider::createImage(std::size_t width, std::size_t height) const
{
    return imageFromBitmap(
        pixel_primitives::bitmap{new std::uint32_t[width * height], width, height});
}

e172::Image GraphicsProvider::createImage(std::size_t width,
                                          std::size_t height,
                                          const ImageInitFunction &imageInitFunction) const
{
    if(imageInitFunction) {
        pixel_primitives::bitmap btmp{new std::uint32_t[width * height], width, height};
        imageInitFunction(btmp.matrix);
        return imageFromBitmap(btmp);
    }
    return e172::Image();
}

e172::Image GraphicsProvider::createImage(std::size_t width,
                                          std::size_t height,
                                          const ImageInitFunctionExt &imageInitFunction) const
{
    if(imageInitFunction) {
        pixel_primitives::bitmap btmp{new std::uint32_t[width * height], width, height};
        imageInitFunction(width, height, btmp.matrix);
        return imageFromBitmap(btmp);
    }
    return e172::Image();
}

void GraphicsProvider::destructImage(e172::SharedContainer::DataPtr ptr) const
{
    const auto &handle = e172::Image::castHandle<pixel_primitives::bitmap>(ptr);
    delete[] handle->c.matrix;
    delete handle;
}

e172::SharedContainer::Ptr GraphicsProvider::imageBitMap(e172::SharedContainer::DataPtr ptr) const
{
    return e172::Image::castHandle<pixel_primitives::bitmap>(ptr)->c.matrix;
}

e172::SharedContainer::DataPtr GraphicsProvider::imageFragment(e172::SharedContainer::DataPtr ptr,
                                                               std::size_t x,
                                                               std::size_t y,
                                                               std::size_t &w,
                                                               std::size_t &h) const
{
    const auto &btmp = e172::Image::castHandle<pixel_primitives::bitmap>(ptr)->c;
    pixel_primitives::bitmap result { new std::uint32_t[w * h], std::size_t(w), std::size_t(h) };
    pixel_primitives::blit(result, btmp, -x, -y, w, h);
    return new e172::Image::Handle<pixel_primitives::bitmap>(result);
}

e172::SharedContainer::DataPtr GraphicsProvider::blitImages(e172::SharedContainer::DataPtr ptr0,
                                                            e172::SharedContainer::DataPtr ptr1,
                                                            std::ptrdiff_t x,
                                                            std::ptrdiff_t y,
                                                            std::size_t &w,
                                                            std::size_t &h) const
{
    const auto &btmp0 = e172::Image::castHandle<pixel_primitives::bitmap>(ptr0)->c;
    const auto &btmp1 = e172::Image::castHandle<pixel_primitives::bitmap>(ptr1)->c;
    pixel_primitives::bitmap result { new std::uint32_t[btmp0.width * btmp1.width], btmp0.width, btmp0.height };
    pixel_primitives::copy(result, btmp0);
    pixel_primitives::blit(result, btmp1, x, y, w, h);
    return new e172::Image::Handle<pixel_primitives::bitmap>(result);
}

e172::Vector<uint32_t> GraphicsProvider::screenSize() const
{
    return Writer::outputStreamSize(m_output, m_style.symbolWHFraction);
}

} // namespace e172::impl::console
