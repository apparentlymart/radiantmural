
#include "gameoflife.h"
#include "gameofliferenderer.h"
#include <stdint.h>

// Target-specific wiring code
#ifdef RADIANTMURAL_TARGET_arduino_uno

#define F_CPU 14745600L
#include <util/delay.h>

    // For some reason these are not defined for atmega328p
    #define MOSI_DDR DDRB
    #define MOSI_PORT PORTB
    #define MOSI_PIN PINB
    #define MOSI_BIT 3
    #define SCK_DDR DDRB
    #define SCK_PORT PORTB
    #define SCK_PIN PINB
    #define SCK_BIT 5

    #include <alambre/system/avr.h>
    #include <alambre/bus/spi.h>
    #include <alambre/capability/1dgraphics.h>
    #include <alambre/capability/2dgraphics.h>
    #include <alambre/device/lpd8806.h>
    #include <avr/sleep.h>
    #include <avr/interrupt.h>
    #include "ledmatrix.h"

    volatile uint8_t redraw = 0;

    ISR(TIMER1_OVF_vect) {
        redraw = 1;
    }

    SoftwareSpiMasterOutputBus <typeof(*avr_system.B3), typeof(*avr_system.B5)> spi_bus(avr_system.B3, avr_system.B5);
    Lpd8806Device<typeof(spi_bus)> strip_device(&spi_bus);
    typedef typeof(strip_device) strip_device_type;

    Lpd8806Color palette[] = {
        Lpd8806Color::get_closest(0, 0, 0),
        Lpd8806Color::get_closest(0, 8, 8),
        Lpd8806Color::get_closest(0, 16, 16),
        Lpd8806Color::get_closest(0, 24, 24),
        Lpd8806Color::get_closest(0, 32, 32),
        Lpd8806Color::get_closest(0, 40, 40),
        Lpd8806Color::get_closest(0, 48, 48),
        Lpd8806Color::get_closest(0, 56, 56),
        Lpd8806Color::get_closest(0, 64, 64),
        Lpd8806Color::get_closest(0, 72, 72),
        Lpd8806Color::get_closest(0, 80, 80),
        Lpd8806Color::get_closest(0, 88, 88),
        Lpd8806Color::get_closest(0, 96, 96),
        Lpd8806Color::get_closest(0, 104, 104),
        Lpd8806Color::get_closest(0, 112, 112),
        Lpd8806Color::get_closest(0, 120, 120),
        Lpd8806Color::get_closest(0, 128, 128),
        Lpd8806Color::get_closest(0, 136, 136),
        Lpd8806Color::get_closest(0, 144, 144),
        Lpd8806Color::get_closest(0, 152, 152),
        Lpd8806Color::get_closest(0, 160, 160),
        Lpd8806Color::get_closest(0, 168, 168),
        Lpd8806Color::get_closest(0, 176, 176),
        Lpd8806Color::get_closest(0, 184, 184),
        Lpd8806Color::get_closest(0, 192, 192),
        Lpd8806Color::get_closest(0, 200, 200),
        Lpd8806Color::get_closest(0, 208, 208),
        Lpd8806Color::get_closest(0, 216, 216),
        Lpd8806Color::get_closest(0, 224, 224),
        Lpd8806Color::get_closest(0, 232, 232),
        Lpd8806Color::get_closest(0, 240, 240),
        Lpd8806Color::get_closest(0, 254, 254)
    };
    PaletteBitmap1d<unsigned int, strip_device_type::raw_color, 161, sizeof(palette) / sizeof(palette[0])> bitmap1d(palette);
    Lpd8806Bitmap1dDisplay<typeof(bitmap1d.render_bitmap), strip_device_type, 161> display(&strip_device);
    RadiantMuralZigZagMutableBitmap1dAsBitmap2dAdapter<typeof(bitmap1d)> bitmap(&bitmap1d);
    GameOfLife<24, 9> game_of_life;
    GameOfLifeRenderer<typeof(game_of_life), typeof(bitmap)> renderer(&game_of_life, &bitmap);
    auto white = strip_device.get_closest_color(255, 255, 255);
    auto blue = strip_device.get_closest_color(0, 0, 127);

    inline void main_loop() {
        // Enable the overflow interrupt.
        TIMSK1 |= (1 << TOIE1);

        // Start timer at Fcpu/8
        TCCR1B |= (1 << CS11);

        while (1) {
            /*sleep_enable();
            set_sleep_mode(SLEEP_MODE_PWR_SAVE);
            sei();
            sleep_cpu();
            sleep_disable();
            cli();*/
            redraw = 1;
            _delay_ms(150);
            if (redraw) {
                game_of_life.next_frame();
                renderer.update();
                display.update(&bitmap1d.render_bitmap);
                game_of_life.set_pixel(13, 6);
                redraw = 0;
            }
        }
    }

#elif RADIANTMURAL_TARGET_dummy

    #include <alambre/capability/2dgraphics.h>
    #include <alambre/system/sdl/2dgraphics.h>

    Uint32 palette[] = {
        0xff000000,
        0xff080808,
        0xff0f0f0f,
        0xff181818,
        0xff202020,
        0xff282828,
        0xff303030,
        0xff383838,
        0xff404040,
        0xff484848,
        0xff505050,
        0xff585858,
        0xff606060,
        0xff686868,
        0xff707070,
        0xff787878,
        0xff808080,
        0xff888888,
        0xff909090,
        0xff989898,
        0xffa0a0a0,
        0xffa8a8a8,
        0xffb0b0b0,
        0xffb8b8b8,
        0xffc0c0c0,
        0xffc8c8c8,
        0xffd0d0d0,
        0xffd8d8d8,
        0xffe0e0e0,
        0xffe8e8e8,
        0xfff0f0f0,
        0xfff8f8f8,
    };
    PaletteBitmap2d<Uint32, Uint32, 24, 7, sizeof(palette) / sizeof(palette[0])> bitmap(palette);

    WindowedSdlBitmap2dDisplay<typeof(bitmap.render_bitmap), 24, 7, 10> display;
    GameOfLife<24, 9> game_of_life;
    GameOfLifeRenderer<typeof(game_of_life), typeof(bitmap)> renderer(&game_of_life, &bitmap);
    auto white = display.get_closest_color(255, 255, 255);
    auto blue = display.get_closest_color(0, 0, 127);

    Uint32 timer_func(Uint32 interval, void *param) {
        // Just generate a dummy event so our mainloop
        // will wake up periodically to process this nonsense message.
        SDL_Event event;
        SDL_UserEvent userevent;
        event.type = SDL_USEREVENT;
        event.user = userevent;
        userevent.type = SDL_USEREVENT;
        userevent.code = 0;
        userevent.data1 = NULL;
        userevent.data2 = NULL;
        event.user = userevent;
        SDL_PushEvent(&event);
        return interval;
    }

    inline void main_loop() {
        SDL_Event event;

        SDL_Init(SDL_INIT_EVERYTHING);

        SDL_TimerID timerid = SDL_AddTimer(200, timer_func, NULL);
        if (timerid == NULL) {
            printf("Failed to timer: %s\n", SDL_GetError());
        }

        while (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
            else if (event.type == SDL_USEREVENT) {
                game_of_life.next_frame();
                renderer.update();
                display.update(&bitmap.render_bitmap);
                game_of_life.set_pixel(13, 6);
            }
        }
    }

#else
#error No wiring code for this target.
#endif

int main() {

    /*game_of_life.set_pixel(3, 1);
    game_of_life.set_pixel(4, 1);
    game_of_life.set_pixel(5, 1);*/

    game_of_life.set_pixel(8, 4);
    game_of_life.set_pixel(8, 5);
    game_of_life.set_pixel(8, 6);
    game_of_life.set_pixel(9, 4);
    game_of_life.set_pixel(7, 5);

    /*game_of_life.set_pixel(12, 4);
    game_of_life.set_pixel(12, 5);
    game_of_life.set_pixel(12, 6);
    game_of_life.set_pixel(11, 6);
    game_of_life.set_pixel(10, 5);*/

    main_loop();

}
