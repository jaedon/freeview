
/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_PES_H__
#define	__PAL_PES_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "htype.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef HERROR (* PalPes_SBTL_Notify_t)(Handle_t hAction, HUINT32 ulDemuxId, HUINT16 usBufLen, HUINT8 *pucBuffer);
typedef HERROR (* PalPes_TTX_Notify_t)(Handle_t hAction, HUINT32 ulDemuxId, HUINT32 ulLen, HUINT8 *pucBuffer);
typedef HERROR (* PalPes_USERDEF_Notify_t)(HUINT32 ulDemuxId, HUINT32 ulLen, HUINT8 *pucBuffer, HUINT32 ulRqId);

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
extern HERROR PAL_PES_Init(void);

extern HERROR PAL_PES_StartSbtlTask(void);
extern HERROR PAL_PES_RegisterSbtlCallBack(PalPes_SBTL_Notify_t pfnNotify);
extern HERROR PAL_PES_StartSbtl(Handle_t hAction, HUINT32 ulDemuxId, HUINT16 usPid);
extern HERROR PAL_PES_StopSbtl(HUINT32 ulDemuxId);

extern HERROR PAL_PES_StartTtxTask(void);
extern HERROR PAL_PES_RegisterTtxCallBack(PalPes_TTX_Notify_t pfnNotify);
extern HERROR PAL_PES_EnableTtx(Handle_t hAction, HUINT32 ulDemuxId);
extern HERROR PAL_PES_DisableTtx(Handle_t hAction, HUINT32 ulDemuxId);
extern HERROR PAL_PES_StartTtx(Handle_t hAction, HUINT32 ulDemuxId, HUINT16 usPid);
extern HERROR PAL_PES_StopTtx(HUINT32 ulDemuxId);
extern HERROR PAL_PES_RegisterUserDefCallBack(PalPes_USERDEF_Notify_t pfnNotify);
extern HERROR PAL_PES_StartUserDef(HUINT32 ulDemuxId, HUINT16 usPid, HUINT32 ulRqId);
extern HERROR PAL_PES_StopUserDef(HUINT32 ulDemuxId, HUINT32 ulRqId);
#endif /* !__PAL_SUBTELE_H__ */

