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

  function automatic logic [31:0] iso(input logic en, input logic [31:0] x);
    return en ? x : 32'h0; 
  endfunction

  logic en_bcop32, en_rol32, en_cbd, en_karats, en_gfclmul, en_rej, en_montg, en_compress, en_mat, en_matG;
  //logic en_barrett;

  always_comb begin
    en_bcop32   = (insn_i == bcop32);
    en_rol32    = (insn_i == keccak_rol32_h) || (insn_i == keccak_rol32_l);
    en_cbd      = (insn_i inside {cbd_eta1, cbd_eta2, cbd_eta3, cbd_eta4});
    en_karats   = (insn_i inside {karats_1, karats_2, karats_3, karats_4});
    en_gfclmul  = (insn_i == gf_carryless_mul);
    en_rej      = (insn_i inside {rej12x2, rej23x1, rej8x4});
    en_compress = (insn_i inside {compress1, compress2, compress3, compress4});
    //en_barrett  = (insn_i == barrett_kyber);
    en_montg    = (insn_i inside {montg_kyber, montg_newhope, montg_falcon, montg_ntru, montg_dilithium});
    en_mat      = (insn_i inside {vec_by_matrix, restr_vec_by_matrix, double_zero_norm, synd_minus_vec_scaled, vec_by_vec_pointwise});
    en_matG     = (insn_i inside {restr_to_val_G, restr_vec_by_matrix_G, vec_by_matrix_G, synd_minus_vec_scaled_G});
  end


  //--------always-present signals declarations ----------------------------------------------------------------------------
  horcrux_pkg::out_t out;
  logic [31:0] a1, b1, c1;

  //-------- input declarations ---------------------------------------------------------------------------
  assign a1 = rs_values_i.rs1_0;
  assign b1 = rs_values_i.rs2_0;
  assign c1 = rs_values_i.rs3_0;

  //-------- blocks instantiations ---------------------------------------------------------------------------
  //----bcop32---------------------
  logic [31:0] a_bcop;
  assign a_bcop = iso(en_bcop32, a1);
  logic [31:0] b_bcop;
  assign b_bcop = iso(en_bcop32, b1);
  logic [31:0] c_bcop;
  assign c_bcop = iso(en_bcop32, c1);

  logic [31:0] bcop32_result;
  bcop32 bcop32_inst (
    .a(a_bcop),
    .b(b_bcop),
    .c(c_bcop),
    .result(bcop32_result)
  );

  //----rol32---------------------
  logic [31:0] a_rol_lo;
  assign a_rol_lo  = iso(en_rol32, a1);
  logic [31:0] b_rol_hi;
  assign b_rol_hi  = iso(en_rol32, b1);
  logic [31:0] c_rol_off;
  assign c_rol_off = iso(en_rol32, c1);
  logic [31:0] rol32_1_result, rol32_2_result;

  rol32 rol32_inst (
      .high(b_rol_hi),
      .low(a_rol_lo),
      .offset(c_rol_off),
      .result1(rol32_1_result),
      .result2(rol32_2_result)
  );

  //----cbd-eta---------------------
  logic [31:0] a_cbd_eta;
  assign a_cbd_eta  = iso(en_cbd, a1);
  logic [31:0] b_cbd_eta;
  assign b_cbd_eta  = iso(en_cbd, b1);
  logic [31:0] cbd_eta_result;

  cbd_eta cbd_eta_inst (
    .d_i(a_cbd_eta),
    .j_i(b_cbd_eta),
    .insn_i(insn_i),
    .cbd_o(cbd_eta_result)
  );


  //----karatsuba---------------------
  logic [31:0] a_karatsuba;
  assign a_karatsuba  = iso(en_karats, a1);
  logic [31:0] b_karatsuba;
  assign b_karatsuba  = iso(en_karats, b1);
  logic [31:0] karatsuba_result1, karatsuba_result2;

  karats karatsuba_inst (
    .clk_i(clk_i),
    .rst_ni(rst_ni),
    .karatsuba1_i(a_karatsuba),
    .karatsuba2_i(b_karatsuba),
    .insn_i(insn_i),
    .result1(karatsuba_result1),
    .result2(karatsuba_result2)
  );

  //----gf_carryless---------------------
  logic [31:0] a_gf_carryless;
  assign a_gf_carryless  = iso(en_gfclmul, a1);
  logic [31:0] b_gf_carryless;
  assign b_gf_carryless  = iso(en_gfclmul, b1);
  logic [31:0] c_gf_carryless;
  assign c_gf_carryless  = iso(en_gfclmul, c1);
  logic [31:0] gf_carryless_mul_result;

  gf_carryless_mul gf_carryless_mul_inst (
    .gf_carryless_mul_i_0(a_gf_carryless),
    .gf_carryless_mul_i_1(b_gf_carryless),
    .gf_carryless_mul_i_2(c_gf_carryless),
    .insn_i(insn_i),
    .gf_carryless_mul_o(gf_carryless_mul_result)
  );


  //----rej---------------------
  logic [31:0] a_rej;
  assign a_rej  = iso(en_rej, a1);
  logic [31:0] b_rej;
  assign b_rej  = iso(en_rej, b1);
  logic [31:0] c_rej;
  assign c_rej  = iso(en_rej, c1);
  logic [31:0] rej_fixed_result;

  rej_fixed rej_fixed_inst (
    .d_i(a_rej),
    .j_i(b_rej),
    .m_i(c_rej),
    .insn_i(insn_i),
    .rd_o(rej_fixed_result)
  );


  // --- MONTGOMERY ---
  logic [31:0] a_montg, b_montg;
  assign a_montg = iso(en_montg, a1);
  assign b_montg = iso(en_montg, b1);

  logic [31:0] montg_result;
  montg montg_inst (
    .a      (a_montg),
    .b      (b_montg),
    .insn_i (insn_i),
    .result (montg_result)
  );

  // --- barrett ---
  //logic [31:0] a_barrett, b_barrett;
  //assign a_barrett = iso(en_barrett, a1);
  //assign b_barrett = iso(en_barrett, b1);
  //logic [31:0] barrett_result;
  //barrett barrett_inst (
  //  .a      (a_barrett),
  //  .b      (b_barrett),
  //  .insn_i (insn_i),
  //  .result (barrett_result)
  //);

  // --- COMPRESS ---
  logic [31:0] a_compress, b_compress;
  assign a_compress = iso(en_compress, a1);
  assign b_compress = iso(en_compress, b1);

  logic [31:0] compress_result;
  compress compress_inst (
    .a      (a_compress),
    .b      (b_compress),
    .insn_i (insn_i),
    .result (compress_result)
  );

  // --- MATRIX (RSDP path) ---
  logic [31:0] a_mat32, b_mat32, c_mat32;
  assign a_mat32 = iso(en_mat, a1);
  assign b_mat32 = iso(en_mat, b1);
  assign c_mat32 = iso(en_mat, c1);

  logic [31:0] matrix_result;
  matrix_unit matrix_unit_inst (
    .rs1    (a_mat32[15:0]),  // e[i]
    .rs2    (b_mat32[15:0]),  // V_tr[i][j]
    .rs3    (c_mat32[15:0]),  // res[i][j]
    .insn_i (insn_i),
    .result (matrix_result) // computed_res[i][j]
  );

  // --- MATRIX_G (P=509 path) ---
  logic [31:0] a_matG32, b_matG32, c_matG32;
  assign a_matG32 = iso(en_matG, a1);
  assign b_matG32 = iso(en_matG, b1);
  assign c_matG32 = iso(en_matG, c1);

  logic [31:0] matrix_G_result;
  matrix_unit_G matrix_unit_G_inst (
    .rs1    (a_matG32[15:0]),  // e[i]
    .rs2    (b_matG32[15:0]),  // V_tr[i][j]
    .rs3    (c_matG32[15:0]),  // res[i][j]
    .insn_i (insn_i),
    .result (matrix_G_result) // computed_res[i][j]
  );


  //-------- output assignment ---------------------------------------------------------------------------

  always_comb begin
    out.rd1 = '0;  // fills all bits with zero
    
    case (insn_i)
      nada: begin out.rd1 = '0;  end
      //Keccak-instructions
      keccak_rol32_h: begin  out.rd1 = rol32_1_result; end
      keccak_rol32_l: begin  out.rd1 = rol32_2_result; end
      bcop32:         begin  out.rd1 = bcop32_result; end
      //GF-instructions
      karats_1, karats_2, karats_3, karats_4:
                      begin  out.rd1 = karatsuba_result2; end
      gf_carryless_mul:
                      begin  out.rd1 = gf_carryless_mul_result; end
      // Sampling-instructions
      cbd_eta1, cbd_eta2, cbd_eta3, cbd_eta4:
                      begin  out.rd1 = cbd_eta_result; end
      rej12x2, rej23x1, rej8x4:    begin  out.rd1 = rej_fixed_result; end


      // Modular Reduction-instructions
      montg_kyber, montg_newhope, montg_falcon, montg_ntru, montg_dilithium: 
                      begin out.rd1 = montg_result; end
      //barrett_kyber: begin out.rd1 = barrett_result; end

      vec_by_matrix, restr_vec_by_matrix, double_zero_norm, synd_minus_vec_scaled, vec_by_vec_pointwise:
                      begin out.rd1 = matrix_result; end
      restr_to_val_G, restr_vec_by_matrix_G, vec_by_matrix_G, synd_minus_vec_scaled_G:
                      begin out.rd1 = matrix_G_result; end

      // Compress variants
      compress1, compress2, compress3, compress4:
                      begin  out.rd1 = compress_result; end

      default:        begin  out.rd1 = '0; end
    endcase
  end

  assign rd_values_o.rd1 = out.rd1;


endmodule