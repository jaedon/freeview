/**
	@file     mgr_models.c
	@brief    .

	Description:
	Module: MGR/PARAM			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_common.h>

#include <mgr_models.h>
#include <pmgr_model.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/

/********************************************************************/
/********************	Application Proc.	*************************/
/********************************************************************/

HERROR	MGR_MODEL_Init (void)
{
	return pmgr_model_Init ();
}

HERROR	MGR_MODEL_LiveGetSiSpec (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec)
{
	if (NULL == pstStart)
		return ERR_FAIL;

	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_LiveGetSiSpec (pstStart, pstSiSpec);
}

HERROR	MGR_MODEL_SearchGetSiSpec (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec)
{
	if (NULL == pstOption)
		return ERR_FAIL;

	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_SearchGetSiSpec (pstOption, pstSiSpec);
}

HERROR 	MGR_MODEL_SearchGetLcnPolicy (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy)
{
	if (NULL == peLcnPolicy)
		return ERR_FAIL;

	if (NULL == pePreProgChNumPolicy)
		return ERR_FAIL;

	return pmgr_model_SearchGetLcnPolicy (peLcnPolicy, pePreProgChNumPolicy);
}

HERROR	MGR_MODEL_ExtsearchGetSiSpec (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec)
{
	if (NULL == pstOption)
		return ERR_FAIL;

	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_ExtsearchGetSiSpec (pstOption, pstSiSpec);
}

HERROR	MGR_MODEL_SatrecGetSiSpec (SvcSi_SatRecordSpec_t *pstOption)
{
	if (NULL == pstOption)
		return ERR_FAIL;

	return pmgr_model_SatrecGetSiSpec (pstOption);
}

HERROR 	MGR_MODEL_TimeupdateGetSiSpec (SvcSi_TimeUpdateSpec_t *pstSiSpec)
{
	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_TimeupdateGetSiSpec (pstSiSpec);
}

HERROR 	MGR_MODEL_MakesectionGetSiSpec (SvcSi_MakeSectionSpec_t *pstSiSpec)
{
	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_MakesectionGetSiSpec (pstSiSpec);
}

HERROR 	MGR_MODEL_GetSiEsPriorityTable (SvcSi_EsPriorityTable_t *pstTable)
{
	if (NULL == pstTable)
		return ERR_FAIL;

	return pmgr_model_GetSiEsPriorityTable (pstTable);
}

HERROR	MGR_MODEL_SwupInitProduct(void)
{
	return pmgr_model_SwupInitProduct();
}

HERROR	MGR_MODEL_SwupGetSiSpec (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec)
{
	if (NULL == pstInArg)
		return ERR_FAIL;

	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_SwupGetSiSpec (pstInArg, pstSiSpec);
}

HERROR 	MGR_MODEL_RecordGetSiSpec (SvcSi_RecSpec_t *pstSiSpec)
{
	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_RecordGetSiSpec (pstSiSpec);
}

HERROR	MGR_MODEL_SatipGetSiSpec (SvcSi_SatipSpec_t *pstSiSpec)
{
	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_SatipGetSiSpec (pstSiSpec);
}

HERROR	MGR_MODEL_RecordGetCasTypes (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	return pmgr_model_RecordGetCasTypes (pulCasTypes);
}

HERROR 	MGR_MODEL_PlaybackGetSiSpec (SvcSi_PbSpec_t *pstSiSpec)
{
	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_PlaybackGetSiSpec (pstSiSpec);
}

HERROR	MGR_MODEL_PlaybackGetCasTypes (HUINT32 *pulCasTypes)
{
	if (NULL == pulCasTypes)
		return ERR_FAIL;

	return pmgr_model_PlaybackGetCasTypes (pulCasTypes);
}

HERROR 	MGR_MODEL_MediaplaybackGetSiSpec (SvcSi_MediaPbSpec_t *pstSiSpec)
{
	if (NULL == pstSiSpec)
		return ERR_FAIL;

	return pmgr_model_MediaplaybackGetSiSpec (pstSiSpec);
}


