/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgfx_blit.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/9/12 4:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/bgfx_blit.h $
 * 
 * Hydra_Software_Devel/3   10/9/12 4:05p syang
 * SW7346-572: include bgrc.h
 * 
 * Hydra_Software_Devel/2   10/9/12 3:19p syang
 * SW7346-572: fix compile error for chips without grc
 * 
 * Hydra_Software_Devel/1   10/9/12 1:48p syang
 * SW7346-572: rm symbolic link and init a trimmed version
 * 
 * Hydra_Software_Devel/5   11/24/10 12:54p pntruong
 * SW7552-17: Added initial 7552 support.
 *
 * Hydra_Software_Devel/4   7/31/09 6:50p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/3   5/16/06 11:15a jessem
 * PR 17530: Added BTMR support.
 *
 * Hydra_Software_Devel/2   8/4/05 5:26p darnstein
 * PR16057: adapt to use with new appframeworks package: get rid of
 * CPUINT1_ calls. Also, use new global variable g_frmInfo instead of
 * g_h* variables.
 *
 * Hydra_Software_Devel/1   8/4/05 1:45p darnstein
 * PR16057: source files for GRC part of BMEtest.
 *
 ***************************************************************************/

#ifndef BGFX_BLIT_H__
#define BGFX_BLIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsur.h"
#if (BTST_P_SUPPORT_GRC)
#include "bgrc.h"
#endif
	
/* This file is a trimmed version of
 *      /rockford/applications/bmetest/grc_test/bgfx_blit.h
 * You could copy from the original file if you need more BGFX functions
 */
	
/***************************************************************************/
typedef struct
{
	uint32_t ulX;
	uint32_t ulY;
	uint32_t ulWidth;
	uint32_t ulHeight;
}
BGFX_Rect;

void BGFX_MakeImageARGB8888( BSUR_Surface_Handle hSurface );
void BGFX_CopyImage( BSUR_Surface_Handle hDstSurface, BSUR_Surface_Handle hSrcSurface );
void BGFX_DumpImage( BSUR_Surface_Handle hSurface, BSUR_Surface_Handle hSrcSurface, uint8_t *pucOp );
void BGFX_MakePalette( uint32_t *pulPalette, uint32_t ulBpp );
#if (BTST_P_SUPPORT_GRC)	
void BGFX_Blit_ColorSpaceConvert( BGRC_Handle hGrc, BSUR_Surface_Handle hSrcSurface, BSUR_Surface_Handle hDstSurface, BGFX_Rect *pSrcRect, BGFX_Rect *pDstRect, uint32_t ulPixel );
#endif
	
#ifdef __cplusplus
}
#endif

#endif /* BGFX_BLIT_H__ */

/* end of file */
	
