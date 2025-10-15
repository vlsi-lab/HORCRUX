///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: id_stage.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

module id_stage
  import horcrux_pkg::*;
  (
    input clk_i,
    input rst_ni,

    cv32e40px_if_xif.coproc_issue xif_issue_if,

    output horcrux_pkg::in_t rs_values_o,
    output logic [4:0] rd_o,
    output logic [3:0] id_o,
    output horcrux_pkg::horcrux_insn select_insn_o,

    output logic save_rd_o,
    output logic done_o,

    output logic [1:0] select_op_o
);

  horcrux_pkg::instruction_u instruction;
  horcrux_pkg::in_t rs_values;
  horcrux_pkg::horcrux_insn select_insn;

  logic [1:0] select_op;
  logic sample_in;
  logic save_rd;
  logic done;

  assign instruction = xif_issue_if.issue_req.instr;

  logic [NbInstr-1:0] sel;

  for (genvar i = 0; i < NbInstr; i++) begin : gen_predecoder_selector
    assign sel[i] = ((CoproInstr[i].mask & instruction) == CoproInstr[i].instr);
  end

  //DECODER
  always_comb begin
    
    xif_issue_if.issue_resp.loadstore = '0;
    xif_issue_if.issue_resp.ecswrite = '0;
    xif_issue_if.issue_resp.exc = '0;
  
    xif_issue_if.issue_resp.accept = '0;
    xif_issue_if.issue_resp.writeback = '0;
    xif_issue_if.issue_resp.dualread = '0;
    xif_issue_if.issue_resp.dualwrite = '0;
    xif_issue_if.issue_resp.loadstore = '0;
    xif_issue_if.issue_resp.ecswrite = '0;
    xif_issue_if.issue_resp.exc = '0;
    xif_issue_if.issue_ready = '0;

    sample_in = '0;
    select_op = 2'b00;
    select_insn = nada;
    save_rd = '0;
    rs_values = '0;
    rd_o <= '0;
    id_o <= '0;
    
    for (int unsigned i = 0; i < NbInstr; i++) begin
      if (sel[i] && xif_issue_if.issue_valid) begin

          xif_issue_if.issue_ready = 1'b1;
          xif_issue_if.issue_resp.accept = CoproInstr[i].resp.accept;
          xif_issue_if.issue_resp.writeback = CoproInstr[i].resp.writeback;
          
          select_op = CoproInstr[i].resp.op_type;
          sample_in = 1'b1;
          save_rd = 1'b1;
          select_insn = CoproInstr[i].resp.insn;

          rs_values.rs1_0 = xif_issue_if.issue_req.rs[0];
          rs_values.rs2_0 = xif_issue_if.issue_req.rs[1];
          rs_values.rs3_0 = xif_issue_if.issue_req.rs[2];
          
          rd_o <= instruction.as_horcrux_R.rd;
          id_o <= xif_issue_if.issue_req.id;
      end
    end
  end
  //INSTRUCTION DECODER


  generate
    always @(posedge clk_i or negedge rst_ni) begin
      if (~rst_ni) begin
        done       <= '0;

      end else begin
        for (int unsigned i = 0; i < NbInstr; i++) begin
          if (xif_issue_if.issue_valid) begin     

            if (sel[i]) begin
              done      <= CoproInstr[i].resp.done;
            end 

          end else begin
            done      <= '0;
          end 
        end
      end
    end
  endgenerate 


  always @(posedge clk_i or negedge rst_ni) begin : pipe_reg_ID_EX
    if (~rst_ni) begin
      rs_values_o <= '0;
      select_insn_o <= nada;
    end else begin
      //if (sample_in) begin
        rs_values_o <= rs_values;
        select_insn_o <= select_insn;
      //end
    end
  end


  assign save_rd_o       = save_rd;
  assign select_op_o     = select_op;
  assign done_o          = done;

endmodule
