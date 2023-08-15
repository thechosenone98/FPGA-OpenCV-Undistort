module bram_reader #(
    parameter ADDR_WIDTH = 12,
    parameter DATA_WIDTH = 256,
    parameter PIXEL_PER_ADDRESS = 8,
    parameter BITS_PER_PIXEL = 8
)(
    input clk,
    input rst,
    input [ADDR_WIDTH-1:0] pixel_addr_oo,
    input [ADDR_WIDTH-1:0] pixel_addr_oe,
    input [ADDR_WIDTH-1:0] pixel_addr_eo,
    input [ADDR_WIDTH-1:0] pixel_addr_ee,
    input [2:0] pixel_row_index_oo,
    input [2:0] pixel_row_index_oe,
    input [2:0] pixel_row_index_eo,
    input [2:0] pixel_row_index_ee,
    input odd_pixel,
    input odd_row,
    input start,
    output [4*ADDR_WIDTH-1:0] bram_addr,
    input [DATA_WIDTH-1:0] bram_out,
    output bram_we,
    output odd_pixel_out,
    output odd_row_out,
    output reg [BITS_PER_PIXEL - 1:0] pixel_tl,
    output reg [BITS_PER_PIXEL - 1:0] pixel_tr,
    output reg [BITS_PER_PIXEL - 1:0] pixel_bl,
    output reg [BITS_PER_PIXEL - 1:0] pixel_br,
    output reg data_valid
);

assign bram_we = 1'b0; // This module never writes to BRAM
assign bram_addr = {pixel_addr_oo, pixel_addr_oe, pixel_addr_eo, pixel_addr_ee};
assign odd_pixel_out = odd_pixel;
assign odd_row_out = odd_row;

// State machine states
localparam IDLE = 1'b0;
localparam READ_DATA = 1'b1;

// State machine signals
reg current_state, next_state;
reg [3:0] pixel_row_index_oo_reg;

// State machine: Next state logic
always @ (posedge clk or negedge rst) begin
    if (!rst) begin
        current_state <= IDLE;
    end else begin
        current_state <= next_state;
    end
end

// State machine: State logic
always @ (*) begin
    case (current_state)
        IDLE: begin
            if (start) begin
                next_state = READ_DATA;
            end else begin
                next_state = IDLE;
            end
        end
        READ_DATA: begin
            if(!start)
                next_state = IDLE;
            else
                next_state = READ_DATA;
        end
        default: next_state = IDLE;
    endcase
end


// We receive 4 chunks of 8 pixels each, they represent the top left chunk, the top right chunk, the bottom left chunk and the bottom right chunk
// We need to extract our top pixel from them, the top left and top right pixel may be in the same chunk, same goes for the bottom left and bottom right pixel
// The reason we require 4 chunks is if our pixels happen to be on the last pixel of the top left chunk, the first pixel of the top right chunk, the last pixel of the bottom left chunk and the first pixel of the bottom right chunk
wire [7:0] in_tl = bram_out[256 - pixel_row_index_oo_reg*8 - 1 -: 8];
wire [7:0] in_tr = bram_out[256 - (pixel_row_index_oo_reg + 1)*8 - 1 -: 8];
wire [7:0] in_bl = bram_out[128 - pixel_row_index_oo_reg*8 - 1 -: 8];
wire [7:0] in_br = bram_out[128 - (pixel_row_index_oo_reg + 1)*8 - 1 -: 8];


// State machine: counter and register logic
always @ (posedge clk or negedge rst) begin
    if (!rst) begin
        data_valid <= 1'b0;
        pixel_row_index_oo_reg <= 0;
        pixel_tl <= 0;
        pixel_tr <= 0;
        pixel_bl <= 0;
        pixel_br <= 0;
    end else begin
        case (current_state)
            IDLE: begin
                data_valid <= 1'b0;
                if (next_state == READ_DATA) begin
                    pixel_row_index_oo_reg <= pixel_row_index_oo;
                end
            end
            READ_DATA: begin
                pixel_tl <= in_tl;
                pixel_tr <= in_tr;
                pixel_bl <= in_bl;
                pixel_br <= in_br;
                data_valid <= 1'b1;
                pixel_row_index_oo_reg <= pixel_row_index_oo;
            end
            default: begin
                data_valid <= 1'b0;
                pixel_row_index_oo_reg <= 0;
                pixel_tl <= 0;
                pixel_tr <= 0;
                pixel_bl <= 0;
                pixel_br <= 0;
            end
        endcase
    end
end

endmodule