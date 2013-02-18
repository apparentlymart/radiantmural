
#include "gameoflife.h"
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
//Lpd8806Device<typeof(*avr_system.spi_bus)> strip_device(avr_system.spi_bus);
    typedef typeof(strip_device) strip_device_type;

    Bitmap1d<unsigned int, strip_device_type::raw_color, 161> bitmap1d;
    Lpd8806Bitmap1dDisplay<typeof(bitmap1d), strip_device_type, 161> display(&strip_device);
    RadiantMuralZigZagMutableBitmap1dAsBitmap2dAdapter<typeof(bitmap1d)> bitmap(&bitmap1d);
    GameOfLife<typeof(bitmap), typeof(display), 24, 7> game_of_life(&bitmap, &display);
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
            _delay_ms(1000);
            if (redraw) {
                game_of_life.next_frame();
                display.update(&bitmap1d);
                redraw = 0;
            }
        }
    }

#elif RADIANTMURAL_TARGET_dummy

    #include <alambre/capability/2dgraphics.h>
    #include <alambre/system/sdl/2dgraphics.h>

    Bitmap2d<Uint32, Uint32, 24, 7> bitmap;

    WindowedSdlBitmap2dDisplay<typeof(bitmap), 24, 7, 10> display;
    GameOfLife<typeof(bitmap), typeof(display), 24, 7> game_of_life(&bitmap, &display);
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
                display.update(&bitmap);
            }
        }
    }

#else
#error No wiring code for this target.
#endif

int main() {

    uint8_t bri = 255;
    int8_t bridir = -1;
    while (1) {
        int lasty = 6;
        int lastx = 23;
        for (int y = 6; y >= 0; y--) {
            for (int x = 23; x >= 0; x--) {
                bitmap.set_pixel(lastx, lasty, strip_device.get_closest_color(bri, 255 - bri, 0));
                bitmap.set_pixel(x, y, white);
                display.update(&bitmap1d);
                _delay_ms(50);
                lastx = x;
                lasty = y;
                bri += bridir;
                if (bri == 0) bridir = 1;
                if (bri == 255) bridir = -1;
            }
        }
    }

    bitmap.set_pixel(0, 0, white);
    bitmap.set_pixel(1, 0, white);
    bitmap.set_pixel(23, 5, white);

    // Just a dummy pattern to test the wiring.
    // TODO: Remove this and replace it with the real program.
    bitmap.set_pixel(0, 0, white);
    bitmap.set_pixel(0, 1, white);
    bitmap.set_pixel(0, 2, white);
    bitmap.set_pixel(0, 3, white);
    bitmap.set_pixel(0, 4, white);
    bitmap.set_pixel(0, 5, white);
    bitmap.set_pixel(0, 6, white);

    bitmap.set_pixel(1, 3, white);

    bitmap.set_pixel(2, 0, white);
    bitmap.set_pixel(2, 1, white);
    bitmap.set_pixel(2, 2, white);
    bitmap.set_pixel(2, 3, white);
    bitmap.set_pixel(2, 4, white);
    bitmap.set_pixel(2, 5, white);
    bitmap.set_pixel(2, 6, white);

    bitmap.set_pixel(4, 1, white);
    bitmap.set_pixel(4, 3, white);
    bitmap.set_pixel(4, 4, white);
    bitmap.set_pixel(4, 5, white);
    bitmap.set_pixel(4, 6, white);

#ifdef RADIANTMURAL_TARGET_arduino_uno
    display.update(&bitmap1d);
#else
    display.update(&bitmap);
#endif

    while(1) {}

    /*SPCR = 0;
    avr_system.B5->set_direction(IGpioPin::OUTPUT);

    while (1) {
        avr_system.B5->set();
        _delay_ms(1000);
        avr_system.B5->clear();
        _delay_ms(1000);
    }

    return 0;*/

    //while(1) { };

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
