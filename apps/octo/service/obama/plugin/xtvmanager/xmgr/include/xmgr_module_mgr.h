/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_module_mgr.h
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_MODULE_MGR_H__
#define	__XMGR_MODULE_MGR_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <isosvc.h>
#include <stb_params.h>

#include <xmgr_event.h>
// Module

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*
 * Porting Layer에서 TV Manager Module 끼리 주고 받는 Message
 */

enum
{
	MSG_XMGR_CAS_START		= MSG_XMGR_MODULE_START,

/* ==================================================================================================================
	CAS Message
   ================================================================================================================== */

	/* - Description : Message for cas bc
		   - Parameters Usage :
			   handle : Action handle
			   p1 : TBD
			   p2 : TBD
			   p3 : TBD */
	MSG_XMGR_CAS_DESTROY_UIMGR,

	/* - Description : Message for cas bc
			   - Parameters Usage :
				   handle : Action handle
				   p1 : Ca Link Info (BCAS_CaAlternativeServiceInfo_t*)
				   p2 : Error number (HUINT16)
				   p3 : is Bcas (HBOOL) */
	MSG_XMGR_CAS_SHOW_CALINK_MSG,

	/* - Description : Message for cas bc
			   - Parameters Usage :
				   handle : Action handle
				   p1 : errNumber (HUINT16)
				   p2 : Ca SystemId (HUINT16)
				   p3 : TBD */
	MSG_XMGR_CAS_SHOW_CAS_ERROR_MSG,

	/* - Description : Message for cas bc
			   - Parameters Usage :
				   handle : Action handle
				   p1 : AutoDispInfo (BCAS_AutoDisplayInfo_t*)
				   p2 : MsgId (HUINT32)
				   p3 : TBD */
	MSG_XMGR_CAS_SHOW_AUTO_DISP_MSG,

	/* - Description : Message for cas bc
			   - Parameters Usage :
				   handle : Action handle
				   p1 : AutoDispInfo (BCAS_AutoDisplayInfo_t*)
				   p2 : MsgId (HUINT32)
				   p3 : TBD */
	MSG_XMGR_CAS_HIDE_AUTO_DISP_MSG,

	/* - Description : Message for cas bc
			   - Parameters Usage :
				   handle : Action handle
				   p1 : AutoDispInfo (BCAS_AutoDisplayInfo_t*)
				   p2 : MsgId (HUINT32)
				   p3 : TBD */
	MSG_XMGR_CAS_CLEAR_AUTO_DISP_MSG,

	/* - Description : Message for cas bc
			   - Parameters Usage :
				   handle : Action handle
				   p1 : TBD
				   p2 : TBD
				   p3 : TBD */
	MSG_XMGR_CAS_CHECK_EIT_PF,

	/* - Description : Message for cas bc
			   - Parameters Usage :
				   handle : N/A
				   p1 : new main action id
				   p2 : old main action id
				   p3 : TBD */
	MSG_XMGR_CAS_AUTO_MSG_SWAP,

	MSG_XMGR_CAS_END,
};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/* Setup info Proc */
BUS_Result_t AP_Setup_Info_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

#endif	/*	!__XMGR_MODULE_MGR_H__	*/
