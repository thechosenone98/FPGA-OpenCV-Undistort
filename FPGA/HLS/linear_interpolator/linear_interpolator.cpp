#include "linear_interpolator.h"

output_pixel_precision linear_interpolator(input_pixel_precision pixel_tl,
                                           input_pixel_precision pixel_tr,
                                           input_pixel_precision pixel_bl,
                                           input_pixel_precision pixel_br,
                                           input_coeff_precision tl, 
                                           input_coeff_precision tr, 
                                           input_coeff_precision bl, 
                                           input_coeff_precision br)
{
    #pragma HLS PIPELINE II=1

    static calculation_precision pixel_out;
    #pragma HLS RESOURCE variable=pixel_out core=AddSub_DSP
    static calculation_precision pixel_tl_conv;
    static calculation_precision pixel_tr_conv;
    static calculation_precision pixel_bl_conv;
    static calculation_precision pixel_br_conv;
    static calculation_precision pixel_tl_mult_coeff;
    static calculation_precision pixel_tr_mult_coeff;
    static calculation_precision pixel_bl_mult_coeff;
    static calculation_precision pixel_br_mult_coeff;

    pixel_tl_conv(23, 16) = pixel_tl(7, 0);
    pixel_tr_conv(23, 16) = pixel_tr(7, 0);
    pixel_bl_conv(23, 16) = pixel_bl(7, 0);
    pixel_br_conv(23, 16) = pixel_br(7, 0);

    pixel_tl_mult_coeff = pixel_tl_conv * tl;
    pixel_tr_mult_coeff = pixel_tr_conv * tr;
    pixel_bl_mult_coeff = pixel_bl_conv * bl;
    pixel_br_mult_coeff = pixel_br_conv * br;

    pixel_out = pixel_tl_mult_coeff + pixel_tr_mult_coeff + pixel_bl_mult_coeff + pixel_br_mult_coeff;

    return hls::round(pixel_out);
}
