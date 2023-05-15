/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_mm_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 1/3/13 5:21p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_mm_priv.h $
 * 
 * Hydra_Software_Devel/14   1/3/13 5:21p ananthan
 * SW7425-3628: Changes for cached access (Merge to mainline)
 * 
 * Hydra_Software_Devel/SW7425-3628/1   8/29/12 5:57p jgarrett
 * SW7425-3628: Converting BMEM accesses to cached memory
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_mm_priv.h $
 * 
 * Hydra_Software_Devel/14   1/3/13 5:21p ananthan
 * SW7425-3628: Changes for cached access (Merge to mainline)
 * 
 * Hydra_Software_Devel/13   11/26/12 1:20p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/12   3/1/12 8:49p ashoky
 * SW7231-585: Changing the RDQ implementation from DRAM based to RDB
 * based.
 * 
 * Hydra_Software_Devel/11   2/15/12 3:41p gprasad
 * SW7425-2091: Increasing number of tasks supported in DSP to 12 to
 * support seamless 6-passthru usage
 * 
 * Hydra_Software_Devel/10   2/8/12 8:18p ashoky
 * SW7346-598: Removing dynamic mem-alloc/free from decode start stop
 * time. Moving it to device  open/close.
 * 
 * Hydra_Software_Devel/9   2/3/12 11:53a ashoky
 * FWRAA-362: Raaga dsp sram optimization. Providing memory to swap
 * resident data.
 * 
 * Hydra_Software_Devel/8   1/27/12 1:20p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/7   10/20/11 5:43p gprasad
 * FWRAA-359: Fixing the warnings
 * 
 * Hydra_Software_Devel/6   10/19/11 4:10p gprasad
 * SW7425-468: Fixing NRT mode related issue with seamless transcode
 * 
 * Hydra_Software_Devel/5   8/18/11 12:53p gprasad
 * SW7422-400: Updating the APIs with review comments
 * 
 * Hydra_Software_Devel/4   7/4/11 6:35p gprasad
 * SW7422-400: Please add hooks to support dumping the UART debug buffer
 * into DRAM
 * 
 * Hydra_Software_Devel/3   6/8/11 4:43p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/2   5/16/11 10:08p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/1   4/6/11 2:28p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/8   3/5/11 4:52a srajapur
 * SW7422-314 : [7422] Adding RDB Independence Feature in BASE DSP
 * Codebase
 * 
 * Hydra_Software_Devel/7   2/26/11 12:40a srajapur
 * SW7422-293:[7422] Adding support for encode algorithms
 * 
 * Hydra_Software_Devel/6   1/26/11 3:18a gautamk
 * SW7422-191: Allocating FW debug buffer on basis of runtime Parrameter
 * enableDebugLog of BDSP_RaagaSettings.
 * 
 * Hydra_Software_Devel/5   1/25/11 2:23a gautamk
 * SW7422-191:Adding error recovery code for Raaga Open
 * 
 * Hydra_Software_Devel/4   1/20/11 8:11a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/3   1/19/11 7:15a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/2   1/18/11 4:20a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/1   1/13/11 5:30a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 ***************************************************************************/

#ifndef BDSP_RAAGA_MM_PRIV_H_
#define BDSP_RAAGA_MM_PRIV_H_
#include "bdsp_raaga_fwinterface_priv.h"

#define BDSP_RAAGA_MAX_MSGS_PER_QUEUE     10
#define BDSP_RAAGA_MAX_ASYNC_MSGS_PER_QUEUE     40
#define BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE   32


#ifdef BDSP_AUDIODESC_SUPPORT
#define BDSP_RAAGA_AUDDESC_TASK 1
#else
#define BDSP_RAAGA_AUDDESC_TASK 0
#endif

#define BDSP_RAAGA_CAPTURE_TASK 1

/* To support 1 MS-11 usage + dual seamless transcods usage cases - 8 tasks */
/* To support 6 seamless transcodes - 12 tasks */
#define BDSP_RAAGA_MAX_FW_TASK_PER_CTXT (int32_t)12 

#define BDSP_RAAGA_MAX_FW_TASK_PER_DSP (int32_t)12  /*There are 39 FIFO registers avalailable now, 1 is used for 
debug message, 1 is used for Global command and 1 for generic response. So upto 18 tasks can be supported. */



#define BDSP_RAAGA_INVALID_DRAM_ADDRESS			0xFFFFFFFF

#define BDSP_RAAGA_MAX_AUD_PROCESSING_CTX 6
#define BDSP_RAAGA_MAX_FS_CTXT  12                          /* If mixer ids is not counted then only 6 */
#define BDSP_RAAGA_MAX_DECODE_CTXT 12                       /* If mixer task is not counted then only 6 */
#define BDSP_RAAGA_MAX_PASSTHRU_CTXT 6
#define BDSP_RAAGA_MAX_ENCODE_CTXT 2

#define BDSP_RAAGA_MAX_VIDEODECODE_CTXT 1
/* settings for video encoder on Raaga */
#define BDSP_RAAGA_MAX_VIDEOENCODE_CTXT					1
#define	BDSP_RAAGA_MAX_FW_TASK_PER_VIDEO_ENCODE_CTXT	1
#define BDSP_RAAGA_MAX_SCM_CTXT					(int32_t)1
#define	BDSP_RAAGA_MAX_FW_TASK_PER_SCM_CTXT		(int32_t)1

/* Memory Block */
typedef struct BDSP_Raaga_P_MemBlock
{
    uint32_t    ui32Addr;
    uint32_t    ui32Size;    
}BDSP_Raaga_P_MemBlock;



/***************************************************************************
Summary:
	This structure contains the Buffer Properties.
***************************************************************************/
typedef struct BDSP_Raaga_P_CtxtFwBufInfo
{
	uint32_t				ui32BaseAddr;
	uint32_t				ui32Size;
    
	union
	{
		BDSP_AudioType   eAlgoType;
		BDSP_AudioProcessing 	ePAlgoType;
        BDSP_AudioEncode eEAlgoType;
		BDSP_VideoType  eVideoAlgoType;        
		BDSP_ScmType	eScmAlgoType;
		BDSP_VideoEncode eVideoEncAlgoType;
	}uExecAlgoType;
	
	int32_t				numUser;
}BDSP_Raaga_P_CtxtFwBufInfo;

/*******************************************************************************
Note: In case of OpenTimeDownload, The memory allocation for following should 
be consecutive and in the same order as it is mentioned in this datstructure.
*******************************************************************************/
typedef struct BDSP_Raaga_P_DwnldMemInfo
{
	BDSP_Raaga_P_MemBlock sSystemCodeMem;     /* System Code */
	BDSP_Raaga_P_MemBlock sSystemDataMem;     /* System Data */
    BDSP_Raaga_P_MemBlock sSystemRdbVarsMem;  /* System RdbVars */    
    BDSP_Raaga_P_MemBlock sSystemLibMem;      /* System Lib */
    BDSP_Raaga_P_MemBlock sAlgoLibMem;        /* Algo Lib */    
    BDSP_Raaga_P_MemBlock sCommonIdsCodeMem;  /* Common Ids Code */      
	BDSP_Raaga_P_MemBlock sScmTaskCodeMem;  /* Common Ids Code */      
	BDSP_Raaga_P_MemBlock sScm1DigestMem;  /* Common Ids Code */      
	BDSP_Raaga_P_MemBlock sScm2DigestMem;  /* Common Ids Code */      

	BDSP_Raaga_P_CtxtFwBufInfo	sProcessingAlgoBufInfo[BDSP_RAAGA_MAX_AUD_PROCESSING_CTX];
						/*This will have the information regarding the Buffer 
						for Processing Algorithm.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						BDSP_RAAGA_MAX_PROCESSING_CTX = Max no. of Processing 
						algorithm which can simultaneously execute at a time in 
						the DSP*/

	BDSP_Raaga_P_CtxtFwBufInfo	sFsBufInfo[BDSP_RAAGA_MAX_FS_CTXT]; 
						/*This will have the information regarding the all the 
						buffers for Frame Sync.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the Buf for downloading 
						framesync of all the supported algorithm
						BDSP_RAAGA_MAX_FS_CTXT = Max no. of Framesync Executable
						whose algorithm can simultaneously execute at a time in 
						the DSP*/

	BDSP_Raaga_P_CtxtFwBufInfo	sDecCodecBufInfo[BDSP_RAAGA_MAX_DECODE_CTXT];
						/*This will have the information regarding the all the 
						buffers for Decode Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Decode Exec of all the supported algorithm 
						BDSP_RAAGA_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/

	BDSP_Raaga_P_CtxtFwBufInfo	sPtCodecBufInfo[BDSP_RAAGA_MAX_PASSTHRU_CTXT];
						/*This will have the information regarding the all the 
						buffers for Passthru Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Passthru Exec of all the supported algorithm 
						BDSP_RAAGA_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/

	BDSP_Raaga_P_CtxtFwBufInfo	sEncodeCodecBufInfo[BDSP_RAAGA_MAX_ENCODE_CTXT];
						/*This will have the information regarding the all the 
						buffers for Passthru Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Passthru Exec of all the supported algorithm 
						BDSP_RAAGA_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/
	BDSP_Raaga_P_CtxtFwBufInfo	sVideoDecCodecBufInfo[BDSP_RAAGA_MAX_VIDEODECODE_CTXT];
						/*This will have the information regarding the all the 
						buffers for Decode Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Decode Exec of all the supported algorithm 
						BDSP_RAAGA_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/						
	BDSP_Raaga_P_CtxtFwBufInfo	sScmCodecBufInfo[BDSP_RAAGA_MAX_SCM_CTXT];
						/*This will have the information regarding the all the 
						buffers for SCM Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Decode Exec of all the supported algorithm 
						BDSP_RAAGA_MAX_SCM_CTXT = Max no. of SCM
						executable which can simultaneously execute at a time in 
						the DSP*/							
						
	BDSP_Raaga_P_CtxtFwBufInfo	sVideoEncodeCodecBufInfo[BDSP_RAAGA_MAX_VIDEOENCODE_CTXT];
						/*This will have the information regarding the all the 
						buffers for Video Encode Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Decode Exec of all the supported algorithm 
						BDSP_RAAGA_MAX_VIDEOENCODE_CTXT = Max no. of Encode
						executable which can simultaneously execute at a time in 
						the DSP*/	
						
						
}BDSP_Raaga_P_DwnldMemInfo;



/* This structure is used to store base pointer & size of buffers used by 
   firmware like the interframe buffers & configparams buffers */
typedef struct BDSP_Raaga_P_FwBuffer
{
	uint32_t    ui32BaseAddr;
	uint32_t	ui32Size;
}BDSP_Raaga_P_FwBuffer;
/* contains Task Sync & Async Queue info */
typedef struct BDSP_Raaga_P_TaskQueues
{
    BDSP_Raaga_P_MsgQueueParams    sTaskSyncQueue;     /* Synchronous queue */
    BDSP_Raaga_P_MsgQueueParams    sTaskAsyncQueue;    /* Asynchronous queue */
    BDSP_Raaga_P_FwBuffer           sAsyncMsgBufmem;    /* Memory for Async */

    /*Video Task related Queues*/
    BDSP_Raaga_P_MsgQueueParams    sPDQueue;     /* PD queue */    
    BDSP_Raaga_P_MsgQueueParams    sPRQueue;     /* PR queue */    
    BDSP_Raaga_P_MsgQueueParams    sDSQueue;     /* DS queue */    
	
	/* Video Encode Task related Queues. Could be done away with video task
	itself but then video decode and encode can't co-exist */
	BDSP_Raaga_P_RdbQueueParams    sRDQueue;	 /* Raw picture delivery queue */    
	BDSP_Raaga_P_MsgQueueParams    sRRQueue;	 /* Raw picture release queue */    
    
}BDSP_Raaga_P_TaskQueues;

/* This structure contains actual addresses & sizes per task */
typedef struct BDSP_Raaga_P_TaskMemoryInfo
{
    BDSP_Raaga_P_TaskQueues       sTaskQueue;
    BDSP_Raaga_P_FwBuffer           sTaskInfo;
    BDSP_Raaga_P_FwBuffer           sCitStruct;
    BDSP_Raaga_P_FwBuffer           sTaskIFrameCfgBufInfo;
    BDSP_Raaga_P_FwBuffer            sFrameSyncTsmConfigParamBufInfo;    
                            /* Extra buffer to on-the-fly prog of Frame Sync
                                buffer. Since TSM is now part of FrameSync.                                    
                                This memory is allocated per task because 
                                configuration can be set asynchronously for 
                                different tasks, So using a single buffer 
                                was overwriting the memory */
}BDSP_Raaga_P_TaskMemoryInfo;




/* This structure contains iframe & cfgparams buffer memory req per task. This
   memory pool size is calculated as the worst case. This can be per DSP. This 
   can be extended later to not to use the worst case size. */
typedef struct BDSP_Raaga_P_TaskMemoryRequirement
{
    uint32_t    ui32IframeCommon;   /* Size of Iframe of common exec */
    uint32_t    ui32IFramePAlgo;    /* Size of Iframe of Processing Algo */
    uint32_t    ui32CfgBufCommon;   /* Size of ConfigParams of common exec */
    uint32_t    ui32CfgBufPAlgo;    /* Size of ConfigParams of Processing Algo*/
    uint32_t    ui32StatusBufCommon;/* Size of Status buffer of common exec */    
    uint32_t    ui32StatusBufPAlgo; /* Size of Status buffer of Processing Algo*/    
    uint32_t    ui32CBitBuf;        /* Buffer for the CBIT Information */
    uint32_t    ui32ExtraBuf ;      /* Extra buffer required for 
                                                - Stack Swap
                                                - Task Port Config
                                                - Task FMM Gate */
}BDSP_Raaga_P_TaskMemoryRequirement;



/* Memory requirement for the Raptor Device */
typedef struct BDSP_Raaga_P_MemoryRequest
{
    BDSP_Raaga_P_DwnldMemInfo       sDwnldMemInfo; 
                                /* Download memory requirements */
                                
    uint32_t                    ui32OpenTimeDownloadSize; 
                                /* Open time download memory requirement */
                                
    uint32_t                    ui32TaskMemory[BDSP_RAAGA_MAX_DSP]; 
                                /* Task memory requirements */
                                
    uint32_t                    ui32DspScratchMemReq[BDSP_RAAGA_MAX_DSP]; 
                                /* Scratch,interstage,interface buffers per dsp */
                                
    uint32_t                    ui32ConfigBufReq;   
                                /* Extra buffer to on-the-fly program config */
                                
    BDSP_Raaga_P_TaskMemoryRequirement    sTaskMemReq[BDSP_RAAGA_MAX_DSP]; 
                                /* Iframe & Cfgbuf memory requirement */

}BDSP_Raaga_P_MemoryRequest;

/* Memory allocated information for whole Raptor device */
typedef struct BDSP_Raaga_P_MemoryGrant
{
    BDSP_Raaga_P_DwnldMemInfo       sDwnldMemInfo;  
                                /* Download memory requirements */
                                
    BDSP_Raaga_P_FwBuffer            sOpenTimeMemInfo; 
                                /* Open time download memory info */
                                
    BDSP_Raaga_P_MsgQueueParams    cmdQueueParams[BDSP_RAAGA_MAX_DSP];  
                                /* Command queue per DSP */
                                
    BDSP_Raaga_P_MsgQueueParams    genRspQueueParams[BDSP_RAAGA_MAX_DSP];  
                                /* Generic (non-task) response queue per DSP */                                
                                
    void *pDSPFifoAddrStruct[BDSP_RAAGA_MAX_DSP];   /*Dram buffer to store addresses of DSP Fifo's used for message queues*/
                                
    BDSP_Raaga_P_MemBlock pFwDebugBuf[BDSP_RAAGA_MAX_DSP][BDSP_Raaga_DebugType_eLast];
	/* DRAM buffer for raaga system to swap its data memory with */
	BDSP_Raaga_P_MemBlock	sRaagaSwapMemoryBuf[BDSP_RAAGA_MAX_DSP];

}BDSP_Raaga_P_MemoryGrant;

/* Memory allocated information for whole Raptor device */
typedef struct BDSP_Raaga_P_ContextMemoryGrant
{

BDSP_Raaga_P_FwBuffer            sDspScratchInfo[BDSP_RAAGA_MAX_DSP]; 
                            /* Scratch,interstage,interface buffers per DSP*/                                                        
BDSP_Raaga_P_FwBuffer            sConfigParamBufInfo;    
                            /* Extra buffer to on-the-fly program cfg params*/                            
#if 0
BDSP_Raaga_P_FwBuffer            sSpdifStatusBitBufInfo;    
                            /* Extra buffer to on-the-fly program cfg params*/ 
#endif
BDSP_Raaga_P_FwBuffer            sVomTableInfo;
                            /* DRAM for VOM Table */
}BDSP_Raaga_P_ContextMemoryGrant;

/* Buffers required by CIT. This is to handle SW7346-598 */
typedef struct BDSP_Raaga_P_CitBuffers
{
    void			*psCitDataStructure;
    void			*psCitFwBranchInfo[BDSP_P_MAX_FW_BRANCH_PER_FW_TASK];
	void			*psCitPortCfg;
	void			*psCitTaskIsBuffCfg;
}BDSP_Raaga_P_CitBuffers;


#define BDSP_Raaga_P_AllocateAlignedMemory(hdev, sz, algn, bndry) BMEM_AllocAligned(((BDSP_Raaga *)(hdev))->memHandle, sz, algn, bndry)
#define BDSP_Raaga_P_FreeMemory(hdev, mptr) ((NULL == mptr)?BERR_SUCCESS:BMEM_Free(((BDSP_Raaga *)(hdev))->memHandle, mptr))

BERR_Code BDSP_Raaga_P_AllocateInitMemory (
    void *pDeviceHandle
    );

BERR_Code BDSP_Raaga_P_FreeInitMemory(
    void *pDeviceHandle
    );

BERR_Code BDSP_Raaga_P_AllocateContextMemory (
    void *pContextHandle
    );

BERR_Code BDSP_Raaga_P_FreeContextMemory(
    void *pContextHandle
    );

BERR_Code BDSP_Raaga_P_FreeTaskMemory(
    void *pTaskHandle
    );

BERR_Code BDSP_Raaga_P_AllocateTaskMemory(
    void *pTaskHandle
    );



void BDSP_Raaga_P_MemWrite32(
    BMEM_Handle hHeap,     
    uint32_t	memAddress, /* Memory offset to write */
    uint32_t	data    /* Data to write */
    );

void BDSP_Raaga_P_MemWrite32_isr(
    BMEM_Handle hHeap,     
    uint32_t	memAddress, /* Memory offset to write */
    uint32_t	data    /* Data to write */
    );


uint32_t BDSP_Raaga_P_MemRead32(
        BMEM_Handle hHeap,     
        uint32_t	memAddress /* Memory offset to write */
    );

uint32_t BDSP_Raaga_P_MemRead32_isr(
        BMEM_Handle hHeap,     
        uint32_t	memAddress /* Memory offset to write */
    );


    /* Summary: isr version of BMEM_ConvertOffsetToAddress */
#define BMEM_ConvertOffsetToAddress_isr(hHeap, ulOffset, ppvAddress) \
        BMEM_ConvertOffsetToAddress(hHeap, ulOffset, ppvAddress)

    

BERR_Code BDSP_Raaga_P_CopyDataToDram (
    BMEM_Handle hHeap, 
    uint32_t *data, 
    uint32_t memAdr, 
    uint32_t size
    );
BERR_Code BDSP_Raaga_P_CopyDataToDram_isr (
    BMEM_Handle hHeap, 
    uint32_t *data, 
    uint32_t memAdr, 
    uint32_t size
    );
BERR_Code BDSP_Raaga_P_CopyDataFromDram (
    BMEM_Handle hHeap, 
    uint32_t *data, 
    uint32_t memAdr, 
    uint32_t size
    );
BERR_Code BDSP_Raaga_P_CopyDataFromDram_isr (
    BMEM_Handle hHeap, 
    uint32_t *data, 
    uint32_t memAdr, 
    uint32_t size
    );





#endif

