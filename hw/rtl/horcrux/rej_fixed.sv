///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: rej_fixed.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
// Description:
//   Fixed-slice rejection sampler (RV32):
//   - d_i: 32-bit random word; m_i: 32-bit bound;
//   - decode (b,k) from insn_i (Kyber 12x2 by default);
//   - extract up to k lanes of b bits starting at base=j_i*b;
//   - for each lane: if cand < m_eff keep, else zero;
//   - pack accepted lanes at fixed positions (LSB-first).
//
///////////////////////////////////////////////////////////////////////////////////

module rej_fixed (
    input  logic [31:0] d_i,      // random bits for this step (RS1)
    input  logic [31:0] m_i,      // bound m (RS2)
    input  logic [31:0] j_i,      // slice index (which group in the stream word)
    input  horcrux_pkg::horcrux_insn insn_i,
    output logic [31:0] rd_o      // packed b-bit lanes (zeros where rejected)
);

  // -------- Decode (b,k) from opcode --------
  logic [4:0] b;   // lane width in bits (1..32)
  logic [2:0] k;   // lanes we try to extract (1..4)

  always_comb begin
    unique case (insn_i)
        horcrux_pkg::rej12x2: begin // Kyber: two 12-bit lanes in [11:0] and [23:12]
          b = 5'd12;
          k = 3'd2;
        end
        horcrux_pkg::rej23x1: begin // Dilithium: one 23-bit lane in RS1[22:0]
          b = 5'd23;
          k = 3'd1;
        end
        horcrux_pkg::rej8x4: begin // New profile: four 8-bit lanes in [7:0], [15:8], [23:16], [31:24]
            b = 5'd8;
            k = 3'd4;
        end
        // Add more fixed profiles here if you want (e.g., 16x2, 10x3, ...)
        default: begin
            b = 5'd12;
            k = 3'd2;   // safe default: Kyber profile
        end
    endcase
  end

  // -------- Sanitization & derived values --------
  logic [5:0]  b_eff;      // 1..32 (we'll keep 1..31 in practice)
  logic [2:0]  k_eff;      // capped to fit within 32-bit RS1
  logic [2:0]  k_max;      // max lanes that fit
  logic [31:0] lane_mask;
  logic [31:0] m_eff;
  logic [7:0]  base;

  logic [5:0]  j;

  assign j = j_i[5:0];

  always_comb begin : DERIVE
    // Sanitize b (b is 5-bit; it can never be > 31)
    if (b == 5'd0)  b_eff = 6'd1;
    else            b_eff = {1'b0, b};   // 1..31

    // Cap k to available lanes within a 32-bit word
    if (b_eff >= 6'd32)       k_max = 3'd1; // (won't happen with b<=31, kept for completeness)
    else if (b_eff >= 6'd16)  k_max = 3'd2;
    else if (b_eff >= 6'd11)  k_max = 3'd2;
    else if (b_eff >= 6'd08)  k_max = 3'd4;
    else                      k_max = 3'd4;

    if (k == 3'd0)       k_eff = 3'd1;
    else if (k > k_max)  k_eff = k_max;
    else                 k_eff = k;

    // Mask for b_eff bits; (b_eff==32) branch left for completeness
    if (b_eff == 6'd32)  lane_mask = 32'hFFFF_FFFF;
    else                 lane_mask = (32'h1 << b_eff) - 32'h1;

    // Compare only b-bit values
    m_eff = m_i & lane_mask;

    // base bit index for lane 0: j*b
    base = j * b_eff[5:0];
  end

  // -------- Core extraction & packing --------
  always_comb begin : BUILD
    // declare locals first (no init)
    int t;
    logic [7:0]  sh;
    logic [31:0] cand;
    logic        acc;
    logic [31:0] rd_next;
    logic [31:0] lane_val;

    rd_next = 32'b0;

    // lanes 0..k_eff-1
    for (t = 0; t < 4; t++) begin
      if (t < k_eff) begin
        sh = base + (t * b_eff);
        if (sh + b_eff <= 8'd32) begin
          cand = (d_i >> sh) & lane_mask;
          acc  = (cand < m_eff);
          if (acc) begin
            // place accepted candidate at lane position t (LSB-first)
            lane_val = cand << (t * b_eff);
            rd_next  = rd_next | lane_val;
          end
        end
        // else: overrun -> lane contributes zeros
      end
    end

    rd_o = rd_next;
  end

endmodule
