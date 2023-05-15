/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_cipher.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/25/08 10:13a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_cipher.c $
 * 
 * 2   3/25/08 10:13a jgarrett
 * PR 40857: Implementing stubs
 * 
 *************************************************************************/

#include "bsettop_impl.h"

BDBG_MODULE(cipher);

/*
Summary:
	Open a cipher handle.
*/
bcipher_t bcipher_open(
	bobject_t id
	)
{
    bresult rc;
    /* Not supported on post-7038 platforms */
    BSTD_UNUSED(id);
    rc = BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

/*
Summary:
	Close a cipher handle.
*/
void bcipher_close(bcipher_t encrypt)
{
    BSTD_UNUSED(encrypt);
}

/*
Summary:
Required to initialize bcipher_settings.
*/
void bcipher_settings_init(
	bcipher_settings *settings,	/* [out] structure to be initialized */
	bcipher_t cipher			/* Cipher handle if there are resource-specific defaults */
	)
{
    BSTD_UNUSED(settings);
    BSTD_UNUSED(cipher);
}


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
	)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(dest);
    BSTD_UNUSED(source);
    BSTD_UNUSED(datalen);
    BSTD_UNUSED(settings);
    return BSETTOP_ERROR(berr_not_supported);
}

