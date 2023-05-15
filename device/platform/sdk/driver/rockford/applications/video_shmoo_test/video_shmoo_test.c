/***************************************************************************
*     Copyright (c) 2003-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: video_shmoo_test.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 5/23/11 11:52a $
*
* Module Description: Video Shmoo Test
*
* Revision History:
*
* $brcm_Log: /rockford/applications/video_shmoo_test/video_shmoo_test.c $
* 
* Hydra_Software_Devel/8   5/23/11 11:52a jessem
* SW7231-128: Added BCHP_Handle parameter to BTST_VideoShmoo() to support
* power management.
*
* Hydra_Software_Devel/7   3/18/11 12:31p jessem
* SW7344-35: Call BTST_VideoShmoo() only if FRAMEWORK_DO_SHMOO is not
* defined. BTST_ VideoShmoo is called with BFramework_Init().
*
* Hydra_Software_Devel/6   12/8/10 11:51a jessem
* SW7422-93: Simplified public API for easy use with Nexus.
*
* Hydra_Software_Devel/5   12/7/10 2:06p jessem
* SW7422-93: Updated buffer size.
*
* Hydra_Software_Devel/4   12/3/10 4:35p jessem
* SW7422-93: Simplified phase search algorithm.
*
* Hydra_Software_Devel/3   12/3/10 2:54p jessem
* SW7422-93: Added capture facitlity for debug.
*
* Hydra_Software_Devel/2   12/2/10 1:41p jessem
* SW7422-93: Added conditional build rules.
*
* Hydra_Software_Devel/1   12/2/10 11:18a jessem
* SW7422-93: Initial version.
*
**************************************************************************/

#include <stdio.h>
#include <string.h>
#include "framework.h"

#include "framework_board_bvdc.h"

#include "btst_video_shmoo.h"


BDBG_MODULE(VIDEO_SHMOO_TEST);

/**************************************************************************/
int app_main( int argc, char **argv )
{
	BSystem_Info sysInfo;
	BFramework_Info frmInfo;

	BERR_Code               err = BERR_SUCCESS;

	/* System Init (interrupts/memory mapping) */
	int iErr = BSystem_Init( argc, argv, &sysInfo );
	if ( iErr )
	{
		BDBG_ERR(( "System init FAILED!" ));
		return iErr;
	}

	/* Framework init (base modules) */
	iErr = BFramework_Init( &sysInfo, &frmInfo );
	if ( iErr )
	{
		BDBG_ERR(( "Framework init FAILED!" ));
		return iErr;
	}


#if (!FRAMEWORK_DO_SHMOO)
	if (BTST_VideoShmoo(frmInfo.hMem, frmInfo.hChp, frmInfo.hReg) != BERR_SUCCESS)
	{
		BDBG_ERR(("Video Shmoo test failed."));
	}
	else
	{
		BDBG_ERR(("Video Shmoo test passed."));
	}
#endif

	BFramework_Uninit(&frmInfo);
	BSystem_Uninit(&sysInfo);

	BDBG_MSG(("Test complete."));

	return err;
}

/* End of file */
