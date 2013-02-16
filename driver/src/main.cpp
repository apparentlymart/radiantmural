
#include "gameoflife.h"
#include <stdint.h>

// Target-specific wiring code
#ifdef RADIANTMURAL_TARGET_arduino_uno

    #include <alambre/system/avr.h>
    #include <alambre/capability/1dgraphics.h>
    #include <alambre/device/lpd8806.h>
    #include <avr/sleep.h>
    #include <avr/interrupt.h>
    #include "ledmatrix.h"

    volatile uint8_t redraw = 0;

    ISR(TIMER1_OVF_vect) {
        redraw = 1;
    }

    Lpd8806Device<typeof(*avr_system.spi_bus)> strip_device(avr_system.spi_bus);
    Lpd8806Buffered1dGraphicsSurface<typeof(strip_device), 161> strip(&strip_device);
    RadiantMuralZigZagBuffered2dTo1dGraphicsSurfaceAdapter<typeof(strip)> surface(&strip);
    GameOfLife<typeof(surface), 24, 7> game_of_life(&surface);

    inline void main_loop() {
        // Enable the overflow interrupt.
        TIMSK1 |= (1 << TOIE1);

        // Start timer at Fcpu/8
        TCCR1B |= (1 << CS11);

        while (1) {
            sleep_enable();
            set_sleep_mode(SLEEP_MODE_PWR_SAVE);
            sei();
            sleep_cpu();
            sleep_disable();
            cli();
            if (redraw) {
                game_of_life.next_frame();
                redraw = 0;
            }
        }
    }

#elif RADIANTMURAL_TARGET_dummy

    #include <alambre/system/sdl/2dgraphics.h>

    WindowedSdl2dGraphicsSurface<24, 7, 10> surface;
    GameOfLife<typeof(surface), 24, 7> game_of_life(&surface);

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
            }
        }
    }

#else
#error No wiring code for this target.
#endif

int main() {

    auto white = surface.get_closest_color(255, 255, 255);

    // Just a dummy pattern to test the wiring.
    // TODO: Remove this and replace it with the real program.
    surface.set_pixel(0, 0, white);
    surface.set_pixel(0, 1, white);
    surface.set_pixel(0, 2, white);
    surface.set_pixel(0, 3, white);
    surface.set_pixel(0, 4, white);
    surface.set_pixel(0, 5, white);
    surface.set_pixel(0, 6, white);

    surface.set_pixel(1, 3, white);

    surface.set_pixel(2, 0, white);
    surface.set_pixel(2, 1, white);
    surface.set_pixel(2, 2, white);
    surface.set_pixel(2, 3, white);
    surface.set_pixel(2, 4, white);
    surface.set_pixel(2, 5, white);
    surface.set_pixel(2, 6, white);

    surface.set_pixel(4, 1, white);
    surface.set_pixel(4, 3, white);
    surface.set_pixel(4, 4, white);
    surface.set_pixel(4, 5, white);
    surface.set_pixel(4, 6, white);

    surface.flip(0, 0, 23, 6);

    game_of_life.set_pixel(3, 1);
    game_of_life.set_pixel(4, 1);
    game_of_life.set_pixel(5, 1);

    game_of_life.set_pixel(12, 4);
    game_of_life.set_pixel(12, 5);
    game_of_life.set_pixel(12, 6);
    game_of_life.set_pixel(11, 6);
    game_of_life.set_pixel(10, 5);

    main_loop();

}
