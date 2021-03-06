#ifndef GAMEOFLIFERENDERER_H
#define GAMEOFLIFERENDERER_H


template <class GAME_TYPE, class BITMAP_TYPE>
class GameOfLifeRenderer {

    GAME_TYPE *game;
    BITMAP_TYPE *bitmap;

  public:

    GameOfLifeRenderer(GAME_TYPE *game, BITMAP_TYPE *bitmap) : game(game), bitmap(bitmap) {}

    void update() {
        int height = bitmap->get_height();
        int width = bitmap->get_width();
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint8_t set = game->get_pixel(x, y);
                if (set) {
                    bitmap->set_pixel(x, y, 31);
                }
                else {
                    // Decay the pixel color until we reach black.
                    uint8_t current = bitmap->get_pixel(x, y);
                    if (current & 0b00011111) {
                        bitmap->set_pixel(x, y, current - 1);
                    }
                }
            }
        }
    }

};

#endif
