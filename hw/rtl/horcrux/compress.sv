///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: compress.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

module compress (
    input  logic signed [31:0]        a,
    input  logic signed [31:0]        b,
    input  horcrux_pkg::horcrux_insn  insn_i,
    output logic signed [31:0]        result
);

    // Moduli
    localparam logic signed [31:0] Q_kyber     = 32'd3329;
    localparam logic signed [31:0] Q_dilithium = 32'd8380417;
    
    logic signed [31:0] u;
    logic signed [31:0] d0, d1;
    logic signed [63:0] d2;
    logic signed [63:0] t0, t1, t2, t3, t4, t5, t6, t7, t8, t9,
                        t10, t11, t12, t13, t14, t15;
    logic signed [31:0] d3, z;


    assign u = a + ((a >> 15) & Q_kyber);

    always_comb begin
        // Precompute multiples of a
        unique case(insn_i)

            horcrux_pkg::compress1: begin
                d0 = u << 4;
            end
            horcrux_pkg::compress2: begin
                d0 = u << 5;
            end
            horcrux_pkg::compress3: begin
                d0 = u << 10;
            end
            horcrux_pkg::compress4: begin
                d0 = u << 11;
            end
            default: begin
                d0 = 32'd0; // Default case, no operation
            end
        endcase
    end

    always_comb begin
        // Precompute multiples of a
        unique case(insn_i)
            horcrux_pkg::compress1, horcrux_pkg:: compress3: begin
                d1 = d0 + 1665;
            end
            horcrux_pkg::compress2, horcrux_pkg::compress4: begin
                d1 = d0 + 1664;
            end
            default: begin
                d1 = 32'd0; // Default case, no operation
            end
        endcase
    end

    always_comb begin
        // Precompute multiples of a
        unique case(insn_i)

            horcrux_pkg::compress1: begin
                t0  = d1;              
                t1  = d1 << 1;         
                t2  = d1 << 3;         
                t3  = d1 << 4;         
                t4  = d1 << 5;         
                t5  = d1 << 6;         
                t7  = d1 << 7;         
                t8  = d1 << 9;         
                t9  = d1 << 11;
                t10 = d1 << 12;
                t11 = d1 << 13;
                t12 = d1 << 16;
                t13 = '0;
                t14 = '0;
                t15 = '0;
            end
            horcrux_pkg::compress2: begin
                t0  = d1 << 1;              
                t1  = d1 << 2;         
                t2  = d1 << 3;         
                t3  = d1 << 4;         
                t4  = d1 << 5;         
                t5  = d1 << 6;         
                t7  = d1 << 8;         
                t8  = d1 << 10;         
                t9  = d1 << 11;
                t10 = d1 << 12;
                t11 = d1 << 15;
                t12 = '0;
                t13 = '0;
                t14 = '0;
                t15 = '0;
            end
            horcrux_pkg::compress3: begin
                t0  = d1;              
                t1  = d1 << 1;         
                t2  = d1 << 2;         
                t3  = d1 << 4;         
                t4  = d1 << 5;         
                t5  = d1 << 7;         
                t7  = d1 << 8;         
                t8  = d1 << 9;         
                t9  = d1 << 10;
                t10 = d1 << 11;
                t11 = d1 << 13;
                t12 = d1 << 15;
                t13 = d1 << 16;
                t14 = d1 << 17;
                t15 = d1 << 20;
            end
            horcrux_pkg::compress4: begin
                t0  = d1 << 2;              
                t1  = d1 << 3;         
                t2  = d1 << 4;         
                t3  = d1 << 6;         
                t4  = d1 << 7;         
                t5  = d1 << 8;         
                t7  = d1 << 9;         
                t8  = d1 << 10;         
                t9  = d1 << 12;
                t10 = d1 << 14;
                t11 = d1 << 15;
                t12 = d1 << 16;
                t13 = d1 << 19;
                t14 = '0;
                t15 = '0;
            end
            default: begin
                t0 = 32'd0;              
                t1 = 32'd0;         
                t2 = 32'd0;         
                t3 = 32'd0;         
                t4 = 32'd0;         
                t5 = 32'd0;         
                t7 = 32'd0;         
                t8 = 32'd0;         
                t9 = 32'd0;
                t10 = 32'd0;
                t11 = 32'd0;
                t12 = 32'd0;
                t13 = 32'd0;
                t14 = 32'd0;
                t15 = 32'd0;
            end
        endcase
    end

    assign d2 = t0 + t1 + t2 + t3 + t4 + t5 + t7 + t8 + t9 + t10 + t11 + t12 + t13 + t14 + t15;

    always_comb begin
        // Precompute multiples of a
        unique case(insn_i)

            horcrux_pkg::compress1: begin
                d3 = d2 >>> 28;
                z = d3 & 32'h0f;
            end
            horcrux_pkg::compress2: begin
                d3 = d2 >>> 27;
                z = d3 & 32'h1f;
            end
            horcrux_pkg::compress3: begin
                d3 = d2 >>> 32;
                z = d3 & 32'h3ff;
            end
            horcrux_pkg::compress4: begin
                d3 = d2 >>> 31;
                z = d3 & 32'h7ff;
            end
            default: begin
                d3 = 32'd0; // Default case, no operation
                z  = 32'd0; // Default case, no operation
            end
        endcase
    end


    assign result = z;

endmodule
