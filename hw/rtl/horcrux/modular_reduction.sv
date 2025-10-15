///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: modular_reduction.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: // Date: October 2025 2025
//
///////////////////////////////////////////////////////////////////////////////////

module modular_reduction (
    input  logic signed [31:0]        a,
    input  logic signed [31:0]        b,
    input  horcrux_pkg::horcrux_insn  insn_i,
    output logic signed [31:0]        result
);

    // Moduli
    localparam logic signed [31:0] Q_kyber     = 32'd3329;
    localparam logic signed [31:0] Q_dilithium = 32'd8380417;
    
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
        // Precompute multiples of a
        unique case(insn_i)

            horcrux_pkg::barrett_k: begin 
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
                if (z >= Q_kyber)
                    z = z - Q_kyber;
            end

            horcrux_pkg::barrett_d: begin 
                t1 = a >>> 23;       // floor(a / 2^23)
                t2 = (a >>> 22) & 1; // bit 22 of a
                tb = t1 + t2;
                m = (tb << 23) - (tb << 13) + tb;
                z = a - m;
                if (z >= Q_dilithium)
                    z = z - Q_dilithium;
            end

            horcrux_pkg::montg_k: begin 
                a16 = a & 16'hFFFF; // lower 16 bits of a
                t1 = a16;              // 1·a
                t2 = a16 << 8;         // 2^8·a
                t3 = a16 << 9;         // 2^9·a
                t4 = a16 << 12;        // 2^12·a
                t5 = a16 << 13;        // 2^13·a
                t6 = a16 << 14;        // 2^14·a
                t7 = a16 << 15;        // 2^15·a
                tb = t1 + t2 + t3 + t4 + t5 + t6 + t7;
                tb16 = tb & 16'hFFFF; // lower 16 bits of tb
                tu = {16'd0, tb16};     // zero-extend to 32 bits
                m = (tu << 11) + (tu << 10) + (tu << 8) + tu;

                z = (a - m) >>> 16;
            end

            horcrux_pkg::montg_d: begin 
                a64 = {b,a};
                t1 = a64;              // 1·a
                t2 = a64 << 13;        // 2^13·a
                t3 = a64 << 23;        // 2^23·a
                t4 = a64 << 24;        // 2^24·a
                t5 = a64 << 25;        // 2^25·a
                tb = t1 + t2 + t3 + t4 + t5;
                tb64 = {32'b0, tb};   // lower 32 bits of tb64
                m64 = (tb64 << 23) - (tb64 << 13) + tb64;

                z = (a64 - m64) >>> 32; // note: shift by 32 yields 0 for 32-bit
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
            end
        endcase
    end

    assign result = z;

endmodule
