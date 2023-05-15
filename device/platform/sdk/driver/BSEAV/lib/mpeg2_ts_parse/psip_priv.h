/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: psip_priv.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:26p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/psip_priv.h $
 * 
 * 1   2/7/05 11:26p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/4/04 9:56a erickson
 * PR9217: converted BDBG_ASSERT calls to CHECK calls. Don't assert on bad
 * data.
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:04p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef PSIP_PRIV_H__
#define PSIP_PRIV_H__

#include "ts_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSIP_PROTOCOL_VERSION_OFFSET	8
#define PSIP_TABLE_DATA_OFFSET			9

#define CHECK(COND) \
	do {if (!(COND)) BDBG_ERR(("Bad CHECK: %s at %s, %d", #COND, __FILE__, __LINE__)); } while (0)

#ifdef __cplusplus
}
#endif
#endif
/* End of File */
