/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d.c $
 * $brcm_Revision: Hydra_Software_Devel/59 $
 * $brcm_Date: 1/17/12 8:18p $
 *
 * Module Description: 3D Rendering Module Entry Point Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7405/bp3d.c $
 * 
 * Hydra_Software_Devel/59   1/17/12 8:18p nissen
 * SW7435-30: Fixed build warning.
 * 
 * Hydra_Software_Devel/58   1/17/12 2:59p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/57   8/6/10 12:50p nissen
 * SW7405-4720: Fixed problem with closing module.
 * 
 * Hydra_Software_Devel/56   3/10/10 5:48p nissen
 * SW7405-3704: Switched to truncating screen coordinates instead of
 * rounding.
 * 
 * Hydra_Software_Devel/55   3/1/10 11:20a nissen
 * SW7405-3704: Added use of memory block out when rendering with a depth
 * buffer for 3548/3556.
 * 
 * Hydra_Software_Devel/54   1/16/10 12:34p nissen
 * SW7405-3785: Fixed problem with closing module.
 * 
 * Hydra_Software_Devel/53   1/11/10 1:18p nissen
 * SW7400-2378: Fixed texture perspective when flat shading.
 * 
 * Hydra_Software_Devel/52   1/4/10 5:16p nissen
 * SW7405-3704: Added use of memory block out when rendering with a depth
 * buffer to avoid possible hang on 7405.
 * 
 * Hydra_Software_Devel/51   8/4/09 8:13p nissen
 * PR 57173: Fixed coverity defect involving context handle.
 * 
 * Hydra_Software_Devel/50   8/4/09 3:57p nissen
 * PR 35853: Added initial support for binning instead of tiling.
 * 
 * Hydra_Software_Devel/49   7/8/09 6:10p nissen
 * PR 55223: Added support for 7340 and 7342.
 * 
 * Hydra_Software_Devel/48   7/4/09 5:01p nissen
 * PR 56587: Fixed problem destroying context. Set viewport to at least 6
 * pixels for OpenGL conformance.
 * 
 * Hydra_Software_Devel/47   6/8/09 2:28p nissen
 * PR 54292: Fixed resource leak when creating context.
 * 
 * Hydra_Software_Devel/46   4/10/09 5:24p mward
 * PR54060: Compiler warning building with DEBUG=n.
 * 
 * Hydra_Software_Devel/45   3/17/09 3:37p nissen
 * PR 42507: Fixed context deallocation when destroying context.
 * 
 * Hydra_Software_Devel/44   2/17/09 5:26p nissen
 * PR 51629: Added support for 7336.
 * 
 * Hydra_Software_Devel/43   2/17/09 5:17p nissen
 * PR 45822: Fixed include for 7420 A1.
 * 
 * Hydra_Software_Devel/42   2/17/09 5:08p nissen
 * PR 45822: Switch GFD MEMC0 and MEMC1 for 7420 A1 also.
 * 
 * Hydra_Software_Devel/41   2/6/09 5:07p nissen
 * PR 45822: Switch GFD MEMC0 and MEMC1 for 7420 A0.
 * 
 * Hydra_Software_Devel/40   1/15/09 2:08p nissen
 * PR 47763: Changed interrupt settup for 7420.
 * 
 * Hydra_Software_Devel/39   1/15/09 12:08p nissen
 * PR 47763: Writing Mode register bit to enable memory client on 7420.
 * 
 * Hydra_Software_Devel/38   12/29/08 9:37p nissen
 * PR 50479: Added support for building for the 7405 C0.
 * 
 * Hydra_Software_Devel/37   12/3/08 10:09p nissen
 * PR 47763: Fixed support for 7420.
 * 
 * Hydra_Software_Devel/36   12/3/08 8:51p nissen
 * PR 47763: Fixed support for 7420.
 * 
 * Hydra_Software_Devel/35   12/3/08 6:53p nissen
 * PR 47763: Added support for 7420.
 * 
 * Hydra_Software_Devel/34   10/22/08 7:56p nissen
 * PR 47913: Removed bandwidth workaround for 3548/3556 B0.
 * 
 * Hydra_Software_Devel/33   10/22/08 7:35p nissen
 * PR 46638: Fixed clearing.
 * 
 * Hydra_Software_Devel/32   9/8/08 9:17p nissen
 * PR 46638: Added support for clearing with an alpha blend.
 * 
 * Hydra_Software_Devel/31   9/4/08 10:54a nissen
 * PR 39323: Fixed problem with deallocating context handle when module is
 * closed.
 * 
 * Hydra_Software_Devel/30   8/18/08 4:42p nissen
 * PR 39323: Added support for configuring memory block out for 3549/3556
 * A0.
 * 
 * Hydra_Software_Devel/29   8/1/08 4:20p nissen
 * PR 39323: Setting memory block out for 3549/3556 A0 to prevent hangs.
 * 
 * Hydra_Software_Devel/28   7/15/08 1:47p nissen
 * PR 41229: Added support for configuring tiles and for timing renders.
 * 
 * Hydra_Software_Devel/27   6/30/08 5:58a nissen
 * PR 39323: Setup interrupt handler to use trigger instead of interrupt
 * for 3548 and 3556.
 * 
 * Hydra_Software_Devel/26   6/16/08 8:14p nissen
 * PR 42507: Added support for rendering with multiple contexts.
 * 
 * Hydra_Software_Devel/25   4/23/08 2:16p nissen
 * PR 38949: Fixed include file.
 * 
 * Hydra_Software_Devel/24   4/4/08 1:31p nissen
 * PR 38949: Added support for building for the 3548.
 * 
 * Hydra_Software_Devel/23   10/24/07 2:45p nissen
 * PR 36107: Including bint.h instead of bint_plat.h.
 * 
 * Hydra_Software_Devel/22   9/10/07 11:33a nissen
 * PR 30062: Modified code to not render tiles when empty.
 * 
 * Hydra_Software_Devel/21   1/29/07 1:40p nissen
 * PR 26921: Added support for building for the 7400 B0.
 * 
 * Hydra_Software_Devel/20   12/7/06 12:00p nissen
 * PR 26403: Added support for clearing the alpha channel of the render
 * target to a specified value.
 * 
 * Hydra_Software_Devel/19   9/22/06 11:06a nissen
 * PR 22858: Fixed problem when rendering without primitives or clears but
 * with a source color buffer.
 * 
 * Hydra_Software_Devel/18   9/20/06 2:14p nissen
 * PR 22858: Added support for polygon offset through a depth scaling
 * factor used to offset a primitive's depth values.
 * 
 * Hydra_Software_Devel/17   9/14/06 4:08p nissen
 * PR 22858: Turned off perspective texture, color, and fog when flat
 * shading is selected.
 * 
 * Hydra_Software_Devel/16   6/5/06 11:17a nissen
 * PR 20908: Added support for preparing and flushing render and checking
 * if a render is busy. Added support for swizzling textures.
 * 
 * Hydra_Software_Devel/15   3/21/06 12:56p nissen
 * PR 20331: Calling new function to get surface offset.
 * 
 * Hydra_Software_Devel/14   3/21/06 12:01p nissen
 * PR 19084: Added support for building for the 7400 A0.
 * 
 * Hydra_Software_Devel/13   11/11/05 3:03p nissen
 * PR 17423: Fixed closing app and added extra YCbCr format for textures.
 * 
 * Hydra_Software_Devel/12   10/12/05 8:53p nissen
 * PR 12814: Fixed problem with waiting for previous render.
 * 
 * Hydra_Software_Devel/11   9/17/04 4:23p nissen
 * PR 9132: Added function to get current count of primitives pending to
 * be rendered.
 * 
 * Hydra_Software_Devel/10   9/7/04 2:54p nissen
 * PR 9132: Added extra parameter to interrupt callback.
 * 
 * Hydra_Software_Devel/9   8/10/04 10:38p nissen
 * PR 9132: Switched from truncating to rounding screen coordinates.
 * 
 * Hydra_Software_Devel/8   8/10/04 8:45p nissen
 * PR 9132: Changed fog table support.
 * 
 * Hydra_Software_Devel/7   7/27/04 10:59p nissen
 * PR 9132: Added support for setting surface offset when setting
 * viewport.
 * 
 * Hydra_Software_Devel/6   6/17/04 11:33a nissen
 * PR 11451: Added call to chip function to check if chip is 3D capable.
 * 
 * Hydra_Software_Devel/5   4/30/04 11:11a nissen
 * PR 9132: Changed entry point function names.
 * PR 10612: Added clearing of interrupt before enabling.
 * 
 * Hydra_Software_Devel/4   3/18/04 12:36p nissen
 * PR 9132: Changed screen coordinates to round instead of truncate.
 * 
 * Hydra_Software_Devel/3   3/5/04 2:44p nissen
 * PR 9132: Added new debug related entry-point functions. Fixed problem
 * with setting texture address.
 * 
 * Hydra_Software_Devel/2   2/20/04 3:44p nissen
 * PR 9132: More implementation.
 * 
 * Hydra_Software_Devel/1   1/22/04 2:46p nissen
 * PR 9132: Initial version.
 * 
 ***************************************************************************/

#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bint.h"
#include "bchp.h"
#include "bchp_px3d.h"
#include "bchp_common.h"
#include "bchp_hif_cpu_intr1.h"

#if ((BCHP_CHIP==7420) && (BCHP_VER <= BCHP_VER_A1))
#include "bchp_memc_0_1.h"
#include "bchp_memc_1_1.h"
#endif

#if (BCHP_CHIP==7400)
#include "bchp_int_id_px3d_l2.h"
#include "bchp_memc_0.h"
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
#include "bchp_int_id_gfx_l2.h"
#include "bchp_int_id_bvnf_intr2_1.h"
#include "bchp_memc_0_1.h"
#elif (BCHP_CHIP==7405)
#include "bchp_px3d_intr2.h"
#include "bchp_memc_0_1.h"
#elif (BCHP_CHIP==7336)
#include "bchp_px3d_intr2.h"
#elif (BCHP_CHIP!=7038)
#include "bchp_int_id_gfx_l2.h"
#include "bchp_gfx_l2.h"
#endif

#include "bp3d.h"
#include "bp3d_private.h"

BDBG_MODULE(BP3D);

#ifdef BP3D_P_ISR_TIMER
#include "btmr.h"
BTMR_DefaultSettings dsettings;
BTMR_Settings tsettings;
BTMR_Handle hTmr = 0;
BTMR_TimerHandle hTimer = 0;
#endif

/***************************************************************************/
BERR_Code BP3D_Open( 
	BP3D_Handle *phP3d,
	BCHP_Handle hChip,
	BREG_Handle hRegister,
	BMEM_Handle hMemory,
	BINT_Handle hInterrupt )
{
	BP3D_Handle hP3d = 0;
	BERR_Code err = BERR_SUCCESS;
	bool b3DGraphicsCapable;

	BDBG_ENTER(BP3D_Open);
	BDBG_ASSERT( phP3d );
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( hRegister );
	BDBG_ASSERT( hMemory );
	BDBG_ASSERT( hInterrupt );

#ifdef BP3D_P_ISR_TIMER
	BTMR_GetDefaultSettings( &dsettings );
	BTMR_Open( &hTmr, hChip, hRegister, hInterrupt, &dsettings );

	BTMR_GetDefaultTimerSettings( &tsettings );
	BTMR_CreateTimer( hTmr, &hTimer, &tsettings );
#endif

#if ((BCHP_CHIP==3548) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==3556) && (BCHP_VER == BCHP_VER_A0))
	BREG_Write32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_25, 0x4000 /*BCHP_MEMC_0_1_CLIENT_INFO_25_RR_EN_ENABLED*/ );
#endif

#if ((BCHP_CHIP==7420) && (BCHP_VER <= BCHP_VER_A1))
{
	/* Switch GFD from MEMC1 to MEMC0 */
	uint32_t ulMemClient0 = BREG_Read32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_27 );
	uint32_t ulMemClient1 = BREG_Read32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_27 );

	if( ulMemClient0 < ulMemClient1 )
	{
		BREG_Write32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_27, ulMemClient1 );
		BREG_Write32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_27, ulMemClient0 );

		ulMemClient0 = BREG_Read32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_28 );
		ulMemClient1 = BREG_Read32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_28 );
		BREG_Write32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_28, ulMemClient1 );
		BREG_Write32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_28, ulMemClient0 );

		ulMemClient0 = BREG_Read32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_29 );
		ulMemClient1 = BREG_Read32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_29 );
		BREG_Write32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_29, ulMemClient1 );
		BREG_Write32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_29, ulMemClient0 );
	}
}
#endif

	/* allocate memory for private data */
	hP3d = (BP3D_Handle) BKNI_Malloc( sizeof (BP3D_P_Handle) );
	if( hP3d == 0 )
	{
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		goto fail;
	}

	/* clear main data structure */
	BKNI_Memset( (void *) hP3d, 0, sizeof (BP3D_P_Handle) );

	/* check if PX3D device is present */
	err = BCHP_GetFeature( hChip, BCHP_Feature_e3DGraphicsCapable, (void *) (&b3DGraphicsCapable) );
	if( err != BERR_SUCCESS )
		goto fail;

	if( !b3DGraphicsCapable )
		goto fail;

	/* create interrupt callback */
#if ((BCHP_CHIP==3548) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==3556) && (BCHP_VER == BCHP_VER_A0))
	err = BINT_CreateCallback( &hP3d->hInterruptCallback, hInterrupt, 
		BCHP_INT_ID_BVNF_INTR2_1_RDC_TRIG_10_INTR, &BP3D_P_Render_Isr, hP3d, 0 );
#elif (BCHP_CHIP==7405) || (BCHP_CHIP==7336)
	err = BINT_CreateCallback( &hP3d->hInterruptCallback, hInterrupt, BCHP_INT_ID_CREATE(BCHP_PX3D_INTR2_R5F_STATUS, BCHP_PX3D_INTR2_R5F_STATUS_PX3D_2_INTR_SHIFT), &BP3D_P_Render_Isr, hP3d, 0 );
#else
	err = BINT_CreateCallback( &hP3d->hInterruptCallback, hInterrupt, 
		BCHP_INT_ID_PX3D_2_INTR, &BP3D_P_Render_Isr, hP3d, 0 );
#endif
	if( err != BERR_SUCCESS )
		goto fail;

	/* clear interrupt callback */
	err = BINT_ClearCallback( hP3d->hInterruptCallback );
	if( err != BERR_SUCCESS )
		goto fail;

	/* enable interrupt callback */
	err = BINT_EnableCallback( hP3d->hInterruptCallback );
	if( err != BERR_SUCCESS )
		goto fail;

#if (BCHP_CHIP==7405) || (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
	/* PX3D Fix: Store memory client */
	hP3d->ulMemClientInfoReg = BREG_Read32( hRegister, BP3D_P_MEMC_0_1_CLIENT_INFO() ) | 
		BP3D_P_MEMC_0_1_CLIENT_INFO(_BO_VAL_MASK) | BP3D_P_MEMC_0_1_CLIENT_INFO(_RR_EN_MASK);
	BREG_Write32( hRegister, BP3D_P_MEMC_0_1_CLIENT_INFO(), hP3d->ulMemClientInfoReg );
#endif

	/* allocate device memory for intermediate multi-render buffer */
	hP3d->pvTmpBuffer = BMEM_AllocAligned( hMemory, BP3D_P_TILE_WIDTH * BP3D_P_TILE_HEIGHT * 
		(BPXL_BITS_PER_PIXEL(BPXL_eA8_R8_G8_B8) / 8 + BPXL_BITS_PER_PIXEL(BPXL_eZ16) / 8), 2, 0 );
	if( hP3d->pvTmpBuffer == NULL )
	{
		err = BERR_OUT_OF_DEVICE_MEMORY;
		goto fail;
	}

	/* convert intermediate buffer address to offset */
	err = BMEM_ConvertAddressToOffset( hMemory, hP3d->pvTmpBuffer, &hP3d->TmpColorBuffer.ulOffset );
	if( err != BERR_SUCCESS )
		goto fail;

	/* set intermediate buffer data */
	hP3d->TmpColorBuffer.eFormat = BPXL_eA8_R8_G8_B8;
	hP3d->TmpColorBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->TmpColorBuffer.eFormat) / 8;
	hP3d->TmpColorBuffer.ulPitch = BP3D_P_TILE_WIDTH * hP3d->TmpColorBuffer.ulBpp;

	hP3d->TmpDepthBuffer.ulOffset = hP3d->TmpColorBuffer.ulOffset + hP3d->TmpColorBuffer.ulPitch * BP3D_P_TILE_HEIGHT;
	hP3d->TmpDepthBuffer.eFormat = BPXL_eZ16;
	hP3d->TmpDepthBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->TmpDepthBuffer.eFormat) / 8;
	hP3d->TmpDepthBuffer.ulPitch = BP3D_P_TILE_WIDTH * hP3d->TmpDepthBuffer.ulBpp;

	/* set private data */
	hP3d->hChip = hChip;
	hP3d->hRegister = hRegister;
	hP3d->hMemory = hMemory;
	hP3d->hInterrupt = hInterrupt;

	/* set return handle */
	*phP3d = hP3d;

	BDBG_LEAVE(BP3D_Open);
	return BERR_SUCCESS;

fail:
	if( hP3d )
	{
		if( hP3d->pvTmpBuffer )
			BMEM_Free( hMemory, hP3d->pvTmpBuffer );

		if( hP3d->hInterruptCallback )
			BINT_DestroyCallback( hP3d->hInterruptCallback );

		BKNI_Free( (void *) hP3d );
	}

	BDBG_LEAVE(BP3D_Open);
	return BERR_TRACE(err);
}

/***************************************************************************/
void BP3D_Close(
	BP3D_Handle hP3d )
{
	BDBG_ENTER(BP3D_Close);
	BDBG_ASSERT( hP3d );

#ifdef BP3D_P_ISR_TIMER
	if( hTimer )
		BTMR_DestroyTimer( hTimer );
	if( hTmr )
		BTMR_Close( hTmr );
#endif

#if (BCHP_CHIP==7405) || (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
	/* PX3D Fix: Restore memory client */
	BREG_Write32( hP3d->hRegister, BP3D_P_MEMC_0_1_CLIENT_INFO(), hP3d->ulMemClientInfoReg );
#endif

	BMEM_Free( hP3d->hMemory, hP3d->pvTmpBuffer );
	BINT_DisableCallback( hP3d->hInterruptCallback );
	BINT_DestroyCallback( hP3d->hInterruptCallback );
	BKNI_Free( (void *) hP3d );

	BDBG_LEAVE(BP3D_Close);
}

/***************************************************************************/
BERR_Code BP3D_Context_Create( 
	BP3D_Handle hP3d,
	BP3D_Context *phContext )
{
	BP3D_Context hContext = 0;
	BP3D_Context hCurrentContext;
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BP3D_Context_Create);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( phContext );

	/* allocate memory for private data */
	hContext = (BP3D_Context) BKNI_Malloc( sizeof (BP3D_P_Context) );
	if( hContext == 0 )
	{
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		goto fail;
	}

	BKNI_Memset( (void *) hContext, 0, sizeof (BP3D_P_Context) );

	/* create interrupt event */
	err = BKNI_CreateEvent( &hContext->hInterruptEvent );
	if( err != BERR_SUCCESS )
		goto fail;

	BKNI_ResetEvent( hContext->hInterruptEvent );

	/* save current context */
	hCurrentContext = hP3d->hContext;
	BP3D_Context_SetCurrent( hP3d, hContext );

	/* configure tiles */
	BP3D_ConfigureTiles( hP3d, BP3D_P_TILE_WIDTH, BP3D_P_TILE_HEIGHT, true, true );

	/* create empty point primitive */
	err = BP3D_P_CreateSpecialPrimitives( hP3d );
	if( err != BERR_SUCCESS )
		goto fail;

	/* set surfaces and viewport */
	BP3D_SetDstColorSurface( hP3d, (BSUR_Surface_Handle) NULL );
	BP3D_SetDstDepthSurface( hP3d, (BSUR_Surface_Handle) NULL );
	BP3D_SetSrcColorSurface( hP3d, (BSUR_Surface_Handle) NULL );
	BP3D_SetSrcDepthSurface( hP3d, (BSUR_Surface_Handle) NULL );
	BP3D_SetViewport( hP3d, 0, 0, 0, 0, 0, 0 );

	/* set state */
	BP3D_SetTextureSurface( hP3d, (BSUR_Surface_Handle) NULL );
	BP3D_SetTextureBlend( hP3d, BP3D_TextureBlend_Decal );
	BP3D_SetTextureFilter( hP3d, BP3D_TextureFilter_Point );
	BP3D_SetTextureWrap( hP3d, BP3D_TextureWrap_Tile );
	BP3D_SetTextureMipmap( hP3d, false, 0.0f, 0.0f );
	BP3D_SetTextureColorKey( hP3d, false, 0 );
	BP3D_SetTextureSwizzle( hP3d, false );
	BP3D_SetSrcBlend( hP3d, BP3D_Blend_One );
	BP3D_SetDstBlend( hP3d, BP3D_Blend_Zero );
	BP3D_SetDepthCompare( hP3d, BP3D_Compare_Less );
	BP3D_SetDepthMask( hP3d, false );
	BP3D_SetDepthFactorScale( hP3d, 0.0f );
	BP3D_SetAlphaCompare( hP3d, BP3D_Compare_Always );
	BP3D_SetAlphaReference( hP3d, 0xFF );
	BP3D_SetSpecular( hP3d, false );
	BP3D_SetFog( hP3d, BP3D_Fog_None );
	BP3D_SetFogColor( hP3d, 0 );
	BP3D_SetFogTable( hP3d, NULL );
	BP3D_SetShade( hP3d, BP3D_Shade_Gouraud );
	BP3D_SetCull( hP3d, BP3D_Cull_None );
	BP3D_SetDither( hP3d, true );
	
	/* set default register fields */
	hContext->aulRegs[BP3D_P_PX3D_Mode] &= ~(
#ifdef BP3D_P_PX3D_TIMER
		BCHP_MASK(PX3D_Mode, TriggerEnable) | 
#endif
#if (BCHP_CHIP==7420)
		BCHP_MASK(PX3D_Mode, Mixed_SCB_Mode) | 
#endif
		BCHP_MASK(PX3D_Mode, RoundCoordinates) | 
		BCHP_MASK(PX3D_Mode, AlphaWrite) | 
		BCHP_MASK(PX3D_Mode, ByteOrder) | 
		BCHP_MASK(PX3D_Mode, EnablePageFlip) | 
		BCHP_MASK(PX3D_Mode, LocalDisplayList) | 
		BCHP_MASK(PX3D_Mode, ScatterGather) | 
		BCHP_MASK(PX3D_Mode, DisableSrcMSB) | 
		BCHP_MASK(PX3D_Mode, OddCompare) | 
		BCHP_MASK(PX3D_Mode, OneSizeFitsAll) | 
		BCHP_MASK(PX3D_Mode, AlphaCompareEnable) | 
		BCHP_MASK(PX3D_Mode, LinearTexture) |
		BCHP_MASK(PX3D_Mode, PerspectiveZ));

	hContext->aulRegs[BP3D_P_PX3D_Mode] |= 
#ifdef BP3D_P_PX3D_TIMER
		BCHP_FIELD_DATA(PX3D_Mode, TriggerEnable, 1) | 
#endif
#if (BCHP_CHIP==7420)
		BCHP_FIELD_DATA(PX3D_Mode, Mixed_SCB_Mode, 1) | 
#endif
		BCHP_FIELD_ENUM(PX3D_Mode, RoundCoordinates, TruncateXY) | 
		BCHP_FIELD_ENUM(PX3D_Mode, AlphaWrite, Calculated) | 
		BCHP_FIELD_ENUM(PX3D_Mode, ByteOrder, LittleEndian32) | 
		BCHP_FIELD_DATA(PX3D_Mode, EnablePageFlip, 0) | 
		BCHP_FIELD_ENUM(PX3D_Mode, LocalDisplayList, FromMemorySCB) | 
		BCHP_FIELD_ENUM(PX3D_Mode, ScatterGather, TriangleScatterMode) | 
		BCHP_FIELD_DATA(PX3D_Mode, DisableSrcMSB, 0) | 
		BCHP_FIELD_ENUM(PX3D_Mode, OddCompare, GreaterZ) | 
		BCHP_FIELD_DATA(PX3D_Mode, OneSizeFitsAll, 1) | 
		BCHP_FIELD_DATA(PX3D_Mode, AlphaCompareEnable, 1) | 
		BCHP_FIELD_ENUM(PX3D_Mode, LinearTexture, Perspective) | 
		BCHP_FIELD_DATA(PX3D_Mode, PerspectiveZ, 0);

	/* set default primitive header fields */
	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, AlphaEnable, 1 ); 
	BP3D_P_SET_PRIMHEADER_DATA( TextureAddr, SampleTexelCenter, 1 );

	/* reset device */
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_ExceptionAction, 0 );
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_IRQMask, 0 );
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_RegTableAddr, 0 );
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_AddedRegSets, 0 );
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_PrimCount, 0 );
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_Reset, 0 );

	/* reset current context */
	BP3D_Context_SetCurrent( hP3d, hCurrentContext );

	*phContext = hContext;
	BDBG_LEAVE(BP3D_Context_Create);
	return BERR_SUCCESS;

fail:
	if( hContext )
	{
		if( hContext->hInterruptEvent )
			BKNI_DestroyEvent( hContext->hInterruptEvent );

		BKNI_Free( (void *) hContext );
	}

	BDBG_LEAVE(BP3D_Context_Create);
	return BERR_TRACE(err);
}

/***************************************************************************/
void BP3D_Context_Destroy(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	BDBG_ENTER(BP3D_Context_Destroy);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hContext );

	BP3D_P_WaitForRender( hP3d, hContext );

	if( hContext->pTiles )
		BP3D_P_DestroyTiles( hP3d, hContext );

	BP3D_P_DestroySpecialPrimitives( hP3d, hContext );
	BP3D_P_FreeAllMemoryBlocks( hP3d, hContext );

	if( hContext->hInterruptEvent )
		BKNI_DestroyEvent( hContext->hInterruptEvent );

	if( hContext == hP3d->hContext )
		BP3D_Context_SetCurrent( hP3d, 0 );

	if( hContext == hP3d->hRenderContext )
		hP3d->hRenderContext = NULL;

	BKNI_Free( (void *) hContext );
	BDBG_LEAVE(BP3D_Context_Destroy);
}

/***************************************************************************/
void BP3D_Context_SetCurrent(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	BDBG_ENTER(BP3D_Context_SetCurrent);
	BDBG_ASSERT( hP3d );

	hP3d->hContext = hContext;

	BDBG_LEAVE(BP3D_Context_SetCurrent);
}

/***************************************************************************/
BERR_Code BP3D_ConfigureTiles(
	BP3D_Handle hP3d,
	uint32_t ulTileWidth,
	uint32_t ulTileHeight,
	bool bMergeHorizontal,
	bool bMergeVertical )
{
	BERR_Code err;
	BDBG_ENTER(BP3D_ConfigureTiles);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( ulTileWidth );
	BDBG_ASSERT( ulTileHeight );

	/* re-create tiles if they are changing */
	if( (hP3d->hContext->ulTileWidth != ulTileWidth) || (hP3d->hContext->ulTileHeight != ulTileHeight) )
	{
		if( hP3d->hContext->pTiles )
		{
			hP3d->hContext->ulTileWidth = ulTileWidth;
			hP3d->hContext->ulTileHeight = ulTileHeight;

			BP3D_P_DestroyTiles( hP3d, hP3d->hContext );

			err = BP3D_P_CreateTiles( hP3d, hP3d->hContext );
			if( err != BERR_SUCCESS )
				return BERR_TRACE(err);
		}
	}

	hP3d->hContext->ulTileWidth = ulTileWidth;
	hP3d->hContext->ulTileHeight = ulTileHeight;
	hP3d->hContext->bTileMergeHorizontal = bMergeHorizontal;
	hP3d->hContext->bTileMergeVertical = bMergeVertical;

	BDBG_LEAVE(BP3D_ConfigureTiles);
	return BERR_SUCCESS;
}

/***************************************************************************/
int32_t BP3D_ConfigureMemoryBlockout(
	BP3D_Handle hP3d,
	int32_t lMemoryBlockout )
{
#if ((BCHP_CHIP==3548) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==3556) && (BCHP_VER == BCHP_VER_A0))
	uint32_t ulPrevValue;

	BDBG_ENTER(BP3D_ConfigureMemoryBlockout);
	BDBG_ASSERT( hP3d );

	ulPrevValue = BREG_Read32( hP3d->hRegister, BCHP_MEMC_0_1_CLIENT_INFO_25 );

	BREG_Write32( hP3d->hRegister, BCHP_MEMC_0_1_CLIENT_INFO_25, 
		(lMemoryBlockout >= 0) ? ((uint32_t) lMemoryBlockout) << 8 : BCHP_MEMC_0_1_CLIENT_INFO_25_RR_EN_ENABLED );

	BDBG_LEAVE(BP3D_ConfigureMemoryBlockout);
	return ((ulPrevValue & BCHP_MEMC_0_1_CLIENT_INFO_25_RR_EN_ENABLED) ? -1 : ((int32_t) (ulPrevValue >> 8)));
#else
	BDBG_ENTER(BP3D_ConfigureMemoryBlockout);
	BDBG_ASSERT( hP3d );
	BSTD_UNUSED( hP3d );
	BSTD_UNUSED( lMemoryBlockout );
	BDBG_LEAVE(BP3D_ConfigureMemoryBlockout);
	return 0;
#endif
}

/***************************************************************************/
BERR_Code BP3D_PrepareRender(
	BP3D_Handle hP3d )
{
	BDBG_ENTER(BP3D_PrepareRender);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( (hP3d->hContext->ulPrimCount == 0) && 
		(hP3d->hContext->hDstColorSurface == hP3d->hContext->hSrcColorSurface) && 
		(!hP3d->hContext->bClearColor) && (!hP3d->hContext->bClearDepth) )
		return BERR_SUCCESS;

	/* clear register list information */
	hP3d->hContext->ulRegSetOffset = 0;
	hP3d->hContext->ulRegSetCount = 0;
	hP3d->hContext->pulPrevRegSet = NULL;

	/* process tiles */
#ifdef BP3D_P_USE_BINS
	BP3D_P_ProcessTiles_Bins( hP3d );
#else
	if( hP3d->hContext->bTileMergeHorizontal && hP3d->hContext->bTileMergeVertical )
	{
		BP3D_P_ProcessTiles_HorzAndVertMerge( hP3d );
	}
	else if( hP3d->hContext->bTileMergeHorizontal )
	{
		BP3D_P_ProcessTiles_HorzMerge( hP3d );
	}
	else
	{
		BP3D_P_ProcessTiles_NoMerge( hP3d );
	}
#endif

	hP3d->hContext->bRenderPrepared = true;

	BDBG_LEAVE(BP3D_PrepareRender);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_IssueRender(
	BP3D_Handle hP3d,
	BP3D_Callback pCallback,
	void *pvParam1,
	int iParam2 )
{
	BDBG_ENTER(BP3D_IssueRender);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( (hP3d->hContext->ulPrimCount == 0) && 
		(hP3d->hContext->hDstColorSurface == hP3d->hContext->hSrcColorSurface) && 
		(!hP3d->hContext->bClearColor) && (!hP3d->hContext->bClearDepth) )
		return BERR_SUCCESS;

	if( !hP3d->hContext->bRenderPrepared )
		BP3D_PrepareRender( hP3d );

	BP3D_P_WaitForRender( hP3d, hP3d->hContext );

	if( hP3d->hContext->ulRegSetCount )
	{
		hP3d->hContext->pUserCallback = pCallback;
		hP3d->hContext->pUserParam1 = pvParam1;
		hP3d->hContext->iUserParam2 = iParam2;

		BP3D_P_InititateRender( hP3d, hP3d->hContext );
		hP3d->hContext->ulSceneNum++;
	}

	BP3D_P_FreeUnusedMemoryBlocks( hP3d, hP3d->hContext );
	BP3D_P_ResetPrimBlocks( hP3d );
	BP3D_P_ResetTiles( hP3d, hP3d->hContext );

	hP3d->hContext->ulPrimCount = 0;
	hP3d->hContext->ulColorKeyCount = 0;
	hP3d->hContext->bClearColor = false;
	hP3d->hContext->bClearDepth = false;
	hP3d->hContext->bRenderPrepared = false;

	BDBG_LEAVE(BP3D_IssueRender);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_IssueRenderAndWait(
	BP3D_Handle hP3d )
{
	BERR_Code err;

	BDBG_ENTER(BP3D_IssueRenderAndWait);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( (hP3d->hContext->ulPrimCount == 0) && 
		(hP3d->hContext->hDstColorSurface == hP3d->hContext->hSrcColorSurface) && 
		(!hP3d->hContext->bClearColor) && (!hP3d->hContext->bClearDepth) )
		return BERR_SUCCESS;

	err = BP3D_IssueRender( hP3d, NULL, NULL, 0 );
	if( err != BERR_SUCCESS )
		return BERR_TRACE(err);

	BP3D_P_WaitForRender( hP3d, hP3d->hContext );

	BDBG_LEAVE(BP3D_IssueRenderAndWait);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_IsRenderBusy(
	BP3D_Handle hP3d,
	bool *pbRenderBusy )
{
	BDBG_ENTER(BP3D_IsRenderBusy);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( hP3d->hContext->ulSceneNum == hP3d->hContext->ulRenderNum )
		*pbRenderBusy = false;
	else
		*pbRenderBusy = true;

	BDBG_LEAVE(BP3D_IsRenderBusy);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_FlushRender(
	BP3D_Handle hP3d )
{
	BDBG_ENTER(BP3D_FlushRender);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_WaitForRender( hP3d, hP3d->hContext );

	BDBG_LEAVE(BP3D_FlushRender);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_GetPrimitiveCount(
	BP3D_Handle hP3d,
	uint32_t *pulPrimCount )
{
	BDBG_ENTER(BP3D_GetPrimitiveCount);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( pulPrimCount )
		*pulPrimCount = hP3d->hContext->ulPrimCount;

	BDBG_LEAVE(BP3D_GetPrimitiveCount);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDstColorSurface(
	BP3D_Handle hP3d,
	BSUR_Surface_Handle hSurface )
{
	BDBG_ENTER(BP3D_SetDstColorSurface);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->hDstColorSurface = NULL;
	
	if( hSurface )
	{
		BERR_Code err;

		err = BSUR_Surface_GetFormat( hSurface, &hP3d->hContext->DstColorBuffer.eFormat );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetAddress( hSurface, &hP3d->hContext->DstColorBuffer.pvData, &hP3d->hContext->DstColorBuffer.ulPitch );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetOffset( hSurface, &hP3d->hContext->DstColorBuffer.ulOffset );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		if( (hP3d->hContext->DstColorBuffer.eFormat != BPXL_eA8_R8_G8_B8) && 
			(hP3d->hContext->DstColorBuffer.eFormat != BPXL_eR5_G6_B5) &&
			(hP3d->hContext->DstColorBuffer.eFormat != BPXL_eA1_R5_G5_B5) && 
			(hP3d->hContext->DstColorBuffer.eFormat != BPXL_eA4_R4_G4_B4) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		hP3d->hContext->DstColorBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->hContext->DstColorBuffer.eFormat) / 8;
		hP3d->hContext->hDstColorSurface = hSurface;
	}

	BDBG_LEAVE(BP3D_SetDstColorSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDstColorPlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch )
{
	BDBG_ENTER(BP3D_SetDstColorPlane);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->hDstColorSurface = NULL;
	
	if( ulOffset )
	{
		hP3d->hContext->DstColorBuffer.eFormat = eFormat;
		hP3d->hContext->DstColorBuffer.pvData = 0;
		hP3d->hContext->DstColorBuffer.ulPitch = ulPitch;
		hP3d->hContext->DstColorBuffer.ulOffset = ulOffset;

		if( (hP3d->hContext->DstColorBuffer.eFormat != BPXL_eA8_R8_G8_B8) && 
			(hP3d->hContext->DstColorBuffer.eFormat != BPXL_eR5_G6_B5) &&
			(hP3d->hContext->DstColorBuffer.eFormat != BPXL_eA1_R5_G5_B5) && 
			(hP3d->hContext->DstColorBuffer.eFormat != BPXL_eA4_R4_G4_B4) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		hP3d->hContext->DstColorBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->hContext->DstColorBuffer.eFormat) / 8;
		hP3d->hContext->hDstColorSurface = (BSUR_Surface_Handle) ulOffset;
	}

	BDBG_LEAVE(BP3D_SetDstColorPlane);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDstDepthSurface(
	BP3D_Handle hP3d,
	BSUR_Surface_Handle hSurface )
{
	BDBG_ENTER(BP3D_SetDstDepthSurface);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->hDstDepthSurface = NULL;

	if( hSurface )
	{
		BERR_Code err;

		err = BSUR_Surface_GetFormat( hSurface, &hP3d->hContext->DstDepthBuffer.eFormat );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetAddress( hSurface, &hP3d->hContext->DstDepthBuffer.pvData, &hP3d->hContext->DstDepthBuffer.ulPitch );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetOffset( hSurface, &hP3d->hContext->DstDepthBuffer.ulOffset );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		if( hP3d->hContext->DstDepthBuffer.eFormat != BPXL_eZ16 )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		hP3d->hContext->DstDepthBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->hContext->DstDepthBuffer.eFormat) / 8;
		hP3d->hContext->hDstDepthSurface = hSurface;
	}

	BDBG_LEAVE(BP3D_SetDstDepthSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDstDepthPlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch )
{
	BDBG_ENTER(BP3D_SetDstDepthPlane);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->hDstDepthSurface = NULL;
	
	if( ulOffset )
	{
		hP3d->hContext->DstDepthBuffer.eFormat = eFormat;
		hP3d->hContext->DstDepthBuffer.pvData = 0;
		hP3d->hContext->DstDepthBuffer.ulPitch = ulPitch;
		hP3d->hContext->DstDepthBuffer.ulOffset = ulOffset;

		if( hP3d->hContext->DstDepthBuffer.eFormat != BPXL_eZ16 )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		hP3d->hContext->DstDepthBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->hContext->DstDepthBuffer.eFormat) / 8;
		hP3d->hContext->hDstDepthSurface = (BSUR_Surface_Handle) ulOffset;
	}

	BDBG_LEAVE(BP3D_SetDstDepthPlane);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetSrcColorSurface(
	BP3D_Handle hP3d,
	BSUR_Surface_Handle hSurface )
{
	BDBG_ENTER(BP3D_SetSrcColorSurface);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->hSrcColorSurface = NULL;

	if( hSurface )
	{
		BERR_Code err;

		err = BSUR_Surface_GetFormat( hSurface, &hP3d->hContext->SrcColorBuffer.eFormat );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetAddress( hSurface, &hP3d->hContext->SrcColorBuffer.pvData, &hP3d->hContext->SrcColorBuffer.ulPitch );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetOffset( hSurface, &hP3d->hContext->SrcColorBuffer.ulOffset );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		if( (hP3d->hContext->SrcColorBuffer.eFormat != BPXL_eA8_R8_G8_B8) && 
			(hP3d->hContext->SrcColorBuffer.eFormat != BPXL_eR5_G6_B5) &&
			(hP3d->hContext->SrcColorBuffer.eFormat != BPXL_eA1_R5_G5_B5) && 
			(hP3d->hContext->SrcColorBuffer.eFormat != BPXL_eA4_R4_G4_B4) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		hP3d->hContext->SrcColorBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->hContext->SrcColorBuffer.eFormat) / 8;
		hP3d->hContext->hSrcColorSurface = hSurface;
	}

	BDBG_LEAVE(BP3D_SetSrcColorSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetSrcColorPlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch )
{
	BDBG_ENTER(BP3D_SetSrcColorPlane);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->hSrcColorSurface = NULL;
	
	if( ulOffset )
	{
		hP3d->hContext->SrcColorBuffer.eFormat = eFormat;
		hP3d->hContext->SrcColorBuffer.pvData = 0;
		hP3d->hContext->SrcColorBuffer.ulPitch = ulPitch;
		hP3d->hContext->SrcColorBuffer.ulOffset = ulOffset;

		if( (hP3d->hContext->SrcColorBuffer.eFormat != BPXL_eA8_R8_G8_B8) && 
			(hP3d->hContext->SrcColorBuffer.eFormat != BPXL_eR5_G6_B5) &&
			(hP3d->hContext->SrcColorBuffer.eFormat != BPXL_eA1_R5_G5_B5) && 
			(hP3d->hContext->SrcColorBuffer.eFormat != BPXL_eA4_R4_G4_B4) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		hP3d->hContext->SrcColorBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->hContext->SrcColorBuffer.eFormat) / 8;
		hP3d->hContext->hSrcColorSurface = (BSUR_Surface_Handle) ulOffset;
	}

	BDBG_LEAVE(BP3D_SetSrcColorPlane);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetSrcDepthSurface(
	BP3D_Handle hP3d,
	BSUR_Surface_Handle hSurface )
{
	BDBG_ENTER(BP3D_SetSrcDepthSurface);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->hSrcDepthSurface = NULL;

	if( hSurface )
	{
		BERR_Code err;

		err = BSUR_Surface_GetFormat( hSurface, &hP3d->hContext->SrcDepthBuffer.eFormat );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetAddress( hSurface, &hP3d->hContext->SrcDepthBuffer.pvData, &hP3d->hContext->SrcDepthBuffer.ulPitch );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetOffset( hSurface, &hP3d->hContext->SrcDepthBuffer.ulOffset );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		if( hP3d->hContext->SrcDepthBuffer.eFormat != BPXL_eZ16 )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		hP3d->hContext->SrcDepthBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->hContext->SrcDepthBuffer.eFormat) / 8;
		hP3d->hContext->hSrcDepthSurface = hSurface;
	}

	BDBG_LEAVE(BP3D_SetSrcDepthSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetSrcDepthPlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch )
{
	BDBG_ENTER(BP3D_SetSrcDepthPlane);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->hSrcDepthSurface = NULL;
	
	if( ulOffset )
	{
		hP3d->hContext->SrcDepthBuffer.eFormat = eFormat;
		hP3d->hContext->SrcDepthBuffer.pvData = 0;
		hP3d->hContext->SrcDepthBuffer.ulPitch = ulPitch;
		hP3d->hContext->SrcDepthBuffer.ulOffset = ulOffset;

		if( hP3d->hContext->SrcDepthBuffer.eFormat != BPXL_eZ16 )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		hP3d->hContext->SrcDepthBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->hContext->SrcDepthBuffer.eFormat) / 8;
		hP3d->hContext->hSrcDepthSurface = (BSUR_Surface_Handle) ulOffset;
	}

	BDBG_LEAVE(BP3D_SetSrcDepthPlane);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetViewport(
	BP3D_Handle hP3d,
	uint32_t ulX,
	uint32_t ulY,
	uint32_t ulWidth,
	uint32_t ulHeight,
	uint32_t ulOffsetX,
	uint32_t ulOffsetY )
{
	uint32_t ulViewportWidth;
	uint32_t ulViewportHeight;
	BERR_Code err;

	BDBG_ENTER(BP3D_SetViewport);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( (ulWidth == 0) || (ulHeight == 0) )
		return BERR_SUCCESS;

	ulViewportWidth = hP3d->hContext->Viewport.usMaxX - hP3d->hContext->Viewport.usMinX;
	ulViewportHeight = hP3d->hContext->Viewport.usMaxY - hP3d->hContext->Viewport.usMinY;

#ifdef EGL_P_CONFORMANT
	/* ensure viewport is at least 6 pixels */
	if( ulWidth * ulHeight < 6 )
	{
		ulWidth = BP3D_P_MAX(ulWidth, 2);
		ulHeight = BP3D_P_MAX(ulHeight, 2);
	}
#endif

	hP3d->hContext->Viewport.usMinX = (uint16_t) ulX;
	hP3d->hContext->Viewport.usMinY = (uint16_t) ulY;
	hP3d->hContext->Viewport.usMaxX = (uint16_t) (ulX + ulWidth);
	hP3d->hContext->Viewport.usMaxY = (uint16_t) (ulY + ulHeight);
	hP3d->hContext->ulBufferOffsetX = ulOffsetX;
	hP3d->hContext->ulBufferOffsetY = ulOffsetY;

	if( (ulViewportWidth != ulWidth) || (ulViewportHeight != ulHeight) )
	{
		if( hP3d->hContext->pTiles )
			BP3D_P_DestroyTiles( hP3d, hP3d->hContext );

		err = BP3D_P_CreateTiles( hP3d, hP3d->hContext );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);
	}

	BDBG_LEAVE(BP3D_SetViewport);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetTextureSurface(
	BP3D_Handle hP3d,
	BSUR_Surface_Handle hSurface )
{
	uint32_t ulWidth = 0;
	uint32_t ulHeight = 0;

	BDBG_ENTER(BP3D_SetTextureSurface);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, TextureEnable, hSurface ? 1 : 0 );
	BP3D_P_SET_PRIMHEADER_OPAQUE();

	if( hSurface )
	{
		BERR_Code err;
		BPXL_Format eFormat;
		void *pvImageData;
		uint32_t ulOffset, ulPitch, ulSize, ulSizeS, ulSizeT;

		err = BSUR_Surface_GetFormat( hSurface, &eFormat );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetDimensions( hSurface, &ulWidth, &ulHeight );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetAddress( hSurface, &pvImageData, &ulPitch );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		err = BSUR_Surface_GetOffset( hSurface, &ulOffset );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);

		switch( eFormat )
		{
		case BPXL_eA8_R8_G8_B8:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, ARGB8888 ); 
			break;
		case BPXL_eR5_G6_B5:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, RGB565 ); 
			break;
		case BPXL_eA1_R5_G5_B5:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, ARGB1555 ); 
			break;
		case BPXL_eA4_R4_G4_B4:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, ARGB4444 ); 
			break;
		case BPXL_eY18_Cr8_Y08_Cb8:
		case BPXL_eY08_Cb8_Y18_Cr8:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, Y0CbY1Cr ); 
			break;
		case BPXL_eL8:
		case BPXL_eA8:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, Luma8 ); 
			break;
		case BPXL_eL8_A8:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, LumaAlpha88 ); 
			break;
		default:
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		ulSize = 0;
		ulSizeS = ulWidth;
		while( ulSizeS > BP3D_P_TEXTURE_SIZE_MIN ) { ulSizeS /= 2; ulSize++; }
		BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, TextureSizeS, ulSize );

		ulSize = 0;
		ulSizeT = ulHeight;
		while( ulSizeT > BP3D_P_TEXTURE_SIZE_MIN ) { ulSizeT /= 2; ulSize++; }
		BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, TextureSizeT, ulSize );

		BP3D_P_SET_PRIMHEADER_ADDR( ulOffset );
	}

	hP3d->hContext->hTextureSurface = hSurface;
	hP3d->hContext->ulTextureWidth = ulWidth;
	hP3d->hContext->ulTextureHeight = ulHeight;

	BDBG_LEAVE(BP3D_SetTextureSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetTexturePlane(
	BP3D_Handle hP3d,
	BPXL_Format eFormat,
	uint32_t ulOffset,
	uint32_t ulPitch,
	uint32_t ulWidth,
	uint32_t ulHeight )
{
	BDBG_ENTER(BP3D_SetTexturePlane);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BSTD_UNUSED( ulPitch );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, TextureEnable, ulOffset ? 1 : 0 );
	BP3D_P_SET_PRIMHEADER_OPAQUE();

	if( ulOffset )
	{
		uint32_t ulSize;
		uint32_t ulSizeS;
		uint32_t ulSizeT;

		switch( eFormat )
		{
		case BPXL_eA8_R8_G8_B8:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, ARGB8888 ); 
			break;
		case BPXL_eR5_G6_B5:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, RGB565 ); 
			break;
		case BPXL_eA1_R5_G5_B5:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, ARGB1555 ); 
			break;
		case BPXL_eA4_R4_G4_B4:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, ARGB4444 ); 
			break;
		case BPXL_eY18_Cr8_Y08_Cb8:
		case BPXL_eY08_Cb8_Y18_Cr8:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, Y0CbY1Cr ); 
			break;
		case BPXL_eL8:
		case BPXL_eA8:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, Luma8 ); 
			break;
		case BPXL_eL8_A8:
			BP3D_P_SET_PRIMHEADER_ENUM( PrimHeaderA, TexelFormat, LumaAlpha88 ); 
			break;
		default:
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		ulSize = 0;
		ulSizeS = ulWidth;
		while( ulSizeS > BP3D_P_TEXTURE_SIZE_MIN ) { ulSizeS /= 2; ulSize++; }
		BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, TextureSizeS, ulSize );

		ulSize = 0;
		ulSizeT = ulHeight;
		while( ulSizeT > BP3D_P_TEXTURE_SIZE_MIN ) { ulSizeT /= 2; ulSize++; }
		BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, TextureSizeT, ulSize );

		BP3D_P_SET_PRIMHEADER_ADDR( ulOffset );
	}

	hP3d->hContext->hTextureSurface = (BSUR_Surface_Handle) ulOffset;
	hP3d->hContext->ulTextureWidth = ulWidth;
	hP3d->hContext->ulTextureHeight = ulHeight;

	BDBG_LEAVE(BP3D_SetTexturePlane);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetTextureBlend(
	BP3D_Handle hP3d,
	BP3D_TextureBlend eTextureBlend )
{
	BDBG_ENTER(BP3D_SetTextureBlend);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, TextureBlend, eTextureBlend );

	BDBG_LEAVE(BP3D_SetTextureBlend);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetTextureFilter(
	BP3D_Handle hP3d,
	BP3D_TextureFilter eTextureFilter )
{
	BDBG_ENTER(BP3D_SetTextureFilter);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( TextureAddr, InterpType, eTextureFilter );

	BDBG_LEAVE(BP3D_SetTextureFilter);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetTextureWrap(
	BP3D_Handle hP3d,
	BP3D_TextureWrap eTextureWrap )
{
	BDBG_ENTER(BP3D_SetTextureWrap);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( TextureAddr, WrapType, eTextureWrap );

	BDBG_LEAVE(BP3D_SetTextureWrap);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetTextureMipmap(
	BP3D_Handle hP3d,
	bool bEnableMipmap,
	float fLodMax,
	float fLodBias )
{
	BDBG_ENTER(BP3D_SetTextureMipmap);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->bMipmap = bEnableMipmap;
	hP3d->hContext->fMipmapLodMax = fLodMax;
	hP3d->hContext->fMipmapLodBias = fLodBias;

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, MIPData, bEnableMipmap ? 1 : 0 );

	BDBG_LEAVE(BP3D_SetTextureMipmap);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetTextureColorKey(
	BP3D_Handle hP3d,
	bool bEnable,
	uint32_t ulColorKey )
{
	uint32_t ulColorKeyNum = 0;

	BDBG_ENTER(BP3D_SetTextureColorKey);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( bEnable )
	{
		for( ulColorKeyNum = 0; ulColorKeyNum < hP3d->hContext->ulColorKeyCount; ++ulColorKeyNum )
		{
			if( hP3d->hContext->ulColorKey[ulColorKeyNum] == ulColorKey )
				break;
		}

		if( (hP3d->hContext->ulColorKeyCount == ulColorKeyNum) &&
			(hP3d->hContext->ulColorKeyCount < BP3D_P_COLOR_KEY_COUNT) )
		{
			hP3d->hContext->ulColorKeyCount++;
			hP3d->hContext->ulColorKey[ulColorKeyNum] = ulColorKey;
		}
	}

	BP3D_P_SET_PRIMHEADER_DATA( TextureAddr, ColorKeySelect, bEnable ? ulColorKeyNum + 1 : 0 );
	BP3D_P_SET_PRIMHEADER_OPAQUE();

	BDBG_LEAVE(BP3D_SetTextureColorKey);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetTextureSwizzle(
	BP3D_Handle hP3d,
	bool bEnable )
{
	BDBG_ENTER(BP3D_SetTextureSwizzle);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( TextureAddr, Swizzled, bEnable ? 1 : 0 );

	BDBG_LEAVE(BP3D_SetTextureSwizzle);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetSrcBlend(
	BP3D_Handle hP3d,
	BP3D_Blend eSrcBlend )
{
	BDBG_ENTER(BP3D_SetSrcBlend);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, SrcAlphaBlend, eSrcBlend );
	BP3D_P_SET_PRIMHEADER_OPAQUE();

	BDBG_LEAVE(BP3D_SetSrcBlend);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDstBlend(
	BP3D_Handle hP3d,
	BP3D_Blend eDstBlend )
{
	BDBG_ENTER(BP3D_SetDstBlend);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, DstAlphaBlend, eDstBlend );
	BP3D_P_SET_PRIMHEADER_OPAQUE();

	BDBG_LEAVE(BP3D_SetDstBlend);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDepthCompare(
	BP3D_Handle hP3d,
	BP3D_Compare eDepthCompare )
{
	BDBG_ENTER(BP3D_SetDepthCompare);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderB, ZCompare, eDepthCompare );

	BDBG_LEAVE(BP3D_SetDepthCompare);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDepthMask(
	BP3D_Handle hP3d,
	bool bDepthMask )
{
	BDBG_ENTER(BP3D_SetDepthMask);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderB, DepthMask, bDepthMask ? 1 : 0 );
	BP3D_P_SET_PRIMHEADER_OPAQUE();

	BDBG_LEAVE(BP3D_SetDepthMask);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDepthFactorScale(
	BP3D_Handle hP3d,
	float fScale )
{
	BDBG_ENTER(BP3D_SetDepthFactorScale);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->fDepthFactorScale = fScale;

	BDBG_LEAVE(BP3D_SetDepthFactorScale);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetAlphaCompare(
	BP3D_Handle hP3d,
	BP3D_Compare eAlphaCompare )
{
	BDBG_ENTER(BP3D_SetAlphaCompare);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, AlphaCompare, eAlphaCompare );
	BP3D_P_SET_PRIMHEADER_OPAQUE();

	BDBG_LEAVE(BP3D_SetAlphaCompare);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetAlphaReference(
	BP3D_Handle hP3d,
	uint8_t ucAlphaReference )
{
	BDBG_ENTER(BP3D_SetAlphaReference);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_REGISTER_DATA( AlphaReference, AlphaReference, ucAlphaReference );

	BDBG_LEAVE(BP3D_SetAlphaReference);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetSpecular(
	BP3D_Handle hP3d,
	bool bEnableSpecular )
{
	BDBG_ENTER(BP3D_SetSpecular);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, SpecularEnable, bEnableSpecular ? 1 : 0 );

	BDBG_LEAVE(BP3D_SetSpecular);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetFog(
	BP3D_Handle hP3d,
	BP3D_Fog eFog )
{
	BDBG_ENTER(BP3D_SetFog);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->eFog = eFog;
	if( eFog == BP3D_Fog_Table )
		hP3d->hContext->bFogTable = true;

	BP3D_P_SET_PRIMHEADER_DATA( PrimHeaderA, Fog, eFog );

	BDBG_LEAVE(BP3D_SetFog);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetFogColor(
	BP3D_Handle hP3d,
	uint32_t ulColor )
{
	BDBG_ENTER(BP3D_SetFogColor);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->ulFogColor = ulColor;
	BP3D_P_SET_REGISTER_DATA( FogColor, Color, ulColor );

	BDBG_LEAVE(BP3D_SetFogColor);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetFogTable(
	BP3D_Handle hP3d,
	uint8_t *pucFogTable )
{
	BDBG_ENTER(BP3D_SetFogTable);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->pucFogTable = pucFogTable;

	BDBG_LEAVE(BP3D_SetFogTable);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetShade(
	BP3D_Handle hP3d,
	BP3D_Shade eShade )
{
	BDBG_ENTER(BP3D_SetShade);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->ePrimitiveShade = eShade;

	if( eShade == BP3D_Shade_Flat )
	{
/*		BP3D_P_SET_REGISTER_ENUM( Mode, LinearTexture, Linear );*/
		BP3D_P_SET_REGISTER_ENUM( Mode, LinearColor, Linear );
		BP3D_P_SET_REGISTER_ENUM( Mode, LinearSpecular, Linear );
		BP3D_P_SET_REGISTER_ENUM( Mode, LinearFog, Linear );
	}
	else
	{
/*		BP3D_P_SET_REGISTER_ENUM( Mode, LinearTexture, Perspective );*/
		BP3D_P_SET_REGISTER_ENUM( Mode, LinearColor, Perspective );
		BP3D_P_SET_REGISTER_ENUM( Mode, LinearSpecular, Perspective );
		BP3D_P_SET_REGISTER_ENUM( Mode, LinearFog, Perspective );
	}

	BDBG_LEAVE(BP3D_SetShade);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetCull(
	BP3D_Handle hP3d,
	BP3D_Cull eCull )
{
	BDBG_ENTER(BP3D_SetCull);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	hP3d->hContext->ePrimitiveCull = eCull;

	BDBG_LEAVE(BP3D_SetCull);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_SetDither(
	BP3D_Handle hP3d,
	bool bEnableDither )
{
	BDBG_ENTER(BP3D_SetDither);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	BP3D_P_SET_REGISTER_DATA( Mode, DisableDither, bEnableDither ? 0 : 1 );

	BDBG_LEAVE(BP3D_SetDither);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_ClearColor(
	BP3D_Handle hP3d,
	uint32_t ulColor )
{
	BDBG_ENTER(BP3D_ClearColor);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( (ulColor >> 24) != 0xFF )
		BP3D_P_AddColorRectangleToTiles( hP3d, ulColor );

	hP3d->hContext->bClearColor = true;

	BP3D_P_SET_REGISTER_DATA( TranslateColorKey, Color, ulColor );

	BDBG_LEAVE(BP3D_ClearColor);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_ClearDepth(
	BP3D_Handle hP3d,
	float fDepth )
{
	BDBG_ENTER(BP3D_ClearDepth);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );

	if( fDepth != 1.0f )
		BP3D_P_AddDepthRectangleToTiles( hP3d, fDepth );

	hP3d->hContext->bClearDepth = true;

	BDBG_LEAVE(BP3D_ClearDepth);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawPoint(
	BP3D_Handle hP3d,
	BP3D_Vertex *pV0 )
{
	BP3D_P_Primitive *pPrim;

	BDBG_ENTER(BP3D_DrawPoint);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pV0 );

	pPrim = BP3D_P_AllocPrimitives( hP3d, 1 );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	BP3D_P_CopyPoint( hP3d, pPrim, pV0 );

	BDBG_LEAVE(BP3D_DrawPoint);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawLine(
	BP3D_Handle hP3d,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1 )
{
	BP3D_P_Primitive *pPrim;

	BDBG_ENTER(BP3D_DrawLine);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pV0 );
	BDBG_ASSERT( pV1 );

	pPrim = BP3D_P_AllocPrimitives( hP3d, 1 );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	BP3D_P_CopyLine( hP3d, pPrim, pV0, pV1 );

	BDBG_LEAVE(BP3D_DrawLine);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawRectangle(
	BP3D_Handle hP3d,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1 )
{
	BP3D_P_Primitive *pPrim;

	BDBG_ENTER(BP3D_DrawRectangle);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pV0 );
	BDBG_ASSERT( pV1 );

	pPrim = BP3D_P_AllocPrimitives( hP3d, 1 );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	BP3D_P_CopyRectangle( hP3d, pPrim, pV0, pV1 );

	BDBG_LEAVE(BP3D_DrawRectangle);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawTriangle(
	BP3D_Handle hP3d,
	BP3D_Vertex *pV0,
	BP3D_Vertex *pV1,
	BP3D_Vertex *pV2 )
{
	BP3D_P_Primitive *pPrim;

	BDBG_ENTER(BP3D_DrawTriangle);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pV0 );
	BDBG_ASSERT( pV1 );
	BDBG_ASSERT( pV2 );

	pPrim = BP3D_P_AllocPrimitives( hP3d, 1 );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );

	BDBG_LEAVE(BP3D_DrawTriangle);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawTriangleList(
	BP3D_Handle hP3d,
	BP3D_Vertex *pVertices,
	uint32_t ulTriangleCount )
{
	BP3D_P_Primitive *pPrim;
	uint32_t ii;

	BDBG_ENTER(BP3D_DrawTriangleList);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pVertices );
	BDBG_ASSERT( ulTriangleCount );

	pPrim = BP3D_P_AllocPrimitives( hP3d, ulTriangleCount );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	for( ii = 0; ii < ulTriangleCount; ++ii )
	{
		BP3D_Vertex *pV0 = pVertices++;
		BP3D_Vertex *pV1 = pVertices++;
		BP3D_Vertex *pV2 = pVertices++;

		pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );
	}

	BDBG_LEAVE(BP3D_DrawTriangleList);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawTriangleStrip(
	BP3D_Handle hP3d,
	BP3D_Vertex *pVertices,
	uint32_t ulTriangleCount )
{
	BP3D_P_Primitive *pPrim;
	BP3D_Vertex *pV0 = pVertices++;
	BP3D_Vertex *pV1 = pVertices++;
	uint32_t ii;

	BDBG_ENTER(BP3D_DrawTriangleStrip);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pVertices );
	BDBG_ASSERT( ulTriangleCount );

	pPrim = BP3D_P_AllocPrimitives( hP3d, ulTriangleCount );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	for( ii = 0; ii < ulTriangleCount / 2; ++ii )
	{
		BP3D_Vertex *pV2 = pVertices++;
		BP3D_Vertex *pV3 = pVertices++;

		pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );
		pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV1, pV3, pV2 );
		pV0 = pV2;
		pV1 = pV3;
	}

	if( ulTriangleCount & 1 )
		BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pVertices );

	BDBG_LEAVE(BP3D_DrawTriangleStrip);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawTriangleFan(
	BP3D_Handle hP3d,
	BP3D_Vertex *pVertices,
	uint32_t ulTriangleCount )
{
	BP3D_P_Primitive *pPrim;
	BP3D_Vertex *pV0 = pVertices++;
	BP3D_Vertex *pV1 = pVertices++;
	BP3D_Vertex *pV2 = pVertices++;
	uint32_t ii;

	BDBG_ENTER(BP3D_DrawTriangleFan);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pVertices );
	BDBG_ASSERT( ulTriangleCount );

	pPrim = BP3D_P_AllocPrimitives( hP3d, ulTriangleCount );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );
  
	for( ii = 0; ii < ulTriangleCount - 1; ++ii )
	{
		pV1 = pV2;
		pV2 = pVertices++;
		pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );
	}

	BDBG_LEAVE(BP3D_DrawTriangleFan);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawIndexedTriangleList(
	BP3D_Handle hP3d,
	BP3D_Vertex *pVertices,
	uint16_t *pusIndices,
	uint32_t ulTriangleCount )
{
	BP3D_P_Primitive *pPrim;
	uint32_t ii;

	BDBG_ENTER(BP3D_DrawIndexedTriangleList);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pVertices );
	BDBG_ASSERT( pusIndices );
	BDBG_ASSERT( ulTriangleCount );

	pPrim = BP3D_P_AllocPrimitives( hP3d, ulTriangleCount );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	for( ii = 0; ii < ulTriangleCount; ++ii )
	{
		BP3D_Vertex *pV0 = pVertices + (*pusIndices++);
		BP3D_Vertex *pV1 = pVertices + (*pusIndices++);
		BP3D_Vertex *pV2 = pVertices + (*pusIndices++);

		pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );
	}

	BDBG_LEAVE(BP3D_DrawIndexedTriangleList);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawIndexedTriangleStrip(
	BP3D_Handle hP3d,
	BP3D_Vertex *pVertices,
	uint16_t *pusIndices,
	uint32_t ulTriangleCount )
{
	BP3D_P_Primitive *pPrim;
	BP3D_Vertex *pV0 = pVertices + (*pusIndices++);
	BP3D_Vertex *pV1 = pVertices + (*pusIndices++);
	uint32_t ii;

	BDBG_ENTER(BP3D_DrawIndexedTriangleStrip);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pVertices );
	BDBG_ASSERT( pusIndices );
	BDBG_ASSERT( ulTriangleCount );

	pPrim = BP3D_P_AllocPrimitives( hP3d, ulTriangleCount );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	for( ii = 0; ii < ulTriangleCount / 2; ++ii )
	{
		BP3D_Vertex *pV2 = pVertices + (*pusIndices++);
		BP3D_Vertex *pV3 = pVertices + (*pusIndices++);

		pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );
		pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV1, pV3, pV2 );
		pV0 = pV2;
		pV1 = pV3;
	}

	if( ulTriangleCount & 1 )
		BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pVertices + (*pusIndices) );

	BDBG_LEAVE(BP3D_DrawIndexedTriangleStrip);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_DrawIndexedTriangleFan(
	BP3D_Handle hP3d,
	BP3D_Vertex *pVertices,
	uint16_t *pusIndices,
	uint32_t ulTriangleCount )
{
	BP3D_P_Primitive *pPrim;
	BP3D_Vertex *pV0 = pVertices + (*pusIndices++);
	BP3D_Vertex *pV1 = pVertices + (*pusIndices++);
	BP3D_Vertex *pV2 = pVertices + (*pusIndices++);
	uint32_t ii;

	BDBG_ENTER(BP3D_DrawIndexedTriangleFan);
	BDBG_ASSERT( hP3d );
	BDBG_ASSERT( hP3d->hContext );
	BDBG_ASSERT( pVertices );
	BDBG_ASSERT( pusIndices );
	BDBG_ASSERT( ulTriangleCount );

	pPrim = BP3D_P_AllocPrimitives( hP3d, ulTriangleCount );
	if( pPrim == NULL )
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);

	pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );

	for( ii = 0; ii < ulTriangleCount - 1; ++ii )
	{
		pV1 = pV2;
		pV2 = pVertices + (*pusIndices++);
		pPrim = BP3D_P_CopyTriangle( hP3d, pPrim, pV0, pV1, pV2 );
	}

	BDBG_LEAVE(BP3D_DrawIndexedTriangleFan);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BP3D_ShowTiles(
	BP3D_Handle hP3d,
	bool bShowTiles )
{
	BDBG_ENTER(BP3D_ShowTiles);
	BDBG_ASSERT( hP3d );

	hP3d->bShowTiles = bShowTiles;

	BDBG_LEAVE(BP3D_ShowTiles);
	return BERR_SUCCESS;
}

/* End of File */
