`timescale 1ns / 1ps

/////////////////////////////////////////////////////////////////////
//  READ_WIDTH | BRAM_SIZE | READ Depth  | ADDR Width |            //
// WRITE_WIDTH |           | WRITE Depth |            |  WE Width  //
// ============|===========|=============|============|============//
//    37-72    |  "36Kb"   |      512    |    9-bit   |    8-bit   //
//    19-36    |  "36Kb"   |     1024    |   10-bit   |    4-bit   //
//    19-36    |  "18Kb"   |      512    |    9-bit   |    4-bit   //
//    10-18    |  "36Kb"   |     2048    |   11-bit   |    2-bit   //
//    10-18    |  "18Kb"   |     1024    |   10-bit   |    2-bit   //
//     5-9     |  "36Kb"   |     4096    |   12-bit   |    1-bit   //
//     5-9     |  "18Kb"   |     2048    |   11-bit   |    1-bit   //
//     3-4     |  "36Kb"   |     8192    |   13-bit   |    1-bit   //
//     3-4     |  "18Kb"   |     4096    |   12-bit   |    1-bit   //
//       2     |  "36Kb"   |    16384    |   14-bit   |    1-bit   //
//       2     |  "18Kb"   |     8192    |   13-bit   |    1-bit   //
//       1     |  "36Kb"   |    32768    |   15-bit   |    1-bit   //
//       1     |  "18Kb"   |    16384    |   14-bit   |    1-bit   //
/////////////////////////////////////////////////////////////////////
module priority_encoder #(
    parameter N = 4,
    parameter INDEX_WIDTH = 2
) (
    input [N-1:0] one_hot_vector,
    output reg [INDEX_WIDTH-1:0] index
);
    integer i;

    always @(*) begin
        index = 0;
        for (i = 0; i < N; i = i + 1) begin
            if (one_hot_vector[i]) begin
                index = i;
            end
        end
    end

endmodule

module my_single_port_bram #(
    parameter ADDR_WIDTH = 8,
    parameter NUM_BYTES = 76800,
    parameter DATA_WIDTH = 32,
    // This parameter indicates wether you want to use 32k of the 36k block ram or the full 36k
    // If we use the full width without needing it, it create an extra write enable signal for the list byte that we have to deal with for no reason
    parameter full_width = 0
) (
    input clk,
    input rst,
    input we,
    input [ADDR_WIDTH-1:0] addr,
    input [DATA_WIDTH-1:0] data_in,
`ifdef SIMULATION
    output reg [DATA_WIDTH-1:0] data_out
`else
    output [DATA_WIDTH-1:0] data_out
`endif
);

function integer clog2;
    input [31:0] value;
    begin
        value = value - 1;
        for (clog2 = 0; value > 0; clog2 = clog2 + 1) begin
            value = value >> 1;
        end
    end
endfunction

`ifdef SIMULATION
    reg [DATA_WIDTH-1:0] mem [NUM_BYTES-1:0];
    integer i;

    always @(posedge clk) begin
        if (!rst) begin
            data_out <= 0;
            // for (i = 0; i < NUM_BYTES; i = i + 1) begin
            //     $display("mem[%d] = %d", i, mem[i]);
            //     mem[i] <= 0;
            // end
        end else begin
            if (we) begin
                mem[addr] <= data_in;
            end
            data_out <= mem[addr];
        end
    end
`else
    //Determine how many block RAM we need based on parameters
    localparam TOTAL_MEM_SIZE = NUM_BYTES * 8;
    localparam NUM_BRAMS_36k = (TOTAL_MEM_SIZE + (32768 * (1-full_width) + (36864 * full_width)) - 1) / (32768 * (1-full_width) + (36864 * full_width));
    localparam WE_WIDTH = DATA_WIDTH / 8;

    // Create a adress decoder for the block RAMs
    // 36k BRAM have 9 bit addresses and the write enable varies with the data width
    // register enable (re) is always 1 bit (we toggle it here to avoid multiple drivers to our mux later on)
    wire [8:0] addr_36k [NUM_BRAMS_36k-1:0];
    // This signal is the bus that contains all the outputs from the BRAMs
    wire [DATA_WIDTH-1:0] data_out_36k [NUM_BRAMS_36k-1:0];
    // This signal enables the write signal to the correct BRAMbased on the address
    wire [WE_WIDTH-1:0] we_36k [NUM_BRAMS_36k-1:0];
    // Create a signal that takes 1 bit every nth bit from we_36k to create a 1-hot signal
    // This signal is used to get the index with the priority encoder and then enable the correct output
    wire [NUM_BRAMS_36k-1:0] index_1hot;

    // Define the address decoder
    // Start by calculating base addresses for each BRAM
    integer i;
    reg [31:0] RAM_BASE_ADDR [NUM_BRAMS_36k - 1:0];
    initial begin
        for (i = 0; i < NUM_BRAMS_36k; i = i + 1) begin
            RAM_BASE_ADDR[i] = i * ((32768 * (1-full_width) + (36864 * full_width)) / DATA_WIDTH);
        end
    end
    
    // Now set the we and addr signals for each BRAM
    genvar l;
    generate
        for (l = 0; l < NUM_BRAMS_36k; l = l + 1) begin
            assign addr_36k[l] = addr - RAM_BASE_ADDR[l];
            if (l + 1 < NUM_BRAMS_36k) begin
                assign we_36k[l] = (we && (addr >= RAM_BASE_ADDR[l]) && (addr < RAM_BASE_ADDR[l+1])) ? {WE_WIDTH{1'b1}} : {WE_WIDTH{1'b0}};
                assign index_1hot[l] = (addr >= RAM_BASE_ADDR[l]) && (addr < RAM_BASE_ADDR[l+1]);
            end else begin
                assign we_36k[l] = (we && (addr >= RAM_BASE_ADDR[l])) ? {WE_WIDTH{1'b1}} : {WE_WIDTH{1'b0}};
                assign index_1hot[l] = (addr >= RAM_BASE_ADDR[l]);
            end
        end
    endgenerate

    //Create the block RAMs using VIVADO UNISIM library
    generate
        for (l = 0; l < NUM_BRAMS_36k; l = l + 1) begin : BRAM_SINGLE_MACRO_instances
            BRAM_SINGLE_MACRO #(
                .BRAM_SIZE("36Kb"), // Target BRAM, "18Kb" or "36Kb"
                .DEVICE("7SERIES"), // Target Device: "7SERIES"
                .DO_REG(0), // Optional output register (0 or 1)
                .INIT(72'h000000000), // Initial values on output port
                .INIT_FILE ("NONE"),
                .WRITE_WIDTH(DATA_WIDTH), // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
                .READ_WIDTH(DATA_WIDTH),  // Valid values are 1-72 (37-72 only valid when BRAM_SIZE="36Kb")
                .SRVAL(72'h000000000), // Set/Reset value for port output
                .WRITE_MODE("WRITE_FIRST") // "WRITE_FIRST", "READ_FIRST", or "NO_CHANGE"
            ) BRAM_SINGLE_MACRO_inst (
                .DO(data_out_36k[l]),       // Output data, width defined by READ_WIDTH parameter
                .ADDR(addr_36k[l]),   // Input address, width defined by read/write port depth
                .CLK(clk),     // 1-bit input clock
                .DI(data_in),       // Input data port, width defined by WRITE_WIDTH parameter
                .EN(1'b1),       // 1-bit input RAM enable
                .REGCE(1'b0), // 1-bit input output register enable (not needed if DO_REG=0)
                .RST(!rst),     // 1-bit input reset
                .WE(we_36k[l])        // Input write enable, width defined by write port depth
            );
        end
    endgenerate


    // Create a priority encoder to select the correct BRAM 
    // (turns a 1-hot signals into a single index of the high bit)
    wire [clog2(NUM_BRAMS_36k)-1:0] index;
    priority_encoder #(
        .N(NUM_BRAMS_36k),
        .INDEX_WIDTH(clog2(NUM_BRAMS_36k))
    ) pe (
        .one_hot_vector(index_1hot),
        .index(index)
    );

    // Clock index signal to avoid wrong BRAM selection
    reg [clog2(NUM_BRAMS_36k)-1:0] index_reg;
    always @(posedge clk) begin
        if (!rst) begin
            index_reg <= 0;
        end else begin
            index_reg <= index;
        end
    end

    // Create a multiplexer to select the correct output
    assign data_out = data_out_36k[index_reg];
`endif

endmodule
