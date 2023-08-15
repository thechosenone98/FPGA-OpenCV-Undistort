module bram_address_decoder_output_splitter(
    input [143:0] in_vec, 
    output [11:0] pixel_address_oo,
    output [2:0] pixel_row_index_oo,
    output [11:0] pixel_address_oe,
    output [2:0] pixel_row_index_oe,
    output [11:0] pixel_address_eo,
    output [2:0] pixel_row_index_eo,
    output [11:0] pixel_address_ee,
    output [2:0] pixel_row_index_ee,
    output odd_pixel,
    output odd_row
);

// The output created by Vitis HLS are 4 bytes aligned, so we account for padding
// Also every variable takes multiple of bytes (not bits)

assign pixel_address_oo = in_vec[11-:12];
// 4 bits to fill up 16 bits
assign pixel_row_index_oo = in_vec[18-:3];
// fill up to 4 bytes
assign pixel_address_oe = in_vec[43-:12];
// 4 bits to fill up 16 bits
assign pixel_row_index_oe = in_vec[50-:3];
// fill up to 4 bytes
assign pixel_address_eo = in_vec[75-:12];
// 4 bits to fill up 16 bits
assign pixel_row_index_eo = in_vec[82-:3];
// fill up to 4 bytes
assign pixel_address_ee = in_vec[107-:12];
// 4 bits to fill up 16 bits
assign pixel_row_index_ee = in_vec[114-:3];
// fill up to 4 bytes
assign odd_pixel = in_vec[128];
assign odd_row = in_vec[136];


  
endmodule