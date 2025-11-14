///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2025 PoliTO - @VLSI Lab
// Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
// SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1
//
// Auth: Alessandra Dolmeta, Valeria Piscopo
// Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
// Affiliation: Politecnico di Torino - @VLSI Lab
// Date: October 2025
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef HQC1_INSTRUCTIONS_H
#define HQC1_INSTRUCTIONS_H

/* Set to 1 to enable the hardware-accelerated/custom version,
   or 0 to fall back to the reference (“clean”) implementation. */
#define ENABLE_KARATSUBA      1
#define ENABLE_GF_CARRYLESS   1
#define ENABLE_KECCAK         1

#endif /* HQC1_INSTRUCTIONS_H */
