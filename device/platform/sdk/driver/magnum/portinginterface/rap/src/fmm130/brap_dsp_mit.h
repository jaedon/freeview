/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dsp_mit.h $
* $brcm_Revision: Hydra_Software_Devel/22 $
* $brcm_Date: 8/8/08 6:32p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_dsp_mit.h $
* 
* Hydra_Software_Devel/22   8/8/08 6:32p gdata
* PR43971: [7443] Merging DVD family for 7443 and onward chips
* 
* Hydra_Software_Devel/RAP_DVD_Family/1   6/23/08 8:07p gdata
* PR43971: [7440] Creating DVD family for 7443 and onward chips
* 
* Hydra_Software_Devel/21   9/30/07 2:00p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/20   6/12/07 9:59p sushmit
* PR 27646: Updating 7405 PI
* 
* Hydra_Software_Devel/19   7/10/06 3:14p sushmit
* PR18604: SQA Release for 2 Stage MP3 Encoder on 7401.
* 
* Hydra_Software_Devel/18   5/23/06 5:30p sushmit
* PR18604: Updating 3 stage MP3 Encoder FW & merged scheduler.
* 
* Hydra_Software_Devel/17   5/17/06 11:31a sushmit
* PR18604: Updating Encoder PI for 7401.
* 
* Hydra_Software_Devel/16   4/24/06 10:51a sushmit
* PR18604: Updating Encoder MIT.
* 
* Hydra_Software_Devel/15   4/20/06 4:23p sushmit
* PR18604: Fixing Encoder MIT.
* 
* Hydra_Software_Devel/14   4/11/06 6:05p sushmit
* PR18604: Modifying
* #if (( BCHP_CHIP == 7401 )||( BCHP_CHIP == 7400 ))
* With code like
* #ifndef BCHP_7411_VER
* 
* Hydra_Software_Devel/13   4/7/06 2:51p sushmit
* PR18604: Updating Encoder MIT & Watchdog.
* 
* Hydra_Software_Devel/12   3/28/06 12:05p rnayak
* PR20439: Restore Bangalore Raptor files after magnum vob corruption on
* 17th March 2006
* 
* Hydra_Software_Devel/12   3/21/06 6:27p sushmit
* PR18604: Updating review comments.
* 
* Hydra_Software_Devel/11   2/27/06 4:17p sushmit
* PR18604: Fixing 7411 compilation issues.
* 
* Hydra_Software_Devel/10   2/17/06 9:53a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/1   12/14/05 5:10p sushmit
* [PR18604]:Susmit
* Merging Encoder to latest.
* SUSMIT
* 14 DEC 2005
* 
* Hydra_Software_Devel/10   11/29/05 11:37a susmit
* Merging Decoder & Encoder
* 
* Hydra_Software_Devel/9   9/30/05 5:11p nitinb
* PR 16982: Audio Raptor PI shall use BIMG interface to access firmware
* interface
* 
* Hydra_Software_Devel/8   7/28/05 11:21p nitinb
* PR 16092: Merging new MIT code into main branch
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/1   6/23/05 6:58p nitinb
* Removing DDP, AAC and AAC-HE algorithms for audio decoder to work in
* 600K of memory
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/5   6/21/05 10:37p nitinb
* Added new mit support for AC3 and MPEG algorithms
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/4   6/15/05 6:10p nitinb
* Adding DDP and AAC-HE in the list of supported algorithms
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/3   6/14/05 9:04p nitinb
* Making changes for AAC algorithm as required by new MIT
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/2   6/14/05 1:58p nitinb
* PR 15272: Changed name of file brap_ddp_table.c to
* brap_ddp_decode_table.c. Making corresponding changes in code as
* firmware array names are derived from the file name.
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/1   6/13/05 3:00p nitinb
* PR 15272: Made changes in code required for new MIT format
* 
* Hydra_Software_Devel/5   5/13/05 3:28p nitinb
* PR 15271: Merging MPEG changes from multicodec branch to main branch
* 
* Hydra_Software_Devel/4   4/14/05 11:48a nitinb
* PR 14507: Merging changes from multicodec branch to main branch
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_MultiCodec/2   4/13/05 10:49a nitinb
* PR 14507: Added support for MPEG decode
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_MultiCodec/1   3/18/05 8:30p nitinb
* PR 13082: Added AC3 pass thru variables
* 
* Hydra_Software_Devel/3   3/16/05 3:13p nitinb
* PR 13082: Correcting AAC delay table size macro
* 
* Hydra_Software_Devel/2   1/24/05 3:59p nitinb
* PR 13082: Added AAC donwmix and TSM related functions
* 
* Hydra_Software_Devel/2   10/29/04 1:13p nitinb
* PR 13082: Adding DSP initialization code
* 
* Hydra_Software_Devel/1   10/26/04 8:22a nitinb
* PR13082: Initial Version
* 
***************************************************************************/

#ifndef BRAP_DSP_MIT_H__
#define BRAP_DSP_MIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "brap_img.h"

#define NOT_ALLOTTED_BUFPTR				0xFFFFFFFF
#define NOT_ALLOTTED_BUFSIZE			0x0

#define BRAP_DSP_P_AAC_TABLE_DELAY_SIZE		(512 * 4)	
#define BRAP_DSP_P_AC3_TABLE_DELAY_SIZE		3072
#define BRAP_DSP_P_MPEG_TABLE_DELAY_SIZE	10880

#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Adding MP3 & DTS Encoder */
/*#define BAF_ALGORITHM_MPEG1_L3_ENCODER_STAGES 5*/
/*#define BAF_ALGORITHM_MPEG1_L3_ENCODER_STAGES 3*/
#define BAF_ALGORITHM_MPEG1_L3_ENCODER_STAGES 2
#if (( BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
#define BAF_ALGORITHM_DTS_ENCODER_STAGES	1
#endif

const uint32_t BRAP_DSP_P_supportedEncode[] = {
	BAF_ALGORITHM_MPEG1_L3_ENCODER
#if (( BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
	,BAF_ALGORITHM_DTS_ENCODER
#endif
};

const uint32_t BRAP_DSP_P_numEnc = sizeof(BRAP_DSP_P_supportedEncode)/sizeof(uint32_t);

const uint32_t BRAP_DSP_P_EncodeStages[] = {
	BAF_ALGORITHM_MPEG1_L3_ENCODER_STAGES
#if (( BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
	,BAF_ALGORITHM_DTS_ENCODER_STAGES
#endif
};

const uint32_t BRAP_DSP_P_EncodeAlgoIDs[] = {
	BRAP_IMG_MP3_ENCODE_STG0_ID
	,BRAP_IMG_MP3_ENCODE_STG1_ID
/*	,BRAP_IMG_MP3_ENCODE_STG2_ID*/
/*	,BRAP_IMG_MP3_ENCODE_STG3_ID
	,BRAP_IMG_MP3_ENCODE_STG4_ID*/
#if (( BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
	,BRAP_IMG_DTS_ENCODE_CODE_ID
#endif
};

const uint32_t BRAP_DSP_P_EncodeTblIDs[] = {
	BRAP_IMG_MP3_ENCODE_TABLE_ID
#if (( BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
	,BRAP_IMG_DTS_ENCODE_TABLE_ID
#endif
};

const uint32_t BRAP_DSP_P_EncodeScratchIDs[] = {
	BRAP_IMG_MP3_ENCODE_SCRATCH_ID
#if (( BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
	,BRAP_IMG_DTS_ENCODE_SCRATCH_ID
#endif
};

const uint32_t BRAP_DSP_P_EncodeIFrameIDs[] = {
	BRAP_IMG_MP3_ENCODE_INTER_FRAME_ID
#if (( BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
	,BRAP_IMG_DTS_ENCODE_INTER_FRAME_ID
#endif
};

const uint32_t BRAP_DSP_P_EncodeIStageIDs[] = {
	BRAP_IMG_MP3_ENCODE_INTER_STAGE_ID
#if (( BRAP_DVD_FAMILY == 1)||(BRAP_7405_FAMILY == 1))
	,BRAP_IMG_DTS_ENCODE_INTER_STAGE_ID
#endif
};

/* Re-enable these once needed
const uint32_t BRAP_DSP_P_supportedEncodePreP[] = {

};

const uint32_t BRAP_DSP_P_numEncPreP = sizeof(BRAP_DSP_P_supportedEncodePreP)/sizeof(uint32_t);

const uint32_t BRAP_DSP_P_supportedEncodePostP[] = {

};

const uint32_t BRAP_DSP_P_numEncPostP = sizeof(BRAP_DSP_P_supportedEncodePostP)/sizeof(uint32_t);
*/
#endif

const uint32_t BRAP_DSP_P_supportedFS[] = {
	BAF_FRAME_SYNC_MPEG,
	BAF_FRAME_SYNC_AC3
};

const uint32_t BRAP_DSP_P_numFS = sizeof(BRAP_DSP_P_supportedFS)/sizeof(uint32_t);

const uint32_t BRAP_DSP_P_supportedDecode[] = {
	BAF_ALGORITHM_MPEG_L1,
	BAF_ALGORITHM_MPEG_L2,
	BAF_ALGORITHM_MP3,
	BAF_ALGORITHM_AC3,
};

const uint32_t BRAP_DSP_P_numDec = sizeof(BRAP_DSP_P_supportedDecode)/sizeof(uint32_t);

const uint32_t BRAP_DSP_P_supportedPP[] = {
	BAF_PP_DOWNMIX_AC3,
};

const uint32_t BRAP_DSP_P_numPP = sizeof(BRAP_DSP_P_supportedPP)/sizeof(uint32_t);

const uint32_t BRAP_DSP_P_supportedPT[] = {
	BAF_PASS_THRU_AC3,
	BAF_PASS_THRU_MPEG
};

const uint32_t BRAP_DSP_P_numPT = sizeof(BRAP_DSP_P_supportedPT)/sizeof(uint32_t);

#ifdef __cplusplus
}
#endif

#endif /* BRAP_DSP_MIT_H__ */

/* End of File */
