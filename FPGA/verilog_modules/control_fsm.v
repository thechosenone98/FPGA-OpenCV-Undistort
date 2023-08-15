module control_fsm #(
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
    output start_bram_writer,
    output [8:0] u,
    output [8:0] v
);

reg [9:0] row, col;

// State machine states
localparam IDLE = 3'b000;
localparam START_COEFF = 3'b001;
localparam START_XY_TO_BRAM = 3'b010;
localparam START_BRAM_READER = 3'b011;
localparam START_INTERPOLATOR = 3'b100;
localparam DONE = 3'b110;
reg [2:0] current_state, next_state;
reg single_cycle;

// Signal assignments
assign start_coeff = current_state == START_COEFF && single_cycle;
assign start_xy_to_bram = current_state == START_XY_TO_BRAM && single_cycle;
assign start_bram_reader = current_state == START_BRAM_READER && single_cycle;
assign start_interpolator = current_state == START_INTERPOLATOR && single_cycle;
assign start_bram_writer = start && current_state == IDLE;
assign u = col;
assign v = row;

// State machine process 1: State register
always @(posedge clk or negedge rst) begin
    if (!rst) begin
        current_state <= IDLE;
    end else begin
        current_state <= next_state;
    end
end

// State machine process 2: Next state logic
always @ (*) begin
    case (current_state)
        IDLE: begin
            if (start) begin
                next_state = START_COEFF;
            end else begin
                next_state = IDLE;
            end
        end
        START_COEFF: begin
            if (coeff_done) begin
                next_state = START_XY_TO_BRAM;
            end else begin
                next_state = START_COEFF;
            end
        end
        START_XY_TO_BRAM: begin
            if (xy_to_bram_done) begin
                next_state = START_BRAM_READER;
            end else begin
                next_state = START_XY_TO_BRAM;
            end
        end
        START_BRAM_READER: begin
            if (bram_reader_done) begin
                next_state = START_INTERPOLATOR;
            end else begin
                next_state = START_BRAM_READER;
            end
        end
        START_INTERPOLATOR: begin
            if (interpolator_done) begin
                if(row == ROWS - 1 && col == COLS - 1) begin
                    next_state = DONE;
                end else begin
                    next_state = START_COEFF;
                end
            end else begin
                next_state = START_INTERPOLATOR;
            end
        end
        DONE: begin
            if(transfer_done) begin
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

// State machine process 3: registers update logic
always @(posedge clk or negedge rst) begin
    if (!rst) begin
        row <= 0;
        col <= 0;
        single_cycle <= 0;
    end else begin
        if (current_state == IDLE) begin
            row <= 0;
            col <= 0;
            single_cycle <= 0;
        end else begin
            if (current_state == START_INTERPOLATOR && interpolator_done) begin
                if (col < COLS - 1) begin
                    col <= col + 1;
                end else begin
                    col <= 0;
                    if (row < ROWS - 1) begin
                        row <= row + 1;
                    end else begin
                        row <= 0;
                    end
                end
            end
        end
        if (next_state != current_state) begin
            single_cycle <= 1;
        end else begin
            single_cycle <= 0;
        end
    end
end


endmodule