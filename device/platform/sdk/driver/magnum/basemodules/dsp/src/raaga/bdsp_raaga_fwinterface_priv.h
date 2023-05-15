/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_fwinterface_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 6/19/12 3:16a $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_fwinterface_priv.h $
 * 
 * Hydra_Software_Devel/9   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/8   3/22/12 6:49p gprasad
 * FWRAA-399: DTS LBR SUPPORT
 * 
 * Hydra_Software_Devel/7   3/1/12 8:48p ashoky
 * SW7231-585: Changing the RDQ implementation from DRAM based to RDB
 * based.
 * 
 * Hydra_Software_Devel/6   1/27/12 1:19p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/5   8/17/11 12:52p gprasad
 * FWRAA-280: Syncing basemodules and Raaga host interface files
 * 
 * Hydra_Software_Devel/4   6/8/11 4:43p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/3   5/16/11 10:08p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/2   4/6/11 2:44p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   4/6/11 2:27p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/9   3/8/11 9:13a srajapur
 * SW7422-294 : [7422]Adding watchdog recovery support in BASE DSP
 * 
 * Hydra_Software_Devel/8   2/26/11 12:40a srajapur
 * SW7422-293:[7422] Adding support for encode algorithms
 * 
 * Hydra_Software_Devel/7   2/5/11 4:24a srajapur
 * SW7422-247 : [7422] Adding Generic passthrul audio processing support.
 * 
 * Hydra_Software_Devel/6   1/20/11 8:10a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/5   1/19/11 2:14a gautamk
 * SW7422-191: putting all the Details of Audio codec in a const array and
 * then using it across the code.
 * 
 * Hydra_Software_Devel/4   1/18/11 5:58a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/3   1/18/11 4:21a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/2   1/15/11 3:32a gautamk
 * SW7422-191:[7422] CLeaning up unwanted debug message
 * 
 * Hydra_Software_Devel/1   1/13/11 5:34a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * 
 ***************************************************************************/

#ifndef BDSP_RAAGA_FWINTERFACE_PRIV_H_
#define BDSP_RAAGA_FWINTERFACE_PRIV_H_
#include "bdsp_raaga_types.h"
#include "bdsp_raaga_fw.h"
#include "bdsp_audio_task.h"
#include "bdsp_raaga_cmdresp_priv.h"
#include "bdsp_raaga_fw_settings.h"
#include "bdsp_raaga_fw_status.h"


#define BDSP_RAAGA_P_FIFO_BASE_OFFSET    0
#define BDSP_RAAGA_P_FIFO_END_OFFSET     4
#define BDSP_RAAGA_P_FIFO_READ_OFFSET    12 /*8*/
#define BDSP_RAAGA_P_FIFO_WRITE_OFFSET   8  /*12*/


#define BDSP_RAAGA_P_INVALID_TASK_ID		    ((unsigned int)(-1))


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
typedef struct BDSP_Raaga_P_MsgQueue  
{ 
    BMEM_Handle hHeap; 
    BREG_Handle hRegister;
    unsigned int uiBaseAddr;  /* phys address */
    unsigned int uiEndAddr;  /* phys address */
    unsigned int uiReadPtr;  /* phys address */
    unsigned int uiWritePtr;  /* phys address */
    uint32_t ui32FifoId;  /* Fifo Id for this message queue */
    uint32_t ui32DspOffset; /* DSP Register Offset */
    
} BDSP_Raaga_P_MsgQueue; 
 
typedef struct BDSP_Raaga_P_MsgQueue *BDSP_Raaga_P_MsgQueueHandle;

/***************************************************************************
Summary:
    This enum hold the type of message passed 
***************************************************************************/ 
typedef enum  BDSP_Raaga_P_MsgType
{
    BDSP_Raaga_P_MsgType_eSyn,
    BDSP_Raaga_P_MsgType_eAsyn
}BDSP_Raaga_P_MsgType;

typedef struct BDSP_Raaga_P_UserConfigStruct
{
#if 0
    BDSP_Raaga_Audio_MpegConfigParams	        sMpegConfigParam;	
    BDSP_Raaga_Audio_AacheConfigParams           sAacheConfigParam;
    BDSP_Raaga_Audio_WmaConfigParams             sWmaStdConfigParam;    
    BDSP_Raaga_Audio_WmaProConfigParams          sWmaProConfigParam;    
    BDSP_Raaga_Audio_LpcmUserConfig      	    sLpcmDvdConfigParam;       
    BDSP_Raaga_Audio_DtsHdConfigParams           sDtsBroadcastConfigParam;
    BDSP_Raaga_Audio_DtsHdConfigParams           sDtsHdConfigParam;   
	BDSP_Raaga_Audio_DtslbrConfigParams          sDtslbrConfigParam;
    BDSP_Raaga_Audio_PcmWavConfigParams          sPcmWavConfigParam;
    BDSP_Raaga_Audio_AmrConfigParams             sAmrConfigParam;    
    BDSP_Raaga_Audio_DraConfigParams             sDraConfigParam;        
    BDSP_Raaga_Audio_RalbrConfigParams           sRealAudioLbrConfigParam;
    BDSP_Raaga_Audio_DolbyPulseUserConfig        sDolbyPulseConfigParam;
    BDSP_Raaga_Audio_DDPMultiStreamConfigParams  sMs10DDPConfigParam;    
	BDSP_Raaga_Audio_AdpcmConfigParams           sAdpcmConfigParam;        
#endif
    /* This is valid if only one Passthru exists in the system */
	BDSP_Raaga_Audio_PassthruConfigParams    sPassthruConfigParam;	      
    
    /* TODO :: Since there may be more than one task, framesync and hence 
        TSM might be different for each of them. The below structure might have
        to be per task or an array of MAX_TASKS */
    BDSP_Raaga_Audio_FrameSyncTsmConfigParams   sFrameSyncTsmConfigParams;

}BDSP_Raaga_P_UserConfigStruct;


typedef struct BDSP_Raaga_P_DecoderSettings
{
	BDSP_Raaga_P_UserConfigStruct sUserConfigStruct; 
} BDSP_Raaga_P_DecoderSettings;


 
/***************************************************************************
Summary:
	Data structure details of the message queue parameters in the system memory
    
Description:
	Parmeters passed:-
		Base address(virtual) of the message queue
		Size of the message queue
		Address of the attribute structure address of the message queue

***************************************************************************/
typedef struct BDSP_Raaga_P_MsgQueueParams
{
    unsigned int uiBaseAddr; /* Virtual Address */
    unsigned int uiMsgQueueSize;
    uint32_t ui32FifoId;  /* Fifo Id for this message queue */
} BDSP_Raaga_P_MsgQueueParams;

typedef struct BDSP_Raaga_P_RdbQueueParams
{
    int32_t 	startIndexOfFreeFifo; /* Start Id of Fifo */
    unsigned 	int uiMsgQueueSize;
    uint32_t 	ui32FifoId;  /* Fifo Id for this message queue */
} BDSP_Raaga_P_RdbQueueParams;

BERR_Code BDSP_Raaga_P_CreateMsgQueue(
    BDSP_Raaga_P_MsgQueueParams    *psMsgQueueParams , /* [in]*/
    BMEM_Handle                 hHeap,              /* [in] */
    BREG_Handle                 hRegister,          /* [in] */
    uint32_t                    ui32DspOffset,      /* [in] */
    BDSP_Raaga_P_MsgQueueHandle    *hMsgQueue,         /* [out]*/
    bool bWdgRecovery /* [in] */ 
    );

BERR_Code BDSP_Raaga_P_DestroyMsgQueue(
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue, 
    BREG_Handle                 hRegister,          /* [in] */
    uint32_t                    ui32DspOffset      /* [in] */    
    );

BERR_Code BDSP_Raaga_P_CreateRdbQueue(
    BDSP_Raaga_P_RdbQueueParams    *psMsgQueueParams , /* [in]*/
    BMEM_Handle                 hHeap,              /* [in] */
    BREG_Handle                 hRegister,          /* [in] */
    uint32_t                    ui32DspOffset,      /* [in] */
    BDSP_Raaga_P_MsgQueueHandle    *hMsgQueue,         /* [out]*/
    bool bWdgRecovery /* [in] */ 
    );

BERR_Code BDSP_Raaga_P_SendCommand( 
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue   /*[in]*/, 
    const BDSP_Raaga_P_Command         *psCommand  /*[in]*/ ,
    void *pTaskHandle      /*[in] Task handle */
    );

BERR_Code BDSP_Raaga_P_SendCommand_isr( 
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue   /*[in]*/, 
    const BDSP_Raaga_P_Command         *psCommand  /*[in]*/,
    void    *pTaskHandle       /*[in] Task handle */);

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

BERR_Code BDSP_Raaga_P_GetMsg(BDSP_Raaga_P_MsgQueueHandle  hMsgQueue, 
									  void *pMsgBuf, BDSP_Raaga_P_MsgType eMgsType);

/***************************************************************************
Summary:
	Gets a message from the message queue and writes in into the message buffer    
    
Description:
    This is the isr version of BDSP_Raaga_P_GetMsg.

Returns:
    BERR_SUCCESS else error

**************************************************************************/
BERR_Code BDSP_Raaga_P_GetMsg_isr(BDSP_Raaga_P_MsgQueueHandle  hMsgQueue, 
									  void *pMsgBuf, BDSP_Raaga_P_MsgType eMgsType);

BERR_Code BDSP_Raaga_P_SetAudioDecodeStageSettings(
        BMEM_Handle         hHeap,        
        BDSP_AudioType   eAudioType,         /* [in] Decode audio type */
        BDSP_DecodeMode  eDecodeMode,        /* [in] Decode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void *pSettingsBuffer,
        size_t settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_GetAudioDecodeStageSettings(
        BMEM_Handle         hHeap,        
        BDSP_AudioType   eAudioType,         /* [in] Decode audio type */
        BDSP_DecodeMode  eDecodeMode,        /* [in] Decode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        void *pSettingsBuffer,                        /*[out]*/
        size_t settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_GetAudioProcessingStageSettings(
        BMEM_Handle         hHeap,
        BDSP_AudioProcessing   eProcessingType,         /* [in] Processing type */
        BDSP_CIT_P_ProcessingMode  eProcessingMode,        /* [in] Processing Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        void *pSettingsBuffer,                        /*[out]*/
        size_t settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_SetAudioProcessingStageSettings(
        BMEM_Handle         hHeap,    
        BDSP_AudioProcessing   eProcessingType,         /* [in] Processing type */
        BDSP_CIT_P_ProcessingMode  eProcessingMode,        /* [in] Processing Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void *pSettingsBuffer,
        size_t settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_GetAudioEncodeStageSettings(
        BMEM_Handle         hHeap,
        BDSP_AudioEncode    eEncodeType,         /* [in] Encode type */
        BDSP_EncodeMode     eEncodeMode,        /* [in] Encode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        void *pSettingsBuffer,                        /*[out]*/
        size_t settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_SetAudioEncodeStageSettings(
        BMEM_Handle         hHeap,    
        BDSP_AudioEncode    eEncodeType,         /* [in] Encode type */
        BDSP_EncodeMode     eEncodeMode,        /* [in] Encode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void *pSettingsBuffer,
        size_t settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_GetVideoDecodeStageSettings(
        BMEM_Handle         hHeap,
        BDSP_VideoType   	eVideoType,         	/* [in] Decode Video type */
        BDSP_DecodeMode  	eDecodeMode,        	/* [in] Decode Mode */
        unsigned int        uiConfigBufAddr,    	/* [in] Config Buf Address */
        unsigned int        uiConfigBufSize,    	/* [in] Config Buf Size */
        void 				*pSettingsBuffer,       /*[out]*/
        size_t 				settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_GetVideoEncodeStageSettings(
        BMEM_Handle         	hHeap,
        BDSP_VideoEncode   		eVidEncType,        /* [in] Video encode type */
        BDSP_EncodeMode  		eEncodeMode,        /* [in] Encode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        void 					*pSettingsBuffer,                        /*[out]*/
        size_t 					settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_SetVideoDecodeStageSettings(
        BMEM_Handle         hHeap,    
        BDSP_VideoType   eVideoType,         /* [in] Decode audio type */
        BDSP_DecodeMode  eDecodeMode,        /* [in] Decode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void *pSettingsBuffer,
        size_t settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_SetVideoEncodeStageSettings(
        BMEM_Handle         	hHeap,    
        BDSP_VideoEncode   		eVideoEncType,         /* [in] Decode audio type */
        BDSP_EncodeMode  		eEncodeMode,        /* [in] Encode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void 				*pSettingsBuffer,
        size_t 					settingsBufferSize    
    );

BERR_Code BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams(
    BMEM_Handle         hHeap,        
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    const void *pSettingsBuffer    
);

BERR_Code BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams(
    BMEM_Handle         hHeap,        
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    void *pSettingsBuffer    
);

BERR_Code BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams_isr(
    BMEM_Handle         hHeap,        
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    void *pSettingsBuffer    
);
    BERR_Code BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams_isr(
        BMEM_Handle         hHeap,            
        unsigned int        uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int        uiConfigBufSize,     /* [in] Config Buf Size */
        const void *pSettingsBuffer    
    );

BERR_Code BDSP_Raaga_P_SetVideoEncodeFrameSyncStageConfigParams(
    BMEM_Handle         hHeap,        
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    const void *pSettingsBuffer    
);

BERR_Code BDSP_Raaga_P_GetVideoEncodeFrameSyncStageConfigParams(
    BMEM_Handle         hHeap,        
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    void *pSettingsBuffer    
);

BERR_Code BDSP_Raaga_P_GetVideoEncodeFrameSyncStageConfigParams_isr(
    BMEM_Handle         hHeap,        
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    void *pSettingsBuffer    
);
    BERR_Code BDSP_Raaga_P_SetVideoEncodeFrameSyncStageConfigParams_isr(
        BMEM_Handle         hHeap,            
        unsigned int        uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int        uiConfigBufSize,     /* [in] Config Buf Size */
        const void *pSettingsBuffer    
    );
BERR_Code BDSP_Raaga_P_GetAudioDecodeStageStatus(
        BMEM_Handle         hHeap,        
        BDSP_AudioType   eAudioType,         /* [in] Decode audio type */
        BDSP_DecodeMode  eDecodeMode,        /* [in] Decode Mode */
        unsigned int            uiStatusBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiStatusBufSize,    /* [in] Config Buf Size */
        void *pStatusBuffer,                        /*[out]*/
        size_t statusBufferSize    
    );



BERR_Code BDSP_Raaga_P_GetAsyncMsg_isr(
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue,  /*[in]*/
	void                        *pMsgBuf,   /*[in]*/
	unsigned int                *puiNumMsgs /*[out]*/
	);

BERR_Code BDSP_Raaga_P_GetVideoMsg_isr(BDSP_Raaga_P_MsgQueueHandle  hMsgQueue,/*[in]*/
									 uint32_t *pMsgBuf,bool bReadUpdate);

BERR_Code BDSP_Raaga_P_WriteVideoMsg_isr(BDSP_Raaga_P_MsgQueueHandle   hMsgQueue/*[in]*/,
									     void *pMsgBuf, /*[in]*/
									     unsigned int uiBufSize/*[in]*/);



#endif

