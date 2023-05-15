/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cmd_usgmod.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 10/1/09 2:15p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/cmd_usgmod.c $
 * 
 * Hydra_Software_Devel/21   10/1/09 2:15p jessem
 * SW3548-2506, SW3556-818:
 * 1. Added config 14 support (SW3548-2506).
 * 2. Added WXGA and XGA support for config 7 (SW3556-818).
 * 3. Added 1024x768_PC@60 and 1024x768_PC@85 input format groups.
 * 4. Modified algorithm to search for progressive input formats in
 * BVDB_GetInputFormatGroup.
 *
 * Hydra_Software_Devel/20   9/11/09 3:08p jessem
 * SW3548-2463, SW3548-2465: Applied needed changes in call to
 * BVDB_GetDisplaySourceAs.
 *
 * Hydra_Software_Devel/19   8/27/09 5:13p jessem
 * SW3548-2419: Corrected calls to BVDB_GetInputFormatGroup to adapt to
 * new VDB API.
 *
 * Hydra_Software_Devel/18   8/5/09 5:59p jessem
 * PR 57391, PR 57392:
 * 1. Added 1920x1080 PC @ 60 Group. Renamed 1024x768_PC@60 Group to
 * 1366x768_PC@60 Group , 1280x768_PC@85 Group to 1366x768_PC@85 Group.
 * Added 1920x1080_PC@60 usage mdoes to all configurations (57391).
 * 2.  Removed and modified certain invalid PC formats (57392).
 *
 * Hydra_Software_Devel/17   7/10/09 5:54p rpan
 * PR55188: Convert scanf() to BTST_P_Scanf().
 *
 * Hydra_Software_Devel/16   4/27/09 11:20a jessem
 * PR 51743: Added bPbp parameter to BVDB_GetVideoDisplayMode call and
 * cleaned up UI in BTST_P_GetVideoDisplayMode.
 *
 * Hydra_Software_Devel/15   4/13/09 3:48p jessem
 * PR 51743: Modified  BVDB_GetDisplaySrcAs and BVDB_GetVideoDisplayModes
 * to handle source aspect ratio and aspect ratio correction.
 *
 * Hydra_Software_Devel/12   3/19/09 4:09p jessem
 * PR 50031, PR 51709, PR 51848: Added config 8 and 9 support, Added
 * memory allocation requriement test.
 *
 * Hydra_Software_Devel/11   1/5/09 12:41p jessem
 * PR 50327: Added display resolution in
 * BTST_P_GetUserUsageModeParameters.
 *
 * Hydra_Software_Devel/10   12/9/08 3:58p jessem
 * PR 49051, PR 46591, PR 48974, PR 49929, PR 49870, PR 49932: Added
 * support for dot-by-dot in configs 1 and 2. Added config for 1066Mhz
 * x16 (demo). Added config 6 for BroadbandTV-US. Modified config 1 to
 * include support for Brazil's PDP XGA and BroadbandTV-KR. Added support
 * for Magic TV to config 1 and 2.
 *
 * Hydra_Software_Devel/9   10/21/08 11:01a jessem
 * PR 46299: Split BVDB_InputFormatGroup_e1400_1050_PC to
 * BVDB_InputFormatGroup_e1400_1050_75_PC and
 * BVDB_InputFormatGroup_e1400_1050_60_PC.
 *
 * Hydra_Software_Devel/8   10/15/08 4:25p jessem
 * PR 39335: Suppressed unused parameter warning.
 *
 * Hydra_Software_Devel/7   10/3/08 4:38p jessem
 * PR 46299: Added test for BVDB_GetInputFormatGroup.
 *
 * Hydra_Software_Devel/6   10/3/08 11:03a jessem
 * PR 46299: Added B0 support.
 *
 * Hydra_Software_Devel/5   8/19/08 11:13a jessem
 * PR 43098: Added new PC formats to comply with new tolerance and bias
 * requirements fro config 4.
 *
 * Hydra_Software_Devel/4   8/4/08 2:15p jessem
 * PR 45385: Changed bvdb_file.h to bvdb_parser.h.
 *
 * Hydra_Software_Devel/3   7/14/08 3:26p jessem
 * PR 39152: Integrated use of VDB RTS.
 *
 * Hydra_Software_Devel/2   6/18/08 3:42p jessem
 * PR 39237: Modified comment in BTST_P_ConfirmMode.
 *
 * Hydra_Software_Devel/1   6/18/08 1:09p jessem
 * PR 39237, PR 43759: Initial version.
 *
 ***************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "commands.h"
#include "bvdb.h"

#ifdef BVDB_GEN_FILE
#include "bvdb_parser.h"

static void BTST_P_GetUserUsageModeParameters
	( BTST_P_Context  *pContext,
	  BVDB_UsageMode  *pstUsageMode,
	  bool             bQuery );

#endif

void BTST_P_CreateVdb
	( BTST_P_Context  *pContext )
{
	BVDB_CreateVdb(&pContext->hVdb, &pContext->ulEntries);

	printf("Got VDB\n");
	printf("Number of VDB entries = %d\n", pContext->ulEntries);
}

void BTST_P_DestroyVdb
	( BTST_P_Context  *pContext )
{
	BVDB_DestroyVdb(pContext->hVdb);

	pContext->hVdb = NULL;
}

void BTST_P_InsertVdbEntry
	( BTST_P_Context  *pContext )
{
	char acCfg[2];
	BVDB_UsageMode stUsageMode;

#ifdef BVDB_GEN_FILE
	BTST_P_GetUserUsageModeParameters(pContext, &stUsageMode, false);
#else
	/* TODO:
	 * Fill all the BVDB_UsageMode fields excpet for the acId field which just requires
	 * the config #. See below.
	 */
#endif

	printf("Enter configuration number: ");
	BTST_P_Scanf(pContext, "%s", acCfg);

	strcpy(stUsageMode.acId, acCfg);

	BVDB_AddVdbEntry(pContext->hVdb, &stUsageMode);
}

void BTST_P_RemoveVdbEntry
	( BTST_P_Context  *pContext )
{
	BVDB_UsageMode stUsageMode;

	printf("Enter usage mode id: ");
	scanf ("%s", stUsageMode.acId);

	BVDB_RemoveVdbEntry(pContext->hVdb, &stUsageMode);
}

void BTST_P_PrintVdb
	( BTST_P_Context  *pContext )
{
	BVDB_PrintVdb(pContext->hVdb);
}

void BTST_P_GetUsageMode
	( BTST_P_Context      *pContext )
{
	BVDB_UsageMode stUsageMode;

	printf("Enter usage mode ID : ");
	BTST_P_Scanf(pContext, "%s", stUsageMode.acId);

	if (BVDB_GetVdbEntryById(pContext->hVdb, &stUsageMode) == BERR_SUCCESS)
	{
		if (BVDB_PrintUsageMode(&stUsageMode) != BERR_SUCCESS)
			printf("error\n");
		else
			printf("success\n");
	}
	else
	{
		printf("Cannot find usage mode ID %s\n", stUsageMode.acId);
	}
}

void BTST_P_ValidateCfgId
	( BTST_P_Context      *pContext )
{
	uint32_t i;

	printf("Enter configuration number: ");
	BTST_P_Scanf(pContext, "%u", &i);
	if (BVDB_ValidateConfigId(pContext->hVdb, i) != BERR_SUCCESS)
		printf("Config ID %d is invalid\n", i);
	else
		printf("Config ID %d is valid\n", i);
}

void BTST_P_ConfirmMode
	( BTST_P_Context      *pContext )
{
	BVDB_UsageMode stUsageMode;
	char acCfg[3];

#ifdef BVDB_GEN_FILE
	BTST_P_GetUserUsageModeParameters(pContext, &stUsageMode, true);
#else
	/* TODO: Fill the following BVDB_UsageMode fields :
	 * acId (config #), eDisplay, eSrcFmtSet, eWindow, eOsd, eInput, stDisplayMode  */
#endif

	printf("Enter configuration number: ");
	BTST_P_Scanf(pContext, "%s", acCfg);

	strcpy(stUsageMode.acId, acCfg);

	if (BVDB_GetVdbEntry(pContext->hVdb, &stUsageMode) == BERR_SUCCESS)
	{
		printf("Valid usage mode. The ID is %s\n", stUsageMode.acId);
		BVDB_PrintUsageMode(&stUsageMode);
	}
	else
	{
		printf("Invalid usage mode\n");
	}
}

void BTST_P_GetRts
	( BTST_P_Context      *pContext )
{
	BVDB_UsageMode stUsageMode;
	uint32_t i;
	const BVDB_RtsTable *pstRtsTbl = NULL;
	char acClientName[20];

	printf("Enter usage mode ID: ");
	BTST_P_Scanf(pContext, "%s", stUsageMode.acId);


	if (BVDB_GetVdbEntryById(pContext->hVdb, &stUsageMode) == BERR_SUCCESS)
	{
		pstRtsTbl = BVDB_GetRtsTable(pContext->hVdb, stUsageMode.eRtsSet);
		if (pstRtsTbl)
		{
			printf("RTS %d:\n", stUsageMode.eRtsSet);
			for (i=0; i<pstRtsTbl->ulValidTableEntries; i++)
			{
				printf("ulId: %d\n", pstRtsTbl->astRts[i].ulId);
				BVDB_GetRtsClientName(pContext->hVdb, pstRtsTbl->astRts[i].ulId, acClientName);
				printf("Client: %s\n", acClientName);
				printf("ulRegValue = 0x%8.8x\n", pstRtsTbl->astRts[i].ulRegValue);
			}
		}
	}
	else
	{
		printf("Cannot find usage mode with ID %s\n", stUsageMode.acId);
	}
}

int BTST_P_ValidateAndClip
	( BTST_P_Context      *pContext,
	  BVDB_UsageMode      *pstUsageMode,
	  BFMT_VideoFmt        eSrcFmt )
{
	int ans = 0;
	uint32_t ulLeft, ulRight, ulTop, ulBot, ulHeightClipAmount, ulWidthClipAmount;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	const BFMT_VideoInfo *pVidInfo;
	BVDB_VideoDisplayMode stMode;

	/* Clip the source. */
	printf("Do you want to clip the source? (0=n/1=y) ");
	BTST_P_Scanf(pContext, "%d", &ans);

	/* show allowed clip amount for usage mode */
	if (ans)
	{
		BVDB_UsageMode  stMainWinUsageMode;

		printf("\nPlease enter clip values (Left, Right, Top, Bot):");
		BTST_P_Scanf(pContext, "%u %u %u %u", &ulLeft, &ulRight, &ulTop, &ulBot);

		ulHeightClipAmount = ulTop + ulBot;
		ulWidthClipAmount = ulLeft + ulRight;

		pVidInfo = BFMT_GetVideoFormatInfoPtr(eSrcFmt);

#if (BCHP_VER == BCHP_VER_A0)
		if (BVDB_DISP_MODE_IS_VCR_1(&pstUsageMode->stDisplayMode))
#else
		if (BVDB_DISP_MODE_IS_MON_1(&pstUsageMode->stDisplayMode))
#endif
		{
			printf("Enter usage mode ID for main window: ");
			BTST_P_Scanf(pContext, "%s", stMainWinUsageMode.acId);

			if (BVDB_GetVdbEntryById(pContext->hVdb, &stMainWinUsageMode) != BERR_SUCCESS)
			{
				printf("Need main window display mode info for this VCR out window\n");
				return -1;
			}
			else
			{
				/* save current display mode */
				stMode = pstUsageMode->stDisplayMode;
				/* copy main window's display mode to the VCR usage mode */
				pstUsageMode->stDisplayMode = stMainWinUsageMode.stDisplayMode;
			}
		}

		printf("pVidInfo->ulHeight = %d,  pVidInfo->ulWidth = %d\n",
			 pVidInfo->ulHeight, pVidInfo->ulWidth);

		if (BVDB_ValidateSourceClipAmount(pContext->hVdb, pstUsageMode,
			pVidInfo->ulHeight,  pVidInfo->ulWidth, ulHeightClipAmount,  ulWidthClipAmount) != 0)
		{
			printf("Invalid source clip amount\n");
			return -1;
		}
		else
		{
			BVDC_Window_SetSrcClip(hVidWin, ulLeft, ulRight, ulTop, ulBot);
		}

#if (BCHP_VER == BCHP_VER_A0)
		if (BVDB_DISP_MODE_IS_VCR_1(&pstUsageMode->stDisplayMode))
#else
		if (BVDB_DISP_MODE_IS_MON_1(&pstUsageMode->stDisplayMode))
#endif
		{
			/* restore */
			pstUsageMode->stDisplayMode = stMode;
		}
	}
	return 0;
}

void BTST_P_SourceClip
	( BTST_P_Context      *pContext )
{
	BVDB_UsageMode stUsageMode;
#if (BCHP_VER == BCHP_VER_A0)

	BFMT_VideoFmt eFmt = BVDB_SrcFmtSet_eInvalid;
#else
	BFMT_VideoFmt eFmt = BVDB_InputFormatGroup_eInvalid;
#endif

	printf("Enter usage mode ID: \n");
	BTST_P_Scanf(pContext, "%s", stUsageMode.acId);

	BVDB_GetVdbEntryById(pContext->hVdb, &stUsageMode);

	BVDB_PrintUsageMode(&stUsageMode);

#if (BCHP_VER == BCHP_VER_A0)

	switch(stUsageMode.eSrcFmtSet)
	{
		case BVDB_SrcFmtSet_eNtsc:
			eFmt = BFMT_VideoFmt_eNTSC;
			break;
		case BVDB_SrcFmtSet_ePal:
			eFmt = BFMT_VideoFmt_ePAL_G;
			break;
		case BVDB_SrcFmtSet_e480p:
			eFmt = BFMT_VideoFmt_e480p;
			break;
		case BVDB_SrcFmtSet_e576p:
			eFmt = BFMT_VideoFmt_e576p_50Hz;
			break;
		case BVDB_SrcFmtSet_e720p_50:
			eFmt = BFMT_VideoFmt_e720p_50Hz;
			break;
		case BVDB_SrcFmtSet_e720p_60:
			eFmt = BFMT_VideoFmt_e720p;
			break;
		case BVDB_SrcFmtSet_e1080i_50:
		case BVDB_SrcFmtSet_e1440_1080i_50:
			eFmt = BFMT_VideoFmt_e1080i_50Hz;
			break;
		case BVDB_SrcFmtSet_e1080i_60:
		case BVDB_SrcFmtSet_e1440_1080i_60:
			eFmt = BFMT_VideoFmt_e1080i;
			break;
		case BVDB_SrcFmtSet_e1080p_24:
		case BVDB_SrcFmtSet_e1440_1080p_24:
			eFmt = BFMT_VideoFmt_e1080p_24Hz;
			break;
		case BVDB_SrcFmtSet_e1080p_25:
		case BVDB_SrcFmtSet_e1440_1080p_25:
			eFmt = BFMT_VideoFmt_e1080p_25Hz;
			break;
		case BVDB_SrcFmtSet_e1080p_30:
		case BVDB_SrcFmtSet_e1440_1080p_30:
			eFmt = BFMT_VideoFmt_e1080p_30Hz;
			break;
		case BVDB_SrcFmtSet_e1080p_50:
			eFmt = BFMT_VideoFmt_e1080p_50Hz;
			break;
		case BVDB_SrcFmtSet_e1080p_60:
			eFmt = BFMT_VideoFmt_e1080p;
			break;
		case BVDB_SrcFmtSet_e1024_768_PC:
			eFmt = BFMT_VideoFmt_eDVI_1024x768p;
			break;
		case BVDB_SrcFmtSet_e1280_768_PC:
			eFmt = BFMT_VideoFmt_eDVI_1280x768p_85Hz;
			break;
		case BVDB_SrcFmtSet_e1600_1200_PC:
			eFmt = BFMT_VideoFmt_eDVI_1600x1200p_60Hz;
			break;
		case BVDB_SrcFmtSet_e1400_1050_PC:
			eFmt = BFMT_VideoFmt_eDVI_1400x1050p_75Hz;
			break;

		case BVDB_SrcFmtSet_eAdcCalibration:
		case BVDB_SrcFmtSet_eJpeg:
			if ((stUsageMode.eDisplay == BVDB_Display_ePrimary50) ||
				(stUsageMode.eDisplay == BVDB_Display_eSecondary50))
				eFmt = BFMT_VideoFmt_ePAL_G;
			else
				eFmt = BFMT_VideoFmt_eNTSC;
			break;
		case BVDB_SrcFmtSet_eInvalid:
		default:
			printf("Unknown format\n");
	}
#else
	switch(stUsageMode.eInputFormatGroup)
	{
		case BVDB_InputFormatGroup_eNtsc:
			eFmt = BFMT_VideoFmt_eNTSC;
			break;
		case BVDB_InputFormatGroup_ePal:
			eFmt = BFMT_VideoFmt_ePAL_G;
			break;
		case BVDB_InputFormatGroup_e480p:
			eFmt = BFMT_VideoFmt_e480p;
			break;
		case BVDB_InputFormatGroup_e576p:
			eFmt = BFMT_VideoFmt_e576p_50Hz;
			break;
		case BVDB_InputFormatGroup_e720p_50:
			eFmt = BFMT_VideoFmt_e720p_50Hz;
			break;
		case BVDB_InputFormatGroup_e720p_60:
			eFmt = BFMT_VideoFmt_e720p;
			break;
		case BVDB_InputFormatGroup_e1080i_50:
		case BVDB_InputFormatGroup_e1440_1080i_50:
			eFmt = BFMT_VideoFmt_e1080i_50Hz;
			break;
		case BVDB_InputFormatGroup_e1080i_60:
		case BVDB_InputFormatGroup_e1440_1080i_60:
			eFmt = BFMT_VideoFmt_e1080i;
			break;
		case BVDB_InputFormatGroup_e1080p_24:
		case BVDB_InputFormatGroup_e1440_1080p_24:
			eFmt = BFMT_VideoFmt_e1080p_24Hz;
			break;
		case BVDB_InputFormatGroup_e1080p_25:
		case BVDB_InputFormatGroup_e1440_1080p_25:
			eFmt = BFMT_VideoFmt_e1080p_25Hz;
			break;
		case BVDB_InputFormatGroup_e1080p_30:
		case BVDB_InputFormatGroup_e1440_1080p_30:
			eFmt = BFMT_VideoFmt_e1080p_30Hz;
			break;
		case BVDB_InputFormatGroup_e1080p_50:
			eFmt = BFMT_VideoFmt_e1080p_50Hz;
			break;
		case BVDB_InputFormatGroup_e1080p_60:
			eFmt = BFMT_VideoFmt_e1080p;
			break;
		case BVDB_InputFormatGroup_e1024_768_60_PC:
			eFmt = BFMT_VideoFmt_eDVI_1024x768p;
			break;
		case BVDB_InputFormatGroup_e1024_768_85_PC:
			eFmt = BFMT_VideoFmt_eDVI_1024x768p_85Hz;
			break;
		case BVDB_InputFormatGroup_e1366_768_60_PC:
			eFmt = BFMT_VideoFmt_eDVI_1366x768p_60Hz;
			break;
		case BVDB_InputFormatGroup_e1366_768_85_PC:
			eFmt = BFMT_VideoFmt_eDVI_1280x768p_85Hz;
			break;
		case BVDB_InputFormatGroup_e1600_1200_60_PC:
			eFmt = BFMT_VideoFmt_eDVI_1600x1200p_60Hz;
			break;
		case BVDB_InputFormatGroup_e1400_1050_75_PC:
			eFmt = BFMT_VideoFmt_eDVI_1400x1050p_75Hz;
			break;
		case BVDB_InputFormatGroup_e1400_1050_60_PC:
			eFmt = BFMT_VideoFmt_eDVI_1400x1050p_60Hz;
			break;
		case BVDB_InputFormatGroup_e1920_1080_60_PC:
			eFmt = BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red;
			break;
		case BVDB_InputFormatGroup_eAdcCalibration:
		case BVDB_InputFormatGroup_eJpeg_540:
		case BVDB_InputFormatGroup_eJpeg_1080:
#if (BCHP_VER == BCHP_VER_A0)

			if ((stUsageMode.eDisplay == BVDB_Display_ePrimary50) ||
				(stUsageMode.eDisplay == BVDB_Display_eSecondary50))
#else
			if ((stUsageMode.stDisplay.eType == BVDB_Display_ePrimary50) ||
				(stUsageMode.stDisplay.eType == BVDB_Display_eSecondary50))

#endif
				eFmt = BFMT_VideoFmt_ePAL_G;
			else
				eFmt = BFMT_VideoFmt_eNTSC;
			break;
		case BVDB_InputFormatGroup_eInvalid:
		default:
			printf("Unknown format\n");
	}
#endif

	BTST_P_ValidateAndClip(pContext, &stUsageMode, eFmt);
}


void BTST_P_GetVideoDisplayMode
	( BTST_P_Context      *pContext )
{
	BVDB_UsageMode stUsageMode;
	uint32_t ulSrcH, ulSrcW, ulSrcClipH, ulSrcClipW, ulDstH, ulDstW, ulConfigId;
	uint32_t eSrcAr;
	BFMT_AspectRatio eDisplayedAr;
	uint32_t uiSampleAspectRatioX, uiSampleAspectRatioY;
	uint32_t eArCorrection;
	uint32_t ulPbp;

	BVDB_VideoDisplayMode *pstDispMode = (BVDB_VideoDisplayMode *)BKNI_Malloc(sizeof(BVDB_VideoDisplayMode));

	printf("Enter config ID : ");
	BTST_P_Scanf(pContext, "%u", &ulConfigId);

	printf("Enter usage mode ID : ");
	BTST_P_Scanf(pContext, "%s", stUsageMode.acId);

	if (BVDB_GetVdbEntryById(pContext->hVdb, &stUsageMode) == BERR_SUCCESS)
	{

		printf("Is window used for PBP (0=n/y=1) : ");
		BTST_P_Scanf(pContext, "%u", &ulPbp);

		printf("Enter source w and h : ");
		BTST_P_Scanf(pContext, "%u", &ulSrcW);
		BTST_P_Scanf(pContext, "%u", &ulSrcH);

		printf("Enter dest w and h : ");
		BTST_P_Scanf(pContext, "%u", &ulDstW);
		BTST_P_Scanf(pContext, "%u", &ulDstH);

		printf("Enter source clip amounts for w and h : ");
		BTST_P_Scanf(pContext, "%u", &ulSrcClipW);
		BTST_P_Scanf(pContext, "%u", &ulSrcClipH);

		printf("Enter source's frame aspect ratio: Unknown (0), Square pixel (1), 4:3(2), 16:9(3), 2.21:1(4), 15:9(5), SAR(6): ");
		BTST_P_Scanf(pContext, "%u", &eSrcAr);

		printf("Enter pixel aspect ratio x and y :");
		BTST_P_Scanf(pContext, "%u", &uiSampleAspectRatioX);
		BTST_P_Scanf(pContext, "%u", &uiSampleAspectRatioY);

		printf("Enter aspect ratio correction : All Source (4), All Dest(5) Bypass(7) :");
		BTST_P_Scanf(pContext, "%u", &eArCorrection);

		if (BVDB_GetVideoDisplayMode( pContext->hVdb, ulConfigId, &stUsageMode.stDisplay,
				stUsageMode.eWindow, (bool)ulPbp, stUsageMode.eInput,
				ulSrcH, ulSrcW, ulSrcClipH, ulSrcClipW, ulDstH, ulDstW,
				(BFMT_AspectRatio)eSrcAr, (uint16_t)uiSampleAspectRatioX,
				(uint16_t)uiSampleAspectRatioY, (BVDB_AspectRatio)eArCorrection,
				pstDispMode, &eDisplayedAr ) == BERR_SUCCESS)
		{
			BVDB_DisplaySourceAs eDisplaySourceAs;
			printf("VideoDisplayMode = 0x%x\n", *((uint32_t *)pstDispMode));
			printf("Aspect Ratio = %s\n", (eDisplayedAr == BFMT_AspectRatio_e16_9) ? "16:9" :
											(eDisplayedAr == BFMT_AspectRatio_e4_3) ? "4:3" :
											"other");

			BVDB_GetDisplaySourceAs(pContext->hVdb, pstDispMode,
				ulSrcH, ulSrcW, &stUsageMode.stDisplay,
				eSrcAr, uiSampleAspectRatioX, uiSampleAspectRatioY,
				eArCorrection, &eDisplaySourceAs);
			printf("Display Source as = %s\n", (eDisplaySourceAs == BVDB_DisplaySourceAs_eFull) ? "FULL":
									(eDisplaySourceAs == BVDB_DisplaySourceAs_eLetterbox) ? "LBOX" :
									(eDisplaySourceAs == BVDB_DisplaySourceAs_ePillarbox) ? "PBOX" :
									"other");
		}
		else
		{
			printf("Cannot find matching video display mode for %s\n", stUsageMode.acId);
		}
	}
	else
	{
		printf("Failed to find usage mode %s\n", stUsageMode.acId);
	}

	BKNI_Free(pstDispMode);
}


void BTST_P_GetConfigMemAlloc
	( BTST_P_Context      *pContext )
{
	uint32_t ulConfigId, ulWin, ulInput;
	BVDB_Heap_Settings stSettings;
	uint32_t ulHeapSize;

	BSTD_UNUSED(pContext);

	printf("Enter config id: ");
	BTST_P_Scanf(pContext, "%u", &ulConfigId);
	printf("Enter window (0=main, 1=pip, 2=monitor): ");
	BTST_P_Scanf(pContext, "%u", &ulWin);
	printf("Enter input (0,1,2,4,5=analog, 3=digital): ");
	BTST_P_Scanf(pContext, "%u", &ulInput);

	if (BVDB_GetMemoryAllocation(
		ulConfigId, (BVDB_Window)ulWin, (BVDB_Input)ulInput,
		&stSettings, &ulHeapSize) != BERR_SUCCESS)
	{
		printf("Failed to obtain config %d memory heap settings for %s window, %s input\n",
		ulConfigId, (ulWin == 0) ? "MAIN" : (ulWin == 1) ? "PIP" : "MONITOR",
		(ulInput == 0) ? "Digital" : "Analog");
	}
	else
	{
		printf("Config %d memory heap settings for %s window, %s input:\n",
		ulConfigId, (ulWin == 0) ? "MAIN" : (ulWin == 1) ? "PIP" : "MONITOR",
		(ulInput == 3) ? "Digital" : "Analog");

		printf("\t2HD buffer count : %d\n", stSettings.ulBufferCnt_2HD);
		printf("\t2HD buffer format : %d\n", stSettings.eBufferFormat_2HD);
		printf("\t2HD pixel format : 0x%x\n", stSettings.ePixelFormat_2HD);
		printf("\t2HD_Pip buffer count : %d\n", stSettings.ulBufferCnt_2HD_Pip);

		printf("\tHD buffer count : %d\n", stSettings.ulBufferCnt_HD);
		printf("\tHD buffer format : %d\n", stSettings.eBufferFormat_HD);
		printf("\tHD pixel format : 0x%x\n", stSettings.ePixelFormat_HD);
		printf("\tHD_Pip buffer count : %d\n", stSettings.ulBufferCnt_HD_Pip);

		printf("\tSD buffer count : %d\n", stSettings.ulBufferCnt_SD);
		printf("\tSD buffer format : %d\n", stSettings.eBufferFormat_SD);
		printf("\tSD pixel format : 0x%x\n", stSettings.ePixelFormat_SD);
		printf("\tSD_Pip buffer count : %d\n", stSettings.ulBufferCnt_SD_Pip);

		printf("\tHeap size in KBytes = %d\n", ulHeapSize);
	}

}


void BTST_P_GetInputFormatGroup
	( BTST_P_Context *pContext )
{
	uint32_t ulW, ulH, ulRate, ulInput;
	int bProg, bJpeg;
	BVDB_InputFormatGroup eInputFormatGroup;

	printf("Enter format width: ");
	scanf ("%u", &ulW);
	printf("Enter format height: ");
	scanf ("%u", &ulH);
	printf("Enter format rate: ");
	scanf ("%u", &ulRate);
	printf("Enter Progressive (1) or Interlaced (0): ");
	scanf ("%d", &bProg);
	printf("Enter Jpeg (1) or no (0): ");
	scanf ("%d", &bJpeg);
	printf("Enter input CVBS/Svideo (0), COMP (1), HDMI (2), MFD (3), VGA (4), 656 (5): ");
	scanf ("%u", &ulInput);

	BVDB_GetInputFormatGroup(pContext->hVdb, ulH, ulW, ulRate,
		(bool)bProg, (bool)bJpeg, (BVDB_Input)ulInput,
		 &eInputFormatGroup);
	if (eInputFormatGroup == BVDB_InputFormatGroup_eInvalid)
		printf("Invalid input format.\n");
	else
		printf("Input format group is %d\n", eInputFormatGroup);
}

#ifdef BVDB_GEN_FILE
#if (BCHP_VER == BCHP_VER_A0)
static void BTST_P_GetUserUsageModeParameters
	( BTST_P_Context  *pContext,
	  BVDB_UsageMode  *pstUsageMode,
	  bool             bQuery )
{
	char *pcStr;
	uint16_t i, ulSize;

	BSTD_UNUSED(pContext);

	pcStr = (char *)BKNI_Malloc(sizeof(char) * 20);
	if (pcStr == NULL)
	{
		printf("Out of memory\n");
		BDBG_ASSERT(pcStr);
		return;
	}

	if(bQuery)
		ulSize = 6;
	else
		ulSize = 20;

	for (i=0; i<ulSize; i++)
	{
		switch(i)
		{
			case 0:
				do
				{
					printf("Enter source format: ");
					scanf ("%s", pcStr);
					pstUsageMode->eSrcFmtSet = BVDB_GetSourceFormatSet(pcStr);
					if (pstUsageMode->eSrcFmtSet == BVDB_SrcFmtSet_eInvalid)
					{
						printf("Invalid source format.\n");
					}
				} while (pstUsageMode->eSrcFmtSet == BVDB_SrcFmtSet_eInvalid);
				break;
			case 1:
				do
				{
					printf("Enter display: ");
					scanf ("%s", pcStr);
					pstUsageMode->eDisplay = BVDB_GetDisplay(pcStr);
					if (pstUsageMode->eDisplay == BVDB_Display_eInvalid)
					{
						printf("Invalid display.\n");
					}
				} while (pstUsageMode->eDisplay == BVDB_Display_eInvalid);
				break;
			case 2:
				do
				{
					printf("Enter window: ");
					scanf ("%s", pcStr);
					pstUsageMode->eWindow = BVDB_GetWindow(pcStr);
					if (pstUsageMode->eWindow == BVDB_Window_eInvalid)
					{
						printf("Invalid window.\n");
					}
				} while (pstUsageMode->eWindow == BVDB_Window_eInvalid);
				break;
			case 3:
				do
				{
					printf("Enter input type: ");
					scanf ("%s", pcStr);
					pstUsageMode->eInput = BVDB_GetInput(pcStr);
					if (pstUsageMode->eInput == BVDB_Input_eInvalid)
					{
						printf("Invalid input type.\n");
					}
				} while (pstUsageMode->eInput == BVDB_Input_eInvalid);
				break;
			case 4:
				do
				{
					printf("Enter video display mode: ");
					scanf ("%s", pcStr);
					pstUsageMode->stDisplayMode = BVDB_GetVideoDisplayMode(pcStr);
					if (pstUsageMode->stDisplayMode.ulUnknown)
					{
						printf("Unknown video display mode.\n");
					}
				} while (pstUsageMode->stDisplayMode.ulUnknown);
				break;
			case 5:
				printf("Enter OSD: ");
				scanf ("%s", pcStr);
				pstUsageMode->stOsd.eOsd = BVDB_GetOsd(pcStr);
				break;
			case 6:
				printf("Enter 3D Comb (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->e3dComb = BVDB_GetState(pcStr);
				break;
			case 7:
				printf("Enter Oversampling (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->eOversample = BVDB_GetState(pcStr);
				break;
			case 8:
				printf("Enter MNR/BNR (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->eMnrBnr = BVDB_GetState(pcStr);
				break;
			case 9:
				printf("Enter DCR (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->eDcr = BVDB_GetState(pcStr);
				break;
			case 10:
				printf("Enter ANR (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->stAnr.eState = BVDB_GetState(pcStr);
				i++;
				if (pstUsageMode->stAnr.eState == BVDB_State_eYes) /* ANR format */
				{
					do
					{
						char acTemp[10];
						printf("Enter ANR pixel bits: ");
						BTST_P_Scanf(pContext, "%s", pcStr);
						strcat(pcStr, "-bit ");
						printf("Enter ANR pixel format: ");
						BTST_P_Scanf(pContext, "%s", acTemp);
						strcat(pcStr, acTemp);
				 		pstUsageMode->stAnr.ePixelFormat = BVDB_GetPixelFormat(pcStr);
						if (pstUsageMode->stAnr.ePixelFormat == BVDB_PixelFmt_eInvalid)
							printf("Invalid ANR pixel format.\n");
					} while (pstUsageMode->stAnr.ePixelFormat == BVDB_PixelFmt_eInvalid);
				}
				else
				{
					pstUsageMode->stAnr.ePixelFormat = BVDB_PixelFmt_eInvalid;
				}
				break;
			case 12:
				printf("Enter CAP/VFD (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->stCapVfd.eState = BVDB_GetState(pcStr);
				i++;
				if (pstUsageMode->stCapVfd.eState == BVDB_State_eYes) /* CAP/VFD format */
				{
					do
					{
						char acTemp[10];
						printf("Enter CAP/VFD pixel bits: ");
						BTST_P_Scanf(pContext, "%s", pcStr);
						strcat(pcStr, "-bit ");
						printf("Enter CAP/VFD pixel format: ");
						BTST_P_Scanf(pContext, "%s", acTemp);
						strcat(pcStr, acTemp);
						pstUsageMode->stCapVfd.ePixelFormat = BVDB_GetPixelFormat(pcStr);
						if (pstUsageMode->stCapVfd.ePixelFormat == BVDB_PixelFmt_eInvalid)
								printf("Invalid CAP/VFD pixel format.\n");
					} while (pstUsageMode->stCapVfd.ePixelFormat == BVDB_PixelFmt_eInvalid);
				}
				else
				{
					pstUsageMode->stCapVfd.ePixelFormat = BVDB_PixelFmt_eInvalid;
				}
				break;
			case 14:
				printf("Enter MAD (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->stMad.eState = BVDB_GetState(pcStr);
				i+=2;
				if (pstUsageMode->stMad.eState == BVDB_State_eYes)
				{
					do
					{
						char acTemp[10];
						printf("Enter number of MAD fields: ");
						BTST_P_Scanf(pContext, "%s", pcStr);
						strcat(pcStr, "-field");
						pstUsageMode->stMad.eFieldStore = BVDB_GetDeinterlacerFieldStoreCnt(pcStr);

						printf("Enter MAD pixel bits: ");
						BTST_P_Scanf(pContext, "%s", pcStr);
						strcat(pcStr, "-bit ");
						printf("Enter MAD pixel format: ");
						BTST_P_Scanf(pContext, "%s", acTemp);
						strcat(pcStr, acTemp);

						pstUsageMode->stMad.ePixelFormat = BVDB_GetPixelFormat(pcStr);
						if (pstUsageMode->stMad.ePixelFormat == BVDB_PixelFmt_eInvalid)
							printf("Invalid MAD pixel format.\n");

						if (pstUsageMode->stMad.eFieldStore == BVDB_FieldStore_eNotApplicable)
							printf("Invalid number of MAD fields/\n");

					} while ((pstUsageMode->stMad.ePixelFormat == BVDB_PixelFmt_eInvalid)  ||
							 (pstUsageMode->stMad.eFieldStore == BVDB_FieldStore_eNotApplicable));
				}
				else if (pstUsageMode->stMad.eState == BVDB_State_eNo || BVDB_State_eNotApplicable)
				{
					pstUsageMode->stMad.eFieldStore = BVDB_FieldStore_eNotApplicable;
					pstUsageMode->stMad.ePixelFormat = BVDB_PixelFmt_eInvalid;
				}
				break;
			case 17:
				printf("Enter RTS set: ");
				BTST_P_Scanf(pContext, "%s", pcStr);
				pstUsageMode->eRtsSet = BVDB_GetRtsSet(pcStr);
				break;
			case 18:
				printf("Enter tolerance: ");
				BTST_P_Scanf(pContext, "%s", pcStr);
				pstUsageMode->ulSclCapTolerance = BVDB_GetValue(pcStr);
				break;
			case 19:
				printf("Enter bias: ");
				BTST_P_Scanf(pContext, "%s", pcStr);
				pstUsageMode->eSclCapBias = BVDB_GetSclCapBias(pcStr);
				break;
		}
	}

}

#else
static void BTST_P_GetUserUsageModeParameters
	( BTST_P_Context  *pContext,
	  BVDB_UsageMode  *pstUsageMode,
	  bool             bQuery )
{
	char *pcStr;
	uint16_t i, ulSize;


	pcStr = (char *)BKNI_Malloc(sizeof(char) * 20);
	if (pcStr == NULL)
	{
		printf("Out of memory\n");
		BDBG_ASSERT(pcStr);
		return;
	}

	if(bQuery)
		ulSize = 7;
	else
		ulSize = 24;

	for (i=0; i<ulSize; i++)
	{
		switch(i)
		{
			case 0:
				do
				{
					printf("Enter input format: ");
					scanf ("%s", pcStr);
					pstUsageMode->eInputFormatGroup = BVDB_Parser_GetInputFormat(pcStr);
					if (pstUsageMode->eInputFormatGroup == BVDB_InputFormatGroup_eInvalid)
					{
						uint32_t ulW, ulH, ulRate;
						int bProg, bPc, bJpeg;

						printf("Can't find format %s. ", pcStr);
						printf("Enter format width: ");
						scanf ("%u", &ulW);
						printf("Enter format height: ");
						scanf ("%u", &ulH);
						printf("Enter format rate: ");
						scanf ("%u", &ulRate);
						printf("Enter Interlaced 0) or Progressive (1): ");
						scanf ("%d", &bProg);
						printf("Enter Jpeg (1) or no (0): ");
						scanf ("%d", &bJpeg);
						printf("Enter Pc(1) or not (0): ");
						scanf ("%d", &bPc);

						BVDB_GetInputFormatGroup(pContext->hVdb, ulH, ulW, ulRate,
							bProg, bPc, bJpeg, &pstUsageMode->eInputFormatGroup);
						if (pstUsageMode->eInputFormatGroup == BVDB_InputFormatGroup_eInvalid)
							printf("Invalid input format.\n");
					}
				} while (pstUsageMode->eInputFormatGroup == BVDB_InputFormatGroup_eInvalid);
				break;
			case 1:
				do
				{
					printf("Enter display: ");
					scanf ("%s", pcStr);

					pstUsageMode->stDisplay.eType = BVDB_Parser_GetDisplayType(pcStr);
					if (pstUsageMode->stDisplay.eType == BVDB_Display_eInvalid)
					{
						printf("Invalid display.\n");
					}
				} while (pstUsageMode->stDisplay.eType == BVDB_Display_eInvalid);
				break;
			case 2:
				do
				{
					printf("Enter display resolution: ");
					scanf ("%s", pcStr);

					pstUsageMode->stDisplay.eResolution = BVDB_Parser_GetOutputResolution(pcStr);
					if (pstUsageMode->stDisplay.eResolution == BVDB_OutputResolution_eInvalid)
					{
						printf("Invalid display resolution.\n");
					}
				} while (pstUsageMode->stDisplay.eResolution == BVDB_OutputResolution_eInvalid);
				break;
			case 3:
				do
				{
					printf("Enter display's aspect ratio: ");
					scanf ("%s", pcStr);

					pstUsageMode->stDisplay.eAspectRatio = BVDB_Parser_GetAspectRatio(pcStr);
					if (pstUsageMode->stDisplay.eAspectRatio == BVDB_AspectRatio_eInvalid)
					{
						printf("Invalid aspect ratio.\n");
					}
				} while (pstUsageMode->stDisplay.eAspectRatio == BVDB_AspectRatio_eInvalid);
				break;
			case 4:
				do
				{
					printf("Enter window: ");
					scanf ("%s", pcStr);
					pstUsageMode->eWindow = BVDB_Parser_GetWindow(pcStr);
					if (pstUsageMode->eWindow == BVDB_Window_eInvalid)
					{
						printf("Invalid window.\n");
					}
				} while (pstUsageMode->eWindow == BVDB_Window_eInvalid);
				break;
			case 5:
				do
				{
					printf("Enter input type: ");
					scanf ("%s", pcStr);
					pstUsageMode->eInput = BVDB_Parser_GetInput(pcStr);
					if (pstUsageMode->eInput == BVDB_Input_eInvalid)
					{
						printf("Invalid input type.\n");
					}
				} while (pstUsageMode->eInput == BVDB_Input_eInvalid);
				break;
			case 6:
				do
				{
					printf("Enter video display mode: ");
					scanf ("%s", pcStr);
					pstUsageMode->stDisplayMode = BVDB_Parser_GetVideoDisplayMode(pcStr);
					if (pstUsageMode->stDisplayMode.ulUnknown)
					{
						printf("Unknown video display mode.\n");
					}
				} while (pstUsageMode->stDisplayMode.ulUnknown);
				break;
			case 7:
				printf("Enter OSD: ");
				scanf ("%s", pcStr);
				pstUsageMode->stOsd.eOsd = BVDB_Parser_GetOsd(pcStr);
				break;
			case 8:
				printf("Enter 3D Comb (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->e3dComb = BVDB_Parser_GetState(pcStr);
				break;
			case 9:
				printf("Enter Oversampling (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->eOversample = BVDB_Parser_GetState(pcStr);
				break;
			case 10:
				printf("Enter MNR/BNR (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->eMnrBnr = BVDB_Parser_GetState(pcStr);
				break;
			case 11:
				printf("Enter DCR (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->eDcr = BVDB_Parser_GetState(pcStr);
				break;
			case 12:
				printf("Enter ANR (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->stAnr.eState = BVDB_Parser_GetState(pcStr);
				i++;
				if (pstUsageMode->stAnr.eState == BVDB_State_eYes) /* ANR format */
				{
					do
					{
						char acTemp[10];
						printf("Enter ANR pixel bits: ");
						BTST_P_Scanf(pContext, "%s", pcStr);
						strcat(pcStr, "-bit ");
						printf("Enter ANR pixel format: ");
						BTST_P_Scanf(pContext, "%s", acTemp);
						strcat(pcStr, acTemp);
				 		pstUsageMode->stAnr.ePixelFormat = BVDB_Parser_GetPixelFormat(pcStr);
						if (pstUsageMode->stAnr.ePixelFormat == BVDB_PixelFmt_eInvalid)
							printf("Invalid ANR pixel format.\n");
					} while (pstUsageMode->stAnr.ePixelFormat == BVDB_PixelFmt_eInvalid);
				}
				else
				{
					pstUsageMode->stAnr.ePixelFormat = BVDB_PixelFmt_eInvalid;
				}
				break;
			case 14:
				printf("Enter CAP/VFD (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->stCapVfd.eState = BVDB_Parser_GetState(pcStr);
				i+=2;

				if (pstUsageMode->stCapVfd.eState == BVDB_State_eYes) /* CAP/VFD format */
				{
					do
					{
						char acTemp[10];
						printf("Enter CAP/VFD pixel bits: ");
						BTST_P_Scanf(pContext, "%s", pcStr);
						strcat(pcStr, "-bit ");
						printf("Enter CAP/VFD pixel format: ");
						BTST_P_Scanf(pContext, "%s", acTemp);
						strcat(pcStr, acTemp);
						pstUsageMode->stCapVfd.ePixelFormat = BVDB_Parser_GetPixelFormat(pcStr);
						if (pstUsageMode->stCapVfd.ePixelFormat == BVDB_PixelFmt_eInvalid)
								printf("Invalid CAP/VFD pixel format.\n");
					} while (pstUsageMode->stCapVfd.ePixelFormat == BVDB_PixelFmt_eInvalid);
				}
				else
				{
					pstUsageMode->stCapVfd.ePixelFormat = BVDB_PixelFmt_eInvalid;
				}

				printf("Enter CAP/VFD reserved : ");
				scanf ("%s", pcStr);
				pstUsageMode->stCapVfd.ulReserved = BVDB_Parser_GetValue(pcStr);

				break;
			case 17:
				printf("Enter MAD (n/a, no, or yes): ");
				scanf ("%s", pcStr);
				pstUsageMode->stMad.eState = BVDB_Parser_GetState(pcStr);
				i+=3;

				if (pstUsageMode->stMad.eState == BVDB_State_eYes)
				{
					do
					{
						char acTemp[10];
						printf("Enter number of MAD fields: ");
						BTST_P_Scanf(pContext, "%s", pcStr);
						strcat(pcStr, "-field");
						pstUsageMode->stMad.eFieldStore = BVDB_Parser_GetDeinterlacerFieldStoreCnt(pcStr);

						printf("Enter MAD pixel bits: ");
						BTST_P_Scanf(pContext, "%s", pcStr);
						strcat(pcStr, "-bit ");
						printf("Enter MAD pixel format: ");
						BTST_P_Scanf(pContext, "%s", acTemp);
						strcat(pcStr, acTemp);

						pstUsageMode->stMad.ePixelFormat = BVDB_Parser_GetPixelFormat(pcStr);
						if (pstUsageMode->stMad.ePixelFormat == BVDB_PixelFmt_eInvalid)
							printf("Invalid MAD pixel format.\n");

						if (pstUsageMode->stMad.eFieldStore == BVDB_FieldStore_eNotApplicable)
							printf("Invalid number of MAD fields/\n");

					} while ((pstUsageMode->stMad.ePixelFormat == BVDB_PixelFmt_eInvalid)  ||
							 (pstUsageMode->stMad.eFieldStore == BVDB_FieldStore_eNotApplicable));
				}
				else if (pstUsageMode->stMad.eState == BVDB_State_eNo || BVDB_State_eNotApplicable)
				{
					pstUsageMode->stMad.eFieldStore = BVDB_FieldStore_eNotApplicable;
					pstUsageMode->stMad.ePixelFormat = BVDB_PixelFmt_eInvalid;
				}

				printf("Enter MAD reserved : ");
				scanf ("%s", pcStr);
				pstUsageMode->stMad.ulReserved = BVDB_Parser_GetValue(pcStr);

				break;
			case 21:
				printf("Enter RTS set: ");
				BTST_P_Scanf(pContext, "%s", pcStr);
				pstUsageMode->eRtsSet = BVDB_Parser_GetRtsSet(pcStr);
				break;
			case 22:
				printf("Enter tolerance: ");
				BTST_P_Scanf(pContext, "%s", pcStr);
				pstUsageMode->ulSclCapTolerance = BVDB_Parser_GetValue(pcStr);
				break;
			case 23:
				printf("Enter bias: ");
				BTST_P_Scanf(pContext, "%s", pcStr);
				pstUsageMode->eSclCapBias = BVDB_Parser_GetSclCapBias(pcStr);
				break;
		}
	}

}
#endif /* BCHP_VER == BCHP_VER_A0 */
#endif
