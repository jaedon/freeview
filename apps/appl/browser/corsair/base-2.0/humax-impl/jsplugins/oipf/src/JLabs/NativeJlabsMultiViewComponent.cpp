// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>

#include "NativeJlabsMultiViewComponent.h"
#include "macros.h"
#include "oplavcomponent.h"
#include "oplavoutput.h"
#include "oplmultiviewcomponent.h"

OOIFReturnCode native_jlabsMultiViewComponentInitialize(jlabsMultiViewComponentHandle i, jlabsMultiViewComponentData *data)
{
	jlabsMultiViewComponentData		*pstSrc = (jlabsMultiViewComponentData *)i;

	if ((NULL != pstSrc) && (NULL != data))
	{
#ifdef JLABS
		memcpy (data, pstSrc, sizeof(jlabsMultiViewComponentData));

		if (NULL != pstSrc->name)			{ data->name = HLIB_STD_StrDup (pstSrc->name); }

		if( (pstSrc->compNum > 0) && (pstSrc->avcomponents != NULL ))
		{
			HINT32 i =0;

			data->avcomponents= (void **)HLIB_STD_MemAlloc( sizeof(void *) * pstSrc->compNum );
			for( i =0; i< pstSrc->compNum; i++ )
			{
				data->avcomponents[i] = pstSrc->avcomponents[i];
			}
		}
#endif // JLABS
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMultiViewComponentGetComponents(jlabsMultiViewComponentData *data, int *count, AVComponentHandle **handles)
{
	AVComponentHandle *myHandles = NULL;

	if ((NULL == count) || (NULL == handles) || ( NULL == data) )
		return OOIF_RETURN_ERROR;

	*count = data->compNum;
	if( (data->compNum) > 0 && ( data->avcomponents != NULL ))
	{
		myHandles = ( AVComponentHandle * )HLIB_STD_MemAlloc( sizeof(AVComponentHandle) * data->compNum );
		for(int i =0; i< data->compNum; i++ )
		{
			myHandles[i] = (AVComponentHandle)data->avcomponents[i];
		}
	}

	*handles = myHandles;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsMultiViewComponentReleaseHandle(jlabsMultiViewComponentHandle id)
{
	jlabsMultiViewComponentData		*pstSrc = (jlabsMultiViewComponentData *)id;

	if (NULL != pstSrc)
	{
		if (NULL != pstSrc->name)				{ HLIB_STD_MemFree(pstSrc->name); }
		if (pstSrc->avcomponents)
		{
			for( int i =0; i<pstSrc->compNum; i++ )
			{
				if( pstSrc->avcomponents[i] )
				{
					HLIB_STD_MemFree( (void *)pstSrc->avcomponents[i] );
				}
			}
			HLIB_STD_MemFree(pstSrc->avcomponents);
		}
		HLIB_STD_MemFree(pstSrc);
	}

	return OOIF_RETURN_OK;
}

/* opl 구조체를 native 구조체로 바꾼다.
둘 사이의 데이타 구조가 달라서 실제로 class에서 바로 처리되는 거 때문에.. */
OOIFReturnCode native_jlabsMultiViewComponentConvert( void *vpstSrc, jlabsMultiViewComponentData *pstDst)
{
	OplMultiViewComponent_t	*pstSrc = (OplMultiViewComponent_t *)vpstSrc;
	AVComponentData			*tempAVData = NULL;

	/* 해당 포인터는 class가 release 될 때 같이 free 된다. */
	if ((NULL == pstSrc) || (NULL == pstDst) )
		return OOIF_RETURN_ERROR;

	pstDst->compNum = pstSrc->compNum;
	pstDst->index = pstSrc->index;
	if (0 < strlen(pstSrc->szName))		{ pstDst->name = HLIB_STD_StrDup (pstSrc->szName); }

	if( pstSrc->compNum > 0 )
	{
		pstDst->avcomponents = ( void ** )HLIB_STD_MemAlloc( sizeof(void *) * pstSrc->compNum );
		for(HINT32 i =0; i< pstSrc->compNum; i++ )
		{
			tempAVData = (AVComponentData *)HLIB_STD_MemAlloc( sizeof( AVComponentData));
			memset( tempAVData , 0x00, sizeof ( AVComponentData ));

			native_AVComponentConvert( (void *)&(pstSrc->hComponents[i]) , tempAVData );
			pstDst->avcomponents[i] = (void *)tempAVData;
		}
	}

	return OOIF_RETURN_OK;
}


