// This module receives the interpolated pixels from the interpolator and writes them to a double buffered BRAM.
// It has a BRAM interface to allow the next module to read the images from it's double buffered BRAM.
module bram_writer_and_normalizer #(
    parameter ADDR_WIDTH = 14,
    parameter NUM_BYTES = 76800, // 1 full image
    parameter DATA_WIDTH_BRAM = 64,
    parameter NB_PARALLEL_PIXELS = 1 // Only use powers of 2 (up to 8)
) (
    input clk,
    input rst,
    input start,
    input data_valid,
    input [NB_PARALLEL_PIXELS * 8 - 1:0] interpolated_pixel_in,
    input [ADDR_WIDTH-1:0] bram_addr,
    input bram_we,
    output [DATA_WIDTH_BRAM - 1:0] bram_data_out,
    output done_irq
    // DEBUG SIGNALS (add comma above if you uncomment this)
    // output [ADDR_WIDTH-1:0] address,
    // output [8*8-1:0] pixels_out,
    // output [2:0] pixel_counter_out,
    // output [2:0] state_out,
    // output [2:0] next_state_out,
    // output [ADDR_WIDTH-1:0] bram_addr_counter_out
);

// Controls for the multi-BRAM
wire bram_we_first_framebuffer;
wire bram_we_second_framebuffer;
wire [ADDR_WIDTH-1:0] bram_addr_first_framebuffer;
wire [ADDR_WIDTH-1:0] bram_addr_second_framebuffer;
wire [DATA_WIDTH_BRAM - 1:0] data_out_bram_first_framebuffer;
wire [DATA_WIDTH_BRAM - 1:0] data_out_bram_second_framebuffer;


// Bus for the BRAM data input
wire [DATA_WIDTH_BRAM - 1:0] data_in_bram;

//instantiate BRAM for first framebuffer
my_single_port_bram #(
    .ADDR_WIDTH(ADDR_WIDTH),
    .NUM_BYTES(NUM_BYTES),
    .DATA_WIDTH(DATA_WIDTH_BRAM),
    .full_width(0)
) bram_inst (
    .clk(clk),
    .rst(rst),
    .we(bram_we_first_framebuffer),
    .addr(bram_addr_first_framebuffer),
    .data_in(data_in_bram),
    .data_out(data_out_bram_first_framebuffer)
);

//instantiate BRAM for second framebuffer
my_single_port_bram #(
    .ADDR_WIDTH(ADDR_WIDTH),
    .NUM_BYTES(NUM_BYTES),
    .DATA_WIDTH(DATA_WIDTH_BRAM),
    .full_width(0)
) bram_inst2 (
    .clk(clk),
    .rst(rst),
    .we(bram_we_second_framebuffer),
    .addr(bram_addr_second_framebuffer),
    .data_in(data_in_bram),
    .data_out(data_out_bram_second_framebuffer)
);

// State machine states variables
localparam IDLE = 2'b00;
localparam ACCUMULATE_PIXELS = 2'b01;
localparam WRITE_AND_SUM = 2'b10;
localparam DONE = 2'b11;
reg [1:0] current_state, next_state;
reg [ADDR_WIDTH-1:0] bram_addr_counter;
reg [2:0] pixel_counter;
reg [7:0] pixels [7:0];
reg framebuffer_select;

// BRAM Signal assignements
assign bram_we_first_framebuffer = framebuffer_select ? current_state == WRITE_AND_SUM : bram_we;
assign bram_we_second_framebuffer = framebuffer_select ? bram_we : current_state == WRITE_AND_SUM;
assign bram_addr_first_framebuffer = framebuffer_select ? bram_addr_counter : bram_addr;
assign bram_addr_second_framebuffer = framebuffer_select ? bram_addr : bram_addr_counter;
assign bram_data_out = framebuffer_select ? data_out_bram_second_framebuffer : data_out_bram_first_framebuffer; // This is the interface to allow the next block to read the data in the BRAMs
assign data_in_bram = {pixels[0], pixels[1], pixels[2], pixels[3], pixels[4], pixels[5], pixels[6], pixels[7]};
assign done_irq = (current_state == DONE);


// DEBUG SIGNALS
// assign address = bram_addr_counter;
// assign pixels_out = data_in_bram;
// assign pixel_counter_out = pixel_counter;
// assign state_out = current_state;
// assign next_state_out = next_state;
// assign bram_addr_counter_out = bram_addr_counter;

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
                next_state = ACCUMULATE_PIXELS;
            end else begin
                next_state = IDLE;
            end
        end
        ACCUMULATE_PIXELS: begin
            if (data_valid && pixel_counter == 8 - NB_PARALLEL_PIXELS) begin
                next_state = WRITE_AND_SUM;
            end else begin
                next_state = ACCUMULATE_PIXELS;
            end
        end
        WRITE_AND_SUM: begin
            if (bram_addr_counter < 9600-1) begin
                next_state = ACCUMULATE_PIXELS;
            end else begin
                next_state = DONE;
            end
        end
        DONE: begin
            next_state = IDLE;
        end
        default: begin
            next_state = IDLE;
        end
    endcase
end

// State machine process 3: counters and registers update
integer x;
always @(posedge clk or negedge rst) begin
    if (!rst) begin
        framebuffer_select <= 0;
        bram_addr_counter <= 0;
        pixel_counter <= 0;
        for (x = 0; x < 8; x = x + 1) begin
            pixels[x] <= 0;
        end
    end else begin
        case (current_state)
            IDLE: begin
                bram_addr_counter <= 0;
                if (next_state == ACCUMULATE_PIXELS && data_valid) begin
                    for (x = 0; x < NB_PARALLEL_PIXELS; x = x + 1) begin
                        pixels[pixel_counter + x] <= interpolated_pixel_in[(NB_PARALLEL_PIXELS - x) * 8 - 1 -: 8];
                    end
                    pixel_counter <= pixel_counter + NB_PARALLEL_PIXELS;
                end else begin
                    for (x = 0; x < 8; x = x + 1) begin
                        pixels[x] <= 0;
                    end
                    pixel_counter <= 0;
                end
            end
            ACCUMULATE_PIXELS: begin
                if (data_valid) begin
                    for (x = 0; x < NB_PARALLEL_PIXELS; x = x + 1) begin
                        pixels[pixel_counter + x] <= interpolated_pixel_in[(NB_PARALLEL_PIXELS - x) * 8 - 1 -: 8];
                    end
                    if (next_state == WRITE_AND_SUM) begin
                        pixel_counter <= 0;
                    end else begin
                        pixel_counter <= pixel_counter + NB_PARALLEL_PIXELS;
                    end
                end
            end
            WRITE_AND_SUM: begin
                bram_addr_counter <= bram_addr_counter + 1;
                if(next_state == ACCUMULATE_PIXELS && data_valid) begin
                    for (x = 0; x < NB_PARALLEL_PIXELS; x = x + 1) begin
                        pixels[pixel_counter + x] <= interpolated_pixel_in[(NB_PARALLEL_PIXELS - x) * 8 - 1 -: 8];
                    end
                    pixel_counter <= pixel_counter + NB_PARALLEL_PIXELS;
                end
            end
            DONE: begin
                bram_addr_counter <= 0;
            end
            default: begin
                bram_addr_counter <= 0;
                pixel_counter <= 0;
                for (x = 0; x < 8; x = x + 1) begin
                    pixels[x] <= 0;
                end
            end
        endcase
        if (next_state == DONE) begin
            framebuffer_select <= ~framebuffer_select;
        end
    end
end


endmodule