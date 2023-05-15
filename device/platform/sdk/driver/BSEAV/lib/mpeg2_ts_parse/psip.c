/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:23p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip.c $
 * 
 * 1   2/7/05 11:23p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:05p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "psip_priv.h"
#include "psip.h"

uint8_t PSIP_getProtocolVersion( const uint8_t *buf )
{
	return buf[PSIP_PROTOCOL_VERSION_OFFSET];
}
