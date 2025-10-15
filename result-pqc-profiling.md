# Profiling results 
## ORIGINAL
### SLH-DSA: SPHINCS+-128f-simple
(Keygen)
inith_hash cycles: 3
merkle_gen_root cycles: 148919667

(Sign)
init_hash cycles: 3
set_type cycles: 6
gen_message_random cycles: 28520
hash_message cycles: 29624
fors_sign cycles: 210172414
set_layer_addr cycles: 4
set_tree_addr cycles: 86
copy_subtree_addr cycles: 11
set_keypair_addr cycles: 21
merkle_sign cycles: 148925396

(Verify)
init_hash cycles: 3
hash_message cycles: 29627
set_tree_addr cycles: 85
set_keypair_addr cycles: 28
fors_pk_from_sig cycles: 7854309
set_layer_addr cycles: 4
set_tree_addr cycles: 85
copy_subtree_addr cycles: 11
set_keypair_addr cycles: 18
copy_keypair_addr cycles: 13
wots_pk_from_sig cycles: 8908953
thash cycles: 148266
compute_root_cycles cycles: 100356

### SLH-DSA: SPHINCS+-128f-robust
(Keygen)
inith_hash cycles: 3
merkle_gen_root cycles: 286164360

(Sign)
init_hash cycles: 3
set_type cycles: 6
gen_message_random cycles: 28546
hash_message cycles: 29655
fors_sign cycles: 346284104
set_layer_addr cycles: 4
set_tree_addr cycles: 79
copy_subtree_addr cycles: 11
set_keypair_addr cycles: 16
merkle_sign cycles: 286170091

(Verify)
init_hash cycles: 3
hash_message cycles: 29658
set_tree_addr cycles: 85
set_keypair_addr cycles: 25
fors_pk_from_sig cycles: 15471570
set_layer_addr cycles: 9
set_tree_addr cycles: 77
copy_subtree_addr cycles: 11
set_keypair_addr cycles: 13
copy_keypair_addr cycles: 13
wots_pk_from_sig cycles: 16647498
thash cycles: 290799
compute_root_cycles cycles: 197839


### ML-DSA-2
shake_cycles cycles: 29546
polyvec_matrix_expand_cycles cycles: 2334148
polyvecl_uniform_eta_cycles cycles: 192464
polyveck_uniform_eta_cycles cycles: 219791
polyvecl_ntt_cycles cycles: 185791
polyvec_matrix_pointwise_montgomery_cycles cycles: 191771
polyveck_reduce_cycles cycles: 19489
polyveck_invntt_tomont_cycles cycles: 223551
polyveck_add_cycles cycles: 9243
polyveck_caddq_cycles cycles: 9248
polyveck_power2round_cycles cycles: 12318
pack_pk_cycles cycles: 6210
shake256_cycles cycles: 276981
pack_sk_cycles cycles: 20852
Keygen cycles: 136764
unpack_sk_cycles cycles: 24036
shake256_cycles cycles: 29998
polyvec_matrix_expand_cycles cycles: 2334146
polyvecl_ntt_cycles cycles: 185787
polyveck_ntt_cycles cycles: 185787
polyvecl_uniform_gamma1_cycles cycles: 575009
polyvec_matrix_pointwise_montgomery_cycles cycles: 191769
polyveck_reduce_cycles cycles: 19488
polyveck_invntt_tomont_cycles cycles: 223550
polyveck_caddq_cycles cycles: 9239
polyveck_decompose_cycles cycles: 24610
polyveck_pack_w1_cycles cycles: 8483
poly_challenge_cycles cycles: 36727
poly_ntt_cycles cycles: 46445
polyvecl_invntt_tomont_cycles cycles: 223556
polyveck_sub_cycles cycles: 9247
polyvecl_reduce_cycles cycles: 0
polyvecl_uniform_gamma1_cycles cycles: 575009
polyvec_matrix_pointwise_montgomery_cycles cycles: 191769
polyveck_reduce_cycles cycles: 19488
polyveck_invntt_tomont_cycles cycles: 223550
polyveck_caddq_cycles cycles: 9239
polyveck_decompose_cycles cycles: 24610
polyveck_pack_w1_cycles cycles: 8483
poly_challenge_cycles cycles: 36705
poly_ntt_cycles cycles: 46445
polyvecl_invntt_tomont_cycles cycles: 223556
polyveck_sub_cycles cycles: 9247
polyvecl_reduce_cycles cycles: 0
pack_sig_cycles cycles: 25176
Sign cycles: 144951
unpack_pk_cycles cycles: 7224
shake256_cycles cycles: 306967
poly_challenge_cycles cycles: 36711
polyvec_matrix_expand_cycles cycles: 2334153
polyvecl_ntt_cycles cycles: 185789
polyvec_matrix_pointwise_montgomery_cycles cycles: 191767
poly_ntt_cycles cycles: 46449
polyveck_shiftl_cycles cycles: 7188
polyveck_ntt_cycles cycles: 185788
polyveck_pointwise_poly_montgomery_cycles cycles: 38949
polyveck_sub_cycles cycles: 9253
polyveck_reduce_cycles cycles: 19488
polyveck_invntt_tomont_cycles cycles: 223550
polyveck_caddq_cycles cycles: 9243
make_hint_cycles cycles: 18979
polyveck_pack_w1_cycles cycles: 8484
Verify cycles: 353299

## HQC-1 - PROFILING_HQC
hash_i_cycles cycles: 30421  
xof_init_cycles cycles: 2222  
vect_sample_fixed_weight1_cycles cycles: 400835  
vect_set_random_cycles cycles: 469657  
vect_mul_cycles cycles: 56146314  
vect_add_cycles cycles: 3890  
Keygen cycles: 154909  
xof_init_cycles cycles: 2222  
hqc_ek_pke_from_string_cycles cycles: 495230  
vect_sample_fixed_weight2_cycles cycles: 481638  
vect_mul_cycles cycles: 56146310  
vect_add_cycles cycles: 3890  
code_encode_cycles cycles: 175405  
vect_truncate_cycles cycles: 11  
Encaps cycles: 210879  
hqc_dk_pke_from_string_cycles cycles: 405569  
vect_mul_cycles cycles: 56146312  
vect_truncate_cycles cycles: 11  
vect_add_cycles cycles: 3881  
code_decode_cycles cycles: 1974237  
xof_init_cycles cycles: 2222  
hqc_ek_pke_from_string_cycles cycles: 495230  
vect_sample_fixed_weight2_cycles cycles: 481638  
vect_mul_cycles cycles: 56146310  
vect_add_cycles cycles: 3890  
code_encode_cycles cycles: 175405  
vect_truncate_cycles cycles: 11  
Decaps cycles: 1185042  


## ML-KEM-512 - PROFILING_INDCPA
hash_g_cycles cycles: 28172  
gen_a_cycles cycles: 361788  
getnoise_eta1_cycles cycles: 60476  
getnoise_eta1_cycles cycles: 60470  
polyvec_ntt_cycles cycles: 120784  
basemul_acc_cycles cycles: 73949  
poly_tomont_cycles cycles: 11520  
basemul_acc_cycles cycles: 73949  
poly_tomont_cycles cycles: 11520  
polyvec_add_cycles cycles: 4623  
polyvec_reduce_cycles cycles: 20499  
pack_sk_cycles cycles: 6171  
pack_pk_cycles cycles: 6188  
Keygen cycles: 285590  
unpack_pk_cycles cycles: 4649  
poly_from_msg_cycles cycles: 3017  
gen_at_cycles cycles: 361775  
poly_getnoise_eta1_cycles cycles: 60478  
poly_getnoise_eta1_cycles cycles: 60471  
poly_getnoise_eta2_cycles cycles: 33593  
poly_getnoise_eta2_cycles cycles: 33588  
 polyvec_ntt_cycles cycles: 120784  
polyvec_basemul_acc_cycles cycles: 73954  
polyvec_basemul_acc_cycles cycles: 73949  
polyvec_invntt_tomont_cycles cycles: 187395  
poly_invntt_tomont_cycles cycles: 93700  
polyvec_add_cycles cycles: 4875  
poly_add_cycles cycles: 2573  
polyvec_reduce_cycles cycles: 20506  
poly_reduce_cycles cycles: 10256  
pack_cp_cycles cycles: 19974  
Encaps cycles: 135633  
unpack_ciphertext_cycles cycles: 11695  
unpack_sk_cycles cycles: 4630  
polyvec_ntt_cycles cycles: 120784  
polyvec_basemul_acc_cycles cycles: 73953  
poly_invntt_tomont_cycles cycles: 93699  
poly_sub_cycles cycles: 2306  
poly_reduce_cycles cycles: 10255  
poly_tomsg_cycles cycles: 3826  
unpack_pk_cycles cycles: 4649  
poly_from_msg_cycles cycles: 3017  
gen_at_cycles cycles: 361775  
poly_getnoise_eta1_cycles cycles: 60478  
poly_getnoise_eta1_cycles cycles: 60471  
poly_getnoise_eta2_cycles cycles: 33593  
poly_getnoise_eta2_cycles cycles: 33588  
 polyvec_ntt_cycles cycles: 120784  
polyvec_basemul_acc_cycles cycles: 73954  
polyvec_basemul_acc_cycles cycles: 73949  
polyvec_invntt_tomont_cycles cycles: 187395  
poly_invntt_tomont_cycles cycles: 93700  
polyvec_add_cycles cycles: 4875  
poly_add_cycles cycles: 2573  
polyvec_reduce_cycles cycles: 20506  
poly_reduce_cycles cycles: 10256  
pack_cp_cycles cycles: 19974  
Decaps cycles: 314379  

## PROFILING_KEM 

### HQC-1
prng_get_bytes_cycles cycles: 25849  
xof_init_cycles cycles: 2220  
xof_get_bytes_cycles cycles: 25848  
hqc_pke_keygen_cycles cycles: 57573236  
Keygen cycles: 57733774  
prng_get_bytes_cycles cycles: 590  
hash_h_cycles cycles: 491684  
hash_g_cycles cycles: 31330  
hqc_pke_encrypt_cycles cycles: 114511219  
hqc_c_kem_to_string_cycles cycles: 24219  
Encaps cycles: 187889  
hqc_c_kem_from_string_cycles cycles: 24089  
hqc_pke_decrypt_cycles cycles: 58599143  
hash_h_cycles cycles: 491684  
hash_g_cycles cycles: 31343  
hqc_pke_encrypt_cycles cycles: 114511228  
hash_j_cycles cycles: 953129  
vect_compare_cycles cycles: 22105  
Decaps cycles: 181327  

### ML-KEM-512
keypair_cycles cycles: 1084803  
hash_h_cycles cycles: 166382  
Keygen cycles: 282196  
hash_h_cycles cycles: 166383  
hash_g_cycles cycles: 28844  
kem-enc_cycles cycles: 1270285  
Encaps cycles: 1393985  
dec_cycles cycles: 319706  
hash_g_cycles cycles: 28848  
enc cycles: 1270285  
verify_cycles cycles: 6916  
rkprf_cycles cycles: 171480  
Decaps cycles: 283636  


## OPTIMIZED

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
