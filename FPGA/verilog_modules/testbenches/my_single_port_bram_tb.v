`timescale 1ns / 1ps

module my_single_port_bram_tb();

localparam ADDR_WIDTH = 8;
localparam DATA_WIDTH = 8;

// Some varaibles to control the testbench
integer i;

// Inputs to DUT
reg clk;
reg we;
reg [ADDR_WIDTH-1:0] addr;
reg [DATA_WIDTH-1:0] data_out;
wire [DATA_WIDTH-1:0] data_in;

// Instantiate DUT
my_single_port_bram #(
    .ADDR_WIDTH(ADDR_WIDTH), 
    .DATA_WIDTH(DATA_WIDTH)
) dut (
    .clk(clk), 
    .we(we), 
    .addr(addr), 
    .data_in(data_out),
    .data_out(data_in)
);

// Generate clock singal
always #5 clk = ~clk;

// Testbench
initial begin
    clk = 0;
    we = 0;
    addr = 0;
    data_out = 0;
end

initial begin
    #100;
    // Fill the memory with 1-255 in addres 1-255
    for (i = 1; i < 256; i = i + 1) begin
        we = 1;
        addr = i;
        data_out = i;
        #10;
    end
    // Read the memory and check if the values are correct
    for (i = 1; i < 256; i = i + 1) begin
        we = 0;
        addr = i;
        #10;
        if (data_in != i) begin
            $display("ERROR: data_out = %h, expected = %h", data_out, i);
            $finish;
        end
    end
    $finish;
end

endmodule
