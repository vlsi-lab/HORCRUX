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

#ifndef MLKEM512_INSTRUCTIONS_H
#define MLKEM512_INSTRUCTIONS_H

/* Set to 1 to enable the hardware-accelerated/custom version,
   or 0 to fall back to the reference (“clean”) implementation. */
#define ENABLE_KYBER_NTT       1
#define ENABLE_KYBER_INTT      1
#define ENABLE_KYBER_BARRETT   1
#define ENABLE_KYBER_MONTG     1
#define ENABLE_KYBER_CBD       1
#define ENABLE_KYBER_CBD3      1
#define ENABLE_KECCAK          1
#define ENABLE_COMPRESS        1
#define ENABLE_COMPRESS2       1

#endif /* MLKEM512_INSTRUCTIONS_H */
