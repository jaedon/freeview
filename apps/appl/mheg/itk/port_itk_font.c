/**
	@file	  port_itk_font.c														\n
	@brief	  itk file for FONT_Driver porting layer							\n

	Description: ITK에서 Font Driver Porting 에 사용되는 기능을 정의한 Body파일	\n
	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n

	[Description]

*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include "port_itk_main.h"
#include "port_itk_console.h"
#include "port_itk_font.h"

// freetype API
#include <ft2build.h>
#include <freetype/ftmodapi.h>
#include FT_FREETYPE_H
#include FT_PFR_H
#include FT_MODULE_H

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
// supported font metrics types
#define EF_PFR 1
#define EF_TTF 2

// IMPORTANT: TrueType hinting is covered by a number of patents
// owned by Apple Computer Inc. See http://www.freetype.org/patents.html
// To avoid patent infringements make sure that when building the
// Freetype library, the compile option:
//     TT_CONFIG_OPTION_BYTECODE_INTERPRETER
// is NOT defined in freetype/config/ftoption.h. Freetype will then
// use its unpatented Auto Hinter for font hinting.

// The following flags define how glyphs are to be rendered by Freetype.
//
// To render using UNHINTED glyphs, use these definitions:
//#define RENDER_FLAGS_MONO  FT_LOAD_RENDER | FT_LOAD_MONOCHROME |  FT_LOAD_CROP_BITMAP | FT_LOAD_NO_HINTING
//#define RENDER_FLAGS_8BIT  FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL | FT_LOAD_NO_HINTING | FT_LOAD_CROP_BITMAP

// To render using AutoHinted glyphs, use these defnitions:
#define RENDER_FLAGS_MONO  FT_LOAD_RENDER | FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO | FT_LOAD_CROP_BITMAP | FT_LOAD_FORCE_AUTOHINT
#define RENDER_FLAGS_8BIT  FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_CROP_BITMAP

// NOTE! Hinting improves the appearance of characters considerably, especially
// in monochrome single-bit rendering. However, the process of hinting affects
// the width and positioning of characters. This font driver attempts to
// compensate for the difference between logical character widths and their
// hinted widths, but occassional single-pixel errors in character positioning
// are inevitable. If this is not acceptable, do not use hinting.

#define MAX_STATIC_FONT 10

/* bitmap & metrics cache */
#define MCACHE_MAX 250

#define CMCACHE_SIZE			256
#define CMCACHE_EMPTY			0xDDDDDDD

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/**
 * Implementation of ITK interface to font driver.
 */

// simple linked list
typedef struct itk_fd_list_entry_t
{
    struct itk_fd_list_entry_t *nextCell;   // pointer to next font in list
} itk_fd_list_entry_t;

typedef struct itk_fd_linked_list_t
{
    struct itk_fd_list_entry_t *firstCell;   // pointer to next font in list
    int count;
} itk_fd_linked_list_t;

typedef struct itk_fd_font_attributes_t
{
    itk_fd_list_entry_t *nextCell;   // pointer to next font in list
    itk_fontdata_t info;           // font info structure
    itk_uint8_t *raw_data;          // font binary data
    itk_int32_t raw_data_length;    // length of font binary data (bytes)
    itk_uint8_t rendered14x9;       // 0 = normal, 1 = pre-rendered at 14x9
    itk_uint8_t type;               // Metrics type: EF_TTF or EF_PFR
    itk_uint8_t embedded;           // 0 = downloaded, 1 = embedded
    FT_UInt outline_res;            // outline resolution
    FT_Face  face;                  // Freetype font face structure
} itk_fd_font_attributes_t;

typedef struct itk_fd_mcache_t
{
	itk_fd_list_entry_t *nextCell;   // pointer to next cache entry in list
	itk_uint16_t			pointSize;
	itk_fontid_t			font_id;
	itk_charindex_t			index;
	itk_pmetrics_t			pmetrics;
	itk_uint32_t			log_width;
	itk_uint16_t			ar;
	itk_mask_t				*mask;
	int						access_count;
} itk_fd_mcache_t;

/* charmap lookup cache */
struct itk_fd_cmap_t
{
    itk_fontid_t font_id;
    itk_charindex_t index;
    itk_unicode_t unicode;
    int access_count;
} itk_fd_cmap_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static itk_fontdriver_t			s_stFontdriverObj;

static unsigned int ef_staticFontCount = 0;
static unsigned long ef_size[MAX_STATIC_FONT];
static unsigned int ef_render_14x9[MAX_STATIC_FONT];
static char *ef_names[MAX_STATIC_FONT];
static int ef_type[MAX_STATIC_FONT];
static itk_binaryfont_t ef_data[MAX_STATIC_FONT];

static itk_fd_linked_list_t fonts;            // list of dynamic fonts this driver knows about

static itk_fontid_t idlist[MAX_STATIC_FONT];  // ids of embedded fonts

static FT_Library ftlib;  // handle for the initialised library

static int initialised = 0;
static itk_graphics_pipeline_t *thePipe = NULL;
static itk_uint8_t bpp = 1;

itk_fd_linked_list_t m_cache;

struct itk_fd_cmap_t cmap_cache[ CMCACHE_SIZE ];
itk_uint32_t cmcache_next = 0;

/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/
static void mheg_itk_port_ftdInitList( itk_fd_linked_list_t *list );                //initialise linked list
static void mheg_itk_port_ftdAddList( itk_fd_linked_list_t *list, void *item );     //add item to linked list
static void mheg_itk_port_ftdRemoveList( itk_fd_linked_list_t *list, void *item );  //remove item from linked list
static itk_fd_list_entry_t *mheg_itk_port_ftdFindLastCell( itk_fd_linked_list_t *list );   // find last item of linked list

//static itk_fd_font_attributes_t *newFont();
static void mheg_itk_port_ftdReleaseFont( itk_fd_font_attributes_t *font );
#if 0		// not used
static itk_fd_font_attributes_t *local_itk_FONTDRIVER_getFontByURL( char *url );
#endif
static itk_fd_font_attributes_t *mheg_itk_port_ftdGetFontByID( itk_fontid_t id );

/* helper functions */
static void mheg_itk_port_ftdSetRenderSize( itk_fd_font_attributes_t *f, itk_int32_t points, itk_ar_factor_t ar );
static void mheg_itk_port_ftdReleaseMask( itk_mask_t *mask );
static itk_mask_t *mheg_itk_port_ftdCreateMask( const FT_Bitmap *bm, int depth );
static itk_fd_font_attributes_t *mheg_itk_port_ftdInitFont( itk_binaryfont_t newFont, char *name,
    itk_int32_t fontSize, itk_fontid_t id, itk_uint8_t embedded,
    itk_uint8_t type, itk_uint8_t rendered14x9 );


static void mheg_itk_port_ftdPurgeCaches( itk_fontid_t id );
static struct itk_fd_mcache_t *mheg_itk_port_ftdGetCharFromCache( itk_fontid_t id, itk_charindex_t ch,
    itk_uint16_t pointSize, itk_ar_factor_t ar );

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/**
 *
 */
static void *mheg_itk_port_ftdAlloc(FT_Memory memory, long size)
{
	void *nb;

	PARAM_UNUSED(memory);
	nb = HLIB_STD_MemAlloc( size );

	return nb;
}

/**
 *
 */
static void *mheg_itk_port_ftdRealloc(FT_Memory memory, long cur_size, long new_size, void *block)
{
	void *nblock = HLIB_STD_MemAlloc(new_size);

	PARAM_UNUSED(memory);
	if( nblock == NULL )
	{
		HxLOG_Assert(nblock);
		return NULL;
	}

	memcpy( nblock, block, (new_size > cur_size ? cur_size : new_size) );
	//DBG(("mheg_itk_port_ftdRealloc: block %p (%d) to new block %p (%d)\n",block,cur_size,nblock,new_size));
	HLIB_STD_MemFree( block );
	return nblock;
}

/**
 *
 */
static void mheg_itk_port_ftdFree(FT_Memory memory, void *block)
{
	PARAM_UNUSED(memory);
	//DBG(("mheg_itk_port_ftdFree: block %p\n",block));
	HLIB_STD_MemFree(block);
}

/**
 * General initialisation call
 * returns ITKE_OK if successful
 */
itk_errcode_t mheg_itk_port_ftdInitialize( struct itk_fontdriver_t *thiz,
	itk_graphics_pipeline_t *pipe, itk_uint8_t rbpp )
{
	unsigned int n;
	FT_Memory memory;

	PARAM_UNUSED(thiz);

	if( thiz != &s_stFontdriverObj )
	{
		HxLOG_Assert(thiz == &s_stFontdriverObj);
		return ITKE_FAIL;
	}

	if( initialised != 0 )
	{
		// duplicate font initialisation (skipped)
		HxLOG_Error(" Already initialised!!\r\n");
		return ITKE_OK;
	}

	// the structure is freed up by FT when the library is done
	memory = (FT_Memory)mheg_itk_port_ftdAlloc( NULL, sizeof(*memory) );
	if( memory == NULL )
	{
		HxLOG_Assert(memory);
		return ITKE_FAIL;
	}

	memory->user = NULL;
	memory->alloc = mheg_itk_port_ftdAlloc;
	memory->realloc = mheg_itk_port_ftdRealloc;
	memory->free = mheg_itk_port_ftdFree;

	thePipe = pipe;
	bpp = rbpp;

	mheg_itk_port_ftdInitList( &m_cache );
	mheg_itk_port_ftdInitList( &fonts );

	// initialise freetype
	// we assume this works, because if it doesn't, we're screwed
	if (0 != FT_New_Library(memory, &ftlib))
	{
		HxLOG_Error(" FATAL ERROR: Failed to init Freetype\r\n");
		HxLOG_Debug("free (%p)\n", memory);
		mheg_itk_port_ftdFree( NULL, memory );

		return ITKE_FAIL;
	}
	FT_Add_Default_Modules(ftlib);

	// load the embedded fonts
	for( n = 0; n < ef_staticFontCount; n++ )
	{
		mheg_itk_port_ftdInitFont( (itk_binaryfont_t)ef_data[n], ef_names[n], ef_size[n], NULL, 1,
			(itk_uint8_t)ef_type[n], (ef_render_14x9[n] == 1 ? 0 : 1 ) );
	}

 	for( n = 0 ; n < CMCACHE_SIZE; n++ )
		cmap_cache[n].index = CMCACHE_EMPTY;

	initialised = 1;

	return ITKE_OK;
}

/**
 * General shutdown call
 */
void mheg_itk_port_ftdShutdown( struct itk_fontdriver_t *thiz )
{
	struct itk_fd_mcache_t *slot;
	itk_fd_font_attributes_t *f;

	PARAM_UNUSED(thiz);

	initialised = 0;
	FT_Done_FreeType(ftlib);

	// release masks from cache
	slot = (itk_fd_mcache_t *)m_cache.firstCell;
	while( slot != NULL )
	{
		mheg_itk_port_ftdRemoveList( &m_cache, slot );
		mheg_itk_port_ftdReleaseMask( slot->mask );
		mheg_itk_port_ftdFree( NULL, slot );
		slot = (itk_fd_mcache_t *)m_cache.firstCell;
	}

	f = (itk_fd_font_attributes_t *)fonts.firstCell;
	while( f != NULL )
	{
		mheg_itk_port_ftdReleaseFont( f );
		f = (itk_fd_font_attributes_t *)fonts.firstCell;
	}
}

/*
* Convert unicode character id to internal format id
*/
itk_charindex_t mheg_itk_port_ftdUnicode2Index( struct itk_fontdriver_t *thiz,
	itk_fontid_t id, itk_unicode_t ch )
{
	itk_fd_font_attributes_t *f;
	itk_charindex_t index;
	struct itk_fd_cmap_t temp;
	int n;

	PARAM_UNUSED(thiz);
	for( n = 0; n < CMCACHE_SIZE; n++ )
	{
		if( cmap_cache[n].index == CMCACHE_EMPTY )
			break; // not found, out of entries

		if( cmap_cache[n].font_id == id && cmap_cache[n].unicode == ch )
		{
			cmap_cache[n].access_count++;
			if( n != 0 )
			{
				// move frequently accessed entries toward the front of the list
				if( cmap_cache[n].access_count > cmap_cache[n-1].access_count )
				{
					memcpy( &temp, &cmap_cache[n-1], sizeof(struct itk_fd_cmap_t) );
					memcpy( &cmap_cache[n-1], &cmap_cache[n], sizeof(struct itk_fd_cmap_t) );
					memcpy( &cmap_cache[n], &temp, sizeof(struct itk_fd_cmap_t) );
					n--;
				}
			}
			return cmap_cache[n].index;
		}
	}

	// not cached!
	f = mheg_itk_port_ftdGetFontByID( id );
	index = FT_Get_Char_Index( f->face, ch );

	if( index != 0 )
	{
		cmap_cache[ cmcache_next ].font_id = id;
		cmap_cache[ cmcache_next ].index = index;
		cmap_cache[ cmcache_next ].unicode = ch;
		cmap_cache[ cmcache_next ].access_count = 1;

		cmcache_next++;
		if( cmcache_next == CMCACHE_SIZE )
			cmcache_next = CMCACHE_SIZE / 2;
	}

	return index;
}

/**
 * Convert internal format character id to unicode
 * (or 0 if not found)
 */
 itk_unicode_t mheg_itk_port_ftdIndex2Unicode( struct itk_fontdriver_t *thiz,
	itk_fontid_t id, itk_charindex_t ch )
{
	FT_UInt glyph;
	FT_ULong ccode;
	itk_fd_font_attributes_t *f;
	int n;

	UNUSED(thiz);
	for( n = 0; n < CMCACHE_SIZE; n++ )
	{
		if( cmap_cache[n].index == CMCACHE_EMPTY )
			break; // not found, out of entries

		if( cmap_cache[n].font_id == id && cmap_cache[n].index == ch )
			return cmap_cache[n].unicode;
	}

	f = mheg_itk_port_ftdGetFontByID( id );
	// search font character map for a match (this is unpleasant, alternative is pregenerated table)
	ccode = FT_Get_First_Char( f->face, &glyph);
	while( glyph != ch && glyph != 0 )
	{
		ccode = FT_Get_Next_Char( f->face, ccode, &glyph);
	}

	cmap_cache[ cmcache_next ].font_id = id;
	cmap_cache[ cmcache_next ].index = ch;
	cmap_cache[ cmcache_next ].unicode = ccode;
	cmap_cache[ cmcache_next ].access_count = 1;

	cmcache_next++;
	if( cmcache_next == CMCACHE_SIZE )
	{
		cmcache_next = CMCACHE_SIZE / 2;
		for( n = cmcache_next; n < CMCACHE_SIZE; n++ )
		{
			cmap_cache[n].index = CMCACHE_EMPTY;
			cmap_cache[n].font_id = NULL;
			cmap_cache[n].unicode = 0;
			cmap_cache[n].access_count = 0;
		}
	}

	return ccode;
}

/*
* Get the logical width of a character
*/
itk_uint32_t mheg_itk_port_ftdGetLogicalSize( struct itk_fontdriver_t *thiz,
																		itk_fontid_t id, itk_charindex_t ch )
{
	struct itk_fd_mcache_t *slot = NULL;
	itk_ar_factor_t ar = { 1, 1 };
	PARAM_UNUSED(thiz);

	// see if we have a cached value:
	slot = mheg_itk_port_ftdGetCharFromCache( id, ch, 0, ar );
	if( slot == NULL )
	{
		HxLOG_Assert(slot);
		return 0;
	}

	return slot->log_width;
}

/**
 * Get the logical kerning from ch1 to ch2 (in em)
 */
itk_int32_t mheg_itk_port_ftdGetLogicalKerning( struct itk_fontdriver_t *thiz,
	itk_fontid_t id, itk_charindex_t ch1, itk_charindex_t ch2 )
{
	FT_Vector kerndata;
	itk_fd_font_attributes_t *f;
	PARAM_UNUSED(thiz);

	f = mheg_itk_port_ftdGetFontByID( id );
	if( f == NULL )
	{
		HxLOG_Assert(f);
		return 0;
	}

	if( FT_Get_PFR_Kerning( f->face, ch1, ch2, &kerndata ) != 0 )
	{
		HxLOG_Assert(0);
		return 0;
	}
	return( kerndata.x );
}

/**
 * Get physical metrics of a character at given point size
 * Fills in structure pointed to by 'metrics'
 * returns ITKE_OK or error code
 */
itk_errcode_t mheg_itk_port_ftdGetPhysicalMetrics( struct itk_fontdriver_t *thiz, itk_fontid_t id,
		itk_charindex_t ch, itk_uint16_t pointSize, itk_ar_factor_t ar, itk_pmetrics_t *metrics )
{
	struct itk_fd_mcache_t *slot = NULL;
	PARAM_UNUSED(thiz);

	slot = mheg_itk_port_ftdGetCharFromCache( id, ch, pointSize, ar );
	if( slot == NULL )
	{
		HxLOG_Assert(slot);
		return ITKE_FAIL;
	}

	metrics->setWidth = slot->pmetrics.setWidth;
	metrics->xPos = slot->pmetrics.xPos;
	metrics->xSize = slot->pmetrics.xSize;
	metrics->yPos = slot->pmetrics.yPos;
	metrics->ySize = slot->pmetrics.ySize;
	return ITKE_OK;
}

/**
 * Get general font information
 */
itk_errcode_t mheg_itk_port_ftdGetFontData( struct itk_fontdriver_t *thiz,
																	itk_fontid_t id, itk_fontdata_t *fontData )
{
	itk_fd_font_attributes_t *f;

	PARAM_UNUSED(thiz);
	f = mheg_itk_port_ftdGetFontByID( id );
	if( f == NULL )
	{
		HxLOG_Assert(f);

		return ITKE_FAIL;
	}

	fontData->bitmapSizes = f->info.bitmapSizes;
	fontData->charCount = f->info.charCount;
	fontData->charSize = f->info.charSize;
	fontData->id = id;
	fontData->metricsRes = f->info.metricsRes;
	fontData->name = f->info.name;
	fontData->sizesCount = f->info.sizesCount;
	fontData->xMax = f->info.xMax;
	fontData->xMin = f->info.xMin;
	fontData->yMax = f->info.yMax;
	fontData->yMin = f->info.yMin;

	HxLOG_Debug("fontId:0x%x, charSize:%d bitmapSize:%p, charCount:%ld,  name(%s) \n", (int)fontData->id, fontData->charSize, fontData->bitmapSizes, fontData->charCount, fontData->name);

	return ITKE_OK;
}

/**
 * Get the glyph bitmap for the given character into the
 * specific 'bitmap' buffer. Return 0 or non-zero for error.
 */
itk_mask_t *mheg_itk_port_ftdGetGlyphBitmap( struct itk_fontdriver_t *thiz,
						itk_fontid_t id, itk_charindex_t ch, itk_uint16_t pointSize, itk_ar_factor_t ar )
{
	itk_fd_mcache_t *slot = NULL;
	PARAM_UNUSED(thiz);

	slot = mheg_itk_port_ftdGetCharFromCache( id, ch, pointSize, ar );
	if( slot == NULL )
	{
		HxLOG_Assert(slot);
		return NULL;
	}

	return slot->mask;
}

static itk_fd_font_attributes_t *mheg_itk_port_ftdInitFont( itk_binaryfont_t newFont, char *name,
		itk_int32_t fontSize, itk_fontid_t id, itk_uint8_t embedded,
		itk_uint8_t type, itk_uint8_t rendered14x9 )
{
	FT_Error res;
	FT_UInt aoutline_resolution;
	FT_UInt ametrics_resolution;
	FT_Fixed ametrics_x_scale;
	FT_Fixed ametrics_y_scale;
	FT_BBox *bb;
	FT_Face ftface;
	itk_fd_font_attributes_t *f;
	int t,sz;
	itk_ar_factor_t ar = { 45, 54 };

	PARAM_UNUSED( embedded );
	PARAM_UNUSED( name );
	PARAM_UNUSED( type );

	HxLOG_Debug("newFont (%s), fontSize (%ld)\n", newFont, fontSize);
	res = FT_New_Memory_Face(ftlib, (FT_Byte *)newFont, fontSize, 0, &ftface);
	if( res != 0 )
	{
		HxLOG_Error("FATAL ERROR: Freetype failed to load font %s (error %d).\n", name, res);
		return NULL;
	}

	f = (itk_fd_font_attributes_t *)mheg_itk_port_ftdAlloc( NULL, sizeof(itk_fd_font_attributes_t) );
	if( f == NULL )
	{
		HxLOG_Assert(f);
		return NULL;
	}

	HxLOG_Debug("alloc (%p)\n", f);
	memset( f, 0, sizeof( itk_fd_font_attributes_t ) );

	f->raw_data = (itk_uint8_t *)newFont;
	f->raw_data_length = fontSize;
	f->face = ftface;

	for( t = 0; t < ftface->num_charmaps; t++ )
	{
		FT_CharMap const * cm = ftface->charmaps;
		FT_CharMapRec const * cmr = cm[t];
		unsigned long const e = cmr->encoding;
		if( e == 0x63696e75 && // "unic"
			cmr->platform_id == 3 && cmr->encoding_id == 1 ) // unicode 16 bit
		{
			HxLOG_Debug( "Selecting unicode 3,1 character map\n" );
			FT_Set_Charmap( ftface, cm[t] );
		}
	}

	// get font metrics
	res =  FT_Get_PFR_Metrics( ftface, &aoutline_resolution, &ametrics_resolution,
		&ametrics_x_scale, &ametrics_y_scale );
	if( res != 0 )
	{
		HxLOG_Debug( "Caxton: WARNING: error getting font metrics for font %s; error %d\n", f->face->family_name, res );
		HxLOG_Debug( "Retrieved values are %d, %d\n", aoutline_resolution, ametrics_resolution );
		//return ITKE_FAIL;
	}

	bb = &ftface->bbox;

	f->info.id = id;
	f->info.name = ftface->family_name;
	f->outline_res = aoutline_resolution;

	if( type == EF_PFR )
	{
		f->info.metricsRes = (itk_int16_t)ametrics_resolution;
		f->info.xMin = (itk_int16_t)(bb->xMin * ametrics_resolution / aoutline_resolution);
		f->info.xMax = (itk_int16_t)(bb->xMax * ametrics_resolution / aoutline_resolution);
		f->info.yMin = (itk_int16_t)(bb->yMin * ametrics_resolution / aoutline_resolution);
		f->info.yMax = (itk_int16_t)(bb->yMax * ametrics_resolution / aoutline_resolution);
		f->type = EF_PFR;
	}
	else
	{
		f->info.metricsRes = (itk_int16_t)aoutline_resolution;
		f->info.xMin = (itk_int16_t)bb->xMin;
		f->info.xMax = (itk_int16_t)bb->xMax;
		f->info.yMin = (itk_int16_t)bb->yMin;
		f->info.yMax = (itk_int16_t)bb->yMax;
		f->type = EF_TTF;
	}

	f->rendered14x9 = rendered14x9;
	f->info.charCount = ftface->num_glyphs;
	f->info.charSize = 1;
	if( ftface->num_glyphs > 255 )
		f->info.charSize = 2;
	if( ftface->num_glyphs > 65535 )
		f->info.charSize = 4;

	f->info.bitmapSizes = NULL;
	f->info.sizesCount = 0;
	if( f->type != EF_TTF )
	{
		sz = ftface->num_fixed_sizes;
		if( sz > 0 )
		{
			itk_uint16_t *sizes;
			sizes = mheg_itk_port_ftdAlloc( NULL, sizeof(itk_uint16_t)*sz );
			if( sizes != NULL )
			{
				HxLOG_Debug("alloc (%p)\n", sizes);
				f->info.bitmapSizes = sizes;
				f->info.sizesCount = (itk_uint16_t)sz;
				for( t = 0; t < ftface->num_fixed_sizes; t++ )
					f->info.bitmapSizes[t] = ftface->available_sizes[t].height;
			}
		}
	}

	mheg_itk_port_ftdSetRenderSize( f, 24, ar ); // default font size
	mheg_itk_port_ftdAddList( &fonts, f );
	return f;
}

static void mheg_itk_port_ftdReleaseFont( itk_fd_font_attributes_t *font )
{
	mheg_itk_port_ftdRemoveList( &fonts, font );
	if( font->info.bitmapSizes != NULL )
	{
		HxLOG_Debug("free (%p)\n", font->info.bitmapSizes);
		mheg_itk_port_ftdFree( NULL, (void *)font->info.bitmapSizes );
	}

	HxLOG_Debug("free (%p)\n", font);
	mheg_itk_port_ftdFree( NULL, (void *)font );
}

/**
 * Add a new font to the list. newFont points to the font binary data.
 * new font's id returned in id. Returns 0 for success, 1 for error.
 */
 itk_errcode_t mheg_itk_port_ftdAddFont( struct itk_fontdriver_t *thiz, itk_binaryfont_t newFont,
											char *name, itk_int32_t fontSize, itk_fontid_t id, itk_font_type_t type )
{
	itk_fd_font_attributes_t *f;
	//itk_uint8_t ftype;
	PARAM_UNUSED( thiz );
	PARAM_UNUSED( type );

	HxLOG_Debug(" newFont(%p), name(%s), fontSize(%ld), id(%p), type(%d)\n", newFont, name, fontSize, id, type);


	if( newFont == NULL || fontSize < 1 || name == NULL || id == NULL )
	{
		HxLOG_Error("ERROR\n");
		return ITKE_FAIL;
	}

#if 0
	switch( type )
	{
		case ITK_FONT_TYPE_PFR:
			ftype = EF_PFR;
			break;
		case ITK_FONT_TYPE_TTF:
			ftype = EF_TTF;
			break;
		case ITK_FONT_TYPE_PFR_COMPRESSED:
		case ITK_FONT_TYPE_TTF_COMPRESSED:
		default:
		{
			ItkAssert(("ERROR\n"));
			return ITKE_FAIL;
		}
	}
#endif
	// Reject TrueType Collection Files as required by freesat spec
	if( fontSize > 4 && newFont[0] == 't' && newFont[1] == 't' && newFont[2] == 'c' && newFont[3] == 'f' )
	{
		//DBG(("FTD: rejecting %s, ttcf file\n", name));
		HxLOG_Error("ERROR\n");
		return ITKE_FAIL;
	}

	f = mheg_itk_port_ftdInitFont( newFont, name, fontSize, id, 0, EF_TTF, 0);
	if( f == NULL )
	{
		HxLOG_Error("ERROR\n");
		return ITKE_FAIL;
	}

	return ITKE_OK;
}

/**
 * Delete the given font.
 */
void mheg_itk_port_ftdRemoveFont( struct itk_fontdriver_t *thiz, itk_fontid_t id )
{
	itk_fd_font_attributes_t *f;

	PARAM_UNUSED( thiz );

	f = (itk_fd_font_attributes_t *)fonts.firstCell;
	while( f != NULL )
	{
		if( f->info.id == id )
		{
			mheg_itk_port_ftdReleaseFont( f );
			break;
		}
		f = (itk_fd_font_attributes_t *)f->nextCell;
	}
	mheg_itk_port_ftdPurgeCaches( id );
}

/* helper functions */

static void mheg_itk_port_ftdSetRenderSize( itk_fd_font_attributes_t *f, itk_int32_t points, itk_ar_factor_t ar )
{
	int hdpi, vdpi, n, subs;
	FT_Bitmap_Size const *sz;

	hdpi = 72;	// defaults 72dpi in both dimensions
	vdpi = 72;

	// set font size
	if( f->rendered14x9 == 1 )	// if font is already 14x9
		hdpi = hdpi * 54 / 45;	// 'undo' existing scaling

	// apply aspect ratio conversion
    	hdpi = (hdpi * ar.numerator / ar.denominator) - 1;

	if( FT_Set_Char_Size( f->face, 0, points * 64, hdpi, vdpi ) != 0 )
	{
		// couldn't match requested size, find a substitute
		subs = 0;
		sz = f->face->available_sizes;
		for( n = 0; n < f->face->num_fixed_sizes; n++ )
		{
			if( sz[n].height < points )
				subs = sz[n].height;
		}
		if( subs == 0 ) // smaller than smallest size, use smallest size
			subs = sz[0].height;

		if( FT_Set_Char_Size( f->face, 0, subs * 64, hdpi, vdpi ) != 0 )
		{
           HxLOG_Debug( "Caxton: FATAL ERROR: Failed to set fallback font size!\n" );
		}
	}
}

itk_errcode_t mheg_itk_port_ftdAddStaticFont( struct itk_fontdriver_t *thiz, itk_binaryfont_t newFont,
													    char *name, itk_int32_t fontSize, itk_fontid_t id, itk_font_type_t type )
{
    UNUSED(thiz);
    if( newFont == NULL || fontSize < 1 || name == NULL || id == NULL || MAX_STATIC_FONT == ef_staticFontCount)
        return ITKE_FAIL;

    //todo correct render_14x9 field below.
    ef_data[ef_staticFontCount] = newFont;
    ef_names[ef_staticFontCount] = name;
    ef_type[ef_staticFontCount] = type;
    ef_render_14x9[ef_staticFontCount] = 1;
    ef_size[ef_staticFontCount] = fontSize;
    idlist[ef_staticFontCount] = id;

    ef_staticFontCount++;
    return ITKE_OK;
}
struct itk_static_font_list_t mheg_itk_port_ftdGetStaticFonts( struct itk_fontdriver_t *thiz )
{
	struct itk_static_font_list_t fl;
	PARAM_UNUSED( thiz );
	fl.font_count = (itk_uint16_t)ef_staticFontCount;
	fl.font_urls = ef_names;
	fl.font_ids = idlist;
	return fl;
}

/*
* setup call from font_manager
*/
#if 0			// not used
static itk_fd_font_attributes_t *local_itk_FONTDRIVER_getFontByURL( char *url )
{
	itk_fd_font_attributes_t *f;
	f = (itk_fd_font_attributes_t *)fonts.firstCell;
	while( f != NULL )
	{
		if( strcmp( f->info.name, url ) == 0 )
			return f;
		f = (itk_fd_font_attributes_t *)f->nextCell;
	}
	return NULL;
}
#endif

static itk_fd_font_attributes_t *mheg_itk_port_ftdGetFontByID( itk_fontid_t id )
{
	unsigned int n;
	itk_fd_font_attributes_t *f;
	f = (itk_fd_font_attributes_t *)fonts.firstCell;

	if( f->info.id == NULL && idlist[0] != NULL )
	{
		// ids have been assigned, copy them in!
		for( n = 0; n < ef_staticFontCount; n++ )
		{
			f->info.id = idlist[n];
			f = (itk_fd_font_attributes_t *)f->nextCell;
		}
		f = (itk_fd_font_attributes_t *)fonts.firstCell;
	}

	while( f != NULL )
	{
		if( f->info.id == id )
			return f;
		f = (itk_fd_font_attributes_t *)f->nextCell;
	}
	return NULL;
}

/*** pipeline/mask memory handling ***/

static itk_mask_t *mheg_itk_port_ftdCreateMask( const FT_Bitmap *bm, int depth )
{
	itk_mask_t *mask;
	int x,r;
	itk_uint8_t byte, bit, pixel;

	// zero size character?
	if( bm->width < 1 || bm->rows < 1 )
	{
		HxLOG_Error(" Size ERROR! width(%d) rows(%d)\n", bm->width, bm->rows);
		return NULL;
	}

	HxLOG_Debug(" +++ bm->width(%d) .rows(%d) depth(%d)\n", bm->width, bm->rows, depth);

	mask = thePipe->createMask( thePipe, bm->width, bm->rows, depth );
	if( mask == NULL )
	{
		HxLOG_Error(" Create ERROR\n");
		return NULL;
	}

	HxLOG_Debug(" +++ mask(%p)->depth(%ld) depth(%ld) mask->pitch(%ld) bm->pitch(%ld) bm->pixel_mode(%ld)\n", mask, mask->depth, depth, mask->pitch, bm->pitch, bm->pixel_mode);

	// do we have to convert the bitmap?
	if( mask->depth == depth && mask->pitch == bm->pitch )
	{
		// no conversion
		memcpy( mask->mask, bm->buffer, bm->rows * bm->pitch );
		return mask;
	}

	// same depth, but different pitch?
	if( mask->depth == depth && mask->pitch != bm->pitch )
	{
		// copy using the smaller of the pitches. If this truncates data, tough luck.
		for( r = 0; r < mask->height; r++ )
		{
			if( mask->pitch > bm->pitch )
				memcpy( &(mask->mask[ r * mask->pitch ]), &(bm->buffer[ r * bm->pitch ]), bm->pitch );
			else
				memcpy( &(mask->mask[ r * mask->pitch ]), &(bm->buffer[ r * bm->pitch ]), mask->pitch );
		}
		return mask;
	}

	// Depth must be different. This should always be an upconvert.
	if( mask->depth != depth && bm->pixel_mode != FT_PIXEL_MODE_MONO )
	{
		HxLOG_Error(" Depth ERROR\n");
		return NULL;  // can't do down-conversion.
	}

	for( r = 0; r < bm->rows; r++ )
	{
		for( x = 0; x < bm->width; x++ )
		{
			byte = (itk_uint8_t)(x / 8);
			bit = (itk_uint8_t)(7-(x % 8));
			pixel = bm->buffer[ r * bm->pitch + byte ] & 1 << bit;
			mask->mask[ r * mask->pitch + x ] = (pixel == 0 ? 0 : 255);
		}
	}

	return mask;
}

static void mheg_itk_port_ftdReleaseMask( itk_mask_t *mask )
{
	HxLOG_Debug(" +++ mask(%p)\n", mask);

	if( mask != NULL )
	{
		thePipe->releaseMask( thePipe, mask );
	}
}


/* metrics and mask cache ----------------------------------------------- */

/**
 * Remove all entries related to the specified font id from the character
 * and bitmap caches. This is called when a downloaded font is removed so
 * that if the font id is reused in future, invalid data is not pulled from
 * the cache.
 */
static void mheg_itk_port_ftdPurgeCaches( itk_fontid_t id )
{
	struct itk_fd_mcache_t *entry, *next;
	int n;

	// purge metrics cache
	entry = (struct itk_fd_mcache_t *)m_cache.firstCell;
	while( entry != NULL )
	{
		next = (struct itk_fd_mcache_t *)entry->nextCell;
		if( entry->font_id == id )
		{
			mheg_itk_port_ftdRemoveList( &m_cache, entry );
			if( entry->mask != NULL )
				mheg_itk_port_ftdReleaseMask( entry->mask );
			HxLOG_Debug("free (%p)\n", entry);
			mheg_itk_port_ftdFree( NULL, entry );
		}
		entry = next;
	}

	// purge character map cache
	for( n = 0; n < CMCACHE_SIZE; n++ )
	{
		if( cmap_cache[n].index == CMCACHE_EMPTY )
			break; // not found, out of entries

		while( cmap_cache[n].font_id == id  )
		{
			// swap last entry into current entry slot, wipe last entry
			if(cmcache_next == 0)
				break;

			if( cmcache_next > 0 )
			{
				cmap_cache[n].index = cmap_cache[cmcache_next-1].index;
				cmap_cache[n].unicode = cmap_cache[cmcache_next-1].unicode;
				cmap_cache[n].font_id = cmap_cache[cmcache_next-1].font_id;
				cmap_cache[n].access_count = cmap_cache[cmcache_next-1].access_count;
			}
			cmap_cache[cmcache_next-1].index = CMCACHE_EMPTY;
			cmap_cache[cmcache_next-1].unicode  = 0;
			cmap_cache[cmcache_next-1].font_id = NULL;
			cmap_cache[cmcache_next-1].access_count = 0;
			cmcache_next--;
		}
	}
}

/**
 * Check if the given character is in the cache and return the entry if so.
 * Otherwise, render it, fill in the cache entry details and return it.
 */
static struct itk_fd_mcache_t *mheg_itk_port_ftdGetCharFromCache( itk_fontid_t id, itk_charindex_t ch,
		itk_uint16_t pointSize, itk_ar_factor_t ar )
{
	FT_Error err;
	FT_GlyphSlot glyph = NULL;
	FT_Bitmap const *bm = NULL;
	struct itk_fd_mcache_t *entry, *first, *prev, *prevprev;
	struct itk_mask_t *nmask = NULL;
	int newEntry;
	itk_int32_t pwidth;
	itk_fd_font_attributes_t *f;
	itk_uint16_t arf;

//	DBG(("charFromCache: Looking for %p, %d, %d, %d/%d\n",id,ch,pointSize,ar.numerator,ar.denominator));
	// Check if already rendered
	arf = (itk_uint16_t)(ar.numerator * 100 + ar.denominator);
	first = entry = (struct itk_fd_mcache_t *)m_cache.firstCell;
	prevprev = prev = NULL;
	while( entry != NULL )
	{
		if( entry->font_id == id && entry->index == ch &&
			( (entry->pointSize == pointSize && entry->ar == arf ) ||
			pointSize == 0 ) )
		{
			if( entry != first )
			{
				entry->access_count++;
				if( prev != NULL && entry->access_count > prev->access_count )
				{
					if( prevprev != NULL )
						prevprev->nextCell = (itk_fd_list_entry_t *)entry;
					prev->nextCell = entry->nextCell;
					entry->nextCell = (itk_fd_list_entry_t *)prev;
					if( prev == first )
						m_cache.firstCell = (itk_fd_list_entry_t *)entry;
				}
			}
			return entry;
		}
	        prevprev = prev;
		prev = entry;
	        entry = (itk_fd_mcache_t *)entry->nextCell;
	}

	// Ok... so we must render it.
	//DBG(("charFromCache: Not found in cache! Rendering.\n"));
	// get freetype to render the character
	f = mheg_itk_port_ftdGetFontByID( id );
	if( pointSize != 0 )
	{
		itk_unicode_t uc = mheg_itk_port_ftdIndex2Unicode( NULL, id, ch );

		// render at "real" size for metrics
		mheg_itk_port_ftdSetRenderSize( f, pointSize, ar );

		if( uc == 0xE3 || uc == 0xF5 || uc == 0xF1 ) // auto-hinting doesn't work on these characters
		{
			if( bpp == 1 )
				err = FT_Load_Glyph(f->face, ch, RENDER_FLAGS_MONO | FT_LOAD_NO_HINTING );
			else
				err = FT_Load_Glyph(f->face, ch, RENDER_FLAGS_8BIT | FT_LOAD_NO_HINTING );
		}
		else
		{
			if( bpp == 1 )
				err = FT_Load_Glyph(f->face, ch, RENDER_FLAGS_MONO );
			else
				err = FT_Load_Glyph(f->face, ch, RENDER_FLAGS_8BIT );
		}

		if( err != 0 )
		{
			HxLOG_Error( "freetype font driver: Failed to render glyph %d\n", ch );
			return NULL;
		}

		glyph = f->face->glyph;
		bm = &(glyph->bitmap);

		// look and see if there's a logical entry we can fill in
		entry = (struct itk_fd_mcache_t *)m_cache.firstCell;
		while( entry != NULL )
		{
			if( entry->font_id == id && entry->index == ch && entry->pointSize == 0 )
			{
				//DBG(("charFromCache: Reusing logical entry for physical data\n"));
				break;
			}
			else
			{
				entry = (itk_fd_mcache_t *)entry->nextCell;
			}
		}
	}

	newEntry = 0;
	if( entry == NULL )
	{
		//DBG(("charFromCache: Allocating new entry\n" ));
		entry = (struct itk_fd_mcache_t *)mheg_itk_port_ftdAlloc( NULL, sizeof(struct itk_fd_mcache_t) );
		if( entry == NULL )
			return NULL;
		memset( entry, 0, sizeof(struct itk_fd_mcache_t) );
		newEntry = 1;
		entry->nextCell = NULL;
	}

	entry->font_id = id;
	entry->index = ch;
	entry->pointSize = pointSize;
	entry->ar = arf;
	entry->mask = NULL;

	if( pointSize != 0 )
	{
		entry->pmetrics.setWidth = (itk_int16_t)(glyph->metrics.horiAdvance * 4);
		entry->pmetrics.xPos = glyph->bitmap_left;
		entry->pmetrics.xSize = glyph->bitmap.width;
		entry->pmetrics.yPos = glyph->bitmap_top - glyph->bitmap.rows;
		entry->pmetrics.ySize = glyph->bitmap.rows;
		nmask = mheg_itk_port_ftdCreateMask( bm, bpp );
		entry->mask = nmask;

		// adapt for hinting creating too much spacing
		//DBG(( "Character %d (%d) - LSB: %d  RSB %d  XPOS %d  WID %d\n",ch,pointSize,glyph->lsb_delta, glyph->rsb_delta, entry->pmetrics.xPos, entry->pmetrics.setWidth ));
		// check for "extra" pixels
		if( glyph->lsb_delta < -29 )
		{
			entry->pmetrics.xPos -= 1;  // shift glyph left 1 pixel
			entry->pmetrics.setWidth -= 256;
		}
		if( glyph->rsb_delta > 30 )
		{
			entry->pmetrics.setWidth -= 256;
		}
	}

	if( newEntry == 1 )
	{
		if( f->type == EF_PFR )
		{
			FT_Pos advance = 0;
			// PFR off-by-1 error in Freetype
			if( FT_Get_PFR_Advance( f->face, ch-1, &advance ) != 0 )
				advance = 0;
			entry->log_width = advance;
		}
		else
		{
			if( FT_Load_Glyph( f->face, ch, FT_LOAD_NO_SCALE ) != 0 )
			{
				mheg_itk_port_ftdReleaseMask( entry->mask );
				mheg_itk_port_ftdFree( NULL, entry );
				return NULL;
			}
			entry->log_width = f->face->glyph->metrics.horiAdvance;
			//DBG(("LW: %d - %d\n",ch,entry->log_width));
		}

		if( m_cache.count == MCACHE_MAX )
		{
			// if the cache is full, dump the last 30% of entries. Most frequently
			// accessed entries are toward the front.
			while( m_cache.count > (MCACHE_MAX / 3) * 2 )
			{
				struct itk_fd_mcache_t *old = (struct itk_fd_mcache_t *)mheg_itk_port_ftdFindLastCell( &m_cache );
				mheg_itk_port_ftdRemoveList( &m_cache, old );
				if( old->mask != NULL )
					mheg_itk_port_ftdReleaseMask( old->mask );
				mheg_itk_port_ftdFree( NULL, old );
			}
		}
	        mheg_itk_port_ftdAddList( &m_cache, entry );
	}

	if( pointSize != 0 )
	{
		// we definitely have a logical width at this point
		// compare logical width with physical for additional hinting correction
		pwidth = entry->log_width * pointSize  * ar.numerator / ( f->outline_res * ar.denominator );
		pwidth *= 256;
		if( entry->pmetrics.setWidth - pwidth > 128 )
		{
			entry->pmetrics.setWidth -= 256;
		}
		else if( pwidth - entry->pmetrics.setWidth > 128 )
		{
			entry->pmetrics.setWidth += 256;
		}
	}

	return entry;
}

// link list support -------------------------------------------------
static void mheg_itk_port_ftdInitList( itk_fd_linked_list_t *list )              //initialise linked list
{
    list->count = 0;
    list->firstCell = NULL;
}

static void mheg_itk_port_ftdAddList( itk_fd_linked_list_t *list, void *item )     //add item to linked list
{
    itk_fd_list_entry_t *entry = (itk_fd_list_entry_t *)item;
    if( list != NULL && item != NULL )
    {
       entry->nextCell = list->firstCell;
       list->firstCell = entry;
       list->count++;
    }
}

static void mheg_itk_port_ftdRemoveList( itk_fd_linked_list_t *list, void *item )  //remove item from linked list
{
	itk_fd_list_entry_t *l, *p;
	itk_fd_list_entry_t *entry = (itk_fd_list_entry_t *)item;

	if( list == NULL || item == NULL )
		return;

	// deleting first entry?
	if( list->firstCell == entry )
	{
		list->firstCell = entry->nextCell;
		list->count--;
		return;
	}

	l = list->firstCell;
	p = NULL;
	while( l != NULL )
	{
		if( l == entry )
		{
			if (p != NULL)
				p->nextCell = entry->nextCell;
			entry->nextCell = NULL;
			list->count--;
			return;
		}
		p = l;
		l = l->nextCell;
	}
}

static itk_fd_list_entry_t *mheg_itk_port_ftdFindLastCell( itk_fd_linked_list_t *list )   // find last item of linked list
{
    itk_fd_list_entry_t *le;

    if( list == NULL )
        return NULL;

    le = list->firstCell;
    while( le->nextCell != NULL )
        le = le->nextCell;

    return le;
}

#define _____PUBLIC_APIs________________________________________________________________

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//----- ITK FONTDRIVER API -----END--------------------------------------------//
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
void PORT_ITK_FONTDRIVER_CreateObject(void)
{
	s_stFontdriverObj.initialise = mheg_itk_port_ftdInitialize;
	s_stFontdriverObj.shutdown = mheg_itk_port_ftdShutdown;
	s_stFontdriverObj.getStaticFonts = mheg_itk_port_ftdGetStaticFonts;
	s_stFontdriverObj.unicodeToIndex = mheg_itk_port_ftdUnicode2Index;
	s_stFontdriverObj.indexToUnicode = mheg_itk_port_ftdIndex2Unicode;
	s_stFontdriverObj.getLogicalWidth = mheg_itk_port_ftdGetLogicalSize;
	s_stFontdriverObj.getLogicalKerning = mheg_itk_port_ftdGetLogicalKerning;
	s_stFontdriverObj.getPhysicalMetrics = mheg_itk_port_ftdGetPhysicalMetrics;
	s_stFontdriverObj.getFontData = mheg_itk_port_ftdGetFontData;
	s_stFontdriverObj.getGlyphBitmap = mheg_itk_port_ftdGetGlyphBitmap;
	s_stFontdriverObj.addFont = mheg_itk_port_ftdAddFont;
	s_stFontdriverObj.removeFont = mheg_itk_port_ftdRemoveFont;

	ITK_registerManager(ITK_FONT, &s_stFontdriverObj);
}


