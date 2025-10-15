///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: cadd.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

module caddq #(
  parameter logic [31:0] Q = 32'd8380417
)(
  input  logic signed [31:0] caddq_i,
  output logic signed [31:0] caddq_o
);

  // Replicate sign bit across all positions, mask with Q
  logic signed [31:0] sign_mask;

  assign caddq_o = caddq_i + ( {32{caddq_i[31]}}   // all-ones if negative, all-zeros if positive
                        & Q );


endmodule
