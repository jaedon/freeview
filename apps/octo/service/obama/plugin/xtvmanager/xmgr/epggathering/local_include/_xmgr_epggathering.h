/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_epggathering.h
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
#include <mgr_epggathering.h>
#include <_mgr_epggathering.h>

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
HERROR xmgr_epggathering_setupBackGroundAction (EpgGathering_Content_t *pstContents);
HERROR xmgr_epggathering_releaseBackGroundAction (EpgGathering_Content_t *pstContents);
HERROR xmgr_epggathering_startTune (EpgGathering_Content_t *pstContents);

HERROR xmgr_epggathering_checkActionIdAndType (EpgGathering_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR xmgr_epggathering_evtChLocked (EpgGathering_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR xmgr_epggathering_evtChUnLocked (EpgGathering_Content_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


/*	Base Functions
 */
HERROR	xmgr_epggathering_Start_Base(void);
HERROR	xmgr_epggathering_Stop_Base(void);

// for batch
BUS_Callback_t	xmgr_epggathering_GetProc_Base(void);

#if defined(CONFIG_MW_EPG_GATHERING)
HERROR	xmgr_epggathering_Start_BgBarker(void);
HERROR	xmgr_epggathering_Stop_BgBarker(void);
BUS_Callback_t	xmgr_epggathering_GetProc_BgBarker(void);
#endif

#endif /* !___XMGR_EPGGATHERING_INT_H___ */

