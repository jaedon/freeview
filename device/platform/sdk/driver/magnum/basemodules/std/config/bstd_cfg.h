/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bstd_cfg.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 2/21/12 1:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/std/config/bstd_cfg.h $
 * 
 * Hydra_Software_Devel/8   2/21/12 1:57p erickson
 * SW7425-2130: remove unnecessary sample defines
 * 
 * Hydra_Software_Devel/7   1/7/05 1:16p jasonh
 * PR 13752: Removed reference to BCHP_FAMILY*
 * 
 * Hydra_Software_Devel/7   1/7/05 1:10p jasonh
 * PR 13752: Removed reference to BCHP_FAMILY*
 * 
 * Hydra_Software_Devel/7   1/7/05 1:08p jasonh
 * PR 13752: Removed reference to BCHP_FAMILY*
 * 
 * Hydra_Software_Devel/7   1/7/05 1:06p jasonh
 * PR 13752: Removed reference to BCHP_FAMILY*
 * 
 * Hydra_Software_Devel/6   4/12/04 12:01p vsilyaev
 * PR 10566: Moved test  for standard defines to bstd.h
 * 
 * Hydra_Software_Devel/5   8/13/03 2:14p aram
 * removed 7120, 7036 and added 7038
 * 
 * Hydra_Software_Devel/4   4/17/03 1:29p dlwin
 * Fixed problem with test for BSTD_CPU_ENDIAN.
 * 
 * Hydra_Software_Devel/3   4/4/03 4:56p marcusk
 * Updated references of BSTD_XXX_ENDIAN to BSTD_ENDIAN_XXX
 * 
 * Hydra_Software_Devel/3   4/4/03 4:51p marcusk
 * Updated references of BSTD_XXX_ENDIAN to BSTD_ENDIAN_XXX
 * 
 * Hydra_Software_Devel/2   4/4/03 3:28p jasonh
 * Removed board configuration and added ifdefs to check that required
 * settings are given.
 * 
 * Hydra_Software_Devel/1   3/26/03 11:08a jasonh
 * Initial version.
 *
 ***************************************************************************/
#ifndef BSTD_CFG_H__
#define BSTD_CFG_H__

/***************************************************************************
The following is a list of platform-specific defines. These are used to
create code that can be used for multiple chipsets/revisions, multiple 
families/revisions, and different board types. Porting interfaces and
SysLib authors should use them in Init or Open functions, not speckled
throughout the code. 

The user has two options:
-  Copy this file into their build environment and adjust the values to
   meet their platform specifications.
-  Provide values for the defines through makefile settings. In that case,
   this file may be used directly to satisfy the need for the bstd_cfg.h
   include.
****************************************************************************/

/* CPU endianness (choose one only) */
#if 0
#define BSTD_CPU_ENDIAN BSTD_ENDIAN_BIG
#define BSTD_CPU_ENDIAN BSTD_ENDIAN_LITTLE
#endif

/* enabling the debug build */
#if 0
#define BDBG_DEBUG_BUILD 1
#endif

/* chip type (choose one only) */
#if 0
#define BCHP_CHIP 7038
#endif

/* chip revisions supported (choose one or more) */
#if 0
#define BCHP_REV_A0
#endif

#endif /* #ifndef BSTD_CFG_H__ */

/* end of file */
