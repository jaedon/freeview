/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_models.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_MODELS_H___
#define	___MGR_MODELS_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_common.h>
#include <mgr_live.h>
#include <mgr_swup.h>

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

extern HERROR	MGR_MODEL_Init (void);
extern HERROR	MGR_MODEL_LiveGetSiSpec (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec);
extern HERROR	MGR_MODEL_SearchGetSiSpec (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec);
extern HERROR 	MGR_MODEL_SearchGetLcnPolicy (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy);
extern HERROR	MGR_MODEL_ExtsearchGetSiSpec (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec);
extern HERROR	MGR_MODEL_SatrecGetSiSpec (SvcSi_SatRecordSpec_t *pstOption);
extern HERROR 	MGR_MODEL_TimeupdateGetSiSpec (SvcSi_TimeUpdateSpec_t *pstSiSpec);
extern HERROR 	MGR_MODEL_MakesectionGetSiSpec (SvcSi_MakeSectionSpec_t *pstSiSpec);
extern HERROR 	MGR_MODEL_GetSiEsPriorityTable (SvcSi_EsPriorityTable_t *pstTable);

extern HERROR	MGR_MODEL_SwupInitProduct(void);
extern HERROR	MGR_MODEL_SwupGetSiSpec (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec);

extern HERROR 	MGR_MODEL_RecordGetSiSpec (SvcSi_RecSpec_t *pstSiSpec);
extern HERROR	MGR_MODEL_SatipGetSiSpec (SvcSi_SatipSpec_t *pstSiSpec);
extern HERROR	MGR_MODEL_RecordGetCasTypes (HUINT32 *pulCasTypes);
extern HERROR 	MGR_MODEL_PlaybackGetSiSpec (SvcSi_PbSpec_t *pstSiSpec);
extern HERROR	MGR_MODEL_PlaybackGetCasTypes (HUINT32 *pulCasTypes);

extern HERROR 	MGR_MODEL_MediaplaybackGetSiSpec (SvcSi_MediaPbSpec_t *pstSiSpec);


#endif /* !___MGR_MODELS_H___ */

