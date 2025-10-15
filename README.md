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

| `barrett_d` | `barrett_k` | `caddq` | `cbd2` |
|:-------------:|:-------------:|:-------------:|:-------------:|
| `cbd3` | `compress` | `gf_carryless` | `intt_dilithium` |
| `intt_kyber` | `karats` | `keccak` | `montg_d` |
| `montg_k` | `ntt_dilithium` | `ntt_kyber` | `rej_uniform` |


### 🔐 Running PQC Algorithms

To run a complete PQC algorithm (KEM or DS):
```
make app-v1-KEM-ML-KEM-ml-kem-512 SCHEME=KEM ALG=ML-KEM VERSION=ml-kem-512 V=v1
```

This repository currently supports:

| **SCHEME** | **ALG**    | **VERSION**                                                                 |
|-------------|-------------|----------------------------------------------------------------------------|
| **KEM**     | ML-KEM      | `ml-kem-512`, `ml-kem-768`, `ml-kem-1024`                                 |
| **KEM**     | HQC         | `HQC-1`, `HQC-3`, `HQC-5`                                                 |
| **DS**      | ML-DSA      | `ML-DSA-2`, `ML-DSA-3`, `ML-DSA-5`                                        |
| **DS**      | SLH-DSA     | `SPHINCS+-128f-robust`, `SPHINCS+-128f-simple`, `SPHINCS+-192f-robust`,   |
|             |             | `SPHINCS+-192f-simple`, `SPHINCS+-256f-robust`, `SPHINCS+-256f-simple`    |

In particular:

- **ML-KEM**    ([Kyber](https://github.com/pq-crystals/kyber))
This repository contains the official reference implementation of the [Kyber](https://www.pq-crystals.org/kyber/) key encapsulation mechanism.
Kyber has been selected for standardization in [round 3](https://csrc.nist.gov/Projects/post-quantum-cryptography/round-3-submissions) 
of the [NIST PQC](https://csrc.nist.gov/projects/post-quantum-cryptography) standardization project.


- **ML-DSA**    ([Dilithium](https://github.com/pq-crystals/dilithium))
This repository contains the official reference implementation of the [Dilithium](https://www.pq-crystals.org/dilithium/) signature scheme.
Dilithium is standardized as [FIPS 204](https://csrc.nist.gov/pubs/fips/204/final).

- **HQC**       ([HQC](https://gitlab.com/pqc-hqc/hqc/))
This repository provides the official implementation of [HQC](https://pqc-hqc.org), a code-based Key Encapsulation Mechanism (KEM) whose security is based on the hardness of solving the Quasi-Cylic Syndrome Decoding (QCSD) problem. HQC is one of the selected algorithms from the [NIST's Post-Quantum Cryptography Standardization Project](https://csrc.nist.gov/projects/post-quantum-cryptography) (last version of 22/08/2025).

- **SLH-DSA**  (SPHINCS+)
This repository contains the software that accompanies the [SPHINCS+ submission](https://sphincs.org/) to [NIST's Post-Quantum Cryptography](https://csrc.nist.gov/Projects/Post-Quantum-Cryptography) project.

Each algorithm comes with a **self-contained KAT test**, adapted from the **official reference implementation submitted to NIST**.  

Each test provides:
- **Key generation** (`keygen`)  
- **Encapsulation / Signing** (`encaps` / `sign`)  
- **Decapsulation / Verification** (`decaps` / `verify`)  

Inside the application’s *main.c*, you can configure which phase of the protocol to execute by setting the following flags:
```
#define TEST_KEY  1
#define TEST_ENC  1
#define TEST_DEC  1
```
or
```
#define TEST_KEY        1
#define TEST_SIGN       1
#define TEST_SIGN_OPEN  1
```



Run simulation with QuestaSim
```
make questasim-sim
```



## 📊 Results


### 💾 Code Size Comparison
| **Algorithm** | **Code Size<br>(Orig.) [KB]** | **Code Size<br>(Opt.) [KB]** |
|-------------|-------------------------------|-------------------------------|
| ml-kem-512           |  27,564   |   24,004 [x1.15]   |
| ml-kem-768           |  27,400   |   23,572 [x1.16]   |
| ml-kem-1024          |  28,008   |   24,280 [x1.15]   |
| HQC-1                |  37,606   |   33,654 [x1.12]   |
| HQC-3                |  38,838   |   34,870 [x1.11]   |
| HQC-5                |  45,426   |   41,430 [x1.10]   |
| ML-DSA-2             |  32,704   |   28,472 [x1.15]   |
| ML-DSA-3             |  32,424   |   28,688 [x1.13]   |
| ML-DSA-5             |  32,072   |   28,416 [x1.13]   |
| SPHINCS+-128f-robust |  28,452   |   24,972 [x1.14]   |
| SPHINCS+-128f-simple |  27,624   |   24,140 [x1.14]   |
| SPHINCS+-192f-robust |  28,416   |   24,936 [x1.14]   |
| SPHINCS+-192f-simple |  27,596   |   24,116 [x1.14]   |
| SPHINCS+-256f-robust |  28,892   |   25,408 [x1.14]   |
| SPHINCS+-256f-simple |  28,000   |   24,516 [x1.14]   |

### ⚙️ PQC Benchmark 

| **Algorithm** | **KeyGen Cycles (Orig.)** | **KeyGen Cycles (Opt.)** | **Encaps/Sign Cycles (Orig.)** | **Encaps/Sign Cycles (Opt.)** | **Decaps/Verify Cycles (Orig.)** | **Decaps/Verify Cycles (Opt.)** |
|---------------|---------------------------|---------------------------|--------------------------------|--------------------------------|---------------------------------|--------------------------------|
| **ML-KEM-512** | 1,250 | 484 [×2.58] | 1,470 | 509 [×2.89] | 1,805 | 612 [×2.95] |
| **ML-KEM-768** | 2,068 | 801 [×2.58] | 2,402 | 849 [×2.83] | 2,842 | 992 [×2.86] |
| **ML-KEM-1024** | 3,274 | 1,260 [×2.58] | 3,657 | 1,337 [×2.74] | 4,208 | 1,518 [×2.77] |
| **HQC-1** | 57,647 | 2,447 [×23.6] | 115,082 | 4,689 [×24.5] | 174,718 | 8,042 [×21.7] |
| **HQC-3** | 174,267 | 6,947 [×25.1] | 348,111 | 13,381 [×26.0] | 524,831 | 21,246 [×24.7] |
| **HQC-5** | 439,928 | 13,412 [32.8] | 879,182 | 25,801  [x34.1] | 1,324,315 | 40,628 [×32.6] |
| **ML-DSA-2** | 3,713 | 1,770 [×2.09] | 7,140 | 3,579 [×1.99] | 3,853 | 1,863 [×2.07] |
| **ML-DSA-3** | 6,522 | 3,217 [×2.03] | 24,265 | 13,712 [×1.77] | 6,530 | 3,297 [×1.98] |
| **ML-DSA-5** | 11,058 | 5,191 [×2.13] | 28,353 | 14,156 [×2.00] | 11,245 | 5,328 [×2.11] |
| **SPHINCS+-128f-robust** | 286,087 | 145,947 [×1.96] | 2,345,442 | *XXXX* [×] | 407,115 | 207,616 [×1.96] |
| **SPHINCS+-128f-simple** | 148,920 | 76,565 [×1.94] | 3,486,490 | 1,793,416 [×1.94] | 204,805 | 105,412 [×1.94] |
| **SPHINCS+-192f-simple** | 221,042 | 114,140 [×1.94] | 1,425,544 | *XXXX* [×] | 303,500 | 156,758 [×1.95] |
| **SPHINCS+-256f-simple** | 585,456 | 306,815 [×1.90] | 3,195,353 | 1,893,273 [×1.69] | 321,355 | 167,431 [×1.92] |



## 📄 License

This repository follows the licensing terms of the respective reference implementations used as the starting point. Please check individual algorithm directories for specific license details.

## 👥 Authors

- Alessandra Dolmeta    - alessandra.dolmeta@polito.it
- Valeria Piscopo       - valeria.piscopo@polito.it