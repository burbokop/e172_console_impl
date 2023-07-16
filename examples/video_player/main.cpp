#include "../../src/eventprovider.h"
#include "../../src/graphicsprovider.h"
#include "../../src/png_reader.h"
#include "mp4_decoder.h"
#include "painter.h"
#include "videoplayer.h"
#include <SDL2/SDL.h>
#include <cassert>
#include <curses.h>
#include <e172/gameapplication.h>
#include <e172/net/linux/socket.h>
#include <e172/utility/flagparser.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

namespace e172::impl::console::video_player {

inline pixel_primitives::bitmap sdl_surface_to_bitmap(SDL_Surface* surf) {
    return pixel_primitives::bitmap {
        .matrix = reinterpret_cast<std::uint32_t*>(surf->pixels),
        .width = static_cast<std::size_t>(surf->w),
        .height = static_cast<std::size_t>(surf->h)
    };
}

} // namespace e172::impl::console::video_player

struct Flags
{
    /// input file path
    std::filesystem::path input;
    double scale;
    bool v2;
    double contrast;
    std::uint8_t deterioration;
};

int mainV1(int argc, const char **argv, const Flags &flags);
int mainV2(int argc, const char **argv, const Flags &flags);

int main(int argc, const char **argv)
{
    const auto flags
        = e172::FlagParser::parse<Flags>(
              argc,
              argv,
              [](e172::FlagParser &p) {
                  return Flags{
                      .input = p.flag<std::filesystem::path>(
                          e172::Flag{.shortName = "i",
                                     .longName = "input",
                                     .description = "Input file path (.mp4 file)"}),
                      .scale = p.flag<double>(e172::Flag{.shortName = "s",
                                                         .longName = "scale",
                                                         .description = "Scale of video frame"}),
                      .v2 = p.flag<bool>(e172::Flag{.shortName = "v2",
                                                    .longName = "use-version2",
                                                    .description = "Use version 2"}),
                      .contrast = p.flag(e172::OptFlag<double>{.shortName = "c",
                                                               .longName = "contrast",
                                                               .description = "Contrast of video",
                                                               .defaultVal = 1}),
                      .deterioration = p.flag(
                          e172::OptFlag<std::uint8_t>{.shortName = "d",
                                                      .longName = "deterioration",
                                                      .description = "Deterioration of color",
                                                      .defaultVal = 32})};
              },
              [](const e172::FlagParser &p) {
                  p.displayErr(std::cerr);
                  std::exit(1);
              },
              [](const e172::FlagParser &p) {
                  p.displayHelp(std::cout);
                  std::exit(0);
              },
              nullptr)
              .value();

    if (flags.v2) {
        return mainV2(argc, argv, flags);
    } else {
        return mainV1(argc, argv, flags);
    }
}

int mainV1(int argc, const char **argv, const Flags &flags)
{
    using namespace e172::impl::console;

    std::iostream null(0);

    const auto absolutePath = std::filesystem::absolute(flags.input);
    video_player::MP4Decoder decoder(absolutePath.c_str(), null, flags.scale);

    std::cout << "Frame count: " << decoder.frameCount() << std::endl
              << "press Left to rewind back" << std::endl
              << "right to rewind forward" << std::endl
              << "space to pause" << std::endl
              << "esc to exit" << std::endl
              << "To begin press any button";

    EventProvider eventProvider(null);
    while (!eventProvider.pullEvent()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    const auto graphicsProvider = std::make_shared<GraphicsProvider>(std::cout);
    e172::GameApplication app(argc, argv);
    app.setGraphicsProvider(graphicsProvider);
    app.initRenderer("video player", decoder.size());

    //return 0;

    //std::ifstream istr("~/Pictures/sculpture_64.png");
    //if(!istr.is_open()) {
    //    throw std::runtime_error("file not open");
    //}
    //const auto img = png::read(istr);

    //std::cout << "w: " << img.width << ", h:" << img.height << "\n";
    //exit(0);

    auto window = SDL_CreateWindow("app_using_spm", 0, 0, 700, 500, 0);

    auto sdl_surface = SDL_GetWindowSurface(window);

    video_player::painter p;

    AnsiColorizer ansi_colorizer;

    std::ofstream out("output.ansi", std::ios::out);

    Writer s(std::cout,
             Style{.colorizer = std::make_shared<AnsiTrueColorizer>(flags.deterioration),
                   .gradient = DefaultGradient,
                   .contrast = flags.contrast,
                   .ignoreAlpha = false});

    std::size_t last_w = 0;
    std::size_t last_h = 0;

    e172::ElapsedTimer updateTimer(1000 / 30);


    e172::ElapsedTimer frameChangeTimer(1000 / 4);
    std::size_t frame_index = 0;
    double angle = 0;

    bool framesIncrementing = true;
    bool exit = false;
    while (!exit && frame_index < decoder.frameCount()) {
        if (updateTimer.check()) {
            constexpr std::size_t rewindStep = 20;

            while (const auto &event = eventProvider.pullEvent()) {
                if (event->type() == e172::Event::KeyUp) {
                    if (event->scancode().value() == e172::ScancodeLeft) {
                        if (frame_index > rewindStep) {
                            frame_index -= rewindStep;
                        } else {
                            frame_index = 0;
                        }
                    } else if (event->scancode().value() == e172::ScancodeRight) {
                        frame_index = (frame_index + rewindStep) % decoder.frameCount();
                    } else if (event->scancode().value() == e172::ScancodeSpace) {
                        framesIncrementing = !framesIncrementing;
                    } else if (event->scancode().value() == e172::ScancodeEscape) {
                        exit = true;
                    }
                }
            }

            if (!framesIncrementing)
                continue;

            pixel_primitives::fill_area(s.bitmap(),
                                        0,
                                        0,
                                        s.bitmap().width,
                                        s.bitmap().height,
                                        0x00000000);

            p.paint(s.bitmap());

            pixel_primitives::draw_rect(s.bitmap(),
                                        0,
                                        0,
                                        s.bitmap().width - 2,
                                        s.bitmap().height - 2,
                                        0xff0000ff);

            pixel_primitives::draw_grid(s.bitmap(), 10, 4, 60, 36, 4, 0xffff4800);

            pixel_primitives::draw_circle(s.bitmap(),
                                          s.bitmap().width / 2,
                                          s.bitmap().height / 2,
                                          12,
                                          0xff00ff00);

            //pixel_primitives::rotate(s.bitmap(),
            //                         decoder.frame(frame_index),
            //                         std::complex<double>(std::cos(angle), std::sin(angle)));

            pixel_primitives::copy(s.bitmap(), decoder.frame(frame_index, *graphicsProvider).bitmap);

            //pixel_primitives::blit(s.bitmap(), decoder.frame(frame_index), 0, 0);

            if (false && (s.bitmap().width != last_w || s.bitmap().height != last_h)) {
                SDL_FreeSurface(sdl_surface);
                SDL_SetWindowSize(window, s.bitmap().width, s.bitmap().height);
                sdl_surface = SDL_GetWindowSurface(window);
                last_w = s.bitmap().width;
                last_h = s.bitmap().height;
            }
            SDL_FillRect(sdl_surface, nullptr, 0x00000000);
            SDL_LockSurface(sdl_surface);
            auto btmp = video_player::sdl_surface_to_bitmap(sdl_surface);
            pixel_primitives::copy(btmp, s.bitmap());

            const auto frame = decoder.frame(frame_index, *graphicsProvider);

            pixel_primitives::blit_transformed(btmp,
                                               frame.bitmap,
                                               std::complex<double>(std::cos(angle),
                                                                    std::sin(angle)),
                                               (std::sin(frame_index * 0.02) + 2) / 3.,
                                               100,
                                               100);
            angle += 0.01;

            //if(frameChangeTimer.check()) {
            ++frame_index;
            frame_index %= decoder.frameCount();
            //}

            SDL_UnlockSurface(sdl_surface);

            //app.renderer()->drawImage(frame.image, {}, 0, 1);

            s.writeFrame();

            std::cout << "frame: " << frame_index << "/" << decoder.frameCount() << " ("
                      << frame_index * 100 / decoder.frameCount() << "%)" << std::endl;

            SDL_UpdateWindowSurface(window);
        }
    }

    return 0;
}

int mainV2(int argc, const char **argv, const Flags &flags)
{
    auto a = new std::uint32_t[426 * 240];

    std::cout << "a: " << a << std::endl;

    using namespace e172::impl::console;

    std::iostream null(0);
    std::ofstream log("/tmp/console-event-provider.log");

    const auto decoder = std::make_shared<video_player::MP4Decoder>(std::filesystem::absolute(
                                                                        flags.input),
                                                                    null,
                                                                    flags.scale);

    e172::GameApplication app(argc, argv);

    AnsiTrueColorizer colorizer = 32;

    const auto graphicsProvider
        = std::make_shared<GraphicsProvider>(std::cout,
                                             Style{.colorizer = std::make_shared<AnsiTrueColorizer>(
                                                       flags.deterioration),
                                                   .gradient = DefaultGradient,
                                                   .contrast = flags.contrast});

    const auto eventProvider = std::make_shared<EventProvider>(log);

    std::cout
        << "Frame count: " << decoder->frameCount() << std::endl
        << "Frame rate: " << decoder->frameRate().intoReal() << std::endl
        << "Draw area size: " << graphicsProvider->screenSize() << std::endl
        << "Control buttons:" << std::endl
        << "  Left    - rewind back" << std::endl
        << "  Right   - rewind forward" << std::endl
        << "  Space   - pause" << std::endl
        << "  Plus    - zoom in" << std::endl
        << "  Minus   - zoom out" << std::endl
        << "  Esc | Q - exit" << std::endl
        << "Also you can use Ctrl + Mouse weel on most terminal emulators to change draw area size"
        << std::endl
        << "To begin press any button" << std::endl;

    while (!eventProvider->pullEvent()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    app.setGraphicsProvider(graphicsProvider);
    app.setEventProvider(eventProvider);
    app.initRenderer("video player", graphicsProvider->screenSize());

    app.setProccedInterval(1000 / 60);
    app.setRenderInterval(1000 / 60);

    app.addApplicationExtension<video_player::VideoPlayerExtension>(std::cout, decoder);

    return app.exec();
}
