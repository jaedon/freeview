/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib_bitread.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/27/10 1:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/bvbilib_bitread.h $
 * 
 * Hydra_Software_Devel/7   8/27/10 1:46p darnstein
 * SW3548-3022: merge to production branch.
 * 
 * Hydra_Software_Devel/SW3548-3022/1   7/28/10 4:39p darnstein
 * SW3548-3022: bitstream parsing is relayed to commonutils module BUDP.
 * 
 * Hydra_Software_Devel/6   9/11/07 5:18p darnstein
 * PR25708: First release of SCTE encoder software.
 * 
 * Hydra_Software_Devel/5   3/22/05 1:51p darnstein
 * PR 14504: Arman's speed optimizations have been applied.
 * 
 * Hydra_Software_Devel/4   1/14/05 11:58a darnstein
 * PR 13797: Allow read from address that is not a multiple of 4.  Fix
 * counting error that Bill Fassl discovered.
 * 
 * Hydra_Software_Devel/3   10/18/04 5:06p darnstein
 * PR 12874: Restore the calling sequence of BVBILIB_DCCparse() to avoid
 * giving problems to Broadcom customers.
 * 
 * Hydra_Software_Devel/2   10/12/04 11:36a darnstein
 * PR 12874: fix a structured comment.
 * 
 * Hydra_Software_Devel/1   10/11/04 3:23p darnstein
 * PR12874: Initial version.
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
Software has been moved to the BUDP commonutils module.

</verbatim>
***************************************************************************/

#ifndef BVBILIBBITREAD_H__
#define BVBILIBBITREAD_H__

#include "budp_bitread.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Data structures */
#define BVBIlib_Bitread_Context BUDP_Bitread_Context

/* Functions */
#define BVBIlib_Bitread_Init BUDP_Bitread_Init 
#define BVBIlib_Bitread_Read BUDP_Bitread_Read
#define BVBIlib_Bitread_Byte BUDP_Bitread_Byte
#define BVBIlib_Bitread_GetByteOffset BUDP_Bitread_GetByteOffset
#define BVBIlib_Bitread_next_start_code BUDP_Bitread_next_start_code

#ifdef __cplusplus
}
#endif
 
#endif /* BVBILIBBITREAD_H__ */
