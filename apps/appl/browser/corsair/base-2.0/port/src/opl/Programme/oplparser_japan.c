/**************************************************************************************/
/**
 * @file oplparser.c
 *
 * Defines DVB programme routine
 *
 * @author  seung bum kim (sbkim@humaxdigital.com)
 * @date    2012/09/20
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>
#include <silib.h>

#include <apk.h>
#include <oplparser_japan.h>

#define	return_if_void(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

typedef HUINT8* (*descAlloc_t)(void);
typedef void  	(*descFree_t)(void *pucDesc);
typedef HERROR (*descParse_t)(HUINT8 *pucRaw, HUINT8 *pucDes);
typedef void (*descPrint_t)(HUINT8 *pucDes);

typedef struct tagOPLParsingTable
{
	HINT32		tag;
	descAlloc_t	alloc;
	descFree_t 	free;
	descParse_t	parse;
	descPrint_t	print;
}OPLParsingTable;

static OPLParsingTable s_parsingTable[] = {
	{
		TAG_DIGITAL_COPY_CONTROL,
		OPL_PARSER_JAPAN_AllocDigitalCopyControlDes,
		OPL_PARSER_JAPAN_FreeDigitalCopyControlDes,
		OPL_PARSER_JAPAN_ParseDigitalCopyControlDes,
		OPL_PARSER_JAPAN_PrintDigitalCopyControlDes
	},
	{
		TAG_AUDIO_COMPONENT,
		OPL_PARSER_JAPAN_AllocAudioComponentDes,
		OPL_PARSER_JAPAN_FreeAudioComponentDes,
		OPL_PARSER_JAPAN_ParseAudioComponentDes,
		OPL_PARSER_JAPAN_PrintAudioComponentDes
	},
	{
		TAG_DATA_CONTENTS,
		OPL_PARSER_JAPAN_AllocDataContentsDes,
		OPL_PARSER_JAPAN_FreeDataContentsDes,
		OPL_PARSER_JAPAN_ParseDataContentsDes,
		OPL_PARSER_JAPAN_PrintDataContentsDes
	},
	{
		TAG_EVENT_GROUP,
		OPL_PARSER_JAPAN_AllocEventGroupDes,
		OPL_PARSER_JAPAN_FreeEventGroupDes,
		OPL_PARSER_JAPAN_ParseEventGroupDes,
		OPL_PARSER_JAPAN_PrintEventGroupDes
	},
	{
		TAG_COMPONENT_GROUP,
		OPL_PARSER_JAPAN_AllocEventComponentGroupDes,
		OPL_PARSER_JAPAN_FreeEventComponentGroupDes,
		OPL_PARSER_JAPAN_ParseEventComponentGroupDes,
		OPL_PARSER_JAPAN_PrintEventComponentGroupDes
	},
	{
		TAG_CONTENT_AVAILABILITY,
		OPL_PARSER_JAPAN_AllocEventContentAvailabilityDes,
		OPL_PARSER_JAPAN_FreeEventContentAvailabilityDes,
		OPL_PARSER_JAPAN_ParseEventContentAvailabilityDes,
		OPL_PARSER_JAPAN_PrintEventContentAvailabilityDes
	},
	{
		TAG_CA_CONTRACT_INFOMATION,
		OPL_PARSER_JAPAN_AllocCaContractInfoDes,
		OPL_PARSER_JAPAN_FreeCaContractInfoDes,
		OPL_PARSER_JAPAN_ParseCaContractInfoDes,
		OPL_PARSER_JAPAN_PrintCaContractInfoDes
	},
	{
		TAG_CA,
		OPL_PARSER_JAPAN_AllocCaDes,
		OPL_PARSER_JAPAN_FreeCaDes,
		OPL_PARSER_JAPAN_ParseCaDes,
		OPL_PARSER_JAPAN_PrintCaDes
	}
};

OPLParsingTable *OPL_PARSER_JAPAN_GetParsingDesc( HINT32 tag )
{
	HINT32			max =0, i=0;
	OPLParsingTable	*pRetTable = NULL;

	max = sizeof( s_parsingTable ) / sizeof( OPLParsingTable );

	if( max > 0 )
	{
		for( i =0; i< max; i++)
		{
			if( s_parsingTable[i].tag == tag )
			{
				pRetTable = &s_parsingTable[i];
				break;
			}
		}
	}

	return pRetTable;
}

#define ______0xC1_DIGITAL_COPY_CONTROL_DESCRIPTOR______
void  OPL_PARSER_JAPAN_FreeDigitalCopyControlDes(void *pucDesc)
{
	APKS_EventDigitalCopyControl_t 	*pstDes = (APKS_EventDigitalCopyControl_t *)pucDesc;

	if(pstDes == NULL)
	{
		return;
	}

	if(pstDes->ucDesTag != TAG_DIGITAL_COPY_CONTROL)
	{
		HxLOG_Error("ucDesTag(0x%X) invalid(%s, %d)\n", pstDes->ucDesTag, __FUNCTION__, __LINE__);
		return;
	}

	if(pstDes != NULL)
	{
		if(pstDes->pstComponent != NULL)
		{
			HLIB_STD_MemFree(pstDes->pstComponent);
			pstDes->pstComponent = NULL;
		}
		HLIB_STD_MemFree (pstDes);
		pstDes = NULL;
	}
}

HUINT8*  OPL_PARSER_JAPAN_AllocDigitalCopyControlDes(void)
{
	APKS_EventDigitalCopyControl_t	*pstDes;

	pstDes = (APKS_EventDigitalCopyControl_t *)HLIB_STD_MemAlloc(sizeof(APKS_EventDigitalCopyControl_t));
	ARIB_DES_CHECK_MEM(pstDes);
	if(pstDes != NULL)
	{
		memset(pstDes, 0, sizeof(APKS_EventDigitalCopyControl_t));
	}

	return (HUINT8 *)pstDes;
}

HERROR  OPL_PARSER_JAPAN_ParseDigitalCopyControlDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8				 	ucDesTag, ucDesLen;
	APKS_EventDigitalCopyControl_t			*pstDes;
	APKS_EventDigitalCopyControlComponent_t *pstComponent = NULL;
	HUINT8					*pucTemp;
	HINT8					ncComponentControlLen;
	HUINT8					ucNum;
	HUINT8					ucMaximumBitRateFlag;
	HUINT8					ucCopyControlType;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pstDes = (APKS_EventDigitalCopyControl_t *)pucDes;

	ucDesTag = pucRaw[0];
	ucDesLen = pucRaw[1];
	pucRaw += 2;

	if((ucDesTag != TAG_DIGITAL_COPY_CONTROL) || (ucDesLen == 0))
	{
		return ERR_FAIL;
	}

	pstDes->ucDesTag = ucDesTag;
	pstDes->ucDigitalRecordingControlData = (*pucRaw & 0xC0) >> 6;
	pstDes->ucMaximumBitRateFlag = (*pucRaw & 0x20) >> 5;
	pstDes->ucComponentControlFlag = (*pucRaw & 0x10) >> 4;
	pstDes->ucCopyControlType = (*pucRaw & 0x0C) >> 2;		/* TR-B15/B14 volume 4 table 30-20 (not STD-B10) */
	pstDes->ucAPSControlData = (*pucRaw & 0x03);		/* TR-B15/B14 volume 4 table 30-20 (not STD-B10) */
	pucRaw += 1;
	if(pstDes->ucMaximumBitRateFlag == 1)
	{
		pstDes->ucMaximumBitRate = *pucRaw++;
	}
	if(pstDes->ucComponentControlFlag == 1)
	{
		pucTemp = pucRaw;
		ncComponentControlLen = *pucTemp++;
		ucNum = 0;

		while(ncComponentControlLen > 0)
		{
			if(ncComponentControlLen < 2)
			{
				HxLOG_Error("Componet control length(%d) is invalid. (%d)\n", ncComponentControlLen, __LINE__);
			}
			pucTemp++;	/* component_tag */
			ucMaximumBitRateFlag = (*pucTemp & 0x20) >> 5;
			ucCopyControlType = (*pucTemp & 0x0C) >> 2;
			pucTemp++;
			ncComponentControlLen -= 2;
			if(ucMaximumBitRateFlag == 1)
			{
				pucTemp++;
				ncComponentControlLen--;
			}
			ucNum++;
		}

		pstDes->ucNumComponents = ucNum;
		if(ucNum > 0)
		{
			pstDes->pstComponent = (APKS_EventDigitalCopyControlComponent_t *)HLIB_STD_MemAlloc(sizeof(APKS_EventDigitalCopyControlComponent_t) * ucNum);
			ARIB_DES_CHECK_MEM(pstDes->pstComponent);
			if(pstDes->pstComponent != NULL)
			{
				memset(pstDes->pstComponent, 0, (sizeof(APKS_EventDigitalCopyControlComponent_t) * ucNum));
			}
			else
			{
				return ERR_FAIL;
			}
		}

		ncComponentControlLen = *pucRaw++;
		ucNum = 0;

		while(ncComponentControlLen > 0)
		{
			pstComponent = &pstDes->pstComponent[ucNum];
			if(ncComponentControlLen < 2)
			{
				HxLOG_Error("Componet control length(%d) is invalid. (%d)\n", ncComponentControlLen, __LINE__);
				break;
			}

			pstComponent->ucComponentTag = *pucRaw++;
			pstComponent->ucDigitalRecordingControlData = (*pucRaw & 0xC0) >> 6;
			pstComponent->ucMaximumBitRateFlag = (*pucRaw & 0x20) >> 5;
			pstComponent->ucCopyControlType = (*pucRaw & 0x0C) >> 2;
			pstComponent->ucAPSControlData = ((*pucRaw++) & 0x03);
			ncComponentControlLen -= 2;
			if(pstComponent->ucMaximumBitRateFlag == 1)
			{
				pstComponent->ucMaximumBitRate = *pucRaw++;
				ncComponentControlLen--;
			}
			ucNum++;
		}
	}

	return ERR_OK;
}

void  OPL_PARSER_JAPAN_PrintDigitalCopyControlDes(HUINT8 *pucDes)
{
	APKS_EventDigitalCopyControl_t			*pstDes;

	if (pucDes == NULL)					{ return; }

	pstDes = (APKS_EventDigitalCopyControl_t *)pucDes;

	HxLOG_Print( "\n\n##### DIGITAL COPY CONTROL DESCRIPTOR #####\n\n");

	HxLOG_Print("ucDesTag : 0x%x\n" , pstDes->ucDesTag );
	HxLOG_Print("ucDigitalRecordingControlData : 0x%x\n" , pstDes->ucDigitalRecordingControlData );
	HxLOG_Print("ucMaximumBitRateFlag : 0x%x\n" , pstDes->ucMaximumBitRateFlag );
	HxLOG_Print("ucComponentControlFlag : 0x%x\n" , pstDes->ucComponentControlFlag );
	HxLOG_Print("ucCopyControlType : 0x%x\n" , pstDes->ucCopyControlType );
	HxLOG_Print("ucAPSControlData : 0x%x\n" , pstDes->ucAPSControlData );
	HxLOG_Print("ucMaximumBitRate : 0x%x\n" , pstDes->ucMaximumBitRate );
	HxLOG_Print("ucNumComponents : 0x%x\n" , pstDes->ucNumComponents );

	HxLOG_Print("pstComponent\n");
	if( pstDes->pstComponent )
	{
		HxLOG_Print("	ucComponentTag : 0x%x\n" , pstDes->pstComponent->ucComponentTag );
		HxLOG_Print("	ucDigitalRecordingControlData : 0x%x\n" , pstDes->pstComponent->ucDigitalRecordingControlData );
		HxLOG_Print("	ucMaximumBitRateFlag : 0x%x\n" , pstDes->pstComponent->ucMaximumBitRateFlag );
		HxLOG_Print("	ucCopyControlType : 0x%x\n" , pstDes->pstComponent->ucCopyControlType );
		HxLOG_Print("	ucAPSControlData : 0x%x\n" , pstDes->pstComponent->ucAPSControlData );
		HxLOG_Print("	ucAPSControlData : 0x%x\n" , pstDes->pstComponent->ucAPSControlData );
	}

	HxLOG_Print( "\n\n##### ##### END #### #####\n\n");
}

#define ______0xC4_AUDIO_COMPONENT_DESCRIPTOR______

void  OPL_PARSER_JAPAN_FreeAudioComponentDes(void *pucDesc)
{
	APKS_EventAudioComponent_t 	*pstDes = (APKS_EventAudioComponent_t *)pucDesc;
	APKS_EventAudioComponent_t	*pstTemp = NULL;

	if(pstDes == NULL)
	{
		return;
	}

	if(pstDes->ucDesTag != TAG_AUDIO_COMPONENT)
	{
		HxLOG_Error("ucDesTag(0x%X) invalid(%s, %d)\n", pstDes->ucDesTag, __FUNCTION__, __LINE__);
		return;
	}

	while(pstDes != NULL)
	{
		if(pstDes->pucTextChar != NULL)
		{
			HLIB_STD_MemFree(pstDes->pucTextChar);
			pstDes->pucTextChar = NULL;
		}
		pstTemp = pstDes->pstNext;
		HLIB_STD_MemFree (pstDes);
		pstDes = pstTemp;
	}
}

HUINT8*  OPL_PARSER_JAPAN_AllocAudioComponentDes(void)
{
	APKS_EventAudioComponent_t	*pstDes;

	pstDes = (APKS_EventAudioComponent_t *)HLIB_STD_MemAlloc(sizeof(APKS_EventAudioComponent_t));
	ARIB_DES_CHECK_MEM(pstDes);
	if(pstDes != NULL)
	{
		memset(pstDes, 0, sizeof(APKS_EventAudioComponent_t));
	}

	return (HUINT8 *)pstDes;
}

HERROR  OPL_PARSER_JAPAN_ParseAudioComponentDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8				 ucDesTag, ucDesLen, ucTempLen;
	APKS_EventAudioComponent_t	*pstDes;
 	HUINT8				szIso639LangCode[4], *pszText = NULL;
	HUINT32				nTempTextLen;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pstDes = (APKS_EventAudioComponent_t *)pucDes;

	ucDesTag = pucRaw[0];
	ucDesLen = pucRaw[1];

	if((ucDesTag != TAG_AUDIO_COMPONENT) || (ucDesLen == 0))
	{
		return ERR_FAIL;
	}

 	pstDes->ucDesTag = ucDesTag;
	pstDes->ucStreamContent = pucRaw[2] & 0x0F;
	pstDes->ucComponentType = pucRaw[3];
	pstDes->ucComponentTag = pucRaw[4];
	pstDes->ucStreamType = pucRaw[5];
	pstDes->ucSimulcastGroupTag = pucRaw[6];
	pstDes->bESMultiLlingualFlag = ((pucRaw[7] & 0x80) >> 7)?TRUE:FALSE;
	pstDes->bMainComponentFlag = ((pucRaw[7] & 0x40) >> 6)?TRUE:FALSE;
	pstDes->ucQualityIndicator = (pucRaw[7] & 0x30) >> 4;
	pstDes->ucSamplingRate = (pucRaw[7] & 0x0E) >> 1;
	pstDes->unISO639LanguageCode = HxMACRO_Get24Bit(pucRaw + 8);
	memcpy (szIso639LangCode, (pucRaw + 8), 3);
	szIso639LangCode [3] = '\0';
	if(pstDes->bESMultiLlingualFlag == 1)
	{
		pstDes->unISO639LanguageCode2 = HxMACRO_Get24Bit(pucRaw + 11);
		pstDes->ucTextCharLen = ucTempLen = ucDesLen - 12;
		pucRaw += 14;
	}
	else
	{
		pstDes->unISO639LanguageCode2 = -1;
		pstDes->ucTextCharLen = ucTempLen = ucDesLen - 9;
		pucRaw += 11;
	}


	pszText = (HUINT8 *)SILIB_TEXT_MakeUtf8Ex(eSIxTEXT_ENC_JIS, (const HCHAR *)szIso639LangCode, pucRaw, (HUINT32)pstDes->ucTextCharLen, NULL, NULL);
	if(pszText != NULL)
	{
		nTempTextLen = HxSTD_StrLen((HCHAR *)pszText);
		pstDes->pucTextChar = HLIB_STD_MemAlloc(sizeof(HUINT8) * (nTempTextLen + 1));
		if(pstDes->pucTextChar != NULL)
		{
			memset(pstDes->pucTextChar, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
			memcpy(pstDes->pucTextChar, pszText, HxSTD_StrLen((HCHAR *)pszText));
			pstDes->pucTextChar[HxSTD_StrLen((HCHAR *)pszText)] = ARIB_DES_EOS;
		}
		HLIB_STD_MemFree(pszText);
	}
	return ERR_OK;
}

void  OPL_PARSER_JAPAN_PrintAudioComponentDes(HUINT8 *pucDes)
{
	APKS_EventAudioComponent_t			*pstDes;

	if (pucDes == NULL)					{ return; }

	pstDes = (APKS_EventAudioComponent_t *)pucDes;

	HxLOG_Print( "\n\n##### AUDIO COMPONENT DESCRIPTOR #####\n\n");

	do
	{
		HxLOG_Print("ucDesTag : 0x%x\n" , pstDes->ucDesTag );
		HxLOG_Print("ucStreamContent : 0x%x\n" , pstDes->ucStreamContent );
		HxLOG_Print("ucComponentType : 0x%x\n" , pstDes->ucComponentType );
		HxLOG_Print("ucComponentTag : 0x%x\n" , pstDes->ucComponentTag );
		HxLOG_Print("ucStreamType : 0x%x\n" , pstDes->ucStreamType );
		HxLOG_Print("ucSimulcastGroupTag : 0x%x\n" , pstDes->ucSimulcastGroupTag );
		HxLOG_Print("bESMultiLlingualFlag : 0x%x\n" , pstDes->bESMultiLlingualFlag );
		HxLOG_Print("bMainComponentFlag : 0x%x\n" , pstDes->bMainComponentFlag );
		HxLOG_Print("ucQualityIndicator : 0x%x\n" , pstDes->ucQualityIndicator );
		HxLOG_Print("ucSamplingRate : 0x%x\n" , pstDes->ucSamplingRate );
		HxLOG_Print("unISO639LanguageCode : %d\n" , pstDes->unISO639LanguageCode );
		HxLOG_Print("unISO639LanguageCode2 : %d\n" , pstDes->unISO639LanguageCode2 );
		HxLOG_Print("ucTextCharLen : %d\n" , pstDes->ucTextCharLen );
		HxLOG_Print("pucTextChar : ");
		if( pstDes->pucTextChar == NULL )
		{
			HxLOG_Print( "\n" );
		}
		else
		{
			HxLOG_Print ( "%s\n" , pstDes->pucTextChar );
		}
		pstDes = pstDes->pstNext;
	}while( pstDes );

	HxLOG_Print( "\n\n##### ##### END ##### #####\n\n");
}

#define ______0xC7_DATA_CONTENTS_DESCRIPTOR______

void  OPL_PARSER_JAPAN_FreeDataContentsDes(void *pucDesc)
{
	APKS_EventDataContentDes_t 	*pstDes = (APKS_EventDataContentDes_t *)pucDesc;
	APKS_EventDataContentDes_t	*pstTemp = NULL;

	if(pstDes == NULL)
	{
		return;
	}

	if(pstDes->ucDesTag != TAG_DATA_CONTENTS)
	{
		HxLOG_Error("ucDesTag(0x%X) invalid(%s, %d)\n", pstDes->ucDesTag, __FUNCTION__, __LINE__);
		return;
	}

	while(pstDes != NULL)
	{
		if(pstDes->pucComponentRef != NULL)
		{
			HLIB_STD_MemFree(pstDes->pucComponentRef);
			pstDes->pucComponentRef = NULL;
		}
		if(pstDes->pucTextChar != NULL)
		{
			HLIB_STD_MemFree(pstDes->pucTextChar);
			pstDes->pucTextChar = NULL;
		}
		if(pstDes->pstAribBxmlInfo != NULL)
		{
			if(pstDes->pstAribBxmlInfo->pstCarouselInfo != NULL)
			{
				if(pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent != NULL)
				{
					HLIB_STD_MemFree(pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent);
					pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent = NULL;
				}
				HLIB_STD_MemFree(pstDes->pstAribBxmlInfo->pstCarouselInfo);
				pstDes->pstAribBxmlInfo->pstCarouselInfo = NULL;
			}
			if(pstDes->pstAribBxmlInfo->pstStoredCarouselInfo != NULL)
			{
				if(pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent != NULL)
				{
					HLIB_STD_MemFree(pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent);
					pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent = NULL;
				}
				HLIB_STD_MemFree(pstDes->pstAribBxmlInfo->pstStoredCarouselInfo);
				pstDes->pstAribBxmlInfo->pstStoredCarouselInfo = NULL;
			}
			HLIB_STD_MemFree(pstDes->pstAribBxmlInfo);
			pstDes->pstAribBxmlInfo = NULL;
		}
		if(pstDes->pstAribCaptionInfo != NULL)
		{
			if(pstDes->pstAribCaptionInfo->pucLanguageTag != NULL)
			{
				HLIB_STD_MemFree(pstDes->pstAribCaptionInfo->pucLanguageTag);
				pstDes->pstAribCaptionInfo->pucLanguageTag = NULL;
			}
			if(pstDes->pstAribCaptionInfo->pucDMF != NULL)
			{
				HLIB_STD_MemFree(pstDes->pstAribCaptionInfo->pucDMF);
				pstDes->pstAribCaptionInfo->pucDMF = NULL;
			}
			if(pstDes->pstAribCaptionInfo->punISO639LanguageCode != NULL)
			{
				HLIB_STD_MemFree(pstDes->pstAribCaptionInfo->punISO639LanguageCode);
				pstDes->pstAribCaptionInfo->punISO639LanguageCode = NULL;
			}
			HLIB_STD_MemFree (pstDes->pstAribCaptionInfo);
			pstDes->pstAribCaptionInfo = NULL;
		}

		pstTemp = pstDes->pstNext;
		HLIB_STD_MemFree(pstDes);
		pstDes = pstTemp;
	}
}

HUINT8*  OPL_PARSER_JAPAN_AllocDataContentsDes(void)
{
	APKS_EventDataContentDes_t	*pstDes;

	pstDes = (APKS_EventDataContentDes_t *)HLIB_STD_MemAlloc(sizeof(APKS_EventDataContentDes_t));
	ARIB_DES_CHECK_MEM(pstDes);
	if(pstDes != NULL)
	{
		memset(pstDes, 0, sizeof(APKS_EventDataContentDes_t));
	}

	return (HUINT8 *)pstDes;
}

HERROR  OPL_PARSER_JAPAN_ParseDataContentsDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8				 	ucDesTag, ucDesLen;
	APKS_EventDataContentDes_t			*pstDes = NULL;
	HUINT32					i;
	HUINT8					*pucTemp = NULL;
	AribBxmlInfo				*pstBxmlInfo = NULL;
	AribCarouselInfo			*pstCarouselInfo = NULL;
	AribCarouselComponent	*pstCarouselComponent = NULL;
	AribStoredCarouselInfo		*pstStoredCarouselInfo = NULL;
	AribStoredCarouselComponent *pstStoredCarouselComponent = NULL;
	AribCaptionInfo			*pstCaptionInfo = NULL;
	HUINT8					szIso639LangCode[4], *pszText = NULL;
	HUINT32					nTempTextLen;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pstDes = (APKS_EventDataContentDes_t *)pucDes;

	ucDesTag = pucRaw[0];
	ucDesLen = pucRaw[1];

	if((ucDesTag != TAG_DATA_CONTENTS) || (ucDesLen == 0))
	{
		return ERR_FAIL;
	}

	pstDes->ucDesTag = ucDesTag;
	pstDes->usDataComponentId = HxMACRO_Get16Bit(pucRaw + 2);
	pstDes->ucEntryComponent = pucRaw[4];
	pstDes->ucSelectorLen = pucRaw[5];
	pucRaw += 6;

	pstDes->pstAribBxmlInfo = NULL;
	pucTemp = pucRaw;
	if((pstDes->usDataComponentId == 0x0007) || (pstDes->usDataComponentId == 0x000C))
	{
		pstDes->pstAribBxmlInfo = (AribBxmlInfo *)HLIB_STD_MemAlloc(sizeof(AribBxmlInfo));
		ARIB_DES_CHECK_MEM(pstDes->pstAribBxmlInfo);
		memset(pstDes->pstAribBxmlInfo, 0, sizeof(AribBxmlInfo));
		pstBxmlInfo = pstDes->pstAribBxmlInfo;

		pstBxmlInfo->ucTransmissionFormat		= (*pucTemp >> 6) & 0x03;		/* 2 bits */
		pstBxmlInfo->ucAutoStartFlag 			= (*pucTemp >> 4) & 0x01;		/* 1 bit */
		pstBxmlInfo->ucDocumentResolution 		= (*pucTemp++) & 0x0F;
		pstBxmlInfo->ucUseXml 				= (*pucTemp >> 7) & 0x01;
		pstBxmlInfo->ucDefaultVersionFlag 		= (*pucTemp >> 6) & 0x01;
		pstBxmlInfo->ucIndependentFlag 		= (*pucTemp >> 5) & 0x01;
		pstBxmlInfo->ucContentIdFlag 			= (*pucTemp >> 4) & 0x01;
		pstBxmlInfo->ucAssociatedContentsFlag 	= (*pucTemp >> 3) & 0x01;
		pstBxmlInfo->ucStyleForTvFlag 			= (*pucTemp >> 1) & 0x01;
		pstBxmlInfo->ucUpdateFlag 				= (*pucTemp++) & 0x01;
		pstBxmlInfo->unISO639LanguageCode 	= HxMACRO_Get24Bit(pucTemp);
		pucTemp	+= 3;
		if ( pstBxmlInfo->ucContentIdFlag == 1)
		{
			pstBxmlInfo->unContentId = HxMACRO_Get32Bit(pucTemp);
			pucTemp		+= 4;
			pstBxmlInfo->usContentVersion = HxMACRO_Get16Bit(pucTemp);
			pucTemp		+= 2;
		}
		if ( pstBxmlInfo->ucDefaultVersionFlag == 0)
		{
			pstBxmlInfo->usBmlMajorVersion = HxMACRO_Get16Bit(pucTemp);
			pucTemp		+= 2;
			pstBxmlInfo->usBmlMinorVersion = HxMACRO_Get16Bit(pucTemp);
			pucTemp		+= 2;
			if ( pstBxmlInfo->ucUseXml == 1)
			{
				pstBxmlInfo->usBxmlMajorVersion = HxMACRO_Get16Bit(pucTemp);
				pucTemp		+= 2;
				pstBxmlInfo->usBxmlMinorVersion = HxMACRO_Get16Bit(pucTemp);
				pucTemp		+= 2;
			}
		}
		if(pstBxmlInfo->ucTransmissionFormat == 0)
		{
			pstBxmlInfo->pstCarouselInfo = (AribCarouselInfo *)HLIB_STD_MemAlloc(sizeof(AribCarouselInfo));
			ARIB_DES_CHECK_MEM(pstBxmlInfo->pstCarouselInfo);
			memset(pstBxmlInfo->pstCarouselInfo, 0, sizeof(AribCarouselInfo));
			pstCarouselInfo = pstBxmlInfo->pstCarouselInfo;
			pstCarouselInfo->unNumOfCarousels = *pucTemp++;
			if(pstCarouselInfo->unNumOfCarousels > 0)
			{
				pstCarouselInfo->pstCarouselComponent = (AribCarouselComponent *)HLIB_STD_MemAlloc(sizeof(AribCarouselComponent) * pstCarouselInfo->unNumOfCarousels);
				ARIB_DES_CHECK_MEM(pstCarouselInfo->pstCarouselComponent);
				memset(pstCarouselInfo->pstCarouselComponent, 0, (sizeof(AribCarouselComponent) * pstCarouselInfo->unNumOfCarousels));

				for(i=0;i<pstCarouselInfo->unNumOfCarousels;i++)
				{
					pstCarouselComponent = &pstCarouselInfo->pstCarouselComponent[i];
					pstCarouselComponent->ucComponentTag = pucTemp[0];
					pstCarouselComponent->ucEventSectionFlag = (pucTemp[1] >> 7) & 0x01;
					pstCarouselComponent->ucComponentSizeFlag = (pucTemp[1] >> 3) & 0x01;
					pstCarouselComponent->ucDefaultTransactionIdFlag = (pucTemp[1] >> 2) & 0x01;
					pstCarouselComponent->ucDefaultTimeoutDIIFlag = (pucTemp[1] >> 1) & 0x01;
					pstCarouselComponent->ucDefaultLeakRateFlag = pucTemp[1] & 0x01;
					pucTemp += 2;
					if(pstCarouselComponent->ucComponentSizeFlag == 1)
					{
						pstCarouselComponent->unComponentSize = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstCarouselComponent->ucDefaultTransactionIdFlag == 1)
					{
						pstCarouselComponent->unTimeoutValueDII = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstCarouselComponent->ucDefaultLeakRateFlag == 1)
					{
						pstCarouselComponent->unLeakRate = (pucTemp[0] << 14) | (pucTemp[1] << 6) | ((pucTemp[2] >> 2) & 0x3F);
						pucTemp += 3;
					}
				}
			}
			pstBxmlInfo->ucOndemandRetrievalFlag = (pucTemp[0] >> 7) & 0x01;
			pstBxmlInfo->ucFileStorableFlag = (pucTemp[0] >> 6) & 0x01;
			pucTemp += 1;
		}
		else if(pstBxmlInfo->ucTransmissionFormat == 1)
		{
			pstBxmlInfo->pstStoredCarouselInfo = (AribStoredCarouselInfo *)HLIB_STD_MemAlloc(sizeof(AribStoredCarouselInfo));
			ARIB_DES_CHECK_MEM(pstBxmlInfo->pstStoredCarouselInfo);
			memset(pstBxmlInfo->pstStoredCarouselInfo, 0, (sizeof(AribStoredCarouselInfo)));
			pstStoredCarouselInfo = pstBxmlInfo->pstStoredCarouselInfo;

			pstStoredCarouselInfo->unNumOfCarousels = *pucTemp++;
			if(pstStoredCarouselInfo->unNumOfCarousels > 0)
			{
				pstStoredCarouselInfo->pstStoredCarouselComponent = (AribStoredCarouselComponent *)HLIB_STD_MemAlloc(sizeof(AribStoredCarouselComponent) * pstStoredCarouselInfo->unNumOfCarousels);
				ARIB_DES_CHECK_MEM(pstStoredCarouselInfo->pstStoredCarouselComponent);
				memset(pstStoredCarouselInfo->pstStoredCarouselComponent, 0, (sizeof(AribStoredCarouselComponent) * pstStoredCarouselInfo->unNumOfCarousels));

				for(i=0;i<pstStoredCarouselInfo->unNumOfCarousels;i++)
				{
					pstStoredCarouselComponent = &pstStoredCarouselInfo->pstStoredCarouselComponent[i];
					pstStoredCarouselComponent->ucComponentTag = *pucTemp++;
					pstStoredCarouselComponent->ucNumDataEventFlag = (*pucTemp >> 7) & 0x01;
					pstStoredCarouselComponent->ucNumModulesFlag = (*pucTemp >> 6) & 0x01;
					pstStoredCarouselComponent->ucNumResourcesFlag = (*pucTemp >> 5) & 0x01;
					pstStoredCarouselComponent->ucCompressedComponentSizeFlag = (*pucTemp >> 4) & 0x01;
					pstStoredCarouselComponent->ucComponentSizeFlag = (*pucTemp >> 3) & 0x01;
					pstStoredCarouselComponent->ucDefaultTransactionIdFlag = (*pucTemp >> 2) & 0x01;
					pstStoredCarouselComponent->ucDefaultTimeoutDIIFlag = (*pucTemp >> 1) & 0x01;
					pstStoredCarouselComponent->ucDefaultLeakRateFlag = *pucTemp++ & 0x01;
					if(pstStoredCarouselComponent->ucNumDataEventFlag == 1)
					{
						pstStoredCarouselComponent->usNumDataEvent = HxMACRO_Get16Bit(pucTemp);
						pucTemp += 2;
					}
					if(pstStoredCarouselComponent->ucNumModulesFlag == 1)
					{
						pstStoredCarouselComponent->unNumModules = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucNumResourcesFlag == 1)
					{
						pstStoredCarouselComponent->unNumResoures = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucCompressedComponentSizeFlag == 1)
					{
 						pstStoredCarouselComponent->unCompressedComponentSize = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucComponentSizeFlag == 1)
					{
						pstStoredCarouselComponent->unComponentSize = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucDefaultTransactionIdFlag == 1)
					{
						pstStoredCarouselComponent->unTransactionId = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucDefaultTimeoutDIIFlag == 1)
					{
						pstStoredCarouselComponent->unTimeoutValueDII = HxMACRO_Get32Bit(pucTemp);
						pucTemp += 4;
					}
					if(pstStoredCarouselComponent->ucDefaultLeakRateFlag == 1)
					{
						pstStoredCarouselComponent->unLeakRate = (pucTemp[0] << 14) | (pucTemp[1] << 6) | ((pucTemp[2] >> 2) & 0x3F);
						pucTemp += 3;
					}
				}
			}
		}
	}
	else if(pstDes->usDataComponentId == 0x0008)
	{
		pstDes->pstAribCaptionInfo = (AribCaptionInfo *)HLIB_STD_MemAlloc(sizeof(AribCaptionInfo));
		ARIB_DES_CHECK_MEM(pstDes->pstAribCaptionInfo);
		memset(pstDes->pstAribCaptionInfo, 0, (sizeof(AribCaptionInfo)));

		pstCaptionInfo = pstDes->pstAribCaptionInfo;

		pstCaptionInfo->ucNumLanguages = *pucTemp++;
		if(pstCaptionInfo->ucNumLanguages > 0)
		{
			pstCaptionInfo->pucLanguageTag = (HUINT8 *)HLIB_STD_MemAlloc((sizeof(HUINT8) * pstCaptionInfo->ucNumLanguages));
			ARIB_DES_CHECK_MEM(pstCaptionInfo->pucLanguageTag);
			memset(pstCaptionInfo->pucLanguageTag, 0, (sizeof(HUINT8) * pstCaptionInfo->ucNumLanguages));
			pstCaptionInfo->pucDMF = (HUINT8 *)HLIB_STD_MemAlloc((sizeof(HUINT8) * pstCaptionInfo->ucNumLanguages));
			ARIB_DES_CHECK_MEM(pstCaptionInfo->pucDMF);
			memset(pstCaptionInfo->pucDMF, 0, (sizeof(HUINT8) * pstCaptionInfo->ucNumLanguages));
			pstCaptionInfo->punISO639LanguageCode = (HUINT32 *)HLIB_STD_MemAlloc((sizeof(HUINT32) * pstCaptionInfo->ucNumLanguages));
			ARIB_DES_CHECK_MEM(pstCaptionInfo->punISO639LanguageCode);
			memset(pstCaptionInfo->punISO639LanguageCode, 0, (sizeof(HUINT32) * pstCaptionInfo->ucNumLanguages));

			for(i=0;i<pstCaptionInfo->ucNumLanguages;i++)
			{
				pstCaptionInfo->pucLanguageTag[i]		= (*pucTemp >> 5) & 0x07;
				pstCaptionInfo->pucDMF[i]				= (*pucTemp++) & 0x0F;
				pstCaptionInfo->punISO639LanguageCode[i] =  HxMACRO_Get24Bit(pucTemp);
				pucTemp += 3;
			}
		}
	}

	pucRaw += pstDes->ucSelectorLen;
	pstDes->ucNumOfComponentRef = *pucRaw++;
	if(pstDes->ucNumOfComponentRef > 0)
	{
		pstDes->pucComponentRef = (HUINT8 *)HLIB_STD_MemAlloc(sizeof(HUINT8) * pstDes->ucNumOfComponentRef);
		ARIB_DES_CHECK_MEM(pstDes->pucComponentRef);
		memset(pstDes->pucComponentRef, 0, (sizeof(HUINT8) * pstDes->ucNumOfComponentRef));
		for(i=0;i<pstDes->ucNumOfComponentRef;i++)
		{
			pstDes->pucComponentRef[i] = *pucRaw++;
		}
	}

	pstDes->unISO639LanguageCode = HxMACRO_Get24Bit(pucRaw);
	memcpy (szIso639LangCode, pucRaw, 3);
	szIso639LangCode [3] = '\0';
	pucRaw += 3;
	pstDes->ucTextLen = *pucRaw++;

	if(pstDes->ucTextLen > 0)
	{
		pszText = (HUINT8 *)SILIB_TEXT_MakeUtf8Ex(eSIxTEXT_ENC_JIS, (const HCHAR *)szIso639LangCode, pucRaw, (HUINT32)pstDes->ucTextLen, NULL, NULL);
		if(pszText != NULL)
		{
			nTempTextLen = HxSTD_StrLen((HCHAR *)pszText);
			pstDes->pucTextChar = (HUINT8 *)HLIB_STD_MemAlloc(sizeof(HUINT8) * (nTempTextLen + 1));
			if(pstDes->pucTextChar != NULL)
			{
				memset(pstDes->pucTextChar, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
				memcpy(pstDes->pucTextChar, pszText, HxSTD_StrLen((HCHAR *)pszText));
				pstDes->pucTextChar[HxSTD_StrLen((HCHAR *)pszText)] = ARIB_DES_EOS;
			}
			HLIB_STD_MemFree(pszText);
		}
	}

	return ERR_OK;
}

void  OPL_PARSER_JAPAN_PrintDataContentsDes(HUINT8 *pucDes)
{
	APKS_EventDataContentDes_t			*pstDes;
	HINT32							i = 0;

	if (pucDes == NULL)					{ return; }

	pstDes = (APKS_EventDataContentDes_t *)pucDes;

	HxLOG_Print( "\n\n##### DATA CONTENTS DESCRIPTOR #####\n\n");

	do
	{
		HxLOG_Print("ucDesTag : 0x%x\n" , pstDes->ucDesTag );
		HxLOG_Print("usDataComponentId : %d\n" , pstDes->usDataComponentId );
		HxLOG_Print("ucEntryComponent : 0x%x\n" , pstDes->ucEntryComponent );
		HxLOG_Print("ucSelectorLen : 0x%x\n" , pstDes->ucSelectorLen );
		HxLOG_Print("pstAribBxmlInfo\n" );
		if( pstDes->pstAribBxmlInfo )
		{
			HxLOG_Print("	ucTransmissionFormat : 0x%x\n" , pstDes->pstAribBxmlInfo->ucTransmissionFormat );
			HxLOG_Print("	ucAutoStartFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucAutoStartFlag );
			HxLOG_Print("	ucDocumentResolution : 0x%x\n" , pstDes->pstAribBxmlInfo->ucDocumentResolution );
			HxLOG_Print("	ucUseXml : 0x%x\n" , pstDes->pstAribBxmlInfo->ucUseXml );
			HxLOG_Print("	ucDefaultVersionFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucDefaultVersionFlag );
			HxLOG_Print("	ucIndependentFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucIndependentFlag );
			HxLOG_Print("	ucContentIdFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucContentIdFlag );
			HxLOG_Print("	ucAssociatedContentsFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucAssociatedContentsFlag );
			HxLOG_Print("	ucStyleForTvFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucStyleForTvFlag );
			HxLOG_Print("	ucUpdateFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucUpdateFlag );
			HxLOG_Print("	unISO639LanguageCode : %dx\n" , pstDes->pstAribBxmlInfo->unISO639LanguageCode );
			HxLOG_Print("	unContentId : %d\n" , pstDes->pstAribBxmlInfo->unContentId );
			HxLOG_Print("	usContentVersion : %d\n" , pstDes->pstAribBxmlInfo->usContentVersion );
			HxLOG_Print("	usBmlMajorVersion : %d\n" , pstDes->pstAribBxmlInfo->usBmlMajorVersion );
			HxLOG_Print("	usBmlMinorVersion : %d\n" , pstDes->pstAribBxmlInfo->usBmlMinorVersion );
			HxLOG_Print("	usBxmlMajorVersion : %d\n" , pstDes->pstAribBxmlInfo->usBxmlMajorVersion );
			HxLOG_Print("	usBxmlMinorVersion : %d\n" , pstDes->pstAribBxmlInfo->usBxmlMinorVersion );
			HxLOG_Print("	pstCarouselInfo\n");
			if( pstDes->pstAribBxmlInfo->pstCarouselInfo )
			{
				HxLOG_Print("		usBxmlMinorVersion : %d\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->unNumOfCarousels );
				HxLOG_Print("		pstCarouselComponent\n");
				if( pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent )
				{
					HxLOG_Print("			ucComponentTag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->ucComponentTag );
					HxLOG_Print("			ucEventSectionFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->ucEventSectionFlag );
					HxLOG_Print("			ucComponentSizeFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->ucComponentSizeFlag );
					HxLOG_Print("			ucDefaultTransactionIdFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->ucDefaultTransactionIdFlag );
					HxLOG_Print("			ucDefaultTimeoutDIIFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->ucDefaultTimeoutDIIFlag );
					HxLOG_Print("			ucDefaultLeakRateFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->ucDefaultLeakRateFlag );
					HxLOG_Print("			unComponentSize : %d\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->unComponentSize );
					HxLOG_Print("			unTransactionId : %d\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->unTransactionId );
					HxLOG_Print("			unTimeoutValueDII : %d\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->unTimeoutValueDII );
					HxLOG_Print("			unLeakRate : %d\n" , pstDes->pstAribBxmlInfo->pstCarouselInfo->pstCarouselComponent->unLeakRate );
				}
			}
			HxLOG_Print("	ucOndemandRetrievalFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucOndemandRetrievalFlag );
			HxLOG_Print("	ucFileStorableFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->ucFileStorableFlag );
			HxLOG_Print("	pstStoredCarouselInfo\n");
			if( pstDes->pstAribBxmlInfo->pstStoredCarouselInfo )
			{
				HxLOG_Print("		unNumOfCarousels : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->unNumOfCarousels );
				HxLOG_Print("		pstStoredCarouselComponent\n");
				if( pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent )
				{
					HxLOG_Print("			ucComponentTag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucComponentTag );
					HxLOG_Print("			ucNumDataEventFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucNumDataEventFlag );
					HxLOG_Print("			ucNumModulesFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucNumModulesFlag );
					HxLOG_Print("			ucNumResourcesFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucNumResourcesFlag );
					HxLOG_Print("			ucCompressedComponentSizeFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucCompressedComponentSizeFlag );
					HxLOG_Print("			ucComponentSizeFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucComponentSizeFlag );
					HxLOG_Print("			ucDefaultTransactionIdFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucDefaultTransactionIdFlag );
					HxLOG_Print("			ucDefaultTimeoutDIIFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucDefaultTimeoutDIIFlag );
					HxLOG_Print("			ucDefaultLeakRateFlag : 0x%x\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->ucDefaultLeakRateFlag );
					HxLOG_Print("			usNumDataEvent : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->usNumDataEvent );
					HxLOG_Print("			unNumModules : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->unNumModules );
					HxLOG_Print("			unNumResoures : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->unNumResoures );
					HxLOG_Print("			unCompressedComponentSize : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->unCompressedComponentSize );
					HxLOG_Print("			unComponentSize : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->unComponentSize );
					HxLOG_Print("			unTransactionId : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->unTransactionId );
					HxLOG_Print("			unTimeoutValueDII : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->unTimeoutValueDII );
					HxLOG_Print("			unLeakRate : %d\n" , pstDes->pstAribBxmlInfo->pstStoredCarouselInfo->pstStoredCarouselComponent->unLeakRate );
				}
			}

		}
		HxLOG_Print("pstAribCaptionInfo\n" );
		if( pstDes->pstAribCaptionInfo )
		{
			HxLOG_Print("	ucNumLanguages : 0x%x\n" , pstDes->pstAribCaptionInfo->ucNumLanguages );
			for( i =0; i< pstDes->pstAribCaptionInfo->ucNumLanguages ; i++ )
			{
				HxLOG_Print("	[%d]pucLanguageTag : 0x%x\n" , i, pstDes->pstAribCaptionInfo->pucLanguageTag[i] );
				HxLOG_Print("	[%d]pucDMF : 0x%x\n" , i, pstDes->pstAribCaptionInfo->pucDMF[i] );
				HxLOG_Print("	[%d]punISO639LanguageCode : %d\n" , i, pstDes->pstAribCaptionInfo->punISO639LanguageCode[i] );
			}
		}
		HxLOG_Print("ucNumOfComponentRef : 0x%x\n" , pstDes->ucNumOfComponentRef );
		HxLOG_Print("pucComponentRef : ");
		if( pstDes->pucComponentRef == NULL )
		{
			HxLOG_Print( "\n" );
		}
		else
		{
			HxLOG_Print ( "%s\n" , pstDes->pucComponentRef );
		}
		HxLOG_Print("unISO639LanguageCode : %d\n" , pstDes->unISO639LanguageCode );
		HxLOG_Print("ucTextLen : %d\n" , pstDes->ucTextLen );
		HxLOG_Print("pucTextChar : ");
		if( pstDes->pucTextChar == NULL )
		{
			HxLOG_Print( "\n" );
		}
		else
		{
			HxLOG_Print ( "%s\n" , pstDes->pucTextChar );
		}
		pstDes = pstDes->pstNext;
	}while( pstDes );

	HxLOG_Print( "\n\n##### ##### END ##### #####\n\n");
}

#define ______0xD6_EVENT_GROUP_DESCRIPTOR______

void  OPL_PARSER_JAPAN_FreeEventGroupDes(void *pucDesc)
{
	APKS_EventGroupDes_t 	*pstDes = (APKS_EventGroupDes_t*)pucDesc;
	APKS_EventGroupDes_t	*pstTemp = NULL;

	if(pstDes == NULL)
	{
		return;
	}

	if(pstDes->ucDesTag != TAG_EVENT_GROUP)
	{
		HxLOG_Error("ucDesTag(0x%X) invalid(%s, %d)\n", pstDes->ucDesTag, __FUNCTION__, __LINE__);
		return;
	}

	while(pstDes != NULL)
	{
		if(pstDes->pusServiceId != NULL)
		{
			HLIB_STD_MemFree(pstDes->pusServiceId);
			pstDes->pusServiceId = NULL;
		}
		if(pstDes->pusEventId != NULL)
		{
			HLIB_STD_MemFree(pstDes->pusEventId);
			pstDes->pusEventId = NULL;
		}
		if(pstDes->pstOtherNetworkInfo != NULL)
		{
			HLIB_STD_MemFree(pstDes->pstOtherNetworkInfo);
			pstDes->pstOtherNetworkInfo = NULL;
		}
		if(pstDes->pucDataByte != NULL)
		{
			HLIB_STD_MemFree(pstDes->pucDataByte);
			pstDes->pucDataByte = NULL;
		}
		pstTemp = pstDes->pstNext;
		HLIB_STD_MemFree (pstDes);
		pstDes = pstTemp;
	}
}

HUINT8*  OPL_PARSER_JAPAN_AllocEventGroupDes(void)
{
	APKS_EventGroupDes_t	*pstDes;

	pstDes = (APKS_EventGroupDes_t*)HLIB_STD_MemAlloc(sizeof(APKS_EventGroupDes_t));
	ARIB_DES_CHECK_MEM(pstDes);
	if(pstDes != NULL)
	{
		memset(pstDes, 0, sizeof(APKS_EventGroupDes_t));
	}

	return (HUINT8 *)pstDes;
}

HERROR  OPL_PARSER_JAPAN_ParseEventGroupDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8			 ucDesTag, ucDesLen;
	APKS_EventGroupDes_t	*pstDes;
	HINT32			i;
	HUINT8			*pTemp;
	HUINT8			ucTempLen;
	AribEventGroupOtherNetworkInfo *pstOtherNetInfo;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pstDes = (APKS_EventGroupDes_t*)pucDes;

	ucDesTag = pucRaw[0];
	ucDesLen = pucRaw[1];

	if((ucDesTag != TAG_EVENT_GROUP) || (ucDesLen == 0))
	{
		return ERR_FAIL;
	}

	pstDes->ucDesTag = ucDesTag;
	pstDes->ucGroupType = (pucRaw[2] & 0xF0) >> 4;
	pstDes->ucEventCount = (pucRaw[2] & 0x0F);
	if(pstDes->ucEventCount > 0)
	{
		pstDes->pusServiceId = (HUINT16*)HLIB_STD_MemAlloc(sizeof(HUINT16) * pstDes->ucEventCount);
		ARIB_DES_CHECK_MEM(pstDes->pusServiceId);
		memset(pstDes->pusServiceId, 0, (sizeof(HUINT16) * pstDes->ucEventCount));
		pstDes->pusEventId = (HINT32*)HLIB_STD_MemAlloc(sizeof(HINT32) * pstDes->ucEventCount);
		ARIB_DES_CHECK_MEM(pstDes->pusEventId);
		memset(pstDes->pusEventId, 0, (sizeof(HINT32) * pstDes->ucEventCount));

		pTemp = (pucRaw + 3);
		for(i=0;i<pstDes->ucEventCount;i++)
		{
			pstDes->pusServiceId[i] = HxMACRO_Get16Bit(pTemp);
			pTemp += 2;
			pstDes->pusEventId[i] = HxMACRO_Get16Bit(pTemp);
			pTemp += 2;
		}
	}

	ucTempLen = ucDesLen - 1 - (pstDes->ucEventCount * 4);
	if(ucTempLen > 0)
	{
		if((pstDes->ucGroupType == 0x4) || (pstDes->ucGroupType == 0x05))
		{
			pstDes->ucNumLoop = ucTempLen / 8;
			if((ucTempLen % sizeof(AribEventGroupOtherNetworkInfo)) != 0)
			{
				HxLOG_Error("The rest size is strange(right:%d, actual:%d). %d\n", sizeof(AribEventGroupOtherNetworkInfo), ucTempLen, __LINE__);
			}

			if(pstDes->ucNumLoop > 0)
			{
				pstDes->pstOtherNetworkInfo = (AribEventGroupOtherNetworkInfo*)HLIB_STD_MemAlloc(sizeof(AribEventGroupOtherNetworkInfo)*pstDes->ucNumLoop);
				ARIB_DES_CHECK_MEM(pstDes->pstOtherNetworkInfo);
				memset(pstDes->pstOtherNetworkInfo, 0, (sizeof(AribEventGroupOtherNetworkInfo)*pstDes->ucNumLoop));
				pTemp = (pucRaw + 3 + (pstDes->ucEventCount * 4));
				for(i=0;i<pstDes->ucNumLoop;i++)
				{
					pstOtherNetInfo = &(pstDes->pstOtherNetworkInfo[i]);
					pstOtherNetInfo->usOrigNetworkId = HxMACRO_Get16Bit(pTemp);
					pTemp += 2;
					pstOtherNetInfo->usTransStreamId = HxMACRO_Get16Bit(pTemp);
					pTemp += 2;
					pstOtherNetInfo->usServiceId = HxMACRO_Get16Bit(pTemp);
					pTemp += 2;
					pstOtherNetInfo->nEventId = HxMACRO_Get16Bit(pTemp);
					pTemp += 2;
				}
			}
		}
		else
		{
			pstDes->ucPrivateDataByteLen = ucTempLen;
			pstDes->pstOtherNetworkInfo = (AribEventGroupOtherNetworkInfo*)HLIB_STD_MemAlloc(sizeof(AribEventGroupOtherNetworkInfo)*pstDes->ucNumLoop);
			pTemp = (pucRaw + 3 + (pstDes->ucEventCount * 4));
			for(i=0;i<ucTempLen;i++)
			{
				pstDes->pucDataByte[i] = *pTemp++;
			}
		}
	}
	return ERR_OK;
}

void  OPL_PARSER_JAPAN_PrintEventGroupDes(HUINT8 *pucDes)
{
	APKS_EventGroupDes_t			*pstDes;
	HINT32						i =0;

	if (pucDes == NULL)					{ return; }

	pstDes = (APKS_EventGroupDes_t *)pucDes;

	HxLOG_Print( "\n\n##### EVENT GROUP DESCRIPTOR #####\n\n");

	do
	{
		HxLOG_Print("ucDesTag : 0x%x\n" , pstDes->ucDesTag );
		HxLOG_Print("ucGroupType : 0x%x\n" , pstDes->ucGroupType );
		HxLOG_Print("ucEventCount : 0x%x\n" , pstDes->ucEventCount );
		for( i =0; i<pstDes->ucEventCount; i++ )
		{
			HxLOG_Print("pusServiceId[%d] : %d\n" , i, pstDes->pusServiceId[i] );
			HxLOG_Print("pusEventId[%d] : %dx\n" , i, pstDes->pusEventId[i] );
		}
		HxLOG_Print("ucNumLoop : %d\n" , pstDes->ucNumLoop );
		HxLOG_Print("pstOtherNetworkInfo \n");
		if( pstDes->pstOtherNetworkInfo )
		{
			for( i =0; i< pstDes->ucNumLoop ; i++)
			{
				HxLOG_Print("	[%d]usOrigNetworkId : %d\n" , i, pstDes->pstOtherNetworkInfo[i].usOrigNetworkId );
				HxLOG_Print("	[%d]usTransStreamId : %d\n" , i, pstDes->pstOtherNetworkInfo[i].usTransStreamId );
				HxLOG_Print("	[%d]usServiceId : %d\n" , i, pstDes->pstOtherNetworkInfo[i].usServiceId );
				HxLOG_Print("	[%d]nEventId : %d\n" , i, pstDes->pstOtherNetworkInfo[i].nEventId );
			}
		}
		HxLOG_Print("ucPrivateDataByteLen : %d\n" , pstDes->ucPrivateDataByteLen );
		HxLOG_Print("pucDataByte : ");
		if( pstDes->pucDataByte == NULL )
		{
			HxLOG_Print( "\n" );
		}
		else
		{
			HxLOG_Print ( "%s\n" , pstDes->pucDataByte );
		}
		pstDes = pstDes->pstNext;
	}while( pstDes );

	HxLOG_Print( "\n\n##### ##### END ##### #####\n\n");
}

#define ______0xD9_COMPONENT_GROUP_DESCRIPTOR______

void  OPL_PARSER_JAPAN_FreeEventComponentGroupDes(void *pucDesc)
{
	APKS_EventComponentGroupDes_t 	*pstDes = (APKS_EventComponentGroupDes_t *)pucDesc;
	HINT32	i, j;

	if(pstDes == NULL)
	{
		return;
	}

	if(pstDes->ucDesTag != TAG_COMPONENT_GROUP)
	{
		HxLOG_Error("ucDesTag(0x%X) invalid(%s, %d)\n", pstDes->ucDesTag, __FUNCTION__, __LINE__);
		return;
	}

	if(pstDes != NULL)
	{
		if(pstDes->pstGroup != NULL)
		{
			for(i=0;i<pstDes->ucNumOfGroup;i++)
			{
				if(pstDes->pstGroup[i].pstCaUnit != NULL)
				{
					for(j=0;j<pstDes->pstGroup[i].ucNumOfCaUnit;j++)
					{
						if(pstDes->pstGroup[i].pstCaUnit[j].pucComponentTag != NULL)
						{
							HLIB_STD_MemFree(pstDes->pstGroup[i].pstCaUnit[j].pucComponentTag);
							pstDes->pstGroup[i].pstCaUnit[j].pucComponentTag = NULL;
						}
					}
					HLIB_STD_MemFree(pstDes->pstGroup[i].pstCaUnit);
					pstDes->pstGroup[i].pstCaUnit = NULL;
				}
			}
			HLIB_STD_MemFree(pstDes->pstGroup);
			pstDes->pstGroup = NULL;
		}
		HLIB_STD_MemFree (pstDes);
		pstDes = NULL;
	}
}

HUINT8*  OPL_PARSER_JAPAN_AllocEventComponentGroupDes(void)
{
	APKS_EventComponentGroupDes_t	*pstDes;

	pstDes = (APKS_EventComponentGroupDes_t *)HLIB_STD_MemAlloc(sizeof(APKS_EventComponentGroupDes_t));
	ARIB_DES_CHECK_MEM(pstDes);
	if(pstDes != NULL)
	{
		memset(pstDes, 0, sizeof(APKS_EventComponentGroupDes_t));
	}

	return (HUINT8 *)pstDes;
}

HERROR  OPL_PARSER_JAPAN_ParseEventComponentGroupDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8			 ucDesTag, ucDesLen;
	APKS_EventComponentGroupDes_t	*pstDes;
	HINT32			i, j, k;
	AribComponentGroupDesGroup *pstGroup;
	AribComponentGroupDesCaUnit	*pstCaUnit;
	HUINT8			*pszText = NULL;
	HUINT32			nTempTextLen;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pstDes = (APKS_EventComponentGroupDes_t *)pucDes;

	ucDesTag = pucRaw[0];
	ucDesLen = pucRaw[1];

	if((ucDesTag != TAG_COMPONENT_GROUP) || (ucDesLen == 0))
	{
		return ERR_FAIL;
	}

	pstDes->ucDesTag = ucDesTag;
	pstDes->ucComponentGroupType = (pucRaw[2] & 0xE0) >> 5;
	pstDes->ucTotalBitRateFlag = (pucRaw[2]& 0x10) >> 4;
	pstDes->ucNumOfGroup = pucRaw[2]&0x0F;
	if(pstDes->ucNumOfGroup > 0)
	{
		pstDes->pstGroup = (AribComponentGroupDesGroup *)HLIB_STD_MemAlloc(sizeof(AribComponentGroupDesGroup) * pstDes->ucNumOfGroup);
		ARIB_DES_CHECK_MEM(pstDes->pstGroup);
		memset(pstDes->pstGroup, 0, (sizeof(AribComponentGroupDesGroup) * pstDes->ucNumOfGroup));
		pucRaw += 3;
		for(i=0;i<pstDes->ucNumOfGroup;i++)
		{
			pstGroup = &pstDes->pstGroup[i];
			pstGroup->ucComponentGroupId = (*pucRaw >> 4) & 0x0F;
			pstGroup->ucNumOfCaUnit = *pucRaw & 0x0F;
			pucRaw++;
			if(pstGroup->ucNumOfCaUnit > 0)
			{
				pstGroup->pstCaUnit = (AribComponentGroupDesCaUnit *)HLIB_STD_MemAlloc(sizeof(AribComponentGroupDesCaUnit) * pstGroup->ucNumOfCaUnit);
				ARIB_DES_CHECK_MEM(pstGroup->pstCaUnit);
				memset(pstGroup->pstCaUnit, 0, (sizeof(AribComponentGroupDesCaUnit) * pstGroup->ucNumOfCaUnit));
				for(j=0;j<pstGroup->ucNumOfCaUnit;j++)
				{
					pstCaUnit = &pstGroup->pstCaUnit[j];
					pstCaUnit->ucCaUnitId = (*pucRaw >> 4) & 0x0F;
					pstCaUnit->ucNumOfComponent = *pucRaw & 0x0F;
					pucRaw++;
					if(pstCaUnit->ucNumOfComponent > 0)
					{
						pstCaUnit->pucComponentTag = (HUINT8 *)HLIB_STD_MemAlloc(sizeof(HUINT8) * pstCaUnit->ucNumOfComponent);
						ARIB_DES_CHECK_MEM(pstCaUnit->pucComponentTag);
						memset(pstCaUnit->pucComponentTag, 0, (sizeof(HUINT8) * pstCaUnit->ucNumOfComponent));
						for(k=0;k<pstCaUnit->ucNumOfComponent;k++)
						{
							pstCaUnit->pucComponentTag[k] = *pucRaw++;
						}
					}
				}
			}
			if(pstDes->ucTotalBitRateFlag == 0x01)
			{
				pstGroup->ucTotalBitRate = *pucRaw++;
			}
			pstGroup->ucTextLen = *pucRaw++;

			pszText = (HUINT8 *)SILIB_TEXT_MakeUtf8Ex(eSIxTEXT_ENC_JIS, NULL, pucRaw, (HUINT32)pstGroup->ucTextLen, NULL, NULL);
			if(pszText != NULL)
			{
				nTempTextLen = HxSTD_StrLen((HCHAR *)pszText);
				pstGroup->pucTextChar = HLIB_STD_MemAlloc(sizeof(HUINT8) * (nTempTextLen+1));
				if(pstGroup->pucTextChar != NULL)
				{
					memset(pstGroup->pucTextChar, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
					memcpy(pstGroup->pucTextChar, pszText, HxSTD_StrLen((HCHAR *)pszText));
					pstGroup->pucTextChar[HxSTD_StrLen((HCHAR *)pszText)] = ARIB_DES_EOS;
				}

				HLIB_STD_MemFree(pszText);
			}
		}
	}

	return ERR_OK;
}

void  OPL_PARSER_JAPAN_PrintEventComponentGroupDes(HUINT8 *pucDes)
{
	APKS_EventComponentGroupDes_t			*pstDes;

	if (pucDes == NULL)					{ return; }

	pstDes = (APKS_EventComponentGroupDes_t *)pucDes;

	HxLOG_Print( "\n\n##### EVENT COMPONENT GROUP DESCRIPTOR #####\n\n");

	HxLOG_Print("ucDesTag : 0x%x\n" , pstDes->ucDesTag );
	HxLOG_Print("ucComponentGroupType : 0x%x\n" , pstDes->ucComponentGroupType );
	HxLOG_Print("ucTotalBitRateFlag : 0x%x\n" , pstDes->ucTotalBitRateFlag );
	HxLOG_Print("ucNumOfGroup : 0x%x\n" , pstDes->ucNumOfGroup );
	HxLOG_Print("pstGroup : ");
	if( pstDes->pstGroup )
	{
		HxLOG_Print("	ucComponentGroupId : 0x%x\n" , pstDes->pstGroup->ucComponentGroupId );
		HxLOG_Print("	ucNumOfCaUnit : 0x%x\n" , pstDes->pstGroup->ucNumOfCaUnit );
		HxLOG_Print("	pstCaUnit : ");
		if( pstDes->pstGroup->pstCaUnit )
		{
			HxLOG_Print("		ucCaUnitId : 0x%x\n" , pstDes->pstGroup->pstCaUnit->ucCaUnitId );
			HxLOG_Print("		ucNumOfComponent : %d\n" , pstDes->pstGroup->pstCaUnit->ucNumOfComponent );
			HxLOG_Print("		pucComponentTag : ");
			if( pstDes->pstGroup->pstCaUnit->pucComponentTag == NULL )
			{
				HxLOG_Print( "\n" );
			}
			else
			{
				HxLOG_Print ( "%s\n" , pstDes->pstGroup->pstCaUnit->pucComponentTag );
			}
		}
		HxLOG_Print("	ucTotalBitRate : 0x%x\n" , pstDes->pstGroup->ucTotalBitRate );
		HxLOG_Print("	ucTextLen : %d\n" , pstDes->pstGroup->ucTextLen );
		HxLOG_Print("	pucTextChar : ");
		if( pstDes->pstGroup->pucTextChar )
		{
			HxLOG_Print( "\n" );
		}
		else
		{
			HxLOG_Print ( "%s\n" , pstDes->pstGroup->pucTextChar );
		}
	}

	HxLOG_Print( "\n\n##### ##### END ##### #####\n\n");
}


#define ______0xDE_CONTENT_AVAILABILITY_DESCRIPTOR______

void  OPL_PARSER_JAPAN_FreeEventContentAvailabilityDes(void *pucDesc)
{
	APKS_EventContentAvailabilityDes_t 	*pstDes = (APKS_EventContentAvailabilityDes_t *)pucDesc;

	if(pstDes == NULL)
	{
		return;
	}

	if(pstDes->ucDesTag != TAG_CONTENT_AVAILABILITY)
	{
		HxLOG_Error("ucDesTag(0x%X) invalid(%s, %d)\n", pstDes->ucDesTag, __FUNCTION__, __LINE__);
		return;
	}

	if(pstDes != NULL)
	{
		HLIB_STD_MemFree (pstDes);
		pstDes = NULL;
	}
}

HUINT8*  OPL_PARSER_JAPAN_AllocEventContentAvailabilityDes(void)
{
	APKS_EventContentAvailabilityDes_t	*pstDes;

	pstDes = (APKS_EventContentAvailabilityDes_t *)HLIB_STD_MemAlloc(sizeof(APKS_EventContentAvailabilityDes_t));
	ARIB_DES_CHECK_MEM(pstDes);
	if(pstDes != NULL)
	{
		memset(pstDes, 0, sizeof(APKS_EventContentAvailabilityDes_t));
	}

	return (HUINT8 *)pstDes;
}

HERROR  OPL_PARSER_JAPAN_ParseEventContentAvailabilityDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8			 ucDesTag, ucDesLen;
	APKS_EventContentAvailabilityDes_t	*pstDes;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pstDes = (APKS_EventContentAvailabilityDes_t *)pucDes;

	ucDesTag = pucRaw[0];
	ucDesLen = pucRaw[1];

	if((ucDesTag != TAG_CONTENT_AVAILABILITY) || (ucDesLen == 0))
	{
		return ERR_FAIL;
	}

	pstDes->ucDesTag = ucDesTag;
	pstDes->ucCopyRestrictionMode = (pucRaw[2] & 0x40) >> 6;
	pstDes->ucImageContraintToken = (pucRaw[2] & 0x20) >> 5;
	pstDes->ucRetentionMode = (pucRaw[2] & 0x10) >> 4;
	pstDes->ucRetentionState = (pucRaw[2] & 0x0E) >>1 ;
	pstDes->ucEncryptionMode = (pucRaw[2] & 0x01) ;

	return ERR_OK;
}

void  OPL_PARSER_JAPAN_PrintEventContentAvailabilityDes(HUINT8 *pucDes)
{
	APKS_EventContentAvailabilityDes_t			*pstDes;

	if (pucDes == NULL)					{ return; }

	pstDes = (APKS_EventContentAvailabilityDes_t *)pucDes;

	HxLOG_Print( "\n\n##### EVENT CONTENT AVAILABILITY DESCRIPTOR #####\n\n");

	HxLOG_Print("ucDesTag : 0x%x\n" , pstDes->ucDesTag );
	HxLOG_Print("ucCopyRestrictionMode : 0x%x\n" , pstDes->ucCopyRestrictionMode );
	HxLOG_Print("ucImageContraintToken : 0x%x\n" , pstDes->ucImageContraintToken );
	HxLOG_Print("ucRetentionMode : 0x%x\n" , pstDes->ucRetentionMode );
	HxLOG_Print("ucRetentionState : 0x%x\n" , pstDes->ucRetentionState );
	HxLOG_Print("ucEncryptionMode : 0x%x\n" , pstDes->ucEncryptionMode );

	HxLOG_Print( "\n\n##### ##### END ##### #####\n\n");
}

#define ______0xCB_CA_CONTRACT_INFO_DESCRIPTOR______

void  OPL_PARSER_JAPAN_FreeCaContractInfoDes(void *pucDesc)
{
	APKS_CaContractInfoDes_t 	*pstDes = (APKS_CaContractInfoDes_t*)pucDesc;
	APKS_CaContractInfoDes_t	*pstTemp = NULL;

	if(pstDes == NULL)
	{
		return;
	}

	if(pstDes->ucDesTag != TAG_CA_CONTRACT_INFOMATION)
	{
		HxLOG_Error("ucDesTag(0x%X) invalid(%s, %d)\n", pstDes->ucDesTag, __FUNCTION__, __LINE__);
		return;
	}

	while(pstDes != NULL)
	{
		if(pstDes->pucComponentTag != NULL)
		{
			HLIB_STD_MemFree(pstDes->pucComponentTag);
			pstDes->pucComponentTag = NULL;
		}
		if(pstDes->pucContractVerificationInfo != NULL)
		{
			HLIB_STD_MemFree(pstDes->pucContractVerificationInfo);
			pstDes->pucContractVerificationInfo = NULL;
		}
		if(pstDes->pucFeeName != NULL)
		{
			HLIB_STD_MemFree(pstDes->pucFeeName);
			pstDes->pucFeeName = NULL;
		}

		pstTemp = pstDes->pstNext;
		HLIB_STD_MemFree (pstDes);
		pstDes = pstTemp;
	}
}

HUINT8*  OPL_PARSER_JAPAN_AllocCaContractInfoDes(void)
{
	APKS_CaContractInfoDes_t	*pstDes;

	pstDes = (APKS_CaContractInfoDes_t*)HLIB_STD_MemAlloc(sizeof(APKS_CaContractInfoDes_t));
	ARIB_DES_CHECK_MEM(pstDes);
	if(pstDes != NULL)
	{
		memset(pstDes, 0, sizeof(APKS_CaContractInfoDes_t));
	}

	return (HUINT8 *)pstDes;
}

HERROR  OPL_PARSER_JAPAN_ParseCaContractInfoDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8			 		ucDesTag, ucDesLen;
	APKS_CaContractInfoDes_t	*pstDes;
	HINT32					i;
	HUINT8					*pTemp;
	HUINT8					*pszText = NULL;
	HUINT32					nTempTextLen;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pstDes = (APKS_CaContractInfoDes_t*)pucDes;

	ucDesTag = pucRaw[0];
	ucDesLen = pucRaw[1];

	if((ucDesTag != TAG_CA_CONTRACT_INFOMATION) || (ucDesLen == 0))
	{
		return ERR_FAIL;
	}
	pstDes->ucDesTag = ucDesTag;
 	pstDes->usCASystemId = HxMACRO_Get16Bit(pucRaw + 2);
 	pstDes->ucCAUnitId = (pucRaw[4] >> 4) & 0x0F;
	pstDes->ucNumOfComponent = (pucRaw[4] & 0x0F);
	pucRaw += 5;
	if(pstDes->ucNumOfComponent > 0)
	{
		pstDes->pucComponentTag = (HUINT8*)HLIB_STD_MemAlloc(sizeof(HUINT8) * pstDes->ucNumOfComponent);
		ARIB_DES_CHECK_MEM(pstDes->pucComponentTag);
		memset(pstDes->pucComponentTag, 0, (sizeof(HUINT8) * pstDes->ucNumOfComponent));

		for(i=0;i<pstDes->ucNumOfComponent;i++)
		{
			pstDes->pucComponentTag[i] = HxMACRO_Get8Bit(pucRaw);
			pucRaw += 1;
		}
	}

	pstDes->ucContractVerificationInfoLength = HxMACRO_Get8Bit(pucRaw);
	pucRaw += 1;
	if(pstDes->ucContractVerificationInfoLength > 0)
	{
		pstDes->pucContractVerificationInfo = (HUINT8*)HLIB_STD_MemAlloc(sizeof(HUINT8) * pstDes->ucContractVerificationInfoLength);
		ARIB_DES_CHECK_MEM(pstDes->pucContractVerificationInfo);
		memset(pstDes->pucContractVerificationInfo, 0, (sizeof(HUINT8) * pstDes->ucContractVerificationInfoLength));

		pTemp = (pucRaw + 5 + pstDes->ucNumOfComponent);
		for(i=0;i<pstDes->ucContractVerificationInfoLength;i++)
		{
			pstDes->pucContractVerificationInfo[i] = HxMACRO_Get8Bit(pucRaw);
			pucRaw += 1;
		}
	}
	pTemp = pucRaw + 4 + pstDes->ucNumOfComponent + pstDes->ucContractVerificationInfoLength;
	pstDes->ucFeeNameLength = HxMACRO_Get8Bit(pTemp);
	if(pstDes->ucFeeNameLength > 0)
	{
		pszText = (HUINT8 *)SILIB_TEXT_MakeUtf8Ex(eSIxTEXT_ENC_JIS, NULL, pucRaw, (HUINT32)pstDes->ucFeeNameLength, NULL, NULL);
		if(pszText != NULL)
		{
			nTempTextLen = HxSTD_StrLen((HCHAR *)pszText);
			pstDes->pucFeeName = HLIB_STD_MemAlloc(sizeof(HUINT8) * (nTempTextLen+1));
			if(pstDes->pucFeeName != NULL)
			{
				memset(pstDes->pucFeeName, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
				memcpy(pstDes->pucFeeName, pszText, HxSTD_StrLen((HCHAR *)pszText));
				pstDes->pucFeeName[HxSTD_StrLen((HCHAR *)pszText)] = ARIB_DES_EOS;
			}

			HLIB_STD_MemFree(pszText);
		}
	}

	return ERR_OK;
}

void  	OPL_PARSER_JAPAN_PrintCaContractInfoDes(HUINT8 *pucDes)
{
	APKS_CaContractInfoDes_t			*pstDes;
	HINT32							i =0;

	if (pucDes == NULL)					{ return; }

	pstDes = (APKS_CaContractInfoDes_t *)pucDes;

	HxLOG_Print( "\n\n##### Ca Contrat Info Descriptor #####\n\n");

	do
	{
		HxLOG_Print("ucDesTag : 0x%x\n" , pstDes->ucDesTag );
		HxLOG_Print("usCASystemId : 0x%x\n" , pstDes->usCASystemId );
		HxLOG_Print("ucCAUnitId : 0x%x\n" , pstDes->ucCAUnitId );
		HxLOG_Print("ucNumOfComponent : %d\n" , pstDes->ucNumOfComponent );
		if( pstDes->pucComponentTag )
		{
			for( i =0; i< pstDes->ucNumOfComponent ; i++)
			{
				HxLOG_Print("pucComponentTag[%d] : 0x%x\n" , i, pstDes->pucComponentTag[i] );
			}
		}
		HxLOG_Print("ucContractVerificationInfoLength : %d\n" , pstDes->ucContractVerificationInfoLength );
		if( pstDes->pucContractVerificationInfo )
		{
			HxLOG_Print( "pucContractVerificationInfo : " );
			for( i =0; i< pstDes->ucContractVerificationInfoLength ; i++)
			{
				HxLOG_Print("0x%x, " , pstDes->pucContractVerificationInfo[i] );
			}
			HxLOG_Print( "\n" );
		}
		HxLOG_Print("ucFeeNameLength : %d\n" , pstDes->ucFeeNameLength );
		if( pstDes->pucFeeName )
		{
			HxLOG_Print("pucFeeName : %s\n" , pstDes->pucFeeName );
		}

		pstDes = pstDes->pstNext;
	}while( pstDes );

	HxLOG_Print( "\n\n##### ##### END ##### #####\n\n");
}

#define ______0x09_CA______
void 	OPL_PARSER_JAPAN_FreeCaDes(void *pucDesc)
{
	APKS_CaDes_t				*pstDes = (APKS_CaDes_t *)pucDesc;
	APKS_PrivateDataInfo_t		*pPrivateDataInfo = NULL;

	if(pstDes == NULL)
	{
		return;
	}

	if (pstDes->pPrivateDataInfo != NULL)
	{
		pPrivateDataInfo = pstDes->pPrivateDataInfo;
		if (pPrivateDataInfo->pPrivateData != NULL )
		{
			HLIB_STD_MemFree((void *)pPrivateDataInfo->pPrivateData);
		}

		HLIB_STD_MemFree((void *)pstDes->pPrivateDataInfo);
	}

	HLIB_STD_MemFree(pstDes);
}

HUINT8*	OPL_PARSER_JAPAN_AllocCaDes(void)
{
	APKS_CaDes_t	*pCaDes;

	pCaDes = (APKS_CaDes_t *)HLIB_STD_MemAlloc(sizeof(APKS_CaDes_t));
	if (pCaDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocCaDes()\n");
		return NULL;
	}

	pCaDes->usCaSysId 		= 0;
	pCaDes->usCaPid 			= 0;
	pCaDes->pPrivateDataInfo	= NULL;

	return ((HUINT8 *)pCaDes);
}

HERROR	OPL_PARSER_JAPAN_ParseCaDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8			 		ucDesTag;
	APKS_CaDes_t				*pCaDes = (APKS_CaDes_t *)pucDes;
	APKS_PrivateDataInfo_t		*pNewPrivateDataInfo = NULL;
	HUINT16					usDesLen, usCaSysId, usCaPid;

	HxLOG_Info("parseCaDes() Entered!");

	/** < descriptor tag (1byte) */
	HxLOG_Print("\t\t(descriptor tag = 0x%x)\n", *pucRaw);
	ucDesTag = pucRaw[0];
	pucRaw++;

	/** < descriptor length (1byte) */
	usDesLen = *pucRaw++;
	HxLOG_Print("\t\t(descriptor length = %d)\n", usDesLen);

	if (usDesLen < 4)
	{
		HxLOG_Print("[WARN] Unknown parseCaDes Format!\n");
		return ERR_FAIL;
	}

	usCaSysId = (HUINT16)HxMACRO_Get16Bit(pucRaw);
	pucRaw += 2;
	usDesLen -=2;
	HxLOG_Print("\t\t(CA system id = 0x%x)\n", usCaSysId);

	usCaPid = (HUINT16)HxMACRO_Get13Bit(pucRaw);
	pucRaw += 2;
	usDesLen -= 2;
	HxLOG_Print("\t\t(CA pid = 0x%x)\n", usCaPid);

	/** < Private Data */
	if (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		pNewPrivateDataInfo = (APKS_PrivateDataInfo_t *)HLIB_STD_MemAlloc(sizeof(APKS_PrivateDataInfo_t));
		if (pNewPrivateDataInfo == NULL)
			return ERR_FAIL;

		HxSTD_MemSet( pNewPrivateDataInfo, 0x00 , sizeof( APKS_PrivateDataInfo_t *));

		pNewPrivateDataInfo->ucPrivateDataLen	= (HUINT8) usDesLen;
		pNewPrivateDataInfo->pPrivateData = (HUINT8 *)HLIB_STD_MemAlloc(usDesLen+1);

		if( pNewPrivateDataInfo->pPrivateData != NULL )
		{
			HxSTD_MemCopy(pNewPrivateDataInfo->pPrivateData, pucRaw, usDesLen);
		}
	}

	/** < Descriptor 데이타 구성. */
	pCaDes->ucDesTag			= ucDesTag;
	pCaDes->usCaSysId 		= usCaSysId;
	pCaDes->usCaPid 			= usCaPid;
	pCaDes->pPrivateDataInfo	= pNewPrivateDataInfo;

	HxLOG_Info(" parseCaDes() Leaved!");

	return ERR_OK;
}

void  	OPL_PARSER_JAPAN_PrintCaDes(HUINT8 *pucDes)
{
	APKS_CaDes_t					*pstDes;

	if (pucDes == NULL)					{ return; }

	pstDes = (APKS_CaDes_t *)pucDes;

	HxLOG_Print( "\n\n##### Ca Contrat Info Descriptor #####\n\n");

	if( pstDes )
	{
		HxLOG_Print("ucDesTag : 0x%x\n" , pstDes->ucDesTag );
		HxLOG_Print("usCaSysId : 0x%x\n" , pstDes->usCaSysId );
		HxLOG_Print("usCaPid : 0x%x\n" , pstDes->usCaPid );

		if( pstDes->pPrivateDataInfo )
		{
			HxLOG_Print("pPrivateDataInfo->ucPrivateDataLen : %d\n" , pstDes->pPrivateDataInfo->ucPrivateDataLen);
			if( pstDes->pPrivateDataInfo->pPrivateData )
			{
				HxLOG_Print("pPrivateDataInfo->pPrivateData : %s\n" , pstDes->pPrivateDataInfo->pPrivateData);
			}
		}
	}

	HxLOG_Print( "\n\n##### ##### END ##### #####\n\n");
}


#define _________________FOR_PARSING_PMT___________________________________

static INLINE void local_japan_GetFirstProgDescriptorPos(HUINT8 *pucSection, HUINT8 **ppFirstProgDes)
{
	*ppFirstProgDes = pucSection + 12;
}
static INLINE HBOOL local_japan_IsSameDesTag (HUINT32 ulSecDesTag, HUINT32 ulArgDesTag)
{
  	return ((ulArgDesTag == TAG_ALL_DESCRIPTORS) || ((ulArgDesTag & 0xff) == (ulSecDesTag & 0xff))) ? TRUE : FALSE;
}
static INLINE void local_japan_GetNextProgDescriptorPos(HUINT8 *pucProgDes, HUINT8 **ppNextProgDes, HUINT32 ulDesLen)
{
	*ppNextProgDes = pucProgDes + 2 + (HUINT32)ulDesLen;
}

HERROR OPL_PARSER_JAPAN_GetPmtProgramDescriptorFromRaw (HUINT8 *pucRaw,
																	HUINT32 ulTag,
																	HUINT32 ulDesIdx,
																	HxVector_t *pDest)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucProgDes;

	if (pucRaw == NULL)			{ return ERR_FAIL; }
	if (pDest == NULL)				{ return ERR_FAIL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 10);

	ulDesCnt = 0;
	local_japan_GetFirstProgDescriptorPos (pucRaw, &pucProgDes);

	while ((nSecLen > 0) && (nProgInfoLen > 0) && (nSecLen > nProgInfoLen))
	{
		if (nSecLen < 2)
		{
			HxLOG_Error("sSecLen < 2 :  %d\n" , nSecLen );
			return ERR_FAIL;
		}

		ucDesTag = HxMACRO_Get8Bit (pucProgDes);
		ucDesLen = HxMACRO_Get8Bit (pucProgDes + 1);

		nSecLen -= (2 + ucDesLen);
		nProgInfoLen -= (2 + ucDesLen);

		if (nSecLen < 0)
		{
			HxLOG_Error("sSecLen < 0 :  %d\n" , nSecLen );
			return ERR_FAIL;
		}

		if (local_japan_IsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)			// Different TAG
		{
			local_japan_GetNextProgDescriptorPos (pucProgDes, &pucProgDes, (HUINT32)ucDesLen);
			continue;
		}

		if (ulDesCnt != ulDesIdx)		// Different Index
		{
			ulDesCnt++;
			local_japan_GetNextProgDescriptorPos (pucProgDes, &pucProgDes, (HUINT32)ucDesLen);
			continue;
		}

		HxLOG_Print("Pmt program Descriptor Tag = 0x%x\n",ucDesTag);

		HLIB_VECTOR_Add( pDest, (void *)pucProgDes );
		return ERR_OK;
	}

	return ERR_FAIL;
}

#define ___________________________PARSING________________________________________________________

HxVector_t	*OPL_PARSER_JAPAN_ParsingDesc( HxVector_t *descriptor, HINT32 tag )
{
	HxVector_t 					*list = NULL;
	const HUINT8					*desc = NULL;
	void							*comp = NULL;
	HERROR						hErr = ERR_FAIL;
	OPLParsingTable				*pstDesc = NULL;

	HxLOG_Trace();
	return_if(descriptor == NULL , NULL);

	pstDesc = OPL_PARSER_JAPAN_GetParsingDesc( tag );
	return_if(pstDesc == NULL , NULL);

	list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))pstDesc->free, NULL);
	return_if(list == NULL, NULL);

	if(descriptor)
	{
		HxVECTOR_EACH(descriptor, const HUINT8 *, desc, {
			if (desc == NULL)
				continue;

			if( tag != desc[0] )
				continue;

			comp =pstDesc->alloc();
			if( comp == NULL )
				continue;

			hErr = pstDesc->parse( (HUINT8 *)desc , (HUINT8 *)comp );
			if( hErr == ERR_OK && comp )
			{
				(void) HLIB_VECTOR_Add(list, comp);
			}
		});
	}

	if (HLIB_VECTOR_Length(list) == 0)
	{
		HLIB_VECTOR_Delete(list);
		return NULL;
	}

	return list;
}

