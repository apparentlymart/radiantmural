
// Target-specific wiring code
#ifdef RADIANTMURAL_TARGET_arduino_uno

    #include <alambre/system/avr.h>

    // TODO: Define "surface" for real (this abstract class won't link)
    #include <alambre/capability/2dgraphics.h>
    AbstractBuffered2dGraphicsSurface<24, 7, uint32_t> surface;

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

    // Just a dummy pattern to test the wiring.
    // TODO: Remove this and replace it with the real program.
    surface.set_pixel(0, 0, 0xffffffff);
    surface.set_pixel(0, 1, 0xffffffff);
    surface.set_pixel(0, 2, 0xffffffff);
    surface.set_pixel(0, 3, 0xffffffff);
    surface.set_pixel(0, 4, 0xffffffff);
    surface.set_pixel(0, 5, 0xffffffff);
    surface.set_pixel(0, 6, 0xffffffff);

    surface.set_pixel(1, 3, 0xffffffff);

    surface.set_pixel(2, 0, 0xffffffff);
    surface.set_pixel(2, 1, 0xffffffff);
    surface.set_pixel(2, 2, 0xffffffff);
    surface.set_pixel(2, 3, 0xffffffff);
    surface.set_pixel(2, 4, 0xffffffff);
    surface.set_pixel(2, 5, 0xffffffff);
    surface.set_pixel(2, 6, 0xffffffff);

    surface.set_pixel(4, 1, 0xffffffff);
    surface.set_pixel(4, 3, 0xffffffff);
    surface.set_pixel(4, 4, 0xffffffff);
    surface.set_pixel(4, 5, 0xffffffff);
    surface.set_pixel(4, 6, 0xffffffff);

    surface.flip(0, 0, 23, 6);

    main_loop();

}
