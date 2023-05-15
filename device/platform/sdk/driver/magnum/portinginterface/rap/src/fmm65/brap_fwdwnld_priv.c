/***************************************************************************
*     Copyright (c) 2006-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_fwdwnld_priv.c $
* $brcm_Revision: Hydra_Software_Devel/83 $
* $brcm_Date: 4/18/11 1:51p $
*
* Module Description:
*	This file contains FW Downloading Code.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_fwdwnld_priv.c $
* 
* Hydra_Software_Devel/83   4/18/11 1:51p gautamk
* SW7420-1791: [7405] iface->close should not be called if it is not
* open.
* 
* Hydra_Software_Devel/82   4/18/11 1:44p gautamk
* SW7420-1791: [7405] iface->close should not be called if it is not
* open.
* 
* Hydra_Software_Devel/81   3/18/11 9:40p sgadara
* SW7420-767: [7420] Merge MS11 development branch to mainline
* 
* Hydra_Software_Devel/80   3/7/11 6:03p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/79   1/12/11 3:55p bselva
* SW7468-346: [7468]Fixed the memory requirement calculation for video FW
* files.
* 
* Hydra_Software_Devel/78   10/21/10 7:08p srajapur
* SW7550-601 : [7550] Adding PI Support for BTSC Encoder
* 
* Hydra_Software_Devel/77   10/7/10 5:02p gautamk
* SW7405-4807: [7405] Fixing KLOCKWORK issues  SW7405-4834: [7405] Fixing
* KLOCKWORK issues
* 
* Hydra_Software_Devel/76   9/16/10 12:18a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/75   9/6/10 5:15p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/74   9/6/10 3:43p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code.
* Merging to mainline
* 
* Hydra_Software_Devel/RAP_7550_TASK_DOWNLOAD_BRANCH/1   8/31/10 3:36p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code
* 
* Hydra_Software_Devel/73   8/13/10 5:23p gautamk
* SW7405-4713:[7405] Merging changes for RDB indepedence in Main line
* 
* Hydra_Software_Devel/RAP_RDB_INDEP_FW/1   8/3/10 1:38p gautamk
* SW7405-3359: [7405] Adding support for RDB indepdence for FW
* 
* Hydra_Software_Devel/72   5/24/10 8:27p sgadara
* SW3548-2923: [3548,3556] Adding Support for SRS StudioSound.
* StudioSound  --> CsTd+TSHD+TVOL+EqHl
* 
* Hydra_Software_Devel/71   4/26/10 1:59p srajapur
* SW3548-2899 : [3548] Added PI support for ABX post processing
* 
* Hydra_Software_Devel/70   4/22/10 8:59p srajapur
* SW7405-3993 : [3548] Adding Audyssey Volume post processing algorithm
* 
* Hydra_Software_Devel/69   4/22/10 7:56p sgadara
* SW7405-4170:[7405,3548,3556] Making chages to internally map AACHE and
* AC3 Encode to Dolby Pulse and DD Transcode in PI for MS10 license
* 
* Hydra_Software_Devel/68   4/6/10 2:37p gautamk
* SW7405-4170: [7405] Adding FWMixer processing stage.
* 
* Hydra_Software_Devel/67   3/12/10 2:47p speter
* SW3548-2827:[3548] Adding Mono Downmix support as post processing
* algorithm
* 
* Hydra_Software_Devel/66   3/11/10 5:02p speter
* SW3548-2822:[3548] Add Dolby Transcode and Broadcom 3D surround post
* processing algos
* 
* Hydra_Software_Devel/65   3/10/10 5:05p speter
* SW3548-2819:[3548] Adding Dolby Pulse Algorithm
* 
* Hydra_Software_Devel/64   2/19/10 5:20p gautamk
* SW7468-119: [7468] Adding RAP PI support for Real audio LBR
* 
* Hydra_Software_Devel/63   12/30/09 3:20p gautamk
* SW7405-2001: [7405] removing non-const variable
* 
* Hydra_Software_Devel/62   9/23/09 6:12p gautamk
* SW7420-357: [7420] Adding support for SBC Encode
* 
* Hydra_Software_Devel/61   9/18/09 3:42p gautamk
* SW7405-2606: [7405] Fixing coverity issue.
* 
* Hydra_Software_Devel/60   9/18/09 3:03p gautamk
* SW7405-3030: [7405] If FwAuthenable is true, downloading encode
* correctly.
* 
* Hydra_Software_Devel/59   9/11/09 6:46p sgadara
* SW3548-2188: [3548,3556] Adding support for AMR decode algorithm.
* 
* Hydra_Software_Devel/58   9/8/09 2:32p gautamk
* SW7405-2986: [7405]Enabinge DTS Broadcast Pasthru without DTS
* Licensing. Also Enable Other Passthru which can be allowed without
* license
* 
* Hydra_Software_Devel/57   7/15/09 11:03a gautamk
* PR56006: [7405] Adding support for mp3 encode in rap pi
* 
* Hydra_Software_Devel/56   6/23/09 2:33p gautamk
* PR56006: [7405] Fixing coverity bug
* 
* Hydra_Software_Devel/55   6/17/09 4:49p gautamk
* PR56010: [7405] Implmenting Code review comment
* 
* Hydra_Software_Devel/54   6/12/09 3:42p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/53   6/9/09 11:37a gautamk
* PR47949: [7405]Changing name from PCMOnAVI to PCMWAV
* 
* Hydra_Software_Devel/52   6/4/09 3:05p gautamk
* PR47949: [7405] Adding PI support for PCM On Avi
* 
* Hydra_Software_Devel/51   5/11/09 3:53p gautamk
* PR52383: [7405] Adding algorithm support for DTS decoder.
* 
* Hydra_Software_Devel/50   5/8/09 4:19p sgadara
* PR 52383: [3548,3556] Fixing bug in downloading logic whenever there is
* no table to be downloaded.
* 
* Hydra_Software_Devel/49   4/14/09 3:30p gautamk
* PR54056: [7405] Making the max chunk size to 65532
* 
* Hydra_Software_Devel/48   4/1/09 4:39p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/47   3/18/09 9:52p gautamk
* PR53365: [7405] Adding PI support for dolby volume
* 
* Hydra_Software_Devel/46   3/13/09 3:22p speter
* PR 53192: [3548,3556] Changing the order of generic passthru and srshd
* to match the CIT structure
* 
* Hydra_Software_Devel/45   2/13/09 1:33p speter
* PR 52090: [3548,3556] Merging the Phase 4.2 Take 2 release to main line
* 
* Hydra_Software_Devel/RAP_354856_PH_4_3/1   2/10/09 12:07p speter
* PR 46135: [3548,3556] Adding SRS Tru Volume Support - Chaning the name
* to TruVolume
* 
* Hydra_Software_Devel/44   2/2/09 4:28p gautamk
* PR51652: [7405]
* PR51618: [7405] Changing the global variables as per following rule.
* 1) make them static
* 2) if you cannot make them static because they must be shared between
* multiple .c files in your module, you must rename them to include the
* BXXX_ magnum prefix for your module.
* 
* Hydra_Software_Devel/43   1/16/09 2:55p gautamk
* PR48850: [7405] Fixing Coverity issue.
* 
* Hydra_Software_Devel/42   12/22/08 4:11p gautamk
* PR48850: [7405] Accessing invalid index of the array was causing
* Segmentaion fault . Fixing it.
* 
* Hydra_Software_Devel/41   12/22/08 11:32a speter
* PR 46135: [3548,3556] Adding SRS VIQ Support
* 
* Hydra_Software_Devel/40   10/10/08 10:03a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   10/1/08 3:20p speter
* PR 46051: [3548,3556] Changing DDEnc to AC3Enc
* 
* Hydra_Software_Devel/39   9/16/08 2:44p sushmit
* PR 43454:[7405]Adding AC3 Encode Support.
* 
* Hydra_Software_Devel/38   9/15/08 8:41p sushmit
* PR 46702:[7325] Removing warning in non-DBG build.
* 
* Hydra_Software_Devel/37   9/4/08 11:03p speter
* PR 46565: [3548,3556] Adding SRS-HD support
* 
* Hydra_Software_Devel/36   8/12/08 11:01p sushmit
* PR 43454:[7405]Adding DSOLA.
* 
* Hydra_Software_Devel/35   8/10/08 8:36p sushmit
* PR 43454:[7405]DTSENC Upd.
* 
* Hydra_Software_Devel/34   8/10/08 11:55a sushmit
* PR 43454:[7405] DTSENC Updates.
* 
* Hydra_Software_Devel/33   7/28/08 9:38a speter
* PR 45171: [3548,3556] Merging from Phase 2.0 release to main line of
* development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/32   7/23/08 9:35p gautamk
* PR43454: [7405] Adding PI support for WMA Passthru
* 
* Hydra_Software_Devel/31   7/23/08 3:57p gautamk
* PR43454: [7405] inserting PCMrouter satge interanally for AD Fade
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/1   7/24/08 4:58p speter
* PR 43454: [3548,3556] Adding PCM router post processing algorithm
* 
* Hydra_Software_Devel/30   6/16/08 5:14p gautamk
* PR43454: [7405] In case of BOpentimeDownload , don't download duplicate
* execs.
* 
* Hydra_Software_Devel/29   5/29/08 2:15a speter
* PR 38950: [3548,3556] Adding Audio Descriptor support
* 
* Hydra_Software_Devel/28   5/28/08 11:38p gautamk
* PR41726: [7405] Modifying files for AAC/AAC-HE LOAS/ADTS
* 
* Hydra_Software_Devel/27   5/16/08 1:39p gautamk
* PR42710: [7405] coverity issued fixed
* PR42711: [7405] coverity issued fixed
* 
* Hydra_Software_Devel/26   5/6/08 8:24p gautamk
* PR41726: [7405] Changes for AAC
* 
* Hydra_Software_Devel/25   5/2/08 2:44p gautamk
* PR41726: [7405] Adding PI support for Fw Verification in mainline
* 
* Hydra_Software_Devel/Rap_NewFwArch_FwVerfication/1   5/2/08 2:18p gautamk
* PR41726: [7405] Adding PI support for Fw Verification.
* 
* Hydra_Software_Devel/24   4/16/08 12:06p gautamk
* PR41726: [7405] Coverity fixes.
* 
* Hydra_Software_Devel/23   4/2/08 6:15p sushmit
* PR 34648: [7405] Fixing Processing Download
* 
* Hydra_Software_Devel/22   4/2/08 2:43p sushmit
* PR 34648:[7405] Moving PCM Capture to Interrupt Mode & Fixing
* AudProcessing Enable Array
* 
* Hydra_Software_Devel/21   4/2/08 2:05p sushmit
* PR 34648: [7405] DD Convert Code Download Fix
* 
* Hydra_Software_Devel/20   2/29/08 4:24p gautamk
* PR34648: [7405] Changing file names includes
* 
* Hydra_Software_Devel/19   2/19/08 4:06p gautamk
* PR34648: [7405] Updating function names.
* 
* Hydra_Software_Devel/18   2/19/08 3:31p gautamk
* PR34648: [7405] updating file names and fuction names
* 
* Hydra_Software_Devel/17   2/7/08 2:41p gautamk
* PR34648: [7405] implementing getDefaultConfigParams,
* getCurrentConfigparams.
* Removing compilation warning of
* brap_fwif.h
* brap_fwif_user_config.h
* brap_fwif_proc.c
* brap_fwif_dwnldutil.c
* 
* Hydra_Software_Devel/16   1/31/08 5:23p gautamk
* PR34648: [7405] Removing BRAP_DSPCHN_DecodeMode_eSimulMode Enum
* 
* Hydra_Software_Devel/15   1/30/08 4:36p gautamk
* PR34648: [7405] Updating design for downloading FW. Now download FW ids
* are coming from common arrary between PI/FW
* 
* Hydra_Software_Devel/14   1/24/08 9:04p sushmit
* PR 34648: [7405] Updaing Passthru Bringup PI Code
***************************************************************************/
#include "brap_fwdwnld_priv.h"
#ifndef OFFLINE_UTIL
#include "brap.h"
#include "brap_priv.h"
#include "brap_dsp.h"
#include "brap_dspchn_priv.h"
#include "brap_img.h"
#endif
#ifdef RAP_SCM_SUPPORT
#include "brap_scm_priv.h"
#endif


BDBG_MODULE(rap_dwnldutil);

extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eMax][BRAP_DSPCHN_DecodeMode_eMax];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eMax][BRAP_DSPCHN_EncodeMode_eSimulMode];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eMax][BRAP_DSPCHN_ProcessingMode_eMax];
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sGfxDecodeExecInfo[BRAP_DSPCHN_GfxType_eMax];
#ifdef RAP_SCM_SUPPORT
extern const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sScmDecodeExecInfo[BRAP_DSPCHN_ScmType_eMax];
#endif


/**************** Start Global Variables **************/
 static const bool bAudioCodecSupported[/*BRAP_DSPCHN_AudioType_eMax*/]=
{
#ifdef RAP_MPEG_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AACSBR_SUPPORT
	true,   /*AAC ADTS*/
	true,   /*AAC LOAS*/
	true,   /*AACHE ADTS*/
	true,	   /*AACHE LOAS*/
#else
	false, /*AAC ADTS*/
	false,/*AAC LOAS*/
	false, /*AACHE ADTS*/
	false,/*AACHE LOAS*/	
#endif
#ifdef RAP_AC3_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DDP_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DTS_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_LPCMBD_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_LPCMHDDVD_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DTSHD_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_LPCMDVD_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_WMASTD_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AC3LOSSLESS_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_MLP_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_PCM_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DTSLBR_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DDP7_1_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_MPEGMC_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_WMAPRO_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DTSHDSUB_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_LPCMDVDA_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DTSBROADCAST_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_PCMWAV_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AMR_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DRA_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_REALAUDIOLBR_SUPPORT
	true,
#else
	false,
#endif
};

#ifdef RAP_VIDEOONDSP_SUPPORT
static bool bVideoCodecSupported[/*BRAP_DSPCHN_AudioType_eMax*/]=
{
#ifdef RAP_REALVIDEO_SUPPORT
	true
#else
	false
#endif
};
#endif

 static const bool bEncodeSupported[/*BRAP_CIT_P_EncAudioType_eMax*/]=
{
#ifdef RAP_MP3ENC_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_MP2_ENCODE_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DTS_ENCODE_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AACLC_ENCODE_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AACHE_ENCODE_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AC3ENC_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DTSENC_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_SBCENC_SUPPORT
	true,
#else
	false,
#endif
};

static const bool bAudioProcSupported[/*BRAP_CIT_P_ProcessingType_eMax*/]=
{
#ifdef RAP_DDBM_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DTSNEO_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AVL_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DDP_TO_AC3_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_PL2_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_SRSXT_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_XEN_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_BBE_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_SRC_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_CUSTOMSURROUND_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_CUSTOMBASS_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_CUSTOMVOICE_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_PEQ_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AACDOWNMIX_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_AUDIODESC_SUPPORT
	true,
	true,
#else
	false,
	false,	
#endif
#ifdef RAP_PCMROUTER_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_WMAPROPASSTHRU_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DSOLA_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_SRSHD_SUPPORT
	true,
#else
	false,
#endif
    true,   /* Generic Passthru */
#ifdef RAP_SRSTRUVOL_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DOLBYVOL_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_ADV_SUPPORT 
	true,
#else
	false,
#endif
#ifdef RAP_BRCM3DSURROUND_SUPPORT 
	true,
#else
	false,
#endif
#ifdef RAP_FWMIXER_SUPPORT 
        true,
#else
        false,
#endif
#ifdef RAP_MONODOWNMIX_SUPPORT 
	true,
#else
	false,
#endif
#ifdef RAP_ABX_SUPPORT 
	true,
#else
	false,
#endif
#ifdef RAP_DDRE_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_DV258_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_SRSCSTD_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_SRSEQHL_SUPPORT
	true,
#else
	false,
#endif
#ifdef RAP_BTSC_SUPPORT
	true
#else
	false
#endif
};

static bool bGfxSupported[]=
{
#ifdef RAP_GFX_SUPPORT
	true
#else
	false
#endif
};

static bool bScmSupported[]=
{

#ifdef RAP_SCM_SUPPORT
	false,
	false
#else
	false,
	false
#endif
};

/**************** END Global Variables **************/



/**************** Start Function Prototype **************/



/*******************************************************************************
Summary: 
	This function downloads all the resident codes. This function is called
	at device open time.

Pseudo Code:	
DownloadResidentCode:
{
	This Function will download following Execs
		- OSKernal
		- hostCommunicationRoutine
		- EDFRoutine
		- topLevelFunctionofAllTask
		- HW Interface Functions
	All the Exec will be downloaded using Image Interface.

	BRAP_DWN_CopyFWImageToMem();
}
		
Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadResidentCode(
						BRAP_FWDWNLD_P_Param *pParam
						);


/*******************************************************************************
Summary: 
	This function downloads all type of TSM executable required, And will 
	update the MIT entries accordingly.
	This function is called at device open time.

Pseudo code:
DownloadAllTSMExec:
{
	In the new FW architecture, TSM is not the part of schedular code. 
	But it is a seperate entity. It can be different according to usage
	mode. so download all type of TSM execs.
	BRAP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadAllTSMExec(
						BRAP_FWDWNLD_P_Param *pParam
						);

BERR_Code BRAP_FWDWNLD_P_DownloadSystemTaskCode(BRAP_FWDWNLD_P_Param *pParam);

/*******************************************************************************
Summary: 
	This Function will download all the supported audio processing algorithms.
	The entries in the MIT will be updated with the Mem. pointers and its size.
	This function is called at device open time.

Pseudo code:
DownloadProcessingExecs:
{
	-For All Supported audio processing algorithm
		-Download Code+Table
		BRAP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadAudProcessingExecs(
						BRAP_FWDWNLD_P_Param *pParam
						);


/*******************************************************************************
Summary:
	This Function will be called only if the OpenTimeDownload is set. It will 
	download the executables for all supported 	audio codecs.
	The entries of MIT wiil be updated as well.

Pseudo code:
DownloadAllSupportedExecs:
{
	
	For each Supported Audio Type
		- Download Framesync Exec
			- There are can be option to choose framesync from various
			  framesync execs available for an algorithm. Sodownload all
			  the framesync options available for algorithms.
			  
		- Download Decode Code+Table Exec
		- Download Passthru Code+Table Exec
		- Download Other required Execs.

	BRAP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadSupportedCodecExecs(
						BRAP_FWDWNLD_P_Param *pParam
						);

/*******************************************************************************
Summary:
	This Function will be called only if the OpenTimeDownload is set. It will 
	download the Passthru executables.
	The entries of MIT wiil be updated as well.

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadPassthruExecs(
						BRAP_FWDWNLD_P_Param *pParam
						);

/*******************************************************************************
Summary:
	This Function will be called only if the OpenTimeDownload is set. It will 
	download the Encode executables.
	The entries of MIT wiil be updated as well.

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadEncodeExecs(
                                            BRAP_FWDWNLD_P_Param *pParam
                                            );

/**************** End Function Prototype **************/




/******************************************************************************
Summary:
	This Function Writes the Data into a file
*******************************************************************************/
void BRAP_P_WritetoFile(uint32_t	ui32Data)
{
	/* Append the ui32Data to the Output File */
        BSTD_UNUSED(ui32Data);
}


/******************************************************************************
Summary:
	This Function Copies the FW exec from Image interface to buffer in the DRAM.
*******************************************************************************/

BERR_Code BRAP_FWDWNLD_P_CopyFWImageToMem(
		const BIMG_Interface *iface,
		void *pImgContext,
		uint32_t ui32MemAddr,	/* 0 , If its a offline utility */
		BRAP_Img_Id firmware_id,
		BMEM_Handle hHeap
		)
{
	void *image = NULL, *cacheAddr = NULL;	
	const void *data = NULL;	
	void *context = pImgContext;

	uint32_t ui32Size = 0, ui32numOfChunks = 0,ui32ChunkLen = 0;
	uint32_t ui32Count = 0;
       uint32_t uiSizeCopied=0;

#ifndef OFFLINE_UTIL
	BERR_Code rc = BERR_SUCCESS;	
	BDBG_ENTER(BRAP_FWDWNLD_P_CopyFWImageToMem);
	BDBG_ASSERT(iface);
	BDBG_ASSERT(pImgContext);

	rc = iface->open(context, &image, firmware_id);
	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("Error in Opening the Image Interface"));
		return BERR_TRACE(rc);
	}

	rc = iface->next(image, 0, &data, 8);
	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("\nError in fetching next chunk in Image Interface\n"));
		iface->close(image);        
		return BERR_TRACE(rc);
	}

#else

	iface->open(context, &image, (uint32_t)firmware_id);
	iface->next(image, 0, &data, 8);	

#endif

	ui32Size =((uint32_t *) data)[0];
	ui32numOfChunks = ((uint32_t *) data)[1];

        BDBG_MSG(("Total Size = %d",ui32Size));
	for (ui32Count = 1;ui32Count <= ui32numOfChunks; ui32Count++)
	{
		/* The implementation of image interface has to be such that there is 
		one header array and then there are firmware binary	arrays each having
		bytes of size BRAP_IMG_CHUNK_SIZE. But the last array most probably will
		not have a size equal to exactly BRAP_IMG_CHUNK_SIZE. So we are testing 
		for the last chunk here and getting the size based on the total firmware
		binary size and number of chunks*/

		ui32ChunkLen = (ui32Count == ui32numOfChunks) ?  (ui32Size - ((ui32Count - 1)*BRAP_IMG_CHUNK_SIZE)): BRAP_IMG_CHUNK_SIZE;

              BDBG_ASSERT(ui32ChunkLen <= BRAP_IMG_CHUNK_SIZE);  


              BDBG_MSG(("ui32Count = %d, ui32numOfChunks = %d , ui32ChunkLen =%d , ui32MemAddr = %#x",ui32Count,ui32numOfChunks,ui32ChunkLen,ui32MemAddr));

#ifndef OFFLINE_UTIL
		rc = iface->next(image, ui32Count, &data, ui32ChunkLen);
		if (rc != BERR_SUCCESS) 
		{		
			BDBG_ERR(("\nError in fetching next chunk in Image Interface\n"));;	
			iface->close(image);
			return BERR_TRACE(rc);
		}
#else
		iface->next(image, ui32Count, &data, ui32ChunkLen);
#endif


            rc = BMEM_Heap_ConvertAddressToCached(hHeap,(void *) ui32MemAddr,(void **) &cacheAddr);
            if (rc!=BERR_SUCCESS)
             {
                BDBG_ERR(("BMEM_Heap_ConvertAddressToCached returned Error %d",rc));
                iface->close(image);                
                return BERR_TRACE(rc);
            }

            BKNI_Memcpy((void *)(volatile void *)cacheAddr,data,ui32ChunkLen);	
            BMEM_Heap_FlushCache(hHeap,(void *)cacheAddr,ui32ChunkLen);   
            ui32MemAddr +=ui32ChunkLen;
            uiSizeCopied +=  ui32ChunkLen;
	}	
        if(uiSizeCopied != ui32Size)
        {
            BDBG_ERR(("FW Image (Id =%#x)not downloaded correctly",firmware_id));
        }

	iface->close(image);

#ifndef OFFLINE_UTIL		
	BDBG_LEAVE(BRAP_FWDWNLD_P_CopyFWImageToMem);
	return BERR_SUCCESS;
#else
	return;
#endif

}

/******************************************************************************
Summary:
	This function downloads following at device open time.
			-- Resident Code
			-- TSM Exec
			-- Audio Processing Algo
			-- Decode algorithm Exec

Pseudo Code:			
DownloadFwExec:
{
	DownloadResidentCode();

	DownloadAllTSMExec();

	If(DownloadAudProcAtOpentime) 
	{
	 	DownloadPPAlgoAtOpentime will be decided based on some logic if
	 	it is beneficial to download the PP algos at start time to save 
	 	the memory.
	 	
		DownloadProcessingExecs();
	}
	If(OpenTimeDownload)
	{

		DownloadAllSupportedExecs();
	}
}

Input Parameter:
	void *phRap:	RAP Handle (Device Handle)
			In case offline utility,Offline function will pass the a pointer to 
			structure containing required fields to copy Fw into the file.
Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadFwExec(
#ifdef OFFLINE_UTIL
	BRAP_OFFLINEUTIL_DwnldParams	*pOfflineDwnldParams
#else
	BRAP_Handle						hRap
#endif
)
{
	bool	bOpenTimeFwDownload;
	bool	bOpenTimeAudProcDownload = true;
       unsigned int i;
	BRAP_FWDWNLD_P_Param	sParam;
	
#ifdef OFFLINE_UTIL
	bOpenTimeFwDownload = 1;
	bOpenTimeAudProcDownload = 1;
	sParam.pOfflineDwnldParams = pOfflineDwnldParams;
#else
    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadFwExec);
	BDBG_ASSERT(hRap);

	sParam.hRap = hRap;
	bOpenTimeFwDownload = hRap->sSettings.bOpenTimeFwDownload;
	BRAP_FWDWNLD_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);
#endif

       for(i =0;i<BRAP_AF_P_AlgoId_eMax;i++)
        {
            hRap->bIsExecDownloaded[i] =false;
        }
	BRAP_FWDWNLD_P_DownloadResidentCode(&sParam);

	BRAP_FWDWNLD_P_DownloadAllTSMExec(&sParam);

	BRAP_FWDWNLD_P_DownloadSystemTaskCode(&sParam);
    
#ifdef RAP_SCM_SUPPORT
	if ( hRap->sSettings.sScmSettings.bSCMEnabled )
	{
		/* SCM Execs Download */
		BRAP_FWDWNLD_P_DownloadSCMExecs(&sParam,false,BRAP_SCM_P_GetSCMAlgorithmID(hRap->sSettings.sScmSettings.ui32CaSystemId));
	}
#endif		
	
    
	if(bOpenTimeAudProcDownload) 
	{
	 	/* DownloadPPAlgoAtOpentime will be decided based on some logic if
	 	it is beneficial to download the PP algos at start time to save 
	 	the memory. */
	 	
		BRAP_FWDWNLD_P_DownloadAudProcessingExecs(&sParam);
	}
	if(bOpenTimeFwDownload)
	{ 
		BRAP_FWDWNLD_P_DownloadEncodeExecs(&sParam);	
		BRAP_FWDWNLD_P_DownloadSupportedCodecExecs(&sParam);
		BRAP_FWDWNLD_P_DownloadPassthruExecs(&sParam);
#ifdef RAP_VIDEOONDSP_SUPPORT
		BRAP_FWDWNLD_P_DownloadVideoDecodeExecs(&sParam,true,BRAP_DSPCHN_VideoType_eInvalid);        
#endif
#ifdef 	RAP_GFX_SUPPORT
        BRAP_FWDWNLD_P_DownloadGfxExecs(&sParam,false,0);
#endif
	}

#ifdef OFFLINE_UTIL
	return;
#else
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadFwExec);
	return BERR_SUCCESS;
#endif	
}

/*******************************************************************************
Summary: 
	This function downloads all the resident codes. This function is called
	at device open time.

Pseudo Code:	
DownloadResidentCode:
{
	This Function will download following Execs
		- OSKernal
		- hostCommunicationRoutine
		- EDFRoutine
		- topLevelFunctionofAllTask
		- HW Interface Functions
	All the Exec will be downloaded using Image Interface.

	BRAP_DWN_CopyFWImageToMem();
}
		
Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadResidentCode(BRAP_FWDWNLD_P_Param *pParam)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32MemAddr = 0,ui32Size = 0;
	BRAP_Img_Id eFirmwareId = BRAP_Img_Id_eInvalid;


	
#ifdef OFFLINE_UTIL

/*Offline utility application will pass some structure handle having members 
iface and pImgContext. This will be assigned here in the local variables*/
	BRAP_OFFLINEUTIL_DwnldParams *hOfflineDwnldParams = NULL;
	hOfflineDwnldParams = pParam->pOfflineDwnldParams;
	iface = hOfflineDwnldParams->pImgInterface;
	pImgContext = hOfflineDwnldParams->pImgContext;
	
#else

	BRAP_Handle   hRap = NULL;
	BERR_Code rc = BERR_SUCCESS;
	uint32_t ui32physAddress = 0;

    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadResidentCode);	
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;
	
#endif


	/* Download Iboot Code */
	eFirmwareId = BRAP_Img_Id_eSystemIboot; 
#ifdef OFFLINE_UTIL
	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,NULL);
#else

	ui32MemAddr = hRap->sDwnldMemInfo.ui32SystemIbootCodePtr;
	BRAP_DSP_P_GetFWSize (hRap, eFirmwareId, &ui32Size);
	
	if(ui32Size > hRap->sDwnldMemInfo.ui32SystemIbootCodeSize)
	{
		BDBG_ERR(("Size available (%d) to download Iboot code"
				"(%#x) is less than required (%d)",
				hRap->sDwnldMemInfo.ui32SystemIbootCodeSize,eFirmwareId,
				ui32Size
				));
		return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}
	
	BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);	
	BDBG_MSG(("Downloading Iboot code(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
					eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
#endif

	/* Download System Codes , Includes uCOS OS, CPT and ATM resident tasks*/
	eFirmwareId = BRAP_Img_Id_eSystemCode;
#ifdef OFFLINE_UTIL
	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,NULL);
#else

	ui32MemAddr = hRap->sDwnldMemInfo.ui32SystemCodePtr;
	BRAP_DSP_P_GetFWSize (hRap, eFirmwareId, &ui32Size);
	
	if(ui32Size > hRap->sDwnldMemInfo.ui32SystemCodeSize)
	{
		BDBG_ERR(("Size available (%d) to download System code"
				"(%#x) is less than required (%d)",
				hRap->sDwnldMemInfo.ui32SystemCodeSize,eFirmwareId,
				ui32Size
				));
		return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}
	
	BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);	
	BDBG_MSG(("Downloading System code(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
					eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
#endif

	/* Download System Data, Includes uCOS OS, CPT and ATM resident tasks */
	eFirmwareId = BRAP_Img_Id_eSystemData; 
#ifdef OFFLINE_UTIL
	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,NULL);
#else

	ui32MemAddr = hRap->sDwnldMemInfo.ui32SystemDataPtr;
	BRAP_DSP_P_GetFWSize (hRap, eFirmwareId, &ui32Size);
	
	if(ui32Size > hRap->sDwnldMemInfo.ui32SystemDataSize)
	{
		BDBG_ERR(("Size available (%d) to download System Data"
				"(%#x) is less than required (%d)",
				hRap->sDwnldMemInfo.ui32SystemDataSize,eFirmwareId,
				ui32Size
				));
		return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}
	
	BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);	
	BDBG_MSG(("Downloading System Data(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
					eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
#endif

	/* Download Iboot Code */
	eFirmwareId = BRAP_Img_Id_eSystemRdb; 


	ui32MemAddr = hRap->sDwnldMemInfo.ui32SystemRdbVariablesPtr;
	BRAP_DSP_P_GetFWSize (hRap, eFirmwareId, &ui32Size);
	
	if(ui32Size > hRap->sDwnldMemInfo.ui32SystemRdbVariablesSize)
	{
		BDBG_ERR(("Size available (%d) to download System RDB"
				"(%#x) is less than required (%d)",
				hRap->sDwnldMemInfo.ui32SystemRdbVariablesSize,eFirmwareId,
				ui32Size
				));
		return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}
	
	BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);	
	BDBG_MSG(("Downloading System RDB code(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
					eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);

#ifdef RAP_SCM_SUPPORT
	if ( hRap->sSettings.sScmSettings.bSCMEnabled )
	{
		/*Download Signatures */
		eFirmwareId = BRAP_Img_Id_eSystemTableOfHashes; 


		ui32MemAddr = hRap->sDwnldMemInfo.ui32SystemTableOfHashesPtr;
		BRAP_DSP_P_GetFWSize (hRap, eFirmwareId, &ui32Size);
		
		if(ui32Size > hRap->sDwnldMemInfo.ui32SystemTableOfHashesSize)
		{
			BDBG_ERR(("Size available (%d) to download System TableOfHashes"
					"(%#x) is less than required (%d)",
					hRap->sDwnldMemInfo.ui32SystemTableOfHashesSize,eFirmwareId,
					ui32Size
					));
			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
		}
		
		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);	
		BDBG_MSG(("Downloading System TableOfHashes (ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
						eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
	}
#endif
 
#ifdef OFFLINE_UTIL
	return;
#else
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadResidentCode);	
	return rc;
#endif
}

/*******************************************************************************
Summary: 
	This function downloads all type of TSM executable required, And will 
	update the MIT entries accordingly.
	This function is called at device open time.

Pseudo code:
DownloadAllTSMExec:
{
	In the new FW architecture, TSM is not the part of schedular code. 
	But it is a seperate entity. It can be different according to usage
	mode. so download all type of TSM execs.
	BRAP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadAllTSMExec(BRAP_FWDWNLD_P_Param *pParam)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32MemAddr = 0,ui32Size = 0;
	BRAP_Img_Id eFirmwareId = BRAP_Img_Id_eInvalid;

#ifdef OFFLINE_UTIL

/*Offline utility application will pass some structure handle having members 
iface and pImgContext. This will be assigned here in the local variables*/
	BRAP_OFFLINEUTIL_DwnldParams *hOfflineDwnldParams = NULL;
	hOfflineDwnldParams = pParam->pOfflineDwnldParams;
	iface = hOfflineDwnldParams->pImgInterface;
	pImgContext = hOfflineDwnldParams->pImgContext;
	
#else

	BRAP_Handle   hRap = NULL;
	BERR_Code rc = BERR_SUCCESS;
	uint32_t	ui32EndAddr = 0,ui32physAddress = 0;
	
    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadAllTSMExec);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;

	ui32MemAddr = hRap->sDwnldMemInfo.ui32TsmCodePtr;
	ui32EndAddr = ui32MemAddr + hRap->sDwnldMemInfo.ui32TsmCodeSize;	
	
#endif

/* Download each TSM Exec */
		eFirmwareId =hRap->sImgIdMappingArrays.ui32CodeImgId[BRAP_AF_P_AlgoId_eDecodeTsm];

		if(BRAP_Img_Id_eInvalid != eFirmwareId)
		{		
#ifdef OFFLINE_UTIL
                	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,NULL);
#else
                	ui32Size = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eDecodeTsm];
                	if((ui32MemAddr + ui32Size) > ui32EndAddr)
                	{
                		BDBG_ERR(("Size available (%d) to download TSM Algorithm"
                				"(%#x) is less than required (%d)",
                				(ui32EndAddr-(ui32MemAddr+ ui32Size)),eFirmwareId,
                				ui32Size
                				));
                		return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                	}
                	
                	BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);
                	
                	BDBG_MSG(("Downloading TSM Exec(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
                					eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
                	
                	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
                	BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_eDecodeTsm,ui32MemAddr,ui32Size,0,0);		
#if 0                    
                	ui32MemAddr += ui32Size;
#endif
#endif
		  }
        
		ui32Size = 0;
		eFirmwareId =hRap->sImgIdMappingArrays.ui32CodeImgId[BRAP_AF_P_AlgoId_eEncodeTsm];

		if(BRAP_Img_Id_eInvalid != eFirmwareId)
		{
		
#ifdef OFFLINE_UTIL
        		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,NULL);
#else
        		ui32Size = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eEncodeTsm];
#if 0
        		if((ui32MemAddr + ui32Size) > ui32EndAddr)
        		{
        			BDBG_ERR(("Size available (%d) to download TSM Algorithm"
        					"(%#x) is less than required (%d)",
        					(ui32EndAddr-(ui32MemAddr+ ui32Size)),eFirmwareId,
        					ui32Size
        					));
        			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        		}
        		
        		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);
        		
        		BDBG_MSG(("Downloading TSM Exec(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
        						eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
        		
        		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
#endif                
        		BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_eEncodeTsm,ui32MemAddr,ui32Size,0,0);			
#if 0
        		ui32MemAddr += ui32Size;
#endif
#endif
	        }
        
		ui32Size = 0;
		eFirmwareId =hRap->sImgIdMappingArrays.ui32CodeImgId[BRAP_AF_P_AlgoId_ePassthroughTsm];

		if(BRAP_Img_Id_eInvalid != eFirmwareId)
		{	
#ifdef OFFLINE_UTIL
        		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,NULL);
#else
        		ui32Size = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_ePassthroughTsm];
#if 0
        		if((ui32MemAddr + ui32Size) > ui32EndAddr)
        		{
        			BDBG_ERR(("Size available (%d) to download TSM Algorithm"
        					"(%#x) is less than required (%d)",
        					(ui32EndAddr-(ui32MemAddr+ ui32Size)),eFirmwareId,
        					ui32Size
        					));
        			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        		}
        		
        		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);
        		
        		BDBG_MSG(("Downloading TSM Exec(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
        						eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
        		
        		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
#endif
        		BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_ePassthroughTsm,ui32MemAddr,ui32Size,0,0);			
#if 0
        		ui32MemAddr += ui32Size;
#endif
#endif
		}

#ifdef OFFLINE_UTIL
	return;
#else
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadAllTSMExec);		
	return rc;
#endif
}

BERR_Code BRAP_FWDWNLD_P_DownloadSystemTaskCode(BRAP_FWDWNLD_P_Param *pParam)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32MemAddr = 0,ui32Size = 0;
	BRAP_Img_Id eFirmwareId = BRAP_Img_Id_eInvalid;

	BRAP_Handle   hRap = NULL;
	BERR_Code rc = BERR_SUCCESS;
	uint32_t	ui32EndAddr = 0,ui32physAddress = 0;
	
    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadAllTSMExec);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;

	ui32MemAddr = hRap->sDwnldMemInfo.ui32SystemTaskCodePtr;
	ui32EndAddr = ui32MemAddr + hRap->sDwnldMemInfo.ui32SystemTaskCodeSize;	
	
	eFirmwareId =hRap->sImgIdMappingArrays.ui32CodeImgId[BRAP_AF_P_AlgoId_eSystemDecodeTask];

	if(BRAP_Img_Id_eInvalid != eFirmwareId)
	{		
            	ui32Size = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSystemDecodeTask];
            	if((ui32MemAddr + ui32Size) > ui32EndAddr)
            	{
            		BDBG_ERR(("Size available (%d) to download System decode task"
            				"(%#x) is less than required (%d)",
            				(ui32EndAddr-(ui32MemAddr+ ui32Size)),eFirmwareId,
            				ui32Size
            				));
            		return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            	}
            	
            	BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);
            	
            	BDBG_MSG(("Downloading System decode task Exec(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
            					eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
            	
            	BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
            	BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_eSystemDecodeTask,ui32MemAddr,ui32Size,0,0);		
              
            	ui32MemAddr += ui32Size;
	  }
        
	ui32Size = 0;
	eFirmwareId =hRap->sImgIdMappingArrays.ui32CodeImgId[BRAP_AF_P_AlgoId_eSystemGraphicTask];

	if(BRAP_Img_Id_eInvalid != eFirmwareId)
	{
		
        	ui32Size = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSystemGraphicTask];
    		if((ui32MemAddr + ui32Size) > ui32EndAddr)
    		{
    			BDBG_ERR(("Size available (%d) to download System Gfx task"
    					"(%#x) is less than required (%d)",
    					(ui32EndAddr-(ui32MemAddr+ ui32Size)),eFirmwareId,
    					ui32Size
    					));
    			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    		}
    		
    		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);
    		
    		BDBG_MSG(("Downloading System Gfx task Exec(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
    						eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
    		
    		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);

    		BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_eSystemGraphicTask,ui32MemAddr,ui32Size,0,0);			
    		ui32MemAddr += ui32Size;
        }
        
	ui32Size = 0;
	eFirmwareId =hRap->sImgIdMappingArrays.ui32CodeImgId[BRAP_AF_P_AlgoId_eSystemVideoTask];

	if(BRAP_Img_Id_eInvalid != eFirmwareId)
	{	
    		ui32Size = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSystemVideoTask];
    		if((ui32MemAddr + ui32Size) > ui32EndAddr)
    		{
    			BDBG_ERR(("Size available (%d) to download System video task"
    					"(%#x) is less than required (%d)",
    					(ui32EndAddr-(ui32MemAddr+ ui32Size)),eFirmwareId,
    					ui32Size
    					));
    			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    		}
    		
    		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);
    		
    		BDBG_MSG(("Downloading System video task Exec(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
    						eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
    		
    		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
    		BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_eSystemVideoTask,ui32MemAddr,ui32Size,0,0);			

    		ui32MemAddr += ui32Size;
	}

#ifdef RAP_SCM_SUPPORT
	ui32Size = 0;
	eFirmwareId =hRap->sImgIdMappingArrays.ui32CodeImgId[BRAP_AF_P_AlgoId_eSCMTask];

	if(BRAP_Img_Id_eInvalid != eFirmwareId)
	{	
    		ui32Size = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_eSCMTask];
    		if((ui32MemAddr + ui32Size) > ui32EndAddr)
    		{
    			BDBG_ERR(("Size available (%d) to download SCM task"
    					"(%#x) is less than required (%d)",
    					(ui32EndAddr-(ui32MemAddr+ ui32Size)),eFirmwareId,
    					ui32Size
    					));
    			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    		}
    		
    		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32MemAddr), &ui32physAddress);
    		
    		BDBG_MSG(("Downloading SCM task Exec(ID = %#x) at address 0x%x (Physical = %#x), size = 0x%x",
    						eFirmwareId,ui32MemAddr,ui32physAddress,ui32Size));
    		
    		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,eFirmwareId,hRap->hHeap);
    		BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_eSCMTask,ui32MemAddr,ui32Size,0,0);			

    		ui32MemAddr += ui32Size;
	}
#endif	

    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadAllTSMExec);		
	return rc;
}



/*******************************************************************************
Summary: 
	This Function will download all the supported encode algorithms.
	The entries in the MIT will be updated with the Mem. pointers and its size.
	This function is called at device open time.

Pseudo code:
DownloadEncodeExecs:
{
	-For All Supported Encode algorithm
		-Download Code+Table
		BRAP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadEncodeExecs(BRAP_FWDWNLD_P_Param *pParam)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32CodePtr = 0,ui32CodeSize = 0,ui32TblPtr = 0,ui32TblSize = 0;
	BRAP_Img_Id eCodeFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_Img_Id eTblFirmwareId = BRAP_Img_Id_eInvalid ;
	BRAP_AF_P_AlgoId eCodeExecId,eTempExecId;
	unsigned int count = 0,i = 0;
	bool bDownloaded= false;	

	
#ifdef OFFLINE_UTIL
	/*Offline utility application will pass some structure handle having members 
	iface and pImgContext. This will be assigned here in the local variables*/
	BRAP_OFFLINEUTIL_DwnldParams *hOfflineDwnldParams = NULL;
	hOfflineDwnldParams = pParam->pOfflineDwnldParams;
	iface = hOfflineDwnldParams->pImgInterface;
	pImgContext = hOfflineDwnldParams->pImgContext;
#else
	BRAP_Handle		hRap = NULL;
	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadEncodeExecs);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;
	ui32CodePtr= hRap->sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32BaseAddr;	
	ui32EndAddr = ui32CodePtr+ hRap->sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;	    
#endif


	for(count = 0; count < BRAP_CIT_P_EncAudioType_eMax; count++)
	{
		if(false == BRAP_FWDWNLD_P_IsEncodeSupported(count))
		{
			continue;
		}

		eCodeExecId = BRAP_sEncAlgoExecInfo[count][0].eAlgoIds[0];
            if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
            {
                continue;
            }                  
		eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

		if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
		{
			BDBG_MSG(("Invalid Image ID for Framesync of Encode Algorithm %#x",count));
			continue;
		}

            if(true == hRap->bIsExecDownloaded[eCodeExecId])
            {
                continue;
            }
            else
            {
                hRap->bIsExecDownloaded[eCodeExecId] = true;
            }

#ifdef OFFLINE_UTIL
		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,NULL);
#else
		ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

		if((ui32CodePtr + ui32CodeSize) > ui32EndAddr)
		{
			BDBG_ERR(("Size available (%d) to download Framesync (IMG ID = %#x) of Encode Algorithm (%#x)"
					"is less than required (%d)",
					(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,count,
					ui32CodeSize
					));
			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
		}

		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

		BDBG_MSG(("Downloading Framesync(IMG ID = %#x) of Encode Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
						eCodeFirmwareId,count,ui32CodePtr,ui32physAddress,ui32CodeSize));
		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
		BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);		
		ui32CodePtr += ui32CodeSize;
#endif
	}
		/*Download all encode Code+Table for supported algorithm*/	
#ifndef OFFLINE_UTIL
		ui32CodeSize = 0;
		ui32CodePtr= hRap->sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32BaseAddr;	
		ui32EndAddr = ui32CodePtr + hRap->sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;	
#endif	
	for(count = 0; count < BRAP_CIT_P_EncAudioType_eMax; count++)
	{
		if(false == BRAP_FWDWNLD_P_IsEncodeSupported(count))
		{
			continue;
		}
		
		for(i = 2;i<BRAP_sEncAlgoExecInfo[count][0].NumNodes;i++)
		{

		/*Download encode Code */
			eCodeExecId = BRAP_sEncAlgoExecInfo[count][0].eAlgoIds[i];
                    if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }         
			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				BDBG_MSG(("Invalid Image ID for  code of Encode algorithm %#x Stage %d",count,i));
				break;
			}
                    if(true == hRap->bIsExecDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        hRap->bIsExecDownloaded[eCodeExecId] = true;
                    }            

#ifdef OFFLINE_UTIL
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,NULL);
#else
			ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

			if((ui32CodePtr+ ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download  code (IMG ID = %#x) of Encode Algorithm (%#x)"
						" is less than required (%d)",
						(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,count,
						ui32CodeSize
						));
				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

			BDBG_MSG(("Downloading code (IMG ID = %#x) of Encode Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,count,ui32CodePtr,ui32physAddress,ui32CodeSize));
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
#endif

			ui32TblPtr = ui32CodePtr + ui32CodeSize;

			/*Download encode table */
			eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
			{
				BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
				ui32CodePtr += ui32CodeSize;	                
				BDBG_MSG(("Invalid Image ID for  table of Encode algorithm %#x Stage %d",count,i));
				continue;
			}
			bDownloaded = false;
			BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eEncode,eCodeExecId,&bDownloaded,&eTempExecId);
			
#ifdef OFFLINE_UTIL
			if(!bDownloaded)
			{
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,NULL);
			}
#else
			if(!bDownloaded)
			{
				ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

				if((ui32TblPtr+ ui32TblSize) > ui32EndAddr)
				{
					BDBG_ERR(("Size available (%d) to download  table (IMG ID = %#x) of Encode Algorithm (%#x)"
							" is less than required (%d)",
							(ui32EndAddr-(ui32TblPtr+ ui32TblSize)),eTblFirmwareId,count,
							ui32TblSize
							));
					return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
				}
				
				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Downloading table (IMG ID = %#x) of Encode Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
			}
			else
			{
				ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
				ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Table (IMG ID = %#x) of Encode Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
			}
			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	

			/* updating ui32CodePtr variable, used to track the used size */ 		
			if(false == bDownloaded)
			{
				ui32CodePtr += ui32CodeSize + ui32TblSize;
			}
			else
			{
				ui32CodePtr += ui32CodeSize;				
			}
#endif
		}
	}


#ifdef OFFLINE_UTIL
	return;
#else
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadEncodeExecs);	
	return rc;
#endif

}

/*******************************************************************************
Summary: 
	This Function will download all the supported audio processing algorithms.
	The entries in the MIT will be updated with the Mem. pointers and its size.
	This function is called at device open time.

Pseudo code:
DownloadProcessingExecs:
{
	-For All Supported audio processing algorithm
		-Download Code+Table
		BRAP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadAudProcessingExecs(BRAP_FWDWNLD_P_Param *pParam)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32CodePtr = 0,ui32CodeSize = 0,ui32TblPtr = 0,ui32TblSize = 0;
	BRAP_Img_Id eCodeFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_Img_Id eTblFirmwareId = BRAP_Img_Id_eInvalid ;
	BRAP_AF_P_AlgoId eCodeExecId,eTempExecId;
	unsigned int count = 0,i = 0, uiStartNodeIndex=0;
	bool bDownloaded= false;	

	
#ifdef OFFLINE_UTIL
	/*Offline utility application will pass some structure handle having members 
	iface and pImgContext. This will be assigned here in the local variables*/
	BRAP_OFFLINEUTIL_DwnldParams *hOfflineDwnldParams = NULL;
	hOfflineDwnldParams = pParam->pOfflineDwnldParams;
	iface = hOfflineDwnldParams->pImgInterface;
	pImgContext = hOfflineDwnldParams->pImgContext;
#else
	BRAP_Handle		hRap = NULL;
	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadAudProcessingExecs);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;
	ui32CodePtr= hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr;	
	ui32EndAddr = ui32CodePtr+ hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;	
#endif

	
		/*Download all Processing Code+Table for supported algorithm*/	
#ifndef OFFLINE_UTIL
		ui32CodeSize = 0;
		ui32CodePtr= hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr;	
		ui32EndAddr = ui32CodePtr + hRap->sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;	
#endif	
	for(count = 0; count < BRAP_CIT_P_ProcessingType_eMax; count++)
	{
		if(false == BRAP_FWDWNLD_P_IsAudProcSupported(count))
		{
			continue;
		}
        BDBG_MSG(("eCodeExecId > %d", BRAP_sPpAlgoExecInfo[count][0].eAlgoIds[2]));
		
        if(count == BRAP_CIT_P_ProcessingType_eFWMixerPostProc)
            uiStartNodeIndex = 0;
        else
            uiStartNodeIndex = 2;

		for(i = uiStartNodeIndex;i<BRAP_sPpAlgoExecInfo[count][0].NumNodes;i++)
		{

		/*Download Processing Code */
			eCodeExecId = BRAP_sPpAlgoExecInfo[count][0].eAlgoIds[i];
                    if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }         
			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				BDBG_MSG(("Invalid Image ID for Audio Processing %#x Stage %d",count,i));
				break;
			}
                    if(true == hRap->bIsExecDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        hRap->bIsExecDownloaded[eCodeExecId] = true;
                    }


#ifdef OFFLINE_UTIL
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,NULL);
#else
			ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

			if((ui32CodePtr+ ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download code (IMG ID = %#x) of Audio Processing (%#x)"
						" is less than required (%d)",
						(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,count,
						ui32CodeSize
						));
				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

			BDBG_MSG(("Downloading  code (IMG ID = %#x) of Audio Processing (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,count,ui32CodePtr,ui32physAddress,ui32CodeSize));
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
#endif

			ui32TblPtr = ui32CodePtr + ui32CodeSize;

			/*Download encode table */
			eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
			{
				BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
				/* updating ui32CodePtr variable, used to track the used size */ 		
				ui32CodePtr += ui32CodeSize;				
				BDBG_MSG(("Invalid Image ID for  table of Audio Processing %#x Stage %d",count,i));
				continue;
			}
			bDownloaded = false;
			BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eProcessingAlgo,eCodeExecId,&bDownloaded,&eTempExecId);
#ifdef OFFLINE_UTIL
			if(!bDownloaded)
			{
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,NULL);
			}
#else
			if(!bDownloaded)
			{
				ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

				if((ui32TblPtr+ ui32TblSize) > ui32EndAddr)
				{
					BDBG_ERR(("Size available (%d) to download  table (IMG ID = %#x) of Audio Processing (%#x)"
							" is less than required (%d)",
							(ui32EndAddr-(ui32TblPtr+ ui32TblSize)),eTblFirmwareId,count,
							ui32TblSize
							));
					return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
				}
				
				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Downloading table (IMG ID = %#x) of Audio Processing (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
			}
			else
			{
				ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
				ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG((" Table (IMG ID = %#x) of Audio Processing (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
			}
			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	

			/* updating ui32CodePtr variable, used to track the used size */ 		
			if(false == bDownloaded)
			{
				ui32CodePtr += ui32CodeSize + ui32TblSize;
			}
			else
			{
				ui32CodePtr += ui32CodeSize;				
			}
#endif
		}
	}


#ifdef OFFLINE_UTIL
	return;
#else
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadAudProcessingExecs);	
	return rc;
#endif

}


/*******************************************************************************
Summary:
	This Function will be called only if the OpenTimeDownload is set. It will 
	download the Passthru executables.
	The entries of MIT wiil be updated as well.

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadPassthruExecs(BRAP_FWDWNLD_P_Param *pParam)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32CodePtr = 0,ui32CodeSize = 0,ui32TblPtr = 0,ui32TblSize = 0;
	BRAP_Img_Id eCodeFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_Img_Id eTblFirmwareId = BRAP_Img_Id_eInvalid ;

#ifdef OFFLINE_UTIL
	/*Offline utility application will pass some structure handle having members 
	iface and pImgContext. This will be assigned here in the local variables*/
	BRAP_OFFLINEUTIL_DwnldParams *hOfflineDwnldParams = NULL;
	hOfflineDwnldParams = pParam->pOfflineDwnldParams;
	iface = hOfflineDwnldParams->pImgInterface;
	pImgContext = hOfflineDwnldParams->pImgContext;

#else
	BRAP_Handle		hRap = NULL;
	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadPassthruExecs);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;

	ui32CodePtr= hRap->sDwnldMemInfo.sPtCodecBufInfo[0].ui32BaseAddr;	
	ui32EndAddr = ui32CodePtr+ hRap->sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;	
#endif

			/*Download passthru Code */
			eCodeFirmwareId = BRAP_Img_Id_ePassthruCode;

#ifdef OFFLINE_UTIL
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,NULL);
#else
			ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[BRAP_AF_P_AlgoId_ePassThru];

			if((ui32CodePtr + ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download passthru code (IMG ID = %#x) "
						" is less than required (%d)",
						(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,
						ui32CodeSize
						));
				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}
			
			BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

			BDBG_MSG(("Downloading passthru code (IMG ID = %#x)  at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,ui32CodePtr,ui32physAddress,ui32CodeSize));
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
#endif

			/*Download passthru table */
			eTblFirmwareId = BRAP_Img_Id_ePassthruTable;

			ui32TblPtr = ui32CodePtr + ui32CodeSize;
			
#ifdef OFFLINE_UTIL
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,NULL);

#else
			ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[BRAP_AF_P_AlgoId_ePassThru];

			if((ui32TblPtr + ui32TblSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download passthru table (IMG ID = %#x) of Audio Algorithm (Passthru)"
						" is less than required (%d)",
						(ui32EndAddr-(ui32TblPtr + ui32TblSize)),eTblFirmwareId,
						ui32TblSize
						));
				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);			
			BDBG_MSG(("Downloading passthru table (IMG ID = %#x) of Audio Algorithm (Passthru) at address 0x%x(Physical = %#x), size = 0x%x",
							eTblFirmwareId,ui32TblPtr,ui32physAddress,ui32TblSize));
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	

			BRAP_UpdateMitEntry(hRap,BRAP_AF_P_AlgoId_ePassThru,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);		
#endif

#ifdef OFFLINE_UTIL
	return;
#else
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadPassthruExecs);	
	return rc;
#endif

}



/*******************************************************************************
Summary:
	This Function will be called only if the OpenTimeDownload is set. It will 
	download the executables for all supported 	audio codecs.
	The entries of MIT wiil be updated as well.

Pseudo code:
DownloadAllSupportedExecs:
{
	
	For each Supported Audio Type
		- Download Framesync Exec
			- There are can be option to choose framesync from various
			  framesync execs available for an algorithm. Sodownload all
			  the framesync options available for algorithms.
			  
		- Download Decode Code+Table Exec
		- Download Passthru Code+Table Exec
		- Download Other required Execs.

	BRAP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pParam : Rap handle
		In case offline utility,Offline function will pass the a pointer to 
		structure containing required fields to copy Fw into the file.

Return:
	BERR_Code	- If RAPTOR
	void		- If Offline Utility
*******************************************************************************/
BERR_Code BRAP_FWDWNLD_P_DownloadSupportedCodecExecs(BRAP_FWDWNLD_P_Param *pParam)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32CodePtr = 0,ui32CodeSize = 0,ui32TblPtr = 0,ui32TblSize = 0;
	BRAP_Img_Id eCodeFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_Img_Id eTblFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_AF_P_AlgoId eCodeExecId ,eTempExecId;
	unsigned int count = 0,i = 0;
	bool bDownloaded= false;	

	
#ifdef OFFLINE_UTIL
	/*Offline utility application will pass some structure handle having members 
	iface and pImgContext. This will be assigned here in the local variables*/
	BRAP_OFFLINEUTIL_DwnldParams *hOfflineDwnldParams = NULL;
	hOfflineDwnldParams = pParam->pOfflineDwnldParams;
	iface = hOfflineDwnldParams->pImgInterface;
	pImgContext = hOfflineDwnldParams->pImgContext;
#else
	BRAP_Handle		hRap = NULL;
	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadSupportedCodecExecs);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;
	ui32CodePtr= hRap->sDwnldMemInfo.sFsBufInfo[0].ui32BaseAddr;	
	ui32EndAddr = ui32CodePtr+ hRap->sDwnldMemInfo.sFsBufInfo[0].ui32Size;	
#endif


	for(count = 0; count < BRAP_DSPCHN_AudioType_eMax; count++)
	{
            if(!(BRAP_P_IsPassthruSupportedWithoutLicense(count)))
            {
                if(!BRAP_FWDWNLD_P_IsAudCodecSupported(count))
                {
                     continue;
                }
		}

		eCodeExecId = BRAP_sDecAlgoExecInfo[count][BRAP_DSPCHN_DecodeMode_eDecode].eAlgoIds[0];                        
                if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                } 
		eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

		if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
		{
			BDBG_MSG(("Invalid Image ID for Framesync of Audio Algorithm %#x",count));
                    continue;
		}

            if(true == hRap->bIsExecDownloaded[eCodeExecId])
            {
                continue;
            }
            else
            {
                hRap->bIsExecDownloaded[eCodeExecId] = true;
            }


#ifdef OFFLINE_UTIL
		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,NULL);
#else
		ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

		if((ui32CodePtr + ui32CodeSize) > ui32EndAddr)
		{
			BDBG_ERR(("Size available (%d) to download Framesync (IMG ID = %#x) of Audio Algorithm (%#x)"
					"is less than required (%d)",
					(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,count,
					ui32CodeSize
					));
			return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
		}

		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

		BDBG_MSG(("Downloading Framesync(IMG ID = %#x) of Audio Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
						eCodeFirmwareId,count,ui32CodePtr,ui32physAddress,ui32CodeSize));
		BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
		BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);		
		ui32CodePtr += ui32CodeSize;
#endif
	}
		/*Download all decode Code+Table for supported algorithm*/	
#ifndef OFFLINE_UTIL
		ui32CodeSize = 0;
		ui32CodePtr= hRap->sDwnldMemInfo.sDecCodecBufInfo[0].ui32BaseAddr;	
		ui32EndAddr = ui32CodePtr + hRap->sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;	
#endif	
	for(count = 0; count < BRAP_DSPCHN_AudioType_eMax; count++)
	{
		if(false == BRAP_FWDWNLD_P_IsAudCodecSupported(count))
		{
			continue;
		}
		
		for(i = 2;i<BRAP_sDecAlgoExecInfo[count][BRAP_DSPCHN_DecodeMode_eDecode].NumNodes;i++)
		{

		/*Download decode Code */
			eCodeExecId = BRAP_sDecAlgoExecInfo[count][BRAP_DSPCHN_DecodeMode_eDecode].eAlgoIds[i];
                    if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }         
			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",count,i));
				break;
			}
                    if(true == hRap->bIsExecDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        hRap->bIsExecDownloaded[eCodeExecId] = true;
                    }


#ifdef OFFLINE_UTIL
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,NULL);
#else
			ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

			if((ui32CodePtr+ ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of Audio Algorithm (%#x)"
						" is less than required (%d)",
						(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,count,
						ui32CodeSize
						));
				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

			BDBG_MSG(("Downloading decode code (IMG ID = %#x) of Audio Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,count,ui32CodePtr,ui32physAddress,ui32CodeSize));
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
#endif

			ui32TblPtr = ui32CodePtr + ui32CodeSize;

			/*Download decode table */
			eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
			{
				BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
				ui32CodePtr += ui32CodeSize;	                
				BDBG_MSG(("Invalid Image ID for decode table of algorithm %#x Stage %d",count,i));
				continue;
			}
			bDownloaded = false;
			BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
			
#ifdef OFFLINE_UTIL
			if(!bDownloaded)
			{
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,NULL);
			}
#else
			if(!bDownloaded)
			{
				ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

				if((ui32TblPtr+ ui32TblSize) > ui32EndAddr)
				{
					BDBG_ERR(("Size available (%d) to download decode  table (IMG ID = %#x) of Audio Algorithm (%#x)"
							" is less than required (%d)",
							(ui32EndAddr-(ui32TblPtr+ ui32TblSize)),eTblFirmwareId,count,
							ui32TblSize
							));
					return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
				}
				
				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Downloading decode table (IMG ID = %#x) of Audio Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
			}
			else
			{
				ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
				ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Decode Table (IMG ID = %#x) of Audio Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
			}
			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	

			/* updating ui32CodePtr variable, used to track the used size */ 		
			if(false == bDownloaded)
			{
				ui32CodePtr += ui32CodeSize + ui32TblSize;
			}
			else
			{
				ui32CodePtr += ui32CodeSize;				
			}
#endif
		}
	}



#ifdef OFFLINE_UTIL
	return;
#else
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadSupportedCodecExecs);	
	return rc;
#endif

}

#ifdef RAP_VIDEOONDSP_SUPPORT
BERR_Code BRAP_FWDWNLD_P_DownloadVideoDecodeExecs(BRAP_FWDWNLD_P_Param *pParam,bool bDownloadAll,BRAP_DSPCHN_VideoType    eVideoType)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32CodePtr = 0,ui32CodeSize = 0,ui32TblPtr = 0,ui32TblSize = 0;
	BRAP_Img_Id eCodeFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_Img_Id eTblFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_AF_P_AlgoId eCodeExecId ,eTempExecId;
	unsigned int count = 0,i = 0,uiTempCount=0;
	bool bDownloaded= false,bWatchDogRecovery = false;	

	
#ifdef OFFLINE_UTIL
	/*Offline utility application will pass some structure handle having members 
	iface and pImgContext. This will be assigned here in the local variables*/
	BRAP_OFFLINEUTIL_DwnldParams *hOfflineDwnldParams = NULL;
	hOfflineDwnldParams = pParam->pOfflineDwnldParams;
	iface = hOfflineDwnldParams->pImgInterface;
	pImgContext = hOfflineDwnldParams->pImgContext;
#else
	BRAP_Handle		hRap = NULL;
	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadVideoDecodeExecs);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	bWatchDogRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);    
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;
	ui32CodePtr= hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32BaseAddr;	
	ui32EndAddr = ui32CodePtr+ hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;	
#endif


		/*Download all decode Code+Table for supported algorithm*/	
#ifndef OFFLINE_UTIL
		ui32CodeSize = 0;
		ui32CodePtr= hRap->sDwnldMemInfo.sDecCodecBufInfo[0].ui32BaseAddr;	
		ui32EndAddr = ui32CodePtr + hRap->sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;	
#endif	
	for(count = 0; (count < BRAP_DSPCHN_VideoType_eMax -BRAP_VIDEO_CODEC_START_INDEX); count++)
	{
            if(bDownloadAll == false)
            {
                count = eVideoType  -BRAP_VIDEO_CODEC_START_INDEX;
                if(uiTempCount > 0)
                    break;
            }
            uiTempCount++;
		if(false == BRAP_FWDWNLD_P_IsVidCodecSupported(count))
		{
			continue;
		}
            if(bDownloadAll == false)
            {
                unsigned int ui32FwCtxIndx;
                 BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap, count, &ui32FwCtxIndx,&bDownloaded,BRAP_DSP_ExecType_eVideoDecode,bWatchDogRecovery);

                /* If its Watchdog recovery, And its exec is not already in the DRAM, 
                then this is a BAS DEVICE STATE */

                if ((BRAP_P_GetWatchdogRecoveryFlag(hRap))
                && (true == bDownloaded))
                {
                    BDBG_ERR(("In Watchdog recovery Decode exec is not already in the DRAM"
                    " Corruption of DRAM "));
                    return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
                }

                /* If no Free Buf to download algo code */
                if((RAP_INVALID_INDEX == ui32FwCtxIndx)
                || ui32FwCtxIndx >= BRAP_DSP_P_MAX_VIDEODECODE_CTXT)
                {
                    BDBG_ERR(("ERROR!!! -- All decode context buffers are in use."));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }   
        	ui32CodePtr= hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[ui32FwCtxIndx].ui32BaseAddr;	
        	ui32EndAddr = ui32CodePtr+ hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[ui32FwCtxIndx].ui32Size;	    
            BDBG_ERR(("ui32CodePtr = %x,ui32EndAddr=%x,hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[ui32FwCtxIndx].ui32Size =%d"
                ,ui32CodePtr,ui32EndAddr,hRap->sDwnldMemInfo.sVideoDecCodecBufInfo[ui32FwCtxIndx].ui32Size));
            }		
		for(i = 0;i<BRAP_sDecAlgoExecInfo[(count + BRAP_VIDEO_CODEC_START_INDEX)][BRAP_DSPCHN_DecodeMode_eDecode].NumNodes;i++)
		{

		/*Download decode Code */
			eCodeExecId = BRAP_sDecAlgoExecInfo[(count + BRAP_VIDEO_CODEC_START_INDEX)][BRAP_DSPCHN_DecodeMode_eDecode].eAlgoIds[i];
                    if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }         
			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",count,i));
				break;
			}
                    if(true == hRap->bIsExecDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        hRap->bIsExecDownloaded[eCodeExecId] = true;
                    }


#ifdef OFFLINE_UTIL
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,NULL);
#else
			ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

			if((ui32CodePtr+ ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of Video Algorithm (%#x)"
						" is less than required (%d)",
						(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,count,
						ui32CodeSize
						));
				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

			BDBG_MSG(("Downloading decode code (IMG ID = %#x) of Video Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,count,ui32CodePtr,ui32physAddress,ui32CodeSize));
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	
#endif

			ui32TblPtr = ui32CodePtr + ui32CodeSize;

			/*Download decode table */
			eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
			{
				BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
				ui32CodePtr += ui32CodeSize;	                
				BDBG_MSG(("Invalid Image ID for decode table of Video algorithm %#x Stage %d",count,i));
				continue;
			}
			bDownloaded = false;
			BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
			
#ifdef OFFLINE_UTIL
			if(!bDownloaded)
			{
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,NULL);
			}
#else
			if(!bDownloaded)
			{
				ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

				if((ui32TblPtr+ ui32TblSize) > ui32EndAddr)
				{
					BDBG_ERR(("Size available (%d) to download decode  table (IMG ID = %#x) of Video Algorithm (%#x)"
							" is less than required (%d)",
							(ui32EndAddr-(ui32TblPtr+ ui32TblSize)),eTblFirmwareId,count,
							ui32TblSize
							));
					return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
				}
				
				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Downloading decode table (IMG ID = %#x) of Video Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
			}
			else
			{
				ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
				ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Decode Table (IMG ID = %#x) of Video Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
			}
			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	

			/* updating ui32CodePtr variable, used to track the used size */ 		
			if(false == bDownloaded)
			{
				ui32CodePtr += ui32CodeSize + ui32TblSize;
			}
			else
			{
				ui32CodePtr += ui32CodeSize;				
			}
#endif
		}      
	}
#ifdef OFFLINE_UTIL
	return;
#else
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadVideoDecodeExecs);	
	return rc;
#endif

}
#endif


/*******************************************************************************
Summary:
	This returns if the audio processing needs to be downloaded at Device open 
	time or Channel Start time.
*******************************************************************************/
void BRAP_FWDWNLD_P_IfOpenTimeProcDownload(bool *bOpenTimeAudProcDownload)
{
    BDBG_ENTER(BRAP_FWDWNLD_P_IfOpenTimeAudProcDownload);	

	*bOpenTimeAudProcDownload = true;

    BDBG_LEAVE(BRAP_FWDWNLD_P_IfOpenTimeAudProcDownload);	
	return;
}
/******************************************************************************
Summary:
	This Function returns true, If the Encode algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BRAP_FWDWNLD_P_IsEncodeSupported(unsigned int AlgoId)
{
	return	bEncodeSupported[AlgoId];
}

/******************************************************************************
Summary:
	This Function returns true, If the Processing algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BRAP_FWDWNLD_P_IsAudProcSupported(unsigned int AlgoId)
{
	return	bAudioProcSupported[AlgoId];
}

/******************************************************************************
Summary:
	This Function returns true, If the audio processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BRAP_FWDWNLD_P_IsAudCodecSupported(unsigned int AlgoId)
{
	return bAudioCodecSupported[AlgoId];
}
	
#ifdef RAP_VIDEOONDSP_SUPPORT
/******************************************************************************
Summary:
	This Function returns true, If the audio processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BRAP_FWDWNLD_P_IsVidCodecSupported(unsigned int AlgoId)
{
#if 1
    BSTD_UNUSED(AlgoId);
	return bVideoCodecSupported[0];
#else
    if(AlgoId == BRAP_DSPCHN_VideoType_eRealVideo9)
        return true;
    else
        return false;
#endif    
}
#endif

#ifndef OFFLINE_UTIL
/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Decode Mode and Decode algo type.
*******************************************************************************/
void BRAP_FWDWNLD_P_GetExecIds(
						BRAP_DSPCHN_DecodeMode eDecMode, 
						BRAP_DSPCHN_AudioType eAudType, 
						BRAP_AF_P_sALGO_EXEC_INFO *psExecID /* [out] */)
{

    BDBG_ENTER(BRAP_FWDWNLD_P_GetExecIds);

    if((eDecMode < BRAP_DSPCHN_DecodeMode_eMax)
        &&(eAudType < BRAP_DSPCHN_AudioType_eMax))
    {
	*psExecID = BRAP_sDecAlgoExecInfo[eAudType][eDecMode];
    }

    BDBG_LEAVE(BRAP_FWDWNLD_P_GetExecIds);
	return ;
}

#ifdef RAP_REALVIDEO_SUPPORT
/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Decode Mode and Decode algo type.
*******************************************************************************/

void BRAP_FWDWNLD_P_GetVidExecIds(
						BRAP_DSPCHN_DecodeMode eDecMode, 
						BRAP_DSPCHN_VideoType eVidType, 
						BRAP_AF_P_sALGO_EXEC_INFO *psExecID /* [out] */)
{

    BDBG_ENTER(BRAP_FWDWNLD_P_GetVidExecIds);

    if((eDecMode < BRAP_DSPCHN_DecodeMode_eMax) && (eVidType >= BRAP_VIDEO_CODEC_START_INDEX)
        &&(eVidType < BRAP_DSPCHN_VideoType_eMax))
    {
	*psExecID = BRAP_sDecAlgoExecInfo[eVidType][eDecMode];
    }

    BDBG_LEAVE(BRAP_FWDWNLD_P_GetVidExecIds);
	return ;
}
#endif

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Decode Mode and audio processing algorithm type.
*******************************************************************************/
void  BRAP_FWDWNLD_P_GetAudProcExecIds(
				BRAP_CIT_P_ProcessingType eAudProcAlgo,	/* [in]Audio processing algorithm */
				BRAP_AF_P_sALGO_EXEC_INFO *psAudProcExecID /* [out] */)
{
	*psAudProcExecID = BRAP_sPpAlgoExecInfo[eAudProcAlgo][0];
	return;
}

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Encode algorithm type.
*******************************************************************************/
void  BRAP_FWDWNLD_P_GetEncodeExecIds(
				BRAP_CIT_P_EncAudioType eEncodeAlgo,	/* [in]Audio processing algorithm */
				BRAP_AF_P_sALGO_EXEC_INFO *psEncodeExecID /* [out] */)
{
	*psEncodeExecID = BRAP_sEncAlgoExecInfo[eEncodeAlgo][0];
	return;
}

#endif

/*******************************************************************************
Summary:
	This function returns if the Algo table is already downloaded or not. If its
	downloaded, it sets bDownloaded=true and eCodeFirmwareId to the Algo Code 
	with which the table was associated in the MIT when it was downloaded earlier.
*******************************************************************************/
void BRAP_FWDWNLD_P_IsTableDownloaded(
					BRAP_DSP_ExecType	eExecType,	
					BRAP_AF_P_AlgoId	 eCodeExecId,
					bool		*bDownloaded,
					BRAP_AF_P_AlgoId *eExecId
					)
{
    BDBG_ENTER(BRAP_FWDWNLD_P_IsTableDownloaded);
    BSTD_UNUSED(eCodeExecId);    

    *eExecId = BRAP_AF_P_AlgoId_eAudioAlgoStartIdx;

		if(BRAP_DSP_ExecType_eDecode == eExecType)
		{
			*bDownloaded = false;		

			if(0)
			{
				
			}
#ifdef RAP_MPEG_SUPPORT
#endif
		}
		else if(BRAP_DSP_ExecType_ePassthru == eExecType)
		{
				*bDownloaded = false;							
		}
		else if(BRAP_DSP_ExecType_eProcessingAlgo == eExecType)
		{
			*bDownloaded = false;				
#ifdef RAP_MP3_ENCODE_SUPPORT		
#endif
		}
		else
		{
				*bDownloaded = false;				
		}
		
	    BDBG_LEAVE(BRAP_FWDWNLD_P_IsTableDownloaded);	
		return;
}


bool    BRAP_FWDWNLD_P_IsGfxSupported(unsigned int AlgoId)
{
	return bGfxSupported[AlgoId];
}

bool    BRAP_FWDWNLD_P_IsScmSupported(unsigned int AlgoId)
{
	return bScmSupported[AlgoId];
}

#ifdef RAP_GFX_SUPPORT
/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a Gfx.
*******************************************************************************/
void BRAP_FWDWNLD_P_GetGfxExecIds(
						BRAP_DSPCHN_AudioType eAudType, 
						BRAP_AF_P_sALGO_EXEC_INFO *psExecID /* [out] */)
{

    BDBG_ENTER(BRAP_FWDWNLD_P_GetGfxExecIds);

	*psExecID = BRAP_sGfxDecodeExecInfo[eAudType];

    BDBG_LEAVE(BRAP_FWDWNLD_P_GetGfxExecIds);
	return ;
}

BERR_Code BRAP_FWDWNLD_P_DownloadGfxExecs(BRAP_FWDWNLD_P_Param *pParam,bool bDownloadAll,BRAP_DSPCHN_GfxType    eGfxType)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32CodePtr = 0,ui32CodeSize = 0,ui32TblPtr = 0,ui32TblSize = 0;
	BRAP_Img_Id eCodeFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_Img_Id eTblFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_AF_P_AlgoId eCodeExecId ,eTempExecId;
	unsigned int count = 0,i = 0,uiTempCount=0;
	bool bDownloaded= false,bWatchDogRecovery = false;	

	

	BRAP_Handle		hRap = NULL;
	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

        BDBG_ENTER(BRAP_FWDWNLD_P_DownloadGfxExecs);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	bWatchDogRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);    
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;
	for(count = 0; (count < BRAP_DSPCHN_GfxType_eMax ); count++)
	{
            if(bDownloadAll == false)
            {
                count = eGfxType;
                if(uiTempCount > 0)
                    break;
            }
            uiTempCount++;
        	if(false == BRAP_FWDWNLD_P_IsGfxSupported(count))
        	{
        		continue;
        	}
            if(bDownloadAll == false)
            {
                unsigned int ui32FwCtxIndx;
                
                 BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap, count, &ui32FwCtxIndx,&bDownloaded,BRAP_DSP_ExecType_eGfx,bWatchDogRecovery);
                /* If its Watchdog recovery, And its exec is not already in the DRAM, 
                then this is a BAS DEVICE STATE */

                if ((BRAP_P_GetWatchdogRecoveryFlag(hRap))
                && (true == bDownloaded))
                {
                    BDBG_ERR(("In Watchdog recovery Decode exec is not already in the DRAM"
                    " Corruption of DRAM "));
                    return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
                }

                /* If no Free Buf to download algo code */
                if((RAP_INVALID_INDEX == ui32FwCtxIndx)
                || ui32FwCtxIndx >= BRAP_DSP_P_MAX_GFX_CTXT)
                {
                    BDBG_ERR(("ERROR!!! -- All GFX context buffers are in use."));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }   
        	ui32CodePtr= hRap->sDwnldMemInfo.sGfxBufInfo[ui32FwCtxIndx].ui32BaseAddr;	
        	ui32EndAddr = ui32CodePtr+ hRap->sDwnldMemInfo.sGfxBufInfo[ui32FwCtxIndx].ui32Size;	    
            BDBG_MSG(("ui32CodePtr = %x,ui32EndAddr=%x,hRap->sDwnldMemInfo.sGfxBufInfo[ui32FwCtxIndx].ui32Size =%d"
                ,ui32CodePtr,ui32EndAddr,hRap->sDwnldMemInfo.sGfxBufInfo[ui32FwCtxIndx].ui32Size));
            }		
		for(i = 0;i<BRAP_sGfxDecodeExecInfo[(count )].NumNodes;i++)
		{

		/*Download decode Code */
			eCodeExecId = BRAP_sGfxDecodeExecInfo[(count )].eAlgoIds[i];
                    if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }         
			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",count,i));
				break;
			}
                    if(true == hRap->bIsExecDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        hRap->bIsExecDownloaded[eCodeExecId] = true;
                    }



			ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

			if((ui32CodePtr+ ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download GFX code (IMG ID = %#x) of Video Algorithm (%#x)"
						" is less than required (%d)",
						(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,count,
						ui32CodeSize
						));
				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

			BDBG_MSG(("Downloading GFX code (IMG ID = %#x) of Video Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,count,ui32CodePtr,ui32physAddress,ui32CodeSize));
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	


			ui32TblPtr = ui32CodePtr + ui32CodeSize;

			/*Download decode table */
			eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
			{
				BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
				ui32CodePtr += ui32CodeSize;	                
				BDBG_MSG(("Invalid Image ID for decode table of Video algorithm %#x Stage %d",count,i));
				continue;
			}
			bDownloaded = false;
			BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
			

			if(!bDownloaded)
			{
				ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

				if((ui32TblPtr+ ui32TblSize) > ui32EndAddr)
				{
					BDBG_ERR(("Size available (%d) to download GFX  table (IMG ID = %#x) of Video Algorithm (%#x)"
							" is less than required (%d)",
							(ui32EndAddr-(ui32TblPtr+ ui32TblSize)),eTblFirmwareId,count,
							ui32TblSize
							));
					return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
				}
				
				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Downloading GFX table (IMG ID = %#x) of Video Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
			}
			else
			{
				ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
				ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Decode Table (IMG ID = %#x) of Video Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
			}
			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	

			/* updating ui32CodePtr variable, used to track the used size */ 		
			if(false == bDownloaded)
			{
				ui32CodePtr += ui32CodeSize + ui32TblSize;
			}
			else
			{
				ui32CodePtr += ui32CodeSize;				
			}
		}      
	}
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadGfxExecs);	
	return rc;


}
#endif

#ifdef RAP_SCM_SUPPORT

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a SCM.
*******************************************************************************/
void BRAP_FWDWNLD_P_GetScmExecIds(
						BRAP_DSPCHN_AudioType eAudType, 
						BRAP_AF_P_sALGO_EXEC_INFO *psExecID /* [out] */)
{

    BDBG_ENTER(BRAP_FWDWNLD_P_GetScmExecIds);

	*psExecID = BRAP_sScmDecodeExecInfo[eAudType];

    BDBG_LEAVE(BRAP_FWDWNLD_P_GetScmExecIds);
	return ;
}


BERR_Code BRAP_FWDWNLD_P_DownloadSCMExecs(BRAP_FWDWNLD_P_Param *pParam,bool bDownloadAll,BRAP_DSPCHN_ScmType    eSCMType)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t ui32CodePtr = 0,ui32CodeSize = 0,ui32TblPtr = 0,ui32TblSize = 0;
	BRAP_Img_Id eCodeFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_Img_Id eTblFirmwareId = BRAP_Img_Id_eInvalid;
	BRAP_AF_P_AlgoId eCodeExecId ,eTempExecId;
	unsigned int count = 0,i = 0,uiTempCount=0;
	bool bDownloaded= false,bWatchDogRecovery = false;	

	

	BRAP_Handle		hRap = NULL;
	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

    BDBG_ENTER(BRAP_FWDWNLD_P_DownloadSCMExecs);		
	BDBG_ASSERT(pParam);
	
	hRap = pParam->hRap;
	bWatchDogRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);    
	iface = hRap->sSettings.pImgInterface;
	pImgContext = hRap->sSettings.pImgContext;
	for(count = 0; (count < BRAP_DSPCHN_ScmType_eMax ); count++)
	{
#if 0	/* Needs to be verified with Gautam */
            if(bDownloadAll == false)
#endif				
            if(bDownloadAll == false && uiTempCount == 0)
            {
                count = eSCMType;
#if 0 /* Needs to be verified with Gautam */
                if(uiTempCount > 0)
                    break;
#endif
            }
            uiTempCount++;
        	if(false == BRAP_FWDWNLD_P_IsScmSupported(count))
        	{
        		continue;
        	}
            if(bDownloadAll == false)
            {
                unsigned int ui32FwCtxIndx;
                
                 BRAP_DSPCHN_P_AllocateFwCtxtBuf(hRap, count, &ui32FwCtxIndx,&bDownloaded,BRAP_DSP_ExecType_eScm,bWatchDogRecovery);
                /* If its Watchdog recovery, And its exec is not already in the DRAM, 
                then this is a BAD DEVICE STATE */

                if ((BRAP_P_GetWatchdogRecoveryFlag(hRap))
                && (true == bDownloaded))
                {
                    BDBG_ERR(("In Watchdog recovery Decode exec is not already in the DRAM"
                    " Corruption of DRAM "));
                    return BERR_TRACE( BRAP_ERR_BAD_DEVICE_STATE );
                }

                /* If no Free Buf to download algo code */
                if((RAP_INVALID_INDEX == ui32FwCtxIndx)
                || ui32FwCtxIndx >= BRAP_DSP_P_MAX_SCM_CTXT)
                {
                    BDBG_ERR(("ERROR!!! -- All SCM context buffers are in use."));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }   
        	ui32CodePtr= hRap->sDwnldMemInfo.sScmBufInfo[ui32FwCtxIndx].ui32BaseAddr;	
        	ui32EndAddr = ui32CodePtr+ hRap->sDwnldMemInfo.sScmBufInfo[ui32FwCtxIndx].ui32Size;	    
            BDBG_MSG(("ui32CodePtr = %x,ui32EndAddr=%x,hRap->sDwnldMemInfo.sScmBufInfo[ui32FwCtxIndx].ui32Size =%d"
                ,ui32CodePtr,ui32EndAddr,hRap->sDwnldMemInfo.sScmBufInfo[ui32FwCtxIndx].ui32Size));
            }		
		for(i = 0;i<BRAP_sScmDecodeExecInfo[(count )].NumNodes;i++)
		{

			/*Download scm Code */
			eCodeExecId = BRAP_sScmDecodeExecInfo[(count )].eAlgoIds[i];
                    if(eCodeExecId == BRAP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }         
			eCodeFirmwareId = hRap->sImgIdMappingArrays.ui32CodeImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",count,i));
				break;
			}
                    if(true == hRap->bIsExecDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        hRap->bIsExecDownloaded[eCodeExecId] = true;
                    }



			ui32CodeSize = hRap->sImgIdMappingArrays.ui32CodeSize[eCodeExecId];

			if((ui32CodePtr+ ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download SCM code (IMG ID = %#x) of SCM Algorithm (%#x)"
						" is less than required (%d)",
						(ui32EndAddr-(ui32CodePtr + ui32CodeSize)),eCodeFirmwareId,count,
						ui32CodeSize
						));
				return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			}

			BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32CodePtr), &ui32physAddress);

			BDBG_MSG(("Downloading SCM code (IMG ID = %#x) of SCM Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,count,ui32CodePtr,ui32physAddress,ui32CodeSize));
			BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32CodePtr,eCodeFirmwareId,hRap->hHeap);	


			ui32TblPtr = ui32CodePtr + ui32CodeSize;

			/*Download scm table */
			eTblFirmwareId = hRap->sImgIdMappingArrays.ui32TableImgId[eCodeExecId];

			if(BRAP_Img_Id_eInvalid == eTblFirmwareId)
			{
				BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,0,0);	
				ui32CodePtr += ui32CodeSize;	                
				BDBG_MSG(("Invalid Image ID for scm table of scm algorithm %#x Stage %d",count,i));
				continue;
			}
			bDownloaded = false;
			BRAP_FWDWNLD_P_IsTableDownloaded(BRAP_DSP_ExecType_eScm,eCodeExecId,&bDownloaded,&eTempExecId);
			

			if(!bDownloaded)
			{
				ui32TblSize = hRap->sImgIdMappingArrays.ui32TableSize[eCodeExecId];

				if((ui32TblPtr+ ui32TblSize) > ui32EndAddr)
				{
					BDBG_ERR(("Size available (%d) to download SCM  table (IMG ID = %#x) of SCM Algorithm (%#x)"
							" is less than required (%d)",
							(ui32EndAddr-(ui32TblPtr+ ui32TblSize)),eTblFirmwareId,count,
							ui32TblSize
							));
					return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
				}
				
				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Downloading GFX table (IMG ID = %#x) of SCM Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
				BRAP_FWDWNLD_P_CopyFWImageToMem(iface,pImgContext,ui32TblPtr,eTblFirmwareId,hRap->hHeap);	
			}
			else
			{
				ui32TblPtr = hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
				ui32TblSize= hRap->sMit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

				BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(ui32TblPtr), &ui32physAddress);
				BDBG_MSG(("Decode Table (IMG ID = %#x) of SCM Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,ui32TblPtr,ui32physAddress,ui32TblSize));
			}
			BRAP_UpdateMitEntry(hRap,eCodeExecId,ui32CodePtr,ui32CodeSize,ui32TblPtr,ui32TblSize);	

			/* updating ui32CodePtr variable, used to track the used size */ 		
			if(false == bDownloaded)
			{
				ui32CodePtr += ui32CodeSize + ui32TblSize;
			}
			else
			{
				ui32CodePtr += ui32CodeSize;				
			}
		}      
	}
    BDBG_LEAVE(BRAP_FWDWNLD_P_DownloadSCMExecs);	
	return rc;


}

/***************************************************************************
 Summary:
	 This API sets the SCM Algo id based  on the CA System ID.
	 
 Description:
	 This API sets the SCM Algo id based  on the CA System ID.

 
 ****************************************************************************/

void BRAP_SCM_P_SetSCMAlgorithmID(BRAP_DSPCHN_ScmType eScmAlgoType)
{
	bScmSupported[eScmAlgoType] = true;
}

#endif


