/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: native_private.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 5/11/12 2:18p $
 *
 * Module Description: Private Native header file.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/native_private.h $
 * 
 * Hydra_Software_Devel/7   5/11/12 2:18p nissen
 * SW7125-1283: Fixed P3D memory leak when closing.
 * 
 * Hydra_Software_Devel/6   1/17/12 4:07p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/5   9/23/08 6:41p nissen
 * PR 46638: Optimized rectangle blits.
 * 
 * Hydra_Software_Devel/4   9/8/08 9:07p nissen
 * PR 46638: Added support for waiting for blits.
 * 
 * Hydra_Software_Devel/3   9/8/08 3:23p nissen
 * PR 46638: Added blit function to support 2D rectangle polygon blits.
 * 
 * Hydra_Software_Devel/2   11/24/07 12:03p nissen
 * PR 35852: Added documentation.
 * 
 * Hydra_Software_Devel/1   11/16/07 6:51a nissen
 * PR 35855: Added support for new native model.
 * 
 ***************************************************************************/

#ifndef _NATIVE_PRIVATE_H__
#define _NATIVE_PRIVATE_H__

#include "bstd_defs.h"
#include "berr.h"
#include "bmem.h"
#include "bsur.h"
#include "bgrc.h"
#include "bp3d.h"

#include "native.h"

#include "egl.h"
#include "egl_private.h"

/***************************************************************************
Summary:
	Inits Native Display
	
Description:
	This function returns the Memory (BMEM) and 3D Graphics (P3D) module 
	handles from the Magnum porting interface corresponding to the 
	Native Display. These modules have been previously opened by the 
	underlying platform.
****************************************************************************/
void native_P_InitDisplay( 
	NativeDisplay display,             /* Native Display */
	BMEM_Handle *phMem,                /* Pointer to returned BMEM handle */
	BP3D_Handle *phP3d                 /* Pointer to returned P3D handle */
);

/***************************************************************************/
void native_P_TermDisplay( 
	NativeDisplay display
);

/***************************************************************************
Summary:
	Inits Native Window
	
Description:
	This function returns the Surface (BSUR) handle and size corresponding
	to the Native Window. The Surface handle has been previously created
	by the underlying platform.
****************************************************************************/
void native_P_InitWindow( 
	NativeWindow window,             /* Native Window */
	BSUR_Surface_Handle *phSurface,  /* Pointer to returned BSUR handle */
	unsigned int *puiFormat,
	unsigned int *puiOffset,
	void **ppvMemory,
	int *piPitch,
	int *piWidth,                    /* Pointer to returned width */
	int *piHeight                    /* Pointer to returned height */
);

/***************************************************************************
Summary:
	Inits Native Pixmap
	
Description:
	This function returns the Surface (BSUR) handle and size attached
	to the Native Pixmap.  The Surface handle has been previously created
	by the underlying platform.
****************************************************************************/
void native_P_InitPixmap( 
	NativePixmap pixmap,             /* Native Pixmap */ 
	BSUR_Surface_Handle *phSurface,  /* Pointer to returned BSUR handle */ 
	unsigned int *puiFormat,
	unsigned int *puiOffset,
	void **ppvMemory,
	int *piPitch,
	int *piWidth,                    /* Pointer to returned width */ 
	int *piHeight                    /* Pointer to returned height */
);

/***************************************************************************
Summary:
	Copys Surface
	
Description:
	This function copies a BSUR Surface to another Surface which may need 
	to be scaled with filering.
****************************************************************************/
void native_P_CopySurface(
	NativeDisplay display, 
	BSUR_Surface_Handle hDstSurface, 
	BSUR_Surface_Handle hSrcSurface
);

/***************************************************************************/
void native_P_BlitSurface( 
	EGL_P_Context *pContext,
	NativeDisplay display,
	BSUR_Surface_Handle hDstSurface,
	int iDstX, int iDstY, int iDstWidth, int iDstHeight,
	BSUR_Surface_Handle hSrcSurface,
	int iSrcX, int ifSrcY, int iSrcWidth, int iSrcHeight,
	float fScaleX, float fScaleY, 
	float fPhaseX, float fPhaseY, 
	uint32_t ulColor,
	GLboolean bSame
);

/***************************************************************************/
void native_P_BlitWait( 
	EGL_P_Context *pContext,
	NativeDisplay display,
	BSUR_Surface_Handle hSurface
);
#endif /* _NATIVE_PRIVATE_H__ */
