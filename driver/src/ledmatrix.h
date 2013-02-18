#ifndef RADIANTMURAL_LEDMATRIX
#define RADIANTMURAL_LEDMATRIX

#include <alambre/capability/1dgraphics.h>

/**
 * Mapping of 2D bitmap onto a bizarre chopped up, zig-zagging LED strip.
 *
 * This implementation of AbstractBuffered2dTo1dGraphicsSurfaceAdapter
 * maps a 24*7 2D surface onto a 1D LED strip cut into zig-zagging
 * smaller strips of irregular lengths.
 *
 * The top and bottom rows are shorter than the others at 20 pixels each,
 * with those 20 pixels centered. This means that the two pixels at either
 * end of these rows are not addressable.
 *
 * The eight unaddressable coordinates caused by the display's irregular
 * shape are mapped to an additional fictitious pixel at the end of the
 * strip, since the projection function is required to map all coordinates
 * somewhere. This means the length of the 1D surface must be at least 161
 * pixels or the caller must be careful to never access these pixels.
 *
 * The 1D coordinate system starts at the bottom right moving left, and then
 * on the row above counts in the opposite direction, and it continues
 * switching in this zig-zag pattern until it reaches the top left.
 */
template <class BITMAP1D_TYPE>
class RadiantMuralZigZagMutableBitmap1dAsBitmap2dAdapter : public AbstractMutableBitmap1dAsBitmap2dAdapter<BITMAP1D_TYPE, RadiantMuralZigZagMutableBitmap1dAsBitmap2dAdapter<BITMAP1D_TYPE> > {

  private:
    typedef AbstractMutableBitmap1dAsBitmap2dAdapter<BITMAP1D_TYPE, RadiantMuralZigZagMutableBitmap1dAsBitmap2dAdapter<BITMAP1D_TYPE> > base_type;

  public:

    // Forcefully "inherit" the typedefs from our parent, because
    // typedefs don't inherit automatically in C++.
    typedef typename base_type::index_type index_type;
    typedef typename base_type::coord_type coord_type;
    typedef typename base_type::color_type color_type;

    RadiantMuralZigZagMutableBitmap1dAsBitmap2dAdapter(BITMAP1D_TYPE *bitmap1d) {
        this->bitmap1d = bitmap1d;
    }

    inline index_type get_width() {
        return 24;
    }

    inline index_type get_height() {
        return 7;
    }

    inline index_type map_coordinates(coord_type x, coord_type y) {
        // Our y axis is upside-down compared to what users of this
        // interface expect, so we invert the y coord.
        y = 6 - y;

        if (y == 0) {
            if (x < 2 || x > 21) {
                // One of the unaddressable pixels.
                return 160;
            }
            else {
                return 21 - x;
            }
        }
        else if (y == 6) {
            if (x < 2 || x > 21) {
                // One of the unaddressable pixels.
                return 160;
            }
            else {
                return 161 - x;
            }
        }
        else {
            if (y & 1) {
                // odd row, so left-to-right
                return 20 + (y - 1) * 24 + x;
            }
            else {
                // even row, so right-to-left
                return 19 + y * 24 - x;
            }
        }
    }

};

#endif
