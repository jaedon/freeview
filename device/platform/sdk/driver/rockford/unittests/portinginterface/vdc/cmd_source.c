/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cmd_source.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 10/11/12 12:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/cmd_source.c $
 * 
 * Hydra_Software_Devel/21   10/11/12 12:08p syang
 * SW7346-572: add long-time-lock for vdc_test
 * 
 * Hydra_Software_Devel/20   10/8/12 3:31p syang
 * SW7346-572: use BTST_P_ApplyChanges to get uniform error handling;
 * avoid holding hVdcCfgLock too long
 * 
 * Hydra_Software_Devel/19   3/5/12 9:26a yuxiaz
 * SW7425-2526: Added tests for runtime capabilities query.
 * 
 * Hydra_Software_Devel/18   8/25/11 10:45a tdo
 * SW7420-2030: Add apply changes error tracking capability
 *
 * Hydra_Software_Devel/17   12/21/10 10:56a yuxiaz
 * SW7422-34: Added test for BVDC_Source_SetOrientation and
 * BVDC_Source_GetOrientation.
 *
 * Hydra_Software_Devel/16   7/10/09 5:54p rpan
 * PR55188: Convert scanf() to BTST_P_Scanf().
 *
 * Hydra_Software_Devel/15   1/15/09 5:09p syang
 * PR 50717: allow user to set delay to resume from src-pending
 *
 * Hydra_Software_Devel/14   12/17/08 7:45p pntruong
 * PR50002: Added force source pending for api test.
 *
 * Hydra_Software_Devel/13   6/18/08 12:10p syang
 * PR35549:  add mutex to serialize access to vdc api
 *
 * Hydra_Software_Devel/12   6/16/08 4:37p syang
 * PR35549: have SrcPendingTask exit from event wait before event is
 * destroyed
 *
 * Hydra_Software_Devel/11   6/9/08 4:28p syang
 * PR 39152, PR 39237, PR 35549:  fix problem with srcpending app_auto
 *
 * Hydra_Software_Devel/10   6/9/08 3:11p syang
 * PR 39152, PR 39237, PR 35549: check hSourcePendingEvent !- NULL before
 * destroy it
 *
 * Hydra_Software_Devel/9   6/7/08 5:45p jessem
 * PR 39152, PR 39237, PR 35549: Revised enabling/disabling of source
 * pending and handling of source pending event.
 *
 * Hydra_Software_Devel/8   6/7/08 4:07p jessem
 * PR 39152: Reverted to version 6 version temporarily.
 *
 * Hydra_Software_Devel/7   6/7/08 9:42a jessem
 * PR 39152: Added support for dynamic RTS test.
 *
 * Hydra_Software_Devel/6   5/14/08 12:57p syang
 * PR35549:  continue to resume from src pending as  src callback fires
 * with bSrcPending
 *
 * Hydra_Software_Devel/5   5/12/08 5:15p jessem
 * PR 39237: Fixed compile problems.
 *
 * Hydra_Software_Devel/4   5/12/08 5:09p jessem
 * PR 39237: Added BTST_P_SourcePendingTask.
 *
 * Hydra_Software_Devel/3   4/30/08 5:29p pntruong
 * PR35549: Added api test for source pending.
 *
 * Hydra_Software_Devel/2   10/18/07 12:11a pntruong
 * PR36137: Merged both customer lists.
 *
 * Hydra_Software_Devel/1   10/18/07 12:02a pntruong
 * PR36137: [PC]:Set pc input video format without disable auto format
 * detection.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bvdc.h"
#include "test_util.h"


BDBG_MODULE(BTST);


/* LG: Mapping */
static const BVDC_VideoFmtPair s_aeLgMap1024x768p[] =
{
	{BFMT_VideoFmt_eDVI_1280x768p_75Hz, BFMT_VideoFmt_eDVI_1024x768p_75Hz},
	{BFMT_VideoFmt_eDVI_1280x768p,      BFMT_VideoFmt_eDVI_1024x768p     },
	{BFMT_VideoFmt_eDVI_1360x768p_60Hz, BFMT_VideoFmt_eDVI_1024x768p     }
};

static const BVDC_VideoFmtPair s_aeLgMap1280x768p[] =
{
	{BFMT_VideoFmt_eDVI_1024x768p_75Hz, BFMT_VideoFmt_eDVI_1280x768p_75Hz},
	{BFMT_VideoFmt_eDVI_1024x768p,      BFMT_VideoFmt_eDVI_1280x768p     },
	{BFMT_VideoFmt_eDVI_1360x768p_60Hz, BFMT_VideoFmt_eDVI_1280x768p     }
};

static const BVDC_VideoFmtPair s_aeLgMap1360x768p[] =
{
	{BFMT_VideoFmt_eDVI_1024x768p,      BFMT_VideoFmt_eDVI_1360x768p_60Hz},
	{BFMT_VideoFmt_eDVI_1280x768p,      BFMT_VideoFmt_eDVI_1360x768p_60Hz}
};

/* Sharp: Mapping */
static const BVDC_VideoFmtPair s_aeSharpMap1024x768p[] =
{
	{BFMT_VideoFmt_eDVI_1360x768p_60Hz, BFMT_VideoFmt_eDVI_1024x768p     }
};

static const BVDC_VideoFmtPair s_aeSharpMap1360x768p[] =
{
	{BFMT_VideoFmt_eDVI_1024x768p,      BFMT_VideoFmt_eDVI_1360x768p_60Hz}
};

#define BTST_P_MAKE_REMAP(remap_entry)                                   \
{                                                                        \
	sizeof(remap_entry)/sizeof(BVDC_VideoFmtPair),                       \
	(remap_entry),                                                       \
	(#remap_entry),                                                      \
}

#define BTST_P_REMAP_COUNT \
	(sizeof(s_aReMapList)/sizeof(BTST_P_ReMapEntry))

typedef struct
{
	uint32_t                           ulFormats;
	const BVDC_VideoFmtPair           *aFormats;
	const char                        *pchName;

} BTST_P_ReMapEntry;

static const BTST_P_ReMapEntry s_aReMapList[] =
{
	BTST_P_MAKE_REMAP(s_aeLgMap1024x768p),
	BTST_P_MAKE_REMAP(s_aeLgMap1280x768p),
	BTST_P_MAKE_REMAP(s_aeLgMap1360x768p),
	BTST_P_MAKE_REMAP(s_aeSharpMap1024x768p),
	BTST_P_MAKE_REMAP(s_aeSharpMap1360x768p)
};


/***************************************************************************
 *
 */
void BTST_P_ReMapDetectFmt
	( BTST_P_Context                   *pContext )
{
	uint32_t iChoice;
	uint32_t i, j;
	const BFMT_VideoInfo *pFrFmt;
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	if(!hVidSrc)
	{
		goto done;
	}

	BKNI_Printf("VDEC PC Input format remap!\n");
	for(i = 0; i < BTST_P_REMAP_COUNT; i++)
	{
		BKNI_Printf("\t(%d) %s\n", i, s_aReMapList[i].pchName);
		for(j = 0; j < s_aReMapList[i].ulFormats; j++)
		{
			pFrFmt = BFMT_GetVideoFormatInfoPtr(s_aReMapList[i].aFormats[j].eFmtA);
			BKNI_Printf("\t\t %s\n", pFrFmt->pchFormatStr);
		}
	}
	BKNI_Printf("\t(%d) Disable format remap!\n", i);
	BKNI_Printf("VDEC PC Input format remap selection:");
	BTST_P_Scanf(pContext, "%d", (int*)&iChoice);

	if(iChoice < BTST_P_REMAP_COUNT)
	{
		BVDC_Source_SetReMapFormats(hVidSrc, true,
			s_aReMapList[iChoice].aFormats, s_aReMapList[iChoice].ulFormats);
	}
	else
	{
		BVDC_Source_SetReMapFormats(hVidSrc, false, NULL, 0);
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

void BTST_P_SourcePendingTask
	( BTST_TaskHandle          hTask,
	  void                    *pvParams )
{
	BERR_Code  rc;
	BTST_P_Context *pContext = (BTST_P_Context *)pvParams;

	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	BSTD_UNUSED(hTask);

	while (true)
	{
		rc = BKNI_WaitForEvent(pContext->hSourcePendingEvent, BTST_P_INFITITE_WAIT);
		if (BERR_SUCCESS == rc)
		{
			if (NULL == pContext->hSourcePendingEvent)
			{
				/* must be truning src-pending off */
				printf("   SrcPending task exit\n");
				return;
			}

			BKNI_ResetEvent(pContext->hSourcePendingEvent);

			printf("   Received source pending event signal\n");
			if (pContext->bManualResume == false)
			{
				/* resume */
				printf("   resume after %d milli-seconds\n", pContext->lMilliSeconds);
				BKNI_Sleep(pContext->lMilliSeconds);

				BTST_P_AcquireLongTimeLock(&pContext->stVdcCfgLock);
				
				BVDC_Source_Resume(hVidSrc);
				if(pContext->bApplyImmediately)
				{
					BTST_P_ApplyChanges(pContext);
				}

				BTST_P_ReleaseLongTimeLock(&pContext->stVdcCfgLock);
			}
			else
			{
				printf("   Reconfig BVN and RTS if needed, and must resume manually.\n");
			}
		}
		else
		{
			printf("   src pending WaitForEvent err 0x%80X\n", rc);
		}
	}
}

/***************************************************************************
 *
 */
void BTST_P_SourcePending
	( BTST_P_Context          *pContext )
{
	int iChoice;
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	if(!hVidSrc)
	{
		goto done;
	}

	printf("Enter resume mode (0) VDC_Auto (src pending off), (1) App_Auto, (2) Manual :\n");
	BTST_P_Scanf(pContext, "%d", &iChoice);

	if (iChoice == 0)
	{
		if ((pContext->hSourcePendingEvent != NULL) &&
			(pContext->hSourcePendingTask != NULL))
		{
			BKNI_EventHandle  hEvent;

			hEvent = pContext->hSourcePendingEvent;

			/* before destroy event, have SourcePendingTask to exit from event waiting */
			pContext->hSourcePendingEvent = NULL;
			BKNI_SetEvent(hEvent);
			BKNI_Sleep(10);

			BTST_DestroyTask(pContext->hSourcePendingTask);
			BKNI_DestroyEvent(hEvent);
		}
		pContext->hSourcePendingEvent = NULL;
		pContext->hSourcePendingTask = NULL;
		pContext->bManualResume = false;

		BVDC_Source_SetResumeMode(hVidSrc, BVDC_ResumeMode_eAuto);
		if(pContext->bApplyImmediately)
		{
			BTST_P_ApplyChanges(pContext);
		}
	}
	else
	{
		if ((pContext->hSourcePendingEvent == NULL) &&
			(pContext->hSourcePendingTask == NULL))
		{
			BKNI_CreateEvent(&pContext->hSourcePendingEvent);
			BKNI_ResetEvent(pContext->hSourcePendingEvent);
			BTST_CreateTask(&pContext->hSourcePendingTask, BTST_P_SourcePendingTask, (void*)pContext);
		}

		pContext->bManualResume = (iChoice == 2);
		if (iChoice == 1)
		{
			printf("Enter milli-seconds resume delay after reg shows src active:\n");
			BTST_P_Scanf(pContext, "%d", &iChoice);
			pContext->lMilliSeconds = iChoice;
		}

		BVDC_Source_SetResumeMode(hVidSrc, BVDC_ResumeMode_eManual);
		if(pContext->bApplyImmediately)
		{
			BTST_P_ApplyChanges(pContext);
		}
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_SourceResume
	( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_Resume(hVidSrc);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_ForcePending
	( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_ForcePending(hVidSrc);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_Source3D
	( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	bool               bOverride;
	BFMT_Orientation   eOrientation;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_GetOrientation(hVidSrc, &bOverride, &eOrientation);
	printf("Override source Orientation [Current = %d]: false(0), true(1)", bOverride) ;
	BTST_P_Scanf(pContext, "%d", &bOverride) ;

	if(bOverride)
	{
		printf("Enter new source Orientation [Current = %d]: 2D(0),LR(1),OU(2)", eOrientation) ;
		BTST_P_Scanf(pContext, "%d", &eOrientation) ;
	}
	BVDC_Source_SetOrientation(hVidSrc, bOverride, eOrientation);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_SourceGetCap
	( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Source_Capabilities   stCapabilities;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_GetCapabilities(hVidSrc, &stCapabilities);

	printf("Checking pxlfmt:\n");
	printf("BPXL_eY18_Cr8_Y08_Cb8 is supported %d \n", 
		stCapabilities.pfIsPxlfmtSupported(BPXL_eY18_Cr8_Y08_Cb8));
	printf("BPXL_eCr8_Y18_Cb8_Y08 is supported %d \n", 
		stCapabilities.pfIsPxlfmtSupported(BPXL_eCr8_Y18_Cb8_Y08));
	printf("BPXL_eY18_Cb8_Y08_Cr8 is supported %d \n", 
		stCapabilities.pfIsPxlfmtSupported(BPXL_eY18_Cb8_Y08_Cr8));
	printf("BPXL_eCb8_Y18_Cr8_Y08 is supported %d \n", 
		stCapabilities.pfIsPxlfmtSupported(BPXL_eCb8_Y18_Cr8_Y08));
	printf("BPXL_eY08_Cb8_Y18_Cr8 is supported %d \n", 
		stCapabilities.pfIsPxlfmtSupported(BPXL_eY08_Cb8_Y18_Cr8));
	printf("BPXL_eCb8_Y08_Cr8_Y18 is supported %d \n", 
		stCapabilities.pfIsPxlfmtSupported(BPXL_eCb8_Y08_Cr8_Y18));
	printf("BPXL_eY08_Cr8_Y18_Cb8 is supported %d \n", 
		stCapabilities.pfIsPxlfmtSupported(BPXL_eY08_Cr8_Y18_Cb8));
	printf("BPXL_eCr8_Y08_Cb8_Y18 is supported %d \n", 
		stCapabilities.pfIsPxlfmtSupported(BPXL_eCr8_Y08_Cb8_Y18));

done:
	return;
}

/* end of file */
