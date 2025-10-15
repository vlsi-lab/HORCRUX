///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: bcop32.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

module bcop32 ( 
    input  wire [31:0] a,  
    input  wire [31:0] b,  
    input  wire [31:0] c, 
    output reg  [31:0] result
);

    always @(*) begin
        result = a ^ ((~b) & c);  // result = a ^ ((~b) & c)
    end

endmodule
