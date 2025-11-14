//////////////////////////////////////////////////////////////////////////////////////////
// Engineer:      Alessandra Dolmeta - alessandra.dolmeta@polito.it                     //
//                Valeria Piscopo    - valeria.piscopo@polito.it                        //
//                                                                                      //
// File name:      restr_to_val_G.sv                                                    //
// Language:       SystemVerilog                                                        //
//                                                                                      //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

module restr_to_val_G (
    input  logic [15:0] x,         
    output logic [31:0] res1,
    output logic [31:0] res2,
    output logic [31:0] res3,
    output logic [31:0] res4
);

    // ===== Constants (FP_ELEM == 16-bit) =====
    localparam logic [15:0] G1   = 16'd16;   // RESTR_G_GEN_1
    localparam logic [15:0] G2   = 16'd256;  // RESTR_G_GEN_2
    localparam logic [15:0] G4   = 16'd384;  // RESTR_G_GEN_4
    localparam logic [15:0] G8   = 16'd355;  // RESTR_G_GEN_8
    localparam logic [15:0] G16  = 16'd302;  // RESTR_G_GEN_16
    localparam logic [15:0] G32  = 16'd93;   // RESTR_G_GEN_32
    localparam logic [15:0] G64  = 16'd505;  // RESTR_G_GEN_64
    localparam logic [15:0] ONE  = 16'd1;

    // ===== Explicit shifts (x0..x6) and the LSB bits (b0..b6) =====
    logic [15:0] x0, x1, x2, x3, x4, x5, x6;
    logic        b0, b1, b2, b3, b4, b5, b6;

    always_comb begin
        x0 = (x >> 0);  b0 = x0[0];
        x1 = (x >> 1);  b1 = x1[0];
        x2 = (x >> 2);  b2 = x2[0];
        x3 = (x >> 3);  b3 = x3[0];
        x4 = (x >> 4);  b4 = x4[0];
        x5 = (x >> 5);  b5 = x5[0];
        x6 = (x >> 6);  b6 = x6[0];
    end

    // FP_ELEM_CMOV(bit, T, F) := (mask & T) | (~mask & F), mask = {16{bit}}
    logic [15:0] mask0, mask1, mask2, mask3, mask4, mask5, mask6;
    logic [15:0] sel0, sel1, sel2, sel3, sel4, sel5, sel6;

    always_comb begin
        mask0 = {16{b0}};
        mask1 = {16{b1}};
        mask2 = {16{b2}};
        mask3 = {16{b3}};
        mask4 = {16{b4}};
        mask5 = {16{b5}};
        mask6 = {16{b6}};

        sel0 = (mask0 & G1 ) | (~mask0 & ONE);
        sel1 = (mask1 & G2 ) | (~mask1 & ONE);
        sel2 = (mask2 & G4 ) | (~mask2 & ONE);
        sel3 = (mask3 & G8 ) | (~mask3 & ONE);
        sel4 = (mask4 & G16) | (~mask4 & ONE);
        sel5 = (mask5 & G32) | (~mask5 & ONE);
        sel6 = (mask6 & G64) | (~mask6 & ONE);
    end

    // ===== Partial products (32-bit) =====
    // Match C:
    //   res1 = sel0 * sel1; res2 = sel2 * sel3; res3 = sel4 * sel5; res4 = sel6;
    // Note: res4 is a single CMOV in C; we keep it 32-bit for uniformity.
    always_comb begin
        res1 = sel0 * sel1;         // up to ~ (505*384) < 2^19
        res2 = sel2 * sel3;
        res3 = sel4 * sel5;
        res4 = {16'd0, sel6};       // widen to 32-bit
    end


endmodule
