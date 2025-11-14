# PQC-Tests

This document organizes the build and run commands for the different cryptographic schemes, algorithms, and configurations in the repository.

#### keccak
```bash
make app-tests-keccak TESTS=keccak
make run-tests-keccak TESTS=keccak
```

#### cbd-eta
```bash
make app-tests-cbd_eta1 TESTS=cbd_eta1
make run-tests-cbd_eta1 TESTS=cbd_eta1

make app-tests-cbd_eta2 TESTS=cbd_eta2
make run-tests-cbd_eta2 TESTS=cbd_eta2

make app-tests-cbd_eta3 TESTS=cbd_eta3
make run-tests-cbd_eta3 TESTS=cbd_eta3

make app-tests-cbd_eta4 TESTS=cbd_eta4
make run-tests-cbd_eta4 TESTS=cbd_eta4
```

#### rej-uniform
```bash
make app-tests-rej12x2 TESTS=rej12x2
make run-tests-rej12x2 TESTS=rej12x2

make app-tests-rej23x1 TESTS=rej23x1
make run-tests-rej23x1 TESTS=rej23x1

make app-tests-rej8x4 TESTS=rej8x4
make run-tests-rej8x4 TESTS=rej8x4
```

#### gf-carryless
```bash
make app-tests-gf_carryless TESTS=gf_carryless
make run-tests-gf_carryless TESTS=gf_carryless
```

#### karats
```bash
make app-tests-karats TESTS=karats
make run-tests-karats TESTS=karats
```


#### montg
```bash
make app-tests-montg_kyber TESTS=montg_kyber
make run-tests-montg_kyber TESTS=montg_kyber

make app-tests-montg_falcon TESTS=montg_falcon
make run-tests-montg_falcon TESTS=montg_falcon

make app-tests-montg_newhope TESTS=montg_newhope
make run-tests-montg_newhope TESTS=montg_newhope

make app-tests-montg_ntrup TESTS=montg_ntrup
make run-tests-montg_ntrup TESTS=montg_ntrup

make app-tests-montg_dilithium TESTS=montg_dilithium
make run-tests-montg_dilithium TESTS=montg_dilithium
```

#### montg
```bash
make app-tests-barrett_kyber TESTS=barrett_kyber
make run-tests-barrett_kyber TESTS=barrett_kyber
```


#### compress
```bash
make app-tests-compress TESTS=compress
make run-tests-compress TESTS=compress
```






#### ntt_kyber
```bash
make app-tests-ntt_kyber TESTS=ntt_kyber
make run-tests-ntt_kyber TESTS=ntt_kyber
```
```bash
make app-tests-intt_kyber TESTS=intt_kyber
make run-tests-intt_kyber TESTS=intt_kyber
```

#### ntt_dilithium
```bash
make app-tests-ntt_dilithium TESTS=ntt_dilithium
make run-tests-ntt_dilithium TESTS=ntt_dilithium
```
```bash
make app-tests-intt_dilithium TESTS=intt_dilithium
make run-tests-intt_dilithium TESTS=intt_dilithium
```




```bash
make app-tests-vec_by_matrix TESTS=vec_by_matrix
make run-tests-vec_by_matrix TESTS=vec_by_matrix

make app-tests-restr_vec_by_matrix TESTS=restr_vec_by_matrix
make run-tests-restr_vec_by_matrix TESTS=restr_vec_by_matrix

make app-tests-double_zero_norm TESTS=double_zero_norm
make run-tests-double_zero_norm TESTS=double_zero_norm

make app-tests-vec_by_vec_pointwise TESTS=vec_by_vec_pointwise
make run-tests-vec_by_vec_pointwise TESTS=vec_by_vec_pointwise

make app-tests-vec_by_restr_vec_scaled TESTS=vec_by_restr_vec_scaled
make run-tests-vec_by_restr_vec_scaled TESTS=vec_by_restr_vec_scaled

make app-tests-synd_minus_vec_scaled TESTS=synd_minus_vec_scaled
make run-tests-synd_minus_vec_scaled TESTS=synd_minus_vec_scaled

make app-tests-convert_restr_vec_to_fp TESTS=convert_restr_vec_to_fp
make run-tests-convert_restr_vec_to_fp TESTS=convert_restr_vec_to_fp
```


```bash
make app-tests-G_restr_to_value TESTS=G_restr_to_value
make run-tests-G_restr_to_value TESTS=G_restr_to_value

make app-tests-G_vec_by_matrix TESTS=G_vec_by_matrix
make run-tests-G_vec_by_matrix TESTS=G_vec_by_matrix

make app-tests-G_restr_vec_by_matrix TESTS=G_restr_vec_by_matrix
make run-tests-G_restr_vec_by_matrix TESTS=G_restr_vec_by_matrix

make app-tests-G_vec_by_vec_pointwise TESTS=G_vec_by_vec_pointwise
make run-tests-G_vec_by_vec_pointwise TESTS=G_vec_by_vec_pointwise

make app-tests-G_synd_minus_vec_scaled TESTS=G_synd_minus_vec_scaled
make run-tests-G_synd_minus_vec_scaled TESTS=G_synd_minus_vec_scaled



make app-tests-G_vec_by_restr_vec_scaled TESTS=G_vec_by_restr_vec_scaled
make run-tests-G_vec_by_restr_vec_scaled TESTS=G_vec_by_restr_vec_scaled
```