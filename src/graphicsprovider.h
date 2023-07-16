#pragma once

#include "renderer.h"
#include <e172/graphics/abstractgraphicsprovider.h>

namespace e172::impl::console {

class GraphicsProvider : public e172::AbstractGraphicsProvider
{
public:
    GraphicsProvider(std::ostream &output, const Style &style = {});

    // AbstractGraphicsProvider interface
public:
    virtual std::shared_ptr<e172::AbstractRenderer> createRenderer(
        const std::string &title, const Vector<std::uint32_t> &resolution) const override;

    virtual e172::Image loadImage(const std::string &path) const override;
    virtual e172::Image createImage(std::size_t width, std::size_t height) const override;
    virtual e172::Image createImage(std::size_t width,
                                    std::size_t height,
                                    const ImageInitFunction &imageInitFunction) const override;
    virtual e172::Image createImage(std::size_t width,
                                    std::size_t height,
                                    const ImageInitFunctionExt &imageInitFunction) const override;
    virtual void loadFont(const std::string &, const std::filesystem::path &) override {}
    virtual bool fontLoaded(const std::string &) const override { return false; }
    virtual e172::Vector<std::uint32_t> screenSize() const override;

protected:
    virtual void destructImage(e172::SharedContainer::DataPtr ptr) const override;
    virtual e172::SharedContainer::Ptr imageBitMap(e172::SharedContainer::DataPtr ptr) const override;
    virtual bool saveImage(e172::SharedContainer::DataPtr, const std::string &) const override
    {
        return false;
    }

    virtual e172::SharedContainer::DataPtr imageFragment(e172::SharedContainer::DataPtr ptr,
                                                         std::size_t x,
                                                         std::size_t y,
                                                         std::size_t &w,
                                                         std::size_t &h) const override;

    virtual e172::SharedContainer::DataPtr blitImages(e172::SharedContainer::DataPtr ptr0,
                                                      e172::SharedContainer::DataPtr ptr1,
                                                      std::ptrdiff_t x,
                                                      std::ptrdiff_t y,
                                                      std::size_t &w,
                                                      std::size_t &h) const override;

    virtual e172::SharedContainer::DataPtr transformImage(e172::SharedContainer::DataPtr ptr,
                                                          std::uint64_t) const override
    {
        return ptr;
    }

private:
    e172::Image imageFromBitmap(const pixel_primitives::bitmap &btmp) const;

private:
    std::ostream &m_output;
    Style m_style;
};

} // namespace e172::impl::console
