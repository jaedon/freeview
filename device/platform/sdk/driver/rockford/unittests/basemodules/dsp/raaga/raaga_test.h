/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test.h $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 9/17/12 2:21p $
 *
 * Module Description:
 *          This file contains all structures and function prototypes used 
 *          by raaga_test.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/raaga_test.h $
 * 
 * Hydra_Software_Devel/27   9/17/12 2:21p gprasad
 * SW7425-3952: Fixing build errors with raaga test for emulation build
 * 
 * Hydra_Software_Devel/26   6/18/12 12:40p gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/25   6/14/12 12:16p gprasad
 * FWRAA-382: Adding support to provide the path to ring buffer capture
 * files
 * 
 * Hydra_Software_Devel/24   5/18/12 8:29a gprasad
 * FWRAA-382: Adding change to not use capture buffer in afap mode
 * 
 * Hydra_Software_Devel/23   2/29/12 5:54p gprasad
 * FWRAA-398: Adding support in raaga_test for AFAP decoding mode
 * 
 * Hydra_Software_Devel/22   2/22/12 2:44p gprasad
 * FWRAA-398: Add changes required for running audio certification tests
 * to raaga_test
 * 
 * Hydra_Software_Devel/21   1/25/12 6:20p gprasad
 * SW7360-2: Fixing issue with streamer playback in raaga test app
 * 
 * Hydra_Software_Devel/20   1/17/12 2:34p gprasad
 * FWRAA-342: enabling momo output
 * 
 * Hydra_Software_Devel/19   1/16/12 12:46p ashoky
 * SW7435-5: Merging raaga_test to mainline.
 * 
 * Hydra_Software_Devel/18   1/13/12 8:08p gprasad
 * FWRAA-342: Adding support for configuring AEC for speech encoders in
 * raaga test
 * 
 * Hydra_Software_Devel/17   1/10/12 4:37p gprasad
 * FWRAA-370: Adding support for encoder testing and output buffer capture
 * to raaga_test
 * 
 * Hydra_Software_Devel/16   1/5/12 12:47p gprasad
 * FWRAA-370: Adding changes to configure HIFIDAC RM based on sample rate
 * interrupt
 * 
 * Hydra_Software_Devel/15   11/29/11 12:10p ashoky
 * SW7429-18: Fixing build issue.
 * 
 * Hydra_Software_Devel/14   11/25/11 3:15p ashoky
 * SW7429-18: Fixing raaga_test build issue.
 * 
 * Hydra_Software_Devel/13   9/26/11 4:20p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/Raaga_Test_Automation/2   9/11/11 10:31p purush
 * SW7425-1063: [7425] Raaga test development for automation
 * 
 * Hydra_Software_Devel/Raaga_Test_Automation/1   8/8/11 4:59p purush
 * SW7425-1063: [7425] Raaga test development for automation
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
/* DSP includes */
#include "bdsp.h"
#include "bdsp_raaga.h"
#include "bdsp_raaga_fw.h"
#include "bdsp_raaga_fw_cit.h"
#include "bxdm_picture.h"

/* includes for configuring FMM */
#include "bchp_common.h"
#include "bchp_aud_fmm_bf_ctrl.h"
#include "bchp_aud_fmm_dp_ctrl0.h"
#include "bchp_aud_fmm_src_ctrl0.h"
#include "bchp_aud_fmm_src_esr0.h"

#ifdef BCHP_HIFIDAC_CTRL0_REG_START
/* Old Style RDB */
#include "bchp_hifidac_ctrl0.h"
#include "bchp_hifidac_esr0.h"
#include "bchp_hifidac_rm0.h"
#else
#include "bchp_hifidac_ctrl_0.h"
#include "bchp_hifidac_esr_0.h"
#include "bchp_hifidac_rm_0.h"
#endif

#ifdef BCHP_AIO_MISC_REG_START
#include "bchp_aio_misc.h"
#include "bchp_aud_fmm_misc.h"
#else
#include "bchp_aud_misc.h"
#endif

#ifdef BCHP_AUD_FMM_BF_ESR0_D0_REG_START
/* Old Style RDB */
#include "bchp_aud_fmm_bf_esr0_d0.h"
#include "bchp_aud_fmm_bf_esr0_h.h"
#include "bchp_aud_fmm_bf_esr1_d0.h"
#include "bchp_aud_fmm_bf_esr1_h.h"
#include "bchp_aud_fmm_bf_esr2_d0.h"
#include "bchp_aud_fmm_bf_esr2_h.h"
#else
#include "bchp_aud_fmm_bf_esr.h"
#endif

#ifdef BCHP_AIO_INTH_REG_START
/* Old Style RDB */
#include "bchp_aio_inth.h"
#include "bchp_aio_intd0.h"
#else
#include "bchp_aud_inth.h"
#endif

#ifdef BCHP_AUD_FMM_OP_CTRL_REG_START
#include "bchp_aud_fmm_op_ctrl.h"
#include "bchp_aud_fmm_op_esr.h"
#else
#include "bchp_aud_fmm_iop_out_dac_ctrl_0.h"
#endif

#ifdef BCHP_AUD_FMM_IOP_CTRL_REG_START
#include "bchp_aud_fmm_iop_ctrl.h"
#include "bchp_aud_fmm_iop_esr.h"
#else
#include "bchp_aud_fmm_iop_out_ms_strm_cfg_0.h"
#include "bchp_aud_fmm_iop_out_ms_strm_esr_0.h"
#endif

#ifdef BCHP_AUD_FMM_MS_CTRL_REG_START
#include "bchp_aud_fmm_ms_ctrl.h"
#include "bchp_aud_fmm_ms_esr.h"
#else
#include "bchp_aud_fmm_iop_out_ms_ctrl_0.h"
#include "bchp_aud_fmm_iop_out_ms_esr_0.h"
#endif

#if 0
#undef BDBG_MSG
#define BDBG_MSG	BDBG_ERR
#endif
#ifndef FIREPATH_BM
#define RAAGA_TEST_ENABLE_TRANSPORT 			1 /* Set this to 1 if you want to enable Xpt */
#endif
#define RAAGA_TEST_STATIC_CDB_ITB   			1 /* Set this to 1 if you want to use static CDB-ITB */
#define RAAGA_FWAUTHENTICATION_ENABLE           0 /* Set this to 1 if you want to test FW authentication */

#define MAX_STREAMS                 			2
#define XPT_DUMP_ENABLED    					0 /* Change this to 1 if you want to dump the 
                                       			XPT related info */
#define RAAGA_TEST_PROFILING_SUPPORT 			0
#define RAAGA_TEST_STATIC_CDB_ITB_BINARYFILE 	1

#define RAAGA_TEST_XPT_CHECK (RAAGA_TEST_ENABLE_TRANSPORT^RAAGA_TEST_STATIC_CDB_ITB)
                                    /* XPT CHECK should be 1: meaning either XPT
                                       or static CDB-ITB is enabled not both */
#define RAAGA_TEST_ENABLE_FRONTEND_INPUT 		1
#define RAAGA_TEST_MAX_FW_TASK_PER_DSP			7
#define	RAAGA_TEST_MAX_TASK_PER_CONTEXT			3

#define RAAGA_TEST_AUTOMATION_MAX_TEST_CASES	25

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


#if RAAGA_TEST_ENABLE_TRANSPORT
/* XPT includes */
#include "bxpt_priv.h"
#include "bxpt.h"
#include "bxpt_rave.h"
#include "bxpt_pcr.h"
#include "bxpt_pcr_offset.h"
#include "xpt_pb.h"
#if RAAGA_TEST_DIRECTV_SUPPORT
#include "bxpt_directv.h"
#include "bxpt_directv_pcr.h"
#include "bxpt_directv_pcroffset.h"
#endif
#endif

/* Register includes */
#include "bchp_xpt_rave.h"
#include "bchp_sun_top_ctrl.h"
#if (BCHP_CHIP == 7422)||(BCHP_CHIP == 7425)||(BCHP_CHIP == 7231)||(BCHP_CHIP == 7435)
#include "bchp_aon_pin_ctrl.h"
#endif
#include "bchp_gio.h"

#if (RAAGA_TEST_STATIC_CDB_ITB == 1 && RAAGA_TEST_ENABLE_TRANSPORT == 0)
#define BXPT_Handle uint32_t
#define BXPT_PCR_Handle uint32_t
#define BXPT_PcrOffset_Handle uint32_t
#define BXPT_Rave_Handle uint32_t
#define BRAAGA_TEST_PbLib_Handle uint32_t
#endif

#define	BRAAG_TEST_CMD_SIZE		50

extern BDSP_AudioProcessing g_pp_audio_type;
extern BDSP_AudioEncode     g_encoder_audio_type;
extern bool g_enable_echo_canceller;
extern bool g_capture_encoder_out;
extern bool g_capture_decoder_stereo;
extern bool g_capture_decoder_multich;
extern bool g_afap_mode;

/****** VCS Support *****/
#define BDSP_MONO_DISTINCT_OP_TYPE  BDSP_AF_P_DistinctOpType_eMono_PCM

/* Use export raaga_test_enable_echo_canceller=1 to enable echo canceller at run time */

/* Use export raaga_test_encoder_audio_type="algo_id" to enable encoder at run time */

/* Use export raaga_test_pp_audio_type="algo_id" to enable any pp except aec at run time */

/* Use export raaga_test_capture_encoder_out=1 to enable encoder output capture run time */

/* Use export raaga_test_capture_decoder_stereo=1 to enable decoder stereo output capture run time */

/* Use export raaga_test_capture_decoder_multich=1 to enable decoder multi-ch output capture run time */

/* Use export raaga_test_enable_afap_mode=1 to enable AFAP decoding mode - This will take effect only
 * if stereo/multi-ch PCM capture is enabled */

/*
export raaga_test_encoder_audio_type="0"
export raaga_test_pp_audio_type="0"
export raaga_test_enable_echo_canceller=1
export raaga_test_capture_encoder_out=1
export raaga_test_capture_decoder_stereo=1
export raaga_test_capture_decoder_multich=1
*/

/* Enable this define to write the buffer capture file fromt eh capture task */
#if 0
    #define  WRITE_FILE_FROM_CAPTURE_TASK
#endif

#include "btst_kni.h"
#include "btst.h"

#define MAX_NUM_CAPTURES                (uint32_t)4
#define BRGA_TST_FILE_WRITE_BUFFER_SIZE (uint32_t)(7*1024*1024)

#define READ_UPDATE     0
#define WRITE_UPDATE    1

typedef struct BRAAGA_TEST_CaptureChannelInfo
{
    bool                                running;
    bool                                enabled;
	FILE								*OutFilePtr[BDSP_AF_P_MAX_CHANNELS];
    BDSP_AF_P_sIO_BUFFER                sIoBfrDesc;
    uint32_t                            readPointers[BDSP_AF_P_MAX_CHANNELS];
    bool                                wrapBit[BDSP_AF_P_MAX_CHANNELS];
    /* This element is required for FMM captures since the capture can start only after wr = st_wr */
    bool                                start_capture;
    bool                                move_read;
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER     sLocalBuffers[BDSP_AF_P_MAX_CHANNELS];
} BRAAGA_TEST_CaptureChannelInfo;

typedef struct BRAAGA_TEST_CaptureStruct
{
    uint32_t                            numCaptures;
    
	BTST_TaskHandle						hCaptureTask;	
	BTST_TaskHandle						hFileWriteTask;	
	
	BCHP_Handle							hChp;
	BMEM_Handle							hMem;
	BINT_Handle							hInt;
	BREG_Handle							hReg;

	BDSP_TaskHandle						hDspTask;

    BRAAGA_TEST_CaptureChannelInfo      capChanInfo[MAX_NUM_CAPTURES];

}BRAAGA_TEST_CaptureStruct;

extern BRAAGA_TEST_CaptureStruct captureInfo;
extern BRAAGA_TEST_CaptureStruct *pcaptureInfo;

uint32_t BRGA_TST_Capture_Add(BDSP_AF_P_sIO_BUFFER *pBufDesc, const char *outFile, bool capture, BREG_Handle hReg);
BERR_Code BRGA_TST_Capture_Start(uint32_t index);
BERR_Code BRGA_TST_Capture_Stop(uint32_t index);
uint32_t BRGA_TST_Capture_Remove(uint32_t index);
void BRGA_TST_Capture_RemoveAll(void);
void BRGA_TST_BufferCaptureTask(BTST_TaskHandle Task, void *Parm);
void BRGA_TST_GetBufferStatus(BDSP_AF_P_sIO_BUFFER *pBuffDesc, bool *wrapBit, uint32_t *readPointers, uint32_t *size, BREG_Handle hReg);
uint32_t BRGA_TST_GetUpdatedReadPtr(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer, BDSP_AF_P_BufferType eBufferType, uint32_t ui32ReadPtr, uint32_t Size, BREG_Handle hReg);
void BRGA_TST_UpdateReadPtr(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer, BDSP_AF_P_BufferType eBufferType, uint32_t ui32ReadPtr, BREG_Handle hReg);
void BRGA_TST_TransFmmBuffId2Addr(BDSP_CIT_P_sIO_BUFF_ID *psIoBuffId, BDSP_AF_P_sIO_BUFFER *psIoBuffer);
bool BRGA_TST_IsSpeechEncoder(BDSP_AudioEncode eAudioType);

void BRGA_TST_UpdateFileWriteBuffer(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer, uint32_t ui32AddrType, uint32_t ui32Size);
uint32_t BRGA_TST_GetFileWriteBufferInfo(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer);
void BRGA_TST_WriteDataToFileWritebuffer (void *pBuffer, BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer, uint32_t ui32Size);
void BRGA_TST_FileWriteTask(BTST_TaskHandle Task, void *Parm);

#define PP_USER_CFG_FILE                "pp_usercfg.bin"
#define DEC_USER_CFG_FILE               "dec_usercfg.bin"
#define ENC_USER_CFG_FILE               "enc_usercfg.bin"

extern BDSP_AF_P_sNODE_INFO         BDSP_sNodeInfo[BDSP_AF_P_AlgoId_eMax];
extern BDSP_AF_P_sALGO_EXEC_INFO    BDSP_sDecAlgoExecInfo[BDSP_AudioType_eMax][BDSP_DecodeMode_eMax];
extern BDSP_AF_P_sALGO_EXEC_INFO    BDSP_sEncAlgoExecInfo[BDSP_AudioEncode_eMax][BDSP_EncodeMode_eSimulMode];
extern BDSP_AF_P_sALGO_EXEC_INFO    BDSP_sPpAlgoExecInfo[BDSP_AudioProcessing_eMax][BDSP_DSPCHN_ProcessingMode_eMax];

#define ECHO_CANCELLER_INPUT_FILE       "ec_file.bin"
#define INTER_TASK_IO_BUFFER_SIZE       (uint32_t)1024*1024
#define INTER_TASK_GENERIC_BUFFER_SIZE  (uint32_t)1024*1024

/***********************/

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
    BRAAGA_TEST_CxtType_eGraphics,
    BRAAGA_TEST_CxtType_eSecurity,
    BRAAGA_TEST_CxtType_eMax
} BRAAGA_TEST_CxtType;

#define	RAAGA_MAX_CONTEXT	2 /*BRAAGA_TEST_CxtType_eMax*/

/**************************************************************************
Following defines are chip specific.
***************************************************************************/
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR
/* Old Style RDB */
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_FREEFULL_MARK
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_BASEADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_2_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_RDADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_2_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_WRADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_2_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_ENDADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_2_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_RINGBUF_2_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_RINGBUF_2_FREEFULL_MARK
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_BASEADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_3_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_RDADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_3_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_WRADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_3_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_ENDADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_3_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_RINGBUF_3_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_RINGBUF_3_FREEFULL_MARK
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_BASEADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_4_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_RDADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_4_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_WRADDR						BCHP_AUD_FMM_BF_CTRL_RINGBUF_4_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_ENDADDR					BCHP_AUD_FMM_BF_CTRL_RINGBUF_4_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_RINGBUF_4_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_RINGBUF_4_FREEFULL_MARK
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_27_MI_VALID					BCHP_AUD_FMM_BF_CTRL_RINGBUF_27_MI_VALID
#else
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_FREE_MARK
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_FREE_MARK
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_BASEADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_RDADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_WRADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_ENDADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_2_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_FREE_MARK
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_BASEADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_RDADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_WRADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_ENDADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_3_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_FREE_MARK
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_BASEADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_BASEADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_RDADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_RDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_WRADDR						BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_WRADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_ENDADDR					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_ENDADDR
#define BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_START_WRPOINT				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_START_WRPOINT
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_4_FREEFULL_MARK				BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_FREE_MARK
#define	BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_27_MI_VALID					BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_27_MI_VALID
#endif

#ifdef BCHP_AUD_FMM_MS_CTRL_REG_START
#define	BRAAGA_FMM_MS_CTRL_0_USEQ_INSTi_ARRAY_END					BCHP_AUD_FMM_MS_CTRL_USEQ_INSTi_ARRAY_END
#else
#define	BRAAGA_FMM_MS_CTRL_0_USEQ_INSTi_ARRAY_END					BCHP_AUD_FMM_IOP_OUT_MS_CTRL_0_USEQ_INSTi_ARRAY_END
#endif

#ifdef BCHP_HIFIDAC_CTRL0_REG_START
#define	BRAAGA_HIFIDAC_CTRL0_CONFIG									BCHP_HIFIDAC_CTRL0_CONFIG
#define	BRAAGA_HIFIDAC_CTRL0_MUTECTRL								BCHP_HIFIDAC_CTRL0_MUTECTRL
#define	BRAAGA_HIFIDAC_CTRL0_MUTECTRL_DACONLY                       BCHP_HIFIDAC_CTRL0_MUTECTRL_DACONLY
#define	BRAAGA_HIFIDAC_RM0_RATE_RATIO                               BCHP_HIFIDAC_RM0_RATE_RATIO
#define	BRAAGA_HIFIDAC_RM0_SAMPLE_INC                               BCHP_HIFIDAC_RM0_SAMPLE_INC
#define	BRAAGA_HIFIDAC_RM0_PHASE_INC                                BCHP_HIFIDAC_RM0_PHASE_INC
#define	BRAAGA_HIFIDAC_RM0_CONTROL                                  BCHP_HIFIDAC_RM0_CONTROL
#define	BRAAGA_HIFIDAC_CTRL0_TEST                                   BCHP_HIFIDAC_CTRL0_TEST
#define	BRAAGA_HIFIDAC_CTRL0_CURRDAC_CTRL                           BCHP_HIFIDAC_CTRL0_CURRDAC_CTRL
#define	BRAAGA_HIFIDAC_CTRL0_PEAK_A1                                BCHP_HIFIDAC_CTRL0_PEAK_A1
#define	BRAAGA_HIFIDAC_CTRL0_PEAK_A2                                BCHP_HIFIDAC_CTRL0_PEAK_A2
#define	BRAAGA_HIFIDAC_CTRL0_PEAK_GAIN                              BCHP_HIFIDAC_CTRL0_PEAK_GAIN
#define	BRAAGA_HIFIDAC_CTRL0_RANGE                                  BCHP_HIFIDAC_CTRL0_RANGE
#else
#define	BRAAGA_HIFIDAC_CTRL0_CONFIG									BCHP_HIFIDAC_CTRL_0_CONFIG
#define	BRAAGA_HIFIDAC_CTRL0_MUTECTRL								BCHP_HIFIDAC_CTRL_0_MUTECTRL
#define	BRAAGA_HIFIDAC_CTRL0_MUTECTRL_DACONLY                       BCHP_HIFIDAC_CTRL_0_MUTECTRL_DACONLY
#define	BRAAGA_HIFIDAC_RM0_RATE_RATIO                               BCHP_HIFIDAC_RM_0_RATE_RATIO
#define	BRAAGA_HIFIDAC_RM0_SAMPLE_INC                               BCHP_HIFIDAC_RM_0_SAMPLE_INC
#define	BRAAGA_HIFIDAC_RM0_PHASE_INC                                BCHP_HIFIDAC_RM_0_PHASE_INC
#define	BRAAGA_HIFIDAC_RM0_CONTROL                                  BCHP_HIFIDAC_RM_0_CONTROL
#define	BRAAGA_HIFIDAC_CTRL0_TEST                                   BCHP_HIFIDAC_CTRL_0_TEST
#define	BRAAGA_HIFIDAC_CTRL0_CURRDAC_CTRL                           BCHP_HIFIDAC_CTRL_0_CURRDAC_CTRL
#define	BRAAGA_HIFIDAC_CTRL0_PEAK_A1                                BCHP_HIFIDAC_CTRL_0_PEAK_A1
#define	BRAAGA_HIFIDAC_CTRL0_PEAK_A2                                BCHP_HIFIDAC_CTRL_0_PEAK_A2
#define	BRAAGA_HIFIDAC_CTRL0_PEAK_GAIN                              BCHP_HIFIDAC_CTRL_0_PEAK_GAIN
#define	BRAAGA_HIFIDAC_CTRL0_RANGE                                  BCHP_HIFIDAC_CTRL_0_RANGE
#endif
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

}BRAAGA_TEST_ChSettings;
typedef enum BRAAGA_TEST_TaskDstType
{
	BRAAGA_TEST_TaskDstType_eFwStg,			/* Source or destination is another FW stage */
	BRAAGA_TEST_TaskDstType_eRaveBuf,		/* Source or destination is an IO buffer */
	BRAAGA_TEST_TaskDstType_eFMMBuf,			/* Source or destination is an IO buffer */
	BRAAGA_TEST_TaskDstType_eRDB,	
	BRAAGA_TEST_TaskDstType_eDRAMBuf		
} BRAAGA_TEST_TaskDstType;

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
	BAVC_XptContextMap	CtxMap1;
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
    char                    aStreamName[500];
#if RAAGA_TEST_STATIC_CDB_ITB    
    char                    aCdbFileName[500];
    char                    aItbFileName[500];
#endif
    BAVC_StreamType         eStreamType;
    unsigned int            uiPcrPid;
    unsigned int            uiAudioPid;
    BDSP_AudioType			eAlgoType;
    BDSP_VideoType			eVAlgoType;    
    unsigned int            uiInputBand;
	bool					bUsed;
}BRAAGA_TEST_StrParams;

/*Structure defining the parameters of a test case to be used during config file usage for automation.*/
typedef struct BRAAGA_TEST_CaseInfo
{	
	BRAAGA_TEST_StrParams TestStreamInfo;	
	unsigned int 		StreamRefFrameCount;		
	unsigned int 		StreamDecodedFrameCount;
} BRAAGA_TEST_CaseInfo;


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
    bool						bFilePb;
	bool						bVidEnable; 
    bool                        bAudEnable;
    BRAAGA_TEST_Mode            eTestmode;
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


void BRAAGA_TEST_ConfigureSampleRate(
    void *pParam1,
    int param2,
    unsigned streamSampleRate,
    unsigned baseSampleRate
);



void TestRegAccessByHost( void);
void TestDramAccessByHost( void);
void BRAAGA_TEST_ManualTestMenu (BRAAGA_TEST_Handle    hRaagaTest);


