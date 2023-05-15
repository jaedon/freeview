/***************************************************************************
*     Copyright (c) 2006-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dsp.c $
* $brcm_Revision: Hydra_Software_Devel/127 $
* $brcm_Date: 4/18/11 1:51p $
*
* Module Description:
*	This file contains high level functions related to DSP and firmware.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_dsp.c $
* 
* Hydra_Software_Devel/127   4/18/11 1:51p gautamk
* SW7420-1791: [7405] iface->close should not be called if it is not
* open.
* 
* Hydra_Software_Devel/126   4/18/11 1:43p gautamk
* SW7420-1791: [7405] iface->close should not be called if it is not
* open.
* 
* Hydra_Software_Devel/125   3/21/11 5:17p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/124   3/18/11 9:39p sgadara
* SW7420-767: [7420] Merge MS11 development branch to mainline
* 
* Hydra_Software_Devel/123   3/7/11 6:02p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/122   10/21/10 7:08p srajapur
* SW7550-601 : [7550] Adding PI Support for BTSC Encoder
* 
* Hydra_Software_Devel/121   9/16/10 12:17a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/120   9/6/10 5:14p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/119   9/6/10 3:43p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code.
* Merging to mainline
* 
* Hydra_Software_Devel/RAP_7550_TASK_DOWNLOAD_BRANCH/2   8/31/10 3:39p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code
* 
* Hydra_Software_Devel/RAP_7550_TASK_DOWNLOAD_BRANCH/1   8/31/10 3:36p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code
* 
* Hydra_Software_Devel/118   8/13/10 5:23p gautamk
* SW7405-4713:[7405] Merging changes for RDB indepedence in Main line
* 
* Hydra_Software_Devel/117   8/10/10 1:52p gautamk
* SW7420-767: [7420] Merging MS11 feature support to mainline
* 
* Hydra_Software_Devel/116   5/24/10 8:26p sgadara
* SW3548-2923: [3548,3556] Adding Support for SRS StudioSound.
* StudioSound  --> CsTd+TSHD+TVOL+EqHl
* 
* Hydra_Software_Devel/115   4/26/10 1:58p srajapur
* SW3548-2899 : [3548] Added PI support for ABX post processing
* 
* Hydra_Software_Devel/114   4/22/10 8:58p srajapur
* SW7405-3993 : [3548] Adding Audyssey Volume post processing algorithm
* 
* Hydra_Software_Devel/113   4/22/10 5:11p sgadara
* SW7405-4170:[7405,3548,3556] Making chages to internally map AACHE and
* AC3 Encode to Dolby Pulse and DD Transcode in PI for MS10 license
* 
* Hydra_Software_Devel/112   4/19/10 7:10p gautamk
* SW7405-4170:[7405] Adding Support for MS10 DDP/AC3
* 
* Hydra_Software_Devel/111   4/6/10 2:37p gautamk
* SW7405-4170: [7405] Adding FWMixer processing stage.
* 
* Hydra_Software_Devel/110   3/22/10 7:25p sgadara
* SW3548-2827: [3548,3556] Correct the mapping of Image Id to Exec Id
* 
* Hydra_Software_Devel/109   3/12/10 2:45p speter
* SW3548-2827:[3548] Adding Mono Downmix support as post processing
* algorithm
* 
* Hydra_Software_Devel/108   3/11/10 5:00p speter
* SW3548-2822:[3548] Add Dolby Transcode and Broadcom 3D surround post
* processing algos
* 
* Hydra_Software_Devel/107   3/10/10 5:04p speter
* SW3548-2819:[3548] Adding Dolby Pulse Algorithm
* 
* Hydra_Software_Devel/106   2/19/10 5:19p gautamk
* SW7468-119: [7468] Adding RAP PI support for Real audio LBR
* 
* Hydra_Software_Devel/105   11/19/09 2:28p gautamk
* SW7405-3063: [7405] Support for DTS HD
* 
* Hydra_Software_Devel/104   11/17/09 12:50p gautamk
* SW7405-2001: [7405] removing entries of WMAPRo passthru
* table/interframe.
* 
* Hydra_Software_Devel/103   11/10/09 2:30p speter
* SW7405-2959: [3548] Adding DTS-HD support
* 
* Hydra_Software_Devel/102   9/23/09 6:10p gautamk
* SW7420-357: [7420] Adding support for SBC Encode
* 
* Hydra_Software_Devel/101   9/23/09 1:13p gautamk
* SW7405-3063: [7405] defining Macros for Passthru. Enabling them in
* default in .inc file for chips which supports them.
* 
* Hydra_Software_Devel/100   9/22/09 5:49p gautamk
* SW7405-3058: [7405] Adding support for DRA decoder.
* 
* Hydra_Software_Devel/99   9/11/09 7:56p sgadara
* SW3548-2188: [3548,3556] Adding support for AMR decode algorithm.
* 
* Hydra_Software_Devel/98   9/11/09 4:45p gautamk
* SW7405-2986: [7405] Enabling Passthru without license
* 
* Hydra_Software_Devel/97   7/30/09 4:41p gautamk
* PR56006: [7405] Boot DSP when watchdog is trigerred in case of
* BFwAuthEnabled
* 
* Hydra_Software_Devel/96   7/23/09 4:48p bhanus
* PR 55499 : [7340,7342,7125]Adding 7420 Family support
* 
* Hydra_Software_Devel/95   7/15/09 11:04a gautamk
* PR56006: [7405] Adding support for mp3 encode in rap pi
* 
* Hydra_Software_Devel/94   6/23/09 3:52p gautamk
* PR56006: [7506]merging AACHEV2 to mainline
* 
* Hydra_Software_Devel/REFSW_RAP_7405_AACV2/1   6/15/09 2:36p gautamk
* PR56006: [7506] PI support for AACV2
* 
* Hydra_Software_Devel/93   6/12/09 3:44p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/92   6/9/09 11:36a gautamk
* PR47949: [7405]Changing name from PCMOnAVI to PCMWAV
* 
* Hydra_Software_Devel/91   6/4/09 3:04p gautamk
* PR47949: [7405] Adding PI support for PCM On Avi
* 
* Hydra_Software_Devel/90   5/21/09 12:30p gautamk
* PR52383: [7405] Allocating DBG buffer of size 4, when it is disabled.
* 
* Hydra_Software_Devel/89   5/11/09 3:52p gautamk
* PR52383: [7405] Adding algorithm support for DTS decoder.
* 
* Hydra_Software_Devel/88   4/20/09 3:55p gautamk
* PR52383: [7405] Creating seperate event handles for each task.
* 
* Hydra_Software_Devel/87   4/1/09 4:38p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/86   3/18/09 9:50p gautamk
* PR53365: [7405] Adding PI support for dolby volume
* 
* Hydra_Software_Devel/85   2/13/09 1:33p speter
* PR 52090: [3548,3556] Merging the Phase 4.2 Take 2 release to main line
* 
* Hydra_Software_Devel/RAP_354856_PH_4_3/1   2/10/09 12:05p speter
* PR 46135: [3548,3556] Adding SRS Tru Volume Support - Chaning the name
* to TruVolume
* 
* Hydra_Software_Devel/84   1/22/09 5:27p gautamk
* PR50455: [7405] Setting VOLUME STEP RAMP to 0x20 when DSP FW triggers
* EnableRamp interrupt.
* 
* Hydra_Software_Devel/83   1/13/09 9:16p bhanus
* PR 47757: [7420] Updated firmware data memory download address to
* 0x8020000 for ZSP 800
* 
* Hydra_Software_Devel/82   12/23/08 1:32p gautamk
* PR48850: [7405]  adding support for DVD LPCM. Merging to mainline
* 
* Hydra_Software_Devel/81   12/22/08 11:29a speter
* PR 46135: [3548,3556] Adding SRS VIQ Support
* 
* Hydra_Software_Devel/80   11/25/08 10:19a speter
* PR 49564: [3548,3556] Merging the Phase 4.0 release to main line
* 
* Hydra_Software_Devel/77   11/10/08 1:56p gautamk
* PR43454: [7405] allocating  FW Debug buffer as per export variable
* export FW_DBG=y. Because in the release executable the mem for debug
* buffer sshould not get allocated.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_0/1   11/5/08 7:04p sgadara
* PR 40715: [3548,3556] Adding support for new AAC decoder.
* 
* Hydra_Software_Devel/76   10/28/08 3:58a speter
* PR 48263: [3548,3556] Adding Stop DSP code to avoid GISB Timeout error
* that happens as the DSP is active after memory deallocation.
* 
* Hydra_Software_Devel/75   10/21/08 11:42p speter
* PR 46051: [3548,3556] Changing DDEnc to AC3Enc
* 
* Hydra_Software_Devel/74   10/10/08 10:00a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   10/1/08 3:18p speter
* PR 46051: [3548,3556] Changing DDEnc to AC3Enc
* 
* Hydra_Software_Devel/73   9/22/08 11:31a speter
* PR 38950: [3548,3556] Adding support for SPDIF In. Adding DTS Passthru
* support
* 
* Hydra_Software_Devel/72   9/16/08 2:43p sushmit
* PR 43454:[7405]Adding AC3 Encode Support.
* 
* Hydra_Software_Devel/71   9/4/08 11:01p speter
* PR 46565: [3548,3556] Adding SRS-HD support
* 
* Hydra_Software_Devel/70   8/28/08 12:20p speter
* PR 46047: [3548,3556] Adding support for Custom Bass and Custom
* Surround
* 
* Hydra_Software_Devel/69   8/21/08 12:10p sushmit
* PR 43454:[7405]Updating AD Files.
* 
* Hydra_Software_Devel/68   8/12/08 11:00p sushmit
* PR 43454:[7405]Adding DSOLA.
* 
* Hydra_Software_Devel/67   8/10/08 11:55a sushmit
* PR 43454:[7405] DTSENC Updates.
* 
* Hydra_Software_Devel/66   7/28/08 9:28a speter
* PR 45171: [3548,3556] Merging from Phase 2.0 release to main line of
* development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/65   7/23/08 9:31p gautamk
* PR43454: [7405] Adding PI support for WMA Passthru
* 
* Hydra_Software_Devel/64   7/23/08 3:56p gautamk
* PR43454: [7405] inserting PCMrouter satge interanally for AD Fade
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/1   7/24/08 4:54p speter
* PR 43454: [3548,3556] Adding PCM router post processing algorithm
* 
* Hydra_Software_Devel/63   7/22/08 12:18p speter
* PR 45032: [3548,3556] Adding PCM algorithm support
* 
* Hydra_Software_Devel/62   7/10/08 5:08p speter
* PR 44701: [3548,3556] Adding support for post processing algorithms
* PL2, SRSXT and AVL
* 
* Hydra_Software_Devel/61   6/6/08 4:52p gautamk
* PR41726: [7405] not making tsm status buffer invalid in dspchn_start(),
* if its getting called from internal function like FlushChannel()
* 
* Hydra_Software_Devel/60   5/29/08 2:12a speter
* PR 38950: [3548,3556] Adding Audio Descriptor support
* 
* Hydra_Software_Devel/59   5/28/08 11:38p gautamk
* PR41726: [7405] Modifying files for AAC/AAC-HE LOAS/ADTS
* 
* Hydra_Software_Devel/58   5/28/08 8:11p gautamk
* PR41726: [7405] Adding support for SRC postprocessing
* 
* Hydra_Software_Devel/57   5/23/08 1:13p gautamk
* PR42302: [7325] Fixing the race condition between MIPS and DSP for
* updating ET bit of wachdog timer.
* PR42807: [7335]
* PR42837: [7325]
* 
* Hydra_Software_Devel/56   5/2/08 6:59p gautamk
* PR41726: [7405] modification for wma pro
* 
* Hydra_Software_Devel/55   5/2/08 2:40p gautamk
* PR41726: [7405] Adding PI support for Fw Verification in mainline
* 
* Hydra_Software_Devel/54   4/25/08 5:15p gautamk
* PR41726: [7405] PI support for WMA PRO
* 
* Hydra_Software_Devel/53   4/23/08 8:03p gautamk
* PR42092: [7405] Fixing the check for return value of BRap_Allocaligned.
* 
* Hydra_Software_Devel/52   4/22/08 4:56p speter
* PR 38950: [3548] Adding CustomVoice (Post Processing) Support
* 
* Hydra_Software_Devel/51   4/11/08 5:14p sushmit
* PR 41365: [7405] Allowing only AC3 Passthru without license.
* 
* Hydra_Software_Devel/50   4/8/08 2:17p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/4   4/8/08 1:41p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/3   4/8/08 1:18p gautamk
* PR34648: [7405] adding code for wdog...
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/2   4/1/08 2:20p gautamk
* PR34648: [7405] Merging change to wdog branch
* 
* Hydra_Software_Devel/46   3/31/08 12:12p nitinb
* PR 34648: [7405] Implemented review comments: Removed unused handle
* hArc from memory read/write functions. Removed function
* BRAP_P_DummyMemRead32().
* 
* Hydra_Software_Devel/45   3/13/08 2:44p sushmit
* PR 34648: [7405] Checking in AACHE Support Changes
* 
* Hydra_Software_Devel/44   3/7/08 9:58p sushmit
* PR 34648: [7405] Updating AACHE Support
* 
* Hydra_Software_Devel/43   2/29/08 4:22p gautamk
* PR34648: [7405] Changing file names includes
* 
* Hydra_Software_Devel/42   2/22/08 4:17p gautamk
* PR34648: [7405] fixing coverity defects + implementing review comments
* 
* Hydra_Software_Devel/41   2/15/08 8:44p gautamk
* PR34648: [7405]Adding User Config for Passthru
* Adding spdif CBit buffer.
* 
* Hydra_Software_Devel/40   2/11/08 7:02p gautamk
* PR34648: [7405] Adding PI Support for DDP.
* 
* Hydra_Software_Devel/39   2/11/08 3:06p gautamk
* PR34648: [7405]: Fixing compilation error for TSM
* 
* Hydra_Software_Devel/38   2/11/08 3:02p gautamk
* PR34648: [7405]: Fixing TSM Comilation.
* 
* Hydra_Software_Devel/37   2/11/08 2:28p gautamk
* PR34648: [7405] Adding FreeCtxBuffer Function to free the buffer usage
* at DSPCHN_STOP.
* Adding support for Decode TSM
* 
* Hydra_Software_Devel/36   2/7/08 5:37p nitinb
* PR34648: [7405] Fixed compilation warnings.
* 
* Hydra_Software_Devel/35   1/31/08 5:22p gautamk
* PR34648: [7405] Removing BRAP_DSPCHN_DecodeMode_eSimulMode Enum
* 
* Hydra_Software_Devel/34   1/30/08 4:35p gautamk
* PR34648: [7405] Updating design for downloading FW. Now download FW ids
* are coming from common arrary between PI/FW
* 
* Hydra_Software_Devel/33   1/29/08 7:09p gdata
* PR34648: [7405] Adding support for close calls
* 
* Hydra_Software_Devel/32   1/28/08 3:12p gautamk
* PR34648: [7405] removing unwanted structure and includes
* 
* Hydra_Software_Devel/31   1/24/08 9:04p sushmit
* PR 34648: [7405] Updaing Passthru Bringup PI Code
* 
* Hydra_Software_Devel/30   1/24/08 4:05p sushmit
* PR34648: [7405] Updating code for Mpeg bringup
* 
* Hydra_Software_Devel/29   1/22/08 1:35p sushmit
* PR 34648: [7405] AC3 Bringup Changes.
* 
* Hydra_Software_Devel/28   1/21/08 11:50a sushmit
* PR 34648: [7405] AC3 Bringup Changes.
* Change AUD_START_TASK_ID_OFFSET
* Convert MIT Addrs to physical addresses
* Fix FIFO Addr Table Generation
* Take care of AC3 IDS instead of AC3 FrameSync
* Take care of AC3 IDS Interframe requirement
* Convert CIT Virt Addr to Phys Addrs
* 
* Hydra_Software_Devel/27   1/18/08 4:42p sushmit
* PR34648: [7405] Fixing Ping Command ACK L1 Mask Clearing issue
* 
* Hydra_Software_Devel/26   1/18/08 11:55a gautamk
* PR34648: [7405] updating files for initializing interframe and user
* config params.
* 
***************************************************************************/


#include "brap.h"
#include "brap_priv.h"
#include "brap_dsp.h"
#include "brap_dspchn_priv.h"
#include "brap_img.h"
#ifdef RAP_SCM_SUPPORT
#include "brap_sha1_priv.h"
#include "brap_scm.h"
#include "brap_scm_priv.h"
#endif

BDBG_MODULE(rap_dsp);

#if (BRAP_7420_FAMILY == 1)
#define BRAP_P_ZSP800
#endif

#define BRAP_DSP_P_PREBOOT_MAILBOX_PATTERN			0xA5A5
#define BRAP_DSP_P_DMA_BUSY							1
#define BRAP_DSP_P_DMA_TIMEOUT_COUNT				500


static BERR_Code BRAP_DSP_P_DmaIn(	BRAP_DSP_Handle hDsp,
								uint8_t dataType,
								uint8_t swapType,
								uint32_t size,
								uint32_t src_addr,
								uint32_t dst_addr);

#ifdef RAP_SCM_SUPPORT

static BERR_Code BRAP_DSP_P_DmaOut(	BRAP_DSP_Handle hDsp,
								uint8_t dataType,
								uint8_t swapType,
								uint32_t size,
								uint32_t src_addr,
								uint32_t dst_addr);

#endif


static uint32_t BRAP_DSP_P_ResetHardware(BRAP_DSP_Handle hDsp);

void BRAP_MapImgIdWithExecId(BRAP_Handle hRap,BRAP_ImgIdMappingArrays *psImgIdMappingArrays)
{
	uint32_t count = 0, size = 0;

    BDBG_ENTER(BRAP_MapImgIdWithExecId);
	BDBG_ASSERT(hRap);
	
	for(count = 0; count < BRAP_AF_P_AlgoId_eMax; count++)
	{
		psImgIdMappingArrays->ui32CodeImgId[count] = BRAP_MAX_IMG_ID;
		psImgIdMappingArrays->ui32TableImgId[count] = BRAP_MAX_IMG_ID;	
		psImgIdMappingArrays->ui32CodeSize[count] = 0;
		psImgIdMappingArrays->ui32TableSize[count] = 0;		
		psImgIdMappingArrays->ui32InterframeImgId[count]= BRAP_MAX_IMG_ID;
		psImgIdMappingArrays->ui32InterframeSize[count]= 0;		
	}

/* Mapping Ids For TSM Exec	And Storing the sizes as well*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDecodeTsm] = BRAP_Img_Id_eDecodeTsm;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eEncodeTsm] = BRAP_Img_Id_eEncodeTsm;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_ePassthroughTsm] = BRAP_Img_Id_ePassthruTsm;

	
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDecodeTsm, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDecodeTsm] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eEncodeTsm, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eEncodeTsm] = BRAP_P_ALIGN(size, 2);
	
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePassthruTsm, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_ePassthroughTsm] = BRAP_P_ALIGN(size, 2);
	
        psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDecodeTsm] = BRAP_Img_Id_eDecodeTsmInterframe;
        BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDecodeTsmInterframe, &size);
        psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDecodeTsm]= BRAP_P_ALIGN(size, 2);            
	
/* Mapping Ids For System task Code And Storing the sizes as well*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSystemDecodeTask] = BRAP_Img_Id_eSystemAudioTask;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSystemGraphicTask] = BRAP_Img_Id_eSystemGfxTask;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSystemVideoTask] = BRAP_Img_Id_eSystemVideoTask;

    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSystemAudioTask, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSystemDecodeTask] = BRAP_P_ALIGN(size, 2);

    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSystemGfxTask, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSystemGraphicTask] = BRAP_P_ALIGN(size, 2);

    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSystemVideoTask, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSystemVideoTask] = BRAP_P_ALIGN(size, 2);
    
#ifdef 	RAP_SCM_SUPPORT
	/*Secure Code Module */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSCMTask] = BRAP_Img_Id_eSCMTask;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSCMTask, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSCMTask]= BRAP_P_ALIGN(size, 2);
#endif	
	

/* Mapping ids for Passthru */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_ePassThru] = BRAP_Img_Id_ePassthruCode;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_ePassThru] = BRAP_Img_Id_ePassthruTable;
	
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePassthruCode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_ePassThru] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePassthruTable, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_ePassThru] = BRAP_P_ALIGN(size, 2);

    psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_ePassThru] = BRAP_Img_Id_ePassthruInterFrame;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePassthruInterFrame, &size);
    psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_ePassThru]= BRAP_P_ALIGN(size, 2);     

    BDBG_MSG(("psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_ePassThru] = %d",psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_ePassThru] ));
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMs10DdpFrameSync] = (uint32_t)BRAP_Img_Id_eMsDdp_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMs10DdpFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMs10DdpFrameSync] = BRAP_Img_Id_eMsDdp_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMs10DdpFrameSync]= BRAP_P_ALIGN(size, 2);
#else
#ifdef  RAP_AC3_PASSTHRU_SUPPORT         
/* AC3 FrameSync for using AC3 Passthru without license*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAc3FrameSync] = (uint32_t)BRAP_Img_Id_eAc3_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAc3FrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAc3FrameSync] = BRAP_Img_Id_eAc3_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAc3FrameSync]= BRAP_P_ALIGN(size, 2);

#endif
#ifdef  RAP_DDP_PASSTHRU_SUPPORT         

/* DDP FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDdpFrameSync] = (uint32_t)BRAP_Img_Id_eDdp_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDdpFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDdpFrameSync] = BRAP_Img_Id_eDdp_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDdpFrameSync]= BRAP_P_ALIGN(size, 2);

#endif
#endif

#ifdef  RAP_DTSBROADCAST_PASSTHRU_SUPPORT         
/* DTS Broadcast FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDtsFrameSync] = (uint32_t)BRAP_Img_Id_eDtsBroadcast_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsBroadcast_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDtsFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDtsFrameSync] = BRAP_Img_Id_eDtsBroadcast_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsBroadcast_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDtsFrameSync]= BRAP_P_ALIGN(size, 2);    

#endif

#ifdef  RAP_DTSHD_PASSTHRU_SUPPORT
/* DTS HD FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDtsHdFrameSync] = (uint32_t)BRAP_Img_Id_eDtsHd_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsHd_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDtsHdFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDtsHdFrameSync] = BRAP_Img_Id_eDtsHd_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsHd_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDtsHdFrameSync]= BRAP_P_ALIGN(size, 2);    

#endif


#ifdef  RAP_MPEG_PASSTHRU_SUPPORT         
/* MPEG FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMpegFrameSync] = (uint32_t)BRAP_Img_Id_eMpeg1_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMpeg1_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMpegFrameSync] = BRAP_P_ALIGN(size, 2);    

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMpegFrameSync] = BRAP_Img_Id_eMpeg1_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMpeg1_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMpegFrameSync]= BRAP_P_ALIGN(size, 2);    
#endif

#ifdef RAP_AACSBR_PASSTHRU_SUPPORT
/* AAC-HE ADTS FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAdtsFrameSync] = (uint32_t)BRAP_IMG_Id_eAache_Adts_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Adts_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAdtsFrameSync] = BRAP_P_ALIGN(size, 2);


/* AAC-HE LOAS FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eLoasFrameSync] = (uint32_t)BRAP_IMG_Id_eAache_Loas_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Loas_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eLoasFrameSync] = BRAP_P_ALIGN(size, 2);


/* AAC-HE ADTS FS Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAdtsFrameSync] = BRAP_IMG_Id_eAache_Adts_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Adts_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAdtsFrameSync]= BRAP_P_ALIGN(size, 2);

/* AAC-HE LOAS FS Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eLoasFrameSync] = BRAP_IMG_Id_eAache_Loas_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Loas_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eLoasFrameSync]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef  RAP_DRA_PASSTHRU_SUPPORT       
    /* DRA FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDraFrameSync] = (uint32_t)BRAP_Img_Id_eDRA_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDRA_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDraFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDraFrameSync] = BRAP_Img_Id_eDRA_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDRA_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDraFrameSync]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef  RAP_REALAUDIOLBR_PASSTHRU_SUPPORT       
    /* RA LBR FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eRealAudioLbrFrameSync] = (uint32_t)BRAP_Img_Id_eRealAudioLbr_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealAudioLbr_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eRealAudioLbrFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eRealAudioLbrFrameSync] = BRAP_Img_Id_eRealAudioLbr_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealAudioLbr_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eRealAudioLbrFrameSync]= BRAP_P_ALIGN(size, 2);
#endif








#ifdef 	RAP_MPEG_SUPPORT

/* MPEG Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMpegDecode] = (uint32_t)BRAP_Img_Id_eMpeg1_Decode;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMpeg1_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMpegDecode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eMpegDecode] = (uint32_t)BRAP_Img_Id_eMpeg1_Decode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMpeg1_Decode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eMpegDecode] = BRAP_P_ALIGN(size, 2);

/* MPEG Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMpegDecode] = BRAP_Img_Id_eMpeg1_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMpeg1_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMpegDecode]= BRAP_P_ALIGN(size, 2);
#endif	
    	

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
/* MS10 Ddp Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1] = (uint32_t)BRAP_Img_Id_eMsDdp_Fe_Decode;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2] = (uint32_t)BRAP_Img_Id_eMsDdp_Be_Decode;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Fe_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1] = BRAP_P_ALIGN(size, 2);
	
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Be_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2] = BRAP_P_ALIGN(size, 2);


	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1] = (uint32_t)BRAP_Img_Id_eMsDdp_Fe_Decode_Tables;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2] = (uint32_t)BRAP_Img_Id_eMsDdp_Be_Decode_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Fe_Decode_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Be_Decode_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2] = BRAP_P_ALIGN(size, 2);

/* MS10 Ddp Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1] = BRAP_Img_Id_eMsDdp_Fe_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Fe_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1]= BRAP_P_ALIGN(size, 2);
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2] = BRAP_Img_Id_eMsDdp_Be_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Be_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2]= BRAP_P_ALIGN(size, 2);

\
/* MS10DD Convert Code & Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMs10DDConvert] = (uint32_t)BRAP_Img_Id_eMsDdp_Convert;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Convert, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMs10DDConvert] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eMs10DDConvert] = (uint32_t)BRAP_Img_Id_eMsDdp_Convert_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Convert_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eMs10DDConvert] = BRAP_P_ALIGN(size, 2);

/* MS10DD Convert Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMs10DDConvert] = BRAP_Img_Id_eMsDdp_Convert_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMsDdp_Convert_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMs10DDConvert]= BRAP_P_ALIGN(size, 2);

#else
#ifdef RAP_AC3_SUPPORT

/* AC3 Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAc3DecodeStage1] = (uint32_t)BRAP_Img_Id_eAc3_Fe_Decode;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAc3DecodeStage2] = (uint32_t)BRAP_Img_Id_eAc3_Be_Decode;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Fe_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAc3DecodeStage1] = BRAP_P_ALIGN(size, 2);
	
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Be_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAc3DecodeStage2] = BRAP_P_ALIGN(size, 2);


	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAc3DecodeStage1] = (uint32_t)BRAP_Img_Id_eAc3_Fe_Decode_Tables;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAc3DecodeStage2] = (uint32_t)BRAP_Img_Id_eAc3_Be_Decode_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Fe_Decode_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAc3DecodeStage1] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Be_Decode_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAc3DecodeStage2] = BRAP_P_ALIGN(size, 2);

/* AC3 Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAc3DecodeStage1] = BRAP_Img_Id_eAc3_Fe_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Fe_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAc3DecodeStage1]= BRAP_P_ALIGN(size, 2);
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAc3DecodeStage2] = BRAP_Img_Id_eAc3_Be_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Be_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAc3DecodeStage2]= BRAP_P_ALIGN(size, 2);
	
/*	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAc3FrameSync] = BRAP_Img_Id_eAc3_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAc3FrameSync]= BRAP_P_ALIGN(size, 2);*/
#endif

#ifdef RAP_DDP_SUPPORT


/* Ddp Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDdpDecodeStage1] = (uint32_t)BRAP_Img_Id_eDdp_Fe_Decode;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDdpDecodeStage2] = (uint32_t)BRAP_Img_Id_eDdp_Be_Decode;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Fe_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDdpDecodeStage1] = BRAP_P_ALIGN(size, 2);
	
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Be_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDdpDecodeStage2] = BRAP_P_ALIGN(size, 2);


	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDdpDecodeStage1] = (uint32_t)BRAP_Img_Id_eDdp_Fe_Decode_Tables;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDdpDecodeStage2] = (uint32_t)BRAP_Img_Id_eDdp_Be_Decode_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Fe_Decode_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDdpDecodeStage1] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Be_Decode_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDdpDecodeStage2] = BRAP_P_ALIGN(size, 2);

/* Ddp Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDdpDecodeStage1] = BRAP_Img_Id_eDdp_Fe_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Fe_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDdpDecodeStage1]= BRAP_P_ALIGN(size, 2);
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDdpDecodeStage2] = BRAP_Img_Id_eDdp_Be_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Be_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDdpDecodeStage2]= BRAP_P_ALIGN(size, 2);
	
#endif
#ifdef RAP_DDP_TO_AC3_SUPPORT
/* DD Convert Code & Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDDConvert] = (uint32_t)BRAP_Img_Id_eDdp_Convert;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Convert, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDDConvert] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDDConvert] = (uint32_t)BRAP_Img_Id_eDdp_Convert_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Convert_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDDConvert] = BRAP_P_ALIGN(size, 2);

/* DD Convert Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDDConvert] = BRAP_Img_Id_eDdp_Convert_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDdp_Convert_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDDConvert]= BRAP_P_ALIGN(size, 2);
#endif


#endif

#ifdef RAP_DTS_SUPPORT
/*  DTS FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDtsFrameSync] = (uint32_t)BRAP_Img_Id_eDts_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDts_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDtsFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDtsFrameSync] = BRAP_Img_Id_eDts_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDts_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDtsFrameSync]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
/* Dolby Pulse Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0] = (uint32_t)BRAP_IMG_Id_eDolbyPulse_Decode_Stg0;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1] = (uint32_t)BRAP_IMG_Id_eDolbyPulse_Decode_Stg1;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2] = (uint32_t)BRAP_IMG_Id_eDolbyPulse_Decode_Stg2;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3] = (uint32_t)BRAP_IMG_Id_eDolbyPulse_Decode_Stg3;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Decode_Stg0, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Decode_Stg1, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Decode_Stg2, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Decode_Stg3, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0] = (uint32_t)BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg0;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1] = (uint32_t)BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg1;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2] = (uint32_t)BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg2;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3] = (uint32_t)BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg3;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg0, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg1, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg2, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Decode_Table_Stg3, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3] = BRAP_P_ALIGN(size, 2);

/* Dolby Pulse Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0] = BRAP_IMG_Id_eDolbyPulse_Interframe_Stg0;
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1] = BRAP_IMG_Id_eDolbyPulse_Interframe_Stg1;
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2] = BRAP_IMG_Id_eDolbyPulse_Interframe_Stg2;    
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3] = BRAP_IMG_Id_eDolbyPulse_Interframe_Stg3;    
    
	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Interframe_Stg0, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0]= BRAP_P_ALIGN(size, 2);
	

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Interframe_Stg1, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1]= BRAP_P_ALIGN(size, 2);
	

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Interframe_Stg2, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2]= BRAP_P_ALIGN(size, 2);
	

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eDolbyPulse_Interframe_Stg3, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3]= BRAP_P_ALIGN(size, 2);	
#else
#ifdef RAP_AACSBR_SUPPORT
/* AAC-HE Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Stg0;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Stg1;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Stg2;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Stg3;
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Stg4;    

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Stg0, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Stg1, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Stg2, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Stg3, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Stg4, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4] = BRAP_P_ALIGN(size, 2);    

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Table_Stg0;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Table_Stg1;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Table_Stg2;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Table_Stg3;
	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4] = (uint32_t)BRAP_IMG_Id_eAache_Decode_Table_Stg4;    

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Table_Stg0, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Table_Stg1, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Table_Stg2, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2] = BRAP_P_ALIGN(size, 2);

    	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Table_Stg3, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3] = BRAP_P_ALIGN(size, 2);

	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Decode_Table_Stg4, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4] = BRAP_P_ALIGN(size, 2);

/* AAC-HE Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0] = BRAP_IMG_Id_eAache_Interframe_Stg0;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Interframe_Stg0, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0]= BRAP_P_ALIGN(size, 2);
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1] = BRAP_IMG_Id_eAache_Interframe_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Interframe_Stg1, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1]= BRAP_P_ALIGN(size, 2);
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2] = BRAP_IMG_Id_eAache_Interframe_Stg2;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Interframe_Stg2, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2]= BRAP_P_ALIGN(size, 2);
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3] = BRAP_IMG_Id_eAache_Interframe_Stg3;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Interframe_Stg3, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3]= BRAP_P_ALIGN(size, 2);	
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4] = BRAP_IMG_Id_eAache_Interframe_Stg4;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_IMG_Id_eAache_Interframe_Stg4, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4]= BRAP_P_ALIGN(size, 2);	    
#endif
#endif

#ifdef 	RAP_WMASTD_SUPPORT
/* WMA STD FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eWmaStdFrameSync] = (uint32_t)BRAP_Img_Id_eWma_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWma_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eWmaStdFrameSync] = BRAP_P_ALIGN(size, 2);

/* WMA STD Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eWmaStdDecode] = (uint32_t)BRAP_Img_Id_eWma_Decode;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWma_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eWmaStdDecode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eWmaStdDecode] = (uint32_t)BRAP_Img_Id_eWma_Decode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWma_Decode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eWmaStdDecode] = BRAP_P_ALIGN(size, 2);

/* WMA STD Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eWmaStdDecode] = BRAP_Img_Id_eWma_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWma_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eWmaStdDecode]= BRAP_P_ALIGN(size, 2);
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eWmaStdFrameSync] = BRAP_Img_Id_eWma_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWma_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eWmaStdFrameSync]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_LPCMDVD_SUPPORT
/* LPCM DVD FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDvdLpcmFrameSync] = (uint32_t)BRAP_Img_Id_eLpcmDvd_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eLpcmDvd_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDvdLpcmFrameSync] = BRAP_P_ALIGN(size, 2);

/* LPCM DVD  Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDvdLpcmDecode] = (uint32_t)BRAP_Img_Id_eLpcmDvd_Decode;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eLpcmDvd_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDvdLpcmDecode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDvdLpcmDecode] = (uint32_t)BRAP_Img_Id_eLpcmDvd_Decode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eLpcmDvd_Decode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDvdLpcmDecode] = BRAP_P_ALIGN(size, 2);

/* LPCM DVD Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDvdLpcmDecode] = BRAP_Img_Id_eLpcmDvd_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eLpcmDvd_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDvdLpcmDecode]= BRAP_P_ALIGN(size, 2);

	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDvdLpcmFrameSync] = BRAP_Img_Id_eLpcmDvd_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eLpcmDvd_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDvdLpcmFrameSync]= BRAP_P_ALIGN(size, 2);

#endif


#ifdef 	RAP_WMAPRO_SUPPORT
/* WMA PRO FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eWmaProFrameSync] = (uint32_t)BRAP_Img_Id_eWmaPro_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eWmaProFrameSync] = BRAP_P_ALIGN(size, 2);

/* WMA PRO Decode Code and Table */
/*STG0*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage1] = (uint32_t)BRAP_Img_Id_eWmaPro_Decode_Stg0;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Decode_Stg0, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage1] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage1] = (uint32_t)BRAP_Img_Id_eWmaPro_Decode_Stg0_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Decode_Stg0_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage1] = BRAP_P_ALIGN(size, 2);

/*STG1*/
    	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage2] = (uint32_t)BRAP_Img_Id_eWmaPro_Decode_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Decode_Stg1, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage2] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage2] = (uint32_t)BRAP_Img_Id_eWmaPro_Decode_Stg1_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Decode_Stg1_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage2] = BRAP_P_ALIGN(size, 2);

/* WMA PRO Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage1] = BRAP_Img_Id_eWmaPro_Stg0_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Stg0_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage1]= BRAP_P_ALIGN(size, 2);

    	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage2] = BRAP_Img_Id_eWmaPro_Stg1_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Stg1_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage2]= BRAP_P_ALIGN(size, 2);
	
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eWmaProFrameSync] = BRAP_Img_Id_eWmaPro_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eWmaProFrameSync]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_WMAPROPASSTHRU_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eWMAPassThrough] = (uint32_t)BRAP_Img_Id_eWmaPro_Passthru_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eWmaPro_Passthru_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eWMAPassThrough] = BRAP_P_ALIGN(size, 2);
#endif

#ifdef 	RAP_AVL_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAvlPostProc] = (uint32_t)BRAP_Img_Id_eAvl_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAvl_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAvlPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAvlPostProc] = (uint32_t)BRAP_Img_Id_eAvl_Table;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAvl_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAvlPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAvlPostProc] = BRAP_Img_Id_eAvl_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAvl_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAvlPostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef 	RAP_PL2_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_ePl2PostProc] = (uint32_t)BRAP_Img_Id_ePL2_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePL2_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_ePl2PostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_ePl2PostProc] = (uint32_t)BRAP_Img_Id_ePL2_Table;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePL2_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_ePl2PostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_ePl2PostProc] = BRAP_Img_Id_ePL2_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePL2_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_ePl2PostProc]= BRAP_P_ALIGN(size, 2);

#endif

#ifdef 	RAP_SRSXT_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSrsTruSurroundPostProc] = (uint32_t)BRAP_Img_Id_eSrsXt_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsXt_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSrsTruSurroundPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eSrsTruSurroundPostProc] = (uint32_t)BRAP_Img_Id_eSrsXt_Table;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsXt_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eSrsTruSurroundPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eSrsTruSurroundPostProc] = BRAP_Img_Id_eSrsXt_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsXt_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eSrsTruSurroundPostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef 	RAP_SRSHD_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = (uint32_t)BRAP_Img_Id_eSrsHd_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsHd_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = (uint32_t)BRAP_Img_Id_eSrsHd_Table;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsHd_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = BRAP_Img_Id_eSrsHd_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsHd_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eSrsTruSurroundHDPostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef 	RAP_SRSTRUVOL_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSrsTruVolumePostProc] = (uint32_t)BRAP_Img_Id_eSrsTruVolume_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsTruVolume_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSrsTruVolumePostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eSrsTruVolumePostProc] = (uint32_t)BRAP_Img_Id_eSrsTruVolume_Table;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsTruVolume_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eSrsTruVolumePostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eSrsTruVolumePostProc] = BRAP_Img_Id_eSrsTruVolume_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsTruVolume_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eSrsTruVolumePostProc]= BRAP_P_ALIGN(size, 2);
#endif



#ifdef RAP_CUSTOMVOICE_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eCustomVoicePostProc] = (uint32_t)BRAP_Img_Id_eCustomVoice_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomVoice_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eCustomVoicePostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eCustomVoicePostProc] = (uint32_t)BRAP_Img_Id_eCustomVoice_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomVoice_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eCustomVoicePostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eCustomVoicePostProc] = BRAP_Img_Id_eCustomVoice_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomVoice_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eCustomVoicePostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_CUSTOMSURROUND_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eCustomSurroundPostProc] = (uint32_t)BRAP_Img_Id_eCustomSurround_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomSurround_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eCustomSurroundPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eCustomSurroundPostProc] = (uint32_t)BRAP_Img_Id_eCustomSurround_Table;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomSurround_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eCustomSurroundPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eCustomSurroundPostProc] = BRAP_Img_Id_eCustomSurround_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomSurround_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eCustomSurroundPostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_CUSTOMBASS_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eCustomBassPostProc] = (uint32_t)BRAP_Img_Id_eCustomBass_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomBass_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eCustomBassPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eCustomBassPostProc] = (uint32_t)BRAP_Img_Id_eCustomBass_Table;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomBass_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eCustomBassPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eCustomBassPostProc] = BRAP_Img_Id_eCustomBass_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eCustomBass_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eCustomBassPostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_SRC_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSrcPostProc] = (uint32_t)BRAP_Img_Id_eSrc_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrc_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSrcPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eSrcPostProc] = (uint32_t)BRAP_Img_Id_eSrc_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrc_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eSrcPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eSrcPostProc] = BRAP_Img_Id_eSrc_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrc_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eSrcPostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_AUDIODESC_SUPPORT
    /* Fade Control */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAudioDescriptorFadePostProc] = (uint32_t)BRAP_Img_Id_eFadeControl_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eFadeControl_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAudioDescriptorFadePostProc] = BRAP_P_ALIGN(size, 2);

/*	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAudioDescriptorFadePostProc] = (uint32_t)BRAP_Img_Id_eFadeControl_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eFadeControl_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAudioDescriptorFadePostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAudioDescriptorFadePostProc] = BRAP_Img_Id_eFadeControl_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eFadeControl_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAudioDescriptorFadePostProc]= BRAP_P_ALIGN(size, 2);*/

    /* Pan Control */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = (uint32_t)BRAP_Img_Id_ePanControl_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePanControl_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = (uint32_t)BRAP_Img_Id_ePanControl_Tables;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePanControl_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = BRAP_Img_Id_ePanControl_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePanControl_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAudioDescriptorPanPostProc]= BRAP_P_ALIGN(size, 2);    
#endif

#ifdef RAP_PCM_SUPPORT
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_ePesFrameSync] = (uint32_t)BRAP_Img_Id_ePcm_Ids;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePcm_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_ePesFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_ePesFrameSync] = BRAP_Img_Id_ePcm_Interframe;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePcm_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_ePesFrameSync]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_PCMROUTER_SUPPORT
    /* PCM Router */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_ePCMRouterPostProc] = (uint32_t)BRAP_Img_Id_ePCMRouter_Code;

	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePCMRouter_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_ePCMRouterPostProc] = BRAP_P_ALIGN(size, 2);

#endif
    
#ifdef 	RAP_DTSENC_SUPPORT

/* DTS Encode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDtsBroadcastEncode] = (uint32_t)BRAP_Img_Id_eDts_Encode_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDts_Encode_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDtsBroadcastEncode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDtsBroadcastEncode] = (uint32_t)BRAP_Img_Id_eDts_Encode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDts_Encode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDtsBroadcastEncode] = BRAP_P_ALIGN(size, 2);

/* DTS Encode Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDtsBroadcastEncode] = BRAP_Img_Id_eDts_Encode_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDts_Encode_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDtsBroadcastEncode]= BRAP_P_ALIGN(size, 2);
	
#endif

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
/*  Dolby Transcode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMs10DDTranscode] = (uint32_t)BRAP_Img_Id_eDolbyTranscode_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDolbyTranscode_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMs10DDTranscode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eMs10DDTranscode] = (uint32_t)BRAP_Img_Id_eDolbyTranscode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDolbyTranscode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eMs10DDTranscode] = BRAP_P_ALIGN(size, 2);

/*  Dolby Transcode Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMs10DDTranscode] = BRAP_Img_Id_eDolbyTranscode_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDolbyTranscode_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMs10DDTranscode]= BRAP_P_ALIGN(size, 2);
#else    
#ifdef 	RAP_AC3ENC_SUPPORT
/* Ac3 Encode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAc3Encode] = (uint32_t)BRAP_Img_Id_eAc3_Encode_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Encode_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAc3Encode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAc3Encode] = (uint32_t)BRAP_Img_Id_eAc3_Encode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Encode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAc3Encode] = BRAP_P_ALIGN(size, 2);

/* Ac3 Encode Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAc3Encode] = BRAP_Img_Id_eAc3_Encode_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAc3_Encode_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAc3Encode]= BRAP_P_ALIGN(size, 2);
	
#endif
#endif

#ifdef 	RAP_DSOLA_SUPPORT

/* DSOLA Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDsolaPostProc] = (uint32_t)BRAP_Img_Id_eDsola_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDsola_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDsolaPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDsolaPostProc] = (uint32_t)BRAP_Img_Id_eDsola_Tables;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDsola_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDsolaPostProc] = BRAP_P_ALIGN(size, 2);

/* DSOLA Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDsolaPostProc] = BRAP_Img_Id_eDsola_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDsola_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDsolaPostProc]= BRAP_P_ALIGN(size, 2);
	
#endif

#ifdef 	RAP_DOLBYVOL_SUPPORT

/* Dolby Vol Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDolbyVolumePostProc] = (uint32_t)BRAP_Img_Id_eDolbyVol_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDolbyVol_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDolbyVolumePostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDolbyVolumePostProc] = (uint32_t)BRAP_Img_Id_eDolbyVol_Tables;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDolbyVol_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDolbyVolumePostProc] = BRAP_P_ALIGN(size, 2);

/* Dolby Vol Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDolbyVolumePostProc] = BRAP_Img_Id_eDolbyVol_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDolbyVol_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDolbyVolumePostProc]= BRAP_P_ALIGN(size, 2);
	
#endif

#if defined(RAP_DTSBROADCAST_SUPPORT) || defined(RAP_DTSHD_SUPPORT)

/* DTS Broadcast Decode Code and Table */
/*STG0*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = (uint32_t)BRAP_Img_Id_eDtsBroadcast_Decode_Stg0;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsBroadcast_Decode_Stg0, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = (uint32_t)BRAP_Img_Id_eDtsBroadcast_Decode_Table_Stg0;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsBroadcast_Decode_Table_Stg0, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = BRAP_P_ALIGN(size, 2);

/*STG1*/
    	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = (uint32_t)BRAP_Img_Id_eDtsBroadcast_Decode_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsBroadcast_Decode_Stg1, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = (uint32_t)BRAP_Img_Id_eDtsBroadcast_Decode_Table_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsBroadcast_Decode_Table_Stg1, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = BRAP_P_ALIGN(size, 2);

/* DTS Broadcast Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = BRAP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg0;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg0, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0]= BRAP_P_ALIGN(size, 2);

    	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = BRAP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg1, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1]= BRAP_P_ALIGN(size, 2);
	
#endif

#if 0
#ifdef RAP_DTSHD_SUPPORT

/* DTS HD Decode Code and Table */
/*STG0*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = (uint32_t)BRAP_Img_Id_eDtsHd_Decode_Stg0;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsHd_Decode_Stg0, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = (uint32_t)BRAP_Img_Id_eDtsHd_Decode_Table_Stg0;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsHd_Decode_Table_Stg0, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = BRAP_P_ALIGN(size, 2);

/*STG1*/
    	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = (uint32_t)BRAP_Img_Id_eDtsHd_Decode_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsHd_Decode_Stg1, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = (uint32_t)BRAP_Img_Id_eDtsHd_Decode_Table_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsHd_Decode_Table_Stg1, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = BRAP_P_ALIGN(size, 2);

/* DTS HD Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] = BRAP_Img_Id_eDtsHd_Decode_Interframe_Stg0;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsHd_Decode_Interframe_Stg0, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0]= BRAP_P_ALIGN(size, 2);

    	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] = BRAP_Img_Id_eDtsHd_Decode_Interframe_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDtsHd_Decode_Interframe_Stg1, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1]= BRAP_P_ALIGN(size, 2);
	
#endif
#endif

#ifdef RAP_PCMWAV_SUPPORT
/* PCM On AVI FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_ePcmWavFrameSync] = (uint32_t)BRAP_Img_Id_ePcmWav_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePcmWav_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_ePcmWavFrameSync] = BRAP_P_ALIGN(size, 2);

/* PCM On AVI Decode Code*/

	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_ePcmWavDecode] = (uint32_t)BRAP_Img_Id_ePcmWav_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePcmWav_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_ePcmWavDecode] = BRAP_P_ALIGN(size, 2);

/* PCM On AVI Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_ePcmWavDecode] = BRAP_Img_Id_ePcmWav_Code_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePcmWav_Code_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_ePcmWavDecode]= BRAP_P_ALIGN(size, 2);

    	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_ePcmWavFrameSync] = BRAP_Img_Id_ePcmWav_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_ePcmWav_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_ePcmWavFrameSync]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef RAP_MP3ENC_SUPPORT
/*MP3 Encode Decode Code*/

	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMpegL3EncodeStage1] = (uint32_t)BRAP_Img_Id_eMp3Enc_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMp3Enc_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMpegL3EncodeStage1] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eMpegL3EncodeStage1] = (uint32_t)BRAP_Img_Id_eMp3Enc_Code_Tables;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMp3Enc_Code_Tables, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eMpegL3EncodeStage1] = BRAP_P_ALIGN(size, 2);    

/* MP3 Encode Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMpegL3EncodeStage1] = BRAP_Img_Id_eMp3Enc_Code_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMp3Enc_Code_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMpegL3EncodeStage1]= BRAP_P_ALIGN(size, 2);

#endif

#ifdef RAP_AMR_SUPPORT
/* AMR FrameSync */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAmrFrameSync] = (uint32_t)BRAP_Img_Id_eAMR_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAMR_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAmrFrameSync] = BRAP_P_ALIGN(size, 2);

/* AMR Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAmrDecode] = (uint32_t)BRAP_Img_Id_eAMR_Decode;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAMR_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAmrDecode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAmrDecode] = (uint32_t)BRAP_Img_Id_eAMR_Decode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAMR_Decode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAmrDecode] = BRAP_P_ALIGN(size, 2);    
    
/* AMR Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAmrDecode] = BRAP_Img_Id_eAMR_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAMR_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAmrDecode]= BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAmrFrameSync] = BRAP_Img_Id_eAMR_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAMR_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAmrFrameSync]= BRAP_P_ALIGN(size, 2);
#endif


#ifdef RAP_DRA_SUPPORT

/* DRA Decode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDraDecode] = (uint32_t)BRAP_Img_Id_eDRA_Decode;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDRA_Decode, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDraDecode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDraDecode] = (uint32_t)BRAP_Img_Id_eDRA_Decode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDRA_Decode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDraDecode] = BRAP_P_ALIGN(size, 2);    
    
/* DRA Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDraDecode] = BRAP_Img_Id_eDRA_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDRA_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDraDecode]= BRAP_P_ALIGN(size, 2);
    
#endif


#ifdef 	RAP_SBCENC_SUPPORT

/*Sbc Encode Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSbcEncode] = (uint32_t)BRAP_Img_Id_eSbc_Encode_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSbc_Encode_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSbcEncode] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eSbcEncode] = (uint32_t)BRAP_Img_Id_eSbc_Encode_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSbc_Encode_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eSbcEncode] = BRAP_P_ALIGN(size, 2);

/* Sbc Encode Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eSbcEncode] = BRAP_Img_Id_eSbc_Encode_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSbc_Encode_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eSbcEncode]= BRAP_P_ALIGN(size, 2);
	
#endif

#ifdef RAP_VIDEOONDSP_SUPPORT
#ifdef RAP_REALVIDEO_SUPPORT

/*Real Video Decode Stage 1*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eRealVideo9Stage1] = (uint32_t)BRAP_Img_Id_eRealVideo_Decode_Stg1;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Decode_Stg1, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eRealVideo9Stage1] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eRealVideo9Stage1] = (uint32_t)BRAP_Img_Id_eRealVideo_Decode_Stg1_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Decode_Stg1_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eRealVideo9Stage1] = BRAP_P_ALIGN(size, 2);

/*Real Video Decode Stage 2*/
    	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eRealVideo9Stage2] = (uint32_t)BRAP_Img_Id_eRealVideo_Decode_Stg2;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Decode_Stg2, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eRealVideo9Stage2] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eRealVideo9Stage2] = (uint32_t)BRAP_Img_Id_eRealVideo_Decode_Stg2_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Decode_Stg2_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eRealVideo9Stage2] = BRAP_P_ALIGN(size, 2);

/*Real Video Decode Stage 3*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eRealVideo9Stage3] = (uint32_t)BRAP_Img_Id_eRealVideo_Decode_Stg3;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Decode_Stg3, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eRealVideo9Stage3] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eRealVideo9Stage3] = (uint32_t)BRAP_Img_Id_eRealVideo_Decode_Stg3_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Decode_Stg3_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eRealVideo9Stage3] = BRAP_P_ALIGN(size, 2);    

/*Real Video PLF*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eRealVideo9PLF] = (uint32_t)BRAP_Img_Id_eRealVideo_Plf;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Plf, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eRealVideo9PLF] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eRealVideo9PLF] = (uint32_t)BRAP_Img_Id_eRealVideo_Plf_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Plf_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eRealVideo9PLF] = BRAP_P_ALIGN(size, 2);


/*Real Video PPD*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eRealVideo9PPD] = (uint32_t)BRAP_Img_Id_eRealVideo_Ppd;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Ppd, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eRealVideo9PPD] = BRAP_P_ALIGN(size, 2);


/*Real Video Decode Interframe*/
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eRealVideo9PLF] = (uint32_t)BRAP_Img_Id_eRealVideo_Decode_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealVideo_Decode_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eRealVideo9PLF] = BRAP_P_ALIGN(size, 2);

#endif
#endif

#ifdef  RAP_REALAUDIOLBR_SUPPORT    
    /* RA LBR Decode Code and Table */
        psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eRealAudioLbrDecode] = (uint32_t)BRAP_Img_Id_eRealAudioLbr_Decode_Code;
        BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealAudioLbr_Decode_Code, &size);
        psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eRealAudioLbrDecode] = BRAP_P_ALIGN(size, 2);
    
        psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eRealAudioLbrDecode] = (uint32_t)BRAP_Img_Id_eRealAudioLbr_Decode_Table;
        BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealAudioLbr_Decode_Table, &size);
        psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eRealAudioLbrDecode] = BRAP_P_ALIGN(size, 2);    
        
    /*  RA LBR Interframe */
        psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eRealAudioLbrDecode] = BRAP_Img_Id_eRealAudioLbr_Decode_Interframe;
        BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eRealAudioLbr_Decode_Interframe, &size);
        psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eRealAudioLbrDecode]= BRAP_P_ALIGN(size, 2);
        
#endif

#ifdef 	RAP_ADV_SUPPORT
    /*  Audyssey Volume Code and Table */
    psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAudysseyVolumePostProc] = (uint32_t)BRAP_Img_Id_eAvd_Code;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAvd_Code, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAudysseyVolumePostProc] = BRAP_P_ALIGN(size, 2);

    psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAudysseyVolumePostProc] = (uint32_t)BRAP_Img_Id_eAvd_Table;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAvd_Table, &size);
    psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAudysseyVolumePostProc] = BRAP_P_ALIGN(size, 2);

    /*  Audyssey Volume Interframe */
    psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAudysseyVolumePostProc] = BRAP_Img_Id_eAvd_Interframe;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAvd_Interframe, &size);
    psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAudysseyVolumePostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef 	RAP_ABX_SUPPORT
    /*  Audyssey ABX Code and Table */
    psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eAudysseyABXPostProc] = (uint32_t)BRAP_Img_Id_eAbx_Code;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAbx_Code, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eAudysseyABXPostProc] = BRAP_P_ALIGN(size, 2);

    psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eAudysseyABXPostProc] = (uint32_t)BRAP_Img_Id_eAbx_Table;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAbx_Table, &size);
    psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eAudysseyABXPostProc] = BRAP_P_ALIGN(size, 2);

    /*  Audyssey ABX Interframe */
    psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eAudysseyABXPostProc] = BRAP_Img_Id_eAbx_Interframe;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eAbx_Interframe, &size);
    psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eAudysseyABXPostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef 	RAP_BRCM3DSURROUND_SUPPORT

/*  Brcm 3D Surround Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = (uint32_t)BRAP_Img_Id_eBrcm3DSurround_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eBrcm3DSurround_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = (uint32_t)BRAP_Img_Id_eBrcm3DSurround_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eBrcm3DSurround_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = BRAP_P_ALIGN(size, 2);

/*  Brcm 3D Surround Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = BRAP_Img_Id_eBrcm3DSurround_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eBrcm3DSurround_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eBrcm3DSurroundPostProc]= BRAP_P_ALIGN(size, 2);
	
#endif

#ifdef 	RAP_MONODOWNMIX_SUPPORT

/*  Mono Downmix Code */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMonoDownMixPostProc] = (uint32_t)BRAP_Img_Id_eMonoDownmix_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eMonoDownmix_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMonoDownMixPostProc] = BRAP_P_ALIGN(size, 2);
#endif


#ifdef 	RAP_FWMIXER_SUPPORT
    /* Firmware Mixer FrameSync*/
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eMixerTaskFrameSync] = (uint32_t)BRAP_Img_Id_eFwMixer_Ids;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eFwMixer_Ids, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eMixerTaskFrameSync] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eMixerTaskFrameSync] = (uint32_t)BRAP_Img_Id_eFwMixer_Ids_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eFwMixer_Ids_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eMixerTaskFrameSync]= BRAP_P_ALIGN(size, 2);
    
/*  Firmware Mixer Code and Table */
    psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eFWMixerPostProc] = (uint32_t)BRAP_Img_Id_eFwMixer_Code;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eFwMixer_Code, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eFWMixerPostProc] = BRAP_P_ALIGN(size, 2);

    psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eFWMixerPostProc] = (uint32_t)BRAP_Img_Id_eFwMixer_Table;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eFwMixer_Table, &size);
    psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eFWMixerPostProc] = BRAP_P_ALIGN(size, 2);

/*  Firmware Mixer Surround Interframe */
    psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eFWMixerPostProc] = BRAP_Img_Id_eFwMixer_Interframe;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eFwMixer_Interframe, &size);
    psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eFWMixerPostProc]= BRAP_P_ALIGN(size, 2);
        
#endif

#ifdef 	RAP_SRSCSTD_SUPPORT
/*  SRS Circle-Surround and Tru-Dialogue Code and Table */
    psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSrsCircleSurrPostProc] = (uint32_t)BRAP_Img_Id_eSrsCsTd_Code;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsCsTd_Code, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSrsCircleSurrPostProc] = BRAP_P_ALIGN(size, 2);

    psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eSrsCircleSurrPostProc] = (uint32_t)BRAP_Img_Id_eSrsCsTd_Table;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsCsTd_Table, &size);
    psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eSrsCircleSurrPostProc] = BRAP_P_ALIGN(size, 2);
    
/*  SRS Circle-Surround and Tru-Dialogue Interframe */
    psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eSrsCircleSurrPostProc] = BRAP_Img_Id_eSrsCsTd_Interframe;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsCsTd_Interframe, &size);
    psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eSrsCircleSurrPostProc]= BRAP_P_ALIGN(size, 2);
#endif

#ifdef 	RAP_SRSEQHL_SUPPORT
/*  SRS Equalizer and Hard Limiter Code and Table */
    psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eSrsEqualizerPostProc] = (uint32_t)BRAP_Img_Id_eSrsEqHl_Code;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsEqHl_Code, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eSrsEqualizerPostProc] = BRAP_P_ALIGN(size, 2);

    psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eSrsEqualizerPostProc] = (uint32_t)BRAP_Img_Id_eSrsEqHl_Table;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsEqHl_Table, &size);
    psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eSrsEqualizerPostProc] = BRAP_P_ALIGN(size, 2);
    
/*  SRS Equalizer and Hard Limiter Interframe */
    psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eSrsEqualizerPostProc] = BRAP_Img_Id_eSrsEqHl_Interframe;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eSrsEqHl_Interframe, &size);
    psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eSrsEqualizerPostProc]= BRAP_P_ALIGN(size, 2);
#endif
#ifdef 	RAP_DV258_SUPPORT
        /*  DV258 Code and Table */
        psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDv258PostProc] = (uint32_t)BRAP_Img_Id_eDV258_Code;
        BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDV258_Code, &size);
        psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDv258PostProc] = BRAP_P_ALIGN(size, 2);
    
        psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDv258PostProc] = (uint32_t)BRAP_Img_Id_eDV258_Table;
        BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDV258_Table, &size);
        psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDv258PostProc] = BRAP_P_ALIGN(size, 2);
    
        /*   DV258  Interframe */
        psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDv258PostProc] = BRAP_Img_Id_eDV258_Interframe;
        BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDV258_Interframe, &size);
        psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDv258PostProc]= BRAP_P_ALIGN(size, 2);
#endif


#ifdef 	RAP_DDRE_SUPPORT
            /*  DDRE Code and Table */
            psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eDdrePostProc] = (uint32_t)BRAP_Img_Id_eDDRE_Code;
            BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDDRE_Code, &size);
            psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eDdrePostProc] = BRAP_P_ALIGN(size, 2);
        
            psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eDdrePostProc] = (uint32_t)BRAP_Img_Id_eDDRE_Table;
            BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDDRE_Table, &size);
            psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eDdrePostProc] = BRAP_P_ALIGN(size, 2);
        
            /*   DDRE  Interframe */
            psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eDdrePostProc] = BRAP_Img_Id_eDDRE_Interframe;
            BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eDDRE_Interframe, &size);
            psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eDdrePostProc]= BRAP_P_ALIGN(size, 2);
#endif




#ifdef 	RAP_GFX_SUPPORT
/*  GFX Code and Table */
    psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eGfxDecode] = (uint32_t)BRAP_Img_Id_egfx_Code;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_egfx_Code, &size);
    psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eGfxDecode] = BRAP_P_ALIGN(size, 2);

    psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eGfxDecode] = (uint32_t)BRAP_Img_Id_egfx_Table;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_egfx_Table, &size);
    psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eGfxDecode] = BRAP_P_ALIGN(size, 2);
    
/*  GFX Interframe */
    psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eGfxDecode] = BRAP_Img_Id_egfx_Interframe;
    BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_egfx_Interframe, &size);
    psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eGfxDecode]= BRAP_P_ALIGN(size, 2);

#endif

#ifdef 	RAP_BTSC_SUPPORT

/*  BTSC Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eBtscEncPostProc] = (uint32_t)BRAP_Img_Id_eBtsc_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eBtsc_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eBtscEncPostProc] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eBtscEncPostProc] = (uint32_t)BRAP_Img_Id_eBtsc_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eBtsc_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eBtscEncPostProc] = BRAP_P_ALIGN(size, 2);

/*  BTSC Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eBtscEncPostProc] = BRAP_Img_Id_eBtsc_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eBtsc_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eBtscEncPostProc]= BRAP_P_ALIGN(size, 2);
	
#endif

#ifdef 	RAP_SCM_SUPPORT
	/*	SCM_1 Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eScm_1] = (uint32_t)BRAP_Img_Id_eScm_1_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eScm_1_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eScm_1] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eScm_1] = (uint32_t)BRAP_Img_Id_eScm_1_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eScm_1_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eScm_1] = BRAP_P_ALIGN(size, 2);

	/*	SCM_1 Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eScm_1] = BRAP_Img_Id_eScm_1_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eScm_1_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eScm_1]= BRAP_P_ALIGN(size, 2);

	/*	SCM_2 Code and Table */
	psImgIdMappingArrays->ui32CodeImgId[BRAP_AF_P_AlgoId_eScm_2] = (uint32_t)BRAP_Img_Id_eScm_2_Code;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eScm_2_Code, &size);
	psImgIdMappingArrays->ui32CodeSize[BRAP_AF_P_AlgoId_eScm_2] = BRAP_P_ALIGN(size, 2);

	psImgIdMappingArrays->ui32TableImgId[BRAP_AF_P_AlgoId_eScm_2] = (uint32_t)BRAP_Img_Id_eScm_2_Table;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eScm_2_Table, &size);
	psImgIdMappingArrays->ui32TableSize[BRAP_AF_P_AlgoId_eScm_2] = BRAP_P_ALIGN(size, 2);

	/*	SCM_2 Interframe */
	psImgIdMappingArrays->ui32InterframeImgId[BRAP_AF_P_AlgoId_eScm_2] = BRAP_Img_Id_eScm_2_Interframe;
	BRAP_DSP_P_GetFWSize (hRap, BRAP_Img_Id_eScm_2_Interframe, &size);
	psImgIdMappingArrays->ui32InterframeSize[BRAP_AF_P_AlgoId_eScm_2]= BRAP_P_ALIGN(size, 2);
	
#endif

    BDBG_LEAVE(BRAP_MapImgIdWithExecId);
	return;
}

/* take argument hrap and do accordingly */

void BRAP_UpdateMitEntry(
		BRAP_Handle		hRap,
		BRAP_AF_P_AlgoId	eExecId,
		uint32_t		ui32CodePtr,
		uint32_t 		ui32CodeSize,
		uint32_t		ui32TblPtr,		
		uint32_t		ui32TblSize				
		)
{
	BDBG_ENTER(BRAP_UpdateMitEntry);
	BDBG_ASSERT(hRap);
	
	hRap->sMit.sAlgoDwnldDetail[eExecId].ui32AlgoCodePtr = ui32CodePtr;
	hRap->sMit.sAlgoDwnldDetail[eExecId].ui32AlgoCodeSize= ui32CodeSize;
	hRap->sMit.sAlgoDwnldDetail[eExecId].ui32AlgoDataPtr = ui32TblPtr;
	hRap->sMit.sAlgoDwnldDetail[eExecId].ui32AlgoDataSize= ui32TblSize;
	
    BDBG_LEAVE(BRAP_UpdateMitEntry);
	return;
}

void BRAP_PrintMit(BRAP_Handle	hRap)
{
	uint32_t	i=0, ui32TempPhysAddr1 =0,ui32TempPhysAddr2=0;
	
	for(i = 0; i < BRAP_AF_P_AlgoId_eMax; i++)
	{
		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoCodePtr), &ui32TempPhysAddr1);
		BRAP_P_ConvertAddressToOffset(hRap->hHeap, (void *)(hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoDataPtr), &ui32TempPhysAddr2);
		BDBG_MSG(("Exec ID %#x",i));		
		BDBG_MSG(("ui32AlgoCodePtr = %#x , physical Addr =%#x ",hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoCodePtr,ui32TempPhysAddr1));
		BDBG_MSG(("ui32AlgoCodeSize = %#x",hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoCodeSize));
		BDBG_MSG(("ui32AlgoDataPtr = %#x , physical Addr =%#x",hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoDataPtr,ui32TempPhysAddr2));
		BDBG_MSG(("ui32AlgoDataSize = %#x",hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoDataSize));		
	}
}

void BRAP_InitializeMit(BRAP_Handle	hRap)
{
	uint32_t	i=0;
	for(i = 0; i < BRAP_AF_P_AlgoId_eMax; i++)
	{
		hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoCodePtr = 0xffffffff;
		hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoCodeSize= 0;
		hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoDataPtr = 0xffffffff;
		hRap->sMit.sAlgoDwnldDetail[i].ui32AlgoDataSize= 0;		
	}
}

/***************************************************************************
Summary:
	Gets DSP default channel setting

Description:
	The default DSP channel configuration settings are returned
	
Returns:
    BERR_SUCCESS                - if successful 

See Also:
	BRAP_DSP_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSP_P_GetDefaultSettings( 
   BRAP_DSP_P_Settings	*psDefSettings	/* [out] The DEC channel default configuration setting */
   )
{
       BSTD_UNUSED(psDefSettings);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary: Opens an BRAP_DSP channel 

Description:
	This function is responsible for opening a DSP channel. 
	When a BRAP_DSP channel is opened, it will create a module channel handle 
	and configure the module based on the channel settings. 
	Once a channel is opened, it must be closed before it can be opened 
	again.

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSP_P_Close,
	BRAP_DSP_P_GetDefaultSettings
**************************************************************************/
BERR_Code 
BRAP_DSP_P_Open(
	BRAP_DSP_Handle			*phDsp,			/* [out] DSP handle */
	BRAP_Handle				hRap,			/* [in] AUD handle */
	uint32_t				dspIndex,		/* [in] Channel index starting from 0 to max number of channels */ 
	const BRAP_DSP_P_Settings	*pDefSettings	/* [in] The DSP channel configuration setting */
	)
{
	BRAP_DSP_P_Device *handle;
	BERR_Code rc = BERR_SUCCESS ;
    uint32_t physAddress = 0;

    BRAP_FWIF_MsgQueueHandle hMsgQueue;
    unsigned int i = 0;
    uint32_t index = 0, dspCfgRamSize = 0, ui32FifoPtr = 0;
    bool                bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);
    unsigned int regVal;

    BDBG_ENTER(BRAP_DSP_P_Open);

    /* Assert the function arguments*/
    BDBG_ASSERT(phDsp);
    BDBG_ASSERT(hRap);
       BSTD_UNUSED(pDefSettings);

    if(false == bWdgRecovery)
    {
	handle = (BRAP_DSP_P_Device *)BKNI_Malloc( sizeof(BRAP_DSP_P_Device));
	if(handle == NULL)
	{
		rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto exit;
        }
        
	*phDsp = handle;
		/* Reset the entire structure */
	BKNI_Memset( handle, 0, sizeof(BRAP_DSP_P_Device) );

	/* init the data structure  */
	handle->hRap = hRap;
	handle->hChip = hRap->hChip;
	handle->hRegister = hRap->hRegister;
	handle->hHeap = hRap->hHeap;
	handle->hInt = hRap->hInt;
    handle->uiDspIndex = dspIndex;
    /* Modify this for multiple DSP case */
    handle->ui32Offset = 0;

    for(i =0 ; i < BRAP_RM_P_MAX_FW_TASK_PER_DSP; i++)
    {
        handle->ui32EventIdMatrix[i] = BRAP_FWIF_P_EventIdMask_eNone;
    }

        
    }
    else
    {
        handle = *phDsp;
    }
    
    /* Initialize entire DSP configuration RAM to zero */
    /* Skip overwriting 17 as that is used for FW dbg buffer */
    dspCfgRamSize = BCHP_AUD_DSP_CFG0_READ_PTR_FIFO_17 - BCHP_AUD_DSP_CFG0_MASTER_IT_POINTER;
    for (index = 0; index < dspCfgRamSize; index+=4)
    {
        BRAP_Write32(handle->hRegister, BCHP_AUD_DSP_CFG0_MASTER_IT_POINTER + 
        handle->ui32Offset + index, 0x0);
    }


    if(false == bWdgRecovery)
    {
/*  Creating structure for firmware which will contain physical addresses of all 
    18 fifo registers in the order base,end,read,write,wrap. Wrap will also be 
    programmed with fifo end address. Base address of this will be programmed in
    BCHP_AUD_DSP_CFG0_CONTROL_REGISTER0_CXT0 before bootup */
    handle->ui32DSPFifoAddrStruct = (uint32_t)BRAP_P_AllocAligned(hRap, 
                BRAP_DSP_P_NUM_FIFOS * BRAP_DSP_P_NUM_PTR_PER_FIFO * 4,2, 0);	/* 32 bit aligned*/
    if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)handle->ui32DSPFifoAddrStruct)
    {
        BDBG_ERR(("BRAP_DSP_P_Open: Unable to Allocate memory for FIFO Register addresses!"));
            BKNI_Free(handle);
            goto exit;
    }
    }

		BRAP_P_ConvertAddressToOffset(handle->hHeap, (void *)handle->ui32DSPFifoAddrStruct, &physAddress);
        BDBG_MSG(("BASE_ADDR FIFO STRUCT > %x", handle->ui32DSPFifoAddrStruct));
        BDBG_MSG(("BASE_ADDR PHYS FIFO STRUCT > %x", physAddress));
    	BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_CFG0_CONTROL_REGISTER0_CXT0 + 
            handle->ui32Offset, physAddress);
        /* Fill up this structure */
        physAddress = BCHP_AUD_DSP_CFG0_BASE_ADDR_FIFO_0 + handle->ui32Offset + BCHP_PHYSICAL_OFFSET;
        ui32FifoPtr = handle->ui32DSPFifoAddrStruct;
        for(i=0;i<BRAP_DSP_P_NUM_FIFOS;i++)
        {
        	BRAP_MemWrite32(ui32FifoPtr,physAddress); /* base */
            ui32FifoPtr += 4;
        	BRAP_MemWrite32(ui32FifoPtr,physAddress + 4); /* end */
            ui32FifoPtr += 4;
        	BRAP_MemWrite32(ui32FifoPtr,physAddress + 12); /* read */
            ui32FifoPtr += 4;
        	BRAP_MemWrite32(ui32FifoPtr,physAddress + 8); /* write */
            ui32FifoPtr += 4;
        	BRAP_MemWrite32(ui32FifoPtr,physAddress + 4); /* wrap */
            ui32FifoPtr += 4;
            physAddress += 4*(BRAP_DSP_P_NUM_PTR_PER_FIFO-1);
        }

    if(false == bWdgRecovery)
    {
    handle->ui32DbgBufAddr = (uint32_t)BRAP_P_AllocAligned(hRap, BRAP_DSP_P_DBG_BUF_SIZE,2, 0);	/* 32 bit aligned*/
    if((void *) BRAP_P_INVALID_DRAM_ADDRESS == (void *)handle->ui32DbgBufAddr)
    {
        BDBG_ERR(("BRAP_DSP_P_Open: Unable to Allocate memory for debug buffer!"));
            BKNI_Free(handle);            
            goto exit;
    }
    }

    BKNI_Memset((void *) (volatile uint32_t *)handle->ui32DbgBufAddr,0,BRAP_DSP_P_DBG_BUF_SIZE);

    BRAP_P_ConvertAddressToOffset(handle->hHeap, (void *)handle->ui32DbgBufAddr, &physAddress);

        BDBG_MSG(("BASE_ADDR > %x", handle->ui32DbgBufAddr));
        BDBG_MSG(("BASE_ADDR PHYS > %x", physAddress));
    
    	BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_CFG0_BASE_ADDR_FIFO_17 + 
            handle->ui32Offset, physAddress);
    	BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_CFG0_READ_PTR_FIFO_17 + 
            handle->ui32Offset, physAddress);
    	BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_CFG0_WRITE_PTR_FIFO_17 + 
            handle->ui32Offset, physAddress);
		BRAP_P_ConvertAddressToOffset(handle->hHeap, (void *)((uint32_t)(handle->ui32DbgBufAddr + 
            BRAP_DSP_P_DBG_BUF_SIZE)), &physAddress);
    	BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_CFG0_END_ADDR_FIFO_17 + 
            handle->ui32Offset, physAddress);


    if(true == bWdgRecovery)
    {
        hMsgQueue =     handle->hCmdQueue;
    }
    rc = BRAP_FWIF_P_CreateMsgQueue(&(hRap->sMemInfo.sCmdQueue[dspIndex]), handle->hHeap, 
                                                                handle->hRegister, handle->ui32Offset, &hMsgQueue,bWdgRecovery);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Open: Cmd queue creation failed!"));
        BKNI_Free(handle);        
        goto exit;
    }
    if(false == bWdgRecovery)
    {    
    handle->hCmdQueue = hMsgQueue;
    }
           
    if(true == bWdgRecovery)
    {
        hMsgQueue =     handle->hGenRspQueue ;
    }
    rc = BRAP_FWIF_P_CreateMsgQueue(&(hRap->sMemInfo.sGenRspQueue[dspIndex]), handle->hHeap, 
                                                                handle->hRegister, handle->ui32Offset, &hMsgQueue,bWdgRecovery);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Open: Generic Response queue creation failed!"));
        BKNI_Free(handle);        
        goto exit;
    }
    if(false == bWdgRecovery)
    {        
    handle->hGenRspQueue = hMsgQueue;
    }

#ifdef RAP_VIDEOONDSP_SUPPORT
    /* Picture delivery queue */
    if(true == bWdgRecovery)
    {
        hMsgQueue =     handle->hPDQueue;
    }
    rc = BRAP_P_CreateMsgQueue(&(hRap->sMemInfo.sPDQueue[dspIndex]), handle->hHeap, 
                                                                handle->hRegister, handle->ui32Offset, &hMsgQueue,bWdgRecovery);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Open: Picture delivery queue(PDQ) creation failed!"));
        BKNI_Free(handle);        
        goto exit;
    }
    if(false == bWdgRecovery)
    {    
    handle->hPDQueue = hMsgQueue;
    }
    
    /*Picture release queue*/       
    if(true == bWdgRecovery)
    {
        hMsgQueue =     handle->hPRQueue ;
    }
    rc = BRAP_P_CreateMsgQueue(&(hRap->sMemInfo.sPRQueue[dspIndex]), handle->hHeap, 
                                                                handle->hRegister, handle->ui32Offset, &hMsgQueue,bWdgRecovery);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Open: Picture release queue(PRQ) creation failed!"));
        BKNI_Free(handle);        
        goto exit;
    }
    if(false == bWdgRecovery)
    {        
    handle->hPRQueue = hMsgQueue;
    }
    /*Display  queue*/       
    if(true == bWdgRecovery)
    {
        hMsgQueue =     handle->hDSQueue ;
    }
    rc = BRAP_P_CreateMsgQueue(&(hRap->sMemInfo.sDSQueue[dspIndex]), handle->hHeap, 
                                                                handle->hRegister, handle->ui32Offset, &hMsgQueue,bWdgRecovery);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Open: Display Queue(DSQ) creation failed!"));
        BKNI_Free(handle);        
        goto exit;
    }
    if(false == bWdgRecovery)
    {        
    handle->hDSQueue = hMsgQueue;
    }
    
#endif

#if 0
    /* Install Generic Response Queue Callback */
    rc = BRAP_P_AckInstall(handle);
    if(BERR_SUCCESS != rc)
    {
        BKNI_Free(handle);    
        BDBG_ERR(("BRAP_DSP_P_Open: Unable to install Generic Response ISR!"));
        return BERR_TRACE(rc);
    }
#endif    

    if(false == bWdgRecovery)
    {
        for(i=0; i<BRAP_RM_P_MAX_FW_TASK_PER_DSP; i++)
        {
        /* Create Event Required */
            rc = BRAP_P_EventCreate(handle,i);
        if(rc != BERR_SUCCESS)
        {
            BDBG_ERR(("Event creation failed"));
            return BRAP_ERR_BAD_DEVICE_STATE;
        }
            BRAP_P_EventReset(handle,(i));            
        }
    }
    
    
    if((hRap->sSettings.bFwAuthEnable==true) && (bWdgRecovery == false))
    {
        /* Reset DSP processor and its peripherals */
        regVal = BRAP_DSP_P_ResetHardware(handle);
        
        /* Withdraw only RESET0_B reset */
        regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET0_B)))|
        (BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET0_B,1));
        BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET, regVal);
        BDBG_MSG(("\n**** Soft reset: reset0_b withdrawn = 0x%x",regVal));

        regVal = BRAP_Read32(handle->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET);
        regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET_SCBDMA0_B)))|
        (BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET_SCBDMA0_B,1));
        BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET, regVal);
        BDBG_MSG(("\n**** Soft reset: SCBDMA withdrawn = 0x%x ****",regVal));
    }
    else
    {
        rc = BRAP_DSP_P_InitHardware(handle,false);
        if (rc!=BERR_SUCCESS) {
            BDBG_MSG(("BRAP_DSP_P_InitHardware::Error returned %x!",rc));
            rc = BERR_TRACE(rc);
            goto exit;
        }
    }
    
    /* Program command queue & generic response queue */
	BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_CFG0_CONTROL_REGISTER1_CXT0 + 
        handle->ui32Offset, BRAP_DSP_P_FIFO_CMD);
	BRAP_Write32(handle->hRegister,BCHP_AUD_DSP_CFG0_CONTROL_REGISTER2_CXT0 + 
        handle->ui32Offset, BRAP_DSP_P_FIFO_GENERIC_RSP);

    if(false == bWdgRecovery)
    {
    for(i=0;i<BRAP_DSP_P_NUM_FIFOS;i++)
    {
        handle->bFifoUsed[i] = false;
    }
        
    handle->bFifoUsed[BRAP_DSP_P_FIFO_CMD] = true;
    handle->bFifoUsed[BRAP_DSP_P_FIFO_GENERIC_RSP] = true;
    handle->bFifoUsed[BRAP_DSP_P_FIFO_DEBUG] = true;
    }

exit:
    BDBG_LEAVE(BRAP_DSP_P_Open);
    return rc;
}


/***************************************************************************
Summary: Closes an BRAP_DSP channel

Description:
	This function is responsible for closing a DSP channel and releasing
	all the resources associated with this channel.
	

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSP_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSP_P_Close( 
	BRAP_DSP_Handle hDsp /* [in] DSP handle */
	)
{
	BRAP_Handle				hRap;			/* [in] AUD handle */
	BERR_Code rc = BERR_SUCCESS ;
    uint32_t index = 0, dspCfgRamSize = 0;
    uint32_t regVal = 0,i=0;


    BDBG_ENTER(BRAP_DSP_P_Close);
    BDBG_ASSERT(hDsp);

    hRap = hDsp->hRap;
    
    /* Stop DSP. Otherwise DSP is known to access invalid memory location and 
    cause a GISB timeout error */    
	regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset);
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET_PROC0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET_PROC0_B,0));
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset, regVal);
    
    rc = BRAP_P_AckUnInstall(hDsp);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Close: call to BRAP_P_AckUnInstall failed."));		
        return BERR_TRACE(rc);
    }

    /* Program command invalide queue */
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CFG0_CONTROL_REGISTER1_CXT0 + 
        hDsp->ui32Offset, BRAP_DSP_P_FIFO_INVALID);
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CFG0_CONTROL_REGISTER2_CXT0 + 
        hDsp->ui32Offset, BRAP_DSP_P_FIFO_INVALID);    

        for(i=0; i<BRAP_RM_P_MAX_FW_TASK_PER_DSP; i++)
        {
    /*Destroy the event*/
            BRAP_P_EventDestroy(hDsp,i);
        }

    /* Destroyt command and response message queue */
    rc = BRAP_FWIF_P_DestroyMsgQueue(hDsp->hGenRspQueue, hDsp);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Close: Generic Response queue destroy failed!"));
        return BERR_TRACE(rc);
    }
    hDsp->hGenRspQueue = NULL;
    
    rc = BRAP_FWIF_P_DestroyMsgQueue(hDsp->hCmdQueue, hDsp);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Close: Cmd queue destroy failed!"));
        return BERR_TRACE(rc);
    }
    hDsp->hCmdQueue = NULL;
    
#ifdef RAP_VIDEOONDSP_SUPPORT
    /* Destroye Picture delivery and release message queue */
    rc = BRAP_P_DestroyMsgQueue(hDsp->hPRQueue, hDsp);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Close: Picture release queue destroy failed!"));
        return BERR_TRACE(rc);
    }
    hDsp->hPRQueue= NULL;
    
    rc = BRAP_FWIF_P_DestroyMsgQueue(hDsp->hPDQueue, hDsp);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Close: Picture delivery queue destroy failed!"));
        return BERR_TRACE(rc);
    }
    hDsp->hPDQueue= NULL;
    rc = BRAP_FWIF_P_DestroyMsgQueue(hDsp->hDSQueue, hDsp);
    if(BERR_SUCCESS != rc)
    {
        BDBG_ERR(("BRAP_DSP_P_Close: Display delivery queue destroy failed!"));
        return BERR_TRACE(rc);
    }
    hDsp->hDSQueue= NULL;
    
#endif
    
    /*Clear the debug buffer registers*/
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CFG0_BASE_ADDR_FIFO_17 + 
        hDsp->ui32Offset, BRAP_P_INVALID_DRAM_ADDRESS);
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CFG0_READ_PTR_FIFO_17 + 
        hDsp->ui32Offset, BRAP_P_INVALID_DRAM_ADDRESS);
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CFG0_WRITE_PTR_FIFO_17 + 
        hDsp->ui32Offset, BRAP_P_INVALID_DRAM_ADDRESS);
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CFG0_END_ADDR_FIFO_17 + 
        hDsp->ui32Offset, BRAP_P_INVALID_DRAM_ADDRESS);
	BRAP_P_Free(hDsp->hRap, (void *)hDsp->ui32DbgBufAddr);
    hDsp->ui32DbgBufAddr = (uint32_t) NULL;        


    /*Free fifo structures */
	BRAP_P_Free(hDsp->hRap, (void *)hDsp->ui32DSPFifoAddrStruct);
    hDsp->ui32DSPFifoAddrStruct = (uint32_t) NULL;

    /* Initialize entire DSP configuration RAM to zero */
    /* Skip overwriting 17 as that is used for FW dbg buffer */
	dspCfgRamSize = BCHP_AUD_DSP_CFG0_READ_PTR_FIFO_17 - BCHP_AUD_DSP_CFG0_MASTER_IT_POINTER;
	for (index = 0; index < dspCfgRamSize; index+=4) 
    {
		BRAP_Write32(hDsp->hRegister, BCHP_AUD_DSP_CFG0_MASTER_IT_POINTER + 
                    hDsp->ui32Offset + index, 0x0);
	}

    BKNI_Free(hDsp);
    
    BDBG_LEAVE(BRAP_DSP_P_Close);    
	return BERR_SUCCESS;
}

BERR_Code BRAP_DSP_P_GetFWSize (BRAP_Handle hRap, BRAP_Img_Id firmware_id, uint32_t *size)
{
	void *image;	
	BERR_Code rc;	
	const void *data;	
	const BIMG_Interface *iface = hRap->sSettings.pImgInterface;	
	void *context = hRap->sSettings.pImgContext;

	rc = iface->open (context, &image, (uint32_t)firmware_id);

	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("\nError in Opening the Image Interface for FW_ID =%d \n",firmware_id));
		return BERR_TRACE(rc);
	}

	rc = iface->next(image, 0, &data, 8);

	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("\nError in fetching next chunk in Image Interface\n"));
		iface->close(image);        
		return BERR_TRACE(rc);
	}

	*size =((uint32_t *) data)[0];

	iface->close(image);
	return rc;
	
}
BERR_Code BRAP_DSP_P_GetFWBaseAddr(BRAP_Handle hRap, BRAP_Img_Id firmware_id, uint32_t *ImgAddr)
{
	void *image;	
	BERR_Code rc;	
	const void *data;	
	const BIMG_Interface *iface = hRap->sSettings.pImgInterface;	
	void *context = hRap->sSettings.pImgContext;

	rc = iface->open (context, &image, (uint32_t)firmware_id);

	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("\nError in Opening the Image Interface for FW_ID =%d \n",firmware_id));
		return BERR_TRACE(rc);
	}

	rc = iface->next(image, 0, &data, 8);

	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("\nError in fetching next chunk in Image Interface\n"));        
		iface->close(image);        
		return BERR_TRACE(rc);
	}

	rc = iface->next(image, 1, &data, 8);

	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("\nError in fetching next chunk in Image Interface\n"));
		iface->close(image);
		return BERR_TRACE(rc);
	}
	

	*ImgAddr = (uint32_t)data;

	iface->close(image);
	return rc;

}


BERR_Code BRAP_DSP_P_InitHardware(BRAP_DSP_Handle hDsp, bool bFwAuthEnabled)
{
    BERR_Code err = BERR_SUCCESS;
    uint32_t	regVal = 0, size;
    uint8_t		dmaDataType, swapType;
    uint32_t	physAddress;
    bool                bWdgRecovery = false;
#ifdef RAP_SCM_SUPPORT	
	BRAP_HashTable sHashTable[BRAP_HashImageTypes_eMax];
	uint32_t ui32RootHash[5] = { 0, 0, 0, 0, 0 };
#endif

    BDBG_ENTER(BRAP_DSP_P_InitHardware);

    if(false == bFwAuthEnabled)
    {
	/* Reset DSP processor and its peripherals */
	/*regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET);*/

	regVal=BRAP_DSP_P_ResetHardware(hDsp);

	/* Withdraw only RESET0_B reset */
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET0_B,1));
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset, regVal);
	BDBG_MSG(("\n**** Soft reset: reset0_b withdrawn = 0x%x",regVal));

        regVal = 0x0;
        /* Withdraw SCBDMA reset before DMA */
        regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset);
        regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET_SCBDMA0_B)))|
        (BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET_SCBDMA0_B,1));
        BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset, regVal);
        BDBG_MSG(("\n**** Soft reset: SCBDMA withdrawn = 0x%x ****",regVal));
    }

    bWdgRecovery= BRAP_P_GetWatchdogRecoveryFlag(hDsp->hRap);
    if(true == bWdgRecovery)
    {
        err = BRAP_DSP_P_EnableDspWatchdogTimer(hDsp,true);
        if (err!=BERR_SUCCESS)
        {
            return BERR_TRACE(err);
        }                
    }

	/* Initialize Mailbox5 register to zero */
        regVal = 0;
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_MAILBOX5 + hDsp->ui32Offset,
	            BRAP_DSP_P_PREBOOT_MAILBOX_PATTERN);
	regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_MAILBOX5 + hDsp->ui32Offset);
	BDBG_MSG(("Mailbox5 = 0x%x",regVal));

	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_MAILBOX0 + hDsp->ui32Offset,
                BRAP_DSP_P_PREBOOT_MAILBOX_PATTERN);
	regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_MAILBOX0 + hDsp->ui32Offset);
	BDBG_MSG(("Mailbox0 = 0x%x",regVal));

	/* Boot the processor */
	dmaDataType = 2;
	swapType = 0;

#ifdef RAP_SCM_SUPPORT

		if ( hDsp->hRap->sSettings.sScmSettings.bSCMEnabled )
		{
			/* DMA System Iboot into SRAM */

			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)hDsp->hRap->sDwnldMemInfo.ui32SystemIbootCodePtr, &physAddress);

			size = hDsp->hRap->sDwnldMemInfo.ui32SystemIbootCodeSize;

			err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size, physAddress, 0);

			/* DMA System code into SRAM */
			/* Note: Scheduler code should always start at 512 even if iboot < 512 */
			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)(hDsp->hRap->sDwnldMemInfo.ui32SystemCodePtr), &physAddress);

			size = hDsp->hRap->sDwnldMemInfo.ui32SystemCodeSize;

	/*		err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size,  physAddress, 512);*/
			err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size - 16,	physAddress + 16, 0x800);

			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesPtr, &physAddress);

			size = hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesSize;

#ifdef BRAP_P_ZSP800
			err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size - 16, physAddress + 16, 0x8020000);
#else
			err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size - 16, physAddress + 16, 0x8000000);
#endif


			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)hDsp->hRap->sDwnldMemInfo.ui32SystemDataPtr, &physAddress);

			size = hDsp->hRap->sDwnldMemInfo.ui32SystemDataSize;

#ifdef BRAP_P_ZSP800
			err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size - 16, physAddress + 16, 0x8020100);
#else
			err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size - 16, physAddress + 16, 0x8000100);
#endif


			/* DMA Signatures to SRAM */
			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)hDsp->hRap->sDwnldMemInfo.ui32SystemTableOfHashesPtr, &physAddress);
		
			size = hDsp->hRap->sDwnldMemInfo.ui32SystemTableOfHashesSize;
		
#ifdef BRAP_P_ZSP800
			err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size, physAddress, 0x802FA00);
#else
			err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size, physAddress, 0x800FA00);
#endif
		
		}
		else
		{
#endif
		/* DMA System Iboot into SRAM */

		BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)hDsp->hRap->sDwnldMemInfo.ui32SystemIbootCodePtr, &physAddress);

		size = hDsp->hRap->sDwnldMemInfo.ui32SystemIbootCodeSize;

		err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size, physAddress, 0);

		/* DMA System code into SRAM */
		/* Note: Scheduler code should always start at 512 even if iboot < 512 */
		BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)(hDsp->hRap->sDwnldMemInfo.ui32SystemCodePtr), &physAddress);

		size = hDsp->hRap->sDwnldMemInfo.ui32SystemCodeSize;

/*		err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size,  physAddress, 512);*/
		err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size,  physAddress + 16, 0x6e0);

        BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesPtr, &physAddress);

        size = hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesSize;

#ifdef BRAP_P_ZSP800
		err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size, physAddress + 16, 0x8020000);
#else
		err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size, physAddress + 16, 0x8000000);
#endif


		BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)hDsp->hRap->sDwnldMemInfo.ui32SystemDataPtr, &physAddress);

		size = hDsp->hRap->sDwnldMemInfo.ui32SystemDataSize;

#ifdef BRAP_P_ZSP800
		err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size, physAddress + 16, 0x8020100);
#else
		err = BRAP_DSP_P_DmaIn(hDsp, dmaDataType, swapType, size, physAddress + 16, 0x8000100);
#endif



#ifdef RAP_SCM_SUPPORT

		}

		if ( hDsp->hRap->sSettings.sScmSettings.bSCMEnabled )
		{
		
			uint32_t ui32ImgBaseAddr;
			uint32_t ui32ImgSize;
			
			BDBG_ERR(("BRAP_DSP_P_InitHardware : Authenticating Resident Code/Data/Signatures using SHA1"));

		    BKNI_EnterCriticalSection(); /* SHA1 Authentication Should be Atomic */

			ui32ImgBaseAddr = (uint32_t)BRAP_P_AllocAligned(hDsp->hRap, hDsp->hRap->sDwnldMemInfo.ui32SystemIbootCodeSize,2, 0);
			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)ui32ImgBaseAddr, &physAddress);
			err = BRAP_DSP_P_DmaOut(hDsp, dmaDataType, swapType, hDsp->hRap->sDwnldMemInfo.ui32SystemIbootCodeSize, 0, physAddress);

			BRAP_SHA_P_ComputeSha1(ui32ImgBaseAddr,(hDsp->hRap->sDwnldMemInfo.ui32SystemIbootCodeSize)*8,sHashTable[BRAP_HashImageTypes_eIboot].ui32HashValue);			
			BRAP_P_Free(hDsp->hRap, (void *)ui32ImgBaseAddr);


			ui32ImgBaseAddr = (uint32_t)BRAP_P_AllocAligned(hDsp->hRap, (hDsp->hRap->sDwnldMemInfo.ui32SystemCodeSize ),2, 0);
			BKNI_Memcpy((void *)ui32ImgBaseAddr, (const void *)hDsp->hRap->sDwnldMemInfo.ui32SystemCodePtr, 16);
			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)(ui32ImgBaseAddr + 16), &physAddress);
			err = BRAP_DSP_P_DmaOut(hDsp, dmaDataType, swapType, hDsp->hRap->sDwnldMemInfo.ui32SystemCodeSize - 16, 0x800, physAddress);

			BRAP_SHA_P_ComputeSha1(ui32ImgBaseAddr,(hDsp->hRap->sDwnldMemInfo.ui32SystemCodeSize)*8,sHashTable[BRAP_HashImageTypes_eSystemCode].ui32HashValue);			
			BRAP_P_Free(hDsp->hRap, (void *)ui32ImgBaseAddr);


			ui32ImgBaseAddr = (uint32_t)BRAP_P_AllocAligned(hDsp->hRap, hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesSize,2, 0);
			BKNI_Memcpy((void *)ui32ImgBaseAddr, (const void *)hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesPtr, 16);
			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)(ui32ImgBaseAddr + 16), &physAddress);
#ifdef BRAP_P_ZSP800
			err = BRAP_DSP_P_DmaOut(hDsp, dmaDataType, swapType, hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesSize - 16, 0x8020000, physAddress);
#else
			err = BRAP_DSP_P_DmaOut(hDsp, dmaDataType, swapType, hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesSize - 16, 0x8000000, physAddress);
#endif

			BRAP_SHA_P_ComputeSha1(ui32ImgBaseAddr,(hDsp->hRap->sDwnldMemInfo.ui32SystemRdbVariablesSize)*8,sHashTable[BRAP_HashImageTypes_eSystemRdb].ui32HashValue);			
			BRAP_P_Free(hDsp->hRap, (void *)ui32ImgBaseAddr);


			ui32ImgBaseAddr = (uint32_t)BRAP_P_AllocAligned(hDsp->hRap, hDsp->hRap->sDwnldMemInfo.ui32SystemDataSize,2, 0);
			BKNI_Memcpy((void *)ui32ImgBaseAddr, (const void *)hDsp->hRap->sDwnldMemInfo.ui32SystemDataPtr, 16);
			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)(ui32ImgBaseAddr + 16), &physAddress);
#ifdef BRAP_P_ZSP800
			err = BRAP_DSP_P_DmaOut(hDsp, dmaDataType, swapType, hDsp->hRap->sDwnldMemInfo.ui32SystemDataSize - 16, 0x8020100, physAddress);
#else
			err = BRAP_DSP_P_DmaOut(hDsp, dmaDataType, swapType, hDsp->hRap->sDwnldMemInfo.ui32SystemDataSize - 16, 0x8000100, physAddress);
#endif

			BRAP_SHA_P_ComputeSha1(ui32ImgBaseAddr,(hDsp->hRap->sDwnldMemInfo.ui32SystemDataSize)*8,sHashTable[BRAP_HashImageTypes_eSystemData].ui32HashValue);			
			BRAP_P_Free(hDsp->hRap, (void *)ui32ImgBaseAddr);			


			ui32ImgBaseAddr = (uint32_t)BRAP_P_AllocAligned(hDsp->hRap, hDsp->hRap->sDwnldMemInfo.ui32SystemTableOfHashesSize,2, 0);
			BRAP_P_ConvertAddressToOffset(hDsp->hHeap, (void *)ui32ImgBaseAddr, &physAddress);
#ifdef BRAP_P_ZSP800
			err = BRAP_DSP_P_DmaOut(hDsp, dmaDataType, swapType, hDsp->hRap->sDwnldMemInfo.ui32SystemTableOfHashesSize, 0x802FA00, physAddress);
#else
			err = BRAP_DSP_P_DmaOut(hDsp, dmaDataType, swapType, hDsp->hRap->sDwnldMemInfo.ui32SystemTableOfHashesSize, 0x800FA00, physAddress);
#endif

			BRAP_SHA_P_ComputeSha1(ui32ImgBaseAddr,(hDsp->hRap->sDwnldMemInfo.ui32SystemTableOfHashesSize)*8,sHashTable[BRAP_HashImageTypes_eTableOfHashes].ui32HashValue);			
			BRAP_P_Free(hDsp->hRap, (void *)ui32ImgBaseAddr);			


			BRAP_DSP_P_GetFWSize (hDsp->hRap, BRAP_Img_Id_eScm_1_Code, &ui32ImgSize);
			BRAP_DSP_P_GetFWBaseAddr(hDsp->hRap, BRAP_Img_Id_eScm_1_Code, &ui32ImgBaseAddr);
			BRAP_SHA_P_ComputeSha1(ui32ImgBaseAddr,ui32ImgSize * 8,sHashTable[BRAP_HashImageTypes_eScm_1_Code].ui32HashValue); 		

			BRAP_DSP_P_GetFWSize (hDsp->hRap, BRAP_Img_Id_eScm_2_Code, &ui32ImgSize);
			BRAP_DSP_P_GetFWBaseAddr(hDsp->hRap, BRAP_Img_Id_eScm_2_Code, &ui32ImgBaseAddr);
			BRAP_SHA_P_ComputeSha1(ui32ImgBaseAddr,ui32ImgSize * 8,sHashTable[BRAP_HashImageTypes_eScm_2_Code].ui32HashValue); 		

			BRAP_DSP_P_GetFWSize (hDsp->hRap, BRAP_Img_Id_eSCMTask, &ui32ImgSize);
			BRAP_DSP_P_GetFWBaseAddr(hDsp->hRap, BRAP_Img_Id_eSCMTask, &ui32ImgBaseAddr);
			BRAP_SHA_P_ComputeSha1(ui32ImgBaseAddr,ui32ImgSize * 8,sHashTable[BRAP_HashImageTypes_eScmTask].ui32HashValue);	
			
			

			BRAP_SHA_P_ComputeSha1((uint32_t)sHashTable,(sizeof(sHashTable))*8,ui32RootHash);			

			err = BRAP_SCM_P_AuthenticateSignature(ui32RootHash, hDsp->hRap->sSettings.sScmSettings.ui32RefHashDigest);
			
			if ( BERR_SUCCESS != err )
			{
				BDBG_ERR(("SCM Authentication Failed"));
				BKNI_LeaveCriticalSection();
				return err;
			}

			if ( hDsp->hRap->sSettings.sScmSettings.pfCallbackOtpProgramming )
			{
				BRAP_DSPCHN_ScmType		eScmAlgoType;
				uint32_t ui32Scm_1_versionid[4] = { 0 };
				uint32_t ui32Scm_2_versionid    = 0;
				void *   pScmVersionId          = NULL;
				uint32_t ui32ImgBaseAddr;
				
				eScmAlgoType = BRAP_SCM_P_GetSCMAlgorithmID(hDsp->hRap->sSettings.sScmSettings.ui32CaSystemId);
				
				switch(eScmAlgoType)
				{
					case BRAP_DSPCHN_ScmType_eScm_1:
					BRAP_DSP_P_GetFWBaseAddr(hDsp->hRap, BRAP_Img_Id_eScm_1_Code, &ui32ImgBaseAddr);	
					BKNI_Memcpy((void *)ui32Scm_1_versionid, (const void *)ui32ImgBaseAddr, 16);
					pScmVersionId = (void *)ui32Scm_1_versionid;
					break;	
					case BRAP_DSPCHN_ScmType_eScm_2:
					BRAP_DSP_P_GetFWBaseAddr(hDsp->hRap, BRAP_Img_Id_eScm_2_Code, &ui32ImgBaseAddr);	
					BKNI_Memcpy((void *)&ui32Scm_2_versionid, (const void *)ui32ImgBaseAddr, 4);						
					pScmVersionId = (void *)&ui32Scm_2_versionid;
					break;						
					default:
					err = BERR_INVALID_PARAMETER;
					break;							
				}
				
				if ( BERR_SUCCESS != err )
				{
					BDBG_ERR(("SCM Version ID Parsing Failed"));
					BKNI_LeaveCriticalSection();
					return err;
				}
				
				err = hDsp->hRap->sSettings.sScmSettings.pfCallbackOtpProgramming(pScmVersionId, hDsp->hRap->sSettings.sScmSettings.ui32CaSystemId);

				if ( BERR_SUCCESS != err )
				{
					BDBG_ERR(("OTP Programming Failed"));
					BKNI_LeaveCriticalSection();
					return err;
				}
				
			}	

		    BKNI_LeaveCriticalSection(); /*SHA1 Authentication Should be Atomic */

		}		
#endif


#ifndef RAP_SCM_SUPPORT

	/* Withdraw processor resets */
	regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset);
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET_PROC0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET_PROC0_B,1));
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET + hDsp->ui32Offset, regVal);
	BDBG_MSG(("\n**** Soft reset: reset withdrawn = 0x%x ****",regVal));

	BDBG_MSG(("Mailbox5 = 0x%x",regVal));
#endif	
        BDBG_LEAVE(BRAP_DSP_P_InitHardware);
	return err;
}


static BERR_Code BRAP_DSP_P_DmaIn(	BRAP_DSP_Handle hDsp,
								uint8_t dataType,
								uint8_t swapType,
								uint32_t size,
								uint32_t src_addr,
								uint32_t dst_addr)
{
	uint32_t regVal, dmaCount = 0;
	BERR_Code err = BERR_SUCCESS;

	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_SRC_ADDR0 + hDsp->ui32Offset,src_addr);
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_DST_ADDR0 + hDsp->ui32Offset,dst_addr);
	/* regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_TRANSFER0); */
	regVal = 0x0;
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_DMA_TRANSFER0,SWAP_TYPE)))|
				(BCHP_FIELD_DATA(AUD_DSP_CTRL0_DMA_TRANSFER0,SWAP_TYPE,swapType));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_DMA_TRANSFER0,DATA_TYPE)))|
				(BCHP_FIELD_DATA(AUD_DSP_CTRL0_DMA_TRANSFER0,DATA_TYPE,dataType));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_DMA_TRANSFER0,TRAN_TYPE)))|
				(BCHP_FIELD_ENUM(AUD_DSP_CTRL0_DMA_TRANSFER0,TRAN_TYPE,Read));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_DMA_TRANSFER0,NUM_BYTES)))|
				(BCHP_FIELD_DATA(AUD_DSP_CTRL0_DMA_TRANSFER0,NUM_BYTES,size));
	BDBG_MSG(("\n**** DMA TRANSFER0 = 0x%x", regVal));
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_TRANSFER0 + hDsp->ui32Offset,regVal);
	BDBG_MSG(("Waiting for DMA to complete..."));	
	regVal = BRAP_DSP_P_DMA_BUSY;
	while(regVal){
		regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_STATUS + hDsp->ui32Offset);
		regVal = BCHP_GET_FIELD_DATA(regVal, AUD_DSP_CTRL0_DMA_STATUS, BUSY0);
		dmaCount++;
		if (dmaCount==BRAP_DSP_P_DMA_TIMEOUT_COUNT) {
			BDBG_ERR(("DMA timeout: SRC Adr = 0x%x, DST Adr = 0x%x, size = 0x%x", src_addr, dst_addr, size));
			return BERR_TRACE(BERR_TIMEOUT);
		}
		BKNI_Delay(10);	/* 10 us of delay for total timeout of (BRAP_DSP_P_DMA_TIMEOUT_COUNT * 0.01) ms */
	}
	BDBG_MSG(("Done!"));
	return err;
}

#ifdef RAP_SCM_SUPPORT

static BERR_Code BRAP_DSP_P_DmaOut(	BRAP_DSP_Handle hDsp,
								uint8_t dataType,
								uint8_t swapType,
								uint32_t size,
								uint32_t src_addr,
								uint32_t dst_addr)
{
	uint32_t regVal, dmaCount = 0;
	BERR_Code err = BERR_SUCCESS;

	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_SRC_ADDR0 + hDsp->ui32Offset,src_addr);
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_DST_ADDR0 + hDsp->ui32Offset,dst_addr);
	/* regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_TRANSFER0); */
	regVal = 0x0;
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_DMA_TRANSFER0,SWAP_TYPE)))|
				(BCHP_FIELD_DATA(AUD_DSP_CTRL0_DMA_TRANSFER0,SWAP_TYPE,swapType));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_DMA_TRANSFER0,DATA_TYPE)))|
				(BCHP_FIELD_DATA(AUD_DSP_CTRL0_DMA_TRANSFER0,DATA_TYPE,dataType));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_DMA_TRANSFER0,TRAN_TYPE)))|
				(BCHP_FIELD_ENUM(AUD_DSP_CTRL0_DMA_TRANSFER0,TRAN_TYPE,Write));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_CTRL0_DMA_TRANSFER0,NUM_BYTES)))|
				(BCHP_FIELD_DATA(AUD_DSP_CTRL0_DMA_TRANSFER0,NUM_BYTES,size));
	BDBG_MSG(("\n**** DMA TRANSFER0 = 0x%x", regVal));
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_TRANSFER0 + hDsp->ui32Offset,regVal);
	BDBG_MSG(("Waiting for DMA to complete..."));	
	regVal = BRAP_DSP_P_DMA_BUSY;
	while(regVal){
		regVal = BRAP_Read32(hDsp->hRegister,BCHP_AUD_DSP_CTRL0_DMA_STATUS + hDsp->ui32Offset);
		regVal = BCHP_GET_FIELD_DATA(regVal, AUD_DSP_CTRL0_DMA_STATUS, BUSY0);
		dmaCount++;
		if (dmaCount==BRAP_DSP_P_DMA_TIMEOUT_COUNT) {
			BDBG_ERR(("DMA timeout: SRC Adr = 0x%x, DST Adr = 0x%x, size = 0x%x", src_addr, dst_addr, size));
			return BERR_TRACE(BERR_TIMEOUT);
		}
		BKNI_Delay(10);	/* 10 us of delay for total timeout of (BRAP_DSP_P_DMA_TIMEOUT_COUNT * 0.01) ms */
	}
	BDBG_MSG(("Done!"));
	return err;
}

#endif

static uint32_t BRAP_DSP_P_ResetHardware(BRAP_DSP_Handle hDsp)
{
	uint32_t regVal;

	BDBG_ASSERT(hDsp);
	BDBG_ASSERT(hDsp->hRegister);
	/* Reset DSP processor and its peripherals */

	regVal = 0;
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET_SCBDMA0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET_SCBDMA0_B,0));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET_PROC0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET_PROC0_B,0));
	regVal = (regVal & ~(BCHP_MASK(AUD_DSP_MISC_SOFT_RESET,RESET0_B)))|
				(BCHP_FIELD_DATA(AUD_DSP_MISC_SOFT_RESET,RESET0_B,0));
	BRAP_Write32(hDsp->hRegister,BCHP_AUD_DSP_MISC_SOFT_RESET, regVal);
	BDBG_MSG(("\n**** Soft reset: reset all = 0x%x",regVal));
	return regVal;
}

BERR_Code BRAP_DSP_P_CopyFWImageToMem (BRAP_DSP_Handle hDsp, uint32_t memAdr, uint32_t firmware_id)
{
       BSTD_UNUSED(hDsp);
       BSTD_UNUSED(memAdr);
       BSTD_UNUSED(firmware_id);
	return BERR_SUCCESS;
}
