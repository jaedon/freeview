/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test_video.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/14/12 9:22p $
 *
 * Module Description:
 *          This file contains all structures and function prototypes used 
 *          by raaga_test.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/raaga_test_video.h $
 * 
 * Hydra_Software_Devel/3   8/14/12 9:22p rshijith
 * FWRAA-478: Removing C++ type comment warning
 * 
 * Hydra_Software_Devel/2   8/14/12 7:41p rshijith
 * FWRAA-478: Added IVF header support
 * 
 * Hydra_Software_Devel/1   8/14/12 3:04p rshijith
 * FWRAA-478: Adding video encoder related files to raaga
 * 
 * Hydra_Software_Devel/H264_enc_devel/3   11/7/11 8:36p ashoky
 * SW7231-344: Adding changes in the interface after discussion
 * 
 * Hydra_Software_Devel/H264_enc_devel/2   11/7/11 9:39a ashoky
 * SW7231-344: Adding changes in the interface after discussion.
 * 
 * Hydra_Software_Devel/H264_enc_devel/1   9/20/11 7:08p ashoky
 * SW7231-344: Adding initial code changes for video encoder on raaga as
 * per first design.
 * 
 * Hydra_Software_Devel/12   5/18/11 9:55p srajapur
 * SW7425-572: [7425] Adding VP6 support in raaga test app
 * 
 * Hydra_Software_Devel/11   5/17/11 5:30p srajapur
 * SWDTV-6746 : [35230] Adding support for 35230 and 35233 to raaga_test
 * 
 * Hydra_Software_Devel/10   5/17/11 4:27p srajapur
 * SWDTV-6746 : [35230] Adding support for 35230 and 35233 to raaga_test
 * 
 * Hydra_Software_Devel/9   5/16/11 10:09p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/8   4/13/11 3:43p srajapur
 * SW7422-191:[7422] Adding the static CDB/ITB support in raaga test
 * application
 * 
 * Hydra_Software_Devel/7   4/5/11 4:08p srajapur
 * SW7231-103:[7231] Added support for raaga_test for 7231
 * 
 * Hydra_Software_Devel/7   4/5/11 4:03p srajapur
 * SW7231-103:[7231] Added support for raaga_test for 7231
 * 
 * Hydra_Software_Devel/7   4/5/11 4:02p srajapur
 * SW7231-103:[7231] Added support for raaga_test for 7231
 * 
 * Hydra_Software_Devel/6   3/2/11 11:39p srajapur
 * SW7425-90 : [7425] Added raaga test app support for 7425
 * 
 * Hydra_Software_Devel/5   3/2/11 4:25a srajapur
 * SW7425-90 : [7425] Added raaga test app support for 7425
 * 
 * Hydra_Software_Devel/4   1/28/11 8:28a srajapur
 * SW7422-191:[7422] Adding different test modes.
 * 
 * Hydra_Software_Devel/3   1/25/11 3:36p gdata
 * SW7422-191:[7422] Reading all the parameters from the command line
 * argument so that we can run nightly build
 * 
 * Hydra_Software_Devel/2   1/19/11 5:15a srajapur
 * SW7422-191:[7422] Adding support for 7422 chip.
 * 
 * Hydra_Software_Devel/1   1/14/11 3:24p gdata
 * SW7422-191:[7422] Adding the raaga test application
 * 
**************************** Include Files**********************************/ 
#include "framework.h"

/* Linux stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#define GO_BIT_ENABLED

/* DSP includes */
#include "bdsp.h"
#include "bdsp_raaga.h"
#include "bxdm_picture.h"
#include "btst_kni.h"
#include "btst.h"
#include "data_feeder.h"

#if 0
#include "bdsp_raaga_fw_priv.h"
#endif 

#if 0
#undef BDBG_MSG
#define BDBG_MSG	BDBG_ERR
#endif

/*#define OUPUT_IVF_FILE*/

#define RAAGA_TEST_ENABLE_TRANSPORT 			0 /* Set this to 1 if you want to enable Xpt */
#define RAAGA_TEST_STATIC_CDB_ITB   			0 /* Set this to 1 if you want to use static CDB-ITB */
#define RAAGA_FWAUTHENTICATION_ENABLE           0 /* Set this to 1 if you want to test FW authentication */

#define MAX_STREAMS                 			2
#define XPT_DUMP_ENABLED    					0 /* Change this to 1 if you want to dump the 
                                       			XPT related info */
#define RAAGA_TEST_PROFILING_SUPPORT 			0
#define RAAGA_TEST_STATIC_CDB_ITB_BINARYFILE 	1

#define	BTST_P_USE_MRC_MONITOR					0


#define RAAGA_TEST_XPT_CHECK (RAAGA_TEST_ENABLE_TRANSPORT^RAAGA_TEST_STATIC_CDB_ITB)
                                    /* XPT CHECK should be 1: meaning either XPT
                                       or static CDB-ITB is enabled not both */
#define RAAGA_TEST_ENABLE_FRONTEND_INPUT 		1
#define RAAGA_TEST_MAX_FW_TASK_PER_DSP			7
#define	RAAGA_TEST_MAX_TASK_PER_CONTEXT			3

/*(CEILING(MaxHorzSize/StripeWidth) x StripeWidth) x (CEILING(MaxVertSize/16) x 16)*/
/*(CEILING(720/128) x 128) x (CEILING(576/16) x 16)*/
#define BDSP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE 442368

/*(CEILING(MaxHorzSize/StripeWidth) x StripeWidth) x (CEILING((MaxVertSize/2)/16) x 16)*/
#define BDSP_REALVIDEO_DECODE_CHROMAFRAME_BUF_SIZE 221184  

/*TotalHorzPadd = 96, TotalVertPadd_Luma = 96, and TotalVertPadd_Chroma = 48*/
/*(CEILING(((MaxHorzSize + TotalHorzPadd) x 2)/StripeWidth) x StripeWidth) x (CEILING((MaxVertSize + TotalVertPadd_Luma)/16) x 16)*/
#define BDSP_REALVIDEO_DECODE_REF_LUMAFRAME_BUF_SIZE 1118208

/*(CEILING(((MaxHorzSize + TotalHorzPadd) x 2)/StripeWidth) x StripeWidth) x (CEILING(((MaxVertSize/2) + TotalVertPadd_Chroma)/16) x 16)*/
#define BDSP_REALVIDEO_DECODE_REF_CHROMAFRAME_BUF_SIZE 559104  

#define BDSP_H264_ENCODE_LUMAFRAME_BUF_SIZE						442368
#define BDSP_H264_ENCODE_CHROMAFRAME_BUF_SIZE					442368

/* 691200bytes   (= 720*480*2) */
#define BDSP_H264_ENCODE_RAW_PICTURE_BUF_SIZE					691200

/* (CEILING((PicWidth+(Padding*2))/StripeWidth,1)* StripeWidth)*(CEILING((PicHeight +( Padding *2))/32,1)*32)) */
#define BDSP_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE					487424
/* (=(CEILING((PicWidth+(Padding*2))/StripeWidth,1)* StripeWidth)*(CEILING(((PicHeight/2) +( Padding *2))/32,1)*32)) */
#define BDSP_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE				286720

/* CDB output buffer Keeping 1 MB */
#define	H264_ENCODE_OUTPUT_CDB_SIZE								(1)*(1024)*(1024)
/* ITB output buffer. Keeping 512KB */
#define	H264_ENCODE_OUTPUT_ITB_SIZE								(512)*(1024)

#define	H264_ENCODE_OUTPUT_CDB_FIFO_NUM							9
#define	H264_ENCODE_OUTPUT_ITB_FIFO_NUM							10

#define RAAGA_TEST_VIDEO_MAX_MSGS_PER_QUEUE						32
#define	RAAGA_TEST_DSP_P_FIFO_PDQ								15
#define	RAAGA_TEST_DSP_P_FIFO_PRQ								16

#define RAAGA_TEST_FIFO_BASE_OFFSET								0
#define RAAGA_TEST_FIFO_END_OFFSET								4
#define RAAGA_TEST_FIFO_READ_OFFSET								12 /*8*/
#define RAAGA_TEST_FIFO_WRITE_OFFSET							8  /*12*/




#if RAAGA_TEST_ENABLE_TRANSPORT

#if RAAGA_TEST_DIRECTV_SUPPORT
#include "bxpt_directv.h"
#include "bxpt_directv_pcr.h"
#include "bxpt_directv_pcroffset.h"
#endif
#endif

/* XPT includes */
#include "bxpt_priv.h"
#include "bxpt.h"
#include "bxpt_rave.h"
#include "bxpt_pcr.h"
#include "bxpt_pcr_offset.h"
#include "xpt_pb.h"

/* Register includes */
#include "bchp_xpt_rave.h"
#include "bchp_sun_top_ctrl.h"
#if (BCHP_CHIP == 7422)||(BCHP_CHIP == 7425)||(BCHP_CHIP == 7231)
#include "bchp_aon_pin_ctrl.h"
#endif
#include "bchp_gio.h"

#include "bchp_aud_fmm_bf_ctrl.h"
#include "bchp_hifidac_rm0.h"
#include "bchp_hifidac_ctrl0.h"
#include "bchp_hifidac_esr0.h"
#include "bchp_aud_fmm_iop_ctrl.h"
#include "bchp_aud_fmm_dp_ctrl0.h"
#include "bchp_aud_fmm_op_ctrl.h"
#include "bchp_sun_top_ctrl.h"

#if (BTST_P_USE_MRC_MONITOR)
#include "bmrc_monitor.h"
#endif 

#if (RAAGA_TEST_STATIC_CDB_ITB == 1)
#define BXPT_Handle uint32_t
#define BXPT_PCR_Handle uint32_t
#define BXPT_PcrOffset_Handle uint32_t
#define BXPT_Rave_Handle uint32_t
#define BRAAGA_TEST_PbLib_Handle uint32_t
#endif

#define	BRAAG_TEST_CMD_SIZE		50
typedef enum BRAAGA_TEST_BufferCfgMode
{
	BRAAGA_TEST_BufferCfgMode_eStdAudioMode,	/* Standard Audio Mode (for algorithm 
	                                       like AC3, MPEG, AAC etc) */
	BRAAGA_TEST_BufferCfgMode_eAdvAudioMode,	/* Advanced Audio Mode (for algorithm 
	                                       like AAC-HE, DD+ etc)*/
	BRAAGA_TEST_BufferCfgMode_eIpStbMode,		/* IP Settop Box Mode */
	BRAAGA_TEST_BufferCfgMode_eHdDvdMode,		/* HD DVD Mode */
	BRAAGA_TEST_BufferCfgMode_eBlueRayMode,	/* Blue Ray Mode */
	BRAAGA_TEST_BufferCfgMode_eMaxMode         /* Illegal last value */
}BRAAGA_TEST_BufferCfgMode;

typedef enum BRAAGA_TEST_CxtType
{
    BRAAGA_TEST_CxtType_eAudio,
    BRAAGA_TEST_CxtType_eVideo,
    BRAAGA_TEST_CxtType_eVideoEncode,
    BRAAGA_TEST_CxtType_eGraphics,
    BRAAGA_TEST_CxtType_eSecurity,
    BRAAGA_TEST_CxtType_eMax
} BRAAGA_TEST_CxtType;

typedef enum BRAAGA_TEST_eTaskType
{
	BRAAGA_TEST_eTaskType_eDecode,
	BRAAGA_TEST_eTaskType_eEncode,
	BRAAGA_TEST_eTaskType_eMax
}BRAAGA_TEST_eTaskType;

#define	RAAGA_MAX_CONTEXT	3 /*BRAAGA_TEST_CxtType_eMax*/

/***************************************************************************
Summary:
   Structure defining the settings of a raaga_test channel.
***************************************************************************/     
typedef struct BRAAGA_TEST_ChSettings
{
    bool                bEnableTSM;         

    /* Add/Remove Input source to a channel */
    BDSP_AF_P_BufferType    eDspAudioSrc;   
    unsigned int        	uiItbformat;    

    /* Post-processing Parameters */
	unsigned int        	uiNumStages;        
	bool                	bLfeEn;             

	unsigned int        	uiTimebase;         
    
	BAVC_AudioSamplingRate 	eInSR;           
    unsigned int        	uiInputSelect;
	BRAAGA_TEST_CxtType		eChTaskType;
	BRAAGA_TEST_eTaskType	eTaskType;
}BRAAGA_TEST_ChSettings;
typedef enum BRAAGA_TEST_TaskDstType
{
	BRAAGA_TEST_TaskDstType_eFwStg,			/* Source or destination is another FW stage */
	BRAAGA_TEST_TaskDstType_eRaveBuf,		/* Source or destination is an IO buffer */
	BRAAGA_TEST_TaskDstType_eFMMBuf,			/* Source or destination is an IO buffer */
	BRAAGA_TEST_TaskDstType_eRDB,	
	BRAAGA_TEST_TaskDstType_eDRAMBuf		
} BRAAGA_TEST_TaskDstType;

typedef struct BRAAGA_TEST_VidEncodeBuffer
{
	unsigned int					ui32NumBuffers;
	BDSP_AF_P_sDRAM_CIRCULAR_BUFFER	sPDQ;
	BDSP_AF_P_sDRAM_CIRCULAR_BUFFER	sPRQ;
	unsigned int					ui32RawPicturePtr[BDSP_FWMAX_VIDEO_BUFF_AVAIL];
	BVENC_VF_sPicParamBuff			*psPPB[BDSP_FWMAX_VIDEO_BUFF_AVAIL]; 
}BRAAGA_TEST_VidEncodeBuffer;

typedef struct BRAAGA_TEST_Task
{
	BDSP_TaskHandle 	task;
	unsigned int		uiStreamId;
	bool				bused;
	bool				bRunning;
	BRAAGA_TEST_TaskDstType	eDstType;
    bool				bXptPbEn;		    	
    unsigned int	    uiPidChnNo;     /* Added PID Channel Number */
   	unsigned int        uiChId;
#if RAAGA_TEST_ENABLE_TRANSPORT
    BXPT_RaveCx_Handle  hCtx;
#endif 
	BAVC_XptContextMap	CtxMap;
	BRAAGA_TEST_VidEncodeBuffer		*psVidEncBuffer;
}BRAAGA_TEST_Task;
typedef struct BRAAGA_TEST_Task			*BRAAGA_TEST_TaskHandle;

/***************************************************************************
Summary:
   Structure defining the state of a raaga_test channel.
***************************************************************************/     
typedef struct BRAAGA_TEST_Channel
{
    BDSP_ContextHandle 	hDspcxt;
    bool                bRunning;       /* TRUE: the channel is running. 
                                           FALSE: channel is stopped. */
	unsigned int 	   uiChId;
    int                 iLeftFd;        /* Left file id: Used for PCM Playback */
    int                 iRightFd;       /* Right file id: Used for PCM Playback */
    unsigned int        uiTotal;        /* Used for PCM Playback */
    unsigned int        uiDataSize;     /* Used for PCM Playback */
    
    BKNI_EventHandle    hPcmWatermarkEvent; /* Event Handle for task */

    BRAAGA_TEST_ChSettings sSettings;
    
   
	BRAAGA_TEST_TaskHandle		hTaskInfo[RAAGA_TEST_MAX_TASK_PER_CONTEXT];
}BRAAGA_TEST_Channel;

typedef struct BRAAGA_TEST_Channel			*BRAAGA_TEST_ChHandle;


typedef enum BRAAGA_TEST_Mode
{
    BRAAGA_TEST_Mode_ConfigFile,
    BRAAGA_TEST_Mode_Manual,
    BRAAGA_TEST_Mode_Auto,
    BRAAGA_TEST_Mode_ArgCmd
} BRAAGA_TEST_Mode;


typedef struct BRAAGA_TEST_StrParams
{
    char                    aStreamName[300];
#if RAAGA_TEST_STATIC_CDB_ITB    
    char                    aCdbFileName[100];
    char                    aItbFileName[100];
#endif
    BAVC_StreamType         eStreamType;
    unsigned int            uiPcrPid;
    unsigned int            uiAudioPid;
    BDSP_AudioType			eAlgoType;
    BDSP_VideoType			eVAlgoType;    
    unsigned int            uiInputBand;
	bool					bUsed;
	uint32_t				ui32OpWidth;
	uint32_t				ui32OpHeight;
}BRAAGA_TEST_StrParams;

typedef struct BRAAGA_TEST_VidEncCfgParams
{
	uint32_t				ui32IpWidth;
	uint32_t				ui32IpHeight;
	uint32_t				ui32FrameRate;
	uint32_t				ui32Profile;
	uint32_t				ui32Level;
	uint32_t				ui32TargetBitRate;
	uint32_t				ui32IntraPeriod;
	uint32_t				ui32IdrPeriod;
	uint32_t				ui32GopStruct;
	bool					bDblkEnable;
	bool					bRateCtrlEnable;
}BRAAGA_TEST_VidEncCfgParams;


/***************************************************************************
Summary:
   Structure defining the state of a raaga_test Device.
***************************************************************************/     
typedef struct BRAAGA_TEST_Device
{
	BCHP_Handle         hChp;
	BREG_Handle         hReg;
	BTMR_Handle         hTmr;
	BMEM_ModuleHandle   hMemModule;     /* new BMEM */
	BMEM_Handle		    hMem;           /* main heap, old  BMEM_Handle */
	BINT_Handle         hInt;

    /* Store the DSP device handle within RAAGA TEST Device */
    BDSP_Handle     		hDsp;
    unsigned int            uiNumStreams;
    BRAAGA_TEST_StrParams   sRaagaStrParams[MAX_STREAMS];
    BRAAGA_TEST_ChHandle    hRaagaTestCh[RAAGA_MAX_CONTEXT];
    unsigned int            uiNumAudioTask;
	
#if RAAGA_TEST_ENABLE_TRANSPORT
    BXPT_Handle                 	hXpt;
	BXPT_Rave_Handle            	hRave;    
	BXPT_PCR_Handle             	hPcr; 
	BXPT_PcrOffset_Handle       	hPcrOff; 
    BRAAGA_TEST_PbLib_Handle      	hPbLib;   
#endif    

	/* Watchdog Recovery */
	BKNI_EventHandle			watchdog_event;
	pthread_t					*watchdog_thread;
	bool						bXptPbEn;
	bool						bVidEnable; 
    bool                        bAudEnable;
	bool						bVidEncodeEnable;
    BRAAGA_TEST_Mode            eTestmode;
	
	BRAAGA_TEST_Feeder_Handle	hFeeder;

	BRAAGA_TEST_VidEncCfgParams	sVidEncodeParams;
	/* Adding Memory Range checker. Helpful in finding mem violation */
#if (BTST_P_USE_MRC_MONITOR)		
	BMRC_Handle                   hMrc;
	BMRC_Monitor_Handle           hMonitor;
#endif 

}BRAAGA_TEST_Device;


typedef struct BRAAGA_TEST_Device			*BRAAGA_TEST_Handle;

extern BRAAGA_TEST_Handle             hGlobalRaagaTestHandle;
extern BCHP_Handle   g_hChip;
extern BREG_Handle   g_hRegister;
extern BMEM_Handle   g_hMemory;
extern BINT_Handle   g_hInterrupt;
extern BTMR_Handle   g_hTmr;

/* Debug Stuff */
const char *bsettop_get_config(const char *name);
#if BDBG_DEBUG_BUILD
void BRAAGA_TEST_SetModuleDebugLevel(const char	*modulelist, BDBG_Level	level);
#endif

void RAAGA_TEST_ProgramGpio(BREG_Handle	hReg);
void BRAAGA_TEST_ProgramGenClk(BREG_Handle	hReg);
void BRAAGA_TEST_ProgramHifiDac(BREG_Handle hReg);
void BRAAGA_TEST_ProgrameVCXO(BREG_Handle	hReg);
void BRAAGA_TEST_ProgramFmmMisc(BREG_Handle hReg);
void BRAAGA_TEST_ProgramMS(BREG_Handle		hReg);
void BRAAGA_TEST_ProgramOP(BREG_Handle		hReg);
void BRAAGA_TEST_ProgramDP(BREG_Handle		hReg);
void BRAAGA_TEST_GroupIOP(BREG_Handle		hReg);
void BRAAGA_TEST_ProgramSRC(BREG_Handle hReg);
void BRAAGA_TEST_ProgramBF(BREG_Handle	hReg);
void BRAAGA_TEST_EnableFmmPath(BREG_Handle	hReg);

BERR_Code BRAAGA_TEST_Uninit(
		BRAAGA_TEST_Handle            hRaagaTest
);

BERR_Code BRAAGA_TEST_XptInit(
	BRAAGA_TEST_Handle            hRaagaTest,
    BRAAGA_TEST_ChHandle          hRaagaTestCh,
    BCHP_Handle                 hChip,
	BMEM_Handle                 hMemory,
	BINT_Handle                 hInt,
	BREG_Handle                 hReg
);

BERR_Code BRAAGA_TEST_DspInit(
    BRAAGA_TEST_Handle          hRaagaTest,
    BCHP_Handle                 hChip,
	BMEM_Handle                 hMemory,
	BINT_Handle                 hInt,
	BREG_Handle                 hReg,
	BTMR_Handle		            hTmr
);

BERR_Code BRAAGA_TEST_OpenRaagaContext(
    BRAAGA_TEST_Handle      hRaagaTest,
	BRAAGA_TEST_CxtType		eChTaskType    
);

BERR_Code BRAAGA_TEST_StartRaagaTask(
	BRAAGA_TEST_Handle		hRaagaTest,
	BRAAGA_TEST_TaskHandle	hTaskHandle,
	BRAAGA_TEST_CxtType		eChTaskType,    
    unsigned int			uiStreamId,
	BRAAGA_TEST_TaskDstType	eDstType,
	bool					bXptPbEn
);

BERR_Code BRAAGA_TEST_OpenRaagaTask(
	BRAAGA_TEST_Handle		hRaagaTest,
	BRAAGA_TEST_TaskHandle	*hTaskHandle,
	BRAAGA_TEST_CxtType		eChTaskType,    
    unsigned int			uiStreamId,
	BRAAGA_TEST_TaskDstType	eDstType
);

BERR_Code BRAAGA_TEST_StopRaagaTask(
	BRAAGA_TEST_Handle		hRaagaTest,
	BRAAGA_TEST_TaskHandle	hTaskHandle
);

BERR_Code BRAAGA_TEST_CloseRaagaContext(
    BRAAGA_TEST_Handle      hRaagaTest,
	BRAAGA_TEST_ChHandle	hRaagaTestCh
);

BERR_Code BRAAGA_TEST_CloseRaagaTask(
	BRAAGA_TEST_Handle		hRaagaTest,
	BRAAGA_TEST_ChHandle	hRaagaTestCh,
	BRAAGA_TEST_TaskHandle	hTaskHandle
);





void TestRegAccessByHost( void);
void TestDramAccessByHost( void);
void BRAAGA_TEST_ManualTestMenu (BRAAGA_TEST_Handle    hRaagaTest);


