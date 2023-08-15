#include "undistort_coordinate_generator.h"

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
                                               coeff_precision s4_in){
    #pragma HLS pipeline II=1
    #pragma HLS interface s_axilite port=fx_in
    #pragma HLS interface s_axilite port=fx_inverse_in
    #pragma HLS interface s_axilite port=fy_in
    #pragma HLS interface s_axilite port=fy_inverse_in
    #pragma HLS interface s_axilite port=cx_in
    #pragma HLS interface s_axilite port=cy_in
    #pragma HLS interface s_axilite port=k1_in
    #pragma HLS interface s_axilite port=k2_in
    #pragma HLS interface s_axilite port=k3_in
    #pragma HLS interface s_axilite port=k4_in
    #pragma HLS interface s_axilite port=k5_in
    #pragma HLS interface s_axilite port=k6_in
    #pragma HLS interface s_axilite port=p1_in
    #pragma HLS interface s_axilite port=p2_in
    #pragma HLS interface s_axilite port=s1_in
    #pragma HLS interface s_axilite port=s2_in
    #pragma HLS interface s_axilite port=s3_in
    #pragma HLS interface s_axilite port=s4_in


    static large_coeff_precision fx = 0;
    static coeff_precision fx_inverse = 0;
    static large_coeff_precision fy = 0;
    static coeff_precision fy_inverse = 0;
    static large_coeff_precision cx = 0;
    static large_coeff_precision cy = 0;
    static coeff_precision k1 = 0;
    static coeff_precision k2 = 0;
    static coeff_precision k3 = 0;
    static coeff_precision k4 = 0;
    static coeff_precision k5 = 0;
    static coeff_precision k6 = 0;
    static coeff_precision p1 = 0;
    static coeff_precision p2 = 0;
    static coeff_precision s1 = 0;
    static coeff_precision s2 = 0;
    static coeff_precision s3 = 0;
    static coeff_precision s4 = 0;


	fx = fx_in;
	fx_inverse = fx_inverse_in;
	fy = fy_in;
	fy_inverse = fy_inverse_in;
	cx = cx_in;
	cy = cy_in;
	k1 = k1_in;
	k2 = k2_in;
	k3 = k3_in;
	k4 = k4_in;
	k5 = k5_in;
	k6 = k6_in;
	p1 = p1_in;
	p2 = p2_in;
	s1 = s1_in;
	s2 = s2_in;
	s3 = s3_in;
	s4 = s4_in;

    // All required variables
    static small_calc_precision x;
    static small_calc_precision y;
    static medium_calc_precision r2;
    static large_calc_precision r4;
    static largest_calc_precision r6;
    static largest_calc_precision div_top;
    static largest_calc_precision div_bottom;
    static largest_calc_precision division;
    static largest_calc_precision x_tmp;
    static largest_calc_precision y_tmp;
    static largest_calc_precision x_tmp_2;
    static largest_calc_precision y_tmp_2;
    static largest_calc_precision x_tmp_3;
    static largest_calc_precision y_tmp_3;
    static coord_out x_out;
    static coord_out y_out;
    static bool_width no_frac;
    static lin_coeff_precision tl_coeff;
    static lin_coeff_precision tr_coeff;
    static lin_coeff_precision bl_coeff;
    static lin_coeff_precision br_coeff;
    static coord_out x_frac;
    static coord_out y_frac;
    static lin_coeff_tmp_precision one_minus_xfrac;
    static lin_coeff_tmp_precision one_minus_yfrac;



    // x and y are between -0.5 and 0.5
    // x←(u−c′x)/f′x
    x = (u - cx) * fx_inverse;
    // y←(v−c′y)/f′y
    y = (v - cy) * fy_inverse;
    //r^2←x′2+y′2
    r2 = x*x + y*y;
    //r^4←r^2×r^2
    r4 = r2*r2;
    //r^6←r^4×r^2
    r6 = r2*r2*r2;

    //1+k1r^2+k2r^4+k3r^6
    //-------------------
    //1+k4r^2+k5r^4+k6r^6
    div_top = static_cast<largest_calc_precision>(1) + static_cast<largest_calc_precision>(static_cast<medium_calc_precision>(k1)*r2) + static_cast<largest_calc_precision>(static_cast<large_calc_precision>(k2)*r4) + static_cast<largest_calc_precision>(static_cast<largest_calc_precision>(k3)*r6);
    div_bottom = static_cast<largest_calc_precision>(1) + static_cast<largest_calc_precision>(static_cast<medium_calc_precision>(k4)*r2) + static_cast<largest_calc_precision>(static_cast<large_calc_precision>(k5)*r4) + static_cast<largest_calc_precision>(static_cast<largest_calc_precision>(k6)*r6);
    division = div_top/div_bottom;
    //x←x′(1+k1r2+k2r4+k3r6)/(1+k4r2+k5r4+k6r6)
    x_tmp = static_cast<tmp_calc_precision>(x) * division;
    //y←y′(1+k1r2+k2r4+k3r6)/(1+k4r2+k5r4+k6r6)
    y_tmp = static_cast<tmp_calc_precision>(y) * division;

    //x←x+2p1xy+p2(r2+2x^2)
    x_tmp_2 = x_tmp + 2*p1*x*y + p2*(r2 + 2*x*x);
    //y←y+p1(r2+2y^2)+2p2xy
    y_tmp_2 = y_tmp + p1*(r2 + 2*y*y) + 2*p2*x*y;

    //x←x+s1r^2+s2r^4
    x_tmp_3 = x_tmp_2 + s1*r2 + s2*r4;
    //y←y+s3r^2+s4r^4
    y_tmp_3 = y_tmp_2 + s3*r2 + s4*r4;

    //u←fx×x+cx
    x_out = fx*x_tmp_3 + cx;
    //v←fy×y+cy
    y_out = fy*y_tmp_3 + cy;
    // Linear interpolation coefficients
    x_frac(4,0) = x_out.range(4, 0);
    y_frac(4,0) = y_out.range(4, 0);
    no_frac = x_frac.range(4,0) == 0 && y_frac.range(4, 0) == 0;
    one_minus_xfrac = static_cast<coord_out>(1) - x_frac;
    one_minus_yfrac = static_cast<coord_out>(1) - y_frac;
    tl_coeff = no_frac ? static_cast<lin_coeff_precision>(1.00) : static_cast<lin_coeff_precision>(one_minus_xfrac * one_minus_yfrac);
    tr_coeff = no_frac ? static_cast<lin_coeff_precision>(0.00) : static_cast<lin_coeff_precision>(x_frac * one_minus_yfrac);
    bl_coeff = no_frac ? static_cast<lin_coeff_precision>(0.00) : static_cast<lin_coeff_precision>(one_minus_xfrac * y_frac);
    br_coeff = no_frac ? static_cast<lin_coeff_precision>(0.00) : static_cast<lin_coeff_precision>(x_frac * y_frac);
    return {x_out, y_out, tl_coeff, tr_coeff, bl_coeff, br_coeff};
}
