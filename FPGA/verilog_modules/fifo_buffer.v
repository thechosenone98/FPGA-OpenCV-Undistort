// This module is a FIFO buffer that stores the coefficients for the bilinear interpolation
// until the bram_address_decoder module is fully primed and the bram_reader outputs the first group of pixels.
// Then the linear_interpolator module can start using the pixels in addition with the coefficients from this FIFO buffer.
module fifo_buffer #(
    parameter DEPTH = 20    
)(
    input clk,
    input rst,
    input start,
    input [9:0] coeff_tl,
    input [9:0] coeff_tr,
    input [9:0] coeff_bl,
    input [9:0] coeff_br,
    output wire [9:0] coeff_tl_out,
    output wire [9:0] coeff_tr_out,
    output wire [9:0] coeff_bl_out,
    output wire [9:0] coeff_br_out
);

// State machine states
localparam IDLE = 2'b00;
localparam FILLING = 2'b01;
localparam CONTINUOUS = 2'b10;
reg [2:0] current_state, next_state;
reg [5:0] counter;

reg [9:0] fifo_coeff_tl [DEPTH-1 : 0];
reg [9:0] fifo_coeff_tr [DEPTH-1 : 0];
reg [9:0] fifo_coeff_bl [DEPTH-1 : 0];
reg [9:0] fifo_coeff_br [DEPTH-1 : 0];

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
                next_state = FILLING;
            end else begin
                next_state = IDLE;
            end
        end
        FILLING: begin
            if (counter >= DEPTH - 1) begin
                next_state = CONTINUOUS;
            end else begin
                next_state = FILLING;
            end
        end
        CONTINUOUS: begin
            if(!start) begin
                next_state = IDLE;
            end else begin
                next_state = CONTINUOUS;
            end
        end
        default: begin
            next_state = IDLE;
        end
    endcase
end

// Counter logic
always @ (posedge clk or negedge rst) begin
    if (!rst) begin
        counter <= 0;
    end else begin
        if (current_state == FILLING || next_state == FILLING) begin
            counter <= counter + 1;
        end else begin
            counter <= 0;
        end
    end
end

//FIFO Logic
integer i;
always @ (posedge clk) begin
    if (current_state == FILLING || next_state == FILLING) begin
        fifo_coeff_tl[counter] <= coeff_tl;
        fifo_coeff_tr[counter] <= coeff_tr;
        fifo_coeff_bl[counter] <= coeff_bl;
        fifo_coeff_br[counter] <= coeff_br;
    end else if (current_state == CONTINUOUS) begin
        for(i = 0; i < DEPTH - 1; i = i + 1) begin
            fifo_coeff_tl[i] <= fifo_coeff_tl[i+1];
            fifo_coeff_tr[i] <= fifo_coeff_tr[i+1];
            fifo_coeff_bl[i] <= fifo_coeff_bl[i+1];
            fifo_coeff_br[i] <= fifo_coeff_br[i+1];
        end
        fifo_coeff_tl[DEPTH-1] <= coeff_tl;
        fifo_coeff_tr[DEPTH-1] <= coeff_tr;
        fifo_coeff_bl[DEPTH-1] <= coeff_bl;
        fifo_coeff_br[DEPTH-1] <= coeff_br;
    end
end

assign coeff_tl_out = fifo_coeff_tl[0];
assign coeff_tr_out = fifo_coeff_tr[0];
assign coeff_bl_out = fifo_coeff_bl[0];
assign coeff_br_out = fifo_coeff_br[0];

endmodule
