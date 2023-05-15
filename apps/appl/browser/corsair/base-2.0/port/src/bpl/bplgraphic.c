/**************************************************************************************/
/**
 * @file bplgraphickit.c
 *
 * The media interfaces
 *
 **************************************************************************************
 **/

#include "bplgraphic.h"

#include "gorgon.h"

#include <hlib.h>

#include <stdlib.h>

/******************************************************************
	Define
******************************************************************/


/******************************************************************
	Typedef
******************************************************************/
typedef	struct {
	gorgon_surface_t	*hSurface;
	int		nWidth;
	int		nHeight;
} xBplGraphic_t;

/******************************************************************
	Static variables
******************************************************************/

/******************************************************************
	Static functions
******************************************************************/
inline static xBplGraphic_t *__xgraphic(BplGraphic_t *hGraphic)
{
	return (xBplGraphic_t *)hGraphic;
}

/******************************************************************
	Interfaces
******************************************************************/
BplGraphic_t *BPL_Graphic_Create(int nWidth, int nHeight)
{
	xBplGraphic_t *hFB = NULL;
	gorgon_rect_t rect = { 0, 0, nWidth, nHeight };

	hFB = HLIB_STD_MemAlloc(sizeof(*hFB));
	if ( hFB == NULL )
	{
		return NULL;
	}

	if ( nWidth <= 0 ) nWidth = CONFIG_MWC_OSD_HORIZONTAL;
	if ( nHeight <= 0 ) nHeight = CONFIG_MWC_OSD_VERTICAL;

	hFB->hSurface = gorgon_surface_create(GORGON_FORMAT_ARGB32, &rect, 1);
	if ( ! hFB->hSurface )
	{
		HLIB_STD_MemFree(hFB);
		return NULL;
	}

	hFB->nWidth = nWidth;
	hFB->nHeight = nHeight;

	return hFB;
}

void BPL_Graphic_Destroy(BplGraphic_t *hGraphic)
{
	xBplGraphic_t *hFB = __xgraphic(hGraphic);
	if ( hFB == NULL )
	{
		return;
	}

	gorgon_surface_destroy(hFB->hSurface);

	HLIB_STD_MemFree(hFB);
	hFB = NULL;
}

char *BPL_Graphic_GetFrameBuffer(BplGraphic_t *hGraphic)
{
	gorgon_imageraw_t *raw_data = NULL;

	xBplGraphic_t *hFB = __xgraphic(hGraphic);
	if ( hFB == NULL )
	{
		return NULL;
	}

	gorgon_surface_acquire_data(hFB->hSurface, &raw_data);

	return (char *)raw_data;
}


BPLStatus BPL_Graphic_GetWidth(BplGraphic_t *hGraphic, int *pnWidth)
{
	xBplGraphic_t *hFB = __xgraphic(hGraphic);
	if ( hFB == NULL )
	{
		return BPL_STATUS_ERROR;
	}

	if ( pnWidth == NULL )
	{
		return BPL_STATUS_ERROR;
	}

	*pnWidth = hFB->nWidth;

	return BPL_STATUS_OK;
}

BPLStatus BPL_Graphic_GetHeight(BplGraphic_t *hGraphic, int *pnHeight)
{
	xBplGraphic_t *hFB = __xgraphic(hGraphic);
	if ( hFB == NULL )
	{
		return BPL_STATUS_ERROR;
	}

	if ( pnHeight == NULL )
	{
		return BPL_STATUS_ERROR;
	}

	*pnHeight = hFB->nHeight;

	return BPL_STATUS_OK;
}

BPLStatus BPL_Graphic_UpdateScreen(BplGraphic_t *hGraphic, int x, int y,
							int width, int height)
{
	gorgon_rect_t rect = {x, y, width, height};

	xBplGraphic_t *hFB = __xgraphic(hGraphic);
	if ( hFB == NULL )
	{
		return BPL_STATUS_ERROR;
	}

	rect.x = x;
	rect.y = y;
	rect.w = width - x;
	rect.h = height - y;
	gorgon_surface_flip(hFB->hSurface, &rect);

	return BPL_STATUS_OK;
}
