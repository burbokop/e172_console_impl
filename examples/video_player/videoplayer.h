#pragma once

#include <e172/entity.h>
#include <e172/gameapplication.h>
#include <e172/time/elapsedtimer.h>

namespace e172::impl::console::video_player {

class MP4Decoder;

class VideoPlayer : public e172::Entity
{
public:
    VideoPlayer(e172::FactoryMeta &&meta, const std::shared_ptr<MP4Decoder> &decoder);

    std::size_t currentFrameIndex() const { return m_currentFrameIndex; };
    std::size_t frameCount() const;

    // Entity interface
public:
    void proceed(e172::Context *context, e172::EventHandler *eventHandler) override;
    void render(e172::Context *context, e172::AbstractRenderer *renderer) override;

private:
    std::shared_ptr<MP4Decoder> m_decoder;
    std::size_t m_currentFrameIndex = 0;
    e172::ElapsedTimer m_frameRateTimer;
    e172::Vector<double> m_centerPosition;
    bool m_playing = true;
    double m_scale = 1;
};

class VideoPlayerExtension : public e172::GameApplicationExtension
{
public:
    VideoPlayerExtension(std::ostream &output, const std::shared_ptr<MP4Decoder> &decoder);

    // GameApplicationExtension interface
public:
    virtual void proceed(e172::GameApplication *application) override;

private:
    std::ostream &m_output;
    std::shared_ptr<MP4Decoder> m_decoder;
    std::shared_ptr<VideoPlayer> m_player;
};

} // namespace e172::impl::console::video_player
