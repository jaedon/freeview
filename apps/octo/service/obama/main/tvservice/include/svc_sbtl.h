/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mw_sbtl.h
	@brief	  MW subtitle 모듈 헤더 파일

	Description: 관련 API 및 데이터 구조를 선언한 헤더 파일.
	Module: MW/SBTL			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef _MW_SUBTITLE_H_
#define _MW_SUBTITLE_H_

#if defined(CONFIG_MW_SUBTITLE)

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <osd.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if defined (CONFIG_MWC_OSD_SBTL_ON_UIPLANE)
typedef HERROR (*pfnAP_SBTLDW_Alloc)(HUINT32 len, HUINT8 **p);
#endif

/** @brief SBTL error Table */
enum
{
	eSEVT_SBTL_ = eSEVT_SBTL_START,

	/* - Description : Action의 subtitle이 stop되었다는 이벤트 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SBTL_STARTED,
	eSEVT_SBTL_STOPPED,
	eSEVT_SBTL_UPDATED,

	eSEVT_SBTL_END /* 그냥 끝나는 거 표시 */
};



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
/**
  @remark	Subtitle service를 초기화 하는 함수.\n
            Task를 만들어 app에 대한 요구를 기다린다. \n

  @remark   API Type : Sync \n

  @return   (X) void
*/
extern void SVC_SBTL_Init(PostMsgToMgrCb_t fnPostMsg);

/**
  @remark	Subtitle을 시작 시키는 API 함수.\n
			Action이 master action이면서 version이 최신이면\n
			subtitle을 동작하게 만든다. \n

  @remark   API Type : Async \n

  @return   ERR_OK \n
            ERR_FAIL : msg send 실패
*/
extern HERROR SVC_SBTL_StartService(
							Handle_t 		hAction,
							HUINT16			usPid,
							HUINT16         usCompPageId,
							HUINT16         usAncPageId
							);

/**
  @remark	Subtitle을 stop 시키는 함수로 Sync와 Async를 mode \n
  			를 지원한다. \n

  @remark   API Type : Async or Sync\n

  @return   ERR_OK \n
            ERR_FAIL : msg send 실패
*/
extern HERROR SVC_SBTL_StopService(
							Handle_t		hAction,
							ApiSyncMode_t	syncMode
							);

/**
  @remark	subtitle에서 filtering하고 있는 PID를 변경해주는 API 함수로
  			만약 동작 중 이었다면 stop 후 다시 start한다. \n

  @remark   API Type : Async \n

  @return   ERR_OK \n
            ERR_FAIL : msg send 실패
*/
extern HERROR SVC_SBTL_ChangePid(
							Handle_t 		hAction,
							HUINT16			usPid
							);

/**
  @remark	Master action을 변경 해 주는 함수로 해당 action으로	\n
  			master를 변경하며 이전 master가 동작 중이라면 이전	\n
  			master action을 stop 시키고 새로운 master action을	\n
  			시작 시킨다.\n

  @remark   API Type : Async \n

  @return   ERR_OK \n
            ERR_FAIL : msg send 실패
*/
extern HERROR SVC_SBTL_SetMasterAction(
							HUINT32			ulMasterActionId
							);



/**
  @remark	Decoder를 On/Off 시켜주는 함수. \n

  @remark   API Type : Async \n

  @return   ERR_OK \n
            ERR_FAIL : msg send 실패
*/
extern HERROR SVC_SBTL_SetOnOffDecoder(
							Handle_t 		hAction,
							HBOOL			bOffDec
							);


#if defined (CONFIG_MWC_OSD_SBTL_ON_UIPLANE)
/**
  @remark	Decoder를 app에서 init 하게 할 수 있는 함수. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE에서만 사용

  @return   ERR_OK \n
            ERR_FAIL 실패
*/
extern HERROR SVC_SBTL_InitDecoder(
							HUINT32 	ulActionId
							);


/**
  @remark	Decoder를 app에서 terminate 하게 할 수 있는 함수. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE에서만 사용

  @return   ERR_OK \n
            ERR_FAIL 실패
*/
extern HERROR SVC_SBTL_TermDecoder(
							HUINT32 	ulActionId
							);


/**
  @remark	app에서 packet을 decoding 하게 할 수 있는 함수. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE에서만 사용

  @return   ERR_OK \n
            ERR_FAIL 실패
*/
extern HERROR SVC_SBTL_DecodingPacket(
							HUINT32 	ulActionId,
							HUINT8 		*pPes,
							HUINT32 	ulPesSize,
							HUINT32 	*timeOut,
							OSD_Rect_t *pRect
							);


/**
  @remark	app에서 packet을 alloc 하게 할 수 있는 함수. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE에서만 사용

  @return   ERR_OK \n
            ERR_FAIL 실패
*/
extern HERROR SVC_SBTL_RegisterAllocCb(
							pfnAP_SBTLDW_Alloc Cb
							);


/**
  @remark	app에서 decoding된 내용을 OSD plane에 update(draw) 하는 함수. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE에서만 사용

  @return   ERR_OK \n
            ERR_FAIL 실패
*/
extern HERROR SVC_SBTL_UpdatePacket(
							HUINT32 	ulActionId
							);


/**
  @remark	app에서 decoding된 내용을 OSD plane에 update(draw) 하는 함수. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE에서만 사용

  @return   ERR_OK \n
            ERR_FAIL : msg send 실패
*/
extern HERROR SVC_SBTL_ClearPacket(
							HUINT32 	ulActionId
							);
#endif


/**
  @remark	Action을 subtitle 동작과 연관 시켜 보여주는 함수. \n

  @remark   API Type : Sync \n

  @return   ERR_OK \n
            ERR_FAIL : msg send 실패
*/
#ifdef CONFIG_DEBUG
extern HERROR SVC_SBTL_PrintStatus(void);
extern void	  SVC_SBTL_InitCmd(void);
#endif

#endif /* #if defined(CONFIG_MW_SUBTITLE) */
#endif /* _MW_SUBTITLE_H_ */

