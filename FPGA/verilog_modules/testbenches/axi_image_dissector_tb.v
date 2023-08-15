`timescale 1ns / 1ps

module axi_image_dissector_tb();

  // Some local parameters to tune the DUT
  localparam n = 24;
  localparam i = 1;
  localparam d = 1;
  localparam u = 1;
  localparam pixel_per_clk = 8;
  localparam num_brams = 1;
  localparam addr_width = 14;

  // Inputs/Outputs of AXI Image Dissector
  reg ACLK;
  reg rst;
  reg s_TVALID;
  wire s_TREADY;
  reg [(8*n-1):0] s_TDATA;
  reg [(n-1):0] s_TSTRB;
  reg [(n-1):0] s_TKEEP;
  reg s_TLAST;
  reg [(i-1):0] s_TID;
  reg [(d-1):0] s_TDEST;
  reg [(u-1):0] s_TUSER;
  //All BRAM connections (use arrays for each BRAM)
  wire [num_brams-1:0] bram_we;
  wire [num_brams * addr_width - 1:0] bram_addr;
  wire [num_brams * (pixel_per_clk*8) - 1:0] bram_data_in;
  wire [num_brams * (pixel_per_clk*8) - 1:0] bram_data_out;
  wire transfer_ready;
  wire transfer_done;

  // Input/Ouput of BRAM to AXI Stream
  wire m_TVALID;
  wire [(8*n-1):0] m_TDATA;
  wire [(n-1):0] m_TSTRB;
  wire [(n-1):0] m_TKEEP;
  wire m_TLAST;
  wire [(i-1):0] m_TID;
  wire [(d-1):0] m_TDEST;
  wire [(u-1):0] m_TUSER;

  // Input/Output for image_undistort
  wire [72*3-1:0] data_out;


  // Instantiate the AXI Image Dissector
  axi_image_dissector_v2 #(
      .n(n),
      .i(i),
      .d(d),
      .u(u),
      .pixel_per_clk(pixel_per_clk),
      .num_brams(num_brams),
      .addr_width(addr_width)
  ) uut (
      .ACLK(ACLK),
      .rst(rst),
      .s_TVALID(s_TVALID),
      .s_TREADY(s_TREADY),
      .s_TDATA(s_TDATA),
      .s_TSTRB(s_TSTRB),
      .s_TKEEP(s_TKEEP),
      .s_TLAST(s_TLAST),
      .s_TID(s_TID),
      .s_TDEST(s_TDEST),
      .s_TUSER(s_TUSER),
      .bram_we(bram_we),
      .bram_addr(bram_addr),
      .bram_data_in(bram_data_in),
      .bram_data_out(bram_data_out),
      .transfer_ready(transfer_ready),
      .transfer_done(transfer_done)
  );

  // bram_to_axi_stream #(
  //     .n(n),
  //     .i(i),
  //     .d(d),
  //     .u(u),
  //     .pixel_per_clk(pixel_per_clk),
  //     .num_brams(num_brams),
  //     .addr_width(addr_width)
  // ) bram_to_axi_stream_inst (
  //     .ACLK(ACLK),
  //     .rst(rst),
  //     .m_TVALID(m_TVALID),
  //     .m_TREADY(1'b1),
  //     .m_TDATA(m_TDATA),
  //     .m_TSTRB(m_TSTRB),
  //     .m_TKEEP(m_TKEEP),
  //     .m_TLAST(m_TLAST),
  //     .m_TID(m_TID),
  //     .m_TDEST(m_TDEST),
  //     .m_TUSER(m_TUSER),
  //     .bram_we(bram_we),
  //     .bram_addr(bram_addr),
  //     .bram_data_in(bram_data_in),
  //     .bram_data_out(bram_data_out),
  //     .transfer_ready(transfer_ready),
  //     .transfer_done(transfer_done)
  // );

image_undistort #(
    .ADDR_WIDTH(14),
    .ADDR_WIDTH_INTERNAL_BRAM(15),
    .NUM_BYTES(204800),
    .DATA_WIDTH_BRAM(72),
    .DATA_WIDTH_IN(64),
    .N(3)
) img_undist (
    .clk(ACLK),
    .rst(rst),
    .go(transfer_ready),
    .bram_data_in(bram_data_in),
    .bram_data_out(bram_data_out),
    .bram_we(bram_we),
    .bram_addr(bram_addr),
    .data_out(data_out)
);

  // Clock generator
  always #5 ACLK = ~ACLK;

  // Testbench
  integer infile;
  integer idx = 0;
  integer n_bram = 0;
  integer tmp;
  reg [200*8-1:0] dummy;
  initial begin
    $dumpfile("test.vcd");
    $dumpvars(0, uut);
    // $dumpvars(0, bram_to_axi_stream_inst);
    $dumpvars(0, uut.pixel_parallel_inst);
    $dumpvars(0, img_undist);

    // $dumpvars(0, uut.pixel_parallel_inst.macc_loop[0].macc_inst);
    // $dumpvars(0, uut.pixel_parallel_inst.macc_loop[1].macc_inst);
    // $dumpvars(0, uut.pixel_parallel_inst.macc_loop[2].macc_inst);

//`ifdef SIMULATION
//    // Also dump the bram content
//    for(idx = 0; idx < 2**addr_width-1; idx = idx + 1) begin
//      $dumpvars(0, uut.bram_instances[0].bram_inst.mem[idx]);
//    end
//`else
//    // Dump the Xilinx MACRO BRAM
//   $dumpvars(0, uut.bram_instances[0].bram_inst.BRAM_SINGLE_MACRO_instances[0].BRAM_SINGLE_MACRO_inst);
//`endif

`ifdef SIMULATION
    // Dump the 24 bit image undistort bram content
    for(idx = 0; idx < 9600-1; idx = idx + 1) begin
      $dumpvars(0, img_undist.bram_instances[0].bram_inst.mem[idx]);
      $dumpvars(0, img_undist.bram_instances[1].bram_inst.mem[idx]);
      $dumpvars(0, img_undist.bram_instances[2].bram_inst.mem[idx]);
    end
`else
    // Dump the Xilinx MACRO BRAM
    $dumpvars(0, img_undist.bram_instances[0].bram_inst.BRAM_SINGLE_MACRO_instances[0].BRAM_SINGLE_MACRO_inst);
`endif
    
    //Initial values
    ACLK = 0;

    // Reset the DUT
    $display("Resetting DUT");
    rst = 0;
    #100;
    rst = 1;
    #15;
    $display("DUT reset");
    // Display start time
    $display("Start time: %t", $time);

    // Open the input file
    infile = $fopen("/Users/zach-mcc/KTH/Accerion-Thesis/fpga-image-processing/FPGA/verilog_modules/testbenches/input_vectors/axi_debug_vectors_noise_image.txt", "r"); // open file for reading
    $display("File opened: %d", infile);
    if (infile == 0) $fatal(-1, "Error: could not open input file");

    // Skip the first line
    tmp = $fgets(dummy, infile);
    $display("Num item read: %h", tmp);
    $display("dummy: %s", dummy);

    #5;

    // Read all lines in a loop (order is TDATA	TDEST	TID	TKEEP	TSTRB	TUSER	TVALID	TREADY	TLAST)
    while (!$feof(infile)) begin

      // read inputs for a and b from file
      tmp = $fscanf(infile, "%h %h %h %h %h %h %h %h %h\n", s_TDATA, s_TDEST, s_TID, s_TKEEP, s_TSTRB, s_TUSER, s_TVALID, dummy, s_TLAST);
      $display("Num item read: %h", tmp);
      // Display simulation time and inputs
      $display("%t: %h %h %h %h %h %h %h %h", $time, s_TDATA, s_TDEST, s_TID, s_TKEEP, s_TSTRB, s_TUSER, s_TVALID, s_TLAST);
      
      #10;
    end


    // Wait until transfer done is high
    #100000;
    $display("End time: %t", $time);
    $display("Simulation finished");
    
    $fclose(infile); // close file
    $finish;
  end



endmodule