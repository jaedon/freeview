/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   pumper_soc_brcm.c  		$
 * Version:		$Revision:   1.0  			$
 * Original Author:	YoungKi Kim				$
 * Current Author:	$Author:   YoungKi Kim 			$
 * Date:		$Date:  $
 * File Description:
 * Module:
 * Remarks:
 */


/**
 * @ingroup Pumper
 */

/**
 * @author YoungKi Kim(ykkim3@humaxdigital.com)
 * @date 18 April 2013
 */

/**
 * @note
 * Copyright (C) 2013 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

/*@{*/

/**
 * @file pumper_soc_brcm.c
 */

/*******************************************************************/
/* Copyright (c) 2007 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#include "nexus_types.h"
#include "nexus_playpump.h"
#include "bstd.h"                       /* brcm includes */
#include "bdbg.h"
#include "bkni.h"

#include "cso.h"
#include "pumper.h"

/* End Including Headers */

#ifndef UNUSED_PARAM
#define UNUSED_PARAM(x)  ((void)x)
#endif

#if 0
static          NEXUS_TransportType P_PUMPER_GetNexusTransportType(PUMPER_PUMPING_Type_t tPumpingType);
#endif

#if 0
static NEXUS_TransportType P_PUMPER_GetNexusTransportType (PUMPER_PUMPING_Type_t tPumpingType)
{
	NEXUS_TransportType     tRtnNexusTransportType = NEXUS_TransportType_eUnknown;
	switch (tPumpingType)
	{
		case PUMPER_PUMPING_TYPE_TS :
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >=0) && (CALYPSO_VERSION_MICRO >=2) ) \
			|| ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >=1) ) \
			|| (CALYPSO_VERSION_MAJOR > 1)

		case PUMPER_PUMPING_TYPE_TS192 :
#endif
			tRtnNexusTransportType = NEXUS_TransportType_eTs;
			break;
		case PUMPER_PUMPING_TYPE_PES :
			tRtnNexusTransportType = NEXUS_TransportType_eMpeg2Pes;
			break;
		case PUMPER_PUMPING_TYPE_ES :
			tRtnNexusTransportType = NEXUS_TransportType_eEs;
			break;
		default :
//			PRINT_ERROR(("[%s](L:%d) error - invalid pumping type !!!\n", __FUNCTION__, __LINE__));
			break;
	}
	return tRtnNexusTransportType;
}
#endif

int DI_MI_PUMPER_SOC_Setup(PUMPER_Context_t* pPumperContext)
{
	NEXUS_PlaypumpSettings			stPlaypumpSettings;
	NEXUS_Error nResult;
	if(pPumperContext == NULL || pPumperContext->pPlaypumpHandle == NULL)
	{
		return -1;
	}
	(void)NEXUS_Playpump_GetSettings(pPumperContext->pPlaypumpHandle, &stPlaypumpSettings);

	/* Pump의 TransportType는 PidChannel이 Open되고 난 이후에는 NEXUS_Playpump_SetSettings에서 예외처리 되어 변경 하지 못한다.
		만약 Pump의 TransportType를 변경이 필요하다면
		OCTO의 di_calypso에서 DI_CSO_SetInputStreamType()과 DRV_CSO_GetPlaypump(Audio/Video)PidChannel()을 참고 해서 수정 해야 한다.*/
	//stPlaypumpSettings.transportType		= P_PUMPER_GetNexusTransportType(pPumperContext->tPumpingType);

#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >=0) && (CALYPSO_VERSION_MICRO >=2) ) \
			|| ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >=1) ) \
			|| (CALYPSO_VERSION_MAJOR > 1)
	if(pPumperContext->tPumpingType == PUMPER_PUMPING_TYPE_TS192)
	{
		stPlaypumpSettings.timestamp.type = NEXUS_TransportTimestampType_eMod300;
		stPlaypumpSettings.timestamp.timebase = NEXUS_Timebase_eInvalid;
		stPlaypumpSettings.timestamp.pacing = false;
		stPlaypumpSettings.timestamp.pacingMaxError = 2048;
	}
#endif
	nResult = NEXUS_Playpump_SetSettings(pPumperContext->pPlaypumpHandle, &stPlaypumpSettings);
	if(nResult != NEXUS_SUCCESS)
	{
//		PRINT_ERROR(("[%s][%d] NEXUS_Playpump_SetSettings failed!\n", __FUNCTION__,__LINE__));
		return -1;
	}
	return 0;
}

int DI_MI_PUMPER_SOC_Finalize(PUMPER_Context_t* pPumperContext)
{
	if(pPumperContext == NULL || pPumperContext->pPlaypumpHandle == NULL)
	{
		return -1;
	}
	
	return 0;
}

int DI_MI_PUMPER_SOC_GetBuffer(PUMPER_Context_t* pPumperContext, void **pGetBuf, size_t *pBufferSize)
{
	if(pPumperContext == NULL || pPumperContext->pPlaypumpHandle == NULL)
	{
		return -1;
	}
	if ( NEXUS_Playpump_GetBuffer (pPumperContext->pPlaypumpHandle, pGetBuf, pBufferSize) )
	{
		return -1;
	}
	return 0;
}

int DI_MI_PUMPER_SOC_WriteComplete(PUMPER_Context_t* pPumperContext, unsigned int unWriteSize)
{
	NEXUS_Error nResult;
	if(pPumperContext == NULL || pPumperContext->pPlaypumpHandle == NULL)
	{
		return -1;
	}
	nResult = NEXUS_Playpump_WriteComplete(pPumperContext->pPlaypumpHandle, 0, unWriteSize);
	if(nResult != NEXUS_SUCCESS)
	{
//		PRINT_ERROR(("[%s][%d] NEXUS_Playpump_WriteComplete failed!\n", __FUNCTION__,__LINE__));
		return -1;
	}
	return 0;
}

int DI_MI_PUMPER_SOC_Start(PUMPER_Context_t* pPumperContext)
{
	NEXUS_Error nResult;
	NEXUS_PlaypumpStatus bStatus;

	if(pPumperContext == NULL || pPumperContext->pPlaypumpHandle == NULL)
	{
		return -1;
	}
	/* Check pumping task */
	/* ... */
	nResult = NEXUS_Playpump_GetStatus(pPumperContext->pPlaypumpHandle, &bStatus);
	if(nResult != NEXUS_SUCCESS)
	{
//		PrintError("[%s][%d]  Error(0x%x) in NEXUS_Playpump_GetStatus\n\r", nResult);
		return (-1);
	}
	if(bStatus.started == false)
	{
		nResult = NEXUS_Playpump_Start(pPumperContext->pPlaypumpHandle);
		if(nResult != NEXUS_SUCCESS)
		{
//			PRINT_ERROR(("[%s][%d] NEXUS_Playpump_Start failed!\n", __FUNCTION__,__LINE__));
			return (-1);
		}
	}
	nResult = NEXUS_Playpump_SetPause(pPumperContext->pPlaypumpHandle, 0);
	if(nResult != NEXUS_SUCCESS)
	{
//		PRINT_ERROR(("[%s][%d] NEXUS_Playpump_SetPause failed!\n", __FUNCTION__,__LINE__));
		return (-1);
	}
	return 0;
}

int DI_MI_PUMPER_SOC_Stop(PUMPER_Context_t* pPumperContext)
{
	NEXUS_PlaypumpSettings playpumpSettings;

	if(pPumperContext == NULL
		|| pPumperContext->pPlaypumpHandle == NULL)
	{
		return -1;
	}

	(void) NEXUS_Playpump_Stop(pPumperContext->pPlaypumpHandle);

	/* All of playpump settings should be clear to default variables after using.*/
	(void) NEXUS_Playpump_GetDefaultSettings (&playpumpSettings);
	(void) NEXUS_Playpump_SetSettings (pPumperContext->pPlaypumpHandle,&playpumpSettings);

#if defined(CALYPSO_CREATEOWN_RESOURCE)
	(void) NEXUS_Playpump_Close(pPumperContext->pPlaypumpHandle);
#endif
	return 0;
}

int DI_MI_PUMPER_SOC_Flush(PUMPER_Context_t* pPumperContext)
{
	NEXUS_Error nResult;
	if(pPumperContext == NULL || pPumperContext->pPlaypumpHandle == NULL)
	{
		return -1;
	}

	(void)NEXUS_Playpump_Stop(pPumperContext->pPlaypumpHandle);

	nResult = NEXUS_Playpump_Flush(pPumperContext->pPlaypumpHandle);
	if(nResult != NEXUS_SUCCESS)
	{
//		PRINT_ERROR(("[%s][%d] NEXUS_Playpump_Flush failed!\n", __FUNCTION__,__LINE__));
		return (-1);
	}
	return 0;
}

