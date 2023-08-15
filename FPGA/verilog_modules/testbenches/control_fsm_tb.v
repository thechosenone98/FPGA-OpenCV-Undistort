`timescale 1ns/1ps

module control_fsm_tb();
    reg clk;
    reg rst;
    reg start;
    reg coeff_done;
    reg xy_to_bram_done;
    reg bram_reader_done;
    reg interpolator_done;
    reg bram_writer_done;
    wire start_coeff;
    wire start_xy_to_bram;
    wire start_bram_reader;
    wire start_interpolator;
    wire start_bram_writer;

    // Instantiate the control_fsm module
    control_fsm #(
        .ROWS(240),
        .COLS(320)
    ) dut (
        .clk(clk),
        .rst(rst),
        .start(start),
        .coeff_done(coeff_done),
        .xy_to_bram_done(xy_to_bram_done),
        .bram_reader_done(bram_reader_done),
        .interpolator_done(interpolator_done),
        .bram_writer_done(bram_writer_done),
        .start_coeff(start_coeff),
        .start_xy_to_bram(start_xy_to_bram),
        .start_bram_reader(start_bram_reader),
        .start_interpolator(start_interpolator),
        .start_bram_writer(start_bram_writer)
    );

    // Clock generation
    always #5 clk = ~clk;

    integer i;
    // Testbench stimulus
    initial begin
        $dumpfile("control_fsm.vcd");
        $dumpvars(0, dut);
        // Initialize inputs
        clk = 0;
        rst = 0;
        start = 0;
        coeff_done = 0;
        xy_to_bram_done = 0;
        bram_reader_done = 0;
        interpolator_done = 0;
        bram_writer_done = 0;

        #10;
        rst = 1;

        // Start the control_fsm
        #10;
        start = 1;
        #10;
        start = 0;

        // Iterate through the different inputs
        for(i = 0; i < 320*240; i = i + 1) begin
            #100;
            coeff_done = 1;
            #10;
            coeff_done = 0;
            #100;
            xy_to_bram_done = 1;
            #10;
            xy_to_bram_done = 0;
            #100;
            bram_reader_done = 1;
            #10;
            bram_reader_done = 0;
            #100;
            interpolator_done = 1;
            #10;
            interpolator_done = 0;
        end

        // Finish the simulation
        #10 $finish;
    end

endmodule
