/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip_mss.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/27/07 11:10a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip_mss.h $
 * 
 * 2   2/27/07 11:10a marcusk
 * PR 28155: Added support for language code.
 * 
 * 1   2/7/05 11:26p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:04p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef PSIP_MSS_H__
#define PSIP_MSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "psip_common.h"

/* TODO: add support for Unicode beyond ASCII */

uint8_t PSIP_MSS_getNumStrings( PSIP_MSS_string mss );
BERR_Code PSIP_MSS_getString( PSIP_MSS_string mss, int stringNum, int *p_stringSize, char *p_string );
BERR_Code PSIP_MSS_getCode( PSIP_MSS_string mss, int stringNum, char **ppLanguageCode );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */
