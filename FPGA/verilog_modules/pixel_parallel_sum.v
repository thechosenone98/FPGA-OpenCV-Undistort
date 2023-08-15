// Precision required: 0,0000000511
// Number of bit with fixed point to achieve precision: 24

module pixel_parallel_sum (
    input wire [63:0] in_data,
    input wire ADDSUB,
    input wire CE,
    input wire CLK,
    input wire LOAD,
    input wire RST,
    output wire [24:0] total_sum_out
);

wire [21:0] macc_results[7:0];
wire [8:0] extended_inputs[7:0];
genvar i;

// Extend the inputs to 9 bits
generate
    for (i = 0; i < 8; i = i + 1) begin
        assign extended_inputs[i] = {1'b0, in_data[(i+1)*8-1 -: 8]};
    end
endgenerate

`ifdef SIMULATION
reg [21:0] accumulator[7:0];
`endif

generate
    for (i = 0; i < 8; i = i + 1) begin : macc_loop
        // Instantiate MACC_MACRO for synthesis
`ifndef SIMULATION
        MACC_MACRO #(
            .DEVICE("7SERIES"),
            .LATENCY(1),
            .WIDTH_A(9),
            .WIDTH_B(2),
            .WIDTH_P(22)
        ) macc_inst (
            .P(macc_results[i]),
            .A(extended_inputs[i]),
            .ADDSUB(ADDSUB),
            .B(2'b01),
            .CARRYIN(1'b0),
            .CE(CE),
            .CLK(CLK),
            .LOAD(LOAD),
            .LOAD_DATA(22'h000000),
            .RST(RST)
        );
        // Instantiate simulation model for simulation
`else
        always @(posedge CLK) begin
            if (RST) begin
                accumulator[i] <= 22'h0;
            end else if (CE) begin
                if (LOAD) begin
                    accumulator[i] <= 22'h0;
                end else begin
                    accumulator[i] <= ADDSUB ? (accumulator[i] + in_data[(i+1)*8-1 -: 8]) : (accumulator[i] - in_data[(i+1)*8-1 -: 8]);
                end
            end
        end
        assign macc_results[i] = accumulator[i];
`endif
    end
endgenerate

// Calculate the total sum
assign total_sum_out = macc_results[0] + macc_results[1] + macc_results[2] + macc_results[3] +
                       macc_results[4] + macc_results[5] + macc_results[6] + macc_results[7];

endmodule

