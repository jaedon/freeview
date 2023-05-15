/***************************************************************************
*     Copyright (c) 2004-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dspchn_priv.h $
* $brcm_Revision: Hydra_Software_Devel/98 $
* $brcm_Date: 11/14/11 4:08p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_dspchn_priv.h $
* 
* Hydra_Software_Devel/98   11/14/11 4:08p srajapur
* SW7420-2074 : [7420] Adding support for MpegConformanceMode
* 
* Hydra_Software_Devel/97   8/25/11 11:15a srajapur
* SW7420-2019: [7420] 7420 - Corrected the downmix path cut/boost values
* settings.
* 
* Hydra_Software_Devel/96   6/2/11 5:51p gautamk
* SW7405-4673: [7405] Changes for userconfig for ms11 dolby pulse.
* 
* Hydra_Software_Devel/95   4/18/11 3:04p sgadara
* SW7420-1756: [7420] Implementing code review comments and correcting
* the indentations
* 
* Hydra_Software_Devel/94   4/6/11 3:20p gautamk
* SW7125-901: [7125] MAI channel Status bit should be picked from
* different structure.
* 
* Hydra_Software_Devel/93   3/18/11 11:06p sgadara
* SW7420-767: [7420] Fixing non-MS build issue
* 
* Hydra_Software_Devel/92   3/18/11 9:40p sgadara
* SW7420-767: [7420] Merge MS11 development branch to mainline
* 
* Hydra_Software_Devel/91   3/7/11 6:03p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/90   10/25/10 6:18p sgadara
* SW3556-1206: [3548,3556] Merge customer branch to main line -->
* Removing the param from AppFormat strcuture as there is no format
* conversion before passing it on to FW
* 
* Hydra_Software_Devel/89   9/16/10 12:18a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/88   9/6/10 5:15p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/87   9/6/10 3:43p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code.
* Merging to mainline
* 
* Hydra_Software_Devel/RAP_7550_TASK_DOWNLOAD_BRANCH/1   8/31/10 3:36p gautamk
* SW7550-547: [7550] Adding support for System task downloadable code
* 
* Hydra_Software_Devel/86   8/13/10 5:22p gautamk
* SW7405-4713:[7405] Merging changes for RDB indepedence in Main line
* 
* Hydra_Software_Devel/85   8/10/10 1:53p gautamk
* SW7420-767: [7420] Merging MS11 feature support to mainline
* 
* Hydra_Software_Devel/RAP_RDB_INDEP_FW/2   8/3/10 8:00p tthomas
* SW7405-3359: [7405] Adding support for RDB indepdence for FW
* 
* Hydra_Software_Devel/RAP_RDB_INDEP_FW/1   8/3/10 1:38p gautamk
* SW7405-3359: [7405] Adding support for RDB indepdence for FW
* 
* Hydra_Software_Devel/83   5/27/10 4:53p gautamk
* SW7405-4170: [7405] Modifying user config programming for Dolby pulse +
* No SRC required for Dolby pulse
* 
* Hydra_Software_Devel/82   4/19/10 7:11p gautamk
* SW7405-4170:[7405] Adding Support for MS10 DDP/AC3
* 
* Hydra_Software_Devel/81   3/10/10 5:05p speter
* SW3548-2819:[3548] Adding Dolby Pulse Algorithm
* 
* Hydra_Software_Devel/80   2/19/10 5:20p gautamk
* SW7468-119: [7468] Adding RAP PI support for Real audio LBR
* 
* Hydra_Software_Devel/79   12/18/09 5:08p gautamk
* SW7405-3591 : [7405] Fixing coverity issue
* 
* Hydra_Software_Devel/78   11/17/09 6:21p sgadara
* SW3556-832 : [3548,3556] Adding the "dynamic port switching" feature
* support.
* --> Adding PI-CIT interface for FMM Port(Destination) reconfiguration
* 
* Hydra_Software_Devel/77   11/10/09 2:34p speter
* SW7405-2959: [3548] Adding DTS-HD support
* 
* Hydra_Software_Devel/76   10/14/09 4:06p gautamk
* sw7405-2740:[7405] adding PI support for WMA DRC, Merging to mainline
* 
* Hydra_Software_Devel/NEXTGEN_WMA_USERCFG_BRANCH/1   9/24/09 5:06p gautamk
* sw7405-2740:[7405] adding PI support for WMA DRC
* 
* Hydra_Software_Devel/75   9/22/09 5:49p gautamk
* SW7405-3058: [7405] Adding support for DRA decoder.
* 
* Hydra_Software_Devel/74   9/11/09 6:45p sgadara
* SW3548-2188: [3548,3556] Adding support for AMR decode algorithm.
* 
* Hydra_Software_Devel/73   6/17/09 5:17p gautamk
* PR56011: [7405] Implementing code review comments
* 
* Hydra_Software_Devel/72   6/12/09 3:43p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/71   6/9/09 11:37a gautamk
* PR47949: [7405]Changing name from PCMOnAVI to PCMWAV
* 
* Hydra_Software_Devel/70   6/4/09 3:05p gautamk
* PR47949: [7405] Adding PI support for PCM On Avi
* 
* Hydra_Software_Devel/69   5/21/09 12:30p gautamk
* PR52383: [7405] Allocating DBG buffer of size 4, when it is disabled.
* 
* Hydra_Software_Devel/68   5/11/09 3:53p gautamk
* PR52383: [7405] Adding algorithm support for DTS decoder.
* 
* Hydra_Software_Devel/67   4/20/09 3:56p gautamk
* PR52383: [7405] Creating seperate event handles for each task.
* 
* Hydra_Software_Devel/66   4/2/09 1:37p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/65   4/1/09 4:38p gautamk
* PR53764: [7405]
* PR53393: [7405] Memory optimization in RAP PI
* 
* Hydra_Software_Devel/64   3/23/09 2:56p gautamk
* PR53419: [7405] non-isr function hsould not be called from isr
* functions.
* 
* Hydra_Software_Devel/63   1/22/09 5:26p gautamk
* PR50455: [7405] Setting VOLUME STEP RAMP to 0x20 when DSP FW triggers
* EnableRamp interrupt.
* 
* Hydra_Software_Devel/62   12/23/08 1:33p gautamk
* PR48850: [7405]  adding support for DVD LPCM. Merging to mainline
* 
* Hydra_Software_Devel/RAP_DVD_LPCM_SUPPORT/1   11/25/08 6:25p gautamk
* PR48850: [7405]  adding support for DVD LPCM
* 
* Hydra_Software_Devel/61   9/22/08 9:12a speter
* PR 45032: [3548,3556] Adding Framesync user config params related
* changes
* 
* Hydra_Software_Devel/60   9/12/08 1:23p gautamk
* PR45739: [7405] removing unwanted function call.
* 
* Hydra_Software_Devel/59   8/12/08 10:23a speter
* PR 45032: [3548,3556] Adding PES Framesync user config params related
* changes
* 
* Hydra_Software_Devel/58   5/29/08 2:13a speter
* PR 38950: [3548,3556] Adding Audio Descriptor support
* 
* Hydra_Software_Devel/57   5/21/08 7:20p gdata
* PR41726: [7405] Implementing review comments for Interrputs
* 
* Hydra_Software_Devel/56   5/5/08 11:38a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/55   5/2/08 2:43p gautamk
* PR41726: [7405] Adding PI support for Fw Verification in mainline
* 
* Hydra_Software_Devel/54   4/28/08 4:27p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/53   4/25/08 5:20p gautamk
* PR41726: [7405] PI support for WMA PRO
* 
* Hydra_Software_Devel/52   4/22/08 4:58p speter
* PR 38950: [3548] Adding CustomVoice (Post Processing) Support
* 
* Hydra_Software_Devel/51   4/11/08 12:50a gautamk
* PR34648: [7405] Adding PI support for Trick modes.
* 
* Hydra_Software_Devel/50   4/3/08 6:48p sushmit
* PR 34648: [7405] Removing hack for RBUFs for all paths.
* 
* Hydra_Software_Devel/49   4/3/08 3:40p gautamk
* PR34648: [7405] Adding PI support for WMA STD
* 
* Hydra_Software_Devel/48   4/1/08 9:30p gautamk
* PR34648: [7405]Adding Support for WMASTD
* 
* Hydra_Software_Devel/47   3/13/08 2:44p sushmit
* PR 34648: [7405] Checking in AACHE Support Changes
* 
* Hydra_Software_Devel/46   3/4/08 6:40p gautamk
* PR34648: [7405] adding paramater in hRapch to have the userconfig
* parameters in Appformat for some fields.
* 
* Hydra_Software_Devel/45   2/29/08 4:22p gautamk
* PR34648: [7405] Changing file names includes
* 
* Hydra_Software_Devel/44   2/29/08 3:46p gautamk
* PR34648: [7405] Removing Unwanted function
* 
* Hydra_Software_Devel/43   2/28/08 4:50p sushmit
* PR 40092: [7405] Adding branch for NEWFWARCH Code.
* 
* Hydra_Software_Devel/42   2/26/08 7:23p gautamk
* PR34648: [7405] Changing the structure names from DDp to Ac3Plus
* 
* Hydra_Software_Devel/41   2/19/08 4:06p gautamk
* PR34648: [7405] Updating function names.
* 
* Hydra_Software_Devel/40   2/19/08 3:31p gautamk
* PR34648: [7405] updating file names and fuction names
* 
* Hydra_Software_Devel/39   2/15/08 8:44p gautamk
* PR34648: [7405]Adding User Config for Passthru
* Adding spdif CBit buffer.
* 
* Hydra_Software_Devel/38   2/11/08 7:02p gautamk
* PR34648: [7405] Adding PI Support for DDP.
* 
* Hydra_Software_Devel/37   2/11/08 2:29p gautamk
* PR34648: [7405] Adding FreeCtxBuffer Function to free the buffer usage
* at DSPCHN_STOP.
* Adding support for Decode TSM
* 
* Hydra_Software_Devel/36   2/7/08 2:40p gautamk
* PR34648: [7405] implementing getDefaultConfigParams,
* getCurrentConfigparams.
* Removing compilation warning of
* brap_fwif.h
* brap_fwif_user_config.h
* brap_fwif_proc.c
* brap_fwif_dwnldutil.c
* 
* Hydra_Software_Devel/35   2/6/08 8:09p nitinb
* PR34648: [7405] Fixed compilation warnings.
* 
* Hydra_Software_Devel/34   2/6/08 5:30p nitinb
* PR34648: [7405] Removed unused macros
* 
* Hydra_Software_Devel/33   1/31/08 1:57p gautamk
* PR34648: [7405] Changes for TSM config params and Added CreateFwTask
* Function
* 
* Hydra_Software_Devel/32   1/30/08 4:36p gautamk
* PR34648: [7405] Updating design for downloading FW. Now download FW ids
* are coming from common arrary between PI/FW
* 
* Hydra_Software_Devel/31   1/30/08 3:05p gdata
* PR34648: [7405] Adding support for status buffers
* 
* Hydra_Software_Devel/30   1/29/08 7:09p gdata
* PR34648: [7405] Adding support for close calls
* 
* Hydra_Software_Devel/29   1/29/08 4:16p gautamk
* PR34648: [7405] Adding config params for MPEG
* 
* Hydra_Software_Devel/28   1/28/08 7:46p gautamk
* PR34648: [7405] Adding config param programming for Ac3 as per new
* design in which hRap stores config struct in the Format FW expects
* 
* Hydra_Software_Devel/27   1/28/08 3:13p gautamk
* PR34648: [7405] removing unwanted structure and includes
* 
* Hydra_Software_Devel/26   1/24/08 8:17a sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/25   1/18/08 3:53p gautamk
* PR34648: [7405] Changes for statr time download
* 
* Hydra_Software_Devel/24   1/16/08 4:50p nitinb
* PR34648: [7405] Added support for programming configuration parameters
* 
* Hydra_Software_Devel/23   1/16/08 3:21p gautamk
* PR34648: [7405] Checkin in updated code
* 
* Hydra_Software_Devel/22   1/11/08 6:39p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/21   1/10/08 6:45p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/20   1/10/08 3:18p sushmit
* PR 34648: [7405] Removing hDsp from hDspChn
* 
* Hydra_Software_Devel/19   1/10/08 2:25p sushmit
* PR 34648: [7405] Fixing hDsp population
* 
* Hydra_Software_Devel/18   1/10/08 12:08p sushmit
* PR34648: [7405] Updating Async Event ID handling
* 
* Hydra_Software_Devel/17   1/7/08 4:48p sushmit
* PR34648: [7405] Updating Start
* 
* Hydra_Software_Devel/16   1/7/08 11:59a sushmit
* PR34648: [7405] Modifying CMD-RSP logic & FIFO usage logic.
* 
* Hydra_Software_Devel/15   1/4/08 1:49p sushmit
* PR 34840: [7405] Updating code for FIFO INIT
* 
* Hydra_Software_Devel/14   1/3/08 6:52p nitinb
* PR 34648: [7405] Integration of FW-Interface module with other modules
* 
* Hydra_Software_Devel/13   1/2/08 4:13p gdata
* PR34648: [7405] Adding the interrupt changes
* 
* Hydra_Software_Devel/12   12/31/07 7:16p sushmit
* PR 34648: [7405] Updating more code.
* 
* Hydra_Software_Devel/11   12/31/07 7:09p sushmit
* PR34648: [7405] Updating more code.
* 
* Hydra_Software_Devel/10   12/31/07 4:54p nitinb
* PR34648: [7405] Resolved circular dependancy in files brap_dsp_priv.h
* and brap_fwif.h by moving handle definitions to brap_types_priv.h.
* 
* Hydra_Software_Devel/9   12/31/07 3:02p nitinb
* PR34648: [7405] Updated data structures to integrate firmware interface
* module with other PI modules.
* 
* Hydra_Software_Devel/8   12/28/07 9:53p sushmit
* PR 34648: [7405] Initial Checkins after DSP Bootup completion.
* 
* Hydra_Software_Devel/7   12/20/07 3:02p srajapur
* PR 34648: [7405] Implemented APIs for creating/destroying audio
* processing stages.
* 
* Hydra_Software_Devel/6   12/14/07 10:19p gautamk
* PR34648:  [7405] Adding updated files for downloading FW module.
* 
* Hydra_Software_Devel/5   12/14/07 2:01p gautamk
* PR34648:  [7405] Adding updated files for downloading FW module.
* 
* Hydra_Software_Devel/4   10/31/07 1:51p nitinb
* PR 34648: [7405] Implemented review comments of new API interface as
* mentioned in PR progress-log
* 
* Hydra_Software_Devel/3   10/25/07 5:34p nitinb
* PR 34648: [7405] Included header file brap_fwif_dwnldutil.h
* 
* Hydra_Software_Devel/2   10/3/07 4:00p gautamk
* PR34648: [7405] check in updated files
* 
* Hydra_Software_Devel/44   8/21/07 2:07p speter
* PR 34202: [3563] Provide Sh Bass post processing algorithm support
* 
* Hydra_Software_Devel/43   7/30/07 5:42p speter
* PR 33608: [3563] Providing support for Sh Surround post processing
* algorithm
* 
* Hydra_Software_Devel/42   7/16/07 7:01p bhanus
* PR 32986 : [7440 B0] Making code generic for Sequential and Non-
* Sequential data flow.
* 
* Hydra_Software_Devel/41   7/13/07 1:46p bhanus
* PR 32986: [7440 B0] Code modifed for Sequential path bring up.
* 
* Hydra_Software_Devel/40   6/27/07 8:34p gautamk
* PR32504: Providing  SPDIF0/1 C-bit  params to DSP1 for XCD.
* 
* Hydra_Software_Devel/39   6/15/07 8:03p gautamk
* PR30069: [7440] Added DTS-Extension Support.
* 
* Hydra_Software_Devel/38   6/15/07 2:25p kagrawal
* PR 30069: [7440] Merging Phase2.0 branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/6   6/14/07 1:29p bhanus
* PR 30069 : [7440 B0] Added code to support:
* - HBR path
* - HBR configuration of I2sMulti_1 and Mai
* - Passing HBR flag to DSP1
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/5   6/12/07 5:50p kagrawal
* PR 30069: [7440] Added support for independent channel delay for DSP1
* audio processing
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/4   6/11/07 7:06p gautamk
* PR30069: New Config Parameter structure for Speaker Management
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/3   6/11/07 3:30p gautamk
* PR30069: New Config Parameter structure for Speaker Management
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/2   6/9/07 5:29p kagrawal
* PR 30069: [7440] Added support for DTS-Neo PostProcessing
* 
* Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/1   6/7/07 2:20p kagrawal
* PR 30069: Added support for branches and stages for DSP1 audio post
* processing
* 
* Hydra_Software_Devel/37   6/12/07 10:02p sushmit
* PR 27646: Updating 7405 PI
* 
* Hydra_Software_Devel/36   6/7/07 7:25p bhanus
* PR 30069 : [7440B0] Added code for Secondary Metadata Processing.
* 
* Hydra_Software_Devel/35   6/5/07 10:32a speter
* PR 31827: [3563] Merging from Phase 2.0 release branch to main
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/1   5/29/07 1:45p sushmit
* PR28719: Adding code for independent delay & fixing MULPP broken
* 
* Hydra_Software_Devel/34   5/14/07 7:37p kagrawal
* PR 30069: [7440] Added BRAP_SetTsmTransitionThreshold()
* 
* Hydra_Software_Devel/33   5/11/07 8:58p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/32   5/7/07 4:33p nitinb
* PR 28862: [3563] Added support for post processing algorithm BBE
* 
* Hydra_Software_Devel/31   4/20/07 1:12p nitinb
* PR 29947: Free interframe buffers allocated for post process stages on
* decode stop
* 
* Hydra_Software_Devel/30   4/18/07 4:28p sushmit
* PR 28714: Using field in DspChn instead of separate APIs
* 
* Hydra_Software_Devel/29   4/6/07 12:19p kagrawal
* PR 28561: [7440] Adding support for PVR functionality
* 
* Hydra_Software_Devel/28   4/4/07 11:42a kagrawal
* PR 28865: Fixed 7440 compilation break due to XEN changes in 3563
* 
* Hydra_Software_Devel/27   4/4/07 1:14a nitinb
* PR 28865: Added support for post processing algorithm XEN
* 
* Hydra_Software_Devel/26   4/3/07 1:49p sushmit
* PR 28714: Multiple PP branch support
* 
* Hydra_Software_Devel/25   3/30/07 6:11p kagrawal
* PR 28561: [7440] Redefining BRAP_DSP_P_SM_STAGE_INDEX to 0
* 
* Hydra_Software_Devel/24   3/29/07 5:16p kagrawal
* PR 28561: Fixed compilation breaks for 3563 due to the last check-in
* 
* Hydra_Software_Devel/23   3/29/07 5:05p kagrawal
* PR 28561: [7440] Adding support for DDBM user config params
* 
* Hydra_Software_Devel/22   3/28/07 5:06p nitinb
* PR 28177: Added field sSpdifChStatusParams to struct
* BRAP_DSPCHN_P_AudioParams. This controls (enables/disables) updation
* of SPDIF channel status bit by DSP FW
* 
* Hydra_Software_Devel/21   3/14/07 12:26p kagrawal
* PR 28561: [7440, 3563] Restoring 7440. Changes in config params s per
* the new MIT. [Checking in on Nitin's behalf]
* 
* Hydra_Software_Devel/20   3/8/07 6:10p nitinb
* PR 26864: Changes to support configuration parameters for post
* processing algorithms
* 
* Hydra_Software_Devel/19   3/1/07 10:07a kagrawal
* PR 27912: New MIT changes for 7440
* 
* Hydra_Software_Devel/18   2/27/07 9:02p kagrawal
* PR 27912: Modifications to take Audio Processing config params
* 
* Hydra_Software_Devel/17   2/27/07 7:06p nitinb
* PR 26864: Fixed compilation errors for 7440
* 
* Hydra_Software_Devel/16   2/27/07 6:28p nitinb
* PR 26864: Changed PP FW buffer pointer and size arrays to single
* elements in BRAP_DSP_P_MemInfo structure
* 
* Hydra_Software_Devel/15   1/29/07 3:31p bhanus
* PR 25607: Added Support for Multi-executable DTS/DTS-HD
* 
* Hydra_Software_Devel/14   1/12/07 8:28p sushmit
* PR 26864:
* New MIT structure modfications
* Added elements for PP memory calculations
* Modified BRAP_DSP_P_DownloadExecFW to take care of PP branches
* 
* Hydra_Software_Devel/13   1/10/07 3:18p bselva
* PR 26864: Checking in the changes for new decoder config buffer
* 
* Hydra_Software_Devel/12   1/9/07 6:22p bselva
* PR 26864: Checking in the changes for new dec config buffer param
* interface
* 
* Hydra_Software_Devel/11   1/3/07 4:24p speter
* PR 25346: [7440/3563] - Removing the bCompressed field and instead
* using ePcm as algo ID
* 
* Hydra_Software_Devel/10   12/28/06 1:35p speter
* PR 25346: [3563] SPDIF/HDMI In Feature - DSP source related changes
* 
* Hydra_Software_Devel/9   12/7/06 1:27p sushmit
* PR 25346: Merging 3563 to Hydra
* 
* Hydra_Software_Devel/RAP_3563_Merge/2   12/4/06 12:18p sushmit
* PR 24037: Adding 3563 specific preprocessor directive.
* 
* Hydra_Software_Devel/RAP_3563_Merge/1   12/3/06 11:21p sushmit
* PR 24037: Merging 7440 to 3563.
* 
* Hydra_Software_Devel/8   11/29/06 12:18p sushmit
* PR 25607: DSP Index related changes, more to come w.r.t. RM
* 
* Hydra_Software_Devel/7   11/22/06 7:20p kagrawal
* PR 25607: Removed BRAP_DSPCHN_P_SetPointers()
* 
* Hydra_Software_Devel/6   11/10/06 11:04a kagrawal
* PR 25607: Added support for multi channel output on I2s_multi
* 
* Hydra_Software_Devel/5   10/10/06 6:45p kagrawal
* PR 24717:  Post bring up code clean-up
* 
* Hydra_Software_Devel/4   10/6/06 5:40p kagrawal
* PR 24717: Added support for passthru in 7440
* 
* Hydra_Software_Devel/3   10/2/06 4:32p kagrawal
* PR 20654: Added support for Start-StopAtPTS
* 
* Hydra_Software_Devel/2   9/22/06 12:51p kagrawal
* PR 20654: Removed bOutputLfeOn from the AlgoConfig structure
* 
* Hydra_Software_Devel/1   8/21/06 6:55p kagrawal
* PR 20654: Added
* 
* Hydra_Software_Devel/66   8/13/06 6:12p nitinb
* PR 22699: Added TSM upper threshold field to dspchn_p_settings.
* 
* Hydra_Software_Devel/65   6/25/06 6:34p kagrawal
* PR 20590: Merged Stop/Start at PTS code from RAP_7411D0_PR20590 branch
* 
* Hydra_Software_Devel/64   6/25/06 3:40p kagrawal
* PR 22297: Added support for stereo downmix path concurrent with
* multichannel path
* 
* Hydra_Software_Devel/63   6/6/06 11:18a sushmit
* PR18604: Updating Encoder PI for 7401.
* 
* Hydra_Software_Devel/62   5/17/06 11:31a sushmit
* PR18604: Updating Encoder PI for 7401.
* 
* Hydra_Software_Devel/61   4/24/06 10:52a sushmit
* PR18604: Updating Encoder MIT.
* 
* Hydra_Software_Devel/60   4/11/06 6:02p sushmit
* PR18604: Modifying
* #if (( BCHP_CHIP == 7401 )||( BCHP_CHIP == 7400 ))
* With code like
* #ifndef BCHP_7411_VER
* 
* Hydra_Software_Devel/59   4/11/06 8:51a nitinb
* PR 20670: Implement code review comments for dsp module of RAP PI
* 
* Hydra_Software_Devel/58   4/7/06 2:51p sushmit
* PR18604: Updating Encoder MIT & Watchdog.
* 
* Hydra_Software_Devel/57   4/3/06 6:05p rjain
* PR 20318: Implementing review comments for AM
* 
* Hydra_Software_Devel/56   3/30/06 4:44p bselva
* PR 19878: Checking in the changes required for 7400 phase 0.5 features
* 
* Hydra_Software_Devel/55   3/28/06 12:05p rnayak
* PR20439: Restore Bangalore Raptor files after magnum vob corruption on
* 17th March 2006
* 
* Hydra_Software_Devel/55   3/21/06 6:26p sushmit
* PR18604: Updating review comments.
* 
* Hydra_Software_Devel/54   3/17/06 2:34p nitinb
* PR 20117: Initialize DDBM table on every "start decode"
* 
* Hydra_Software_Devel/53   3/16/06 5:19p nitinb
* PR 20117: Implement dolby digital bass management algorithm
* 
* Hydra_Software_Devel/52   3/10/06 5:58p kagrawal
* PR 19615: Encorporated RDB change for multi channel I2S support
* 
* Hydra_Software_Devel/51   2/28/06 10:14p nitinb
* PR 19502: Added prototype for BRAP_DSP_P_InitInterframeBuf function.
* Updated prototype of function BRAP_DSP_AllocateInterframeBuf.
* 
* Hydra_Software_Devel/50   2/28/06 11:13a sushmit
* PR18604: Fix for 7411 compilation issues.
* 
* Hydra_Software_Devel/49   2/27/06 4:16p sushmit
* PR18604: Fixing 7411 compilation issues.
* 
* Hydra_Software_Devel/48   2/17/06 9:52a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/47   1/30/06 2:27p nitinb
* PR 17882: Increased frame sync executable buffer size (as required for
* DDP)
* 
* Hydra_Software_Devel/46   1/24/06 2:46p bselva
* PR 19168: Checking in the changes for 7400 bringup
* 
* Hydra_Software_Devel/RAP_AUD_ENC/5   2/6/06 4:14p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/RAP_AUD_ENC/3   12/19/05 6:30p sushmit
* PR18604: Updating for combined decoder & encoder scheduler.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   12/14/05 5:30p sushmit
* [PR18604]:Susmit
* Merging Encoder to latest.
* SUSMIT
* 14 DEC 2005
* 
* Hydra_Software_Devel/RAP_AUD_ENC/1   12/14/05 5:10p sushmit
* [PR18604]:Susmit
* Merging Encoder to latest.
* SUSMIT
* 14 DEC 2005
* 
* Hydra_Software_Devel/43   11/30/05 12:07p susmit
* Merging Decoder & Encoder
* 
* Hydra_Software_Devel/42   12/5/05 10:35a nitinb
* PR 18061: Making TSM log support compile time selectable
* 
* Hydra_Software_Devel/41   12/2/05 3:28p nitinb
* PR 18061: Raptor memory allocation must be less than 0.5M for MPEG/AC-3
* only
* 
* Hydra_Software_Devel/40   11/16/05 9:16p rjain
* PR 18129 : Making the changes reqd for PI-Fw interface to add an output
* port on the fly for a DEC channel.
* 
* Hydra_Software_Devel/39   11/16/05 9:11p kagrawal
* PR 17590: Endian independent changes for multi codec
* 
* Hydra_Software_Devel/38   11/2/05 3:24p bselva
* PR 17590: Added watchdog support for 7401
* 
* Hydra_Software_Devel/37   10/25/05 5:46p nitinb
* PR 17415: Correcting buffer sizes for downloading decode and passthru
* firmware images
* 
* Hydra_Software_Devel/36   10/24/05 5:14p nitinb
* PR 17415: Added decoder configuration parameters for DDP, AAC, AAC-HE
* and MPEG
* 
* Hydra_Software_Devel/35   10/23/05 8:15p nitinb
* PR 17415: Added decoder configuration parameters for DDP, AAC, AAC-HE
* and MPEG
* 
* Hydra_Software_Devel/34   10/20/05 7:30p nitinb
* PR 17651: Changing checks for BCHP_CHIP ==/!= 7401 to check for
* BCHP_7411_VER instead
* 
* Hydra_Software_Devel/33   10/18/05 6:21p nitinb
* PR 17399: PVR: wrap-around with 'slow' trickmode result in audio noise
* at wrap-around point
* 
* Hydra_Software_Devel/32   10/2/05 10:00p nitinb
* PR 17415: Add support for advance codec for Raptor PI
* 
* Hydra_Software_Devel/31   9/29/05 8:19p shyam
* PR 17367: Handling multiple codecs in single codec memory allocation
* 
* Hydra_Software_Devel/30   9/22/05 10:00p nitinb
* PR 16889: Implemented private function to enable watchdog in DSP
* 
* Hydra_Software_Devel/29   9/22/05 1:09p nitinb
* PR 16899: Making changes in the code for watchdog feature
* 
* Hydra_Software_Devel/28   9/20/05 1:11p nitinb
* PR 16899: Making changes in the code for watchdog feature
* 
* Hydra_Software_Devel/27   9/15/05 8:29p nitinb
* PR 16899: Added function to get RAP handle
* 
* Hydra_Software_Devel/26   9/14/05 7:39p nitinb
* PR 17042: Raptor does not support sample rate change in the middle of a
* stream
* 
* Hydra_Software_Devel/25   9/13/05 7:41p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/3   8/19/05 12:02p bmishra
* PR 16148: Mergeded the latest changes in the main branch.
* 
* Hydra_Software_Devel/24   8/3/05 9:16p bselva
* PR 15766: Support for BRAP_GetRevision API added
* 
* Hydra_Software_Devel/23   7/28/05 11:21p nitinb
* PR 16092: Merging new MIT code into main branch
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/7   7/22/05 4:43p nitinb
* PR 16354: Made function BRAP_DSPCHN_P_GetCurrentPTS_isr public
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/6   7/20/05 10:32p nitinb
* PR 16354: Added field bPlayback to structure BRAP_DSPCHN_P_AudioParams
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/5   7/19/05 6:58p nitinb
* PR 16354: Develope PVR features for RAP PI
*
* Hydra_Software_Devel/20   7/13/05 7:43p nitinb
* PR 16093: Registers must be accessed using absolute addresses
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   7/12/05 8:14p bmishra
* PR 16148: Modified the file for 7401 compatibility
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/4   7/11/05 1:05p bselva
* The decode scratch buffer size is reduced to 512 bytes ( As MPEG and
* AC3 Decoder does not use this buffer )
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/3   7/7/05 5:26p nitinb
* PR 15765: Added DSP channel specific offset for ESR_SO0X registers in
* DSPCHN handle
* Storing decoder lock unlock status in DSPCHN handle
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/2   6/24/05 5:46p nitinb
* PR 15925: Removing function BRAP_DSPCHN_P_SPDIFConfig
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/BLR_Raptor7411C0_NewMit_Singlecodec/1   6/23/05 6:56p nitinb
* Merging main branch changes into newmit_singlecodec branch.
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/2   6/14/05 9:04p nitinb
* Correcting the buffer sizes. Allocating worst case buffer sizes as
* required by various algorithms.
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_NewMit_Multicodec/1   6/13/05 2:58p nitinb
* PR 15272: Made changes in code required for new MIT format
* 
* Hydra_Software_Devel/14   6/10/05 8:18p nitinb
* PR 15837: Feature development for the Minor Release of June 2005
* (Implemented TSM log)
* 
* Hydra_Software_Devel/13   5/31/05 10:18a nitinb
* PR 15272: Making interrupt handler code interrupt safe
* 
* Hydra_Software_Devel/12   5/25/05 7:44p nitinb
* PR 15272: Changing name of enumtype BRAP_PtsType to BRAP_DSPCHN_PtsType
* and name of struct BRAP_PtsInfo to BRAP_DSPCHN_PtsInfo
* 
* Hydra_Software_Devel/11   5/25/05 3:10p nitinb
* PR 15272: Removing inclusion of bchp_aud_dsp* header files
* 
* Hydra_Software_Devel/10   5/25/05 11:46a nitinb
* PR 15272: Implementing FirstPTSReady, PTSError and StreamInfo
* interrupts
* 
* Hydra_Software_Devel/9   5/13/05 12:33p nitinb
* PR 15160: Need ability to set various fields in the SPDIF channel
* status
* 
* Hydra_Software_Devel/8   5/4/05 5:25p nitinb
* PR 14057: Added DSP channel state information in channel handle
* 
* Hydra_Software_Devel/7   4/21/05 4:07p nitinb
* PR 14507: Made changes in AudioParams structure as required by simul
* mode
* 
* Hydra_Software_Devel/6   4/14/05 11:47a nitinb
* PR 14507: Merging changes from multicodec branch to main branch
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_MultiCodec/2   3/28/05 11:12a nitinb
* PR 14507: Added channel memory information field in channel handle
* 
* Hydra_Software_Devel/BLR_Raptor7411C0_MultiCodec/1   3/18/05 8:31p nitinb
* PR 13082: Added pass thru related parameters to dsp handle
* 
* Hydra_Software_Devel/5   3/10/05 2:05p nitinb
* PR 13082: Added fields BRAP_OutputMode and BAVC_Timebase to structure
* BRAP_DSPCHN_P_AudioParams
* 
* Hydra_Software_Devel/3   2/16/05 8:29p bmishra
* PR 14143: Implemented Phase 1 code review comments for the interfaces
* only
* 
* Hydra_Software_Devel/2   1/24/05 3:59p nitinb
* PR 13082: Added AAC donwmix and TSM related functions
* 
* Hydra_Software_Devel/3   11/2/04 6:55p nitinb
* PR 13082: Adding BRAP_DSPCHN_Start code
* 
* Hydra_Software_Devel/2   10/29/04 1:14p nitinb
* PR 13082: Adding DSP initialization code
* 
* Hydra_Software_Devel/1   10/26/04 8:22a nitinb
* PR13082: Initial Version
* 
***************************************************************************/

#ifndef BRAP_DSP_PRIV_H__
#define BRAP_DSP_PRIV_H__

#include "brap_types.h"
#include "brap_dsp.h"
#include "brap_dspchn.h"
#include "brap_fwdwnld_priv.h"
#include "brap_cit_priv.h"
#include "brap_fwif_priv.h"
#include "brap_fwuserconfig_priv.h"
#ifdef __cplusplus
extern "C" {
#endif

#define	BRAP_DSP_P_MAX_AUDIO_CHANNELS					3

/* Inter-channel delay user param buffer size */
#ifdef BRAP_P_FW_DBG_ENABLE
#define BRAP_DSP_P_DBG_BUF_SIZE     2*1024*1024
#else
#define BRAP_DSP_P_DBG_BUF_SIZE     4
#endif

#define BRAP_DSP_P_NUM_FIFOS            18
#define BRAP_DSP_P_NUM_PTR_PER_FIFO     5

/* Hardcode cmd queue as FIFO 0 */
#define BRAP_DSP_P_FIFO_CMD     0
/* Hardcode response queue which doesn't have any task id associated */
#define BRAP_DSP_P_FIFO_GENERIC_RSP     1
#define BRAP_DSP_P_FIFO_DEBUG           17
#define BRAP_REALVIDEO_DSP_P_FIFO_PDQ   15
#define BRAP_REALVIDEO_DSP_P_FIFO_PRQ   16
#define BRAP_REALVIDEO_DSP_P_FIFO_DSQ   14
#define BRAP_DSP_P_FIFO_INVALID         ((unsigned int)(-1))

#define BRAP_GET_TASK_INDEX(uiTaskId) (uiTaskId -BRAP_FWIF_P_TASK_ID_START_OFFSET)

#define BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS   400
#define BRAP_DSPCHN_P_START_STOP_EVENT_TIMEOUT_IN_MS   400


/***************************************************************************
Summary:
	This structure contains DSP settings.

***************************************************************************/
typedef struct BRAP_DSP_P_Settings
{
	BRAP_DSP_Settings sDspExtSettings;
} BRAP_DSP_P_Settings;

typedef struct BRAP_FWIF_P_UserConfigStruct
{
    BRAP_FWIF_P_MpegConfigParams	    sMpegConfigParam;	
    BRAP_FWIF_P_Ac3ConfigParams	        sAc3ConfigParam;
    BRAP_FWIF_P_Ac3ConfigParams	        sAc3PlusConfigParam;
    BRAP_FWIF_P_AacheConfigParams       sAacheConfigParam;
    BRAP_FWIF_P_WmaConfigParams         sWmaStdConfigParam;    
    BRAP_FWIF_P_WmaProConfigParams      sWmaProConfigParam;    
    BRAP_FWIF_P_LpcmUserConfig      	    sLpcmDvdConfigParam;       
    BRAP_FWIF_P_DtsHdConfigParams       sDtsBroadcastConfigParam;
    BRAP_FWIF_P_DtsHdConfigParams       sDtsHdConfigParam;   
    BRAP_FWIF_P_PcmWavConfigParams       sPcmWavConfigParam;       
    BRAP_FWIF_P_AmrConfigParams         sAmrConfigParam;    
    BRAP_FWIF_P_DraConfigParams         sDraConfigParam;    
    BRAP_FWIF_P_RalbrConfigParams       sRealAudioLbrConfigParam;
    BRAP_FWIF_P_DolbyPulseUserConfig    sDolbyPulseConfigParam;
    BRAP_FWIF_P_DDPMs10ConfigParams sMs10DDPConfigParam;    
    BRAP_FWIF_P_DDPMultiStreamConfigParams sMs11DDPConfigParam;        
    BRAP_FWIF_P_DDPMultiStreamConfigParams sMs11AC3ConfigParam;            
    
/*	BRAP_FWIF_P_AacConfigParams         sAacConfigParam;
	BRAP_FWIF_P_DtsConfigParams	        sDtsConfigParam;
  	BRAP_FWIF_P_DtshdConfigParams       sDtshdConfigParam;
	BRAP_FWIF_P_BdlpcmConfigParams	    sBdlpcmConfigParam;
	BRAP_FWIF_P_LpcmHdDvdConfigParams	sLpcmHdDvdConfigParam;    
	BRAP_FWIF_P_LpcmDvdConfigParams	    sLpcmDvdConfigParam;    
	BRAP_FWIF_P_Ac3PlusConfigParams		sAc3LosslessConfigParam;
	BRAP_FWIF_P_MlpConfigParams		    sMlpConfigParams;	
	BRAP_FWIF_P_DtslbrConfigParams      sDtslbrConfigParam;
	BRAP_FWIF_P_Ac3PlusConfigParams     sDdp7_1ConfigParam;	
	BRAP_FWIF_P_MpegMcConfigParams      sMpegMcConfigParam;*/
	BRAP_FWIF_P_TsmConfigParams         sDecodeTsmConfigParam;	
	BRAP_FWIF_P_TsmConfigParams         sEncodeTsmConfigParam;	
	BRAP_FWIF_P_TsmConfigParams         sPassthruTsmConfigParam;	
	BRAP_FWIF_P_PassthruConfigParams    sPassthruConfigParam;	  
    BRAP_FWIF_P_FrameSyncConfigParams   sFrameSyncConfigParams;
}BRAP_FWIF_P_UserConfigStruct;


/***************************************************************************
Summary:
	This structure stores user config parameter in App format who are converted to Q131 format
	while programming to FW.

***************************************************************************/
typedef struct BRAP_DEC_P_UserConfigStruct
{
/*MPEG*/
    int32_t i32MpegLeftChannelGain;
    int32_t i32MpegRightChannelGain;
/*Ac3*/    
    int32_t i32Ac3PcmScale;
    int32_t i32Ac3DynScaleHigh;
    int32_t i32Ac3DynScaleLow;

/*MS11 AC3*/    
    int32_t i32Ms11Ac3PcmScale_DownMixPath;
    int32_t i32Ms11Ac3DynScaleHigh_DownMixPath;
    int32_t i32Ms11Ac3DynScaleLow_DownMixPath;

/*MS11 DDP*/    
    int32_t i32Ms11DdpPcmScale_DownMixPath;
    int32_t i32Ms11DdpDynScaleHigh_DownMixPath;
    int32_t i32Ms11DdpDynScaleLow_DownMixPath;
    
/*Ac3Plus*/    
    int32_t i32Ac3PlusPcmScale;
    int32_t i32Ac3PlusDynScaleHigh;
    int32_t i32Ac3PlusDynScaleLow;
/* AAC/AACHE */    
    int32_t i32AacDrcGainControlCompress;
    int32_t i32AacDrcGainControlBoost;

/*DTS Broadcast*/
    int32_t i32DtsBroadcastDynScaleHigh;
    int32_t i32DtsBroadcastDynScaleLow;
   
    bool    bMsUsageMode;  
    bool    bMpegConformanceMode;
}BRAP_DEC_P_UserConfigStruct;

/***************************************************************************
Summary:
	This structure contains decoder related settings.

***************************************************************************/
typedef struct BRAP_P_DecoderSettings
{
	BRAP_FWIF_P_UserConfigStruct sUserConfigStruct; 
       BRAP_DEC_P_UserConfigStruct  sUserConfigAppFormat;
} BRAP_P_DecoderSettings;

/***************************************************************************
Summary:
	This structure contains DSPCHN channel settings.

***************************************************************************/
typedef struct BRAP_DSPCHN_P_Settings
{
    unsigned int    tbd;
}BRAP_DSPCHN_P_Settings;


/***************************************************************************
Summary:
	Handle structure for DSP device.
***************************************************************************/
typedef struct BRAP_DSP_P_Device
{
    BCHP_Handle hChip;  /* handle to chip interface*/
    BREG_Handle hRegister;  /* handle to register interface */
    BMEM_Handle hHeap;  /* handle to memory interface */
    BINT_Handle hInt;   /* handle to interrupt interface*/
    BRAP_Handle hRap;   /* handle to parent audio device*/
    unsigned int    uiDspIndex; /* DSP object index */
    uint32_t    ui32Offset; /* Offset of a register of current 
                                          DSP from the corresponding register of 
                                          the first DSP */
    uint32_t                 ui32DbgBufAddr; /* Address of 2MB Debug buffer for FW */



    /* Following field is used in interrupt routine. Need cleanup!!! */                                                                
    BRAP_DSPCHN_Handle  phAudChannelHandle[BRAP_DSP_P_MAX_AUDIO_CHANNELS];
                                                                            /* handles to DSP contexts for
                                                                                this DSP channel */
    BKNI_EventHandle hEvent[BRAP_RM_P_MAX_FW_TASK_PER_DSP]; /* Event to be used during slow handshake with 
                                                  firmware */
    BINT_CallbackHandle     hDspAckCallback;    /* This will install the Callback for
                                                  Ping the DSp*/
                                                    
    BRAP_FWIF_MsgQueueHandle      hCmdQueue;      /* Cmd queue */
    BRAP_FWIF_MsgQueueHandle      hGenRspQueue;      /* Generic Response queue */
#ifdef RAP_VIDEOONDSP_SUPPORT    
    BRAP_FWIF_MsgQueueHandle      hPDQueue;      /* Picture Delivery queue(PDQ) */
    BRAP_FWIF_MsgQueueHandle      hPRQueue;      /* Picture Release queue(PRQ) */
    BRAP_FWIF_MsgQueueHandle      hDSQueue;      /* Display queue(DSQ) */    
#endif    
    
    uint32_t        ui32DSPFifoAddrStruct; /* contains fifo address structure */
    bool            bFifoUsed[BRAP_DSP_P_NUM_FIFOS]; /* Whether this Fifo is used by any task */
    uint32_t        ui32EventIdMatrix[BRAP_RM_P_MAX_FW_TASK_PER_DSP];  /* Contains information abt. event ids already enabled */
} BRAP_DSP_P_Device;

/***************************************************************************
Summary:
***************************************************************************/
typedef struct BRAP_DSPCHN_OpPortConfig
{
	BRAP_OutputPort			eOutputPortType;
	BRAP_OP_Pll			ePll;
#if (BRAP_INDEP_OP_DELAY_SUPPORTED == 1)
    /* The following information is required for the FW to implement indep
    delay on an output port */
    int             iDelay; /* The independent delay to be applied to this 
                            output port in units of milliseconds. 
                            Note: This delay value is used only if the 
                            channel is opened with bSelectDelay as TRUE  */
    unsigned int    uiDlydRBufIndex[BRAP_RM_P_MAX_RBUFS_PER_SRCCH]; 
                            /* If this port has to be delayed at start time, 
                            list the associated RBUFs here. Else leave this
                            field as BRAP_RM_P_INVALID_INDEX */
    unsigned int    uiSrcChIndex; /* If this port has to be delayed at start 
                            time, list the associated SrcCh here. Else leave 
                            this field as BRAP_RM_P_INVALID_INDEX */    
    BRAP_OutputChannelPair eChanPair; /* This output port can be a master 
                            or cloned port. FW needs to know which channel
                            pair data it is carrying */
#endif                            
} BRAP_DSPCHN_OpPortConfig; 


/***************************************************************************
Summary:
***************************************************************************/
typedef struct BRAP_DSPCHN_OpPortParams
{
	unsigned int			uiNumOpPorts;
	BRAP_DSPCHN_OpPortConfig	sOpPortConfig[BRAP_RM_P_MAX_OUTPUTS];
} BRAP_DSPCHN_OpPortParams;

/***************************************************************************
Summary:
***************************************************************************/
typedef enum BRAP_DSPCHN_P_SourceType
{
	BRAP_DSPCHN_P_SourceType_eFMMBuf,
	BRAP_DSPCHN_P_SourceType_eDRAMBuf,     
	BRAP_DSPCHN_P_SourceType_eMax	
} BRAP_DSPCHN_P_SourceType;
/***************************************************************************
Summary: Mixer input params provided by the AM. This will be used by the DSP
    to prepare the Metadata.
    The uiScale and uiPan values can be set to the user passed / default value
    during DSPCHN start.
    TODO: Once RM is designed, this data structure can be a part of MIXER module
***************************************************************************/
typedef struct BRAP_DSPCHN_P_MixerInputParams
{
    unsigned int                uiMixerInputId; /* Mixer input index */
    BRAP_OutputChannelPair      eChanPair;      /* Channel pair fed to this 
                                                   mixer input */
    unsigned int                uiScale;        /* Host scale value for this 
                                                   channel pair */
    unsigned int                uiPan;          /* Host pan value for this 
                                                   channel pair */
}BRAP_DSPCHN_P_MixerInputParams;

/***************************************************************************
Summary: Mixer params provided by the AM. This will be used by the DSP to 
    prepare the Metadata.
    TODO: Once RM is designed, this data structure can be a part of MIXER module
***************************************************************************/
typedef struct BRAP_DSPCHN_P_MixerParams
{
    unsigned int    uiDPId;                     /* DP Id of the mixer */           
    unsigned int    uiMixerId;                  /* Mixer Id */
    BRAP_DSPCHN_P_MixerInputParams sMixerInputParams[BRAP_RM_P_MAX_MIXER_INPUTS];
                                                /* Mixer input params */
}BRAP_DSPCHN_P_MixerParams;


typedef struct BRAP_DSPCHN_P_FMMParams
{
	int8_t				        rBufIndex[BRAP_RM_P_MAX_OP_CHANNELS];
	int8_t				        inputRBufIndex[BRAP_RM_P_MAX_OP_CHANNELS]; /* Used when 
	                                            the input audio source is ring buffer*/  
    BRAP_DSPCHN_P_MixerParams   sMixerParams[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS];

    int8_t                      i8HbrRbufId;
    bool                        bIsSingleI2SHbrMode;
} BRAP_DSPCHN_P_FMMParams;

typedef struct BRAP_DSPCHN_P_DRAMBufDetails
{                                                   
    BRAP_AF_P_sIO_BUFFER            sDecIoBuffer;                                                
	BRAP_AF_P_sIO_GENERIC_BUFFER	sDecIoGenericBuffer;
}BRAP_DSPCHN_P_DRAMBufDetails;

/***************************************************************************
Summary:
    Some AM API will take scale/pan coeffs from the user/host. That API will
    internally program uiScale and uiPan available in sMixerInParams in 
    sDecPathFmmParams and sEncPathFmmParams. These values will be re-informed to 
    the DSP.   
***************************************************************************/
typedef struct BRAP_DSPCHN_DecModeParams
{
    BRAP_DSPCHN_P_SourceType    eDecSourceType;     /* Source type of DSP Channel */
    union
    {
        BRAP_DSPCHN_P_FMMParams sDecPathFmmParams; /* Decode Path FMM rsrcs required 
                                                       for decode channel */ 
        BRAP_DSPCHN_P_DRAMBufDetails  sDecSourceBufDetails;                                                            
    }uDecBufFmmParams;

}BRAP_DSPCHN_DecModeParams;

typedef struct BRAP_DSPCHN_P_VideoParams
{
    BRAP_DSPCHN_VideoType	eType;          /* Video Type */
    unsigned int    ui32DispStripeWidth;                    /*What will be Default Values?? */                    
    unsigned int    ui32DispStripeHeight;                   /*What will be Default Values?? */ 
    unsigned int    ui32NumBuffAvl;                     /*What will be Default Values?? */ 
}BRAP_DSPCHN_P_VideoParams;

/***************************************************************************
Summary:
	This structure contains audio parameters required to decode
	input compressed audio data.

***************************************************************************/
typedef struct BRAP_DSPCHN_P_AudioParams
{
	BRAP_DSPCHN_AudioParams		sExtAudioParams;
	BRAP_DSPCHN_P_VideoParams	sExtVideoParams;    
	BAVC_Timebase				eTimebase;
	unsigned int				uiTransChId;	
	BAVC_XptContextMap	        sXptContextMap;	 
                                        /* This structure holds the CDB and 
										   ITB register addresses for the 
										   corresponding Rave Context 
										   allocated by the Xpt PI.*/	
	bool						bPlayback;
	bool                       	bMultiChanOnI2S;
	BRAP_DSPCHN_DecModeParams	sDecOrPtParams;
	BRAP_DSPCHN_DecModeParams	sSimulPtParams;
	bool						bUpdateChStatusParams;
	BRAP_OP_SpdifChanStatusParams sSpdifChStatusParams[BRAP_AF_P_MAX_NUM_SPDIF_PORTS]; /* This field is valid only
										when bUpdateChStatusParams = TRUE */
    bool                        bFwToUpdateRdPtr;
                                        /* Flag to indicate if FW needs to 
                                           update ring buffer Read Pointers */
} BRAP_DSPCHN_P_AudioParams;


/***************************************************************************
Summary:
	This structure returns information of sample rate change in stream
***************************************************************************/
typedef struct BRAP_DSPCHN_P_SampleRateChangeInfo
{
	bool 					bFsChanged; /* true = Change in sample rate,
											false = Sample rate not changed*/
	BRAP_DSPCHN_SampleRateChangeInfo	sSampleRateChangeInfo;
										/* Sample rate change information */
} BRAP_DSPCHN_P_SampleRateChangeInfo;

/***************************************************************************
Summary:
	This structure contains Stream Information private to RAP PI.
***************************************************************************/
typedef struct BRAP_DSPCHN_P_StreamInfo
{
	BRAP_DSPCHN_P_SampleRateChangeInfo sPrivateFsChangeInfo; /* RAP private
											sample rate change structure */
	BRAP_DSPCHN_StreamInfo	sStreamInfo; /* Stream information */
} BRAP_DSPCHN_P_StreamInfo;

/***************************************************************************
Summary:
	This enumeration defines current state of the DSP channel 
***************************************************************************/
typedef enum BRAP_DSPCHN_P_ChState
{
	BRAP_DSPCHN_P_ChState_eStart,
	BRAP_DSPCHN_P_ChState_eStop
}BRAP_DSPCHN_P_ChState;

typedef struct BRAP_DSPCHN_P_PtsBasedStartStop
{
#if (BRAP_NEW_TIMING_MARKER==0)
	uint32_t	ui32NumPtsPairProgrammed;
	uint32_t	ui32StartPts0;
	uint32_t 	ui32StopPts0;
	uint32_t	ui32StartPts1;
	uint32_t 	ui32StopPts1;
#else
    BRAP_DSPCHN_FwTimingMarker      sTimingMarker;
    BRAP_DSPCHN_HostTimingMarker    sHostTimingmarker;
#endif
} BRAP_DSPCHN_P_PtsBasedStartStop;

#if ((BRAP_7405_FAMILY == 1) || (BRAP_3548_FAMILY == 1) )
/***************************************************************************
Summary:
	Structure for Setting the post process stages in a post processing branch
    Note: This structure is exposed through RAP PI for 3563.	
****************************************************************************/

typedef struct BRAP_PpStageSettings
{
	BRAP_ProcessingType	ePpAlgo[BRAP_MAX_PP_PER_BRANCH_SUPPORTED];
					/* Stage wise post processing 
					   algorithms. If no post
					   processing required for a
					   stage, then corresponding
 					   entry should be set to
				 	   BRAP_ProcessingType_eMax */
} BRAP_PpStageSettings;

typedef struct BRAP_DSPCHN_DdbmConfigParams
{
    BRAP_OutputMode        			eAcmod;
	BAVC_AudioSamplingRate			eInputSamplingRate;
    BRAP_OutputMode        			eOpMode;
    bool                    		bInLfeOn;
    bool                    		bOutLfeOn;
	BRAP_DSPCHN_Ddbm_FilterCutOff 	eFilterCutOff;	
	BRAP_DSPCHN_AudioType 			eAlgoType; /*if user doesn't know primary
										algo id, pass BRAP_DSPCHN_Algo_eMax.
										Else pass a valid value. */
	bool							bInterleaved;
	bool							bBmEnable;										
    BRAP_DSPCHN_DdbmMode    		eDdbmMode;

    bool                            bEnableUserGainSettings;
                                            /* if True User gain Setting is applied
                                               if False, user gains not applied.*/
    unsigned int                    uiConfig1Gain[BRAP_RM_P_MAX_OP_CHANNELS];
                                            /* 
                                                ui32Config1Gain[0] = front left
                                                ui32Config1Gain[1] = center
                                                ui32Config1Gain[2] = front right
                                                ui32Config1Gain[3] = surround left
                                                ui32Config1Gain[4] = surround right
                                                ui32Config1Gain[5] = back left
                                                ui32Config1Gain[6] = back right
                                                ui32Config1Gain[7] = LFE
                                            */
    unsigned int                    uiConfig2aGain[6];
                                            /*
                                                ui32Config2aGain[0] = center
                                                ui32Config2aGain[1] = surround left
                                                ui32Config2aGain[2] = surround right
                                                ui32Config2aGain[3] = back left
                                                ui32Config2aGain[4] = back right
                                                ui32Config2aGain[5] = LFE
                                            */
    unsigned int                    uiConfig2bGain[6];
                                            /*
                                                ui32Config2bGain[0] = center
                                                ui32Config2bGain[1] = back left to front
                                                ui32Config2bGain[2] = back right to front
                                                ui32Config2bGain[3] = back left to surround
                                                ui32Config2bGain[4] = back right to surround
                                                ui32Config2bGain[5] = LFE 
                                            */
}BRAP_DSPCHN_DdbmConfigParams;

typedef struct BRAP_DSPCHN_DtsNeoConfigParams
{
    BRAP_OutputMode                 eOpMode;
    bool                            bLfeOn;
    BRAP_DSPCHN_FidelitySubBands    eFidelitySubBands;
    BRAP_DSPCHN_CinemaMode          eCinemaMode;
    BAVC_AudioSamplingRate          eSamplingRate;
	unsigned int	                uiCenterGain;
                                    /* Center gain. Input value range = 0% to 100%. 
                                       100% corresponds to value of unity. Any 
                                       value above 100% is considered as unity */

}BRAP_DSPCHN_DtsNeoConfigParams;

typedef struct BRAP_PP_ConfigParams
{	
	unsigned int				uiPpBranchId;	
	unsigned int				uiPpStage;	
	BRAP_ProcessingType	ePpAlgo;	
	union	
	{		
		BRAP_DSPCHN_TruSurroundXtParams		    sTruSurroundXtParams;	    /* TruSurroundXT config parameters */		
		BRAP_DSPCHN_AVLConfigParams		        sAvlConfigParams;		    /* Auto Volume Level Config params */		
		BRAP_DSPCHN_ProLogicllConfigParams	    sProLogicllConfigParams;    /* ProLogic-II config params */	
		BRAP_DSPCHN_DdbmConfigParams            sDdbmConfigParams;          /* DDBM config params */
		BRAP_DSPCHN_DtsNeoConfigParams          sDtsNeoConfigParams;        /* DDBM config params */
		BRAP_DSPCHN_XenConfigParams		        sXenConfigParams;           /* XEN config params */
		BRAP_DSPCHN_BbeConfigParams		        sBbeConfigParams;	        /* BBE config params */
        BRAP_DSPCHN_CustomSurroundConfigParams  sCustomSurroundConfigParams;/* CUSTOM SURROUND config params */
        BRAP_DSPCHN_CustomBassConfigParams      sCustomBassConfigParams;    /* CUSTOM BASS config params */        
        BRAP_DSPCHN_CustomVoiceConfigParams     sCustomVoiceConfigParams;   /* CUSTOM VOICE config params */                
        BRAP_DSPCHN_PeqConfigParams             sPeqConfigParams;           /* PEQ config params */                        
	} uConfigParams;
} BRAP_PP_ConfigParams;

#endif 

typedef struct BRAP_DSPCHN_P_PpBranchInfo
{
	unsigned int			uiParentId;	
	unsigned int			uiForkPt;	
    BRAP_OutputPort         eOutputPort;
	BRAP_PpStageSettings	sPpStgSettings;
} BRAP_DSPCHN_P_PpBranchInfo;



/*************************************************************************
Summary:
        Structure for describing task information.
Description:
        This structure describes the firmware task information.
***************************************************************************/
typedef struct BRAP_DSPCHN_P_FirmwareTaskInfo
{
	BRAP_FWIF_P_FwTaskHandle		hFwTask;
	BRAP_P_AudioProcNetwork	sProcessingNw;
	BRAP_FWIF_P_TaskInterface	sInputInterface;
	BRAP_FWIF_P_TaskInterface	sOutputInterface;
} BRAP_DSPCHN_P_FirmwareTaskInfo;

/***************************************************************************
Summary:
	Handle structure for DSP context.
***************************************************************************/
typedef struct BRAP_DSPCHN_P_Channel
{
	uint32_t				channelIndex;			/* DSP context number */
	BRAP_DSPCHN_P_Settings	sSettings;				/* setting that is public */

	BCHP_Handle				hChip;					/* handle to chip object */
	BREG_Handle				hRegister;				/* handle to register object */
	BMEM_Handle				hHeap;					/* handle to memory object */
	BINT_Handle				hInt;					/* handle to interrupt object */

    bool bChSpecificDspCh;   /* TRUE = DSPCHN is channel specific,
                                FALSE = DSPCHN belongs to association */
    union
    {
        BRAP_ChannelHandle hRapCh; /* Valid if DSPCHN is channel specific, bChSpecificDspCh = true */
        BRAP_AssociatedChannelHandle hAssociation; /* Valid if DSPCHN is shared between channels, 
                                                              bChSpecificDspCh = false */
    } uHandle;
	uint32_t				chCfgRegOffset;			/* channel configuration reg offset */
	uint32_t				chEsrSiRegOffset;	/* ESR SI register offset for the channel */
	uint32_t				chEsrSoRegOffset;	/* ESR SO register offset for the channel */
	BRAP_DSPCHN_P_AudioParams		sDspAudioParams;	/* Audio parameters for the channel */
	BRAP_DSPCHN_P_ChState			eChState;		/* Channel state */
	bool					bFirstPtsPairProgrammed;
	BRAP_DSPCHN_P_PtsBasedStartStop	sPtsBasedStartStop;	
#if ( (BRAP_3548_FAMILY == 1) )
    unsigned int            uiPPBranchId; 
	BRAP_DSPCHN_P_AudioParams		sDspAudioParamsforPPonSPDIFInput;	/* Audio parameters for the channel */
#endif
	BRAP_DSPCHN_P_FirmwareTaskInfo	sFwTaskInfo[BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN];
    BRAP_P_NetworkInfo      sNetworkInfo;
}BRAP_DSPCHN_P_Channel;

typedef struct BRAP_DSPCHN_P_InterChanDelay
{
    int iChDelay[BRAP_RM_P_MAX_OP_CHANNELS];
    unsigned int    uiChannelIdWithMaxDelay; /* 0 for L, 1 for R, etc.. */
}BRAP_DSPCHN_P_InterChanDelay;

/***************************************************************************
Summary:
	This enumeration defines various firmwares type that needs to be 
	downloaded.
***************************************************************************/
typedef enum BRAP_DSP_P_FwType
{
	BRAP_DSP_P_FwType_eFs,
	BRAP_DSP_P_FwType_eDec,
	BRAP_DSP_P_FwType_ePp,
	BRAP_DSP_P_FwType_ePt
}BRAP_DSP_P_FwType;
#define BRAP_DSP_P_MAX_FWTYPE 	(BRAP_DSP_P_FwType_ePt+1)
	
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
   );

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
	);

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
	BRAP_DSP_Handle handle /* [in] DSP handle */
	);

/***************************************************************************
Summary: Gets the watchdog recovery flag 

Description:
	This function returns current status of watchdog recovery flag. This
	is required to know whether a function is getting called in watchdog
	context or from application.

Returns:
	Watchdog flag

See Also:
**************************************************************************/
bool
BRAP_DSP_P_GetWatchdogRecoveryFlag(BRAP_DSP_Handle hDsp);

/***************************************************************************
Summary:
	Gets DSP context default settings

Description:
	The default configuration for DSP context (DSPCHN channel) are returned
	
Returns:
    BERR_SUCCESS                - if successful 

See Also:
	BRAP_DSPCHN_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_GetDefaultSettings( 
   BRAP_DSPCHN_P_Settings	*psDefSettings	/* [out] The DEC channel default configuration setting */
   );

/***************************************************************************
Summary:
	Gets DSP context current settings

Description:
	The current configuration for DSP context (DSPCHN channel) are returned
	
Returns:
    BERR_SUCCESS                - if successful 

See Also:
	BRAP_DSPCHN_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_GetCurrentSettings(
	BRAP_DSPCHN_Handle		hDspCh,			/* [in] DSP channel handle */
   BRAP_DSPCHN_P_Settings	*psDefSettings	/* [out] The DEC channel default configuration setting */
   );

/***************************************************************************
Summary:
	Gets DSP context default params 

Description:
	The default parameters for DSP context (DSPCHN channel) are returned
	
Returns:
    BERR_SUCCESS                - if successful 

See Also:
	BRAP_DSPCHN_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_GetDefaultParams( 
   BRAP_DSPCHN_P_AudioParams	*psDefParams	/* [out] The DEC channel default parameters */
   );

/***************************************************************************
Summary: Opens a DSP context 

Description:
	This function is responsible for opening a DSP context (DSPCHN channel). 
	When a DSP context is opened, it will create a module channel handle 
	and configure the module based on the channel settings. 
	Once a channel is opened, it must be closed before it can be opened 
	again.

Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Close,
	BRAP_DSPCHN_P_GetDefaultSettings
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_Open(
	BRAP_DSPCHN_Handle		*phDspChn,		/* [out] DSP channel handle */
	void	                *pHandle,           /* [in] Raptor Channel or Association handle */	
    bool                    bChSpecificDspCh,   /* [in] DSP channel handle is Raptor Channel 
                                                        or Association specific */	
	const BRAP_DSPCHN_P_Settings	*pDefSettings	/* [in] The DSP channel configuration setting */
	);

/***************************************************************************
Summary: Closes an DSP context

Description:
	This function is responsible for closing a DSP context and releasing
	all the resources associated with this context.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Open
**************************************************************************/
BERR_Code 
BRAP_DSPCHN_P_Close( 
	BRAP_DSPCHN_Handle hDspChn /* [in] DSP handle */
	);

/***************************************************************************
Summary: This is a dummy start for a DSP Context

Description:
	This function is responsible for copying the DSP Channel parameters from
	Audio params. It does not do the actual DSP channel start.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Start
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_DummyStart(
	BRAP_DSPCHN_Handle	pDspChn,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	);
	
/***************************************************************************
Summary: Starts a DSP context

Description:
	This function is responsible for starting a DSP context for the given
	input decode mode.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Stop
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_Start(
	BRAP_DSPCHN_Handle	pDspChn,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	);
	
/***************************************************************************
Summary: Creates DSP Out Config in NetWorkInfo

Description:
	This function is responsible for creating the whole DSP Output Config.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Start
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_CreateDstConfig(
	BRAP_DSPCHN_Handle	hDspCh,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	);
	

/***************************************************************************
Summary: Stops a DSP context

Description:
	This function stops a currently running context
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Start
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_Stop(
	BRAP_DSPCHN_Handle	pDspChn					/* [in] DSPCHN handle */
	);

/***************************************************************************
Summary:
    Private Structre BRAP_P_IOBufferDetails holds Input/Output Buffer details 
    i.e., FMM Ringbuffer Ids or DRAM Buffer pointers. The buffers of interest 
    here carry the outputs of Decode/PB/PCM capture channel, fed to the the 
    Association network(currently FW Mixer task).
**************************************************************************/
typedef enum BRAP_P_IOBufferType
{
    BRAP_P_IOBufferType_eFMM,
    BRAP_P_IOBufferType_eDRAM,
    BRAP_P_IOBufferType_eMax
}BRAP_P_IOBufferType;

typedef struct BRAP_P_CircularBuffer
{
	uint32_t	ui32BaseAddr;		/*	Circular buffer's base address */
	uint32_t	ui32EndAddr;		/*	Circular buffer's End address */
	uint32_t	ui32ReadAddr;		/*	Circular buffer's read address */
	uint32_t	ui32WriteAddr;		/*	Circular buffer's write address */
	uint32_t	ui32WrapAddr;		/*	Circular buffer's wrap address */
}BRAP_P_CircularBuffer;

typedef struct BRAP_P_IODRAMBuffer
{
    unsigned int            uiNumBuffers;
    BRAP_P_CircularBuffer   sIOCircularBuf[BRAP_AF_P_MAX_CHANNELS];
    BRAP_P_CircularBuffer   sIOGenericCircularBuf;    
}BRAP_P_IODRAMBuffer;

typedef struct BRAP_P_IOBufferID
{
    unsigned int            uiNumBuffers;
    uint32_t                ui32RbufId[BRAP_RM_P_MAX_OP_CHANNELS];
}BRAP_P_IOBufferID;

typedef struct BRAP_P_IOBufferDetails
{
    BRAP_P_IOBufferType     eBufferType;
    union
    {
        BRAP_P_IODRAMBuffer     sIOBuffer;
        BRAP_P_IOBufferID       sIOBufId;        
    }uBufferDetails;
}BRAP_P_IOBufferDetails;
/**************************************************************************/

#ifdef RAP_MULTISTREAM_DECODER_SUPPORT

/***************************************************************************
Summary: Dynamically add an input to a task

Description:
	This function enables successive inputs of a running task. 

	In MS11 case FW Mixer being a task takes multiple inputs and any input 
	addition need to be seamless. So the DSP Channel(inc CIT and Task) need not 
	be restarted, instead the below API is called which takse the address of the 
	CIT of FW Mixer and the details of the input to be enabled, and also the 
	address of the DRAM where the updated CIT is to be stored. This updated CIT 
	address along with the current FW mixer CIT address need to be passed to FW 
	through command interface for FW to switch to the new CIT seamlessly.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Start
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_AddRemoveInputToTask(
	BRAP_ChannelHandle 	    hRapCh,		        /* [in] RAP Channel handle */
	BRAP_DSPCHN_Handle	    hDspCh,             /* [in] DSPCHN handle */
    bool                    bAddInput ,
    BRAP_P_IOBufferDetails  *psIOBuffer
	);

#endif

BERR_Code BRAP_DSPCHN_P_StopVideo(
	BRAP_DSPCHN_Handle	pDspChn					/* [in] DSPCHN handle */
	);


void
BRAP_DSPCHN_P_TsmLog_isr(
	BRAP_DSPCHN_Handle	hDspCh,			/* [in] DSP channel handle */
	BRAP_DSPCHN_TsmLogInfo	*psTsmLogInfo);	/* [out] Information about logged TSM data */

BERR_Code
BRAP_DSPCHN_P_EnablePause(
						BRAP_DSPCHN_Handle      hDspCh,	/* [in] DSPCHN handle */
						bool					bOnOff		/* [In] TRUE = Pause video
																FALSE = Resume video */
						);
BERR_Code
BRAP_DSPCHN_P_GetOneAudioFrameTime(
						BRAP_DSPCHN_Handle      hDspChn,			/* [in] DSPCHN handle */
						unsigned int			*puiOneFrameTime	/* [out] One Audio Frame Time */
						);

BERR_Code
BRAP_DSPCHN_P_FrameAdvance(
						BRAP_DSPCHN_Handle      hDspCh,			/* [in] DSPCHN handle */
						unsigned int			uiFrameAdvTime				/* [In] Frame advance time in msec */
						);

void BRAP_DSP_P_GetFwVersionInfo(
		BRAP_DSP_Handle hDsp,
		BRAP_DSPVersionInfo	*psDSPVersion
		);

/* BRAP_DSP_P_InitInterframeBuffer: This function initializes interframe buffer as per the 
 * input audio type and programs corresponding entry in MIT. */
BERR_Code BRAP_DSP_P_InitInterframeBuffer(
				BRAP_FWIF_P_FwTaskHandle	hTask);

/* BRAP_DSP_P_InitInterframeBuffer: This function initializes status buffer  */
BERR_Code BRAP_DSP_P_InitStatusBuffer(
				BRAP_FWIF_P_FwTaskHandle	hTask);


BERR_Code BRAP_DSPCHN_P_GetCurrentAudioParams (
						BRAP_DSPCHN_Handle	hDspCh,
						BRAP_DSPCHN_P_AudioParams	*psDspChAudioParams
						);

bool BRAP_DSPCHN_P_GetDecoderPauseState (
						BRAP_DSPCHN_Handle	hDspCh
						);

BERR_Code
BRAP_DSPCHN_P_SetConfig (
	BRAP_DSPCHN_Handle	hDspCh,
	BRAP_DEC_DownmixPath	eDownmixPath,
	BRAP_DSPCHN_AudioType eType
	);

BERR_Code BRAP_DSP_P_EnableDspWatchdogTimer (
						BRAP_DSP_Handle		hDsp,
						bool				bEnable
						);

void BRAP_DSP_P_WatchDogResetHardware(BRAP_DSP_Handle hDsp);



/***************************************************************************
Summary:
    This private routine returns the DSPCH handle associated with the
    decode channel handle.
**************************************************************************/
BRAP_DSPCHN_Handle 
BRAP_DSPCHN_P_GetDspChHandle(
	void	                *pHandle,       /* [in] Raptor Channel or Association handle */
	bool                    bChSpecificDspCh	
	);

/***************************************************************************
Summary:
    Isr version of BRAP_DSPCHN_P_GetDspChHandle
**************************************************************************/
#define BRAP_DSPCHN_P_GetDspChHandle_isr(pHandle, bChSpecific) \
     BRAP_DSPCHN_P_GetDspChHandle(pHandle, bChSpecific)

    
void BRAP_DSPCHN_P_AddWordToFWBuffer(
					unsigned char *bufPtr,
					uint32_t data,
					uint32_t nBytes);

/***************************************************************************
Summary:
    This Privte function calls a CIT function that updates a DRAM memory provided
    by PI with the updated FMM Port Config and this address needs to be given to
    FW through FMM Port Config command to reconfigure the output/capture ports accordingly
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_FMMPortReconfig (
    BRAP_Handle             hRap,
	BRAP_ChannelHandle      hRapCh,
    BRAP_DstDetails         *psExtDstDetails,
	bool                    bPortEnable
	);

/***************************************************************************
Summary:
    Destroys audio processing stage.

Description:
    This function free up the processing Handle memory.This function will not free's the memory 
    which is allocated in BRAP_CreateProcessingStage.To free the memory which is allocated in 
    create stage,we must call the BRAP_DestroyProcessingStage.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_CreateProcessingStage,BRAP_DestroyProcessingStage
    
**************************************************************************/
BERR_Code BRAP_P_DestroyProcessingStage(
    BRAP_ProcessingStageHandle   hAudioProcessingStageHandle  
                                                                        /* [in] Handle for the audio processing
                                                                            stage */
);

/********* New Changes***************/

#define BRAP_DSP_P_MAX_AUD_PROCESSING_CTX 6
#define BRAP_DSP_P_MAX_FS_CTXT  2
#if (BRAP_7405_FAMILY ==1)   
#define BRAP_DSP_P_MAX_DECODE_CTXT 2
#else
#define BRAP_DSP_P_MAX_DECODE_CTXT 4
#endif
#define BRAP_DSP_P_MAX_PASSTHRU_CTXT 1

#define BRAP_DSP_P_MAX_ENCODE_CTXT 2

#ifdef RAP_REALVIDEO_SUPPORT
#define BRAP_DSP_P_MAX_VIDEODECODE_CTXT 1
#endif

#ifdef RAP_GFX_SUPPORT
#define BRAP_DSP_P_MAX_GFX_CTXT 1
#endif
#ifdef RAP_SCM_SUPPORT
#define BRAP_DSP_P_MAX_SCM_CTXT 1
#endif

#define	BRAP_DSP_P_MAX_TSM_FW_TYPE BRAP_FWIF_P_MAX_TSM_FW_TYPE



/***************************************************************************
Summary:
	This structure contains the Buffer Properties.
***************************************************************************/
typedef struct BRAP_DSP_P_CtxtFwBufInfo
{
	uint32_t				ui32BaseAddr;
	uint32_t				ui32Size;
	union
	{
		BRAP_DSPCHN_AudioType				eAlgoType;
		BRAP_ProcessingType 			ePAlgoType;
#ifdef RAP_REALVIDEO_SUPPORT
		BRAP_DSPCHN_VideoType				eVideoAlgoType;
#endif
#ifdef RAP_GFX_SUPPORT        
		BRAP_DSPCHN_GfxType				eGfxAlgoType;        
#endif
#ifdef RAP_SCM_SUPPORT
		BRAP_DSPCHN_ScmType				eScmAlgoType;
#endif
	}uExecAlgoType;
	
	int32_t				numUser;
}BRAP_DSP_P_CtxtFwBufInfo;


/*******************************************************************************

Note: In case of OpenTimeDownload, The memory allocation for following should 
be consecutive and in the same order as it is mentioned in this datstructure.

*******************************************************************************/
typedef struct BRAP_DSP_DwnldMemInfo
{
	uint32_t					ui32SystemRdbVariablesPtr; /*Ptr to buffer  for System RDB Variables*/
	uint32_t					ui32SystemRdbVariablesSize; /*Size of buffer  for System RDB Variables*/    
	uint32_t					ui32SystemIbootCodePtr; /*Ptr to buffer  for System Iboot Code*/
	uint32_t					ui32SystemIbootCodeSize; /*size of buffer  for System Iboot Code*/

	uint32_t					ui32SystemCodePtr;/*Ptr to buffer for system
													resident code*/
	uint32_t					ui32SystemCodeSize;/*size of buffer for system
													resident code*/
	uint32_t					ui32SystemDataPtr;/*Ptr to buffer for system
													resident code*/
	uint32_t					ui32SystemDataSize;/*size of buffer for system
													resident code*/
#ifdef RAP_SCM_SUPPORT														
	uint32_t					ui32SystemTableOfHashesPtr;/*Ptr to buffer for system
													table of hashes*/
	uint32_t					ui32SystemTableOfHashesSize;/*size of buffer for system
													table of hashes*/
#endif														
														

	uint32_t					ui32TsmCodePtr;
	uint32_t					ui32TsmCodeSize;													
						/*This will have the information regarding the ptr and 
						size of buffer allocated to download all the Tsm Exec.*/			

	uint32_t					ui32SystemTaskCodePtr;
	uint32_t					ui32SystemTaskCodeSize;													
						/*This will have the information regarding the ptr and 
						size of buffer allocated to download all the System Task code.*/		                        

	BRAP_DSP_P_CtxtFwBufInfo	sProcessingAlgoBufInfo[BRAP_DSP_P_MAX_AUD_PROCESSING_CTX];
						/*This will have the information regarding the Buffer 
						for Processing Algorithm.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						BRAP_FWIF_P_MAX_PROCESSING_CTX = Max no. of Processing 
						algorithm which can simultaneously execute at a time in 
						the DSP*/

	BRAP_DSP_P_CtxtFwBufInfo	sFsBufInfo[BRAP_DSP_P_MAX_FS_CTXT]; 
						/*This will have the information regarding the all the 
						buffers for Frame Sync.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the Buf for downloading 
						framesync of all the supported algorithm
						BRAP_FWIF_P_MAX_FS_CTXT = Max no. of Framesync Executable
						whose algorithm can simultaneously execute at a time in 
						the DSP*/

	BRAP_DSP_P_CtxtFwBufInfo	sDecCodecBufInfo[BRAP_DSP_P_MAX_DECODE_CTXT];
						/*This will have the information regarding the all the 
						buffers for Decode Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Decode Exec of all the supported algorithm 
						BRAP_FWIF_P_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/

	BRAP_DSP_P_CtxtFwBufInfo	sPtCodecBufInfo[BRAP_DSP_P_MAX_PASSTHRU_CTXT];
						/*This will have the information regarding the all the 
						buffers for Passthru Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Passthru Exec of all the supported algorithm 
						BRAP_FWIF_P_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/

	BRAP_DSP_P_CtxtFwBufInfo	sEncodeCodecBufInfo[BRAP_DSP_P_MAX_ENCODE_CTXT];
						/*This will have the information regarding the all the 
						buffers for Passthru Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Passthru Exec of all the supported algorithm 
						BRAP_FWIF_P_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/
#ifdef RAP_REALVIDEO_SUPPORT
	BRAP_DSP_P_CtxtFwBufInfo	sVideoDecCodecBufInfo[BRAP_DSP_P_MAX_VIDEODECODE_CTXT];
						/*This will have the information regarding the all the 
						buffers for Decode Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Decode Exec of all the supported algorithm 
						BRAP_FWIF_P_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/						

#endif
#ifdef RAP_GFX_SUPPORT
BRAP_DSP_P_CtxtFwBufInfo	sGfxBufInfo[BRAP_DSP_P_MAX_GFX_CTXT];
						/*This will have the information regarding the all the 
						buffers for Passthru Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Passthru Exec of all the supported algorithm 
						BRAP_FWIF_P_MAX_CODEC_CTXT = Max no. of Decode
						executable which can simultaneously execute at a time in 
						the DSP*/                        
#endif						
#ifdef RAP_SCM_SUPPORT
BRAP_DSP_P_CtxtFwBufInfo	sScmBufInfo[BRAP_DSP_P_MAX_SCM_CTXT];
						/*This will have the information regarding the all the 
						buffers for SCM Exec.
						If the OpenTimeDownload is True, The the 0th Index will
						contain the information of the buffer for downloading 
						Passthru Exec of all the supported algorithm 
						BRAP_DSP_P_MAX_SCM_CTXT = Max no. of SCM
						executable which can simultaneously execute at a time in 
						the DSP*/                        
#endif						

}BRAP_DSP_DwnldMemInfo;


/***************************************************************************
Summary:
	This structure is one to one mapping between Exec_Image_Id and the Exec_Id 
	(BAF Ids) common between PI and FW. This will also contain the sizes of the 
	Exec_image. 
		Since for the Exec_Image_Id of Decode table, there is no Exec_Id, so it will store 
	the if its already downloaded.

	Also ui32InterframeImgId stores the Image id of interframe Fw image for each ExecId.
***************************************************************************/
typedef struct BRAP_ImgIdMappingArrays
{
	uint32_t ui32CodeImgId[BRAP_AF_P_AlgoId_eMax];
	uint32_t ui32CodeSize[BRAP_AF_P_AlgoId_eMax];
	uint32_t ui32TableImgId[BRAP_AF_P_AlgoId_eMax];	
	uint32_t ui32TableSize[BRAP_AF_P_AlgoId_eMax];	
	uint32_t ui32InterframeImgId[BRAP_AF_P_AlgoId_eMax];
	uint32_t	ui32InterframeSize[BRAP_AF_P_AlgoId_eMax];
} BRAP_ImgIdMappingArrays;

void BRAP_MapImgIdWithExecId(BRAP_Handle hRap,BRAP_ImgIdMappingArrays *psImgIdMappingArrays);
void BRAP_UpdateMitEntry(
		BRAP_Handle		hRap,
		BRAP_AF_P_AlgoId	eExecId,
		uint32_t		ui32CodePtr,
		uint32_t 		ui32CodeSize,
		uint32_t		ui32TblPtr,		
		uint32_t		ui32TblSize				
		);

void BRAP_InitializeMit(BRAP_Handle	hRap);
void BRAP_PrintMit(BRAP_Handle	hRap);

BERR_Code BRAP_DSP_GetDownloadMemRequirement(
		BRAP_Handle	 hRap,
		BRAP_DSP_DwnldMemInfo *pDwnldMemInfo,
		uint32_t	*pui32Size);

BERR_Code BRAP_DSP_P_InitHardware(BRAP_DSP_Handle hDsp, bool bFwAuthEnabled);

/***************************************************************************
Summary: Enable/Disable Time Stamp Management

Description:
	This function enables/disables Time Stamp Management for a given audio
	channel. 
	Note: This API is valid only for a decode channel.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_SetTSMDiscardThreshold
**************************************************************************/
BERR_Code 
BRAP_GetTsmNodeInfo(
    BRAP_ChannelHandle	            hRapCh,             /*[In] Rap channel handle */
	BRAP_DSPCHN_Handle              hDspCh,             /*[In] Dsp CHannel handle */
    unsigned int                    *pConfigBufAddr,    /*[Out] Config buffer address */
    BRAP_FWIF_P_FwTaskHandle        *hFwTask,       /*{out] task handle */    
    BRAP_FWIF_P_TsmConfigParams     **psTsmConfigParam /*[out] Tsm config param*/   
);

#define BRAP_GetTsmNodeInfo_isr(hRapCh,hDspCh,pConfigBufAddr,hFwTask,psTsmConfigParam) \
    BRAP_GetTsmNodeInfo(hRapCh,hDspCh,pConfigBufAddr,hFwTask,psTsmConfigParam)


/**************************************/
BERR_Code BRAP_DSP_AllocMem(BRAP_Handle hRap);



BERR_Code BRAP_DSP_FreeMem(BRAP_Handle hRap);


 BERR_Code
BRAP_DSPCHN_P_CreateFwTask(
        BRAP_DSPCHN_Handle hDspCh,
        BRAP_P_AudioProcNetwork     *psAudProcNetwork
        );

 BERR_Code BRAP_DSPCHN_P_AllocateFwCtxtBuf(
	BRAP_Handle hRap,
	uint32_t eAudType,	/* For PostProcessing it wiil be BRAP_ProcessingType. For Decode it will be audio type*/
	uint32_t *pui32FwCtxIndx,
	bool *bDownload,
	BRAP_DSP_ExecType eExecType,
	bool bWatchDogRecovery);

BERR_Code BRAP_DSPCHN_P_FreeFwCtxtBuf(
	BRAP_Handle hRap,
	uint32_t eAudType,	/* For PostProcessing it wiil be BRAP_ProcessingType. For Decode it will be audtio type*/
	BRAP_DSP_ExecType eExecType);

/***************************************************************************
Summary: Starts a DSP context

Description:
	This function is responsible for starting a DSP context for the given
	input decode mode.
	
Returns:
	BERR_SUCCESS - If successful

See Also:
	BRAP_DSPCHN_P_Stop
**************************************************************************/
BERR_Code
BRAP_DSPCHN_P_StartVideo(
	BRAP_DSPCHN_Handle	hDspCh,					/* [in] DSPCHN handle */
	const BRAP_DSPCHN_P_AudioParams *psAudioParams	/* [in] Audio decode parameters */
	);


#ifdef __cplusplus
}
#endif

#endif /* BRAP_DSP_PRIV_H__ */

/* End of File */
