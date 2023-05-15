/**
	@file     	pmgr_swup.c
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
#include <mgr_swup.h>

#include <xmgr_swup.h>

#include <pmgr_swup.h>

#include "local_include/_xmgr_swup_wakeup.h"
#include "local_include/_xmgr_swup_usb.h"
#include "local_include/_xmgr_swup.h"

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
STATIC DxSwUpdate_Spec_e			 s_eMultipleSwUpdateSpec = eDxSwUpdate_SPEC_NONE;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR 	pmgr_swup_GetSpec (DxSwUpdate_Spec_e *peSwUpdateSpec)
{
	if (peSwUpdateSpec == NULL)					{ return ERR_BUS_TARGET_NULL; }

#if defined(CONFIG_OTA_NONE)
	{
		*peSwUpdateSpec = eDxSwUpdate_SPEC_NONE;
	}
#elif defined(CONFIG_OTA_SINGLE)
	{
	#if defined(CONFIG_OTA_ASTRA)
		*peSwUpdateSpec = eDxSwUpdate_SPEC_ASTRA;

	#elif defined(CONFIG_OTA_DVBSSU)
		*peSwUpdateSpec = eDxSwUpdate_SPEC_DVBSSU;

	#elif defined(CONFIG_OTA_DTT_UK) || defined(CONFIG_OTA_DTT_EU)|| defined(CONFIG_OTA_DTT_UK_SSU)
		*peSwUpdateSpec = eDxSwUpdate_SPEC_DTT;

	#elif defined(CONFIG_OTA_IRDETO)
		*peSwUpdateSpec = eDxSwUpdate_SPEC_IRDETO;

	#elif defined(CONFIG_OTA_HUMAX)
		*peSwUpdateSpec = eDxSwUpdate_SPEC_HUMAX;

	#elif defined(CONFIG_OTA_SES)
		*peSwUpdateSpec = eDxSwUpdate_SPEC_SES;

	// 새 사양이 추가될 때마다 여기에 추가한다.

	// 추가 사양 끝
	#else
		*peSwUpdateSpec = eDxSwUpdate_SPEC_NONE;

	#endif
	}
#elif defined(CONFIG_OTA_MULTIPLE)
{
	//HxLOG_Debug("s_eMultipleOtaSpec : %d\n", s_eMultipleOtaSpec);
	*peSwUpdateSpec = s_eMultipleSwUpdateSpec;
}
#else
{
	*peSwUpdateSpec = eDxSwUpdate_SPEC_NONE;
}
#endif

	return ERR_OK;
}
DxSwUpdate_Spec_e pmgr_swup_GetMultipleSpec (DxBatchType_e eBatchType)
// batch로 ota를 동작 시킬시에 multiple spec 인경우 batch type 에 따라 별도로 spec을 setting 하게 함.
{
	DxSwUpdate_Spec_e eSwUpSpec = eDxSwUpdate_SPEC_NONE;
#if defined(CONFIG_OP_MIDDLE_EAST)
	eSwUpSpec = eDxSwUpdate_SPEC_HUMAX;
#else
	pmgr_swup_GetSpec (&eSwUpSpec);
#endif
	return eSwUpSpec;
}

STATIC HCHAR* _swup_GetStrOfDxSwUpdate_Spec_e(DxSwUpdate_Spec_e e)
{
#ifdef CONFIG_DEBUG
	switch(e)
	{
	ENUM_TO_STR(eDxSwUpdate_SPEC_NONE);
	ENUM_TO_STR(eDxSwUpdate_SPEC_ASTRA);
	ENUM_TO_STR(eDxSwUpdate_SPEC_DVBSSU);
	ENUM_TO_STR(eDxSwUpdate_SPEC_DTT);
	ENUM_TO_STR(eDxSwUpdate_SPEC_SKYD);
	ENUM_TO_STR(eDxSwUpdate_SPEC_SES);
	ENUM_TO_STR(eDxSwUpdate_SPEC_IRDETO);
	ENUM_TO_STR(eDxSwUpdate_SPEC_HUMAX);
	ENUM_TO_STR(eDxSwUpdate_SPEC_FSATSSU);
	ENUM_TO_STR(eDxSwUpdate_SPEC_UPC);
	ENUM_TO_STR(eDxSwUpdate_SPEC_KDG);
	default:
		return "??";
	}
#else
	return "";
#endif
}

HERROR pmgr_swup_SetSpec (DxSwUpdate_Spec_e eSwUpdateSpec)
{
	HxLOG_Print("(%s) -> (%s)\n", _swup_GetStrOfDxSwUpdate_Spec_e(s_eMultipleSwUpdateSpec), _swup_GetStrOfDxSwUpdate_Spec_e(eSwUpdateSpec));
	s_eMultipleSwUpdateSpec = eSwUpdateSpec;
	return ERR_OK;
}

BUS_Callback_t pmgr_swup_DownloadGetAction (DxSwUpdate_Spec_e eSwUpdateSpec)
{
#if defined(CONFIG_MW_DOWNLOAD_OTA_IMAGE)
	BUS_Callback_t	pfnProc = NULL;

	switch(eSwUpdateSpec)
	{
	case eDxSwUpdate_SPEC_NONE:
		break;

	case eDxSwUpdate_SPEC_ASTRA:
	case eDxSwUpdate_SPEC_DVBSSU:
	case eDxSwUpdate_SPEC_DTT:
	case eDxSwUpdate_SPEC_HUMAX:
	case eDxSwUpdate_SPEC_SES:
		pfnProc = xproc_swup_Download_Dvbssu;
		break;

	default:
		break;
	}

	return pfnProc;
#else
	return NULL;
#endif
}

BUS_Callback_t pmgr_swup_DetectGetAction (void)
{
	DxSwUpdate_Spec_e	 eSwUpdateSpec;
	HERROR					 hErr;

	hErr = MGR_SWUP_GetSpec(&eSwUpdateSpec);
	if (hErr != ERR_OK)
	{
		return NULL;
	}
	HxLOG_Print("eSwUpdateSpec[%d]\n", eSwUpdateSpec);

	switch(eSwUpdateSpec)
	{
#if defined(CONFIG_OTA_DVBSSU)
	case eDxSwUpdate_SPEC_DVBSSU:		return xproc_swup_Detect_Dvbssu;
#endif
#if defined(CONFIG_OTA_ASTRA)
	case eDxSwUpdate_SPEC_ASTRA:			return xproc_swup_Detect_Astra;
#endif
#if defined(CONFIG_OTA_HUMAX)
	case eDxSwUpdate_SPEC_HUMAX:			return xproc_swup_Detect_Humax;
#endif
#if defined(CONFIG_OTA_IRDETO)
	case eDxSwUpdate_SPEC_IRDETO:			return xproc_swup_Detect_Irdeto;
#endif
#if defined(CONFIG_OTA_DTT_UK) || defined(CONFIG_OTA_DTT_EU)|| defined(CONFIG_OTA_DTT_UK_SSU)
	case eDxSwUpdate_SPEC_DTT:				return xproc_swup_Detect_Dtt;
#endif
#if defined(CONFIG_OTA_SES)
	case eDxSwUpdate_SPEC_SES:				return xproc_swup_Detect_Ses;
#endif

		default:
			break;
	}

	return NULL;
}

HERROR 	pmgr_swup_SetDefaultTp (void)
{
	DxSwUpdate_Spec_e	 eSwUpdateSpec;
	HERROR					 hErr;

	hErr = MGR_SWUP_GetSpec (&eSwUpdateSpec);
	if (hErr != ERR_OK) 				{ return ERR_FAIL; }

	switch (eSwUpdateSpec)
	{
#if defined(CONFIG_OTA_ASTRA)
	case eDxSwUpdate_SPEC_ASTRA:
		hErr = xmgr_swup_SetDefaultHomeTsInfo_Astra();
		break;
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eDxSwUpdate_SPEC_DVBSSU:
		hErr = xmgr_swup_SetDefaultHomeTsInfo_Dvbssu();
		break;
#endif

#if defined(CONFIG_OTA_DTT_UK) || defined(CONFIG_OTA_DTT_EU)|| defined(CONFIG_OTA_DTT_UK_SSU)
	case eDxSwUpdate_SPEC_DTT:
		hErr = xmgr_swup_SetDefaultHomeTsInfo_Dtt();
		break;
#endif

#if defined(CONFIG_OTA_IRDETO)
	case eDxSwUpdate_SPEC_IRDETO:
		hErr = xmgr_swup_SetDefaultHomeTsInfo_Irdeto();
		break;
#endif

#if defined(CONFIG_OTA_HUMAX)
	case eDxSwUpdate_SPEC_HUMAX:
		hErr = xmgr_swup_SetDefaultHomeTsInfo_Humax();
		break;
#endif

#if defined(CONFIG_OTA_SES)
	case eDxSwUpdate_SPEC_SES:
		hErr = xmgr_swup_SetDefaultHomeTsInfo_Ses();
		break;
#endif
	// 새 사양이 추가될 때마다 여기에 추가한다.

	// 추가 사양 끝

	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HERROR	pmgr_swup_FindUsbSwImage(Handle_t hSWUpdate, HCHAR *szPath, SvcSwUpdate_UsbDetectInfo_t *pstContents)
{
	DxSwUpdate_Spec_e	 eSwUpdateSpec;
	HERROR				hErr;

	hErr = MGR_SWUP_GetSpec (&eSwUpdateSpec);
	if (hErr != ERR_OK) 				{ return ERR_FAIL; }

	switch (eSwUpdateSpec)
	{
#if !defined(CONFIG_OTA_HUMAX)
	case eDxSwUpdate_SPEC_ASTRA:
	case eDxSwUpdate_SPEC_DVBSSU:
	case eDxSwUpdate_SPEC_DTT:
	case eDxSwUpdate_SPEC_SES:
		hErr = xmgr_swup_UsbFindImage(hSWUpdate, szPath, pstContents);
		break;
#endif

#if defined(CONFIG_OTA_HUMAX)
	case eDxSwUpdate_SPEC_HUMAX:
		hErr = xmgr_swup_UsbFindImage_Humax(hSWUpdate, szPath, pstContents);
		break;
#endif

	default:
		hErr = ERR_FAIL;
		break;
	}

	return hErr;
}

HUINT32 pmgr_swup_GetStartUpdateSource(DxBatchType_e eBatchType)
{
	DxSwUpdate_Spec_e		eSwUpdateSpec;

	eSwUpdateSpec = eDxSwUpdate_SPEC_NONE;
	(void)MGR_SWUP_GetSpec (&eSwUpdateSpec);

	switch (eSwUpdateSpec)
	{
#if defined(CONFIG_OTA_SES)
	case eDxSwUpdate_SPEC_SES:
		return xmgr_swup_GetStartUpdateSource_Ses();
#endif

#if defined(CONFIG_OTA_DVBSSU)
	case eDxSwUpdate_SPEC_DVBSSU:
		return XMGR_SWUP_GetStartSource_DvbSsu(eBatchType);
#endif

	default:
		HxLOG_Print("eBatchType[%x]\n", eBatchType);

		switch(eBatchType)
		{
		case eDxBAT_TYPE_NONE:
			return (HUINT32)XMGR_SWUP_GetStartSource();

		/*
			Even though usb starts first, it would not be a problem.
			1) Usually, usb -> ip -> rf
			2) Another case, usb -> rf -> ip : TDC models, 20151014
		*/
		case eDxBAT_TYPE_STANDBY:
			return (HUINT32)XMGR_SWUP_GetStartSource();

		default:
			return (HUINT32)eDxSWUPDATE_SOURCE_Ip;
		}

		break;
	}

	return (HUINT32)eDxSWUPDATE_SOURCE_Ip;
}

/*********************** End of File ******************************/
