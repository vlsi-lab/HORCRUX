# Copyright 2025 PoliTO - @VLSI Lab
# Solderpad Hardware License, Version 2.1, see LICENSE.md for details.
# SPDX-License-Identifier: Apache-2.0 WITH SHL-2.1

# Auth: Alessandra Dolmeta, Valeria Piscopo
# Email: alessandra.dolmeta@polito.it, valeria.piscopo@polito.it
# Affiliation: Politecnico di Torino - @VLSI Lab
# Date: October 2025

# Makefile to generates keccak-x-heep files and build the design with fusesoc

MAKE                       = make
ROOT_DIR			:= $(realpath .)
BUILD_DIR			:= $(ROOT_DIR)/build
LOG_DIR				:= $(BUILD_DIR)/logs

# Get the absolute path
mkfile_path := $(shell dirname "$(realpath $(firstword $(MAKEFILE_LIST)))")


# Target options are 'sim' (default) and 'pynq-z2'
TARGET   			?= sim

# Linker options are 'on_chip' (default),'flash_load','flash_exec','freertos'
LINKER   			?= on_chip

# X-HEEP configuration
HEEP				?= $(ROOT_DIR)/hw/esl_epfl_x_heep
MCU_CFG				?= $(ROOT_DIR)/config/mcu-gen.hjson
PAD_CFG				?= $(ROOT_DIR)/config/heep-pads.hjson
APP_DIR				:= $(ROOT_DIR)/sw/applications
EXTERNAL_DOMAINS 	:= 1 
MCU_GEN_OPTS		:= \
	MCU_CFG=$(MCU_CFG) \
	PAD_CFG=$(PAD_CFG) \
	EXTERNAL_DOMAINS=$(EXTERNAL_DOMAINS)
MCU_GEN_LOCK		:= $(BUILD_DIR)/.mcu-gen.lock

# RTL simulation configuration
FIRMWARE 			?= $(ROOT_DIR)/sw/build/main.hex
BYPASS_FLL			?= 1 # 0: use FLL, 1: bypass FLL
FUSESOC_FLAGS		?=
FUSESOC_ARGS		?=

# QuestaSim
DPI_LIBS			:= $(BUILD_DIR)/sw/sim/uartdpi.so
QUESTA_SIM_POSTSYNTH_DIR 			= sim_postsynthesis-modelsim

# Power Analysis
PWR_VCD ?= ./build/vlsi_polito_mcu_horcrux_0/$(QUESTA_SIM_POSTSYNTH_DIR)/waves-0.vcd

# Application-specific makefile
APP_MAKE  			:= $(wildcard sw/applications/$(PROJECT)/*akefile)
APP_MAKE_PARAMS		?=

# Compiler options are 'gcc' (default) and 'clang'
COMPILER ?= gcc
# Compiler prefix options are 'riscv32-unknown-' (default)
COMPILER_PREFIX ?= riscv32-unknown-
# Arch options are any RISC-V ISA string supported by the CPU. Default 'rv32imc'
ARCH     ?= rv32imc
# Path relative from the location of sw/Makefile from which to fetch source files. The directory of that file is the default value.
SOURCE 	 ?= "."
# 1 external domain for the KECCAK
EXTERNAL_DOMAINS = 1
# Keccak application flags
USE_DMA   ?= 1


ifndef CONDA_DEFAULT_ENV
$(info USING VENV)
FUSESOC = $(PWD)/$(VENV)/fusesoc
PYTHON  = $(PWD)/$(VENV)/python
else
$(info USING MINICONDA $(CONDA_DEFAULT_ENV))
FUSESOC := $(shell which fusesoc)
PYTHON  := $(shell which python)
endif


.PHONY: vendor-update-heep
vendor-update-heep: clean-lock
	@echo "### Updating vendored IPs..."
	find hw/vendor -maxdepth 1 -type f -name "esl_*.vendor.hjson" -exec python3 util/vendor.py -vU '{}' \;

x_heep-sync:
	@echo "### Modifiying esl_epfl_x_heep repository..."
	rsync -a config/x_heep_system.sv hw/vendor/esl_epfl_x_heep/hw/system/x_heep_system.sv
	rsync -a config/cpu_subsystem.sv hw/vendor/esl_epfl_x_heep/hw/core-v-mini-mcu/cpu_subsystem.sv
	rsync -a config/core_v_mini_mcu.sv hw/vendor/esl_epfl_x_heep/hw/core-v-mini-mcu/core_v_mini_mcu.sv
	rsync -a config/cv32e40px_controller.sv hw/vendor/esl_epfl_x_heep/hw/vendor/esl_epfl_cv32e40px/rtl/cv32e40px_controller.sv
	rsync -a config/cv32e40px_id_stage.sv hw/vendor/esl_epfl_x_heep/hw/vendor/esl_epfl_cv32e40px/rtl/cv32e40px_id_stage.sv
	rsync -a config/cv32e40px_if_stage.sv hw/vendor/esl_epfl_x_heep/hw/vendor/esl_epfl_cv32e40px/rtl/cv32e40px_if_stage.sv
	rsync -a config/cv32e40px_x_disp.sv hw/vendor/esl_epfl_x_heep/hw/vendor/esl_epfl_cv32e40px/rtl/cv32e40px_x_disp.sv
	rsync -a config/cv32e40px_register_file_ff.sv hw/vendor/esl_epfl_x_heep/hw/vendor/esl_epfl_cv32e40px/rtl/cv32e40px_register_file_ff.sv


HOCRUX-sync:
	@echo "### Modifiying HOCRUX repository..."
	rsync -a hw/vlsi_polito_horcrux ../HORCRUX

mcu-gen:
	@sed -i 's/stack_size: 0x[0-9a-fA-F]*,/stack_size: 0x20000,/g' hw/vendor/esl_epfl_x_heep/mcu_cfg.hjson
	@sed -i 's/heap_size: 0x[0-9a-fA-F]*,/heap_size: 0x20000,/g' hw/vendor/esl_epfl_x_heep/mcu_cfg.hjson
	$(MAKE) -f $(XHEEP_MAKE) $(MAKECMDGOALS) CPU=cv32e40px BUS=NtoM MEMORY_BANKS=16 EXTERNAL_DOMAINS=$(EXTERNAL_DOMAINS)

# horcrux-gen:
# 	$(PYTHON) util/horcrux_gen.py --algorithms_list $(ALGORITHMS)


# Tests  ########################################################################################################################################

app-tests-$(TESTS):
	$(MAKE) -C sw applications/tests/$(TESTS)/main.hex TARGET=$(TARGET) LINKER=$(LINKER)
	riscv32-unknown-elf-objdump -d sw/applications/tests/$(TESTS)/main.elf > dis/test-$(TESTS).s
	riscv32-unknown-elf-objdump -S sw/applications/tests/$(TESTS)/main.elf > dis/test-$(TESTS).disasm
	@echo "### DONE! App app-optimized-test-$(TESTS) generated successfully!"


app-tests-$(VER)-$(TESTS):
	$(MAKE) -C sw applications/tests-$(VER)/$(TESTS)/main.hex TARGET=$(TARGET) LINKER=$(LINKER)
	riscv32-unknown-elf-objdump -d sw/applications/tests-$(VER)/$(TESTS)/main.elf > dis/test-$(VER)-$(TESTS)-.s
	riscv32-unknown-elf-objdump -S sw/applications/tests-$(VER)/$(TESTS)/main.elf > dis/test-$(VER)-$(TESTS).disasm
	@echo "### DONE! App app-test-$(VER)-$(TESTS) generated successfully!"


run-tests-$(TESTS):
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-modelsim; \
	make run PLUSARGS="c firmware=../../../sw/applications/tests/$(TESTS)/main.hex"; \
	cat uart0.log; \
	cd ../../..;

run-tests-$(TESTS)-gui:
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-modelsim; \
	make run-gui PLUSARGS="c firmware=../../../sw/applications/tests/$(TESTS)/main.hex"; \
	cat uart0.log; \
	cd ../../..;

# Applications  ###################################################################################################################################

app-$(V)-$(SCHEME)-$(ALG)-$(VERSION):
	$(MAKE) -C sw applications/PQC/$(V)/$(SCHEME)/$(ALG)/$(VERSION)/main.hex TARGET=$(TARGET) LINKER=$(LINKER)
	riscv32-unknown-elf-objdump -d  sw/applications/PQC/$(V)/$(SCHEME)/$(ALG)/$(VERSION)/main.elf > dis/test-$(V)-$(SCHEME)-$(ALG)-$(VERSION).s
	riscv32-unknown-elf-objdump -S  sw/applications/PQC/$(V)/$(SCHEME)/$(ALG)/$(VERSION)/main.elf > dis/test-$(V)-$(SCHEME)-$(ALG)-$(VERSION).disasm
	riscv32-unknown-elf-size sw/applications/PQC/$(V)/$(SCHEME)/$(ALG)/$(VERSION)/main.elf
	@echo "### DONE! App app-$(SCHEME)-$(ALG)-$(VERSION) generated successfully!"
	@echo "Code size (.text): $$(riscv32-unknown-elf-size sw/applications/PQC/$(V)/$(SCHEME)/$(ALG)/$(VERSION)/main.elf | sed -n '2p' | awk '{print $$1}') bytes"

run-$(V)-$(SCHEME)-$(ALG)-$(VERSION):
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-modelsim; \
	make run PLUSARGS="c firmware=../../../sw/applications/PQC/$(V)/$(SCHEME)/$(ALG)/$(VERSION)/main.hex"; \
	cat uart0.log; \
	cd ../../..;


# Applications  ###################################################################################################################################

app-helloworld:
	$(MAKE) -C sw x_heep_applications/hello_world/hello_world.hex TARGET=$(TARGET)

app-$(ACC)-$(SCHEME)-$(ALG):
	$(MAKE) -C sw applications/CRYPTO-VLSI-SW/$(ACC)/$(SCHEME)/$(ALG)/main.hex TARGET=$(TARGET) LINKER=$(LINKER)
	@echo "### DONE! App app-$(ACC)-$(SCHEME)-$(ALG) generated successfully for $(ACC)-version!"

# riscv32-unknown-elf-objdump -d sw/applications/CRYPTO-VLSI-SW/tests/$(TESTS)/main.elf > dis/test-$(TESTS).s
# riscv32-unknown-elf-objdump -S sw/applications/CRYPTO-VLSI-SW/tests/$(TESTS)/main.elf > dis/test-$(TESTS).disasm

# Simulation

questasim-sim:
	$(FUSESOC) --cores-root . run --no-export --target=sim --tool=modelsim $(FUSESOC_FLAGS) --setup --build vlsi:polito:mcu_horcrux 2>&1 | tee buildsim.log

verilator-sim: 
	fusesoc --cores-root . run --no-export --target=sim --tool=verilator $(FUSESOC_FLAGS) --setup --build vlsi:polito:mcu_horcrux 2>&1 | tee buildsim.log

run-helloworld-questasim: questasim-sim app-helloworld
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-modelsim; \
	make run PLUSARGS="c firmware=../../../sw/x_heep_applications/hello_world/hello_world.hex"; \
	cat uart0.log; \
	cd ../../..;

run-helloworld-questasim-gui: questasim-sim app-helloworld
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-modelsim; \
	make run-gui PLUSARGS="c firmware=../../../sw/x_heep_applications/hello_world/hello_world.hex"; \
	cat uart0.log; \
	cd ../../..;

run-helloworld-verilator: verilator-sim app-helloworld
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-verilator; \
	./Vtestharness +firmware=../../../sw/x_heep_applications/hello_world/hello_world.hex; \
	cat uart0.log; \
	cd ../../..;

run-$(ACC)-$(SCHEME)-$(ALG):
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-modelsim; \
	make run PLUSARGS="c firmware=../../../sw/applications/CRYPTO-VLSI-SW/$(ACC)/$(SCHEME)/$(ALG)/main.hex"; \
	cat uart0.log; \
	cd ../../..;

run-$(ACC)-$(SCHEME)-$(ALG)-gui:
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-modelsim; \
	make run-gui PLUSARGS="c firmware=../../../sw/applications/CRYPTO-VLSI-SW/$(ACC)/$(SCHEME)/$(ALG)/main.hex"; \
	cat uart0.log; \
	cd ../../..;

run-$(ACC)-$(SCHEME)-$(ALG)-$(VERSION)-gui:
	cd ./build/vlsi_polito_mcu_horcrux_0/sim-modelsim; \
	make run-gui PLUSARGS="c firmware=../../../sw/applications/CRYPTO-VLSI-SW/$(ACC)/$(SCHEME)/$(ALG)/$(VERSION)/main.hex"; \
	cat uart0.log; \
	cd ../../..;


## @subsection QuestaSim postsynthesis simulation

## Build simulation model
.PHONY: questasim-sim-postsynth
questasim-sim-postsynth: 
	$(FUSESOC) --cores-root . run --no-export --target=sim_postsynthesis --tool=modelsim $(FUSESOC_FLAGS) --setup --build vlsi:polito:mcu_horcrux 2>&1 | tee buildsim.log

run-tests-postsynth-$(TESTS):
	cd ./build/vlsi_polito_mcu_horcrux_0/$(QUESTA_SIM_POSTSYNTH_DIR); \
	make run PLUSARGS="c firmware=../../../sw/applications/tests/$(TESTS)/main.hex"; \
	cat uart0.log; \
	cd ../../..;

run-tests-postsynth-$(VER)-$(TESTS):
	cd ./build/vlsi_polito_mcu_horcrux_0/$(QUESTA_SIM_POSTSYNTH_DIR); \
	make run PLUSARGS="c firmware=../../../sw/applications/tests-$(VER)/$(TESTS)/main.hex"; \
	cat uart0.log; \
	cd ../../..;

run-tests-postsynth-$(VER)-$(TESTS)-gui:
	cd ./build/vlsi_polito_mcu_horcrux_0/$(QUESTA_SIM_POSTSYNTH_DIR); \
	make run-gui PLUSARGS="c firmware=../../../sw/applications/tests-$(VER)/$(TESTS)/main.hex"; \
	cat uart0.log; \
	cd ../../..;


## Perform power analysis
.PHONY: power-analysis
power-analysis:
	@echo "### Running power analysis..."
	rm -rf implementation/power_analysis/reports/*
	pushd implementation/power_analysis/; ./run_pwr_flow.sh $(PWR_VCD) horcrux_x_heep_top; popd;

# Debug
openOCD_epflp:
	$(MAKE) -f $(XHEEP_MAKE) openOCD_epflp

## @section Design Compiler
.PHONY: asic
asic:
	$(FUSESOC) --cores-root . run --no-export --target asic_synthesis $(FUSESOC_FLAGS) --setup --build vlsi:polito:mcu_horcrux 2>&1 | tee fusesoc_synthesis.log
	./scripts/check_log_synth.sh


# Clean rules
.PHONY: clean clean-build clean-app clean-lock
clean: clean-app clean-sim

clean-sim:
	@rm -rf build
clean-app:
	$(MAKE) -C sw clean
clean-build:
	$(RM) -rf $(BUILD_DIR)
clean-lock:
	$(RM) $(BUILD_DIR)/.*.lock


export HEEP_DIR = hw/vendor/esl_epfl_x_heep/
XHEEP_MAKE = $(HEEP_DIR)external.mk
#include $(XHEEP_MAKE)
