# HORCRUX 

HORCRUX is a modular hardware–software framework for accelerating Post-Quantum Cryptography (PQC) primitives and algorithms on RISC-V.

It provides a unified environment to design, simulate, and test cryptographic accelerators and their integration within the [X-HEEP](https://github.com/esl-epfl/x-heep) SoC platform .


## 📁 Repository Structure
```bash
HORCRUX/
├── ci/scripts/     # Continuous integration and automation scripts
├── config/         # Configuration files for builds and SoC setup
├── hw/             # Hardware modules (RTL, accelerators, and interfaces)
├── scripts/        # Build and utility scripts
├── sw/             # Software tests, applications, and PQC implementations
├── tb/             # Testbenches for hardware simulation
└── util/           # Utility files and helper functions
```

## 🚀 Getting Started

Clone the repository
```
git clone <your_repo_url>
cd horcrux
```

Generate MCU sources and modify some of the X-HEEP files:
```
make mcu-gen
make x_heep-sync
```

**Next steps**

- See **[Software Structure (sw/)](#-software-structure-sw)** to learn which apps you can build and how.

- See **[Simulation Set-Up](#-simulation-set-up)** to build and run the simulation.

- See **[ASIC](#asic)** to learn more about the ASIC flow.



## 💾 Software Structure (sw/)
The sw/ directory contains all software components, including:

- Instruction-level tests for verifying custom operations and arithmetic units. See [how to run them](#-running-single-instruction-tests).

- PQC algorithm tests for full key generation, encapsulation/decapsulation, and signature operations. See [how to run them](#-running-pqc-algorithms).

### 🧪 Running Single Instruction Tests

You can run an individual instruction test with:
```
make app-tests-ntt_dilithium TESTS=ntt_dilithium
```
Available values for TESTS include:

| `barrett_kyber` | `cbd_eta1` | `cbd_eta2` | `cbd_eta3` |
| `cbd_eta4` | `compress` | `convert_restr_vec_to_fp` | `double_zero_norm` |  
| `G_restr_to_value` | `G_restr_vec_by_matrix` | `G_synd_minus_vec_scaled` | `G_vec_by_matrix` | `G_vec_by_restr_vec_scaled` |  
| `G_vec_by_vec_pointwise` | `gf_carryless` | `intt_dilithium` | `intt_kyber` | `karats` | `keccak` | `montg_dilithium` |    |`montg_falcon`|`montg_kyber`|`montg_newhope`|`montg_ntrup`|`ntt_dilithium`|`ntt_kyber`|`rej_uniform`|`rej8x4`|`rej12x2`|  
|`rej23x2`|`restr_vec_by_matrix`|`synd_minus_vec_scaled`|`vec_by_matrix`|`vec_by_restr_vec_scaled`|`vec_by_vec_pointwise`|

Besides the `tests` folder, two other folders, `tests-simple` and `tests-custom`, are used to distinguish the software baselines and the custom instructions. This division helps for the post-synthesis power analyses.


## 📟 Simulation Set Up
Set up Questasim environment:
```
make questasim-sim
```
And then run the application of your choice:
```
make run-v1-KEM-ML-KEM-ml-kem-512 SCHEME=KEM ALG=ML-KEM VERSION=ml-kem-512 V=v1
```

Also in this case you can use the helper script [`scripts/compile_apps.sh`](./compile_apps.sh).
```
# Run an already compiled ML-KEM-512 app
./compile_apps.sh -V v2 -S KEM -A ML-KEM -R ml-kem-512 --no-app

# Batch mode: compile and run all v2 apps, continuing even on errors
./compile_apps.sh -V v2 --all --keep-going
```

## 🔧 ASIC
All the commands for the ASIC flow have been left in this repo for completion, even though all the sensitive information and scripts have been removed.

To perform the ASIC synthesis:
```
make asic
```
All the post-synthesis functional simulations and power analyses are supported:
```
make questasim-sim-postsynth
run-tests-postsynth-$(TESTS) TESTS=montg_dilithium
```
To obtain the post-synthesis power results for a specific test, for both the software baseline and the custom instruction:
```
make questasim-sim-postsynth
make app-tests-simple-keccak VER=simple TESTS=keccak
make app-tests-custom-keccak VER=custom TESTS=keccak
make run-tests-postsynth-simple-keccak VER=simple TESTS=keccak
make run-tests-postsynth-custom-keccak VER=custom TESTS=keccak
```
The helper script [`scripts/get_power_res_postsynth.sh`](./get_power_res_postsynth.sh) can be used to automatically compile and run the power analyses on the tests. The user can choose whether to run all tests, just the simple, custom or specific cases. Some examples:
```
# Run all simple tests
source ./scripts/get_power_res_postsynth.sh --only-ver simple

# Run both simple and custom selected tests
source ./scripts/get_power_res_postsynth.sh --only-tests keccak,karats,cbd_eta1

# Run some selected, custom tests
source ./scripts/get_power_res_postsynth.sh --only-ver custom --only-tests rej12x2,gf_carryless,compress1,compress2

# Run both simple and custom tests, for all tests
source ./scripts/get_power_res_postsynth.sh 
```

## 📄 License

This repository follows the licensing terms of the respective reference implementations used as the starting point. Please check individual algorithm directories for specific license details.


## 👥 Authors

- Alessandra Dolmeta    - alessandra.dolmeta@polito.it
- Valeria Piscopo       - valeria.piscopo@polito.it
