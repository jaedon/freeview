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
	@file     sxdescparse_dvb.c
	@brief    sxdescparse_dvb.c (Descriptor Parser)

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

static HUINT8 	*silib_desc_allocInfo_Dvb(SIxDescriptorTag_e eTag, HUINT16 usSubTag);
static void 	 silib_desc_freeInfo_Dvb(SIxDescriptorTag_e eTag, HUINT16 usSubTag, HUINT8 *pucDesInfo);

static HCHAR *silib_desc_getTagString_Dvb(SIxDescriptorTag_e eTag)
{
	switch (eTag)
	{
 	ENUM_TO_STR(eSIxDESCTAG_VIDEO_STREAM);
 	ENUM_TO_STR(eSIxDESCTAG_AUDIO_STREAM);
 	ENUM_TO_STR(eSIxDESCTAG_HIERARCHY);
 	ENUM_TO_STR(eSIxDESCTAG_REGISTRATION);
 	ENUM_TO_STR(eSIxDESCTAG_DATA_STREAM_ALIGNMENT);
 	ENUM_TO_STR(eSIxDESCTAG_TARGET_BACKGROUND_GRID);
 	ENUM_TO_STR(eSIxDESCTAG_VIDEO_WINDOW);
 	ENUM_TO_STR(eSIxDESCTAG_CA);
 	ENUM_TO_STR(eSIxDESCTAG_ISO_639_LANGUAGE);
 	ENUM_TO_STR(eSIxDESCTAG_SYSTEM_CLOCK);
 	ENUM_TO_STR(eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION);
 	ENUM_TO_STR(eSIxDESCTAG_COPYRIGHT);
 	ENUM_TO_STR(eSIxDESCTAG_MAXIMUM_BITRATE);
 	ENUM_TO_STR(eSIxDESCTAG_PRIVATE_DATA_INDICATOR);
 	ENUM_TO_STR(eSIxDESCTAG_SMOOTHING_BUFFER);
 	ENUM_TO_STR(eSIxDESCTAG_STD);
 	ENUM_TO_STR(eSIxDESCTAG_CAROUSEL_ID);
 	ENUM_TO_STR(eSIxDESCTAG_ASSOCIATION_TAG);
 	ENUM_TO_STR(eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG);
	ENUM_TO_STR(eSIxDESCTAG_AVC_VIDEO);
	/** < ETS 300 468에 정의된 descriptor tag */
 	ENUM_TO_STR(eSIxDESCTAG_NETWORK_NAME);
 	ENUM_TO_STR(eSIxDESCTAG_SERVICE_LIST);
 	ENUM_TO_STR(eSIxDESCTAG_STUFFING);
 	ENUM_TO_STR(eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM);
 	ENUM_TO_STR(eSIxDESCTAG_CABLE_DELIVERY_SYSTEM);
 	ENUM_TO_STR(eSIxDESCTAG_BOUQUET_NAME);
 	ENUM_TO_STR(eSIxDESCTAG_SERVICE);
 	ENUM_TO_STR(eSIxDESCTAG_COUNTRY_AVAILABILITY);
 	ENUM_TO_STR(eSIxDESCTAG_LINKAGE);
 	ENUM_TO_STR(eSIxDESCTAG_NVOD_REFERENCE);
 	ENUM_TO_STR(eSIxDESCTAG_TIME_SHIFTED_SERVICE);
 	ENUM_TO_STR(eSIxDESCTAG_SHORT_EVENT);
 	ENUM_TO_STR(eSIxDESCTAG_EXTENDED_EVENT);
 	ENUM_TO_STR(eSIxDESCTAG_TIME_SHIFTED_EVENT);
 	ENUM_TO_STR(eSIxDESCTAG_COMPONENT);
 	ENUM_TO_STR(eSIxDESCTAG_MOSAIC);
 	ENUM_TO_STR(eSIxDESCTAG_STREAM_IDENTIFIER);
 	ENUM_TO_STR(eSIxDESCTAG_CA_IDENTIFIER);
 	ENUM_TO_STR(eSIxDESCTAG_CONTENT);
 	ENUM_TO_STR(eSIxDESCTAG_PARENTAL_RATING);
 	ENUM_TO_STR(eSIxDESCTAG_TELETEXT);
 	ENUM_TO_STR(eSIxDESCTAG_TELEPHONE);
 	ENUM_TO_STR(eSIxDESCTAG_LOCAL_TIME_OFFSET);
 	ENUM_TO_STR(eSIxDESCTAG_SUBTITLING);
 	ENUM_TO_STR(eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM);
 	ENUM_TO_STR(eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME);
 	ENUM_TO_STR(eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME);
 	ENUM_TO_STR(eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME);
 	ENUM_TO_STR(eSIxDESCTAG_MULTILINGUAL_COMPONENT);
 	ENUM_TO_STR(eSIxDESCTAG_PRIVATE_DATA_SPECIFIER);
 	ENUM_TO_STR(eSIxDESCTAG_SERVICE_MOVE);
 	ENUM_TO_STR(eSIxDESCTAG_SHORT_SMOOTHING_BUFFER);
	ENUM_TO_STR(eSIxDESCTAG_FREQUENCY_LIST);
	ENUM_TO_STR(eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM);
	ENUM_TO_STR(eSIxDESCTAG_DATA_BROADCAST);
	ENUM_TO_STR(eSIxDESCTAG_DATA_BROADCAST_ID);
	ENUM_TO_STR(eSIxDESCTAG_PDC);
	ENUM_TO_STR(eSIxDESCTAG_DOLBY_AC3);
	ENUM_TO_STR(eSIxDESCTAG_ANCILLARY_DATA);
	ENUM_TO_STR(eSIxDESCTAG_CELL_LIST);
	ENUM_TO_STR(eSIxDESCTAG_CELL_FREQ_LIST);
	ENUM_TO_STR(eSIxDESCTAG_ANNOUNCEMENT_SUPPORT);
	ENUM_TO_STR(eSIxDESCTAG_APPLICATION_SIGNALLING);
	ENUM_TO_STR(eSIxDESCTAG_ADAPTATION_FIELD_DATA);
	ENUM_TO_STR(eSIxDESCTAG_SVC_IDENTIFIER);
	ENUM_TO_STR(eSIxDESCTAG_SVC_AVAILABILITY);
	ENUM_TO_STR(eSIxDESCTAG_DEFAULT_AUTHORITY);
	ENUM_TO_STR(eSIxDESCTAG_RELATED_CONTENT);
	ENUM_TO_STR(eSIxDESCTAG_CONTENT_IDENTIFIER);
	ENUM_TO_STR(eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM);
	ENUM_TO_STR(eSIxDESCTAG_ENHANCED_AC3);
	ENUM_TO_STR(eSIxDESCTAG_DTS);
	ENUM_TO_STR(eSIxDESCTAG_AAC);
	ENUM_TO_STR(eSIxDESCTAG_XAIT_LOCATION);
	ENUM_TO_STR(eSIxDESCTAG_FTA_CONTENT_MANAGEMENT);
	ENUM_TO_STR(eSIxDESCTAG_EXTENSION);
	default:
		break;
	}

	return NULL;
}

/************************************************************************
 * eSIxDESCTAG_PRIVATE_DATA (TAG = 0x00)
 ************************************************************************/


/************************************************************************
 * eSIxDESCTAG_VIDEO_STREAM (TAG = 0x02)
 ************************************************************************/
static void initVideoStmDes(SIxVideoStmDes_t *pstDes)
{
	pstDes->bMultiFrameRate		= FALSE;
	pstDes->ucFrameRateCode		= 0;
	pstDes->bMPEG2				= FALSE;
	pstDes->bConstrainParam		= FALSE;
	pstDes->bStillPic			= FALSE;
}

static SIxVideoStmDes_t *allocVideoStmDes(void)
{
	return (SIxVideoStmDes_t *)silib_desc_alloc(sizeof(SIxVideoStmDes_t));
}

static void freeVideoStmDes(SIxVideoStmDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setVideoStmDes(SIxVideoStmDes_t *pstDes, HBOOL bMultiFrameRate, HUINT8 ucFrameRateCode, HBOOL bMPEG2, HBOOL bConstrainParam, HBOOL bStillPic)
{
	pstDes->bMultiFrameRate 	= bMultiFrameRate;
	pstDes->ucFrameRateCode 	= ucFrameRateCode;
	pstDes->bMPEG2 				= bMPEG2;
	pstDes->bConstrainParam 	= bConstrainParam;
	pstDes->bStillPic 			= bStillPic;
}

static SIxError_e parseVideoStmDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxVideoStmDes_t *pstDes)
{
	HUINT8			ucFrameRateCode;
	HBOOL			bMultiFrameRate, bMPEG2, bConstrainParam, bStillPic;
	HUINT8			ucTemp, ucVal;
	HUINT16			usDesLen;

	HxLOG_Trace();
	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Info("[WARN] Unknown parseVideoStmDes Format!\n");
		return eSIxERR_FAIL;
	}

	/** < multiple frame rate flag & frame rate flag & MPEG 2 flag
		 & constraned parameter flag & still picture flag (1byte) */
	ucTemp = *p++;
	ucVal = (HUINT8)((ucTemp & 0x80) >> 7);
	(ucVal == 1) ? (bMultiFrameRate = TRUE) : (bMultiFrameRate = FALSE);
	ucFrameRateCode = (HUINT8)((*p & 0x78) >> 3);
	ucVal = (HUINT8)((ucTemp & 0x04) >> 2);
	(ucVal == 1) ? (bMPEG2 = TRUE) : (bMPEG2 = FALSE);
	ucVal = (HUINT8)((ucTemp & 0x02) >> 1);
	(ucVal == 1) ? (bConstrainParam = TRUE) : (bConstrainParam = FALSE);
	ucVal = ucTemp & 0x01;
	(ucVal == 1) ? (bStillPic = TRUE) : (bStillPic = FALSE);
	HxLOG_Print("\t\t(multiple frame rate flag = 0x%d)\n", bMultiFrameRate);
	HxLOG_Print("\t\t(frame rate flag = 0x%x)\n", ucFrameRateCode);
	HxLOG_Print("\t\t(MPEG 2 flag = 0x%x)\n", bMPEG2);
	HxLOG_Print("\t\t(constraned parameter flag = 0x%d)\n", bConstrainParam);
	HxLOG_Print("\t\t(still picture flag = 0x%d)\n", bStillPic);

	/** < Descriptor 데이타 구성. */
	setVideoStmDes(pstDes, bMultiFrameRate, ucFrameRateCode, bMPEG2, bConstrainParam, bStillPic);

	HxLOG_Trace();

	return eSIxERR_OK;
}

static SIxError_e copyVideoStmDes(SIxVideoStmDes_t *pstDest, const SIxVideoStmDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxVideoStmDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static void printVideoStmDes(SIxVideoStmDes_t *pstDes)
{
	if (NULL == pstDes)	return;

	HxLOG_Print("\t\t(multiple frame rate flag = 0x%d)\n", pstDes->bMultiFrameRate);
	HxLOG_Print("\t\t(frame rate flag = 0x%x)\n", pstDes->ucFrameRateCode);
	HxLOG_Print("\t\t(MPEG 2 flag = 0x%x)\n", pstDes->bMPEG2);
	HxLOG_Print("\t\t(constraned parameter flag = 0x%d)\n", pstDes->bConstrainParam);
	HxLOG_Print("\t\t(still picture flag = 0x%d)\n", pstDes->bStillPic);
}

/************************************************************************
 * eSIxDESCTAG_AUDIO_STREAM (TAG = 0x03)
 ************************************************************************/
static void initAudioStmDes(SIxAudioStmDes_t *pstDes)
{
	pstDes->bFreeFormat				= FALSE;
	pstDes->ucId					= 0;
	pstDes->ucLayer					= 0;
	pstDes->ucVarRateAudioIndicator	= 0;
}

static SIxAudioStmDes_t *allocAudioStmDes(void)
{
	return (SIxAudioStmDes_t *)silib_desc_alloc(sizeof(SIxAudioStmDes_t));
}

static void freeAudioStmDes(SIxAudioStmDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setAudioStmDes(SIxAudioStmDes_t *pstDes, HBOOL bFreeFormat, HUINT8 ucId, HUINT8 ucLayer, HUINT8 ucVarRateAudioIndicator)
{
	pstDes->bFreeFormat 				= bFreeFormat;
	pstDes->ucId 						= ucId;
	pstDes->ucLayer 					= ucLayer;
	pstDes->ucVarRateAudioIndicator 	= ucVarRateAudioIndicator;
}

static 	SIxError_e	parseAudioStmDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxAudioStmDes_t *pstDes)
{
	HUINT8			ucId, ucLayer, ucVarRateAudioIndicator;
	HBOOL			bFreeFormat;
	HUINT8			ucTemp, ucVal;
	HUINT16			usDesLen;

	HxLOG_Info("parseAudioStmDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown parseAudioStmDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < free format flag & ID & layer & variable rate audio indicator (1byte) */
	ucTemp = *p;
	ucVal = (HUINT8)((ucTemp & 0x80) >> 7);
	(ucVal == 1) ? (bFreeFormat = TRUE) : (bFreeFormat = FALSE);
	ucId = (HUINT8)((ucTemp & 0x40) >> 6);
	ucLayer = (HUINT8)((ucTemp & 0x30) >> 4);
	ucVarRateAudioIndicator = (HUINT8)((ucTemp & 0x08) >> 3);
	HxLOG_Print("\t\t(free format flag = 0x%d)\n", bFreeFormat);
	HxLOG_Print("\t\t(ID = 0x%x)\n", ucId);
	HxLOG_Print("\t\t(layer = 0x%x)\n", ucLayer);
	HxLOG_Print("\t\t(variable rate audio indicator = 0x%x)\n", ucVarRateAudioIndicator);

	/** < Descriptor 데이타 구성. */
	setAudioStmDes (pstDes, bFreeFormat, ucId, ucLayer, ucVarRateAudioIndicator);

	HxLOG_Info(" parseAudioStmDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyAudioStmDes(SIxAudioStmDes_t *pstDest, const SIxAudioStmDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAudioStmDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static void printAudioStmDes(const SIxAudioStmDes_t *pstDes)
{
}


/************************************************************************
 * eSIxDESCTAG_HIERARCHY (TAG = 0x04)
 ************************************************************************/
static void initHierDes(SIxHierDes_t *pstDes)
{
	pstDes->ucHierType				= 0;
	pstDes->ucHierLayerIndex		= 0;
	pstDes->ucHierEmbedLayerIndex	= 0;
	pstDes->ucHierChannel			= 0;
}

static SIxHierDes_t *allocHierDes(void)
{
	return (SIxHierDes_t *)silib_desc_alloc(sizeof(SIxHierDes_t));
}

static void freeHierDes(SIxHierDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setHierDes(SIxHierDes_t *pstDes, HUINT8 ucHierType, HUINT8 ucHierLayerIndex, HUINT8 ucHierEmbedLayerIndex, HUINT8 ucHierChannel)
{
	pstDes->ucHierType 				= ucHierType;
	pstDes->ucHierLayerIndex 		= ucHierLayerIndex;
	pstDes->ucHierEmbedLayerIndex 	= ucHierEmbedLayerIndex;
	pstDes->ucHierChannel 			= ucHierChannel;
}

static SIxError_e parseHierDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxHierDes_t *pstDes)
{
	HUINT8		ucHierType, ucHierLayerIndex, ucHierEmbedLayerIndex, ucHierChannel;
	HUINT16		usDesLen;

	HxLOG_Info("parseHierDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown parseHierDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < hierarchy type (1byte) */
	ucHierType = *p++ & 0x04;
	HxLOG_Print("\t\t(hierarchy type = 0x%x)\n", ucHierType);

	/** < hierarchy layer index (1byte) */
	ucHierLayerIndex = *p++ & 0x3f;
	HxLOG_Print("\t\t(hierarchy layer index = 0x%x)\n", ucHierLayerIndex);

	/** < hierarchy embedded layer index (1byte) */
	ucHierEmbedLayerIndex = *p++ & 0x3f;
	HxLOG_Print("\t\t(hierarchy embedded layer index = 0x%x)\n", ucHierEmbedLayerIndex);

	/** < hierarchy service (1byte) */
	ucHierChannel = *p & 0x3f;
	HxLOG_Print("\t\t(hierarchy service = 0x%x)\n", ucHierChannel);

	/** < Descriptor 데이타 구성. */
	setHierDes(pstDes, ucHierType, ucHierLayerIndex, ucHierEmbedLayerIndex, ucHierChannel);

	HxLOG_Info(" parseHierDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyHierDes(SIxHierDes_t *pstDest, const SIxHierDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxHierDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_REGISTRATION (TAG = 0x05)
 ************************************************************************/

static void initRegDes(SIxRegDes_t *pstDes)
{
	pstDes->ulFormatIdent		= 0;
	pstDes->pstPrivateDataInfo	= NULL;
}

static SIxRegDes_t *allocRegDes(void)
{
	return (SIxRegDes_t *)silib_desc_alloc(sizeof(SIxRegDes_t));
}

static void freeRegDes(SIxRegDes_t *pstDes)
{
	if (NULL == pstDes)	return;

	silib_desc_util_freePrivateDataInfo(pstDes->pstPrivateDataInfo);
	silib_desc_free(pstDes);
}

static void setRegDes(SIxRegDes_t *pstDes, HUINT32 ulFormatIdent, SIxPrivateDataInfo_t *pstPrivateDataInfo)
{
	pstDes->ulFormatIdent		= ulFormatIdent;
	pstDes->pstPrivateDataInfo	= pstPrivateDataInfo;
}

static SIxError_e parseRegDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxRegDes_t *pstDes)
{
	SIxPrivateDataInfo_t	*pstNewInfo = NULL;
	HUINT16				 usDesLen;
	HUINT32				 ulFormatId;

	HxLOG_Info("parseRegDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4 || usDesLen > MAX_DESCRIPTOR_LIMIT)
	{
		HxLOG_Print("[WARN] Unknown parseRegDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < format identifier (4byte) */
	ulFormatId = HxMACRO_Get32Bit(p);
	p += 4;
	usDesLen -= 4;
	HxLOG_Print("\t\t(format identifier = 0x%x)\n", ulFormatId);

	/** < Private Data */
	if (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		SIxError_e	eErr;
		pstNewInfo = silib_desc_util_allocPrivateDataInfo((HUINT8)usDesLen);
		if (pstNewInfo == NULL)
			goto err_exit;

		eErr = silib_desc_util_setPrivateDataInfo(pstNewInfo, p, (HUINT8)usDesLen);
		if (eSIxERR_OK != eErr)
		{
			silib_desc_util_freePrivateDataInfo(pstNewInfo);
			goto err_exit;
		}
	}

	/** < Descriptor 데이타 구성. */
	setRegDes(pstDes, ulFormatId, pstNewInfo);

	HxLOG_Info(" parseRegDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	return eSIxERR_FAIL;
}

static SIxError_e copyRegDes(SIxRegDes_t *pstDest, const SIxRegDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxRegDes_t));
		pstDest->pstPrivateDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPrivateDataInfo);
		if (NULL == pstDest->pstPrivateDataInfo)
			goto err_exit;
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_util_freePrivateDataInfo(pstDest->pstPrivateDataInfo);
		pstDest->pstPrivateDataInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_DATA_STREAM_ALIGNMENT (TAG = 0x06)
 ************************************************************************/
static void initDataStmAlignDes(SIxDataStmAlignDes_t *pstDes)
{
	pstDes->ucAlignType = 0;
}

static SIxDataStmAlignDes_t *allocDataStmAlignDes(void)
{
	return (SIxDataStmAlignDes_t *)silib_desc_alloc(sizeof(SIxDataStmAlignDes_t));
}

static void freeDataStmAlignDes(SIxDataStmAlignDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setDataStmAlignDes(SIxDataStmAlignDes_t *pstDes, HUINT8 ucAlignType)
{
	pstDes->ucAlignType = ucAlignType;
}

static SIxError_e parseDataStmAlignDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxDataStmAlignDes_t *pstDes)
{
	HUINT8				ucAlignType;
	HUINT16				usDesLen;

	HxLOG_Info("parseDataStmAlignDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown parseDataStmAlignDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < alignment type (1byte) */
	ucAlignType = *p;
	HxLOG_Print("\t\t(alignment type = 0x%x)\n", ucAlignType);

	/** < Descriptor 데이타 구성. */
	setDataStmAlignDes(pstDes, ucAlignType);

	HxLOG_Info("parseDataStmAlignDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyDataStmAlignDes(SIxDataStmAlignDes_t *pstDest, const SIxDataStmAlignDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDataStmAlignDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_TARGET_BACKGROUND_GRID (TAG = 0x07)
 ************************************************************************/
static void initBackGridDes(SIxBackGridDes_t *pstDes)
{
	pstDes->usHoriSize		= 0;
	pstDes->usVertSize		= 0;
	pstDes->ucAspectRatio	= 0;
}

static SIxBackGridDes_t *allocBackGridDes(void)
{
	return (SIxBackGridDes_t *)silib_desc_alloc(sizeof(SIxBackGridDes_t));
}

static void freeBackGridDes(SIxBackGridDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setBackGridDes(SIxBackGridDes_t *pstDes, HUINT16 usHoriSize, HUINT16 usVertSize, HUINT8 ucAspectRatio)
{
	pstDes->usHoriSize 		= usHoriSize;
	pstDes->usVertSize 		= usVertSize;
	pstDes->ucAspectRatio	= ucAspectRatio;
}

static SIxError_e parseBackGridDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxBackGridDes_t *pstDes)
{
	HUINT16			usHorSize, usVerSize, usDesLen;
	HUINT8			ucAspectRatio;
	HUINT32			ulTemp;

	HxLOG_Info("parseBackGridDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown parseBackGridDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < horizontal size & vertical size & aspect ratio information (4byte) */
	ulTemp = HxMACRO_Get32Bit(p);
	usHorSize = (HUINT16)((ulTemp & 0xfffc0000) >> 18);
	usVerSize = (HUINT16)((ulTemp & 0x0003fff0) >> 4);
	ucAspectRatio = (HUINT8)(ulTemp & 0x0000000f);
	HxLOG_Print("\t\t(horizontal size = 0x%x)\n", usHorSize);
	HxLOG_Print("\t\t(vertical size = 0x%x)\n", usVerSize);
	HxLOG_Print("\t\t(aspect ratio information = 0x%x)\n", ucAspectRatio);

	/** < Descriptor 데이타 구성. */
	setBackGridDes(pstDes, usHorSize, usVerSize, ucAspectRatio);

	HxLOG_Info(" parseBackGridDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyBackGridDes(SIxBackGridDes_t *pstDest, const SIxBackGridDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxBackGridDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_VIDEO_WINDOW (TAG = 0x08)
 ************************************************************************/

static void initVideoWindowDes(SIxVideoWindowDes_t *pstDes)
{
	pstDes->usHoriOff			= 0;
	pstDes->usVertOff			= 0;
	pstDes->ucWindowPriority	= 0;
}

static SIxVideoWindowDes_t *allocVideoWindowDes(void)
{
	return (SIxVideoWindowDes_t *)silib_desc_alloc(sizeof(SIxVideoWindowDes_t));
}

static void freeVideoWindowDes(SIxVideoWindowDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setVideoWindowDes(SIxVideoWindowDes_t *pstDes, HUINT16 usHoriOff, HUINT16 usVertOff, HUINT8 ucWindowPriority)
{
	pstDes->usHoriOff			= usHoriOff;
	pstDes->usVertOff			= usVertOff;
	pstDes->ucWindowPriority	= ucWindowPriority;
}

static SIxError_e parseVideoWindowDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxVideoWindowDes_t *pstDes)
{
	HUINT16				usDesLen, usHorOff, usVerOff;
	HUINT8				ucWindowPriority;
	HUINT32				ulTemp;

	HxLOG_Info("parseVideoWindowDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown parseVideoWindowDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < horizontal offset & vertical offset & window priority (4byte) */
	ulTemp = HxMACRO_Get32Bit(p);
	usHorOff = (HUINT16)((ulTemp & 0xfffc0000) >> 18);
	usVerOff = (HUINT16)((ulTemp & 0x0003fff0) >> 4);
	ucWindowPriority = (HUINT8)(ulTemp & 0x0000000f);
	HxLOG_Print("\t\t(horizontal offset = 0x%x)\n", usHorOff);
	HxLOG_Print("\t\t(vertical offset = 0x%x)\n", usVerOff);
	HxLOG_Print("\t\t(window priority = 0x%x)\n", ucWindowPriority);

	/** < Descriptor 데이타 구성. */
	setVideoWindowDes(pstDes, usHorOff, usVerOff, ucWindowPriority);

	HxLOG_Info(" parseVideoWindowDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyVideoWindowDes(SIxVideoWindowDes_t *pstDest, const SIxVideoWindowDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxVideoWindowDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_CA (TAG = 0x09)
 ************************************************************************/
static void initCaDes(SIxCaDes_t *pstDes)
{
	pstDes->usCaSysId 			= 0;
	pstDes->usCaPid 			= 0;
	pstDes->pstPrivateDataInfo	= NULL;
}

static SIxCaDes_t *allocCaDes(void)
{
	return (SIxCaDes_t *)silib_desc_alloc(sizeof(SIxCaDes_t));
}

static void freeCaDes(SIxCaDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_util_freePrivateDataInfo(pstDes->pstPrivateDataInfo);
	silib_desc_free(pstDes);
}

static void setCaDes(SIxCaDes_t *pstDes, HUINT16 usCaSysId, HUINT16 usCaPid, SIxPrivateDataInfo_t *pstPrivateDataInfo)
{
	pstDes->usCaSysId 			= usCaSysId;
	pstDes->usCaPid 			= usCaPid;
	pstDes->pstPrivateDataInfo	= pstPrivateDataInfo;
}

static SIxError_e parseCaDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCaDes_t *pstDes)
{
	SIxPrivateDataInfo_t	*pstNewInfo = NULL;
	HUINT16				 usDesLen, usCaSysId, usCaPid;

	HxLOG_Info("parseCaDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4)
	{
		HxLOG_Print("[WARN] Unknown parseCaDes Format!\n");
		return eSIxERR_INVALID;
	}

	usCaSysId = HxMACRO_Get16Bit(p);
	p += 2;
	usDesLen -=2;
	HxLOG_Print("\t\t(CA system id = 0x%x)\n", usCaSysId);

	usCaPid = HxMACRO_Get13Bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(CA pid = 0x%x)\n", usCaPid);

	/** < Private Data */
	if (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		SIxError_e	eErr;
		pstNewInfo = silib_desc_util_allocPrivateDataInfo((HUINT8)usDesLen);
		if (pstNewInfo == NULL)
			goto err_exit;

		eErr = silib_desc_util_setPrivateDataInfo(pstNewInfo, p, (HUINT8)usDesLen);
		if (eSIxERR_OK != eErr)
		{
			silib_desc_util_freePrivateDataInfo(pstNewInfo);
			goto err_exit;
		}
	}

	/** < Descriptor 데이타 구성. */
	setCaDes(pstDes, usCaSysId, usCaPid, pstNewInfo);

	HxLOG_Info(" parseCaDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	return eSIxERR_FAIL;
}

static SIxError_e copyCaDes(SIxCaDes_t *pstDest, const SIxCaDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCaDes_t));
		pstDest->pstPrivateDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPrivateDataInfo);
		if (NULL == pstDest->pstPrivateDataInfo)
			goto err_exit;
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_util_freePrivateDataInfo(pstDest->pstPrivateDataInfo);
		pstDest->pstPrivateDataInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_ISO_639_LANGUAGE (TAG = 0x0a)
 ************************************************************************/
static void initIso639LangDes(SIxIso639LangDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static void initLangInfo(SIxLangInfo_t *pLangInfo)
{
	HxSTD_MemSet(pLangInfo->szIso639LangCode, '\0', 4);
	pLangInfo->ucAudioType = 0;
}

static SIxIso639LangDes_t *allocIso639LangDes(void)
{
	return (SIxIso639LangDes_t *)silib_desc_alloc(sizeof(SIxIso639LangDes_t));
}

static SIxLangInfo_t *allocLangInfo(void)
{
	SIxLangInfo_t	*pstInfo;

	pstInfo = (SIxLangInfo_t *)silib_desc_alloc(sizeof(SIxLangInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocLangInfo()\n");
		return NULL;
	}
	initLangInfo(pstInfo);

	return ((SIxLangInfo_t *)pstInfo);
}


static void freeIso639LangDes(SIxIso639LangDes_t *pstDes)
{
	FREE_INFO_LIST(pstDes->pstInfo, SIxLangInfo_t);
	silib_desc_free(pstDes);
}

static void freeLangInfo(SIxLangInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setLangInfo(SIxLangInfo_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 ucAudioType)
{
	HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstInfo->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	pstInfo->ucAudioType = ucAudioType;
}

static SIxError_e parseIso639LangDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxIso639LangDes_t *pstDes)
{
	SIxLangInfo_t			*pstNewInfo = NULL, *pstNextInfo = NULL, *pstPrevInfo = NULL;
	HUINT8					 ucAudioType;
	HUINT8					 szIso639LangCode[4];
	HUINT16					 usDesLen;

	HxLOG_Info("parseIso639LangDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 4) != 0)
	{
		HxLOG_Print("[WARN] Unknown Iso639LangDes Format!\n");
		return eSIxERR_INVALID;
	}

	if (usDesLen == 0)
	{
		HxSTD_MemSet (szIso639LangCode, '\0', 4);
		ucAudioType = 0x0;

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxLangInfo_t, pstDes);
		pstNewInfo = allocLangInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setLangInfo(pstNewInfo, szIso639LangCode, ucAudioType);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, pstNextInfo, pstPrevInfo);
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 lang code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 lang code = %s)", (char *)szIso639LangCode);

		ucAudioType = *p++;
		usDesLen--;
		HxLOG_Print("(audio type = 0x%x)\n", ucAudioType);

		HxLOG_Print("\t\t\t(lang = %s)(audio type = 0x%x)\n", (char *)szIso639LangCode, ucAudioType);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxLangInfo_t, pstDes);
		pstNewInfo = allocLangInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setLangInfo(pstNewInfo, szIso639LangCode, ucAudioType);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, pstNextInfo, pstPrevInfo);
	}

	HxLOG_Info(" parseIso639LangDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyIso639LangDes(SIxIso639LangDes_t *pstDest, const SIxIso639LangDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxLangInfo_t	*pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxIso639LangDes_t));
		pstDest->pstInfo = NULL;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxLangInfo_t, pstDest);
			pstNewInfo = allocLangInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			setLangInfo(pstNewInfo, pstSourceInfo->szIso639LangCode, pstSourceInfo->ucAudioType);
			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST(pstDest->pstInfo, SIxLangInfo_t);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_SYSTEM_CLOCK (TAG = 0x0b)
 ************************************************************************/
static void initSysClockDes(SIxSysClockDes_t *pstDes)
{
	pstDes->ucExtClockRef		= 0;
	pstDes->ucClockAccuInt	= 0;
	pstDes->ucClockAccuExp	= 0;
}

static SIxSysClockDes_t *allocSysClockDes(void)
{
	return (SIxSysClockDes_t *)silib_desc_alloc(sizeof(SIxSysClockDes_t));
}

static void freeSysClockDes(SIxSysClockDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setSysClockDes(SIxSysClockDes_t *pstDes, HUINT8 ucExtClockRef, HUINT8 ucClockAccuInt, HUINT8 ucClockAccuExp)
{
	pstDes->ucExtClockRef		= ucExtClockRef;
	pstDes->ucClockAccuInt		= ucClockAccuInt;
	pstDes->ucClockAccuExp		= ucClockAccuExp;
}

static SIxError_e parseSysClockDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSysClockDes_t *pstDes)
{
	HUINT8			ucExtClockRef, ucClockAccuInt, ucClockAccuExp;
	HUINT8			ucTemp;
	HUINT16			usDesLen;

	HxLOG_Info("parseSysClockDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 2)
	{
		HxLOG_Print("[WARN] Unknown parseSysClockDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < external clock reference indicator & clock accuracy integer (1byte) */
	ucTemp = *p++;
	ucExtClockRef = (HUINT8)((ucTemp & 0x80) >> 7);
	ucClockAccuInt = ucTemp & 0x3f;
	HxLOG_Print("\t\t(external clock reference indicator = 0x%x)\n", ucExtClockRef);
	HxLOG_Print("\t\t(clock accuracy integer = 0x%x)\n", ucClockAccuInt);

	/** < clock accuracy exponent (1byte) */
	ucClockAccuExp = *p++ & 0xe0;
	HxLOG_Print("\t\t(clock accuracy exponent = 0x%x)\n", ucClockAccuExp);

	/** < Descriptor 데이타 구성. */
	setSysClockDes(pstDes, ucExtClockRef, ucClockAccuInt, ucClockAccuExp);

	HxLOG_Info(" parseSysClockDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copySysClockDes(SIxSysClockDes_t *pstDest, const SIxSysClockDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSysClockDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION (TAG = 0x0c)
 ************************************************************************/
static void initMultiplexBufUtilDes(SIxMultiplexBufUtilDes_t *pstDes)
{
	pstDes->bBoundValid			= 0;
	pstDes->usLtwOffLowBound	= 0;
	pstDes->usLtwOffUpBound		= 0;
}

static SIxMultiplexBufUtilDes_t *allocMultiplexBufUtilDes(void)
{
	return (SIxMultiplexBufUtilDes_t *)silib_desc_alloc(sizeof(SIxMultiplexBufUtilDes_t));
}

static void freeMultiplexBufUtilDes(SIxMultiplexBufUtilDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setMultiplexBufUtilDes(SIxMultiplexBufUtilDes_t *pstDes, HBOOL bBoundValid, HUINT16 usLtwOffLowBound, HUINT16 usLtwOffUpBound)
{
	pstDes->bBoundValid			= bBoundValid;
	pstDes->usLtwOffLowBound	= usLtwOffLowBound;
	pstDes->usLtwOffUpBound		= usLtwOffUpBound;
}

static SIxError_e parseMultiplexBufUtilDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxMultiplexBufUtilDes_t *pstDes)
{
	HUINT16					usDesLen, usLtwOffLowBound, usLtwOffUpBound;
	HBOOL					bBoundValid;
	HUINT16					usTemp;

	HxLOG_Info("parseMultiBufUtilDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown parseMultiBufUtilDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < bound valid flag & LTW offset lower bound (2byte) */
	usTemp = HxMACRO_Get16Bit(p);
	p += 2;
	bBoundValid = (HUINT8)((usTemp & 0x8000) >> 15);
	usLtwOffLowBound = usTemp & 0x7fff;
	HxLOG_Print("\t\t(bound valid flag = 0x%d)\n", bBoundValid);
	HxLOG_Print("\t\t(LTW offset lower bound = 0x%x)\n", usLtwOffLowBound);

	/** < LTW offset upper bound (2byte) */
	usTemp = HxMACRO_Get16Bit(p);
	//p += 2;
	usLtwOffUpBound = usTemp & 0x7fff;
	HxLOG_Print("\t\t(LTW offset upper bound = 0x%x)\n", usLtwOffUpBound);

	/** < Descriptor 데이타 구성. */
	setMultiplexBufUtilDes(pstDes, bBoundValid, usLtwOffLowBound, usLtwOffUpBound);

	HxLOG_Info(" parseMultiBufUtilDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyMultiplexBufUtilDes(SIxMultiplexBufUtilDes_t *pstDest, const SIxMultiplexBufUtilDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMultiplexBufUtilDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_COPYRIGHT (TAG = 0x0d)
 ************************************************************************/
static void initCopyRightDes(SIxCopyRightDes_t *pstDes)
{
	pstDes->ulCopyrightIdent	= 0;
	pstDes->pstPrivateDataInfo	= NULL;
}

static SIxCopyRightDes_t *allocCopyRightDes(void)
{
	return (SIxCopyRightDes_t *)silib_desc_alloc(sizeof(SIxCopyRightDes_t));
}

static void freeCopyRightDes(SIxCopyRightDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_util_freePrivateDataInfo(pstDes->pstPrivateDataInfo);
	silib_desc_free(pstDes);
}

static void setCopyRightDes(SIxCopyRightDes_t *pstDes, HUINT32 ulCopyrightIdent, SIxPrivateDataInfo_t *pstPrivateDataInfo)
{
	pstDes->ulCopyrightIdent	= ulCopyrightIdent;
	pstDes->pstPrivateDataInfo	= pstPrivateDataInfo;
}

static SIxError_e parseCopyRightDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCopyRightDes_t *pstDes)
{
	SIxPrivateDataInfo_t		*pstNewInfo = NULL;
	HUINT16					 usDesLen;
	HUINT32					 ulCopyrightIdent;

	HxLOG_Info("parseCopyDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		HxLOG_Print("[WARN] Unknown parseCopyDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < copyright identifier (4byte); */
	ulCopyrightIdent = HxMACRO_Get32Bit(p);
	p += 4;
	usDesLen -= 4;
	HxLOG_Print("\t\t(copyright identifier = 0x%x)\n", ulCopyrightIdent);

	/** < Private Data */
	if (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		SIxError_e	eErr;
		pstNewInfo = silib_desc_util_allocPrivateDataInfo((HUINT8)usDesLen);
		if (pstNewInfo == NULL)
			goto err_exit;

		eErr = silib_desc_util_setPrivateDataInfo(pstNewInfo, p, (HUINT8)usDesLen);
		if (eSIxERR_OK != eErr)
		{
			silib_desc_util_freePrivateDataInfo(pstNewInfo);
			goto err_exit;
		}
	}

	/** < Descriptor 데이타 구성. */
	setCopyRightDes(pstDes, ulCopyrightIdent, pstNewInfo);

	HxLOG_Info(" parseCopyDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	return eSIxERR_FAIL;
}

static SIxError_e copyCopyRightDes(SIxCopyRightDes_t *pstDest, const SIxCopyRightDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCopyRightDes_t));
		pstDest->pstPrivateDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPrivateDataInfo);
		if (NULL == pstDest->pstPrivateDataInfo)
			goto err_exit;
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_util_freePrivateDataInfo(pstDest->pstPrivateDataInfo);
		pstDest->pstPrivateDataInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_MAXIMUM_BITRATE (TAG = 0x0e)
 ************************************************************************/
static void initMaxBitrateDes(SIxMaxBitrateDes_t *pstDes)
{
	pstDes->ulMaxBitrate	= 0;
}

static SIxMaxBitrateDes_t *allocMaxBitrateDes(void)
{
	return (SIxMaxBitrateDes_t *)silib_desc_alloc(sizeof(SIxMaxBitrateDes_t));
}

static void freeMaxBitrateDes(SIxMaxBitrateDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setMaxBitrateDes(SIxMaxBitrateDes_t *pstDes, HUINT32 ulMaxBitrate)
{
	pstDes->ulMaxBitrate	= ulMaxBitrate;
}

static SIxError_e parseMaxBitrateDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxMaxBitrateDes_t *pstDes)
{
	HUINT16			usDesLen;
	HUINT32			ulMaxBitrate;
	HUINT32			ulTemp;

	HxLOG_Info("parseMaxBitrateDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 3)
	{
		HxLOG_Print("[WARN] Unknown parseMaxBitrateDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < maximum bitrate (3byte); */
	ulTemp = HxMACRO_Get24Bit(p);
	//p += 3;
	ulMaxBitrate = ulTemp & 0x3fffff;
	HxLOG_Print("\t\t(maximum bitrate = %d)\n", ulMaxBitrate);

	/** < Descriptor 데이타 구성. */
	setMaxBitrateDes (pstDes, ulMaxBitrate);

	HxLOG_Info("parseMaxBitrateDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyMaxBitrateDes(SIxMaxBitrateDes_t *pstDest, const SIxMaxBitrateDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMaxBitrateDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_PRIVATE_DATA_INDICATOR (TAG = 0x0f)
 ************************************************************************/
static void initPrivateDataIndicatorDes(SIxPrivateDataIndicatorDes_t *pstDes)
{
	pstDes->ulPrivateDataIndicatorDes = 0;
}

static SIxPrivateDataIndicatorDes_t *allocPrivateDataIndicatorDes(void)
{
	return (SIxPrivateDataIndicatorDes_t *)silib_desc_alloc(sizeof(SIxPrivateDataIndicatorDes_t));
}

static void freePrivateDataIndicatorDes(SIxPrivateDataIndicatorDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setPrivateDataIndicatorDes(SIxPrivateDataIndicatorDes_t *pstDes, HUINT32 ulPrivateDataIndicatorDes)
{
	pstDes->ulPrivateDataIndicatorDes = ulPrivateDataIndicatorDes;
}

static SIxError_e parsePrivateDataIndicatorDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxPrivateDataIndicatorDes_t *pstDes)
{
	HUINT16						usDesLen;
	HUINT32						ulPrivateDataIndicator;

	HxLOG_Info("parsePrivateDataIndiDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown PrivateDataIndicatorDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < private data indicator (4byte); */
	ulPrivateDataIndicator = HxMACRO_Get32Bit(p);
	HxLOG_Print("\t\t(private data indicator = 0x%x)\n", ulPrivateDataIndicator);

	/** < Descriptor 데이타 구성. */
	setPrivateDataIndicatorDes (pstDes, ulPrivateDataIndicator);

	HxLOG_Info("parsePrivateDataIndiDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyPrivateDataIndicatorDes(SIxPrivateDataIndicatorDes_t *pstDest, const SIxPrivateDataIndicatorDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxPrivateDataIndicatorDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_SMOOTHING_BUFFER (TAG = 0x10)
 ************************************************************************/
static void initSmoothBufDes(SIxSmoothBufDes_t *pstDes)
{
	pstDes->ulLeakRate 	= 0;
	pstDes->ulSize 		= 0;
}

static SIxSmoothBufDes_t *allocSmoothBufDes(void)
{
	return (SIxSmoothBufDes_t *)silib_desc_alloc(sizeof(SIxSmoothBufDes_t));
}

static void freeSmoothBufDes(SIxSmoothBufDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setSmoothBufDes(SIxSmoothBufDes_t *pstDes, HUINT32 ulLeakRate, HUINT32 ulSize)
{
	pstDes->ulLeakRate 	= ulLeakRate;
	pstDes->ulSize 		= ulSize;
}

static SIxError_e parseSmoothBufDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSmoothBufDes_t *pstDes)
{
	HUINT16			usDesLen;
	HUINT32			ulLeakRate, ulSize;
	HUINT32			ulTemp;

	HxLOG_Info("parseSmoothBufDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 6)
	{
		HxLOG_Print("[WARN] Unknown parseSmoothBufDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < sb leak rate (3byte); */
	ulTemp = HxMACRO_Get24Bit(p);
	p += 3;
	ulLeakRate = ulTemp & 0x3fffff;
	HxLOG_Print("\t\t(sb leak rate = 0x%x)\n", ulLeakRate);

	/** < sb size (3byte); */
	ulTemp = HxMACRO_Get24Bit(p);
	ulSize = ulTemp & 0x3fffff;
	HxLOG_Print("\t(sb size = 0x%x)\n", ulSize);

	/** < Descriptor 데이타 구성. */
	setSmoothBufDes(pstDes, ulLeakRate, ulSize);

	HxLOG_Info(" parseSmoothBufDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copySmoothBufDes(SIxSmoothBufDes_t *pstDest, const SIxSmoothBufDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSmoothBufDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_STD (TAG = 0x11)
 ************************************************************************/
static void initStdDes(SIxStdDes_t *pstDes)
{
	pstDes->bLeakValid	= 0;
}

static SIxStdDes_t *allocStdDes(void)
{
	return (SIxStdDes_t *)silib_desc_alloc(sizeof(SIxStdDes_t));
}

static void freeStdDes(SIxStdDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setStdDes(SIxStdDes_t *pstDes, HBOOL bLeakValid)
{
	pstDes->bLeakValid	= bLeakValid;
}

static SIxError_e parseStdDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxStdDes_t *pstDes)
{
	HBOOL		bLeakValid;
	HUINT8		ucTemp, ucVal;
	HUINT16		usDesLen;

	HxLOG_Info("parseStdDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown parseStdDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < leak valid flag (1byte); */
	ucTemp = *p;
	ucVal = ucTemp & 0x01;
	(ucVal == 1) ? (bLeakValid = TRUE) : (bLeakValid = FALSE);
	HxLOG_Print("\t\t(leak valid flag = 0x%d)\n", bLeakValid);

	/** < Descriptor 데이타 구성. */
	setStdDes(pstDes, bLeakValid);

	HxLOG_Info(" parseStdDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyStdDes(SIxStdDes_t *pstDest, const SIxStdDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxStdDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_CAROUSEL_ID  (TAG = 0x13)
 ************************************************************************/
static void initCarouselIdDesInfo(SIxCarouselIdDesInfo_t *pstInfo)
{
	pstInfo->ucModuleVersion = 0;
	pstInfo->usModuleId = 0;
	pstInfo->usBlockSize = 0;
	pstInfo->ulModuleSize = 0;
	pstInfo->ucCompressionMethod = 0;
	pstInfo->ulOriginalSize = 0;
	pstInfo->ucTimeout = 0;
	HxSTD_MemSet(pstInfo->aucObjectKeyData, 0, 4);
}

static SIxCarouselIdDesInfo_t *allocCarouselIdDesInfo(void)
{
	SIxCarouselIdDesInfo_t *pstInfo;

	pstInfo = (SIxCarouselIdDesInfo_t *)silib_desc_alloc(sizeof(SIxCarouselIdDesInfo_t));
	if (pstInfo)
	{
		initCarouselIdDesInfo(pstInfo);
	}

	return pstInfo;
}

static void freeCarouselIdDesInfo(SIxCarouselIdDesInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void initCarouselIdDes(SIxCarouselIdDes_t *pstDes)
{
	pstDes->ulCarouselId = 0;
	pstDes->ucFormatId = 0;
	pstDes->pstInfo = NULL;
}

static SIxCarouselIdDes_t *allocCarouselIdDes(void)
{
	return (SIxCarouselIdDes_t *)silib_desc_alloc(sizeof(SIxCarouselIdDes_t));
}

static void freeCarouselIdDes(SIxCarouselIdDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	freeCarouselIdDesInfo(pstDes->pstInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseCarouselIdDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCarouselIdDes_t *pstDes)
{
	HUINT8					ucDesLen;
	HUINT32					ulCarouselId;
	HUINT8					ucFormatId;
	SIxCarouselIdDesInfo_t		*pstInfo;

	HxLOG_Info("parseCarouselIdDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	/** < carousel_id */
	ulCarouselId = HxMACRO_Get32Bit(p);
	p += 4; ucDesLen -= 4;

	/** < format_id */
	ucFormatId = *p++; ucDesLen--;

	/** < if ucFormatId == 0x01, parse deeply... */
	if (ucFormatId == 0x01)
	{
		HUINT8 ucTemp;

		pstInfo = allocCarouselIdDesInfo();
		if (pstInfo == NULL)
		{
			/** < hwlee : memory allocation error : back to normal boot */
			pstDes->pstInfo = NULL;
			pstDes->ucFormatId = 0x00;
			return eSIxERR_OK;
		}

		pstInfo->ucModuleVersion = *p;
		p++; ucDesLen--;

		pstInfo->usModuleId = HxMACRO_Get16Bit(p);
		p += 2; ucDesLen -= 2;

		pstInfo->usBlockSize = HxMACRO_Get16Bit(p);
		p += 2; ucDesLen -= 2;

		pstInfo->ulModuleSize = HxMACRO_Get32Bit(p);
		p += 4; ucDesLen -= 4;

		pstInfo->ucCompressionMethod = *p;
		p++; ucDesLen--;

		pstInfo->ulOriginalSize = HxMACRO_Get32Bit(p);
		p += 4; ucDesLen -= 4;

		pstInfo->ucTimeout = *p;
		p++; ucDesLen--;

		/** < hwlee : store ucObjectKeyDataLen to ucTemp */
		ucTemp = *p;
		p++;ucDesLen--;

		if (ucTemp <= 4)
		{
			HxSTD_MemCopy(pstInfo->aucObjectKeyData
					, p
					, 4);
			//p += 4;ucDesLen -= 4;
		}
		else
		{
			silib_desc_freeInfo_Dvb(eSIxDESCTAG_CAROUSEL_ID, 0, (HUINT8 *)pstInfo);
			pstDes->ucFormatId = 0x00;
			pstDes->pstInfo = NULL;
			return eSIxERR_OK;
		}
	}
	else
	{
		pstInfo = NULL;
	}

	/** < assign parsing result */
	pstDes->ulCarouselId = ulCarouselId;
	pstDes->ucFormatId = ucFormatId;
	pstDes->pstInfo = pstInfo;
	HxLOG_Info(" parseCarouselIdDes() Leaved!");
	return eSIxERR_OK;
}

static SIxError_e copyCarouselIdDes(SIxCarouselIdDes_t *pstDest, const SIxCarouselIdDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCarouselIdDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			eErr = eSIxERR_FAIL;
			pstDest->pstInfo = allocCarouselIdDesInfo();
			if (pstDest->pstInfo)
			{
				HxSTD_MemCopy(pstDest->pstInfo, pstSource->pstInfo, sizeof(SIxCarouselIdDesInfo_t));
				eErr = eSIxERR_OK;
			}
		}
		if (eSIxERR_OK != eErr)
			goto err_exit;
	}

	return eErr;

err_exit:
	freeCarouselIdDesInfo(pstDest->pstInfo);
	pstDest->pstInfo = NULL;
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_ASSOCIATION_TAG (TAG = 0x14)
 ************************************************************************/
static void initAssocTagDes(SIxAssocTagDes_t *pstDes)
{
	pstDes->usAssocTag = 0;
	pstDes->usUse = 0;
	pstDes->ulTransactionId = 0;
	pstDes->ulTimeout = 0;
}


static SIxAssocTagDes_t *allocAssocTagDes(void)
{
	return (SIxAssocTagDes_t *)silib_desc_alloc(sizeof(SIxAssocTagDes_t));
}

static void freeAssocTagDes(SIxAssocTagDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseAssocTagDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxAssocTagDes_t *pstDes)
{
	HUINT8					ucDesLen, ucSelectorLen;
	HUINT16					usAssocTag;
	HUINT16					usUse;
	HUINT32					ulTransactionId, ulTimeout;

	HxLOG_Info("parseAssocTagDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseAssocTagDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < assoc_tag */
	usAssocTag = HxMACRO_Get16Bit(p);
	p += 2;	ucDesLen -= 2;

	/** < use */
	usUse = HxMACRO_Get16Bit(p);
	p += 2;	ucDesLen -= 2;

	/** < selector length */
	ucSelectorLen = *p++; ucDesLen--;
	HxLOG_Print("\t\t(selector length = %d)\n", ucSelectorLen);

	if (usUse == 0x0000)
	{
		/** < transaction_id */
		ulTransactionId = HxMACRO_Get32Bit(p);
		p += 4; ucDesLen -= 4;
		/** < timeout */
		ulTimeout = HxMACRO_Get32Bit(p);
		//p += 4; ucDesLen -= 4;
	}
	else
	{
		/** < if usUse != 0x0000, ignore selector_byte */
		ulTransactionId = ulTimeout = (HUINT32)-1;
	}

	/** < assign parsing results */
	pstDes->usAssocTag = usAssocTag;
	pstDes->usUse = usUse;
	pstDes->ulTransactionId = ulTransactionId;
	pstDes->ulTimeout = ulTimeout;

	HxLOG_Info(" parseAssocTagDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyAssocTagDes(SIxAssocTagDes_t *pstDest, const SIxAssocTagDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAssocTagDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG (TAG = 0x15)
 ************************************************************************/
static void initDeferAssocTagDes(SIxDeferAssocTagDes_t *pstDes)
{
	pstDes->ucAssocTagSize 			= 0;
	pstDes->ausAssocTags 			= NULL;
	pstDes->usTransportStreamId 		= 0;
	pstDes->usProgNum 				= 0;
	pstDes->usOriginalNetworkId	 	= 0;
}

static SIxDeferAssocTagDes_t *allocDeferAssocTagDes(void)
{
	return (SIxDeferAssocTagDes_t *)silib_desc_alloc(sizeof(SIxDeferAssocTagDes_t));
}

static void freeDeferAssocTagDes(SIxDeferAssocTagDes_t *pstDes)
{
	if (NULL != pstDes)
	{
		if (NULL != pstDes->ausAssocTags)
		{
			silib_desc_free(pstDes->ausAssocTags);
		}

		silib_desc_free(pstDes);
	}
}

static SIxError_e parseDeferAssocTagDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxDeferAssocTagDes_t *pstDes)
{
	HUINT8					ucDesLen;
	HUINT8					ucAssocTagLen = 0;
	HUINT16					*ausAssocTags = NULL;
	HUINT16					on_id, ts_id, prog_num;

	HxLOG_Info("parseDeferAssocTagDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseDeferAssocTagDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < assoc_tag_length */
	ucAssocTagLen = *p;
	p++; ucDesLen--;

	/** < alloc memory for assoc_tag array */
	if (ucAssocTagLen > 0)
	{
		HUINT8 i;

		ausAssocTags = (HUINT16 *)silib_desc_alloc(ucAssocTagLen);

		/** < handling unErr : NIY */
		for(i = 0; i < ucAssocTagLen/2; i++)
		{
			ausAssocTags[i] = HxMACRO_Get16Bit(p);
			p += 2; ucDesLen -= 2;
		}
	}

	ts_id = HxMACRO_Get16Bit(p);
	p += 2; ucDesLen -= 2;

	prog_num = HxMACRO_Get16Bit(p);
	p += 2; ucDesLen -= 2;

	on_id = HxMACRO_Get16Bit(p);
	//p += 2; ucDesLen -= 2;

	/** < assign parsing results */
	pstDes->ucAssocTagSize = (HUINT8)(ucAssocTagLen/2);
	pstDes->ausAssocTags = ausAssocTags;
	pstDes->usTransportStreamId = ts_id;
	pstDes->usProgNum = prog_num;
	pstDes->usOriginalNetworkId = on_id;

	HxLOG_Info(" parseDeferAssocTagDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyDeferAssocTagDes(SIxDeferAssocTagDes_t *pstDest, const SIxDeferAssocTagDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDeferAssocTagDes_t));
		pstDest->ucAssocTagSize = 0;
		pstDest->ausAssocTags = NULL;
		eErr = eSIxERR_OK;

		// Deep Copy
		if (pstSource->ucAssocTagSize > 0 && NULL != pstSource->ausAssocTags)
		{
			eErr = eSIxERR_FAIL;
			pstDest->ausAssocTags = silib_desc_alloc(sizeof(HUINT16)*pstSource->ucAssocTagSize);
			if (pstDest->ausAssocTags)
			{
				HxSTD_MemCopy(pstDest->ausAssocTags, pstSource->ausAssocTags, sizeof(HUINT16)*pstSource->ucAssocTagSize);
				pstDest->ucAssocTagSize = pstSource->ucAssocTagSize;
				eErr = eSIxERR_OK;
			}
		}
	}

	return eErr;
#if 0
err_exit:
	silib_desc_free(pstDest->ausAssocTags);
	pstDest->ausAssocTags = NULL;
	pstDest->ucAssocTagSize = 0;
	return eSIxERR_FAIL;
#endif
}

/************************************************************************
 * eSIxDESCTAG_AVC_VIDEO (TAG = 0x28)
 ************************************************************************/
static void initAvcVideoDes(SIxAvcVideoDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxAvcVideoDes_t));
}

static SIxAvcVideoDes_t *allocAvcVideoDes(void)
{
	return (SIxAvcVideoDes_t *)silib_desc_alloc(sizeof(SIxAvcVideoDes_t));
}

static void freeAvcVideoDes(SIxAvcVideoDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseAvcVideoDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxAvcVideoDes_t *pstDes)
{
	HUINT8			ucDesLen;
	HUINT8			ucTemp;

	HxLOG_Info("parseAvcVideoDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseAvcVideoDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < Profile idc (1byte) */
	pstDes->ucProfile = *p++;
	HxLOG_Print("\t\t(profile idc = 0x%x)\n", pstDes->ucProfile);

	/** < constraint_set_flag & AVC_compatible_flags (1byte) */
	ucTemp = *p++;
	pstDes->bConstSet0Flag 		= (HBOOL)((ucTemp & 0x80) >> 7);
	pstDes->bConstSet1Flag 		= (HBOOL)((ucTemp & 0x40) >> 6);
	pstDes->bConstSet2Flag 		= (HBOOL)((ucTemp & 0x20) >> 5);
	pstDes->ucAvcCompatibleFlags 	= (HUINT8)((ucTemp & 0x1f));
	HxLOG_Print("\t\t(constraint_set0_flag = 0x%x)\n", pstDes->bConstSet0Flag);
	HxLOG_Print("\t\t(constraint_set1_flag = 0x%x)\n", pstDes->bConstSet1Flag);
	HxLOG_Print("\t\t(constraint_set2_flag = 0x%x)\n", pstDes->bConstSet1Flag);
	HxLOG_Print("\t\t(AVC_compatible_flags = 0x%x)\n", pstDes->ucAvcCompatibleFlags);

	/** < level_idc */
	pstDes->ucLevel		= *p++;
	HxLOG_Print("\t\t(level idc = 0x%x)\n", pstDes->ucLevel);

	/** < AVC_still_present & AVC_24_hour_picture_flag (1byte) */
	ucTemp = *p++;
	pstDes->ucAvcStillPresent	= (HUINT8)((ucTemp & 0x80) >> 7);
	pstDes->bAvc24hourPicFlag	= (HBOOL)((ucTemp & 0x40) >> 6);
	HxLOG_Print("\t\t(Avc_still_present = 0x%x)\n", pstDes->ucAvcStillPresent);
	HxLOG_Print("\t\t(AVC_24Hour_picture_flag = 0x%x)\n", pstDes->bAvc24hourPicFlag);

	HxLOG_Info(" parseAvcVideoDes() Leaved!");

	return eSIxERR_OK;

}

static SIxError_e copyAvcVideoDes(SIxAvcVideoDes_t *pstDest, const SIxAvcVideoDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAvcVideoDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_NETWORK_NAME (TAG = 0x40)
 ************************************************************************/
static void initNetNameDes(SIxNetNameDes_t *pstDes)
{
	pstDes->pszNetName = NULL;
}

static SIxNetNameDes_t *allocNetNameDes(void)
{
	return (SIxNetNameDes_t *)silib_desc_alloc(sizeof(SIxNetNameDes_t));
}

static void freeNetNameDes(SIxNetNameDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_free(pstDes->pszNetName);
	silib_desc_free(pstDes);
}

static void setNetNameDes(SIxNetNameDes_t *pstDes, HUINT8 *pszNetName)
{
	if(pszNetName != NULL)
		HxSTD_MemCopy(pstDes->pszNetName, pszNetName, silib_desc_util_dvbStrlen(pszNetName));
	pstDes->pszNetName[silib_desc_util_dvbStrlen(pszNetName)] = '\0';
}

static SIxError_e parseNetNameDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxNetNameDes_t *pstDes)
{
	HUINT8			*pszNetName = NULL;
	HUINT16			usDesLen;
	HUINT32			ulTempNetNameLen, ulAssignedNetNameLen;

	HxLOG_Info("parseNetNameDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen == 0)
		goto err_exit;

	/** < Max allocation limit & allocation */
	ulAssignedNetNameLen = (usDesLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usDesLen);

	/** < network name */
	ulTempNetNameLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, ulAssignedNetNameLen, (const HUINT8 *)p, &pszNetName);
	HxLOG_Print("\t\t(network name = %s)\n", (char *)pszNetName);

	/** < Descriptor 데이타 구성. */
	pstDes->pszNetName = silib_desc_util_allocStr(ulTempNetNameLen+1);
	if (pstDes->pszNetName == NULL)
		goto err_exit;

	setNetNameDes(pstDes, pszNetName);
	silib_desc_free(pszNetName);

	HxLOG_Info(" parseNetNameDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszNetName);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyNetNameDes(SIxNetNameDes_t *pstDest, const SIxNetNameDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxNetNameDes_t));
		pstDest->pszNetName = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pszNetName)
		{
			HUINT32	ulSize = silib_desc_util_dvbStrlen(pstSource->pszNetName);

			eErr = eSIxERR_FAIL;
			if (ulSize > 0)
			{
				pstDest->pszNetName = silib_desc_util_allocStr(ulSize+1);
				if (pstDest->pszNetName)
				{
					HxSTD_MemCopy(pstDest->pszNetName, pstSource->pszNetName, ulSize);
					pstDest->pszNetName[ulSize] = '\0';
					eErr = eSIxERR_OK;
				}
			}
		}
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_SERVICE_LIST (TAG = 0x41)
 ************************************************************************/
static void initSvcListInfo(SIxSvcListInfo_t *pstInfo)
{
	pstInfo->usSvcId 		= 0;
	pstInfo->ucServType 	= 0;
	pstInfo->pstNext 		= NULL;
}

static SIxSvcListInfo_t *allocSvcListInfo(void)
{
	SIxSvcListInfo_t	*pstInfo;

	pstInfo = (SIxSvcListInfo_t *)silib_desc_alloc(sizeof(SIxSvcListInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocSvcListInfo()\n");
		return NULL;
	}
	initSvcListInfo(pstInfo);

	return ((SIxSvcListInfo_t *)pstInfo);
}

static void freeSvcListInfo(SIxSvcListInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setSvcListInfo(SIxSvcListInfo_t *pstInfo, HUINT16 usServId, HUINT8 ucServType)
{
	pstInfo->usSvcId 		= usServId;
	pstInfo->ucServType		= ucServType;
}

static void initSvcListDes(SIxSvcListDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxSvcListDes_t *allocSvcListDes(HUINT32 ulSize)
{
	SIxSvcListDes_t	*pstDes;

	pstDes = (SIxSvcListDes_t *)silib_desc_alloc(ulSize);
	if (pstDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocSvcListDes()\n");
		return NULL;
	}
	initSvcListDes(pstDes);

	return ((SIxSvcListDes_t *)pstDes);
}

static void freeSvcListDes(SIxSvcListDes_t *pstDes)
{
	SIxSvcListInfo_t	*pIndexSvcListInfo, *pDestSvcListInfo;

	if (NULL == pstDes)		return;

	pIndexSvcListInfo = pstDes->pstInfo;
	while (pIndexSvcListInfo)
	{
		pDestSvcListInfo = pIndexSvcListInfo;
		pIndexSvcListInfo = pIndexSvcListInfo->pstNext;
		freeSvcListInfo(pDestSvcListInfo);
	}

	silib_desc_free(pstDes);
}

static SIxError_e parseSvcListDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSvcListDes_t *pstDes)
{
	SIxSvcListInfo_t	*pstNewInfo = NULL, *pstPrevInfo = NULL;
	HUINT16			usDesLen;
	HUINT16			usSvcId;
	HUINT8			ucServType;

	HxLOG_Info("parseSvcListDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 3) != 0)
	{
		HxLOG_Print("[WARN] Unknown SvcListDes Format!\n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < service id (2byte) */
		usSvcId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t\t(service id = 0x%x)\n", usSvcId);

		/** < service type (1byte) */
		ucServType = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(servie type = 0x%x)\n", ucServType);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxSvcListInfo_t, pstDes);

		pstNewInfo = allocSvcListInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setSvcListInfo(pstNewInfo, usSvcId, ucServType);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseSvcListDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copySvcListDes(SIxSvcListDes_t *pstDest, const SIxSvcListDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcListDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		// Deep Copy
		if (pstSource->pstInfo)
		{
			SIxSvcListInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxSvcListInfo_t, pstDest);

				pstNewInfo = allocSvcListInfo();
				if (pstNewInfo == NULL)
					goto err_exit;

				setSvcListInfo(pstNewInfo, pstSourceInfo->usSvcId, pstSourceInfo->ucServType);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxSvcListInfo_t, freeSvcListInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_STUFFING (TAG = 0x42)
 ************************************************************************/
//Stuffing
#if 0
static void initStuffingDes(SIxStuffingDes_t *pStuffingDes)
{
}

static SIxStuffingDes_t *allocStuffingDes(HUINT32 ulSize)
{
	SIxStuffingDes_t	*pStuffingDes;

	pStuffingDes = (SIxStuffingDes_t *)silib_desc_alloc(ulSize);
	if (pStuffingDes == NULL)
	{
		HxLOG_Print("[ERROR] %s()\n", __FUNCTION__);
		return NULL;
	}
	initStuffingDes(pStuffingDes);

	return ((SIxStuffingDes_t *)pStuffingDes);
}

static void freeStuffingDes(SIxStuffingDes_t *pStuffingDes)
{
}

static SIxError_e parseStuffingDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxStuffingDes_t *pstDes)
{
}

static SIxError_e copyStuffingDes(SIxStuffingDes_t *pstDest, const SIxStuffingDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxStuffingDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}
#endif

/************************************************************************
 * eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM (TAG = 0x43)
 ************************************************************************/
static void initSatDeliSystemDes(SIxSatDeliSystemDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxSatDeliSystemDes_t));
	pstDes->ulFreq 		= 0;
	pstDes->usOrbitPos 	= 0;
	pstDes->bWestEastFlag 	= FALSE;
	pstDes->ucPolar 		= 0;
	pstDes->ucModul 		= 0;
	pstDes->ulSymbolRate 	= 0;
	pstDes->ucFECInner 	= 0;
	/* DVB S2 */
	pstDes->ucTransSpec	= 0;
	pstDes->ucRollOff		= 0;
}

static SIxSatDeliSystemDes_t *allocSatDeliSystemDes(void)
{
	return (SIxSatDeliSystemDes_t *)silib_desc_alloc(sizeof(SIxSatDeliSystemDes_t));
}

static void freeSatDeliSystemDes(SIxSatDeliSystemDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setSatDeliSystemDes(SIxSatDeliSystemDes_t *pstDes, HUINT32 ulFreq, HUINT16 usOrbitPos, HBOOL bWestEastFlag, HUINT8 ucPolar, HUINT8 ucModul, HUINT8 ucModType, HUINT32 ulSymbolRate, HUINT8 ucFECInner, HUINT8 ucTransSpec, HUINT8 ucRollOff)
{
	pstDes->ulFreq 			= ulFreq;
	pstDes->usOrbitPos 		= usOrbitPos;
	pstDes->bWestEastFlag 	= bWestEastFlag;
	pstDes->ucPolar 		= ucPolar;
	pstDes->ucModul 		= ucModul;
	pstDes->ucModType		= ucModType;
	pstDes->ulSymbolRate 	= ulSymbolRate;
	pstDes->ucFECInner 		= ucFECInner;
	/* DVB S2 */
	pstDes->ucTransSpec 	= ucTransSpec;
	pstDes->ucRollOff		= ucRollOff;
}

static SIxError_e parseSatDeliSystemDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSatDeliSystemDes_t *pstDes)
{
	HUINT8				ucPolar, ucModul, ucModType, ucFECInner = 0;
	HUINT16				usDesLen;
	HUINT16				usOrbitPos;
	HUINT32				ulFreq, ulSymbolRate;
	HBOOL				bWestEastFlag;
	HUINT8				ucTemp, ucVal;
	HUINT32				ulTemp;
	HUINT8				ucTransSpec;
	HUINT8 				ucRollOff = 0;


	HxLOG_Info("parseSatDeliSystemDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 11)
	{
		HxLOG_Print("[WARN] Unknown parseSatDeliSystemDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < frequency (4byte) */
	ulFreq = (HUINT32)silib_desc_util_bcdToBinary(HxMACRO_Get32Bit(p));
	p += 4;
	HxLOG_Print("\t\t(frequency = %d)\n", ulFreq);

	/** < orbital position (2byte) */
	usOrbitPos = (HUINT16)silib_desc_util_bcdToBinary(HxMACRO_Get16Bit(p));
	p += 2;
	HxLOG_Print("\t\t(orbital position = %d)\n", usOrbitPos);

	/** < west east flag & polarization & modulation (1byte) */
	ucTemp = *p++;
	ucVal = (HUINT8)((ucTemp & 0x80) >> 7);					// west_east_flag
	bWestEastFlag = (ucVal == 1) ? TRUE : FALSE;

	ucPolar = (HUINT8)((ucTemp & 0x60) >> 5);				// polarization

	/* DVB S2 */
	ucModul = ucTemp & 0x03;
	HxLOG_Print("\t\t(west east flag = 0x%d)\n", bWestEastFlag);
	HxLOG_Print("\t\t(polarization = 0x%x)\n", ucPolar);
	HxLOG_Print("\t\t(modulation = 0x%x)\n", ucModul);

	ucTransSpec = ((ucTemp & 0x04) != 0) ? 1 : 0;
	if (ucTransSpec == 1)
	{
		ucRollOff = (HUINT8)((ucTemp & 0x18) >> 3);
	}
	else
	{
		ucRollOff = 0x00;
	}

	ucModType = ucTemp & 0x03;

	HxLOG_Print("\t\t(trans spec = 0x%x)\n", ucTransSpec);
	HxLOG_Print("\t\t(roll off = 0x%x)\n", ucRollOff);


	/** < symbol rate & FEC inner (4byte) */
	ulTemp = HxMACRO_Get32Bit(p);
	ulSymbolRate = (HUINT32)silib_desc_util_bcdToBinary((ulTemp & 0xfffffff0) >> 4);
	ucFECInner = (HUINT8)(ulTemp & 0x0000000f);
	HxLOG_Print("\t\t(symbol rate = %d)\n", ulSymbolRate);
	HxLOG_Print("\t\t(FEC inner = 0x%x)\n", ucFECInner);

	/** < Descriptor 데이타 구성. */
	setSatDeliSystemDes(pstDes, ulFreq, usOrbitPos, bWestEastFlag, ucPolar, ucModul, ucModType,
						ulSymbolRate, ucFECInner, ucTransSpec, ucRollOff);

	HxLOG_Info(" parseSatDeliSystemDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copySatDeliSystemDes(SIxSatDeliSystemDes_t *pstDest, const SIxSatDeliSystemDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSatDeliSystemDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_CABLE_DELIVERY_SYSTEM (TAG = 0x44)
 ************************************************************************/
static void initCabDeliSystemDes(SIxCabDeliSystemDes_t *pstDes)
{
	pstDes->ulFreq 		= 0;
	pstDes->ucFECOuter	= 0;
	pstDes->ucModul 		= 0;
	pstDes->ulSymbolRate = 0;
	pstDes->ucFECInner 	= 0;
}

static SIxCabDeliSystemDes_t *allocCabDeliSystemDes(void)
{
	return (SIxCabDeliSystemDes_t *)silib_desc_alloc(sizeof(SIxCabDeliSystemDes_t));
}

static void freeCabDeliSystemDes(SIxCabDeliSystemDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setCabDeliSystemDes(SIxCabDeliSystemDes_t *pstDes, HUINT32 ulFreq, HUINT8 ucFECOuter, HUINT8 ucModul, HUINT32 ulSymbolRate, HUINT8 ucFECInner)
{
	pstDes->ulFreq 		= ulFreq;
	pstDes->ucFECOuter 	= ucFECOuter;
	pstDes->ucModul 		= ucModul;
	pstDes->ulSymbolRate = ulSymbolRate;
	pstDes->ucFECInner 	= ucFECInner;
}

static SIxError_e parseCabDeliSystemDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCabDeliSystemDes_t *pstDes)
{
	HUINT8				ucFECOuter, ucModul, ucFECInner;
	HUINT16				usDesLen;
	HUINT32				ulFreq, ulSymbolRate;
	HUINT32				ulTemp;

	HxLOG_Info("parseCabDeliSystemDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 11)
	{
		HxLOG_Print("[WARN] Unknown parseCabDeliSystemDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < frequency (4byte) */
	ulFreq = (HUINT32)silib_desc_util_bcdToBinary(HxMACRO_Get32Bit(p));
	p += 4;
	HxLOG_Print("\t\t(frequency = %d)\n", ulFreq/10);

	/** < reserved & FEC outer (2byte) */
	ucFECOuter = (HUINT8)silib_desc_util_bcdToBinary(HxMACRO_Get16Bit(p) & 0x000f);
	p += 2;
	HxLOG_Print("\t\t(FEC outer = 0x%x)\n", ucFECOuter);

	/** < modulation (1byte) */
	ucModul = *p++;
	HxLOG_Print("\t\t(modulation = 0x%x)\n", ucModul);

	/** < symbol rate & FEC inner (4byte) */
	ulTemp = HxMACRO_Get32Bit(p);
	ulSymbolRate = (HUINT32)silib_desc_util_bcdToBinary((ulTemp & 0xfffffff0) >> 4);
	ucFECInner = (HUINT8)(ulTemp & 0x0000000f);
	HxLOG_Print("\t\t(symbol rate = %d)\n", ulSymbolRate/10);
	HxLOG_Print("\t\t(FEC inner = 0x%x)\n", ucFECInner);

	/** < Descriptor 데이타 구성. */
	setCabDeliSystemDes(pstDes, ulFreq, ucFECOuter, ucModul, ulSymbolRate, ucFECInner);

	HxLOG_Info(" parseCabDeliSystemDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyCabDeliSystemDes(SIxCabDeliSystemDes_t *pstDest, const SIxCabDeliSystemDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCabDeliSystemDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_BOUQUET_NAME (TAG = 0x47)
 ************************************************************************/
static void initBouqNameDes(SIxBouqNameDes_t *pstDes)
{
	pstDes->pszBouqName = NULL;
}

static SIxBouqNameDes_t *allocBouqNameDes(void)
{
	return (SIxBouqNameDes_t *)silib_desc_alloc(sizeof(SIxBouqNameDes_t));
}

static void freeBouqNameDes(SIxBouqNameDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_free(pstDes->pszBouqName);
	silib_desc_free(pstDes);
}

static void setBouqNameDes(SIxBouqNameDes_t *pstDes, HUINT8 *pszBouqName)
{
    if(pszBouqName != NULL)
    	HxSTD_MemCopy(pstDes->pszBouqName, pszBouqName, silib_desc_util_dvbStrlen(pszBouqName));
	pstDes->pszBouqName[silib_desc_util_dvbStrlen(pszBouqName)] = '\0';
}

static SIxError_e parseBouqNameDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxBouqNameDes_t *pstDes)
{
	HUINT8			*pszBouqName = NULL;
	HUINT16			 usDesLen;
	HUINT32			 ulTempBouqNameLen, ulAssignedBouqNameLen;

	HxLOG_Info("parseBouqNameDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen == 0)
		goto err_exit;

	ulAssignedBouqNameLen = (usDesLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usDesLen);

	/** < bouquet name */
	ulTempBouqNameLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, ulAssignedBouqNameLen, (const HUINT8 *)p, &pszBouqName);
	HxLOG_Print("\t\t(bouquet name = %s)\n", (char *)pszBouqName);

	/** < Descriptor 데이타 구성. */
	pstDes->pszBouqName = silib_desc_util_allocStr(ulTempBouqNameLen+1);
	if (pstDes->pszBouqName == NULL)
		goto err_exit;

	setBouqNameDes(pstDes, pszBouqName);
	silib_desc_free(pszBouqName);

	HxLOG_Info(" parseBouqNameDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszBouqName);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyBouqNameDes(SIxBouqNameDes_t *pstDest, const SIxBouqNameDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxBouqNameDes_t));
		pstDest->pszBouqName = NULL;
		eErr = eSIxERR_OK;

		// Deep Copy
		if (pstSource->pszBouqName)
		{
			HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSource->pszBouqName);

			eErr = eSIxERR_FAIL;
			if (ulSize > 0)
			{
				pstDest->pszBouqName = silib_desc_util_allocStr(ulSize+1);
				if (pstDest->pszBouqName)
				{
					HxSTD_MemCopy(pstDest->pszBouqName, pstSource->pszBouqName, ulSize);
					pstDest->pszBouqName[ulSize] = '\0';
					eErr = eSIxERR_OK;
				}
			}
		}
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_SERVICE (TAG = 0x48)
 ************************************************************************/
static void initSvcDes(SIxSvcDes_t *pstDes)
{
	pstDes->ucServType 		= 0;
    pstDes->pszProviderName 	= NULL;
    pstDes->pszServName		= NULL;
}

static SIxSvcDes_t *allocSvcDes(void)
{
	return (SIxSvcDes_t *)silib_desc_alloc(sizeof(SIxSvcDes_t));
}

static void freeSvcDes(SIxSvcDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_free(pstDes->pszProviderName);
	silib_desc_free(pstDes->pszServName);
	silib_desc_free(pstDes);
}

static void setSvcDes(SIxSvcDes_t *pstDes, HUINT8 ucServType, HUINT8 *pszProviderName, HUINT8 *pszServName)
{
	pstDes->ucServType = ucServType;
	if(pszProviderName != NULL)
	    HxSTD_MemCopy(pstDes->pszProviderName, pszProviderName, silib_desc_util_dvbStrlen(pszProviderName));
	pstDes->pszProviderName[silib_desc_util_dvbStrlen(pszProviderName)] = '\0';
	if(pszServName != NULL)
	    HxSTD_MemCopy(pstDes->pszServName, pszServName, silib_desc_util_dvbStrlen(pszServName));
	pstDes->pszServName[silib_desc_util_dvbStrlen(pszServName)] = '\0';
}

static SIxError_e parseSvcDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSvcDes_t *pstDes)
{
	HUINT8		ucServType;
	HUINT8	 	ucProviderNameLen, ucServNameLen;
	HUINT8		*pszProviderName = NULL, *pszServName =NULL;
	HUINT16		usDesLen;
	HUINT32		nTempProviderNameLen, nTempServNameLen;

	HxLOG_Info("parseSvcDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen == 0)
		goto err_exit;

	/** < service type (1byte) */
	ucServType = *p++;
	HxLOG_Print("\t\t(service type = 0x%x)\n", ucServType);

	/** < service provider name length (1byte) */
	ucProviderNameLen = *p++;
	HxLOG_Print("\t\t(service provider name length = %d)\n", ucProviderNameLen);

	/** < provider name */
	nTempProviderNameLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucProviderNameLen, (const HUINT8 *)p, &pszProviderName);
	p += ucProviderNameLen;
	HxLOG_Print("\t\t(service provider name = %s)\n", (char *)pszProviderName);

	/** < service name length (1byte) */
	ucServNameLen = *p++;
	HxLOG_Print("\t\t(service name length = %d)\n", ucServNameLen);

	/** < service name */
	nTempServNameLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucServNameLen, (const HUINT8 *)p, &pszServName);
	HxLOG_Print("\t\t(service name = %s)\n", (char *)pszServName);

	/** < Descriptor 데이타 구성. */
	pstDes->pszProviderName = silib_desc_util_allocStr(nTempProviderNameLen+1);
	if (pstDes->pszProviderName == NULL)
		goto err_exit;
	pstDes->pszServName = silib_desc_util_allocStr(nTempServNameLen+1);
	if (pstDes->pszServName == NULL)
		goto err_exit;
	setSvcDes(pstDes, ucServType, pszProviderName, pszServName);
	silib_desc_free(pszProviderName);
	silib_desc_free(pszServName);

	pstDes->ucPrvName = (HUINT8)nTempProviderNameLen + 1;
	pstDes->ucSvcName = (HUINT8)nTempServNameLen + 1;

	HxLOG_Info(" parseSvcDes() Leaved!\n");

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszProviderName);
	silib_desc_free(pszServName);
	silib_desc_free(pstDes->pszProviderName);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copySvcDes(SIxSvcDes_t *pstDest, const SIxSvcDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HUINT32 ulSize;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcDes_t));
		pstDest->pszProviderName = NULL;
		pstDest->pszServName = NULL;
		eErr = eSIxERR_OK;

		// Deep Copy
		if (pstSource->pszProviderName)
		{
			eErr = eSIxERR_FAIL;
			ulSize = silib_desc_util_dvbStrlen(pstSource->pszProviderName);
			if (ulSize > 0)
			{
				pstDest->pszProviderName = silib_desc_util_allocStr(ulSize+1);
				if (NULL == pstDest->pszProviderName)
					goto err_exit;

				HxSTD_MemCopy(pstDest->pszProviderName, pstSource->pszProviderName, ulSize);
				pstDest->pszProviderName[ulSize] = '\0';
				eErr = eSIxERR_OK;
			}
		}

		if (pstSource->pszServName)
		{
			eErr = eSIxERR_FAIL;
			ulSize = silib_desc_util_dvbStrlen(pstSource->pszServName);
			if (ulSize > 0)
			{
				pstDest->pszServName = silib_desc_util_allocStr(ulSize+1);
				if (NULL == pstDest->pszServName)
					goto err_exit;

				HxSTD_MemCopy(pstDest->pszServName, pstSource->pszServName, ulSize);
				pstDest->pszServName[ulSize] = '\0';
				eErr = eSIxERR_OK;
			}
		}
	}

	return eErr;

err_exit:
	silib_desc_free(pstDest->pszProviderName);
	pstDest->pszProviderName = NULL;

	silib_desc_free(pstDest->pszServName);
	pstDest->pszServName = NULL;
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_COUNTRY_AVAILABILITY (TAG = 0x49)
 ************************************************************************/
static void initCountryAvailInfo(SIxCountryAvailInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo->szCountryAvailCode, '\0', 4);
	pstInfo->pstNext = NULL;
}

static SIxCountryAvailInfo_t *allocCountryAvailInfo(void)
{
	SIxCountryAvailInfo_t	*pstInfo;

	pstInfo = (SIxCountryAvailInfo_t *)silib_desc_alloc(sizeof(SIxCountryAvailInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocCountryAvailInfo()\n");
		return NULL;
	}
	initCountryAvailInfo(pstInfo);

	return ((SIxCountryAvailInfo_t *)pstInfo);
}

static void freeCountryAvailInfo(SIxCountryAvailInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setCountryAvailInfo(SIxCountryAvailInfo_t *pstInfo, HUINT8 *pszCountryAvailCode)
{
	HxSTD_MemCopy(pstInfo->szCountryAvailCode, pszCountryAvailCode, silib_desc_util_dvbStrlen(pszCountryAvailCode));
	pstInfo->szCountryAvailCode[silib_desc_util_dvbStrlen(pszCountryAvailCode)] = '\0';
}

static void initCountryAvailDes(SIxCountryAvailDes_t *pstDes)
{
	pstDes->bCountryAvail 	= FALSE;
	pstDes->pstInfo = NULL;
}

static SIxCountryAvailDes_t *allocCountryAvailDes(void)
{
	return (SIxCountryAvailDes_t *)silib_desc_alloc(sizeof(SIxCountryAvailDes_t));
}

static void freeCountryAvailDes(SIxCountryAvailDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxCountryAvailInfo_t, freeCountryAvailInfo);
	silib_desc_free(pstDes);
}


static SIxError_e parseCountryAvailDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCountryAvailDes_t *pstDes)
{
	SIxCountryAvailInfo_t	*pstNewInfo = NULL, *pstPrevInfo = NULL;
	HBOOL					bCountryAvailAvail;
	HUINT8					szCountryAvailCode[4];
	HUINT8					ucTemp, ucVal;
	HUINT16					usDesLen;

	HxLOG_Info("parseCountryAvailDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	/** < country availability flag (1byte) */
	ucTemp = *p++;
	usDesLen--;
	ucVal = ucTemp & 0x80;
	(ucVal == 1) ? (bCountryAvailAvail = TRUE) : (bCountryAvailAvail = FALSE);
	HxLOG_Print("\t\t(country availability flag = 0x%d)\n", bCountryAvailAvail);

	if ((usDesLen % 3) != 0)
	{
		HxLOG_Print("[WARN] Unknown CountryAvailDes Format!\n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < country code (3byte) */
		HxSTD_MemCopy (szCountryAvailCode, p, 3);
		szCountryAvailCode [3] = '\0';
		silib_desc_util_makeCorrct639Code(szCountryAvailCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(country code = %s)\n", (char *)szCountryAvailCode);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxCountryAvailInfo_t, pstDes);
		pstNewInfo = allocCountryAvailInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setCountryAvailInfo (pstNewInfo, szCountryAvailCode);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseCountryAvailDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyCountryAvailDes(SIxCountryAvailDes_t *pstDest, const SIxCountryAvailDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCountryAvailDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		// Deep Copy
		if (pstSource->pstInfo)
		{
			SIxCountryAvailInfo_t	*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxCountryAvailInfo_t, pstDest);

				pstNewInfo = allocCountryAvailInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setCountryAvailInfo(pstNewInfo, pstSourceInfo->szCountryAvailCode);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxCountryAvailInfo_t, freeCountryAvailInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_LINKAGE (TAG = 0x4a)
 ************************************************************************/
static void initLinkDes(SIxLinkDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxLinkDes_t));
}

static SIxLinkDes_t *allocLinkDes(void)
{
	return (SIxLinkDes_t *)silib_desc_alloc(sizeof(SIxLinkDes_t));
}

static void freeLinkDes(SIxLinkDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_util_freePrivateDataInfo(pstDes->pstPrivateDataInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseLinkDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxLinkDes_t *pstDes)
{
	SIxPrivateDataInfo_t	*pstNewInfo = NULL;
	HUINT16				usDesLen;

	HxLOG_Info("parseLinkDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 7)
	{
		HxLOG_Print("[WARN] Unknown parseLinkDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < transport stream id (2byte) */
	pstDes->usTsId = HxMACRO_Get16Bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(transport stream id = 0x%x)\n", pstDes->usTsId);

	/** < original network id (2byte) */
	pstDes->usOrgNetId = HxMACRO_Get16Bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(original network id = 0x%x)\n", pstDes->usOrgNetId);

	/** < service id (2byte) */
	pstDes->usSvcId = HxMACRO_Get16Bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(service id = 0x%x)\n", pstDes->usSvcId);

	/** < linkage type (1byte) */
	pstDes->ucLinkType = *p++;
	usDesLen--;
	HxLOG_Print("\t\t(linkage type = 0x%x)\n", pstDes->ucLinkType);

	/** < Private Data */
	if (usDesLen > 0)
	{
		SIxError_e		eErr;

		pstNewInfo = (SIxPrivateDataInfo_t *)silib_desc_util_allocPrivateDataInfo((HUINT8)usDesLen);
		if (pstNewInfo == NULL)
			return eSIxERR_NOT_ENOUGH_MEMORY;

		eErr = silib_desc_util_setPrivateDataInfo(pstNewInfo, p, (HUINT8)usDesLen);
		if (eSIxERR_OK != eErr)
		{
			silib_desc_util_freePrivateDataInfo(pstNewInfo);
			return eSIxERR_FAIL;
		}
	}

	/** < Descriptor 데이타 구성. */
	pstDes->pstPrivateDataInfo = pstNewInfo;

	HxLOG_Info("parseLinkDes() Leaved!");
	return eSIxERR_OK;
}

static SIxError_e copyLinkDes(SIxLinkDes_t *pstDest, const SIxLinkDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxLinkDes_t));
		pstDest->pstPrivateDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPrivateDataInfo);
		if (NULL == pstDest->pstPrivateDataInfo)
			goto err_exit;
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_util_freePrivateDataInfo(pstDest->pstPrivateDataInfo);
		pstDest->pstPrivateDataInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_NVOD_REFERENCE (TAG = 0x4B)
 ************************************************************************/
static void initNvodRefInfo(SIxNvodRefInfo_t *pstInfo)
{
	pstInfo->usTsId 		= 0;
	pstInfo->usOrgNetId 	= 0;
	pstInfo->usSvcId 		= 0;
	pstInfo->pstNext 		= NULL;
}

static SIxNvodRefInfo_t *allocNvodRefInfo(void)
{
	SIxNvodRefInfo_t	*pstInfo;

	pstInfo = (SIxNvodRefInfo_t *)silib_desc_alloc(sizeof(SIxNvodRefInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocNvodRefInfo()\n");
		return NULL;
	}
	initNvodRefInfo(pstInfo);

	return ((SIxNvodRefInfo_t *)pstInfo);
}

static void freeNvodRefInfo(SIxNvodRefInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setNvodRefInfo(SIxNvodRefInfo_t *pstInfo, HUINT16 usTsId, HUINT16 usOrgNetId, HUINT16 usSvcId)
{
	pstInfo->usTsId 		= usTsId;
	pstInfo->usOrgNetId 	= usOrgNetId;
	pstInfo->usSvcId 		= usSvcId;
}

static void initNvodRefDes(SIxNvodRefDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxNvodRefDes_t *allocNvodRefDes(void)
{
	return (SIxNvodRefDes_t *)silib_desc_alloc(sizeof(SIxNvodRefDes_t));
}

static void freeNvodRefDes(SIxNvodRefDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNvodRefInfo_t, freeNvodRefInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseNvodRefDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxNvodRefDes_t *pstDes)
{
	SIxNvodRefInfo_t	*pstNewInfo = NULL, *pstPrevInfo = NULL;
	HUINT16			usDesLen;
	HUINT16			usTsId, usOrigNetId, usSvcId;

	HxLOG_Info("parseNvodRefDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 6) != 0)
	{
		HxLOG_Print("[WARN] Unknown NvodRefDes Format!\n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < transport stream id (2byte) */
		usTsId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(transport stream id = 0x%x)\n", usTsId);

		/** < original network id (2byte) */
		usOrigNetId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(original network id = 0x%x)\n", usOrigNetId);

		/** < service id (2byte) */
		usSvcId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(service id = 0x%x)\n", usSvcId);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxNvodRefInfo_t, pstDes);

		pstNewInfo = allocNvodRefInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setNvodRefInfo (pstNewInfo, usTsId, usOrigNetId, usSvcId);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseNvodRefDes() Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxNvodRefInfo_t, freeNvodRefInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyNvodRefDes(SIxNvodRefDes_t *pstDest, const SIxNvodRefDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxNvodRefDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		// Deep Copy
		if (pstSource->pstInfo)
		{
			SIxNvodRefInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxNvodRefInfo_t, pstDest);

				pstNewInfo = allocNvodRefInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setNvodRefInfo(pstNewInfo, pstSourceInfo->usTsId, pstSourceInfo->usOrgNetId, pstSourceInfo->usSvcId);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxNvodRefInfo_t, freeNvodRefInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_TIME_SHIFTED_SERVICE (TAG = 0x4c)
 ************************************************************************/
static void initTimeShiftSvcDes(SIxTimeShiftSvcDes_t *pstDes)
{
	pstDes->usRefSvcId = 0;
}

static SIxTimeShiftSvcDes_t *allocTimeShiftSvcDes(void)
{
	return (SIxTimeShiftSvcDes_t *)silib_desc_alloc(sizeof(SIxTimeShiftSvcDes_t));
}

static void freeTimeShiftSvcDes(SIxTimeShiftSvcDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setTimeShiftSvcDes(SIxTimeShiftSvcDes_t *pstDes, HUINT16 usRefServId)
{
	pstDes->usRefSvcId = usRefServId;
}

static SIxError_e parseTimeShiftSvcDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxTimeShiftSvcDes_t *pstDes)
{
	HUINT16				usDesLen;
	HUINT16				usRefServId;

	HxLOG_Info("parseTimeShiftSvcDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 2)
	{
		HxLOG_Print("[WARN] Unknown parseTimeShiftSvcDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < reference service id (2byte) */
	usRefServId = HxMACRO_Get16Bit(p);
	HxLOG_Print("\t\t(reference service id = 0x%x)\n", usRefServId);

	/** < Descriptor 데이타 구성. */
	setTimeShiftSvcDes (pstDes, usRefServId);

	HxLOG_Info("parseTimeShiftSvcDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyTimeShiftSvcDes(SIxTimeShiftSvcDes_t *pstDest, const SIxTimeShiftSvcDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTimeShiftSvcDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_SHORT_EVENT (TAG = 0x4D)
 ************************************************************************/
static void initShortEvtDes(SIxShortEvtDes_t *pstDes)
{
	HxSTD_MemSet(pstDes->szIso639LangCode, '\0', 4);
	pstDes->pszEvtName 			= NULL;
	pstDes->pszText 				= NULL;
}

static SIxShortEvtDes_t *allocShortEvtDes(void)
{
	return (SIxShortEvtDes_t *)silib_desc_alloc(sizeof(SIxShortEvtDes_t));
}

static void freeShortEvtDes(SIxShortEvtDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_free(pstDes->pszEvtName);
	silib_desc_free(pstDes->pszText);
	silib_desc_free(pstDes);
}

static void setShortEvtDes(SIxShortEvtDes_t *pstDes, HUINT8 *pszIso639LangCode, HUINT8 *pszEvtName, HUINT8 *pszText)
{
    if(pszIso639LangCode != NULL)
    	HxSTD_MemCopy(pstDes->szIso639LangCode, pszIso639LangCode, 3);
	pstDes->szIso639LangCode[3] = '\0';
    if(pszEvtName != NULL)
	    HxSTD_MemCopy(pstDes->pszEvtName, pszEvtName, silib_desc_util_dvbStrlen(pszEvtName));
	pstDes->pszEvtName[silib_desc_util_dvbStrlen(pszEvtName)] = '\0';
    if(pszText != NULL)
	    HxSTD_MemCopy(pstDes->pszText, pszText, silib_desc_util_dvbStrlen(pszText));
	pstDes->pszText[silib_desc_util_dvbStrlen(pszText)] = '\0';
}

static SIxError_e parseShortEvtDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxShortEvtDes_t *pstDes)
{
	HUINT8			ucEvtNameLen;
	HUINT8			szIso639LangCode[4], *pszEvtName = NULL, *pszText = NULL;
	HUINT16			usTextLen;
	HINT16			usDesLen;
	HUINT32			nTempEvtNameLen, nTempTextLen;

	HxLOG_Info("parseShortEvtDes() Entered!\n");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = (HINT16)*p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen < 6)
	{
		HxLOG_Print("[WARN] Unknown parseShortEvtDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < ISO 639 language code (3byte) */
	HxSTD_MemCopy(szIso639LangCode, p, 3);
	szIso639LangCode[3] = '\0';
	silib_desc_util_makeCorrct639Code(szIso639LangCode);
	p += 3;
	usDesLen -= 3;
	HxLOG_Print("\t\t(ISO 639 language code = %s)\n", (char *)szIso639LangCode);

	/** < event name length (1byte) */
	ucEvtNameLen = *p++;
	usDesLen--;
	HxLOG_Print("\t\t(event name length = %d)\n", ucEvtNameLen);

	if(usDesLen < ucEvtNameLen)
	{
		HxLOG_Print("[WARN] Unknown parseShortEvtDes Format!\n");
		return eSIxERR_INVALID;
	}
	/** < Max allocation limit & allocation */
#ifdef EVTNAME_DEBUG
	{
		HUINT32 i,j;
		HxLOG_Print("Event name(raw data -len %d, %s)\n",ucEvtNameLen,szIso639LangCode);
		HLIB_LOG_Dump(p,ucEvtNameLen, 0, FALSE);
		HxLOG_Info("\n\t");
		for ( i = 0; i < ucEvtNameLen; i++ ) {
			HxLOG_Print("%c",*(p+i));
			HxLOG_Print("-");
			HxLOG_Print("*");
		}
		HxLOG_Info("\n\n");
	}
#endif
	/** < event name */
	nTempEvtNameLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucEvtNameLen, (const HUINT8 *)p, &pszEvtName);
	p += ucEvtNameLen;
	usDesLen -= ucEvtNameLen;
#ifdef EVTNAME_DEBUG
	{
		HUINT32 i;
		HxLOG_Print("\tString Len:%d\n\t", nTempEvtNameLen);
		HLIB_LOG_Dump(pszEvtName, nTempEvtNameLen, 0, FALSE);
		HxLOG_Info("\n\n");
	}
#else
	HxLOG_Print("\t\t(event name = %s)\n", (char *)pszEvtName);
#endif

	if(usDesLen <= 0)
	{
		/** < jmyang, 2003.07.15 for ukrine cable
			  현지 스트림에서 Descriptor length가 Event name까지만 있는 버그로 인해서, 정상동작하지 않았었음. */
		HxLOG_Print("[parseShortEvtDes] OOPs: Descriptor Length is lack = [%d]\n", usDesLen-1);
		usTextLen = 0;
	}
	else
	{
		/** < text length (1byte) */
		usTextLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t(text length = %d)\n", usTextLen);

		if(usDesLen < usTextLen)
		{
			silib_desc_free(pszEvtName);
			HxLOG_Print("[WARN] Unknown parseShortEvtDes Format!\n");
			return eSIxERR_INVALID;
		}
	}

	/** < Max allocation limit & allocation */
#ifdef EVTNAME_DEBUG
	{
		HUINT32 i,j;
		HxLOG_Print("Text (raw data -len %d, %s)\n",usTextLen, szIso639LangCode);
		HLIB_LOG_Dump(p, usTextLen, 0, FALSE);
		HxLOG_Print("\n\t");
		for ( i = 0; i < usTextLen; i++ ) {
			HxLOG_Print("%c",*(p+i));
			HxLOG_Print("-");
			HxLOG_Print("*");
		}
		HxLOG_Print("\n\n");
	}
#endif

	/** < text */
	nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT32)usTextLen, (const HUINT8 *)p, &pszText);
#ifdef EVTNAME_DEBUG
	{
		HUINT32 i;
		HxLOG_Print("\tText Len:%d\n\t", nTempTextLen);
		HLIB_LOG_Dump(pszText, nTempTextLen, 0, FALSE);
	}
#else
	HxLOG_Print("\t\t(text = %s)\n", (char *)pszText);
#endif

	/** < Descriptor 데이타 구성. */
	pstDes->pszEvtName = silib_desc_util_allocStr(nTempEvtNameLen+1);
	if (pstDes->pszEvtName == NULL)
		goto err_exit;
	pstDes->pszText = silib_desc_util_allocStr(nTempTextLen+1);
	if (pstDes->pszText == NULL)
		goto err_exit;
	setShortEvtDes(pstDes, szIso639LangCode, pszEvtName, pszText);
	silib_desc_free(pszEvtName);
	silib_desc_free(pszText);

	HxLOG_Info(" parseShortEvtDes() Leaved!\n");

	pstDes->ucEvtNameSize = nTempEvtNameLen + 1;
	pstDes->ucTextSize = nTempTextLen + 1;

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszEvtName);
	silib_desc_free(pszText);
	silib_desc_free(pstDes->pszEvtName);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyShortEvtDes(SIxShortEvtDes_t *pstDest, const SIxShortEvtDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxShortEvtDes_t));
		pstDest->pszEvtName = NULL;
		pstDest->ucEvtNameSize = 0;
		pstDest->pszText = NULL;
		pstDest->ucTextSize = 0;
		eErr = eSIxERR_OK;

		if (0 < pstSource->ucEvtNameSize && NULL != pstSource->pszEvtName)
		{
			eErr = eSIxERR_FAIL;

			pstDest->pszEvtName = silib_desc_util_allocStr(pstSource->ucEvtNameSize+1);
			if (pstDest->pszEvtName)
			{
				HxSTD_MemCopy(pstDest->pszEvtName, pstSource->pszEvtName, pstSource->ucEvtNameSize);
				pstDest->pszEvtName[pstSource->ucEvtNameSize] = '\0';
				pstDest->ucEvtNameSize = pstSource->ucEvtNameSize;
				eErr = eSIxERR_OK;
			}
			else
				goto err_exit;
		}

		if (0 < pstSource->ucTextSize && NULL != pstSource->pszText)
		{
			eErr = eSIxERR_FAIL;

			pstDest->pszText = silib_desc_util_allocStr(pstSource->ucTextSize+1);
			if (pstDest->pszText)
			{
				HxSTD_MemCopy(pstDest->pszText, pstSource->pszText, pstSource->ucTextSize);
				pstDest->pszText[pstSource->ucTextSize] = '\0';
				pstDest->ucTextSize = pstSource->ucTextSize;
				eErr = eSIxERR_OK;
			}
			else
				goto err_exit;
		}
	}

	return eErr;

err_exit:
	silib_desc_free(pstDest->pszEvtName);
	pstDest->pszEvtName = NULL;
	pstDest->ucEvtNameSize = 0;

	silib_desc_free(pstDest->pszText);
	pstDest->pszText = NULL;
	pstDest->ucTextSize = 0;
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_EXTENDED_EVENT (TAG = 0x4E)
 ************************************************************************/
static void initExtEvtInfo(SIxExtEvtInfo_t *pstInfo)
{
	pstInfo->pszDescription = NULL;
	pstInfo->pszItem 		= NULL;
	pstInfo->pstNext 		= NULL;
}

static SIxExtEvtInfo_t *allocExtEvtInfo(void)
{
	SIxExtEvtInfo_t	*pstInfo;

	pstInfo = (SIxExtEvtInfo_t *)silib_desc_alloc(sizeof(SIxExtEvtInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocExtEvtInfo()\n");
		return NULL;
	}
	initExtEvtInfo(pstInfo);

	return ((SIxExtEvtInfo_t *)pstInfo);
}

static void freeExtEvtInfo(SIxExtEvtInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo->pszDescription);
	silib_desc_free(pstInfo->pszItem);
	silib_desc_free(pstInfo);
}

static void setExtEvtInfo(SIxExtEvtInfo_t *pstInfo, HUINT8 *pszDescription, HUINT8 *pszItem)
{
    if(pszDescription != NULL)
	    HxSTD_MemCopy(pstInfo->pszDescription, pszDescription, silib_desc_util_dvbStrlen(pszDescription));
	pstInfo->pszDescription[silib_desc_util_dvbStrlen(pszDescription)] = '\0';
	if(pszItem != NULL)
	    HxSTD_MemCopy(pstInfo->pszItem, pszItem, silib_desc_util_dvbStrlen(pszItem));
	pstInfo->pszItem[silib_desc_util_dvbStrlen(pszItem)] = '\0';
}

static void initExtEvtDes(SIxExtEvtDes_t *pstDes)
{
	pstDes->ucDesNum		= 0;
	pstDes->ucLastDesNum	= 0;
	HxSTD_MemSet(pstDes->szIso639LangCode, '\0', 4);
	pstDes->pstInfo 	= NULL;
	pstDes->pszChar			= NULL;
}


static SIxExtEvtDes_t *allocExtEvtDes(void)
{
	return (SIxExtEvtDes_t *)silib_desc_alloc(sizeof(SIxExtEvtDes_t));
}

static void freeExtEvtDes(SIxExtEvtDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxExtEvtInfo_t, freeExtEvtInfo);
	silib_desc_free(pstDes->pucTextChar);
	silib_desc_free(pstDes->pszChar);
	silib_desc_free(pstDes);
}

static void setExtEvtDes(SIxExtEvtDes_t *pstDes, HUINT8 ucDesNum, HUINT8 ucLastDesNum, HUINT8 *pszIso639LangCode, HUINT8 *pszChar)
{
	pstDes->ucDesNum = ucDesNum;
	pstDes->ucLastDesNum = ucLastDesNum;
	if(pszIso639LangCode != NULL)
	    HxSTD_MemCopy(pstDes->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstDes->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	if(pszChar != NULL)
	    HxSTD_MemCopy(pstDes->pszChar, pszChar, silib_desc_util_dvbStrlen(pszChar));
	pstDes->pszChar[silib_desc_util_dvbStrlen(pszChar)] = '\0';
}

static SIxError_e parseExtEvtDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxExtEvtDes_t *pstDes)
{
	SIxExtEvtInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8			ucDesNum, ucLastDesNum;
	HUINT8			szIso639LangCode[4], *pszItemDes = NULL, *pszItem = NULL, *pszText = NULL;
	HUINT8			*pszTextChar = NULL;
	HUINT8			ucTemp;
	HUINT16			usItemDesLen, usItemLen, usTextLen;
	HINT16			usDesLen, wLenOfItem;
	HUINT32			nTempItemDesLen, nTempItemLen, nTempTextLen;
	HUINT32			nAssignedItemDesLen, nAssignedItemLen, nAssignedTextLen;

	HxLOG_Info("parseShortEvtDes() Entered!\n");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = (HINT16)*p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 5)
	{
		HxLOG_Print("[WARN] Unknown parseShortEvtDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < descriptor number & last descriptor number (1byte) */
	ucTemp = *p++;
	usDesLen--;
	ucDesNum = (HUINT8)((ucTemp & 0xf0) >> 4);
	ucLastDesNum = ucTemp & 0x0f;
	HxLOG_Print("\t\t(descriptor number = 0x%x)\n", ucDesNum);
	HxLOG_Print("\t\t(last descriptor number = 0x%x)\n", ucLastDesNum);

	/** < ISO 639 language code (3byte) */
	HxSTD_MemCopy(szIso639LangCode, p, 3);
	szIso639LangCode[3] = '\0';
	silib_desc_util_makeCorrct639Code(szIso639LangCode);
	p += 3;
	usDesLen -= 3;
	HxLOG_Print("\t\t(ISO 639 language code = %s)\n", (char *)szIso639LangCode);

	/** < length of item (1byte) */
	wLenOfItem = (HINT16)*p++;
	usDesLen--;
	HxLOG_Print("\t\t(length of item = %d)\n", wLenOfItem);

	while (usDesLen >= wLenOfItem && wLenOfItem >= 2 && wLenOfItem < MAX_DESCRIPTOR_LIMIT)
	{
		/** < item description length (1byte) */
		usItemDesLen = *p++;
		wLenOfItem--;
		usDesLen--;
		HxLOG_Print("\t\t\t(item description length = %d)\n", usItemDesLen);

		/** < Max allocation limit & allocation */
		nAssignedItemDesLen = (usItemDesLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usItemDesLen);

		/** < item des */
		nTempItemDesLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, nAssignedItemDesLen, (const HUINT8 *)p, &pszItemDes);
		p += usItemDesLen;
		wLenOfItem -= usItemDesLen;
		usDesLen -= usItemDesLen;
		HxLOG_Print("\t\t\t(item description = %s)\n", (char *)pszItemDes);

		/** < item length (1byte) */
		usItemLen = *p++;
		wLenOfItem--;
		usDesLen--;
		HxLOG_Print("\t\t\t(item length = %d)\n", usItemLen);

		/** < Max allocation limit & allocation */
		nAssignedItemLen = (usItemLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usItemLen);

		/** < item */
		nTempItemLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, nAssignedItemLen, (const HUINT8 *)p, &pszItem);

		p += usItemLen;
		wLenOfItem -= usItemLen;
		usDesLen -= usItemLen;
		HxLOG_Print("\t\t\t(item = %s)\n", (char *)pszItem);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxExtEvtInfo_t, pstDes);
		pstNewInfo = allocExtEvtInfo();
		if (pstNewInfo == NULL)
			goto err_exit1;
		pstNewInfo->pszDescription = silib_desc_util_allocStr(nTempItemDesLen+1);
		if (pstNewInfo->pszDescription == NULL)
			goto err_exit1;
		pstNewInfo->pszItem = silib_desc_util_allocStr(nTempItemLen+1);
		if (pstNewInfo->pszItem == NULL)
			goto err_exit1;
		pstNewInfo->ucItemLength = nTempItemLen;
		setExtEvtInfo(pstNewInfo, pszItemDes, pszItem);
		silib_desc_free(pszItemDes);
		pszItemDes = NULL;

		silib_desc_free(pszItem);
		pszItem = NULL;

		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	/** < text length (1byte) */
	usTextLen = *p++;
	HxLOG_Print("\t\t(text length = %d)\n", usTextLen);
#ifdef EXTEVTTEXT_DEBUG
	{
		HUINT32 i;
		HxLOG_Print("Extevt text(raw data-len %d )\n",usTextLen);
		for ( i = 0; i < usTextLen; i++ ) {
			HxLOG_Print("%x ",*(p+i));
		}
		HxLOG_Print("\n\t");
		for ( i = 0; i < usTextLen; i++ ) {
			HxLOG_Print("%c",*(p+i));
			HxLOG_Print("-");
			HxLOG_Print("*");
		}
		HxLOG_Print("\n\n");
	}
#endif
	/**
	 * Extended는 text char를 byte로 유지한 후, 나중에 같은 language code에 해당되는
	 * 디스크립터들의 모든 byte를 하나로 모은 후, 한번에 parsing해야 함
	 */
	pstDes->pucTextChar = (HUINT8 *)HLIB_STD_MemDupEx(p, (HUINT32)usTextLen, (void *(*)(HUINT32))silib_desc_util_allocStr);
	if (pstDes->pucTextChar == NULL)
		goto err_exit2;

	pstDes->ucTextLength = (HUINT8)usTextLen;

	/** < Max allocation limit & allocation */
	nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);

	/** < text */
	nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, nAssignedTextLen, (const HUINT8 *)p, &pszText);
	HxLOG_Print("\t\t(text = %s)\n", pszText);

	/** < Descriptor 데이타 구성. */
	pstDes->pszChar = silib_desc_util_allocStr(nTempTextLen+1);
	if (pstDes->pszChar == NULL)
		goto err_exit2;

	setExtEvtDes(pstDes, ucDesNum, ucLastDesNum, szIso639LangCode, pszText);
	silib_desc_free(pszText);

	HxLOG_Info(" parseShortEvtDes() Leaved!\n");

	return eSIxERR_OK;

err_exit1:
	silib_desc_free(pszItemDes);
	silib_desc_free(pszItem);
	silib_desc_free(pszText);
	silib_desc_free(pszTextChar);

	if (pstNewInfo != NULL)
	{
		silib_desc_free(pstNewInfo->pszDescription);
		silib_desc_freeInfo_Dvb(eSIxDESCTAG_EXTENDED_EVENT, 0, (HUINT8 *)pstNewInfo);
	}
	return eSIxERR_NOT_ENOUGH_MEMORY;


err_exit2:
	silib_desc_free(pszItemDes);
	silib_desc_free(pszItem);
	silib_desc_free(pszText);
	silib_desc_free(pszTextChar);

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyExtEvtDes(SIxExtEvtDes_t *pstDest, const SIxExtEvtDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxExtEvtDes_t));
		pstDest->pstInfo		= NULL;
		pstDest->pszChar			= NULL;
		pstDest->pucTextChar		= NULL;
		pstDest->ucTextLength		= 0;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			HUINT32			 ulLength;
			SIxExtEvtInfo_t	*pstPrevInfo = NULL, *pstNewInfo, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				/** < Descriptor 데이타 구성. */
				GET_PREV_INFO(pstPrevInfo, SIxExtEvtInfo_t, pstDest);

				pstNewInfo = allocExtEvtInfo();
				if (pstNewInfo == NULL)
					goto err_exit;

				ulLength = silib_desc_util_dvbStrlen(pstSourceInfo->pszDescription);
				pstNewInfo->pszDescription = silib_desc_util_allocStr(ulLength+1);
				if (pstNewInfo->pszDescription == NULL)
					goto err_exit;

				ulLength = silib_desc_util_dvbStrlen(pstSourceInfo->pszItem);
				pstNewInfo->pszItem = silib_desc_util_allocStr(ulLength+1);
				if (pstNewInfo->pszItem == NULL)
					goto err_exit;

				pstNewInfo->ucItemLength = ulLength;

				setExtEvtInfo(pstNewInfo, pstSourceInfo->pszDescription, pstSourceInfo->pszItem);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}

		if (pstSource->pszChar)
		{
			HUINT32 	ulSize = silib_desc_util_dvbStrlen(pstSource->pszChar);

			eErr = eSIxERR_FAIL;
			if (0 < ulSize)
			{
				pstDest->pszChar = silib_desc_util_allocStr(ulSize+1);
				if (pstDest->pszChar)
				{
					HxSTD_MemCopy(pstDest->pszChar, pstSource->pszChar, ulSize);
					pstDest->pszChar[ulSize] = '\0';
					eErr = eSIxERR_OK;
				}
			}
		}

		if (0 < pstSource->ucTextLength && NULL != pstSource->pucTextChar)
		{
			eErr = eSIxERR_FAIL;
			pstDest->pucTextChar = silib_desc_util_allocStr(pstSource->ucTextLength+1);
			if (pstDest->pucTextChar)
			{
				HxSTD_MemCopy(pstDest->pucTextChar, pstSource->pucTextChar, pstSource->ucTextLength);
				pstDest->pucTextChar[pstSource->ucTextLength] = '\0';
				pstDest->ucTextLength = pstSource->ucTextLength;
				eErr = eSIxERR_OK;
			}
		}
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxExtEvtInfo_t, freeExtEvtInfo);
		pstDest->pstInfo = NULL;

		silib_desc_free(pstDest->pszChar);
		pstDest->pszChar = NULL;

		silib_desc_free(pstDest->pucTextChar);
		pstDest->pucTextChar = NULL;
		pstDest->ucTextLength = 0;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_TIME_SHIFTED_EVENT (TAG = 0x4F)
 ************************************************************************/
static void initTimeShiftEvtDes(SIxTimeShiftEvtDes_t *pstDes)
{
	pstDes->usRefSvcId 	= 0;
	pstDes->usRefEvtId 	= 0;
}

static SIxTimeShiftEvtDes_t *allocTimeShiftEvtDes(void)
{
	return (SIxTimeShiftEvtDes_t *)silib_desc_alloc(sizeof(SIxTimeShiftEvtDes_t));
}

static void freeTimeShiftEvtDes(SIxTimeShiftEvtDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setTimeShiftEvtDes(SIxTimeShiftEvtDes_t *pstDes, HUINT16 usRefServId, HUINT16 usRefEvtId)
{
	pstDes->usRefSvcId 	= usRefServId;
	pstDes->usRefEvtId 	= usRefEvtId;
}

static SIxError_e parseTimeShiftEvtDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxTimeShiftEvtDes_t *pstDes)
{
	HUINT16				usDesLen;
	HUINT16				usRefServId, usRefEvtId;

	HxLOG_Info("parseTimeShiftEvtDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown parseTimeShiftEvtDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < reference service id (2byte) */
	usRefServId = HxMACRO_Get16Bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(reference service id = 0x%x)\n", usRefServId);

	/** < reference event id (2byte) */
	usRefEvtId = HxMACRO_Get16Bit(p);
	/** <p += 2; */
	/** <usDesLen -= 2; */
	HxLOG_Print("\t\t(reference event id = 0x%x)\n", usRefEvtId);

	/** < Descriptor 데이타 구성. */
	setTimeShiftEvtDes(pstDes, usRefServId, usRefEvtId);

	HxLOG_Info(" parseTimeShiftEvtDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyTimeShiftEvtDes(SIxTimeShiftEvtDes_t *pstDest, const SIxTimeShiftEvtDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTimeShiftEvtDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_COMPONENT (TAG = 0x50)
 ************************************************************************/
static void initCompDes(SIxCompDes_t *pstDes)
{
	pstDes->ucStmContent 	= 0;
	pstDes->ucCompType 	= 0;
	pstDes->ucCompTag 	= 0;
	HxSTD_MemSet(pstDes->szIso639LangCode, '\0', 4);
	pstDes->pszText 		= NULL;
}

static SIxCompDes_t *allocCompDes(void)
{
	return (SIxCompDes_t *)silib_desc_alloc(sizeof(SIxCompDes_t));
}

static void freeCompDes(SIxCompDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_free(pstDes->pszText);
	silib_desc_free(pstDes);
}

static void setCompDes(SIxCompDes_t *pstDes, HUINT8 ucStmContent, HUINT8 ucCompType, HUINT8 ucCompTag, HUINT8 *pszIso639LangCode, HUINT8 *pszText)
{
	pstDes->ucStmContent = ucStmContent;
	pstDes->ucCompType = ucCompType;
	pstDes->ucCompTag = ucCompTag;
	if(pszIso639LangCode != NULL)
    	HxSTD_MemCopy(pstDes->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstDes->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	if(pszText != NULL)
    	HxSTD_MemCopy(pstDes->pszText, pszText, silib_desc_util_dvbStrlen(pszText));
	pstDes->pszText[silib_desc_util_dvbStrlen(pszText)] = '\0';
}

static SIxError_e parseCompDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCompDes_t *pstDes)
{
	HUINT8		ucStmContent, ucCompType, ucCompTag;
	HUINT8		szIso639LangCode [4], *pszText = NULL;
	HUINT16		usDesLen, usTextLen;
	HUINT32		nTempTextLen;
	HUINT32		nAssignedTextLen;

	HxLOG_Info("parseCompDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	/** < stream content (1byte) */
	ucStmContent = *p++ & 0x0f;
	usDesLen--;
	HxLOG_Print("\t\t(stream content = 0x%x)\n", ucStmContent);

	/** < component type (1byte) */
	ucCompType = *p++;
	usDesLen--;
	HxLOG_Print("\t\t(component type = 0x%x)\n", ucCompType);

	/** < component tag (1byte) */
	ucCompTag = *p++;
	usDesLen--;
	HxLOG_Print("\t\t(component tag = 0x%x)\n", ucCompTag);

	/** < ISO 639 language code (3byte) */
	HxSTD_MemCopy (szIso639LangCode, p, 3);
	szIso639LangCode [3] = '\0';
	silib_desc_util_makeCorrct639Code(szIso639LangCode);
	p += 3;
	usDesLen -= 3;
	HxLOG_Print("\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

	/** < text */
	usTextLen = usDesLen;

	/** < Max allocation limit & allocation */
	nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);

	/** < event name */
	nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, nAssignedTextLen, (const HUINT8 *)p, &pszText);
	HxLOG_Print("\t\t(text = %s)\n", pszText);

	/** < Descriptor 데이타 구성. */
	pstDes->pszText = silib_desc_util_allocStr (nTempTextLen + 1);
	if (pstDes->pszText == NULL)
		goto err_exit;

	setCompDes (pstDes, ucStmContent, ucCompType, ucCompTag, szIso639LangCode, pszText);
	silib_desc_free(pszText);

	HxLOG_Info("parseCompDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszText);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}


static SIxError_e copyCompDes(SIxCompDes_t *pstDest, const SIxCompDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCompDes_t));
		pstDest->pszText = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pszText)
		{
			HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSource->pszText);
			eErr = eSIxERR_FAIL;
			if (0 < ulSize)
			{
				pstDest->pszText = silib_desc_util_allocStr(ulSize+1);
				if (pstDest->pszText)
				{
					HxSTD_MemCopy(pstDest->pszText, pstSource->pszText, ulSize);
					pstDest->pszText[ulSize] = '\0';
					eErr = eSIxERR_OK;
				}
			}
		}
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_MOSAIC (TAG = 0x51)
 ************************************************************************/
#if 0
//Mosaic
static SIxError_e parseMosaicDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxMosaicDes_t *pstDes)
{
	HUINT8			ucTemp;
	HUINT16			usDesLen;

	HxLOG_Info("parseStmIdentDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown parseStmIdentDes Format!\n");
		return eSIxERR_INVALID;
	}

	ucTemp = *p;

	pstDes->ucEntryPoint = (HUINT8)((ucTemp >> 7) & 0x01);
	pstDes->ucHoriElemCellNum = (HUINT8)((ucTemp >> 4) & 0x07);
	pstDes->ucVertElemCellNum = (HUINT8)((ucTemp >> 0) & 0x07);

	HxLOG_Info("parseStmIdentDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyMosaicDes(SIxMosaicDes_t *pstDest, const SIxMosaicDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMosaicDes_t));
		eErr = ERR_OK;
	}

	return eErr;
}
#endif

/************************************************************************
 * eSIxDESCTAG_STREAM_IDENTIFIER (TAG = 0x52)
 ************************************************************************/
static void initStmIdentDes(SIxStmIdentDes_t *pstDes)
{
	pstDes->ucCompTag = 0;
}

static SIxStmIdentDes_t *allocStmIdentDes(void)
{
	return (SIxStmIdentDes_t *)silib_desc_alloc(sizeof(SIxStmIdentDes_t));
}

static void freeStmIdentDes(SIxStmIdentDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setStmIdentDes(SIxStmIdentDes_t *pstDes, HUINT8 ucCompTag)
{
	pstDes->ucCompTag = ucCompTag;
}

static 	HUINT32	parseStmIdentDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxStmIdentDes_t *pstDes)
{
	HUINT8			ucCompTag;
	HUINT16			usDesLen;

	HxLOG_Info("parseStmIdentDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown parseStmIdentDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < component tag (1byte) */
	ucCompTag = *p;
	HxLOG_Print("\t\t(component tag = 0x%x)\n", ucCompTag);

	/** < Descriptor 데이타 구성. */
	setStmIdentDes (pstDes, ucCompTag);

	HxLOG_Info("parseStmIdentDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyStmIdentDes(SIxStmIdentDes_t *pstDest, const SIxStmIdentDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxStmIdentDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_CA_IDENTIFIER (TAG = 0x53)
 ************************************************************************/
static void initCaIdentInfo(SIxCaIdentInfo_t *pstInfo)
{
	pstInfo->usCaStmId 	= 0;
	pstInfo->pstNext 	= NULL;
}

static SIxCaIdentInfo_t *allocCaIdentInfo(void)
{
	SIxCaIdentInfo_t	*pstInfo;

	pstInfo = (SIxCaIdentInfo_t *)silib_desc_alloc(sizeof(SIxCaIdentInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocCaIdentInfo()\n");
		return NULL;
	}
	initCaIdentInfo(pstInfo);

	return ((SIxCaIdentInfo_t *)pstInfo);
}

static void freeCaIdentInfo(SIxCaIdentInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setCaIdentInfo(SIxCaIdentInfo_t *pstInfo, HUINT16 usCaStmId)
{
	pstInfo->usCaStmId = usCaStmId;
}

static void initCaIdentDes(SIxCaIdentDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxCaIdentDes_t *allocCaIdentDes(void)
{
	return (SIxCaIdentDes_t *)silib_desc_alloc(sizeof(SIxCaIdentDes_t));
}

static void freeCaIdentDes(SIxCaIdentDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxCaIdentInfo_t, freeCaIdentInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseCaIdentDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCaIdentDes_t *pstDes)
{
	SIxCaIdentInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT16			usDesLen;
	HUINT16			usCaStmId;

	HxLOG_Info("parseCaIdentDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 2) != 0)
	{
		HxLOG_Print("[WARN] Unknown CaIdentDes Format!\n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < CA system id (2byte) */
		usCaStmId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t\t(CA system id = 0x%x)\n", usCaStmId);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxCaIdentInfo_t, pstDes);

		pstNewInfo = allocCaIdentInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setCaIdentInfo (pstNewInfo, usCaStmId);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info("parseCaIdentDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyCaIdentDes(SIxCaIdentDes_t *pstDest, const SIxCaIdentDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCaIdentDes_t));
		pstDest->pstInfo = NULL;

		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxCaIdentInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxCaIdentInfo_t, pstDest);

				pstNewInfo = allocCaIdentInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setCaIdentInfo(pstNewInfo, pstSourceInfo->usCaStmId);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxCaIdentInfo_t, freeCaIdentInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


/************************************************************************
 * eSIxDESCTAG_CONTENT (TAG = 0x54)
 ************************************************************************/
static void initContentInfo(SIxContentInfo_t *pstInfo)
{
	pstInfo->ucContentNibble	= 0;
	pstInfo->ucUserNibble		= 0;
	pstInfo->pstNext			= NULL;
}

static SIxContentInfo_t *allocContentInfo(void)
{
	SIxContentInfo_t	*pstInfo;

	pstInfo = (SIxContentInfo_t *)silib_desc_alloc(sizeof(SIxContentInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocContentInfo()\n");
		return NULL;
	}
	initContentInfo(pstInfo);

	return ((SIxContentInfo_t *)pstInfo);
}

static void freeContentInfo(SIxContentInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setContentInfo(SIxContentInfo_t *pstInfo, HUINT8 ucContentNibble, HUINT8 ucUserNibble)
{
	pstInfo->ucContentNibble	= ucContentNibble;
	pstInfo->ucUserNibble		= ucUserNibble;
}

static void initContentDes(SIxContentDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxContentDes_t *allocContentDes(void)
{
	return (SIxContentDes_t *)silib_desc_alloc(sizeof(SIxContentDes_t));
}

static void freeContentDes(SIxContentDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxContentInfo_t, freeContentInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseContentDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxContentDes_t *pstDes)
{
	SIxContentInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8				 ucContentNibble, ucUserNibble;
	HUINT16				 usDesLen;

	HxLOG_Info("parseContentDes() Entered!\n");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 2) != 0)
	{
		HxLOG_Print("[WARN] Unknown ContentDes Format!\n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < content nibble (1byte) */
		ucContentNibble = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(content nibble = 0x%x)\n", ucContentNibble);

		ucUserNibble = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(user nibble = 0x%x)\n", ucUserNibble);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxContentInfo_t, pstDes);

		pstNewInfo = allocContentInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setContentInfo(pstNewInfo, ucContentNibble, ucUserNibble);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseContentDes() Leaved!\n");

	return eSIxERR_OK;

err_exit:
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyContentDes(SIxContentDes_t *pstDest, const SIxContentDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxContentDes_t));
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxContentInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxContentInfo_t, pstDest);

				pstNewInfo = allocContentInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setContentInfo(pstNewInfo, pstSourceInfo->ucContentNibble, pstSourceInfo->ucUserNibble);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxContentInfo_t, freeContentInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_PARENTAL_RATING (TAG = 0x55)
 ************************************************************************/
static void initParentInfo(SIxParentInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo->szCountryCode, '\0', 4);
	pstInfo->ucRate		= 0;
	pstInfo->pstNext	= NULL;
}

static void freeParentInfo(SIxParentInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static SIxParentInfo_t *allocParentInfo(void)
{
	SIxParentInfo_t	*pstInfo;

	pstInfo = (SIxParentInfo_t *)silib_desc_alloc(sizeof(SIxParentInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocParentInfo()\n");
		return NULL;
	}
	initParentInfo(pstInfo);

	return ((SIxParentInfo_t *)pstInfo);
}

static void setParentInfo(SIxParentInfo_t *pstInfo, HUINT8 *pszCountryCode, HUINT8 ucRate)
{
	HxSTD_MemCopy(pstInfo->szCountryCode, pszCountryCode, silib_desc_util_dvbStrlen(pszCountryCode));
	pstInfo->szCountryCode[silib_desc_util_dvbStrlen(pszCountryCode)] = '\0';
	pstInfo->ucRate = ucRate;
}

static void initParentDes(SIxParentDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxParentDes_t *allocParentDes(void)
{
	return (SIxParentDes_t *)silib_desc_alloc(sizeof(SIxParentDes_t));
}

static void freeParentDes(SIxParentDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxParentInfo_t, freeParentInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseParentDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxParentDes_t *pstDes)
{
	SIxParentInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8				ucRate;
	HUINT8				szCountryCode[4];
	HUINT16				usDesLen;

	HxLOG_Info("parseParentRateDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 4) != 0)
	{
		HxLOG_Print("[WARN] Unknown ParentRateDes Format!\n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < country code (3byte) */
		HxSTD_MemCopy(szCountryCode, p, 3);
		szCountryCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szCountryCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(country code = %s)\n", szCountryCode);

		/** < rating (1byte) */
		ucRate = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(rating = 0x%x)\n", ucRate);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxParentInfo_t, pstDes);

		pstNewInfo = allocParentInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setParentInfo(pstNewInfo, szCountryCode, ucRate);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseParentRateDes() Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxParentInfo_t, freeParentInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyParentDes(SIxParentDes_t *pstDest, const SIxParentDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxParentDes_t));

		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxParentInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxParentInfo_t, pstDest);

				pstNewInfo = allocParentInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setParentInfo(pstNewInfo, pstSourceInfo->szCountryCode, pstSourceInfo->ucRate);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxParentInfo_t, freeParentInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_TELETEXT (TAG = 0x56)
 ************************************************************************/
static void initTeletextInfo(SIxTeletextInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo->szIso639LangCode, '\0', 4);
	pstInfo->ucType 			= 0;
	pstInfo->ucMagazineNum 	= 0;
	pstInfo->ucPageNum 		= 0;
	pstInfo->pstNext 		= NULL;
}

static SIxTeletextInfo_t *allocTeletextInfo(void)
{
	SIxTeletextInfo_t	*pstInfo;

	pstInfo = (SIxTeletextInfo_t *)silib_desc_alloc(sizeof(SIxTeletextInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocTeletextInfo()\n");
		return NULL;
	}
	initTeletextInfo(pstInfo);

	return ((SIxTeletextInfo_t *)pstInfo);
}

static void freeTeletextInfo(SIxTeletextInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setTeletextInfo(SIxTeletextInfo_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 ucType, HUINT8 ucMagazineNum, HUINT8 ucPageNum)
{
	HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstInfo->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	pstInfo->ucType 			= ucType;
	pstInfo->ucMagazineNum 	= ucMagazineNum;
	pstInfo->ucPageNum 		= ucPageNum;
}

static void initTeletextDes(SIxTeletextDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxTeletextDes_t *allocTeletextDes(void)
{
	return (SIxTeletextDes_t *)silib_desc_alloc(sizeof(SIxTeletextDes_t));
}

static void freeTeletextDes(SIxTeletextDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxTeletextInfo_t, freeTeletextInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseTeletextDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxTeletextDes_t *pstDes)
{
	SIxTeletextInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8				ucType, ucMagazineNum, ucPageNum;
	HUINT8				szIso639LangCode[4];
	HUINT8				ucTemp;
	HUINT16				usDesLen;

	HxLOG_Info("parseTeletextDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 5) != 0)
	{
		HxLOG_Print("[WARN] Unknown TeletextDes Format, or no teletext information!\n");
		return eSIxERR_INVALID;
	}
	else if (usDesLen == 0)
	{
		HxSTD_MemSet (szIso639LangCode, '\0', 4);
		ucType = 1;
		ucMagazineNum = ucPageNum = 0;

		/** < Pseudo Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxTeletextInfo_t, pstDes);

		pstNewInfo = allocTeletextInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setTeletextInfo (pstNewInfo, szIso639LangCode, ucType, ucMagazineNum, ucPageNum);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < teletext type & teletext magazine number (1byte) */
		ucTemp = *p++;
		usDesLen--;
		ucType = (HUINT8)((ucTemp & 0xf8) >> 3);
		ucMagazineNum = ucTemp & 0x07;
		HxLOG_Print("\t\t\t(teletext type = 0x%x)\n", ucType);
		HxLOG_Print("\t\t\t(teletext magazine number = 0x%x)\n", ucMagazineNum);

		/** < teletext page number (1byte) */
		ucPageNum = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(teletext page number = 0x%x)\n", ucPageNum);

		HxLOG_Print("\t\t\t(lang = %s)(type = 0x%x)(mag no = 0x%x)(page no = 0x%x)\n", szIso639LangCode, ucType, ucMagazineNum, ucPageNum);


		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxTeletextInfo_t, pstDes);

		pstNewInfo = allocTeletextInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		setTeletextInfo(pstNewInfo, szIso639LangCode, ucType, ucMagazineNum, ucPageNum);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseTeletextDes() Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxTeletextInfo_t, freeTeletextInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyTeletextDes(SIxTeletextDes_t *pstDest, const SIxTeletextDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTeletextDes_t));

		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxTeletextInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxTeletextInfo_t, pstDest);

				pstNewInfo = allocTeletextInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setTeletextInfo(pstNewInfo, pstSourceInfo->szIso639LangCode, pstSourceInfo->ucType, pstSourceInfo->ucMagazineNum, pstSourceInfo->ucPageNum);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxTeletextInfo_t, freeTeletextInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_TELEPHONE (TAG = 0x57)
 ************************************************************************/
static void initTelephoneDes(SIxTelephoneDes_t *pstDes)
{
	pstDes->ucForeignAvail 			= 0;
	pstDes->ucConnectionType 		= 0;
	pstDes->pszCountryPrefix 		= NULL;
	pstDes->pszInternationalAreaCode = NULL;
	pstDes->pszOperatorCode 		= NULL;
	pstDes->pszNationalAreaCode		= NULL;
	pstDes->pszCoreNumber 			= NULL;
}

static SIxTelephoneDes_t *allocTelephoneDes(void)
{
	return (SIxTelephoneDes_t *)silib_desc_alloc(sizeof(SIxTelephoneDes_t));
}

static void freeTelephoneDes(SIxTelephoneDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_free(pstDes->pszCountryPrefix);
	silib_desc_free(pstDes->pszInternationalAreaCode);
	silib_desc_free(pstDes->pszOperatorCode);
	silib_desc_free(pstDes->pszNationalAreaCode);
	silib_desc_free(pstDes->pszCoreNumber);
	silib_desc_free(pstDes);
}

static void setTelephoneDes(SIxTelephoneDes_t *pstDes, HUINT8 ucForeignAvail, HUINT8 ucConnectionType, HUINT8 *pszCountryPrefix, HUINT8 *pszInternationalAreaCode, HUINT8 *pszOperatorCode, HUINT8 *pszNationalAreaCode, HUINT8 *pszCoreNumber)
{
	pstDes->ucForeignAvail = ucForeignAvail;
	pstDes->ucConnectionType = ucConnectionType;
	if(pszCountryPrefix != NULL)
	    HxSTD_MemCopy(pstDes->pszCountryPrefix, pszCountryPrefix, silib_desc_util_dvbStrlen(pszCountryPrefix));
	pstDes->pszCountryPrefix[silib_desc_util_dvbStrlen(pszCountryPrefix)] = '\0';
	if(pszInternationalAreaCode != NULL)
	    HxSTD_MemCopy(pstDes->pszInternationalAreaCode, pszInternationalAreaCode, silib_desc_util_dvbStrlen(pszInternationalAreaCode));
	pstDes->pszInternationalAreaCode[silib_desc_util_dvbStrlen(pszInternationalAreaCode)] = '\0';
	if(pszOperatorCode != NULL)
	    HxSTD_MemCopy(pstDes->pszOperatorCode, pszOperatorCode, silib_desc_util_dvbStrlen(pszOperatorCode));
	pstDes->pszOperatorCode[silib_desc_util_dvbStrlen(pszOperatorCode)] = '\0';
	if(pszNationalAreaCode != NULL)
	    HxSTD_MemCopy(pstDes->pszNationalAreaCode, pszNationalAreaCode, silib_desc_util_dvbStrlen(pszNationalAreaCode));
	pstDes->pszNationalAreaCode[silib_desc_util_dvbStrlen(pszNationalAreaCode)] = '\0';
	if(pszCoreNumber != NULL)
	    HxSTD_MemCopy(pstDes->pszCoreNumber, pszCoreNumber, silib_desc_util_dvbStrlen(pszCoreNumber));
	pstDes->pszCoreNumber[silib_desc_util_dvbStrlen(pszCoreNumber)] = '\0';
}

static SIxError_e parseTelephoneDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxTelephoneDes_t *pstDes)
{
	HUINT8			ucForeignAvail, ucConnectionType;
	HUINT8			ucCountryPrefixLen, ucInterAreaCodeLen, ucOperatorCodeLen,
					ucNationalAreaCodeLen, ucCoreNumLen;
	HUINT8			*pszCountryPrefix = NULL,
					*pszInternationalAreaCode = NULL,
					*pszOperatorCode = NULL,
					*pszNationalAreaCode = NULL,
					*pszCoreNumber = NULL;
	HUINT8			ucTemp;
	HUINT16			usDesLen;
	HUINT32			nTempCountryPrefixLen, nTempInterAreaCodeLen, nTempOperatorCodeLen,
					nTempNationalAreaCodeLen, nTempCoreNumLen;

	HxLOG_Info("parseTelephoneDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	/** < foregin availability & connection type (1byte) */
	ucTemp = *p++;
	ucForeignAvail = (HUINT8)((ucTemp & 0x20) >> 5);
	ucConnectionType = ucTemp & 0x1f;
	HxLOG_Print("\t\t(foregin availability = 0x%x)\n", ucForeignAvail);
	HxLOG_Print("\t\t(connection type = 0x%x)\n", ucConnectionType);

	/** < country prefix length & international area code length & operator code length (1byte) */
	ucTemp = *p++;
	ucCountryPrefixLen = (HUINT8)((ucTemp & 0x60) >> 5);
	ucInterAreaCodeLen = (HUINT8)((ucTemp & 0x1c) >> 2);
	ucOperatorCodeLen = ucTemp & 0x03;
	HxLOG_Print("\t\t(country prefix length = %d)\n", ucCountryPrefixLen);
	HxLOG_Print("\t\t(international area code length = %d)\n", ucInterAreaCodeLen);
	HxLOG_Print("\t\t(operator code length = %d)\n", ucOperatorCodeLen);

	/** < national area code length & core number length (1byte) */
	ucTemp = *p++;
	ucNationalAreaCodeLen = (HUINT8)((ucTemp & 0x70) >> 4);
	ucCoreNumLen = ucTemp & 0x0f;
	HxLOG_Print("\t\t(national area code length = %d)\n", ucNationalAreaCodeLen);
	HxLOG_Print("\t\t(core number length = %d)\n", ucCoreNumLen);

	/** < country frefix char */
	nTempCountryPrefixLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucCountryPrefixLen, (const HUINT8 *)p, &pszCountryPrefix);
	p += ucCountryPrefixLen;
	usDesLen -= ucCountryPrefixLen;
	HxLOG_Print("\t\t(country frefix char = %s)\n", pszCountryPrefix);

	/** < international area code char */
	nTempInterAreaCodeLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucInterAreaCodeLen, (const HUINT8 *)p, &pszInternationalAreaCode);
	p += ucInterAreaCodeLen;
	usDesLen -= ucInterAreaCodeLen;
	HxLOG_Print("\t\t(international area code char = %s)\n", pszInternationalAreaCode);

	/** < operator code char */
	nTempOperatorCodeLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucOperatorCodeLen, (const HUINT8 *)p, &pszOperatorCode);
	p += ucOperatorCodeLen;
	usDesLen -= ucOperatorCodeLen;
	HxLOG_Print("\t\t(operator code char = %s)\n", pszOperatorCode);

	/** < national area code char */
	nTempNationalAreaCodeLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucNationalAreaCodeLen, (const HUINT8 *)p, &pszNationalAreaCode);
	p += ucNationalAreaCodeLen;
	usDesLen -= ucNationalAreaCodeLen;
	HxLOG_Print("\t\t(national area code char = %s)\n", pszNationalAreaCode);

	/** < core number char */
	nTempCoreNumLen = silib_desc_util_textToUTF8((const HCHAR *)NULL, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucCoreNumLen, (const HUINT8 *)p, &pszCoreNumber);
	/** <p += ucCoreNumLen; */
	/** <usDesLen -= ucCoreNumLen; */
	HxLOG_Print("\t\t(core number char = %s)\n", pszCoreNumber);

	/** < Descriptor 데이타 구성. */
	pstDes->pszCountryPrefix = silib_desc_util_allocStr(nTempCountryPrefixLen+1);
	if (pstDes->pszCountryPrefix == NULL)
		goto err_exit;
	pstDes->pszInternationalAreaCode = silib_desc_util_allocStr(nTempInterAreaCodeLen+1);
	if (pstDes->pszInternationalAreaCode == NULL)
		goto err_exit;
	pstDes->pszOperatorCode = silib_desc_util_allocStr(nTempOperatorCodeLen+1);
	if (pstDes->pszOperatorCode == NULL)
		goto err_exit;
	pstDes->pszNationalAreaCode = silib_desc_util_allocStr(nTempNationalAreaCodeLen+1);
	if (pstDes->pszNationalAreaCode == NULL)
		goto err_exit;
	pstDes->pszCoreNumber = silib_desc_util_allocStr(nTempCoreNumLen+1);
	if (pstDes->pszCoreNumber == NULL)
		goto err_exit;

	setTelephoneDes(pstDes, ucForeignAvail, ucConnectionType, pszCountryPrefix, pszInternationalAreaCode, pszOperatorCode, pszNationalAreaCode, pszCoreNumber);

	silib_desc_free(pszCountryPrefix);
	silib_desc_free(pszInternationalAreaCode);
	silib_desc_free(pszOperatorCode);
	silib_desc_free(pszNationalAreaCode);
	silib_desc_free(pszCoreNumber);

	HxLOG_Info(" parseTelephoneDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	silib_desc_free(pszCountryPrefix);
	silib_desc_free(pszInternationalAreaCode);
	silib_desc_free(pszOperatorCode);
	silib_desc_free(pszNationalAreaCode);
	silib_desc_free(pszCoreNumber);

	silib_desc_free(pstDes->pszCountryPrefix);
	silib_desc_free(pstDes->pszInternationalAreaCode);
	silib_desc_free(pstDes->pszOperatorCode);
	silib_desc_free(pstDes->pszNationalAreaCode);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyTelephoneDes(SIxTelephoneDes_t *pstDest, const SIxTelephoneDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HUINT32		ulSize;

		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTelephoneDes_t));
		pstDest->pszCountryPrefix			= NULL;
		pstDest->pszInternationalAreaCode	= NULL;
		pstDest->pszOperatorCode			= NULL;
		pstDest->pszNationalAreaCode		= NULL;
		pstDest->pszCoreNumber				= NULL;

		if (pstSource->pszCountryPrefix)
		{
			ulSize = silib_desc_util_dvbStrlen(pstSource->pszCountryPrefix);
			if (0 < ulSize)
			{
				pstDest->pszCountryPrefix = silib_desc_util_allocStr(ulSize+1);
				if (NULL == pstDest->pszCountryPrefix)
					goto err_exit;
				HxSTD_MemCopy(pstDest->pszCountryPrefix, pstSource->pszCountryPrefix, sizeof(HUINT8)*ulSize);
				pstDest->pszCountryPrefix[ulSize] = '\0';
			}
		}

		if (pstSource->pszInternationalAreaCode)
		{
			ulSize = silib_desc_util_dvbStrlen(pstSource->pszInternationalAreaCode);
			if (0 < ulSize)
			{
				pstDest->pszInternationalAreaCode = silib_desc_util_allocStr(ulSize+1);
				if (NULL == pstDest->pszInternationalAreaCode)
					goto err_exit;
				HxSTD_MemCopy(pstDest->pszInternationalAreaCode, pstSource->pszInternationalAreaCode, sizeof(HUINT8)*ulSize);
				pstDest->pszInternationalAreaCode[ulSize] = '\0';
			}
		}

		if (pstSource->pszOperatorCode)
		{
			ulSize = silib_desc_util_dvbStrlen(pstSource->pszOperatorCode);
			if (0 < ulSize)
			{
				pstDest->pszOperatorCode = silib_desc_util_allocStr(ulSize+1);
				if (NULL == pstDest->pszOperatorCode)
					goto err_exit;
				HxSTD_MemCopy(pstDest->pszOperatorCode, pstSource->pszOperatorCode, sizeof(HUINT8)*ulSize);
				pstDest->pszOperatorCode[ulSize] = '\0';
			}
		}

		if (pstSource->pszNationalAreaCode)
		{
			ulSize = silib_desc_util_dvbStrlen(pstSource->pszNationalAreaCode);
			if (0 < ulSize)
			{
				pstDest->pszNationalAreaCode = silib_desc_util_allocStr(ulSize+1);
				if (NULL == pstDest->pszNationalAreaCode)
					goto err_exit;
				HxSTD_MemCopy(pstDest->pszNationalAreaCode, pstSource->pszNationalAreaCode, sizeof(HUINT8)*ulSize);
				pstDest->pszNationalAreaCode[ulSize] = '\0';
			}
		}

		if (pstSource->pszCoreNumber)
		{
			ulSize = silib_desc_util_dvbStrlen(pstSource->pszCoreNumber);
			if (0 < ulSize)
			{
				pstDest->pszCoreNumber = silib_desc_util_allocStr(ulSize+1);
				if (NULL == pstDest->pszCoreNumber)
					goto err_exit;
				HxSTD_MemCopy(pstDest->pszCoreNumber, pstSource->pszCoreNumber, sizeof(HUINT8)*ulSize);
				pstDest->pszCoreNumber[ulSize] = '\0';
			}
		}
	}

	return eErr;

err_exit:
	silib_desc_free(pstDest->pszCountryPrefix);
	silib_desc_free(pstDest->pszInternationalAreaCode);
	silib_desc_free(pstDest->pszOperatorCode);
	silib_desc_free(pstDest->pszNationalAreaCode);
	silib_desc_free(pstDest->pszCoreNumber);
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_LOCAL_TIME_OFFSET (TAG = 0x58)
 ************************************************************************/
static void initLocalTimeInfo(SIxLocalTimeInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo, 0, sizeof(SIxLocalTimeInfo_t));
}

static SIxLocalTimeInfo_t *allocLocalTimeInfo(void)
{
	SIxLocalTimeInfo_t	*pstInfo;


	pstInfo = (SIxLocalTimeInfo_t *)silib_desc_alloc(sizeof(SIxLocalTimeInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocLocalTimeInfo()\n");
		return NULL;
	}
	initLocalTimeInfo(pstInfo);

	return ((SIxLocalTimeInfo_t *)pstInfo);
}

static void freeLocalTimeInfo(SIxLocalTimeInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setLocalTimeInfo(SIxLocalTimeInfo_t *pstInfo, HUINT8 *pszCountryCode, HUINT8 ucRegionId, HUINT8 ucLocalTimeOffPolar, HUINT8 *pucLocalTimeOff, UNIXTIME timeOfChange, HUINT8 *pucNextTimeOffset)
{
	HxSTD_MemCopy(pstInfo->szCountryCode, pszCountryCode, 3);
	pstInfo->szCountryCode[3] = '\0';
	pstInfo->ucRegionId 			= ucRegionId;
	pstInfo->ucLocalTimeOffPolar = ucLocalTimeOffPolar;
	HxSTD_MemCopy (pstInfo->pucLocalTimeOff, pucLocalTimeOff, 2);
	pstInfo->timeOfChange 		= timeOfChange;
	HxSTD_MemCopy (pstInfo->pucNextTimeOffset, pucNextTimeOffset, 2);
}

static void initLocalTimeDes(SIxLocalTimeDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxLocalTimeDes_t *allocLocalTimeDes(void)
{
	return (SIxLocalTimeDes_t *)silib_desc_alloc(sizeof(SIxLocalTimeDes_t));
}

static void freeLocalTimeDes(SIxLocalTimeDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxLocalTimeInfo_t, freeLocalTimeInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseLocalTimeDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxLocalTimeDes_t *pstDes)
{
/** 아래 time 구하는 부분 추가후 다시 정리 하기 일단 임시로 막아 놓음. hcyoo */
	SIxLocalTimeInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8			 ucDesLen;

	HUINT8		szCountryCode[4];
	HUINT8		ucRegionId;
	HUINT8		ucLocalTimeOffPolar;
	HUINT8		pucLocalTimeOff[2];
	HUINT8		szTimeOfChange[5];
	HUINT8		pucNextTimeOffset[2];
	UNIXTIME	timeOfChange ;


	HxLOG_Info("INT_SI_PARSE_LocalTimeOffsetDes() Entered!");
	HxLOG_Print("\t\t(descriptor tag = 0x%x , LocalTimeOffsetDes)\n", *p);
	p++;
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	while (ucDesLen > 0 ) {

		HxSTD_MemCopy(szCountryCode, p, 3);
		szCountryCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szCountryCode);
		p += 3;
		ucDesLen -= 3;
		HxLOG_Print("\t\t\t(country_code = %s)\n", szCountryCode);

		/* TOT 잘못 Parsing 하는것 수정 : 2004.10.06 By Moong */
		ucRegionId = ((*p >> 2) & 0x3f);	/** < 6bits */
		ucLocalTimeOffPolar = (HUINT8)(*p & 0x01);

		p++;
		ucDesLen--;
		HxLOG_Print("\t\t\t(local_time_offset_polarity = %d\n", ucLocalTimeOffPolar);

		HxSTD_MemCopy (pucLocalTimeOff, p, 2);
		p += 2;
		ucDesLen -= 2;
		HxLOG_Print("\t\t\t(local_time_offset = %x%02x\n", pucLocalTimeOff[0], pucLocalTimeOff[1]);

		HxSTD_MemCopy(szTimeOfChange, p, 5);
		p += 5;
		ucDesLen -= 5;
		HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(szTimeOfChange, &timeOfChange) ;

		HxLOG_Print("\t\t\t(time_of_change_date = %02X %02X %02X %02X %02X\n", szTimeOfChange[0], szTimeOfChange[1], szTimeOfChange[2], szTimeOfChange[3], szTimeOfChange[4]);
		HxSTD_MemCopy (pucNextTimeOffset, p, 2);
		p += 2;
		ucDesLen -= 2;
		HxLOG_Print("\t\t\t(next_time_offset = %x%02x\n", pucNextTimeOffset[0], pucNextTimeOffset[1]);

		GET_PREV_INFO(pstPrevInfo, SIxLocalTimeInfo_t, pstDes);
		pstNewInfo = allocLocalTimeInfo();

		if (pstNewInfo == NULL)
			goto err_exit;

		setLocalTimeInfo( pstNewInfo, szCountryCode, ucRegionId, ucLocalTimeOffPolar, pucLocalTimeOff, timeOfChange, pucNextTimeOffset);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" INT_SI_PARSE_LocalTimeOffsetDes() Leaved!");
	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxLocalTimeInfo_t, freeLocalTimeInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyLocalTimeDes(SIxLocalTimeDes_t *pstDest, const SIxLocalTimeDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxLocalTimeDes_t));

		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxLocalTimeInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxLocalTimeInfo_t, pstDest);
				pstNewInfo = allocLocalTimeInfo();

				if (NULL == pstNewInfo)
					goto err_exit;

				setLocalTimeInfo(pstNewInfo, pstSourceInfo->szCountryCode, pstSourceInfo->ucRegionId,
												pstSourceInfo->ucLocalTimeOffPolar, pstSourceInfo->pucLocalTimeOff,
												pstSourceInfo->timeOfChange, pstSourceInfo->pucNextTimeOffset);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxLocalTimeInfo_t, freeLocalTimeInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_SUBTITLING (TAG = 0x59)
 ************************************************************************/
static void initSubtitleInfo(SIxSubtitleInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo->szIso639LangCode, '\0', 4);
	pstInfo->ucSubtitleType 		= 0;
	pstInfo->usCompositionPageId 	= 0;
	pstInfo->usAncillaryPageId 	= 0;
	pstInfo->pstNext 			= NULL;
}

static SIxSubtitleInfo_t *allocSubtitleInfo(void)
{
	SIxSubtitleInfo_t	*pstInfo;


	pstInfo = (SIxSubtitleInfo_t *)silib_desc_alloc(sizeof(SIxSubtitleInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocSubtitleInfo()\n");
		return NULL;
	}
	initSubtitleInfo(pstInfo);

	return ((SIxSubtitleInfo_t *)pstInfo);
}

static void freeSubtitleInfo(SIxSubtitleInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void setSubtitleInfo(SIxSubtitleInfo_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 ucSubtitleType, HUINT16 usCompositionPageId, HUINT16 usAncillaryPageId)
{
	HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstInfo->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	pstInfo->ucSubtitleType 		= ucSubtitleType;
	pstInfo->usCompositionPageId 	= usCompositionPageId;
	pstInfo->usAncillaryPageId 	= usAncillaryPageId;
}

static void initSubtitleDes(SIxSubtitleDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxSubtitleDes_t *allocSubtitleDes(void)
{
	return (SIxSubtitleDes_t *)silib_desc_alloc(sizeof(SIxSubtitleDes_t));
}

static void freeSubtitleDes(SIxSubtitleDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxSubtitleInfo_t, freeSubtitleInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseSubtitleDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSubtitleDes_t *pstDes)
{
	SIxSubtitleInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8				ucSubtitleType;
	HUINT16				usDesLen;
	HUINT16				usCompositionPageId, usAncillaryPageId;
	HUINT8				szIso639LangCode[4];

	HxLOG_Info("parseSubtitleDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 8) != 0)
	{
		HxLOG_Print("[WARN] Unknown SubtitleDes Format!\n");
		return eSIxERR_INVALID;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < subtitle type (1byte) */
		ucSubtitleType = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(subtitle type = 0x%x)\n", ucSubtitleType);

		/** < compsition page id (2byte) */
		usCompositionPageId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t\t(compsition page id = 0x%x)\n", usCompositionPageId);

		/** < ancillary page id (2byte) */
		usAncillaryPageId = HxMACRO_Get16Bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t\t(ancillary page id = 0x%x)\n", usAncillaryPageId);
		HxLOG_Print("\t\t\t(lang = %s)(type = 0x%x)(comp page id = 0x%x)(acil page id = 0x%x)\n", szIso639LangCode, ucSubtitleType, usCompositionPageId, usAncillaryPageId);


		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxSubtitleInfo_t, pstDes);
		pstNewInfo = allocSubtitleInfo();

		if (pstNewInfo == NULL)
			goto err_exit;

		setSubtitleInfo(pstNewInfo, szIso639LangCode, ucSubtitleType, usCompositionPageId, usAncillaryPageId);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseSubtitleDes() Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxSubtitleInfo_t, freeSubtitleInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copySubtitleDes(SIxSubtitleDes_t *pstDest, const SIxSubtitleDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSubtitleDes_t));

		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxSubtitleInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxSubtitleInfo_t, pstDest);
				pstNewInfo = allocSubtitleInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setSubtitleInfo(pstNewInfo, pstSourceInfo->szIso639LangCode, pstSourceInfo->ucSubtitleType,
											pstSourceInfo->usCompositionPageId, pstSourceInfo->usAncillaryPageId);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxSubtitleInfo_t, freeSubtitleInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM (TAG = 0x5A)
 ************************************************************************/
static void initTerDeliSystemDes(SIxTerDeliSystemDes_t *pstDes)
{
	pstDes->ulFreq 				= 0;
	pstDes->ucBandWidth 			= 0;
	pstDes->ucPriority			= 0;
	pstDes->ucConstellation	 	= 0;
	pstDes->ucHierInfo 			= 0;
	pstDes->ucCodeRateHPStream 	= 0;
	pstDes->ucCodeRateLPStream 	= 0;
	pstDes->ucGuardInterval 	= 0;
	pstDes->ucTransmissionMode 	= 0;
	pstDes->bOtherFreqFlag 		= FALSE;
}

static SIxTerDeliSystemDes_t *allocTerDeliSystemDes(void)
{
	return (SIxTerDeliSystemDes_t *)silib_desc_alloc(sizeof(SIxTerDeliSystemDes_t));
}

static void freeTerDeliSystemDes(SIxTerDeliSystemDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setTerDeliSystemDes(SIxTerDeliSystemDes_t *pstDes, HUINT32 ulFreq
	, HUINT8 ucBandWidth, HUINT8 ucPriority, HUINT8 ucConstellation, HUINT8 ucHierInfo
	, HUINT8 ucCodeRateHPStream, HUINT8 ucCodeRateLPStream, HUINT8 ucGuardInterval
	, HUINT8 ucTransmissionMode, HBOOL bOtherFreqFlag)
{
	pstDes->ulFreq 				= ulFreq;
	pstDes->ucBandWidth 			= ucBandWidth;
	pstDes->ucPriority			= ucPriority;
	pstDes->ucConstellation	 	= ucConstellation;
	pstDes->ucHierInfo 			= ucHierInfo;
	pstDes->ucCodeRateHPStream 	= ucCodeRateHPStream;
	pstDes->ucCodeRateLPStream 	= ucCodeRateLPStream;
	pstDes->ucGuardInterval 		= ucGuardInterval;
	pstDes->ucTransmissionMode 	= ucTransmissionMode;
	pstDes->bOtherFreqFlag 		= bOtherFreqFlag;
}

static SIxError_e parseTerDeliSystemDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxTerDeliSystemDes_t *pstDes)
{
	HUINT32				ulTemp;
	HUINT32				ulFreq;
	HUINT16				usDesLen;
	HUINT8				ucBandWidth, ucPriority, ucConstellation, ucHierInfo, ucCodeRateHPStream;
	HUINT8				ucCodeRateLPStream, ucGuardInterval, ucTransmissionMode;
	HUINT8				ucTemp, ucVal;
	HBOOL 				bOtherFreqFlag;

	HxLOG_Info("parseTerDeliSystemDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 11)
	{
		HxLOG_Print("[WARN] Unknown parseTerDeliSystemDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < center-frequency (4byte) */
	ulFreq = HxMACRO_Get32Bit(p);

	p += 4;
	usDesLen -= 4;
	HxLOG_Print("\t\t(frequency = %d)\n", ulFreq/100);

	/** < bandwidth (3-bits) */
	ucTemp = *p++;
	usDesLen -=1;
	ucBandWidth = (HUINT8)((ucTemp & 0xe0) >> 5);
	HxLOG_Print("\t\t(bandwidth = 0x%x)\n", ucBandWidth);

	ucPriority = (HUINT8) (((ucTemp & 0x10) != 0) ? 1 : 0);
	HxLOG_Print("\t\t(Priority = 0x%x)\n", ucBandWidth);

	/** < constellation & hierarchy_information & code_rate-HP_stream (1byte) */
	ucTemp = *p++;
	usDesLen -=1;
	ucConstellation = (HUINT8)((ucTemp & 0xc0) >> 6);
	ucHierInfo = (HUINT8)((ucTemp & 0x38) >> 3);
	ucCodeRateHPStream = (ucTemp & 0x07);
	HxLOG_Print("\t\t(constellation = 0x%x)\n", ucConstellation);
	HxLOG_Print("\t\t(hierarchy information = 0x%x)\n", ucHierInfo);
	HxLOG_Print("\t\t(code-rate-HP stream = 0x%x)\n", ucCodeRateHPStream);

	/** < code_rate-LP_stream & guard_interval & transmission_mode & other_frequency_flag (1 byte) */
	ucTemp = *p++;
	usDesLen -=1;
	ucCodeRateLPStream = (HUINT8)((ucTemp & 0xe0) >> 5);
	ucGuardInterval = (HUINT8)((ucTemp & 0x18) >> 3);
	ucTransmissionMode = (HUINT8)((ucTemp & 0x06) >> 1);
	ucVal = (ucTemp & 0x01);
	(ucVal == 1) ? (bOtherFreqFlag = TRUE) : (bOtherFreqFlag = FALSE);

	HxLOG_Print("\t\t(code-rate-LP stream = 0x%x)\n", ucCodeRateLPStream);
	HxLOG_Print("\t\t(guard interval = 0x%x)\n", ucGuardInterval);
	HxLOG_Print("\t\t(transmission mode = 0x%x)\n", ucTransmissionMode);
	HxLOG_Print("\t\t(other frequency flag = 0x%x)\n", bOtherFreqFlag);

	/** < reserved (4byte) */
	ulTemp = HxMACRO_Get32Bit(p);
	usDesLen -=4;
	HxLOG_Print("\t\t(reserved = 0x%x)\n", ulTemp);

	if (usDesLen != 0)
		HxLOG_Print("[desparse] Something is wrong. Length is not zero.\n");

	/** < Descriptor 데이타 구성. */
	setTerDeliSystemDes(pstDes, ulFreq, ucBandWidth, ucPriority, ucConstellation, ucHierInfo,
						ucCodeRateHPStream, ucCodeRateLPStream, ucGuardInterval, ucTransmissionMode, bOtherFreqFlag);

	HxLOG_Info(" parseTerDeliSystemDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyTerDeliSystemDes(SIxTerDeliSystemDes_t *pstDest, const SIxTerDeliSystemDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxTerDeliSystemDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME (TAG = 0x5B)
 ************************************************************************/
static void initMultiNetNameInfo(SIxMultiNetNameInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo->szIso639LangCode, '\0', 4);
	pstInfo->pszNetName 	= NULL;
	pstInfo->pstNext	= NULL;
}

static SIxMultiNetNameInfo_t *allocMultiNetNameInfo(void)
{
	SIxMultiNetNameInfo_t	*pstInfo;


	pstInfo = (SIxMultiNetNameInfo_t *)silib_desc_alloc(sizeof(SIxMultiNetNameInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiNetNameInfo()\n");
		return NULL;
	}
	initMultiNetNameInfo(pstInfo);

	return ((SIxMultiNetNameInfo_t *)pstInfo);
}

static void freeMultiNetNameInfo(SIxMultiNetNameInfo_t *pstInfo)
{
	if (NULL == pstInfo)		return;

	silib_desc_free(pstInfo->pszNetName);
	silib_desc_free(pstInfo);
}

static void setMultiNetNameInfo(SIxMultiNetNameInfo_t *pstInfo
	, HUINT8 *pszIso639LangCode, HUINT8 *pszNetName)
{
    if(pszIso639LangCode != NULL)
	    HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstInfo->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	if(pszNetName != NULL)
	    HxSTD_MemCopy(pstInfo->pszNetName, pszNetName, silib_desc_util_dvbStrlen(pszNetName));
	pstInfo->pszNetName[silib_desc_util_dvbStrlen(pszNetName)] = '\0';
}

static void initMultiNetNameDes(SIxMultiNetNameDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxMultiNetNameDes_t *allocMultiNetNameDes(void)
{
	return (SIxMultiNetNameDes_t *)silib_desc_alloc(sizeof(SIxMultiNetNameDes_t));
}

static void freeMultiNetNameDes(SIxMultiNetNameDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMultiNetNameInfo_t, freeMultiNetNameInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseMultiNetNameDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxMultiNetNameDes_t *pstDes)
{
	SIxMultiNetNameInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8				ucNetNameLen;
	HUINT8				szIso639LangCode[4], *pszNetName = NULL;
	HUINT16				usDesLen;
	HUINT32				nTempNetNameLen;

	HxLOG_Info("parseMultiNetNameDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < network name length */
		ucNetNameLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(network name length = %d)\n", ucNetNameLen);

		/** < network name */
		nTempNetNameLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucNetNameLen, (const HUINT8 *)p, &pszNetName);
		p += ucNetNameLen;
		usDesLen -= ucNetNameLen;
		HxLOG_Print("\t\t\t(network name = %s)\n", pszNetName);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxMultiNetNameInfo_t, pstDes);
		pstNewInfo = allocMultiNetNameInfo();

		if (pstNewInfo == NULL)
			goto err_exit;
		pstNewInfo->pszNetName = silib_desc_util_allocStr(nTempNetNameLen+1);
		if (pstNewInfo->pszNetName == NULL)
			goto err_exit;

		setMultiNetNameInfo(pstNewInfo, szIso639LangCode, pszNetName);
		silib_desc_free(pszNetName);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseMultiNetNameDes() Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMultiNetNameInfo_t, freeMultiNetNameInfo);
		pstDes->pstInfo = NULL;
	}
	silib_desc_free(pszNetName);
	silib_desc_freeInfo_Dvb(eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME, 0, (HUINT8 *)pstNewInfo);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyMultiNetNameDes(SIxMultiNetNameDes_t *pstDest, const SIxMultiNetNameDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMultiNetNameDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxMultiNetNameInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxMultiNetNameInfo_t, pstDest);
				pstNewInfo = allocMultiNetNameInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setMultiNetNameInfo(pstNewInfo, pstSourceInfo->szIso639LangCode, pstSourceInfo->pszNetName);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxMultiNetNameInfo_t, freeMultiNetNameInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME (TAG = 0x5C)
 ************************************************************************/
static void initMultiBouqNameInfo(SIxMultiBouqNameInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo->szIso639LangCode, '\0', 4);
	pstInfo->pszBouqName = NULL;
	pstInfo->pstNext	= NULL;
}

static SIxMultiBouqNameInfo_t *allocMultiBouqNameInfo(void)
{
	SIxMultiBouqNameInfo_t	*pstInfo;


	pstInfo = (SIxMultiBouqNameInfo_t *)silib_desc_alloc(sizeof(SIxMultiBouqNameInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiBouqNameInfo()\n");
		return NULL;
	}
	initMultiBouqNameInfo(pstInfo);

	return ((SIxMultiBouqNameInfo_t *)pstInfo);
}

static void freeMultiBouqNameInfo(SIxMultiBouqNameInfo_t *pstInfo)
{
	if (NULL == pstInfo)		return;

	silib_desc_free(pstInfo->pszBouqName);
	silib_desc_free(pstInfo);
}

static void setMultiBouqNameInfo(SIxMultiBouqNameInfo_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszBouqName)
{
    if(pszIso639LangCode != NULL)
    	HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstInfo->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	if(pszBouqName != NULL)
    	HxSTD_MemCopy(pstInfo->pszBouqName, pszBouqName, silib_desc_util_dvbStrlen(pszBouqName));
	pstInfo->pszBouqName[silib_desc_util_dvbStrlen(pszBouqName)] = '\0';
}

static void initMultiBouqNameDes(SIxMultiBouqNameDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxMultiBouqNameDes_t *allocMultiBouqNameDes(void)
{
	return (SIxMultiBouqNameDes_t *)silib_desc_alloc(sizeof(SIxMultiBouqNameDes_t));
}

static void freeMultiBouqNameDes(SIxMultiBouqNameDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMultiBouqNameInfo_t,  freeMultiBouqNameInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseMultiBouqNameDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxMultiBouqNameDes_t *pstDes)
{
	SIxMultiBouqNameInfo_t		*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8					ucBouqNameLen;
	HUINT8					szIso639LangCode[4], *pszBouqName = NULL;
	HUINT16					usDesLen;
	HUINT32					ulTempBouqNameLen;

	HxLOG_Info("parseMultiBouqNameDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < bouquet name length */
		ucBouqNameLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(bouquet name length = %d)\n", ucBouqNameLen);

		/** < bouquet name */
		ulTempBouqNameLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucBouqNameLen, (const HUINT8 *)p, &pszBouqName);
		p += ucBouqNameLen;
		usDesLen -= ucBouqNameLen;
		HxLOG_Print("\t\t\t(Bouqwork name = %s)\n", pszBouqName);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxMultiBouqNameInfo_t, pstDes);
		pstNewInfo = allocMultiBouqNameInfo();
		if (pstNewInfo == NULL)
			goto err_exit;
		pstNewInfo->pszBouqName = silib_desc_util_allocStr(ulTempBouqNameLen+1);
		if (pstNewInfo->pszBouqName == NULL)
		{
			silib_desc_free(pstNewInfo);
			goto err_exit;
		}

		setMultiBouqNameInfo(pstNewInfo, szIso639LangCode, pszBouqName);
		silib_desc_free(pszBouqName);
		pszBouqName = NULL;
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseMultiBouqNameDes() Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMultiBouqNameInfo_t,  freeMultiBouqNameInfo);
		pstDes->pstInfo = NULL;
	}
	silib_desc_free(pszBouqName);
	silib_desc_freeInfo_Dvb(eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME, 0, (HUINT8 *)pstNewInfo);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyMultiBouqNameDes(SIxMultiBouqNameDes_t *pstDest, const SIxMultiBouqNameDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMultiBouqNameDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxMultiBouqNameInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxMultiBouqNameInfo_t, pstDest);
				pstNewInfo = allocMultiBouqNameInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				if (pstSourceInfo->pszBouqName)
				{
					HUINT32	ulSize = silib_desc_util_dvbStrlen(pstSourceInfo->pszBouqName);
					pstNewInfo->pszBouqName = silib_desc_util_allocStr(ulSize+1);
					if (pstNewInfo->pszBouqName == NULL)
					{
						silib_desc_free(pstNewInfo);
						goto err_exit;
					}
				}

				setMultiBouqNameInfo(pstNewInfo, pstSourceInfo->szIso639LangCode, pstSourceInfo->pszBouqName);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxMultiBouqNameInfo_t,  freeMultiBouqNameInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME (TAG = 0x5D)
 ************************************************************************/
static void initMultiSvcNameInfo(SIxMultiSvcNameInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo->szIso639LangCode, '\0', 4);
	pstInfo->pszServProviderName = NULL;
	pstInfo->pszServName 		= NULL;
	pstInfo->pstNext			= NULL;
}

static SIxMultiSvcNameInfo_t *allocMultiSvcNameInfo(void)
{
	SIxMultiSvcNameInfo_t	*pstInfo;

	pstInfo = (SIxMultiSvcNameInfo_t *)silib_desc_alloc(sizeof(SIxMultiSvcNameInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiSvcNameInfo()\n");
		return NULL;
	}
	initMultiSvcNameInfo(pstInfo);

	return ((SIxMultiSvcNameInfo_t *)pstInfo);
}

static void freeMultiSvcNameInfo(SIxMultiSvcNameInfo_t *pstInfo)
{
	if (NULL == pstInfo)		return;

	silib_desc_free(pstInfo->pszServProviderName);
	silib_desc_free(pstInfo->pszServName);
	silib_desc_free(pstInfo);
}

static void setMultiSvcNameInfo(SIxMultiSvcNameInfo_t *pstInfo
	, HUINT8 *pszIso639LangCode, HUINT8 *pszSvcProviderName, HUINT8 *pszSvcName)
{
	HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode
		, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstInfo->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	HxSTD_MemCopy(pstInfo->pszServProviderName, pszSvcProviderName
		, silib_desc_util_dvbStrlen(pszSvcProviderName));
	pstInfo->pszServProviderName[silib_desc_util_dvbStrlen(pszSvcProviderName)] = '\0';
	HxSTD_MemCopy(pstInfo->pszServName, pszSvcName
		, silib_desc_util_dvbStrlen(pszSvcName));
	pstInfo->pszServName[silib_desc_util_dvbStrlen(pszSvcName)] = '\0';
}

static void initMultiSvcNameDes(SIxMultiSvcNameDes_t *pstDes)
{
	pstDes->pstInfo = NULL;
}

static SIxMultiSvcNameDes_t *allocMultiSvcNameDes(void)
{
	return (SIxMultiSvcNameDes_t *)silib_desc_alloc(sizeof(SIxMultiSvcNameDes_t));
}

static void freeMultiSvcNameDes(SIxMultiSvcNameDes_t *pstDes)
{
	if (NULL == pstDes)	return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMultiSvcNameInfo_t, freeMultiSvcNameInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseMultiSvcNameDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxMultiSvcNameDes_t *pstDes)
{
	SIxMultiSvcNameInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8					ucSvcProviderNameLen, ucSvcNameLen;
	HUINT8					szIso639LangCode[4], *pszSvcProviderName = NULL, *pszSvcName = NULL;
	HUINT16					usDesLen;
	HUINT32					nTempSvcProviderNameLen, nTempSvcNameLen;

	HxLOG_Info("parseMultiSvcNameDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < service provider name length */
		ucSvcProviderNameLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(service provider name length = %d)\n", ucSvcProviderNameLen);

		/** < service provider name */
		pszSvcProviderName = NULL;
		nTempSvcProviderNameLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucSvcProviderNameLen, (const HUINT8 *)p, &pszSvcProviderName);
		p += ucSvcProviderNameLen;
		usDesLen -= ucSvcProviderNameLen;
		HxLOG_Print("\t\t\t(service provider name = %s)\n", pszSvcProviderName ? (char *)pszSvcProviderName : "Null");
		if (pszSvcProviderName == NULL)
			goto err_exit;

		/** < service name length */
		ucSvcNameLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(service name length = %d)\n", ucSvcNameLen);

		/** < service name */
		pszSvcName = NULL;
		nTempSvcNameLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, (HUINT32)ucSvcNameLen, (const HUINT8 *)p, &pszSvcName);
		p += ucSvcNameLen;
		usDesLen -= ucSvcNameLen;
		HxLOG_Print("\t\t\t(service name = %s)\n", pszSvcName ? (char *)pszSvcName : "Null");
		if (pszSvcName == NULL)
			goto err_exit;

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxMultiSvcNameInfo_t, pstDes);
		pstNewInfo = allocMultiSvcNameInfo();
		if (pstNewInfo == NULL)
			goto err_exit;
		pstNewInfo->pszServProviderName = silib_desc_util_allocStr(nTempSvcProviderNameLen+1);
		if (pstNewInfo->pszServProviderName == NULL)
			goto err_exit;
		pstNewInfo->pszServName = silib_desc_util_allocStr(nTempSvcNameLen+1);
		if (pstNewInfo->pszServName == NULL)
			goto err_exit;

		setMultiSvcNameInfo(pstNewInfo, szIso639LangCode, pszSvcProviderName, pszSvcName);
		silib_desc_free(pszSvcProviderName);
		silib_desc_free(pszSvcName);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
		pstNewInfo = NULL;
	}

	HxLOG_Info(" parseMultiSvcNameDes() Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMultiSvcNameInfo_t, freeMultiSvcNameInfo);
		pstDes->pstInfo = NULL;
	}

	silib_desc_free(pszSvcProviderName);
	silib_desc_free(pszSvcName);

	if (pstNewInfo != NULL)
	{
		silib_desc_free(pstNewInfo->pszServProviderName);
		silib_desc_freeInfo_Dvb(eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME, 0, (HUINT8 *)pstNewInfo);
	}

	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyMultiSvcNameDes(SIxMultiSvcNameDes_t *pstDest, const SIxMultiSvcNameDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMultiSvcNameDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxMultiSvcNameInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxMultiSvcNameInfo_t, pstDest);
				pstNewInfo = allocMultiSvcNameInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				setMultiSvcNameInfo(pstNewInfo, pstSourceInfo->szIso639LangCode, pstSourceInfo->pszServProviderName, pstSourceInfo->pszServName);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxMultiSvcNameInfo_t, freeMultiSvcNameInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_MULTILINGUAL_COMPONENT (TAG = 0x5E)
 ************************************************************************/
static void initMultiCompInfo(SIxMultiCompInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo->szIso639LangCode, '\0', 4);
	pstInfo->pszTextDes 	= NULL;
	pstInfo->pstNext	= NULL;
}

static SIxMultiCompInfo_t *allocMultiCompInfo(void)
{
	SIxMultiCompInfo_t	*pstInfo;

	pstInfo = (SIxMultiCompInfo_t *)silib_desc_alloc(sizeof(SIxMultiCompInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiCompInfo()\n");
		return NULL;
	}
	initMultiCompInfo(pstInfo);

	return ((SIxMultiCompInfo_t *)pstInfo);
}

static void freeMultiCompInfo(SIxMultiCompInfo_t *pstInfo)
{
	if (NULL == pstInfo)		return;

	silib_desc_free(pstInfo->pszTextDes);
	silib_desc_free(pstInfo);
}

static void setMultiCompInfo(SIxMultiCompInfo_t *pstInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszTextDes)
{
    if(pszIso639LangCode != NULL)
	    HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstInfo->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	if(pszTextDes != NULL)
	    HxSTD_MemCopy(pstInfo->pszTextDes, pszTextDes, silib_desc_util_dvbStrlen(pszTextDes));
	pstInfo->pszTextDes[silib_desc_util_dvbStrlen(pszTextDes)] = '\0';
}

static void initMultiCompDes(SIxMultiCompDes_t *pstDes)
{
	pstDes->ucCompTag		= 0;
	pstDes->pstInfo 	= NULL;
}

static SIxMultiCompDes_t *allocMultiCompDes(void)
{
	return (SIxMultiCompDes_t *)silib_desc_alloc(sizeof(SIxMultiCompDes_t));
}

static void freeMultiCompDes(SIxMultiCompDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMultiCompInfo_t, freeMultiCompInfo);
	silib_desc_free(pstDes);
}

static void setMultiCompDes(SIxMultiCompDes_t *pstDes, HUINT8 ucCompTag)
{
	pstDes->ucCompTag = ucCompTag;
}

static SIxError_e parseMultiCompDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxMultiCompDes_t *pstDes)
{
	SIxMultiCompInfo_t	*pstPrevInfo = NULL, *pstNewInfo = NULL;
	HUINT8			ucCompTag;
	HUINT8			szIso639LangCode[4], *pszText = NULL;
	HUINT16			usDesLen, usTextLen;
	HUINT32			nTempTextLen;
	HUINT32			nAssignedTextLen;

	HxLOG_Info("parseMultiCompDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	/** < component tag (1byte) */
	ucCompTag = *p++;
	usDesLen--;
	HxLOG_Print("\t\t(component tag = 0x%x)\n", ucCompTag);

	/** < Descriptor 데이타 구성. */
	setMultiCompDes(pstDes, ucCompTag);

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		silib_desc_util_makeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < text length */
		usTextLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(text length = %d)\n", usTextLen);

		/** < text */
		nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);
		nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, nAssignedTextLen, (const HUINT8 *)p, &pszText);
		p += usTextLen;
		usDesLen -= usTextLen;
		HxLOG_Print("\t\t\t(text = %s)\n", pszText);

		/** < Descriptor 데이타 구성. */
		GET_PREV_INFO(pstPrevInfo, SIxMultiCompInfo_t, pstDes);
		pstNewInfo = allocMultiCompInfo();
		if (pstNewInfo == NULL)
			goto err_exit;
		pstNewInfo->pszTextDes = silib_desc_util_allocStr(nTempTextLen+1);
		if (pstNewInfo->pszTextDes == NULL)
			goto err_exit;

		setMultiCompInfo(pstNewInfo, szIso639LangCode, pszText);
		silib_desc_free(pszText);
		LINK_INFO_NODE(pstDes->pstInfo, pstNewInfo, NULL, pstPrevInfo);
	}

	HxLOG_Info(" parseMultiCompDes() Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxMultiCompInfo_t, freeMultiCompInfo);
		pstDes->pstInfo = NULL;
	}

	silib_desc_free(pszText);
	silib_desc_freeInfo_Dvb(eSIxDESCTAG_MULTILINGUAL_COMPONENT, 0, (HUINT8 *)pstNewInfo);
	return eSIxERR_NOT_ENOUGH_MEMORY;
}

static SIxError_e copyMultiCompDes(SIxMultiCompDes_t *pstDest, const SIxMultiCompDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxMultiCompDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxMultiCompInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxMultiCompInfo_t, pstDest);
				pstNewInfo = allocMultiCompInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				if (pstSourceInfo->pszTextDes)
				{
					HUINT32	ulSize = silib_desc_util_dvbStrlen(pstSourceInfo->pszTextDes);
					pstNewInfo->pszTextDes = silib_desc_util_allocStr(ulSize);
					if (NULL == pstNewInfo->pszTextDes)
					{
						silib_desc_freeInfo_Dvb(eSIxDESCTAG_MULTILINGUAL_COMPONENT, 0, (HUINT8 *)pstNewInfo);
						goto err_exit;
					}
				}

				setMultiCompInfo(pstNewInfo, pstSourceInfo->szIso639LangCode, pstSourceInfo->pszTextDes);
				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxMultiCompInfo_t, freeMultiCompInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_PRIVATE_DATA_SPECIFIER (TAG = 0x5F)
 ************************************************************************/
static void initPrivateDataSpecDes(SIxPrivateDataSpecDes_t *pstDes)
{
	pstDes->ulPrivateDataSpec = 0;
}

static SIxPrivateDataSpecDes_t *allocPrivateDataSpecDes(void)
{
	return (SIxPrivateDataSpecDes_t *)silib_desc_alloc(sizeof(SIxPrivateDataSpecDes_t));
}

static void freePrivateDataSpecDes(SIxPrivateDataSpecDes_t *pPrivateDataSpecDes)
{
	silib_desc_free(pPrivateDataSpecDes);
}

static void setPrivateDataSpecDes(SIxPrivateDataSpecDes_t *pPrivateDataSpecDes
	, HUINT32 ulPrivateDataSpec)
{
	pPrivateDataSpecDes->ulPrivateDataSpec = ulPrivateDataSpec;
}

static SIxError_e parsePrivateDataSpecDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxPrivateDataSpecDes_t *pstDes)
{
	HUINT16					usDesLen;
	HUINT32					ulPrivateDataSpec;

	HxLOG_Info("parsePrivateDataSpecDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown parsePrivateDataSpecDes Format!\n");
		return eSIxERR_INVALID;
	}

	ulPrivateDataSpec = HxMACRO_Get32Bit(p);
	/** <p += 4; */
	HxLOG_Print("\t\t(private data specifier = 0x%x)\n", ulPrivateDataSpec);

	/** < Descriptor 데이타 구성. */
	setPrivateDataSpecDes (pstDes, ulPrivateDataSpec);

	HxLOG_Info("parsePrivateDataSpecDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyPrivateDataSpecDes(SIxPrivateDataSpecDes_t *pstDest, const SIxPrivateDataSpecDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxPrivateDataSpecDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_SERVICE_MOVE (TAG = 0x60)
 ************************************************************************/
static void initSvcMoveDes(SIxSvcMoveDes_t *pstDes)
{
	pstDes->usNewOrgNetId	= 0;
	pstDes->usNewTsId		= 0;
	pstDes->usNewServId 	= 0;
}

static SIxSvcMoveDes_t *allocSvcMoveDes(void)
{
	return (SIxSvcMoveDes_t *)silib_desc_alloc(sizeof(SIxSvcMoveDes_t));
}

static void freeSvcMoveDes(SIxSvcMoveDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static void setSvcMoveDes(SIxSvcMoveDes_t *pstDes, HUINT16 usNewOrgNetId, HUINT16 usNewTsId, HUINT16 usNewSvcId)
{
	pstDes->usNewOrgNetId	= usNewOrgNetId;
	pstDes->usNewTsId 		= usNewTsId;
	pstDes->usNewServId 	= usNewSvcId;
}

static SIxError_e parseSvcMoveDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSvcMoveDes_t *pstDes)
{
	HUINT16			usDesLen;
	HUINT16			usNewOrgNetId, usNewTsId, usNewSvcId;

	HxLOG_Info("parseSvcMoveDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 6)
	{
		HxLOG_Print("[WARN] Unknown parseSvcMoveDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < new original network id (2byte) */
	usNewOrgNetId = HxMACRO_Get16Bit(p);
	p += 2;
	HxLOG_Print("\t\t(new original network id = 0x%x)\n", usNewOrgNetId);

	/** < new transport stream id (2byte) */
	usNewTsId = HxMACRO_Get16Bit(p);
	p += 2;
	HxLOG_Print("\t\t(new transport stream id = 0x%x)\n", usNewTsId);

	/** < new service id (2byte) */
	usNewSvcId = HxMACRO_Get16Bit(p);
	HxLOG_Print("\t\t(new service id = 0x%x)\n", usNewSvcId);

	/** < Descriptor 데이타 구성. */
	setSvcMoveDes(pstDes, usNewOrgNetId, usNewTsId, usNewSvcId);

	HxLOG_Info(" parseSvcMoveDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copySvcMoveDes(SIxSvcMoveDes_t *pstDest, const SIxSvcMoveDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcMoveDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_SHORT_SMOOTHING_BUFFER (TAG = 0x61)
 ************************************************************************/
static void initShortSmoothBufDes(SIxShortSmoothBufDes_t *pstDes)
{
	pstDes->ucSbSize 			= 0;
	pstDes->ucSbLeakRate		= 0;
	pstDes->pstPrivateDataInfo	= NULL;
}

static SIxShortSmoothBufDes_t *allocShortSmoothBufDes(void)
{
	return (SIxShortSmoothBufDes_t *)silib_desc_alloc(sizeof(SIxShortSmoothBufDes_t));
}

static void freeShortSmoothBufDes(SIxShortSmoothBufDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_util_freePrivateDataInfo(pstDes->pstPrivateDataInfo);
	silib_desc_free(pstDes);
}

static void setShortSmoothBufDes(SIxShortSmoothBufDes_t *pstDes
	, HUINT8 ucSbSize, HUINT8 ucSbLeakRate, SIxPrivateDataInfo_t	*pstPrivateDataInfo)
{
	pstDes->ucSbSize 			= ucSbSize;
	pstDes->ucSbLeakRate		= ucSbLeakRate;
	pstDes->pstPrivateDataInfo	= pstPrivateDataInfo;
}

static SIxError_e parseShortSmoothBufDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxShortSmoothBufDes_t *pstDes)
{
	SIxPrivateDataInfo_t		*pstNewInfo = NULL;
	HUINT8					ucSbSize, ucSbLeakRate;
	HUINT8					ucTemp;
	HUINT16					usDesLen;

	HxLOG_Info("parseShortSmoothBufDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseShortSmoothBufDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < sb size & sb leak rate (1byte) */
	ucTemp = *p++;
	usDesLen--;
	ucSbSize = (HUINT8)((ucTemp & 0xc0) >> 6);
	ucSbLeakRate = ucTemp & 0x07;
	HxLOG_Print("\t\t(sb size = 0x%x)\n", ucSbSize);
	HxLOG_Print("\t\t(sb leak rate = 0x%x)\n", ucSbLeakRate);

	/** < Private Data */
	if (usDesLen > 0)
	{
		SIxError_e	eErr;

		pstNewInfo = silib_desc_util_allocPrivateDataInfo((HUINT8)usDesLen);
		if (pstNewInfo == NULL)
			goto err_exit;

		eErr = silib_desc_util_setPrivateDataInfo(pstNewInfo, p, (HUINT8)usDesLen);
		if (eSIxERR_OK != eErr)
		{
			silib_desc_util_freePrivateDataInfo(pstNewInfo);
			goto err_exit;
		}
	}

	/** < Descriptor 데이타 구성. */
	setShortSmoothBufDes (pstDes, ucSbSize, ucSbLeakRate, pstNewInfo);

	HxLOG_Info("parseShortSmoothBufDes() Leaved!");
	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		silib_desc_util_freePrivateDataInfo(pstDes->pstPrivateDataInfo);
		pstDes->pstPrivateDataInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyShortSmoothBufDes(SIxShortSmoothBufDes_t *pstDest, const SIxShortSmoothBufDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxShortSmoothBufDes_t));
		pstDest->pstPrivateDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPrivateDataInfo);
		if (NULL == pstDest->pstPrivateDataInfo)
			goto err_exit;
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest->pstPrivateDataInfo)
	{
		silib_desc_util_freePrivateDataInfo(pstDest->pstPrivateDataInfo);
		pstDest->pstPrivateDataInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_FREQUENCY_LIST (TAG = 0x62)
 ************************************************************************/
static void initFrequencyListDes(SIxFrequencyListDes_t *pstDes)
{
	pstDes->ucCodingType = 0;
	pstDes->ucNumFreq = 0;
	pstDes->aulCentreFreq = NULL;
}

static SIxFrequencyListDes_t *allocFrequencyListDes(void)
{
	return (SIxFrequencyListDes_t *) silib_desc_alloc(sizeof(SIxFrequencyListDes_t));
}

static void freeFrequencyListDes(SIxFrequencyListDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	silib_desc_free(pstDes->aulCentreFreq);
	silib_desc_free(pstDes);
}

static SIxError_e parseFrequencyListDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxFrequencyListDes_t *pstDes)
{
	HUINT8				ucFreqNum;
	HUINT16				usDesLen;
	HUINT32				i;

	HxLOG_Info("parseFrequencyListDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 5)	/** < coding_type(1) + centre_frequency(4) */
	{
		HxLOG_Print("[WARN] Unknown parseFrequencyListDes Format!\n");
		return eSIxERR_INVALID;
	}

	pstDes->ucCodingType = (*p & 0x03);

	p ++; usDesLen --;

	ucFreqNum = usDesLen / 4;

	if (ucFreqNum == 0)	/** < if (usDesLen < 5)로 걸리지는 않음. */
		return eSIxERR_OK;

	pstDes->ucNumFreq = ucFreqNum;
	pstDes->aulCentreFreq = (HUINT32 *)silib_desc_alloc(ucFreqNum * 4);
	if (pstDes->aulCentreFreq == NULL)
	{
		HxLOG_Print("[ERR] parseFrequencyListDes Allocation Error!\n");
		return eSIxERR_NOT_ENOUGH_MEMORY;
	}

	for (i = 0; i < ucFreqNum; i++)
	{
		pstDes->aulCentreFreq[i] = HxMACRO_Get32Bit(p);
		HxLOG_Print("\t\t\t(centre frequency of FLD[%d] = %d)\n", i, pstDes->aulCentreFreq[i]);

		p += 4;
	}

	HxLOG_Info("parseFrequncyListDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyFrequencyListDes(SIxFrequencyListDes_t *pstDest, const SIxFrequencyListDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxFrequencyListDes_t));
		pstDest->ucNumFreq = 0;
		pstDest->aulCentreFreq = NULL;
		eErr = eSIxERR_OK;
		if (0 < pstSource->ucNumFreq && NULL != pstSource->aulCentreFreq)
		{
			eErr = eSIxERR_FAIL;
			pstDest->aulCentreFreq = silib_desc_alloc(sizeof(HUINT32)*pstSource->ucNumFreq);
			if (pstSource->aulCentreFreq)
			{
				HxSTD_MemCopy(pstDest->aulCentreFreq, pstSource->aulCentreFreq, sizeof(HUINT32)*pstSource->ucNumFreq);
				pstDest->ucNumFreq = pstSource->ucNumFreq;
				eErr = eSIxERR_OK;
			}
		}
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM (TAG = 0x63)
 ************************************************************************/
static void initPartialTsDes(SIxPartialTsDes_t *pstDes)
{
	pstDes->ulPeakRate = 0;
	pstDes->ulMinimumOverallSmoothingRate = 0;
	pstDes->ulMaximumOverallSmoothingBuffer = 0;
}

static SIxPartialTsDes_t *allocPartialTsDes(void)
{
	return (SIxPartialTsDes_t *)silib_desc_alloc(sizeof(SIxPartialTsDes_t));
}

static void freePartialTsDes(SIxPartialTsDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parsePartialTsDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxPartialTsDes_t *pstDes)
{
	HUINT8			 ucDesLen;

	HxLOG_Info("parsePartialTsDes() Entered!");

	ucDesLen = p[1];

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", p[0]);

	/** < descriptor length (1byte) */
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	pstDes->ulPeakRate = ((p[2] & 0x3F) <<16) | ((p[3] & 0xFF) << 8) | (p[4] & 0xFF);
	pstDes->ulMinimumOverallSmoothingRate = ((p[5] & 0x3F) <<16) | ((p[6] & 0xFF) << 8) | (p[7] & 0xFF);
	pstDes->ulMaximumOverallSmoothingBuffer = ((p[8] & 0x3F) << 8) | (p[9] & 0xFF);

	HxLOG_Info("parsePartialTsDes() Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyPartialTsDes(SIxPartialTsDes_t *pstDest, const SIxPartialTsDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxPartialTsDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_DATA_BROADCAST (TAG = 0x64)
 ************************************************************************/
static void initDataBroadcastInfo(SIxDataCarouselInfo_t *pstInfo)
{
	pstInfo->ucCarouselId = 0;
	pstInfo->ulTransactionId = 0;
	pstInfo->ulTimeOutValueDSI = 0;
	pstInfo->ulTimeOutValueDII = 0;
	pstInfo->ulLeakRate = 0;
}

static SIxDataCarouselInfo_t *allocDataBroadcastInfo(void)
{
	SIxDataCarouselInfo_t	*pstInfo;

	pstInfo = (SIxDataCarouselInfo_t *)silib_desc_alloc(sizeof(SIxDataCarouselInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocDataBroadcastInfo()\n");
		return NULL;
	}
	initDataBroadcastInfo(pstInfo);

	return ((SIxDataCarouselInfo_t *)pstInfo);
}

static void setDataBroadcastInfo(SIxDataCarouselInfo_t *pstInfo
	, HUINT8 ucCarouselId, HUINT32 ulTransactionId, HUINT32 ulTimeOutValueDSI
	, HUINT32 ulTimeOutValueDII, HUINT32 ulLeakRate)
{
	pstInfo->ucCarouselId = ucCarouselId;
	pstInfo->ulTransactionId = ulTransactionId;
	pstInfo->ulTimeOutValueDSI = ulTimeOutValueDSI;
	pstInfo->ulTimeOutValueDII = ulTimeOutValueDII;
	pstInfo->ulLeakRate = ulLeakRate;
}

static void freeDataBroadcastInfo(SIxDataCarouselInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void initObjectCarouselInfo(SIxObjectCarouselInfo_t *pstInfo)
{
	pstInfo->ucCarouselId = 0;
	pstInfo->ulTransactionId = 0;
	pstInfo->ulTimeOutValueDSI = 0;
	pstInfo->ulTimeOutValueDII = 0;
	pstInfo->ulLeakRate = 0;
	HxSTD_MemSet (pstInfo->szIso639LangCode, '\0', 4);
	pstInfo->pszObjName = NULL;
}

static SIxObjectCarouselInfo_t *allocObjectCarouselInfo(void)
{
	SIxObjectCarouselInfo_t	*pstInfo;

	pstInfo = (SIxObjectCarouselInfo_t *)silib_desc_alloc(sizeof(SIxObjectCarouselInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] allocObjectCarouselInfo()\n");
		return NULL;
	}
	initObjectCarouselInfo(pstInfo);

	return ((SIxObjectCarouselInfo_t *)pstInfo);
}

static void freeObjectCarouselInfo(SIxObjectCarouselInfo_t *pstInfo)
{
	if (NULL == pstInfo)	return;

	silib_desc_free(pstInfo->pszObjName);
	silib_desc_free(pstInfo);
}

static void setObjectCarouselInfo(SIxObjectCarouselInfo_t *pstInfo
	, HUINT8 ucCarouselId, HUINT32 ulTransactionId, HUINT32 ulTimeOutValueDSI
	, HUINT32 ulTimeOutValueDII, HUINT32 ulLeakRate
	, HUINT8 *pszIso639LangCode, HUINT8 *pszObjName)
{
	pstInfo->ucCarouselId = ucCarouselId;
	pstInfo->ulTransactionId = ulTransactionId;
	pstInfo->ulTimeOutValueDSI = ulTimeOutValueDSI;
	pstInfo->ulTimeOutValueDII = ulTimeOutValueDII;
	pstInfo->ulLeakRate = ulLeakRate;
	if(pszIso639LangCode != NULL)
	    HxSTD_MemCopy(pstInfo->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	if(pszObjName != NULL)
    	HxSTD_MemCopy(pstInfo->pszObjName, pszObjName, silib_desc_util_dvbStrlen(pszObjName));
}

static void initDataBroadcastDes(SIxDataBroadcastDes_t *pstDes)
{
	pstDes->usDataBroadcastId	 = 0;
	pstDes->ucCompTag			 = 0;
	pstDes->pucSelector			 = NULL;
	HxSTD_MemSet(pstDes->szIso639LangCode, '\0', 4);
	pstDes->pszText				 = NULL;
}

static SIxDataBroadcastDes_t *allocDataBroadcastDes(void)
{
	return (SIxDataBroadcastDes_t *)silib_desc_alloc(sizeof(SIxDataBroadcastDes_t));
}

static void freeDataBroadcastDes(SIxDataBroadcastDes_t *pstDes)
{
	if (NULL == pstDes)	return;

	if (pstDes->usDataBroadcastId == 0x0006)
	{
		freeDataBroadcastInfo ((SIxDataCarouselInfo_t *)pstDes->pucSelector);
	}
	else if (pstDes->usDataBroadcastId == 0x0007)
	{
		freeObjectCarouselInfo ((SIxObjectCarouselInfo_t *)pstDes->pucSelector);
	}

	silib_desc_free(pstDes->pszText);
	silib_desc_free(pstDes);
}

static void setDataBroadcastDes(SIxDataBroadcastDes_t *pstDes
	, HUINT16 usDataBroadcastId, HUINT8 ucCompTag, HUINT8 *pucSelector, HUINT8 *pszIso639LangCode, HUINT8 *pszText)
{
	pstDes->usDataBroadcastId = usDataBroadcastId;
	pstDes->ucCompTag = ucCompTag;
	pstDes->pucSelector = pucSelector;
	HxSTD_MemCopy(pstDes->szIso639LangCode, pszIso639LangCode, silib_desc_util_dvbStrlen(pszIso639LangCode));
	pstDes->szIso639LangCode[silib_desc_util_dvbStrlen(pszIso639LangCode)] = '\0';
	pstDes->pszText = pszText;
}

static SIxError_e parseDataBroadcastDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxDataBroadcastDes_t *pstDes)
{
	HUINT8					*aucInfo = NULL;
	HUINT16					usDataBroadcastId;
	HUINT16					usDesLen, usSelLen, usTextLen;
	HUINT8					ucCompTag;
	HUINT8					szIso639LangCode[4];
	HUINT8					*pszText = NULL;
	HUINT8					*pszObjName = NULL;
	HUINT32					nTempTextLen;
	HUINT32					nAssignedTextLen;
	HUINT8					ucCarouselId;
	HUINT32   				ulTransactionId;
	HUINT32					ulTimeOutValueDSI;
	HUINT32					ulTimeOutValueDII;
	HUINT32					ulLeakRate;

	HxLOG_Info("parseDataBroadcastDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseDataBroadcastDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < data broadcast id */
	usDataBroadcastId = HxMACRO_Get16Bit(p);
	p += 2;
	usDesLen -= 2;

	/** < component tag */
	ucCompTag = *p++;
	usDesLen--;

	/** < selector length */
	usSelLen = *p++;
	usDesLen--;

	/** < selector bytes */
	if( usSelLen > 0)
	{
		switch (usDataBroadcastId)
		{
			case 0x0006:	/** <DVB data carousel */
				aucInfo = (HUINT8 *)allocDataBroadcastInfo();
				if (aucInfo == NULL)
				{
					HxLOG_Print("[WARN] parseDataBroadcastDes Alloc error\n");
					HxLOG_Info(" parseDataBroadcastDes () Leaved!");
					goto err_exit;
				}
				ucCarouselId = (HUINT8)((*p) >> 6);
				p++;
				ulTransactionId = HxMACRO_Get32Bit(p);
				p += 4;
				ulTimeOutValueDSI = HxMACRO_Get32Bit(p);
				p += 4;
				ulTimeOutValueDII = HxMACRO_Get32Bit(p);
				p += 4;
				ulLeakRate = (HUINT32)((HxMACRO_Get32Bit(p) >> 8) & 0x003fffff);
				p += 3;
			 	setDataBroadcastInfo((SIxDataCarouselInfo_t *)aucInfo, ucCarouselId
			 		, ulTransactionId, ulTimeOutValueDSI, ulTimeOutValueDII
			 		, ulLeakRate);
				usDesLen -= 16;
				break;
			case 0x0007:	/** <DVB object carousel */
				aucInfo = (HUINT8 *)allocObjectCarouselInfo();
				if (aucInfo == NULL)
				{
					HxLOG_Print("[WARN] parseDataBroadcastDes Alloc error\n");
					HxLOG_Info(" parseDataBroadcastDes () Leaved!");
					goto err_exit;
				}
				ucCarouselId = (HUINT8)((*p) >> 6);
				p++;
				ulTransactionId = HxMACRO_Get32Bit(p);
				p += 4;
				ulTimeOutValueDSI = HxMACRO_Get32Bit(p);
				p += 4;
				ulTimeOutValueDII = HxMACRO_Get32Bit(p);
				p += 4;
				ulLeakRate = (HUINT32)((HxMACRO_Get32Bit(p) >> 8) & 0x003fffff);
				p += 3;
				usDesLen -= 16;

				HxSTD_MemCopy(szIso639LangCode, p, 3);
				szIso639LangCode[3] = '\0';
				silib_desc_util_makeCorrct639Code(szIso639LangCode);
				p += 3;
				usDesLen -= 3;

				/** < Objec Name length */
				usTextLen = *p++;
				usDesLen--;

				/** < Max allocation limit & allocation */
				nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);

				/** < Object Name */
				nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, nAssignedTextLen, (const HUINT8 *)p, &pszObjName);
				p += usTextLen;
				usDesLen -= usTextLen;
				HxLOG_Print("\t\t(Object Name = %s)\n", pszObjName);

			 	setObjectCarouselInfo((SIxObjectCarouselInfo_t *)aucInfo, ucCarouselId
			 		, ulTransactionId, ulTimeOutValueDSI, ulTimeOutValueDII
			 		, ulLeakRate, szIso639LangCode, pszObjName);

				silib_desc_free(pszObjName);

			 	break;
			default:
				HxLOG_Print("\t\tInvalid DataBroadcastId : %X\n", usDataBroadcastId);
				p += usSelLen;
				usDesLen -= usSelLen;
				break;
		}
	}
	/** < ISO 639 language code (3byte) */
	HxSTD_MemCopy(szIso639LangCode, p, 3);
	szIso639LangCode[3] = '\0';
	silib_desc_util_makeCorrct639Code(szIso639LangCode);
	p += 3;
	usDesLen -= 3;

	/** < text length */
	usTextLen = *p++;
	usDesLen--;

	/** < Max allocation limit & allocation */
	nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);

	/** <text */
	nTempTextLen = silib_desc_util_textToUTF8((const HCHAR *)szIso639LangCode, pstDes->stHead.stParam.eTextEnc, nAssignedTextLen, (const HUINT8 *)p, &pszText);
	if (NULL == pszText)
		goto err_exit;

	HxLOG_Print("\t\t(text = %s)\n", pszText);

 	setDataBroadcastDes(pstDes, usDataBroadcastId, ucCompTag
 		, (HUINT8 *)aucInfo, szIso639LangCode, pszText);

	HxLOG_Info("parseDataBroadcastDes() Leaved!");

	return eSIxERR_OK;

err_exit:
	if (aucInfo)
	{
		switch (usDataBroadcastId)
		{
		case 0x0006:	/** <DVB data carousel */
			freeDataBroadcastInfo((SIxDataCarouselInfo_t *)aucInfo);
			break;
		case 0x0007:	/** <DVB object carousel */
			freeObjectCarouselInfo((SIxObjectCarouselInfo_t *)aucInfo);
			break;
		default:
			break;
		}
	}

	silib_desc_free(pszText);
	return eSIxERR_FAIL;
}

static SIxError_e copyDataBroadcastDes(SIxDataBroadcastDes_t *pstDest, const SIxDataBroadcastDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDataBroadcastDes_t));
		pstDest->pucSelector = NULL;
		pstDest->pszText = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pucSelector)
		{
			HUINT8	*pucInfo;

			eErr = eSIxERR_FAIL;
			switch (pstSource->usDataBroadcastId)
			{
			case 0x0006:	/** <DVB data carousel */
				{
					SIxDataCarouselInfo_t	*pstSourceInfo = (SIxDataCarouselInfo_t *)pstSource->pucSelector;

					eErr = eSIxERR_FAIL;
					pucInfo = (HUINT8 *)silib_desc_allocInfo_Dvb(eSIxDESCTAG_DATA_BROADCAST, pstSource->usDataBroadcastId);
					if (pucInfo == NULL)
						goto err_exit;

					setDataBroadcastInfo((SIxDataCarouselInfo_t *)pucInfo, pstSourceInfo->ucCarouselId
						, pstSourceInfo->ulTransactionId, pstSourceInfo->ulTimeOutValueDSI, pstSourceInfo->ulTimeOutValueDII
						, pstSourceInfo->ulLeakRate);
					eErr = eSIxERR_OK;
				}
				break;
			case 0x0007:	/** <DVB object carousel */
				{
					SIxObjectCarouselInfo_t	*pstSourceInfo = (SIxObjectCarouselInfo_t *)pstSource->pucSelector;

					eErr = eSIxERR_FAIL;
					pucInfo = (HUINT8 *)silib_desc_allocInfo_Dvb(eSIxDESCTAG_DATA_BROADCAST, pstSource->usDataBroadcastId);
					if (pucInfo == NULL)
						goto err_exit;

				 	setObjectCarouselInfo((SIxObjectCarouselInfo_t *)pucInfo, pstSourceInfo->ucCarouselId
				 		, pstSourceInfo->ulTransactionId, pstSourceInfo->ulTimeOutValueDSI, pstSourceInfo->ulTimeOutValueDII
				 		, pstSourceInfo->ulLeakRate, pstSourceInfo->szIso639LangCode, pstSourceInfo->pszObjName);
					eErr = eSIxERR_OK;
				}
			default:
				break;
			}
		}

		if (pstSource->pszText)
		{
			HUINT32		ulSize = silib_desc_util_dvbStrlen(pstSource->pszText);

			eErr = eSIxERR_FAIL;
			if (0 < ulSize)
			{
				pstDest->pszText = silib_desc_util_allocStr(ulSize+1);
				if (pstDest->pszText)
				{
					HxSTD_MemCopy(pstDest->pszText, pstSource->pszText, sizeof(HUINT8)*ulSize);
					pstDest->pszText[ulSize] = '\0';
					eErr = eSIxERR_OK;
				}
			}
		}
	}
	return eErr;

err_exit:
	if (pstDest->pucSelector)
	{
		switch (pstDest->usDataBroadcastId)
		{
		case 0x0006:	/** <DVB data carousel */
			freeDataBroadcastInfo((SIxDataCarouselInfo_t *)pstDest->pucSelector);
			break;
		case 0x0007:	/** <DVB object carousel */
			freeObjectCarouselInfo((SIxObjectCarouselInfo_t *)pstDest->pucSelector);
			break;
		default:
			break;
		}
		pstDest->pucSelector = NULL;
	}

	silib_desc_free(pstDest->pszText);
	pstDest->pszText = NULL;
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_DATA_BROADCAST_ID (TAG = 0x66)
 ************************************************************************/
static void initDataBroadcastIdDes(SIxDataBroadcastIdDes_t *pstDes)
{
	pstDes->usDataBroadcastId = 0;
	pstDes->usSelectorLen = 0;
	pstDes->pucIdSelector = NULL;
}

static SIxDataBroadcastIdDes_t *allocDataBroadcastIdDes(void)
{
	return (SIxDataBroadcastIdDes_t *)silib_desc_alloc(sizeof(SIxDataBroadcastIdDes_t));
}

static void freeDataBroadcastIdDes(SIxDataBroadcastIdDes_t *pstDes)
{
	silib_desc_free(pstDes->pucIdSelector);
	silib_desc_free(pstDes);
}

static SIxError_e parseDataBroadcastIdDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxDataBroadcastIdDes_t *pstDes)
{
	HUINT8		*pucIdSelector = NULL;
	HUINT16		 usDesLen, usSelectorLen, usDataBroadcastId;

	HxLOG_Info("parseDataBroadcastDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseDataBroadcastIdDes Format!\n");
		return eSIxERR_INVALID;
	}

	/** < data broadcast id */
	usDataBroadcastId = HxMACRO_Get16Bit(p);
	p += 2;
	usDesLen -= 2;

	/** < selector length (usDesLen) */
	usSelectorLen = usDesLen;
	if (usSelectorLen > 0)
	{
		pucIdSelector = silib_desc_alloc(sizeof(HUINT8)*usSelectorLen);
		HxSTD_MemCopy (pucIdSelector, p, usSelectorLen);
	}

	/** < set descriptor */
	pstDes->usDataBroadcastId = usDataBroadcastId;
	pstDes->usSelectorLen = usSelectorLen;
	pstDes->pucIdSelector = pucIdSelector;

	return eSIxERR_OK;
}


static SIxError_e copyDataBroadcastIdDes(SIxDataBroadcastIdDes_t *pstDest, const SIxDataBroadcastIdDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDataBroadcastIdDes_t));
		pstDest->pucIdSelector = NULL;
		pstDest->usSelectorLen = 0;
		eErr = eSIxERR_OK;

		if (0 < pstSource->usSelectorLen && NULL != pstSource->pucIdSelector)
		{
			eErr = eSIxERR_FAIL;
			pstDest->pucIdSelector = silib_desc_alloc(sizeof(HUINT8)*pstSource->usSelectorLen);
			if (pstDest->pucIdSelector)
			{
				pstDest->usSelectorLen = pstSource->usSelectorLen;
				HxSTD_MemCopy(pstDest->pucIdSelector, pstSource->pucIdSelector, sizeof(HUINT8)*pstSource->usSelectorLen);
				eErr = eSIxERR_OK;
			}
		}
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_PDC (TAG = 0x69)
 ************************************************************************/
static void initPDCDes (SIxPDCDes_t *pstDes)
{
	pstDes->stHead.eTag		= 0;
	pstDes->stHead.eSpec	= eSIxSPEC_DVB;
	pstDes->ucMonth			= 0;
	pstDes->ucDay			= 0;
	pstDes->ucHour			= 0;
	pstDes->ucMinute		= 0;
}

static SIxPDCDes_t *allocPDCDes(void)
{
	return (SIxPDCDes_t *) silib_desc_alloc(sizeof(SIxPDCDes_t));
}

static void freePDCDes(SIxPDCDes_t *pPDCDes)
{
	silib_desc_free (pPDCDes);
}

static SIxError_e parsePDCDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxPDCDes_t *pstDes)
{
	HUINT16				usDesLen;
	HUINT32				ulPIL = 0;

	HxLOG_Info("parsePDCDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	ulPIL = HxMACRO_Get24Bit(p);
	p += 3;

	pstDes->ucDay	= (HUINT8)((ulPIL >> 15) & 0x001F);
	pstDes->ucMonth	= (HUINT8)((ulPIL >> 11) & 0x000F);
	pstDes->ucHour	= (HUINT8)((ulPIL >> 6) & 0x001F);
	pstDes->ucMinute	= (HUINT8)(ulPIL & 0x003F);

	HxLOG_Info(" parsePDCDes () Leaved!");

	return eSIxERR_OK;
}


static SIxError_e copyPDCDes(SIxPDCDes_t *pstDest, const SIxPDCDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxPDCDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_DOLBY_AC3 (TAG = 0x6A)
 ************************************************************************/
static void initDolbyAC3Des(SIxDolbyAC3Des_t *pstDes)
{
	pstDes->stHead.eTag			= 0;
	pstDes->stHead.eSpec		= eSIxSPEC_DVB;
	pstDes->bComponentTypeFlag	= 0;
	pstDes->bBsidFlag			= 0;
	pstDes->bMainIdFlag			= 0;
	pstDes->bAsvcFlag			= 0;
	pstDes->ucComponentType		= 0;
	pstDes->ucBsId				= 0;
	pstDes->ucMainId			= 0;
	pstDes->ucAsvc				= 0;
}

static SIxDolbyAC3Des_t *allocDolbyAC3Des(void)
{
	return (SIxDolbyAC3Des_t *)silib_desc_alloc(sizeof(SIxDolbyAC3Des_t));
}

static void freeDolbyAC3Des(SIxDolbyAC3Des_t *pstDes)
{
	silib_desc_free (pstDes);
}

static SIxError_e parseDolbyAC3Des(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxDolbyAC3Des_t *pstDes)
{
	HUINT8				ucTmpValue;
	HUINT16				usDesLen;

	HxLOG_Info("parseDolbyAC3Des () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	ucTmpValue = *p++; usDesLen--;
	pstDes->bComponentTypeFlag = (HBOOL)((ucTmpValue >> 7) & 1);
	pstDes->bBsidFlag = (HBOOL)((ucTmpValue >> 6) & 1);
	pstDes->bMainIdFlag = (HBOOL)((ucTmpValue >> 5) & 1);
	pstDes->bAsvcFlag = (HBOOL)((ucTmpValue >> 4) & 1);

	if (1 == pstDes->bComponentTypeFlag)
	{
		pstDes->ucComponentType = *p++; usDesLen--;
		HxLOG_Print("\t\t\t(component type = 0x%X)\n", pstDes->ucComponentType);
	}
	if (1 == pstDes->bBsidFlag)
	{
		pstDes->ucBsId = *p++; usDesLen--;
		HxLOG_Print("\t\t\t(bsid = 0x%X)\n", pstDes->ucBsId);
	}
	if (1 == pstDes->bMainIdFlag)
	{
		pstDes->ucMainId = *p++; usDesLen--;
		HxLOG_Print("\t\t\t(mainid = 0x%X)\n", pstDes->ucMainId);
	}
	if (1 == pstDes->bAsvcFlag)
	{
		pstDes->ucAsvc = *p++; usDesLen--;
		HxLOG_Print("\t\t\t(asvc = 0x%X)\n", pstDes->ucAsvc);
	}

	HxLOG_Info(" parseDolbyAC3Des () Leaved!");

	return eSIxERR_OK;

}

static SIxError_e copyDolbyAC3Des(SIxDolbyAC3Des_t *pstDest, const SIxDolbyAC3Des_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDolbyAC3Des_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_ANCILLARY_DATA (TAG = 0x6B)
 ************************************************************************/
//AncillaryData
#if 0
static void initAncillaryDataDes(SIxAncillaryDataDes_t *pstDes)
{
}

static SIxAncillaryDataDes_t *allocAncillaryDataDes(void)
{
	return (SIxAncillaryDataDes_t *)silib_desc_alloc(sizeof(SIxAncillaryDataDes_t));
}

static void freeAncillaryDataDes(SIxAncillaryDataDes_t *pstDes)
{
}

static SIxError_e parseAncillaryDataDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxAncillaryDataDes_t *pstDes)
{
}

static SIxError_e copyAncillaryDataDes(SIxAncillaryDataDes_t *pstDest, const SIxAncillaryDataDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAncillaryDataDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}
#endif

/************************************************************************
 * eSIxDESCTAG_CELL_LIST (TAG = 0x6C)
 ************************************************************************/
static void initSubCellListInfo(SIxSubCellListInfo_t *pstInfo)
{
	pstInfo->ucCellIdExtension = 0;
	pstInfo->usSubCellLatitude = 0;
	pstInfo->usSubCellLongitude = 0;
	pstInfo->usSubCellExtentOfLatitude = 0;
	pstInfo->usSubCellExtentOfLongitude = 0;
	pstInfo->pstNext = NULL;
}

static void initCellListInfo(SIxCellListInfo_t *pstInfo)
{
	pstInfo->usCellId = 0;
	pstInfo->usCellLatitude = 0;
	pstInfo->usCellLongitude = 0;
	pstInfo->usCellExtentOfLatitude = 0;
	pstInfo->usCellExtentOfLongitude = 0;
	pstInfo->pstInfo = NULL;
	pstInfo->pstNext = NULL;
}

static void initCellListDes(SIxCellListDes_t *pstDes)
{
	pstDes->pstInfo				= NULL;
}

SIxSubCellListInfo_t *allocSubCellListInfo(void)
{
	SIxSubCellListInfo_t	*pstInfo;

	pstInfo = (SIxSubCellListInfo_t *)silib_desc_alloc(sizeof(SIxSubCellListInfo_t));
	if (NULL == pstInfo)
	{
		HxLOG_Print("[ERROR] allocSubCellListInfo()\n");
		return NULL;
	}

	initSubCellListInfo(pstInfo);

	return pstInfo;
}

static SIxCellListInfo_t *allocCellListInfo(void)
{
	SIxCellListInfo_t	*pstInfo;

	pstInfo = (SIxCellListInfo_t *)silib_desc_alloc(sizeof(SIxCellListInfo_t));
	if (NULL == pstInfo)
	{
		HxLOG_Print("[ERROR] allocCellListInfo()\n");
		return NULL;
	}

	initCellListInfo (pstInfo);

	return pstInfo;
}

static SIxCellListDes_t *allocCellListDes(void)
{
	return (SIxCellListDes_t *)silib_desc_alloc(sizeof(SIxCellListDes_t));
}

static void freeCellList(SIxCellListInfo_t *pstInfo)
{
	FREE_INFO_LIST(pstInfo->pstInfo, SIxSubCellListInfo_t);
	silib_desc_free(pstInfo);
}

static void freeCellListInfo(SIxCellListInfo_t *pstInfo)
{
	FREE_INFO_LIST_EX(pstInfo, SIxCellListInfo_t, freeCellList);
}

static void freeCellListDes(SIxCellListDes_t *pstDes)
{
	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxCellListInfo_t, freeCellList);
	silib_desc_free(pstDes);
}

static SIxError_e parseCellListDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCellListDes_t *pstDes)
{
	SIxCellListInfo_t		*pstNewInfo = NULL, *pstPrevInfo = NULL;
	SIxSubCellListInfo_t		*pstNewSubInfo = NULL, *pstPrevSubInfo = NULL;
	HUINT8					ucSubCellLen;
	HUINT16					usDesLen;
	HUINT32					ulTmpValue;

	HxLOG_Info("parseCellListDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseCellListDes Format!\n");
		return eSIxERR_OK;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		if (usDesLen < 10)
		{
			p += usDesLen;
			break;
		}

		/** < alloc Info */
		pstNewInfo = allocCellListInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Print("[WARN] parseCellListDes Alloc error\n");
			HxLOG_Print(" parseCellListDes () Leaved!");
			goto err_exit;
		}

		pstNewInfo->usCellId					= HxMACRO_Get16Bit(p);
		p += 2; usDesLen -= 2;
		pstNewInfo->usCellLatitude			= HxMACRO_Get16Bit(p);
		p += 2; usDesLen -= 2;
		pstNewInfo->usCellLongitude			= HxMACRO_Get16Bit(p);
		p += 2; usDesLen -= 2;
		ulTmpValue = HxMACRO_Get32Bit(p);
		pstNewInfo->usCellExtentOfLatitude	= (HUINT16)((ulTmpValue >> 20) & (0x0fff));
		pstNewInfo->usCellExtentOfLongitude	= (HUINT16)((ulTmpValue >> 8) & (0x0fff));
		p += 4; usDesLen -= 4;
		pstNewInfo->pstNext = NULL;
		pstNewInfo->pstInfo = NULL;

		ucSubCellLen = (HUINT8)(ulTmpValue & 0xff);

		while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT && ucSubCellLen > 0)
		{
			if (ucSubCellLen < 8)
			{
				p += ucSubCellLen;
				break;
			}

			/** < alloc Info */
			pstNewSubInfo = (SIxSubCellListInfo_t *)allocSubCellListInfo();
			if (NULL == pstNewSubInfo)
			{
				freeCellListInfo(pstNewInfo);
				pstNewInfo = NULL;
				HxLOG_Print("[WARN] parseCellListDes Alloc error\n");
				HxLOG_Print(" parseCellListDes () Leaved!");
				goto err_exit;
			}

			pstNewSubInfo->ucCellIdExtension			= HxMACRO_Get8Bit(p);
			p += 1; usDesLen -= 1; ucSubCellLen -= 1;
			pstNewSubInfo->usSubCellLatitude			= HxMACRO_Get16Bit(p);
			p += 2; usDesLen -= 2; ucSubCellLen -= 2;
			pstNewSubInfo->usSubCellLongitude			= HxMACRO_Get16Bit(p);
			p += 2; usDesLen -= 2; ucSubCellLen -= 2;
			ulTmpValue = HxMACRO_Get24Bit(p);
			pstNewSubInfo->usSubCellExtentOfLatitude	= (HUINT16)((ulTmpValue >> 12) & (0x0fff));
			pstNewSubInfo->usSubCellExtentOfLatitude	= (HUINT16)((ulTmpValue) & (0x0fff));
			p += 3; usDesLen -= 3; ucSubCellLen -= 3;
			pstNewSubInfo->pstNext = NULL;

#if 0
			if (pstPrevSubInfo == NULL)
			{
				pstNewInfo->pstInfo = pstNewSubInfo;
				pstPrevSubInfo = pstNewSubInfo;
			}
			else
			{
				pstPrevSubInfo->pstNext = pstNewSubInfo;
				pstPrevSubInfo = pstPrevSubInfo->pstNext;
			}
#endif
			LINK_DES_NODE(pstNewInfo->pstInfo, pstPrevSubInfo, pstNewSubInfo);
		}

#if 0
		if (pstPrevInfo == NULL)
		{
			pstDes->pstInfo = pstNewInfo;
			pstPrevInfo = pstNewInfo;
		}
		else
		{
			pstPrevInfo->pstNext = pstNewInfo;
			pstPrevInfo = pstPrevInfo->pstNext;
		}
#endif
		LINK_DES_NODE(pstDes->pstInfo, pstPrevInfo, pstNewInfo);
	}

	HxLOG_Info(" parseCellListDes () Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxCellListInfo_t, freeCellList);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyCellListDes(SIxCellListDes_t *pstDest, const SIxCellListDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCellListDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxCellListInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxCellListInfo_t, pstDest);

				pstNewInfo = (SIxCellListInfo_t *)silib_desc_allocInfo_Dvb(eSIxDESCTAG_CELL_LIST, 0);
				if (NULL == pstNewInfo)
					goto err_exit;

				pstNewInfo->usCellId					= pstSourceInfo->usCellId;
				pstNewInfo->usCellLatitude			= pstSourceInfo->usCellLatitude;
				pstNewInfo->usCellLongitude			= pstSourceInfo->usCellLongitude;
				pstNewInfo->usCellExtentOfLatitude	= pstSourceInfo->usCellExtentOfLatitude;
				pstNewInfo->usCellExtentOfLongitude	= pstSourceInfo->usCellExtentOfLongitude;
				pstNewInfo->pstInfo = NULL;
				if (pstSourceInfo->pstInfo)
				{
					SIxSubCellListInfo_t		*pstNewSubInfo, *pstPrevSubInfo, *pstSourceSubInfo;

					pstSourceSubInfo = pstSourceInfo->pstInfo;
					while(pstSourceSubInfo)
					{
						GET_PREV_INFO(pstPrevSubInfo, SIxSubCellListInfo_t, pstNewInfo);

						pstNewSubInfo = (SIxSubCellListInfo_t *)allocSubCellListInfo();
						if (NULL == pstNewSubInfo)
							goto err_exit;

						pstNewSubInfo->ucCellIdExtension				= pstSourceSubInfo->ucCellIdExtension;
						pstNewSubInfo->usSubCellLatitude				= pstSourceSubInfo->usSubCellLatitude;
						pstNewSubInfo->usSubCellLongitude				= pstSourceSubInfo->usSubCellLongitude;
						pstNewSubInfo->usSubCellExtentOfLatitude		= pstSourceSubInfo->usSubCellExtentOfLatitude;
						pstNewSubInfo->usSubCellExtentOfLongitude		= pstSourceSubInfo->usSubCellExtentOfLongitude;

						LINK_INFO_NODE(pstNewInfo->pstInfo, pstNewSubInfo, NULL, pstPrevSubInfo);

						pstSourceSubInfo = pstSourceSubInfo->pstNext;
					}
				}

				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxCellListInfo_t, freeCellList);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;


}

/************************************************************************
 * eSIxDESCTAG_CELL_FREQ_LIST (TAG = 0x6D)
 ************************************************************************/
//CellFreqList
#if 0
static void initCellFreqListDes(SIxCellFreqListDes_t *pstDes)
{
}

static SIxCellFreqListDes_t *allocCellFreqListDes(HUINT32 ulSize)
{
	SIxCellFreqListDes_t	*pstDes;

	pstDes = (SIxCellFreqListDes_t *)silib_desc_alloc(ulSize);
	if (pstDes == NULL)
	{
		HxLOG_Print("[ERROR] %s()\n", __FUNCTION__);
		return NULL;
	}
	initCellFreqListDes(pstDes);

	return ((SIxCellFreqListDes_t *)pstDes);
}

static void freeCellFreqListDes(SIxCellFreqListDes_t *pstDes)
{
}

static SIxError_e parseCellFreqListDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxCellFreqListDes_t *pstDes)
{
}

static SIxError_e copyCellFreqListDes(SIxCellFreqListDes_t *pstDest, const SIxCellFreqListDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxCellFreqListDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}
#endif

/************************************************************************
 * eSIxDESCTAG_ANNOUNCEMENT_SUPPORT (TAG = 0x6E)
 ************************************************************************/
//AnnouncementSupport
#if 0
static void initAnnouncementSupportDes(SIxAnnouncementSupportDes_t *pstDes)
{
}

static SIxAnnouncementSupportDes_t *allocAnnouncementSupportDes(HUINT32 ulSize)
{
	SIxAnnouncementSupportDes_t	*pstDes;

	pstDes = (SIxAnnouncementSupportDes_t *)silib_desc_alloc(ulSize);
	if (pstDes == NULL)
	{
		HxLOG_Print("[ERROR] %s()\n", __FUNCTION__);
		return NULL;
	}
	initAnnouncementSupportDes(pstDes);

	return ((SIxAnnouncementSupportDes_t *)pstDes);
}

static void freeAnnouncementSupportDes(SIxAnnouncementSupportDes_t *pstDes)
{
}

static SIxError_e parseAnnouncementSupportDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxAnnouncementSupportDes_t *pstDes)
{
}

static SIxError_e copyAnnouncementSupportDes(SIxAnnouncementSupportDes_t *pstDest, const SIxAnnouncementSupportDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAnnouncementSupportDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}
#endif


/************************************************************************
 * eSIxDESCTAG_APPLICATION_SIGNALLING (TAG = 0x6F)
 ************************************************************************/
static void initAppSignallingInfo (SIxAppSignallingInfo_t *pstInfo)
{
	pstInfo->usAppType = 0;
	pstInfo->ucAitVersion = 0;
	pstInfo->pstNext = NULL;
}

static SIxAppSignallingInfo_t *allocAppSignallingInfo(void)
{
	SIxAppSignallingInfo_t	*pstInfo;

	pstInfo = (SIxAppSignallingInfo_t *)silib_desc_alloc(sizeof(SIxAppSignallingInfo_t));
	if (pstInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocAppSignallingInfo ()\n");
		return NULL;
	}

	initAppSignallingInfo (pstInfo);
	return ((SIxAppSignallingInfo_t *) pstInfo);
}

static void freeAppSignallingInfo (SIxAppSignallingInfo_t *pstInfo)
{
	silib_desc_free(pstInfo);
}

static void initAppSignallingDes (SIxAppSignallingDes_t *pstDes)
{
	pstDes->stHead.eTag 	= 0;
	pstDes->stHead.eSpec	= eSIxSPEC_DVB;
	pstDes->pstInfo			= NULL;
}

static SIxAppSignallingDes_t *allocAppSignallingDes(void)
{
	return (SIxAppSignallingDes_t *)silib_desc_alloc(sizeof(SIxAppSignallingDes_t));
}

static void freeAppSignallingDes (SIxAppSignallingDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST(pstDes->pstInfo, SIxAppSignallingInfo_t);
	silib_desc_free (pstDes);
}

static SIxError_e parseAppSignallingDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxAppSignallingDes_t *pstDes)
{
	SIxAppSignallingInfo_t	*pstNewInfo = NULL, *pstPrevInfo = NULL;
	HUINT16				usDesLen;

	HxLOG_Info("parseAppSignallingDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseAppSignallingDes Format!\n");
		/** < return eSIxERR_INVALID;
			  Application_signaling_descriptor에서 descriptor_length가
			  0일 수도 있다. 따라서 이 경우에 에러가 아니다.
			  JPROH in Italy. 2002/6/11 */
		return eSIxERR_OK;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < JPROH */
		if (usDesLen < 3)
		{
			p += usDesLen;
			break;
		}

		/** < alloc Info */
		pstNewInfo = allocAppSignallingInfo();
		if (pstNewInfo == NULL)
		{
			HxLOG_Print("[WARN] parseAppSignallingInfo Alloc error\n");
			HxLOG_Print(" parseAppSignallingDes () Leaved!");
			goto err_exit;
		}

		pstNewInfo->usAppType = HxMACRO_Get15Bit(p);	/** < application type */
		p += 2; usDesLen -= 2;
		pstNewInfo->ucAitVersion = (*p & 0x1f);	/** < AIT version number */
		p++; usDesLen --; /** < JPROH */

#if 0
		if (pstPrevInfo == NULL)
		{
			pstDes->pstInfo = pstNewInfo;
			pstPrevInfo = pstNewInfo;
		}
		else
		{
			pstPrevInfo->pstNext = pstNewInfo;
			pstPrevInfo = pstPrevInfo->pstNext;
		}
#endif
		LINK_DES_NODE(pstDes->pstInfo, pstPrevInfo, pstNewInfo);
	}

	HxLOG_Info(" parseAppSignallingDes () Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST(pstDes->pstInfo, SIxAppSignallingInfo_t);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyAppSignallingDes(SIxAppSignallingDes_t *pstDest, const SIxAppSignallingDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAppSignallingDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxAppSignallingInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxAppSignallingInfo_t, pstDest);

				pstNewInfo = allocAppSignallingInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				pstNewInfo->usAppType		= pstSourceInfo->usAppType;
				pstNewInfo->ucReserved		= pstSourceInfo->ucReserved;
				pstNewInfo->ucAitVersion	= pstSourceInfo->ucAitVersion;

				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}

	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST(pstDest->pstInfo, SIxAppSignallingInfo_t);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;

}

/************************************************************************
 * eSIxDESCTAG_ADAPTATION_FIELD_DATA (TAG = 0x70)
 ************************************************************************/
//AdaptationFieldDataDes
#if 0
static void initAdaptationFieldDataDes(SIxAdaptationFieldDataDes_t *pstDes)
{
}

static SIxAdaptationFieldDataDes_t *allocAdaptationFieldDataDes()
{
	return (SIxAdaptationFieldDataDes_t *)silib_desc_alloc(sizeof(SIxAdaptationFieldDataDes_t));
}

static void freeAdaptationFieldDataDes(SIxAdaptationFieldDataDes_t *pstDes)
{
}

static SIxError_e parseAdaptationFieldDataDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxAdaptationFieldDataDes_t *pstDes)
{
}

static SIxError_e copyAdaptationFieldDataDes(SIxAdaptationFieldDataDes_t *pstDest, const SIxAdaptationFieldDataDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAdaptationFieldDataDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}
#endif

/************************************************************************
 * eSIxDESCTAG_SVC_IDENTIFIER (TAG = 0x71)
 ************************************************************************/
//SvcIdentifier
#if 0
static void initSvcIdentifierDes(SIxSvcIdentifierDes_t *pstDes)
{
}

static SIxSvcIdentifierDes_t *allocSvcIdentifierDes(HUINT32 ulSize)
{
	SIxSvcIdentifierDes_t	*pstDes;

	pstDes = (SIxSvcIdentifierDes_t *)silib_desc_alloc(ulSize);
	if (pstDes == NULL)
	{
		HxLOG_Print("[ERROR] %s()\n", __FUNCTION__);
		return NULL;
	}
	initSvcIdentifierDes(pstDes);

	return ((SIxSvcIdentifierDes_t *)pstDes);
}

static void freeSvcIdentifierDes(SIxSvcIdentifierDes_t *pstDes)
{
}

static SIxError_e parseSvcIdentifierDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSvcIdentifierDes_t *pstDes)
{
}

static SIxError_e copySvcIdentifierDes(SIxSvcIdentifierDes_t *pstDest, const SIxSvcIdentifierDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcIdentifierDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}
#endif

/************************************************************************
 * eSIxDESCTAG_SVC_AVAILABILITY (TAG = 0x72)
 ************************************************************************/
//SvcAvailability
#if 0
static void initSvcAvailabilityDes(SIxSvcAvailabilityDes_t *pstDes)
{
}

static SIxSvcAvailabilityDes_t *allocSvcAvailabilityDes(HUINT32 ulSize)
{
	SIxSvcAvailabilityDes_t	*pstDes;

	pstDes = (SIxSvcAvailabilityDes_t *)silib_desc_alloc(ulSize);
	if (pstDes == NULL)
	{
		HxLOG_Print("[ERROR] %s()\n", __FUNCTION__);
		return NULL;
	}
	initSvcAvailabilityDes(pstDes);

	return ((SIxSvcAvailabilityDes_t *)pstDes);
}

static void freeSvcAvailabilityDes(SIxSvcAvailabilityDes_t *pstDes)
{
}

static SIxError_e parseSvcAvailabilityDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxSvcAvailabilityDes_t *pstDes)
{
}

static SIxError_e copySvcAvailabilityDes(SIxSvcAvailabilityDes_t *pstDest, const SIxSvcAvailabilityDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxSvcAvailabilityDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}
#endif

/************************************************************************
 * eSIxDESCTAG_DEFAULT_AUTORITY (TAG = 0x73)
 ************************************************************************/
static void initDefaultAuthorityDes(SIxDefaultAuthorityDes_t	*pstDes)
{
	pstDes->stHead.eTag 	= 0;
	pstDes->stHead.eSpec	= eSIxSPEC_DVB;
	pstDes->pszDefaultAuthority = NULL;
}

static SIxDefaultAuthorityDes_t *allocDefaultAuthorityDes(void)
{
	return (SIxDefaultAuthorityDes_t*)silib_desc_alloc(sizeof(SIxDefaultAuthorityDes_t));
}

static void freeDefaultAuthorityDes(SIxDefaultAuthorityDes_t *pstDes)
{
	silib_desc_free(pstDes->pszDefaultAuthority);
	silib_desc_free(pstDes);
}

static SIxError_e parseDefaultAuthorityDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxDefaultAuthorityDes_t *pstDes)
{
	HUINT16				usDesLen, uriLen;

	HxLOG_Info("parseDefaultAuthority () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	uriLen = silib_desc_util_uriStrlen(usDesLen, p);
	pstDes->pszDefaultAuthority = silib_desc_alloc(uriLen + 1);
	if (silib_desc_util_uriStrcpy(pstDes->pszDefaultAuthority, p, usDesLen) != uriLen)
		HxLOG_Print("\t\t\tSomething is wrong!!! - check function (URI)\n");
	pstDes->pszDefaultAuthority[uriLen]= '\0';
	HxLOG_Print("\t\t\t(default Authority = %s)\n", pstDes->pszDefaultAuthority);

	HxLOG_Info(" parseDefaultAuthority () Leaved!");

	return eSIxERR_OK;
}


static SIxError_e copyDefaultAuthorityDes(SIxDefaultAuthorityDes_t *pstDest, const SIxDefaultAuthorityDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDefaultAuthorityDes_t));
		pstDest->pszDefaultAuthority = NULL;

		if (pstSource->pszDefaultAuthority)
		{
			HUINT32 ulSize = silib_desc_util_dvbStrlen(pstSource->pszDefaultAuthority);

			pstDest->pszDefaultAuthority = silib_desc_util_allocStr(ulSize+1);
			if (NULL == pstDest->pszDefaultAuthority)
				goto err_exit;

			HxSTD_MemCopy(pstDest->pszDefaultAuthority, pstSource->pszDefaultAuthority, ulSize);
			pstDest->pszDefaultAuthority[ulSize] = '\0';
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_free(pstDest->pszDefaultAuthority);
		pstDest->pszDefaultAuthority = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_RELATED_CONTENT (TAG = 0x74)
 ************************************************************************/
static void initRelatedContentDes(SIxRelatedContentDes_t *pstDes)
{
	pstDes->stHead.eTag			= 0;
	pstDes->stHead.eSpec		= eSIxSPEC_DVB;
	pstDes->pstPrivateDataInfo	= NULL;
}

static SIxRelatedContentDes_t *allocRelatedContentDes(void)
{
	return (SIxRelatedContentDes_t*)silib_desc_alloc(sizeof(SIxRelatedContentDes_t));
}

static void freeRelatedContentDes(SIxRelatedContentDes_t *pstDes)
{
	silib_desc_free(pstDes->pstPrivateDataInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseRelatedContentDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxRelatedContentDes_t *pstDes)
{
	HUINT8					 ucDesLen;
	SIxPrivateDataInfo_t		*pstNewInfo = NULL;

	HxLOG_Info("parseRelatedContentDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	/** < Private Data */
	if (ucDesLen > 0)
	{
		SIxError_e	eErr;

		pstNewInfo = silib_desc_util_allocPrivateDataInfo(ucDesLen);
		if (pstNewInfo == NULL)
			goto err_exit;

		eErr = silib_desc_util_setPrivateDataInfo(pstNewInfo, p, ucDesLen);
		if (eSIxERR_OK != eErr)
		{
			silib_desc_util_freePrivateDataInfo(pstNewInfo);
			goto err_exit;
		}
	}

	pstDes->pstPrivateDataInfo = pstNewInfo;

	HxLOG_Info(" parseRelatedContentDes () Leaved!");
	return eSIxERR_OK;

err_exit:
	return eSIxERR_FAIL;
}


static SIxError_e copyRelatedContentDes(SIxRelatedContentDes_t *pstDest, const SIxRelatedContentDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxRelatedContentDes_t));
		pstDest->pstPrivateDataInfo = silib_desc_util_dumpPrivateDataInfo(pstSource->pstPrivateDataInfo);
		if (NULL == pstDest->pstPrivateDataInfo)
			goto err_exit;
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		silib_desc_util_freePrivateDataInfo(pstDest->pstPrivateDataInfo);
		pstDest->pstPrivateDataInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_CONTENT_IDENTIFIER (Tag = 0x76)
 ************************************************************************/
static void initCridInfo(SIxCridInfo_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxCridInfo_t));
}

static SIxCridInfo_t	*allocCridInfo(void)
{
	SIxCridInfo_t	*pstDes;

	pstDes = (SIxCridInfo_t *)silib_desc_alloc(sizeof(SIxCridInfo_t));
	if (pstDes == NULL)
	{
		HxLOG_Print("[ERROR] allocCridInfo ()\n");
		return NULL;
	}

	initCridInfo(pstDes);

	return ((SIxCridInfo_t *)pstDes);
}

static void freeCridInfo(SIxCridInfo_t *pstDes)
{
	silib_desc_free(pstDes->pszCridBytes);
	silib_desc_free(pstDes);
}

static void initContentIdentifierDes(SIxContentIdentifierDes_t *pstDes)
{
	pstDes->stHead.eTag = 0;
	pstDes->stHead.eSpec	= eSIxSPEC_DVB;
	pstDes->pstInfo = NULL;
}

static SIxContentIdentifierDes_t	*allocContentIdentifierDes(void)
{
	return (SIxContentIdentifierDes_t *)silib_desc_alloc(sizeof(SIxContentIdentifierDes_t));
}

static void freeContentIdentifierDes(SIxContentIdentifierDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxCridInfo_t, freeCridInfo);
	silib_desc_free(pstDes);
}

static SIxError_e parseContentIdentifierDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxContentIdentifierDes_t *pstDes)
{
	SIxCridInfo_t	*pstNewInfo = NULL, *pstPrevInfo = NULL;
	HUINT8		ucTmpByte;
	HUINT16		usDesLen, uriLen;

	HxLOG_Info("parseContentIdentifierDes () Entered!");
	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		pstNewInfo = allocCridInfo();
		if (pstNewInfo == NULL)
			goto err_exit;

		ucTmpByte = *p++;		usDesLen--;
		pstNewInfo->ucCridType = (ucTmpByte >> 2) & 0x3F;
		pstNewInfo->ucCridLocation = (ucTmpByte & 0x03);

		HxLOG_Print("\t\t\tCridType = 0x%x)\n", pstNewInfo->ucCridType);
		HxLOG_Print("\t\t\tCridLocation = 0x%x)\n", pstNewInfo->ucCridLocation);

		if (pstNewInfo->ucCridLocation == 0)
		{
			//	supported format. if you delete below conditional check line, you will encounter system hangup.
			//	some of streams are sent with abnormal TV anytime specification
			//	0x30 ~ 0x3f
			if ((pstNewInfo->ucCridType < 0x30) || (pstNewInfo->ucCridType > 0x3F))
			{
				//	not support crid type.
				HxLOG_Print("\t\t\tnot supported\n");
				freeCridInfo(pstNewInfo);
				break;
			}

			ucTmpByte = *p++;	usDesLen--;

			uriLen = silib_desc_util_uriStrlen(ucTmpByte, p);
			pstNewInfo->pszCridBytes = silib_desc_alloc(uriLen + 1);
			if (NULL == pstNewInfo->pszCridBytes)
			{
				freeCridInfo(pstNewInfo);
				pstNewInfo = NULL;
				goto err_exit;
			}
			if (silib_desc_util_uriStrcpy(pstNewInfo->pszCridBytes, p, ucTmpByte) != uriLen)
				HxLOG_Print("\t\t\tSomething is Wrong!! - check function uri\n");
			pstNewInfo->pszCridBytes[uriLen] = 0;
			p += ucTmpByte;		usDesLen -= ucTmpByte;
			HxLOG_Print("\t\t\tcridBytes = %s\n", pstNewInfo->pszCridBytes);
		}
		else if (pstNewInfo->ucCridLocation == 1)		//	CIT reference, not supported yet
		{
			pstNewInfo->usCridRef = HxMACRO_Get16Bit(p);
			p += 2;		usDesLen -= 2;
			HxLOG_Print("\t\t\tcridRef = 0x%x\n", pstNewInfo->usCridRef);
		}
		else
		{
			//	not support location.
			HxLOG_Print("\t\t\tnot supported\n");
			freeCridInfo(pstNewInfo);
			continue;
		}

#if 0
		if (pstPrevInfo == NULL)
		{
			pstDes->pstInfo = pstNewInfo;
			pstPrevInfo = pstNewInfo;
		}
		else
		{
			pstPrevInfo->pstNext = pstNewInfo;
			pstPrevInfo = pstPrevInfo->pstNext;
		}
#endif
		LINK_DES_NODE(pstDes->pstInfo, pstPrevInfo, pstNewInfo);
	}

	HxLOG_Info("parseContentIdentifierDes () Leaved!");

	return eSIxERR_OK;

err_exit:
//	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxCridInfo_t, freeCridInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}


static SIxError_e copyContentIdentifierDes(SIxContentIdentifierDes_t *pstDest, const SIxContentIdentifierDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxContentIdentifierDes_t));

		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		if (pstSource->pstInfo)
		{
			SIxCridInfo_t		*pstNewInfo, *pstPrevInfo = NULL, *pstSourceInfo;

			eErr = eSIxERR_FAIL;
			pstSourceInfo = pstSource->pstInfo;
			while (pstSourceInfo)
			{
				GET_PREV_INFO(pstPrevInfo, SIxCridInfo_t, pstDest);

				pstNewInfo = allocCridInfo();
				if (NULL == pstNewInfo)
					goto err_exit;

				pstNewInfo->ucCridType		= pstSourceInfo->ucCridType;
				pstNewInfo->ucCridLocation	= pstSourceInfo->ucCridLocation;
				if (pstSourceInfo->pszCridBytes)
				{
					HUINT32		ulSize = silib_desc_util_dvbStrlen(pstSourceInfo->pszCridBytes);

					if (0 < ulSize)
					{
						pstNewInfo->pszCridBytes = silib_desc_alloc(ulSize+1);
						if (NULL == pstNewInfo->pszCridBytes)
						{
							freeCridInfo(pstNewInfo);
							pstNewInfo = NULL;
							goto err_exit;
						}

						HxSTD_MemCopy(pstNewInfo->pszCridBytes, pstSourceInfo->pszCridBytes, sizeof(HUINT8)*ulSize);
						pstNewInfo->pszCridBytes[ulSize] = '\0';
					}
				}

				LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

				pstSourceInfo = pstSourceInfo->pstNext;
			}
			eErr = eSIxERR_OK;
		}
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxCridInfo_t, freeCridInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

/************************************************************************
 * eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM (Tag = 0x79)
 ************************************************************************/
static void initS2SatDeliSystemDes(SIxS2SatDeliSystemDes_t *pstDes)
{
	pstDes->ucBackCompatibilityIndicator	= 0;
	pstDes->ucInputStreamIdentifier		= 0;
	pstDes->ucMultipleInStreamFlag			= 0;
	pstDes->ucScrambleSeqSelect			= 0;
	pstDes->ulScramblingSeqIndex			= 0;
}

static SIxS2SatDeliSystemDes_t *allocS2SatDeliSystemDes(void)
{
	return (SIxS2SatDeliSystemDes_t *)silib_desc_alloc(sizeof(SIxS2SatDeliSystemDes_t));
}

static void freeS2SatDeliSystemDes(SIxS2SatDeliSystemDes_t *pstDes)
{
	silib_desc_free (pstDes);
}

static SIxError_e parseS2SatDeliSystemDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxS2SatDeliSystemDes_t *pstDes)
{
	HUINT16		usDesLen;
	HUINT8		ucTemp;

	HxLOG_Info("parseS2SatDeliSystemDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseS2SatDeliSystemDes Format!\n");
		return eSIxERR_INVALID;
	}

	ucTemp = *p++;

	/** < scrambling sequence selector */
	pstDes->ucScrambleSeqSelect = (HUINT8)((ucTemp & 0x80) >> 7);

	/** < multiple input stream flag */
	pstDes->ucMultipleInStreamFlag = (HUINT8)((ucTemp & 0x40) >> 6);

	/** < backwards ocmpatibility indicator */
	pstDes->ucBackCompatibilityIndicator = (HUINT8)((ucTemp & 0x20) >> 5);

	/** < reserved for use */
	pstDes->ucReserved1 = (HUINT8)(ucTemp & 0x1f);

	ucTemp = *p++;

	if( pstDes->ucScrambleSeqSelect == 1)
	{
		pstDes->ucReserved2 = (HUINT8)((ucTemp & 0xfb)>>2);

		/** < scrambling sequence index */
		pstDes->ulScramblingSeqIndex
			= (HUINT32)( ((ucTemp & 0x03 ) <<16) |(*p<<8) |*(p+1) );

//		*p++;
		p++;
	}
	if( pstDes->ucMultipleInStreamFlag == 1)
	{
		/** < input stream identifier */
		pstDes->ucInputStreamIdentifier = *p++;
	}

	HxLOG_Info(" parseS2SatDeliSystemDes () Leaved!");

	return eSIxERR_OK;
}


static SIxError_e copyS2SatDeliSystemDes(SIxS2SatDeliSystemDes_t *pstDest, const SIxS2SatDeliSystemDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxS2SatDeliSystemDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_ENHANCED_AC3 (Tag = 0x7A)
 ************************************************************************/
static void initEnhancedAC3Des(SIxEnhancedAC3Des_t *pstDes)
{
	pstDes->bComponentTypeFlag 	= FALSE;
	pstDes->bBsidFlag			= FALSE;
	pstDes->bMainIdFlag			= FALSE;
	pstDes->bAsvcFlag				= FALSE;
	pstDes->bMixinfoexists 		= FALSE;
	pstDes->bSubStream1Flag 	= FALSE;
	pstDes->bSubStream2Flag 	= FALSE;
	pstDes->bSubStream3Flag 	= FALSE;
	pstDes->ucComponentType		= 0;
	pstDes->ucBsId				= 0;
	pstDes->ucMainId			= 0;
	pstDes->ucAsvc				= 0;
	pstDes->ucSubStream1		= 0;
	pstDes->ucSubStream2		= 0;
	pstDes->ucSubStream3		= 0;

}

static SIxEnhancedAC3Des_t *allocEnhancedAC3Des(void)
{
	return (SIxEnhancedAC3Des_t *)silib_desc_alloc(sizeof(SIxEnhancedAC3Des_t));
}

static void freeEnhancedAC3Des(SIxEnhancedAC3Des_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseEnhancedAC3Des (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxEnhancedAC3Des_t *pstDes)
{
	HUINT16				usDesLen;
	HUINT8				ucTemp;

	HxLOG_Info("parseEnhancedAC3Des () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseEnhancedAC3Des Format!\n");
		return eSIxERR_INVALID;
	}

	ucTemp = *p++;
	pstDes->bComponentTypeFlag 	= (HBOOL)((ucTemp & 0x80)>>7);
	pstDes->bBsidFlag				= (HBOOL)((ucTemp & 0x40)>>6);
	pstDes->bMainIdFlag			= (HBOOL)((ucTemp & 0x20)>>5);
	pstDes->bAsvcFlag				= (HBOOL)((ucTemp & 0x10)>>4);
	pstDes->bMixinfoexists 		= (HBOOL)((ucTemp & 0x08)>>3);
	pstDes->bSubStream1Flag 		= (HBOOL)((ucTemp & 0x04)>>2);
	pstDes->bSubStream2Flag 		= (HBOOL)((ucTemp & 0x02)>>1);
	pstDes->bSubStream3Flag 		= (HBOOL)(ucTemp & 0x01);
	HxLOG_Print("\t\t(bComponentTypeFlag = %d)\n", pstDes->bComponentTypeFlag);
	HxLOG_Print("\t\t(bBsidFlag = %d)\n", pstDes->bBsidFlag);
	HxLOG_Print("\t\t(bMainIdFlag = %d)\n", pstDes->bMainIdFlag);
	HxLOG_Print("\t\t(bAsvcFlag = %d)\n", pstDes->bAsvcFlag);
	HxLOG_Print("\t\t(bMixinfoexists = %d)\n", pstDes->bMixinfoexists);
	HxLOG_Print("\t\t(bSubStreamFlag1 = %d)\n", pstDes->bSubStream1Flag);
	HxLOG_Print("\t\t(bSubStreamFlag2 = %d)\n", pstDes->bSubStream2Flag);
	HxLOG_Print("\t\t(bSubStreamFlag3 = %d)\n", pstDes->bSubStream3Flag);

	if( pstDes->bComponentTypeFlag == 1)
		pstDes->ucComponentType = *p++;
	if( pstDes->bBsidFlag == 1)
		pstDes->ucBsId = *p++;
	if( pstDes->bMainIdFlag == 1)
		pstDes->ucMainId = *p++;
	if( pstDes->bAsvcFlag == 1)
		pstDes->ucAsvc = *p++;
	if( pstDes->bSubStream1Flag == 1)
		pstDes->ucSubStream1 = *p++;
	if( pstDes->bSubStream2Flag == 1)
		pstDes->ucSubStream2 = *p++;
	if( pstDes->bSubStream3Flag == 1)
		pstDes->ucSubStream3 = *p++;

	HxLOG_Info(" parseEnhancedAC3Des () Leaved!");

	return eSIxERR_OK;

}

static SIxError_e copyEnhancedAC3Des(SIxEnhancedAC3Des_t *pstDest, const SIxEnhancedAC3Des_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxEnhancedAC3Des_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * eSIxDESCTAG_DTS (TAG = 0x7B)
 ************************************************************************/
//DTS
#if 0
static void initDTSDes(SIxDTSDes_t *pstDes)
{
}

static SIxDTSDes_t *allocDTSDes(void)
{
	return (SIxDTSDes_t *)silib_desc_alloc(sizeof(SIxDTSDes_t));
}

static void freeDTSDes(SIxDTSDes_t *pstDes)
{
}

static SIxError_e parseDTSDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxDTSDes_t *pstDes)
{
}

static SIxError_e copyDTSDes(SIxDTSDes_t *pstDest, const SIxDTSDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxDTSDes_t));
	}

	return eErr;

err_exit:
	return eSIxERR_FAIL;
}
#endif

/************************************************************************
 * eSIxDESCTAG_AAC (Tag = 0x7C)
 ************************************************************************/
static void initAACDes(SIxAACDes_t *pstDes)
{
	pstDes->ucProfileAndLevel	= 0;
	pstDes->bAACTypeFlag		= FALSE;
	pstDes->ucAACType			= 0;
}

static SIxAACDes_t *allocAACDes(void)
{
	return (SIxAACDes_t *)silib_desc_alloc(sizeof(SIxAACDes_t));
}

static void freeAACDes(SIxAACDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseAACDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxAACDes_t *pstDes)
{
	HUINT8				ucTemp;
	HUINT16				usDesLen;

	HxLOG_Info("parseAACDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 2)
	{
		HxLOG_Print("[WARN] Unknown parseAACDes Format!\n");
		return eSIxERR_INVALID;
	}

	ucTemp 						= *p++;
	pstDes->ucProfileAndLevel	= ucTemp;
	ucTemp 						= *p++;
	pstDes->bAACTypeFlag		= (HBOOL)((ucTemp & 0x80)>>7);

	HxLOG_Print("\t\t(ucProfileAndLevel = %d)\n", pstDes->ucProfileAndLevel);
	HxLOG_Print("\t\t(bAacTypeAvilable = %d)\n", pstDes->bAACTypeFlag);

	if (pstDes->bAACTypeFlag && usDesLen >= 3)
	{
		ucTemp 				= *p++;
		pstDes->ucAACType	= ucTemp;
		HxLOG_Print("\t\t(ucAacType = %d)\n", pstDes->ucAACType);
	}

	HxLOG_Info(" parseAACDes () Leaved!");

	return eSIxERR_OK;
}

static SIxError_e copyAACDes(SIxAACDes_t *pstDest, const SIxAACDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxAACDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}


/************************************************************************
 * eSIxDESCTAG_XAIT_LOCATION (TAG = 0x7D)
 ************************************************************************/
//XaitLocation
#if 0
static void initXaitLocationDes(SIxXaitLocationDes_t *pstDes)
{
}

static SIxXaitLocationDes_t *allocXaitLocationDes(void)
{
	return (SIxXaitLocationDes_t *)silib_desc_alloc(sizeof(SIxXaitLocationDes_t));
}

static void freeXaitLocationDes(SIxXaitLocationDes_t *pstDes)
{
}

static SIxError_e parseXaitLocationDes(HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxXaitLocationDes_t *pstDes)
{
}

static SIxError_e copyXaitLocationDes(SIxXaitLocationDes_t *pstDest, const SIxXaitLocationDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxXaitLocationDes_t));
	}

	return eErr;

err_exit:
	return eSIxERR_FAIL;
}
#endif

/************************************************************************
 * eSIxDESCTAG_FTA_CONTENT_MANAGEMENT (Tag = 0x7E)
 ************************************************************************/
static void initFtaContentManagementDes(SIxFtaContentManagementDes_t *pstDes)
{
	pstDes->ucDoNotScramble = 0xff;
	pstDes->ucDoNotApplyRevocation = 0xff;
	pstDes->ucCtrlRAccOverINet = 0xff;
}

static SIxFtaContentManagementDes_t *allocFtaContentManagementDes(void)
{
	return (SIxFtaContentManagementDes_t *)silib_desc_alloc(sizeof(SIxFtaContentManagementDes_t));
}

static void freeFtaContentManagementDes (SIxFtaContentManagementDes_t *pstDes)
{
	silib_desc_free(pstDes);
}

static SIxError_e parseFtaContentManagementDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxFtaContentManagementDes_t *pstDes)
{
	HUINT8		ucDesLen,ucValue;

	HxLOG_Info("parseFtaContentManagementDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseFtaContentManagementDes Format!\n");
		return eSIxERR_OK;
	}

	ucValue = *p;
	pstDes->ucDoNotScramble = (HUINT8)((ucValue >> 3) & 0x1);
	pstDes->ucCtrlRAccOverINet = (HUINT8)((ucValue >> 1) & 0x3);
	pstDes->ucDoNotApplyRevocation = (HUINT8)(ucValue & 0x1);

	HxLOG_Print("\t\t\t(Do not scramble = %d)\n", pstDes->ucDoNotScramble);
	HxLOG_Print("\t\t\t(Control Remote Access Over Internet = %d)\n", pstDes->ucCtrlRAccOverINet);
	HxLOG_Print("\t\t\t(Do not Apply Revocation = %d)\n", pstDes->ucDoNotApplyRevocation);

	HxLOG_Info(" parseFtaContentManagementDes () Leaved!");
	return eSIxERR_OK;
}

static SIxError_e copyFtaContentManagementDes(SIxFtaContentManagementDes_t *pstDest, const SIxFtaContentManagementDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxFtaContentManagementDes_t));
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/************************************************************************
 * Extension Descriptor 관련... (TAG = 0x7F)
 ************************************************************************/

static void initExtensionDes(SIxExtensionDes_t *pstDes, HUINT32 ulRawLen)
{
	HxSTD_MemSet(pstDes->pucRawData, 0, ulRawLen + 1); // ext tag len를 위해 +1
}

static SIxExtensionDes_t *allocExtensionDes (HUINT32 ulRawLen)
{
	SIxExtensionDes_t	*pstDes;

	pstDes = (SIxExtensionDes_t *)silib_desc_alloc(sizeof(SIxExtensionDes_t));
	if (NULL == pstDes)
	{
		HxLOG_Print("[ERROR] allocExtensionDes()\n");
		return NULL;
	}

	pstDes->pucRawData = (HUINT8 *)silib_desc_alloc(ulRawLen + 1);	// ext tag len를 위해 +1
	if (NULL == pstDes->pucRawData)
	{
		HxLOG_Print("[ERROR] allocExtensionDes()\n");
		return NULL;
	}

	return ((SIxExtensionDes_t *)pstDes);
}

static void freeExtensionDes (SIxExtensionDes_t *pstDes)
{
	if (NULL == pstDes) 	return;

	silib_desc_free(pstDes->pucRawData);
	silib_desc_free(pstDes);
}

static SIxError_e parseExtensionDes (HUINT8 *p, const SIxDescriptorHeaderParam_t *pstParam, SIxExtensionDes_t *pstDes)
{
	HUINT8				ucDesLen;

	HxLOG_Info("parseExtensionDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown parseExtensionDes Format!\n");
		return eSIxERR_OK;
	}

	pstDes->pucRawData[0] = *p++;		// ext tag
	ucDesLen--;

	pstDes->pucRawData[1] = ucDesLen;	// ext desc length
	HxSTD_MemCopy(&pstDes->pucRawData[2], p, ucDesLen);

	HxLOG_Print("\t\t(extended descriptor's tag = %d)\n", pstDes->pucRawData[0]);
	HxLOG_Print("\t\t(extended descriptor's length = %d)\n", pstDes->pucRawData[1]);

	HxLOG_Info(" parseExtensionDes () Leaved!");
	return eSIxERR_OK;
}

static SIxError_e copyExtensionDes(SIxExtensionDes_t *pstDest, const SIxExtensionDes_t *pstSource)
{
	SIxError_e		eErr = eSIxERR_FAIL;

	HxSTD_MemCopy(&pstDest->stHead, &pstSource->stHead, sizeof(SIxDescriptorHeader_t));
	if (0 < pstSource->stHead.ulLength && NULL != pstSource->pucRawData)
	{
		// 주의 pstDest->aucRawData는 pstDest alloc시 할당해주기 때문에 여기서 별도로 Alloc 하면 안됨...
		HxSTD_MemCopy(pstDest->pucRawData, pstSource->pucRawData, sizeof(HUINT8) * pstSource->stHead.ulLength);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

/* eSIxDESCTAG_VIDEO_STREAM						: (0x02)	*/
//VideoStmDes
static SIxDescriptor_t *silib_desc_allocVideoStmDes(void)
{
	SIxVideoStmDes_t	*pstDes;

	pstDes = allocVideoStmDes();
	if (pstDes)
	{
		initVideoStmDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeVideoStmDes(SIxDescriptor_t *pstDes)
{
	freeVideoStmDes((SIxVideoStmDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseVideoStmDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseVideoStmDes((HUINT8 *)pucRawData, pstParam, (SIxVideoStmDes_t *)pstDes);
}

static SIxError_e silib_desc_copyVideoStmDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyVideoStmDes((SIxVideoStmDes_t *)pstDest, (SIxVideoStmDes_t *)pstSource);
}

static void silib_desc_printVideoStmDes(const SIxDescriptor_t *pstDes)
{
	printVideoStmDes((SIxVideoStmDes_t *)pstDes);
}

/* eSIxDESCTAG_AUDIO_STREAM						: (0x03)	*/
//AudioStmDes
static SIxDescriptor_t *silib_desc_allocAudioStmDes(void)
{
	SIxAudioStmDes_t	*pstDes;

	pstDes = allocAudioStmDes();
	if (pstDes)
	{
		initAudioStmDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeAudioStmDes(SIxDescriptor_t *pstDes)
{
	freeAudioStmDes((SIxAudioStmDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseAudioStmDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseAudioStmDes((HUINT8 *)pucRawData, pstParam, (SIxAudioStmDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAudioStmDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAudioStmDes((SIxAudioStmDes_t *)pstDest, (const SIxAudioStmDes_t *)pstSource);
}

static void silib_desc_printAudioStmDes(const SIxDescriptor_t *pstDes)
{
	printAudioStmDes((SIxAudioStmDes_t *)pstDes);
}

/* eSIxDESCTAG_HIERARCHY							: (0x04)	*/
//HierDes
static SIxDescriptor_t *silib_desc_allocHierDes(void)
{
	SIxHierDes_t		*pstDes;

	pstDes = allocHierDes();
	if (pstDes)
	{
		initHierDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeHierDes(SIxDescriptor_t *pstDes)
{
	freeHierDes((SIxHierDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseHierDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseHierDes((HUINT8 *)pucRawData, pstParam, (SIxHierDes_t *)pstDes);
}

static SIxError_e silib_desc_copyHierDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyHierDes((SIxHierDes_t *)pstDest, (const SIxHierDes_t *)pstSource);
}

static void silib_desc_printHierDes(const SIxDescriptor_t *pstDes)
{
	SIxHierDes_t		*pstDesc;

	pstDesc = (SIxHierDes_t *)pstDes;
}

/* eSIxDESCTAG_REGISTRATION						: (0x05)	*/
//RegDes
static SIxDescriptor_t *silib_desc_allocRegDes(void)
{
	SIxRegDes_t		*pstDes;

	pstDes = allocRegDes();
	if (pstDes)
	{
		initRegDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeRegDes(SIxDescriptor_t *pstDes)
{
	freeRegDes((SIxRegDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseRegDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseRegDes((HUINT8 *)pucRawData, pstParam, (SIxRegDes_t *)pstDes);
}

static SIxError_e silib_desc_copyRegDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyRegDes((SIxRegDes_t *)pstDest, (const SIxRegDes_t *)pstSource);
}

static void silib_desc_printRegDes(const SIxDescriptor_t *pstDes)
{
	SIxRegDes_t		*pstDesc;

	pstDesc = (SIxRegDes_t *)pstDes;

}

/* eSIxDESCTAG_DATA_STREAM_ALIGNMENT				: (0x06)	*/
//DataStmAlignDes
static SIxDescriptor_t *silib_desc_allocDataStmAlignDes(void)
{
	SIxDataStmAlignDes_t	*pstDes;

	pstDes = allocDataStmAlignDes();
	if (pstDes)
	{
		initDataStmAlignDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeDataStmAlignDes(SIxDescriptor_t *pstDes)
{
	freeDataStmAlignDes((SIxDataStmAlignDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDataStmAlignDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDataStmAlignDes((HUINT8 *)pucRawData, pstParam, (SIxDataStmAlignDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDataStmAlignDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDataStmAlignDes((SIxDataStmAlignDes_t *)pstDest, (const SIxDataStmAlignDes_t *)pstSource);
}

static void silib_desc_printDataStmAlignDes(const SIxDescriptor_t *pstDes)
{
	SIxDataStmAlignDes_t		*pstDesc;

	pstDesc = (SIxDataStmAlignDes_t *)pstDes;

}

/* eSIxDESCTAG_TARGET_BACKGROUND_GRID				: (0x07)	*/
//BackGridDes
static SIxDescriptor_t *silib_desc_allocBackGridDes(void)
{
	SIxBackGridDes_t	*pstDes;

	pstDes = allocBackGridDes();
	if (pstDes)
	{
		initBackGridDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeBackGridDes(SIxDescriptor_t *pstDes)
{
	freeBackGridDes((SIxBackGridDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseBackGridDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseBackGridDes((HUINT8 *)pucRawData, pstParam, (SIxBackGridDes_t *)pstDes);
}

static SIxError_e silib_desc_copyBackGridDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyBackGridDes((SIxBackGridDes_t *)pstDest, (const SIxBackGridDes_t *)pstSource);
}

static void silib_desc_printBackGridDes(const SIxDescriptor_t *pstDes)
{
	SIxBackGridDes_t		*pstDesc;

	pstDesc = (SIxBackGridDes_t *)pstDes;

}


/* eSIxDESCTAG_VIDEO_WINDOW						: (0x08)	*/
//VideoWindowDes
static SIxDescriptor_t *silib_desc_allocVideoWindowDes(void)
{
	SIxVideoWindowDes_t	*pstDes;

	pstDes = allocVideoWindowDes();
	if (pstDes)
	{
		initVideoWindowDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeVideoWindowDes(SIxDescriptor_t *pstDes)
{
	freeVideoWindowDes((SIxVideoWindowDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseVideoWindowDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseVideoWindowDes((HUINT8 *)pucRawData, pstParam, (SIxVideoWindowDes_t *)pstDes);
}

static SIxError_e silib_desc_copyVideoWindowDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyVideoWindowDes((SIxVideoWindowDes_t *)pstDest, (const SIxVideoWindowDes_t *)pstSource);
}

static void silib_desc_printVideoWindowDes(const SIxDescriptor_t *pstDes)
{
	SIxVideoWindowDes_t		*pstDesc;

	pstDesc = (SIxVideoWindowDes_t *)pstDes;

}


/* eSIxDESCTAG_CA									: (0x09)	*/
//CaDes
static SIxDescriptor_t *silib_desc_allocCaDes(void)
{
	SIxCaDes_t		*pstDes;

	pstDes = allocCaDes();
	if (pstDes)
	{
		initCaDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCaDes(SIxDescriptor_t *pstDes)
{
	freeCaDes((SIxCaDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCaDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCaDes((HUINT8 *)pucRawData, pstParam, (SIxCaDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCaDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCaDes((SIxCaDes_t *)pstDest, (const SIxCaDes_t *)pstSource);
}

static void silib_desc_printCaDes(const SIxDescriptor_t *pstDes)
{
	SIxCaDes_t		*pstDesc;

	pstDesc = (SIxCaDes_t *)pstDes;

}


/* eSIxDESCTAG_ISO_639_LANGUAGE					: (0x0A)	*/
//Iso639LangDes
static SIxDescriptor_t *silib_desc_allocIso639LangDes(void)
{
	SIxIso639LangDes_t	*pstDes;

	pstDes = allocIso639LangDes();
	if (pstDes)
	{
		initIso639LangDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeIso639LangDes(SIxDescriptor_t *pstDes)
{
	freeIso639LangDes((SIxIso639LangDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseIso639LangDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseIso639LangDes((HUINT8 *)pucRawData, pstParam, (SIxIso639LangDes_t *)pstDes);
}

static SIxError_e silib_desc_copyIso639LangDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyIso639LangDes((SIxIso639LangDes_t *)pstDest, (const SIxIso639LangDes_t *)pstSource);
}

static void silib_desc_printIso639LangDes(const SIxDescriptor_t *pstDes)
{
	SIxIso639LangDes_t		*pstDesc;

	pstDesc = (SIxIso639LangDes_t *)pstDes;

}


/* eSIxDESCTAG_SYSTEM_CLOCK						: (0x0B)	*/
//SysClockDes
static SIxDescriptor_t *silib_desc_allocSysClockDes(void)
{
	SIxSysClockDes_t	*pstDes;

	pstDes = allocSysClockDes();
	if (pstDes)
	{
		initSysClockDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSysClockDes(SIxDescriptor_t *pstDes)
{
	freeSysClockDes((SIxSysClockDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSysClockDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSysClockDes((HUINT8 *)pucRawData, pstParam, (SIxSysClockDes_t *)pstDes);
}

static SIxError_e silib_desc_copySysClockDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySysClockDes((SIxSysClockDes_t *)pstDest, (const SIxSysClockDes_t *)pstSource);
}

static void silib_desc_printSysClockDes(const SIxDescriptor_t *pstDes)
{
	SIxSysClockDes_t		*pstDesc;

	pstDesc = (SIxSysClockDes_t *)pstDes;

}


/* eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION		: (0x0C)	*/
//MultiplexBufUtilDes
static SIxDescriptor_t *silib_desc_allocMultiplexBufUtilDes(void)
{
	SIxMultiplexBufUtilDes_t	*pstDes;

	pstDes = allocMultiplexBufUtilDes();
	if (pstDes)
	{
		initMultiplexBufUtilDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeMultiplexBufUtilDes(SIxDescriptor_t *pstDes)
{
	freeMultiplexBufUtilDes((SIxMultiplexBufUtilDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseMultiplexBufUtilDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseMultiplexBufUtilDes((HUINT8 *)pucRawData, pstParam, (SIxMultiplexBufUtilDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMultiplexBufUtilDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMultiplexBufUtilDes((SIxMultiplexBufUtilDes_t *)pstDest, (const SIxMultiplexBufUtilDes_t *)pstSource);
}

static void silib_desc_printMultiplexBufUtilDes(const SIxDescriptor_t *pstDes)
{
	SIxMultiplexBufUtilDes_t		*pstDesc;

	pstDesc = (SIxMultiplexBufUtilDes_t *)pstDes;

}


/* eSIxDESCTAG_COPYRIGHT							: (0x0D)	*/
//CopyRightDes
static SIxDescriptor_t *silib_desc_allocCopyRightDes(void)
{
	SIxCopyRightDes_t	*pstDes;

	pstDes = allocCopyRightDes();
	if (pstDes)
	{
		initCopyRightDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCopyRightDes(SIxDescriptor_t *pstDes)
{
	freeCopyRightDes((SIxCopyRightDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCopyRightDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCopyRightDes((HUINT8 *)pucRawData, pstParam, (SIxCopyRightDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCopyRightDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCopyRightDes((SIxCopyRightDes_t *)pstDest, (const SIxCopyRightDes_t *)pstSource);
}

static void silib_desc_printCopyRightDes(const SIxDescriptor_t *pstDes)
{
	SIxCopyRightDes_t		*pstDesc;

	pstDesc = (SIxCopyRightDes_t *)pstDes;

}


/* eSIxDESCTAG_MAXIMUM_BITRATE						: (0x0E)	*/
//MaxBitrateDes
static SIxDescriptor_t *silib_desc_allocMaxBitrateDes(void)
{
	SIxMaxBitrateDes_t	*pstDes;

	pstDes = allocMaxBitrateDes();
	if (pstDes)
	{
		initMaxBitrateDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeMaxBitrateDes(SIxDescriptor_t *pstDes)
{
	freeMaxBitrateDes((SIxMaxBitrateDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseMaxBitrateDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseMaxBitrateDes((HUINT8 *)pucRawData, pstParam, (SIxMaxBitrateDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMaxBitrateDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMaxBitrateDes((SIxMaxBitrateDes_t *)pstDest, (const SIxMaxBitrateDes_t *)pstSource);
}

static void silib_desc_printMaxBitrateDes(const SIxDescriptor_t *pstDes)
{
	SIxMaxBitrateDes_t		*pstDesc;

	pstDesc = (SIxMaxBitrateDes_t *)pstDes;

}


/* eSIxDESCTAG_PRIVATE_DATA_INDICATOR				: (0x0F)	*/
//PrivateDataIndicatorDes
static SIxDescriptor_t *silib_desc_allocPrivateDataIndicatorDes(void)
{
	SIxPrivateDataIndicatorDes_t	*pstDes;

	pstDes = allocPrivateDataIndicatorDes();
	if (pstDes)
	{
		initPrivateDataIndicatorDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freePrivateDataIndicatorDes(SIxDescriptor_t *pstDes)
{
	freePrivateDataIndicatorDes((SIxPrivateDataIndicatorDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parsePrivateDataIndicatorDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parsePrivateDataIndicatorDes((HUINT8 *)pucRawData, pstParam, (SIxPrivateDataIndicatorDes_t *)pstDes);
}

static SIxError_e silib_desc_copyPrivateDataIndicatorDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPrivateDataIndicatorDes((SIxPrivateDataIndicatorDes_t *)pstDest, (const SIxPrivateDataIndicatorDes_t *)pstSource);
}

static void silib_desc_printPrivateDataIndicatorDes(const SIxDescriptor_t *pstDes)
{
	SIxPrivateDataIndicatorDes_t		*pstDesc;

	pstDesc = (SIxPrivateDataIndicatorDes_t *)pstDes;

}


/* eSIxDESCTAG_SMOOTHING_BUFFER					: (0x10)	*/
//SmoothBufDes
static SIxDescriptor_t *silib_desc_allocSmoothBufDes(void)
{
	SIxSmoothBufDes_t	*pstDes;

	pstDes = allocSmoothBufDes();
	if (pstDes)
	{
		initSmoothBufDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSmoothBufDes(SIxDescriptor_t *pstDes)
{
	freeSmoothBufDes((SIxSmoothBufDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSmoothBufDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSmoothBufDes((HUINT8 *)pucRawData, pstParam, (SIxSmoothBufDes_t *)pstDes);
}

static SIxError_e silib_desc_copySmoothBufDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySmoothBufDes((SIxSmoothBufDes_t *)pstDest, (const SIxSmoothBufDes_t *)pstSource);
}

static void silib_desc_printSmoothBufDes(const SIxDescriptor_t *pstDes)
{
	SIxSmoothBufDes_t		*pstDesc;

	pstDesc = (SIxSmoothBufDes_t *)pstDes;

}


/* eSIxDESCTAG_STD									: (0x11)	*/
//StdDes
static SIxDescriptor_t *silib_desc_allocStdDes(void)
{
	SIxStdDes_t		*pstDes;

	pstDes = allocStdDes();
	if (pstDes)
	{
		initStdDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeStdDes(SIxDescriptor_t *pstDes)
{
	freeStdDes((SIxStdDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseStdDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseStdDes((HUINT8 *)pucRawData, pstParam, (SIxStdDes_t *)pstDes);
}

static SIxError_e silib_desc_copyStdDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyStdDes((SIxStdDes_t *)pstDest, (const SIxStdDes_t *)pstSource);
}

static void silib_desc_printStdDes(const SIxDescriptor_t *pstDes)
{
	SIxStdDes_t		*pstDesc;

	pstDesc = (SIxStdDes_t *)pstDes;

}


/* eSIxDESCTAG_CAROUSEL_ID							: (0x13)	*/
//CarouselIdDes
static SIxDescriptor_t *silib_desc_allocCarouselIdDes(void)
{
	SIxCarouselIdDes_t	*pstDes;

	pstDes = allocCarouselIdDes();
	if (pstDes)
	{
		initCarouselIdDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCarouselIdDes(SIxDescriptor_t *pstDes)
{
	freeCarouselIdDes((SIxCarouselIdDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCarouselIdDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCarouselIdDes((HUINT8 *)pucRawData, pstParam, (SIxCarouselIdDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCarouselIdDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCarouselIdDes((SIxCarouselIdDes_t *)pstDest, (const SIxCarouselIdDes_t *)pstSource);
}

static void silib_desc_printCarouselIdDes(const SIxDescriptor_t *pstDes)
{
	SIxCarouselIdDes_t		*pstDesc;

	pstDesc = (SIxCarouselIdDes_t *)pstDes;

}


/* eSIxDESCTAG_ASSOCIATION_TAG						: (0x14)	*/
//AssocTagDes
static SIxDescriptor_t *silib_desc_allocAssocTagDes(void)
{
	SIxAssocTagDes_t	*pstDes;

	pstDes = allocAssocTagDes();
	if (pstDes)
	{
		initAssocTagDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeAssocTagDes(SIxDescriptor_t *pstDes)
{
	freeAssocTagDes((SIxAssocTagDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseAssocTagDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseAssocTagDes((HUINT8 *)pucRawData, pstParam, (SIxAssocTagDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAssocTagDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAssocTagDes((SIxAssocTagDes_t *)pstDest, (const SIxAssocTagDes_t *)pstSource);
}

static void silib_desc_printAssocTagDes(const SIxDescriptor_t *pstDes)
{
	SIxAssocTagDes_t		*pstDesc;

	pstDesc = (SIxAssocTagDes_t *)pstDes;

}


/* eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG			: (0x15)	*/
//DeferAssocTagDes
static SIxDescriptor_t *silib_desc_allocDeferAssocTagDes(void)
{
	SIxDeferAssocTagDes_t	*pstDes;

	pstDes = allocDeferAssocTagDes();
	if (pstDes)
	{
		initDeferAssocTagDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeDeferAssocTagDes(SIxDescriptor_t *pstDes)
{
	freeDeferAssocTagDes((SIxDeferAssocTagDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseDeferAssocTagDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDeferAssocTagDes((HUINT8 *)pucRawData, pstParam, (SIxDeferAssocTagDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDeferAssocTagDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDeferAssocTagDes((SIxDeferAssocTagDes_t *)pstDest, (const SIxDeferAssocTagDes_t *)pstSource);
}

static void silib_desc_printDeferAssocTagDes(const SIxDescriptor_t *pstDes)
{
	SIxDeferAssocTagDes_t		*pstDesc;

	pstDesc = (SIxDeferAssocTagDes_t *)pstDes;

}


/* eSIxDESCTAG_AVC_VIDEO 							: (0x28)	*/
//AvcVideoDes
static SIxDescriptor_t *silib_desc_allocAvcVideoDes(void)
{
	SIxAvcVideoDes_t	*pstDes;

	pstDes = allocAvcVideoDes();
	if (pstDes)
	{
		initAvcVideoDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeAvcVideoDes(SIxDescriptor_t *pstDes)
{
	freeAvcVideoDes((SIxAvcVideoDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseAvcVideoDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseAvcVideoDes((HUINT8 *)pucRawData, pstParam, (SIxAvcVideoDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAvcVideoDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAvcVideoDes((SIxAvcVideoDes_t *)pstDest, (const SIxAvcVideoDes_t *)pstSource);
}

static void silib_desc_printAvcVideoDes(const SIxDescriptor_t *pstDes)
{
	SIxAvcVideoDes_t		*pstDesc;

	pstDesc = (SIxAvcVideoDes_t *)pstDes;

}


/** < ETS 300 468에 정의된 descriptor tag */
/* eSIxDESCTAG_NETWORK_NAME						: (0x40)	*/
//NetNameDes
static SIxDescriptor_t *silib_desc_allocNetNameDes(void)
{
	SIxNetNameDes_t	*pstDes;

	pstDes = allocNetNameDes();
	if (pstDes)
	{
		initNetNameDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeNetNameDes(SIxDescriptor_t *pstDes)
{
	freeNetNameDes((SIxNetNameDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseNetNameDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseNetNameDes((HUINT8 *)pucRawData, pstParam, (SIxNetNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyNetNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyNetNameDes((SIxNetNameDes_t *)pstDest, (const SIxNetNameDes_t *)pstSource);
}

static void silib_desc_printNetNameDes(const SIxDescriptor_t *pstDes)
{
	SIxNetNameDes_t		*pstDesc;

	pstDesc = (SIxNetNameDes_t *)pstDes;

}


/* eSIxDESCTAG_SERVICE_LIST						: (0x41)	*/
//SvcListDes
static SIxDescriptor_t *silib_desc_allocSvcListDes(void)
{
	SIxSvcListDes_t	*pstDes;

	pstDes = allocSvcListDes(sizeof(SIxSvcListDes_t));
	if (pstDes)
	{
		initSvcListDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSvcListDes(SIxDescriptor_t *pstDes)
{
	freeSvcListDes((SIxSvcListDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSvcListDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSvcListDes((HUINT8 *)pucRawData, pstParam, (SIxSvcListDes_t *)pstDes);
}

static SIxError_e silib_desc_copySvcListDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySvcListDes((SIxSvcListDes_t *)pstDest, (const SIxSvcListDes_t *)pstSource);
}

static void silib_desc_printSvcListDes(const SIxDescriptor_t *pstDes)
{
	SIxSvcListDes_t		*pstDesc;

	pstDesc = (SIxSvcListDes_t *)pstDes;

}


/* eSIxDESCTAG_STUFFING							: (0x42)	*/
//StuffingDes
#if 0
static SIxDescriptor_t *silib_desc_allocStuffingDes(void)
{
	SIxStuffingDes_t		*pstDes;

	pstDes = allocStuffingDes(sizeof(SIxStuffingDes_t));
	if (pstDes)
	{
		initStuffingDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeStuffingDes(SIxDescriptor_t *pstDes)
{
	freeStuffingDes((SIxStuffingDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseStuffingDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseStuffingDes((HUINT8 *)pucRawData, pstParam, (SIxStuffingDes_t *)pstDes);
}

static SIxError_e silib_desc_copyStuffingDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyStuffingDes((SIxStuffingDes_t *)pstDest, (const SIxStuffingDes_t *)pstSource);
}

static void silib_desc_printSIxStuffingDes_t(const SIxDescriptor_t *pstDes)
{
	SIxStuffingDes_t		*pstDesc;

	pstDesc = (SIxStuffingDes_t *)pstDes;

}
#endif

/* eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM			: (0x43)	*/
//SatDeliSystemDes
static SIxDescriptor_t *silib_desc_allocSatDeliSystemDes(void)
{
	SIxSatDeliSystemDes_t	*pstDes;

	pstDes = allocSatDeliSystemDes();
	if (pstDes)
	{
		initSatDeliSystemDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSatDeliSystemDes(SIxDescriptor_t *pstDes)
{
	freeSatDeliSystemDes((SIxSatDeliSystemDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSatDeliSystemDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSatDeliSystemDes((HUINT8 *)pucRawData, pstParam, (SIxSatDeliSystemDes_t *)pstDes);
}

static SIxError_e silib_desc_copySatDeliSystemDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySatDeliSystemDes((SIxSatDeliSystemDes_t *)pstDest, (const SIxSatDeliSystemDes_t *)pstSource);
}

static void silib_desc_printSatDeliSystemDes(const SIxDescriptor_t *pstDes)
{
	SIxSatDeliSystemDes_t		*pstDesc;

	pstDesc = (SIxSatDeliSystemDes_t *)pstDes;

}


/* eSIxDESCTAG_CABLE_DELIVERY_SYSTEM				: (0x44)	*/
//CabDeliSystemDes
static SIxDescriptor_t *silib_desc_allocCabDeliSystemDes(void)
{
	SIxCabDeliSystemDes_t	*pstDes;

	pstDes = allocCabDeliSystemDes();
	if (pstDes)
	{
		initCabDeliSystemDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCabDeliSystemDes(SIxDescriptor_t *pstDes)
{
	freeCabDeliSystemDes((SIxCabDeliSystemDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCabDeliSystemDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCabDeliSystemDes((HUINT8 *)pucRawData, pstParam, (SIxCabDeliSystemDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCabDeliSystemDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCabDeliSystemDes((SIxCabDeliSystemDes_t *)pstDest, (const SIxCabDeliSystemDes_t *)pstSource);
}

static void silib_desc_printCabDeliSystemDes(const SIxDescriptor_t *pstDes)
{
	SIxCabDeliSystemDes_t		*pstDesc;

	pstDesc = (SIxCabDeliSystemDes_t *)pstDes;

}


/* eSIxDESCTAG_BOUQUET_NAME						: (0x47)	*/
//BouqNameDes
static SIxDescriptor_t *silib_desc_allocBouqNameDes(void)
{
	SIxBouqNameDes_t	*pstDes;

	pstDes = allocBouqNameDes();
	if (pstDes)
	{
		initBouqNameDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeBouqNameDes(SIxDescriptor_t *pstDes)
{
	freeBouqNameDes((SIxBouqNameDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseBouqNameDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseBouqNameDes((HUINT8 *)pucRawData, pstParam, (SIxBouqNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyBouqNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyBouqNameDes((SIxBouqNameDes_t *)pstDest, (const SIxBouqNameDes_t *)pstSource);
}

static void silib_desc_printBouqNameDes(const SIxDescriptor_t *pstDes)
{
	SIxBouqNameDes_t		*pstDesc;

	pstDesc = (SIxBouqNameDes_t *)pstDes;

}


/* eSIxDESCTAG_SERVICE								: (0x48)	*/
//SvcDes
static SIxDescriptor_t *silib_desc_allocSvcDes(void)
{
	SIxSvcDes_t		*pstDes;

	pstDes = allocSvcDes();
	if (pstDes)
	{
		initSvcDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSvcDes(SIxDescriptor_t *pstDes)
{
	freeSvcDes((SIxSvcDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseSvcDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSvcDes((HUINT8 *)pucRawData, pstParam, (SIxSvcDes_t *)pstDes);
}

static SIxError_e silib_desc_copySvcDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySvcDes((SIxSvcDes_t *)pstDest, (const SIxSvcDes_t *)pstSource);
}

static void silib_desc_printSvcDes(const SIxDescriptor_t *pstDes)
{
	SIxSvcDes_t		*pstDesc;

	pstDesc = (SIxSvcDes_t *)pstDes;

}


/* eSIxDESCTAG_COUNTRY_AVAILABILITY				: (0x49)	*/
//CountryAvailDes
static SIxDescriptor_t *silib_desc_allocCountryAvailDes(void)
{
	SIxCountryAvailDes_t	*pstDes;

	pstDes = allocCountryAvailDes();
	if (pstDes)
	{
		initCountryAvailDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCountryAvailDes(SIxDescriptor_t *pstDes)
{
	freeCountryAvailDes((SIxCountryAvailDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCountryAvailDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCountryAvailDes((HUINT8 *)pucRawData, pstParam, (SIxCountryAvailDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCountryAvailDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCountryAvailDes((SIxCountryAvailDes_t *)pstDest, (const SIxCountryAvailDes_t *)pstSource);
}

static void silib_desc_printCountryAvailDes(const SIxDescriptor_t *pstDes)
{
	SIxCountryAvailDes_t		*pstDesc;

	pstDesc = (SIxCountryAvailDes_t *)pstDes;

}


/* eSIxDESCTAG_LINKAGE								: (0x4A)	*/
//LinkDes
static SIxDescriptor_t *silib_desc_allocLinkDes(void)
{
	SIxLinkDes_t		*pstDes;

	pstDes = allocLinkDes();
	if (pstDes)
	{
		initLinkDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeLinkDes(SIxDescriptor_t *pstDes)
{
	freeLinkDes((SIxLinkDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseLinkDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseLinkDes((HUINT8 *)pucRawData, pstParam, (SIxLinkDes_t *)pstDes);
}

static SIxError_e silib_desc_copyLinkDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLinkDes((SIxLinkDes_t *)pstDest, (const SIxLinkDes_t *)pstSource);
}

static void silib_desc_printLinkDes(const SIxDescriptor_t *pstDes)
{
	SIxLinkDes_t		*pstDesc;

	pstDesc = (SIxLinkDes_t *)pstDes;

}


/* eSIxDESCTAG_NVOD_REFERENCE						: (0x4B)	*/
//NvodRefDes
static SIxDescriptor_t *silib_desc_allocNvodRefDes(void)
{
	SIxNvodRefDes_t	*pstDes;

	pstDes = allocNvodRefDes();
	if (pstDes)
	{
		initNvodRefDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeNvodRefDes(SIxDescriptor_t *pstDes)
{
	freeNvodRefDes((SIxNvodRefDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseNvodRefDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseNvodRefDes((HUINT8 *)pucRawData, pstParam, (SIxNvodRefDes_t *)pstDes);
}

static SIxError_e silib_desc_copyNvodRefDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyNvodRefDes((SIxNvodRefDes_t *)pstDest, (const SIxNvodRefDes_t *)pstSource);
}

static void silib_desc_printNvodRefDes(const SIxDescriptor_t *pstDes)
{
	SIxNvodRefDes_t		*pstDesc;

	pstDesc = (SIxNvodRefDes_t *)pstDes;

}


/* eSIxDESCTAG_TIME_SHIFTED_SERVICE				: (0x4C)	*/
//TimeShiftSvcDes
static SIxDescriptor_t *silib_desc_allocTimeShiftSvcDes(void)
{
	SIxTimeShiftSvcDes_t	*pstDes;

	pstDes = allocTimeShiftSvcDes();
	if (pstDes)
	{
		initTimeShiftSvcDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeTimeShiftSvcDes(SIxDescriptor_t *pstDes)
{
	freeTimeShiftSvcDes((SIxTimeShiftSvcDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseTimeShiftSvcDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseTimeShiftSvcDes((HUINT8 *)pucRawData, pstParam, (SIxTimeShiftSvcDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTimeShiftSvcDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTimeShiftSvcDes((SIxTimeShiftSvcDes_t *)pstDest, (const SIxTimeShiftSvcDes_t *)pstSource);
}

static void silib_desc_printTimeShiftSvcDes(const SIxDescriptor_t *pstDes)
{
	SIxTimeShiftSvcDes_t		*pstDesc;

	pstDesc = (SIxTimeShiftSvcDes_t *)pstDes;

}


/* eSIxDESCTAG_SHORT_EVENT							: (0x4D)	*/
//ShortEvtDes
static SIxDescriptor_t *silib_desc_allocShortEvtDes(void)
{
	SIxShortEvtDes_t		*pstDes;

	pstDes = allocShortEvtDes();
	if (pstDes)
	{
		initShortEvtDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeShortEvtDes(SIxDescriptor_t *pstDes)
{
	freeShortEvtDes((SIxShortEvtDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseShortEvtDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseShortEvtDes((HUINT8 *)pucRawData, pstParam, (SIxShortEvtDes_t *)pstDes);
}

static SIxError_e silib_desc_copyShortEvtDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyShortEvtDes((SIxShortEvtDes_t *)pstDest, (const SIxShortEvtDes_t *)pstSource);
}

static void silib_desc_printShortEvtDes(const SIxDescriptor_t *pstDes)
{
	SIxShortEvtDes_t		*pstDesc;

	pstDesc = (SIxShortEvtDes_t *)pstDes;

}


/* eSIxDESCTAG_EXTENDED_EVENT						: (0x4E)	*/
//ExtEvtDes
static SIxDescriptor_t *silib_desc_allocExtEvtDes(void)
{
	SIxExtEvtDes_t	*pstDes;

	pstDes = allocExtEvtDes();
	if (pstDes)
	{
		initExtEvtDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeExtEvtDes(SIxDescriptor_t *pstDes)
{
	freeExtEvtDes((SIxExtEvtDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseExtEvtDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseExtEvtDes((HUINT8 *)pucRawData, pstParam, (SIxExtEvtDes_t *)pstDes);
}

static SIxError_e silib_desc_copyExtEvtDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyExtEvtDes((SIxExtEvtDes_t *)pstDest, (const SIxExtEvtDes_t *)pstSource);
}

static void silib_desc_printExtEvtDes(const SIxDescriptor_t *pstDes)
{
	SIxExtEvtDes_t		*pstDesc;

	pstDesc = (SIxExtEvtDes_t *)pstDes;

}


/* eSIxDESCTAG_TIME_SHIFTED_EVENT					: (0x4F)	*/
//TimeShiftEvtDes
static SIxDescriptor_t *silib_desc_allocTimeShiftEvtDes(void)
{
	SIxTimeShiftEvtDes_t	*pstDes;

	pstDes = allocTimeShiftEvtDes();
	if (pstDes)
	{
		initTimeShiftEvtDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeTimeShiftEvtDes(SIxDescriptor_t *pstDes)
{
	freeTimeShiftEvtDes((SIxTimeShiftEvtDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseTimeShiftEvtDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseTimeShiftEvtDes((HUINT8 *)pucRawData, pstParam, (SIxTimeShiftEvtDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTimeShiftEvtDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTimeShiftEvtDes((SIxTimeShiftEvtDes_t *)pstDest, (const SIxTimeShiftEvtDes_t *)pstSource);
}

static void silib_desc_printTimeShiftEvtDes(const SIxDescriptor_t *pstDes)
{
	SIxTimeShiftEvtDes_t		*pstDesc;

	pstDesc = (SIxTimeShiftEvtDes_t *)pstDes;

}


/* eSIxDESCTAG_COMPONENT							: (0x50)	*/
//CompDes
static SIxDescriptor_t *silib_desc_allocCompDes(void)
{
	SIxCompDes_t		*pstDes;

	pstDes = allocCompDes();
	if (pstDes)
	{
		initCompDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCompDes(SIxDescriptor_t *pstDes)
{
	freeCompDes((SIxCompDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseCompDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCompDes((HUINT8 *)pucRawData, pstParam, (SIxCompDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCompDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCompDes((SIxCompDes_t *)pstDest, (const SIxCompDes_t *)pstSource);
}

static void silib_desc_printCompDes(const SIxDescriptor_t *pstDes)
{
	SIxCompDes_t		*pstDesc;

	pstDesc = (SIxCompDes_t *)pstDes;

}


/* eSIxDESCTAG_MOSAIC								: (0x51)	*/
//MosaicDes
#if 0
static SIxDescriptor_t *silib_desc_allocMosaicDes(void)
{
	SIxMosaicDes_t	*pstDes;

	pstDes = allocMosaicDes();
	if (pstDes)
	{
		initMosaicDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeMosaicDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeMosaicDes((SIxMosaicDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseMosaicDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseMosaicDes((HUINT8 *)pucRawData, pstParam, (SIxMosaicDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMosaicDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e eErr;

	return copyMosaicDes((SIxMosaicDes_t *)pstDest, (const SIxMosaicDes_t *)pstSource);

	return eErr;
}

static void silib_desc_printMosaicDes(const SIxDescriptor_t *pstDes)
{
	SIxMosaicDes_t		*pstDesc;

	pstDesc = (SIxMosaicDes_t *)pstDes;

}
#endif

/* eSIxDESCTAG_STREAM_IDENTIFIER					: (0x52)	*/
//StmIdentDes
static SIxDescriptor_t *silib_desc_allocStmIdentDes(void)
{
	SIxStmIdentDes_t	*pstDes;

	pstDes = allocStmIdentDes();
	if (pstDes)
	{
		initStmIdentDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeStmIdentDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeStmIdentDes((SIxStmIdentDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseStmIdentDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseStmIdentDes((HUINT8 *)pucRawData, pstParam, (SIxStmIdentDes_t *)pstDes);
}

static SIxError_e silib_desc_copyStmIdentDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyStmIdentDes((SIxStmIdentDes_t *)pstDest, (const SIxStmIdentDes_t *)pstSource);
}

static void silib_desc_printStmIdentDes(const SIxDescriptor_t *pstDes)
{
	SIxStmIdentDes_t		*pstDesc;

	pstDesc = (SIxStmIdentDes_t *)pstDes;

}


/* eSIxDESCTAG_CA_IDENTIFIER						: (0x53)	*/
//CaIdentDes
static SIxDescriptor_t *silib_desc_allocCaIdentDes(void)
{
	SIxCaIdentDes_t	*pstDes;

	pstDes = allocCaIdentDes();
	if (pstDes)
	{
		initCaIdentDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCaIdentDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeCaIdentDes((SIxCaIdentDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseCaIdentDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCaIdentDes((HUINT8 *)pucRawData, pstParam, (SIxCaIdentDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCaIdentDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCaIdentDes((SIxCaIdentDes_t *)pstDest, (const SIxCaIdentDes_t *)pstSource);
}

static void silib_desc_printCaIdentDes(const SIxDescriptor_t *pstDes)
{
	SIxCaIdentDes_t		*pstDesc;

	pstDesc = (SIxCaIdentDes_t *)pstDes;

}


/* eSIxDESCTAG_CONTENT								: (0x54)	*/
//ContentDes
static SIxDescriptor_t *silib_desc_allocContentDes(void)
{
	SIxContentDes_t	*pstDes;

	pstDes = allocContentDes();
	if (pstDes)
	{
		initContentDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeContentDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeContentDes((SIxContentDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseContentDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseContentDes((HUINT8 *)pucRawData, pstParam, (SIxContentDes_t *)pstDes);
}

static SIxError_e silib_desc_copyContentDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyContentDes((SIxContentDes_t *)pstDest, (const SIxContentDes_t *)pstSource);
}

static void silib_desc_printContentDes(const SIxDescriptor_t *pstDes)
{
	SIxContentDes_t		*pstDesc;

	pstDesc = (SIxContentDes_t *)pstDes;

}


/* eSIxDESCTAG_PARENTAL_RATING						: (0x55)	*/
//ParentDes
static SIxDescriptor_t *silib_desc_allocParentDes(void)
{
	SIxParentDes_t	*pstDes;

	pstDes = allocParentDes();
	if (pstDes)
	{
		initParentDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeParentDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeParentDes((SIxParentDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseParentDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseParentDes((HUINT8 *)pucRawData, pstParam, (SIxParentDes_t *)pstDes);
}

static SIxError_e silib_desc_copyParentDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyParentDes((SIxParentDes_t *)pstDest, (const SIxParentDes_t *)pstSource);
}

static void silib_desc_printParentDes(const SIxDescriptor_t *pstDes)
{
	SIxParentDes_t		*pstDesc;

	pstDesc = (SIxParentDes_t *)pstDes;

}


/* eSIxDESCTAG_TELETEXT							: (0x56)	*/
//TeletextDes
static SIxDescriptor_t *silib_desc_allocTeletextDes(void)
{
	SIxTeletextDes_t	*pstDes;

	pstDes = allocTeletextDes();
	if (pstDes)
	{
		initTeletextDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeTeletextDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeTeletextDes((SIxTeletextDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseTeletextDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseTeletextDes((HUINT8 *)pucRawData, pstParam, (SIxTeletextDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTeletextDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTeletextDes((SIxTeletextDes_t *)pstDest, (const SIxTeletextDes_t *)pstSource);
}

static void silib_desc_printTeletextDes(const SIxDescriptor_t *pstDes)
{
	SIxTeletextDes_t		*pstDesc;

	pstDesc = (SIxTeletextDes_t *)pstDes;

}


/* eSIxDESCTAG_TELEPHONE							: (0x57)	*/
//TelephoneDes
static SIxDescriptor_t *silib_desc_allocTelephoneDes(void)
{
	SIxTelephoneDes_t	*pstDes;

	pstDes = allocTelephoneDes();
	if (pstDes)
	{
		initTelephoneDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeTelephoneDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeTelephoneDes((SIxTelephoneDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseTelephoneDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseTelephoneDes((HUINT8 *)pucRawData, pstParam, (SIxTelephoneDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTelephoneDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTelephoneDes((SIxTelephoneDes_t *)pstDest, (const SIxTelephoneDes_t *)pstSource);
}

static void silib_desc_printTelephoneDes(const SIxDescriptor_t *pstDes)
{
	SIxTelephoneDes_t		*pstDesc;

	pstDesc = (SIxTelephoneDes_t *)pstDes;

}


/* eSIxDESCTAG_LOCAL_TIME_OFFSET					: (0x58)	*/
//LocalTimeDes
static SIxDescriptor_t *silib_desc_allocLocalTimeDes(void)
{
	SIxLocalTimeDes_t	*pstDes;

	pstDes = allocLocalTimeDes();
	if (pstDes)
	{
		initLocalTimeDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeLocalTimeDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeLocalTimeDes((SIxLocalTimeDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseLocalTimeDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseLocalTimeDes((HUINT8 *)pucRawData, pstParam, (SIxLocalTimeDes_t *)pstDes);
}

static SIxError_e silib_desc_copyLocalTimeDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyLocalTimeDes((SIxLocalTimeDes_t *)pstDest, (const SIxLocalTimeDes_t *)pstSource);
}

static void silib_desc_printLocalTimeDes(const SIxDescriptor_t *pstDes)
{
	SIxLocalTimeDes_t		*pstDesc;

	pstDesc = (SIxLocalTimeDes_t *)pstDes;

}


/* eSIxDESCTAG_SUBTITLING							: (0x59)	*/
//SubtitleDes
static SIxDescriptor_t *silib_desc_allocSubtitleDes(void)
{
	SIxSubtitleDes_t	*pstDes;

	pstDes = allocSubtitleDes();
	if (pstDes)
	{
		initSubtitleDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSubtitleDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeSubtitleDes((SIxSubtitleDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseSubtitleDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSubtitleDes((HUINT8 *)pucRawData, pstParam, (SIxSubtitleDes_t *)pstDes);
}

static SIxError_e silib_desc_copySubtitleDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySubtitleDes((SIxSubtitleDes_t *)pstDest, (const SIxSubtitleDes_t *)pstSource);
}

static void silib_desc_printSubtitleDes(const SIxDescriptor_t *pstDes)
{
	SIxSubtitleDes_t		*pstDesc;

	pstDesc = (SIxSubtitleDes_t *)pstDes;

}


/* eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM			: (0x5A)	*/
//TerDeliSystemDes
static SIxDescriptor_t *silib_desc_allocTerDeliSystemDes(void)
{
	SIxTerDeliSystemDes_t	*pstDes;

	pstDes = allocTerDeliSystemDes();
	if (pstDes)
	{
		initTerDeliSystemDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeTerDeliSystemDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeTerDeliSystemDes((SIxTerDeliSystemDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseTerDeliSystemDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseTerDeliSystemDes((HUINT8 *)pucRawData, pstParam, (SIxTerDeliSystemDes_t *)pstDes);
}

static SIxError_e silib_desc_copyTerDeliSystemDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyTerDeliSystemDes((SIxTerDeliSystemDes_t *)pstDest, (const SIxTerDeliSystemDes_t *)pstSource);
}

static void silib_desc_printTerDeliSystemDes(const SIxDescriptor_t *pstDes)
{
	SIxTerDeliSystemDes_t		*pstDesc;

	pstDesc = (SIxTerDeliSystemDes_t *)pstDes;

}


/* eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME			: (0x5B)	*/
//MultiNetNameDes
static SIxDescriptor_t *silib_desc_allocMultiNetNameDes(void)
{
	SIxMultiNetNameDes_t	*pstDes;

	pstDes = allocMultiNetNameDes();
	if (pstDes)
	{
		initMultiNetNameDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeMultiNetNameDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeMultiNetNameDes((SIxMultiNetNameDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseMultiNetNameDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseMultiNetNameDes((HUINT8 *)pucRawData, pstParam, (SIxMultiNetNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMultiNetNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMultiNetNameDes((SIxMultiNetNameDes_t *)pstDest, (const SIxMultiNetNameDes_t *)pstSource);
}

static void silib_desc_printMultiNetNameDes(const SIxDescriptor_t *pstDes)
{
	SIxMultiNetNameDes_t		*pstDesc;

	pstDesc = (SIxMultiNetNameDes_t *)pstDes;

}


/* eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME			: (0x5C)	*/
//MultiBouqNameDes
static SIxDescriptor_t *silib_desc_allocMultiBouqNameDes(void)
{
	SIxMultiBouqNameDes_t	*pstDes;

	pstDes = allocMultiBouqNameDes();
	if (pstDes)
	{
		initMultiBouqNameDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeMultiBouqNameDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeMultiBouqNameDes((SIxMultiBouqNameDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseMultiBouqNameDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseMultiBouqNameDes((HUINT8 *)pucRawData, pstParam, (SIxMultiBouqNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMultiBouqNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMultiBouqNameDes((SIxMultiBouqNameDes_t *)pstDest, (const SIxMultiBouqNameDes_t *)pstSource);
}

static void silib_desc_printMultiBouqNameDes(const SIxDescriptor_t *pstDes)
{
	SIxMultiBouqNameDes_t		*pstDesc;

	pstDesc = (SIxMultiBouqNameDes_t *)pstDes;

}


/* eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME			: (0x5D)	*/
//MultiSvcNameDes
static SIxDescriptor_t *silib_desc_allocMultiSvcNameDes(void)
{
	SIxMultiSvcNameDes_t	*pstDes;

	pstDes = allocMultiSvcNameDes();
	if (pstDes)
	{
		initMultiSvcNameDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeMultiSvcNameDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeMultiSvcNameDes((SIxMultiSvcNameDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseMultiSvcNameDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseMultiSvcNameDes((HUINT8 *)pucRawData, pstParam, (SIxMultiSvcNameDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMultiSvcNameDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMultiSvcNameDes((SIxMultiSvcNameDes_t *)pstDest, (const SIxMultiSvcNameDes_t *)pstSource);
}

static void silib_desc_printMultiSvcNameDes(const SIxDescriptor_t *pstDes)
{
	SIxMultiSvcNameDes_t		*pstDesc;

	pstDesc = (SIxMultiSvcNameDes_t *)pstDes;

}


/* eSIxDESCTAG_MULTILINGUAL_COMPONENT				: (0x5E)	*/
//MultiCompDes
static SIxDescriptor_t *silib_desc_allocMultiCompDes(void)
{
	SIxMultiCompDes_t	*pstDes;

	pstDes = allocMultiCompDes();
	if (pstDes)
	{
		initMultiCompDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeMultiCompDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeMultiCompDes((SIxMultiCompDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseMultiCompDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseMultiCompDes((HUINT8 *)pucRawData, pstParam, (SIxMultiCompDes_t *)pstDes);
}

static SIxError_e silib_desc_copyMultiCompDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyMultiCompDes((SIxMultiCompDes_t *)pstDest, (const SIxMultiCompDes_t *)pstSource);
}

static void silib_desc_printMultiCompDes(const SIxDescriptor_t *pstDes)
{
	SIxMultiCompDes_t		*pstDesc;

	pstDesc = (SIxMultiCompDes_t *)pstDes;

}


/* eSIxDESCTAG_PRIVATE_DATA_SPECIFIER				: (0x5F)	*/
//PrivateDataSpecDes
static SIxDescriptor_t *silib_desc_allocPrivateDataSpecDes(void)
{
	SIxPrivateDataSpecDes_t	*pstDes;

	pstDes = allocPrivateDataSpecDes();
	if (pstDes)
	{
		initPrivateDataSpecDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freePrivateDataSpecDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freePrivateDataSpecDes((SIxPrivateDataSpecDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parsePrivateDataSpecDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parsePrivateDataSpecDes((HUINT8 *)pucRawData, pstParam, (SIxPrivateDataSpecDes_t *)pstDes);
}

static SIxError_e silib_desc_copyPrivateDataSpecDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPrivateDataSpecDes((SIxPrivateDataSpecDes_t *)pstDest, (const SIxPrivateDataSpecDes_t *)pstSource);
}

static void silib_desc_printPrivateDataSpecDes(const SIxDescriptor_t *pstDes)
{
	SIxPrivateDataSpecDes_t		*pstDesc;

	pstDesc = (SIxPrivateDataSpecDes_t *)pstDes;

}


/* eSIxDESCTAG_SERVICE_MOVE						: (0x60)	*/
//SvcMoveDes
static SIxDescriptor_t *silib_desc_allocSvcMoveDes(void)
{
	SIxSvcMoveDes_t	*pstDes;

	pstDes = allocSvcMoveDes();
	if (pstDes)
	{
		initSvcMoveDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSvcMoveDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeSvcMoveDes((SIxSvcMoveDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseSvcMoveDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSvcMoveDes((HUINT8 *)pucRawData, pstParam, (SIxSvcMoveDes_t *)pstDes);
}

static SIxError_e silib_desc_copySvcMoveDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySvcMoveDes((SIxSvcMoveDes_t *)pstDest, (const SIxSvcMoveDes_t *)pstSource);
}

static void silib_desc_printSvcMoveDes(const SIxDescriptor_t *pstDes)
{
	SIxSvcMoveDes_t		*pstDesc;

	pstDesc = (SIxSvcMoveDes_t *)pstDes;

}


/* eSIxDESCTAG_SHORT_SMOOTHING_BUFFER				: (0x61)	*/
//ShortSmoothBufDes
static SIxDescriptor_t *silib_desc_allocShortSmoothBufDes(void)
{
	SIxShortSmoothBufDes_t	*pstDes;

	pstDes = allocShortSmoothBufDes();
	if (pstDes)
	{
		initShortSmoothBufDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeShortSmoothBufDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeShortSmoothBufDes((SIxShortSmoothBufDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseShortSmoothBufDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseShortSmoothBufDes((HUINT8 *)pucRawData, pstParam, (SIxShortSmoothBufDes_t *)pstDes);
}

static SIxError_e silib_desc_copyShortSmoothBufDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyShortSmoothBufDes((SIxShortSmoothBufDes_t *)pstDest, (const SIxShortSmoothBufDes_t *)pstSource);
}

static void silib_desc_printShortSmoothBufDes(const SIxDescriptor_t *pstDes)
{
	SIxShortSmoothBufDes_t		*pstDesc;

	pstDesc = (SIxShortSmoothBufDes_t *)pstDes;

}


/* eSIxDESCTAG_FREQUENCY_LIST						: (0x62)	*/
//FrequencyListDes
static SIxDescriptor_t *silib_desc_allocFrequencyListDes(void)
{
	SIxFrequencyListDes_t	*pstDes;

	pstDes = allocFrequencyListDes();
	if (pstDes)
	{
		initFrequencyListDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeFrequencyListDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeFrequencyListDes((SIxFrequencyListDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseFrequencyListDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseFrequencyListDes((HUINT8 *)pucRawData, pstParam, (SIxFrequencyListDes_t *)pstDes);
}

static SIxError_e silib_desc_copyFrequencyListDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyFrequencyListDes((SIxFrequencyListDes_t *)pstDest, (const SIxFrequencyListDes_t *)pstSource);
}

static void silib_desc_printFrequencyListDes(const SIxDescriptor_t *pstDes)
{
	SIxFrequencyListDes_t		*pstDesc;

	pstDesc = (SIxFrequencyListDes_t *)pstDes;

}


/* eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM			: (0x63)	*/
//PartialTsDes
static SIxDescriptor_t *silib_desc_allocPartialTsDes(void)
{
	SIxPartialTsDes_t	*pstDes;

	pstDes = allocPartialTsDes();
	if (pstDes)
	{
		initPartialTsDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freePartialTsDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freePartialTsDes((SIxPartialTsDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parsePartialTsDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parsePartialTsDes((HUINT8 *)pucRawData, pstParam, (SIxPartialTsDes_t *)pstDes);
}

static SIxError_e silib_desc_copyPartialTsDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPartialTsDes((SIxPartialTsDes_t *)pstDest, (const SIxPartialTsDes_t *)pstSource);
}

static void silib_desc_printPartialTsDes(const SIxDescriptor_t *pstDes)
{
	SIxPartialTsDes_t		*pstDesc;

	pstDesc = (SIxPartialTsDes_t *)pstDes;

}


/* eSIxDESCTAG_DATA_BROADCAST						: (0x64)	*/
//DataBroadcastDes
static SIxDescriptor_t *silib_desc_allocDataBroadcastDes(void)
{
	SIxDataBroadcastDes_t	*pstDes;

	pstDes = allocDataBroadcastDes();
	if (pstDes)
	{
		initDataBroadcastDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeDataBroadcastDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeDataBroadcastDes((SIxDataBroadcastDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseDataBroadcastDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDataBroadcastDes((HUINT8 *)pucRawData, pstParam, (SIxDataBroadcastDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDataBroadcastDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDataBroadcastDes((SIxDataBroadcastDes_t *)pstDest, (const SIxDataBroadcastDes_t *)pstSource);
}

static void silib_desc_printDataBroadcastDes(const SIxDescriptor_t *pstDes)
{
	SIxDataBroadcastDes_t		*pstDesc;

	pstDesc = (SIxDataBroadcastDes_t *)pstDes;

}


/* eSIxDESCTAG_DATA_BROADCAST_ID					: (0x66)	*/
//DataBroadcastIdDes
static SIxDescriptor_t *silib_desc_allocDataBroadcastIdDes(void)
{
	SIxDataBroadcastIdDes_t	*pstDes;

	pstDes = allocDataBroadcastIdDes();
	if (pstDes)
	{
		initDataBroadcastIdDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeDataBroadcastIdDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeDataBroadcastIdDes((SIxDataBroadcastIdDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseDataBroadcastIdDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDataBroadcastIdDes((HUINT8 *)pucRawData, pstParam, (SIxDataBroadcastIdDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDataBroadcastIdDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDataBroadcastIdDes((SIxDataBroadcastIdDes_t *)pstDest, (const SIxDataBroadcastIdDes_t *)pstSource);
}

static void silib_desc_printDataBroadcastIdDes(const SIxDescriptor_t *pstDes)
{
	SIxDataBroadcastIdDes_t		*pstDesc;

	pstDesc = (SIxDataBroadcastIdDes_t *)pstDes;

}


/* eSIxDESCTAG_PDC									: (0x69)	*/
//PDCDes
static SIxDescriptor_t *silib_desc_allocPDCDes(void)
{
	SIxPDCDes_t		*pstDes;

	pstDes = allocPDCDes();
	if (pstDes)
	{
		initPDCDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freePDCDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freePDCDes((SIxPDCDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parsePDCDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parsePDCDes((HUINT8 *)pucRawData, pstParam, (SIxPDCDes_t *)pstDes);
}

static SIxError_e silib_desc_copyPDCDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyPDCDes((SIxPDCDes_t *)pstDest, (const SIxPDCDes_t *)pstSource);
}

static void silib_desc_printPDCDes(const SIxDescriptor_t *pstDes)
{
	SIxPDCDes_t		*pstDesc;

	pstDesc = (SIxPDCDes_t *)pstDes;

}


/* eSIxDESCTAG_DOLBY_AC3							: (0x6A)	*/
//DolbyAC3Des
static SIxDescriptor_t *silib_desc_allocDolbyAC3Des(void)
{
	SIxDolbyAC3Des_t	*pstDes;

	pstDes = allocDolbyAC3Des();
	if (pstDes)
	{
		initDolbyAC3Des(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeDolbyAC3Des(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeDolbyAC3Des((SIxDolbyAC3Des_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseDolbyAC3Des(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDolbyAC3Des((HUINT8 *)pucRawData, pstParam, (SIxDolbyAC3Des_t *)pstDes);
}

static SIxError_e silib_desc_copyDolbyAC3Des(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDolbyAC3Des((SIxDolbyAC3Des_t *)pstDest, (const SIxDolbyAC3Des_t *)pstSource);
}

static void silib_desc_printDolbyAC3Des(const SIxDescriptor_t *pstDes)
{
	SIxDolbyAC3Des_t		*pstDesc;

	pstDesc = (SIxDolbyAC3Des_t *)pstDes;

}


/* eSIxDESCTAG_ANCILLARY_DATA						: (0x6B)	*/
//AncillaryData
#if 0
static SIxDescriptor_t *silib_desc_allocAncillaryDataDes(void)
{
	SIxAncillaryDataDes_t	*pstDes;

	pstDes = allocAncillaryDataDes();
	if (pstDes)
	{
		initAncillaryDataDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeAncillaryDataDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeAncillaryDataDes((SIxAncillaryDataDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseAncillaryDataDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseAncillaryDataDes(pucRawData, (SIxAncillaryDataDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAncillaryDataDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAncillaryDataDes((SIxAncillaryDataDes_t *)pstDest, (const SIxAncillaryDataDes_t *)pstSource);
}

static void silib_desc_printAncillaryDataDes(const SIxDescriptor_t *pstDes)
{
	SIxAncillaryDataDes_t		*pstDesc;

	pstDesc = (SIxAncillaryDataDes_t *)pstDes;

}
#endif

/* eSIxDESCTAG_CELL_LIST							: (0x6C)	*/
//CellListDes
static SIxDescriptor_t *silib_desc_allocCellListDes(void)
{
	SIxCellListDes_t	*pstDes;

	pstDes = allocCellListDes();
	if (pstDes)
	{
		initCellListDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCellListDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeCellListDes((SIxCellListDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseCellListDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCellListDes((HUINT8 *)pucRawData, pstParam, (SIxCellListDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCellListDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCellListDes((SIxCellListDes_t *)pstDest, (const SIxCellListDes_t *)pstSource);
}

static void silib_desc_printCellListDes(const SIxDescriptor_t *pstDes)
{
	SIxCellListDes_t		*pstDesc;

	pstDesc = (SIxCellListDes_t *)pstDes;

}


/* eSIxDESCTAG_CELL_FREQ_LIST						: (0x6D)	*/
//CellFreqListDes
#if 0
static SIxDescriptor_t *silib_desc_allocCellFreqListDes(void)
{
	SIxCellFreqListDes_t	*pstDes;

	pstDes = allocCellFreqListDes(sizeof(SIxCellFreqListDes_t));
	if (pstDes)
	{
		initCellFreqListDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeCellFreqListDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeCellFreqListDes((SIxCellFreqListDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseCellFreqListDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseCellFreqListDes((HUINT8 *)pucRawData, pstParam, (SIxCellFreqListDes_t *)pstDes);
}

static SIxError_e silib_desc_copyCellFreqListDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyCellFreqListDes((SIxCellFreqListDes_t *)pstDest, (const SIxCellFreqListDes_t *)pstSource);
}

static void silib_desc_printCellFreqListDes(const SIxDescriptor_t *pstDes)
{
	SIxCellFreqListDes_t		*pstDesc;

	pstDesc = (SIxCellFreqListDes_t *)pstDes;

}
#endif

/* eSIxDESCTAG_ANNOUNCEMENT_SUPPORT				: (0x6E)	*/
//AnnouncementSupportDes
#if 0
static SIxDescriptor_t *silib_desc_allocAnnouncementSupportDes(void)
{
	SIxAnnouncementSupportDes_t	*pstDes;

	pstDes = allocAnnouncementSupportDes(sizeof(SIxAnnouncementSupportDes_t));
	if (pstDes)
	{
		initAnnouncementSupportDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeAnnouncementSupportDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeAnnouncementSupportDes((SIxAnnouncementSupportDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseAnnouncementSupportDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseAnnouncementSupportDes((HUINT8 *)pucRawData, pstParam, (SIxAnnouncementSupportDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAnnouncementSupportDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAnnouncementSupportDes((SIxAnnouncementSupportDes_t *)pstDest, (const SIxAnnouncementSupportDes_t *)pstSource);
}

static void silib_desc_printAnnouncementSupportDes(const SIxDescriptor_t *pstDes)
{
	SIxAnnouncementSupportDes_t		*pstDesc;

	pstDesc = (SIxAnnouncementSupportDes_t *)pstDes;

}
#endif


/* eSIxDESCTAG_APPLICATION_SIGNALLING				: (0x6F)	*/
//AppSignallingDes
static SIxDescriptor_t *silib_desc_allocAppSignallingDes(void)
{
	SIxAppSignallingDes_t	*pstDes;

	pstDes = allocAppSignallingDes();
	if (pstDes)
	{
		initAppSignallingDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeAppSignallingDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeAppSignallingDes((SIxAppSignallingDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseAppSignallingDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseAppSignallingDes((HUINT8 *)pucRawData, pstParam, (SIxAppSignallingDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAppSignallingDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAppSignallingDes((SIxAppSignallingDes_t *)pstDest, (const SIxAppSignallingDes_t *)pstSource);
}

static void silib_desc_printAppSignallingDes(const SIxDescriptor_t *pstDes)
{
	SIxAppSignallingDes_t		*pstDesc;

	pstDesc = (SIxAppSignallingDes_t *)pstDes;

}


/* eSIxDESCTAG_ADAPTATION_FIELD_DATA				: (0x70)	*/
//AdaptationFieldDataDes
#if 0
static SIxDescriptor_t *silib_desc_allocAdaptationFieldDataDes(void)
{
	SIxAdaptationFieldDataDes_t	*pstDes;

	pstDes = allocAdaptationFieldDataDes();
	if (pstDes)
	{
		initAdaptationFieldDataDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeAdaptationFieldDataDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeAdaptationFieldDataDes((SIxAdaptationFieldDataDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseAdaptationFieldDataDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseAdaptationFieldDataDes((HUINT8 *)pucRawData, pstParam, (SIxAdaptationFieldDataDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAdaptationFieldDataDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAdaptationFieldDataDes((SIxAdaptationFieldDataDes_t *)pstDest, (const SIxAdaptationFieldDataDes_t *)pstSource);
}

static void silib_desc_printAdaptationFieldDataDes(const SIxDescriptor_t *pstDes)
{
	SIxAdaptationFieldDataDes_t		*pstDesc;

	pstDesc = (SIxAdaptationFieldDataDes_t *)pstDes;

}
#endif

/* eSIxDESCTAG_SVC_IDENTIFIER						: (0x71)	*/
//SvcIdentifierDes
#if 0
static SIxDescriptor_t *silib_desc_allocSvcIdentifierDes(void)
{
	SIxSvcIdentifierDes_t	*pstDes;

	pstDes = allocSvcIdentifierDes(sizeof(SIxSvcIdentifierDes_t));
	if (pstDes)
	{
		initSvcIdentifierDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSvcIdentifierDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeSvcIdentifierDes((SIxSvcIdentifierDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseSvcIdentifierDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSvcIdentifierDes((HUINT8 *)pucRawData, pstParam, (SIxSvcIdentifierDes_t *)pstDes);
}

static SIxError_e silib_desc_copySvcIdentifierDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySvcIdentifierDes((SIxSvcIdentifierDes_t *)pstDest, (const SIxSvcIdentifierDes_t *)pstSource);
}

static void silib_desc_printSvcIdentifierDes(const SIxDescriptor_t *pstDes)
{
	SIxSvcIdentifierDes_t		*pstDesc;

	pstDesc = (SIxSvcIdentifierDes_t *)pstDes;

}
#endif

/* eSIxDESCTAG_SVC_AVAILABILITY					: (0x72)	*/
//SvcAvailabilityDes
#if 0
static SIxDescriptor_t *silib_desc_allocSvcAvailabilityDes(void)
{
	SIxSvcAvailabilityDes_t	*pstDes;

	pstDes = allocSvcAvailabilityDes(sizeof(SIxSvcAvailabilityDes_t));
	if (pstDes)
	{
		initSvcAvailabilityDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeSvcAvailabilityDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeSvcAvailabilityDes((SIxSvcAvailabilityDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseSvcAvailabilityDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseSvcAvailabilityDes((HUINT8 *)pucRawData, pstParam, (SIxSvcAvailabilityDes_t *)pstDes);
}

static SIxError_e silib_desc_copySvcAvailabilityDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copySvcAvailabilityDes((SIxSvcAvailabilityDes_t *)pstDest, (const SIxSvcAvailabilityDes_t *)pstSource);
}

static void silib_desc_printSvcAvailabilityDes(const SIxDescriptor_t *pstDes)
{
	SIxSvcAvailabilityDes_t		*pstDesc;

	pstDesc = (SIxSvcAvailabilityDes_t *)pstDes;

}
#endif


/* eSIxDESCTAG_DEFAULT_AUTORITY					: (0x73)	*/
//DefaultAuthorityDes
static SIxDescriptor_t *silib_desc_allocDefaultAuthorityDes(void)
{
	SIxDefaultAuthorityDes_t	*pstDes;

	pstDes = allocDefaultAuthorityDes();
	if (pstDes)
	{
		initDefaultAuthorityDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeDefaultAuthorityDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeDefaultAuthorityDes((SIxDefaultAuthorityDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseDefaultAuthorityDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDefaultAuthorityDes((HUINT8 *)pucRawData, pstParam, (SIxDefaultAuthorityDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDefaultAuthorityDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDefaultAuthorityDes((SIxDefaultAuthorityDes_t *)pstDest, (const SIxDefaultAuthorityDes_t *)pstSource);
}

static void silib_desc_printDefaultAuthorityDes(const SIxDescriptor_t *pstDes)
{
	SIxDefaultAuthorityDes_t		*pstDesc;

	pstDesc = (SIxDefaultAuthorityDes_t *)pstDes;

}


/* eSIxDESCTAG_RELATED_CONTENT						: (0x74)	*/
//RelatedContentDes
static SIxDescriptor_t *silib_desc_allocRelatedContentDes(void)
{
	SIxRelatedContentDes_t	*pstDes;

	pstDes = allocRelatedContentDes();
	if (pstDes)
	{
		initRelatedContentDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeRelatedContentDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeRelatedContentDes((SIxRelatedContentDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseRelatedContentDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseRelatedContentDes((HUINT8 *)pucRawData, pstParam, (SIxRelatedContentDes_t *)pstDes);
}

static SIxError_e silib_desc_copyRelatedContentDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyRelatedContentDes((SIxRelatedContentDes_t *)pstDest, (const SIxRelatedContentDes_t *)pstSource);
}

static void silib_desc_printRelatedContentDes(const SIxDescriptor_t *pstDes)
{
	SIxRelatedContentDes_t		*pstDesc;

	pstDesc = (SIxRelatedContentDes_t *)pstDes;

}


/* eSIxDESCTAG_CONTENT_IDENTIFIER					: (0x76)	*/
//ContentIdentifierDes
static SIxDescriptor_t *silib_desc_allocContentIdentifierDes(void)
{
	SIxContentIdentifierDes_t	*pstDes;

	pstDes = allocContentIdentifierDes();
	if (pstDes)
	{
		initContentIdentifierDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeContentIdentifierDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeContentIdentifierDes((SIxContentIdentifierDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseContentIdentifierDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseContentIdentifierDes((HUINT8 *)pucRawData, pstParam, (SIxContentIdentifierDes_t *)pstDes);
}

static SIxError_e silib_desc_copyContentIdentifierDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyContentIdentifierDes((SIxContentIdentifierDes_t *)pstDest, (const SIxContentIdentifierDes_t *)pstSource);
}

static void silib_desc_printContentIdentifierDes(const SIxDescriptor_t *pstDes)
{
	SIxContentIdentifierDes_t		*pstDesc;

	pstDesc = (SIxContentIdentifierDes_t *)pstDes;

}


/* eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM		: (0x79)	*/
//S2SatDeliSystemDes
static SIxDescriptor_t *silib_desc_allocS2SatDeliSystemDes(void)
{
	SIxS2SatDeliSystemDes_t	*pstDes;

	pstDes = allocS2SatDeliSystemDes();
	if (pstDes)
	{
		initS2SatDeliSystemDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeS2SatDeliSystemDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeS2SatDeliSystemDes((SIxS2SatDeliSystemDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseS2SatDeliSystemDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseS2SatDeliSystemDes((HUINT8 *)pucRawData, pstParam, (SIxS2SatDeliSystemDes_t *)pstDes);
}

static SIxError_e silib_desc_copyS2SatDeliSystemDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyS2SatDeliSystemDes((SIxS2SatDeliSystemDes_t *)pstDest, (const SIxS2SatDeliSystemDes_t *)pstSource);
}

static void silib_desc_printS2SatDeliSystemDes(const SIxDescriptor_t *pstDes)
{
	SIxS2SatDeliSystemDes_t		*pstDesc;

	pstDesc = (SIxS2SatDeliSystemDes_t *)pstDes;

}


/* eSIxDESCTAG_ENHANCED_AC3						: (0x7A)	*/
//EnhancedAC3Des
static SIxDescriptor_t *silib_desc_allocEnhancedAC3Des(void)
{
	SIxEnhancedAC3Des_t	*pstDes;

	pstDes = allocEnhancedAC3Des();
	if (pstDes)
	{
		initEnhancedAC3Des(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeEnhancedAC3Des(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeEnhancedAC3Des((SIxEnhancedAC3Des_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseEnhancedAC3Des(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseEnhancedAC3Des((HUINT8 *)pucRawData, pstParam, (SIxEnhancedAC3Des_t *)pstDes);
}

static SIxError_e silib_desc_copyEnhancedAC3Des(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyEnhancedAC3Des((SIxEnhancedAC3Des_t *)pstDest, (const SIxEnhancedAC3Des_t *)pstSource);
}

static void silib_desc_printEnhancedAC3Des(const SIxDescriptor_t *pstDes)
{
	SIxEnhancedAC3Des_t		*pstDesc;

	pstDesc = (SIxEnhancedAC3Des_t *)pstDes;

}


/* eSIxDESCTAG_DTS									: (0x7B)	*/
//DTSDes
#if 0
static SIxDescriptor_t *silib_desc_allocDTSDes(void)
{
	SIxDTSDes_t		*pstDes;

	pstDes = allocDTSDes();
	if (pstDes)
	{
		initDTSDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeDTSDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeDTSDes((SIxDTSDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseDTSDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseDTSDes((HUINT8 *)pucRawData, pstParam, (SIxDTSDes_t *)pstDes);
}

static SIxError_e silib_desc_copyDTSDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyDTSDes((SIxDTSDes_t *)pstDest, (const SIxDTSDes_t *)pstSource);
}

static void silib_desc_printDTSDes(const SIxDescriptor_t *pstDes)
{
	SIxDTSDes_t		*pstDesc;

	pstDesc = (SIxDTSDes_t *)pstDes;

}
#endif

/* eSIxDESCTAG_AAC									: (0x7C)	*/
//AACDes
static SIxDescriptor_t *silib_desc_allocAACDes(void)
{
	SIxAACDes_t		*pstDes;

	pstDes = allocAACDes();
	if (pstDes)
	{
		initAACDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeAACDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeAACDes((SIxAACDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseAACDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseAACDes((HUINT8 *)pucRawData, pstParam, (SIxAACDes_t *)pstDes);
}

static SIxError_e silib_desc_copyAACDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyAACDes((SIxAACDes_t *)pstDest, (const SIxAACDes_t *)pstSource);
}

static void silib_desc_printAACDes(const SIxDescriptor_t *pstDes)
{
	SIxAACDes_t		*pstDesc;

	pstDesc = (SIxAACDes_t *)pstDes;

}


/* eSIxDESCTAG_XAIT_LOCATION						: (0x7D)	*/
//XaitLocattionDes
#if 0
static SIxDescriptor_t *silib_desc_allocXaitLocationDes(void)
{
	SIxXaitLocationDes_t	*pstDes;

	pstDes = allocXaitLocationDes();
	if (pstDes)
	{
		initXaitLocationDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeXaitLocationDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeXaitLocationDes((SIxXaitLocationDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseXaitLocationDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseXaitLocationDes((HUINT8 *)pucRawData, pstParam, (SIxXaitLocationDes_t *)pstDes);
}

static SIxError_e silib_desc_copyXaitLocationDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyXaitLocationDes((SIxXaitLocationDes_t *)pstDest, (const SIxXaitLocationDes_t *)pstSource);
}

static void silib_desc_printXaitLocationDes(const SIxDescriptor_t *pstDes)
{
	SIxXaitLocationDes_t		*pstDesc;

	pstDesc = (SIxXaitLocationDes_t *)pstDes;

}
#endif

/* eSIxDESCTAG_FTA_CONTENT_MANAGEMENT				: (0x7E)	*/
//FtaContentManagementDes
static SIxDescriptor_t *silib_desc_allocFtaContentManagementDes(void)
{
	SIxFtaContentManagementDes_t	*pstDes;

	pstDes = allocFtaContentManagementDes();
	if (pstDes)
	{
		initFtaContentManagementDes(pstDes);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeFtaContentManagementDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeFtaContentManagementDes((SIxFtaContentManagementDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseFtaContentManagementDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseFtaContentManagementDes((HUINT8 *)pucRawData, pstParam, (SIxFtaContentManagementDes_t *)pstDes);
}

static SIxError_e silib_desc_copyFtaContentManagementDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyFtaContentManagementDes((SIxFtaContentManagementDes_t *)pstDest, (const SIxFtaContentManagementDes_t *)pstSource);
}

static void silib_desc_printFtaContentManagementDes(const SIxDescriptor_t *pstDes)
{
	SIxFtaContentManagementDes_t		*pstDesc;

	pstDesc = (SIxFtaContentManagementDes_t *)pstDes;

}


/* eSIxDESCTAG_EXTENSION							: (0x7F)	*/
//ExtensionDes
static SIxDescriptor_t *silib_desc_allocExtensionDes(HUINT32 ulRawLen)
{
	SIxExtensionDes_t	*pstDes;

	pstDes = allocExtensionDes(ulRawLen);
	if (pstDes)
	{
		initExtensionDes(pstDes, ulRawLen);
		return (SIxDescriptor_t *)pstDes;
	}

	return NULL;
}

static SIxError_e silib_desc_freeExtensionDes(SIxDescriptor_t *pstDes)
{
	SIxError_e	 eErr = eSIxERR_FAIL;

	if (pstDes)
	{
		freeExtensionDes((SIxExtensionDes_t *)pstDes);
		eErr = eSIxERR_OK;
	}

	return eErr;
}

static SIxError_e silib_desc_parseExtensionDes(const HUINT8 *pucRawData, const SIxDescriptorHeaderParam_t *pstParam, SIxDescriptor_t *pstDes)
{
	return parseExtensionDes((HUINT8 *)pucRawData, pstParam, (SIxExtensionDes_t *)pstDes);
}

static SIxError_e silib_desc_copyExtensionDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyExtensionDes((SIxExtensionDes_t *)pstDest, (const SIxExtensionDes_t *)pstSource);
}

static void silib_desc_printExtensionDes(const SIxDescriptor_t *pstDes)
{
	SIxExtensionDes_t		*pstDesc;

	pstDesc = (SIxExtensionDes_t *)pstDes;

	if (pstDesc->pucRawData)
	{
		HxLOG_Print("\t(Extension Descriptor Tag : %d\n)\n", pstDesc->pucRawData[0]);
		HxLOG_Print("\t(Extension Descriptor Length : %d\n)\n", pstDesc->pucRawData[1]);
		if (pstDesc->pucRawData[1] > 0)
		{
			HLIB_LOG_Dump(pstDesc->pucRawData+2, pstDesc->pucRawData[1], 0, FALSE);
		}
	}
}

#define	___MODULE_APIs___

SIxDescriptor_t *silib_desc_alloc_Dvb(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen)
{
	switch (eTag)
	{
 	case eSIxDESCTAG_VIDEO_STREAM:							return silib_desc_allocVideoStmDes();
 	case eSIxDESCTAG_AUDIO_STREAM:							return silib_desc_allocAudioStmDes();
 	case eSIxDESCTAG_HIERARCHY:								return silib_desc_allocHierDes();
 	case eSIxDESCTAG_REGISTRATION:							return silib_desc_allocRegDes();
 	case eSIxDESCTAG_DATA_STREAM_ALIGNMENT:					return silib_desc_allocDataStmAlignDes();
 	case eSIxDESCTAG_TARGET_BACKGROUND_GRID:				return silib_desc_allocBackGridDes();
 	case eSIxDESCTAG_VIDEO_WINDOW:							return silib_desc_allocVideoWindowDes();
 	case eSIxDESCTAG_CA:									return silib_desc_allocCaDes();
 	case eSIxDESCTAG_ISO_639_LANGUAGE:						return silib_desc_allocIso639LangDes();
 	case eSIxDESCTAG_SYSTEM_CLOCK:							return silib_desc_allocSysClockDes();
 	case eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION:			return silib_desc_allocMultiplexBufUtilDes();
 	case eSIxDESCTAG_COPYRIGHT:								return silib_desc_allocCopyRightDes();
 	case eSIxDESCTAG_MAXIMUM_BITRATE:						return silib_desc_allocMaxBitrateDes();
 	case eSIxDESCTAG_PRIVATE_DATA_INDICATOR:				return silib_desc_allocPrivateDataIndicatorDes();
 	case eSIxDESCTAG_SMOOTHING_BUFFER:						return silib_desc_allocSmoothBufDes();
 	case eSIxDESCTAG_STD:									return silib_desc_allocStdDes();
 	case eSIxDESCTAG_CAROUSEL_ID:							return silib_desc_allocCarouselIdDes();
 	case eSIxDESCTAG_ASSOCIATION_TAG:						return silib_desc_allocAssocTagDes();
 	case eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG:				return silib_desc_allocDeferAssocTagDes();
	case eSIxDESCTAG_AVC_VIDEO:								return silib_desc_allocAvcVideoDes();
	/** < ETS 300 468에 정의된 descriptor tag */
 	case eSIxDESCTAG_NETWORK_NAME:							return silib_desc_allocNetNameDes();
 	case eSIxDESCTAG_SERVICE_LIST:							return silib_desc_allocSvcListDes();
 	case eSIxDESCTAG_STUFFING:									break;	//return silib_desc_allocStuffingDes();
 	case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:				return silib_desc_allocSatDeliSystemDes();
 	case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:					return silib_desc_allocCabDeliSystemDes();
 	case eSIxDESCTAG_BOUQUET_NAME:							return silib_desc_allocBouqNameDes();
 	case eSIxDESCTAG_SERVICE:								return silib_desc_allocSvcDes();
 	case eSIxDESCTAG_COUNTRY_AVAILABILITY:					return silib_desc_allocCountryAvailDes();
 	case eSIxDESCTAG_LINKAGE:								return silib_desc_allocLinkDes();
 	case eSIxDESCTAG_NVOD_REFERENCE:						return silib_desc_allocNvodRefDes();
 	case eSIxDESCTAG_TIME_SHIFTED_SERVICE:					return silib_desc_allocTimeShiftSvcDes();
 	case eSIxDESCTAG_SHORT_EVENT:							return silib_desc_allocShortEvtDes();
 	case eSIxDESCTAG_EXTENDED_EVENT:						return silib_desc_allocExtEvtDes();
 	case eSIxDESCTAG_TIME_SHIFTED_EVENT:					return silib_desc_allocTimeShiftEvtDes();
 	case eSIxDESCTAG_COMPONENT:								return silib_desc_allocCompDes();
 	case eSIxDESCTAG_MOSAIC:									break;	//return silib_desc_allocMosaicDes();
 	case eSIxDESCTAG_STREAM_IDENTIFIER:						return silib_desc_allocStmIdentDes();
 	case eSIxDESCTAG_CA_IDENTIFIER:							return silib_desc_allocCaIdentDes();
 	case eSIxDESCTAG_CONTENT:								return silib_desc_allocContentDes();
 	case eSIxDESCTAG_PARENTAL_RATING:						return silib_desc_allocParentDes();
 	case eSIxDESCTAG_TELETEXT:								return silib_desc_allocTeletextDes();
 	case eSIxDESCTAG_TELEPHONE:								return silib_desc_allocTelephoneDes();
 	case eSIxDESCTAG_LOCAL_TIME_OFFSET:						return silib_desc_allocLocalTimeDes();
 	case eSIxDESCTAG_SUBTITLING:							return silib_desc_allocSubtitleDes();
 	case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:			return silib_desc_allocTerDeliSystemDes();
 	case eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME:				return silib_desc_allocMultiNetNameDes();
 	case eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME:				return silib_desc_allocMultiBouqNameDes();
 	case eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME:				return silib_desc_allocMultiSvcNameDes();
 	case eSIxDESCTAG_MULTILINGUAL_COMPONENT:				return silib_desc_allocMultiCompDes();
 	case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:				return silib_desc_allocPrivateDataSpecDes();
 	case eSIxDESCTAG_SERVICE_MOVE:							return silib_desc_allocSvcMoveDes();
 	case eSIxDESCTAG_SHORT_SMOOTHING_BUFFER:				return silib_desc_allocShortSmoothBufDes();
	case eSIxDESCTAG_FREQUENCY_LIST:						return silib_desc_allocFrequencyListDes();
	case eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM:				return silib_desc_allocPartialTsDes();
	case eSIxDESCTAG_DATA_BROADCAST:						return silib_desc_allocDataBroadcastDes();
	case eSIxDESCTAG_DATA_BROADCAST_ID:						return silib_desc_allocDataBroadcastIdDes();
	case eSIxDESCTAG_PDC:									return silib_desc_allocPDCDes();
	case eSIxDESCTAG_DOLBY_AC3:								return silib_desc_allocDolbyAC3Des();
	case eSIxDESCTAG_ANCILLARY_DATA:							break;	//return silib_desc_allocAncillaryDataDes();
	case eSIxDESCTAG_CELL_LIST:								return silib_desc_allocCellListDes();
	case eSIxDESCTAG_CELL_FREQ_LIST:							break;	//return silib_desc_allocCellFreqListDes();
	case eSIxDESCTAG_ANNOUNCEMENT_SUPPORT:						break;	//return silib_desc_allocAnnouncementSupportDes();
	case eSIxDESCTAG_APPLICATION_SIGNALLING:				return silib_desc_allocAppSignallingDes();
	case eSIxDESCTAG_ADAPTATION_FIELD_DATA:						break;	//return silib_desc_allocAdaptationFieldDataDes();
	case eSIxDESCTAG_SVC_IDENTIFIER:							break;	//silib_desc_allocSvcIdentifierDes();
	case eSIxDESCTAG_SVC_AVAILABILITY:							break;	//return silib_desc_allocSvcAvailabilityDes();
	case eSIxDESCTAG_DEFAULT_AUTHORITY:						return silib_desc_allocDefaultAuthorityDes();
	case eSIxDESCTAG_RELATED_CONTENT:						return silib_desc_allocRelatedContentDes();
	case eSIxDESCTAG_CONTENT_IDENTIFIER:					return silib_desc_allocContentIdentifierDes();
	case eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM:			return silib_desc_allocS2SatDeliSystemDes();
	case eSIxDESCTAG_ENHANCED_AC3:							return silib_desc_allocEnhancedAC3Des();
	case eSIxDESCTAG_DTS:									break;	//return silib_desc_allocDTSDes();
	case eSIxDESCTAG_AAC:									return silib_desc_allocAACDes();
	case eSIxDESCTAG_XAIT_LOCATION:								break;	//return silib_desc_allocXaitLocationDes();
	case eSIxDESCTAG_FTA_CONTENT_MANAGEMENT:				return silib_desc_allocFtaContentManagementDes();
	case eSIxDESCTAG_EXTENSION:								return silib_desc_allocExtensionDes(ulRawLen);

	default:													break;
	}

	return NULL;
}

SIxError_e silib_desc_free_Dvb(SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstDes->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_VIDEO_STREAM: 						return silib_desc_freeVideoStmDes(pstDes);
	case eSIxDESCTAG_AUDIO_STREAM: 						return silib_desc_freeAudioStmDes(pstDes);
	case eSIxDESCTAG_HIERARCHY:							return silib_desc_freeHierDes(pstDes);
	case eSIxDESCTAG_REGISTRATION: 						return silib_desc_freeRegDes(pstDes);
	case eSIxDESCTAG_DATA_STREAM_ALIGNMENT:				return silib_desc_freeDataStmAlignDes(pstDes);
	case eSIxDESCTAG_TARGET_BACKGROUND_GRID:			return silib_desc_freeBackGridDes(pstDes);
	case eSIxDESCTAG_VIDEO_WINDOW: 						return silib_desc_freeVideoWindowDes(pstDes);
	case eSIxDESCTAG_CA:								return silib_desc_freeCaDes(pstDes);
	case eSIxDESCTAG_ISO_639_LANGUAGE: 					return silib_desc_freeIso639LangDes(pstDes);
	case eSIxDESCTAG_SYSTEM_CLOCK: 						return silib_desc_freeSysClockDes(pstDes);
	case eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION: 		return silib_desc_freeMultiplexBufUtilDes(pstDes);
	case eSIxDESCTAG_COPYRIGHT:							return silib_desc_freeCopyRightDes(pstDes);
	case eSIxDESCTAG_MAXIMUM_BITRATE:					return silib_desc_freeMaxBitrateDes(pstDes);
	case eSIxDESCTAG_PRIVATE_DATA_INDICATOR:			return silib_desc_freePrivateDataIndicatorDes(pstDes);
	case eSIxDESCTAG_SMOOTHING_BUFFER: 					return silib_desc_freeSmoothBufDes(pstDes);
	case eSIxDESCTAG_STD:								return silib_desc_freeStdDes(pstDes);
	case eSIxDESCTAG_CAROUSEL_ID:						return silib_desc_freeCarouselIdDes(pstDes);
	case eSIxDESCTAG_ASSOCIATION_TAG:					return silib_desc_freeAssocTagDes(pstDes);
	case eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG: 			return silib_desc_freeDeferAssocTagDes(pstDes);
	case eSIxDESCTAG_AVC_VIDEO:							return silib_desc_freeAvcVideoDes(pstDes);
	/** < ETS 300 468에 정의된 descriptor tag */
	case eSIxDESCTAG_NETWORK_NAME: 						return silib_desc_freeNetNameDes(pstDes);
	case eSIxDESCTAG_SERVICE_LIST: 						return silib_desc_freeSvcListDes(pstDes);
	case eSIxDESCTAG_STUFFING: 								break;	//return silib_desc_freeStuffingDes(pstDes);
	case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:			return silib_desc_freeSatDeliSystemDes(pstDes);
	case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:				return silib_desc_freeCabDeliSystemDes(pstDes);
	case eSIxDESCTAG_BOUQUET_NAME: 						return silib_desc_freeBouqNameDes(pstDes);
	case eSIxDESCTAG_SERVICE:							return silib_desc_freeSvcDes(pstDes);
	case eSIxDESCTAG_COUNTRY_AVAILABILITY: 				return silib_desc_freeCountryAvailDes(pstDes);
	case eSIxDESCTAG_LINKAGE:							return silib_desc_freeLinkDes(pstDes);
	case eSIxDESCTAG_NVOD_REFERENCE:					return silib_desc_freeNvodRefDes(pstDes);
	case eSIxDESCTAG_TIME_SHIFTED_SERVICE: 				return silib_desc_freeTimeShiftSvcDes(pstDes);
	case eSIxDESCTAG_SHORT_EVENT:						return silib_desc_freeShortEvtDes(pstDes);
	case eSIxDESCTAG_EXTENDED_EVENT:					return silib_desc_freeExtEvtDes(pstDes);
	case eSIxDESCTAG_TIME_SHIFTED_EVENT:				return silib_desc_freeTimeShiftEvtDes(pstDes);
	case eSIxDESCTAG_COMPONENT:							return silib_desc_freeCompDes(pstDes);
	case eSIxDESCTAG_MOSAIC:								break;	//return silib_desc_freeMosaicDes(pstDes);
	case eSIxDESCTAG_STREAM_IDENTIFIER:					return silib_desc_freeStmIdentDes(pstDes);
	case eSIxDESCTAG_CA_IDENTIFIER:						return silib_desc_freeCaIdentDes(pstDes);
	case eSIxDESCTAG_CONTENT:							return silib_desc_freeContentDes(pstDes);
	case eSIxDESCTAG_PARENTAL_RATING:					return silib_desc_freeParentDes(pstDes);
	case eSIxDESCTAG_TELETEXT: 							return silib_desc_freeTeletextDes(pstDes);
	case eSIxDESCTAG_TELEPHONE:							return silib_desc_freeTelephoneDes(pstDes);
	case eSIxDESCTAG_LOCAL_TIME_OFFSET:					return silib_desc_freeLocalTimeDes(pstDes);
	case eSIxDESCTAG_SUBTITLING:						return silib_desc_freeSubtitleDes(pstDes);
	case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:		return silib_desc_freeTerDeliSystemDes(pstDes);
	case eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME:			return silib_desc_freeMultiNetNameDes(pstDes);
	case eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME:			return silib_desc_freeMultiBouqNameDes(pstDes);
	case eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME:			return silib_desc_freeMultiSvcNameDes(pstDes);
	case eSIxDESCTAG_MULTILINGUAL_COMPONENT:			return silib_desc_freeMultiCompDes(pstDes);
	case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:			return silib_desc_freePrivateDataSpecDes(pstDes);
	case eSIxDESCTAG_SERVICE_MOVE: 						return silib_desc_freeSvcMoveDes(pstDes);
	case eSIxDESCTAG_SHORT_SMOOTHING_BUFFER:			return silib_desc_freeShortSmoothBufDes(pstDes);
	case eSIxDESCTAG_FREQUENCY_LIST:					return silib_desc_freeFrequencyListDes(pstDes);
	case eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM: 			return silib_desc_freePartialTsDes(pstDes);
	case eSIxDESCTAG_DATA_BROADCAST:					return silib_desc_freeDataBroadcastDes(pstDes);
	case eSIxDESCTAG_DATA_BROADCAST_ID:					return silib_desc_freeDataBroadcastIdDes(pstDes);
	case eSIxDESCTAG_PDC:								return silib_desc_freePDCDes(pstDes);
	case eSIxDESCTAG_DOLBY_AC3:							return silib_desc_freeDolbyAC3Des(pstDes);
	case eSIxDESCTAG_ANCILLARY_DATA:						break;	//return silib_desc_freeAncillaryDataDes(pstDes);
	case eSIxDESCTAG_CELL_LIST:							return silib_desc_freeCellListDes(pstDes);
	case eSIxDESCTAG_CELL_FREQ_LIST:						break;	//return silib_desc_freeCellFreqListDes(pstDes);
	case eSIxDESCTAG_ANNOUNCEMENT_SUPPORT: 					break;	//return silib_desc_freeAnnouncementSupportDes(pstDes);
	case eSIxDESCTAG_APPLICATION_SIGNALLING:			return silib_desc_freeAppSignallingDes(pstDes);
	case eSIxDESCTAG_ADAPTATION_FIELD_DATA:					break;	//return silib_desc_freeAdaptationFieldDataDes(pstDes);
	case eSIxDESCTAG_SVC_IDENTIFIER:						break;	//silib_desc_freeSvcIdentifierDes(pstDes);
	case eSIxDESCTAG_SVC_AVAILABILITY: 						break;	//return silib_desc_freeSvcAvailabilityDes(pstDes);
	case eSIxDESCTAG_DEFAULT_AUTHORITY: 					return silib_desc_freeDefaultAuthorityDes(pstDes);
	case eSIxDESCTAG_RELATED_CONTENT:					return silib_desc_freeRelatedContentDes(pstDes);
	case eSIxDESCTAG_CONTENT_IDENTIFIER:				return silib_desc_freeContentIdentifierDes(pstDes);
	case eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM: 		return silib_desc_freeS2SatDeliSystemDes(pstDes);
	case eSIxDESCTAG_ENHANCED_AC3: 						return silib_desc_freeEnhancedAC3Des(pstDes);
	case eSIxDESCTAG_DTS:									break;	//return silib_desc_freeDTSDes(pstDes);
	case eSIxDESCTAG_AAC:								return silib_desc_freeAACDes(pstDes);
	case eSIxDESCTAG_XAIT_LOCATION:							break;	//return silib_desc_freeXaitLocationDes(pstDes);
	case eSIxDESCTAG_FTA_CONTENT_MANAGEMENT:			return silib_desc_freeFtaContentManagementDes(pstDes);
	case eSIxDESCTAG_EXTENSION:							return silib_desc_freeExtensionDes(pstDes);

	default:												break;
	}

	return eErr;
}


SIxError_e silib_desc_parse_Dvb(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	SIxError_e			 eErr;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_VIDEO_STREAM: 						return silib_desc_parseVideoStmDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_AUDIO_STREAM: 						return silib_desc_parseAudioStmDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_HIERARCHY:							return silib_desc_parseHierDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_REGISTRATION: 						return silib_desc_parseRegDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_DATA_STREAM_ALIGNMENT:				return silib_desc_parseDataStmAlignDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_TARGET_BACKGROUND_GRID:			return silib_desc_parseBackGridDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_VIDEO_WINDOW: 						return silib_desc_parseVideoWindowDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_CA:								return silib_desc_parseCaDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_ISO_639_LANGUAGE: 					return silib_desc_parseIso639LangDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SYSTEM_CLOCK: 						return silib_desc_parseSysClockDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION: 		return silib_desc_parseMultiplexBufUtilDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_COPYRIGHT:							return silib_desc_parseCopyRightDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_MAXIMUM_BITRATE:					return silib_desc_parseMaxBitrateDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_PRIVATE_DATA_INDICATOR:			return silib_desc_parsePrivateDataIndicatorDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SMOOTHING_BUFFER: 					return silib_desc_parseSmoothBufDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_STD:								return silib_desc_parseStdDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_CAROUSEL_ID:						return silib_desc_parseCarouselIdDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_ASSOCIATION_TAG:					return silib_desc_parseAssocTagDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG: 			return silib_desc_parseDeferAssocTagDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_AVC_VIDEO:							return silib_desc_parseAvcVideoDes(pucRawData, pstParam, pstDes);
	/** < ETS 300 468에 정의된 descriptor tag */
	case eSIxDESCTAG_NETWORK_NAME: 						return silib_desc_parseNetNameDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SERVICE_LIST: 						return silib_desc_parseSvcListDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_STUFFING: 								break;	//return silib_desc_parseStuffingDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:			return silib_desc_parseSatDeliSystemDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:				return silib_desc_parseCabDeliSystemDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_BOUQUET_NAME: 						return silib_desc_parseBouqNameDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SERVICE:							return silib_desc_parseSvcDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_COUNTRY_AVAILABILITY: 				return silib_desc_parseCountryAvailDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_LINKAGE:							return silib_desc_parseLinkDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_NVOD_REFERENCE:					return silib_desc_parseNvodRefDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_TIME_SHIFTED_SERVICE: 				return silib_desc_parseTimeShiftSvcDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SHORT_EVENT:						return silib_desc_parseShortEvtDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_EXTENDED_EVENT:					return silib_desc_parseExtEvtDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_TIME_SHIFTED_EVENT:				return silib_desc_parseTimeShiftEvtDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_COMPONENT:							return silib_desc_parseCompDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_MOSAIC:								break;	//return silib_desc_parseMosaicDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_STREAM_IDENTIFIER:					return silib_desc_parseStmIdentDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_CA_IDENTIFIER:						return silib_desc_parseCaIdentDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_CONTENT:							return silib_desc_parseContentDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_PARENTAL_RATING:					return silib_desc_parseParentDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_TELETEXT: 							return silib_desc_parseTeletextDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_TELEPHONE:							return silib_desc_parseTelephoneDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_LOCAL_TIME_OFFSET:					return silib_desc_parseLocalTimeDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SUBTITLING:						return silib_desc_parseSubtitleDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:		return silib_desc_parseTerDeliSystemDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME:			return silib_desc_parseMultiNetNameDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME:			return silib_desc_parseMultiBouqNameDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME:			return silib_desc_parseMultiSvcNameDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_MULTILINGUAL_COMPONENT:			return silib_desc_parseMultiCompDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:			return silib_desc_parsePrivateDataSpecDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SERVICE_MOVE: 						return silib_desc_parseSvcMoveDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SHORT_SMOOTHING_BUFFER:			return silib_desc_parseShortSmoothBufDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_FREQUENCY_LIST:					return silib_desc_parseFrequencyListDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM: 			return silib_desc_parsePartialTsDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_DATA_BROADCAST:					return silib_desc_parseDataBroadcastDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_DATA_BROADCAST_ID:					return silib_desc_parseDataBroadcastIdDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_PDC:								return silib_desc_parsePDCDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_DOLBY_AC3:							return silib_desc_parseDolbyAC3Des(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_ANCILLARY_DATA:						break;	//return silib_desc_parseAncillaryDataDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_CELL_LIST:							return silib_desc_parseCellListDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_CELL_FREQ_LIST:						break;	//return silib_desc_parseCellFreqListDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_ANNOUNCEMENT_SUPPORT: 					break;	//return silib_desc_parseAnnouncementSupportDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_APPLICATION_SIGNALLING:			return silib_desc_parseAppSignallingDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_ADAPTATION_FIELD_DATA:					break;	//return silib_desc_parseAdaptationFieldDataDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SVC_IDENTIFIER:						break;	//silib_desc_parseSvcIdentifierDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_SVC_AVAILABILITY: 						break;	//return silib_desc_parseSvcAvailabilityDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_DEFAULT_AUTHORITY: 					return silib_desc_parseDefaultAuthorityDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_RELATED_CONTENT:					return silib_desc_parseRelatedContentDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_CONTENT_IDENTIFIER:				return silib_desc_parseContentIdentifierDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM: 		return silib_desc_parseS2SatDeliSystemDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_ENHANCED_AC3: 						return silib_desc_parseEnhancedAC3Des(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_DTS:									break;	//return silib_desc_parseDTSDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_AAC:								return silib_desc_parseAACDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_XAIT_LOCATION:							break;	//return silib_desc_parseXaitLocationDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_FTA_CONTENT_MANAGEMENT:			return silib_desc_parseFtaContentManagementDes(pucRawData, pstParam, pstDes);
	case eSIxDESCTAG_EXTENSION:							return silib_desc_parseExtensionDes(pucRawData, pstParam, pstDes);

	default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_copy_Dvb(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	SIxError_e			 eErr;
	SIxDescriptorTag_e	 eTag;

	eTag = pstSource->stHead.eTag;

	eErr = eSIxERR_NOT_SUPPORT;
	switch (eTag)
	{
	case eSIxDESCTAG_VIDEO_STREAM: 						return silib_desc_copyVideoStmDes(pstDest, pstSource);
	case eSIxDESCTAG_AUDIO_STREAM: 						return silib_desc_copyAudioStmDes(pstDest, pstSource);
	case eSIxDESCTAG_HIERARCHY:							return silib_desc_copyHierDes(pstDest, pstSource);
	case eSIxDESCTAG_REGISTRATION: 						return silib_desc_copyRegDes(pstDest, pstSource);
	case eSIxDESCTAG_DATA_STREAM_ALIGNMENT:				return silib_desc_copyDataStmAlignDes(pstDest, pstSource);
	case eSIxDESCTAG_TARGET_BACKGROUND_GRID:			return silib_desc_copyBackGridDes(pstDest, pstSource);
	case eSIxDESCTAG_VIDEO_WINDOW: 						return silib_desc_copyVideoWindowDes(pstDest, pstSource);
	case eSIxDESCTAG_CA:								return silib_desc_copyCaDes(pstDest, pstSource);
	case eSIxDESCTAG_ISO_639_LANGUAGE: 					return silib_desc_copyIso639LangDes(pstDest, pstSource);
	case eSIxDESCTAG_SYSTEM_CLOCK: 						return silib_desc_copySysClockDes(pstDest, pstSource);
	case eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION: 		return silib_desc_copyMultiplexBufUtilDes(pstDest, pstSource);
	case eSIxDESCTAG_COPYRIGHT:							return silib_desc_copyCopyRightDes(pstDest, pstSource);
	case eSIxDESCTAG_MAXIMUM_BITRATE:					return silib_desc_copyMaxBitrateDes(pstDest, pstSource);
	case eSIxDESCTAG_PRIVATE_DATA_INDICATOR:			return silib_desc_copyPrivateDataIndicatorDes(pstDest, pstSource);
	case eSIxDESCTAG_SMOOTHING_BUFFER: 					return silib_desc_copySmoothBufDes(pstDest, pstSource);
	case eSIxDESCTAG_STD:								return silib_desc_copyStdDes(pstDest, pstSource);
	case eSIxDESCTAG_CAROUSEL_ID:						return silib_desc_copyCarouselIdDes(pstDest, pstSource);
	case eSIxDESCTAG_ASSOCIATION_TAG:					return silib_desc_copyAssocTagDes(pstDest, pstSource);
	case eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG: 			return silib_desc_copyDeferAssocTagDes(pstDest, pstSource);
	case eSIxDESCTAG_AVC_VIDEO:							return silib_desc_copyAvcVideoDes(pstDest, pstSource);
	/** < ETS 300 468에 정의된 descriptor tag */
	case eSIxDESCTAG_NETWORK_NAME: 						return silib_desc_copyNetNameDes(pstDest, pstSource);
	case eSIxDESCTAG_SERVICE_LIST: 						return silib_desc_copySvcListDes(pstDest, pstSource);
	case eSIxDESCTAG_STUFFING: 								break;	//return silib_desc_copyStuffingDes(pstDest, pstSource);
	case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:			return silib_desc_copySatDeliSystemDes(pstDest, pstSource);
	case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:				return silib_desc_copyCabDeliSystemDes(pstDest, pstSource);
	case eSIxDESCTAG_BOUQUET_NAME: 						return silib_desc_copyBouqNameDes(pstDest, pstSource);
	case eSIxDESCTAG_SERVICE:							return silib_desc_copySvcDes(pstDest, pstSource);
	case eSIxDESCTAG_COUNTRY_AVAILABILITY: 				return silib_desc_copyCountryAvailDes(pstDest, pstSource);
	case eSIxDESCTAG_LINKAGE:							return silib_desc_copyLinkDes(pstDest, pstSource);
	case eSIxDESCTAG_NVOD_REFERENCE:					return silib_desc_copyNvodRefDes(pstDest, pstSource);
	case eSIxDESCTAG_TIME_SHIFTED_SERVICE: 				return silib_desc_copyTimeShiftSvcDes(pstDest, pstSource);
	case eSIxDESCTAG_SHORT_EVENT:						return silib_desc_copyShortEvtDes(pstDest, pstSource);
	case eSIxDESCTAG_EXTENDED_EVENT:					return silib_desc_copyExtEvtDes(pstDest, pstSource);
	case eSIxDESCTAG_TIME_SHIFTED_EVENT:				return silib_desc_copyTimeShiftEvtDes(pstDest, pstSource);
	case eSIxDESCTAG_COMPONENT:							return silib_desc_copyCompDes(pstDest, pstSource);
	case eSIxDESCTAG_MOSAIC:								break;	//return silib_desc_copyMosaicDes(pstDest, pstSource);
	case eSIxDESCTAG_STREAM_IDENTIFIER:					return silib_desc_copyStmIdentDes(pstDest, pstSource);
	case eSIxDESCTAG_CA_IDENTIFIER:						return silib_desc_copyCaIdentDes(pstDest, pstSource);
	case eSIxDESCTAG_CONTENT:							return silib_desc_copyContentDes(pstDest, pstSource);
	case eSIxDESCTAG_PARENTAL_RATING:					return silib_desc_copyParentDes(pstDest, pstSource);
	case eSIxDESCTAG_TELETEXT: 							return silib_desc_copyTeletextDes(pstDest, pstSource);
	case eSIxDESCTAG_TELEPHONE:							return silib_desc_copyTelephoneDes(pstDest, pstSource);
	case eSIxDESCTAG_LOCAL_TIME_OFFSET:					return silib_desc_copyLocalTimeDes(pstDest, pstSource);
	case eSIxDESCTAG_SUBTITLING:						return silib_desc_copySubtitleDes(pstDest, pstSource);
	case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:		return silib_desc_copyTerDeliSystemDes(pstDest, pstSource);
	case eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME:			return silib_desc_copyMultiNetNameDes(pstDest, pstSource);
	case eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME:			return silib_desc_copyMultiBouqNameDes(pstDest, pstSource);
	case eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME:			return silib_desc_copyMultiSvcNameDes(pstDest, pstSource);
	case eSIxDESCTAG_MULTILINGUAL_COMPONENT:			return silib_desc_copyMultiCompDes(pstDest, pstSource);
	case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:			return silib_desc_copyPrivateDataSpecDes(pstDest, pstSource);
	case eSIxDESCTAG_SERVICE_MOVE: 						return silib_desc_copySvcMoveDes(pstDest, pstSource);
	case eSIxDESCTAG_SHORT_SMOOTHING_BUFFER:			return silib_desc_copyShortSmoothBufDes(pstDest, pstSource);
	case eSIxDESCTAG_FREQUENCY_LIST:					return silib_desc_copyFrequencyListDes(pstDest, pstSource);
	case eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM: 			return silib_desc_copyPartialTsDes(pstDest, pstSource);
	case eSIxDESCTAG_DATA_BROADCAST:					return silib_desc_copyDataBroadcastDes(pstDest, pstSource);
	case eSIxDESCTAG_DATA_BROADCAST_ID:					return silib_desc_copyDataBroadcastIdDes(pstDest, pstSource);
	case eSIxDESCTAG_PDC:								return silib_desc_copyPDCDes(pstDest, pstSource);
	case eSIxDESCTAG_DOLBY_AC3:							return silib_desc_copyDolbyAC3Des(pstDest, pstSource);
	case eSIxDESCTAG_ANCILLARY_DATA:						break;	//return silib_desc_copyAncillaryDataDes(pstDest, pstSource);
	case eSIxDESCTAG_CELL_LIST:							return silib_desc_copyCellListDes(pstDest, pstSource);
	case eSIxDESCTAG_CELL_FREQ_LIST:						break;	//return silib_desc_copyCellFreqListDes(pstDest, pstSource);
	case eSIxDESCTAG_ANNOUNCEMENT_SUPPORT: 					break;	//return silib_desc_copyAnnouncementSupportDes(pstDest, pstSource);
	case eSIxDESCTAG_APPLICATION_SIGNALLING:			return silib_desc_copyAppSignallingDes(pstDest, pstSource);
	case eSIxDESCTAG_ADAPTATION_FIELD_DATA:					break;	//return silib_desc_copyAdaptationFieldDataDes(pstDest, pstSource);
	case eSIxDESCTAG_SVC_IDENTIFIER:						break;	//silib_desc_copySvcIdentifierDes(pstDest, pstSource);
	case eSIxDESCTAG_SVC_AVAILABILITY: 						break;	//return silib_desc_copySvcAvailabilityDes(pstDest, pstSource);
	case eSIxDESCTAG_DEFAULT_AUTHORITY: 					return silib_desc_copyDefaultAuthorityDes(pstDest, pstSource);
	case eSIxDESCTAG_RELATED_CONTENT:					return silib_desc_copyRelatedContentDes(pstDest, pstSource);
	case eSIxDESCTAG_CONTENT_IDENTIFIER:				return silib_desc_copyContentIdentifierDes(pstDest, pstSource);
	case eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM: 		return silib_desc_copyS2SatDeliSystemDes(pstDest, pstSource);
	case eSIxDESCTAG_ENHANCED_AC3: 						return silib_desc_copyEnhancedAC3Des(pstDest, pstSource);
	case eSIxDESCTAG_DTS:									break;	//return silib_desc_copyDTSDes(pstDest, pstSource);
	case eSIxDESCTAG_AAC:								return silib_desc_copyAACDes(pstDest, pstSource);
	case eSIxDESCTAG_XAIT_LOCATION:							break;	//return silib_desc_copyXaitLocationDes(pstDest, pstSource);
	case eSIxDESCTAG_FTA_CONTENT_MANAGEMENT:			return silib_desc_copyFtaContentManagementDes(pstDest, pstSource);
	case eSIxDESCTAG_EXTENSION:							return silib_desc_copyExtensionDes(pstDest, pstSource);

	default:											break;
	}

	return eErr;
}

SIxError_e silib_desc_print_Dvb(const SIxDescriptor_t *pstDes)
{
	SIxError_e		eErr;

//	if (NULL == pstDes)		{ return eSIxERR_FAIL; }

	HxLOG_Print("\t\t(eTag  : 0x%02x [%s])\n", pstDes->stHead.eTag, silib_desc_getTagString_Dvb(pstDes->stHead.eTag) );
	HxLOG_Print("\t\t(eSpec : 0x%02x)\n", pstDes->stHead.eSpec );

	eErr = eSIxERR_NOT_SUPPORT;
	switch (pstDes->stHead.eTag)
	{
	case eSIxDESCTAG_VIDEO_STREAM: 					silib_desc_printVideoStmDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_AUDIO_STREAM: 					silib_desc_printAudioStmDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_HIERARCHY:						silib_desc_printHierDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_REGISTRATION: 					silib_desc_printRegDes(pstDes);						eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_DATA_STREAM_ALIGNMENT:			silib_desc_printDataStmAlignDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_TARGET_BACKGROUND_GRID:		silib_desc_printBackGridDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_VIDEO_WINDOW: 					silib_desc_printVideoWindowDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_CA:							silib_desc_printCaDes(pstDes);						eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_ISO_639_LANGUAGE: 				silib_desc_printIso639LangDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SYSTEM_CLOCK: 					silib_desc_printSysClockDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_MULTIPLEX_BUFFER_UTILIZATION: 	silib_desc_printMultiplexBufUtilDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_COPYRIGHT:						silib_desc_printCopyRightDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_MAXIMUM_BITRATE:				silib_desc_printMaxBitrateDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_PRIVATE_DATA_INDICATOR:		silib_desc_printPrivateDataIndicatorDes(pstDes);	eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SMOOTHING_BUFFER: 				silib_desc_printSmoothBufDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_STD:							silib_desc_printStdDes(pstDes);						eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_CAROUSEL_ID:					silib_desc_printCarouselIdDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_ASSOCIATION_TAG:				silib_desc_printAssocTagDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_DEFERRED_ASSOCIATION_TAG: 		silib_desc_printDeferAssocTagDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_AVC_VIDEO:						silib_desc_printAvcVideoDes(pstDes);				eErr = eSIxERR_OK;	break;
	/** < ETS 300 468에 정의된 descriptor tag */
	case eSIxDESCTAG_NETWORK_NAME: 					silib_desc_printNetNameDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SERVICE_LIST: 					silib_desc_printSvcListDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_STUFFING: 						//silib_desc_printStuffingDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SATELLITE_DELIVERY_SYSTEM:		silib_desc_printSatDeliSystemDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_CABLE_DELIVERY_SYSTEM:			silib_desc_printCabDeliSystemDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_BOUQUET_NAME: 					silib_desc_printBouqNameDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SERVICE:						silib_desc_printSvcDes(pstDes);						eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_COUNTRY_AVAILABILITY: 			silib_desc_printCountryAvailDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_LINKAGE:						silib_desc_printLinkDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_NVOD_REFERENCE:				silib_desc_printNvodRefDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_TIME_SHIFTED_SERVICE: 			silib_desc_printTimeShiftSvcDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SHORT_EVENT:					silib_desc_printShortEvtDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXTENDED_EVENT:				silib_desc_printExtEvtDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_TIME_SHIFTED_EVENT:			silib_desc_printTimeShiftEvtDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_COMPONENT:						silib_desc_printCompDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_MOSAIC:							eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_printMosaicDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_STREAM_IDENTIFIER:				silib_desc_printStmIdentDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_CA_IDENTIFIER:					silib_desc_printCaIdentDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_CONTENT:						silib_desc_printContentDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_PARENTAL_RATING:				silib_desc_printParentDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_TELETEXT: 						silib_desc_printTeletextDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_TELEPHONE:						silib_desc_printTelephoneDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_LOCAL_TIME_OFFSET:				silib_desc_printLocalTimeDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SUBTITLING:					silib_desc_printSubtitleDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_TERRESTRIAL_DELIVERY_SYSTEM:	silib_desc_printTerDeliSystemDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME:		silib_desc_printMultiNetNameDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME:		silib_desc_printMultiBouqNameDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME:		silib_desc_printMultiSvcNameDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_MULTILINGUAL_COMPONENT:		silib_desc_printMultiCompDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_PRIVATE_DATA_SPECIFIER:		silib_desc_printPrivateDataSpecDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SERVICE_MOVE: 					silib_desc_printSvcMoveDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SHORT_SMOOTHING_BUFFER:		silib_desc_printShortSmoothBufDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FREQUENCY_LIST:				silib_desc_printFrequencyListDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_PARTIAL_TRANSPORT_STREAM: 		silib_desc_printPartialTsDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_DATA_BROADCAST:				silib_desc_printDataBroadcastDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_DATA_BROADCAST_ID:				silib_desc_printDataBroadcastIdDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_PDC:							silib_desc_printPDCDes(pstDes);						eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_DOLBY_AC3:						silib_desc_printDolbyAC3Des(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_ANCILLARY_DATA:					eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_printAncillaryDataDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_CELL_LIST:						silib_desc_printCellListDes(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_CELL_FREQ_LIST:					eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_printCellFreqListDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_ANNOUNCEMENT_SUPPORT: 				eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_printAnnouncementSupportDes(pstDes);	eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_APPLICATION_SIGNALLING:		silib_desc_printAppSignallingDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_ADAPTATION_FIELD_DATA:				eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_printAdaptationFieldDataDes(pstDes);	eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SVC_IDENTIFIER:					eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_allocSvcIdentifierDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_SVC_AVAILABILITY: 					eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_printSvcAvailabilityDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_DEFAULT_AUTHORITY: 				silib_desc_printDefaultAuthorityDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_RELATED_CONTENT:				silib_desc_printRelatedContentDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_CONTENT_IDENTIFIER:			silib_desc_printContentIdentifierDes(pstDes);		eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_S2_SATELLITE_DELIVERY_SYSTEM: 	silib_desc_printS2SatDeliSystemDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_ENHANCED_AC3: 					silib_desc_printEnhancedAC3Des(pstDes);				eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_DTS:								eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_printDTSDes(pstDes);					eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_AAC:							silib_desc_printAACDes(pstDes);						eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_XAIT_LOCATION:						eErr = eSIxERR_NOT_SUPPORT;	break;
														//silib_desc_printXaitLocationDes(pstDes);			eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_FTA_CONTENT_MANAGEMENT:		silib_desc_printFtaContentManagementDes(pstDes);	eErr = eSIxERR_OK;	break;
	case eSIxDESCTAG_EXTENSION:						silib_desc_printExtensionDes(pstDes);				eErr = eSIxERR_OK;	break;

	default:										eErr = eSIxERR_NOT_SUPPORT;	break;
	}

	return eErr;
}


static HUINT8 *silib_desc_allocInfo_Dvb(SIxDescriptorTag_e eTag, HUINT16 usSubTag)
{
	HUINT8	*pNewDesInfo = NULL;

	switch (eTag)
	{
	case eSIxDESCTAG_CAROUSEL_ID:					pNewDesInfo = (HUINT8 *)allocCarouselIdDesInfo();	break;
	case eSIxDESCTAG_ISO_639_LANGUAGE:				pNewDesInfo = (HUINT8 *)allocLangInfo();			break;
	case eSIxDESCTAG_SERVICE_LIST:					pNewDesInfo = (HUINT8 *)allocSvcListInfo();			break;
	case eSIxDESCTAG_COUNTRY_AVAILABILITY:			pNewDesInfo = (HUINT8 *)allocCountryAvailInfo();	break;
	case eSIxDESCTAG_NVOD_REFERENCE:				pNewDesInfo = (HUINT8 *)allocNvodRefInfo();			break;
	case eSIxDESCTAG_EXTENDED_EVENT:				pNewDesInfo = (HUINT8 *)allocExtEvtInfo();			break;
	case eSIxDESCTAG_CA_IDENTIFIER:					pNewDesInfo = (HUINT8 *)allocCaIdentInfo();			break;
	case eSIxDESCTAG_CONTENT:						pNewDesInfo = (HUINT8 *)allocContentInfo();			break;
	case eSIxDESCTAG_PARENTAL_RATING:				pNewDesInfo = (HUINT8 *)allocParentInfo();			break;
	case eSIxDESCTAG_TELETEXT:						pNewDesInfo = (HUINT8 *)allocTeletextInfo();		break;
	case eSIxDESCTAG_LOCAL_TIME_OFFSET:				pNewDesInfo = (HUINT8 *)allocLocalTimeInfo();		break;
	case eSIxDESCTAG_SUBTITLING:					pNewDesInfo = (HUINT8 *)allocSubtitleInfo();		break;
	case eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME:		pNewDesInfo = (HUINT8 *)allocMultiNetNameInfo();	break;
	case eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME:		pNewDesInfo = (HUINT8 *)allocMultiBouqNameInfo();	break;
	case eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME:		pNewDesInfo = (HUINT8 *)allocMultiSvcNameInfo();	break;
	case eSIxDESCTAG_MULTILINGUAL_COMPONENT:		pNewDesInfo = (HUINT8 *)allocMultiCompInfo();		break;
	case eSIxDESCTAG_APPLICATION_SIGNALLING:		pNewDesInfo = (HUINT8 *)allocAppSignallingInfo ();	break;
	case eSIxDESCTAG_CONTENT_IDENTIFIER:			pNewDesInfo = (HUINT8 *)allocCridInfo();			break;
	case eSIxDESCTAG_DATA_BROADCAST:
		if (usSubTag == 0x0006)
		{
			pNewDesInfo = (HUINT8 *)allocDataBroadcastInfo();
		}
		else if (usSubTag == 0x0007)
		{
			pNewDesInfo = (HUINT8 *)allocObjectCarouselInfo();
		}
		break;
 	case eSIxDESCTAG_CELL_LIST:					pNewDesInfo = (HUINT8 *)allocCellListInfo ();		break;
	default :									HxLOG_Print("alloc fail Tag = 0x%x\n", eTag);		break;
	}

	return ((HUINT8 *)pNewDesInfo);
}

static void silib_desc_freeInfo_Dvb(SIxDescriptorTag_e eTag, HUINT16 usSubTag, HUINT8 *pucDesInfo)
{
	if (pucDesInfo == NULL)
	{
		return;
	}

	switch (eTag)
	{
	case eSIxDESCTAG_ISO_639_LANGUAGE:				freeLangInfo((SIxLangInfo_t *)pucDesInfo);						break;
	case eSIxDESCTAG_SERVICE_LIST:					freeSvcListInfo((SIxSvcListInfo_t *)pucDesInfo);					break;
	case eSIxDESCTAG_COUNTRY_AVAILABILITY:			freeCountryAvailInfo((SIxCountryAvailInfo_t *)pucDesInfo);		break;
	case eSIxDESCTAG_NVOD_REFERENCE:				freeNvodRefInfo((SIxNvodRefInfo_t *)pucDesInfo);					break;
	case eSIxDESCTAG_EXTENDED_EVENT:				freeExtEvtInfo((SIxExtEvtInfo_t *)pucDesInfo);					break;
	case eSIxDESCTAG_CA_IDENTIFIER:					freeCaIdentInfo((SIxCaIdentInfo_t *)pucDesInfo);					break;
	case eSIxDESCTAG_CONTENT:						freeContentInfo((SIxContentInfo_t *)pucDesInfo);					break;
	case eSIxDESCTAG_PARENTAL_RATING:				freeParentInfo((SIxParentInfo_t *)pucDesInfo);					break;
	case eSIxDESCTAG_TELETEXT:						freeTeletextInfo((SIxTeletextInfo_t *)pucDesInfo);				break;
	case eSIxDESCTAG_LOCAL_TIME_OFFSET:				freeLocalTimeInfo((SIxLocalTimeInfo_t *)pucDesInfo);				break;
	case eSIxDESCTAG_SUBTITLING:					freeSubtitleInfo((SIxSubtitleInfo_t *)pucDesInfo);				break;
	case eSIxDESCTAG_MULTILINGUAL_NETWORK_NAME:		freeMultiNetNameInfo((SIxMultiNetNameInfo_t *)pucDesInfo);		break;
	case eSIxDESCTAG_MULTILINGUAL_BOUQUET_NAME:		freeMultiBouqNameInfo((SIxMultiBouqNameInfo_t *)pucDesInfo);		break;
	case eSIxDESCTAG_MULTILINGUAL_SERVICE_NAME:		freeMultiSvcNameInfo((SIxMultiSvcNameInfo_t *)pucDesInfo);		break;
	case eSIxDESCTAG_MULTILINGUAL_COMPONENT:		freeMultiCompInfo((SIxMultiCompInfo_t *)pucDesInfo);				break;
	case eSIxDESCTAG_APPLICATION_SIGNALLING:		freeAppSignallingInfo ((SIxAppSignallingInfo_t *)pucDesInfo);	break;
	case eSIxDESCTAG_CONTENT_IDENTIFIER:			freeCridInfo((SIxCridInfo_t*)pucDesInfo);						break;
	case eSIxDESCTAG_DATA_BROADCAST:
		if (usSubTag == 0x0006)
		{
			freeDataBroadcastInfo((SIxDataCarouselInfo_t *)pucDesInfo);
		}
		else if (usSubTag == 0x0007)
		{
			freeObjectCarouselInfo((SIxObjectCarouselInfo_t *)pucDesInfo);
		}
		break;
	case eSIxDESCTAG_CELL_LIST:					freeCellListInfo((SIxCellListInfo_t *)pucDesInfo);				break;
	default :									HxLOG_Print("[ERROR] Free fail Tag = 0x%x\n", eTag);			break;
	}
}

/* end of file */
