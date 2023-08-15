`timescale 1ns / 1ps

module axi_image_dissector #(
    parameter integer n = 24, // Default n value
    parameter integer i = 1, // Default i value
    parameter integer d = 1, // Default d value
    parameter integer u = 1, // Default u value
    parameter integer pixel_per_clk = 8, // Number of pixels per clock
    parameter integer num_brams = 1, // Number of BRAM instances
    parameter integer addr_width = 14 // Address width of BRAM
) (
    input wire ACLK,
    input wire ARESETn,
    input wire TVALID,
    output reg TREADY,
    input wire [(8*n-1):0] TDATA,
    input wire [(n-1):0] TSTRB,
    input wire [(n-1):0] TKEEP,
    input wire TLAST,
    input wire [(i-1):0] TID,
    input wire [(d-1):0] TDEST,
    input wire [(u-1):0] TUSER,
    //All BRAM connections (use arrays for each BRAM)
    input wire [num_brams-1:0] bram_we,
    input wire [num_brams * addr_width - 1:0] bram_addr,
    input wire [num_brams * (pixel_per_clk*8) - 1:0] bram_data_in,
    output wire [num_brams * (pixel_per_clk*8) - 1:0] bram_data_out
);

    // State machine states
    localparam IDLE = 1'b0;
    localparam DATA = 1'b1;
    localparam [0:0] state_t = {IDLE, DATA};
    reg [0:0] current_state, next_state;
    // Counter
    reg [(addr_width-1):0] counter [0:num_brams-1];

    // Bus select for BRAM
    reg [num_brams-1:0] bus_select;
    wire [num_brams * (pixel_per_clk*8)-1:0] bram_data_in_bus;
    wire [num_brams * (pixel_per_clk*8)-1:0] bram_data_out_bus;
    wire [num_brams * addr_width-1:0] bram_addr_bus;
    wire [num_brams-1:0] bram_we_bus;
    wire [((pixel_per_clk*8)-1):0] bram_data_in_local_bus;
    wire [((pixel_per_clk*8)-1):0] bram_data_out_local_bus;
    reg bram_we_local;
    genvar bus;
    generate
        for(bus = 0; bus < num_brams; bus = bus + 1) begin: bram_bus_multiplexer
            assign bram_data_in_bus[bus * (pixel_per_clk * 8) +: pixel_per_clk * 8] = bus_select[bus] ? bram_data_in_local_bus : bram_data_in[bus * (pixel_per_clk * 8) +: pixel_per_clk * 8];
            assign bram_data_out_bus[bus * (pixel_per_clk * 8) +: pixel_per_clk * 8] = bus_select[bus] ? bram_data_out_local_bus : bram_data_out[bus * (pixel_per_clk * 8) +: pixel_per_clk * 8];
            assign bram_addr_bus[bus * addr_width +: addr_width] = bus_select[bus] ? counter[bus] : bram_addr[bus * addr_width +: addr_width];
            assign bram_we_bus[bus] = bus_select[bus] ? bram_we_local : bram_we[bus];
        end
    endgenerate

    // Generate single port BRAM instances and connect them to the interface
    integer x;
    genvar k;
    generate
        for (k = 0; k < num_brams; k = k + 1) begin: bram_instances
            my_single_port_bram #(
                .ADDR_WIDTH(addr_width),
                .DATA_WIDTH(pixel_per_clk*8)
            ) bram_inst (
                .clk(ACLK),
                .we(bram_we_bus[k]),
                .addr(bram_addr_bus[k * addr_width +: addr_width]),
                .data_in(bram_data_in_bus[k * (pixel_per_clk * 8) +: pixel_per_clk * 8]),
                .data_out(bram_data_out_bus[k * (pixel_per_clk * 8) +: pixel_per_clk * 8])
            );
        end
    endgenerate

    //Data register
    wire [pixel_per_clk*8-1:0] bytes;
    // Assign every third byte of TDATA to bytes using a for loop
    genvar b;
    for (b = 0; b < pixel_per_clk; b = b + 1) begin
        assign bytes[(b+1)*8-1:b*8] = TDATA[(b*3)*8 +: 8];
    end

    assign bram_data_in_local_bus = bytes;

    // State machine process 1: State register
    always @(posedge ACLK or negedge ARESETn) begin
        if (!ARESETn) begin
            current_state <= IDLE;
        end else begin
            current_state <= next_state;
        end
    end

    // State machine process 2: Next state logic
    always @(*) begin
        case (current_state)
            IDLE: begin
                if (TVALID) begin
                    next_state = TLAST ? IDLE : DATA;
                end else begin
                    next_state = IDLE;
                end
            end
            DATA: begin
                if (TVALID) begin
                    next_state = TLAST ? IDLE : DATA;
                end else begin
                    next_state = DATA;
                end
            end
            default: begin
                next_state = IDLE;
            end
        endcase
    end

    // State machine process 3: Output logic
    always @(*) begin
        case (current_state)
            IDLE: begin
                TREADY = 1'b1;
                bram_we_local = 1'b0;
                for (x = 0; x < num_brams; x = x + 1) begin
                    bus_select[x] = 1'b0;
                end
            end
            DATA: begin
                TREADY = 1'b1;
                bram_we_local = 1'b1;
                for (x = 0; x < num_brams; x = x + 1) begin
                    bus_select[x] = 1'b1;
                    if (TUSER == 1'b1) begin
                        counter[x] <= 0;
                    end else begin
                        bram_addr[x * addr_width +: addr_width] = counter[x];
                        counter[x] <= counter[x] + 1;
                    end
                end
            end
            default: begin
                TREADY = 1'b0;
                bram_we_local = 1'b0;
                for (x = 0; x < num_brams; x = x + 1) begin
                    bus_select[x] = 1'b0;
                end
            end
        endcase
    end

endmodule