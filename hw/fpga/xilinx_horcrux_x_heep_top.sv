// Copyright 2023 PoliTO
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//

// horcrux_x_heep_top : wrapper of the accelerator and x_heep
// Designed by Alessandra Dolmeta, Mattia Mirigaldi, Valeria Piscopo
// alessandra.dolmeta@polito.it, mattia.mirigaldi@polito.it, valeria.piscopo@polito.it
//

module xilinx_horcrux_x_heep_top
  import obi_pkg::*;
  import reg_pkg::*;
#( 
    parameter COREV_PULP           = 0,
    parameter FPU                  = 0,
    parameter ZFINX                = 0,
    parameter CLK_LED_COUNT_LENGTH = 27
) (

`ifdef FPGA_ZCU104
    inout logic clk_300mhz_n,
    inout logic clk_300mhz_p,
`elsif FPGA_ZCU102
    inout logic clk_125mhz_n,
    inout logic clk_125mhz_p,
`else
    inout logic clk_i,
`endif
    inout logic rst_i,

    output logic rst_led_o,
    output logic clk_led_o,

    inout logic boot_select_i,
    inout logic execute_from_flash_i,

    inout logic jtag_tck_i,
    inout logic jtag_tms_i,
    inout logic jtag_trst_ni,
    inout logic jtag_tdi_i,
    inout logic jtag_tdo_o,

    inout logic uart_rx_i,
    inout logic uart_tx_o,

    inout logic [17:0] gpio_io,

    output logic exit_value_o,
    inout  logic exit_valid_o,

    inout logic [3:0] spi_flash_sd_io,
    inout logic spi_flash_csb_o,
    inout logic spi_flash_sck_o,

    inout logic [3:0] spi_sd_io,
    inout logic spi_csb_o,
    inout logic spi_sck_o,

    inout logic [3:0] spi2_sd_io,
    inout logic [1:0] spi2_csb_o,
    inout logic spi2_sck_o,

    inout logic i2c_scl_io,
    inout logic i2c_sda_io,

    inout logic pdm2pcm_clk_io,
    inout logic pdm2pcm_pdm_io,

    inout logic i2s_sck_io,
    inout logic i2s_ws_io,
    inout logic i2s_sd_io
);

  wire                               clk_gen;
  logic [                      31:0] exit_value;
  wire                               rst_n;
  logic [CLK_LED_COUNT_LENGTH - 1:0] clk_count;
  logic [                      31:0] gpio;
  logic                              spi_flash_csb_unused;
  logic                              spi_csb_unused;

 

  // Adpating gpio
   assign gpio[17:0] = gpio_io;
   assign gpio[18] = pdm2pcm_pdm_io;
   assign gpio[19] = pdm2pcm_clk_io;
   assign gpio[20] = i2s_sck_io;		
   assign gpio[21] = i2s_ws_io;
   assign gpio[22] = i2s_sd_io;  
   assign gpio[24:23] = spi2_csb_o;
   assign gpio[25] = spi2_sck_o;
   assign gpio[26] = spi2_sd_io[0];
   assign gpio[27] = spi2_sd_io[1];
   assign gpio[28] = spi2_sd_io[2];
   assign gpio[29] = spi2_sd_io[3];
   assign gpio[30] = i2c_scl_io;
   assign gpio[31] = i2c_sda_io;
   
		    
			     
  // low active reset
  assign rst_n   = !rst_i;

  // reset LED for debugging
  assign rst_led_o = rst_n;

  // counter to blink an LED
  assign clk_led_o = clk_count[CLK_LED_COUNT_LENGTH-1];

  always_ff @(posedge clk_gen or negedge rst_n) begin : clk_count_process
    if (!rst_n) begin
      clk_count <= '0;
    end else begin
      clk_count <= clk_count + 1;
    end
  end

  // clock output for debugging
  //assign clk_out = clk_gen;

`ifdef FPGA_ZCU104
  xilinx_clk_wizard_wrapper xilinx_clk_wizard_wrapper_i (
    .CLK_IN1_D_0_clk_n(clk_300mhz_n),
    .CLK_IN1_D_0_clk_p(clk_300mhz_p),
    .clk_out1_0       (clk_gen)
  );
`elsif FPGA_ZCU102
  xilinx_clk_wizard_wrapper xilinx_clk_wizard_wrapper_i (
      .CLK_IN1_D_0_clk_n(clk_125mhz_n),
      .CLK_IN1_D_0_clk_p(clk_125mhz_p),
      .clk_out1_0(clk_gen)
  );
`elsif FPGA_NEXYS
  xilinx_clk_wizard_wrapper xilinx_clk_wizard_wrapper_i (
    .clk_100MHz(clk_i),
    .clk_out1_0(clk_gen)
  );
`else  // FPGA PYNQ-Z2
  xilinx_clk_wizard_wrapper xilinx_clk_wizard_wrapper_i (
    .clk_125MHz(clk_i),
    .clk_out1_0(clk_gen)
  );
`endif

  // eXtension Interface
  if_xif #() ext_if ();
    
    horcrux_x_heep_top  #(
      .COREV_PULP(COREV_PULP),
      .FPU(FPU),
      .ZFINX(ZFINX)
    ) horcrux_x_heep_top_i (
        .clk_i(clk_gen),
        .rst_ni(rst_n),
        .boot_select_i(boot_select_i),
        .execute_from_flash_i(execute_from_flash_i),
        .jtag_tck_i(jtag_tck_i),
        .jtag_tms_i(jtag_tms_i),
        .jtag_trst_ni(jtag_trst_ni),
        .jtag_tdi_i(jtag_tdi_i),
        .jtag_tdo_o(jtag_tdo_o),
        .uart_rx_i(uart_rx_i),
        .uart_tx_o(uart_tx_o),
        .gpio_io(gpio),
        .spi_flash_sd_io(spi_flash_sd_io),
        .spi_flash_csb({spi_flash_csb_unused, spi_flash_csb_o}),
        .spi_flash_sck(spi_flash_sck_o),
        .spi_sd_io(spi_sd_io),
        .spi_csb({spi_csb_unused, spi_csb_o}),
        .spi_sck(spi_sck_o),
        .external_subsystem_powergate_switch_o(),
        .external_subsystem_powergate_switch_ack_i(),
        .external_subsystem_powergate_iso_o(),
        .external_subsystem_rst_no(),
        .external_ram_banks_set_retentive_o(),
        .exit_value_o(exit_value),
        .exit_valid_o(exit_valid_o)
    );

  assign exit_value_o = exit_value[0];


endmodule