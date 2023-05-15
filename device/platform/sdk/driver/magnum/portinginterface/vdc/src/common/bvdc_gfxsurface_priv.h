/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_gfxsurface_priv.h $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 8/9/12 11:19a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_gfxsurface_priv.h $
* 
* Hydra_Software_Devel/6   8/9/12 11:19a syang
* SW7425-2172: add SetSurface support for MFD gfx feeding
* 
* Hydra_Software_Devel/5   6/14/12 3:43p syang
* SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
* prepare to add SetSurface support to MFD
* 
* Hydra_Software_Devel/4   5/23/12 6:47p syang
* SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
* use gfxsurface sub-module
* 
* Hydra_Software_Devel/3   5/9/12 2:55p syang
* SW7425-2093:  fix compile error for diff chips
* 
* Hydra_Software_Devel/2   5/9/12 12:13p syang
* SW7425-2093:  compile warning msg clean up for diff chips
* 
* Hydra_Software_Devel/1   5/9/12 10:48a syang
* SW7425-2093: abstract out gfx surface managing code into separate
* submodule, to be shared by gfd and vfd
*
***************************************************************************/
#ifndef BVDC_GFXSURFACE_PRIV_H__
#define BVDC_GFXSURFACE_PRIV_H__

#include "bstd.h"             /* standard types */
#include "bvdc.h"
#include "bvdc_source_priv.h"
#include "bvdc_common_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BVDC_GFXSUR);

#define BVDC_P_ORIENTATION_IS_3D(ori) \
	((BFMT_Orientation_e3D_LeftRight  == (ori)) || \
	 (BFMT_Orientation_e3D_OverUnder  == (ori)))

/* copy from bvdc_gfxfeeder_priv.h" */
#define BVDC_P_SUPPORT_GFD_VER_1            (1) /* 7400, 7405, 7325, 7335 */

#define BVDC_P_SUPPORT_OLD_SET_ALPHA_SUR  1
	
/*-------------------------------------------------------------------------
 * VDC internal information for isr, new or cur surface 
 */
typedef struct BVDC_P_SurfaceInfo
{
	BAVC_Gfx_Picture                stAvcPic;            /* AVC gfx pic passed by user */

	/* main surface */
	uint32_t                        ulAddress;           /* main surface pixel addr */
	uint32_t                        ulPitch;             /* main surface pitch  */
	uint32_t                        ulWidth;             /* main surface width */
	uint32_t                        ulHeight;            /* main surface height */
	BPXL_Format                     eInputPxlFmt;        /* original surface pixel format */
	BPXL_Format                     eActivePxlFmt;       /* active pixel format after palette lookup */

	bool                            bChangePaletteTable; /* for table loading */
	uint32_t                        ulPaletteAddress;
	uint32_t                        ulPaletteNumEntries;
	uint32_t                        ulPaletteUsageMask;

	/* Right surface used with stereo 3D. It is assumed that 
	 * hRSurface's size and format are equal to hSurface. 
	 * However, this assumption is checked during validation 
	 * and an error is returned if proven otherwise.
	 */
	uint32_t                        ulRAddress;          /* right surface pixel addr */

#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
	/* separate alpha surface for old chips: it must be format of BPXL_eW1 */
	uint32_t                        ulAlphaAddress;      /* alpha surface pixel addr */
	uint32_t                        ulAlphaPitch;        /* alpha surface pitch */
#endif	

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_GFD)
	BSUR_TestFeature1_Settings      stTestFeature1;
#endif

	/* dirty bit for AppplyChange */
	bool                            bChangeSurface;

} BVDC_P_SurfaceInfo;

/*-------------------------------------------------------------------------
 * Used to store one record of surface set to gfd / vfd hw.
 */
typedef struct BVDC_P_GfxSurNode
{
	BAVC_Gfx_Picture                stAvcPic;            /* AVC gfx pic passed by user */

	bool                            bExeDuringSet;       /* RUL executed while setting */
	uint32_t                        ulVsyncCntr;         /* vsync cntr seen after setting */
	uint32_t                        ulAddr;              /* main surface pixel addr */
	uint32_t                        ulRAddr;             /* right surface pixel addr */
	uint32_t                        ulPitch;             /* surface pitch */
	
} BVDC_P_GfxSurNode;

/*-------------------------------------------------------------------------
 * graphics surface manager main context
 */
typedef struct BVDC_P_GfxSurfaceContext
{
	BDBG_OBJECT(BVDC_GFXSUR)

	BREG_Handle                      hRegister;
    BAVC_SourceId                    eSrcId;

	/* only 3d src needs ping-pong buffer mechanism */
	bool                             b3dSrc;
	
	/* private surface info */
	BVDC_P_SurfaceInfo               stIsrSurInfo;
	BVDC_P_SurfaceInfo               stNewSurInfo;
	BVDC_P_SurfaceInfo               stCurSurInfo;

	/* top and left clip related offset in bytes,
	 * it is the same for both left and right surfaces
	 * it is put here because it is needed when query
	 * current surface used in HW */
	uint32_t                         ulMainByteOffset;
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)	
	uint32_t                         ulAlphaByteOffset;
#endif
	
	/* main surface addr register inside GFD / VFD, rember this for
	 * GetHwUsingSurs_isr */
	uint32_t                         ulHwMainSurAddrReg;
	
	/* double bufferred surface address shadow registers, for atomic switch
	 * 5 registers used */
	uint32_t                         ulSurAddrReg[2];
	uint32_t                         ulRSurAddrReg[2];
	uint32_t                         ulRegIdxReg;

	uint32_t                         ulRegIdx;

	/* vsync counter register for stSurNode managing,
	 * it is updated by RUL when it is executed, and is read by CPU 
	 * 1 registers used */
	 uint32_t                        ulVsyncCntrReg;

	/* setting records, for HW surface usage query */
	BVDC_P_GfxSurNode                stSurNode[4];
	uint8_t                          ucNodeIdx;

} BVDC_P_GfxSurfaceContext;


/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxSurface_AllocShadowRegs
 *
 * Called by GfxFeeder or VfdFeeder to allocate surface address shadow
 * registers from scratch pool, 
 */
BERR_Code BVDC_P_GfxSurface_AllocShadowRegs
	( BVDC_P_GfxSurfaceContext        *pGfxSurface,
	  BRDC_Handle                      hRdc,
	  bool                             b3dSrc );

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxSurface_FreeShadowRegs
 *
 * Called to fee surface address shadow registers back to scratch pool, 
 * when the shadow registers are no longer needed.
 */
BERR_Code BVDC_P_GfxSurface_FreeShadowRegs
	( BVDC_P_GfxSurfaceContext        *pGfxSurface,
	  BRDC_Handle                      hRdc );

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxSurface_Init
 *
 * Called by BVDC_P_GfxFeeder_Init or BVDC_P_Feeder_Init when a source
 * handle is created with BVDC_Source_Created
 */
BERR_Code BVDC_P_GfxSurface_Init
	( BVDC_P_GfxSurfaceContext        *pGfxSurface );

/*------------------------------------------------------------------------
 * {private}
 *  BVDC_P_GfxSurface_SetSurface_isr
 *
 *  It will check the BAVC_Gfx_Picture struct pointed by pAvcGfxPic to ensure
 *  that there is no conflict inside itself. It will then program the surface
 *  address shadow registers if size / pitch and format match the current. 
 *  Otherwise it will mark the surface as "changed" so that ApplyChange will 
 *  validate it, and then the next rul build will program it into RUL
 */
BERR_Code BVDC_P_GfxSurface_SetSurface_isr 
	( BVDC_P_GfxSurfaceContext        *pGfxSurface,
	  BVDC_P_SurfaceInfo              *pSurInfo,
	  const BAVC_Gfx_Picture          *pAvcGfxPic );

/*-------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxSurface_SetShadowRegs_isr
 *
 * set the surface addr to the shadow registers.
 * Called from BVDC_Source_SetSurface or BuildRul after ApplyChange
 */
void BVDC_P_GfxSurface_SetShadowRegs_isr
	( BVDC_P_GfxSurfaceContext        *pGfxSurface,
	  BVDC_P_SurfaceInfo              *pSurInfo );

/*------------------------------------------------------------------------
 *  {private}
 *	BVDC_P_GfxSurface_GetSurfaceInHw_isr
 *
 *  Read HW registers to decide which picture node the HW is using.
 */
BAVC_Gfx_Picture *BVDC_P_GfxSurface_GetSurfaceInHw_isr
	( BVDC_P_GfxSurfaceContext        *pGfxSurface );

#endif /* #ifndef BVDC_GFXSURFACE_PRIV_H__ */
/* End of file. */
