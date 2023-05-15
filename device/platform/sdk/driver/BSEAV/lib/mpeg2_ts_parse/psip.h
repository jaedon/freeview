/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:23p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip.h $
 * 
 * 1   2/7/05 11:23p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:02p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef PSIP_H__
#define PSIP_H__

#ifdef __cplusplus
extern "C" {
#endif

uint8_t PSIP_getProtocolVersion( const uint8_t *buf );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */
