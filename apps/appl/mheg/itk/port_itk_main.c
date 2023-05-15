/**
	@file     port_itk_main.c
	@brief    port_itk_main.c (Main module for Redkey2 Porting layer)

	Description: Redkey2의 Porting Layer(ITK - Interactive TVKit)에서 사용하는 함수중에서	\n
				1. Redkey Library와 Asyn하게 동작 요하는 기능 (Task switch로 처리 )  	\n
				2. Redkey Porting Layer상에서 전반적으로 사용되거나 전체를 Control 하는 기능     \n
			  관련된 함수 Set
	Module: MW/mheg/itk 																\n
	Remarks : 																			\n
            1. 여기서 사용하는 Porting Layer의 Local 변수는 itk convention rule에 맞춘다     \n
              그 외 다른 모든 변수의 Convention rule은 OCTO의 형식으로 간다. 			\n
	Copyright (c) 2008 HUMAX Co., Ltd.													\n
	All rights reserved.																\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <apk.h>

#include "mheg_param.h"
#include "mheg_int.h"
#include "port_itk_main.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      external Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HUINT16			s_usEngineProfile = 0;
static redkey_profile_t	s_ItkProfile;
static size_t			s_ulItkMemSize;

static PostMsgCB_t	s_cbPostMsgCallback = NULL;

static HBOOL		s_bMasterMhegRunningStatus = FALSE;
static HBOOL		s_bInternalMhegRunningStatus = FALSE; 	// s_bInternalMhegRunningStatus : MHEG State의 hole을 보완하기 위해서 사용.
															//		MHEG의 CallBack에 의해서 update 되는 s_mastMhegState와는 달리,
															//		MHEG Stop request를 보낼 때 부터 REDKEY_STOP 상태로 바뀌며,
															//		masgMhegState 값이 바뀌기 전 까지 일부 기능을 막는 역할을 함.
static HUINT32		s_ulMastViewId, s_ulMastSessionId;
static HINT32		s_nMastCurSvcUid = 0;
static HINT32		s_nMastDefSvcUid = 0;

static HINT32		s_nVideoPid = PID_NULL, s_nAudioPid = PID_NULL, s_nPcrPid = PID_NULL;
static HUINT8		s_ucVideoType = 0, s_ucAudioType = 0;
static HUINT32		s_ulDemuxId = 0;
static MHEG_MONITOR_Mode_e	 s_eMonitorMode = eMHEG_MONITOR_MODE_None;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static HUINT8			s_mastAvMode = 0;
static HUINT8			s_mastAvStatus = 0;
#endif

static itk_control_t	s_mastBroadcastInterruption;
static itk_control_t	s_mastRct;

/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


#define _____PUBLIC_APIs________________________________________________________________
void PORT_ITK_SetEngineProfile(HUINT16 usEngineProfile)
{
	s_usEngineProfile = usEngineProfile;
}

HBOOL PORT_ITK_CheckEngineProfile(HUINT16 usEngineProfile)
{
	return ((s_usEngineProfile & usEngineProfile) ? TRUE:FALSE);
}

void PORT_ITK_SetItkProfile(redkey_profile_t eItkProfile)
{
	s_ItkProfile = eItkProfile;
}

redkey_profile_t PORT_ITK_GetItkProfile(void)
{
	return s_ItkProfile;
}

void PORT_ITK_SetItkMemSize(size_t ulItkMemSize)
{
	s_ulItkMemSize = ulItkMemSize;
}

size_t PORT_ITK_GetItkMemSize(void)
{
	return s_ulItkMemSize;
}

HERROR	PORT_ITK_RegisterCallback(PostMsgCB_t cbPostMsgCallback)
{
	s_cbPostMsgCallback = cbPostMsgCallback;

	return ERR_OK;
}

HERROR	PORT_ITK_GetEngineVersion(HCHAR *pszBuffer, HINT32 nBufferLen)
{
	if (nBufferLen >= 6)
	{
		ITK_getVersionInfo(pszBuffer, nBufferLen);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR	PORT_ITK_SetMhegRunningStatus(HBOOL bRunning)
{
	s_bMasterMhegRunningStatus = bRunning;
	s_bInternalMhegRunningStatus = bRunning;
	// 일단은 MhegRunningStatus가 바뀌면, s_bInternalMhegRunningStatus 값도 이 값을 따라서 복구 되도록 한다.
	// 추가적인 이슈가 생기면 s_bInternalMhegRunningStatus 값을 별도의 타이밍으로 관리하게 될 수도 있으나,
	// 현재로서는 side-effect가 없도록 이 곳에서 변경하는 것이 최선이다.
	return ERR_OK;
}

HBOOL	PORT_ITK_GetMhegRunningStatus(void)
{
	return s_bMasterMhegRunningStatus;
}

// s_bInternalMhegRunningStatus : MHEG State의 hole을 보완하기 위해서 사용.
//		MHEG의 CallBack에 의해서 update 되는 s_bMastMhegRunningStatus와는 달리,
//		MHEG Stop request를 보낼 때 부터 REDKEY_STOP 상태로 바뀌며,
//		masgMhegState 값이 바뀌기 전 까지 일부 기능을 막는 역할을 함.
HERROR	PORT_ITK_SetMhegInternalRunningStatus(HBOOL bRunning)
{
	s_bInternalMhegRunningStatus = bRunning;

	return ERR_OK;
}

HBOOL	PORT_ITK_GetMhegInternalRunningStatus(void)
{
	return s_bInternalMhegRunningStatus;
}

HERROR	PORT_ITK_PostMsg(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	return (HERROR)PORT_ITK_PostMsgEx(nMsg, nViewId, nParam1, nParam2, nParam3, nParam4, NULL, 0);
}

HERROR	PORT_ITK_PostMsgEx(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4, void *pData, HUINT32 ulDataSize)
{
	if(s_cbPostMsgCallback == NULL)
		return ERR_FAIL;

	return (HERROR)s_cbPostMsgCallback(nMsg, nViewId, nParam1, nParam2, nParam3, nParam4, pData, ulDataSize);
}

HERROR	PORT_ITK_SetMastCurService(HINT32 nCurSvcUid)
{
	s_nMastCurSvcUid = nCurSvcUid;

	return ERR_OK;
}

HERROR PORT_ITK_GetMastViewId(HUINT32* pulViewId)
{
	*pulViewId = s_ulMastViewId;

	return ERR_OK;
}


HERROR	PORT_ITK_GetSvcUid(HINT32 *pnDefSvcUid, HINT32 *pnCurSvcUid)
{
	if (pnDefSvcUid)
		*pnDefSvcUid = s_nMastDefSvcUid;
	if (pnCurSvcUid)
		*pnCurSvcUid = s_nMastCurSvcUid;

	return ERR_OK;
}

//	APPKIT MEDIA PLAY가 정상 동작하지 않아 우선 하드 코딩,
HERROR	PORT_ITK_SetSvcUid(HUINT32 ulViewId, HUINT32 ulSessionId, HINT32 nDefSvcUid, HINT32 nCurSvcUid)
{
	s_ulMastViewId = ulViewId;
	s_ulMastSessionId = ulSessionId;
	s_nMastDefSvcUid = nDefSvcUid;
	s_nMastCurSvcUid = nCurSvcUid;

	return ERR_OK;
}

void	PORT_ITK_SetMasterMhegPesInfo(HINT32 nVideoPid, HINT32 nAudioPid, HINT32 nPcrPid, HUINT8 ucVideoType, HUINT8 ucAudioType)
{
	s_nVideoPid   = nVideoPid;
	s_nAudioPid   = nAudioPid;
	s_nPcrPid     = nPcrPid;
	s_ucVideoType = ucVideoType;
	s_ucAudioType = ucAudioType;
}

HERROR	PORT_ITK_GetMasterMhegPesInfo(HINT32 *pnVideoPid, HINT32 *pnAudioPid, HINT32 *pnPcrPid, HUINT8 *pucVideoType, HUINT8 *pucAudioType)
{
	*pnVideoPid   = s_nVideoPid;
	*pnAudioPid   = s_nAudioPid;
	*pnPcrPid     = s_nPcrPid;
	*pucVideoType = s_ucVideoType;
	*pucAudioType = s_ucAudioType;

	return ERR_OK;
}

void PORT_ITK_SetBroadcastInterruptionStatus(itk_control_t broadcastInterruption)
{
	s_mastBroadcastInterruption = broadcastInterruption;
}

void PORT_ITK_SetRctStatus(itk_control_t rct)
{
	s_mastRct = rct;
}

HUINT32	PORT_ITK_GetDemuxId(HINT32 nViewId)
{
	return s_ulDemuxId;
}

void	PORT_ITK_SetDemuxId(HINT32 nViewId, HUINT32 ulDemuxId)
{
	s_ulDemuxId = ulDemuxId;
}

HUINT32 PORT_ITK_GetMonitoringMode (HINT32 nViewId)
{
	return (HUINT32)s_eMonitorMode;
}

void PORT_ITK_SetMonitoringMode (HINT32 nViewId, HUINT32 eMode)
{
	s_eMonitorMode = (MHEG_MONITOR_Mode_e)eMode;
}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
void PORT_ITK_SetMastMhegAvMode(HUINT8 avMode)
{
	s_mastAvMode = avMode;
}

void PORT_ITK_GetMastMhegAvMode(HUINT8 *pAvMode)
{
	*pAvMode = s_mastAvMode ;
}

void PORT_ITK_SetMastMhegAvStatus(HUINT8 avStatus)
{
	s_mastAvStatus = avStatus;
}

void PORT_ITK_GetMastMhegAvStatus(HUINT8 *pAvStatus)
{
	*pAvStatus = s_mastAvStatus;
}
#endif

