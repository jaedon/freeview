/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_satrecord.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __PMGR_SATRECORD_H__
#define __PMGR_SATRECORD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_satrecord.h>

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/
BUS_Callback_t pmgr_satrecord_GetDetectPIDAction (void);
BUS_Callback_t pmgr_satrecord_GetDownloadAction (void);
HERROR			pmgr_satrecord_RescheduleBatch(void);

#endif /* __PMGR_SATRECORD_H__ */
