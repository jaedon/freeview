//#define	CI_HW_TEST
#ifdef	CI_HW_TEST
#else
/* $Header$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_ci.c
// Original Author:
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "linden_trace.h"

#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_ci.h"
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#include "pdriver_cimax.h"
#include "drv_cimax.h"
#else
#include "pdriver.h"
#include "drv_ci.h"
#endif

#include "drv_demux.h"


/*******************************************************************/
/* LOCAL DEFINE													*/
/*******************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_CI

#define CAM_POWER_ON_WAIT_TIME		(500)

/*******************************************************************/
/* TYPE DEFINE													*/
/*******************************************************************/

/*******************************************************************/
/* GLOBAL VARIABLES												*/
/*******************************************************************/
void (*_pfnCiCallbackFunc)(unsigned short, unsigned short);


/*******************************************************************/
/* STATIC VARIABLES												*/
/*******************************************************************/
static DI_CI_CAM_TYPE sCamType[CAM_MAXSLOTID] = {0};

/*******************************************************************/
/* STATIC FUNCTIONS												*/
/*******************************************************************/


/*****************************************************************
 * Function		: DI_CI_GetNumberOfModule
 * Description	: 지원하는 CI Slot 수를 리턴한다.
 * Inputs
 * Outputs
 * Returns		: CAM_MAXSLOTID
 *****************************************************************/
unsigned short DI_CI_GetNumberOfModule (void)
{
	return CAM_MAXSLOTID;
}


/*****************************************************************
 * Function		: DI_CI_RegisterCallback
 * Description	: 캠의 상태에 따라 CI 드라이버로부터 호출되기를 원하는 콜백을 등록하기 위한 함수
 * Inputs
 * Outputs
 * Returns		: DI_ERR_CODE
 *****************************************************************/
DI_ERR_CODE DI_CI_RegisterCallback (void (*CiCallbackFunc)(unsigned short usSlotId, unsigned short flag))
{
	_pfnCiCallbackFunc = CiCallbackFunc;

	return DI_ERR_OK;
}


/*****************************************************************
 * Function		: DI_CI_CheckModule
 * Description	: CAM State (Extract:0/Insert:1/Ready:2)를 리턴함
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_CI_STATE DI_CI_CheckModule (unsigned short usSlotId)
{
	int nDevId;

	if ((nDevId = DRV_CI_CheckSlotId (usSlotId)) < 0)
	{
		return DI_CI_EXTRACTED;
	}

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	if (DRV_CI_GetCAMDetect (nDevId,usSlotId) == CAM_NO_DETECT)
#else
	if (DRV_CI_GetCAMDetect (nDevId) == CAM_NO_DETECT)
#endif
	{
		return DI_CI_EXTRACTED;
	}

	return DI_CI_INSERTED;
}


/*****************************************************************
 * Function		: DI_CI_ResetModule
 * Description	: 캠 모듈을 리셋함
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DI_CI_ResetModule (unsigned short usSlotId)
{
	int nDevId;
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	DRV_Error drvErr = 0;
#endif

	if ((nDevId = DRV_CI_CheckSlotId (usSlotId)) < 0)
	{
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	if (DRV_CI_GetCAMDetect (nDevId,usSlotId) == CAM_NO_DETECT)
#else
	if (DRV_CI_GetCAMDetect (nDevId) == CAM_NO_DETECT)
#endif
	{
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	drvErr = DRV_CI_SWReset_CAM(usSlotId);
	return drvErr;
#else
	/* CAM Module POWER OFF */
	DRV_CI_PowerOff_CAM(nDevId);

	VK_TASK_Sleep (CAM_POWER_ON_WAIT_TIME);

	/* CAM Module POWER ON */
	DRV_CI_PowerOn_CAM(nDevId);

	return DRV_CI_Reset_CAM(nDevId);
#endif
}

/*****************************************************************
 * Function		: DI_CI_PowerOffModule
 * Description	: Stand-by 시 CAM Power off (For Power Saving)
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DI_CI_PowerOffModule (unsigned short usSlotId)
{
	int nDevId;

	if ((nDevId = DRV_CI_CheckSlotId (usSlotId)) < 0)
	{
		return DI_ERR_ERROR;
	}

	/* CAM Module POWER OFF */
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	DRV_CI_PowerOff_CAM(nDevId);
#endif
	return DI_ERR_OK;
}

/*****************************************************************
 * Function		: DI_CI_SetTsPath
 * Description	: bypass or pass-thru 를 지정하기 위한 함수
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DI_CI_SetTsPath (unsigned short usSlotId, DI_CI_PATH path)
{
	int nDevId;
	if ((nDevId = DRV_CI_CheckSlotId (usSlotId)) < 0)
	{
		return DI_ERR_ERROR;
	}

	switch (path)
	{
		case DI_CI_BYPASS:
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
			DRV_DEMUX_CamBypass(usSlotId);
#endif
			// To Do in kraken
			break;

		case DI_CI_PASSTHROUGH:
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
			DRV_DEMUX_CamPassthrough(usSlotId);
#endif
			break;

		default:
			PrintError(("DI_CI_SetTsPath : unknown !!! \n"));
			break;
	}

	return DI_ERR_OK;
}

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
/*****************************************************************
 * Function		: DI_CI_SetExtendedTsPath
 * Description	: bypass or pass-thru 를 지정하기 위한 함수
 * Inputs
 * Outputs
 * Returns
*****************************************************************/
#define	IN_CAM0OUT	5
#define	IN_CAM1OUT	6
#define	OUT_CAMIN	2
#define	OUT_NONE	0xff

DI_ERR_CODE DI_CI_SetExtendedTsPath(unsigned int tunerId, DI_CI_INPUT input, unsigned int slotId, DI_CI_OUTPUT output, DI_CI_PATHWAY pathway )
{

	DRV_Error drvErr = 0,fdrvErr = 0;

	HUINT32 inputSrc = 0, selSlotId = 0xff, outputPort = 0xff;
	HUINT32 sSlotId = 0;
	HUINT32 allCamDetect = 0;
	HUINT8 camDetect = 0;

	HUINT32 resetFIFO0 = 0,resetFIFO1 = 0;

	if((input >= DI_CI_INPUT_MAX)||(slotId >= CAM_MAXSLOTID)||(output >= DI_CI_OUTPUT_MAX)||(pathway >= DI_CI_PATHWAY_MAX))
	{
		return DI_ERR_INVALID_PARAM;
	}

	if(((CAM_MAXSLOTID < 2)&&(pathway == DI_CI_DAISYCHAIN_PASSTHROUGH))||((CAM_MAXSLOTID < 2)&&(pathway == DI_CI_CUSTOM_PASSTHROUGH)))
	{
		return DI_ERR_INVALID_PARAM;
	}

	switch(pathway)
	{
		case DI_CI_NORMAL_BYPASS:
			drvErr = DRV_CI_MakePath(tunerId,input,input,selSlotId,output,output,pathway,&resetFIFO0);
			break;

		case DI_CI_NORMAL_PASSTHROUGH:
			camDetect = DRV_CI_GetCAMDetect (0,slotId);

			if (camDetect == CAM_DETECT)
			{
				drvErr = DRV_CI_MakePath(tunerId,input,input,slotId,output,output,pathway,&resetFIFO0);
			}
			else
			{
				drvErr = DRV_ERR_INITIALIZATION;
			}

			break;

		case DI_CI_DAISYCHAIN_PASSTHROUGH:
			DRV_CI_GetAllCAMDetect (&allCamDetect);

			if(allCamDetect == 1)
			{
				sSlotId = 0;
				outputPort = OUT_CAMIN;
				fdrvErr = DRV_CI_MakePath(tunerId,input,input,sSlotId,outputPort,output,pathway,&resetFIFO0);
				if(fdrvErr == 0)
				{
					sSlotId = 1;
					inputSrc = IN_CAM0OUT;
					drvErr = DRV_CI_MakePath(tunerId,input,inputSrc,sSlotId,output,output,pathway,&resetFIFO1);
				}
				else
				{
					drvErr = fdrvErr;
				}

			}
			else
			{
				drvErr = DRV_ERR_INITIALIZATION;
			}
			break;

		case DI_CI_CUSTOM_PASSTHROUGH:
			DRV_CI_GetAllCAMDetect (&allCamDetect);

			if(allCamDetect == 1)
			{
				fdrvErr = DRV_CI_MakePath(tunerId,input,input,slotId,output,output,pathway,&resetFIFO0);
				if(fdrvErr == 0)
				{
					if(slotId == 0)
					{
						sSlotId = 1;
						inputSrc = input;
						outputPort = OUT_NONE;
					}
					else
					{
						sSlotId = 0;
						inputSrc = input;
						outputPort = OUT_NONE;
					}

					drvErr = DRV_CI_MakePath(tunerId,input,inputSrc,sSlotId,outputPort,output,pathway,&resetFIFO1);
				}
				else
				{
					drvErr = fdrvErr;
				}
			}
			else
			{
				drvErr = DRV_ERR_INITIALIZATION;
			}
			break;

		default:
			drvErr = DRV_ERR_INVALID_PARAMETER;
			break;
	}

	if(drvErr != DRV_OK)
	{
		return DI_ERR_ERROR;
	}

	if(resetFIFO0 == 1)
	{
		drvErr = DRV_CIMAX_ResetFIFO(slotId);
		if(drvErr != DRV_OK)
		{
			return DI_ERR_ERROR;
		}
	}

	return DI_ERR_OK;
}
#endif

/*****************************************************************
 * Function		: DI_CI_SetCAMType
 * Description	: 현재 연결된 CAM type 설정. (v1 or plus)
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DI_CI_SetCAMType(unsigned short usSlotId, DI_CI_CAM_TYPE eCamType)
{
	if(usSlotId >= CAM_MAXSLOTID)
	{
		PrintError("SlotId is out of range.\n");
		return DI_ERR_ERROR;
	}
	if(eCamType >= DI_CI_TYPE_MAX)
	{
		PrintError("Cam type is invalid.\n");
		return DI_ERR_INVALID_PARAM;
	}
	sCamType[usSlotId] = eCamType;
	return DI_ERR_OK;
}

/*****************************************************************
 * Function		: DI_CI_GetStatusCI
 * Description	: 현재 연결된 CAM type 설정. (v1 or plus)
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DI_CI_GetStatusCI(unsigned short usSlotId, DI_CI_CONNECT_STATUS_t *pCIStatus)
{
 	HUINT16	camStatus = 0;

	if(usSlotId >= CAM_MAXSLOTID)
	{
		PrintError("SlotId is out of range.\n");
		return DI_ERR_ERROR;
	}

	if(pCIStatus == NULL)
	{
		return DI_ERR_ERROR;
	}

 	camStatus = DRV_CI_GetCAMStatus(0);
	pCIStatus->connectStatus  = camStatus;
	pCIStatus->connectWay = 0;

	return DI_ERR_OK;
}

DRV_Error DRV_CI_GetCAMType(unsigned short usSlotId, DI_CI_CAM_TYPE *peCamType)
{
	if(usSlotId >= CAM_MAXSLOTID)
	{
		PrintError("SlotId is out of range.\n");
		return DRV_ERR;
	}
	*peCamType = sCamType[usSlotId];
	return DRV_OK;
}

/*********************** End of File ******************************/

#endif
