#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

template <class BITMAP_TYPE, class DISPLAY_TYPE, unsigned int WIDTH, unsigned int HEIGHT>
class GameOfLife {

    BITMAP_TYPE *bitmap;
    DISPLAY_TYPE *display;
    uint8_t world[2][HEIGHT][WIDTH];
    uint8_t current_world;
    uint8_t next_world;
    typename BITMAP_TYPE::color_type white;
    typename BITMAP_TYPE::color_type black;
    typedef typename BITMAP_TYPE::coord_type coord_type;

  public:

    GameOfLife(BITMAP_TYPE *bitmap, DISPLAY_TYPE *display) {
        this->bitmap = bitmap;
        this->display = display;
        this->white = display->get_closest_color(255, 255, 255);
        this->black = display->get_closest_color(0, 0, 0);
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

        // Update the visible bitmap
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                bitmap->set_pixel(x, y, get_pixel(x, y) ? white : black);
            }
        }
        display->update(bitmap);
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