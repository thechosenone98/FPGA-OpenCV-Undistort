#include "bram_address_decoder.h"

pixel_addresses bram_address_decoder(coord_in_with_frac x, coord_in_with_frac y) {
	#pragma HLS PIPELINE II=1

    static coord_in x_int;
    static coord_in y_int;
	static pixel_index_width mult;
    static pixel_index_width pixel_index;
    static ap_uint<1> x_and_y_no_frac;
    static pixel_index_width pixel_index_tl;
    static pixel_index_width pixel_index_tr;
    static pixel_index_width pixel_index_bl;
    static pixel_index_width pixel_index_br;
    static bram_address_width base_address_tmp;
    static bram_address_width base_address;
    static bram_address_width num_rows;
    static bool_width negative_row;
    static bram_address_width num_pixel_groups;
    static bram_address_width pixel_address_oo;
    static bram_address_width pixel_address_oe;
    static bram_address_width pixel_address_eo;
    static bram_address_width pixel_address_ee;
    static pixel_row_index_width pixel_row_index_tl;
    static pixel_row_index_width pixel_row_index_tr;
    static pixel_row_index_width pixel_row_index_bl;
    static pixel_row_index_width pixel_row_index_br;
    static bool_width odd_pixel;
    static bool_width odd_row;

    static pixel_addresses pixel_addresses_out;

    x_int = static_cast<coord_in>(x);
    y_int = y < 0 ? static_cast<coord_in>(-1) : static_cast<coord_in>(y);
    // This gets optimized as an addition of shift (input << 8) + (input << 6) same as (input * 320) 🤯
    mult = y_int * static_cast<coord_in>(IMAGE_WIDTH);
    pixel_index = mult + x_int;

    x_and_y_no_frac = x.range(4, 0) == 0 && y.range(4, 0) == 0;

    pixel_index_tl = static_cast<pixel_index_width>(pixel_index);
    pixel_index_tr = x_and_y_no_frac ? pixel_index_tl : static_cast<pixel_index_width>(pixel_index + 1);
    pixel_index_bl = x_and_y_no_frac ? pixel_index_tl : static_cast<pixel_index_width>(pixel_index + IMAGE_WIDTH);
    pixel_index_br = x_and_y_no_frac ? pixel_index_tl : static_cast<pixel_index_width>(pixel_index + IMAGE_WIDTH + 1);

    // Divide by 8 to get the block of 8 pixel that contains that pixel
    base_address_tmp = static_cast<bram_address_width>(pixel_index_tl >> 3);
    num_rows = base_address_tmp / 40;
    num_pixel_groups = base_address_tmp % 40;
    base_address = (base_address_tmp >> 1);
    negative_row = y < 0 ? 1 : 0;

    // Decode address to read from 4 BRAMs that store odd/odd, odd/even, even/odd, even/even pixel groups
    // For visual representation, this is the arrangement the variable represent for odd/odd pixel group:
    // oo oe
    // eo ee
    // For odd/even pixel group:
    // oe oo
    // ee eo
    // For even/odd pixel group:
    // eo ee
    // oo oe
    // For even/even pixel group:
    // ee eo
    // oe oo
    odd_pixel = (x < 0) ? 0 : (base_address_tmp % 2 == 0);
    odd_row = (y < 0) ? 0 : (num_rows % 2 == 0);
    if (odd_row){
        if (odd_pixel){
            pixel_address_oo = (num_rows >> 1) * 20 + (num_pixel_groups >> 1) + static_cast<bram_address_width>(base_address_tmp[0]);
            pixel_address_oe = pixel_address_oo;
            pixel_address_eo = pixel_address_oo;
            pixel_address_ee = pixel_address_oo;
        } else {
            pixel_address_oo = (num_rows >> 1) * 20 + (num_pixel_groups >> 1) + static_cast<bram_address_width>(base_address_tmp[0]);
            pixel_address_oe = pixel_address_oo - 1;
            pixel_address_eo = pixel_address_oo;
            pixel_address_ee = pixel_address_oo - 1;
        }
    } else {
        if (odd_pixel){
            pixel_address_eo = (num_rows >> 1) * 20 + (num_pixel_groups >> 1) + static_cast<bram_address_width>(base_address_tmp[0]);
            pixel_address_ee = pixel_address_eo;
            pixel_address_oo = pixel_address_eo + 20;
            pixel_address_oe = pixel_address_eo + 20;
        } else {
            pixel_address_eo = (num_rows >> 1) * 20 + (num_pixel_groups >> 1) + static_cast<bram_address_width>(base_address_tmp[0]);
            pixel_address_ee = pixel_address_eo - 1;
            pixel_address_oo = pixel_address_eo + 20;
            pixel_address_oe = pixel_address_eo + 20 - 1;
        }
    }

    pixel_row_index_tl = x < 0 ? static_cast<pixel_row_index_width>(7) : static_cast<pixel_row_index_width>(pixel_index_tl);
    pixel_row_index_tr = x < 0 ? static_cast<pixel_row_index_width>(0) : (x_and_y_no_frac ? pixel_row_index_tl : static_cast<pixel_row_index_width>(pixel_index_tr));
    pixel_row_index_bl = x < 0 ? static_cast<pixel_row_index_width>(7) : (x_and_y_no_frac ? pixel_row_index_tl : static_cast<pixel_row_index_width>(pixel_index_bl));
    pixel_row_index_br = x < 0 ? static_cast<pixel_row_index_width>(0) : (x_and_y_no_frac ? pixel_row_index_tl : static_cast<pixel_row_index_width>(pixel_index_br));


    if(x >= IMAGE_WIDTH - 8 && x < IMAGE_WIDTH){
    	// Last group of pixel per row is an even group so the odd column to it's right is out of range
        pixel_address_oo = 0x3FFF;
        pixel_address_eo = 0x3FFF;
    } else if (x >= IMAGE_WIDTH){
        pixel_address_oo = 0x3FFF;
        pixel_address_oe = 0x3FFF;
        pixel_address_eo = 0x3FFF;
        pixel_address_ee = 0x3FFF;
    } else if (x < 0.0 && x > -1.0){ // TODO: TEST THAT WHEN YOU COME BACK (was -1.0 before)
    	// First column is odd so the even column to it's left is out of range
        pixel_address_oe = 0x3FFF;
        pixel_address_ee = 0x3FFF;
    } else if (x <= -1.0){ // TODO: SAME HERE (-1.0 before)
        pixel_address_oo = 0x3FFF;
        pixel_address_oe = 0x3FFF;
        pixel_address_eo = 0x3FFF;
        pixel_address_ee = 0x3FFF;
    }

    if(y >= IMAGE_HEIGHT - 1 && y < IMAGE_HEIGHT){
    	// Last row of image is even so the odd row under is out of range
        pixel_address_oo = 0x3FFF;
        pixel_address_oe = 0x3FFF;
    } else if (y >= IMAGE_HEIGHT){
        pixel_address_oo = 0x3FFF;
        pixel_address_oe = 0x3FFF;
        pixel_address_eo = 0x3FFF;
        pixel_address_ee = 0x3FFF;
    } else if (y < 0.0 && y > -1.0){
    	// First row is odd so the even row above is out of range
        pixel_address_eo = 0x3FFF;
        pixel_address_ee = 0x3FFF;
    } else if (y <= -1.0){
        pixel_address_oo = 0x3FFF;
        pixel_address_oe = 0x3FFF;
        pixel_address_eo = 0x3FFF;
        pixel_address_ee = 0x3FFF;
    }

    pixel_addresses_out.pixel_addr_oo.address = pixel_address_oo;
    pixel_addresses_out.pixel_addr_oo.row_index = pixel_row_index_tl;
    pixel_addresses_out.pixel_addr_oe.address = pixel_address_oe;
    pixel_addresses_out.pixel_addr_oe.row_index = pixel_row_index_tr;
    pixel_addresses_out.pixel_addr_eo.address = pixel_address_eo;
    pixel_addresses_out.pixel_addr_eo.row_index = pixel_row_index_bl;
    pixel_addresses_out.pixel_addr_ee.address = pixel_address_ee;
    pixel_addresses_out.pixel_addr_ee.row_index = pixel_row_index_br;
    // 1 = odd, 0 = even (the logic is that the first row of the image is odd, so is the first pixel)
    // Also making anything negative even (for edge cases this works and the rest of the negative we don't care about anyway)
    pixel_addresses_out.odd_row = odd_row;
    pixel_addresses_out.odd_pixel = odd_pixel;

    return pixel_addresses_out;
}
