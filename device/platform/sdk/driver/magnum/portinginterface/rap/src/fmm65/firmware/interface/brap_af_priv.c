/***************************************************************************
*     Copyright (c) 2003-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_af_priv.c $
* $brcm_Revision: Hydra_Software_Devel/170 $
* $brcm_Date: 6/1/11 7:58p $
*
* Module Description: 
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_af_priv.c $
* 
* Hydra_Software_Devel/170   6/1/11 7:58p arnabb
* SW7405-4673:[7420] Dolby Pulse development for Dolby MS11. SW7420-
* 1881:[7420] Adding chan map in stream info structure for ddp.
* 
* Hydra_Software_Devel/RAP_MS111_DOLBY_PULSE_SUPPORT_BRANCH/4   5/31/11 3:44p arnabb
* SW7405-4673:[7420]Dolby Pulse development for Dolby MS11.
* 
* Hydra_Software_Devel/RAP_MS111_DOLBY_PULSE_SUPPORT_BRANCH/3   5/30/11 8:12p arnabb
* SW7420-1881:[7420] Updating new iogeneric buff requirement.
* 
* Hydra_Software_Devel/RAP_MS111_DOLBY_PULSE_SUPPORT_BRANCH/2   5/27/11 3:36p arnabb
* SW7405-4673:[7420] Dolby Pulse development for Dolby MS11. SW7420-
* 1896:[7420] Adding CIT changes to handle sample rate issue.
* 
* Hydra_Software_Devel/169   3/18/11 7:17p tthomas
* SW7420-767:[7420] Merging MS11 to main/latest
* 
* Hydra_Software_Devel/RAP_MS11_DEV_REV1/NEXTGEN_MS11_TOMAIN_BRANCH/3   3/18/11 6:58p tthomas
* SW7420-767:[7420] ADding more comments
* 
* Hydra_Software_Devel/RAP_MS11_DEV_REV1/NEXTGEN_MS11_TOMAIN_BRANCH/2   3/9/11 11:17a tthomas
* SW7550-648: [7550] Removing the #ifdef RAP_SCM_SUPPORT
* 
* Hydra_Software_Devel/168   3/9/11 10:55a tthomas
* SW7550-648: [7550] Removing the #ifdef RAP_SCM_SUPPORT
* 
* Hydra_Software_Devel/RAP_MS11_DEV_REV1/NEXTGEN_MS11_TOMAIN_BRANCH/1   3/8/11 7:42p tthomas
* SW7420-767:[7420] Merging the Main latest Code to MSS11 branch
* 
* Hydra_Software_Devel/RAP_MS11_DEV_REV1/2   1/11/11 1:42p tthomas
* SW7420-767:[7420] Adding CIT support for the Mixer task and SIPS API
* initital Verison tested.. All interface changes are present ..
* 
* Hydra_Software_Devel/RAP_MS11_DEV_REV1/1   1/10/11 11:15a tthomas
* SW7420-767:[7420] Adding CIT support for the Mixer task and SIPS API
* initital Verison.. All interface changes are present ..
* Hydra_Software_Devel/167   3/7/11 6:05p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/166   2/22/11 12:14p tthomas
* SW7405-5060: [3549] Adding Changes to SRS Studio user cfg
* 
* Hydra_Software_Devel/165   2/10/11 6:56p tthomas
* SW7550-637 : [7550] Adding Multiple Sampling Freq support
* 
* Hydra_Software_Devel/164   2/8/11 2:13p tthomas
* SW7405-4914: [7405] Increasing User configuration size for DD pulse
* 
* Hydra_Software_Devel/RAP_MS11_DEV_REV1/2   1/11/11 1:42p tthomas
* SW7420-767:[7420] Adding CIT support for the Mixer task and SIPS API
* initital Verison tested.. All interface changes are present ..
* 
* Hydra_Software_Devel/RAP_MS11_DEV_REV1/1   1/10/11 11:15a tthomas
* SW7420-767:[7420] Adding CIT support for the Mixer task and SIPS API
* initital Verison.. All interface changes are present ..
* Hydra_Software_Devel/163   11/16/10 12:29p dinesha
* SW7550-533: [7550]Additional ZSP GFX Functionality/APIs required.
* modified gfx interframe size.

* 
* Hydra_Software_Devel/162   10/26/10 4:08p tthomas
* SW3556-1189: [3556] Updating Interframe Buffer size
* 
* Hydra_Software_Devel/161   10/21/10 3:33p tthomas
* SW7550-601 : [7550] Adding CIT support for BTSC encoder
* 
* Hydra_Software_Devel/160   10/13/10 3:46p tthomas
* SW7550-601 : [7550] Adding CIT support for BRSC encoder
* 
* Hydra_Software_Devel/158   9/27/10 4:08p tthomas
* SW3548-2923: [3548] Updated: Fixing PpOutMode for SRS HD.
* 
* Hydra_Software_Devel/157   9/15/10 5:07p tthomas
* SW7468-298 : [7468] Merging the CIT changes for Video to main latest
* 
* Hydra_Software_Devel/154   8/26/10 3:45p tthomas
* SW7550-547 : [7550] Adding support for the Audio/Video/Graphic task
* binaries.
* 
* Hydra_Software_Devel/153   8/17/10 4:57p tthomas
* SW7420-767: [7420]Changing the scratch buffer size for MS10 ddp
* 
* Hydra_Software_Devel/152   8/17/10 2:46p tthomas
* SW7420-767: [7420]Changing the IO buffer size for MS10 ddp
* 
* Hydra_Software_Devel/151   8/10/10 1:19p tthomas
* SW7420-767: [7420] Merging the MS11 feature to main/latest
* 
* Hydra_Software_Devel/RAP_MS11_DEV/4   7/15/10 6:33p tthomas
* SW7420-767: [7420] Modifying user cfg size for FWMixer
* 
* Hydra_Software_Devel/RAP_MS11_DEV/3   6/29/10 11:39a tthomas
* SW7420-767: [7420] Modifying iF for DDRE
* 
* Hydra_Software_Devel/RAP_MS11_DEV/2   6/25/10 4:24p tthomas
* SW7420-767: [7420] modifying the buffer alloc
* 
* Hydra_Software_Devel/RAP_MS11_DEV/1   6/7/10 5:26p gautamk
* SW7420-767: [7420] Adding support for DDRE Compressor
* 
* Hydra_Software_Devel/150   8/9/10 11:41a tthomas
* SW7405-3359 :[7405] increasing the buffer size  TSM interframe
* 
* Hydra_Software_Devel/149   7/26/10 5:35p tthomas
* SW3548-2899 : [3548] Adding Status buffer for ADV
* 
* Hydra_Software_Devel/148   7/15/10 3:14p tthomas
* SW3548-2923 : [3548] modifying the buffer size for  SRS Studio user cfg
* 
* Hydra_Software_Devel/147   5/31/10 4:56p arung
* SW3548-2923:[3548] Max Num channels supported by circle surround stage
* is 7.
* 
* Hydra_Software_Devel/146   5/28/10 3:37p arung
* SW3548-2923: [3548] Changing the inter frame and user configuration
* buffer sizes for SRS TSHD
* 
* Hydra_Software_Devel/144   5/18/10 12:12p tthomas
* SW7405-4356 : [7405] Changing the IF and Io generic buffer for all IDS
* 
* Hydra_Software_Devel/143   5/17/10 2:45p tthomas
* SW3548-2923: [3548] Adding the SRS Studio Support with Stages
* 
* Hydra_Software_Devel/142   5/17/10 12:36p tthomas
* SW7405-4052: [7405] Adding MS10 Interface changes to main line
* 
* Hydra_Software_Devel/141   5/12/10 1:56p tthomas
* SW7405-4322 : [7405]Adding DDRE Support and DV258 Support
* 
* Hydra_Software_Devel/RAP_MS10_DEVEL/1   4/28/10 3:17p tthomas
* SW7405-4052: [7405] Adding MS10 Interface changes
* Hydra_Software_Devel/139   4/26/10 11:57a tthomas
* SW7405-3492: [7405]Spelling correction for Audyssey
* 
* Hydra_Software_Devel/138   4/22/10 4:24p tthomas
* SW7405-3492: [7405] Modifying the buffer requirements for Dolby Pulse
* 
* Hydra_Software_Devel/137   4/21/10 12:49p tthomas
* SW7405-3492:[7405] Modifying the DD Pulse stage IDS
* 
* Hydra_Software_Devel/136   4/20/10 2:42p tthomas
* SW7405-3492:[7405] Adding MS10 DD Decoder Status Buff
* 
* Hydra_Software_Devel/135   4/20/10 2:24p tthomas
* SW3548-2899 : [3548] Adding support for ABX
* 
* Hydra_Software_Devel/134   4/19/10 5:49p tthomas
* SW7405-3492:[7405] Adding MS10 DD Decoder and MS10 Convert
* 
* Hydra_Software_Devel/133   4/12/10 7:35p tthomas
* SW7405-3492:[7405] Adding MS10 DD Decoder and MS10 Convert
* 
* Hydra_Software_Devel/132   3/11/10 3:30p tthomas
* SW3548-2816 : [3548] Making  DD Xcode as Encode
* 
* Hydra_Software_Devel/131   3/10/10 7:09a tthomas
* SW3543-1179 : [3548] Adding the CIT support for Mono Downmix
* 
* Hydra_Software_Devel/130   3/9/10 5:04p tthomas
* SW3548-2816 : [3548] Adding Support for FW Mixer and DD Xcode
* 
* Hydra_Software_Devel/129   3/9/10 4:28p tthomas
* SW7405-3492: [7405] Modifying the buffer requirements for Dolby Pulse
* 
* Hydra_Software_Devel/128   3/4/10 7:13p tthomas
* SW3548-2809  : [3548] Adding support for BRCM 3D support
* 
* Hydra_Software_Devel/127   3/4/10 3:26a tthomas
* SW7405-3993 : [7405] Adding Support for Audyssey
* 
* Hydra_Software_Devel/126   2/26/10 6:34a tthomas
* SW7405-3492: [7405] Modifying the buffer requirements for Dolby Pulse
* 
* Hydra_Software_Devel/125   2/23/10 1:56p tthomas
* SW7405-3492: [7405]Adding support for Dolby Pulse
* 
* Hydra_Software_Devel/124   2/19/10 12:04p tthomas
* CR3548-877: [7405] Adding Real Audio Support
* 
* Hydra_Software_Devel/123   2/2/10 11:52a tthomas
* SW7405-3639 : [7405] Modifying DDp buffer sizes
* 
* Hydra_Software_Devel/122   1/19/10 11:28a tthomas
* SW7405-2001 : [7405] Cleanup of cit files
* 
* Hydra_Software_Devel/121   12/15/09 7:48p tthomas
* SW7405-2001 : [7405] increasing the size of CV IF
* 
* Hydra_Software_Devel/120   11/17/09 10:32a tthomas
* SW7405-2001 : [7405] Adding the const for BRAP_sPpOutMode array
* 
* Hydra_Software_Devel/119   11/17/09 10:13a tthomas
* SW7405-2001 :[7405] Removing the interframe buffer for WMA PT
* 
* Hydra_Software_Devel/118   11/3/09 10:35a tthomas
* SW3548-2566:[3548] Modifying the user cfg size for Clear Voice
* 
* Hydra_Software_Devel/117   10/28/09 12:22p tthomas
* SW7405-2959  : [7405] Adding the DTSHD Frame sync Support
* 
* Hydra_Software_Devel/116   10/22/09 7:28p arung
* SW3548-2566:[3548] Port New Custom Voice code
* Changing the size for the new user configuration structure of custom
* voice.
* 
* Hydra_Software_Devel/115   9/30/09 5:48p tthomas
* SW7325-609: [7405] Allocating status buffer for DTS decoder
* 
* Hydra_Software_Devel/114   9/25/09 4:23p tthomas
* SW7420-352:[7420]: Modifying the WMA pro memory
* 
* Hydra_Software_Devel/113   9/18/09 7:00p dinesha
* SW7420-352:[7420] Check in the Firmware files corresponding to latest
* Nexus/BSEAV. Corresponding to Rap 6.5 release for 7405.
* Modified The interframe file for SRS TruVolume.
* 
* Hydra_Software_Devel/112   9/17/09 6:10p tthomas
* SW7405-2967 : [7405] Modifying the IF buffer size of AMR and DRA
* 
* Hydra_Software_Devel/111   9/4/09 10:11a tthomas
* SW7405-2967 : [7405] Adding support for DRA Decoder
* 
* Hydra_Software_Devel/106   7/10/09 10:45a tthomas
* PR47682:[7405] Modofying number of channels for DDP and Ac3 from 6 to 8
* 
* Hydra_Software_Devel/105   7/8/09 1:38p tthomas
* PR47682:[7405] Modofying the scratch buffer size for Dolby volume
* 
* Hydra_Software_Devel/104   7/6/09 4:25p tthomas
* PR53764 : [7405] Memory Optimization is done for algos
* 
* Hydra_Software_Devel/101   6/23/09 3:07p tthomas
* PR47682:[7405] Merging AACHE V2 Support decoder to main line
* 
* Hydra_Software_Devel/100   6/12/09 3:45p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/99   6/11/09 4:30p tthomas
* PR47949: [7405] Modifying table for TDS broadcast encoer
* 
* Hydra_Software_Devel/98   6/9/09 10:46p tthomas
* PR47949: [7405] Giving the more optimized number for PCM WAV
* 
* Hydra_Software_Devel/97   6/9/09 11:36a gautamk
* PR47949: [7405]Changing name from PCMOnAVI to PCMWAV
* 
* Hydra_Software_Devel/96   6/5/09 10:56a tthomas
* PR47949: [7405] Adding support for DTS broadcast decoder PT
* 
* Hydra_Software_Devel/95   6/4/09 3:10p gautamk
* PR47949: [7405] Modifying comments and variable names
* 
* Hydra_Software_Devel/NEXTGENMAGNUM_AACHEV2_BRANCH/1   6/3/09 9:16a tthomas
* PR47682:[7405] Adding AACHE V2 Support decoder
* 
* Hydra_Software_Devel/94   6/2/09 10:16p tthomas
* PR47682:[7405] Adding PCM Wav decoder
* 
* Hydra_Software_Devel/93   5/5/09 4:07p tthomas
* PR47682:[7405] Modifying Algo IDS for DTS broadcast decoder
* 
* Hydra_Software_Devel/92   5/5/09 3:23p tthomas
* PR47682:[7405] Modifying the memory requirement for DTS broadcast
* decoder
* 
* Hydra_Software_Devel/91   5/4/09 5:32p tthomas
* PR47682:[7405] Adding support for DTS broadcast decoder
* 
* Hydra_Software_Devel/87   3/11/09 8:58p tthomas
* PR52015: [3548] Merging the Custom voice status buffer to Mainline and
* increasing SRC iogeneric size
*  
* Hydra_Software_Devel/85   2/27/09 3:37p tthomas
* PR47682:[7405] Modifying the buffer allocation for SRC
* 
* Hydra_Software_Devel/84   2/10/09 10:01a tthomas
* PR47682:[3548] Modifying the SrsVolume Iq name to SrsTruVolume
* 
* Hydra_Software_Devel/83   2/2/09 4:28p gautamk
* PR51652: [7405]
* PR51618: [7405] Changing the global variables as per following rule.
* 1) make them static
* 2) if you cannot make them static because they must be shared between
* multiple .c files in your module, you must rename them to include the
* BXXX_ magnum prefix for your module.
* 
* Hydra_Software_Devel/82   1/28/09 1:08p tthomas
* PR47683: [3548,3556] Increasing the interframe buffer size allocation.
* 
* Hydra_Software_Devel/RAP_AC3Enc_Certification/3   1/27/09 6:40p sgadara
* PR47683: [3548,3556] Increasing the interframe buffer size allocation.
* 
* Hydra_Software_Devel/RAP_AC3Enc_Certification/2   1/22/09 11:13a tthomas
* PR47683: [3548] modifying the Total IO buffer size in CDB PT
* 
* Hydra_Software_Devel/RAP_AC3Enc_Certification/1   1/21/09 7:39p tthomas
* PR47683: [3548] Increasing the number of output channels supported for
* CDBPT to 8. This is to support PCM PT
* 
* Hydra_Software_Devel/81   1/13/09 11:56a tthomas
* PR47683: [3548] Modifying the Scratch memory for DSOLA
* 
* Hydra_Software_Devel/NEXTGEN_MAGNUM_LOWDELAY_BRANCH/1   1/12/09 8:18p tthomas
* PR47683: [3548] Modifying the Scratch memory for DSOLA
* 
* Hydra_Software_Devel/80   1/2/09 12:29p tthomas
* PR48850:[3548] Checkin updated size for SRS Volume IQ
* 
* Hydra_Software_Devel/79   12/24/08 1:44p gautamk
* PR48850:[7405] Checkin updated size for DVD LPCM
* 
* Hydra_Software_Devel/78   12/22/08 4:43p gautamk
* PR48850: [7405] Fixing compilation warning.
* 
* Hydra_Software_Devel/77   12/19/08 2:21p tthomas
* PR47683: [3548] Adding support for dummy DTS decoder and Generic
* passthrough as a stage
* 
* Hydra_Software_Devel/76   12/10/08 10:31a tthomas
* PR47683: [3548] Adding support for dummy DTS decoder and Generic
* passthrough as a stage
* 
* Hydra_Software_Devel/75   11/25/08 11:34a tthomas
* PR47682: [3548] Changing LPCM Stage ID
* 
* Hydra_Software_Devel/74   11/21/08 4:53p tthomas
* PR47682: [3548] Increasing the buffer sizes for DVDLPCM
* 
* Hydra_Software_Devel/73   11/5/08 1:09p tthomas
* PR47682: [3548] Adding new code base support for AACHE decoder
* 
* Hydra_Software_Devel/72   10/24/08 2:18p tthomas
* PR47683: [7405] Adding support for FMM port datatype for the use of
* Zero/Pause burst filling in FW
* 
* Hydra_Software_Devel/71   10/20/08 4:30p tthomas
* PR47688: [7405] The number of output buffers for PL2 is changed form 2
* to 6
* 
* Hydra_Software_Devel/70   10/10/08 11:46a tthomas
* PR 47682: [7405] Adding the PostProcessing Outmode configuration
* structures
* Hydra_Software_Devel/69   10/8/08 11:04a tthomas
* PR 43454:[7405] Modifying the Interframe buffer requirement for AC3 and
* DDP stage 2 passthrough
* 
* Hydra_Software_Devel/68   9/25/08 12:06p tthomas
* PR 43454:[7405] Modifying the Interframe buffer requirement for CDB
* passthrough
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/4   10/8/08 6:48p speter
* PR 47226: [3548,3556] Adding code in RAP PI to provide the audio mode
* to all post processing algorithm stages.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/3   9/25/08 12:23p speter
* PR 47311: [3548,3556] Adding the fix provided in the release branch.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/2   9/24/08 6:35p speter
* PR 43454:[7405] Adding the changes required for DTS Encode firmware
* code
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   9/23/08 4:54p speter
* PR 43454:[7405] Fixing the memory changes for SRC as it is causing
* Audio Descriptor failure. This needs to be verified with CIT module
* owner before being merged to latest line of development
* 
* Hydra_Software_Devel/67   9/20/08 10:01p tthomas
* PR 43454:[7405] Adding support for DTS frame sync and memory changes
* for Ac3 Encoder and SRC
* 
* Hydra_Software_Devel/65   9/10/08 2:43p tthomas
* PR 43454 : [7405] Adding support for Ac3 Encoder
* 
* Hydra_Software_Devel/64   9/5/08 2:24p tthomas
* PR41726: [7405] Adding memory allocation for User cfg in all IDS
* 
* Hydra_Software_Devel/63   9/4/08 4:37p tthomas
* PR41726: [7405] Adding support for SRS-HD PP
* 
* Hydra_Software_Devel/62   9/1/08 10:08a tthomas
* PR41726: [7405] Modifying the memory allocation and usercfg for Custom
* Voice III
* 
* Hydra_Software_Devel/61   8/26/08 12:07p tthomas
* PR41726: [7405]Modifying the memory for DTS encoder
* 
* Hydra_Software_Devel/60   8/26/08 12:04p tthomas
* PR41726: [7405] Adding Support For Custom Surround and Bass
* 
* Hydra_Software_Devel/58   8/21/08 12:08p tthomas
* PR41726: [7405] Modifying the memory requirement for new user cfg
* 
* Hydra_Software_Devel/57   8/21/08 10:56a tthomas
* PR41726: [7405]Removing the commeted code
* 
* Hydra_Software_Devel/56   8/11/08 12:06p sushmit
* PR43454:[7405]DTSENC Upd.
* 
* Hydra_Software_Devel/55   8/11/08 11:28a tthomas
* PR 43454:[7405] Adding support for 1) DSOLA 2) DVDLPCM decode
* 3)  DVDLPCM IDS
* 
* Hydra_Software_Devel/54   8/10/08 11:55a sushmit
* PR 43454:[7405] DTSENC Updates.
* 
* Hydra_Software_Devel/53   8/4/08 4:54p tthomas
* PR41726: [7405] Modifying the user cfg size for custom voice II v1
* 
* Hydra_Software_Devel/52   8/4/08 4:17p tthomas
* PR41726: [7405] Modifying the code size for custom voice II v1
* 
* Hydra_Software_Devel/50   7/25/08 7:49p tthomas
* PR41726: [7405] Modifying the Output generic buffer size for WMA Stage2
* when passthrough is enabled
* 
* Hydra_Software_Devel/49   7/21/08 3:29p tthomas
* PR41726: [7405] Adding suppport WMA pass through
* 
* Hydra_Software_Devel/48   7/18/08 2:57p tthomas
* PR41726: [7405] Adding suppport for PCM Router Stage
* 
* Hydra_Software_Devel/47   7/10/08 2:09p tthomas
* PR41726: [7405] Adding suppport for Dolby Prologic II and correcting
* the default values for SRS Xt and AVL user cfg
* 
* Hydra_Software_Devel/46   7/9/08 9:44a tthomas
* PR41726: [7405] Adding suppport for SRS Xt and AVL
* 
* Hydra_Software_Devel/45   6/19/08 11:17p tthomas
* PR41726: [7405] Doing modification for Fade module
* 
* Hydra_Software_Devel/44   6/19/08 3:50p tthomas
* PR41726: [7405] Adding support for PES Based PCM Passthrough
* 
* Hydra_Software_Devel/43   6/12/08 3:58p tthomas
* PR41726: [7405] Adding Buffer allocation  and user cfg Modification for
* AD
* 
* Hydra_Software_Devel/42   6/4/08 3:55p tthomas
* PR41726: [7405] Adding Extra samples for IO buffer
* 
* Hydra_Software_Devel/41   5/29/08 9:09a tthomas
* PR41726: [7405] Correcting the comments
* 
* Hydra_Software_Devel/40   5/28/08 8:43p tthomas
* PR41726: [7405] Adding support for AAC/HE ADTS/LOAS independently
* 
* Hydra_Software_Devel/39   5/27/08 5:09p tthomas
* PR41726: [7405] Adding support for SRC and Audio Descriptor
* 
* Hydra_Software_Devel/38   5/26/08 2:23p tthomas
* PR41726: [7405] Adding support for WMA PRO Node Config
* 
* Hydra_Software_Devel/37   5/6/08 8:23p gautamk
* PR41726: [7405] Changes for AAC
* 
* Hydra_Software_Devel/36   5/2/08 5:50p tthomas
* PR34648:[7405] Adding Support for the DTS Broadcast Encoder.
* 
* Hydra_Software_Devel/35   4/25/08 7:03p dinesha
* PR34648:[7405] Fixed compilation error and removed warnings.
* 
* Hydra_Software_Devel/33   4/25/08 2:13p dinesha
* PR34648:[7405] Adding support for WMAPro.
* 
* Hydra_Software_Devel/31   4/23/08 6:56p tthomas
* PR34648:[7405] Adding support for Custom voice support
* 
* Hydra_Software_Devel/30   4/21/08 4:20p tthomas
* PR34648:[7405] Adding support for Custom voice support
* 
* Hydra_Software_Devel/29   4/21/08 4:08p tthomas
* PR34648:[7405] Adding support for Status buffer on Passthrough
* 
* Hydra_Software_Devel/28   4/18/08 4:22p tthomas
* PR34648:[7405] Adding support for Status buffer on Passthrough
* 
* Hydra_Software_Devel/27   4/16/08 4:45p tthomas
* PR34648:[7405] Adding ASTMTSM Support
* 
* Hydra_Software_Devel/26   4/14/08 1:33p gautamk
* PR34648: [7405] Changiong the streaminfo parameter name for DDp.
* Also fixing warning in brap_af_priv.c
* 
* Hydra_Software_Devel/25   4/11/08 5:47p tthomas
* PR 34648: [7405] Adding WMA support for the sDecAlgoExecInfo structure
* 
* Hydra_Software_Devel/24   4/8/08 3:58p sushmit
* PR 34648: [7405] Fixing DDP FrameSync Id in NodeInfo
* 
* Hydra_Software_Devel/23   4/2/08 2:06p sushmit
* PR 34648: [7405] DD Convert Code Download Fix
* 
* Hydra_Software_Devel/22   4/1/08 3:23p tthomas
* PR 34648: [7405] Adding support for the DD Convert
* 
* Hydra_Software_Devel/21   3/27/08 3:36p tthomas
* PR 34648: [7405] Adding the WMA Support
* 
* Hydra_Software_Devel/20   3/18/08 6:22p tthomas
* PR 34648: [7405] Increasing the size of FW Status buffer
* 
* Hydra_Software_Devel/19   3/13/08 2:44p sushmit
* PR 34648: [7405] Checking in AACHE Support Changes
* 
* Hydra_Software_Devel/18   3/3/08 6:38p gautamk
* PR34648: [7405] Implementing review comments.
* 
* Hydra_Software_Devel/17   2/29/08 4:23p gautamk
* PR34648: [7405] Changing file names includes
* 
* Hydra_Software_Devel/16   2/19/08 4:17p tthomas
* PR34648: [7405]: Removing the warnings
* 
* Hydra_Software_Devel/15   2/15/08 8:44p gautamk
* PR34648: [7405]Adding User Config for Passthru
* Adding spdif CBit buffer.
* 
* Hydra_Software_Devel/14   2/15/08 2:30p tthomas
* PR34648: [7405]: Adding the TSM Buffer Requirement
* 
* 9   2/14/08 2:53p tthomas
* PR 31859: Adding 1) Zero Fill Configuration 2) Port Configuration 3)
* Gateopen feature to CIT Gen module
* 
* NEXTGEN_ADSP_CIT_PORT_CONFIG/2   2/13/08 8:20p tthomas
* PR 31859: Adding Gate open configuration
* 
* 8   2/6/08 12:01p tthomas
* PR 31859:  Adding status buffer size and valid entry
* 
* Hydra_Software_Devel/1   1/16/08 12:02p sushmit
* PR34648: [7405] Adding brap_fw_algo.h & brap_fw_algo_init.c to CC
* 
* 5   12/13/07 5:03a arung
* PR 31859: Checking in the fixes to the sizes found during emulation of
* decode and pass thru module.
* 
* 4   11/13/07 3:08p arung
* PR 31859: Updating the files after adding the AC3 algo type
* 
* 3   11/11/07 5:24p arung
* PR 31859: checking in the first version of the CIT generation module.
* 
* 2   10/30/07 7:30p arung
* PR 31859: Updating the file with separate enums for decode encode and
* post proc algos.
* 
* 1   10/29/07 11:54a arung
* PR 31859: Adding common header files between firmware and PI.
* 
***************************************************************************/

/* HEADER FILE INCLUSION */

#ifdef TESTBENCH_BUILD
	/* Test bench File Inclusion */
	#include "data_types.h"
	#include "brap_buff_defs_7043.h"
#else 

	/* PI header File Inclusion */
	#include "brap_types.h"
	#include "brap_dspchn.h"
#endif

/* CIT-Gen  header File Inclusion */
#include "brap_af_priv.h"
#include "brap_cit_priv.h"

/*-------------------------------------------------------------------------*/
/* Defines Used only in the brap_af_priv.c File*/
/*-------------------------------------------------------------------------*/

#define BRAP_AF_P_EXTRA_SAMPLES	(8)	/* Extra Buffer Spacing given for	
									   IoBuffers 
									*/

/*-------------------------------------------------------------------------*/
/*						Array instantiations							   */
/*-------------------------------------------------------------------------*/

/*	The structure will be instantiated based on the max algo type id */
/*	This will be a two dimensional structure. The second dimesion
	indicates what algos need to be executed for say passthrough of the same
	algorithm.
	There will be three such structures. One for Decoder, Encoder and 
	one for post process */
/*	The structure will be initialized by the firmware and provided to the
	PI for each of the algorithm type */
/*	WARNING!!! Removed static declaration due to compilation error */

/*	Init structure for decoder algorithms */
const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_VideoType_eMax][BRAP_DSPCHN_DecodeMode_eMax] =
{
	/* BRAP_DSPCHN_AudioType_eMpeg */
	{
		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eMpeg][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			3,		/* Number of nodes in Mpeg Decode */
			{
				BRAP_AF_P_AlgoId_eMpegFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,
				
				BRAP_AF_P_AlgoId_eMpegDecode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eMpeg][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in Mpeg pass through */
			{
				BRAP_AF_P_AlgoId_eMpegFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,
				
				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/* BRAP_DSPCHN_AudioType_eAacAdts */
	{
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
		/* MS10 Dolby ADTS Pulse support */
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacAdts][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			6,		
			{
				BRAP_AF_P_AlgoId_eAdtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacAdts][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eAdtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

#else
		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacAdts][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			7,		/* Number of nodes in AAC ADTS Decode */
			{
				BRAP_AF_P_AlgoId_eAdtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacAdts][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in AAC ADTS  pass thru */
			{
				BRAP_AF_P_AlgoId_eAdtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},

	/* BRAP_DSPCHN_AudioType_eAacLoas */
	{

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT

		/* MS10 Dolby LOAS Pulse support */
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacLoas][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			6,		
			{
				BRAP_AF_P_AlgoId_eLoasFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacLoas][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eLoasFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

#else
		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacLoas][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			7,		/* Number of nodes in AAC LOAS Decode */
			{
				BRAP_AF_P_AlgoId_eLoasFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacLoas][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in AAC LOAS pass thru */
			{
				BRAP_AF_P_AlgoId_eLoasFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
#endif

	},	
	
	/* BRAP_DSPCHN_AudioType_eAacSbrAdts */
	{

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
		
		/* MS10 Dolby ADTS Pulse support */

		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacSbrAdts][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			6,		
			{
				BRAP_AF_P_AlgoId_eAdtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacSbrAdts][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eAdtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

#else
		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacSbrAdts][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			7,		/* Number of nodes in AAC-HE ADTS Decode */
			{
				BRAP_AF_P_AlgoId_eAdtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacSbrAdts][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in AAC-HE ADTS pass thru */
			{
				BRAP_AF_P_AlgoId_eAdtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},
		
	/* BRAP_DSPCHN_AudioType_eAacSbrLoas */
	{
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT

		/* MS10 Dolby LOAS Pulse support */

		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacLoasSbr][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			6,		
			{
				BRAP_AF_P_AlgoId_eLoasFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2,
				BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacLoasSbr][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eLoasFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

#else
		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacSbrLoas][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			7,		/* Number of nodes in AAC-HE LOAS Decode */
			{
				BRAP_AF_P_AlgoId_eLoasFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3,
				BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAacLoasSbr][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in AAC-HE  LOAS pass thru */
			{
				BRAP_AF_P_AlgoId_eLoasFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},
	/* BRAP_DSPCHN_AudioType_eAc3 */
	{
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
		
		/*MS10 DD Decoder */
					
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAc3][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			4,		
			{
				BRAP_AF_P_AlgoId_eMs10DdpFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1,
				BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAc3][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eMs10DdpFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
#else					
		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAc3][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			4,		/* Number of nodes in AC3 Decode */
			{
				BRAP_AF_P_AlgoId_eAc3FrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eAc3DecodeStage1,
				BRAP_AF_P_AlgoId_eAc3DecodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAc3][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in AC3 Pass thru */
			{
				BRAP_AF_P_AlgoId_eAc3FrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},

	/* BRAP_DSPCHN_AudioType_eAc3Plus */
	{

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT

		/*MS10 DDP  Decoder */				
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAc3Plus][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			4,		
			{
				BRAP_AF_P_AlgoId_eMs10DdpFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1,
				BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAc3Plus][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eMs10DdpFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
#else	
		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAc3Plus][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			4,		/* Number of nodes in AC3 Plus Decode */
			{
				BRAP_AF_P_AlgoId_eDdpFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDdpDecodeStage1,
				BRAP_AF_P_AlgoId_eDdpDecodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAc3Plus][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in AC3 Plus Pass thru */
			{
				BRAP_AF_P_AlgoId_eDdpFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},
	/* BRAP_DSPCHN_AudioType_eDts */
	{
		{/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eDts][BRAP_DSPCHN_DecodeMode_eDecode] = */
			3,		
			{
				BRAP_AF_P_AlgoId_eDtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDtsDecodeStage1,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eDts][BRAP_DSPCHN_DecodeMode_ePassThru] = */
			3,		
			{
				BRAP_AF_P_AlgoId_eDtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/* BRAP_DSPCHN_AudioType_eLpcmBd */
	{
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/* BRAP_DSPCHN_AudioType_eLpcmHdDvd */
	{
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/*BRAP_DSPCHN_AudioType_eDtshd*/
	{
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eDtshd][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			4,		
			{
				BRAP_AF_P_AlgoId_eDtsHdFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDtsHdDecodeStage0,
				BRAP_AF_P_AlgoId_eDtsHdDecodeStage1,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eDtshd][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eDtsHdFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/* BRAP_DSPCHN_AudioType_eLpcmDvd */
	{
		/*BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eLpcmDvd][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eDvdLpcmFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDvdLpcmDecode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/*BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eLpcmDvd][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/* BRAP_DSPCHN_AudioType_eWmaStd */
	{

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eWmaStd][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			3,		/* Number of nodes in WMA-STD Decode */
			{
				BRAP_AF_P_AlgoId_eWmaStdFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eWmaStdDecode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eWmaStd][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in WMA-STD Pass thru */
			{
				BRAP_AF_P_AlgoId_eWmaStdFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},		
	},
	
	/* BRAP_DSPCHN_AudioType_eAc3Lossless */
	{		
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/*BRAP_DSPCHN_AudioType_eMlp*/
	{		
		{	
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/* BRAP_DSPCHN_AudioType_ePcm*/
	{		
		{
			/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_ePcm][BRAP_DSPCHN_DecodeMode_eDecode] = */
			3,		
			{
				BRAP_AF_P_AlgoId_ePesFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,
				BRAP_AF_P_AlgoId_ePassThru,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_ePcm][BRAP_DSPCHN_DecodeMode_ePassThru] = */
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	{
		/*BRAP_DSPCHN_AudioType_eDtsLbr*/
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	{
		/* BRAP_DSPCHN_AudioType_eDdp7_1*/
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	{
		/*BRAP_DSPCHN_AudioType_eMpegMc*/
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},


	/* BRAP_DSPCHN_AudioType_eWmaPro */
	{

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eWmaPro][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			4,		/* Number of nodes in WMA-pro Decode */
			{
				BRAP_AF_P_AlgoId_eWmaProFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eWmaProStdDecodeStage1,
				BRAP_AF_P_AlgoId_eWmaProStdDecodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eWmaPro][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		/* Number of nodes in WMA-pro Pass thru */
			{
				BRAP_AF_P_AlgoId_eWmaProFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},		
	},

	{
		/*BRAP_DSPCHN_AudioType_eDtshdSub*/
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	{
		/*BRAP_DSPCHN_AudioType_eLpcmDvdA*/
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eLpcmDvdA][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eLpcmDvdA][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},
	{
		/*BRAP_DSPCHN_AudioType_eDtsBroadcast*/
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eDtsBroadcast][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			4,		
			{
				BRAP_AF_P_AlgoId_eDtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDtsHdDecodeStage0,
				BRAP_AF_P_AlgoId_eDtsHdDecodeStage1,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eDtsBroadcast][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eDtsFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},
	{
		/*BRAP_DSPCHN_AudioType_ePcmWav*/
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_ePcmWav][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_ePcmWavFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePcmWavDecode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_ePcmWav][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BRAP_DSPCHN_AudioType_eAmr*/
	{		
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAmr][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eAmrFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eAmrDecode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eAmr][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eAmrFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/*BRAP_DSPCHN_AudioType_eDra*/
	{		
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eDra][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eDraFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eDraDecode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eDra][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eDraFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},
	/*BRAP_DSPCHN_AudioType_eRealAudioLbr*/
	{		
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eRealAudioLbr][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eRealAudioLbrFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_eRealAudioLbrDecode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eRealAudioLbr][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			3,		
			{
				BRAP_AF_P_AlgoId_eRealAudioLbrFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,

				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},

	/*BRAP_DSPCHN_AudioType_eRealVideo9*/
	{		
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eRealVideo9][BRAP_DSPCHN_DecodeMode_eDecode] = */
		{
			5,		
			{
				BRAP_AF_P_AlgoId_eRealVideo9PLF,
				BRAP_AF_P_AlgoId_eRealVideo9Stage1,
				BRAP_AF_P_AlgoId_eRealVideo9Stage2,
				BRAP_AF_P_AlgoId_eRealVideo9Stage3,
				BRAP_AF_P_AlgoId_eRealVideo9PPD,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/* BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_AudioType_eRealVideo9][BRAP_DSPCHN_DecodeMode_ePassThru] = */
		{
			0,		
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,

				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		}
	},
	
};

/*	Structure for Encoder algorithms */

const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eMax][BRAP_DSPCHN_EncodeMode_eSimulMode] =
{
	/*BRAP_CIT_P_EncAudioType_eMpeg1Layer3*/
	{
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eMpeg1Layer3][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			3,	/* Number of nodes in MPEG1 L3 Encode */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eMpegL3EncodeStage1,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eMpeg1Layer3][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			3,	/* Number of nodes in MPEG1 Layer 3 Encode NRT */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eMpegL3EncodeStage1,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_EncAudioType_eMpeg1Layer2*/
	{
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eMpeg1Layer2][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			4,	/* Number of nodes in MPEG1 Layer 2 Encode */
			{
				BRAP_AF_P_AlgoId_eMpegL2EncFrameSync,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eMpegL2EncodeStage1,
				BRAP_AF_P_AlgoId_eMpegL2EncodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eMpeg1Layer2][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			4,	/* Number of nodes in MPEG1 Layer 2 Encode NRT */
			{
				BRAP_AF_P_AlgoId_eMpegL2EncFrameSync,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eMpegL2EncodeStage1,
				BRAP_AF_P_AlgoId_eMpegL2EncodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_EncAudioType_eDTS*/
	{
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eDTS][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			3,	/* Number of nodes in DTS Encode */
			{
				BRAP_AF_P_AlgoId_eDtsEncFrameSync,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eDtsEncode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eDTS][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			3,	/* Number of nodes in DTS Encode */
			{
				BRAP_AF_P_AlgoId_eDtsEncFrameSync,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eDtsEncode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_EncAudioType_eAacLc*/
	{
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eAacLc][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			4,	/* Number of nodes in AAC-LC Encode */
			{
				BRAP_AF_P_AlgoId_eAacLcEncFrameSync,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eAacLcEncodeStage1,
				BRAP_AF_P_AlgoId_eAacLcEncodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eAacLc][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			4,	/* Number of nodes in AAC-LC Encode */
			{
				BRAP_AF_P_AlgoId_eAacLcEncFrameSync,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eAacLcEncodeStage1,
				BRAP_AF_P_AlgoId_eAacLcEncodeStage2,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_EncAudioType_eAacHe*/
	{
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eAacHe][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			5,	/* Number of nodes in AAC-HE Encode */
			{
				BRAP_AF_P_AlgoId_eAacHeEncFrameSync,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eAacHeEncodeStage1,
				BRAP_AF_P_AlgoId_eAacHeEncodeStage2,
				BRAP_AF_P_AlgoId_eAacHeEncodeStage3,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eAacHe][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			5,	/* Number of nodes in AAC-HE Encode NRT*/
			{
				BRAP_AF_P_AlgoId_eAacHeEncFrameSync,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eAacHeEncodeStage1,
				BRAP_AF_P_AlgoId_eAacHeEncodeStage2,
				BRAP_AF_P_AlgoId_eAacHeEncodeStage3,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_EncAudioType_eAc3*/
	{

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT

		/*MS10 DD Encoder */
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eAc3][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			3,	/* Number of nodes in DD Transcode */
			{
                BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eMs10DDTranscode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eAc3][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			3,	/* Number of nodes in DDTranscode NRT*/
			{
                BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eMs10DDTranscode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
#else

		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eAc3][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			3,	/* Number of nodes in AC3 Encode */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eAc3Encode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},

		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eAc3][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			3,	/* Number of nodes in AC3 Encode NRT*/
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eAc3Encode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
#endif
	},

	/*BRAP_CIT_P_EncAudioType_eDTSBroadcast*/
	{
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eDTSBroadcast][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			3,	/* Number of nodes in DTSBroadcast Encode */
			{
                BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eDtsBroadcastEncode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid 
			},
		},

		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eDTSBroadcast][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			3,	/* Number of nodes in DTSBroadcast Encode NRT*/
			{
                BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eDtsBroadcastEncode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_EncAudioType_eSbc*/
	{
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eSbc][BRAP_DSPCHN_EncodeMode_eRealtime] = */
		{
			3,	/* Number of nodes in SBC Encode */
			{
                BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eSbcEncode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
		/*	BRAP_sEncAlgoExecInfo[BRAP_CIT_P_EncAudioType_eSbc][BRAP_DSPCHN_EncodeMode_eNonRealtime] = */
		{
			3,	/* Number of nodes in SBC Encode NRT*/
			{
                BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eEncodeTsm,
				
				BRAP_AF_P_AlgoId_eSbcEncode,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},	
};


/*	Structure for Encoder algorithms */
const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eMax][BRAP_DSPCHN_ProcessingMode_eMax] =
{
	/*BRAP_CIT_P_ProcessingType_eDdbm*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDdbm][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in DDBM Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eDdbmPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eDtsNeo*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDtsNeo][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in DTS Neo Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eDtsNeoPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eAVL*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eAVL][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in AVL Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eAvlPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT

	/*BRAP_CIT_P_ProcessingType_eDDConvert,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDDConvert][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes MS DDConvert Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eMs10DDConvert,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
#else
	
	/*BRAP_CIT_P_ProcessingType_eDDConvert*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDDConvert][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in DDConvert Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eDDConvert,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
#endif

	/*BRAP_CIT_P_ProcessingType_ePLll,*/    /* Dolby Prologic-II. */
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_ePLll][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in Dolby Prologic-II*/
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_ePl2PostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_ProcessingType_eSrsXt,	*/    /* TruSurroundXT. */
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eSrsXt][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in Srs Xt */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eSrsTruSurroundPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BRAP_CIT_P_ProcessingType_eXen,		*/	/* XEN 3D Sound Processing. */
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eXen][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			0,	/* Number of nodes in XEN 3D Sound Processing Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BRAP_CIT_P_ProcessingType_eBbe,	  */       /* BBE */
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eBbe][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			0,	/* Number of nodes in BBE Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BRAP_CIT_P_ProcessingType_eSrc,	 */	    /* Sample Rate Change */
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eSrc][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in SRC Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eSrcPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BRAP_CIT_P_ProcessingType_eCustomSurround, */	/* CUSTOM Surround Algorithm */
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eCustomSurround][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in CUSTOM Surround Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eCustomSurroundPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*	BRAP_CIT_P_ProcessingType_eCustomBass,   */  /* CUSTOM Bass Algorithm */	
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eCustomBass][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in CUSTOM Bass Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eCustomBassPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eCustomVoice, */  /* CUSTOM Voice Algorithm*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eCustomVoice][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in CUSTOM Voice Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eCustomVoicePostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_ePeq, */          /* PEQ Algorithm */	
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_ePeq][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			0,	/* Number of nodes in PEQ Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eDownmix0,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDownmix0][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			0,	/* Number of nodes in Downmix Post Proc */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_ProcessingType_eAudioDescriptorFade,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eAudioDescriptorFade][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in AudioDescriptor Fade */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eAudioDescriptorFadePostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_ProcessingType_eAudioDescriptorPan,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eAudioDescriptorPan][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in AudioDescriptor Pan */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eAudioDescriptorPanPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_ProcessingType_ePCMRouter,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_ePCMRouter][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in PCMRouter */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_ePCMRouterPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_ProcessingType_eWMAPassThrough,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eWMAPassThrough][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in WMAPassThrough */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eWMAPassThrough,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},	

	/*BRAP_CIT_P_ProcessingType_eDsola,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDsola][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in DSOLA  */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eDsolaPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eSrsHd,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eSrsHd][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in SrsHd  */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eSrsTruSurroundHDPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eGenericPassThru,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eGenericPassThru][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in Generic Pass through  */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_ePassThru,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eSrsTruVolume,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eSrsTruVolume][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in Srs VolumeIq  */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eSrsTruVolumePostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eDolbyVolume,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDolbyVolume][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in Dolby Volume */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eDolbyVolumePostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eAudysseyVolume,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eAudysseyVolume][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in AudysseyV olume */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eAudysseyVolumePostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eBrcm3DSurround,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eBrcm3DSurround][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in AudysseyV olume */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eBrcm3DSurroundPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eFWMixerPostProc,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eFWMixerPostProc][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in FWMixer */
			{
				BRAP_AF_P_AlgoId_eMixerTaskFrameSync,
				BRAP_AF_P_AlgoId_eDecodeTsm,
				
				BRAP_AF_P_AlgoId_eFWMixerPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},

	/*BRAP_CIT_P_ProcessingType_eMonoDownmix,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eMonoDownmix][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in Mono Downmix */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eMonoDownMixPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eAudysseyABX,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eAudysseyABX][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in Mono Downmix */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eAudysseyABXPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eDdre,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDdre][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in DDRE */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eDdrePostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eDv258,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eDv258][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in DV258 */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eDv258PostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eSrsCsdTd,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eSrsCsdTd][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in CSD and TD */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eSrsCircleSurrPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eSrsGeq,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eSrsGeq][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in SRS GEQ */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eSrsEqualizerPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
	/*BRAP_CIT_P_ProcessingType_eBtsc,*/
	{
		/*	BRAP_sPpAlgoExecInfo[BRAP_CIT_P_ProcessingType_eBtsc][BRAP_DSPCHN_ProcessingMode_ePP] = */
		{
			3,	/* Number of nodes in BTSC encoder PP */
			{
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				
				BRAP_AF_P_AlgoId_eBtscEncPostProc,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid,
				BRAP_AF_P_AlgoId_eInvalid
			},
		},
	},
};


/*	Structure for Gfx Decode */

const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sGfxDecodeExecInfo[BRAP_DSPCHN_GfxType_eMax] =
{	
	/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_GfxType_eSoftGfx] = */
	{
		1,		/* Number of nodes in Mpeg Decode */
		{
			BRAP_AF_P_AlgoId_eGfxDecode,
			BRAP_AF_P_AlgoId_eInvalid,
			
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid
		},
	}
};

/*	Structure for Secure Code Module */

const BRAP_AF_P_sALGO_EXEC_INFO BRAP_sScmDecodeExecInfo[BRAP_DSPCHN_ScmType_eMax] =
{	
	/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_ScmType_eScm_1] = */
	{
		1,		/* Number of nodes in SCM */
		{
			BRAP_AF_P_AlgoId_eScm_1,
			BRAP_AF_P_AlgoId_eInvalid,
			
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid
		},
	},

	/*	BRAP_sDecAlgoExecInfo[BRAP_DSPCHN_ScmType_eScm_2] = */
	{
		1,		/* Number of nodes in SCM */
		{
			BRAP_AF_P_AlgoId_eScm_2,
			BRAP_AF_P_AlgoId_eInvalid,
			
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid,
			BRAP_AF_P_AlgoId_eInvalid
		},
	}
	
	
};



/* ------------------------------------------------------------------------------ */
/*                       NODE BUFFER DETAILS									  */
/* ------------------------------------------------------------------------------ */

const BRAP_AF_P_sNODE_INFO BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMax] =
{
	/*	this entry will contain all the sizes (in bytes) of the Mpeg Decode algorihtm */
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegDecode] =  */
	{
		28436,											/*	ui32CodeSize */
		14336,											/*	ui32RomTableSize */
		10272,											/*	ui32InterFrameBuffSize */
		(1152+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		64,												/*	ui32UserCfgBuffSize */
		(1152+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eStatusBuffType */
		64												/*  StatusBuffSize */
		
		/*BRAP_AF_P_NodeStatusBuffType_ePresent */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAc3DecodeStage1] =  */
	{
		30000,											/*	ui32CodeSize */
		16000,											/*	ui32RomTableSize */
		6000,											/*	ui32InterFrameBuffSize */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		40000,											/*	ui32InterStageGenericBuffSize */
		15360,											/*	ui32ScratchBuffSize */
		600,											/*	ui32UserCfgBuffSize	 */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		200												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAc3DecodeStage2] =  */
	{
		25000,											/*	ui32CodeSize */
		8000,											/*	ui32RomTableSize */
		65000,											/*	ui32InterFrameBuffSize */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		40000,											/*	ui32InterStageGenericBuffSize */
		15360,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize		 */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacDecodeStage1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacDecodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage0] =  */
	{ 
		33792,											/*	ui32CodeSize */
		35720,											/*	ui32RomTableSize */	
		15000,											/*	ui32InterFrameBuffSize */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		41984,											/*	ui32InterStageGenericBuffSize */
		81920,											/*	ui32ScratchBuffSize */
		1024,											/*	ui32UserCfgBuffSize		 */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		400,											/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage1] =  */
	{ 
		40960,											/*	ui32CodeSize */
		15288,											/*	ui32RomTableSize */	
		75700,											/*	ui32InterFrameBuffSize */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		41984,											/*	ui32InterStageGenericBuffSize */
		81920,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize		 */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0,												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage2] =  */
	{ 
		33792,											/*	ui32CodeSize */
		12500,											/*	ui32RomTableSize */	
		76700,											/*	ui32InterFrameBuffSize */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		41984,											/*	ui32InterStageGenericBuffSize */
		81920,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize		 */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0,												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage3] =  */
	{ 
		15240,											/*	ui32CodeSize */
		12500,											/*	ui32RomTableSize */	
		19400,											/*	ui32InterFrameBuffSize */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		41984,											/*	ui32InterStageGenericBuffSize */
		81920,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize		 */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0,												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeLpSbrDecodeStage4] =  */
	{ 
		7000,											/*	ui32CodeSize */
		2048,											/*	ui32RomTableSize */	
		10240,											/*	ui32InterFrameBuffSize */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		41984,											/*	ui32InterStageGenericBuffSize */
		81920,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize		 */
		(12300+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0,												/*  FwStatusBuffSize */
	},


	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdpDecodeStage1] =  */
	{
		30000,											/*	ui32CodeSize */
		16000,											/*	ui32RomTableSize */
		6000,											/*	ui32InterFrameBuffSize */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		40000,											/*	ui32InterStageGenericBuffSize */
		15360,											/*	ui32ScratchBuffSize */
		600,											/*	ui32UserCfgBuffSize	 */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		200												/*  FwStatusBuffSize */
	
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdpDecodeStage2] =  */
	{
		25000,											/*	ui32CodeSize */
		8000,											/*	ui32RomTableSize */
		65000,											/*	ui32InterFrameBuffSize */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		40000,											/*	ui32InterStageGenericBuffSize */
		15360,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize		 */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */		
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdLosslessDecodeStage1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdLosslessDecodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eLpcmCustomDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eBdLpcmDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDvdLpcmDecode] =  */
	{	
		23000,											/*	ui32CodeSize */
		4000,											/*	ui32RomTableSize */	
		2000,											/*	ui32InterFrameBuffSize */
		(5000+BRAP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		4300,											/*	ui32UserCfgBuffSize		 */
		(5000+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		200,											/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eHdDvdLpcmDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegMcDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eWmaStdDecode] =  */
	{	
		36000,											/*	ui32CodeSize */
		34000,											/*	ui32RomTableSize */	
		34000,											/*	ui32InterFrameBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		7000,											/*	ui32ScratchBuffSize */
		512,											/*	ui32UserCfgBuffSize		 */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		400,											/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage1] =  */
	 {
        27000,                                          /*  ui32CodeSize */
        12500,                                          /*  ui32RomTableSize */
        200,                                            /*  ui32InterFrameBuffSize */
        (4500+BRAP_AF_P_EXTRA_SAMPLES)*4*6,             /*  ui32InterStageIoBuffSize */
        60000,                                          /*  ui32InterStageGenericBuffSize */
        76000,                                          /*  ui32ScratchBuffSize */
        496,                                            /*  ui32UserCfgBuffSize */
        (4500+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*  ui32MaxSizePerChan */
        6,                                              /*  ui32MaxNumChansSupported */
        BRAP_AF_P_InterFrameBuffType_ePresent,			/*  eInterFrameBuffType */
        BRAP_AF_P_FwStatus_ePresent,                    /*  eFwStatusBuffType */
        400                                             /*  FwStatusBuffSize */
     },


	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eWmaProStdDecodeStage2] =  */
	{
        28000,                                          /*  ui32CodeSize */
        20000,                                          /*  ui32RomTableSize */
        160000,                                         /*  ui32InterFrameBuffSize */
        (4500+BRAP_AF_P_EXTRA_SAMPLES)*4*6,             /*  ui32InterStageIoBuffSize */
        13000,											/*  ui32InterStageGenericBuffSize */
        0,												/*  ui32ScratchBuffSize */
        0,												/*  ui32UserCfgBuffSize */
        (4500+BRAP_AF_P_EXTRA_SAMPLES)*4,               /*  ui32MaxSizePerChan */
        6,                                              /*  ui32MaxNumChansSupported */
        BRAP_AF_P_InterFrameBuffType_ePresent,			/*  eInterFrameBuffType */
        BRAP_AF_P_FwStatus_eShared,                     /*  eFwStatusBuffType */
        0												/*  FwStatusBuffSize */
     },	

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMlpDecode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdp71DecodeStage1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdp71DecodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsDecodeStage1] =  */
	{   7000,	                                        /*  ui32CodeSize */
        100,                                            /*  ui32RomTableSize */
        100,											/*  ui32InterFrameBuffSize */
        (4500+BRAP_AF_P_EXTRA_SAMPLES)*4*6,             /*  ui32InterStageIoBuffSize */
        2048,											/*  ui32InterStageGenericBuffSize */
        100,											/*  ui32ScratchBuffSize */
        100,											/*  ui32UserCfgBuffSize */
        (4500+BRAP_AF_P_EXTRA_SAMPLES)*4,               /*  ui32MaxSizePerChan */
        6,                                              /*  ui32MaxNumChansSupported */
        BRAP_AF_P_InterFrameBuffType_ePresent,			/*  eInterFrameBuffType */
        BRAP_AF_P_FwStatus_ePresent,                    /*  eFwStatusBuffType */
        200	 
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsDecodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsDecodeStage3] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsLbrDecodeStage1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsLbrDecodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsLbrDecodeStage3] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsHdDecodeStage0] =  */
	{
		37888,											/*	ui32CodeSize */
		81920,											/*	ui32RomTableSize */
		7168,											/*	ui32InterFrameBuffSize */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		54000,											/*	ui32InterStageGenericBuffSize */
		51200,											/*	ui32ScratchBuffSize */
		1100,											/*	ui32UserCfgBuffSize */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		200												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsHdDecodeStage1] =  */
	{
		37888,											/*	ui32CodeSize */
		35840,											/*	ui32RomTableSize */
		60000,											/*	ui32InterFrameBuffSize */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		54000,											/*	ui32InterStageGenericBuffSize */
		51200,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsHdDecodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsHdDecodeStage3] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsHdDecodeStage4] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePcmWavDecode] =  */
	{
		7600,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		100,											/*	ui32InterFrameBuffSize */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		1024,											/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		100												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAmrDecode] =  */
	{
		30720,											/*	ui32CodeSize */
		30720,											/*	ui32RomTableSize */
		2048,											/*	ui32InterFrameBuffSize */
		(160+BRAP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		256,											/*	ui32UserCfgBuffSize */
		(160+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		100												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDraDecode] =  */
	{
		30720,											/*	ui32CodeSize */
		38912,											/*	ui32RomTableSize */
		39936,											/*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		38912,											/*	ui32ScratchBuffSize */
		128,											/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		100												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eRealAudioLbrDecode] =  */
	{
		30720,											/*	ui32CodeSize */
		30720,											/*	ui32RomTableSize */
		18432,											/*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		4096,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		200,											/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		64												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage0] =  */
	{
		46080,											/*	ui32CodeSize */
		16000,											/*	ui32RomTableSize */
		14336,											/*	ui32InterFrameBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		12400,											/*	ui32InterStageGenericBuffSize */
		25500,											/*	ui32ScratchBuffSize */
		512,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		100												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage1] =  */
	{
		46080,											/*	ui32CodeSize */
		7680,											/*	ui32RomTableSize */
		48600,											/*	ui32InterFrameBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		12400,											/*	ui32InterStageGenericBuffSize */
		200,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage2] =  */
	{
		46080,											/*	ui32CodeSize */
		10240,											/*	ui32RomTableSize */
		20992,											/*	ui32InterFrameBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		12400,											/*	ui32InterStageGenericBuffSize */
		200,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDolbyPulseDecodeStage3] =  */
	{
		46080,											/*	ui32CodeSize */
		6000,											/*	ui32RomTableSize */
		76800,											/*	ui32InterFrameBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		12400,											/*	ui32InterStageGenericBuffSize */
		200,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared	,					/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage1] =  */
	{
		30000,											/*	ui32CodeSize */
		16000,											/*	ui32RomTableSize */
		6000,											/*	ui32InterFrameBuffSize */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		40000,											/*	ui32InterStageGenericBuffSize */
		15360,											/*	ui32ScratchBuffSize */
		600,											/*	ui32UserCfgBuffSize */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		150												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMs10DdpDecodeStage2] =  */
	{
		25000,											/*	ui32CodeSize */
		8000,											/*	ui32RomTableSize */
		65000,											/*	ui32InterFrameBuffSize */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4*8,				/*	ui32InterStageIoBuffSize */
		40000,											/*	ui32InterStageGenericBuffSize */
		15360,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(5250+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		8,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	Algo inits for the frame syncs of the decoder algorithms */
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegFrameSync] =  */
	{
		29696,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegMcFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAdtsFrameSync] =  */
	{
		25600,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eLoasFrameSync] =  */
	{
		25600,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eWmaStdFrameSync] =  */
	{
		26624,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eWmaProFrameSync] =  */
	{
		26624,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAc3FrameSync] =  */
	{
		28672,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdpFrameSync] =  */
	{ 	
	    28672,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdp71FrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsFrameSync] =  */	
	{ 	
	    25600,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsLbrFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsHdFrameSync] =  */
	{ 	
	    25600,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsHdFrameSync_1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsHdHdDvdFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdLosslessFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMlpFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMlpHdDvdFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePesFrameSync] =  */
	{ 	
	    25000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eBdLpcmFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eHdDvdLpcmFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDvdLpcmFrameSync] =  */
	{ 	
	    26624,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDvdLpcmFrameSync_1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },


	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePcmWavFrameSync] =  */
	{ 
		24576,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAmrFrameSync] =  */
	{ 
		25000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDraFrameSync] =  */
	{ 
		25000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eRealAudioLbrFrameSync] =  */
	{ 
		25000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500 ,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300 ,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMs10DdpFrameSync] =  */
	{ 
		28500,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		6500,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},	

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMixerTaskFrameSync] =  */
	{ 
		24500,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		480,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		7300,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},	

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDecodeTsm] =  */
	{
		6600,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		100,											/*	ui32InterFrameBuffSize */
		1152*4*2,										/*	ui32InterStageIoBuffSize */
		400,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		72,												/*	ui32UserCfgBuffSize */
		1152*4,											/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		400												/*  FwStatusBuffSize */
		
	},
	
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eEncodeTsm] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePassthroughTsm] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSystemDecodeTask] =  */
	{ 	10000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSystemGraphicTask] =  */
	{ 	10000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSystemVideoTask] =  */
	{ 	10000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSCMTask] =  */
	{	10000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent, 					/*	eFwStatusBuffType */
		0												/*	FwStatusBuffSize */
	},
	
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAc3Encode] =  */
	{ 	44000,											/*	ui32CodeSize */
		12000,											/*	ui32RomTableSize */
		56000,											/*	ui32InterFrameBuffSize */
		(12000 +BRAP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		85000,											/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		12000*4,										/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegL2EncodeStage1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegL2EncodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegL3EncodeStage1] =  */
	{ 	
		37000,											/*	ui32CodeSize */
		28000,											/*	ui32RomTableSize */
		25500,											/*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		1024*4,											/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegL3EncodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacLcEncodeStage1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacLcEncodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeEncodeStage1] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeEncodeStage2] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeEncodeStage3] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsEncode] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsBroadcastEncode] =  */
	{	
		21504,											/* ui32CodeSize */
		41984,											/* ui32RomTableSize */
		26624,											/* ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*1,				/* ui32InterStageIoBuffSize */
		2048,											/* ui32InterStageGenericBuffSize */
		0,												/* ui32ScratchBuffSize */
		200,											/* ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/* eFwStatusBuffType */
		400												/* FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSbcEncode] =  */
	{	
		10240,											/* ui32CodeSize */
		1024,											/* ui32RomTableSize */
		2048,											/* ui32InterFrameBuffSize */
		(37400+BRAP_AF_P_EXTRA_SAMPLES)*1,				/* ui32InterStageIoBuffSize */
		2048,											/* ui32InterStageGenericBuffSize */
		256,											/* ui32ScratchBuffSize */
		40,												/* ui32UserCfgBuffSize */
		(37400+BRAP_AF_P_EXTRA_SAMPLES),				/* ui32MaxSizePerChan */
		1,												/* ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/* eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/* eFwStatusBuffType */
		0												/* FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMs10DDTranscode] =  */
	{
		32000,											/*	ui32CodeSize */
		10000,											/*	ui32RomTableSize */
		54000,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		40000,											/*	ui32ScratchBuffSize */
		32,												/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAc3EncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegL3EncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMpegL2EncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacLcEncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAacHeEncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsEncFrameSync] =  */
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePassThru] =  */
	{
		6000,											/*	ui32CodeSize */
		220,											/*	ui32RomTableSize */
		100,											/*	ui32InterFrameBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		32768,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		4,												/*	ui32UserCfgBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		400												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePtAuxDataGenStg] =  */
	{
		10000,											/*	ui32CodeSize */
		1000,											/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		2048,											/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		2048*4,											/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSrsTruSurroundPostProc] =  */
	{
		13312,											/*	ui32CodeSize */
		256,											/*	ui32RomTableSize */
		2000,											/*	ui32InterFrameBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSrcPostProc] =  */
	{
		10000,											/*	ui32CodeSize */
		2000,											/*	ui32RomTableSize */
		5000,											/*	ui32InterFrameBuffSize */
		(8192+BRAP_AF_P_EXTRA_SAMPLES)*4*6,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		20,												/*	ui32UserCfgBuffSize */
		(8192+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdbmPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDownmixPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eCustomSurroundPostProc] =  */
	{
		7168,											/*	ui32CodeSize */
		8000,											/*	ui32RomTableSize */
		1500,											/*	ui32InterFrameBuffSize */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		60,												/*	ui32UserCfgBuffSize	 */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0	 
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eCustomBassPostProc] =  */
	{
		7168,											/*	ui32CodeSize */
		5120 ,											/*	ui32RomTableSize */
		800,											/*	ui32InterFrameBuffSize */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		64,												/*	ui32UserCfgBuffSize	 */
		(2048+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0	 
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eKaraokeCapablePostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eCustomVoicePostProc] =  */ 
	{
		25600,											/*	ui32CodeSize */
		33000,											/*	ui32RomTableSize */
		14000,											/*	ui32InterFrameBuffSize */
		(256*4+BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		1700,											/*	ui32UserCfgBuffSize	 */
		(256*4+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		100												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePeqPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAvlPostProc] =  */
	{
		10240 ,											/*	ui32CodeSize */
		300 ,											/*	ui32RomTableSize */
		512,											/*	ui32InterFrameBuffSize */
		(1152*4+BRAP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		60,												/*	ui32UserCfgBuffSize*/
		(1152*4+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePl2PostProc] =  */
	{
		18000,											/*	ui32CodeSize */
		1536,											/*	ui32RomTableSize */
		3072,											/*	ui32InterFrameBuffSize */
		(256*4+BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		64,												/*	ui32UserCfgBuffSize */
		(256*4+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eXenPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eBbePostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDsolaPostProc] =  */
	{
		10000,											/*	ui32CodeSize */
		1536,											/*	ui32RomTableSize */
		14000,											/*	ui32InterFrameBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		60000,											/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize	*/
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDtsNeoPostProc] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 },
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDDConvert] =  */
	{
		14336,											/*	ui32CodeSize */
		4096,											/*	ui32RomTableSize */
		2048,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		1024,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAudioDescriptorFadePostProc] =  */
	{
		7168,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		48,												/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAudioDescriptorPanPostProc] =  */
	{
		7168,											/*	ui32CodeSize */
		4096,											/*	ui32RomTableSize */
		256,											/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		48,												/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePCMRouterPostProc] =  */
	{
		12000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eWMAPassThrough] =  */
	{
		6000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		0,												/*	ui32InterStageIoBuffSize */
		13000,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize	*/
		0,												/*	ui32MaxSizePerChan */
		0,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eAbsent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] =  */
	{
		13312,											/*	ui32CodeSize */
		3072,											/*	ui32RomTableSize */
		800,											/*	ui32InterFrameBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		4096,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		300,											/*	ui32UserCfgBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSrsTruVolumePostProc] =  */
	{
		20000,											/*	ui32CodeSize */
		8192,											/*	ui32RomTableSize */
		15360,											/*	ui32InterFrameBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4*2,				/*	ui32InterStageIoBuffSize */
		4096,											/*	ui32InterStageGenericBuffSize */
		10,												/*	ui32ScratchBuffSize */
		360,											/*	ui32UserCfgBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDolbyVolumePostProc] =  */
	{
		40000,											/*	ui32CodeSize */
		200000,											/*	ui32RomTableSize */
		20000,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		114700,											/*	ui32ScratchBuffSize */
		200,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAudysseyVolumePostProc] =  */
	{
		40000,											/*	ui32CodeSize */
		5000,											/*	ui32RomTableSize */
		107000,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		120,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		200												/*  FwStatusBuffSize */
	},

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eBrcm3DSurroundPostProc] =  */
	{
		6000,											/*	ui32CodeSize */
		12624,											/*	ui32RomTableSize */
		2400,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		0,												/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		16,												/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eFWMixerPostProc] =  */
	{
		40000,											/*	ui32CodeSize */
		4152,											/*	ui32RomTableSize */
		12400,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		1024,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,												/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMonoDownMixPostProc] =  */
	{
		6000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		128,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		20,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eMs10DDConvert] =  */
	{
		15000,											/*	ui32CodeSize */
		2600,											/*	ui32RomTableSize */
		1000,											/*	ui32InterFrameBuffSize */
		(1536 + BRAP_AF_P_EXTRA_SAMPLES)*4*1,			/*	ui32InterStageIoBuffSize */
		128,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		20,												/*	ui32UserCfgBuffSize */
		(1536+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eAudysseyABXPostProc] = */
	{
		22500,											/*	ui32CodeSize */
		2560,											/*	ui32RomTableSize */
		6144,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		1024,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSrsCircleSurrPostProc] =  */
	{
		22500,											/*	ui32CodeSize */
		1024,											/*	ui32RomTableSize */
		6144,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*7,			/*	ui32InterStageIoBuffSize */
		4096,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		200,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		7,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eSrsEqualizerPostProc] =  */
	{
		22500,											/*	ui32CodeSize */
		10240,											/*	ui32RomTableSize */
		13312,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*2,			/*	ui32InterStageIoBuffSize */
		4096,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		1300,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		2,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDdrePostProc] =  */
	{
		22000,											/*	ui32CodeSize */
		6500,											/*	ui32RomTableSize */
		45000,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		200,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eDv258PostProc] =  */
	{
		30000,											/*	ui32CodeSize */
		2500,											/*	ui32RomTableSize */
		20000,											/*	ui32InterFrameBuffSize */
		(4096 + BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eBtscEncPostProc] =  */
	{
		10000,											/*	ui32CodeSize */
		3000,											/*	ui32RomTableSize */
		1800,											/*	ui32InterFrameBuffSize */
		(4096 + BRAP_AF_P_EXTRA_SAMPLES)*4*1,			/*	ui32InterStageIoBuffSize */
		1024,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		(4096+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eAbsent,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},



	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_ePostProcFrameSync] =  */
	{ 0,0,0,0,0,0,0,0,0,0,0,0 }, 

	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eGfxDecode] =  */
	{
		25000,											/*	ui32CodeSize */
		6200,											/*	ui32RomTableSize */
		48000,											/*	ui32InterFrameBuffSize */
		(6144 + BRAP_AF_P_EXTRA_SAMPLES)*4*6,			/*	ui32InterStageIoBuffSize */
		1024,											/*	ui32InterStageGenericBuffSize */
		100,											/*	ui32ScratchBuffSize */
		164,											/*	ui32UserCfgBuffSize */
		(6144+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		6,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		12												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eRealVideo9PLF] =  */
	{
		20000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		200000,											/*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*1,				/*	ui32InterStageIoBuffSize */
		1536,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		100,											/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent, 					/*	eFwStatusBuffType */
		100												/*  FwStatusBuffSize */

	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eRealVideo9Stage1] =  */
	{
		20000,											/*	ui32CodeSize */
		0,												/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*1,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eShared,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eRealVideo9Stage2] =  */
	{
		42000,											/*	ui32CodeSize */
		200000,											/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*1,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eShared,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eRealVideo9Stage3] =  */
	{
		30000,											/*	ui32CodeSize */
		20000,											/*	ui32RomTableSize */
		0,												/*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*1,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,											    /*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eShared,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eRealVideo9PPD] =  */
	{
		20000,											/*	ui32CodeSize */
		20000,											/*	ui32RomTableSize */
		0,											    /*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*1,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		0,												/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_eShared,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_eShared,						/*	eFwStatusBuffType */
		0												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eScm_1] =  */
	{
		20000,											/*	ui32CodeSize */
		20000,											/*	ui32RomTableSize */
		100,										    /*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*1,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		2000,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		100												/*  FwStatusBuffSize */
	},
	/*	BRAP_sNodeInfo[BRAP_AF_P_AlgoId_eScm_2] =  */
	{
		20000,											/*	ui32CodeSize */
		20000,											/*	ui32RomTableSize */
		100,										    /*	ui32InterFrameBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4*1,				/*	ui32InterStageIoBuffSize */
		2048,											/*	ui32InterStageGenericBuffSize */
		2000,											/*	ui32ScratchBuffSize */
		0,												/*	ui32UserCfgBuffSize */
		(1024+BRAP_AF_P_EXTRA_SAMPLES)*4,				/*	ui32MaxSizePerChan */
		1,												/*	ui32MaxNumChansSupported */
		BRAP_AF_P_InterFrameBuffType_ePresent,			/*	eInterFrameBuffType */
		BRAP_AF_P_FwStatus_ePresent,					/*	eFwStatusBuffType */
		100												/*  FwStatusBuffSize */
	}
};
/*
	This structure is used in the PI to determine the input-output channel mapping for all the algorithms 
	for configuring the FORK matrix configuration.

	Bypass  =  Output number of channel is the same as that of input number channel.
	eFixed  =  Output number of channel is fixed to a particular value.

*/
const BRAP_AF_P_PpOutMode BRAP_sPpOutMode[/*BRAP_CIT_P_ProcessingType_eMax*/] =
{
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Ddbm*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Dts Neo*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*AVL*/
    {BRAP_AF_P_OutModeType_eFixed,	BRAP_AF_P_DistinctOpType_eCompressed},	/*DD Convert*/
    {BRAP_AF_P_OutModeType_eFixed,	BRAP_AF_P_DistinctOpType_e5_1_PCM},		/*Pl2*/
    {BRAP_AF_P_OutModeType_eFixed,	BRAP_AF_P_DistinctOpType_eStereo_PCM},	/*SrsXt*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Xen*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Bbe*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*SRC*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Custom Surround*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Custom Bass*/
    {BRAP_AF_P_OutModeType_eFixed,	BRAP_AF_P_DistinctOpType_eStereo_PCM},  /*Custom Voice*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Peq*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Downmix0*/    
    {BRAP_AF_P_OutModeType_eFixed,	BRAP_AF_P_DistinctOpType_eStereo_PCM},  /*Ad Fade*/
    {BRAP_AF_P_OutModeType_eFixed,	BRAP_AF_P_DistinctOpType_eStereo_PCM},	/*Ad Pan*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*PCM router*/
    {BRAP_AF_P_OutModeType_eFixed,	BRAP_AF_P_DistinctOpType_eCompressed},	/*WMA Passthrough*/
    {BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*DSOLA*/
    {BRAP_AF_P_OutModeType_eFixed,	BRAP_AF_P_DistinctOpType_eStereo_PCM},	/*SRSHD*/
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Generic pass through*/
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*SRS Volume IQ */
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Dolby Volume*/
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Audyssey Volume*/
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*3D Surround Volume*/
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Fw mixer */
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Mono Downmix */
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*Audyssey ABX */
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*DDRE */
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*DV258 */
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*SRS CSD */
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*SRS GEQ */
	{BRAP_AF_P_OutModeType_eBypass,	BRAP_AF_P_DistinctOpType_eInvalid},		/*BTSC */
};

