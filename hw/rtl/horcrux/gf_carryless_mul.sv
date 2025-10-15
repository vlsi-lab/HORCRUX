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
    input  horcrux_pkg::horcrux_insn insn_i,
    output logic [31:0] gf_carryless_mul_o
);

    logic [7:0] a,b; 
    logic [15:0] u[0:3];
    logic [15:0] g1, g2, g3, g4;
    logic [15:0] tmp2[0:3];
    logic [15:0] tmp1;

    assign a = gf_carryless_mul_i_0[7:0];
    assign b = gf_carryless_mul_i_1[7:0];

     always_comb begin
        u[0] = 16'h0000; // Equivalent to u[0] = 0;
        u[1] = b & ((1 << 7) - 1); // Equivalent to u[1] = b & ((1UL << 7) - 1UL);
        u[2] = u[1] << 1; // Equivalent to u[2] = u[1] << 1;
        u[3] = u[2] ^ u[1]; // Equivalent to u[3] = u[2] ^ u[1];
    end

    always_comb begin
        case(insn_i)
            horcrux_pkg:: gf_carryless_mul_1: tmp1 = a & 8'h3;
            horcrux_pkg:: gf_carryless_mul_2: tmp1 = (a >> 2) & 8'h3;
            horcrux_pkg:: gf_carryless_mul_3: tmp1 = (a >> 4) & 8'h3;
            horcrux_pkg:: gf_carryless_mul_4: tmp1 = (a >> 6) & 8'h3;
            default: tmp1 = '0;  // Handle the default case if needed
        endcase
    end

    always_comb begin
        tmp2[0] = tmp1;
        g1 = u[0] & -(1 - ((tmp2[0] | -tmp2[0]) >> 31)); 
        
        tmp2[1] = tmp1 - 1;
        g2 = u[1] & -(1 - ((tmp2[1] | -tmp2[1]) >> 31)); 
        
        tmp2[2] = tmp1 -2 ;
        g3 = u[2] & -(1 - ((tmp2[2] | -tmp2[2]) >> 31)); 
        
        tmp2[3] = tmp1 -3;
        g4 = u[3] & -(1 - ((tmp2[3] | -tmp2[3]) >> 31)); 

    end

    assign gf_carryless_mul_o  = g1 ^ g2 ^  g3 ^ g4;


endmodule