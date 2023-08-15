module axi_image_dissector_v2 #(
    parameter integer n = 24, // Default n value
    parameter integer i = 1, // Default i value
    parameter integer d = 1, // Default d value
    parameter integer u = 1, // Default u value
    parameter integer pixel_per_clk = 8, // Number of pixels per clock
    parameter integer num_brams = 1, // Number of BRAM instances
    parameter integer addr_width = 14, // Address width of BRAM
    parameter integer NUM_BYTES = 76800 // Number of bytes in images
) (
    input wire rst,
    input wire ACLK,
    input wire [(8*n-1):0] s_TDATA,
    input wire [(d-1):0] s_TDEST,
    input wire [(i-1):0] s_TID,
    input wire [(n-1):0] s_TKEEP,
    input wire [0:0] s_TLAST,
    output reg s_TREADY,
    input wire [(n-1):0] s_TSTRB,
    input wire [(u-1):0] s_TUSER,
    input wire s_TVALID,
    //All BRAM connections (use arrays for each BRAM)
    input wire bram_we,
    input wire odd_pixel,
    input wire odd_row,
    input wire [(addr_width - 2)*4 - 1:0] bram_addr,
    input wire [(pixel_per_clk*8) - 1:0] bram_data_in,
    output reg [256 - 1:0] bram_data_out,
    // Communication signal with Image Transfer module
    output wire transfer_ready,
    input wire transfer_done,
    output wire interrupt
    // DEBUG SIGNALS (add comma above if you uncomment this)
    // output wire framebuffer_select_out,
    // output wire [1:0] ready_images_out,
    // output wire completed_early_out,
    // output wire primed_out,
    // output wire [8:0] row_count,
    // output wire [8:0] col_count,
    // output wire [15:0] counter_odd_out,
    // output wire [15:0] counter_even_out,
    // output wire [3:0] we,
    // output wire [31:0] counter_out,
    // output wire [1:0] fsm_state,
    // output wire [256 - 1:0] bram_data_out_local_debug
);

    // State machine states variables
    localparam IDLE = 2'b00;
    localparam DATA = 2'b01;
    localparam DONE = 2'b10;
    localparam [1:0] state_t = {IDLE, DATA, DONE};
    reg [1:0] current_state, next_state;
    reg [31:0] counter;
    reg [15:0] counter_even, counter_odd;
    reg [1:0] ready_images;
    reg finished_reading_image;
    reg completed_early;
    wire [15:0] counters [1:0];
    reg count_odd_toggle, count_even_toggle;
    reg [8:0] row_count_reg;
    reg [8:0] col_count_reg;
    wire [13:0] addr_div_40;

    assign counters[0] = counter_odd;
    assign counters[1] = counter_even;

    //BRAM parameters
    localparam NUM_BYTES_PER_BRAM = NUM_BYTES / 4; // Odd-Even col/row BRAMs
    localparam ADDR_WIDTH_PER_BRAM = addr_width - 2;

    // Iteration variables
    integer x;

    // First framebuffer control signals
    wire [(pixel_per_clk*8)-1:0] bram_data_in_bus_first_framebuffer [3:0];
    wire [ADDR_WIDTH_PER_BRAM-1:0] bram_addr_bus_first_framebuffer [3:0];
    wire [0:0] bram_we_bus_first_framebuffer [3:0];
    // Second framebuffer control signals
    wire [(pixel_per_clk*8)-1:0] bram_data_in_bus_second_framebuffer [3:0];
    wire [ADDR_WIDTH_PER_BRAM-1:0] bram_addr_bus_second_framebuffer [3:0];
    wire [0:0] bram_we_bus_second_framebuffer [3:0];
    reg frame_buffer_select;
    reg can_write;
    reg primed;
    // Local BRAM signal that should be directed to either the first or second block of BRAMs framebuffers
    reg [((pixel_per_clk*8)-1):0] bram_data_in_local_bus [3:0];
    wire [ADDR_WIDTH_PER_BRAM-1:0] bram_addr_local_bus;
    reg [0:0] bram_we_local [3:0];
    wire [256 - 1:0] bram_data_out_local_first_framebuffer;
    wire [256 - 1:0] bram_data_out_local_second_framebuffer;
    wire [256 - 1:0] bram_data_out_local;

    // Debug signals
    // assign fsm_state = current_state;
    // assign counter_out = counter;
    // assign we = {bram_we_local[0], bram_we_local[1], bram_we_local[2], bram_we_local[3]};
    // assign bram_data_out_local_debug = bram_data_out_local;
    // assign counter_odd_out = counter_odd;
    // assign counter_even_out = counter_even;
    // assign row_count = row_count_reg;
    // assign col_count = col_count_reg;
    // DEBUG FRAMBUFFER SELECT
    // assign primed_out = primed;
    // assign completed_early_out = completed_early;
    // assign ready_images_out = ready_images;
    // assign framebuffer_select_out = frame_buffer_select;

    // Assign counter to address bus
    assign bram_addr_local_bus = (row_count_reg[0] == 1'b0 && counter[0] == 1'b0) || (row_count_reg[0] == 1'b0 && counter[0] == 1'b1) ? counter_odd : counter_even;
    
    genvar j;
    generate
        for(j = 0; j < 4; j = j + 1) begin: bus_addr_select_generate
            assign bram_addr_bus_first_framebuffer[j] = frame_buffer_select ? bram_addr_local_bus : bram_addr[(ADDR_WIDTH_PER_BRAM * 4) - (ADDR_WIDTH_PER_BRAM * j) - 1-:ADDR_WIDTH_PER_BRAM];
            assign bram_addr_bus_second_framebuffer[j] = frame_buffer_select ? bram_addr[(ADDR_WIDTH_PER_BRAM * 4) - (ADDR_WIDTH_PER_BRAM * j) - 1-:ADDR_WIDTH_PER_BRAM] : bram_addr_local_bus;
        end
        for(j = 0; j < 4; j = j + 1) begin: bus_data_select_generate
            assign bram_data_in_bus_first_framebuffer[j] = frame_buffer_select ? bram_data_in_local_bus[j] : bram_data_in;
            assign bram_data_in_bus_second_framebuffer[j] = frame_buffer_select ? bram_data_in : bram_data_in_local_bus[j];
        end
        for(j = 0; j < 4; j = j + 1) begin: bus_we_select_generate
            assign bram_we_bus_first_framebuffer[j] = frame_buffer_select ? bram_we_local[j] : bram_we;
            assign bram_we_bus_second_framebuffer[j] = frame_buffer_select ? bram_we : bram_we_local[j];
        end
    endgenerate
    

    // Instantiate the first framebuffer BRAMs
    genvar k;
    generate
        //      Row/Col      Row/Col       Row/Col       Row/Col
        // 0 -> Odd/Odd 1 -> Odd/Even 2 -> Even/Odd 3 -> Even/Even
        for (k = 0; k < 4; k = k + 1) begin: bram_instances_first_framebuffer
            my_single_port_bram #(
                .ADDR_WIDTH(ADDR_WIDTH_PER_BRAM),
                .NUM_BYTES(NUM_BYTES_PER_BRAM),
                .DATA_WIDTH(pixel_per_clk*8)
            ) bram_inst (
                .clk(ACLK),
                .rst(rst),
                .we(bram_we_bus_first_framebuffer[k]),
                .addr(bram_addr_bus_first_framebuffer[k]),
                .data_in(bram_data_in_bus_first_framebuffer[k]),
                .data_out(bram_data_out_local_first_framebuffer[256 - k*64 - 1 -: 64])
            );
        end
    endgenerate

    // Instantiate the second framebuffer BRAMs
    generate
        //      Row/Col      Row/Col       Row/Col       Row/Col
        // 0 -> Odd/Odd 1 -> Odd/Even 2 -> Even/Odd 3 -> Even/Even
        for (k = 0; k < 4; k = k + 1) begin: bram_instances_second_framebuffer
            my_single_port_bram #(
                .ADDR_WIDTH(ADDR_WIDTH_PER_BRAM),
                .NUM_BYTES(NUM_BYTES_PER_BRAM),
                .DATA_WIDTH(pixel_per_clk*8)
            ) bram_inst (
                .clk(ACLK),
                .rst(rst),
                .we(bram_we_bus_second_framebuffer[k]),
                .addr(bram_addr_bus_second_framebuffer[k]),
                .data_in(bram_data_in_bus_second_framebuffer[k]),
                .data_out(bram_data_out_local_second_framebuffer[256 - k*64 - 1 -: 64])
            );
        end
    endgenerate

    assign bram_data_out_local = frame_buffer_select ? bram_data_out_local_second_framebuffer : bram_data_out_local_first_framebuffer;

    reg [ADDR_WIDTH_PER_BRAM-1:0] prev_addr_oo, prev_addr_oe, prev_addr_eo, prev_addr_ee;
    reg prev_odd_pixel, prev_odd_row;
    always @(posedge ACLK or negedge rst) begin
        if(!rst) begin
            prev_addr_oo <= 0;
            prev_addr_oe <= 0;
            prev_addr_eo <= 0;
            prev_addr_ee <= 0;
            prev_odd_pixel <= 0;
            prev_odd_row <= 0;
        end else begin
            prev_addr_oo <= bram_addr[(ADDR_WIDTH_PER_BRAM * 4)-1 -: ADDR_WIDTH_PER_BRAM];
            prev_addr_oe <= bram_addr[(ADDR_WIDTH_PER_BRAM * 3)-1 -: ADDR_WIDTH_PER_BRAM];
            prev_addr_eo <= bram_addr[(ADDR_WIDTH_PER_BRAM * 2)-1 -: ADDR_WIDTH_PER_BRAM];
            prev_addr_ee <= bram_addr[(ADDR_WIDTH_PER_BRAM * 1)-1 -: ADDR_WIDTH_PER_BRAM];
            prev_odd_pixel <= odd_pixel;
            prev_odd_row <= odd_row;
        end
    end

    always @(*) begin
        if (prev_odd_pixel && prev_odd_row) begin
            // OO-OE-EO-EE
            bram_data_out = {prev_addr_oo < 2400 ? bram_data_out_local[255-:64] : 64'b0, prev_addr_oe < 2400 ? bram_data_out_local[191-:64] : 64'b0, prev_addr_eo < 2400 ? bram_data_out_local[127-:64] : 64'b0, prev_addr_ee < 2400 ? bram_data_out_local[63-:64] : 64'b0};
        end else if (!prev_odd_pixel && prev_odd_row) begin
            // OE-OO-EE-EO
            bram_data_out = {prev_addr_oe < 2400 ? bram_data_out_local[191-:64] : 64'b0, prev_addr_oo < 2400 ? bram_data_out_local[255-:64] : 64'b0, prev_addr_ee < 2400 ? bram_data_out_local[63-:64] : 64'b0, prev_addr_eo < 2400 ? bram_data_out_local[127-:64] : 64'b0};
        end else if (prev_odd_pixel && !prev_odd_row) begin
            // EO-EE-OO-OE
            bram_data_out = {prev_addr_eo < 2400 ? bram_data_out_local[127-:64] : 64'b0, prev_addr_ee < 2400 ? bram_data_out_local[63-:64] : 64'b0, prev_addr_oo < 2400 ? bram_data_out_local[255-:64] : 64'b0, prev_addr_oe < 2400 ? bram_data_out_local[191-:64] : 64'b0};
        end else if (!prev_odd_pixel && !prev_odd_row) begin
            // EE-EO-OE-OO
            bram_data_out = {prev_addr_ee < 2400 ? bram_data_out_local[63-:64] : 64'b0, prev_addr_eo < 2400 ? bram_data_out_local[127-:64] : 64'b0, prev_addr_oe < 2400 ? bram_data_out_local[191-:64] : 64'b0, prev_addr_oo < 2400 ? bram_data_out_local[255-:64] : 64'b0};
        end else begin
            bram_data_out = 256'b0;
        end
    end

    // Take correct data from TDATA bus
    // Data register
    wire [pixel_per_clk*8-1:0] bytes;
    // Assign every third byte of TDATA to bytes using a for loop
    genvar b;
    for (b = 0; b < pixel_per_clk; b = b + 1) begin
        // THIS FLIPS THE BYTE ORDER (REMEMBER TO FLIP IT BACK WHEN SENDING IT TO THE WRITE IP TO DRAM)
        // aka 8, 7, 6, 5, 4, 3, 2, 1, 0 -> 0, 1, 2, 3, 4, 5, 6, 7, 8 (more understandable for coordinates)
        assign bytes[b*8 +: 8] = s_TDATA[((pixel_per_clk - b - 1)*3)*8 +: 8];
    end

    // STATE MACHINE START
    // State machine process 1: State register
    always @(posedge ACLK or negedge rst) begin
        if (!rst) begin
            current_state <= IDLE;
        end else begin
            current_state <= next_state;
        end
    end
    // State machine process 2: Next state logic
    always @(*) begin
        case (current_state)
            IDLE: begin
                if (s_TVALID) begin
                    next_state = s_TLAST ? IDLE : DATA;
                end else begin
                    next_state = IDLE;
                end
            end
            DATA: begin
                if (counter == 9600 - 1) begin
                    next_state = DONE;
                end else if (s_TVALID) begin
                    next_state = s_TLAST ? IDLE : DATA;
                end else begin
                    next_state = DATA;
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

    // State machine process 3: Output logic
    always @(*) begin
        case (current_state)
            IDLE: begin
                s_TREADY = 1'b1;

                // BRAM ADDRESSING LOGIC
                if(s_TVALID && s_TREADY) begin
                    // Assign the correct byte to the correct BRAM along with we signals
                    //(checking for last bit equal 0 because we start counting at 0)
                    if(row_count_reg[0] == 1'b0 && counter[0] == 1'b0) begin
                        bram_data_in_local_bus[0] = bytes;
                        bram_data_in_local_bus[1] = 0;
                        bram_data_in_local_bus[2] = 0;
                        bram_data_in_local_bus[3] = 0;
                        bram_we_local[0] = 1'b1;
                        bram_we_local[1] = 1'b0;
                        bram_we_local[2] = 1'b0;
                        bram_we_local[3] = 1'b0;
                    end else if (row_count_reg[0] == 1'b0 && counter[0] == 1'b1) begin
                        bram_data_in_local_bus[0] = 0;
                        bram_data_in_local_bus[1] = bytes;
                        bram_data_in_local_bus[2] = 0;
                        bram_data_in_local_bus[3] = 0;
                        bram_we_local[0] = 1'b0;
                        bram_we_local[1] = 1'b1;
                        bram_we_local[2] = 1'b0;
                        bram_we_local[3] = 1'b0;
                    end else if (row_count_reg[0] == 1'b1 && counter[0] == 1'b0) begin
                        bram_data_in_local_bus[0] = 0;
                        bram_data_in_local_bus[1] = 0;
                        bram_data_in_local_bus[2] = bytes;
                        bram_data_in_local_bus[3] = 0;
                        bram_we_local[0] = 1'b0;
                        bram_we_local[1] = 1'b0;
                        bram_we_local[2] = 1'b1;
                        bram_we_local[3] = 1'b0;
                    end else if (row_count_reg[0] == 1'b1 && counter[0] == 1'b1) begin
                        bram_data_in_local_bus[0] = 0;
                        bram_data_in_local_bus[1] = 0;
                        bram_data_in_local_bus[2] = 0;
                        bram_data_in_local_bus[3] = bytes;
                        bram_we_local[0] = 1'b0;
                        bram_we_local[1] = 1'b0;
                        bram_we_local[2] = 1'b0;
                        bram_we_local[3] = 1'b1;
                    end else begin
                        // Impossible but avoids latch being inferred
                        bram_data_in_local_bus[0] = 0;
                        bram_data_in_local_bus[1] = 0;
                        bram_data_in_local_bus[2] = 0;
                        bram_data_in_local_bus[3] = 0;    
                        bram_we_local[0] = 1'b0;
                        bram_we_local[1] = 1'b0;
                        bram_we_local[2] = 1'b0;
                        bram_we_local[3] = 1'b0;
                    end
                end else begin
                    bram_data_in_local_bus[0] = 0;
                    bram_data_in_local_bus[1] = 0;
                    bram_data_in_local_bus[2] = 0;
                    bram_data_in_local_bus[3] = 0;    
                    bram_we_local[0] = 1'b0;
                    bram_we_local[1] = 1'b0;
                    bram_we_local[2] = 1'b0;
                    bram_we_local[3] = 1'b0;
                end
            end
            DATA: begin
                s_TREADY = 1'b1;
                // Assign the correct byte to the correct BRAM along with we signals
                //(checking for last bit equal 0 because we start counting at 0)
                if(row_count_reg[0] == 1'b0 && counter[0] == 1'b0) begin
                    bram_data_in_local_bus[0] = bytes;
                    bram_data_in_local_bus[1] = 0;
                    bram_data_in_local_bus[2] = 0;
                    bram_data_in_local_bus[3] = 0;
                    bram_we_local[0] = 1'b1;
                    bram_we_local[1] = 1'b0;
                    bram_we_local[2] = 1'b0;
                    bram_we_local[3] = 1'b0;
                end else if (row_count_reg[0] == 1'b0 && counter[0] == 1'b1) begin
                    bram_data_in_local_bus[0] = 0;
                    bram_data_in_local_bus[1] = bytes;
                    bram_data_in_local_bus[2] = 0;
                    bram_data_in_local_bus[3] = 0;
                    bram_we_local[0] = 1'b0;
                    bram_we_local[1] = 1'b1;
                    bram_we_local[2] = 1'b0;
                    bram_we_local[3] = 1'b0;
                end else if (row_count_reg[0] == 1'b1 && counter[0] == 1'b0) begin
                    bram_data_in_local_bus[0] = 0;
                    bram_data_in_local_bus[1] = 0;
                    bram_data_in_local_bus[2] = bytes;
                    bram_data_in_local_bus[3] = 0;
                    bram_we_local[0] = 1'b0;
                    bram_we_local[1] = 1'b0;
                    bram_we_local[2] = 1'b1;
                    bram_we_local[3] = 1'b0;
                end else if (row_count_reg[0] == 1'b1 && counter[0] == 1'b1) begin
                    bram_data_in_local_bus[0] = 0;
                    bram_data_in_local_bus[1] = 0;
                    bram_data_in_local_bus[2] = 0;
                    bram_data_in_local_bus[3] = bytes;
                    bram_we_local[0] = 1'b0;
                    bram_we_local[1] = 1'b0;
                    bram_we_local[2] = 1'b0;
                    bram_we_local[3] = 1'b1;
                end else begin
                    // Impossible but avoids latch being inferred by compiler
                    bram_data_in_local_bus[0] = 0;
                    bram_data_in_local_bus[1] = 0;
                    bram_data_in_local_bus[2] = 0;
                    bram_data_in_local_bus[3] = 0;    
                    bram_we_local[0] = 1'b0;
                    bram_we_local[1] = 1'b0;
                    bram_we_local[2] = 1'b0;
                    bram_we_local[3] = 1'b0;
                end
            end
            DONE: begin
                s_TREADY = 1'b0;
                bram_we_local[0] = 1'b0;
                bram_we_local[1] = 1'b0;
                bram_we_local[2] = 1'b0;
                bram_we_local[3] = 1'b0;
                bram_data_in_local_bus[0] = 0;
                bram_data_in_local_bus[1] = 0;
                bram_data_in_local_bus[2] = 0;
                bram_data_in_local_bus[3] = 0;
            end
            default: begin
                s_TREADY = 1'b0;
                bram_we_local[0] = 1'b0;
                bram_we_local[1] = 1'b0;
                bram_we_local[2] = 1'b0;
                bram_we_local[3] = 1'b0;
                bram_data_in_local_bus[0] = 0;
                bram_data_in_local_bus[1] = 0;
                bram_data_in_local_bus[2] = 0;
                bram_data_in_local_bus[3] = 0;
            end
        endcase
    end
    // STATE MACHINE END

    // Counter control for BRAM address
    always @(posedge ACLK or negedge rst) begin
        if (!rst) begin
            counter <= 0;
            col_count_reg <= 0;
            row_count_reg <= 0;
            counter_odd <= 0;
            counter_even <= 0;
            count_odd_toggle <= 0;
            count_even_toggle <= 0;
        // s_TUSER is used by the previous block to indicate start of an image
        // so we use it as our reset signal in IDLE state
        end else if(current_state == IDLE && s_TUSER && !s_TVALID) begin
            counter <= 0;
            col_count_reg <= 0;
            row_count_reg <= 0;
            counter_odd <= 0;
            counter_even <= 0;
            count_odd_toggle <= 0;
            count_even_toggle <= 0;
        end else if (s_TVALID && s_TREADY && (next_state == DATA || current_state == DATA)) begin
            counter <= counter + 1;
            if((row_count_reg[0] == 1'b0 && counter[0] == 1'b1) && !count_odd_toggle) begin
                counter_odd <= counter_odd + 1;
                count_odd_toggle <= 1'b1;
            end else if((row_count_reg[0] == 1'b1 && counter[0] == 1'b1) && !count_even_toggle) begin
                counter_even <= counter_even + 1;
                count_even_toggle <= 1'b1;
            end
            if(col_count_reg[1] == 1'b1) begin
                count_odd_toggle <= 0;
                count_even_toggle <= 0;
            end
            if (s_TLAST) begin
                row_count_reg <= row_count_reg + 1;
                col_count_reg <= 0;
            end else begin
                col_count_reg <= col_count_reg + 1;
            end
        end
    end

    always @(posedge ACLK or negedge rst) begin
        if(!rst) begin
            finished_reading_image <= 1'b0;
        end else if(current_state == IDLE && s_TUSER) begin
            finished_reading_image <= 1'b0;
        end else if(current_state == DONE) begin
            finished_reading_image <= 1'b1;
        end
    end

    always @(posedge ACLK or negedge rst) begin
        if (!rst) begin
            primed <= 1'b0;
            can_write <= 1'b0;
            ready_images <= 0;
            frame_buffer_select <= 1'b0;
            completed_early <= 1'b0;
        end else if (current_state == DONE) begin
            ready_images <= ready_images + 1;
            if (completed_early) begin
                // Clear the flag
                completed_early <= 1'b0;
                // Continue as usual
                frame_buffer_select <= ~frame_buffer_select;
                can_write <= 1'b1;
            end else if (primed) begin
                can_write <= 1'b0;
            end else begin
                frame_buffer_select <= ~frame_buffer_select;
                can_write <= 1'b1;
                primed <= 1'b1;
            end
        end else if (transfer_done) begin
            ready_images <= ready_images - 1;
            if(current_state == IDLE && finished_reading_image) begin
                if (ready_images > 1) begin
                    frame_buffer_select <= ~frame_buffer_select;
                end else begin
                    // Reached zero images, so we can't switch the frame buffer select
                    // and need to go trough the priming process again on the next read frame
                    primed <= 1'b0;
                end
                can_write <= 1'b1;
            end else begin
                // This signal indicates that the processing of the image was faster than the reading of the next one
                // It is used to tell the state machine how to proceed when it finishes reading the new frame in.
                completed_early <= 1'b1;
            end
        end else if (can_write) begin
            can_write <= 1'b0;
        end
    end

    assign interrupt = can_write;
    assign transfer_ready = ready_images > 0 && !completed_early;

endmodule