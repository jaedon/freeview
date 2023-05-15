/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_epgbgtracking.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_EPGGATHERING_INT_H___
#define	___XMGR_EPGGATHERING_INT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_epgbgtracking.h>
#include <_mgr_epgbgtracking.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/






/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*	Common Functions
 */

/*	Base Functions
 */
HERROR	xmgr_epgbgtracking_Start_Base(void);
HERROR	xmgr_epgbgtracking_Stop_Base(void);

// for batch
BUS_Callback_t	xmgr_epgbgtracking_GetProc_Base(void);





#endif /* !___XMGR_EPGGATHERING_INT_H___ */
