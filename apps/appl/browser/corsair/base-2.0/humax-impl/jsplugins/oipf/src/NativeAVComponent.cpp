// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>

#include "NativeAVComponent.h"
#include "macros.h"
#include "oplavcomponent.h"
#include "oplavoutput.h"

OOIFReturnCode native_AVComponentInitialize(AVComponentHandle i, AVComponentData *data)
{
	AVComponentData		*pstSrc = (AVComponentData *)i;

	if ((NULL != pstSrc) && (NULL != data))
	{
//		OOIF_LOG_PRINT (">>>>> native_AVComponentInitialize (Handle=%d, pid=%d)\n", (unsigned int)i, (unsigned int)pstSrc->pid);
		memcpy (data, pstSrc, sizeof(AVComponentData));

		if (NULL != pstSrc->encoding)			{ data->encoding = HLIB_STD_StrDup (pstSrc->encoding); }
#if defined(OIPF) || defined(HBBTV_1_2_1)
		if (pstSrc->type == COMPONENT_TYPE_VIDEO)
		{
			;
		}
		else if (pstSrc->type == COMPONENT_TYPE_AUDIO)
		{
			if (NULL != pstSrc->audio_language)
			{
				data->audio_language = HLIB_STD_StrDup (pstSrc->audio_language);
			}
		}
		else if (pstSrc->type == COMPONENT_TYPE_SUBTITLE)
		{
			if (NULL != pstSrc->subtitle_language)
			{
				data->subtitle_language = HLIB_STD_StrDup (pstSrc->subtitle_language);
			}
		}
#else
		if (NULL != pstSrc->language)
		{
			data->language = HLIB_STD_StrDup (pstSrc->language);
		}
#endif
#if defined(HMX_WEBUI)
		if (NULL != pstSrc->language2) 			{ data->language2 = HLIB_STD_StrDup (pstSrc->language2); }
#endif
#ifdef JLABS
		if (NULL != pstSrc->language2)			{ data->language2 = HLIB_STD_StrDup (pstSrc->language2); }
		if (NULL != pstSrc->text)				{ data->text = HLIB_STD_StrDup (pstSrc->text); }
		if( pstSrc->multiviewNum > 0 )
		{
			HINT32 i =0;

			data->multiview = ( char ** )HLIB_STD_MemAlloc( sizeof(char *) * pstSrc->multiviewNum );
			for( i =0; i< pstSrc->multiviewNum; i++ )
			{
				if (NULL != pstSrc->multiview[i])				{ data->multiview[i] = HLIB_STD_StrDup (pstSrc->multiview[i]); }
			}
		}
#endif // JLABS
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVComponentReleaseHandle(AVComponentHandle id)
{
	AVComponentData		*pstSrc = (AVComponentData *)id;

	if (NULL != pstSrc)
	{
		if (NULL != pstSrc->encoding)			{ HLIB_STD_MemFree(pstSrc->encoding); }
#if defined(OIPF) || defined(HBBTV_1_2_1)
		if (pstSrc->type == COMPONENT_TYPE_VIDEO)
		{
			;
		}
		else if (pstSrc->type == COMPONENT_TYPE_AUDIO)
		{
			if (NULL != pstSrc->audio_language)
			{
				HLIB_STD_MemFree(pstSrc->audio_language);
			}
		}
		else if (pstSrc->type == COMPONENT_TYPE_SUBTITLE)
		{
			if (NULL != pstSrc->subtitle_language)
			{
				HLIB_STD_MemFree(pstSrc->subtitle_language);
			}
		}
#else
		if (NULL != pstSrc->language)
		{
			HLIB_STD_MemFree(pstSrc->language);
		}
#endif
#if defined(HMX_WEBUI)
		if (NULL != pstSrc->language2) 			{ HLIB_STD_MemFree(pstSrc->language2); }
#endif
#ifdef JLABS
		if (NULL != pstSrc->language2)			{ HLIB_STD_MemFree(pstSrc->language2); }
		if (NULL != pstSrc->text)				{ HLIB_STD_MemFree(pstSrc->text); }
		if( pstSrc->multiviewNum > 0 )
		{
			HINT32 i=0;
			for( i =0; i< pstSrc->multiviewNum; i++ )
			{
				HLIB_STD_MemFree( pstSrc->multiview[i] );
			}
			HLIB_STD_MemFree( pstSrc->multiview );
		}
#endif // JLABS

		HLIB_STD_MemFree(pstSrc);
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVComponentCopy(AVComponentData *pstDst, AVComponentHandle vpstSrc)
{
	AVComponentData *pstSrc = (AVComponentData *)vpstSrc;

	if ((NULL != pstSrc) && (NULL != pstDst))
	{
		memset( pstDst, 0x00, sizeof(AVComponentData ));
		memcpy (pstDst, pstSrc, sizeof(AVComponentData));
		if (NULL != pstSrc->encoding)			{ pstDst->encoding = HLIB_STD_StrDup (pstSrc->encoding); }
#if defined(OIPF) || defined(HBBTV_1_2_1)
		if (pstSrc->type == COMPONENT_TYPE_VIDEO)
		{
			;
		}
		else if (pstSrc->type == COMPONENT_TYPE_AUDIO)
		{
			if (NULL != pstSrc->audio_language)
			{
				pstDst->audio_language = HLIB_STD_StrDup (pstSrc->audio_language);
			}
		}
		else if (pstSrc->type == COMPONENT_TYPE_SUBTITLE)
		{
			if (NULL != pstSrc->subtitle_language)
			{
				pstDst->subtitle_language = HLIB_STD_StrDup (pstSrc->subtitle_language);
			}
		}
#else
		if (NULL != pstSrc->language)
		{
			pstDst->language = HLIB_STD_StrDup (pstSrc->language);
		}
#endif
#if defined(HMX_WEBUI)
		if (NULL != pstSrc->language2)			{ pstDst->language2 = HLIB_STD_StrDup (pstSrc->language2); }
#endif
#ifdef JLABS
		if (NULL != pstSrc->language2)			{ pstDst->language2 = HLIB_STD_StrDup (pstSrc->language2); }
		if (NULL != pstSrc->text)				{ pstDst->text = HLIB_STD_StrDup (pstSrc->text); }
		if( pstSrc->multiviewNum > 0 )
		{
			HINT32 i =0;

			pstDst->multiview = ( char ** )HLIB_STD_MemAlloc( sizeof(char *) * pstSrc->multiviewNum );
			for( i =0; i< pstSrc->multiviewNum; i++ )
			{
				if (NULL != pstSrc->multiview[i])				{ pstDst->multiview[i] = HLIB_STD_StrDup (pstSrc->multiview[i]); }
			}
		}
#endif // JLABS
	}

	return OOIF_RETURN_OK;
}

/* opl 구조체를 native 구조체로 바꾼다.
둘 사이의 데이타 구조가 달라서 실제로 class에서 바로 처리되는 거 때문에.. */
OOIFReturnCode native_AVComponentConvert( void *vpstSrc, AVComponentData *pstDst)
{
	OplAVComponent_t *pstSrc = (OplAVComponent_t *)vpstSrc;

	/* 해당 포인터는 class가 release 될 때 같이 free 된다. */
	if ((NULL == pstSrc) || (NULL == pstDst) )
		return OOIF_RETURN_ERROR;

	pstDst->type		= static_cast<AVComponentType>(pstSrc->eType);
	pstDst->componentTag = pstSrc->ulComponentTag;
	pstDst->pid = pstSrc->ulPid;
#if defined(HMX_WEBUI)
	pstDst->subType = pstSrc->eSubType;
#endif
#if defined(OIPF) || defined(HBBTV_1_2_1)
	if (pstDst->type == COMPONENT_TYPE_VIDEO)
	{
		pstDst->aspectRatio = pstSrc->fAspectRatio;
	}
	else if (pstDst->type == COMPONENT_TYPE_AUDIO)
	{
		pstDst->audioDescription = pstSrc->bAudioDescription;
		pstDst->audioChannels = pstSrc->ulAudioChannels;
		if (0 < strlen( pstSrc->szLanguage))
		{
			pstDst->audio_language = HLIB_STD_StrDup (pstSrc->szLanguage);
		}
	}
	else if (pstDst->type == COMPONENT_TYPE_SUBTITLE)
	{
		pstDst->hearingImpaired = pstSrc->bHardOfHearing_Impaired;
	}
#else
	pstDst->aspectRatio = pstSrc->fAspectRatio;
	pstDst->audioDescription = pstSrc->bAudioDescription;
	pstDst->audioChannels = pstSrc->ulAudioChannels;
	if (0 < strlen( pstSrc->szLanguage))
	{
		pstDst->language = HLIB_STD_StrDup (pstSrc->szLanguage);
	}
	pstDst->hearingImpaired = pstSrc->bHardOfHearing_Impaired;
#endif
#if defined(HMX_WEBUI)
	if (0 < strlen(pstSrc->szLanguage2))		{ pstDst->language2 = HLIB_STD_StrDup (pstSrc->szLanguage2); }
#endif

#ifdef JLABS
	pstDst->componentType = pstSrc->ulComponentType;
	pstDst->streamType = pstSrc->ulStreamType;
	pstDst->associatedContentFlag = pstSrc->bAssociatedContentFlag;
	pstDst->dataEntryComponentFlag = pstSrc->bDataEntryComponentFlag;
	if (0 < strlen(pstSrc->szLanguage2))		{ pstDst->language2 = HLIB_STD_StrDup (pstSrc->szLanguage2); }
	if (0 < strlen(pstSrc->szText))				{ pstDst->text = HLIB_STD_StrDup (pstSrc->szText); }
	if( pstSrc->nMultiViewNum > 0 )
	{
		HINT32 i =0;

		pstDst->multiview = ( char ** )HLIB_STD_MemAlloc( sizeof(char *) * pstSrc->nMultiViewNum );
		for( i =0; i< pstSrc->nMultiViewNum; i++ )
		{
			if (0 < strlen(pstSrc->szMultiView[i]))		{ pstDst->multiview[i]= HLIB_STD_StrDup (pstSrc->szMultiView[i]); }
		}
	}
	pstDst->multiviewNum = pstSrc->nMultiViewNum;
	pstDst->digitalCopyControl = pstSrc->nDigitalCopyControl;
#endif // JLABS

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVComponentGetMultiView(AVComponentData *data, int *count, char ***multiView)
{
	if ((NULL == count) || (NULL == multiView) || ( NULL == data) )
		return OOIF_RETURN_ERROR;

	*count =0;
	*multiView = NULL;

#ifdef JLABS
	*count = data->multiviewNum;
	if( data->multiviewNum > 0 )
	{
		HINT32 i =0;

		*multiView = ( char ** )HLIB_STD_MemAlloc( sizeof(char *) * data->multiviewNum );
		for( i =0; i< data->multiviewNum; i++ )
		{
			if (0 < strlen(data->multiview[i]))		{ (*multiView)[i]= HLIB_STD_StrDup (data->multiview[i]); }
		}
	}
#endif

	return OOIF_RETURN_OK;
}

#ifdef JLABS
#define MAX_BUF_LEN 20
static char szAudioMode[MAX_BUF_LEN];

OOIFReturnCode native_AVComponentGetAudioMode(OOIFNumber *retval)
{
	int getValue = 0;
	if (NULL == retval)
		return OOIF_RETURN_VALUE_UNDEFINED;

	HxSTD_MemSet(szAudioMode,0,MAX_BUF_LEN);

	if ( !OplHmxAudio::getDualMono(0, &getValue) )
	{
		OOIF_LOG_ERROR("[%s:%d] getDualMono ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	*retval = getValue;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_AVComponentSetAudioMode(OOIFNumber value)
{
	if (!OplHmxAudio::setDualMono(0, (int)value))
	{
		OOIF_LOG_ERROR("[%s:%d] getDualMono ret Error!!!\n",__PRETTY_FUNCTION__,__LINE__);
		return OOIF_RETURN_ERROR;
	}

	return OOIF_RETURN_OK;
}
#endif

