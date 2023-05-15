/***************************************************************************
 *	   Copyright (c) 2003-2007, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_sims.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/16/07 2:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_sims.c $
 * 
 * Hydra_Software_Devel/2   10/16/07 2:24p jessem
 * PR 36129: Change BCHIPIF_xxx to IKOS_Client_xxx.
 * 
 * Hydra_Software_Devel/1   3/31/07 6:25p shyam
 * PR 29285 : Added linuxclient support for simulations/emulation
 * 
 *
 ***************************************************************************/
#include "bstd.h"

#include "bkni.h"

#include "breg_mem.h"
#include "client.h"

#if BDBG_DEBUG_BUILD
typedef struct BREG_Impl
{
	uintptr_t BaseAddr; /* BaseAddr shall be the first member to keep it run-time compatible with the release builds */
	size_t MaxRegOffset;
}BREG_Impl;
#endif

void BREG_Open( BREG_Handle *pRegHandle, void *Address, size_t MaxRegOffset )
{
	*pRegHandle = (BREG_Handle)BKNI_Malloc( sizeof(BREG_Impl) );
	BDBG_ASSERT(*pRegHandle != NULL );

#if BDBG_DEBUG_BUILD
	(*pRegHandle)->MaxRegOffset = MaxRegOffset;
#else
	BSTD_UNUSED(MaxRegOffset);
#endif
	(*pRegHandle)->BaseAddr = (uintptr_t)Address;
}

void BREG_Close( BREG_Handle RegHandle )
{
	BDBG_ASSERT(RegHandle != NULL );
	BKNI_Free(RegHandle);
}

/* compile the register access functions even for the release build */
#undef	BREG_Write32
#undef	BREG_Write16
#undef	BREG_Write8

#undef	BREG_Read32
#undef	BREG_Read16
#undef	BREG_Read8


uint32_t BREG_Read32(BREG_Handle RegHandle, uint32_t reg)
{
	uint32_t data ;

	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	IKOS_Client_ReadRegister(reg, &data);
	return data ;
}

uint16_t BREG_Read16(BREG_Handle RegHandle, uint32_t reg)
{
	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BDBG_ASSERT(0);
	return *((volatile uint16_t *)(RegHandle->BaseAddr+reg));
}

uint8_t BREG_Read8(BREG_Handle RegHandle, uint32_t reg)
{
	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BDBG_ASSERT(0);
	return *((volatile uint8_t *)(RegHandle->BaseAddr+reg));
}

void BREG_Write32(BREG_Handle RegHandle, uint32_t reg, uint32_t data)
{
	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	IKOS_Client_WriteRegister(reg, data) ;
}

void BREG_Write16(BREG_Handle RegHandle, uint32_t reg, uint16_t data)
{
	BSTD_UNUSED(data);
	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BDBG_ASSERT(0);
}

void BREG_Write8(BREG_Handle RegHandle, uint32_t reg, uint8_t data)
{
	BSTD_UNUSED(data);
	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BDBG_ASSERT(0);
}


/* End of File */

