/**************************************************************
*	@file	pl_dms.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DMS
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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

#define	________________Header_Files_______________________________
#include "pl_dms.h"
#include "pl_fs.h"
#include "pl_common.h"
#include "pl_ch_list.h"
#include "pl_pvr.h"

#include <clib.h>
#include <dlib.h>
#include <thapi.h>
#include <dapi.h>

#include <mxdlna_dms.h>

#define ________________Defines____________________________________
#define PL_DMS_DEFAULT_CHANNEL_PREFIX			"tuner/HCHANNEL:"

#define SUPPORT_DMS_CHANNEL_LOGO                0

#define PL_DMS_EPG_TIMEOUT					( 10 * 1000 )
#define PL_DMS_CHANNELLOGO_TIMEOUT			( 2 * 1000 )

#define PL_DMS_PREFIX_VIDEOTYPE				"eDxVIDEO_TYPE_"
#define PL_DMS_PREFIX_SATTYPE				"eSatType_"
#define PL_DMS_PREFIX_CASTYPE				"eDxCAS_TYPE_"
#define PL_DMS_PREFIX_DELIVERYTYPE			"eDxDELIVERY_TYPE_"
#define PL_DMS_PREFIX_ANTCONNTYPE			"eDxANTCONN_"

#define ________________Static_Variables___________________________
static 	PL_DMS_MXDLNA_HANDLE_t	pDmsHandles;
static  HxList_t				*s_pChannelList = NULL;
static	HUINT32					s_nChannelCount = 0;
#ifdef HOMMA_USE_APPKIT_DIRECTLY
static	HxList_t				*s_pChannelGroupList = NULL;
static	HUINT32					s_nChannelNotifier = 0;
#endif

#define ________________Internal_APIs_Prototype____________________
PL_DMS_Status_e	PL_DMS_GetStatus(void);
HBOOL	PL_DMS_IsStopping(void);

#define ________________Internal_Functions_________________________
/*******************************************************************************/
/* Channel Additional Info 관련 String 변환 함수군 */
/*******************************************************************************/

/* Service의 Video type : all/sd/hd */
static const HCHAR* __VideoTypeToStr( DxVideoType_e eVideoType )
{
	switch( eVideoType )
	{
		case eDxVIDEO_TYPE_SD: return "Sd";
		case eDxVIDEO_TYPE_HD: return "Hd";
		default:
			break;
	}

	return "UnKnown";
}

/* Service의 Cas Type */
static const HCHAR* __CasTypeToStr( DxCasType_e eCasType )
{
	switch( eCasType )
	{
		case eDxCAS_TYPE_FTA: 			return "Fta";
		case eDxCAS_TYPE_NAGRA: 		return "Nagra";
		case eDxCAS_TYPE_IRDETO: 		return "Irdeto";
		case eDxCAS_TYPE_NDS: 			return "Nds";
		case eDxCAS_TYPE_VIACCESS: 		return "Viaccess";
		case eDxCAS_TYPE_CONAX: 		return "Conax";
		case eDxCAS_TYPE_MEDIAGUARD: 	return "Mediaguard";
		case eDxCAS_TYPE_CRYPTOWORKS: 	return "Cryptoworks";
		case eDxCAS_TYPE_JPBCAS: 		return "Jpbcas";
		case eDxCAS_TYPE_JPCCAS: 		return "Jpccas";
		case eDxCAS_TYPE_ALLCAS: 		return "Allcas";
		case eDxCAS_TYPE_VERIMATRIX:	return "Verimatrix";
		default:
			break;
	}

	return "UnKnown";
}

/* Service의 Delivery Type */
static const HCHAR* __DeliveryTypeToStr( DxDeliveryType_e eDeliveryType )
{
	switch( eDeliveryType )
	{
		case eDxDELIVERY_TYPE_SAT: 			return "SAT";
		case eDxDELIVERY_TYPE_TER:			return "TER";
		case eDxDELIVERY_TYPE_CAB:			return "CAB";
		case eDxDELIVERY_TYPE_MULTICAST:	return "MULTICAST";
		default:
			break;
	}

	return "UnKnown";
}

/* Antenna Connection Type */
static const HCHAR* __AntConnTypeToStr( DxAntennaConnection_e eAntConnType )
{
	switch( eAntConnType )
	{
		case eDxANTCONN_ONECABLE: return "OneCable";
		case eDxANTCONN_TWOSAME: return "TwoSame";
		case eDxANTCONN_TWODIFF: return "TwoDiff";
		case eDxANTCONN_NONSATONECABLE: return "NonSatOneCable";
		case eDxANTCONN_NONSATTWOCABLE: return "NonSatTwoCable";
		default:
			break;
	}

	return "UnKnown";
}

/* Antenna Type */
static const HCHAR* __AntTypeToStr( DxAntennaType_e eAntType )
{
	switch( eAntType )
	{
		case eDxANT_TYPE_LNB_ONLY: return "lnb_only";
		case eDxANT_TYPE_DISEQC: return "diseqc";
		case eDxANT_TYPE_SCD: return "scd";
		case eDxANT_TYPE_SMATV: return "smatv";
		case eDxANT_TYPE_MONOBLOCK_LNB: return "monoblock_lnb";
		default:
			break;
	}

	return "unknown";
}

#ifdef HOMMA_USE_APPKIT_DIRECTLY
/* LCN은 CList의 것을 이용해야, HMS와 일치함 */
static HINT32 __channel_no( HINT32 nSvcUid )
{
	HINT32					nLogicalNumber = -1;
	HxList_t				*pTmpList = NULL;
	CListItem_t				*pCListItem = NULL;

	pTmpList = s_pChannelGroupList;
	if(pTmpList == NULL)
		HxLOG_Debug("[%s:%d] Error> channel group is not existed! \n", __func__, __LINE__);

	while( pTmpList != NULL )
	{
		pCListItem = ( CListItem_t * )HLIB_LIST_Data( pTmpList );

		if(pCListItem != NULL)
		{
			if( pCListItem->uid == (HUID)nSvcUid )
			{
				nLogicalNumber = pCListItem->nLogicalNo;
				break;
			}

			pTmpList = pTmpList->next;
		}
		else
			break;
	}

	return nLogicalNumber;
}
#endif

static void __channel_info_printf(MXDLNA_DMS_EPG_CHANNEL *pChannel)
{
	if(pChannel != NULL)
	{
		HxLOG_Debug("[PL_DMS:EPG:%d] logoURL - %s \n", __LINE__, pChannel->logoURL);
		HxLOG_Debug("[PL_DMS:EPG:%d] uniqValue - %d  \n", __LINE__, pChannel->uniqValue);
		HxLOG_Debug("[PL_DMS:EPG:%d] channelID - %s \n", __LINE__, pChannel->channelID);
		HxLOG_Debug("[PL_DMS:EPG:%d] channelID_type - %s \n", __LINE__, pChannel->channelID_type);
		HxLOG_Debug("[PL_DMS:EPG:%d] channelID_distriNetworkID - %d \n", __LINE__, pChannel->channelID_distriNetworkID);
		HxLOG_Debug("[PL_DMS:EPG:%d] channelID_distriNetworkName - %s \n", __LINE__, pChannel->channelID_distriNetworkName);
		HxLOG_Debug("[PL_DMS:EPG:%d] channelNr - %d \n", __LINE__, pChannel->channelNr);
		HxLOG_Debug("[PL_DMS:EPG:%d] channelName - %s \n", __LINE__, pChannel->channelName);
		HxLOG_Debug("[PL_DMS:EPG:%d] channel_title - %s \n", __LINE__, pChannel->channel_title);
		HxLOG_Debug("[PL_DMS:EPG:%d] channel_localUrl - %s \n", __LINE__, pChannel->channel_localUrl);
		HxLOG_Debug("[PL_DMS:EPG:%d] resource_protocol - %s \n", __LINE__, pChannel->resource_protocol);
		HxLOG_Debug("[PL_DMS:EPG:%d] resource_bitrate(%d) resource_duration(%d) resource_resolutionX(%d) resource_resolutionY(%d) \n", __LINE__,
				pChannel->resource_bitrate, pChannel->resource_duration, pChannel->resource_resolutionX, pChannel->resource_resolutionY);
		HxLOG_Debug("[PL_DMS:EPG:%d] nChannel_Type - %d \n", __LINE__, pChannel->nChannel_Type);
		HxLOG_Debug("[PL_DMS:EPG:%d] AddtionalInfo - %s \n", __LINE__, pChannel->pChannel_additional_info);
		HxLOG_Debug("[PL_DMS:EPG:%d] nExConfig - %d \n", __LINE__, pChannel->nExConfig);
	}
}

static HCHAR* __get_enumStr_DxCab_Constellation_e(DxCab_Constellation_e eCons)
{
	switch(eCons)
	{
	case eDxCAB_CONSTELLATION_AUTO:		return "auto";
	case eDxCAB_CONSTELLATION_16QAM:	return "16qam";
	case eDxCAB_CONSTELLATION_32QAM:	return "32qam";
	case eDxCAB_CONSTELLATION_64QAM:	return "64qam";
	case eDxCAB_CONSTELLATION_128QAM:	return "128qam";
	case eDxCAB_CONSTELLATION_256QAM:	return "256qam";
	case eDxCAB_CONSTELLATION_1024QAM:	return "1024qam";
	case eDxCAB_CONSTELLATION_4096QAM:	return "4096qam";
	default:
		return "?";
	}
}

/* SvcInfo를 기반으로 pChannel의 정보를 채워주는 함수 */
static HERROR __make_channel_info( MXDLNA_DMS_EPG_CHANNEL *pChannel, CO_CHANNEL_t *pSrc)
{
	HCHAR						szAdditionalInfo[ 512 ] = { 0, };
	HCHAR						szTuningInfo[ 512 ] = { 0, };

	DxAntennaConnection_e 		eAntCon = eDxANTCONN_ONECABLE;
	DxService_t 				*pSvcInfo = pSrc->svc;
	DxTransponder_t				*pTsInfo = pSrc->ts;
	DxNetwork_t					*pNetInfo = NULL;

	if( pChannel == NULL || pSvcInfo == NULL)
	{
		HxLOG_Error("Invalid args(pChannel:%p pSvcInfo:%p)\n", pChannel, pSvcInfo);
		return ERR_FAIL;
	}

	if( pSvcInfo->lcn == 0)
	{
		HxLOG_Error("Invalid values(pSvcInfo->lcn == 0)\n");
		return ERR_FAIL;
	}

#if 0
	if( pSvcInfo->casType != eDxCAS_TYPE_FTA)
	{
		HxLOG_Debug("[%s:%d] %s is not fta channel !! \n", __func__, __LINE__, pSvcInfo->name);
		return ERR_FAIL;
	}
#endif

	/* initialize (pChannel) */
	HxSTD_MemSet( pChannel, 0x00, sizeof( MXDLNA_DMS_EPG_CHANNEL ) );
	{
		// Raw EPG data related
		pChannel->logoURL = NULL;

		// Channel identification related
		pChannel->uniqValue = 0x0;

		// DLNA Mandatory property related
		HxSTD_MemSet(pChannel->channelID, 0x00, sizeof(MXDLNA_DMS_EPG_CHANNELID_LEN));
		HxSTD_MemSet(pChannel->channelID_type, 0x00, sizeof(MXDLNA_DMS_EPG_CHANNELID_TYPE_LEN));
		HxSTD_MemSet(pChannel->channelID_distriNetworkName, 0x00, sizeof(MXDLNA_DMS_EPG_CHANNELID_DISTRINETWORKNAME_LEN));
		pChannel->channelID_distriNetworkID = 0x0;

		// UPnP property related
		pChannel->channelNr = 0x0;
		HxSTD_MemSet(pChannel->channelName, 0x00, sizeof(MXDLNA_DMS_EPG_CHANNELNAME_LEN));

		// MXDLNA_DMS_EPG_USERSETPROP related
		HxSTD_MemSet(pChannel->channel_title, 0x00, sizeof(MXDLNA_DMS_EPG_TITLE_LEN));
		HxSTD_MemSet(pChannel->channel_localUrl, 0x00, sizeof(MXDLNA_DMS_EPG_LOCALURL_LEN));
		HxSTD_MemSet(pChannel->resource_protocol, 0x00, sizeof(MXDLNA_DMS_EPG_RESOURCEPROTOCOL_LEN));

		pChannel->resource_bitrate = 0x0;
		pChannel->resource_duration = 0x0;
		pChannel->resource_resolutionX = 0x0;
		pChannel->resource_resolutionY = 0x0;

		//#if	defined(HUMAX_X_CHANNEL_ADDITIONAL_INFO)
		pChannel->pChannel_additional_info = NULL;
		//#endif
	}

#if SUPPORT_DMS_CHANNEL_LOGO
	/* logo */
	{
		DxChannelLogo_t				*pChannelLogo = NULL;

		HOMMA_INTEROP_META_SVC_GetLogoUrl(pSvcInfo->uid, pChannelLogo);

		if(pChannelLogo)
		{
			if(HxSTD_StrEmpty(pChannelLogo->ucServerLogoUrl))
			{
				pChannel->logoURL = HLIB_STD_StrDup( pChannelLogo->ucServerLogoUrl );
			}

			HLIB_MEM_Free(pChannelLogo);
		}
	}
#endif

	/* service info */
	pChannel->uniqValue = pSvcInfo->uid;

	HxSTD_snprintf( pChannel->channelID, sizeof( pChannel->channelID ), "%d,%d,%d", pSvcInfo->onid, pSvcInfo->tsid, pSvcInfo->svcid);
	HxSTD_snprintf( pChannel->channelID_type, sizeof( pChannel->channelID_type ), "SI" );

#ifdef HOMMA_USE_APPKIT_DIRECTLY
	pChannel->channelNr = __channel_no( pSvcInfo->uid );
#else
	pChannel->channelNr = HOMMA_INTEROP_META_SVC_GetChannelNumber( pSvcInfo->uid );
#endif
	if( pChannel->channelNr < 0 )
		pChannel->channelNr = pSvcInfo->lcn;

	if( pSvcInfo->svcType== eDxSVC_TYPE_TV )
		pChannel->nChannel_Type = MXDLNA_DMS_EPG_CHANNEL_TV;
	else if( pSvcInfo->svcType== eDxSVC_TYPE_RADIO)
		pChannel->nChannel_Type = MXDLNA_DMS_EPG_CHANNEL_RADIO;
	else
		pChannel->nChannel_Type = MXDLNA_DMS_EPG_CHANNEL_TV;

	HxSTD_snprintf( pChannel->channelName, sizeof( pChannel->channelName ), "%s", pSvcInfo->name);

	/* transponder info */
	if(pTsInfo != NULL)
	{
		if (pTsInfo->deliType == eDxDELIVERY_TYPE_SAT)
		{
			HxSTD_snprintf( szTuningInfo, sizeof(szTuningInfo), "freq=%d,pol=%c",
					pTsInfo->tuningParam.sat.tuningInfo.ulFrequency,
					( pTsInfo->tuningParam.sat.tuningInfo.ePolarization == eDxSAT_POLAR_HOR ) ? 'H' : 'V');
		}
		else if (pTsInfo->deliType == eDxDELIVERY_TYPE_TER)
		{
			HxSTD_snprintf( szTuningInfo, sizeof(szTuningInfo), "freq=%d", pTsInfo->tuningParam.ter.ulFrequency);
		}
		else if (pTsInfo->deliType == eDxDELIVERY_TYPE_CAB)
		{
			HxSTD_snprintf( szTuningInfo, sizeof(szTuningInfo), "freq=%d,sr=%d,cons=%s"
				, pTsInfo->tuningParam.cab.ulFrequency
				, pTsInfo->tuningParam.cab.ulSymbolRate
				, __get_enumStr_DxCab_Constellation_e(pTsInfo->tuningParam.cab.constellation)
				);
		}
		else
		{
			HxLOG_Warning("invalid delivery type (%d)\n", pTsInfo->deliType);
		}
		/* (stTs->deliType == eDxDELIVERY_TYPE_MULTICAST) */
	}

	/* network info */
	if(pTsInfo != NULL)
	{
		pNetInfo = HOMMA_INTEROP_META_SVC_GetNetwork(pTsInfo->netuid);
	}

	if(pNetInfo)
	{
		pChannel->channelID_distriNetworkID = pNetInfo->uid;
		/* 2014/2/18 mhkang:
			distriNetworkName string은 DxNetwork_t.linkdata DB필드 값을 참조하여 만드는데 linkdata필드에 XML stringd으로 사용할 수 없는 임의의 ASCII 문자가 들어가는 경우 있다(예:0x14, 0x01, 0x3c...).
			이때 DeviceSpy 또는 HUMAX LiveTV앱에서 tuner invoke browse하는 경우 응답 string이 XML validation이 깨진채로 전달되어 에러가 발생한다.
			근본적으로 DxNetwork_t DB Table에 insert하는 process에서 수정해야한다.
			그 전까지 Homma쪽에서 방어 임시코드(distriNetworkName을 "NULL" string으로 사용)로 대응한다.
		HxSTD_snprintf( pChannel->channelID_distriNetworkName, sizeof( pChannel->channelID_distriNetworkName ), "%s", stNetInfo->name);
		*/
		HxSTD_snprintf( pChannel->channelID_distriNetworkName, sizeof( pChannel->channelID_distriNetworkName ), "%s", "NULL");

		HOMMA_INTEROP_META_SVC_Delete(pNetInfo);
	}
	else
	{
		pChannel->channelID_distriNetworkID = -1;
		HxSTD_snprintf( pChannel->channelID_distriNetworkName, sizeof( pChannel->channelID_distriNetworkName ), "%s", "NULL");
	}

	/* antenna info */
	HOMMA_INTEROP_SCENARIO_GetAntennaConnection(&eAntCon);

	/* Humax Channel List를 구성하기 위한 추가 정보 기술 : x_channel_additional_info */
	if (pTsInfo && pTsInfo->deliType == eDxDELIVERY_TYPE_SAT)
	{
		if (HOMMA_INTEROP_SCENARIO_GetAntennaConnection(&eAntCon) != ERR_OK)
		{
			HxLOG_Error("[ DMS:%s] Error> Cant get antenna connection(svcid:%d)!\n", __func__, pSvcInfo->svcid);
		}

		/* x_channel_additional_info for SAT */
		HxSTD_snprintf( szAdditionalInfo, sizeof( szAdditionalInfo ),
			"video_type=%s,satellite_type=%s,cas_type=%s,delivery_type=%s,antenna_type=%s,favorite=%d,ts_id=%d,svc_id=%d,on_id=%d,%s,conn=%s,userlock=%d",
					__VideoTypeToStr(pSvcInfo->highDefinition),
					pSvcInfo->satType,
					__CasTypeToStr( pSvcInfo->casType),
					__DeliveryTypeToStr( pSvcInfo->deliType),
					(pTsInfo != NULL)?__AntTypeToStr(pTsInfo->tuningParam.sat.antennaType):"unknown",
					0, /* Favorite 정보는 webapp 에서 관리하여 알수 없음 */
					pSvcInfo->tsid,
					pSvcInfo->svcid,
					pSvcInfo->onid,
					(pTsInfo != NULL)?szTuningInfo:"freq=0,pol=0",
					__AntConnTypeToStr(eAntCon),
					pSvcInfo->locked);
	}
	else if (pTsInfo && pTsInfo->deliType == eDxDELIVERY_TYPE_TER)
	{
		/* x_channel_additional_info for TER, ... */
		HxSTD_snprintf( szAdditionalInfo, sizeof( szAdditionalInfo ),
			"video_type=%s,satellite_type=%s,cas_type=%s,delivery_type=%s,favorite=%d,ts_id=%d,svc_id=%d,on_id=%d,%s,userlock=%d",
					__VideoTypeToStr(pSvcInfo->highDefinition),
					pSvcInfo->satType,
					__CasTypeToStr( pSvcInfo->casType),
					__DeliveryTypeToStr( pSvcInfo->deliType),
					0, /* Favorite 정보는 webapp 에서 관리하여 알수 없음 */
					pSvcInfo->tsid,
					pSvcInfo->svcid,
					pSvcInfo->onid,
					(pTsInfo != NULL)?szTuningInfo:"freq=0",
					pSvcInfo->locked);
	}
	/*  2014/10/17 mhkang: YSR2000
	YSR2000은 케이블용 STB인데 케이블관련 정보를 생성해서 LiveTV앱에게 전달해야함.
	일단, TER 쪽 부분을 Copy&Paste하였고 CAB에 맞게 수정이 필요할 것 같음.
	*/
	else if (pTsInfo && pTsInfo->deliType == eDxDELIVERY_TYPE_CAB)
	{
		/* x_channel_additional_info for TER, ... */
		HxSTD_snprintf( szAdditionalInfo, sizeof( szAdditionalInfo ),
					"video_type=%s,satellite_type=%s,cas_type=%s,delivery_type=%s,favorite=%d,ts_id=%d,svc_id=%d,on_id=%d,%s,userlock=%d",
					__VideoTypeToStr(pSvcInfo->highDefinition),
					pSvcInfo->satType,
					__CasTypeToStr( pSvcInfo->casType),
					__DeliveryTypeToStr( pSvcInfo->deliType),
					0, /* Favorite 정보는 webapp 에서 관리하여 알수 없음 */
					pSvcInfo->tsid,
					pSvcInfo->svcid,
					pSvcInfo->onid,
					(pTsInfo != NULL)?szTuningInfo:"freq=0",
					pSvcInfo->locked);
	}

	/* (stTs->deliType == eDxDELIVERY_TYPE_CAB)*/
	/* (stTs->deliType == eDxDELIVERY_TYPE_MULTICAST) */

	pChannel->pChannel_additional_info = HLIB_STD_StrDup( szAdditionalInfo );

	/* Resource related property */
	if( pSvcInfo->svcType == eDxSVC_TYPE_TV )
		HxSTD_snprintf( pChannel->channel_title, sizeof( pChannel->channel_title ), "%04d %s", pChannel->channelNr, pSvcInfo->name);
	else if( pSvcInfo->svcType == eDxSVC_TYPE_RADIO )
		HxSTD_snprintf( pChannel->channel_title, sizeof( pChannel->channel_title ), "%04d %s( RADIO )", pChannel->channelNr, pSvcInfo->name);

	HxSTD_snprintf( pChannel->channel_localUrl, sizeof( pChannel->channel_localUrl ), "%s%d.ts", PL_DMS_DEFAULT_CHANNEL_PREFIX, pChannel->uniqValue );

	if (pSvcInfo->svcType == eDxSVC_TYPE_RADIO)
		HxSTD_snprintf( pChannel->resource_protocol, sizeof( pChannel->resource_protocol ), "http-get:*:audio/mpeg:DLNA.ORG_CI=0" );
	else
		HxSTD_snprintf( pChannel->resource_protocol, sizeof( pChannel->resource_protocol ), "http-get:*:video/mpeg:DLNA.ORG_CI=0" );

	/* 아래 값은 각 Channel 별로 고정된 값이 아니어서 채워 줄 수 없음. */
	pChannel->resource_bitrate = -1;
	pChannel->resource_duration = -1;
	pChannel->resource_resolutionX = -1;
	pChannel->resource_resolutionY = -1;

	//__channel_info_printf(pChannel);
	//HxLOG_Print("[%s:%d] szAdditionalInfo - %s \n", __func__, __LINE__, szAdditionalInfo);

	return ERR_OK;
}

void __free_channel_info(void *data)
{
	MXDLNA_DMS_EPG_CHANNEL *pChannel = (MXDLNA_DMS_EPG_CHANNEL *)data;

	if(pChannel == NULL)
	{
		return;
	}

	if (pChannel->logoURL != NULL)
	{
		HLIB_STD_MemFree(pChannel->logoURL);
		pChannel->logoURL = NULL;
	}

	if(pChannel->pChannel_additional_info != NULL)
	{
		HLIB_STD_MemFree(pChannel->pChannel_additional_info);
		pChannel->pChannel_additional_info = NULL;
	}

	HxSTD_MemSet( pChannel, 0x00, sizeof( MXDLNA_DMS_EPG_CHANNEL ) );
	HLIB_STD_MemFree(pChannel);
	pChannel= NULL;
	return;
}


/* EPG Callback - 전체 channel 수를 return (only for Browse action) */
static HLONG __cb_epg_channel_count( void )
{
	HUINT32 nCount = 0;

	nCount = HLIB_LIST_Length(HLIB_LIST_First(s_pChannelList));
	if (nCount != s_nChannelCount)
	{
		HxLOG_Warning( "Unmatched channel count(new:%d, old:%d)\n", nCount, s_nChannelCount);
		s_nChannelCount = nCount;
	}

	return s_nChannelCount;
}

/* EPG Callback - uniqValue를 가지는 channel의 정보를 return (only for Browse action) */
static MXDLNA_DMS_EPG_CHANNEL* __cb_epg_channel_info( HINT32 nUniqValue )
{
	MXDLNA_DMS_EPG_CHANNEL		*pChannel = NULL, *pRet = NULL;
	HxList_t *tmp = NULL;
	HUINT32 uLoop = 0;
	HUINT32 uMatched = 0;

	HxLOG_Print( "[ DMS:%s] ( %d )\n", __func__, nUniqValue );

	tmp = HLIB_LIST_First(s_pChannelList);
	while(tmp)
	{
		pChannel = HLIB_LIST_Data(tmp);

		if(pChannel->uniqValue == nUniqValue)
		{
			pRet = HLIB_STD_MemDup(pChannel, sizeof(MXDLNA_DMS_EPG_CHANNEL));

			if(pChannel->logoURL != NULL)
				pRet->logoURL = HLIB_STD_StrDup(pChannel->logoURL);
			if(pChannel->pChannel_additional_info != NULL )
				pRet->pChannel_additional_info = HLIB_STD_StrDup(pChannel->pChannel_additional_info);

			__channel_info_printf(pRet);
			uMatched++;
			break;
		}

		uLoop++;
		tmp = tmp->next;
	}

	if ((pRet == NULL) || (uLoop == 0)  || (uMatched == 0))
	{
		HxLOG_Error( "[ DMS] Error> No matched channels(svc:%d, cnt:%d, loop:%d, match:%d)\n",
					nUniqValue, HLIB_LIST_Length(HLIB_LIST_First(s_pChannelList)), uLoop, uMatched);
	}

	return pRet;
}

/*  접속 즉시 채널 리스트 요청  */
/* EPG Callback - Channel search 결과를 return (for Browse/Search action) */
static MXDLNA_DMS_EPG_CHANNEL*	__cb_epg_channel_search( HCHAR *search_criteria, HCHAR *sort_criteria ,HINT32 startIdx, HINT32 requestedCount )
{
	HUINT32 				nCount = 0;
	HINT32					i = 0;
	HINT32					nAddChCount = 0;

	MXDLNA_DMS_EPG_CHANNEL *pChannel = NULL, *pChannelList = NULL;
	MXDLNA_DMS_EPG_CHANNEL *pChTemp = NULL, *_pChTemp = NULL;

	/* search_criteria가 Null일 경우, Browse() 요청임. */
	HxLOG_Print( "[ PL_DMS ] __cb_epg_channel_search() +++++\n" );
	HxLOG_Print( "[ PL_DMS ] search_criteria = %s\n", search_criteria );
	HxLOG_Print( "[ PL_DMS ] sort_criteria = %s\n", sort_criteria );
	HxLOG_Print( "[ PL_DMS ] startIdx = %d\n", startIdx );
	HxLOG_Print( "[ PL_DMS ] requestedCount = %d\n", requestedCount );

#if 0	// 2016/7/15 mhkang: in case of requesting search action, search_criteria is not null. so following codes must be commented out.
	if( search_criteria != NULL )
	{
		HxLOG_Print( "[ DMS:%s] search_criteria( %s ), Not Supported\n", __FUNCTION__, search_criteria );
		return NULL;
	}
#endif
	nCount = s_nChannelCount; /* PL_COMMON_GetChannelCount()..?? */
	HxLOG_Print( "[ PL_DMS ] Find SVC Count ( %d )\n", nCount);

	/* startIdx가 검색된 갯수보다 많으면 안됨 */
	if( startIdx > nCount )
	{
		HxLOG_Error( "[ PL_DMS ] Error> exceed count(start:%d, cnt:%d)\n", startIdx, nCount);
		return NULL;
	}

	for( i = startIdx ; i < nCount ; ++i )
	{
		HxList_t *tmp = HLIB_LIST_GetListItem(HLIB_LIST_First(s_pChannelList), i);

		if(HLIB_LIST_Data(tmp) == NULL)
			break;

		_pChTemp = HLIB_LIST_Data(tmp);

		pChannel = HLIB_STD_MemDup(_pChTemp, sizeof( MXDLNA_DMS_EPG_CHANNEL ));

		if(_pChTemp->logoURL != NULL)
			pChannel->logoURL = HLIB_STD_StrDup(_pChTemp->logoURL);

		if(_pChTemp->pChannel_additional_info != NULL )
			pChannel->pChannel_additional_info = HLIB_STD_StrDup(_pChTemp->pChannel_additional_info);

		_pChTemp = NULL;

		if( nAddChCount == 0 )
		{
			pChannelList = pChannel;
			pChTemp = pChannelList;
		}
		else
		{
			pChTemp->pNextChannel = pChannel;
			pChTemp = pChTemp->pNextChannel;
		}

		//HxLOG_Print( "[ PL_DMS ] %d >  Add Channel( %s )\n", nAddChCount, pChannel->channelName );
		//__channel_info_printf(pChannel);

		nAddChCount++;

		if( nAddChCount >= requestedCount )
			break;
	}

	HxLOG_Print( "[ PL_DMS ] Channel Add Count( %d )\n", nAddChCount );
	HxLOG_Print( "[ PL_DMS ] __cb_epg_channel_search() ------\n" );

	return pChannelList;
}

/* EPG Callback - Event search 결과를 return (for Browse/Search action) */
/* 최신 버전에서는 이 부분 사용되지 않음. - Portal 에서 EPG 정보 받음.  */
static MXDLNA_DMS_EPG_EVENT* __cb_epg_channel_event( HCHAR *search_criteria, HCHAR *sort_criteria ,HINT32 startIdx, HINT32 requestedCount, HINT32 *pnTotal )
{
	MXDLNA_DMS_EPG_EVENT *pDlnaEventList = NULL;

	/* search_criteria가 Null일 경우, Browse() 요청임. */
	HOM_FUNC_ENTER;
	HxLOG_Print( "[ PL_DMS ] search_criteria = %s\n", search_criteria );
	HxLOG_Print( "[ PL_DMS ] sort_criteria = %s\n", sort_criteria );
	HxLOG_Print( "[ PL_DMS ] startIdx = %d\n", startIdx );
	HxLOG_Print( "[ PL_DMS ] requestedCount = %d\n", requestedCount );

	/***************************************************/
	/* search_criteria 의 형태 예시 */
	/* (upnp:channelNr >= "1" and upnp:channelNr <= "5") and */
	/* (upnp:scheduledStartTime > "2012-03-28T08:00:00" and */
	/* upnp:scheduledEndTime < "2012-03-28T10:00:00") */
	/***************************************************/
	/* Parser : SearchCriteria_ToSQLExpression_epg() 참조 */
	/***************************************************/

	HOM_FUNC_LEAVE;

	return pDlnaEventList;
}

#ifdef HOMMA_USE_APPKIT_DIRECTLY
/* Channel Group List를 받은 후, ALL Category를 DLNA에서 사용하기 위해 저장 */
static void __cb_grouplist( const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata )
{
	DAxIterator_t		 iter;
	HINT32 i = 0;
	HOM_FUNC_ENTER;
	HxLOG_Print( "[ DMS:%s ] name - %s, uid - %lld, size - %d +++++++++++\n", __FUNCTION__, name, uid, size );

	if (PL_DMS_IsStopping() == TRUE)
	{
		/* Break if DMS is stopping */
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> DMS is stopping(%d)\n", __FUNCTION__, __LINE__, PL_DMS_GetStatus());
		return;
	}

	DAPI_InitIterator( &iter, data, size, 0 );

	/* 기존 리스트 삭제 */
	if (s_pChannelGroupList != NULL)
	{
		HLIB_LIST_RemoveAllFunc( s_pChannelGroupList, HLIB_STD_MemFree_CB);
		s_pChannelGroupList = NULL;
	}

	while( DAPI_NextIterator( &iter ) )
	{
		DxGroupList_t* pGroupInfo = ( DxGroupList_t* )iter.data;
		HxVector_t* pGroupList;

		if (PL_DMS_IsStopping() == TRUE)
		{
			/* Break if DMS is stopping */
			HxLOG_Error( "[ PL_DMS ] %s(%d) Error> DMS is stopping( %d )\n", __FUNCTION__, __LINE__, PL_DMS_GetStatus());
			break;
		}

		pGroupList = CLIB_DeSerialize( ( CList_t* )pGroupInfo->pucData );
		if( pGroupList )
		{
			HxLOG_Print( "\n\n[ DMS:%s ] GROUP # %s #( %d )\n", __func__, pGroupInfo->szGroupName, pGroupInfo->nSize );
			if( HxSTD_StrCmp( pGroupInfo->szGroupName, "ALL") == 0 )
			{
				for( i = 0 ; i < HLIB_VECTOR_Length( pGroupList ); i++ )
				{
					CListItem_t* pListItem;
					if (PL_DMS_IsStopping() == TRUE)
					{
						/* Break if DMS is stopping */
						HxLOG_Error( "[ PL_DMS ] %s(%d) Error> DMS is stopping( %d )\n", __FUNCTION__, __LINE__, PL_DMS_GetStatus());
						break;
					}

					pListItem = ( CListItem_t* )HLIB_VECTOR_ItemAt( pGroupList, i );
					if( pListItem )
					{
						s_pChannelGroupList = HLIB_LIST_Append(s_pChannelGroupList, HLIB_STD_MemDup( pListItem, sizeof( CListItem_t ) ) );
						//HxLOG_Print( "[ DMS ] uid( %lld ), bLCNControl( %d ), LCN( %d )\n", pListItem->uid, pListItem->bLCNControl, pListItem->nLogicalNo );
					}
				}
			}

			HLIB_VECTOR_Delete( pGroupList );
		}
	}

	HOM_FUNC_LEAVE;
}

/* Channel Group List가 Update/Edit 되면 발생하는 Event CB */
static void __cb_grouplist_event( const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata )
{
	HUINT32	reqId;
	HxLOG_Print( "[ DMS:%s ] Channel Event Notifier( name - %s, uid - %lld, size - %d )\n",__func__, name, uid, size );

	if (PL_DMS_IsStopping() == TRUE)
	{
		/* Break if DMS is stopping */
		HxLOG_Error( "[ PL_DMS ] %s(%d) Error> DMS is stopping(%d)\n", __FUNCTION__, __LINE__, PL_DMS_GetStatus());
		return;
	}

	/* 리스트를 새로 받음 */
	reqId = DAPI_GetAll( DxNAMEOF( DxGroupList_t ), __cb_grouplist, NULL );
	(void)reqId;
	/* DAPI_Wait(,reqId) */
}

/* Channel Group List 요청 */
static void __register_grouplist_notifier( void )
{
	HUINT32	reqId;

	/* Channel Group List Edit시 호출될 Notifier 등록; APK_META_SVC_XXX() 대체 */
	s_nChannelNotifier = DAPI_AddTableNotifier( DxNAMEOF( DxGroupList_t ), __cb_grouplist_event, NULL );
	reqId = DAPI_GetAll( DxNAMEOF( DxGroupList_t ), __cb_grouplist, NULL );
	(void)reqId;
	/* DAPI_Wait(,reqId) */
}

static void __unregister_grouplist_notifier( void )
{
	if (s_nChannelNotifier)
	{
		DAPI_RemoveNotifier(s_nChannelNotifier);
		s_nChannelNotifier = 0;
	}
}
#endif

static void __make_channellist(void)
{
	HxList_t *pChannelList = NULL, *lastItem = NULL;
	MXDLNA_DMS_EPG_CHANNEL *pChannel = NULL;
	HERROR hErr;
	HUINT32		 ulLoopCnt = 0;

	HxLOG_Info("(+)\n");

	/* Added error_check because dms_start() blocking after factory-default */
	hErr = PL_COMMON_GetChannelList(&pChannelList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("PL_COMMON_GetChannelList failed (hErr=%d)\n", hErr);

#ifdef HOMMA_USE_APPKIT_DIRECTLY
		if (s_pChannelGroupList != NULL)
		{
			HLIB_LIST_RemoveAllFunc( s_pChannelGroupList, HLIB_STD_MemFree_CB);
			s_pChannelGroupList = NULL;
		}
#endif
		s_nChannelCount = 0;
		return;
	}

	// 2014/3/17 mhkang:기존 채널리스트는 삭제한다.
	if (s_pChannelList != NULL)
	{
		HLIB_LIST_RemoveAllFunc( s_pChannelList, __free_channel_info);
		s_pChannelList = NULL;
	}
	s_nChannelCount = 0;

	pChannelList = HLIB_LIST_First(pChannelList);
	HxLOG_Debug("[%s:%d] ChannelList count:%d\n", __func__, __LINE__, HLIB_LIST_Length(pChannelList));

	while(pChannelList) /* Sometimes, this takes very long-time */
	{
		CO_CHANNEL_t *data = HLIB_LIST_Data(pChannelList);

		if(data != NULL)
		{
			pChannel = HLIB_STD_MemCalloc(sizeof( MXDLNA_DMS_EPG_CHANNEL ));
			if (pChannel != NULL && data->svc->removed == FALSE) // 2014/3.18 mhkang: refer to #commnet:5f8c89f4-ae39-11e3-adf3-8f370a9a2572
			{
				hErr = __make_channel_info( pChannel, data);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("__make_channel_info(CH:name(%s) uid(%d) svcid(%d)) failed(hErr=%d) \n",data->svc->name, data->svc->uid, data->svc->svcid, hErr);
					HLIB_STD_MemFree(pChannel);
				}
				else
				{
					if(lastItem == NULL)
					{
						s_pChannelList = HLIB_LIST_Append(s_pChannelList, pChannel);
						lastItem = HLIB_LIST_Last(s_pChannelList);
					}
					else
					{
						(void)HLIB_LIST_Append(lastItem, pChannel);
						lastItem = HLIB_LIST_Last(lastItem);
					}

					pChannel = NULL;

					if ((ulLoopCnt & 0xff) == 0xff)
					{
						HLIB_STD_TaskSleep(250);
					}

					ulLoopCnt++;
				}
			}
			else
			{
				if(pChannel != NULL)
				{
					HLIB_STD_MemFree(pChannel);
				}

				HxLOG_Warning("ignore deleted channel(name:%s uid:%d)\n", data->svc->name, data->svc->uid);
			}
		}
		pChannelList = pChannelList->next;
	}

	s_nChannelCount = HLIB_LIST_Length(HLIB_LIST_First(s_pChannelList));

	PL_COMMON_ReleaseChannelList(pChannelList); /* not (struct HxList_t **) */

	HxLOG_Info("s_nChannelCount=%d (-)\n", s_nChannelCount);
}

/* 2014/3/18 mhkang: #comment:5f8c89f4-ae39-11e3-adf3-8f370a9a2572
  HMS1000S PH2에서는 채널이 삭제되면 DB상에서 채널 레코드가 삭제되지만,
  HMS1000T 에서는 채널 레코드의 removed필드 값만 1로 셋팅된다(레코드가 삭제되는 것이 아님).
  따라서 removed=1인 채널은 삭제된 채널로 처리한다.
*/

#define ________________Public_Interfaces__________________________

void PL_DMS_ClearTunerEpg( void )
{
	if (0)
	{
		/* TODO: handle보다 tuner/epg status 확인 필요 */
		if (PL_DMS_Get_MxDlna_Res(&pDmsHandles) != ERR_OK)
		{
			HxLOG_Error("Error> Cant get resHandle!\n");
			return;
		}
	}

	/* TODO: event_wait or waiting looping with checking tuner/epg running status */
	/* waiting... */

#ifdef HOMMA_USE_APPKIT_DIRECTLY
	/* unregister dapi event callback */
	__unregister_grouplist_notifier();
#endif

	/* Clear channel list */
	if (s_pChannelList != NULL)
	{
		HLIB_LIST_RemoveAllFunc( s_pChannelList, __free_channel_info);
		s_pChannelList = NULL;
	}
	s_nChannelCount = 0;

#ifdef HOMMA_USE_APPKIT_DIRECTLY
	/* Clear channel group */
	if (s_pChannelGroupList != NULL)
	{
		HLIB_LIST_RemoveAllFunc( s_pChannelGroupList, HLIB_STD_MemFree_CB);
		s_pChannelGroupList = NULL;
	}
#endif
	return;
}

/* TUNER/EPG Container를 만들고, 관련 Callback을 채워주는 함수. */
void PL_DMS_SetupTunerEpg( PL_DMS_EPG_TARGET_e eTarget )
{
	MXDLNA_DMS_EPG_CALLBACK		stEpgCallback = {0};
	HINT32						nRet = 0;

	if (PL_DMS_Get_MxDlna_Res(&pDmsHandles) != ERR_OK)
	{
		HxLOG_Error("Error> Cant get resHandle!\n");
		return;
	}

	stEpgCallback.fnGetChannelCount = __cb_epg_channel_count;
	stEpgCallback.fnGetChannel		= __cb_epg_channel_info;
	stEpgCallback.fnSearchChannel 	= __cb_epg_channel_search;
	stEpgCallback.fnSearchEvent 	= __cb_epg_channel_event;

	mxDlnaDmsEpg_setCallback( &stEpgCallback );

	nRet = mxDlnaDmsEpg_allClear( pDmsHandles.hDms );
	nRet = mxDlnaDmsEpg_createContainer( pDmsHandles.hDms, "tuner", "Channel", MX_FALSE, MX_TRUE );

	HxLOG_Print( "[ PL_DMS ] mxDlnaDmsEpg_createRootContainer( ret = %d )\n", nRet );

	/* HOMMA_INTEROP_SCENARIO_Init() is already done at DLNA_Init();InitChannelList() */

#ifdef HOMMA_USE_APPKIT_DIRECTLY
	__register_grouplist_notifier();
#endif

	__make_channellist(); /* Added error_check because dms_start() blocking after factory-default */

	// 2014/3/17 mhkang: STB에서 채널정보가 변경(Add/Modify/Delete)되었을 때, 호출되는 콜백 등록.
	PL_COMMON_SetChannelNotifier(__make_channellist);
	return;
}

HCHAR	*PL_DMS_GetLocalPathfromUri(HCHAR *pUri)
{
	if(pUri == NULL) return NULL;

	return mxDlnaDms_getURI2Path(pUri);
}
