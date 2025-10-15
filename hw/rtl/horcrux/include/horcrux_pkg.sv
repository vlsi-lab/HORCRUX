///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// HORCRUX: horcrux_pkg.sv
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

package horcrux_pkg;

parameter int unsigned NbInstr = 32;

typedef enum logic[6:0] {
  ILLEGAL            = 7'b0000000,
  KECCAK             = 7'b0000001,
  REDUCTION          = 7'b0000010,
  GF                 = 7'b0000011,
  CBD                = 7'b0000100,
  COMPRESS           = 7'b0000101
} opcode_t;


// Opcode of the new instruction
typedef enum logic [6:0] {
  nada                = 7'b0000000,
  keccak_rol32_h      = 7'b0000001,
  keccak_rol32_l      = 7'b0000010,
  bcop32              = 7'b0000011,
  karats_1            = 7'b0000100,
  karats_2            = 7'b0000101,
  karats_3            = 7'b0000110,
  karats_4            = 7'b0000111,
  montg_k             = 7'b0001000,
  montg_d             = 7'b0001001,
  gf_carryless_mul_1  = 7'b0001011,
  gf_carryless_mul_2  = 7'b0001100,
  gf_carryless_mul_3  = 7'b0001101,
  gf_carryless_mul_4  = 7'b0001110,
  barrett_k           = 7'b0010000,
  barrett_d           = 7'b0010001,
  cbd3_1              = 7'b0010010,
  cbd3_2              = 7'b0010011,
  cbd3_3              = 7'b0010100,
  cbd3_4              = 7'b0010101,
  cbd2_1              = 7'b0010110,
  cbd2_2              = 7'b0010111,
  cbd2_3              = 7'b0011000,
  cbd2_4              = 7'b0011001,
  cbd2_5              = 7'b0011010,
  cbd2_6              = 7'b0011011,
  cbd2_7              = 7'b0011100,
  cbd2_8              = 7'b0011101,
  compress1           = 7'b0011110,
  compress2           = 7'b0011111,
  compress3           = 7'b0100000,
  compress4           = 7'b0100001,
  rej_uniform         = 7'b0100010
} horcrux_insn;


typedef struct packed {
  logic accept;
  logic writeback;       
  logic [2:0] register_read;
  logic [1:0] op_type;    //10 if R4, 01 if R
  horcrux_insn insn;
  logic done;
} issue_resp_t;

typedef struct packed {
  logic [31:0] instr;
  logic [31:0] mask;
  issue_resp_t resp;
  opcode_t     opcode;
} copro_issue_resp_t;


parameter copro_issue_resp_t CoproInstr[NbInstr] = '{
// Keccak instructions
  '{
    instr: 32'b00000_00_00000_00000_001_00000_1101011,  // KECCAK opcode - ROL32_H
    mask:  32'b00000_11_00000_00000_111_00000_1111111,  
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: keccak_rol32_h, done : 1'b1},
    opcode : KECCAK
  },

  '{
    instr: 32'b00000_01_00000_00000_001_00000_1101011,  // KECCAK opcode - ROL32_L
    mask:  32'b00000_11_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: keccak_rol32_l, done : 1'b1},
    opcode : KECCAK
  },

  '{
    instr: 32'b00000_10_00000_00000_001_00000_1101011,  // KECCAK opcode - bcop32
    mask:  32'b00000_11_00000_00000_111_00000_1111111, 
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: bcop32, done : 1'b1},
    opcode : KECCAK
  },

  //'{
  //  instr: 32'b00000_11_00000_00000_001_00000_1101011,  // KECCAK opcode - xor3
  //  mask:  32'b00000_11_00000_00000_111_00000_1111111, 
  //  resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: xor3, done : 1'b1},
  //  opcode : KECCAK
  //},      
// GF instructions
  '{
    instr: 32'b0000001_00000_00000_001_00000_0111011,  // GF opcode - karats_1
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: karats_1, done : 1'b1},
    opcode : GF
  },

  '{
    instr: 32'b0000010_00000_00000_001_00000_0111011,  // GF opcode - karats_2
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: karats_2, done : 1'b1},
    opcode : GF
  },

  '{
    instr: 32'b0000011_00000_00000_001_00000_0111011,  // GF opcode - karats_3
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: karats_3, done : 1'b1},
    opcode : GF
  },

  '{
    instr: 32'b0000100_00000_00000_001_00000_0111011,  // GF opcode - karats_4
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: karats_4, done : 1'b1},
    opcode : GF
  },

  '{
      instr: 32'b0001001_00000_00000_001_00000_0111011,  // GF opcode - gf_carryless_mul_1
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: gf_carryless_mul_1, done : 1'b1},
      opcode : GF
    },

  '{
      instr: 32'b0001010_00000_00000_001_00000_0111011,  // GF opcode - gf_carryless_mul_2
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: gf_carryless_mul_2, done : 1'b1},
      opcode : GF
    },

  '{
      instr: 32'b0001011_00000_00000_001_00000_0111011,  // GF opcode - gf_carryless_mul_3
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: gf_carryless_mul_3, done : 1'b1},
      opcode : GF
    },

  '{
      instr: 32'b0001100_00000_00000_001_00000_0111011,  // GF opcode - gf_carryless_mul_4
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: gf_carryless_mul_4, done : 1'b1},
      opcode : GF
    },

// Reduction instructions
  '{
    instr: 32'b0000101_00000_00000_001_00000_0111011,  // REDUCTION opcode - montg_k
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: montg_k, done : 1'b1},
    opcode : REDUCTION
  },      
  
  '{
    instr: 32'b0000110_00000_00000_001_00000_0111011,  // REDUCTION opcode - montg_d
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: montg_d, done : 1'b1},
    opcode : REDUCTION
  },

  '{
    instr: 32'b0010000_00000_00000_001_00000_0111011,  // REDUCTION opcode - barrett_k
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: barrett_k, done : 1'b1},
    opcode : REDUCTION
  },
'{
    instr: 32'b0010001_00000_00000_001_00000_0111011,  // REDUCTION opcode - barrett_d
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: barrett_d, done : 1'b1},
    opcode : REDUCTION
  },

// CBD instructions
  '{
      instr: 32'b0011001_00000_00000_001_00000_0111011,  // CBD opcode - cbd3_1
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd3_1, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0011010_00000_00000_001_00000_0111011,  // CBD opcode - cbd3_2
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd3_2, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0011011_00000_00000_001_00000_0111011,  // CBD opcode - cbd3_3
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd3_3, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0011100_00000_00000_001_00000_0111011,  // CBD opcode - cbd3_4
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd3_4, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0011101_00000_00000_001_00000_0111011,  // CBD opcode - cbd2_1
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd2_1, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0011110_00000_00000_001_00000_0111011,  // CBD opcode - cbd2_2
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd2_2, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0011111_00000_00000_001_00000_0111011,  // CBD opcode - cbd2_3
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd2_3, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0100000_00000_00000_001_00000_0111011,  // CBD opcode - cbd2_4
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd2_4, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0100001_00000_00000_001_00000_0111011,  // CBD opcode - cbd2_5
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd2_5, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0100010_00000_00000_001_00000_0111011,  // CBD opcode - cbd2_6
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd2_6, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0100011_00000_00000_001_00000_0111011,  // CBD opcode - cbd2_7
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd2_7, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b0100100_00000_00000_001_00000_0111011,  // CBD opcode - cbd2_8
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd2_8, done : 1'b1},
      opcode : CBD
    },
  '{
      instr: 32'b00000_01_00000_00000_010_00000_1101011,  // CBD opcode - rej_uniform
      mask:  32'b00000_11_00000_00000_111_00000_1111111, 
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: rej_uniform, done : 1'b1},
      opcode : CBD
    },
// Compression instructions
  '{
      instr: 32'b0100101_00000_00000_001_00000_0111011,  // COMPRESS opcode - compress1
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: compress1, done : 1'b1},
      opcode : COMPRESS
    },
  '{
      instr: 32'b0100110_00000_00000_001_00000_0111011,  // COMPRESS opcode - compress2
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: compress2, done : 1'b1},
      opcode : COMPRESS
    },
  '{
      instr: 32'b0100111_00000_00000_001_00000_0111011,  // COMPRESS opcode - compress3
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: compress3, done : 1'b1},
      opcode : COMPRESS
    },
  '{
      instr: 32'b0101000_00000_00000_001_00000_0111011,  // COMPRESS opcode - compress4
      mask:  32'b1111111_00000_00000_111_00000_1111111,
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: compress4, done : 1'b1},
      opcode : COMPRESS
    }
};


// funct3 for horcrux_R intruction
typedef enum logic [2:0] {
  OP_R_R0 = 3'b000, //0
  OP_R_R1 = 3'b001, //1
  OP_R_R2 = 3'b010, //2
  OP_R_R3 = 3'b011, //3
  OP_R_R4 = 3'b100, //4
  OP_R_R5 = 3'b101, //5
  OP_R_R6 = 3'b110, //6
  OP_R_R7 = 3'b111  //7
} horcrux_r_op;

// funct2 for horcrux_R4 intruction
typedef enum logic [1:0] {
  OP_R4_R0 = 2'b00, //1
  OP_R4_R1 = 2'b01, //1
  OP_R4_R2 = 2'b10, //2
  OP_R4_R3 = 2'b11 //3
} horcrux_r4_op;

// Mode of operation of horcrux
typedef enum logic [6:0] {
  FUNCT7_0         = 7'b0000000, //0
  FUNCT7_1         = 7'b0000001, //1
  FUNCT7_2         = 7'b0000010, //2
  FUNCT7_3         = 7'b0000011, //3
  FUNCT7_4         = 7'b0000100, //4
  FUNCT7_5         = 7'b0000101, //5
  FUNCT7_6         = 7'b0000110, //6
  FUNCT7_7         = 7'b0000111, //7
  FUNCT7_8         = 7'b0001000, //8
  FUNCT7_9         = 7'b0001001, //9
  FUNCT7_10        = 7'b0001010, //10
  FUNCT7_11        = 7'b0001011, //11
  FUNCT7_12        = 7'b0001100, //12
  FUNCT7_13        = 7'b0001101, //13
  FUNCT7_14        = 7'b0001110, //14
  FUNCT7_15        = 7'b0001111, //15
  FUNCT7_16        = 7'b0010000, //16
  FUNCT7_17        = 7'b0010001, //17
  FUNCT7_18        = 7'b0010010, //18
  FUNCT7_19        = 7'b0010011, //19
  FUNCT7_20        = 7'b0010100, //20
  FUNCT7_21        = 7'b0010101, //21
  FUNCT7_22        = 7'b0010110, //22
  FUNCT7_23        = 7'b0010111, //23
  FUNCT7_24        = 7'b0011000, //24
  FUNCT7_25        = 7'b0011001, //25
  FUNCT7_26        = 7'b0011010, //26
  FUNCT7_27        = 7'b0011011, //27
  FUNCT7_28        = 7'b0011100, //28
  FUNCT7_29        = 7'b0011101, //29
  FUNCT7_30        = 7'b0011110, //30
  FUNCT7_31        = 7'b0011111, //31
  FUNCT7_32        = 7'b0100000, //32
  FUNCT7_33        = 7'b0100001, //33
  FUNCT7_34        = 7'b0100010, //34
  FUNCT7_35        = 7'b0100011, //35
  FUNCT7_36        = 7'b0100100, //36
  FUNCT7_37        = 7'b0100101, //37
  FUNCT7_38        = 7'b0100110, //38
  FUNCT7_39        = 7'b0100111 //39
} horcrux_funct7;

// funct3 for horcrux_I intruction
typedef enum logic [2:0] {
  FUNCT3_1   = 3'b001,
  FUNCT3_2   = 3'b010,
  FUNCT3_3   = 3'b011,
  FUNCT3_4   = 3'b100,
  FUNCT3_5   = 3'b101,
  FUNCT3_6   = 3'b110,
  FUNCT3_7   = 3'b111
} horcrux_funct3;

// New instruction definition
typedef union packed {
  struct packed {
    horcrux_funct7 funct7;  // 31:25
    logic [4:0] rs2;     // 24:20
    logic [4:0] rs1;     // 19:15
    horcrux_r_op funct3;  // 14:12
    logic [4:0] rd;      // 11:7
    logic [6:0] opcode;  // 6:0
  } as_horcrux_R;
  struct packed {
    logic [4:0] rs3;  // 31:27
    horcrux_r4_op funct2;  // 26:25
    logic [4:0] rs2;     // 24:20
    logic [4:0] rs1;     // 19:15
    horcrux_funct3 funct3;  // 14:12
    logic [4:0] rd;      // 11:7
    logic [6:0] opcode;  // 6:0
  } as_horcrux_R4;
  logic [31:0] raw;
} instruction_u;

// Opcode of the new instruction
typedef enum logic [6:0] {
  horcrux_R  = 7'b0111011, //3b
  horcrux_R4  = 7'b1101011 //6b
} horcrux_op;

typedef struct packed {
  logic [2:0] mode_horcrux;
} mode_t;

typedef struct packed {
  logic [6:0] funct7_horcrux;
} funct7_t;

typedef struct packed {
  logic [1:0] funct2_horcrux;
} funct2_t;

typedef struct packed {
  logic [11:0] immediate_horcrux;
} immediate_t;

typedef struct packed {
  logic [31:0] rs1_0;
  logic [31:0] rs2_0;
  logic [31:0] rs3_0;
} in_t;

typedef struct packed {
    logic [31:0] rd1;
} out_t;

endpackage