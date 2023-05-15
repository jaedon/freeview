/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bstd.h $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 11/14/06 12:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/std/bstd.h $
 * 
 * Hydra_Software_Devel/18   11/14/06 12:19p erickson
 * PR25893: reorder bstd_cfg.h include so it can be properly used
 * 
 * Hydra_Software_Devel/17   9/15/05 7:03p erickson
 * PR17148: moved include bchp.h below bstd_cfg.h because some projects
 * define BCHP_CHIP in bstd_cfg.h
 * 
 * Hydra_Software_Devel/16   9/15/05 10:54a erickson
 * PR17148: moved BCHP checks into bchp.h
 * 
 * Hydra_Software_Devel/15   9/14/05 2:15p erickson
 * PR17148: added BCHP_VER check and removed deprecated BCHP_REV_XX check
 * 
 * Hydra_Software_Devel/14   5/18/05 11:38a agin
 * PR14720: B2, C1, C2 compilation support.
 * 
 * Hydra_Software_Devel/14   5/18/05 11:38a agin
 * PR14720: B2, C1, C2 compilation support.
 * 
 * Hydra_Software_Devel/13   3/25/05 2:57p jasonh
 * PR 14364: Added B2 and C1 revision detection.
 * 
 * Hydra_Software_Devel/12   1/7/05 1:06p jasonh
 * PR 13752: Removed reference to BCHP_FAMILY*
 * 
 * Hydra_Software_Devel/11   12/30/04 2:42p hongtaoz
 * PR13488: added C0 family rev define;
 * 
 * Hydra_Software_Devel/10   8/17/04 3:14p aram
 * PR11787: fixed a compile issue for C0
 * 
 * Hydra_Software_Devel/9   8/17/04 4:15p shyam
 * PR 12312 : Added define for B1 Rev of 7038
 * 
 * Hydra_Software_Devel/8   5/24/04 8:03p jasonh
 * PR 11189: Merge down from B0 to main-line
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/28/04 1:02p hongtaoz
 * PR10566: added  support for 7038B0;
 * 
 * Hydra_Software_Devel/7   4/15/04 12:17p erickson
 * PR10566: Modified the #error messages to fix spelling mistake and be
 * complete
 * 
 * Hydra_Software_Devel/6   4/12/04 12:00p vsilyaev
 * PR 10566: Added test  for standard defines
 * 
 * Hydra_Software_Devel/4   4/4/03 3:16p marcusk
 * Updated references of BSTD_XXX_ENDIAN to BSTD_ENDIAN_XXX
 * 
 * Hydra_Software_Devel/2   3/10/03 2:43p vsilyaev
 * Added bdbg.h header.
 *
 ***************************************************************************/
#ifndef BSTD_H__
#define BSTD_H__

/* standard types */
#include "bstd_defs.h"

/* standard defines */
#define BSTD_ENDIAN_BIG 4321
#define BSTD_ENDIAN_LITTLE 1234

/* platform specific include file. This must
come before other basemodules which may depend upon #defines. 
Setting compile options in bstd_cfg.h or on the command line
should be equivalent. */
#include "bstd_cfg.h"

/* base error codes */
#include "berr.h"

/* debug interface */
#include "bdbg.h"

/* chip interface */
#include "bchp.h"

/*
 * insuring we have a proper configuration
 */

#if ((BSTD_CPU_ENDIAN != BSTD_ENDIAN_BIG) && \
     (BSTD_CPU_ENDIAN != BSTD_ENDIAN_LITTLE))
#error Must define BSTD_CPU_ENDIAN as BSTD_ENDIAN_BIG or BSTD_ENDIAN_LITTLE.
#endif

#endif /* #ifndef BSTD_H__ */

/* end of file */
