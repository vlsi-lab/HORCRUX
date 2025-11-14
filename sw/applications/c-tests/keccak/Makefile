# Copyright 2025 PoliTO - @VLSI Lab
# Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
# SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1

# Auth: Alessandra Dolmeta, Valeria Piscopo
# Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
# Affiliation: Politecnico di Torino - @VLSI Lab
# Date: October 2025


APPLI   = $(shell basename $(CURDIR))
SRC     = keccak32.c main.c

INC         = -I include/ 
EXTRA_CFLAGS = -g -O2

# Debug print statements
$(info APPLI: $(APPLI))
$(info EXTRA_CFLAGS: $(EXTRA_CFLAGS))
$(info PRINT_VECT: $(PRINT_VECT))
# Compiler
CC = gcc

# Targets
all: $(APPLI)

$(APPLI): $(SRC)
	$(CC) $(SRC) $(INC) $(EXTRA_CFLAGS) -o $(APPLI)

clean:
	rm -f $(APPLI)

.PHONY: all clean