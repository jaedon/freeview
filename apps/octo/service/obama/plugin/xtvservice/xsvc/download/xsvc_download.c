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
#include <stdio.h>
#include <string.h>
#include <octo_common.h>

#include <xsvc_download.h>

/** @brief local header file  */
#include <_svc_download.h>
#include "./local_include/_xsvc_download_hbbtv.h"
#include "./local_include/_xsvc_download_dvb_ssu.h"
#include "./local_include/_xsvc_download_satrecord.h"

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

/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
*/

HERROR xsvc_download_InitDsmcc_Hbbtv(void)
{
#if defined(CONFIG_MW_SI_AIT)
	SvcDownload_CB_t	stCbList;

	HxSTD_memset(&stCbList, 0, sizeof(SvcDownload_CB_t));

	stCbList.pfnInit		= xsvc_download_InitAction_Hbbtv;
	stCbList.pfnTerm		= xsvc_download_TermAction_Hbbtv;
	stCbList.pfnGetData		= xsvc_download_GetData_Hbbtv;
	stCbList.pfnFreeData	= xsvc_download_FreeData_Hbbtv;

	return svc_download_ActionRegister(eDOWN_DOWNLOAD_HBBTV, &stCbList);
#else
	return ERR_FAIL;
#endif
}

HERROR xsvc_download_InitDvbssu(void)
{
#if defined(CONFIG_MW_DOWNLOAD_OTA_IMAGE)
	SvcDownload_CB_t	stCbList;

	HxSTD_memset(&stCbList, 0, sizeof(SvcDownload_CB_t));

	stCbList.pfnInit					= xsvc_download_InitAction_Dvbssu;
	stCbList.pfnTerm					= xsvc_download_TermAction_Dvbssu;
	stCbList.pfnCheckDonwloadComplete	= xsvc_download_CheckDownloadComplete_Dvbssu;
	stCbList.pfnClearDownloadInfo		= xsvc_download_ClearDownloadInfo_Dvbssu;
	stCbList.pfnInitMetaInfo			= xsvc_download_InitMetaInfo_Dvbssu;

	return svc_download_ActionRegister(eDOWN_DOWNLOAD_OTA_IMAGE, &stCbList);
#else
	return ERR_FAIL;
#endif
}

HERROR xsvc_download_InitSatRecord(void)
{
#if defined(CONFIG_MW_DOWNLOAD_SAT_RECORD)
	SvcDownload_CB_t	stCbList;

	VK_memset(&stCbList, 0, sizeof(SvcDownload_CB_t));

	stCbList.pfnInit		= xsvc_download_InitAction_SatRecord;
	stCbList.pfnTerm		= xsvc_download_TermAction_SatRecord;
	stCbList.pfnGetData		= xsvc_download_GetData_SatRecord;
	stCbList.pfnFreeData	= xsvc_download_FreeData_SatRecord;

	return svc_download_ActionRegister(eDOWN_DOWNLOAD_SATRECORD, &stCbList);
#else
	return ERR_FAIL;
#endif
}

/*********************** End of File ******************************/
