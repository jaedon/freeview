/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <silib.h>
#include "util.h"

#include <octo_common.h>
#include <isosvc.h>

#include "../local_include/_svc_cas_si_api.h"
#include "_svc_cas_si_desmgr.h"
#include "_svc_cas_si_desparse.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/** @brief struct에 대한 간단한 설명  */

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
  */
STATIC 	HERROR	svc_cas_SiParseAudioStmDes(HUINT8 *p, svcCas_AudioStmDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseVideoStmDes(HUINT8 *p ,svcCas_VideoStmDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseHierDes(HUINT8 *p, svcCas_HierDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseRegDes(HUINT8 *p, svcCas_RegDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseDataStmAlignDes(HUINT8 *p, svcCas_DataStmAlignDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseBackGridDes(HUINT8 *p, svcCas_BackGridDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseVideoWindowDes(HUINT8 *p, svcCas_VideoWindowDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseCaDes(HUINT8 *p, svcCas_CaDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseIso639LangDes(HUINT8 *p, svcCas_Iso639LangDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseSysClockDes(HUINT8 *p, svcCas_SysClockDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseMultiplexBufUtilDes(HUINT8 *p, svcCas_MultiplexBufUtilDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseCopyrightDes(HUINT8 *p, svcCas_CopyRightDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseMaxBitrateDes(HUINT8 *p, svcCas_MaxBitrateDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParsePrivateDataIndiDes(HUINT8 *p, svcCas_PrivateDataIndicatorDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseSmoothBufDes(HUINT8 *p, svcCas_SmoothBufDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseStdDes(HUINT8 *p, svcCas_StdDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseNetNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_NetNameDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseServListDes(HUINT8 *p, svcCas_SvcListDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseSatDeliSystemDes(HUINT8 *p, svcCas_SatDeliSystemDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseCabDeliSystemDes(HUINT8 *p, svcCas_CabDeliSystemDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseBouqNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_BouqNameDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseServDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_SvcDes_t *pDes);
#if 0 // 이건 잠시 보류
STATIC 	HERROR	svc_cas_SiParseLinkDes(HUINT8 *p, svcCas_LinkDes_t *pDes);
#endif
STATIC 	HERROR	svc_cas_SiParseNvodRefDes(HUINT8 *p, svcCas_NvodRefDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseTimeShiftServDes(HUINT8 *p, svcCas_TimeShiftSvcDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseShortEvtDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_ShortEventDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseExtEvtDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_ExtEvtDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseTimeShiftEvtDes(HUINT8 *p, svcCas_TimeShiftEvtDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseCompDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_CompDes_t *pDes);
STATIC	HERROR  svc_cas_SiParseMosaicDes(HUINT8 *p, svcCas_MosaicDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseStmIdentDes(HUINT8 *p, svcCas_StmIdentDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseCaIdentDes(HUINT8 *p, svcCas_CaIdentDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseContentDes(HUINT8 *p, svcCas_ContentDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseParentRateDes(HUINT8 *p, svcCas_ParentDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseTeletextDes(HUINT8 *p, svcCas_TeletextDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseTelephoneDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_TelephoneDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseSubtitleDes(HUINT8 *p, svcCas_SubTitleDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseMultiNetNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_MultiNetNameDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseMultiBouqNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_MultiBouqNameDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseMultiServNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_MultiSvcNameDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseMultiCompDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_MultiCompDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParsePrivDataSpecDes(HUINT8 *p, svcCas_PrivateDataSpecDes_t *pDes);
STATIC 	HERROR	svc_cas_SiParseServMoveDes(HUINT8 *p, svcCas_SvcMoveDes_t *pDes);
STATIC	HERROR	svc_cas_SiParseShortSmoothBufDes(HUINT8 *p, svcCas_ShortSmoothBufDes_t *pDes);
STATIC	HERROR	svc_cas_SiParseDataBroadcastDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_DataBroadcastDes_t *pDes);
STATIC	HERROR	svc_cas_SiParseCarouselIdDes(HUINT8 *p, svcCas_CarouselIdDes_t *pDes);
STATIC	HERROR	svc_cas_SiParseAssocTagDes(HUINT8 *p, svcCas_AssocTagDes_t *pDes);
STATIC	HERROR	svc_cas_SiParseDeferAssocTagDes(HUINT8 *p, svcCas_DeferAssocTagDes_t *pDes);
STATIC	HERROR	svc_cas_SiParseS2SatDeliSystemDes(HUINT8 *p, svcCas_S2SatDeliSystemDes_t *pDes);
STATIC	HERROR	svc_cas_SiParseEnhancedAC3Des(HUINT8 *p, svcCas_EnhancedAC3Des_t *pDes);
// STATIC	HERROR	svc_cas_SiParseAvcVideo(HUINT8 *p, svcCas_AvcVideoDes_t *pDes);

STATIC	HERROR	svc_cas_SiParseUserDefinedDes(HUINT8 *pucRaw, HUINT8 *pucDes);

STATIC	HULONG	BcdToBinary(HULONG ulVal)
{
	HULONG	ulRetVal, ulIndex;
	HLONG	i;

	ulRetVal	= 0;
	ulIndex		= 1;

	for (i = 0; i < 8; i++)
	{
		ulRetVal += (ulVal & 0x0f) * ulIndex;
		ulVal  = ulVal >> 4;
		ulIndex *= 10;
	}

	return (ulRetVal);
}

/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
  */
#ifdef	SUPPORT_BIG5_IN_SI
STATIC	HUINT32	Uni2UTF8(HUINT16 uni)
{
	HUINT32	utf;

	if (uni < 0x0080)
		utf = uni & 0x007f;
	else if (uni < 0x0800)
	{
		utf  = 0xc000 | (uni & 0x07c0) << 2;
		utf |= 0x0080 | (uni & 0x003f);
	}
	else
	{
		utf  = 0xe00000 | (uni & 0xf000) << 4;
		utf |= 0x008000 | (uni & 0x0fc0) << 2;
		utf |= 0x000080 | (uni & 0x003f);
	}
//	HxLOG_Print("Uni2UTF8: %04x --> %08x\n", uni, utf);

	return utf;
}
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
  */

STATIC	HINT32	ConvertUniStringToUTF8String(HUINT8 *pSrc, HUINT8 **pDest, HUINT16 usSrcLen)
{
	HINT32	nLen;
	HUINT16	uni, i;
	HUINT32	utf;
	HUINT8	tmpBuf[1024], *pTmp;

	pTmp = tmpBuf;
	HxSTD_MemSet(pTmp, 0, 1024);
	nLen = 0;
	pSrc++;
	i = 0;
	while (*pSrc != 0x00 && *(pSrc + 1) != 0x00 && (i < usSrcLen))
	{
		uni = (*pSrc << 8) | *(pSrc + 1);
		utf = Uni2UTF8(uni);
		if (utf > 0xffff)
		{
			*pTmp++ = (utf & 0xff0000) >> 16;
			*pTmp++ = (utf & 0xff00) >> 8;
			*pTmp++ = (utf & 0xff);
			nLen += 3;
		} else if (utf > 0xff)
		{
			*pTmp++ = (utf & 0xff00) >> 8;
			*pTmp++ = (utf & 0xff);
			nLen += 2;
		} else
		{
			*pTmp++ = (utf & 0xff);
			nLen++;
		}

		pSrc += 2;
		i += 2;
	}
	nLen++;
	*pDest = (HUINT8*)OxCAS_Malloc(nLen);
	HxSTD_MemCopy(*pDest, tmpBuf, nLen);

	pTmp = *pDest;

	return nLen;
}

STATIC	HINT32	ConvertBIG5StringToUTF8String(HUINT8 *pSrc, HUINT8 **pDest, HUINT16	usSrcLen)
{
	HINT32	nLen;
	HUINT16	uni, i;
	HUINT32	utf;
	HUINT8	tmpBuf[1024], *pTmp;

	pTmp = tmpBuf;
	HxSTD_MemSet(pTmp, 0, 1024);
	nLen = 0;
	i = 0;
	while (*pSrc && (i < usSrcLen))
	{
		if (*pSrc < 0xa0)
		{
			*pTmp++ = *pSrc++;
			nLen++;
			i++;
		}
		else
		{
			uni = (*pSrc << 8) | *(pSrc + 1);
#if		0
		uni = GetUniFromBIG5(uni);
#endif
		utf = Uni2UTF8(uni);
		if (utf > 0xffff)
		{
			*pTmp++ = (utf & 0xff0000) >> 16;
			*pTmp++ = (utf & 0xff00) >> 8;
			*pTmp++ = (utf & 0xff);
			nLen += 3;
		} else if (utf > 0xff)
		{
			*pTmp++ = (utf & 0xff00) >> 8;
			*pTmp++ = (utf & 0xff);
			nLen += 2;
		} else
		{
			*pTmp++ = (utf & 0xff);
			nLen++;
		}
		pSrc += 2;
			i += 2;
		}
	}

	nLen++;
	*pDest = (HUINT8*)OxCAS_Malloc(nLen);
	HxSTD_MemCopy(*pDest, tmpBuf, nLen);

	pTmp = *pDest;

	return nLen;
}
#endif

STATIC	HUINT32	uriStrlen(HUINT8 ucSrcLen, HUINT8 *pucStr)
{
	HINT32	i, j = 0;

	for (i = 0; i < ucSrcLen; i++)
	{
		if (pucStr[i] >= 0x20 && pucStr[i] <= 0x7F)
			j++;
	}

	return j;
}

STATIC	HUINT32	uriStrcpy(HUINT8 *pucDestUri, HUINT8 *pucSrcStr, HUINT8 ucStrLen)
{
	HINT32	i, j = 0;

	for (i = 0; i < ucStrLen; i++)
	{
		if (pucSrcStr[i] >= 0x20 && pucSrcStr[i] <= 0x7F)
			pucDestUri[j++] = pucSrcStr[i];
	}

	return j;
}



//#define	TEST_SILIB
STATIC HCHAR *	svc_cas_SiUtilHandleDecodeResult (const HCHAR *result, void *userdata)
{
	static const HUINT32 utf8_skip_data[256] = {
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
	};
	HUINT32	skip;
	HUINT32	len;
	HCHAR	*dup;
	HUINT8	*d;
	const HUINT8 *s;

	len = HxSTD_StrLen(result);
	dup = (HCHAR *)OxCAS_Malloc(len + 1);	// 아래 size가 늘어나는 작업이 없기 때문에 같은 사이즈로 alloc
	if (dup == NULL)
		return NULL;

	for (s = (const HUINT8 *)result, d = (HUINT8 *)dup ; *s != '\0' ; )
	{
		skip = utf8_skip_data[(int)*s];

		if (skip == 1)
		{
			if ((*s == 0xa) || (*s == 0xd))
			{
				*d++ = '\n'; // carrage return;
				s++;
				continue;
			}

			if (*s < 0x20)
			{
				*d++ = 0x20; // space;
				s++;
				continue;
			}

		}
		else if (skip == 3)
		{
			if (s[0] == 0xEE && s[1] == 0x82)
			{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) // HMS에서 임시로 사용한다. Web에서 처리하는 방안 확실해 지면 지워도록 하자.
				if (s[2] >= 0x80)
				{
					if (s[2] >= 0x8A)
					{
						*d++ = '\n';
					}
					s += 3;
					continue;
				}
#endif
			}
		}

		while(skip--)
			*d++ = *s++;
	}
	*d = '\0';

	(void)userdata;
	return dup;
}

STATIC SIxTextEncoding_e svc_cas_SiUtilConvEncodingType(const SiTxtEncodedType_t eEncType)
{
	STATIC const HINT32	s_aulTextEncMap[][2] =
	{
		{eSITEXT_ENCODED_300468,        eSIxTEXT_ENC_300468},
		{eSITEXT_ENCODED_300468_EUROPE, eSIxTEXT_ENC_DEFAULT},
		{eSITEXT_ENCODED_300468_EUROPE, eSIxTEXT_ENC_300468_8859_9_BASE},
		{eSITEXT_ENCODED_300468_UK,     eSIxTEXT_ENC_300468_UK},
		{eSITEXT_ENCODED_JAPAN_JIS,     eSIxTEXT_ENC_JIS}
	};

	return (SIxTextEncoding_e)HLIB_STD_Map(0, (HINT32)eEncType, s_aulTextEncMap
							, sizeof(s_aulTextEncMap) / sizeof(s_aulTextEncMap[0]), eSIxTEXT_ENC_DEFAULT);
}


STATIC HUINT32 svc_cas_SiUtilConvSiStringToUtf8(SIxTextEncoding_e eTextEnc, HUINT8 *szLangCode, HUINT32 ulSrcStrLen, HUINT8 *szSrcStr, HUINT8 **ppszDstStr)
{
	*ppszDstStr = SILIB_TEXT_MakeUtf8Ex(eTextEnc, (HCHAR *)szLangCode, szSrcStr, ulSrcStrLen, svc_cas_SiUtilHandleDecodeResult, NULL);
	if (*ppszDstStr)
		return HxSTD_StrLen(*ppszDstStr);

	return 0;
}

HUINT32	svc_cas_SiPreStrMgr(SIxTextEncoding_e eTextEnc, HUINT8 *ucaLangCode, HUINT8 ucSrcStrLen, HUINT8 *pSrcStr, HUINT8 **ppszDestStr)
{
	return svc_cas_SiUtilConvSiStringToUtf8(eTextEnc, ucaLangCode, (HUINT32)ucSrcStrLen, pSrcStr, ppszDestStr);
}

STATIC HERROR	svc_cas_SiMakeCorrct639Code(HUINT8 *p)
{
	HUINT32		i;

	for (i = 0; i < 3; i++)
	{
		// DVB string 의 language indicator charactor 와 conflict 를 방지한다.
		// SI data 가 이상해하게 들어와 hangup 되는 issue 가 있었음
		if (*p < 0x20)
		{
			*p = '\0';
		}
	}

	return ERR_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParseVideoStmDes											*/
/*  																	*/
/*  Description : tag = 0x02											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseVideoStmDes(HUINT8 *p, svcCas_VideoStmDes_t *pDes)
{
	svcCas_VideoStmDes_t	*pVideoStmDes = pDes;
	HUINT8			ucFrameRateCode;
	HBOOL			bMultiFrameRate, bMPEG2, bConstrainParam, bStillPic;
	HUINT8			ucTemp, ucVal;
	HUINT16			usDesLen;

	HxLOG_Info("svc_cas_SiParseVideoStmDes() Entered!");
	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Info("[WARN] Unknown svc_cas_SiParseVideoStmDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
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
	svc_cas_SiSetVideoStmDes(pVideoStmDes, bMultiFrameRate, ucFrameRateCode, bMPEG2, bConstrainParam, bStillPic);

	HxLOG_Info(" svc_cas_SiParseVideoStmDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseAudioStmDes											*/
/*  																	*/
/*  Description : tag = 0x03											*/
/************************************************************************/
STATIC 	HERROR	svc_cas_SiParseAudioStmDes(HUINT8 *p, svcCas_AudioStmDes_t *pDes)
{
	svcCas_AudioStmDes_t	*pAudioStmDes = pDes;
	HUINT8			ucId, ucLayer, ucVarRateAudioIndicator;
	HBOOL			bFreeFormat;
	HUINT8			ucTemp, ucVal;
	HUINT16			usDesLen;

	HxLOG_Info("svc_cas_SiParseAudioStmDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseAudioStmDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
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
	svc_cas_SiSetAudioStmDes (pAudioStmDes, bFreeFormat, ucId, ucLayer, ucVarRateAudioIndicator);

	HxLOG_Info(" svc_cas_SiParseAudioStmDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParseHierDes												*/
/*  																	*/
/*  Description : tag = 0x04											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseHierDes(HUINT8 *p, svcCas_HierDes_t *pDes)
{
	svcCas_HierDes_t	*pHierDes = pDes;
	HUINT8		ucHierType, ucHierLayerIndex, ucHierEmbedLayerIndex, ucHierChannel;
	HUINT16		usDesLen;

	HxLOG_Info("svc_cas_SiParseHierDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseHierDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
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
	svc_cas_SiSetHierDes(pHierDes, ucHierType, ucHierLayerIndex, ucHierEmbedLayerIndex, ucHierChannel);

	HxLOG_Info(" svc_cas_SiParseHierDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParseRegDes												*/
/*  																	*/
/*  Description : tag = 0x05											*/
/************************************************************************/
STATIC 	HERROR	svc_cas_SiParseRegDes(HUINT8 *p, svcCas_RegDes_t *pDes)
{
	svcCas_RegDes_t			*pRegDes = pDes;
	svcCas_PrivateDataInfo_t	*pNewPrivateDataInfo = NULL;
	HUINT16				usDesLen;
	HUINT32				ulFormatId;

	HxLOG_Info("svc_cas_SiParseRegDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4 || usDesLen > MAX_DESCRIPTOR_LIMIT)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseRegDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < format identifier (4byte) */
	ulFormatId = get32bit(p);
	p += 4;
	usDesLen -= 4;
	HxLOG_Print("\t\t(format identifier = 0x%x)\n", ulFormatId);

	/** < Private Data */
	if (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
#ifdef ALLOW_PRIVATE_DATA_ACQUIRE
		pNewPrivateDataInfo = (svcCas_PrivateDataInfo_t *)svc_cas_SiAllocDesInfo(TAG_PRIVATE_DATA, 0);
		if (pNewPrivateDataInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetPrivateDataInfo(pNewPrivateDataInfo, p, usDesLen)  ;
		usDesLen -= usDesLen;
#endif
	}

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetRegDes(pRegDes, ulFormatId, pNewPrivateDataInfo);

	HxLOG_Info(" svc_cas_SiParseRegDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
#ifdef ALLOW_PRIVATE_DATA_ACQUIRE
ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
#endif
}


/************************************************************************/
/*	function : svc_cas_SiParseDataStmAlignDes										*/
/*  																	*/
/*  Description : tag = 0x06											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseDataStmAlignDes(HUINT8 *p, svcCas_DataStmAlignDes_t *pDes)
{
	svcCas_DataStmAlignDes_t	*pDataStmAlignDes = pDes;
	HUINT8				ucAlignType;
	HUINT16				usDesLen;

	HxLOG_Info("svc_cas_SiParseDataStmAlignDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseDataStmAlignDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < alignment type (1byte) */
	ucAlignType = *p;
	HxLOG_Print("\t\t(alignment type = 0x%x)\n", ucAlignType);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetDataStmAlignDes(pDataStmAlignDes, ucAlignType);

	HxLOG_Info("svc_cas_SiParseDataStmAlignDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}



/************************************************************************/
/*	function : svc_cas_SiParseBackGridDes											*/
/*  																	*/
/*  Description : tag = 0x07											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseBackGridDes(HUINT8 *p, svcCas_BackGridDes_t *pDes)
{
	svcCas_BackGridDes_t	*pBackGridDes = pDes;
	HUINT16			usHorSize, usVerSize, usDesLen;
	HUINT8			ucAspectRatio;
	HUINT32			ulTemp;

	HxLOG_Info("svc_cas_SiParseBackGridDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseBackGridDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < horizontal size & vertical size & aspect ratio information (4byte) */
	ulTemp = get32bit(p);
	usHorSize = (HUINT16)((ulTemp & 0xfffc0000) >> 18);
	usVerSize = (HUINT16)((ulTemp & 0x0003fff0) >> 4);
	ucAspectRatio = (HUINT8)(ulTemp & 0x0000000f);
	HxLOG_Print("\t\t(horizontal size = 0x%x)\n", usHorSize);
	HxLOG_Print("\t\t(vertical size = 0x%x)\n", usVerSize);
	HxLOG_Print("\t\t(aspect ratio information = 0x%x)\n", ucAspectRatio);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetBackGridDes(pBackGridDes, usHorSize, usVerSize, ucAspectRatio);

	HxLOG_Info(" svc_cas_SiParseBackGridDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseVideoWindowDes										*/
/*  																	*/
/*  Description : tag = 0x08											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseVideoWindowDes(HUINT8 *p, svcCas_VideoWindowDes_t *pDes)
{
	svcCas_VideoWindowDes_t	*pVideoWindowDes = pDes;
	HUINT16				usDesLen, usHorOff, usVerOff;
	HUINT8				ucWindowPriority;
	HUINT32				ulTemp;

	HxLOG_Info("svc_cas_SiParseVideoWindowDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseVideoWindowDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < horizontal offset & vertical offset & window priority (4byte) */
	ulTemp = get32bit(p);
	usHorOff = (HUINT16)((ulTemp & 0xfffc0000) >> 18);
	usVerOff = (HUINT16)((ulTemp & 0x0003fff0) >> 4);
	ucWindowPriority = (HUINT8)(ulTemp & 0x0000000f);
	HxLOG_Print("\t\t(horizontal offset = 0x%x)\n", usHorOff);
	HxLOG_Print("\t\t(vertical offset = 0x%x)\n", usVerOff);
	HxLOG_Print("\t\t(window priority = 0x%x)\n", ucWindowPriority);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetVideoWindowDes(pVideoWindowDes, usHorOff, usVerOff, ucWindowPriority);

	HxLOG_Info(" svc_cas_SiParseVideoWindowDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseCaDes												*/
/*  																	*/
/*  Description : tag = 0x09											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseCaDes(HUINT8 *p, svcCas_CaDes_t *pDes)
{
	svcCas_CaDes_t				*pCaDes = pDes;
	svcCas_PrivateDataInfo_t	*pNewPrivateDataInfo = NULL;
	HUINT16				usDesLen, usCaSysId, usCaPid;

	HxLOG_Info("svc_cas_SiParseCaDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseCaDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	usCaSysId = (HUINT16)get16bit(p);
	p += 2;
	usDesLen -=2;
	HxLOG_Print("\t\t(CA system id = 0x%x)\n", usCaSysId);

	usCaPid = (HUINT16)get13bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(CA pid = 0x%x)\n", usCaPid);

	/** < Private Data */
	if (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		pNewPrivateDataInfo = (svcCas_PrivateDataInfo_t *)svc_cas_SiAllocDesInfo(TAG_PRIVATE_DATA, 0);
		if (pNewPrivateDataInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetPrivateDataInfo(pNewPrivateDataInfo, p, usDesLen);
	}

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetCaDes(pCaDes, usCaSysId, usCaPid, pNewPrivateDataInfo);

	HxLOG_Info(" svc_cas_SiParseCaDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseIso639LangDes										*/
/*  																	*/
/*  Description : tag = 0x0a											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseIso639LangDes(HUINT8 *p, svcCas_Iso639LangDes_t *pDes)
{
	svcCas_Iso639LangDes_t		*pIso639LangDes = pDes;
	svcCas_LangInfo_t			*pNewLangInfo = NULL, *pNextLangInfo = NULL, *pPrevLangInfo = NULL;
	HUINT8				ucAudioType;
	HUINT8				szIso639LangCode[4];
	HUINT16				usDesLen;

	HxLOG_Info("svc_cas_SiParseIso639LangDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 4) != 0)
	{
		HxLOG_Print("[WARN] Unknown Iso639LangDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	if (usDesLen == 0)
	{
		HxSTD_MemSet (szIso639LangCode, '\0', 4);
		ucAudioType = 0x0;

		/** < Descriptor 데이타 구성. */
		pPrevLangInfo = svc_cas_SiGetPrevLangInfo(pIso639LangDes);
		pNewLangInfo = (svcCas_LangInfo_t *)svc_cas_SiAllocDesInfo(TAG_ISO_639_LANGUAGE, 0);
		if (pNewLangInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetLangInfo(pNewLangInfo, szIso639LangCode, ucAudioType);
		if (pPrevLangInfo == NULL)
			svc_cas_SiLinkLangInfoHead(pIso639LangDes, pNewLangInfo);
		svc_cas_SiLinkLangInfoNode(pNewLangInfo, pNextLangInfo, pPrevLangInfo);
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 lang code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		svc_cas_SiMakeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 lang code = %s)", (char *)szIso639LangCode);

		ucAudioType = *p++;
		usDesLen--;
		HxLOG_Print("(audio type = 0x%x)\n", ucAudioType);

		HxLOG_Print("\t\t\t(lang = %s)(audio type = 0x%x)\n", (char *)szIso639LangCode, ucAudioType);

		/** < Descriptor 데이타 구성. */
		pPrevLangInfo = svc_cas_SiGetPrevLangInfo(pIso639LangDes);
		pNewLangInfo = (svcCas_LangInfo_t *)svc_cas_SiAllocDesInfo(TAG_ISO_639_LANGUAGE, 0);
		if (pNewLangInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetLangInfo(pNewLangInfo, szIso639LangCode, ucAudioType);
		if (pPrevLangInfo == NULL)
			svc_cas_SiLinkLangInfoHead(pIso639LangDes, pNewLangInfo);
		svc_cas_SiLinkLangInfoNode(pNewLangInfo, pNextLangInfo, pPrevLangInfo);
	}

	HxLOG_Info(" svc_cas_SiParseIso639LangDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseSysClockDes											*/
/*  																	*/
/*  Description : tag = 0x0b											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseSysClockDes(HUINT8 *p, svcCas_SysClockDes_t *pDes)
{
	svcCas_SysClockDes_t	*pSysClockDes = pDes;
	HUINT8			ucExtClockRef, ucClockAccuInt, ucClockAccuExp;
	HUINT8			ucTemp;
	HUINT16			usDesLen;

	HxLOG_Info("svc_cas_SiParseSysClockDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 2)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseSysClockDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
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
	svc_cas_SiSetSysClockDes(pSysClockDes, ucExtClockRef, ucClockAccuInt, ucClockAccuExp);

	HxLOG_Info(" svc_cas_SiParseSysClockDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseMultiplexBufUtilDes									*/
/*  																	*/
/*  Description : tag = 0x0c											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseMultiplexBufUtilDes(HUINT8 *p, svcCas_MultiplexBufUtilDes_t *pDes)
{
	svcCas_MultiplexBufUtilDes_t	*pMultiplexBufUtilDes = pDes;
	HUINT16					usDesLen, wLtwOffLowBound, wLtwOffUpBound;
	HBOOL					bBoundValid;
	HUINT16					wTemp;

	HxLOG_Info("svc_cas_SiParseMultiBufUtilDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseMultiBufUtilDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < bound valid flag & LTW offset lower bound (2byte) */
	wTemp = (HUINT16)get16bit(p);
	p += 2;
	bBoundValid = (HUINT8)((wTemp & 0x8000) >> 15);
	wLtwOffLowBound = wTemp & 0x7fff;
	HxLOG_Print("\t\t(bound valid flag = 0x%d)\n", bBoundValid);
	HxLOG_Print("\t\t(LTW offset lower bound = 0x%x)\n", wLtwOffLowBound);

	/** < LTW offset upper bound (2byte) */
	wTemp = (HUINT16)get16bit(p);
	//p += 2;
	wLtwOffUpBound = wTemp & 0x7fff;
	HxLOG_Print("\t\t(LTW offset upper bound = 0x%x)\n", wLtwOffUpBound);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetMultiplexBufUtilDes(pMultiplexBufUtilDes, bBoundValid, wLtwOffLowBound, wLtwOffUpBound);

	HxLOG_Info(" svc_cas_SiParseMultiBufUtilDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseCopyrightDes										*/
/*  																	*/
/*  Description : tag = 0x0d											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseCopyrightDes(HUINT8 *p, svcCas_CopyRightDes_t *pDes)
{
	svcCas_CopyRightDes_t		*pCopyrightDes = pDes;
	svcCas_PrivateDataInfo_t	*pNewPrivateDataInfo = NULL;
	HUINT16				usDesLen;
	HUINT32				dwCopyrightIdent;

	HxLOG_Info("svc_cas_SiParseCopyDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseCopyDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < copyright identifier (4byte); */
	dwCopyrightIdent = get32bit(p);
	p += 4;
	usDesLen -= 4;
	HxLOG_Print("\t\t(copyright identifier = 0x%x)\n", dwCopyrightIdent);

	/** < Private Data */
	if (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
#ifdef ALLOW_PRIVATE_DATA_ACQUIRE
		pNewPrivateDataInfo = (svcCas_PrivateDataInfo_t *)svc_cas_SiAllocDesInfo(TAG_PRIVATE_DATA, 0);
		if (pNewPrivateDataInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetPrivateDataInfo(pNewPrivateDataInfo, p, usDesLen)  ;
#endif
	}

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetCopyrightDes(pCopyrightDes, dwCopyrightIdent, pNewPrivateDataInfo);

	HxLOG_Info(" svc_cas_SiParseCopyDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

#ifdef ALLOW_PRIVATE_DATA_ACQUIRE
ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
#endif
}


/************************************************************************/
/*	function : svc_cas_SiParseMaxBitrateDes										*/
/*  																	*/
/*  Description : tag = 0x0e											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseMaxBitrateDes (HUINT8 *p, svcCas_MaxBitrateDes_t *pDes)
{
	svcCas_MaxBitrateDes_t	*pMaxBitrateDes = pDes;
	HUINT16			usDesLen;
	HUINT32			dwMaxBitrate;
	HUINT32			ulTemp;

	HxLOG_Info("svc_cas_SiParseMaxBitrateDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 3)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseMaxBitrateDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < maximum bitrate (3byte); */
	ulTemp = (HUINT32) get24bit (p);
	//p += 3;
	dwMaxBitrate = ulTemp & 0x3fffff;
	HxLOG_Print("\t\t(maximum bitrate = %d)\n", dwMaxBitrate);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetMaxBitrateDes (pMaxBitrateDes, dwMaxBitrate);

	HxLOG_Info("svc_cas_SiParseMaxBitrateDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParsePrivateDataIndiDes									*/
/*  																	*/
/*  Description : tag = 0x0f											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParsePrivateDataIndiDes (HUINT8 *p, svcCas_PrivateDataIndicatorDes_t *pDes)
{
	svcCas_PrivateDataIndicatorDes_t	*pPrivateDataIndicatorDes = pDes;
	HUINT16						usDesLen;
	HUINT32						dwPrivateDataIndicator;

	HxLOG_Info("svc_cas_SiParsePrivateDataIndiDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown PrivateDataIndicatorDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < private data indicator (4byte); */
	dwPrivateDataIndicator = get32bit (p);
	HxLOG_Print("\t\t(private data indicator = 0x%x)\n", dwPrivateDataIndicator);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetPrivateDataIndicatorDes (pPrivateDataIndicatorDes, dwPrivateDataIndicator);

	HxLOG_Info("svc_cas_SiParsePrivateDataIndiDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

}


/************************************************************************/
/*	function : svc_cas_SiParseSmoothBufDes										*/
/*  																	*/
/*  Description : tag = 0x10											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseSmoothBufDes(HUINT8 *p, svcCas_SmoothBufDes_t *pDes)
{
	svcCas_SmoothBufDes_t	*pSmoothBufDes = pDes;
	HUINT16			usDesLen;
	HUINT32			ulLeakRate, ulSize;
	HUINT32			ulTemp;

	HxLOG_Info("svc_cas_SiParseSmoothBufDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 6)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseSmoothBufDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < sb leak rate (3byte); */
	ulTemp = (HUINT32)get24bit(p);
	p += 3;
	ulLeakRate = ulTemp & 0x3fffff;
	HxLOG_Print("\t\t(sb leak rate = 0x%x)\n", ulLeakRate);

	/** < sb size (3byte); */
	ulTemp = (HUINT32)get24bit(p);
	ulSize = ulTemp & 0x3fffff;
	HxLOG_Print("\t(sb size = 0x%x)\n", ulSize);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetSmoothBufDes(pSmoothBufDes, ulLeakRate, ulSize);

	HxLOG_Info(" svc_cas_SiParseSmoothBufDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseStdDes												*/
/*  																	*/
/*  Description : tag = 0x11											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseStdDes(HUINT8 *p, svcCas_StdDes_t *pDes)
{
	svcCas_StdDes_t	*pStdDes = pDes;
	HBOOL		bLeakValid;
	HUINT8		ucTemp, ucVal;
	HUINT16		usDesLen;

	HxLOG_Info("svc_cas_SiParseStdDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseStdDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < leak valid flag (1byte); */
	ucTemp = *p;
	ucVal = ucTemp & 0x01;
	(ucVal == 1) ? (bLeakValid = TRUE) : (bLeakValid = FALSE);
	HxLOG_Print("\t\t(leak valid flag = 0x%d)\n", bLeakValid);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetStdDes(pStdDes, bLeakValid);

	HxLOG_Info(" svc_cas_SiParseStdDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

STATIC	HERROR	svc_cas_SiParseCarouselIdDes(HUINT8 *p, svcCas_CarouselIdDes_t *pDes)
{
	HUINT8					ucDesLen;
	HUINT32					ulCarouselId;
	HUINT8					ucFormatId;
	svcCas_CarouselIdDesInfo_t		*pCarouselIdDesInfo;

	HxLOG_Info("svc_cas_SiParseCarouselIdDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	/** < carousel_id */
	ulCarouselId = (HUINT32)get32bit(p);
	p += 4; ucDesLen -= 4;

	/** < format_id */
	ucFormatId = *p++; ucDesLen--;

	/** < if ucFormatId == 0x01, svc_cas_SiParse deeply... */
	if (ucFormatId == 0x01)
	{
		HUINT8 ucTemp;

		pCarouselIdDesInfo = (svcCas_CarouselIdDesInfo_t *)svc_cas_SiAllocDesInfo(TAG_CAROUSEL_ID, 0);
		if (pCarouselIdDesInfo == NULL)
		{
			/** < hwlee : memory allocation error : back to normal boot */
			pDes->pCarouselIdDesInfo = NULL;
			pDes->ucFormatId = 0x00;
			return ERR_CAS_SI_RESULT_OK;
		}

		pCarouselIdDesInfo->ucModuleVersion = *p;
		p++; ucDesLen--;

		pCarouselIdDesInfo->usModuleId = (HUINT16)get16bit(p);
		p += 2; ucDesLen -= 2;

		pCarouselIdDesInfo->usBlockSize = (HUINT16)get16bit(p);
		p += 2; ucDesLen -= 2;

		pCarouselIdDesInfo->ulModuleSize = (HUINT32)get32bit(p);
		p += 4; ucDesLen -= 4;

		pCarouselIdDesInfo->ucCompressionMethod = *p;
		p++; ucDesLen--;

		pCarouselIdDesInfo->ulOriginalSize = (HUINT32)get32bit(p);
		p += 4; ucDesLen -= 4;

		pCarouselIdDesInfo->ucTimeout = *p;
		p++; ucDesLen--;

		/** < hwlee : store ucObjectKeyDataLen to ucTemp */
		ucTemp = *p;
		p++;ucDesLen--;

		if (ucTemp <= 4)
		{
			HxSTD_MemCopy(pCarouselIdDesInfo->aucObjectKeyData
					, p
					, 4);
			//p += 4;ucDesLen -= 4;
		}
		else
		{
			svc_cas_SiFreeDesInfo(TAG_CAROUSEL_ID, 0, (HUINT8 *)pCarouselIdDesInfo);
			pDes->ucFormatId = 0x00;
			pDes->pCarouselIdDesInfo = NULL;
			return ERR_CAS_SI_RESULT_OK;
		}
	}
	else
	{
		pCarouselIdDesInfo = NULL;
	}

	/** < assign parsing result */
	pDes->ulCarouselId = ulCarouselId;
	pDes->ucFormatId = ucFormatId;
	pDes->pCarouselIdDesInfo = pCarouselIdDesInfo;
	HxLOG_Info(" svc_cas_SiParseCarouselIdDes() Leaved!");
	return ERR_CAS_SI_RESULT_OK;
}

STATIC	HERROR	svc_cas_SiParseAssocTagDes(HUINT8 *p, svcCas_AssocTagDes_t *pDes)
{
	HUINT8					ucDesLen, ucSelectorLen;
	HUINT16					usAssocTag;
	HUINT16					usUse;
	HUINT32					ulTransactionId, ulTimeout;
	svcCas_AssocTagDes_t			*pAssocTagDes = pDes;

	HxLOG_Info("svc_cas_SiParseAssocTagDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseAssocTagDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < assoc_tag */
	usAssocTag = (HUINT16)get16bit(p);
	p += 2;	ucDesLen -= 2;

	/** < use */
	usUse = (HUINT16)get16bit(p);
	p += 2;	ucDesLen -= 2;

	/** < selector length */
	ucSelectorLen = *p++; ucDesLen--;
	HxLOG_Print("\t\t(selector length = %d)\n", ucSelectorLen);

	if (usUse == 0x0000)
	{
		/** < transaction_id */
		ulTransactionId = (HUINT32)get32bit(p);
		p += 4; ucDesLen -= 4;
		/** < timeout */
		ulTimeout = (HUINT32)get32bit(p);
		//p += 4; ucDesLen -= 4;
	}
	else
	{
		/** < if usUse != 0x0000, ignore selector_byte */
		ulTransactionId = ulTimeout = (HUINT32)-1;
	}

	/** < assign parsing results */
	pAssocTagDes->usAssocTag = usAssocTag;
	pAssocTagDes->usUse = usUse;
	pAssocTagDes->ulTransactionId = ulTransactionId;
	pAssocTagDes->ulTimeout = ulTimeout;

	HxLOG_Info(" svc_cas_SiParseAssocTagDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

STATIC	HERROR	svc_cas_SiParseDeferAssocTagDes(HUINT8 *p, svcCas_DeferAssocTagDes_t *pDes)
{
	HUINT8					ucDesLen;
	HUINT8					ucAssocTagLen = 0;
	HUINT16					*pusAssocTags = NULL;
	HUINT16					on_id, ts_id, prog_num;
	svcCas_DeferAssocTagDes_t		*pDeferAssocTagDes = pDes;

	HxLOG_Info("svc_cas_SiParseDeferAssocTagDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseDeferAssocTagDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < assoc_tag_length */
	ucAssocTagLen = *p;
	p++; ucDesLen--;

	/** < alloc memory for assoc_tag array */
	if (ucAssocTagLen > 0)
	{
		HUINT8 i;

		pusAssocTags = (HUINT16 *)OxCAS_Malloc(ucAssocTagLen);

		/** < handling unErr : NIY */
		for(i = 0; i < ucAssocTagLen/2; i++)
		{
			pusAssocTags[i] = (HUINT16)get16bit(p);
			p += 2; ucDesLen -= 2;
		}
	}

	ts_id = (HUINT16)get16bit(p);
	p += 2; ucDesLen -= 2;

	prog_num = (HUINT16)get16bit(p);
	p += 2; ucDesLen -= 2;

	on_id = (HUINT16)get16bit(p);
	//p += 2; ucDesLen -= 2;

	/** < assign parsing results */
	pDeferAssocTagDes->ucAssocTagSize = (HUINT8)(ucAssocTagLen/2);
	pDeferAssocTagDes->pusAssocTags = pusAssocTags;
	pDeferAssocTagDes->usTransportStreamId = ts_id;
	pDeferAssocTagDes->usProgNum = prog_num;
	pDeferAssocTagDes->usOriginalNetworkId = on_id;

	HxLOG_Info(" svc_cas_SiParseDeferAssocTagDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParseAvcVideoDes											*/
/*  																	*/
/*  Description : tag = 0x28											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseAvcVideoDes(HUINT8 *p, svcCas_AvcVideoDes_t *pDes)
{
	svcCas_AvcVideoDes_t	*pAvcVideoDes = pDes;
	HUINT8			ucDesLen;
	HUINT8			ucTemp;

	HxLOG_Info("svc_cas_SiParseAvcVideoDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseAvcVideoDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < Profile idc (1byte) */
	pAvcVideoDes->ucProfile = *p++;
	HxLOG_Print("\t\t(profile idc = 0x%x)\n", pAvcVideoDes->ucProfile);

	/** < constraint_set_flag & AVC_compatible_flags (1byte) */
	ucTemp = *p++;
	pAvcVideoDes->bConstSet0Flag 		= (HBOOL)((ucTemp & 0x80) >> 7);
	pAvcVideoDes->bConstSet1Flag 		= (HBOOL)((ucTemp & 0x40) >> 6);
	pAvcVideoDes->bConstSet2Flag 		= (HBOOL)((ucTemp & 0x20) >> 5);
	pAvcVideoDes->ucAvcCompatibleFlags 	= (HUINT8)((ucTemp & 0x1f));
	HxLOG_Print("\t\t(constraint_set0_flag = 0x%x)\n", pAvcVideoDes->bConstSet0Flag);
	HxLOG_Print("\t\t(constraint_set1_flag = 0x%x)\n", pAvcVideoDes->bConstSet1Flag);
	HxLOG_Print("\t\t(constraint_set2_flag = 0x%x)\n", pAvcVideoDes->bConstSet1Flag);
	HxLOG_Print("\t\t(VC_compatible_flags = 0x%x)\n",pAvcVideoDes->ucAvcCompatibleFlags);

	/** < level_idc */
	pAvcVideoDes->ucLevel		= *p++;
	HxLOG_Print("\t\t(level idc = 0x%x)\n", pAvcVideoDes->ucLevel);

	/** < AVC_still_present & AVC_24_hour_picture_flag (1byte) */
	ucTemp = *p++;
	pAvcVideoDes->ucAvcStillPresent	= (HUINT8)((ucTemp & 0x80) >> 7);
	pAvcVideoDes->bAvc24hourPicFlag	= (HBOOL)((ucTemp & 0x40) >> 6);
	HxLOG_Print("\t\t(Avc_still_present = 0x%x)\n", pAvcVideoDes->ucAvcStillPresent);
	HxLOG_Print("\t\t(AVC_24Hour_picture_flag = 0x%x)\n", pAvcVideoDes->bAvc24hourPicFlag);

	HxLOG_Info(" svc_cas_SiParseAvcVideoDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

}


/************************************************************************/
/*	function : svc_cas_SiParseNetNameDes											*/
/*  																	*/
/*  Description : tag = 0x40											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseNetNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_NetNameDes_t *pDes)
{
	svcCas_NetNameDes_t	*pNetNameDes = pDes;
	HUINT8			*pszNetName = NULL;
	HUINT16			usDesLen;
	HUINT32			nTempNetNameLen, nAssignedNetNameLen;

	HxLOG_Info("svc_cas_SiParseNetNameDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen == 0)
		goto ErrBlock;

	/** < Max allocation limit & allocation */
	nAssignedNetNameLen = (usDesLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usDesLen);

	/** < network name */
	nTempNetNameLen = svc_cas_SiPreStrMgr(eTextEnc, (HUINT8 *) NULL, (HUINT8)nAssignedNetNameLen, p, &pszNetName);
	HxLOG_Print("\t\t(network name = %s)\n", (char *)pszNetName);

	/** < Descriptor 데이타 구성. */
	pNetNameDes->pszNetName = svc_cas_SiAllocMem(nTempNetNameLen+1);
	if (pNetNameDes->pszNetName == NULL)
		goto ErrBlock;
	svc_cas_SiSetNetNameDes(pNetNameDes, pszNetName);
	if(pszNetName != NULL)
		OxCAS_Free (pszNetName);

	HxLOG_Info(" svc_cas_SiParseNetNameDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if (pszNetName != NULL) 
		OxCAS_Free ( pszNetName );
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}



/************************************************************************/
/*	function : svc_cas_SiParseServListDes											*/
/*  																	*/
/*  Description : tag = 0x41											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseServListDes(HUINT8 *p, svcCas_SvcListDes_t *pDes)
{
	svcCas_SvcListDes_t	*pServListDes = pDes;
	svcCas_SvcListInfo_t	*pNewServListInfo = NULL, *pNextServListInfo = NULL, *pPrevServListInfo = NULL;
	HUINT16			usDesLen;
	HUINT16			usSvcId;
	HUINT8			ucServType;

	HxLOG_Info("svc_cas_SiParseServListDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 3) != 0)
	{
		HxLOG_Print("[WARN] Unknown ServListDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < service id (2byte) */
		usSvcId = (HUINT16)get16bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t\t(service id = 0x%x)\n", usSvcId);

		/** < service type (1byte) */
		ucServType = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(servie type = 0x%x)\n", ucServType);

		/** < Descriptor 데이타 구성. */
		pPrevServListInfo = svc_cas_SiGetPrevServListInfo(pServListDes);
		pNewServListInfo = (svcCas_SvcListInfo_t *)svc_cas_SiAllocDesInfo(TAG_SERVICE_LIST, 0);
		if (pNewServListInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetServListInfo(pNewServListInfo, usSvcId, ucServType);
		if (pPrevServListInfo == NULL)
			svc_cas_SiLinkServListInfoHead(pServListDes, pNewServListInfo);
		svc_cas_SiLinkServListInfoNode(pNewServListInfo, pNextServListInfo, pPrevServListInfo);
	}

	HxLOG_Info(" svc_cas_SiParseServListDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}

/************************************************************************/
/*	function : svc_cas_SiParseSatDeliSystemDes									*/
/*  																	*/
/*  Description : tag = 0x43											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseSatDeliSystemDes(HUINT8 *p, svcCas_SatDeliSystemDes_t *pDes)
{
	svcCas_SatDeliSystemDes_t	*pSatDeliSystemDes = pDes;
	HUINT8				ucPolar, ucModul, ucModType, ucFECInner = 0;
	HUINT16				usDesLen;
	HUINT16				wOrbitPos;
	HUINT32				ulFreq, dwSymbolRate;
	HBOOL				bWestEastFlag;
	HUINT8				ucTemp, ucVal;
	HUINT32				ulTemp;
	HUINT8				ucTransSpec;
	HUINT8 				ucRollOff = 0;


	HxLOG_Info("svc_cas_SiParseSatDeliSystemDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 11)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseSatDeliSystemDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < frequency (4byte) */
	ulFreq = (HUINT32)BcdToBinary(get32bit(p));
	p += 4;
	HxLOG_Print("\t\t(frequency = %d)\n", ulFreq);

	/** < orbital position (2byte) */
	wOrbitPos = (HUINT16)BcdToBinary(get16bit(p));
	p += 2;
	HxLOG_Print("\t\t(orbital position = %d)\n", wOrbitPos);

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
	ulTemp = get32bit(p);
	dwSymbolRate = (HUINT32)BcdToBinary((ulTemp & 0xfffffff0) >> 4);
	ucFECInner = (HUINT8)(ulTemp & 0x0000000f);
	HxLOG_Print("\t\t(symbol rate = %d)\n", dwSymbolRate);
	HxLOG_Print("\t\t(FEC inner = 0x%x)\n", ucFECInner);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetSatDeliSystemDes(pSatDeliSystemDes, ulFreq, wOrbitPos, bWestEastFlag, ucPolar, ucModul, ucModType,
						dwSymbolRate, ucFECInner, ucTransSpec, ucRollOff);

	HxLOG_Info(" svc_cas_SiParseSatDeliSystemDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseCabDeliSystemDes									*/
/*  																	*/
/*  Description : tag = 0x44											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseCabDeliSystemDes(HUINT8 *p, svcCas_CabDeliSystemDes_t *pDes)
{
	svcCas_CabDeliSystemDes_t	*pCabDeliSystemDes = pDes;
	HUINT8				ucFECOuter, ucModul, ucFECInner;
	HUINT16				usDesLen;
	HUINT32				ulFreq, dwSymbolRate;
	HUINT32				ulTemp;

	HxLOG_Info("svc_cas_SiParseCabDeliSystemDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 11)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseCabDeliSystemDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < frequency (4byte) */
	ulFreq = (HUINT32)BcdToBinary(get32bit(p));
	p += 4;
	HxLOG_Print("\t\t(frequency = %d)\n", ulFreq/10);

	/** < reserved & FEC outer (2byte) */
	ucFECOuter = (HUINT8)BcdToBinary(get16bit(p) & 0x000f);
	p += 2;
	HxLOG_Print("\t\t(FEC outer = 0x%x)\n", ucFECOuter);

	/** < modulation (1byte) */
	ucModul = *p++;
	HxLOG_Print("\t\t(modulation = 0x%x)\n", ucModul);

	/** < symbol rate & FEC inner (4byte) */
	ulTemp = get32bit(p);
	dwSymbolRate = (HUINT32)BcdToBinary((ulTemp & 0xfffffff0) >> 4);
	ucFECInner = (HUINT8)(ulTemp & 0x0000000f);
	HxLOG_Print("\t\t(symbol rate = %d)\n", dwSymbolRate/10);
	HxLOG_Print("\t\t(FEC inner = 0x%x)\n", ucFECInner);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetCabDeliSystemDes(pCabDeliSystemDes, ulFreq, ucFECOuter, ucModul, dwSymbolRate, ucFECInner);

	HxLOG_Info(" svc_cas_SiParseCabDeliSystemDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseBouqNameDes											*/
/*  																	*/
/*  Description : tag = 0x47											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseBouqNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_BouqNameDes_t *pDes)
{
	svcCas_BouqNameDes_t	*pBouqNameDes = pDes;
	HUINT8			*pszBouqName = NULL;
	HUINT16			usDesLen;
	HUINT32			nTempBouqNameLen, nAssignedBouqNameLen;

	HxLOG_Info("svc_cas_SiParseBouqNameDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen == 0)
		goto ErrBlock;

	nAssignedBouqNameLen = (usDesLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usDesLen);

	/** < bouquet name */
	nTempBouqNameLen = svc_cas_SiPreStrMgr(eTextEnc, (HUINT8 *)NULL, (HUINT8)nAssignedBouqNameLen, p, &pszBouqName);
	HxLOG_Print("\t\t(bouquet name = %s)\n", (char *)pszBouqName);

	/** < Descriptor 데이타 구성. */
	pBouqNameDes->pszBouqName = svc_cas_SiAllocMem(nTempBouqNameLen+1);
	if (pBouqNameDes->pszBouqName == NULL)
		goto ErrBlock;
	svc_cas_SiSetBouqNameDes(pBouqNameDes, pszBouqName);
	if(pszBouqName != NULL)
		OxCAS_Free (pszBouqName);

	HxLOG_Info(" svc_cas_SiParseBouqNameDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if (pszBouqName != NULL) OxCAS_Free (pszBouqName);
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseServDes												*/
/*  																	*/
/*  Description : tag = 0x48											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseServDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_SvcDes_t *pDes)
{
	svcCas_SvcDes_t	*pServDes = pDes;
	HUINT8		ucServType;
	HUINT8	 	ucProviderNameLen, ucServNameLen;
	HUINT8		*pszProviderName = NULL, *pszServName =NULL;
	HUINT16		usDesLen;
	HUINT32		nTempProviderNameLen, nTempServNameLen;

	HxLOG_Info("svc_cas_SiParseServDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen == 0)
		goto ErrBlock;

	/** < service type (1byte) */
	ucServType = *p++;
	HxLOG_Print("\t\t(service type = 0x%x)\n", ucServType);

	/** < service provider name length (1byte) */
	ucProviderNameLen = *p++;
	HxLOG_Print("\t\t(service provider name length = %d)\n", ucProviderNameLen);

	/** < provider name */
	nTempProviderNameLen = svc_cas_SiPreStrMgr(eTextEnc, (HUINT8 *)NULL, ucProviderNameLen, p, &pszProviderName);
	p += ucProviderNameLen;
	HxLOG_Print("\t\t(service provider name = %s)\n", (char *)pszProviderName);

	/** < service name length (1byte) */
	ucServNameLen = *p++;
	HxLOG_Print("\t\t(service name length = %d)\n", ucServNameLen);

	/** < service name */
	nTempServNameLen = svc_cas_SiPreStrMgr(eTextEnc, (HUINT8 *)NULL, ucServNameLen, p, &pszServName);
	HxLOG_Print("\t\t(service name = %s)\n", (char *)pszServName);

	/** < Descriptor 데이타 구성. */
	pServDes->pszProviderName = svc_cas_SiAllocMem(nTempProviderNameLen+1);
	if (pServDes->pszProviderName == NULL)
		goto ErrBlock;
	pServDes->pszServName = svc_cas_SiAllocMem(nTempServNameLen+1);
	if (pServDes->pszServName == NULL)
		goto ErrBlock;
	svc_cas_SiSetServDes(pServDes, ucServType, pszProviderName, pszServName);
	if(pszProviderName != NULL)
		OxCAS_Free ( pszProviderName );
	if(pszServName != NULL)
		OxCAS_Free ( pszServName );

	pServDes->nPrvName = nTempProviderNameLen + 1;
	pServDes->nSvcName = nTempServNameLen + 1;

	HxLOG_Info(" svc_cas_SiParseServDes() Leaved!\n");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if ( pszProviderName != NULL ) OxCAS_Free ( pszProviderName );
	if ( pszServName != NULL ) OxCAS_Free ( pszServName );
	if (pServDes->pszProviderName != NULL)
		svc_cas_SiFreeMem(pServDes->pszProviderName);
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseCountryAvailDes										*/
/*  																	*/
/*  Description : tag = 0x49											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseCountryAvailDes (HUINT8 *p, svcCas_CountryAvailDes_t *pDes)
{
	svcCas_CountryAvailDes_t	*pCountryAvailDes = pDes;
	svcCas_CountryAvailInfo_t	*pNewCountryAvailInfo = NULL, *pPrevCountryAvailInfo = NULL, *pNextCountryAvailInfo = NULL;
	HBOOL				bCountryAvailAvail;
	HUINT8				szCountryAvailCode[4];
	HUINT8				ucTemp, ucVal;
	HUINT16				usDesLen;

	HxLOG_Info("svc_cas_SiParseCountryAvailDes() Entered!");

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
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < country code (3byte) */
		HxSTD_MemCopy (szCountryAvailCode, p, 3);
		szCountryAvailCode [3] = '\0';
		svc_cas_SiMakeCorrct639Code(szCountryAvailCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(country code = %s)\n", (char *)szCountryAvailCode);

		/** < Descriptor 데이타 구성. */
		pPrevCountryAvailInfo = svc_cas_SiGetPrevCountryAvailInfo (pCountryAvailDes);
		pNewCountryAvailInfo = (svcCas_CountryAvailInfo_t *) svc_cas_SiAllocDesInfo  (TAG_COUNTRY_AVAILABILITY, 0);
		if (pNewCountryAvailInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetCountryAvailInfo (pNewCountryAvailInfo, szCountryAvailCode);
		if (pPrevCountryAvailInfo == NULL)
			svc_cas_SiLinkCountryAvailInfoHead (pCountryAvailDes, pNewCountryAvailInfo);
		svc_cas_SiLinkCountryAvailInfoNode (pNewCountryAvailInfo, pNextCountryAvailInfo, pPrevCountryAvailInfo);
	}

	HxLOG_Info(" svc_cas_SiParseCountryAvailDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/** < define 과 함께 다시 정리 필요함. hcyoo */
/************************************************************************/
/*	function : svc_cas_SiParseLinkDes												*/
/*  																	*/
/*  Description : tag = 0x4a											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseLinkDes (HUINT8 *p, svcCas_LinkDes_t *pDes)
{
	svcCas_LinkDes_t			*pLinkDes = pDes;
	svcCas_PrivateDataInfo_t	*pNewPrivateDataInfo = NULL;
	HUINT16				usDesLen;

	HxLOG_Info("svc_cas_SiParseLinkDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 7)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseLinkDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < transport stream id (2byte) */
	pLinkDes->usTsId = (HUINT16) get16bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(transport stream id = 0x%x)\n", pLinkDes->usTsId);

	/** < original network id (2byte) */
	pLinkDes->usOrgNetId = (HUINT16) get16bit (p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(original network id = 0x%x)\n", pLinkDes->usOrgNetId);

	/** < service id (2byte) */
	pLinkDes->usSvcId = (HUINT16) get16bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(service id = 0x%x)\n", pLinkDes->usSvcId);

	/** < linkage type (1byte) */
	pLinkDes->ucLinkType = *p++;
	usDesLen--;
	HxLOG_Print("\t\t(linkage type = 0x%x)\n", pLinkDes->ucLinkType);

	/** < Private Data */
	if (usDesLen > 0)
	{
		pNewPrivateDataInfo = (svcCas_PrivateDataInfo_t *) svc_cas_SiAllocDesInfo  (TAG_PRIVATE_DATA, 0);
		if (pNewPrivateDataInfo == NULL)
			return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
		svc_cas_SiSetPrivateDataInfo (pNewPrivateDataInfo, p, usDesLen);
	}
	/** < Descriptor 데이타 구성. */
	pLinkDes->pPrivateDataInfo = pNewPrivateDataInfo;

	HxLOG_Info("svc_cas_SiParseLinkDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParseNvodRefDes											*/
/*  																	*/
/*  Description : tag = 0x4b											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseNvodRefDes (HUINT8 *p, svcCas_NvodRefDes_t *pDes)
{
	svcCas_NvodRefDes_t		*pNvodRefDes = pDes;
	svcCas_NvodRefInfo_t	*pstNewNvodRefInfo = NULL, *pstPrevNvodRefInfo = NULL, *pstNextNvodRefInfo = NULL;
	HUINT16					usDesLen;
	HUINT16					wTsId, wOrigNetId, usSvcId;

	HxLOG_Info("svc_cas_SiParseNvodRefDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 6) != 0)
	{
		HxLOG_Print("[WARN] Unknown NvodRefDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < transport stream id (2byte) */
		wTsId = (HUINT16) get16bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(transport stream id = 0x%x)\n", wTsId);

		/** < original network id (2byte) */
		wOrigNetId = (HUINT16) get16bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(original network id = 0x%x)\n", wOrigNetId);

		/** < service id (2byte) */
		usSvcId = (HUINT16) get16bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t(service id = 0x%x)\n", usSvcId);

		/** < Descriptor 데이타 구성. */
		pstPrevNvodRefInfo = svc_cas_SiGetPrevNvodRefInfo (pNvodRefDes);
		pstNewNvodRefInfo = (svcCas_NvodRefInfo_t *) svc_cas_SiAllocDesInfo  (TAG_NVOD_REFERENCE, 0);
		if (pstNewNvodRefInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetNvodRefInfo (pstNewNvodRefInfo, wTsId, wOrigNetId, usSvcId);
		if (pstPrevNvodRefInfo == NULL)
			svc_cas_SiLinkNvodRefInfoHead (pNvodRefDes, pstNewNvodRefInfo);
		svc_cas_SiLinkNvodRefInfoNode (pstNewNvodRefInfo, pstNextNvodRefInfo, pstPrevNvodRefInfo);
	}

	HxLOG_Info(" svc_cas_SiParseNvodRefDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseTimeShiftServDes									*/
/*  																	*/
/*  Description : tag = 0x4c											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseTimeShiftServDes (HUINT8 *p, svcCas_TimeShiftSvcDes_t *pDes)
{
	svcCas_TimeShiftSvcDes_t	*pTimeShiftServDes = pDes;
	HUINT16				usDesLen;
	HUINT16				wRefServId;

	HxLOG_Info("svc_cas_SiParseTimeShiftServDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 2)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseTimeShiftServDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < reference service id (2byte) */
	wRefServId = (HUINT16) get16bit(p);
	HxLOG_Print("\t\t(reference service id = 0x%x)\n", wRefServId);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetTimeShiftServDes (pTimeShiftServDes, wRefServId);

	HxLOG_Info("svc_cas_SiParseTimeShiftServDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseShortEvtDes											*/
/*  																	*/
/*  Description : tag = 0x4d											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseShortEvtDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_ShortEventDes_t *pDes)
{
	svcCas_ShortEventDes_t	*pShortEvtDes = pDes;
	HUINT8			ucEvtNameLen;
	HUINT8			szIso639LangCode[4], *pszEvtName = NULL, *pszText = NULL;
	HUINT16			usTextLen;
	HINT16			usDesLen;
	HUINT32			nTempEvtNameLen, nTempTextLen;

	HxLOG_Info("svc_cas_SiParseShortEvtDes() Entered!\n");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = (HINT16)*p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if(usDesLen < 6)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseShortEvtDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < ISO 639 language code (3byte) */
	HxSTD_MemCopy(szIso639LangCode, p, 3);
	szIso639LangCode[3] = '\0';
	svc_cas_SiMakeCorrct639Code(szIso639LangCode);
	p += 3;
	usDesLen -= 3;
	HxLOG_Print("\t\t(ISO 639 language code = %s)\n", (char *)szIso639LangCode);

	/** < event name length (1byte) */
	ucEvtNameLen = *p++;
	usDesLen--;
	HxLOG_Print("\t\t(event name length = %d)\n", ucEvtNameLen);

	if(usDesLen < ucEvtNameLen)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseShortEvtDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}
	/** < Max allocation limit & allocation */
#ifdef EVTNAME_DEBUG
	{
		HUINT32 i,j;
		HxLOG_Print("Event name(raw data -len %d, %s)\n",ucEvtNameLen,szIso639LangCode);
		PrintMemDump (p, ucEvtNameLen);
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
	nTempEvtNameLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, ucEvtNameLen, p, &pszEvtName);
	p += ucEvtNameLen;
	usDesLen -= ucEvtNameLen;
#ifdef EVTNAME_DEBUG
	{
		HUINT32 i;
		HxLOG_Print("\tString Len:%d\n\t", nTempEvtNameLen);
		PrintMemDump (pszEvtName, nTempEvtNameLen);
		HxLOG_Info("\n\n");
	}
#else
	HxLOG_Print("\t\t(event name = %s)\n", (char *)pszEvtName);
#endif

	if(usDesLen <= 0)
	{
		/** < jmyang, 2003.07.15 for ukrine cable
			  현지 스트림에서 Descriptor length가 Event name까지만 있는 버그로 인해서, 정상동작하지 않았었음. */
		HxLOG_Print("[svc_cas_SiParseShortEvtDes] OOPs: Descriptor Length is lack = [%d]\n", usDesLen-1);
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
			if (pszEvtName != NULL) OxCAS_Free ( pszEvtName );
			HxLOG_Print("[WARN] Unknown svc_cas_SiParseShortEvtDes Format!\n");
			return ERR_CAS_SI_INVALID_DESCRIPTOR;
		}
	}

	/** < Max allocation limit & allocation */
#ifdef EVTNAME_DEBUG
	{
		HUINT32 i,j;
		HxLOG_Print("Text (raw data -len %d, %s)\n",usTextLen, szIso639LangCode);
		PrintMemDump (p, usTextLen);
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
	nTempTextLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, usTextLen, p, &pszText);
#ifdef EVTNAME_DEBUG
	{
		HUINT32 i;
		HxLOG_Print("\tText Len:%d\n\t", nTempTextLen);
		PrintMemDump (pszText, nTempTextLen);
	}
#else
	HxLOG_Print("\t\t(text = %s)\n", (char *)pszText);
#endif

	/** < Descriptor 데이타 구성. */
	pShortEvtDes->pszEvtName = svc_cas_SiAllocMem(nTempEvtNameLen+1);
	if (pShortEvtDes->pszEvtName == NULL)
		goto ErrBlock;
	pShortEvtDes->pszText = svc_cas_SiAllocMem(nTempTextLen+1);
	if (pShortEvtDes->pszText == NULL)
		goto ErrBlock;
	svc_cas_SiSetShortEvtDes(pShortEvtDes, szIso639LangCode, pszEvtName, pszText);
	if(pszEvtName != NULL)
		OxCAS_Free ( pszEvtName );
	if(pszText != NULL)
		OxCAS_Free ( pszText );

	HxLOG_Info(" svc_cas_SiParseShortEvtDes() Leaved!\n");

	pShortEvtDes->ucEvtNameSize = nTempEvtNameLen + 1;
	pShortEvtDes->ucTextSize = nTempTextLen + 1;

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if (pszEvtName != NULL) OxCAS_Free ( pszEvtName );
	if (pszText != NULL ) OxCAS_Free ( pszText );
	if (pShortEvtDes->pszEvtName != NULL)
		svc_cas_SiFreeMem(pShortEvtDes->pszEvtName);
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseExtEvtDes											*/
/*  																	*/
/*  Description : tag = 0x4e											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseExtEvtDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_ExtEvtDes_t *pDes)
{
	svcCas_ExtEvtDes_t		*pExtEvtDes = pDes;
	svcCas_ExtEvtInfo_t	*pPrevExtEvtInfo = NULL, *pNewExtEvtInfo = NULL, *pNextExtEvtInfo = NULL;
	HUINT8			ucDesNum, ucLastDesNum;
	HUINT8			szIso639LangCode[4], *pszItemDes = NULL, *pszItem = NULL, *pszText = NULL;
	HUINT8			*pszTextChar = NULL;
	HUINT8			ucTemp;
	HUINT16			wItemDesLen, wItemLen, usTextLen;
	HINT16			usDesLen, wLenOfItem;
	HUINT32			nTempItemDesLen, nTempItemLen, nTempTextLen;
	HUINT32			nAssignedItemDesLen, nAssignedItemLen, nAssignedTextLen;

	HxLOG_Info("svc_cas_SiParseShortEvtDes() Entered!\n");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = (HINT16)*p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 5)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseShortEvtDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
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
	svc_cas_SiMakeCorrct639Code(szIso639LangCode);
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
		wItemDesLen = *p++;
		wLenOfItem--;
		usDesLen--;
		HxLOG_Print("\t\t\t(item description length = %d)\n", wItemDesLen);

		/** < Max allocation limit & allocation */
		nAssignedItemDesLen = (wItemDesLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : wItemDesLen);

		/** < item des */
		nTempItemDesLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, (HUINT8)nAssignedItemDesLen, p, &pszItemDes);
		p += wItemDesLen;
		wLenOfItem -= wItemDesLen;
		usDesLen -= wItemDesLen;
		HxLOG_Print("\t\t\t(item description = %s)\n", (char *)pszItemDes);

		/** < item length (1byte) */
		wItemLen = *p++;
		wLenOfItem--;
		usDesLen--;
		HxLOG_Print("\t\t\t(item length = %d)\n", wItemLen);

		/** < Max allocation limit & allocation */
		nAssignedItemLen = (wItemLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : wItemLen);

		/** < item */
		nTempItemLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, (HUINT8)nAssignedItemLen, p, &pszItem);

		p += wItemLen;
		wLenOfItem -= wItemLen;
		usDesLen -= wItemLen;
		HxLOG_Print("\t\t\t(item = %s)\n", (char *)pszItem);

		/** < Descriptor 데이타 구성. */
		pPrevExtEvtInfo = svc_cas_SiGetPrevExtEvtInfo(pExtEvtDes);
		pNewExtEvtInfo = (svcCas_ExtEvtInfo_t *)svc_cas_SiAllocDesInfo(TAG_EXTENDED_EVENT, 0);
		if (pNewExtEvtInfo == NULL)
			goto ErrBlock1;
		pNewExtEvtInfo->pszDescription = svc_cas_SiAllocMem(nTempItemDesLen+1);
		if (pNewExtEvtInfo->pszDescription == NULL)
			goto ErrBlock1;
		pNewExtEvtInfo->pszItem = svc_cas_SiAllocMem(nTempItemLen+1);
		if (pNewExtEvtInfo->pszItem == NULL)
			goto ErrBlock1;

		svc_cas_SiSetExtEvtInfo(pNewExtEvtInfo, pszItemDes, pszItem);
		if(pszItemDes != NULL)
		{
			OxCAS_Free ( pszItemDes );
			pszItemDes = NULL;
		}

		if(pszItem != NULL)
		{
			OxCAS_Free ( pszItem );
			pszItem = NULL;
		}

		if (pPrevExtEvtInfo == NULL)
			svc_cas_SiLinkExtEvtInfoHead(pExtEvtDes, pNewExtEvtInfo);
		svc_cas_SiLinkExtEvtInfoNode(pNewExtEvtInfo, pNextExtEvtInfo, pPrevExtEvtInfo);
	}

	/** < text length (1byte) */
	usTextLen = *p++;
	HxLOG_Print("\t\t(text length = %d)\n", usTextLen);
#ifdef EXTEVTTEXT_DEBUG
	{
		HUINT32i;
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
	pExtEvtDes->pucTextChar = svc_cas_SiDupMem(p, (HUINT32)usTextLen);
	if (pExtEvtDes->pucTextChar == NULL)
		goto ErrBlock2;
	pExtEvtDes->ucTextLength = (HUINT8)usTextLen;

	/** < Max allocation limit & allocation */
	nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);

	/** < text */
	nTempTextLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, (HUINT8)nAssignedTextLen, p, &pszText);
	HxLOG_Print("\t\t(text = %s)\n", pszText);

	/** < Descriptor 데이타 구성. */
	pExtEvtDes->pszChar = svc_cas_SiAllocMem(nTempTextLen+1);
	if (pExtEvtDes->pszChar == NULL)
		goto ErrBlock2;
	svc_cas_SiSetExtEvtDes(pExtEvtDes, ucDesNum, ucLastDesNum, szIso639LangCode, pszText);
	if(pszText != NULL)
		OxCAS_Free ( pszText );

	HxLOG_Info(" svc_cas_SiParseShortEvtDes() Leaved!\n");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock1:
	if ( pszItemDes != NULL ) OxCAS_Free ( pszItemDes );
	if ( pszItem != NULL ) OxCAS_Free ( pszItem );
	if ( pszText != NULL ) OxCAS_Free ( pszText );
	if ( pszTextChar != NULL) OxCAS_Free( pszTextChar );

	if (pNewExtEvtInfo != NULL)
	{
		if (pNewExtEvtInfo->pszDescription != NULL)
		{
			svc_cas_SiFreeMem(pNewExtEvtInfo->pszDescription);
		}

		svc_cas_SiFreeDesInfo(TAG_EXTENDED_EVENT, 0, (HUINT8 *)pNewExtEvtInfo);
	}

	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;


ErrBlock2:
	if ( pszItemDes != NULL ) OxCAS_Free ( pszItemDes );
	if ( pszItem != NULL ) OxCAS_Free ( pszItem );
	if ( pszText != NULL ) OxCAS_Free ( pszText );
	if ( pszTextChar != NULL) OxCAS_Free( pszTextChar );

	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseTimeShiftEvtDes										*/
/*  																	*/
/*  Description : tag = 0x4f											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseTimeShiftEvtDes(HUINT8 *p, svcCas_TimeShiftEvtDes_t *pDes)
{
	svcCas_TimeShiftEvtDes_t	*pTimeShiftEvtDes = pDes;
	HUINT16				usDesLen;
	HUINT16				wRefServId, wRefEvtId;

	HxLOG_Info("svc_cas_SiParseTimeShiftEvtDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseTimeShiftEvtDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < reference service id (2byte) */
	wRefServId = (HUINT16)get16bit(p);
	p += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(reference service id = 0x%x)\n", wRefServId);

	/** < reference event id (2byte) */
	wRefEvtId = get16bit(p);
	/** <p += 2; */
	/** <usDesLen -= 2; */
	HxLOG_Print("\t\t(reference event id = 0x%x)\n", wRefEvtId);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetTimeShiftEvtDes(pTimeShiftEvtDes, wRefServId, wRefEvtId);

	HxLOG_Info(" svc_cas_SiParseTimeShiftEvtDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseCompDes												*/
/*  																	*/
/*  Description : tag = 0x50											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseCompDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_CompDes_t *pDes)
{
	svcCas_CompDes_t	*pCompDes = pDes;
	HUINT8		ucStmContent, ucCompType, ucCompTag;
	HUINT8		szIso639LangCode [4], *pszText = NULL;
	HUINT16		usDesLen, usTextLen;
	HUINT32		nTempTextLen;
	HUINT32		nAssignedTextLen;

	HxLOG_Info("svc_cas_SiParseCompDes() Entered!");

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
	svc_cas_SiMakeCorrct639Code(szIso639LangCode);
	p += 3;
	usDesLen -= 3;
	HxLOG_Print("\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

	/** < text */
	usTextLen = usDesLen;

	/** < Max allocation limit & allocation */
	nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);

	/** < event name */
	nTempTextLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, (HUINT8)nAssignedTextLen, p, &pszText);
	HxLOG_Print("\t\t(text = %s)\n", pszText);

	/** < Descriptor 데이타 구성. */
	pCompDes->pszText = svc_cas_SiAllocMem (nTempTextLen + 1);
	if (pCompDes->pszText == NULL)
		goto ErrBlock;
	svc_cas_SiSetCompDes (pCompDes, ucStmContent, ucCompType, ucCompTag, szIso639LangCode, pszText);
	if(pszText != NULL)
		OxCAS_Free  ( pszText );

	HxLOG_Info("svc_cas_SiParseCompDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if ( pszText != NULL ) OxCAS_Free ( pszText );
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/***********************************************************************/
/*	function : svc_cas_SiParseMosaicDes														*/
/*																				*/
/*  Description : tag = 0x51															*/
/***********************************************************************/
STATIC	HERROR svc_cas_SiParseMosaicDes (HUINT8 *p, svcCas_MosaicDes_t *pDes)
{
	HUINT8			ucTemp;
	HUINT16			usDesLen;

	HxLOG_Info("svc_cas_SiParseStmIdentDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseStmIdentDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	ucTemp = *p;

	pDes->ucEntryPoint = (HUINT8)((ucTemp >> 7) & 0x01);
	pDes->ucHoriElemCellNum = (HUINT8)((ucTemp >> 4) & 0x07);
	pDes->ucVertElemCellNum = (HUINT8)((ucTemp >> 0) & 0x07);

	HxLOG_Info("svc_cas_SiParseStmIdentDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseStmIdentDes											*/
/*  																	*/
/*  Description : tag = 0x52											*/
/************************************************************************/

STATIC 	HUINT32	svc_cas_SiParseStmIdentDes (HUINT8 *p, svcCas_StmIdentDes_t *pDes)
{
	svcCas_StmIdentDes_t	*pStmIdentDes = pDes;
	HUINT8			ucCompTag;
	HUINT16			usDesLen;

	HxLOG_Info("svc_cas_SiParseStmIdentDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseStmIdentDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < component tag (1byte) */
	ucCompTag = *p;
	HxLOG_Print("\t\t(component tag = 0x%x)\n", ucCompTag);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetStmIdentDes (pStmIdentDes, ucCompTag);

	HxLOG_Info("svc_cas_SiParseStmIdentDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParseCaIdentDes											*/
/*  																	*/
/*  Description : tag = 0x53											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseCaIdentDes (HUINT8 *p, svcCas_CaIdentDes_t *pDes)
{
	svcCas_CaIdentDes_t	*pCaIdentDes = pDes;
	svcCas_CaIdentInfo_t	*pPrevCaIdentInfo = NULL, *pNewCaIdentInfo = NULL, *pNextCaIdentInfo = NULL;
	HUINT16			usDesLen;
	HUINT16			wCaStmId;

	HxLOG_Info("svc_cas_SiParseCaIdentDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 2) != 0)
	{
		HxLOG_Print("[WARN] Unknown CaIdentDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < CA system id (2byte) */
		wCaStmId = (HUINT16) get16bit (p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t\t(CA system id = 0x%x)\n", wCaStmId);

		/** < Descriptor 데이타 구성. */
		pPrevCaIdentInfo = svc_cas_SiGetPrevCaIdentInfo (pCaIdentDes);
		pNewCaIdentInfo = (svcCas_CaIdentInfo_t *) svc_cas_SiAllocDesInfo  (TAG_CA_IDENTIFIER, 0);
		if (pNewCaIdentInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetCaIdentInfo (pNewCaIdentInfo, wCaStmId);
		if (pPrevCaIdentInfo == NULL)
			svc_cas_SiLinkCaIdentInfoHead (pCaIdentDes, pNewCaIdentInfo);
		svc_cas_SiLinkCaIdentInfoNode (pNewCaIdentInfo, pNextCaIdentInfo, pPrevCaIdentInfo);
	}

	HxLOG_Info("svc_cas_SiParseCaIdentDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseContentDes											*/
/*  																	*/
/*  Description : tag = 0x54											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseContentDes(HUINT8 *p, svcCas_ContentDes_t *pDes)
{
	svcCas_ContentDes_t		*pContentDes = pDes;
	svcCas_ContentInfo_t	*pPrevContentInfo = NULL, *pNewContentInfo = NULL, *pNextContentInfo = NULL;
	HUINT8			ucContentNibble, ucUserNibble;
	HUINT16			usDesLen;

	HxLOG_Info("svc_cas_SiParseContentDes() Entered!\n");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 2) != 0)
	{
		HxLOG_Print("[WARN] Unknown ContentDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
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
		pPrevContentInfo = svc_cas_SiGetPrevContentInfo(pContentDes);
		pNewContentInfo = (svcCas_ContentInfo_t *)svc_cas_SiAllocDesInfo(TAG_CONTENT, 0);
		if (pNewContentInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetContentInfo(pNewContentInfo, ucContentNibble, ucUserNibble);
		if (pPrevContentInfo == NULL)
			svc_cas_SiLinkContentInfoHead(pContentDes, pNewContentInfo);
		svc_cas_SiLinkContentInfoNode(pNewContentInfo, pNextContentInfo, pPrevContentInfo);
	}

	HxLOG_Info(" svc_cas_SiParseContentDes() Leaved!\n");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseParentRateDes										*/
/*  																	*/
/*  Description : tag = 0x55											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseParentRateDes(HUINT8 *p, svcCas_ParentDes_t *pDes)
{
	svcCas_ParentDes_t		*pParentDes = pDes;
	svcCas_ParentInfo_t	*pPrevParentInfo = NULL, *pNewParentInfo = NULL;
	HUINT8		ucRate;
	HUINT8		szCountryCode[4];
	HUINT16		usDesLen;

	HxLOG_Info("svc_cas_SiParseParentRateDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 4) != 0)
	{
		HxLOG_Print("[WARN] Unknown ParentRateDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < country code (3byte) */
		HxSTD_MemCopy(szCountryCode, p, 3);
		szCountryCode[3] = '\0';
		svc_cas_SiMakeCorrct639Code(szCountryCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(country code = %s)\n", szCountryCode);

		/** < rating (1byte) */
		ucRate = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(rating = 0x%x)\n", ucRate);

		/** < Descriptor 데이타 구성. */
		pPrevParentInfo = svc_cas_SiGetPrevParentInfo(pParentDes);
		pNewParentInfo = (svcCas_ParentInfo_t *)svc_cas_SiAllocDesInfo(TAG_PARENTAL_RATING, 0);
		if (pNewParentInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetParentInfo(pNewParentInfo, szCountryCode, ucRate);
		if (pPrevParentInfo == NULL)
			svc_cas_SiLinkParentInfoHead(pParentDes, pNewParentInfo);
		svc_cas_SiLinkParentInfoNode(pNewParentInfo, NULL, pPrevParentInfo);
	}

	HxLOG_Info(" svc_cas_SiParseParentRateDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseTeletextDes											*/
/*  																	*/
/*  Description : tag = 0x56											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseTeletextDes(HUINT8 *p, svcCas_TeletextDes_t *pDes)
{
	svcCas_TeletextDes_t	*pTeletextDes = pDes;
	svcCas_TeletextInfo_t	*pPrevTeletextInfo = NULL, *pNewTeletextInfo = NULL, *pNextTeletextInfo = NULL;
	HUINT8			ucType, ucMagazineNum, ucPageNum;
	HUINT8			szIso639LangCode[4];
	HUINT8			ucTemp;
	HUINT16			usDesLen;

	HxLOG_Info("svc_cas_SiParseTeletextDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 5) != 0)
	{
		HxLOG_Print("[WARN] Unknown TeletextDes Format, or no teletext information!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}
	else if (usDesLen == 0)
	{
		HxSTD_MemSet (szIso639LangCode, '\0', 4);
		ucType = 1;
		ucMagazineNum = ucPageNum = 0;

		/** < Pseudo Descriptor 데이타 구성. */
		pPrevTeletextInfo = svc_cas_SiGetPrevTeletextInfo(pTeletextDes);
		pNewTeletextInfo = (svcCas_TeletextInfo_t *) svc_cas_SiAllocDesInfo(TAG_TELETEXT, 0);
		if (pNewTeletextInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetTeletextInfo (pNewTeletextInfo, szIso639LangCode, ucType, ucMagazineNum, ucPageNum);
		if (pPrevTeletextInfo == NULL)
			svc_cas_SiLinkTeletextInfoHead (pTeletextDes, pNewTeletextInfo);
		svc_cas_SiLinkTeletextInfoNode (pNewTeletextInfo, pNextTeletextInfo, pPrevTeletextInfo);
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		svc_cas_SiMakeCorrct639Code(szIso639LangCode);
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
		pPrevTeletextInfo = svc_cas_SiGetPrevTeletextInfo(pTeletextDes);
		pNewTeletextInfo = (svcCas_TeletextInfo_t *)svc_cas_SiAllocDesInfo(TAG_TELETEXT, 0);
		if (pNewTeletextInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetTeletextInfo(pNewTeletextInfo, szIso639LangCode, ucType, ucMagazineNum, ucPageNum);
		if (pPrevTeletextInfo == NULL)
			svc_cas_SiLinkTeletextInfoHead(pTeletextDes, pNewTeletextInfo);
		svc_cas_SiLinkTeletextInfoNode(pNewTeletextInfo, pNextTeletextInfo, pPrevTeletextInfo);
	}

	HxLOG_Info(" svc_cas_SiParseTeletextDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseTelephoneDes										*/
/*  																	*/
/*  Description : tag = 0x57											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseTelephoneDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_TelephoneDes_t *pDes)
{
	svcCas_TelephoneDes_t	*pTelephoneDes = pDes;
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

	HxLOG_Info("svc_cas_SiParseTelephoneDes() Entered!");

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
	nTempCountryPrefixLen = svc_cas_SiPreStrMgr(eTextEnc, NULL, ucCountryPrefixLen, p, &pszCountryPrefix);
	p += ucCountryPrefixLen;
	usDesLen -= ucCountryPrefixLen;
	HxLOG_Print("\t\t(country frefix char = %s)\n", pszCountryPrefix);

	/** < international area code char */
	nTempInterAreaCodeLen = svc_cas_SiPreStrMgr(eTextEnc, NULL, ucInterAreaCodeLen, p, &pszInternationalAreaCode);
	p += ucInterAreaCodeLen;
	usDesLen -= ucInterAreaCodeLen;
	HxLOG_Print("\t\t(international area code char = %s)\n", pszInternationalAreaCode);

	/** < operator code char */
	nTempOperatorCodeLen = svc_cas_SiPreStrMgr(eTextEnc, NULL, ucOperatorCodeLen, p, &pszOperatorCode);
	p += ucOperatorCodeLen;
	usDesLen -= ucOperatorCodeLen;
	HxLOG_Print("\t\t(operator code char = %s)\n", pszOperatorCode);

	/** < national area code char */
	nTempNationalAreaCodeLen = svc_cas_SiPreStrMgr(eTextEnc, NULL, ucNationalAreaCodeLen, p, &pszNationalAreaCode);
	p += ucNationalAreaCodeLen;
	usDesLen -= ucNationalAreaCodeLen;
	HxLOG_Print("\t\t(national area code char = %s)\n", pszNationalAreaCode);

	/** < core number char */
	nTempCoreNumLen = svc_cas_SiPreStrMgr(eTextEnc, NULL, ucCoreNumLen, p, &pszCoreNumber);
	/** <p += ucCoreNumLen; */
	/** <usDesLen -= ucCoreNumLen; */
	HxLOG_Print("\t\t(core number char = %s)\n", pszCoreNumber);

	/** < Descriptor 데이타 구성. */
	pTelephoneDes->pszCountryPrefix = svc_cas_SiAllocMem(nTempCountryPrefixLen+1);
	if (pTelephoneDes->pszCountryPrefix == NULL)
		goto ErrBlock;
	pTelephoneDes->pszInternationalAreaCode = svc_cas_SiAllocMem(nTempInterAreaCodeLen+1);
	if (pTelephoneDes->pszInternationalAreaCode == NULL)
		goto ErrBlock;
	pTelephoneDes->pszOperatorCode = svc_cas_SiAllocMem(nTempOperatorCodeLen+1);
	if (pTelephoneDes->pszOperatorCode == NULL)
		goto ErrBlock;
	pTelephoneDes->pszNationalAreaCode = svc_cas_SiAllocMem(nTempNationalAreaCodeLen+1);
	if (pTelephoneDes->pszNationalAreaCode == NULL)
		goto ErrBlock;
	pTelephoneDes->pszCoreNumber = svc_cas_SiAllocMem(nTempCoreNumLen+1);
	if (pTelephoneDes->pszCoreNumber == NULL)
		goto ErrBlock;
	svc_cas_SiSetTelephoneDes(pTelephoneDes, ucForeignAvail, ucConnectionType, pszCountryPrefix, pszInternationalAreaCode, pszOperatorCode, pszNationalAreaCode, pszCoreNumber);
	if(pszCountryPrefix != NULL)
		OxCAS_Free ( pszCountryPrefix );
	if(pszInternationalAreaCode != NULL)
		OxCAS_Free ( pszInternationalAreaCode );
	if(pszOperatorCode != NULL)
		OxCAS_Free ( pszOperatorCode );
	if(pszNationalAreaCode != NULL)
		OxCAS_Free ( pszNationalAreaCode );
	if(pszCoreNumber != NULL)
		OxCAS_Free ( pszCoreNumber );

	HxLOG_Info(" svc_cas_SiParseTelephoneDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if ( pszCountryPrefix != NULL ) OxCAS_Free ( pszCountryPrefix );
	if ( pszInternationalAreaCode != NULL ) OxCAS_Free (  pszInternationalAreaCode );
	if ( pszOperatorCode != NULL ) OxCAS_Free ( pszOperatorCode );
	if ( pszNationalAreaCode != NULL ) OxCAS_Free (  pszNationalAreaCode );
	if ( pszCoreNumber != NULL ) OxCAS_Free ( pszCoreNumber );

	if (pTelephoneDes->pszCountryPrefix != NULL)
		svc_cas_SiFreeMem(pTelephoneDes->pszCountryPrefix);
	if (pTelephoneDes->pszInternationalAreaCode != NULL)
		svc_cas_SiFreeMem(pTelephoneDes->pszInternationalAreaCode);
	if (pTelephoneDes->pszOperatorCode != NULL)
		svc_cas_SiFreeMem(pTelephoneDes->pszOperatorCode);
	if (pTelephoneDes->pszNationalAreaCode != NULL)
		svc_cas_SiFreeMem(pTelephoneDes->pszNationalAreaCode);
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseLocalTimeOffsetDes									*/
/*  																	*/
/*  Description : tag = 0x58											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseLocalTimeOffsetDes(HUINT8 *p, svcCas_LocalTimeDes_t *pDes)
{
/** 아래 time 구하는 부분 추가후 다시 정리 하기 일단 임시로 막아 놓음. hcyoo */
	svcCas_LocalTimeDes_t	*pLocalTimeDes = pDes;
	svcCas_LocalTimeInfo_t	*pPrevLocalTimeInfo = NULL, *pNewLocalTimeInfo = NULL, *pNextLocalTimeInfo = NULL;
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
		svc_cas_SiMakeCorrct639Code(szCountryCode);
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

		pPrevLocalTimeInfo = svc_cas_SiGetPrevLocalTimeInfo(pLocalTimeDes);
		pNewLocalTimeInfo = (svcCas_LocalTimeInfo_t *)svc_cas_SiAllocDesInfo(TAG_LOCAL_TIME_OFFSET, 0);

		if (pNewLocalTimeInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetLocalTimeInfo( pNewLocalTimeInfo, szCountryCode, ucRegionId, ucLocalTimeOffPolar, pucLocalTimeOff, timeOfChange, pucNextTimeOffset);
		if (pPrevLocalTimeInfo == NULL)svc_cas_SiLinkLocalTimeInfoHead(pLocalTimeDes, pNewLocalTimeInfo);
		svc_cas_SiLinkLocalTimeInfoNode(pNewLocalTimeInfo, pNextLocalTimeInfo, pPrevLocalTimeInfo);
	}

	HxLOG_Info(" INT_SI_PARSE_LocalTimeOffsetDes() Leaved!");
	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseSubtitleDes											*/
/*  																	*/
/*  Description : tag = 0x59											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseSubtitleDes(HUINT8 *p, svcCas_SubTitleDes_t *pDes)
{
	svcCas_SubTitleDes_t	*pSubTitleDes = pDes;
	svcCas_SubTitleInfo_t	*pPrevSubTitleInfo = NULL, *pNewSubTitleInfo = NULL, *pNextSubTitleInfo = NULL;
	HUINT8			ucSubTitleType;
	HUINT16			usDesLen;
	HUINT16			wCompositionPageId, wAncillaryPageId;
	HUINT8			szIso639LangCode[4];

	HxLOG_Info("svc_cas_SiParseSubtitleDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if ((usDesLen % 8) != 0)
	{
		HxLOG_Print("[WARN] Unknown SubtitleDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		svc_cas_SiMakeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < subtitle type (1byte) */
		ucSubTitleType = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(subtitle type = 0x%x)\n", ucSubTitleType);

		/** < compsition page id (2byte) */
		wCompositionPageId = (HUINT16)get16bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t\t(compsition page id = 0x%x)\n", wCompositionPageId);

		/** < ancillary page id (2byte) */
		wAncillaryPageId = (HUINT16)get16bit(p);
		p += 2;
		usDesLen -= 2;
		HxLOG_Print("\t\t\t(ancillary page id = 0x%x)\n", wAncillaryPageId);
		HxLOG_Print("\t\t\t(lang = %s)(type = 0x%x)(comp page id = 0x%x)(acil page id = 0x%x)\n", szIso639LangCode, ucSubTitleType, wCompositionPageId, wAncillaryPageId);


		/** < Descriptor 데이타 구성. */
		pPrevSubTitleInfo = svc_cas_SiGetPrevSubTitleInfo(pSubTitleDes);
		pNewSubTitleInfo = (svcCas_SubTitleInfo_t *)svc_cas_SiAllocDesInfo(TAG_SUBTITLING, 0);
		if (pNewSubTitleInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetSubTitleInfo(pNewSubTitleInfo, szIso639LangCode, ucSubTitleType, wCompositionPageId, wAncillaryPageId);
		if (pPrevSubTitleInfo == NULL)
			svc_cas_SiLinkSubTitleInfoHead(pSubTitleDes, pNewSubTitleInfo);
		svc_cas_SiLinkSubTitleInfoNode(pNewSubTitleInfo, pNextSubTitleInfo, pPrevSubTitleInfo);
	}

	HxLOG_Info(" svc_cas_SiParseSubtitleDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseTerDeliSystemDes									*/
/*  																	*/
/*  Description : tag = 0x5a											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseTerDeliSystemDes(HUINT8 *p, svcCas_TerDeliSystemDes_t *pDes)
{
	svcCas_TerDeliSystemDes_t	*pTerDeliSystemDes = pDes;
	HUINT32				ulTemp;
	HUINT32				ulFreq;
	HUINT16				usDesLen;
	HUINT8				ucBandWidth, ucPriority, ucConstellation, ucHierInfo, ucCodeRateHPStream;
	HUINT8				ucCodeRateLPStream, ucGuardInterval, ucTransmissionMode;
	HUINT8				ucTemp, ucVal;
	HBOOL 				bOtherFreqFlag;

	HxLOG_Info("svc_cas_SiParseTerDeliSystemDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 11)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseTerDeliSystemDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < center-frequency (4byte) */
	ulFreq = get32bit(p);

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
	ulTemp = get32bit(p);
	usDesLen -=4;
	HxLOG_Print("\t\t(reserved = 0x%x)\n", ulTemp);

	if (usDesLen != 0)
		HxLOG_Print("[dessvc_cas_SiParse] Something is wrong. Length is not zero.\n");

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetTerDeliSystemDes(pTerDeliSystemDes, ulFreq, ucBandWidth, ucPriority, ucConstellation, ucHierInfo,
						ucCodeRateHPStream, ucCodeRateLPStream, ucGuardInterval, ucTransmissionMode, bOtherFreqFlag);

	HxLOG_Info(" svc_cas_SiParseTerDeliSystemDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseMultiNetNameDes										*/
/*  																	*/
/*  Description : tag = 0x5b											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseMultiNetNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_MultiNetNameDes_t *pDes)
{
	svcCas_MultiNetNameDes_t	*pMultiNetNameDes = pDes;
	svcCas_MultiNetNameInfo_t	*pPrevMultiNetNameInfo = NULL, *pNewMultiNetNameInfo = NULL, *pNextMultiNetNameInfo = NULL;
	HUINT8				ucNetNameLen;
	HUINT8				szIso639LangCode[4], *pszNetName = NULL;
	HUINT16				usDesLen;
	HUINT32				nTempNetNameLen;

	HxLOG_Info("svc_cas_SiParseMultiNetNameDes() Entered!");

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
		svc_cas_SiMakeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < network name length */
		ucNetNameLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(network name length = %d)\n", ucNetNameLen);

		/** < network name */
		nTempNetNameLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, ucNetNameLen, p, &pszNetName);
		p += ucNetNameLen;
		usDesLen -= ucNetNameLen;
		HxLOG_Print("\t\t\t(network name = %s)\n", pszNetName);

		/** < Descriptor 데이타 구성. */
		pPrevMultiNetNameInfo = svc_cas_SiGetPrevMultiNetNameInfo(pMultiNetNameDes);
		pNewMultiNetNameInfo = (svcCas_MultiNetNameInfo_t *)svc_cas_SiAllocDesInfo(TAG_MULTILINGUAL_NETWORK_NAME, 0);
		if (pNewMultiNetNameInfo == NULL)
			goto ErrBlock;
		pNewMultiNetNameInfo->pszNetName = svc_cas_SiAllocMem(nTempNetNameLen+1);
		if (pNewMultiNetNameInfo->pszNetName == NULL)
			goto ErrBlock;
		svc_cas_SiSetMultiNetNameInfo(pNewMultiNetNameInfo, szIso639LangCode, pszNetName);
		if(pszNetName != NULL)
			OxCAS_Free ( pszNetName );
		if (pPrevMultiNetNameInfo == NULL)
			svc_cas_SiLinkMultiNetNameInfoHead(pMultiNetNameDes, pNewMultiNetNameInfo);
		svc_cas_SiLinkMultiNetNameInfoNode(pNewMultiNetNameInfo, pNextMultiNetNameInfo, pPrevMultiNetNameInfo);
	}

	HxLOG_Info(" svc_cas_SiParseMultiNetNameDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if ( pszNetName != NULL ) OxCAS_Free ( pszNetName );
	if (pNewMultiNetNameInfo != NULL)
		svc_cas_SiFreeDesInfo(TAG_MULTILINGUAL_NETWORK_NAME, 0, (HUINT8 *)pNewMultiNetNameInfo);
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseMultiBouqNameDes									*/
/*  																	*/
/*  Description : tag = 0x5c											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseMultiBouqNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_MultiBouqNameDes_t *pDes)
{
	svcCas_MultiBouqNameDes_t		*pMultiBouqNameDes = pDes;
	svcCas_MultiBouqNameInfo_t		*pPrevMultiBouqNameInfo = NULL, *pNewMultiBouqNameInfo = NULL, *pNextMultiBouqNameInfo = NULL;
	HUINT8					ucBouqNameLen;
	HUINT8					szIso639LangCode[4], *pszBouqName = NULL;
	HUINT16					usDesLen;
	HUINT32					nTempBouqNameLen;

	HxLOG_Info("svc_cas_SiParseMultiBouqNameDes() Entered!");

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
		svc_cas_SiMakeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < bouquet name length */
		ucBouqNameLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(bouquet name length = %d)\n", ucBouqNameLen);

		/** < bouquet name */
		nTempBouqNameLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, ucBouqNameLen, p, &pszBouqName);
		p += ucBouqNameLen;
		usDesLen -= ucBouqNameLen;
		HxLOG_Print("\t\t\t(Bouqwork name = %s)\n", pszBouqName);

		/** < Descriptor 데이타 구성. */
		pPrevMultiBouqNameInfo = svc_cas_SiGetPrevMultiBouqNameInfo(pMultiBouqNameDes);
		pNewMultiBouqNameInfo = (svcCas_MultiBouqNameInfo_t *)svc_cas_SiAllocDesInfo(TAG_MULTILINGUAL_BOUQUET_NAME, 0);
		if (pNewMultiBouqNameInfo == NULL)
			goto ErrBlock;
		pNewMultiBouqNameInfo->pszBouqName = svc_cas_SiAllocMem(nTempBouqNameLen+1);
		if (pNewMultiBouqNameInfo->pszBouqName == NULL)
			goto ErrBlock;
		svc_cas_SiSetMultiBouqNameInfo(pNewMultiBouqNameInfo, szIso639LangCode, pszBouqName);
		if(pszBouqName != NULL)
			OxCAS_Free ( pszBouqName );
		if (pPrevMultiBouqNameInfo == NULL)
			svc_cas_SiLinkMultiBouqNameInfoHead(pMultiBouqNameDes, pNewMultiBouqNameInfo);
		svc_cas_SiLinkMultiBouqNameInfoNode(pNewMultiBouqNameInfo, pNextMultiBouqNameInfo, pPrevMultiBouqNameInfo);
	}

	HxLOG_Info(" svc_cas_SiParseMultiBouqNameDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if ( pszBouqName != NULL ) OxCAS_Free ( pszBouqName );
	if (pNewMultiBouqNameInfo != NULL)
		svc_cas_SiFreeDesInfo(TAG_MULTILINGUAL_BOUQUET_NAME, 0, (HUINT8 *)pNewMultiBouqNameInfo);
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseMultiServNameDes									*/
/*  																	*/
/*  Description : tag = 0x5d											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseMultiServNameDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_MultiSvcNameDes_t *pDes)
{
	svcCas_MultiSvcNameDes_t		*pMultiServNameDes = pDes;
	svcCas_MultiSvcNameInfo_t		*pPrevMultiServNameInfo = NULL, *pNewMultiServNameInfo = NULL, *pNextMultiServNameInfo = NULL;
	HUINT8					ucServProviderNameLen, ucServNameLen;
	HUINT8					szIso639LangCode[4], *pszServProviderName = NULL, *pszServName = NULL;
	HUINT16					usDesLen;
	HUINT32					nTempServProviderNameLen, nTempServNameLen;

	HxLOG_Info("svc_cas_SiParseMultiServNameDes() Entered!");

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
		svc_cas_SiMakeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < service provider name length */
		ucServProviderNameLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(service provider name length = %d)\n", ucServProviderNameLen);

		/** < service provider name */
		pszServProviderName = NULL;
		nTempServProviderNameLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, ucServProviderNameLen, p, &pszServProviderName);
		p += ucServProviderNameLen;
		usDesLen -= ucServProviderNameLen;
		HxLOG_Print("\t\t\t(service provider name = %s)\n", pszServProviderName ? (char *)pszServProviderName : "Null");
		if (pszServProviderName == NULL)
			goto ErrBlock;

		/** < service name length */
		ucServNameLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(service name length = %d)\n", ucServNameLen);

		/** < service name */
		pszServName = NULL;
		nTempServNameLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, ucServNameLen, p, &pszServName);
		p += ucServNameLen;
		usDesLen -= ucServNameLen;
		HxLOG_Print("\t\t\t(service name = %s)\n", pszServName ? (char *)pszServName : "Null");
		if (pszServName == NULL)
			goto ErrBlock;

		/** < Descriptor 데이타 구성. */
		pPrevMultiServNameInfo = svc_cas_SiGetPrevMultiServNameInfo(pMultiServNameDes);
		pNewMultiServNameInfo = (svcCas_MultiSvcNameInfo_t *)svc_cas_SiAllocDesInfo(TAG_MULTILINGUAL_SERVICE_NAME, 0);
		if (pNewMultiServNameInfo == NULL)
			goto ErrBlock;
		pNewMultiServNameInfo->pszServProviderName = svc_cas_SiAllocMem(nTempServProviderNameLen+1);
		if (pNewMultiServNameInfo->pszServProviderName == NULL)
			goto ErrBlock;
		pNewMultiServNameInfo->pszServName = svc_cas_SiAllocMem(nTempServNameLen+1);
		if (pNewMultiServNameInfo->pszServName == NULL)
			goto ErrBlock;
		svc_cas_SiSetMultiServNameInfo(pNewMultiServNameInfo, szIso639LangCode, pszServProviderName, pszServName);
		if(pszServProviderName != NULL)
			OxCAS_Free ( pszServProviderName );
		if(pszServName != NULL)
			OxCAS_Free ( pszServName );
		if (pPrevMultiServNameInfo == NULL)
			svc_cas_SiLinkMultiServNameInfoHead(pMultiServNameDes, pNewMultiServNameInfo);
		svc_cas_SiLinkMultiServNameInfoNode(pNewMultiServNameInfo, pNextMultiServNameInfo, pPrevMultiServNameInfo);
	}

	HxLOG_Info(" svc_cas_SiParseMultiServNameDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if ( pszServProviderName != NULL ) OxCAS_Free ( pszServProviderName );
	if ( pszServName != NULL ) OxCAS_Free ( pszServName);

	if (pNewMultiServNameInfo != NULL)
	{
		if (pNewMultiServNameInfo->pszServProviderName != NULL)
		{
			svc_cas_SiFreeMem(pNewMultiServNameInfo->pszServProviderName);
		}

		svc_cas_SiFreeDesInfo(TAG_MULTILINGUAL_SERVICE_NAME, 0, (HUINT8 *)pNewMultiServNameInfo);
	}

	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseMultiCompDes										*/
/*  																	*/
/*  Description : tag = 0x5e											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseMultiCompDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_MultiCompDes_t *pDes)
{
	svcCas_MultiCompDes_t	*pMultiCompDes = pDes;
	svcCas_MultiCompInfo_t	*pPrevMultiCompInfo = NULL, *pNewMultiCompInfo = NULL, *pNextMultiCompInfo = NULL;
	HUINT8			ucCompTag;
	HUINT8			szIso639LangCode[4], *pszText = NULL;
	HUINT16			usDesLen, usTextLen;
	HUINT32			nTempTextLen;
	HUINT32			nAssignedTextLen;

	HxLOG_Info("svc_cas_SiParseMultiCompDes() Entered!");

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
	svc_cas_SiSetMultiCompDes(pMultiCompDes, ucCompTag);

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < ISO 639 language code (3byte) */
		HxSTD_MemCopy(szIso639LangCode, p, 3);
		szIso639LangCode[3] = '\0';
		svc_cas_SiMakeCorrct639Code(szIso639LangCode);
		p += 3;
		usDesLen -= 3;
		HxLOG_Print("\t\t\t(ISO 639 language code = %s)\n", szIso639LangCode);

		/** < text length */
		usTextLen = *p++;
		usDesLen--;
		HxLOG_Print("\t\t\t(text length = %d)\n", usTextLen);

		/** < text */
		nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);
		nTempTextLen = svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, (HUINT8)nAssignedTextLen, p, &pszText);
		p += usTextLen;
		usDesLen -= usTextLen;
		HxLOG_Print("\t\t\t(text = %s)\n", pszText);

		/** < Descriptor 데이타 구성. */
		pPrevMultiCompInfo = svc_cas_SiGetPrevMultiCompInfo(pMultiCompDes);
		pNewMultiCompInfo = (svcCas_MultiCompInfo_t *)svc_cas_SiAllocDesInfo(TAG_MULTILINGUAL_COMPONENT, 0);
		if (pNewMultiCompInfo == NULL)
			goto ErrBlock;
		pNewMultiCompInfo->pszTextDes = svc_cas_SiAllocMem(nTempTextLen+1);
		if (pNewMultiCompInfo->pszTextDes == NULL)
			goto ErrBlock;
		svc_cas_SiSetMultiCompInfo(pNewMultiCompInfo, szIso639LangCode, pszText);
		if(pszText != NULL)
			OxCAS_Free ( pszText );
		if (pPrevMultiCompInfo == NULL)
			svc_cas_SiLinkMultiCompInfoHead(pMultiCompDes, pNewMultiCompInfo);
		svc_cas_SiLinkMultiCompInfoNode(pNewMultiCompInfo, pNextMultiCompInfo, pPrevMultiCompInfo);
	}

	HxLOG_Info(" svc_cas_SiParseMultiCompDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	if ( pszText != NULL ) OxCAS_Free ( pszText );
	if (pNewMultiCompInfo != NULL)
		svc_cas_SiFreeDesInfo(TAG_MULTILINGUAL_COMPONENT, 0, (HUINT8 *)pNewMultiCompInfo);
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParsePrivDataSpecDes										*/
/*  																	*/
/*  Description : tag = 0x5f											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParsePrivDataSpecDes (HUINT8 *p, svcCas_PrivateDataSpecDes_t *pDes)
{
	svcCas_PrivateDataSpecDes_t	*pPrivateDataSpecDes = pDes;
	HUINT16					usDesLen;
	HUINT32					dwPrivateDataSpec;

	HxLOG_Info("svc_cas_SiParsePrivateDataSpecDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 4)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParsePrivateDataSpecDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	dwPrivateDataSpec = get32bit (p);
	/** <p += 4; */
	HxLOG_Print("\t\t(private data specifier = 0x%x)\n", dwPrivateDataSpec);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetPrivateDataSpecDes (pPrivateDataSpecDes, dwPrivateDataSpec);

	HxLOG_Info("svc_cas_SiParsePrivateDataSpecDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseServMoveDes											*/
/*  																	*/
/*  Description : tag = 0x60											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseServMoveDes(HUINT8 *p, svcCas_SvcMoveDes_t *pDes)
{
	svcCas_SvcMoveDes_t	*pServMoveDes = pDes;
	HUINT16			usDesLen;
	HUINT16			usNewOrgNetId, usNewTsId, usNewSvcId;

	HxLOG_Info("svc_cas_SiParseServMoveDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen != 6)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseServMoveDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < new original network id (2byte) */
	usNewOrgNetId = (HUINT16)get16bit(p);
	p += 2;
	HxLOG_Print("\t\t(new original network id = 0x%x)\n", usNewOrgNetId);

	/** < new transport stream id (2byte) */
	usNewTsId = (HUINT16)get16bit(p);
	p += 2;
	HxLOG_Print("\t\t(new transport stream id = 0x%x)\n", usNewTsId);

	/** < new service id (2byte) */
	usNewSvcId = (HUINT16)get16bit(p);
	HxLOG_Print("\t\t(new service id = 0x%x)\n", usNewSvcId);

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetServMoveDes(pServMoveDes, usNewOrgNetId, usNewTsId, usNewSvcId);

	HxLOG_Info(" svc_cas_SiParseServMoveDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseShortSmoothBufDes									*/
/*  																	*/
/*  Description : tag = 0x61											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseShortSmoothBufDes(HUINT8 *p, svcCas_ShortSmoothBufDes_t *pDes)
{
	svcCas_ShortSmoothBufDes_t	*pShortSmoothBufDes = pDes;
	svcCas_PrivateDataInfo_t		*pNewPrivateDataInfo = NULL;
	HUINT8					ucSbSize, ucSbLeakRate;
	HUINT8					ucTemp;
	HUINT16					usDesLen;

	HxLOG_Info("svc_cas_SiParseShortSmoothBufDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseShortSmoothBufDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
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
#ifdef ALLOW_PRIVATE_DATA_ACQUIRE
		pNewPrivateDataInfo = (svcCas_PrivateDataInfo_t *)svc_cas_SiAllocDesInfo(TAG_PRIVATE_DATA, 0);
		if (pNewPrivateDataInfo == NULL)
			goto ErrBlock;
		svc_cas_SiSetPrivateDataInfo(pNewPrivateDataInfo, p, usDesLen);
#endif
	}

	/** < Descriptor 데이타 구성. */
	svc_cas_SiSetShortSmoothBufDes (pShortSmoothBufDes, ucSbSize, ucSbLeakRate, pNewPrivateDataInfo);

	HxLOG_Info("svc_cas_SiParseShortSmoothBufDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

#ifdef ALLOW_PRIVATE_DATA_ACQUIRE
ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
#endif
}


/************************************************************************/
/*	function : svc_cas_SiParseFrequncyListDes										*/
/*  																	*/
/*  Description : tag = 0x62											*/
/************************************************************************/
STATIC	HERROR svc_cas_SiParseFrequncyListDes(HUINT8 *p, svcCas_FrequencyListDes_t *pDes)
{
	svcCas_FrequencyListDes_t	*pFreqList = pDes;
	HUINT8				ucFreqNum;
	HUINT16				usDesLen;
	HUINT32				i;

	HxLOG_Info("svc_cas_SiParseFrequncyListDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 5)	/** < coding_type(1) + centre_frequency(4) */
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseFrequncyListDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	pFreqList->codingType = (*p & 0x03);

	p ++; usDesLen --;

	ucFreqNum = usDesLen / 4;

	if (ucFreqNum == 0)	/** < if (usDesLen < 5)로 걸리지는 않음. */
		return ERR_CAS_SI_RESULT_OK;

	pFreqList->numFreq = ucFreqNum;
	pFreqList->centreFreq = (HUINT32 *)OxCAS_Malloc(ucFreqNum * 4);
	if (pFreqList->centreFreq == NULL)
	{
		HxLOG_Print("[ERR] svc_cas_SiParseFrequncyListDes Allocation Error!\n");
		return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
	}

	for (i = 0; i < ucFreqNum; i++)
	{
		pFreqList->centreFreq[i] = get32bit(p);
		HxLOG_Print("\t\t\t(centre frequency of FLD[%d] = %d)\n", i, pFreqList->centreFreq[i]);

		p += 4;
	}

	HxLOG_Info("svc_cas_SiParseFrequncyListDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParsePartialTransportStreamDes							*/
/*  																	*/
/*  Description : tag = 0x63											*/
/************************************************************************/
STATIC	HERROR svc_cas_SiParsePartialTransportStreamDes(HUINT8 *p, svcCas_PartialTsDes *pDes)
{
	HUINT8			 ucDesTag, ucDesLen;
	svcCas_PartialTsDes	*pstDes;

	HxLOG_Info("svc_cas_SiParsePartialTransportStreamDes() Entered!");

	pstDes = (svcCas_PartialTsDes*)pDes;

	ucDesTag = p[0];
	ucDesLen = p[1];

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", ucDesTag);

	/** < descriptor length (1byte) */
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	pstDes->ucDesTag = ucDesTag;
	pstDes->ulPeakRate = ((p[2] & 0x3F) <<16) | ((p[3] & 0xFF) << 8) | (p[4] & 0xFF);
	pstDes->ulMinimumOverallSmoothingRate = ((p[5] & 0x3F) <<16) | ((p[6] & 0xFF) << 8) | (p[7] & 0xFF);
	pstDes->ulMaximumOverallSmoothingBuffer = ((p[8] & 0x3F) << 8) | (p[9] & 0xFF);

	HxLOG_Info("svc_cas_SiParsePartialTransportStreamDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseDataBroadcastDes									*/
/*  																	*/
/*  Description : tag = 0x64											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseDataBroadcastDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, svcCas_DataBroadcastDes_t *pDes)
{
	svcCas_DataBroadcastDes_t		*pDataBroadcastDes = pDes;
	HUINT8					*pszInfo = NULL;
	HUINT16					usDataBroadcastId;
	HUINT16					usDesLen, usSelLen, usTextLen;
	HUINT8					ucCompTag;
	HUINT8					szIso639LangCode[4];
	HUINT8					*pszText = NULL;
	HUINT8					*pszObjName = NULL;
	HUINT32					nAssignedTextLen;
	HUINT8					ucCarouselId;
	HUINT32   				ulTransactionId;
	HUINT32					ulTimeOutValueDSI;
	HUINT32					ulTimeOutValueDII;
	HUINT32					ulLeakRate;

	HxLOG_Info("svc_cas_SiParseDataBroadcastDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseDataBroadcastDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < data broadcast id */
	usDataBroadcastId = (HUINT16)get16bit(p);
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
				pszInfo = (HUINT8 *)svc_cas_SiAllocDesInfo(TAG_DATA_BROADCAST, usDataBroadcastId);
				if (pszInfo == NULL)
				{
					HxLOG_Print("[WARN] svc_cas_SiParseDataBroadcastDes Alloc error\n");
					HxLOG_Info(" svc_cas_SiParseDataBroadcastDes () Leaved!");
					return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
				}
				ucCarouselId = (HUINT8)((*p) >> 6);
				p++;
				ulTransactionId = (HUINT32)get32bit(p);
				p += 4;
				ulTimeOutValueDSI = (HUINT32)get32bit(p);
				p += 4;
				ulTimeOutValueDII = (HUINT32)get32bit(p);
				p += 4;
				ulLeakRate = (HUINT32)( (get32bit(p) >> 8) & 0x003fffff );
				p += 3;
			 	svc_cas_SiSetDataBroadcastInfo((svcCas_DataCarouselInfo_t *)pszInfo, ucCarouselId
			 		, ulTransactionId, ulTimeOutValueDSI, ulTimeOutValueDII
			 		, ulLeakRate);
				usDesLen -= 16;
				break;
			case 0x0007:	/** <DVB object carousel */
				pszInfo = (HUINT8 *)svc_cas_SiAllocDesInfo(TAG_DATA_BROADCAST, usDataBroadcastId);
				if (pszInfo == NULL)
				{
					HxLOG_Print("[WARN] svc_cas_SiParseDataBroadcastDes Alloc error\n");
					HxLOG_Info(" svc_cas_SiParseDataBroadcastDes () Leaved!");
					return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
				}
				ucCarouselId = (HUINT8)((*p) >> 6);
				p++;
				ulTransactionId = (HUINT32)get32bit(p);
				p += 4;
				ulTimeOutValueDSI = (HUINT32)get32bit(p);
				p += 4;
				ulTimeOutValueDII = (HUINT32)get32bit(p);
				p += 4;
				ulLeakRate = (HUINT32)( (get32bit(p) >> 8) & 0x003fffff );
				p += 3;
				usDesLen -= 16;

				HxSTD_MemCopy(szIso639LangCode, p, 3);
				szIso639LangCode[3] = '\0';
				svc_cas_SiMakeCorrct639Code(szIso639LangCode);
				p += 3;
				usDesLen -= 3;

				/** < Objec Name length */
				usTextLen = *p++;
				usDesLen--;

				/** < Max allocation limit & allocation */
				nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);

				/** < Object Name */
				(void)svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, (HUINT8)nAssignedTextLen, p, &pszObjName);
				p += usTextLen;
				usDesLen -= usTextLen;
				HxLOG_Print("\t\t(Object Name = %s)\n", pszObjName);

			 	svc_cas_SiSetObjectCarouselInfo((svcCas_ObjectCarouselInfo_t *)pszInfo, ucCarouselId
			 		, ulTransactionId, ulTimeOutValueDSI, ulTimeOutValueDII
			 		, ulLeakRate, szIso639LangCode, pszObjName);

				if (pszObjName != NULL)
				{
					OxCAS_Free (pszObjName);
				}

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
	svc_cas_SiMakeCorrct639Code(szIso639LangCode);
	p += 3;
	usDesLen -= 3;

	/** < text length */
	usTextLen = *p++;
	usDesLen--;

	/** < Max allocation limit & allocation */
	nAssignedTextLen = (usTextLen > MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : usTextLen);

	/** <text */
	(void)svc_cas_SiPreStrMgr(eTextEnc, szIso639LangCode, (HUINT8)nAssignedTextLen, p, &pszText);
	//p += usTextLen;
	//usDesLen -= usTextLen;
	HxLOG_Print("\t\t(text = %s)\n", pszText);

 	svc_cas_SiSetDataBroadcastDes(pDataBroadcastDes, usDataBroadcastId, ucCompTag
 		, (HUINT8 *)pszInfo, szIso639LangCode, pszText);

	HxLOG_Info("svc_cas_SiParseDataBroadcastDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

}

STATIC	HERROR	svc_cas_SiParseDataBroadcastIdDes(HUINT8 *p, svcCas_DataBroadcastIdDes_t *pDes)
{
	HUINT8 *pucIdSelector = NULL;
	HUINT16 usDesLen, usSelectorLen, usDataBroadcastId;

	HxLOG_Info("svc_cas_SiParseDataBroadcastDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseDataBroadcastIdDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	/** < data broadcast id */
	usDataBroadcastId = (HUINT16)get16bit(p);
	p += 2;
	usDesLen -= 2;

	/** < selector length (usDesLen) */
	usSelectorLen = usDesLen;
	if (usSelectorLen > 0)
	{
		pucIdSelector = OxCAS_Malloc (usSelectorLen);
		HxSTD_MemCopy (pucIdSelector, p, usSelectorLen);
	}

	/** < set descriptor */
	pDes->usDataBroadcastId = usDataBroadcastId;
	pDes->usSelectorLen = usSelectorLen;
	pDes->pszIdSelector = pucIdSelector;

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParsePDCDes											*/
/*  																	*/
/*  Description : tag = 0x69											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParsePDCDes (HUINT8 *p, svcCas_PDCDes_t *pDes)
{
	HUINT16				usDesLen;
	HUINT32				ulPIL = 0;

	HxLOG_Info("svc_cas_SiParsePDCDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	ulPIL = get24bit (p);
	p += 3;

	pDes->ucDay	= (HUINT8)((ulPIL >> 15) & 0x001F);
	pDes->ucMonth	= (HUINT8)((ulPIL >> 11) & 0x000F);
	pDes->ucHour	= (HUINT8)((ulPIL >> 6) & 0x001F);
	pDes->ucMinute	= (HUINT8)(ulPIL & 0x003F);

	HxLOG_Info(" svc_cas_SiParsePDCDes () Leaved!");

	return ERR_CAS_SI_RESULT_OK;

}

/************************************************************************/
/*	function : svc_cas_SiParseDolbyAC3Des											*/
/*  																	*/
/*  Description : tag = 0x6a											*/
/************************************************************************/
STATIC	HERROR	svc_cas_SiParseDolbyAC3Des (HUINT8 *p, svcCas_DolbyAc3Des_t *pDes)
{
	HUINT8				ucTmpValue;
	HUINT16				usDesLen;

	HxLOG_Info("svc_cas_SiParseDolbyAC3Des () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	ucTmpValue = *p++; usDesLen--;
	pDes->bComponentTypeFlag = (HBOOL)((ucTmpValue >> 7) & 1);
	pDes->bBsidFlag = (HBOOL)((ucTmpValue >> 6) & 1);
	pDes->bMainIdFlag = (HBOOL)((ucTmpValue >> 5) & 1);
	pDes->bAsvcFlag = (HBOOL)((ucTmpValue >> 4) & 1);

	if (1 == pDes->bComponentTypeFlag)
	{
		pDes->ucComponentType = *p++; usDesLen--;
		HxLOG_Print("\t\t\t(component type = 0x%X)\n", pDes->ucComponentType);
	}
	if (1 == pDes->bBsidFlag)
	{
		pDes->ucBsId = *p++; usDesLen--;
		HxLOG_Print("\t\t\t(bsid = 0x%X)\n", pDes->ucBsId);
	}
	if (1 == pDes->bMainIdFlag)
	{
		pDes->ucMainId = *p++; usDesLen--;
		HxLOG_Print("\t\t\t(mainid = 0x%X)\n", pDes->ucMainId);
	}
	if (1 == pDes->bAsvcFlag)
	{
		pDes->ucAsvc = *p++; usDesLen--;
		HxLOG_Print("\t\t\t(asvc = 0x%X)\n", pDes->ucAsvc);
	}

	HxLOG_Info(" svc_cas_SiParseDolbyAC3Des () Leaved!");

	return ERR_CAS_SI_RESULT_OK;

}

/************************************************************************/
/*	function : svc_cas_SiParseDefaultAuthority											*/
/*  																	*/
/*  Description : tag = 0x73											*/
/************************************************************************/
STATIC	HERROR	svc_cas_SiParseDefaultAuthorityDes(HUINT8 *p, svcCas_DefaultAuthorityDes_t *pDes)
{
	HUINT16				usDesLen, uriLen;

	HxLOG_Info("svc_cas_SiParseDefaultAuthority () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	uriLen = uriStrlen(usDesLen, p);
	pDes->pszDefaultAuthority = OxCAS_Malloc(uriLen + 1);
	if (uriStrcpy(pDes->pszDefaultAuthority, p, usDesLen) != uriLen)
		HxLOG_Print("\t\t\tSomething is wrong!!! - check function (URI)\n");
	pDes->pszDefaultAuthority[uriLen]= 0;
	HxLOG_Print("\t\t\t(default Authority = %s)\n", pDes->pszDefaultAuthority);

	HxLOG_Info(" svc_cas_SiParseDefaultAuthority () Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParseContentIdentifierDes											*/
/*  																	*/
/*  Description : tag = 0x76											*/
/************************************************************************/
STATIC	HERROR	svc_cas_SiParseContentIdentifierDes (HUINT8 *p, svcCas_ContentIdentifierDes_t *pDes)
{
	svcCas_CridInfo_t	*pNewInfo = NULL, *pPrevInfo = NULL;
	HUINT8		ucTmpByte;
	HUINT16		usDesLen, uriLen;

	HxLOG_Info("svc_cas_SiParseContentIdentifierDes () Entered!");
	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		pNewInfo = (svcCas_CridInfo_t*)svc_cas_SiAllocDesInfo(TAG_CONTENT_IDENTIFIER, 0);
		if (pNewInfo == NULL)
		{
			HxLOG_Print("[WARN] svc_cas_SiParseContentIdentifierInfo Alloc error\n");
			HxLOG_Info("svc_cas_SiParseContentIdentifierDes () Leaved!");
			return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
		}

		ucTmpByte = *p++;		usDesLen--;
		pNewInfo->ucCridType = (ucTmpByte >> 2) & 0x3F;
		pNewInfo->ucCridLocation = (ucTmpByte & 0x03);

		HxLOG_Print("\t\t\tCridType = 0x%x)\n", pNewInfo->ucCridType);
		HxLOG_Print("\t\t\tCridLocation = 0x%x)\n", pNewInfo->ucCridLocation);

		if (pNewInfo->ucCridLocation == 0)
		{
			//	supported format. if you delete below conditional check line, you will encounter system hangup.
			//	some of streams are sent with abnormal TV anytime specification
			//	0x30 ~ 0x3f
			if ((pNewInfo->ucCridType < 0x30) || (pNewInfo->ucCridType > 0x3F))
			{
				//	not support crid type.
				HxLOG_Print("\t\t\tnot supported\n");
				svc_cas_SiFreeDesInfo(TAG_CONTENT_IDENTIFIER, 0, (HUINT8 *)pNewInfo);
				break;
			}

			ucTmpByte = *p++;	usDesLen--;

			uriLen = uriStrlen(ucTmpByte, p);
			pNewInfo->pszCridBytes = OxCAS_Malloc(uriLen + 1);
			if (uriStrcpy(pNewInfo->pszCridBytes, p, ucTmpByte) != uriLen)
				HxLOG_Print("\t\t\tSomething is Wrong!! - check function uri\n");
			pNewInfo->pszCridBytes[uriLen] = 0;
			p += ucTmpByte;		usDesLen -= ucTmpByte;
			HxLOG_Print("\t\t\tcridBytes = %s\n", pNewInfo->pszCridBytes);
		} else
		if (pNewInfo->ucCridLocation == 1)		//	CIT reference, not supported yet
		{
			pNewInfo->usCridRef = get16bit(p);
			p += 2;		usDesLen -= 2;
			HxLOG_Print("\t\t\tcridRef = 0x%x\n", pNewInfo->usCridRef);
		} else
		{
			//	not support location.
			HxLOG_Print("\t\t\tnot supported\n");
			svc_cas_SiFreeDesInfo(TAG_CONTENT_IDENTIFIER, 0, (HUINT8 *)pNewInfo);
			continue;
		}

		if (pPrevInfo == NULL)
		{
			pDes->pInfo = pNewInfo;
			pPrevInfo = pNewInfo;
		} else
		{
			pPrevInfo->pNext = pNewInfo;
			pPrevInfo = pPrevInfo->pNext;
		}
	}

	HxLOG_Info("svc_cas_SiParseContentIdentifierDes () Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

/************************************************************************/
/*	function : svc_cas_SiParseS2SatDeliSystemDes									*/
/*  																	*/
/*  Description : tag = 0x79											*/
/************************************************************************/
STATIC HERROR svc_cas_SiParseS2SatDeliSystemDes (HUINT8 *p, svcCas_S2SatDeliSystemDes_t *pDes)
{
	svcCas_S2SatDeliSystemDes_t		*pS2SatDeliSystemDes = pDes;
	HUINT16		usDesLen;
	HUINT8		ucTemp;

	HxLOG_Info("svc_cas_SiParseS2SatDeliSystemDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseS2SatDeliSystemDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	ucTemp = *p++;

	/** < scrambling sequence selector */
	pS2SatDeliSystemDes->ucScrambleSeqSelect = (HUINT8)((ucTemp & 0x80) >> 7);

	/** < multiple input stream flag */
	pS2SatDeliSystemDes->ucMultipleInStreamFlag = (HUINT8)((ucTemp & 0x40) >> 6);

	/** < backwards ocmpatibility indicator */
	pS2SatDeliSystemDes->ucBackCompatibilityIndicator = (HUINT8)((ucTemp & 0x20) >> 5);

	/** < reserved for use */
	pS2SatDeliSystemDes->ucReserved1 = (HUINT8)(ucTemp & 0x1f);

	ucTemp = *p++;

	if( pS2SatDeliSystemDes->ucScrambleSeqSelect == 1)
	{
		pS2SatDeliSystemDes->ucReserved2 = (HUINT8)((ucTemp & 0xfb)>>2);

		/** < scrambling sequence index */
		pS2SatDeliSystemDes->ulScramblingSeqIndex
			= (HUINT32)( ((ucTemp & 0x03 ) <<16) |(*p<<8) |*(p+1) );

//		*p++;
		p++;
	}
	if( pS2SatDeliSystemDes->ucMultipleInStreamFlag == 1)
	{
		/** < input stream identifier */
		pS2SatDeliSystemDes->ucInputStreamIdentifier = *p++;
	}

	HxLOG_Info(" svc_cas_SiParseS2SatDeliSystemDes () Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}


/************************************************************************/
/*	function : svc_cas_SiParseEnhancedAC3Des											*/
/*  																	*/
/*  Description : tag = 0x7a											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseEnhancedAC3Des (HUINT8 *p, svcCas_EnhancedAC3Des_t *pDes)
{
	svcCas_EnhancedAC3Des_t	*pEnhancedAC3Des = pDes;
	HUINT16				usDesLen;
	HUINT8				ucTemp;

	HxLOG_Info("svc_cas_SiParseEnhancedAC3Des () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseEnhancedAC3Des Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	ucTemp = *p++;
	pEnhancedAC3Des->bComponentTypeFlag 	= (HBOOL)((ucTemp & 0x80)>>7);
	pEnhancedAC3Des->bBsidFlag				= (HBOOL)((ucTemp & 0x40)>>6);
	pEnhancedAC3Des->bMainIdFlag			= (HBOOL)((ucTemp & 0x20)>>5);
	pEnhancedAC3Des->bAsvcFlag				= (HBOOL)((ucTemp & 0x10)>>4);
	pEnhancedAC3Des->bMixinfoexists 		= (HBOOL)((ucTemp & 0x08)>>3);
	pEnhancedAC3Des->bSubStream1Flag 		= (HBOOL)((ucTemp & 0x04)>>2);
	pEnhancedAC3Des->bSubStream2Flag 		= (HBOOL)((ucTemp & 0x02)>>1);
	pEnhancedAC3Des->bSubStream3Flag 		= (HBOOL)(ucTemp & 0x01);
	HxLOG_Print("\t\t(bComponentTypeFlag = %d)\n", pEnhancedAC3Des->bComponentTypeFlag);
	HxLOG_Print("\t\t(bBsidFlag = %d)\n", pEnhancedAC3Des->bBsidFlag);
	HxLOG_Print("\t\t(bMainIdFlag = %d)\n", pEnhancedAC3Des->bMainIdFlag);
	HxLOG_Print("\t\t(bAsvcFlag = %d)\n", pEnhancedAC3Des->bAsvcFlag);
	HxLOG_Print("\t\t(bMixinfoexists = %d)\n", pEnhancedAC3Des->bMixinfoexists);
	HxLOG_Print("\t\t(bSubStreamFlag1 = %d)\n", pEnhancedAC3Des->bSubStream1Flag);
	HxLOG_Print("\t\t(bSubStreamFlag2 = %d)\n", pEnhancedAC3Des->bSubStream2Flag);
	HxLOG_Print("\t\t(bSubStreamFlag3 = %d)\n", pEnhancedAC3Des->bSubStream3Flag);

	if( pEnhancedAC3Des->bComponentTypeFlag == 1)
		pEnhancedAC3Des->ucComponentType = *p++;
	if( pEnhancedAC3Des->bBsidFlag == 1)
		pEnhancedAC3Des->ucBsId = *p++;
	if( pEnhancedAC3Des->bMainIdFlag == 1)
		pEnhancedAC3Des->ucMainId = *p++;
	if( pEnhancedAC3Des->bAsvcFlag == 1)
		pEnhancedAC3Des->ucAsvc = *p++;
	if( pEnhancedAC3Des->bSubStream1Flag == 1)
		pEnhancedAC3Des->ucSubStream1 = *p++;
	if( pEnhancedAC3Des->bSubStream2Flag == 1)
		pEnhancedAC3Des->ucSubStream2 = *p++;
	if( pEnhancedAC3Des->bSubStream3Flag == 1)
		pEnhancedAC3Des->ucSubStream3 = *p++;

	HxLOG_Info(" svc_cas_SiParseEnhancedAC3Des () Leaved!");

	return ERR_CAS_SI_RESULT_OK;

}


/************************************************************************/
/*	function : svc_cas_SiParseAACDes											*/
/*  																	*/
/*  Description : tag = 0x7C											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseAACDes (HUINT8 *p, svcCas_AACDes_t *pDes)
{
	HUINT8				ucTemp;
	HUINT16				usDesLen;
	svcCas_AACDes_t			*pAACDes_t = pDes;


	HxLOG_Info("svc_cas_SiParseAACDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	pAACDes_t->ucDesTag	= *p;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 2)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseAACDes Format!\n");
		return ERR_CAS_SI_INVALID_DESCRIPTOR;
	}

	ucTemp 							= *p++;
	pAACDes_t->ucProfileAndLevel	= ucTemp;
	ucTemp 							= *p++;
	pAACDes_t->bAACTypeFlag			= (HBOOL)((ucTemp & 0x80)>>7);

	HxLOG_Print("\t\t(ucProfileAndLevel = %d)\n", pAACDes_t->ucProfileAndLevel);
	HxLOG_Print("\t\t(bAacTypeAvilable = %d)\n", pAACDes_t->bAACTypeFlag);

	if (pAACDes_t->bAACTypeFlag && usDesLen >= 3)
	{
		ucTemp 					= *p++;
		pAACDes_t->ucAACType	= ucTemp;
		HxLOG_Print("\t\t(ucAacType = %d)\n", pAACDes_t->ucAACType);
	}

	HxLOG_Info(" svc_cas_SiParseAACDes () Leaved!");

	return ERR_CAS_SI_RESULT_OK;

}

/************************************************************************/
/*	function : svc_cas_SiParseAppSignallingDes											*/
/*  																	*/
/*  Description : tag = 0x6a											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseAppSignallingDes (HUINT8 *p, svcCas_AppSignallingDes_t *pDes)
{
	svcCas_AppSignallingInfo_t	*pNewInfo = NULL, *pPrevInfo = NULL;
	HUINT16				usDesLen;

	HxLOG_Info("svc_cas_SiParseAppSignallingDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseAppSignallingDes Format!\n");
		/** < return ERR_CAS_SI_INVALID_DESCRIPTOR;
			  Application_signaling_descriptor에서 descriptor_length가
			  0일 수도 있다. 따라서 이 경우에 에러가 아니다.
			  JPROH in Italy. 2002/6/11 */
		return ERR_CAS_SI_RESULT_OK;
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
		pNewInfo = (svcCas_AppSignallingInfo_t *)svc_cas_SiAllocDesInfo(TAG_APPLICATION_SIGNALLING, 0);
		if (pNewInfo == NULL)
		{
			HxLOG_Print("[WARN] svc_cas_SiParseAppSignallingInfo Alloc error\n");
			HxLOG_Print(" svc_cas_SiParseAppSignallingDes () Leaved!");
			return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
		}

		pNewInfo->usAppType = get16bit (p);	/** < application type */
		p += 2; usDesLen -= 2;
		pNewInfo->ucAitVersion = (*p & 0x1f);	/** < AIT version number */
		p++; usDesLen --; /** < JPROH */

		if (pPrevInfo == NULL)
		{
			pDes->pInfo = pNewInfo;
			pPrevInfo = pNewInfo;
		}
		else
		{
			pPrevInfo->pNext = pNewInfo;
			pPrevInfo = pPrevInfo->pNext;
		}
	}

	HxLOG_Info(" svc_cas_SiParseAppSignallingDes () Leaved!");

	return ERR_CAS_SI_RESULT_OK;

}

/************************************************************************/
/*	function : svc_cas_SiParseCellListDes											*/
/*  																	*/
/*  Description : tag = 0x6c											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseCellListDes (HUINT8 *p, svcCas_CellListDes_t *pDes)
{
	svcCas_CellListInfo_t		*pNewInfo = NULL, *pPrevInfo = NULL;
	svcCas_SubCellListInfo_t	*pNewSubInfo = NULL, *pPrevSubInfo = NULL;
	HUINT8				ucSubCellLen;
	HUINT16				usDesLen;
	HUINT32				ulTmpValue;

	HxLOG_Info("svc_cas_SiParseCellListDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	usDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseCellListDes Format!\n");
		return ERR_CAS_SI_RESULT_OK;
	}

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		if (usDesLen < 10)
		{
			p += usDesLen;
			break;
		}

		/** < alloc Info */
		pNewInfo = (svcCas_CellListInfo_t *)svc_cas_SiAllocDesInfo(TAG_CELL_LIST, 0);
		if (pNewInfo == NULL)
		{
			HxLOG_Print("[WARN] svc_cas_SiParseCellListDes Alloc error\n");
			HxLOG_Print(" svc_cas_SiParseCellListDes () Leaved!");
			return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
		}

		pNewInfo->usCellId					= get16bit(p);
		p += 2; usDesLen -= 2;
		pNewInfo->usCellLatitude			= get16bit(p);
		p += 2; usDesLen -= 2;
		pNewInfo->usCellLongitude			= get16bit(p);
		p += 2; usDesLen -= 2;
		ulTmpValue = get32bit(p);
		pNewInfo->usCellExtentOfLatitude	= (HUINT16)((ulTmpValue >> 20) & (0x0fff));
		pNewInfo->usCellExtentOfLongitude	= (HUINT16)((ulTmpValue >> 8) & (0x0fff));
		p += 4; usDesLen -= 4;
		pNewInfo->pNextInfo = NULL;
		pNewInfo->pSubCellListInfo = NULL;

		ucSubCellLen = (HUINT8)(ulTmpValue & 0xff);

		while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT && ucSubCellLen > 0)
		{
			if (ucSubCellLen < 8)
			{
				p += ucSubCellLen;
				break;
			}

			/** < alloc Info */
			pNewSubInfo = (svcCas_SubCellListInfo_t *)svc_cas_SiAllocSubCellListInfo();
			if (NULL == pNewSubInfo)
			{
				HxLOG_Print("[WARN] svc_cas_SiParseCellListDes Alloc error\n");
				HxLOG_Print(" svc_cas_SiParseCellListDes () Leaved!");
				return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
			}

			pNewSubInfo->ucCellIdExtension			= get8bit(p);
			p += 1; usDesLen -= 1; ucSubCellLen -= 1;
			pNewSubInfo->usSubCellLatitude			= get16bit(p);
			p += 2; usDesLen -= 2; ucSubCellLen -= 2;
			pNewSubInfo->usSubCellLongitude			= get16bit(p);
			p += 2; usDesLen -= 2; ucSubCellLen -= 2;
			ulTmpValue = get24bit(p);
			pNewSubInfo->usSubCellExtentOfLatitude	= (HUINT16)((ulTmpValue >> 12) & (0x0fff));
			pNewSubInfo->usSubCellExtentOfLatitude	= (HUINT16)((ulTmpValue) & (0x0fff));
			p += 3; usDesLen -= 3; ucSubCellLen -= 3;
			pNewSubInfo->pNextInfo = NULL;
			if (pPrevSubInfo == NULL)
			{
				pNewInfo->pSubCellListInfo = pNewSubInfo;
				pPrevSubInfo = pNewSubInfo;
			}
			else
			{
				pPrevSubInfo->pNextInfo = pNewSubInfo;
				pPrevSubInfo = pPrevSubInfo->pNextInfo;
			}
		}

		if (pPrevInfo == NULL)
		{
			pDes->pCellListInfo = pNewInfo;
			pPrevInfo = pNewInfo;
		}
		else
		{
			pPrevInfo->pNextInfo = pNewInfo;
			pPrevInfo = pPrevInfo->pNextInfo;
		}
	}

	HxLOG_Info(" svc_cas_SiParseCellListDes () Leaved!");

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_SiParseUserDefinedDes (HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT32		 ulDesSize;

	if (pucRaw == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pucDes == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	ulDesSize = (HUINT32)pucRaw[1] + 2;
	HxSTD_MemCopy (pucDes, pucRaw, ulDesSize);

	return ERR_CAS_SI_RESULT_OK;
}

#define ____0X74_RELATED_CONTENT_____
/************************************************************************/
/*	function : svc_cas_SiParseRelatedContentDes											*/
/*  																	*/
/*  Description : tag = 0x74											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseRelatedContentDes (HUINT8 *p, svcCas_RelatedContentDes_t *pDes)
{
	HUINT8				ucDesLen;

	HxLOG_Info("svc_cas_SiParseRelatedContentDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	pDes->pPrivateData = NULL;

	HxLOG_Info(" svc_cas_SiParseRelatedContentDes () Leaved!");
	return ERR_CAS_SI_RESULT_OK;
}

#define ____0X7E_FTA_CONTENT_MANAGEMENT_DES_____
/************************************************************************/
/*	function : svc_cas_SiParseFtaContentManageDes											*/
/*  																	*/
/*  Description : tag = 0x7E											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseFtaContentManageDes (HUINT8 *p, svcCas_FtaContentManagementDes_t *pDes)
{
	HUINT8		ucDesLen,ucValue;

	HxLOG_Info("svc_cas_SiParseFtaContentManageDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseFtaContentManageDes Format!\n");
		return ERR_CAS_SI_RESULT_OK;
	}

	ucValue = *p;
	pDes->ucDoNotScramble = (HUINT8)((ucValue >> 3) & 0x1);
	pDes->ucCtrlRAccOverINet = (HUINT8)((ucValue >> 1) & 0x3);
	pDes->ucDoNotApplyRevocation = (HUINT8)(ucValue & 0x1);

	HxLOG_Print("\t\t\t(Do not scramble = %d)\n", pDes->ucDoNotScramble);
	HxLOG_Print("\t\t\t(Control Remote Access Over Internet = %d)\n", pDes->ucCtrlRAccOverINet);
	HxLOG_Print("\t\t\t(Do not Apply Revocation = %d)\n", pDes->ucDoNotApplyRevocation);

	HxLOG_Info(" svc_cas_SiParseFtaContentManageDes () Leaved!");
	return ERR_CAS_SI_RESULT_OK;
}

#define ____0X7F_EXTENSION_DES_____
/************************************************************************/
/*	function : svc_cas_SiParseExtensionDes											*/
/*  																	*/
/*  Description : tag = 0x7f											*/
/************************************************************************/

STATIC	HERROR	svc_cas_SiParseExtensionDes (HUINT8 *p, svcCas_ExtensionDes_t *pDes)
{
	HUINT8				ucDesLen;

	HxLOG_Info("svc_cas_SiParseExtensionDes () Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *p);
	p++;

	/** < descriptor length (1byte) */
	ucDesLen = *p++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", ucDesLen);

	if (ucDesLen < 1)
	{
		HxLOG_Print("[WARN] Unknown svc_cas_SiParseExtensionDes Format!\n");
		return ERR_CAS_SI_RESULT_OK;
	}
	pDes->ucExtDesTag = *p++; ucDesLen -= 1;
	HxLOG_Print("\t\t(extended descriptor's tag = %d)\n", pDes->ucExtDesTag);
	pDes->ucExtDesLen = ucDesLen;
	if (ucDesLen < 1 )
		pDes->pExtDesData = NULL;
	else
	{
		pDes->pExtDesData = OxCAS_Malloc(ucDesLen);
		HxSTD_MemCopy(pDes->pExtDesData,p,ucDesLen);
		HxLOG_Print("\t\t(extended descriptor's length = %d)\n", ucDesLen);
	}

	HxLOG_Info(" svc_cas_SiParseExtensionDes () Leaved!");
	return ERR_CAS_SI_RESULT_OK;
}

#define ____PARSE_COMMON_FUNC____

/************************************************************************/
/*	function : svc_cas_SiParseDes													*/
/*  																	*/
/*  Description : 														*/
/************************************************************************/

HERROR	svc_cas_SiParseDes(SIxTextEncoding_e eTextEnc, HUINT8 *p, HUINT8 *pDes)
{
	HUINT8			 ucDesTag;
	HUINT32			 ulRes;

	HxLOG_Info("svc_cas_SiParseDes() Entered!");

	ucDesTag = *p;

	if (MACRO_IS_USER_DEFINED_DESTAG(ucDesTag))
	{
		ulRes = svc_cas_SiParseUserDefinedDes(p, pDes);
		return ulRes;
	}

	ulRes = ERR_CAS_SI_INVALID_TAG;

	switch (ucDesTag)
	{

		case TAG_VIDEO_STREAM:	/** < tag = 0x02 */
			HxLOG_Print("\t\t[TAG_VIDEO_STREAM(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseVideoStmDes(p, (svcCas_VideoStmDes_t *) pDes);
			break;

		case TAG_AUDIO_STREAM:	/** < tag = 0x03 */
			HxLOG_Print("\t\t[TAG_AUDIO_STREAM(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseAudioStmDes(p, (svcCas_AudioStmDes_t *) pDes);
			break;

		case TAG_HIERARCHY:	/** < tag = 0x04 */
			HxLOG_Print("\t\t[TAG_HIERARCHY(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseHierDes(p, (svcCas_HierDes_t *) pDes);
			break;

		case TAG_REGISTRATION:	/** < tag = 0x05 */
			HxLOG_Print("\t\t[TAG_REGISTRATION(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseRegDes(p, (svcCas_RegDes_t *) pDes);
			break;

		case TAG_DATA_STREAM_ALIGNMENT:	/** < tag = 0x06 */
			HxLOG_Print("\t\t[TAG_DATA_STREAM_ALIGNMENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseDataStmAlignDes(p, (svcCas_DataStmAlignDes_t *) pDes);
			break;

		case TAG_TARGET_BACKGROUND_GRID:	/** < tag = 0x07 */
			HxLOG_Print("\t\t[TAG_TARGET_BACKGROUND_GRID(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseBackGridDes(p, (svcCas_BackGridDes_t *) pDes);
			break;

		case TAG_VIDEO_WINDOW:	/** < tag = 0x08 */
			HxLOG_Print("\t\t[TAG_VIDEO_WINDOW(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseVideoWindowDes(p, (svcCas_VideoWindowDes_t *) pDes);
			break;

		case TAG_CA:	/** < tag = 0x09 */
			HxLOG_Print("\t\t[TAG_CA(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseCaDes(p, (svcCas_CaDes_t *) pDes);
			break;

		case TAG_ISO_639_LANGUAGE:	/** < tag = 0x0a */
			HxLOG_Print("\t\t[TAG_ISO_639_LANGUAGE(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseIso639LangDes(p, (svcCas_Iso639LangDes_t *) pDes);
			break;

		case TAG_SYSTEM_CLOCK:	/** < tag = 0x0b */
			HxLOG_Print("\t\t[TAG_SYSTEM_CLOCK(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseSysClockDes(p, (svcCas_SysClockDes_t *) pDes);
			break;

		case TAG_MULTIPLEX_BUFFER_UTILIZATION:	/** < tag = 0x0c */
			HxLOG_Print("\t\t[TAG_MULTIPLEX_BUFFER_UTILIZATION(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseMultiplexBufUtilDes(p, (svcCas_MultiplexBufUtilDes_t *) pDes);
			break;

		case TAG_COPYRIGHT:	/** < tag = 0x0d */
			HxLOG_Print("\t\t[TAG_COPYRIGHT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseCopyrightDes(p, (svcCas_CopyRightDes_t *) pDes);
			break;

		case TAG_MAXIMUM_BITRATE:	/** < tag = 0x0e */
			HxLOG_Print("\t\t[TAG_MAXIMUM_BITRATE(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseMaxBitrateDes(p, (svcCas_MaxBitrateDes_t *) pDes);	/** < TEST O.K.*/
			break;

		case TAG_PRIVATE_DATA_INDICATOR:	/** < tag = 0x0f */
			HxLOG_Print("\t\t[TAG_PRIVATE_DATA_INDICATOR(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParsePrivateDataIndiDes(p, (svcCas_PrivateDataIndicatorDes_t *) pDes);	/** < TEST O.K.*/
			break;

		case TAG_SMOOTHING_BUFFER:	/** < tag = 0x10 */
			HxLOG_Print("\t\t[TAG_SMOOTHING_BUFFER(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseSmoothBufDes(p, (svcCas_SmoothBufDes_t *) pDes);
			break;

		case TAG_STD:	/** < tag = 0x11 */
			HxLOG_Print("\t\t[TAG_STD(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseStdDes(p, (svcCas_StdDes_t *) pDes);
			break;

		case TAG_CAROUSEL_ID:	/** < tag = 0x13 */
			HxLOG_Print("\t\t[TAG_CAROUSEL_ID(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseCarouselIdDes(p, (svcCas_CarouselIdDes_t *) pDes);
			break;

		case TAG_ASSOCIATION_TAG:	/** < tag = 0x14 */
			HxLOG_Print("\t\t[TAG_ASSOCIATION_TAG(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseAssocTagDes(p, (svcCas_AssocTagDes_t *) pDes);
			break;

		case TAG_DEFERRED_ASSOCIATION_TAG:	/** < tag = 0x15 */
			HxLOG_Print("\t\t[TAG_DEFERRED_ASSOCIATION_TAG(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseDeferAssocTagDes(p, (svcCas_DeferAssocTagDes_t *) pDes);
			break;

		case TAG_AVC_VIDEO:	/** < tag = 0x28 */
			HxLOG_Print("\t\t[TAG_AVC_VIDEO(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseAvcVideoDes(p, (svcCas_AvcVideoDes_t *) pDes);
			break;

		case TAG_NETWORK_NAME:	/** < tag = 0x40 */
			HxLOG_Print("\t\t[TAG_NETWORK_NAME(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseNetNameDes(eTextEnc, p, (svcCas_NetNameDes_t *) pDes);
			break;

		case TAG_SERVICE_LIST:	/** < tag = 0x41 */
			HxLOG_Print("\t\t[TAG_SERVICE_LIST(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseServListDes(p, (svcCas_SvcListDes_t *) pDes);
			break;

		case TAG_SATELLITE_DELIVERY_SYSTEM:		/** < tag = 0x43 */
			HxLOG_Print("\t\t[TAG_SATELLITE_DELIVERY_SYSTEM(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseSatDeliSystemDes(p, (svcCas_SatDeliSystemDes_t *) pDes);
			break;

		case TAG_CABLE_DELIVERY_SYSTEM:		/** < tag = 0x44 */
			HxLOG_Print("\t\t[TAG_CABLE_DELIVERY_SYSTEM(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseCabDeliSystemDes(p, (svcCas_CabDeliSystemDes_t *) pDes);
			break;

		case TAG_BOUQUET_NAME:	/** < tag = 0x47 */
			HxLOG_Print("\t\t[TAG_BOUQUET_NAME(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseBouqNameDes(eTextEnc, p, (svcCas_BouqNameDes_t *) pDes);
			break;

		case TAG_SERVICE:	/** < tag = 0x48 */
			HxLOG_Print("\t\t[TAG_SERVICE(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseServDes(eTextEnc, p, (svcCas_SvcDes_t *) pDes);
			break;

		case TAG_COUNTRY_AVAILABILITY:	/** < tag = 0x49 */
			HxLOG_Print("\t\t[TAG_COUNTRYAVAILABILITY(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseCountryAvailDes(p, (svcCas_CountryAvailDes_t *) pDes);	/** < TEST O.K.*/
			break;

		case TAG_LINKAGE:	/** < tag = 0x4a */
			HxLOG_Print("\t\t[TAG_LINKAGE(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseLinkDes(p, (svcCas_LinkDes_t *) pDes);
			break;

		case TAG_NVOD_REFERENCE:	/** < tag = 0x4b */
			HxLOG_Print("\t\t[TAG_NVOD_REFERENCE(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseNvodRefDes(p, (svcCas_NvodRefDes_t *) pDes);	/** < TEST O.K. */
			break;

		case TAG_TIME_SHIFTED_SERVICE:		/** < tag = 0x4c */
			HxLOG_Print("\t\t[TAG_TIME_SHIFT_SERVICE(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseTimeShiftServDes(p, (svcCas_TimeShiftSvcDes_t *) pDes);	/** < TEST O.K.*/
			break;

		case TAG_SHORT_EVENT:	/** < tag = 0x4d */
			HxLOG_Print("\t\t[TAG_SHORT_EVENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseShortEvtDes(eTextEnc, p, (svcCas_ShortEventDes_t *) pDes);
			break;

		case TAG_EXTENDED_EVENT:	/** < tag = 0x4e */
			HxLOG_Print("\t\t[TAG_EXTENDED_EVENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseExtEvtDes(eTextEnc, p, (svcCas_ExtEvtDes_t *) pDes);
			break;

		case TAG_TIME_SHIFTED_EVENT:	/** < tag = 0x4f */
			HxLOG_Print("\t\t[TAG_TIME_SHIFTED_EVENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseTimeShiftEvtDes(p, (svcCas_TimeShiftEvtDes_t *) pDes);
			break;

		case TAG_COMPONENT:	/** < tag = 0x50 */
			HxLOG_Print("\t\t[TAG_COMPONENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseCompDes(eTextEnc, p, (svcCas_CompDes_t *) pDes);	/** < TEST O.K.*/
			break;

		case TAG_MOSAIC:	/* < tag = 0x51 */
			HxLOG_Print("\t\t[TAG_MOSAIC(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseMosaicDes(p, (svcCas_MosaicDes_t *) pDes);
			break;

		case TAG_STREAM_IDENTIFIER:	/** < tag = 0x52 */
			HxLOG_Print("\t\t[TAG_STREAM_IDENTIFIER(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseStmIdentDes(p, (svcCas_StmIdentDes_t *) pDes);	/** < TEST O.K.*/
			break;

		case TAG_CA_IDENTIFIER:	/** < tag = 0x53 */
			HxLOG_Print("\t\t[TAG_CA_IDENTIFIER(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseCaIdentDes(p, (svcCas_CaIdentDes_t *) pDes);	/** < TEST O.K. */
			break;

		case TAG_CONTENT:	/** < tag = 0x54 */
			HxLOG_Print("\t\t[TAG_CONTENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseContentDes(p, (svcCas_ContentDes_t *) pDes);
			break;

		case TAG_PARENTAL_RATING:	/** < tag = 0x55 */
			HxLOG_Print("\t\t[TAG_PARENTAL_RATING(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseParentRateDes(p, (svcCas_ParentDes_t *) pDes);
			break;

		case TAG_TELETEXT:	/** < tag = 0x56 */
			HxLOG_Print("\t\t[TAG_TELETEXT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseTeletextDes(p, (svcCas_TeletextDes_t *) pDes);
			break;

		case TAG_TELEPHONE:	/** < tag = 0x57 */
			HxLOG_Print("\t\t[TAG_TELEPHONE(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseTelephoneDes(eTextEnc, p, (svcCas_TelephoneDes_t *) pDes);
			break;

		case TAG_LOCAL_TIME_OFFSET:	/** < tag = 0x58 */
			HxLOG_Print("\t\t[TAG_LOCAL_TIME_OFFSET(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseLocalTimeOffsetDes(p, (svcCas_LocalTimeDes_t *) pDes);
			break;

		case TAG_SUBTITLING:	/** < tag = 0x59 */
			HxLOG_Print("\t\t[TAG_SUBTITLING(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseSubtitleDes(p, (svcCas_SubTitleDes_t *) pDes);
			break;

		case TAG_TERRESTRIAL_DELIVERY_SYSTEM:	/** < tag = 0x5a */
			HxLOG_Print("\t\t[TAG_TERRESTRIAL_DELIVERY_SYSTEM(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseTerDeliSystemDes(p, (svcCas_TerDeliSystemDes_t *) pDes);
			break;

		case TAG_MULTILINGUAL_NETWORK_NAME:	/** < tag = 0x5b */
			HxLOG_Print("\t\t[TAG_MULTILINGUAL_NETWORK_NAME(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseMultiNetNameDes(eTextEnc, p, (svcCas_MultiNetNameDes_t *) pDes);
			break;

		case TAG_MULTILINGUAL_BOUQUET_NAME:	/** < tag = 0x5c */
			HxLOG_Print("\t\t[TAG_MULTILINGUAL_BOUQUET_NAME(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseMultiBouqNameDes(eTextEnc, p, (svcCas_MultiBouqNameDes_t *) pDes);
			break;

		case TAG_MULTILINGUAL_SERVICE_NAME:	/** < tag = 0x5d */
			HxLOG_Print("\t\t[TAG_MULTILINGUAL_SERVICE_NAME(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseMultiServNameDes(eTextEnc, p, (svcCas_MultiSvcNameDes_t *) pDes);
			break;

		case TAG_MULTILINGUAL_COMPONENT:	/** < tag = 0x5e */
			HxLOG_Print("\t\t[TAG_MULTILINGUAL_COMPONENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseMultiCompDes(eTextEnc, p, (svcCas_MultiCompDes_t *) pDes);
			break;

		case TAG_PRIVATE_DATA_SPECIFIER:	/** < tag = 0x5f */
			HxLOG_Print("\t\t[TAG_PRIVATE_DATA_SPECIFIER(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParsePrivDataSpecDes(p, (svcCas_PrivateDataSpecDes_t *) pDes);
			break;

		case TAG_SERVICE_MOVE:				/** < tag = 0x60 */
			HxLOG_Print("\t\t[TAG_SERVICE_MOVE(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseServMoveDes(p, (svcCas_SvcMoveDes_t *) pDes);
			break;

		case TAG_SHORT_SMOOTHING_BUFFER:	/** < tag = 0x61 */
			HxLOG_Print("\t\t[TAG_SHORT_SMOOTHING_BUFFER(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseShortSmoothBufDes(p, (svcCas_ShortSmoothBufDes_t *) pDes);
			break;

		case TAG_FREQUENCY_LIST:			/** < tag = 0x62 */
			HxLOG_Print("\t\t[TAG_FREQUENCY_LIST(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseFrequncyListDes(p, (svcCas_FrequencyListDes_t *) pDes);
			break;

		case TAG_PARTIAL_TRANSPORT_STREAM:			/** < tag = 0x63 */
			HxLOG_Print("\t\t[TAG_PARTIAL_TRANSPORT_STREAM(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParsePartialTransportStreamDes(p, (svcCas_PartialTsDes *) pDes);
			break;

		case TAG_DATA_BROADCAST:			/** < tag = 0x64 */
			HxLOG_Print("\t\t[TAG_DATA_BROADCAST(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseDataBroadcastDes(eTextEnc, p, (svcCas_DataBroadcastDes_t *) pDes);
			break;

		case TAG_DATA_BROADCAST_ID:			/** < tag = 0x66 */
			HxLOG_Print("\t\t[TAG_DATA_BROADCAST_ID(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseDataBroadcastIdDes(p, (svcCas_DataBroadcastIdDes_t *) pDes);
			break;

		case TAG_PDC:						/** < tag = 0x69 */
			HxLOG_Print("\t\t[TAG_PDC(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParsePDCDes(p, (svcCas_PDCDes_t *)pDes);
			break;

		case TAG_DOLBY_AC3:					/** < tag = 0x6a */
			HxLOG_Print("\t\t[TAG_DOLBY_AC3(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseDolbyAC3Des(p, (svcCas_DolbyAc3Des_t *)pDes);
			break;

		case TAG_DEFAULT_AUTORITY:
			HxLOG_Print("\t\t[TAG_DEFAULT_AUTORITY(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseDefaultAuthorityDes(p, (svcCas_DefaultAuthorityDes_t *)pDes);
			break;

		case TAG_RELATED_CONTENT:
			HxLOG_Print("\t\t[TAG_RELATED_CONTENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseRelatedContentDes(p, (svcCas_RelatedContentDes_t *)pDes);
			break;

		case TAG_CONTENT_IDENTIFIER:
			HxLOG_Print("\t\t[TAG_CONTENT_IDENTIFIER(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseContentIdentifierDes(p, (svcCas_ContentIdentifierDes_t *)pDes);
			break;

		case TAG_S2_SATELLITE_DELIVERY_SYSTEM:/** <tag = 0x79, */
			HxLOG_Print("\t\t[TAG_S2_SATELLITE_DELIVERY_SYSTEM(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseS2SatDeliSystemDes(p,(svcCas_S2SatDeliSystemDes_t *)pDes);
			break;

		case TAG_ENHANCED_AC3:				/** <tag = 0x7A, */
			HxLOG_Print("\t\t[TAG_ENHANCED_AC3(0x%x)]\n", ucDesTag);
			ulRes =svc_cas_SiParseEnhancedAC3Des(p,(svcCas_EnhancedAC3Des_t*)pDes);
			break;

		case TAG_AAC:						/** <tag = 0x7C, */
			HxLOG_Print("\t\t[TAG_AAC(0x%x)]\n", ucDesTag);
			ulRes =svc_cas_SiParseAACDes(p,(svcCas_AACDes_t*)pDes);
			break;

		case TAG_APPLICATION_SIGNALLING:	/** < tag = 0x6f */
			HxLOG_Print("\t\t[TAG_APPLICATION_SIGNALLING(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseAppSignallingDes(p, (svcCas_AppSignallingDes_t *)pDes);
			break;

		case TAG_CELL_LIST:	/** < tag = 0x6f */
			HxLOG_Print("\t\t[TAG_CELL_LIST(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseCellListDes(p, (svcCas_CellListDes_t *)pDes);
			break;

		case TAG_FTA_CONTENT_MANAGEMENT: /** < tag = 0x7E */
			HxLOG_Print("\t\t[TAG_FTA_CONTENT_MANAGEMENT(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseFtaContentManageDes(p, (svcCas_FtaContentManagementDes_t *)pDes);
			break;

		case TAG_EXTENSION:	/** < tag = 0x7f */
			HxLOG_Print("\t\t[TAG_EXTENSION(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseExtensionDes(p, (svcCas_ExtensionDes_t *)pDes);
			break;

		default:
			HxLOG_Print("\t\t[Current not svc_cas_SiParsed tag! (0x%x)]\n", ucDesTag);
			ulRes = ERR_CAS_SI_INVALID_TAG;
			break;
	}

	if (ulRes == ERR_CAS_SI_RESULT_OK)
	{
		svcCas_SiBaseDesForm_t	*pstBaseForm;

		pstBaseForm = (svcCas_SiBaseDesForm_t *)pDes;
		if (pstBaseForm != NULL)
		{
			pstBaseForm->ucDesTag = ucDesTag;
		}
	}

	HxLOG_Info("svc_cas_SiParseDes() Leaved!");

	return ulRes;
}

#if 0
/************************************************************************/
/*	function : svc_cas_SiParseDummyNetNameDes										*/
/*  																	*/
/*  Description : tag = 0x40											*/
/************************************************************************/

STATIC 	HERROR	svc_cas_SiParseDummyNetNameDes(svcCas_NetNameDes_t *pDes)
{
	svcCas_NetNameDes_t	*pNetNameDes = pDes;
	HUINT8			ucNetNameLen = 0;
	HUINT8			szNetName[128];

	HxLOG_Info("svc_cas_SiParseDummyNetNameDes() Entered!");

	/** < network name */
	szNetName[ucNetNameLen] = '\0';

	/** < Descriptor 데이타 구성. */
	pNetNameDes->pszNetName = svc_cas_SiAllocMem(ucNetNameLen+1);
	if (pNetNameDes->pszNetName == NULL)
		goto ErrBlock;
	svc_cas_SiSetNetNameDes(pNetNameDes, szNetName);

	HxLOG_Info(" svc_cas_SiParseDummyNetNameDes() Leaved!");

	return ERR_CAS_SI_RESULT_OK;

ErrBlock:
	return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
}


/************************************************************************/
/*	function : svc_cas_SiParseDummyDes											*/
/*  																	*/
/*  Description : 가상의 descriptor 구성.								*/
/************************************************************************/

HERROR	svc_cas_SiParseDummyDes (HUINT8 ucDesTag, HUINT8 *pDes)
{
	HUINT32	ulRes = (HUINT32)-1;


	HxLOG_Info("svc_cas_SiParseDummyDes() Entered!");

	switch (ucDesTag)
	{
		case TAG_NETWORK_NAME:	/** < 0x40 */
			HxLOG_Print("\t\t[TAG_NETWORK_NAME(0x%x)]\n", ucDesTag);
			ulRes = svc_cas_SiParseDummyNetNameDes((svcCas_NetNameDes_t *)pDes);
			break;
	}

	HxLOG_Info(" svc_cas_SiParseDummyDes() Leaved!");

	return ulRes;
}
#endif


/*********************** End of File ******************************/
