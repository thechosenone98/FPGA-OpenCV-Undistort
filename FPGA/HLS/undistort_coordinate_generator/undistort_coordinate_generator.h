#include "ap_int.h"
#include "ap_fixed.h"

using coord_in = ap_uint<9>;
// Same precision as OpenCV (Fixed point map)
using coord_out = ap_fixed<15, 10, AP_RND, AP_SAT>;
using coord_out_no_frac = ap_int<10>;
using coeff_precision = ap_fixed<32, 5>;
using inverse_coeff_precision = ap_fixed<32, 0>;
using large_coeff_precision = ap_fixed<32, 11>;
using lin_coeff_precision = ap_ufixed<10, 0, AP_RND, AP_SAT>;
using lin_coeff_tmp_precision = ap_ufixed<11, 1, AP_RND, AP_SAT>;
using small_calc_precision = ap_fixed<24, 9, AP_RND, AP_SAT>;
using medium_calc_precision = ap_fixed<32, 17, AP_RND, AP_SAT>;
using large_calc_precision = ap_fixed<48, 33, AP_RND, AP_SAT>;
using largest_calc_precision = ap_fixed<64, 49, AP_RND, AP_SAT>;
using calc_precision = ap_fixed<48, 32, AP_RND, AP_SAT>; // Was 24, 9
using tmp_calc_precision = ap_fixed<36, 8, AP_RND, AP_SAT>;
using bool_width = ap_uint<1>;

//Define a pair of coordinates coord_out
struct coord_and_coeff{
    coord_out x;
    coord_out y;
    lin_coeff_precision tl_coeff;
    lin_coeff_precision tr_coeff;
    lin_coeff_precision bl_coeff;
    lin_coeff_precision br_coeff;
    // bool_width was_resetted;
};

coord_and_coeff undistort_coordinate_generator(coord_in u, coord_in v,
                                               large_coeff_precision fx_in,
                                               inverse_coeff_precision fx_inverse_in,
                                               large_coeff_precision fy_in,
                                               inverse_coeff_precision fy_inverse_in,
                                               large_coeff_precision cx_in,
                                               large_coeff_precision cy_in,
                                               coeff_precision k1_in,
                                               coeff_precision k2_in,
                                               coeff_precision k3_in,
                                               coeff_precision k4_in,
                                               coeff_precision k5_in,
                                               coeff_precision k6_in,
                                               coeff_precision p1_in,
                                               coeff_precision p2_in,
                                               coeff_precision s1_in,
                                               coeff_precision s2_in,
                                               coeff_precision s3_in,
                                               coeff_precision s4_in);
