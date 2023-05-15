/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_live.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_LIVE_INT_H___
#define	___XMGR_LIVE_INT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define LIVE_CHECK_ACTION_ID()	\
	if(pstContext->ulActionId != ulActionId) \
	{ \
		return MESSAGE_PASS; \
	}


#define LIVE_CHECK_ACTION_HANDLE() \
	LIVE_CHECK_ACTION_ID() \
	if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK) \
	{ \
		HxLOG_Print("[AP: %s(%d)] MESSAGE[0x%08X](0x%08x) is expired. Expected 0x%08X\n", __FUNCTION__, ulActionId, message, hAction, pstContext->hAction); \
		return MESSAGE_PASS; \
	}

/* local macros using in proc msg  */
#define LIVE_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
			}

#define LIVE_POINTER_INVALID_CHECK(pArg, returnValue)			\
			if(pArg == NULL)									\
			{													\
				HxLOG_Critical("\n\n");									\
				return returnValue;								\
			}

#define LIVE_HERROR_INVALID_CHECK(pArg, returnValue)				\
			if(pArg != ERR_OK)									\
			{													\
				HxLOG_Critical("\n\n");									\
				return returnValue;								\
			}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eLiveMwSvcType_Channel,
	eLiveMwSvcType_Si,
	eLiveMwSvcType_Cas,
	eLiveMwSvcType_Av,
	eLiveMwSvcType_Transcoder,
	eLiveMwSvcType_Subtitle,
	eLiveMwSvcType_Teletext,
	eLiveMwSvcType_CloseCaption,

	eLiveMwSvcType_MAX
} XmgrLive_MwSvcType_e;

typedef enum
{
	eLiveMwSvcState_Unknown,
	eLiveMwSvcState_ReqStop,
	eLiveMwSvcState_Stopped,
	eLiveMwSvcState_ReqStart,
	eLiveMwSvcState_Started,

	eLiveMwSvcState_EndOfCase
} XmgrLive_MwSvcState_e;

// Live�� ���� TV Service�� Start ��ų �� Start ��Ű�� ������ �˷��ִ� enum
// Ư�� M/W�� ��쿣 �ݵ�� Channel ���Ŀ�, � M/W�� A/V�� ������ ���� ���� ��å�� �����Ƿ�
// �̸� ���ϱ� ���� Flag�̴�.
typedef enum
{
	eLiveMwSvcExecPhase_None = 0,					// �ش� MW Service�� �����Ű�� �ʴ´�

	eLiveMwSvcExecPhase_AfterChannelTuned,			// �ش� MW Service�� Channel Tune ���� ���۽�Ų��.
	eLiveMwSvcExecPhase_AfterAvOut,					// �ش� MW Service�� AV�� ���� ���� ���۽�Ų��.
	eLiveMwSvcExecPhase_AfterPmtReceived,

	eLiveMwSvcExecPhase_EndOfCase
} XmgrLive_MwSvcExecPhase_e;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	xmgr_live_Init_Base(void);
HERROR	xmgr_live_ResetUiState_Base(HUINT32 ulActionId, HBOOL bSvcStart);
HERROR	xmgr_live_GetState_Base(HUINT32 ulActionId, MgrLive_ProcType_e eProcType, HUINT32 *pulUiState);


/**
  @remark	UI���� ������ service list group�� string�� �ִ� API.\n
  @remark   API Type : Inline \n

  @return   String pointer.
*/
HINT8	*xmgr_live_GetSvcListGroupStr(ChListUiGroup_t svcListGroup);

/**
  @remark	�Էµ� service list group�� valid�� ������ üũ�� �ִ� API \n
  @remark   API Type : Inline \n

  @return   ERR_OK
  			ERR_FAIL
*/
HERROR	xmgr_live_CheckValidSvcListGroup(ChListUiGroup_t svcListGroup);

/**
  @remark				\n
  @remark   API Type : 	\n

  @return   ERR_OK
  			ERR_FAIL
*/
HERROR	xmgr_live_GetFavouriteCHNumber(ChListUiGroup_t eChlistUiGroup, Handle_t hSvc, HINT32 *pnFavChNum);


// for DEBUG	>>
HUINT32	xmgr_live_PrintLiveLockContext (void);
HUINT32	xmgr_live_PrintWholeLiveContext (void);
// for DEBUG	<<


#endif /* !___XMGR_LIVE_INT_H___ */

