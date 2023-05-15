/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>

#include <rema_int.h>

#include <di_init.h>
#include <di_err.h>
#include <di_pvr.h>
#include <di_network.h>
#include <di_audio.h>
#include <di_video.h>
#include <di_usb.h>
#include <di_system.h>
#include <di_cm.h>
#include <di_hdmi.h>
#include "rema_MOD.h"
#include "RMOD_DI.h"

#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

typedef struct RMD_ProcStatus_t
{
	Base_MOD_t stMOD;
	DI_SYS_PROCESS_STATUS_TABLE_t *pstProcStatus;

}RMD_ProcStatus_t;
//nima에서 api를 받아서 처리해야 할듯 한데. 흠.
typedef struct RMD_LANStatus_t
{
	Base_MOD_t stMOD;
	DI_NETWORK_DATA_AMOUNT_INFO_t *pstGetStatus;

}RMD_LANStatus_t;
#define ___STATIC_VARIABLES___

RMD_ProcStatus_t g_stProcStatus; 
RMD_LANStatus_t g_stLANStatus; 

#define ___INTERNAL_FUNCTIONS___
static HINT32 RMOD_DI_GetProcessStatus_Init(void)
{
	REM_FUNC_ENTER;

	g_stProcStatus.pstProcStatus = (DI_SYS_PROCESS_STATUS_TABLE_t*) HLIB_STD_MemAlloc(sizeof(DI_SYS_PROCESS_STATUS_TABLE_t));
	if(g_stProcStatus.pstProcStatus)
	{
	}

	REM_FUNC_LEAVE;

	return 0;
}
static HINT32 RMOD_DI_GetProcessStatus_Deinit(void)
{
	REM_FUNC_ENTER;

	if(g_stProcStatus.pstProcStatus)
		HLIB_STD_MemFree(g_stProcStatus.pstProcStatus );

	REM_FUNC_LEAVE;

	return 0;
}

static HINT32 RMOD_DI_GetProcessStatus_Update(void)
{
	DI_ERR_CODE				diErr;

	REM_FUNC_ENTER;

	diErr = DI_SYSTEM_GetProcessStatus(g_stProcStatus.pstProcStatus);

	REM_FUNC_LEAVE;
	return 0;
}


static HINT32 RMOD_DI_GetLANStatus_Init(void)
{
	REM_FUNC_ENTER;

	g_stLANStatus.pstGetStatus = (DI_NETWORK_DATA_AMOUNT_INFO_t*) HLIB_STD_MemAlloc(sizeof(DI_NETWORK_DATA_AMOUNT_INFO_t));
	if(g_stLANStatus.pstGetStatus )
	{
		HxSTD_MemSet(g_stLANStatus.pstGetStatus, 0, sizeof(DI_NETWORK_DATA_AMOUNT_INFO_t));
	}

	REM_FUNC_LEAVE;

	return 0;
}
static HINT32 RMOD_DI_GetLANStatus_Deinit(void)
{
	REM_FUNC_ENTER;

	if(g_stLANStatus.pstGetStatus )
		HLIB_STD_MemFree(g_stLANStatus.pstGetStatus  );

	REM_FUNC_LEAVE;

	return 0;
}

static HINT32 RMOD_DI_GetLANStatus_Update(void)
{
	DI_ERR_CODE				diErr;

	REM_FUNC_ENTER;

	diErr = DI_NETWORK_GetDevDataAmountInfo(0, g_stLANStatus.pstGetStatus );
	if(diErr != DI_ERR_OK)
	{
		RemLOG_Debug("errro \n");
	}
	REM_FUNC_LEAVE;
	return 0;
}
#define ___PUBLIC_FUNCTIONS___
HERROR RMOD_DI_Init(void)
{
	REM_FUNC_ENTER;

	

	g_stProcStatus.stMOD.bIsInit = FALSE;
	g_stProcStatus.stMOD.bIsUpdate = FALSE;
	g_stProcStatus.stMOD.pcbMod_Init = RMOD_DI_GetProcessStatus_Init;
	g_stProcStatus.stMOD.pcbMod_Deinit = RMOD_DI_GetProcessStatus_Deinit;
	g_stProcStatus.stMOD.pcbMod_Update = RMOD_DI_GetProcessStatus_Update;


	g_stLANStatus.stMOD.bIsInit = FALSE;
	g_stLANStatus.stMOD.bIsUpdate = FALSE;
	g_stLANStatus.stMOD.pcbMod_Init = RMOD_DI_GetLANStatus_Init;
	g_stLANStatus.stMOD.pcbMod_Deinit = RMOD_DI_GetLANStatus_Deinit;
	g_stLANStatus.stMOD.pcbMod_Update = RMOD_DI_GetLANStatus_Update;



	
	rema_MOD_QInit(g_stProcStatus);

	DI_NETWORK_Init();
	rema_MOD_QInit(g_stLANStatus);


	REM_FUNC_LEAVE;

	return ERR_OK;
}

void RMOD_DI_Deinit(void)
{
	REM_FUNC_ENTER;

	rema_MOD_QDeinit(g_stProcStatus);
	rema_MOD_QInit(g_stLANStatus);
	REM_FUNC_LEAVE;
	return;
}
HINT32 RMOD_DI_resetUpdate(void)
{
	rema_MOD_QResetUpdate(g_stProcStatus);
	rema_MOD_QResetUpdate(g_stLANStatus);
	return 0 ; 
}
int RMOD_DI_ProcessorInfo(void)
{
	DI_ERR_CODE				diErr;
	DI_SYS_PROCESSOR_ARCH_TABLE_t ProcessorArchtecture;

	diErr = DI_SYSTEM_GetProcessorInfo(&ProcessorArchtecture);
	return diErr;
}



HERROR RMOD_DI_GetProcessStatus_Max(HUINT32 *punMax)
{
	if(!punMax)
		return ERR_FAIL;
	if(rema_MOD_QUpdate(g_stProcStatus))
	{
		*punMax = g_stProcStatus.pstProcStatus->unNumOfEntries;
	}

	return ERR_OK;
}
HERROR RMOD_DI_GetProcessStatus_Pid(HUINT32 unIndex, HUINT32 *punData)
{

	DI_SYS_PROCESS_STATUS_t *pstCurPS = NULL;
	if(!punData)
		return ERR_FAIL;
	
	if(rema_MOD_QUpdate(g_stProcStatus))
	{
		if( unIndex >= g_stProcStatus.pstProcStatus->unNumOfEntries)
			return ERR_FAIL;
		pstCurPS = &g_stProcStatus.pstProcStatus->ProcessStatus[unIndex];

		*punData = pstCurPS->unPid;
	}

	return ERR_OK;
}

HERROR RMOD_DI_GetProcessStatus_Command(HUINT32 unIndex, HCHAR *pBuf, HINT32 nBufLen)
{
	DI_SYS_PROCESS_STATUS_t *pstCurPS = NULL;
	if(!pBuf)
		return ERR_FAIL;

	if(rema_MOD_QUpdate(g_stProcStatus))
	{
		if( unIndex >= g_stProcStatus.pstProcStatus->unNumOfEntries)
			return ERR_FAIL;
		pstCurPS = &g_stProcStatus.pstProcStatus->ProcessStatus[unIndex];

		HxSTD_StrCpy(pBuf, pstCurPS->strCommand);
	}
	return ERR_OK;
}
HERROR RMOD_DI_GetProcessStatus_Size(HUINT32 unIndex, HUINT32 *punData)
{
	DI_SYS_PROCESS_STATUS_t *pstCurPS = NULL;
	if(!punData)
		return ERR_FAIL;
	
	
	if(rema_MOD_QUpdate(g_stProcStatus))
	{
		if(g_stProcStatus.pstProcStatus == NULL)
			return ERR_FAIL;
		if( unIndex >= g_stProcStatus.pstProcStatus->unNumOfEntries)
			return ERR_FAIL;
		pstCurPS = &g_stProcStatus.pstProcStatus->ProcessStatus[unIndex];

		*punData = pstCurPS->unSize;
	}

	return ERR_OK;
}
HERROR RMOD_DI_GetProcessStatus_Priority(HUINT32 unIndex, HUINT32 *punData)
{

	DI_SYS_PROCESS_STATUS_t *pstCurPS = NULL;
	if(!punData)
		return ERR_FAIL;
	
	if(rema_MOD_QUpdate(g_stProcStatus))
	{
		if(g_stProcStatus.pstProcStatus == NULL)
			return ERR_FAIL;
		if( unIndex >= g_stProcStatus.pstProcStatus->unNumOfEntries)
			return ERR_FAIL;
		pstCurPS = &g_stProcStatus.pstProcStatus->ProcessStatus[unIndex];

		*punData = pstCurPS->unPriority;
	}

	return ERR_OK;
}
HERROR RMOD_DI_GetProcessStatus_CPUTime(HUINT32 unIndex, HUINT32 *punData)
{

	DI_SYS_PROCESS_STATUS_t *pstCurPS = NULL;
	if(!punData)
		return ERR_FAIL;
	
	if(g_stProcStatus.pstProcStatus == NULL)
		return ERR_FAIL;
	if( unIndex >= g_stProcStatus.pstProcStatus->unNumOfEntries)
		return ERR_FAIL;
	pstCurPS = &g_stProcStatus.pstProcStatus->ProcessStatus[unIndex];

	*punData = pstCurPS->unCPUTime;

	return ERR_OK;
}
HERROR RMOD_DI_GetProcessStatus_State(HUINT32 unIndex, HCHAR *pBuf, HINT32 nBufLen)
{

	DI_SYS_PROCESS_STATUS_t *pstCurPS = NULL;
	if(!pBuf)
		return ERR_FAIL;

	if(g_stProcStatus.pstProcStatus == NULL)
		return ERR_FAIL;
	if( unIndex >= g_stProcStatus.pstProcStatus->unNumOfEntries)
		return ERR_FAIL;
	pstCurPS = &g_stProcStatus.pstProcStatus->ProcessStatus[unIndex];
	switch(pstCurPS->eState)
	{
	case PROCESS_STATE_RUNNING:
		HxSTD_StrCpy(pBuf, "Running");
		break;
	case PROCESS_STATE_SLEEPING_INTERRUPTABLE:
		HxSTD_StrCpy(pBuf, "Sleeping Interruptable");
		break;
	case PROCESS_STATE_SLEEPING:
		HxSTD_StrCpy(pBuf, "Sleeping");
		break;
	case PROCESS_STATE_ZOMBIE:
		HxSTD_StrCpy(pBuf, "Zombie");
		break;
	case PROCESS_STATE_STOPPED:
		HxSTD_StrCpy(pBuf, "Stopped");
		break;
	case PROCESS_STATE_PAGING:
		HxSTD_StrCpy(pBuf, "Paging");
		break;
	default:
		break;
	}
	return ERR_OK;
}

HERROR RMOD_DI_GetTemperatureSensor_Max(HUINT32 *punMax)
{
	if(!punMax)
		return ERR_FAIL;

	// Currently TemperatureSensor of STB is one.
	*punMax = 1;

	return ERR_OK;
}

HERROR RMOD_DI_GetStrLAN_Statues(const HCHAR *pszName, HCHAR *pBuf, HINT32 nBufLen)
{
	return ERR_OK;
}
HERROR RMOD_DI_GetUINTLAN_Statues(const HCHAR *pszName, HUINT32 *punData)
{
	if(!pszName)
		return ERR_FAIL;

	if(!punData)
		return ERR_FAIL;
	
	if(rema_MOD_QUpdate(g_stLANStatus))
	{

		
		if(!HxSTD_StrCmp(pszName, "TotalBytesSent"))
		{
			*punData = g_stLANStatus.pstGetStatus->ulBytesSend;
		}
		else if(!HxSTD_StrCmp(pszName, "TotalBytesReceived"))
		{
			*punData = g_stLANStatus.pstGetStatus->ulBytesReceive;
		}
		else if(!HxSTD_StrCmp(pszName, "TotalPacketsSent"))
		{
			*punData = g_stLANStatus.pstGetStatus->ulPacketsSend;
		}
		else if(!HxSTD_StrCmp(pszName, "TotalPacketsReceived"))
		{
			*punData = g_stLANStatus.pstGetStatus->ulPacketsReceive;
		}
		else
		{
			RemLOG_Debug("erro pszName(%s)\n", pszName);
		}
		RemLOG_Debug("pszName(%s)  value(%u)\n", pszName, *punData);
	}
	
	return ERR_OK;
}

HERROR RMOD_DI_GetCPUUsage(HUINT32 *CPUUsage)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32 cpuUsageVal = 0;
	
	hErr = DI_SYSTEM_GetCPUUsage(&cpuUsageVal);
	
	if(hErr == ERR_FAIL)
	{
		RemLOG_Debug("Cannot get CPU Usage!!!\n");
	}
	else
	{
		*CPUUsage = cpuUsageVal;
		RemLOG_Debug("CPUUsage : %d", *CPUUsage);
	}

	return hErr;
}


#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

