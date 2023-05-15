/*****************************************************************************************
*	@file		xmgr_swup_debug.c
*
*	http://www.humaxdigital.com
*
*	@date			2015/10/03
*	@author
*	@brief
******************************************************************************************/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <octo_common.h>
#include <mgr_swup.h>
#include <xmgr_swup.h>
#include "local_include/_xmgr_swup.h"


/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___FUNCTION______________________________________________________________________________________
/* *************************************************************************************************** */

const HINT8 * XMGR_SWUP_GetEnumStrOfMessage(HINT32 nMessage)
{
#if defined(CONFIG_DEBUG)
	switch (nMessage)
	{
		ENUM_TO_STR(eMEVT_SWUP);
		ENUM_TO_STR(eMEVT_SWUP_SET_STATE);
		ENUM_TO_STR(eMEVT_SWUP_STATE_POST_MSG);
		ENUM_TO_STR(eMEVT_SWUP_INIT);
		ENUM_TO_STR(eMEVT_SWUP_CONNECT_START);
		ENUM_TO_STR(eMEVT_SWUP_CONNECT_SUCCESS);
		ENUM_TO_STR(eMEVT_SWUP_CONNECT_FAIL);
		ENUM_TO_STR(eMEVT_SWUP_DETECT_START);
		ENUM_TO_STR(eMEVT_SWUP_DETECT_STOP);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DETECT_SUCCESS);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DETECT_FAIL);
		ENUM_TO_STR(eMEVT_SWUP_DOWNLOAD_START);
		ENUM_TO_STR(eMEVT_SWUP_DOWNLOAD_STOP);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DOWNLOAD_SUCCESS);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DOWNLOAD_FAIL);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_DOWNLOAD_PROGRESS);
		ENUM_TO_STR(eMEVT_SWUP_INSTALL_START);
		ENUM_TO_STR(eMEVT_SWUP_INSTALL_STOP);
		ENUM_TO_STR(eMEVT_SWUP_INSTALL_SUCCESS);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_INSTALL_FAIL);
		ENUM_TO_STR(eMEVT_SWUP_NOTIFY_INSTALL_PROGRESS);
		default:
			return ("Unknown");
	}
#else
	return "";
#endif
}

const HINT8 * XMGR_SWUP_GetEnumStrOfState(MgrSwup_State_e State)
{
#ifdef CONFIG_DEBUG
	switch (State)
	{
		ENUM_TO_STR(eSWUP_STATE_Unknown);
		ENUM_TO_STR(eSWUP_STATE_Init);
		ENUM_TO_STR(eSWUP_STATE_Connect);
		ENUM_TO_STR(eSWUP_STATE_Detect);
		ENUM_TO_STR(eSWUP_STATE_Download);
		ENUM_TO_STR(eSWUP_STATE_Install);
		ENUM_TO_STR(eSWUP_STATE_Error);
		ENUM_TO_STR(eSWUP_STATE_Finish);
		default:
			return ("Unknown");
	}
#else
	return "";
#endif
}

const HINT8 * XMGR_SWUP_GetEnumStrOfSource(DxSwUpdate_Source_e eSource)
{
#ifdef CONFIG_DEBUG
	switch (eSource)
	{
		ENUM_TO_STR(eDxSWUPDATE_SOURCE_None);
		ENUM_TO_STR(eDxSWUPDATE_SOURCE_Rf);
		ENUM_TO_STR(eDxSWUPDATE_SOURCE_Ip);
		ENUM_TO_STR(eDxSWUPDATE_SOURCE_File);
		ENUM_TO_STR(eDxSWUPDATE_SOURCE_Usb);
		default:
			return ("Unknown");
	}
#else
		return "";
#endif
}

void XMGR_SWUP_PrintTuningInfo(DxDeliveryType_e eDeliType, DbSvc_TuningInfo_t *pstTuningParam)
{
#ifdef CONFIG_DEBUG
	if(NULL == pstTuningParam)
		return;

	switch(eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
		{
			HUINT8			szFecStr[5];
			switch(pstTuningParam->sat.eFecCodeRate)
			{
				case eDxSAT_CODERATE_1_2:		HxSTD_StrNCpy(szFecStr, "1/2",4); 	break;
				case eDxSAT_CODERATE_2_3:		HxSTD_StrNCpy(szFecStr, "2/3",4); 	break;
				case eDxSAT_CODERATE_3_4:		HxSTD_StrNCpy(szFecStr, "3/4",4); 	break;
				case eDxSAT_CODERATE_3_5:		HxSTD_StrNCpy(szFecStr, "3/5",4); 	break;
				case eDxSAT_CODERATE_4_5:		HxSTD_StrNCpy(szFecStr, "4/5",4); 	break;
				case eDxSAT_CODERATE_5_6:		HxSTD_StrNCpy(szFecStr, "5/6",4); 	break;
				case eDxSAT_CODERATE_5_11:		HxSTD_StrNCpy(szFecStr, "5/11",4);	break;
				case eDxSAT_CODERATE_6_7:		HxSTD_StrNCpy(szFecStr, "6/7",4); 	break;
				case eDxSAT_CODERATE_7_8:		HxSTD_StrNCpy(szFecStr, "7/8",4); 	break;
				case eDxSAT_CODERATE_8_9:		HxSTD_StrNCpy(szFecStr, "8/9",4); 	break;
				case eDxSAT_CODERATE_9_10:		HxSTD_StrNCpy(szFecStr, "9/10",4);	break;
				case eDxSAT_CODERATE_AUTO:		HxSTD_StrNCpy(szFecStr, "Auto",4);	break;
				default:						HxSTD_StrNCpy(szFecStr, "???",4); 	break;
			}
			HLIB_CMD_Printf("SAT: [%5dMHz(%c) SR:%5d %4s]\n"
				, pstTuningParam->sat.ulFrequency
				, ((pstTuningParam->sat.ePolarization == eDxSAT_POLAR_HOR) ? 'H': ((pstTuningParam->sat.ePolarization == eDxSAT_POLAR_VER) ? 'V':'?'))
				, pstTuningParam->sat.ulSymbolRate
				, szFecStr);
		}
		break;

	case eDxDELIVERY_TYPE_TER:
		HLIB_CMD_Printf("TER : [%5dMHz] \n", pstTuningParam->ter.ulFreq);
		break;

	case eDxDELIVERY_TYPE_CAB:
		{
			HUINT8			szCons[10];
			switch(pstTuningParam->cab.ucCon)
			{
				case eDxCAB_CONSTELLATION_AUTO:		HxSTD_snprintf(szCons, 10, "AUTO");		break;
				case eDxCAB_CONSTELLATION_16QAM:	HxSTD_snprintf(szCons, 10, "16QAM");	break;
				case eDxCAB_CONSTELLATION_32QAM:	HxSTD_snprintf(szCons, 10, "32QAM");	break;
				case eDxCAB_CONSTELLATION_64QAM:	HxSTD_snprintf(szCons, 10, "64QAM");	break;
				case eDxCAB_CONSTELLATION_128QAM:	HxSTD_snprintf(szCons, 10, "128QAM");	break;
				case eDxCAB_CONSTELLATION_256QAM:	HxSTD_snprintf(szCons, 10, "256QAM");	break;
				case eDxCAB_CONSTELLATION_1024QAM:	HxSTD_snprintf(szCons, 10, "1024QAM");	break;
				case eDxCAB_CONSTELLATION_4096QAM:	HxSTD_snprintf(szCons, 10, "4096QAM");	break;
				default:							HxSTD_snprintf(szCons, 10, "????");		break;
			}
			HLIB_CMD_Printf("CAB : [%d KHz, %d, %s]\n", pstTuningParam->cab.ulFreq, pstTuningParam->cab.usSR, szCons);
		}
		break;

	default:
		break;
	}
#endif
}


/* end of file */

