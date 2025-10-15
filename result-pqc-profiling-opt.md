# Profiling results 
## OPTIMIZED
### SLH-DSA: SPHINCS+-128f-simple
(Keygen)
inith_hash cycles: 3
merkle_gen_root cycles: 76565601

(Sign)
init_hash cycles: 3
set_type cycles: 6
gen_message_random cycles: 12575
hash_message cycles: 13694
fors_sign cycles: 108817238
set_layer_addr cycles: 4
set_tree_addr cycles: 86
copy_subtree_addr cycles: 11
set_keypair_addr cycles: 21
merkle_sign cycles: 76571330

(Verify)
init_hash cycles: 3
hash_message cycles: 13697
set_tree_addr cycles: 85
set_keypair_addr cycles: 28
fors_pk_from_sig cycles: 4081927
set_layer_addr cycles: 4
set_tree_addr cycles: 85
copy_subtree_addr cycles: 11
set_keypair_addr cycles: 18
copy_keypair_addr cycles: 13
wots_pk_from_sig cycles: 4593541
thash cycles: 67880
compute_root_cycles cycles: 52408


### SLH-DSA: SPHINCS+-128f-robust
(Keygen)
inith_hash cycles: 3
merkle_gen_root cycles: 145861146

(Sign)
init_hash cycles: 3
set_type cycles: 6
gen_message_random cycles: 12548
hash_message cycles: 13662
fors_sign cycles: 177930011
set_layer_addr cycles: 4
set_tree_addr cycles: 79
copy_subtree_addr cycles: 11
set_keypair_addr cycles: 16
merkle_sign cycles: 145866940


(Verify)
init_hash cycles: 3
hash_message cycles: 13665
set_tree_addr cycles: 85
set_keypair_addr cycles: 25
fors_pk_from_sig cycles: 7949277
set_layer_addr cycles: 9
set_tree_addr cycles: 77
copy_subtree_addr cycles: 11
set_keypair_addr cycles: 13
copy_keypair_addr cycles: 13
wots_pk_from_sig cycles: 8995527
thash cycles: 130371
compute_root_cycles cycles: 102027


### ML-DSA-2
Keygen cycles: 1773554  
Sign cycles: 3584534  
Verify cycles: 1864930 

## HQC-1 - PROFILING_HQC
hash_i_cycles cycles: 14424  
xof_init_cycles cycles: 2207  
vect_sample_fixed_weight1_cycles cycles: 368903  
vect_set_random_cycles cycles: 197963  
vect_mul_cycles cycles: 1346026  
vect_add_cycles cycles: 3890  
Keygen cycles: 154909  
xof_init_cycles cycles: 2207  
hqc_ek_pke_from_string_cycles cycles: 223521  
vect_sample_fixed_weight2_cycles cycles: 433730  
vect_mul_cycles cycles: 1346022  
vect_add_cycles cycles: 3890  
code_encode_cycles cycles: 54392  
vect_truncate_cycles cycles: 11  
Encaps cycles: 210679  
hqc_dk_pke_from_string_cycles cycles: 373622  
vect_mul_cycles cycles: 1346024  
vect_truncate_cycles cycles: 11  
vect_add_cycles cycles: 3881  
code_decode_cycles cycles: 1054352  
xof_init_cycles cycles: 2207  
hqc_ek_pke_from_string_cycles cycles: 223521  
vect_sample_fixed_weight2_cycles cycles: 433730  
vect_mul_cycles cycles: 1346022  
vect_add_cycles cycles: 3890  
code_encode_cycles cycles: 54392  
vect_truncate_cycles cycles: 11  
Decaps cycles: 652772  

## ML-KEM-512 - PROFILING_INDCPA
hash_g_cycles cycles: 12191  
gen_a_cycles cycles: 171135  
getnoise_eta1_cycles cycles: 26326  
getnoise_eta1_cycles cycles: 26319  
polyvec_ntt_cycles cycles: 34700  
basemul_acc_cycles cycles: 11321  
poly_tomont_cycles cycles: 3586  
basemul_acc_cycles cycles: 11321  
poly_tomont_cycles cycles: 3586  
polyvec_add_cycles cycles: 4880  
polyvec_reduce_cycles cycles: 6921  
pack_sk_cycles cycles: 6426  
pack_pk_cycles cycles: 6443  
Keygen cycles: 189612  
unpack_pk_cycles cycles: 4397  
poly_from_msg_cycles cycles: 3017  
gen_at_cycles cycles: 171120  
poly_getnoise_eta1_cycles cycles: 26327  
poly_getnoise_eta1_cycles cycles: 26320  
poly_getnoise_eta2_cycles cycles: 15816  
poly_getnoise_eta2_cycles cycles: 15800  
 polyvec_ntt_cycles cycles: 34701  
polyvec_basemul_acc_cycles cycles: 11324  
polyvec_basemul_acc_cycles cycles: 11322  
polyvec_invntt_tomont_cycles cycles: 35231  
poly_invntt_tomont_cycles cycles: 17617  
polyvec_add_cycles cycles: 4878  
poly_add_cycles cycles: 2318  
polyvec_reduce_cycles cycles: 7182  
poly_reduce_cycles cycles: 3342  
pack_cp_cycles cycles: 5836  
Encaps cycles: 117764  
unpack_ciphertext_cycles cycles: 12209  
unpack_sk_cycles cycles: 4379  
polyvec_ntt_cycles cycles: 34702  
polyvec_basemul_acc_cycles cycles: 11327  
poly_invntt_tomont_cycles cycles: 17618  
poly_sub_cycles cycles: 2561  
poly_reduce_cycles cycles: 3342  
poly_tomsg_cycles cycles: 4050  
unpack_pk_cycles cycles: 4397  
poly_from_msg_cycles cycles: 3017  
gen_at_cycles cycles: 171120  
poly_getnoise_eta1_cycles cycles: 26327  
poly_getnoise_eta1_cycles cycles: 26320  
poly_getnoise_eta2_cycles cycles: 15816  
poly_getnoise_eta2_cycles cycles: 15800  
 polyvec_ntt_cycles cycles: 34701  
polyvec_basemul_acc_cycles cycles: 11324  
polyvec_basemul_acc_cycles cycles: 11322  
polyvec_invntt_tomont_cycles cycles: 35231  
poly_invntt_tomont_cycles cycles: 17617  
polyvec_add_cycles cycles: 4878  
poly_add_cycles cycles: 2318  
polyvec_reduce_cycles cycles: 7182  
poly_reduce_cycles cycles: 3342  
pack_cp_cycles cycles: 5836  
Decaps cycles: 201260  

## PROFILING_KEM
### HQC-1
prng_get_bytes_cycles cycles: 9867  
xof_init_cycles cycles: 2205  
xof_get_bytes_cycles cycles: 9866  
hqc_pke_keygen_cycles cycles: 2405365  
Keygen cycles: 2562025  
prng_get_bytes_cycles cycles: 590  
hash_h_cycles cycles: 219957  
hash_g_cycles cycles: 15321  
hqc_pke_encrypt_cycles cycles: 4406135  
hqc_c_kem_to_string_cycles cycles: 24219  
Encaps cycles: 187899  
hqc_c_kem_from_string_cycles cycles: 24089  
hqc_pke_decrypt_cycles cycles: 2847163  
hash_h_cycles cycles: 219957  
hash_g_cycles cycles: 15334  
hqc_pke_encrypt_cycles cycles: 4406144  
hash_j_cycles cycles: 425599  
vect_compare_cycles cycles: 19897  
Decaps cycles: 177147  
### ML-KEM-512
keypair_cycles cycles: 417365  
hash_h_cycles cycles: 70965  
Keygen cycles: 182951  
hash_h_cycles cycles: 70966  
hash_g_cycles cycles: 12878  
kem-enc_cycles cycles: 431365  
Encaps cycles: 551295  
dec_cycles cycles: 93489  
hash_g_cycles cycles: 12883  
enc cycles: 431364  
verify_cycles cycles: 7683  
rkprf_cycles cycles: 75587  
Decaps cycles: 184056  
