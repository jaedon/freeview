/**
	@file     	xmgr_clockrecover_base.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <hapi.h>
#include <db_svc.h>
#include <db_param.h>

#include <isosvc.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_svclist.h>
#include <mgr_time.h>
#include <mgr_models.h>
#include <pmgr_time.h>
#include "../local_include/_xmgr_time.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockRefTp_Base[] =
{ /* Clock time search will be done according to the order of the below TP info until the clock time is successfully set up.
	Modify the Antenna order and list according to the clock setup scenario */
#if defined(CONFIG_MW_CH_SATELLITE)
	{eREF_SAT_TP, eSatType_ASTRA_1, 12604, 22000, eDxSAT_CODERATE_5_6, eDxSAT_POLAR_HOR, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},			// WDR
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO) || defined(CONFIG_PROD_HGS1000S)
	{eREF_SAT_TP, eSatType_ASTRA_1, 12552, 22000, eDxSAT_CODERATE_5_6, eDxSAT_POLAR_VER, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},
	{eREF_SAT_TP, eSatType_ASTRA_1, 10832, 22000, eDxSAT_CODERATE_5_6, eDxSAT_POLAR_HOR, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},
#endif
#endif
	{eREF_WAKEUP_SVC, 0, 0, 0, 0, 0, 0, 0, 0},
	{eREF_LAST_SVC, 0, 0, 0, 0, 0, 0, 0, 0},
#ifdef CONFIG_MW_CH_SATELLITE
	{eREF_FIRST_SVC_SAT, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
#ifdef CONFIG_MW_CH_TERRESTRIAL
	{eREF_FIRST_SVC_TER, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
#ifdef CONFIG_MW_CH_CABLE
	{eREF_FIRST_SVC_CAB, 0, 0, 0, 0, 0, 0, 0, 0},
#endif
	{eREF_END, 0, 0, 0, 0, 0, 0, 0, 0} /* eREF_END will terminate the clock setup process */
};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR xmgr_time_ClockRecoverGetDstMode_Base (VK_CLOCK_DST_MODE *peDstMode)
{
	return ERR_FAIL;
}

MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_Base (void)
{
	return s_stClockRefTp_Base;
}

HERROR xmgr_time_ClockRecoverSearchByFirstSvc_Base (MgrTime_ClockRecoverRefMethod_e eRefMethod, DxTuneParam_t	*tuningInfo)
{
	DbSvc_Attr_t		svcAttr;
	Handle_t		hSvc;
	DbSvc_TsInfo_t	tsInfo;
	HERROR			hErr;

	/* Setup satellite attribute */
	MGR_SVCLIST_InitSvcAttribute(&svcAttr);

	switch(eRefMethod)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eREF_FIRST_SVC_SAT:
			svcAttr.eOrgDeliType = eDxDELIVERY_TYPE_SAT;
			HxLOG_Print("[Clock Setup] xmgr_time_ClockRecoverSearchByFirstSvc_Base( SATELITE )\n");
			break;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eREF_FIRST_SVC_TER:
			svcAttr.eOrgDeliType = eDxDELIVERY_TYPE_TER;
			HxLOG_Print("[Clock Setup] xmgr_time_ClockRecoverSearchByFirstSvc_Base( TERRESTRIAL )\n");
			break;
#endif
#if defined(CONFIG_MW_CH_CABLE)
		case eREF_FIRST_SVC_CAB:
			svcAttr.eOrgDeliType = eDxDELIVERY_TYPE_CAB;
			HxLOG_Print("[Clock Setup] xmgr_time_ClockRecoverSearchByFirstSvc_Base( CABLE )\n");
			break;
#endif
		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	hErr = DB_SVC_FindServiceHandle(eSvcGroup_All, &svcAttr, 0, eSvcFindDirection_First, &hSvc);
	if(hErr == ERR_OK)
	{
		hErr = DB_SVC_GetTsInfoByServiceHandle(hSvc, &tsInfo);
		if(hErr == ERR_OK)
		{
			hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&tsInfo, tuningInfo);
			if(ERR_OK != hErr)			HxLOG_Critical("Critical Eror!!!\n");
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR xmgr_time_Init_Base (void)
{
	unsigned long	utCurrTime;
	HCHAR			szTzSend[TZ_NAME_LENGTH] = {0,};
	HCHAR 			system_command[128 + 5] = {0,};
	HERROR			hErr;
	HUINT64			ulTzFileSize = 0;
	HBOOL			bTzFileExist = FALSE;
	HxFILE_t		hTzFile = NULL;

	hErr = VK_CLOCK_GetTime (&utCurrTime);
	if (VK_OK != hErr)
	{
		utCurrTime = 0;
	}

	if(HLIB_FILE_Exist((const HUINT8 *)TZ_NAME_PATH) == TRUE)
	{
		bTzFileExist = TRUE;
		hTzFile = HLIB_FILE_Open((const HUINT8 *)TZ_NAME_PATH, (const HUINT8 *)"r");

		if(hTzFile != NULL)
		{
			HLIB_FILE_Size(hTzFile, &ulTzFileSize);
			HLIB_FILE_Close(hTzFile);
		}
	}

	if((bTzFileExist == FALSE) || (ulTzFileSize == 0))
	{
		HxCountry_e 	eCountryId = eCountryID_NONE;
		HUINT32			ulRegionId = 0;

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
		}

		hErr = HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID, &ulRegionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID) Error!!\n");
			ulRegionId = 0;
		}

		// 현재는 op config 로 설정하지만, conutry code 가 셋팅되면 country code 를 얻어와 사용해야한다.
		hErr = HLIB_DATETIME_MakeTzFileFromKnownTzCode(utCurrTime, eCountryId, ulRegionId, szTzSend);

		HxSTD_snprintf(system_command, 128, "%s%c", szTzSend, 0x0a);
		HLIB_FILE_WriteFile((const HUINT8 *)TZ_NAME_PATH, system_command, HxSTD_StrLen(system_command));

		HLIB_FILE_Delete((const HUINT8 *)TZ_NAME_PATH_TMP);
		HLIB_FILE_Copy((const HUINT8 *)TZ_NAME_PATH, (const HUINT8 *)TZ_NAME_PATH_TMP);
		// 이 루트를 탄다는 것은 이제 정상이다. 박스 처음 부팅후 강제로 TZ 를 지우지않는이상 여기는 들어올리 없다.
	}

#if defined(CONFIG_MW_SI_LOCALTIME)
	if (HLIB_FILE_Exist(LOCALTIME_NAME_PATH_TMP) != TRUE)
	{
		HERROR 	hErr = ERR_FAIL;
		HxCountry_e eCountryId = eCountryID_NONE;
		HUINT32	ulRegionId = 0;

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
		}

		hErr = HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID, &ulRegionId);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID) Error!!\n");
			ulRegionId = 0;
		}

		(void)SVC_SI_UpdateLocaltime_linkTzDB(eCountryId, ulRegionId);
	}
#endif
	// TZ 가 존재해도 여기서 먼가 비교해서 변경 하면 안된다.
	// 일단 존재하는 TZ 는 최신이고, UTC 시간 정보 없이 어느것이 최신인지 비교할 수 없다.

	return ERR_OK;
}

HERROR xmgr_time_SetGmtOffset_Base(HINT32 nGmtOffset, HBOOL bUpdateTzFile)
{
	DB_PARAM_SetItem(eDB_PARAM_ITEM_GMT_OFFSET_MINUTES, nGmtOffset, 0);

	if (bUpdateTzFile)
	{
		HCHAR	szTimezone[TZ_NAME_LENGTH]; 	//[MAX_TIMEZONE_STR_LEN];
		HCHAR	szSystemCmd[384];

		//	Set TZ
		HxSTD_PrintToStrN (szTimezone, TZ_NAME_LENGTH, "GMT%c%02d:%02d", (nGmtOffset > 0) ? '-':'+', (nGmtOffset / 60), (nGmtOffset % 60));

		// Write down to /etc/TZ file.
		HxSTD_snprintf(szSystemCmd, 128, "%s%c", szTimezone, 0x0a);
		HLIB_FILE_WriteFile((const HUINT8 *)TZ_NAME_PATH, szSystemCmd, HxSTD_StrLen(szSystemCmd));
		HLIB_FILE_Delete((const HUINT8 *)TZ_NAME_PATH_TMP);
		HLIB_FILE_Copy((const HUINT8 *)TZ_NAME_PATH, (const HUINT8 *)TZ_NAME_PATH_TMP);
	}

	return ERR_OK;
}

HERROR xmgr_time_ClockRecoverSearchByTerTp_Base(HUINT32 ulRefTpIdx, DxTuneParam_t	*tuningInfo)
{
	return ERR_FAIL;
}

HERROR xmgr_time_ClockRecoverTune_Base(Handle_t hAction,DxTuneParam_t	*tuningInfo)
{
	HERROR					hErr;
	SvcSi_TimeUpdateSpec_t  stTimeUpdateSpec;

	HxLOG_Print("[Clock Setup] MGR_TIME_ClockRecoverTune ()\n");

	HxSTD_MemSet(&stTimeUpdateSpec, 0x00, sizeof(SvcSi_TimeUpdateSpec_t));
	hErr = MGR_MODEL_TimeupdateGetSiSpec(&stTimeUpdateSpec);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return hErr;
	}

	hErr = SVC_SI_StartTimeupdate(hAction, &stTimeUpdateSpec, FALSE /* TOT */, FALSE /* sync */);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	return hErr;
}


/*********************** End of File ******************************/
