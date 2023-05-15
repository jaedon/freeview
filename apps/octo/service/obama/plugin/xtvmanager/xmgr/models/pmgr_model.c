/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_model.c
	@brief

	Description:  									\n
	Module: Portinig Layer / Initialization				\n

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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include <octo_common.h>
#include <db_svc.h>
#include <svc_si.h>
//#include <pmgr_model.h>
#include <mgr_live.h>
#include <mgr_search.h>
#include <mgr_swup.h>

#include "local_include/_xmgr_model.h"

HERROR pmgr_model_Init (void)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_Init_HMS1000S();

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_Init_IR4000HD();

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_Init_ICORDPRO();

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_Init_HDNEO();

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_Init_HGS1000S();

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_Init_HD9000I();

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_Init_HMS1000T();

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_Init_IHDR6100C();

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_Init_FVP4000T();

#elif defined(CONFIG_PROD_HMX4KBBC)
	return xmgr_model_Init_HMX4KBBC();

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_Init_DIGIMAXT2();

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_Init_HDFOXIRCONNECT();

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_Init_YSR2000();

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_Init_YS1000();

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_Init_CDIG2000C();

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_Init_ROCKYPOM();

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_Init_HA7100S();

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_Init_M1();

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/*
	Live Type:
*/
HERROR pmgr_model_LiveGetSiSpec (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_LiveGetSiSpec_HMS1000S (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_LiveGetSiSpec_IR4000HD (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_LiveGetSiSpec_ICORDPRO (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetLiveSiSpec_HDNEO (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_LiveGetSiSpec_HGS1000S (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_LiveGetSiSpec_HD9000I (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_LiveGetSiSpec_HMS1000T (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_LiveGetSiSpec_IHDR6100C (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_LiveGetSiSpec_FVP4000T (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return xmgr_model_LiveGetSiSpec_HMX4KBBC (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_LiveGetSiSpec_DIGIMAXT2 (pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_LiveGetSiSpec_HDFOXIRCONNECT(pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_LiveGetSiSpec_YSR2000(pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_LiveGetSiSpec_YS1000(pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_LiveGetSiSpec_CDIG2000C(pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_LiveGetSiSpec_ROCKYPOM(pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_LiveGetSiSpec_HA7100S(pstStart, pstSiSpec);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_LiveGetSiSpec_M1(pstStart, pstSiSpec);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/*
	Search Type:
*/
HERROR pmgr_model_SearchGetSiSpec (SvcSi_SearchOption_t *pstOption, SvcSi_SearchSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_SearchGetSiSpec_HMS1000S (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_SearchGetSiSpec_IR4000HD (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_SearchGetSiSpec_ICORDPRO (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetSearchSiSpec_HDNEO (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_SearchGetSiSpec_HGS1000S (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_SearchGetSiSpec_HD9000I (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_SearchGetSiSpec_HMS1000T (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_SearchGetSiSpec_IHDR6100C (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_SearchGetSiSpec_FVP4000T (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return xmgr_model_SearchGetSiSpec_HMX4KBBC (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_SearchGetSiSpec_DIGIMAXT2 (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_SearchGetSiSpec_HDFOXIRCONNECT (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_SearchGetSiSpec_YSR2000(pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_SearchGetSiSpec_YS1000(pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_SearchGetSiSpec_CDIG2000C(pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_SearchGetSiSpec_ROCKYPOM(pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_SearchGetSiSpec_HA7100S(pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_SearchGetSiSpec_M1(pstOption, pstSiSpec);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/*
	Search Type:
*/
HERROR pmgr_model_SearchGetLcnPolicy (DefaultLcnPolicy_t *peLcnPolicy, PreProgrammedChNumPolicy_t *pePreProgChNumPolicy)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_SearchGetLcnPolicy_HMS1000S (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_SearchGetLcnPolicy_IR4000HD (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_SearchGetLcnPolicy_ICORDPRO (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetLcnPolicy_HDNEO (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_SearchGetLcnPolicy_HGS1000S (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_SearchGetLcnPolicy_HD9000I (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_SearchGetLcnPolicy_HMS1000T (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_SearchGetLcnPolicy_IHDR6100C (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_SearchGetLcnPolicy_FVP4000T (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return xmgr_model_SearchGetLcnPolicy_HMX4KBBC (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_SearchGetLcnPolicy_DIGIMAXT2 (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_SearchGetLcnPolicy_HDFOXIRCONNECT (peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_SearchGetLcnPolicy_YSR2000(peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_SearchGetLcnPolicy_YS1000(peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_SearchGetLcnPolicy_CDIG2000C(peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_SearchGetLcnPolicy_ROCKYPOM(peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_SearchGetLcnPolicy_HA7100S(peLcnPolicy, pePreProgChNumPolicy);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_SearchGetLcnPolicy_M1(peLcnPolicy, pePreProgChNumPolicy);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/*
	Ext Search Type:
*/
HERROR pmgr_model_ExtsearchGetSiSpec (SvcSi_ExtSearchOption_t *pstOption, SvcSi_ExtSearchSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_IR4000HD)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_ICORDPRO)
		return xmgr_model_ExtsearchGetSiSpec_ICORDPRO (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
		return xmgr_model_GetExtSearchSiSpec_HDNEO (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
		return xmgr_model_ExtsearchGetSiSpec_HGS1000S (pstOption, pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_HMS1000T)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_IHDR6100C)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_FVP4000T)
		// not supported.
		return ERR_FAIL;
#elif defined(CONFIG_PROD_HMX4KBBC)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_DIGIMAXT2)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
		// not supported
		return ERR_FAIL;

#elif defined(CONFIG_PROD_YSR2000)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_YS1000)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_CDIG2000C)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_HA7100S)
		// not supported.
		return ERR_FAIL;

#elif defined(CONFIG_PROD_M1)
		// not supported.
		return ERR_FAIL;

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/*
	Time update Type:
*/
HERROR pmgr_model_TimeupdateGetSiSpec (SvcSi_TimeUpdateSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_TimeupdateGetSiSpec_HMS1000S (pstSiSpec);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_TimeupdateGetSiSpec_IR4000HD (pstSiSpec);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_TimeupdateGetSiSpec_ICORDPRO (pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetTimeupdateSiSpec_HDNEO (pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_TimeupdateGetSiSpec_HGS1000S (pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_TimeupdateGetSiSpec_HD9000I (pstSiSpec);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_TimeupdateGetSiSpec_HMS1000T (pstSiSpec);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_TimeupdateGetSiSpec_IHDR6100C (pstSiSpec);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_TimeupdateGetSiSpec_FVP4000T (pstSiSpec);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return xmgr_model_TimeupdateGetSiSpec_HMX4KBBC (pstSiSpec);

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_TimeupdateGetSiSpec_DIGIMAXT2 (pstSiSpec);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_TimeupdateGetSiSpec_HDFOXIRCONNECT (pstSiSpec);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_TimeupdateGetSiSpec_YSR2000(pstSiSpec);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_TimeupdateGetSiSpec_YS1000(pstSiSpec);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_TimeupdateGetSiSpec_CDIG2000C(pstSiSpec);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_TimeupdateGetSiSpec_ROCKYPOM(pstSiSpec);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_TimeupdateGetSiSpec_HA7100S(pstSiSpec);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_TimeupdateGetSiSpec_M1(pstSiSpec);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/*
	Ext Search Type:
*/
HERROR pmgr_model_SatrecGetSiSpec (SvcSi_SatRecordSpec_t *pstOption)
{
#if defined(CONFIG_PROD_HDR1000S)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_IR4000HD)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_ICORDPRO)
		return xmgr_model_SatrecGetSiSpec_ICORDPRO (pstOption);

#elif defined(CONFIG_PROD_HDNEO)
		return xmgr_model_GetSatRecSiSpec_HDNEO (pstOption);

#elif defined(CONFIG_PROD_HGS1000S)
		return xmgr_model_SatrecGetSiSpec_HGS1000S (pstOption);

#elif defined(CONFIG_PROD_HD9000I)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HMS1000T)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_IHDR6100C)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_FVP4000T)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HMX4KBBC)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_DIGIMAXT2)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
		// not supported
		return FALSE;

#elif defined(CONFIG_PROD_YSR2000)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_YS1000)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_CDIG2000C)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HA7100S)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_M1)
		// not supported.
		return FALSE;

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/*
	Transcording make section Type:
	Recording make section Type:
*/
HERROR pmgr_model_MakesectionGetSiSpec (SvcSi_MakeSectionSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_MakesectionGetSiSpec_HMS1000S (pstSiSpec);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_MakesectionGetSiSpec_IR4000HD (pstSiSpec);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_MakesectionGetSiSpec_ICORDPRO (pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetMakeSectionSiSpec_HDNEO (pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_MakesectionGetSiSpec_HGS1000S (pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_MakesectionGetSiSpec_HD9000I (pstSiSpec);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_MakesectionGetSiSpec_HMS1000T (pstSiSpec);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_MakesectionGetSiSpec_IHDR6100C (pstSiSpec);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_MakesectionGetSiSpec_FVP4000T (pstSiSpec);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return xmgr_model_MakesectionGetSiSpec_HMX4KBBC (pstSiSpec);

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_MakesectionGetSiSpec_DIGIMAXT2 (pstSiSpec);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_MakesectionGetSiSpec_HDFOXIRCONNECT (pstSiSpec);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_MakesectionGetSiSpec_YSR2000(pstSiSpec);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_MakesectionGetSiSpec_YS1000(pstSiSpec);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_MakesectionGetSiSpec_CDIG2000C(pstSiSpec);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_MakesectionGetSiSpec_ROCKYPOM(pstSiSpec);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_MakesectionGetSiSpec_HA7100S(pstSiSpec);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_MakesectionGetSiSpec_M1(pstSiSpec);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

HERROR 	pmgr_model_GetSiEsPriorityTable (SvcSi_EsPriorityTable_t *pstTable)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_GetSiEsPriorityTable_HMS1000S (pstTable);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_GetSiEsPriorityTable_IR4000HD (pstTable);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_GetSiEsPriorityTable_ICORDPRO (pstTable);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_GetSiEsPriorityTable_HGS1000S (pstTable);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_GetSiEsPriorityTable_HD9000I (pstTable);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_GetSiEsPriorityTable_HMS1000T (pstTable);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_GetSiEsPriorityTable_IHDR6100C (pstTable);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_GetSiEsPriorityTable_FVP4000T (pstTable);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return xmgr_model_GetSiEsPriorityTable_HMX4KBBC (pstTable);

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_GetSiEsPriorityTable_DIGIMAXT2 (pstTable);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_GetSiEsPriorityTable_HDFOXIRCONNECT (pstTable);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_GetSiEsPriorityTable_YSR2000(pstTable);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_GetSiEsPriorityTable_YS1000(pstTable);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_GetSiEsPriorityTable_CDIG2000C(pstTable);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_GetSiEsPriorityTable_ROCKYPOM(pstTable);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_GetSiEsPriorityTable_HA7100S(pstTable);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_GetSiEsPriorityTable_M1(pstTable);

#else
#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

HERROR	pmgr_model_SwupInitProduct(void)
{
	HERROR hErr = ERR_FAIL;

#if 0

#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	hErr = xmgr_model_SwupInitProduct_HMS1000S();

#elif defined(CONFIG_PROD_IR4000HD)
	hErr = xmgr_model_SwupInitProduct_IR4000HD();

#elif defined(CONFIG_PROD_ICORDPRO)
	hErr = xmgr_model_SwupInitProduct_ICORDPRO();

#elif defined(CONFIG_PROD_HDNEO)
	hErr = xmgr_model_InitSwUpdate_HDNEO();

#elif defined(CONFIG_PROD_HGS1000S)
	hErr = xmgr_model_SwupInitProduct_HGS1000S();

#elif defined(CONFIG_PROD_HD9000I)
	hErr = xmgr_model_SwupInitProduct_HD9000I();

#elif defined(CONFIG_PROD_HMS1000T)
	hErr = xmgr_model_SwupInitProduct_HMS1000T();

#elif defined(CONFIG_PROD_IHDR6100C)
	hErr = xmgr_model_SwupInitProduct_IHDR6100C();

#elif defined(CONFIG_PROD_FVP4000T)
	hErr = xmgr_model_SwupInitProduct_FVP4000T();

#elif defined(CONFIG_PROD_HMX4KBBC)
	hErr = xmgr_model_SwupInitProduct_HMX4KBBC();

#elif defined(CONFIG_PROD_DIGIMAXT2)
	hErr = xmgr_model_SwupInitProduct_DIGIMAXT2();

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	hErr = xmgr_model_SwupInitProduct_HDFOXIRCONNECT();

#elif defined(CONFIG_PROD_YSR2000)
	hErr = xmgr_model_SwupInitProduct_YSR2000();

#elif defined(CONFIG_PROD_YS1000)
	hErr = xmgr_model_SwupInitProduct_YS1000();

#elif defined(CONFIG_PROD_CDIG2000C)
	hErr = xmgr_model_SwupInitProduct_CDIG2000C();

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	hErr = xmgr_model_SwupInitProduct_ROCKYPOM();

#elif defined(CONFIG_PROD_HA7100S)
	hErr = xmgr_model_SwupInitProduct_HA7100S();

#elif defined(CONFIG_PROD_M1)
	hErr = xmgr_model_SwupInitProduct_M1();

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
#endif

	return hErr;
}

/*
	OTA SI Type
*/
HERROR pmgr_model_SwupGetSiSpec (MgrSwup_InArg_t *pstInArg, SvcSi_SwupCheckSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_SwupGetSiSpec_HMS1000S(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_SwupGetSiSpec_IR4000HD(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_SwupGetSiSpec_ICORDPRO(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetOtaCheckSiSpec_HDNEO(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_SwupGetSiSpec_HGS1000S(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_SwupGetSiSpec_HD9000I(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_SwupGetSiSpec_HMS1000T(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_SwupGetSiSpec_IHDR6100C(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_SwupGetSiSpec_FVP4000T(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return ERR_FAIL;

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_SwupGetSiSpec_DIGIMAXT2(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_SwupGetSiSpec_HDFOXIRCONNECT(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_SwupGetSiSpec_YSR2000(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_SwupGetSiSpec_YS1000(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_SwupGetSiSpec_CDIG2000C(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_SwupGetSiSpec_ROCKYPOM(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_SwupGetSiSpec_HA7100S(pstInArg, pstSiSpec);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_SwupGetSiSpec_M1(pstInArg, pstSiSpec);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/*
	PVR Type
*/
HERROR pmgr_model_RecordGetSiSpec (SvcSi_RecSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_RecordGetSiSpec_HMS1000S(pstSiSpec);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_RecordGetSiSpec_IR4000HD(pstSiSpec);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_RecordGetSiSpec_ICORDPRO(pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetRecSiSpec_HDNEO(pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_RecordGetSiSpec_HGS1000S(pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_RecordGetSiSpec_HD9000I(pstSiSpec);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_RecordGetSiSpec_HMS1000T(pstSiSpec);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_RecordGetSiSpec_IHDR6100C(pstSiSpec);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_RecordGetSiSpec_FVP4000T(pstSiSpec);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return ERR_FAIL;

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_RecordGetSiSpec_DIGIMAXT2(pstSiSpec);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_RecordGetSiSpec_HDFOXIRCONNECT(pstSiSpec);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_RecordGetSiSpec_YSR2000(pstSiSpec);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_RecordGetSiSpec_YS1000(pstSiSpec);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_RecordGetSiSpec_CDIG2000C(pstSiSpec);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_RecordGetSiSpec_ROCKYPOM(pstSiSpec);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_RecordGetSiSpec_HA7100S(pstSiSpec);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_RecordGetSiSpec_M1(pstSiSpec);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

HERROR pmgr_model_SatipGetSiSpec (SvcSi_SatipSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HDR1000S)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_IR4000HD)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_ICORDPRO)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HDNEO)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HGS1000S)
		return xmgr_model_SatipGetSiSpec_HGS1000S(pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HMS1000T)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_IHDR6100C)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_FVP4000T)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HMX4KBBC)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_DIGIMAXT2)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
		// not supported
		return FALSE;

#elif defined(CONFIG_PROD_YSR2000)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_YS1000)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_CDIG2000C)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_HA7100S)
		// not supported.
		return FALSE;

#elif defined(CONFIG_PROD_M1)
		// not supported.
		return FALSE;

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif

}

HERROR pmgr_model_PlaybackGetSiSpec (SvcSi_PbSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_PlaybackGetSiSpec_HMS1000S(pstSiSpec);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_PlaybackGetSiSpec_IR4000HD(pstSiSpec);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_PlaybackGetSiSpec_ICORDPRO(pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetPbSiSpec_HDNEO(pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_PlaybackGetSiSpec_HGS1000S(pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_PlaybackGetSiSpec_HD9000I(pstSiSpec);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_PlaybackGetSiSpec_HMS1000T(pstSiSpec);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_PlaybackGetSiSpec_IHDR6100C(pstSiSpec);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_PlaybackGetSiSpec_FVP4000T(pstSiSpec);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return ERR_FAIL;

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_PlaybackGetSiSpec_DIGIMAXT2(pstSiSpec);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_PlaybackGetSiSpec_HDFOXIRCONNECT(pstSiSpec);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_PlaybackGetSiSpec_YSR2000(pstSiSpec);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_PlaybackGetSiSpec_YS1000(pstSiSpec);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_PlaybackGetSiSpec_CDIG2000C(pstSiSpec);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_PlaybackGetSiSpec_ROCKYPOM(pstSiSpec);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_PlaybackGetSiSpec_HA7100S(pstSiSpec);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_PlaybackGetSiSpec_M1(pstSiSpec);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

HERROR pmgr_model_MediaplaybackGetSiSpec (SvcSi_MediaPbSpec_t *pstSiSpec)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_MediaplaybackGetSiSpec_HMS1000S(pstSiSpec);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_MediaplaybackGetSiSpec_IR4000HD(pstSiSpec);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_MediaplaybackGetSiSpec_ICORDPRO(pstSiSpec);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetMediaPbSiSpec_HDNEO(pstSiSpec);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_MediaplaybackGetSiSpec_HGS1000S(pstSiSpec);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_MediaplaybackGetSiSpec_HD9000I(pstSiSpec);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_MediaplaybackGetSiSpec_HMS1000T(pstSiSpec);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_MediaplaybackGetSiSpec_IHDR6100C(pstSiSpec);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_MediaplaybackGetSiSpec_FVP4000T(pstSiSpec);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return ERR_FAIL;

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_MediaplaybackGetSiSpec_DIGIMAXT2(pstSiSpec);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_MediaplaybackGetSiSpec_HDFOXIRCONNECT(pstSiSpec);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_MediaplaybackGetSiSpec_YSR2000(pstSiSpec);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_MediaplaybackGetSiSpec_YS1000(pstSiSpec);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_MediaplaybackGetSiSpec_CDIG2000C(pstSiSpec);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_MediaplaybackGetSiSpec_ROCKYPOM(pstSiSpec);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_MediaplaybackGetSiSpec_HA7100S(pstSiSpec);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_MediaplaybackGetSiSpec_M1(pstSiSpec);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}


HERROR pmgr_model_RecordGetCasTypes (HUINT32 *pulCasTypes)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_RecordGetCasTypes_HMS1000S(pulCasTypes);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_RecordGetCasTypes_IR4000HD(pulCasTypes);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_RecordGetCasTypes_ICORDPRO(pulCasTypes);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetRecCasTypes_HDNEO(pulCasTypes);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_RecordGetCasTypes_HGS1000S(pulCasTypes);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_RecordGetCasTypes_HD9000I(pulCasTypes);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_RecordGetCasTypes_HMS1000T(pulCasTypes);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_RecordGetCasTypes_IHDR6100C(pulCasTypes);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_RecordGetCasTypes_FVP4000T(pulCasTypes);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return ERR_FAIL;

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_RecordGetCasTypes_DIGIMAXT2(pulCasTypes);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_RecordGetCasTypes_HDFOXIRCONNECT(pulCasTypes);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_RecordGetCasTypes_YSR2000(pulCasTypes);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_RecordGetCasTypes_YS1000(pulCasTypes);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_RecordGetCasTypes_CDIG2000C(pulCasTypes);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_RecordGetCasTypes_ROCKYPOM(pulCasTypes);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_RecordGetCasTypes_HA7100S(pulCasTypes);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_RecordGetCasTypes_M1(pulCasTypes);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

HERROR pmgr_model_PlaybackGetCasTypes (HUINT32 *pulCasTypes)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_model_PlaybackGetCasTypes_HMS1000S(pulCasTypes);

#elif defined(CONFIG_PROD_IR4000HD)
	return xmgr_model_PlaybackGetCasTypes_IR4000HD(pulCasTypes);

#elif defined(CONFIG_PROD_ICORDPRO)
	return xmgr_model_PlaybackGetCasTypes_ICORDPRO(pulCasTypes);

#elif defined(CONFIG_PROD_HDNEO)
	return xmgr_model_GetPbCasTypes_HDNEO(pulCasTypes);

#elif defined(CONFIG_PROD_HGS1000S)
	return xmgr_model_PlaybackGetCasTypes_HGS1000S(pulCasTypes);

#elif defined(CONFIG_PROD_HD9000I)
	return xmgr_model_PlaybackGetCasTypes_HD9000I(pulCasTypes);

#elif defined(CONFIG_PROD_HMS1000T)
	return xmgr_model_PlaybackGetCasTypes_HMS1000T(pulCasTypes);

#elif defined(CONFIG_PROD_IHDR6100C)
	return xmgr_model_PlaybackGetCasTypes_IHDR6100C(pulCasTypes);

#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_model_PlaybackGetCasTypes_FVP4000T(pulCasTypes);

#elif defined(CONFIG_PROD_HMX4KBBC)
	return ERR_FAIL;

#elif defined(CONFIG_PROD_DIGIMAXT2)
	return xmgr_model_PlaybackGetCasTypes_DIGIMAXT2(pulCasTypes);

#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	return xmgr_model_PlaybackGetCasTypes_HDFOXIRCONNECT(pulCasTypes);

#elif defined(CONFIG_PROD_YSR2000)
	return xmgr_model_PlaybackGetCasTypes_YSR2000(pulCasTypes);

#elif defined(CONFIG_PROD_YS1000)
	return xmgr_model_PlaybackGetCasTypes_YS1000(pulCasTypes);

#elif defined(CONFIG_PROD_CDIG2000C)
	return xmgr_model_PlaybackGetCasTypes_CDIG2000C(pulCasTypes);

#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	return xmgr_model_PlaybackGetCasTypes_ROCKYPOM(pulCasTypes);

#elif defined(CONFIG_PROD_HA7100S)
	return xmgr_model_PlaybackGetCasTypes_HA7100S(pulCasTypes);

#elif defined(CONFIG_PROD_M1)
	return xmgr_model_PlaybackGetCasTypes_M1(pulCasTypes);

#else
	#error ("[%s:%d] This model does not activated for this function..!\n")
	return ERR_FAIL;
#endif
}

/* end of file */
