/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mw_sbtl.h
	@brief	  MW subtitle ��� ��� ����

	Description: ���� API �� ������ ������ ������ ��� ����.
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

	/* - Description : Action�� subtitle�� stop�Ǿ��ٴ� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_SBTL_STARTED,
	eSEVT_SBTL_STOPPED,
	eSEVT_SBTL_UPDATED,

	eSEVT_SBTL_END /* �׳� ������ �� ǥ�� */
};



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
/**
  @remark	Subtitle service�� �ʱ�ȭ �ϴ� �Լ�.\n
            Task�� ����� app�� ���� �䱸�� ��ٸ���. \n

  @remark   API Type : Sync \n

  @return   (X) void
*/
extern void SVC_SBTL_Init(PostMsgToMgrCb_t fnPostMsg);

/**
  @remark	Subtitle�� ���� ��Ű�� API �Լ�.\n
			Action�� master action�̸鼭 version�� �ֽ��̸�\n
			subtitle�� �����ϰ� �����. \n

  @remark   API Type : Async \n

  @return   ERR_OK \n
            ERR_FAIL : msg send ����
*/
extern HERROR SVC_SBTL_StartService(
							Handle_t 		hAction,
							HUINT16			usPid,
							HUINT16         usCompPageId,
							HUINT16         usAncPageId
							);

/**
  @remark	Subtitle�� stop ��Ű�� �Լ��� Sync�� Async�� mode \n
  			�� �����Ѵ�. \n

  @remark   API Type : Async or Sync\n

  @return   ERR_OK \n
            ERR_FAIL : msg send ����
*/
extern HERROR SVC_SBTL_StopService(
							Handle_t		hAction,
							ApiSyncMode_t	syncMode
							);

/**
  @remark	subtitle���� filtering�ϰ� �ִ� PID�� �������ִ� API �Լ���
  			���� ���� �� �̾��ٸ� stop �� �ٽ� start�Ѵ�. \n

  @remark   API Type : Async \n

  @return   ERR_OK \n
            ERR_FAIL : msg send ����
*/
extern HERROR SVC_SBTL_ChangePid(
							Handle_t 		hAction,
							HUINT16			usPid
							);

/**
  @remark	Master action�� ���� �� �ִ� �Լ��� �ش� action����	\n
  			master�� �����ϸ� ���� master�� ���� ���̶�� ����	\n
  			master action�� stop ��Ű�� ���ο� master action��	\n
  			���� ��Ų��.\n

  @remark   API Type : Async \n

  @return   ERR_OK \n
            ERR_FAIL : msg send ����
*/
extern HERROR SVC_SBTL_SetMasterAction(
							HUINT32			ulMasterActionId
							);



/**
  @remark	Decoder�� On/Off �����ִ� �Լ�. \n

  @remark   API Type : Async \n

  @return   ERR_OK \n
            ERR_FAIL : msg send ����
*/
extern HERROR SVC_SBTL_SetOnOffDecoder(
							Handle_t 		hAction,
							HBOOL			bOffDec
							);


#if defined (CONFIG_MWC_OSD_SBTL_ON_UIPLANE)
/**
  @remark	Decoder�� app���� init �ϰ� �� �� �ִ� �Լ�. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE������ ���

  @return   ERR_OK \n
            ERR_FAIL ����
*/
extern HERROR SVC_SBTL_InitDecoder(
							HUINT32 	ulActionId
							);


/**
  @remark	Decoder�� app���� terminate �ϰ� �� �� �ִ� �Լ�. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE������ ���

  @return   ERR_OK \n
            ERR_FAIL ����
*/
extern HERROR SVC_SBTL_TermDecoder(
							HUINT32 	ulActionId
							);


/**
  @remark	app���� packet�� decoding �ϰ� �� �� �ִ� �Լ�. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE������ ���

  @return   ERR_OK \n
            ERR_FAIL ����
*/
extern HERROR SVC_SBTL_DecodingPacket(
							HUINT32 	ulActionId,
							HUINT8 		*pPes,
							HUINT32 	ulPesSize,
							HUINT32 	*timeOut,
							OSD_Rect_t *pRect
							);


/**
  @remark	app���� packet�� alloc �ϰ� �� �� �ִ� �Լ�. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE������ ���

  @return   ERR_OK \n
            ERR_FAIL ����
*/
extern HERROR SVC_SBTL_RegisterAllocCb(
							pfnAP_SBTLDW_Alloc Cb
							);


/**
  @remark	app���� decoding�� ������ OSD plane�� update(draw) �ϴ� �Լ�. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE������ ���

  @return   ERR_OK \n
            ERR_FAIL ����
*/
extern HERROR SVC_SBTL_UpdatePacket(
							HUINT32 	ulActionId
							);


/**
  @remark	app���� decoding�� ������ OSD plane�� update(draw) �ϴ� �Լ�. \n

  @remark   API Type : Sync \n, CONFIG_MWC_OSD_SBTL_ON_UIPLANE������ ���

  @return   ERR_OK \n
            ERR_FAIL : msg send ����
*/
extern HERROR SVC_SBTL_ClearPacket(
							HUINT32 	ulActionId
							);
#endif


/**
  @remark	Action�� subtitle ���۰� ���� ���� �����ִ� �Լ�. \n

  @remark   API Type : Sync \n

  @return   ERR_OK \n
            ERR_FAIL : msg send ����
*/
#ifdef CONFIG_DEBUG
extern HERROR SVC_SBTL_PrintStatus(void);
extern void	  SVC_SBTL_InitCmd(void);
#endif

#endif /* #if defined(CONFIG_MW_SUBTITLE) */
#endif /* _MW_SUBTITLE_H_ */

