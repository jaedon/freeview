/*
  FILE : stub_ctrl.c
  PURPOSE: This file is a stub for linking tests.
*/
//#include "va_def.h"
//#include <./../vamgr_main.h>
#include "va_pi_api.h"
#include "va_dscr_api.h"
#include "va_util.h"
#include "va_ctrl.h"

INT VA_CTRL_GetProgramInformation( DWORD dwAcsId, tVA_CTRL_ProgramInformation *pProgramInfo )
{
	VA_PI_AcsInstance *pAcs;
	HUINT16 usOrgNetId, usTsId, usUniqueId, usServiceId;
	HERROR nErr;

	HxLOG_Print("[Enter][%x]\n", dwAcsId);

	// wrong dwAcsId
	VA_PI_IfNotValidAcsId(dwAcsId, kVA_INVALID_PARAMETER);

	if (pProgramInfo == NULL)
	{
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test 중 abnoral test case 시에 의도된 에러 상황에 불필요하게 로그 찍지 않도록...
		HxLOG_Info("pProgramInfo = NULL\n");
		#else
		HxLOG_Error("pProgramInfo = NULL\n");
		#endif
		return kVA_INVALID_PARAMETER;
	}

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_ASSERT(gVaPiSemId);
	VA_SEM_Get(gVaPiSemId);
#endif

	pAcs = pstVaAcsInstance[dwAcsId];

	// ACS is not opened
	if (pAcs->nAcsState == VA_PI_ACS_CLOSE)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("ACS is not opened\n");
		return kVA_OPEN_ACS_FIRST;
	}

	// current program is switched off
	if (pAcs->nProgramState == VA_PI_PROGRAM_SWITCH_OFF)
	{
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("current program is switched off\n");
		return kVA_SWITCH_ON_PROGRAM_FIRST;
	}

#if 0 // 아래의 상황은 가정하지 않는다.
	if (0)
	{
		HxLOG_Info("[VA_PI_GetProgramInformation] prog info not available yet\n");
		VA_SEM_Release(gVaPiSemId);
		return kVA_DATA_NOT_AVAILABLE;
	}
#endif

	// get program info
	nErr = VAMGR_GetTripleID(pAcs->hVaAction, &usOrgNetId, &usTsId, &usUniqueId, &usServiceId);
	if (nErr)
	{
		//* information is not received yet (e.g. NIT)
#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
		VA_SEM_Release(gVaPiSemId);
#endif
		HxLOG_Error("prog info not available yet\n");
		return kVA_DATA_NOT_AVAILABLE;
	}

	pProgramInfo->wNetworkId = usOrgNetId;		//이거는 이리 해도 되는지 나중에 다시 확인해봐야 할 듯 ...
	pProgramInfo->wOriginalNetworkId = usOrgNetId;
	pProgramInfo->wTransportStreamId = usTsId;
	pProgramInfo->wServiceId = usServiceId;

#if defined(USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA)
	VA_SEM_Release(gVaPiSemId);
#endif

	HxLOG_Info("[NID:%x, ONID:%x, TSID:%x, SrvID:%x]\n", pProgramInfo->wNetworkId, pProgramInfo->wOriginalNetworkId, pProgramInfo->wTransportStreamId, pProgramInfo->wServiceId);
	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
tVA_CTRL_StreamNotificationCallback
dwStbStreamHandle:
	The handle of the elementary stream (as provided in VA_CTRL_AddStream) that triggers the notification.
eStreamNotificationType:
	The actual notification type as defined by the tVA_CTRL_StreamNotificationType.
uStreamNotificationInfo:
	The actual notification value. The STB must use the appropriate field of the
	tVA_CTRL_StreamNotificationInfo union based on the notification type specified by the
	eStreamNotificationType parameter .

The following values can be returned:
	return kVA_OK
If the notification is successful.
	return kVA_ERROR
For any other reason.
*/
#if !defined(__VIACCESS50__)
INT VA_CTRL_StreamNotifyEvent(DWORD dwStbStreamHandle,
                                tVA_CTRL_StreamNotificationType eStreamNotificationType,
                                tVA_CTRL_StreamNotificationInfo uStreamNotificationInfo)
{
	return kVA_OK;
}

/*
tVA_CTRL_QueryStreamNotificationCallback
dwStbStreamHandle:
	The handle of the elementary stream (as provided in VA_CTRL_AddStream) for which the reply is sent.
tVA_CTRL_StreamCaEnableEvent:
	The actual notification value as defined by the tVA_CTRL_StreamCaEnableEvent enumeration.
*/
INT VA_CTRL_QueryStreamNotifyEvent( DWORD dwStbStreamHandle, tVA_CTRL_StreamCaEnableEvent eStreamCaEnableEvent)
{
	return kVA_OK;
}
#endif

#if 1//defined(_VIACCESS50_DISABLECODE_BEFORE_RELEASE_)
/*================================
 =================================
	dummy function for just build
	릴리즈 하기 전에 삭제 한다.
 =================================
==================================*/

INT   VA_CTRL_Init(const tVA_CTRL_ConfigurationParameters *pParameters)
{
	return kVA_OK;
}

void  VA_CTRL_Start(void)
{

}

INT   VA_CTRL_Stop(void)
{
	return kVA_OK;
}

INT   VA_CTRL_OpenAcsInstance(DWORD dwAcsId, tVA_CTRL_AcsMode eAcsMode)
{
	return kVA_OK;
}

INT   VA_CTRL_ChangeAcsInstanceMode(DWORD dwAcsId, tVA_CTRL_AcsMode eNewAcsMode)
{
	return kVA_OK;
}

INT   VA_CTRL_CloseAcsInstance(DWORD dwAcsId)
{
	return kVA_OK;
}

INT   VA_CTRL_TsChanged(DWORD dwAcsId)
{
	return kVA_OK;
}

INT   VA_CTRL_CatUpdated(DWORD dwAcsId, UINT32 uiCatLength, BYTE * pCat)
{
	return kVA_OK;
}

INT   VA_CTRL_SwitchOnProgram(DWORD dwAcsId, UINT32 uiPmtLength, BYTE * pPmt)
{
	return kVA_OK;
}

INT   VA_CTRL_SwitchOffProgram(DWORD dwAcsId)
{
	return kVA_OK;
}

INT   VA_CTRL_PmtUpdated(DWORD dwAcsId, UINT32 uiPmtLength, BYTE * pPmt)
{
	return kVA_OK;
}

INT   VA_CTRL_AddStream(DWORD dwAcsId, DWORD dwStbStreamHandle, DWORD dwPid, tVA_CTRL_StreamNotificationCallback pfVaStreamNotificationCallback)
{
	return kVA_OK;
}

INT   VA_CTRL_QueryStream(DWORD dwAcsId,
                          DWORD dwStbStreamHandle,
                          DWORD dwPid,
                          tVA_CTRL_QueryStreamNotificationCallback pfVaQueryStreamNotificationCallback)
{
	return kVA_OK;
}

INT   VA_CTRL_UpdateStream(DWORD dwStbStreamHandle, DWORD dwPid)
{
	return kVA_OK;
}

INT   VA_CTRL_RemoveStream(DWORD dwStbStreamHandle)
{
	return kVA_OK;
}

INT   VA_CTRL_SetSoidBlackList(DWORD dwAcsId, UINT32 uiSoidCount, tVA_CTRL_Soid * pSoidBlackList)
{
	return kVA_OK;
}

#endif


/* End of File */
