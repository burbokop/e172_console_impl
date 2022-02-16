
#include <iostream>
#include "painter.h"
#include <SDL2/SDL.h>
#include "mp4_decoder.h"
#include "png_reader.h"
#include <fstream>
#include <curses.h>
#include <thread>
#include <termios.h>
#include <cassert>

int getch2(FILE *file) {
      int c=0;

      struct termios org_opts, new_opts;
      int res=0;
          //-----  store old settings -----------
      res=tcgetattr(fileno(file), &org_opts);
      assert(res==0);
          //---- set new terminal parms --------
      memcpy(&new_opts, &org_opts, sizeof(new_opts));
      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      new_opts.c_cc[VTIME] = 10;
      new_opts.c_cc[VMIN] = 1;
      tcsetattr(fileno(file), TCSANOW, &new_opts);

      c=fgetc(file);
          //------  restore old settings ---------
      res=tcsetattr(fileno(file), TCSANOW, &org_opts);
      assert(res==0);
      return(c);
}


int getkey(FILE *file) {
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(file), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 100;
    tcsetattr(fileno(file), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(file);

    /* restore the original terminal attributes */
    tcsetattr(fileno(file), TCSANOW, &orig_term_attr);

    return character;
}

inline pixel_primitives::bitmap sdl_surface_to_bitmap(SDL_Surface* surf) {
    return pixel_primitives::bitmap {
        .matrix = reinterpret_cast<std::uint32_t*>(surf->pixels),
        .width = static_cast<std::size_t>(surf->w),
        .height = static_cast<std::size_t>(surf->h)
    };
}

int main() {
    //std::vector<pixel_primitives::bitmap> vvv;

    std::iostream null (0);


    //mp4_decoder decoder("/home/borys/Videos/a.mp4", std::cout, 1 / 4.);
    //mp4_decoder decoder("/home/borys/Videos/DeadPiratesUGO.mp4", null, 1 / 4.);

    mp4_decoder decoder("/home/borys/Videos/video_2022-02-10_12-08-58.mp4", null, 1 / 8.);



    std::cout << "frame count: " << decoder.frame_count() << std::endl;
    //return 0;


    std::ifstream istr("/home/borys/Pictures/sculpture_64.png");
    if(!istr.is_open()) {
        throw std::runtime_error("file not open");
    }
    const auto img = png_reader::read(istr);

    //std::cout << "w: " << img.width << ", h:" << img.height << "\n";
    //exit(0);

    //auto window = SDL_CreateWindow("app_using_spm", 0, 0, 700, 500, 0);

    //auto sdl_surface = SDL_GetWindowSurface(window);

    painter p;

    ansi_colorizer ansi_colorizer;
    ansi_true_colorizer ansi_true_colorizer = 32;

    std::ofstream out("output.ansi", std::ios::out);

    console_writer s(std::cout, &ansi_true_colorizer, console_writer::default_gradient, 1, true, 0xffffffff, false);

    std::size_t last_w = 0;
    std::size_t last_h = 0;

    e172::ElapsedTimer updateTimer(1000 / 30);


    e172::ElapsedTimer frameChangeTimer(1000 / 4);
    std::size_t frame_index = 0;
    double angle = 0;

    while (true) {
        if(updateTimer.check()) {
            pixel_primitives::fill_area(s.bitmap(), 0, 0, s.bitmap().width, s.bitmap().height, 0x00000000);

            //p.paint(s.bitmap());

            //pixel_primitives::draw_rect(s.bitmap(), 0, 0, s.bitmap().width - 2, s.bitmap().height - 2, 0xff0000ff);

            //pixel_primitives::draw_grid(s.bitmap(), 10, 4, 60, 36, 4, 0xffff4800);

            //pixel_primitives::draw_circle(s.bitmap(), s.bitmap().width / 2, s.bitmap().height / 2, 12, 0xff00ff00);


            const int key = getch2(stdin);
            //const int c1 = getch();

            if (key == 0x1B || key == 0x04) {
                break;
            }

            //pixel_primitives::rotate(s.bitmap(), decoder.frame(frame_index), std::complex<double>(std::cos(angle), std::sin(angle)));

            //pixel_primitives::copy(s.bitmap(), decoder.frame(frame_index));

            //pixel_primitives::blit(s.bitmap(), decoder.frame(frame_index), 0, 0);


            if(false && (s.bitmap().width != last_w || s.bitmap().height != last_h)) {
                //SDL_FreeSurface(sdl_surface);
                //SDL_SetWindowSize(window, s.bitmap().width, s.bitmap().height);
                //sdl_surface = SDL_GetWindowSurface(window);
                last_w = s.bitmap().width;
                last_h = s.bitmap().height;
            }
            //SDL_FillRect(sdl_surface, nullptr, 0x00000000);
            //SDL_LockSurface(sdl_surface);
            //auto btmp = sdl_surface_to_bitmap(sdl_surface);
            //pixel_primitives::copy(btmp, s.bitmap());

            const auto frame = decoder.frame(frame_index);

            /*
            pixel_primitives::blit_transformed(
                        btmp,
                        frame,
                        std::complex<double>(std::cos(angle), std::sin(angle)),
                        (std::sin(frame_index * 0.02) + 2) / 3.,
                        100,
                        100
                        );
            angle += 0.01;
            */



            //if(frameChangeTimer.check()) {
            ++frame_index;
            frame_index %= decoder.frame_count();
            //}

            //SDL_UnlockSurface(sdl_surface);

            s.write_frame();

            std::cout << "key: " << key << std::endl;

            //SDL_UpdateWindowSurface(window);
        }
    }

    return 0;
}
