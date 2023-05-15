/***************************************************************************
 *     Copyright (c) 1999-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brap_img.c $
 * $brcm_Revision: Hydra_Software_Devel/133 $
 * $brcm_Date: 5/10/10 5:38p $
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/rap/base/brap_img.c $
 * 
 * Hydra_Software_Devel/133   5/10/10 5:38p srajapur
 * SW7400-2688 : [7400] Added Support for BRAP_DSPCHN_AudioType_ePcmWav
 * algorithm
 * 
 * Hydra_Software_Devel/132   12/16/09 5:36a srajapur
 * SW7400-2602: [7400]Support DSOLA PB80 / PB120 on BCM7400. PI support
 * for DSOLA.
 * 
 * Hydra_Software_Devel/131   10/27/09 10:25p srajapur
 * SW7400-2166 : [7400] Static noise heard while decoding AACHE LOAS
 * streams
 * 
 * Hydra_Software_Devel/130   7/29/09 5:28p srajapur
 * PR53240 : [7403] DRA decoder development on 7403 chip
 * 
 * Hydra_Software_Devel/129   7/21/09 6:18p srajapur
 * PR53240 : [7403] DRA decoder development on 7403 chip
 * 
 * Hydra_Software_Devel/128   6/12/09 6:02p srajapur
 * PR 53489:[7401,7400] Added support for SRS True Volume certification
 * and added PCM decode algorithm
 * 
 * Hydra_Software_Devel/127   4/6/09 4:50p srajapur
 * PR 53700 : [7400,7401,7403,7118] Added Downloadable scheduler code for
 * code memory optimization
 * 
 * Hydra_Software_Devel/126   3/18/09 8:38p srajapur
 * PR53363 : [7400] Adding DTS passthrough support
 * 
 * Hydra_Software_Devel/125   3/18/09 7:57p srajapur
 * PR43833 : [7400]  BCM7400 support codec for AAC HE V2
 * 
 * Hydra_Software_Devel/124   2/25/09 6:18p srajapur
 * PR52480: [7400] Name changing SRS VIQ to SRS TrueVolume
 * 
 * Hydra_Software_Devel/123   1/12/09 11:06p srajapur
 * PR50613 :[7400] Added SRS VIQ post processing algorithm for 7400
 * 
 * Hydra_Software_Devel/122   11/24/08 6:25p srajapur
 * PR42131 : [7118,7403] Added support for AACHEV2
 * 
 * Hydra_Software_Devel/121   11/12/08 7:30p srajapur
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
 * Hydra_Software_Devel/120   10/3/08 2:25p bhanus
 * PR 45762:[7440]Adding Multiple tables for AAC HE stages.
 * 
 * Hydra_Software_Devel/119   9/29/08 2:50p bhanus
 * PR 45762:[7440]Adding AAC ADTS Support
 * 
 * Hydra_Software_Devel/118   9/19/08 5:03p bhanus
 * PR 46417 : [7440] Fixing the typo.
 * 
 * Hydra_Software_Devel/117   9/19/08 11:58a bhanus
 * PR 46417 : [7440] Changing the name of FW arrays.
 * 
 * Hydra_Software_Devel/116   9/16/08 3:17p bhanus
 * PR 46417 : [7440] Merging DTS MA Support on main line
 * 
 * Hydra_Software_Devel/RAP_7440_DTSMA_DEVEL/2   9/12/08 7:03p bhanus
 * PR 46417 : [7440] Adding support for Downloadable Scheduler.
 * 
 * Hydra_Software_Devel/RAP_7440_DTSMA_DEVEL/1   9/2/08 11:54a bhanus
 * PR 46417 : [7440] Adding DTS MA Support
 * 
 * Hydra_Software_Devel/115   8/27/08 1:32p bhanus
 * PR 45762:[7440]Removing unused old AAC files.
 * 
 * Hydra_Software_Devel/114   8/27/08 11:51a bhanus
 * PR 45762:[7440]Removing unused old AAC files.
 * 
 * Hydra_Software_Devel/113   8/21/08 5:10p bhanus
 * PR 45762:[7440] Adding Support for AAC HE for 7440.
 * 
 * Hydra_Software_Devel/112   8/18/08 4:40p bhanus
 * PR 45762:[7440] Adding Support for AAC HE for 7440.
 * 
 * Hydra_Software_Devel/111   8/18/08 4:14p srajapur
 * PR 41308 : [7400] Added MPEG Layer 3 LSF(SRC) support needed for DivX
 * 
 * Hydra_Software_Devel/110   8/18/08 4:01p bhanus
 * PR 45762:[7440] Adding Support for AAC HE for 7440.
 * 
 * Hydra_Software_Devel/109   8/4/08 12:31p srajapur
 * PR 41308 : [7400] Removed MPEG Layer 3 LSF support needed for DivX
 * 
 * Hydra_Software_Devel/108   7/31/08 7:51p srajapur
 * PR42131 : [7118] Added support for AACHEV2
 * 
 * Hydra_Software_Devel/107   6/12/08 9:30p srajapur
 * PR 41308 : [7400] MPEG Layer 3 LSF support needed for DivX
 * 
 * Hydra_Software_Devel/106   6/9/08 3:08p srajapur
 * PR43120 : [7400] won't build without RAP_DDP_SUPPORT=y
 * ---> Added condition to check for RAP_DDP_SUPPORT
 * 
 * Hydra_Software_Devel/105   5/23/08 11:52a srajapur
 * PR42245 : [7400] Added 7.1 Channel Pass-Thru Support for DDP on HDMI
 * output
 * 
 * Hydra_Software_Devel/104   5/19/08 10:50p srajapur
 * PR42140 : [7400] Implemented  PPM adjustments as in 7401
 * 
 * Hydra_Software_Devel/103   5/16/08 3:48p srajapur
 * PR42131 : [7401,7403] Added support for AACHEV2
 * 
 * Hydra_Software_Devel/102   5/16/08 12:12p gdata
 * PR42762: [7440] Adding support for DTSHD-NBC algo
 * 
 * Hydra_Software_Devel/101   5/6/08 8:03p srajapur
 * PR42131: [7401,7403] Added support for AACHEV2
 * 
 * Hydra_Software_Devel/100   4/28/08 2:28p srajapur
 * PR42131 : [7401] Added support for AACHEV2
 * 
 * Hydra_Software_Devel/99   4/8/08 11:34a bhanus
 * PR 41469 : [7440] Adding Support for WMA and WMA-Pro for 7440
 * 
 * Hydra_Software_Devel/98   1/17/08 9:19p srajapur
 * PR 36680 : [7400,7401,7403,7118] Removed extra comments
 * 
 * Hydra_Software_Devel/97   1/17/08 6:10p gdata
 * PR38790:  [7440] Adding support for LPCM_A_DVD
 * 
 * Hydra_Software_Devel/96   12/21/07 6:39p gdata
 * PR38296: [7440] Adding the support for DTSHD sub audio
 * 
 * Hydra_Software_Devel/95   12/20/07 11:32a gdata
 * PR38296: [7440] Adding support for DTSHD SUB audio
 * 
 * Hydra_Software_Devel/94   11/12/07 2:16p sushmit
 * PR 36603: [7405] Fixing Wmapro img file linkage
 * 
 * Hydra_Software_Devel/93   11/11/07 7:32p sushmit
 * PR 36003: [7405] Wmapro RDB header update taken care of
 * 
 * Hydra_Software_Devel/92   11/2/07 5:21p srajapur
 * PR 36680 : [7401,7118,7403,7400] Removed ifdef to support  LPCM
 * algorithm
 * 
 * Hydra_Software_Devel/91   11/2/07 12:09p sushmit
 * PR 36605:  [7405] Fixing Wmapro support
 * 
 * Hydra_Software_Devel/90   10/26/07 12:27p sushmit
 * PR 36003: [7405] Updated WMAPRO Support
 * 
 * Hydra_Software_Devel/89   10/25/07 4:22p sushmit
 * PR 36003: [7405] Updated WMAPRO Support
 * 
 * Hydra_Software_Devel/88   10/4/07 2:32p sushmit
 * PR35140: [7325] Develop 7325 Raptor PI for Bringup
 * 
 * Hydra_Software_Devel/87   9/30/07 2:01p sushmit
 * PR35140: [7405] Develop 7325 Raptor PI for Bringup
 * 
 * Hydra_Software_Devel/86   9/19/07 3:29p speter
 * PR 35074: [3563] Adding firmware image IDs for PEQ post processing
 * algorithm
 * 
 * Hydra_Software_Devel/85   9/7/07 3:59p speter
 * PR 34673: [3563] Merging the Phase 2.6 branch to mainline
 * 
 * Hydra_Software_Devel/84   9/5/07 3:16p gdata
 * PR 32827: [7440] Merging DTS-LBR 3-Stage support to
 * Hydra_Software_Devel branch
 * 
 * Hydra_Software_Devel/RAP_7440_DTSLBR_3Stages/1   9/4/07 4:44p gdata
 * PR 32827: [7440] Adding support for 3 stage DTS-LBR
 * 
 * Hydra_Software_Devel/RAP_3563_PH_2_6/1   9/6/07 1:06p speter
 * PR 34460: [3563] Changing code to use 'Custom' for specific Post
 * procesing algorithms
 * 
 * Hydra_Software_Devel/83   8/27/07 2:59p speter
 * PR 34376: [3563] Adding Firmware image IDs for the Custom Voice post
 * processing support
 * 
 * Hydra_Software_Devel/82   8/23/07 4:49p sushmit
 * PR 34135: [7405] Reverting DDP FW file related changes & adding AACHE
 * files.
 * 
 * Hydra_Software_Devel/81   8/22/07 2:05p sushmit
 * PR 34135: [7405] Reverting back changes for DDP till FW is checked in.
 * 
 * Hydra_Software_Devel/80   8/21/07 1:55p speter
 * PR 34202: [3563] Adding Custom Bass post processing Image IDs
 * 
 * Hydra_Software_Devel/79   8/13/07 11:38a speter
 * PR 33985: [3563] Removing unnecessary reference to AC3 files
 * 
 * Hydra_Software_Devel/78   8/10/07 4:43p speter
 * PR 33403: [3563] AAC_HE LOAS/ADTS support merged to main line.
 * 
 * Hydra_Software_Devel/77   8/8/07 2:26p bselva
 * PR 33582:[7400,7401,7118,7403] Stub files are used when ac3 codec is
 * not enabled in compilation
 * 
 * Hydra_Software_Devel/76   8/6/07 4:34p sushmit
 * PR 27646: [7405] Removing bringup workaround of loading AC3 files
 * always, even for DDP.
 * 
 * Hydra_Software_Devel/RAP_3563_AAC_HE_ADTS_LOAS_SUPPORT/1   8/2/07 4:58p speter
 * PR 33403: [3563] AAC SBR multichannel support with 3 executables
 * support added in RAP PI
 * 
 * Hydra_Software_Devel/74   7/30/07 5:49p speter
 * PR 33608: [3563] Adding Image IDs of firmware files for post processing
 * algorithm Custom surround
 * 
 * Hydra_Software_Devel/73   7/19/07 6:38p bselva
 * PR 33176:[7400] Checking in the changes for phase 4.0 release.
 * 
 * Hydra_Software_Devel/72   7/18/07 6:25p gautamk
 * PR33094 : [7440] Adding Support for Passthru mode For Algo DDP7.1, MLp
 * and DTSHD (HBR Mode)
 * 
 * Hydra_Software_Devel/71   7/16/07 5:02p bselva
 * PR 28274:[7403]Adding the support for DVD LPCM
 * 
 * Hydra_Software_Devel/70   7/12/07 10:32a gautamk
 * PR32256: [7401] Adding support for passthru in WMA PRo
 * 
 * Hydra_Software_Devel/69   7/9/07 4:18p gautamk
 * PR32827 :  Adding PI Support for DTS LBR Multichannel
 * 
 * Hydra_Software_Devel/68   7/6/07 12:17p gautamk
 * PR32256: [7401] Adding new algo WMA PRO for 7401
 * 
 * Hydra_Software_Devel/67   7/5/07 4:51p gautamk
 * PR32256: [7401] Adding new algo WMA PRO for 7401
 * 
 * Hydra_Software_Devel/66   7/5/07 4:02p gautamk
 * PR32256: [7401] Adding new algo WMA PRO for 7401
 * 
 * Hydra_Software_Devel/RAP_7401_WMA_PRO/2   6/25/07 11:04a gautamk
 * PR32256: [7401] Adding new algo WMA PRO for 7401
 * 
 * Hydra_Software_Devel/RAP_7401_WMA_PRO/1   6/18/07 12:25p gautamk
 * PR32256: [7401] Adding new algo WMA PRO for 7401
 * 
 * Hydra_Software_Devel/65   6/22/07 11:02a speter
 * PR 27646: [7405] Adding AC3 support to 7405. Kindly note that as of now
 * even if RAP_DDP_SUPPORT=y 7405 picks up the AC3 files as per this
 * checkin. Once we have clarity we need to update this.
 * 
 * Hydra_Software_Devel/64   6/15/07 8:06p gautamk
 * PR30069: [7440] Added DTS-Extension Support.
 * 
 * Hydra_Software_Devel/63   6/15/07 2:17p kagrawal
 * PR 30069: [7440] Merging Phase2.0 branch to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/62   6/14/07 7:27p gautamk
 * PR30069: Changes for new algorithm Mpeg MC
 * 
 * Hydra_Software_Devel/61   6/14/07 4:53p gautamk
 * PR30069: [7440] Adding algorithm DDP 7.1
 * 
 * Hydra_Software_Devel/60   6/13/07 2:54p nitinb
 * PR 30789: Support MPEG2 LSF audio decoding
 * 
 * Hydra_Software_Devel/59   6/12/07 10:06p sushmit
 * PR 27646: Updating 7405 PI
 * 
 * Hydra_Software_Devel/RAP_7440_Phase2.0_Branch/1   6/9/07 5:42p kagrawal
 * PR 30069: [7440] Added support for DTS-Neo PostProcessing
 *
 * Hydra_Software_Devel/58   6/1/07 3:45p bselva
 * PR 26501:[7401 Family] Second round of changes for aache multichannel
 * 
 * Hydra_Software_Devel/57   5/31/07 6:05p bselva
 * PR 26501: [7401 Family]Checking in the changes for AAC-HE multichannel
 * support
 * 
 * Hydra_Software_Devel/56   5/22/07 12:29p rjain
 * PR 28422:[7401 family]  second round of changes for FSPB
 * 
 * Hydra_Software_Devel/55   5/11/07 9:02p sushmit
 * PR 27646: Getting 7405 PI compiled
 * 
 * Hydra_Software_Devel/54   5/7/07 4:36p nitinb
 * PR 28862: Added BBE images
 * 
 * Hydra_Software_Devel/53   4/10/07 5:24p speter
 * PR 28931: Fixing issues for other chips due to the DTS passthru checkin
 * 
 * Hydra_Software_Devel/52   4/10/07 3:17p speter
 * PR 28931: [3563] Adding only DTS passthru support  (Removing decode and
 * table from DTS support in 3563 as they are not required for the case
 * of SPDIF In)
 * 
 * Hydra_Software_Devel/51   4/6/07 6:00p speter
 * PR 28931: [3563] Adding DTS support. Kindly note that only passthru is
 * required for SPDIF In
 * 
 * Hydra_Software_Devel/50   4/4/07 1:34a nitinb
 * PR 28865: Added support for post processing algorithm XEN
 * 
 * Hydra_Software_Devel/49   3/30/07 1:02p kagrawal
 * PR 28561: [7440] Added support for MLP interframe image file
 * 
 * Hydra_Software_Devel/48   3/27/07 3:28p kagrawal
 * PR 28561: [7440] Adding support for DTS-LBR
 * 
 * Hydra_Software_Devel/47   3/22/07 8:59p kagrawal
 * PR 28561: [7440] Adding image file support for MLP & DD lossless
 * 
 * Hydra_Software_Devel/46   3/22/07 7:58p kagrawal
 * PR 28561: [7440] Adding externs for MLP and DD lossless image files
 * 
 * Hydra_Software_Devel/45   3/3/07 7:59p kagrawal
 * PR 27912: [7440] Added support for DDBM interframe and separate
 * scheduler for DSP1
 * 
 * Hydra_Software_Devel/44   3/1/07 11:05a rjain
 * PR 24247: Porting LSF(with SRC) to 7403 and 7118
 * 
 * Hydra_Software_Devel/43   2/27/07 6:25p nitinb
 * PR 26864: Added 3563 firmware files for SRS, PL-II and AVL
 * 
 * Hydra_Software_Devel/42   2/9/07 2:52p rjain
 * PR 24247: changing the array names for SRC
 * 
 * Hydra_Software_Devel/41   2/9/07 1:48p rjain
 * PR 24247: changing name of SRC post processing executables
 * 
 * Hydra_Software_Devel/40   2/8/07 4:27p rjain
 * PR 24247: adding support for SRc as post processing module
 * 
 * Hydra_Software_Devel/39   2/5/07 10:42a kagrawal
 * PR 25607: [7440] Added rule to compile DDP convert fw
 * 
 * Hydra_Software_Devel/38   1/29/07 3:25p bhanus
 * PR 25607: Added Support for Multi-executable DTS/DTS-HD
 * 
 * Hydra_Software_Devel/37   1/24/07 5:31p speter
 * PR 27309: [3563] DDP and AC3 separation
 * 
 * Hydra_Software_Devel/36   1/23/07 4:50p sushmit
 * PR 27078: AC3/DDP change broke 3563, fixing
 * 
 * Hydra_Software_Devel/35   1/23/07 11:02a bselva
 * PR 27078: Checking in the changes for Ac3 separation from DDP
 * 
 * Hydra_Software_Devel/34   1/22/07 7:25p sushmit
 * PR 26864:
 * Added PES Framesync & LPCM_Custom algos.
 * 
 * Hydra_Software_Devel/33   12/29/06 1:10a nitinb
 * PR 26766: Added BTSC firmware files
 * 
 * Hydra_Software_Devel/32   11/3/06 3:17p speter
 * PR 24717: Added support for DTS algorithm in 7440 A0
 * 
 * Hydra_Software_Devel/31   11/2/06 5:20p speter
 * PR 24717: Added support for LPCM algos in 7440.
 * 
 * Hydra_Software_Devel/30   10/16/06 2:56p sushmit
 * PR 24717: Updating for 7440
 * 
 * Hydra_Software_Devel/29   10/3/06 11:10a bselva
 * PR 24468: Checking in the changes required for 7400 phase 3.0 release
 * 
 * Hydra_Software_Devel/29   10/3/06 11:02a bselva
 * PR 24468: Checking in the changes required for 7400 phase 3.0 release
 * 
 * Hydra_Software_Devel/29   10/3/06 11:01a bselva
 * PR 24468: Checking in the changes required for 7400 phase 3.0 release
 * 
 * Hydra_Software_Devel/28   9/25/06 3:13p kagrawal
 * PR 20654: Merged from 7440 bring up branch
 * 
 * Hydra_Software_Devel/27   7/10/06 3:14p sushmit
 * PR18604: SQA Release for 2 Stage MP3 Encoder on 7401.
 * 
 * Hydra_Software_Devel/26   6/30/06 4:19p rjain
 * PR 22417: adding WMA support for 7401 and 7400 to PI
 * 
 * Hydra_Software_Devel/25   6/30/06 12:42p kagrawal
 * PR 22083: Added support for HD-DVD MLP for 7411D0
 * 
 * Hydra_Software_Devel/24   6/15/06 3:43p nitinb
 * PR 22083: Add support for MLP algorithm in 7411D0 RAP PI
 * 
 * Hydra_Software_Devel/23   6/15/06 2:46p nitinb
 * PR 22083: Add support for MLP algorithm in 7411D0 RAP PI
 * 
 * Hydra_Software_Devel/22   6/13/06 2:53p nitinb
 * PR 22083: Add support for MLP algorithm in 7411D0 RAP PI
 * 
 * Hydra_Software_Devel/21   5/24/06 6:25p nitinb
 * PR 21706: Download interframe buffer files for all LPCM algorithms
 * 
 * Hydra_Software_Devel/20   5/24/06 4:13p nitinb
 * PR 21705: Require RAP PI support to download framesync executable
 * depending on DVD audio type
 * 
 * Hydra_Software_Devel/19   5/23/06 5:30p sushmit
 * PR18604: Updating 3 stage MP3 Encoder FW & merged scheduler.
 * 
 * Hydra_Software_Devel/18   5/4/06 11:35a nitinb
 * PR 21400: Add RAP support for DD in lossless for 7411D0
 * 
 * Hydra_Software_Devel/17   5/3/06 11:07p nitinb
 * PR 21105: 7411D0 - Add support WMA Raptor Porting Interface
 * 
 * Hydra_Software_Devel/16   3/28/06 12:10p rnayak
 * PR20439: Restore Bangalore Raptor files after magnum vob corruption on
 * 17th March 2006
 * 
 * Hydra_Software_Devel/17   3/21/06 6:28p sushmit
 * PR18604: Updating review comments.
 * 
 * Hydra_Software_Devel/16   3/20/06 8:10p nitinb
 * PR 20287: Added algorithm DVD LPCM
 * 
 * Hydra_Software_Devel/15   3/16/06 5:21p nitinb
 * PR 20117: Implement dolby digital bass management algorithm
 * 
 * Hydra_Software_Devel/14   2/17/06 9:59a sushmit
 * PR18604: Merging Encoder to latest.
 * 
 * Hydra_Software_Devel/RAP_AUD_ENC/5   2/9/06 10:37a sushmit
 * PR18604: Updating Audio Encoder PI.
 * 
 * Hydra_Software_Devel/RAP_AUD_ENC/4   2/6/06 4:22p sushmit
 * PR18604: Merging latest Decoder PI to Encoder PI branch
 * 
 * Hydra_Software_Devel/RAP_AUD_ENC/2   12/20/05 1:05p sushmit
 * PR18604: Updating with new firmware.
 * 
 * Hydra_Software_Devel/RAP_AUD_ENC/1   12/14/05 5:10p sushmit
 * [PR18604]:Susmit
 * Merging Encoder to latest.
 * SUSMIT
 * 14 DEC 2005
 * 
 * Hydra_Software_Devel/10   12/02/05 11:45a susmit
 * Added Image IDs for Encoder
 * 
 * Hydra_Software_Devel/9   12/2/05 1:17p bselva
 * PR 18312: Added support for LPCM
 * 
 * Hydra_Software_Devel/8   11/29/05 6:03p bselva
 * PR 18312: Added pass through support for DTS
 * 
 * Hydra_Software_Devel/7   11/29/05 4:58p bselva
 * PR 18312: Added for DTS support
 * 
 * Hydra_Software_Devel/6   11/3/05 3:23p rjain
 * PR 16982: Merging Vladimir's image interface related changes to main
 * branch
 * 
 * Hydra_Software_Devel/5   10/23/05 9:57a nitinb
 * PR 17415: Added support for AAC downmix
 * 
 * Hydra_Software_Devel/4   10/18/05 5:18p rjain
 * PR 17651: Changing checks for BCHP_CHIP ==/!= 7401 to check for
 * BCHP_7411_VER instead
 * 
 * Hydra_Software_Devel/3   10/6/05 12:00p nitinb
 * PR 17415 : Adding support for AAC passthru
 * 
 * Hydra_Software_Devel/2   10/2/05 10:09p nitinb
 * PR 17415: Add support for advance codec for Raptor PI
 * 
 * Hydra_Software_Devel/1   9/30/05 6:09p nitinb
 * PR 16982: Audio Raptor PI shall use BIMG interface to access firmware
 * interface
 ***************************************************************************/

#include "bstd.h"
#include "brap_img.h"


BDBG_MODULE(rap_img);

/* External references of firmware binaries */
extern const void * BRAP_IMG_iboot [];

extern const void * BRAP_IMG_scheduler_data [];
extern const void * BRAP_IMG_scheduler_code [];

extern const void * BRAP_IMG_mpeg_framesync [];
extern const void * BRAP_IMG_ac3_framesync [];

extern const void * BRAP_IMG_mpeg_l1_decode [];
extern const void * BRAP_IMG_mpeg_l2_decode [];
extern const void * BRAP_IMG_mp3_decode [];

#if defined ( BCHP_7411_VER ) || (BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403)
extern const void * BRAP_IMG_ac3_decode [];
#endif 

extern const void * BRAP_IMG_mpeg_decode_table [];
extern const void * BRAP_IMG_ac3_decode_table [];

#ifdef BCHP_7411_VER /* For the 7411 */ 
extern const void * BRAP_IMG_ac3_downmix_code [];
extern const void * BRAP_IMG_ac3_downmix_table [];
#endif

extern const void * BRAP_IMG_ac3_passthru [];
extern const void * BRAP_IMG_mpeg_passthru [];

extern const void * BRAP_IMG_mpeg_interframe_buf [];

#if (BRAP_DVD_FAMILY != 1)
extern const void * BRAP_IMG_aac_framesync [];
extern const void * BRAP_IMG_aac_decode [];
extern const void * BRAP_IMG_aac_decode_table []; 
extern const void * BRAP_IMG_aac_interframe_buf [];
extern const void * BRAP_IMG_aac_passthru [];
extern const void * BRAP_IMG_aac_downmix [];
extern const void * BRAP_IMG_aache_decode [];
#endif

extern const void * BRAP_IMG_aache_framesync [];

extern const void * BRAP_IMG_aacplus_decode_table [];
extern const void * BRAP_IMG_aacplus_interframe_buf [];

extern const void * BRAP_IMG_ddp_framesync [];
extern const void * BRAP_IMG_ddp_be_decode [];
extern const void * BRAP_IMG_ddp_fe_decode [];
extern const void * BRAP_IMG_ddp_decode_table [];
extern const void * BRAP_IMG_ddp_interframe_buf [];

extern const void * BRAP_IMG_ddp_passthru [];
extern const void * BRAP_IMG_ddp_passthru_interframe_buf [];
extern const void * BRAP_IMG_ddp_passthru_table [];

#if (BRAP_DVD_FAMILY == 1)
extern const void * BRAP_IMG_wma_std_framesync[];
extern const void * BRAP_IMG_wma_std_stg0_decode[];
extern const void * BRAP_IMG_wma_std_stg0_decode_table[];
extern const void * BRAP_IMG_wma_std_interframe_buf[];
#else
extern const void * BRAP_IMG_wma_std_framesync[];
extern const void * BRAP_IMG_wma_std_stg1_decode[];
extern const void * BRAP_IMG_wma_std_stg1_decode_table[];
extern const void * BRAP_IMG_wma_std_interframe_buf[];

#endif

#if ( BCHP_CHIP == 3563)
extern const void * BRAP_IMG_dts_framesync [];
extern const void * BRAP_IMG_dts_decode [];
extern const void * BRAP_IMG_dts_decode_table [];
extern const void * BRAP_IMG_dts_interframe_buf [];
extern const void * BRAP_IMG_dts_passthru [];
#endif

#if (BRAP_DVD_FAMILY == 1)
extern const void * BRAP_IMG_dts_framesync [];
extern const void * BRAP_IMG_dts_decode [];
extern const void * BRAP_IMG_dts_1_decode [];
extern const void * BRAP_IMG_dts_decode_table [];
extern const void * BRAP_IMG_dts_interframe_buf [];
extern const void * BRAP_IMG_dts_passthru [];

extern const void * BRAP_IMG_dtshd_x96_decode[];
extern const void * BRAP_IMG_dtshd_xll_decode[];
extern const void * BRAP_IMG_dtshd_extras_decode[];

extern const void * BRAP_IMG_dtshd_framesync [];
extern const void * BRAP_IMG_dtshd_hddvd_framesync [];
extern const void * BRAP_IMG_dtshd_passthru [];


extern const void * BRAP_IMG_bdlpcm_framesync [];
extern const void * BRAP_IMG_bdlpcm_decode [];
extern const void * BRAP_IMG_bdlpcm_interframe_buf[];

extern const void * BRAP_IMG_hddvdlpcm_framesync [];
extern const void * BRAP_IMG_hddvdlpcm_decode [];
extern const void * BRAP_IMG_hddvdlpcm_interframe_buf[];

extern const void * BRAP_IMG_dvdlpcm_framesync [];
extern const void * BRAP_IMG_dvdlpcm_decode [];
extern const void * BRAP_IMG_dvdlpcm_interframe_buf[];

extern const void * BRAP_IMG_ddbm_postprocess[];
extern const void * BRAP_IMG_ddbm_postprocess_table[];
extern const void * BRAP_IMG_ddbm_interframe_buf[];

extern const void * BRAP_IMG_dts_neo_postprocess[];
extern const void * BRAP_IMG_dts_neo_postprocess_table[];
extern const void * BRAP_IMG_dts_neo_interframe_buf[];

extern const void * BRAP_IMG_ac3lossless_framesync [];

extern const void * BRAP_IMG_mlp_decode [];
extern const void * BRAP_IMG_mlp_framesync [];
extern const void * BRAP_IMG_mlp_hddvd_framesync [];
extern const void * BRAP_IMG_mlp_decode_table [];
extern const void * BRAP_IMG_mlp_passthru [];

extern const void * BRAP_IMG_scheduler_dsp1_data [];
extern const void * BRAP_IMG_scheduler_dsp1_code [];

extern const void * BRAP_IMG_dtslbr_framesync [];
extern const void * BRAP_IMG_dtslbr0_decode_code [];
extern const void * BRAP_IMG_dtshd_exss_decode_code [];
extern const void * BRAP_IMG_dtslbr1_decode_table [];
extern const void * BRAP_IMG_dtslbr1_decode_interframe_buf [];
extern const void * BRAP_IMG_dtslbr1_decode_code[];
extern const void * BRAP_IMG_mlp_interframe_buf [];

extern const void * BRAP_IMG_ddpdep_interframe_buf [];
extern const void * BRAP_IMG_ddpdep_decode_table [];
extern const void * BRAP_IMG_ddpdep_framesync [];
extern const void * BRAP_IMG_ddpdep_be_decode [];
extern const void * BRAP_IMG_ddpdep_fe_decode [];
extern const void * BRAP_IMG_ddpdep_passthru [];

extern const void * BRAP_IMG_mpeg_mc_framesync [];
extern const void * BRAP_IMG_mpeg_mc_decode [];
extern const void * BRAP_IMG_mpeg_mc_decode_table [];
extern const void * BRAP_IMG_mpeg_mc_passthru [];

extern const void * BRAP_IMG_dtshd_subaudio_framesync [];
extern const void * BRAP_IMG_dtshd_subaudio_1_decode [];
extern const void * BRAP_IMG_dtshd_subaudio_decode [];
extern const void * BRAP_IMG_dtshd_subaudio_decode_table [];
extern const void * BRAP_IMG_dtshd_subaudio_interframe_buf [];

extern const void * BRAP_IMG_dvdalpcm_decode [];
extern const void * BRAP_IMG_dvdlapcm_framesync [];
extern const void * BRAP_IMG_dvdalpcm_interframe [];
#endif

#ifdef BCHP_7411_VER /* For the 7411 */ 
extern const void * BRAP_IMG_dts_framesync [];
extern const void * BRAP_IMG_dts_decode [];
extern const void * BRAP_IMG_dts_decode_table [];
extern const void * BRAP_IMG_dts_interframe_buf [];
extern const void * BRAP_IMG_dts_passthru [];

extern const void * BRAP_IMG_dtshd_framesync [];
extern const void * BRAP_IMG_dtshd_hddvd_framesync [];

extern const void * BRAP_IMG_bdlpcm_framesync [];
extern const void * BRAP_IMG_bdlpcm_decode [];
extern const void * BRAP_IMG_bdlpcm_interframe_buf[];

extern const void * BRAP_IMG_hddvdlpcm_framesync [];
extern const void * BRAP_IMG_hddvdlpcm_decode [];
extern const void * BRAP_IMG_hddvdlpcm_interframe_buf[];

extern const void * BRAP_IMG_dvdlpcm_framesync [];
extern const void * BRAP_IMG_dvdlpcm_decode [];
extern const void * BRAP_IMG_dvdlpcm_interframe_buf[];

extern const void * BRAP_IMG_ddbm_postprocess[];
extern const void * BRAP_IMG_ddbm_postprocess_table[];

extern const void * BRAP_IMG_ac3lossless_framesync [];
extern const void * BRAP_IMG_mlp_decode [];
extern const void * BRAP_IMG_mlp_framesync [];
extern const void * BRAP_IMG_mlp_hddvd_framesync [];
extern const void * BRAP_IMG_mlp_decode_table [];


#endif

/* Adding for MP3 Encoder */
#ifndef BCHP_7411_VER /* For chips other than 7411 */
extern const void * BRAP_IMG_encoder_iboot [];
extern const void * BRAP_IMG_encoder_scheduler_data [];
extern const void * BRAP_IMG_encoder_scheduler_code [];
extern const void * BRAP_IMG_mp3e_table [];
extern const void * BRAP_IMG_mp3e_inter_frame [];
extern const void * BRAP_IMG_mp3e_inter_stage [];
extern const void * BRAP_IMG_mp3e_scratch [];
extern const void * BRAP_IMG_mp3e_stg0 [];
extern const void * BRAP_IMG_mp3e_stg1 [];
#if (BRAP_DVD_FAMILY == 1)||( BCHP_CHIP == 3563 ) || (BRAP_7405_FAMILY == 1)
extern const void * BRAP_IMG_dts_enc_code [];
extern const void * BRAP_IMG_dts_enc_table [];
extern const void * BRAP_IMG_dts_enc_inter_frame [];
extern const void * BRAP_IMG_dts_enc_inter_stage [];
extern const void * BRAP_IMG_dts_enc_scratch [];
#endif /* 7440 */
#endif

#if ( BCHP_CHIP == 7400 || BCHP_CHIP == 3563 || (BRAP_DVD_FAMILY == 1) || BRAP_7405_FAMILY == 1)
#ifdef BRAP_DDP_SUPPORT		
extern const void * BRAP_IMG_ddpdep_framesync [];
extern const void * BRAP_IMG_ddp_convert [];
#endif
#endif

#if ( BCHP_CHIP==3563 )
extern const void *BRAP_IMG_BtscUs[];
extern const void *BRAP_IMG_BtscJapan[];
extern const void *BRAP_IMG_BtscKorea[];
extern const void *BRAP_IMG_pes_framesync[];
extern const void *BRAP_IMG_pcm_input[];
extern const void *BRAP_IMG_prologic2_decode[];
extern const void *BRAP_IMG_prologic2_table[];
extern const void *BRAP_IMG_prologic2_interframe_buf[];
extern const void *BRAP_IMG_srsxt[];
extern const void *BRAP_IMG_srsxt_tables[];
extern const void *BRAP_IMG_srsxt_interframe[];
extern const void *BRAP_IMG_avl[];
extern const void *BRAP_IMG_avl_table[];
extern const void *BRAP_IMG_avl_interframe_buf[];
extern const void *BRAP_IMG_xen[];
extern const void *BRAP_IMG_xen_table[];
extern const void *BRAP_IMG_xen_interframe_buf[];
extern const void *BRAP_IMG_bbe[];
extern const void *BRAP_IMG_bbe_table[];
extern const void *BRAP_IMG_bbe_interframe_buf[];
extern const void *BRAP_IMG_custom_surround[];
extern const void *BRAP_IMG_custom_surround_table[];
extern const void *BRAP_IMG_custom_surround_interframe_buf[];
extern const void *BRAP_IMG_custom_bass[];
extern const void *BRAP_IMG_custom_bass_table[];
extern const void *BRAP_IMG_custom_bass_interframe_buf[];
extern const void *BRAP_IMG_custom_voice[];
extern const void *BRAP_IMG_custom_voice_table[];
extern const void *BRAP_IMG_custom_voice_interframe_buf[];
extern const void *BRAP_IMG_peq[];
extern const void *BRAP_IMG_peq_table[];
extern const void *BRAP_IMG_peq_interframe_buf[];
#endif

#if ( BCHP_CHIP == 7400 || BCHP_CHIP == 3563 || (BRAP_DVD_FAMILY == 1) || BRAP_7405_FAMILY == 1 )
#ifndef BRAP_DDP_SUPPORT
extern const void * BRAP_IMG_ac3_passthru_table [];
extern const void * BRAP_IMG_ac3_be_decode [];
extern const void * BRAP_IMG_ac3_convert [];
extern const void * BRAP_IMG_ac3_fe_decode [];
extern const void * BRAP_IMG_ac3_interframe_buf [];
extern const void * BRAP_IMG_ac3_passthru_interframe_buf [];
#endif
#endif


#if (BCHP_CHIP == 7401 || BCHP_CHIP == 7118 || BCHP_CHIP == 7403 || BCHP_CHIP == 3563 ||BCHP_CHIP == 7400 )
extern const void * BRAP_IMG_src_postprocess[];
extern const void * BRAP_IMG_src_postprocess_table[];
extern const void * BRAP_IMG_src_interframe_buf[];
#endif

#if RAP_DOLBYVOLUME_SUPPORT 
extern const void * BRAP_IMG_dolby_volume[];
extern const void * BRAP_IMG_dolby_volume_table[];
extern const void * BRAP_IMG_dolby_volume_interframe_buf[];
#endif

#ifdef RAP_SRSTRUVOL_SUPPORT
extern const void * BRAP_IMG_srs_TruVolume_post_process[];
extern const void * BRAP_IMG_srs_TruVolume_table[];
extern const void * BRAP_IMG_srs_TruVolume_interframe_buf[];
#endif

#if (BCHP_CHIP == 7401 || BCHP_CHIP == 7118 || BCHP_CHIP == 7403 || BCHP_CHIP == 7400)
extern const void * BRAP_IMG_dsola_postprocess[];
extern const void * BRAP_IMG_dsola_postprocess_table[];
extern const void * BRAP_IMG_dsola_interframe_buf[];
#if (BRAP_AD_SUPPORTED == 1)
extern const void * BRAP_IMG_fade_control[];
extern const void * BRAP_IMG_fade_control_tables[];
extern const void * BRAP_IMG_fade_control_interframe_buf[];
extern const void * BRAP_IMG_pan_control[];
extern const void * BRAP_IMG_pan_control_tables[];
extern const void * BRAP_IMG_pan_control_interframe_buf[];
#endif
#endif

#if ( BCHP_CHIP == 3563 ) || (BRAP_7405_FAMILY == 1)
extern const void * BRAP_IMG_aache_lp_decode_stg0[];
extern const void * BRAP_IMG_aache_lp_decode_stg1[];
extern const void * BRAP_IMG_aache_lp_decode_stg2[];
extern const void * BRAP_IMG_aache_lp_interframe_buf[];
extern const void * BRAP_IMG_aache_lp_table_stg0[];
extern const void * BRAP_IMG_aache_lp_table_stg1[];
extern const void * BRAP_IMG_aache_passthru[];
#endif

#if (BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403) || ( BCHP_CHIP == 7400 ) 
extern const void * BRAP_IMG_wmapro_framesync[];
extern const void * BRAP_IMG_wmapro_decode_stg0[];
extern const void * BRAP_IMG_wmapro_decode_stg1[];
extern const void * BRAP_IMG_wmapro_decode_table[];
extern const void * BRAP_IMG_wmapro_passthru[];
#endif

#if (BRAP_7405_FAMILY == 1) || (BRAP_DVD_FAMILY == 1)
extern const void * BRAP_IMG_wmapro_framesync[];
extern const void * BRAP_IMG_wmapro_decode_stg0[];
extern const void * BRAP_IMG_wmapro_decode_stg1[];
extern const void * BRAP_IMG_wmapro_decode_table[];
extern const void * BRAP_IMG_wmapro_passthru[];
#endif

extern const void * BRAP_IMG_dvdlpcm_framesync [];
extern const void * BRAP_IMG_dvdlpcm_decode [];
extern const void * BRAP_IMG_dvdlpcm_interframe_buf[];
#if (BRAP_DVD_FAMILY == 1)
extern const void * BRAP_IMG_dtshd_nbc_core_framesync[]; 
#endif
#if (BCHP_CHIP == 7401) ||(BCHP_CHIP==7403) || (BCHP_CHIP==7118) || ( BCHP_CHIP == 7400 )
extern const void * BRAP_IMG_aache_decode_stg0[];
extern const void * BRAP_IMG_aache_decode_stg1[];
extern const void * BRAP_IMG_aache_decode_stg2[];
extern const void * BRAP_IMG_aache_decode_stg3[];
extern const void * BRAP_IMG_aache_decode_stg4[];

extern const void * BRAP_IMG_aache_decode_table_stg0[];
extern const void * BRAP_IMG_aache_decode_table_stg1[];
extern const void * BRAP_IMG_aache_decode_table_stg2[];
extern const void * BRAP_IMG_aache_decode_table_stg3[];
extern const void * BRAP_IMG_aache_decode_table_stg4[];

extern const void * BRAP_IMG_aache_interframe_buf[];
extern const void * BRAP_IMG_aache_passthru[];
#endif

#if (BRAP_DVD_FAMILY == 1)
extern const void * BRAP_IMG_aacloas_framesync[];
extern const void * BRAP_IMG_aache_decode_stg0[];
extern const void * BRAP_IMG_aache_decode_stg1[];
extern const void * BRAP_IMG_aache_decode_stg2[];
extern const void * BRAP_IMG_aache_decode_stg3[];
extern const void * BRAP_IMG_aache_decode_stg4[];

extern const void * BRAP_IMG_aache_decode_table[];
extern const void * BRAP_IMG_aache_interframe_buf[];

extern const void * BRAP_IMG_aacadts_framesync[];

#endif

extern const void * BRAP_IMG_dtshd_stg1_framesync[];
extern const void * BRAP_IMG_dtshd_stg1_decode_table[];
extern const void * BRAP_IMG_dtshd_stg1_interframe_buf[];

extern const void * BRAP_IMG_scheduler_dwd_code[];
extern const void * BRAP_IMG_scheduler_dsp1_dwd_code[];

extern const void * BRAP_IMG_aache_decode_stg0_tables[];
extern const void * BRAP_IMG_aache_decode_stg1_tables[];
extern const void * BRAP_IMG_aache_decode_stg2_tables[];
extern const void * BRAP_IMG_aache_decode_stg3_tables[];
extern const void * BRAP_IMG_aache_decode_stg4_tables[];


#if (BRAP_DTS_SUPPORTED == 1)		
extern const void * BRAP_IMG_dts_framesync [];
extern const void * BRAP_IMG_dts_core_decode [];
extern const void * BRAP_IMG_dts_core_decode_table [];
extern const void * BRAP_IMG_dts_core_decode_interframe_buf [];
extern const void * BRAP_IMG_dts_passthru [];
#endif

#ifdef RAP_SRSTRUVOL_CERTIFICATION
extern const void * BRAP_IMG_pcm_decode[];
extern const void * BRAP_IMG_pcm_framesync[];
#endif

#if (BRAP_DTS_PASSTHRU_SUPPORTED == 1)		
extern const void * BRAP_IMG_dts_framesync [];
extern const void * BRAP_IMG_dts_passthru [];
#endif

#ifdef RAP_DRA_SUPPORT	
extern const void * BRAP_IMG_dra_decode[];
extern const void * BRAP_IMG_dra_decode_table[];
extern const void * BRAP_IMG_dra_interframe_buf[];
extern const void * BRAP_IMG_dra_passthru[];
extern const void * BRAP_IMG_dra_framesync[];
#endif

#ifdef RAP_PCMWAV_SUPPORT
extern const void * BRAP_IMG_pcmwav_decode[];
extern const void * BRAP_IMG_pcmwav_framesync[];
extern const void * BRAP_IMG_pcmwav_interframe_buf[];
#endif

/*
	This array will contain pointers to the arrays that contain that chunk pointers of the firmware binary.
	Based on the firmware ID one particular firmware Image can be accessed from this array
*/
static const void **BRAP_IMG_FirmwareImages [] = 
	{
		BRAP_IMG_iboot, 
		BRAP_IMG_scheduler_code, 	
		BRAP_IMG_scheduler_data, 
		BRAP_IMG_mpeg_framesync,
#if defined ( BCHP_7411_VER ) || (BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403)		
		BRAP_IMG_ac3_framesync, 
#else
		NULL, 
#endif
		BRAP_IMG_mpeg_l1_decode, 
		BRAP_IMG_mpeg_l2_decode, 
		BRAP_IMG_mp3_decode, 
#if defined ( BCHP_7411_VER ) || (BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403)		
		BRAP_IMG_ac3_decode,
#else
		NULL, 
#endif
		BRAP_IMG_mpeg_decode_table, 
#if defined ( BCHP_7411_VER ) || (BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403)		
		BRAP_IMG_ac3_decode_table,
#else
		NULL, 
#endif

#ifndef BCHP_7411_VER /* For chips other than 7411 */
		NULL, 
		NULL,
#else
		BRAP_IMG_ac3_downmix_code, 
		BRAP_IMG_ac3_downmix_table,
#endif
#if defined ( BCHP_7411_VER ) || (BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403)		
		BRAP_IMG_ac3_passthru, 
#else
		NULL, 
#endif
		BRAP_IMG_mpeg_passthru,
		BRAP_IMG_mpeg_interframe_buf,
#ifndef BCHP_7411_VER /* For chips other than 7411 */
		
#if (BRAP_DVD_FAMILY == 1)
        BRAP_IMG_aacadts_framesync,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        BRAP_IMG_aacloas_framesync,
		NULL,
        NULL,
#else
        BRAP_IMG_aac_framesync,
		BRAP_IMG_aac_decode,
		BRAP_IMG_aac_decode_table,
		BRAP_IMG_aac_interframe_buf,
		BRAP_IMG_aac_passthru,
		BRAP_IMG_aac_downmix,
        BRAP_IMG_aache_framesync,
		BRAP_IMG_aache_decode,
        BRAP_IMG_aacplus_decode_table,
#endif

#if ((BRAP_DVD_FAMILY == 1) || BCHP_CHIP == 7118 || BCHP_CHIP ==7403 || BCHP_CHIP ==7401 || BCHP_CHIP == 7400)
		BRAP_IMG_aache_interframe_buf,
#else
		BRAP_IMG_aacplus_interframe_buf,
#endif
#if ( BCHP_CHIP == 7400 || BCHP_CHIP == 3563 || (BRAP_DVD_FAMILY == 1) || BRAP_7405_FAMILY == 1 )
#ifndef BRAP_DDP_SUPPORT	
		BRAP_IMG_ac3_framesync,
		BRAP_IMG_ac3_be_decode,
		BRAP_IMG_ac3_fe_decode,
		BRAP_IMG_ac3_decode_table,
		BRAP_IMG_ac3_interframe_buf,
		BRAP_IMG_ac3_passthru,
		BRAP_IMG_ac3_passthru_interframe_buf,
		BRAP_IMG_ac3_passthru_table,
#else
		BRAP_IMG_ddp_framesync,
		BRAP_IMG_ddp_be_decode,
		BRAP_IMG_ddp_fe_decode,
		BRAP_IMG_ddp_decode_table,
		BRAP_IMG_ddp_interframe_buf,
		BRAP_IMG_ddp_passthru,
		BRAP_IMG_ddp_passthru_interframe_buf,
		BRAP_IMG_ddp_passthru_table,
#endif
#else
		BRAP_IMG_ddp_framesync,
		BRAP_IMG_ddp_be_decode,
		BRAP_IMG_ddp_fe_decode,
		BRAP_IMG_ddp_decode_table,
		BRAP_IMG_ddp_interframe_buf,
		BRAP_IMG_ddp_passthru,
		BRAP_IMG_ddp_passthru_interframe_buf,
		BRAP_IMG_ddp_passthru_table,
#endif

#if (BRAP_DVD_FAMILY == 1)
		BRAP_IMG_dts_framesync,
		BRAP_IMG_dts_decode,
		BRAP_IMG_dts_decode_table,
		BRAP_IMG_dts_interframe_buf,
		BRAP_IMG_dts_passthru,
		BRAP_IMG_bdlpcm_framesync,
		BRAP_IMG_bdlpcm_decode,
#elif ( BCHP_CHIP == 3563 )
		NULL,
		NULL,
		NULL,
		NULL,
		BRAP_IMG_dts_passthru,
		NULL,
		NULL,
#elif (BRAP_DTS_SUPPORTED == 1)
        BRAP_IMG_dts_framesync,
        BRAP_IMG_dts_core_decode,
        BRAP_IMG_dts_core_decode_table,
        BRAP_IMG_dts_core_decode_interframe_buf,
        BRAP_IMG_dts_passthru,
        NULL,
        NULL,
#elif (BRAP_DTS_PASSTHRU_SUPPORTED == 1)
        BRAP_IMG_dts_framesync,
        NULL,
        NULL,
        NULL,
        BRAP_IMG_dts_passthru,
        NULL,
        NULL,        
#else
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
        NULL,
        NULL,        
#endif		
#else
		BRAP_IMG_aac_framesync,
		BRAP_IMG_aac_decode,
		BRAP_IMG_aac_decode_table,
		BRAP_IMG_aac_interframe_buf,
		BRAP_IMG_aac_passthru,
		BRAP_IMG_aac_downmix,
		BRAP_IMG_aache_framesync,
		BRAP_IMG_aache_decode,
		BRAP_IMG_aacplus_decode_table,
		BRAP_IMG_aacplus_interframe_buf,
		BRAP_IMG_ddp_framesync,
		BRAP_IMG_ddp_be_decode,
		BRAP_IMG_ddp_fe_decode,
		BRAP_IMG_ddp_decode_table,
		BRAP_IMG_ddp_interframe_buf,
		BRAP_IMG_ddp_passthru,
		BRAP_IMG_ddp_passthru_interframe_buf,
		BRAP_IMG_ddp_passthru_table,
		BRAP_IMG_dts_framesync,
		BRAP_IMG_dts_decode,
		BRAP_IMG_dts_decode_table,
		BRAP_IMG_dts_interframe_buf,
		BRAP_IMG_dts_passthru,
		BRAP_IMG_bdlpcm_framesync,
		BRAP_IMG_bdlpcm_decode,
#endif
/* Adding for MP3 Encoder */
#ifndef BCHP_7411_VER /* For chips other than 7411 */
		BRAP_IMG_encoder_iboot,
		BRAP_IMG_encoder_scheduler_code,
		BRAP_IMG_encoder_scheduler_data,
		BRAP_IMG_mp3e_table,
		BRAP_IMG_mp3e_inter_frame,
		BRAP_IMG_mp3e_inter_stage,
		BRAP_IMG_mp3e_scratch,
		BRAP_IMG_mp3e_stg0,
		BRAP_IMG_mp3e_stg1,
/*		BRAP_IMG_mp3e_stg2,*/
		NULL,
/*		BRAP_IMG_mp3e_stg3,
		BRAP_IMG_mp3e_stg4,*/
		NULL,
		NULL,
#else
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
#endif
#ifndef BCHP_7411_VER /* For chips other than 7411 */
#if (BRAP_DVD_FAMILY == 1)
		BRAP_IMG_hddvdlpcm_framesync,
		BRAP_IMG_hddvdlpcm_decode,
		BRAP_IMG_dtshd_framesync,
	    BRAP_IMG_ddbm_postprocess,
		BRAP_IMG_ddbm_postprocess_table,
		BRAP_IMG_dvdlpcm_framesync,
		BRAP_IMG_dvdlpcm_decode,

        BRAP_IMG_wma_std_framesync,
		BRAP_IMG_wma_std_stg0_decode,
		BRAP_IMG_wma_std_stg0_decode_table,
		BRAP_IMG_wma_std_interframe_buf,

        BRAP_IMG_ac3lossless_framesync,
		BRAP_IMG_dtshd_hddvd_framesync,
		BRAP_IMG_bdlpcm_interframe_buf,
		BRAP_IMG_dvdlpcm_interframe_buf,
		BRAP_IMG_hddvdlpcm_interframe_buf,
		BRAP_IMG_mlp_decode,
		BRAP_IMG_mlp_decode_table,
		BRAP_IMG_mlp_framesync,
		BRAP_IMG_mlp_hddvd_framesync,
#else
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		BRAP_IMG_dvdlpcm_framesync, 
		BRAP_IMG_dvdlpcm_decode,		

		BRAP_IMG_wma_std_framesync,
		BRAP_IMG_wma_std_stg1_decode,
		BRAP_IMG_wma_std_stg1_decode_table,
		BRAP_IMG_wma_std_interframe_buf,

		NULL,
		NULL,
		NULL,
		BRAP_IMG_dvdlpcm_interframe_buf, 
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
#endif		
#else
		BRAP_IMG_hddvdlpcm_framesync,
		BRAP_IMG_hddvdlpcm_decode,
        /* Note: Except for the frame-sync, all other fw is common between DTS and DTS-HD */
		BRAP_IMG_dtshd_framesync,
		BRAP_IMG_ddbm_postprocess,
		BRAP_IMG_ddbm_postprocess_table,
		BRAP_IMG_dvdlpcm_framesync,
		BRAP_IMG_dvdlpcm_decode,
		BRAP_IMG_wma_std_framesync,
		BRAP_IMG_wma_std_stg1_decode,
		BRAP_IMG_wma_std_stg1_decode_table,
		BRAP_IMG_wma_std_interframe_buf,
        /* Note: Except for the frame-sync, all other fw is common between AC3 Lossless and AC3 */
		BRAP_IMG_ac3lossless_framesync,
		BRAP_IMG_dtshd_hddvd_framesync,
		BRAP_IMG_bdlpcm_interframe_buf,
		BRAP_IMG_dvdlpcm_interframe_buf,
		BRAP_IMG_hddvdlpcm_interframe_buf,
		BRAP_IMG_mlp_decode,
		BRAP_IMG_mlp_decode_table,
		BRAP_IMG_mlp_framesync,
		BRAP_IMG_mlp_hddvd_framesync,
#endif		
/* Adding for DTS Encoder */
#if (BRAP_DVD_FAMILY == 1)||( BCHP_CHIP == 3563 ) || (BRAP_7405_FAMILY == 1)
		BRAP_IMG_dts_enc_code,
		BRAP_IMG_dts_enc_table,
		BRAP_IMG_dts_enc_inter_frame,
		BRAP_IMG_dts_enc_inter_stage,
		BRAP_IMG_dts_enc_scratch,
#else
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
#endif/* 7440 */
/* Adding for DDP Converted */
#if ( BCHP_CHIP == 7400 || BCHP_CHIP == 3563 || (BRAP_DVD_FAMILY == 1) || BRAP_7405_FAMILY == 1 )
#ifdef BRAP_DDP_SUPPORT		
		BRAP_IMG_ddp_convert,
#else
		BRAP_IMG_ac3_convert,
#endif
#else
		NULL,
#endif
#if ( BCHP_CHIP==3563 )
        BRAP_IMG_BtscUs,
        BRAP_IMG_BtscJapan,
        BRAP_IMG_BtscKorea,
        BRAP_IMG_pes_framesync,
        BRAP_IMG_pcm_input,
#else
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
#endif
#if (BRAP_DVD_FAMILY == 1)
        BRAP_IMG_dts_1_decode
#else
        NULL
#endif
#if (BCHP_CHIP == 7401 || BCHP_CHIP == 7118 || BCHP_CHIP == 7403 || BCHP_CHIP==3563 || BCHP_CHIP == 7400)
        ,BRAP_IMG_src_postprocess,
        BRAP_IMG_src_postprocess_table,
        BRAP_IMG_src_interframe_buf,           
#else
        ,NULL,
        NULL,
        NULL,
#endif
#if (BCHP_CHIP==3563)
	BRAP_IMG_prologic2_decode,
	BRAP_IMG_prologic2_table,
	BRAP_IMG_prologic2_interframe_buf,
	BRAP_IMG_srsxt,
	BRAP_IMG_srsxt_tables,
	BRAP_IMG_srsxt_interframe,
	BRAP_IMG_avl,
	BRAP_IMG_avl_table,
	BRAP_IMG_avl_interframe_buf,
#else	
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
#endif
#if (BRAP_DVD_FAMILY == 1)
    BRAP_IMG_ddbm_interframe_buf,
    BRAP_IMG_scheduler_dsp1_code,
    BRAP_IMG_scheduler_dsp1_data,
    BRAP_IMG_dtslbr_framesync,
    BRAP_IMG_dtslbr0_decode_code,
    BRAP_IMG_dtslbr1_decode_table,
    BRAP_IMG_dtslbr1_decode_interframe_buf,
    BRAP_IMG_mlp_interframe_buf,
#else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
#if (BCHP_CHIP==3563)
	BRAP_IMG_xen,
	BRAP_IMG_xen_table,
	BRAP_IMG_xen_interframe_buf,
	BRAP_IMG_bbe,
	BRAP_IMG_bbe_table,
	BRAP_IMG_bbe_interframe_buf
#else
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,	
	NULL
#endif
#if (BCHP_CHIP == 7401 || BCHP_CHIP == 7118 || BCHP_CHIP == 7403 || BCHP_CHIP == 7400)
        ,BRAP_IMG_dsola_postprocess,
        BRAP_IMG_dsola_postprocess_table,
        BRAP_IMG_dsola_interframe_buf,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	BRAP_IMG_aache_passthru,
#else
        ,NULL,
        NULL,
        NULL,
#if ( BCHP_CHIP == 7400 ) || ( BCHP_CHIP == 3563 ) || (BRAP_7405_FAMILY == 1)
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	BRAP_IMG_aache_passthru,
#else
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
#endif        
#endif
#if (BRAP_DVD_FAMILY == 1)
		BRAP_IMG_ddpdep_framesync,
		BRAP_IMG_ddpdep_be_decode,
		BRAP_IMG_ddpdep_fe_decode,
		BRAP_IMG_ddpdep_decode_table,
		BRAP_IMG_ddpdep_interframe_buf,
    	BRAP_IMG_mpeg_mc_framesync,
		BRAP_IMG_mpeg_mc_decode,
		BRAP_IMG_mpeg_mc_decode_table,
		BRAP_IMG_mpeg_mc_passthru,
        BRAP_IMG_dts_neo_postprocess,
        BRAP_IMG_dts_neo_postprocess_table,
        BRAP_IMG_dts_neo_interframe_buf,
		BRAP_IMG_dtshd_x96_decode,
		BRAP_IMG_dtshd_xll_decode,
		BRAP_IMG_dtshd_extras_decode,
#else
#ifdef BRAP_DDP_SUPPORT
#if (BCHP_CHIP==7400)
        BRAP_IMG_ddpdep_framesync,
#else
	    NULL,
#endif
#else
	    NULL,
#endif	    
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    NULL,
        NULL,
	    NULL,
	    NULL,
        NULL,
#endif
#if (BCHP_CHIP == 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403) || ( BCHP_CHIP == 7400 ) 
	BRAP_IMG_wmapro_framesync,
	BRAP_IMG_wmapro_decode_stg0,
	BRAP_IMG_wmapro_decode_stg1,
	BRAP_IMG_wmapro_decode_table,
	BRAP_IMG_wmapro_passthru,
#elif (BRAP_7405_FAMILY == 1)||(BRAP_DVD_FAMILY == 1)
	BRAP_IMG_wmapro_framesync,
	BRAP_IMG_wmapro_decode_stg0,
	BRAP_IMG_wmapro_decode_stg1,
	BRAP_IMG_wmapro_decode_table,
	BRAP_IMG_wmapro_passthru,
#else
        NULL,
	    NULL,
	    NULL,
	    NULL,	    
        NULL,
#endif
#if (BRAP_DVD_FAMILY == 1)
		BRAP_IMG_dtshd_exss_decode_code,
		BRAP_IMG_ddpdep_passthru,
		BRAP_IMG_mlp_passthru,
		BRAP_IMG_dtshd_passthru
#else
	    NULL,
	    NULL,
	    NULL,	    
        NULL
#endif
#if (BCHP_CHIP==3563)
	,BRAP_IMG_custom_surround,
	BRAP_IMG_custom_surround_table,
	BRAP_IMG_custom_surround_interframe_buf,
	BRAP_IMG_custom_bass,
	BRAP_IMG_custom_bass_table,
	BRAP_IMG_custom_bass_interframe_buf,
	BRAP_IMG_custom_voice,
	BRAP_IMG_custom_voice_table,
	BRAP_IMG_custom_voice_interframe_buf
#else
	,NULL,
	NULL,
	NULL,
	NULL,	
	NULL,
	NULL,
	NULL,	
	NULL,
	NULL
#endif	

#if (BRAP_DVD_FAMILY == 1)
	,BRAP_IMG_dtslbr1_decode_code
#else
	,NULL
#endif	

#if (BCHP_CHIP==3563)
	,BRAP_IMG_peq,
	BRAP_IMG_peq_table,
	BRAP_IMG_peq_interframe_buf
#else
	,NULL,
	NULL,
	NULL
#endif	



#if (BRAP_DVD_FAMILY == 1)
	,BRAP_IMG_dtshd_subaudio_decode_table,
    BRAP_IMG_dtshd_subaudio_interframe_buf,
	BRAP_IMG_dtshd_subaudio_framesync,
    BRAP_IMG_dtshd_subaudio_decode,
    BRAP_IMG_dtshd_subaudio_1_decode
#else
	,NULL,
	NULL,
	NULL,
	NULL,
	NULL
#endif	

/*DVD_A_LPCM*/
#if (BRAP_DVD_FAMILY == 1)
    ,BRAP_IMG_dvdalpcm_interframe,
    BRAP_IMG_dvdlapcm_framesync,
    BRAP_IMG_dvdalpcm_decode
#else
    ,NULL,
    NULL,
    NULL
#endif
#if (BRAP_DVD_FAMILY == 1)
    ,BRAP_IMG_dtshd_nbc_core_framesync
#else
    ,NULL
#endif

#if (BCHP_CHIP==7401) ||(BCHP_CHIP==7403) ||(BCHP_CHIP==7118)||(BCHP_CHIP==7400)
	,BRAP_IMG_aache_decode_stg0,
	BRAP_IMG_aache_decode_stg1,
	BRAP_IMG_aache_decode_stg2,
	BRAP_IMG_aache_decode_stg3,
	BRAP_IMG_aache_decode_stg4,
	BRAP_IMG_aache_decode_table_stg0,
	BRAP_IMG_aache_decode_table_stg1,
	BRAP_IMG_aache_decode_table_stg2,
	BRAP_IMG_aache_decode_table_stg3,
	BRAP_IMG_aache_decode_table_stg4,
	NULL,
	NULL,
	NULL,
	BRAP_IMG_scheduler_dwd_code,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
	
#elif (BRAP_DVD_FAMILY == 1)
    ,BRAP_IMG_aache_decode_stg0,
	BRAP_IMG_aache_decode_stg1,
	BRAP_IMG_aache_decode_stg2,
	BRAP_IMG_aache_decode_stg3,
	BRAP_IMG_aache_decode_stg4,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	BRAP_IMG_dtshd_stg1_framesync,
	BRAP_IMG_dtshd_stg1_decode_table,
	BRAP_IMG_dtshd_stg1_interframe_buf,
	BRAP_IMG_scheduler_dwd_code,
	BRAP_IMG_scheduler_dsp1_dwd_code,
	BRAP_IMG_aache_decode_stg0_tables,
	BRAP_IMG_aache_decode_stg1_tables,
	BRAP_IMG_aache_decode_stg2_tables,
	BRAP_IMG_aache_decode_stg3_tables,
	BRAP_IMG_aache_decode_stg4_tables
#else
	,NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
#endif

#if (BRAP_AD_SUPPORTED == 1)
    ,BRAP_IMG_fade_control,
    BRAP_IMG_fade_control_tables,
    BRAP_IMG_fade_control_interframe_buf,
    BRAP_IMG_pan_control,
    BRAP_IMG_pan_control_tables,
    BRAP_IMG_pan_control_interframe_buf
#else
	,NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
#endif

#ifdef RAP_DRA_SUPPORT	
	,BRAP_IMG_dra_passthru,
	BRAP_IMG_dra_framesync,
	BRAP_IMG_dra_interframe_buf
#else
	,NULL,
	NULL,
	NULL
#endif

#if RAP_DOLBYVOLUME_SUPPORT 
    ,BRAP_IMG_dolby_volume,
    BRAP_IMG_dolby_volume_table,
    BRAP_IMG_dolby_volume_interframe_buf           
#else
	,NULL,
	NULL,
	NULL
#endif
#ifdef RAP_SRSTRUVOL_SUPPORT
    ,BRAP_IMG_srs_TruVolume_post_process,
    BRAP_IMG_srs_TruVolume_table,
    BRAP_IMG_srs_TruVolume_interframe_buf           
#else
	,NULL,
	NULL,
	NULL
#endif

#ifdef RAP_SRSTRUVOL_CERTIFICATION
    ,BRAP_IMG_pcm_decode,
    BRAP_IMG_pcm_framesync
#else
	,NULL,
	NULL
#endif

#ifdef RAP_DRA_SUPPORT	
	,BRAP_IMG_dra_decode,
	BRAP_IMG_dra_decode_table
#else
	,NULL,
	NULL
#endif

#ifdef RAP_PCMWAV_SUPPORT
    ,BRAP_IMG_pcmwav_decode,
    BRAP_IMG_pcmwav_framesync,
    BRAP_IMG_pcmwav_interframe_buf
#else
	,NULL,
	NULL,
	NULL
#endif

};

/*	This context is used by other modules to make use of this interface. They need to supply this as a parameter to BRAP_IMG_Open */
void *BRAP_IMG_Context = (void *)BRAP_IMG_FirmwareImages;

static BERR_Code BRAP_IMG_Open(void *context, void **image, unsigned image_id)
{
/*
	This function has to be used for opening a firmware image. The pointer to the firmware image array is given
	that contains the header and the chunks. This works based on the firmware image id  that is supplied.
*/
	if (image_id >=  sizeof(BRAP_IMG_FirmwareImages)/sizeof(*BRAP_IMG_FirmwareImages)) 
	{
		/* If the image ID is greater than available IDs return an error */
		BDBG_ERR(("Invalid image_id %d",image_id));
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}	

	*image = ((void **)context)[image_id];	

	if (NULL == *image)
	{
	    BDBG_ERR (("Null image pointer!!!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}

	return BERR_SUCCESS;
}

static BERR_Code BRAP_IMG_Next(void *image, unsigned chunk, const void **data, uint16_t length)
{
/*
	After opening the firmware image, the user of this interface will then be interested in getting the chunks
	and the header giving information about the chunks.
*/

	BDBG_ASSERT(data);	
	BSTD_UNUSED(length);

	if (chunk > ((uint32_t*)((const void **)image)[0])[1])
	{
		/* if the chunk number is greater than the available chunks in that image return error */
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	*data = ((const void **)image)[chunk];	
	
	if (NULL == *data) 
	{		
		return BERR_TRACE(BERR_INVALID_PARAMETER);	
	}	

	return BERR_SUCCESS;
}

static void BRAP_IMG_Close(void *image)
{	
/* This function is used to close the firmware image that was opened using BRAP_IMG_Open */
	BSTD_UNUSED(image);	
	return;
}

/* The interface is actually a set of three functions open, next and close. The same has been implemented here and their function pointers supplied */
const BIMG_Interface BRAP_IMG_Interface = {BRAP_IMG_Open, BRAP_IMG_Next, BRAP_IMG_Close};

