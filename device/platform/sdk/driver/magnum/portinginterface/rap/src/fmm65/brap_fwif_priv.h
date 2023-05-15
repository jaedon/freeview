/***************************************************************************
*     Copyright (c) 2006-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_fwif_priv.h $
* $brcm_Revision: Hydra_Software_Devel/56 $
* $brcm_Date: 3/7/11 6:04p $
*
* Module Description:
*	This file contains data structures and prototypes of low level functions that interact with 
*      firmware and DSP hardware.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_fwif_priv.h $
* 
* Hydra_Software_Devel/56   3/7/11 6:04p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/7550_SCM_DEVEL/1   1/6/11 3:39p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/55   9/16/10 12:19a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/54   9/6/10 5:15p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT_PH6P0/1   8/9/10 5:09p gautamk
* SW7550-459: [7550] Meging GFX support to Ph6p0 of 7550
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/4   7/15/10 2:01p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/3   7/12/10 2:52p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/2   7/12/10 12:26p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/1   6/16/10 3:20p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/53   5/17/10 10:54a gautamk
* SW7405-4170: [7405] Merging MS10 changes to mainline
* 
* Hydra_Software_Devel/RAP_MS10_DEVEL/1   4/27/10 5:23p gautamk
* SW7405-4170: [7405] Adding PI/CIT/FW interface change for MS10
* 
* Hydra_Software_Devel/52   10/21/09 5:01p gautamk
* SW7335-609:[7405] Setting hTask-bStopped appropriately in Isr Ctxt when
* Event is getting set. This is done to fix race condition. Porting to
* mainline
* 
* Hydra_Software_Devel/51   6/12/09 3:41p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/50   3/23/09 3:03p gautamk
* PR53419: [7405] non-isr function hsould not be called from isr
* functions.
* 
* Hydra_Software_Devel/49   9/16/08 3:43p speter
* PR 46942: [3548,3556] Merging back to main line of development to get
* the distinct output feature
* 
* Hydra_Software_Devel/48   9/15/08 4:09p gautamk
* PR43454: [7405] Do not create seperate path/branch for
* bDownmixedOpPort= true O/P, when outputmode is 2_0
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/1   9/5/08 3:20p speter
* PR 45032: [3548,3556] Adding PES Framesync user config params related
* changes
* 
* Hydra_Software_Devel/47   8/20/08 6:13p gautamk
* PR43454: [7405] Changes for Concurrent downmix.
* 
* Hydra_Software_Devel/46   8/12/08 10:24a speter
* PR 45032: [3548,3556] Adding PES Framesync user config params related
* changes
* 
* Hydra_Software_Devel/45   6/20/08 5:57p gautamk
* PR43454: [7405] Adding function determine if the the brancg is cloned
* or not.
* 
* Hydra_Software_Devel/44   5/29/08 3:28p gautamk
* PR41726: [7405] Adding function to insert/destroy stages internally in
* Task network.
* 
* Hydra_Software_Devel/43   5/13/08 6:29p gautamk
* PR42504:[7405] Not sending task command if the stop command for task
* already sent.
* 
* Hydra_Software_Devel/42   5/9/08 4:17p gautamk
* PR42302:[7405] If watchdogISR is trigerred in between when PI waiting a
* response/ack from FW DSP, Then PI shoul exit gracefully. And then
* ProcessWatchdogRecovery() function should be called
* 
* PR42504:[7405]
* 
* Hydra_Software_Devel/41   5/3/08 1:53a gautamk
* PR41726: [7405] Update for TSM config params
* 
* Hydra_Software_Devel/40   4/30/08 6:58p gautamk
* PR41726: [7405] Adding code to program user config param for processing
* algos on the fly.
* 
* Hydra_Software_Devel/39   4/24/08 2:36p gautamk
* PR41726: [7405] Modifying TSM config params
* 
* Hydra_Software_Devel/38   4/21/08 6:23p gautamk
* PR41726: [7405] IMplementing review comments start/stop APis
* 
* Hydra_Software_Devel/37   4/14/08 11:37a sushmit
* PR 34648: [7405] Reducing Stack Usage
* 
* Hydra_Software_Devel/36   4/8/08 2:22p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/1   3/28/08 4:02p gautamk
* PR34648: [7405] adding code for watchdog recovery in a branch
* 
* Hydra_Software_Devel/35   3/18/08 12:26p sushmit
* PR 34648: [7405] Reading all async messages on one interrupt
* 
* Hydra_Software_Devel/34   3/17/08 3:28p gdata
* PR34648: [7405] Adding the isr version of write and read message queue
* 
* Hydra_Software_Devel/33   3/6/08 10:09p gautamk
* PR34648: [7405] Adding extra parameter in
* BRAP_FWIF_P_SetDecodeStageConfigParams to return Actual Config size.
* 
* Hydra_Software_Devel/32   2/29/08 4:24p gautamk
* PR34648: [7405] Changing file names includes
* 
* Hydra_Software_Devel/31   2/22/08 4:18p gautamk
* PR34648: [7405] fixing coverity defects + implementing review comments
* 
* Hydra_Software_Devel/30   2/18/08 3:00p sushmit
* PR34648: [7405] PI Changes for OpPortConfig to CIT Input
* 
* Hydra_Software_Devel/29   2/15/08 8:45p gautamk
* PR34648: [7405]Adding User Config for Passthru
* Adding spdif CBit buffer.
* 
* Hydra_Software_Devel/28   2/11/08 7:03p gautamk
* PR34648: [7405] Adding PI Support for DDP.
* 
* Hydra_Software_Devel/27   2/7/08 2:41p gautamk
* PR34648: [7405] implementing getDefaultConfigParams,
* getCurrentConfigparams.
* Removing compilation warning of
* brap_fwif.h
* brap_fwif_user_config.h
* brap_fwif_proc.c
* brap_fwif_dwnldutil.c
* 
* Hydra_Software_Devel/26   2/1/08 9:12p gdata
* PR34648: [7405] Removing the toggle logic from message queue
* 
* Hydra_Software_Devel/25   1/31/08 7:56p nitinb
* PR34648: [7405] Implemented review comments on stack usage
* 
* Hydra_Software_Devel/24   1/31/08 5:23p gautamk
* PR34648: [7405] Removing BRAP_DSPCHN_DecodeMode_eSimulMode Enum
* 
* Hydra_Software_Devel/23   1/30/08 1:44p gdata
* PR34648: [7405] Adding enum to decide the type of message
* Adding message type parameter in BRAP_FWIF_P_GetMsg
* 
* Hydra_Software_Devel/22   1/28/08 7:47p gautamk
* PR34648: [7405] Adding config param programming for Ac3 as per new
* design in which hRap stores config struct in the Format FW expects
* 
* Hydra_Software_Devel/21   1/24/08 3:51p gdata
* PR 34648: [7405] adding support for stop dpschan
* 
* Hydra_Software_Devel/20   1/22/08 2:17p gautamk
* PR 34648: [7405] programming user config paramas for MPEG/Ac3Plus
* 
* Hydra_Software_Devel/19   1/17/08 2:29p sushmit
* PR 34648: [7405] PING Bringup Changes
* 
* Hydra_Software_Devel/18   1/16/08 4:50p nitinb
* PR34648: [7405] Added support for programming configuration parameters
* 
* Hydra_Software_Devel/17   1/15/08 5:02p sushmit
* PR34648: [7405] Moving CIT Output malloc to DRAM
* 
* Hydra_Software_Devel/16   1/15/08 2:30p sushmit
* PR34648: [7405] Moving CIT Output malloc to DRAM
* 
* Hydra_Software_Devel/15   1/11/08 6:40p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/14   1/9/08 10:36p sushmit
* PR 34648: [7405] Updating FW-PI Interface after syncup meeting.
* 
* Hydra_Software_Devel/13   1/7/08 4:21p gdata
* PR34648: [7405] Adding the changes realted to interrupts
* 
* Hydra_Software_Devel/12   1/7/08 2:27p sushmit
* PR34648: [7405] Adding Start Task Command
* 
* Hydra_Software_Devel/11   1/7/08 11:59a sushmit
* PR34648: [7405] Modifying CMD-RSP logic & FIFO usage logic.
* 
* Hydra_Software_Devel/10   1/4/08 1:50p bhanus
* PR34648: [7405] Adding changes for Common Audio Processing network for
* CIT module.
* 
* Hydra_Software_Devel/9   1/3/08 6:52p nitinb
* PR 34648: [7405] Integration of FW-Interface module with other modules
* 
* Hydra_Software_Devel/8   1/3/08 2:22p bhanus
* PR34648: [7405] Adding changes for Channel Audio Processing network for
* CIT module.
* 
* Hydra_Software_Devel/7   12/31/07 10:25p sushmit
* PR34648: [7405] Adding Interrupt Change.
* 
* Hydra_Software_Devel/6   12/31/07 7:10p sushmit
* PR34648: [7405] Updating more code.
* 
* Hydra_Software_Devel/5   12/31/07 4:54p nitinb
* PR34648: [7405] Resolved circular dependancy in files brap_dsp_priv.h
* and brap_fwif.h by moving handle definitions to brap_types_priv.h.
* 
* Hydra_Software_Devel/4   12/28/07 9:54p sushmit
* PR 34648: [7405] Initial Checkins after DSP Bootup completion.
* 
* Hydra_Software_Devel/3   11/21/07 2:12p nitinb
* PR34648: [7405] Added prototype for send/receive command functions
* 
* Hydra_Software_Devel/2   11/21/07 9:59a nitinb
* PR34648: [7405] Impelementation of low level message queue functions
* 
* Hydra_Software_Devel/1   10/8/07 11:26a nitinb
* PR34648: [7405] Files for firmware interface
***************************************************************************/
#ifndef _BRAP_FWIF_H__
#define _BRAP_FWIF_H__

#include <brap_cmdresp_priv.h>
#include "brap_types_priv.h"
#include "brap_priv.h"

#define BRAP_FWIF_P_MASK_MSB 0x7fffffff  
/*#define MESSAGE_SIZE 20*/
#define MESSAGE_SIZE sizeof(BRAP_FWIF_P_Command)

#define BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN      (1)

#define BRAP_FWIF_P_FIFO_BASE_OFFSET    0
#define BRAP_FWIF_P_FIFO_END_OFFSET     4
#define BRAP_FWIF_P_FIFO_READ_OFFSET    12 /*8*/
#define BRAP_FWIF_P_FIFO_WRITE_OFFSET   8  /*12*/


/*************************************************************************
Summary:
       Data structure details for the message queue in the system memory
    
Description:
	Parmeters passed in this structure:-
		Handle for the heap of memory  to be allocated
		Base and End Addresses of the message queue (local copy)
    		Read pointer and write pointer addresses (local copy)required for operations like writing a message and reading a message  
    		The message queue attribute address containing the base address of the message queue needed to be passed to the shared copy in DRAM 
		The MSB toggle bits for both the read and write pointers to determine wrap around conditions in the message queue
***************************************************************************/
typedef struct BRAP_FWIF_MsgQueue  
{ 
    BMEM_Handle hHeap; 
    BREG_Handle hRegister;
    unsigned int uiBaseAddr;  /* phys address */
    unsigned int uiEndAddr;  /* phys address */
    unsigned int uiReadPtr;  /* phys address */
    unsigned int uiWritePtr;  /* phys address */
    uint32_t ui32FifoId;  /* Fifo Id for this message queue */
    uint32_t ui32DspOffset; /* DSP Register Offset */
    
} BRAP_FWIF_MsgQueue; 
 
typedef struct BRAP_FWIF_MsgQueue *BRAP_FWIF_MsgQueueHandle;
 
/***************************************************************************
Summary:
	Data structure details of the message queue parameters in the system memory
    
Description:
	Parmeters passed:-
		Base address(virtual) of the message queue
		Size of the message queue
		Address of the attribute structure address of the message queue

***************************************************************************/
typedef struct BRAP_FWIF_MsgQueueParams
{
    unsigned int uiBaseAddr; /* Virtual Address */
    unsigned int uiMsgQueueSize;
    uint32_t ui32FifoId;  /* Fifo Id for this message queue */
} BRAP_FWIF_MsgQueueParams;

/*************************************************************************
Summary:
       Data structure for describing FW task
    
Description:
        This data structure describes firmware task object.
***************************************************************************/
typedef struct BRAP_FWIF_P_FwTask
{
    unsigned int    uiTaskId;

	BRAP_CIT_P_Output	sCitOutput;
#ifdef RAP_REALVIDEO_SUPPORT
    BRAP_CIT_P_VideoCITOutput   sVideoCitOutput;    
#endif
#ifdef RAP_GFX_SUPPORT    
    BRAP_CIT_P_GfxCITOutput	sGfxCitOutput;
#endif    
#ifdef RAP_SCM_SUPPORT
	BRAP_CIT_P_ScmCITOutput	sScmCitOutput;
#endif
    bool bChSpecificTask;   /* TRUE = Task is channel specific,
                                               FALSE = Task belongs to association */
    union
    {
        BRAP_ChannelHandle hRapCh; /* Valid if task is channel specific, bChSpecificTask = true */
        BRAP_AssociatedChannelHandle hAssociation; /* Valid if task is shared between channels, 
                                                                                   bChSpecificTask = false */
    } uHandle;
    BRAP_DSP_Handle hDsp; /* Handle of DSP in which the task is running */
    BRAP_FWIF_MsgQueueHandle hAsyncMsgQueue; /* Asynchronous message queue 
                                                belonging to this task */
    BRAP_FWIF_MsgQueueHandle hSyncMsgQueue; /* Synchronous message queue 
                                               belonging to this task */
    void *pAsyncMsgMemory;                  /* Memory for contiguous Async Msgs */
    bool bStopped;  /* TRUE : If the stop command for this task has been sent,
                                                But the Processing of stop is still under process. Keep it in true state untill the hDspCh is started*/              
    unsigned int uiLastEventType;                                                                                                        
    unsigned int uiCommandCounter;    
    bool                bMaster;                                                
    unsigned int    uiMasterTaskId;
                                                
} BRAP_FWIF_P_FwTask;

/*************************************************************************
Summary:
       FW task handle
    
Description:
    This is an opaque handle for firmware task object. 
***************************************************************************/
typedef struct BRAP_FWIF_P_FwTask *BRAP_FWIF_P_FwTaskHandle;


typedef struct BRAP_FWIF_P_TaskInterface
{
	unsigned int	tbd;
} BRAP_FWIF_P_TaskInterface;

/***************************************************************************
Summary:
    This structure for providing info of DSP output ring buffers.
***************************************************************************/ 

typedef struct BRAP_P_DspOutputBufferConfig
{
    BRAP_CIT_P_IoBuf            sOutputBufConfig;
    BRAP_DestinationHandle      hDestHandle;
}BRAP_P_DspOutputBufferConfig;


/***************************************************************************
Summary:
    This structure holds all the Network info that are required for generating
    CIT input structure from channel Audio processing network.
***************************************************************************/ 
typedef struct BRAP_P_NetworkInfo
{
    BRAP_DSPCHN_AudioType           eAudioType;                                                      /*Audio type of decode stage */
    BRAP_DSPCHN_VideoType           eVideoType;                                                      /*Audio type of decode stage */
    BRAP_DSPCHN_DecodeMode        eDecodeMode;                                                      /*Decode Mode of decode stage */    
    BRAP_CIT_P_FwStgSrcDstType      eNetworkInputType;                                         /* Type of the input to the network */
    BRAP_CIT_P_IoBuf                sInputBufConfig;                                                      /* Input Buf Config*/ 
    BRAP_P_DspOutputBufferConfig    sDspOutConfig[BRAP_P_MAX_DST_PER_RAPCH]; /* Outputs Buf Config*/
    
}BRAP_P_NetworkInfo;

/***************************************************************************
Summary:
    This enum hold the type of message passed 
***************************************************************************/ 
typedef enum  BRAP_P_MsgType
{
    BRAP_P_MsgType_eSyn,
    BRAP_P_MsgType_eAsyn
}BRAP_P_MsgType;


/***************************************************************************
Summary:
    This structure is used to MAP the PI Processing Enum to CIT Encode/Processing Enum.
***************************************************************************/ 
typedef struct BRAP_FWIF_P_MapProcessingEnum
{
    union
    {
        BRAP_CIT_P_ProcessingType   eProcessingType;
        BRAP_CIT_P_EncAudioType     eEncodeType;
    } uEnumName;
    
    BRAP_CIT_P_AudioAlgoType    eEnumType; /* Either BRAP_CIT_P_AudioAlgoType_eEncode 
                                                                                or BRAP_CIT_P_AudioAlgoType_ePostProc */

}BRAP_FWIF_P_MapProcessingEnum;


/***************************************************************************
Summary:
    Creates the Message Queue
    
Description:

	Allocate memory for the message queue in system memory
	Initializes attributes in the DRAM(shared copy)
	Initializes the attributes in the local copy in the sytem memory

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_CreateMsgQueue(BRAP_FWIF_MsgQueueParams *psMsgQueueParams ,
                                       	 BMEM_Handle    hHeap, 
                            		     BREG_Handle    hRegister,
                            		     uint32_t       ui32DspOffset, /* Dsp Register offset for DSP */
                                       	 BRAP_FWIF_MsgQueueHandle  *hMsgQueue,
                                       	  bool bWdgRecovery /*bWatchdogrecovery*/);



/***************************************************************************
Summary:
    Destroys the Message Queue
    
Description:
    Free the memory that was allocated for the Message Queue

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_DestroyMsgQueue(BRAP_FWIF_MsgQueueHandle   hMsgQueue,
                                        BRAP_DSP_Handle          hDsp);




/***************************************************************************
Summary:
    Writes a message into the message queue reading from the message buffer
    
Description:
	Sanity check is done to check if the read and write pointers haven't been corrupted
	Checks for free space in the message queue.BUFFER FULL error is generated if there no free space in the message queue.
	Buffer_Size/4 number of bytes are copied from the Message buffer into the message queue
	Write Pointers are updated in both the shared and the local copy.
	
Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_WriteMsg(BRAP_FWIF_MsgQueueHandle   hMsgQueue, 
									     void *pMsgBuf,
									     unsigned int uiBufSize);


/***************************************************************************
Summary:
    Isr function of BRAP_FWIF_P_WriteMsg
    Writes a message into the message queue reading from the message buffer
    
Description:
	Sanity check is done to check if the read and write pointers haven't been corrupted
	Checks for free space in the message queue.BUFFER FULL error is generated if there no free space in the message queue.
	Buffer_Size/4 number of bytes are copied from the Message buffer into the message queue 
	taking wrap around also into consideration
	Write Pointers are updated in both the QueueAttr Structure and the handle.
	
Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_WriteMsg_isr(BRAP_FWIF_MsgQueueHandle   hMsgQueue/*[in]*/,
									     void *pMsgBuf, /*[in]*/
									     unsigned int uiBufSize/*[in]*/);


/***************************************************************************
Summary:
	Gets a message from the message queue and writes in into the message buffer    
    
Description:
	Sanity check is done to check if the read and write pointers haven't been corrupted in the shared copy.
	Checks if a message is present. If no message is there in the message queue BUFFER_EMPTY error is returned
	MESSAGE_SIZE/4 number of words are read from the msg buffer into the message queue
	Read Pointers are updated both in the shared and the local copy    

Returns:
    BERR_SUCCESS else error

**************************************************************************/

BERR_Code BRAP_FWIF_P_GetMsg(BRAP_FWIF_MsgQueueHandle  hMsgQueue, 
									  void *pMsgBuf, BRAP_P_MsgType eMgsType);

/***************************************************************************
Summary:
	Gets a message from the message queue and writes in into the message buffer    
    
Description:
    This is the isr version of BRAP_FWIF_P_GetMsg.

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_GetMsg_isr(BRAP_FWIF_MsgQueueHandle  hMsgQueue, 
									  void *pMsgBuf, BRAP_P_MsgType eMgsType);

/***************************************************************************
Summary:
        Send command to firmware
        
Description:
        This function sends command to firmware.

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_SendCommand( BRAP_FWIF_MsgQueueHandle  hMsgQueue/*[in]*/, 
                        								    BRAP_FWIF_P_Command *psCommand/*[in]*/ ,
                        								        BRAP_Handle hRap ,
                                                                                BRAP_FWIF_P_FwTaskHandle     hTask   /* [in] Task handle */);

/***************************************************************************
Summary:
        Send command to firmware
        
Description:
        This function sends command to firmware. This is isr version of send 
        command        

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_SendCommand_isr( BRAP_FWIF_MsgQueueHandle  hMsgQueue/*[in]*/, 
                        								    BRAP_FWIF_P_Command *psCommand/*[in]*/,
                        								        BRAP_Handle hRap,
                        								        BRAP_FWIF_P_FwTaskHandle     hTask   /* [in] Task handle */);

/***************************************************************************
Summary:
        Get response from firmware for a given command 
    
Description:
        This function receives response from firmware for an issued command

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_GetResponse( BRAP_FWIF_MsgQueueHandle  hMsgQueue, 
                        BRAP_FWIF_P_Response    *psResponse,
                        BRAP_P_MsgType          eMsgType);
/***************************************************************************
Summary:
        Get acknowledgement from firmware for a given command 
    
Description:
        This function receives acknowledgement from firmware for an issued command

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_GetAck( BRAP_FWIF_MsgQueueHandle  hMsgQueue, 
                       						  BRAP_FWIF_P_Response    *psAck );

/***************************************************************************
Summary:
        Get response from firmware for a given asynchronous command 
    
Description:
        This function receives response from firmware for an issued asynchronous 
        command

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_GetAsynResponse( BRAP_FWIF_MsgQueueHandle  hMsgQueue, 
                                        BRAP_FWIF_P_AsynEventMsg   *psAsynResponse);

/***************************************************************************
Summary:
        Flushes the queue
    
Description:
        This function resets all the pointers if a queue is created afresh
Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_FlushMsgQueue( BRAP_FWIF_MsgQueueHandle  hMsgQueue);

/***************************************************************************
Summary:
        Forms the audio processing network.
    
Description:
        This function forms the audio processing network and stores it in the 
        RAP Channel Handle.
Returns:
    BERR_SUCCESS else error

**************************************************************************/

BERR_Code BRAP_FWIF_P_FormProcessingNetworks(
	BRAP_ChannelHandle      hRapCh,		        /* [in] RAP Channel handle */
	bool                    bDecoderEnabled,    /* [in] If decoder is present */
	BRAP_DSPCHN_DecodeMode	eDecodeMode,         /* [in] Decode Mode */
	BRAP_DSPCHN_AudioType eAudioType
);

/***************************************************************************
Summary:
        Generate the CIT input Structure
    
Description:
        This function takes the audio processing network from channel handle and
        Generates the CIT input structure to be passed as Input to CIT module.
Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_GenerateCitInput(
    BRAP_Handle                         hRap,               /* [in] Rap Device Handle */
    BRAP_DSPCHN_Handle	hDspCh,					/* [in] DSPCHN handle */    
    BRAP_P_AudioProcNetwork             *psAudProcNetwork,  /* [in] Audio Processing Network */
    BRAP_P_NetworkInfo                     *psNetworkInfo,          /* [in] input network Info */
    BRAP_CIT_P_InputInfo                *pCitInputStruct,    /* [out] CIT input structure */
    BRAP_FWIF_P_FwTaskHandle hFwTaskCreate     /*[in] Fw Task handle */
);

BERR_Code BRAP_FWIF_P_GenerateVideoCitInput(
    BRAP_Handle                         hRap,               /* [in] Rap Device Handle */
    BRAP_DSPCHN_Handle	hDspCh,					/* [in] DSPCHN handle */
    BRAP_P_AudioProcNetwork             *psAudProcNetwork,  /* [in] Audio Processing Network */
    BRAP_P_NetworkInfo                     *psNetworkInfo,          /* [in] input network Info */
    BRAP_CIT_P_InputInfo                *pCitInputStruct,    /* [out] CIT input structure */
#ifdef RAP_REALVIDEO_SUPPORT  
    BRAP_VF_P_sVDecodeBuffCfg   *psVDecodeBuffCfg,
#endif    
    BRAP_FWIF_P_FwTaskHandle hFwTaskCreate     /*[in] Fw Task handle */
);
BERR_Code BRAP_FWIF_P_SetTsmStageConfigParams(
    BRAP_DSPCHN_Handle	hDspCh,	         /* [in]DSP  Channel Handle */
    BRAP_AF_P_AlgoId	eFwExecId,     /* [in] Node type */
    unsigned int    uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int    uiConfigBufSize     /* [in] Config Buf Size */
);

BERR_Code BRAP_FWIF_P_SetFrameSyncStageConfigParams(
    BRAP_DSPCHN_Handle	hDspCh,	         /* [in]DSP  Channel Handle */
    unsigned int    uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int    uiConfigBufSize     /* [in] Config Buf Size */
);

BERR_Code BRAP_FWIF_P_SetDecodeStageConfigParams(
    BRAP_ChannelHandle  hRapCh,     /* [in] Channel Handle */
    BRAP_DSPCHN_AudioType   eAudioType,     /* [in] Decode audio type */
    BRAP_DSPCHN_DecodeMode eDecodeMode,     /*[in] Decode Mode */    
    unsigned int    uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int    uiConfigBufSize,     /* [in] Config Buf Size */
    unsigned int   *uiActualConfigSize      /*[out] Actual Config Size */
);
BERR_Code BRAP_FWIF_P_SetProcessingStageConfigParams(
    BRAP_ProcessingStageHandle  hStage,     /* [in] Stage handle */
    unsigned int    uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int    uiConfigBufSize,     /* [in] Config Buf Size */
    unsigned int   *uiActualSize    
);
BERR_Code BRAP_FWIF_P_InitSpdifChanStatusParams(
                        BRAP_OP_SpdifChanStatusParams   *psSpdifChStatusParams,
                        unsigned int    uiConfigBufAddr,    /* [in] Config Buf Address */
                        unsigned int    uiConfigBufSize     /* [in] Config Buf Size */);

/***************************************************************************
Summary:
	Gets all posted Async messages from the Async queue     
    
Description:
    This is the isr version of BRAP_FWIF_P_GetMsg.

Returns:
    BERR_SUCCESS else error
    Also returns the number of messages received

**************************************************************************/
BERR_Code BRAP_FWIF_P_GetAsyncMsg_isr(BRAP_FWIF_MsgQueueHandle  hMsgQueue,/*[in]*/
									 void *pMsgBuf,/*[in]*/
                                     unsigned int *puiNumMsgs/*[out]*/);

/***************************************************************************
Summary:
    Destroy all the internal stages for the Channel.
    
Description:

    Destroy all the internal stages for the Channel.
    
Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BRAP_FWIF_P_DestroyInternalStages(	BRAP_ChannelHandle      hRapCh/* [in] RAP Channel handle */);


/***************************************************************************
Summary:
    It will tell if the Branch is the clone of already existing branch. i.e. complete branch is cloned
       
Returns:
    BERR_SUCCESS else error

**************************************************************************/

BERR_Code BRAP_FWIF_P_IsBranchCloned(
    BRAP_ChannelHandle hRapCh,
    BRAP_P_AudioProcNetwork             *psAudProcNetwork,  /* [in] Audio Processing Network */
    unsigned int iBranchId,
    unsigned int iLastStageId,
    bool *bCloned,
    bool *bIsFwBranch  /* If there is no, PP in the branch Fw doesn't treat it as a branch*/);



    
#endif /* _BRAP_FWIF_H__ */
