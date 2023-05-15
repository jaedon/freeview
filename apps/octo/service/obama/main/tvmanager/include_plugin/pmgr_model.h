/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_model.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_MODEL_H___
#define	___PMGR_MODEL_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_models.h>


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
extern HERROR	pmgr_model_Init (void);
extern HERROR	pmgr_model_LiveGetSiSpec (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec);
extern HERROR	pmgr_model_SearchGetSiSpec (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec);
extern HERROR 	pmgr_model_SearchGetLcnPolicy (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy);
extern HERROR	pmgr_model_ExtsearchGetSiSpec (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec);
extern HERROR	pmgr_model_SatrecGetSiSpec (SvcSi_SatRecordSpec_t *pstOption);
extern HERROR 	pmgr_model_TimeupdateGetSiSpec (SvcSi_TimeUpdateSpec_t *pstSiSpec);
extern HERROR 	pmgr_model_MakesectionGetSiSpec (SvcSi_MakeSectionSpec_t *pstSiSpec);
extern HERROR 	pmgr_model_GetSiEsPriorityTable (SvcSi_EsPriorityTable_t *pstTable);

extern HERROR	pmgr_model_SwupInitProduct (void);
extern HERROR	pmgr_model_SwupGetSiSpec (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec);

extern HERROR 	pmgr_model_RecordGetSiSpec (SvcSi_RecSpec_t *pstSiSpec);
extern HERROR	pmgr_model_RecordGetCasTypes (HUINT32 *pulCasTypes);
extern HERROR	pmgr_model_SatipGetSiSpec (SvcSi_SatipSpec_t *pstSiSpec);
extern HERROR 	pmgr_model_PlaybackGetSiSpec (SvcSi_PbSpec_t *pstSiSpec);
extern HERROR	pmgr_model_PlaybackGetCasTypes (HUINT32 *pulCasTypes);

extern HERROR 	pmgr_model_MediaplaybackGetSiSpec (SvcSi_MediaPbSpec_t *pstSiSpec);

#endif /* !___PMGR_MODEL_H___ */

