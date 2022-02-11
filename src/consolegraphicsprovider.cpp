#include "consolegraphicsprovider.h"
#include <fstream>
#include "png_reader.h"

e172::Image ConsoleGraphicsProvider::imageFromBitmap(const pixel_primitives::bitmap &btmp) const {
    return imageFromData(new e172::Image::handle<pixel_primitives::bitmap>(btmp), btmp.width, btmp.height);
}

ConsoleGraphicsProvider::ConsoleGraphicsProvider(const std::vector<std::string> &args, std::ostream &output)
    : e172::AbstractGraphicsProvider(args),
      m_output(output) {}

e172::AbstractRenderer *ConsoleGraphicsProvider::renderer() const {
    if(!m_renderer) {
        m_renderer = new ConsoleRenderer(m_output);
    }
    return m_renderer;
}

bool ConsoleGraphicsProvider::isValid() const {
    return m_output.rdbuf();
}

e172::Image ConsoleGraphicsProvider::loadImage(const std::string &path) const {
    std::ifstream ifile(path, std::ios::in);
    const auto& btmp = png_reader::read(ifile);
    ifile.close();
    return imageFromBitmap(btmp);
}

e172::Image ConsoleGraphicsProvider::createImage(int width, int height) const {
    return imageFromBitmap(pixel_primitives::bitmap {
                               new std::uint32_t[width * height],
                               std::size_t(width),
                               std::size_t(height)
                           });
}

e172::Image ConsoleGraphicsProvider::createImage(int width, int height, const ImageInitFunction &imageInitFunction) const {
    if(imageInitFunction) {
        pixel_primitives::bitmap btmp { new std::uint32_t[width * height], std::size_t(width), std::size_t(height) };
        imageInitFunction(btmp.matrix);
        return imageFromBitmap(btmp);
    }
    return e172::Image();
}

e172::Image ConsoleGraphicsProvider::createImage(int width, int height, const ImageInitFunctionExt &imageInitFunction) const {
    if(imageInitFunction) {
        pixel_primitives::bitmap btmp { new std::uint32_t[width * height], std::size_t(width), std::size_t(height) };
        imageInitFunction(width, height, btmp.matrix);
        return imageFromBitmap(btmp);
    }
    return e172::Image();
}

void ConsoleGraphicsProvider::destructImage(e172::SharedContainer::data_ptr ptr) const {
    const auto& handle = e172::Image::handle_cast<pixel_primitives::bitmap>(ptr);
    const auto& siz = handle->c.width * handle->c.height;
    delete[] handle->c.matrix;
    delete handle;
}

e172::SharedContainer::ptr ConsoleGraphicsProvider::imageBitMap(e172::SharedContainer::data_ptr ptr) const {
    return e172::Image::handle_cast<pixel_primitives::bitmap>(ptr)->c.matrix;
}

e172::SharedContainer::data_ptr ConsoleGraphicsProvider::imageFragment(e172::SharedContainer::data_ptr ptr, int x, int y, int &w, int &h) const {
    const auto& btmp = e172::Image::handle_cast<pixel_primitives::bitmap>(ptr)->c;
    pixel_primitives::bitmap result { new std::uint32_t[w * h], std::size_t(w), std::size_t(h) };
    pixel_primitives::blit(result, btmp, -x, -y, w, h);
    return new e172::Image::handle<pixel_primitives::bitmap>(result);
}

e172::SharedContainer::data_ptr ConsoleGraphicsProvider::blitImages(e172::SharedContainer::data_ptr ptr0, e172::SharedContainer::data_ptr ptr1, int x, int y, int &w, int &h) const {
    const auto& btmp0 = e172::Image::handle_cast<pixel_primitives::bitmap>(ptr0)->c;
    const auto& btmp1 = e172::Image::handle_cast<pixel_primitives::bitmap>(ptr1)->c;
    pixel_primitives::bitmap result { new std::uint32_t[btmp0.width * btmp1.width], btmp0.width, btmp0.height };
    pixel_primitives::copy(result, btmp0);
    pixel_primitives::blit(result, btmp1, x, y, w, h);
    return new e172::Image::handle<pixel_primitives::bitmap>(result);
}

