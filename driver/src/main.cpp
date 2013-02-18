
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

    Lpd8806Color palette[2] = {
        Lpd8806Color::get_closest(0, 0, 0),
        Lpd8806Color::get_closest(0, 255, 255)
    };
    PaletteBitmap1d<unsigned int, strip_device_type::raw_color, 161, sizeof(palette) / sizeof(palette[0])> bitmap1d(palette);
    Lpd8806Bitmap1dDisplay<typeof(bitmap1d.render_bitmap), strip_device_type, 161> display(&strip_device);
    RadiantMuralZigZagMutableBitmap1dAsBitmap2dAdapter<typeof(bitmap1d)> bitmap(&bitmap1d);
    GameOfLife<25, 10> game_of_life;
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
                redraw = 0;
            }
        }
    }

#elif RADIANTMURAL_TARGET_dummy

    #include <alambre/capability/2dgraphics.h>
    #include <alambre/system/sdl/2dgraphics.h>

    Uint32 palette[] = {
        0xff000000,
        0xffffffff,
    };
    PaletteBitmap2d<Uint32, Uint32, 24, 7, sizeof(palette) / sizeof(palette[0])> bitmap(palette);

    WindowedSdlBitmap2dDisplay<typeof(bitmap.render_bitmap), 24, 7, 10> display;
    GameOfLife<25, 10> game_of_life;
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
