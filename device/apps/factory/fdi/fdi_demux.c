/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_demux.h"

#include "di_err.h"
#include "di_demux.h"
#include "di_ci.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FDI_DEMUX_DEVICE_ID 0

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_GetNumOfDevice(unsigned long *pulNumOfDevice)
{
	DI_DEMUX_GetNumOfDemux((HUINT32*)pulNumOfDevice);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_GetCapability(unsigned long ulDeviceId, FDI_DEMUX_CAPABILITY *tpCapability)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_Open(unsigned long ulDeviceId)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_SetPath(unsigned long ulDeviceId, FDI_DEMUX_INPUT_PATH_e ePath, FDI_DEMUX_CI_PATH_e eCiPath)
{
	DI_ERR_CODE eDiErr;
	HINT32 		nCI_Id;
#if defined(FACTORY_USE_CI)
	nCI_Id = 0;
#else
	nCI_Id = -1;
#endif

	eDiErr = DI_DEMUX_SetPath(FDI_DEMUX_DEVICE_ID, DI_DEMUX_INPUT_TUNER, (HUINT32)ePath, DI_DEMUX_OUTPUT_DECODER, 0, nCI_Id);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_DEMUX_SetPath] DI_DEMUX_SetPath Error : %d\n", eDiErr));
		return FDI_ERR;
	}

#if defined(FACTORY_USE_CI)
	if (eCiPath == FDI_DEMUX_CI_PASSTHROUGH)
	{
		eDiErr = DI_CI_SetTsPath(nCI_Id, DI_CI_PASSTHROUGH);
	}
	else	/* FDI_DEMUX_CI_BYPASS */
	{
		eDiErr = DI_CI_SetTsPath(nCI_Id, DI_CI_BYPASS);
	}
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_DEMUX_SetPath] DI_CI_SetTsPath Error : %d, eCiPath(%d)\n", eDiErr, eCiPath));
		return FDI_ERR;
	}
#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_ReleasePath(unsigned long ulDeviceId)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_DEMUX_ReleasePath(FDI_DEMUX_DEVICE_ID);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_DEMUX_ReleasePath] DI_DEMUX_ReleasePath Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_StartPID(unsigned long ulDeviceId, FDI_DEMUX_CHANNELTYPE_e eChannelType, unsigned short usPid, unsigned long *pulChannelId)
{
	DI_ERR_CODE eDiErr;
	Pid_Info ptPidInfo;

	switch(eChannelType)
	{
		case FDI_DEMUX_CHANNELTYPE_VIDEO:
			ptPidInfo.pidBitmask = PID_BITMASK_VIDEO;
			ptPidInfo.usVideoPid = usPid;
			break;
		case FDI_DEMUX_CHANNELTYPE_AUDIO:
			ptPidInfo.pidBitmask = PID_BITMASK_AUDIO_MAIN;
			ptPidInfo.usAudioMainPid = usPid;
			break;
		case FDI_DEMUX_CHANNELTYPE_PCR:
			ptPidInfo.pidBitmask = PID_BITMASK_PCR;
			ptPidInfo.usPCRPid= usPid;
			break;
		default:
			return FDI_ERR;
			break;
	}

	eDiErr = DI_DEMUX_StartPID(FDI_DEMUX_DEVICE_ID, &ptPidInfo);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_DEMUX_StartPID] DDI_DEMUX_DisableChannel Error : %d\n", eDiErr));
		return FDI_ERR;
	}
	*pulChannelId = eChannelType;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_StopPID(unsigned long ulDeviceId, unsigned long ulChannelId)
{
	DI_ERR_CODE eDiErr;
	unsigned long ulPidBitmask;

	switch(ulChannelId)
	{
	case FDI_DEMUX_CHANNELTYPE_VIDEO:
		ulPidBitmask = PID_BITMASK_VIDEO;
		break;
	case FDI_DEMUX_CHANNELTYPE_AUDIO:
		ulPidBitmask = PID_BITMASK_AUDIO_MAIN;
		break;
	case FDI_DEMUX_CHANNELTYPE_PCR:
		ulPidBitmask = PID_BITMASK_PCR;
		break;
	default:
		return FDI_ERR;
		break;
	}

	eDiErr = DI_DEMUX_StopPID(FDI_DEMUX_DEVICE_ID, ulPidBitmask);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_DEMUX_StopPID] DDI_DEMUX_DisableChannel Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

