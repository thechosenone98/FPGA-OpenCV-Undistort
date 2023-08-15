#include "undistort_coordinate_generator.h"
#include <math.h>
// template <int N, int Q>
// double fixed_to_double(ap_fixed<N, Q> input, int nb_fractionnal_fixed_bits)
// {
// 	double tmp = (double)input;
// 	double divider = (double)(1 << nb_fractionnal_fixed_bits);
// 	double result = tmp / divider;
//     return tmp/divider;
// }

std::pair<double, double> undistort_coordinates_gold(int x, int y, double fx, double fy, double cx, double cy, double k1, double k2, double k3, double k4, double k5, double k6, double p1, double p2, double s1, double s2, double s3, double s4){
    double x_double = (double)x;
    double y_double = (double)y;
    double x_prime = (x_double - cx) / fx;
    double y_prime = (y_double - cy) / fy;
    double r2 = x_prime * x_prime + y_prime * y_prime;
    double r4 = r2 * r2;
    double r6 = r4 * r2;
    double divisor_top = 1 + k1 * r2 + k2 * r4 + k3 * r6;
    double divisor_bottom = 1 + k4 * r2 + k5 * r4 + k6 * r6;
    double x_tmp = x_prime * divisor_top/divisor_bottom;
    double y_tmp = y_prime * divisor_top/divisor_bottom;
    double x_tmp_2 = x_tmp + 2*p1*x_tmp*y_tmp + p2*(r2 + 2*x_tmp*x_tmp);
    double y_tmp_2 = y_tmp + p1*(r2 + 2*y_tmp*y_tmp) + 2*p2*x_tmp*y_tmp;
    double x_tmp_3 = x_tmp_2 + s1*r2 + s2*r4;
    double y_tmp_3 = y_tmp_2 + s3*r2 + s4*r4;
    double x_out = fx*x_tmp_3 + cx;
    double y_out = fy*y_tmp_3 + cy;
    return std::make_pair(x_out, y_out);
}

int main(){
    double largest_xdiff = 0;
    double largest_ydiff = 0;
    for(coord_out y = 0; y < 240; y++){
        for (coord_in x = 0; x < 320; x++){
            coord_and_coeff result = undistort_coordinate_generator(x, y, 320, 1.0/320.0, 240, 1.0/240.0, 160, 120, 0.2, -0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            std::pair<double, double> gold = undistort_coordinates_gold(x, y, 320, 240, 160, 120, 0.2, -0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            double x_gold = gold.first;
            double y_gold = gold.second;
            double x_diff = (double)result.x - x_gold;
            double y_diff = (double)result.y - y_gold;
            if ((x_diff > 1 || x_diff < -1 || y_diff > 1 || y_diff < -1) && (x_gold > -1 && x_gold < 320 && y_gold > -1 && y_gold < 240)){
                printf("x: %f y: %f tl: %f tr: %f bl: %f br: %f\n", (double)result.x, (double)result.y, (double)result.tl_coeff, (double)result.tr_coeff, (double)result.bl_coeff, (double)result.br_coeff);
                printf("x_gold: %f y_gold: %f\n", x_gold, y_gold);
                printf("x_diff: %f y_diff: %f\n", x_diff, y_diff);
                return 1;
            }
            if (abs(x_diff) > largest_xdiff){
                largest_xdiff = abs(x_diff);
            }
            if (abs(y_diff) > largest_ydiff){
                largest_ydiff = abs(y_diff);
            }
            printf("x: %f y: %f tl: %f tr: %f bl: %f br: %f\n", (double)result.x, (double)result.y, (double)result.tl_coeff, (double)result.tr_coeff, (double)result.bl_coeff, (double)result.br_coeff);
            printf("x_diff: %f y_diff: %f\n", x_diff, y_diff);
        }
    }
    printf("largest_xdiff: %f largest_ydiff: %f\n", largest_xdiff, largest_ydiff);
    return 0;
}
