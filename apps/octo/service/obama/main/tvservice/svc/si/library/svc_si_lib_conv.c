/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_lib_conv.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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
#include <octo_common.h>
#include <pal_sirawpool.h>
#include <db_svc.h>
#include <isosvc.h>
#include <svc_si.h>
//#include "../../si/local_include/si_service.h"
#include <_svc_si.h>
#include <_svc_si_lib.h>

#include <psvc_si.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

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
HERROR svc_si_LibConvertSiCasIdToCasType (HUINT32 ulCasId, DxCasType_e *peCasType)
{
	HUINT16			 usMaskedId;

	if (peCasType == NULL)			{ return ERR_FAIL; }

	usMaskedId = (HUINT16)(ulCasId & CAS_ID_MASK_ALL);
	switch (usMaskedId)
	{
	case CAS_ID_MASK_MG:		*peCasType = eDxCAS_TYPE_MEDIAGUARD;	break;
	case CAS_ID_MASK_VA:		*peCasType = eDxCAS_TYPE_VIACCESS;		break;
	case CAS_ID_MASK_IR:		*peCasType = eDxCAS_TYPE_IRDETO;		break;
	case CAS_ID_MASK_CX:		*peCasType = eDxCAS_TYPE_CONAX;			break;
	case CAS_ID_MASK_CR:		*peCasType = eDxCAS_TYPE_CRYPTOWORKS;	break;
	case CAS_ID_MASK_NA:		*peCasType = eDxCAS_TYPE_NAGRA;			break;
	case CAS_ID_MASK_ND:		*peCasType = eDxCAS_TYPE_NDS;			break;
	case CAS_ID_MASK_VX:		*peCasType = eDxCAS_TYPE_VERIMATRIX;	break;
	default:					*peCasType = eDxCAS_TYPE_All;			break;
	}

	return ERR_OK;
}

HERROR svc_si_LibConvertSiStreamTypeToVideoCodec (HUINT32 ulStreamType, DxVideoCodec_e *peCodec)
{
	if (NULL == peCodec)			{ return ERR_FAIL; }

	/* SI에서 결정 하기로 합의 됨. */
	switch (ulStreamType)
	{
	/* VIDEO */
	case ISO_VIDEO_1:
	case ISO_VIDEO_2:
		*peCodec = eDxVIDEO_CODEC_MPEG2;
		break;

	case ISO_VIDEO_4:
		*peCodec = eDxVIDEO_CODEC_H264;
		break;

	case ISO_VIDEO_5:
		*peCodec = eDxVIDEO_CODEC_H265;
		break;

	default:
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_LibConvertSiStreamTypeToAudioCodec (HUINT32 ulStreamType, DxAudioCodec_e *peCodec)
{
	if (NULL == peCodec)			{ return ERR_FAIL; }

	/* SI에서 결정 하기로 합의 됨. */
	switch (ulStreamType)
	{
	/* AUDIO */
	case ISO_AUDIO_1:	/* ISO/IEC 11172 Audio */
		*peCodec = eDxAUDIO_CODEC_MPEG;
		break;

	case ISO_AUDIO_2:	/* ISO/IEC 13818-3 Audio */
		*peCodec = eDxAUDIO_CODEC_MPEG;
		break;

	case ISO_AUDIO_AAC:
		*peCodec = eDxAUDIO_CODEC_AAC;
		break;

	case ISO_AUDIO_4:
		*peCodec = eDxAUDIO_CODEC_AAC_PLUS;
		break;

	case ISO_AUDIO_AC3:
		*peCodec = eDxAUDIO_CODEC_DOLBY_AC3;
		break;

	default:
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_LibConvertSiAacTypeToAudioCodec (HUINT8 ucAacType, DxAudioCodec_e *peAudioCodec)
{
	if (NULL == peAudioCodec)			{ return ERR_FAIL; }

	switch (ucAacType)
	{
	case 0x01:
	case 0x03:
	case 0x05:
	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4A:
	case 0x58:
	case 0x59:
	case 0x5A:
	case 0x5B:
		*peAudioCodec = eDxAUDIO_CODEC_AAC_PLUS;
		break;

	default:
		*peAudioCodec = eDxAUDIO_CODEC_AAC;
		break;
	}

	return ERR_OK;
}

HERROR svc_si_LibConvertSiAacProfileLevelToChannelNum (HUINT8 ucProfileAndLevel, HUINT8 *pucChanNum)
{
	if (NULL == pucChanNum)			{ return ERR_FAIL; }

	if ((ucProfileAndLevel >= 0x50) && (ucProfileAndLevel <= 0x59))
	{
		*pucChanNum = SvcSi_AUDIO_CHANNEL_NUM_STEREO;
	}
	else if ((ucProfileAndLevel >= 0x5A) && (ucProfileAndLevel <= 0x5B))
	{
		*pucChanNum = SvcSi_AUDIO_CHANNEL_NUM_MULTI;
	}
	else
	{
		*pucChanNum = SvcSi_AUDIO_CHANNEL_NUM_NA;
	}

	return ERR_OK;
}

HERROR svc_si_LibConvertSiAc3ComponentTypeToChannelNum (HUINT8 ucAc3CompType, HUINT8 *pucChanNum)
{
	HUINT8			 ucValue = (ucAc3CompType & 0x07);

	if (NULL == pucChanNum)			{ return ERR_FAIL; }

	if (ucValue <= 0x03)
	{
		*pucChanNum = SvcSi_AUDIO_CHANNEL_NUM_STEREO;
	}
	else if (ucValue <= 0x05)
	{
		*pucChanNum = SvcSi_AUDIO_CHANNEL_NUM_MULTI;
	}
	else
	{
		*pucChanNum = SvcSi_AUDIO_CHANNEL_NUM_NA;
	}

	return ERR_OK;
}

HERROR svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (HUINT32 ulActionId, HUINT32 ulDvbSvcType, DxSvcType_e *peSvcType, DxVideoType_e *peVideoType)
{
	HERROR			 hErr;

	HxLOG_Info ("(+)\n");
	hErr = psvc_si_GetSvcAndVideoTypeFromDvbSvcType (ulActionId, ulDvbSvcType, peSvcType, peVideoType);
	HxLOG_Info ("(-) err:0x%08x\n", hErr);

	return hErr;
}
/* end of file */

