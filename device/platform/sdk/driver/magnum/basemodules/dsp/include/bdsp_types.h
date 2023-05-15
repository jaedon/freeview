/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_types.h $
 * $brcm_Revision: Hydra_Software_Devel/25 $
 * $brcm_Date: 2/6/13 6:34p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/bdsp_types.h $
 * 
 * Hydra_Software_Devel/25   2/6/13 6:34p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/5   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/24   1/10/13 10:49a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/MAGNUM_ILBC_ISAC_integration/1   12/20/12 11:11a skalapur
 * FWRAA-474:iLBC and iSAC integration
 * 
 * Hydra_Software_Devel/23   11/26/12 1:13p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
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
 * Hydra_Software_Devel/22   8/23/12 12:15a rshijith
 * FWRAA-424: Code modification to support VP8 encoder in raaga.
 * 
 * Hydra_Software_Devel/21   7/16/12 3:52p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/20   6/6/12 6:56p ananthan
 * FWRAA-443: Add MAT Encoder support in Raaga
 * 
 * Hydra_Software_Devel/19   6/6/12 12:58p ananthan
 * FWRAA-443: Add MAT Encoder support in Raaga
 * 
 * Hydra_Software_Devel/18   5/11/12 3:29p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
 * sys_top_raaga changes
 * 
 * Hydra_Software_Devel/17   3/16/12 5:00p ramanatm
 * SW7425-2378:[7425] Add support for Monkey Audio Codec (MAC)
 * 
 * Hydra_Software_Devel/16   2/22/12 7:21p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/15   2/3/12 2:22p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/14   1/27/12 1:02p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/13   1/13/12 9:50p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/12   12/29/11 5:55p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/11   12/5/11 12:33p ashoky
 * FWRAA-330: WMA Encoder Development on Raaga.
 * 
 * Hydra_Software_Devel/10   11/8/11 1:36p ramanatm
 * FWRAA-348:[7425] Vorbis bringup
 * 
 * Hydra_Software_Devel/9   8/5/11 5:50p ananthan
 * SW7425-724: [7425] Add BTSC audio encoder support
 * 
 * Hydra_Software_Devel/9   8/5/11 5:00p kiranr
 * SW7425-724: Adding support for BTSCEnc
 * 
 * Hydra_Software_Devel/8   8/1/11 3:11p gprasad
 * FWRAA-327: Add support for G.711/G.726/G.729 codecs
 * 
 * Hydra_Software_Devel/7   7/26/11 2:33p gprasad
 * SWDTV-6761: Add StudioSound support
 * 
 * Hydra_Software_Devel/6   7/12/11 4:44p gprasad
 * SW7422-370: Add changes for bdsp_types.h based on code review feedback
 * 
 * Hydra_Software_Devel/5   6/22/11 3:31p gprasad
 * SW7422-370: Code review changes in bdsp_tyes.h
 * 
 * Hydra_Software_Devel/4   6/9/11 8:12p gautamk
 * SW7422-357 : [7425] Modifying API prtotype for Add/Remove stage input
 * 
 * Hydra_Software_Devel/3   5/16/11 10:07p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/2   4/13/11 8:18p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   4/6/11 2:16p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/6   3/21/11 7:06p jgarrett
 * SW7422-356: Correcting type names for GenCdbItb
 * 
 * Hydra_Software_Devel/5   3/5/11 5:01a srajapur
 * SW7422-247 : [7422] Adding GenCdbItb audio processing support.
 * 
 * Hydra_Software_Devel/4   1/19/11 7:23a gautamk
 * SW7422-191:Adding API for Default settings.
 * 
 * Hydra_Software_Devel/3   1/19/11 2:08a gautamk
 * SW7422-191: Removing eInvalid from enum
 * 
 * Hydra_Software_Devel/2   1/13/11 6:00a gautamk
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
 ***************************************************************************/

#ifndef BDSP_TYPES_H_
#define BDSP_TYPES_H_

#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bimg.h"
#include "bavc.h"




/***************************************************************************
Summary:
DSP Handle
***************************************************************************/
typedef struct BDSP_Device *BDSP_Handle;

/***************************************************************************
Summary:
DSP Context Handle
***************************************************************************/
typedef struct BDSP_Context *BDSP_ContextHandle;

/***************************************************************************
Summary:
DSP Task Handle
***************************************************************************/
typedef struct BDSP_Task *BDSP_TaskHandle;

/***************************************************************************
Summary:
DSP Task Stage Input Handle
***************************************************************************/
typedef struct BDSP_TaskStageInput *BDSP_TaskStageInputHandle;

/***************************************************************************
Summary:
DSP External Interrupt Handle
***************************************************************************/
typedef struct BDSP_ExternalInterrupt *BDSP_ExternalInterruptHandle;

/***************************************************************************
Summary:
DSP Rdb Register handle 
***************************************************************************/
typedef struct BDSP_RdbRegister *BDSP_RdbRegisterHandle;

#if 0
/***************************************************************************
Summary:
Task Types
***************************************************************************/
typedef enum BDSP_TaskType
{
    BDSP_TaskType_eAudio,
    BDSP_TaskType_eVideo,
    BDSP_TaskType_eGraphics,
    BDSP_TaskType_eSecurity,
    BDSP_TaskType_eMax
} BDSP_TaskType;
#endif

/***************************************************************************
Summary:
Task Types
***************************************************************************/
typedef enum BDSP_ContextType
{
    BDSP_ContextType_eAudio,
    BDSP_ContextType_eVideo,
    BDSP_ContextType_eVideoEncode,
    BDSP_ContextType_eGraphics,
    BDSP_ContextType_eSecurity,
    BDSP_ContextType_eScm,
    BDSP_ContextType_eMax
} BDSP_ContextType;


/***************************************************************************
Summary:
Placeholder Types
***************************************************************************/
    typedef         uint32_t                    BDSP_TIME_45KHZ_TICKS;

/***************************************************************************
Summary:
    This enumeration defines the supported audio input type.

***************************************************************************/
typedef enum BDSP_AudioType
{
    BDSP_AudioType_eMpeg,            /* MPEG */
    BDSP_AudioType_eAacAdts,         /* AAC ADTS */
    BDSP_AudioType_eAacLoas,         /* AAC LOAS */       
    BDSP_AudioType_eAacSbrAdts,      /* AAC_SBR ADTS */
    BDSP_AudioType_eAacSbrLoas,       /* AAC_SBR LOAS */
    BDSP_AudioType_eAc3,             /* AC3 */
    BDSP_AudioType_eAc3Plus,         /* AC3_PLUS */
    BDSP_AudioType_eLpcmBd,          /* LPCM Blue Ray Disc */
    BDSP_AudioType_eLpcmHdDvd,       /* LPCM HD-DVD */
    BDSP_AudioType_eDtshd,           /* DTSHD */
    BDSP_AudioType_eLpcmDvd,         /* LPCM DVD */
    BDSP_AudioType_eWmaStd,          /* WMA Standard */
    BDSP_AudioType_eMlp,             /* MLP */
    BDSP_AudioType_ePcm,             /* PCM Data */
    BDSP_AudioType_eDtsLbr,          /* DTS-LBR */
    BDSP_AudioType_eDdp7_1,          /* DDP 7.1 */  
    BDSP_AudioType_eMpegMc,          /* MPEG MC*/        
    BDSP_AudioType_eWmaPro,          /* WMA Pro */
    BDSP_AudioType_eLpcmDvdA,        /* LPCM A DVD*/
    BDSP_AudioType_eDtsBroadcast,    /* DTS Broadcast*/
    BDSP_AudioType_ePcmWav,          /* PCM On AVI*/         
    BDSP_AudioType_eAmr,             /* AMR decoder */
    BDSP_AudioType_eDra,             /* DRA Decoder */
    BDSP_AudioType_eRealAudioLbr,    /* Real Audio LBR*/   
    BDSP_AudioType_eAdpcm,           /* ADPCM Decode*/   
    BDSP_AudioType_eG711G726,        /* G.711/G.726 Decode */   
    BDSP_AudioType_eG729,            /* G.729 Decode */   
    BDSP_AudioType_eVorbis,          /* Vorbis Decode */   
    BDSP_AudioType_eG723_1,          /* G.723.1 Decode */   
    BDSP_AudioType_eFlac,            /* Flac Decode */   
    BDSP_AudioType_eMac,            /* Mac Decode */   
    BDSP_AudioType_eAmrwb,             /* AMRWB decoder */
	BDSP_AudioType_eiLBC,             /* ILBC decoder */
	BDSP_AudioType_eiSAC,           /* iSAC decoder */
    BDSP_AudioType_eMax             /* Max value */
} BDSP_AudioType;

/*#define BDSP_VIDEO_CODEC_START_INDEX    BDSP_AudioType_eMax*/

typedef enum BDSP_VideoType
{
    BDSP_VideoType_eRealVideo9, /* Real Video*/
    BDSP_VideoType_eVP6,
    BDSP_VideoType_eMax,			    /* Max value */
    BDSP_VideoType_eInvalid	= 0xFF    
}BDSP_VideoType;
typedef enum BDSP_ScmType
{
    BDSP_ScmType_eScm1,                         /* Scm1 */
    BDSP_ScmType_eScm2,                     /* Scm2 */
    BDSP_ScmType_eScm3,                     /* Scm3 */
    BDSP_ScmType_eMax,
    BDSP_ScmType_eInvalid       = 0xFF
}BDSP_ScmType;

/***************************************************************************
Summary:
    This enumeration defines the supported audio passthrough type.

***************************************************************************/
typedef enum BDSP_PassThruType
{
	BDSP_PassThruType_eGeneric,
	BDSP_PassThruType_eMlp,
	BDSP_PassThruType_eMax
}BDSP_PassThruType;


/***************************************************************************
Summary:
    This enumeration defines various decode modes.

***************************************************************************/
typedef enum BDSP_DecodeMode
{
    BDSP_DecodeMode_eDecode,     /* Decode mode */
    BDSP_DecodeMode_ePassThru,   /* Pass thru mode */
    BDSP_DecodeMode_eMax
} BDSP_DecodeMode;  

/*********************************************************************
Summary:
    Enums that describes the various pre and post processing algorithms.

Description:
    This enum describes the various pre and post processing audio
	algorithms supported. 

See Also:
	BDSP_AudioEncode.
**********************************************************************/ 

typedef enum BDSP_AudioProcessing
{
    BDSP_AudioProcessing_eDtsNeo,			    /* DTS-Neo */     
	BDSP_AudioProcessing_eAVL,				    /* Automated Volume Level control. */
	BDSP_AudioProcessing_ePLll,			        /* Dolby Prologic-II. */
	BDSP_AudioProcessing_eSrsXt,			    /* TruSurroundXT. */
	BDSP_AudioProcessing_eBbe,				    /* BBE */
	BDSP_AudioProcessing_eSrc,				    /* Sample Rate Change */
	BDSP_AudioProcessing_eCustomVoice,		    /* CUSTOM Voice Algorithm */		
	BDSP_AudioProcessing_eAacDownmix,		    /* Downmix algorithm for AAC */	
	BDSP_AudioProcessing_eDsola,			    /* DSOLA */
	BDSP_AudioProcessing_eSrsHd,			    /* TruSurroundHD. */
	BDSP_AudioProcessing_eGenericPassThru,	    /* Generic pass through*/
	BDSP_AudioProcessing_eMlpPassThru,	    /* Generic pass through*/
	BDSP_AudioProcessing_eSrsTruVolume,		    /* SRS Tru Volume */			
	BDSP_AudioProcessing_eDolbyVolume,		    /* Dolby Volume */			
	BDSP_AudioProcessing_eAudysseyVolume,	    /* Audyssey Volume */
	BDSP_AudioProcessing_eBrcm3DSurround,	    /* Brcm 3D Surround  */
	BDSP_AudioProcessing_eFWMixer,	            /* FW Mixer */
	BDSP_AudioProcessing_eAudysseyABX,		    /* Audyssey ABX  */	
	BDSP_AudioProcessing_eDdre,				    /* DDRE post processing  */
	BDSP_AudioProcessing_eDv258,			    /* DV258 post processing  */
	BDSP_AudioProcessing_eSrsCsdTd,			    /* SRS CS decoder and TruDialog  */
	BDSP_AudioProcessing_eSrsEqHl,			    /* SRS Equalizer HardLimiter  */
	BDSP_AudioProcessing_eCustomDbe,		    /* CUSTOM DBE Algorithm */		
	BDSP_AudioProcessing_eCustomAcf,		    /* CUSTOM ACF Algorithm */		
	BDSP_AudioProcessing_eCustomAvlp,		    /* CUSTOM AVLP Algorithm */		
    BDSP_AudioProcessing_eGenCdbItb,            /* Generate CdbItb algorithm */
	BDSP_AudioProcessing_eBtscEncoder,		/* BTSC Encoder */
	BDSP_AudioProcessing_eSpeexAec,		        /* Speex acoustic echo canceller */
	BDSP_AudioProcessing_eMax,					/* Max value */
	BDSP_AudioProcessing_eInvalid = 0x7FFFFFFF
} BDSP_AudioProcessing;

/*********************************************************************
Summary:
    Enums that describes the various Encode algorithms.

Description:
    This enum describes the various encode algorithms supported. The 
	encode stage for the spplication is just like a post process. But 
	for the DSP, it is a different processing algorithm all together.
	To ensure functional separation, the PI will translate the processing
	type it receives from the application to the Encode audio type &
	processing audio type algorithms.

See Also:
	BDSP_AudioProcessing.
**********************************************************************/ 

typedef enum BDSP_AudioEncode
{
	BDSP_AudioEncode_eMpeg1Layer3,			/* MPEG1 Layer 3 */
	BDSP_AudioEncode_eMpeg1Layer2,			/* MPEG1 Layer 2 */
	BDSP_AudioEncode_eDTS,					/* DTS */
 	BDSP_AudioEncode_eAacLc,					/* AAC-LC */
 	BDSP_AudioEncode_eAacHe,					/* AAC-HE */
	BDSP_AudioEncode_eAc3,					/* AC3 */
	BDSP_AudioEncode_eSbc,					/* SBC Encoder*/
	BDSP_AudioEncode_eG711G726,				/* G.711/G.726 Encoder*/
	BDSP_AudioEncode_eG729,					/* G.729 Encoder*/
	BDSP_AudioEncode_eWma,					/* WMA Encoder */
    BDSP_AudioEncode_eG723_1,               /* G.723.1 Encoder*/
    BDSP_AudioEncode_eG722,					/* G.722 Encoder*/
    BDSP_AudioEncode_eAmr,					/* Amr Encoder*/    
        BDSP_AudioEncode_eAmrwb,					/* Amrwb Encoder*/    
	BDSP_AudioEncode_eiLBC,					/* iLBC Encoder*/
	BDSP_AudioEncode_eiSAC,					/* iSAC Encoder*/
 	BDSP_AudioEncode_eMax,			        /* Max value */
   	BDSP_AudioEncode_eInvalid	= 0x7FFFFFFF          
} BDSP_AudioEncode;

/***************************************************************************
Summary:
    This enumeration defines various encode modes.

***************************************************************************/
typedef enum BDSP_EncodeMode
{
    BDSP_EncodeMode_eRealtime,       /* Real-time encode from Line-In/Mic-In */
    BDSP_EncodeMode_eNonRealtime,    /* Non Real-time encode from CD etc. */
    BDSP_EncodeMode_eSimulMode,  /* Simultaneous mode: It comprises 
                                                           of one real-time and one non real-time 
                                                           encode operation. */
    BDSP_EncodeMode_eInvalid = 0xFF
} BDSP_EncodeMode;

/************************************************************************
Summary:
	This enumeration defines various video encoders supported
	
************************************************************************/
typedef enum BDSP_VideoEncode
{
    BDSP_VideoEncode_eH264, 			/* H264 encoder */
    BDSP_VideoEncode_eVP8, 			/* VP8 encoder */		
    BDSP_VideoEncode_eMax,			    /* Max value */
    BDSP_VideoEncode_eInvalid	= 0xFF    
}BDSP_VideoEncode;


#if 0
typedef void *BDSP_CIT_P_FwBranchInfo;
typedef void *BDSP_CIT_P_RdbCfg;
typedef int BDSP_AF_P_TimeBaseType;
typedef void *BDSP_CIT_P_FwStgSrcDstDetails;
#define BDSP_P_MAX_FW_BRANCH_PER_FW_TASK 2
#endif
#endif

