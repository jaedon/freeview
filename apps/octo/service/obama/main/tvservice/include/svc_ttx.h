/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mw_ttx.h
	@brief	  MW teletext ��� ��� ����

	Description: ���� API �� ������ ������ ������ ��� ����.
	Module: MW/SBTL			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef _MW_TELETEXT_H_
#define _MW_TELETEXT_H_

#if defined(CONFIG_MW_TELETEXT)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
enum
{
	eSEVT_TTX_ = eSEVT_TTX_START,

	/* - Description : Action�� subtitle�� stop�Ǿ��ٴ� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eSEVT_TTX_STOPPED,
};


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern void SVC_TTX_Init(PostMsgToMgrCb_t fnPostMsg);
extern HERROR   SVC_TTX_StartService(Handle_t 	hAction,HUINT16	usPid);
extern HERROR   SVC_TTX_StopService(Handle_t	hAction,ApiSyncMode_t	syncMode);

#endif /* CONFIG_MW_TELETEXT */
#endif /* _MW_TELETEXT_H_ */

