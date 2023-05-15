/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_pep_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/68 $
 * $brcm_Date: 8/13/12 2:31p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_pep_priv.h $
 * 
 * Hydra_Software_Devel/68   8/13/12 2:31p tdo
 * SW7425-3679: sync up TNT programming with the programming guide
 *
 * Hydra_Software_Devel/67   3/30/12 5:05p tdo
 * SW7346-760: Kernel oops during BVDC_P_Pep_BuildRul_isr.  Avoid read
 * HIST stats during vnet reconfig or comp reset
 *
 * Hydra_Software_Devel/66   2/16/11 5:14p tdo
 * SW7346-85, SW7422-51 : Software TN2TH Setting
 *
 * Hydra_Software_Devel/65   7/1/10 2:16p tdo
 * SW3548-2817, SW3548-2707: more fine tuning
 *
 * Hydra_Software_Devel/64   4/19/10 10:36p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/63   4/7/10 11:33a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/62   4/5/10 4:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/61   11/10/09 4:32p tdo
 * SW7400-2574: Adjust TNT based on input and output format
 *
 * Hydra_Software_Devel/60   10/12/09 4:58p tdo
 * SW3548-2536: Improve use of histogram width in Dyn Cont calculations
 *
 * Hydra_Software_Devel/59   9/1/09 5:22p tdo
 * SW7420-303: Cleanup unused parameters in contrast stretch structure
 *
 * Hydra_Software_Devel/58   8/3/09 8:39p tdo
 * PR57179: Update Dynamic Contrast to support Settop Box
 *
 * Hydra_Software_Devel/57   3/4/09 4:11p tdo
 * PR51106: Support table base dynamic contrast algorithm.  Change
 * precision of level threshold for histogram data.
 *
 * Hydra_Software_Devel/56   2/19/09 3:25p tdo
 * PR51106: Add Level Stat
 *
 * Hydra_Software_Devel/55   9/30/08 12:33p jessem
 * PR 46489: Removed declaration of BVDC_P_Tnt_BuildRul_isr and mvoed this
 * to bvdc_tnt_priv.h.
 *
 * Hydra_Software_Devel/54   9/29/08 6:37p pntruong
 * PR 46489: Rollbacked all the build errors introduced.  Todo: need to
 * properly factored out the code for tnt and tnt new.
 *
 * Hydra_Software_Devel/52   9/25/08 2:26p tdo
 * PR46529: Implement a consistent delay for histogram data for the
 * dynamic contrast algorithm
 *
 * Hydra_Software_Devel/51   9/17/08 8:44p pntruong
 * PR47013: B0: Initial vdc support.
 *
 * Hydra_Software_Devel/50   9/17/08 1:48p tdo
 * PR46948: Add Backlight Dimming in Dynamic Contrast code
 *
 * Hydra_Software_Devel/49   8/28/08 4:11p tdo
 * PR45896: Add new dynamic back-light callback
 *
 * Hydra_Software_Devel/48   7/30/08 11:36a tdo
 * PR45181: Implement New Dynamic Contrast Algorithm
 *
 * Hydra_Software_Devel/47   6/11/08 11:45p tdo
 * PR39331: Bringup dynamic contrast for 3548/3556
 *
 * Hydra_Software_Devel/46   5/8/08 6:09p tdo
 * PR39331: Add LAB table control and demo mode.  Also, force LAB table to
 * be loaded in one vsync with the enable to avoid using uninitialized
 * LAB table.
 *
 * Hydra_Software_Devel/45   4/2/08 4:32p tdo
 * PR39331: Implement LAB changes for 3548
 *
 * Hydra_Software_Devel/44   3/19/08 5:45p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/43   3/4/08 2:37p rpan
 * PR40144: Moved TNT ring suppression enabling to another location.
 *
 * Hydra_Software_Devel/42   3/4/08 11:34a rpan
 * PR40144: Always enable TNT ring suppression if chip supports it.
 *
 * Hydra_Software_Devel/41   2/14/08 4:32p tdo
 * PR39331, PR39332: Regroup PEP block to add support for 3548
 *
 * Hydra_Software_Devel/40   2/13/08 3:10p pntruong
 * PR39421: Removed unused code to improve readability.
 *
 * Hydra_Software_Devel/39   2/8/08 4:48p tdo
 * PR39420: use uint16_t instead of uint32_t to reduce
 * bvdc_pepcmstable_priv module size
 *
 * Hydra_Software_Devel/38   1/23/08 11:14a tdo
 * PR38710: Rename blue stretch and green stretch to blue boost and green
 * boost to match their functionality
 *
 * Hydra_Software_Devel/37   12/5/07 9:07p tdo
 * PR37550: Fix STACK_USE coverity issue
 *
 * Hydra_Software_Devel/36   11/19/07 3:50p tdo
 * PR36898: Add VDC PI support for 7335
 *
 * Hydra_Software_Devel/35   11/19/07 1:59p tdo
 * PR37046: changing the large memory block allocation method from stack
 * to heap
 *
 * Hydra_Software_Devel/34   10/29/07 2:21p yuxiaz
 * PR34523: Added PEP support on 7325.
 *
 * Hydra_Software_Devel/33   10/8/07 2:49p pntruong
 * PR34855: Initial VDC bringup.
 *
 * Hydra_Software_Devel/32   7/10/07 6:11p tdo
 * PR32853: Extending the range of CMS saturation and hue gains to match
 * BBS3.0 tool
 *
 * Hydra_Software_Devel/31   6/11/07 1:08p tdo
 * PR30613: Modify dynamic contrast stretch prototype to meet customer
 * requirement
 *
 * Hydra_Software_Devel/30   6/7/07 2:36p tdo
 * PR29965: Alternating CAB and LAB table programming using RUL in every
 * other vsync
 *
 * Hydra_Software_Devel/29   6/6/07 11:38a tdo
 * PR28978: Fix range for hue gain
 *
 * Hydra_Software_Devel/28   5/31/07 3:27p tdo
 * PR28978: C0: Provide API to realize CMS function
 *
 * Hydra_Software_Devel/27   5/25/07 2:12p tdo
 * PR30613: Move dynamic contrast stretch into its own file
 *
 * Hydra_Software_Devel/26   4/19/07 2:32p tdo
 * PR29965: PEP cleanup
 *
 * Hydra_Software_Devel/25   4/5/07 6:13p tdo
 * PR25462: add support for luma average reporting in the PEP
 *
 * Hydra_Software_Devel/24   4/5/07 2:03p pntruong
 * PR28538, PR28539, PR28540, PR25462: Average pixel luma and customized
 * deinterlacer support.
 *
 * Hydra_Software_Devel/23   3/28/07 3:29p tdo
 * PR 28403: Provide API to customize TNT for sharpness
 *
 * Hydra_Software_Devel/22   2/20/07 6:11p tdo
 * PR 27576: Cleanup PEP related code.  Init PEP context with window init.
 * Move TNT context inside of PEP context.
 *
 * Hydra_Software_Devel/21   1/18/07 8:38p tdo
 * PR 23212:  bringup PEP algorithm.  Force rebuild PEP RUL when
 * compositor reset
 *
 * Hydra_Software_Devel/20   12/18/06 11:36p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 10:22p tdo
 * PR 22574: Move sharpness calculation to user context to optimize
 * TAB/TNT isr time
 *
 * Hydra_Software_Devel/19   9/12/06 5:30p pntruong
 * PR 17994, PR 23213, PR 23383: Hush warnings.
 *
 * Hydra_Software_Devel/18   9/7/06 6:47p pntruong
 * PR23744: Update RDB with MAD_0, DNR_0, and TAB_0.
 *
 * Hydra_Software_Devel/17   8/30/06 5:59p tdo
 * PR 17994, PR 23213, PR 23383:  Implement demo mode support for the PEP.
 *
 * Hydra_Software_Devel/16   8/28/06 3:40p tdo
 * PR 22574: Remove BVDC_P_SUPPORT_TAB and BVDC_P_SUPPORT_TNT
 *
 * Hydra_Software_Devel/15   8/24/06 1:58p tdo
 * PR 22574: Add support for TNT block.  Add support for
 * BVDC_P_SUPPORT_TAB and BVDC_P_SUPPORT_TNT macro
 *
 * Hydra_Software_Devel/14   8/14/06 1:53p tdo
 * PR 23456: Moving large array of local variables into handle context
 *
 * Hydra_Software_Devel/13   6/15/06 5:26p tdo
 * PR 21535: Remove warning for 7401 B0 build
 *
 * Hydra_Software_Devel/12   6/14/06 1:42p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/11   6/13/06 4:33p tdo
 * PR 21535: Create a context for PEP block to store static private data
 *
 * Hydra_Software_Devel/10   5/26/06 3:42p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/9   2/2/06 5:29p tdo
 * PR 16061: Changing gain value used in contrast stretch calculation to a
 * fixed point representation
 *
 * Hydra_Software_Devel/8   1/31/06 5:21p pntruong
 * PR17778: Take in changes for new hddvi.
 *
 * Hydra_Software_Devel/4   1/12/06 9:58a tdo
 * PR 16061: Implementing dynamic contrast stretch in the PEP
 *
 * Hydra_Software_Devel/3   6/28/05 11:03a jasonh
 * PR 16012: Adding support for 7401
 *
 * Hydra_Software_Devel/2   8/12/04 1:49p pntruong
 * PR12272: TAB is not enabled the first time when bring up analog.
 *
 * Hydra_Software_Devel/1   7/30/04 12:50p tdo
 * PR11971: add file bvdc_pep_priv.h
 *
 ***************************************************************************/

#ifndef BVDC_PEP_PRIV_H__
#define BVDC_PEP_PRIV_H__

#include "bstd.h"
#include "bvdc.h"
#include "bdbg.h"
#include "bvdc_hist_priv.h"
#include "bvdc_window_priv.h"

#define BVDC_P_SUPPORT_PEP_VER_0                             (0)
#define BVDC_P_SUPPORT_PEP_VER_1                             (1)
#define BVDC_P_SUPPORT_PEP_VER_2                             (2)
#define BVDC_P_SUPPORT_PEP_VER_3                             (3)
#define BVDC_P_SUPPORT_PEP_VER_4                             (4)

#if (BVDC_P_SUPPORT_PEP)
#if (BVDC_P_SUPPORT_PEP_VER <= BVDC_P_SUPPORT_PEP_VER_3)
#include "bchp_pep_cmp_0_v0.h"
#else
#include "bchp_vivid_cmp_0.h"
#endif
#endif

#if(BVDC_P_SUPPORT_HIST_VER >= BVDC_P_SUPPORT_HIST_VER_2)
#include "bchp_hist.h"
#endif

#if (BVDC_P_SUPPORT_TAB)
#include "bchp_tab_0.h"
#endif
#if (BVDC_P_SUPPORT_TNT)
#include "bchp_tnt_cmp_0_v0.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BVDC_HST);

#if (BVDC_P_SUPPORT_PEP)
#if (BVDC_P_SUPPORT_PEP_VER >= BVDC_P_SUPPORT_PEP_VER_4)
#define BVDC_P_CAB_TABLE_SIZE   (BCHP_VIVID_CMP_0_CAB_LUT_DATA_i_ARRAY_END + 1)
#define BVDC_P_LAB_TABLE_SIZE   (BCHP_VIVID_CMP_0_LAB_LUT_DATA_i_ARRAY_END + 1)
#else
#define BVDC_P_CAB_TABLE_SIZE   (BCHP_PEP_CMP_0_V0_CAB_LUT_DATA_i_ARRAY_END + 1)
#define BVDC_P_LAB_TABLE_SIZE   (BCHP_PEP_CMP_0_V0_LAB_LUT_DATA_i_ARRAY_END + 1)
#endif
#else
#define BVDC_P_CAB_TABLE_SIZE   1  /* hush warnings */
#define BVDC_P_LAB_TABLE_SIZE   1  /* hush warnings */
#endif

#if (BVDC_P_SUPPORT_HIST)
#if (BVDC_P_SUPPORT_HIST_VER >= BVDC_P_SUPPORT_HIST_VER_2)
#define BVDC_P_HISTO_TABLE_SIZE (BCHP_HIST_RD_BINi_ARRAY_END + 1)
#else
#define BVDC_P_HISTO_TABLE_SIZE (BCHP_PEP_CMP_0_V0_HISTO_DATA_COUNT_i_ARRAY_END + 1)
#endif
#else
#define BVDC_P_HISTO_TABLE_SIZE 1  /* hush warnings */
#endif

#define BVDC_P_PEP_FIX_FRACTIONAL_SHIFT         20

/* values in 10 bits */
#define BVDC_P_PEP_BLACK_LUMA_VALUE             64
#define BVDC_P_PEP_WHITE_LUMA_VALUE             940
#define BVDC_P_PEP_MAX_LUMA_VALUE               1023

/* This defines the number of points originally calculated for the LAB in the Dyn Cont code */
/* Later expanded to the number of entries in the actual hardware LAB table */
#define BVDC_P_PEP_LAB_GEN_SIZE                 64

#define BVDC_P_PEP_MAX_CAB_SETTING_GRANUALITY   4
#define BVDC_P_PEP_CMS_SAT_MIN_RANGE           -140
#define BVDC_P_PEP_CMS_SAT_MAX_RANGE            140
#define BVDC_P_PEP_CMS_HUE_MIN_RANGE           -50
#define BVDC_P_PEP_CMS_HUE_MAX_RANGE            50
#define BVDC_P_PEP_CMS_COLOR_REGION_NUM         6

#define BVDC_P_PEP_ITOFIX(x) \
	(int32_t)((x) << BVDC_P_PEP_FIX_FRACTIONAL_SHIFT)

#define BVDC_P_PEP_CMS_IS_ENABLE(sat, hue) \
	(((sat)->lGreen   != 0 ) || \
	 ((sat)->lYellow  != 0 ) || \
	 ((sat)->lRed     != 0 ) || \
	 ((sat)->lMagenta != 0 ) || \
	 ((sat)->lBlue    != 0 ) || \
	 ((sat)->lCyan    != 0 ) || \
	 ((hue)->lGreen   != 0 ) || \
	 ((hue)->lYellow  != 0 ) || \
	 ((hue)->lRed     != 0 ) || \
	 ((hue)->lMagenta != 0 ) || \
	 ((hue)->lBlue    != 0 ) || \
	 ((hue)->lCyan    != 0 ))

#define BVDC_P_PEP_CMS_DISABLE(colorBar) \
	{(colorBar)->lGreen   = 0;  \
	 (colorBar)->lYellow  = 0;  \
	 (colorBar)->lRed     = 0;  \
	 (colorBar)->lMagenta = 0;  \
	 (colorBar)->lBlue    = 0;  \
	 (colorBar)->lCyan    = 0;}

#define BVDC_P_PEP_CMS_COMPARE_EQ(src, dst) \
	(((src)->lGreen   == (dst)->lGreen  ) && \
	 ((src)->lYellow  == (dst)->lYellow ) && \
	 ((src)->lRed     == (dst)->lRed    ) && \
	 ((src)->lMagenta == (dst)->lMagenta) && \
	 ((src)->lBlue    == (dst)->lBlue   ) && \
	 ((src)->lCyan    == (dst)->lCyan   ))

#define BVDC_P_PEP_CMS_SAT_WITHIN_RANGE(sat) \
	(((sat)->lGreen   >= BVDC_P_PEP_CMS_SAT_MIN_RANGE) && ((sat)->lGreen   <= BVDC_P_PEP_CMS_SAT_MAX_RANGE) && \
	 ((sat)->lYellow  >= BVDC_P_PEP_CMS_SAT_MIN_RANGE) && ((sat)->lYellow  <= BVDC_P_PEP_CMS_SAT_MAX_RANGE) && \
	 ((sat)->lRed     >= BVDC_P_PEP_CMS_SAT_MIN_RANGE) && ((sat)->lRed     <= BVDC_P_PEP_CMS_SAT_MAX_RANGE) && \
	 ((sat)->lMagenta >= BVDC_P_PEP_CMS_SAT_MIN_RANGE) && ((sat)->lMagenta <= BVDC_P_PEP_CMS_SAT_MAX_RANGE) && \
	 ((sat)->lBlue    >= BVDC_P_PEP_CMS_SAT_MIN_RANGE) && ((sat)->lBlue    <= BVDC_P_PEP_CMS_SAT_MAX_RANGE) && \
	 ((sat)->lCyan    >= BVDC_P_PEP_CMS_SAT_MIN_RANGE) && ((sat)->lCyan    <= BVDC_P_PEP_CMS_SAT_MAX_RANGE))

#define BVDC_P_PEP_CMS_HUE_WITHIN_RANGE(hue) \
	(((hue)->lGreen   >= BVDC_P_PEP_CMS_HUE_MIN_RANGE) && ((hue)->lGreen   <= BVDC_P_PEP_CMS_HUE_MAX_RANGE) && \
	 ((hue)->lYellow  >= BVDC_P_PEP_CMS_HUE_MIN_RANGE) && ((hue)->lYellow  <= BVDC_P_PEP_CMS_HUE_MAX_RANGE) && \
	 ((hue)->lRed     >= BVDC_P_PEP_CMS_HUE_MIN_RANGE) && ((hue)->lRed     <= BVDC_P_PEP_CMS_HUE_MAX_RANGE) && \
	 ((hue)->lMagenta >= BVDC_P_PEP_CMS_HUE_MIN_RANGE) && ((hue)->lMagenta <= BVDC_P_PEP_CMS_HUE_MAX_RANGE) && \
	 ((hue)->lBlue    >= BVDC_P_PEP_CMS_HUE_MIN_RANGE) && ((hue)->lBlue    <= BVDC_P_PEP_CMS_HUE_MAX_RANGE) && \
	 ((hue)->lCyan    >= BVDC_P_PEP_CMS_HUE_MIN_RANGE) && ((hue)->lCyan    <= BVDC_P_PEP_CMS_HUE_MAX_RANGE))

/***************************************************************************
 * PEP private data structures
 ***************************************************************************/
typedef struct BVDC_P_PepContext
{
	BDBG_OBJECT(BVDC_PEP)

	/* private fields. */
	BVDC_P_WindowId                eId;
	BREG_Handle                    hReg;
	bool                           bInitial;
	BVDC_Backlight_CallbackData    stCallbackData;

	bool                           bLoadCabTable;
	bool                           bLoadLabTable;
	bool                           bProcessCab;
	bool                           bProcessHist;
	bool                           bLabTableValid;
	bool                           bLabCtrlPending;

	/* These variables are used in contrast stretch algorithm */
	uint32_t                       aulLastBin[BVDC_P_HISTO_TABLE_SIZE];
	uint32_t                       aulLastLastBin[BVDC_P_HISTO_TABLE_SIZE];
	uint32_t                       aulBin[BVDC_P_HISTO_TABLE_SIZE];

	/* 24.8 notation */
	int32_t                        lFixLastMin;
	int32_t                        lFixLastMax;
	int32_t                        lFixLastMid;
	int32_t                        lFixBrtCur;
	int32_t                        lFixBrtLast;

	/* 16.16 notation */
	int32_t                        lFixEstLuma[BVDC_P_LAB_TABLE_SIZE];
	int32_t                        lFixHist_out[BVDC_P_PEP_LAB_GEN_SIZE];

	int32_t                        alDCTableTemp[BVDC_DC_TABLE_ROWS * BVDC_DC_TABLE_COLS];

	/* These are the output of dynamic contrast stretch algorithm */
	uint32_t                       aulLabTable[BVDC_P_LAB_TABLE_SIZE];

	/* Histogram min and max value */
	uint32_t                       ulHistSize;
	BVDC_LumaStatus                stHistoData;
	/* This is a temp histo data read every vsync */
	BVDC_LumaStatus                stTmpHistoData;
	uint32_t                       ulAvgAPL;
	uint32_t                       ulFiltAPL;
	uint32_t                       ulAvgLevelStats[BVDC_LUMA_HISTOGRAM_LEVELS];
	int32_t                        lLastGain;

	/* sharpness chroma gain, changed by source */
	uint32_t                       ulLumaChromaGain;

	/* These variables are used in CMS algorithm */
	int32_t                        alSatGain[BVDC_P_PEP_CMS_COLOR_REGION_NUM];
	int32_t                        alHueGain[BVDC_P_PEP_CMS_COLOR_REGION_NUM];
	int32_t                        alCr[BVDC_P_CAB_TABLE_SIZE];
	int32_t                        alCb[BVDC_P_CAB_TABLE_SIZE];
	int32_t                        tempCr[BVDC_P_CAB_TABLE_SIZE];
	int32_t                        tempCb[BVDC_P_CAB_TABLE_SIZE];

} BVDC_P_PepContext;


/***************************************************************************
 * PEP private functions
 ***************************************************************************/
BERR_Code BVDC_P_Pep_Create
	( BVDC_P_Pep_Handle           *phPep,
	  const BVDC_P_WindowId        eWinId,
	  const BREG_Handle            hReg );

void BVDC_P_Pep_Destroy
	( BVDC_P_Pep_Handle            hPep );

void BVDC_P_Pep_Init
	( const BVDC_P_Pep_Handle      hPep );

void BVDC_P_Pep_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList,
	  bool                         bInitial );

void BVDC_P_Tab_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList );

void BVDC_P_Pep_DynamicContrast_isr
	( const BVDC_ContrastStretch  *pCS,
	  BVDC_P_PepContext           *pPep,
	  int16_t                     *psSaturation,
	  int16_t                     *psBrightness,
	  int16_t                     *psContrast,
	  uint32_t                    *pulLabTable );

BERR_Code BVDC_P_Pep_ComposeCabTable
	( const uint32_t               ulFleshtone,
	  const uint32_t               ulGreenBoost,
	  const uint32_t               ulBlueBoost,
	  uint32_t                    *pulCabTable );

void BVDC_P_Pep_Cms
	( BVDC_P_PepContext           *pPep,
	  const BVDC_ColorBar         *pSatGain,
	  const BVDC_ColorBar         *pHueGain,
	  bool                         bIsHd,
	  uint32_t                    *pulCabTable );

void BVDC_P_Sharpness_Calculate_Peak_Values
	( const int16_t                sSharpness,
	  uint32_t                    *ulPeakSetting,
	  uint32_t                    *ulPeakScale );

void BVDC_P_Sharpness_Calculate_Gain_Value
	( const int16_t                sSharpness,
	  const int16_t                sMinGain,
	  const int16_t                sCenterGain,
	  const int16_t                sMaxGain,
	  uint32_t                    *ulSharpnessGain );

void BVDC_P_Pep_GetLumaStatus
	( const BVDC_Window_Handle     hWindow,
	  BVDC_LumaStatus             *pLumaStatus );

void BVDC_P_Pep_GetRadialTable
	( uint32_t                     ulColorId,
	  bool                         bIsHd,
	  const uint16_t             **ppRadialTable );

void BVDC_P_Pep_GetAngleTable
	( uint32_t                     ulColorId,
	  bool                         bIsHd,
	  const uint16_t             **ppAngleTable );

void BVDC_P_Pep_GetHueAngleTable
	( uint32_t                     ulColorId,
	  bool                         bIsHd,
	  const uint16_t             **ppHueAngleTable );

#if(BVDC_P_SUPPORT_HIST && BVDC_P_SUPPORT_HIST_VER == BVDC_P_SUPPORT_HIST_VER_1)
void BVDC_P_Histo_UpdateHistoData_isr
	( BVDC_P_Pep_Handle            hPep );
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_PEP_PRIV_H__ */
/* End of File */

