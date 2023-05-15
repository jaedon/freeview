/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_hgs1000s.h
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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


#ifndef	___INT_XMGR_HGS1000S_H___
#define	___INT_XMGR_HGS1000S_H___

#if !defined(___INSIDE_INT_XMGR_MODEL_H_FILE___)
	#error ("!!! _xmgr_hgs1000s.h shall be included only in the _xmgr_model.h !!!\n");
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
HERROR		xmgr_model_Init_HGS1000S (void);
HERROR		xmgr_model_GetLiveSpec_HGS1000S (XmgrLive_LiveType_InArg_t *pstIn, XmgrLive_LiveType_t *pstOut);
HERROR		xmgr_model_LiveGetSiSpec_HGS1000S (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec);
HERROR		xmgr_model_SearchGetSiSpec_HGS1000S (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec);
HERROR 		xmgr_model_SearchGetLcnPolicy_HGS1000S (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy);
HERROR 		xmgr_model_ExtsearchGetSiSpec_HGS1000S (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec);
HERROR		xmgr_model_SwupGetSiSpec_HGS1000S (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec);
HERROR		xmgr_model_RecordGetSiSpec_HGS1000S (SvcSi_RecSpec_t *pstSiSpec);
HERROR		xmgr_model_PlaybackGetSiSpec_HGS1000S (SvcSi_PbSpec_t *pstSiSpec);
HERROR 		xmgr_model_MediaplaybackGetSiSpec_HGS1000S (SvcSi_MediaPbSpec_t *pstSiSpec);
HERROR		xmgr_model_RecordGetCasTypes_HGS1000S (HUINT32 *pulCasTypes);
HERROR		xmgr_model_SatipGetSiSpec_HGS1000S (SvcSi_SatipSpec_t *pstSiSpec);
HERROR		xmgr_model_PlaybackGetCasTypes_HGS1000S (HUINT32 *pulCasTypes);
HERROR 		xmgr_model_SatrecGetSiSpec_HGS1000S (SvcSi_SatRecordSpec_t *pstSiSpec);
HERROR		xmgr_model_SwupInitProduct_HGS1000S (void);
HERROR 		xmgr_model_TimeupdateGetSiSpec_HGS1000S (SvcSi_TimeUpdateSpec_t *pstSiSpec);
HERROR 		xmgr_model_MakesectionGetSiSpec_HGS1000S (SvcSi_MakeSectionSpec_t *pstSiSpec);
HERROR 		xmgr_model_GetSiEsPriorityTable_HGS1000S (SvcSi_EsPriorityTable_t *pstTable);



#endif	/*	!___INT_XMGR_HGS1000S_H___	*/
