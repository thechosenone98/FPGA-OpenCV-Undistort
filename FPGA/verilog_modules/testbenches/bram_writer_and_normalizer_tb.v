`timescale 1ns/1ps

module bram_writer_and_normalizer_tb();

localparam ADDR_WIDTH = 14;
localparam DATA_WIDTH_BRAM = 72;
localparam NUM_BYTES = 259200;
localparam N = 3;
localparam NB_PARALLEL_PIXELS = 2;

// Inputs to DUT
reg clk;
reg rst;
reg start;
reg [NB_PARALLEL_PIXELS * 8 - 1:0] interpolated_pixel;
reg [NB_PARALLEL_PIXELS * 8 - 1:0] normalized_pixel;
reg data_valid;
reg normalized_data_valid;
wire [(DATA_WIDTH_BRAM * N) - 1:0] data_out; // wrong size for now, should be 24 bit for the normalize operation
wire done;

//BRAM interface of DUT
reg [ADDR_WIDTH - 1:0] bram_addr;
reg bram_we;
wire [(DATA_WIDTH_BRAM * N) - 1:0] bram_data_out;


// Instantiate DUT
bram_writer_and_normalizer #(
    .ADDR_WIDTH(ADDR_WIDTH),
    .NUM_BYTES(NUM_BYTES),
    .DATA_WIDTH_BRAM(DATA_WIDTH_BRAM),
    .N(N)
) dut (
    .clk(clk), 
    .rst(rst), 
    .start(start), 
    .data_valid(data_valid), 
    .normalized_data_valid(normalized_data_valid), 
    .interpolated_pixel_in(interpolated_pixel), 
    .normalized_pixel_in(normalized_pixel), 
    .data_out(data_out),
    .bram_addr(bram_addr),
    .bram_we(bram_we),
    .bram_data_out(bram_data_out),
    .done(done)
);

// Clock oscillate
always #5 clk = ~clk;

integer idx;
initial begin
    $dumpfile("bram_writer_and_normalizer.vcd");
    $dumpvars(0, dut);
    $dumpvars(0, dut.pixels[0]);
    $dumpvars(0, dut.pixels[1]);
    $dumpvars(0, dut.pixels[2]);
    $dumpvars(0, dut.pixels[3]);
    $dumpvars(0, dut.pixels[4]);
    $dumpvars(0, dut.pixels[5]);
    $dumpvars(0, dut.pixels[6]);
    $dumpvars(0, dut.pixels[7]);
    for (idx = 0; idx < 10; idx = idx + 1) begin
        $dumpvars(0, dut.bram_instances[0].bram_inst.mem[idx]);
        $dumpvars(0, dut.bram_instances[1].bram_inst.mem[idx]);
        $dumpvars(0, dut.bram_instances[2].bram_inst.mem[idx]);
    end
    // Initialize inputs
    clk = 0;
    rst = 0;
    start = 0;
    data_valid = 0;
    normalized_data_valid = 0;
    interpolated_pixel = 0;
    normalized_pixel = 0;
    bram_we = 0;
    bram_addr = 0;
    #100;
    rst = 1;
    #100
    start = 1;
    interpolated_pixel = 16'hAA55;
    data_valid = 1;
    #10;
    start = 0;
    // Wait until DUT current state is ACCUMULATE_NORMALIZED_PIXELS
    while (dut.current_state != 5) begin
        // Display the current state
        $display("Current state: %d", dut.current_state);
        $display("Next state: %d", dut.next_state);
        $display("BRAM address counter: %d", dut.bram_addr_counter);
        $display("Start:", dut.start);
        $display("RST:", dut.rst);
        // Flip all bits of interpolated_pixel (turning it to 0xAA)
        interpolated_pixel = ~interpolated_pixel;
        #10;
    end

    // Read some data from the BRAM
    for (idx = 0; idx < 100; idx = idx + 1) begin
        bram_addr = idx;
        #10;
        $display("BRAM data out: %h", bram_data_out);
    end
    $finish;
    
end

endmodule