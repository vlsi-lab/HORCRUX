///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: commit_stage.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

module commit_stage
  import horcrux_pkg::*;
  (
    input logic clk_i,
    input logic rst_ni,

    input logic [4:0] rd_i,
    input logic [3:0] id_i,
    input horcrux_pkg::out_t result_i,
    input logic done_i,
    
    cv32e40px_if_xif.coproc_result xif_result_if

);


  //always @(present_state) begin
  always_comb begin
    xif_result_if.result_valid = '0;
    xif_result_if.result.we = '0;
    xif_result_if.result.rd = '0;
    xif_result_if.result.id = '0;

    xif_result_if.result.ecsdata = '0;
    xif_result_if.result.exc = '0;
    xif_result_if.result.ecswe = '0;
    xif_result_if.result.exccode = '0;
    xif_result_if.result.err = '0;
    xif_result_if.result.dbg = '0;
    xif_result_if.result.data = result_i.rd1;

    if (done_i) begin
        xif_result_if.result_valid = '1;
        xif_result_if.result.we = done_i;
        xif_result_if.result.rd = rd_i;
        xif_result_if.result.id = id_i;    
    end
  end

endmodule
