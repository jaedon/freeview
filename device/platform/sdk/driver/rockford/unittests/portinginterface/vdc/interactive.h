/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: interactive.h $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 9/26/12 3:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/interactive.h $
 * 
 * Hydra_Software_Devel/15   9/26/12 3:23p nilesh
 * SW7425-2568: Added TSM mode support for playback decode
 * 
 * Hydra_Software_Devel/14   8/17/12 4:09p vanessah
 * SW7425-2501: further cleanup xport file
 * 
 * Hydra_Software_Devel/13   4/27/11 4:15p tdo
 * SW7231-133, SW7422-417: Add support for multiple channel decode within
 * a decoder with streamer input and playback  Add option for users to
 * manually force SVD to only decode AVC, this is needed to do multi
 * channel decode with SVD due to memory allocation issue.  Cleanup old
 * code.
 *
 * Hydra_Software_Devel/12   4/15/11 1:53p tdo
 * SW7422-417: Add MVC/SVC support to vdc_test
 *
 * Hydra_Software_Devel/11   11/11/10 6:44p pntruong
 * SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
 * stb chips.  Removed dead codes.
 *
 * Hydra_Software_Devel/10   12/4/09 3:57p tdo
 * SW7408-23: Add playback feature for digital source
 *
 * Hydra_Software_Devel/9   5/21/09 3:13p tdo
 * PR54768: Fully support dual decode with same PID
 *
 * Hydra_Software_Devel/8   7/28/08 9:48a rpan
 * PR36501: Save the work for parsing PSI messages.
 *
 * Hydra_Software_Devel/7   7/18/08 3:22p rpan
 * PR36501: Hook up tuner and demodulator with xport.
 *
 * Hydra_Software_Devel/6   7/14/08 1:35p rpan
 * PR36501: Check-in point for tuner work. Tuner input is not working yet.
 *
 * Hydra_Software_Devel/5   10/16/07 2:45p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/21/07 1:55p rpan
 * PR34857: Initial revision.
 *
 *
 ***************************************************************************/

#ifndef VDC_TEST_INTERACTIVE_H__
#define VDC_TEST_INTERACTIVE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "framework.h"
#include "framework_board.h"
#include "bavc_xpt.h"

#include "commands.h"

/***************************************************************************
 *
 * Have hReg, hChp, hMem, and hInt opened and this BTST_Interactive test
 * will create and close all necessary handles to run vdc tests.
 */
void BTST_Interactive
	( const BFramework_Info   *pFrameworkInfo,
	  const BSystem_Info      *pSysInfo,
	  bool                     bSharedMem,
	  unsigned long            ulSharedMemSize,
	  bool                     bIntercept,
	  bool                     b422 );

/***************************************************************************
 *
 * Transport function prototypes
 *
 */
BERR_Code InitXPT(
	const BFramework_Info    *pFrameworkInfo
	);

BERR_Code StartXPT
(
	BTST_P_Context            *pContext,
	char                      *fn,
	BAVC_StreamType           eStreamType,
	uint16_t                  ui16VideoPid,
	uint16_t                  ui16PcrPid,
	uint16_t                  ui16EnhPid,
	int                       uiChannelNum
);


BERR_Code StopXPT_Pcr(
	int                       iPidChannelNum
);

BERR_Code ShutdownXPT( void );

void* GetXPTPcrOffset( int );

/***************************************************************************
 *
 * MPEG docoder function prototypes
 *
 */
void  BTST_OpenDecoder
	(BTST_P_Context           *pContext);

void BTST_CloseDecoder
	(BTST_P_Context           *pContext);


/***************************************************************************
 *
 * Frame interception function prototypes
 *
 */

BERR_Code AllocateInterceptFrameBuffer(
	uint32_t       uiFrames,      /* number of frame stores */
	bool           b422,
	uint32_t      *pulLumaBufSize,
	void         **ppvLumaStartAddress,
	uint32_t      *pulChromaBufSize,
	void         **ppvChromaStartAddress
	);

void DumpInterceptFrameBuffer(
	void         *pvLumaStartAddress,
	void         *pvChromaStartAddress,
	bool          b422 );



void b_cacheflush(const void *addr, size_t nbytes);


#if (BTST_P_SUPPORT_TUNER)

/***************************************************************************
 *
 * Frontend (tuner and demod) function prototypes
 *
 */
BERR_Code frontend_init(
	const BFramework_Info    *pFrameworkInfo,
	BTST_P_Context           *pContext
	);

BERR_Code frontend_exit(void);

#endif


#ifdef __cplusplus
}
#endif

#endif /* VDC_TEST_INTERACTIVE_H__ */

/* end of file */
