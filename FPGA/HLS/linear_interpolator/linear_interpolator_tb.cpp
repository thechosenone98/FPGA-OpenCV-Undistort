#include "linear_interpolator.h"

int main()
{
    input_pixel_precision pixel_tl = 1;
    input_pixel_precision pixel_tr = 2;
    input_pixel_precision pixel_bl = 3;
    input_pixel_precision pixel_br = 4;
    input_coeff_precision tl = 0.1;
    input_coeff_precision tr = 0.2;
    input_coeff_precision bl = 0.3;
    input_coeff_precision br = 0.4;
    output_pixel_precision pixel_out = linear_interpolator(pixel_tl, pixel_tr, pixel_bl, pixel_br, tl, tr, bl, br);

    printf("pixel_out = %f, expected = %f, error = %f", (double)pixel_out, 3, (double)pixel_out - 3);

    return 0;
}
