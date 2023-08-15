`timescale 1ns/1ps

module bram_reader_tb();
    // Inputs to DUT
    reg clk;
    reg rst;
    reg start;
    reg [13:0] pixel_address_tl;
    reg [13:0] pixel_address_tr;
    reg [13:0] pixel_address_bl;
    reg [13:0] pixel_address_br;
    reg [2:0] pixel_row_index_tl;
    reg [2:0] pixel_row_index_tr;
    reg [2:0] pixel_row_index_bl;
    reg [2:0] pixel_row_index_br;
    wire [63:0] bram_data_out;
    reg [63:0] bram_data_in;
    wire [13:0] bram_addr_reader;
    reg [13:0] bram_addr_tb;
    wire [13:0] bram_addr_muxed;
    wire bram_we_reader;
    reg bram_we_tb;
    wire bram_we_muxed;
    reg interface_select;
    wire [7:0] pixel_tl;
    wire [7:0] pixel_tr;
    wire [7:0] pixel_bl;
    wire [7:0] pixel_br;
    wire data_valid;
    
    assign bram_addr_muxed = interface_select ? bram_addr_reader : bram_addr_tb;
    assign bram_we_muxed = interface_select ? bram_we_reader : bram_we_tb;

    // Instantiate DUT
    bram_reader #(
        .ADDR_WIDTH(14),
        .DATA_WIDTH(64),
        .PIXEL_PER_ADDRESS(8),
        .BITS_PER_PIXEL(8)
    ) dut (
        .clk(clk), 
        .rst(rst), 
        .start(start), 
        .pixel_addr_tl(pixel_address_tl), 
        .pixel_addr_tr(pixel_address_tr),
        .pixel_addr_bl(pixel_address_bl),
        .pixel_addr_br(pixel_address_br),
        .pixel_row_index_tl(pixel_row_index_tl),
        .pixel_row_index_tr(pixel_row_index_tr),
        .pixel_row_index_bl(pixel_row_index_bl),
        .pixel_row_index_br(pixel_row_index_br),
        .bram_out(bram_data_out), 
        .bram_addr(bram_addr_reader), 
        .bram_we(bram_we_reader), 
        .data_valid(data_valid),
        .pixel_tl(pixel_tl),
        .pixel_tr(pixel_tr),
        .pixel_bl(pixel_bl),
        .pixel_br(pixel_br)
    );

    // Instantiate BRAM
    my_single_port_bram #(
        .ADDR_WIDTH(14),
        .DATA_WIDTH(64)
    ) bram (
        .clk(clk),
        .rst(rst),
        .we(bram_we_muxed),
        .addr(bram_addr_muxed),
        .data_in(bram_data_in),
        .data_out(bram_data_out)
    );
    
    // Generate clock signal
    always #5 clk = ~clk;
    
    // Testbench
    initial begin
        clk = 0;
        rst = 0;
        interface_select = 0;
        start = 0;
        pixel_address_tl = 0;
        pixel_address_tr = 0;
        pixel_address_bl = 40;
        pixel_address_br = 40;
        pixel_row_index_tl = 0;
        pixel_row_index_tr = 1;
        pixel_row_index_bl = 0;
        pixel_row_index_br = 1;
        bram_data_in = 0;
    end
    
    integer idx;
    integer i;
    initial begin
        $dumpfile("bram_reader.vcd");
        $dumpvars(0, dut);
        $dumpvars(0, dut.pixels[0]);
        $dumpvars(0, dut.pixels[1]);
        $dumpvars(0, dut.pixels[2]);
        $dumpvars(0, dut.pixels[3]);
        // Also dump the bram content
        for(idx = 0; idx < 9600; idx = idx + 1) begin
            $dumpvars(0, bram.mem[idx]);
        end
        #10;
        rst = 1;
        #100;
        // Fill the memory with 1-255 in addres 1-255
        bram_we_tb = 1;
        for (i = 0; i < 9600; i = i + 1) begin
            bram_addr_tb = i;
            bram_data_in = ($urandom << 32) | $urandom;
            #10;
        end
        interface_select = 1;
        bram_we_tb = 0;
        // Start with (x, y) = (0, 0)
        start = 1;
        #10;
        start = 0;
        #100;
        // Give a negative address
        pixel_address_tl = -1;
        pixel_address_tr = -1;
        pixel_address_bl = -1;
        pixel_address_br = -1;
        #10;
        start = 1;
        #100;
        start = 0;
        #10;
        // Give a too large address
        pixel_address_tl = 9600;
        pixel_address_tr = 9600;
        pixel_address_bl = 9600;
        pixel_address_br = 9600;
        #10;
        start = 1;
        #100;
        start = 0;
        #10;
        $finish;
    end

endmodule
