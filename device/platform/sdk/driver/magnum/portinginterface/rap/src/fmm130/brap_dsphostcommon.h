/***************************************************************************
*     Copyright (c) 2003-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dsphostcommon.h $
* $brcm_Revision: Hydra_Software_Devel/66 $
* $brcm_Date: 5/10/10 5:38p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_dsphostcommon.h $
* 
* Hydra_Software_Devel/66   5/10/10 5:38p srajapur
* SW7400-2688 : [7400] Added Support for BRAP_DSPCHN_AudioType_ePcmWav
* algorithm
* 
* Hydra_Software_Devel/65   7/21/09 5:27p srajapur
* PR53240 : [7403] DRA decoder development on 7403 chip
* 
* Hydra_Software_Devel/64   6/12/09 6:00p srajapur
* PR 53489:[7401,7400] Added support for SRS True Volume certification
* and added PCM decode algorithm
* 
* Hydra_Software_Devel/63   3/18/09 7:56p srajapur
* PR43833 : [7400]  BCM7400 support codec for AAC HE V2
* 
* Hydra_Software_Devel/62   2/25/09 6:18p srajapur
* PR52480: [7400] Name changing SRS VIQ to SRS TrueVolume
* 
* Hydra_Software_Devel/61   1/12/09 11:00p srajapur
* PR50613 :[7400] Added SRS VIQ post processing algorithm for 7400
* 
* Hydra_Software_Devel/60   11/12/08 7:30p srajapur
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
* Hydra_Software_Devel/59   9/16/08 3:16p bhanus
* PR 46417 : [7440] Merging DTS MA Support on main line.
* 
* Hydra_Software_Devel/RAP_7440_DTSMA_DEVEL/1   9/2/08 11:54a bhanus
* PR 46417 : [7440] Adding DTS MA Support
* 
* Hydra_Software_Devel/58   8/18/08 4:01p bhanus
* PR 45762:[7440] Adding Support for AAC HE for 7440.
* 
* Hydra_Software_Devel/57   8/8/08 6:32p gdata
* PR43971: [7443] Merging DVD family for 7443 and onward chips
* 
* Hydra_Software_Devel/RAP_DVD_Family/1   6/23/08 8:08p gdata
* PR43971: [7440] Creating DVD family for 7443 and onward chips
* 
* Hydra_Software_Devel/56   5/16/08 12:12p gdata
* PR42762: [7440] Adding support for DTSHD-NBC algo
* 
* Hydra_Software_Devel/55   5/10/08 3:54a srajapur
* PR42131: [7401,7403,7118] Added support for AACHEV2
* 
* Hydra_Software_Devel/54   5/7/08 5:56p srajapur
* PR42131: [7401,7403] Added support for AACHEV2
* 
* Hydra_Software_Devel/53   5/6/08 8:03p srajapur
* PR42131: [7401,7403] Added support for AACHEV2
* 
* Hydra_Software_Devel/52   4/28/08 2:28p srajapur
* PR42131 : [7401] Added support for AACHEV2
* 
* Hydra_Software_Devel/51   4/8/08 11:34a bhanus
* PR 41469 : [7440] Adding Support for WMA and WMA-Pro for 7440
* 
* Hydra_Software_Devel/50   1/17/08 6:10p gdata
* PR38790:  [7440] Adding support for LPCM_A_DVD
* 
* Hydra_Software_Devel/49   12/20/07 11:32a gdata
* PR38296: [7440] Adding support for DTSHD SUB audio
* 
* Hydra_Software_Devel/48   11/20/07 8:44p sushmit
* PR 36877: [7335] Bringup PI
* 
* Hydra_Software_Devel/47   10/25/07 3:04p sushmit
* PR 36003: [7405] Updated WMAPRO Support
* 
* Hydra_Software_Devel/46   9/7/07 3:53p speter
* PR 34673: [3563] Merging the Phase 2.6 branch to mainline
* 
* Hydra_Software_Devel/45   9/5/07 3:16p gdata
* PR 32827: [7440] Merging DTS-LBR 3-Stage support to
* Hydra_Software_Devel branch
* 
* Hydra_Software_Devel/RAP_7440_DTSLBR_3Stages/1   9/4/07 4:44p gdata
* PR 32827: [7440] Adding support for 3 stage DTS-LBR
* 
* Hydra_Software_Devel/RAP_3563_PH_2_6/1   9/6/07 1:05p speter
* PR 34460: [3563] Changing code to use 'Custom' for specific Post
* procesing algorithms
* 
* Hydra_Software_Devel/44   7/31/07 5:14p speter
* PR 33608: [3563] Removing the duplicate Custom Surround ID
* 
* Hydra_Software_Devel/43   7/31/07 4:18p speter
* PR 33608: [3563] Fixing the PP IDs for Custom Surround, Custom Bass, Custom
* Voice
* 
* Hydra_Software_Devel/42   7/30/07 5:47p speter
* PR 33608: [3563] Adding post processing algorithm Custom surround
* 
* Hydra_Software_Devel/41   7/19/07 6:25p bselva
* PR 33176:[7400]Checkin the PI changes for phase 4.0 release
* 
* Hydra_Software_Devel/40   7/18/07 6:26p gautamk
* PR33094 :[7440]Adding Support for Passthru mode For Algo DDP7.1, MLp
* and DTSHD (HBR Mode)
* 
* Hydra_Software_Devel/39   7/12/07 10:23a gautamk
* PR32256: [7401] Adding Support for passthru in WMA PRO
* 
* Hydra_Software_Devel/38   7/9/07 4:18p gautamk
* PR32827 :  Adding PI Support for DTS LBR Multichannel
* 
* Hydra_Software_Devel/37   7/6/07 12:17p gautamk
* PR32256: [7401] Adding new algo WMA PRO for 7401
* 
* Hydra_Software_Devel/36   7/5/07 4:29p gautamk
* PR32256: [7401] Adding new algo WMA PRO for 7401
* 
* Hydra_Software_Devel/35   7/5/07 4:25p gautamk
* PR32256: [7401] Adding new algo WMA PRO for 7401
* 
* Hydra_Software_Devel/34   6/27/07 10:56a kagrawal
* PR 32504: Changing MPEG-MC algo-id from 0x0C to 0x0E
* 
* Hydra_Software_Devel/33   6/15/07 8:06p gautamk
* PR30069: [7440] Added DTS-Extension Support.
* 
* Hydra_Software_Devel/32   6/15/07 2:17p kagrawal
* PR 30069: [7440] Merging Phase2.0 branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/31   6/14/07 7:27p gautamk
* PR30069: Changes for new algorithm Mpeg MC
* 
* Hydra_Software_Devel/30   6/14/07 4:52p gautamk
* PR30069: [7440] Adding algorithm DDP 7.1
*
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/1   6/9/07 5:33p kagrawal
* PR 30069: [7440] Added support for DTS-Neo PostProcessing
* 
* Hydra_Software_Devel/29   5/31/07 6:04p bselva
* PR 26501: [7401 Family]Checking in the changes for AAC-HE multichannel
* support
* 
* Hydra_Software_Devel/28   5/15/07 4:07p rjain
* PR 28422:[7401 family] Checking in PI changes for FSPB and DSOLA [needs
* new 7401 MIT]
* 
* Hydra_Software_Devel/27   5/7/07 4:36p nitinb
* PR 28862: [3563] Added support for post processing algorithm BBE
* 
* Hydra_Software_Devel/26   4/4/07 1:32a nitinb
* PR 28865: Added support for post processing algorithm XEN
* 
* Hydra_Software_Devel/25   3/27/07 5:05p kagrawal
* PR 28561: [7440] Adding support for DTS-LBR
* 
* Hydra_Software_Devel/24   1/31/07 3:49p bhanus
* PR 25607 : Modified Algo Id for DTS/DTS-HD
* 
* Hydra_Software_Devel/23   1/29/07 3:24p bhanus
* PR 25607: Added Support for Multi-executable DTS/DTS-HD
* 
* Hydra_Software_Devel/22   1/22/07 7:30p sushmit
* PR 26864:
* Changed BAF_FRAME_CONTINUATION_DTS to BAF_FRAME_PES to support external
* PCM input from I2S/SPDIF/BTSC for 3563
* Added PP Algo IDs.
* 
* Hydra_Software_Devel/22   1/22/07 7:24p sushmit
* PR 26864:
* Changed BAF_FRAME_CONTINUATION_DTS to BAF_FRAME_PES to support external
* PCM input from I2S/SPDIF/BTSC for 3563
* Added PP Algo IDs.
* 
* Hydra_Software_Devel/22   1/12/07 8:30p sushmit
* PR 26864:
* Changed BAF_FRAME_CONTINUATION_DTS to BAF_FRAME_PES to support external
* PCM input from I2S/SPDIF/BTSC for 3563
* Added PP Algo IDs.
* 
* Hydra_Software_Devel/21   10/3/06 10:51a bselva
* PR 24468: Checking in the changes required for phase 3 features
* 
* Hydra_Software_Devel/20   7/3/06 8:15p nitinb
* PR 22443: Replace AC3 decoder with DDP decoder in AC3 Lossless codec
* 
* Hydra_Software_Devel/19   6/30/06 12:39p kagrawal
* PR 22083: Added support for HD-DVD MLP for 7411D0
* 
* Hydra_Software_Devel/18   6/15/06 2:45p nitinb
* PR 22083: Add support for MLP algorithm in 7411D0 RAP PI
* 
* Hydra_Software_Devel/17   6/13/06 2:53p nitinb
* PR 22083: Add support for MLP algorithm in 7411D0 RAP PI
* 
* Hydra_Software_Devel/16   5/24/06 4:13p nitinb
* PR 21705: Require RAP PI support to download framesync executable
* depending on DVD audio type
* 
* Hydra_Software_Devel/15   5/19/06 5:16p nitinb
* PR 21105: Fixed MPEG decode, which was broken due to WMA addition
* 
* Hydra_Software_Devel/14   5/4/06 11:35a nitinb
* PR 21400: Add RAP support for DD in lossless for 7411D0
* 
* Hydra_Software_Devel/13   5/3/06 11:07p nitinb
* PR 21105: 7411D0 - Add support WMA Raptor Porting Interface
* 
* Hydra_Software_Devel/12   4/11/06 6:05p sushmit
* PR18604: Modifying
* #if (( BCHP_CHIP == 7401 )||( BCHP_CHIP == 7400 ))
* With code like
* #ifndef BCHP_7411_VER
* 
* Hydra_Software_Devel/11   3/28/06 12:06p rnayak
* PR20439: Restore Bangalore Raptor files after magnum vob corruption on
* 17th March 2006
* 
* Hydra_Software_Devel/11   3/20/06 8:09p nitinb
* PR 20287: Implement DVD LPCM algorithm for 7411 D0
* 
* Hydra_Software_Devel/10   3/16/06 5:21p nitinb
* PR 20117: Implement dolby digital bass management algorithm
* 
* Hydra_Software_Devel/9   2/27/06 4:17p sushmit
* PR18604: Fixing 7411 compilation issues.
* 
* Hydra_Software_Devel/8   2/17/06 9:56a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/4   2/6/06 4:18p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/RAP_AUD_ENC/3   1/16/06 5:38p sushmit
* PR18604: Updating Audio Encoder PI.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   12/19/05 6:31p sushmit
* PR18604: Updating for combined decoder & encoder scheduler.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/1   12/19/05 10:57a sushmit
* PR18604: Updating for combined decoder & encoder scheduler.
* 
* Hydra_Software_Devel/5   12/2/05 1:16p bselva
* PR 18312: Added support for LPCM
* 
* Hydra_Software_Devel/4   11/29/05 6:03p bselva
* PR 18312: Added pass through support for DTS
* 
* Hydra_Software_Devel/3   11/29/05 4:58p bselva
* PR 18312: Added for DTS support
* 
* Hydra_Software_Devel/2   7/28/05 11:22p nitinb
* PR 16092: Merging new MIT code into main branch
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/1   6/16/05 9:20a nitinb
* Made changes to support DDP and AAC-HE firmware developement with new
* MIT changes
* 
* DSP_Raptor_Devel/21   6/7/05 3:29p ramanms
* PR 15608: Changing the Algo Ids to reflect the Multi stages
* 
* DSP_Raptor_Devel/20   5/27/05 10:20p ramanms
* PR 15608: Changing  the ALGO IDs to incorporate the Num executables in
* MSB.
* 
* DSP_Raptor_Devel/19   10/26/04 11:16a ramanms
* PR 13035: Changing the defines for the SRS tables
* 
* DSP_Raptor_Devel/18   9/27/04 7:56p ramanms
* PR 12700: Adding defines for SRS
* 
* DSP_Raptor_Devel/17   6/4/04 7:01p ramanms
* SPDIF parser support - Intermediate-coding not complete
* 
* DSP_Raptor_Devel/16   5/14/04 1:24p kwelch
* Updating post processing support
* 
* DSP_Raptor_Devel/15   5/7/04 1:47p kwelch
* Changing Mastger Index Table
* 
* DSP_Raptor_Devel/14   3/31/04 5:33p kwelch
* intermediate update
* 
* DSP_Raptor_Devel/12   2/18/04 4:13p kwelch
* Updating table ID's for AAC and AC3
* 
* DSP_Raptor_Devel/11   2/13/04 3:35p kwelch
* Changing algorithm selections ID's
* 
* DSP_Raptor_Devel/10   2/10/04 12:54p kwelch
* Adding additional algorithm Id's
* 
* Hydra_Software_Devel/1   4/9/03 5:32p vobadm
* merged from aramPrivateBranch
* 
* aramPrivateBranch/2   4/1/03 9:27a aram
* more files was added
* 
* aramPrivateBranch/1   3/24/03 12:29p aram
* genesis of files
***************************************************************************/
 
#ifndef AUDIO_DSP_HOST_COMMON_H__
#define AUDIO_DSP_HOST_COMMON_H__

/* Section load address's for Host pre-boot initialization of DSP local memory, (word address) */
/* These values must match those specified in linker build scripts */
#define HOST_APP_IBOOT_LOAD_ADDRESS			0x0000  /* Load address in DSP IMEM for 'iboot' interrupt vector section */
#define HOST_APP_TEXT_LOAD_ADDRESS			0x0100  /* Load address in DSP IMEM for 'text' scheduler executable section */
#define HOST_APP_DATA_LOAD_ADDRESS			0x0000  /* Load address in DSP DMEM for 'data' scheduler data section */

#define BAF_INVALID							0xffff
#define BAF_ALGORITHM_INVALID				BAF_INVALID
#define BAF_FRAME_SYNC_INVALID				BAF_INVALID
#define BAF_FRAME_CONTINUATION_INVALID		BAF_INVALID
#define BAF_PP_INVALID						BAF_INVALID

#if (BRAP_DVD_FAMILY == 1)

/* decode algorithm support */
#define BAF_ALGORITHM_MPEG_L1				0x00
#define BAF_ALGORITHM_MPEG_L2				0x01
#define BAF_ALGORITHM_MP3					0x02
#define BAF_ALGORITHM_AAC					0x03
#define BAF_ALGORITHM_AAC_HE				0x04
#define BAF_ALGORITHM_AAC_HE_1				0x05
#define BAF_ALGORITHM_AC3					0x06
#define BAF_ALGORITHM_DDP					0x07
#define BAF_ALGORITHM_DDP_1					0x08
#define BAF_ALGORITHM_DD_LOSSLESS			BAF_ALGORITHM_DDP
#define BAF_ALGORITHM_DD_LOSSLESS_1			BAF_ALGORITHM_DDP_1

#define BAF_ALGORITHM_LPCM_CUSTOM           0x0A
#define BAF_ALGORITHM_BD_LPCM				0x0B

#define BAF_ALGORITHM_LPCM_HDDVD			0x0C
#define BAF_ALGORITHM_MPEG_MC				0x0E

/* Support for Encoder Scheduler */
#define BAF_ALGORITHM_ENCODER_SCHEDULER  0x0F

#define BAF_ALGORITHM_LPCM_DVD				0x10
#define BAF_ALGORITHM_WMA_STD				0x11
#define BAF_ALGORITHM_MLP					0x12

#define BAF_ALGORITHM_DDP_7_1				0x13
#define BAF_ALGORITHM_DDP_7_1_1				0x14

#define BAF_ALGORITHM_DTS_LBR               0x15 
#define BAF_ALGORITHM_DTS_LBR_1             0x16 
#define BAF_ALGORITHM_DTS_LBR_2             0x17
/* BAF_ALGORITHM_DTS_ENC 0x18 is in brap_hostencoderinterface.h. Should be moved here*/
#define BAF_ALGORITHM_DTS					0x19              /* Common for DTS-HD and DTS */
#define BAF_ALGORITHM_DTS_0					BAF_ALGORITHM_DTS	/* Common for DTS-HD and DTS */
#define BAF_ALGORITHM_DTS_1					0x1A	/* Common for DTS-HD and DTS */
#define BAF_ALGORITHM_DTSHD_0				BAF_ALGORITHM_DTS_0
#define BAF_ALGORITHM_DTSHD_1				BAF_ALGORITHM_DTS_1
#define BAF_ALGORITHM_DTSHD_2				0x1B
#define BAF_ALGORITHM_DTSHD_3				0x1C
#define BAF_ALGORITHM_DTSHD_4				0x1D

#define BAF_ALGORITHM_DTSHD_SUB_0			0x1E
#define BAF_ALGORITHM_DTSHD_SUB_1			0x1F
#define BAF_ALGORITHM_DVD_A_LPCM            0x20

#define BAF_ALGORITHM_WMA_PRO_0				0x21
#define BAF_ALGORITHM_WMA_PRO_1				0x22

#define BAF_ALGORITHM_AACHE_0		        0x23
#define BAF_ALGORITHM_AACHE_1		        0x24
#define BAF_ALGORITHM_AACHE_2		        0x25
#define BAF_ALGORITHM_AACHE_3		        0x26
#define BAF_ALGORITHM_AACHE_4		        0x27


#else

/* decode algorithm support */
#define BAF_ALGORITHM_MPEG_L1				0x00
#define BAF_ALGORITHM_MPEG_L2				0x01
#define BAF_ALGORITHM_MP3					0x02
#define BAF_ALGORITHM_AAC					0x03
#define BAF_ALGORITHM_AAC_HE				0x04
#define BAF_ALGORITHM_AAC_HE_1				0x05
#define BAF_ALGORITHM_AC3					0x06
#define BAF_ALGORITHM_DDP					0x07
#define BAF_ALGORITHM_DDP_1					0x08
/* BAF_ALGORITHM_DTS and BAF_ALGORITHM_DTSHD below are kept for old fw executables. 
   For new executables that has support for extensions, please use 
   BAF_ALGORITHM_DTS_x and BAF_ALGORITHM_DTSHD_x */
#define BAF_ALGORITHM_DTS					0x09              /* Common for DTS-HD and DTS */
#define BAF_ALGORITHM_DTSHD					BAF_ALGORITHM_DTS /* Common for DTS-HD and DTS */
#define BAF_ALGORITHM_LPCM_CUSTOM           0x0A
#define BAF_ALGORITHM_BD_LPCM				0x0B
/*#define BAF_ALGORITHM_DV25					0x0C
#define BAF_ALGORITHM_LPCM_HDDVD			0x0C
#define BAF_ALGORITHM_PCM_PLAYBACK			0x0D
#define BAF_ALGORITHM_AUTO_DETECT			0x0E*/
#define BAF_ALGORITHM_LPCM_HDDVD			0x0C
/*#define BAF_ALGORITHM_MPEG_MC				0x0E*/
#define BAF_ALGORITHM_LPCM_DVD				0x10
#define BAF_ALGORITHM_WMA_STD				0x11
#define BAF_ALGORITHM_DD_LOSSLESS			BAF_ALGORITHM_DDP
#define BAF_ALGORITHM_DD_LOSSLESS_1			BAF_ALGORITHM_DDP_1
#define BAF_ALGORITHM_MLP					0x12

#if ((BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403) || (BCHP_CHIP == 7400) || (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335))
#define BAF_ALGORITHM_WMA_PRO_0				0x0D
#define BAF_ALGORITHM_WMA_PRO_1				0x0E
#endif

#ifndef BCHP_7411_VER /* For chips other than 7411 */
/* Support for Encoder Scheduler */
#define BAF_ALGORITHM_ENCODER_SCHEDULER  0x0F
/*#define BAF_ALGORITHM_ENCODER_SCHEDULER  (BAF_HOST_MAX_DL_MODULE - 1)*/
#endif

#if (BCHP_CHIP == 7401) || (BCHP_CHIP == 7403)|| (BCHP_CHIP == 7118)|| (BCHP_CHIP == 7400)
#define BAF_ALGORITHM_AACHE_0		0x13
#define BAF_ALGORITHM_AACHE_1		0x14
#define BAF_ALGORITHM_AACHE_2		0x15
#define BAF_ALGORITHM_AACHE_3		0x16
#define BAF_ALGORITHM_AACHE_4		0x17
#else
#define BAF_ALGORITHM_AAC_HE_LPSBR_0		0x16
#define BAF_ALGORITHM_AAC_HE_LPSBR_1		0x17
#define BAF_ALGORITHM_AAC_HE_LPSBR_2		0x18
#endif

#define BAF_ALGORITHM_DDP_7_1				0x13
#define BAF_ALGORITHM_DDP_7_1_1				0x14


#define BAF_ALGORITHM_DTS_0					0x19	/* Common for DTS-HD and DTS */
#define BAF_ALGORITHM_DTS_1					0x1A	/* Common for DTS-HD and DTS */
#define BAF_ALGORITHM_DTSHD_0				BAF_ALGORITHM_DTS_0
#define BAF_ALGORITHM_DTSHD_1				BAF_ALGORITHM_DTS_1
#define BAF_ALGORITHM_DTSHD_2				0x1B
#define BAF_ALGORITHM_DTSHD_3				0x1C
#define BAF_ALGORITHM_DTSHD_4				0x1D
#define BAF_ALGORITHM_PCM_PASSTHRU          0x1E
#define BAF_ALGORITHM_DRA   				0x1F
#define BAF_ALGORITHM_PCMWAV                0x20

#endif

/* frame sync support */
#define BAF_FRAME_SYNC_MPEG					0
#define BAF_FRAME_SYNC_DD_LOSSLESS			1
#define BAF_FRAME_SYNC_AAC					2
#define BAF_FRAME_SYNC_WMA_STD				3
#define BAF_FRAME_SYNC_AC3					4
#define BAF_FRAME_SYNC_DTS_HD_HDDVD			5
#define BAF_FRAME_SYNC_AAC_HE				6
#define BAF_FRAME_SYNC_MLP					7
#define BAF_FRAME_SYNC_DDP					8
#define BAF_FRAME_SYNC_MLP_HDDVD			9
#define BAF_FRAME_SYNC_DTS					10
/* Changed for 3563 to handle PCM Capture from external input & subsequent PP cases */
/*#define BAF_FRAME_CONTINUATION_DTS			11*/
#define BAF_FRAME_SYNC_PES         			11
#define BAF_FRAME_SYNC_BD_LPCM				12
#define BAF_FRAME_SYNC_DDP_7_1			    13
/*#define BAF_FRAME_CONTINUATION_BD_LPCM		13*/
#define BAF_FRAME_SYNC_HDDVD_LPCM			14
#define BAF_FRAME_SYNC_WMA_PRO				15
/*#define BAF_FRAME_CONTINUATION_HDDVD_LPCM	15*/
#define BAF_FRAME_SYNC_DTSHD					16
#define BAF_FRAME_CONTINUATION_DTSHD		17
#define BAF_FRAME_SYNC_DVD_LPCM				18
#define BAF_FRAME_CONTINUATION_DVD_LPCM		19
#define BAF_FRAME_SYNC_DTS_LBR              20 
#define BAF_FRAME_SYNC_MPEG_MC				21
#define BAF_FRAME_SYNC_DTSHD_SUB            22 
#define BRAF_FRAME_SYNC_DVD_A_LPCM          23
#define BAF_FRAME_SYNC_DTSHD_NBC_CORE       24
#define BAF_FRAME_SYNC_DTSHD_STG1           25
#define BAF_FRAME_SYNC_DRA					26
#define BAF_FRAME_PCM_PASSTHRU				27
#define BAF_FRAME_SYNC_PCMWAV				28

/* SPDIF parser support */
#define BAF_PASS_THRU_MPEG				0
#define BAF_PASS_THRU_AAC					1
#define BAF_PASS_THRU_AC3					2
#define BAF_PASS_THRU_AAC_HE				3
#define BAF_PASS_THRU_DDP					4
#define BAF_PASS_THRU_DTS					5
#define BAF_PASS_THRU_HDDVD_LPCM               6
#define BAF_CONVERT_DDP					7

#define BAF_PASS_THRU_DTSHD	                BAF_PASS_THRU_DTS /* Common for DTS-HD and DTS */
#define BAF_PASS_THRU_DD_LOSSLESS		BAF_PASS_THRU_DDP
#define BAF_PASS_THRU_MPEG_MC		8
#define BAF_PASS_THRU_WMA_PRO		9

#define BAF_PASS_THRU_MLP			10
#define BAF_PASS_THRU_DDP_7_1		11

#define BAF_PASS_THRU_DTSHD_HBR	       12
#define BAF_PASS_THRU_DRA				13

/* post processing support */ 
#define BAF_PP_SRS_TRUSURROUND			0
#define BAF_PP_SRC							1
#define BAF_PP_KARAOKE						2
#define BAF_PP_DDBM						2
#define BAF_PP_DOWNMIX					3
#define BAF_PP_DOWNMIX_USER1				4
#define BAF_PP_DOWNMIX_AAC				5
#define BAF_PP_DOWNMIX_AAC_USER1			6
#define BAF_PP_DOWNMIX_AC3				7
#define BAF_PP_KARAOKE_UNAWARE			8
#define BAF_PP_CUSTOM_SURROUND                         8
#define BAF_PP_KARAOKE_AWARE				9
#define BAF_PP_CUSTOM_BASS                                 9
#define BAF_PP_KARAOKE_CAPABLE			10
#define BAF_PP_CUSTOM_VOICE                      10
#define BAF_PP_KARAOKE_USER1				11
#define BAF_PP_PEQ                                        11
#define BAF_PP_DOWNMIX_DD_LOSSLESS		BAF_PP_DOWNMIX_AC3
#define BAF_PP_AVL                                         12
#define BAF_PP_PLII                                         13
#define BAF_PP_XEN					             14
#define BAF_PP_BBE					             15
#define BAF_PP_DSOLA                                     16
#define BAF_PP_DTS_NEO                                 17
#define BAF_PP_AD_FADECTRL                  18
#define BAF_PP_AD_PANCTRL                   19
#define BAF_PP_DOBLYVOLUME							   20
#define BAF_PP_SRSTRUVOL					   21

/* Table support for AAC decode processing */
#define BAF_AAC_TABLE_SWBTABLE				0
#define BAF_AAC_TABLE_HUFFMAN				1
#define BAF_AAC_TABLE_PRETWIDDLE			2
#define BAF_AAC_TABLE_IQTSCL				3
#define BAF_AAC_TABLE_TWIDDLE				4
#define BAF_AAC_TABLE_WINCOEF				5
#define BAF_AAC_TABLE_DELAY					6

/* Table support for AC3 decode processing */
#define BAF_AC3_TABLE_IMDCT					0
#define BAF_AC3_TABLE_16_BITS				1
#define BAF_AC3_TABLE_DELAY					2
#define BAF_AC3_TABLE_ASSEMBLY_LABEL		3

/* Table support for MPEG decode processing */
#define BAF_MPEG_TABLE_ASSEMBLY_LABEL		0
#define BAF_MPEG_TABLE_COMMON				1
#define BAF_MPEG_TABLE_PARSER_L1L2			2
#define BAF_MPEG_TABLE_PARSER_L3LSF			3
#define BAF_MPEG_TABLE_HUFFMAN				4
#define BAF_MPEG_TABLE_HUFFMAN_DATA			5
#define BAF_MPEG_TABLE_HYBRID				6
#define BAF_MPEG_TABLE_SUBBAND				7
#define BAF_MPEG_TABLE_DELAY				8
#define BAF_MPEG_TABLE_STRUCT				9

/* Table support for SRS */
#define BAF_PP_SRS_TABLE_TRU_SURROUND		0
#define BAF_PP_SRS_TABLE_FILTER_COEFF		1 
#define BAF_PP_SRS_TABLE_FILTER_STATES		2

#endif

