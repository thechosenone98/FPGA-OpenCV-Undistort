module fifo_buffer_tb();

// Signals for DUT
reg clk;
reg rst;
reg start;
reg [9:0] coeff_tl;
reg [9:0] coeff_tr;
reg [9:0] coeff_bl;
reg [9:0] coeff_br;

wire [9:0] coeff_tl_out;
wire [9:0] coeff_tr_out;
wire [9:0] coeff_bl_out;
wire [9:0] coeff_br_out;

// Clock generation
always #5 clk = ~clk;

// Instantiate DUT
fifo_buffer #(
    .DEPTH(20)
) dut (
    .clk(clk),
    .rst(rst),
    .start(start),
    .coeff_tl(coeff_tl),
    .coeff_tr(coeff_tr),
    .coeff_bl(coeff_bl),
    .coeff_br(coeff_br),
    .coeff_tl_out(coeff_tl_out),
    .coeff_tr_out(coeff_tr_out),
    .coeff_bl_out(coeff_bl_out),
    .coeff_br_out(coeff_br_out)
);

// Initialize signals
integer i;
initial begin
    $dumpfile("fifo_buffer.vcd");
    $dumpvars(0, dut);
    clk = 0;
    rst = 0;
    start = 0;
    coeff_tl = 0;
    coeff_tr = 0;
    coeff_bl = 0;
    coeff_br = 0;
    #10 rst = 0;
    #10 rst = 1;
    coeff_tl = $random;
    coeff_tr = $random;
    coeff_bl = $random;
    coeff_br = $random;
    #10 start = 1;
    for(i = 0; i < 20; i = i + 1) begin
        // Put random data into the FIFO
        coeff_tl = $random;
        coeff_tr = $random;
        coeff_bl = $random;
        coeff_br = $random;
        #10;
    end
    #1000;
    start = 0;
    #10;
    start = 1;
    #300;
    $finish;
end

endmodule