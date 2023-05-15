/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_lib_es.c
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
#include <sapi.h>
#include <octo_common.h>
#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <db_svc.h>
#include <db_param.h>
#include <isosvc.h>

//#include "../../si/local_include/si_service.h"
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_hbbtv.h>


#include <psvc_si.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	svcSi_EsPrint			HxLOG_Print

#define	svcSi_MAX_AUD_CANDIDATE			64

#define	svcSi_EsAudSelGetMatchedList(pstInfo)					((svcSi_EsAudioCandidate_t *)pstInfo)->stMatched.astMatchedList
#define	svcSi_EsAudSelGetMatchedInfo(pstInfo)					&((svcSi_EsAudioCandidate_t *)pstInfo)->stMatched
#define	svcSi_EsAudSelGetEsProperty(pstInfo)					&((svcSi_EsAudioCandidate_t *)pstInfo)->stProperty

#define	svcSi_MAX_SUBTTL_CANDIDATE		64

#define	svcSi_EsSubttlSelGetMatchedList(pstInfo)				((svcSi_EsSubtitleCandidate_t *)pstInfo)->stMatched.astMatchedList
#define	svcSi_EsSubttlSelGetMatchedInfo(pstInfo)				&((svcSi_EsSubtitleCandidate_t *)pstInfo)->stMatched
#define	svcSi_EsSubttlSelGetEsProperty(pstInfo)					&((svcSi_EsSubtitleCandidate_t *)pstInfo)->stProperty

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define	___AUDIO_SELECTION___

typedef struct
{
	HxLANG_Id_e				 eLangId;
	DxDualMonoSelect_e		 eDualMonoSelect;
	svcSi_EsMatch_t	 		astMatchedList[eSvcSi_AUD_SEL_TYPE_NUM];
} svcSi_EsAudioMatchedInfo_t;

typedef struct
{

	HINT32					 nIndex;
	HUINT16					 usPid;
	DxAudioCodec_e			 eCodec;
	HBOOL					 bFirstTrack;
	HBOOL					 bDualMono;
	HBOOL					 bMultiChannel;
	HBOOL					 bAdBroadcasterMixed;
	HBOOL					 bAdReceiverMixed;
	HINT32					 nAlternativePriority;
	HBOOL					 bNormaiAudio; //audio type(ISO 639 language descriptor ) is undefined  || supplementary audio MAIN
} svcSi_EsAudioProperty_t;

typedef struct
{
	// Matched Info
	svcSi_EsAudioMatchedInfo_t	 stMatched;
	// ES Info as Boolean
	svcSi_EsAudioProperty_t		 stProperty;
	// Priority
	HINT32						 nPriority;
	// User Data
	void						*pvUser;
} svcSi_EsAudioCandidate_t;

typedef struct
{
	HxLANG_Id_e			eLangId;
//	HBOOL				bDualMono;
	DxDualMonoSelect_e	eDualMonoSelect;
} svcSi_EsAudioFoundInfo_t;

#define	___SUBTITLE_SELECTION___

typedef enum
{
	eSvcSi_SUBTITLE_HD_NA			= 0,
	eSvcSi_SUBTITLE_HD_YES,
	eSvcSi_SUBTITLE_HD_NO,
	eSvcSi_SUBTITLE_HD_MAX
} svcSi_SubtitleHd_e;

typedef enum
{
	eSvcSi_SUBTITLE_AR_NA			= 0,
	eSvcSi_SUBTITLE_AR_NO,
	eSvcSi_SUBTITLE_AR_4x3,
	eSvcSi_SUBTITLE_AR_16x9,
	eSvcSi_SUBTITLE_AR_221x1,
	eSvcSi_SUBTITLE_AR_MAX
} svcSi_SubtitleAr_e;

typedef struct
{
	HUINT8				ucType;
	HBOOL				bDvb;
	HBOOL				bHoh;
	svcSi_SubtitleHd_e	eHd;
	svcSi_SubtitleAr_e	eAspectRatio;
} svcSvc_SubtitleTypeTable_t;

typedef struct
{
//	HxLANG_Id_e				 eLangId;
//	HUINT8					 ucType;
	svcSi_EsMatch_t			 astMatchedList[eSvcSi_SUBTTL_PRIORITY_TYPE_NUM];
} svcSi_EsSubtitleMatchedInfo_t;

typedef struct
{

	HINT32					 nIndex;
	HBOOL					 bFirstTrack;
	HUINT8					 ucType;	// Reference
	HUINT16					 usPid;
	HxLANG_Id_e				 eLangId;
	HBOOL					 bHoh;
	HBOOL					 bDvb;
	svcSi_SubtitleHd_e		 eHD;
	svcSi_SubtitleAr_e		 eAspecRatio;
} svcSi_EsSubtitleProperty_t;

typedef struct
{
	// Matched Info
	svcSi_EsSubtitleMatchedInfo_t	 stMatched;
	// ES Info as Boolean
	svcSi_EsSubtitleProperty_t		 stProperty;
	// Priority
	HINT32							 nPriority;
	// User Data
	void							*pvUser;
} svcSi_EsSubtitleCandidate_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___
STATIC void svc_si_LibFreeSiEsItem(void *pvData)
{
	if (NULL != pvData)
	{
		OxSI_Free (pvData);
	}
}

STATIC HERROR svc_si_LibUpdateVideoEs(svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedVideo)
{
	DbSvc_SetVideoPid(pstSvcInfo, PID_NULL);
	DbSvc_SetVideoCodec(pstSvcInfo, eDxVIDEO_CODEC_UNKNOWN);

	if (TRUE == pstSelectedVideo->bFound)
	{
		svcSi_VideoEs_t 		*pstVideoEs;

		pstVideoEs = (svcSi_VideoEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem (pstPmtEsInfo->pstVideoTrack, pstSelectedVideo->nEsIdx));
		if (NULL != pstVideoEs)
		{
			DbSvc_SetVideoPid(pstSvcInfo, pstVideoEs->usPid);
			DbSvc_SetVideoCodec(pstSvcInfo, pstVideoEs->eVideoCodec);
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_si_LibUpdateAudioEs(svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedAudio_t *pstSelectedAudio)
{
	DbSvc_SetAudioPid(pstSvcInfo, PID_NULL);
	DbSvc_SetAudioAuxPid(pstSvcInfo, PID_NULL);
	DbSvc_SetDolbyPid(pstSvcInfo, PID_NULL);
	DbSvc_SetAudioCodec(pstSvcInfo, eDxAUDIO_CODEC_UNKNOWN);
	DbSvc_SetAudioAuxCodec(pstSvcInfo, eDxAUDIO_CODEC_UNKNOWN);
	DbSvc_SetDolbyCodec(pstSvcInfo, eDxAUDIO_CODEC_UNKNOWN);


	if (TRUE == pstSelectedAudio->bFound)
	{
		svcSi_AudioEs_t 		*pstAudioEs;

		// MPEG Audio
		if (pstSelectedAudio->nMpegAudioIdx >= 0)
		{
			HINT32	nAudioIdx;


			nAudioIdx = pstSelectedAudio->nMpegAudioIdx;
			pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem (pstPmtEsInfo->pstAudioTrack, nAudioIdx));
			if (NULL != pstAudioEs)
			{
				DbSvc_SetAudioPid(pstSvcInfo, pstAudioEs->usPid);
				DbSvc_SetAudioCodec(pstSvcInfo, pstAudioEs->eAudCodec);

				// mpeg audio pid가 선택되었으므로 dolbyFlag off
				DbSvc_SetDolbyFlag(pstSvcInfo, FALSE);

			}
		}

		// Sound Mode
		DbSvc_SetStereoSelect(pstSvcInfo, pstSelectedAudio->eSoundMode);
		DbSvc_SetDualmonoSelect(pstSvcInfo, pstSelectedAudio->eDualMonoSelect);

		// Dolby
		if (pstSelectedAudio->nDolbyIdx >= 0)
		{
			pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem (pstPmtEsInfo->pstAudioTrack, pstSelectedAudio->nDolbyIdx));
			if (NULL != pstAudioEs)
			{
				DbSvc_SetDolbyPid(pstSvcInfo, pstAudioEs->usPid);
				DbSvc_SetDolbyCodec(pstSvcInfo, pstAudioEs->eAudCodec);

				// dolby pid가 선택되었으므로 dolbyFlag on
				DbSvc_SetDolbyFlag(pstSvcInfo, TRUE);

			}
		}

		// AD
		if (pstSelectedAudio->nAdIdx >= 0)
		{
			pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem (pstPmtEsInfo->pstAudioTrack, pstSelectedAudio->nAdIdx));
			if (NULL != pstAudioEs)
			{
				DbSvc_SetAudioAuxPid(pstSvcInfo, pstAudioEs->usPid);
				DbSvc_SetAudioAuxCodec(pstSvcInfo, pstAudioEs->eAudCodec);

			}
		}
	}


	return ERR_OK;
}

STATIC HERROR svc_si_LibUpdateSubtitleEs(svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedSubttl)
{
	if (TRUE == pstSelectedSubttl->bFound)
	{
		if (pstSelectedSubttl->nEsIdx >= 0)
		{
			DbSvc_SetSubttlIdx(pstSvcInfo, pstSelectedSubttl->nEsIdx);
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_si_LibUpdateTeletextEs(svcSi_PmtEsInfo_t *pstPmtEsInfo, DbSvc_Info_t *pstSvcInfo, svc_si_SelectedEs_t *pstSelectedTtx)
{
	DbSvc_SetTtxPid(pstSvcInfo, PID_NULL);

	if (TRUE == pstSelectedTtx->bFound)
	{
		svcSi_TeletextEs_t 		*pstTtxEs;

		if (pstSelectedTtx->nEsIdx >= 0)
		{
			pstTtxEs= (svcSi_TeletextEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem (pstPmtEsInfo->pstTeltxtTrack, pstSelectedTtx->nEsIdx));
			if (NULL != pstTtxEs)
			{
				DbSvc_SetTtxPid(pstSvcInfo, pstTtxEs->usPid);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_si_LibUpdateVideoInfo(HUINT32 ulActionId, HUINT16 usSvcId, SIxTable_PsiTable_t *pstSdtActTable, svcSi_VideoEs_t *pstVideoEs, SvcSi_VideoInfo_t *pstCurVideoInfo)
{
	if ((pstVideoEs == NULL) || (pstCurVideoInfo == NULL))
	{
		HxLOG_Error("\tInvalid parameter\n");
		return ERR_FAIL;
	}

	/* Get PID */
	pstCurVideoInfo->usPid 					= pstVideoEs->usPid;

	/* Get Component Tag */
	pstCurVideoInfo->ucComponentTag 		= pstVideoEs->ucComponentTag;

	/* Get Codec */
	pstCurVideoInfo->eCodec 				= pstVideoEs->eVideoCodec;

	/* Get HD  */
	pstCurVideoInfo->bIsHd 					= pstVideoEs->bIsHd;

	/* Get CaDesFound */
	pstCurVideoInfo->bCaDesFound			= pstVideoEs->bCaDesFound;

	/* SDT component desc.에서 component type을 가져옴 */
	if (pstSdtActTable != NULL)
	{
		HxList_t			*pstElemItem, *pstDesItem;
		HUINT8				*pucRawDes;

		for (pstElemItem = pstSdtActTable->pst2ndLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstElemItem);

			if (NULL == pst2ndItem)					{ continue; }

			for (pstDesItem = pst2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
			{
				pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstDesItem);

				if (eSIxDESCTAG_COMPONENT == pucRawDes[0])
				{
					SIxDescriptor_t			*pstDes = NULL;
					SIxCompDes_t 		*pstComponentDes = NULL;

					pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					if (NULL != pstDes)
					{
						pstComponentDes = (SIxCompDes_t *)pstDes;
						if (pstComponentDes->ucStmContent == eEsStreamContent_MPEG2_Video)		// Video
						{
							/* Component descriptor 중에서 Tag 값과 일치하는 것에서 얻는다. */
							if (pstComponentDes->ucCompTag == pstCurVideoInfo->ucComponentTag)
							{
								pstCurVideoInfo->ucComponentType = pstComponentDes->ucCompType;
							}
						}
						PAL_SIRAWPOOL_FreeDes (pstDes);
					}
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_si_LibUpdateAudioInfo(HUINT32 ulActionId, HUINT16 usSvcId, SIxTable_PsiTable_t *pstEitActPfTable, svcSi_AudioEs_t *pstAudioEs, SvcSi_AudioInfo_t *pstCurAudioInfo)
{
	HBOOL			bGetEvtId = FALSE;
	HBOOL			bFindComponent = FALSE;

	if ((pstAudioEs == NULL) || (pstCurAudioInfo == NULL))
	{
		HxLOG_Error("\tInvalid parameter\n");
		return ERR_FAIL;
	}

	// temp
	bGetEvtId = TRUE;

	/* Get PID */
	pstCurAudioInfo->usPid 					= pstAudioEs->usPid;

	/* Get Component Tag */
	pstCurAudioInfo->ucComponentTag 		= pstAudioEs->ucComponentTag;

	/* Get Codec */
	pstCurAudioInfo->eCodec 				= pstAudioEs->eAudCodec;

	/* Get Supplementary Audio Type. */
	pstCurAudioInfo->suppAudioEditorial 	= pstAudioEs->eSuppAudioEditorial;

	/* Get MixType. */
	pstCurAudioInfo->ucMixType 	= pstAudioEs->ucMixType;

	/* Clear Stream name buffer */
	HxSTD_MemSet(pstCurAudioInfo->aucStreamName, 0, SvcSi_MAX_AUD_NAME_LEN);

	/* Dolby 여부 확인 */
	if (SvcSi_ES_AUDIO_IS_DOLBY_TYPE(pstAudioEs->eAudCodec))
	{
		pstCurAudioInfo->bIsDolby = TRUE;
	}
	else
	{
		pstCurAudioInfo->bIsDolby = FALSE;
	}

	/* Copy Language Code */
	// MwIso639LangInfo_t과 svcSi_Iso639_t사이의 memcopy이다. 이 둘의 structure내용이 틀려지면 memcpy로 안된다.!!!
	HxSTD_MemCopy(pstCurAudioInfo->stIsoLangInfo, pstAudioEs->astIsoLangInfo, sizeof(SvcSi_Iso639LangInfo_t) * SvcSi_MAX_AUD_NUM);

	/* EIT component desc.에서 component type와 stream name을 가져옴 */
	if ((bGetEvtId == TRUE) && (pstEitActPfTable != NULL))
	{
		HxList_t			*pstElemItem, *pstDesItem;
		HUINT8				*pucRawDes;
		HUINT32				ulStrLen = 0;

		for (pstElemItem = pstEitActPfTable->pst2ndLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstElemItem);

			if (NULL == pst2ndItem)					{ continue; }

			for (pstDesItem = pst2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
			{
				pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstDesItem);

				if (eSIxDESCTAG_COMPONENT == pucRawDes[0])
				{
					SIxDescriptor_t			*pstDes = NULL;
					SIxCompDes_t 		*pstComponentDes = NULL;

					pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
					if (NULL != pstDes)
					{
						pstComponentDes = (SIxCompDes_t *)pstDes;

						// if (pstComponentDes->ucStmContent == eEsStreamContent_MPEG1_Audio)		// Audio
						{
							/* Component descriptor 중에서 Tag 값과 일치하는 것에서 얻는다. */
							if (pstComponentDes->ucCompTag == pstCurAudioInfo->ucComponentTag)
							{
								ulStrLen = HxSTD_StrLen(pstComponentDes->pszText);
								if(ulStrLen >= SvcSi_MAX_AUD_NAME_LEN)
								{
									ulStrLen = SvcSi_MAX_AUD_NAME_LEN - 1;
								}

								HxSTD_MemCopy(pstCurAudioInfo->aucStreamName, pstComponentDes->pszText, ulStrLen);
								pstCurAudioInfo->aucStreamName[ulStrLen] = '\0';

								/* ETSI EN 300 468 - Stream_content and component_type */
								switch (pstComponentDes->ucStmContent)
								{
								case eEsStreamContent_MPEG1_Audio:
									switch (pstComponentDes->ucCompType)
									{
										case 0x01:
											pstCurAudioInfo->eAudCpnt = eSI_AUD_CPNT_SINGLE_MONO;
											bFindComponent = TRUE;
											break;

										case 0x02:
											pstCurAudioInfo->eAudCpnt = eSI_AUD_CPNT_DUAL_MONO;
											bFindComponent = TRUE;
											break;

										case 0x03:
											pstCurAudioInfo->eAudCpnt = eSI_AUD_CPNT_STEREO;
											bFindComponent = TRUE;
											break;

										case 0x04:
											pstCurAudioInfo->eAudCpnt = eSI_AUD_CPNT_MULTILINGUAL;
											bFindComponent = TRUE;
											break;

										case 0x05:
											pstCurAudioInfo->eAudCpnt = eSI_AUD_CPNT_SURROUND;
											bFindComponent = TRUE;
											break;

										default:
											break;
										}
										break;

									case 0x04:
										pstCurAudioInfo->eAudCpnt = eSI_AUD_CPNT_DOLBY;
										bFindComponent = TRUE;
										break;

									default:
										break;
								}
							}
						}

						PAL_SIRAWPOOL_FreeDes (pstDes);
					}
				}
			}
		}
	}

	/* EIT 정보에서 찾지 못했으면 PMT 정보만으로 Auido cpnt 값을 결정한다. */
	if ( bFindComponent == FALSE )
	{
		/* Language Code가 2개이면 Multi Language이다. */
		if ( HxSTD_StrLen(pstCurAudioInfo->stIsoLangInfo[1].szIso639LangCode) != 0 )
		{
			pstCurAudioInfo->eAudCpnt = eSI_AUD_CPNT_MULTILINGUAL;
		}
		else
		{
			SvcSi_AudCpnt_e		eAudCpnt;

			if (psvc_si_GetAudioComponent(ulActionId, &eAudCpnt) != ERR_OK)
			{
				HxLOG_Error("psvc_si_GetAudioComponent() failed..! \n");
				eAudCpnt = eSI_AUD_CPNT_STEREO;
			}

			pstCurAudioInfo->eAudCpnt = eAudCpnt;
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_si_LibUpdateSubtitleInfo(svcSi_SubtitleEs_t *pstSubtitleEs, SvcSi_SubtitleInfo_t *pstCurSubtitleInfo)
{
	if ((pstSubtitleEs == NULL) || (pstCurSubtitleInfo == NULL))
	{
		HxLOG_Error("\tInvalid parameter\n");
		return ERR_FAIL;
	}

	pstCurSubtitleInfo->usPid = pstSubtitleEs->usPid;
	HxLOG_Print("\t[Subtitle PID = (0x%x), TYPE = (0x%x)]\n",(int)pstCurSubtitleInfo->usPid, (int)pstSubtitleEs->ucType);

	switch (pstSubtitleEs->ucType)
	{
		case TELETEXT_SUBTITLE :		/* EBU subtitle */
		case TTX_SBTL_PAGE_HERARING_IMPAIRED :
			pstCurSubtitleInfo->eType = eSI_SUBTITLE_EBU;
			pstCurSubtitleInfo->utArg.stEbuInfo.ucType 			= pstSubtitleEs->ucType;
			pstCurSubtitleInfo->utArg.stEbuInfo.ucMagazineNum		= pstSubtitleEs->ucMagazineNumber;
			pstCurSubtitleInfo->utArg.stEbuInfo.ucPageNum			= pstSubtitleEs->ucPageNumber;
			HxSTD_MemCopy(pstCurSubtitleInfo->utArg.stEbuInfo.aucLangCode, pstSubtitleEs->aucLangCode, 4);

			/* Get Component Tag */
			pstCurSubtitleInfo->utArg.stEbuInfo.ucComponentTag = pstSubtitleEs->ucComponentTag;

			HxLOG_Print("\tSubtitle Type = %s\n", (char *)"EBU");
			HxLOG_Print("\tMagazine num = 0x%x\n", (int)pstCurSubtitleInfo->utArg.stEbuInfo.ucMagazineNum);
			HxLOG_Print("\tPage num = 0x%x\n", (int)pstCurSubtitleInfo->utArg.stEbuInfo.ucPageNum);
			HxLOG_Print("\tLang Code = %s\n\n", (char *)pstCurSubtitleInfo->utArg.stEbuInfo.aucLangCode);
			break;

	    /*
	     *  DVB subtitle의 type 값은 Component descriptor의 Stream_context(0x03) & componet_type Table
	     *  값을 따른다. 여기서 componet_type 0x01, 0x02는 EBU subtitle이고, 0x10~0x13, 0x20~0x23은 DVB로
	     *  되어 있는데 실제 EBU subtitle은 Subtitle descriptor를 통해서 들어 오는 것이 아니라 Teletext
	     *  descriptor에서 type value 0x02로 들어 오는 경우만 있었으므로 위에 0x01, 0x02s는 component descriptor에
	     *  값과 다른 의미다 일단 0x2만 들어온다고 보고 이 경우만 처리하게 한다.
	     *  0x05(Teletext subtitle page for hearing impaired people)에 대한 처리도 추가 한다.
	     */
		/* Normal */
		case 0x10 :
		case 0x11 :
		case 0x12 :
		case 0x13 :
		case 0x14 : 	/* Freesat HD stream에서 여기로 들어 옴. */

		/* For the hard of hearing */
		case 0x20 :
		case 0x21 :
		case 0x22 :
		case 0x23 :
		case 0x24 :
			pstCurSubtitleInfo->eType = eSI_SUBTITLE_DVB;
			pstCurSubtitleInfo->utArg.stDvbInfo.ucType					= pstSubtitleEs->ucType;
			pstCurSubtitleInfo->utArg.stDvbInfo.usCompositionPageId	= pstSubtitleEs->usCompositionPageId;
			pstCurSubtitleInfo->utArg.stDvbInfo.usAncillaryPageId		= pstSubtitleEs->usAncillaryPageId;
			HxSTD_MemCopy(pstCurSubtitleInfo->utArg.stDvbInfo.aucLangCode, pstSubtitleEs->aucLangCode, 4);

			/* Get Component Tag */
			pstCurSubtitleInfo->utArg.stDvbInfo.ucComponentTag = pstSubtitleEs->ucComponentTag;

			HxLOG_Print("\tSubtitle Type = %s\n", (char *)"DVB");
			HxLOG_Print("\tComp Page Id = 0x%x\n", (int)pstCurSubtitleInfo->utArg.stDvbInfo.usCompositionPageId);
			HxLOG_Print("\tAncillary Page Id = 0x%x\n", (int)pstCurSubtitleInfo->utArg.stDvbInfo.usAncillaryPageId);
			HxLOG_Print("\tLang Code = %s\n\n", (char *)pstCurSubtitleInfo->utArg.stDvbInfo.aucLangCode);
			break;

		default :
			/* 확인 요망. */
			HxLOG_Print("Other Type = 0x%x\n", pstSubtitleEs->ucType);
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR svc_si_LibUpdateEcmInfo(svcSi_EcmEs_t *pstEcmEs, SvcSi_EcmInfo_t *pstCurEcmInfo)
{
	if ((pstEcmEs == NULL) || (pstCurEcmInfo == NULL))
	{
		HxLOG_Error("\tInvalid parameter\n");
		return ERR_FAIL;
	}

	pstCurEcmInfo->usPid		=	pstEcmEs->usPid;
	pstCurEcmInfo->usCaSysId	=	pstEcmEs->usCaSysId;

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___
#define _____ES_RELATED_____
HERROR svc_si_LibGetPmtPidFromPat (HUINT32 ulActionId, HUINT16 usSvcId, HUINT16 *pusPid)
{
	HxList_t			*pstListItem;
	SIxTable_PsiTable_t	*pstTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
	HERROR				 hResult = ERR_FAIL;

	if (NULL == pstTable)
	{
		HxLOG_Error("PAL_SIRAWPOOL_ParseTable ulActionId=%d, usSvcId(0x%x) PAT\n", ulActionId, usSvcId);
		return ERR_FAIL;
	}

	for (pstListItem = pstTable->pst2ndLoop; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		SIxTable_2ndLoop_t			*pst2ndItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data (pstListItem);

		if (NULL != pst2ndItem)
		{
			SIxTable_PatProgram_t	*pstProgram = &(pst2ndItem->unInfo.stPat);

			if (pstProgram->usProgNum == usSvcId)
			{
				if (NULL != pusPid)					{ *pusPid = pstProgram->usProgMapPid; }
				hResult = ERR_OK;

				goto END_FUNC;
			}
		}
	}

END_FUNC:
	PAL_SIRAWPOOL_FreeTable (pstTable);
	return hResult;
}

HERROR svc_si_LibGetAllAitPidFromPmt (HUINT32 ulActionId, HxList_t **ppAitPidList)
{
	SIxTable_PsiTable_t	*pstPmtTable = NULL;
	HxList_t			*pstPmt2ndLoop = NULL;
	HxList_t			*pstAitList = NULL;
	HERROR				 hResult = ERR_FAIL;

	if (ppAitPidList == NULL)
	{
		HxLOG_Error ("Invalid parameter !! \n");
		return ERR_FAIL;
	}

	*ppAitPidList = NULL;
	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", ulActionId);
		return ERR_FAIL;
	}

	for (pstPmt2ndLoop = pstPmtTable->pst2ndLoop; NULL != pstPmt2ndLoop; pstPmt2ndLoop = pstPmt2ndLoop->next)
	{
		SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmt2ndLoop);

		if (NULL == pst2ndItem) 				{ continue; }

		if (pst2ndItem->unInfo.stPmt.ucStreamType == ISO_PRIVDATA_1)
		{
			HxList_t				*pstPmtDesLoop;

			for (pstPmtDesLoop = pst2ndItem->pstDesLoop; NULL != pstPmtDesLoop; pstPmtDesLoop = pstPmtDesLoop->next)
			{
				HUINT8					*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstPmtDesLoop);
				SIxAppSignallingDes_t	*pstDesc = NULL;

				if (eSIxDESCTAG_APPLICATION_SIGNALLING == *pucRawDes)
				{
					pstDesc = (SIxAppSignallingDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
					if (pstDesc != NULL)
					{
						if(pstDesc->pstInfo != NULL)
						{
							HxLOG_Print("[%s:%d] AIT appType(0x%X) aitVersion(0x%X) \n", __FUNCTION__, __LINE__, pstDesc->pstInfo->usAppType, pstDesc->pstInfo->ucAitVersion);

							if (pstDesc->pstInfo->usAppType == SVCSI_HBBTV_APPLICATION_TYPE) /* we need only 0x10 type of AIT */
							{
								HUINT32 *pulAitPid = (HUINT32 *)OxSI_Calloc (sizeof(HUINT32));

								*pulAitPid = pst2ndItem->unInfo.stPmt.usElemPid;
								pstAitList = HLIB_LIST_Append (pstAitList, (void *)pulAitPid);
							}

						}
						else /* in the PMT, it has the application_signalling_descriptor but the length is 0 */
						{
							HUINT32 *pulAitPid = (HUINT32 *)OxSI_Calloc (sizeof(HUINT32));

							*pulAitPid = pst2ndItem->unInfo.stPmt.usElemPid;
							pstAitList = HLIB_LIST_Append (pstAitList, (void *)pulAitPid);
						}

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);
					}
					else /* in the PMT, it has the application_signalling_descriptor but the length is 0 */
					{
						HUINT32 *pulAitPid = (HUINT32 *)OxSI_Calloc (sizeof(HUINT32));

						*pulAitPid = pst2ndItem->unInfo.stPmt.usElemPid;
						pstAitList = HLIB_LIST_Append (pstAitList, (void *)pulAitPid);
					}
				}
			}
		}
	}

	*ppAitPidList = pstAitList;
	hResult = ERR_OK;

	PAL_SIRAWPOOL_FreeTable (pstPmtTable);

	return hResult;
}

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)

HERROR svc_si_LibGetEsInfo(SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, HINT32 nIndex, HUINT16 *pusPid, HUINT32 *pulCodec, HUINT8 *pucComponentTag, HUINT32 *pulParam)
{
	HxList_t		*pstEsList = (HxList_t *)hEsInfoList;
	HUINT16		 usPid = PID_NULL;
	HUINT8		 ucComponentTag = 0;
	HUINT32		 ulCodec = 0;
	HUINT32		 ulParam = 0;
	HERROR		 hErr = ERR_FAIL;

	if (NULL == pstEsList)
		return ERR_FAIL;

	if (0 > nIndex)
		return ERR_FAIL;

	if (NULL == pusPid)
		return ERR_FAIL;


	switch (eType)
	{
	case eES_UPDATE_Video:
		{
			svcSi_VideoEs_t	*pstEsInfo = (svcSi_VideoEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem(pstEsList, nIndex));
			if (pstEsInfo)
			{
				usPid = pstEsInfo->usPid;
				ucComponentTag = pstEsInfo->ucComponentTag;
				ulCodec = pstEsInfo->eVideoCodec;
				hErr = ERR_OK;
			}
		}
		break;

	case eES_UPDATE_Audio:
		{
			svcSi_AudioEs_t	*pstEsInfo = (svcSi_AudioEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem(pstEsList, nIndex));
			if (pstEsInfo)
			{
				usPid = pstEsInfo->usPid;
				ucComponentTag = pstEsInfo->ucComponentTag;
				ulCodec = pstEsInfo->eAudCodec;
				hErr = ERR_OK;
			}
		}
		break;

	case eES_UPDATE_Subtitle:
		{
			svcSi_SubtitleEs_t *pstEsInfo = (svcSi_SubtitleEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem(pstEsList, nIndex));
			if (pstEsInfo)
			{
				usPid = pstEsInfo->usPid;
				ucComponentTag = pstEsInfo->ucComponentTag;
				hErr = ERR_OK;
			}
		}
		break;

	case eES_UPDATE_Teletext:
		{
			svcSi_TeletextEs_t *pstEsInfo = (svcSi_TeletextEs_t *)HLIB_LIST_Data (HLIB_LIST_GetListItem(pstEsList, nIndex));
			if (pstEsInfo)
			{
				usPid = pstEsInfo->usPid;
				ucComponentTag = pstEsInfo->ucComponentTag;
				hErr = ERR_OK;
			}
		}
		break;

	default:
		break;
	}

	if (ERR_OK == hErr)
	{
		*pusPid = usPid;
		if (pucComponentTag)	*pucComponentTag = ucComponentTag;
		if (pulCodec)			*pulCodec = ulCodec;
		if (pulParam)			*pulParam = ulParam;
	}

	return hErr;
}

STATIC void svc_si_LibConvertPmtEsInfo(svcSi_PmtEsInfo_t *pstSource, SvcSi_PmtEsInfo_t *pstDest)
{
	pstDest->usServiceId	= pstSource->usServiceId;
	pstDest->usPcrPid		= pstSource->usPcrPid;

	pstDest->ulPmtCRC		= pstSource->ulPmtCRC;
	pstDest->ulCasType		= pstSource->ulCasType;

	pstDest->hVideoList		= (Handle_t)pstSource->pstVideoTrack;
	pstDest->hAudioList		= (Handle_t)pstSource->pstAudioTrack;
	pstDest->hSubttlList	= (Handle_t)pstSource->pstSubttlTrack;
	pstDest->hTeltxtList	= (Handle_t)pstSource->pstTeltxtTrack;
}

STATIC void svc_si_LibUpdatePmtEsInfoByEit(HUINT32 ulActionId, SIxTable_PsiTable_t *pstEitActPfTable, svcSi_PmtEsInfo_t *pstPmtEsInfo)
{
	HxList_t		*pstAudioTrack = (HxList_t*)pstPmtEsInfo->pstAudioTrack;
	HUINT32		ulAudioTrackNum = 0;//, ulIndex;

	if(pstAudioTrack == NULL)		{ return;}

	ulAudioTrackNum = HLIB_LIST_Length(pstAudioTrack);

	if(ulAudioTrackNum==0)			{return;}

	HxLOG_Debug("pstEitActPfTable (0x%x) \n", pstEitActPfTable);

	if (pstEitActPfTable != NULL)
	{
		HxList_t		*pstElemItem, *pstDesItem;
		HUINT8		*pucRawDes;
		//HUINT32 		ulStrLen = 0;
		svcSi_AudioEs_t	*pstAudioEs = NULL;

		while (pstAudioTrack != NULL)
		{
			pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data(pstAudioTrack) ;

			for (pstElemItem = pstEitActPfTable->pst2ndLoop; NULL != pstElemItem; pstElemItem = pstElemItem->next)
			{
				SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstElemItem);

				if (NULL == pst2ndItem) 				{ continue; }

				for (pstDesItem = pst2ndItem->pstDesLoop; NULL != pstDesItem; pstDesItem = pstDesItem->next)
				{
					pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstDesItem);

					if (eSIxDESCTAG_COMPONENT == pucRawDes[0])
					{
						//SIxDescriptor_t		*pstDes = NULL;
						SIxCompDes_t 		*pstComponentDes = NULL;

						pstComponentDes = (SIxCompDes_t*)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);
						if (NULL != pstComponentDes)
						{
							 if (pstComponentDes->ucStmContent == eEsStreamContent_MPEG1_Audio)		// Audio
							{
								/* Component descriptor 중에서 Tag 값과 일치하는 것에서 얻는다. */
								if (pstComponentDes->ucCompTag == pstAudioEs->ucComponentTag)
								{
									pstAudioEs->eAudCompType = pstComponentDes->ucCompType;
								}

								HxLOG_Debug("ucCompTag (0x%x : 0x%x) eAudCompType(%d)\n", pstComponentDes->ucCompTag , pstAudioEs->ucComponentTag, pstAudioEs->eAudCompType);
							}

							 PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstComponentDes);
						}
					}
				}
			}
			pstAudioTrack = pstAudioTrack->next;
		}
	}
}


HERROR svc_si_LibCheckDefaultPidUpdated2(HUINT32 ulActionId, SIxTable_PsiTable_t *pstPmtTable, SIxTable_PsiTable_t *pstEitTable, const SvcSi_UpdateEs_t *pstUpdate, HBOOL *pbUpdated)
{
	HBOOL					 bUpdated = FALSE;
	svcSi_PmtEsInfo_t		*pstPmtEsInfo = (svcSi_PmtEsInfo_t *)OxSI_Calloc (sizeof(svcSi_PmtEsInfo_t));
	svc_si_SelectedEs_t	 	 stVideoSelected;
	SvcSi_SelectedEs_t		 stSelected;
	//svc_si_SelectedEs_t		 stSbtlSelected;
	svc_si_SelectedEs_t		 stTeltxtSelected;
	HERROR					 hErr, hResult = ERR_FAIL;
	SvcSi_PmtEsInfo_t 		 stPmtEsInfo = { 0, };

	if (NULL == pstPmtTable)
	{
		goto END_FUNC;
	}

	hErr = psvc_si_GetPmtEsInfoFromPmtSiTable (ulActionId, pstPmtTable, pstPmtEsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("psvc_si_GetPmtEsInfoFromPmtSiTable err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	svc_si_LibUpdatePmtEsInfoByEit(ulActionId, pstEitTable, pstPmtEsInfo);

	svc_si_LibConvertPmtEsInfo(pstPmtEsInfo, &stPmtEsInfo);

	(void)(pstUpdate->fnUpdateSvc)((const SvcSi_PmtEsInfo_t *)&stPmtEsInfo, pstUpdate->pvDstInfo);

	// VIDEO
	hErr = psvc_si_SelectVideoEs (ulActionId, pstPmtEsInfo, pstUpdate->pvOrgInfo, &stVideoSelected);
	if (ERR_OK == hErr)
	{

		HxSTD_MemSet(&stSelected, 0x00, sizeof(SvcSi_SelectedEs_t));
		stSelected.bFound			= stVideoSelected.bFound;
		stSelected.nIndexMain		= stVideoSelected.nEsIdx;
		(void)(pstUpdate->fnUpdateEs)(eES_UPDATE_Video, stPmtEsInfo.hVideoList, pstUpdate->pvDstInfo, &stSelected);
	}

	// AUDIO
 	hErr = svc_si_LibSvcEsSelectAudio ((const svcSi_PmtEsInfo_t *)pstPmtEsInfo, svc_si_GetSysConfig((SvcSi_UserSettings_t *)&pstUpdate->stUserSettings), &stSelected);
	if (ERR_OK == hErr)
	{

		(void)(pstUpdate->fnUpdateEs)(eES_UPDATE_Audio, stPmtEsInfo.hAudioList, pstUpdate->pvDstInfo, &stSelected);
	}

	// SUBTITLE
	hErr = svc_si_LibSvcEsSelectSubtitle ((const svcSi_PmtEsInfo_t *)pstPmtEsInfo, svc_si_GetSysConfig((SvcSi_UserSettings_t *)&pstUpdate->stUserSettings), &stSelected);
	if (ERR_OK == hErr)
	{
		(void)(pstUpdate->fnUpdateEs)(eES_UPDATE_Subtitle, stPmtEsInfo.hSubttlList, pstUpdate->pvDstInfo, &stSelected);
	}

	// TELETEXT
	hErr = psvc_si_SelectTeletextEs (ulActionId, pstPmtEsInfo, pstUpdate->pvOrgInfo, &stTeltxtSelected);
	if (ERR_OK == hErr)
	{
		HxSTD_MemSet(&stSelected, 0x00, sizeof(SvcSi_SelectedEs_t));
		stSelected.bFound			= stTeltxtSelected.bFound;
		stSelected.nIndexMain		= stTeltxtSelected.nEsIdx;
		(void)(pstUpdate->fnUpdateEs)(eES_UPDATE_Teletext, stPmtEsInfo.hTeltxtList, pstUpdate->pvDstInfo, &stSelected);
	}

	// Check Updated
	if (NULL != pbUpdated)
	{
		bUpdated = (pstUpdate->fnCompareSvc)(pstUpdate->pvDstInfo, pstUpdate->pvOrgInfo, pstUpdate->ulSize);
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pbUpdated)						{ *pbUpdated = bUpdated; }
	if (NULL != pstPmtEsInfo)					{ svc_si_LibFreePmtEsInfo (pstPmtEsInfo); }
	return hResult;
}

HERROR svc_si_LibGetServiceInfoPid2(HUINT32 ulActionId, HUINT32 ulSvcId, const SvcSi_UpdateEs_t *pstUpdate, HBOOL *pbChanged)
{
	SIxTable_PsiTable_t		*pstPmtTable = NULL;
	SIxTable_PsiTable_t		*pstEitTable = NULL;
	HERROR				hResult;

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, ulSvcId);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, ulSvcId);
		return ERR_FAIL;
	}

	pstEitTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_EitActPf, ulSvcId);
	if (NULL == pstEitTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, ulSvcId);
	}

	hResult = svc_si_LibCheckDefaultPidUpdated2(ulActionId, pstPmtTable, pstEitTable, pstUpdate, pbChanged);
	if (ERR_OK != hResult)
	{
		HxLOG_Error ("svc_si_LibCheckDefaultPidUpdated2 Error:\n");
	}

	PAL_SIRAWPOOL_FreeTable (pstPmtTable);
	PAL_SIRAWPOOL_FreeTable (pstEitTable);
	return hResult;
}
#endif

HERROR svc_si_LibCheckDefaultPidUpdated(HUINT32 ulActionId, SIxTable_PsiTable_t *pstPmtTable, DbSvc_Info_t *pstOrgSvcInfo, DbSvc_Info_t *pstNewSvcInfo, const SvcSi_Config_t *pstConfig, HBOOL *pbUpdated)
{
	HBOOL					 bUpdated = FALSE;
	svcSi_PmtEsInfo_t		*pstPmtEsInfo = (svcSi_PmtEsInfo_t *)OxSI_Calloc (sizeof(svcSi_PmtEsInfo_t));
	svc_si_SelectedEs_t	 	 stVideoSelected;
	svc_si_SelectedAudio_t	 stAudioSelected;
	svc_si_SelectedEs_t		 stSbtlSelected;
	svc_si_SelectedEs_t		 stTeltxtSelected;
	HERROR					 hErr, hResult = ERR_FAIL;

	if ((NULL == pstPmtTable) || (NULL == pstOrgSvcInfo) || (NULL == pstNewSvcInfo) || (NULL == pstConfig))
	{
		goto END_FUNC;
	}

	hErr = psvc_si_GetPmtEsInfoFromPmtSiTable (ulActionId, pstPmtTable, pstPmtEsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("psvc_si_GetPmtEsInfoFromPmtSiTable err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	// new와 org가 동일하게 내려오는 경우엔 memcpy하면 안된다..!
	if (pstNewSvcInfo != pstOrgSvcInfo)
	{
		HxSTD_MemCopy (pstNewSvcInfo, pstOrgSvcInfo, sizeof(DbSvc_Info_t));
	}

	// PCR
	DbSvc_SetPcrPid(pstNewSvcInfo, pstPmtEsInfo->usPcrPid);

	// VIDEO
	hErr = psvc_si_SelectVideoEs (ulActionId, pstPmtEsInfo, pstOrgSvcInfo, &stVideoSelected);
	if (ERR_OK == hErr)
	{
		(void)svc_si_LibUpdateVideoEs (pstPmtEsInfo, pstNewSvcInfo, &stVideoSelected);
	}

	// AUDIO
	hErr = psvc_si_SelectAudioEs (ulActionId, pstPmtEsInfo, pstOrgSvcInfo, &stAudioSelected);
	if (ERR_OK == hErr)
	{
		(void)svc_si_LibUpdateAudioEs (pstPmtEsInfo, pstNewSvcInfo, &stAudioSelected);
	}

	// SUBTITLE
	hErr = psvc_si_SelectSubtitleEs (ulActionId, pstPmtEsInfo, pstOrgSvcInfo, &stSbtlSelected);
	if (ERR_OK == hErr)
	{
		(void)svc_si_LibUpdateSubtitleEs (pstPmtEsInfo, pstNewSvcInfo, &stSbtlSelected);
	}

	// TELETEXT
	hErr = psvc_si_SelectTeletextEs (ulActionId, pstPmtEsInfo, pstOrgSvcInfo, &stTeltxtSelected);
	if (ERR_OK == hErr)
	{
		(void)svc_si_LibUpdateTeletextEs (pstPmtEsInfo, pstNewSvcInfo, &stTeltxtSelected);
	}

	// CAS type - PID는 아니지만 이것도 넣어줘야 cas type이 변경됐을 경우 처리할 수 있다
	DbSvc_SetCasType(pstNewSvcInfo, (pstPmtEsInfo->ulCasType > eDxCAS_TYPE_FTA) ? (DxCasType_e)pstPmtEsInfo->ulCasType : eDxCAS_TYPE_FTA);

	if (NULL != pbUpdated)
	{
		bUpdated = (HxSTD_MemCmp (pstNewSvcInfo, pstOrgSvcInfo, sizeof(DbSvc_Info_t)) == 0) ? FALSE : TRUE;
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pbUpdated)						{ *pbUpdated = bUpdated; }
	if (NULL != pstPmtEsInfo)					{ svc_si_LibFreePmtEsInfo (pstPmtEsInfo); }
	return hResult;
}

HERROR svc_si_LibGetServiceInfoPid(HUINT32 ulActionId, HUINT32 ulSvcId, DbSvc_Info_t *pstOrgSvcInfo, DbSvc_Info_t *pstDstSvcInfo, const SvcSi_Config_t *pstConfig, HBOOL *pbChanged)
{
	SIxTable_PsiTable_t		*pstPmtTable;
	HERROR					 hResult;

	if ((NULL == pstOrgSvcInfo) || (NULL == pstDstSvcInfo))
	{
		HxLOG_Error ("Error:\n");
		return ERR_FAIL;
	}

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, ulSvcId);
	if (NULL == pstPmtTable)
	{
		HxLOG_Warning("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, ulSvcId);
		return ERR_FAIL;
	}

	hResult = svc_si_LibCheckDefaultPidUpdated(ulActionId, pstPmtTable, pstOrgSvcInfo, pstDstSvcInfo, pstConfig, pbChanged);
	if (ERR_OK != hResult)
	{
		HxLOG_Error ("Error:\n");
	}

	PAL_SIRAWPOOL_FreeTable (pstPmtTable);
	return hResult;
}

HERROR svc_si_LibUpdateAllService_inSDT(HUINT32 ulActionId, const DbSvc_Info_t *pstSvcInfo, HBOOL *pbChanged)
{
	HERROR					 hErr = ERR_FAIL;
	HERROR					 hResult = ERR_FAIL;
	SIxTable_PsiTable_t 	*pstSdtTable = NULL;
	HxList_t				*pst2ndLoop = NULL;
	DxSvcType_e 			 eSvcType;
	DxVideoType_e			 eVideoType;
	HUINT32					 ulSvcCnt;
	DbSvc_Info_t			*pstOrgSvcInfo = NULL;
	DbSvc_Info_t			*pstDstSvcInfo = NULL;
	Handle_t				*phDbSvcList = NULL;
	DbSvc_Info_t			*pstDbSvcArray = NULL;
	DbSvc_Condition_t	 	 stSvcCond;
	HUINT32 			 	 ulSvcNum = 0;

	if(pbChanged == NULL)
	{
		goto END_FUNC;
	}

	*pbChanged = FALSE;

	DB_SVC_InitSvcFindCondition(&stSvcCond);
	stSvcCond.nOnId = DbSvc_GetOnId(pstSvcInfo);
	stSvcCond.nTsId= DbSvc_GetTsId(pstSvcInfo);

	hErr = DB_SVC_FindSvcInfoList (&stSvcCond, (HINT32*)&ulSvcNum, &pstDbSvcArray);
	hErr |= DB_SVC_FindSvcHandleList (&stSvcCond, (HINT32*)&ulSvcNum, &phDbSvcList);

	if ((NULL == phDbSvcList) || (NULL == pstDbSvcArray) || (ulSvcNum == 0) || (hErr != ERR_OK))
	{
		goto END_FUNC;
	}

	pstSdtTable = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	pstOrgSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));
	pstDstSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));

	if ((pstOrgSvcInfo == NULL) || (pstDstSvcInfo == NULL))
	{
		HxLOG_Error("mem alloc failed.. \n");
		goto END_FUNC;
	}

	for (pst2ndLoop = pstSdtTable->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
	{
		SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data(pst2ndLoop);
		HxList_t			*pstSvcDesLoop = NULL;

		if (pstSdtSvcItem == NULL)									{ continue; }

		for(ulSvcCnt=0; ulSvcCnt<ulSvcNum; ulSvcCnt++)
		{
			HxSTD_MemCopy(pstOrgSvcInfo, &pstDbSvcArray[ulSvcCnt], sizeof(DbSvc_Info_t));
			HxSTD_MemCopy(pstDstSvcInfo, &pstDbSvcArray[ulSvcCnt], sizeof(DbSvc_Info_t));

			if(pstSdtSvcItem->unInfo.stSdt.usSvcId != DbSvc_GetSvcId(pstOrgSvcInfo))
			{
				continue;
			}

			for (pstSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSvcDesLoop; pstSvcDesLoop = pstSvcDesLoop->next)
			{
				HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSvcDesLoop);

				if (eSIxDESCTAG_SERVICE == *pucRawDes)
				{
					SIxSvcDes_t *pstSvcDes = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

					if (NULL != pstSvcDes)
					{
						// onid, ts id
						DbSvc_SetOnId(pstDstSvcInfo, pstSdtTable->unInfo.stSdt.usOrgNetId);
						DbSvc_SetTsId(pstDstSvcInfo, pstSdtTable->unInfo.stSdt.usTsId);

						// service name
						if (DbSvc_GetRenamed(pstDstSvcInfo) == FALSE)
						{
							svc_si_LibCheckServiceNameBySvcDes (pstSvcDes->ucSvcName, pstSvcDes->pszServName, DbSvc_GetName(pstDstSvcInfo), NULL);
						}

						// Service type, Video Type
						hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, pstSvcDes->ucServType, &eSvcType, &eVideoType);
						if (ERR_OK != hErr)
						{
							HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: SvcType(%d)\n", pstSvcDes->ucServType);
						}

						DbSvc_SetSvcType(pstDstSvcInfo, eSvcType);
						DbSvc_SetVideoType(pstDstSvcInfo, eVideoType);

						// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
						DbSvc_SetOrgSvcType(pstDstSvcInfo, pstSvcDes->ucServType); //??

						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcDes);
					}

					break;
				}
			}

			if(HxSTD_MemCmp(pstDstSvcInfo, pstOrgSvcInfo, sizeof(DbSvc_Info_t)) == 0)
			{
				//No Changed
			}
			else
			{
				hErr = DB_SVC_UpdateServiceInfo (phDbSvcList[ulSvcCnt], pstDstSvcInfo);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("Error - DB_SVC_UpdateServiceInfo \n");
				}

				*pbChanged = TRUE;
			}

			// 원하는 SVC 정보를 획득하고 Update 했다.
			break;
		}
	}

	hResult = ERR_OK;

END_FUNC:

	if(pstSdtTable != NULL)			{ PAL_SIRAWPOOL_FreeTable(pstSdtTable); }
	if(phDbSvcList != NULL)			{ DB_SVC_FreeServiceInfoList(NULL, phDbSvcList, NULL); }
	if(pstDbSvcArray != NULL)		{ DB_SVC_FreeServiceInfoList(NULL, NULL, pstDbSvcArray); }
	if(pstOrgSvcInfo != NULL)		{ OxMGR_Free(pstOrgSvcInfo); }
	if(pstDstSvcInfo != NULL)		{ OxMGR_Free(pstDstSvcInfo); }

	return hResult;
}

HERROR svc_si_LibGetServiceInfo(HUINT32 ulActionId, HUINT32 ulSvcId, DbSvc_Info_t *pstOrgSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged)
{
	HERROR					hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstSdtTable = NULL;
	HxList_t				*pst2ndLoop = NULL;
	DxSvcType_e			eSvcType;
	DxVideoType_e		eVideoType;

	if ((NULL == pstOrgSvcInfo) || (NULL == pstDstSvcInfo))
	{
		HxLOG_Error ("Error:\n");
		return ERR_FAIL;
	}

	if (pstDstSvcInfo != pstOrgSvcInfo)
	{
		HxSTD_MemCopy (pstDstSvcInfo, pstOrgSvcInfo, sizeof(DbSvc_Info_t));
	}

	pstSdtTable = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstSdtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, ulSvcId);
		return ERR_FAIL;
	}

	for (pst2ndLoop = pstSdtTable->pst2ndLoop; pst2ndLoop != NULL; pst2ndLoop = pst2ndLoop->next)
	{
		SIxTable_2ndLoop_t	*pstSdtSvcItem = (SIxTable_2ndLoop_t *)HLIB_LIST_Data(pst2ndLoop);
		HxList_t			*pstSvcDesLoop = NULL;

		if (pstSdtSvcItem == NULL)								{ continue; }
		if (pstSdtSvcItem->unInfo.stSdt.usSvcId != ulSvcId)		{ continue; }

		for (pstSvcDesLoop = pstSdtSvcItem->pstDesLoop; NULL != pstSvcDesLoop; pstSvcDesLoop = pstSvcDesLoop->next)
		{
			HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstSvcDesLoop);

			if (eSIxDESCTAG_SERVICE == *pucRawDes)
			{
				SIxSvcDes_t *pstSvcDes = (SIxSvcDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

				if (NULL != pstSvcDes)
				{
					// onid, ts id
					DbSvc_SetOnId(pstDstSvcInfo, pstSdtTable->unInfo.stSdt.usOrgNetId);
					DbSvc_SetTsId(pstDstSvcInfo, pstSdtTable->unInfo.stSdt.usTsId);

					// service name
					if (DbSvc_GetRenamed(pstDstSvcInfo) == FALSE)
					{
						svc_si_LibCheckServiceNameBySvcDes (pstSvcDes->ucSvcName, pstSvcDes->pszServName, DbSvc_GetName(pstDstSvcInfo), NULL);
					}

					// Service type, Video Type
					hErr = svc_si_LibGetSvcAndVideoTypeFromDvbSvcType (ulActionId, pstSvcDes->ucServType, &eSvcType, &eVideoType);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("svc_si_LibGetSvcAndVideoTypeFromDvbSvcType err: SvcType(%d)\n", pstSvcDes->ucServType);
					}

					DbSvc_SetSvcType(pstDstSvcInfo, eSvcType);
					DbSvc_SetVideoType(pstDstSvcInfo, eVideoType);

					// orgSvcType 은 Dvb Svc Type을 그대로 저장한다..by chmin
					//DbSvc_SetOrgSvcType(pstDstSvcInfo, DbSvc_GetSvcType(pstDstSvcInfo));
					DbSvc_SetOrgSvcType(pstDstSvcInfo, pstSvcDes->ucServType); //??

					PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSvcDes);
				}

				break;
			}
		}

		// svcId가 일치하는 sdt를 찾아 처리했으므로 여기서 break
		break;
	}

	if (NULL != pbChanged)
	{
		*pbChanged = (HxSTD_MemCmp(pstDstSvcInfo, pstOrgSvcInfo, sizeof(DbSvc_Info_t)) == 0) ? FALSE : TRUE;
	}

	if (pstSdtTable != NULL)
	{
		PAL_SIRAWPOOL_FreeTable (pstSdtTable);
	}

	return ERR_OK;
}

HERROR svc_si_LibFreePmtEsInfo (svcSi_PmtEsInfo_t *pstPmtEsInfo)
{
	if (NULL != pstPmtEsInfo)
	{
		pstPmtEsInfo->pstVideoTrack		= HLIB_LIST_RemoveAllFunc (pstPmtEsInfo->pstVideoTrack, svc_si_LibFreeSiEsItem);
		pstPmtEsInfo->pstAudioTrack		= HLIB_LIST_RemoveAllFunc (pstPmtEsInfo->pstAudioTrack, svc_si_LibFreeSiEsItem);
		pstPmtEsInfo->pstSubttlTrack	= HLIB_LIST_RemoveAllFunc (pstPmtEsInfo->pstSubttlTrack, svc_si_LibFreeSiEsItem);
		pstPmtEsInfo->pstTeltxtTrack	= HLIB_LIST_RemoveAllFunc (pstPmtEsInfo->pstTeltxtTrack, svc_si_LibFreeSiEsItem);
		pstPmtEsInfo->pstCaptionTrack	= HLIB_LIST_RemoveAllFunc (pstPmtEsInfo->pstCaptionTrack, svc_si_LibFreeSiEsItem);
		pstPmtEsInfo->pstBmlTrack		= HLIB_LIST_RemoveAllFunc (pstPmtEsInfo->pstBmlTrack, svc_si_LibFreeSiEsItem);
		pstPmtEsInfo->pstEcmTrack		= HLIB_LIST_RemoveAllFunc (pstPmtEsInfo->pstEcmTrack, svc_si_LibFreeSiEsItem);

		OxSI_Free (pstPmtEsInfo);
	}

	return ERR_OK;
}

HERROR svc_si_LibGetAudioLanguageCodeFromPmt (HUINT32 ulActionId, SIxTable_2ndLoop_t *pstPmtElemLoop, HUINT32 ulLangMax, svcSi_Iso639_t *pstIsoLangInfo, HUINT32 *pulLangNum)
{
	HUINT32			 ulTargetIdx = 0;
	HxList_t		*pstItem;

	if ((NULL == pstPmtElemLoop) || (0 == ulLangMax) || (NULL == pstIsoLangInfo))
	{
		return ERR_FAIL;
	}

	for (pstItem = pstPmtElemLoop->pstDesLoop; pstItem != NULL; pstItem = pstItem->next)
	{
		HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstItem);

		if ((NULL != pucRawDes) && (eSIxDESCTAG_ISO_639_LANGUAGE == pucRawDes[0]))
		{
			SIxIso639LangDes_t		*pstIso639Des = (SIxIso639LangDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

			if (NULL != pstIso639Des)
			{
				SIxLangInfo_t		*pstLangInfo = pstIso639Des->pstInfo;

				for (pstLangInfo = pstIso639Des->pstInfo; NULL != pstLangInfo; pstLangInfo = pstLangInfo->pstNext)
				{
					if (ulTargetIdx < ulLangMax)
					{
						pstIsoLangInfo[ulTargetIdx].eAudType	= pstLangInfo->ucAudioType;
						HxSTD_MemCopy (pstIsoLangInfo[ulTargetIdx].szIso639LangCode, pstLangInfo->szIso639LangCode, 4);

						ulTargetIdx++;
					}
				}

				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstIso639Des);
			}
		}
	}

	if (NULL != pulLangNum)					{ *pulLangNum = ulTargetIdx; }
	return ERR_OK;
}

HERROR svc_si_LibGetSupplementaryAudioLanguageCodeFromPmt (HUINT32 ulActionId, SIxTable_2ndLoop_t *pstPmtElemLoop, HUINT32 ulLangMax, svcSi_AudioEs_t *pstAudioEs, HUINT32 *pulLangNum)
{
	HxList_t		*pstItem = NULL;

	if ((NULL == pstPmtElemLoop) || (0 == ulLangMax) || (NULL == pstAudioEs))
	{
		return ERR_FAIL;
	}

	for (pstItem = pstPmtElemLoop->pstDesLoop; pstItem != NULL; pstItem = pstItem->next)
	{
		HUINT8				*pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstItem);

		if ((NULL != pucRawDes) && (eSIxDESCTAG_EXTENSION == pucRawDes[0]))
		{
			SIxExtensionDes_t *pstExtensionDes = (SIxExtensionDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, pucRawDes);

			if (NULL != pstExtensionDes)
			{
				if(pstExtensionDes->pucRawData[0] == eSIxDESCTAG_EXT_SUPPLEMENTARY_AUDIO)
				{
					SIxSupplementaryAudioDes_t *pstSupplemetaryAudioDes = (SIxSupplementaryAudioDes_t *)PAL_SIRAWPOOL_ParseDvbExtDes (ulActionId, pstExtensionDes->pucRawData);
					if(NULL != pstSupplemetaryAudioDes)
					{
						if(pstSupplemetaryAudioDes->ucEditorialClassification == eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED)
						{
							pstAudioEs->ucMixType = pstSupplemetaryAudioDes->ucMixType;
							pstAudioEs->eSuppAudioEditorial = pstSupplemetaryAudioDes->ucEditorialClassification;
#if 0
							HxLOG_Warning("\t\t #### ysmoon [%s] ucMixType(%d) ucEditorialClassification(%d) \n", __FUNCTION__, pstAudioEs->ucMixType, pstAudioEs->eSuppAudioEditorial);

							for(ucIndex = 0 ; ucIndex < ulLangMax; ucIndex++)
							{
								if(pstSupplemetaryAudioDes->ucMixType)
								{
									// BroadCast Mixed
									if( (pstAudioEs->astIsoLangInfo[ucIndex].eAudType == eAUDIO_TYPE_UNDEFINED)
									 ||(pstAudioEs->astIsoLangInfo[ucIndex].eAudType == eAUDIO_TYPE_CLEAN_EFFECTS)
									 ||(pstAudioEs->astIsoLangInfo[ucIndex].eAudType == eAUDIO_TYPE_VISUAL_IMPAIRED))
									{
										pstAudioEs->eSuppAudioEditorial = eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED;
									}
								}
								else
								{	// Receiver Mixed

									if(pstAudioEs->astIsoLangInfo[ucIndex].eAudType == eAUDIO_TYPE_VISUAL_IMPAIRED)
									{
										pstAudioEs->eSuppAudioEditorial = eAUDIO_TYPE_SUPP_AD_VISUAL_IMPARED;
									}
								}

								HxLOG_Warning("[%02d] eAudType (%d) ucMixType(%d) ucEditorialClassification(%d) ucLanguageCodePresent(%d) eSuppAudioEditorial(%d) \n",
									ucIndex, pstAudioEs->astIsoLangInfo[ucIndex].eAudType, pstSupplemetaryAudioDes->ucMixType, pstSupplemetaryAudioDes->ucEditorialClassification, pstSupplemetaryAudioDes->ucLanguageCodePresent, pstAudioEs->eSuppAudioEditorial);

							}
#endif
						}


						PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstSupplemetaryAudioDes);
					}
				}

				PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstExtensionDes);
			}
		}
	}

	return ERR_OK;
}


HERROR svc_si_LibGetVideoInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_VideoList_t *pstVideoList)
{
	HERROR					 hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstPmtTable = NULL;
	SIxTable_PsiTable_t		*pstSdtActTable = NULL;
	svcSi_PmtEsInfo_t		*pstPmtEsInfo = NULL;
	HxList_t				*pstItem = NULL;
	svcSi_VideoEs_t			*pstVideoEs = NULL;
	HINT32					 nVideoCnt = 0, nIdx = 0;
	SvcSi_VideoInfo_t		*pstCurVideoInfo = NULL;

	if (NULL == pstVideoList)
	{
		return ERR_FAIL;
	}

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, usSvcId);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	pstPmtEsInfo = (svcSi_PmtEsInfo_t *)OxSI_Calloc(sizeof(svcSi_PmtEsInfo_t));
	if (NULL == pstPmtEsInfo)
	{
		HxLOG_Error ("mem alloc failed..\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	hErr = psvc_si_GetPmtEsInfoFromPmtSiTable (ulActionId, pstPmtTable, pstPmtEsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("psvc_si_GetPmtEsInfoFromPmtSiTable err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	// count valid pid number
	nVideoCnt = 0;
	pstItem = pstPmtEsInfo->pstVideoTrack;
	while (pstItem != NULL)
	{
		pstVideoEs = (svcSi_VideoEs_t *)HLIB_LIST_Data (pstItem);
		if ((NULL != pstVideoEs) && (PID_NULL != pstVideoEs->usPid))
		{
			nVideoCnt++;
		}

		pstItem = pstItem->next;
	}

	if (nVideoCnt == 0)
	{
		HxLOG_Print ("[%s:%d] No video track ! \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	// set video list
	pstVideoList->nVideo = nVideoCnt;
	pstVideoList->pstVideoInfo = (SvcSi_VideoInfo_t *)OxSI_Calloc(sizeof(SvcSi_VideoInfo_t) * pstVideoList->nVideo);
	if (pstVideoList->pstVideoInfo == NULL)
	{
		HxLOG_Print ("[%s:%d] mem alloc failed.. \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	// get EIT ACT PF table
	pstSdtActTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_SdtAct, SILIB_TABLE_EXT_ID_ALL); // SILIB_TABLE_EXT_ID_ALL
	if (NULL == pstSdtActTable)
	{
		HxLOG_Warning ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, usSvcId);
	}

	nIdx = 0;
	pstItem = pstPmtEsInfo->pstVideoTrack;

	while (pstItem != NULL)
	{
		pstCurVideoInfo = &(pstVideoList->pstVideoInfo[nIdx++]);
		pstVideoEs = (svcSi_VideoEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL != pstVideoEs) && (PID_NULL != pstVideoEs->usPid))
		{
			hErr = svc_si_LibUpdateVideoInfo(ulActionId, usSvcId, pstSdtActTable, pstVideoEs, pstCurVideoInfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Error ("svc_si_LibUpdateVideoInfo() return fail.. \n");
			}
		}
		pstItem = pstItem->next;
	}

END_FUNC:
	if (NULL != pstPmtEsInfo)					{ svc_si_LibFreePmtEsInfo (pstPmtEsInfo); }
	if (NULL != pstPmtTable)					{ PAL_SIRAWPOOL_FreeTable (pstPmtTable); }
	if (NULL != pstSdtActTable)				{ PAL_SIRAWPOOL_FreeTable (pstSdtActTable); }

	return hErr;
}

HERROR svc_si_LibGetAudioInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_AudioList_t *pstAudioList)
{
	HERROR					 hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstPmtTable = NULL;
	SIxTable_PsiTable_t		*pstEitActPfTable = NULL;
	svcSi_PmtEsInfo_t		*pstPmtEsInfo = NULL;
	HxList_t				*pstItem = NULL;
	svcSi_AudioEs_t			*pstAudioEs = NULL;
	HINT32					 nAudioCnt = 0, nIdx = 0;
	SvcSi_AudioInfo_t		*pstCurAudioInfo = NULL;

	if (NULL == pstAudioList)
	{
		return ERR_FAIL;
	}

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, usSvcId);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	pstPmtEsInfo = (svcSi_PmtEsInfo_t *)OxSI_Calloc(sizeof(svcSi_PmtEsInfo_t));
	if (NULL == pstPmtEsInfo)
	{
		HxLOG_Error ("mem alloc failed..\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	hErr = psvc_si_GetPmtEsInfoFromPmtSiTable (ulActionId, pstPmtTable, pstPmtEsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("psvc_si_GetPmtEsInfoFromPmtSiTable err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	// count valid pid number
	nAudioCnt = 0;
	pstItem = pstPmtEsInfo->pstAudioTrack;
	while (pstItem != NULL)
	{
		pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (pstItem);
		if ((NULL != pstAudioEs) && (PID_NULL != pstAudioEs->usPid))
		{
			nAudioCnt++;
		}

		pstItem = pstItem->next;
	}

	if (nAudioCnt == 0)
	{
		HxLOG_Print ("[%s:%d] No audio track ! \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	// set audio list
	pstAudioList->nAudio = nAudioCnt;
	pstAudioList->pstAudioInfo = (SvcSi_AudioInfo_t *)OxSI_Calloc(sizeof(SvcSi_AudioInfo_t) * pstAudioList->nAudio);
	if (pstAudioList->pstAudioInfo == NULL)
	{
		HxLOG_Print ("[%s:%d] mem alloc failed.. \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	// get event id
	// TODO: ...

	// get EIT ACT PF table
	pstEitActPfTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_EitActPf, usSvcId); // SILIB_TABLE_EXT_ID_ALL
	if (NULL == pstEitActPfTable)
	{
		HxLOG_Warning ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, usSvcId);
	}

	nIdx = 0;
	pstItem = pstPmtEsInfo->pstAudioTrack;

	while (pstItem != NULL)
	{
		pstCurAudioInfo = &(pstAudioList->pstAudioInfo[nIdx++]);
		pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL != pstAudioEs) && (PID_NULL != pstAudioEs->usPid))
		{
			hErr = svc_si_LibUpdateAudioInfo(ulActionId, usSvcId, pstEitActPfTable, pstAudioEs, pstCurAudioInfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Error ("svc_si_LibUpdateAudioInfo() return fail.. \n");
			}
		}
		pstItem = pstItem->next;
	}

END_FUNC:
	if (NULL != pstPmtEsInfo)					{ svc_si_LibFreePmtEsInfo (pstPmtEsInfo); }
	if (NULL != pstPmtTable)					{ PAL_SIRAWPOOL_FreeTable (pstPmtTable); }
	if (NULL != pstEitActPfTable)				{ PAL_SIRAWPOOL_FreeTable (pstEitActPfTable); }

	return hErr;
}

HERROR svc_si_LibGetSubtitleInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_SubtitleList_t *pstSubtitleList)
{
	HERROR					 hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstPmtTable = NULL;
	svcSi_PmtEsInfo_t		*pstPmtEsInfo = NULL;
	HxList_t				*pstItem = NULL;
	svcSi_SubtitleEs_t		*pstSubtitleEs = NULL;
	SvcSi_SubtitleInfo_t			*pstCurSubtitleInfo = NULL;
	HINT32					 lSubtitleCount = 0, nIdx = 0;

	if (NULL == pstSubtitleList)
	{
		return ERR_FAIL;
	}

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, usSvcId);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	pstPmtEsInfo = (svcSi_PmtEsInfo_t *)OxSI_Calloc(sizeof(svcSi_PmtEsInfo_t));
	if (NULL == pstPmtEsInfo)
	{
		HxLOG_Error ("mem alloc failed..\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	hErr = psvc_si_GetPmtEsInfoFromPmtSiTable (ulActionId, pstPmtTable, pstPmtEsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("psvc_si_GetPmtEsInfoFromPmtSiTable err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	// count valid pid number
	lSubtitleCount = 0;
	pstItem = pstPmtEsInfo->pstSubttlTrack;
	while (pstItem != NULL)
	{
		pstSubtitleEs = (svcSi_SubtitleEs_t *)HLIB_LIST_Data (pstItem);
		if ((NULL != pstSubtitleEs) && (PID_NULL != pstSubtitleEs->usPid))
		{
			lSubtitleCount++;
		}

		pstItem = pstItem->next;
	}

	if (lSubtitleCount == 0)
	{
		HxLOG_Print ("[%s:%d] No Subtitle track ! \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	pstSubtitleList->ulSubtitleNum = lSubtitleCount;
	pstSubtitleList->pstSubtitleInfo= (SvcSi_SubtitleInfo_t *)OxSI_Calloc(sizeof(SvcSi_SubtitleInfo_t) * pstSubtitleList->ulSubtitleNum);
	if (pstSubtitleList->pstSubtitleInfo == NULL)
	{
		HxLOG_Print ("[%s:%d] mem alloc failed.. \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	nIdx = 0;
	pstItem = pstPmtEsInfo->pstSubttlTrack;
	while (pstItem != NULL)
	{
		pstCurSubtitleInfo = &(pstSubtitleList->pstSubtitleInfo[nIdx++]);
		pstSubtitleEs = (svcSi_SubtitleEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL != pstSubtitleEs) && (PID_NULL != pstSubtitleEs->usPid))
		{
			svc_si_LibUpdateSubtitleInfo(pstSubtitleEs, pstCurSubtitleInfo);
		}

		pstItem = pstItem->next;
	}

END_FUNC:
	if (NULL != pstPmtEsInfo)					{ svc_si_LibFreePmtEsInfo (pstPmtEsInfo); }
	if (NULL != pstPmtTable)					{ PAL_SIRAWPOOL_FreeTable (pstPmtTable); }

	return hErr;
}

HERROR svc_si_LibGetSuperCcInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_SubtitleList_t *pstSuperCcList)
{
	HERROR					 hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstPmtTable = NULL;
	svcSi_PmtEsInfo_t		*pstPmtEsInfo = NULL;
	HxList_t				*pstItem = NULL;
	svcSi_CaptionEs_t		*pstSuperCcEs = NULL;
	SvcSi_SubtitleInfo_t			*pstCurSubtitleInfo = NULL;
	HINT32					 lSuperCcCount = 0, nIdx = 0;

	if (NULL == pstSuperCcList)
	{
		return ERR_FAIL;
	}

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, usSvcId);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	pstPmtEsInfo = (svcSi_PmtEsInfo_t *)OxSI_Calloc(sizeof(svcSi_PmtEsInfo_t));
	if (NULL == pstPmtEsInfo)
	{
		HxLOG_Error ("mem alloc failed..\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	hErr = psvc_si_GetPmtEsInfoFromPmtSiTable (ulActionId, pstPmtTable, pstPmtEsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("psvc_si_GetPmtEsInfoFromPmtSiTable err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	// count valid pid number
	lSuperCcCount = 0;
	pstItem = pstPmtEsInfo->pstCaptionTrack;
	while (pstItem != NULL)
	{
		pstSuperCcEs = (svcSi_CaptionEs_t *)HLIB_LIST_Data (pstItem);
		if ((NULL != pstSuperCcEs) && (PID_NULL != pstSuperCcEs->usPid))
		{
			lSuperCcCount++;
		}

		pstItem = pstItem->next;
	}

	if (lSuperCcCount == 0)
	{
		HxLOG_Print ("[%s:%d] No SuperCc track ! \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	pstSuperCcList->ulSubtitleNum = lSuperCcCount;
	pstSuperCcList->pstSubtitleInfo= (SvcSi_SubtitleInfo_t *)OxSI_Calloc(sizeof(SvcSi_SubtitleInfo_t) * pstSuperCcList->ulSubtitleNum);
	if (pstSuperCcList->pstSubtitleInfo == NULL)
	{
		HxLOG_Print ("[%s:%d] mem alloc failed.. \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	nIdx = 0;
	pstItem = pstPmtEsInfo->pstCaptionTrack;
	while (pstItem != NULL)
	{
		pstCurSubtitleInfo = &(pstSuperCcList->pstSubtitleInfo[nIdx++]);
		pstSuperCcEs = (svcSi_CaptionEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL != pstSuperCcEs) && (PID_NULL != pstSuperCcEs->usPid))
		{
			pstCurSubtitleInfo->usPid 		= pstSuperCcEs->usPid;
			pstCurSubtitleInfo->eType 		= eSI_SUBTITLE_SUPERCC;
			pstCurSubtitleInfo->utArg.stSuperCcInfo.ucComponentTag = pstSuperCcEs->ucComponentTag;
		}

		pstItem = pstItem->next;
	}

END_FUNC:
	if (NULL != pstPmtEsInfo)					{ svc_si_LibFreePmtEsInfo (pstPmtEsInfo); }
	if (NULL != pstPmtTable)					{ PAL_SIRAWPOOL_FreeTable (pstPmtTable); }

	return hErr;
}

HERROR svc_si_LibGetEcmInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_EcmList_t *pstEcmList)
{
	HERROR					 hErr = ERR_FAIL;
	SIxTable_PsiTable_t		*pstPmtTable = NULL;
	svcSi_PmtEsInfo_t		*pstPmtEsInfo = NULL;
	HxList_t				*pstItem = NULL;
	svcSi_EcmEs_t			*pstEcmEs = NULL;
	HINT32					 nEcmCnt = 0, nIdx = 0;
	SvcSi_EcmInfo_t			*pstCurEcmInfo = NULL;

	if (NULL == pstEcmList)
	{
		return ERR_FAIL;
	}

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, usSvcId);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("PAL_SIRAWPOOL_ParseTable: ulActionId(%d), ulSvcId(0x%04x)\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	pstPmtEsInfo = (svcSi_PmtEsInfo_t *)OxSI_Calloc(sizeof(svcSi_PmtEsInfo_t));
	if (NULL == pstPmtEsInfo)
	{
		HxLOG_Error ("mem alloc failed..\n", ulActionId, usSvcId);
		goto END_FUNC;
	}

	hErr = psvc_si_GetPmtEsInfoFromPmtSiTable (ulActionId, pstPmtTable, pstPmtEsInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("psvc_si_GetPmtEsInfoFromPmtSiTable err: ActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	// count valid ecm pid number
	nEcmCnt = 0;
	pstItem = pstPmtEsInfo->pstEcmTrack;
	while (pstItem != NULL)
	{
		pstEcmEs = (svcSi_EcmEs_t *)HLIB_LIST_Data (pstItem);
		if ((NULL != pstEcmEs) && (PID_NULL != pstEcmEs->usPid))
		{
			nEcmCnt++;
		}

		pstItem = pstItem->next;
	}

	if (nEcmCnt == 0)
	{
		HxLOG_Print ("[%s:%d] No audio track ! \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	// set ecm list
	pstEcmList->nEcm = nEcmCnt;
	pstEcmList->pstEcmInfo = (SvcSi_EcmInfo_t *)OxSI_Calloc(sizeof(SvcSi_EcmInfo_t) * pstEcmList->nEcm);
	if (pstEcmList->pstEcmInfo == NULL)
	{
		HxLOG_Print ("[%s:%d] mem alloc failed.. \n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	nIdx = 0;
	pstItem = pstPmtEsInfo->pstEcmTrack;

	while (pstItem != NULL)
	{
		pstCurEcmInfo = &(pstEcmList->pstEcmInfo[nIdx++]);
		pstEcmEs = (svcSi_EcmEs_t *)HLIB_LIST_Data (pstItem);

		if ((NULL != pstEcmEs) && (PID_NULL != pstEcmEs->usPid))
		{
			hErr = svc_si_LibUpdateEcmInfo	(pstEcmEs, pstCurEcmInfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Error ("svc_si_LibUpdateEcmInfo() return fail.. \n");
			}
		}
		pstItem = pstItem->next;
	}

END_FUNC:
	if (NULL != pstPmtEsInfo)					{ svc_si_LibFreePmtEsInfo (pstPmtEsInfo); }
	if (NULL != pstPmtTable)					{ PAL_SIRAWPOOL_FreeTable (pstPmtTable); }

	return hErr;
}

HERROR svc_si_LibFreeVideoList (SvcSi_VideoList_t *pstVideoList)
{
	if (NULL == pstVideoList)
	{
		return ERR_FAIL;
	}

	if (pstVideoList->pstVideoInfo != NULL)
	{
		OxSI_Free (pstVideoList->pstVideoInfo);
	}

	pstVideoList->nVideo = 0;
	pstVideoList->pstVideoInfo = NULL;

	return ERR_OK;
}

HERROR svc_si_LibFreeAudioList (SvcSi_AudioList_t *pstAudioList)
{
	if (NULL == pstAudioList)
	{
		return ERR_FAIL;
	}

	if (pstAudioList->pstAudioInfo != NULL)
	{
		OxSI_Free (pstAudioList->pstAudioInfo);
	}

	pstAudioList->nAudio = 0;
	pstAudioList->pstAudioInfo = NULL;

	return ERR_OK;
}

HERROR svc_si_LibFreeSubtitleList (SvcSi_SubtitleList_t *pstSubtitleList)
{
	if (NULL == pstSubtitleList)
	{
		return ERR_FAIL;
	}

	if (pstSubtitleList->pstSubtitleInfo != NULL)
	{
		OxSI_Free (pstSubtitleList->pstSubtitleInfo);
	}

	pstSubtitleList->ulSubtitleNum = 0;
	pstSubtitleList->pstSubtitleInfo = NULL;

	return ERR_OK;
}

HERROR svc_si_LibFreeEcmList (SvcSi_EcmList_t *pstEcmList)
{
	if (NULL == pstEcmList)
	{
		return ERR_FAIL;
	}

	if (pstEcmList->pstEcmInfo != NULL)
	{
		OxSI_Free (pstEcmList->pstEcmInfo);
	}

	pstEcmList->nEcm = 0;
	pstEcmList->pstEcmInfo = NULL;

	return ERR_OK;
}

#define _____SI_CONFIG_____
#define	svcSi_ConfigPrintSystem					0x00000001
#define	svcSi_ConfigPrintAudio					0x00000002
#define	svcSi_ConfigPrintSubtitle				0x00000004
#define	svcSi_ConfigPrintAll					0xFFFFFFFF
#define	svcSi_ConfigCheckMask(ulMask, ulFlag)	((ulMask == ((ulFlag) & (ulMask))) ? TRUE : FALSE)

#define	svcSi_LangIdTo639(eLangId)				((0 == eLangId || eLangID_MAX == eLangId || eLangID_NONE == eLangId) ? "" : HLIB_LANG_IdTo639(eLangId))

STATIC void svc_si_LibSvcEsPrintConfig(const SvcSi_Config_t *pstConfig, HUINT32 ulFlag)
{
	HBOOL		bSystem, bAudio, bSubtitle;

	if (ulFlag == 0)
		return;

	bSystem = bAudio = bSubtitle = FALSE;
	if (svcSi_ConfigCheckMask(svcSi_ConfigPrintSystem, ulFlag))
	{
		bSystem = TRUE;
	}

	if (svcSi_ConfigCheckMask(svcSi_ConfigPrintAudio, ulFlag))
	{
		bAudio = TRUE;
	}

	if (svcSi_ConfigCheckMask(svcSi_ConfigPrintSubtitle, ulFlag))
	{
		bSubtitle = TRUE;
	}

	HxLOG_Print("-----------------------------------------------------------------------------\n");
	HxLOG_Print("- Config \n");

	if (bSystem)
	{
		HxLOG_Print("-----------------------------------------------------------------------------\n");
		HxLOG_Print("- System\n");
		HxLOG_Print("-----------------------------------------------------------------------------\n");
		HxLOG_Print("- [%60s] : [%d]\n", "eOperator", pstConfig->stSystem.eOperator);
		HxLOG_Print("- [%60s] : [%d]\n", "eCountry", pstConfig->stSystem.eCountry);
		HxLOG_Print("- [%60s] : [%d]\n", "eAspectRatio", pstConfig->stSystem.eAspectRatio);
		HxLOG_Print("- [%60s] : [%s]\n", "eOsdLang", svcSi_LangIdTo639(pstConfig->stSystem.eOsdLang));
	}

	if (bAudio)
	{
		HxLOG_Print("-----------------------------------------------------------------------------\n");
		HxLOG_Print("- Audio\n");
		HxLOG_Print("-----------------------------------------------------------------------------\n");
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_AUD_LANG_User]", svcSi_LangIdTo639(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_User]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_AUD_LANG_0]", svcSi_LangIdTo639(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_0]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_AUD_LANG_1]", svcSi_LangIdTo639(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_1]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_AUD_LANG_Undefined]", svcSi_LangIdTo639(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Undefined]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_AUD_LANG_Original]", svcSi_LangIdTo639(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Original]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_AUD_LANG_Default]", svcSi_LangIdTo639(pstConfig->stAudioLanguage.aeLangId[eSvcSi_CFG_AUD_LANG_Default]));
		HxLOG_Print("- [%60s] : [%d]\n", "abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_User]", pstConfig->stAudioDesc.abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_User]);
		HxLOG_Print("- [%60s] : [%d]\n", "abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_Menu]", pstConfig->stAudioDesc.abAdFlag[eSvcSi_CFG_AUD_DESCRIPTION_Menu]);
		HxLOG_Print("- [%60s] : [%d]\n", "eStereoSelect", pstConfig->stAudioOutput.eStereoSelect);
		HxLOG_Print("- [%60s] : [%d]\n", "abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_User]", pstConfig->stAudioOutput.abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_User]);
		HxLOG_Print("- [%60s] : [%d]\n", "abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_Menu]", pstConfig->stAudioOutput.abOutputFlag[eSvcSi_CFG_AUD_OUTPUT_Multichannel_Menu]);
	}

	if (bSubtitle)
	{
		HxLOG_Print("-----------------------------------------------------------------------------\n");
		HxLOG_Print("- Subtitle\n");
		HxLOG_Print("-----------------------------------------------------------------------------\n");
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_SUBTTL_LANG_User]", svcSi_LangIdTo639(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_User]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_SUBTTL_LANG_0]", svcSi_LangIdTo639(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_0]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_SUBTTL_LANG_1]", svcSi_LangIdTo639(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_1]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_SUBTTL_LANG_Undefined]", svcSi_LangIdTo639(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Undefined]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_SUBTTL_LANG_Original]", svcSi_LangIdTo639(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Original]));
		HxLOG_Print("- [%60s] : [%s]\n", "aeLangId[eSvcSi_CFG_SUBTTL_LANG_Default]", svcSi_LangIdTo639(pstConfig->stSubtitleLangauge.aeLangId[eSvcSi_CFG_SUBTTL_LANG_Default]));
		HxLOG_Print("- [%60s] : [%d]\n", "abDvb[eSvcSi_CFG_SUBTTL_DVB_User]", pstConfig->stSubtitleType.abDvb[eSvcSi_CFG_SUBTTL_DVB_User]);
		HxLOG_Print("- [%60s] : [%d]\n", "abDvb[eSvcSi_CFG_SUBTTL_DVB_Menu]", pstConfig->stSubtitleType.abDvb[eSvcSi_CFG_SUBTTL_DVB_Menu]);
		HxLOG_Print("- [%60s] : [%d]\n", "abHoh[eSvcSi_CFG_SUBTTL_HOH_User]", pstConfig->stSubtitleType.abHoh[eSvcSi_CFG_SUBTTL_HOH_User]);
		HxLOG_Print("- [%60s] : [%d]\n", "abHoh[eSvcSi_CFG_SUBTTL_HOH_Menu]", pstConfig->stSubtitleType.abHoh[eSvcSi_CFG_SUBTTL_HOH_Menu]);
		HxLOG_Print("- [%60s] : [%d]\n", "abHD[eSvcSi_CFG_SUBTTL_HD_User]", pstConfig->stSubtitleType.abHD[eSvcSi_CFG_SUBTTL_HD_User]);
		HxLOG_Print("- [%60s] : [%d]\n", "abHD[eSvcSi_CFG_SUBTTL_HD_Menu]", pstConfig->stSubtitleType.abHD[eSvcSi_CFG_SUBTTL_HD_Menu]);
	}

	HxLOG_Print("-----------------------------------------------------------------------------\n\n");
}

#define _____SELECT_ES_COMMON_____

STATIC void svc_si_LibSvcEsPrintEsList(HxList_t *pstEsList, HBOOL (*pfnFunc)(void *pvData, void *pvUserData))
{
#if defined(CONFIG_DEBUG)
	if (pfnFunc)
	{
		HINT32	nCnt = 0;
	 	HxLOG_Print("-------------------------------------------------------------------\n");
		HxLOG_Print("- ES Count [%d]\n", HLIB_LIST_Length(pstEsList));
	 	HxLOG_Print("-------------------------------------------------------------------\n");

		(void)HLIB_LIST_ForeachEx(pstEsList, pfnFunc, (void *)&nCnt);

	 	HxLOG_Print("-------------------------------------------------------------------\n");
 	}
#endif
}

STATIC void svc_si_LibSvcEsPrintCandidateList(HxVector_t *pstCandidateList, HBOOL (* pfnFunc)(void *pvData, void *pvUserData))
{
#if defined(CONFIG_DEBUG)
	if (pfnFunc)
	{
		HUINT32 ulItemNum;

		ulItemNum = HLIB_VECTOR_Length(pstCandidateList);
		HxLOG_Print("-----------------------------------------------------------------------\n");
		HxLOG_Print("- Candidate List [%02d] -------------------------------------------------\n", ulItemNum);
		HxLOG_Print("-----------------------------------------------------------------------\n");
		if (ulItemNum > 0)
		{
			HUINT32 ulCount = 0;
			HLIB_VECTOR_ForEach(pstCandidateList, pfnFunc, (void *)&ulCount);
		}
		HxLOG_Print("-----------------------------------------------------------------------\n");
	}
#endif
}


#define _____SELECT_AUDIO_____
#if defined(CONFIG_DEBUG)
STATIC HUINT8* svc_si_LibSvcEsPrintAudioCandidateName(SvcSi_EsAudioSelType_e eSelectType)
{
	switch (eSelectType)
	{
	ENUM_TO_STR(eSvcSi_AUD_SEL_LANG_USER);
	ENUM_TO_STR(eSvcSi_AUD_SEL_LANG_USER_WITH_PID);
	ENUM_TO_STR(eSvcSi_AUD_SEL_LANG_0);
	ENUM_TO_STR(eSvcSi_AUD_SEL_LANG_1);
	ENUM_TO_STR(eSvcSi_AUD_SEL_LANG_OSD);
	ENUM_TO_STR(eSvcSi_AUD_SEL_LANG_UNDEFINED);
	ENUM_TO_STR(eSvcSi_AUD_SEL_LANG_ORIGINAL);
	ENUM_TO_STR(eSvcSi_AUD_SEL_LANG_DEFAULT);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF);
	ENUM_TO_STR(eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF);
	ENUM_TO_STR(eSvcSi_AUD_SEL_MULTICHANNEL_USER);
	ENUM_TO_STR(eSvcSi_AUD_SEL_MULTICHANNEL_MENU);
	default: break;
	}
	return "eSvcSi_AUD_SEL_Unknown";
}

STATIC void svc_si_LibSvcEsPrintAudioCandidateItem(svcSi_EsAudioCandidate_t *pstCandidate, HUINT32 ulItemNum, HBOOL bDetail)
{
	svcSi_EsAudioMatchedInfo_t		*pstMatched = svcSi_EsAudSelGetMatchedInfo(pstCandidate);
	svcSi_EsAudioProperty_t			*pstProperty = svcSi_EsAudSelGetEsProperty(pstCandidate);
	HCHAR							*pszLang = (HCHAR *)svcSi_LangIdTo639(pstMatched->eLangId);

	HxLOG_Print("- [%02d] Index [%02d], Pid[0x%04x] Priority [%08d], Lang [%3s], eDualMonoSelect [%d]\n", ulItemNum, pstProperty->nIndex, pstProperty->usPid,
																						pstCandidate->nPriority,
																						(char *)pszLang,
																						pstMatched->eDualMonoSelect);
	if (bDetail)
	{
		HUINT32 ulCount;
		for (ulCount = 0; ulCount <eSvcSi_AUD_SEL_TYPE_NUM; ulCount++)
		{
			svcSi_EsPrint("     [%50s] : Matched [%d], Priority [%08d]\n",
							svc_si_LibSvcEsPrintAudioCandidateName(ulCount),
							pstMatched->astMatchedList[ulCount].bMached,
							pstMatched->astMatchedList[ulCount].nPriority);
		}
	}
}

STATIC void svc_si_LibSvcEsPrintAudioEsItem(svcSi_AudioEs_t *pstAudioEs, HUINT32 ulIndex, HBOOL bDetail)
{
	HxLOG_Print("- [%02d] : Pid(0x%04x), Codec(%d), ComponentTag(0x%x) [Lang[0]([%3s], %d) Lang[1]([%3s], %d), MixType(0x%02x), SupAudEditorial(%02d)\n", ulIndex,
				pstAudioEs->usPid, pstAudioEs->eAudCodec,pstAudioEs->ucComponentTag,
				pstAudioEs->astIsoLangInfo[0].szIso639LangCode, pstAudioEs->astIsoLangInfo[0].eAudType,
				pstAudioEs->astIsoLangInfo[1].szIso639LangCode, pstAudioEs->astIsoLangInfo[1].eAudType,
				pstAudioEs->ucMixType, pstAudioEs->eSuppAudioEditorial);
}

STATIC HBOOL svc_si_LibSvcEsPrintAudioEsItem2(void *pvData, void *pvUserData)
{
	svcSi_AudioEs_t		*pstAudioEs;
	HINT32				*pnCount = (HINT32 *)pvUserData;

	pstAudioEs = (svcSi_AudioEs_t *)pvData;

	// Ignore track
	if ((NULL == pstAudioEs) || (PID_NULL == pstAudioEs->usPid) || (NULL == pnCount))
		return FALSE;

	svc_si_LibSvcEsPrintAudioEsItem(pstAudioEs, *pnCount, FALSE);
	(*pnCount)++;

	return FALSE;
}

STATIC HBOOL svc_si_LibSvcEsPrintAudioCandidateItemCb(void *pvData, void *pvUserData)
{
	svcSi_EsAudioCandidate_t	*pstCandidate = (svcSi_EsAudioCandidate_t *)pvData;
	HUINT32						*pulItemIndex = (HUINT32 *)pvUserData;

	if (NULL == pstCandidate)
		return FALSE;

	if (NULL == pulItemIndex)
		return FALSE;

	svc_si_LibSvcEsPrintAudioCandidateItem(pstCandidate, *pulItemIndex, FALSE);
	(*pulItemIndex)++;

	return FALSE;
}
#endif

STATIC SvcSi_EsAudioSelType_e svc_si_LibEsAudioGetSelTypeFromLangConfiguration(SvcSi_CfgAudioLang_e eConfiguration)
{
	switch (eConfiguration)
	{
	case eSvcSi_CFG_AUD_LANG_User:				return eSvcSi_AUD_SEL_LANG_USER;
	case eSvcSi_CFG_AUD_LANG_User_With_Pid:		return eSvcSi_AUD_SEL_LANG_USER_WITH_PID;
	case eSvcSi_CFG_AUD_LANG_0:					return eSvcSi_AUD_SEL_LANG_0;
	case eSvcSi_CFG_AUD_LANG_1:					return eSvcSi_AUD_SEL_LANG_1;
	case eSvcSi_CFG_AUD_LANG_Undefined:			return eSvcSi_AUD_SEL_LANG_UNDEFINED;
	case eSvcSi_CFG_AUD_LANG_Original:			return eSvcSi_AUD_SEL_LANG_ORIGINAL;
	case eSvcSi_CFG_AUD_LANG_Default:			return eSvcSi_AUD_SEL_LANG_DEFAULT;
	default:									break;
	}

	return eSvcSi_AUD_SEL_TYPE_NUM;
}

STATIC HBOOL svc_si_LibEsAudioGetMatched(svcSi_EsMatch_t *astMatchedList, SvcSi_EsAudioSelType_e eSelType)
{
	if (eSvcSi_AUD_SEL_TYPE_NUM <= eSelType)
		return FALSE;

	return astMatchedList[eSelType].bMached;
}

STATIC HBOOL svc_si_LibEsAudioSetMatched(svcSi_EsMatch_t *astMatchedList, SvcSi_EsAudioSelType_e eSelType, HBOOL bMatched)
{
	if (eSvcSi_AUD_SEL_TYPE_NUM <= eSelType)
		return FALSE;

	astMatchedList[eSelType].bMached = bMatched;

	return astMatchedList[eSelType].bMached;
}

STATIC HBOOL svc_si_LibEsAudioIsLanguage(svcSi_EsMatch_t *astMatchedList)
{
	if (NULL == astMatchedList)	return FALSE;

	return (astMatchedList[eSvcSi_AUD_SEL_LANG_USER].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_LANG_USER_WITH_PID].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_LANG_0].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_LANG_1].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_LANG_UNDEFINED].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_LANG_ORIGINAL].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_LANG_DEFAULT].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_LANG_OSD].bMached);
}

STATIC HBOOL svc_si_LibEsAudioIsMultichannel(svcSi_EsMatch_t *astMatchedList)
{
	return astMatchedList[eSvcSi_AUD_SEL_MULTICHANNEL_USER].bMached || astMatchedList[eSvcSi_AUD_SEL_MULTICHANNEL_MENU].bMached;
}

STATIC HBOOL svc_si_LibEsAudioIsAudioDescriptionBroadcasterMixed(svcSi_EsMatch_t *astMatchedList)
{
	if (NULL == astMatchedList)	return FALSE;

	return (astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU].bMached);
}

STATIC HBOOL svc_si_LibEsAudioIsAudioDescriptionReceiverMixed(svcSi_EsMatch_t *astMatchedList)
{
	if (NULL == astMatchedList)	return FALSE;

	return (astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU].bMached);
}

STATIC HBOOL svc_si_LibEsAudioIsAudioDescription(svcSi_EsMatch_t *astMatchedList)
{
	if (NULL == astMatchedList)	return FALSE;

	return (astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU].bMached);
}

STATIC HBOOL svc_si_LibEsAudioIsUser(svcSi_EsMatch_t *astMatchedList)
{
	if (NULL == astMatchedList)	return FALSE;

	return (astMatchedList[eSvcSi_AUD_SEL_LANG_USER].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_LANG_USER_WITH_PID].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_MULTICHANNEL_USER].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER].bMached ||
			astMatchedList[eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER].bMached);
}

STATIC HINT32 svc_si_LibEsAudioGetPriority(svcSi_EsMatch_t *astMatchedList, SvcSi_EsAudioSelType_e eSelType)
{
	if (eSvcSi_AUD_SEL_TYPE_NUM <= eSelType)
		return -1;

	return astMatchedList[eSelType].nPriority;
}

STATIC HINT32 svc_si_LibEsAudioSetPriority(svcSi_EsMatch_t *astMatchedList, SvcSi_EsAudioSelType_e eSelType, HINT32 nPriority)
{
	if (eSvcSi_AUD_SEL_TYPE_NUM <= eSelType)
		return -1;

	astMatchedList[eSelType].nPriority = nPriority;

	return astMatchedList[eSelType].nPriority;
}

STATIC HINT32	svc_si_LibCompareAudioPriority (const void *pvArg1, const void *pvArg2)
{
	const svcSi_EsAudioCandidate_t *pstLeft = *(const svcSi_EsAudioCandidate_t **)pvArg1;
	const svcSi_EsAudioCandidate_t *pstRight = *(const svcSi_EsAudioCandidate_t **)pvArg2;

	if (pstLeft->nPriority > pstRight->nPriority)
		return -1;

	if (pstLeft->nPriority < pstRight->nPriority)
		return +1;

	return 0;
}

STATIC void svc_si_LibEsAudioResetCandidate(svcSi_EsAudioCandidate_t *pstCandidate)
{
	if (pstCandidate)
	{
		svcSi_EsAudioProperty_t		*pstProperty = svcSi_EsAudSelGetEsProperty(pstCandidate);
		svcSi_EsAudioMatchedInfo_t	*pstMatched = svcSi_EsAudSelGetMatchedInfo(pstCandidate);

		HxSTD_MemSet(pstCandidate, 0x00, sizeof(svcSi_EsAudioCandidate_t));

		pstMatched->eLangId					= eLangID_MAX;
		pstMatched->eDualMonoSelect			= eDxDUALMONO_SELECT_UNKNOWN;

		pstProperty->nIndex						= -1;
		pstProperty->bFirstTrack				= FALSE;
		pstProperty->bMultiChannel				= FALSE;
		pstProperty->bAdBroadcasterMixed		= FALSE;
		pstProperty->bAdReceiverMixed			= FALSE;
		pstProperty->bDualMono					= FALSE;
		pstProperty->nAlternativePriority		= 0;
		pstProperty->usPid						= PID_NULL;
		pstProperty->eCodec						= eDxAUDIO_CODEC_UNKNOWN;

		pstCandidate->nPriority				= -1;
	}
}

STATIC HBOOL svc_si_LibSvcEsGetAudioOutputConfig(const SvcSi_CfgAudioOutput_t *pstConfig, SvcSi_CfgAudioOutput_e eOutputType)
{
	if (eSvcSi_CFG_AUD_OUTPUT_NUM <= eOutputType)
		return FALSE;

	return pstConfig->abOutputFlag[eOutputType];
}

STATIC HBOOL svc_si_LibSvcEsGetAudioDescriptionConfig(const SvcSi_CfgAudioDesc_t *pstConfig, SvcSi_CfgAudioDesc_e eOutputType)
{
	if (eSvcSi_CFG_AUD_DESCRIPTION_NUM <= eOutputType)
		return FALSE;

	return pstConfig->abAdFlag[eOutputType];
}


STATIC void svc_si_LibSvcEsInitAudioSelection(SvcSi_SelectedEs_t *pstSelectedAudio)
{
	pstSelectedAudio->bFound			= FALSE;
	pstSelectedAudio->nIndexMain		= -1;
	pstSelectedAudio->nIndexAux			= -1;
	pstSelectedAudio->usPid				= PID_NULL;
	pstSelectedAudio->ucComponentTag	= 0;
	pstSelectedAudio->ulCodec			= 0;
	pstSelectedAudio->eLangIdMain		= eLangID_MAX;
	pstSelectedAudio->eLangIdAux		= eLangID_MAX;
	pstSelectedAudio->ulParam1			= eDxDUALMONO_SELECT_UNKNOWN;
	pstSelectedAudio->ulParam2			= 0;
	pstSelectedAudio->ulParam3			= 0;
}

STATIC HBOOL svc_si_LibSvcEsIsAudioTrackDolby (const svcSi_AudioEs_t *pstAudioEs)
{
	return SvcSi_ES_AUDIO_IS_DOLBY_TYPE(pstAudioEs->eAudCodec);
}

STATIC HBOOL svc_si_LibSvcEsIsAudioTrackMultiChannel (const svcSi_AudioEs_t *pstAudioEs)
{
	return SvcSi_ES_AUDIO_IS_MULTICHANNEL_TYPE(pstAudioEs->eAudCodec);
}

HBOOL svc_si_LibSvcEsGetLangInfo(const svcSi_AudioEs_t *pstAudioEs, HxLANG_Id_e *peLangId1, HxLANG_Id_e *peLangId2, HBOOL *pbDualMono)
{
	HxLANG_Id_e		 eLangId1, eLangId2;
	HUINT32			 ulLen1, ulLen2;
	AUDIO_Type_t	 eType1, eType2;
	HBOOL			 bDualMono;

	eLangId1 = eLangId2 = eLangID_NONE;
	eType1 = eType2 = eAUDIO_TYPE_UNDEFINED;

	// 1st Track : Check the language number:
	eType1 = pstAudioEs->astIsoLangInfo[0].eAudType;
	ulLen1 = HxSTD_StrLen(pstAudioEs->astIsoLangInfo[0].szIso639LangCode);
	if (3 == ulLen1)
	{
		eLangId1 = HLIB_LANG_639ToId ((const HCHAR *)pstAudioEs->astIsoLangInfo[0].szIso639LangCode);
	}

	HxLOG_Print("[Lang][0][%3s](%d) => [0x%x][eType1 : %d]\n", pstAudioEs->astIsoLangInfo[0].szIso639LangCode, ulLen1, eLangId1, eType1);

	// 2nd Track :
	eType2 = pstAudioEs->astIsoLangInfo[1].eAudType;
	ulLen2 = HxSTD_StrLen(pstAudioEs->astIsoLangInfo[1].szIso639LangCode);
	if (3 == ulLen2)
	{
		eLangId2 = HLIB_LANG_639ToId ((const HCHAR *)pstAudioEs->astIsoLangInfo[1].szIso639LangCode);
	}

	HxLOG_Print("[Lang][1][%3s](%d) => [0x%x][eType2 : %d]\n", pstAudioEs->astIsoLangInfo[1].szIso639LangCode, ulLen2, eLangId2, eType2);

	bDualMono = FALSE;
	if (eLangId1 != eLangID_NONE && eLangId2 != eLangID_NONE)
	{
		if (eLangId1 != eLangId2)
		{
			bDualMono = TRUE;
		}
		else// if(eLangId1 == eLangId2)
		{
			// eLangId1 == eLangId2 인 경우임..
			if (eType1 != eType2)
			{
				bDualMono = TRUE;
			}
		}
	}

	//bDualMono = (eLangID_MAX != eLangId2 && eLangID_MAX != eLangId1) ? TRUE : FALSE;

	if (peLangId1) *peLangId1 = eLangId1;
	if (peLangId2) *peLangId2 = eLangId2;
	if (pbDualMono)	*pbDualMono = bDualMono;


	HxLOG_Print("eLangId1[0x%x] eLangId2[0x%x] \n", eLangId1, eLangId2);
	if(pbDualMono) HxLOG_Print("bDualMono [%d] \n", bDualMono);
	else	HxLOG_Print("bDualMono is unkwon \n");


	return bDualMono;
}
HBOOL svc_si_LibSvcEsGetAudioTypeInfo(const svcSi_AudioEs_t *pstAudioEs, AUDIO_Type_t *peAudioType1, AUDIO_Type_t *peAudioType2,HBOOL *pbDualMono)
{
	HxLANG_Id_e		 eLangId1, eLangId2;
	HUINT32			 ulLen1, ulLen2;
	AUDIO_Type_t	 eType1, eType2;
	HBOOL			 bDualMono;

	eLangId1 = eLangId2 = eLangID_NONE;
	eType1 = eType2 = eAUDIO_TYPE_UNDEFINED;

	// 1st Track : Check the language number:
	eType1 = pstAudioEs->astIsoLangInfo[0].eAudType;
	ulLen1 = HxSTD_StrLen(pstAudioEs->astIsoLangInfo[0].szIso639LangCode);
	if (3 == ulLen1)
	{
		eLangId1 = HLIB_LANG_639ToId ((const HCHAR *)pstAudioEs->astIsoLangInfo[0].szIso639LangCode);
	}

	HxLOG_Print("[Lang][0][%3s](%d) => [0x%x][eType1 : %d]\n", pstAudioEs->astIsoLangInfo[0].szIso639LangCode, ulLen1, eLangId1, eType1);

	// 2nd Track :
	eType2 = pstAudioEs->astIsoLangInfo[1].eAudType;
	ulLen2 = HxSTD_StrLen(pstAudioEs->astIsoLangInfo[1].szIso639LangCode);
	if (3 == ulLen2)
	{
		eLangId2 = HLIB_LANG_639ToId ((const HCHAR *)pstAudioEs->astIsoLangInfo[1].szIso639LangCode);
	}

	HxLOG_Print("[Lang][1][%3s](%d) => [0x%x][eType2 : %d]\n", pstAudioEs->astIsoLangInfo[1].szIso639LangCode, ulLen2, eLangId2, eType2);

	bDualMono = FALSE;
	if (eLangId1 != eLangID_NONE && eLangId2 != eLangID_NONE)
	{
		if (eLangId1 != eLangId2)
		{
			bDualMono = TRUE;
		}
		else// if(eLangId1 == eLangId2)
		{
			// eLangId1 == eLangId2 인 경우임..
			if (eType1 != eType2)
			{
				bDualMono = TRUE;
			}
		}
	}

	if (pbDualMono)		*pbDualMono = bDualMono;
	if (peAudioType1)	*peAudioType1 = eType1;
	if (peAudioType2)	*peAudioType2 = eType2;


	HxLOG_Print("eLangId1[0x%x] eLangId2[0x%x] \n", eLangId1, eLangId2);
	HxLOG_Print("eType1[0x%x] eType2[0x%x] \n", eType1, eType2);
	if(pbDualMono) HxLOG_Print("bDualMono [%d] \n", bDualMono);
	else	HxLOG_Print("bDualMono is unkwon \n");


	return bDualMono;
}
STATIC HBOOL svc_si_LibSvcEsFindAudioLanguage (const svcSi_AudioEs_t *pstAudioEs, HxLANG_Id_e eFindId, HBOOL bAd, svcSi_EsAudioFoundInfo_t *pstFoundInfo)
{
	HxLANG_Id_e 		 eLangIdSelected;
	HBOOL				 bLang1, bLang2;
	HBOOL				 bDualMono;
	DxDualMonoSelect_e	 eDualMonoSelect;
	HBOOL				 bFoundLang;

	if (NULL == pstFoundInfo)
		return FALSE;

	eLangIdSelected = eLangID_MAX;
	bLang1 = bLang2 = FALSE;
	bDualMono = FALSE;
	eDualMonoSelect = eDxDUALMONO_SELECT_UNKNOWN;
	bFoundLang = FALSE;

	// Get Selected Lang, Track Index (0 or 1) and Dual Mono(Flag and Dualmono Select)
	{
		HxLANG_Id_e 	 eLangId1, eLangId2;

		eLangId1 = eLangId2 = eLangID_MAX;

		HxLOG_Print("usPid[0x%x] ucComponentTag[0x%x] eAudCompType [%d] \n", pstAudioEs->usPid, pstAudioEs->ucComponentTag, pstAudioEs->eAudCompType);

		if (pstAudioEs->eAudCompType == eSI_AUD_CPNT_DUAL_MONO)
		{
			bDualMono = TRUE;
			(void)svc_si_LibSvcEsGetLangInfo(pstAudioEs, &eLangId1, &eLangId2, NULL);
		}
		else
		{
			(void)svc_si_LibSvcEsGetLangInfo(pstAudioEs, &eLangId1, &eLangId2, &bDualMono);
		}

		if (eLangID_NONE != eFindId && eLangID_MAX != eFindId)
		{
			bLang1 = (eFindId == eLangId1) ? TRUE : FALSE;
			bLang2 = (eFindId == eLangId2) ? TRUE : FALSE;

			if (bDualMono)
			{
				if (bLang1 == bLang2)
				{
					eLangIdSelected = eLangId1;

					if (FALSE == bAd)
					{
						if (pstAudioEs->astIsoLangInfo[0].eAudType == eAUDIO_TYPE_HEARING_IMPAIRED)
							eDualMonoSelect = eDxDUALMONO_SELECT_RIGHT;
						else if (pstAudioEs->astIsoLangInfo[1].eAudType == eAUDIO_TYPE_HEARING_IMPAIRED)
							eDualMonoSelect = eDxDUALMONO_SELECT_LEFT;
					}
 				}
				else
				{
					eLangIdSelected = (bLang1) ? eLangId1 : (bLang2) ? eLangId2 : eLangID_MAX;
					if (bDualMono)
						eDualMonoSelect = (bLang1) ? eDxDUALMONO_SELECT_LEFT : (bLang2) ? eDxDUALMONO_SELECT_RIGHT : eDxDUALMONO_SELECT_UNKNOWN;
				}
			}
			else
			{
				eLangIdSelected = eLangId1;
			}
 		}
		else if (eLangID_NONE == eFindId)	// Auto Select
		{
			if (eFindId == eLangId1)
			{
				bLang1 = TRUE;
			}

			eLangIdSelected = eLangId1;
			eDualMonoSelect = (bDualMono) ? eDxDUALMONO_SELECT_LEFT : eDxDUALMONO_SELECT_UNKNOWN;
		}

		bFoundLang = (TRUE == bDualMono) ? (bLang1 || bLang2) : bLang1;

		HxLOG_Print("[FindLang:%s]\n", (eFindId != eLangID_MAX) ? svcSi_LangIdTo639(eFindId) : "\"\"");
		HxLOG_Print("[bLang1:%d], [bLang2:%d], [SelectedLang:%s]\n", bLang1, bLang2, (eLangIdSelected != eLangID_MAX) ? svcSi_LangIdTo639(eLangIdSelected) : "\"\"");
		HxLOG_Print("[bDualMono:%d], [eDualMonoSelect:%d]\n", bDualMono, eDualMonoSelect);
	}

	pstFoundInfo->eDualMonoSelect	= eDualMonoSelect;
	pstFoundInfo->eLangId			= (bFoundLang) ? eLangIdSelected : eLangID_MAX;

	return bFoundLang;
}

STATIC HBOOL svc_si_LibSvcEsGetAudioDescriptionInfo (const svcSi_AudioEs_t *pstAudioEs, HBOOL *pbADBroadcast, HBOOL *pbADReceiver)
{
	HBOOL	bAdBm1	= SvcSi_ES_AUDIO_IS_AD_BM_TYPE(pstAudioEs->astIsoLangInfo[0].eAudType, pstAudioEs->ucMixType, pstAudioEs->eSuppAudioEditorial);
	HBOOL	bAdRm1	= SvcSi_ES_AUDIO_IS_AD_RM_TYPE(pstAudioEs->astIsoLangInfo[0].eAudType, pstAudioEs->ucMixType, pstAudioEs->eSuppAudioEditorial);
	HBOOL	bAdBm2	= SvcSi_ES_AUDIO_IS_AD_BM_TYPE(pstAudioEs->astIsoLangInfo[1].eAudType, pstAudioEs->ucMixType, pstAudioEs->eSuppAudioEditorial);
	HBOOL	bAdRm2	= SvcSi_ES_AUDIO_IS_AD_RM_TYPE(pstAudioEs->astIsoLangInfo[1].eAudType, pstAudioEs->ucMixType, pstAudioEs->eSuppAudioEditorial);
	HBOOL	bAdBm	= (bAdBm1 || bAdBm2);
	HBOOL	bAdRm	= (bAdRm1 || bAdRm2);

	HxLOG_Print("[bAdBm1:%d], [bAdBm2:%d], [bAdRm1:%d] [bAdRm2:%d]\n", bAdBm1, bAdBm2, bAdRm1, bAdRm2);

	HxLOG_Print("[IS_AD_BM:%s], [IS_AD_RM:%s]\n", bAdBm ? "Yes" : "No", bAdRm ? "Yes" : "No");

	if (pbADBroadcast)	*pbADBroadcast		= bAdBm;
	if (pbADReceiver)	*pbADReceiver		= bAdRm;

	return (bAdBm || bAdRm);
}

STATIC HINT32 svc_si_LibSvcEsGetAudioPriority (SvcSi_AudioPriorityType_e eType)
{
	SvcSi_EsTypePriority_t	*astTable;
	HUINT32					 ulNum, ulCount;

	if (eType >= eSvcSi_AUD_PRIORITY_TYPE_NUM)
	{
		HxLOG_Error("eType >= eSvcSi_AUD_PRIORITY_TYPE_NUM\n");
		return 0;
	}

	astTable = NULL;
	ulNum = 0;
	(void)svc_si_GetAudioPriorityTable(&astTable, &ulNum);
	if (NULL == astTable || 0 >= ulNum)
	{
		HxLOG_Error("NULL == astTable(%p) || 0 >= ulNum(%d)\n", astTable, ulNum);
		return 0;
	}

	for (ulCount=0; ulCount<ulNum; ulCount++)
	{
		if (astTable[ulCount].nPriorityType == eSvcSi_AUD_PRIORITY_TYPE_NUM)
			break;

		if (astTable[ulCount].nPriorityType == eType)
			return astTable[ulCount].nPriority;
	}

	return 0;
}

STATIC HCHAR* svc_si_LibSvcEsGetAudioPriorityTableItemName(SvcSi_AudioPriorityType_e eItem)
{
	switch (eItem)
	{
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_BASE);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_USER);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_USER_WITH_PID);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_0);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_1);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_OSD);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_UNDEFINED);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_ORIGINAL);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_LANG_DEFAULT);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_TYPE_UNDEFINED);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_TYPE_CLEANEFFECT);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_TYPE_HEARING_IMPAIRED);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_TYPE_VISUAL_IMPAIRED);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_USER);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_MENU);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_ON);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_BASE);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_USER);			// Not Implemented
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_MENU);			// Not Implemented
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_ON);		// Not Implemented
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_BASE);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_MULTICHANNEL_USER);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_MULTICHANNEL_MENU);						// Dolby
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_MULTICHANNEL_BASE);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_FIRST_TRACK);
	ENUM_TO_STR(eSvcSi_AUD_PRIORITY_DUAL_MONO);
	default:
		break;
	}
	return "";
}

void svc_si_LibSvcEsPrintAudioPriorityTable(void)
{
	HUINT32	ulCount;

	HxLOG_Print("=================================================================================\n");
	HxLOG_Print("=== Audio Priority Table ========================================================\n");
	HxLOG_Print("=================================================================================\n");
	for (ulCount=0; ulCount<eSvcSi_AUD_PRIORITY_TYPE_NUM; ulCount++)
	{
		SvcSi_AudioPriorityType_e ePriorityType = (SvcSi_AudioPriorityType_e)ulCount;
		HxLOG_Print("  [%-60s] : [%08d]\n", svc_si_LibSvcEsGetAudioPriorityTableItemName(ePriorityType),
											svc_si_LibSvcEsGetAudioPriority(ePriorityType));
	}
	HxLOG_Print("=================================================================================\n");

}

STATIC HINT32 svc_si_LibSvcEsGetLangScore (svcSi_EsMatch_t *astMatchedList)
{
	HINT32	nTemp;

	nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_BASE);
	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_OSD))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_OSD));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_1))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_1));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_0))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_0));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_USER))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_USER));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_USER_WITH_PID))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_USER_WITH_PID));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_UNDEFINED))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_UNDEFINED));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_ORIGINAL))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_ORIGINAL));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_DEFAULT))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_LANG_DEFAULT));
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsGetAdBroadcasterMixedScore (svcSi_EsMatch_t *astMatchedList, svcSi_EsAudioProperty_t *pstProperty)
{
	HINT32	nTemp = 0;

//	if (pstProperty->bAdBroadcasterMixed)
#if 0
	{
		nTemp = 0;

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU))
		{
			CHECK;
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_MENU));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER))
		{
			CHECK;
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_USER));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON))
		{
			CHECK;
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_ON));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF))
		{
			CHECK;
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_OFF));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF))
		{
			CHECK;
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_BROADCASTER_MIX_OFF));
		}

	}
#else
	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER))
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_USER);
	}
	else if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU))
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_MENU);
	}
	else if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON))
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_ON);
	}
	else if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF))
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_BROADCASTER_MIX_OFF);
	}
	else if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF))
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_BROADCASTER_MIX_OFF);
	}
	else
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_BROADCASTER_MIX_BASE);
	}
#endif

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsGetAdReceiverMixedScore (svcSi_EsMatch_t *astMatchedList, svcSi_EsAudioProperty_t *pstProperty)
{
	HINT32	nTemp = 0;

#if 0
//	if (pstProperty->bAdReceiverMixed)
	{
		nTemp = 0;

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU))
		{
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_MENU));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER))
		{
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_USER));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF))
		{
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_RECEIVER_MIX_OFF));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON))
		{
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_ON));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF))
		{
			CHECK;
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_OFF));
		}
	}
#else
		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER))
		{
			nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_USER);
		}
		else  if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU))
		{
			nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_MENU);
		}
		else if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON))
		{
			nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_ON);
		}
		else if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF))
		{
			nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_ON_RECEIVER_MIX_OFF);
		}
		else if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF))
		{
			nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_OFF_RECEIVER_MIX_OFF);
		}
		else
		{
			nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DESCRIPTION_RECEIVER_MIX_BASE);
		}
#endif

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsGetMultichannelScore (svcSi_EsMatch_t *astMatchedList, svcSi_EsAudioProperty_t *pstProperty)
{
	HINT32 nTemp = 0;

	if (pstProperty->bMultiChannel)
	{
		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_MULTICHANNEL_USER))
		{
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_MULTICHANNEL_USER));
		}

		if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_MULTICHANNEL_MENU))
		{
			nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_MULTICHANNEL_MENU));
		}

		if (nTemp == 0)
		{
			nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_MULTICHANNEL_BASE);
		}
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsGetAudioTypeScore (svcSi_EsMatch_t *astMatchedList)
{
	HINT32 nTemp = 0;

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_TYPE_UNDEFINED))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_TYPE_UNDEFINED));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_TYPE_CLEANEFFECT))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_TYPE_CLEANEFFECT));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_TYPE_HEARING_IMPAIRED))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_TYPE_HEARING_IMPAIRED));
	}

	if (svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_TYPE_VISUAL_IMPAIRED))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_TYPE_VISUAL_IMPAIRED));
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsGetFirstTrackScore (svcSi_EsAudioProperty_t *pstProperty)
{
	HINT32 nTemp = 0;

	if (pstProperty->bFirstTrack)
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_FIRST_TRACK);
	}

	return nTemp;
}


STATIC HINT32 svc_si_LibSvcEsGetNormalAudioScore (svcSi_EsAudioProperty_t *pstProperty)
{
	HINT32 nTemp = 0;

	if (pstProperty->bNormaiAudio)
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_NORMAL_AUDIO);
	}

	return nTemp;
}


STATIC HINT32 svc_si_LibSvcEsGetDualMonoScore (svcSi_EsAudioProperty_t *pstProperty)
{
	HINT32 nTemp = 0;

	if (pstProperty->bDualMono)
	{
		nTemp = svc_si_LibSvcEsGetAudioPriority(eSvcSi_AUD_PRIORITY_DUAL_MONO);
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsSetAudioPriority (svcSi_EsAudioCandidate_t	*pstCandidate, HBOOL bAdBroadcasterMixed)
{
	HINT32						 nScoreLang, nScoreMultichannel, nScoreAudioDescription, nScoreTrack, nScoreDualMono, nScoreAudioType, nScoreNormalAudio;
	svcSi_EsMatch_t				*astMatchedList = svcSi_EsAudSelGetMatchedList(pstCandidate);
	svcSi_EsAudioProperty_t		*pstProperty = svcSi_EsAudSelGetEsProperty(pstCandidate);

//	svc_si_LibSvcEsPrintAudioPriorityTable();

	nScoreLang = nScoreMultichannel = nScoreAudioDescription = nScoreTrack = nScoreDualMono = 0;


	// Firt Track Num
	nScoreTrack = svc_si_LibSvcEsGetFirstTrackScore(pstProperty);

	// Normal type audio
	nScoreNormalAudio = svc_si_LibSvcEsGetNormalAudioScore(pstProperty);

	// Dual Mono
	nScoreDualMono = svc_si_LibSvcEsGetDualMonoScore(pstProperty);
	// Language
	nScoreLang = svc_si_LibSvcEsGetLangScore(astMatchedList);
	// Audio Description
	if (bAdBroadcasterMixed)	nScoreAudioDescription = svc_si_LibSvcEsGetAdBroadcasterMixedScore(astMatchedList, pstProperty);
	else						nScoreAudioDescription = svc_si_LibSvcEsGetAdReceiverMixedScore(astMatchedList, pstProperty);
	// Multichannel
	nScoreMultichannel = svc_si_LibSvcEsGetMultichannelScore(astMatchedList, pstProperty);

	nScoreAudioType = svc_si_LibSvcEsGetAudioTypeScore(astMatchedList );

	pstCandidate->nPriority = nScoreLang + nScoreMultichannel + nScoreAudioDescription + nScoreTrack + nScoreDualMono + nScoreAudioType+ pstProperty->nAlternativePriority + nScoreNormalAudio;

	HxLOG_Print("[%02d] = { [Language : %06d], [Multi Channel : %06d], [Audio Desc : %06d], [First Track : %06d], [Normal Audio : %06d], [Dual Mono : %06d]  [nScoreAudioType : %06d] [Alt Priority : %06d]} Total [%06d]\n",
		pstProperty->nIndex, nScoreLang, nScoreMultichannel, nScoreAudioDescription, nScoreTrack, nScoreNormalAudio, nScoreDualMono, nScoreAudioType, pstProperty->nAlternativePriority, pstCandidate->nPriority);
//	HxLOG_Print("[Language : %06d],", nScoreLang);
//	HxLOG_Print("[Multi Channel : %06d],", nScoreMultichannel);
//	HxLOG_Print("[Audio Desc : %06d],", nScoreAudioDescription);
//	HxLOG_Print("[First Track : %06d],", nScoreTrack);
//	HxLOG_Print("[Dual Mono : %06d]\n", nScoreDualMono);
//	HxLOG_Print("[Alt Priority : %06d]} Total [%06d]\n", pstProperty->nAlternativePriority, pstCandidate->nPriority);

	return pstCandidate->nPriority;
}

STATIC void svc_si_LibSvcEsSetDualMono(svcSi_EsAudioProperty_t *pstProperty, const svcSi_AudioEs_t *pstAudioEs)
{
	HBOOL		bDualMono;

	// Set Dual Mono
	bDualMono = FALSE;
	(void)svc_si_LibSvcEsGetLangInfo(pstAudioEs, NULL, NULL, &bDualMono);
	if (bDualMono)
	{
		pstProperty->bDualMono = TRUE;
	}
}

STATIC void svc_si_LibSvcEsSetMultiChannel(svcSi_EsAudioCandidate_t 			*pstCandidate,
												const svcSi_AudioEs_t			*pstAudioEs,
												const SvcSi_CfgAudioOutput_t	*pstAudioOutput)
{
	HBOOL	bMultiChannelEs;
	HBOOL	bDolbyEs;

	//	1. Check Multichannel Audio
	bMultiChannelEs = svc_si_LibSvcEsIsAudioTrackMultiChannel(pstAudioEs);
	(void)bMultiChannelEs;

	//	2. Check Dolby Audio or Stereo
	bDolbyEs = svc_si_LibSvcEsIsAudioTrackDolby(pstAudioEs);

	if (bDolbyEs)
	{
		svcSi_EsMatch_t				*astMatchedList = svcSi_EsAudSelGetMatchedList(pstCandidate);
		svcSi_EsAudioProperty_t 	*pstProperty = svcSi_EsAudSelGetEsProperty(pstCandidate);

		// Set Selected Reasons : Multichannel User
		if (svc_si_LibSvcEsGetAudioOutputConfig(pstAudioOutput, eSvcSi_CFG_AUD_OUTPUT_Multichannel_User))
		{
			(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_MULTICHANNEL_USER, TRUE);
		}

		// Set Selected Reasons : Multichannel Menu
		if (svc_si_LibSvcEsGetAudioOutputConfig(pstAudioOutput, eSvcSi_CFG_AUD_OUTPUT_Multichannel_Menu))
		{
			(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_MULTICHANNEL_MENU, TRUE);
		}

		pstProperty->bMultiChannel = TRUE;
	}
}

STATIC void svc_si_LibSvcEsSetLang(svcSi_EsAudioCandidate_t 			*pstCandidate,
										const svcSi_AudioEs_t			*pstAudioEs,
										const HxLANG_Id_e				 eOsdLang,
										const SvcSi_CfgAudioLang_t		*pstAudioLang,
										const SvcSi_CfgAudioDesc_t 		*pstAudioDesc)
{
	HUINT32						 ulCount;
	svcSi_EsAudioFoundInfo_t	 stExtInfo, stExtInfoTemp;
	HBOOL						 bFoundLang, bFoundLangOnce;
	svcSi_EsMatch_t				*astMatchedList = svcSi_EsAudSelGetMatchedList(pstCandidate);
	svcSi_EsAudioMatchedInfo_t	*pstMatched = svcSi_EsAudSelGetMatchedInfo(pstCandidate);
//	svcSi_EsAudioProperty_t		*pstProperty = svcSi_EsAudSelGetEsProperty(pstCandidate);
	HBOOL						 bAdUser = svc_si_LibSvcEsGetAudioDescriptionConfig(pstAudioDesc, eSvcSi_CFG_AUD_DESCRIPTION_User);
	HBOOL						 bAdMenu = svc_si_LibSvcEsGetAudioDescriptionConfig(pstAudioDesc, eSvcSi_CFG_AUD_DESCRIPTION_Menu);
	HBOOL						 bAd = bAdUser || bAdMenu;
	HINT32						 nPriority = 0, nPriorityNow = 0;

	//	A Audio Track has one or two Languages.
	// User, Subtitle Lang1 and Lang2, Undefined, Original, Default
	bFoundLang = FALSE;
	bFoundLangOnce = FALSE;
	HxSTD_MemSet(&stExtInfo, 0x00, sizeof(svcSi_EsAudioFoundInfo_t));
	HxSTD_MemSet(&stExtInfoTemp, 0x00, sizeof(svcSi_EsAudioFoundInfo_t));

	for (ulCount = 0; ulCount < eSvcSi_CFG_AUD_LANG_NUM ; ulCount++)
	{
		HxLANG_Id_e 	eLangId = pstAudioLang->aeLangId[ulCount];

		if (eLangID_MAX == eLangId)
		{
			continue;
		}

		bFoundLang = svc_si_LibSvcEsFindAudioLanguage (pstAudioEs, eLangId, bAd, &stExtInfoTemp);

		if (eLangID_NONE == eLangId)
		{
			if (pstAudioLang->nPid != pstAudioEs->usPid)
			{
				continue;
			}
		}

		if (bFoundLang)
		{
			SvcSi_EsAudioSelType_e	eSelType;
		
			bFoundLangOnce = TRUE;
			eSelType = svc_si_LibEsAudioGetSelTypeFromLangConfiguration((SvcSi_CfgAudioLang_e)ulCount);
			nPriority = svc_si_LibSvcEsGetAudioPriority(eSelType);
			if (nPriority > nPriorityNow)
			{
				HxLOG_Print("eSelType : %d, nPriority : %d\n", eSelType, nPriority);
				HxSTD_memcpy(&stExtInfo, &stExtInfoTemp, sizeof(svcSi_EsAudioFoundInfo_t));
				nPriorityNow = nPriority;
			}

			if ((eSelType == eSvcSi_AUD_SEL_LANG_USER_WITH_PID) && (pstAudioLang->nPid == pstAudioEs->usPid))
			{
				// eSvcSi_AUD_SEL_LANG_USER_WITH_PID는 PID가 함께 일치 해야 한다.
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSelType, TRUE);
			}
			else if (eSelType != eSvcSi_AUD_SEL_LANG_USER_WITH_PID)
			{
				// eSvcSi_AUD_SEL_LANG_USER_WITH_PID 조건을 제외한 나머지 일치 확인.
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSelType, TRUE);
			}
		}
	}


	// Find OSD Lang
	if (!bFoundLangOnce && ((eLangID_MAX != eOsdLang) && (eLangID_NONE != eOsdLang)))
	{
		HxSTD_MemSet(&stExtInfo, 0x00, sizeof(svcSi_EsAudioFoundInfo_t));
		bFoundLang = svc_si_LibSvcEsFindAudioLanguage (pstAudioEs, eOsdLang, bAd, &stExtInfo);
		if (bFoundLang)
		{
			(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_LANG_OSD, TRUE);
		}
	}

	HxLOG_Print("[bFoundLang:%d] [bFoundLangOnce:%d]\n",bFoundLang, bFoundLangOnce);
	if (bFoundLangOnce)
	{
		pstMatched->eDualMonoSelect	= stExtInfo.eDualMonoSelect;
		pstMatched->eLangId			= stExtInfo.eLangId;
	}
	else
	{
		pstMatched->eDualMonoSelect	= stExtInfoTemp.eDualMonoSelect;
		pstMatched->eLangId			= stExtInfoTemp.eLangId;
	}

	HxLOG_Print("[eDualMonoSelect:%d][Lang:%s]\n", pstMatched->eDualMonoSelect, svcSi_LangIdTo639(pstMatched->eLangId));
}

STATIC void svc_si_LibSvcEsSetAD(svcSi_EsAudioCandidate_t 			*pstCandidate,
									const svcSi_AudioEs_t			*pstAudioEs,
									const SvcSi_CfgAudioDesc_t		*pstAudioDesc)
{
	svcSi_EsMatch_t				*astMatchedList = svcSi_EsAudSelGetMatchedList(pstCandidate);
	svcSi_EsAudioProperty_t		*pstProperty = svcSi_EsAudSelGetEsProperty(pstCandidate);
	HBOOL						 bADBroadcast, bADReceiver;

	// Set Selected Flag for Audio Description

	bADBroadcast = bADReceiver = FALSE;

	(void)svc_si_LibSvcEsGetAudioDescriptionInfo(pstAudioEs, &bADBroadcast, &bADReceiver);
//	HxLOG_Print("[%s] bADBroadcast:%d, bADReceiver:%d] \n", __FUNCTION__, bADBroadcast, bADReceiver);
//	if (bADBroadcast || bADReceiver)
	{
		HBOOL	bAdUser = svc_si_LibSvcEsGetAudioDescriptionConfig(pstAudioDesc, eSvcSi_CFG_AUD_DESCRIPTION_User);
		HBOOL	bAdMenu = svc_si_LibSvcEsGetAudioDescriptionConfig(pstAudioDesc, eSvcSi_CFG_AUD_DESCRIPTION_Menu);

		HxLOG_Print("[%s] Config : bAdUser:(%s), bAdMenu:(%s)] \n", __FUNCTION__, bAdUser ? "ON" : "OFF", bAdMenu ? "ON" : "OFF");

		// AD On
		if (bAdUser || bAdMenu)
		{
			//HxLOG_Print("[%s] [bAdUser:%d], [bAdMenu:%d] \n",  __FUNCTION__, bAdUser, bAdMenu);
			if (bADBroadcast)
			{
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER, bAdUser);
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU, bAdMenu);

				// Matched가 아니더라도 Audio Description 은 Candidate로 넣는다.
				pstProperty->bAdBroadcasterMixed = (bAdUser || bAdMenu);
			}
			else
			{
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF, TRUE);
			}

			if (bADReceiver)
			{
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER, bAdUser);
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU, bAdMenu);

				// Matched가 아니더라도 Audio Description 은 Candidate로 넣는다.
				pstProperty->bAdReceiverMixed = (bAdUser || bAdMenu);
			}
			else
			{
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF, TRUE);
			}
		}
		// AD Off
		else
		{
			if (bADBroadcast)
			{
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON, TRUE);
			}
			else
			{
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF, TRUE);
			}

			if (bADReceiver)
			{
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON, TRUE);
			}
			else
			{
				(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF, TRUE);
			}

			pstProperty->bAdBroadcasterMixed	= bADBroadcast;
			pstProperty->bAdReceiverMixed		= bADReceiver;
		}

		HxLOG_Print("[%s] AD_BM_U:[%d], AD_BM_M:[%d], AD_RM_U:[%d], AD_RM_M:[%d], AD_BM_OFF:[%d], AD_RM_OFF:[%d]\n", __FUNCTION__,
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_USER),
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_BROADCASTER_MIX_MENU),
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_USER),
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_RECEIVER_MIX_MENU),
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_ON_BROADCASTER_MIX_OFF),
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_ON_RECEIVER_MIX_OFF));
		HxLOG_Print("[%s] AD_OFF_BM_ON:[%d], AD_OFF_RM_ON:[%d], AD_OFF_BM_OFF:[%d], AD_OFF_RM_OFF:[%d]\n", __FUNCTION__,
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_ON),
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_ON),
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_BROADCASTER_MIX_OFF),
			svc_si_LibEsAudioGetMatched(astMatchedList, eSvcSi_AUD_SEL_DESCRIPTION_OFF_RECEIVER_MIX_OFF));

	}
}

STATIC void svc_si_LibSvcEsSetAudioType(svcSi_EsAudioCandidate_t *pstCandidate,const svcSi_AudioEs_t *pstAudioEs)
{
	HBOOL		bDualMono;
	AUDIO_Type_t	eAudioType1,eAudioType2;

	svcSi_EsMatch_t				*astMatchedList = svcSi_EsAudSelGetMatchedList(pstCandidate);

	// Set Dual Mono
	bDualMono = FALSE;
	eAudioType1 = eAudioType2 = eAUDIO_TYPE_UNDEFINED;
	(void)svc_si_LibSvcEsGetAudioTypeInfo(pstAudioEs, &eAudioType1, &eAudioType2, &bDualMono);
	if (bDualMono == FALSE)
	{
		if(eAudioType1 == eAUDIO_TYPE_UNDEFINED)
		{
			(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_TYPE_UNDEFINED, TRUE);

		}
		else if(eAudioType1 == eAUDIO_TYPE_CLEAN_EFFECTS)
		{
			(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_TYPE_CLEANEFFECT, TRUE);

		}
		else if(eAudioType1 == eAUDIO_TYPE_HEARING_IMPAIRED)
		{
			(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_TYPE_HEARING_IMPAIRED, TRUE);

		}
		else if(eAudioType1 == eAUDIO_TYPE_VISUAL_IMPAIRED)
		{
			(void)svc_si_LibEsAudioSetMatched(astMatchedList, eSvcSi_AUD_SEL_TYPE_VISUAL_IMPAIRED, TRUE);

		}
		else
		{
		}

	}
}

STATIC HBOOL svc_si_LibSvcEsAddToCandidateList(HxVector_t					*pstCandidateList,
												svcSi_EsAudioCandidate_t 	*pstCandidate)
{
	// 1. Set Priority
	if (svc_si_LibSvcEsSetAudioPriority(pstCandidate, TRUE) > 0)
	{
	// 2. Clone Item
		svcSi_EsAudioCandidate_t *pstClone = (svcSi_EsAudioCandidate_t *)HLIB_STD_MemDup(pstCandidate, sizeof(svcSi_EsAudioCandidate_t));
		if (pstClone)
		{
			HERROR	hErr;
	// 3. Add to Candidate List
			hErr = HLIB_VECTOR_Add(pstCandidateList, pstClone);
			return (ERR_OK == hErr) ? TRUE : FALSE;
		}
	}

	return FALSE;
}

STATIC HBOOL svc_si_LibSvcEsSetAdReceiverMixedPriorityCb(void *pvData, void *pvUserData)
{
	svcSi_EsAudioCandidate_t	*pstCandidate = (svcSi_EsAudioCandidate_t *)pvData;

	if (NULL == pstCandidate)
		return FALSE;

	if (pstCandidate->stProperty.bAdBroadcasterMixed)
		pstCandidate->nPriority = 0;
	else
		pstCandidate->nPriority = svc_si_LibSvcEsSetAudioPriority(pstCandidate, FALSE);

	return FALSE;
}


STATIC HERROR svc_si_LibSvcEsGetTopPriorityMainIndex(HxVector_t *pstCandidateList, HINT32 *pnIndex, DxDualMonoSelect_e *peDualMono, HxLANG_Id_e *peLangId)
	{
	HERROR						 hErr = ERR_FAIL;
	svcSi_EsAudioCandidate_t	*pstCandidate;
	HINT32						 i, nItemNum;

	*pnIndex = -1;
	*peDualMono = eDxDUALMONO_SELECT_UNKNOWN;
	*peLangId = eLangID_MAX;


	nItemNum = HLIB_VECTOR_Length(pstCandidateList);

	// Sort : descending
	(void)HLIB_VECTOR_Sort(pstCandidateList, svc_si_LibCompareAudioPriority);

#if defined(CONFIG_DEBUG)
	svc_si_LibSvcEsPrintCandidateList(pstCandidateList, svc_si_LibSvcEsPrintAudioCandidateItemCb);
#endif

	for (i=0; i<nItemNum; i++)
	{
		pstCandidate = (svcSi_EsAudioCandidate_t *)HLIB_VECTOR_ItemAt(pstCandidateList, i);

		// Get 1st Item : Top Priority
		if (pstCandidate)
		{
			svcSi_EsAudioMatchedInfo_t		*pstMatched = svcSi_EsAudSelGetMatchedInfo(pstCandidate);
			svcSi_EsAudioProperty_t			*pstProperty = svcSi_EsAudSelGetEsProperty(pstCandidate);

			if (!pstProperty->bAdReceiverMixed)
			{
				HxLOG_Print("Top priority Main audio [Index : %d], [Pid : 0x%x], [nPriority:%d]\n", pstProperty->nIndex, pstProperty->usPid, pstCandidate->nPriority);
				// Dual Mono
				*peDualMono = pstMatched->eDualMonoSelect;
				// Main Audio
				*pnIndex = pstProperty->nIndex;
				// Lang Id
				*peLangId = pstMatched->eLangId;
				hErr = ERR_OK;
				break;
			}
		}
	}

	return hErr;
}

STATIC HERROR svc_si_LibSvcEsGetTopPriorityAudioDescriptionIndex(HxVector_t *pstCandidateList, HINT32 *pnIndex, HxLANG_Id_e *peLangId)
{
	HERROR						 hErr = ERR_FAIL;
	HUINT32 					 ulCount;
	svcSi_EsAudioCandidate_t	*pstCandidate;
	HUINT32 	 				 ulItemNum = 0;

	*pnIndex = -1;
	*peLangId = eLangID_MAX;

	// Set Priority : Audio Description Receiver Mixed
	(void)HLIB_VECTOR_ForEach(pstCandidateList, svc_si_LibSvcEsSetAdReceiverMixedPriorityCb, NULL);

	// Sort : descending
	(void)HLIB_VECTOR_Sort(pstCandidateList, svc_si_LibCompareAudioPriority);


	ulItemNum = HLIB_VECTOR_Length(pstCandidateList);

	// Get Audio Description (Receiver Mixed) Index
	for (ulCount = 0; ulCount<ulItemNum; ulCount++)
	{
		pstCandidate = (svcSi_EsAudioCandidate_t *)HLIB_VECTOR_ItemAt(pstCandidateList, ulCount);
		if (pstCandidate)
		{
			svcSi_EsAudioMatchedInfo_t		*pstMatched = svcSi_EsAudSelGetMatchedInfo(pstCandidate);
			svcSi_EsAudioProperty_t			*pstProperty = svcSi_EsAudSelGetEsProperty(pstCandidate);

			if (pstProperty->bAdReceiverMixed)
			{
				if (svc_si_LibEsAudioIsAudioDescriptionReceiverMixed(pstMatched->astMatchedList))
				{
					HxLOG_Print("Top priority Aux audio [Index : %d], [Pid : 0x%x], [nPriority:%d]\n", pstProperty->nIndex, pstProperty->usPid, pstCandidate->nPriority);

					// Aux Audio
					*pnIndex = pstProperty->nIndex;
					// Lang Id
					*peLangId = pstMatched->eLangId;
					hErr = ERR_OK;
					break;
				}
			}
		}
	}

	return hErr;
}

STATIC HERROR svc_si_LibSvcEsGetTopPriorityEsInfo(HxVector_t				*pstCandidateList,
													SvcSi_SelectedEs_t		*pstSelected)
{
	HERROR		 hErr = ERR_FAIL;

	if (HLIB_VECTOR_Length(pstCandidateList) > 0)
	{
		// Get Main Audio Index and DualMono Select
		(void)svc_si_LibSvcEsGetTopPriorityMainIndex(pstCandidateList, &pstSelected->nIndexMain, (DxDualMonoSelect_e *)&pstSelected->ulParam1, &pstSelected->eLangIdMain);
		// Get Audio Description (Receiver Mixed) Index
		(void)svc_si_LibSvcEsGetTopPriorityAudioDescriptionIndex(pstCandidateList, &pstSelected->nIndexAux, &pstSelected->eLangIdAux);
		hErr = ERR_OK;
	}

	return hErr;
}

STATIC HBOOL svc_si_LibSvcEsCheckUpdated(SvcSi_SelectedEs_t *pstSelected)
{
	if (-1 != pstSelected->nIndexMain)												return TRUE;
	if (-1 != pstSelected->nIndexAux)												return TRUE;
	if (eDxDUALMONO_SELECT_UNKNOWN != (DxDualMonoSelect_e)pstSelected->ulParam1)	return TRUE;
	if (eLangID_MAX != pstSelected->eLangIdMain)									return TRUE;
	if (eLangID_MAX != pstSelected->eLangIdAux)										return TRUE;

	return FALSE;
}

STATIC HERROR svc_si_LibSvcEsGetSelectedAudioInfoFromCandidateList(SvcSi_SelectedEs_t				*pstSelected,
																	HxVector_t						*pstCandidateList,
																	const SvcSi_CfgAudioOutput_t	*pstAudioOutput)
{
	HERROR	hErr;

	// Reset
	svc_si_LibSvcEsInitAudioSelection(pstSelected);

	// Get Top Priority Audio Info
	hErr = svc_si_LibSvcEsGetTopPriorityEsInfo(pstCandidateList, pstSelected);
	if (ERR_OK == hErr)
	{
		if (svc_si_LibSvcEsCheckUpdated(pstSelected))
		{
			pstSelected->bFound			= TRUE;
		}
	}

	return hErr;
}

HERROR svc_si_LibSvcEsSelectAudio (const svcSi_PmtEsInfo_t *pstPmtEsInfo, const SvcSi_Config_t *pstConfig, SvcSi_SelectedEs_t *pstSelectedAudio)
{
	HERROR							 hErr;
	HINT32							 nAudioCnt;
	HBOOL							 bAdded;
	svcSi_AudioEs_t					*pstAudioEs;
	HxList_t						*pstItem;
	HxVector_t						*pstCandidateList;
	svcSi_EsAudioCandidate_t		 stCandidate;
//	svcSi_EsAudioMatchedInfo_t		*pstMatched;
	svcSi_EsAudioProperty_t			*pstProperty;
	HBOOL							 bFirstTrack;

	if ((NULL == pstPmtEsInfo) || (NULL == pstConfig))
	{
		HxLOG_Error("Source NULL : (NULL == pstPmtEsInfo) || (NULL == pstConfig)\n");
		return ERR_FAIL;
	}

	if (NULL == pstSelectedAudio)
	{
		HxLOG_Error("Target NULL : (NULL == pstSelectedAudio)\n");
		return ERR_FAIL;
	}

	// ==========================================================
	// Init Audio Selection
	svc_si_LibSvcEsInitAudioSelection(pstSelectedAudio);


	// Init Audio Candidate List
	pstCandidateList = HLIB_VECTOR_NewEasy(svcSi_MAX_AUD_CANDIDATE, (HxFreeFunc_t)HLIB_STD_MemFree_CB, (HxCompareFunc_t)svc_si_LibCompareAudioPriority);
	if (NULL == pstCandidateList)
	{
		HxLOG_Error("0 == HLIB_LIST_Length(pstPmtEsInfo->pstAudioTrack)\n");
		return ERR_FAIL;
	}
	// ==========================================================

#if defined(CONFIG_DEBUG)
	svc_si_LibSvcEsPrintEsList(pstPmtEsInfo->pstAudioTrack, svc_si_LibSvcEsPrintAudioEsItem2);
	svc_si_LibSvcEsPrintConfig(pstConfig, svcSi_ConfigPrintSystem | svcSi_ConfigPrintAudio);
#endif

	// Make Candidate List
	bFirstTrack = TRUE;
	pstItem = pstPmtEsInfo->pstAudioTrack;
	for (nAudioCnt = 0; NULL != pstItem; pstItem = HLIB_LIST_Next(pstItem), nAudioCnt++)
	{
		pstAudioEs = (svcSi_AudioEs_t *)HLIB_LIST_Data(pstItem);

		// Ignore track
		if ((NULL == pstAudioEs) || (PID_NULL == pstAudioEs->usPid))		{ continue; }

		// Init Selection
		svc_si_LibEsAudioResetCandidate(&stCandidate);
		pstProperty		= svcSi_EsAudSelGetEsProperty(&stCandidate);

		/* ===================================================================
		 *	1. Set Basic ES Info to Flag
		 * =================================================================== */
		pstProperty->nIndex		= nAudioCnt;											// Index
		pstProperty->usPid		= pstAudioEs->usPid;									// PID
		pstProperty->eCodec		= pstAudioEs->eAudCodec;								// Codec
		svc_si_LibSvcEsSetDualMono(pstProperty, (const svcSi_AudioEs_t *)pstAudioEs);	// Dual Mono Flag
		if (bFirstTrack)																// First Track Flag
		{
			pstProperty->bFirstTrack = TRUE;
			bFirstTrack = FALSE;
		}
		if (pstAudioEs->astIsoLangInfo[0].eAudType == eAUDIO_TYPE_UNDEFINED)
		{
			pstProperty->bNormaiAudio = TRUE;
		}

		/* ===================================================================
		 *	2. Check Multichannel Audio
		 * =================================================================== */
		svc_si_LibSvcEsSetMultiChannel(&stCandidate,
										(const svcSi_AudioEs_t			*)pstAudioEs,
										(const SvcSi_CfgAudioOutput_t	*)&pstConfig->stAudioOutput);

		/* ===================================================================
		 *	3. Check Language
		 * =================================================================== */
		svc_si_LibSvcEsSetLang(&stCandidate,
										(const svcSi_AudioEs_t			*)pstAudioEs,
										(const HxLANG_Id_e)pstConfig->stSystem.eOsdLang,
										(const SvcSi_CfgAudioLang_t		*)&pstConfig->stAudioLanguage,
										(const SvcSi_CfgAudioDesc_t 	*)&pstConfig->stAudioDesc);

		/* ===================================================================
		 *	4. Check Audio Description
		 * =================================================================== */
		svc_si_LibSvcEsSetAD(&stCandidate,
										(const svcSi_AudioEs_t			*)pstAudioEs,
										(const SvcSi_CfgAudioDesc_t 	*)&pstConfig->stAudioDesc);

		/* ===================================================================
		 *	5. Check Audio Description
		 * =================================================================== */
		svc_si_LibSvcEsSetAudioType(&stCandidate,(const svcSi_AudioEs_t			*)pstAudioEs);

		/* ===================================================================
		 *	6. Set Priority and Add to Candidate List
		 * =================================================================== */
		bAdded = svc_si_LibSvcEsAddToCandidateList(pstCandidateList, &stCandidate);
		if (bAdded)
		{
//			svc_si_LibSvcEsPrintAudioCandidateItem(&stCandidate, HLIB_VECTOR_Length(pstCandidateList), TRUE);
		}
	}

//	svc_si_LibSvcEsPrintCandidateList(pstCandidateList, svc_si_LibSvcEsPrintAudioCandidateItemCb);

	/* ===================================================================
	 *	6. Get Top Priority Audio Track and Extract Information for Audio Play
	 * =================================================================== */
	hErr = svc_si_LibSvcEsGetSelectedAudioInfoFromCandidateList(pstSelectedAudio,
															pstCandidateList,
															(const SvcSi_CfgAudioOutput_t *)&pstConfig->stAudioOutput);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("svc_si_LibSvcEsGetSelectedAudioInfoFromCandidateList failed. \n");
	}

	/* ===================================================================
	 *	7. Tear Down
	 * =================================================================== */
	(void)HLIB_VECTOR_Delete(pstCandidateList);
	pstCandidateList = NULL;
	return ERR_OK;
}

#define _____SELECT_SUBTITLE_____

#if defined(WIN32)
STATIC svcSvc_SubtitleTypeTable_t s_astSubtitleTypeTable[] =
{
	{	0x01,	FALSE,		FALSE, 	eSvcSi_SUBTITLE_AR_NA, 		eSvcSi_SUBTITLE_HD_NA	},
	{	0x02, 	FALSE,		FALSE,	eSvcSi_SUBTITLE_AR_NA,		eSvcSi_SUBTITLE_HD_NA	},
	{	0x03, 	FALSE,		FALSE,	eSvcSi_SUBTITLE_AR_NA,		eSvcSi_SUBTITLE_HD_NA	},
	{	0x04, 	FALSE,		FALSE,	eSvcSi_SUBTITLE_AR_NA,		eSvcSi_SUBTITLE_HD_NA	},
	{	0x05, 	FALSE,		TRUE,	eSvcSi_SUBTITLE_AR_NA,		eSvcSi_SUBTITLE_HD_NA	},

	{	0x10, 	TRUE,		FALSE,	eSvcSi_SUBTITLE_AR_NO,		eSvcSi_SUBTITLE_HD_NO	},
	{	0x11, 	TRUE,		FALSE,	eSvcSi_SUBTITLE_AR_4x3,		eSvcSi_SUBTITLE_HD_NO	},
	{	0x12, 	TRUE,		FALSE,	eSvcSi_SUBTITLE_AR_16x9,	eSvcSi_SUBTITLE_HD_NO	},
	{	0x13, 	TRUE,		FALSE,	eSvcSi_SUBTITLE_AR_221x1,	eSvcSi_SUBTITLE_HD_NO	},
	{	0x14, 	TRUE,		FALSE,	eSvcSi_SUBTITLE_AR_NA,		eSvcSi_SUBTITLE_HD_YES	},

	{	0x20, 	TRUE,		TRUE,	eSvcSi_SUBTITLE_AR_NO,		eSvcSi_SUBTITLE_HD_NO	},
	{	0x21, 	TRUE,		TRUE,	eSvcSi_SUBTITLE_AR_4x3,		eSvcSi_SUBTITLE_HD_NO	},
	{	0x22, 	TRUE,		TRUE,	eSvcSi_SUBTITLE_AR_16x9,	eSvcSi_SUBTITLE_HD_NO	},
	{	0x23, 	TRUE,		TRUE,	eSvcSi_SUBTITLE_AR_221x1,	eSvcSi_SUBTITLE_HD_NO	},
	{	0x24, 	TRUE,		TRUE,	eSvcSi_SUBTITLE_AR_NA,		eSvcSi_SUBTITLE_HD_YES	}
};
#else
STATIC svcSvc_SubtitleTypeTable_t s_astSubtitleTypeTable[] =
{
	{	.ucType = 0x01,		.bDvb = FALSE,		.bHoh = FALSE, 	.eAspectRatio = eSvcSi_SUBTITLE_AR_NA, 		.eHd = eSvcSi_SUBTITLE_HD_NA	},
	{	.ucType = 0x02, 	.bDvb = FALSE,		.bHoh = FALSE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_NA,		.eHd = eSvcSi_SUBTITLE_HD_NA	},
	{	.ucType = 0x03, 	.bDvb = FALSE,		.bHoh = FALSE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_NA,		.eHd = eSvcSi_SUBTITLE_HD_NA	},
	{	.ucType = 0x04, 	.bDvb = FALSE,		.bHoh = FALSE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_NA,		.eHd = eSvcSi_SUBTITLE_HD_NA	},
	{	.ucType = 0x05, 	.bDvb = FALSE,		.bHoh = TRUE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_NA,		.eHd = eSvcSi_SUBTITLE_HD_NA	},

	{	.ucType = 0x10, 	.bDvb = TRUE,		.bHoh = FALSE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_NO,		.eHd = eSvcSi_SUBTITLE_HD_NO	},
	{	.ucType = 0x11, 	.bDvb = TRUE,		.bHoh = FALSE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_4x3,		.eHd = eSvcSi_SUBTITLE_HD_NO	},
	{	.ucType = 0x12, 	.bDvb = TRUE,		.bHoh = FALSE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_16x9,	.eHd = eSvcSi_SUBTITLE_HD_NO	},
	{	.ucType = 0x13, 	.bDvb = TRUE,		.bHoh = FALSE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_221x1,	.eHd = eSvcSi_SUBTITLE_HD_NO	},
	{	.ucType = 0x14, 	.bDvb = TRUE,		.bHoh = FALSE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_NA,		.eHd = eSvcSi_SUBTITLE_HD_YES	},

	{	.ucType = 0x20, 	.bDvb = TRUE,		.bHoh = TRUE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_NO,		.eHd = eSvcSi_SUBTITLE_HD_NO	},
	{	.ucType = 0x21, 	.bDvb = TRUE,		.bHoh = TRUE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_4x3,		.eHd = eSvcSi_SUBTITLE_HD_NO	},
	{	.ucType = 0x22, 	.bDvb = TRUE,		.bHoh = TRUE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_16x9,	.eHd = eSvcSi_SUBTITLE_HD_NO	},
	{	.ucType = 0x23, 	.bDvb = TRUE,		.bHoh = TRUE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_221x1,	.eHd = eSvcSi_SUBTITLE_HD_NO	},
	{	.ucType = 0x24, 	.bDvb = TRUE,		.bHoh = TRUE,	.eAspectRatio = eSvcSi_SUBTITLE_AR_NA,		.eHd = eSvcSi_SUBTITLE_HD_YES	}
};
#endif

STATIC HBOOL svc_si_LibSvcEsSetDvbFlagFromType(HUINT8 ucType, HBOOL *pbDvb,
																HBOOL *pbHoh,
																svcSi_SubtitleHd_e *peHd,
																svcSi_SubtitleAr_e *peAspectRatio)
{
	HINT32						 i, nItemNum;
	HBOOL						 bRet;
	svcSvc_SubtitleTypeTable_t	*pstItem;

	bRet = FALSE;
	nItemNum = sizeof(s_astSubtitleTypeTable) / sizeof(svcSvc_SubtitleTypeTable_t);

	for (i=0; i<nItemNum; i++)
	{
		pstItem = &s_astSubtitleTypeTable[i];
		if (pstItem->ucType == ucType)
		{
			if (pbDvb)			*pbDvb			= pstItem->bDvb;
			if (pbHoh)			*pbHoh			= pstItem->bHoh;
			if (peHd)			*peHd			= pstItem->eHd;
			if (peAspectRatio)	*peAspectRatio	= pstItem->eAspectRatio;
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

STATIC void svc_si_LibSvcEsInitSubtitleSelection(SvcSi_SelectedEs_t *pstSelected)
{
	pstSelected->bFound				= FALSE;
	pstSelected->nIndexMain			= -1;
	pstSelected->nIndexAux			= -1;
	pstSelected->usPid				= PID_NULL;
	pstSelected->ucComponentTag		= 0;
	pstSelected->ulCodec			= 0;
	pstSelected->eLangIdMain		= eLangID_MAX;
	pstSelected->eLangIdAux			= eLangID_MAX;
	pstSelected->ulParam1			= 0;
	pstSelected->ulParam2			= 0;
	pstSelected->ulParam3			= 0;
}

STATIC HINT32	svc_si_LibCompareSubtitlePriority (const void *pvArg1, const void *pvArg2)
{
	const svcSi_EsSubtitleCandidate_t *pstLeft = *(const svcSi_EsSubtitleCandidate_t **)pvArg1;
	const svcSi_EsSubtitleCandidate_t *pstRight = *(const svcSi_EsSubtitleCandidate_t **)pvArg2;

	if (pstLeft->nPriority > pstRight->nPriority)
		return -1;

	if (pstLeft->nPriority < pstRight->nPriority)
		return +1;

	return 0;
}

#if defined(CONFIG_DEBUG)
STATIC HUINT8* svc_si_LibSvcEsPrintSubtitleCandidateName(SvcSi_EsSubtitleSelType_e eSelectType)
{
	switch (eSelectType)
	{
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_LANG_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_LANG_0);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_LANG_1);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_LANG_OSD);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_LANG_UNDEFINED);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_LANG_ORIGINAL);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_LANG_DEFAULT);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_TYPE_DVB_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_TYPE_DVB_MENU);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_TYPE_DVB_AUTO);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_TYPE_EBU_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_TYPE_EBU_MENU);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_TYPE_EBU_AUTO);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_HOH_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_HOH_MENU);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_HOH_OFF_ES_YES);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_HOH_OFF_ES_NO);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_HOH_ON_ES_NO);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_ASPECTRATIO_NA);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_ASPECTRATIO_YES);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_HD_NA);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_HD_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_HD_MENU);
	ENUM_TO_STR(eSvcSi_SUBTTL_SEL_TRACK);
	default: break;
	}
	return "eSvcSi_SUBTTL_SEL_Unknown";
}

STATIC void svc_si_LibSvcEsPrintSubtitleCandidateItem(svcSi_EsSubtitleCandidate_t *pstCandidate, HUINT32 ulItemNum, HBOOL bDetail)
{
	svcSi_EsSubtitleMatchedInfo_t		*pstMatched = svcSi_EsSubttlSelGetMatchedInfo(pstCandidate);
	svcSi_EsSubtitleProperty_t			*pstProperty = svcSi_EsSubttlSelGetEsProperty(pstCandidate);
//	HCHAR								*pszLang = svcSi_LangIdTo639(pstMatched->eLangId);

	svcSi_EsPrint("- [%02d] Index [%02d], Pid[0x%04x] Lang[%3s] Priority [%08d]\n", ulItemNum, pstProperty->nIndex, pstProperty->usPid,
																		svcSi_LangIdTo639(pstProperty->eLangId) ,pstCandidate->nPriority);
	if (bDetail)
	{
		HUINT32 ulCount;
		for (ulCount = 0; ulCount <eSvcSi_SUBTTL_SEL_TYPE_NUM; ulCount++)
		{
			svcSi_EsPrint("     [%50s] : Matched [%d], Priority [%08d]\n",
									svc_si_LibSvcEsPrintSubtitleCandidateName(ulCount),
									pstMatched->astMatchedList[ulCount].bMached,
									pstMatched->astMatchedList[ulCount].nPriority);
		}
	}
}

STATIC void svc_si_LibSvcEsPrintSubtitleEsItem(svcSi_SubtitleEs_t *pstSubtitleEs, HUINT32 ulIndex, HBOOL bDetail)
{
	HBOOL				bDvb, bHoh;
	svcSi_SubtitleHd_e	eHd;
	svcSi_SubtitleAr_e	eAspectRatio;

	bDvb = bHoh = FALSE;
	eHd = eSvcSi_SUBTITLE_HD_NA;
	eAspectRatio = eSvcSi_SUBTITLE_AR_NA;

	svc_si_LibSvcEsSetDvbFlagFromType(pstSubtitleEs->ucType, &bDvb, &bHoh, &eHd, &eAspectRatio);

	svcSi_EsPrint("- [%02d] : Pid(0x%04x), Lang[%3s], Type(%02d:[DVB:%d][HOH:%d][HD:%d][AR:%d]))\n", ulIndex,
				pstSubtitleEs->usPid,
				pstSubtitleEs->aucLangCode,
				pstSubtitleEs->ucType, bDvb, bHoh, eHd, eAspectRatio);
}

STATIC HBOOL svc_si_LibSvcEsPrintSubtitleEsItem2(void *pvData, void *pvUserData)
{
	svcSi_SubtitleEs_t		*pstSubtitleEs;
	HINT32					*pnCount = (HINT32 *)pvUserData;

	pstSubtitleEs = (svcSi_SubtitleEs_t *)pvData;

	// Ignore track
	if ((NULL == pstSubtitleEs) || (PID_NULL == pstSubtitleEs->usPid) || (NULL == pnCount))
		return FALSE;

	svc_si_LibSvcEsPrintSubtitleEsItem(pstSubtitleEs, *pnCount, FALSE);
	(*pnCount)++;

	return FALSE;
}

STATIC HBOOL svc_si_LibSvcEsPrintSubtitleCandidateItemCb(void *pvData, void *pvUserData)
{
	svcSi_EsSubtitleCandidate_t	*pstCandidate = (svcSi_EsSubtitleCandidate_t *)pvData;
	HUINT32						*pulItemIndex = (HUINT32 *)pvUserData;

	if (NULL == pstCandidate)
		return FALSE;

	if (NULL == pulItemIndex)
		return FALSE;

	svc_si_LibSvcEsPrintSubtitleCandidateItem(pstCandidate, *pulItemIndex, FALSE);
	(*pulItemIndex)++;

	return FALSE;
}
#endif

STATIC SvcSi_EsSubtitleSelType_e svc_si_LibEsSubtitleGetSelTypeFromLangConfiguration(SvcSi_CfgSubtitleLang_e eConfiguration)
{
	switch (eConfiguration)
	{
	case eSvcSi_CFG_SUBTTL_LANG_User:			return eSvcSi_SUBTTL_SEL_LANG_USER;
	case eSvcSi_CFG_SUBTTL_LANG_User_With_Idx:	return eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX;
	case eSvcSi_CFG_SUBTTL_LANG_0:				return eSvcSi_SUBTTL_SEL_LANG_0;
	case eSvcSi_CFG_SUBTTL_LANG_1:				return eSvcSi_SUBTTL_SEL_LANG_1;
	case eSvcSi_CFG_SUBTTL_LANG_Undefined:		return eSvcSi_SUBTTL_SEL_LANG_UNDEFINED;
	case eSvcSi_CFG_SUBTTL_LANG_Original:		return eSvcSi_SUBTTL_SEL_LANG_ORIGINAL;
	case eSvcSi_CFG_SUBTTL_LANG_Default:		return eSvcSi_SUBTTL_SEL_LANG_DEFAULT;
	default:									break;
	}

	return eSvcSi_AUD_SEL_TYPE_NUM;
}

STATIC HBOOL svc_si_LibEsSubtitleGetMatched(svcSi_EsMatch_t *astMatchedList, SvcSi_EsSubtitleSelType_e eSelType)
{
	if (eSvcSi_SUBTTL_SEL_TYPE_NUM <= eSelType)
		return FALSE;

	return astMatchedList[eSelType].bMached;
}

STATIC HBOOL svc_si_LibEsSubtitleSetMatched(svcSi_EsMatch_t *astMatchedList, SvcSi_EsSubtitleSelType_e eSelType, HBOOL bMatched)
{
	if (eSvcSi_SUBTTL_SEL_TYPE_NUM <= eSelType)
		return FALSE;

	astMatchedList[eSelType].bMached = bMatched;

	return astMatchedList[eSelType].bMached;
}

STATIC void svc_si_LibEsSubtitleResetCandidate(svcSi_EsSubtitleCandidate_t *pstCandidate)
{
	if (pstCandidate)
	{
		svcSi_EsSubtitleProperty_t		*pstProperty = svcSi_EsSubttlSelGetEsProperty(pstCandidate);

		HxSTD_MemSet(pstCandidate, 0x00, sizeof(svcSi_EsSubtitleCandidate_t));

		pstProperty->nIndex			= -1;
		pstProperty->bFirstTrack	= FALSE;
		pstProperty->usPid			= PID_NULL;
		pstProperty->eLangId		= eLangID_MAX;
		pstProperty->bHoh			= FALSE;
		pstProperty->bDvb			= FALSE;
		pstProperty->eHD			= eSvcSi_SUBTITLE_HD_MAX;
		pstProperty->eAspecRatio	= eSvcSi_SUBTITLE_AR_MAX;

		pstCandidate->nPriority		= -1;
	}
}

STATIC HINT32 svc_si_LibSvcEsGetSubtitlePriority (SvcSi_SubtitlePriorityType_e eType)
{
	SvcSi_EsTypePriority_t	*astTable;
	HUINT32					 ulNum, ulCount;

	if (eType >= eSvcSi_SUBTTL_PRIORITY_TYPE_NUM)
	{
		HxLOG_Error("eType >= eSvcSi_SUBTTL_PRIORITY_TYPE_NUM\n");
		return 0;
	}

	astTable = NULL;
	ulNum = 0;
	(void)svc_si_GetSubtitlePriorityTable(&astTable, &ulNum);
	if (NULL == astTable || 0 >= ulNum)
	{
		HxLOG_Error("NULL == astTable(%p) || 0 >= ulNum(%d)\n", astTable, ulNum);
		return 0;
	}

	for (ulCount=0; ulCount<ulNum; ulCount++)
	{
		if (astTable[ulCount].nPriorityType == eSvcSi_SUBTTL_PRIORITY_TYPE_NUM)
			break;

		if (astTable[ulCount].nPriorityType == eType)
			return astTable[ulCount].nPriority;
	}

	return 0;
}

STATIC HCHAR* svc_si_LibSvcEsGetSubtitlePriorityTableItemName(SvcSi_SubtitlePriorityType_e eItem)
{
	switch (eItem)
	{
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_BASE);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_USER_WITH_IDX);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_MENU_0);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_MENU_1);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_OSD);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_UNDEFINED);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_ORIGINAL);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_LANG_DEFAULT);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_TYPE_BASE);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_MENU);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_AUTO);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_MENU);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_AUTO);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HOH_BASE);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_MENU);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_YES);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_NO);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HOH_ON_ES_NO);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_BASE);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_NA);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_MATCHED_YES);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HD_BASE);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HD_NA);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_USER);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_MENU);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_BASE);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_YES);
	ENUM_TO_STR(eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_NO);
	default:
		break;
	}
	return "";
}

void svc_si_LibSvcEsPrintSubtitlePriorityTable(void)
{
	HUINT32	ulCount;

	svcSi_EsPrint("=================================================================================\n");
	svcSi_EsPrint("=== Subtitle Priority Table =====================================================\n");
	svcSi_EsPrint("=================================================================================\n");
	for (ulCount=0; ulCount<eSvcSi_SUBTTL_PRIORITY_TYPE_NUM; ulCount++)
	{
		SvcSi_SubtitlePriorityType_e ePriorityType = (SvcSi_SubtitlePriorityType_e)ulCount;
		svcSi_EsPrint("  [%-60s] : [%08d]\n", svc_si_LibSvcEsGetSubtitlePriorityTableItemName(ePriorityType),
											svc_si_LibSvcEsGetSubtitlePriority(ePriorityType));
	}
	svcSi_EsPrint("=================================================================================\n");

}


STATIC HINT32 svc_si_LibSvcEsSubtitleGetFirstTrackScore (svcSi_EsSubtitleProperty_t *pstProperty)
{
	HINT32 nTemp = 0;

	if (pstProperty->bFirstTrack)
	{
		nTemp = svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_YES);
	}
	else
	{
		nTemp = svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_FIRST_TRACK_NO);
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsSubtitleGetLangScore (svcSi_EsMatch_t *astMatchedList)
{
	HINT32	nTemp;

	nTemp = svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_BASE);

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_USER))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_USER));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_USER_WITH_IDX));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_1))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_MENU_1));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_0))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_MENU_0));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_UNDEFINED))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_UNDEFINED));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_ORIGINAL))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_ORIGINAL));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_DEFAULT))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_DEFAULT));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_OSD))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_LANG_OSD));
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsSubtitleGetMainTypeScore (svcSi_EsMatch_t *astMatchedList)
{
	HINT32 nTemp = 0;

	nTemp = svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_TYPE_BASE);

	// User
	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_TYPE_DVB_USER))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_USER));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_TYPE_EBU_USER))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_USER));
	}

	// Menu
	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_TYPE_DVB_MENU))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_MENU));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_TYPE_EBU_MENU))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_MENU));
	}

	// Auto
	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_TYPE_DVB_AUTO))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_TYPE_DVB_MATCHED_AUTO));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_TYPE_EBU_AUTO))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_TYPE_EBU_MATCHED_AUTO));
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsSubtitleGetHohScore (svcSi_EsMatch_t *astMatchedList)
{
	HINT32 nTemp = 0;

	nTemp = svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HOH_BASE);

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_HOH_USER))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_USER));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_HOH_MENU))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HOH_MATCHED_MENU));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_HOH_OFF_ES_YES))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_YES));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_HOH_OFF_ES_NO))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HOH_OFF_ES_NO));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_HOH_ON_ES_NO))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HOH_ON_ES_NO));
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsSubtitleGetAspectRatioScore (svcSi_EsMatch_t *astMatchedList)
{
	HINT32 nTemp = 0;

	nTemp = svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_BASE);

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_ASPECTRATIO_NA))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_NA));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_ASPECTRATIO_YES))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_ASPECTRATIO_MATCHED_YES));
	}

	return nTemp;
}

STATIC HINT32 svc_si_LibSvcEsSubtitleGetHdScore (svcSi_EsMatch_t *astMatchedList)
{
	HINT32 nTemp = 0;

	nTemp = svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HD_BASE);

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_HD_NA))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HD_NA));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_HD_USER))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_USER));
	}

	if (svc_si_LibEsSubtitleGetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_HD_MENU))
	{
		nTemp = HxMACRO_MAX(nTemp, svc_si_LibSvcEsGetSubtitlePriority(eSvcSi_SUBTTL_PRIORITY_HD_MATCHED_MENU));
	}

	return nTemp;
}


STATIC HINT32 svc_si_LibSvcEsSetSubtitlePriority (svcSi_EsSubtitleCandidate_t	*pstCandidate)
{
	HINT32	nScoreLang, nScoreHoh, nScoreMainType, nScoreHd, nScoreAr, nScoreTrack;
	svcSi_EsMatch_t					*astMatchedList = svcSi_EsSubttlSelGetMatchedList(pstCandidate);
	svcSi_EsSubtitleProperty_t		*pstProperty = svcSi_EsSubttlSelGetEsProperty(pstCandidate);

	nScoreLang = nScoreHoh = nScoreMainType = nScoreHd = nScoreAr = nScoreTrack = 0;

	// Firt Track
	nScoreTrack = svc_si_LibSvcEsSubtitleGetFirstTrackScore(pstProperty);
	// Language
	nScoreLang = svc_si_LibSvcEsSubtitleGetLangScore(astMatchedList);
	// Main Type
	nScoreMainType = svc_si_LibSvcEsSubtitleGetMainTypeScore(astMatchedList);
	// HOH
	nScoreHoh = svc_si_LibSvcEsSubtitleGetHohScore(astMatchedList);
	// Aspect Ratio
	nScoreAr = svc_si_LibSvcEsSubtitleGetAspectRatioScore(astMatchedList);
	// HD
	nScoreHd = svc_si_LibSvcEsSubtitleGetHdScore(astMatchedList);

	pstCandidate->nPriority = nScoreLang + nScoreHoh + nScoreMainType + nScoreHd + nScoreAr + nScoreTrack;

	svcSi_EsPrint("[%02d] = { [Language : %06d], [HoH : %06d], [MainType : %06d], [HD : %06d], [AR : %06d], [First Track : %06d] } Total [%06d]\n",
		pstProperty->nIndex,
		nScoreLang, nScoreHoh, nScoreMainType, nScoreHd, nScoreAr, nScoreTrack,
		pstCandidate->nPriority);

	return pstCandidate->nPriority;
}


STATIC HxLANG_Id_e svc_si_LibSvcEsGetSubtitleLangInfo(const svcSi_SubtitleEs_t *pstSubtitleEs)
{
	HxLANG_Id_e		 eLangId;
	HUINT32			 ulLen;

	eLangId = eLangID_MAX;

	// 1st Track : Check the language number:
	ulLen = HxSTD_StrLen(pstSubtitleEs->aucLangCode);
	if (3 == ulLen)
	{
		eLangId = HLIB_LANG_639ToId ((const HCHAR *)pstSubtitleEs->aucLangCode);
	}

	return eLangId;
}

STATIC HBOOL svc_si_LibSvcSubtitleIsLanguageMatched (const svcSi_SubtitleEs_t *pstSubtitleEs, HxLANG_Id_e eFindId)
{
	HxLANG_Id_e 	 eLangId;

	if (eLangID_NONE == eFindId || eLangID_MAX == eFindId)
		return FALSE;

	eLangId = svc_si_LibSvcEsGetSubtitleLangInfo(pstSubtitleEs);

	return (eFindId == eLangId) ? TRUE : FALSE;
}

STATIC void svc_si_LibSvcEsSubtitleSetLang(svcSi_EsSubtitleCandidate_t 			*pstCandidate,
												const svcSi_SubtitleEs_t		*pstSubtitleEs,
												const HxLANG_Id_e				 eOsdLang,
												const SvcSi_CfgSubtitleLang_t	*pstSubtitleLang,
												const HINT32					 usUserSubtitlePid)
{
	HUINT32							 ulCount;
	svcSi_EsMatch_t					*astMatchedList = svcSi_EsSubttlSelGetMatchedList(pstCandidate);

	// User, Subtitle Lang1 and Lang2, Undefined, Original, Default
	for (ulCount = 0; ulCount<eSvcSi_CFG_SUBTTL_LANG_NUM; ulCount++)
	{
		SvcSi_CfgSubtitleLang_e		eSubttlLang = (SvcSi_CfgSubtitleLang_e)ulCount;
		HxLANG_Id_e 				eLangId = pstSubtitleLang->aeLangId[ulCount];

		if ((eLangID_MAX == eLangId) || (eLangID_NONE == eLangId))	continue;

		if (svc_si_LibSvcSubtitleIsLanguageMatched (pstSubtitleEs, eLangId))
		{
			SvcSi_EsSubtitleSelType_e	eSelType = svc_si_LibEsSubtitleGetSelTypeFromLangConfiguration(eSubttlLang);

			if ((eSelType == eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX)
				&& (usUserSubtitlePid == pstSubtitleEs->usPid)
				&& (usUserSubtitlePid != PID_NULL))
			{
				// eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX는 PID가 함께 일치 해야 한다.
				(void)svc_si_LibEsSubtitleSetMatched(astMatchedList, eSelType, TRUE);
			}
			else if (eSelType != eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX)
			{
				// eSvcSi_SUBTTL_SEL_LANG_USER_WITH_IDX 조건을 제외한 나머지 일치 확인.
				(void)svc_si_LibEsSubtitleSetMatched(astMatchedList, eSelType, TRUE);
			}
		}
	}

	// Find OSD Lang
	if ((eLangID_MAX != eOsdLang) && (eLangID_NONE != eOsdLang))
	{
		if (svc_si_LibSvcSubtitleIsLanguageMatched (pstSubtitleEs, eOsdLang))
		{
			(void)svc_si_LibEsSubtitleSetMatched(astMatchedList, eSvcSi_SUBTTL_SEL_LANG_OSD, TRUE);
		}
	}

#if 0
#define	PrintBool(bTrue)		(bTrue) ? "TRUE" : "FALSE"

	svcSi_EsPrint("\n\n+++++++++++++++++++++++++++++++++++++++\n");
	svcSi_EsPrint("[%60s] : [%s]\n", "astMatchedList[eSvcSi_SUBTTL_SEL_LANG_USER]", PrintBool(astMatchedList[eSvcSi_SUBTTL_SEL_LANG_USER].bMached));
	svcSi_EsPrint("[%60s] : [%s]\n", "astMatchedList[eSvcSi_SUBTTL_SEL_LANG_0]", PrintBool(astMatchedList[eSvcSi_SUBTTL_SEL_LANG_0].bMached));
	svcSi_EsPrint("[%60s] : [%s]\n", "astMatchedList[eSvcSi_SUBTTL_SEL_LANG_1]", PrintBool(astMatchedList[eSvcSi_SUBTTL_SEL_LANG_1].bMached));
	svcSi_EsPrint("[%60s] : [%s]\n", "astMatchedList[eSvcSi_SUBTTL_SEL_LANG_OSD]", PrintBool(astMatchedList[eSvcSi_SUBTTL_SEL_LANG_OSD].bMached));
	svcSi_EsPrint("[%60s] : [%s]\n", "astMatchedList[eSvcSi_SUBTTL_SEL_LANG_UNDEFINED]", PrintBool(astMatchedList[eSvcSi_SUBTTL_SEL_LANG_UNDEFINED].bMached));
	svcSi_EsPrint("[%60s] : [%s]\n", "astMatchedList[eSvcSi_SUBTTL_SEL_LANG_ORIGINAL]", PrintBool(astMatchedList[eSvcSi_SUBTTL_SEL_LANG_ORIGINAL].bMached));
	svcSi_EsPrint("[%60s] : [%s]\n", "astMatchedList[eSvcSi_SUBTTL_SEL_LANG_DEFAULT]", PrintBool(astMatchedList[eSvcSi_SUBTTL_SEL_LANG_DEFAULT].bMached));
	svcSi_EsPrint("+++++++++++++++++++++++++++++++++++++++\n\n");
#endif
}

STATIC void svc_si_LibSvcEsSubtitleSetHoh(svcSi_EsSubtitleCandidate_t 			*pstCandidate,
												const svcSi_SubtitleEs_t		*pstSubtitleEs,
												const SvcSi_CfgSubtitleType_t	*pstSubtitleType)
{
	svcSi_EsMatch_t					*astMatchedList = svcSi_EsSubttlSelGetMatchedList(pstCandidate);
	svcSi_EsSubtitleProperty_t		*pstProperty = svcSi_EsSubttlSelGetEsProperty(pstCandidate);
	SvcSi_EsSubtitleSelType_e		 eSelType = eSvcSi_SUBTTL_SEL_TYPE_NUM;
	HBOOL							 bMatched = FALSE;

	if (pstSubtitleType->abHoh[eSvcSi_CFG_SUBTTL_HOH_User])
	{
		if (pstProperty->bHoh)
		{
			eSelType = eSvcSi_SUBTTL_SEL_HOH_USER;
			bMatched = TRUE;
		}
		else
		{
			eSelType = eSvcSi_SUBTTL_SEL_HOH_ON_ES_NO;
			bMatched = TRUE;
		}
	}
	else if (pstSubtitleType->abHoh[eSvcSi_CFG_SUBTTL_HOH_Menu])
	{
		if (pstProperty->bHoh)
		{
			eSelType = eSvcSi_SUBTTL_SEL_HOH_MENU;
			bMatched = TRUE;
		}
		else
		{
			eSelType = eSvcSi_SUBTTL_SEL_HOH_ON_ES_NO;
			bMatched = TRUE;
		}
	}
	else
	{
		if (pstProperty->bHoh)
		{
			eSelType = eSvcSi_SUBTTL_SEL_HOH_OFF_ES_YES;
			bMatched = TRUE;
		}
		else
		{
			eSelType = eSvcSi_SUBTTL_SEL_HOH_OFF_ES_NO;
			bMatched = TRUE;
		}
	}

	(void)svc_si_LibEsSubtitleSetMatched(astMatchedList, eSelType, bMatched);
}


STATIC void svc_si_LibSvcEsSubtitleSetMainType(svcSi_EsSubtitleCandidate_t 		*pstCandidate,
												const svcSi_SubtitleEs_t		*pstSubtitleEs,
												const SvcSi_CfgSubtitleType_t	*pstSubtitleType)
{
	svcSi_EsMatch_t					*astMatchedList = svcSi_EsSubttlSelGetMatchedList(pstCandidate);
	svcSi_EsSubtitleProperty_t		*pstProperty = svcSi_EsSubttlSelGetEsProperty(pstCandidate);
	SvcSi_EsSubtitleSelType_e		 eSelType;
	HBOOL							 bMatched;

	eSelType = eSvcSi_SUBTTL_SEL_TYPE_NUM;
	bMatched = FALSE;

	if (pstSubtitleType->abDvb[eSvcSi_CFG_SUBTTL_DVB_User])
	{
		bMatched = TRUE;
		eSelType = (pstProperty->bDvb) ? eSvcSi_SUBTTL_SEL_TYPE_DVB_USER : eSvcSi_SUBTTL_SEL_TYPE_EBU_USER;
	}
	else
	{
		bMatched = TRUE;
 		eSelType = (pstProperty->bDvb) ? eSvcSi_SUBTTL_SEL_TYPE_DVB_AUTO : eSvcSi_SUBTTL_SEL_TYPE_EBU_AUTO;
	}

	if (!bMatched)
	{
		if (pstSubtitleType->abDvb[eSvcSi_CFG_SUBTTL_DVB_Menu])
		{
			bMatched = TRUE;
			eSelType = (pstProperty->bDvb) ? eSvcSi_SUBTTL_SEL_TYPE_DVB_MENU : eSvcSi_SUBTTL_SEL_TYPE_EBU_MENU;
		}
		else
		{
			bMatched = TRUE;
			eSelType = (pstProperty->bDvb) ? eSvcSi_SUBTTL_SEL_TYPE_DVB_AUTO : eSvcSi_SUBTTL_SEL_TYPE_EBU_AUTO;
		}
	}

	(void)svc_si_LibEsSubtitleSetMatched(astMatchedList, eSelType, bMatched);
}

STATIC void svc_si_LibSvcEsSubtitleSetAspectRatio(svcSi_EsSubtitleCandidate_t 		*pstCandidate,
													const svcSi_SubtitleEs_t		*pstSubtitleEs,
													const SvcSi_CfgSystem_t 		*pstSystem)
{
	svcSi_EsMatch_t					*astMatchedList = svcSi_EsSubttlSelGetMatchedList(pstCandidate);
	svcSi_EsSubtitleProperty_t		*pstProperty = svcSi_EsSubttlSelGetEsProperty(pstCandidate);
	SvcSi_EsSubtitleSelType_e		 eSelType;
	HBOOL							 bMatched;

	eSelType = eSvcSi_SUBTTL_SEL_ASPECTRATIO_NA;
	bMatched = FALSE;

	switch (pstProperty->eAspecRatio)
	{
	case eSvcSi_SUBTITLE_AR_NA:
		eSelType = eSvcSi_SUBTTL_SEL_ASPECTRATIO_NA;
		bMatched = TRUE;
		break;
	case eSvcSi_SUBTITLE_AR_NO:
		break;
	case eSvcSi_SUBTITLE_AR_4x3:
		if (eDxASPECTRATIO_4X3 == pstSystem->eAspectRatio)
		{
			eSelType = eSvcSi_SUBTTL_SEL_ASPECTRATIO_YES;
			bMatched = TRUE;
		}
		break;
	case eSvcSi_SUBTITLE_AR_16x9:
		if (eDxASPECTRATIO_16X9 == pstSystem->eAspectRatio)
		{
			eSelType = eSvcSi_SUBTTL_SEL_ASPECTRATIO_YES;
			bMatched = TRUE;
		}
		break;
	case eSvcSi_SUBTITLE_AR_221x1:
	default:
		break;
	}

	(void)svc_si_LibEsSubtitleSetMatched(astMatchedList, eSelType, bMatched);
}

STATIC void svc_si_LibSvcEsSubtitleSetHd(svcSi_EsSubtitleCandidate_t 		*pstCandidate,
											const svcSi_SubtitleEs_t		*pstSubtitleEs,
											const SvcSi_CfgSubtitleType_t	*pstSubtitleType)
{
	svcSi_EsMatch_t					*astMatchedList = svcSi_EsSubttlSelGetMatchedList(pstCandidate);
	svcSi_EsSubtitleProperty_t		*pstProperty = svcSi_EsSubttlSelGetEsProperty(pstCandidate);
	SvcSi_EsSubtitleSelType_e		 eSelType;
	HBOOL							 bMatched;

	eSelType = eSvcSi_SUBTTL_SEL_HD_NA;
	bMatched = FALSE;

	switch (pstProperty->eHD)
	{
	case eSvcSi_SUBTITLE_HD_NA:
		eSelType = eSvcSi_SUBTTL_SEL_HD_NA;
		bMatched = TRUE;
		break;
	case eSvcSi_SUBTITLE_HD_YES:
		if (pstSubtitleType->abHD[eSvcSi_CFG_SUBTTL_HD_User])
		{
			eSelType = eSvcSi_SUBTTL_SEL_HD_USER;
		}
		else if (pstSubtitleType->abHD[eSvcSi_CFG_SUBTTL_HD_Menu])
		{
			eSelType = eSvcSi_SUBTTL_SEL_HD_MENU;
		}
		bMatched = TRUE;
		break;
	case eSvcSi_SUBTITLE_HD_NO:
	default:
		break;
	}

	(void)svc_si_LibEsSubtitleSetMatched(astMatchedList, eSelType, bMatched);
}

STATIC HBOOL svc_si_LibSvcEsSubtitleAddToCandidateList(HxVector_t						*pstCandidateList,
														svcSi_EsSubtitleCandidate_t 	*pstCandidate)
{
	// 1. Set Priority
	if (svc_si_LibSvcEsSetSubtitlePriority(pstCandidate) > 0)
	{
	// 2. Clone Item
		svcSi_EsSubtitleCandidate_t *pstClone = (svcSi_EsSubtitleCandidate_t *)HLIB_STD_MemDup(pstCandidate, sizeof(svcSi_EsSubtitleCandidate_t));
		if (pstClone)
		{
			HERROR	hErr;
	// 3. Add to Candidate List
			hErr = HLIB_VECTOR_Add(pstCandidateList, pstClone);
			return (ERR_OK == hErr) ? TRUE : FALSE;
		}
	}

	return FALSE;
}

STATIC HERROR svc_si_LibSvcEsSubtitleGetTopPriorityMainIndex(HxVector_t *pstCandidateList, HINT32 *pnIndex, HxLANG_Id_e *peLangId, HUINT8 *pucType)
{
	HERROR						 hErr;
	svcSi_EsSubtitleCandidate_t	*pstCandidate;
	HINT32						 i, nItemNum;

	hErr = ERR_FAIL;
	*pnIndex = -1;
	*peLangId = eLangID_MAX;
	*pucType = 0xFF;

	nItemNum = HLIB_VECTOR_Length(pstCandidateList);

	// Sort : descending
	(void)HLIB_VECTOR_Sort(pstCandidateList, svc_si_LibCompareSubtitlePriority);

#if defined(CONFIG_DEBUG)
	svc_si_LibSvcEsPrintCandidateList(pstCandidateList, svc_si_LibSvcEsPrintSubtitleCandidateItemCb);
#endif
	for (i=0; i<nItemNum; i++)
	{
		pstCandidate = (svcSi_EsSubtitleCandidate_t *)HLIB_VECTOR_ItemAt(pstCandidateList, i);

		// Get 1st Item : Top Priority
		if (pstCandidate && pstCandidate->nPriority > 0)
		{
			svcSi_EsSubtitleProperty_t			*pstProperty = svcSi_EsSubttlSelGetEsProperty(pstCandidate);

			// Main Subtitle
			*pnIndex = pstProperty->nIndex;
			// Lang Id
			*peLangId = pstProperty->eLangId;
			// Type
			*pucType = pstProperty->ucType;

			hErr = ERR_OK;

//			svcSi_EsPrint("[SelectedInfo_#1-1] [MainIdx:%d], [Lang:%s], [Type:0x%02x]\n", *pnIndex, svcSi_LangIdTo639(*peLangId), *pucType);
			break;
		}
	}

	return hErr;
}

STATIC HERROR svc_si_LibSvcEsSubtitleGetTopPriorityEsInfo(HxVector_t				*pstCandidateList,
															SvcSi_SelectedEs_t		*pstSelected)
{
	if (HLIB_VECTOR_Length(pstCandidateList) <= 0)
		return ERR_FAIL;

	return svc_si_LibSvcEsSubtitleGetTopPriorityMainIndex(pstCandidateList, &pstSelected->nIndexMain, &pstSelected->eLangIdMain, (HUINT8 *)&pstSelected->ulParam1);
}

STATIC HBOOL svc_si_LibSvcEsSubtitleCheckUpdated(SvcSi_SelectedEs_t *pstSelected)
{
	if (-1 != pstSelected->nIndexMain)							return TRUE;
	if (eLangID_MAX != pstSelected->eLangIdMain)				return TRUE;

	return FALSE;
}

STATIC HERROR svc_si_LibSvcEsGetSelectedSubtitleInfoFromCandidateList(SvcSi_SelectedEs_t	*pstSelected,
																		HxVector_t			*pstCandidateList)
{
	HERROR	hErr;

	// Reset
	svc_si_LibSvcEsInitSubtitleSelection(pstSelected);

	// Get Top Priority Subtitle Info
	hErr = svc_si_LibSvcEsSubtitleGetTopPriorityEsInfo(pstCandidateList, pstSelected);
	if (ERR_OK == hErr)
	{
		if (svc_si_LibSvcEsSubtitleCheckUpdated(pstSelected))
		{
			pstSelected->bFound			= TRUE;
//			svcSi_EsPrint("[[[]]] [Found:%d], [MainIdx:%d], [Lang:%s], [Type:0x%02x]\n", pstSelected->bFound, pstSelected->nIndexMain, svcSi_LangIdTo639(pstSelected->eLangIdMain), pstSelected->ulParam1);
		}
	}

	return hErr;
}


HERROR svc_si_LibSvcEsSelectSubtitle (const svcSi_PmtEsInfo_t *pstPmtEsInfo, const SvcSi_Config_t *pstConfig, SvcSi_SelectedEs_t *pstSelected)
{
	HERROR				 			 hErr = ERR_FAIL;
	HINT32							 nSubtitleCnt;
	HBOOL							 bAdded;
	svcSi_SubtitleEs_t				*pstSubtitleEs;
	HxList_t						*pstItem;
	HxVector_t						*pstCandidateList;
	svcSi_EsSubtitleCandidate_t		 stCandidate;
	svcSi_EsSubtitleProperty_t		*pstProperty;
	HBOOL							 bFirstTrack;
	HBOOL							 bSubtitleEnable;
	HINT32							 usUserSubtitlePid = PID_NULL;
	HBOOL							 bHardOfHearingEnable = FALSE;
	HBOOL							 bSubtitleTempOff = FALSE;

	if ((NULL == pstPmtEsInfo) || (NULL == pstConfig))
	{
		HxLOG_Error("Source NULL : (NULL == pstPmtEsInfo) || (NULL == pstConfig)\n");
		return ERR_FAIL;
	}

	if (NULL == pstSelected)
	{
		HxLOG_Error("Target NULL : (NULL == pstSelected)\n");
		return ERR_FAIL;
	}

	// ==========================================================
	// Init Subtitle Selection
	svc_si_LibSvcEsInitSubtitleSelection(pstSelected);
	// Init Subtitle Candidate List
	pstCandidateList = HLIB_VECTOR_NewEasy(svcSi_MAX_SUBTTL_CANDIDATE, (HxFreeFunc_t)HLIB_STD_MemFree_CB, (HxCompareFunc_t)svc_si_LibCompareSubtitlePriority);
	if (NULL == pstCandidateList)
	{
		HxLOG_Error("0 == HLIB_LIST_Length(pstPmtEsInfo->pstSubttlTrack)\n");
		return ERR_FAIL;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_SUBTITLE_SETTING, (HUINT32 *)&bSubtitleEnable, 0);
	if((hErr != ERR_OK) || bSubtitleEnable == FALSE)
	{
		HxLOG_Error("eDB_PARAM_ITEM_SUBTITLE_SETTING  is FALSE\n");
		(void)HLIB_VECTOR_Delete(pstCandidateList);
		return ERR_OK;
	}

#if defined(CONFIG_OP_UK_DTT)
    // In UK specification, don't use user selected information of subtitle on/off.
#else
	if (pstConfig->stSubtitleLangauge.bUserSelectedOff == TRUE)
	{
		HxLOG_Error("User selected 'OFF' track of subtitle ! \n");
		(void)HLIB_VECTOR_Delete(pstCandidateList);
		return ERR_OK;
	}
#endif

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING, (HUINT32 *)&bHardOfHearingEnable, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HARDOFHEARING) Error!!\n");
	}

	// ==========================================================

#if defined(CONFIG_DEBUG)
	svc_si_LibSvcEsPrintEsList(pstPmtEsInfo->pstSubttlTrack, svc_si_LibSvcEsPrintSubtitleEsItem2);
	svc_si_LibSvcEsPrintConfig(pstConfig, svcSi_ConfigPrintSystem | svcSi_ConfigPrintSubtitle);
#endif
	// Make Candidate List
	bFirstTrack = TRUE;
	pstItem = pstPmtEsInfo->pstSubttlTrack;

	for (nSubtitleCnt = 0; NULL != pstItem; pstItem = HLIB_LIST_Next(pstItem), nSubtitleCnt++)
	{
		pstSubtitleEs = (svcSi_SubtitleEs_t *)HLIB_LIST_Data(pstItem);

		// Ignore track
		if ((NULL == pstSubtitleEs) || (PID_NULL == pstSubtitleEs->usPid))		{ continue; }

		// Init Selection
		svc_si_LibEsSubtitleResetCandidate(&stCandidate);
		pstProperty 	= svcSi_EsSubttlSelGetEsProperty(&stCandidate);

		/* ===================================================================
		 *	1. Set Basic ES Info to Flag
		 * =================================================================== */
		pstProperty->nIndex 	= nSubtitleCnt;											// Index
		pstProperty->usPid		= pstSubtitleEs->usPid;									// PID
		pstProperty->ucType		= pstSubtitleEs->ucType;								// Type
		pstProperty->eLangId 	= svc_si_LibSvcEsGetSubtitleLangInfo((const svcSi_SubtitleEs_t *)pstSubtitleEs);	// Lang Id
		(void)svc_si_LibSvcEsSetDvbFlagFromType(pstSubtitleEs->ucType,	&pstProperty->bDvb,
																		&pstProperty->bHoh,
																		&pstProperty->eHD,
																		&pstProperty->eAspecRatio);

		// If Hoh menu sets Off, Hoh subtitle cannot be displayed. but normal subtitle can be displayed.
		if (pstProperty->bHoh == TRUE)
		{
			if (bHardOfHearingEnable == FALSE)
			{
				bSubtitleTempOff = TRUE;
				HxLOG_Info("If Hoh menu sets Off, Hoh subtitle cannot be displayed.  Set bSubtitleTempOff = TRUE \n");
				continue;
			}
		}

		if (bFirstTrack)																// First Track Flag
		{
			pstProperty->bFirstTrack = TRUE;
			bFirstTrack = FALSE;
		}

		if (nSubtitleCnt == pstConfig->stSubtitleLangauge.nTrack)
		{
			usUserSubtitlePid = pstSubtitleEs->usPid;
		}
		else
		{
			usUserSubtitlePid = PID_NULL;
		}

		/* ===================================================================
		 *	2. Check Language
		 * =================================================================== */
		svc_si_LibSvcEsSubtitleSetLang(&stCandidate,
										(const svcSi_SubtitleEs_t		*)pstSubtitleEs,
										(const HxLANG_Id_e)pstConfig->stSystem.eOsdLang,
										(const SvcSi_CfgSubtitleLang_t 	*)&pstConfig->stSubtitleLangauge,
										(const HINT32) usUserSubtitlePid);

		/* ===================================================================
		 *	3. Check Hoh
		 * =================================================================== */
		svc_si_LibSvcEsSubtitleSetHoh(&stCandidate,
										(const svcSi_SubtitleEs_t		*)pstSubtitleEs,
										(const SvcSi_CfgSubtitleType_t 	*)&pstConfig->stSubtitleType);

		/* ===================================================================
		 *	4. Check Main Type
		 * =================================================================== */
		svc_si_LibSvcEsSubtitleSetMainType(&stCandidate,
										(const svcSi_SubtitleEs_t		*)pstSubtitleEs,
										(const SvcSi_CfgSubtitleType_t 	*)&pstConfig->stSubtitleType);

		/* ===================================================================
		 *	5. Check Aspect Ratio
		 * =================================================================== */
		svc_si_LibSvcEsSubtitleSetAspectRatio(&stCandidate,
										(const svcSi_SubtitleEs_t		*)pstSubtitleEs,
										(const SvcSi_CfgSystem_t		*)&pstConfig->stSystem);

		/* ===================================================================
		 *	6. Check HD
		 * =================================================================== */
		svc_si_LibSvcEsSubtitleSetHd(&stCandidate,
										(const svcSi_SubtitleEs_t		*)pstSubtitleEs,
										(const SvcSi_CfgSubtitleType_t 	*)&pstConfig->stSubtitleType);

		/* ===================================================================
		 *	7. Set Priority and Add to Candidate List
		 * =================================================================== */
		bAdded = svc_si_LibSvcEsSubtitleAddToCandidateList(pstCandidateList, &stCandidate);
		if (bAdded)
		{
#if defined(CONFIG_DEBUG)
			svc_si_LibSvcEsPrintSubtitleCandidateItem(&stCandidate, HLIB_VECTOR_Length(pstCandidateList), FALSE);
#endif
		}
	}

//	svc_si_LibSvcEsPrintCandidateList(pstCandidateList, svc_si_LibSvcEsPrintSubtitleCandidateItemCb);

	/* ===================================================================
	 *	8. Get Top Priority Subtitle Track and Extract Information for Subtitle Play
	 * =================================================================== */
	hErr = svc_si_LibSvcEsGetSelectedSubtitleInfoFromCandidateList(pstSelected, pstCandidateList);

	//If selected subtitle doesn't exist and bSubtitleTempOff is TRUE, set subtitleTrack Index as SUBTITLE_TEMP_OFF_IDX and update it.
	if ( (hErr == ERR_FAIL) &&  (bSubtitleTempOff== TRUE))
	{
		HxLOG_Info("Set subtitleTrack as SUBTITLE_TEMP_OFF_IDX \n");
		pstSelected->nIndexMain = SUBTITLE_TEMP_OFF_IDX;
		pstSelected->bFound = TRUE;
	}

	/* ===================================================================
	 *	9. Tear Down
	 * =================================================================== */
	(void)HLIB_VECTOR_Delete(pstCandidateList);
	pstCandidateList = NULL;

	return ERR_OK;
}

/* end of file */

