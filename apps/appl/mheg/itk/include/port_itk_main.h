/**
	@file	  port_itk_main.h														\n
	@brief	  itk main header file for porting layer							\n

	Description: ITK에서 공통으로 사용되는 기능을 정의한 header 파일  			\n
	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n
*/

/* 1. ITK Porting layer에서 ITK Main Task로 전달되어 지는 Messages 들을 정의 한다.
     이 부분은 Redkey2 Library가 Porting API와 Host 사이를 Asyn하게 동작 시키고자 할경우
     아래 메시지를 이용해서 사용한다 . 즉 Redkey Library가 Function Call 형식이 아닌 Task 전환을 요구하는
     Action일 경우 ITK의 main task를 이용해서 Task Switching을 해 준다.
   2. Message Format
      사용되는 Message는 Message Class로 크게 나뉘어 지고, 각각의 Message Class에서 해당 항목에 맞는
      Message Type이 정의 된다.
      MSG_ITK_CLASS_SYSTEM Class는 ITK와 Redkey Library간에 전달되는 message 혹은 ITK간 내부적인
      Message 처리에 사용되는 Message 이고,
      그 외의 Message Class는 ITK와 Application(MHEG Client Manager)간의 Message 로서 Application에서 ITK로
      보내지는 Message 이다.

      ITK에서 Application으로 보내지는 Message는 EVENT라는 이름과 형식으로 Application으로 전달 된다.

   [참고]
        ITK Task는 Redkey2 Library와 Host간의 Asyn동작을 위해서 사용되고
        Client Manager의 Task는 Action Manager에 의해서 생성되고 다른 Application과 그리고 ITK Task 와의 동기를
        위해서 동작 한다.         */


#ifndef _PORT_ITK_MAIN_H_
#define _PORT_ITK_MAIN_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include "mheg_int.h"
#include "mheg_param.h"
#include "itk.h"
#include "itk_redkey.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern void 		PORT_ITK_SetEngineProfile(HUINT16 usEngineProfile);
extern HBOOL 		PORT_ITK_CheckEngineProfile(HUINT16 usEngineProfile);
extern HERROR	PORT_ITK_RegisterCallback(PostMsgCB_t cbPostMsgCallback);
extern HERROR	PORT_ITK_GetEngineVersion(HCHAR *pszBuffer, HINT32 nBufferLen);
extern void 		PORT_ITK_SetItkMemSize(size_t ulItkMemSize);
extern HERROR	PORT_ITK_SetMhegRunningStatus(HBOOL bRunning);
extern HBOOL		PORT_ITK_GetMhegRunningStatus(void);
extern HERROR	PORT_ITK_SetMhegInternalRunningStatus(HBOOL bRunning);
extern HBOOL		PORT_ITK_GetMhegInternalRunningStatus(void);
extern HERROR	PORT_ITK_PostMsg(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4);
extern HERROR	PORT_ITK_PostMsgEx(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4, void *pData, HUINT32 ulDataSize);
extern HERROR	PORT_ITK_SetMastCurService(HINT32 nCurSvcUid);
extern HERROR	PORT_ITK_GetMastViewId(HUINT32* pulViewId);
extern HERROR	PORT_ITK_GetSvcUid(HINT32 *pnDefSvcUid, HINT32 *pnCurSvcUid);
extern HERROR	PORT_ITK_SetSvcUid(HUINT32 ulViewId, HUINT32 ulSessionId, HINT32 nDefSvcUid, HINT32 nCurSvcUid);
extern void		PORT_ITK_SetMasterMhegPesInfo(HINT32 nVideoPid, HINT32 nAudioPid, HINT32 nPcrPid, HUINT8 ucVideoType, HUINT8 ucAudioType);
extern HERROR	PORT_ITK_GetMasterMhegPesInfo(HINT32 *pnVideoPid, HINT32 *pnAudioPid, HINT32 *pnPcrPid, HUINT8 *pucVideoType, HUINT8 *pucAudioType);
extern void 		PORT_ITK_SetBroadcastInterruptionStatus(itk_control_t broadcastInterruption);
extern void		PORT_ITK_SetRctStatus(itk_control_t rct);
extern HERROR	PORT_ITK_GetMasterSessionId(HUINT32 *pulSessionId);
extern HUINT32	PORT_ITK_GetDemuxId(HINT32 nViewId);
extern void		PORT_ITK_SetDemuxId(HINT32 nViewId, HUINT32 ulDemuxId);

extern HUINT32	PORT_ITK_GetMonitoringMode (HINT32 nViewId);
extern void		PORT_ITK_SetMonitoringMode (HINT32 nViewId, HUINT32 eMode);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
extern void		PORT_ITK_SetMastMhegAvMode(HUINT8 avMode);
extern void		PORT_ITK_GetMastMhegAvMode(HUINT8 *pAvMode);
extern void		PORT_ITK_SetMastMhegAvStatus(HUINT8 avStatus);
extern void		PORT_ITK_GetMastMhegAvStatus(HUINT8 *pAvStatus);
#endif


#if 0
// 아래 함수들은 module internal 로 이동되어야 함.
void port_itk_UtilPostMsgToAp(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
void port_itk_UtilPostDataToAp(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, void *pData, HUINT32 ulSize);


void PORT_ITK_Init();

HERROR	PORT_ITK_RegisterNotifyCallback(PostMsgCB_t pfnMsgNotify, PostMsgCB_t pfnDataNotify);

/**
  @remark   get action handle stored in MHEG context
  @remark	type : syc

  @return	ERR_OK
            ERR_FAIL

*/
HERROR PORT_ITK_GetContextActionHandle(HUINT32 ulActionId, Handle_t* phAction);


/**
  @remark   get Service handle stored in MHEG context
  @remark	type : syc

  @return	ERR_OK
            ERR_FAIL

*/
HERROR PORT_ITK_GetSvcUid(Handle_t* phDefSvcHandle, Handle_t* pCurSvcHandle);


/**
  @remark   get current Mast Action ID registered in MHEG context.
  @remark	type : syc

  @return	ERR_OK
            ERR_FAIL

*/
HERROR PORT_ITK_GetMastActionId(HUINT32* pMastActionId);


/**
  @remark   post ITK event to GWM message handler.
  @remark	type : syc

  @return	NONE

*/
void PORT_ITK_PostEvent(HUINT32 ulEvtId, Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


/**
  @remark   get mast action handle.
  @remark	type : syc

  @return	ERR_OK
            ERR_FAIL

*/
HERROR PORT_ITK_GetMastActionHandle(Handle_t* phAction);


/**
  @remark   Get MHEG Screen.
  @remark	type : Sync

  @return	MHEG screen pointer
*/
void	PORT_ITK_SetMhegHandle(HUINT32 ulActionId, Handle_t hAction, Handle_t hDefSvc, Handle_t hCurSvc);
void	PORT_ITK_SetMastCurService(Handle_t hCurSvc);
void	PORT_ITK_SetMastMhegCurPesInfo(HUINT16 usVideoPid, HUINT16 usAudioPid, HUINT16 usPcrPid, HUINT8 ucVideoType, HUINT8 ucAudioType);
HERROR	PORT_ITK_GetMastMhegCurPesInfo(HUINT16* pusVideoPid, HUINT16* pusAudioPid, HUINT16* pusPcrPid, HUINT8* pucVideoType, HUINT8* pucAudioType);
void	PORT_ITK_SetMhegRunningStatus(HBOOL bRunning);
HBOOL	PORT_ITK_GetMhegRunningStatus(void);
void	PORT_ITK_SetInternalMhegRunningStatus(HBOOL bRunning);
HBOOL	PORT_ITK_GetSubMhegRunningStatus(void);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
void PORT_ITK_SetMastMhegAvMode(HUINT8 avMode);
void PORT_ITK_GetMastMhegAvMode(HUINT8 *pAvMode);
void PORT_ITK_SetMastMhegAvStatus(HUINT8 avStatus);
void PORT_ITK_GetMastMhegAvStatus(HUINT8 *pAvStatus);
#endif

HERROR PORT_ITK_GetEngineVersion(char *buffer, HUINT32 bufferLength);
void PORT_ITK_SetBroadcastInterruptionStatus(itk_control_t broadcastInterruption);
itk_control_t PORT_ITK_GetBroadcastInterruptionStatus(void);
void PORT_ITK_SetRctStatus(itk_control_t rct);
itk_control_t PORT_ITK_GetRctStatus(void);

#ifdef CONFIG_DEBUG
void 	PORT_ITK_DEBUG_ShowMhegContext(void);
void	PORT_ITK_DEBUG_SetupMhegMediaResource(void);
#endif	/* end of CONFIG_DEBUG */

#endif

#endif	/* port_itk_main */


