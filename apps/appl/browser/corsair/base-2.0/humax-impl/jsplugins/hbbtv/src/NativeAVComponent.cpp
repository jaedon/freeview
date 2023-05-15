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

#include "NativeAVComponent.h"
#include "macros.h"
#include <oplavcomponent.h>

OOIFReturnCode native_AVComponentInitialize(AVComponentHandle i, AVComponentData *data)
{
	AVComponentData		*pstSrc = (AVComponentData *)i;

	if ((NULL != pstSrc) && (NULL != data))
	{
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