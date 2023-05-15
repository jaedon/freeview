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

#ifndef	__SVC_SI_HOMECHANNEL_H__
#define	__SVC_SI_HOMECHANNEL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>

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
HERROR svc_si_HomeChannelProcessNit(Handle_t hAction, HINT32 lDeliType, HUINT16 usTsUniqId, HUINT32 ulHead);
HERROR svc_si_HomeChannelGetChannelInfo(DbSvc_TripleId_t *pstChannelInfo, HINT32 *plOrgDeliType, HBOOL *pbValid);
HERROR svc_si_HomeChannelInitChannelInfo(void);

#endif /* !__SVC_SI_HOMECHANNEL_H__ */
