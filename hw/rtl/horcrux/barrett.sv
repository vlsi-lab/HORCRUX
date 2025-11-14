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

module barrett (
    input  logic signed [31:0]        a,
    input  logic signed [31:0]        b,          // currently unused (reserved for R=2^32 variants)
    input  horcrux_pkg::horcrux_insn  insn_i,
    output logic signed [31:0]        result
);


    // ------------------ Kyber Barrett constant ------------------
    // v = ((1 << 26) + Q/2) / Q  with Q=3329  => v = 20159
    localparam logic signed [31:0] KYBER_BARRETT_V = 32'sd20159;
    localparam logic signed [31:0] Q_kyber         = 32'sd3329;
    localparam logic signed [31:0] Q_kyber_half    = 32'sd1665;


    logic signed [31:0] t1, t2, t3, t4, t5, t6, t7, t8, t9;
    logic signed [15:0] a16;
    logic signed [31:0] ta, tb, tc;
    logic signed [15:0] tb16;
    logic signed [63:0] tb64;
    logic [31:0] tu;
    logic signed [31:0] m;
    logic signed [63:0] m64;
    logic signed [31:0] z, z2;

    logic signed [63:0] a64;


    always_comb begin

        unique case (insn_i)

            horcrux_pkg::barrett_kyber: begin 
                t1 = a;              // 1·a
                t2 = a << 1;         // 2·a
                t3 = a << 2;         // 4·a
                t4 = a << 3;         // 8·a
                t5 = a << 5;         // 32·a
                t6 = a << 7;         // 128·a
                t7 = a << 8;         // 256·a
                t8 = a << 9;         // 512·a
                t9 = a << 12;        // 4096·a
                ta = t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9;
                tb = ta >>> 24;      // floor(ta / 2^24)
                m = (tb << 11) + (tb << 10) + (tb << 8) + tb;
                z = a - m;
                if (z >= Q_kyber_half)
                    z = z - Q_kyber;

                result = z;
            end

            default: begin
                a16 = '0;
                a64 = '0;
                t1 = '0;
                t2 = '0;
                t3 = '0;
                t4 = '0;
                t5 = '0;
                t6 = '0;
                t7 = '0;
                t8 = '0;
                t9 = '0;
                ta = '0;
                tb = '0;
                tb16 = '0;
                tb64 = '0;
                tu = '0;
                tc = '0;
                m = '0;
                z = '0;
                result   = '0;
            end

        endcase

    end

endmodule