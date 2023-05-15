/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_fwdownload_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 11/26/12 1:19p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_fwdownload_priv.h $
 * 
 * Hydra_Software_Devel/6   11/26/12 1:19p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/5   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/4   1/27/12 1:19p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
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
 * Hydra_Software_Devel/4   1/20/11 8:10a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/3   1/19/11 7:14a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/2   1/18/11 4:21a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/1   1/13/11 5:36a gautamk
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
#ifndef BDSP_RAAGA_FWDOWNLOAD_PRIV_H__
#define BDSP_RAAGA_FWDOWNLOAD_PRIV_H__


#include "bdsp_raaga_fw_cit.h"
#include "bdsp_raaga_fw.h"
#include "bdsp_raaga_types.h"
#include "bdsp_task.h"
#include "bdsp_raaga_img.h"




/***************************************************************************
Summary:
    This enumeration defines various Executable types at the Task Node (Stages).
***************************************************************************/
typedef enum BDSP_Raaga_P_ExecType
{
    BDSP_Raaga_P_ExecType_eDecode,      /* Decode Exec */
    BDSP_Raaga_P_ExecType_ePassthru,    /* Pass thru Exec */
    BDSP_Raaga_P_ExecType_eFrameSync,   /* FrameSync Exec */
    BDSP_Raaga_P_ExecType_eEncode,      /* Encode Exec */
    BDSP_Raaga_P_ExecType_eProcessingAlgo,  /* Processing Algo Exec */
    BDSP_Raaga_P_ExecType_eTsm,         /* Tsm Exec */
    BDSP_Raaga_P_ExecType_eVideoDecode, /* Video Decode */    
    BDSP_Raaga_P_ExecType_eScm, 		/* SCM */    
    BDSP_Raaga_P_ExecType_eVideoEncode, /* Video Encode */
    BDSP_Raaga_P_ExecType_eInvalid  = 0xFF
} BDSP_Raaga_P_ExecType;


/******************************************************************************
Summary:
	This Function returns true, If the Processing algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/

bool BDSP_Raaga_P_IsAudioProcessingSupported(
                                    unsigned int algoId);

/******************************************************************************
Summary:
	This Function returns true, If the video processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsVideoTypeSupported(unsigned int AlgoId);

/******************************************************************************
Summary:
	This Function returns true, If the video encode algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsVideoEncodeTypeSupported(unsigned int AlgoId);

/******************************************************************************
Summary:
	This Function returns true, If the audio processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsAudioTypeSupported(
					unsigned int algoId);

/******************************************************************************
Summary:
	This Function returns true, If the Encode algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsAudioEncodeSupported(
                                    unsigned int algoId);
/******************************************************************************
Summary:
	This Function returns true, If the Scm processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsScmTypeSupported(unsigned int AlgoId);

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Decode Mode and Decode algo type.
*******************************************************************************/
void BDSP_Raaga_P_GetExecIds(
						BDSP_DecodeMode decMode, 
						BDSP_AudioType audType, 
						BDSP_AF_P_sALGO_EXEC_INFO *pExecID /* [out] */);

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Decode Mode and audio processing algorithm type.
*******************************************************************************/

void  BDSP_Raaga_P_GetAudProcExecIds(
				BDSP_AudioProcessing audProcAlgo,	/* [in]Audio processing algorithm */
				BDSP_AF_P_sALGO_EXEC_INFO *pAudProcExecID /* [out] */);

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Encode algorithm type.
*******************************************************************************/
void  BDSP_Raaga_P_GetEncodeExecIds(
				BDSP_AudioEncode encodeAlgo,	/* [in]Audio processing algorithm */
				BDSP_AF_P_sALGO_EXEC_INFO *pEncodeExecID /* [out] */);

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Video algorithm type.
*******************************************************************************/
void  BDSP_Raaga_P_GetVideoExecIds(
                BDSP_DecodeMode decMode, 
				BDSP_VideoType vidType,	/* [in]Video processing algorithm */
				BDSP_AF_P_sALGO_EXEC_INFO *pEncodeExecID /* [out] */);
/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Scm algorithm type.
*******************************************************************************/
void  BDSP_Raaga_P_GetScmExecIds(
				BDSP_ScmType scmType,	/* [in]Scm processing algorithm */
				BDSP_AF_P_sALGO_EXEC_INFO *pScmExecID /* [out] */);

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular video encode Mode and video encode algo type.
*******************************************************************************/
void BDSP_Raaga_P_GetVideoEncodeExecIds(
						BDSP_EncodeMode encMode, 
						BDSP_VideoEncode vidEncType, 
						BDSP_AF_P_sALGO_EXEC_INFO *pExecID /* [out] */);

/*******************************************************************************
Summary:
	This function returns if the Algo table is already downloaded or not. If its
	downloaded, it sets isDownloaded=true and eCodeFirmwareId to the Algo Code 
	with which the table was associated in the MIT when it was downloaded earlier.
*******************************************************************************/
void BDSP_Raaga_P_IsTableDownloaded(
					BDSP_Raaga_P_ExecType	execType,	
					BDSP_AF_P_AlgoId	 codeExecId,
					bool		*isDownloaded,
					BDSP_AF_P_AlgoId *eExecId);

/*******************************************************************************
Summary:
	This returns if the audio processing needs to be downloaded at Device open 
	time or Channel Start time.
*******************************************************************************/
void BDSP_Raaga_P_IfOpenTimeProcDownload(
						bool *bOpenTimeAudProcDownload);

BERR_Code BDSP_Raaga_P_DownloadFwExec(
	void *pDeviceHandle);

BERR_Code BDSP_Raaga_P_DownloadStartTimeFWExec(
                    void *pContextHandle, 
                    const BDSP_TaskCreateSettings *pSettings);

BERR_Code BDSP_Raaga_P_DownloadAudProcessingExecs(
	void *pDeviceHandle);

/******************************************************************************
Summary:
	This Function Copies the FW exec from Image interface to buffer in the DRAM.
*******************************************************************************/

BERR_Code BDSP_Raaga_P_CopyFWImageToMem(
		const BIMG_Interface *iface,
		void *pImgContext,
		uint32_t ui32MemAddr,	/* 0 , If its a offline utility */
		BDSP_Img_Id firmware_id,
		BMEM_Handle hHeap);

BERR_Code BDSP_Raaga_P_FreeStartTimeFWCtxt(
    void *pTaskHandle);


#endif

