#include "videoplayer.h"

#include "mp4_decoder.h"
#include <e172/context.h>
#include <e172/eventhandler.h>
#include <e172/graphics/abstractrenderer.h>

namespace e172::impl::console::video_player {

VideoPlayer::VideoPlayer(e172::FactoryMeta &&meta, const std::shared_ptr<MP4Decoder> &decoder)
    : e172::Entity(std::forward<e172::FactoryMeta>(meta))
    , m_decoder(decoder)
    , m_frameRateTimer(1000 * (~m_decoder->frameRate()).value())
{
    assert(m_decoder);
}

std::size_t VideoPlayer::frameCount() const
{
    return m_decoder->frameCount();
}

void VideoPlayer::proceed(e172::Context *context, e172::EventHandler *eventHandler)
{
    constexpr std::size_t rewindStep = 20;

    if (eventHandler) {
        if (eventHandler->keySinglePressed(e172::ScancodeLeft)) {
            if (m_currentFrameIndex > rewindStep) {
                m_currentFrameIndex -= rewindStep;
            } else {
                m_currentFrameIndex = 0;
            }
        } else if (eventHandler->keySinglePressed(e172::ScancodeRight)) {
            m_currentFrameIndex = (m_currentFrameIndex + rewindStep) % m_decoder->frameCount();

        } else if (eventHandler->keySinglePressed(e172::ScancodeEquals)) {
            m_scale *= 1.1;
        } else if (eventHandler->keySinglePressed(e172::ScancodeMinus)) {
            m_scale /= 1.1;
        } else if (eventHandler->keySinglePressed(e172::ScancodeW)) {
            m_centerPosition.decrementY();
        } else if (eventHandler->keySinglePressed(e172::ScancodeA)) {
            m_centerPosition.decrementX();
        } else if (eventHandler->keySinglePressed(e172::ScancodeS)) {
            m_centerPosition.incrementY();
        } else if (eventHandler->keySinglePressed(e172::ScancodeD)) {
            m_centerPosition.incrementX();

        } else if (eventHandler->keySinglePressed(e172::ScancodeSpace)) {
            m_playing = !m_playing;
        } else if (eventHandler->keySinglePressed(e172::ScancodeEscape)
                   || eventHandler->keySinglePressed(e172::ScancodeQ)) {
            context->quitLater();
        }
    }

    if (m_frameRateTimer.check(m_playing)) {
        if (m_currentFrameIndex < m_decoder->frameCount() - 1) {
            ++m_currentFrameIndex;
        } else {
            context->quitLater();
        }
    }
}

void VideoPlayer::render(e172::Context *context, e172::AbstractRenderer *renderer)
{
    const auto frame = m_decoder->frame(m_currentFrameIndex, *renderer->provider());
    renderer->drawImage(frame.image,
                        renderer->resolution().into<double>() / 2 + m_centerPosition,
                        0,
                        m_scale);
}

VideoPlayerExtension::VideoPlayerExtension(std::ostream &output,
                                           const std::shared_ptr<MP4Decoder> &decoder)
    : e172::GameApplicationExtension(PostPresentExtension)
    , m_output(output)
    , m_decoder(decoder)
{
    assert(m_decoder);
    assert(m_output.good());
}

void VideoPlayerExtension::proceed(GameApplication *application)
{
    if (!m_player) {
        m_player = e172::FactoryMeta::makeShared<video_player::VideoPlayer>(m_decoder);
        application->addEntity(m_player);
    }
    m_output << "frame: " << m_player->currentFrameIndex() << "/" << m_player->frameCount() << " ("
             << m_player->currentFrameIndex() * 100 / m_player->frameCount() << "%)" << std::endl;
}

} // namespace e172::impl::console::video_player
