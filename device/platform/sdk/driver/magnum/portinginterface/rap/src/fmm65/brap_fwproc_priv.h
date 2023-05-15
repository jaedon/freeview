/***************************************************************************
*     Copyright (c) 2006-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_fwproc_priv.h $
* $brcm_Revision: Hydra_Software_Devel/30 $
* $brcm_Date: 3/18/11 9:41p $
*
* Module Description:
*	This file contains PI-FW Interface.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_fwproc_priv.h $
* 
* Hydra_Software_Devel/30   3/18/11 9:41p sgadara
* SW7420-767: [7420] Merge MS11 development branch to mainline
* 
* Hydra_Software_Devel/29   3/7/11 6:05p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/28   10/6/10 4:33p srajapur
* SW7468-323:[7468] sReferenceBuffParams added to
* BRAP_VF_P_sVDecodeBuffCfg struct for supporting new design of video
* decoder on ZSP
* 
* Hydra_Software_Devel/27   10/5/10 4:16p srajapur
* SW7468-323:[7468] sReferenceBuffParams added to
* BRAP_VF_P_sVDecodeBuffCfg struct for supporting new design of video
* decoder on ZSP
* 
* Hydra_Software_Devel/26   9/16/10 12:19a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/25   9/6/10 5:43p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/24   9/6/10 5:16p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/RAP-SW7550-510-20100805/RAP_GFX_SUPPORT_PH6P0/3   9/6/10 3:26p gautamk
* SW7550-554: [7550]Fixing the bug in command queue size
* 
* Hydra_Software_Devel/RAP-SW7550-510-20100805/RAP_GFX_SUPPORT_PH6P0/2   9/6/10 3:21p gautamk
* SW7550-554: [7550]Fixing the bug in command queue size
* 
* Hydra_Software_Devel/RAP-SW7550-510-20100805/RAP_GFX_SUPPORT_PH6P0/1   8/9/10 5:12p gautamk
* SW7550-459: [7550] Meging GFX support to Ph6p0 of 7550
* 
* Hydra_Software_Devel/RAP-SW7550-510-20100805/1   8/5/10 8:40p srajapur
* SW7550-510 : [7550] ASTM enabled decode results into raptor watchdog --
* --> Changed the Messages per queue size to 5
* 
* Hydra_Software_Devel/23   1/28/10 10:58p gautamk
* SW7405-3063:[7405] INCREASING aSYNC QUEUE SIZE
* 
* Hydra_Software_Devel/22   1/28/10 10:55p gautamk
* SW7405-3063:[7405] INCREASING SYNC QUEUE SIZE
* 
* Hydra_Software_Devel/21   10/21/09 5:02p gautamk
* SW7335-609:[7405] Setting hTask-bStopped appropriately in Isr Ctxt when
* Event is getting set. This is done to fix race condition. Porting to
* mainline
* 
* Hydra_Software_Devel/20   6/12/09 3:41p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/19   4/3/09 4:24p gautamk
* PR53764: [7405]correcting Macro
* 
* PR53393: [7405]  correcting Macro
* 
* Hydra_Software_Devel/18   4/1/09 4:39p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/17   10/10/08 10:04a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   9/24/08 6:01p speter
* PR 45907: [3548,3556] Support for 4 contexts. Audio descriptor decode
* (2) and passthru (1) with capture channel (1)
* 
* Hydra_Software_Devel/16   8/5/08 3:54p speter
* PR 45407: [3548,3556] Merging the Phase 2.1 release branch to main line
* of development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/RAP_354856_PH_2_1/1   8/4/08 1:28p speter
* PR 45094: [3548,3556] Added support for 3 simultaneous decode channels.
* This usage case is required when we have Audio Desc (2) + passthru(1)
* 
* Hydra_Software_Devel/15   7/25/08 4:39p gautamk
* PR43493: [7405] TSM configuration can be  set asynchronously for
* different tasks, So using a single buffer for ChangeCfgCommand was
* causing overwrite of the memory. Using seperate memory buffer for each
* task for updating TSm config param.
* 
* Hydra_Software_Devel/14   7/18/08 4:21p gautamk
* PR44827: [7405] Increasing the async fifo size.
* 
* Hydra_Software_Devel/13   4/14/08 11:38a sushmit
* PR 34648: [7405] Reducing Stack Usage
* 
* Hydra_Software_Devel/12   2/29/08 4:25p gautamk
* PR34648: [7405] Changing file names includes
* 
* Hydra_Software_Devel/11   2/29/08 3:41p gautamk
* PR34648: [7405] Adding on the fly change for spdifstatuparam
* 
* Hydra_Software_Devel/10   2/15/08 8:45p gautamk
* PR34648: [7405]Adding User Config for Passthru
* Adding spdif CBit buffer.
* 
* Hydra_Software_Devel/9   1/30/08 3:07p gdata
* PR34648: [7405] Adding support for status buffers
* 
* Hydra_Software_Devel/8   1/16/08 4:51p nitinb
* PR34648: [7405] Added support for programming configuration parameters
* 
* Hydra_Software_Devel/7   1/15/08 2:30p sushmit
* PR34648: [7405] Moving CIT Output malloc to DRAM
* 
* Hydra_Software_Devel/6   1/15/08 1:37p sushmit
* PR34648: [7405] Checkin in updated code
* 
* Hydra_Software_Devel/5   1/8/08 1:45p sushmit
* PR34648: [7405] Updating Memory allocation to allocate one big chunk
* for Iframe & Cfgbuf instead of separate buffers per stage
* 
***************************************************************************/
#include "brap_dspchn_priv.h"
#include "brap_fwif_priv.h"
#ifdef RAP_GFX_SUPPORT
#include "brap_gfx.h"
#endif

#define BRAP_MAX_DSP_SUPPORTED 1
#ifdef RAP_AUDIODESC_SUPPORT
#define BRAP_AUDDESC_TASK 1
#else
#define BRAP_AUDDESC_TASK 0
#endif

#if (BRAP_3548_FAMILY == 1)
#define BRAP_CAPTURE_TASK 1
#else
#define BRAP_CAPTURE_TASK 0
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
#define BRAP_MIXER_TASK 1
#else
#define BRAP_MIXER_TASK 0
#endif

#ifdef RAP_GFX_SUPPORT
#define BRAP_GFX_TASK 1
#else
#define BRAP_GFX_TASK 0
#endif

#ifdef RAP_SCM_SUPPORT
#define BRAP_SCM_TASK 1
#else
#define BRAP_SCM_TASK 0
#endif


#ifdef RAP_VIDEOONDSP_SUPPORT
#ifdef RAP_REALVIDEO_SUPPORT
#define BRAP_REALVIDEO_TASK 1
#endif
#else
#define BRAP_REALVIDEO_TASK 0
#endif

#ifdef RAP_SCM_SUPPORT
#define BRAP_MAX_FW_TSK_SUPPORTED (2 + BRAP_AUDDESC_TASK + BRAP_CAPTURE_TASK + BRAP_GFX_TASK + BRAP_REALVIDEO_TASK + BRAP_SCM_TASK )
#else
#define BRAP_MAX_FW_TSK_SUPPORTED (2 + BRAP_AUDDESC_TASK + BRAP_CAPTURE_TASK + \
                                BRAP_MIXER_TASK + BRAP_GFX_TASK + BRAP_REALVIDEO_TASK)
#endif

#define BRAP_MAX_FW_MAIN_EXEC_SUPPORTED BRAP_MAX_FW_TSK_SUPPORTED
#define BRAP_MAX_FW_TSM_EXEC_SUPPORTED BRAP_MAX_FW_TSK_SUPPORTED
#define BRAP_MAX_FW_FS_EXEC_SUPPORTED BRAP_MAX_FW_TSK_SUPPORTED

#define BRAP_MAX_MSGS_PER_QUEUE     5
#define BRAP_MAX_ASYNC_MSGS_PER_QUEUE     40
#define BRAP_FWIF_P_INVALID_TSK_ID		    ((unsigned int)(-1))
#define BRAP_FWIF_MSGQUE_ATTR_SIZE  16

#define BRAP_REALVIDEO_MAX_MSGS_PER_QUEUE   32
#define BRAP_REALVIDEO_TEMP_FRAME_BUF_SIZE 1000
#define BRAP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE 368640
#define BRAP_REALVIDEO_DECODE_CHROMAFRAME_BUF_SIZE 184320  
#define BRAP_REALVIDEO_DECODE_REF_LUMAFRAME_BUF_SIZE 770048
#define BRAP_REALVIDEO_DECODE_REF_CHROMAFRAME_BUF_SIZE 385024  
#define BRAP_REALVIDEO_DISP_FRAME_BUF_SIZE  1000
#define BRAP_REALVIDEO_TEMP_UPB_BUF_SIZE    1000
#define BRAP_REALVIDEO_DECODE_UPB_BUF_SIZE  1000
#define BRAP_REALVIDEO_DISP_UPB_BUF_SIZE 1000
#define BRAP_REALVIDEO_DRAM_MB_BUF_SIZE 675000

#define BRAP_MAX_GFX_OP_IN_QUEUE   41
#ifdef RAP_GFX_SUPPORT
#define BRAP_ADDITIONAL_MSGS_FOR_GFX   ((BRAP_MAX_GFX_OP_SUPPORTED+1 -BRAP_MAX_MSGS_PER_QUEUE)*BRAP_GFX_TASK)
#else
#define BRAP_ADDITIONAL_MSGS_FOR_GFX   (0)
#endif



/* This structure is used to store base pointer & size of buffers used by 
   firmware like the interframe buffers & configparams buffers */
typedef struct BRAP_Fwif_P_FwBufInfo
{
	uint32_t				ui32BaseAddr;
	uint32_t				ui32Size;
}BRAP_Fwif_P_FwBufInfo;

#ifdef RAP_GFX_SUPPORT                   
typedef struct BRAP_Fwif_P_GfxBufPool
{
    bool                                bFree[BRAP_MAX_GFX_OP_IN_QUEUE];
    unsigned int                                uiGfxOpId[BRAP_MAX_GFX_OP_IN_QUEUE];
    BRAP_Fwif_P_FwBufInfo   sGfxBuffer[BRAP_MAX_GFX_OP_IN_QUEUE];
}BRAP_Fwif_P_GfxBufPool;
#endif

/* This structure contains iframe & cfgparams buffer memory req per task. This
   memory pool size is calculated as the worst case. This can be per DSP. This 
   can be extended later to not to use the worst case size. */
typedef struct BRAP_Fwif_P_TskMemRequirement
{
    uint32_t    ui32IframeCommon; /* Memory Pool Size required for Iframe of common exec */
    uint32_t    ui32IFramePAlgo; /* Memory Pool Size required for Iframe of Processing Algo */
    uint32_t    ui32CfgBufCommon; /* Memory Pool Size required for ConfigParams of common exec */
    uint32_t    ui32CfgBufPAlgo; /* Memory Pool Size required for ConfigParams of Processing Algo */
    uint32_t    ui32StatusBufCommon; /* Memory Pool Size required for Status buffer of common exec */    
    uint32_t    ui32StatusBufPAlgo; /* Memory Pool Size required for Status buffer of Processing Algo */    
    uint32_t    ui32CBitBuf;    /* Buffer for the CBIT Information */
    uint32_t    ui32ExtraBuf ;  /* Extra buffer required for 
                                                - Stack Swap
                                                - Task Port Config
                                                - Task FMM Gate */
}BRAP_Fwif_P_TskMemRequirement;

/* contains Task Sync & Async Queue info */
typedef struct BRAP_Fwif_P_TskQueues
{
    BRAP_FWIF_MsgQueueParams    sTskSyncQueue;  /* Synchronous queue */
    BRAP_FWIF_MsgQueueParams    sTskAsyncQueue;  /* Asynchronous queue */
    uint32_t    ui32TaskId;     /* This determines whether this is in use or not */
    BRAP_Fwif_P_FwBufInfo   sAsyncMsgBufmem; /* Memory for consecutive 5 Async Msgs */
}BRAP_Fwif_P_TskQueues;

/* This structure contains actual addresses & sizes per task */
typedef struct BRAP_Fwif_P_TskMemInfo
{
    BRAP_Fwif_P_TskQueues    sTskQueue[BRAP_MAX_FW_TSK_SUPPORTED];
    BRAP_Fwif_P_FwBufInfo    sTskInfo[BRAP_MAX_FW_TSK_SUPPORTED]; /* Task memory info */
    BRAP_Fwif_P_FwBufInfo    sCitStruct[BRAP_MAX_FW_TSK_SUPPORTED]; /* Cit memory info */
    BRAP_Fwif_P_FwBufInfo    sTskIFrameCfgBufInfo[BRAP_MAX_FW_TSK_SUPPORTED];
}BRAP_Fwif_P_TskMemInfo;

/* Memory requirement for the Raptor Device */
typedef struct BRAP_Fwif_P_MemRequirement
{
    BRAP_DSP_DwnldMemInfo   sDwnldMemInfo; /* Download memory requirements */
    uint32_t    ui32OpenTimeDownloadSize; /* Open time download memory requirement */
    uint32_t    ui32TskMemory[BRAP_MAX_DSP_SUPPORTED]; /* Task memory requirements */
    uint32_t    ui32DspScratchMemReq[BRAP_MAX_DSP_SUPPORTED]; /* Scratch & interstage & interface buffers per dsp */
    BRAP_Fwif_P_TskMemRequirement sTskMemReq[BRAP_MAX_DSP_SUPPORTED]; /* Iframe & Cfgbuf memory requirement */
    uint32_t    ui32ConfigBufReq;   /* Extra buffer for on-the-fly programming of config params */
}BRAP_Fwif_P_MemRequirement;

#ifdef RAP_REALVIDEO_SUPPORT
typedef struct BRAP_Fwif_P_RealVideoBufInfo
{
    BRAP_Fwif_P_FwBufInfo   sLumaFrameBuffParams[BRAP_FWMAX_VIDEO_BUFF_AVAIL];
    BRAP_Fwif_P_FwBufInfo   sChromaFrameBuffParams[BRAP_FWMAX_VIDEO_BUFF_AVAIL];  
    BRAP_Fwif_P_FwBufInfo   sLumaReferenceBuffParams[BRAP_FWMAX_VIDEO_REF_BUFF_AVAIL];
    BRAP_Fwif_P_FwBufInfo   sChromaReferenceBuffParams[BRAP_FWMAX_VIDEO_REF_BUFF_AVAIL];            
    BRAP_Fwif_P_FwBufInfo   sUPBs[BRAP_FWMAX_VIDEO_BUFF_AVAIL];
    BRAP_Fwif_P_FwBufInfo   sDRAMMBInfoStartAdr[BRAP_FWMAX_MB_INFO_AVAIL];
}BRAP_Fwif_P_RealVideoBufInfo;
#endif

/* Memory allocated information for whole Raptor device */
typedef struct BRAP_Fwif_MemInfo
{
    BRAP_DSP_DwnldMemInfo   sDwnldMemInfo; /* Download memory requirements */
    BRAP_Fwif_P_FwBufInfo   sOpenTimeMemInfo; /* Open time download memory info */
    BRAP_FWIF_MsgQueueParams    sCmdQueue[BRAP_MAX_DSP_SUPPORTED];  /* Command queue per DSP */
    BRAP_FWIF_MsgQueueParams    sGenRspQueue[BRAP_MAX_DSP_SUPPORTED];  /* Generic (non-task) response queue per DSP */
#ifdef RAP_VIDEOONDSP_SUPPORT    
    BRAP_FWIF_MsgQueueParams    sPDQueue[BRAP_MAX_DSP_SUPPORTED];  /* Picture Delivery queue per DSP */
    BRAP_FWIF_MsgQueueParams    sPRQueue[BRAP_MAX_DSP_SUPPORTED];  /* Picture Release queue per DSP */
    BRAP_FWIF_MsgQueueParams    sDSQueue[BRAP_MAX_DSP_SUPPORTED];  /* Picture Release queue per DSP */    
#endif
    BRAP_Fwif_P_FwBufInfo   sDspScratchInfo[BRAP_MAX_DSP_SUPPORTED]; /* Scratch & interstage & interface buffers per dsp */
    BRAP_Fwif_P_TskMemInfo  sTskMemInfo[BRAP_MAX_DSP_SUPPORTED]; /* Iframe & Cfgbuf memory */
    BRAP_Fwif_P_FwBufInfo   sConfigParamBufInfo;    /* Extra buffer for on-the-fly programming of config params */
    BRAP_Fwif_P_FwBufInfo   sTsmConfigParamBufInfo[BRAP_MAX_FW_TSK_SUPPORTED];    /* Extra buffer for on-the-fly programming of TSM config params for each task*/ 
                                                                                                                                        /*This memory is allocated per task for TSM configuration, because TSM configuration can be
                                                                                                                                        set asynchronously for different tasks, So using a single buffer was overwriting the memory*/
    BRAP_Fwif_P_FwBufInfo   sSpdifStatusBitBufInfo;    /* Extra buffer for on-the-fly programming of config params */    
#ifdef RAP_VIDEOONDSP_SUPPORT 
    BRAP_Fwif_P_RealVideoBufInfo   sRealVideoBufferInfo;    /* Real Video */                                  
#endif
#ifdef RAP_GFX_SUPPORT                                                                                                                                        
    BRAP_Fwif_P_GfxBufPool   sGfxfInfo;    /* Extra buffer for on-the-fly programming of config params */    
#endif                                                                                                                                        
}BRAP_Fwif_MemInfo;

