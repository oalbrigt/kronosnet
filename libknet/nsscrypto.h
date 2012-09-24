#ifndef NSSCRYPTO_H_DEFINED
#define NSSCRYPTO_H_DEFINED

#include <sys/types.h>
#include "libknet.h"

struct crypto_instance;

size_t crypto_sec_header_size(
	const char *crypto_cipher_type,
	const char *crypto_hash_type);

int crypto_authenticate_and_decrypt (
	struct crypto_instance *instance,
	unsigned char *buf,
	int *buf_len);

int crypto_encrypt_and_sign (
	struct crypto_instance *instance,
	const unsigned char *buf_in,
	const size_t buf_in_len,
	unsigned char *buf_out, 
	size_t *buf_out_len);

int crypto_init(
	knet_handle_t knet_h,
	const struct knet_handle_cfg *knet_handle_cfg);

void crypto_fini(
	knet_handle_t knet_h);

#endif /* NSSCRYPTO_H_DEFINED */
