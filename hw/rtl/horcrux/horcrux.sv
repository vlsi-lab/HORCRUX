///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: horcrux.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

module horcrux
  import cv32e40px_core_v_xif_pkg::*;
  import horcrux_pkg::*;
(
    input logic clk_i,
    input logic rst_ni,

    input horcrux_pkg::in_t rs_values_i,
    input horcrux_pkg::horcrux_insn insn_i,
    output horcrux_pkg::out_t rd_values_o
);

  //--------always-present signals declarations ----------------------------------------------------------------------------
  horcrux_pkg::out_t out;
  logic [31:0] a1, b1, c1;

  //-------- input declarations ---------------------------------------------------------------------------
  assign a1 = rs_values_i.rs1_0;
  assign b1 = rs_values_i.rs2_0;
  assign c1 = rs_values_i.rs3_0;

  //-------- blocks instantiations ---------------------------------------------------------------------------
  logic [31:0] bcop32_result;
  bcop32 bcop32_inst (
    .a(a1),
    .b(b1),
    .c(c1),
    .result(bcop32_result)
  );

  logic [31:0] rol32_1_result, rol32_2_result;
  rol32 rol32_inst (
      .high(b1),
      .low(a1),
      .offset(c1),
      .result1(rol32_1_result),
      .result2(rol32_2_result)
  );

  logic [31:0] karatsuba_result1, karatsuba_result2;
  karats karatsuba_inst (
    .clk_i(clk_i),
    .rst_ni(rst_ni),
    .karatsuba1_i(a1),
    .karatsuba2_i(b1),
    .insn_i(insn_i),
    .result1(karatsuba_result1),
    .result2(karatsuba_result2)
  );

  logic [31:0] gf_carryless_mul_result;
  gf_carryless_mul gf_carryless_mul_inst (
    .gf_carryless_mul_i_0(a1),
    .gf_carryless_mul_i_1(b1),
    .insn_i(insn_i),
    .gf_carryless_mul_o(gf_carryless_mul_result)
  );

  logic [31:0] modular_reduction_result;
  modular_reduction modular_reduction_inst (
    .a(a1),
    .b(b1),
    .insn_i(insn_i),
    .result(modular_reduction_result)
  );

  logic [31:0] cbd_result;
  cbd cbd_inst (
    .cbd1_i(a1),
    .cbd2_i(b1),
    .insn_i(insn_i),
    .cbd_o(cbd_result)
  );


  logic [31:0] compress_result;
  compress compress_inst (
    .a(a1),
    .b(b1),
    .insn_i(insn_i),
    .result(compress_result)
  );


  logic [31:0] rej_uniform_result, temp_rej_uniform;
  //assign temp_rej_uniform  = a1 | (b1 << 8) | (c1 << 16);
  assign temp_rej_uniform = (32'(a1) & 32'hFF)
      | ((32'(b1) & 32'hFF) << 8)
      | ((32'(c1) & 32'hFF) << 16);
  assign rej_uniform_result = temp_rej_uniform & 32'h007FFFFF;



  //-------- output assignment ---------------------------------------------------------------------------

  always_comb begin
    out.rd1 = '0;  // fills all bits with zero
    
    case (insn_i)
      nada: begin out.rd1 = '0;  end
      keccak_rol32_h: begin  out.rd1 = rol32_1_result; end
      keccak_rol32_l: begin  out.rd1 = rol32_2_result; end
      bcop32:         begin  out.rd1 = bcop32_result; end
      karats_1, karats_2, karats_3, karats_4:
                      begin  out.rd1 = karatsuba_result2; end
      gf_carryless_mul_1, gf_carryless_mul_2,
      gf_carryless_mul_3, gf_carryless_mul_4:
                      begin  out.rd1 = gf_carryless_mul_result; end
      barrett_k, barrett_d,
      montg_k, montg_d:
                      begin  out.rd1 = modular_reduction_result; end
      // CBD variants
      cbd3_1, cbd3_2, cbd3_3, cbd3_4,
      cbd2_1, cbd2_2, cbd2_3, cbd2_4,
      cbd2_5, cbd2_6, cbd2_7, cbd2_8:
                      begin  out.rd1 = cbd_result; end
      compress1, compress2, compress3, compress4:
                      begin  out.rd1 = compress_result; end
      rej_uniform: begin  out.rd1 = rej_uniform_result; end 
      default:        begin  out.rd1 = '0; end
    endcase
  end

  assign rd_values_o.rd1 = out.rd1;


endmodule