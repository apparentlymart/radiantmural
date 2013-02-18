#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

template <uint8_t WIDTH, uint8_t HEIGHT>
class GameOfLife {

    uint8_t world[2][HEIGHT][WIDTH];
    uint8_t current_world;
    uint8_t next_world;
    typedef uint8_t coord_type;

  public:

    GameOfLife() {
        this->current_world = 0;
        this->next_world = 1;
        memset(&world, 0, sizeof(world));
    }

    void next_frame() {

        // Decide who lives and who dies
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                int above = (y + HEIGHT - 1) % HEIGHT;
                int below = (y + 1) % HEIGHT;
                int left = (x + WIDTH - 1) % WIDTH;
                int right = (x + 1) % WIDTH;

                int neighbors = (
                    get_pixel(left, above) +
                    get_pixel(x, above) +
                    get_pixel(right, above) +
                    get_pixel(left, y) +
                    get_pixel(right, y) +
                    get_pixel(left, below) +
                    get_pixel(x, below) +
                    get_pixel(right, below)
                );

                if (neighbors == 3) {
                    world[next_world][y][x] = 1;
                }
                else if (neighbors < 2 || neighbors > 3) {
                    world[next_world][y][x] = 0;
                }
                else {
                    world[next_world][y][x] = get_pixel(x, y);
                }
            }
        }

        next_world = next_world ^ current_world;
        current_world = next_world ^ current_world;
        next_world = next_world ^ current_world;
    }

    inline uint8_t get_pixel(coord_type x, coord_type y) {
        return world[current_world][y][x];
    }

    inline void set_pixel(coord_type x, coord_type y) {
        world[current_world][y][x] = 1;
    }

    inline void clear_pixel(coord_type x, coord_type y) {
        world[current_world][y][x] = 0;
    }

};

#endif
