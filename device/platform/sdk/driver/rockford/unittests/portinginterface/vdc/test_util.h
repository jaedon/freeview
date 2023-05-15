/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: test_util.h $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 10/11/12 12:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/test_util.h $
 * 
 * Hydra_Software_Devel/18   10/11/12 12:09p syang
 * SW7346-572: add long-time-lock for vdc_test
 * 
 * Hydra_Software_Devel/17   9/27/12 2:10p vanessah
 * SW7425-2501: fix compile problem
 * 
 * Hydra_Software_Devel/16   9/27/12 2:03p vanessah
 * SW7425-2501: change some util macro
 * 
 * Hydra_Software_Devel/15   7/4/12 11:13a vanessah
 * SW7425-2501: clean up vdc
 * 
 * Hydra_Software_Devel/14   1/20/12 6:41p vanessah
 * SW7435-9: add STG2/3 support
 * 
 * Hydra_Software_Devel/13   7/18/11 11:30a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 * 
 * Hydra_Software_Devel/12   1/21/11 7:57a tdo
 * SW7425-50: Fix compiling error
 *
 * Hydra_Software_Devel/11   1/20/11 5:47p vanessah
 * SW7425-50:  STG custom display
 *
 * Hydra_Software_Devel/10   11/11/10 10:06a yuxiaz
 * SW7422-77: Fixed DAC settings for 3D formats.
 *
 * Hydra_Software_Devel/9   5/6/09 3:32p rpan
 * PR53106: Added 720p25/30 into supported HD formats.
 *
 * Hydra_Software_Devel/8   12/19/08 9:57a tdo
 * PR50391: define BFMT_VideoFmt_e1080p as HD
 *
 * Hydra_Software_Devel/7   11/13/08 5:39p darnstein
 * PR48884: microcode dump command "udmp" is ready for evaluation.
 *
 * Hydra_Software_Devel/6   9/30/08 5:03p darnstein
 * PR44592: Add 720P@24Hz to list of "IS_HD" formats.
 *
 * Hydra_Software_Devel/5   10/16/07 2:46p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/17/07 4:36p rpan
 * PR34857: Initial revision. Ported from xvd_vdc_test.
 *
 *
 ***************************************************************************/

#ifndef TEST_UTIL_H__
#define TEST_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bstd.h"           /* standard types */
#include "bavc.h"
#include "config.h"
#include "bkni_multi.h"
#include "commands.h"

#define IS_HD(fmt) (\
	(fmt == BFMT_VideoFmt_e480p)               || \
	(fmt == BFMT_VideoFmt_e576p_50Hz)          || \
	(fmt == BFMT_VideoFmt_e720p)               || \
	(fmt == BFMT_VideoFmt_e720p_24Hz)          || \
	(fmt == BFMT_VideoFmt_e720p_25Hz)          || \
	(fmt == BFMT_VideoFmt_e720p_30Hz)          || \
	(fmt == BFMT_VideoFmt_e720p_50Hz)          || \
	(fmt == BFMT_VideoFmt_e1080i)              || \
	(fmt == BFMT_VideoFmt_e1080i_50Hz)         || \
	(fmt == BFMT_VideoFmt_e1250i_50Hz)         || \
	(fmt == BFMT_VideoFmt_e1080p)              || \
	(fmt == BFMT_VideoFmt_e1080p_24Hz)         || \
	(fmt == BFMT_VideoFmt_e1080p_25Hz)         || \
	(fmt == BFMT_VideoFmt_e1080p_30Hz)         || \
	(fmt == BFMT_VideoFmt_e1080p_50Hz)         || \
	(fmt == BFMT_VideoFmt_e720p_60Hz_3DOU_AS)  || \
	(fmt == BFMT_VideoFmt_e720p_50Hz_3DOU_AS)  || \
	(fmt == BFMT_VideoFmt_e1080p_24Hz_3DOU_AS) || \
	(fmt == BFMT_VideoFmt_e1080p_30Hz_3DOU_AS) || \
	(fmt == BFMT_VideoFmt_eCUSTOM_1366x768p)   || \
	(fmt == BFMT_VideoFmt_eDVI_1600x1200p_60Hz)|| \
	(fmt == BFMT_VideoFmt_eCustom2)               \
)


#define TestError(e, str)	{\
	err = e;\
	if (err != BERR_SUCCESS)\
	{\
	BDBG_ERR(( str". %s: %d", __FILE__, __LINE__ ));\
		goto Done;\
	}\
}

#define TestFail(e, str)	{\
	eStatus = e;\
	if (eStatus == BERR_SUCCESS)\
	{\
	BDBG_ERR(( str". %s: %d", __FILE__, __LINE__ ));\
		goto Done;\
	}\
}

#define TEST_CLOSE_HANDLE(func, h) { \
	if(h) { \
		func(h); \
		h = NULL;\
	}\
}/* close a handle */


/* Get the min / max of two numbers */
#define BTST_P_MIN(a, b)                    (((a) < (b)) ? (a) : (b))
#define BTST_P_MAX(a, b)                    (((a) > (b)) ? (a) : (b))
#define BSTT_P_CLP(lo, hi, val)             \
	BTST_P_MIN(BTST_P_MIN(hi, val), BTST_P_MAX(lo, val))

#define BTST_P_USED_DIGTRIG(eMasterTg)            \
		((eMasterTg == BVDC_DisplayTg_eStg0)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg1)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg2)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg3)   || \
		 (eMasterTg == BVDC_DisplayTg_eDviDtg) || \
		 (eMasterTg == BVDC_DisplayTg_e656Dtg))

/* STG trigger transcoding path*/
#define BTST_P_USED_STG(eMasterTg)    \
		((eMasterTg == BVDC_DisplayTg_eStg0)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg1)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg2)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg3))

/* SRC classification */
#define BTST_P_IS_MPEG_SRC(eSrc)         \
	(eSrc <= BAVC_SourceId_eMpegMax)

#define BTST_P_IS_GFX_SRC(eSrc)         \
	((eSrc >= BAVC_SourceId_eGfx0)   && (eSrc <= BAVC_SourceId_eGfxMax))

#define BTST_P_IS_VFD_SRC(eSrc)         \
	((eSrc >= BAVC_SourceId_eVfd0)   && (eSrc <= BAVC_SourceId_eVfdMax))

#define BTST_P_IS_VDEC_SRC(eSrc)        \
	((eSrc == BAVC_SourceId_eVdec0)  || \
	 (eSrc == BAVC_SourceId_eVdec1))


#define BTST_P_IS_656_SRC(eSrc)            \
		((eSrc == BAVC_SourceId_e656In0)|| \
		 (eSrc == BAVC_SourceId_e656In1))


#define BTST_P_IS_HDDVI_SRC(eSrc)      \
	((eSrc == BAVC_SourceId_eHdDvi0)|| \
	 (eSrc == BAVC_SourceId_eHdDvi1))

#define BTST_P_IS_DS_SRC(eSrc)         \
		((eSrc == BAVC_SourceId_eDs0))


/***********************************************************************
 * Utility functions used in vdc_test
 */
#define BTST_P_ApplyChanges(c) \
        BTST_P_ApplyChanges_tagged((c), __FILE__, __LINE__)
BERR_Code BTST_P_ApplyChanges_tagged( BTST_P_Context  *pContext, const char *fileName, unsigned uiLine );

#define BTST_P_AcquireLongTimeLock(l) \
	BTST_P_AcquireLongTimeLock_tagged((l), __FILE__, __LINE__)
void BTST_P_AcquireLongTimeLock_tagged( BTST_P_LongTimeLock  *pLongTimeLock, const char *fileName, unsigned uiLine );
void BTST_P_ReleaseLongTimeLock( BTST_P_LongTimeLock  *pLongTimeLock);
BERR_Code BTST_P_InitLongTimeLock( BTST_P_LongTimeLock  *pLongTimeLock, unsigned uiMaxWaitSec);
void BTST_P_UninitLongTimeLock( BTST_P_LongTimeLock  *pLongTimeLock);
	
	
/***********************************************************************
 * Pan scan vector generator module:
 */
typedef struct TEST_PanVecGen    *TEST_PanVecGen_Handle;

int TEST_PanVecGen_Create( TEST_PanVecGen_Handle *phPanVecGen,
						   uint32_t ulSrcWidth,
						   uint32_t ulSrcHeight,
						   uint32_t ulDspWidth,
						   uint32_t ulDspHeight );

void TEST_PanVecGen_GetPanVector(TEST_PanVecGen_Handle hPanVecGen,
								 BAVC_Polarity eFldPlrt,
								 int16_t * piPanTop, int16_t * piPanLeft);

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTIL_H__ */

/* end of file */
