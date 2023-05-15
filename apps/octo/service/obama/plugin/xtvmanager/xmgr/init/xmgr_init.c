/**
	@file     	pmgr_init.c
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
#include <bus.h>
#include <mgr_common.h>
#include <mgr_appmgr.h>
#include <mgr_init.h>
//#include <xmgr_init.h>

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


#ifdef CONFIG_TEST_AUTOMATIC_OTA_TEST
#include <mgr_download.h>
void XMGR_INIT_TestAutomaticOtaTestAfterAction (void)
{
	HUINT32 		 ulDiTuneDataSize;
	DbSvc_TsInfo_t	 stTsInfo;
	SvcSi_SwupSignal_t	 stOtaInfo;
	DxTuneParam_t  stChTuneInfo;
//	MgrDownload_Info_t	 stDnInfo;
//	void			*pvDiTuneData;
	HERROR			 hErr;

	hErr = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	// 1. Tuning Info 를 DD용으로 converting 한다.
	// 1-1. DB Tuning Info -> Ch Tuning Info
	hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
	if (hErr != ERR_OK)
	{
		return;
	}

#if 0
	// 1-2. Ch Tuning Info -> DD Tuning Data
	hErr = SVC_CH_MakeDiTuningDataByChTuningParam (&stChTuneInfo, &ulDiTuneDataSize, &pvDiTuneData);
	if (hErr != ERR_OK || pvDiTuneData == NULL)
	{
		return;
	}
#endif

	MGR_SWUPINFO_ResetSearchedInfo();

	// 2. Now, store the informations
	//	당장 Reboot 하지 않아도 곧 Reboot할 수 있으니 적어놓도록 한다.
#if defined(CONFIG_OTA_DTT_UK)
	SVC_SYS_SetOtaType (eSWUP_INFO_SLOT_0, eSWUP_DOWNLOAD_DTG);
#else
	SVC_SYS_SetOtaType (eSWUP_INFO_SLOT_0, eSWUP_DOWNLOAD_DVBSSU);
#endif
	SVC_SYS_SetOtaPid (eSWUP_INFO_SLOT_0, stOtaInfo.usPid);
//	SVC_SYS_SetOtaTuningInfo (eSWUP_INFO_SLOT_0, &stChTuneInfo);

	SVC_SYS_SetSystemVersion (eVERSION_TYPE_NEXTOTA, stOtaInfo.ulDataVersion);
//	SVC_CH_FreeDiTuningData (pvDiTuneData);

	// Reboot 한다.
	SVC_SYS_SetOtaFlag (eSWUP_INFO_SLOT_0, eSWUP_FLAG_INFO_DETECTED);
	MGR_ACTION_RebootSystem();

	return;
}
#endif


/*********************** End of File ******************************/
