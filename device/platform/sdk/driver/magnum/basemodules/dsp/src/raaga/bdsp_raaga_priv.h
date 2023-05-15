/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/40 $
 * $brcm_Date: 2/6/13 7:17p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_priv.h $
 * 
 * Hydra_Software_Devel/40   2/6/13 7:17p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/39   2/4/13 12:45p nihar
 * SW7425-4229: Adding support for BDSP Version - Merging changes to
 * Hydra_Software_Devel Branch
 * 
 * Hydra_Software_Devel/BDSP_VERSION_BRANCH/2   1/30/13 11:52a nihar
 * SW7425-4229: Adding support for BDSP Version - Modification after Jon's
 * feedback.
 * 
 * Hydra_Software_Devel/BDSP_VERSION_BRANCH/1   1/29/13 5:10p nihar
 * SW7425-4229: Adding support for BDSP Version
 * 
 * Hydra_Software_Devel/SW7425-4216/6   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/38   11/26/12 1:21p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/5   1/8/13 4:58p gprasad
 * SW7425-4216: Merging mainline changes to rbuf capture branch as of
 * 01/08/2012
 * 
 * Hydra_Software_Devel/SW7425-4216/4   12/4/12 5:07p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/SW7425-4216/3   11/29/12 1:36p gprasad
 * SW7425-4216: Fixing the compile error
 * 
 * Hydra_Software_Devel/SW7425-4216/2   11/28/12 4:26p gprasad
 * SW7425-4216: Added capture create and destroy apis for configuring
 * capture
 * 
 * Hydra_Software_Devel/SW7425-4216/1   11/12/12 10:41p gprasad
 * SW7425-4216: Initial changes for ring buffer capture - verified to be
 * working for single decode with raaga test
 * 
 * Hydra_Software_Devel/37   10/19/12 2:42p ananthan
 * SW7425-4031: Raaga reset code needs an upgrade Macro to fix the build
 * problem. Needs to be fixed once a fix is available on the bint
 * routine.
 * 
 * Hydra_Software_Devel/36   10/17/12 3:49p kavitha
 * SW7425-4031: Pulling in reset changes to mainline.
 * 
 * Hydra_Software_Devel/BUS_ERROR_FIX/1   10/3/12 6:04p kavitha
 * SW7425-4031: Updates to the reset logic. Updated reset logic masks the
 * watchdog interrupt. So, post reset we need to unmask the required
 * interrupts.
 * 
 * Hydra_Software_Devel/35   9/17/12 2:12p gprasad
 * SW7425-3952: Fixing build errors with raaga test for emulation build
 * 
 * Hydra_Software_Devel/34   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/33   3/30/12 12:31a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/32   3/20/12 6:15p gprasad
 * SW7429-96: DSP gets "BMEM: Back guard violation" when running apex
 * 
 * Hydra_Software_Devel/31   3/1/12 8:49p ashoky
 * SW7231-585: Changing the RDQ implementation from DRAM based to RDB
 * based.
 * 
 * Hydra_Software_Devel/30   2/22/12 7:25p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/29   2/15/12 5:41p gprasad
 * FWRAA-377: fixing the rbuf setting decalaration
 * 
 * Hydra_Software_Devel/28   2/15/12 3:04p ashoky
 * SW7425-2308: Adding interface to put audio firmware in audio gap fill
 * mode.
 * 
 * Hydra_Software_Devel/27   2/8/12 8:19p ashoky
 * SW7346-598: Removing dynamic mem-alloc/free from decode start stop
 * time. Moving it to device  open/close.
 * 
 * Hydra_Software_Devel/26   1/27/12 1:20p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/25   1/3/12 2:23p ashoky
 * SW7435-5: Adding support for 2nd DSP into mainline.
 * 
 * Hydra_Software_Devel/24   11/16/11 5:46p gprasad
 * SW7425-1169: Confirm audio FW supports 6 simultaneous audio pass-thru
 * for transcoder
 * 
 * Hydra_Software_Devel/23   10/20/11 5:43p gprasad
 * FWRAA-359: Add support for ring buffer capture for cert testing
 * 
 * Hydra_Software_Devel/22   8/18/11 12:53p gprasad
 * SW7422-400: Updating the APIs with review comments
 * 
 * Hydra_Software_Devel/21   8/17/11 12:51p gprasad
 * FWRAA-280: Syncing basemodules and Raaga host interface files
 * 
 * Hydra_Software_Devel/20   7/4/11 6:35p gprasad
 * SW7422-400, SW7422-373: Raaga Debug support and GetInfo modifications
 * 
 * Hydra_Software_Devel/19   6/17/11 3:42p gautamk
 * SW7422-373: [7422] Adding GetInfo routines for AudioType,
 * AudioProcessing, and AudioEncode support in BDSP
 * 
 * Hydra_Software_Devel/18   6/16/11 6:04p gautamk
 * SW7422-369: [7422] Adding support for mulitple context for watchdog
 * 
 * Hydra_Software_Devel/17   6/15/11 5:54p gautamk
 * SW7425-683: [7425] Adding support for Power management in BDSP
 * 
 * Hydra_Software_Devel/16   6/15/11 4:18p gautamk
 * SW7422-357 : [7425] Adding support for new API for getStageInputIndex
 * 
 * Hydra_Software_Devel/15   6/14/11 3:06p gautamk
 * SW7422-357 : [7425] Merging to mainline
 * 
 * Hydra_Software_Devel/14   6/8/11 5:16p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/13   6/3/11 11:59a srajapur
 * SW7422-357 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/12   6/2/11 8:08p srajapur
 * SW7422-358 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/11   5/31/11 5:39p srajapur
 * SW7422-358 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/10   5/25/11 5:22p srajapur
 * SW7422-358: [7425] Adding support for MS11  in BDSP
 * 
 * Hydra_Software_Devel/9   5/20/11 7:05p srajapur
 * SW7422-358: [7425] Added support for MS11  in BDSP
 * 
 * Hydra_Software_Devel/8   5/16/11 10:08p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/7   5/11/11 5:31p srajapur
 * SW7422-374 : [7425] Incorporate changes to BDSP "details" structures
 * from code review feedback.
 * 
 * Hydra_Software_Devel/6   4/14/11 5:35p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/5   4/14/11 2:26a jgarrett
 * SW7425-347: Fixing build errors
 * 
 * Hydra_Software_Devel/4   4/13/11 8:22p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/3   4/12/11 2:24p srajapur
 * SW7422-419:[7425]Adding BDSP_Raaga_GetFwDwnldInfo and
 * BDSP_Raaga_StartDsp API to support authentication
 * 
 * Hydra_Software_Devel/2   4/6/11 2:45p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   4/6/11 2:29p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/15   3/15/11 3:00p gskerl
 * SW7422-146: Made declaration of BDSP_sVomTable to be const
 * 
 * Hydra_Software_Devel/14   3/15/11 11:55a gskerl
 * SW7422-146: Moved external declarations from other .c files into
 * bdsp_raaga_priv.h
 * 
 * Hydra_Software_Devel/13   3/8/11 9:15a srajapur
 * SW7422-294 : [7422]Adding watchdog recovery support in BASE DSP
 * 
 * Hydra_Software_Devel/12   2/26/11 12:41a srajapur
 * SW7422-293:[7422] Adding support for encode algorithms
 * 
 * Hydra_Software_Devel/11   2/5/11 4:25a srajapur
 * SW7422-247 : [7422] Adding Generic passthrul audio processing support.
 * 
 * Hydra_Software_Devel/10   1/27/11 3:13a gautamk
 * SW7422-191: changing const char[] to const char*
 * 
 * Hydra_Software_Devel/9   1/26/11 3:18a gautamk
 * SW7422-191: Allocating FW debug buffer on basis of runtime Parrameter
 * enableDebugLog of BDSP_RaagaSettings.
 * 
 * Hydra_Software_Devel/8   1/25/11 2:24a gautamk
 * SW7422-191:Adding error recovery code for Raaga Open
 * 
 * Hydra_Software_Devel/7   1/20/11 8:12a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/6   1/19/11 7:16a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/5   1/19/11 2:16a gautamk
 * SW7422-191: putting all the Details of Audio codec in a const array and
 * then using it across the code.
 * 
 * Hydra_Software_Devel/4   1/18/11 4:55a gautamk
 * SW7422-191: Implementing code review comments.
 * 
 * Hydra_Software_Devel/3   1/18/11 4:19a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/2   1/13/11 5:26a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/1   12/15/10 6:47p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 * Hydra_Software_Devel/1   12/15/10 2:01p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/

#ifndef BDSP_RAAGA_PRIV_H_
#define BDSP_RAAGA_PRIV_H_

#include "bdsp.h"
#include "../src/bdsp_priv.h" /* FIXME: Remove the priv file dependency */
#include "bdsp_raaga.h"
#include "blst_slist.h"
#include "bdsp_raaga_img.h"
#include "bdsp_raaga_fw.h"
#include "bdsp_raaga_types.h"
#include "bdsp_raaga_fwinterface_priv.h"
#include "bdsp_raaga_mm_priv.h"
#include "bdsp_raaga_util.h"
#include "bdsp_audio_task.h"
#include "bdsp_raaga_cmdresp_priv.h"
#include "bdsp_raaga_fwdownload_priv.h"


BDBG_OBJECT_ID_DECLARE(BDSP_Raaga);
BDBG_OBJECT_ID_DECLARE(BDSP_RaagaContext);
BDBG_OBJECT_ID_DECLARE(BDSP_RaagaTask);
BDBG_OBJECT_ID_DECLARE(BDSP_RaagaExternalInterrupt);
BDBG_OBJECT_ID_DECLARE(BDSP_RaagaRdbRegister);




#define BDSP_RAAGA_PREBOOT_MAILBOX_PATTERN			0xA5A5
#define BDSP_RAAGA_DMA_BUSY							1
#ifdef FIREPATH_BM
#define BDSP_RAAGA_DMA_TIMEOUT_COUNT				50000
#else
#define BDSP_RAAGA_DMA_TIMEOUT_COUNT				500
#endif
#define BDSP_RAAGA_DMA_SIZE_PER_TRANSFER			0xFFFF

#define BDSP_RAAGA_EVENT_TIMEOUT_IN_MS               400
#define BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS    400

/* SW7425-4031: Interrim HACK! To be removed once the bint.h support is available 
                Added to address short term build problems */ 
#define BINT_GetCallbackStatus_isr BINT_GetCallbackStatus


/***** DSPCHN_PRIV_H*****/
/********* New Changes***************/


/* Inter-channel delay user param buffer size */
#define BDSP_RAAGA_DBG_BUF_SIZE     2*1024*1024

/* Number of extra FIFOS that can be used below the actual FIFO 0 */
#define BDSP_RAAGA_NUM_EXTRA_FIFOS		(uint32_t)40

#define BDSP_RAAGA_FIFO_0_INDEX         (int32_t)-BDSP_RAAGA_NUM_EXTRA_FIFOS

#define	BDSP_RAAGA_DEBUG_FIFO_START_INDEX	17
/* It should till the point where debug fifos start */
#define BDSP_RAAGA_NUM_FIFOS            	(BDSP_RAAGA_DEBUG_FIFO_START_INDEX+BDSP_RAAGA_NUM_EXTRA_FIFOS)
/* Fifos actually needed for tasks */
#define BDSP_RAAGA_MAX_TASK_SUPPORTED		16
/* 2 FIFOs (Async and Sync Queues per task) */
#define BDSP_RAAGA_MAX_TASK_FIFO_REQUIRED	(2*BDSP_RAAGA_MAX_TASK_SUPPORTED)

#define BDSP_RAAGA_NUM_PTR_PER_FIFO     5

/* Hardcode cmd queue as FIFO 0 */
#define BDSP_RAAGA_FIFO_CMD     (BDSP_RAAGA_FIFO_0_INDEX+0)
/* Hardcode response queue which doesn't have any task id associated */
#define BDSP_RAAGA_FIFO_GENERIC_RSP     (BDSP_RAAGA_FIFO_0_INDEX+1)
#define BDSP_RAAGA_FIFO_DEBUG           17
#define BDSP_RAAGA_FIFO_INVALID         ((unsigned int)(-1))

#define BDSP_RAAGA_GET_TASK_INDEX(taskId) (taskId -BDSP_RAAGA_TASK_ID_START_OFFSET)

#define BDSP_RAAGA_INVALID_INDEX	                (unsigned int)-1

#define BDSP_RAAGA_DSP_P_FIFO_PDQ   15
#define BDSP_RAAGA_DSP_P_FIFO_PRQ   16
#define BDSP_RAAGA_DSP_P_FIFO_DSQ   14
#define BDSP_RAAGA_MAX_INTERRUPTS_PER_DSP	32
#define BDSP_RAAGA_RDB_REGISTERS	15

#ifdef BDSP_FW_RBUF_CAPTURE
/* Specific to FW Ring Buffer capture required for their unit testing */
typedef enum BDSP_P_RbufType 
{
    BDSP_P_RbufType_eDecode,
    BDSP_P_RbufType_ePassthru,
    BDSP_P_RbufType_eMixer,
    BDSP_P_RbufType_eEncoder,
    BDSP_P_RbufType_eTranscode,
    BDSP_P_RbufType_eSM 
}BDSP_P_RbufType;

typedef struct BDSP_P_RbufCapture
{
    uint8_t (*rbuf_init)(BREG_Handle hRegister, BMEM_Handle hMem);
    uint8_t (*rbuf_uninit)(void);
    uint8_t (*rbuf_capture_channel_start)(uint32_t ui32Channel, uint32_t ui32Path, uint32_t ui32RbufId, BDSP_P_RbufType eRbufType);
    uint8_t (*rbuf_capture_stop_channel)(uint32_t ui32Channel, uint32_t ui32Path, uint32_t ui32RbufId, BDSP_P_RbufType eRbufType);

}BDSP_P_RbufCapture;

extern BDSP_P_RbufCapture Rbuf_Setting;
  
void BDSP_P_RbufSetup(
	BDSP_P_RbufCapture sRbufCap
);

#endif

#define BDSP_Raaga_P_ResetRaagaCore_isr BDSP_Raaga_P_ResetRaagaCore 

/***************************************************************************
Summary:
	This structure is one to one mapping between Exec_Image_Id and the Exec_Id 
	(BAF Ids) common between PI and FW. This will also contain the sizes of the 
	Exec_image. 
		Since for the Exec_Image_Id of Decode table, there is no Exec_Id, so it will store 
	the if its already downloaded.

	Also interframeImgId stores the Image id of interframe Fw image for each ExecId.
***************************************************************************/
typedef struct BDSP_Raaga_P_ImgIdMappingArrays
{
	uint32_t    codeImgId[BDSP_AF_P_AlgoId_eMax];
	uint32_t    ui32CodeSize[BDSP_AF_P_AlgoId_eMax];
	uint32_t    tableImgId[BDSP_AF_P_AlgoId_eMax];	
	uint32_t    tableSize[BDSP_AF_P_AlgoId_eMax];	
	uint32_t    interframeImgId[BDSP_AF_P_AlgoId_eMax];
	uint32_t	interframeSize[BDSP_AF_P_AlgoId_eMax];
} BDSP_Raaga_P_ImgIdMappingArrays;

#define BDSP_RAAGA_ALIGN(x,alignbits)               (((x) + \
                                                 ((1 << (alignbits)) - 1)) & \
                                                 ~((1 << (alignbits)) - 1))
                                                 
struct BDSP_RaagaTask;


typedef struct BDSP_Raaga_P_TaskInfo
{
    bool    taskId[BDSP_RAAGA_MAX_DSP][BDSP_RAAGA_MAX_FW_TASK_PER_DSP];
    struct BDSP_RaagaTask    *pRaagaTask[BDSP_RAAGA_MAX_DSP][BDSP_RAAGA_MAX_FW_TASK_PER_DSP];       
    BKNI_MutexHandle taskIdMutex;    
}BDSP_Raaga_P_TaskInfo;




typedef struct BDSP_Raaga
{
    BDBG_OBJECT(BDSP_Raaga)
    BDSP_Device device;
    BDSP_RaagaSettings settings;
    BCHP_Handle chpHandle;
    BREG_Handle regHandle;
    BMEM_Handle memHandle;
    BINT_Handle intHandle;
    uint32_t dspOffset[BDSP_RAAGA_MAX_DSP];
    BINT_CallbackHandle     ackCallbackHandle[BDSP_RAAGA_MAX_DSP];    /* This will install the Callback for Ping the DSp*/    
    BDSP_Raaga_P_Mit				mit;			
                                /* Structure which contains the the information 
                                    of downloaded executable */
                                    
    BDSP_Raaga_P_ImgIdMappingArrays	imgIdMappingArrays;	
                                /* This structure contains the mapping 
                                    information between Exec Ids and its image 
                                    id. Also, It have the size information of 
                                    each image Id */   
    BDSP_Raaga_P_MemoryRequest   memReq;    
                                /* Memory request structure */                                
    BDSP_Raaga_P_MemoryGrant     memInfo; 
                                /* Memory Grant structure */                                                                
                                
    BDSP_Raaga_P_TaskInfo taskDetails;

    bool    dspFifo[BDSP_RAAGA_MAX_DSP][BDSP_RAAGA_NUM_FIFOS];
    BKNI_MutexHandle fifoIdMutex[BDSP_RAAGA_MAX_DSP]; 
    BKNI_MutexHandle    watchdogMutex;
    BDSP_Raaga_P_MsgQueueHandle      hCmdQueue[BDSP_RAAGA_MAX_DSP];      /* Cmd queue handle*/
    BDSP_Raaga_P_MsgQueueHandle      hGenRspQueue[BDSP_RAAGA_MAX_DSP];      /* Generic Response queue handle*/ 
    BINT_CallbackHandle     		hWatchdogCallback[BDSP_RAAGA_MAX_DSP];      
    bool                    deviceWatchdogFlag;      
    BLST_S_HEAD(BDSP_RaagaContextList, BDSP_RaagaContext) contextList;
    bool    powerStandby;/*True if DSP is in standby*/
	/* This is a temp fix for SW7346-598. No mem alloc should be done at stop-start decoder */
	BDSP_Raaga_P_CitBuffers		sCitBuffers;
	bool						dspInterrupts[BDSP_RAAGA_MAX_DSP][BDSP_RAAGA_MAX_INTERRUPTS_PER_DSP];
	BKNI_MutexHandle 			dspInterruptMutex[BDSP_RAAGA_MAX_DSP]; 
	BLST_S_HEAD(BDSP_RaagaExtInterruptList, BDSP_RaagaExternalInterrupt) interruptList;
	/* Rdb register handle */
	bool						rdbRegisters[BDSP_RAAGA_MAX_DSP][BDSP_RAAGA_RDB_REGISTERS];
	BKNI_MutexHandle 			rdbRegistersMutex[BDSP_RAAGA_MAX_DSP]; 
	BLST_S_HEAD(BDSP_RaagaRdbRegisterList, BDSP_RaagaRdbRegister) rdbRegisterList;
    BKNI_MutexHandle 			captureMutex;
}BDSP_Raaga;

/* Handle for External interrupt to DSP */
typedef struct BDSP_RaagaRdbRegister
{
	BDBG_OBJECT(BDSP_RaagaRdbRegister)
	BDSP_RdbRegister	rdbRegister;
	struct BDSP_Raaga *pDevice;
	BLST_S_ENTRY(BDSP_RaagaRdbRegister) node;
	uint32_t	dspIndex;
	uint32_t	numRegisters;
	uint32_t	startIndex;
	BDSP_RdbRegisterInfo RdbRegisterInfo;
}BDSP_RaagaRdbRegister;


/* Handle for External interrupt to DSP */
typedef struct BDSP_RaagaExternalInterrupt
{
	BDBG_OBJECT(BDSP_RaagaExternalInterrupt)
	BDSP_ExternalInterrupt	extInterrupt;
	struct BDSP_Raaga *pDevice;
	BLST_S_ENTRY(BDSP_RaagaExternalInterrupt) node;
	uint32_t	dspIndex;
	BDSP_ExternalInterruptInfo InterruptInfo;
}BDSP_RaagaExternalInterrupt;

typedef struct BDSP_RaagaContext
{
    BDBG_OBJECT(BDSP_RaagaContext)
    BDSP_Context context;
    struct BDSP_Raaga *pDevice;
    BDSP_ContextCreateSettings settings;
    BDSP_ContextInterruptHandlers interruptHandlers;
    BLST_S_ENTRY(BDSP_RaagaContext) node;
    BLST_S_HEAD(BDSP_RaagaAllocTaskList, BDSP_RaagaTask) allocTaskList;
    BLST_S_HEAD(BDSP_RaagaFreeTaskList, BDSP_RaagaTask) freeTaskList;
    BDSP_Raaga_P_ContextMemoryGrant  contextMemInfo;
    bool                    contextWatchdogFlag;          
    /* TODO: More context-level info */
}BDSP_RaagaContext;

typedef struct BDSP_Raaga_P_TaskCallBacks
{
    BINT_CallbackHandle     hDSPAsync[BDSP_RAAGA_MAX_DSP];  
                                    /* DSP Async Interrupt callback */                                        
    BINT_CallbackHandle     hDSPSync[BDSP_RAAGA_MAX_DSP];  
                                    /* DSP SYNC interrupt callback */    
}BDSP_Raaga_P_TaskCallBacks;

 typedef struct BDSP_RaagaTask
{
    BDBG_OBJECT(BDSP_RaagaTask)
    BDSP_Task task;
    struct BDSP_RaagaContext *pContext;
    BDSP_TaskCreateSettings settings;
    BDSP_AudioInterruptHandlers audioInterruptHandlers;
    bool allocated;
    BLST_S_ENTRY(BDSP_RaagaTask) node;    
    unsigned int    taskId;   
    BDSP_CIT_P_Output	citOutput;
    BDSP_CIT_P_VideoCITOutput   videoCitOutput;        
    BDSP_CIT_P_ScmCITOutput   scmCitOutput;        
    BDSP_Raaga_P_MsgQueueHandle hAsyncMsgQueue; /* Asynchronous message queue 
                                                belonging to this task */
    BDSP_Raaga_P_MsgQueueHandle hSyncMsgQueue; /* Synchronous message queue 
                                               belonging to this task */
    bool isStopped;  /* TRUE : If the stop command for this task has been sent,
                                                But the Processing of stop is still under process. Keep it in true state untill the hDspCh is started*/              
    unsigned int lastEventType;
    unsigned int    masterTaskId;    
    BKNI_EventHandle hEvent;
    BDSP_Raaga_P_TaskCallBacks    interruptCallbacks;
    unsigned commandCounter;
    BDSP_CTB_Output ctbOutput;    
    bool    decLocked;
    bool    paused;
    BDSP_Raaga_P_TaskMemoryInfo taskMemGrants;                  /* Memory for contiguous Async Msgs */
    uint32_t        eventEnabledMask;  /* Contains information abt. event ids already enabled */
    BDSP_TaskSchedulingMode schedulingMode;        
    void                                *pFeedbackBuffer;               /* Feedback buffer between Tasks(Master writes-Slaves read) */
    BDSP_Raaga_P_MsgQueueHandle      hPDQueue;      /* Picture Delivery queue(PDQ)*/
    BDSP_Raaga_P_MsgQueueHandle      hPRQueue;      /* Picture Release queue(PRQ)*/ 
    BDSP_Raaga_P_MsgQueueHandle      hDSQueue;      /* Display queue(DSQ)*/    
	/* Handles for video encoder queue's */	
    BDSP_Raaga_P_MsgQueueHandle      hRDQueue;      /* Raw Picture Delivery queue(RDQ)*/
    BDSP_Raaga_P_MsgQueueHandle      hRRQueue;      /* Raw Picture Release queue(RRQ)*/ 
    /* Capture information for the task */
    BLST_S_HEAD(BDSP_RaagaCaptureList, BDSP_RaagaCapture) captureList;
} BDSP_RaagaTask;

/***************************************************************************
Summary:
Capture pointers structure
***************************************************************************/
typedef struct BDSP_RaagaCapturePointerInfo
{
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER outputBufferPtr; /* Structure containing pointers to the output buffers 
                                                   from which the data has to be captured */
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER captureBufferPtr; /* Structure containing the pointers to the intermediate 
                                                    buffer into which the captured data is written */
    uint32_t shadowRead; /* The shadow read pointer for the output buffer */
    uint32_t lastWrite; /* The last value of the write pointer; will be used for capture error detection*/
} BDSP_RaagaCapturePointerInfo;

BDBG_OBJECT_ID_DECLARE(BDSP_RaagaCapture);

typedef struct BDSP_RaagaCapture
{
    BDBG_OBJECT(BDSP_RaagaCapture)
    BDSP_AudioCapture    capture;
    BDSP_RaagaTask  *pTask;
    BDSP_Raaga      *pDevice;
    
    BLST_S_ENTRY(BDSP_RaagaCapture) node;
    
    BMEM_Handle     hHeap; /* Heap from which the capture buffers need to be allocated */
    
    BDSP_AF_P_BufferType eBuffType; /* The buffer type of the the output buffer (RAVE, FMM, DRAM etc ...) */
    bool updateRead; /* If true then the read pointers of the output buffer are updated in the capture 
                          thread. This can be set to true when there is not consumer for the output data */
    bool enabled; /* Flag to indicate whether the capture is enabled or disabled */
    uint8_t numBuffers; /* Number of valid buffers */
    uint8_t maxBuffers; /* Maximum number of buffers */
    
    BDSP_RaagaCapturePointerInfo capPtrs[BDSP_AF_P_MAX_CHANNELS]; /* Capture pointer info for all the output capture ports */
} BDSP_RaagaCapture;
 
 BDBG_OBJECT_ID_DECLARE(BDSP_RaagaTaskStageInput);
 
 typedef struct BDSP_RaagaTaskStageInput
 {
    BDBG_OBJECT(BDSP_RaagaTaskStageInput)
    BDSP_TaskStageInput        stageInput;
    unsigned branchId;
    unsigned stageId;
    BDSP_CIT_P_FwStgSrcDstType      inputType;      /* input type */
    unsigned inputIndex;
    BDSP_AF_P_sIO_BUFFER  *pIoBuffer;
    BDSP_AF_P_sIO_GENERIC_BUFFER *pIoGenericBuffer;            
 }BDSP_RaagaTaskStageInput;

 typedef struct BDSP_Raaga_P_VideoCodecDetails
{
    BDSP_VideoType  videoType;
    const  char     *videoTypeName;
    bool    codecSupported;
    void    *pDefaultUserConfig;
    size_t  userConfigSize;
    size_t  statusBufferSize;
    BDSP_Img_Id codeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];
    BDSP_Img_Id tableImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];    
    BDSP_Img_Id interframeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];        
}BDSP_Raaga_P_VideoCodecDetails;


typedef struct BDSP_Raaga_P_AudioCodecPTDetails
{
    BDSP_Img_Id passthrucodeImgId;
    BDSP_Img_Id passthrutableImgId;
    BDSP_Img_Id passthruinterframeImgId;
}BDSP_Raaga_P_AudioCodecPTDetails;

 typedef struct BDSP_Raaga_P_AudioCodecDetails
{
    BDSP_AudioType  audioType;
    const  char     *audioTypeName;
    bool    codecSupported;
    bool    passthruSupportedWithoutLicense;    
    void    *pDefaultUserConfig;
    size_t  userConfigSize;
    size_t  statusBufferSize;
    BDSP_Img_Id codeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];
    BDSP_Img_Id tableImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];    
    BDSP_Img_Id interframeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];        
    BDSP_Img_Id passthruIdsCode;
    BDSP_Img_Id passthruIdsInterframe;    
    const BDSP_Raaga_P_AudioCodecPTDetails *pCodecSpecPtDetails;
}BDSP_Raaga_P_AudioCodecDetails;

typedef struct BDSP_Raaga_P_AudioProcessingDetails
{
    BDSP_AudioProcessing  audioType;
    const  char     *audioProcessingTypeName;
    bool    processingSupported;
    void    *pDefaultUserConfig;
    size_t  userConfigSize;
    size_t  statusBufferSize;
    BDSP_Img_Id codeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];
    BDSP_Img_Id tableImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];    
    BDSP_Img_Id interframeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];      
}BDSP_Raaga_P_AudioProcessingDetails;

typedef struct BDSP_Raaga_P_AudioEncodeDetails
{
    BDSP_AudioEncode  encodeType;
    const  char     *audioEncodeTypeName;
    bool    EncodeSupported;
    void    *pDefaultUserConfig;
    size_t  userConfigSize;
    size_t  statusBufferSize;
    BDSP_Img_Id codeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];
    BDSP_Img_Id tableImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];    
    BDSP_Img_Id interframeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];      
}BDSP_Raaga_P_AudioEncodeDetails;

typedef struct BDSP_Raaga_P_ScmDetails
{
    BDSP_ScmType  scmType;
    const  char     *scmTypeName;
    bool    scmSupported;
    void    *pDefaultUserConfig;
    size_t  userConfigSize;
    size_t  statusBufferSize;
    BDSP_Img_Id codeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO]; /* TODO: Vijay Can this footprint be reduced. SCM context not going to have these many nodes */
    BDSP_Img_Id tableImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];    
    BDSP_Img_Id interframeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];      
}BDSP_Raaga_P_ScmDetails;
typedef struct BDSP_Raaga_P_VideoEncodeDetails
{
    BDSP_VideoEncode  encodeType;
    const  char     *videoEncodeTypeName;
    bool    EncodeSupported;
    void    *pDefaultUserConfig;
    size_t  userConfigSize;
    size_t  statusBufferSize;
    BDSP_Img_Id codeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];
    BDSP_Img_Id tableImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];    
    BDSP_Img_Id interframeImgId[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];      
}BDSP_Raaga_P_VideoEncodeDetails;

void BDSP_Raaga_P_Close(
    void *pDeviceHandle);

void BDSP_Raaga_P_GetDefaultContextSettings(
    void *pDeviceHandle, 
    BDSP_ContextType contextType,    
    BDSP_ContextCreateSettings *pSettings);

BERR_Code BDSP_Raaga_P_CreateContext(
    void *pDeviceHandle,
    const BDSP_ContextCreateSettings *pSettings,
    BDSP_ContextHandle *pContextHandle);

void BDSP_Raaga_P_GetStatus(
    void *pDeviceHandle,
    BDSP_Status *pStatus);

void BDSP_Raaga_P_DestroyContext(
    void *pContextHandle);

void BDSP_Raaga_P_GetDefaultTaskSettings(
    void *pContextHandle, 
    BDSP_TaskCreateSettings *pSettings);

BERR_Code BDSP_Raaga_P_CreateTask(
    void *pContextHandle, 
    const BDSP_TaskCreateSettings *pSettings, 
    BDSP_TaskHandle *pTask);

BERR_Code BDSP_Raaga_P_Open(
    void *pDeviceHandle);

void BDSP_Raaga_P_DestroyTask(
    void *pTaskHandle);

BERR_Code BDSP_Raaga_P_StartTask(
    void *pTaskHandle);

BERR_Code BDSP_Raaga_P_StopTask(
    void *pTaskHandle);


BERR_Code BDSP_Raaga_P_GetFWSize (
    void *pDeviceHandle, 
    BDSP_Img_Id firmware_id, 
    uint32_t *size);

void BDSP_Raaga_P_UpdateMitEntry(
		void		*pDeviceHandle,
		BDSP_AF_P_AlgoId	eExecId,
		uint32_t		codePtr,
		uint32_t 		ui32CodeSize,
		uint32_t		tblPtr,		
		uint32_t		tblSize);

BERR_Code BDSP_Raaga_P_GetStageSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    void *pSettingsBuffer, 
    size_t settingsSize);

/* Get video stage setting */

BERR_Code BDSP_Raaga_P_GetVideoStageSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    void *pSettingsBuffer, 
    size_t settingsSize);

BERR_Code BDSP_Raaga_P_SetVideoStageSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const void *pSettingsBuffer, 
    size_t settingsSize);

BERR_Code BDSP_Raaga_P_SetStageSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const void *pSettingsBuffer, 
    size_t settingsSize);

BERR_Code BDSP_Raaga_P_AddStageInput(
    void *pTaskHandle, 
    const BDSP_TaskAddStageInputSettings *pSettings,
    BDSP_TaskStageInputHandle *pHandle);

BERR_Code BDSP_Raaga_P_RemoveStageInput(void *pStageInputHandle);

BERR_Code BDSP_Raaga_P_GetStageInputIndex(
    void *pStageInputHandle,
    unsigned *inputIndex);


BERR_Code BDSP_Raaga_P_GetDatasyncSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskDatasyncSettings *pSettings);

BERR_Code BDSP_Raaga_P_SetDatasyncSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_AudioTaskDatasyncSettings *pSettings);

BERR_Code BDSP_Raaga_P_GetDatasyncSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskDatasyncSettings *pSettings);

BERR_Code BDSP_Raaga_P_SetDatasyncSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_AudioTaskDatasyncSettings *pSettings);

BERR_Code BDSP_Raaga_P_GetVideoEncodeDatasyncSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_VideoEncodeTaskDatasyncSettings *pSettings);

BERR_Code BDSP_Raaga_P_SetVideoEncodeDatasyncSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_VideoEncodeTaskDatasyncSettings *pSettings);
	

BERR_Code BDSP_Raaga_P_GetVideoEncodeDatasyncSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_VideoEncodeTaskDatasyncSettings *pSettings);

BERR_Code BDSP_Raaga_P_SetVideoEncodeDatasyncSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_VideoEncodeTaskDatasyncSettings *pSettings);
BERR_Code BDSP_Raaga_P_SetTsmSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_AudioTaskTsmSettings *pSettings);

BERR_Code BDSP_Raaga_P_GetTsmSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskTsmSettings    *pSettings);

BERR_Code BDSP_Raaga_P_GetStageStatus(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    void *pStatusBuffer, 
    size_t statusSize);

BERR_Code BDSP_Raaga_P_GetTsmStatus_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskTsmStatus *pStatusBuffer);

BERR_Code BDSP_Raaga_P_GetDatasyncStatus(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskDatasyncStatus *pStatusBuffer);

BERR_Code BDSP_Raaga_P_Pause(
    void *pTaskHandle);

BERR_Code BDSP_Raaga_P_Resume(
    void *pTaskHandle);

BERR_Code BDSP_Raaga_P_Advance(
    void *pTaskHandle, 
    unsigned ms);

BERR_Code BDSP_Raaga_P_LoadScm(
    void *pDeviceHandle,
    unsigned uiDspIndex,
	BDSP_ScmType uiScmIndex);
BERR_Code BDSP_Raaga_P_InitHardware(    
    void *pDeviceHandle,
    unsigned uiDspIndex);

BERR_Code BDSP_Raaga_P_GetPictureCount_isr(
    void *pTaskHandle,
    unsigned *pPictureCount);
BERR_Code BDSP_Raaga_P_PeekAtPicture_isr(
    void *pTaskHandle, 
    unsigned index,
    BXDM_Picture **pUnifiedPicture);
BERR_Code BDSP_Raaga_P_GetNextPicture_isr(
    void *pTaskHandle,
    BXDM_Picture **pUnifiedPicture);
BERR_Code BDSP_Raaga_P_ReleasePicture_isr(
    void *pTaskHandle,
    BXDM_Picture *pUnifiedPicture);
BERR_Code BDSP_Raaga_P_GetPictureDropPendingCount_isr(
    void *pTaskHandle, 
    unsigned *pPictureDropPendingCount);
BERR_Code BDSP_Raaga_P_RequestPictureDrop_isr(
    void *pTaskHandle,
    unsigned *pPictureDropRequestCount);

BERR_Code BDSP_Raaga_P_GetPictureBuffer_isr(
    void *pTaskHandle,
    BVENC_VF_sPicParamBuff **pPictureParmBuf);

BERR_Code BDSP_Raaga_P_PutPicture_isr(
    void *pTaskHandle,
    uint32_t	ui32PPBAddress
    );

BERR_Code BDSP_Raaga_P_DisplayInterruptEvent_isr(
    void *pTaskHandle);
BERR_Code BDSP_Raaga_P_PowerStandby(
    void *pDeviceHandle,
    BDSP_StandbySettings 	*pSettings);
BERR_Code BDSP_Raaga_P_PowerResume(
    void *pDeviceHandle);

BERR_Code BDSP_Raaga_P_ResetRaagaCore(
   void *pDeviceHandle,
   uint32_t	ui32DspIndex);

uint32_t BDSP_Raaga_P_ResetHardware(
   void *pDeviceHandle,
   uint32_t	ui32DspIndex);


uint32_t BDSP_Raaga_P_ResetHardware_isr(
   void *pDeviceHandle,
   uint32_t	ui32DspIndex);

uint32_t BDSP_Raaga_P_RestoreInterrupts(
   void *pDeviceHandle,
   uint32_t	ui32DspIndex);


uint32_t BDSP_Raaga_P_RestoreInterrupts_isr(
   void *pDeviceHandle,
   uint32_t	ui32DspIndex);


void   BDSP_Raaga_P_GetAudioTypeInfo(
     BDSP_AudioType type, /* [in] */
     const BDSP_AudioTypeInfo **pInfo /* [out] */
    );

void   BDSP_Raaga_P_GetAudioProcessingInfo(
    BDSP_AudioProcessing type, /* [in] */
    const BDSP_AudioProcessingInfo **pInfo /* [out] */
    );

void   BDSP_Raaga_P_GetAudioEncodeInfo(
     BDSP_AudioEncode type, /* [in] */
     const BDSP_AudioEncodeInfo **pInfo /* [out] */
    );

BERR_Code BDSP_Raaga_P_SendScmCommand(	  
	void *pTaskHandle,	  
	BDSP_Raaga_P_SCM_CmdPayload *pScmCmdPayload
	);
BERR_Code BDSP_Raaga_P_AudioGapFill(
    void *pTaskHandle);

BERR_Code BDSP_Raaga_P_AllocateExternalInterrupt(
	void 		*pDeviceHandle, /* in */
	uint32_t 	dspIndex, /* in */
	BDSP_ExternalInterruptHandle *pInterruptHandle	/* out */
	);

BERR_Code BDSP_Raaga_P_FreeExternalInterrupt(
	void *pInterruptHandle
	);

BERR_Code BDSP_Raaga_P_GetExternalInterruptInfo(
	void *pInterruptHandle, 
	BDSP_ExternalInterruptInfo **pInfo
	);
BERR_Code BDSP_Raaga_P_AllocateRdbRegisters(
	void *pDeviceHandle, 
	uint32_t dspIndex, 
	uint32_t numRegs, 
	BDSP_RdbRegisterHandle *pRdbRegisterHandle /* out */
	);

BERR_Code BDSP_Raaga_P_FreeRdbRegisters( 
	void  *pRdbRegisterHandle
	);

BERR_Code BDSP_Raaga_P_GetRdbRegistersInfo(
	void *pRdbRegisterHandle, 
	BDSP_RdbRegisterInfo **pInfo	/* out */
	);

BERR_Code BDSP_Raaga_P_UnresetRaagaCore(
	void *pDeviceHandle,
	unsigned uiDspIndex
	);
	
BERR_Code BDSP_Raaga_P_InitRaagaCore (
	void *pDeviceHandle,
	unsigned uiDspIndex
	);
BERR_Code BDSP_Raaga_P_InitAudioCaptureInfo(
    BDSP_RaagaCapture *pRaagaCapture, /* [in] capture handle*/
    const BDSP_AudioCaptureCreateSettings *pSettings /* [in] capture create settings */
    );


BERR_Code BDSP_Raaga_P_GetAudioOutputPointers(
    BDSP_CIT_P_FwStgSrcDstDetails *pDstDetails, /* [in] Desination details of the output port to be captured */
    BDSP_RaagaCapture *pRaagaCap /* [out] Capture Handle */
    );
    
BERR_Code BDSP_Raaga_P_AudioCaptureCreate(
    void *pTaskHandle, 
    const BDSP_AudioCaptureCreateSettings *pCaptureCreateSettings,
    BDSP_AudioCaptureHandle *pCapture);

void BDSP_Raaga_P_AudioCaptureDestroy (
    void *pCapHandle
    );

BERR_Code BDSP_Raaga_P_AudioCaptureAdd(
    void *pCapHandle,
    void *pTaskHandle, 
    const BDSP_AudioTaskAddCaptureSettings *pSettings
    );

void BDSP_Raaga_P_AudioCaptureRemove(
    void *pCapHandle,
    void *pTaskHandle
    );

/***************************************************************************
Summary:
Function to copy data from output buffer to intermediate buffers
***************************************************************************/
BERR_Code BDSP_Raaga_P_ProcessAudioCapture(
    void *pDevice /* [in] task handle */
    );

/***************************************************************************
Summary:
Function that returns the pointers from where the data can be read and 
written to file.
***************************************************************************/
BERR_Code BDSP_Raaga_P_AudioCaptureGetBuffer(
    void *pCapHandle,   /* [in] capture handle */
    BDSP_BufferDescriptor *pBuffers     /* [out] pointer to the structure 
                        with address and size of the intermediate buffer from 
                        which the data can be written into the file */
    );
    
/***************************************************************************
Summary:
Function to update the read pointers for the intermediate buffers.
***************************************************************************/
BERR_Code BDSP_Raaga_P_AudioCaptureConsumeData(
    void *pCapture, /* [in] capture handle */
    uint32_t numBytes /* [in] sizes of data read from each intermediate buffer */
    );

/***************************************************************************
Summary:
Function to determine the buffer depth as one continuous chunk based on the 
buffer type and a specified read pointer
***************************************************************************/
uint32_t BDSP_Raaga_P_GetAudioBufferDepthLinear(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, /* [in] pointer to circular buffer */
    uint32_t ui32ShadowRead, /* [in] read pointer */
    BDSP_AF_P_BufferType eType, /* [in] buffer type */
    BREG_Handle hReg
    ); /* [in] register handle */
    
/***************************************************************************
Summary:
Function to determine the free space as one continuous chunk based on the 
buffer type
***************************************************************************/
uint32_t BDSP_Raaga_P_GetAudioCaptureBufferFreeSpaceLinear(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer /* [in] pointer to circular buffer */
    );

/***************************************************************************
Summary:
Function to detect a capture error by checking if the write pointer has 
overtaken the shadow read pointer
***************************************************************************/
bool BDSP_Raaga_P_DetectAudioCaptureError(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, /* [in] pointer to circular buffer */
    uint32_t ui32ShadowRead, /* shadow read pointer value */
    uint32_t ui32LastWrite, /* last read value of the write pointer */
    BDSP_AF_P_BufferType eType, /* [in] buffer type */
    BREG_Handle hReg /* [in] register handle */
    );
    
/***************************************************************************
Summary:
Function to update the shadow read pointer with the bytes read and snapshot
write pointer
***************************************************************************/
void BDSP_Raaga_P_GetUpdatedShadowReadAndLastWrite(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, /* [in] pointer to circular buffer */
    uint32_t *pShadowRead, /* [in/out] shadow read pointer */
    uint32_t *pLastWrite, /* [out] snapshot of the write pointer */
    BDSP_AF_P_BufferType eType, /* [in] buffer type */
    uint32_t bytesRead, /* [in] number of bytes to update the shadow read with */
    BREG_Handle hReg /* [in] register handle */
    );

/***************************************************************************
Summary:
Function to update the output buffer read pointer
***************************************************************************/
void BDSP_Raaga_P_UpdateReadPointer(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, /* [out] pointer to circular buffer */
    BDSP_AF_P_BufferType eType, /* [in] buffer type */
    uint32_t ui32ReadAddr, /* Value of read pointer to update with */
    BREG_Handle hReg /* [in] register handle */
    );

BERR_Code BDSP_Raaga_P_UnresetRaagaCore(
	void *pDeviceHandle,
	unsigned uiDspIndex
	);
	
BERR_Code BDSP_Raaga_P_InitRaagaCore (
	void *pDeviceHandle,
	unsigned uiDspIndex
	);


/***************************************************************************
Summary: 
    Various global data structures that are shared between source files. 
***************************************************************************/
extern  const BDSP_Raaga_P_AudioProcessingDetails           *BDSP_sAudioProcessingDetails [BDSP_AudioProcessing_eMax];
extern  const BDSP_Raaga_P_AudioCodecDetails                *BDSP_sAudioCodecDetails [BDSP_AudioType_eMax];
extern  const BDSP_Raaga_P_AudioEncodeDetails               *BDSP_sAudioEncodeDetails [BDSP_AudioEncode_eMax];
extern  const BDSP_Raaga_P_VideoCodecDetails                *BDSP_sVideoCodecDetails[BDSP_VideoType_eMax];
extern	const BDSP_Raaga_P_ScmDetails						*BDSP_sScmCodecDetails[BDSP_ScmType_eMax];
extern	const BDSP_Raaga_P_VideoEncodeDetails				*BDSP_sVideoEncodeCodecDetails[BDSP_VideoEncode_eMax];

extern  const BDSP_Raaga_Audio_FrameSyncTsmConfigParams     BDSP_sDefaultFrameSyncTsmSettings;
extern  const BDSP_AF_P_sALGO_EXEC_INFO                     BDSP_sDecAlgoExecInfo [BDSP_AudioType_eMax][BDSP_DecodeMode_eMax];
extern  const BDSP_AF_P_sALGO_EXEC_INFO                     BDSP_sVideoDecAlgoExecInfo [BDSP_VideoType_eMax][BDSP_DecodeMode_eMax];
extern  const BDSP_AF_P_sALGO_EXEC_INFO                     BDSP_sEncAlgoExecInfo [BDSP_AudioEncode_eMax][BDSP_EncodeMode_eSimulMode];
extern  const BDSP_AF_P_sALGO_EXEC_INFO 					BDSP_sVideoEncAlgoExecInfo[BDSP_VideoEncode_eMax][BDSP_EncodeMode_eSimulMode];
extern  const BDSP_AF_P_sALGO_EXEC_INFO                     BDSP_sPpAlgoExecInfo [BDSP_AudioProcessing_eMax][BDSP_DSPCHN_ProcessingMode_eMax];
extern  const BDSP_AF_P_sALGO_EXEC_INFO                     BDSP_sScmAlgoExecInfo [BDSP_ScmType_eMax];
extern  const BDSP_VOM_Algo_Start_Addr                      BDSP_sAlgoStartAddr;
extern  const BDSP_AF_P_sNODE_INFO                          BDSP_sNodeInfo [BDSP_AF_P_AlgoId_eMax];
extern  const BDSP_VOM_Table                                BDSP_sVomTable;
extern const BDSP_AudioTypeInfo sAudioDecodeInfo[BDSP_AudioType_eMax];
extern const BDSP_AudioEncodeInfo sAudioEncodeInfo[BDSP_AudioEncode_eMax];
extern const BDSP_AudioProcessingInfo sAudioProcessingInfo[BDSP_AudioProcessing_eMax];

extern const BDSP_Version BDSP_sRaagaVersion;
#endif /* #ifndef BDSP_RAAGA_PRIV_H_*/

