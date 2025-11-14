# Copyright 2025 PoliTO - @VLSI Lab
# Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
# SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1

# Auth: Alessandra Dolmeta, Valeria Piscopo
# Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
# Affiliation: Politecnico di Torino - @VLSI Lab
# Date: October 2025


# Compiled application has the same name of its folder
APPLI   = $(shell basename $(CURDIR))
SOURCES = 
SRC     = main.c

INC         = -I include/ 
EXTRA_CFLAGS = -g -O2

# Conditional flags based on the defined parameters
ifdef NTESTS
EXTRA_CFLAGS += -DNTESTS=$(NTESTS)
endif

# Performance counters
PERF_CNT_CYCLES ?= 0
PERF_CNT_INSTR  ?= 0

# Add the test parameters to EXTRA_CFLAGS
EXTRA_CFLAGS += 

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