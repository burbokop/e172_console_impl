#pragma once

#include "renderer.h"
#include <e172/graphics/abstractgraphicsprovider.h>

namespace e172::impl::console {

class GraphicsProvider : public e172::AbstractGraphicsProvider
{
    std::ostream& m_output;
    mutable Renderer *m_renderer = nullptr;
    e172::Image imageFromBitmap(const pixel_primitives::bitmap& btmp) const;
public:
    GraphicsProvider(const std::vector<std::string> &args, std::ostream &output);

    // AbstractGraphicsProvider interface
public:
    virtual e172::AbstractRenderer *renderer() const override;
    virtual bool isValid() const override;
    virtual e172::Image loadImage(const std::string &path) const override;
    virtual e172::Image createImage(int width, int height) const override;
    virtual e172::Image createImage(int width, int height, const ImageInitFunction &imageInitFunction) const override;
    virtual e172::Image createImage(int width, int height, const ImageInitFunctionExt &imageInitFunction) const override;
    virtual void loadFont(const std::string &, const std::string &) override {}
    virtual bool fontLoaded(const std::string &) const override { return false; }

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
                                                      int x,
                                                      int y,
                                                      std::size_t &w,
                                                      std::size_t &h) const override;

    virtual e172::SharedContainer::DataPtr transformImage(e172::SharedContainer::DataPtr ptr,
                                                          std::uint64_t) const override
    {
        return ptr;
    }
};

} // namespace e172::impl::console
