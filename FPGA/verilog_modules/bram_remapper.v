module bram_remapper(
    input [63:0] in_vec,
    output [63:0] out_vec
);

assign out_vec = {in_vec[7-:8], in_vec[15-:8], in_vec[23-:8], in_vec[31-:8], in_vec[39-:8], in_vec[47-:8], in_vec[55-:8], in_vec[63-:8]};

endmodule
