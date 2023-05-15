/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdgp.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/21/07 2:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/dgp/7405/bdgp.c $
 * 
 * Hydra_Software_Devel/1   6/21/07 2:04p nissen
 * PR 29766: Initial version of DGP module.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "bchp_dgp_0.h"
#include "bchp_int_id_bvnb_intr2.h"
#include "bdgp.h"
#include "bdgp_priv.h"

/***************************************************************************/
BERR_Code BDGP_Open(
	BDGP_Handle *phDgp,
	BCHP_Handle hChip,
	BREG_Handle hRegister,
	BINT_Handle hInterrupt )
{
	BDGP_Handle hDgp = 0;
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BDGP_Open);
	BDBG_ASSERT(phDgp);
	BDBG_ASSERT(hChip);
	BDBG_ASSERT(hRegister);
	BDBG_ASSERT(hInterrupt);

	*phDgp = NULL;

	/* allocate memory for private data */
	hDgp = (BDGP_Handle) BKNI_Malloc( sizeof (BDGP_P_Context) );
	if( hDgp == 0 )
		goto fail;
	
	BKNI_Memset( (void *) hDgp, 0, sizeof (BDGP_P_Context) );

	/* create interrupt callback */
	err = BINT_CreateCallback( &hDgp->hTriggerCallback, hInterrupt, 
		BCHP_INT_ID_DGP0_INTR, BDGP_P_Trigger_isr, hDgp, 0 );
	if( err != BERR_SUCCESS )
		goto fail;

	/* set private data */
	hDgp->hChip = hChip;
	hDgp->hRegister = hRegister;
	hDgp->hInterrupt = hInterrupt;

	/* set return handle */
	*phDgp = hDgp;

	BDBG_LEAVE(BDGP_Open);
	return BERR_SUCCESS;

fail:
	if( hDgp )
	{
		if( hDgp->hTriggerCallback )
			BINT_DestroyCallback( hDgp->hTriggerCallback );

		BKNI_Free( (void *) hDgp );
	}

	BDBG_LEAVE(BDGP_Open);
	return BERR_TRACE(err);
}

/***************************************************************************/
BERR_Code BDGP_Close(
	BDGP_Handle hDgp )
{
	BDBG_ENTER(BDGP_Close);
	BDBG_ASSERT( hDgp );

	/* set control register to disable */
	BREG_Write32( hDgp->hRegister, BCHP_DGP_0_CTRL, 
		BCHP_FIELD_ENUM(DGP_0_CTRL, ENABLE_CTRL, STOP_ON_PICTURE_END) );

	BINT_DisableCallback( hDgp->hTriggerCallback );
	BINT_DestroyCallback( hDgp->hTriggerCallback );

	BKNI_Free( (void *) hDgp );

	BDBG_LEAVE(BDGP_Close);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BDGP_ApplyChanges(
	BDGP_Handle hDgp )
{
	BDBG_ENTER(BDGP_ApplyChanges);
	BDBG_ASSERT( hDgp );

	if( hDgp->hSurface )
	{
		/* set control register */
		BREG_Write32( hDgp->hRegister, BCHP_DGP_0_CTRL, 
			BCHP_FIELD_ENUM(DGP_0_CTRL, ENABLE_CTRL, CONTINUOUS) |
			BCHP_FIELD_DATA(DGP_0_CTRL, ERROR_PIXEL_SEL, hDgp->bErrorSelectPixelValue ? 1 : 0) |
			BCHP_FIELD_DATA(DGP_0_CTRL, ALPHA_PRE_MULT, hDgp->bAlphaPremultiply ? 1 : 0) );

		/* set error pixel value register */
		if( hDgp->bErrorSelectPixelValue )
		{
			BREG_Write32( hDgp->hRegister, BCHP_DGP_0_ERROR_PIXEL_VALUE, 
				BCHP_FIELD_DATA(DGP_0_ERROR_PIXEL_VALUE, VALUE, hDgp->uiErrorPixelValue) );
		}

		/* set trigger control register */
		BREG_Write32( hDgp->hRegister, BCHP_DGP_0_TRIG_CTRL, 
			BCHP_FIELD_DATA(DGP_0_TRIG_CTRL, TRIG_SEL, hDgp->eCallbackTrigger) );

		/* set line compare config register */
		if( hDgp->eCallbackTrigger != BDGP_Trigger_Disable )
		{
			BREG_Write32( hDgp->hRegister, BCHP_DGP_0_LINE_CMP_TRIG_CFG, 
				BCHP_FIELD_DATA(DGP_0_LINE_CMP_TRIG_CFG, MASK, hDgp->uiLineCompareMask) |
				BCHP_FIELD_DATA(DGP_0_LINE_CMP_TRIG_CFG, POS, hDgp->uiLineComparePosition) );
		}

		/* set format register */
		switch( hDgp->uiBpp )
		{
		case 32:
			BREG_Write32( hDgp->hRegister, BCHP_DGP_0_FORMAT_DEF_1, 
				BCHP_FIELD_ENUM(DGP_0_FORMAT_DEF_1, FORMAT_TYPE, ARGB8888) );
			break;
		case 16:
			BREG_Write32( hDgp->hRegister, BCHP_DGP_0_FORMAT_DEF_1, 
				BCHP_FIELD_ENUM(DGP_0_FORMAT_DEF_1, FORMAT_TYPE, AP88) );
			break;
		case 8:
			BREG_Write32( hDgp->hRegister, BCHP_DGP_0_FORMAT_DEF_1, 
				BCHP_FIELD_ENUM(DGP_0_FORMAT_DEF_1, FORMAT_TYPE, P8) );
			break;
		}

		/* set format pixel channel position register */
		BREG_Write32( hDgp->hRegister, BCHP_DGP_0_FORMAT_DEF_2, 
			BCHP_FIELD_DATA(DGP_0_FORMAT_DEF_2, CH0_LSB_POS, BPXL_COMPONENT_POS(hDgp->eFormat, 0)) |
			BCHP_FIELD_DATA(DGP_0_FORMAT_DEF_2, CH1_LSB_POS, BPXL_COMPONENT_POS(hDgp->eFormat, 1)) |
			BCHP_FIELD_DATA(DGP_0_FORMAT_DEF_2, CH2_LSB_POS, BPXL_COMPONENT_POS(hDgp->eFormat, 2)) |
			BCHP_FIELD_DATA(DGP_0_FORMAT_DEF_2, CH3_LSB_POS, BPXL_COMPONENT_POS(hDgp->eFormat, 3)) );

		/* set surface dimensions register */
		BREG_Write32( hDgp->hRegister, BCHP_DGP_0_SRC_PIC_SIZE, 
			BCHP_FIELD_DATA(DGP_0_SRC_PIC_SIZE, HSIZE, hDgp->uiWidth) |
			BCHP_FIELD_DATA(DGP_0_SRC_PIC_SIZE, VSIZE, hDgp->uiHeight) );

		/* set surface adress and pitch registers */
		BREG_Write32( hDgp->hRegister, BCHP_DGP_0_SRC_START, 
			BCHP_FIELD_DATA(DGP_0_SRC_START, ADDR, hDgp->uiOffset) );
		BREG_Write32( hDgp->hRegister, BCHP_DGP_0_SRC_PITCH, 
			BCHP_FIELD_DATA(DGP_0_SRC_PITCH, PITCH, hDgp->uiPitch) );

		/* set device enable register */
		if( !hDgp->bDeviceEnabled )
		{
			BREG_Write32( hDgp->hRegister, BCHP_DGP_0_ENABLE, 
				BCHP_FIELD_ENUM(DGP_0_ENABLE, DGP_ENABLE, ENABLE ) );
			hDgp->bDeviceEnabled = true;
		}
	}
	else
	{
		/* set control register to disable */
		if( hDgp->bDeviceEnabled )
		{
			BREG_Write32( hDgp->hRegister, BCHP_DGP_0_CTRL, 
				BCHP_FIELD_ENUM(DGP_0_CTRL, ENABLE_CTRL, STOP_ON_PICTURE_END) );
			hDgp->bDeviceEnabled = false;
		}
	}

	BDBG_LEAVE(BDGP_ApplyChanges);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BDGP_SetSurface(
	BDGP_Handle hDgp, 
	BSUR_Surface_Handle hSurface )
{
	BPXL_Format eFormat = 0;
	void *pvMemory = 0;
	uint32_t uiPitch = 0;
	uint32_t uiWidth = 0;
	uint32_t uiHeight = 0;
	uint32_t uiOffset = 0;
	uint32_t uiBpp = 0;

	BDBG_ENTER(BDGP_SetSurface);
	BDBG_ASSERT( hDgp );

	if( hSurface )
	{
		/* get surface information */
		if( (BSUR_Surface_GetFormat( hSurface, &eFormat ) != BERR_SUCCESS) ||
			(BSUR_Surface_GetAddress( hSurface, &pvMemory, &uiPitch) != BERR_SUCCESS) ||
			(BSUR_Surface_GetDimensions( hSurface, &uiWidth, &uiHeight) != BERR_SUCCESS) ||
			(BSUR_Surface_GetOffset( hSurface, &uiOffset) != BERR_SUCCESS) )
		{
			BDBG_LEAVE(BDGP_SetSurface);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
		
		uiBpp = BPXL_BITS_PER_PIXEL(eFormat);

		/* verify surface format is supported (ARGB8888, P8 or AP88) */
		if( !((BPXL_IS_RGB_FORMAT(eFormat) && (uiBpp == 32)) ||
			(BPXL_IS_PALETTE_FORMAT(eFormat) && (uiBpp == 8)) ||
			(BPXL_IS_PALETTE_FORMAT(eFormat) && BPXL_HAS_ALPHA(eFormat) && (uiBpp == 16))) )
		{
			BDBG_LEAVE(BDGP_SetSurface);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		/* verify width is aligned to 32 bits */
		if( uiWidth & (32 / uiBpp - 1) )
		{
			BDBG_LEAVE(BDGP_SetSurface);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		/* verify address and pitch are aligned to a pixel */
		if( (uiOffset & (uiBpp / 8 - 1)) || 
			(uiPitch & (uiBpp / 8 - 1)) )
		{
			BDBG_LEAVE(BDGP_SetSurface);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}

	/* store surface information */
	hDgp->hSurface = hSurface;
	hDgp->eFormat = eFormat;
	hDgp->pvMemory = pvMemory;
	hDgp->uiPitch = uiPitch;
	hDgp->uiWidth = uiWidth;
	hDgp->uiHeight = uiHeight;
	hDgp->uiOffset = uiOffset;
	hDgp->uiBpp = uiBpp;

	BDBG_LEAVE(BDGP_SetSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BDGP_SetAlphaPremultiply(
	BDGP_Handle hDgp,
	bool bPremultiply )
{
	BDBG_ENTER(BDGP_SetAlphaPremultiply);
	BDBG_ASSERT( hDgp );

	hDgp->bAlphaPremultiply = bPremultiply;

	BDBG_LEAVE(BDGP_SetAlphaPremultiply);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BDGP_SetPixelErrorSelect(
	BDGP_Handle hDgp,
	bool bSelectPixelValue,
	uint32_t uiPixelValue )
{
	BDBG_ENTER(BDGP_SetAlphaPremultiply);
	BDBG_ASSERT( hDgp );

	hDgp->bErrorSelectPixelValue = bSelectPixelValue;
	hDgp->uiErrorPixelValue = uiPixelValue;

	BDBG_LEAVE(BDGP_SetAlphaPremultiply);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BDGP_InstallCallback(
	BDGP_Handle hDgp,
	BDGP_Trigger eTrigger,
	const BDGP_CallbackFunc_isr pfCallback,
	void *pvParm1,
	int iParm2 )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BDGP_InstallCallback);
	BDBG_ASSERT( hDgp );

	/* enable callback */
	if( pfCallback )
	{
		err = BINT_ClearCallback( hDgp->hTriggerCallback );
		if( err != BERR_SUCCESS )
		{
			BDBG_LEAVE(BDGP_InstallCallback);
			return BERR_TRACE(err);
		}

		err = BINT_EnableCallback( hDgp->hTriggerCallback );
		if( err != BERR_SUCCESS )
		{
			BDBG_LEAVE(BDGP_InstallCallback);
			return BERR_TRACE(err);
		}
	}
	/* disable callback */
	else
	{
		err = BINT_DisableCallback( hDgp->hTriggerCallback );
		if( err != BERR_SUCCESS )
		{
			BDBG_LEAVE(BDGP_InstallCallback);
			return BERR_TRACE(err);
		}
	}

	hDgp->eCallbackTrigger = eTrigger;
	hDgp->pfCallback = pfCallback;
	hDgp->pvCallbackParm1 = pvParm1;
	hDgp->iCallbackParm2 = iParm2;

	BDBG_LEAVE(BDGP_InstallCallback);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BDGP_SetLineCompare(
	BDGP_Handle hDgp,
	uint32_t uiMask,
	uint32_t uiPosition )
{
	BDBG_ENTER(BDGP_SetTriggerLineCompare);
	BDBG_ASSERT( hDgp );

	hDgp->uiLineCompareMask = uiMask;
	hDgp->uiLineComparePosition = uiPosition;

	BDBG_LEAVE(BDGP_SetTriggerLineCompare);
	return BERR_SUCCESS;
}

/* End of File */
