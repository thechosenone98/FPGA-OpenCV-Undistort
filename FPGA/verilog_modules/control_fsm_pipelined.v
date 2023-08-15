// This module controls the entire pipeline and is responsible for making sure that
// the pipeline is properly primed and resetted between frames.
module control_fsm_pipelined #(
    parameter ROWS = 240,
    parameter COLS = 320
)(
    input clk,
    input rst,
    input start,
    input coeff_done,
    input xy_to_bram_done,
    input bram_reader_done,
    input interpolator_done,
    input bram_writer_done,
    input transfer_done,
    output start_coeff,
    output start_xy_to_bram,
    output start_bram_reader,
    output start_interpolator,
    output start_bram_writer_irq,
    output reset_undistort_machine_n,
    output reset_undistort_machine,
    output [8:0] u,
    output [8:0] v
);

localparam IDLE = 3'b000;
localparam PRIMING_ONE = 3'b001;
localparam PRIMING_TWO = 3'b010;
localparam PRIMING_THREE = 3'b011;
localparam CONTINUOUS = 3'b100;
localparam DONE = 3'b101;
reg [2:0] current_state, next_state;
reg [8:0] col, row;
reg [16:0] processed_pixels;

// Signal assignments
assign start_bram_writer_irq = start && current_state == IDLE;
assign start_coeff = ((current_state != IDLE && current_state != DONE) || next_state == PRIMING_ONE);
assign start_xy_to_bram = ((current_state != IDLE && current_state != PRIMING_ONE && current_state != DONE) || next_state == PRIMING_TWO);
assign start_bram_reader = ((current_state != IDLE && current_state != PRIMING_ONE && current_state != PRIMING_TWO && current_state != DONE) || next_state == PRIMING_THREE);
assign start_interpolator = ((current_state != IDLE && current_state != PRIMING_ONE && current_state != PRIMING_TWO && current_state != PRIMING_THREE && current_state != DONE) || next_state == CONTINUOUS);
assign u = col;
assign v = row;
assign reset_undistort_machine_n = (next_state == IDLE || (current_state == IDLE && !start)) ? 0 : 1;
assign reset_undistort_machine = (next_state == IDLE || (current_state == IDLE && !start));

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
                next_state = PRIMING_ONE;
            end else begin
                next_state = IDLE;
            end
        end
        PRIMING_ONE: begin
            if (coeff_done) begin
                next_state = PRIMING_TWO;
            end else begin
                next_state = PRIMING_ONE;
            end
        end
        PRIMING_TWO: begin
            if (xy_to_bram_done) begin
                next_state = PRIMING_THREE;
            end else begin
                next_state = PRIMING_TWO;
            end
        end
        PRIMING_THREE: begin
            if (bram_reader_done) begin
                next_state = CONTINUOUS;
            end else begin
                next_state = PRIMING_THREE;
            end
        end
        CONTINUOUS: begin
            if(processed_pixels == ROWS * COLS) begin
                next_state = DONE;
            end else begin
                next_state = CONTINUOUS;
            end
        end
        DONE: begin
            if(transfer_done) begin // Could technically be bram_writer_done if you are sure you can read the frame faster than the pipeline can process the next one
                next_state = IDLE;
            end else begin
                next_state = DONE;
            end
        end
        default: begin
            next_state = IDLE;
        end
    endcase
end

// State machine: counter logic
always @ (posedge clk or negedge rst) begin
    if (!rst) begin
        col <= 0;
        row <= 0;
        processed_pixels <= 0;
    end else begin
        if(interpolator_done) begin
            processed_pixels <= processed_pixels + 1;
        end
        if ((current_state != IDLE && current_state != DONE) || next_state == PRIMING_ONE) begin
            if (col == COLS - 1) begin
                col <= 0;
                row <= row + 1;
            end else begin
                col <= col + 1;
            end
        end else begin
            col <= 0;
            row <= 0;
            processed_pixels <= 0;
        end
    end
end

endmodule