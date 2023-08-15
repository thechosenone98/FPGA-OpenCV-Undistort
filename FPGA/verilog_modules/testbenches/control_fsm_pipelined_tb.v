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
    reg transfer_done;
    wire start_coeff;
    wire start_xy_to_bram;
    wire start_bram_reader;
    wire start_interpolator;
    wire start_bram_writer;
    wire [8:0] u, v;

    // Instantiate the control_fsm module
    control_fsm_pipelined #(
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
        .transfer_done(transfer_done),
        .start_coeff(start_coeff),
        .start_xy_to_bram(start_xy_to_bram),
        .start_bram_reader(start_bram_reader),
        .start_interpolator(start_interpolator),
        .start_bram_writer(start_bram_writer),
        .u(u),
        .v(v)
    );

    // Clock generation
    always #5 clk = ~clk;

    integer i;
    // Testbench stimulus
    initial begin
        $dumpfile("control_fsm_pipelined.vcd");
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

        #100;
        coeff_done = 1;
        #100;
        xy_to_bram_done = 1;
        #100;
        bram_reader_done = 1;
        #100;
        interpolator_done = 1;
        // Wait for the bram_writer to finish (wait for the signal to become high)
        wait (dut.processed_pixels == 76800);
        #50;
        transfer_done = 1;
        #50;
        transfer_done = 0;
        #50;
        start = 1;
        #50;



        // Finish the simulation
        #10 $finish;
    end

endmodule
