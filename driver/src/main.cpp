
// Target-specific wiring code
#ifdef RADIANTMURAL_TARGET_arduino_uno

    #include <alambre/system/avr.h>
    #include <alambre/capability/1dgraphics.h>
    #include <alambre/device/lpd8806.h>
    #include "ledmatrix.h"

    Lpd8806Device<typeof(*avr_system.spi_bus)> strip_device(avr_system.spi_bus);
    Lpd8806Buffered1dGraphicsSurface<typeof(strip_device), 161> strip(&strip_device);
    RadiantMuralZigZagBuffered2dTo1dGraphicsSurfaceAdapter<typeof(strip)> surface(&strip);

    inline void main_loop() {
        // TODO: Put the AVR to sleep, rather than busy-looping
        while (1) {}
    }

#elif RADIANTMURAL_TARGET_dummy

    #include <alambre/system/sdl/2dgraphics.h>

    WindowedSdl2dGraphicsSurface<24, 7, 10> surface;

    inline void main_loop() {
        SDL_Event event;

        while (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
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

    main_loop();

}
