# PQC-Tests

This document organizes the build and run commands for the different cryptographic schemes, algorithms, and configurations in the repository.

#### 1.1. Keccak
```bash 
make app-tests-keccak TESTS=keccak  
make run-tests-keccak TESTS=keccak
```

#### 1.2. ntt_kyber
```bash
make app-tests-ntt_kyber TESTS=ntt_kyber
make run-tests-ntt_kyber TESTS=ntt_kyber
```

#### 1.3. ntt_dilithium
```bash
make app-tests-ntt_dilithium TESTS=ntt_dilithium
make run-tests-ntt_dilithium TESTS=ntt_dilithium
```

#### 1.4. intt_kyber
```bash
make app-tests-intt_kyber TESTS=intt_kyber
make run-tests-intt_kyber TESTS=intt_kyber
```

#### 1.5. intt_dilithium
```bash
make app-tests-intt_dilithium TESTS=intt_dilithium
make run-tests-intt_dilithium TESTS=intt_dilithium
```

#### 1.6. Barrett_k
```bash
make app-tests-barrett_k TESTS=barrett_k
make run-tests-barrett_k TESTS=barrett_k
```

#### 1.7. Barrett_d
```bash
make app-tests-barrett_d TESTS=barrett_d
make run-tests-barrett_d TESTS=barrett_d
```

#### 1.8. Montgomery_d
```bash
make app-tests-montg_d TESTS=montg_d
make run-tests-montg_d TESTS=montg_d
```

#### 1.9. Montgomery_k
```bash
make app-tests-montg_k TESTS=montg_k
make run-tests-montg_k TESTS=montg_k
```

#### 1.10. Karatsuba
```bash
make app-tests-karats TESTS=karats
make run-tests-karats TESTS=karats
```

#### 1.11. compress
```bash
make app-tests-compress TESTS=compress
make run-tests-compress TESTS=compress
```

#### 1.12. GF-carryless
```bash
make app-tests-gf_carryless TESTS=gf_carryless
make run-tests-gf_carryless TESTS=gf_carryless
```

#### 1.13. cbd
```bash
make app-tests-cbd2 TESTS=cbd2
make run-tests-cbd2 TESTS=cbd2
```

```bash
make app-tests-cbd3 TESTS=cbd3
make run-tests-cbd3 TESTS=cbd3
```

#### 1.14. rej-uniform
```bash
make app-tests-rej_uniform TESTS=rej_uniform
make run-tests-rej_uniform TESTS=rej_uniform
```

#### 1.15. caddq
```bash
make app-tests-caddq TESTS=caddq
make run-tests-caddq TESTS=caddq
```



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#### 1.16. Trailing-zero [X]
```bash
make app-tests-trailing_zero TESTS=trailing_zero
make run-tests-trailing_zero TESTS=trailing_zero
```

