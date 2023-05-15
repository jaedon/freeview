/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_task.h $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 2/6/13 6:42p $
 *
 * Module Description: Host DSP Task Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/bdsp_task.h $
 * 
 * Hydra_Software_Devel/18   2/6/13 6:42p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/18   2/6/13 6:30p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/5   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/17   12/5/12 7:53a kavitha
 * SW7425-3404: Adding element maxIndependentDelay element as part of the
 * TaskCreateSettings
 * 
 * Hydra_Software_Devel/SW7425-4216/4   1/8/13 4:57p gprasad
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
 * Hydra_Software_Devel/SW7425-4216/1   11/12/12 10:40p gprasad
 * SW7425-4216: Initial changes for ring buffer capture - verified to be
 * working for single decode with raaga test
 * 
 * Hydra_Software_Devel/16   9/12/12 4:32p gprasad
 * SW7425-3061: Renaming the sample rate map parameter
 * 
 * Hydra_Software_Devel/15   9/7/12 12:17p gprasad
 * SW7425-3061, SW7425-3831: Moving sample raate LUT generation out of CIT
 * and - Adding support for initial mute frame fill in RT transcode cases
 * 
 * Hydra_Software_Devel/14   6/19/12 3:15a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/13   2/22/12 7:21p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/12   1/27/12 1:02p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/11   8/19/11 4:02p ashoky
 * SWDTV-8271: [35233] Adding low delay support.
 * 
 * Hydra_Software_Devel/10   6/24/11 1:50p gautamk
 * SW7425-499: [7425] Merging to mainline.
 * 
 * Hydra_Software_Devel/NRT_XCODE_DEVEL/2   6/22/11 6:21p gautamk
 * SW7425-499: [7425] Adding bdsp suport for non-real time transcode
 * cases.
 * 
 * Hydra_Software_Devel/NRT_XCODE_DEVEL/1   6/17/11 6:15p gautamk
 * SW7425-499: [7425] Adding bdsp suport for non-real time transcode
 * cases.
 * 
 * Hydra_Software_Devel/9   6/16/11 1:53p gautamk
 * SW7422-357 : [7425] rehsuffling the defenitions
 * 
 * Hydra_Software_Devel/8   6/15/11 4:21p gautamk
 * SW7422-357 : [7425] Adding support for new API for getStageInputIndex
 * 
 * Hydra_Software_Devel/7   6/14/11 3:13p gautamk
 * SW7422-357 : [7425] Merging to mainline
 * 
 * Hydra_Software_Devel/6   6/3/11 11:59a srajapur
 * SW7422-357 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/5   6/2/11 7:56p srajapur
 * SW7422-358 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/4   5/31/11 5:39p srajapur
 * SW7422-358 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/3   5/25/11 5:24p srajapur
 * SW7422-358: [7425] Adding support for MS11  in BDSP
 * 
 * Hydra_Software_Devel/2   5/16/11 10:07p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/1   4/6/11 2:16p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/3   1/13/11 6:00a gautamk
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
 * Hydra_Software_Devel/1   12/14/10 2:17p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BDSP_TASK_H_
#define BDSP_TASK_H_

#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bimg.h"
#include "bdsp_context.h"
#include "bdsp_raaga_fw_cit.h"
#include "bdsp_raaga_fw_status.h"
#include "bdsp_raaga_fw_status.h"

/* Kept as 2 keeping in mind the ping-pong type of arrangement for video encoder */
#define BDSP_MAX_EXT_INTERRUPT_PER_TASK				2
#define BDSP_MAX_METADATA_REGISTERS_PER_TASK		2


/***************************************************************************
Summary:
Task Scheduling mode
***************************************************************************/
typedef enum BDSP_TaskSchedulingMode
{
   BDSP_TaskSchedulingMode_eStandalone,    /* Default.  Standalone task */
   BDSP_TaskSchedulingMode_eMaster,            /* Master task in a master/slave relationship */
   BDSP_TaskSchedulingMode_eSlave,                /* Slave task in a master/slave relationship */
   BDSP_TaskSchedulingMode_eMax
} BDSP_TaskSchedulingMode;


/***************************************************************************
Summary:
Task Realtime mode
***************************************************************************/
typedef enum BDSP_TaskRealtimeMode
{
    BDSP_TaskRealtimeMode_eRealTime,                /* Realtime */      
    BDSP_TaskRealtimeMode_eNonRealTime,                /* Non-Realtime */   
    BDSP_TaskRealtimeMode_eSoftRealTime,                /* Soft-Realtime */      
    BDSP_TaskRealtimeMode_eMax
} BDSP_TaskRealtimeMode;


/***************************************************************************
Summary:
Audio Task Low delay  mode
***************************************************************************/
typedef enum BDSP_AudioTaskDelayMode
{
    BDSP_AudioTaskDelayMode_eDefault,  /* Fixed path delay of 128ms is used for all audio processing */
    BDSP_AudioTaskDelayMode_eLow,        /* Variable path delay depending on decode algorithm. */
    BDSP_AudioTaskDelayMode_eMax        
}BDSP_AudioTaskDelayMode;


/***************************************************************************
Summary: 
	The structure contains all information regarding soft increment of STC

Description:
	This structure contains configuration info of soft STC increment. 

See Also:
	None.
****************************************************************************/

typedef struct BDSP_STCIncrementConfig
{
	/* If soft triggering is required. Default = BDSP_AF_P_eDisable */
	bool	                    enableStcTrigger;
	/* High and Low part of registers to tell the amount of STC increment. */
	uint32_t				stcIncHiAddr;
	uint32_t				stcIncLowAddr;
	/* Address of register to send trigger for incrementing STC */
	uint32_t				stcIncTrigAddr;
    /* Trigger bit in the above register. Bit count [031]*/
	uint32_t				triggerBit;

}BDSP_STCIncrementConfig;

/***************************************************************************
Summary: 
	The structure contains all information regarding soft external interrupts to DSP 

Description:
	This structure contains configuration info of soft external interrupts to DSP.

See Also:
	None.
****************************************************************************/
typedef struct BDSP_ExtInterruptConfig
{
	/* If the dsp task will be interrupted by external client */
	bool	                enableInterrupts;
	uint32_t				numInterrupts;
	/* only numInterrupts of following struct will be valid */
	struct
	{
		/* ESR_SI register address. Full 32bit address */
		uint32_t				interruptRegAddr;
	    /* Trigger bit in the above register. Bit count [0...31]*/
		uint32_t				interruptBit;
	}interruptInfo[BDSP_MAX_EXT_INTERRUPT_PER_TASK];

}BDSP_ExtInterruptConfig;

/***************************************************************************
Summary: 
	The structure contains all information regarding RDB registers that will be used to 
	communicate between dsp and some other clients.

Description:
	This structure contains configuration info of RDB registers that will be used to 
	communicate between dsp and some other clients.

See Also:
	None.
****************************************************************************/
typedef struct BDSP_VideoEncRdbRegisterConfig
{
	/* Full 32 bit RDB address for STC to take snapshot for generating ESCR/PTS */
	uint32_t				stcAddress;
	/* Full 32 bit RDB address of DSP page registers where metadata will be written */
	uint32_t				metadataRdbAddress[BDSP_MAX_METADATA_REGISTERS_PER_TASK];
}BDSP_VideoEncRdbRegisterConfig;

/***************************************************************************
Summary:
Create A DSP Task
***************************************************************************/
typedef struct BDSP_TaskCreateSettings
{
    unsigned dspIndex;          /* Which DSP to create this task on */
    BDSP_TaskSchedulingMode schedulingMode;    
    BDSP_TaskRealtimeMode realtimeMode;
    BDSP_TaskHandle masterTask;
    
    unsigned numBranches;
    BDSP_CIT_P_FwBranchInfo *pBranchInfo[BDSP_P_MAX_FW_BRANCH_PER_FW_TASK]; /* Information of FW branches in a FW task */

    /* TODO: Should probably be a union based on context/task type - revisit with video encoding */
    BDSP_VF_P_sVDecodeBuffCfg   *psVDecodeBuffCfg;
	BDSP_VF_P_sVEncoderConfig		*psVEncoderConfig;
    BDSP_AudioTaskDelayMode     audioTaskDelayMode;

    BDSP_AF_P_TimeBaseType  timeBaseType;       /* Time base type for a task 45Khz or 27 Mhz (Direct TV)*/          
    bool    ppmCorrection;  /*Enable/Disable 2ms tight lypsinc*/
    BDSP_STCIncrementConfig    stcIncrementConfig;  /* Soft increment of STC*/
	BDSP_ExtInterruptConfig		extInterruptConfig; /* External interrupt to DSP configuration */
	BDSP_VideoEncRdbRegisterConfig	videoEncRdbConfig;  /* RDB of DSP page configuration */
    bool    openGateAtStart; /* Enable mute frame rendering at the start of audio decode */
    BDSP_AF_P_sOpSamplingFreq *pSampleRateMap; /* Pointer to the input -> output sample rate mapping table */
	unsigned maxIndependentDelay; /* Maximum independent delay value in ms (default=500).  Used only for audio tasks. */
} BDSP_TaskCreateSettings;



/***************************************************************************
Summary:
Get Default DSP Task Settings
***************************************************************************/
void BDSP_Task_GetDefaultCreateSettings(
    BDSP_ContextHandle context,
    BDSP_TaskCreateSettings *pSettings     /* [out] */
    );

/***************************************************************************
Summary:
Create a DSP task
***************************************************************************/
BERR_Code BDSP_Task_Create(
    BDSP_ContextHandle context,
    const BDSP_TaskCreateSettings *pSettings,
    BDSP_TaskHandle *pTask    /* [out] */
    );

/***************************************************************************
Summary:
Destroy a DSP task
***************************************************************************/
void BDSP_Task_Destroy(
    BDSP_TaskHandle task
    );

/***************************************************************************
Summary:
Start a DSP task
***************************************************************************/
BERR_Code BDSP_Task_Start(
    BDSP_TaskHandle task
    );

/***************************************************************************
Summary:
Stop a DSP task
***************************************************************************/
BERR_Code BDSP_Task_Stop(
    BDSP_TaskHandle task
    );

/***************************************************************************
Summary:
Get settings for a particular task stage
***************************************************************************/
BERR_Code BDSP_Task_GetStageSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    void *pSettingsBuffer,        /* [out] */
    size_t settingsBufferSize
    );

/***************************************************************************
Summary:
Set settings for a particular task stage
***************************************************************************/
BERR_Code BDSP_Task_SetStageSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const void *pSettingsBuffer,
    size_t settingsBufferSize
    );

/***************************************************************************
Summary:
Get status from a particular task stage
***************************************************************************/
BERR_Code BDSP_Task_GetStageStatus(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    void *pStatusBuffer,        /* [out] */
    size_t statusBufferSize
    );

/***************************************************************************
Summary:
Add Stage Input settings
***************************************************************************/
typedef struct BDSP_TaskAddStageInputSettings
{
    unsigned branchId;
    unsigned stageId;
    BDSP_CIT_P_FwStgSrcDstDetails sourceDetails;
} BDSP_TaskAddStageInputSettings;


/***************************************************************************
Summary:
Get default add Stage Input settings
***************************************************************************/
void BDSP_Task_GetDefaultAddStageInputSettings(
    BDSP_CIT_P_FwStgSrcDstType  inputType,
    BDSP_TaskAddStageInputSettings *pSettings /*[out] */
    );


/***************************************************************************
Summary:
Add input to a task stage
***************************************************************************/
BERR_Code BDSP_Task_AddStageInput(
    BDSP_TaskHandle task,
    const BDSP_TaskAddStageInputSettings *pSettings,
    BDSP_TaskStageInputHandle *pHandle /* [out] */
    );


/***************************************************************************
Summary:
remove input from a task stage
***************************************************************************/
BERR_Code BDSP_Task_RemoveStageInput(
    BDSP_TaskStageInputHandle input
    );

/***************************************************************************
Summary:
Get input index of a task stage
***************************************************************************/
BERR_Code BDSP_Task_GetStageInputIndex(
    BDSP_TaskStageInputHandle input,
    unsigned    *inputIndex
    );

#endif

