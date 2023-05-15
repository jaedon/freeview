/***************************************************************************
 *     Copyright (c) 2006-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_crypto.h $
 * $brcm_Revision: 13 $
 * $brcm_Date: 8/27/07 5:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_crypto.h $
 * 
 * 13   8/27/07 5:11p vsilyaev
 * PR 34353: Data structure change to allow the keyladder to be sourced
 * correctly
 * 
 * 12   8/22/07 5:26p jgarrett
 * PR 31818: Adding external encryption algorithm support
 * 
 * 11   7/19/07 12:23a vsilyaev
 * PR 32275: Added 5-th level keys
 * 
 * 10   6/28/07 2:19p vsilyaev
 * PR 32275: Added hmac (keyed-hash message authentication code) operation
 * 
 * 9   3/14/07 3:27p vsilyaev
 * PR 26200: Changes in bcrypto configuration structures
 * 
 * 8   11/28/06 1:23p mphillip
 * PR26154: Enable hardware RSA for 7401
 * 
 * 7   11/6/06 6:00p mphillip
 * PR23943: Add counter mode
 * 
 * 6   11/1/06 6:27p mphillip
 * PR23943: First pass at adding CMAC support.  The current approach may
 * change in the future if it doesn't work out correctly.
 * 
 * 5   9/27/06 4:19p mphillip
 * PR24607: Add extern statement
 * 
 * 4   8/11/06 5:57p mphillip
 * PR23573: Rename bcrypt_ to bcrypto_
 * 
 * 3   8/3/06 11:09a mphillip
 * PR19544: Correct the paramater for bcrypt_open
 * 
 * 2   8/2/06 1:58p mphillip
 * PR19544: Update bsettop_crypt.h to handle multiple calls to hashes, and
 * fix a typo in bsettop_crypt.c
 * 
 * 1   7/11/06 5:47p mphillip
 * PR19544: Initial commit of bcrypt_ stubs
 * 
 *
 *
 ***************************************************************************/

#ifndef BSETTOP_CRYPTO_H__
#define BSETTOP_CRYPTO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*=*************************
The crypto module provides access to cryptographic functionality.

This is not the interface for controlling playback or record encryption or live conditional access. 
PVR encryption is handled through the bstream_mpeg.encryption structure passed in to
bplayback_start.  Live conditional access (or playback of network-encrypted streams) is controlled 
through bstream_set_encryption.
****************************/

/*
Summary:
Cryptographic hardware selection
Description:
This controls how cryptography is performed.  When bcrypto_hardware_default is used, the bcrypto layer will choose 
whether to use a hardware or software implementation.  Other values will dictate a specific implementation.
*/
typedef enum bcrypto_hardware_ctrl {
	bcrypto_hardware_default,	/* Use either a software or hardware implementation, bcrypto will choose */
	bcrypto_hardware_none,		/* Use only a software implementation */
	bcrypto_hardware_m2m,		/* Use only a mem-to-mem (M2M) implementation */
	bcrypto_hardware_keyladder	/* Use the keyladder approach */
} bcrypto_hardware_ctrl;

/*
Summary:
Cryptographic operation selection
Description:
This controls what cryptographic operation should be performed.  Each algorithm supports only certain operations, 
so the bcrypto documentation should be consulted for specifics.
*/
typedef enum bcrypto_operation {
	bcrypto_operation_encrypt,
	bcrypto_operation_decrypt,
	bcrypto_operation_sign,
	bcrypto_operation_verify,
	bcrypto_operation_hash,
	bcrypto_operation_mac,
	bcrypto_operation_hmac
} bcrypto_operation;

/*
Summary:
Description:
*/
typedef enum bcrypto_data_format {
	bcrypto_data_format_raw, 	/* Process all data */
	bcrypto_data_format_ts,		/* Data will be processed in 188 byte packets, skipping the transport headers. */
	bcrypto_data_format_dss		/* Data will be processed in 130 byte packets, skipping the transport headers. */
} bcrypto_data_format;

/*
Summary:
RSA 1024-bit key structure
Description:
A pointer to this structure should be used for long_key (see bencryption_params) to pass an RSA key.  In this case, bencryption_params.key_length will refer to the size of the struct (in bits, so sizeof(bcrypto_rsa_1024)*8), not the length of the key.  That will be in the first member variable of this struct.

Not all values need to be filled in for all modes of operation.  For signing, n, and d are required.  For verifying, n and e are required.

For software modes, dmp1, dmq1, and iqmp are also required.

For 7401 hardware, p, q, dmp1, dmq1, and iqmp are not necessary.
*/
typedef struct bcrypto_rsa_1024 {
	unsigned key_length;	/* Should be 1024 */
	uint8_t n[128];		/* modulus */
	uint8_t e[128];		/* public exponent */
	uint8_t d[128];		/* private exponent */
	uint8_t p[128];		/* first prime number */
	uint8_t q[128];		/* second prime number */
	uint8_t dmp1[128];	/* exponent corresponding to the first prime */
	uint8_t dmq1[128];	/* exponent corresponding to the second prime */
	uint8_t iqmp[128];	/* inverse of q mod p */
} bcrypto_rsa_1024;

/*
Summary:
RSA 2048-bit key structure
Description:
A pointer to this structure should be used for long_key (see bencryption_params) to pass an RSA key.  In this case, bencryption_params.key_length will refer to the size of the struct (in bits, so sizeof(bcrypto_rsa_2048)*8), not the length of the key.  That will be in the first member variable of this struct.

Not all values need to be filled in for all modes of operation.  For signing, n, and d are required.  For verifying, n and e are required.

For software modes, dmp1, dmq1, and iqmp are also required.

For 7401 hardware, p, q, dmp1, dmq1, and iqmp are not necessary.
*/
typedef struct bcrypto_rsa_2048 {
	unsigned key_length;	/* Should be 2048 */
	uint8_t n[256];		/* modulus */
	uint8_t e[256];		/* public exponent */
	uint8_t d[256];		/* private exponent */
	uint8_t p[256];		/* first prime number */
	uint8_t q[256];		/* second prime number */
	uint8_t dmp1[256];	/* exponent corresponding to the first prime */
	uint8_t dmq1[256];	/* exponent corresponding to the second prime */
	uint8_t iqmp[256];	/* inverse of q mod p */
} bcrypto_rsa_2048;

/*
Summary:
Keyladder input structure
Description:
This is part of bcrypto_keyladder_data.
*/
typedef enum bcrypto_swizzle {
	bcrypto_swizzle_none,
	bcrypto_swizzle_0,
	bcrypto_swizzle_1
} bcrypto_swizzle;
 
/*
Summary:
Keyladder input structure
Description:
This is part of bcrypto_keyladder_data.
*/
typedef enum bcrypto_keyladder_key {
	bcrypto_keyladder_key_0,
	bcrypto_keyladder_key_1,
	bcrypto_keyladder_key_2,
	bcrypto_keyladder_key_3,
	bcrypto_keyladder_key_4,
	bcrypto_keyladder_key_5
} bcrypto_keyladder_key;
 
/*
Summary:
Keyladder input structure
Description:
This is part of bcrypto_keyladder_data.
*/
typedef enum bcrypto_keyladder_rootkey_src {
	bcrypto_rootkey_cust_key,
	bcrypto_rootkey_otp_a,
	bcrypto_rootkey_otp_b,
	bcrypto_rootkey_otp_c,
	bcrypto_rootkey_reserved_0,
	bcrypto_rootkey_reserved_1,
	bcrypto_rootkey_reserved_2,
	bcrypto_rootkey_invalid
} bcrypto_keyladder_rootkey_src;

/*
Summary:
Keyladder input structure
Description:
This is passed in as long_key when using keyladder.  Remember to set key_length to the size of this struct in bits (key_length = 8*sizeof(struct bcrypto_keyladder_data)).
*/
typedef struct bcrypto_keyladder_data {
	bcrypto_swizzle swizzle;
	bcrypto_keyladder_key keyladder_key;
	uint8_t odd_in_3[24];
	uint8_t even_in_3[24];
	uint8_t odd_in_4[24];
	uint8_t even_in_4[24];
	uint8_t odd_in_5[24];
	uint8_t even_in_5[24];
	uint8_t key_cust_low;
	uint8_t key_cust_high;
	uint8_t key_var_low;
	uint8_t key_var_high;
	bcrypto_keyladder_rootkey_src rootkey_src;
	bool use_cust_key_decrypt;
	bool gen_key_decrypt; /* matches generateRouteKeyIO.bls3DESDecrypt, false unless instructed otherwise */
} bcrypto_keyladder_data;

/*
Summary:
External bcrypto algorithm input structure
Description:
This is passed in as long_key when using an external encryption algorithm.  Remember to set key_length to the size of this struct in bits (key_length = 8*sizeof(struct bcrypto_external_crypto_data)).

When using an external algorithm with encrypted PVR, this is used to control the parameters.  All other encryption parameters are ignored, so it is up to the external algorithm to manage keys, state, and other data.
*/
typedef void (*bcrypto_external_callback)(void *context, void *buffer, long size);
typedef struct bcrypto_external_crypto_data {
	void *callback_context;
	bcrypto_external_callback callback;
} bcrypto_external_crypto_data;

/*
Summary:
BCrypt settings structure
Description:
This structure controls the bcrypto settings.  It carries bencryption_params forward to control keys and algorithm choices.
*/
typedef struct bcrypto_settings {
	bencryption_params	encryption;
	bcrypto_operation	operation;	/* encrypt, decrypt, sign, verify, hash */
						/* (formerly "bcypher_mode", changed to avoid confusion) */
	bcrypto_data_format	data_format;	/* all, ts */
	bcrypto_hardware_ctrl	hardware_ctrl;	/* any, software only, m2m only, keyladder only, etc. */
	bool				multiple_hashes;	/* allows hashing functions to occur one block at a time; requires an extra closing process call with in_buffer == NULL to close out the hash */
} bcrypto_settings;

/*
Summary:
BCrypt status structure
Description:
This returns information about the algorithm chosen.  It can be used to dynamically allocate output blocks, but it is 
strongly recommended that the bcrypto user know the algorithms already.

If matching_output_size is set, then the output buffer for bcrypto_process needs to be the same size as the input buffer.

If fixed_output_size is set, then the output buffer needs to be of a specific, fixed length, regardless of the size of the 
input buffer.
*/
typedef struct bcrypto_status_t {
	bool	matching_output_size;	/* true for block ciphers, where the output block length should match the input block length */
	bool	fixed_output_size;		/* true for hash, sign, and verify operations */
	int		fixed_output_length;	/* if fixed_output_size is true, this holds the expected output size */
} bcrypto_status_t;

/*
Summary:
BCrypt structure
Description:
This is an opaque structure containing internal state for bcrypto operations.
*/
typedef struct bcrypto *bcrypto_t;

/*
Summary:
BCrypt settings initialization
Description:
This function initializes settings to safe, default values.  The bcrypto user will still need to 
populate the structure with algorithm choices, keys, and any other necessary settings after this call.
*/
void bcrypto_settings_init(
	bcrypto_settings	*settings
);

/*
Summary:
BCrypt open call
Description:
This creates a bcrypto object and populates it from the passed in bcrypto_settings.
*/
bcrypto_t bcrypto_open (
	const bcrypto_settings	*settings
);

/*
Summary:
BCrypt status call
Description:
This retrieves status information about a current bcrypto session.  Its primary use is to find out 
algorithmic requirements.
*/
bresult bcrypto_status (
	bcrypto_t	crypt,
	bcrypto_status_t	*status
);

/*
Summary:
BCrypt process call
Description:
This processes a chunk of data and, where appropriate, returns the result.
*/
bresult bcrypto_process (
	bcrypto_t	crypt,
	uint8_t		*in_buffer,
	unsigned long	in_buffer_length,
	uint8_t		*out_buffer,
	unsigned long	out_buffer_length
);

/*
Summary:
BCrypt close call
Description:
This closes out a bcrypto session.
*/
void bcrypto_close (
	bcrypto_t	crypt
);

/*
Summary:
BCrypt m2m/dma memory allocation
Description:
This allocates a buffer suitable for use with m2m hardware encryption.

This function is probably not suitable for here, and the m2m/dma support should probably be handled differently.
*/
uint8_t *bcrypto_alloc(
	unsigned	buffer_length
);

/*
Summary:
BCrypt m2m/dma memory de-allocation
Description:
This frees a buffer suitable for use with m2m hardware encryption.

This function is probably not suitable for here, and the m2m/dma support should probably be handled differently.
*/
void bcrypto_free(
	uint8_t *buffer
);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_CRYPTO_H__ */
