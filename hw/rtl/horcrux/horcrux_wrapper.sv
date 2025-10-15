///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: horcrux_wrapper.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////


module horcrux_wrapper 
  import cv32e40px_pkg::*;
  import cv32e40px_core_v_xif_pkg::*;
(
    input logic clk_i,
    input logic rst_ni,

    // eXtension interface
    cv32e40px_if_xif.coproc_compressed xif_compressed_if,
    cv32e40px_if_xif.coproc_issue      xif_issue_if,
    cv32e40px_if_xif.coproc_commit     xif_commit_if,
    cv32e40px_if_xif.coproc_mem        xif_mem_if,
    cv32e40px_if_xif.coproc_mem_result xif_mem_result_if,
    cv32e40px_if_xif.coproc_result     xif_result_if
);


    horcrux_top horcrux_top_i(
    .clk_i (clk_i),
    .rst_ni(rst_ni),

    .xif_compressed_if,
    .xif_issue_if,
    .xif_commit_if,
    .xif_mem_if,
    .xif_mem_result_if,
    .xif_result_if
  );

endmodule

