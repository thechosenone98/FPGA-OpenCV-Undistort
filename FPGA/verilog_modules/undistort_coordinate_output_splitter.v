module undistort_coordinate_output_splitter(
    input [95:0] in_vec, 
    output [14:0] x,
    output [14:0] y,
    output [9:0] coeff_tl,
    output [9:0] coeff_tr,
    output [9:0] coeff_bl,
    output [9:0] coeff_br
    // output was_resetted
);

// The output created by Vitis HLS are 4 bytes aligned, so we account for padding
// Also every variable takes multiple of bytes (not bits)

assign x = in_vec[14-:15];
// 16 bits to over is the next value
assign y = in_vec[30-:15];
// 16 bits to over is the next value
assign coeff_tl = in_vec[41-:10];
// 16 bits to over is the next value
assign coeff_tr = in_vec[57-:10];
// 16 bits to over is the next value
assign coeff_bl = in_vec[73-:10];
// 16 bits to over is the next value
assign coeff_br = in_vec[89-:10];
// 16 bits to over is the next value
// assign was_resetted = in_vec[96-:1];
  
endmodule