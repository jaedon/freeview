/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bbase64.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/22/06 4:40p $
 *
 * Module Description:
 *
 * BASE64 encoder/decoder
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/utils/bbase64.h $
 * 
 * 1   8/22/06 4:40p vsilyaev
 * PR 23826: base64 encoder and decoder
 * 
 *******************************************************************************/
#ifndef _BBASE64_H__
#define _BBASE64_H__

#ifdef __cplusplus
extern "C"
{
#endif

int bbase64_encode(const uint8_t *src, size_t src_len, char *dest, size_t dst_len);
int bbase64_decode(const char *src, size_t src_len, uint8_t *dest, size_t dst_len);

#ifdef __cplusplus
}
#endif


#endif /* _BBASE64_H__ */

