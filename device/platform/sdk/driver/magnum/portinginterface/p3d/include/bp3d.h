/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d.h $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 1/17/12 2:54p $
 *
 * Module Description: 3D Rendering Module Public Header File
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7405/bp3d.h $
 * 
 * Hydra_Software_Devel/15   1/17/12 2:54p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/14   8/18/08 4:48p nissen
 * PR 39323: Added support for configuring memory block out for 3549/3556
 * A0.
 * 
 * Hydra_Software_Devel/13   7/15/08 12:41p nissen
 * PR 41229: Added function to configure device tiles.
 * 
 * Hydra_Software_Devel/12   6/16/08 8:19p nissen
 * PR 42507: Added support for rendering with multiple contexts.
 * 
 * Hydra_Software_Devel/11   9/20/06 2:10p nissen
 * PR 22858: Added support for polygon offset through a depth scaling
 * factor used to offset a primitive's depth values.
 * 
 * Hydra_Software_Devel/10   6/5/06 1:09p nissen
 * PR 20908: Fixed problem with function declaration.
 * 
 * Hydra_Software_Devel/9   6/5/06 11:20a nissen
 * PR 20908: Added support for preparing and flushing render and checking
 * if a render is busy. Added support for swizzling textures.
 * 
 * Hydra_Software_Devel/8   1/5/05 1:34p nissen
 * PR 12728: Removed commas at the end of enums for pedantic errors.
 * 
 * Hydra_Software_Devel/7   9/17/04 4:19p nissen
 * PR 9132: Added function to get current count of primitives pending to
 * be rendered.
 * 
 * Hydra_Software_Devel/6   9/7/04 2:59p nissen
 * PR 9132: Added extra parameter to interrupt callback.
 * 
 * Hydra_Software_Devel/5   8/10/04 4:41p nissen
 * PR 9132: Rewrote fog.
 * 
 * Hydra_Software_Devel/4   7/27/04 10:56p nissen
 * PR 9132: Added offset parameters to function that sets viewport.
 * 
 * Hydra_Software_Devel/3   4/30/04 11:03a nissen
 * PR 9132: Changed entry point function names and some enums.
 * 
 * Hydra_Software_Devel/2   3/5/04 2:57p nissen
 * PR 9132: Added function definitions for debug entry points.
 * 
 * Hydra_Software_Devel/1   1/22/04 2:40p nissen
 * PR 9132: Initial version.
 * 
 ***************************************************************************/

#ifndef BP3D_H__
#define BP3D_H__

#include "bchp.h"
#include "bmem.h"
#include "bsur.h"
#include "bint.h"
#include "breg_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=Module Overview: ********************************************************
The purpose of this module is to ...
****************************************************************************/

/***************************************************************************
Summary:
	The handle for the 3D rendering module.

Description:
	This is the main handle required to do any operations within the 
	3D rendering module.

See Also:
	BP3D_Open
****************************************************************************/
typedef struct BP3D_P_Handle *BP3D_Handle;

/***************************************************************************
Summary:
	The context for 3D rendering.

Description:
	There can be as many contexts as the user can create.

See Also:
	BP3D_Context_Create
****************************************************************************/
typedef struct BP3D_P_Context *BP3D_Context;

/***************************************************************************
Summary:
	Callback used for task completion.

Description:
	When a task is issued, this callback can be used to get notification
	when the task is completed.

See Also:
	BP3D_IssueState
****************************************************************************/
typedef BERR_Code (*BP3D_Callback)(BP3D_Handle hP3d, void *pvParam1, int iParam2);

/***************************************************************************
Summary:
	The vertex data structure.

Description:
	This is the structure used to hold vertex data for primitives.

See Also:
	BP3D_DrawTriangle
****************************************************************************/
typedef struct BP3D_Vertex
{
	float fX;                   /* x coordinate */
	float fY;                   /* y coordinate */
	float fZ;                   /* z coordinate */
	float fW;                   /* w value */
	uint32_t ulDiffuse;         /* diffuse color in ARGB8888 */
	uint32_t ulSpecular;        /* specular color in FRGB888, F=Fog */
	float fS;                   /* s texture coordinate */
	float fT;                   /* t texture coordinate */
	float fInvW;                /* inverse w value */
	int16_t sX;                 /* integer x coordinate */
	int16_t sY;                 /* integer y coordinate */
}
BP3D_Vertex;

/***************************************************************************
Summary:
	Texture blend enumeration.

Description:
	Used to specify texture blending when drawing primitives.

See Also:
	BP3D_SetTextureBlend
****************************************************************************/
typedef enum BP3D_TextureBlend
{                                        /* COLOR            ALPHA */  
	BP3D_TextureBlend_Decal = 0,         /* TC               TA    */
	BP3D_TextureBlend_Modulate = 1,      /* TC*VC            TA    */
	BP3D_TextureBlend_DecalAlpha = 2,    /* TC*TA+VC*(1-TA)  VA    */
	BP3D_TextureBlend_ModulateAlpha = 3, /* TC*VC            TA*VA */
	BP3D_TextureBlend_Add = 6            /* TC+VC            VA    */
}
BP3D_TextureBlend;

/***************************************************************************
Summary:
	Texture filter enumeration.

Description:
	Used to specify texture filtering when drawing primitives.

See Also:
	BP3D_SetTextureFilter
****************************************************************************/
typedef enum BP3D_TextureFilter
{
	BP3D_TextureFilter_Point = 0,
	BP3D_TextureFilter_Bilinear = 1
}
BP3D_TextureFilter;

/***************************************************************************
Summary:
	Texture blend enumeration.

Description:
	Used to specify texture wrapping when drawing primitives.

See Also:
	BP3D_SetTextureWrap
****************************************************************************/
typedef enum BP3D_TextureWrap
{
	BP3D_TextureWrap_Tile = 0,
	BP3D_TextureWrap_Mirror = 1,
	BP3D_TextureWrap_Clamp = 2
}
BP3D_TextureWrap;

/***************************************************************************
Summary:
	Blend enumeration.

Description:
	Used to specify source and destination blending when drawing primitives.

See Also:
	BP3D_SetSrcBlend,
	BP3D_SetDstBlend
****************************************************************************/
typedef enum BP3D_Blend
{                                           /*    A     R     G     B */
	BP3D_Blend_Zero = 0,                    /*    0,    0,    0,    0 */
	BP3D_Blend_One = 1,                     /*    1,    1,    1,    1 */
	BP3D_Blend_SrcColor = 2,                /*   SA,   SR,   SG,   SB */
	BP3D_Blend_InvSrcColor = 3,             /* 1-SA, 1-SR, 1-SG, 1-SB */
	BP3D_Blend_SrcAlpha = 4,                /*   SA,   SA,   SA,   SA */
	BP3D_Blend_InvSrcAlpha = 5,             /* 1-SA, 1-SA, 1-SA, 1-SA */
	BP3D_Blend_DstAlpha = 6,                /*   DA,   DA,   DA,   DA */
	BP3D_Blend_InvDstAlpha = 7,             /* 1-DA, 1-DA, 1-DA, 1-DA */
	BP3D_Blend_DstColor = 8,                /*   DA,   DR,   DG,   DB */
	BP3D_Blend_InvDstColor = 9,             /* 1-DA, 1-DR, 1-DG, 1-DB */
	BP3D_Blend_SrcAlphaSat = 10             /*    1, min(SA,(1-DA) x3 */
}
BP3D_Blend;

/***************************************************************************
Summary:
	Compare enumeration.

Description:
	Used to specify depth and alpha compare when drawing primitives.

See Also:
	BP3D_SetDepthCompare,
	BP3D_SetAlphaCompare
****************************************************************************/
typedef enum BP3D_Compare
{
	BP3D_Compare_Never = 0,
	BP3D_Compare_Less = 1,
	BP3D_Compare_Equal = 2,
	BP3D_Compare_LessEqual = 3,
	BP3D_Compare_Greater = 4,
	BP3D_Compare_NotEqual = 5,
	BP3D_Compare_GreaterEqual = 6,
	BP3D_Compare_Always = 7
}
BP3D_Compare;

/***************************************************************************
Summary:
	Fog type enumeration.

Description:
	Used to specify fog type when drawing primitives.

See Also:
	BP3D_SetFogType
****************************************************************************/
typedef enum BP3D_Fog
{
	BP3D_Fog_None = 0,
	BP3D_Fog_Vertex = 1,
	BP3D_Fog_Depth = 2,
	BP3D_Fog_Table = 3
}
BP3D_Fog;

/***************************************************************************
Summary:
	Shade enumeration.

Description:
	Used to specify color shading mode using vertices' diffuse color when 
	drawing primitives.

See Also:
	BP3D_SetShade
****************************************************************************/
typedef enum BP3D_Shade
{
	BP3D_Shade_Flat = 0,
	BP3D_Shade_Gouraud = 1
}
BP3D_Shade;

/***************************************************************************
Summary:
	Cull enumeration.

Description:
	Used to specify cull mode when drawing primitives.

See Also:
	BP3D_SetCull
****************************************************************************/
typedef enum BP3D_Cull
{
	BP3D_Cull_None = 0,
	BP3D_Cull_Clockwise = 1,
	BP3D_Cull_CounterClockwise = 2
}
BP3D_Cull;

/***************************************************************************
Summary:
	Opens the 3D rendering module.

Description:
	The module is opened and a 3D rendering module handle is
	created and returned. This handle will be necessary to perform any
	tasks in the 3D rendering module.

Returns:
	BERR_SUCCESS - Module was opened and valid handle was returned.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_Close
****************************************************************************/
BERR_Code BP3D_Open( 
	BP3D_Handle *phP3d,           /* [out] Pointer to returned P3D handle. */
	BCHP_Handle hChip,            /* [in] Chip handle. */
	BREG_Handle hRegister,        /* [in] Register access handle. */
	BMEM_Handle hMemory,          /* [in] Memory allocation handle. */
	BINT_Handle hInterrupt        /* [in] Interrupt handle. */
);

/***************************************************************************
Summary:
	Closes the 3D rendering module.

Description:
	Once this function is called no more 3D rendering module
	functions can be used. 

	Outstanding tasks may be interrupted without callbacks being
	called.

See Also:
	BP3D_Open
****************************************************************************/
void BP3D_Close(
	BP3D_Handle hP3d              /* [in] Handle to P3D module. */
);

/***************************************************************************
Summary:
	Creates a context for rendering.

Description:
	A context needs to be created before any rendering can occur. The user
	can create as many contexts as they want, and each one will be rendered
	as requested.

Returns:
	BERR_SUCCESS - Context was created and a valid context was returned.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_Context_Destroy, BP3D_Context_SetCurrent
****************************************************************************/
BERR_Code BP3D_Context_Create( 
	BP3D_Handle hP3d,
	BP3D_Context *phContext
);

/***************************************************************************
Summary:
	Destroys a context.

Description:
	This function will destroy all resources associated with a context.

See Also:
	BP3D_Context_Create, BP3D_Context_SetCurrent
****************************************************************************/
void BP3D_Context_Destroy(
	BP3D_Handle hP3d,
	BP3D_Context hContext
);

/***************************************************************************
Summary:
	Sets the current context.

Description:
	This function must be called with a previously created context before 
	drawing any primitives and before rendering can occur. A context can be
	changed without a penality.

See Also:
	BP3D_Context_Create, BP3D_Context_Destroy
****************************************************************************/
void BP3D_Context_SetCurrent(
	BP3D_Handle hP3d,
	BP3D_Context hContext
);

/***************************************************************************
Summary:
	Configures tiles for rendering.

Description:
	This function allows the user to set the size of tiles and whether or
	not they can be merged when rendering. Useful when trying to render
	in vertical or horizontal stripes, or when trying to control the size
	of overflow areas.
****************************************************************************/
BERR_Code BP3D_ConfigureTiles(
	BP3D_Handle hP3d,
	uint32_t ulTileWidth,
	uint32_t ulTileHeight,
	bool bMergeHorizontal,
	bool bMergeVertical
);

/***************************************************************************
Summary:
	Configures memory block for device.
****************************************************************************/
int32_t BP3D_ConfigureMemoryBlockout(
	BP3D_Handle hP3d,
	int32_t lMemoryBlockout 
);

/***************************************************************************
Summary:
	Prepares scene data for rendering.

Description:
	This will function will do the setup that is required before a scene 
	is rendered. It is not necessary to call this function before issuing
	a render. The purpose to this function is to allow the user to 
	control when this driver prepares a scene for rendering.

Returns:
	BERR_SUCCESS - Render was prepared.

See Also:
	BP3D_IssueRender
	BP3D_IssueRenderAndWait
****************************************************************************/
BERR_Code BP3D_PrepareRender(
	BP3D_Handle hP3d              /* [in] Handle to P3D module. */
);

/***************************************************************************
Summary:
	Issues render to hardware.

Description:
	When this call is made all the primitives in the scene will be 
	rendered using the specified scene states.

    Hardware will work on this task asyncronously. The user has
	the option of waiting for the task to complete by using the function
	BGRC_IssueStateAndWait, having the module call a callback function
	when complete using BGRC_IssueState, or not be notified when
	this task is complete.

	Callbacks will be called from an interrupt handler and must
	follow the calling restrictions of interrupt handlers.

Returns:
	BERR_SUCCESS - Task was issued.

See Also:
	BP3D_IssueRenderAndWait
	BP3D_FlushRender
****************************************************************************/
BERR_Code BP3D_IssueRender(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Callback pCallback,      /* [in] Pointer to completion callback. */
	void *pvParam1,               /* [in] Pointer to callback user data. */
	int iParam2                   /* [in] Callback user data. */
);

/***************************************************************************
Summary:
	Issues render to hardware and wait for completion.

Description:
	When this call is made all the primitives in the scene will be 
	rendered using the specified scene states.

    Hardware will work on this task asyncronously. The user has
	the option of waiting for the task to complete by using the function
	BGRC_IssueStateAndWait, having the module call a callback function
	when complete using BGRC_IssueState, or not be notified when
	this task is complete.

	This specific function does not use a callback but waits for
	the given task to complete.

Returns:
	BERR_SUCCESS - Task was issued and finished.

See Also:
	BP3D_IssueRender
****************************************************************************/
BERR_Code BP3D_IssueRenderAndWait(
	BP3D_Handle hP3d              /* [in] Handle to P3D module. */
);

/***************************************************************************
Summary:
	Checks if 3D device is busy.

Description:
	This function will determine if the 3D device has completed the render
	started by the previous call to issue a render.

See Also:
	BP3D_IssueRender
****************************************************************************/
BERR_Code BP3D_IsRenderBusy(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	bool *pbRenderBusy            /* [out] Returns busy flag. */
);

/***************************************************************************
Summary:
	Flushes previous render.

Description:
	When this call is made the previous render initiated by
	BP3D_IssueRender will be flushed.

Returns:
	BERR_SUCCESS - Task was issued and finished.

See Also:
	BP3D_IssueRender
****************************************************************************/
BERR_Code BP3D_FlushRender(
	BP3D_Handle hP3d              /* [in] Handle to P3D module. */
);

/***************************************************************************
Summary:
	Get primitive count.

Description:
	This function will get the count of primitives currently added to
	be drawn by the next render.

Returns:
	BERR_SUCCESS - Task was issued and finished.

See Also:
	BP3D_IssueRender, BP3D_IssueRenderAndWait
****************************************************************************/
BERR_Code BP3D_GetPrimitiveCount(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	uint32_t *pulPrimCount        /* [out] Pointer to return prim count */
);

/***************************************************************************
Summary:
	Sets the destination color surface.

Description:
	This function sets the destination color surface for rendering the 
	scene. Either a destination color or depth surface is required for 
	rendering.

	The destination and source color and depth surfaces must all have the
	same widths and heights.

	Supported surface formats are: 
		BPXL_eA8_R8_G8_B8
		BPXL_eR5_G6_B5
		BPXL_eA1_R5_G5_B5
		BPXL_eA4_R4_G4_B4

	The default surface is NULL.

Returns:
	BERR_SUCCESS - Destination color surface was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetDstDepthSurface, BP3D_SetSrcColorSurface
****************************************************************************/
BERR_Code BP3D_SetDstColorSurface(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BSUR_Surface_Handle hSurface  /* [in] Handle to dst color surface. */
);

BERR_Code BP3D_SetDstColorPlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch 
);

/***************************************************************************
Summary:
	Sets the destination depth surface.

Description:
	This function sets the destination depth surface for rendering the 
	scene. Either a destination color or depth surface is required for 
	rendering.

	The destination and source color and depth surfaces must all have the
	same widths and heights.

	Supported surface format is BPXL_eZ16.

	The default surface is NULL.

Returns:
	BERR_SUCCESS - Destination depth surface was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetDstColorSurface, BP3D_SetSrcDepthSurface
****************************************************************************/
BERR_Code BP3D_SetDstDepthSurface(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BSUR_Surface_Handle hSurface  /* [in] Handle to dst depth surface. */
);

BERR_Code BP3D_SetDstDepthPlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch 
);

/***************************************************************************
Summary:
	Sets the source color surface.

Description:
	This function sets the source color surface that is used as an input 
	or background image for rendering the scene's primitives on to. This
	surface can be set the same as the destination color surface to 
	blend the scene's primitives with the destination, or it can be set to
	a different surface with a different color format. 

	The destination and source color and depth surfaces must all have the
	same widths and heights. Setting a source color surface will 
	override color clearing.

	Supported surface formats are: 
		BPXL_eA8_R8_G8_B8
		BPXL_eR5_G6_B5
		BPXL_eA1_R5_G5_B5
		BPXL_eA4_R4_G4_B4

	The default surface is NULL.

Returns:
	BERR_SUCCESS - Source color surface was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetDstColorSurface, BP3D_SetSrcDepthSurface
****************************************************************************/
BERR_Code BP3D_SetSrcColorSurface(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BSUR_Surface_Handle hSurface  /* [in] Handle to src color surface. */
);

BERR_Code BP3D_SetSrcColorPlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch 
);

/***************************************************************************
Summary:
	Sets the source depth surface.

Description:
	This function sets the source depth surface that is used as an input 
	or background depth buffer for rendering the scene's primitives to. 
	This surface can be set the same as the destination depth surface to 
	blend the scene's primitives with the destination, or it can be set to
	a different surface. 

	The destination and source color and depth surfaces must all have the
	same widths and heights. Setting a source depth surface will 
	override depth clearing.

	Supported surface format is BPXL_eZ16.

	The default surface is NULL.

Returns:
	BERR_SUCCESS - Source depth surface was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetSrcColorSurface, BP3D_SetDstDepthSurface
****************************************************************************/
BERR_Code BP3D_SetSrcDepthSurface(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BSUR_Surface_Handle hSurface  /* [in] Handle to src depth surface. */
);

BERR_Code BP3D_SetSrcDepthPlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch 
);

/***************************************************************************
Summary:
	Sets viewport.

Description:
	This function sets the viewport of the scene. Any primitives that fall
	outside of the viewport will be clipped when rendered. The viewport 
	parameters must be greater then or equal to 0, and less than 2048.
	
	The default is 0 for x and y, and the destination's dimensions for the 
	width and height.

Returns:
	BERR_SUCCESS - Viewport was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_SetViewport(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	uint32_t ulX,                 /* [in] Viewport left. */
	uint32_t ulY,                 /* [in] Viewport top. */
	uint32_t ulWidth,             /* [in] Viewport width. */
	uint32_t ulHeight,            /* [in] Viewport height. */
	uint32_t ulOffsetX,           /* [in] Buffer address x offset. */
	uint32_t ulOffsetY            /* [in] Buffer address y offset. */
);

/***************************************************************************
Summary:
	Sets texture surface.

Description:
	This function sets the texture surface for drawing primitives. This
	surface cannot be modified by the user until rendering has been 
	started	and completed.

	Supported surface formats are: 
		BPXL_eA8_R8_G8_B8
		BPXL_eR5_G6_B5
		BPXL_eA1_R5_G5_B5
		BPXL_eA4_R4_G4_B4
		BPXL_eY08_Cb8_Y18_Cr8
		BPXL_eL8
		BPXL_eL8_A8

	The default surface is NULL.

Returns:
	BERR_SUCCESS - Texture surface was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_SetTextureBlend, BP3D_SetTextureFilter, BP3D_SetTextureWrap, 
	BP3D_SetTextureMipmap, BP3D_SetTextureColorKey
****************************************************************************/
BERR_Code BP3D_SetTextureSurface(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BSUR_Surface_Handle hSurface  /* [in] Handle to texture surface. */
);

BERR_Code BP3D_SetTexturePlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch,
	uint32_t ulWidth,
	uint32_t ulHeight 
);

/***************************************************************************
Summary:
	Sets texture blend.

Description:
	This function sets the texture blend for drawing primitives. Texture 
	blending mixes the colors of texels and vertices. Only valid when a 
	texture is set.

	BLEND           COLOR            ALPHA
	Decal           TC               TA         TC = texel color
	Modulate        TC*VC            TA         TA = texel alpha
	Decal Alpha     TC*TA+VC*(1-TA)  VA         VC = vertex diffuse color
	Modulate Alpha  TC*VC            TA*VA      VA = vertex diffuse alpha
	Add             TC+VC            VA

	The default is BP3D_TextureBlend_Decal.

Returns:
	BERR_SUCCESS - Texture blend was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_SetTextureSurface
****************************************************************************/
BERR_Code BP3D_SetTextureBlend(
	BP3D_Handle hP3d,                     /* [in] Handle to P3D module. */
	BP3D_TextureBlend eTextureBlend       /* [in] Texture blend. */
);

/***************************************************************************
Summary:
	Sets texture filter.

Description:
	This function sets the texture filter for drawing primitives. Texture 
	filtering can be used to blurr the blockiness of texels that are mapped 
	to the primitive. Only valid when a texture is set.

	The default is BP3D_TextureFilter_Point.

Returns:
	BERR_SUCCESS - Texture filter was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetTextureSurface
****************************************************************************/
BERR_Code BP3D_SetTextureFilter(
	BP3D_Handle hP3d,                     /* [in] Handle to P3D module. */
	BP3D_TextureFilter eTextureFilter     /* [in] Texture filter. */
);

/***************************************************************************
Summary:
	Sets texture wrap.

Description:
	This function sets the texture wrapping for drawing primitives. Texture 
	wrapping determines what happens when texture coordinates go outside
	the bounds of 0.0 to 1.0. Only valid when a texture is set.

	The default is BP3D_TextureWrap_Tile.

Returns:
	BERR_SUCCESS - Texture wrap was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetTextureSurface
****************************************************************************/
BERR_Code BP3D_SetTextureWrap(
	BP3D_Handle hP3d,                     /* [in] Handle to P3D module. */
	BP3D_TextureWrap eTextureWrap         /* [in] Texture wrap. */
);

/***************************************************************************
Summary:
	Sets texture mipmap.

Description:
	This function enables or disables texture mipmaping for drawing 
	primitives, and sets a level-of-detail maximum and bias. If enabled, 
	the texture surface must be a mipmap surface. Mipmap surfaces must have
	their level continuous in memory from largest to smallest. Only valid 
	when a texture is set.

	The default is disabled.

Returns:
	BERR_SUCCESS - Texture mipmap was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetTextureSurface
****************************************************************************/
BERR_Code BP3D_SetTextureMipmap(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	bool bEnableMipmap,           /* [in] True enables texture mipmapping. */
	float fLodMax,                /* [in] Mipmap level-of-detail maximum. */
	float fLodBias                /* [in] Mipmap level-of-detail bias. */
);

/***************************************************************************
Summary:
	Sets color keys.

Description:
	This function enables or disables color keying and sets the color key 
	for drawing primitives. Only three color keys are supported per render.
	
	The default is false.

Returns:
	BERR_SUCCESS - Color key was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_SetTextureSurface
****************************************************************************/
BERR_Code BP3D_SetTextureColorKey(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	bool bEnable,                 /* [in] True enables texture color key. */
	uint32_t ulColorKey           /* [in] Color key in ARGB8888 format. */
);

/***************************************************************************
Summary:
	Sets texture swizzling.

Description:
	This function enables or disables texture swizzling. Swizzling requires
	the texture pixels to be rearranged in a specific manner which will
	improve texture cache performance for textures that are rotated before
	being rendered.
	
	The default is false.

Returns:
	BERR_SUCCESS - Texture swizzle was set.

See Also:
	BP3D_SetTextureSurface
****************************************************************************/
BERR_Code BP3D_SetTextureSwizzle(
	BP3D_Handle hP3d,
	bool bEnable
);

/***************************************************************************
Summary:
	Sets source blend.

Description:
	This function sets the source blend factor for drawing primitives. 
	The source blend factor is multiplied by the source pixel. 
	A blended pixel is produced using the formula 
	"src_pixel * src_factor + dst_pixel * dst_factor".

	BLEND           COLOR FACTOR     ALPHA FACTOR
	Zero            0                0                SC = src color
	One             1                1                SA = src alpha
	SrcAlpha        SA               SA               DC = dst color
	InvSrcAlpha     1-SA             1-SA             DA = dst alpha
	DstAlpha        DA               DA
	InvDstAlpha     1-DA             1-DA
    DstColor        DC               DA
	InvDstColor     1-DC             1-DA
	SrcAlphaSat     min(SA,1-DA)     1

	The default is BP3D_Blend_One.

Returns:
	BERR_SUCCESS - Source blend was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetDstBlend
****************************************************************************/
BERR_Code BP3D_SetSrcBlend(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Blend eSrcBlend          /* [in] Source blend factor. */
);

/***************************************************************************
Summary:
	Sets source blend.

Description:
	This function sets the destination blend factor for drawing primitives. 
	The destination blend factor is multiplied by the destination pixel. 
	A blended pixel is produced using the formula 
	"src_pixel * src_factor + dst_pixel * dst_factor".

	BLEND           COLOR FACTOR     ALPHA FACTOR
	Zero            0                0                SC = src color
	One             1                1                SA = src alpha
    SrcColor        SC               SA               DC = dst color
	InvSrcColor     1-SC             1-SA             DA = dst alpha
	SrcAlpha        SA               SA
	InvSrcAlpha     1-SA             1-SA
	DstAlpha        DA               DA
	InvDstAlpha     1-DA             1-DA

	The default is BP3D_Blend_Zero.

Returns:
	BERR_SUCCESS - Destination blend was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetSrcBlend
****************************************************************************/
BERR_Code BP3D_SetDstBlend(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Blend eDstBlend          /* [in] Destination blend factor. */
);

/***************************************************************************
Summary:
	Sets depth compare.

Description:
	This function sets the depth compare for drawing primitives. When using
	depth compare, the primitives' depth values will be compared to the
	corresponding depth value in the depth buffer. Compares that fail will 
	not have their pixels written.

	The default is BP3D_Compare_Less.

Returns:
	BERR_SUCCESS - Depth compare was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetDepthMask
****************************************************************************/
BERR_Code BP3D_SetDepthCompare(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Compare eDepthCompare    /* [in] Depth compare. */
);

/***************************************************************************
Summary:
	Sets depth mask.

Description:
	This function sets the depth mask for drawing primitives. If the depth
	mask is enabled, then the primitives' depth values will not be written
	to the depth buffer.

	The default is disabled.

Returns:
	BERR_SUCCESS - Depth mask was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetDepthCompare
****************************************************************************/
BERR_Code BP3D_SetDepthMask(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	bool bDepthMask               /* [in] True enables depth write mask. */
);

/***************************************************************************
Summary:
	Sets depth factor scale value.

Description:
	This function sets the depth factor scale which gets multiplied by
	a primitive's maximum depth slope and added to the primitive's 
	depth value. 

	The default value is 0.

Returns:
	BERR_SUCCESS - Depth factor scale was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetDepthCompare
****************************************************************************/
BERR_Code BP3D_SetDepthFactorScale(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	float fScale                  /* [in] Depth factor scale value. */
);

/***************************************************************************
Summary:
	Sets alpha compare.

Description:
	This function sets the alpha compare for drawing primitives. When using
	alpha compare, the primitives' alpha values will be compared to the
	corresponding alpha value in the color buffer. Compares that fail will 
	not have their pixels written.

	The default is BP3D_Compare_Always.

Returns:
	BERR_SUCCESS - Alpha compare was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetAlphaReference
****************************************************************************/
BERR_Code BP3D_SetAlphaCompare(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Compare eAlphaCompare    /* [in] Alpha compare. */
);

/***************************************************************************
Summary:
	Sets alpha reference.

Description:
	This function sets the alpha reference value when rendering scenes 
	with primitives that are using alpha compare. Alpha reference can only
	have one value per render.
	
Returns:
	BERR_SUCCESS - Alpha reference was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_SetAlphaCompare
****************************************************************************/
BERR_Code BP3D_SetAlphaReference(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	uint8_t ucAlphaReference      /* [in] Alpha reference for compare. */
);

/***************************************************************************
Summary:
	Sets specular.

Description:
	This function enables or disables specular highlights for drawing 
	primitives. If specular is enabled then the specular color on the
	vertex will be added to the color of the primitive's pixels.
	
	The default is disabled.

Returns:
	BERR_SUCCESS - Specular was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_SetSpecular(
	BP3D_Handle hP3d,            /* [in] Handle to P3D module. */
	bool bEnableSpecular         /* [in] True enables specular highlights. */
);

/***************************************************************************
Summary:
	Sets fog type.

Description:
	This function sets the fog type for drawing primitives. Only one type
	of fog table can be used per render (Linear, Exp, Exp2).

Returns:
	BERR_SUCCESS - Fog type was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_SetFogType
****************************************************************************/
BERR_Code BP3D_SetFog(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Fog eFog                 /* [in] Fog type. */
);

/***************************************************************************
Summary:
	Sets fog color.

Description:
	This function sets the fog color for drawing primitives. The color is
	in the BPXL_eA8_R8_G8_B8 format. Only one fog color can be used per 
	render.

Returns:
	BERR_SUCCESS - Fog color was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_SetFogType
****************************************************************************/
BERR_Code BP3D_SetFogColor(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	uint32_t ulColor              /* [in] Color in ARGB8888 format. */
);

/***************************************************************************
Summary:
	Sets fog table.

Description:
	This function sets the fog table for drawing primitives that are
	set to use the fog table.

Returns:
	BERR_SUCCESS - Fog table was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.

See Also:
	BP3D_SetFog
****************************************************************************/
BERR_Code BP3D_SetFogTable(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	uint8_t *pucFogTable          /* [in] Pointer to fog table. */
);

/***************************************************************************
Summary:
	Sets shade.

Description:
	This function sets the shading type used for drawing primitives. 
	If set to point then the primitive's colors will come from the first
	vertex.
	
	The default is BP3D_Shade_Gouraud.

Returns:
	BERR_SUCCESS - Shade was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_SetShade(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Shade eShade             /* [in] Shade mode. */
);

/***************************************************************************
Summary:
	Sets cull.

Description:
	This function sets the cull mode used for drawing primitives. 
	Culled primitives will not be rendered.
	
	The default is BP3D_Cull_None.

Returns:
	BERR_SUCCESS - Cull was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_SetCull(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Cull eCull               /* [in] Cull mode. */
);

/***************************************************************************
Summary:
	Sets dither.

Description:
	This function enables or disables dithering when rendering a scene.
	Dithering is not performed on the BPXL_eA8_R8_G8_B8 format. Dither can
	only have one setting per render.
	
	The default is disabled.

Returns:
	BERR_SUCCESS - Dither was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_SetDither(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	bool bEnableDither            /* [in] True enables dither. */
);

/***************************************************************************
Summary:
	Clear color surface.

Description:
	This function clears the color surface with an BPXL_eA8_R8_G8_B8 color.
	
Returns:
	BERR_SUCCESS - Surface will be cleared.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetSrcColorSurface, BP3D_ClearDepth
****************************************************************************/
BERR_Code BP3D_ClearColor(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	uint32_t ulColor              /* [in] Clear color in ARGB8888 format. */
);

/***************************************************************************
Summary:
	Clear depth surface.

Description:
	This function clears the color surface with provide fill value.
	
Returns:
	BERR_SUCCESS - Surface will be cleared.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
  
See Also:
	BP3D_SetSrcColorSurface, BP3D_ClearDepth
****************************************************************************/
BERR_Code BP3D_ClearDepth(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	float fDepth                  /* [in] Clear depth in float. */
);

/***************************************************************************
Summary:
	Draw point.

Description:
	This function adds a point primitive to the scene which will be
	rendered when all primitives have been added.
	
Returns:
	BERR_SUCCESS - Primitive was added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawPoint(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pV0              /* [in] Pointer to vertex. */
);

/***************************************************************************
Summary:
	Draw line.

Description:
	This function adds a line primitive to the scene which will be
	rendered when all primitives have been added.
	
Returns:
	BERR_SUCCESS - Primitive was added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawLine(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pV0,             /* [in] Pointer to first vertex. */
	BP3D_Vertex *pV1              /* [in] Pointer to second vertex. */
);

/***************************************************************************
Summary:
	Draw rectangle.

Description:
	This function adds a screen aligned rectangle primitive to the scene 
	which will be rendered when all primitives have been added.
	
Returns:
	BERR_SUCCESS - Primitive was added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawRectangle(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pV0,             /* [in] Pointer to first vertex. */
	BP3D_Vertex *pV1              /* [in] Pointer to second vertex. */
);

/***************************************************************************
Summary:
	Draw triangle.

Description:
	This function adds a triangle primitive to the scene which will be
	rendered when all primitives have been added.
	
Returns:
	BERR_SUCCESS - Primitive was added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawTriangle(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pV0,             /* [in] Pointer to first vertex. */
	BP3D_Vertex *pV1,             /* [in] Pointer to second vertex. */
	BP3D_Vertex *pV2              /* [in] Pointer to third vertex. */
);

/***************************************************************************
Summary:
	Draw triangle list.

Description:
	This function adds a list of triangle primitives to the scene which 
	will be rendered when all primitives have been added. The count of
	vertices must be three times the count of triangles.
	
Returns:
	BERR_SUCCESS - Primitives were added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawTriangleList(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pVertices,       /* [in] Pointer to vertices. */
	uint32_t ulTriangleCount      /* [in] Count of triangles to draw. */
);

/***************************************************************************
Summary:
	Draw triangle strip.

Description:
	This function adds triangle strip primitives to the scene which 
	will be rendered when all primitives have been added. The count of 
	vertices must be two more than the count of triangles.
	
Returns:
	BERR_SUCCESS - Primitives were added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawTriangleStrip(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pVertices,       /* [in] Pointer to vertices. */
	uint32_t ulTriangleCount      /* [in] Count of triangles to draw. */
);

/***************************************************************************
Summary:
	Draw triangle fan.

Description:
	This function adds triangle fan primitives to the scene which 
	will be rendered when all primitives have been added. The count of 
	vertices must be two more than the count of triangles.
	
Returns:
	BERR_SUCCESS - Primitives were added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawTriangleFan(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pVertices,       /* [in] Pointer to vertices. */
	uint32_t ulTriangleCount      /* [in] Count of triangles to draw. */
);

/***************************************************************************
Summary:
	Draw indexed triangle list.

Description:
	This function adds an indexed list of triangle primitives to the scene 
	which will be rendered when all primitives have been added. The count 
	of indices must be three times the count of triangles.
	
Returns:
	BERR_SUCCESS - Primitives were added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawIndexedTriangleList(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pVertices,       /* [in] Pointer to vertices. */
	uint16_t *pusIndices,         /* [in] Pointer to 16 bit indices. */
	uint32_t ulTriangleCount      /* [in] Count of triangles to draw. */
);

/***************************************************************************
Summary:
	Draw indexed triangle strip.

Description:
	This function adds indexed triangle strip primitives to the scene which 
	will be rendered when all primitives have been added. The count of 
	vertices must be two more than the count of triangles.
	
Returns:
	BERR_SUCCESS - Primitives were added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawIndexedTriangleStrip(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pVertices,       /* [in] Pointer to vertices. */
	uint16_t *pusIndices,         /* [in] Pointer to 16 bit indices. */
	uint32_t ulTriangleCount      /* [in] Count of triangles to draw. */
);

/***************************************************************************
Summary:
	Draw indexed triangle fan.

Description:
	This function adds indexed triangle fan primitives to the scene which 
	will be rendered when all primitives have been added. The count of 
	vertices must be two more than the count of triangles.
	
Returns:
	BERR_SUCCESS - Primitives were added to scene.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_DrawIndexedTriangleFan(
	BP3D_Handle hP3d,             /* [in] Handle to P3D module. */
	BP3D_Vertex *pVertices,       /* [in] Pointer to vertices. */
	uint16_t *pusIndices,         /* [in] Pointer to 16 bit indices. */
	uint32_t ulTriangleCount      /* [in] Count of triangles to draw. */
);

/***************************************************************************
Summary:
	Shows tiles (for debugging purposes)

Description:
	This functions enables or disables the showing of tiles, and is
	used for debugging purposes.

	The default is false.

Returns:
	BERR_SUCCESS - Show tiles was set.
	BERR_INVALID_PARAMETER - One of the input parameters was invalid.
****************************************************************************/
BERR_Code BP3D_ShowTiles(
	BP3D_Handle hP3d,         /* [in] Handle to P3D module. */
	bool bShowTiles           /* [in] True enables showing tiles. */
);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BP3D_H__ */

/* end of file */
