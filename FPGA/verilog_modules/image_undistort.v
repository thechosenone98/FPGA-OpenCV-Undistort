module image_undistort #(
    parameter ADDR_WIDTH = 14,
    parameter NUM_BYTES = 259200,
    parameter DATA_WIDTH_BRAM = 72,
    parameter DATA_WIDTH_IN = 64,
    parameter N = 3
) (
    input clk,
    input rst,
    input go,
    output [DATA_WIDTH_IN-1:0] bram_data_in,
    input [DATA_WIDTH_IN-1:0] bram_data_out,
    output bram_we,
    output reg [ADDR_WIDTH-1:0] bram_addr,
    output reg [DATA_WIDTH_BRAM * N - 1:0] data_out,
    //Signals to control the remap map BRAM
    output [ADDR_WIDTH-1:0] addr_remap,
    input [63:0] data_out_bram_remap
);

// Some assignement for signal that we aren't using for the BRAM containing the original image
assign bram_data_in = 0;
assign bram_we = 0;

// Controls for the multi-BRAM
reg we;
reg [ADDR_WIDTH-1:0] addr;
wire [(DATA_WIDTH_BRAM * N) - 1:0] data_out_bram;


// Bus that will contain result of calculation on go to BRAM
reg [(DATA_WIDTH_BRAM * N) - 1:0] data_in_bram; //Actually only needs 8 * 24 bits so last 24 bits are not used

//instantiate multiple brams that work in parallel (can write all 8 bytes from image at once)
genvar i;
generate
    for (i = 0; i < N; i = i + 1) begin : bram_instances
        my_single_port_bram #(
            .ADDR_WIDTH(ADDR_WIDTH),
            .NUM_BYTES(NUM_BYTES / N),
            .DATA_WIDTH(DATA_WIDTH_BRAM),
            .full_width(1)
        ) bram_inst (
            .clk(clk),
            .rst(rst),
            .we(we),
            .addr(addr),
            .data_in(data_in_bram[(N-i)*DATA_WIDTH_BRAM - 1-:DATA_WIDTH_BRAM]),
            .data_out(data_out_bram[(N-i)*DATA_WIDTH_BRAM - 1-:DATA_WIDTH_BRAM])
        );
    end
endgenerate

// START of FSM for undistort
// AS A FIRST APPROACH, LET'S CONVERT PIXELS TO 24 BIT FIXED POINT (Q0.24)

// State machine states variables
localparam IDLE = 2'b00;
localparam CALCULATE_AND_WRITE = 2'b01;
localparam WRITE = 2'b10;
reg [1:0] current_state, next_state;
reg [ADDR_WIDTH-1:0] bram_addr_counter;

// State machine process 1: State register
always @(posedge clk or negedge rst) begin
    if (!rst) begin
        current_state <= IDLE;
    end else begin
        current_state <= next_state;
    end
end

// State machine process 2: State logic
always @(*) begin
    case (current_state)
        IDLE: begin
            if (go) begin
                next_state = CALCULATE_AND_WRITE;
            end else begin
                next_state = IDLE;
            end
        end
        CALCULATE_AND_WRITE: begin
            if (bram_addr_counter < (NUM_BYTES / N) - 1) begin
                next_state = CALCULATE_AND_WRITE;
            end else begin
                next_state = IDLE;
            end
        end
        default: begin
            next_state = IDLE;
        end
    endcase
end

// State machine process 3: State output
integer j;
always @(*) begin
    case (current_state)
        IDLE: begin
            we = 1'b0;
            addr = bram_addr_counter - 1;
            bram_addr = bram_addr_counter;
            data_out = 0;
        end
        CALCULATE_AND_WRITE: begin
            we = 1'b1;
            addr = bram_addr_counter - 1;
            bram_addr = bram_addr_counter;
            for (j = 1; j < 9; j = j + 1) begin
                data_in_bram[j*24+:24] = {bram_data_out[(j-1)*8+:8], 16'b0};
            end
            data_in_bram[0+:24] = 0;
            data_out = data_out_bram;
        end
        default: begin
            we = 1'b0;
            addr = 0;
            bram_addr = 0;
            data_out = 0;
        end
    endcase
end

// State machine process 4: State counter
always @(posedge clk or negedge rst) begin
    if (!rst) begin
        bram_addr_counter <= 0;
    end else begin
        case (current_state)
            IDLE: begin
                if (next_state == CALCULATE_AND_WRITE) begin
                    bram_addr_counter <= bram_addr_counter + 1;
                end else begin
                    bram_addr_counter <= 0;
                end
            end
            CALCULATE_AND_WRITE: begin
                bram_addr_counter <= bram_addr_counter + 1;
            end
            default: begin
                bram_addr_counter <= 0;
            end
        endcase
    end
end

endmodule