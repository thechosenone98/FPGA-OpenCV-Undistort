module bram_to_axi_stream #(
    parameter n = 24,
    parameter i = 1,
    parameter d = 1,
    parameter u = 1,
    parameter pixel_per_clk = 8,
    parameter num_brams = 1,
    parameter addr_width = 14
) ( 
    input wire ACLK,
    input wire rst,
    // AXI Signals
    output wire [(8*n-1):0] m_TDATA,
    output wire [(d-1):0] m_TDEST,
    output wire [(i-1):0] m_TID,
    output wire [(n-1):0] m_TKEEP,
    output reg [0:0] m_TLAST,
    input wire m_TREADY,
    output reg [0:0] m_TVALID,
    output wire [(n-1):0] m_TSTRB,
    output reg [(u-1):0] m_TUSER,
    // BRAM Signals
    input wire [num_brams * (pixel_per_clk*8) - 1:0] bram_data_out,
    output wire [num_brams-1:0] bram_we,
    output wire [num_brams * addr_width - 1:0] bram_addr,
    output wire [num_brams * (pixel_per_clk*8) - 1:0] bram_data_in,
    input wire transfer_ready,
    output reg transfer_done_irq
    // DEBUG SIGNALS (add comma above if you uncomment this)
    // output wire [2:0] fsm_state,
    // output wire [7:0] col_counter_output
);


    // Assign the data on bram_data_out to every third byte of s_TDATA
    genvar b;
    for (b = 0; b < pixel_per_clk; b = b + 1) begin
        assign m_TDATA[(b*3)*8 +: 8] = bram_data_out[(b*8) +: 8];
        // Assign two previous byte to 0
        assign m_TDATA[(b*3+1)*8 +: 8] = 8'h00;
        assign m_TDATA[(b*3+2)*8 +: 8] = 8'h80;
    end

    assign bram_we = 0;
    assign bram_data_in = 0;
    assign m_TSTRB = 0;
    assign m_TDEST = 0;
    assign m_TID = 0;
    assign m_TKEEP = ~0;

    localparam IDLE = 3'b000;
    localparam START_TRANSACTION = 3'b001;
    localparam TRANSFER = 3'b010;
    localparam DECREMENT = 3'b011;
    localparam WAIT_FOR_READY = 3'b100;
    localparam DONE = 3'b101;
    localparam [2:0] state_t = {IDLE, START_TRANSACTION, TRANSFER, DECREMENT, WAIT_FOR_READY, DONE};
    reg [2:0] current_state;
    reg [2:0] next_state;
    reg [31:0] counter;
    reg [7:0] col_counter;
    reg already_decremented;
    reg [2:0] tready_counter;

    // Debug signals
    // assign fsm_state = current_state;
    // assign col_counter_output = col_counter;


    assign bram_addr = counter;

    always @(posedge ACLK or negedge rst) begin
        if (!rst) begin
            current_state <= IDLE;
        end else begin
            current_state <= next_state;
        end
    end

    always @(*) begin
        case (current_state)
            IDLE: begin
                if (transfer_ready) begin
                    next_state = START_TRANSACTION;
                end else begin
                    next_state = IDLE;
                end
            end
            START_TRANSACTION: begin
                if (m_TREADY) begin
                    next_state = TRANSFER;
                end else begin
                    next_state = START_TRANSACTION;
                end
            end
            TRANSFER: begin
                if (counter == 9600 + 1) begin
                    next_state = DONE;
                end else if (m_TREADY || already_decremented) begin
                    next_state = TRANSFER;
                end else begin
                    next_state = DECREMENT;
                end
            end
            DECREMENT: begin
                next_state = WAIT_FOR_READY;
            end
            WAIT_FOR_READY: begin
                if (m_TREADY) begin
                    next_state = TRANSFER;
                end else begin
                    next_state = WAIT_FOR_READY;
                end
            end
            DONE: begin
                next_state = IDLE;
            end
            default: begin
                next_state = IDLE;
            end
        endcase
    end

    always @(*) begin
        case (current_state)
            IDLE: begin
                m_TVALID = 1'b0;
                m_TUSER = 1'b0;
                m_TLAST = 1'b0;
                transfer_done_irq = 1'b0;
            end
            START_TRANSACTION: begin
                m_TVALID = 1'b1;
                m_TUSER = 1'b1;
                m_TLAST = 1'b0;
                transfer_done_irq = 1'b0;
            end
            TRANSFER: begin
                if(next_state == DECREMENT) begin
                    m_TVALID = 1'b0;
                end else begin
                    m_TVALID = 1'b1;
                end
                m_TUSER = 1'b0;
                if (col_counter == 40 - 1 && counter != 0) begin
                    m_TLAST = 1'b1;
                end else begin
                    m_TLAST = 1'b0;
                end
                transfer_done_irq = 1'b0;
            end
            DECREMENT: begin
                m_TVALID = 1'b0;
                m_TUSER = 1'b0;
                m_TLAST = 1'b0;
                transfer_done_irq = 1'b0;
            end
            WAIT_FOR_READY: begin
                m_TVALID = 1'b0;
                m_TUSER = 1'b0;
                m_TLAST = 1'b0;
                transfer_done_irq = 1'b0;
            end
            DONE: begin
                m_TVALID = 1'b0;
                m_TUSER = 1'b0;
                m_TLAST = 1'b0;
                transfer_done_irq = 1'b1;
            end
            default: begin
                m_TVALID = 1'b0;
                m_TUSER = 1'b0;
                m_TLAST = 1'b0;
                transfer_done_irq = 1'b0;
            end
        endcase
    end

    always @(posedge ACLK or negedge rst) begin
        if (!rst) begin
            counter <= 0;
            already_decremented <= 0;
            tready_counter <= 0;
        end else if (next_state != IDLE) begin
            if (m_TREADY && (current_state == TRANSFER || next_state == TRANSFER || next_state == START_TRANSACTION)) begin
                counter <= counter + 1;
                tready_counter <= tready_counter + 1;
                if(tready_counter >= 1) begin
                    already_decremented <= 0;
                    tready_counter <= 0;
                end
            end else if (next_state == DECREMENT) begin
                counter <= counter - 1;
                already_decremented <= 1;
                tready_counter <= 0;
            end
        end else begin
            counter <= 0;
            already_decremented <= 0;
            tready_counter <= 0;
        end
    end

    always @(posedge ACLK or negedge rst) begin
        if (!rst) begin
            col_counter <= 0;
        end else if (current_state != IDLE) begin
            if (m_TREADY && (current_state == TRANSFER || current_state == START_TRANSACTION)) begin
                if (col_counter == 40 - 1) begin
                    col_counter <= 0;
                end else begin
                    col_counter <= col_counter + 1;
                end
            end
        end else begin
            col_counter <= 0;
        end
    end

endmodule