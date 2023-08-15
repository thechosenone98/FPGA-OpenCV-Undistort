#include "ap_fixed.h"
#include "ap_int.h"
#include "hls_math.h"

using calculation_precision = ap_ufixed<24, 8>;
using output_pixel_precision = ap_uint<8>;
using input_coeff_precision = ap_ufixed<10, 0>;
using input_pixel_precision = ap_uint<8>;

output_pixel_precision linear_interpolator(input_pixel_precision pixel_tl, 
                                           input_pixel_precision pixel_tr,
                                           input_pixel_precision pixel_bl,
                                           input_pixel_precision pixel_br,
                                           input_coeff_precision tl, 
                                           input_coeff_precision tr, 
                                           input_coeff_precision bl, 
                                           input_coeff_precision br);
