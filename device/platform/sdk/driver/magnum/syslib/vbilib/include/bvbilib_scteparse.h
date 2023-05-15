/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib_scteparse.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/27/10 1:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/bvbilib_scteparse.h $
 * 
 * Hydra_Software_Devel/3   8/27/10 1:58p darnstein
 * SW3548-3022: merge to production branch.
 * 
 * Hydra_Software_Devel/SW3548-3022/1   7/28/10 4:40p darnstein
 * SW3548-3022: bitstream parsing is relayed to commonutils module BUDP.
 * 
 * Hydra_Software_Devel/2   10/26/07 10:51a ptimariu
 * PR36451: closing bracket, __cplusplus
 * 
 * Hydra_Software_Devel/1   9/7/07 5:10p darnstein
 * PR25708: Parsers for SCTE 20 and SCTE 21 data.
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
Software has been moved to the BUDP commonutils module. */

</verbatim>
***************************************************************************/

#ifndef BVBILIBSCTEPARSE_H__
#define BVBILIBSCTEPARSE_H__

#include "budp_scteparse.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Data structures */
typedef enum {
	BVBIlib_SCTEparse_Format_Unknown   = BUDP_SCTEparse_Format_Unknown,
	BVBIlib_SCTEparse_Format_SCTE20    = BUDP_SCTEparse_Format_SCTE20,
	BVBIlib_SCTEparse_Format_SCTE21CC  = BUDP_SCTEparse_Format_SCTE21CC,
	BVBIlib_SCTEparse_Format_SCTE21ACC = BUDP_SCTEparse_Format_SCTE21ACC,
	BVBIlib_SCTEparse_Format_SCTE21PAM = BUDP_SCTEparse_Format_SCTE21PAM
}
BUDP_SCTEparse_Format;

#define BVBIlib_SCTEparse_sctedata BUDP_SCTEparse_sctedata

/* Functions */
#define BVBIlib_SCTE20parse BUDP_SCTE20parse 
#define BVBIlib_SCTE21parse BUDP_SCTE21parse

#ifdef __cplusplus
}
#endif

#endif /* BVBILIBSCTEPARSE_H__ */
