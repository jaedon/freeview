/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_hddvi_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/92 $
 * $brcm_Date: 11/23/11 11:31a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_hddvi_priv.h $
 * 
 * Hydra_Software_Devel/92   11/23/11 11:31a tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/91   8/12/11 2:01p pntruong
 * SW7231-318: Corrected number of vnet_f drains,  and added grpd/hddvi
 * support.
 *
 * Hydra_Software_Devel/90   10/6/10 1:05p pntruong
 * SW7425-31: Initial support for hddvi.
 *
 * Hydra_Software_Devel/89   7/30/10 11:11a yuxiaz
 * SW3548-2795: Reset HD_DVI for bridge error.
 *
 * Hydra_Software_Devel/88   4/7/10 11:31a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/87   4/5/10 4:07p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/86   3/25/09 5:54p pntruong
 * PR52524: Used the index determined by upstream block.
 *
 * Hydra_Software_Devel/85   3/9/09 1:28p pntruong
 * PR52524: Added support for hdmi rgb quantization of full range and
 * limited range.
 *
 * Hydra_Software_Devel/84   3/4/09 11:19a pntruong
 * PR52568: Customer not needed the custom 1340x725p@60hz.
 *
 * Hydra_Software_Devel/83   3/2/09 3:05p pntruong
 * PR52568: Added support for custom format 1340x725p@60hz.
 *
 * Hydra_Software_Devel/82   2/26/09 10:01p pntruong
 * PR52568: [GDVR] Invalid format at HDMI 1080i.  Added support for detect
 * famous devices.
 *
 * Hydra_Software_Devel/81   1/6/09 4:12p pntruong
 * PR50866: HDDVI/HDMI: Blinking after video is stable.
 *
 * Hydra_Software_Devel/80   12/3/08 7:54p pntruong
 * PR45817: Added hddvi support.
 *
 * Hydra_Software_Devel/79   12/1/08 8:53p pntruong
 * PR49725: 1024x768(MAC19'' SVGA, H:60.241, V74.927) can't be locked and
 * no source pending msg reported.
 *
 * Hydra_Software_Devel/78   9/22/08 1:36p pntruong
 * PR47126, PR46798: Make sure to build muxes on input color space change.
 * Use correct mask color for rgb/ycrcb.
 *
 * Hydra_Software_Devel/77   9/20/08 10:43a pntruong
 * PR47156: Improved detections of reduce blanking formats.
 *
 * Hydra_Software_Devel/76   8/5/08 9:22p pntruong
 * PR44362, PR44807: Better tuned bvb reset states.
 *
 * Hydra_Software_Devel/75   4/2/08 11:21a pntruong
 * PR40742: No input status change also trigger generic callback.
 *
 * Hydra_Software_Devel/74   2/29/08 4:18p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/73   2/22/08 2:32p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/72   2/14/08 6:06p pntruong
 * PR39321: Implement hardware changes for HD_DVI.
 *
 * Hydra_Software_Devel/71   1/14/08 2:36p yuxiaz
 * PR38528: Read PCTR error bits in RUL.
 *
 * Hydra_Software_Devel/70   10/26/07 5:46p pntruong
 * PR36469: Relax tolerance to pickup 480p@60Hz from Master Pattern
 * Generator, as other leading could also detect and display it.
 *
 * Hydra_Software_Devel/69   9/5/07 4:15p pntruong
 * PR34632: Backout previous changes for further testings.
 *
 * Hydra_Software_Devel/68   9/5/07 3:47p pntruong
 * PR34632: [HD_DVI] Incorrect reset bit got written.  Also reduce the
 * reset to one only, there is no need to reset 3.
 *
 * Hydra_Software_Devel/67   9/4/07 3:14p pntruong
 * PR34566: Delay framerate detection until video is locked.
 *
 * Hydra_Software_Devel/66   8/15/07 10:30a pntruong
 * PR33847:  Use top and bottom vertical blank to detect Marantz DVD
 * player field inversion issue.
 *
 * Hydra_Software_Devel/PR33847/1   8/15/07 9:17p honglang
 * PR33847: Denon - Marantz DV9600 Universal DVD Player HDMI has 480i
 * field inversion issue
 *
 * Hydra_Software_Devel/65   7/25/07 12:14p yuxiaz
 * PR32483: Flicker is appeared on the screen after the Samsung BD-P1000
 * player is turned to stand-by mode in HDMI input.
 *
 * Hydra_Software_Devel/64   6/26/07 5:37p pntruong
 * PR32402: TV display "unsupport format" and "no signal" OSD for
 * componet/hdmi input.
 *
 * Hydra_Software_Devel/63   6/26/07 3:48p yuxiaz
 * PR32495: Added YCbCr 30bit 422 double clock mode for HD_DVI.
 *
 * Hydra_Software_Devel/62   6/21/07 2:16p yuxiaz
 * PR32023: Added video format tolerance to HDDVI.
 *
 * Hydra_Software_Devel/61   6/15/07 1:52p yuxiaz
 * PR29940: Added tolerance when trying to lock HD_DVI format in non-auto
 * format mode.
 *
 * Hydra_Software_Devel/60   5/29/07 4:59p yuxiaz
 * PR30334, PR31640: Merge in changes from branch. Use pCurInfo-
 * >ulInputPort to configure HDDVI CHANNEL_CNTRL register MODESEL bit to
 * support extern daught card. Format detection sometimes failed because
 * of wrong pixel decimate value
 *
 * Hydra_Software_Devel/59   5/23/07 1:28p yuxiaz
 * PR29980: Added HD_DVI mux settings for different input color space.
 *
 * Hydra_Software_Devel/58   5/15/07 11:46a yuxiaz
 * PR29940: Add support to set and get Hstart and Vstart for HDDVI.
 *
 * Hydra_Software_Devel/57   4/17/07 4:35p pntruong
 * PR27209: Improved trigger swapped work-around.
 *
 * Hydra_Software_Devel/56   4/4/07 4:44p yuxiaz
 * PR 28391: Added DE settings for more formats.
 *
 * Hydra_Software_Devel/55   3/30/07 10:40a yuxiaz
 * PR28350, PR27180: Added BVDC_Source_GetInputStatus to get input status
 * for VDEC and HD_DVI.
 *
 * Hydra_Software_Devel/54   3/28/07 8:07p pntruong
 * PR28391: Conserved source dirty as it about running out.
 *
 * Hydra_Software_Devel/53   2/15/07 4:05p pntruong
 * PR25235: Need to use actual pixel frequency and vertical frequency
 * fields in BFMT instead of the MASK.
 *
 * Hydra_Software_Devel/52   2/7/07 5:34p yuxiaz
 * PR 27209: Added HD_DVI trigger mismatch workaround for 3563.
 *
 * Hydra_Software_Devel/51   2/2/07 7:38p pntruong
 * PR23482: Added VDEC/HDDVI Frame Rate Detection in VDC.  Also refactored
 * detection to use in both hddvi and vdec.  Added support for all
 * framerate code suported in bavc.h.
 *
 * Hydra_Software_Devel/50   2/1/07 1:53p pntruong
 * PR26677, PR23236: Enhanced autoformat detection, added 1080p support.
 *
 * Hydra_Software_Devel/49   1/22/07 7:51p pntruong
 * PR22579: Bringup of HD_DVI input (dual core).  Fixed colorspace.
 *
 * Hydra_Software_Devel/48   1/22/07 11:12a pntruong
 * PR22579: Bringup of HD_DVI input (dual core).  Redo hddvi code.
 *
 ***************************************************************************/
#ifndef BVDC_HDDVI_PRIV_H__
#define BVDC_HDDVI_PRIV_H__

#include "bvdc.h"
#include "bvdc_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_csc_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Software reset BCHP_HD_DVI_0_SW_RESET_CNTRL */
#define BVDC_P_HDDVI_SW_RESET            (1)

/***************************************************************************
 * Private defines
 ***************************************************************************/
/* Trigger offset from the picture height. */
#define BVDC_P_HDDVI_TRIGGER_OFFSET                 (6)


/* Wait this amount of vsync to get stable pixel decimate value. */
#define BVDC_P_HDDVI_PIXEL_DECIMATE_COUNT           (3)

/* Print out status in case source isn't detect every n second. */
#define BVDC_P_HDDVI_STATUS_MSG_THRESHOLD           (3 * 60)

/* Use to detect vertical refresh rate! */
#define BVDC_P_HDDVI_VERTFREQ_TOLER                 (50)
#define BVDC_P_HDDVI_DOUBLECLOCK_VERTFREQ_TOLER     (150)

/* Use to detect H scanline */
#define BVDC_P_HDDVI_SCANLINE_TOLER                 (16)

/* Use to detect format ! */
#define BVDC_P_HDDVI_FORMAT_TOLER_WIDTH             (2)
#define BVDC_P_HDDVI_FORMAT_TOLER_HEIGHT            (3)
#define BVDC_P_HDDVI_FORMAT_TOLER_MAX_WIDTH         (5)
#define BVDC_P_HDDVI_FORMAT_TOLER_MAX_HEIGHT        (80)

/* PR52568: special 1090i, 725p */
#define BVDC_P_HDDVI_1090i_MIN_TOLER_HEIGHT         (10)
#define BVDC_P_HDDVI_725p_MIN_TOLER_WIDTH           (60)
#define BVDC_P_HDDVI_725p_MIN_TOLER_HEIGHT          (5)

/* Number of vsync to detect trigger swap */
#define BVDC_P_HDDVI_DETECT_TRIGGER_SWAP            (15)

/* Threshold of lost video before going into auto format detection. */
#define BVDC_P_HDDVI_NEW_VER_0                      (0) /* 7038x-ish */
#define BVDC_P_HDDVI_NEW_VER_1                      (1) /* 3560Bx, 7400-ish */
#define BVDC_P_HDDVI_NEW_VER_2                      (2) /* 3563-Ax,Bx */
#define BVDC_P_HDDVI_NEW_VER_3                      (3) /* 3563-Cx */
#define BVDC_P_HDDVI_NEW_VER_4                      (4) /* 3548-Ax,Bx,Cx,  */
#define BVDC_P_HDDVI_NEW_VER_5                      (5) /* 7420Ax, mainly HW fixes */
#define BVDC_P_HDDVI_NEW_VER_6                      (6) /* 7420Bx/Cx, added hw mute */
#define BVDC_P_HDDVI_NEW_VER_7                      (7) /* 7422Ax/7425Ax, added sw_init */
#define BVDC_P_HDDVI_NEW_VER_8                      (8) /* 7425Bx/7231Bx, added I3D detection */
#define BVDC_P_HDDVI_NEW_VER_9                      (9) /* 7435Ax */

/* PR27209: Need HD_DVI trigger workaround*/
#if (BVDC_P_SUPPORT_HDDVI_VER == BVDC_P_HDDVI_NEW_VER_2)
#define BVFD_P_HDDVI_TRIGGER_WORKAROUND             (1)
#endif

/* PR33847: Marantz DVD player field inversion*/
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_3)
#define BVFD_P_HDDVI_FIELD_INVERSION_WORKAROUND     (1)
#endif

/***************************************************************************
 * Private macros
 ***************************************************************************/
/* Get the offset of two groups of register. */
#define BVDC_P_HDDVI_GET_REG_OFFSET(eHdDviId, ulPriReg, ulSecReg) \
	((BVDC_P_HdDviId_eHdDvi0 == (eHdDviId)) ? 0 : ((ulSecReg) - (ulPriReg)))

#define BVDC_P_HDDVI_INPUT_RGB(eInputColorSpace) \
	(BVDC_P_HdDvi_InputColorSpace_eRGB == eInputColorSpace)

#define BVDC_P_HDDVI_INPUT_444(eInputColorSpace) \
	(BVDC_P_HdDvi_InputColorSpace_eYCbCr444 == eInputColorSpace)

#define BVDC_P_HDDVI_INPUT_422(eInputColorSpace) \
	(BVDC_P_HdDvi_InputColorSpace_eYCbCr422 == eInputColorSpace)

#define BVDC_P_HDDVI_INPUT_422_DOUBLECLOCK(eInputColorSpace) \
	(BVDC_P_HdDvi_InputColorSpace_eYCbCr422_DoubleClock == eInputColorSpace)

/***************************************************************************
 * Private enums
 ***************************************************************************/
typedef enum
{
	BVDC_P_HdDviId_eHdDvi0 = 0,
	BVDC_P_HdDviId_eHdDvi1

} BVDC_P_HdDviId;

/* Input color space */
typedef enum
{
	BVDC_P_HdDvi_InputColorSpace_eRGB = 0,
	BVDC_P_HdDvi_InputColorSpace_eYCbCr444,
	BVDC_P_HdDvi_InputColorSpace_eYCbCr422,
	BVDC_P_HdDvi_InputColorSpace_eYCbCr422_DoubleClock

} BVDC_P_HdDvi_InputColorSpace;


typedef struct
{
	bool                           bInterlaced;   /* Raster type */
	bool                           bPllDetected;  /* No vsync count saturated */
	uint32_t                       ulPxlFreq;     /* Pixel frequency */
	uint32_t                       ulVertFreq;    /* Vertical refresh rate */
	uint32_t                       ulClkPerVsync; /* Sysclk per vsync */

	uint32_t                       ulVBlank;      /* Total Vert blanking */
	uint32_t                       ulHBlank;      /* Total Horz blanking */
	uint32_t                       ulAvWidth;     /* Active video width */
	uint32_t                       ulAvHeight;    /* Active video height */
	uint32_t                       ulVPolarity;   /* Vert polarity */
	uint32_t                       ulHPolarity;   /* Horz polarity */

	uint32_t                       ulHFrontPorch; /* Horz front porch */
	uint32_t                       ulHBackPorch;  /* Horz back porch */
	uint32_t                       ulVFrontPorch; /* Vert front porch */
	uint32_t                       ulVBackPorch;  /* Vert back porch */

	uint32_t                       ulBottomVFrontPorch;/* Bottom field Vert front porch */
	uint32_t                       ulBottomVBackPorch;/* Bottom field Vert back porch */
	uint32_t                       ulBottomVBlank; /* Total bottom field Vert blanking */
} BVDC_P_HdDviInput;

/* Internal De settings. */
typedef struct
{
	BFMT_VideoFmt                  eVideoFmt;
	uint32_t                       ulHorzDelay;
	uint32_t                       ulVertDelay;

} BVDC_P_HdDvi_DeConfig;

typedef struct BVDC_P_HdDviContext
{
	BDBG_OBJECT(BVDC_DVI)

	BREG_Handle                    hReg;
	BVDC_Source_Handle             hSource;

	BVDC_P_HdDviId                 eId;
	uint32_t                       ulOffset;

	/* Used by HD_DVI source to sync with XVD */
	BAVC_VDC_HdDvi_Picture         stXvdField;
	bool                           bVideoDetected;
	bool                           bResetFormatDetect;
	uint32_t                       ulClkPerVsyncDelta;
#if (BVDC_P_HDDVI_SW_RESET)
	bool                           bSwReset;
#endif

	/* Status detection */
	bool                           bBvbErr;
	bool                           bFifoErr;
	bool                           bPctrErr;
	bool                           bBridgeErr;
	uint32_t                       ulBridgeErrRegAddr; /* Scratch reg to hold bridge errors */
	uint32_t                       ulPctrErrRegAddr; /* Scratch reg to hold PCTR errors */
	uint32_t                       ulVsyncCnt;       /* Vsync counter */
	BVDC_P_HdDviInput              stStatus;

	/* Misc.  Configuration. */
	BVDC_P_HdDvi_InputColorSpace   eInputColorSpace;

	bool                           b24BitsMode;
	uint32_t                       ulPixelDecimate;
	BVDC_P_CscCoeffs               stCsc;

	uint32_t                       ulPixelDecimateCnt;       /* Pixel Decimate counter */

	/* DE enable signal */
	uint32_t                       ulVertDelay;
	uint32_t                       ulHorzDelay;

	/* Swapped trigger. */
	bool                           bReverseTrigger;
	uint32_t                       ulDetectTrig;

	/* Tolerance on detection of vertical rate */
	uint32_t                       ulVertFreqTolerance;
	uint32_t                       ulScanWidthTolerance;

	/* 444 to 422 down sampler setting */
	BVDC_444To422DnSampler         stDnSampler;
	/* 422 to 444 up sampler setting */
	BVDC_422To444UpSampler         stUpSampler;

	/* Dither */
	BVDC_P_DitherSetting           stDither;

	/* Picture clean cfg regs. */
	uint32_t                       ulPicCleanUpCfg1;

} BVDC_P_HdDviContext;

/***************************************************************************
 * Private function prototypes
 ***************************************************************************/
BERR_Code BVDC_P_HdDvi_ValidateChanges
	( BVDC_P_HdDvi_Handle              hHdDvi );

#if (BVDC_P_SUPPORT_HDDVI)
BERR_Code BVDC_P_HdDvi_Create
	( BVDC_P_HdDvi_Handle             *phHdDvi,
	  BVDC_P_HdDviId                   eHdDviId,
	  BREG_Handle                      hReg,
	  BVDC_Source_Handle               hSource );

void BVDC_P_HdDvi_Destroy
	( BVDC_P_HdDvi_Handle              hHdDvi );

void BVDC_P_HdDvi_Init
	( BVDC_P_HdDvi_Handle              hHdDvi );

void BVDC_P_HdDvi_GetStatus_isr
	( const BVDC_P_HdDvi_Handle        hHdDvi,
	  bool                            *pbVideoDetected );
void BVDC_P_HdDvi_GetInputStatus
	( BVDC_P_HdDviContext             *pHdDvi,
	  BVDC_Source_InputStatus         *pInputStatus );

void BVDC_P_HdDvi_Bringup_isr
	( BVDC_P_HdDvi_Handle              hHdDvi );
#else
#define BVDC_P_HdDvi_Create(phHdDvi, eHdDviId, hReg, hSource)    BDBG_ASSERT(0)
#define BVDC_P_HdDvi_Destroy(hHdDvi)                             BDBG_ASSERT(0)
#define BVDC_P_HdDvi_Init(hHdDvi)                                BDBG_ASSERT(0)
#define BVDC_P_HdDvi_GetStatus_isr(hHdDvi, pbVideoDetected)      BDBG_ASSERT(0)
#define BVDC_P_HdDvi_GetInputStatus(pHdDvi, pInputStatus)        BDBG_ASSERT(0)
#define BVDC_P_HdDvi_Bringup_isr(hHdDvi)                         BDBG_ASSERT(0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_HDDVI_PRIV_H__ */

/* End of file. */
