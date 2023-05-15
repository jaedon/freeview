/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_cipher.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/1/07 6:29p $
 *
 * Module Description: ioctl interface for driver
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_cipher.h $
 * 
 * 2   2/1/07 6:29p vsilyaev
 * PR 27399: Fixed typo and updated comments
 * 
 * 1   2/7/05 7:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/8   6/23/04 3:21p erickson
 * PR11135: fixed comment
 * 
 * Irvine_BSEAVSW_Devel/7   11/18/03 1:54p erickson
 * PR8356: got venom2 working and added scrambling_control support
 * 
 * Irvine_BSEAVSW_Devel/6   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 * 
 * Irvine_BSEAVSW_Devel/5   11/4/03 2:42p erickson
 * documentation changes
 * 
 * Irvine_BSEAVSW_Devel/4   10/31/03 11:58a erickson
 * settop api changes after internal review
 * 
 * Irvine_BSEAVSW_Devel/3   10/28/03 11:28a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/2   9/30/03 12:19a erickson
 * added standard header stuff like cplusplus
 *
 * Irvine_BSEAVSW_Devel/1   9/25/03 1:28p erickson
 * added cpu-based encryption to settop api and util
 *
 *************************************************************************/
#ifndef BSETTOP_CIPHER_H__
#define BSETTOP_CIPHER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=******************************
The cipher module provides access to the PVR DES/3DES encryption
and decryption engine. You can decrypt or encrypt MPEG2 transport data
off-line, or you can also encrypt or decrypt any raw data for your own purposes.

This is not the interface for controlling playback or record encryption or live conditional access. 
PVR encryption is handled through the bstream_mpeg.encryption structure passed in to
bplayback_start.

Live conditional access for Magnum platforms is supported via bstream_set_encryption.
Live conditional access for legacy platforms is not yet supported.

The cipher module is not intended for use with Magnum platforms.  General cryptographic functionality 
for Magnum boards is provided via the bcrypto interface.
*********************************/

/*
Summary:
	Handle for cipher engine.
*/
typedef struct bcipher *bcipher_t;

/*
Summary:
	Open a cipher handle.
*/
bcipher_t bcipher_open(
	bobject_t id
	);

/*
Summary:
	Close a cipher handle.
*/
void bcipher_close(bcipher_t encrypt);

/*
Summary:
Mode used when processing data.
Description:
3DES is not symmetric, therefore you must chose encrypt or decrypt mode.
This doesn't matter for DES.
*/
typedef enum bcipher_mode {
	bcipher_mode_encrypt,
	bcipher_mode_decrypt
} bcipher_mode;

/*
Summary:
Special data format (e.g. MPEG2 Transport) to be processed by the cipher engine.
*/
typedef enum bcipher_data_format {
	bcipher_data_format_raw, 	/* Process all data */
	bcipher_data_format_ts		/* Data will be processed in 188 byte packets,
						skipping the transport headers and verifying the sync byte. */
} bcipher_data_format;

/*
Summary:
Settings structure passed into bcipher_convert_data to control
the type of encryption or decryption performed.
*/
typedef struct bcipher_settings {
	bencryption_params encryption;				/* Encryption type and key */
	bcipher_mode mode;
	bcipher_data_format data_format;	/* Special data format processing */
	bool scrambling_control;	/* If true, then the cipher algorithm will set and
					clear the MPEG2 transport scrambling control field. This is necessary
					to support Broadcom trick modes with pvr encrypted streams. */
} bcipher_settings;

/*
Summary:
Required to initialize bcipher_settings.
*/
void bcipher_settings_init(
	bcipher_settings *settings,	/* [out] structure to be initialized */
	bcipher_t cipher			/* Cipher handle if there are resource-specific defaults */
	);

/*
Summary:
Encrypt or decrypt a buffer of data using the PVR encryption block.
*/
bresult
bcipher_convert_data(
	bcipher_t handle,		/* handle returned by bcipher_open() */
	void *dest,				/* [out,size_is(datalen)] data to be converted */
	const void *source,		/* [size_is(datalen)] data to be converted */
	size_t datalen,			/* length of data in bytes to be converted */
	const bcipher_settings *settings /* settings which control the type of encryption
								or decryption. */
	);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_CIPHER_H__ */

