/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brap_img.h $
 * $brcm_Revision: Hydra_Software_Devel/75 $
 * $brcm_Date: 10/5/10 8:49a $
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/rap/base/brap_img.h $
 * 
 * Hydra_Software_Devel/75   10/5/10 8:49a srajapur
 * SW7400-2918 : [7400] AAC interframe array size is
 * 64K.BRAP_IMG_CHUNK_SIZE  increased to 64K .
 * 
 * Hydra_Software_Devel/74   5/10/10 5:38p srajapur
 * SW7400-2688 : [7400] Added Support for BRAP_DSPCHN_AudioType_ePcmWav
 * algorithm
 * 
 * Hydra_Software_Devel/73   7/29/09 5:29p srajapur
 * PR53240 : [7403] DRA decoder development on 7403 chip
 * 
 * Hydra_Software_Devel/72   7/21/09 6:18p srajapur
 * PR53240 : [7403] DRA decoder development on 7403 chip
 * 
 * Hydra_Software_Devel/71   6/12/09 6:03p srajapur
 * PR 53489:[7401,7400] Added support for SRS True Volume certification
 * and added PCM decode algorithm
 * 
 * Hydra_Software_Devel/70   3/18/09 8:38p srajapur
 * PR53363 : [7400] Adding DTS passthrough support
 * 
 * Hydra_Software_Devel/69   2/27/09 7:34p srajapur
 * PR42285 : [7401] Add audio descriptor support ---> Added RAP_AD_SUPPORT
 * compile time option.
 * 
 * Hydra_Software_Devel/68   2/25/09 6:18p srajapur
 * PR52480: [7400] Name changing SRS VIQ to SRS TrueVolume
 * 
 * Hydra_Software_Devel/67   1/12/09 11:06p srajapur
 * PR50613 :[7400] Added SRS VIQ post processing algorithm for 7400
 * 
 * Hydra_Software_Devel/66   11/12/08 7:30p srajapur
 * PR48893 : 74XX merged release for 7400,7401,7403 and 7118 [For more
 * info see Description]
 * PR45669: [7403] Adding support for DTS Certification.
 * PR43467 : [7403] Implementation of DRA on 7403
 * PR46472 : [7400] Implement Dolby Volume Leveler and Modeler per
 * Motorola's Requirements
 * PR42285 : [7401] Added Audio descriptor support
 * PR44899 :[7401] Independent delay on spdif causes left and right
 * channel to be out of phase
 * PR44614 : [7400] [7401C1][NDS-CDI] Few seconds of noise on SPDIF/HDMI
 * during PCM to Compressed & vice-a-versa transition
 * PR42342: [7401] Added Downmix Method parameter in the AACHE user config
 * structure for Sumitomo
 * 
 * Hydra_Software_Devel/65   10/3/08 2:25p bhanus
 * PR 45762:[7440]Adding Multiple tables for AAC HE stages.
 * 
 * Hydra_Software_Devel/64   9/16/08 3:17p bhanus
 * PR 46417 : [7440] Merging DTS MA Support on main line
 * 
 * Hydra_Software_Devel/RAP_7440_DTSMA_DEVEL/2   9/12/08 7:03p bhanus
 * PR 46417 : [7440] Adding support for Downloadable Scheduler.
 * 
 * Hydra_Software_Devel/RAP_7440_DTSMA_DEVEL/1   9/2/08 11:54a bhanus
 * PR 46417 : [7440] Adding DTS MA Support
 * 
 * Hydra_Software_Devel/63   8/18/08 4:01p bhanus
 * PR 45762:[7440] Adding Support for AAC HE for 7440.
 * 
 * Hydra_Software_Devel/62   5/16/08 3:49p srajapur
 * PR42131 : [7401,7403] Added support for AACHEV2
 * 
 * Hydra_Software_Devel/61   5/16/08 12:13p gdata
 * PR42762: [7440] Adding support for DTSHD-NBC algo
 * 
 * Hydra_Software_Devel/60   4/28/08 2:30p srajapur
 * PR42131 : [7401] Added support for AACHEV2
 * 
 * Hydra_Software_Devel/59   4/8/08 11:34a bhanus
 * PR 41469 : [7440] Adding Support for WMA and WMA-Pro for 7440
 * 
 * Hydra_Software_Devel/58   1/17/08 6:11p gdata
 * PR38790:  [7440] Adding support for LPCM_A_DVD
 * 
 * Hydra_Software_Devel/57   12/20/07 11:32a gdata
 * PR38296: [7440] Adding support for DTSHD SUB audio
 * 
 * Hydra_Software_Devel/56   11/20/07 8:45p sushmit
 * PR 36877: [7335] Bringup PI
 * 
 * Hydra_Software_Devel/55   10/1/07 10:29p sushmit
 * PR35140: [7325] Develop 7325 Raptor PI for Bringup
 * 
 * Hydra_Software_Devel/54   9/19/07 3:28p speter
 * PR 35074: [3563] Adding Image IDs defines for PEQ post processing
 * algorithm
 * 
 * Hydra_Software_Devel/53   9/7/07 3:56p speter
 * PR 34673: [3563] Merging the Phase 2.6 branch to mainline
 * 
 * Hydra_Software_Devel/52   9/5/07 3:16p gdata
 * PR 32827: [7440] Merging DTS-LBR 3-Stage support to
 * Hydra_Software_Devel branch
 * 
 * Hydra_Software_Devel/RAP_7440_DTSLBR_3Stages/1   9/4/07 4:44p gdata
 * PR 32827: [7440] Adding support for 3 stage DTS-LBR
 * 
 * Hydra_Software_Devel/RAP_3563_PH_2_6/1   9/6/07 1:07p speter
 * PR 34460: [3563] Changing code to use 'Custom' for specific Post
 * procesing algorithms
 * 
 * Hydra_Software_Devel/51   8/27/07 3:00p speter
 * PR 34376: [3563] Adding Firmware image IDs for the Custom Voice post
 * processing support
 * 
 * Hydra_Software_Devel/50   8/21/07 1:57p speter
 * PR 34202: [3563] Adding defines for new Custom Bass post processing Image
 * IDs
 * 
 * Hydra_Software_Devel/49   7/30/07 5:50p speter
 * PR 33608: [3563] Adding Image IDs of firmware files for post processing
 * algorithm Custom surround
 * 
 * Hydra_Software_Devel/48   7/27/07 3:47p kagrawal
 * PR 33135: [7440] Swapping the DTS-LBR decode image file ids
 * 
 * Hydra_Software_Devel/47   7/18/07 6:25p gautamk
 * PR33094 : [7440] Adding Support for Passthru mode For Algo DDP7.1, MLp
 * and DTSHD (HBR Mode)
 * 
 * Hydra_Software_Devel/46   7/12/07 10:32a gautamk
 * PR32256: [7401] Adding support for passthru in WMA PRo
 * 
 * Hydra_Software_Devel/45   7/9/07 4:17p gautamk
 * PR32827 :  Adding PI Support for DTS LBR Multichannel
 * 
 * Hydra_Software_Devel/44   7/5/07 4:04p gautamk
 * PR32256: [7401] Adding new algo WMA PRO for 7401
 * 
 * Hydra_Software_Devel/RAP_7401_WMA_PRO/1   6/18/07 12:25p gautamk
 * PR32256: [7401] Adding new algo WMA PRO for 7401
 * 
 * Hydra_Software_Devel/43   6/15/07 8:06p gautamk
 * PR30069: [7440] Added DTS-Extension Support.
 * 
 * Hydra_Software_Devel/42   6/15/07 2:17p kagrawal
 * PR 30069: [7440] Merging Phase2.0 branch to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/41   6/14/07 7:27p gautamk
 * PR30069: Changes for new algorithm Mpeg MC
 * 
 * Hydra_Software_Devel/40   6/14/07 4:52p gautamk
 * PR30069: [7440] Adding algorithm DDP 7.1
 * Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/1   6/9/07 5:42p kagrawal
 * PR 30069: [7440] Added support for DTS-Neo PostProcessing
 * 
 * Hydra_Software_Devel/39   6/1/07 3:45p bselva
 * PR 26501:[7401 Family] Second round of changes for aache multichannel
 * 
 * Hydra_Software_Devel/38   5/31/07 6:05p bselva
 * PR 26501: [7401 Family]Checking in the changes for AAC-HE multichannel
 * support
 * 
 * Hydra_Software_Devel/37   5/15/07 4:07p rjain
 * PR 28422:[7401 family] Checking in PI changes for FSPB and DSOLA [needs
 * new 7401 MIT]
 * 
 * Hydra_Software_Devel/35   5/7/07 4:36p nitinb
 * PR 28862: Added BBE images
 * 
 * Hydra_Software_Devel/34   4/4/07 1:34a nitinb
 * PR 28865: Added support for post processing algorithm XEN
 * 
 * Hydra_Software_Devel/33   3/30/07 1:01p kagrawal
 * PR 28561: [7440] Added support for MLP interframe image file
 * 
 * Hydra_Software_Devel/32   3/27/07 3:28p kagrawal
 * PR 28561: [7440] Adding support for DTS-LBR
 * 
 * Hydra_Software_Devel/31   3/3/07 7:59p kagrawal
 * PR 27912: [7440] Added support for DDBM interframe and separate
 * scheduler for DSP1
 * 
 * Hydra_Software_Devel/30   2/27/07 6:25p nitinb
 * PR 26864: Added 3563 firmware files for SRS, PL-II and AVL
 * 
 * Hydra_Software_Devel/29   2/8/07 4:28p rjain
 * PR 24247: adding support for SRc as post processing module
 * 
 * Hydra_Software_Devel/28   1/29/07 3:25p bhanus
 * PR 25607: Added Support for Multi-executable DTS/DTS-HD
 * 
 * Hydra_Software_Devel/27   1/22/07 7:25p sushmit
 * PR 26864:
 * Added PES Framesync & LPCM_Custom algos.
 * 
 * Hydra_Software_Devel/26   12/29/06 1:10a nitinb
 * PR 26766: Added BTSC firmware files
 * 
 * Hydra_Software_Devel/25   12/7/06 1:26p sushmit
 * PR 25346: Merging 3563 to Hydra
 * 
 * Hydra_Software_Devel/RAP_3563_Merge/1   12/4/06 11:11a sushmit
 * PR 24037: Merging BTSC changes
 * 
 * Hydra_Software_Devel/RAP_3563_API_STUBS/1   12/3/06 10:59p nitinb
 * PR 25346: Added BTSC code
 * 
 * Hydra_Software_Devel/24   10/3/06 11:08a bselva
 * PR 24468: Checking in the changes required for 7400 phase 3.0 release.
 * 
 * Hydra_Software_Devel/23   9/25/06 3:12p kagrawal
 * PR 20654: Merged from 7440 bring up branch
 * 
 * Hydra_Software_Devel/22   7/3/06 8:15p nitinb
 * PR 22443: Replace AC3 decoder with DDP decoder in AC3 Lossless codec
 * 
 * Hydra_Software_Devel/21   6/30/06 12:43p kagrawal
 * PR 22083: Added support for HD-DVD MLP for 7411D0
 * 
 * Hydra_Software_Devel/20   6/15/06 2:46p nitinb
 * PR 22083: Add support for MLP algorithm in 7411D0 RAP PI
 * 
 * Hydra_Software_Devel/19   6/13/06 2:53p nitinb
 * PR 22083: Add support for MLP algorithm in 7411D0 RAP PI
 * 
 * Hydra_Software_Devel/18   5/24/06 6:25p nitinb
 * PR 21706: Download interframe buffer files for all LPCM algorithms
 * 
 * Hydra_Software_Devel/17   5/24/06 4:13p nitinb
 * PR 21705: Require RAP PI support to download framesync executable
 * depending on DVD audio type
 * 
 * Hydra_Software_Devel/16   5/4/06 11:35a nitinb
 * PR 21400: Add RAP support for DD in lossless for 7411D0
 * 
 * Hydra_Software_Devel/15   5/3/06 11:07p nitinb
 * PR 21105: 7411D0 - Add support WMA Raptor Porting Interface
 * 
 * Hydra_Software_Devel/14   3/28/06 12:10p rnayak
 * PR20439: Restore Bangalore Raptor files after magnum vob corruption on
 * 17th March 2006
 * 
 * Hydra_Software_Devel/15   3/21/06 6:28p sushmit
 * PR18604: Updating review comments.
 * 
 * Hydra_Software_Devel/14   3/20/06 8:10p nitinb
 * PR 20287: Added algorithm DVD LPCM
 * 
 * Hydra_Software_Devel/13   3/16/06 5:22p nitinb
 * PR 20117: Implement dolby digital bass management algorithm
 * 
 * Hydra_Software_Devel/12   2/17/06 10:00a sushmit
 * PR18604: Merging Encoder to latest.
 * 
 * Hydra_Software_Devel/11   2/8/06 5:27p kagrawal
 * PR 19194: Added support for DTS-HD
 * 
 * Hydra_Software_Devel/RAP_AUD_ENC/2   2/6/06 4:22p sushmit
 * PR18604: Merging latest Decoder PI to Encoder PI branch
 * 
 * Hydra_Software_Devel/RAP_AUD_ENC/1   12/14/05 5:10p sushmit
 * [PR18604]:Susmit
 * Merging Encoder to latest.
 * SUSMIT
 * 14 DEC 2005
 * 
 * Hydra_Software_Devel/9   12/01/05 04:55p susmit
 * Added Image IDs for Encoder
 * 
 * Hydra_Software_Devel/8   12/2/05 1:17p bselva
 * PR 18312: Added support for LPCM
 * 
 * Hydra_Software_Devel/7   11/29/05 6:03p bselva
 * PR 18312: Added pass through support for DTS
 * 
 * Hydra_Software_Devel/6   11/29/05 4:58p bselva
 * PR 18312: Added for DTS support
 * 
 * Hydra_Software_Devel/5   11/3/05 3:24p rjain
 * PR 16982: Merging Vladimir's image interface related changes to main
 * branch
 * 
 * Hydra_Software_Devel/4   10/23/05 9:58a nitinb
 * PR 17415: Added support for AAC downmix
 * 
 * Hydra_Software_Devel/3   10/6/05 12:01p nitinb
 * PR 17415 : Adding support for AAC passthru
 * 
 * Hydra_Software_Devel/2   10/2/05 10:10p nitinb
 * PR 17415: Add support for advance codec for Raptor PI
 * 
 * Hydra_Software_Devel/1   9/30/05 6:09p nitinb
 * PR 16982: Audio Raptor PI shall use BIMG interface to access firmware
 * interface
 ***************************************************************/
#ifndef BRAP_IMG_H__
#define BRAP_IMG_H__

#include "bimg.h"

#if((BCHP_CHIP == 7405)||(BCHP_CHIP == 7325) || (BCHP_CHIP == 7335))
#define BRAP_7405_FAMILY    1
#endif

#if ((BCHP_CHIP == 7440)||(BCHP_CHIP == 7601))
#define BRAP_DVD_FAMILY     1
#endif

#if (BCHP_CHIP == 7403)
#define BRAP_DTS_SUPPORTED    1
#endif

#if (BCHP_CHIP == 7400)
#define BRAP_DTS_PASSTHRU_SUPPORTED    1
#endif

/* 
	Firmware Image IDs that are to be used in the BRAP_IMG_Open function of the BRAP_IMG_Interface
*/
#define BRAP_IMG_IBOOT_ID 0

#define BRAP_IMG_SCHEDULER_CODE_ID 1
#define BRAP_IMG_SCHEDULER_DATA_ID 2

#define BRAP_IMG_MPEG_FRAMESYNC_ID 3
#define BRAP_IMG_AC3_FRAMESYNC_ID 4

#define BRAP_IMG_MPEG_L1_DECODE_ID 5
#define BRAP_IMG_MPEG_L2_DECODE_ID 6
#define BRAP_IMG_MP3_DECODE_ID 7
#define BRAP_IMG_AC3_DECODE_ID 8

#define BRAP_IMG_MPEG_DECODE_TABLE_ID 9
#define BRAP_IMG_AC3_DECODE_TABLE_ID 10

#define BRAP_IMG_AC3_DOWNMIX_CODE_ID 11
#define BRAP_IMG_AC3_DOWNMIX_TABLE_ID 12

#define BRAP_IMG_AC3_PASSTHRU_ID 13
#define BRAP_IMG_MPEG_PASSTHRU_ID 14

#define BRAP_IMG_MPEG_INTERFRAME_ID 15

#define BRAP_IMG_AAC_FRAMESYNC_ID 16
#define BRAP_IMG_AAC_DECODE_ID 17
#define BRAP_IMG_AAC_DECODE_TABLE_ID 18
#define BRAP_IMG_AAC_INTERFRAME_ID 19
#define BRAP_IMG_AAC_PASSTHRU_ID 20
#define BRAP_IMG_AAC_DOWNMIX_ID 21

#define BRAP_IMG_AACHE_FRAMESYNC_ID 22
#define BRAP_IMG_AACHE_DECODE_ID 23
#define BRAP_IMG_AACHE_DECODE_TABLE_ID 24
#define BRAP_IMG_AACHE_INTERFRAME_ID 25

#define BRAP_IMG_DDP_FRAMESYNC_ID 26
#define BRAP_IMG_DDP_BE_DECODE_ID 27
#define BRAP_IMG_DDP_FE_DECODE_ID 28
#define BRAP_IMG_DDP_DECODE_TABLE_ID 29
#define BRAP_IMG_DDP_INTERFRAME_ID 30

#define BRAP_IMG_DDP_PASSTHRU_ID 31
#define BRAP_IMG_DDP_PASSTHRU_INTERFRAME_ID 32
#define BRAP_IMG_DDP_PASSTHRU_TABLE_ID 33

#define BRAP_IMG_DTS_FRAMESYNC_ID 34
#define BRAP_IMG_DTS_DECODE_ID 35
#define BRAP_IMG_DTS_DECODE_TABLE_ID 36
#define BRAP_IMG_DTS_INTERFRAME_ID 37
#define BRAP_IMG_DTS_PASSTHRU_ID 38

#define BRAP_IMG_BD_LPCM_FRAMESYNC_ID 39
#define BRAP_IMG_BD_LPCM_DECODE_ID 40

/* Merging Decoder & Encoder */
#define BRAP_IMG_ENC_IBOOT_ID 41

#define BRAP_IMG_ENC_SCHEDULER_CODE_ID 42
#define BRAP_IMG_ENC_SCHEDULER_DATA_ID 43

#define BRAP_IMG_MP3_ENCODE_TABLE_ID 44
#define BRAP_IMG_MP3_ENCODE_INTER_FRAME_ID 45
#define BRAP_IMG_MP3_ENCODE_INTER_STAGE_ID 46
#define BRAP_IMG_MP3_ENCODE_SCRATCH_ID 47
#define BRAP_IMG_MP3_ENCODE_STG0_ID 48
#define BRAP_IMG_MP3_ENCODE_STG1_ID 49
#define BRAP_IMG_MP3_ENCODE_STG2_ID 50
#define BRAP_IMG_MP3_ENCODE_STG3_ID 51
#define BRAP_IMG_MP3_ENCODE_STG4_ID 52


#define BRAP_IMG_LPCM_HDDVD_FRAMESYNC_ID 53 
#define BRAP_IMG_LPCM_HDDVD_DECODE_ID 54 


/* Note: Except for the frame-sync, all other fw is common between DTS and DTS-HD */
#define BRAP_IMG_DTSHD_FRAMESYNC_ID 55
#define BRAP_IMG_DTSHD_DECODE_ID BRAP_IMG_DTS_DECODE_ID
#define BRAP_IMG_DTSHD_DECODE_TABLE_ID BRAP_IMG_DTS_DECODE_TABLE_ID
#define BRAP_IMG_DTSHD_INTERFRAME_ID BRAP_IMG_DTS_INTERFRAME_ID
#define BRAP_IMG_DTSHD_PASSTHRU_ID BRAP_IMG_DTS_PASSTHRU_ID
/*#define BRAP_IMG_LPCM_HDDVD_FRAMESYNC_ID 56
#define BRAP_IMG_LPCM_HDDVD_DECODE_ID 57*/

#define BRAP_IMG_DDBM_ID 56
#define BRAP_IMG_DDBM_TABLE_ID 57

#define BRAP_IMG_LPCM_DVD_FRAMESYNC_ID 58 
#define BRAP_IMG_LPCM_DVD_DECODE_ID 59 

#if (BRAP_DVD_FAMILY == 1)
#define BRAP_IMG_WMA_STD_FRAMESYNC_ID 60
#define BRAP_IMG_WMA_STD_STG0_DECODE_ID 61
#define BRAP_IMG_WMA_STD_STG0_DECODE_TABLE_ID 62
#define BRAP_IMG_WMA_STD_INTERFRAME_ID 63


#else
#define BRAP_IMG_WMA_STD_FRAMESYNC_ID 60
#define BRAP_IMG_WMA_STD_STG1_DECODE_ID 61
#define BRAP_IMG_WMA_STD_STG1_DECODE_TABLE_ID 62
#define BRAP_IMG_WMA_STD_INTERFRAME_ID 63
#endif

/* Note: Except for the frame-sync, all other fw is common between AC3 Lossless and AC3 */
#define BRAP_IMG_AC3LOSSLESS_FRAMESYNC_ID 64
#define BRAP_IMG_AC3LOSSLESS_BE_DECODE_ID BRAP_IMG_DDP_BE_DECODE_ID
#define BRAP_IMG_AC3LOSSLESS_FE_DECODE_ID BRAP_IMG_DDP_FE_DECODE_ID
#define BRAP_IMG_AC3LOSSLESS_DECODE_TABLE_ID BRAP_IMG_DDP_DECODE_TABLE_ID
#define BRAP_IMG_AC3LOSSLESS_INTERFRAME_ID BRAP_IMG_DDP_INTERFRAME_ID

#define BRAP_IMG_AC3LOSSLESS_PASSTHRU_ID BRAP_IMG_DDP_PASSTHRU_ID
#define BRAP_IMG_AC3LOSSLESS_PASSTHRU_INTERFRAME_ID BRAP_IMG_DDP_PASSTHRU_INTERFRAME_ID
#define BRAP_IMG_AC3LOSSLESS_PASSTHRU_TABLE_ID BRAP_IMG_DDP_PASSTHRU_TABLE_ID

#define BRAP_IMG_DTSHD_HDDVD_FRAMESYNC_ID 65

#define BRAP_IMG_BD_LPCM_INTERFRAME_ID 66
#define BRAP_IMG_LPCM_DVD_INTERFRAME_ID 67
#define BRAP_IMG_LPCM_HDDVD_INTERFRAME_ID 68

#define BRAP_IMG_MLP_DECODE_ID 69
#define BRAP_IMG_MLP_DECODE_TABLE_ID 70
#define BRAP_IMG_MLP_FRAMESYNC_ID 71
#define BRAP_IMG_MLP_HDDVD_FRAMESYNC_ID 72

#define BRAP_IMG_DTS_ENCODE_CODE_ID 73
#define BRAP_IMG_DTS_ENCODE_TABLE_ID 74
#define BRAP_IMG_DTS_ENCODE_INTER_FRAME_ID 75
#define BRAP_IMG_DTS_ENCODE_INTER_STAGE_ID 76
#define BRAP_IMG_DTS_ENCODE_SCRATCH_ID 77

#define BRAP_IMG_DDP_CONVERT_ID 78

#define BRAP_IMG_BTSC_US_ID 79
#define BRAP_IMG_BTSC_JAPAN_ID 80
#define BRAP_IMG_BTSC_KOREA_ID 81

#define BRAP_IMG_PES_FRAMESYNC_ID   82
#define BRAP_IMG_LPCM_CUSTOM_CODE_ID    83

/* DTS and DTS-HD extensions */
#define BRAP_IMG_DTS_1_DECODE_ID 84
#define BRAP_IMG_DTSHD_1_DECODE_ID BRAP_IMG_DTS_1_DECODE_ID


#define BRAP_IMG_SRC_ID 85
#define BRAP_IMG_SRC_TABLE_ID 86
#define BRAP_IMG_SRC_INTERFRAME_ID 87

#define BRAP_IMG_PL2_ID 88
#define BRAP_IMG_PL2_TABLE_ID 89
#define BRAP_IMG_PL2_INTERFRAME_ID 90
#define BRAP_IMG_SRSXT_ID 91
#define BRAP_IMG_SRSXT_TABLE_ID 92
#define BRAP_IMG_SRSXT_INTERFRAME_ID 93
#define BRAP_IMG_AVL_ID 94
#define BRAP_IMG_AVL_TABLE_ID 95
#define BRAP_IMG_AVL_INTERFRAME_ID 96

#define BRAP_IMG_DDBM_INTERFRAME_ID 97

/* DSP1 scheduler */
#define BRAP_IMG_SCHEDULER_DSP1_CODE_ID 98
#define BRAP_IMG_SCHEDULER_DSP1_DATA_ID 99

/* DTS-LBR to be contd later.. */
#define BRAP_IMG_DTSLBR_FRAMESYNC_ID    	100
#define BRAP_IMG_DTSLBR_0_DECODE_ID     	101
#define BRAP_IMG_DTSLBR_DECODE_TABLE_ID 	102
#define BRAP_IMG_DTSLBR_INTERFRAME_ID   	103

/* MLP interframe */
#define BRAP_IMG_MLP_INTERFRAME_ID      104

/* XEN post processing algorithm */
#define BRAP_IMG_XEN_ID	105
#define BRAP_IMG_XEN_TABLE_ID		106
#define BRAP_IMG_XEN_INTERFRAME_ID	107

/* BBE post processing algorithm */
#define BRAP_IMG_BBE_ID	108
#define BRAP_IMG_BBE_TABLE_ID	109
#define BRAP_IMG_BBE_INTERFRAME_ID	110

/* For DSOLA post processing */
#define BRAP_IMG_DSOLA_ID 111
#define BRAP_IMG_DSOLA_TABLE_ID 112
#define BRAP_IMG_DSOLA_INTERFRAME_ID 113

/* AAC HE Low power SBR */
#define BRAP_IMG_AACHE_LP_DECODE_STG0_ID	114
#define BRAP_IMG_AACHE_LP_DECODE_STG1_ID	115
#define BRAP_IMG_AACHE_LP_DECODE_STG2_ID	116
#define BRAP_IMG_AACHE_LP_INTERFRAME_ID		117
#define BRAP_IMG_AACHE_LP_TABLE_STG0_ID		118
#define BRAP_IMG_AACHE_LP_TABLE_STG1_ID		119

/* AAC HE pass through */
#define BRAP_IMG_AACHE_PASSTHRU_ID	120

#define BRAP_IMG_DDP_7_1_FRAMESYNC_ID 121
#define BRAP_IMG_DDP_7_1_BE_DECODE_ID 122
#define BRAP_IMG_DDP_7_1_FE_DECODE_ID 123
#define BRAP_IMG_DDP_7_1_DECODE_TABLE_ID 124
#define BRAP_IMG_DDP_7_1_INTERFRAME_ID 125

#define BRAP_IMG_MPEG_MC_FRAMESYNC_ID 126
#define BRAP_IMG_MPEG_MC_DECODE_ID 127
#define BRAP_IMG_MPEG_MC_DECODE_TABLE_ID 128
#define BRAP_IMG_MPEG_MC_PASSTHRU_ID 129

/* For DTS-NEO post processing */
#define BRAP_IMG_DTS_NEO_ID             130
#define BRAP_IMG_DTS_NEO_TABLE_ID       131
#define BRAP_IMG_DTS_NEO_INTERFRAME_ID  132

#define BRAP_IMG_DTSHD_2_DECODE_ID 133
#define BRAP_IMG_DTSHD_3_DECODE_ID 134
#define BRAP_IMG_DTSHD_4_DECODE_ID 135


#define BRAP_IMG_WMA_PRO_FRAMESYNC_ID 136
#define BRAP_IMG_WMA_PRO_DECODE_STG0_ID 137
#define BRAP_IMG_WMA_PRO_DECODE_STG1_ID 138
#define BRAP_IMG_WMA_PRO_DECODE_TABLE_ID 139
#define BRAP_IMG_WMA_PRO_PASSTHRU_ID 140

/* DTS-LBR contd */
#define BRAP_IMG_DTSLBR_EXSS_DECODE_ID       141

#define BRAP_IMG_DDP_7_1_PASSTHRU_ID   142
#define BRAP_IMG_MLP_PASSTHRU_ID   	   143	
#define BRAP_IMG_DTSHD_HBR_PASSTHRU_ID 144

/* Custom Surround post processing algorithm */
#define BRAP_IMG_CUSTOM_SURROUND_ID	145
#define BRAP_IMG_CUSTOM_SURROUND_TABLE_ID	146
#define BRAP_IMG_CUSTOM_SURROUND_INTERFRAME_ID	147

/* Custom Bass post processing algorithm */
#define BRAP_IMG_CUSTOM_BASS_ID	148
#define BRAP_IMG_CUSTOM_BASS_TABLE_ID	149
#define BRAP_IMG_CUSTOM_BASS_INTERFRAME_ID	150

/* Custom Voice post processing algorithm */
#define BRAP_IMG_CUSTOM_VOICE_ID	151
#define BRAP_IMG_CUSTOM_VOICE_TABLE_ID	152
#define BRAP_IMG_CUSTOM_VOICE_INTERFRAME_ID	153

/* DTS-LBR contd */
#define BRAP_IMG_DTSLBR_1_DECODE_ID     154

/* Peq post processing algorithm */
#define BRAP_IMG_PEQ_ID	155
#define BRAP_IMG_PEQ_TABLE_ID	156
#define BRAP_IMG_PEQ_INTERFRAME_ID	157


/*DTSHD Sub Audio algorithm*/
#define BRAP_IMG_DTSHD_SUB_TABLE_ID	158
#define BRAP_IMG_DTSHD_SUB_INTERFRAME_ID	159
#define BRAP_IMG_DTSHD_SUB_FRAMESYNC_ID	160
#define BRAP_IMG_DTSHD_SUB_0_DECODE_ID     	161
#define BRAP_IMG_DTSHD_SUB_1_DECODE_ID     	162

/*DVD_A_LPCm Algorithm */
#define BRAP_IMG_DVD_A_LPCM_INTERFRAME_ID	163
#define BRAP_IMG_DVD_A_LPCM_FRAMESYNC_ID	164
#define BRAP_IMG_DVD_A_LPCM_DECODE_ID     	165

#define BRAP_IMG_DTSHD_NBC_CORE_FRAMESYNC_ID    166

/* AAC V2 Executables */
#define BRAP_IMG_AACHE_DECODE_STG0_ID	167
#define BRAP_IMG_AACHE_DECODE_STG1_ID	168
#define BRAP_IMG_AACHE_DECODE_STG2_ID	169
#define BRAP_IMG_AACHE_DECODE_STG3_ID	170
#define BRAP_IMG_AACHE_DECODE_STG4_ID	171

#define BRAP_IMG_AACHE_TABLE_STG0_ID		172
#define BRAP_IMG_AACHE_TABLE_STG1_ID		173
#define BRAP_IMG_AACHE_TABLE_STG2_ID		174
#define BRAP_IMG_AACHE_TABLE_STG3_ID		175
#define BRAP_IMG_AACHE_TABLE_STG4_ID		176

/* DTS HD MA STG1 Frame Sync */
#define BRAP_IMG_DTSHD_STG1_FRAMESYNC_ID        177
#define BRAP_IMG_DTSHD_STG1_DECODE_TABLE_ID     178
#define BRAP_IMG_DTSHD_STG1_INTERFRAME_ID       179

#define BRAP_IMG_SCHEDULER_DNLD_CODE_ID         180
#define BRAP_IMG_SCHEDULER_DSP1_DNLD_CODE_ID    181

#define BRAP_IMG_AACHE_DECODE_TABLE_STG0_ID     182
#define BRAP_IMG_AACHE_DECODE_TABLE_STG1_ID     183
#define BRAP_IMG_AACHE_DECODE_TABLE_STG2_ID     184
#define BRAP_IMG_AACHE_DECODE_TABLE_STG3_ID     185
#define BRAP_IMG_AACHE_DECODE_TABLE_STG4_ID     186


#define BRAP_IMG_AD_FADECTRL_ID 187
#define BRAP_IMG_AD_FADECTRL_TABLE_ID 188
#define BRAP_IMG_AD_FADECTRL_INTERFRAME_ID 189

#define BRAP_IMG_AD_PANCTRL_ID 190
#define BRAP_IMG_AD_PANCTRL_TABLE_ID 191
#define BRAP_IMG_AD_PANCTRL_INTERFRAME_ID 192

#define BRAP_IMG_DRA_PASSTHRU_ID			193
#define BRAP_IMG_DRA_FRAMESYNC_ID		194
#define BRAP_IMG_DRA_INTERFRAME_ID 		195

#define BRAP_IMG_DOLBYVOLUME_ID 196
#define BRAP_IMG_DOLBYVOLUME_TABLE_ID 197
#define BRAP_IMG_DOLBYVOLUME_INTERFRAME_ID 198

#define BRAP_IMG_SRSTRUVOL_ID 199
#define BRAP_IMG_SRSTRUVOL_TABLE_ID 200
#define BRAP_IMG_SRSTRUVOL_INTERFRAME_ID 201

#define BRAP_IMG_PCM_PASSTTHRU_ID 202
#define BRAP_IMG_PCM_PASSTTHRU_FRAMESYNC_ID 203

#define BRAP_IMG_DRA_ID 204
#define BRAP_IMG_DRA_TABLE_ID 205

#define BRAP_IMG_PCMWAV_ID 206
#define BRAP_IMG_PCMWAV_FRAMESYNC_ID 207
#define BRAP_IMG_PCMWAV_INTERFRAME_ID		208

/* 
	This chunk size will be used when the firmware binary is actually present in multiple chunks.
	The BRAP_IMG_CHUNK_SIZE will then give the size of each such chunk
*/
#define BRAP_IMG_CHUNK_SIZE	65532


extern void *BRAP_IMG_Context;
extern const BIMG_Interface BRAP_IMG_Interface;

#endif /* BRAP_IMG_H__ */


