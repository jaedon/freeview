/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 2/6/13 6:58p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga.c $
 * 
 * Hydra_Software_Devel/20   2/6/13 6:58p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/5   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/19   11/26/12 1:20p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/4   1/8/13 4:58p gprasad
 * SW7425-4216: Merging mainline changes to rbuf capture branch as of
 * 01/08/2012
 * 
 * Hydra_Software_Devel/SW7425-4216/3   12/4/12 5:07p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/SW7425-4216/2   11/28/12 4:26p gprasad
 * SW7425-4216: Added capture create and destroy apis for configuring
 * capture
 * 
 * Hydra_Software_Devel/SW7425-4216/1   11/12/12 10:40p gprasad
 * SW7425-4216: Initial changes for ring buffer capture - verified to be
 * working for single decode with raaga test
 * 
 * Hydra_Software_Devel/18   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/17   2/22/12 7:25p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/16   12/5/11 5:50p rshijith
 * FWRAA-365 :Disabling target print by default.
 * 
 * Hydra_Software_Devel/15   12/5/11 5:41p rshijith
 * FWRAA-365 : Adding Target print support to raaga subsystem.
 * 
 * Hydra_Software_Devel/14   11/10/11 11:06a gprasad
 * SW7422-400: Please add hooks to support dumping the UART debug buffer
 * into DRAM
 * 
 * Hydra_Software_Devel/13   10/20/11 5:43p gprasad
 * FWRAA-359: Add support for ring buffer capture for cert testing
 * 
 * Hydra_Software_Devel/12   8/25/11 4:05p gprasad
 * SW7422-400: Updating the APIs with review comments
 * 
 * Hydra_Software_Devel/11   8/18/11 12:53p gprasad
 * SW7422-400: Updating the APIs with review comments
 * 
 * Hydra_Software_Devel/10   7/4/11 6:34p gprasad
 * SW7422-400: Please add hooks to support dumping the UART debug buffer
 * into DRAM
 * 
 * Hydra_Software_Devel/9   6/17/11 3:41p gautamk
 * SW7422-373: [7422] Adding GetInfo routines for AudioType,
 * AudioProcessing, and AudioEncode support in BDSP
 * 
 * Hydra_Software_Devel/8   6/15/11 5:53p gautamk
 * SW7425-683: [7425] Adding support for Power management in BDSP
 * 
 * Hydra_Software_Devel/7   5/19/11 10:19a srajapur
 * SW7231-145 : [7231] Adding BCHP_PWR support to BDSP
 * 
 * Hydra_Software_Devel/6   5/11/11 5:30p srajapur
 * SW7422-374 : [7425] Incorporate changes to BDSP "details" structures
 * from code review feedback.
 * 
 * Hydra_Software_Devel/5   4/29/11 11:45a srajapur
 * SW7422-368 : [7422] BDSP_AudioTask_GetDefaultXxxSettings is Raaga-
 * specific ---------> Moved raaga specific API from common files to
 * raaga files and renamed the API's accordingly.
 * 
 * Hydra_Software_Devel/4   4/27/11 5:08p srajapur
 * SW7422-419 : [7425] Added BDSP_Raaga_GetDownloadStatus and
 * BDSP_Raaga_Initialize API to support authentication
 * 
 * Hydra_Software_Devel/3   4/14/11 6:52p srajapur
 * SW7425-291: [7425] BDSP directory structure changes
 * 
 * Hydra_Software_Devel/2   4/12/11 2:24p srajapur
 * SW7422-419:[7425]Adding BDSP_Raaga_GetFwDwnldInfo and
 * BDSP_Raaga_StartDsp API to support authentication
 * 
 * Hydra_Software_Devel/1   4/6/11 2:27p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/9   3/24/11 5:25p jgarrett
 * SW7422-146: Fixes for kernel mode and big endian
 * 
 * Hydra_Software_Devel/8   1/26/11 3:17a gautamk
 * SW7422-191: Allocating FW debug buffer on basis of runtime Parrameter
 * enableDebugLog of BDSP_RaagaSettings.
 * 
 * Hydra_Software_Devel/7   1/25/11 2:22a gautamk
 * SW7422-191:Adding error recovery code for Raaga Open
 * 
 * Hydra_Software_Devel/6   1/20/11 8:09a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/5   1/19/11 7:13a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/4   1/18/11 4:22a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/3   1/13/11 5:26a gautamk
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
 * Hydra_Software_Devel/1   12/15/10 2:01p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/

#include "bdsp.h"
#include "../src/bdsp_priv.h" /* FIXME: Remove the _priv dependency */
#include "bdsp_raaga.h"
#include "../src/raaga/bdsp_raaga_priv.h" /* FIXME: Remove the _priv dependency */

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

BDBG_MODULE(bdsp_raaga);

#ifdef BDSP_FW_RBUF_CAPTURE
/* Settings for Firmware Ring Buffer Capture.Initialized in BRAP_RB_CBSetup() */
BDSP_P_RbufCapture Rbuf_Setting = { NULL, NULL, NULL, NULL }; 
#endif

void BDSP_Raaga_GetDefaultSettings(
    BDSP_RaagaSettings *pSettings     /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
#if BDSP_IMG_SUPPORT
    pSettings->pImageContext = BDSP_IMG_Context;
    pSettings->pImageInterface = &BDSP_IMG_Interface;
#endif
    pSettings->preloadImages = false;
    pSettings->authenticationEnabled =false;

	/* All debug features will be disabled by default */
	pSettings->debugSettings[BDSP_Raaga_DebugType_eUart].enabled = false;
	pSettings->debugSettings[BDSP_Raaga_DebugType_eDramMsg].enabled = false;
	pSettings->debugSettings[BDSP_Raaga_DebugType_eCoreDump].enabled = false;
    pSettings->debugSettings[BDSP_Raaga_DebugType_eTargetPrintf].enabled = false;

	pSettings->debugSettings[BDSP_Raaga_DebugType_eUart].bufferSize = 0x1000; /* 4 KB by default */
	pSettings->debugSettings[BDSP_Raaga_DebugType_eDramMsg	].bufferSize = 0x40000; /* 256 KB by default */
	pSettings->debugSettings[BDSP_Raaga_DebugType_eCoreDump].bufferSize = 0x80000; /* 512 KB by default */
	pSettings->debugSettings[BDSP_Raaga_DebugType_eTargetPrintf].bufferSize = 0x40000; /* 256 KB by default */

    /*pSettings->enableDebugLog = false;*/
}

BERR_Code BDSP_Raaga_Open(
    BDSP_Handle *pDsp,                      /* [out] */
    BCHP_Handle chpHandle,
    BREG_Handle regHandle,
    BMEM_Handle memHandle,
    BINT_Handle intHandle,
    BTMR_Handle tmrHandle,
    const BDSP_RaagaSettings *pSettings
    )
{
    BDSP_Raaga *pRaaga;
    BERR_Code ret= BERR_SUCCESS;
    
    BDBG_ASSERT(NULL != pDsp);
    BDBG_ASSERT(NULL != chpHandle);
    BDBG_ASSERT(NULL != regHandle);
    BDBG_ASSERT(NULL != memHandle);
    BDBG_ASSERT(NULL != intHandle);
    /* tmr is not required */
    BSTD_UNUSED(tmrHandle);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ENTER(BDSP_Raaga_Open);
    /* Alloc raaga device */
    pRaaga = BKNI_Malloc(sizeof(BDSP_Raaga));
    if ( NULL == pRaaga ) 
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(pRaaga, 0, sizeof(BDSP_Raaga));

    /* Init device */
    BDSP_P_InitDevice(&pRaaga->device, pRaaga);
    pRaaga->device.close = BDSP_Raaga_P_Close;
    pRaaga->device.getDefaultContextSettings = BDSP_Raaga_P_GetDefaultContextSettings;
    pRaaga->device.createContext = BDSP_Raaga_P_CreateContext;    
    pRaaga->device.getStatus= BDSP_Raaga_P_GetStatus;    
    pRaaga->device.powerStandby= BDSP_Raaga_P_PowerStandby;    
    pRaaga->device.powerResume= BDSP_Raaga_P_PowerResume;    
    pRaaga->device.getAudioTypeInfo= BDSP_Raaga_P_GetAudioTypeInfo;    
    pRaaga->device.getAudioProcessingInfo= BDSP_Raaga_P_GetAudioProcessingInfo;    
    pRaaga->device.getAudioEncodeInfo= BDSP_Raaga_P_GetAudioEncodeInfo;    
	pRaaga->device.allocateExternalInterrupt = BDSP_Raaga_P_AllocateExternalInterrupt;
	pRaaga->device.freeExternalInterrupt = BDSP_Raaga_P_FreeExternalInterrupt;
	pRaaga->device.getExternalInterruptInfo = BDSP_Raaga_P_GetExternalInterruptInfo;
	pRaaga->device.allocateRdbRegister = BDSP_Raaga_P_AllocateRdbRegisters;
	pRaaga->device.freeRdbRegister = BDSP_Raaga_P_FreeRdbRegisters;
	pRaaga->device.getRdbRegisterInfo = BDSP_Raaga_P_GetRdbRegistersInfo;
	pRaaga->device.processAudioCapture = BDSP_Raaga_P_ProcessAudioCapture;
     
    /* Init context lists */
    BLST_S_INIT(&pRaaga->contextList);

	/* Init interrupt list */
	BLST_S_INIT(&pRaaga->interruptList);

	/* Init Rdb register list */
	BLST_S_INIT(&pRaaga->rdbRegisterList);

    /* Save Settings and params */
    pRaaga->settings = *pSettings;
    pRaaga->chpHandle = chpHandle;
    pRaaga->regHandle = regHandle;
    pRaaga->memHandle = memHandle;
    pRaaga->intHandle = intHandle;

#ifdef BDSP_FW_RBUF_CAPTURE
    /* Specific to FW Ring Buffer capture required for their unit testing */
	if(Rbuf_Setting.rbuf_init != NULL && Rbuf_Setting.rbuf_uninit != NULL) 
	{
 		Rbuf_Setting.rbuf_init((BREG_Handle)regHandle, (BMEM_Handle)memHandle);
	}
#endif

    /* Done, mark object as valid */
    BDBG_OBJECT_SET(pRaaga, BDSP_Raaga);
    /* power up */
#ifdef BCHP_PWR_RESOURCE_RAAGA_SRAM
            BCHP_PWR_AcquireResource(chpHandle, BCHP_PWR_RESOURCE_RAAGA_SRAM);
#endif
#ifdef BCHP_PWR_RESOURCE_RAAGA
                BCHP_PWR_AcquireResource(chpHandle, BCHP_PWR_RESOURCE_RAAGA);
#endif


    ret = BDSP_Raaga_P_Open(pRaaga->device.pDeviceHandle);
    if(ret != BERR_SUCCESS)
    {        
        ret = BERR_TRACE(ret);        
        goto err_open;
    }
        
    *pDsp = &pRaaga->device;
    goto open_success;
    
err_open:    
    BDBG_OBJECT_DESTROY(pRaaga, BDSP_Raaga);
    BKNI_Free(pRaaga);   
#ifdef BCHP_PWR_RESOURCE_RAAGA_SRAM
    BCHP_PWR_ReleaseResource(chpHandle, BCHP_PWR_RESOURCE_RAAGA_SRAM);
#endif    
#ifdef BCHP_PWR_RESOURCE_RAAGA
    BCHP_PWR_ReleaseResource(chpHandle, BCHP_PWR_RESOURCE_RAAGA);
#endif
    
open_success:    
    return ret;
}

#ifdef BDSP_FW_RBUF_CAPTURE
void BDSP_P_RbufSetup(
	  BDSP_P_RbufCapture sRbufCap
      )
{
	Rbuf_Setting.rbuf_init = sRbufCap.rbuf_init;
	Rbuf_Setting.rbuf_uninit = sRbufCap.rbuf_uninit;
	Rbuf_Setting.rbuf_capture_channel_start = sRbufCap.rbuf_capture_channel_start;
	Rbuf_Setting.rbuf_capture_stop_channel = sRbufCap.rbuf_capture_stop_channel;
}
#endif

/******************************************************************************
Summary:
	This Function returns true, If the Processing algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_IsAudioProcessingSupported(
                                    BDSP_AudioProcessing audioProcessing)
{
	return	BDSP_Raaga_P_IsAudioProcessingSupported(audioProcessing);
}

/******************************************************************************
Summary:
	This Function returns true, If the audio processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_IsAudioTypeSupported(
					BDSP_AudioType audioType)
{
    return	BDSP_Raaga_P_IsAudioTypeSupported(audioType);
}
/******************************************************************************
Summary:
	This Function returns true, If the Encode algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_IsAudioEncodeSupported(
                                    BDSP_AudioEncode audioEncode)
{
    return	BDSP_Raaga_P_IsAudioEncodeSupported(audioEncode);
}

BERR_Code BDSP_Raaga_GetDownloadStatus(
    BDSP_Handle handle,
    BDSP_Raaga_DownloadStatus *pStatus /* [out] */
    )	
{

    BDSP_Raaga *pDevice = (BDSP_Raaga *)handle->pDeviceHandle;
    
	BDBG_ENTER(BDSP_Raaga_GetDownloadStatus);
    /* Assert the function arguments*/
	BDBG_ASSERT(handle->pDeviceHandle);

    /*If Firmware Firmware authentication is Disabled*/	
	if(pDevice->settings.authenticationEnabled==false)     
	{
	    BDBG_ERR(("BDSP_Raaga_GetDownloadStatus should be called only if bFwAuthEnable is true"));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

    /*Logical address*/
    pStatus->pBaseAddress = (void *)(pDevice->memInfo.sOpenTimeMemInfo.ui32BaseAddr);
    /*Physical Address */    
	BMEM_ConvertAddressToOffset(pDevice->memHandle,(void *)(pStatus->pBaseAddress),&(pStatus->physicalAddress));
    /*Size of the executable download */
	pStatus->length =pDevice->memInfo.sOpenTimeMemInfo.ui32Size; 


	BDBG_LEAVE(BDSP_Raaga_GetDownloadStatus);
	return BERR_SUCCESS;
}


BERR_Code BDSP_Raaga_Initialize(BDSP_Handle handle)
{

	BERR_Code rc = BERR_SUCCESS ;
    unsigned int uiDspIndex = 0;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)handle->pDeviceHandle;    

	BDBG_ENTER(BDSP_Raaga_StartDsp);
    /* Assert the function arguments*/
    BDBG_ASSERT(handle->pDeviceHandle);

    /*If Firmware Firmware authentication is Disabled*/
    if(pDevice->settings.authenticationEnabled==false) 
    {
        BDBG_ERR(("BDSP_Raaga_StartDsp should be called only if bFwAuthEnable is true"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    for(uiDspIndex = 0; uiDspIndex < BDSP_RAAGA_MAX_DSP; uiDspIndex++)
    {
        /* Initializing the Hardware */
        rc = BDSP_Raaga_P_InitHardware(pDevice,uiDspIndex);
        if (rc!=BERR_SUCCESS) 
        {
            BDBG_MSG(("BDSP_Raaga_P_InitHardware::Error returned %x!",rc));
            return BERR_TRACE(rc);
        }
	}
	BDBG_LEAVE(BDSP_Raaga_StartDsp);
	return rc;
}

BERR_Code BDSP_Raaga_GetDefaultAudioTypeSettings(
        BDSP_AudioType  audioType,
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    )
{    
    if ( audioType >= BDSP_AudioType_eMax )
    {
        BDBG_ASSERT(audioType < BDSP_AudioType_eMax);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if(BDSP_sAudioCodecDetails[audioType]->userConfigSize != settingsBufferSize)
    {
        BDBG_ERR(("settingsBufferSize (%d) is not equal to Config size (%d) of Audio type %d",
            settingsBufferSize,BDSP_sAudioCodecDetails[audioType]->userConfigSize,
            audioType));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BKNI_Memcpy((void *)(volatile void *)pSettingsBuffer,(void *)BDSP_sAudioCodecDetails[audioType]->pDefaultUserConfig,settingsBufferSize);     

    return BERR_SUCCESS;
}

BERR_Code BDSP_Raaga_GetDefaultAudioProcessingSettings(
        BDSP_AudioProcessing  audioProcessingType,
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    )
{    
    if ( audioProcessingType >= BDSP_AudioProcessing_eMax )
    {
        BDBG_ASSERT(audioProcessingType < BDSP_AudioProcessing_eMax);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if(BDSP_sAudioProcessingDetails[audioProcessingType]->userConfigSize != settingsBufferSize)
    {
        BDBG_ERR(("settingsBufferSize (%d) is not equal to Config size (%d) of Processing type %d",
            settingsBufferSize,BDSP_sAudioProcessingDetails[audioProcessingType]->userConfigSize,
            audioProcessingType));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BKNI_Memcpy((void *)(volatile void *)pSettingsBuffer,(void *)BDSP_sAudioProcessingDetails[audioProcessingType]->pDefaultUserConfig,settingsBufferSize);     

    return BERR_SUCCESS;
}

BERR_Code BDSP_Raaga_GetDefaultAudioEncodeSettings(
        BDSP_AudioEncode  audioEncodeType,
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    )
{    
    if ( audioEncodeType >= BDSP_AudioEncode_eMax )
    {
        BDBG_ASSERT(audioEncodeType < BDSP_AudioEncode_eMax);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if(BDSP_sAudioEncodeDetails[audioEncodeType]->userConfigSize != settingsBufferSize)
    {
        BDBG_ERR(("settingsBufferSize (%d) is not equal to Config size (%d) of Encode type %d",
            settingsBufferSize,BDSP_sAudioEncodeDetails[audioEncodeType]->userConfigSize,
            audioEncodeType));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BKNI_Memcpy((void *)(volatile void *)pSettingsBuffer,(void *)BDSP_sAudioEncodeDetails[audioEncodeType]->pDefaultUserConfig,settingsBufferSize);     

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Get Raaga Firmware Debug Data
***************************************************************************/
BERR_Code BDSP_Raaga_GetDebugBuffer(
	BDSP_Handle handle,
	BDSP_Raaga_DebugType debugType, /* [in] Gives the type of debug buffer for which the Base address is required ... UART, DRAM, CoreDump ... */
    uint32_t dspIndex, /* [in] Gives the DSP Id for which the debug buffer info is required */
	void **pBuffer, /* [out] Base address of the debug buffer data */
	size_t *pSize /* [out] Contiguous length of the debug buffer data in bytes */
)
{
	BERR_Code rc = BERR_SUCCESS ;
    BDSP_Raaga *pDevice;

	uint32_t	ui32BaseAddr=0, ui32ReadAddr=0,ui32WriteAddr=0,
				ui32EndAddr=0, ui32ReadSize=0, uiOffset=0,
				ui32DebugFifoOffset=0;

	BDBG_ENTER(BDSP_Raaga_GetDebugBuffer);
    /* Assert the function arguments*/
    BDBG_ASSERT(handle->pDeviceHandle);
    BDBG_ASSERT(pBuffer);
    BDBG_ASSERT(pSize);
	
	pDevice = (BDSP_Raaga *)handle->pDeviceHandle;

	uiOffset = pDevice->dspOffset[dspIndex] ;
	ui32DebugFifoOffset = (BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0)
							*(BDSP_RAAGA_DEBUG_FIFO_START_INDEX + debugType);

	ui32BaseAddr  = BDSP_Read32(pDevice->regHandle, 
						BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + ui32DebugFifoOffset + uiOffset);
	ui32ReadAddr  = BDSP_Read32(pDevice->regHandle, 
						BCHP_RAAGA_DSP_FW_CFG_READ_PTR_FIFO_0  + ui32DebugFifoOffset + uiOffset);
	ui32WriteAddr = BDSP_Read32(pDevice->regHandle, 
						BCHP_RAAGA_DSP_FW_CFG_WRITE_PTR_FIFO_0 + ui32DebugFifoOffset + uiOffset);
	ui32EndAddr   = BDSP_Read32(pDevice->regHandle, 
						BCHP_RAAGA_DSP_FW_CFG_END_ADDR_FIFO_0  + ui32DebugFifoOffset + uiOffset);

	BMEM_ConvertOffsetToAddress(pDevice->memHandle, ui32ReadAddr, pBuffer);
	
	ui32ReadSize = ui32WriteAddr - ui32ReadAddr ;
	if( ui32ReadAddr > ui32WriteAddr )
	{
		/* Bottom Chunk only - Contiguous data*/
		ui32ReadSize  = (ui32EndAddr - ui32ReadAddr);
	}
	
	*pSize = ui32ReadSize;
	BDBG_LEAVE(BDSP_Raaga_GetDebugBuffer);

	return rc;
}

/***************************************************************************
Summary:
Consume debug data from the debug ringbuffer.  
***************************************************************************/
BERR_Code BDSP_Raaga_ConsumeDebugData(
	BDSP_Handle handle,
	BDSP_Raaga_DebugType debugType, /* [in] Gives the type of debug buffer for which the Base address is required ... UART, DRAM, CoreDump ... */
    uint32_t dspIndex, /* [in] Gives the DSP Id for which the debug data needs to be consumed */
	size_t bytesConsumed    /* [in] Number of bytes consumed from the debug buffer */
)
{
	BERR_Code rc = BERR_SUCCESS;
    BDSP_Raaga *pDevice;    

	uint32_t	ui32BaseAddr=0, ui32ReadAddr=0,ui32WriteAddr=0,
				ui32EndAddr=0, ui32ReadSize=0, uiOffset=0,
				ui32DebugFifoOffset=0;
				
	BDBG_ENTER(BDSP_Raaga_ConsumeDebugData);
    /* Assert the function arguments*/
    BDBG_ASSERT(handle->pDeviceHandle);
	
	pDevice = (BDSP_Raaga *)handle->pDeviceHandle;
	
	uiOffset = pDevice->dspOffset[dspIndex] ;
	ui32DebugFifoOffset = (BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0)
							*(BDSP_RAAGA_DEBUG_FIFO_START_INDEX + debugType);

	ui32BaseAddr  = BDSP_Read32(pDevice->regHandle, 
						BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + ui32DebugFifoOffset + uiOffset);
	ui32ReadAddr  = BDSP_Read32(pDevice->regHandle, 
						BCHP_RAAGA_DSP_FW_CFG_READ_PTR_FIFO_0  + ui32DebugFifoOffset + uiOffset);
	ui32WriteAddr = BDSP_Read32(pDevice->regHandle, 
						BCHP_RAAGA_DSP_FW_CFG_WRITE_PTR_FIFO_0 + ui32DebugFifoOffset + uiOffset);
	ui32EndAddr   = BDSP_Read32(pDevice->regHandle, 
						BCHP_RAAGA_DSP_FW_CFG_END_ADDR_FIFO_0  + ui32DebugFifoOffset + uiOffset);

	/* Get the amount data available in the buffer*/
	ui32ReadSize = ui32WriteAddr - ui32ReadAddr ;
	if( ui32ReadAddr > ui32WriteAddr )
	{
		/* Bottom Chunk + Top Chunk */
		ui32ReadSize  = (ui32EndAddr - ui32ReadAddr) + (ui32WriteAddr - ui32BaseAddr);
	}
	
	if (bytesConsumed <= ui32ReadSize)
	{
		ui32ReadAddr += bytesConsumed;
		if(ui32ReadAddr >= ui32EndAddr)
		{
			ui32ReadAddr = ui32BaseAddr + (ui32ReadAddr - ui32EndAddr);
		}
		
		BDSP_Write32(pDevice->regHandle, BCHP_RAAGA_DSP_FW_CFG_READ_PTR_FIFO_0  + ui32DebugFifoOffset + 
						uiOffset, ui32ReadAddr);
	}
	else
	{
		/* Return error if bytesConsumed is more 
		   than the data available in the buffer */
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	
	BDBG_LEAVE(BDSP_Raaga_ConsumeDebugData);
	
	return rc;
}

/***************************************************************************
Summary:
Returns the Status of the DSP
***************************************************************************/
BDSP_Raaga_FwStatus BDSP_Raaga_GetCoreDumpStatus (
    BDSP_Handle handle,
    uint32_t dspIndex) /* [in] Gives the DSP Id for which the core dump status is required */
{
	uint32_t            uiOffset;

	BDSP_Raaga *pDevice;
	BDSP_Raaga_FwStatus eStatus;
	
    BDBG_ASSERT(handle->pDeviceHandle);

	pDevice = (BDSP_Raaga *)handle->pDeviceHandle;

	uiOffset = pDevice->dspOffset[dspIndex];
	eStatus = BDSP_Read32 (pDevice->regHandle, BCHP_RAAGA_DSP_FW_CFG_SW_UNDEFINED_SPAREi_ARRAY_BASE + uiOffset + 20*4);
	
	return eStatus;
}

/***************************************************************************
Summary:
API to boot SCM related code
***************************************************************************/
#if 0
BERR_Code BDSP_Raaga_LoadScm(BDSP_Handle handle, uint32_t dspIndex, unsigned uiScmIndex)
{
	return BDSP_Raaga_P_LoadScm(handle->pDeviceHandle, dspIndex, uiScmIndex);
}
#endif
