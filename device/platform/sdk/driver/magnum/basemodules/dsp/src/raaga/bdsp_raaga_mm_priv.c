/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_mm_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 1/3/13 5:21p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_mm_priv.c $
 * 
 * Hydra_Software_Devel/23   1/3/13 5:21p ananthan
 * SW7425-3628: Changes for cached access (Merge to mainline)
 * 
 * Hydra_Software_Devel/SW7425-3628/1   8/29/12 5:57p jgarrett
 * SW7425-3628: Converting BMEM accesses to cached memory
 * 
 * Hydra_Software_Devel/22   11/26/12 1:21p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/21   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/20   3/1/12 8:49p ashoky
 * SW7231-585: Changing the RDQ implementation from DRAM based to RDB
 * based.
 * 
 * Hydra_Software_Devel/19   2/8/12 8:18p ashoky
 * SW7346-598: Removing dynamic mem-alloc/free from decode start stop
 * time. Moving it to device  open/close.
 * 
 * Hydra_Software_Devel/18   2/3/12 9:31p gprasad
 * SW7425-1752: Flac codec support
 * 
 * Hydra_Software_Devel/17   2/3/12 11:53a ashoky
 * FWRAA-362: Raaga dsp sram optimization. Providing memory to swap
 * resident data.
 * 
 * Hydra_Software_Devel/16   1/27/12 1:20p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/15   1/3/12 2:21p ashoky
 * SW7435-5: Adding support for 2nd DSP into mainline.
 * 
 * Hydra_Software_Devel/14   12/12/11 9:44p ashoky
 * SW7231-531: Correct raaga heap memory calculation in bdsp. Correcting
 * interframe size for video decoder on dsp.
 * 
 * Hydra_Software_Devel/13   8/18/11 12:53p gprasad
 * SW7422-400: Updating the APIs with review comments
 * 
 * Hydra_Software_Devel/12   7/4/11 6:35p gprasad
 * SW7422-400: Please add hooks to support dumping the UART debug buffer
 * into DRAM
 * 
 * Hydra_Software_Devel/11   6/21/11 2:15p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/10   6/8/11 4:43p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/9   5/28/11 10:53p srajapur
 * SW7422-358 : [7425] Added support for MS11 DDRE and DV258 to BDSP
 * 
 * Hydra_Software_Devel/8   5/27/11 11:46a srajapur
 * SW7425-476 : [7425]  Return code from BMEM_Heap_FlushCache is ignored
 * 
 * Hydra_Software_Devel/7   5/16/11 10:08p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/6   4/25/11 2:17p srajapur
 * SW7425-392: [7425] Corrected the Encode enum type.
 * 
 * Hydra_Software_Devel/5   4/20/11 1:10p jgarrett
 * SW7425-392: Removing BDSP_ProcessingType enum
 * 
 * Hydra_Software_Devel/4   4/14/11 6:52p srajapur
 * SW7425-291: [7425] BDSP directory structure changes
 * 
 * Hydra_Software_Devel/3   4/13/11 8:22p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/2   4/6/11 2:44p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   4/6/11 2:28p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/13   3/15/11 12:28p jgarrett
 * SW7422-146: Removing externs from C files
 * 
 * Hydra_Software_Devel/12   3/5/11 4:52a srajapur
 * SW7422-314 : [7422] Adding RDB Independence Feature in BASE DSP
 * Codebase
 * 
 * Hydra_Software_Devel/11   2/26/11 12:40a srajapur
 * SW7422-293:[7422] Adding support for encode algorithms
 * 
 * Hydra_Software_Devel/10   1/27/11 4:11a gautamk
 * SW7422-191: Fixing memory leak
 * 
 * Hydra_Software_Devel/9   1/26/11 3:17a gautamk
 * SW7422-191: Allocating FW debug buffer on basis of runtime Parrameter
 * enableDebugLog of BDSP_RaagaSettings.
 * 
 * Hydra_Software_Devel/8   1/25/11 4:02a gautamk
 * SW7422-191: Adding error recovery in Create Context
 * 
 * Hydra_Software_Devel/7   1/25/11 2:23a gautamk
 * SW7422-191:Adding error recovery code for Raaga Open
 * 
 * Hydra_Software_Devel/6   1/20/11 8:11a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/5   1/19/11 7:15a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/4   1/19/11 2:15a gautamk
 * SW7422-191: putting all the Details of Audio codec in a const array and
 * then using it across the code.
 * 
 * Hydra_Software_Devel/3   1/18/11 4:20a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/2   1/15/11 3:33a gautamk
 * SW7422-191:[7422] CLeaning up unwanted debug message
 * 
 * Hydra_Software_Devel/1   1/13/11 5:31a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 ***************************************************************************/
#include "bdsp_raaga.h"
#include "bdsp_raaga_mm_priv.h"
#include "bdsp_raaga_priv.h"
#include "bdsp_raaga_fwdownload_priv.h"
#include "bdsp_raaga_cmdresp_priv.h"
#include "bdsp_raaga_fw.h"
#include "bdsp_raaga_fw_settings.h"

BDBG_MODULE(bdsp_raaga_mm);

/******* ********************************************************************
Summary:    This function returns the memory requirement for downloading executable.
	Memory allocation should interpret the requirement in following manner.

	If(true == hRap->sSettings.bOpenTimeFwDownload) 
	{
		Then allocate consecutive memory of size pui32Size, And then 
		Initialize the following pointers as well according to size in strictly 
		following sequence.

			hRap->sDwnldMemInfo->ui32OSKernalCodePtr;
			hRap->sDwnldMemInfo->ui32TsmCodePtr;
			hRap->sDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32BaseAddr;		
			hRap->sDwnldMemInfo->sFsBufInfo[0].ui32BaseAddr;
			hRap->sDwnldMemInfo->sDecCodecBufInfo[0].ui32BaseAddr;
			hRap->sDwnldMemInfo->sPtCodecBufInfo[0].ui32BaseAddr;	

		Sizes for them are stored in following parameters correspondingly

			pDwnldMemInfo->ui32OSKernalui32CodeSize;
			pDwnldMemInfo->ui32Tsmui32CodeSize;
			pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size;		
			pDwnldMemInfo->sFsBufInfo[0].ui32Size;
			pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size;
			pDwnldMemInfo->sPtCodecBufInfo[0].ui32Size;	
	}
	else
	{
		Allocate as following.
			-- Allocate pDwnldMemInfo->ui32OSKernalui32CodeSize	
				and store ptr at hRap->sDwnldMemInfo->ui32OSKernalCodePtr;
			-- Allocate pDwnldMemInfo->ui32Tsmui32CodeSize	
				and store ptr at hRap->sDwnldMemInfo->ui32TsmCodePtr;
			if()
			{
				-- Allocate pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size
					and store ptr at 
					hRap->sDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32BaseAddr;					
			}
			else
			{
				-- Allocate BDSP_RAAGA_MAX_AUD_PROCESSING_CTX buffer of size 
				    pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size and store 
				    ptr at hRap->sDwnldMemInfo->sProcessingAlgoBufInfo
					[0 - (BDSP_RAAGA_MAX_AUD_PROCESSING_CTX-1)].ui32BaseAddr;					
			}
			
			-- Allocate BDSP_RAAGA_MAX_AUD_PROCESSING_CTX buffer of size 
			    pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size and store 
			    ptr at hRap->sDwnldMemInfo->sProcessingAlgoBufInfo
				[0 - (BDSP_RAAGA_MAX_AUD_PROCESSING_CTX-1)].ui32BaseAddr;

			-- Allocate BDSP_RAAGA_MAX_FS_CTXT buffer of size 
			    pDwnldMemInfo->sFsBufInfo[0].ui32Size and store ptr at 
			    hRap->sDwnldMemInfo->sFsBufInfo
			    [0 - (BDSP_RAAGA_MAX_FS_CTXT - 1)].ui32BaseAddr;

			-- Allocate BDSP_RAAGA_MAX_DECODE_CTXT buffer of size 
			    pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size; and store ptr at 
			    hRap->sDwnldMemInfo->sDecCodecBufInfo
			    [0 - (BDSP_RAAGA_MAX_DECODE_CTXT - 1)].ui32BaseAddr;

			-- Allocate BDSP_RAAGA_MAX_PASSTHRU_CTXT buffer of size 
			    pDwnldMemInfo->sPtCodecBufInfo[0].ui32Size and store ptr at 
			    hRap->sDwnldMemInfo->sPtCodecBufInfo
			    [0 - (BDSP_RAAGA_MAX_PASSTHRU_CTXT - 1)].ui32BaseAddr;
	}
******************************************************************************/
static BERR_Code BDSP_MM_P_GetDownloadMemRequirement(
       void *pDeviceHandle,
	BDSP_Raaga_P_DwnldMemInfo *pDwnldMemInfo,
	uint32_t	*pui32Size);


/******* Summary
        Call BDSP_DSP_DownloadMemReq to get download memory requiremnts.
        --
        For (All Processing Algos Supported)
        NOTE: This can be further optimized by allocing combined sizes of 
        BDSP_MAX_FW_PROCESSING_SUPPORTED & providing memory from there but 
        that will enforce that all PAlgos are downloaded at open time only. 
        Same logic applies for IFrameBufs & ConfigParams 
        {
            Find max Processing Algo size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32PAlgo
        --
        For(BDSP_MAX_FW_TASK_SUPPORTED)  Based on usage mode 
            For (All Codecs Supported)
            {
                Find max codec scratch size required
            }
            Put in TempScratchSize
            For (All Processing Algos Supported)
            {
                Add up Scratch Size required
            }
            Add that to TempScratchSize
            Fill psDspMem->sDspMemSizeReq[uiDspId].ui32Scratch with TempScratchSize
        --
        For(BDSP_MAX_FW_TASK_SUPPORTED)  Based on usage mode 
            For (All Codecs Supported)
            {
                Find max codec IStage size required
            }
            Put in TempIStageSize
            For (All Processing Algos Supported)
            {
                Add up IStage Size required
            }
            Add that to TempIStageSize
            Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IStage with TempIStageSize
        --
        For (All Codecs Supported)
        {
            Find max codec Iframe size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IFrameCommon
        --
        For (All Processing Algos Supported)
        {
            Find max Processing Algo Iframe size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IFramePAlgo
        --
        For (All Codecs Supported)
        {
            Find max codec ConfigParams size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32CfgBufCommon
        --
        For (All Processing Algos Supported)
        {
            Find max Processing Algo ConfigParams size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32CfgBufPAlgo
        
********/
static BERR_Code BDSP_MM_P_CalcMemPoolReq (    
    void *pContextHandle
    );


static BERR_Code BDSP_MM_P_GetDownloadMemRequirement(
		void *pDeviceHandle,
		BDSP_Raaga_P_DwnldMemInfo *pDwnldMemInfo,
		uint32_t	*pui32Size)
{
    bool bOpenTimeAudProcDownload = true;
    uint32_t count = 0, i = 0;
    BDSP_Img_Id eCodeFirmwareId,eTblFirmwareId;
    uint32_t uiTempSize = 0;
    bool isDownloaded = false;
    BDSP_AF_P_sALGO_EXEC_INFO	sExecId;
    BDSP_AF_P_AlgoId codeExecId = BDSP_AF_P_AlgoId_eMax, eTempExecId;       
    bool bIsExecAllocated[BDSP_AF_P_AlgoId_eMax];         
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
    BDBG_ENTER(BDSP_MM_P_GetDownloadMemRequirement);

    *pui32Size = 0;

    /* Array to remember if the memory is not getting allocated twice 
        for same Executable*/
    for(i=0; i < BDSP_AF_P_AlgoId_eMax; i++)
    {
        bIsExecAllocated[i] = false;
    }
	
    BDBG_ASSERT(pDwnldMemInfo);
    
    if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)pDwnldMemInfo) == true)
    {
          return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else
    {
        BKNI_Memset(pDwnldMemInfo,0,sizeof(BDSP_Raaga_P_DwnldMemInfo));
    }

	/* Sum of sizes of resident code Buf*/
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSystemCode, 
                                &( pDwnldMemInfo->sSystemCodeMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sSystemCodeMem.ui32Size;
	
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSystemData, 
                                &(pDwnldMemInfo->sSystemDataMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sSystemDataMem.ui32Size;

    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSystemRdbvars, 
                                &(pDwnldMemInfo->sSystemRdbVarsMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sSystemRdbVarsMem.ui32Size;    

    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSyslibCode, 
                                &(pDwnldMemInfo->sSystemLibMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sSystemLibMem.ui32Size;

    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAlgolibCode, 
                                &(pDwnldMemInfo->sAlgoLibMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sAlgoLibMem.ui32Size;

    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCommon_Ids_Code, 
                                &( pDwnldMemInfo->sCommonIdsCodeMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sCommonIdsCodeMem.ui32Size;

	BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eScm_Task_Code, 
                                &( pDwnldMemInfo->sScmTaskCodeMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sScmTaskCodeMem.ui32Size;

    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eScm1_Digest,
                                &( pDwnldMemInfo->sScm1DigestMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sScm1DigestMem.ui32Size;

    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eScm2_Digest, 
                                &( pDwnldMemInfo->sScm2DigestMem.ui32Size));
	*pui32Size += pDwnldMemInfo->sScm2DigestMem.ui32Size;

	uiTempSize = 0;

	BDSP_Raaga_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);

	if(true == bOpenTimeAudProcDownload)
	{		
    	/* Sum of sizes of Post Processing Exec Buf*/
		for(count = 0; count < BDSP_AudioProcessing_eMax; count++)
		{
		    /* Check if audio processing algorithm is supported */
			if(!BDSP_Raaga_IsAudioProcessingSupported(count))
			{
				continue;
			}
            
			BDSP_Raaga_P_GetAudProcExecIds((BDSP_AudioProcessing)count,&sExecId);
            if(count == BDSP_AudioProcessing_eFWMixer)
            {
				/* Get size of fwmixer framesync processing  */	
                codeExecId = sExecId.eAlgoIds[0];
    			eCodeFirmwareId = \
                    pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}

                if(true == bIsExecAllocated[codeExecId])
                {
                    continue;
                }
                else
                {
                    bIsExecAllocated[codeExecId] = true;
                }

                uiTempSize += \
                    pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];
            }
            
            /* For each stage of audio processing algorithm	*/	
			for(i = BDSP_CIT_P_POSTPROCESS_NODE_INFO_INDEX; i<sExecId.NumNodes; i++)
			{
				/* Get size of audio processing code */	
                codeExecId = sExecId.eAlgoIds[i];
    			eCodeFirmwareId = \
                    pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}
                
                if(true == bIsExecAllocated[codeExecId])
                {
                    continue;
                }
                else
                {
                    bIsExecAllocated[codeExecId] = true;
                }
                
                uiTempSize += \
                    pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];
                
                /* Get size of audio processing table */	
				eTblFirmwareId = \
				    pDevice->imgIdMappingArrays.tableImgId[codeExecId];
                
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
                
				/*If the previous table is used for this stage, dont sum its size */
                isDownloaded =false;

                BDSP_Raaga_P_IsTableDownloaded (
                    BDSP_Raaga_P_ExecType_eProcessingAlgo,
                    codeExecId,&isDownloaded,&eTempExecId);
                
				if(!isDownloaded)
				{
					uiTempSize += \
                                    pDevice->imgIdMappingArrays.tableSize[codeExecId];
				}
			}
		}
		
		pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;
	}
	else
	{
		/* BDSP_RAAGA_MAX_AUD_PROCESSING_CTX buffers of 
            size MAX(sizeof(ProcessingAlgo Code +Table))*/
            
		/* Sum of sizes of Post Processing Exec Buf*/
		for(count = 0; count < BDSP_AudioProcessing_eMax; count++)
		{
		    /* Check if audio processing algorithm is supported */
			if(!BDSP_Raaga_IsAudioProcessingSupported(count))
			{
				continue;
			}
			uiTempSize = 0;			
			BDSP_Raaga_P_GetAudProcExecIds(
                (BDSP_AudioProcessing)count,&sExecId);
            
		    /* For each stage of audio processing algorithm	*/	
			for(i = BDSP_CIT_P_POSTPROCESS_NODE_INFO_INDEX; i<sExecId.NumNodes; i++)
			{
				/* Get size of audio processing code */	
                codeExecId = sExecId.eAlgoIds[i];
    			eCodeFirmwareId = \
                    pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}
                
                uiTempSize += \
                    pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];

				/* Get size of audio processing table */	
				eTblFirmwareId = \
				    pDevice->imgIdMappingArrays.tableImgId[codeExecId];
                
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
                
				/*If the previous table is used for this stage, dont sum its size */
                isDownloaded = false;
				BDSP_Raaga_P_IsTableDownloaded (
                    BDSP_Raaga_P_ExecType_eProcessingAlgo,
                    codeExecId,&isDownloaded,&eTempExecId);
                
				if(!isDownloaded)
				{
					uiTempSize += \
                                pDevice->imgIdMappingArrays.tableSize[codeExecId];
				}
			}

			if(pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sProcessingAlgoBufInfo[0].ui32Size = uiTempSize;
		}
	}

	if(true == pDevice->settings.preloadImages)
	{
            uiTempSize =0;        
        /* Sum of sizes of Encode Exec Buf*/
		for(count = 0; count < BDSP_AudioEncode_eMax; count++)
		{
		    /* Check if Encode algorithm is supported */
			if(!BDSP_Raaga_P_IsAudioEncodeSupported(count))
			{
				continue;
			}
			BDSP_Raaga_P_GetEncodeExecIds(
                (BDSP_AudioEncode)count,&sExecId);

            /* Frame sync of Encode */
            codeExecId = sExecId.eAlgoIds[0];
            if(codeExecId != BDSP_AF_P_AlgoId_eInvalid)
            {
                eCodeFirmwareId = \
                    pDevice->imgIdMappingArrays.codeImgId[codeExecId];

                if(BDSP_Img_Id_eInvalid != eCodeFirmwareId)
                {	
                    if(false == bIsExecAllocated[codeExecId])
                    {
                        bIsExecAllocated[codeExecId] = true;
                        uiTempSize += \
                            pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];                                        
                    }
                }
            }
            
		    /* For each stage of Encode algorithm	*/	
			for(i = BDSP_CIT_P_ENCODE_NODE_INFO_INDEX; i<sExecId.NumNodes; i++)
			{
				/* Get size of audio processing code */	
                codeExecId = sExecId.eAlgoIds[i];
                if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }
                
    			eCodeFirmwareId = \
                    pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					break;
				}
                
                if(true == bIsExecAllocated[codeExecId])
                {
                    continue;
                }
                else
                {
                    bIsExecAllocated[codeExecId] = true;
                }
				uiTempSize += \
                    pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];

				/* Get size of Encode table */	
				eTblFirmwareId = \
				    pDevice->imgIdMappingArrays.tableImgId[codeExecId];
                
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
                
				/*If the previous table is used for this stage, 
				    dont sum its size */
                isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded (
                    BDSP_Raaga_P_ExecType_eEncode,
                    codeExecId,&isDownloaded,&eTempExecId);
                
				if(!isDownloaded)
				{
					uiTempSize += \
                                    pDevice->imgIdMappingArrays.tableSize[codeExecId];
				}
			}
        }
		pDwnldMemInfo->sEncodeCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;
    
		for(count = 0; count < BDSP_AudioType_eMax; count++)
		{
		    /* Check if decode algorithm is supported */
            if(!(BDSP_Raaga_P_IsPassthruSupportedWithoutLicense(count)))
            {
    			if(!BDSP_Raaga_P_IsAudioTypeSupported(count))
    			{
    				continue;
    			}
            }
			BDSP_Raaga_P_GetExecIds (
                BDSP_DecodeMode_eDecode,
                (BDSP_AudioType)count,&sExecId);
            
		    /* Sum of sizes of Framesync Exec */
			/* For each stage framesync	*/	
			
            codeExecId = sExecId.eAlgoIds[0];
            if(codeExecId != BDSP_AF_P_AlgoId_eInvalid)
            {
                eCodeFirmwareId = \
                    pDevice->imgIdMappingArrays.codeImgId[codeExecId];

                if(BDSP_Img_Id_eInvalid != eCodeFirmwareId)
                {                            
                    if(false == bIsExecAllocated[codeExecId])
                    {
                        bIsExecAllocated[codeExecId] = true;
                        uiTempSize += \
                            pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];                                    
                    }

                }
            }
		
		}

		pDwnldMemInfo->sFsBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;

		for(count = 0; count < BDSP_AudioType_eMax; count++)
		{
		    /* Check if decode algorithm is supported */
			if(!BDSP_Raaga_P_IsAudioTypeSupported(count))
			{
				continue;
			}
			BDSP_Raaga_P_GetExecIds ( 
                BDSP_DecodeMode_eDecode,
                (BDSP_AudioType)count,&sExecId);         

            /* Sum of sizes of Decode code+table Buf */
			for(i = BDSP_CIT_P_DECODE_NODE_INFO_INDEX; i<sExecId.NumNodes; i++)
			{
				/* Get size of Decode code */	
                codeExecId = sExecId.eAlgoIds[i];
                if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }          
    			eCodeFirmwareId = \
                            pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					break;
				}
                
                if(true == bIsExecAllocated[codeExecId])
                {
                    continue;
                }
                else
                {
                    bIsExecAllocated[codeExecId] = true;
                }
				uiTempSize += \
                    pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];


				/* Get size of Decode table */	

				eTblFirmwareId = \
				    pDevice->imgIdMappingArrays.tableImgId[codeExecId];
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				/*If the previous table is used for this stage, dont sum its size */
                            isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded ( 
                    BDSP_Raaga_P_ExecType_eDecode,
                    codeExecId,&isDownloaded,&eTempExecId);
                
				if(!isDownloaded)
				{
					uiTempSize += \
                        pDevice->imgIdMappingArrays.tableSize[codeExecId];
				}
			}
		}

		pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;

		/* Get size of  Passthru Code */	
		uiTempSize += \
		    pDevice->imgIdMappingArrays.ui32CodeSize[BDSP_AF_P_AlgoId_ePassThru];
        
		/* Get size of  Passthru table */	
		uiTempSize += \
		    pDevice->imgIdMappingArrays.tableSize[BDSP_AF_P_AlgoId_ePassThru];

		pDwnldMemInfo->sPtCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;

        /*Calculate Real Video Exec Size*/
        for(count = 0; count < BDSP_VideoType_eMax; count++)
		{                    		
            /* Check if decode algorithm is supported */
			if(!BDSP_Raaga_P_IsVideoTypeSupported(count))
			{
				continue;
			}                    					
			BDSP_Raaga_P_GetVideoExecIds(BDSP_DecodeMode_eDecode,(BDSP_VideoType)count,&sExecId);			                    		            
			uiTempSize = 0;					
            /* Sum of sizes of Decode code+table Buf */

			for(i = 0;i<sExecId.NumNodes;i++)
			{                    					
				/* Get size of Decode code */	
                codeExecId = sExecId.eAlgoIds[i];
                if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                                		                            
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}                      

				uiTempSize += pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];                       


				/* Get size of Decode table */	

				eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[codeExecId];
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}                
				/*If the previous table is used for this stage, dont sum its size */
                isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eVideoDecode,codeExecId,&isDownloaded,&eTempExecId);
				if(!isDownloaded)
				{
					uiTempSize += pDevice->imgIdMappingArrays.tableSize[codeExecId];                
				}
			}
			if(pDwnldMemInfo->sVideoDecCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sVideoDecCodecBufInfo[0].ui32Size = uiTempSize;
		}          
		pDwnldMemInfo->sVideoDecCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;          
		
        /*Calculate SCM Exec Sizes */
        for(count = 0; count < BDSP_ScmType_eMax; count++)
		{                    		
            /* Check if decode algorithm is supported */
			if(!BDSP_Raaga_P_IsScmTypeSupported(count))
			{
				continue;
			}                    					
			BDSP_Raaga_P_GetScmExecIds((BDSP_ScmType)count,&sExecId);			                    		            
			uiTempSize = 0;	
			
            /* Sum of sizes of SCM code+table Buf */
			for(i = 0;i<sExecId.NumNodes;i++)
			{                    					
				/* Get size of SCM code */	
                codeExecId = sExecId.eAlgoIds[i];
                if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                                		                            
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}                      

				uiTempSize += pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];                       


				/* Get size of SCM table */	
				eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[codeExecId];
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				
				/*If the previous table is used for this stage, dont sum its size */
                isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eScm,codeExecId,&isDownloaded,&eTempExecId);
				if(!isDownloaded)
				{
					uiTempSize += pDevice->imgIdMappingArrays.tableSize[codeExecId];                
				}
			}
			if(pDwnldMemInfo->sScmCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sScmCodecBufInfo[0].ui32Size = uiTempSize;
		}          
		pDwnldMemInfo->sScmCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;          
        /*Calculate Video Encode Exec Sizes */
        for(count = 0; count < BDSP_VideoEncode_eMax; count++)
		{                    		
            /* Check if decode algorithm is supported */
			if(!BDSP_Raaga_P_IsVideoEncodeTypeSupported(count))
			{
				continue;
			}
			BDSP_Raaga_P_GetVideoEncodeExecIds(BDSP_EncodeMode_eNonRealtime,(BDSP_VideoEncode)count,&sExecId);			                    		            
			uiTempSize = 0;	
			
            /* Sum of sizes of Encode code+table Buf */
			for(i = 0;i<sExecId.NumNodes;i++)
			{
				/* Get size of Decode code */	
                codeExecId = sExecId.eAlgoIds[i];
                if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                                		                            
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}                      
		
				uiTempSize += pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];                       


				/* Get size of Encode table */	
				eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[codeExecId];
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
		
				/*If the previous table is used for this stage, dont sum its size */
                isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eVideoEncode,codeExecId,&isDownloaded,&eTempExecId);
				if(!isDownloaded)
				{
					uiTempSize += pDevice->imgIdMappingArrays.tableSize[codeExecId];                
				}
			}
			if(pDwnldMemInfo->sVideoEncodeCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sVideoEncodeCodecBufInfo[0].ui32Size = uiTempSize;
		}          
		pDwnldMemInfo->sVideoEncodeCodecBufInfo[0].ui32Size = uiTempSize;
		*pui32Size += uiTempSize;
		uiTempSize = 0;          
	}
	else
	{
		
		/* BDSP_RAAGA_MAX_FS_CTXT buffers of size MAX(sizeof(Framesync exec))*/
		for(count = 0; count < BDSP_AudioType_eMax; count++)
		{
    		/* Check if decode algorithm is supported */
                    if(!(BDSP_Raaga_P_IsPassthruSupportedWithoutLicense(count)))
                    {
        			if(!BDSP_Raaga_P_IsAudioTypeSupported(count))
        			{
        				continue;
        			}
                    }
			BDSP_Raaga_P_GetExecIds (
                        BDSP_DecodeMode_eDecode,
                        (BDSP_AudioType)count,&sExecId);
            
		    /* Sum of sizes of Framesync Exec */
			/* For each stage framesync	*/	
                    codeExecId = sExecId.eAlgoIds[0];
                    if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }                
			eCodeFirmwareId = \
                pDevice->imgIdMappingArrays.codeImgId[codeExecId];

			if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				continue;
			}

			uiTempSize = \
                pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];

            if(pDwnldMemInfo->sFsBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sFsBufInfo[0].ui32Size = uiTempSize;
		}
		
		/* BDSP_RAAGA_MAX_DECODE_CTXT buffers of 
		    size MAX(sizeof(Decode Code+Table))*/	
		    
		for(count = 0; count < BDSP_AudioType_eMax; count++)
		{
		    /* Check if decode algorithm is supported */
			if(!BDSP_Raaga_P_IsAudioTypeSupported(count))
			{
				continue;
			}
			
			BDSP_Raaga_P_GetExecIds (
                BDSP_DecodeMode_eDecode,
                (BDSP_AudioType)count,&sExecId);          
            
			uiTempSize = 0;					
		    /* Sum of sizes of Decode code+table Buf */
			for(i = BDSP_CIT_P_DECODE_NODE_INFO_INDEX; i<sExecId.NumNodes; i++)
			{
				/* Get size of Decode code */	
                codeExecId = sExecId.eAlgoIds[i];
                if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                     
                eCodeFirmwareId = \
                    pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}

				uiTempSize += \
                    pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];

				/* Get size of Decode table */	
				eTblFirmwareId = \
				    pDevice->imgIdMappingArrays.tableImgId[codeExecId];
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
				/*If the previous table is used for this stage, dont sum its size */
                isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded (
                    BDSP_Raaga_P_ExecType_eDecode,
                    codeExecId,&isDownloaded,&eTempExecId);
                
				if(!isDownloaded)
				{
					uiTempSize += \
                        pDevice->imgIdMappingArrays.tableSize[codeExecId];
				}
			}

			if(pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sDecCodecBufInfo[0].ui32Size = uiTempSize;
		}			

		/* BDSP_RAAGA_MAX_PASSTHRU_CTXT buffers of size 
		    MAX(sizeof(Passthru Code +Table))*/	
        uiTempSize = 0;
        
		/* Get size of  Passthru Code */	
		uiTempSize += \
		    pDevice->imgIdMappingArrays.ui32CodeSize[BDSP_AF_P_AlgoId_ePassThru];

        /* Get size of  Passthru table */	
		uiTempSize += \
		    pDevice->imgIdMappingArrays.tableSize[BDSP_AF_P_AlgoId_ePassThru];

		pDwnldMemInfo->sPtCodecBufInfo[0].ui32Size = uiTempSize;		

		for(count = 0; count < BDSP_AudioEncode_eMax; count++)
		{
		    /* Check if audio processing algorithm is supported */
			if(!BDSP_Raaga_P_IsAudioEncodeSupported(count))
			{
				continue;
			}
			uiTempSize = 0;
            
			BDSP_Raaga_P_GetEncodeExecIds((BDSP_AudioEncode)count,&sExecId);

            codeExecId = sExecId.eAlgoIds[0];
            if(codeExecId != BDSP_AF_P_AlgoId_eInvalid)
            {
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[codeExecId];

                if(BDSP_Img_Id_eInvalid != eCodeFirmwareId)
                {	
                    uiTempSize += pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];
                }
            }

            /* For each stage of Encode algorithm	*/
			for(i = BDSP_CIT_P_ENCODE_NODE_INFO_INDEX; i<sExecId.NumNodes; i++)
			{
                codeExecId = sExecId.eAlgoIds[i];
                eCodeFirmwareId = \
                    pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}
                uiTempSize += \
                    pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];
            
                /* Get size of audio processing table */	
				eTblFirmwareId = \
				    pDevice->imgIdMappingArrays.tableImgId[codeExecId];
                
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}
                
				/* If the previous table is used for this stage, 
				    dont sum its size */
                isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded (
                    BDSP_Raaga_P_ExecType_eEncode,
                    codeExecId,&isDownloaded,&eTempExecId);
                
				if(!isDownloaded)
				{
					uiTempSize += \
                        pDevice->imgIdMappingArrays.tableSize[codeExecId];
				}
			}

			if(pDwnldMemInfo->sEncodeCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sEncodeCodecBufInfo[0].ui32Size = uiTempSize;
		}

        /* VIDEO*/
        for(count = 0; count < BDSP_VideoType_eMax; count++)
		{                    		
            /* Check if decode algorithm is supported */
			if(!BDSP_Raaga_P_IsVideoTypeSupported(count))
			{
				continue;
			}                    					
			BDSP_Raaga_P_GetVideoExecIds(BDSP_DecodeMode_eDecode,(BDSP_VideoType)count,&sExecId);			                    		            
			uiTempSize = 0;					
            /* Sum of sizes of Decode code+table Buf */

			for(i = 0;i<sExecId.NumNodes;i++)
			{                    					
				/* Get size of Decode code */	
                codeExecId = sExecId.eAlgoIds[i];
                if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                                		                            
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}                      

				uiTempSize += pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];                       


				/* Get size of Decode table */	

				eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[codeExecId];
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}                
				/*If the previous table is used for this stage, dont sum its size */
                isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eVideoDecode,codeExecId,&isDownloaded,&eTempExecId);
				if(!isDownloaded)
				{
					uiTempSize += pDevice->imgIdMappingArrays.tableSize[codeExecId];                
				}
			}
			if(pDwnldMemInfo->sVideoDecCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sVideoDecCodecBufInfo[0].ui32Size = uiTempSize;
		}
		for(count = 0; count < BDSP_ScmType_eMax; count++)
		{							
			/* Check if decode algorithm is supported */
			if(!BDSP_Raaga_P_IsScmTypeSupported(count))
			{
				continue;
			}										
			BDSP_Raaga_P_GetScmExecIds((BDSP_ScmType)count,&sExecId);													
			uiTempSize = 0;
			
			
			for(i = 0;i<sExecId.NumNodes;i++)
			{										
				/* Get size of Decode code */	
				codeExecId = sExecId.eAlgoIds[i];
				if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
				{
					continue;
				}																	
				eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[codeExecId];
		
				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}					   
		
				uiTempSize += pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId]; 					  
		
		
				/* Get size of Decode table */	
		
				eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[codeExecId];
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}				 
				/*If the previous table is used for this stage, dont sum its size */
				isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eScm,codeExecId,&isDownloaded,&eTempExecId);
				if(!isDownloaded)
				{
					uiTempSize += pDevice->imgIdMappingArrays.tableSize[codeExecId];				
				}
			}
			if(pDwnldMemInfo->sScmCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sScmCodecBufInfo[0].ui32Size = uiTempSize;
		}
        /* VIDEO Encode */
        for(count = 0; count < BDSP_VideoEncode_eMax; count++)
		{                    		
            /* Check if decode algorithm is supported */
			if(!BDSP_Raaga_P_IsVideoEncodeTypeSupported(count))
			{
				continue;
			}                    					
			BDSP_Raaga_P_GetVideoEncodeExecIds(BDSP_EncodeMode_eNonRealtime,(BDSP_VideoEncode)count,&sExecId);			                    		            
			uiTempSize = 0;
			
            /* Sum of sizes of Encode code+table Buf. copied from video decoder but How FS buffer is handled??  */
			for(i = 0;i<sExecId.NumNodes;i++)
			{                    					
				/* Get size of Decode code */	
                codeExecId = sExecId.eAlgoIds[i];
                if(codeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                                		                            
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[codeExecId];

				if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
				{
					continue;
				}                      

				uiTempSize += pDevice->imgIdMappingArrays.ui32CodeSize[codeExecId];                       


				/* Get size of Decode table */	

				eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[codeExecId];
				if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
				{
					continue;
				}                
				/*If the previous table is used for this stage, dont sum its size */
                isDownloaded =false;
				BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eVideoEncode,codeExecId,&isDownloaded,&eTempExecId);
				if(!isDownloaded)
				{
					uiTempSize += pDevice->imgIdMappingArrays.tableSize[codeExecId];                
				}
			}
			if(pDwnldMemInfo->sVideoEncodeCodecBufInfo[0].ui32Size < uiTempSize)
				pDwnldMemInfo->sVideoEncodeCodecBufInfo[0].ui32Size = uiTempSize;
		}
        
	}

	BDBG_LEAVE(BDSP_MM_P_GetDownloadMemRequirement);
	return BERR_SUCCESS;
}

BERR_Code BDSP_Raaga_P_FreeInitMemory(
    void *pDeviceHandle
    )
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    unsigned i=0,j=0;

    BERR_Code   err=BERR_SUCCESS;    

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);        

    if(true == pDevice->settings.preloadImages)
    {
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sOpenTimeMemInfo.ui32BaseAddr);
    }
    else
    {
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Addr );
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Addr );
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Addr );
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Addr);
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Addr);
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Addr);

		err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Addr);
		err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Addr);
		err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Addr);		

	}

    for(i=0;i<BDSP_RAAGA_MAX_DSP;i++)
    {           
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.cmdQueueParams[i].uiBaseAddr);         
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.genRspQueueParams[i].uiBaseAddr);           
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.pDSPFifoAddrStruct[i]);

		for (j = 0; j < BDSP_Raaga_DebugType_eLast; j++)
		{
			err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.pFwDebugBuf[i][j].ui32Addr);        
		}
		err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sRaagaSwapMemoryBuf[i].ui32Addr);
    }  

	/* Added to handle SW7346-598 */
	if(pDevice->sCitBuffers.psCitDataStructure)
	{
		BKNI_Free(pDevice->sCitBuffers.psCitDataStructure);
	}
	for(i=0; i < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
	{
		if(pDevice->sCitBuffers.psCitFwBranchInfo[i])
		{
			BKNI_Free(pDevice->sCitBuffers.psCitFwBranchInfo[i]);
		}
	}
	BKNI_Free(pDevice->sCitBuffers.psCitPortCfg);
	BKNI_Free(pDevice->sCitBuffers.psCitTaskIsBuffCfg);
return err;    
}


BERR_Code BDSP_Raaga_P_AllocateInitMemory (
    void *pDeviceHandle
    )
{
    BERR_Code err = BERR_SUCCESS;
    uint32_t ui32BaseAddr = 0,ui32UsedSize = 0, ui32TempAddr = 0, i=0, j=0;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_ENTER(BDSP_Raaga_P_AllocateInitMemory);

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);        

	/* Below memory is allocated once at device open time to handle SW7346-598 */
	pDevice->sCitBuffers.psCitDataStructure = (void *)BKNI_Malloc (sizeof( BDSP_CIT_P_sCitDataStructure ));

	if(NULL == pDevice->sCitBuffers.psCitDataStructure)
	{	
		BDBG_ERR(("ERROR: Unable to allocate memory for sCitDataStructure in function BDSP_Raaga_P_AllocateInitMemory "));
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		goto end;
	}
	for(i=0; i < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
	{
		pDevice->sCitBuffers.psCitFwBranchInfo[i] = (void *)BKNI_Malloc (sizeof(BDSP_CIT_P_BranchInfo));
		if(NULL == pDevice->sCitBuffers.psCitFwBranchInfo[i])
		{
			BDBG_ERR(("ERROR: Unable to allocate memory for FwBranchInfo in function BDSP_Raaga_P_AllocateInitMemory "));
			err = BERR_OUT_OF_SYSTEM_MEMORY;
			goto end;
		}
	}
	pDevice->sCitBuffers.psCitPortCfg		= (void *)BKNI_Malloc (sizeof( BDSP_CIT_P_sPortCfg ));
	pDevice->sCitBuffers.psCitTaskIsBuffCfg	= (void *)BKNI_Malloc (sizeof( BDSP_CIT_P_TaskIsBuffCfg )); 
	
	/* Get memory download requirement here */
    err = BDSP_MM_P_GetDownloadMemRequirement ( pDeviceHandle, 
            &(pDevice->memReq.sDwnldMemInfo), &pDevice->memReq.ui32OpenTimeDownloadSize);
    if(BERR_SUCCESS != err)
    {
        BDBG_ERR(("BDSP_MEM_P_CalcMemPoolReq: Error getting memory download requirements!"));
        err = BERR_TRACE(err);
        goto end;
    }    
    
    if(true == pDevice->settings.preloadImages)
    {
        /* OPEN TIME FIRMWARE DOWNLOAD */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory (
                                    pDeviceHandle, 
                                    pDevice->memReq.ui32OpenTimeDownloadSize,
                                    5, 0); /* 32 bit aligned*/
        
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                        "Unable to Allocate memory for open time download!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;            
        }
        
        BKNI_Memset ( (void *)ui32BaseAddr, 
                                0,pDevice->memReq.ui32OpenTimeDownloadSize);
        
        pDevice->memInfo.sOpenTimeMemInfo.ui32BaseAddr = ui32BaseAddr;
        pDevice->memInfo.sOpenTimeMemInfo.ui32Size = \
                                        pDevice->memReq.ui32OpenTimeDownloadSize;

        /* Fill up pointers */
        ui32TempAddr = ui32BaseAddr;
        ui32UsedSize = 0;

        BKNI_Memcpy((void *)(&(pDevice->memInfo.sDwnldMemInfo)),
                &(pDevice->memReq.sDwnldMemInfo),sizeof(BDSP_Raaga_P_DwnldMemInfo));      
        
        /* System Code */
        pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Size;

        /* System Data */
        pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Size;

        /* System RdbVars */
        pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Size;        

        /* System Lib */        
        pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Size;        

        /* Algo Lib */        
        pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Size;        

        /* Common Ids Code */    
        pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Size;                

        /* SCM Task Code */    
        pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Size;    

        /* SCM1 Digest */    
        pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Size; 

        /* SCM2 Digest*/    
        pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Addr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Size; 		

        /* Post Processing */        
        pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr = \
            ui32TempAddr;
        ui32TempAddr += \
            pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;
        ui32UsedSize += \
            pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;

        /* FS Buffers */                
        pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[0].ui32BaseAddr = ui32TempAddr;
        ui32TempAddr += pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[0].ui32Size;
        ui32UsedSize += pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[0].ui32Size;

        /* Decode */                
        pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32BaseAddr = \
            ui32TempAddr;
        ui32TempAddr += \
            pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;
        ui32UsedSize += \
            pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;

        /* Passthru */                
        pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32BaseAddr = \
            ui32TempAddr;
        ui32TempAddr += \
            pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;
        ui32UsedSize += \
            pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;
        
        /* Encode */
        pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32BaseAddr = \
            ui32TempAddr;
        
        ui32TempAddr += \
            pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;
        
        ui32UsedSize += \
            pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;
        
        /* Video */
        pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32BaseAddr = \
            ui32TempAddr;
        
        ui32TempAddr += \
            pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;
        
        ui32UsedSize += \
            pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;
		/* SCM */
        pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[0].ui32BaseAddr = \
            ui32TempAddr;
        
        ui32TempAddr += \
            pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[0].ui32Size;
        
        ui32UsedSize += \
            pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[0].ui32Size;

        /* Video Encode */
        pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32BaseAddr = \
            ui32TempAddr;
        
        ui32TempAddr += \
            pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32Size;
        
        ui32UsedSize += \
            pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32Size;

        
		BKNI_Memcpy ( (void *)(&(pDevice->memInfo.sDwnldMemInfo)),
                &(pDevice->memInfo.sDwnldMemInfo),sizeof(BDSP_Raaga_P_DwnldMemInfo));     
    }
    else
    {
        /* START TIME FIRMWARE DOWNLOAD */   
        
		BKNI_Memcpy ( (void *)(&(pDevice->memInfo.sDwnldMemInfo)), 
                &(pDevice->memReq.sDwnldMemInfo),sizeof(BDSP_Raaga_P_DwnldMemInfo));      
        
        /* SystemCode */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sSystemCodeMem.ui32Size,2, 0);

        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                "Unable to Allocate memory for SystemCode!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Addr = ui32BaseAddr;

        /* SystemData */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sSystemDataMem.ui32Size,2, 0);
        
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                "Unable to Allocate memory for SystemData!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 

        }
        pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Addr = ui32BaseAddr;

        /* SystemRdbBVars */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Size,2, 0);
        
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                "Unable to Allocate memory for SystemRDBVars!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Addr = ui32BaseAddr;

        /* SyslibCode */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sSystemLibMem.ui32Size,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                        "Unable to Allocate memory for SyslibCode!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 

        }
        pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Addr = ui32BaseAddr;

        /* AlgolibCode */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sAlgoLibMem.ui32Size,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                        "Unable to Allocate memory for AlgolibData!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Addr = ui32BaseAddr;
        
        /* CommonIdsCode */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sCommonIdsCodeMem.ui32Size,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                        "Unable to Allocate memory for CommonIdsCode!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Addr = ui32BaseAddr;        

	/* SCM Task Code */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sScmTaskCodeMem.ui32Size,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                        "Unable to Allocate memory for scm task code!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Addr = ui32BaseAddr;      

        /* SCM1 Digest */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sScm1DigestMem.ui32Size,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                        "Unable to Allocate memory for scm1 digest!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Addr = ui32BaseAddr; 

        /* SCM2 Digest */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory (pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sScm2DigestMem.ui32Size,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_MM_AllocateMemory: "
                        "Unable to Allocate memory for scm2 digest!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Addr = ui32BaseAddr;

    }


       /* Allocate Task Cmd/Resp Queues */
    for(i=0;i<BDSP_RAAGA_MAX_DSP;i++)
    {
        /* Allocate Command queue */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, (BDSP_RAAGA_MAX_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_Command)*BDSP_RAAGA_MAX_FW_TASK_PER_DSP),2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_DSP_AllocMem: Unable to Allocate memory for cmd queue!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pDevice->memInfo.cmdQueueParams[i].uiBaseAddr = ui32BaseAddr;
        pDevice->memInfo.cmdQueueParams[i].uiMsgQueueSize = BDSP_RAAGA_MAX_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_Command)*BDSP_RAAGA_MAX_FW_TASK_PER_DSP;
        /*TODO : Correct the calculation for DSP1*/
        pDevice->memInfo.cmdQueueParams[i].ui32FifoId = BDSP_RAAGA_FIFO_CMD;

        /* Allocate Generic Response queue */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, (BDSP_RAAGA_MAX_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_Response)),2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_DSP_AllocMem: Unable to Allocate memory for generic response queue!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pDevice->memInfo.genRspQueueParams[i].uiBaseAddr = ui32BaseAddr;
        pDevice->memInfo.genRspQueueParams[i].uiMsgQueueSize = BDSP_RAAGA_MAX_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_Response);
        /*TODO : Correct the calculation for DSP1*/
        pDevice->memInfo.genRspQueueParams[i].ui32FifoId = BDSP_RAAGA_FIFO_GENERIC_RSP;

    /*  Creating structure for firmware which will contain physical addresses of all 
        18 fifo registers in the order base,end,read,write,wrap. Wrap will also be 
        programmed with fifo end address. Base address of this will be programmed in
        BCHP_AUD_DSP_CFG0_CONTROL_REGISTER0_CXT0 before bootup */
        
        pDevice->memInfo.pDSPFifoAddrStruct[i] = BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                    BDSP_RAAGA_NUM_FIFOS * BDSP_RAAGA_NUM_PTR_PER_FIFO * 4,2, 0);	/* 32 bit aligned*/
        if( NULL == pDevice->memInfo.pDSPFifoAddrStruct[i])
        {
            BDBG_ERR(("BDSP_Raaga_P_Open: Unable to Allocate memory for FIFO Register addresses!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }   

		for (j = 0; j <	BDSP_Raaga_DebugType_eLast; j++)
		{
            void *pCached;
	        if(pDevice->settings.debugSettings[j].enabled == true)
	        {
	            pDevice->memInfo.pFwDebugBuf[i][j].ui32Size = pDevice->settings.debugSettings[j].bufferSize;
	        }
	        else
	        {
	            pDevice->memInfo.pFwDebugBuf[i][j].ui32Size = 4;
	        }
	         pDevice->memInfo.pFwDebugBuf[i][j].ui32Addr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, pDevice->memInfo.pFwDebugBuf[i][j].ui32Size,2, 0);   /* 32 bit aligned*/
	        if( NULL == (void *)pDevice->memInfo.pFwDebugBuf[i][j].ui32Addr)
	        {
	            BDBG_ERR(("BDSP_RAAGA_Open: Unable to Allocate memory for debug buffer!"));
	            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	            goto end; 
	        }

            err = BMEM_Heap_ConvertAddressToCached(pDevice->memHandle, (void *)pDevice->memInfo.pFwDebugBuf[i][j].ui32Addr, &pCached);
            if ( err )
            {
                err = BERR_TRACE(err);
                goto end;
            }
	        BKNI_Memset(pCached,0,pDevice->memInfo.pFwDebugBuf[i][j].ui32Size);
            (void)BMEM_Heap_FlushCache(pDevice->memHandle, pCached, pDevice->memInfo.pFwDebugBuf[i][j].ui32Size);
		}

		/* Need to allocate raaga system swap memory. Raaga system need a memory in DRAM to swap its data memory contents */
		pDevice->memInfo.sRaagaSwapMemoryBuf[i].ui32Size = BDSP_P_FW_SYSTEM_SWAP_MEMORY_SIZE;
		pDevice->memInfo.sRaagaSwapMemoryBuf[i].ui32Addr = 
					(uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, pDevice->memInfo.sRaagaSwapMemoryBuf[i].ui32Size, 2, 0);   /* 32 bit aligned*/
		
		if( NULL == (void *)pDevice->memInfo.sRaagaSwapMemoryBuf[i].ui32Addr)
		{
			BDBG_ERR(("BDSP_RAAGA_Open: Unable to Allocate memory for raaga system swapping!"));
			err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			goto end; 
		}
    }   

    BDBG_LEAVE(BDSP_Raaga_P_AllocateInitMemory);
end:
    if(err != BERR_SUCCESS)
    {
         BDSP_Raaga_P_FreeInitMemory(pDeviceHandle);
    }
    return err;
}

static BERR_Code BDSP_MM_P_CalcMemPoolReq(void *pContextHandle)
{
    /*
        Call BDSP_DSP_DownloadMemReq to get download memory requiremnts.
        --
        For (All Processing Algos Supported)
        NOTE: This can be further optimized by allocing combined sizes of 
        BDSP_MAX_FW_PROCESSING_SUPPORTED & providing memory from there but 
        that will enforce that all PAlgos are downloaded at open time only. 
        Same logic applies for IFrameBufs & ConfigParams 
        {
            Find max Processing Algo size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32PAlgo
        --
        For(BDSP_MAX_FW_TASK_SUPPORTED)  Based on usage mode 
            For (All Codecs Supported)
            {
                Find max codec scratch size required
            }
            Put in TempScratchSize
            For (All Processing Algos Supported)
            {
                Add up Scratch Size required
            }
            Add that to TempScratchSize
            Fill psDspMem->sDspMemSizeReq[uiDspId].ui32Scratch with TempScratchSize
        --
        For(BDSP_MAX_FW_TASK_SUPPORTED)  Based on usage mode 
            For (All Codecs Supported)
            {
                Find max codec IStage size required
            }
            Put in TempIStageSize
            For (All Processing Algos Supported)
            {
                Add up IStage Size required
            }
            Add that to TempIStageSize
            Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IStage with TempIStageSize
        --
        For (All Codecs Supported)
        {
            Find max codec Iframe size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IFrameCommon
        --
        For (All Processing Algos Supported)
        {
            Find max Processing Algo Iframe size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32IFramePAlgo
        --
        For (All Codecs Supported)
        {
            Find max codec ConfigParams size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32CfgBufCommon
        --
        For (All Processing Algos Supported)
        {
            Find max Processing Algo ConfigParams size required
        }
        Fill psDspMem->sDspMemSizeReq[uiDspId].ui32CfgBufPAlgo
        
    */
    BERR_Code err = BERR_SUCCESS;
    uint32_t ui32DecScratch = 0, ui32DecIs = 0, ui32DecIsIf = 0, ui32DecIf = 0, ui32DecCfgBuf = 0, ui32DecStatusBuf = 0;
    uint32_t ui32EncScratch = 0, ui32EncIs = 0, ui32EncIsIf = 0, ui32EncIf = 0, ui32EncCfgBuf = 0, ui32EncStatusbuf = 0;
    uint32_t ui32PaScratch = 0, ui32PaIs = 0, ui32PaIsIf = 0, ui32PaIf = 0, ui32PaCfgBuf = 0, ui32PaStatusBuf = 0;
    uint32_t ui32TempScratch = 0, ui32TempIs = 0, ui32TempIsIf = 0, ui32TempIf = 0, ui32TempCfgBuf = 0, ui32TempStatusBuf =0,ui32NumCh=0;
    unsigned int i = 0, j = 0, uiNumNodes = 0;
    
    BDSP_RaagaContext   *pRaagaContext = (BDSP_RaagaContext   *)pContextHandle;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pRaagaContext->pDevice;    
    
    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    

    BDBG_ENTER(BDSP_MM_P_CalcMemPoolReq);

    /* Calculate scratch & interstage & interstage interface memory requirements */
    /* For Decoders */
    for(i=0; i<BDSP_AudioType_eMax; i++)
    {
	 if(!(BDSP_Raaga_P_IsAudioTypeSupported(i)))
	 	continue;
        uiNumNodes = BDSP_sDecAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf =0;
        for(j=0; (j<uiNumNodes)&&(j < BDSP_AF_P_MAX_NUM_NODES_IN_ALGO) ; j++)
        {
        	if( BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j] < BDSP_AF_P_AlgoId_eMax )
        {
            ui32TempScratch += BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32ScratchBuffSize;

            if(ui32TempIs < BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan)
                ui32TempIs = BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan;
            if(ui32NumCh < BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported)
                ui32NumCh = BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported;            
            
            ui32TempIsIf += BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
            ui32TempIf += pDevice->imgIdMappingArrays.interframeSize[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]];
/*                BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize;*/
            ui32TempCfgBuf += BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32UserCfgBuffSize;
            ui32TempStatusBuf += BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32StatusBuffSize;
        }
        }
        if(ui32TempScratch >= ui32DecScratch)
            ui32DecScratch = ui32TempScratch;
        if(ui32TempIs >= ui32DecIs)
            ui32DecIs = ui32TempIs;
        if(ui32TempIsIf >= ui32DecIsIf)
            ui32DecIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32DecIf)
            ui32DecIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32DecCfgBuf)
            ui32DecCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32DecStatusBuf)
            ui32DecStatusBuf = ui32TempStatusBuf;
    }

    for(i=0; i<BDSP_VideoType_eMax; i++)
    {
        if(!(BDSP_Raaga_P_IsVideoTypeSupported(i)))
        	continue;
        uiNumNodes = BDSP_sVideoDecAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf =0;
        
        for(j=0; (j<uiNumNodes)&&(j < BDSP_AF_P_MAX_NUM_NODES_IN_ALGO) ; j++)
        {
        	if( BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j] < BDSP_AF_P_AlgoId_eMax )
            {
                ui32TempScratch += BDSP_sNodeInfo[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32ScratchBuffSize;

                if(ui32TempIs < BDSP_sNodeInfo[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan)
                    ui32TempIs = BDSP_sNodeInfo[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan;
                if(ui32NumCh < BDSP_sNodeInfo[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported)
                    ui32NumCh = BDSP_sNodeInfo[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported;            
                
                ui32TempIsIf += BDSP_sNodeInfo[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
                ui32TempIf += pDevice->imgIdMappingArrays.interframeSize[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]];
            /*                BDSP_sNodeInfo[BDSP_sDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize;*/
                ui32TempCfgBuf += BDSP_sNodeInfo[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32UserCfgBuffSize;
                ui32TempStatusBuf += BDSP_sNodeInfo[BDSP_sVideoDecAlgoExecInfo[i][0].eAlgoIds[j]].ui32StatusBuffSize;
            }
        }
        if(ui32TempScratch >= ui32DecScratch)
            ui32DecScratch = ui32TempScratch;
        if(ui32TempIs >= ui32DecIs)
            ui32DecIs = ui32TempIs;
        if(ui32TempIsIf >= ui32DecIsIf)
            ui32DecIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32DecIf)
            ui32DecIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32DecCfgBuf)
            ui32DecCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32DecStatusBuf)
            ui32DecStatusBuf = ui32TempStatusBuf;
    }
    
    /* For Encoders */
    for(i=0; i<BDSP_AudioEncode_eMax; i++)
    {
	 if(!(BDSP_Raaga_P_IsAudioEncodeSupported(i)))
	 	continue;    
        uiNumNodes = BDSP_sEncAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf = 0;
        for(j=0; j<uiNumNodes; j++)
        {
             if( BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j] < BDSP_AF_P_AlgoId_eMax )
            {
            ui32TempScratch += BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32ScratchBuffSize;

            if(ui32TempIs < BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan)
                ui32TempIs = BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan;
            if(ui32NumCh < BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported)
                ui32NumCh = BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported;

            ui32TempIsIf += BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
            ui32TempIf += pDevice->imgIdMappingArrays.interframeSize[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]];
/*                BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize;*/
            ui32TempCfgBuf += BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32UserCfgBuffSize;
            ui32TempStatusBuf += BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32StatusBuffSize;
        }
        }
        if(ui32TempScratch >= ui32EncScratch)
            ui32EncScratch = ui32TempScratch;
        if(ui32TempIs >= ui32EncIs)
            ui32EncIs = ui32TempIs;
        if(ui32TempIsIf >= ui32EncIsIf)
            ui32EncIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32EncIf)
            ui32EncIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32EncCfgBuf)
            ui32EncCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32EncStatusbuf)
            ui32EncStatusbuf = ui32TempStatusBuf;
    }
	/* Calculate for Video Encoder */
	for(i=0; i<BDSP_VideoEncode_eMax; i++)
    {
	 	if(!(BDSP_Raaga_P_IsVideoEncodeTypeSupported(i)))
	 		continue;    
        uiNumNodes = BDSP_sVideoEncAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf = 0;
        for(j=0; j<uiNumNodes; j++)
        {
             if( BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j] < BDSP_AF_P_AlgoId_eMax )
            {
            ui32TempScratch += BDSP_sNodeInfo[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32ScratchBuffSize;

            if(ui32TempIs < BDSP_sNodeInfo[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan)
                ui32TempIs = BDSP_sNodeInfo[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan;
            if(ui32NumCh < BDSP_sNodeInfo[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported)
                ui32NumCh = BDSP_sNodeInfo[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported;

            ui32TempIsIf += BDSP_sNodeInfo[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
            ui32TempIf += pDevice->imgIdMappingArrays.interframeSize[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]];
			/* BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize; */
            ui32TempCfgBuf += BDSP_sNodeInfo[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32UserCfgBuffSize;
            ui32TempStatusBuf += BDSP_sNodeInfo[BDSP_sVideoEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32StatusBuffSize;
        }
        }
        if(ui32TempScratch >= ui32EncScratch)
            ui32EncScratch = ui32TempScratch;
        if(ui32TempIs >= ui32EncIs)
            ui32EncIs = ui32TempIs;
        if(ui32TempIsIf >= ui32EncIsIf)
            ui32EncIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32EncIf)
            ui32EncIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32EncCfgBuf)
            ui32EncCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32EncStatusbuf)
            ui32EncStatusbuf = ui32TempStatusBuf;
    }
    /* For Processing Algos */
    for(i=0; i<BDSP_AudioProcessing_eMax; i++)
    {
	 if(!(BDSP_Raaga_IsAudioProcessingSupported(i)))
	 	continue;        
        uiNumNodes = BDSP_sPpAlgoExecInfo[i][0].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf = 0;
        for(j=0; j<uiNumNodes; j++)
        {
             if( BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j] < BDSP_AF_P_AlgoId_eMax )
            {        
            ui32TempScratch += BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32ScratchBuffSize;
            if(ui32TempIs < BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan)
                ui32TempIs = BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxSizePerChan;
            
            if(ui32NumCh < BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported)
                ui32NumCh = BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32MaxNumChansSupported;

            ui32TempIsIf += BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
            ui32TempIf += pDevice->imgIdMappingArrays.interframeSize[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]];
             /*   BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize;*/
            ui32TempCfgBuf += BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32UserCfgBuffSize;
            ui32TempStatusBuf += BDSP_sNodeInfo[BDSP_sPpAlgoExecInfo[i][0].eAlgoIds[j]].ui32StatusBuffSize;
            }
        }
        if(ui32TempScratch >= ui32PaScratch)
            ui32PaScratch = ui32TempScratch;
        if(ui32TempIs >= ui32PaIs)
            ui32PaIs = ui32TempIs;
        if(ui32TempIsIf >= ui32PaIsIf)
            ui32PaIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32PaIf)
            ui32PaIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32PaCfgBuf)
            ui32PaCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32PaStatusBuf)
            ui32PaStatusBuf = ui32TempStatusBuf;
    }
	/* Calculate for SCM */
	for(i=0; i<BDSP_ScmType_eMax; i++)
    {
	 	if(!(BDSP_Raaga_P_IsScmTypeSupported(i)))
	 		continue;    
        uiNumNodes = BDSP_sScmAlgoExecInfo[i].NumNodes; 
        ui32TempScratch = 0;
        ui32TempIs = 0;
        ui32TempIsIf = 0;
        ui32TempIf = 0;
        ui32TempCfgBuf = 0;
        ui32TempStatusBuf = 0;
        for(j=0; j<uiNumNodes; j++)
        {
             if( BDSP_sScmAlgoExecInfo[i].eAlgoIds[j] < BDSP_AF_P_AlgoId_eMax )
            {
            ui32TempScratch += BDSP_sNodeInfo[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]].ui32ScratchBuffSize;

            if(ui32TempIs < BDSP_sNodeInfo[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]].ui32MaxSizePerChan)
                ui32TempIs = BDSP_sNodeInfo[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]].ui32MaxSizePerChan;
            if(ui32NumCh < BDSP_sNodeInfo[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]].ui32MaxNumChansSupported)
                ui32NumCh = BDSP_sNodeInfo[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]].ui32MaxNumChansSupported;

            ui32TempIsIf += BDSP_sNodeInfo[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]].ui32InterStgGenericBuffSize;
            ui32TempIf += pDevice->imgIdMappingArrays.interframeSize[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]];
			/* BDSP_sNodeInfo[BDSP_sEncAlgoExecInfo[i][0].eAlgoIds[j]].ui32InterFrmBuffSize; */
            ui32TempCfgBuf += BDSP_sNodeInfo[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]].ui32UserCfgBuffSize;
            ui32TempStatusBuf += BDSP_sNodeInfo[BDSP_sScmAlgoExecInfo[0].eAlgoIds[j]].ui32StatusBuffSize;
        }
        }
        if(ui32TempScratch >= ui32EncScratch)
            ui32EncScratch = ui32TempScratch;
        if(ui32TempIs >= ui32EncIs)
            ui32EncIs = ui32TempIs;
        if(ui32TempIsIf >= ui32EncIsIf)
            ui32EncIsIf = ui32TempIsIf;
        if(ui32TempIf >= ui32EncIf)
            ui32EncIf = ui32TempIf;
        if(ui32TempCfgBuf >= ui32EncCfgBuf)
            ui32EncCfgBuf = ui32TempCfgBuf;
        if(ui32TempStatusBuf >= ui32EncStatusbuf)
            ui32EncStatusbuf = ui32TempStatusBuf;
    }
    ui32TempScratch = ui32DecScratch;
    if(ui32EncScratch >= ui32TempScratch)
        ui32TempScratch = ui32EncScratch;
    if(ui32PaScratch >= ui32TempScratch)
        ui32TempScratch = ui32PaScratch;    
    ui32TempIs = ui32DecIs;
    if(ui32EncIs >= ui32TempIs)
        ui32TempIs = ui32EncIs;
    if(ui32PaIs >= ui32TempIs)
        ui32TempIs = ui32PaIs;

    ui32TempIs = ui32TempIs*ui32NumCh*(pRaagaContext->settings.maxBranch+1);
    ui32TempIsIf = ui32DecIsIf;
    if(ui32EncIsIf >= ui32TempIsIf)
        ui32TempIsIf = ui32EncIsIf;
    
    if(ui32PaIsIf >= ui32TempIsIf)
        ui32TempIsIf = ui32PaIsIf;    

    ui32TempIf = ui32DecIf + ui32EncIf;
    ui32TempCfgBuf = ui32DecCfgBuf + ui32EncCfgBuf;


    ui32TempStatusBuf = ui32DecStatusBuf + ui32EncStatusbuf;

    for(i=0; i<BDSP_RAAGA_MAX_DSP; i++)
    {
        BDBG_MSG(("ui32TempScratch = %d , ui32TempIs = %d ui32TempIsIf =%d",ui32TempScratch,ui32TempIs,ui32TempIsIf));
        pDevice->memReq.ui32DspScratchMemReq[i] = (ui32TempScratch + ui32TempIs + ui32TempIsIf);
        pDevice->memReq.sTaskMemReq[i].ui32IframeCommon = ui32TempIf;
        pDevice->memReq.sTaskMemReq[i].ui32CfgBufCommon = ui32TempCfgBuf;
        pDevice->memReq.sTaskMemReq[i].ui32StatusBufCommon = ui32TempStatusBuf;        
        pDevice->memReq.sTaskMemReq[i].ui32IFramePAlgo = ui32PaIf;
        pDevice->memReq.sTaskMemReq[i].ui32CfgBufPAlgo = ui32PaCfgBuf;
        pDevice->memReq.sTaskMemReq[i].ui32StatusBufPAlgo = ui32PaStatusBuf;
        pDevice->memReq.sTaskMemReq[i].ui32CBitBuf = BDSP_CIT_P_TASK_SPDIF_USER_CFG_MEM_SIZE;
        pDevice->memReq.sTaskMemReq[i].ui32ExtraBuf = BDSP_CIT_P_TOTAL_TASK_CFG_MEMORY  /*BDSP_CIT_P_TASK_SWAP_BUFFER_SIZE_INBYTES + BDSP_CIT_P_TASK_PORT_CONFIG_MEM_SIZE + BDSP_CIT_P_TASK_FMM_GATE_OPEN_CONFIG*/;
    }

    if(ui32TempCfgBuf< sizeof(BDSP_AF_P_sFMM_DEST_CFG))
    {
        pDevice->memReq.ui32ConfigBufReq = sizeof(BDSP_AF_P_sFMM_DEST_CFG);        
    }
    else
    {
        pDevice->memReq.ui32ConfigBufReq = ui32TempCfgBuf;
    }

    BDBG_LEAVE(BDSP_MM_P_CalcMemPoolReq);
    return err;
}


BERR_Code BDSP_Raaga_P_FreeTaskMemory(
    void *pTaskHandle
    )    
{
    BERR_Code   err=BERR_SUCCESS;    
    BDSP_RaagaTask   *pRaagaTask = (BDSP_RaagaTask   *)pTaskHandle;    
    BDSP_RaagaContext   *pRaagaContext = (BDSP_RaagaContext   *)pRaagaTask->pContext;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pRaagaContext->pDevice;    

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);        
            
    err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32BaseAddr);
    err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sTaskQueue.sTaskSyncQueue.uiBaseAddr);
    err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sTaskQueue.sTaskAsyncQueue.uiBaseAddr);
    if ( pRaagaTask->taskMemGrants.sTaskQueue.sAsyncMsgBufmem.ui32BaseAddr )
    {
        BKNI_Free((void *)pRaagaTask->taskMemGrants.sTaskQueue.sAsyncMsgBufmem.ui32BaseAddr);
    }

    if(pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sTaskQueue.sPDQueue.uiBaseAddr);         
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sTaskQueue.sPRQueue.uiBaseAddr);
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sTaskQueue.sDSQueue.uiBaseAddr);          
    }
    else if(BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType )
    {
	    err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr);
#if 0 /* This is not DRAM memory now but RDB address */		
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sTaskQueue.sRDQueue.uiBaseAddr);         
#endif 
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sTaskQueue.sRRQueue.uiBaseAddr);
    }
    
    err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *) pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr);
    err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr);
    
    if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
    {                
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr);                
    }                
	if(BDSP_ContextType_eScm== pRaagaContext->settings.contextType )
	{
		err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[0].ui32BaseAddr);
	}
    return err;    
}

BERR_Code BDSP_Raaga_P_FreeContextMemory(
    void *pContextHandle
    )
{
    BDSP_RaagaContext   *pRaagaContext = (BDSP_RaagaContext   *)pContextHandle;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pRaagaContext->pDevice;
    unsigned i=0;
    BERR_Code   err=BERR_SUCCESS;    

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);        

    if(false == pDevice->settings.preloadImages)
    {
        if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
        {    
            err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr);
            err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[0].ui32BaseAddr);
            err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32BaseAddr);
            err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32BaseAddr);
            err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32BaseAddr);
        }
        else if(pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
        {
            err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32BaseAddr);        
        }
		else if(BDSP_ContextType_eVideoEncode== pRaagaContext->settings.contextType )
		{
			err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32BaseAddr);
		}
    }

    for(i=0;i<BDSP_RAAGA_MAX_DSP;i++)
    {            
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaContext->contextMemInfo.sDspScratchInfo[i].ui32BaseAddr);        
    }  
    err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32BaseAddr);
#if 0    
    if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
    {        
        err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaContext->contextMemInfo.sSpdifStatusBitBufInfo.ui32BaseAddr );       
    }
#endif    
    err  = BDSP_Raaga_P_FreeMemory((void *)pDevice,(void *)pRaagaContext->contextMemInfo.sVomTableInfo.ui32BaseAddr);  
return err;    
}


BERR_Code BDSP_Raaga_P_AllocateTaskMemory(
    void *pTaskHandle
    )
{
    BERR_Code err = BERR_SUCCESS;
    uint32_t ui32BaseAddr = 0,ui32UsedSize = 0;
    unsigned int i = 0;
    BDSP_RaagaTask   *pRaagaTask = (BDSP_RaagaTask   *)pTaskHandle;    
    BDSP_RaagaContext   *pRaagaContext = (BDSP_RaagaContext   *)pRaagaTask->pContext;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pRaagaContext->pDevice;

    BDBG_ENTER(BDSP_Raaga_P_AllocateContextMemory);

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);        

    err = BDSP_MM_P_CalcMemPoolReq ((void *)pRaagaContext);
    if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: "
                    "Unable to get memory requirements for DSP!"));
        err = BERR_TRACE(err);
        goto end;
    }

    ui32UsedSize = pDevice->memReq.sTaskMemReq[i].ui32IframeCommon +
        pDevice->memReq.sTaskMemReq[i].ui32CfgBufCommon +
        pDevice->memReq.sTaskMemReq[i].ui32IFramePAlgo * 
            pRaagaContext->settings.maxBranch * (pRaagaContext->settings.maxStagePerBranch-1) +
        pDevice->memReq.sTaskMemReq[i].ui32CfgBufPAlgo * 
            pRaagaContext->settings.maxBranch * (pRaagaContext->settings.maxStagePerBranch -1) + 
        pRaagaContext->settings.maxBranch * BDSP_CIT_P_SIZEOF_IOBUFFCFG *
            BDSP_AF_P_MAX_IP_FORKS+
        pDevice->memReq.sTaskMemReq[i].ui32StatusBufCommon +
       pDevice->memReq.sTaskMemReq[i].ui32StatusBufPAlgo*
           pRaagaContext->settings.maxBranch * (pRaagaContext->settings.maxStagePerBranch -1)
           +  pDevice->memReq.sTaskMemReq[i].ui32CBitBuf
           +  pDevice->memReq.sTaskMemReq[i].ui32ExtraBuf;     

    ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, ui32UsedSize,2, 0);  /* 32 bit aligned*/
    if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for IFrameCfgBuf!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;        
    }
    else
    {
        pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32BaseAddr = ui32BaseAddr;
        pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32Size = ui32UsedSize;
    }

    /* Sync Queue */
    ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, (BDSP_RAAGA_MAX_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_Response)),2, 0);
    if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for sync task queue!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;        
    }
    pRaagaTask->taskMemGrants.sTaskQueue.sTaskSyncQueue.uiBaseAddr = ui32BaseAddr;
    pRaagaTask->taskMemGrants.sTaskQueue.sTaskSyncQueue.uiMsgQueueSize = BDSP_RAAGA_MAX_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_Response);
    pRaagaTask->taskMemGrants.sTaskQueue.sTaskSyncQueue.ui32FifoId = BDSP_RAAGA_FIFO_INVALID;

    /* Async Queue */
    ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, (BDSP_RAAGA_MAX_ASYNC_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_AsynEventMsg)),2, 0);
    if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for async task queue!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;        
    }
    pRaagaTask->taskMemGrants.sTaskQueue.sTaskAsyncQueue.uiBaseAddr = ui32BaseAddr;
    pRaagaTask->taskMemGrants.sTaskQueue.sTaskAsyncQueue.uiMsgQueueSize = BDSP_RAAGA_MAX_ASYNC_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_AsynEventMsg);
    pRaagaTask->taskMemGrants.sTaskQueue.sTaskAsyncQueue.ui32FifoId = BDSP_RAAGA_FIFO_INVALID;

    /* Async MSG Buf Memory */
    ui32BaseAddr = (uint32_t)BKNI_Malloc((BDSP_RAAGA_MAX_ASYNC_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_AsynEventMsg)));
    if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for async msgs!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;        
    }
    pRaagaTask->taskMemGrants.sTaskQueue.sAsyncMsgBufmem.ui32BaseAddr = ui32BaseAddr;
    pRaagaTask->taskMemGrants.sTaskQueue.sAsyncMsgBufmem.ui32Size = BDSP_RAAGA_MAX_ASYNC_MSGS_PER_QUEUE * sizeof(BDSP_Raaga_P_AsynEventMsg);


    if(pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
    
		/* Cit memory. This is now context dependent as the sizes changes for different context */
		ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, sizeof(BDSP_VF_P_sDEC_TASK_CONFIG),2, 0);
		if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
		{
			BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for cit!"));
			err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			goto end;		 
		}
		pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr = ui32BaseAddr;
		pRaagaTask->taskMemGrants.sCitStruct.ui32Size = sizeof(BDSP_VF_P_sDEC_TASK_CONFIG);
		
        /* Allocate Picture delivery queue */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE*4,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_DSP_AllocMem: Unable to Allocate memory for Picture delivery queue!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pRaagaTask->taskMemGrants.sTaskQueue.sPDQueue.uiBaseAddr = ui32BaseAddr;
        pRaagaTask->taskMemGrants.sTaskQueue.sPDQueue.uiMsgQueueSize = BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE * 4;
        /*TODO : Correct the calculation for DSP1*/
        pRaagaTask->taskMemGrants.sTaskQueue.sPDQueue.ui32FifoId = (2*i) + BDSP_RAAGA_DSP_P_FIFO_PDQ;

        /* Allocate Picture release queue */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE*4,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_DSP_AllocMem: Unable to Allocate memory for Picture Release queue!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pRaagaTask->taskMemGrants.sTaskQueue.sPRQueue.uiBaseAddr = ui32BaseAddr;
        pRaagaTask->taskMemGrants.sTaskQueue.sPRQueue.uiMsgQueueSize = BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE * 4;
        /*TODO : Correct the calculation for DSP1*/
        pRaagaTask->taskMemGrants.sTaskQueue.sPRQueue.ui32FifoId = (2*i) + BDSP_RAAGA_DSP_P_FIFO_PRQ;

        /* Allocate Display  queue */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE*4,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_DSP_AllocMem: Unable to Allocate memory for DS queue!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pRaagaTask->taskMemGrants.sTaskQueue.sDSQueue.uiBaseAddr = ui32BaseAddr;
        pRaagaTask->taskMemGrants.sTaskQueue.sDSQueue.uiMsgQueueSize = BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE * 4;
        /*TODO : Correct the calculation for DSP1*/
        pRaagaTask->taskMemGrants.sTaskQueue.sDSQueue.ui32FifoId = (2*i) + BDSP_RAAGA_DSP_P_FIFO_DSQ;
    }
	else if(BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType )
    {
    
		/* Cit memory. This is now context dependent as the sizes changes for different context */
		ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, sizeof(BDSP_VF_P_sENC_TASK_CONFIG),2, 0);
		if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
		{
			BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for cit!"));
			err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			goto end;		 
		}
		pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr = ui32BaseAddr;
		pRaagaTask->taskMemGrants.sCitStruct.ui32Size = sizeof(BDSP_VF_P_sENC_TASK_CONFIG);
		
		/* Video Encoder also needs user config params for framesync */
        /* Allocate buffer required for on-the-fly programming of TSM 
            configuration parameters for each task. TSM is now part of 
            Framesync and this memory is allocated per task because TSM 
            configuration can be set asynchronously for different tasks, So 
            using a single buffer was overwriting the memory */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory(
                                    (void*)pDevice, 
                                    sizeof(BDSP_VideoEncodeTaskDatasyncSettings), 
                                    2, 
                                    0);
        
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for"
                      " video encoder Frame sync buffer!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;        
        }

        pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr = \
            ui32BaseAddr;
        
        pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size = \
            sizeof(BDSP_VideoEncodeTaskDatasyncSettings);

#if 0 /* This is not DRAM memory now but RDB address */
    	/* 2 Extra queue's needed for video encoder i.e. RDQ and RRQ */
        /* Allocate Raw Picture delivery queue */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE*4,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_DSP_AllocMem: Unable to Allocate memory for Raw Picture delivery queue!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pRaagaTask->taskMemGrants.sTaskQueue.sRDQueue.uiBaseAddr = ui32BaseAddr;
#endif 

        pRaagaTask->taskMemGrants.sTaskQueue.sRDQueue.uiMsgQueueSize = BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE * 4;

        /*TODO : Correct the calculation for DSP1*/
        pRaagaTask->taskMemGrants.sTaskQueue.sRDQueue.ui32FifoId = (2*i) + BDSP_RAAGA_DSP_P_FIFO_PDQ;

        /* Allocate Raw Picture release queue */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE*4,2, 0);
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_DSP_AllocMem: Unable to Allocate memory for Raw Picture Release queue!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end; 
        }
        pRaagaTask->taskMemGrants.sTaskQueue.sRRQueue.uiBaseAddr = ui32BaseAddr;
        pRaagaTask->taskMemGrants.sTaskQueue.sRRQueue.uiMsgQueueSize = BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE * 4;
        /*TODO : Correct the calculation for DSP1*/
        pRaagaTask->taskMemGrants.sTaskQueue.sRRQueue.ui32FifoId = (2*i) + BDSP_RAAGA_DSP_P_FIFO_PRQ;

    }

	else if(BDSP_ContextType_eScm == pRaagaContext->settings.contextType )
    {
    
		/* Cit memory. This is now context dependent as the sizes changes for different context */
		ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, sizeof(BDSP_SCM_P_sTASK_CONFIG),2, 0);
		if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
		{
			BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for cit!"));
			err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			goto end;		 
		}
		pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr = ui32BaseAddr;
		pRaagaTask->taskMemGrants.sCitStruct.ui32Size = sizeof(BDSP_SCM_P_sTASK_CONFIG);
    }
    /* Start Task memory */
    ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, sizeof(BDSP_Raaga_P_TaskParamInfo),2, 0);
    if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for start task params!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;        
    }
    pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr = ui32BaseAddr;
    pRaagaTask->taskMemGrants.sTaskInfo.ui32Size = sizeof(BDSP_Raaga_P_TaskParamInfo);

    if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
    {

	
	    /* Cit memory. This is now context dependent as the sizes changes for different context */
    ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory((void*)pDevice, sizeof(BDSP_AF_P_sTASK_CONFIG),2, 0);
    if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for cit!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;        
    }
    pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr = ui32BaseAddr;
    pRaagaTask->taskMemGrants.sCitStruct.ui32Size = sizeof(BDSP_AF_P_sTASK_CONFIG);

        /* Allocate buffer required for on-the-fly programming of TSM 
            configuration parameters for each task. TSM is now part of 
            Framesync and this memory is allocated per task because TSM 
            configuration can be set asynchronously for different tasks, So 
            using a single buffer was overwriting the memory */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory(
                                    (void*)pDevice, 
                                    sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams), 
                                    2, 
                                    0);
        
        if(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32BaseAddr))
        {
            BDBG_ERR(("BDSP_Raaga_P_AllocateTaskMemory: Unable to Allocate memory for"
                      " TSM and Frame sync buffer!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;        
        }

        pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr = \
            ui32BaseAddr;
        
        pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size = \
            sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams);
    }

end:
    if(err != BERR_SUCCESS)
    {
        BDSP_Raaga_P_FreeTaskMemory(pTaskHandle);
    }
return err;    
}

BERR_Code BDSP_Raaga_P_AllocateContextMemory(
    void *pContextHandle
    )
{
    BERR_Code err = BERR_SUCCESS;
    uint32_t ui32BaseAddr = 0;
    unsigned int i = 0;
    bool    bOpenTimeAudProcDownload=false;
    BDSP_RaagaContext   *pRaagaContext = (BDSP_RaagaContext   *)pContextHandle;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pRaagaContext->pDevice;

    void *pDeviceHandle=(void *)pDevice;

    BDBG_ENTER(BDSP_Raaga_P_AllocateContextMemory);

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);        
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);        

    err = BDSP_MM_P_CalcMemPoolReq (pContextHandle);
    if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: "
                    "Unable to get memory requirements for DSP!"));
        err = BERR_TRACE(err);
        goto end;
    }


    
    if(false == pDevice->settings.preloadImages)
    {

        if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
        {
            /* START TIME FIRMWARE DOWNLOAD */           
            BDSP_Raaga_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);

            if(true == bOpenTimeAudProcDownload)
            {	
                if(pDevice->memReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size)
                {
                    ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size,
                        2, 0);
                    
                    if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                    {
                        BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: "
                                 "Unable to Allocate memory for Processing Algo!"));
                        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto end;                        
                    }
                    pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr = \
                        ui32BaseAddr ;
                    
                    pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size = 
    					pDevice->memReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;				
                }        
            }
            else
            {
                if(pDevice->memReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size)
                {
                    ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                        pDevice->memReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size *
                        pRaagaContext->settings.audioDownloadBuffer.maxNumAudioProcessingDownloadBuffer,2, 0);
                    
                    if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                    {
                        BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: "
                                "Unable to Allocate memory for Processing Algo!"));
                        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto end;  
                    }
                    
                    for(i=0;i<pRaagaContext->settings.audioDownloadBuffer.maxNumAudioProcessingDownloadBuffer;i++)
                    {
                        pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].ui32BaseAddr = 
                            ui32BaseAddr + 
                            i*pDevice->memReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;
                        
                        pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].ui32Size = 
                            pDevice->memReq.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;
                        
        			    pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].numUser = \
                            -1;
                    
        			    pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i].uExecAlgoType.ePAlgoType = \
                            BDSP_AudioProcessing_eMax;
                    }
                }
            }
            
            if(pDevice->memReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size)
            {
                ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                    pDevice->memReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size *
                    pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDatasyncDownloadBuffer,2, 0);   /* 32 bit aligned*/
                if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: Unable to Allocate memory for FrameSync!"));
                    err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;  
                }
                for(i=0;i<pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDatasyncDownloadBuffer;i++)
                {
                    pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[i].ui32BaseAddr = 
                        ui32BaseAddr + i*pDevice->memReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size;
                    pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[i].ui32Size = 
                        pDevice->memReq.sDwnldMemInfo.sFsBufInfo[0].ui32Size;
    			pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[i].numUser = -1;
    			pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[i].uExecAlgoType.eAlgoType = BDSP_AudioType_eMax;				
                }
            }
            if(pDevice->memReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size)
            {
                ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                    pDevice->memReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size *
                    pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDecodeDownloadBuffer,2, 0); /* 32 bit aligned*/
                if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: Unable to Allocate memory for Decode!"));
                    err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;  
                }
                for(i=0;i<pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDecodeDownloadBuffer;i++)
                {
                    pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[i].ui32BaseAddr = 
                        ui32BaseAddr + i*pDevice->memReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;
                    pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[i].ui32Size = 
                        pDevice->memReq.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;
    			pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[i].numUser = -1;
    			pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[i].uExecAlgoType.eAlgoType = BDSP_AudioType_eMax;					
                }
            }
            if(pDevice->memReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size)
            {
                ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                    pDevice->memReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size *
                    pRaagaContext->settings.audioDownloadBuffer.maxNumAudioPassthruDownloadBuffer,2, 0);  /* 32 bit aligned*/
                if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: Unable to Allocate memory for Passthru!"));
                    err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;  
                }
                for(i=0;i<pRaagaContext->settings.audioDownloadBuffer.maxNumAudioPassthruDownloadBuffer;i++)
                {
                    pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[i].ui32BaseAddr = 
                        ui32BaseAddr + i*pDevice->memReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;
                    pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[i].ui32Size = 
                        pDevice->memReq.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;
    			pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[i].numUser = -1;
    			pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[i].uExecAlgoType.ePAlgoType = BDSP_AudioProcessing_eMax;					
    				
                }
            }
            if( pDevice->memReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size *pRaagaContext->settings.audioDownloadBuffer.maxNumAudioEncodeDownloadBuffer)
            {
                ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                    pDevice->memReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size *
                    pRaagaContext->settings.audioDownloadBuffer.maxNumAudioEncodeDownloadBuffer,2, 0);  /* 32 bit aligned*/
                if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: Unable to Allocate memory for Processing Algo!"));
                    err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;  
                }
                for(i=0;i<pRaagaContext->settings.audioDownloadBuffer.maxNumAudioEncodeDownloadBuffer;i++)
                {
                    pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].ui32BaseAddr = 
                        ui32BaseAddr + i*pDevice->memReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;
                    pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].ui32Size = 
                        pDevice->memReq.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;
                    pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].numUser = -1;
                    pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i].uExecAlgoType.eEAlgoType = BDSP_AudioEncode_eMax;					
                }
            }
        }
        else if(pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
        {
            if(pDevice->memReq.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size)
            {
                ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                    pDevice->memReq.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size *
                    BDSP_RAAGA_MAX_VIDEODECODE_CTXT,2, 0); /* 32 bit aligned*/
                if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Decode!"));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }
                for(i=0;i<BDSP_RAAGA_MAX_VIDEODECODE_CTXT;i++)
                {
                    pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].ui32BaseAddr = 
                        ui32BaseAddr + i*pDevice->memReq.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;
                    BDBG_MSG(("%x",pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].ui32BaseAddr));
                    pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].ui32Size = 
                        pDevice->memReq.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;
        			pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].numUser = -1;
        			pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i].uExecAlgoType.eVideoAlgoType = BDSP_VideoType_eMax;					
                }
            } 
        }
        else if(BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType )
        {
            if(pDevice->memReq.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32Size)
            {
                ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                    pDevice->memReq.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32Size *
                    BDSP_RAAGA_MAX_VIDEOENCODE_CTXT, 2, 0); /* 32 bit aligned*/
				
                if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Decode!"));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }
                for(i=0;i<BDSP_RAAGA_MAX_VIDEOENCODE_CTXT;i++)
                {
                    pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[i].ui32BaseAddr = 
                        ui32BaseAddr + i*pDevice->memReq.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32Size;
                    BDBG_MSG(("Base Addr = 0x%x",pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[i].ui32BaseAddr));
                    pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[i].ui32Size = 
                        pDevice->memReq.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32Size;
        			pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[i].numUser = -1;
        			pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[i].uExecAlgoType.eVideoEncAlgoType = BDSP_VideoEncode_eMax;
                }
            } 
        }                
		else if(BDSP_ContextType_eScm == pRaagaContext->settings.contextType )
        {
            if(pDevice->memReq.sDwnldMemInfo.sScmCodecBufInfo[0].ui32Size)
            {
                ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, 
                    pDevice->memReq.sDwnldMemInfo.sScmCodecBufInfo[0].ui32Size *
                    BDSP_RAAGA_MAX_SCM_CTXT, 2, 0); /* 32 bit aligned*/
				
                if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SCM!"));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }
                for(i=0;i<BDSP_RAAGA_MAX_SCM_CTXT;i++)
                {
                    pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[i].ui32BaseAddr = 
                        ui32BaseAddr + i*pDevice->memReq.sDwnldMemInfo.sScmCodecBufInfo[0].ui32Size;
                    BDBG_MSG(("Base Addr = 0x%x",pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[i].ui32BaseAddr));
                    pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[i].ui32Size = 
                        pDevice->memReq.sDwnldMemInfo.sScmCodecBufInfo[0].ui32Size;
        			pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[i].numUser = -1;
        			pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[i].uExecAlgoType.eScmAlgoType = BDSP_ScmType_eMax;
                }
            } 
        }         
    }
    
    for(i=0;i<BDSP_RAAGA_MAX_DSP;i++)
    {
        /* Allocate Task Memory */
        /* Allocate Scratch */
        ui32BaseAddr = (uint32_t)BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, pDevice->memReq.ui32DspScratchMemReq[i],2, 0);	/* 32 bit aligned*/            
        if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: Unable to Allocate memory for scratch!"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;  
        }
        pRaagaContext->contextMemInfo.sDspScratchInfo[i].ui32BaseAddr = ui32BaseAddr;
        pRaagaContext->contextMemInfo.sDspScratchInfo[i].ui32Size = pDevice->memReq.ui32DspScratchMemReq[i];

        /* Add up memory requirement to create a whole big chunk for status buffer */        
    }
    
    /* Allocate buffer required for on-the-fly programming of configuration parameters */
    ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, pDevice->memReq.ui32ConfigBufReq, 2, 0);
    if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: Unable to Allocate memory for extra config buffer!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;  
    }
    
    pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32BaseAddr = ui32BaseAddr;
    pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32Size = pDevice->memReq.ui32ConfigBufReq;

#if 0
    if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
    {    
        /* Allocate buffer required for on-the-fly programming of SPDIF  Channel status parameters */
        ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, sizeof(BDSP_AF_P_sSPDIF_USER_CFG), 2, 0);
        if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
        {
            BDBG_ERR(("BDSP_DSP_AllocMem: Unable to Allocate memory for SPDIF Channel status config buffer!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        }
        pRaagaContext->contextMemInfo.sSpdifStatusBitBufInfo.ui32BaseAddr = ui32BaseAddr;
        pRaagaContext->contextMemInfo.sSpdifStatusBitBufInfo.ui32Size = sizeof(BDSP_AF_P_sSPDIF_USER_CFG);
    }
#endif
    /* VOM Table DRAM */
    ui32BaseAddr = (uint32_t) BDSP_Raaga_P_AllocateAlignedMemory(pDeviceHandle, sizeof(BDSP_VOM_Table), 2, 0);
    if((void *) BDSP_RAAGA_INVALID_DRAM_ADDRESS == (void *)ui32BaseAddr)
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateContextMemory: Unable to Allocate memory for VOM Table in DRAM!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;  
    }
    pRaagaContext->contextMemInfo.sVomTableInfo.ui32BaseAddr = ui32BaseAddr;
    pRaagaContext->contextMemInfo.sVomTableInfo.ui32Size = sizeof(BDSP_VOM_Table);

end:
    if(err != BERR_SUCCESS)
    {
        BDSP_Raaga_P_FreeContextMemory(pContextHandle);
    }    
    BDBG_LEAVE(BDSP_Raaga_P_AllocateContextMemory);
    return err;
}


void BDSP_Raaga_P_MemWrite32(
    BMEM_Handle hHeap,     
    uint32_t	memAddress, /* Memory offset to write */
    uint32_t	data    /* Data to write */
    )
{
/*	*((volatile uint32_t *)memAddress) = data;*/
        BDSP_Raaga_P_CopyDataToDram(hHeap,&data,memAddress,4);
}

void BDSP_Raaga_P_MemWrite32_isr(
    BMEM_Handle hHeap,     
    uint32_t	memAddress, /* Memory offset to write */
    uint32_t	data    /* Data to write */
    )
{
/*	*((volatile uint32_t *)memAddress) = data;*/
        BDSP_Raaga_P_CopyDataToDram_isr(hHeap,&data,memAddress,4);
}


uint32_t BDSP_Raaga_P_MemRead32(
        BMEM_Handle hHeap,     
        uint32_t	memAddress /* Memory offset to write */
    )
{
    uint32_t ui32ValRead;
    BDSP_Raaga_P_CopyDataFromDram(hHeap,&ui32ValRead,memAddress,4);
    return ui32ValRead;
}

uint32_t BDSP_Raaga_P_MemRead32_isr(
        BMEM_Handle hHeap,     
        uint32_t	memAddress /* Memory offset to write */
    )
{
    uint32_t ui32ValRead;
    BDSP_Raaga_P_CopyDataFromDram_isr(hHeap,&ui32ValRead,memAddress,4);
    return ui32ValRead;
}


BERR_Code BDSP_Raaga_P_CopyDataToDram (
    BMEM_Handle hHeap, 
    uint32_t *data, 
    uint32_t memAdr, 
    uint32_t size
    )
{
    void *dest=NULL;
    BERR_Code    ret=BERR_SUCCESS;

    BDBG_ENTER (BDSP_Raaga_P_CopyDataToDram);
    
    if (data==NULL) 
    {
        ret = BMEM_Heap_ConvertAddressToCached (hHeap,
                (void *) memAdr,(void **) &dest);
        
        if (ret!=BERR_SUCCESS)
        {
            BDBG_ERR(("BMEM_Heap_ConvertAddressToCached gave Error %d",ret));
            return BERR_TRACE(ret);
        }            
        
        BKNI_Memset(dest, 0, size);
        BMEM_Heap_FlushCache(hHeap,(void *)dest,size);                          
    }
    else 
    {
        ret = BMEM_Heap_ConvertAddressToCached (hHeap,(void *) 
                memAdr,(void **) &dest);
        
        if (ret!=BERR_SUCCESS)
        {
            BDBG_ERR(("BMEM_Heap_ConvertAddressToCached gave Error %d",ret));
            return BERR_TRACE(ret);
        }
        
        BKNI_Memcpy(dest, data , size);
        BMEM_Heap_FlushCache(hHeap,(void *)dest,size);               
    }
    
    BDBG_LEAVE(BDSP_Raaga_P_CopyDataToDram);
    return ret;
}


BERR_Code BDSP_Raaga_P_CopyDataFromDram (
    BMEM_Handle hHeap, 
    uint32_t *data, 
    uint32_t memAdr, 
    uint32_t size
    )
{
    void *dest=NULL;
    BERR_Code    ret=BERR_SUCCESS;

    BDBG_ENTER (BDSP_Raaga_P_CopyDataToDram);

    ret = BMEM_Heap_ConvertAddressToCached (hHeap,(void *) 
            memAdr,(void **) &dest);
    
    if (ret!=BERR_SUCCESS)
    {
        BDBG_ERR(("BMEM_Heap_ConvertAddressToCached gave Error %d",ret));
        return BERR_TRACE(ret);
    }
    
    BMEM_Heap_FlushCache(hHeap,(void *)dest,size);
    
    BKNI_Memcpy(data,dest,size);

    BDBG_LEAVE(BDSP_Raaga_P_CopyDataToDram);
    return ret;
}

BERR_Code BDSP_Raaga_P_CopyDataFromDram_isr (
    BMEM_Handle hHeap, 
    uint32_t *data, 
    uint32_t memAdr, 
    uint32_t size
    )
{
    void *dest=NULL;
    BERR_Code    ret=BERR_SUCCESS;

    BDBG_ENTER (BDSP_Raaga_P_CopyDataFromDram_isr);

    ret = BMEM_Heap_ConvertAddressToCached_isr (hHeap,(void *) 
            memAdr,(void **) &dest);
    
    if (ret!=BERR_SUCCESS)
    {
        BDBG_ERR(("BMEM_Heap_ConvertAddressToCached gave Error %d",ret));
        return BERR_TRACE(ret);
    }
    
    BMEM_Heap_FlushCache_isr(hHeap,(void *)dest,size);
    
    BKNI_Memcpy(data,dest,size);

    
    BDBG_LEAVE(BDSP_Raaga_P_CopyDataFromDram_isr);
    return ret;
}



BERR_Code BDSP_Raaga_P_CopyDataToDram_isr (
    BMEM_Handle hHeap, 
    uint32_t *data, 
    uint32_t memAdr, 
    uint32_t size
    )
{
    void *dest=NULL;
    BERR_Code    ret=BERR_SUCCESS;

    BDBG_ENTER (BDSP_Raaga_P_CopyDataToDram_isr);
    
    if (data==NULL) 
    {
        ret = BMEM_Heap_ConvertAddressToCached_isr(hHeap,
                (void *) memAdr,(void **) &dest);
        
        if (ret!=BERR_SUCCESS)
        {
            BDBG_ERR(("BMEM_Heap_ConvertAddressToCached gave Error %d",ret));
            return BERR_TRACE(ret);
        }            
        
        BKNI_Memset(dest, 0, size);
        BMEM_Heap_FlushCache_isr(hHeap,(void *)dest,size);                         
    }
    else 
    {
        ret = BMEM_Heap_ConvertAddressToCached_isr(hHeap,(void *) 
                memAdr,(void **) &dest);
        
        if (ret!=BERR_SUCCESS)
        {
            BDBG_ERR(("BMEM_Heap_ConvertAddressToCached gave Error %d",ret));
            return BERR_TRACE(ret);
        }
        
        BKNI_Memcpy(dest, data , size);
        BMEM_Heap_FlushCache_isr(hHeap,(void *)dest,size);              
    }
    
    BDBG_LEAVE(BDSP_Raaga_P_CopyDataToDram_isr);
    return ret;
}


