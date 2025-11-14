///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: montg.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025 2025
//
///////////////////////////////////////////////////////////////////////////////////

module montg (
    input  logic signed [31:0]        a,
    input  logic signed [31:0]        b,          // currently unused (reserved for R=2^32 variants)
    input  horcrux_pkg::horcrux_insn  insn_i,
    output logic signed [31:0]        result
);

    // ------------------ Moduli (Q) ------------------
    localparam logic signed [31:0] Q_kyber     = 32'sd3329;
    localparam logic signed [31:0] Q_newhope   = 32'sd12289;
    localparam logic signed [31:0] Q_falcon    = 32'sd12289;
    localparam logic signed [31:0] Q_ntru      = 32'sd4591;

    localparam logic [31:0] Q_dilithium    = 32'd8380417;


    // ------------------ QINV (R = 2^16) ------------------
    localparam logic signed [15:0] QINV_kyber   = 16'shF301; // -3327  ≡ 62209 mod 2^16
    localparam logic signed [15:0] QINV_newhope = 16'sd12287; //  12287 ≡ -q^{-1} mod 2^16
    localparam logic signed [15:0] QINV_falcon  = 16'sd12287; //  NewHope (q=12289)
    localparam logic signed [15:0] QINV_ntru    = 16'shC2F1;  // -15631 ≡ 49905 mod 2^16

    localparam logic signed [31:0] QINV_dilithium = 32'd58728449;



    logic signed [15:0] a16;
    logic signed [63:0] a64;
    logic signed [15:0] t16;
    logic signed [31:0] q_sel;
    logic signed [15:0] qinv_sel;
    logic signed [31:0] qinv_sel_32;
    logic signed [31:0] prod;

    // For Barrett (64-bit intermediates)
    logic signed [63:0] prod64, tQ64;
    logic signed [63:0] temp64;
    logic signed [63:0] diff;

    always_comb begin

        a16 = a[15:0]; // (int16_t)a

        unique case (insn_i)

            horcrux_pkg::montg_kyber: begin
                q_sel    = Q_kyber;
                qinv_sel = QINV_kyber;
                qinv_sel_32 = 32'sd0;  
            end

            horcrux_pkg::montg_newhope: begin
                q_sel    = Q_newhope;
                qinv_sel = QINV_newhope;
                qinv_sel_32 = 32'sd0;
            end

            horcrux_pkg::montg_falcon: begin
                q_sel    = Q_falcon;
                qinv_sel = QINV_falcon;
                qinv_sel_32 = 32'sd0;
            end

            horcrux_pkg::montg_ntru: begin
                q_sel    = Q_ntru;
                qinv_sel = QINV_ntru;
                qinv_sel_32 = 32'sd0;
            end

            horcrux_pkg::montg_dilithium: begin
                q_sel    = Q_dilithium;
                qinv_sel_32 = QINV_dilithium;
                qinv_sel    = 16'sd0;
            end

            default: begin
                // NOP / 0
                q_sel       = 32'sd0;
                qinv_sel    = 16'sd0;
                qinv_sel_32 = 32'sd0;
   
            end
        endcase
    end

    always_comb begin

        unique case (insn_i)

            horcrux_pkg::montg_kyber, horcrux_pkg::montg_newhope, horcrux_pkg::montg_falcon, horcrux_pkg::montg_ntru: begin
                prod = $signed(a16) * $signed(qinv_sel);
                t16 = prod[15:0];
                result = ($signed(a) - $signed({{16{t16[15]}}, t16}) * $signed(q_sel)) >>> 16;
                a64      = 64'sd0;
                prod64   = 64'sd0;
                temp64   = 64'sd0;
                tQ64     = 64'sd0;
                diff      = 64'sd0;
            end

            horcrux_pkg::montg_dilithium: begin
                a64    = { $signed(b), $unsigned(a) };
                prod64 = $signed(a) * qinv_sel_32;
                temp64 = {{32{prod64[31]}}, prod64[31:0]};
                tQ64   = temp64 * q_sel;
                diff   = a64 - tQ64;
                result = diff[63:32];
                prod     = 32'sd0;
                t16      = 16'sd0;

            end

            default: begin
                prod     = 32'sd0;
                a64      = 64'sd0;
                t16      = 16'sd0;
                prod64   = 64'sd0;
                temp64   = 64'sd0;
                diff     = 64'sd0;
                result   = '0;
            end

        endcase

    end

endmodule