/***************************************************************************
*     Copyright (c) 2003-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: simple_vdc.c $
* $brcm_Revision: Hydra_Software_Devel/30 $
* $brcm_Date: 12/14/10 4:06p $
*
* Module Description: Framework initialization header file
*
* Revision History:
*
* $brcm_Log: /rockford/applications/simple_vdc/simple_vdc.c $
* 
* Hydra_Software_Devel/30   12/14/10 4:06p darnstein
* SW7231-2: fix some minor logic errors.
* 
* Hydra_Software_Devel/29   12/13/10 6:22p darnstein
* SW7231-2: 7346 does not have S-Video.
* 
* Hydra_Software_Devel/28   11/10/10 11:44a tdo
* SW7422-13: 7422 bringup
*
* Hydra_Software_Devel/27   8/31/09 4:22p darnstein
* SW7342-26: BFramework_GetDacAssignments() replaced by #define's.
*
* Hydra_Software_Devel/26   8/27/09 7:09p darnstein
* SW7342-26: DAC assignments to reference board go through
* BFramework_GetDacAssignments().
*
* Hydra_Software_Devel/25   8/14/09 2:32p darnstein
* PR55219: fix S-Video DAC assignments for 7340 and 7342.
*
* Hydra_Software_Devel/24   8/5/09 4:13p syang
* PR 55812: update 7550 DAC mapping
*
* Hydra_Software_Devel/23   8/3/09 11:22a syang
* PR 55812: add 7550 support
*
* Hydra_Software_Devel/22   6/4/09 3:05p darnstein
* PR44592: add option for 576P output.
*
* Hydra_Software_Devel/21   12/17/08 11:43p tdo
* PR50391: Define7420  Dac setting for simple_vdc test
*
* Hydra_Software_Devel/20   2/20/08 3:05p rpan
* PR38929: Got simple_vdc to run on 3548 IKOS.
*
* Hydra_Software_Devel/19   12/14/07 11:16a tdo
* PR36898: Bringup 7335
*
* Hydra_Software_Devel/18   11/9/07 11:10a yuxiaz
* PR34523: Added DAC settings for 7325.
*
* Hydra_Software_Devel/17   9/13/07 4:17p pntruong
* PR 28750 : Added RDC hooks for RUL capture for splash screen.
*
* Hydra_Software_Devel/16   6/21/07 2:46p yuxiaz
* PR29842: Added correct DAC settings for 7405.
*
* Hydra_Software_Devel/15   3/9/07 11:26a maivu
* PR 26051: Added Vec crossbar support
*
* Hydra_Software_Devel/14   2/26/07 11:13a maivu
* PR 25963: Changes to support 7400B0 Dac settings
*
* Hydra_Software_Devel/13   1/4/07 4:21p pntruong
* PR22577:  Added dac mapping for 93563.
*
* Hydra_Software_Devel/12   12/19/06 12:05a pntruong
* PR22577: Merged back to mainline.
*
* Hydra_Software_Devel/Refsw_Devel_3563/2   9/20/06 1:04p pntruong
* PR23222, PR23225:  Defer window and source heap assignments until user
* create.
*
* Hydra_Software_Devel/Refsw_Devel_3563/1   9/19/06 10:51p pntruong
* PR23222, PR23225:  Adapt new create with heap.
*
* Hydra_Software_Devel/11   9/9/06 6:10a albertl
* PR23117:  Added 7440 support.
*
* Hydra_Software_Devel/10   8/8/06 11:55a pntruong
* PR22577: Initial bringup of VDC.  Removed the gfx test, as it's already
* available with mvd_vdc_test/interactive.
*
* Hydra_Software_Devel/9   6/1/06 12:24p hongtaoz
* PR20140: 7400A0 doesn't support VEC swamp;
*
* Hydra_Software_Devel/8   5/19/06 11:42a pntruong
* PR 17530: Added BTMR support to comply with new BVDC_Open.
*
* Hydra_Software_Devel/7   2/17/06 11:25a hongtaoz
* PR19082: added 7400 support;
*
* Hydra_Software_Devel/6   9/30/05 2:22p pntruong
* PR17377, PR17203:  Settop API start up on 7401 will often have one of
* two VDC timeouts.  This is due to the partial or corrupted rul due to
* cached does not work properly.  Though, 7401 still need vec reset when
* doing vdc open to removed previously vec running state and residue
* interrupts.
*
* Hydra_Software_Devel/5   9/8/05 4:22p syang
* PR 16943: add sophistical gfx test
*
* Hydra_Software_Devel/4   9/2/05 11:16a jasonh
* PR 16967: Added debug settings.
*
* Hydra_Software_Devel/3   9/1/05 1:07p jasonh
* PR 16943: Added optional support for graphics.
*
* Hydra_Software_Devel/2   8/31/05 3:52p jasonh
* PR 16397: cleaned up error detection
*
* Hydra_Software_Devel/1   7/27/05 5:04p jasonh
* PR 16397: Initial version.
*
***************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "framework.h"

#include "brdc.h"
#include "bvdc.h"
#include "framework_board_bvdc.h"

#ifdef IKOS_EMULATION
#include "client.h"
#endif

BDBG_MODULE(SIMPLE_VDC);

/**************************************************************************/
#define DISP_FORMAT BFMT_VideoFmt_e1080i

/**************************************************************************/
#define TestError(e, str)	{\
	err = e;\
	if (err != BERR_SUCCESS)\
	{\
	BDBG_ERR(( str". %s: %d %x", __FILE__,__LINE__, e ));\
		goto Done;\
	}\
}

#ifdef IKOS_EMULATION

static void BTST_P_IkosInitCapture
	( void )
{
	char achCapturePath[40];

	strcpy(achCapturePath, ".");

	if (IKOS_Client_InitCapture( 0, sizeof(achCapturePath), achCapturePath ) != 0)
		BDBG_ERR(("Failed to initialize IKOS capture"));

}

static void BTST_P_IkosStartCapture
	( void )
{
	if (IKOS_Client_StartCapture() != 0)
		BDBG_ERR(("Failed to start IKOS capture"));
}

static void BTST_P_IkosStopCapture
	( void )
{
	if (IKOS_Client_StopCapture() != 0)
		BDBG_ERR(("Failed to stop IKOS capture"));
}

#endif

/**************************************************************************/
int app_main( int argc, char **argv )
{
	BSystem_Info sysInfo;
	BFramework_Info frmInfo;
	BVDC_Settings stVdcConfig;

	BRDC_Handle             hRdc;
	BVDC_Handle             hVdc;
	BVDC_Compositor_Handle  hCompositor0;
	BVDC_Compositor_Handle  hCompositor1;
	BVDC_Display_Handle     hDisplay0;
	BVDC_Display_Handle     hDisplay1;
	BFMT_VideoFmt           eDisplayFmt = BFMT_VideoFmt_eNTSC;
	BVDC_CompositorId       eCmpId0 = BVDC_CompositorId_eCompositor0;
#if (BCHP_CHIP==7325) || (BCHP_CHIP==7335)
	BVDC_DisplayId          eDisplayId0 = BVDC_DisplayId_eDisplay1;
#else
	BVDC_DisplayId          eDisplayId0 = BVDC_DisplayId_eDisplay0;
#endif
	BVDC_CompositorId       eCmpId1 = BVDC_CompositorId_eCompositor0;
	BVDC_DisplayId          eDisplayId1 = BVDC_DisplayId_eDisplay0;
	BFMT_VideoInfo          stVideoFmtInfo;
	BERR_Code               err = BERR_SUCCESS;
	bool                    bDualDisplay=false;
	int                     i;

	/* System Init (interrupts/memory mapping) */
	int iErr = BSystem_Init( argc, argv, &sysInfo );
	if ( iErr )
	{
		printf( "System init FAILED!\n" );
		return iErr;
	}

	/* Framework init (base modules) */
	iErr = BFramework_Init( &sysInfo, &frmInfo );
	if ( iErr )
	{
		printf( "Framework init FAILED!\n" );
		return iErr;
	}

	if (argc > 1)
	{
		for(i=1; i < argc; i++)
		{
			if(!strcmp(argv[i], "NTSC"))
				eDisplayFmt = BFMT_VideoFmt_eNTSC;
			else if(!strcmp(argv[i], "480p"))
				eDisplayFmt = BFMT_VideoFmt_e480p;
			else if(!strcmp(argv[i], "576p"))
				eDisplayFmt = BFMT_VideoFmt_e576p_50Hz;
			else if(!strcmp(argv[i], "720p"))
				eDisplayFmt = BFMT_VideoFmt_e720p;
			else if(!strcmp(argv[i], "1080i"))
				eDisplayFmt = BFMT_VideoFmt_e1080i;
			else if(!strcmp(argv[i], "PAL"))
				eDisplayFmt = BFMT_VideoFmt_ePAL_B;
			else if (!strcmp(argv[i], "c0"))
				eCmpId0 = BVDC_CompositorId_eCompositor0;
			else if (!strcmp(argv[i], "c1"))
				eCmpId0 = BVDC_CompositorId_eCompositor1;
			else if (!strcmp(argv[i], "c2"))
				eCmpId0 = BVDC_CompositorId_eCompositor2;
			else if (!strcmp(argv[i], "d0"))
				eDisplayId0 = BVDC_DisplayId_eDisplay0;
			else if (!strcmp(argv[i], "d1"))
				eDisplayId0 = BVDC_DisplayId_eDisplay1;
			else if (!strcmp(argv[i], "d2"))
				eDisplayId0 = BVDC_DisplayId_eDisplay2;
			else if (!strcmp(argv[i], "auto"))
				eDisplayId0 = BVDC_DisplayId_eAuto;
			else if (!strcmp(argv[i], "dual"))
				bDualDisplay= true;
			else
			{
				fprintf (
					stderr, "ERROR: unrecognized option \"%s\"\n", argv[i]);
				exit (1);
			}

		}
	}

#if 0
	BDBG_SetModuleLevel("BVDC", BDBG_eMsg);
	BDBG_SetModuleLevel("BVDC_DISP", BDBG_eMsg);
#endif

	/* get data about output format */
	TestError( BFMT_GetVideoFormatInfo( eDisplayFmt, &stVideoFmtInfo ),
		"ERROR: BFMT_GetVideoFormatInfo" );

	printf("Display format is %s\n", stVideoFmtInfo.pchFormatStr);
	printf("[Compositor %d -> Display %d]\n", eCmpId0, eDisplayId0);

	/* open Register DMA */
	TestError( BRDC_Open(&hRdc, frmInfo.hChp, frmInfo.hReg, frmInfo.hMem, NULL),
		"ERROR: BRDC_Open" );

	/* open VDC */
	BVDC_GetDefaultSettings(&stVdcConfig);

	TestError( BVDC_Open(&hVdc,frmInfo.hChp, frmInfo.hReg, frmInfo.hMem,
		frmInfo.hInt, hRdc, frmInfo.hTmr, &stVdcConfig),
		"ERROR: BVDC_Open" );

	/* create compositor handle */
	TestError( BVDC_Compositor_Create( hVdc, &hCompositor0, eCmpId0, NULL ),
		"ERROR: BVDC_Compositor_Create" );

	/* create display for our new compositor */
	TestError( BVDC_Display_Create( hCompositor0, &hDisplay0, eDisplayId0, NULL),
		"ERROR: BVDC_Display_Create" );

	/* Set display format */
	TestError( BVDC_Display_SetVideoFormat( hDisplay0, eDisplayFmt ),
		"ERROR: BVDC_Display_SetVideoFormat" );

	/* Set the background color to red */
	TestError( BVDC_Compositor_SetBackgroundColor( hCompositor0,
		0xFF, 0x00, 0x00 ),
		"ERROR: BVDC_Compositor_SetBackgroundColor" );

	/* Set component DACs for all formats */
	TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
		BFramework_Dac_Component_pr, BVDC_DacOutput_ePr),
		"ERROR: BVDC_Display_SetDacConfiguration" );
	TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
		BFramework_Dac_Component_y, BVDC_DacOutput_eY),
		"ERROR: BVDC_Display_SetDacConfiguration" );
	TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
		BFramework_Dac_Component_pb, BVDC_DacOutput_ePb),
		"ERROR: BVDC_Display_SetDacConfiguration" );

#if (BCHP_CHIP != 3548) /** { **/
	/* set DAC configurations for specific display format */
	switch (eDisplayFmt)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_ePAL_G:
	case BFMT_VideoFmt_ePAL_H:
	case BFMT_VideoFmt_ePAL_K:
	case BFMT_VideoFmt_ePAL_B:
		if (!bDualDisplay &&
			((eDisplayId0 == BVDC_DisplayId_eDisplay1) ||
			((eDisplayId0 == BVDC_DisplayId_eAuto) && (eCmpId0 == BVDC_CompositorId_eCompositor1))))
		{
			printf("Svideo/Composite\n");
			TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
				BFramework_Dac_Composite_0, BVDC_DacOutput_eComposite),
				"ERROR: BVDC_Display_SetDacConfiguration" );
#if (BCHP_CHIP != 7550) && (BCHP_CHIP != 7422) && \
    (BCHP_CHIP != 7344) && (BCHP_CHIP != 7346)
			TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
				BFramework_Dac_Svideo_Chroma, BVDC_DacOutput_eSVideo_Chroma),
				"ERROR: BVDC_Display_SetDacConfiguration" );
			TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
				BFramework_Dac_Svideo_Luma, BVDC_DacOutput_eSVideo_Luma),
				"ERROR: BVDC_Display_SetDacConfiguration" );
#endif
		}
		break;
	default: break;
	}
#endif /** } **/

	if (bDualDisplay)
	{
		printf("Dual Display\n");
		printf("[2nd Compositor %d -> Display %d]\n", eCmpId1, eDisplayId1);

		/* create compositor handle */
		TestError( BVDC_Compositor_Create( hVdc, &hCompositor1, eCmpId1, NULL ),
			"ERROR: BVDC_Compositor_Create" );

		/* create display for our new compositor */
		TestError( BVDC_Display_Create( hCompositor1, &hDisplay1, eDisplayId1, NULL),
			"ERROR: BVDC_Display_Create" );

		/* Set display format */
		TestError( BVDC_Display_SetVideoFormat( hDisplay1, BFMT_VideoFmt_eNTSC ),
			"ERROR: BVDC_Display_SetVideoFormat" );

		/* Set the background color to blue */
		TestError( BVDC_Compositor_SetBackgroundColor( hCompositor1,
			0x00, 0x00, 0xFF ),
			"ERROR: BVDC_Compositor_SetBackgroundColor" );

#if (BCHP_CHIP != 3548)
		TestError( BVDC_Display_SetDacConfiguration( hDisplay1,
			BFramework_Dac_Composite_0, BVDC_DacOutput_eComposite),
			"ERROR: BVDC_Display_SetDacConfiguration" );
#if (BCHP_CHIP != 7550) && (BCHP_CHIP != 7422) && (BCHP_CHIP != 7425) && \
    (BCHP_CHIP != 7344) && (BCHP_CHIP != 7346)
		TestError( BVDC_Display_SetDacConfiguration( hDisplay1,
			BFramework_Dac_Svideo_Chroma, BVDC_DacOutput_eSVideo_Chroma),
			"ERROR: BVDC_Display_SetDacConfiguration" );
		TestError( BVDC_Display_SetDacConfiguration( hDisplay1,
			BFramework_Dac_Svideo_Luma, BVDC_DacOutput_eSVideo_Luma),
			"ERROR: BVDC_Display_SetDacConfiguration" );
#endif
#endif
	}

	/* apply changes */
	TestError( BVDC_ApplyChanges(hVdc),
		"ERROR:BVDC_ApplyChanges" );

#ifdef IKOS_EMULATION
	/* start IKOS capture */
	BTST_P_IkosInitCapture();
	BTST_P_IkosStartCapture();

	printf("IKOS capture started \n");
#endif

#if 0
	while (1)
	{
		/* printf ("Black\n"); */
		TestError( BVDC_Compositor_SetBackgroundColor( hCompositor0,
			0x00, 0x00, 0x00 ),
			"ERROR: BVDC_Compositor_SetBackgroundColor" );
		TestError( BVDC_ApplyChanges(hVdc),
			"ERROR:BVDC_ApplyChanges" );
		BKNI_Sleep (1000);

		/* printf ("Red\n"); */
		TestError( BVDC_Compositor_SetBackgroundColor( hCompositor0,
			0xFF, 0x00, 0x00 ),
			"ERROR: BVDC_Compositor_SetBackgroundColor" );
		TestError( BVDC_ApplyChanges(hVdc),
			"ERROR:BVDC_ApplyChanges" );
		BKNI_Sleep (1000);

		/* printf ("Green\n"); */
		TestError( BVDC_Compositor_SetBackgroundColor( hCompositor0,
			0x00, 0xFF, 0x00 ),
			"ERROR: BVDC_Compositor_SetBackgroundColor" );
		TestError( BVDC_ApplyChanges(hVdc),
			"ERROR:BVDC_ApplyChanges" );
		BKNI_Sleep (1000);

		/* printf ("Blue\n"); */
		TestError( BVDC_Compositor_SetBackgroundColor( hCompositor0,
			0x00, 0x00, 0xFF ),
			"ERROR: BVDC_Compositor_SetBackgroundColor" );
		TestError( BVDC_ApplyChanges(hVdc),
			"ERROR:BVDC_ApplyChanges" );
		BKNI_Sleep (1000);
	}
#endif

	/* Wait for user */
	printf( "Hit key to exit\n" );
	getchar();

#ifdef IKOS_EMULATION
	BTST_P_IkosStopCapture();

	printf("IKOS capture stopped \n");
#endif

	/* Clean up! */
	TestError( BVDC_Display_Destroy( hDisplay0 ), "ERROR: BVDC_Display_Destroy" );
	TestError( BVDC_Compositor_Destroy( hCompositor0 ), "ERROR: BVDC_Compositor_Destroy" );
	TestError( BVDC_ApplyChanges( hVdc ), "ERROR: BVDC_ApplyChanges" );
	TestError( BVDC_Close( hVdc ), "ERROR: BVDC_Close" );
	TestError( BRDC_Close( hRdc ), "ERROR: BRDC_Close" );

Done:
	BFramework_Uninit(&frmInfo);
	BSystem_Uninit(&sysInfo);
	return err;
}

/* End of file */

