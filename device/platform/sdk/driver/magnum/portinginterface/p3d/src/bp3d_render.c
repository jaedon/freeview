/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bp3d_render.c $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 3/3/10 6:30p $
 *
 * Module Description: Private 3D Rendering Render Functions
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/p3d/7405/bp3d_render.c $
 * 
 * Hydra_Software_Devel/22   3/3/10 6:30p nissen
 * SW7405-3704: Added use of memory block out when using a source color
 * buffer.
 * 
 * Hydra_Software_Devel/21   3/1/10 11:21a nissen
 * SW7405-3704: Added use of memory block out when rendering with a depth
 * buffer for 3548/3556.
 * 
 * Hydra_Software_Devel/20   1/4/10 5:27p nissen
 * SW7405-3704: Added use of memory block out when rendering with a depth
 * buffer to avoid possible hang on 7405.
 * 
 * Hydra_Software_Devel/19   8/4/09 8:30p nissen
 * PR 57173: Fixed coverity defect involving context handle.
 * 
 * Hydra_Software_Devel/18   12/3/08 6:58p nissen
 * PR 47763: Added support for 7420.
 * 
 * Hydra_Software_Devel/17   7/15/08 1:54p nissen
 * PR 41229: Added support for configuring tiles and for timing renders.
 * 
 * Hydra_Software_Devel/16   6/16/08 8:38p nissen
 * PR 42507: Added support for rendering with multiple contexts.
 * 
 * Hydra_Software_Devel/15   9/14/06 3:39p nissen
 * PR 22858: Fixed warnings.
 * 
 * Hydra_Software_Devel/14   3/22/06 10:45a nissen
 * PR 18903: Fixed warnings when debug is off.
 * 
 * Hydra_Software_Devel/13   1/12/06 3:26p nissen
 * PR 18903: Fixed vxworks warnings and changed BKNI_Printf calls to
 * BDBG_MSG.
 * 
 * Hydra_Software_Devel/12   12/14/05 3:50p nissen
 * PR 17423: Fixed build problem with math.h include file.
 * 
 * Hydra_Software_Devel/11   11/9/05 11:01p nissen
 * PR 12814: Added reset before rendering.
 * 
 * Hydra_Software_Devel/10   10/5/05 9:52p nissen
 * PR 17273: Added support for cached memory.
 * 
 * Hydra_Software_Devel/9   3/9/05 2:49p nissen
 * PR 12818: Fixed code that waits for rendering to complete.
 * 
 * Hydra_Software_Devel/8   9/15/04 3:14p nissen
 * PR 9132: Added replacement code for waiting on event.
 * 
 * Hydra_Software_Devel/7   9/7/04 3:07p nissen
 * PR 9132: Added extra parameter to interrupt callback.
 * 
 * Hydra_Software_Devel/6   8/10/04 4:21p nissen
 * PR 9132: Changed fog table support.
 * 
 * Hydra_Software_Devel/5   7/27/04 11:08p nissen
 * PR 9132: Added fog table support.
 * 
 * Hydra_Software_Devel/4   4/30/04 1:27p nissen
 * PR 10548: Added use of BCHP_MASK and BCHP_SHIFT macros.
 * 
 * Hydra_Software_Devel/3   3/5/04 2:52p nissen
 * PR 9132: Fixed function that waits for render to complete.
 * 
 * Hydra_Software_Devel/2   2/20/04 4:19p nissen
 * PR 9132: More implementation.
 * 
 * Hydra_Software_Devel/1   1/22/04 3:00p nissen
 * PR 9132: Initial version.
 * 
 ***************************************************************************/

#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bchp.h"
#include "bchp_px3d.h"
#include "bchp_common.h"
#include "bp3d.h"
#include "bp3d_private.h"

#if (BCHP_CHIP==7405) || (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
#include "bchp_memc_0_1.h"
#endif

BDBG_MODULE(BP3D);

#ifdef BP3D_P_ISR_TIMER
#include "btmr.h"
extern BTMR_TimerHandle hTimer;
uint32_t ulISRTime;
#endif

#ifdef BP3D_P_PX3D_TIMER
uint32_t ulPX3DTime;
uint32_t ulRegSetCount;
#endif

/***************************************************************************/
void BP3D_P_RenderContext( 
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
#ifdef BP3D_P_ISR_TIMER
	BTMR_StartTimer_isr( hTimer, 0 );
#endif

	/* load fog table */
	if( hContext->bFogTable && hContext->pucFogTable )
		BP3D_P_LoadFogTable( hP3d, hContext );

	/* write reg sets' offset and count registers to initiate rendering */
#ifdef BP3D_P_PX3D_TIMER
	if( hContext->ulRegSetCount == ulRegSetCount )
	{
		BREG_Write32( hP3d->hRegister, BCHP_PX3D_Reset, 0 );
		BREG_Write32( hP3d->hRegister, BCHP_PX3D_RegTableAddr, hContext->ulRegSetOffset );
	}
	
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_AddedRegSets, 1 );
	ulRegSetCount--;
#else
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_Reset, 0 );
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_RegTableAddr, hContext->ulRegSetOffset );
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_AddedRegSets, hContext->ulRegSetCount );
#endif

	/* store reg set and buffer */
	hContext->ulRenderRegSetOffset = hContext->ulRegSetOffset;
	hContext->ulRenderRegSetCount = hContext->ulRegSetCount;
	hContext->RenderDstColorBuffer = hContext->DstColorBuffer;

	/* store context that is rendering */
	hP3d->hRenderContext = hContext;
}

/***************************************************************************/
void BP3D_P_Render_Isr(
	void *pvParam1,
	int iParam2 )
{
 	BP3D_Handle hP3d = (BP3D_Handle) pvParam1;
	BP3D_Context hRenderContext;
	BSTD_UNUSED( iParam2 );

	if( (hP3d == 0) || (hP3d->hRenderContext == 0) )
		return;

#ifdef BP3D_P_ISR_TIMER
	BTMR_StopTimer_isr( hTimer );
	BTMR_ReadTimer_isr( hTimer, &ulISRTime );
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==7420)
	BKNI_Printf( "%5d ", ulISRTime * 216 / 1000 );
#else
	BKNI_Printf( "%5d ", ulISRTime * 108 / 1000 );
#endif
#endif

#ifdef BP3D_P_PX3D_TIMER
	ulPX3DTime += BREG_Read32( hP3d->hRegister, BCHP_PX3D_TIME_STATUS );
#endif

	hRenderContext = hP3d->hRenderContext;

	/* show tiles (for debugging only) */
	if( hP3d->bShowTiles )
		BP3D_P_DrawTileBoundries( hP3d );

	/* clear irq */
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_IRQMask, 0 );
	BREG_Write32( hP3d->hRegister, BCHP_PX3D_ExceptionAction, 0 );

	/* check if there is a queued context */
	if( hP3d->hQueueContext )
	{
		/* get last context in queue */
		BP3D_Context hPrevContext = NULL;
		BP3D_Context hCurrContext = hP3d->hQueueContext;
		while( hCurrContext->hNextContext )
		{
			hPrevContext = hCurrContext;
			hCurrContext = hCurrContext->hNextContext;
		}

		/* remove context from queue */
		if( hPrevContext )
			hPrevContext->hNextContext = NULL;
		else
			hP3d->hQueueContext = NULL;

		/* render context */
		BP3D_P_RenderContext( hP3d, hCurrContext );
	}

#ifdef BP3D_P_PX3D_TIMER
	if( ulRegSetCount == 0 )
	{
		BKNI_Printf( "%5d ", ulPX3DTime / 1000 );
#endif

	/* increment context's render number */
	hRenderContext->ulRenderNum++;

	/* set interrupt event */
	BKNI_SetEvent( hP3d->hRenderContext->hInterruptEvent );

	/* call user callback function */
	if( hP3d->hRenderContext->pUserCallback )
		(*hP3d->hRenderContext->pUserCallback)( hP3d, hP3d->hRenderContext->pUserParam1, hP3d->hRenderContext->iUserParam2 );

#ifdef BP3D_P_PX3D_TIMER
	}
	if( ulRegSetCount )
		BP3D_P_RenderContext( hP3d, hP3d->hRenderContext );
#endif
}

/***************************************************************************/
void BP3D_P_InititateRender( 
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	uint32_t ii;

	/* enable interrupt on last reg set */
	hContext->pulPrevRegSet[BP3D_P_PX3D_IRQMask] = BCHP_MASK(PX3D_ExceptionStatus, EndOfFrame);
	hContext->pulPrevRegSet[BP3D_P_PX3D_ExceptionAction] = BCHP_MASK(PX3D_ExceptionStatus, EndOfFrame);

#if ((BCHP_CHIP==3548) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==3556) && (BCHP_VER == BCHP_VER_A0))
	hContext->pulPrevRegSet[BP3D_P_PX3D_Mode] |= BCHP_FIELD_DATA(PX3D_Mode, TriggerEnable, 1);
#endif

	/* flush primitive blocks */
	for( ii = 0; ii < hContext->ulPrimBlockCount; ++ii )
		BMEM_FlushCache( hP3d->hMemory, hContext->apvPrimBlocks[ii], BP3D_P_BLOCK_SIZE );

	BKNI_EnterCriticalSection();

#if (BCHP_CHIP==7405) || (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
	/* PX3D Fix: Slow down PX3D if render is using a depth buffer */
	if( hP3d->bUsingDepthBuffer != hP3d->hContext->bUsingDepthBuffer )
	{
		uint32_t ulReg = hP3d->hContext->bUsingDepthBuffer ? 
			(hP3d->ulMemClientInfoReg & BP3D_P_MEMC_0_1_CLIENT_INFO(_PR_TAG_MASK)) | 
			(0xC << BP3D_P_MEMC_0_1_CLIENT_INFO(_BO_VAL_SHIFT)) :
			hP3d->ulMemClientInfoReg;

		BREG_Write32( hP3d->hRegister, BP3D_P_MEMC_0_1_CLIENT_INFO(), ulReg );
		hP3d->bUsingDepthBuffer = hP3d->hContext->bUsingDepthBuffer;
	}
#endif

	/* check if previous render is complete */
	if( hP3d->hRenderContext && (hP3d->hRenderContext->ulRenderNum != hP3d->hRenderContext->ulSceneNum) )
	{
		/* store context in queue */
		hContext->hNextContext = hP3d->hQueueContext;
		hP3d->hQueueContext = hContext;
	}
	else
	{
#ifdef BP3D_P_PX3D_TIMER
		ulRegSetCount = hContext->ulRegSetCount;
		ulPX3DTime = 0;
#endif

		BP3D_P_RenderContext( hP3d, hContext );
	}

	BKNI_LeaveCriticalSection();
}

/***************************************************************************/
void BP3D_P_WaitForRender(
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	if( hContext->ulSceneNum == hContext->ulRenderNum )
	{
		BKNI_ResetEvent( hContext->hInterruptEvent );
	}
	else
	{
		if( BKNI_WaitForEvent( hContext->hInterruptEvent, 20000 ) == BERR_TIMEOUT )
		{
			BP3D_P_DumpRegisters( hP3d );
			BP3D_P_DumpRegSets( hP3d );
		}
	}
}

/***************************************************************************/
void BP3D_P_LoadFogTable( 
	BP3D_Handle hP3d,
	BP3D_Context hContext )
{
	uint32_t ii;

	for( ii = 0; ii < 256; ++ii )
		BREG_Write32( hP3d->hRegister, BCHP_PX3D_FogTable, (ii << 8) | hContext->pucFogTable[ii] );

	hContext->bFogTable = hContext->eFog == BP3D_Fog_Table;
}

/***************************************************************************/
void BP3D_P_DumpRegisters(
	BP3D_Handle hP3d )
{
	void *pAddr;
	uint32_t ulPrimOffset = BREG_Read32( hP3d->hRegister, BCHP_PX3D_DisplayListAddr ) & 0xFFFFFF80;

	BDBG_ERR(( "PX3D RENDER TIMED OUT\n" ));
	BDBG_ERR(( "Mode              = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_Mode ) ));
	BDBG_ERR(( "TopLeft           = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_TopLeft ) ));
	BDBG_ERR(( "BottomRight       = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_BottomRight ) ));
	BDBG_ERR(( "DestConfig        = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_DestConfig ) ));
	BDBG_ERR(( "SourceConfig      = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_SourceConfig ) ));
	BDBG_ERR(( "DisplayListConfig = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_DisplayListConfig ) ));
	BDBG_ERR(( "DisplayListAddr   = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_DisplayListAddr ) ));
	BDBG_ERR(( "DisplayPageAddr   = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_DisplayPageAddr ) ));
	BDBG_ERR(( "DestAddr          = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_DestAddr ) ));
	BDBG_ERR(( "DestZAddr         = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_DestZAddr ) ));
	BDBG_ERR(( "SourceAddr        = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_SourceAddr ) ));
	BDBG_ERR(( "SourceZAddr       = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_SourceZAddr ) ));
	BDBG_ERR(( "TranslateColorKey = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_TranslateColorKey ) ));
	BDBG_ERR(( "ColorKey1         = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_ColorKey1 ) ));
	BDBG_ERR(( "ColorKey3         = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_ColorKey2 ) ));
	BDBG_ERR(( "ColorKey2         = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_ColorKey3 ) ));
	BDBG_ERR(( "FogColor          = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_FogColor ) ));
	BDBG_ERR(( "FogTable          = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_FogTable ) ));
	BDBG_ERR(( "AlphaReference    = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_AlphaReference ) ));
	BDBG_ERR(( "PrimCount         = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_PrimCount ) ));
	BDBG_ERR(( "IRQMask           = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_IRQMask ) ));
	BDBG_ERR(( "ExceptionAction   = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_ExceptionAction ) ));
	BDBG_ERR(( "ExceptionStatus   = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_ExceptionStatus ) ));
	BDBG_ERR(( "Status            = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_Status ) ));
	BDBG_ERR(( "RegTableAddr      = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_RegTableAddr ) ));
	BDBG_ERR(( "AddedRegSets      = %08x", (unsigned int) BREG_Read32( hP3d->hRegister, BCHP_PX3D_AddedRegSets ) ));

	if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, ulPrimOffset, &pAddr ) == BERR_SUCCESS )
	{
		BP3D_P_Primitive *pPrim = (BP3D_P_Primitive *) pAddr;
		BSTD_UNUSED( pPrim );
		BDBG_ERR(( "\nHUNG ON PRIMITIVE:\n" ));
		BDBG_ERR(( "%08x - %08x %08x %08x ", (unsigned int) ulPrimOffset, (unsigned int) pPrim->Header.ulPrimHeaderA, (unsigned int) pPrim->Header.ulPrimHeaderB, (unsigned int) pPrim->Header.ulTextureAddr ));
		BDBG_ERR(( "(%3d %3d %3.1f %3.1f) ", (int) pPrim->v0.fX, (int) pPrim->v0.fY, pPrim->v0.fZ, pPrim->v0.fW ));
		BDBG_ERR(( "(%3d %3d %3.1f %3.1f) ", (int) pPrim->v1.fX, (int) pPrim->v1.fY, pPrim->v1.fZ, pPrim->v1.fW ));
		BDBG_ERR(( "(%3d %3d %3.1f %3.1f) ", (int) pPrim->v2.fX, (int) pPrim->v2.fY, pPrim->v2.fZ, pPrim->v2.fW ));
		BDBG_ERR(( "%3d\n ", (int) BP3D_P_MIN(BP3D_P_MIN(pPrim->v0.fY, pPrim->v1.fY), pPrim->v2.fY) ));
		BDBG_ERR(( "\n" ));
	}

	if( BMEM_ConvertOffsetToAddress( hP3d->hMemory, BREG_Read32( hP3d->hRegister, BCHP_PX3D_RegTableAddr ), &pAddr ) == BERR_SUCCESS )
	{
		uint32_t *pulRegSet = (uint32_t *) pAddr;
		BP3D_P_DumpDisplayList( hP3d, pulRegSet );
	}
}

/* End of File */
