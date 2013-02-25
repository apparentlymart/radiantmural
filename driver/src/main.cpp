
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
#define __PROG_TYPES_COMPAT__
    #include <avr/pgmspace.h>
    #include "ffft.h"
    #include "ledmatrix.h"

    int16_t capture[FFT_N];
    complex_t bfly[FFT_N];
    uint16_t spectrum[FFT_N/2];
    volatile uint8_t samplePos = 0;

    volatile uint8_t redraw = 0;

    ISR(TIMER1_OVF_vect) {
        redraw = 1;
    }

    SoftwareSpiMasterOutputBus <typeof(*avr_system.B3), typeof(*avr_system.B5)> spi_bus(avr_system.B3, avr_system.B5);
    Lpd8806Device<typeof(spi_bus)> strip_device(&spi_bus);
    typedef typeof(strip_device) strip_device_type;

    Lpd8806Color palette[] = {
        Lpd8806Color::get_closest(0, 0, 48),
        Lpd8806Color::get_closest(0, 0, 48),
        Lpd8806Color::get_closest(0, 0, 48),
        Lpd8806Color::get_closest(0, 0, 48),
        Lpd8806Color::get_closest(0, 0, 48),
        Lpd8806Color::get_closest(0, 0, 48),
        Lpd8806Color::get_closest(0, 0, 48),
        Lpd8806Color::get_closest(0, 0, 56),
        Lpd8806Color::get_closest(0, 0, 64),
        Lpd8806Color::get_closest(0, 0, 72),
        Lpd8806Color::get_closest(0, 0, 80),
        Lpd8806Color::get_closest(0, 0, 88),
        Lpd8806Color::get_closest(0, 0, 96),
        Lpd8806Color::get_closest(0, 0, 104),
        Lpd8806Color::get_closest(0, 0, 112),
        Lpd8806Color::get_closest(0, 0, 120),
        Lpd8806Color::get_closest(0, 0, 128),
        Lpd8806Color::get_closest(0, 0, 136),
        Lpd8806Color::get_closest(0, 0, 144),
        Lpd8806Color::get_closest(0, 0, 152),
        Lpd8806Color::get_closest(0, 0, 160),
        Lpd8806Color::get_closest(0, 0, 168),
        Lpd8806Color::get_closest(0, 0, 176),
        Lpd8806Color::get_closest(0, 0, 184),
        Lpd8806Color::get_closest(0, 0, 192),
        Lpd8806Color::get_closest(0, 0, 200),
        Lpd8806Color::get_closest(0, 0, 208),
        Lpd8806Color::get_closest(0, 0, 216),
        Lpd8806Color::get_closest(0, 0, 224),
        Lpd8806Color::get_closest(0, 0, 232),
        Lpd8806Color::get_closest(0, 0, 240),
        Lpd8806Color::get_closest(0, 0, 254)
    };
    PaletteBitmap1d<unsigned int, strip_device_type::raw_color, 161, sizeof(palette) / sizeof(palette[0])> bitmap1d(palette);
    Lpd8806Bitmap1dDisplay<typeof(bitmap1d.render_bitmap), strip_device_type, 161> display(&strip_device);
    RadiantMuralZigZagMutableBitmap1dAsBitmap2dAdapter<typeof(bitmap1d)> bitmap(&bitmap1d);
    GameOfLife<24, 9> game_of_life;
    GameOfLifeRenderer<typeof(game_of_life), typeof(bitmap)> renderer(&game_of_life, &bitmap);
    auto white = strip_device.get_closest_color(255, 255, 255);
    auto blue = strip_device.get_closest_color(0, 0, 127);

    ISR(ADC_vect) { // audio-sampling interrupt
        static const int16_t noiseThreshold = 4;
        int16_t sample = ADC; // 0-1023

        capture[samplePos] =
            ((sample > (512 - noiseThreshold)) &&
             (sample < (512 + noiseThreshold))) ? 0 : sample - 512;

        if (++samplePos > FFT_N) ADCSRA &= ~_BV(ADIE); // interrupt off since buffer is full

    }

    inline void main_loop() {

        ADMUX = 0b01000000; // analog channel 0, VREF = AVCC (since we have no other, better reference)
        ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
        ADCSRB = 0; // enable free run mode
        DIDR0 = 1; // turn off digital input
        TIMSK0 = 0; // turn off timer0

        sei();

        while (1) {
            while(ADCSRA & _BV(ADIE)); // wait for sampling to finish -- then ISR will disable the interrupt

            fft_input(capture, bfly);
            samplePos = 0;
            ADCSRA |= _BV(ADIE); // re-enable the interrupt
            fft_execute(bfly);
            fft_output(bfly, spectrum);

            for (int x = 0; x < 22; x++) {
                int ofs = (x + 4) * 2;
                int size = ((spectrum[ofs] + spectrum[ofs+1])) - 5;

                if (size > 3) {
                    game_of_life.set_pixel(x + 1, 6);
                    game_of_life.set_pixel(x + 1, 5);
                    game_of_life.set_pixel(x + 1, 4);
                    game_of_life.set_pixel(x, 5);
                    game_of_life.set_pixel(x + 2, 4);
                }
            }

            //_delay_ms(150);
            game_of_life.next_frame();
            renderer.update();
            display.update(&bitmap1d.render_bitmap);
            //game_of_life.set_pixel(13, 6);
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

    main_loop();

}
