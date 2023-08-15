#include "bram_address_decoder.h"

int main() {
    pixel_addresses pixel_addresses_out;
    pixel_addresses_out = bram_address_decoder(0.0, -0.5);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ODD/EVEN: %d\n", 0.0, -0.5, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row);
    printf("ADDR , ROW\n");
    for (int i = 0; i < 250; i++) {
        for (int j = 0; j < 350; j++) {
            pixel_addresses_out = bram_address_decoder(j, i);
            printf("(%d, %d) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d\n", j, i, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel);
//            pixel_addresses_out = bram_address_decoder(j + 0.5, i + 0.5);
//            printf("(%f, %f) -> TL: %d, %d | TR: %d, %d | BL: %d, %d | BR: %d, %d\n", i + 0.5, j + 0.5, pixel_addresses_out.pixel_tl.address, pixel_addresses_out.pixel_tl.row_index, pixel_addresses_out.pixel_tr.address, pixel_addresses_out.pixel_tr.row_index, pixel_addresses_out.pixel_bl.address, pixel_addresses_out.pixel_bl.row_index, pixel_addresses_out.pixel_br.address, pixel_addresses_out.pixel_br.row_index);
        }
    }
    // Edge cases test
    pixel_addresses_out = bram_address_decoder(312.0, 238.0);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d\n", 312.0, 238.0, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel);
    pixel_addresses_out = bram_address_decoder(-0.5, 0.0);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d\n", -0.5, 0.0, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel);
    pixel_addresses_out = bram_address_decoder(0.0, -0.5);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d\n", 0.0, -0.5, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel);
    pixel_addresses_out = bram_address_decoder(-0.5, -0.5);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d\n", -0.5, -0.5, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel);
    pixel_addresses_out = bram_address_decoder(-1.0, 0);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d\n", -1.0, 0.0, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel);
    pixel_addresses_out = bram_address_decoder(-0.0, -1.0);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d\n", 0.0, -1.0, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel);
    pixel_addresses_out = bram_address_decoder(-1.0, -1.0);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d\n", -1.0, -1.0, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel);
    pixel_addresses_out = bram_address_decoder(-0.5, 100.0);
    printf("(%f, %f) -> OO: %d | OE: %d | EO: %d | EE: %d ROW(O/E): %d PIXEL(O/E): %d ROW INDEX: %d\n", -0.5, 100.0, pixel_addresses_out.pixel_addr_oo.address, pixel_addresses_out.pixel_addr_oe.address, pixel_addresses_out.pixel_addr_eo.address, pixel_addresses_out.pixel_addr_ee.address, pixel_addresses_out.odd_row, pixel_addresses_out.odd_pixel, pixel_addresses_out.pixel_addr_oo.row_index);
    return 0;
}
