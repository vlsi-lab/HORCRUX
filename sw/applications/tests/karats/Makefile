# /////////////////////////////////////////////////////////////////////////////
# ****************************************************************************
#                            		CEA 
# ****************************************************************************
# Author : Alessandra Dolmeta
# Date	 : March 2024
# ****************************************************************************

# Compiled application has the same name of its folder
APPLI   = $(shell basename $(CURDIR))
SRC     = main.c


INC     	 = -I include/
EXTRA_CFLAGS = -g -O2

# Number of tests
ifdef NTESTS
EXTRA_CFLAGS += DNTESTS=$(NTESTS)
endif


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
