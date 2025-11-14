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

parameter int unsigned NbInstr = 33;

typedef enum logic[6:0] {
  ILLEGAL            = 7'b0000000,
  KECCAK             = 7'b0000001,
  REDUCTION          = 7'b0000010,
  GF                 = 7'b0000011,
  SAMPLING           = 7'b0000100,
  COMPRESS           = 7'b0000101,
  VEC                = 7'b0000110,
  VEC_G              = 7'b0000111
} opcode_t;


// Opcode of the new instruction
typedef enum logic [6:0] {
  nada                            = 7'b0000000,
  keccak_rol32_h                  = 7'b0000001,
  keccak_rol32_l                  = 7'b0000010,
  bcop32                          = 7'b0000011,
  karats_1                        = 7'b0000100,
  karats_2                        = 7'b0000101,
  karats_3                        = 7'b0000110,
  karats_4                        = 7'b0000111,
  gf_carryless_mul                = 7'b0001011,
  montg_kyber                     = 7'b0010010,
  montg_newhope                   = 7'b0001100,
  montg_falcon                    = 7'b0001101,
  montg_ntru                      = 7'b0001110,
  montg_dilithium                 = 7'b0010000,
  //barrett_kyber                   = 7'b0010001,
  cbd_eta1                        = 7'b0010111,
  cbd_eta2                        = 7'b0011000,
  cbd_eta3                        = 7'b0011001,
  cbd_eta4                        = 7'b0011010,
  rej12x2                         = 7'b0011011,
  rej23x1                         = 7'b0011100,
  rej8x4                          = 7'b0011101,
  compress1                       = 7'b0011110,
  compress2                       = 7'b0011111,
  compress3                       = 7'b0100000,
  compress4                       = 7'b0100001,
  vec_by_matrix                   = 7'b0100010,
  restr_vec_by_matrix             = 7'b0100011,
  double_zero_norm                = 7'b0100100,
  vec_by_vec_pointwise            = 7'b0100101,
  synd_minus_vec_scaled           = 7'b0100110,
  restr_to_val_G                  = 7'b0100111,
  vec_by_matrix_G                 = 7'b0101000,
  restr_vec_by_matrix_G           = 7'b0101001,
  synd_minus_vec_scaled_G         = 7'b0101010

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


  // GF instructions
  '{
      instr: 32'b0000111_00000_00000_001_00000_0111011,  // GF-Carryless opcode - gf-carryless
      mask:  32'b1111111_00000_00000_111_00000_1111111, 
      resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: gf_carryless_mul, done : 1'b1},
      opcode : GF
    },
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


  // Sampling instructions
  '{
    instr: 32'b0011110_00000_00000_001_00000_0111011,  // SAMPLING opcode - cbd_eta1  - 1E 
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd_eta1, done : 1'b1},
    opcode : SAMPLING
  },
'{
    instr: 32'b0011111_00000_00000_001_00000_0111011,  // SAMPLING opcode - cbd_eta2 - h1F
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd_eta2, done : 1'b1},
    opcode : SAMPLING
  },
'{
    instr: 32'b0100000_00000_00000_001_00000_0111011,  // SAMPLING opcode - cbd_eta3 - h20
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd_eta3, done : 1'b1},
    opcode : SAMPLING
  },
'{
    instr: 32'b0100001_00000_00000_001_00000_0111011,  // SAMPLING opcode - cbd_eta4 - h21
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: cbd_eta4, done : 1'b1},
    opcode : SAMPLING
  },
'{
    instr: 32'b00000_00_00000_00000_010_00000_1101011,  // SAMPLING opcode - rej12x2
    mask:  32'b00000_11_00000_00000_111_00000_1111111, 
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: rej12x2, done : 1'b1},
    opcode : SAMPLING
  },
'{
    instr: 32'b00000_01_00000_00000_010_00000_1101011,  // SAMPLING opcode - rej23x1
    mask:  32'b00000_11_00000_00000_111_00000_1111111, 
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: rej23x1, done : 1'b1},
    opcode : SAMPLING
  },
'{
    instr: 32'b00000_10_00000_00000_010_00000_1101011,  // SAMPLING opcode - rej8x4
    mask:  32'b00000_11_00000_00000_111_00000_1111111, 
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: rej8x4, done : 1'b1},
    opcode : SAMPLING
  },

  //Reduction instructions
  '{
    instr: 32'b0000000_00000_00000_111_00000_0111011,  // REDUCTION opcode - montg_kyber
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: montg_kyber, done : 1'b1},
    opcode : REDUCTION
  },  
  '{
    instr: 32'b0000001_00000_00000_111_00000_0111011,  // REDUCTION opcode - montg_newhope
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: montg_newhope, done : 1'b1},
    opcode : REDUCTION
  },  
  '{
    instr: 32'b0000010_00000_00000_111_00000_0111011,  // REDUCTION opcode - montg_falcon
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: montg_falcon, done : 1'b1},
    opcode : REDUCTION
  },  
  '{
    instr: 32'b0000011_00000_00000_111_00000_0111011,  // REDUCTION opcode - montg_ntru
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: montg_ntru, done : 1'b1},
    opcode : REDUCTION
  },  
  '{
    instr: 32'b0000100_00000_00000_111_00000_0111011,  // REDUCTION opcode - montg_dilithium
    mask:  32'b1111111_00000_00000_111_00000_1111111,
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: montg_dilithium, done : 1'b1},
    opcode : REDUCTION
  }, 
  //'{
  //  instr: 32'b0000101_00000_00000_111_00000_0111011,  // REDUCTION opcode - barrett_k
  //  mask:  32'b1111111_00000_00000_111_00000_1111111,
  //  resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b01, insn: barrett_kyber, done : 1'b1},
  //  opcode : REDUCTION
  //}, 

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
    },


    //VEC
  '{
    instr: 32'b00000_00_00000_00000_011_00000_1101011,  // VEC opcode - vec_by_matrix
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x03, 0x0
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: vec_by_matrix, done : 1'b1},
    opcode : VEC
  },

  '{
    instr: 32'b00000_01_00000_00000_011_00000_1101011,  // VEC opcode - restr_vec_by_matrix
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x03, 0x01
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: restr_vec_by_matrix, done : 1'b1},
    opcode : VEC
  },
  
  '{
    instr: 32'b00000_10_00000_00000_011_00000_1101011,  // VEC opcode - double_zero_norm
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x02, 0x02
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: double_zero_norm, done : 1'b1},
    opcode : VEC
  },
  
  '{
    instr: 32'b00000_11_00000_00000_011_00000_1101011,  // VEC opcode - vec_by_vec_pointwise
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x03, 0x03
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: vec_by_vec_pointwise, done : 1'b1},
    opcode : VEC
  },

  '{
    instr: 32'b00000_00_00000_00000_100_00000_1101011,  // VEC opcode - synd_minus_vec_scaled
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x04, 0x00
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: synd_minus_vec_scaled, done : 1'b1},
    opcode : VEC
  },

  '{
    instr: 32'b00000_00_00000_00000_101_00000_1101011,  // VEC opcode - restr_to_val_G
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x05, 0x0
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: restr_to_val_G, done : 1'b1},
    opcode : VEC_G
  },

  '{
    instr: 32'b00000_01_00000_00000_101_00000_1101011,  // VEC opcode - vec_by_matrix_G
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x05, 0x01
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: vec_by_matrix_G, done : 1'b1},
    opcode : VEC_G
  },

  '{
    instr: 32'b00000_10_00000_00000_101_00000_1101011,  // VEC opcode - restr_vec_by_matrix_G
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x05, 0x02
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: restr_vec_by_matrix_G, done : 1'b1},
    opcode : VEC_G
  },

  '{
    instr: 32'b00000_11_00000_00000_101_00000_1101011,  // VEC opcode - synd_minus_vec_scaled_G
    mask:  32'b00000_11_00000_00000_111_00000_1111111, //  0x6b, 0x05, 0x03
    resp : '{accept : 1'b1, writeback : 1'b1, register_read : {1'b1, 1'b1, 1'b1}, op_type : 2'b10, insn: synd_minus_vec_scaled_G, done : 1'b1},
    opcode : VEC_G
  }    
};



typedef struct packed {
  logic [31:0] rs1_0;
  logic [31:0] rs2_0;
  logic [31:0] rs3_0;
} in_t;

typedef struct packed {
    logic [31:0] rd1;
} out_t;

endpackage