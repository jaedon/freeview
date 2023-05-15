/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>

#include <db_svc.h>

#include <svc_epg.h>
#include <xsvc_epg.h>

#include "_svc_epg_common.h"
#include "_svc_epg_context_mgr.h"
#include "_svc_epg_db_raw_lib.h"

#include <psvc_epg.h>

/** @brief local header file  */
#include "./local_include/_xsvc_epg.h"


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


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

// EPG Main에서 호출하는 함수...
HERROR	psvc_epg_InitProduct(void)
{
	HERROR hErr;

	hErr = ERR_FAIL;

#if 0

#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	hErr = xsvc_epg_Init_Hms1000s();
#elif defined(CONFIG_PROD_IR4000HD)
	hErr = xsvc_epg_Init_Ir4000hd();
#elif defined(CONFIG_PROD_ICORDPRO)
	hErr = xsvc_epg_Init_Icordpro();
#elif defined(CONFIG_PROD_HDNEO)
	hErr = xsvc_epg_Init_Hdneo();
#elif defined(CONFIG_PROD_HGS1000S)
	hErr = xsvc_epg_Init_Hgs1000s();
#elif defined(CONFIG_PROD_HD9000I)
	hErr = xsvc_epg_Init_hd9000i();
#elif defined(CONFIG_PROD_HMS1000T)
	hErr = xsvc_epg_Init_Hms1000t();
#elif defined(CONFIG_PROD_IHDR6100C)
	hErr = xsvc_epg_Init_Ihdr6100c();
#elif defined(CONFIG_PROD_FVP4000T)
	hErr = xsvc_epg_Init_fvp4000t();
#elif defined(CONFIG_PROD_HMX4KBBC)
	hErr = xsvc_epg_Init_Hmx4kbbc();
#elif defined(CONFIG_PROD_DIGIMAXT2)
	hErr = xsvc_epg_Init_Digimaxt2();
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	hErr = xsvc_epg_Init_HdFoxIrConnect();
#elif defined(CONFIG_PROD_YSR2000)
	hErr = xsvc_epg_Init_ysr2000();
#elif defined(CONFIG_PROD_YS1000)
	hErr = xsvc_epg_Init_ys1000();
#elif defined(CONFIG_PROD_CDIG2000C)
	hErr = xsvc_epg_Init_cdig2000c();
#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	hErr = xsvc_epg_Init_rockypom();
#elif defined(CONFIG_PROD_HA7100S)
	hErr = xsvc_epg_Init_Ha7100s();
#elif defined(CONFIG_PROD_M1)
	hErr = xsvc_epg_Init_M1();
#else
	#error ("xsvc_epg_Init_XXX for a Product is not activated!\n")
#endif

	return hErr;
}

HERROR	psvc_epg_InitResource(void)
{
	HERROR hErr;

	hErr = ERR_FAIL;

#if 0

#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	hErr = xsvc_epg_InitResource_Hms1000s();
#elif defined(CONFIG_PROD_IR4000HD)
	hErr = xsvc_epg_InitResource_Ir4000hd();
#elif defined(CONFIG_PROD_ICORDPRO)
	hErr = xsvc_epg_InitResource_Icordpro();
#elif defined(CONFIG_PROD_HDNEO)
	hErr = xsvc_epg_InitResource_Hdneo();
#elif defined(CONFIG_PROD_HGS1000S)
	hErr = xsvc_epg_InitResource_Hgs1000s();
#elif defined(CONFIG_PROD_HD9000I)
	hErr = xsvc_epg_InitResource_hd9000i();
#elif defined(CONFIG_PROD_HMS1000T)
	hErr = xsvc_epg_InitResource_Hms1000t();
#elif defined(CONFIG_PROD_IHDR6100C)
	hErr = xsvc_epg_InitResource_Ihdr6100c();
#elif defined(CONFIG_PROD_FVP4000T)
	hErr = xsvc_epg_InitResource_fvp4000t();
#elif defined(CONFIG_PROD_HMX4KBBC)
	hErr = xsvc_epg_InitResource_Hmx4kbbc();
#elif defined(CONFIG_PROD_DIGIMAXT2)
	hErr = xsvc_epg_InitResource_Digimaxt2();
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	hErr = xsvc_epg_InitResource_HdFoxIrConnect();
#elif defined(CONFIG_PROD_YSR2000)
	hErr = xsvc_epg_InitResource_ysr2000();
#elif defined(CONFIG_PROD_YS1000)
	hErr = xsvc_epg_InitResource_ys1000();
#elif defined(CONFIG_PROD_CDIG2000C)
	hErr = xsvc_epg_InitResource_cdig2000c();
#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	hErr = xsvc_epg_InitResource_rockypom();
#elif defined(CONFIG_PROD_HA7100S)
	hErr = xsvc_epg_InitResource_Ha7100s();
#elif defined(CONFIG_PROD_M1)
	hErr = xsvc_epg_InitResource_M1();
#else
	#error ("xsvc_epg_InitResource_XXX for a Product is not activated!\n")
#endif

	return hErr;
}

HERROR			psvc_epg_CreateCheckServiceList(void)
{
	return SVC_EPGSERVICE_ValidSvcCreateCheckServiceList();
}

HERROR			psvc_epg_GetSectionInfo(EPG_SpecType_e eSpec, HUINT8 *pucRawSection, svcEpg_ServiceInfo_t *pstSectionInfo)
{
	HERROR			hError = ERR_FAIL;

	switch(eSpec)
	{
		case eEPG_SPEC_BASE :
		default :
			hError = xsvc_epgeit_GetSectionInfo_Base(pucRawSection, pstSectionInfo);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xsvc_epgeit_GetSectionInfo_Base fail!!\n");
			}
			break;
	}

	return hError;
}

HERROR	psvc_epg_GetEpgTypeByTableId(HUINT8 ucTableId, SvcEpg_Type_b *pbwEpgType)
{
	return xsvc_epgdb_GetEpgTypeByTableId_Base(ucTableId, pbwEpgType);
}

svcEpg_RawProcessDataInit_t*	psvc_epg_GetEpgProcessInitTable(HUINT32* ulNumberOfTable)
{
#if defined(CONFIG_MW_EPG_TVTV)
	HERROR			hError = ERR_FAIL;
	HBOOL			bTvtvEnable = FALSE;

	HxLOG_Trace();
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_TVTV_ENABLE, (HUINT32 *)&bTvtvEnable, 0);
	if(hError != ERR_OK )
	{
		HxLOG_Error("eDB_PARAM_ITEM_TVTV_ENABLE fail\n");
		goto EXIT_INITTABLE_BASE;
	}

	if(bTvtvEnable == FALSE)
	{
		HxLOG_Info("bTvtvEnable == FALSE \n");
		goto EXIT_INITTABLE_BASE;
	}
	return xsvc_epgcontext_GetEpgProcessInitTable_Tvtv(ulNumberOfTable);

	EXIT_INITTABLE_BASE:
	return xsvc_epgcontext_GetEpgProcessInitTable_Base(ulNumberOfTable);
#else
	return xsvc_epgcontext_GetEpgProcessInitTable_Base(ulNumberOfTable);
#endif
}


#if defined(CONFIG_MW_EPG_TVTV)
// EPG COMMAND
HERROR psvc_epg_CmdCreateBuffer_Tvtv(void)
{
	return xsvc_epg_CreateBuffer_Tvtv();
}

HERROR psvc_epg_CmdCreateTvstInfo_Tvtv(void)
{
	return xsvc_epg_CreateTvstInfo_Tvtv();
}

HERROR psvc_epg_CmdCreateDgktInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epg_CreateDgktInfo_Tvtv(bNeedConverting);
}

HERROR	psvc_epg_CmdCreateTvtvSystemInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epg_CreateTvtvSystemInfo_Tvtv(bNeedConverting);
}

HERROR	psvc_epg_CmdCreateTsmtInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epg_CreateTsmtInfo_Tvtv(bNeedConverting);
}

HERROR	psvc_epg_CmdCreateLtotInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epg_CreateLtotInfo_Tvtv(bNeedConverting);
}

HERROR	psvc_epg_CmdCreateEpgInfo_Tvtv(HBOOL bNeedConverting)
{
	return xsvc_epg_CreateEpgInfo_Tvtv(bNeedConverting);
}

void psvc_epg_CmdInitBuffer_Tvtv(void)
{
	(void)xsvc_epg_InitBuffer_Tvtv();
}
#endif

/*********************** End of File ******************************/
