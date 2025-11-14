///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: gf_carryless_mul.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

module gf_carryless_mul(
    input  logic [31:0]  gf_carryless_mul_i_0,
    input  logic [31:0]  gf_carryless_mul_i_1,
    input  logic [31:0]  gf_carryless_mul_i_2,
    input  horcrux_pkg::horcrux_insn  insn_i,
    output logic [31:0] gf_carryless_mul_o
);

    logic [7:0] a,b;
    logic [2:0] c; 
    logic [15:0] u1, u2;
    logic [15:0] g0, g1, g2, g3;
    logic [1:0]  t0, t1, t2, t3;
    logic [15:0] l, h;
    logic [15:0] l_fix, h_fix;

    assign a = gf_carryless_mul_i_0[7:0];
    assign b = gf_carryless_mul_i_1[7:0];
    assign c = gf_carryless_mul_i_2[2:0];

    always_comb begin

        unique case(insn_i)

            horcrux_pkg::gf_carryless_mul: begin
                u1 = {9'b0, (b & 8'h7F)}; // zero-extend
                u2 = u1 << 1;

                t0 = a[1:0];
                g0 = ({16{t0[0]}} & u1) ^ ({16{t0[1]}} & u2);

                t1 = a[3:2];
                g1 = ({16{t1[0]}} & u1) ^ ({16{t1[1]}} & u2);

                t2 = a[5:4];
                g2 = ({16{t2[0]}} & u1) ^ ({16{t2[1]}} & u2);

                t3 = a[7:6];
                g3 = ({16{t3[0]}} & u1) ^ ({16{t3[1]}} & u2);

                // Assemble low and high parts
                l = (g0 << 0) ^ (g1 << 2) ^ (g2 << 4) ^ (g3 << 6);
                h = (g0 >> 8) ^ (g1 >> 6) ^ (g2 >> 4) ^ (g3 >> 2);

                // MSB(b) adjustment
                l_fix = l ^ ({8'b0, a} << 7 & {16{b[7]}});
                h_fix = h ^ (({8'b0, a} >> 1) & {16{b[7]}});
            end
            default: begin
                u1 = 16'h0000;
                u2 = 16'h0000;
                g0 = 16'h0000;
                g1 = 16'h0000;
                g2 = 16'h0000;
                g3 = 16'h0000;
                l = 16'h0000;
                h = 16'h0000;
                t0 = 2'h0;
                t1 = 2'h0;
                t2 = 2'h0;
                t3 = 2'h0;
                l_fix = 16'h0000;
                h_fix = 16'h0000;
            end

        endcase

    end

    assign gf_carryless_mul_o = {h_fix, l_fix};


endmodule

