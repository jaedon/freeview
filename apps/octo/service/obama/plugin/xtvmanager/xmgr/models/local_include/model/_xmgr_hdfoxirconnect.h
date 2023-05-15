/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_hdfoxirconnect.h
	@brief

	Description:  									\n
	Module: Portinig Layer / Initialization / Model		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


#ifndef	___INT_XMGR_HDFOXIRCONNECT_H___
#define	___INT_XMGR_HDFOXIRCONNECT_H___

#if !defined(___INSIDE_INT_XMGR_MODEL_H_FILE___)
	#error ("!!! _xmgr_hdfoxirconnect.h shall be included only in the _xmgr_model.h !!!\n");
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <xmgr_live.h>
#include <mgr_search.h>
#include <mgr_swup.h>

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
/********************      Functions       *************************/
/*******************************************************************/
HERROR		xmgr_model_Init_HDFOXIRCONNECT (void);
HERROR		xmgr_model_GetLiveSpec_HDFOXIRCONNECT (XmgrLive_LiveType_InArg_t *pstIn, XmgrLive_LiveType_t *pstOut);
HERROR		xmgr_model_LiveGetSiSpec_HDFOXIRCONNECT (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec);
HERROR 		xmgr_model_SearchGetSiSpec_HDFOXIRCONNECT (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec);
HERROR 		xmgr_model_SearchGetLcnPolicy_HDFOXIRCONNECT (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy);
HERROR		xmgr_model_GetOtaCheckSiSpec_HDFOXIRCONNECT (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec);
HERROR		xmgr_model_RecordGetSiSpec_HDFOXIRCONNECT (SvcSi_RecSpec_t *pstSiSpec);
HERROR		xmgr_model_PlaybackGetSiSpec_HDFOXIRCONNECT (SvcSi_PbSpec_t *pstSiSpec);
HERROR 		xmgr_model_MediaplaybackGetSiSpec_HDFOXIRCONNECT (SvcSi_MediaPbSpec_t *pstSiSpec);
HERROR		xmgr_model_RecordGetCasTypes_HDFOXIRCONNECT (HUINT32 *pulCasTypes);
HERROR		xmgr_model_PlaybackGetCasTypes_HDFOXIRCONNECT (HUINT32 *pulCasTypes);
HERROR		xmgr_model_SwupInitProduct_HDFOXIRCONNECT (void);
HERROR 		xmgr_model_TimeupdateGetSiSpec_HDFOXIRCONNECT (SvcSi_TimeUpdateSpec_t *pstSiSpec);
HERROR 		xmgr_model_MakesectionGetSiSpec_HDFOXIRCONNECT (SvcSi_MakeSectionSpec_t *pstSiSpec);
HERROR 		xmgr_model_GetSiEsPriorityTable_HDFOXIRCONNECT (SvcSi_EsPriorityTable_t *pstTable);



#endif	/*	!___INT_XMGR_HDFOXIRCONNECT_H___	*/
