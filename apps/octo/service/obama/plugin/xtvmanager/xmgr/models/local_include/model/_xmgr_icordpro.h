/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_icordpro.h
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


#ifndef	___INT_XMGR_ICORDPRO_H___
#define	___INT_XMGR_ICORDPRO_H___

#if !defined(___INSIDE_INT_XMGR_MODEL_H_FILE___)
	#error ("!!! _xmgr_icordpro.h shall be included only in the _xmgr_model.h !!!\n");
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
HERROR		xmgr_model_Init_ICORDPRO (void);
HERROR		xmgr_model_GetLiveSpec_ICORDPRO (XmgrLive_LiveType_InArg_t *pstIn, XmgrLive_LiveType_t *pstOut);
HERROR		xmgr_model_LiveGetSiSpec_ICORDPRO (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec);
HERROR		xmgr_model_SearchGetSiSpec_ICORDPRO (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec);
HERROR 		xmgr_model_SearchGetLcnPolicy_ICORDPRO (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy);
HERROR 		xmgr_model_ExtsearchGetSiSpec_ICORDPRO (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec);
HERROR		xmgr_model_SwupGetSiSpec_ICORDPRO (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec);
HERROR		xmgr_model_RecordGetSiSpec_ICORDPRO (SvcSi_RecSpec_t *pstSiSpec);
HERROR		xmgr_model_PlaybackGetSiSpec_ICORDPRO (SvcSi_PbSpec_t *pstSiSpec);
HERROR 		xmgr_model_MediaplaybackGetSiSpec_ICORDPRO (SvcSi_MediaPbSpec_t *pstSiSpec);
HERROR		xmgr_model_RecordGetCasTypes_ICORDPRO (HUINT32 *pulCasTypes);
HERROR		xmgr_model_PlaybackGetCasTypes_ICORDPRO (HUINT32 *pulCasTypes);
HERROR 		xmgr_model_SatrecGetSiSpec_ICORDPRO (SvcSi_SatRecordSpec_t *pstSiSpec);
HERROR		xmgr_model_SwupInitProduct_ICORDPRO (void);
HERROR 		xmgr_model_TimeupdateGetSiSpec_ICORDPRO (SvcSi_TimeUpdateSpec_t *pstSiSpec);
HERROR 		xmgr_model_MakesectionGetSiSpec_ICORDPRO (SvcSi_MakeSectionSpec_t *pstSiSpec);
HERROR 		xmgr_model_GetSiEsPriorityTable_ICORDPRO (SvcSi_EsPriorityTable_t *pstTable);



#endif	/*	!___INT_XMGR_ICORDPRO_H___	*/
