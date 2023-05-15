/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_ca.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:29p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/ts_ca.h $
 * 
 * 1   2/7/05 11:29p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:05p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef TS_CA_H__
#define TS_CA_H__

#ifdef __cplusplus
extern "C" {
#endif

TS_PSI_descriptor TS_CA_getDescriptor( const uint8_t *buf, int descriptorNum );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */
