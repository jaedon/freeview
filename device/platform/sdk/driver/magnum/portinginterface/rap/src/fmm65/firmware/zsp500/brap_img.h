/***************************************************************************
*     Copyright (c) 2006-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_img.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 9/11/12 4:59p $
*
* Module Description:
*	This file is part of image interface implementation for RAP PI.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/src/fmm65/firmware/zsp500/brap_img.h $
* 
* Hydra_Software_Devel/1   9/11/12 4:59p srajapur
* SW7420-2341:Remove symbolic links and flatten the RAP directory
* 
* Hydra_Software_Devel/NEW_DIR_SW7420_2341/1   9/7/12 4:11p srajapur
* SW7420-2341:Remove symbolic links and flatten the RAP directory
* 
* Hydra_Software_Devel/82   3/18/11 9:42p sgadara
* SW7420-767: [7420] Merge MS11 development branch to mainline
* 
* Hydra_Software_Devel/81   10/21/10 7:09p srajapur
* SW7550-601 : [7550] Adding PI Support for BTSC Encoder
* 
* Hydra_Software_Devel/80   9/16/10 12:20a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/79   9/6/10 5:16p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/78   9/6/10 3:44p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code.
* Merging to mainline
* 
* Hydra_Software_Devel/RAP_7550_TASK_DOWNLOAD_BRANCH/1   8/31/10 3:37p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code
* 
* Hydra_Software_Devel/77   8/13/10 5:23p gautamk
* SW7405-4713:[7405] Merging changes for RDB indepedence in Main line
* 
* Hydra_Software_Devel/76   8/10/10 1:53p gautamk
* SW7420-767: [7420] Merging MS11 feature support to mainline
* 
* Hydra_Software_Devel/75   5/24/10 8:29p sgadara
* SW3548-2923: [3548,3556] Adding Support for SRS StudioSound.
* StudioSound  --> CsTd+TSHD+TVOL+EqHl
* 
* Hydra_Software_Devel/74   4/26/10 2:05p srajapur
* SW3548-2899 : [3548] Added PI support for ABX post processing
* 
* Hydra_Software_Devel/73   4/22/10 8:59p srajapur
* SW7405-3993 : [3548] Adding Audyssey Volume post processing algorithm
* 
* Hydra_Software_Devel/72   4/22/10 5:13p sgadara
* SW7405-4170:[7405,3548,3556] Making chages to internally map AACHE and
* AC3 Encode to Dolby Pulse and DD Transcode in PI for MS10 license
* 
* Hydra_Software_Devel/71   4/19/10 7:12p gautamk
* SW7405-4170:[7405] Adding Support for MS10 DDP/AC3
* 
* Hydra_Software_Devel/70   4/6/10 2:38p gautamk
* SW7405-4170: [7405] Adding FWMixer processing stage.
* 
* Hydra_Software_Devel/69   3/12/10 2:48p speter
* SW3548-2827:[3548] Adding Mono Downmix support as post processing
* algorithm
* 
* Hydra_Software_Devel/68   3/11/10 4:52p speter
* SW3548-2822:[3548] Add Dolby Transcode and Broadcom 3D surround post
* processing algos
* 
* Hydra_Software_Devel/67   3/10/10 5:06p speter
* SW3548-2819:[3548] Adding Dolby Pulse Algorithm
* 
* Hydra_Software_Devel/66   2/19/10 5:21p gautamk
* SW7468-119: [7468] Adding RAP PI support for Real audio LBR
* 
* Hydra_Software_Devel/65   2/10/10 8:54p sgadara
* SW3556-1031: [3548,3556] Adding India Mode
* -->Merge to main line
* 
* Hydra_Software_Devel/SW3556-1031/1   2/2/10 5:04p jgarrett
* SW3556-1031: [3556] Adding India Mode
* 
* Hydra_Software_Devel/64   11/19/09 2:29p gautamk
* SW7405-3063: [7405] Support for DTS HD
* 
* Hydra_Software_Devel/63   11/17/09 12:50p gautamk
* SW7405-2001: [7405] removing entries of WMAPRo passthru
* table/interframe.
* 
* Hydra_Software_Devel/62   11/10/09 2:21p speter
* SW7405-2959: [3548] Adding DTS-HD support
* 
* Hydra_Software_Devel/61   9/23/09 6:14p gautamk
* SW7420-357: [7420] Adding support for SBC Encode
* 
* Hydra_Software_Devel/60   9/23/09 1:13p gautamk
* SW7405-3063: [7405] defining Macros for Passthru. Enabling them in
* default in .inc file for chips which supports them.
* 
* Hydra_Software_Devel/59   9/22/09 5:50p gautamk
* SW7405-3058: [7405] Adding support for DRA decoder.
* 
* Hydra_Software_Devel/58   9/11/09 8:06p sgadara
* SW3548-2188: [3548,3556] Adding support for AMR decode algorithm.
* 
* Hydra_Software_Devel/57   9/11/09 4:45p gautamk
* SW7405-2986: [7405] Enabling Passthru without license
* 
* Hydra_Software_Devel/56   7/15/09 11:05a gautamk
* PR56006: [7405] Adding support for mp3 encode in rap pi
* 
* Hydra_Software_Devel/55   6/23/09 3:56p gautamk
* PR56006: [7506]merging AACHEV2 to mainline
* 
* Hydra_Software_Devel/REFSW_RAP_7405_AACV2/1   6/15/09 2:36p gautamk
* PR56006: [7506] PI support for AACV2
* 
* Hydra_Software_Devel/54   6/9/09 11:38a gautamk
* PR47949: [7405]Changing name from PCMOnAVI to PCMWAV
* 
* Hydra_Software_Devel/53   6/4/09 3:21p gautamk
* PR47949: [7405]PI Support for PCM on AVI
* 
* Hydra_Software_Devel/52   5/12/09 4:56p gautamk
* PR52383: [7405] Fixing  error for 3548
* 
* Hydra_Software_Devel/51   5/12/09 1:52p gautamk
* PR52383: [7405] Fixing the break in 3548 due to DTS decode checkin.
* 
* Hydra_Software_Devel/50   5/11/09 4:00p gautamk
* PR52383: [7405] Changes for DTS decoder.
* 
* Hydra_Software_Devel/49   4/14/09 3:31p gautamk
* PR54056: [7405] Making the max chunk size to 65532
* 
* Hydra_Software_Devel/48   3/18/09 9:55p gautamk
* PR53365:[7405] Adding support for dolby volume
* 
* Hydra_Software_Devel/47   2/13/09 1:33p speter
* PR 52090: [3548,3556] Merging the Phase 4.2 Take 2 release to main line
* 
* Hydra_Software_Devel/RAP_354856_PH_4_3/1   2/10/09 11:55a speter
* PR 46135: [3548,3556] Adding SRS Tru Volume Support - Chaning the name
* to TruVolume
* 
* Hydra_Software_Devel/46   12/23/08 1:34p gautamk
* PR48850: [7405]  adding support for DVD LPCM. Merging to mainline
* 
* Hydra_Software_Devel/45   12/22/08 11:33a speter
* PR 46135: [3548,3556] Adding SRS VIQ Support
* 
* Hydra_Software_Devel/44   11/25/08 10:04a speter
* PR 49564: [3548,3556] Merging the Phase 4.0 release to main line
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/1   11/5/08 7:09p sgadara
* PR 40715: [3548,3556] Adding support for new AAC decoder.
* 
* Hydra_Software_Devel/41   10/21/08 11:44p speter
* PR 46051: [3548,3556] Changing DDEnc to AC3Enc
* 
* Hydra_Software_Devel/40   10/10/08 10:05a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   10/1/08 3:14p speter
* PR 46051: [3548,3556] Changing DDEnc to AC3Enc
* 
* Hydra_Software_Devel/39   9/22/08 11:29a speter
* PR 38950: [3548,3556] Adding support for SPDIF In. Adding DTS passthru
* support.
* 
* Hydra_Software_Devel/38   9/16/08 2:44p sushmit
* PR 43454:[7405]Adding AC3 Encode Support.
* 
* Hydra_Software_Devel/37   9/4/08 10:56p speter
* PR 46565: [3548,3556] Adding SRS-HD support
* 
* Hydra_Software_Devel/36   8/28/08 12:24p speter
* PR 46047: [3548,3556] Adding support for Custom Bass and Custom
* Surround
* 
* Hydra_Software_Devel/35   8/21/08 12:10p sushmit
* PR 43454:[7405]Updating AD Files.
* 
* Hydra_Software_Devel/34   8/12/08 11:03p sushmit
* PR 43454:[7405]Adding DSOLA.
* 
* Hydra_Software_Devel/33   8/10/08 11:56a sushmit
* PR 43454:[7405] DTSENC Updates.
* 
* Hydra_Software_Devel/32   8/8/08 9:55a speter
* PR 45176: [3548,3556] Adding the code for the RF Audio Nicam mode
* changes
* 
* Hydra_Software_Devel/31   7/28/08 9:42a speter
* PR 45171: [3548,3556] Merging from Phase 2.0 release to main line of
* development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/30   7/23/08 9:33p gautamk
* PR43454: [7405] Adding support for Wma passthru
* 
* Hydra_Software_Devel/29   7/23/08 3:58p gautamk
* PR43454: [7405] inserting PCMrouter satge interanally for AD Fade
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/1   7/24/08 5:01p speter
* PR 43454: [3548,3556] Adding PCM router post processing algorithm
* 
* Hydra_Software_Devel/28   7/22/08 12:17p speter
* PR 45032: [3548,3556] Adding PCM algorithm support
* 
* Hydra_Software_Devel/27   6/11/08 7:35a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/26   5/29/08 2:16a speter
* PR 38950: [3548,3556] Adding Audio Descriptor support
* 
* Hydra_Software_Devel/25   5/28/08 11:39p gautamk
* PR41726: [7405] Modifying files for AAC/AAC-HE LOAS/ADTS
* 
* Hydra_Software_Devel/24   5/28/08 8:07p gautamk
* PR41726: [7405] Adding support for SRC postprocessing
* 
* Hydra_Software_Devel/23   5/28/08 2:49a speter
* PR 39442: [3548,3556] Adding the Nicam and Pal A2 support
* 
* Hydra_Software_Devel/22   4/28/08 3:48p speter
* PR 38950: [3548] Fixing compilation for 3548/3556. Adding BTSC support
* 
* Hydra_Software_Devel/21   4/25/08 5:16p gautamk
* PR41726: [7405] PI support for WMA PRO
* 
* Hydra_Software_Devel/20   4/22/08 4:59p speter
* PR 38950: [3548] Adding CustomVoice (Post Processing) Support
* 
* Hydra_Software_Devel/19   4/17/08 1:56p gautamk
* PR41726: [7405] adding changes for wma std
* 
* Hydra_Software_Devel/18   4/11/08 5:12p sushmit
* PR 41365: [7405] Allowing only AC3 Passthru without license.
* 
* Hydra_Software_Devel/17   4/2/08 2:05p sushmit
* PR 34648: [7405] DD Convert Code Download Fix
* 
* Hydra_Software_Devel/16   3/31/08 4:35p sushmit
* PR 34648: [7405] Modifying TSM Config Params & adding DDP Files.
* 
* Hydra_Software_Devel/15   3/13/08 2:45p sushmit
* PR 34648: [7405] Checking in AACHE Support Changes
* 
* Hydra_Software_Devel/14   3/7/08 9:41p sushmit
* PR 34648: [7405] Adding AAC-HE IMG IDs
* 
* Hydra_Software_Devel/13   2/15/08 8:56p gautamk
* PR34648: [7405] Adding image id for passthru interframe.
* 
* Hydra_Software_Devel/12   2/11/08 7:06p gautamk
* PR34648: [7405] Adding PI Support for DDP.
* 
* Hydra_Software_Devel/11   2/11/08 3:03p gautamk
* PR34648: [7405]: Fixing TSM Comilation.
* 
* Hydra_Software_Devel/10   1/24/08 4:05p sushmit
* PR34648: [7405] Updating code for Mpeg bringup
* 
* Hydra_Software_Devel/9   1/21/08 11:50a sushmit
* PR 34648: [7405] AC3 Bringup Changes.
* Change AUD_START_TASK_ID_OFFSET
* Convert MIT Addrs to physical addresses
* Fix FIFO Addr Table Generation
* Take care of AC3 IDS instead of AC3 FrameSync
* Take care of AC3 IDS Interframe requirement
* Convert CIT Virt Addr to Phys Addrs
* 
* Hydra_Software_Devel/8   1/17/08 5:04p gautamk
* PR34648: [7405] bringin up Ac3 decode
* 
* Hydra_Software_Devel/7   1/15/08 12:02p sushmit
* PR34648: [7405] Checkin in updated code
* 
* Hydra_Software_Devel/6   1/14/08 3:12p gautamk
* PR34648: [7405] Changes for having only one passthru exec for all exec.
* 
* Hydra_Software_Devel/5   12/18/07 7:18p nitinb
* PR34648: [7405] Fixed compilation error.

***************************************************************************/
#ifndef BRAP_IMG_H__
#define BRAP_IMG_H__

#include "bimg.h"

/* 
	Firmware Image IDs that are to be used in the BRAP_IMG_Open function of the BRAP_IMG_Interface
*/

typedef enum BRAP_Img_Id
{
BRAP_Img_Id_eSystemIboot,
BRAP_Img_Id_eSystemCode,
BRAP_Img_Id_eSystemData,
BRAP_Img_Id_eSystemRdb,
BRAP_Img_Id_eDecodeTsm,
BRAP_Img_Id_eDecodeTsmInterframe,
BRAP_Img_Id_eEncodeTsm,
BRAP_Img_Id_ePassthruTsm,
BRAP_Img_Id_ePassthruCode,
BRAP_Img_Id_ePassthruTable,
BRAP_Img_Id_ePassthruInterFrame,
BRAP_Img_Id_eSystemAudioTask,
BRAP_Img_Id_eSystemGfxTask,
BRAP_Img_Id_eSystemVideoTask,

#ifndef RAP_MULTISTREAM_DECODER_SUPPORT
#ifdef  RAP_AC3_PASSTHRU_SUPPORT       
BRAP_Img_Id_eAc3_Ids,
BRAP_Img_Id_eAc3_Ids_Interframe,
#endif
#endif

#ifdef  RAP_MPEG_PASSTHRU_SUPPORT       
BRAP_Img_Id_eMpeg1_Ids,
BRAP_Img_Id_eMpeg1_Ids_Interframe,
#endif

#ifdef RAP_AACSBR_PASSTHRU_SUPPORT
BRAP_IMG_Id_eAache_Adts_Ids,
BRAP_IMG_Id_eAache_Loas_Ids,
BRAP_IMG_Id_eAache_Adts_Ids_Interframe,
BRAP_IMG_Id_eAache_Loas_Ids_Interframe,
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
BRAP_Img_Id_eMsDdp_Ids,
BRAP_Img_Id_eMsDdp_Ids_Interframe,
#else
#ifdef  RAP_DDP_PASSTHRU_SUPPORT       
BRAP_Img_Id_eDdp_Ids,
BRAP_Img_Id_eDdp_Ids_Interframe,
#endif
#endif

#ifdef  RAP_DTSBROADCAST_PASSTHRU_SUPPORT       
BRAP_Img_Id_eDtsBroadcast_Ids,
BRAP_Img_Id_eDtsBroadcast_Ids_Interframe,
#endif

#ifdef  RAP_DTSHD_PASSTHRU_SUPPORT       
BRAP_Img_Id_eDtsHd_Ids,
BRAP_Img_Id_eDtsHd_Ids_Interframe,
#endif

#ifdef  RAP_DRA_PASSTHRU_SUPPORT       
BRAP_Img_Id_eDRA_Ids,
BRAP_Img_Id_eDRA_Ids_Interframe,
#endif

#ifdef  RAP_REALAUDIOLBR_PASSTHRU_SUPPORT       
BRAP_Img_Id_eRealAudioLbr_Ids,
BRAP_Img_Id_eRealAudioLbr_Ids_Interframe,
#endif

#ifdef RAP_MPEG_SUPPORT
BRAP_Img_Id_eMpeg1_Decode,
BRAP_Img_Id_eMpeg1_Decode_Table,
BRAP_Img_Id_eMpeg1_Interframe,
#endif

#ifdef RAP_AAC_SUPPORT
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
BRAP_IMG_Id_eDolbyPulse_Decode_Stg0,
BRAP_IMG_Id_eDolbyPulse_Decode_Stg1,
BRAP_IMG_Id_eDolbyPulse_Decode_Stg2,
BRAP_IMG_Id_eDolbyPulse_Decode_Stg3,
BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg0,
BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg1,
BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg2,
BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg3,
BRAP_IMG_Id_eDolbyPulse_Interframe_Stg0,
BRAP_IMG_Id_eDolbyPulse_Interframe_Stg1,
BRAP_IMG_Id_eDolbyPulse_Interframe_Stg2,
BRAP_IMG_Id_eDolbyPulse_Interframe_Stg3,
#else
#ifdef RAP_AACSBR_SUPPORT
BRAP_IMG_Id_eAache_Decode_Stg0,
BRAP_IMG_Id_eAache_Decode_Stg1,
BRAP_IMG_Id_eAache_Decode_Stg2,
BRAP_IMG_Id_eAache_Decode_Stg3,
BRAP_IMG_Id_eAache_Decode_Stg4,
BRAP_IMG_Id_eAache_Decode_Table_Stg0,
BRAP_IMG_Id_eAache_Decode_Table_Stg1,
BRAP_IMG_Id_eAache_Decode_Table_Stg2,
BRAP_IMG_Id_eAache_Decode_Table_Stg3,
BRAP_IMG_Id_eAache_Decode_Table_Stg4,
BRAP_IMG_Id_eAache_Interframe_Stg0,
BRAP_IMG_Id_eAache_Interframe_Stg1,
BRAP_IMG_Id_eAache_Interframe_Stg2,
BRAP_IMG_Id_eAache_Interframe_Stg3,
BRAP_IMG_Id_eAache_Interframe_Stg4,
#endif
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
BRAP_Img_Id_eMsDdp_Be_Decode,
BRAP_Img_Id_eMsDdp_Fe_Decode,
BRAP_Img_Id_eMsDdp_Be_Decode_Tables,
BRAP_Img_Id_eMsDdp_Fe_Decode_Tables,
BRAP_Img_Id_eMsDdp_Be_Interframe,
BRAP_Img_Id_eMsDdp_Fe_Interframe,

#else
#ifdef RAP_AC3_SUPPORT
/*BRAP_Img_Id_eAc3_Ids,*/
BRAP_Img_Id_eAc3_Be_Decode,
BRAP_Img_Id_eAc3_Fe_Decode,
BRAP_Img_Id_eAc3_Be_Decode_Tables,
BRAP_Img_Id_eAc3_Fe_Decode_Tables,
BRAP_Img_Id_eAc3_Be_Interframe,
BRAP_Img_Id_eAc3_Fe_Interframe,
/*BRAP_Img_Id_eAc3_Ids_Interframe,*/
#endif

#ifdef RAP_DDP_SUPPORT
BRAP_Img_Id_eDdp_Be_Decode,
BRAP_Img_Id_eDdp_Fe_Decode,
BRAP_Img_Id_eDdp_Be_Decode_Tables,
BRAP_Img_Id_eDdp_Fe_Decode_Tables,
BRAP_Img_Id_eDdp_Be_Interframe,
BRAP_Img_Id_eDdp_Fe_Interframe,
#endif
#endif

#ifdef RAP_DTS_SUPPORT
BRAP_Img_Id_eDts_Ids,
BRAP_Img_Id_eDts_Ids_Interframe,
#endif

#if defined(RAP_DTSBROADCAST_SUPPORT) || defined(RAP_DTSHD_SUPPORT)
BRAP_Img_Id_eDtsBroadcast_Decode_Stg0,
BRAP_Img_Id_eDtsBroadcast_Decode_Table_Stg0,
BRAP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg0,
BRAP_Img_Id_eDtsBroadcast_Decode_Stg1,
BRAP_Img_Id_eDtsBroadcast_Decode_Table_Stg1,
BRAP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg1,
#endif

#ifdef RAP_LPCMBD_SUPPORT
#endif

#ifdef RAP_LPCMHDDVD_SUPPORT
#endif

#ifdef RAP_LPCMDVD_SUPPORT
BRAP_Img_Id_eLpcmDvd_Ids,
BRAP_Img_Id_eLpcmDvd_Decode,
BRAP_Img_Id_eLpcmDvd_Decode_Table,
BRAP_Img_Id_eLpcmDvd_Interframe,
BRAP_Img_Id_eLpcmDvd_Ids_Interframe,
#endif

#ifdef RAP_WMASTD_SUPPORT
BRAP_Img_Id_eWma_Ids,
BRAP_Img_Id_eWma_Decode,
BRAP_Img_Id_eWma_Decode_Table,
BRAP_Img_Id_eWma_Interframe,
BRAP_Img_Id_eWma_Ids_Interframe,
#endif

#ifdef RAP_AC3LOSSLESS_SUPPORT
#endif

#ifdef RAP_MLP_SUPPORT
#endif

#ifdef RAP_PCM_SUPPORT
BRAP_Img_Id_ePcm_Ids,
BRAP_Img_Id_ePcm_Interframe,
#endif

#ifdef RAP_DTSLBR_SUPPORT
#endif

#ifdef RAP_DDP7_1_SUPPORT
#endif

#ifdef RAP_MPEGMC_SUPPORT
#endif

#ifdef RAP_WMAPRO_SUPPORT
BRAP_Img_Id_eWmaPro_Ids,
BRAP_Img_Id_eWmaPro_Decode_Stg0,
BRAP_Img_Id_eWmaPro_Decode_Stg0_Table,
BRAP_Img_Id_eWmaPro_Decode_Stg1,
BRAP_Img_Id_eWmaPro_Decode_Stg1_Table,
BRAP_Img_Id_eWmaPro_Ids_Interframe,
BRAP_Img_Id_eWmaPro_Stg0_Interframe,
BRAP_Img_Id_eWmaPro_Stg1_Interframe,
#endif


#ifdef RAP_DDBM_SUPPORT
BRAP_Img_Id_eDdbm_Code,
BRAP_Img_Id_eDdbm_Table,
#endif

#ifdef RAP_DTSNEO_SUPPORT
BRAP_Img_Id_eDtsNeo_Code,
BRAP_Img_Id_eDtsNeo_Table,
BRAP_Img_Id_eDtsNeo_Interframe,
#endif

#ifdef RAP_AVL_SUPPORT
BRAP_Img_Id_eAvl_Code,
BRAP_Img_Id_eAvl_Table,
BRAP_Img_Id_eAvl_Interframe,
#endif

#ifdef RAP_PL2_SUPPORT
BRAP_Img_Id_ePL2_Code,
BRAP_Img_Id_ePL2_Table,
BRAP_Img_Id_ePL2_Interframe,
#endif

#ifdef RAP_SRSXT_SUPPORT
BRAP_Img_Id_eSrsXt_Code,
BRAP_Img_Id_eSrsXt_Table,
BRAP_Img_Id_eSrsXt_Interframe,
#endif

#ifdef RAP_SRSHD_SUPPORT
BRAP_Img_Id_eSrsHd_Code,
BRAP_Img_Id_eSrsHd_Table,
BRAP_Img_Id_eSrsHd_Interframe,
#endif

#ifdef RAP_SRSTRUVOL_SUPPORT
BRAP_Img_Id_eSrsTruVolume_Code,
BRAP_Img_Id_eSrsTruVolume_Table,
BRAP_Img_Id_eSrsTruVolume_Interframe,
#endif

#ifdef RAP_XEN_SUPPORT
BRAP_Img_Id_eXen_Code,
BRAP_Img_Id_eXen_Table,
BRAP_Img_Id_eXen_Interframe,
#endif

#ifdef RAP_BBE_SUPPORT
BRAP_Img_Id_eBbe_Code,
BRAP_Img_Id_eBbe_Table,
BRAP_Img_Id_eBbe_Interframe,
#endif

#ifdef RAP_CUSTOMSURROUND_SUPPORT
BRAP_Img_Id_eCustomSurround_Code,
BRAP_Img_Id_eCustomSurround_Table,
BRAP_Img_Id_eCustomSurround_Interframe,
#endif

#ifdef RAP_CUSTOMBASS_SUPPORT
BRAP_Img_Id_eCustomBass_Code,
BRAP_Img_Id_eCustomBass_Table,
BRAP_Img_Id_eCustomBass_Interframe,
#endif

#ifdef RAP_AACDOWNMIX_SUPPORT
#endif


#ifdef RAP_DTSENC_SUPPORT
BRAP_Img_Id_eDts_Encode_Code,
BRAP_Img_Id_eDts_Encode_Table,
BRAP_Img_Id_eDts_Encode_Interframe,
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
BRAP_Img_Id_eDolbyTranscode_Code,
BRAP_Img_Id_eDolbyTranscode_Table,
BRAP_Img_Id_eDolbyTranscode_Interframe,
#else
#ifdef RAP_AC3ENC_SUPPORT
BRAP_Img_Id_eAc3_Encode_Code,
BRAP_Img_Id_eAc3_Encode_Table,
BRAP_Img_Id_eAc3_Encode_Interframe,
#endif
#endif

#ifdef RAP_WMAPROPASSTHRU_SUPPORT
BRAP_Img_Id_eWmaPro_Passthru_Code,
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
BRAP_Img_Id_eMsDdp_Convert,
BRAP_Img_Id_eMsDdp_Convert_Tables,
BRAP_Img_Id_eMsDdp_Convert_Interframe,
#else
#ifdef RAP_DDP_TO_AC3_SUPPORT
BRAP_Img_Id_eDdp_Convert,
BRAP_Img_Id_eDdp_Convert_Tables,
BRAP_Img_Id_eDdp_Convert_Interframe,
#endif
#endif

#ifdef RAP_CUSTOMVOICE_SUPPORT
BRAP_Img_Id_eCustomVoice_Code,
BRAP_Img_Id_eCustomVoice_Tables,
BRAP_Img_Id_eCustomVoice_Interframe,
#endif
#ifdef RAP_SRC_SUPPORT
BRAP_Img_Id_eSrc_Code,
BRAP_Img_Id_eSrc_Tables,
BRAP_Img_Id_eSrc_Interframe,
#endif
#ifdef RAP_RFAUDIO_SUPPORT
BRAP_Img_Id_eRfAudio_Btsc,
BRAP_Img_Id_eRfAudio_Eiaj,
BRAP_Img_Id_eRfAudio_KoreaA2,
BRAP_Img_Id_eRfAudio_Nicam,
BRAP_Img_Id_eRfAudio_PalA2,
BRAP_Img_Id_eRfAudio_SecamL,
BRAP_Img_Id_eRfAudio_India,
#endif
#ifdef RAP_AUDIODESC_SUPPORT
BRAP_Img_Id_eFadeControl_Code,
/*BRAP_Img_Id_eFadeControl_Tables,
BRAP_Img_Id_eFadeControl_Interframe,*/
BRAP_Img_Id_ePanControl_Code,
BRAP_Img_Id_ePanControl_Tables,
BRAP_Img_Id_ePanControl_Interframe,
#endif
#ifdef RAP_PCMROUTER_SUPPORT
BRAP_Img_Id_ePCMRouter_Code,
#endif
#ifdef RAP_DSOLA_SUPPORT
BRAP_Img_Id_eDsola_Code,
BRAP_Img_Id_eDsola_Tables,
BRAP_Img_Id_eDsola_Interframe,
#endif
#ifdef RAP_DOLBYVOL_SUPPORT
BRAP_Img_Id_eDolbyVol_Code,
BRAP_Img_Id_eDolbyVol_Tables,
BRAP_Img_Id_eDolbyVol_Interframe,
#endif

#ifdef RAP_PCMWAV_SUPPORT
BRAP_Img_Id_ePcmWav_Code,
BRAP_Img_Id_ePcmWav_Code_Interframe,
BRAP_Img_Id_ePcmWav_Ids,
BRAP_Img_Id_ePcmWav_Ids_Interframe,
#endif

#ifdef RAP_MP3ENC_SUPPORT
BRAP_Img_Id_eMp3Enc_Code,
BRAP_Img_Id_eMp3Enc_Code_Tables,
BRAP_Img_Id_eMp3Enc_Code_Interframe,
#endif

#ifdef RAP_AMR_SUPPORT
BRAP_Img_Id_eAMR_Ids,
BRAP_Img_Id_eAMR_Decode,
BRAP_Img_Id_eAMR_Decode_Table,
BRAP_Img_Id_eAMR_Interframe,
BRAP_Img_Id_eAMR_Ids_Interframe,
#endif

#ifdef RAP_DRA_SUPPORT
BRAP_Img_Id_eDRA_Decode,
BRAP_Img_Id_eDRA_Decode_Table,
BRAP_Img_Id_eDRA_Interframe,
#endif

#ifdef RAP_SBCENC_SUPPORT
BRAP_Img_Id_eSbc_Encode_Code,
BRAP_Img_Id_eSbc_Encode_Table,
BRAP_Img_Id_eSbc_Encode_Interframe,
#endif

#ifdef  RAP_REALAUDIOLBR_SUPPORT
BRAP_Img_Id_eRealAudioLbr_Decode_Code,
BRAP_Img_Id_eRealAudioLbr_Decode_Table,
BRAP_Img_Id_eRealAudioLbr_Decode_Interframe,
#endif

#ifdef RAP_BRCM3DSURROUND_SUPPORT
BRAP_Img_Id_eBrcm3DSurround_Code,
BRAP_Img_Id_eBrcm3DSurround_Table,
BRAP_Img_Id_eBrcm3DSurround_Interframe,
#endif

#ifdef RAP_MONODOWNMIX_SUPPORT
BRAP_Img_Id_eMonoDownmix_Code,
#endif

#ifdef RAP_FWMIXER_SUPPORT
BRAP_Img_Id_eFwMixer_Ids,
BRAP_Img_Id_eFwMixer_Code,
BRAP_Img_Id_eFwMixer_Table,
BRAP_Img_Id_eFwMixer_Interframe,
BRAP_Img_Id_eFwMixer_Ids_Interframe,
#endif

#ifdef RAP_ADV_SUPPORT
BRAP_Img_Id_eAvd_Code,
BRAP_Img_Id_eAvd_Table,
BRAP_Img_Id_eAvd_Interframe, 
#endif

#ifdef RAP_ABX_SUPPORT
BRAP_Img_Id_eAbx_Code,
BRAP_Img_Id_eAbx_Table,
BRAP_Img_Id_eAbx_Interframe, 
#endif

#ifdef RAP_SRSCSTD_SUPPORT
BRAP_Img_Id_eSrsCsTd_Code,
BRAP_Img_Id_eSrsCsTd_Table,
BRAP_Img_Id_eSrsCsTd_Interframe,
#endif

#ifdef RAP_SRSEQHL_SUPPORT
BRAP_Img_Id_eSrsEqHl_Code,
BRAP_Img_Id_eSrsEqHl_Table,
BRAP_Img_Id_eSrsEqHl_Interframe,
#endif

#ifdef RAP_DV258_SUPPORT
BRAP_Img_Id_eDV258_Code,
BRAP_Img_Id_eDV258_Table,
BRAP_Img_Id_eDV258_Interframe, 
#endif
#ifdef RAP_DDRE_SUPPORT
BRAP_Img_Id_eDDRE_Code,
BRAP_Img_Id_eDDRE_Table,
BRAP_Img_Id_eDDRE_Interframe, 
#endif



#ifdef RAP_GFX_SUPPORT
BRAP_Img_Id_egfx_Code,
BRAP_Img_Id_egfx_Table,
BRAP_Img_Id_egfx_Interframe,
#endif
#ifdef RAP_REALVIDEO_SUPPORT
BRAP_Img_Id_eRealVideo_Decode_Stg1,
BRAP_Img_Id_eRealVideo_Decode_Stg2,
BRAP_Img_Id_eRealVideo_Decode_Stg3,
BRAP_Img_Id_eRealVideo_Decode_Stg1_Table,
BRAP_Img_Id_eRealVideo_Decode_Stg2_Table,
BRAP_Img_Id_eRealVideo_Decode_Stg3_Table,
BRAP_Img_Id_eRealVideo_Ppd,
BRAP_Img_Id_eRealVideo_Plf,
BRAP_Img_Id_eRealVideo_Decode_Interframe,
BRAP_Img_Id_eRealVideo_Plf_Table,
#endif
#ifdef RAP_BTSC_SUPPORT
BRAP_Img_Id_eBtsc_Code,
BRAP_Img_Id_eBtsc_Table,
BRAP_Img_Id_eBtsc_Interframe,
#endif
BRAP_Img_Id_eInvalid
}BRAP_Img_Id;

#define BRAP_MAX_IMG_ID BRAP_Img_Id_eInvalid

/* 
	This chunk size will be used when the firmware binary is actually present in multiple chunks.
	The BRAP_IMG_CHUNK_SIZE will then give the size of each such chunk
*/
#define BRAP_IMG_CHUNK_SIZE	65532


extern void *BRAP_IMG_Context;
extern const BIMG_Interface BRAP_IMG_Interface;

#endif /* BRAP_IMG_H__ */


