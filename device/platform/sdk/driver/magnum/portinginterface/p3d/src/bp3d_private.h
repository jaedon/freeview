/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d_private.h $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 8/12/10 6:22p $
 *
 * Module Description: Private 3D Rendering Header File
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7405/bp3d_private.h $
 * 
 * Hydra_Software_Devel/27   8/12/10 6:22p nissen
 * SW7405-4720: Added context to macro for getting tiles.
 * 
 * Hydra_Software_Devel/26   3/1/10 11:15a nissen
 * SW7405-3704: Added use of memory block out when rendering with a depth
 * buffer for 3548/3556.
 * 
 * Hydra_Software_Devel/25   1/4/10 5:22p nissen
 * SW7405-3704: Added use of memory block out when rendering with a depth
 * buffer to avoid possible hang on 7405.
 * 
 * Hydra_Software_Devel/24   8/12/09 1:23p nissen
 * PR 35853: Fixed binning.
 * 
 * Hydra_Software_Devel/23   8/4/09 8:25p nissen
 * PR 57173: Fixed coverity defect involving context handle.
 * 
 * Hydra_Software_Devel/22   8/4/09 3:47p nissen
 * PR 35853: Added initial support for binning instead of tiling.
 * 
 * Hydra_Software_Devel/21   7/4/09 5:06p nissen
 * PR 56587: Modified function definitions for freeing context resources.
 * 
 * Hydra_Software_Devel/20   1/23/09 10:04a nissen
 * PR 41229: Increased default tile size to improve performance.
 * 
 * Hydra_Software_Devel/19   8/1/08 9:29p nissen
 * PR 41229: Fixed warning.
 * 
 * Hydra_Software_Devel/18   8/1/08 9:16p nissen
 * PR 41229: Added timer fix for tile height.
 * 
 * Hydra_Software_Devel/17   7/15/08 1:51p nissen
 * PR 41229: Added support for configuring tiles and for timing renders.
 * 
 * Hydra_Software_Devel/16   6/16/08 8:34p nissen
 * PR 42507: Added support for rendering with multiple contexts.
 * 
 * Hydra_Software_Devel/15   9/24/07 9:54a nissen
 * PR 30062: Fixed opaque bit when using depth mask.
 * 
 * Hydra_Software_Devel/14   12/7/06 11:58a nissen
 * PR 26403: Added support for clearing the alpha channel of the render
 * target to a specified value.
 * 
 * Hydra_Software_Devel/13   9/22/06 10:59a nissen
 * PR 22858: Fixed problem when rendering without primitives or clears but
 * with a source color buffer.
 * 
 * Hydra_Software_Devel/12   9/20/06 2:03p nissen
 * PR 22858: Added support for polygon offset through a depth scaling
 * factor used to offset a primitive's depth values.
 * 
 * Hydra_Software_Devel/11   6/5/06 11:14a nissen
 * PR 20908: Added support for preparing and flushing render and checking
 * if a render is busy. Added support for swizzling textures.
 * 
 * Hydra_Software_Devel/10   10/5/05 9:49p nissen
 * PR 17273: Added support for cached memory.
 * 
 * Hydra_Software_Devel/9   9/7/04 3:03p nissen
 * PR 9132: Added extra parameter to interrupt callback.
 * 
 * Hydra_Software_Devel/8   8/10/04 4:27p nissen
 * PR 9132: Added function definitions.
 * 
 * Hydra_Software_Devel/7   7/27/04 11:06p nissen
 * PR 9132: Added support for addressing textures above 64 MB.
 * 
 * Hydra_Software_Devel/6   4/30/04 1:25p nissen
 * PR 10548: Added use of BCHP_MASK and BCHP_SHIFT macros.
 * 
 * Hydra_Software_Devel/5   4/30/04 11:22a nissen
 * PR 9132: Updated structures and function definitions.
 * 
 * Hydra_Software_Devel/4   3/10/04 1:42p nissen
 * PR 9132: Fixed macro that sets a primheader address.
 * 
 * Hydra_Software_Devel/3   3/5/04 2:59p nissen
 * PR 9132: Updated function definitions, structures and macros.
 * 
 * Hydra_Software_Devel/2   2/20/04 4:17p nissen
 * PR 9132: More implementation.
 * 
 * Hydra_Software_Devel/1   1/22/04 2:56p nissen
 * PR 9132: Initial version.
 * 
 ***************************************************************************/

#ifndef BP3D_PRIVATE_H__
#define BP3D_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*#define BP3D_P_ISR_TIMER*/
/*#define BP3D_P_PX3D_TIMER*/

/***************************************************************************/
/*#define BP3D_P_USE_BINS*/

#define BP3D_P_REG_COUNT                    24
#define BP3D_P_TEXTURE_SIZE_MIN             8
#define BP3D_P_TEXTURE_SIZE_MAX             1024
#define BP3D_P_PRIMITIVE_PER_SCENE          65535
#define BP3D_P_PRIMITIVE_PER_SCAN           64

#ifdef BP3D_P_USE_BINS
#define BP3D_P_PRIMITIVE_PER_LAYER          128
#else
#define BP3D_P_PRIMITIVE_PER_LAYER          64
#endif

#define BP3D_P_PRIMITIVE_PER_BIN            255
#define BP3D_P_PRIMITIVE_ADDR_SHIFT         1
#define BP3D_P_PRIMITIVE_SIZE               128
#define BP3D_P_BLOCK_SIZE                   32768
#define BP3D_P_BLOCK_LIFE                   256
#define BP3D_P_BLOCK_ALIGN                  7
#define BP3D_P_PRIM_BLOCK_COUNT             (((BP3D_P_PRIMITIVE_PER_SCENE + 1) * BP3D_P_PRIMITIVE_SIZE) / BP3D_P_BLOCK_SIZE)
#define BP3D_P_PRIM_PER_BLOCK               (BP3D_P_BLOCK_SIZE / BP3D_P_PRIMITIVE_SIZE)
#define BP3D_P_LIST_BLOCK_COUNT             BP3D_P_PRIM_BLOCK_COUNT
#define BP3D_P_SCENE_WIDTH_MAX              2048
#define BP3D_P_SCENE_HEIGHT_MAX             2048
#define BP3D_P_COLOR_KEY_COUNT              3
#define BP3D_P_LOG4_CONSTANT                0.7213475204445f

#ifdef BP3D_P_USE_BINS
#define BP3D_P_TILE_WIDTH                   512
#define BP3D_P_TILE_HEIGHT                  2048
#else
#if BP3D_P_ISR_TIMER || BP3D_P_PX3D_TIMER
#define BP3D_P_TILE_WIDTH                   256
#define BP3D_P_TILE_HEIGHT                  1536
#else
#define BP3D_P_TILE_WIDTH                   256
#define BP3D_P_TILE_HEIGHT                  256
#endif
#endif

#if (BCHP_CHIP==7405)
#define BP3D_P_MEMC_0_1_CLIENT_INFO(ff)     BCHP_MEMC_0_1_CLIENT_INFO_51##ff
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
#define BP3D_P_MEMC_0_1_CLIENT_INFO(ff)     BCHP_MEMC_0_1_CLIENT_INFO_25##ff
#endif

/***************************************************************************/
#define BCHP_PX3D_Mode_BufferClear_EmptyEmpty                      0
#define BCHP_PX3D_Mode_BufferClear_BlackZFar                       1
#define BCHP_PX3D_Mode_BufferClear_BackgroundColorZFar             2
#define BCHP_PX3D_Mode_BufferClear_EmptyZFar                       3
#define BCHP_PX3D_DestConfig_ColorDstFormat_ARGB8888               1
#define BCHP_PX3D_DestConfig_ColorDstFormat_RGB565                 2
#define BCHP_PX3D_DestConfig_ColorDstFormat_ARGB1555               3
#define BCHP_PX3D_DestConfig_ColorDstFormat_ARGB4444               4
#define BCHP_PX3D_SourceConfig_ColorSrcFormat_ARGB8888             1
#define BCHP_PX3D_SourceConfig_ColorSrcFormat_RGB565               2
#define BCHP_PX3D_SourceConfig_ColorSrcFormat_ARGB1555             3
#define BCHP_PX3D_SourceConfig_ColorSrcFormat_ARGB4444             4
#define BCHP_PX3D_DestConfig_ZDstFormat_ARGB8888                   1
#define BCHP_PX3D_DestConfig_ZDstFormat_RGB565                     2
#define BCHP_PX3D_DestConfig_ZDstFormat_ARGB1555                   3
#define BCHP_PX3D_DestConfig_ZDstFormat_ARGB4444                   4
#define BCHP_PX3D_SourceConfig_ZSrcFormat_ARGB8888                 1
#define BCHP_PX3D_SourceConfig_ZSrcFormat_RGB565                   2
#define BCHP_PX3D_SourceConfig_ZSrcFormat_ARGB1555                 3
#define BCHP_PX3D_SourceConfig_ZSrcFormat_ARGB4444                 4
#define BCHP_PX3D_TranslateColorKey_Color_MASK                     0xffffffff
#define BCHP_PX3D_TranslateColorKey_Color_SHIFT                    0
#define BCHP_PX3D_FogColor_Color_MASK                              0xffffffff
#define BCHP_PX3D_FogColor_Color_SHIFT                             0
#define BCHP_PX3D_ColorKey1_Color_MASK                             0xffffffff
#define BCHP_PX3D_ColorKey1_Color_SHIFT                            0
#define BCHP_PX3D_ColorKey2_Color_MASK                             0xffffffff
#define BCHP_PX3D_ColorKey2_Color_SHIFT                            0
#define BCHP_PX3D_ColorKey3_Color_MASK                             0xffffffff
#define BCHP_PX3D_ColorKey3_Color_SHIFT                            0

/***************************************************************************/
#define BCHP_PX3D_Type_PrimHeaderA_TexelFormat_ARGB1555            0
#define BCHP_PX3D_Type_PrimHeaderA_TexelFormat_RGB565              1
#define BCHP_PX3D_Type_PrimHeaderA_TexelFormat_ARGB4444            2
#define BCHP_PX3D_Type_PrimHeaderA_TexelFormat_ARGB8888            3
#define BCHP_PX3D_Type_PrimHeaderA_TexelFormat_Luma8               4
#define BCHP_PX3D_Type_PrimHeaderA_TexelFormat_XRGB8888            5
#define BCHP_PX3D_Type_PrimHeaderA_TexelFormat_Y0CbY1Cr            6
#define BCHP_PX3D_Type_PrimHeaderA_TexelFormat_LumaAlpha88         7
#define BCHP_PX3D_Type_PrimHeaderA_PrimitiveType_Point             0
#define BCHP_PX3D_Type_PrimHeaderA_PrimitiveType_Line              1
#define BCHP_PX3D_Type_PrimHeaderA_PrimitiveType_Triangle          2
#define BCHP_PX3D_Type_PrimHeaderA_PrimitiveType_Rectangle         3

/***************************************************************************/
#define BCHP_PX3D_Type_PrimHeaderA_Fog_None                        0
#define BCHP_PX3D_Type_PrimHeaderA_Fog_Vertex                      1
#define BCHP_PX3D_Type_PrimHeaderA_Fog_DepthTable                  2
#define BCHP_PX3D_Type_PrimHeaderA_Fog_VertexTable                 3

/***************************************************************************/
#define BP3D_P_PX3D_Mode                                           0
#define BP3D_P_PX3D_TopLeft                                        1
#define BP3D_P_PX3D_BottomRight                                    2
#define BP3D_P_PX3D_DestConfig                                     3
#define BP3D_P_PX3D_SourceConfig                                   4
#define BP3D_P_PX3D_DisplayListConfig                              5
#define BP3D_P_PX3D_TextureConfig                                  6
#define BP3D_P_PX3D_DisplayListAddr                                7
#define BP3D_P_PX3D_DisplayPageAddr                                8
#define BP3D_P_PX3D_DestAddr                                       9
#define BP3D_P_PX3D_DestZAddr                                     10
#define BP3D_P_PX3D_SourceAddr                                    11
#define BP3D_P_PX3D_SourceZAddr                                   12
#define BP3D_P_PX3D_TranslateColorKey                             13
#define BP3D_P_PX3D_ColorKey1                                     14
#define BP3D_P_PX3D_ColorKey2                                     15
#define BP3D_P_PX3D_ColorKey3                                     16
#define BP3D_P_PX3D_FogColor                                      17
#define BP3D_P_PX3D_AlphaReference                                18
#define BP3D_P_PX3D_IRQMask                                       19
#define BP3D_P_PX3D_ExceptionAction                               20
#define BP3D_P_PX3D_ExceptionStatus                               21
#define BP3D_P_PX3D_NextRegTableAddr                              22
#define BP3D_P_PX3D_PrimCount                                     23

/***************************************************************************/
typedef struct BP3D_P_Rect
{
	uint16_t usMinX;
	uint16_t usMinY;
	uint16_t usMaxX;
	uint16_t usMaxY;
}
BP3D_P_Rect;

/***************************************************************************/
typedef struct BP3D_P_Vertex
{
	float fX;                   /* x coordinate */
	float fY;                   /* y coordinate */
	float fZ;                   /* z coordinate */
	float fW;                   /* w value */
	uint32_t ulDiffuse;         /* diffuse color in ARGB8888 */
	uint32_t ulSpecular;        /* specular color in FRGB888, F=Fog */
	float fS;                   /* s texture coordinate */
	float fT;                   /* t texture coordinate */
	float fM;                   /* mipmap lod */
}
BP3D_P_Vertex;

/***************************************************************************/
typedef struct BP3D_P_PrimHeader
{
	uint32_t ulPrimHeaderA;
	uint32_t ulPrimHeaderB;
	uint32_t ulTextureAddr;
}
BP3D_P_PrimHeader;

/***************************************************************************/
typedef struct BP3D_P_Primitive
{
	BP3D_P_PrimHeader Header;
	BP3D_P_Vertex v0;
	BP3D_P_Vertex v1;
	BP3D_P_Vertex v2;
	uint16_t usMinX;
	uint16_t usMaxX;
	uint16_t usMinY;
	uint16_t usMaxY;
}
BP3D_P_Primitive;

/***************************************************************************/
typedef struct
{
	uint8_t ucStartCountLeft;
	uint8_t ucStopCountLeft;
	uint8_t ucStartCountRight;
	uint8_t ucStopCountRight;
}
BP3D_P_ScanData;

/***************************************************************************/
typedef struct BP3D_P_Layer BP3D_P_Layer;
struct BP3D_P_Layer
{
	BP3D_P_ScanData aScanData[BP3D_P_TILE_HEIGHT + 4];
	uint8_t aucNodeIndicesLeft[BP3D_P_TILE_HEIGHT + 4];
	uint8_t aucNodeIndicesRight[BP3D_P_TILE_HEIGHT + 4];
	uint32_t aulNodes[BP3D_P_PRIMITIVE_PER_LAYER + 1];
	uint32_t ulNodeCount;
	BP3D_P_Layer *pNextLayer;
};

/***************************************************************************/
typedef struct 
{
	BP3D_P_Layer *pFirstLayer;
	BP3D_P_Layer *pCurrLayer;
	uint8_t aucPrimPerScanLeft[BP3D_P_TILE_HEIGHT + 4];
	uint8_t aucPrimPerScanRight[BP3D_P_TILE_HEIGHT + 4];
	uint32_t ulPrimCountLeft[2];
	uint32_t ulPrimCountRight[2];
	uint32_t ulMergeCount;
	uint32_t ulRenderCount;
}
BP3D_P_Tile;

/***************************************************************************/
typedef struct BP3D_P_Block BP3D_P_Block;
struct BP3D_P_Block
{
	BP3D_P_Block *pNextBlock;
	void *pvMemory;
	void *pvCached;
	uint32_t ulOffset;
	volatile uint32_t ulNum;
};

/***************************************************************************/
typedef struct 
{
	void *pvData;
	BPXL_Format eFormat;
	uint32_t ulOffset;
	uint32_t ulPitch;
	uint32_t ulBpp;
}
BP3D_P_Buffer;

/***************************************************************************/
typedef struct BP3D_P_Context
{
	/* user callback information */
	BKNI_EventHandle hInterruptEvent;           /* handle to interrupt event */
	BP3D_Callback pUserCallback;                /* pointer to user interrupt callback */
	void *pUserParam1;                          /* pointer to user data */
	int iUserParam2;                            /* user data */

	/* register */
	uint32_t aulRegs[BP3D_P_REG_COUNT];         /* array of PX3D registers */
	uint32_t ulRegSetOffset;                    /* offset of first register set in list */
	uint32_t ulRegSetCount;                     /* count of register sets */
	uint32_t *pulPrevRegSet;                    /* previous register set for making tile list */

	/* memory block information */
	BP3D_P_Block *pFirstBlock;                  /* first block in memory block list */
	BP3D_P_Block *pLastBlock;                   /* last block in memory block list */

	/* scene information */
	BSUR_Surface_Handle hDstColorSurface;       /* handle to destination color surface */
	BSUR_Surface_Handle hDstDepthSurface;       /* handle to destination depth surface */
	BSUR_Surface_Handle hSrcColorSurface;       /* handle to source color surface */
	BSUR_Surface_Handle hSrcDepthSurface;       /* handle to source depth surface */
	BP3D_P_Buffer DstColorBuffer;               /* destination color buffer structure */
	BP3D_P_Buffer DstDepthBuffer;               /* destination depth buffer structure */
	BP3D_P_Buffer SrcColorBuffer;               /* source color buffer structure */
	BP3D_P_Buffer SrcDepthBuffer;               /* source depth buffer structure */
	BP3D_P_Rect Viewport;                       /* viewport rectangle */
	uint32_t ulBufferOffsetX;                   /* x offset for buffer addresses */
	uint32_t ulBufferOffsetY;                   /* y offset for buffer addresses */
	uint32_t ulPrimCount;                       /* current count of primitives in scene */
	volatile uint32_t ulSceneNum;               /* current scene number */
	volatile uint32_t ulRenderNum;              /* current render number */
	bool bRenderPrepared;

	/* scene state */
	uint32_t ulColorKeyCount;                   /* scene color key count */
	uint32_t ulColorKey[BP3D_P_COLOR_KEY_COUNT];/* scene color key array */
	BP3D_Fog eFog;                              /* scene fog table type */
	uint32_t ulFogColor;                        /* scene fog color */
	uint8_t *pucFogTable;                       /* scene fog table */
	bool bFogTable;                             /* indicates if fog table is used in render */
	bool bClearColor;                           /* indicates color clear before render */
	bool bClearDepth;                           /* indicates depth clear before render */

	/* special primitives */
	BP3D_P_Primitive *pEmptyPoint;              /* pointer to empty point primitive */
	BP3D_P_Primitive *pColorRectangle;          /* pointer to color clear rectangle primitive */
	BP3D_P_Primitive *pDepthRectangle;          /* pointer to depth clear rectangle primitive */
	uint32_t ulEmptyPointOffset;                /* offset of empty point primitive */
	uint32_t ulColorRectangleOffset;            /* offset of color clear rectangle primitive */
	uint32_t ulDepthRectangleOffset;            /* offset of depth clear rectangle primitive */

	/* primitive state */
	BP3D_P_PrimHeader PrimHeader;               /* current primitive header */
	BP3D_Shade ePrimitiveShade;                 /* primitive shading */
	BP3D_Cull ePrimitiveCull;                   /* primitive culling */
	BSUR_Surface_Handle hTextureSurface;        /* handle to texture surface */
	uint32_t ulTextureWidth;                    /* texture width */
	uint32_t ulTextureHeight;                   /* texture height */
	float fDepthFactorScale;                    /* depth factor scale */
	float fMipmapLodMax;                        /* mipmap level-of-detail max */
	float fMipmapLodBias;                       /* mipmap level-of-detail bias */
	bool bMipmap;                               /* enable mipmaping */

	/* primitive block information */
	void *apvPrimBlocks[BP3D_P_PRIM_BLOCK_COUNT];     /* array of pointers to primitive blocks */
	uint32_t aulPrimOffsets[BP3D_P_PRIM_BLOCK_COUNT]; /* array of pointers to primitive block offsets */
	uint32_t ulPrimBlockPos;                    /* current position within primitive blocks */
	uint32_t ulPrimBlockCount;                  /* count of primitive blocks */
	uint32_t ulPrimBlockFree;                   /* total free size of primitive blocks */
	uint32_t ulCurrPrimPos;                     /* current primitive position in current block */

	/* tile information */
	BP3D_P_Tile *pTiles;                        /* pointer to tiles */
	uint32_t ulTileWidth;                       /* tile width */
	uint32_t ulTileHeight;                      /* tile height */
	uint16_t usTileCountX;                      /* horizontal tile count */
	uint16_t usTileCountY;                      /* vertical tile count */
	uint8_t ucTileShiftX;                       /* horizontal tile shift */
	uint8_t ucTileShiftY;                       /* vertical tile shift */
	bool bTileMergeHorizontal;                  /* horizontal merge of tiles */
	bool bTileMergeVertical;                    /* vertical merge of tiles */
	bool bUsingDepthBuffer;

	/* show tiles information */
	BP3D_P_Buffer RenderDstColorBuffer;
	uint32_t ulRenderRegSetOffset;
	uint32_t ulRenderRegSetCount;

	BP3D_Context hNextContext;
}
BP3D_P_Context;

/***************************************************************************/
typedef struct BP3D_P_Handle
{
	/* module and interrupt handles */
	BCHP_Handle hChip;                          /* handle to chip module */
	BREG_Handle hRegister;                      /* handle to register module */
	BMEM_Handle hMemory;                        /* handle to memory module */
	BINT_Handle hInterrupt;                     /* handle to interrupt module */
	BINT_CallbackHandle hInterruptCallback;     /* handle to interrupt 'end of frame' callback */

	/* current contexts */
	BP3D_Context hContext;
	BP3D_Context hRenderContext;
	BP3D_Context hQueueContext;

	/* memory block information */
	uint32_t ulBlockTotalSize;                  /* total size of block allocations */

	/* temp buffer */
	BP3D_P_Buffer TmpColorBuffer;               /* intermediate color buffer structure */
	BP3D_P_Buffer TmpDepthBuffer;               /* intermediate depth buffer structure */
	void *pvTmpBuffer;                          /* pointer to intermediate multi-render buffer */

	uint32_t ulMemClientInfoReg;
	bool bUsingDepthBuffer;

	bool bShowTiles;
}
BP3D_P_Handle;

/***************************************************************************/
#define BP3D_P_MIN( v0, v1 )        (((v0) < (v1)) ? (v0) : (v1))
#define BP3D_P_MAX( v0, v1 )        (((v0) > (v1)) ? (v0) : (v1))

/***************************************************************************/
#define BP3D_P_GET_TILE( context, xx, yy ) \
	&(context)->pTiles[(xx) + (yy) * (context)->usTileCountX]

/***************************************************************************/
#define BP3D_P_IS_TILE_MERGEABLE( pTile ) \
	((pTile->ulRenderCount == 0) && (hP3d->hContext->bClearColor || hP3d->hContext->bClearDepth || \
		hP3d->hContext->hSrcColorSurface || (pTile->pFirstLayer && pTile->pFirstLayer->ulNodeCount)))

/***************************************************************************/
#define BP3D_P_SET_REGISTER_DATA( reg, field, data ) \
{ \
	hP3d->hContext->aulRegs[BP3D_P_PX3D_##reg] &= ~BCHP_MASK(PX3D_##reg, field); \
	hP3d->hContext->aulRegs[BP3D_P_PX3D_##reg] |= BCHP_FIELD_DATA(PX3D_##reg, field, (uint32_t) (data)); \
}

/***************************************************************************/
#define BP3D_P_SET_REGISTER_ENUM( reg, field, data ) \
{ \
	hP3d->hContext->aulRegs[BP3D_P_PX3D_##reg] &= ~BCHP_MASK(PX3D_##reg, field); \
	hP3d->hContext->aulRegs[BP3D_P_PX3D_##reg] |= BCHP_FIELD_ENUM(PX3D_##reg, field, data); \
}

/***************************************************************************/
#define BP3D_P_SET_REGISTER_ADDR( reg, data ) \
	hP3d->hContext->aulRegs[BP3D_P_PX3D_##reg] = (data)

/***************************************************************************/
#define BP3D_P_GET_PRIMHEADER_DATA( reg, field ) \
	((hP3d->hContext->PrimHeader.ul##reg & BCHP_MASK(PX3D_Type_##reg, field)) >> BCHP_SHIFT(PX3D_Type_##reg, field))

/***************************************************************************/
#define BP3D_P_SET_PRIMHEADER_DATA( reg, field, data ) \
{ \
	hP3d->hContext->PrimHeader.ul##reg &= ~BCHP_MASK(PX3D_Type_##reg, field); \
	hP3d->hContext->PrimHeader.ul##reg |= BCHP_FIELD_DATA(PX3D_Type_##reg, field, (uint32_t) (data)); \
}

/***************************************************************************/
#define BP3D_P_SET_PRIMHEADER_ENUM( reg, field, data ) \
{ \
	hP3d->hContext->PrimHeader.ul##reg &= ~BCHP_MASK(PX3D_Type_##reg, field); \
	hP3d->hContext->PrimHeader.ul##reg |= BCHP_FIELD_ENUM(PX3D_Type_##reg, field, data); \
}

/***************************************************************************/
#define BP3D_P_SET_PRIMHEADER_ADDR( data ) \
{ \
	uint32_t ulAddrMask = BCHP_MASK(PX3D_Type_TextureAddr, TextureAddr); \
	hP3d->hContext->PrimHeader.ulTextureAddr = (hP3d->hContext->PrimHeader.ulTextureAddr & (~ulAddrMask)) | ((data) & ulAddrMask); \
	hP3d->hContext->PrimHeader.ulPrimHeaderB = (hP3d->hContext->PrimHeader.ulPrimHeaderB & 0xFFFFFF) | (((data) & (~ulAddrMask)) >> 2); \
}

/***************************************************************************/
#define BP3D_P_SET_PRIMHEADER_OPAQUE() \
	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, Opaque, ( \
		(BP3D_P_GET_PRIMHEADER_DATA(PrimHeaderA, DstAlphaBlend) != BP3D_Blend_Zero) || \
		(BP3D_P_GET_PRIMHEADER_DATA(PrimHeaderA, SrcAlphaBlend) >= BP3D_Blend_DstAlpha) || \
		(BP3D_P_GET_PRIMHEADER_DATA(PrimHeaderA, AlphaCompare) != BP3D_Compare_Always) || \
		(BP3D_P_GET_PRIMHEADER_DATA(PrimHeaderB, DepthMask) == 1) || ( \
		(BP3D_P_GET_PRIMHEADER_DATA(PrimHeaderA, TextureEnable) == 1) && \
		(BP3D_P_GET_PRIMHEADER_DATA(TextureAddr, ColorKeySelect) != 0))) ? 0 : 1 );

/***************************************************************************/
#define BP3D_P_SET_REGISTER_CFMT( reg, field, eFormat ) \
{ \
	switch( eFormat ) \
	{ \
	case BPXL_eA8_R8_G8_B8: \
		BP3D_P_SET_REGISTER_ENUM( reg, field, ARGB8888 ); \
		break; \
	case BPXL_eR5_G6_B5: \
		BP3D_P_SET_REGISTER_ENUM( reg, field, RGB565 ); \
		break; \
	case BPXL_eA1_R5_G5_B5: \
		BP3D_P_SET_REGISTER_ENUM( reg, field, ARGB1555 ); \
		break; \
	case BPXL_eA4_R4_G4_B4: \
		BP3D_P_SET_REGISTER_ENUM( reg, field, ARGB4444 ); \
		break; \
	case BPXL_eZ16: \
		BP3D_P_SET_REGISTER_DATA( reg, field, 1 ); \
		break; \
	default: \
		BDBG_ERR(( "Surface format not supported\n" )); \
	} \
}

/***************************************************************************/
#define BP3D_P_SET_SURFACE_REGISTERS( reg, regaddr, field, bEnable, pBuffer, usTileX, usTileY, usTileWidth ) \
{ \
	if( (bEnable) ) \
	{ \
		BP3D_P_SET_REGISTER_DATA( reg##Config, field##Enabled, 1 ); \
		BP3D_P_SET_REGISTER_CFMT( reg##Config, field##Format, (pBuffer)->eFormat ); \
		BP3D_P_SET_REGISTER_DATA( reg##Config, field##Stride, \
			(pBuffer)->ulPitch / (pBuffer)->ulBpp - (usTileWidth) ); \
		BP3D_P_SET_REGISTER_ADDR( regaddr##Addr, (pBuffer)->ulOffset + \
			(usTileX) * (pBuffer)->ulBpp + (usTileY) * (pBuffer)->ulPitch ); \
	} \
	else \
	{ \
		BP3D_P_SET_REGISTER_DATA( reg##Config, field##Enabled, 0 ); \
		BP3D_P_SET_REGISTER_DATA( reg##Config, field##Stride, 0 ); \
		BP3D_P_SET_REGISTER_DATA( reg##Config, field##Format, 0 ); \
		BP3D_P_SET_REGISTER_ADDR( regaddr##Addr, 0 ); \
	} \
}

/***************************************************************************/
#define BP3D_P_SET_RECTANGLE_REGISTERS( pViewport, usX, usY, usWidth, usHeight ) \
{ \
	uint16_t usMinX = (pViewport)->usMinX + (usX) * hP3d->hContext->ulTileWidth; \
	uint16_t usMinY = (pViewport)->usMinY + (usY) * hP3d->hContext->ulTileHeight; \
	uint16_t usMaxX = (pViewport)->usMinX + ((usX) + (usWidth)) * hP3d->hContext->ulTileWidth; \
	uint16_t usMaxY = (pViewport)->usMinY + ((usY) + (usHeight)) * hP3d->hContext->ulTileHeight; \
	BP3D_P_SET_REGISTER_DATA( TopLeft, Left, usMinX ); \
	BP3D_P_SET_REGISTER_DATA( TopLeft, Top, usMinY ); \
	BP3D_P_SET_REGISTER_DATA( BottomRight, Right, BP3D_P_MIN((pViewport)->usMaxX, usMaxX) ); \
	BP3D_P_SET_REGISTER_DATA( BottomRight, Bottom, BP3D_P_MIN((pViewport)->usMaxY, usMaxY) ); \
}

/***************************************************************************/
#define BP3D_P_SET_BUFFERCLEAR_REGISTER( bColorClear, bDepthClear, bFirstLayer, bLastLayer ) \
{ \
	if( (bColorClear) && ((bFirstLayer) || (bLastLayer)) ) { \
		BP3D_P_SET_REGISTER_ENUM( Mode, BufferClear, BackgroundColorZFar ); } \
	else if( (bDepthClear) && ((bFirstLayer) || (bLastLayer)) ) { \
		BP3D_P_SET_REGISTER_ENUM( Mode, BufferClear, EmptyZFar ); } \
	else { \
		BP3D_P_SET_REGISTER_ENUM( Mode, BufferClear, EmptyEmpty ); } \
}

/***************************************************************************/
/* bp3d_memory.c */
void *BP3D_P_AllocMemoryBlock( BP3D_Handle hP3d, uint32_t *pulOffset );
void BP3D_P_FreeUnusedMemoryBlocks( BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_FreeAllMemoryBlocks( BP3D_Handle hP3d, BP3D_Context hContext );
BP3D_P_Block *BP3D_P_CreateMemoryBlock( BP3D_Handle hP3d );
BP3D_P_Block *BP3D_P_DestroyMemoryBlock( BP3D_Handle hP3d, BP3D_P_Block *pBlock, BP3D_P_Block **ppPrevBlock );

/* bp3d_primitive.c */
bool BP3D_P_AllocPrimBlock( BP3D_Handle hP3d );
void BP3D_P_ResetPrimBlocks( BP3D_Handle hP3d );
BP3D_P_Primitive *BP3D_P_AllocPrimitives( BP3D_Handle hP3d, uint32_t ulPrimCount );
BP3D_P_Primitive *BP3D_P_GetNextPrimitive( BP3D_Handle hP3d );
BP3D_P_Primitive *BP3D_P_CopyPoint( BP3D_Handle hP3d, BP3D_P_Primitive *pPrim, BP3D_Vertex *pV0 );
BP3D_P_Primitive *BP3D_P_CopyLine( BP3D_Handle hP3d, BP3D_P_Primitive *pPrim, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
BP3D_P_Primitive *BP3D_P_CopyRectangle( BP3D_Handle hP3d, BP3D_P_Primitive *pPrim, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
BP3D_P_Primitive *BP3D_P_CopyTriangle( BP3D_Handle hP3d, BP3D_P_Primitive *pPrim, BP3D_Vertex *pV0, BP3D_Vertex *pV1, BP3D_Vertex *pV2 );
BERR_Code BP3D_P_CreateSpecialPrimitives( BP3D_Handle hP3d );
void BP3D_P_DestroySpecialPrimitives( BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_AddEmptyPointToTiles( BP3D_Handle hP3d, uint16_t usX, uint16_t usY );
void BP3D_P_AddColorRectangleToTiles( BP3D_Handle hP3d, uint32_t ulColor );
void BP3D_P_AddDepthRectangleToTiles( BP3D_Handle hP3d, float fDepth );

/* bp3d_layer.c */
BP3D_P_Layer *BP3D_P_CreateLayer( BP3D_P_Tile *pTile );
void BP3D_P_DestroyAllLayers( BP3D_P_Tile *pTile );
void BP3D_P_ResetLayers( BP3D_P_Tile *pTile );
BP3D_P_Layer *BP3D_P_GetCurrentLayer( BP3D_P_Tile *pTile );
void BP3D_P_AddPrimitiveToLayerLeft( BP3D_P_Layer *pLayer, uint32_t ulPrimOffset, uint16_t usLayerMinY, uint16_t usLayerMaxY );
void BP3D_P_AddPrimitiveToLayerRight( BP3D_P_Layer *pLayer, uint32_t ulPrimOffset, uint16_t usLayerMinY, uint16_t usLayerMaxY );
BP3D_P_Layer *BP3D_P_MergeLayers( BP3D_P_Tile *pTile, BP3D_P_Layer *pLayer );

/* bp3d_tile.c */
BERR_Code BP3D_P_CreateTiles( BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_DestroyTiles( BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_ResetTiles(	BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_AddPrimitiveToTiles( BP3D_Handle hP3d, BP3D_P_Rect *pRect, uint32_t ulPrimOffset );
bool BP3D_P_MergeTiles( BP3D_Handle hP3d, BP3D_P_Tile *pTile0, BP3D_P_Tile *pTile1 );
void BP3D_P_MergeTiles_HorzAndVert( BP3D_Handle hP3d, uint16_t usTileX, uint16_t usTileY );
BP3D_P_Tile *BP3D_P_GetTile( BP3D_Handle hP3d, uint16_t usTileX, uint16_t usTileY );
void BP3D_P_ProcessTiles_NoMerge( BP3D_Handle hP3d );
void BP3D_P_ProcessTiles_HorzMerge( BP3D_Handle hP3d );
void BP3D_P_ProcessTiles_HorzThenVertMerge( BP3D_Handle hP3d );
void BP3D_P_ProcessTiles_HorzAndVertMerge( BP3D_Handle hP3d );
void BP3D_P_ProcessTiles_Bins( BP3D_Handle hP3d );
void BP3D_P_ProcessUnderflowTiles( BP3D_Handle hP3d, uint16_t usTileX,	uint16_t usTileY, uint16_t usTileCountX, uint16_t usTileCountY );
void BP3D_P_ProcessOverflowTile( BP3D_Handle hP3d, BP3D_P_Tile *pTile, BP3D_P_Layer *pLayer0, BP3D_P_Layer *pLayer1, uint16_t usTileX, uint16_t usTileY );
void BP3D_P_WriteTileRegSet( BP3D_Handle hP3d, uint32_t *pulRegs, uint32_t ulRegsOffset, uint32_t ulListOffset, uint32_t ulPrimCount, uint16_t usTileX, uint16_t usTileY, uint16_t usTileCountX, uint16_t usTileCountY, bool bFirstLayer, bool bLastLayer );
void BP3D_P_DumpDisplayList( BP3D_Handle hP3d, uint32_t *pulRegSet );
void BP3D_P_DumpTiles( BP3D_Handle hP3d );
void BP3D_P_DumpRegSets( BP3D_Handle hP3d );
void BP3D_P_VerifyRegSets( BP3D_Handle hP3d );
void BP3D_P_DrawTileBoundry( BP3D_Handle hP3d, BP3D_P_Buffer *pBuffer, uint32_t *pulRegSet, uint32_t ulColor );
void BP3D_P_DrawTileBoundries( BP3D_Handle hP3d );

/* bp3d_displaylist.c */
uint32_t *BP3D_P_AllocDisplayList( BP3D_Handle hP3d, uint32_t ulPrimCount, uint32_t *pulOffset );
uint32_t *BP3D_P_WriteDisplayListScan( BP3D_Handle hP3d, BP3D_P_Layer *pLayer, uint32_t *pulList, uint8_t ucIndex );
uint32_t BP3D_P_WriteUnderflowDisplayList( BP3D_Handle hP3d, uint32_t *pulList, uint16_t usTileX, uint16_t usTileY, uint16_t usTileCountX, uint16_t usTileCountY );
uint32_t BP3D_P_WriteOverflowDisplayList( BP3D_Handle hP3d, BP3D_P_Layer *pLayer0, BP3D_P_Layer *pLayer1, uint32_t *pulList );
uint32_t BP3D_P_WriteBinDisplayList( BP3D_Handle hP3d, BP3D_P_Tile *pTile, uint32_t *pulList );

/* bp3d_render.c */
void BP3D_P_RenderContext( BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_Render_Isr( void *pvParam1, int iParam2 );
void BP3D_P_InititateRender( BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_WaitForRender( BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_LoadFogTable( BP3D_Handle hP3d, BP3D_Context hContext );
void BP3D_P_DumpRegisters( BP3D_Handle hP3d );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BP3D_PRIVATE_H__ */

/* end of file */
