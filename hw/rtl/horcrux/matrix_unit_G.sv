//////////////////////////////////////////////////////////////////////////////////////////
// Engineer:      Alessandra Dolmeta - alessandra.dolmeta@polito.it                     //
//                Valeria Piscopo    - valeria.piscopo@polito.it                        //
//                                                                                      //
// Design Name:    Tightly-coupled Accelerator Module                                   //
// File name:      matrix_unit_G.sv                                                       //
// Language:       SystemVerilog                                                        //
//                                                                                      //
// Description:    Performs all operation of fp_arith.h                                 //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

module matrix_unit_G
import horcrux_pkg::*;
(
    input  wire [15:0] rs1,    // e[i]
    input  wire [15:0] rs2,    // V_tr[i][j]
    input  wire [15:0] rs3,    // res[i][j]
    input  horcrux_insn insn_i,
    output reg  [31:0] result // computed_res[i][j]
);

    localparam logic [15:0] P  = 16'd509;
    localparam logic [31:0] M  = 32'h80C1_21B3; // 2160140723 (for >>40 multiply-high trick)

    function automatic logic [15:0] fpred_single32 (input logic [31:0] xin);
        logic [63:0] xin64, mprod, t, tP, r64;
        begin
            xin64 = {32'd0, xin};                 // zero-extend
            mprod = xin64 * M;                    // (uint64_t)xin * 2160140723
            t     = mprod >> 40;                  // >> 40
            tP    = t * P;                        // t * 509
            r64   = xin64 - tP;                   // x - t*P
            fpred_single32 = r64[15:0];          // cast down to FP_ELEM
        end
    endfunction

    function automatic logic [15:0] fpred_opposite32 (input logic [31:0] xin);
        logic [15:0] r;
        begin
            r = fpred_single32(xin);                // one 32×32→64 mul already exists
            // if r==0 → 0, else P - r
            fpred_opposite32 = (r == 16'd0) ? 16'd0 : (P - r);
        end
    endfunction


    //restr_to_val_G, restr_vec_by_fp_matrix_G, fp_vec_by_fp_matrix_G
    // Internal signals
    logic [15:0] a, b, c;

    logic [31:0] mul;
    logic [31:0] sum;
    logic [31:0] adder1;
    logic [15:0] tmp1, tmp2;
    logic [15:0] restr_to_val_G_res;

    logic [31:0] res1, res2, res3, res4;


    restr_to_val_G restr_to_val_G_insn (
        .x(rs1), 
        .res1(res1),
        .res2(res2),
        .res3(res3),
        .res4(res4)
    );

    logic [15:0] y;
    logic [31:0] prod12, prod34, mid32;
    logic [15:0] red12, red34;

    always_comb begin
        // products guaranteed < 2^32 per your note
        prod12 = res1 * res2;
        prod34 = res3 * res4;

        // FPRED_SINGLE on each product (intermediate reductions)
        red12  = fpred_single32(prod12);
        red34  = fpred_single32(prod34);

        // multiply reduced halves, then final FPRED_SINGLE
        mid32  = {16'd0, red12} * {16'd0, red34};   // 16x16 -> 32
        y      = fpred_single32(mid32);
    end

    assign restr_to_val_G_res = {16'b0, y};




    always @(*) begin
        case (insn_i)
            vec_by_matrix_G, synd_minus_vec_scaled_G: begin
                a = rs1; // e[i]        or s[i]
                b = rs2; // V_tr[i][j]  or chall_1
                c = rs3; // res[i][j]   or synd[i]
            end

            restr_vec_by_matrix_G: begin
                a = restr_to_val_G_res;   // RESTR_TO_VAL(e[i])
                b = rs2;              // V_tr[i][j] - u_prime
                c = rs3;              // res[i][j]  - chall_1
            end

            default: begin
                a = '0;
                b = '0;
                c = '0;
            end
        endcase
    end

    assign mul = a * b;

    assign adder1 =
        (insn_i == synd_minus_vec_scaled_G) ? fpred_opposite32(fpred_single32(mul)) : mul;

    assign sum = adder1 + {16'b0, c};


    always @(*) begin
        case (insn_i)
            restr_vec_by_matrix_G, vec_by_matrix_G, synd_minus_vec_scaled_G: begin
                tmp1 = '0;
                tmp2 = fpred_single32(sum);
            end

            restr_to_val_G: begin
                tmp1 = '0;
                tmp2 = restr_to_val_G_res;
            end
            default: begin
                tmp1 = 16'b0;
                tmp2 = 16'b0;
            end
        endcase

        result = {tmp1,tmp2};
    end

endmodule
