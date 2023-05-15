/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 2/6/13 6:41p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/bdsp.c $
 * 
 * Hydra_Software_Devel/6   2/6/13 6:41p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/1   12/4/12 5:06p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/5   2/22/12 7:21p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/4   7/4/11 6:35p gprasad
 * SW7422-373: Add GetInfo routines for AudioType, AudioProcessing, and
 * AudioEncode in BDSP
 * 
 * Hydra_Software_Devel/3   6/17/11 3:49p gautamk
 * SW7422-373: [7422] Adding GetInfo routines for AudioType,
 * AudioProcessing, and AudioEncode support in BDSP
 * 
 * Hydra_Software_Devel/2   6/15/11 5:47p gautamk
 * SW7425-683: [7425] Adding support for Power management  in BDSP
 * 
 * Hydra_Software_Devel/1   4/6/11 2:15p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/3   1/13/11 5:55a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/2   12/15/10 6:47p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 * Hydra_Software_Devel/1   12/15/10 2:00p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/

#include "bdsp.h"
#include "bdsp_priv.h"

BDBG_MODULE(bdsp);

void BDSP_Close(
    BDSP_Handle handle      /* Handle to be closed */
    )
{
    BDBG_OBJECT_ASSERT(handle, BDSP_Device);
    BDBG_ASSERT(NULL != handle->close);
    handle->close(handle->pDeviceHandle);
}

void BDSP_GetStatus(
    BDSP_Handle handle,
    BDSP_Status *pStatus             /* [out] Current Status */
    )
{

    BDBG_OBJECT_ASSERT(handle, BDSP_Device);
    
    if ( handle->getStatus ) 
    {
        handle->getStatus(handle->pDeviceHandle, pStatus);
    }
    else
    {
        BKNI_Memset(pStatus, 0, sizeof(BDSP_Status));
    }
    return;
}

BERR_Code BDSP_GetDebugLog(
    BDSP_Handle handle,
    char *pBuffer,
    size_t bufferLength
    )
{
    BDBG_OBJECT_ASSERT(handle, BDSP_Device);
    if ( handle->getDebugLog ) 
    {
        return handle->getDebugLog(handle->pDeviceHandle, pBuffer, bufferLength);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_Standby(
	BDSP_Handle 			handle, 		/* [in] DSP device handle */
	BDSP_StandbySettings 	*pSettings 	/* [in] standby settings */
    )
{

    BDBG_OBJECT_ASSERT(handle, BDSP_Device);
    
    if ( handle->powerStandby) 
    {
        handle->powerStandby(handle->pDeviceHandle, pSettings);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    return BERR_SUCCESS;
}

BERR_Code BDSP_Resume(
	BDSP_Handle 			handle 		/* [in] DSP device handle */
    )
{

    BDBG_OBJECT_ASSERT(handle, BDSP_Device);
    
    if ( handle->powerResume) 
    {
        handle->powerResume(handle->pDeviceHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    return BERR_SUCCESS;
}

BERR_Code BDSP_GetAudioTypeInfo(
BDSP_Handle handle,
BDSP_AudioType type, /* [in] */
const BDSP_AudioTypeInfo **pInfo /* [out] */
)
{

    BDBG_OBJECT_ASSERT(handle, BDSP_Device);
    
    if ( handle->getAudioTypeInfo) 
    {
        handle->getAudioTypeInfo(type, pInfo);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    return BERR_SUCCESS;
}

BERR_Code BDSP_GetAudioProcessingInfo(
BDSP_Handle handle,
BDSP_AudioProcessing type, /* [in] */
const BDSP_AudioProcessingInfo **pInfo /* [out] */
)
{

    BDBG_OBJECT_ASSERT(handle, BDSP_Device);
    
    if ( handle->getAudioProcessingInfo) 
    {
        handle->getAudioProcessingInfo(type, pInfo);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    return BERR_SUCCESS;
}


BERR_Code BDSP_GetAudioEncodeInfo(
BDSP_Handle handle,
BDSP_AudioEncode type, /* [in] */
const BDSP_AudioEncodeInfo **pInfo /* [out] */
)
{

    BDBG_OBJECT_ASSERT(handle, BDSP_Device);
    
    if ( handle->getAudioEncodeInfo) 
    {
        handle->getAudioEncodeInfo(type, pInfo);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    return BERR_SUCCESS;
}


/***************************************************************************
Summary:
	Allocate an external interrupt.

Description:
	This function allocates one interrupt handle that can be used to interrupt DSP.

Returns:
	BERR_SUCCESS - If allocation is successful, otherwise error

See Also:
	BDSP_FreeExternalInterrupt
	BDSP_GetExternalInterruptInfo
***************************************************************************/
BERR_Code BDSP_AllocateExternalInterrupt(
    BDSP_Handle hDsp,
    uint32_t	dspIndex,
    BDSP_ExternalInterruptHandle *pInterruptHandle /* [out] */
    )
{
	BERR_Code	ErrCode = BERR_SUCCESS;
	BDBG_OBJECT_ASSERT(hDsp, BDSP_Device);
	
    if ( hDsp->allocateExternalInterrupt) 
    {
        ErrCode = hDsp->allocateExternalInterrupt(hDsp->pDeviceHandle, dspIndex, pInterruptHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
	return ErrCode;
}


/***************************************************************************
Summary:
	Frees an allocated external interrupt.

Description:
	This function frees one interrupt handle that was already allocated.

Returns:
	BERR_SUCCESS if sucessful else error

See Also:
	BDSP_AllocateExternalInterrupt
	BDSP_GetExternalInterruptInfo
***************************************************************************/
BERR_Code BDSP_FreeExternalInterrupt(
			BDSP_ExternalInterruptHandle	hInterrupt
			)	
{
	BDSP_Handle handle;
	BERR_Code	ErrCode = BERR_SUCCESS;
	
	BDBG_OBJECT_ASSERT(hInterrupt, BDSP_ExternalInterrupt);

	handle = hInterrupt->hDsp;
	
	if(handle->freeExternalInterrupt)
	{
		ErrCode = handle->freeExternalInterrupt(hInterrupt->pExtInterruptHandle);
	}
	else
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	return ErrCode;
}

/***************************************************************************
Summary:
	Retrieve external interrupt information.

Description:
	This function provides the interrupt register and the bit that can be used to interrupt DSP.

Returns:
	BERR_SUCCESS - If successful, otherwise error

See Also:
	BDSP_FreeExternalInterrupt
	BDSP_AllocateExternalInterrupt
***************************************************************************/
BERR_Code BDSP_GetExternalInterruptInfo(
    BDSP_ExternalInterruptHandle hInterrupt,
    BDSP_ExternalInterruptInfo **pInfo /* [out] */
    )
{
	BDSP_Handle handle;
	BERR_Code	ErrCode = BERR_SUCCESS;

	BDBG_OBJECT_ASSERT(hInterrupt, BDSP_ExternalInterrupt);

	handle = hInterrupt->hDsp;
	
	if(handle->getExternalInterruptInfo)
	{
		ErrCode = handle->getExternalInterruptInfo(hInterrupt->pExtInterruptHandle, pInfo);
	}
	else
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	return ErrCode;

}

/***************************************************************************
Summary:
	Allocate RDB registers from dsp page for upper SW layer

Description:
	This function allocates RDB registers handle that can be used to send msg etc to DSP.

Returns:
	BERR_SUCCESS - If allocation is successful, otherwise error

See Also:
	BDSP_FreeRdbRegisters
	BDSP_GetRdbRegistersInfo
***************************************************************************/
BERR_Code BDSP_AllocateRdbRegisters(
    BDSP_Handle hDsp,
    uint32_t	dspIndex,
    uint32_t	numRdbToAllocate,
    BDSP_RdbRegisterHandle *pRdbRegisterHandle /* [out] */
    )
{
	BERR_Code	ErrCode = BERR_SUCCESS;
	BDBG_OBJECT_ASSERT(hDsp, BDSP_Device);
	
    if ( hDsp->allocateRdbRegister) 
    {
        ErrCode = hDsp->allocateRdbRegister(hDsp->pDeviceHandle, dspIndex, numRdbToAllocate, pRdbRegisterHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
	return ErrCode;
}


/***************************************************************************
Summary:
	Frees an allocated Rdb Registers handle.

Description:
	This function frees an Rdb Registers handle that was already allocated.

Returns:
	BERR_SUCCESS if sucessful else error

See Also:
	BDSP_AllocateRdbRegisters
	BDSP_GetRdbRegistersInfo
***************************************************************************/
BERR_Code BDSP_FreeRdbRegisters(
			BDSP_RdbRegisterHandle	hRdbRegister
			)	
{
	BDSP_Handle handle;
	BERR_Code	ErrCode = BERR_SUCCESS;
	
	BDBG_OBJECT_ASSERT(hRdbRegister, BDSP_RdbRegister);

	handle = hRdbRegister->hDsp;
	
	if(handle->freeRdbRegister)
	{
		ErrCode = handle->freeRdbRegister(hRdbRegister->pRdbRegisterHandle);
	}
	else
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	return ErrCode;
}

/***************************************************************************
Summary:
	Retrieve Rdb Registers information.

Description:
	This function provides the address (offset) of base RDB register.

Returns:
	BERR_SUCCESS - If successful, otherwise error

See Also:
	BDSP_FreeRdbRegisters
	BDSP_AllocateRdbRegisters
***************************************************************************/
BERR_Code BDSP_GetRdbRegistersInfo(
    BDSP_RdbRegisterHandle hRdbRegister,
    BDSP_RdbRegisterInfo **pInfo /* [out] */
    )
{
	BDSP_Handle handle;
	BERR_Code	ErrCode = BERR_SUCCESS;

	BDBG_OBJECT_ASSERT(hRdbRegister, BDSP_RdbRegister);

	handle = hRdbRegister->hDsp;
	
	if(handle->getRdbRegisterInfo)
	{
		ErrCode = handle->getRdbRegisterInfo(hRdbRegister->pRdbRegisterHandle, pInfo);
	}
	else
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	return ErrCode;

}


/***************************************************************************
Summary:
    Process audio capture data

Description:
    This function must be called in a background thread at a frequent 
    interval (e.g. 10ms). for the data to be copied into the capture buffers.

Returns:
    BERR_SUCCESS - If audio capture is successful

See Also:
    BDSP_AudioCapture_GetBuffer
    BDSP_AudioCapture_ConsumeData
***************************************************************************/
BERR_Code BDSP_ProcessAudioCapture(
    BDSP_Handle hDsp
    )
{
	BERR_Code	ErrCode = BERR_SUCCESS;

	BDBG_OBJECT_ASSERT(hDsp, BDSP_Device);

	if(hDsp->processAudioCapture)
	{
		ErrCode = hDsp->processAudioCapture(hDsp->pDeviceHandle);
	}
	else
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	return ErrCode;
}
