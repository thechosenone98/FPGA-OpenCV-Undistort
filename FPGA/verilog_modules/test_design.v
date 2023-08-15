module counter (
  input clk,
  input load,
  input up_down,
  input [31:0] load_value,
  output reg[31:0] count,
  output reg interrupt
);

  reg[31:0] load_reg;

  always @(posedge clk) begin
    if (load) begin
      load_reg <= load_value;
      count <= load_reg;
    end else if (up_down) begin
      count <= count + 32'h1;
    end else begin
      count <= count - 32'h1;
    end
    
    if (count == 32'hffffffff) begin
      interrupt <= 1;
    end else begin
      interrupt <= 0;
    end
  end

endmodule