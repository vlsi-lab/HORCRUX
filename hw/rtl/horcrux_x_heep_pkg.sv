///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: horcrux_x_heep_pkg.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

package horcrux_x_heep_pkg;

  import addr_map_rule_pkg::*;
  import core_v_mini_mcu_pkg::*;

  //master idx
  localparam logic [31:0] EXT_MASTER0_IDX = 0;

  // There's no Master o Slave port since horcrux connected as a slave peripheral
  localparam EXT_XBAR_NMASTER = 1;
  // One slave port
  localparam EXT_XBAR_NSLAVE = 1;

  //slave mmap and idx
  localparam logic [31:0] HORCRUX_START_ADDRESS = core_v_mini_mcu_pkg::EXT_SLAVE_START_ADDRESS + 32'h000000;
  localparam logic [31:0] HORCRUX_SIZE = 32'h100000;
  localparam logic [31:0] HORCRUX_END_ADDRESS = HORCRUX_START_ADDRESS + HORCRUX_SIZE;
  localparam logic [31:0] HORCRUX_IDX = 32'd0;

  localparam addr_map_rule_t [EXT_XBAR_NSLAVE-1:0] EXT_XBAR_ADDR_RULES = '{
      '{idx: HORCRUX_IDX, start_addr: HORCRUX_START_ADDRESS, end_addr: HORCRUX_END_ADDRESS}
  };

  //slave encoder
  localparam EXT_SYSTEM_NPERIPHERALS = 1;

  localparam logic [31:0] HORCRUX_PERIPH_START_ADDRESS = core_v_mini_mcu_pkg::EXT_PERIPHERAL_START_ADDRESS + 32'h0000000;
  localparam logic [31:0] HORCRUX_PERIPH_SIZE = 32'h0001000;
  localparam logic [31:0] HORCRUX_PERIPH_END_ADDRESS = HORCRUX_PERIPH_START_ADDRESS + HORCRUX_PERIPH_SIZE;
  localparam logic [31:0] HORCRUX_PERIPH_IDX = 32'd0;

  localparam addr_map_rule_t [EXT_SYSTEM_NPERIPHERALS-1:0] EXT_PERIPHERALS_ADDR_RULES = '{
      '{
          idx: HORCRUX_PERIPH_IDX,
          start_addr: HORCRUX_PERIPH_START_ADDRESS,
          end_addr: HORCRUX_PERIPH_END_ADDRESS
      }
  };

  localparam int unsigned EXT_PERIPHERALS_PORT_SEL_WIDTH = EXT_SYSTEM_NPERIPHERALS > 1 ? $clog2(
      EXT_SYSTEM_NPERIPHERALS
  ) : 32'd1;

endpackage  // HORCRUX_x_heep_pkg
