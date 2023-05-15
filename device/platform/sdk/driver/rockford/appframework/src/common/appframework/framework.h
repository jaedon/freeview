/***************************************************************************
*     Copyright (c) 2003-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: framework.h $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 3/11/10 1:48p $
*
* Module Description: Framework initialization header file
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/common/appframework/framework.h $
* 
* Hydra_Software_Devel/8   3/11/10 1:48p tdo
* SW7400-2418: AppFramework: add calls to BPWR_Open/BPWR_Close before
* calling BCHP_Open
*
* Hydra_Software_Devel/7   8/31/09 4:57p darnstein
* SW7342-26: BFramework_GetDacAssignments() replaced by #define's.
*
* Hydra_Software_Devel/6   8/25/09 7:39p darnstein
* SW7342-26: change definition of BFramework_GetDacAssignments().
*
* Hydra_Software_Devel/5   8/25/09 7:18p darnstein
* SW7342-26: New function BFramework_GetDacAssignments().
*
* Hydra_Software_Devel/4   7/15/09 5:39p tdo
* PR56690: Undo previous check ins
*
* Hydra_Software_Devel/3   7/15/09 11:35a jhaberf
* PR53796: Added infrastruction for 35130 build.
*
* Hydra_Software_Devel/2   7/14/09 3:57p tdo
* PR56690: AppFramework: add calls to BPWR_Open/BPWR_Close before calling
* BCHP_Open
*
* Hydra_Software_Devel/1   8/2/06 11:14a syang
* PR 22212: split board specific frame work out to framework_board.c/h
*
* Hydra_Software_Devel/18   6/20/06 2:34p pntruong
* PR 21689: Put back tmr for 3560.
*
* Hydra_Software_Devel/17   6/13/06 4:15p syang
* PR 21689: not use i2C handle if the chip doesn't support
*
* Hydra_Software_Devel/16   6/13/06 3:10p syang
* PR 21689: not include i2c if the chip doesn't support
*
* Hydra_Software_Devel/15   5/11/06 10:41a jessem
* PR 17530: Added BTMR support.
*
* Hydra_Software_Devel/14   4/10/06 5:56p syang
* PR 20748: fix 97398 compile problem
*
* Hydra_Software_Devel/13   4/10/06 2:19p syang
* PR 20748:  added fpga
*
* Hydra_Software_Devel/12   2/14/06 2:37p maivu
* PR 19599: Use BCHP_7411_VER
*
* Hydra_Software_Devel/11   2/13/06 1:55p maivu
* PR 19599: Fixed 97398 builts
*
* Hydra_Software_Devel/10   2/13/06 11:29a hongtaoz
* PR19082: bring up dual xvd decode for 7400;
*
* Hydra_Software_Devel/9   2/10/06 4:59p maivu
* PR 19599: Fixed errors with prevous check-in for non-97398 builts
*
* Hydra_Software_Devel/8   2/10/06 3:29p maivu
* PR 19599: Add 7411 support for 97398
*
* Hydra_Software_Devel/7   2/8/06 8:10p hongtaoz
* PR19082: bring up xvd_vdc_test for 7400;
*
* Hydra_Software_Devel/6   9/12/05 3:38p darnstein
* PR16942: Adapt to name change MAX_I2C_CHANNELS ->
* BI2C_MAX_I2C_CHANNELS.
*
* Hydra_Software_Devel/5   8/31/05 5:23p jasonh
* PR 16943: Added I2C support.
*
* Hydra_Software_Devel/4   7/27/05 6:06p jasonh
* PR 16397: Fixed define of app_main.
*
* Hydra_Software_Devel/3   7/27/05 4:18p jasonh
* PR 16397: Broke out code to initialize FPGA
*
* Hydra_Software_Devel/2   7/27/05 3:58p jasonh
* PR 16397: General clean-up. Renaming of structure elements.
*
* Hydra_Software_Devel/4   2/4/04 7:57a marcusk
* PR9588: Added support for fpga, tested and working.
*
* Hydra_Software_Devel/3   12/16/03 1:39p frusso
* PR8916:  Added magnum includes
*
* Hydra_Software_Devel/2   12/11/03 8:29a frusso
* PR8916: Fixed build errors
*
* Hydra_Software_Devel/1   12/10/03 4:04p frusso
* PR8916: initial checkin
*
***************************************************************************/

#ifndef FRAMEWORK_H__
#define FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bstd.h"
#include "bkni.h"
#include "bchp.h"
#include "bmem.h"
#include "bint.h"
#include "btmr.h"
#include "systeminit.h"

#ifdef BPWR_SUPPORT
#include "bpwr.h"
#endif

typedef struct BFrmWorkBoard_Info *BFrmWorkBoard_Handle;
typedef struct BFramework_Info
{
	BCHP_Handle         hChp;

	BREG_Handle         hReg;

	BTMR_Handle         hTmr;
	BMEM_ModuleHandle   hMemModule;     /* new BMEM */
	BMEM_Heap_Handle    hMem;           /* main heap, old  BMEM_Handle */

	BINT_Handle         hInt;

#ifdef BPWR_SUPPORT
	BPWR_Handle         hPwr;
#endif

	/* board specific stuff such as I2C, FGPA, and other chips such as 7411 */
	BFrmWorkBoard_Handle  hFrmWorkBoard;
} BFramework_Info;

int BFramework_Init( const BSystem_Info *pSysInfo, BFramework_Info *pFrameworkInfo );
void BFramework_Uninit(BFramework_Info *pFrameworkInfo);

/*
 * Each entry in this struct is
 * one of the values BVDC_Dac_n defined in bvdc.h .
 * Zero indicates "not present on board."
 */
typedef struct BFramework_DacAssignments
{
	uint32_t composite_0;
	uint32_t composite_1;
	uint32_t svideo_luma;
	uint32_t svideo_chroma;
	uint32_t component_y;
	uint32_t component_pr;
	uint32_t component_pb;

} BFramework_DacAssignments;

/* Application must define this function */
int app_main(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif

