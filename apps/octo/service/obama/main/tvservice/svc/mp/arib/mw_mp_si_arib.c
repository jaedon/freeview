/** **********************************************************************************************************
	@file 		mw_mp_si_arib.c

	@date		2012/01/02
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include <octo_common.h>

#include "svc_mplayer.h"
#include "../_svc_mp_manager.h"
#include "../_svc_mp_utils.h"
#include "pal_media.h"
#include "pal_media_sub.h"
#include "pal_scaler.h"
#include "pal_video.h"
#include "pal_audio.h"
#include "pal_output.h"
#include "pal_thumbnail.h"
#include "stb_params.h"
#include "pal_sef.h"
#include "svc_mplayersi.h"

#include <string.h>
#include <pal_datastream.h>
#include <arib_descriptor.h>

/*---------------------------------------------------------------------------------
 * defines
 *---------------------------------------------------------------------------------*/
#if defined(CONFIG_DEBUG)
//#define DEBUG_SIT_PARSING
#endif

#define SVC_MP_SI_AUDIO_CHANNEL_NUM_NA				0x00
#define SVC_MP_SI_AUDIO_CHANNEL_NUM_STEREO			0x01
#define SVC_MP_SI_AUDIO_CHANNEL_NUM_MULTI			0x02

#define	SVC_MP_AC3_ComponentTypeToChannelNum(a)		(((a&0x07)<=0x03) ? SVC_MP_SI_AUDIO_CHANNEL_NUM_STEREO : (((a&0x07)<=0x05) ? SVC_MP_SI_AUDIO_CHANNEL_NUM_MULTI : SVC_MP_SI_AUDIO_CHANNEL_NUM_NA))
#define	SVC_MP_AAC_ProfileLevelToChannelNum(a)		(((a>=0x50) && (a<=0x59)) ? SVC_MP_SI_AUDIO_CHANNEL_NUM_STEREO : (((a>=0x5A) && (a<=0x5B)) ? SVC_MP_SI_AUDIO_CHANNEL_NUM_MULTI : SVC_MP_SI_AUDIO_CHANNEL_NUM_NA))


/*---------------------------------------------------------------------------------
 * typedef
 *---------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------
  static variables
 *---------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------
 * static functions
 *---------------------------------------------------------------------------------*/
static HERROR local_si_FindVideoCodecFromPSI(HUINT32 ulStreamType, DxVideoCodec_e *ucCodec)
{
	if ( ucCodec == NULL )
	{
		return ERR_FAIL;
	}

	/* SI에서 결정 하기로 합의 됨. */
	switch (ulStreamType)
	{
	/* VIDEO */
	case ISO_VIDEO_1 :
	case ISO_VIDEO_2 :
		*ucCodec = eDxVIDEO_CODEC_MPEG2;
		break;

	case ISO_VIDEO_4 :
		*ucCodec = eDxVIDEO_CODEC_H264;
		break;

	default :
		*ucCodec = eDxVIDEO_CODEC_UNKNOWN;
		break;
	}

	return ERR_OK;
}

#if 0
static HERROR local_si_GetComponentTag( MwcRawPool *pstPool, HUINT16 usTsUniqId,
												HUINT32 ulIdx,
												HUINT32 nSubIdx,
												HUINT8 *ucComponentTag)
{
	HUINT32				ulRet;
	svcCas_StmIdentDes_t		*streamIdDes = NULL;

	if( (pstPool == NULL) || (ucComponentTag == NULL) )
	{
		return ERR_FAIL;
	}

	ulRet = MWC_SI_GetPmtElementDescriptor (pstPool, usTsUniqId, ulIdx, nSubIdx,
							TAG_STREAM_IDENTIFIER, 0, (void **)&streamIdDes);
	if ( ulRet != ERR_OK )
	{
		return ERR_FAIL;
	}

	*ucComponentTag = streamIdDes->ucCompTag;
	MWC_SI_FreeDescriptor (streamIdDes);

	return ERR_OK;
}
#endif

static HERROR local_si_FindAudioCodecFromPSI(HUINT32 ulStreamType, DxAudioCodec_e *ucCodec)
{
	if ( ucCodec == NULL )
	{
		return ERR_FAIL;
	}

	/* SI에서 결정 하기로 합의 됨. */
	switch (ulStreamType)
	{

	/* AUDIO */
	case ISO_AUDIO_1 :
		*ucCodec = eDxAUDIO_CODEC_MPEG;	/* ISO/IEC 11172 Audio */
		break;

	case ISO_AUDIO_2 :
		*ucCodec = eDxAUDIO_CODEC_MPEG;	/* ISO/IEC 13818-3 Audio */
		break;

	case ISO_AUDIO_AAC :
		*ucCodec = eDxAUDIO_CODEC_AAC;	/* ISO/IEC 13818-3 Audio */
		break;

	case ISO_AUDIO_4 :
		*ucCodec = eDxAUDIO_CODEC_MPEG;
		break;

	default :
		*ucCodec = eDxAUDIO_CODEC_UNKNOWN;
		break;
	}

	return ERR_OK;
}

static DxAudioCodec_e local_si_GetaudCodeFromAACType(HUINT8 ucAACType)
{
	switch (ucAACType)
	{
		case 0x01 :
		case 0x03 :
		case 0x05 :
		case 0x40 :
		case 0x41 :
		case 0x42 :
		case 0x43 :
		case 0x44 :
		case 0x45 :
		case 0x46 :
		case 0x47 :
		case 0x48 :
		case 0x49 :
		case 0x4A :
		case 0x58 :
		case 0x59 :
		case 0x5A :
		case 0x5B :
			return eDxAUDIO_CODEC_AAC_PLUS;

		default :
			return eDxAUDIO_CODEC_AAC;
	}
}

static HERROR local_si_ConvertExtEvtItemTextToUtf8(MW_MEDIA_ExtEvt_t *pstExtEvt)
{
	HUINT32		i, strIndex;
	HINT32		numItemBytes;
	HINT32		mergedStrLen;
	HUINT8		*pszTmpItemsText = NULL, *pszTmpUtfString;

	/* UTF8으로 변환하기전 Item Num에 해당하는 Text를 합친다. */

	/* item의 Text Size 파악 */
	numItemBytes = 0;
	for (i = 0; i < pstExtEvt->ucItemNum; i++)
	{
		numItemBytes += HxSTD_StrLen(pstExtEvt->astItem[i].pszItem);
	}
	pszTmpItemsText = HLIB_STD_MemAlloc(numItemBytes + 1);	/* add 1 byte to insert NULL character */
	if (pszTmpItemsText == NULL)
	{
		return ERR_FAIL;
	}

	/* item Text를 하나의 String으로 합친다 */
	strIndex = 0;
	for (i = 0; i < pstExtEvt->ucItemNum; i++)
	{
		HxSTD_MemCopy(pszTmpItemsText + strIndex, pstExtEvt->astItem[i].pszItem, HxSTD_StrLen(pstExtEvt->astItem[i].pszItem));
		strIndex += HxSTD_StrLen(pstExtEvt->astItem[i].pszItem);
	}
	pszTmpItemsText[strIndex] = '\0';

	if (strIndex != numItemBytes)
	{
		HxLOG_Critical("============= Error strIndex=%d numItemBytes=%d==========\n", strIndex, numItemBytes);
		HLIB_STD_MemFree(pszTmpItemsText);
		pszTmpItemsText = NULL;

		return ERR_FAIL;
	}

	/* 합쳐진 문자열을 UTF8으로 변환 한다. */
	pszTmpUtfString = NULL;
	mergedStrLen = MWC_UTIL_ConvSiStringToUtf8(pstExtEvt->szIso639LangCode, strIndex, pszTmpItemsText, &pszTmpUtfString);
	if (mergedStrLen == 0)
	{
		HLIB_STD_MemFree(pszTmpItemsText);
		pszTmpItemsText = NULL;

		if (pszTmpUtfString != NULL)
		{
			HLIB_STD_MemFree(pszTmpUtfString);
			pszTmpUtfString = NULL;
		}

		return ERR_FAIL;
	}
	HLIB_STD_MemFree(pszTmpItemsText);
	pszTmpItemsText = NULL;

	HxLOG_Print("======================================\n");
	HxLOG_Print("Merged Text[%d][%d]=%s\n",numItemBytes, mergedStrLen, pszTmpUtfString);
	HxLOG_Print("======================================\n\n");

	/* 변환된 UTF8문자를 Item에 다시 넣는다. */
	pstExtEvt->ucItemNum = 0;
	for (i = 0; i < MW_MEDIA_EXT_ITEM_MAX; i++)
	{
		MW_MEDIA_ExtEvtItem_t *pstItem = &pstExtEvt->astItem[i];

		if (pstItem->pszItem != NULL)
		{
			HLIB_STD_MemFree(pstItem->pszItem);
			pstItem->pszItem = NULL;
		}

		if (mergedStrLen > 0)
		{
			pstItem->pszItem = HLIB_STD_MemAlloc(MW_MEDIA_EVENT_TEXT);
			HxSTD_MemSet(pstItem->pszItem, 0, MW_MEDIA_EVENT_TEXT);
			HxSTD_MemCopy(pstItem->pszItem, pszTmpUtfString + (MW_MEDIA_EVENT_TEXT - 1)*i, MW_MEDIA_EVENT_TEXT - 1);
			pstItem->pszItem[(MW_MEDIA_EVENT_TEXT - 1)] = '\0';
			mergedStrLen -= (MW_MEDIA_EVENT_TEXT - 1);
			pstExtEvt->ucItemNum++;

			HxLOG_Print("Split Text[%d]=%s\n",i, pstItem->pszItem);
		}
	}

	if (pszTmpUtfString != NULL)
	{
		HLIB_STD_MemFree(pszTmpUtfString);
		pszTmpUtfString = NULL;
	}

	return ERR_OK;
}

/* OCTO 2.0에서는 Descriptor Parsing시 문자열에 대해서 UTF8으로 변환 한다.
   하지만 일본향의 Extended Event Info Descriptor는 문자 코드 처리상 여러 Extended Event Info의 Descriptor가
   합쳐진 다음에 UTF8으로 변환 해야 올바른 변환이 수행된다. */
static MW_MEDIA_ExtEvt_t *local_si_GetMergedExtEvtInfo(MW_MEDIA_ExtEvt_t *pstExtEvt)
{
	MW_MEDIA_ExtEvt_t	*pstTempExtEvt = NULL, *pstPrevExtEvt = NULL, *pstFreeExtEvt = NULL;
	HUINT32 		k, ulExtEvtCnt = 0;
	MW_MEDIA_ExtEvtItem_t *pstItem = NULL;
	HUINT8 *pstItemText = NULL;

	if (NULL == pstExtEvt)
	{
		return NULL;
	}

	/* Extended Event Descriptor의 갯수를 파악 */
	pstTempExtEvt = pstExtEvt;
	while (pstTempExtEvt)
	{
		ulExtEvtCnt++;
		pstTempExtEvt = pstTempExtEvt->pNext;
	}

	if (ulExtEvtCnt == 0)
	{
		HxLOG_Critical("[%s, %d line] none Ext event !\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	/* Parsing된 item에 대해 Description Text가 없는 item은 이전 ExtEvt Descriptor의 item에 연결 */
	pstTempExtEvt = pstExtEvt;
	while (pstTempExtEvt)
	{
		pstItem = &pstTempExtEvt->astItem[0];

		/* Arib에서 ucItemNum은 항상 1이다. */
		if (HxSTD_StrLen(pstItem->pszDescription) != 0)
		{
			pstPrevExtEvt = pstTempExtEvt;
		}
		else
		{
			if ((HxSTD_StrLen(pstItem->pszItem) != 0) && (pstPrevExtEvt != NULL) && (pstPrevExtEvt->ucItemNum < (MW_MEDIA_EXT_ITEM_MAX - 1)))
			{
				pstItemText = HLIB_STD_StrDup(pstItem->pszItem);
				if (pstItemText != NULL)
				{
					if (pstPrevExtEvt->astItem[pstPrevExtEvt->ucItemNum].pszItem != NULL)
					{
						HLIB_STD_MemFree(pstPrevExtEvt->astItem[pstPrevExtEvt->ucItemNum].pszItem);
					}
					pstPrevExtEvt->astItem[pstPrevExtEvt->ucItemNum].pszItem = pstItemText;
					pstPrevExtEvt->ucItemNum += 1;
					pstPrevExtEvt->pNext = pstTempExtEvt->pNext;
					pstFreeExtEvt = pstTempExtEvt;
				}
			}
		}

		pstTempExtEvt = pstTempExtEvt->pNext;

		/* 이전 ExtEvt에 item을 복사한 ExtEvt의 메모리 초기화 */
		if (pstFreeExtEvt != NULL)
		{
			if (pstFreeExtEvt->pszChar != NULL)
			{
				HLIB_STD_MemFree(pstFreeExtEvt->pszChar);
				pstFreeExtEvt->pszChar = NULL;
			}

			for (k=0; k<pstFreeExtEvt->ucItemNum; k++)
			{
				if (pstFreeExtEvt->astItem[k].pszDescription != NULL)
				{
					HLIB_STD_MemFree(pstFreeExtEvt->astItem[k].pszDescription);
					pstFreeExtEvt->astItem[k].pszDescription = NULL;
				}

				if (pstFreeExtEvt->astItem[k].pszItem != NULL)
				{
					HLIB_STD_MemFree(pstFreeExtEvt->astItem[k].pszItem);
					pstFreeExtEvt->astItem[k].pszItem = NULL;
				}
			}

			HLIB_STD_MemFree(pstFreeExtEvt);
			pstFreeExtEvt = NULL;
		}
	}

	/* ExtEvt Descriptor별로 item항목에 대해서 UTF8변환 */
	pstTempExtEvt = pstExtEvt;
	while (pstTempExtEvt)
	{
		local_si_ConvertExtEvtItemTextToUtf8(pstTempExtEvt);
		pstTempExtEvt = pstTempExtEvt->pNext;
	}

	return pstExtEvt;
}

HERROR SVC_MP_SI_UpdatePmtInfo(MW_MediaSiContext_t *pstPlayContext)
{
	HERROR ulRet = ERR_OK;
	HUINT16 usTsUniqId;
	HUINT16 usSvcId;
	MW_MEDIA_PmtInfo_t *pstPmtInfo;
	HUINT32 i, ulPmtIndex=0, ulEsElem=0;
	MwcRawPool *pstPool = NULL;
	PmtProgramInfo_t stPmtProgInfo;
	PmtElementInfo_t stPmtElemInfo;
	HUINT32	videoCnt = 0, audioCnt = 0, captionCnt = 0;
	HBOOL	bLowLevelTransmission = FALSE;

	DvhsDTCPDes				*pstDtcpDes = NULL;
	DigitalCopyControlDes 	*pstDigitalCopyControlDes = NULL;
	ContentAvailabilityDes 	*pstContentAvailabilityDes = NULL;

	HUINT8				tmpVideoTag	= 0x00;
	HUINT8				tmpAudioTag	= 0x10;
	svcCas_StmIdentDes_t		*pStreamIdDes = NULL;
#if 0
	svcCas_DolbyAc3Des_t		*pAC3Des = NULL;
	svcCas_EnhancedAC3Des_t	*pE_AC3Des = NULL;
#endif
	svcCas_AvcVideoDes_t		*pAvcVideoDes = NULL;
	svcCas_AACDes_t			*pAACDes = NULL;
	void				*pPrivateDes = NULL;
	HierarchicalTransmissionDes *pHierTransDes = NULL;
	DataComponentDes	*pstDataComponentDes = NULL;

	if (pstPlayContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	pstPool = (MwcRawPool *)pstPlayContext->pstRawDataPool;
	if (pstPool == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	usTsUniqId = pstPlayContext->usTsUniqId;
	usSvcId = pstPlayContext->usPmtSvcId;
	pstPmtInfo = &pstPlayContext->stPmtInfo;

	/* Initialize */
	HxSTD_MemSet(pstPmtInfo, 0, sizeof(MW_MEDIA_PmtInfo_t));
	pstPmtInfo->usPcrPid = PID_NULL;

	/* Service PID */
	pstPmtInfo->usServiceId = usSvcId;

	ulRet = MWC_SI_GetPmtIndexByServiceId(pstPool, usTsUniqId, usSvcId, &ulPmtIndex);
	if ( ulRet != ERR_OK )
	{
		HxLOG_Error("[UpdatePmtInfo] MWC_SI_GetPmtIndexByServiceId, ulRet:0x%X\n", ulRet);
		return ERR_FAIL;
	}

	ulRet = MWC_SI_GetPmtProgramInfo(pstPool, usTsUniqId, ulPmtIndex, &stPmtProgInfo);
	if ( ulRet != ERR_OK )
	{
		HxLOG_Error("[UpdatePmtInfo] MWC_SI_GetPmtProgramInfo, ulRet:0x%X\n", ulRet);
		return ERR_FAIL;
	}

	pstPmtInfo->ucVerNum = stPmtProgInfo.ucVer;
	pstPmtInfo->ulCrc = stPmtProgInfo.ulCrc32;
	pstPmtInfo->usPcrPid = stPmtProgInfo.usPCRPid;

	/* DRM 정보 초기화 */
	//MGR_COPYRIGHTCONTROL_InitCcInfo(&pstPmtInfo->stDrmInfo, eDrmModelType_Arib, eDxCopyrightControl_RestrictLevel_None);

	/************************************************************/
	/*                    Program loop of PMT                   */
	/************************************************************/
	/* 0xC1 TAG_DIGITAL_COPY_CONTROL */
	pPrivateDes = NULL;
	ulRet = MWC_SI_GetPmtProgramDescriptor(pstPool, usTsUniqId, ulPmtIndex, TAG_DIGITAL_COPY_CONTROL, 0, (void **)&pPrivateDes);
	if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
	{
		pstDigitalCopyControlDes = (DigitalCopyControlDes*)ARIB_DES_AllocDigitalCopyControlDes();
		if (pstDigitalCopyControlDes != NULL)
		{
			ulRet = ARIB_DES_ParseDigitalCopyControlDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstDigitalCopyControlDes);
			if (ulRet == ERR_OK)
			{
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyControlType = pstDigitalCopyControlDes->ucCopyControlType;
				#if 0	/* TAG_DTCP에서 파악 */
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucDigitalRecording = pstDigitalCopyControlDes->ucDigitalRecordingControlData;
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucAnalogRecording = pstDigitalCopyControlDes->ucAPSControlData;
				#endif
			}
		}
		ARIB_DES_FreeDigitalCopyControlDes((HUINT8 *)pstDigitalCopyControlDes);
		pstDigitalCopyControlDes = NULL;
	}
	else
	{
		HxLOG_Warning("No TAG_DIGITAL_COPY_CONTROL\n");
	}
	MWC_SI_FreeDescriptor(pPrivateDes);
	pPrivateDes = NULL;

	/* 0xDE	TAG_CONTENT_AVAILABILITY */
	pPrivateDes = NULL;
	ulRet = MWC_SI_GetPmtProgramDescriptor(pstPool, usTsUniqId, ulPmtIndex, TAG_CONTENT_AVAILABILITY, 0, (void **)&pPrivateDes);
	if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
	{
		pstContentAvailabilityDes = (ContentAvailabilityDes*)ARIB_DES_AllocContentAvailabilityDes();
		if (pstContentAvailabilityDes != NULL)
		{
			ulRet = ARIB_DES_ParseContentAvailabilityDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstContentAvailabilityDes);
			if (ulRet == ERR_OK)
			{
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyRestrictionMode = 0;	/* DLNA 재생은 Dubbing-10이 존재 하지 않는다. */
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucImageContraintToken = pstContentAvailabilityDes->ucImageContraintToken;
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucRetentionMode = pstContentAvailabilityDes->ucRetentionMode;
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucRetentionState = pstContentAvailabilityDes->ucRetentionState;

				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucEncryptionMode = pstContentAvailabilityDes->ucEncryptionMode;

				#if defined(DEBUG_SIT_PARSING)
				ARIB_DES_PrintContentAvailabilityDes((HUINT8 *)pstContentAvailabilityDes);
				#endif
			}
			ARIB_DES_FreeContentAvailabilityDes((HUINT8 *)pstContentAvailabilityDes);
			pstContentAvailabilityDes = NULL;
		}
	}
	else
	{
		HxLOG_Warning("No TAG_CONTENT_AVAILABILITY\n");
	}
	MWC_SI_FreeDescriptor(pPrivateDes);
	pPrivateDes = NULL;

	/* Get DTCPDescriptor */
	ulRet = MWC_SI_GetPmtProgramDescriptor(pstPool, usTsUniqId, ulPmtIndex, TAG_DTCP, 0, (void **)&pPrivateDes);
	if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
	{
		pstDtcpDes = (DvhsDTCPDes*)ARIB_DES_AllocDtcpDes();
		if (pstDtcpDes != NULL)
		{
			ulRet = ARIB_DES_ParseDtcpDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstDtcpDes);
			if (ulRet == ERR_OK)
			{
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucDigitalRecording = pstDtcpDes->ucDTCPCCI;
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucEncryptionMode = pstDtcpDes->ucEPN;
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucAnalogRecording = pstDtcpDes->ucAPS;

				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyRestrictionMode = 0;	/* DLNA 재생은 Dubbing-10이 존재 하지 않는다. */
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucRetentionMode = pstDtcpDes->ucRetentionMoveMode;
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucRetentionState = pstDtcpDes->ucRetentionState;
				pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucImageContraintToken = pstDtcpDes->ucImageConstraintToken;

				#if 0	/* D단자가 있을 경우 처리 필요 */
				pstDtcpDes->ucAST;
				#endif
			}

			ARIB_DES_FreeDtcpDes((HUINT8 *)pstDtcpDes);
			pstDtcpDes = NULL;
		}
	}
	else
	{
		HxLOG_Warning("No TAG_DTCP\n");
	}
	MWC_SI_FreeDescriptor(pPrivateDes);
	pPrivateDes = NULL;

	/************************************************************/
	/*                    Element loop of PMT                   */
	/************************************************************/
	ulRet = MWC_SI_CountPmtElement(pstPool, usTsUniqId, ulPmtIndex, &ulEsElem);
	if ((ulRet != ERR_OK) || (ulEsElem == 0))
	{
		HxLOG_Error("[UpdatePmtInfo] MWC_SI_CountPmtElement, ulRet:0x%X, usEsElem:%d\n", ulRet, ulEsElem);
		return ERR_FAIL;
	}
	for (i = 0; i < ulEsElem; i++)
	{
		HUINT8 componentTag	= 0xFF;

		ulRet = MWC_SI_GetPmtElementInfo(pstPool, usTsUniqId, ulPmtIndex, i, &stPmtElemInfo);
		if ( ulRet != ERR_OK )
		{
			continue;
		}

		/* 저계층에 해당하는 StreamType은 녹화하지 않는다. */
		bLowLevelTransmission = FALSE;
		pPrivateDes = NULL;
		ulRet = MWC_SI_GetPmtElementDescriptor (pstPool, usTsUniqId, ulPmtIndex, i, TAG_HIERARCHICAL_TRANSMISSION, 0, (void **)&pPrivateDes);
		if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
		{
			pHierTransDes = (HierarchicalTransmissionDes *)ARIB_DES_AllocHierarchicalTransmissionDes();
			if (pHierTransDes != NULL)
			{
				ulRet = ARIB_DES_ParseHierarchicalTransmissionDes((HUINT8 *)pPrivateDes, (HUINT8 *)pHierTransDes);
				if (ulRet == ERR_OK)
				{
					if ((pHierTransDes->bQualityLevel == 0) && (pHierTransDes->usReferencePID < 0x1FFF))
					{
						bLowLevelTransmission = TRUE;
					}
				}
				ARIB_DES_FreeHierarchicalTransmissionDes((HUINT8 *)pHierTransDes);
				pHierTransDes = NULL;
			}
		}
		MWC_SI_FreeDescriptor(pPrivateDes);
		pPrivateDes = NULL;
		if (bLowLevelTransmission == TRUE)
		{
			continue;
		}

		ulRet = MWC_SI_GetPmtElementDescriptor (pstPool, usTsUniqId, ulPmtIndex, i, TAG_STREAM_IDENTIFIER, 0, (void **)&pStreamIdDes);
		if ((ulRet == ERR_OK) && (pStreamIdDes != NULL))
		{
			componentTag = pStreamIdDes->ucCompTag;
		}
		MWC_SI_FreeDescriptor(pStreamIdDes);
		pStreamIdDes = NULL;

		switch (stPmtElemInfo.ucStreamType)
		{
			/* VIDEO */
			case ISO_VIDEO_1 :
			case ISO_VIDEO_2 :
			case ISO_VIDEO_4 :
				if ( videoCnt >= MW_MEDIA_MAX_VIDEO_ES_NUM ) break;
				/*	component tag가 없는 경우에 임시로 만들어 주는 루틴. */
				if (componentTag == 0xFF)
				{
					/* to avoid misunderstanding of CATV 295 multi-audio. it used in Polaris model. */
					componentTag = tmpVideoTag++;
				}
				#if 0	/* Live에서만 필요 */
				if (componentTag > 0x10)
				{
					/* compTag값이 0x00~0x0F이외의 값은 직접 선국 대상이 아님 */
					HxLOG_Warning("Skip this video. ucStreamType=%#x componentTag=%d\n", stPmtElemInfo.ucStreamType, componentTag);
					break;
				}
				#endif

				pstPmtInfo->stVideoInfo[videoCnt].usPid = stPmtElemInfo.usElemPid;
				pstPmtInfo->stVideoInfo[videoCnt].ucStreamType = stPmtElemInfo.ucStreamType;
				pstPmtInfo->stVideoInfo[videoCnt].ucComponentTag = componentTag;
				local_si_FindVideoCodecFromPSI((HUINT32)stPmtElemInfo.ucStreamType, &pstPmtInfo->stVideoInfo[videoCnt].eVideoCodec);

				pstPmtInfo->stVideoInfo[videoCnt].bIsHD = FALSE;

				if(pstPmtInfo->stVideoInfo[videoCnt].eVideoCodec == eDxVIDEO_CODEC_H264)
				{
					ulRet = MWC_SI_GetPmtElementDescriptor (pstPool, usTsUniqId, ulPmtIndex, i, TAG_AVC_VIDEO, 0, (void **)&pAvcVideoDes);
					if ((ulRet == ERR_OK) &&
						(pAvcVideoDes != NULL) &&
						((pAvcVideoDes->ucProfile != 0x4D) || (pAvcVideoDes->ucProfile != 0x1E)))
					{
						pstPmtInfo->stVideoInfo[videoCnt].bIsHD = TRUE;
					}
					MWC_SI_FreeDescriptor (pAvcVideoDes);
					pAvcVideoDes = NULL;
				}
				videoCnt++;
				break;

			/* AUDIO */
			case ISO_AUDIO_1 :
			case ISO_AUDIO_2 :
			case ISO_AUDIO_AAC :
			case ISO_AUDIO_4 :
			case ISO_AUDIO_AC3:
				if ( audioCnt >= MW_MEDIA_MAX_AUDIO_ES_NUM ) break;
				/* component tag가 없는 경우에 임시로 만들어 주는 루틴 */
				if (componentTag == 0xFF)
				{
					/* to avoid misunderstanding of CATV 295 multi-audio. it used in Polaris model. */
					componentTag = tmpAudioTag++;
				}
				#if 0	/* Live에서만 필요 */
				if ((componentTag < 0x10) || (componentTag > 0x2F))
				{
					/* compTag값이 0x10~0x2F이외의 값은 직접 선국 대상이 아님 */
					HxLOG_Warning("Skip this audio. ucStreamType=%#x componentTag=%d\n", stPmtElemInfo.ucStreamType, componentTag);
					break;
				}
				#endif

				pstPmtInfo->stAudioInfo[audioCnt].usPid = stPmtElemInfo.usElemPid;
				pstPmtInfo->stAudioInfo[audioCnt].ucComponentTag = componentTag;
				local_si_FindAudioCodecFromPSI((HUINT32)stPmtElemInfo.ucStreamType, &pstPmtInfo->stAudioInfo[audioCnt].eAudCodec);

				/* AAC존재 여부를 확인한다. */
				ulRet = MWC_SI_GetPmtElementDescriptor (pstPool, usTsUniqId, ulPmtIndex, i, TAG_AAC, 0, (void **)&pAACDes);
				if ( (ulRet == ERR_OK) && (pAACDes != NULL) )
				{
					/*
					 *	APS Test stream에서 flag = 0, ucAACType = 0으로 들어 오는데 stream은 HE-AAC 라고 한다.
					 *	flag가 없는 경우 그냥 ucStreamType을 따르자. 일단 아래와 같이 처리하고 추후에 더 많은
					 *	stream을 봐서 수정하길 바람.
					 */
					if ( pAACDes->bAACTypeFlag == TRUE )
					{
						pstPmtInfo->stAudioInfo[audioCnt].eAudCodec		= local_si_GetaudCodeFromAACType(pAACDes->ucAACType);
						pstPmtInfo->stAudioInfo[audioCnt].ucChannelNum	= SVC_MP_AAC_ProfileLevelToChannelNum(pAACDes->ucAACType);
					}
				}

				MWC_SI_FreeDescriptor (pAACDes);
				pAACDes = NULL;
				audioCnt++;
				break;

			case ISO_PRIVDATA_1 :
			case ISO_PRIVDATA_2 :
				/*
				 * Audio가 ISO_PRIVDATA_2으로 들어 올 수도 있다. 보통 Dolby가 Private Data로 들어 오는 경우가
				 * 있다. 이 경우에 Audio 인지 Teletext 인지 판단 여부는 TAG_DOLBY_AC3 or TAG_ENHANCED_AC3가
				 * 있으면 AUDIO이고 Subtitle or teletext Descriptor가 있으면 Subtitle or Teletext가 되는 것으로
				 * 판단 할 수 밖에 없다.
				 */

				/* Caption */
				pPrivateDes = NULL;
				ulRet = MWC_SI_GetPmtElementDescriptor( pstPool, usTsUniqId, ulPmtIndex, i, TAG_DATA_COMPONENT, 0, (void **)&pPrivateDes);
				if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
				{
					pstDataComponentDes = (DataComponentDes *)ARIB_DES_AllocDataComponentDes();
					if (pstDataComponentDes != NULL)
					{
						ulRet = ARIB_DES_ParseDataComponentDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstDataComponentDes);
						if (ulRet == ERR_OK)
						{
							if(pstDataComponentDes->usDataComponentId == 0x0008)	/* Caption or Super */
							{
								if ((componentTag >= 0x30) && (componentTag <= 0x37))		/* Caption */
								{
									pstPmtInfo->stCaptionInfo[captionCnt].usPid = stPmtElemInfo.usElemPid;
									pstPmtInfo->stCaptionInfo[captionCnt].ucComponentTag = componentTag;
									captionCnt ++;
								}
								else if ((componentTag >= 0x38) && (componentTag <= 0x3F))/* Super */
								{
									pstPmtInfo->stCaptionInfo[captionCnt].usPid = stPmtElemInfo.usElemPid;
									pstPmtInfo->stCaptionInfo[captionCnt].ucComponentTag = componentTag;
									captionCnt ++;
								}
							}
						}
						ARIB_DES_FreeDataComponentDes((HUINT8 *)pstDataComponentDes);
						pstDataComponentDes = NULL;
					}
				}
				MWC_SI_FreeDescriptor(pPrivateDes);
				pPrivateDes = NULL;

				break;

#if 0
			/* For Broadcasting Markup Language(BML) */
			case ISO_DATA:
				pPrivateDes = NULL;
				ulRet = MWC_SI_GetPmtElementDescriptor( pstPool, usTsUniqId, ulPmtIndex, i, TAG_DATA_COMPONENT, 0, (void **)&pPrivateDes);
				if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
				{
					pstDataComponentDes = (DataComponentDes *)ARIB_DES_AllocDataComponentDes();
					if (pstDataComponentDes != NULL)
					{
						ulRet = ARIB_DES_ParseDataComponentDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstDataComponentDes);
						if (ulRet == ERR_OK)
						{
							if ((pstDataComponentDes->usDataComponentId == 0x0007) ||	/* XML of BS */
								(pstDataComponentDes->usDataComponentId == 0x000B) || 	/* CS110 */
								(pstDataComponentDes->usDataComponentId == 0x000C)) 	/* DT */
							{
								if ((componentTag >= 0x40) && (componentTag <= 0x7F))
								{
									pstPmtInfo->stDataInfo[dataCnt].usPid = stPmtElemInfo.usElemPid;
									pstPmtInfo->stDataInfo[dataCnt].ucComponentTag = componentTag;
									dataCnt ++;
								}
							}
						}
						ARIB_DES_FreeDataComponentDes((HUINT8 *)pstDataComponentDes);
						pstDataComponentDes = NULL;
					}
				}
				MWC_SI_FreeDescriptor(pPrivateDes);
				pPrivateDes = NULL;

				break;
#endif /* #if 0 */
		}
	}

	pstPlayContext->bPmtValid = TRUE;
	pstPmtInfo->usVideoNum  = (HUINT16)videoCnt;
	pstPmtInfo->usAudioNum  = (HUINT16)audioCnt;
	pstPmtInfo->usCaptionNum = (HUINT16)captionCnt;

	HxLOG_Warning("SvcId=0x%x, nVideo=%d, nAudio=%d, nCaption=%d, CpInfo=[%d,%d,%d,%d,%d,%d,%d,%d]\n",
		usSvcId, pstPmtInfo->usVideoNum, pstPmtInfo->usAudioNum, pstPmtInfo->usCaptionNum,
			pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyControlType,
			pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucDigitalRecording,
			pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucAnalogRecording,
			pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyRestrictionMode,
			pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucImageContraintToken,
			pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucEncryptionMode,
			pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucRetentionMode,
			pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucRetentionState);

	return ERR_OK;
}


HERROR SVC_MP_SI_UpdateSitInfo(MW_MediaSiContext_t *pstPlayContext)
{
	HERROR 				ulRet = ERR_OK;
	MwcRawPool			*pstPool = NULL;
	HUINT16 			usTsUniqId = 0;
	HUINT32 			unSecCnt = 0, ulDesCnt = 0, ulDesIdx = 0;
	MW_MEDIA_EventInfo_t	*pstEvtInfo = NULL;

	void				*pPrivateDes = NULL;
	svcCas_PartialTsDes		*pstPartialTsDes = NULL;
	NetworkIdentificationDes *pstNetworkIdentificationDes = NULL;
	PartialTsTimeDes 	*pstPartialTsTimeDesFirst = NULL;
	AribTsInfoDesTransmissionTypeInfo *pstTsInfoDes = NULL;

	svcCas_SvcDes_t			*pstSvcDes = NULL;
	svcCas_ShortEventDes_t		*pstShortEventDes = NULL;
	svcCas_ExtEvtDes_t			*pstExtEvtDes = NULL;
	svcCas_CompDes_t			*pstCompDes = NULL;
	svcCas_ParentDes_t			*pstParentDes = NULL;
	AudioComponentDes	*pstAudioComponentDes = NULL;
	DataContentDes		*pstDataContentDes = NULL;
	svcCas_ContentDes_t		*pstContentDes = NULL;
	PartialTsTimeDes 	*pstPartialTsTimeDesSecond = NULL;
	BroadcasterIdDes	*pstBroadcasterIdDes = NULL;

	svcCas_ExtEvtInfo_t		*pstExtEvtInfo = NULL;
	MW_MEDIA_ExtEvtItem_t *pstItem = NULL;
	HUINT32				ulLength = 0;
	HUINT8 				ucItemNum = 0;
	MW_MEDIA_ExtEvt_t 	*pstMediaExtEvt = NULL;
	MW_MEDIA_ExtEvt_t	*pstMediaExtHead = NULL;

	HUINT32 			ulVideoComponentDesCnt = 0;
	HUINT32 			ulAudioComponentDesCnt = 0;

	HINT32				i = 0;
	HUINT32				len;
	MW_MEDIA_PmtInfo_t	*pstPmtInfo;
	HBOOL				bValidEventInfo = FALSE;

	if (pstPlayContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	pstPmtInfo = &pstPlayContext->stPmtInfo;
	pstEvtInfo = &pstPlayContext->stEvtInfo;
	if (pstEvtInfo == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}
	else
	{
		/* 기존 EventInfo Clear */
		SVC_MP_SI_FreeExtEvt(pstEvtInfo->pstExtEvt);
		HxSTD_MemSet(pstEvtInfo, 0, sizeof(MW_MEDIA_EventInfo_t));
	}

	pstPool = pstPlayContext->pstRawDataPool;
	usTsUniqId = pstPlayContext->usTsUniqId;
	ulRet = MWC_SI_CountSitSection(pstPool, usTsUniqId, &unSecCnt);
	if ((ulRet != ERR_OK) || (unSecCnt == 0))
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Print("MWC_SI_CountSitSection() unSecCnt=%d \n", unSecCnt);
	}

#if 0
{
	SitServiceInfo_t	stSitServiceInfo;

	ulRet = MWC_SI_GetSitServiceInfo(pstPool, usTsUniqId, 0, 0, &stSitServiceInfo);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("MWC_SI_GetSitServiceInfo() error. %d\n", __LINE__);
	}
	else
	{
		HxLOG_Print("MWC_SI_GetSitServiceInfo() stSitServiceInfo.usSvcId=%#x \n", stSitServiceInfo.usSvcId);
	}
}
#endif

	/************************************************************/
	/*               Transmission info loop of SIT              */
	/************************************************************/
	/* 0x63	TAG_PARTIAL_TRANSPORT_STREAM */
	pstPartialTsDes = NULL;
	ulRet = MWC_SI_GetSitTransmissionInfoDescriptor (pstPool, usTsUniqId, 0, TAG_PARTIAL_TRANSPORT_STREAM, 0, (void **)&pstPartialTsDes);
	if ((ulRet == ERR_OK) && (pstPartialTsDes != NULL))
	{
		#if defined(DEBUG_SIT_PARSING)
		ARIB_DES_PrintPartialTSDes((HUINT8 *)pstPartialTsDes);
		#endif
	}
	else
	{
		HxLOG_Warning("No TAG_PARTIAL_TRANSPORT_STREAM\n");
	}
	MWC_SI_FreeDescriptor(pstPartialTsDes);
	pstPartialTsDes = NULL;


	/* 0xC2	TAG_NETWORK_IDENTIFICATION */
	pPrivateDes = NULL;
	ulRet = MWC_SI_GetSitTransmissionInfoDescriptor (pstPool, usTsUniqId, 0, TAG_NETWORK_IDENTIFICATION, 0, (void **)&pPrivateDes);
	if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
	{
		pstNetworkIdentificationDes = (NetworkIdentificationDes*)ARIB_DES_AllocNetworkIdentificationDes();
		if (pstNetworkIdentificationDes != NULL)
		{
			ulRet = ARIB_DES_ParseNetworkIdentificationDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstNetworkIdentificationDes);
			if (ulRet == ERR_OK)
			{
				HxLOG_Print("PB TAG_NETWORK_IDENTIFICATION usNetworkId=%#x\n", pstNetworkIdentificationDes->usNetworkId);

				pstPlayContext->usNetworkId = pstNetworkIdentificationDes->usNetworkId;
				pstPlayContext->usMediaType = pstNetworkIdentificationDes->usMediaType;
#if 0
				/* Set default analog copy protection */
				if ((xdb_svc_GetJapanNetworkType(pstNetworkIdentificationDes->usNetworkId) == eJpNetType_CATV) ||
					(xdb_svc_GetJapanNetworkType(pstNetworkIdentificationDes->usNetworkId) == eJpNetType_JCHITS))
				{
					pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyRestrictionMode = 0;
				}
				else
				{
					/* TR B15 4-182 */
					pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyRestrictionMode = 1;
				}
#endif
				#if defined(DEBUG_SIT_PARSING)
				ARIB_DES_PrintNetworkIdentificationDes((HUINT8 *)pstNetworkIdentificationDes);
				#endif
			}
			ARIB_DES_FreeNetworkIdentificationDes((HUINT8 *)pstNetworkIdentificationDes);
			pstNetworkIdentificationDes = NULL;
		}
	}
	else
	{
		HxLOG_Warning("No TAG_NETWORK_IDENTIFICATION\n");
	}
	MWC_SI_FreeDescriptor(pPrivateDes);
	pPrivateDes = NULL;


	/* 0xC3	TAG_PARTIAL_TRANSPORT_STREAM_TIME */
	pPrivateDes = NULL;
	ulRet = MWC_SI_GetSitTransmissionInfoDescriptor (pstPool, usTsUniqId, 0, TAG_PARTIAL_TRANSPORT_STREAM_TIME, 0, (void **)&pPrivateDes);
	if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
	{
		pstPartialTsTimeDesFirst = (PartialTsTimeDes*)ARIB_DES_AllocPartialTsTimeDes();
		if (pstPartialTsTimeDesFirst != NULL)
		{
			ulRet = ARIB_DES_ParsePartialTsTimeDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstPartialTsTimeDesFirst);
			if (ulRet == ERR_OK)
			{
				/* 녹화 시작 시간을 알 수 있다. by ulJSTTime */
				#if defined(DEBUG_SIT_PARSING)
				ARIB_DES_PrintPartialTsTimeDes((HUINT8 *)pstPartialTsTimeDesFirst);
				#endif
			}
			ARIB_DES_FreePartialTsTimeDes((HUINT8 *)pstPartialTsTimeDesFirst);
			pstPartialTsTimeDesFirst = NULL;
		}
	}
	else
	{
		HxLOG_Warning("No TAG_PARTIAL_TRANSPORT_STREAM_TIME 1\n");
	}
	MWC_SI_FreeDescriptor(pPrivateDes);
	pPrivateDes = NULL;

	/* 0xCD	TAG_TS_INFORMATION */
	pPrivateDes = NULL;
	ulRet = MWC_SI_GetSitTransmissionInfoDescriptor (pstPool, usTsUniqId, 0, TAG_TS_INFORMATION, 0, (void **)&pPrivateDes);
	if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
	{
		pstTsInfoDes = (AribTsInfoDesTransmissionTypeInfo*)ARIB_DES_AllocTsInformationDes();
		if (pstTsInfoDes != NULL)
		{
			ulRet = ARIB_DES_ParseTsInformationDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstTsInfoDes);
			if (ulRet == ERR_OK)
			{
				#if defined(DEBUG_SIT_PARSING)
				ARIB_DES_PrintTsInformationDes((HUINT8 *)pstTsInfoDes);
				#endif
			}
			ARIB_DES_FreeTsInformationDes((HUINT8 *)pstTsInfoDes);
			pstTsInfoDes = NULL;
		}
	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_TS_INFORMATION\n", __FUNCTION__, __LINE__);
	}
	MWC_SI_FreeDescriptor(pPrivateDes);
	pPrivateDes = NULL;

	/************************************************************/
	/*                    Service loop of SIT                   */
	/************************************************************/
	/* 0x48 TAG_SERVICE */
	pstSvcDes = NULL;
	ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_SERVICE, 0, (void **)&pstSvcDes);
	if ((ulRet == ERR_OK) && (pstSvcDes != NULL))
	{
		#if defined(DEBUG_SIT_PARSING)
		ARIB_DES_PrintServiceDes((HUINT8 *)pstSvcDes);
		#endif
	}
	else
	{
		HxLOG_Warning("No TAG_SERVICE\n");
	}
	MWC_SI_FreeDescriptor(pstSvcDes);
	pstSvcDes = NULL;

	/* 0x4D TAG_SHORT_EVENT */
	pstShortEventDes = NULL;
	ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_SHORT_EVENT, 0, (void **)&pstShortEventDes);
	if ((ulRet == ERR_OK) && (pstShortEventDes != NULL))
	{
		bValidEventInfo = TRUE;

		/* 이벤트명, 이벤트 간단 정보 */
		HxSTD_MemCopy(pstEvtInfo->aucEventName, pstShortEventDes->pszEvtName, pstShortEventDes->ucEvtNameSize);
		HxSTD_MemCopy(pstEvtInfo->aucEventText, pstShortEventDes->pszText, pstShortEventDes->ucTextSize);

		HxLOG_Print("PB TAG_SHORT_EVENT aucEventName=%s\n", pstEvtInfo->aucEventName);

		#if defined(DEBUG_SIT_PARSING)
		ARIB_DES_PrintShortEventDes((HUINT8 *)pstShortEventDes);
		#endif
	}
	else
	{
		HxLOG_Warning("No TAG_SHORT_EVENT\n");
	}
	MWC_SI_FreeDescriptor(pstShortEventDes);
	pstShortEventDes = NULL;

	/* 0x4D TAG_EXTENDED_EVENT */
	ulDesCnt = 0;
	pstEvtInfo->pstExtEvt = NULL;
	ulRet = MWC_SI_CountSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_EXTENDED_EVENT, &ulDesCnt);
	if (ulRet == ERR_OK)
	{
		pstExtEvtDes = NULL;
		pstMediaExtHead = NULL;
		for (ulDesIdx = 0; ulDesIdx < ulDesCnt; ulDesIdx++)
		{
			pstExtEvtDes = NULL;
			ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_EXTENDED_EVENT, ulDesIdx, (void **)&pstExtEvtDes);
			if ((ulRet == ERR_OK) && (pstExtEvtDes != NULL))
			{
				if (pstMediaExtHead == NULL)
				{
					pstMediaExtEvt = (MW_MEDIA_ExtEvt_t *)HLIB_STD_MemAlloc(sizeof(MW_MEDIA_ExtEvt_t));
					if (pstMediaExtEvt == NULL)
					{
						MWC_SI_FreeDescriptor(pstExtEvtDes);
						pstExtEvtDes = NULL;
						break;
					}
					HxSTD_MemSet(pstMediaExtEvt, 0, sizeof(MW_MEDIA_ExtEvt_t));

					pstMediaExtHead = pstMediaExtEvt;
				}
				else
				{
					pstMediaExtEvt->pNext = (MW_MEDIA_ExtEvt_t *)HLIB_STD_MemAlloc(sizeof(MW_MEDIA_ExtEvt_t));
					pstMediaExtEvt = pstMediaExtEvt->pNext;
					if (pstMediaExtEvt == NULL)
					{
						MWC_SI_FreeDescriptor(pstExtEvtDes);
						pstExtEvtDes = NULL;
						break;
					}
					HxSTD_MemSet(pstMediaExtEvt, 0, sizeof(MW_MEDIA_ExtEvt_t));
				}

				pstMediaExtEvt->ucDesNum = pstExtEvtDes->ucDesNum;
				pstMediaExtEvt->ucLastDesNum = pstExtEvtDes->ucLastDesNum;
				HxSTD_MemCopy(pstMediaExtEvt->szIso639LangCode,  pstExtEvtDes->szIso639LangCode, 4);

				if (pstExtEvtDes->pszChar != NULL)
				{
					ulLength = HxSTD_StrLen(pstExtEvtDes->pszChar);
					pstMediaExtEvt->pszChar = (HUINT8 *) HLIB_STD_MemAlloc(sizeof(HUINT8) * (ulLength+1));
					if (pstMediaExtEvt->pszChar != NULL)
					{
						HxSTD_MemCopy(pstMediaExtEvt->pszChar, pstExtEvtDes->pszChar, ulLength);
						pstMediaExtEvt->pszChar[ulLength] = '\0';
					}
				}

				pstExtEvtInfo = pstExtEvtDes->pExtEvtInfo;
				ucItemNum = 0;
				while ((pstExtEvtInfo != NULL) && (ucItemNum < MW_MEDIA_EXT_ITEM_MAX ))
				{
					pstItem = &pstMediaExtEvt->astItem[ucItemNum];

					if (pstExtEvtInfo->pszItem != NULL)
					{
						ulLength = HxSTD_StrLen(pstExtEvtInfo->pszItem);
						pstItem->pszItem = (HUINT8 *) HLIB_STD_MemAlloc( sizeof(HUINT8) * (ulLength+1));
						if( pstItem->pszItem != NULL )
						{
							HxSTD_MemCopy(pstItem->pszItem, pstExtEvtInfo->pszItem, ulLength);
							pstItem->pszItem[ulLength] = '\0';
						}
					}

					if (pstExtEvtInfo->pszDescription != NULL)
					{
						ulLength = HxSTD_StrLen( pstExtEvtInfo->pszDescription );
						pstItem->pszDescription = (HUINT8 *) HLIB_STD_MemAlloc(sizeof(HUINT8) * (ulLength+1));
						if (pstItem->pszDescription != NULL)
						{
							HxSTD_MemCopy(pstItem->pszDescription, pstExtEvtInfo->pszDescription, ulLength);
							pstItem->pszDescription[ulLength] = '\0';
						}
					}

					ucItemNum++;
					pstExtEvtInfo = pstExtEvtInfo->pNextInfo;
				}
				pstMediaExtEvt->ucItemNum = ucItemNum;

				#if defined(DEBUG_SIT_PARSING)
				/* 아래 SVC_PVR_Meta_PrintExtEventInfo() 대체 */
				/* ARIB_DES_PrintExtendedEventDes((HUINT8 *)pstExtEvtDes); */
				#endif
				HxLOG_Print("PB TAG_EXTENDED_EVENT Present \n");
			}
			else
			{
				HxLOG_Print("[%s,%d] No TAG_EXTENDED_EVENT\n", __FUNCTION__, __LINE__);
			}
			MWC_SI_FreeDescriptor(pstExtEvtDes);
			pstExtEvtDes = NULL;
		}

		/* Extended Event Info를 합친 후 문자열을 UTF8으로 변환 한다. */
		pstMediaExtHead = local_si_GetMergedExtEvtInfo(pstMediaExtHead);
		pstEvtInfo->pstExtEvt = pstMediaExtHead;
	}

	/* 0x50 TAG_COMPONENT */
	ulDesCnt = 0;
	ulRet = MWC_SI_CountSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_COMPONENT, &ulDesCnt);
	if (ulRet == ERR_OK)
	{
		for (ulDesIdx = 0; ulDesIdx < ulDesCnt; ulDesIdx++)
		{
			pstCompDes = NULL;
			ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_COMPONENT, ulDesIdx, (void **)&pstCompDes);
			if ((ulRet == ERR_OK) && (pstCompDes != NULL))
			{
				/* PMT에서 확인된 Tag 값과 일치하는 것 처리 */
				for (i=0; i<pstPmtInfo->usVideoNum; i++)
				{
					if (pstPmtInfo->stVideoInfo[i].ucComponentTag == pstCompDes->ucCompTag)
					{
						pstPmtInfo->stVideoInfo[i].ucComponentType = pstCompDes->ucCompType;

						len = MWC_UTIL_DvbStrlen(pstCompDes->pszText);
						if (len >= MW_MEDIA_MAX_ES_STRING_LEN)
						{
							len = MW_MEDIA_MAX_ES_STRING_LEN - 1;
						}

						HxSTD_MemCopy(pstPmtInfo->stVideoInfo[i].aucStreamName, pstCompDes->pszText, len);
						pstPmtInfo->stVideoInfo[i].aucStreamName[len] = '\0';

						/* Video가 여러개일 일때에 대한 고려가 되어 있지 않다. */
						if (pstCompDes->ucCompTag <= 0x0F) /* Video */
						{
							if (ulVideoComponentDesCnt == 0)
							{
								pstEvtInfo->stAribControlData.u8VideoResolution = pstCompDes->ucCompType;
								ulVideoComponentDesCnt++;
							}
							else
							{
								pstEvtInfo->stAribControlData.u1MultiVideoComponent = 1;
							}
						}
					}
				}

				#if defined(DEBUG_SIT_PARSING)
				ARIB_DES_PrintComponentDes((HUINT8 *)pstCompDes);
				#endif
			}
			else
			{
				HxLOG_Print("[%s,%d] No TAG_COMPONENT\n", __FUNCTION__, __LINE__);
			}

			MWC_SI_FreeDescriptor(pstCompDes);
			pstCompDes = NULL;
		}
	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_COMPONENT\n", __FUNCTION__, __LINE__);
	}

	/* 0xC4	TAG_AUDIO_COMPONENT */
	ulDesCnt = 0;
	ulRet = MWC_SI_CountSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_AUDIO_COMPONENT, &ulDesCnt);
	if (ulRet == ERR_OK)
	{
		for (ulDesIdx = 0; ulDesIdx < ulDesCnt; ulDesIdx++)
		{
			pPrivateDes = NULL;
			ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_AUDIO_COMPONENT, ulDesIdx, (void **)&pPrivateDes);
			if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
			{
				pstAudioComponentDes = (AudioComponentDes*)ARIB_DES_AllocAudioComponentDes();
				if (pstAudioComponentDes != NULL)
				{
					ulRet = ARIB_DES_ParseAudioComponentDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstAudioComponentDes);
					if (ulRet == ERR_OK)
					{
						for (i=0; i<pstPmtInfo->usAudioNum; i++)
						{
							/* PMT에서 확인된 Tag 값과 일치하는 것 처리 */
							if (pstPmtInfo->stAudioInfo[i].ucComponentTag == pstAudioComponentDes->ucComponentTag)
							{
								pstPmtInfo->stAudioInfo[i].ucComponentType = pstAudioComponentDes->ucComponentType;
								pstPmtInfo->stAudioInfo[i].ucStreamType = pstAudioComponentDes->ucStreamType;
								len = MWC_UTIL_DvbStrlen(pstAudioComponentDes->pucTextChar);
								if (len >= MW_MEDIA_MAX_LEN_AUD_NAME)
								{
									len = MW_MEDIA_MAX_LEN_AUD_NAME - 1;
								}

								HxSTD_MemCopy(pstPmtInfo->stAudioInfo[i].aucStreamName, pstAudioComponentDes->pucTextChar, len);
								pstPmtInfo->stAudioInfo[i].aucStreamName[len] = '\0';

								if (pstAudioComponentDes->unISO639LanguageCode != 0)
								{
									pstPmtInfo->stAudioInfo[i].stIsoLangInfo[0].szIso639LangCode[0] = (pstAudioComponentDes->unISO639LanguageCode >> 16) & 0xFF;
									pstPmtInfo->stAudioInfo[i].stIsoLangInfo[0].szIso639LangCode[1] = (pstAudioComponentDes->unISO639LanguageCode >> 8) & 0xFF;
									pstPmtInfo->stAudioInfo[i].stIsoLangInfo[0].szIso639LangCode[2] = (pstAudioComponentDes->unISO639LanguageCode) & 0xFF;
									pstPmtInfo->stAudioInfo[i].stIsoLangInfo[0].szIso639LangCode[3] = 0;

									if (MWC_UTIL_IsIso639Language(pstPmtInfo->stAudioInfo[i].stIsoLangInfo[0].szIso639LangCode) == FALSE)
									{
										HxLOG_Warning("Strange ISO639 Language code=[%c%c%c] change to [etc] code \n",
																							(pstAudioComponentDes->unISO639LanguageCode >> 16) & 0xFF,
																					  		(pstAudioComponentDes->unISO639LanguageCode >> 8) & 0xFF,
																					  		(pstAudioComponentDes->unISO639LanguageCode) & 0xFF);
										HxSTD_snprintf(pstPmtInfo->stAudioInfo[i].stIsoLangInfo[0].szIso639LangCode, MW_MEDIA_LANG_ISO_639_LEN+1, "etc");
									}
								}

								if (pstAudioComponentDes->unISO639LanguageCode2 != -1) /* Parser에서 MultiLingual이 아닐 경우 -1로 Return한다.  */
								{
									pstPmtInfo->stAudioInfo[i].stIsoLangInfo[1].szIso639LangCode[0] = (pstAudioComponentDes->unISO639LanguageCode2 >> 16) & 0xFF;
									pstPmtInfo->stAudioInfo[i].stIsoLangInfo[1].szIso639LangCode[1] = (pstAudioComponentDes->unISO639LanguageCode2 >> 8) & 0xFF;
									pstPmtInfo->stAudioInfo[i].stIsoLangInfo[1].szIso639LangCode[2] = (pstAudioComponentDes->unISO639LanguageCode2) & 0xFF;
									pstPmtInfo->stAudioInfo[i].stIsoLangInfo[1].szIso639LangCode[3] = 0;

									if (MWC_UTIL_IsIso639Language(pstPmtInfo->stAudioInfo[i].stIsoLangInfo[1].szIso639LangCode) == FALSE)
									{
										HxLOG_Warning("Strange ISO639 Language code=[%c%c%c] change to [etc] code \n",
																							(pstAudioComponentDes->unISO639LanguageCode2 >> 16) & 0xFF,
																					  		(pstAudioComponentDes->unISO639LanguageCode2 >> 8) & 0xFF,
																					  		(pstAudioComponentDes->unISO639LanguageCode2) & 0xFF);
										HxSTD_snprintf(pstPmtInfo->stAudioInfo[i].stIsoLangInfo[1].szIso639LangCode, MW_MEDIA_LANG_ISO_639_LEN+1, "etc");
									}
								}

								pstPmtInfo->stAudioInfo[i].ucStreamContent = pstAudioComponentDes->ucStreamContent;	/* For JLabs */

								/* Audio가 여러개일 일때에 대한 고려가 되어 있지 않다. */
								if ((pstAudioComponentDes->ucComponentTag >= 0x10) && (pstAudioComponentDes->ucComponentTag <= 0x2F)) /* Audio */
								{
									if (ulAudioComponentDesCnt == 0)
									{
										pstEvtInfo->stAribControlData.u8AudioComponentType = pstAudioComponentDes->ucComponentType;
										pstEvtInfo->stAribControlData.u8AudioStremType = pstAudioComponentDes->ucStreamType;
										ulAudioComponentDesCnt++;
									}
									else
									{
										pstEvtInfo->stAribControlData.u1MultiAudioComponent = 1;
									}
								}

								HxLOG_Print("PB TAG_AUDIO_COMPONENT ucComponentTag=%#x \n",pstAudioComponentDes->ucComponentTag);
								if (MWC_UTIL_DvbStrlen(pstAudioComponentDes->pucTextChar) != 0)
								{
									HxLOG_Print("PB TAG_AUDIO_COMPONENT pucTextChar=%s \n",pstAudioComponentDes->pucTextChar);
								}
							}
						}

						#if defined(DEBUG_SIT_PARSING)
						ARIB_DES_PrintAudioComponentDes((HUINT8 *)pstAudioComponentDes);
						#endif
					}

					ARIB_DES_FreeAudioComponentDes((HUINT8 *)pstAudioComponentDes);
					pstAudioComponentDes = NULL;
				}
			}
			else
			{
				HxLOG_Print("[%s,%d] No TAG_AUDIO_COMPONENT\n", __FUNCTION__, __LINE__);
			}
			MWC_SI_FreeDescriptor(pPrivateDes);
			pPrivateDes = NULL;
		}
	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_AUDIO_COMPONENT\n", __FUNCTION__, __LINE__);
	}

	/* 0xC7	TAG_DATA_CONTENTS */
	ulDesCnt = 0;
	ulRet = MWC_SI_CountSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_DATA_CONTENTS, &ulDesCnt);
	if (ulRet == ERR_OK)
	{
		for (ulDesIdx = 0; ulDesIdx < ulDesCnt; ulDesIdx++)
		{
			pPrivateDes = NULL;
			ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_DATA_CONTENTS, ulDesIdx, (void **)&pPrivateDes);
			if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
			{
				pstDataContentDes = (DataContentDes*)ARIB_DES_AllocDataContentsDes();
				if (pstDataContentDes != NULL)
				{
					ulRet = ARIB_DES_ParseDataContentsDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstDataContentDes);
					if (ulRet == ERR_OK)
					{
						switch (pstDataContentDes->usDataComponentId)
						{
							#if 0
							case 0x0007:
							case 0x000C:
								if ( (pstDataContentDes->ucEntryComponent < 0x40) || (pstDataContentDes->ucEntryComponent > 0x4f) )
								{
									/* 원래 40 ~ 7f 까지인데,, 기존 모델 JC 시리즈가 4f까지만 해서 동일하게 한다. */
									ARIB_DES_FreeDataContentsDes((HUINT8 *)pstDataContentDes);
									pstDataContentDes = NULL;
                                    MWC_SI_FreeDescriptor(pPrivateDes);
									pPrivateDes = NULL;
									continue;
								}

								for (i = 0; i < pstPmtInfo->usDataNum; i++)
								{
									if (pstPmtInfo->stDataInfo[i].ucComponentTag == pstDataContentDes->ucEntryComponent)
									{
										pstEvtInfo->stAribControlData.u1DataEntryComponentExistFlag	= 1;

										/* 이 Flag는 현재 방송과 관런 없이 Data방송이 있다는 것만 의미 한다. */
										if ((pstDataContentDes->pstAribBxmlInfo != NULL) && (pstDataContentDes->ucEntryComponent == 0x40))
										{
											/* 이 Flag는 현재 방송과 관련 있는 Data방송이 있다는 것을 의미 한다. */
											pstEvtInfo->stAribControlData.u1AssociatedContentsFlag = pstDataContentDes->pstAribBxmlInfo->ucAssociatedContentsFlag;
										}

										if (pstDataContentDes->ucTextLen)
										{
											len = MWC_UTIL_DvbStrlen(pstDataContentDes->pucTextChar);
											if (len >= PVR_MAX_ES_STRING_LEN)
											{
												len = PVR_MAX_ES_STRING_LEN - 1;
											}

											HxSTD_MemCopy(pstPmtInfo->stDataInfo[i].aucTextChar, pstDataContentDes->pucTextChar, len);
											pstPmtInfo->stDataInfo[i].aucTextChar[len] = '\0';

											HxLOG_Print("PB TAG_DATA_CONTENTS TextChar=%s \n", pstDataContentDes->pucTextChar);
										}
										HxLOG_Print("PB TAG_DATA_CONTENTS u1AssociatedContentsFlag=%d \n", pstEvtInfo->stAribControlData.u1AssociatedContentsFlag);
									}
								}
								break;
							#endif
							case 0x0008:
								for (i = 0; i < pstPmtInfo->usCaptionNum; i++)
								{
									if (pstPmtInfo->stCaptionInfo[i].ucComponentTag == pstDataContentDes->ucEntryComponent)
									{
										pstEvtInfo->stAribControlData.u1Caption = 1;
										if (pstDataContentDes->pstAribCaptionInfo != NULL)
										{
											if (pstDataContentDes->pstAribCaptionInfo->punISO639LanguageCode != NULL)
											{
												pstPmtInfo->stCaptionInfo[i].unISO639LanguageCode = *pstDataContentDes->pstAribCaptionInfo->punISO639LanguageCode;
											}
										}
									}
								}

								HxLOG_Print("PB TAG_DATA_CONTENTS u1Caption=%d \n", pstEvtInfo->stAribControlData.u1Caption);
								break;

							default:
								break;
						}

						#if defined(DEBUG_SIT_PARSING)
						ARIB_DES_PrintDataContentDes((HUINT8 *)pstDataContentDes);
						#endif
					}

					ARIB_DES_FreeDataContentsDes((HUINT8 *)pstDataContentDes);
					pstDataContentDes = NULL;
				}
			}
			else
			{
				HxLOG_Print("[%s,%d] No TAG_DATA_CONTENTS\n", __FUNCTION__, __LINE__);
			}
			MWC_SI_FreeDescriptor(pPrivateDes);
			pPrivateDes = NULL;
		}
	}

	/* 0x54 TAG_CONTENT */
	pstContentDes = NULL;
	ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_CONTENT, 0, (void **)&pstContentDes);
	if ((ulRet == ERR_OK) && (pstContentDes != NULL))
	{
		pstEvtInfo->ucContentNibble = pstContentDes->pContentInfo->ucContentNibble;
		pstEvtInfo->ucUserNibble = pstContentDes->pContentInfo->ucUserNibble;

		#if defined(DEBUG_SIT_PARSING)
		ARIB_DES_PrintContentDes((HUINT8 *)pstContentDes);
		#endif
	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_CONTENT\n", __FUNCTION__, __LINE__);
	}
	MWC_SI_FreeDescriptor(pstContentDes);
	pstContentDes = NULL;


	/* 0x55 TAG_PARENTAL_RATING */
	pstContentDes = NULL;
	ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_PARENTAL_RATING, 0, (void **)&pstParentDes);
	if ((ulRet == ERR_OK) && (pstParentDes != NULL))
	{
		svcCas_ParentInfo_t	*pstParentalRatingInfo = NULL;

		ulDesCnt= 0;
		pstParentalRatingInfo = pstParentDes->pParentInfo;
		while ((pstParentalRatingInfo != NULL) && (ulDesCnt < MW_MEDIA_PARENTAL_RATING_MAX))
		{
			HxLOG_Warning("TAG_PARENTAL_RATING value=%#x\n",pstParentalRatingInfo->ucRate);
			pstEvtInfo->astParentalRating[ulDesCnt].ucParentalRating = pstParentalRatingInfo->ucRate;
			if (pstParentalRatingInfo->szCountryCode)
			{
				HxSTD_MemCopy(pstEvtInfo->astParentalRating[ulDesCnt].aucIsoCountryCode, pstParentalRatingInfo->szCountryCode, MW_MEDIA_LANG_ISO_639_LEN+1);
			}
			pstParentalRatingInfo = pstParentalRatingInfo->pNextInfo;
			ulDesCnt++;
		}
		pstEvtInfo->ucParentalRatingNum = ulDesCnt;

		#if defined(DEBUG_SIT_PARSING)
		ARIB_DES_PrintParentalRatingDes((HUINT8 *)pstParentDes);
		#endif

	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_PARENTAL_RATING\n", __FUNCTION__, __LINE__);
	}
	MWC_SI_FreeDescriptor(pstParentDes);
	pstParentDes = NULL;

#if 0	/* 이전 Polaris 모델에서 처리하지 않고 있다. */
	/* 0xC1	TAG_DIGITAL_COPY_CONTROL */
	ulDesCnt = 0;
	ulRet = MWC_SI_CountSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_DIGITAL_COPY_CONTROL, &ulDesCnt);
	if ((ulRet == ERR_OK) && (ulDesCnt != 0))
	{
		for (ulDesIdx = 0; ulDesIdx < ulDesCnt; ulDesIdx++)
		{
			pPrivateDes = NULL;
			ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_DIGITAL_COPY_CONTROL, ulDesIdx, (void **)&pPrivateDes);
			if ((ulRet == ERR_OK) && (pPrivateDes =!= NULL))
			{
				DigitalCopyControlDes *pstDigitalCopyControlDes = NULL;

				pstDigitalCopyControlDes = (DigitalCopyControlDes*)ARIB_DES_AllocDigitalCopyControlDes();
				if (pstDigitalCopyControlDes != NULL)
				{
					ulRet = ARIB_DES_ParseDigitalCopyControlDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstDigitalCopyControlDes);
					if (ulRet == ERR_OK)
					{
						pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucDigitalRecording = pstDigitalCopyControlDes->ucDigitalRecordingControlData;
						pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyControlType = pstDigitalCopyControlDes->ucCopyControlType;
						pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucAnalogRecording = pstDigitalCopyControlDes->ucAPSControlData;

						#if defined(DEBUG_SIT_PARSING)
						ARIB_DES_PrintDigitalCopyControlDes((HUINT8 *)pstDigitalCopyControlDes);
						#endif
					}
					ARIB_DES_FreeDigitalCopyControlDes((HUINT8 *)pstDigitalCopyControlDes);
					pstDigitalCopyControlDes = NULL;
				}
			}
			else
			{
				HxLOG_Print("[%s,%d] No TAG_DIGITAL_COPY_CONTROL\n", __FUNCTION__, __LINE__);
			}
			MWC_SI_FreeDescriptor(pPrivateDes);
			pPrivateDes = NULL;
		}
	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_DIGITAL_COPY_CONTROL\n", __FUNCTION__, __LINE__);
	}
#endif

	/* 0xC3	TAG_PARTIAL_TRANSPORT_STREAM_TIME */
	pPrivateDes = NULL;
	ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_PARTIAL_TRANSPORT_STREAM_TIME, 0, (void **)&pPrivateDes);
	if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
	{
		pstPartialTsTimeDesSecond = (PartialTsTimeDes*)ARIB_DES_AllocPartialTsTimeDes();
		if (pstPartialTsTimeDesSecond != NULL)
		{
			ulRet = ARIB_DES_ParsePartialTsTimeDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstPartialTsTimeDesSecond);
			if (ulRet == ERR_OK)
			{
				pstEvtInfo->ulStartTime = pstPartialTsTimeDesSecond->ulStartTime;
				pstEvtInfo->ulDuration = pstPartialTsTimeDesSecond->ulDuration;
#if 0
				{
					HxDATETIME_Date_t	date;
					HxDATETIME_Time_t	time;

					HLIB_DATETIME_ConvertUnixTimeToDate(pstEvtInfo->ulStartTime, &date);
					HLIB_DATETIME_ConvertUnixTimeToTime(pstEvtInfo->ulStartTime, &time);
					HxLOG_Warning("Event StartTime : [%d.%d.%d] Time : [%dh:%dm:%ds]\n\n",(int)date.usYear,(int)date.ucMonth,(int)date.ucDay,
																		(int)time.ucHour,(int)time.ucMinute,(int)time.ucSecond);
				}
#endif
				#if defined(DEBUG_SIT_PARSING)
				ARIB_DES_PrintPartialTsTimeDes((HUINT8 *)pstPartialTsTimeDesSecond);
				#endif
			}
			ARIB_DES_FreePartialTsTimeDes((HUINT8 *)pstPartialTsTimeDesSecond);
			pstPartialTsTimeDesSecond = NULL;
		}
	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_PARTIAL_TRANSPORT_STREAM_TIME 2\n", __FUNCTION__, __LINE__);
	}
	MWC_SI_FreeDescriptor(pPrivateDes);
	pPrivateDes = NULL;

	/* 0x85	TAG_BROADCASTER_ID */
	pPrivateDes = NULL;
	ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_BROADCASTER_ID, 0, (void **)&pPrivateDes);
	if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
	{
		pstBroadcasterIdDes = (BroadcasterIdDes*)ARIB_DES_AllocBroadcasterIdDes();
		if (pstBroadcasterIdDes != NULL)
		{
			ulRet = ARIB_DES_ParseBroadcasterIdDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstBroadcasterIdDes);
			if (ulRet == ERR_OK)
			{
				pstEvtInfo->nEvtId = pstBroadcasterIdDes->nEventId;
				#if defined(DEBUG_SIT_PARSING)
				ARIB_DES_PrintBroadcasterIdDes((HUINT8 *)pstBroadcasterIdDes);
				#endif
			}
			ARIB_DES_FreeBroadcasterIdDes((HUINT8 *)pstBroadcasterIdDes);
			pstBroadcasterIdDes = NULL;
		}
	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_BROADCASTER_ID\n", __FUNCTION__, __LINE__);
	}
	MWC_SI_FreeDescriptor(pPrivateDes);
	pPrivateDes = NULL;

#if 0
	/* 0xDE	TAG_CONTENT_AVAILABILITY */
	ulDesCnt = 0;
	ulRet = MWC_SI_CountSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_CONTENT_AVAILABILITY, &ulDesCnt);
	if ((ulRet == ERR_OK) && (ulDesCnt != 0))
	{
		for (ulDesIdx = 0; ulDesIdx < ulDesCnt; ulDesIdx++)
		{
			pPrivateDes = NULL;
			ulRet = MWC_SI_GetSitServiceDescriptor (pstPool, usTsUniqId, 0, 0, TAG_CONTENT_AVAILABILITY, ulDesIdx, (void **)&pPrivateDes);
			if ((ulRet == ERR_OK) && (pPrivateDes != NULL))
			{
				ContentAvailabilityDes *pstContentAvailabilityDes = NULL;

				pstContentAvailabilityDes = (ContentAvailabilityDes*)ARIB_DES_AllocContentAvailabilityDes();
				if (pstContentAvailabilityDes != NULL)
				{
					ulRet = ARIB_DES_ParseContentAvailabilityDes((HUINT8 *)pPrivateDes, (HUINT8 *)pstContentAvailabilityDes);
					if (ulRet == ERR_OK)
					{
						pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucCopyRestrictionMode = pstContentAvailabilityDes->ucCopyRestrictionMode;
						pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucImageContraintToken = pstContentAvailabilityDes->ucImageContraintToken;
						pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucRetentionMode = pstContentAvailabilityDes->ucRetentionMode;
						pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucRetentionState = pstContentAvailabilityDes->ucRetentionState;
						pstPmtInfo->stDrmInfo.info.stAribCpInfo.ucEncryptionMode = pstContentAvailabilityDes->ucEncryptionMode;

						#if defined(DEBUG_SIT_PARSING)
						ARIB_DES_PrintContentAvailabilityDes((HUINT8 *)pstContentAvailabilityDes);
						#endif
					}
					ARIB_DES_FreeContentAvailabilityDes((HUINT8 *)pstContentAvailabilityDes);
					pstContentAvailabilityDes = NULL;
				}
			}
			else
			{
				HxLOG_Print("[%s,%d] No TAG_CONTENT_AVAILABILITY\n", __FUNCTION__, __LINE__);
			}
			MWC_SI_FreeDescriptor(pPrivateDes);
			pPrivateDes = NULL;
		}
	}
	else
	{
		HxLOG_Print("[%s,%d] No TAG_CONTENT_AVAILABILITY\n", __FUNCTION__, __LINE__);
	}
#endif

    /* 최소한 Event name이 존재할 때 Event값이 Valid라고 설정 한다. */
	pstPlayContext->bEventValid = bValidEventInfo;

    HxLOG_Warning("\n");
	return ERR_OK;
}


