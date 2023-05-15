/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip_huffman.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:25p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip_huffman.h $
 * 
 * 1   2/7/05 11:25p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:04p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef PSIP_HUFFMAN_H__
#define PSIP_HUFFMAN_H__

#include "psip_mss.h"

#ifdef __cplusplus
extern "C" {
#endif

void PSIP_Huffman_decode( PSIP_MSS_string *p_string, int maxStringSize, char *p_text );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */
