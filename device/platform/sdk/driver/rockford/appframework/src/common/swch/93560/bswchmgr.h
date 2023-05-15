/***************************************************************************
*     Copyright (c) 2003-2005, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bswchmgr.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 9/13/05 2:10p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/93560/linuxuser/bswchmgr.h $
* 
* Hydra_Software_Devel/2   9/13/05 2:10p darnstein
* PR16885: Improve #ifdefs for chip type.
* 
* Hydra_Software_Devel/1   9/12/05 7:17p darnstein
* PR16885: BSWCH support for simple programs.
* 
***************************************************************************/

#ifndef _BSWCHMGR_H
#define _BSWCHMGR_H

#include "bavc.h"
#include "framework.h"

#if __cplusplus
extern "C" {
#endif

typedef enum {
	Bswchmgr_format_cvbs,
	Bswchmgr_format_svideo,
	Bswchmgr_format_component
} Bswchmgr_format;

#if (BCHP_CHIP==3560) /** { **/
BERR_Code
bswchmgr_set (BFramework_Info* pFrmInfo, Bswchmgr_format bsw_format, 
	BAVC_SourceId sourceId);
#else /** } ! (BCHP_CHIP==3560) { **/
#define bswchmgr_set(a,b,c) BERR_SUCCESS
#endif /** } (BCHP_CHIP==3560) **/

#if __cplusplus
}
#endif

#endif /* _BSWCHMGR_H */
