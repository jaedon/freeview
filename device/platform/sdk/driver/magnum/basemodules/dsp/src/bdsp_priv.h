/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 2/6/13 6:55p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/bdsp_priv.h $
 * 
 * Hydra_Software_Devel/13   2/6/13 6:55p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/5   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/12   11/26/12 1:18p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * $brcm_Log: /magnum/basemodules/dsp/src/bdsp_priv.h $
 * 
 * Hydra_Software_Devel/13   2/6/13 6:55p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/5   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/SW7425-4216/4   1/8/13 4:58p gprasad
 * SW7425-4216: Merging mainline changes to rbuf capture branch as of
 * 01/08/2012
 * 
 * Hydra_Software_Devel/SW7425-4216/3   12/4/12 5:06p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/SW7425-4216/2   11/28/12 4:25p gprasad
 * SW7425-4216: Added capture create and destroy apis for configuring
 * capture
 * 
 * Hydra_Software_Devel/11   2/22/12 7:21p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/10   2/15/12 3:02p ashoky
 * SW7425-2308: Adding interface to put audio firmware in audio gap fill
 * mode.
 * 
 * Hydra_Software_Devel/9   1/27/12 1:02p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/8   7/4/11 6:35p gprasad
 * SW7422-373: Add GetInfo routines for AudioType, AudioProcessing, and
 * AudioEncode in BDSP
 * 
 * Hydra_Software_Devel/7   6/17/11 3:50p gautamk
 * SW7422-373: [7422] Adding GetInfo routines for AudioType,
 * AudioProcessing, and AudioEncode support in BDSP
 * 
 * Hydra_Software_Devel/6   6/15/11 5:47p gautamk
 * SW7425-683: [7425] Adding support for Power management  in BDSP
 * 
 * Hydra_Software_Devel/5   6/15/11 4:20p gautamk
 * SW7422-357 : [7425] Adding support for new API for getStageInputIndex
 * 
 * Hydra_Software_Devel/4   6/14/11 3:09p gautamk
 * SW7422-357 : [7425] Merging to mainline
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/1   6/10/11 6:20p gautamk
 * SW7422-357 : [7425] Modifying API prtotype for Add/Remove stage input
 * 
 * Hydra_Software_Devel/3   6/9/11 8:12p gautamk
 * SW7422-357 : [7425] Modifying API prtotype for Add/Remove stage input
 * 
 * Hydra_Software_Devel/2   6/8/11 4:50p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/1   4/6/11 2:16p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/6   1/26/11 12:58a gautamk
 * SW7422-191: reomving redundant parameter.
 * 
 * Hydra_Software_Devel/5   1/20/11 8:13a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/4   1/18/11 4:58a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/3   1/13/11 5:59a gautamk
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
 * Hydra_Software_Devel/1   12/15/10 2:31p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/

#ifndef BDSP_PRIV_H_
#define BDSP_PRIV_H_

#include "bkni.h"
#include "bkni_multi.h"
#include "bdbg.h"

BDBG_OBJECT_ID_DECLARE(BDSP_Device);

#define BDSP_INVALID_INDEX	                (unsigned int)-1


typedef struct BDSP_Device
{
    BDBG_OBJECT(BDSP_Device)
    void *pDeviceHandle;
    /* Device-level Function table */
    void (*close)(void *pDeviceHandle);
    void (*initialize)(void *pDeviceHandle);
    void (*getStatus)(void *pDeviceHandle, BDSP_Status *pStatus);
    BERR_Code (*getDebugLog)(void *pDeviceHandle, char *pBuffer, size_t bufferLength);
    void (*getDefaultContextSettings)(void *pDeviceHandle,BDSP_ContextType contextType, BDSP_ContextCreateSettings *pSettings);
    BERR_Code (*createContext)(void *pDeviceHandle, const BDSP_ContextCreateSettings *pSettings, BDSP_ContextHandle *pContext);
    BERR_Code (*powerStandby)(void *pDeviceHandle, BDSP_StandbySettings 	*pSettings);
    BERR_Code (*powerResume)(void *pDeviceHandle);
    void (*getAudioTypeInfo)(BDSP_AudioType type, const BDSP_AudioTypeInfo **pInfo);
    void (*getAudioProcessingInfo)(BDSP_AudioProcessing type, const BDSP_AudioProcessingInfo **pInfo);
    void (*getAudioEncodeInfo)(BDSP_AudioEncode type, const BDSP_AudioEncodeInfo **pInfo);
	/* Below functions provide external interrupt handles to SW */
	BERR_Code (*allocateExternalInterrupt)(void *pDeviceHandle, uint32_t dspIndex, BDSP_ExternalInterruptHandle *pInterruptHandle);
	BERR_Code (*freeExternalInterrupt)(void  *pInterruptHandle);
	BERR_Code (*getExternalInterruptInfo)(void *pInterruptHandle, BDSP_ExternalInterruptInfo **pInfo);
	/* Below functions provide RDB register handles to SW */
	BERR_Code (*allocateRdbRegister)(void *pDeviceHandle, uint32_t dspIndex, uint32_t numRegs, BDSP_RdbRegisterHandle *pRdbRegisterHandle);
	BERR_Code (*freeRdbRegister)(void  *pRdbRegisterHandle);
	BERR_Code (*getRdbRegisterInfo)(void *pRdbRegisterHandle, BDSP_RdbRegisterInfo **pInfo);
    BERR_Code (*processAudioCapture)(void *pDeviceHandle);

}BDSP_Device;

void BDSP_P_InitDevice(
    BDSP_Device *pDevice,
    void *pDeviceHandle
    );

BDBG_OBJECT_ID_DECLARE(BDSP_Context);

typedef struct BDSP_Context
{
    BDBG_OBJECT(BDSP_Context)
    void *pContextHandle;
    /* Context-level Function table */
    void (*destroy)(void *pContextHandle);
    void (*getInterruptHandlers)(void *pContextHandle, BDSP_ContextInterruptHandlers *pInterrupts);
    BERR_Code (*setInterruptHandlers)(void *pContextHandle, const BDSP_ContextInterruptHandlers *pInterrupts);
    BERR_Code (*processWatchdogInterrupt)(void *pContextHandle);
    void (*getDefaultTaskSettings)(void *pContextHandle, BDSP_TaskCreateSettings *pSettings);
    BERR_Code (*createTask)(void *pContextHandle, const BDSP_TaskCreateSettings *pSettings, BDSP_TaskHandle *pTask);
    /* Capture api */
    BERR_Code (*createCapture)(void *pContextHandle, const BDSP_AudioCaptureCreateSettings *pCaptureCreateSettings, BDSP_AudioCaptureHandle *pCapture);
}BDSP_Context;

void BDSP_P_InitContext(
    BDSP_Context *pContext,
    void *pContextHandle
    );

BDBG_OBJECT_ID_DECLARE(BDSP_Task);


typedef struct BDSP_Task
{
    BDBG_OBJECT(BDSP_Task)
    void *pTaskHandle;
    /* Task-level Function table */
    void      (*destroy)(void *pTaskHandle);
    BERR_Code (*start)(void *pTaskHandle);
    BERR_Code (*stop)(void *pTaskHandle);
    BERR_Code (*getStageSettings)(void *pTaskHandle, unsigned branchId, unsigned stageId, void *pSettingsBuffer, size_t settingsSize);
    BERR_Code (*setStageSettings)(void *pTaskHandle, unsigned branchId, unsigned stageId, const void *pSettingsBuffer, size_t settingsSize);
    BERR_Code (*getStageStatus)(void *pTaskHandle, unsigned branchId, unsigned stageId, void *pStatusBuffer, size_t statusSize);
    /* Audio task Function table */
    BERR_Code (*pause)(void *pTaskHandle);
    BERR_Code (*resume)(void *pTaskHandle);
    BERR_Code (*advance)(void *pTaskHandle, unsigned ms);
    BERR_Code (*getTsmSettings_isr)(void *pTaskHandle, unsigned branchId, unsigned stageId, BDSP_AudioTaskTsmSettings *pTsmSettings);
    BERR_Code (*setTsmSettings_isr)(void *pTaskHandle, unsigned branchId, unsigned stageId, const BDSP_AudioTaskTsmSettings *pTsmSettings);
    BERR_Code (*getTsmStatus_isr)(void *pTaskHandle, unsigned branchId, unsigned stageId, BDSP_AudioTaskTsmStatus *pTsmStatus);
    BERR_Code (*getDatasyncSettings)(void *pTaskHandle, unsigned branchId, unsigned stageId, BDSP_AudioTaskDatasyncSettings *pSettings);
    BERR_Code (*setDatasyncSettings)(void *pTaskHandle, unsigned branchId, unsigned stageId, const BDSP_AudioTaskDatasyncSettings *pSettings);
    BERR_Code (*getDatasyncStatus)(void *pTaskHandle, unsigned branchId, unsigned stageId, BDSP_AudioTaskDatasyncStatus *pStatus);
    BERR_Code (*addStageInput)(void *pTaskHandle,const BDSP_TaskAddStageInputSettings *pSettings,BDSP_TaskStageInputHandle *pHandle);
    BERR_Code (*removeStageInput)(void *pStageInputHandle);
    BERR_Code (*getStageInputIndex)(void *pStageInputHandle,unsigned *inputIndex);    
    BERR_Code (*addStageOutput)(void *pTaskHandle, unsigned branchId, unsigned stageId, const BDSP_CIT_P_FwStgSrcDstDetails *pOutputInfo);
    BERR_Code (*removeStageOutput)(void *pTaskHandle, unsigned branchId, unsigned stageId, const BDSP_CIT_P_FwStgSrcDstDetails *pOutputInfo);
    void      (*getAudioInterruptHandlers_isr)(void *pTaskHandle, BDSP_AudioInterruptHandlers *pHandlers);
    BERR_Code (*setAudioInterruptHandlers_isr)(void *pTaskHandle, const BDSP_AudioInterruptHandlers *pHandlers);
    BERR_Code (*audioGapFill)(void *pTaskHandle);

    /* Video task Function table */
    BERR_Code (*getPictureCount_isr)(void *pTaskHandle,unsigned *pPictureCount);
    BERR_Code (*peekAtPicture_isr)(void *pTaskHandle, unsigned index,BXDM_Picture **pUnifiedPicture);
    BERR_Code (*getNextPicture_isr)(void *pTaskHandle,BXDM_Picture **pUnifiedPicture);
    BERR_Code (*releasePicture_isr)(void *pTaskHandle,BXDM_Picture *pUnifiedPicture);
    BERR_Code (*getPictureDropPendingCount_isr)(void *pTaskHandle, unsigned *pPictureDropPendingCount);
    BERR_Code (*requestPictureDrop_isr)(void *pTaskHandle,unsigned *pPictureDropRequestCount);
    BERR_Code (*displayInterruptEvent_isr)(void *pTaskHandle);

	/* Video Encode task Function table */
	BERR_Code (*getVideoEncodeDatasyncSettings)(void *pTaskHandle, unsigned branchId, unsigned stageId, BDSP_VideoEncodeTaskDatasyncSettings *pSettings);
	BERR_Code (*setVideoEncodeDatasyncSettings)(void *pTaskHandle, unsigned branchId, unsigned stageId, const BDSP_VideoEncodeTaskDatasyncSettings *pSettings);
	BERR_Code (*getPictureBuffer_isr)(void *pTaskHandle, BVENC_VF_sPicParamBuff	**pPpBuf);
	BERR_Code (*putPicture_isr)(void *pTaskHandle, uint32_t	ui32PictureParamBufAddress);
    
    BERR_Code (*sendScmCommand)(void *pTaskHandle, BDSP_Raaga_P_SCM_CmdPayload *pScmCmdPayload);
}BDSP_Task;


/***************************************************************************
Summary: 
        BDSP Capture Handle
***************************************************************************/
BDBG_OBJECT_ID_DECLARE(BDSP_AudioCapture);

typedef struct BDSP_AudioCapture
{
    BDBG_OBJECT(BDSP_AudioCapture)
    void *pCapHandle;
    
    void (*destroy)(void *pCapHandle);
    BERR_Code (*add)(void *pCapHandle, void *pTaskHandle, const BDSP_AudioTaskAddCaptureSettings *pSettings);
    void (*remove)(void *pCapHandle, void *pTaskHandle);
    BERR_Code (*getBuffer)(void *pCapHandle, BDSP_BufferDescriptor *pBuffers);
    BERR_Code (*consumeData)(void *pCapHandle, unsigned numBytes);
} BDSP_AudioCapture;

BDBG_OBJECT_ID_DECLARE(BDSP_ExternalInterrupt);

/* Handle for External interrupt to DSP */
typedef struct BDSP_ExternalInterrupt
{
	BDBG_OBJECT(BDSP_ExternalInterrupt)
	BDSP_Handle	hDsp;
	void * pExtInterruptHandle;

}BDSP_ExternalInterrupt;

BDBG_OBJECT_ID_DECLARE(BDSP_RdbRegister);

/* Handle for a RDB register */
typedef struct BDSP_RdbRegister
{
	BDBG_OBJECT(BDSP_RdbRegister)
	BDSP_Handle	hDsp;
	void * pRdbRegisterHandle;
}BDSP_RdbRegister;
	

BDBG_OBJECT_ID_DECLARE(BDSP_TaskStageInput);


typedef struct BDSP_TaskStageInput
{
    BDBG_OBJECT(BDSP_TaskStageInput)
    BDSP_TaskHandle task;
    void *pStageInputHandle;
}BDSP_TaskStageInput;

void BDSP_P_InitTask(
    BDSP_Task *pTask,
    void *pTaskHandle
    );

#define BDSP_Read32(hReg, addr)             BREG_Read32(hReg, addr)
#define BDSP_Write32(hReg, addr, data)      BREG_Write32(hReg, addr, data)
#define BDSP_Read32_isr(hReg, addr)         BREG_Read32_isr(hReg, addr)
#define BDSP_Write32_isr(hReg, addr, data)  BREG_Write32_isr(hReg, addr, data)

#endif /* #ifndef BDSP_PRIV_H_ */

