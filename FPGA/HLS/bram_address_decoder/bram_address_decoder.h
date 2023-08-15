#include "ap_fixed.h"
#include "ap_int.h"

#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

#define BRAM_ADDRESS_WIDTH 14
#define SMALL_BRAM_ADDRESS_WIDTH 12
#define PIXEL_PER_ADDRESS 8
#define BITS_PER_PIXEL 8
#define PIXELS_PER_BRAM_ADDRESS (PIXEL_PER_ADDRESS * BITS_PER_PIXEL)

using coord_in_with_frac = ap_fixed<15, 10, AP_RND, AP_SAT>; // Precision from previous module (comes from OpenCV precision)
using bram_address_width = ap_int<BRAM_ADDRESS_WIDTH + 1>;
using small_bram_address_width = ap_uint<SMALL_BRAM_ADDRESS_WIDTH>;
using pixel_index_width = ap_int<18>;
using pixel_row_index_width = ap_uint<3>;
using coord_in = ap_int<10>; // For storing the integer part of the coordinates
using bool_width = ap_uint<1>;

struct pixel_address {
    small_bram_address_width address;
    pixel_row_index_width row_index;
};

struct pixel_addresses {
    pixel_address pixel_addr_oo;
    pixel_address pixel_addr_oe;
    pixel_address pixel_addr_eo;
    pixel_address pixel_addr_ee;
    bool_width odd_pixel;
    bool_width odd_row;
};

pixel_addresses bram_address_decoder(coord_in_with_frac x, coord_in_with_frac y);
