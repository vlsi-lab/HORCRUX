# PQC-Tests

This document organizes the build and run commands for the different cryptographic schemes, algorithms, and configurations in the repository.


To do:
```bash
make questasim-sim-postsynth
make run-tests-postsynth-simple-keccak VER=simple TESTS=keccak
make power-analysis
```

#### keccak
```bash
make app-tests-simple-keccak VER=simple TESTS=keccak
make app-tests-custom-keccak VER=custom TESTS=keccak
make run-tests-postsynth-simple-keccak VER=simple TESTS=keccak
make run-tests-postsynth-custom-keccak VER=custom TESTS=keccak
```

#### cbd-eta
```bash
make app-tests-simple-cbd_eta1 VER=simple TESTS=cbd_eta1
make app-tests-custom-cbd_eta1 VER=custom TESTS=cbd_eta1
make run-tests-postsynth-simple-cbd_eta1 VER=simple TESTS=cbd_eta1
make run-tests-postsynth-custom-cbd_eta1 VER=custom TESTS=cbd_eta1

make app-tests-simple-cbd_eta2 VER=simple TESTS=cbd_eta2
make app-tests-custom-cbd_eta2 VER=custom TESTS=cbd_eta2
make run-tests-postsynth-simple-cbd_eta2 VER=simple TESTS=cbd_eta2
make run-tests-postsynth-custom-cbd_eta2 VER=custom TESTS=cbd_eta2

make app-tests-simple-cbd_eta3 VER=simple TESTS=cbd_eta3
make app-tests-custom-cbd_eta3 VER=custom TESTS=cbd_eta3
make run-tests-postsynth-simple-cbd_eta3 VER=simple TESTS=cbd_eta3
make run-tests-postsynth-custom-cbd_eta3 VER=custom TESTS=cbd_eta3

make app-tests-simple-cbd_eta4 VER=simple TESTS=cbd_eta4
make app-tests-custom-cbd_eta4 VER=custom TESTS=cbd_eta4
make run-tests-postsynth-simple-cbd_eta4 VER=simple TESTS=cbd_eta4
make run-tests-postsynth-custom-cbd_eta4 VER=custom TESTS=cbd_eta4
```

#### rej-uniform
```bash
make app-tests-simple-rej12x2 VER=simple TESTS=rej12x2
make app-tests-custom-rej12x2 VER=custom TESTS=rej12x2
make run-tests-postsynth-simple-rej12x2 VER=simple TESTS=rej12x2
make run-tests-postsynth-custom-rej12x2 VER=custom TESTS=rej12x2

make app-tests-simple-rej23x1 VER=simple TESTS=rej23x1
make app-tests-custom-rej23x1 VER=custom TESTS=rej23x1
make run-tests-postsynth-simple-rej23x1 VER=simple TESTS=rej23x1
make run-tests-postsynth-custom-rej23x1 VER=custom TESTS=rej23x1

make app-tests-simple-rej8x4 VER=simple TESTS=rej8x4
make app-tests-custom-rej8x4 VER=custom TESTS=rej8x4
make run-tests-postsynth-simple-rej8x4 VER=simple TESTS=rej8x4
make run-tests-postsynth-custom-rej8x4 VER=custom TESTS=rej8x4
```

#### gf-carryless
```bash
make app-tests-simple-gf_carryless VER=simple TESTS=gf_carryless
make app-tests-custom-gf_carryless VER=custom TESTS=gf_carryless
make run-tests-postsynth-simple-gf_carryless VER=simple TESTS=gf_carryless
make run-tests-postsynth-custom-gf_carryless VER=custom TESTS=gf_carryless
```

#### karats
```bash
make app-tests-simple-karats VER=simple TESTS=karats
make app-tests-custom-karats VER=custom TESTS=karats
make run-tests-postsynth-simple-karats VER=simple TESTS=karats
make run-tests-postsynth-custom-karats VER=custom TESTS=karats
```


#### montg
```bash
make app-tests-simple-montg_kyber VER=simple TESTS=montg_kyber
make app-tests-custom-montg_kyber VER=custom TESTS=montg_kyber
make run-tests-postsynth-simple-montg_kyber VER=simple TESTS=montg_kyber
make run-tests-postsynth-custom-montg_kyber VER=custom TESTS=montg_kyber

make app-tests-simple-montg_falcon VER=simple TESTS=montg_falcon
make app-tests-custom-montg_falcon VER=custom TESTS=montg_falcon
make run-tests-postsynth-simple-montg_falcon VER=simple TESTS=montg_falcon
make run-tests-postsynth-custom-montg_falcon VER=custom TESTS=montg_falcon

make app-tests-simple-montg_newhope VER=simple TESTS=montg_newhope
make app-tests-custom-montg_newhope VER=custom TESTS=montg_newhope
make run-tests-postsynth-simple-montg_newhope VER=simple TESTS=montg_newhope
make run-tests-postsynth-custom-montg_newhope VER=custom TESTS=montg_newhope

make app-tests-simple-montg_ntrup VER=simple TESTS=montg_ntrup
make app-tests-custom-montg_ntrup VER=custom TESTS=montg_ntrup
make run-tests-postsynth-simple-montg_ntrup VER=simple TESTS=montg_ntrup
make run-tests-postsynth-custom-montg_ntrup VER=custom TESTS=montg_ntrup

make app-tests-simple-montg_dilithium VER=simple TESTS=montg_dilithium
make app-tests-custom-montg_dilithium VER=custom TESTS=montg_dilithium
make run-tests-postsynth-simple-montg_dilithium VER=simple TESTS=montg_dilithium
make run-tests-postsynth-custom-montg_dilithium VER=custom TESTS=montg_dilithium
```

#### montg
```bash
make app-tests-simple-barrett_kyber VER=simple TESTS=barrett_kyber
make app-tests-custom-barrett_kyber VER=custom TESTS=barrett_kyber
make run-tests-postsynth-simple-barrett_kyber VER=simple TESTS=barrett_kyber
make run-tests-postsynth-custom-barrett_kyber VER=custom TESTS=barrett_kyber
```


#### compress
```bash
make app-tests-simple-compress1 VER=simple TESTS=compress1
make app-tests-custom-compress1 VER=custom TESTS=compress1
make run-tests-postsynth-simple-compress1 VER=simple TESTS=compress1
make run-tests-postsynth-custom-compress1 VER=custom TESTS=compress1

make app-tests-simple-compress2 VER=simple TESTS=compress2
make app-tests-custom-compress2 VER=custom TESTS=compress2
make run-tests-postsynth-simple-compress2 VER=simple TESTS=compress2
make run-tests-postsynth-custom-compress2 VER=custom TESTS=compress2

make app-tests-simple-compress3 VER=simple TESTS=compress3
make app-tests-custom-compress3 VER=custom TESTS=compress3
make run-tests-postsynth-simple-compress3 VER=simple TESTS=compress3
make run-tests-postsynth-custom-compress3 VER=custom TESTS=compress3

make app-tests-simple-compress4 VER=simple TESTS=compress4
make app-tests-custom-compress4 VER=custom TESTS=compress4
make run-tests-postsynth-simple-compress4 VER=simple TESTS=compress4
make run-tests-postsynth-custom-compress4 VER=custom TESTS=compress4
```


#### ntt_kyber
```bash
make app-tests-simple-ntt_kyber VER=simple TESTS=ntt_kyber
make app-tests-custom-ntt_kyber VER=custom TESTS=ntt_kyber
make run-tests-postsynth-simple-ntt_kyber VER=simple TESTS=ntt_kyber
make run-tests-postsynth-custom-ntt_kyber VER=custom TESTS=ntt_kyber
```

```bash
make app-tests-simple-intt_kyber VER=simple TESTS=intt_kyber
make app-tests-custom-intt_kyber VER=custom TESTS=intt_kyber
make run-tests-postsynth-simple-intt_kyber VER=simple TESTS=intt_kyber
make run-tests-postsynth-custom-intt_kyber VER=custom TESTS=intt_kyber
```

#### ntt_dilithium
```bash
make app-tests-simple-ntt_dilithium VER=simple TESTS=ntt_dilithium
make app-tests-custom-ntt_dilithium VER=custom TESTS=ntt_dilithium
make run-tests-postsynth-simple-ntt_dilithium VER=simple TESTS=ntt_dilithium
make run-tests-postsynth-custom-ntt_dilithium VER=custom TESTS=ntt_dilithium
```
```bash
make app-tests-simple-intt_dilithium VER=simple TESTS=intt_dilithium
make app-tests-custom-intt_dilithium VER=custom TESTS=intt_dilithium
make run-tests-postsynth-simple-intt_dilithium VER=simple TESTS=intt_dilithium
make run-tests-postsynth-custom-intt_dilithium VER=custom TESTS=intt_dilithium
```


#### CROSS_instructions
```bash
make app-tests-simple-vec_by_matrix VER=simple TESTS=vec_by_matrix
make app-tests-custom-vec_by_matrix VER=custom TESTS=vec_by_matrix
make run-tests-postsynth-simple-vec_by_matrix VER=simple TESTS=vec_by_matrix
make run-tests-postsynth-custom-vec_by_matrix VER=custom TESTS=vec_by_matrix

make app-tests-simple-restr_vec_by_matrix VER=simple TESTS=restr_vec_by_matrix
make app-tests-custom-restr_vec_by_matrix VER=custom TESTS=restr_vec_by_matrix
make run-tests-postsynth-simple-restr_vec_by_matrix VER=simple TESTS=restr_vec_by_matrix
make run-tests-postsynth-custom-restr_vec_by_matrix VER=custom TESTS=restr_vec_by_matrix

make app-tests-simple-double_zero_norm VER=simple TESTS=double_zero_norm
make app-tests-custom-double_zero_norm VER=custom TESTS=double_zero_norm
make run-tests-postsynth-simple-double_zero_norm VER=simple TESTS=double_zero_norm
make run-tests-postsynth-custom-double_zero_norm VER=custom TESTS=double_zero_norm

make app-tests-simple-vec_by_vec_pointwise VER=simple TESTS=vec_by_vec_pointwise
make app-tests-custom-vec_by_vec_pointwise VER=custom TESTS=vec_by_vec_pointwise
make run-tests-postsynth-simple-vec_by_vec_pointwise VER=simple TESTS=vec_by_vec_pointwise
make run-tests-postsynth-custom-vec_by_vec_pointwise VER=custom TESTS=vec_by_vec_pointwise

make app-tests-simple-vec_by_restr_vec_scaled VER=simple TESTS=vec_by_restr_vec_scaled
make app-tests-custom-vec_by_restr_vec_scaled VER=custom TESTS=vec_by_restr_vec_scaled
make run-tests-postsynth-simple-vec_by_restr_vec_scaled VER=simple TESTS=vec_by_restr_vec_scaled
make run-tests-postsynth-custom-vec_by_restr_vec_scaled VER=custom TESTS=vec_by_restr_vec_scaled

make app-tests-simple-synd_minus_vec_scaled VER=simple TESTS=synd_minus_vec_scaled
make app-tests-custom-synd_minus_vec_scaled VER=custom TESTS=synd_minus_vec_scaled
make run-tests-postsynth-simple-synd_minus_vec_scaled VER=simple TESTS=synd_minus_vec_scaled
make run-tests-postsynth-custom-synd_minus_vec_scaled VER=custom TESTS=synd_minus_vec_scaled

make app-tests-simple-convert_restr_vec_to_fp VER=simple TESTS=convert_restr_vec_to_fp
make app-tests-custom-convert_restr_vec_to_fp VER=custom TESTS=convert_restr_vec_to_fp
make run-tests-postsynth-simple-convert_restr_vec_to_fp VER=simple TESTS=convert_restr_vec_to_fp
make run-tests-postsynth-custom-convert_restr_vec_to_fp VER=custom TESTS=convert_restr_vec_to_fp

make app-tests-simple-G_restr_to_value VER=simple TESTS=G_restr_to_value
make app-tests-custom-G_restr_to_value VER=custom TESTS=G_restr_to_value
make run-tests-postsynth-simple-G_restr_to_value VER=simple TESTS=G_restr_to_value
make run-tests-postsynth-custom-G_restr_to_value VER=custom TESTS=G_restr_to_value

make app-tests-simple-G_vec_by_matrix VER=simple TESTS=G_vec_by_matrix
make app-tests-custom-G_vec_by_matrix VER=custom TESTS=G_vec_by_matrix
make run-tests-postsynth-simple-G_vec_by_matrix VER=simple TESTS=G_vec_by_matrix
make run-tests-postsynth-custom-G_vec_by_matrix VER=custom TESTS=G_vec_by_matrix

make app-tests-simple-G_restr_vec_by_matrix VER=simple TESTS=G_restr_vec_by_matrix
make app-tests-custom-G_restr_vec_by_matrix VER=custom TESTS=G_restr_vec_by_matrix
make run-tests-postsynth-simple-G_restr_vec_by_matrix VER=simple TESTS=G_restr_vec_by_matrix
make run-tests-postsynth-custom-G_restr_vec_by_matrix VER=custom TESTS=G_restr_vec_by_matrix

make app-tests-simple-G_vec_by_vec_pointwise VER=simple TESTS=G_vec_by_vec_pointwise
make app-tests-custom-G_vec_by_vec_pointwise VER=custom TESTS=G_vec_by_vec_pointwise
make run-tests-postsynth-simple-G_vec_by_vec_pointwise VER=simple TESTS=G_vec_by_vec_pointwise
make run-tests-postsynth-custom-G_vec_by_vec_pointwise VER=custom TESTS=G_vec_by_vec_pointwise

make app-tests-simple-G_synd_minus_vec_scaled VER=simple TESTS=G_synd_minus_vec_scaled
make app-tests-custom-G_synd_minus_vec_scaled VER=custom TESTS=G_synd_minus_vec_scaled
make run-tests-postsynth-simple-G_synd_minus_vec_scaled VER=simple TESTS=G_synd_minus_vec_scaled
make run-tests-postsynth-custom-G_synd_minus_vec_scaled VER=custom TESTS=G_synd_minus_vec_scaled

make app-tests-simple-G_vec_by_restr_vec_scaled VER=simple TESTS=G_vec_by_restr_vec_scaled
make app-tests-custom-G_vec_by_restr_vec_scaled VER=custom TESTS=G_vec_by_restr_vec_scaled
make run-tests-postsynth-simple-G_vec_by_restr_vec_scaled VER=simple TESTS=G_vec_by_restr_vec_scaled
make run-tests-postsynth-custom-G_vec_by_restr_vec_scaled VER=custom TESTS=G_vec_by_restr_vec_scaled
```