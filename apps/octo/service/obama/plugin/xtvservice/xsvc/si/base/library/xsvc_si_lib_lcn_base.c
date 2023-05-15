/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_lcn_base.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIBRARY		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_time.h>
#if defined(CONFIG_OP_NORDIG)
#include "_xsvc_si_lib_lcn_nordig.h"
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#define ______________________________________________
#define DEFAULT_LCN_BASE_RAY_SAT_TV				1
#define DEFAULT_LCN_BASE_RAY_SAT_RADIO			1
#define DEFAULT_LCN_BASE_RAY_TER_TV				1
#define DEFAULT_LCN_BASE_RAY_TER_RADIO			1
#define DEFAULT_LCN_BASE_RAY_CAB_TV				1
#define DEFAULT_LCN_BASE_RAY_CAB_RADIO			1
#define DEFAULT_LCN_BASE_RAY_ANA				1
#define ______________________________________________
#define DEFAULT_LCN_BASE_ALVIN_SAT_TV			1
#define DEFAULT_LCN_BASE_ALVIN_SAT_RADIO		1
#define DEFAULT_LCN_BASE_ALVIN_TER_TV			1
#define DEFAULT_LCN_BASE_ALVIN_TER_RADIO		1
#define DEFAULT_LCN_BASE_ALVIN_CAB_TV			1
#define DEFAULT_LCN_BASE_ALVIN_CAB_RADIO		1
#define DEFAULT_LCN_BASE_ALVIN_ANA				1
#define ______________________________________________
#define DEFAULT_LCN_BASE_APSLCN_TV				100
#define DEFAULT_LCN_BASE_APSLCN_RADIO			100
#define ______________________________________________
#define DEFAULT_LCN_BASE_SKYDLCN_TV				1000
#define DEFAULT_LCN_BASE_SKYDLCN_RADIO			1000
#define ______________________________________________
#define DEFAULT_LCN_BASE_KDGLCN_TV				1000
#define DEFAULT_LCN_BASE_KDGLCN_RADIO			1000
#define ______________________________________________
#define DEFAULT_LCN_BASE_NTVPLUS_TV				1000
#define DEFAULT_LCN_BASE_NTVPLUS_RADIO			2001
#define ______________________________________________
#define DEFAULT_LCN_BASE_UPC_TV					300
#define DEFAULT_LCN_BASE_UPC_RADIO				2000
#define ______________________________________________
#define DEFAULT_LCN_BASE_ME_TV					1
#define DEFAULT_LCN_BASE_ME_RADIO				5000
#define ______________________________________________
#define DEFAULT_LCN_BASE_NONFSAT_TV				5000
#define DEFAULT_LCN_BASE_NONFSAT_RADIO			8000
#define ______________________________________________
#define DEFAULT_LCN_BASE_AUSDTT_TV				350
#define DEFAULT_LCN_BASE_AUSDTT_RADIO			350
#define ______________________________________________
#define DEFAULT_LCN_BASE_ERTELECOM_TV			2000
#define DEFAULT_LCN_BASE_ERTELECOM_RADIO		2000
#define ______________________________________________
#define DEFAULT_LCN_BASE_UKDTT_TV				800
#define DEFAULT_LCN_BASE_UKDTT_RADIO			800
#define ______________________________________________
#define DEFAULT_LCN_BASE_ZIGGO_TV				1000
#define DEFAULT_LCN_BASE_ZIGGO_RADIO			1000
#define ______________________________________________
#define DEFAULT_LCN_BASE_ITADTT_TV				850
#define DEFAULT_LCN_BASE_ITADTT_RADIO			850
#define ______________________________________________
#define DEFAULT_LCN_BASE_NORDIG_TV				1
#define DEFAULT_LCN_BASE_NORDIG_RADIO			1
#define ______________________________________________


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___

HUINT16 xsvc_si_LibLcnGetSvcNumBase(DefaultLcnPolicy_t eLcnPolicy, DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType)
{
	HERROR		hErr;
	HINT32		nMaxLcn;

	/* 1st condition : LCN Policy */
	switch (eLcnPolicy)
	{
	case eDEFAULTLCN_NORMAL:
	case eDEFAULTLCN_AFTER_LCN:
	case eDEFAULTLCN_AFTER_LCN_TYPEDEVIDE:
	case eDEFAULTLCN_SPAIN_TDT:
		/* 2nd condition : Delivery Type */
		switch(eOrgDeliType)
		{
		case eDxDELIVERY_TYPE_SAT:
			/* 3rd condition : service type */
			if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_RAY_SAT_TV;
			else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_RAY_SAT_RADIO;
			break;
		case eDxDELIVERY_TYPE_TER:
			if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_RAY_TER_TV;
			else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_RAY_TER_RADIO;
			break;
		case eDxDELIVERY_TYPE_CAB:
			if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_RAY_CAB_TV;
			else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_RAY_CAB_RADIO;
			break;
		default:
			break;
		}
		break;

	case eDEFAULTLCN_ALVIN_NORMAL:
		/* 2nd condition : Delivery Type */
		switch(eOrgDeliType)
		{
		case eDxDELIVERY_TYPE_SAT:
			/* 3rd condition : service type */
			if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_ALVIN_SAT_TV;
			else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_ALVIN_SAT_RADIO;
			break;
		case eDxDELIVERY_TYPE_TER:
			if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_ALVIN_TER_TV;
			else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_ALVIN_TER_RADIO;
			break;
		case eDxDELIVERY_TYPE_CAB:
			if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_ALVIN_CAB_TV;
			else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_ALVIN_CAB_RADIO;
			break;
		default:
			break;
		}
		break;

	case eDEFAULTLCN_APSLCN:
		hErr = DB_SVC_GetMaxLcnInLcnServices (eOrgDeliType, eSvcType, &nMaxLcn);

		switch (eSvcType)
		{
		case eDxSVC_TYPE_TV:		return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_APSLCN_TV) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_APSLCN_TV;
		case eDxSVC_TYPE_RADIO:	return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_APSLCN_RADIO) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_APSLCN_RADIO;
		default:				break;
		}
		break;

	case eDEFAULTLCN_SKYD:
		hErr = DB_SVC_GetMaxLcnInLcnServices (eOrgDeliType, eSvcType, &nMaxLcn);

		switch (eSvcType)
		{
		case eDxSVC_TYPE_TV:		return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_SKYDLCN_TV) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_SKYDLCN_TV;
		case eDxSVC_TYPE_RADIO:	return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_SKYDLCN_RADIO) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_SKYDLCN_RADIO;
		default:				break;
		}
		break;

	case eDEFAULTLCN_KDG:
		hErr = DB_SVC_GetMaxLcnInLcnServices (eOrgDeliType, eSvcType, &nMaxLcn);

		switch (eSvcType)
		{
		case eDxSVC_TYPE_TV:		return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_KDGLCN_TV) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_KDGLCN_TV;
		case eDxSVC_TYPE_RADIO:	return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_KDGLCN_RADIO) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_KDGLCN_RADIO;
		default:				break;
		}
		break;

	case eDEFAULTLCN_NTVPLUS:
		hErr = DB_SVC_GetMaxLcnInLcnServices (eOrgDeliType, eSvcType, &nMaxLcn);

		switch (eSvcType)
		{
		case eDxSVC_TYPE_TV:		return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_NTVPLUS_TV) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_NTVPLUS_TV;
		case eDxSVC_TYPE_RADIO:	return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_NTVPLUS_RADIO) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_NTVPLUS_RADIO;
		default:				break;
		}
		break;

	case eDEFAULTLCN_UPC:
		hErr = DB_SVC_GetMaxLcnInLcnServices (eOrgDeliType, eSvcType, &nMaxLcn);

		switch (eSvcType)
		{
		case eDxSVC_TYPE_TV:		return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_UPC_TV) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_UPC_TV;
		case eDxSVC_TYPE_RADIO:	return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_UPC_RADIO) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_UPC_RADIO;
		default:				break;
		}
		break;

	case eDEFAULTLCN_ADTV:
		hErr = DB_SVC_GetMaxLcnInLcnServices (eOrgDeliType, eSvcType, &nMaxLcn);
		switch (eSvcType)
		{
		case eDxSVC_TYPE_TV:		return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_ME_TV) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_ME_TV;
		case eDxSVC_TYPE_RADIO:	return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_ME_RADIO) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_ME_RADIO;
		default:				break;
		}
		break;

	case eDEFAULTLCN_NONE_FSAT:
		if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_NONFSAT_TV;
		else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_NONFSAT_RADIO;
		break;

	case eDEFAULTLCN_DIGITURK:
		return DEFAULT_LCN_BASE_RAY_SAT_TV;

	case eDEFAULTLCN_AUSDTT:
		if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_AUSDTT_TV;
		else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_AUSDTT_RADIO;
		break;

	case eDEFAULTLCN_ERTELECOM:
		hErr = DB_SVC_GetMaxLcnInLcnServices (eOrgDeliType, eSvcType, &nMaxLcn);

		switch (eSvcType)
		{
		case eDxSVC_TYPE_TV:		return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_ERTELECOM_TV) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_ERTELECOM_TV;
		case eDxSVC_TYPE_RADIO:	return (hErr == ERR_OK && nMaxLcn >= DEFAULT_LCN_BASE_ERTELECOM_RADIO) ? (HUINT16)nMaxLcn + 1 : DEFAULT_LCN_BASE_ERTELECOM_RADIO;
		default:				break;
		}
		break;

	case eDEFAULTLCN_UKDTT:
		if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_UKDTT_TV;
		else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_UKDTT_RADIO;
		break;

	case eDEFAULTLCN_ZIGGO:
		if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_ZIGGO_TV;
		else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_ZIGGO_RADIO;
		break;

	case eDEFAULTLCN_ITADTT:
		if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_ITADTT_TV;
		else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_ITADTT_RADIO;
		break;

	case eDEFAULTLCN_NORDIG:
		if(eSvcType == eDxSVC_TYPE_TV)				return DEFAULT_LCN_BASE_NORDIG_TV;
		else if(eSvcType == eDxSVC_TYPE_RADIO)		return DEFAULT_LCN_BASE_NORDIG_RADIO;
		break;

	default:
		break;
	}

	return 0;
}

HERROR xsvc_si_LibLcnPostProcessForSvcNumbering (HUINT32 ulActionId, DefaultLcnPolicy_t eLcnPolicy, DxDeliveryType_e eDeliType)
{
	HxLOG_Print("Spec = [%d] \n", eLcnPolicy);

	switch (eLcnPolicy)
	{
	case eDEFAULTLCN_AFTER_LCN:
		// Non LCN Number 들을 LCN Number 뒤로 옮긴다.
		DB_SVC_MakeNonLcnNumber (eDeliType, eDxSVC_TYPE_All);
		break;

	case eDEFAULTLCN_AFTER_LCN_TYPEDEVIDE:
//	case eDEFAULTLCN_APSLCN:
		// Non LCN Number 들을 LCN Number 뒤로 옮긴다.
		DB_SVC_MakeNonLcnNumber (eDeliType, eDxSVC_TYPE_TV);
		DB_SVC_MakeNonLcnNumber (eDeliType, eDxSVC_TYPE_RADIO);
		break;

	case eDEFAULTLCN_SKYD:
	case eDEFAULTLCN_KDG:
		HxLOG_Print("[%s:%d] not support now \n", __FUNCTION__, __LINE__);
		break;

#if defined(CONFIG_OP_NORDIG)
	case eDEFAULTLCN_NORDIG:
		xsvc_si_LcnPostProcessForSvcNumbering_Nordig(eDeliType);
		break;
#endif

#if defined(CONFIG_OP_MBC)
		case eDEFAULTLCN_NORDIG:
			xsvc_si_LcnPostProcessForSvcNumbering_Mbc(eDeliType);
			break;
#endif

	// 구현할 것은 이 위로 구현하도록 한다.
	default:
		break;
	}

	return ERR_OK;
}

/* end of file */

