/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_gfxfeeder_priv.h $
* $brcm_Revision: Hydra_Software_Devel/78 $
* $brcm_Date: 6/14/12 3:43p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_gfxfeeder_priv.h $
* 
* Hydra_Software_Devel/78   6/14/12 3:43p syang
* SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
* prepare to add SetSurface support to MFD
* 
* Hydra_Software_Devel/77   5/30/12 4:54p syang
* SW7425-2093: extra ApplyChange with 1st SetSurface no longer needed;
* vfd/cap time stamp related scratch registers now share with vfd's
* surface manager's; added vfd resource check; more clean up
* 
* Hydra_Software_Devel/76   5/9/12 10:53a syang
* SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
* error fix with GetSurface; use shared gfxsurface to manage gfx surface
* 
* Hydra_Software_Devel/75   1/30/12 1:59p vanessah
* SW7425-2287: roll back 7425/7231/7346 Bx GFX version due to incomplete
* HW fixes set
* 
* Hydra_Software_Devel/74   1/29/12 3:48p vanessah
* SW7425-2287:  rollback workaround for HW7425-385 for GFD_VER_5
* 
* Hydra_Software_Devel/73   1/27/12 11:21p vanessah
* SW7425-2287: remove workaround for HW7425-385 for GFD_VER_5
* 
* Hydra_Software_Devel/72   1/26/12 12:05p vanessah
* SW7429-42: read vscl line buffer length from HW_conf register
* 
* Hydra_Software_Devel/71   1/10/12 5:29p vanessah
* SW7429-42: removal SW_INIT for GFD version e
* 
* Hydra_Software_Devel/70   11/23/11 11:31a tdo
* SW7435-9: add support for 7435A0 in VDC
*
* Hydra_Software_Devel/69   7/7/11 7:18p vanessah
* SW7231-279: fix current info and new info confusion
*
* Hydra_Software_Devel/68   6/9/11 10:29a vanessah
* SW7231-187: workaround for HW init when orientation does not match with
* the previous and the following one.
*
* Hydra_Software_Devel/67   4/29/11 1:34p vanessah
* SW7422-118: add GFD cold-start
*
* Hydra_Software_Devel/66   4/1/11 7:38a vanessah
* SW7422-118: double buffer for GFD PI support
*
* Hydra_Software_Devel/65   1/20/11 10:20a vanessah
* SW7422-43:  Force the usage of HSCL and VSCL due to HW bug. Fix is
* ready for B0 tapeout. (See HW7425-385)
*
* Hydra_Software_Devel/64   1/18/11 4:53p vanessah
* SW7422-43:  3D graphics support
*
* Hydra_Software_Devel/63   11/10/10 4:36p albertl
* SW7550-610: Extended BVDC_Window_SetCoefficientIndex to work for
* graphics windows.
*
* Hydra_Software_Devel/62   11/1/10 1:13p vanessah
* SW7422-43:  merge 2D into 3D with a unified interface, change input
* data structure as const
*
* Hydra_Software_Devel/61   10/29/10 5:07p vanessah
* SW7422-43:  3D graphics render,merge 2D into 3D with a unified
* interface
*
* Hydra_Software_Devel/60   10/25/10 7:22p vanessah
* SW7422-43:  Handle GFD programming for 3D.
*
* Hydra_Software_Devel/59   10/11/10 12:35p jessem
* SW7420-173: Added support for VDC as source feature.
*
* Hydra_Software_Devel/58   8/26/10 7:40p albertl
* SW7405-4556, SW7405-4515: Changed RGB attenuation to use inverse of
* user matrix C.
*
* Hydra_Software_Devel/57   6/16/10 3:05p yuxiaz
* SW7405-4503, SW3548-2976: Black video on component and hdmi outputs in
* the latest VDC.
*
* Hydra_Software_Devel/56   6/14/10 3:37p hongtaoz
* SW3548-2976: soft init GFD when DCX switches from ON to OFF, and when
* GFD source is created; added GFD BVB error monitoring to debug build;
*
* Hydra_Software_Devel/55   5/7/10 7:10p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings.
*
* Hydra_Software_Devel/54   4/19/10 10:15p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic. Move
* BDBG_OBJECT_ID_DECLARE private header files instead of .c.
*
* Hydra_Software_Devel/53   4/7/10 11:30a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/52   4/5/10 4:06p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/51   3/18/10 4:00p syang
* SW7550-271: added error check for unsupported 7550 GFD1 features
*
* Hydra_Software_Devel/50   3/17/10 12:02p syang
* SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
* same as video; ensure that reader and writer vnetState match in sync-
* locked case; aligned width up to even for stride in feeder and cap;
* assert mosaic mode doesn't co-exist with dest cut;
*
* Hydra_Software_Devel/49   1/12/10 8:42p albertl
* SW3548-2700: Changed RGB attenuation to use DVO matrix as YCbCr->RGB
* matrix and Matrix C as RGB->YCbCr matrix.
*
* Hydra_Software_Devel/48   9/11/09 4:35p tdo
* SW7420-338: Need to init correct GFD with correct capability
*
* Hydra_Software_Devel/47   5/1/09 3:42p syang
* PR 54760: fix Coverity Defect: rm unused member in
* BVDC_P_GfxPrevDirtyBits
*
* Hydra_Software_Devel/46   3/16/09 10:47p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/2   2/13/09 5:54p syang
* PR 45796:  added some vscl coeffs, GFD VSCL does show gfx, but with
* some gabage at top, and some non-linear effect.
*
* Hydra_Software_Devel/7420_mcvp/1   2/13/09 10:03a syang
* PR 45796: first time check in for GFD VSCL support
*
* Hydra_Software_Devel/45   10/9/08 5:29p syang
* PR 46891: add _isr to the name of func used in _isr context
*
* Hydra_Software_Devel/44   5/8/08 12:23p syang
* PR 39206: remove alpha surface for 3548 and newer chips
*
* Hydra_Software_Devel/43   9/20/07 1:18a albertl
* PR35135:  Cleaned up color space matrices and changed them to use same
* macro system.  Added color space conversion functionality to graphics
* windows.
*
* Hydra_Software_Devel/42   1/24/07 9:06p albertl
* PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
*
* Hydra_Software_Devel/41   12/18/06 11:27p pntruong
* PR22577: Merged back to mainline.
*
* Hydra_Software_Devel/Refsw_Devel_3563/2   12/11/06 5:22p albertl
* PR24587:  Added 3rd GFX Feeder source.
*
* Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 12:55p pntruong
* PR23222, PR23225:  Defer window and source heap assignments until user
* create.
*
* Hydra_Software_Devel/40   5/26/06 3:40p pntruong
* PR20642: Refactored handling of hList.
*
* Hydra_Software_Devel/39   4/4/06 1:50p hongtaoz
* PR20403: add triple displays for 7400;
*
* Hydra_Software_Devel/38   12/21/05 1:33p syang
* PR 15191: upadated gfd color matrix according to main video win's color
* matrix
*
* Hydra_Software_Devel/37   11/10/05 3:22p syang
* PR 12244: move alpha-only pixel constant color setting from BSUR to
* BVDC
*
* Hydra_Software_Devel/36   10/5/05 6:42p syang
* PR 16391: avoid to mess-up RUL when gfx sur is set with gfx win muted
* or with src disconnected from win.
*
* Hydra_Software_Devel/35   5/11/05 7:08p syang
* PR 15202: change to only override the curr field, but increase
* sur_rec_nodes num to 6, added memory flush after override RUL for
* sur_start. No more error message is seen so far.
*
* Hydra_Software_Devel/34   5/5/05 3:56p syang
* PR 15202: re-organize for better readability and debugability
*
* Hydra_Software_Devel/33   4/27/05 2:45p syang
* PR 14722: re-organize gfx-win dst cut and support sub-pixel clip (due
* to dst cut)
*
* Hydra_Software_Devel/32   3/17/05 6:38p pntruong
* PR14494: Add preliminary software support to 3560 A0.
*
* Hydra_Software_Devel/31   1/19/05 10:59a syang
* PR 13282: new gfx surface setting is now sent to HW as soon as
* possible: with ApplyChanges in user mode, and immediately in _isr
* mode.
*
* Hydra_Software_Devel/30   10/7/04 8:33p pntruong
* PR8960: BVDC_ApplyChanges and error recovery.
*
* Hydra_Software_Devel/29   10/1/04 5:41p syang
* PR 12821: now update gfd clip info according to offset in scaler-out
* rect
*
***************************************************************************/
#ifndef BVDC_GFXFEEDER_PRIV_H__
#define BVDC_GFXFEEDER_PRIV_H__

#include "bstd.h"             /* standard types */
#include "bvdc.h"
#include "bvdc_source_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_gfxsurface_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BVDC_GFX);

/* Core revs to track increatmental changes! */
/* CSC, Gamma-correction, and HSCL are removed, only P4 and AYCbCr8888 supported */
#define BVDC_P_SUPPORT_GFD_VER_0            (0) /* 7550 GFD1 */

#define BVDC_P_SUPPORT_GFD_VER_1            (1) /* 7400, 7405, 7325, 7335 */

/* 6 taps + 2 coeff phases HSCL */
#define BVDC_P_SUPPORT_GFD_VER_2            (2) /* 3548, 3556, 7125 */

/* VSCL added */
#define BVDC_P_SUPPORT_GFD_VER_3            (3) /* 7420, 7340, 7342, 7550 */

/* 3D Graphics */
#define BVDC_P_SUPPORT_GFD_VER_4            (4) /* 7422, 7425 7344 7346 7231: Ax/Bx*/

/* 3D Graphics with VSCL_LSBF_SIZE indication etc*/
#define BVDC_P_SUPPORT_GFD_VER_5            (5) /* 7428/29 7435, a0 */


#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_4)
/* HW7231-187 transition failure between 2D->3D 3D->2D*/
#define BVDC_P_GFX_INIT_WORKAROUND         (1)
#else
#define BVDC_P_GFX_INIT_WORKAROUND         (0)
#endif

#define  BVDC_P_GFX_INIT_CNTR               1
	
/* TODO Add real function into bsur */
BERR_Code BSUR_Palette_GetEntryFormat
( BSUR_Palette_Handle   hPalette,
  BPXL_Format          *pePaletteEntryFormat );

/*-------------------------------------------------------------------------
 *
 */
typedef union BVDC_P_GfxDirtyBits
{
	struct
	{
		/* configure dirty bits */
		uint32_t                 bChromaExpan             : 1; /* 0 */
		uint32_t                 bKey                     : 1;
		uint32_t                 bScaleCoeffs             : 1;
		uint32_t                 bGammaTable              : 1;
		uint32_t                 bConstantColor           : 1;
		uint32_t                 bClutTable               : 1; /* unused */
		uint32_t                 bScanType                : 1; /* for bInterlaced */
		uint32_t                 bSrcClip                 : 1;
		uint32_t                 bOutRect                 : 1; /* 8 */
		uint32_t                 bColorMatrix             : 1; /* unused */
		uint32_t                 bFlags                   : 1; /* some change in BVDC_P_GfxCfgFlags */
		uint32_t                 bDemoMode                : 1;
		uint32_t                 bOrientation             : 1;
		
		/* confugure and surface combined dirty bits */
		uint32_t                 bClipOrOut               : 1;
		uint32_t                 bCsc                     : 1;
		uint32_t                 bPxlFmt                  : 1;
		uint32_t                 bSurOffset               : 1; /* 16 */
		uint32_t                 bSurface                 : 1; 
		uint32_t                 bPaletteTable            : 1;
		
	} stBits;
	
	uint32_t aulInts [BVDC_P_DIRTY_INT_ARRAY_SIZE];

} BVDC_P_GfxDirtyBits;

/*-------------------------------------------------------------------------
 *
 */
typedef struct BVDC_P_GfxCfgFlags
{
	/* enable bits */
	uint32_t                 bEnableKey                     : 1;
	uint32_t                 bEnGfdHwAlphaPreMultiply       : 1;
	uint32_t                 bEnableGammaCorrection         : 1;
	uint32_t                 bNeedHorizScale                : 1;
	uint32_t                 bNeedVertScale                 : 1;
	uint32_t                 bNeedColorSpaceConv            : 1; /* unused */
	uint32_t                 bConstantBlending              : 1;
	
	/* output video scantype */
	uint32_t                 bInterlaced                    : 1;
	
	/* enable control: continuous or stop_on_field_end */
	uint32_t                 bContinueOnFieldEnd            : 1;

	uint32_t                 bDeringDemoMode                : 1;
	uint32_t                 bDejagDemoMode                 : 1;
	
} BVDC_P_GfxCfgFlags;

/*-------------------------------------------------------------------------
 * graphics feeder proccess configure info
 */
typedef struct BVDC_P_GfxFeederCfgInfo
{
	/* dirty bits: when ApplyChanges is called, current Dirty should be
	 * NewDirty | CurDirty, in order to handle that more than one
	 * applyChange is called before the RUL is built */
	BVDC_P_GfxDirtyBits      stDirty;
	
	/* misc one bit configs */
	BVDC_P_GfxCfgFlags       stFlags;
	
	/* chroma expansion method */
	BVDC_ChromaExpansion     eChromaExpansion;
	
	/* color key */
	uint8_t                  ucKeyedAlpha;
	uint32_t                 ulKeyMinAMNO;
	uint32_t                 ulKeyMaxAMNO;
	uint32_t                 ulKeyMaskAMNO;
	
	/* constant color for alpha only pixel */
	uint32_t                 ulConstantColor;

	/* window alpha */
	uint8_t                  ucWindowAlpha;  /* set to HW as default key alpha */

	/* horizontal up scale */
	BVDC_FilterCoeffs        eScaleCoeffs;  /* coeff mode */

	/* gamma correction */
	uint32_t                 ulNumGammaClutEntries;
	uint32_t                 ulGammaClutAddress;

	/* src clip rect, dest scale rectangle dimension, and up-scale */
	uint32_t                 ulCntLeft;  /* left with frac */
	uint32_t                 ulCntTopInt;  /* int, top int part only */
	uint32_t                 ulCntWidth;  /* int, round to ceiling */
	uint32_t                 ulCntHeight; /* int, round to ceiling */
	uint32_t                 ulOutWidth;  /* int */
	uint32_t                 ulOutHeight;  /* int */
	uint32_t                 ulHsclSrcStep;  /* src / out, with frac  */
	uint32_t                 ulVsclSrcStep;  /* src / out, with frac  */
	uint32_t                 ulVsclBlkAvgSize;  /* vert block average size */
	uint32_t                 ulVsclInitPhase;  /* for frame or top-field display */
	uint32_t                 ulVsclInitPhaseBot;  /* for bot-field display */

	/* display orientation */
	BFMT_Orientation         eOutOrientation;    /* output Orientation, might not be useful*/
} BVDC_P_GfxFeederCfgInfo;

/*-------------------------------------------------------------------------
 * graphics feeder main context
 */
typedef struct BVDC_P_GfxFeederContext
{
	BDBG_OBJECT(BVDC_GFX)

	/* Gfx Feeder Id */
	BAVC_SourceId                    eId;
	uint32_t                         ulRegOffset;

	/* memory handle of current chip */
	BMEM_Heap_Handle                 hMemory;
	BREG_Handle                      hRegister;
	BRDC_Handle                      hRdc;
	BVDC_Window_Handle               hWindow;
	
	/* gfx surface manager */
	BVDC_P_GfxSurfaceContext         stGfxSurface;

#ifdef BVDC_P_SUPPORT_OLD_SET_ALPHA_SUR
	/* temporarily used for supporting BVDC_Source_SetAlphaSurface */
	BAVC_Gfx_Picture                 stTmpNewAvcPic;
	BAVC_Gfx_Picture                 stTmpIsrAvcPic;
#endif
	
	/* could be user set new sur, isr set new, or cur sur, 
	 * decided by Validate, used by ApplyChange */
	BVDC_P_SurfaceInfo              *pNewSur;
	
	/* gfx feeder private processing cfg activated by user with ApplyChange */
	BVDC_P_GfxFeederCfgInfo          stNewCfgInfo;
	BVDC_P_GfxFeederCfgInfo          stCurCfgInfo;

	/* record previous dirty in case RUL was not executed */
	BVDC_P_GfxDirtyBits              stPrevDirty;
	
	BVDC_P_CscCoeffs                 stGfxCsc;
	bool                             bSupportVertScl;
	
	uint32_t                         ulInitVsyncCntr;
	
	uint32_t                         ulOffsetPixInByte;   /* Num of offset pix for sub byte pixel format */
	uint32_t                         ulAlphaOffsetPixInByte;   /* Alpha Sur: Num of offset pix in a byte */
	uint32_t                         ulFirInitPhase;    /* scale: init phase */
	
	uint32_t                         ulVertLineBuf;    /* line buffer length of Vert scaler */
	uint32_t                         ulResetRegAddr;
	uint32_t                         ulResetMask;
	
} BVDC_P_GfxFeederContext;


/***************************************************************************
 * Gfx feeder private functions
 ***************************************************************************/

/***************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_Create
 *
 * Called by BVDC_P_Source_Create to create gfx feeder specific context
 * when BVDC_Handle is openned with BVDC_Open
 *
 * Note: assume parameter eSourceId are valid for gfx feeder
 */
BERR_Code BVDC_P_GfxFeeder_Create
	( BVDC_P_GfxFeeder_Handle         *phGfxFeeder,
	  BREG_Handle                      hRegister,
	  BRDC_Handle                      hRdc,
	  BAVC_SourceId                    eGfdId,
	  bool                             b3dSrc );

/*************************************************************************
 * {private}
 *
 * Called by BVDC_P_Source_Destroy to destroy gfx feeder specific context
 * when BVDC_Handle is closed with BVDC_Close
 */
BERR_Code BVDC_P_GfxFeeder_Destroy
	( BVDC_P_GfxFeeder_Handle          hGfxFeeder );

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_Init
 *
 * initialize stuff that will change after destory and re-create. It also
 * allocate surface address shadow registers. We don't want to allocate them
 * until the GFD is really going to be used.
 */
void BVDC_P_GfxFeeder_Init(
	BVDC_P_GfxFeeder_Handle          hGfxFeeder,
	BMEM_Heap_Handle                 hMemory );

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_GetAdjSclOutRect_isr
 *
 * Called by BVDC_P_Window_ApplyChanges to get the adjusted scal-out rect
 * as it SetSurfaceSize in compositor,
 * It should match the design of BVDC_P_GfxFeeder_ValidateSurAndRects
 */
BERR_Code BVDC_P_GfxFeeder_GetAdjSclOutRect_isr
	( const BVDC_P_ClipRect           *pClipRect,            /* in */
	  const BVDC_P_Rect               *pSclOutRect,          /* in */
	  const BVDC_P_Rect               *pDstRect,             /* in */
	  BVDC_P_Rect                     *pAdjSclOutRect );      /* out */

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_ValidateChanges
 *
 * To be called by BVDC_ApplyChanges, to check whether there is conflict
 * between settings related to gfx feeder.
 *
 */
BERR_Code BVDC_P_GfxFeeder_ValidateChanges
	( BVDC_P_GfxFeeder_Handle          hGfxFeeder,
	  BVDC_Source_PictureCallback_isr  pfPicCallbackFunc );

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_ApplyChanges_isr
 *
 * To be called by BVDC_ApplyChanges, to copy "new user state" to "current
 * state", after validation of all VDC modules passed.
 *
 */
BERR_Code BVDC_P_GfxFeeder_ApplyChanges_isr
	( BVDC_P_GfxFeeder_Handle     hGfxFeeder );

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_AbortChanges
 *
 * Cancel the user settings that either fail to validate or simply
 * because user wish to abort the changes in mid-way.
 */
void BVDC_P_GfxFeeder_AbortChanges
	( BVDC_P_GfxFeeder_Handle     hGfxFeeder );

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_BuildRul_isr
 *
 * Append GfxFeeder specific RULs into hList.
 *
 */
void BVDC_P_GfxFeeder_BuildRul_isr
	( BVDC_P_GfxFeeder_Handle          hGfxFeeder,
	  BVDC_P_Source_Info *             pCurSrcInfo,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId,
	  BVDC_P_State                     eVnetState );

/***************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_ValidateBlend
 *
 * Called by BVDC_Window_SetBlendFactor to validate the graphics window
 * blending factor setting
 */
BERR_Code BVDC_P_GfxFeeder_ValidateBlend
	( BVDC_BlendFactor             eSrcBlendFactor,
	  BVDC_BlendFactor             eDstBlendFactor,
	  uint8_t                      ucConstantAlpha );

/***************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_AdjustBlend_isr
 *
 * Called by BVDC_P_Window_SetBlender to adjust the blending factor
 * of a graphics window for HW register setting
 *
 * Note: peSrcBlendFactor, peDstBlendFactor, and pucConstantAlpha are both
 * input and output of this function, they must be filled with current
 * values before calling this function
 */
BERR_Code BVDC_P_GfxFeeder_AdjustBlend_isr
	( BVDC_BlendFactor            *peSrcBlendFactor,
	  BVDC_BlendFactor            *peDstBlendFactor,
	  uint8_t                     *pucConstantAlpha );

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_DecideColorMatrix_isr
 *
 * output: color matrix to convert from active pixel format to output
 *         color primary (main video window's color primary)
 *
 * Note: Because of gamma effect, of not knowing how user treated alpha
 * when the src gfx surface was created, and of diff between Bt601 and
 * Bt709 is not very noticable for gfx, we decide to use idendity matrix
 * to convert between Bt601 and Bt709 (i.e. not conv).
 */
BERR_Code BVDC_P_GfxFeeder_DecideColorMatrix_isr
	( BPXL_Format                  eActivePxlFmt,
	  BVDC_P_GfxFeeder_Handle      hGfxFeeder,
	  bool                         bConstantBlend,
	  BVDC_P_CscCoeffs            *paulClrMatrix,
	  const BVDC_P_CscCoeffs     **ppaulRGBToYCbCr,
	  const BVDC_P_CscCoeffs     **ppaulYCbCrToRGB );

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_DecideFilterCoeff_isr
 *
 * output: Hscl filter coeff
 */
BERR_Code BVDC_P_GfxFeeder_DecideFilterCoeff_isr
	( BVDC_FilterCoeffs     eCoeffs,
	  uint32_t              ulCtIndex,
	  uint32_t              ulSrcSize,
	  uint32_t              ulOutSize,
	  uint32_t **           paulCoeff );

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_DecideVsclFirCoeff_isr
 *
 * output: filter coeff
 *
 * Note: This implementation is originally copied from bgrc, we should watch
 * bgrc's update and update this code accordingly.
 */
BERR_Code BVDC_P_GfxFeeder_DecideVsclFirCoeff_isr
	( BVDC_FilterCoeffs     eCoeffs,
	  uint32_t              ulCtIndex,
	  uint32_t              ulSrcSize,
	  uint32_t              ulOutSize,
	  uint32_t **           paulCoeff );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_GFXFEEDER_PRIV_H__ */
/* End of file. */
