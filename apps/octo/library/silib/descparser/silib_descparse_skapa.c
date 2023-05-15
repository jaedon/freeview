/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/**
	@file     sxdescparse_skapa.c
	@brief    sxdescparse_skapa.c (Descriptor Parser)

	Description	: SI Descriptor Parser.      								\n
	Author		: chmin
	Remarks		: 															\n

	Copyright (c) 2013 HUMAX Co., Ltd.										\n
	All rights reserved.													\n
*/

#include <hlib.h>
#include <silib.h>
#include "_silib_descparse.h"

#define	return_if(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define goto_if(expr, pos)		do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

HERROR silib_desc_descAlloc_Skapa(const SIxDescriptorTag_e eTag, const HCHAR *pszParam, const HUINT32 ulRawLength, void **ppvDesc)
{
	HERROR			 hErr;

	hErr = ERR_FAIL;
	switch (eTag)
	{
 	case eSIxDESCTAG_VIDEO_STREAM:
 	case eSIxDESCTAG_AUDIO_STREAM:
 	case eSIxDESCTAG_HIERARCHY:
 	case eSIxDESCTAG_REGISTRATION:
 	case eSIxDESCTAG_DATA_STREAM_ALIGNMENT:
	default:
		break;
	}

	return hErr;
}

HERROR silib_desc_descFree_Skapa(const SIxDescriptorTag_e eTag, const HCHAR *pszParam, void *pvDesc)
{
	HERROR			 hErr;

	hErr = ERR_FAIL;
	switch (eTag)
	{
 	case eSIxDESCTAG_VIDEO_STREAM:
 	case eSIxDESCTAG_AUDIO_STREAM:
 	case eSIxDESCTAG_HIERARCHY:
 	case eSIxDESCTAG_REGISTRATION:
 	case eSIxDESCTAG_DATA_STREAM_ALIGNMENT:
	default:
		break;
	}

	return hErr;
}


HERROR silib_desc_descParse_Skapa(const SIxDescriptorTag_e eTag, const HCHAR *pszParam, const HUINT8 *pucRawData, void *pvDesc)
{
	HERROR			 hErr;

	hErr = ERR_FAIL;
	switch (eTag)
	{
 	case eSIxDESCTAG_VIDEO_STREAM:
 	case eSIxDESCTAG_AUDIO_STREAM:
 	case eSIxDESCTAG_HIERARCHY:
 	case eSIxDESCTAG_REGISTRATION:
 	case eSIxDESCTAG_DATA_STREAM_ALIGNMENT:
 	case eSIxDESCTAG_TARGET_BACKGROUND_GRID:
 	case eSIxDESCTAG_VIDEO_WINDOW:
 	case eSIxDESCTAG_CA:
 	case eSIxDESCTAG_ISO_639_LANGUAGE:
 	case eSIxDESCTAG_SYSTEM_CLOCK:
 	case eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION:
 	case eSIxDESCTAG_COPYRIGHT:
 	case eSIxDESCTAG_MAXIMUM_BITRATE:
 	case eSIxDESCTAG_PRIVATE_DATA_INDICATOR:
 	case eSIxDESCTAG_SMOOTHING_BUFFER:
 	case eSIxDESCTAG_STD:
 	case eSIxDESCTAG_CAROUSEL_ID:
 	case eSIxDESCTAG_ASSOCIATION_TAG:
 	case eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG:
	case eSIxDESCTAG_AVC_VIDEO:
	/** < ETS 300 468에 정의된 descriptor tag */
 	case eSIxDESCTAG_NETWORK_NAME:
 	case eSIxDESCTAG_SERVICE_LIST:
 	case eSIxDESCTAG_STUFFING:
 	case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:
 	case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:
 	case eSIxDESCTAG_BOUQUET_NAME:
 	case eSIxDESCTAG_SERVICE:
 	case eSIxDESCTAG_COUNTRY_AVAILABILITY:
 	case eSIxDESCTAG_LINKAGE:
 	case eSIxDESCTAG_NVOD_REFERENCE:
 	case eSIxDESCTAG_TIME_SHIFTED_SERVICE:
 	case eSIxDESCTAG_SHORT_EVENT:
 	case eSIxDESCTAG_EXTENDED_EVENT:
 	case eSIxDESCTAG_TIME_SHIFTED_EVENT:
 	case eSIxDESCTAG_COMPONENT:
 	case eSIxDESCTAG_MOSAIC:
 	case eSIxDESCTAG_STREAM_IDENTIFIER:
 	case eSIxDESCTAG_CA_IDENTIFIER:
 	case eSIxDESCTAG_CONTENT:
 	case eSIxDESCTAG_PARENTAL_RATING:
 	case eSIxDESCTAG_TELETEXT:
 	case eSIxDESCTAG_TELEPHONE:
 	case eSIxDESCTAG_LOCAL_TIME_OFFSET:
 	case eSIxDESCTAG_SUBTITLING:
 	case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:
 	case eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME:
 	case eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME:
 	case eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME:
 	case eSIxDESCTAG_MULTILINGUAL_COMPONENT:
 	case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:
 	case eSIxDESCTAG_SERVICE_MOVE:
 	case eSIxDESCTAG_SHORT_SMOOTHING_BUFFER:
	case eSIxDESCTAG_FREQUENCY_LIST:
	case eSIxDESCTAG_DATA_BROADCAST:
	case eSIxDESCTAG_DATA_BROADCAST_ID:
	case eSIxDESCTAG_PDC:
	case eSIxDESCTAG_DOLBY_AC3:
	case eSIxDESCTAG_ANCILLARY_DATA:
	case eSIxDESCTAG_CELL_LIST:
	case eSIxDESCTAG_CELL_FREQ_LIST:
	case eSIxDESCTAG_ANNOUNCEMENT_SUPPORT:
	case eSIxDESCTAG_APPLICATION_SIGNALLING:
	case eSIxDESCTAG_ADAPTATION_FIELD_DATA:
	case eSIxDESCTAG_SVC_IDENTIFIER:
	case eSIxDESCTAG_SVC_AVAILABILITY:
	case eSIxDESCTAG_DEFAULT_AUTHORITY:
	case eSIxDESCTAG_RELATED_CONTENT:
	case eSIxDESCTAG_CONTENT_IDENTIFIER:
	case eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM:
	case eSIxDESCTAG_ENHANCED_AC3:
	case eSIxDESCTAG_DTS:
	case eSIxDESCTAG_AAC:
	case eSIxDESCTAG_XAIT_LOCATION:
	case eSIxDESCTAG_FTA_CONTENT_MANAGEMENT:
	case eSIxDESCTAG_EXTENSION:
	default:
		break;
	}

	return hErr;
}

