/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_epggathering.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_EPGGATHERING_H___
#define	___PMGR_EPGGATHERING_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

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
/********************      Plug-In Functions    ********************/
/*******************************************************************/
HERROR	pmgr_epggathering_Start (void);
HERROR	pmgr_epggathering_Stop (void);

// for batch
BUS_Callback_t		pmgr_epggathering_GetProc(void);


#endif /* !___PMGR_EPGGATHERING_H___ */
