///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: horcrux_x_heep_top.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////



module horcrux_x_heep_top 
  import obi_pkg::*;
  import reg_pkg::*;
  import cv32e40px_pkg::*;
#(
    parameter COREV_PULP = 0,
    parameter FPU        = 0,
    parameter ZFINX      = 0,
    parameter EXT_XBAR_NMASTER = 0,
    parameter EXT_XBAR_NMASTER_RND = EXT_XBAR_NMASTER == 0 ? 1 : EXT_XBAR_NMASTER,
    parameter EXT_DOMAINS_RND = core_v_mini_mcu_pkg::EXTERNAL_DOMAINS == 0 ? 1 : core_v_mini_mcu_pkg::EXTERNAL_DOMAINS
    
) (
    inout logic 		       clk_i,
    inout logic 		       rst_ni,

    inout logic 		       boot_select_i,
    inout logic 		       execute_from_flash_i,

    inout logic 		       jtag_tck_i,
    inout logic 		       jtag_tms_i,
    inout logic 		       jtag_trst_ni,
    inout logic 		       jtag_tdi_i,
    inout logic 		       jtag_tdo_o,
    
    inout logic			       uart_rx_i,
    inout logic			       uart_tx_o,
   
    inout logic [31:0] 		   gpio_io,
   
    inout logic [3:0] 		   spi_flash_sd_io,
    inout logic [1:0] 		   spi_flash_csb,
    inout logic 		         spi_flash_sck,

    inout logic [3:0] 		   spi_sd_io,
    inout logic [1:0] 		   spi_csb,
    inout logic 		         spi_sck,


    output logic [EXT_DOMAINS_RND-1:0] external_subsystem_powergate_switch_o,
    input logic [EXT_DOMAINS_RND-1:0]  external_subsystem_powergate_switch_ack_i,
    output logic [EXT_DOMAINS_RND-1:0] external_subsystem_powergate_iso_o,
   
    output logic [EXT_DOMAINS_RND-1:0] external_subsystem_rst_no,
    output logic [EXT_DOMAINS_RND-1:0] external_ram_banks_set_retentive_o,

    output logic [31:0] 	       exit_value_o,
    inout logic 		       exit_valid_o

   );
   
  import horcrux_x_heep_pkg::*;
  import core_v_mini_mcu_pkg::*;

   
  localparam int unsigned LOG_EXT_XBAR_NSLAVE = EXT_XBAR_NSLAVE > 32'd1 ? $clog2(
      EXT_XBAR_NSLAVE
  ) : 32'd1;

  // External xbar master/slave and peripheral ports
  obi_req_t [EXT_XBAR_NMASTER_RND-1:0] ext_master_req;
  obi_req_t [EXT_XBAR_NMASTER_RND-1:0] heep_slave_req;
  obi_resp_t [EXT_XBAR_NMASTER_RND-1:0] ext_master_resp;
  obi_resp_t [EXT_XBAR_NMASTER_RND-1:0] heep_slave_resp;
  obi_req_t heep_core_instr_req;
  obi_resp_t heep_core_instr_resp;
  obi_req_t heep_core_data_req;
  obi_resp_t heep_core_data_resp;
  obi_req_t heep_debug_master_req;
  obi_resp_t heep_debug_master_resp;
  obi_req_t heep_dma_read_ch0_req;
  obi_resp_t heep_dma_read_ch0_resp;
  obi_req_t heep_dma_write_ch0_req;
  obi_resp_t heep_dma_write_ch0_resp;
  obi_req_t heep_dma_addr_ch0_req;
  obi_resp_t heep_dma_addr_ch0_resp;
  obi_req_t [EXT_XBAR_NSLAVE-1:0] ext_slave_req;
  obi_resp_t [EXT_XBAR_NSLAVE-1:0] ext_slave_resp;

  reg_req_t ext_periph_slave_req;
  reg_rsp_t ext_periph_slave_resp;


  localparam EXT_HARTS = 0;

  //do not touch these parameter
  localparam EXT_HARTS_RND = EXT_HARTS == 0 ? 1 : EXT_HARTS;


  logic [EXT_HARTS_RND-1:0] ext_debug_req;
  logic ext_cpu_subsystem_rst_n;
  logic ext_debug_reset_n;

  // PM signals
  logic cpu_subsystem_powergate_switch_n;
  logic cpu_subsystem_powergate_switch_ack_n;
  logic peripheral_subsystem_powergate_switch_n;
  logic peripheral_subsystem_powergate_switch_ack_n;

  // PAD controller
  reg_req_t pad_req;
  reg_rsp_t pad_resp;
  logic [core_v_mini_mcu_pkg::NUM_PAD-1:0][0:0] pad_muxes;

  logic rst_ngen;

  //input, output pins from core_v_mini_mcu
  logic clk_in_x, clk_out_x, clk_oe_x;

  logic rst_nin_x, rst_nout_x, rst_noe_x;

  logic boot_select_in_x, boot_select_out_x, boot_select_oe_x;

  logic execute_from_flash_in_x, execute_from_flash_out_x, execute_from_flash_oe_x;

  logic jtag_tck_in_x, jtag_tck_out_x, jtag_tck_oe_x;

  logic jtag_tms_in_x, jtag_tms_out_x, jtag_tms_oe_x;

  logic jtag_trst_nin_x, jtag_trst_nout_x, jtag_trst_noe_x;

  logic jtag_tdi_in_x, jtag_tdi_out_x, jtag_tdi_oe_x;

  logic jtag_tdo_in_x, jtag_tdo_out_x, jtag_tdo_oe_x;

  logic uart_rx_in_x, uart_rx_out_x, uart_rx_oe_x;

  logic uart_tx_in_x, uart_tx_out_x, uart_tx_oe_x;

  logic exit_valid_in_x, exit_valid_out_x, exit_valid_oe_x;

  logic gpio_0_in_x, gpio_0_out_x, gpio_0_oe_x;

  logic gpio_1_in_x, gpio_1_out_x, gpio_1_oe_x;

  logic gpio_2_in_x, gpio_2_out_x, gpio_2_oe_x;

  logic gpio_3_in_x, gpio_3_out_x, gpio_3_oe_x;

  logic gpio_4_in_x, gpio_4_out_x, gpio_4_oe_x;

  logic gpio_5_in_x, gpio_5_out_x, gpio_5_oe_x;

  logic gpio_6_in_x, gpio_6_out_x, gpio_6_oe_x;

  logic gpio_7_in_x, gpio_7_out_x, gpio_7_oe_x;

  logic gpio_8_in_x, gpio_8_out_x, gpio_8_oe_x;

  logic gpio_9_in_x, gpio_9_out_x, gpio_9_oe_x;

  logic gpio_10_in_x, gpio_10_out_x, gpio_10_oe_x;

  logic gpio_11_in_x, gpio_11_out_x, gpio_11_oe_x;

  logic gpio_12_in_x, gpio_12_out_x, gpio_12_oe_x;

  logic gpio_13_in_x, gpio_13_out_x, gpio_13_oe_x;

  logic gpio_14_in_x, gpio_14_out_x, gpio_14_oe_x;

  logic gpio_15_in_x, gpio_15_out_x, gpio_15_oe_x;

  logic gpio_16_in_x, gpio_16_out_x, gpio_16_oe_x;

  logic gpio_17_in_x, gpio_17_out_x, gpio_17_oe_x;

  logic spi_flash_sck_in_x, spi_flash_sck_out_x, spi_flash_sck_oe_x;

  logic spi_flash_cs_0_in_x, spi_flash_cs_0_out_x, spi_flash_cs_0_oe_x;

  logic spi_flash_cs_1_in_x, spi_flash_cs_1_out_x, spi_flash_cs_1_oe_x;

  logic spi_flash_sd_0_in_x, spi_flash_sd_0_out_x, spi_flash_sd_0_oe_x;

  logic spi_flash_sd_1_in_x, spi_flash_sd_1_out_x, spi_flash_sd_1_oe_x;

  logic spi_flash_sd_2_in_x, spi_flash_sd_2_out_x, spi_flash_sd_2_oe_x;

  logic spi_flash_sd_3_in_x, spi_flash_sd_3_out_x, spi_flash_sd_3_oe_x;

  logic spi_sck_in_x, spi_sck_out_x, spi_sck_oe_x;

  logic spi_cs_0_in_x, spi_cs_0_out_x, spi_cs_0_oe_x;

  logic spi_cs_1_in_x, spi_cs_1_out_x, spi_cs_1_oe_x;

  logic spi_sd_0_in_x, spi_sd_0_out_x, spi_sd_0_oe_x;

  logic spi_sd_1_in_x, spi_sd_1_out_x, spi_sd_1_oe_x;

  logic spi_sd_2_in_x, spi_sd_2_out_x, spi_sd_2_oe_x;

  logic spi_sd_3_in_x, spi_sd_3_out_x, spi_sd_3_oe_x;

  logic pdm2pcm_pdm_in_x, pdm2pcm_pdm_out_x, pdm2pcm_pdm_oe_x;
  logic gpio_18_in_x, gpio_18_out_x, gpio_18_oe_x;
  logic pdm2pcm_pdm_in_x_muxed, pdm2pcm_pdm_out_x_muxed, pdm2pcm_pdm_oe_x_muxed;

  logic pdm2pcm_clk_in_x, pdm2pcm_clk_out_x, pdm2pcm_clk_oe_x;
  logic gpio_19_in_x, gpio_19_out_x, gpio_19_oe_x;
  logic pdm2pcm_clk_in_x_muxed, pdm2pcm_clk_out_x_muxed, pdm2pcm_clk_oe_x_muxed;

  logic i2s_sck_in_x, i2s_sck_out_x, i2s_sck_oe_x;
  logic gpio_20_in_x, gpio_20_out_x, gpio_20_oe_x;
  logic i2s_sck_in_x_muxed, i2s_sck_out_x_muxed, i2s_sck_oe_x_muxed;

  logic i2s_ws_in_x, i2s_ws_out_x, i2s_ws_oe_x;
  logic gpio_21_in_x, gpio_21_out_x, gpio_21_oe_x;
  logic i2s_ws_in_x_muxed, i2s_ws_out_x_muxed, i2s_ws_oe_x_muxed;

  logic i2s_sd_in_x, i2s_sd_out_x, i2s_sd_oe_x;
  logic gpio_22_in_x, gpio_22_out_x, gpio_22_oe_x;
  logic i2s_sd_in_x_muxed, i2s_sd_out_x_muxed, i2s_sd_oe_x_muxed;

  logic spi2_cs_0_in_x, spi2_cs_0_out_x, spi2_cs_0_oe_x;
  logic gpio_23_in_x, gpio_23_out_x, gpio_23_oe_x;
  logic spi2_cs_0_in_x_muxed, spi2_cs_0_out_x_muxed, spi2_cs_0_oe_x_muxed;

  logic spi2_cs_1_in_x, spi2_cs_1_out_x, spi2_cs_1_oe_x;
  logic gpio_24_in_x, gpio_24_out_x, gpio_24_oe_x;
  logic spi2_cs_1_in_x_muxed, spi2_cs_1_out_x_muxed, spi2_cs_1_oe_x_muxed;

  logic spi2_sck_in_x, spi2_sck_out_x, spi2_sck_oe_x;
  logic gpio_25_in_x, gpio_25_out_x, gpio_25_oe_x;
  logic spi2_sck_in_x_muxed, spi2_sck_out_x_muxed, spi2_sck_oe_x_muxed;

  logic spi2_sd_0_in_x, spi2_sd_0_out_x, spi2_sd_0_oe_x;
  logic gpio_26_in_x, gpio_26_out_x, gpio_26_oe_x;
  logic spi2_sd_0_in_x_muxed, spi2_sd_0_out_x_muxed, spi2_sd_0_oe_x_muxed;

  logic spi2_sd_1_in_x, spi2_sd_1_out_x, spi2_sd_1_oe_x;
  logic gpio_27_in_x, gpio_27_out_x, gpio_27_oe_x;
  logic spi2_sd_1_in_x_muxed, spi2_sd_1_out_x_muxed, spi2_sd_1_oe_x_muxed;

  logic spi2_sd_2_in_x, spi2_sd_2_out_x, spi2_sd_2_oe_x;
  logic gpio_28_in_x, gpio_28_out_x, gpio_28_oe_x;
  logic spi2_sd_2_in_x_muxed, spi2_sd_2_out_x_muxed, spi2_sd_2_oe_x_muxed;

  logic spi2_sd_3_in_x, spi2_sd_3_out_x, spi2_sd_3_oe_x;
  logic gpio_29_in_x, gpio_29_out_x, gpio_29_oe_x;
  logic spi2_sd_3_in_x_muxed, spi2_sd_3_out_x_muxed, spi2_sd_3_oe_x_muxed;

  logic i2c_scl_in_x, i2c_scl_out_x, i2c_scl_oe_x;
  logic gpio_31_in_x, gpio_31_out_x, gpio_31_oe_x;
  logic i2c_scl_in_x_muxed, i2c_scl_out_x_muxed, i2c_scl_oe_x_muxed;

  logic i2c_sda_in_x, i2c_sda_out_x, i2c_sda_oe_x;
  logic gpio_30_in_x, gpio_30_out_x, gpio_30_oe_x;
  logic i2c_sda_in_x_muxed, i2c_sda_out_x_muxed, i2c_sda_oe_x_muxed;


  // Since not used an external device with master port 
  assign ext_master_req[horcrux_x_heep_pkg::EXT_MASTER0_IDX].req = '0;
  assign ext_master_req[horcrux_x_heep_pkg::EXT_MASTER0_IDX].we = '0;
  assign ext_master_req[horcrux_x_heep_pkg::EXT_MASTER0_IDX].be = '0;
  assign ext_master_req[horcrux_x_heep_pkg::EXT_MASTER0_IDX].addr = '0;
  assign ext_master_req[horcrux_x_heep_pkg::EXT_MASTER0_IDX].wdata = '0;

  
  // External interrupts
  logic [core_v_mini_mcu_pkg::NEXT_INT-1:0] ext_intr_vector;
  logic 				    horcrux_intr; 

  // External subsystems
  logic [core_v_mini_mcu_pkg::EXTERNAL_DOMAINS-1:0] external_subsystem_rst_n;
  logic [core_v_mini_mcu_pkg::EXTERNAL_DOMAINS-1:0] external_ram_banks_set_retentive;

   // eXtension Interface
  cv32e40px_if_xif #() ext_if();
  
  always_comb begin
    // All interrupt lines set to zero by default
    for (int i = 0; i < core_v_mini_mcu_pkg::NEXT_INT; i++) begin
      ext_intr_vector[i] = 1'b0;
    end
    // Re-assign the interrupt lines used here
    ext_intr_vector[0] = horcrux_intr;
  end

  horcrux_wrapper horcrux_wrapper_i(
      .clk_i (clk_in_x),
      .rst_ni(rst_ngen),
      // eXtension interface
      .xif_compressed_if(ext_if),
      .xif_issue_if(ext_if),
      .xif_commit_if(ext_if),
      .xif_mem_if(ext_if),
      .xif_mem_result_if(ext_if),
      .xif_result_if(ext_if)
    );

`ifdef FPGA_SYNTHESIS
  assign cpu_subsystem_powergate_switch_ack_n = cpu_subsystem_powergate_switch_n;
  assign peripheral_subsystem_powergate_switch_ack_n = peripheral_subsystem_powergate_switch_n;
`endif

  core_v_mini_mcu #(
      .COREV_PULP(COREV_PULP),
      .FPU(FPU),
      .ZFINX(ZFINX),
      .EXT_XBAR_NMASTER(EXT_XBAR_NMASTER),
      .X_EXT(1),
      .EXT_HARTS(EXT_HARTS)
  ) u_core_v_mini_mcu (

      .rst_ni(rst_ngen),
      .clk_i (clk_in_x),


      .boot_select_i(boot_select_in_x),

      .execute_from_flash_i(execute_from_flash_in_x),

      .jtag_tck_i(jtag_tck_in_x),

      .jtag_tms_i(jtag_tms_in_x),

      .jtag_trst_ni(jtag_trst_nin_x),

      .jtag_tdi_i(jtag_tdi_in_x),

      .jtag_tdo_o(jtag_tdo_out_x),

      .uart_rx_i(uart_rx_in_x),

      .uart_tx_o(uart_tx_out_x),

      .exit_valid_o(exit_valid_out_x),

      .gpio_0_i(gpio_0_in_x),
      .gpio_0_o(gpio_0_out_x),
      .gpio_0_oe_o(gpio_0_oe_x),

      .gpio_1_i(gpio_1_in_x),
      .gpio_1_o(gpio_1_out_x),
      .gpio_1_oe_o(gpio_1_oe_x),

      .gpio_2_i(gpio_2_in_x),
      .gpio_2_o(gpio_2_out_x),
      .gpio_2_oe_o(gpio_2_oe_x),

      .gpio_3_i(gpio_3_in_x),
      .gpio_3_o(gpio_3_out_x),
      .gpio_3_oe_o(gpio_3_oe_x),

      .gpio_4_i(gpio_4_in_x),
      .gpio_4_o(gpio_4_out_x),
      .gpio_4_oe_o(gpio_4_oe_x),

      .gpio_5_i(gpio_5_in_x),
      .gpio_5_o(gpio_5_out_x),
      .gpio_5_oe_o(gpio_5_oe_x),

      .gpio_6_i(gpio_6_in_x),
      .gpio_6_o(gpio_6_out_x),
      .gpio_6_oe_o(gpio_6_oe_x),

      .gpio_7_i(gpio_7_in_x),
      .gpio_7_o(gpio_7_out_x),
      .gpio_7_oe_o(gpio_7_oe_x),

      .gpio_8_i(gpio_8_in_x),
      .gpio_8_o(gpio_8_out_x),
      .gpio_8_oe_o(gpio_8_oe_x),

      .gpio_9_i(gpio_9_in_x),
      .gpio_9_o(gpio_9_out_x),
      .gpio_9_oe_o(gpio_9_oe_x),

      .gpio_10_i(gpio_10_in_x),
      .gpio_10_o(gpio_10_out_x),
      .gpio_10_oe_o(gpio_10_oe_x),

      .gpio_11_i(gpio_11_in_x),
      .gpio_11_o(gpio_11_out_x),
      .gpio_11_oe_o(gpio_11_oe_x),

      .gpio_12_i(gpio_12_in_x),
      .gpio_12_o(gpio_12_out_x),
      .gpio_12_oe_o(gpio_12_oe_x),

      .gpio_13_i(gpio_13_in_x),
      .gpio_13_o(gpio_13_out_x),
      .gpio_13_oe_o(gpio_13_oe_x),

      .gpio_14_i(gpio_14_in_x),
      .gpio_14_o(gpio_14_out_x),
      .gpio_14_oe_o(gpio_14_oe_x),

      .gpio_15_i(gpio_15_in_x),
      .gpio_15_o(gpio_15_out_x),
      .gpio_15_oe_o(gpio_15_oe_x),

      .gpio_16_i(gpio_16_in_x),
      .gpio_16_o(gpio_16_out_x),
      .gpio_16_oe_o(gpio_16_oe_x),

      .gpio_17_i(gpio_17_in_x),
      .gpio_17_o(gpio_17_out_x),
      .gpio_17_oe_o(gpio_17_oe_x),

      .spi_flash_sck_i(spi_flash_sck_in_x),
      .spi_flash_sck_o(spi_flash_sck_out_x),
      .spi_flash_sck_oe_o(spi_flash_sck_oe_x),

      .spi_flash_cs_0_i(spi_flash_cs_0_in_x),
      .spi_flash_cs_0_o(spi_flash_cs_0_out_x),
      .spi_flash_cs_0_oe_o(spi_flash_cs_0_oe_x),

      .spi_flash_cs_1_i(spi_flash_cs_1_in_x),
      .spi_flash_cs_1_o(spi_flash_cs_1_out_x),
      .spi_flash_cs_1_oe_o(spi_flash_cs_1_oe_x),

      .spi_flash_sd_0_i(spi_flash_sd_0_in_x),
      .spi_flash_sd_0_o(spi_flash_sd_0_out_x),
      .spi_flash_sd_0_oe_o(spi_flash_sd_0_oe_x),

      .spi_flash_sd_1_i(spi_flash_sd_1_in_x),
      .spi_flash_sd_1_o(spi_flash_sd_1_out_x),
      .spi_flash_sd_1_oe_o(spi_flash_sd_1_oe_x),

      .spi_flash_sd_2_i(spi_flash_sd_2_in_x),
      .spi_flash_sd_2_o(spi_flash_sd_2_out_x),
      .spi_flash_sd_2_oe_o(spi_flash_sd_2_oe_x),

      .spi_flash_sd_3_i(spi_flash_sd_3_in_x),
      .spi_flash_sd_3_o(spi_flash_sd_3_out_x),
      .spi_flash_sd_3_oe_o(spi_flash_sd_3_oe_x),

      .spi_sck_i(spi_sck_in_x),
      .spi_sck_o(spi_sck_out_x),
      .spi_sck_oe_o(spi_sck_oe_x),

      .spi_cs_0_i(spi_cs_0_in_x),
      .spi_cs_0_o(spi_cs_0_out_x),
      .spi_cs_0_oe_o(spi_cs_0_oe_x),

      .spi_cs_1_i(spi_cs_1_in_x),
      .spi_cs_1_o(spi_cs_1_out_x),
      .spi_cs_1_oe_o(spi_cs_1_oe_x),

      .spi_sd_0_i(spi_sd_0_in_x),
      .spi_sd_0_o(spi_sd_0_out_x),
      .spi_sd_0_oe_o(spi_sd_0_oe_x),

      .spi_sd_1_i(spi_sd_1_in_x),
      .spi_sd_1_o(spi_sd_1_out_x),
      .spi_sd_1_oe_o(spi_sd_1_oe_x),

      .spi_sd_2_i(spi_sd_2_in_x),
      .spi_sd_2_o(spi_sd_2_out_x),
      .spi_sd_2_oe_o(spi_sd_2_oe_x),

      .spi_sd_3_i(spi_sd_3_in_x),
      .spi_sd_3_o(spi_sd_3_out_x),
      .spi_sd_3_oe_o(spi_sd_3_oe_x),

      .pdm2pcm_pdm_i(pdm2pcm_pdm_in_x),
      .pdm2pcm_pdm_o(pdm2pcm_pdm_out_x),
      .pdm2pcm_pdm_oe_o(pdm2pcm_pdm_oe_x),
      .gpio_18_i(gpio_18_in_x),
      .gpio_18_o(gpio_18_out_x),
      .gpio_18_oe_o(gpio_18_oe_x),

      .pdm2pcm_clk_i(pdm2pcm_clk_in_x),
      .pdm2pcm_clk_o(pdm2pcm_clk_out_x),
      .pdm2pcm_clk_oe_o(pdm2pcm_clk_oe_x),
      .gpio_19_i(gpio_19_in_x),
      .gpio_19_o(gpio_19_out_x),
      .gpio_19_oe_o(gpio_19_oe_x),

      .i2s_sck_i(i2s_sck_in_x),
      .i2s_sck_o(i2s_sck_out_x),
      .i2s_sck_oe_o(i2s_sck_oe_x),
      .gpio_20_i(gpio_20_in_x),
      .gpio_20_o(gpio_20_out_x),
      .gpio_20_oe_o(gpio_20_oe_x),

      .i2s_ws_i(i2s_ws_in_x),
      .i2s_ws_o(i2s_ws_out_x),
      .i2s_ws_oe_o(i2s_ws_oe_x),
      .gpio_21_i(gpio_21_in_x),
      .gpio_21_o(gpio_21_out_x),
      .gpio_21_oe_o(gpio_21_oe_x),

      .i2s_sd_i(i2s_sd_in_x),
      .i2s_sd_o(i2s_sd_out_x),
      .i2s_sd_oe_o(i2s_sd_oe_x),
      .gpio_22_i(gpio_22_in_x),
      .gpio_22_o(gpio_22_out_x),
      .gpio_22_oe_o(gpio_22_oe_x),

      .spi2_cs_0_i(spi2_cs_0_in_x),
      .spi2_cs_0_o(spi2_cs_0_out_x),
      .spi2_cs_0_oe_o(spi2_cs_0_oe_x),
      .gpio_23_i(gpio_23_in_x),
      .gpio_23_o(gpio_23_out_x),
      .gpio_23_oe_o(gpio_23_oe_x),

      .spi2_cs_1_i(spi2_cs_1_in_x),
      .spi2_cs_1_o(spi2_cs_1_out_x),
      .spi2_cs_1_oe_o(spi2_cs_1_oe_x),
      .gpio_24_i(gpio_24_in_x),
      .gpio_24_o(gpio_24_out_x),
      .gpio_24_oe_o(gpio_24_oe_x),

      .spi2_sck_i(spi2_sck_in_x),
      .spi2_sck_o(spi2_sck_out_x),
      .spi2_sck_oe_o(spi2_sck_oe_x),
      .gpio_25_i(gpio_25_in_x),
      .gpio_25_o(gpio_25_out_x),
      .gpio_25_oe_o(gpio_25_oe_x),

      .spi2_sd_0_i(spi2_sd_0_in_x),
      .spi2_sd_0_o(spi2_sd_0_out_x),
      .spi2_sd_0_oe_o(spi2_sd_0_oe_x),
      .gpio_26_i(gpio_26_in_x),
      .gpio_26_o(gpio_26_out_x),
      .gpio_26_oe_o(gpio_26_oe_x),

      .spi2_sd_1_i(spi2_sd_1_in_x),
      .spi2_sd_1_o(spi2_sd_1_out_x),
      .spi2_sd_1_oe_o(spi2_sd_1_oe_x),
      .gpio_27_i(gpio_27_in_x),
      .gpio_27_o(gpio_27_out_x),
      .gpio_27_oe_o(gpio_27_oe_x),

      .spi2_sd_2_i(spi2_sd_2_in_x),
      .spi2_sd_2_o(spi2_sd_2_out_x),
      .spi2_sd_2_oe_o(spi2_sd_2_oe_x),
      .gpio_28_i(gpio_28_in_x),
      .gpio_28_o(gpio_28_out_x),
      .gpio_28_oe_o(gpio_28_oe_x),

      .spi2_sd_3_i(spi2_sd_3_in_x),
      .spi2_sd_3_o(spi2_sd_3_out_x),
      .spi2_sd_3_oe_o(spi2_sd_3_oe_x),
      .gpio_29_i(gpio_29_in_x),
      .gpio_29_o(gpio_29_out_x),
      .gpio_29_oe_o(gpio_29_oe_x),

      .i2c_scl_i(i2c_scl_in_x),
      .i2c_scl_o(i2c_scl_out_x),
      .i2c_scl_oe_o(i2c_scl_oe_x),
      .gpio_31_i(gpio_31_in_x),
      .gpio_31_o(gpio_31_out_x),
      .gpio_31_oe_o(gpio_31_oe_x),

      .i2c_sda_i(i2c_sda_in_x),
      .i2c_sda_o(i2c_sda_out_x),
      .i2c_sda_oe_o(i2c_sda_oe_x),
      .gpio_30_i(gpio_30_in_x),
      .gpio_30_o(gpio_30_out_x),
      .gpio_30_oe_o(gpio_30_oe_x),

      .intr_vector_ext_i(ext_intr_vector),
      .xif_compressed_if(ext_if),
      .xif_issue_if(ext_if),
      .xif_commit_if(ext_if),
      .xif_mem_if(ext_if),
      .xif_mem_result_if(ext_if),
      .xif_result_if(ext_if),
      .pad_req_o(pad_req),
      .pad_resp_i(pad_resp),
      .ext_xbar_master_req_i('0),
      .ext_xbar_master_resp_o(heep_slave_resp),
      .ext_core_instr_req_o(heep_core_instr_req),    
      .ext_core_instr_resp_i(heep_core_instr_resp),   
      .ext_core_data_req_o(heep_core_data_req),     
      .ext_core_data_resp_i(heep_core_data_resp),    
      .ext_debug_master_req_o(heep_debug_master_req),  
      .ext_debug_master_resp_i(heep_debug_master_resp), 		     
      .ext_dma_read_ch0_req_o(heep_dma_read_ch0_req),        		     
      .ext_dma_read_ch0_resp_i(heep_dma_read_ch0_resp), 		     
      .ext_dma_write_ch0_req_o(heep_dma_write_ch0_req),       
      .ext_dma_write_ch0_resp_i(heep_dma_write_ch0_resp),
      .ext_dma_addr_ch0_req_o(heep_dma_addr_ch0_req),  
      .ext_dma_addr_ch0_resp_i(heep_dma_addr_ch0_resp), 
      .ext_peripheral_slave_req_o(ext_periph_slave_req),
      .ext_peripheral_slave_resp_i(ext_periph_slave_resp),
      .ext_dma_stop_i('0),
      .ext_debug_req_o(ext_debug_req),
      .ext_debug_reset_no(ext_debug_reset_n),
      .cpu_subsystem_powergate_switch_no(cpu_subsystem_powergate_switch_n),
      .cpu_subsystem_powergate_switch_ack_ni(cpu_subsystem_powergate_switch_ack_n),
      .peripheral_subsystem_powergate_switch_no(peripheral_subsystem_powergate_switch_n),
      .peripheral_subsystem_powergate_switch_ack_ni(peripheral_subsystem_powergate_switch_ack_n),
      .external_subsystem_powergate_switch_no(external_subsystem_powergate_switch_o),
      .external_subsystem_powergate_switch_ack_ni(external_subsystem_powergate_switch_ack_i),
      .external_subsystem_powergate_iso_no(external_subsystem_powergate_iso_o),
      .external_subsystem_rst_no(external_subsystem_rst_n),
      .ext_cpu_subsystem_rst_no(ext_cpu_subsystem_rst_n),
      .external_ram_banks_set_retentive_no(external_ram_banks_set_retentive_o),
      .external_subsystem_clkgate_en_no(external_subsystem_clkgate_en_o),
      .exit_value_o,
      .ext_dma_slot_tx_i('0),
      .ext_dma_slot_rx_i('0)
  );


  // The external bus connects the external peripherals among them and to
  // the corresponding X-HEEP slave port (to the internal system bus).
    ext_bus #(
        .EXT_XBAR_NMASTER(EXT_XBAR_NMASTER),
        .EXT_XBAR_NSLAVE (EXT_XBAR_NSLAVE)
    ) ext_bus_i (
        .clk_i                    (clk_in_x),
        .rst_ni                   (rst_ngen),
        .addr_map_i               (EXT_XBAR_ADDR_RULES),
        .default_idx_i            (HORCRUX_IDX[LOG_EXT_XBAR_NSLAVE-1:0]),
        .heep_core_instr_req_i    (heep_core_instr_req),
        .heep_core_instr_resp_o   (heep_core_instr_resp),
        .heep_core_data_req_i     (heep_core_data_req),
        .heep_core_data_resp_o    (heep_core_data_resp),
        .heep_debug_master_req_i  (heep_debug_master_req),
        .heep_debug_master_resp_o (heep_debug_master_resp),
        .heep_dma_read_ch0_req_i  (heep_dma_read_ch0_req),
        .heep_dma_read_ch0_resp_o (heep_dma_read_ch0_resp),
        .heep_dma_write_ch0_req_i (heep_dma_write_ch0_req),
        .heep_dma_write_ch0_resp_o(heep_dma_write_ch0_resp),
        .heep_dma_addr_ch0_req_i  (heep_dma_addr_ch0_req),
        .heep_dma_addr_ch0_resp_o (heep_dma_addr_ch0_resp),
        .ext_master_req_i         (ext_master_req),
        .ext_master_resp_o        (ext_master_resp),
        .heep_slave_req_o         (heep_slave_req),
        .heep_slave_resp_i        (heep_slave_resp),
        .ext_slave_req_o          (ext_slave_req),
        .ext_slave_resp_i         (ext_slave_resp)
    );

  

pad_ring u_pad_ring (
      .clk_io(clk_i),
      .clk_o(clk_in_x),
      .rst_nio(rst_ni),
      .rst_no(rst_nin_x),
      .boot_select_io(boot_select_i),
      .boot_select_o(boot_select_in_x),
      .execute_from_flash_io(execute_from_flash_i),
      .execute_from_flash_o(execute_from_flash_in_x),
      .jtag_tck_io(jtag_tck_i),
      .jtag_tck_o(jtag_tck_in_x),
      .jtag_tms_io(jtag_tms_i),
      .jtag_tms_o(jtag_tms_in_x),
      .jtag_trst_nio(jtag_trst_ni),
      .jtag_trst_no(jtag_trst_nin_x),
      .jtag_tdi_io(jtag_tdi_i),
      .jtag_tdi_o(jtag_tdi_in_x),
      .jtag_tdo_io(jtag_tdo_o),
      .jtag_tdo_i(jtag_tdo_out_x),
      .uart_rx_io(uart_rx_i),
      .uart_rx_o(uart_rx_in_x),
      .uart_tx_io(uart_tx_o),
      .uart_tx_i(uart_tx_out_x),
      .exit_valid_io(exit_valid_o),
      .exit_valid_i(exit_valid_out_x),
      .gpio_0_io(gpio_io[0]),
      .gpio_0_o(gpio_0_in_x),
      .gpio_0_i(gpio_0_out_x),
      .gpio_0_oe_i(gpio_0_oe_x),
      .gpio_1_io(gpio_io[1]),
      .gpio_1_o(gpio_1_in_x),
      .gpio_1_i(gpio_1_out_x),
      .gpio_1_oe_i(gpio_1_oe_x),
      .gpio_2_io(gpio_io[2]),
      .gpio_2_o(gpio_2_in_x),
      .gpio_2_i(gpio_2_out_x),
      .gpio_2_oe_i(gpio_2_oe_x),
      .gpio_3_io(gpio_io[3]),
      .gpio_3_o(gpio_3_in_x),
      .gpio_3_i(gpio_3_out_x),
      .gpio_3_oe_i(gpio_3_oe_x),
      .gpio_4_io(gpio_io[4]),
      .gpio_4_o(gpio_4_in_x),
      .gpio_4_i(gpio_4_out_x),
      .gpio_4_oe_i(gpio_4_oe_x),
      .gpio_5_io(gpio_io[5]),
      .gpio_5_o(gpio_5_in_x),
      .gpio_5_i(gpio_5_out_x),
      .gpio_5_oe_i(gpio_5_oe_x),
      .gpio_6_io(gpio_io[6]),
      .gpio_6_o(gpio_6_in_x),
      .gpio_6_i(gpio_6_out_x),
      .gpio_6_oe_i(gpio_6_oe_x),
      .gpio_7_io(gpio_io[7]),
      .gpio_7_o(gpio_7_in_x),
      .gpio_7_i(gpio_7_out_x),
      .gpio_7_oe_i(gpio_7_oe_x),
      .gpio_8_io(gpio_io[8]),
      .gpio_8_o(gpio_8_in_x),
      .gpio_8_i(gpio_8_out_x),
      .gpio_8_oe_i(gpio_8_oe_x),
      .gpio_9_io(gpio_io[9]),
      .gpio_9_o(gpio_9_in_x),
      .gpio_9_i(gpio_9_out_x),
      .gpio_9_oe_i(gpio_9_oe_x),
      .gpio_10_io(gpio_io[10]),
      .gpio_10_o(gpio_10_in_x),
      .gpio_10_i(gpio_10_out_x),
      .gpio_10_oe_i(gpio_10_oe_x),
      .gpio_11_io(gpio_io[11]),
      .gpio_11_o(gpio_11_in_x),
      .gpio_11_i(gpio_11_out_x),
      .gpio_11_oe_i(gpio_11_oe_x),
      .gpio_12_io(gpio_io[12]),
      .gpio_12_o(gpio_12_in_x),
      .gpio_12_i(gpio_12_out_x),
      .gpio_12_oe_i(gpio_12_oe_x),
      .gpio_13_io(gpio_io[13]),
      .gpio_13_o(gpio_13_in_x),
      .gpio_13_i(gpio_13_out_x),
      .gpio_13_oe_i(gpio_13_oe_x),
      .gpio_14_io(gpio_io[14]),
      .gpio_14_o(gpio_14_in_x),
      .gpio_14_i(gpio_14_out_x),
      .gpio_14_oe_i(gpio_14_oe_x),
      .gpio_15_io(gpio_io[15]),
      .gpio_15_o(gpio_15_in_x),
      .gpio_15_i(gpio_15_out_x),
      .gpio_15_oe_i(gpio_15_oe_x),
      .gpio_16_io(gpio_io[16]),
      .gpio_16_o(gpio_16_in_x),
      .gpio_16_i(gpio_16_out_x),
      .gpio_16_oe_i(gpio_16_oe_x),
      .gpio_17_io(gpio_io[17]),
      .gpio_17_o(gpio_17_in_x),
      .gpio_17_i(gpio_17_out_x),
      .gpio_17_oe_i(gpio_17_oe_x),
      .spi_flash_sck_io(spi_flash_sck),
      .spi_flash_sck_o(spi_flash_sck_in_x),
      .spi_flash_sck_i(spi_flash_sck_out_x),
      .spi_flash_sck_oe_i(spi_flash_sck_oe_x),
      .spi_flash_cs_0_io(spi_flash_csb[0]),
      .spi_flash_cs_0_o(spi_flash_cs_0_in_x),
      .spi_flash_cs_0_i(spi_flash_cs_0_out_x),
      .spi_flash_cs_0_oe_i(spi_flash_cs_0_oe_x),
      .spi_flash_cs_1_io(spi_flash_csb[1]),
      .spi_flash_cs_1_o(spi_flash_cs_1_in_x),
      .spi_flash_cs_1_i(spi_flash_cs_1_out_x),
      .spi_flash_cs_1_oe_i(spi_flash_cs_1_oe_x),
      .spi_flash_sd_0_io(spi_flash_sd_io[0]),
      .spi_flash_sd_0_o(spi_flash_sd_0_in_x),
      .spi_flash_sd_0_i(spi_flash_sd_0_out_x),
      .spi_flash_sd_0_oe_i(spi_flash_sd_0_oe_x),
      .spi_flash_sd_1_io(spi_flash_sd_io[1]),
      .spi_flash_sd_1_o(spi_flash_sd_1_in_x),
      .spi_flash_sd_1_i(spi_flash_sd_1_out_x),
      .spi_flash_sd_1_oe_i(spi_flash_sd_1_oe_x),
      .spi_flash_sd_2_io(spi_flash_sd_io[2]),
      .spi_flash_sd_2_o(spi_flash_sd_2_in_x),
      .spi_flash_sd_2_i(spi_flash_sd_2_out_x),
      .spi_flash_sd_2_oe_i(spi_flash_sd_2_oe_x),
      .spi_flash_sd_3_io(spi_flash_sd_io[3]),
      .spi_flash_sd_3_o(spi_flash_sd_3_in_x),
      .spi_flash_sd_3_i(spi_flash_sd_3_out_x),
      .spi_flash_sd_3_oe_i(spi_flash_sd_3_oe_x),
      .spi_sck_io(spi_sck),
      .spi_sck_o(spi_sck_in_x),
      .spi_sck_i(spi_sck_out_x),
      .spi_sck_oe_i(spi_sck_oe_x),
      .spi_cs_0_io(spi_csb[0]),
      .spi_cs_0_o(spi_cs_0_in_x),
      .spi_cs_0_i(spi_cs_0_out_x),
      .spi_cs_0_oe_i(spi_cs_0_oe_x),
      .spi_cs_1_io(spi_csb[1]),
      .spi_cs_1_o(spi_cs_1_in_x),
      .spi_cs_1_i(spi_cs_1_out_x),
      .spi_cs_1_oe_i(spi_cs_1_oe_x),
      .spi_sd_0_io(spi_sd_0_io),
      .spi_sd_0_o(spi_sd_0_in_x),
      .spi_sd_0_i(spi_sd_0_out_x),
      .spi_sd_0_oe_i(spi_sd_0_oe_x),
      .spi_sd_1_io(spi_sd_1_io),
      .spi_sd_1_o(spi_sd_1_in_x),
      .spi_sd_1_i(spi_sd_1_out_x),
      .spi_sd_1_oe_i(spi_sd_1_oe_x),
      .spi_sd_2_io(spi_sd_2_io),
      .spi_sd_2_o(spi_sd_2_in_x),
      .spi_sd_2_i(spi_sd_2_out_x),
      .spi_sd_2_oe_i(spi_sd_2_oe_x),
      .spi_sd_3_io(spi_sd_3_io),
      .spi_sd_3_o(spi_sd_3_in_x),
      .spi_sd_3_i(spi_sd_3_out_x),
      .spi_sd_3_oe_i(spi_sd_3_oe_x),
      .pdm2pcm_pdm_io(gpio_io[18]),
      .pdm2pcm_pdm_o(pdm2pcm_pdm_in_x_muxed),
      .pdm2pcm_pdm_i(pdm2pcm_pdm_out_x_muxed),
      .pdm2pcm_pdm_oe_i(pdm2pcm_pdm_oe_x_muxed),
      .pdm2pcm_clk_io(gpio_io[19]),
      .pdm2pcm_clk_o(pdm2pcm_clk_in_x_muxed),
      .pdm2pcm_clk_i(pdm2pcm_clk_out_x_muxed),
      .pdm2pcm_clk_oe_i(pdm2pcm_clk_oe_x_muxed),
      .i2s_sck_io(gpio_io[20]),
      .i2s_sck_o(i2s_sck_in_x_muxed),
      .i2s_sck_i(i2s_sck_out_x_muxed),
      .i2s_sck_oe_i(i2s_sck_oe_x_muxed),
      .i2s_ws_io(gpio_io[21]),
      .i2s_ws_o(i2s_ws_in_x_muxed),
      .i2s_ws_i(i2s_ws_out_x_muxed),
      .i2s_ws_oe_i(i2s_ws_oe_x_muxed),
      .i2s_sd_io(gpio_io[22]),
      .i2s_sd_o(i2s_sd_in_x_muxed),
      .i2s_sd_i(i2s_sd_out_x_muxed),
      .i2s_sd_oe_i(i2s_sd_oe_x_muxed),
      .spi2_cs_0_io(gpio_io[23]),
      .spi2_cs_0_o(spi2_cs_0_in_x_muxed),
      .spi2_cs_0_i(spi2_cs_0_out_x_muxed),
      .spi2_cs_0_oe_i(spi2_cs_0_oe_x_muxed),
      .spi2_cs_1_io(gpio_io[24]),
      .spi2_cs_1_o(spi2_cs_1_in_x_muxed),
      .spi2_cs_1_i(spi2_cs_1_out_x_muxed),
      .spi2_cs_1_oe_i(spi2_cs_1_oe_x_muxed),
      .spi2_sck_io(gpio_io[25]),
      .spi2_sck_o(spi2_sck_in_x_muxed),
      .spi2_sck_i(spi2_sck_out_x_muxed),
      .spi2_sck_oe_i(spi2_sck_oe_x_muxed),
      .spi2_sd_0_io(gpio_io[26]),
      .spi2_sd_0_o(spi2_sd_0_in_x_muxed),
      .spi2_sd_0_i(spi2_sd_0_out_x_muxed),
      .spi2_sd_0_oe_i(spi2_sd_0_oe_x_muxed),
      .spi2_sd_1_io(gpio_io[27]),
      .spi2_sd_1_o(spi2_sd_1_in_x_muxed),
      .spi2_sd_1_i(spi2_sd_1_out_x_muxed),
      .spi2_sd_1_oe_i(spi2_sd_1_oe_x_muxed),
      .spi2_sd_2_io(gpio_io[28]),
      .spi2_sd_2_o(spi2_sd_2_in_x_muxed),
      .spi2_sd_2_i(spi2_sd_2_out_x_muxed),
      .spi2_sd_2_oe_i(spi2_sd_2_oe_x_muxed),
      .spi2_sd_3_io(gpio_io[29]),
      .spi2_sd_3_o(spi2_sd_3_in_x_muxed),
      .spi2_sd_3_i(spi2_sd_3_out_x_muxed),
      .spi2_sd_3_oe_i(spi2_sd_3_oe_x_muxed),
      .i2c_scl_io(gpio_io[30]),
      .i2c_scl_o(i2c_scl_in_x_muxed),
      .i2c_scl_i(i2c_scl_out_x_muxed),
      .i2c_scl_oe_i(i2c_scl_oe_x_muxed),
      .i2c_sda_io(gpio_io[31]),
      .i2c_sda_o(i2c_sda_in_x_muxed),
      .i2c_sda_i(i2c_sda_out_x_muxed),
      .i2c_sda_oe_i(i2c_sda_oe_x_muxed),
      .pad_attributes_i('0)
  );

  assign clk_out_x = 1'b0;
  assign clk_oe_x = 1'b0;
  assign rst_nout_x = 1'b0;
  assign rst_noe_x = 1'b0;
  assign boot_select_out_x = 1'b0;
  assign boot_select_oe_x = 1'b0;
  assign execute_from_flash_out_x = 1'b0;
  assign execute_from_flash_oe_x = 1'b0;
  assign jtag_tck_out_x = 1'b0;
  assign jtag_tck_oe_x = 1'b0;
  assign jtag_tms_out_x = 1'b0;
  assign jtag_tms_oe_x = 1'b0;
  assign jtag_trst_nout_x = 1'b0;
  assign jtag_trst_noe_x = 1'b0;
  assign jtag_tdi_out_x = 1'b0;
  assign jtag_tdi_oe_x = 1'b0;
  assign jtag_tdo_oe_x = 1'b1;
  assign uart_rx_out_x = 1'b0;
  assign uart_rx_oe_x = 1'b0;
  assign uart_tx_oe_x = 1'b1;
  assign exit_valid_oe_x = 1'b1;


  always_comb begin
    pdm2pcm_pdm_in_x = 1'b0;
    gpio_18_in_x = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_PDM2PCM_PDM])
      0: begin
        pdm2pcm_pdm_out_x_muxed = pdm2pcm_pdm_out_x;
        pdm2pcm_pdm_oe_x_muxed = pdm2pcm_pdm_oe_x;
        pdm2pcm_pdm_in_x = pdm2pcm_pdm_in_x_muxed;
      end
      1: begin
        pdm2pcm_pdm_out_x_muxed = gpio_18_out_x;
        pdm2pcm_pdm_oe_x_muxed = gpio_18_oe_x;
        gpio_18_in_x = pdm2pcm_pdm_in_x_muxed;
      end
      default: begin
        pdm2pcm_pdm_out_x_muxed = pdm2pcm_pdm_out_x;
        pdm2pcm_pdm_oe_x_muxed = pdm2pcm_pdm_oe_x;
        pdm2pcm_pdm_in_x = pdm2pcm_pdm_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    pdm2pcm_clk_in_x = 1'b0;
    gpio_19_in_x = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_PDM2PCM_CLK])
      0: begin
        pdm2pcm_clk_out_x_muxed = pdm2pcm_clk_out_x;
        pdm2pcm_clk_oe_x_muxed = pdm2pcm_clk_oe_x;
        pdm2pcm_clk_in_x = pdm2pcm_clk_in_x_muxed;
      end
      1: begin
        pdm2pcm_clk_out_x_muxed = gpio_19_out_x;
        pdm2pcm_clk_oe_x_muxed = gpio_19_oe_x;
        gpio_19_in_x = pdm2pcm_clk_in_x_muxed;
      end
      default: begin
        pdm2pcm_clk_out_x_muxed = pdm2pcm_clk_out_x;
        pdm2pcm_clk_oe_x_muxed = pdm2pcm_clk_oe_x;
        pdm2pcm_clk_in_x = pdm2pcm_clk_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    i2s_sck_in_x = 1'b0;
    gpio_20_in_x = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_I2S_SCK])
      0: begin
        i2s_sck_out_x_muxed = i2s_sck_out_x;
        i2s_sck_oe_x_muxed = i2s_sck_oe_x;
        i2s_sck_in_x = i2s_sck_in_x_muxed;
      end
      1: begin
        i2s_sck_out_x_muxed = gpio_20_out_x;
        i2s_sck_oe_x_muxed = gpio_20_oe_x;
        gpio_20_in_x = i2s_sck_in_x_muxed;
      end
      default: begin
        i2s_sck_out_x_muxed = i2s_sck_out_x;
        i2s_sck_oe_x_muxed = i2s_sck_oe_x;
        i2s_sck_in_x = i2s_sck_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    i2s_ws_in_x  = 1'b0;
    gpio_21_in_x = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_I2S_WS])
      0: begin
        i2s_ws_out_x_muxed = i2s_ws_out_x;
        i2s_ws_oe_x_muxed = i2s_ws_oe_x;
        i2s_ws_in_x = i2s_ws_in_x_muxed;
      end
      1: begin
        i2s_ws_out_x_muxed = gpio_21_out_x;
        i2s_ws_oe_x_muxed = gpio_21_oe_x;
        gpio_21_in_x = i2s_ws_in_x_muxed;
      end
      default: begin
        i2s_ws_out_x_muxed = i2s_ws_out_x;
        i2s_ws_oe_x_muxed = i2s_ws_oe_x;
        i2s_ws_in_x = i2s_ws_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    i2s_sd_in_x  = 1'b0;
    gpio_22_in_x = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_I2S_SD])
      0: begin
        i2s_sd_out_x_muxed = i2s_sd_out_x;
        i2s_sd_oe_x_muxed = i2s_sd_oe_x;
        i2s_sd_in_x = i2s_sd_in_x_muxed;
      end
      1: begin
        i2s_sd_out_x_muxed = gpio_22_out_x;
        i2s_sd_oe_x_muxed = gpio_22_oe_x;
        gpio_22_in_x = i2s_sd_in_x_muxed;
      end
      default: begin
        i2s_sd_out_x_muxed = i2s_sd_out_x;
        i2s_sd_oe_x_muxed = i2s_sd_oe_x;
        i2s_sd_in_x = i2s_sd_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    spi2_cs_0_in_x = 1'b0;
    gpio_23_in_x   = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_SPI2_CS_0])
      0: begin
        spi2_cs_0_out_x_muxed = spi2_cs_0_out_x;
        spi2_cs_0_oe_x_muxed = spi2_cs_0_oe_x;
        spi2_cs_0_in_x = spi2_cs_0_in_x_muxed;
      end
      1: begin
        spi2_cs_0_out_x_muxed = gpio_23_out_x;
        spi2_cs_0_oe_x_muxed = gpio_23_oe_x;
        gpio_23_in_x = spi2_cs_0_in_x_muxed;
      end
      default: begin
        spi2_cs_0_out_x_muxed = spi2_cs_0_out_x;
        spi2_cs_0_oe_x_muxed = spi2_cs_0_oe_x;
        spi2_cs_0_in_x = spi2_cs_0_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    spi2_cs_1_in_x = 1'b0;
    gpio_24_in_x   = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_SPI2_CS_1])
      0: begin
        spi2_cs_1_out_x_muxed = spi2_cs_1_out_x;
        spi2_cs_1_oe_x_muxed = spi2_cs_1_oe_x;
        spi2_cs_1_in_x = spi2_cs_1_in_x_muxed;
      end
      1: begin
        spi2_cs_1_out_x_muxed = gpio_24_out_x;
        spi2_cs_1_oe_x_muxed = gpio_24_oe_x;
        gpio_24_in_x = spi2_cs_1_in_x_muxed;
      end
      default: begin
        spi2_cs_1_out_x_muxed = spi2_cs_1_out_x;
        spi2_cs_1_oe_x_muxed = spi2_cs_1_oe_x;
        spi2_cs_1_in_x = spi2_cs_1_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    spi2_sck_in_x = 1'b0;
    gpio_25_in_x  = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_SPI2_SCK])
      0: begin
        spi2_sck_out_x_muxed = spi2_sck_out_x;
        spi2_sck_oe_x_muxed = spi2_sck_oe_x;
        spi2_sck_in_x = spi2_sck_in_x_muxed;
      end
      1: begin
        spi2_sck_out_x_muxed = gpio_25_out_x;
        spi2_sck_oe_x_muxed = gpio_25_oe_x;
        gpio_25_in_x = spi2_sck_in_x_muxed;
      end
      default: begin
        spi2_sck_out_x_muxed = spi2_sck_out_x;
        spi2_sck_oe_x_muxed = spi2_sck_oe_x;
        spi2_sck_in_x = spi2_sck_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    spi2_sd_0_in_x = 1'b0;
    gpio_26_in_x   = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_SPI2_SD_0])
      0: begin
        spi2_sd_0_out_x_muxed = spi2_sd_0_out_x;
        spi2_sd_0_oe_x_muxed = spi2_sd_0_oe_x;
        spi2_sd_0_in_x = spi2_sd_0_in_x_muxed;
      end
      1: begin
        spi2_sd_0_out_x_muxed = gpio_26_out_x;
        spi2_sd_0_oe_x_muxed = gpio_26_oe_x;
        gpio_26_in_x = spi2_sd_0_in_x_muxed;
      end
      default: begin
        spi2_sd_0_out_x_muxed = spi2_sd_0_out_x;
        spi2_sd_0_oe_x_muxed = spi2_sd_0_oe_x;
        spi2_sd_0_in_x = spi2_sd_0_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    spi2_sd_1_in_x = 1'b0;
    gpio_27_in_x   = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_SPI2_SD_1])
      0: begin
        spi2_sd_1_out_x_muxed = spi2_sd_1_out_x;
        spi2_sd_1_oe_x_muxed = spi2_sd_1_oe_x;
        spi2_sd_1_in_x = spi2_sd_1_in_x_muxed;
      end
      1: begin
        spi2_sd_1_out_x_muxed = gpio_27_out_x;
        spi2_sd_1_oe_x_muxed = gpio_27_oe_x;
        gpio_27_in_x = spi2_sd_1_in_x_muxed;
      end
      default: begin
        spi2_sd_1_out_x_muxed = spi2_sd_1_out_x;
        spi2_sd_1_oe_x_muxed = spi2_sd_1_oe_x;
        spi2_sd_1_in_x = spi2_sd_1_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    spi2_sd_2_in_x = 1'b0;
    gpio_28_in_x   = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_SPI2_SD_2])
      0: begin
        spi2_sd_2_out_x_muxed = spi2_sd_2_out_x;
        spi2_sd_2_oe_x_muxed = spi2_sd_2_oe_x;
        spi2_sd_2_in_x = spi2_sd_2_in_x_muxed;
      end
      1: begin
        spi2_sd_2_out_x_muxed = gpio_28_out_x;
        spi2_sd_2_oe_x_muxed = gpio_28_oe_x;
        gpio_28_in_x = spi2_sd_2_in_x_muxed;
      end
      default: begin
        spi2_sd_2_out_x_muxed = spi2_sd_2_out_x;
        spi2_sd_2_oe_x_muxed = spi2_sd_2_oe_x;
        spi2_sd_2_in_x = spi2_sd_2_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    spi2_sd_3_in_x = 1'b0;
    gpio_29_in_x   = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_SPI2_SD_3])
      0: begin
        spi2_sd_3_out_x_muxed = spi2_sd_3_out_x;
        spi2_sd_3_oe_x_muxed = spi2_sd_3_oe_x;
        spi2_sd_3_in_x = spi2_sd_3_in_x_muxed;
      end
      1: begin
        spi2_sd_3_out_x_muxed = gpio_29_out_x;
        spi2_sd_3_oe_x_muxed = gpio_29_oe_x;
        gpio_29_in_x = spi2_sd_3_in_x_muxed;
      end
      default: begin
        spi2_sd_3_out_x_muxed = spi2_sd_3_out_x;
        spi2_sd_3_oe_x_muxed = spi2_sd_3_oe_x;
        spi2_sd_3_in_x = spi2_sd_3_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    i2c_scl_in_x = 1'b0;
    gpio_31_in_x = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_I2C_SCL])
      0: begin
        i2c_scl_out_x_muxed = i2c_scl_out_x;
        i2c_scl_oe_x_muxed = i2c_scl_oe_x;
        i2c_scl_in_x = i2c_scl_in_x_muxed;
      end
      1: begin
        i2c_scl_out_x_muxed = gpio_31_out_x;
        i2c_scl_oe_x_muxed = gpio_31_oe_x;
        gpio_31_in_x = i2c_scl_in_x_muxed;
      end
      default: begin
        i2c_scl_out_x_muxed = i2c_scl_out_x;
        i2c_scl_oe_x_muxed = i2c_scl_oe_x;
        i2c_scl_in_x = i2c_scl_in_x_muxed;
      end
    endcase
  end
  always_comb begin
    i2c_sda_in_x = 1'b0;
    gpio_30_in_x = 1'b0;
    unique case (pad_muxes[core_v_mini_mcu_pkg::PAD_I2C_SDA])
      0: begin
        i2c_sda_out_x_muxed = i2c_sda_out_x;
        i2c_sda_oe_x_muxed = i2c_sda_oe_x;
        i2c_sda_in_x = i2c_sda_in_x_muxed;
      end
      1: begin
        i2c_sda_out_x_muxed = gpio_30_out_x;
        i2c_sda_oe_x_muxed = gpio_30_oe_x;
        gpio_30_in_x = i2c_sda_in_x_muxed;
      end
      default: begin
        i2c_sda_out_x_muxed = i2c_sda_out_x;
        i2c_sda_oe_x_muxed = i2c_sda_oe_x;
        i2c_sda_in_x = i2c_sda_in_x_muxed;
      end
    endcase
  end

  // Pad control
  pad_control #(
      .reg_req_t(reg_pkg::reg_req_t),
      .reg_rsp_t(reg_pkg::reg_rsp_t),
      .NUM_PAD  (core_v_mini_mcu_pkg::NUM_PAD)
  ) u_pad_control (
      .clk_i(clk_in_x),
      .rst_ni(rst_ngen),
      .reg_req_i(pad_req),
      .reg_rsp_o(pad_resp),
      .pad_muxes_o(pad_muxes)
  );

  rstgen rstgen_i (
      .clk_i(clk_in_x),
      .rst_ni(rst_nin_x),
      .test_mode_i(1'b0),
      .rst_no(rst_ngen),
      .init_no()
  );



endmodule  
