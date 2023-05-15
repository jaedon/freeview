/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/128 $
 * $brcm_Date: 2/7/13 2:10p $
 * $brcm_Revision: Hydra_Software_Devel/128 $
 * $brcm_Date: 2/7/13 2:10p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_priv.c $
 * 
 * Hydra_Software_Devel/128   2/7/13 2:10p arnabb
 * SW7425-4216:Correcting the coverity issue..
 * 
 * Hydra_Software_Devel/127   2/7/13 11:21a jgarrett
 * SW7425-4216: Localizing mutex use for task list, downgrading capture
 * BDBG_ERR messages
 * 
 * Hydra_Software_Devel/126   2/6/13 7:13p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/125   2/4/13 12:40p nihar
 * SW7425-4229: Adding support for BDSP Version - Merging changes to
 * Hydra_Software_Devel Branch
 * 
 * Hydra_Software_Devel/BDSP_VERSION_BRANCH/2   1/30/13 11:53a nihar
 * SW7425-4229: Adding support for BDSP Version - Modification after Jon's
 * feedback.
 * 
 * Hydra_Software_Devel/BDSP_VERSION_BRANCH/1   1/29/13 5:08p nihar
 * SW7425-4229: Adding support for BDSP Version
 * 
 * Hydra_Software_Devel/124   1/25/13 2:36p ananthan
 * SW7425-3404: IndepDelayEnableFlag to be dependent on the value of
 * independent delay ( zero/non-zero) instead of being always true.
 * 
 * Hydra_Software_Devel/SW7425-4216/7   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/123   1/10/13 11:09a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/122   1/9/13 11:34a kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. Merging to mainline.
 * 
 * Hydra_Software_Devel/122   1/8/13 4:00p kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. Merging to mainline.
 * 
 * Hydra_Software_Devel/122   1/8/13 2:23p kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. Merging to mainline.
 * 
 * Hydra_Software_Devel/122   1/8/13 1:54p kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr. Merging to mainline.
 * 
 * Hydra_Software_Devel/121   1/3/13 5:20p ananthan
 * SW7425-3628: Changes for cached access (Merge to mainline)
 * 
 * Hydra_Software_Devel/SW7425-3628/3   10/17/12 1:51p gprasad
 * SW7425-3628: Changes for cached access
 * 
 * Hydra_Software_Devel/SW7425-3628/2   8/30/12 4:14p jgarrett
 * SW7425-3628: Using cached memory for Master/Slave/SIPS APIs
 * 
 * Hydra_Software_Devel/SW7425-3628/1   8/29/12 5:57p jgarrett
 * SW7425-3628: Converting BMEM accesses to cached memory
 * 
 * Hydra_Software_Devel/SW7425-4350/1   12/19/12 11:55a kavitha
 * SW7425-4350: Restoring the L2 interrupts using a new function added in
 * bint- void BINT_ApplyL2State_isr(BINT_Handle IntHandle, uint32_t
 * L2Reg)
 * 
 * Hydra_Software_Devel/119   11/26/12 1:21p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/6   1/18/13 11:06a gprasad
 * SW7425-4216: Fixing issues seen during ape integration of rbuf capture
 * 
 * Hydra_Software_Devel/SW7425-4216/5   1/9/13 10:17a gprasad
 * SW7425-4216: Fixing compilation error.
 * 
 * Hydra_Software_Devel/SW7425-4216/4   1/8/13 4:58p gprasad
 * SW7425-4216: Merging mainline changes to rbuf capture branch as of
 * 01/08/2012
 * 
 * Hydra_Software_Devel/SW7425-4216/3   12/4/12 5:07p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/SW7425-4216/2   11/28/12 4:26p gprasad
 * SW7425-4216: Added capture create and destroy apis for configuring
 * capture
 * 
 * Hydra_Software_Devel/SW7425-4216/1   11/12/12 10:41p gprasad
 * SW7425-4216: Initial changes for ring buffer capture - verified to be
 * working for single decode with raaga test
 * 
 * Hydra_Software_Devel/118   10/17/12 3:42p kavitha
 * SW7425-4031: Pulling in reset changes to mainline.
 * 
 * Hydra_Software_Devel/BUS_ERROR_FIX/5   10/3/12 7:26p kavitha
 * SW7425-4031: Updating the return type of reset raagacore to berr_code
 * from uint32_t.
 * 
 * Hydra_Software_Devel/BUS_ERROR_FIX/4   10/3/12 6:01p kavitha
 * SW7425-4031: Updates to the reset logic. Updated reset logic masks the
 * watchdog interrupt. So, post reset we need to unmask the required
 * interrupts.
 * 
 * Hydra_Software_Devel/117   9/17/12 2:58p kavitha
 * HW7344-467: Pulling out the changes to reset sequence. Need to test
 * interrupts more and then make it part of release.
 * 
 * Hydra_Software_Devel/116   9/17/12 2:11p gprasad
 * SW7425-3952: Fixing build errors with raaga test for emulation build
 * 
 * Hydra_Software_Devel/115   9/12/12 4:33p gprasad
 * SW7425-3061: Renaming the sample rate map parameter
 * 
 * Hydra_Software_Devel/114   9/11/12 1:16p gprasad
 * SW7425-3831: Removing the #define to enable the changes for initial
 * mute fill
 * 
 * Hydra_Software_Devel/113   9/10/12 11:59a kavitha
 * HW7344-467: Updates to the reset/unreset sequence.
 * 
 * Hydra_Software_Devel/112   9/7/12 12:17p gprasad
 * SW7425-3831: Adding support for initial mute frame fill in RT transcode
 * cases
 * 
 * Hydra_Software_Devel/111   8/23/12 12:16a rshijith
 * FWRAA-424: Code modification to support VP8 encoder in raaga.
 * 
 * Hydra_Software_Devel/BUS_ERROR_FIX/1   8/28/12 12:38p kavitha
 * HW7344-467: Updates to the reset/unreset sequence.
 * 
 * Hydra_Software_Devel/110   8/20/12 7:14p ananthan
 * SW7425-3322: Enabling conditional download of MLP Passthrough when Open
 * Time Downloading is enabled.
 * 
 * Hydra_Software_Devel/109   8/10/12 10:35a dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Adding userconfig for
 * AMRWB decoder. Fixing nightly build error.
 * 
 * Hydra_Software_Devel/108   7/20/12 1:28a ananthan
 * SW7425-3490: Adding initial frame preservation for MS-11 certification
 * 
 * Hydra_Software_Devel/107   7/17/12 6:30p ramanatm
 * SW7346-865:[7425]Ragga hangs by swicthing mixer types for secondary
 * decoder.
 * 
 * Hydra_Software_Devel/106   7/17/12 5:23p ramanatm
 * SW7425-2805: [7425] Add AMR-NB and AMR-WB codecs support Added new
 * codecs: AMR and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/105   7/16/12 4:14p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/104   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/103   6/11/12 12:10p gprasad
 * FWRAA-443: Fixing the coverity failure issue
 * 
 * Hydra_Software_Devel/102   6/6/12 12:58p ananthan
 * FWRAA-443: Add MAT Encoder support in Raaga
 * 
 * Hydra_Software_Devel/101   5/11/12 3:30p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
 * sys_top_raaga changes
 * 
 * Hydra_Software_Devel/100   4/17/12 11:01a gprasad
 * SW7425-2786: Enabling the changes for Compressed4x and Compressed HBR
 * output ports
 * 
 * Hydra_Software_Devel/99   4/5/12 11:55a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/98   3/30/12 12:31a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/97   3/23/12 11:41a gprasad
 * SW7429-96: Fixing issue with slave feedback buffer updation
 * 
 * Hydra_Software_Devel/96   3/22/12 6:49p gprasad
 * FWRAA-399: DTS LBR SUPPORT
 * 
 * Hydra_Software_Devel/95   3/22/12 6:09p ashoky
 * SW7346-730: Running CRC tests shows occasional software problems.
 * Correcting the DestroyMutex() call.
 * 
 * Hydra_Software_Devel/94   3/20/12 6:15p gprasad
 * SW7429-96: DSP gets "BMEM: Back guard violation" when running apex
 * 
 * Hydra_Software_Devel/93   3/19/12 2:34p gprasad
 * SW7425-2378: Adding support for Monkey audio codec
 * 
 * Hydra_Software_Devel/92   3/14/12 8:31p ashoky
 * SW7231-585: Adding changes to unmask interrupts before task start and
 * masking them while stopping task. Masking all the external interrupts
 * in watchdog case.
 * 
 * Hydra_Software_Devel/91   3/8/12 5:00p gprasad
 * SW7435-5: Adding burst size programming for DSP 1 DMAs
 * 
 * Hydra_Software_Devel/90   3/1/12 9:54p ashoky
 * SW7231-585: go bit interface between bdsp and firmware.
 * 
 * Hydra_Software_Devel/89   3/1/12 8:49p ashoky
 * SW7231-585: Changing the RDQ implementation from DRAM based to RDB
 * based.
 * 
 * Hydra_Software_Devel/88   2/22/12 7:25p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/87   2/15/12 3:04p ashoky
 * SW7425-2308: Adding interface to put audio firmware in audio gap fill
 * mode.
 * 
 * Hydra_Software_Devel/86   2/8/12 8:19p ashoky
 * SW7346-598: Removing dynamic mem-alloc/free from decode start stop
 * time. Moving it to device  open/close.
 * 
 * Hydra_Software_Devel/85   2/3/12 7:11p ashoky
 * SW7231-628: Correcting setVideoEncodeDataSyncSetting.
 * 
 * Hydra_Software_Devel/84   2/3/12 5:18p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/83   2/3/12 11:53a ashoky
 * FWRAA-362: Raaga dsp sram optimization. Providing memory to swap
 * resident data.
 * 
 * Hydra_Software_Devel/82   1/27/12 1:20p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/81   1/17/12 2:33p gprasad
 * FWRAA-342: Fixing com table issue with speex aec
 * 
 * Hydra_Software_Devel/80   1/13/12 9:50p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/79   1/6/12 10:54a gprasad
 * SW7435-5: Adding changes for modified RDB for 7435
 * 
 * Hydra_Software_Devel/78   1/3/12 2:23p ashoky
 * SW7435-5: Adding support for 2nd DSP into mainline.
 * 
 * Hydra_Software_Devel/77   12/29/11 5:56p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/76   12/15/11 6:13p ashoky
 * SW7231-500: Changes to support IEEE1394LPCM. Making union of
 * userconfig.
 * 
 * Hydra_Software_Devel/75   12/7/11 5:23p gprasad
 * SW7425-1909: Watchdog issue while stopping Vp6 decoder - Correcting the
 * enums used for schduling mode
 * 
 * Hydra_Software_Devel/74   12/5/11 12:35p ashoky
 * FWRAA-330: WMA Encoder Development on Raaga.
 * 
 * Hydra_Software_Devel/73   11/24/11 6:52p ashoky
 * SW7429-18: Merging 7429 rdb related changes to mainline.
 * 
 * Hydra_Software_Devel/SW7429-18/3   11/23/11 12:20p jgarrett
 * SW7429-18: Merge latest changes from main branch
 * 
 * Hydra_Software_Devel/SW7429-18/2   11/4/11 5:53p gskerl
 * SW7429-18: Added conditional includes for bchp_aud_fmm_misc.h to fix
 * compile errors on 7425
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/95   3/22/12 6:09p ashoky
 * SW7346-730: Running CRC tests shows occasional software problems.
 * Correcting the DestroyMutex() call.
 * 
 * Hydra_Software_Devel/94   3/20/12 6:15p gprasad
 * SW7429-96: DSP gets "BMEM: Back guard violation" when running apex
 * 
 * Hydra_Software_Devel/93   3/19/12 2:34p gprasad
 * SW7425-2378: Adding support for Monkey audio codec
 * 
 * Hydra_Software_Devel/92   3/14/12 8:31p ashoky
 * SW7231-585: Adding changes to unmask interrupts before task start and
 * masking them while stopping task. Masking all the external interrupts
 * in watchdog case.
 * 
 * Hydra_Software_Devel/91   3/8/12 5:00p gprasad
 * SW7435-5: Adding burst size programming for DSP 1 DMAs
 * 
 * Hydra_Software_Devel/90   3/1/12 9:54p ashoky
 * SW7231-585: go bit interface between bdsp and firmware.
 * 
 * Hydra_Software_Devel/89   3/1/12 8:49p ashoky
 * SW7231-585: Changing the RDQ implementation from DRAM based to RDB
 * based.
 * 
 * Hydra_Software_Devel/88   2/22/12 7:25p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/87   2/15/12 3:04p ashoky
 * SW7425-2308: Adding interface to put audio firmware in audio gap fill
 * mode.
 * 
 * Hydra_Software_Devel/86   2/8/12 8:19p ashoky
 * SW7346-598: Removing dynamic mem-alloc/free from decode start stop
 * time. Moving it to device  open/close.
 * 
 * Hydra_Software_Devel/85   2/3/12 7:11p ashoky
 * SW7231-628: Correcting setVideoEncodeDataSyncSetting.
 * 
 * Hydra_Software_Devel/84   2/3/12 5:18p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/83   2/3/12 11:53a ashoky
 * FWRAA-362: Raaga dsp sram optimization. Providing memory to swap
 * resident data.
 * 
 * Hydra_Software_Devel/82   1/27/12 1:20p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/81   1/17/12 2:33p gprasad
 * FWRAA-342: Fixing com table issue with speex aec
 * 
 * Hydra_Software_Devel/80   1/13/12 9:50p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/79   1/6/12 10:54a gprasad
 * SW7435-5: Adding changes for modified RDB for 7435
 * 
 * Hydra_Software_Devel/78   1/3/12 2:23p ashoky
 * SW7435-5: Adding support for 2nd DSP into mainline.
 * 
 * Hydra_Software_Devel/77   12/29/11 5:56p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/76   12/15/11 6:13p ashoky
 * SW7231-500: Changes to support IEEE1394LPCM. Making union of
 * userconfig.
 * 
 * Hydra_Software_Devel/75   12/7/11 5:23p gprasad
 * SW7425-1909: Watchdog issue while stopping Vp6 decoder - Correcting the
 * enums used for schduling mode
 * 
 * Hydra_Software_Devel/74   12/5/11 12:35p ashoky
 * FWRAA-330: WMA Encoder Development on Raaga.
 * 
 * Hydra_Software_Devel/73   11/24/11 6:52p ashoky
 * SW7429-18: Merging 7429 rdb related changes to mainline.
 * 
 * Hydra_Software_Devel/SW7429-18/3   11/23/11 12:20p jgarrett
 * SW7429-18: Merge latest changes from main branch
 * 
 * Hydra_Software_Devel/72   11/16/11 5:46p gprasad
 * SW7425-1169: Confirm audio FW supports 6 simultaneous audio pass-thru
 * for transcoder
 * 
 * Hydra_Software_Devel/71   11/8/11 1:37p ramanatm
 * FWRAA-348:[7425] Vorbis bringup
 * 
 * Hydra_Software_Devel/70   10/20/11 5:43p gprasad
 * FWRAA-359: Add support for ring buffer capture for cert testing
 * 
 * Hydra_Software_Devel/69   8/30/11 12:32p gprasad
 * SWDTV-8370: 35233B0 Audio Descriptor test causes crash
 * 
 * Hydra_Software_Devel/68   8/19/11 4:02p ashoky
 * SWDTV-8271: Adding low delay api support.
 * 
 * Hydra_Software_Devel/67   8/18/11 12:53p gprasad
 * SW7422-400: Updating the APIs with review comments
 * 
 * Hydra_Software_Devel/66   8/16/11 9:26a gprasad
 * FWRAA-327: Suppressing the warning from gcc
 * 
 * Hydra_Software_Devel/64   8/13/11 11:22a gprasad
 * SW7425-914: Playback stops when performing DSOLA (FF/SLOW) for DVD-LPCM
 * playback streams
 * 
 * Hydra_Software_Devel/63   8/5/11 5:50p ananthan
 * SW7425-724: [7425] Add BTSC audio encoder support
 * 
 * Hydra_Software_Devel/62   8/4/11 10:19p gprasad
 * SW7422-371: Adding change to reset DSP at BDSP_Close
 * 
 * Hydra_Software_Devel/61   8/1/11 3:12p gprasad
 * FWRAA-327: Add support for G.711/G.726/G.729 codecs
 * 
 * Hydra_Software_Devel/60   7/28/11 2:20p gprasad
 * SW7425-468: audio transcoder generated timestamps should not restart
 * timebase before explicit stop
 * 
 * Hydra_Software_Devel/59   7/26/11 2:33p gprasad
 * SWDTV-6761: Add StudioSound support
 * 
 * Hydra_Software_Devel/58   7/12/11 5:32p gprasad
 * SW7422-372: Add macros in BDSP to help simplify the FW Settings/Status
 * interfaces
 * 
 * Hydra_Software_Devel/57   7/12/11 4:44p gprasad
 * SW7422-370: Add changes for bdsp_types.h based on code review feedback
 * 
 * Hydra_Software_Devel/55   6/30/11 11:27a jgarrett
 * SW7425-409: Resolving _isr calling convention issues with video drop
 * requests
 * 
 * Hydra_Software_Devel/54   6/30/11 9:46a jgarrett
 * SW7425-789: Programming scheduling buffer rate to match rate in CIT
 * 
 * Hydra_Software_Devel/53   6/28/11 8:19p ashoky
 * SW7425-499: Fixing issue with scheduling buffer. NRT changes were
 * affecting scheduling buffer allocation in real time cases as well.
 * 
 * Hydra_Software_Devel/52   6/24/11 2:12p gautamk
 * SW7425-499: [7425] Adding to mainline
 * 
 * Hydra_Software_Devel/51   6/22/11 3:31p gprasad
 * SW7422-370: Code review changes in bdsp_tyes.h
 * 
 * Hydra_Software_Devel/50   6/21/11 5:11p gautamk
 * SW7425-756: [7425] populating ui32DramSchedulingBuffCfgAddr correctly.
 * 
 * Hydra_Software_Devel/49   6/21/11 2:58p gautamk
 * SW7425-746: Clearing DMA Q1/2/3 bits as well.
 * 
 * Hydra_Software_Devel/48   6/21/11 2:11p gautamk
 * SW7425-756: [7405] allocating feedback buffer correctly for audio
 * descriptor channels.
 * 
 * Hydra_Software_Devel/47   6/21/11 1:35p gautamk
 * SW7422-373: [7422] correcting debug level of few debug messages
 * 
 * Hydra_Software_Devel/46   6/21/11 1:26p gautamk
 * SW7422-373: [7422] Reducing debug level of few debug messages
 * 
 * Hydra_Software_Devel/45   6/17/11 3:42p gautamk
 * SW7422-373: [7422] Adding GetInfo routines for AudioType,
 * AudioProcessing, and AudioEncode support in BDSP
 * 
 * Hydra_Software_Devel/43   6/16/11 6:03p gautamk
 * SW7422-369: [7422] Adding support for mulitple context for watchdog
 * 
 * Hydra_Software_Devel/42   6/16/11 2:39p gautamk
 * SW7425-732: [7425] Handling return code from
 * BDSP_Raaga_P_CopyFWImageToMem
 * 
 * Hydra_Software_Devel/41   6/15/11 6:05p gautamk
 * SW7422-357 : [7425] assigning settings in Contexthandle at correct
 * place.
 * 
 * Hydra_Software_Devel/40   6/15/11 5:54p gautamk
 * SW7425-683: [7425] Adding support for Power management in BDSP
 * 
 * Hydra_Software_Devel/39   6/15/11 4:17p gautamk
 * SW7422-357 : [7425] Adding support for new API for getStageInputIndex
 * 
 * Hydra_Software_Devel/38   6/15/11 12:13a gautamk
 * SW7425-406: Fixing Coverity issue
 * 
 * Hydra_Software_Devel/37   6/14/11 10:23a jgarrett
 * SW7425-406: Coverity CID 421, 422, 430, 431, 432, 433
 * 
 * Hydra_Software_Devel/36   6/14/11 8:38p srajapur
 * SW7422-357 : [7425] correcting the ContextInterruptUninstall call in
 * Destroy context.
 * 
 * Hydra_Software_Devel/35   6/14/11 5:50p gautamk
 * SW7422-357 : [7425] passing physical offset of
 * pIoBuffer/pIoGenericBuffer to SIPS function. Review this if we agree
 * to send logical address to SIPS.
 * 
 * Hydra_Software_Devel/34   6/14/11 3:05p gautamk
 * SW7422-357 : [7425] Merging to mainline
 * 
 * Hydra_Software_Devel/33   6/8/11 5:13p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/32   6/3/11 11:59a srajapur
 * SW7422-357 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/31   6/2/11 8:08p srajapur
 * SW7422-358 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/30   5/31/11 5:38p srajapur
 * SW7422-358 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/29   5/28/11 10:53p srajapur
 * SW7422-358 : [7425] Added support for MS11 DDRE and DV258 to BDSP
 * 
 * Hydra_Software_Devel/28   5/27/11 4:45p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/27   5/27/11 11:31a srajapur
 * SW7422-358 : [7425] Corrected default DDRE user config values
 * 
 * Hydra_Software_Devel/26   5/25/11 5:21p srajapur
 * SW7422-358: [7425] Adding support for MS11  in BDSP
 *  
 * Hydra_Software_Devel/25   5/25/11 12:28p srajapur
 * SW7425-572: [7425] Adding VP6 support in BDSP
 * 
 * Hydra_Software_Devel/24   5/20/11 7:05p srajapur
 * SW7422-358: [7425] Added support for MS11  in BDSP
 * 
 * Hydra_Software_Devel/23   5/20/11 12:01p srajapur
 * SW7422-358: [7425] Added support for MS11 DDRE, FWmixer in BDSP
 * 
 * Hydra_Software_Devel/22   5/20/11 11:09a srajapur
 * SWDTV-6763: Adding AVL/ABX/ADV/3DSurround support to mainline
 * 
 * Hydra_Software_Devel/21   5/19/11 11:46p srajapur
 * SW7425-477 : [7425] Adding mp3 audio encode support
 * 
 * Hydra_Software_Devel/20   5/19/11 10:22a srajapur
 * SW7231-145 : [7231] Adding BCHP_PWR support to BDSP
 * 
 * Hydra_Software_Devel/19   5/18/11 9:49p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/18   5/18/11 6:43p ashoky
 * SW7425-477: [7425]Adding mp3enc default user parameters.
 * 
 * Hydra_Software_Devel/17   5/18/11 5:45p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/16   5/17/11 3:06p srajapur
 * SW7425-588 : [7425] Correcting AVL Default settings
 * 
 * Hydra_Software_Devel/15   5/17/11 10:51a srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/14   5/16/11 10:08p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/13   5/13/11 9:31a gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/12   5/12/11 5:36p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/11   5/11/11 5:31p srajapur
 * SW7422-374 : [7425] Incorporate changes to BDSP "details" structures
 * from code review feedback.
 * 
 * Hydra_Software_Devel/8   4/15/11 5:21p srajapur
 * SW7425-342: [7425] DSP: Build fails if MS10 support is disabled
 * 
 * Hydra_Software_Devel/7   4/15/11 2:10p srajapur
 * SW7425-363 : [7425] Adding AMR algorithm support in BDSP
 * 
 * Hydra_Software_Devel/6   4/14/11 6:53p srajapur
 * SW7425-291: [7425] BDSP directory structure changes
 * 
 * Hydra_Software_Devel/5   4/14/11 6:24p srajapur
 * SW7425-291: [7425] BDSP directory structure changes
 * 
 * Hydra_Software_Devel/4   4/14/11 5:35p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/3   4/12/11 2:24p srajapur
 * SW7422-419:[7425]Adding BDSP_Raaga_GetFwDwnldInfo and
 * BDSP_Raaga_StartDsp API to support authentication
 * 
 * Hydra_Software_Devel/2   4/6/11 2:45p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   4/6/11 2:28p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/37   3/28/11 3:53p srajapur
 * SW7422-356 : [7425] Added support forGenCbdItb support ---->  Fixed the
 * issue with downloading of firmware files.
 * 
 * Hydra_Software_Devel/36   3/22/11 6:41p jgarrett
 * SW7422-402: Fixing reset of pause state during advance command
 * 
 * Hydra_Software_Devel/35   3/21/11 7:06p jgarrett
 * SW7422-356: Correcting type names for GenCdbItb
 * 
 * Hydra_Software_Devel/34   3/15/11 12:05p gskerl
 * SW7422-146: Moved external declarations from other .c files into
 * bdsp_raaga_priv.h
 * 
 * Hydra_Software_Devel/33   3/15/11 9:22p srajapur
 * SW7422-356 : [7425] Added support for AACHE encode and GenCbdItb
 * support.
 * 
 * Hydra_Software_Devel/32   3/15/11 8:54p srajapur
 * SW7422-355: [7422] Added DTS Encode support.
 * 
 * Hydra_Software_Devel/31   3/15/11 7:38p srajapur
 * SW7422-355: [7422] Added AC3 Encode support.
 * 
 * Hydra_Software_Devel/30   3/14/11 6:02p jgarrett
 * SW7422-351: Fixing user configuration for PCM audio type
 * 
 * Hydra_Software_Devel/29   3/9/11 7:12a srajapur
 * SW7422-229:[7422]Added support for MS10 algorithm
 * 
 * Hydra_Software_Devel/28   3/8/11 9:14a srajapur
 * SW7422-294 : [7422]Adding watchdog recovery support in BASE DSP
 * 
 * Hydra_Software_Devel/27   3/8/11 12:01a srajapur
 * SW7422-229:[7422]Added support for MS10 algorithm
 * 
 * Hydra_Software_Devel/26   3/5/11 4:56a srajapur
 * SW7422-317 : [7422] Clearing the Token ID bits of DSP DMA register
 * SW7422-314 : [7422] Adding RDB Independence Feature in BASE DSP
 * Codebase
 * 
 * Hydra_Software_Devel/25   3/4/11 2:21p jgarrett
 * SW7422-146: Finding scheduling buffer in CIT
 * 
 * Hydra_Software_Devel/24   3/4/11 3:17a srajapur
 * SW7422-247 : [7422] Added debug messages to print command sent to
 * firmware
 * 
 * Hydra_Software_Devel/23   3/2/11 8:00a srajapur
 * SW7422-247 : [7422] Added correct default post processing values
 * 
 * Hydra_Software_Devel/22   2/26/11 2:01a srajapur
 * SW7422-294 :[7422]  Adding watchdog recovery API support
 * 
 * Hydra_Software_Devel/21   2/26/11 12:41a srajapur
 * SW7422-293:[7422] Adding support for encode algorithms
 * 
 * Hydra_Software_Devel/20   2/16/11 5:38a srajapur
 * SW7422-247 : [7422] Adding all audio post processing algorithms.
 * 
 * Hydra_Software_Devel/19   2/10/11 7:00a srajapur
 * SW7422-229:[7422]Added support for follow audio codec algorithms
 * ========== WMA - tested WMAPRO - tested DRA - tested DTSHD - tested
 * DTSBROADCAST - tested LPCMDVD - tested PCMWAV PCM REALAUDIOLBR ADPCM
 * ==========
 * 
 * Hydra_Software_Devel/18   2/5/11 4:24a srajapur
 * SW7422-247 : [7422] Adding Generic passthrul audio processing support.
 * 
 * Hydra_Software_Devel/17   1/29/11 5:02a srajapur
 * SW7422-229 :[7422]Adding support for AAC, AACLOAS, AACSBR and
 * AACSBRADTS algorithm.
 * 
 * Hydra_Software_Devel/16   1/29/11 1:23a srajapur
 * SW7422-229 : [7422]Adding support for DDP decode.
 * 
 * Hydra_Software_Devel/15   1/26/11 3:18a gautamk
 * SW7422-191: Allocating FW debug buffer on basis of runtime Parrameter
 * enableDebugLog of BDSP_RaagaSettings.
 * 
 * Hydra_Software_Devel/14   1/26/11 12:53a gautamk
 * SW7422-191: Adding error recovery code for create task.
 * 
 * Hydra_Software_Devel/13   1/25/11 5:34a gautamk
 * SW7422-191: Changes in TSM config and status structure
 * 
 * Hydra_Software_Devel/12   1/25/11 4:02a gautamk
 * SW7422-191: Adding error recovery in Create Context
 * 
 * Hydra_Software_Devel/11   1/25/11 2:23a gautamk
 * SW7422-191:Adding error recovery code for Raaga Open
 * 
 * Hydra_Software_Devel/10   1/21/11 5:37a gautamk
 * SW7422-191: Fixing bug in Writemsg_isr and StopTask.
 * 
 * Hydra_Software_Devel/9   1/20/11 8:12a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/8   1/19/11 7:16a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/7   1/19/11 2:15a gautamk
 * SW7422-191: putting all the Details of Audio codec in a const array and
 * then using it across the code.
 * 
 * Hydra_Software_Devel/6   1/18/11 5:58a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/5   1/18/11 4:55a gautamk
 * SW7422-191: Implementing code review comments.
 * 
 * Hydra_Software_Devel/4   1/18/11 4:19a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/3   1/15/11 3:33a gautamk
 * SW7422-191:[7422] CLeaning up unwanted debug message
 * 
 * Hydra_Software_Devel/2   1/13/11 5:26a gautamk
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
#include "bdsp_raaga_priv.h"
#include "bdsp_raaga_fw_cit.h"
#include "bdsp_raaga_fwdownload_priv.h"
#include "bdsp_raaga_int_priv.h"
#include "bdsp_raaga_fw.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_aud_fmm_bf_ctrl.h"

#ifdef  BCHP_AUD_FMM_MISC_REG_START
#include "bchp_aud_fmm_misc.h"
#endif

#ifdef BCHP_RAAGA_DSP_MISC_1_REG_START
#include "bchp_raaga_dsp_misc_1.h"
#endif 

#ifdef BCHP_AUD_MISC_REG_START
#include "bchp_aud_misc.h"
#endif

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

BDBG_MODULE(bdsp_raaga_priv);

BDBG_OBJECT_ID(BDSP_Raaga);
BDBG_OBJECT_ID(BDSP_RaagaContext);
BDBG_OBJECT_ID(BDSP_RaagaTask);
BDBG_OBJECT_ID(BDSP_RaagaTaskStageInput);
BDBG_OBJECT_ID(BDSP_RaagaExternalInterrupt);
BDBG_OBJECT_ID(BDSP_RaagaRdbRegister);
BDBG_OBJECT_ID(BDSP_RaagaCapture);



/*****************************************************************************/
/**************Audio Codec structure details ****************************/
/*****************************************************************************/

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sMpegCodecDetails =
{/*MPEG Details*/
    BDSP_AudioType_eMpeg,
    "MPEG",
#ifdef BDSP_MPEG_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_MPEG_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sMpegDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_MpegConfigParams),
    sizeof(BDSP_Raaga_Audio_MpegStreamInfo),
#ifdef BDSP_MPEG_SUPPORT        
    {BDSP_Img_Id_eMpeg1_Ids,BDSP_Img_Id_eMpeg1_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eMpeg1_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eMpeg1_Ids_Interframe,BDSP_Img_Id_eMpeg1_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_MPEG_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eMpeg1_Ids,
        BDSP_Img_Id_eMpeg1_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAacCodecDetails =
{/*AAC Details*/
    BDSP_AudioType_eAacAdts,
    "AAC",
#ifdef BDSP_AACSBR_SUPPORT        
    true,
#else
    false,
#endif    
#ifdef  BDSP_AACSBR_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif
#ifdef BDSP_MS10_SUPPORT
    (void *)&BDSP_sDolbyPulseDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DolbyPulseUserConfig),
    sizeof(BDSP_Raaga_Audio_DolbyPulseStreamInfo),
#else
    (void *)&BDSP_sAacheDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_AacheConfigParams),
    sizeof(BDSP_Raaga_Audio_AacheStreamInfo),
#endif        
#ifdef BDSP_MS10_SUPPORT
    {BDSP_IMG_Id_eAache_Adts_Ids,BDSP_IMG_Id_eDolbyPulse_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_IMG_Id_eDolbyPulse_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_IMG_Id_eAache_Adts_Ids_Interframe,BDSP_IMG_Id_eDolbyPulse_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#elif BDSP_AACSBR_SUPPORT
    {BDSP_IMG_Id_eAache_Adts_Ids,BDSP_IMG_Id_eAache_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_IMG_Id_eAache_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_IMG_Id_eAache_Adts_Ids_Interframe,BDSP_IMG_Id_eAache_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif  
#ifdef BDSP_AACSBR_SUPPORT       
#ifdef BDSP_MS10_SUPPORT
        BDSP_IMG_Id_eAache_Adts_Ids,
        BDSP_IMG_Id_eAache_Adts_Ids_Interframe,
#else
        BDSP_IMG_Id_eAache_Adts_Ids,
        BDSP_IMG_Id_eAache_Adts_Ids_Interframe,
#endif                
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};


static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAacLoasCodecDetails =
{/*AAC LOAS Details*/
    BDSP_AudioType_eAacLoas,
    "AAC_LOAS",
#ifdef BDSP_AACSBR_SUPPORT        
    true,
#else
    false,
#endif 
#ifdef  BDSP_AACSBR_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif
#ifdef BDSP_MS10_SUPPORT
    (void *)&BDSP_sDolbyPulseDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DolbyPulseUserConfig),
    sizeof(BDSP_Raaga_Audio_DolbyPulseStreamInfo),
#else
    (void *)&BDSP_sAacheDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_AacheConfigParams),
    sizeof(BDSP_Raaga_Audio_AacheStreamInfo),
#endif        
#ifdef BDSP_MS10_SUPPORT
    {BDSP_IMG_Id_eAache_Loas_Ids,BDSP_IMG_Id_eDolbyPulse_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_IMG_Id_eDolbyPulse_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_IMG_Id_eAache_Loas_Ids_Interframe,BDSP_IMG_Id_eDolbyPulse_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#elif BDSP_AACSBR_SUPPORT
    {BDSP_IMG_Id_eAache_Loas_Ids,BDSP_IMG_Id_eAache_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_IMG_Id_eAache_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_IMG_Id_eAache_Loas_Ids_Interframe,BDSP_IMG_Id_eAache_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif  
#ifdef BDSP_AACSBR_SUPPORT       
#ifdef BDSP_MS10_SUPPORT
        BDSP_IMG_Id_eAache_Loas_Ids,
        BDSP_IMG_Id_eAache_Loas_Ids_Interframe,
#else
        BDSP_IMG_Id_eAache_Loas_Ids,
        BDSP_IMG_Id_eAache_Loas_Ids_Interframe,
#endif                
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAacSbrAdtsCodecDetails =
{/*AACSBR ADTS Details*/
    BDSP_AudioType_eAacSbrAdts,
    "AAC_SBR_ADTS",
#ifdef BDSP_AACSBR_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_AACSBR_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif
#ifdef BDSP_MS10_SUPPORT
    (void *)&BDSP_sDolbyPulseDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DolbyPulseUserConfig),
    sizeof(BDSP_Raaga_Audio_DolbyPulseStreamInfo),
#else
    (void *)&BDSP_sAacheDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_AacheConfigParams),
    sizeof(BDSP_Raaga_Audio_AacheStreamInfo),
#endif        
#ifdef BDSP_MS10_SUPPORT
    {BDSP_IMG_Id_eAache_Adts_Ids,BDSP_IMG_Id_eDolbyPulse_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_IMG_Id_eDolbyPulse_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_IMG_Id_eAache_Adts_Ids_Interframe,BDSP_IMG_Id_eDolbyPulse_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#elif BDSP_AACSBR_SUPPORT
    {BDSP_IMG_Id_eAache_Adts_Ids,BDSP_IMG_Id_eAache_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_IMG_Id_eAache_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_IMG_Id_eAache_Adts_Ids_Interframe,BDSP_IMG_Id_eAache_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif  
#ifdef BDSP_AACSBR_SUPPORT       
#ifdef BDSP_MS10_SUPPORT
        BDSP_IMG_Id_eAache_Adts_Ids,
        BDSP_IMG_Id_eAache_Adts_Ids_Interframe,
#else
        BDSP_IMG_Id_eAache_Adts_Ids,
        BDSP_IMG_Id_eAache_Adts_Ids_Interframe,
#endif                
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAacSbrLoasCodecDetails =
{/*AACSBR LOAS Details*/
    BDSP_AudioType_eAacSbrLoas,
    "AAC_SBR_LOAS",
#ifdef BDSP_AACSBR_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_AACSBR_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif
#ifdef BDSP_MS10_SUPPORT
    (void *)&BDSP_sDolbyPulseDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DolbyPulseUserConfig),
    sizeof(BDSP_Raaga_Audio_DolbyPulseStreamInfo),
#else
    (void *)&BDSP_sAacheDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_AacheConfigParams),
    sizeof(BDSP_Raaga_Audio_AacheStreamInfo),
#endif        
#ifdef BDSP_MS10_SUPPORT
    {BDSP_IMG_Id_eAache_Loas_Ids,BDSP_IMG_Id_eDolbyPulse_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_IMG_Id_eDolbyPulse_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_IMG_Id_eAache_Loas_Ids_Interframe,BDSP_IMG_Id_eDolbyPulse_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#elif BDSP_AACSBR_SUPPORT
    {BDSP_IMG_Id_eAache_Loas_Ids,BDSP_IMG_Id_eAache_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_IMG_Id_eAache_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_IMG_Id_eAache_Loas_Ids_Interframe,BDSP_IMG_Id_eAache_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif  
#ifdef BDSP_AACSBR_SUPPORT       
#ifdef BDSP_MS10_SUPPORT
        BDSP_IMG_Id_eAache_Loas_Ids,
        BDSP_IMG_Id_eAache_Loas_Ids_Interframe,
#else
        BDSP_IMG_Id_eAache_Loas_Ids,
        BDSP_IMG_Id_eAache_Loas_Ids_Interframe,
#endif                
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAc3CodecDetails =
{/*AC3 Details*/
    BDSP_AudioType_eAc3,
    "AC3",
#ifdef BDSP_AC3_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_AC3_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif
    (void *)&BDSP_sDDPDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DDPMultiStreamConfigParams),
    sizeof(BDSP_Raaga_Audio_MultiStreamDDPStreamInfo),
#ifdef BDSP_MS10_SUPPORT
    {BDSP_Img_Id_eDdp_Ids,BDSP_Img_Id_eMsDdp_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eMsDdp_Decode_Tables,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eDdp_Ids_Interframe,BDSP_Img_Id_eMsDdp_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#elif BDSP_DDP_SUPPORT
    {BDSP_Img_Id_eDdp_Ids,BDSP_Img_Id_eDdp_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDdp_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eDdp_Ids_Interframe,BDSP_Img_Id_eDdp_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#elif BDSP_AC3_SUPPORT
    {BDSP_Img_Id_eAc3_Ids,BDSP_Img_Id_eAc3_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAc3_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eAc3_Ids_Interframe,BDSP_Img_Id_eAc3_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},        
#endif
#ifdef BDSP_MS10_SUPPORT 
        BDSP_Img_Id_eDdp_Ids,
        BDSP_Img_Id_eDdp_Ids_Interframe,
#elif BDSP_DDP_PASSTHRU_SUPPORT
        BDSP_Img_Id_eDdp_Ids,
        BDSP_Img_Id_eDdp_Ids_Interframe,
#elif BDSP_AC3_PASSTHRU_SUPPORT
        BDSP_Img_Id_eAc3_Ids,
        BDSP_Img_Id_eAc3_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAc3PlusCodecDetails =
{/*AC3 PLUS Details*/
    BDSP_AudioType_eAc3Plus,
    "AC3Plus",
#ifdef BDSP_DDP_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_DDP_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif
    (void *)&BDSP_sDDPDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DDPMultiStreamConfigParams),
    sizeof(BDSP_Raaga_Audio_MultiStreamDDPStreamInfo),
#ifdef BDSP_MS10_SUPPORT
    {BDSP_Img_Id_eDdp_Ids,BDSP_Img_Id_eMsDdp_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eMsDdp_Decode_Tables,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eDdp_Ids_Interframe,BDSP_Img_Id_eMsDdp_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#elif BDSP_DDP_SUPPORT
    {BDSP_Img_Id_eDdp_Ids,BDSP_Img_Id_eDdp_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDdp_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eDdp_Ids_Interframe,BDSP_Img_Id_eDdp_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif  
#ifdef BDSP_DDP_PASSTHRU_SUPPORT       
        BDSP_Img_Id_eDdp_Ids,
        BDSP_Img_Id_eDdp_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sLpcmBdCodecDetails =
{/*BDSP_AudioType_eLpcmBd Details*/
    BDSP_AudioType_eLpcmBd,
    "",
    false,
    false,                
    (void *)NULL,
    0,
    0,
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sLpcmHdDvdCodecDetails =
{/*BDSP_AudioType_eLpcmHdDvd Details*/
    BDSP_AudioType_eLpcmHdDvd,
    "",
    false,
    false,                
    (void *)NULL,
    0,
    0,
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sDtshdCodecDetails =
{/*BDSP_AudioType_eDtshd*/
    BDSP_AudioType_eDtshd,
    "DTSHD",
#ifdef BDSP_DTSHD_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_DTSHD_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sDtsHdDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DtsHdConfigParams),
    sizeof(BDSP_Raaga_Audio_DtsHdStreamInfo),
#ifdef BDSP_DTSHD_SUPPORT        
    {BDSP_Img_Id_eDtsHd_Ids,BDSP_Img_Id_eDtsBroadcast_Decode_Stg,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDtsBroadcast_Decode_Table_Stg,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eDtsHd_Ids_Interframe,BDSP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_DTSHD_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eDtsHd_Ids,
        BDSP_Img_Id_eDtsHd_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sLpcmDvdCodecDetails =
{/*BDSP_AudioType_eLpcmDvd*/
    BDSP_AudioType_eLpcmDvd,
    "LPCMDVD",
#ifdef BDSP_LPCMDVD_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_LPCMDVD_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sLcpmDvdDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_LpcmUserConfig),
    sizeof(BDSP_Raaga_Audio_LpcmStreamInfo),
#ifdef BDSP_LPCMDVD_SUPPORT        
    {BDSP_Img_Id_eLpcmDvd_Ids,BDSP_Img_Id_eLpcmDvd_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eLpcmDvd_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eLpcmDvd_Ids_Interframe,BDSP_Img_Id_eLpcmDvd_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_LPCMDVD_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eLpcmDvd_Ids,
        BDSP_Img_Id_eLpcmDvd_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sWmaStdCodecDetails =
{/*BDSP_AudioType_eWmaStd*/
    BDSP_AudioType_eWmaStd,
    "WMA",
#ifdef BDSP_WMA_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_WMA_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sWmaDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_WmaConfigParams),
    sizeof(BDSP_Raaga_Audio_WmaStreamInfo),
#ifdef BDSP_WMA_SUPPORT        
    {BDSP_Img_Id_eWma_Ids,BDSP_Img_Id_eWma_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eWma_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eWma_Ids_Interframe,BDSP_Img_Id_eWma_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_WMA_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eWma_Ids,
        BDSP_Img_Id_eWma_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};


static const BDSP_Raaga_P_AudioCodecPTDetails BDSP_sMlpPTDetails =
{
    BDSP_Img_Id_eMLP_Passthrough_Code,
    BDSP_Img_Id_eMLP_Passthrough_Table,
    BDSP_Img_Id_eMLP_Passthrough_Interframe
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sMlpCodecDetails =
{/*BDSP_AudioType_eMlp Details*/
    BDSP_AudioType_eMlp,
    "Dolby TrueHD",
#ifdef BDSP_MLP_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_MLP_PASSTHROUGH_SUPPORT                               
    true,
#else
    false,                
#endif        
    (void *)&BDSP_sMlpUserConfig,
    sizeof(BDSP_Raaga_Audio_MlpConfigParams),
    sizeof(BDSP_Raaga_Audio_MlpStreamInfo),
#ifdef BDSP_MLP_SUPPORT        
    {BDSP_Img_Id_eMlp_Ids,BDSP_Img_Id_eTrueHD_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eTrueHD_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eMlp_Ids_Interframe,BDSP_Img_Id_eTrueHD_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
#endif
#ifdef BDSP_MLP_PASSTHROUGH_SUPPORT        
        BDSP_Img_Id_eMlp_Ids,
        BDSP_Img_Id_eMlp_Ids_Interframe,
	    &BDSP_sMlpPTDetails
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
        NULL
#endif
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sPcmCodecDetails =
{/*BDSP_AudioType_ePcm Details*/
    BDSP_AudioType_ePcm,
    "",
#ifdef BDSP_PCM_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_PCM_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sPcmDefaultUserConfig,
    sizeof(BDSP_sPcmDefaultUserConfig),
    0,
#ifdef BDSP_PCM_SUPPORT        
    {BDSP_Img_Id_ePcm_Ids,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_ePcm_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_PCM_PASSTHRU_SUPPORT        
        BDSP_Img_Id_ePcm_Ids,
        BDSP_Img_Id_ePcm_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sDtsLbrCodecDetails =
{/*BDSP_AudioType_eDtsLbr Details*/
    BDSP_AudioType_eDtsLbr,
    "DTSLBR",
#ifdef BDSP_DTSLBR_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_DTSLBR_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sDtsLbrDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DtslbrConfigParams),
    sizeof(BDSP_Raaga_Audio_DtslbrStreamInfo),
#ifdef BDSP_DTSLBR_SUPPORT        
    {BDSP_Img_Id_eDtsLbr_Ids,BDSP_Img_Id_eDtsLbr_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDtsLbr_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDtsLbr_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid}, 
#endif
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
    NULL
};


static const BDSP_Raaga_P_AudioCodecDetails BDSP_sDdp7_1CodecDetails =
{/*BDSP_AudioType_eDdp7_1 Details*/
    BDSP_AudioType_eDdp7_1,
    "",
    false,
    false,                
    (void *)NULL,
    0,
    0,
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sMpegMcCodecDetails =
{/*BDSP_AudioType_eMpegMc Details*/
    BDSP_AudioType_eMpegMc,
    "",
    false,
    false,                
    (void *)NULL,
    0,
    0,
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sWmaProCodecDetails =
{/*BDSP_AudioType_eWmaPro Details*/
    BDSP_AudioType_eWmaPro,
    "WMAPRO",
#ifdef BDSP_WMAPRO_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_WMAPRO_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sWmaProDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_WmaProConfigParams),
    sizeof(BDSP_Raaga_Audio_WmaProStreamInfo),
#ifdef BDSP_WMAPRO_SUPPORT        
    {BDSP_Img_Id_eWmaPro_Ids,BDSP_Img_Id_eWmaPro_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eWmaPro_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eWmaPro_Ids_Interframe,BDSP_Img_Id_eWmaPro_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_WMAPRO_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eWmaPro_Ids,
        BDSP_Img_Id_eWmaPro_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sLpcmDvdACodecDetails =
{/*BDSP_AudioType_eLpcmDvdA Details*/
    BDSP_AudioType_eLpcmDvdA,
    "",
    false,
    false,                
    (void *)NULL,
    0,
    0,
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sDtsBroadcastCodecDetails =
{/*BDSP_AudioType_eDtsBroadcast Details*/
    BDSP_AudioType_eDtsBroadcast,
    "DTSBROADCAST",
#ifdef BDSP_DTSBROADCAST_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_DTSBROADCAST_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sDtsHdDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DtsHdConfigParams),
    sizeof(BDSP_Raaga_Audio_DtsHdStreamInfo),
#ifdef BDSP_DTSBROADCAST_SUPPORT        
    {BDSP_Img_Id_eDtsBroadcast_Ids,BDSP_Img_Id_eDtsBroadcast_Decode_Stg,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDtsBroadcast_Decode_Table_Stg,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eDtsBroadcast_Ids_Interframe,BDSP_Img_Id_eDtsBroadcast_Decode_Interframe_Stg,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_DTSBROADCAST_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eDtsBroadcast_Ids,
        BDSP_Img_Id_eDtsBroadcast_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sPcmWavCodecDetails =
{/*BDSP_AudioType_ePcmWav Details*/
    BDSP_AudioType_ePcmWav,
    "PCMWAV",
#ifdef BDSP_PCMWAV_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_PCMWAV_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sPcmWavDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_PcmWavConfigParams),
    sizeof(BDSP_Raaga_Audio_PcmWavStreamInfo),
#ifdef BDSP_PCMWAV_SUPPORT        
    {BDSP_Img_Id_ePcmWav_Ids,BDSP_Img_Id_ePcmWav_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_ePcmWav_Ids_Interframe,BDSP_Img_Id_ePcmWav_Code_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_PCMWAV_PASSTHRU_SUPPORT        
        BDSP_Img_Id_ePcmWav_Ids,
        BDSP_Img_Id_ePcmWav_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAmrCodecDetails =
{/*BDSP_AudioType_eAmr Details*/
    BDSP_AudioType_eAmr,
    "AMR",
#ifdef BDSP_AMR_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_AMR_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sAmrDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_AmrConfigParams),
    sizeof(BDSP_Raaga_Audio_AmrStreamInfo),
#ifdef BDSP_AMR_SUPPORT        
    {BDSP_Img_Id_eAMR_Ids,BDSP_Img_Id_eAMR_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAMR_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eAMR_Ids_Interframe,BDSP_Img_Id_eAMR_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_AMR_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eAMR_Ids,
        BDSP_Img_Id_eAMR_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sDraCodecDetails =
{/*BDSP_AudioType_eDra Details*/
    BDSP_AudioType_eDra,
    "DRA",
#ifdef BDSP_DRA_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_DRA_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sDraDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_DraConfigParams),
    sizeof(BDSP_Raaga_Audio_DraStreamInfo),
#ifdef BDSP_DRA_SUPPORT        
    {BDSP_Img_Id_eDRA_Ids,BDSP_Img_Id_eDRA_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDRA_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eDRA_Ids_Interframe,BDSP_Img_Id_eDRA_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_DRA_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eDRA_Ids,
        BDSP_Img_Id_eDRA_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sRealAudioLbrCodecDetails =
{/*BDSP_AudioType_eRealAudioLbr Details*/
    BDSP_AudioType_eRealAudioLbr,
    "REALAUDIOLBR",
#ifdef BDSP_REALAUDIOLBR_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_REALAUDIOLBR_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sRalbrDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_RalbrConfigParams),
    sizeof(BDSP_Raaga_Audio_RalbrStreamInfo),
#ifdef BDSP_REALAUDIOLBR_SUPPORT        
    {BDSP_Img_Id_eRealAudioLbr_Ids,BDSP_Img_Id_eRealAudioLbr_Decode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eRealAudioLbr_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eRealAudioLbr_Ids_Interframe,BDSP_Img_Id_eRealAudioLbr_Decode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_REALAUDIOLBR_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eRealAudioLbr_Ids,
        BDSP_Img_Id_eRealAudioLbr_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAdpcmCodecDetails =
{/*BDSP_AudioType_eAdpcm Details*/
    BDSP_AudioType_eAdpcm,
    "ADPCM",
#ifdef BDSP_ADPCM_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_ADPCM_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sAdpcmDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_AdpcmConfigParams),
    sizeof(BDSP_Raaga_Audio_AdpcmStreamInfo),
#ifdef BDSP_ADPCM_SUPPORT        
    {BDSP_Img_Id_eAdpcm_Ids,BDSP_Img_Id_eAdpcm_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAdpcm_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eAdpcm_Ids_Interframe,BDSP_Img_Id_eAdpcm_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_ADPCM_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eAdpcm_Ids,
        BDSP_Img_Id_eAdpcm_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};


static const BDSP_Raaga_P_AudioCodecDetails BDSP_sG711G726CodecDetails =
{/*BDSP_AudioType_eG711G726 Details*/
    BDSP_AudioType_eG711G726,
    "G.711/G.726",
#ifdef BDSP_G711G726_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_G711G726_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sG711G726DecUserConfig,
    sizeof(BDSP_Raaga_Audio_G726ConfigParams),
    sizeof(BDSP_Raaga_Audio_G726StreamInfo),
#ifdef BDSP_G711G726_SUPPORT        
    {BDSP_Img_Id_eG711G726_Ids,BDSP_Img_Id_eG711G726_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG711G726_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eG711G726_Ids_Interframe,BDSP_Img_Id_eG711G726_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_G711G726_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eG711G726_Ids,
        BDSP_Img_Id_eG711G726_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};



static const BDSP_Raaga_P_AudioCodecDetails BDSP_sG729CodecDetails =
{/*BDSP_AudioType_eG729 Details*/
    BDSP_AudioType_eG729,
    "G.729",
#ifdef BDSP_G729_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_G729_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sG729DecUserConfig,
    sizeof(BDSP_Raaga_Audio_G729DecConfigParams),
    sizeof(BDSP_Raaga_Audio_G729DecStreamInfo),
#ifdef BDSP_G729_SUPPORT        
    {BDSP_Img_Id_eG729_Ids,BDSP_Img_Id_eG729_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG729_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eG729_Ids_Interframe,BDSP_Img_Id_eG729_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_G729_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eG729_Ids,
        BDSP_Img_Id_eG729_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};


static const BDSP_Raaga_P_AudioCodecDetails BDSP_sG723_1CodecDetails =
{/*BDSP_AudioType_eG723_1 Details*/
    BDSP_AudioType_eG723_1,
    "G.723.1",
#ifdef BDSP_G723_1_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_G723_1_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sG723_1_Configsettings,
    sizeof(BDSP_Raaga_Audio_G723_1DEC_ConfigParams),
    sizeof(BDSP_Raaga_Audio_G723_1_StreamInfo),
#ifdef BDSP_G723_1_SUPPORT        
    {BDSP_Img_Id_eG723_1_Ids,BDSP_Img_Id_eG723_1_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG723_1_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eG723_1_Ids_Interframe,BDSP_Img_Id_eG723_1_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_G723_1_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eG723_1_Ids,
        BDSP_Img_Id_eG723_1_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};


static const BDSP_Raaga_P_AudioCodecDetails BDSP_sVorbisCodecDetails =
{/*BDSP_AudioType_eVorbis Details*/
    BDSP_AudioType_eVorbis,
    "VORBIS",
#ifdef BDSP_VORBIS_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_VORBIS_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sVorbisDecUserConfig,
    sizeof(BDSP_Raaga_Audio_VorbisDecConfigParams),
    sizeof(BDSP_Raaga_Audio_VorbisDecStreamInfo),
#ifdef BDSP_VORBIS_SUPPORT        
    {BDSP_Img_Id_eVorbis_Ids,BDSP_Img_Id_eVorbis_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eVorbis_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eVorbis_Ids_Interframe,BDSP_Img_Id_eVorbis_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_VORBIS_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eVorbis_Ids,
        BDSP_Img_Id_eVorbis_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};


static const BDSP_Raaga_P_AudioCodecDetails BDSP_sFlacCodecDetails =
{/*BDSP_AudioType_eFlac Details*/
    BDSP_AudioType_eFlac,
    "FLAC",
#ifdef BDSP_FLAC_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_FLAC_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sFlacDecUserConfig,
    sizeof(BDSP_Raaga_Audio_FlacDecConfigParams),
    sizeof(BDSP_Raaga_Audio_FlacDecStreamInfo),
#ifdef BDSP_FLAC_SUPPORT        
    {BDSP_Img_Id_eFlac_Ids,BDSP_Img_Id_eFlac_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eFlac_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eFlac_Ids_Interframe,BDSP_Img_Id_eFlac_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_FLAC_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eFlac_Ids,
        BDSP_Img_Id_eFlac_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sMacCodecDetails =
{/*BDSP_AudioType_eMac Details*/
    BDSP_AudioType_eMac,
    "MAC",
#ifdef BDSP_MAC_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_MAC_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sMacDecUserConfig,
    sizeof(BDSP_Raaga_Audio_MacDecConfigParams),
    sizeof(BDSP_Raaga_Audio_MacDecStreamInfo),
#ifdef BDSP_MAC_SUPPORT        
    {BDSP_Img_Id_eMac_Ids,BDSP_Img_Id_eMac_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eMac_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eMac_Ids_Interframe,BDSP_Img_Id_eMac_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_MAC_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eMac_Ids,
        BDSP_Img_Id_eMac_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_sAmrwbCodecDetails =
{/*BDSP_AudioType_eAmrwb Details*/
    BDSP_AudioType_eAmrwb,
    "AMRWB",
#ifdef BDSP_AMRWB_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_AMRWB_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_sAmrwbdecDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_AmrwbdecConfigParams),
    sizeof(BDSP_Raaga_Audio_AmrWbStreamInfo),
#ifdef BDSP_AMRWB_SUPPORT        
    {BDSP_Img_Id_eAMRWB_Ids,BDSP_Img_Id_eAMRWB_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAMRWB_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eAMRWB_Ids_Interframe,BDSP_Img_Id_eAMRWB_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_AMRWB_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eAMRWB_Ids,
        BDSP_Img_Id_eAMRWB_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

static const BDSP_Raaga_P_AudioCodecDetails BDSP_siLBCCodecDetails =
{/*BDSP_AudioType_eiLBC Details*/
    BDSP_AudioType_eiLBC,
    "ILBC",
#ifdef BDSP_ILBC_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_ILBC_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_siLBCdecDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_iLBCdecConfigParams),
    sizeof(BDSP_Raaga_Audio_iLBCStreamInfo),
#ifdef BDSP_ILBC_SUPPORT        
    {BDSP_Img_Id_eiLBC_Ids,BDSP_Img_Id_eiLBC_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eiLBC_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eiLBC_Ids_Interframe,BDSP_Img_Id_eiLBC_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_ILBC_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eiLBC_Ids,
        BDSP_Img_Id_eiLBC_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};


static const BDSP_Raaga_P_AudioCodecDetails BDSP_siSACCodecDetails =
{/*BDSP_AudioType_eiSAC Details*/
    BDSP_AudioType_eiSAC,
    "ISAC",
#ifdef BDSP_ISAC_SUPPORT        
    true,
#else
    false,
#endif
#ifdef  BDSP_ISAC_PASSTHRU_SUPPORT                               
    true,
#else
    false,
#endif        
    (void *)&BDSP_siSACdecDefaultUserConfig,
    sizeof(BDSP_Raaga_Audio_iSACdecConfigParams),
    sizeof(BDSP_Raaga_Audio_iSACStreamInfo),
#ifdef BDSP_ISAC_SUPPORT        
    {BDSP_Img_Id_eiSAC_Ids,BDSP_Img_Id_eiSAC_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eiSAC_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eiSAC_Ids_Interframe,BDSP_Img_Id_eiSAC_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
#ifdef BDSP_ISAC_PASSTHRU_SUPPORT        
        BDSP_Img_Id_eiSAC_Ids,
        BDSP_Img_Id_eiSAC_Ids_Interframe,
#else
        BDSP_Img_Id_eInvalid,
        BDSP_Img_Id_eInvalid,
#endif
    NULL
};

/*****************************************************************************/
/**************Audio processing structure details ****************************/
/*****************************************************************************/
static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sDtsNeoProcessingDetails =
{/*BDSP_AudioProcessing_eDtsNeo Details*/
    BDSP_AudioProcessing_eDtsNeo,
    "Ddbm",
#ifdef BDSP_DTSNEO_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    0,
    0,
#ifdef BDSP_DTSNEO_SUPPORT        
    {BDSP_Img_Id_eMpeg1_Ids,BDSP_Img_Id_eMpeg1_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eMpeg1_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eMpeg1_Ids_Interframe,BDSP_Img_Id_eMpeg1_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sAVLProcessingDetails =
{/*BDSP_AudioProcessing_eAVL Details*/
    BDSP_AudioProcessing_eAVL,
    "AVL",
#ifdef BDSP_AVL_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefAVLConfigSettings,
    sizeof(BDSP_Raaga_Audio_AVLConfigParams),
    0,
#ifdef BDSP_AVL_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAvl_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAvl_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAvl_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sPLllProcessingDetails =
{/*BDSP_AudioProcessing_ePLll Details*/
    BDSP_AudioProcessing_ePLll,
    "PL2",
#ifdef BDSP_PL2_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefProLogicIIConfigSettings,
    sizeof(BDSP_Raaga_Audio_PL2ConfigParams),
    0,
#ifdef BDSP_PL2_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_ePL2_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_ePL2_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_ePL2_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sSrsXtProcessingDetails =
{/*BDSP_AudioProcessing_eSrsXt Details*/
    BDSP_AudioProcessing_eSrsXt,
    "SrsXt",
#ifdef BDSP_SRSXT_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefTruSurroundXtSettings,
    sizeof(BDSP_Raaga_Audio_TruSurrndXTConfigParams),
    0,
#ifdef BDSP_SRSXT_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsXt_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsXt_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsXt_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sBbeProcessingDetails =
{/*BDSP_AudioProcessing_eBbe Details*/
    BDSP_AudioProcessing_eBbe,
    "Bbe",
#ifdef BDSP_BBE_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    0,
    0,
#ifdef BDSP_BBE_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBbe_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBbe_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBbe_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sSrcProcessingDetails =
{/*BDSP_AudioProcessing_eSrc Details*/
    BDSP_AudioProcessing_eSrc,
    "Src",
#ifdef BDSP_SRC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefaultSrcSettings,
    sizeof(BDSP_Raaga_Audio_SRCUserConfigParams),
    0,
#ifdef BDSP_SRC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrc_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrc_Tables,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrc_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sCustomVoiceProcessingDetails =
{/*BDSP_AudioProcessing_eCustomVoice Details*/
    BDSP_AudioProcessing_eCustomVoice,
    "CustomVoice",
#ifdef BDSP_CUSTOMVOICE_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefCustomVoiceConfigSettings,
    sizeof(BDSP_Raaga_Audio_CustomVoiceConfigParams),
    sizeof(BDSP_Raaga_Audio_CustomVoiceStatusInfo),
#ifdef BDSP_CUSTOMVOICE_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomVoice_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomVoice_Tables,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomVoice_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sAacDownmixProcessingDetails =
{/*BDSP_AudioProcessing_eAacDownmix Details*/
    BDSP_AudioProcessing_eAacDownmix,
    "AacDownmix",
#ifdef BDSP_AACDOWNMIX_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    0,
    0,
#ifdef BDSP_AACDOWNMIX_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sDsolaProcessingDetails =
{/*BDSP_AudioProcessing_eDsola Details*/
    BDSP_AudioProcessing_eDsola,
    "Dsola",
#ifdef BDSP_DSOLA_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefDsolaConfigSettings,
    sizeof(BDSP_Raaga_Audio_DsolaConfigParams),
    0,
#ifdef BDSP_DSOLA_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDsola_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDsola_Tables,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDsola_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sSrsHdProcessingDetails =
{/*BDSP_AudioProcessing_eSrsHd Details*/
    BDSP_AudioProcessing_eSrsHd,
    "SrsHd",
#ifdef BDSP_SRSHD_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefTruSurrndHDConfigSettings,
    sizeof(BDSP_Raaga_Audio_TruSurrndHDConfigParams),
    0,
#ifdef BDSP_SRSHD_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsHd_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsHd_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsHd_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sGenericPassThruProcessingDetails =
{/*BDSP_AudioProcessing_eGenericPassThru Details*/
    BDSP_AudioProcessing_eGenericPassThru,
    "GenericPassThru",
    true,
    (void *)&BDSP_sDefaultPassthruSettings,
    sizeof(BDSP_Raaga_Audio_PassthruConfigParams),
    0,
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
};


static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sMlpPassThruProcessingDetails =
{/*BDSP_AudioProcessing_eMlpPassThru Details*/
    BDSP_AudioProcessing_eMlpPassThru,
    "MlpPassThru",
#ifdef BDSP_MLP_PASSTHROUGH_SUPPORT     
    true,
#else
    false,
#endif    
    (void *)NULL,
    0,
    0,
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
};


static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sSrsTruVolumeProcessingDetails =
{/*BDSP_AudioProcessing_eSrsTruVolume Details*/
    BDSP_AudioProcessing_eSrsTruVolume,
    "SrsTruVolume",
#ifdef BDSP_SRSTRUVOL_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefSrsTruVolumeUserConfig,
    sizeof(BDSP_Raaga_Audio_TruVolumeUserConfig),
    0,
#ifdef BDSP_SRSTRUVOL_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsTruVolume_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsTruVolume_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsTruVolume_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sDolbyVolumeProcessingDetails =
{/*BDSP_AudioProcessing_eDolbyVolume Details*/
    BDSP_AudioProcessing_eDolbyVolume,
    "DolbyVolume",
#ifdef BDSP_DOLBYVOL_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefDolbyVolUserConfig,
    sizeof(BDSP_Raaga_Audio_DolbyVolumeUserConfig),
    0,
#ifdef BDSP_DOLBYVOL_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDolbyVol_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDolbyVol_Tables,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDolbyVol_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sAudysseyVolumeProcessingDetails =
{/*BDSP_AudioProcessing_eAudysseyVolume Details*/
    BDSP_AudioProcessing_eAudysseyVolume,
    "AudysseyVolume",
#ifdef BDSP_ADV_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefAudysseyVolConfigSettings,
    sizeof(BDSP_Raaga_Audio_AudysseyVolUserConfig),
    sizeof(BDSP_Raaga_Audio_AudysseyVolStatusInfo),
#ifdef BDSP_ADV_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAvd_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAvd_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAvd_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sBrcm3DSurroundProcessingDetails =
{/*BDSP_AudioProcessing_eBrcm3DSurround Details*/
    BDSP_AudioProcessing_eBrcm3DSurround,
    "Brcm3DSurround",
#ifdef BDSP_BRCM3DSURROUND_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefBrcm3DSurroundConfigSettings,
    sizeof(BDSP_Raaga_Audio_Brcm3DSurroundConfigParams),
    0,
#ifdef BDSP_BRCM3DSURROUND_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBrcm3DSurround_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBrcm3DSurround_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBrcm3DSurround_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sFWMixerProcessingDetails =
{/*BDSP_AudioProcessing_eFWMixer Details*/
    BDSP_AudioProcessing_eFWMixer,
    "FWMixer",
#ifdef BDSP_FWMIXER_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefFwMixerConfigSettings,
    sizeof(BDSP_Raaga_Audio_MixerConfigParams),
    0,
#ifdef BDSP_FWMIXER_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eFwMixer_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eFwMixer_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eFwMixer_Inter_frame,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sAudysseyABXProcessingDetails =
{/*BDSP_AudioProcessing_eAudysseyABX Details*/
    BDSP_AudioProcessing_eAudysseyABX,
    "AudysseyABX",
#ifdef BDSP_ABX_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefAudysseyABXConfigSettings,
    sizeof(BDSP_Raaga_Audio_ABXConfigParams),
    0,
#ifdef BDSP_ABX_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAbx_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAbx_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAbx_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sDdreProcessingDetails =
{/*BDSP_AudioProcessing_eDdre Details*/
    BDSP_AudioProcessing_eDdre,
    "Ddre",
#ifdef BDSP_DDRE_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefDDReencodeUserConfig,
    sizeof(BDSP_Raaga_Audio_DDReencodeConfigParams),
    0,
#ifdef BDSP_DDRE_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDDRE_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDDRE_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDDRE_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sDv258ProcessingDetails =
{/*BDSP_AudioProcessing_eDv258 Details*/
    BDSP_AudioProcessing_eDv258,
    "Dv258",
#ifdef BDSP_DV258_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefDV258UserConfig,
    sizeof(BDSP_Raaga_Audio_DV258ConfigParams),
    0,
#ifdef BDSP_DV258_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDV258_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDV258_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDV258_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sSrsCsdTdProcessingDetails =
{/*BDSP_AudioProcessing_eSrsCsdTd Details*/
    BDSP_AudioProcessing_eSrsCsdTd,
    "SrsCsdTd",
#ifdef BDSP_SRSCSTD_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefSrsCsdecTDConfigSettings,
    sizeof(BDSP_Raaga_Audio_SRS_CSDecTruDialogConfigParams),
    0,
#ifdef BDSP_SRSCSTD_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsCsTd_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsCsTd_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsCsTd_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sSrsEqHlProcessingDetails =
{/*BDSP_AudioProcessing_eSrsEqHl Details*/
    BDSP_AudioProcessing_eSrsEqHl,
    "SrsEqHl",
#ifdef BDSP_SRSEQHL_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefSrsEqHlConfigSettings,
    sizeof(BDSP_Raaga_Audio_SRS_EqualizerHardLimiterConfigParams),
    0,
#ifdef BDSP_SRSEQHL_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsEqHl_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsEqHl_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSrsEqHl_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sCustomDbeProcessingDetails =
{/*BDSP_AudioProcessing_eCustomDbe Details*/
    BDSP_AudioProcessing_eCustomDbe,
    "CustomDbe",
#ifdef BDSP_CUSTOMDBE_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    0,
    0,
#ifdef BDSP_CUSTOMDBE_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomDbe_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomDbe_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomDbe_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sCustomAcfProcessingDetails =
{/*BDSP_AudioProcessing_eCustomAcf Details*/
    BDSP_AudioProcessing_eCustomAcf,
    "CustomAcf",
#ifdef BDSP_CUSTOMACF_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    0,
    0,
#ifdef BDSP_CUSTOMACF_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomAcf_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomAcf_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomAcf_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sCustomAvlpProcessingDetails =
{/*BDSP_AudioProcessing_eCustomAvlp Details*/
    BDSP_AudioProcessing_eCustomAvlp,
    "CustomAvlp",
#ifdef BDSP_CUSTOMAVLP_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefCustomAvlplusConfigSettings,
    sizeof(BDSP_Raaga_Audio_AvlPConfigParams),
    0,
#ifdef BDSP_CUSTOMAVLP_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomAvlp_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomAvlp_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eCustomAvlp_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sGenCdbItbProcessingDetails =
{/*BDSP_AudioProcessing_eGenCdbItb Details*/
    BDSP_AudioProcessing_eGenCdbItb,
    "GenCdbItb",
#ifdef BDSP_GENCDBITB_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefGenCdbItbConfigSettings,
    sizeof(BDSP_Raaga_Audio_GenCdbItbConfigParams),
    0,
#ifdef BDSP_GENCDBITB_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eGenItbCdb_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eGenItbCdb_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eGenItbCdb_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sBtscEncoderProcessingDetails =
{/*BDSP_AudioProcessing_eBtscEncoder Details*/
    BDSP_AudioProcessing_eBtscEncoder,
    "BtscEncoder",
#ifdef BDSP_BTSCENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefBtscEncoderConfigSettings,
    sizeof(BDSP_Raaga_Audio_BtscEncoderConfigParams),
    0,
#ifdef BDSP_BTSCENC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBtscEncoder_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBtscEncoder_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eBtscEncoder_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioProcessingDetails BDSP_sSpeexAecProcessingDetails =
{/*BDSP_AudioProcessing_eSpeexAec Details*/
    BDSP_AudioProcessing_eSpeexAec,
    "SPEEX AEC",
#ifdef BDSP_SPEEXAEC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefSpeexAECConfigParams,
    sizeof(BDSP_Raaga_Audio_SpeexAECConfigParams),
    0,
#ifdef BDSP_SPEEXAEC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSpeexAec_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSpeexAec_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eSpeexAec_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};


/*****************************************************************************/
/**************Audio Encode structure details ****************************/
/*****************************************************************************/

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sMpeg1Layer3EncodeDetails =
{   /*BDSP_AudioEncode_eMpeg1Layer3 Details*/
    BDSP_AudioEncode_eMpeg1Layer3,
    "Mpeg1Layer3Enc",
#ifdef BDSP_MP3ENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefMpeg1L3EncConfigSettings,
    sizeof(BDSP_Raaga_Audio_Mpeg1L3EncConfigParams),
    0,
#ifdef BDSP_MP3ENC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eMp3Enc_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eMp3Enc_Code_Tables,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eMp3Enc_Code_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sMpeg1Layer2EncodeDetails =
{/*BDSP_AudioEncode_eMpeg1Layer2 Details*/
    BDSP_AudioEncode_eMpeg1Layer2,
    "Mpeg1Layer2Enc",
#ifdef BDSP_MP2ENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    sizeof(BDSP_Raaga_Audio_Mpeg1L2EncConfigParams),
    0,
#ifdef BDSP_MP2ENC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sDTSEncodeDetails =
{/*BDSP_AudioEncode_eDTS Details*/
    BDSP_AudioEncode_eDTS,
    "DTSEnc",
#ifdef BDSP_DTSENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefDTSENCConfigSettings,
    sizeof(BDSP_Raaga_Audio_DtsBroadcastEncConfigParams),
    0,
#ifdef BDSP_DTSENC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDts_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDts_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDts_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sAacLcEncodeDetails =
{/*BDSP_AudioEncode_eAacLc Details*/
    BDSP_AudioEncode_eAacLc,
    "AacLcEnc",
#ifdef BDSP_AACLCENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    sizeof(BDSP_Raaga_Audio_AaclcEncConfigParams),
    0,
#ifdef BDSP_AACLCENC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sAacHeEncodeDetails =
{/*BDSP_AudioEncode_eAacHe Details*/
    BDSP_AudioEncode_eAacHe,
    "AacHeEnc",
#ifdef BDSP_AACHEENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefAacHeENCConfigSettings,
    sizeof(BDSP_Raaga_Audio_AacheEncConfigParams),
    0,
#ifdef BDSP_AACHEENC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAacHe_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAacHe_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAacHe_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sAc3EncodeDetails =
{/*BDSP_AudioEncode_eAc3 Details*/
    BDSP_AudioEncode_eAc3,
    "Ac3Enc",
#ifdef BDSP_AC3ENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefDDTranscodeConfigSettings,
    sizeof(BDSP_Raaga_Audio_DDTranscodeConfigParams),
    0,
#ifdef BDSP_AC3ENC_SUPPORT               
#ifdef BDSP_MS10_SUPPORT
    {BDSP_Img_Id_eDdp_Ids,BDSP_Img_Id_eDolbyTranscode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eDolbyTranscode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eDdp_Ids_Interframe,BDSP_Img_Id_eDolbyTranscode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAc3_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAc3_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAc3_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif        
#else  
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sSbcEncodeDetails =
{/*BDSP_AudioEncode_eSbc Details*/
    BDSP_AudioEncode_eSbc,
    "SbcEnc",
#ifdef BDSP_SBCENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefSbcEncConfigSettings,
    sizeof(BDSP_Raaga_Audio_SbcEncoderUserConfig),
    0,
#ifdef BDSP_SBCENC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};


static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sG711G726EncodeDetails =
{/*BDSP_AudioEncode_eG711G726 Details*/
    BDSP_AudioEncode_eG711G726,
    "G.711/G.726 ENC",
#ifdef BDSP_G711G726ENC_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefG711G726EncConfigSettings,
    sizeof(BDSP_Raaga_Audio_G711_G726EncConfigParams),
    0,
#ifdef BDSP_G711G726ENC_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG711G726_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG711G726_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG711G726_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};


static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sG729EncodeDetails =
{/*BDSP_AudioEncode_eG729 Details*/
    BDSP_AudioEncode_eG729,
    "G.729 ENC",
#ifdef BDSP_G729ENC_SUPPORT
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefG729EncConfigSettings,
    sizeof(BDSP_Raaga_Audio_G729EncoderUserConfig),
    0,
#ifdef BDSP_G729ENC_SUPPORT
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG729_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG729_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG729_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sWmaEncodeDetails =
{/*BDSP_AudioEncode_eG729 Details*/
    BDSP_AudioEncode_eWma,
    "WMA ENC",
#ifdef BDSP_WMAENC_SUPPORT
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefWmaEncodeConfigSettings,
    sizeof(BDSP_Raaga_Audio_WMAEncoderConfigParams),
    0,
#ifdef BDSP_WMAENC_SUPPORT
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eWma_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eWma_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eWma_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sG723_1EncodeDetails =
{/*BDSP_AudioEncode_eG723_1 Details*/
    BDSP_AudioEncode_eG723_1,
    "G.723.1 ENC",
#ifdef BDSP_G723_1ENC_SUPPORT
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefG723_1EncodeConfigSettings,
    sizeof(BDSP_Raaga_Audio_G723EncoderUserConfig),
    0,
#ifdef BDSP_G723_1ENC_SUPPORT
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG723_1_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG723_1_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG723_1_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sG722EncodeDetails =
{/*BDSP_AudioEncode_eG722 Details*/
    BDSP_AudioEncode_eG722,
    "G.722 ENC",
#ifdef BDSP_G722ENC_SUPPORT
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefG722EncConfigSettings,
    sizeof(BDSP_Raaga_Audio_G722EncConfigParams),
    0,
#ifdef BDSP_G722ENC_SUPPORT
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG722_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG722_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eG722_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sAmrEncodeDetails =
{/*BDSP_AudioEncode_eAmr Details*/
    BDSP_AudioEncode_eAmr,
    "AMR ENC",
#ifdef BDSP_AMRENC_SUPPORT
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefAmrEncConfigSettings,
    sizeof(BDSP_Raaga_Audio_AmrEncoderUserConfig),
    0,
#ifdef BDSP_AMRENC_SUPPORT
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAmr_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAmr_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAmr_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_sAmrwbEncodeDetails =
{/*BDSP_AudioEncode_eAmrwb Details*/
    BDSP_AudioEncode_eAmrwb,
    "AMRWB ENC",
#ifdef BDSP_AMRWBENC_SUPPORT
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefAmrwbEncConfigSettings,
    sizeof(BDSP_Raaga_Audio_AMRWBConfigParams),
    0,
#ifdef BDSP_AMRWBENC_SUPPORT
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAmrwb_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAmrwb_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eAmrwb_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_AudioEncodeDetails BDSP_siLBCEncodeDetails =
{/*BDSP_AudioEncode_eiLBC Details*/
    BDSP_AudioEncode_eiLBC,
    "ILBC ENC",
#ifdef BDSP_ILBCENC_SUPPORT
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefiLBCEncConfigSettings,
    sizeof(BDSP_Raaga_Audio_ILBCConfigParams),
    0,
#ifdef BDSP_ILBCENC_SUPPORT
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eiLBC_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eiLBC_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eiLBC_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};


static const BDSP_Raaga_P_AudioEncodeDetails BDSP_siSACEncodeDetails =
{/*BDSP_AudioEncode_eiSAC Details*/
    BDSP_AudioEncode_eiSAC,
    "ISAC ENC",
#ifdef BDSP_ISACENC_SUPPORT
    true,
#else
    false,
#endif
    (void *)&BDSP_sDefiSACEncConfigSettings,
    sizeof(BDSP_Raaga_Audio_ISACConfigParams),
    0,
#ifdef BDSP_ISACENC_SUPPORT
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eiSAC_Encode_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eiSAC_Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eiSAC_Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};


static const BDSP_Raaga_P_VideoCodecDetails BDSP_sRealVideoCodecDetails =
{/*BDSP_VideoType_eRealVideo9 Details*/
    BDSP_VideoType_eRealVideo9,
    "RealVideo9",
#ifdef BDSP_REALVIDEO_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    0,
    0,
#ifdef BDSP_REALVIDEO_SUPPORT        
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_VideoCodecDetails BDSP_sVp6CodecDetails =
{/*BDSP_VideoType_eVP6 Details*/
    BDSP_VideoType_eVP6,
    "VP6",
#ifdef BDSP_VP6_SUPPORT        
    true,
#else
    false,
#endif
    (void *)NULL,
    0,
    0,
#ifdef BDSP_VP6_SUPPORT        
    {BDSP_Img_Id_eVP6_Ids,BDSP_Img_Id_eVP6_Decode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eVP6_Decode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eVP6_Ids_Interframe,BDSP_Img_Id_eVP6_Decode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_ScmDetails BDSP_sScm1CodecDetails =
{/*BDSP_Scm_eScm1 Details*/
    BDSP_ScmType_eScm1,
    "SCM1",
#ifdef BDSP_SCM1_SUPPORT       
    true,
#else
    false,
#endif
    (void *)NULL, 
    0,
    0,
#ifdef BDSP_SCM1_SUPPORT       
    {BDSP_Img_Id_eScm1_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eScm1_Table, BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eScm1_Interframe, BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_ScmDetails BDSP_sScm2CodecDetails =
{/*BDSP_Scm_eScm2 Details*/
    BDSP_ScmType_eScm2,
    "SCM2",
#ifdef BDSP_SCM2_SUPPORT       
    true,
#else
    false,
#endif
    (void *)NULL,
    0,
    0,
#ifdef BDSP_SCM2_SUPPORT        
    {BDSP_Img_Id_eScm2_Code, BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eScm2_Table, BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eScm2_Interframe, BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_ScmDetails BDSP_sScm3CodecDetails =
{/*BDSP_Scm_eScm3 Details*/
    BDSP_ScmType_eScm3,
    "SCM3",
#ifdef BDSP_SCM3_SUPPORT       
    true,
#else
    false,
#endif
    (void *)NULL, 
    0,
    0,
#ifdef BDSP_SCM3_SUPPORT       
    {BDSP_Img_Id_eScm3_Code,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eScm3_Table, BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eScm3_Interframe, BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_VideoEncodeDetails BDSP_sH264EncodeCodecDetails =
{/*BDSP_VideoEncode_eH264 Details*/
    BDSP_VideoEncode_eH264,
    "H264Encode",
#ifdef BDSP_H264_ENCODE_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sBH264EncodeUserConfigSettings,
    sizeof(BDSP_Raaga_VideoBH264UserConfig),
    0,
#ifdef BDSP_H264_ENCODE_SUPPORT        
    {BDSP_Img_Id_eVidCom_Encode_Ids,BDSP_Img_Id_eH264Encode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eH264Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eVidCom_Encode_Ids_Interframe,BDSP_Img_Id_eH264Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

static const BDSP_Raaga_P_VideoEncodeDetails BDSP_sVP8EncodeCodecDetails =
{/*BDSP_VideoEncode_eVP8 Details*/
    BDSP_VideoEncode_eVP8,
    "VP8Encode",
#ifdef BDSP_VP8_ENCODE_SUPPORT        
    true,
#else
    false,
#endif
    (void *)&BDSP_sBVP8EncodeUserConfigSettings,
    sizeof(BDSP_Raaga_VideoBVP8UserConfig),
    0,
#ifdef BDSP_VP8_ENCODE_SUPPORT        
    {BDSP_Img_Id_eVidCom_Encode_Ids,BDSP_Img_Id_eVP8Encode,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eVP8Encode_Table,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eVidCom_Encode_Ids_Interframe,BDSP_Img_Id_eVP8Encode_Interframe,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#else
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},
    {BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid,BDSP_Img_Id_eInvalid},                
#endif
};

const BDSP_Raaga_P_AudioCodecDetails  *BDSP_sAudioCodecDetails[/*BDSP_AudioType_eMax*/]=
{
    &BDSP_sMpegCodecDetails,
    &BDSP_sAacCodecDetails,
    &BDSP_sAacLoasCodecDetails,
    &BDSP_sAacSbrAdtsCodecDetails,
    &BDSP_sAacSbrLoasCodecDetails,
    &BDSP_sAc3CodecDetails,    
    &BDSP_sAc3PlusCodecDetails,    
    &BDSP_sLpcmBdCodecDetails,    
    &BDSP_sLpcmHdDvdCodecDetails,    
    &BDSP_sDtshdCodecDetails,    
    &BDSP_sLpcmDvdCodecDetails,    
    &BDSP_sWmaStdCodecDetails,
    &BDSP_sMlpCodecDetails,    
    &BDSP_sPcmCodecDetails,    
    &BDSP_sDtsLbrCodecDetails,
    &BDSP_sDdp7_1CodecDetails,
    &BDSP_sMpegMcCodecDetails,    
    &BDSP_sWmaProCodecDetails,    
    &BDSP_sLpcmDvdACodecDetails,
    &BDSP_sDtsBroadcastCodecDetails,    
    &BDSP_sPcmWavCodecDetails,    
    &BDSP_sAmrCodecDetails,    
    &BDSP_sDraCodecDetails,    
    &BDSP_sRealAudioLbrCodecDetails,    
    &BDSP_sAdpcmCodecDetails,
    &BDSP_sG711G726CodecDetails,
    &BDSP_sG729CodecDetails,
    &BDSP_sVorbisCodecDetails,
    &BDSP_sG723_1CodecDetails,
    &BDSP_sFlacCodecDetails,
    &BDSP_sMacCodecDetails,
    &BDSP_sAmrwbCodecDetails, 
	&BDSP_siLBCCodecDetails, 
	&BDSP_siSACCodecDetails,
	   

};


const BDSP_Raaga_P_AudioProcessingDetails *BDSP_sAudioProcessingDetails[/*BDSP_AudioProcessing_eMax*/]=
{
    &BDSP_sDtsNeoProcessingDetails,
    &BDSP_sAVLProcessingDetails,
    &BDSP_sPLllProcessingDetails,
    &BDSP_sSrsXtProcessingDetails,
    &BDSP_sBbeProcessingDetails,
    &BDSP_sSrcProcessingDetails,
    &BDSP_sCustomVoiceProcessingDetails,
    &BDSP_sAacDownmixProcessingDetails,
    &BDSP_sDsolaProcessingDetails,
    &BDSP_sSrsHdProcessingDetails,
    &BDSP_sGenericPassThruProcessingDetails,
    &BDSP_sMlpPassThruProcessingDetails,
    &BDSP_sSrsTruVolumeProcessingDetails,
    &BDSP_sDolbyVolumeProcessingDetails,
    &BDSP_sAudysseyVolumeProcessingDetails,
    &BDSP_sBrcm3DSurroundProcessingDetails,
    &BDSP_sFWMixerProcessingDetails,
    &BDSP_sAudysseyABXProcessingDetails,
    &BDSP_sDdreProcessingDetails,
    &BDSP_sDv258ProcessingDetails,
    &BDSP_sSrsCsdTdProcessingDetails,
    &BDSP_sSrsEqHlProcessingDetails,
    &BDSP_sCustomDbeProcessingDetails,
    &BDSP_sCustomAcfProcessingDetails,
    &BDSP_sCustomAvlpProcessingDetails,
    &BDSP_sGenCdbItbProcessingDetails,
    &BDSP_sBtscEncoderProcessingDetails,
    &BDSP_sSpeexAecProcessingDetails,
};


const BDSP_Raaga_P_AudioEncodeDetails *BDSP_sAudioEncodeDetails[/*BDSP_AudioEncode_eMax*/]=
{
    &BDSP_sMpeg1Layer3EncodeDetails,
    &BDSP_sMpeg1Layer2EncodeDetails,
    &BDSP_sDTSEncodeDetails,
    &BDSP_sAacLcEncodeDetails,
    &BDSP_sAacHeEncodeDetails,
    &BDSP_sAc3EncodeDetails,
    &BDSP_sSbcEncodeDetails,
    &BDSP_sG711G726EncodeDetails,
    &BDSP_sG729EncodeDetails,
    &BDSP_sWmaEncodeDetails,
    &BDSP_sG723_1EncodeDetails,
    &BDSP_sG722EncodeDetails,
    &BDSP_sAmrEncodeDetails,
    &BDSP_sAmrwbEncodeDetails,    
	&BDSP_siLBCEncodeDetails,
	&BDSP_siSACEncodeDetails,
};

const BDSP_Raaga_P_VideoCodecDetails  *BDSP_sVideoCodecDetails[/*BDSP_VideoType_eMax*/]=
{
    &BDSP_sRealVideoCodecDetails,   
    &BDSP_sVp6CodecDetails
};

const BDSP_Raaga_P_ScmDetails  *BDSP_sScmCodecDetails[/*BDSP_ScmType_eMax*/]=
{
    &BDSP_sScm1CodecDetails,
	&BDSP_sScm2CodecDetails,
	&BDSP_sScm3CodecDetails,
};
const BDSP_Raaga_P_VideoEncodeDetails  *BDSP_sVideoEncodeCodecDetails[/*BDSP_VideoEncode_eMax*/]=
{
    &BDSP_sH264EncodeCodecDetails,
 	&BDSP_sVP8EncodeCodecDetails
};

const  BDSP_Raaga_Audio_FrameSyncTsmConfigParams   BDSP_sDefaultFrameSyncTsmSettings =    
{                                           
    {                                       /* sFrameSyncConfigParams */
        0,
        BDSP_Raaga_Audio_AudioInputSource_eExtI2s0,
        {         
            48000
        },
            1,
			{{
            BDSP_Raaga_Audio_ASFPTSType_eInterpolated,                
                BDSP_Raaga_Audio_WMAIpType_eASF
			}},
        0                                           /* eForceCompleteFirstFrame */
    },
    {                                       /* sTsmConfigParams */
        90,                                 /* i32TSMSmoothThreshold */
            0,                                  /* i32TSMSyncLimitThreshold */
            360,                                /* i32TSMGrossThreshold */      
            135000,                             /* i32TSMDiscardThreshold */
            0,                                  /* i32TsmTransitionThreshold */
#ifdef BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_BASE
            BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_BASE+BCHP_PHYSICAL_OFFSET,/* ui32STCAddr */
#else
            BCHP_AUD_MISC_STC_UPPERi_ARRAY_BASE+BCHP_PHYSICAL_OFFSET,/* ui32STCAddr */
#endif
            0,                                  /* ui32AVOffset */
            0,                                  /* ui32SwSTCOffset */
            5760,                               /* ui32AudioOffset */
            BDSP_Raaga_eTsmBool_True,           /* eEnableTSMErrorRecovery */                    
            BDSP_Raaga_eTsmBool_False,          /* eSTCValid */                    
            BDSP_Raaga_eTsmBool_True,           /* ePlayBackOn */                    
            BDSP_Raaga_eTsmBool_False,          /* eTsmEnable */                    
            BDSP_Raaga_eTsmBool_True,           /* eTsmLogEnable */
            BDSP_Raaga_eTsmBool_False,           /* eASTMEnable */
    }
};

const BDSP_VideoEncodeTaskDatasyncSettings		BDSP_sDefaultVideoEncodeFrameSyncSettings =
{
	BDSP_AF_P_eDisable,
	0

};

#define FRAMESYNC_TSM_NODE_INDEX                0


void BDSP_Raaga_P_Close(
    void *pDeviceHandle
    )
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    BDSP_RaagaContext *pRaagaContext;
	BDSP_RaagaExternalInterrupt *pRaagaExtInterrput;
	BDSP_RaagaRdbRegister *pRaagaRdbRegister;
    BDSP_Status sStatus;
    unsigned uiDspIndex=0;
    BERR_Code   err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

#ifdef BDSP_FW_RBUF_CAPTURE
    /* Specific to FW Ring Buffer capture required for their unit testing */
    if(Rbuf_Setting.rbuf_init != NULL && Rbuf_Setting.rbuf_uninit != NULL) 
    {
    	Rbuf_Setting.rbuf_uninit();
    }
#endif
	
    BKNI_AcquireMutex(pDevice->captureMutex);

    /* Destroy any contexts left open */
    while ( (pRaagaContext = BLST_S_FIRST(&pDevice->contextList)) )
    {
        BDSP_Context_Destroy(&pRaagaContext->context);
    }

	/* Free up any interrupt handle left open */
	while( (pRaagaExtInterrput = BLST_S_FIRST(&pDevice->interruptList)) )
	{
		BDSP_FreeExternalInterrupt(&pRaagaExtInterrput->extInterrupt);
	}
	
	/* Free up any rdb register handle left open */
	while( (pRaagaRdbRegister = BLST_S_FIRST(&pDevice->rdbRegisterList)) )
	{
		BDSP_FreeRdbRegisters(&pRaagaRdbRegister->rdbRegister);
	}

    pDevice->device.getStatus(pDeviceHandle,&sStatus);

    BDSP_Raaga_P_AckUninstall(pDeviceHandle);

    for (uiDspIndex =0 ; uiDspIndex < BDSP_RAAGA_MAX_DSP ; uiDspIndex++)
    {
        err = BDSP_Raaga_P_DestroyMsgQueue(pDevice->hCmdQueue[uiDspIndex], 
            pDevice->regHandle, pDevice->dspOffset[uiDspIndex]);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_Close: CMD queue destroy failed!"));
            err = BERR_TRACE(err);
        }

        err = BDSP_Raaga_P_DestroyMsgQueue(pDevice->hGenRspQueue[uiDspIndex], 
            pDevice->regHandle, pDevice->dspOffset[uiDspIndex]);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_Close: Generic RSP queue destroy failed!"));
            err = BERR_TRACE(err);
        }

        BKNI_DestroyMutex((pDevice->fifoIdMutex[uiDspIndex]));        
		BKNI_DestroyMutex((pDevice->dspInterruptMutex[uiDspIndex]));
		BKNI_DestroyMutex((pDevice->rdbRegistersMutex[uiDspIndex]));
    }
    BKNI_DestroyMutex((pDevice->watchdogMutex));        
    BDSP_Raaga_P_FreeInitMemory(pDeviceHandle);

    BKNI_ReleaseMutex(pDevice->captureMutex);
    
    BKNI_DestroyMutex((pDevice->taskDetails.taskIdMutex));
    BKNI_DestroyMutex((pDevice->captureMutex));

    /*Disable watchdog*/              
    err = BDSP_Raaga_P_EnableDspWatchdogTimer(pDeviceHandle,false);
    if (err!=BERR_SUCCESS)
    {
        BDBG_MSG(("BDSP_Raaga_P_EnableDspWatchdogTimer::Error returned %x!",err));
        err= BERR_TRACE(err);
    }

    /*Reset DSP/s */
	for (uiDspIndex =0 ; uiDspIndex < BDSP_RAAGA_MAX_DSP ; uiDspIndex++)
	{
    	BDSP_Raaga_P_ResetHardware(pDeviceHandle, uiDspIndex);
	}
    
#ifdef BCHP_PWR_RESOURCE_RAAGA_SRAM
    BCHP_PWR_ReleaseResource(pDevice->chpHandle, BCHP_PWR_RESOURCE_RAAGA_SRAM);
#endif  
#ifdef BCHP_PWR_RESOURCE_RAAGA 
    BCHP_PWR_ReleaseResource(pDevice->chpHandle, BCHP_PWR_RESOURCE_RAAGA );
#endif

    /* Invalidate and free the device structure */
    BDBG_OBJECT_DESTROY(pDevice, BDSP_Raaga);
    BKNI_Free(pDevice);
}


void BDSP_Raaga_P_GetStatus(
    void *pDeviceHandle,
    BDSP_Status *pStatus
    )
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
    pStatus->numDsp =BDSP_RAAGA_MAX_DSP;
    pStatus->numWatchdogEvents=0;
    pStatus->firmwareVersion.majorVersion = BDSP_sRaagaVersion.majorVersion; 
    pStatus->firmwareVersion.minorVersion = BDSP_sRaagaVersion.minorVersion; 
    pStatus->firmwareVersion.branchVersion = BDSP_sRaagaVersion.branchVersion; 
    pStatus->firmwareVersion.branchSubVersion = BDSP_sRaagaVersion.branchSubVersion; 
    return;
}

void   BDSP_Raaga_P_GetAudioTypeInfo(
      BDSP_AudioType type, /* [in] */
    const BDSP_AudioTypeInfo **pInfo /* [out] */
    )
{
    BDBG_ASSERT(pInfo);
    *pInfo = &sAudioDecodeInfo[type];
    return;
}

void   BDSP_Raaga_P_GetAudioProcessingInfo(
    BDSP_AudioProcessing type, /* [in] */
    const BDSP_AudioProcessingInfo **pInfo /* [out] */
    )
{
    BDBG_ASSERT(pInfo);
    *pInfo = &sAudioProcessingInfo[type];
    return;
}

void   BDSP_Raaga_P_GetAudioEncodeInfo(
    BDSP_AudioEncode type, /* [in] */
    const BDSP_AudioEncodeInfo **pInfo /* [out] */
    )
{
    BDBG_ASSERT(pInfo);
    *pInfo = &sAudioEncodeInfo[type];
    return;
}


void BDSP_Raaga_P_GetDefaultContextSettings(
    void *pDeviceHandle, 
    BDSP_ContextType contextType,    
    BDSP_ContextCreateSettings *pSettings
    )
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    if (contextType == BDSP_ContextType_eAudio)
    {
        pSettings->maxTasks = BDSP_RAAGA_MAX_FW_TASK_PER_CTXT;
        pSettings->contextType = contextType;
        pSettings->maxBranch = BDSP_RAAGA_MAX_BRANCH;
        pSettings->maxStagePerBranch = BDSP_RAAGA_MAX_STAGE_PER_BRANCH;
        pSettings->audioDownloadBuffer.maxNumAudioDatasyncDownloadBuffer = BDSP_RAAGA_MAX_FS_CTXT;
        pSettings->audioDownloadBuffer.maxNumAudioDecodeDownloadBuffer= BDSP_RAAGA_MAX_DECODE_CTXT;
        pSettings->audioDownloadBuffer.maxNumAudioPassthruDownloadBuffer= BDSP_RAAGA_MAX_PASSTHRU_CTXT;
        pSettings->audioDownloadBuffer.maxNumAudioProcessingDownloadBuffer= BDSP_RAAGA_MAX_AUD_PROCESSING_CTX;
        pSettings->audioDownloadBuffer.maxNumAudioEncodeDownloadBuffer= BDSP_RAAGA_MAX_ENCODE_CTXT;    
    }
    else if (contextType == BDSP_ContextType_eVideo)
    {
        pSettings->maxTasks = BDSP_RAAGA_MAX_FW_TASK_PER_CTXT;
        pSettings->contextType = contextType;
        pSettings->maxBranch = BDSP_RAAGA_MAX_BRANCH;
        pSettings->maxStagePerBranch = BDSP_RAAGA_MAX_STAGE_PER_BRANCH;
        pSettings->videoDownloadBuffer.maxNumVideoDecodeDownloadBuffer= BDSP_RAAGA_MAX_DECODE_CTXT;
    }
	else if (contextType == BDSP_ContextType_eScm)
    {
        pSettings->maxTasks = BDSP_RAAGA_MAX_FW_TASK_PER_SCM_CTXT;
        pSettings->contextType = contextType;
        pSettings->maxBranch = BDSP_RAAGA_MAX_BRANCH_SCM;
        pSettings->maxStagePerBranch = BDSP_RAAGA_MAX_STAGE_PER_BRANCH_SCM;
        pSettings->scmDownloadBuffer.maxNumScmDecodeDownloadBuffer= BDSP_RAAGA_MAX_SCM_CTXT;
    }
	else if (BDSP_ContextType_eVideoEncode == contextType)
    {
        pSettings->maxTasks = BDSP_RAAGA_MAX_FW_TASK_PER_VIDEO_ENCODE_CTXT;
        pSettings->contextType = contextType;
        pSettings->maxBranch = BDSP_RAAGA_MAX_BRANCH_VIDEO_ENCODE;
        pSettings->maxStagePerBranch = BDSP_RAAGA_MAX_STAGE_PER_BRANCH_VIDEO_ENCODE;
        pSettings->videoEncodeDownloadBuffer.maxNumVideoEncodeDownloadBuffer= BDSP_RAAGA_MAX_VIDEOENCODE_CTXT;
    }
}

BERR_Code BDSP_Raaga_P_CreateContext(
    void *pDeviceHandle, 
    const BDSP_ContextCreateSettings *pSettings, 
    BDSP_ContextHandle *pContextHandle
    )
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    BDSP_RaagaContext *pRaagaContext;
    BDSP_RaagaTask *pTask;
    unsigned i;
    BERR_Code errCode=BERR_SUCCESS;
    bool    bOpenTimeAudProcDownload;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

    /* Alloc raaga context */
    pRaagaContext = BKNI_Malloc(sizeof(BDSP_RaagaContext));
    if ( NULL == pRaagaContext )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc_context;
    }
    BKNI_Memset(pRaagaContext, 0, sizeof(*pRaagaContext));
    BDBG_OBJECT_SET(pRaagaContext, BDSP_RaagaContext);
    BLST_S_INIT(&pRaagaContext->freeTaskList);
    BKNI_AcquireMutex(pDevice->captureMutex);
    BLST_S_INIT(&pRaagaContext->allocTaskList);
    BKNI_ReleaseMutex(pDevice->captureMutex);
    pRaagaContext->pDevice = pDevice;

    /* Init context */
    BDSP_P_InitContext(&pRaagaContext->context, pRaagaContext);
    pRaagaContext->context.destroy = BDSP_Raaga_P_DestroyContext;
    pRaagaContext->context.getDefaultTaskSettings = BDSP_Raaga_P_GetDefaultTaskSettings;
    pRaagaContext->context.getInterruptHandlers = BDSP_Raaga_P_GetInterruptHandlers;
    pRaagaContext->context.setInterruptHandlers= BDSP_Raaga_P_SetInterruptHandlers;   
    pRaagaContext->context.processWatchdogInterrupt= BDSP_Raaga_P_ProcessWatchdogInterrupt;

    pRaagaContext->context.createTask = BDSP_Raaga_P_CreateTask;
    pRaagaContext->context.createCapture = BDSP_Raaga_P_AudioCaptureCreate;
    errCode = BDSP_Raaga_P_ContextInterruptInstall((void *)pRaagaContext);

    if ( BERR_SUCCESS!=errCode )
    {
        goto err_interrupt_install;
    }

    /* Allocate task structures and add to free task list */
    for ( i = 0; i < pSettings->maxTasks; i++ )
    {
        pTask = BKNI_Malloc(sizeof(BDSP_RaagaTask));
        if ( NULL == pTask )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto err_malloc_task;
        }
        BKNI_Memset(pTask, 0, sizeof(BDSP_RaagaTask));
        BDBG_OBJECT_SET(pTask, BDSP_RaagaTask);

        /* Init task */
        BDSP_P_InitTask(&pTask->task, pTask);
        pTask->task.destroy = BDSP_Raaga_P_DestroyTask;
        pTask->task.start = BDSP_Raaga_P_StartTask;
        pTask->task.stop= BDSP_Raaga_P_StopTask;        

        pRaagaContext->settings = *pSettings;
        pTask->pContext = pRaagaContext;

        if (pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
        {
            pTask->task.getStageSettings = BDSP_Raaga_P_GetStageSettings;
            pTask->task.setStageSettings = BDSP_Raaga_P_SetStageSettings;     
            pTask->task.getDatasyncSettings = BDSP_Raaga_P_GetDatasyncSettings;    
            pTask->task.setDatasyncSettings = BDSP_Raaga_P_SetDatasyncSettings;
            pTask->task.getTsmSettings_isr = BDSP_Raaga_P_GetTsmSettings_isr;
            pTask->task.setTsmSettings_isr= BDSP_Raaga_P_SetTsmSettings_isr;        
            pTask->task.getStageStatus = BDSP_Raaga_P_GetStageStatus;
            pTask->task.getTsmStatus_isr = BDSP_Raaga_P_GetTsmStatus_isr;      
            pTask->task.getDatasyncStatus= BDSP_Raaga_P_GetDatasyncStatus;
            pTask->task.pause= BDSP_Raaga_P_Pause;
            pTask->task.resume = BDSP_Raaga_P_Resume;
            pTask->task.advance = BDSP_Raaga_P_Advance;
            pTask->task.getAudioInterruptHandlers_isr = BDSP_Raaga_P_GetAudioInterruptHandlers_isr;
            pTask->task.setAudioInterruptHandlers_isr = BDSP_Raaga_P_SetAudioInterruptHandlers_isr;
            pTask->task.addStageInput = BDSP_Raaga_P_AddStageInput;
            pTask->task.removeStageInput = BDSP_Raaga_P_RemoveStageInput;
            pTask->task.getStageInputIndex= BDSP_Raaga_P_GetStageInputIndex;            
			pTask->task.audioGapFill = BDSP_Raaga_P_AudioGapFill;
        }
        else if (pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
        {
            pTask->task.getPictureCount_isr = BDSP_Raaga_P_GetPictureCount_isr;
            pTask->task.peekAtPicture_isr = BDSP_Raaga_P_PeekAtPicture_isr;
            pTask->task.getNextPicture_isr = BDSP_Raaga_P_GetNextPicture_isr;
            pTask->task.releasePicture_isr = BDSP_Raaga_P_ReleasePicture_isr;
            pTask->task.getPictureDropPendingCount_isr = BDSP_Raaga_P_GetPictureDropPendingCount_isr;
            pTask->task.requestPictureDrop_isr = BDSP_Raaga_P_RequestPictureDrop_isr;
            pTask->task.displayInterruptEvent_isr = BDSP_Raaga_P_DisplayInterruptEvent_isr;
			pTask->task.getStageSettings = BDSP_Raaga_P_GetVideoStageSettings;
			pTask->task.setStageSettings = BDSP_Raaga_P_SetVideoStageSettings;	  
        }
        else if (BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType)
        {
        	pTask->task.getVideoEncodeDatasyncSettings = BDSP_Raaga_P_GetVideoEncodeDatasyncSettings;
			pTask->task.setVideoEncodeDatasyncSettings = BDSP_Raaga_P_SetVideoEncodeDatasyncSettings;
            pTask->task.getPictureBuffer_isr = BDSP_Raaga_P_GetPictureBuffer_isr;
			pTask->task.putPicture_isr = BDSP_Raaga_P_PutPicture_isr;
			pTask->task.getStageSettings = BDSP_Raaga_P_GetVideoStageSettings;
			pTask->task.setStageSettings = BDSP_Raaga_P_SetVideoStageSettings;	  
        }
        else if (pRaagaContext->settings.contextType == BDSP_ContextType_eScm)
        {
			pTask->task.sendScmCommand = BDSP_Raaga_P_SendScmCommand;
        }

#if 0
        pTask->task.addStageOutput = BDSP_Raaga_P_AddStageOutput;
        pTask->task.removeStageOutput= BDSP_Raaga_P_RemoveStageOutput;        
#endif

        errCode = BDSP_Raaga_P_AllocateTaskMemory((void *)pTask);
        if ( errCode != BERR_SUCCESS )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto err_malloc_task;
        }

        /* Add to free task list */
        BLST_S_INSERT_HEAD(&pRaagaContext->freeTaskList, pTask, node);
    }

    /* Success, add to device's context list */
    BKNI_AcquireMutex(pDevice->captureMutex);
    BLST_S_INSERT_HEAD(&pDevice->contextList, pRaagaContext, node);
    *pContextHandle= &pRaagaContext->context;

    errCode = BDSP_Raaga_P_AllocateContextMemory((void *)pRaagaContext);
    if ( errCode != BERR_SUCCESS )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_context_mem;
    }

    BDSP_Raaga_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);    
    if ((bOpenTimeAudProcDownload == true)&& (pRaagaContext->settings.contextType == BDSP_ContextType_eAudio))
    {
        /* DownloadPPAlgoAtOpentime will be decided based on some logic if
        it is beneficial to download the PP algos at start time to save 
        the memory. */
        errCode = BDSP_Raaga_P_DownloadAudProcessingExecs(pDeviceHandle);        
        if ( errCode != BERR_SUCCESS )
        {
            errCode = BERR_TRACE(BDSP_ERR_DOWNLOAD_FAILED);
            goto err_download_exec;
        }

    }

    goto create_context_success;

    err_download_exec:
    BDSP_Raaga_P_FreeContextMemory((void *)pRaagaContext);    
    /* Remove myself from the device's context list */
    BLST_S_REMOVE(&pRaagaContext->pDevice->contextList, pRaagaContext, BDSP_RaagaContext, node);

    err_context_mem:
    err_malloc_task:
    /* Free all tasks */
    while ( (pTask = BLST_S_FIRST(&pRaagaContext->freeTaskList)) )
    {
        BDSP_Raaga_P_FreeTaskMemory((void *)pTask);
        /* Remove from list */
        BLST_S_REMOVE_HEAD(&pRaagaContext->freeTaskList, node);

        /* Destroy task */
        BDBG_OBJECT_DESTROY(pTask, BDSP_RaagaTask);
        BKNI_Free(pTask);
    }
    BDSP_Raaga_P_ContextInterruptUninstall((void *)pRaagaContext);

    err_interrupt_install:    
    /* Invalidate and free object */
    BDBG_OBJECT_DESTROY(pRaagaContext, BDSP_RaagaContext);
    BKNI_Free(pRaagaContext);

    err_malloc_context:
    create_context_success:
    BKNI_ReleaseMutex(pDevice->captureMutex);
    return errCode;
}

void BDSP_Raaga_P_DestroyContext(
    void *pContextHandle
    )
{
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;
    BDSP_RaagaTask *pTask;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);

    BDSP_Raaga_P_FreeContextMemory(pContextHandle);

    BKNI_AcquireMutex(pRaagaContext->pDevice->captureMutex);
    /* Free all tasks */
    while ( (pTask = BLST_S_FIRST(&pRaagaContext->allocTaskList)) )
    {
        /* Remove from list */
        BLST_S_REMOVE_HEAD(&pRaagaContext->allocTaskList, node);

        BDSP_Raaga_P_FreeTaskMemory((void *)pTask);

        /* Destroy task, this will move it to the free list */
        BDSP_Task_Destroy(&pTask->task);
    }
    while ( (pTask = BLST_S_FIRST(&pRaagaContext->freeTaskList)) )
    {
        /* Remove from list */
        BLST_S_REMOVE_HEAD(&pRaagaContext->freeTaskList, node);

        BDSP_Raaga_P_FreeTaskMemory((void *)pTask);

        /* Destroy task */
        BDBG_OBJECT_DESTROY(pTask, BDSP_RaagaTask);
        BKNI_Free(pTask);
    }

    /* Remove myself from the device's context list */
    BLST_S_REMOVE(&pRaagaContext->pDevice->contextList, pRaagaContext, BDSP_RaagaContext, node);
    BKNI_ReleaseMutex(pRaagaContext->pDevice->captureMutex);

    BDSP_Raaga_P_ContextInterruptUninstall(pContextHandle);    

    /* Invalidate and free context */
    BDBG_OBJECT_DESTROY(pRaagaContext, BDSP_RaagaContext);
    BKNI_Free(pRaagaContext);
}

void BDSP_Raaga_P_GetDefaultTaskSettings(
    void *pContextHandle, 
    BDSP_TaskCreateSettings *pSettings
    )
{
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->schedulingMode= BDSP_TaskSchedulingMode_eStandalone;
    pSettings->masterTask = NULL;
	pSettings->maxIndependentDelay = BDSP_AF_P_MAX_INDEPENDENT_DELAY;
}

void BDSP_P_ShowCitSettings(const BDSP_TaskCreateSettings *pSettings)
{
    uint32_t  i=0,j=0,k=0,m=0, n = 0;

    BDBG_MSG(("\n"));
    BDBG_MSG((" Showing the CIT input Structure settings"));
    BDBG_MSG((" **********************************************************"));

    BDBG_MSG(("\n"));
    BDBG_MSG((" The number of branches : %d",pSettings->numBranches));
    BDBG_MSG((" -----------------------"));

    for (j=0; j< pSettings->numBranches; j++)
    {
        BDBG_MSG(("\n"));
        BDBG_MSG((" Branch number : %d ",j));
        BDBG_MSG((" ---------------------------"));
        BDBG_MSG((" Branch[%d] Number of stage = %d",j,pSettings->pBranchInfo[j]->ui32NumStages));
        for (k =0; k < pSettings->pBranchInfo[j]->ui32NumStages; k++)
        {
            BDBG_MSG(("\n"));   
            BDBG_MSG((" Stage number : %d",k));
            BDBG_MSG((" ------------"));
            BDBG_MSG((" Branch info:"));
            BDBG_MSG((" =========== "));
            BDBG_MSG((" Stage[%d].ui32BranchId = %d",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].ui32BranchId));
            BDBG_MSG((" Stage[%d].ui32StageId = %d",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].ui32StageId));
            BDBG_MSG((" Stage[%d].eAudioAlgoType = %d",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].eStageType));
            BDBG_MSG((" Stage[%d].uAlgorithm.eAudioType = %d",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].uAlgorithm.eDecAudioAlgo));
            BDBG_MSG((" Stage[%d].uAudioMode.eDecAudioMode = %d",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].uAudioMode.eDecAudioMode));
            BDBG_MSG((" Stage[%d].sStgConnectivity.ui32NumSrc = %d",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.ui32NumSrc));
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].eType = %d",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].eType));
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails = %d",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails));

            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = %d",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType));               

            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = %d",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers));                

            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0]));               
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[1] = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[1]));                               
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr));                               
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr));                              
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr));                               


            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr));               
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr));                                
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr));                               
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr));                              
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr = 0x%x",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr));

            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = %d",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId));
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = %d",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum));
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = %d",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum));
            BDBG_MSG((" Stage[%d].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = %d",
                k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId));                 

            BDBG_MSG(("\n"));
            BDBG_MSG((" Stage[%d].sStgConnectivity.ui32NumDistinctOp:%d ",k,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.ui32NumDistinctOp));
            BDBG_MSG((" -----------------------------------------------"));
            for ( m=0;m<pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.ui32NumDistinctOp;m++)
            {
                BDBG_MSG((" DistinctOp = %d",m));
                BDBG_MSG((" Stage[%d].sStgConnectivity.ui32NumDst[%d] =%d",k,m,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.ui32NumDst[m]));                
                BDBG_MSG((" Stage[%d].sStgConnectivity.eDistinctOpType[%d] = %d",k,m,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.eDistinctOpType[m]));                                

                for (i = 0; i< pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.ui32NumDst[m]; i++)
                {
                    BDBG_MSG(("\n"));                
                    BDBG_MSG((" NumDst = %d",i));       
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].eType = %d",k,m,i,pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].eType));               

                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffId.uiBufferId[0] = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0]));      
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffId.uiBufferId[1] = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[1]));                      
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType));      
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers));
                    for (n = 0; n <  BDSP_AF_P_MAX_CHANNEL_PAIR;n++)
                    {
                        BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[%d] = %d",k,m,i,n,
                            pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[n]));
                    }
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr));

                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.eBufferType = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.eBufferType));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.ui32NumBuffers = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.ui32NumBuffers));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32BaseAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32BaseAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32EndAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32EndAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32ReadAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32ReadAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32WrapAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32WrapAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32WriteAddr = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.sCircBuffer.ui32WriteAddr));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.eBufferType = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.eBufferType));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.ui32BufferId = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.ui32BufferId));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.ui32NumBuffer = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.ui32NumBuffers));


                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sFwStage.ui32BranchId = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sFwStage.ui32BranchId));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sFwStage.ui32DistinctOpNum = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sFwStage.ui32DistinctOpNum));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sFwStage.ui32OpNum = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sFwStage.ui32OpNum));
                    BDBG_MSG((" Stage[%d].sStgConnectivity.sDstDetails[%d][%d].uDetails.sFwStage.ui32StageId = %d",k,m,i,
                        pSettings->pBranchInfo[j]->sFwStgInfo[k].sStgConnectivity.sDstDetails[m][i].uDetails.sFwStage.ui32StageId));
                }               
            }
        }
    }

    BDBG_MSG(("\n"));
    BDBG_MSG((" RdbCfg Details"));
    BDBG_MSG((" #####################"));
    BDBG_MSG((" pSettings->timeBaseType: %d",pSettings->timeBaseType));  
    BDBG_MSG(("\n"));

}

static BERR_Code BDSP_Raaga_P_GenerateCITInput(
    void *pTaskHandle, 
    const BDSP_TaskCreateSettings *pSettings,
    BDSP_CIT_P_InputInfo        *pCitInput    
    )
{
    BERR_Code   err=BERR_SUCCESS;

    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
    BDSP_RaagaContext *pRaagaContext = pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    unsigned i=0;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    /*Populate CIT Input*/
    pCitInput->ui32NumBranches = pSettings->numBranches;
    for (i = 0; i< BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
        pCitInput->pBranchInfo[i] = pSettings->pBranchInfo[i];

    /* Show all the input CIT structure*/
    BDSP_P_ShowCitSettings(pSettings);

    pCitInput->sRdbCfg.ui32RdbBaseOffset = 0;
    pCitInput->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;
    pCitInput->eTimeBaseType = pSettings->timeBaseType;

    /* Assigning Mit Structure */
    pCitInput->pMit = &(pDevice->mit);

    /* Assigning CIT memInfo */
    pCitInput->memInfo.ui32SharedMemPhysAdr = pRaagaContext->contextMemInfo.sDspScratchInfo[pSettings->dspIndex].ui32BaseAddr;      
    pCitInput->memInfo.ui32SharedMemSize = pRaagaContext->contextMemInfo.sDspScratchInfo[pSettings->dspIndex].ui32Size;
    pCitInput->memInfo.ui32TaskMemPhysAdr = pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32BaseAddr;
    pCitInput->memInfo.ui32TaskMemSize = pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32Size;


    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32SharedMemPhysAdr = 0x%x",pCitInput->memInfo.ui32SharedMemPhysAdr));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32SharedMemSize = %d",pCitInput->memInfo.ui32SharedMemSize));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32TaskMemPhysAdr = 0x%x",pCitInput->memInfo.ui32TaskMemPhysAdr));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32TaskMemSize = %d",pCitInput->memInfo.ui32TaskMemSize));

    BDBG_MSG(("\n"));
    BDBG_MSG(("Details of the master Index Table "));
    BDBG_MSG(("=================================="));
    for (i=0;i<BDSP_AF_P_AlgoId_eMax;i++)
    {
        if (pCitInput->pMit==NULL)
        {
            continue;
        }
        if (pCitInput->pMit->sAlgoDwnldDetail[i].ui32AlgoCodePtr != 0xffffffff)
        {
            BDBG_MSG(("psCitInputStruct->psMit->sAlgoDwnldDetail[%d].ui32AlgoCodePtr :%d",i,pCitInput->pMit->sAlgoDwnldDetail[i].ui32AlgoCodePtr));
            BDBG_MSG(("psCitInputStruct->psMit->sAlgoDwnldDetail[%d].ui32AlgoCodeSize :%d",i,pCitInput->pMit->sAlgoDwnldDetail[i].ui32Algoui32CodeSize));
            BDBG_MSG(("psCitInputStruct->psMit->sAlgoDwnldDetail[%d].ui32AlgoDataPtr:%d",i,pCitInput->pMit->sAlgoDwnldDetail[i].ui32AlgoDataPtr));
            BDBG_MSG(("psCitInputStruct->psMit->sAlgoDwnldDetail[%d].ui32AlgoDataSize:%d",i,pCitInput->pMit->sAlgoDwnldDetail[i].ui32AlgoDataSize));
        }
    }
    BDBG_MSG(("\n"));

    /*Num Output is set to 0*/
    pCitInput->ui32NumOutputs = 0;

    /*indep delay is set to 0*/
    pCitInput->eIndepDelayEnableFlag = pSettings->maxIndependentDelay > 0 ? true : false;

    pCitInput->sStcTrigConfig.eStcTrigRequired = pSettings->stcIncrementConfig.enableStcTrigger;    
    pCitInput->sStcTrigConfig.ui32StcIncHiAddr = pSettings->stcIncrementConfig.stcIncHiAddr;
    pCitInput->sStcTrigConfig.ui32StcIncLowAddr = pSettings->stcIncrementConfig.stcIncLowAddr;
    pCitInput->sStcTrigConfig.ui32StcIncTrigAddr = pSettings->stcIncrementConfig.stcIncTrigAddr;
    pCitInput->sStcTrigConfig.ui32TriggerBit = pSettings->stcIncrementConfig.triggerBit;

    /* Assigning the sample rate LUT pointer */
    pCitInput->pSampleRateMap = pSettings->pSampleRateMap;
    
    return err;
}

static BERR_Code BDSP_Raaga_P_GenerateVideoCITInput(
    void *pTaskHandle, 
    const BDSP_TaskCreateSettings *pSettings,
    BDSP_CIT_P_InputInfo        *pCitInputStruct,    
    BDSP_VF_P_sVDecodeBuffCfg   *psVDecodeBuffCfg
    )
{
    BERR_Code                       ret=BERR_SUCCESS;

    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
    BDSP_RaagaContext *pRaagaContext = pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;    
    unsigned int        i=0;
    uint32_t            ui32RegOffset = 0;  

    BDBG_ENTER(BDSP_Raaga_P_GenerateVideoCITInput);

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pCitInputStruct);


    /*Populate CIT Input*/
    pCitInputStruct->ui32NumBranches = pSettings->numBranches;
    for (i = 0; i< BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
        pCitInputStruct->pBranchInfo[i] = pSettings->pBranchInfo[i];

    /* Show all the input CIT structure*/
    BDSP_P_ShowCitSettings(pSettings);

    pCitInputStruct->sRdbCfg.ui32RdbBaseOffset = 0;
    pCitInputStruct->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;
    pCitInputStruct->eTimeBaseType = pSettings->timeBaseType;

    /* Assigning Mit Structure */
    pCitInputStruct->pMit = &(pDevice->mit);

    /* Assigning CIT memInfo */
    pCitInputStruct->memInfo.ui32SharedMemPhysAdr = pRaagaContext->contextMemInfo.sDspScratchInfo[pSettings->dspIndex].ui32BaseAddr;        
    pCitInputStruct->memInfo.ui32SharedMemSize = pRaagaContext->contextMemInfo.sDspScratchInfo[pSettings->dspIndex].ui32Size;
    pCitInputStruct->memInfo.ui32TaskMemPhysAdr = pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32BaseAddr;
    pCitInputStruct->memInfo.ui32TaskMemSize = pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32Size;
    pCitInputStruct->sRdbCfg.ui32RdbBaseOffset = 0;
    pCitInputStruct->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;


    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32SharedMemPhysAdr = 0x%x",pCitInputStruct->memInfo.ui32SharedMemPhysAdr));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32SharedMemSize = %d",pCitInputStruct->memInfo.ui32SharedMemSize));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32TaskMemPhysAdr = 0x%x",pCitInputStruct->memInfo.ui32TaskMemPhysAdr));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32TaskMemSize = %d",pCitInputStruct->memInfo.ui32TaskMemSize));

    pCitInputStruct->eTimeBaseType = pSettings->timeBaseType;

    /* BALA: TODO: Check with F/W team if this is the physical address */
    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
        (ui32RegOffset * BDSP_RAAGA_DSP_P_FIFO_PDQ) + \
        + BCHP_PHYSICAL_OFFSET;

    psVDecodeBuffCfg->sPDQ.ui32BaseAddr = ui32RegOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;
    psVDecodeBuffCfg->sPDQ.ui32EndAddr  = ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;
    psVDecodeBuffCfg->sPDQ.ui32WriteAddr    = ui32RegOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
    psVDecodeBuffCfg->sPDQ.ui32ReadAddr = ui32RegOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
    /* This is not used by F/W */
    psVDecodeBuffCfg->sPDQ.ui32WrapAddr = ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
        (ui32RegOffset * BDSP_RAAGA_DSP_P_FIFO_PRQ) + \
        + BCHP_PHYSICAL_OFFSET;

    psVDecodeBuffCfg->sPRQ.ui32BaseAddr = ui32RegOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;
    psVDecodeBuffCfg->sPRQ.ui32EndAddr  = ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;
    psVDecodeBuffCfg->sPRQ.ui32WriteAddr    = ui32RegOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
    psVDecodeBuffCfg->sPRQ.ui32ReadAddr = ui32RegOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
    /* This is not used by F/W */
    psVDecodeBuffCfg->sPRQ.ui32WrapAddr = ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

    psVDecodeBuffCfg->ui32MaxFrameWidth = 720;
    psVDecodeBuffCfg->ui32MaxFrameHeight = 576;

    psVDecodeBuffCfg->ui32StripeWidth = 128;

    psVDecodeBuffCfg->sDisplayFrameBuffParams.ui32NumBuffAvl = 
        pSettings->psVDecodeBuffCfg->sDisplayFrameBuffParams.ui32NumBuffAvl;

    /*(CEILING((MaxVertSize/2)/16) x 16)*/
    /*(CEILING((576/2)/16) x 16) */
    psVDecodeBuffCfg->sDisplayFrameBuffParams.ui32ChromaStripeHeight = 288;

    /*(CEILING(MaxVertSize/16) x 16) */
    /*(CEILING(576/16) x 16) */
    psVDecodeBuffCfg->sDisplayFrameBuffParams.ui32LumaStripeHeight = 576;

    for (i=0;i<psVDecodeBuffCfg->sDisplayFrameBuffParams.ui32NumBuffAvl;i++)
    {
        /* Luma Buffer*/
        psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress = \
            pSettings->psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress;
        psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes = \
            pSettings->psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes;

        /* Chroma Buffer*/            
        psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress = \
            pSettings->psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress;
        psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes = \
            pSettings->psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes;

        /* UPB */    
        psVDecodeBuffCfg->sUPBs[i].ui32DramBufferAddress= 
            pSettings->psVDecodeBuffCfg->sUPBs[i].ui32DramBufferAddress;
        psVDecodeBuffCfg->sUPBs[i].ui32BufferSizeInBytes= 
            pSettings->psVDecodeBuffCfg->sUPBs[i].ui32BufferSizeInBytes;      
    }

    /*TotalHorzPadd = 96, TotalVertPadd_Luma = 96, and TotalVertPadd_Chroma = 48*/
    /*(CEILING(((MaxVertSize/2) + TotalVertPadd_Chroma)/16) x 16)*/
    psVDecodeBuffCfg->sReferenceBuffParams.ui32ChromaStripeHeight = 336;

    /*(CEILING((MaxVertSize + TotalVertPadd_Luma)/16) x 16)*/
    psVDecodeBuffCfg->sReferenceBuffParams.ui32LumaStripeHeight = 672;

    psVDecodeBuffCfg->sReferenceBuffParams.ui32NumBuffAvl = 
        pSettings->psVDecodeBuffCfg->sReferenceBuffParams.ui32NumBuffAvl;

    for (i=0;i<psVDecodeBuffCfg->sReferenceBuffParams.ui32NumBuffAvl;i++)
    {
        psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress = \
            pSettings->psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress;            
        psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes = \
            pSettings->psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes;

        psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress = \
            pSettings->psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress;            
        psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes = \
            pSettings->psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes;
    }

    BDBG_LEAVE(BDSP_Raaga_P_GenerateVideoCITInput);
    return ret;
}

static BERR_Code BDSP_Raaga_P_GenerateScmCITInput(
    void 			*pTaskHandle, 
    const BDSP_TaskCreateSettings *pSettings,
    BDSP_CIT_P_InputInfo        *pCitInput    
    )
{
    BERR_Code   err=BERR_SUCCESS;

    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
    BDSP_RaagaContext *pRaagaContext = pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    unsigned i=0;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    /*Populate CIT Input*/
    pCitInput->ui32NumBranches = pSettings->numBranches;
    for (i = 0; i< BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
        pCitInput->pBranchInfo[i] = pSettings->pBranchInfo[i];

    /* Show all the input CIT structure*/
    BDSP_P_ShowCitSettings(pSettings);

    pCitInput->sRdbCfg.ui32RdbBaseOffset = 0;
    pCitInput->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;
    pCitInput->eTimeBaseType = pSettings->timeBaseType;

    /* Assigning Mit Structure */
    pCitInput->pMit = &(pDevice->mit);

    /* Assigning CIT memInfo */
    pCitInput->memInfo.ui32SharedMemPhysAdr = pRaagaContext->contextMemInfo.sDspScratchInfo[pSettings->dspIndex].ui32BaseAddr;      
    pCitInput->memInfo.ui32SharedMemSize = pRaagaContext->contextMemInfo.sDspScratchInfo[pSettings->dspIndex].ui32Size;
    pCitInput->memInfo.ui32TaskMemPhysAdr = pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32BaseAddr;
    pCitInput->memInfo.ui32TaskMemSize = pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32Size;


    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32SharedMemPhysAdr = 0x%x",pCitInput->memInfo.ui32SharedMemPhysAdr));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32SharedMemSize = %d",pCitInput->memInfo.ui32SharedMemSize));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32TaskMemPhysAdr = 0x%x",pCitInput->memInfo.ui32TaskMemPhysAdr));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32TaskMemSize = %d",pCitInput->memInfo.ui32TaskMemSize));

    BDBG_MSG(("\n"));
    BDBG_MSG(("Details of the master Index Table "));
    BDBG_MSG(("=================================="));
    for (i=0;i<BDSP_AF_P_AlgoId_eMax;i++)
    {
        if (pCitInput->pMit==NULL)
        {
            continue;
        }
        if (pCitInput->pMit->sAlgoDwnldDetail[i].ui32AlgoCodePtr != 0xffffffff)
        {
            BDBG_MSG(("psCitInputStruct->psMit->sAlgoDwnldDetail[%d].ui32AlgoCodePtr :%d",i,pCitInput->pMit->sAlgoDwnldDetail[i].ui32AlgoCodePtr));
            BDBG_MSG(("psCitInputStruct->psMit->sAlgoDwnldDetail[%d].ui32AlgoCodeSize :%d",i,pCitInput->pMit->sAlgoDwnldDetail[i].ui32Algoui32CodeSize));
            BDBG_MSG(("psCitInputStruct->psMit->sAlgoDwnldDetail[%d].ui32AlgoDataPtr:%d",i,pCitInput->pMit->sAlgoDwnldDetail[i].ui32AlgoDataPtr));
            BDBG_MSG(("psCitInputStruct->psMit->sAlgoDwnldDetail[%d].ui32AlgoDataSize:%d",i,pCitInput->pMit->sAlgoDwnldDetail[i].ui32AlgoDataSize));
        }
    }
    BDBG_MSG(("\n"));

    /*Num Output is set to 0*/
    pCitInput->ui32NumOutputs = 0;

    /*indep delay is set to 0*/
    pCitInput->eIndepDelayEnableFlag = pSettings->maxIndependentDelay > 0 ? true : false;

    pCitInput->sStcTrigConfig.eStcTrigRequired = pSettings->stcIncrementConfig.enableStcTrigger;    
    pCitInput->sStcTrigConfig.ui32StcIncHiAddr = pSettings->stcIncrementConfig.stcIncHiAddr;
    pCitInput->sStcTrigConfig.ui32StcIncLowAddr = pSettings->stcIncrementConfig.stcIncLowAddr;
    pCitInput->sStcTrigConfig.ui32StcIncTrigAddr = pSettings->stcIncrementConfig.stcIncTrigAddr;
    pCitInput->sStcTrigConfig.ui32TriggerBit = pSettings->stcIncrementConfig.triggerBit;

    return err;


}
static BERR_Code BDSP_Raaga_P_GenerateVideoEncodeCITInput(
    void 			*pTaskHandle, 
    const BDSP_TaskCreateSettings *pSettings,
    BDSP_CIT_P_InputInfo        *pCitInputStruct,    
    BDSP_VF_P_sVEncodeConfig   *psVEncodeBuffCfg
    )
{
    BERR_Code                       ret=BERR_SUCCESS;

    BDSP_RaagaTask 					*pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
    BDSP_RaagaContext 				*pRaagaContext = pRaagaTask->pContext;
    BDSP_Raaga  					*pDevice= pRaagaContext->pDevice;    
    unsigned int        			i=0;
	void 							*pvRrqAddr=NULL;

    BDBG_ENTER(BDSP_Raaga_P_GenerateVideoEncodeCITInput);

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pCitInputStruct);


    /*Populate CIT Input*/
    pCitInputStruct->ui32NumBranches = pSettings->numBranches;
    for (i = 0; i< BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
        pCitInputStruct->pBranchInfo[i] = pSettings->pBranchInfo[i];

	/* Hook RDQ/RRQ to CIT's branch input */

	/* From base Address of pRaagaTask->hRRQueue fill all the buffers. Fill RRQ in the beginning */
	/* This conversion is done to pass a virtual address as the second argument 
	    of BDSP_Raaga_P_MemWrite32 */
    ret = BMEM_ConvertOffsetToAddress(
            pDevice->memHandle, pRaagaTask->hRRQueue->uiBaseAddr, (void **)&pvRrqAddr);
    if (BERR_SUCCESS!= ret)
    {   
    	return BERR_TRACE(ret);
    }
	
	for(i = 0; i < BDSP_FWMAX_VIDEO_BUFF_AVAIL; i++)
	{
		BDSP_Raaga_P_MemWrite32(pDevice->memHandle, (uint32_t)pvRrqAddr+(i*4), pSettings->psVEncoderConfig->sPPBs[i].ui32DramBufferAddress);
	}
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumSrc = 1;
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eRDB;
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eRDB;
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 2;
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = pRaagaTask->hRDQueue->ui32FifoId;
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[1] = pRaagaTask->hRRQueue->ui32FifoId;
	/* May not be needed. To be sure make them INVALID */
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[0] = -1;
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[1] = -1;
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[2] = -1;
	pCitInputStruct->pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[3] = -1;
	
    /* Show all the input CIT structure*/
    BDSP_P_ShowCitSettings(pSettings);

    pCitInputStruct->sRdbCfg.ui32RdbBaseOffset = 0;
    pCitInputStruct->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;
    pCitInputStruct->eTimeBaseType = pSettings->timeBaseType;

    /* Assigning Mit Structure */
    pCitInputStruct->pMit = &(pDevice->mit);

    /* Assigning CIT memInfo */
    pCitInputStruct->memInfo.ui32SharedMemPhysAdr = pRaagaContext->contextMemInfo.sDspScratchInfo[pSettings->dspIndex].ui32BaseAddr;        
    pCitInputStruct->memInfo.ui32SharedMemSize = pRaagaContext->contextMemInfo.sDspScratchInfo[pSettings->dspIndex].ui32Size;
    pCitInputStruct->memInfo.ui32TaskMemPhysAdr = pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32BaseAddr;
    pCitInputStruct->memInfo.ui32TaskMemSize = pRaagaTask->taskMemGrants.sTaskIFrameCfgBufInfo.ui32Size;
    pCitInputStruct->sRdbCfg.ui32RdbBaseOffset = 0;
    pCitInputStruct->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;

    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32SharedMemPhysAdr = 0x%x",pCitInputStruct->memInfo.ui32SharedMemPhysAdr));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32SharedMemSize = %d",pCitInputStruct->memInfo.ui32SharedMemSize));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32TaskMemPhysAdr = 0x%x",pCitInputStruct->memInfo.ui32TaskMemPhysAdr));
    BDBG_MSG(("psCitInputStruct->sMemInfo.ui32TaskMemSize = %d",pCitInputStruct->memInfo.ui32TaskMemSize));

    pCitInputStruct->eTimeBaseType = pSettings->timeBaseType;
	
	psVEncodeBuffCfg->sEncoderParams.ui32Frames2Accum = pSettings->psVEncoderConfig->sEncoderParams.ui32Frames2Accum;
	psVEncodeBuffCfg->sEncoderParams.eEncodeFrameRate = pSettings->psVEncoderConfig->sEncoderParams.eEncodeFrameRate;
	/* Copy the interrupt bits */
	psVEncodeBuffCfg->sEncoderParams.ui32InterruptBit[0] = pSettings->extInterruptConfig.interruptInfo[0].interruptBit;
	psVEncodeBuffCfg->sEncoderParams.ui32InterruptBit[1] = pSettings->extInterruptConfig.interruptInfo[1].interruptBit;
	/* Give the Stc Address */
	psVEncodeBuffCfg->sEncoderParams.ui32StcAddr = pSettings->videoEncRdbConfig.stcAddress;
    psVEncodeBuffCfg->ui32MaxFrameWidth = pSettings->psVEncoderConfig->ui32MaxFrameWidth;
    psVEncodeBuffCfg->ui32MaxFrameHeight = pSettings->psVEncoderConfig->ui32MaxFrameHeight;

    psVEncodeBuffCfg->ui32StripeWidth = pSettings->psVEncoderConfig->ui32StripeWidth;

    /*TotalHorzPadd = 96, TotalVertPadd_Luma = 96, and TotalVertPadd_Chroma = 48*/
    /*(CEILING(((MaxVertSize/2) + TotalVertPadd_Chroma)/16) x 16)*/
    psVEncodeBuffCfg->sReferenceBuffParams.ui32ChromaStripeHeight = pSettings->psVEncoderConfig->sReferenceBuffParams.ui32ChromaStripeHeight;

    /*(CEILING((MaxVertSize + TotalVertPadd_Luma)/16) x 16)*/
    psVEncodeBuffCfg->sReferenceBuffParams.ui32LumaStripeHeight = pSettings->psVEncoderConfig->sReferenceBuffParams.ui32LumaStripeHeight;

    psVEncodeBuffCfg->sReferenceBuffParams.ui32NumBuffAvl = 
        pSettings->psVEncoderConfig->sReferenceBuffParams.ui32NumBuffAvl;

    for (i=0;i<psVEncodeBuffCfg->sReferenceBuffParams.ui32NumBuffAvl;i++)
    {
        psVEncodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress = \
            pSettings->psVEncoderConfig->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress;            
        psVEncodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes = \
            pSettings->psVEncoderConfig->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes;

        psVEncodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress = \
            pSettings->psVEncoderConfig->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress;            
        psVEncodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes = \
            pSettings->psVEncoderConfig->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes;
    }

    BDBG_LEAVE(BDSP_Raaga_P_GenerateVideoEncodeCITInput);
    return ret;
}


static BERR_Code BDSP_P_GetInterframeImgId(
    void *pDeviceHandle,
    BDSP_AF_P_AlgoId    eFwExecId,
    uint32_t    *ui32ImgId,
    bool *isDownloaded)
{
    BDSP_Raaga  *pDevice= (BDSP_Raaga  *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

    BDBG_ENTER(BDSP_P_GetInterframeImgId);

    *isDownloaded = false;

    if (eFwExecId < BDSP_AF_P_AlgoId_eMax)
    {
        *ui32ImgId = pDevice->imgIdMappingArrays.interframeImgId[eFwExecId];
    }
    else
    {
        BDBG_ERR(("Invalid FW exec ID"));
        return BERR_INVALID_PARAMETER;
    }

    BDBG_LEAVE(BDSP_P_GetInterframeImgId);
    return BERR_SUCCESS;
}

static BERR_Code BDSP_VID_P_InitInterframeBuffer(void *pTaskHandle)
{
    BDSP_CIT_P_VideoCITOutput   *pCitOutput = NULL;
    BDSP_CIT_P_FwBufInfo *pOpNodeInfo = NULL;
    BERR_Code   rc = BERR_SUCCESS;
    unsigned int i,j,k;
    const BIMG_Interface *iface = NULL; 
    void *pImgContext = NULL;
    uint32_t    ui32ImgId = BDSP_Img_Id_eInvalid,ui32physAddress=0;
    bool isDownloaded =false;
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
    BDSP_RaagaContext *pRaagaContext = pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;

    BDBG_ENTER(BDSP_VID_P_InitInterframeBuffer);

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    iface = pDevice->settings.pImageInterface;  
    pImgContext = pDevice->settings.pImageContext;
    pCitOutput = &(pRaagaTask->videoCitOutput);

    for (i = 0;i < pCitOutput->ui32NumBranches; i++)
    {
        for (j = 0; j < pCitOutput->sCitBranchInfo[i].ui32NumStages; j++)
        {
            for (k = 0; k < pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].ui32NumNodes; k++)
            {
                if (pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].sFwOpNodeInfo[k].eFwExecId != BDSP_AF_P_AlgoId_eMax)
                {
                    pOpNodeInfo = &(pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].sFwOpNodeInfo[k]);

                    /* check if enough memory is allocated for interframe buffer */
                    if (pOpNodeInfo->ui32InterframeBufSize < pDevice->imgIdMappingArrays.interframeSize[pOpNodeInfo->eFwExecId])
                    {
                        BDBG_ERR(("Allocated memory (%d) for video interframe buffer is less than required (%d) for ExecId =%#x",
                            pOpNodeInfo->ui32InterframeBufSize ,
                            pDevice->imgIdMappingArrays.interframeSize[pOpNodeInfo->eFwExecId],
                            pOpNodeInfo->eFwExecId));
                    }
                    else
                    {
                        rc = BDSP_P_GetInterframeImgId((void *)pDevice,pOpNodeInfo->eFwExecId,&ui32ImgId,&isDownloaded);
                        if (BERR_SUCCESS != rc)
                        {
                            return BERR_TRACE(rc);
                        }
                        BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(pOpNodeInfo->ui32InterframeBufAdr), &ui32physAddress); 
                        BDBG_MSG(("pOpNodeInfo->ui32InterframeBufAdr %#x physical %#x",pOpNodeInfo->ui32InterframeBufAdr,ui32physAddress));


                        if (BDSP_Img_Id_eInvalid == ui32ImgId)
                        {
                            BDBG_MSG(("No FW Image file exists for Exec Id %#x",pOpNodeInfo->eFwExecId));
                        }
                        else if (false == isDownloaded)
                        {
                            BDBG_MSG(("pOpNodeInfo->eFwExecId > %d, ui32ImgId > %d", pOpNodeInfo->eFwExecId, ui32ImgId));
                            rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,pOpNodeInfo->ui32InterframeBufAdr,ui32ImgId,pDevice->memHandle);
                            if (rc != BERR_SUCCESS)
                            {
                                rc = BERR_TRACE(rc);
                                goto end;
                            }
                        }
                    }
                }
                else
                    break;

            }
        }
    }
    end:    
    BDBG_LEAVE(BDSP_VID_P_InitInterframeBuffer);
    return rc;
}

static BERR_Code BDSP_DSP_P_InitInterframeBuffer(void *pTaskHandle)
{
    BDSP_CIT_P_Output   *pCitOutput = NULL;
    BDSP_CIT_P_FwBufInfo *pOpNodeInfo = NULL;
    BERR_Code   rc = BERR_SUCCESS;
    unsigned int i,j,k;
    const BIMG_Interface *iface = NULL; 
    void *pImgContext = NULL;
    uint32_t    ui32ImgId = BDSP_Img_Id_eInvalid,ui32physAddress=0;
    bool isDownloaded =false;
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;
    BDSP_RaagaContext *pRaagaContext = pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    BDBG_ENTER(BDSP_DSP_P_InitInterframeBuffer);

    iface = pDevice->settings.pImageInterface;  
    pImgContext = pDevice->settings.pImageContext;
    pCitOutput = &(pRaagaTask->citOutput);

    for (i = 0;i < pCitOutput->ui32NumBranches; i++)
    {
        for (j = 0; j < pCitOutput->sCitBranchInfo[i].ui32NumStages; j++)
        {
            for (k = 0; k < pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].ui32NumNodes; k++)
            {
                if (pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].sFwOpNodeInfo[k].eFwExecId != BDSP_AF_P_AlgoId_eMax)
                {
                    pOpNodeInfo = &(pCitOutput->sCitBranchInfo[i].sCitStgInfo[j].sFwOpNodeInfo[k]);

                    /* check if enough memory is allocated for interframe buffer */
                    if (pOpNodeInfo->ui32InterframeBufSize < pDevice->imgIdMappingArrays.interframeSize[pOpNodeInfo->eFwExecId])
                    {
                        BDBG_ERR(("Allocated memory (%d) for interframe buffer is less than required (%d) for ExecId =%#x",
                            pOpNodeInfo->ui32InterframeBufSize ,
                            pDevice->imgIdMappingArrays.interframeSize[pOpNodeInfo->eFwExecId],
                            pOpNodeInfo->eFwExecId));
                    }
                    else
                    {
                        rc = BDSP_P_GetInterframeImgId((void *)pDevice,pOpNodeInfo->eFwExecId,&ui32ImgId,&isDownloaded);
                        if (BERR_SUCCESS != rc)
                        {
                            return BERR_TRACE(rc);
                        }
                        BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(pOpNodeInfo->ui32InterframeBufAdr), &ui32physAddress); 
                        BDBG_MSG(("pOpNodeInfo->ui32InterframeBufAdr %#x physical %#x",pOpNodeInfo->ui32InterframeBufAdr,ui32physAddress));


                        if (BDSP_Img_Id_eInvalid == ui32ImgId)
                        {
                            BDBG_MSG(("No FW Image file exists for Exec Id %#x",pOpNodeInfo->eFwExecId));
                        }
                        else if (false == isDownloaded)
                        {
                            BDBG_MSG(("pOpNodeInfo->eFwExecId > %d, ui32ImgId > %d", pOpNodeInfo->eFwExecId, ui32ImgId));
                            rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,pOpNodeInfo->ui32InterframeBufAdr,ui32ImgId,pDevice->memHandle);
                            if (rc != BERR_SUCCESS)
                            {
                                rc = BERR_TRACE(rc);
                                goto end;
                            }
                        }
                    }
                }
                else
                    break;

            }
        }
    }

    end:    
    BDBG_LEAVE(BDSP_DSP_P_InitInterframeBuffer);
    return rc;

}

static BERR_Code
    BDSP_DSPCHN_P_SendVOMChangeCommand (
    void *pTaskHandle
    )
{
    BDSP_Raaga_P_Command sCommand;
    BDSP_VOM_Table  *pVOMTableInDRAM = NULL;
    uint32_t    physAddress = 0;
    unsigned int i,j,index = 0;
    BERR_Code err = BERR_SUCCESS;    
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;        
    BDSP_Raaga  *pDevice= pRaagaTask->pContext->pDevice;
    void *pCached;

    BDBG_ENTER (BDSP_DSPCHN_P_SendVOMChangeCommand);
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);


    pVOMTableInDRAM = (BDSP_VOM_Table *)pRaagaTask->pContext->contextMemInfo.sVomTableInfo.ui32BaseAddr;
    (void)BMEM_ConvertAddressToCached(pDevice->memHandle, pVOMTableInDRAM, &pCached);
    pVOMTableInDRAM = pCached;

    if (NULL == pVOMTableInDRAM)
    {
        BDBG_ERR (("BDSP_DSPCHN_P_SendVOMChangeCommand :: The allocated address"
            " for VOM table is not proper. Please check the allocation"
            " in BDSP_MM_RequestMemoryAllocation"));

        return BERR_TRACE (BDSP_ERR_BAD_DEVICE_STATE);
    }

    BKNI_Memset(pVOMTableInDRAM, 0,(sizeof(BDSP_VOM_Table)));
    BKNI_Memset(&sCommand,0,sizeof(sCommand));
    for (i=0; i<BDSP_AF_P_AlgoId_eMax; i++)
    {
        if (pDevice->mit.sAlgoDwnldDetail[i].ui32AlgoCodePtr != 0xffffffff)
        {
            BDBG_MSG (("The algo ID %d is valid. So make an index for it in "
                "VOM Table",i));
            pVOMTableInDRAM->sVomTableDetail[index].ui32PageStart = \
                BDSP_sVomTable.sVomTableDetail[i].ui32PageStart;

            pVOMTableInDRAM->sVomTableDetail[index].ui32PageEnd = \
                BDSP_sVomTable.sVomTableDetail[i].ui32PageEnd;

            BMEM_ConvertAddressToOffset(
                pDevice->memHandle, 
                (void *)(pDevice->mit.sAlgoDwnldDetail[i].ui32AlgoCodePtr), 
                &physAddress);

            pVOMTableInDRAM->sVomTableDetail[index].ui32DramAddr = physAddress;
            index++;            
        }
    }

    /* Sort the VOM Table based on the Page Start address */
    if (index > 1)
    {
        BDSP_VOM_Table_Entry tempEntry;

        for (i=0;i<index-1;i++)
        {
            for (j=i; j<index;j++)
            {
                if (pVOMTableInDRAM->sVomTableDetail[i].ui32PageStart > 
                    pVOMTableInDRAM->sVomTableDetail[j].ui32PageStart)
                {
                    tempEntry = pVOMTableInDRAM->sVomTableDetail[i];
                    pVOMTableInDRAM->sVomTableDetail[i] = \
                        pVOMTableInDRAM->sVomTableDetail[j];
                    pVOMTableInDRAM->sVomTableDetail[j] = tempEntry;
                }
            }
        }
    }

    /* Print VOM for debug */
    BDBG_MSG (("VOM TABLE ... with %d valid entries",index));
    for (i=0; i<index; i++)
    {
        BDBG_MSG (("Entry ID: %d, Page Start:%x, Page End:%x, DRAM:%x",
            i, pVOMTableInDRAM->sVomTableDetail[i].ui32PageStart,
            pVOMTableInDRAM->sVomTableDetail[i].ui32PageEnd,
            pVOMTableInDRAM->sVomTableDetail[i].ui32DramAddr));
    }

    (void)BMEM_FlushCache(pDevice->memHandle, pCached, sizeof(BDSP_VOM_Table));
    BMEM_ConvertAddressToOffset(
        pDevice->memHandle, (void *)pRaagaTask->pContext->contextMemInfo.sVomTableInfo.ui32BaseAddr, &physAddress);

    BDBG_MSG (("VOM Table's location in DRAM : %x",physAddress));    

    sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_GET_VOM_TABLE_COMMAND_ID;
    sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
    sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
    sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eNone;
    sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

    sCommand.uCommand.sGetVomTable.ui32NumEntries = index;
    sCommand.uCommand.sGetVomTable.ui32HostVomTableAddr = physAddress;

    err = BDSP_Raaga_P_SendCommand(
        pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);

    if (BERR_SUCCESS != err)
    {
        BDBG_ERR(("VOM Command failed!"));
        return BERR_TRACE(err);
    }

    BDBG_LEAVE (BDSP_DSPCHN_P_SendVOMChangeCommand);
    return BERR_SUCCESS;
}


static BERR_Code   BDSP_Raaga_P_AllocateAndInitTask(
    void *pContextHandle,
    void *pTaskHandle,
    const BDSP_TaskCreateSettings *pSettings
    )    
{
    BERR_Code   err=BERR_SUCCESS;
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;    
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    unsigned taskId,j;
	int32_t	i, k = 0;
    unsigned ui32AsyncFifo,ui32SyncFifo,uiOffset;
    uint32_t ui32RegOffset = 0;
    BDSP_RaagaTask *pMasterRaagaTask;     

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    /* Mark task as in use and save settings */
    BLST_S_REMOVE_HEAD(&pRaagaContext->freeTaskList, node);
    pRaagaTask->allocated = true;

    BKNI_Memcpy(&pRaagaTask->settings, pSettings, sizeof(*pSettings));
    BKNI_Memset(&pRaagaTask->audioInterruptHandlers, 0, sizeof(pRaagaTask->audioInterruptHandlers));

    pRaagaTask->isStopped = true;
    pRaagaTask->lastEventType= 0xFFFF;                 
    pRaagaTask->settings = *pSettings;
    pRaagaTask->commandCounter =0;
    pRaagaTask->paused=false;
    pRaagaTask->decLocked=false;
    pRaagaTask->pFeedbackBuffer = NULL;
    pRaagaTask->hPDQueue = NULL;
    pRaagaTask->hPRQueue = NULL;
    pRaagaTask->hDSQueue = NULL;

    pRaagaTask->masterTaskId = BDSP_RAAGA_P_INVALID_TASK_ID;
    pRaagaTask->schedulingMode = pSettings->schedulingMode;    


    if ( pSettings->schedulingMode  != BDSP_TaskSchedulingMode_eSlave && pSettings->masterTask != NULL )
    {
        BDBG_ERR(("master task handle should be set only for Slave scheduling mode"));
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto error_invalid_parameters;
    }

    if ((pSettings->schedulingMode== BDSP_TaskSchedulingMode_eSlave) && (pSettings->masterTask !=NULL))
    {
        pMasterRaagaTask = (BDSP_RaagaTask *)(pSettings->masterTask->pTaskHandle);
        BDBG_OBJECT_ASSERT(pMasterRaagaTask, BDSP_RaagaTask);    
        pRaagaTask->masterTaskId = pMasterRaagaTask->taskId;
        pRaagaTask->pFeedbackBuffer = pMasterRaagaTask->pFeedbackBuffer;
    }
    else if (pSettings->schedulingMode == BDSP_TaskSchedulingMode_eMaster)
    {
        void *pCached;
        pRaagaTask->masterTaskId = BDSP_RAAGA_P_INVALID_TASK_ID;     
        pRaagaTask->pFeedbackBuffer = BMEM_AllocAligned(pDevice->memHandle, (BDSP_AF_P_INTERTASK_FEEDBACK_BUFFER_SIZE + 4) ,5, 0);    /* 32 bit aligned*/
        if ( NULL == pRaagaTask->pFeedbackBuffer )
        {
            BDBG_ERR(("Unable to Allocate memory for Feedback  Buffer !"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto error;
        }
        (void)BMEM_ConvertAddressToCached(pDevice->memHandle, pRaagaTask->pFeedbackBuffer, &pCached);
        BKNI_Memset(pCached, 0, (BDSP_AF_P_INTERTASK_FEEDBACK_BUFFER_SIZE + 4));
        (void)BMEM_FlushCache(pDevice->memHandle, pCached, (BDSP_AF_P_INTERTASK_FEEDBACK_BUFFER_SIZE + 4));
    }

    uiOffset = pDevice->dspOffset[pSettings->dspIndex];

    BKNI_AcquireMutex(pDevice->taskDetails.taskIdMutex);
    for (taskId = 0 ; taskId < BDSP_RAAGA_MAX_FW_TASK_PER_DSP; taskId++)
    {
        if (pDevice->taskDetails.taskId[pSettings->dspIndex][taskId] == true)
        {
            pDevice->taskDetails.taskId[pSettings->dspIndex][taskId] = false;
            break;
        }
    }

    if (taskId >=BDSP_RAAGA_MAX_FW_TASK_PER_DSP)
    {
        BKNI_ReleaseMutex(pDevice->taskDetails.taskIdMutex);    
        BDBG_ERR(("Unable to find free Task Instance!"));       
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);        
        goto err_release_mutex;           
    }

    pRaagaTask->taskId  = taskId + BDSP_RAAGA_TASK_ID_START_OFFSET;

    pDevice->taskDetails.pRaagaTask[pSettings->dspIndex][taskId]  = pRaagaTask;

    BKNI_ReleaseMutex(pDevice->taskDetails.taskIdMutex);

    BKNI_AcquireMutex(pDevice->fifoIdMutex[pSettings->dspIndex]);    
    /* Find free Fifo Ids */
    for (j=0;j<BDSP_RAAGA_NUM_FIFOS;j++)
    {
        if (false == pDevice->dspFifo[pSettings->dspIndex][j])
            break;
    }

    if (j >= BDSP_RAAGA_NUM_FIFOS)
    {
        BKNI_ReleaseMutex(pDevice->fifoIdMutex[pSettings->dspIndex]);                
        BDBG_ERR(("Unable to find free fifo for SYNC MSG!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_release_mutex;           
    }
    else
    {
        pDevice->dspFifo[pSettings->dspIndex][j] = true;
        ui32SyncFifo = j+BDSP_RAAGA_FIFO_0_INDEX;
    }

    for (j=0;j<BDSP_RAAGA_NUM_FIFOS;j++)
    {
        if (false == pDevice->dspFifo[pSettings->dspIndex][j])
            break;
    }
    if (j >= BDSP_RAAGA_NUM_FIFOS)
    {
        BKNI_ReleaseMutex(pDevice->fifoIdMutex[pSettings->dspIndex]);    
        BDBG_ERR(("Unable to find free fifo for ASYNC MSG!"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_release_mutex;           
    }
    else
    {
        pDevice->dspFifo[pSettings->dspIndex][j] = true;
        ui32AsyncFifo = j+BDSP_RAAGA_FIFO_0_INDEX;
    }
    BKNI_ReleaseMutex(pDevice->fifoIdMutex[pSettings->dspIndex]);    

    pRaagaTask->taskMemGrants.sTaskQueue.sTaskSyncQueue.ui32FifoId
        = ui32SyncFifo;
    pRaagaTask->taskMemGrants.sTaskQueue.sTaskAsyncQueue.ui32FifoId
        = ui32AsyncFifo;


    err = BDSP_Raaga_P_CreateMsgQueue(&(pRaagaTask->taskMemGrants.sTaskQueue.sTaskSyncQueue), 
        pDevice->memHandle, pDevice->regHandle, uiOffset, &(pRaagaTask->hSyncMsgQueue),false);
    if (BERR_SUCCESS != err)
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateAndInitTask: SYNC RSP queue creation failed!"));
        err = BERR_TRACE(err);
        goto err_create_syncqueue;
    }

    err = BDSP_Raaga_P_CreateMsgQueue(&(pRaagaTask->taskMemGrants.sTaskQueue.sTaskAsyncQueue), 
        pDevice->memHandle, pDevice->regHandle, uiOffset, &(pRaagaTask->hAsyncMsgQueue),false);
    if (BERR_SUCCESS != err)
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateAndInitTask: ASYNC RSP queue creation failed!"));
        err = BERR_TRACE(err);
        goto err_create_asyncqueue;
    }


    if (pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        /* Picture delivery queue */
        err = BDSP_Raaga_P_CreateMsgQueue(&(pRaagaTask->taskMemGrants.sTaskQueue.sPDQueue), pDevice->memHandle, 
            pDevice->regHandle, uiOffset,&(pRaagaTask->hPDQueue),false);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_AllocateAndInitTask: Picture delivery queue creation failed!"));
            goto err_create_PDQueue;

        }

        /*Picture release queue*/
        err = BDSP_Raaga_P_CreateMsgQueue(&(pRaagaTask->taskMemGrants.sTaskQueue.sPRQueue), pDevice->memHandle, 
            pDevice->regHandle, uiOffset,&(pRaagaTask->hPRQueue),false);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_AllocateAndInitTask: Picture release queue creation failed!"));
            goto err_create_PRQueue;

        }


        /*Display  queue*/
        err = BDSP_Raaga_P_CreateMsgQueue(&(pRaagaTask->taskMemGrants.sTaskQueue.sDSQueue), pDevice->memHandle, 
            pDevice->regHandle, uiOffset,&(pRaagaTask->hDSQueue),false);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_AllocateAndInitTask: display queue creation failed!"));
            goto err_create_DSQueue;

        }

    }

	/* Video encode context needs xtra Q's namely RDQ and RRQ. Creating them here */
    else if (BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType)
    {
        /* Raw Picture delivery queue */
		/* This is not going to be a MsgQueue in real sense. This will hold RDB addresses now. */
		
		BKNI_AcquireMutex(pDevice->fifoIdMutex[pSettings->dspIndex]);	 
		/* Find free Fifo Ids */
		k = 0;
		for (i=BDSP_RAAGA_MAX_TASK_FIFO_REQUIRED; i < (int32_t)BDSP_RAAGA_NUM_FIFOS; i++)
		{
			if (false == pDevice->dspFifo[pSettings->dspIndex][i])
			{
				k++;				
				/* Found enough contiguous RDBs. Remember 1 FIFO has 4 RDBs */
				if(k >= (BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE >> 2))
				{
					break;
				}
			}
			else
			{
				k = 0;
			}
		}
		/* Find start index of free regs */
		k = i - (BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE >> 2) + 1;
		if (i >= (int32_t)BDSP_RAAGA_NUM_FIFOS)
		{
			BKNI_ReleaseMutex(pDevice->fifoIdMutex[pSettings->dspIndex]);				 
			BDBG_ERR(("Unable to find free RDBs for allocating RDQ for raaga encode!!"));
			err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
			goto err_release_mutex;
		}
		else
		{
			for(;i >= k; i--)
			{
				BDBG_MSG(("Allcating fifo (RDB) %d", i));
				pDevice->dspFifo[pSettings->dspIndex][i] = true;
			}
			/* This is the fifo ID from where RDBs are free */
			i = BDSP_RAAGA_FIFO_0_INDEX + k;
		}
		BKNI_ReleaseMutex(pDevice->fifoIdMutex[pSettings->dspIndex]);
		
		pRaagaTask->taskMemGrants.sTaskQueue.sRDQueue.startIndexOfFreeFifo = i;
		
        err = BDSP_Raaga_P_CreateRdbQueue(&(pRaagaTask->taskMemGrants.sTaskQueue.sRDQueue), pDevice->memHandle, 
            pDevice->regHandle, uiOffset,&(pRaagaTask->hRDQueue),false);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_AllocateAndInitTask: Raw Picture delivery queue creation failed!"));
            goto err_create_RDQueue;
        }

        /* Raw Picture release queue*/
        err = BDSP_Raaga_P_CreateMsgQueue(&(pRaagaTask->taskMemGrants.sTaskQueue.sRRQueue), pDevice->memHandle, 
            pDevice->regHandle, uiOffset,&(pRaagaTask->hRRQueue),false);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_AllocateAndInitTask: Raw Picture release queue creation failed!"));
            goto err_create_RRQueue;
        }

		/* RRQ should start with full buffer capacity */
		ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
						BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;
		BDSP_Write32(pRaagaTask->hRRQueue->hRegister, 
				BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + (ui32RegOffset * pRaagaTask->hRRQueue->ui32FifoId) + BDSP_RAAGA_P_FIFO_WRITE_OFFSET + uiOffset,
				pRaagaTask->hRRQueue->uiBaseAddr + (BDSP_FWMAX_VIDEO_BUFF_AVAIL*4));

    }

    err = BKNI_CreateEvent(&(pRaagaTask->hEvent));     
    if (BERR_SUCCESS != err)
    {
        BDBG_ERR(("BDSP_Raaga_P_AllocateAndInitTask: Unable to create event"));
        err = BERR_TRACE(err);
        goto err_create_event;
    }

    BKNI_ResetEvent(pRaagaTask->hEvent);

    BKNI_AcquireMutex(pDevice->captureMutex);
    BLST_S_INSERT_HEAD(&pRaagaTask->pContext->allocTaskList, pRaagaTask, node);
    BKNI_ReleaseMutex(pDevice->captureMutex);

    /* Success */
    return BERR_SUCCESS;

err_create_event:
    if (pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hDSQueue,
            pDevice->regHandle, uiOffset);
    }


err_create_DSQueue:
    if (pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hPRQueue,
            pDevice->regHandle, uiOffset);
    }

err_create_PRQueue:  
    if (pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hPDQueue,
            pDevice->regHandle, uiOffset);
    }

err_create_RRQueue:		
	if (BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType)
	{
		BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hRDQueue,
			pDevice->regHandle, uiOffset);
	}
	
err_create_RDQueue:
err_create_PDQueue:        
    BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hAsyncMsgQueue, 
        pDevice->regHandle, uiOffset);    

err_create_asyncqueue:

    BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hSyncMsgQueue, 
        pDevice->regHandle, uiOffset);     

err_create_syncqueue:     
err_release_mutex:

    BKNI_AcquireMutex(pDevice->captureMutex);
    BLST_S_REMOVE(&pRaagaTask->pContext->allocTaskList, pRaagaTask, BDSP_RaagaTask, node);
    BKNI_ReleaseMutex(pDevice->captureMutex);

error:
    if (err != BERR_SUCCESS)
    {
        if (pRaagaTask->pFeedbackBuffer)
            BMEM_Free(pDevice->memHandle, (void*)pRaagaTask->pFeedbackBuffer);
    }

error_invalid_parameters:
    BLST_S_INSERT_HEAD(&pRaagaTask->pContext->freeTaskList, pRaagaTask, node);
    return err;
}

static BERR_Code   BDSP_Raaga_P_FreeAndInvalidateTask(
    void *pTaskHandle
    )    
{
    BERR_Code   err=BERR_SUCCESS;
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;        
    BDSP_RaagaContext *pRaagaContext = pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    unsigned ui32AsyncFifo,ui32SyncFifo,uiOffset;
	int32_t i;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    uiOffset = pDevice->dspOffset[pRaagaTask->settings.dspIndex];

    if (pRaagaTask->schedulingMode == BDSP_TaskSchedulingMode_eMaster)
    {
        if ((pRaagaTask->pFeedbackBuffer!=NULL))
        {
            BDBG_MSG(("BDSP_Raaga_P_FreeAndInvalidateTask: Freeing pRaagaTask->pFeedbackBuffer = 0x%x",pRaagaTask->pFeedbackBuffer));
            BMEM_Free(pDevice->memHandle, (void*)pRaagaTask->pFeedbackBuffer);        
            pRaagaTask->pFeedbackBuffer = NULL;                        
        }
    }




    BKNI_DestroyEvent(pRaagaTask->hEvent);     

    if (pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        err = BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hPDQueue, 
            pDevice->regHandle, uiOffset);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_FreeAndInvalidateTask: Picture delivery queue destroy failed!"));
            err = BERR_TRACE(err);
        }

        err = BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hPRQueue, 
            pDevice->regHandle, uiOffset);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_FreeAndInvalidateTask: Picture release queue destroy failed!"));
            err = BERR_TRACE(err);
        }

        err = BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hDSQueue, 
            pDevice->regHandle, uiOffset);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_FreeAndInvalidateTask: Display queue destroy failed!"));
            err = BERR_TRACE(err);
        }

    }
	if (BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType )
    {
        err = BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hRDQueue, 
            pDevice->regHandle, uiOffset);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_FreeAndInvalidateTask: Raw Picture delivery queue destroy failed!"));
            err = BERR_TRACE(err);
        }

		/* RDQ is now in RDB so free the fifo ids for it */
		ui32SyncFifo = pRaagaTask->taskMemGrants.sTaskQueue.sRDQueue.startIndexOfFreeFifo;
		BKNI_AcquireMutex(pDevice->fifoIdMutex[pRaagaTask->settings.dspIndex]);
		for(i=0; i < (BDSP_RAAGA_REALVIDEO_MAX_MSGS_PER_QUEUE >> 2); i++)
		{
			BDBG_MSG(("Freeing (Fifo)RDB %d", ui32SyncFifo+i-BDSP_RAAGA_FIFO_0_INDEX));
			pDevice->dspFifo[pRaagaTask->settings.dspIndex][ui32SyncFifo+i-BDSP_RAAGA_FIFO_0_INDEX] = false;
		}
		BKNI_ReleaseMutex(pDevice->fifoIdMutex[pRaagaTask->settings.dspIndex]);    

        err = BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hRRQueue, 
            pDevice->regHandle, uiOffset);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_FreeAndInvalidateTask: Raw Picture release queue destroy failed!"));
            err = BERR_TRACE(err);
        }
    }
	
	/* Destroy Async msg Queue */
    err = BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hAsyncMsgQueue, 
        pDevice->regHandle, uiOffset);
    if (BERR_SUCCESS != err)
    {
        BDBG_ERR(("BDSP_Raaga_P_FreeAndInvalidateTask: ASYNC RSP queue destroy failed!"));
        err = BERR_TRACE(err);
    }

    err = BDSP_Raaga_P_DestroyMsgQueue(pRaagaTask->hSyncMsgQueue, 
        pDevice->regHandle, uiOffset);
    if (BERR_SUCCESS != err)
    {
        BDBG_ERR(("BDSP_Raaga_P_FreeAndInvalidateTask: SYNC RSP queue destroy failed!"));
        err = BERR_TRACE(err);
    }


    ui32SyncFifo = pRaagaTask->taskMemGrants.sTaskQueue.sTaskSyncQueue.ui32FifoId;    
    ui32AsyncFifo = pRaagaTask->taskMemGrants.sTaskQueue.sTaskAsyncQueue.ui32FifoId;

    BKNI_AcquireMutex(pDevice->fifoIdMutex[pRaagaTask->settings.dspIndex]);        
    pDevice->dspFifo[pRaagaTask->settings.dspIndex][ui32SyncFifo-BDSP_RAAGA_FIFO_0_INDEX] = false;
    pDevice->dspFifo[pRaagaTask->settings.dspIndex][ui32AsyncFifo-BDSP_RAAGA_FIFO_0_INDEX] = false;    
    BKNI_ReleaseMutex(pDevice->fifoIdMutex[pRaagaTask->settings.dspIndex]);    

    pRaagaTask->taskMemGrants.sTaskQueue.sTaskSyncQueue.ui32FifoId   = BDSP_RAAGA_INVALID_INDEX;
    pRaagaTask->taskMemGrants.sTaskQueue.sTaskAsyncQueue.ui32FifoId = BDSP_RAAGA_INVALID_INDEX;     

    pDevice->taskDetails.pRaagaTask[pRaagaTask->settings.dspIndex][BDSP_RAAGA_GET_TASK_INDEX(pRaagaTask->taskId)]  = NULL;

    BKNI_AcquireMutex(pDevice->taskDetails.taskIdMutex);   
    pDevice->taskDetails.taskId[pRaagaTask->settings.dspIndex][BDSP_RAAGA_GET_TASK_INDEX(pRaagaTask->taskId)]   = true;
    BKNI_ReleaseMutex(pDevice->taskDetails.taskIdMutex);

    pRaagaTask->isStopped = true;
    pRaagaTask->lastEventType= 0xFFFF;                 
    pRaagaTask->masterTaskId= BDSP_RAAGA_P_INVALID_TASK_ID;     
    pRaagaTask->commandCounter =0;  
    pRaagaTask->paused=false;
    pRaagaTask->decLocked=false;
    pRaagaTask->hAsyncMsgQueue =NULL;
    pRaagaTask->hSyncMsgQueue =NULL;
    pRaagaTask->pFeedbackBuffer = NULL;
    pRaagaTask->schedulingMode = BDSP_TaskSchedulingMode_eStandalone;
    pRaagaTask->hPDQueue = NULL;
    pRaagaTask->hPRQueue = NULL;
    pRaagaTask->hDSQueue = NULL;

    /* Mark task as free */
    BKNI_Memset(&pRaagaTask->audioInterruptHandlers, 0, sizeof(pRaagaTask->audioInterruptHandlers));
    BKNI_Memset(&pRaagaTask->settings, 0, sizeof(pRaagaTask->settings));

    BKNI_AcquireMutex(pDevice->captureMutex);
    BLST_S_REMOVE(&pRaagaTask->pContext->allocTaskList, pRaagaTask, BDSP_RaagaTask, node);
    BKNI_ReleaseMutex(pDevice->captureMutex);
    BLST_S_INSERT_HEAD(&pRaagaTask->pContext->freeTaskList, pRaagaTask, node);
    pRaagaTask->allocated = false;

    return err;
}

static BERR_Code BDSP_Raaga_P_FindSlaveTaskSchedulingBuffer(
            const BDSP_TaskCreateSettings *pTaskSettings,
            uint32_t *pui32DramSchedulingBuffCfgAddr,
            BDSP_AF_P_BufferType    *pBufferType)            
{
    unsigned branch;
    unsigned outputType;
    unsigned output;
    
    BERR_Code   err = BERR_SUCCESS;
    BDSP_CIT_P_FwStageInfo *pStage;
    
    *pBufferType = BDSP_AF_P_BufferType_eDRAM;
    *pui32DramSchedulingBuffCfgAddr = 0;
    
    for (branch = 0; branch < pTaskSettings->numBranches; branch++)
    {
        pStage = &pTaskSettings->pBranchInfo[branch]->sFwStgInfo[(pTaskSettings->pBranchInfo[branch]->ui32NumStages -1)];
        for (outputType = 0; outputType < pStage->sStgConnectivity.ui32NumDistinctOp; outputType++)
        {
            for (output = 0; output < pStage->sStgConnectivity.ui32NumDst[outputType]; output++)
            {
                /* The scheduling buffer for a slave task will always be the intertask buffer */
                if (pStage->sStgConnectivity.sDstDetails[outputType][output].eType == BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf)
                {
                    *pui32DramSchedulingBuffCfgAddr = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sInterTaskDramBuffCfg.ui32IoBuffCfgAddr;
                    /* To break out of the outer for loop */
                    outputType = pStage->sStgConnectivity.ui32NumDistinctOp;
                    break;
                }
            }
        }
    }
    
    if (*pui32DramSchedulingBuffCfgAddr == 0)
    {
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    
    return err;
}

static BERR_Code BDSP_Raaga_P_FindSchedulingBuffer(
    const BDSP_TaskCreateSettings *pSettings,
    BDSP_CIT_P_FwStgSrcDstType    *eSchedulingBufferType,
    unsigned *pBufferId,
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER    *pBufferPtr,
    unsigned *pDelay,
    BDSP_AF_P_FmmDstFsRate *pDstRate
    )
{
    unsigned branch;
    unsigned compressed=(unsigned)-1, compressedDelay=0;
    BDSP_AF_P_FmmDstFsRate compressedRate=BDSP_AF_P_FmmDstFsRate_eBaseRate;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pBufferId);
    BDBG_ASSERT(NULL != pDelay);
    BDBG_ASSERT(NULL != pDstRate);
    *pBufferId = 0;
    *pDelay = 0;
    *pDstRate = BDSP_AF_P_FmmDstFsRate_eBaseRate;

    /* Find the first PCM buffer in the FMM, use that for scheduling */
    for ( branch = 0; branch < pSettings->numBranches; branch++ )
    {
        unsigned stage;
        BDSP_CIT_P_FwBranchInfo *pBranch=pSettings->pBranchInfo[branch];
        BDBG_ASSERT(NULL != pBranch);
        for ( stage=0; stage < pBranch->ui32NumStages; stage++ )
        {
            unsigned outputType;
            BDSP_CIT_P_FwStageInfo *pStage = &pBranch->sFwStgInfo[stage];
            for ( outputType = 0; outputType < pStage->sStgConnectivity.ui32NumDistinctOp; outputType++ )
            {
                unsigned output;
                switch ( pStage->sStgConnectivity.eDistinctOpType[outputType] )
                {
                
                case BDSP_AF_P_DistinctOpType_eStereo_PCM:
                case BDSP_AF_P_DistinctOpType_e5_1_PCM:
                case BDSP_AF_P_DistinctOpType_e7_1_PCM:
                case BDSP_AF_P_DistinctOpType_eCompressed:
                case BDSP_AF_P_DistinctOpType_eCompressed4x:
                case BDSP_AF_P_DistinctOpType_eCompressedHBR:
                    for ( output = 0; output < pStage->sStgConnectivity.ui32NumDst[outputType]; output++ )
                    {
                        if ( pStage->sStgConnectivity.sDstDetails[outputType][output].eType == BDSP_CIT_P_FwStgSrcDstType_eFMMBuf )
                        {
                            unsigned bufferid = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0];
                            *eSchedulingBufferType = BDSP_CIT_P_FwStgSrcDstType_eFMMBuf;
                            /* Found an FMM buffer.  If PCM return now, otherwise save the first compressed buffer found */
                            if ( (pStage->sStgConnectivity.eDistinctOpType[outputType] == BDSP_AF_P_DistinctOpType_eCompressed)
                                 || (pStage->sStgConnectivity.eDistinctOpType[outputType] == BDSP_AF_P_DistinctOpType_eCompressed4x)
                                 || (pStage->sStgConnectivity.eDistinctOpType[outputType] == BDSP_AF_P_DistinctOpType_eCompressedHBR))
                            {
                                if ( compressed == (unsigned)-1 )
                                {
                                    compressed = bufferid;
                                    compressedDelay = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.ui32IndepDelay;
                                    compressedRate = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.eFmmDstFsRate;

                                    /* Changes for setting up the eFmmDstFsRate compressed passthru / BTSC Encoder */
                                    if (pStage->sStgConnectivity.eDistinctOpType[outputType] == BDSP_AF_P_DistinctOpType_eCompressedHBR)
                                    {
                                        compressedRate = BDSP_AF_P_FmmDstFsRate_e16xBaseRate;
                                    }
                                    else if (pStage->sStgConnectivity.eDistinctOpType[outputType] == BDSP_AF_P_DistinctOpType_eCompressed4x)
                                    {
                                        compressedRate = BDSP_AF_P_FmmDstFsRate_e4xBaseRate;
                                    }
                                    else
                                    {
                                        compressedRate = BDSP_AF_P_FmmDstFsRate_eBaseRate;
                                    }
                                    
                                    if ((pStage->sStgConnectivity.eDistinctOpType[outputType] == BDSP_AF_P_DistinctOpType_eCompressed)
                                        && (pStage->eStageType == BDSP_CIT_P_StageType_ePostProc)
                                        && (pStage->uAlgorithm.eProcAudioAlgo == BDSP_AudioProcessing_eBtscEncoder)
                                        && (pStage->uAudioMode.eProcessingAudioMode == BDSP_DSPCHN_ProcessingMode_ePP))
                                    {
                                        compressedRate = BDSP_AF_P_FmmDstFsRate_e4xBaseRate;
                                    }
                                }
                            }
                            else
                            {
                                *pBufferId = bufferid;
                                *pDelay = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.ui32IndepDelay;
                                *pDstRate = BDSP_AF_P_FmmDstFsRate_eBaseRate;
                                return BERR_SUCCESS;
                            }
                        }
                    }
                    break;

                case BDSP_AF_P_DistinctOpType_eCdb:
                case BDSP_AF_P_DistinctOpType_eItb:

                    if (BDSP_TaskRealtimeMode_eNonRealTime == pSettings->realtimeMode)
                    {
                        for ( output = 0; output < pStage->sStgConnectivity.ui32NumDst[outputType]; output++ )
                        {
                            if ( pStage->sStgConnectivity.sDstDetails[outputType][output].eType == BDSP_CIT_P_FwStgSrcDstType_eRaveBuf )
                            {
                                *eSchedulingBufferType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
                                pBufferPtr->ui32BaseAddr = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr;   
                                pBufferPtr->ui32EndAddr = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr;   
                                pBufferPtr->ui32ReadAddr = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr;   
                                pBufferPtr->ui32WriteAddr = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr;   
                                pBufferPtr->ui32WrapAddr = pStage->sStgConnectivity.sDstDetails[outputType][output].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr;                                   
                                *pDelay = 0;                                
                                return BERR_SUCCESS;                                
                            }
                        }
                    }
                    break;  
                default:
                    break;
                }
            }
        }
    }
    /* If we reach here, return success if we found a compressed buffer */
    if ( compressed != (unsigned)-1 )
    {
        *pBufferId = compressed;
        *pDelay = compressedDelay;
        *pDstRate = compressedRate;
        return BERR_SUCCESS;
    }
    /* Nothing to schedule off */
    return BERR_TRACE(BERR_INVALID_PARAMETER);
}

BERR_Code BDSP_Raaga_P_StartTask(
    void *pTaskHandle
    )
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;    
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;    
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BDSP_Raaga_P_TaskParamInfo *pTaskParams = NULL;
    BDSP_Raaga_P_MsgType      eMsgType;
    unsigned ui32RbufOffset,uiDspOffset, uiSchedulingBufId, uiSchedulingBufDelay;
    uint32_t    ui32PhysAddr, i = 0;
    unsigned int    ui32Threshold = 0;
    unsigned int    ui32BlockTime = 0;                
    void           *pCached;
    BERR_Code   err=BERR_SUCCESS;
    BDSP_AF_P_FmmDstFsRate schedulingBufferRate=BDSP_AF_P_FmmDstFsRate_eBaseRate;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    uiDspOffset = pDevice->dspOffset[pRaagaTask->settings.dspIndex];
    BKNI_Memset(&sCommand,0,sizeof(sCommand));
    BKNI_Memset(&sRsp,0,sizeof(sRsp));    

    /* Send VOM change Command */
    BDSP_DSPCHN_P_SendVOMChangeCommand (pRaagaTask);

    BDBG_MSG(("\npRaagaContext->settings.contextType = %d",pRaagaContext->settings.contextType));
    BDBG_MSG(("\npRaagaTask->taskId = %d\n",pRaagaTask->taskId));

    /* Initialize to eDisable by default */
    sCommand.uCommand.sStartTask.eOpenGateAtStart = BDSP_AF_P_eDisable;
       
    if (pRaagaContext->settings.contextType  == BDSP_ContextType_eAudio)
    {
#if 1
        {
            BDSP_RaagaCapture *pRaagaCapture;
            int j;

            /* Print the initialized captur info structure */
            BDBG_MSG(("CAPTURE BUFFER INFORMATION (%d)", pRaagaTask->taskId));
            BDBG_MSG(("--------------------------"));
            
            BKNI_AcquireMutex(pDevice->captureMutex);
            for ( pRaagaCapture = BLST_S_FIRST(&pRaagaTask->captureList);
                pRaagaCapture != NULL;
                pRaagaCapture = BLST_S_NEXT(pRaagaCapture, node) )
            {

                BDBG_ERR(("Cap buff info for capture handle (%x)", pRaagaCapture));
                BDBG_ERR(("Enabled          = %d", pRaagaCapture->enabled));
                BDBG_ERR(("numBuffers       = %d", pRaagaCapture->numBuffers));
                BDBG_ERR(("updateRead       = %d", pRaagaCapture->updateRead));
                BDBG_ERR(("eBuffType        = %d", pRaagaCapture->eBuffType));
                
                for (j = 0; j < pRaagaCapture->numBuffers; j++)
                {
                    BDBG_ERR(("Op  ptrs : 0x%08x , 0x%08x , 0x%08x , 0x%08x , 0x%08x", 
                        pRaagaCapture->capPtrs[j].outputBufferPtr.ui32BaseAddr,
                        pRaagaCapture->capPtrs[j].outputBufferPtr.ui32ReadAddr,
                        pRaagaCapture->capPtrs[j].outputBufferPtr.ui32WriteAddr,
                        pRaagaCapture->capPtrs[j].outputBufferPtr.ui32EndAddr,
                        pRaagaCapture->capPtrs[j].outputBufferPtr.ui32WrapAddr));
                    BDBG_ERR(("Cap ptrs : 0x%08x , 0x%08x , 0x%08x , 0x%08x , 0x%08x", 
                        pRaagaCapture->capPtrs[j].captureBufferPtr.ui32BaseAddr,
                        pRaagaCapture->capPtrs[j].captureBufferPtr.ui32ReadAddr,
                        pRaagaCapture->capPtrs[j].captureBufferPtr.ui32WriteAddr,
                        pRaagaCapture->capPtrs[j].captureBufferPtr.ui32EndAddr,
                        pRaagaCapture->capPtrs[j].captureBufferPtr.ui32WrapAddr));
                    BDBG_ERR(("Cap pointer shadow read : 0x%08x", pRaagaCapture->capPtrs[j].shadowRead));
                }
            }
            BKNI_ReleaseMutex(pDevice->captureMutex);
        }
#endif

    
        /* Send Start Task Command */
        sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_START_TASK_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
        sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
        sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
        sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

        if (pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eDecode)
        {
            switch (pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eDecAudioMode)
            {
            case BDSP_DecodeMode_eDecode:
                sCommand.uCommand.sStartTask.eTaskAlgoType = BDSP_Raaga_P_AlgoType_eDecode; 
                sCommand.uCommand.sStartTask.ePPMCorrEnable = pRaagaTask->settings.ppmCorrection;                
                break;
            case BDSP_DecodeMode_ePassThru:
                /*Till FW adds proper programming */
                /*                sCommand.uCommand.sStartTask.eTaskAlgoType = BDSP_Raaga_P_AlgoType_ePassThru; */
                /*                sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = 
                                BDSP_Raaga_P_DeadLineComputeFuncType_ePassthrough;*/
                sCommand.uCommand.sStartTask.eTaskAlgoType = BDSP_Raaga_P_AlgoType_eDecode; 
                sCommand.uCommand.sStartTask.ePPMCorrEnable = BDSP_AF_P_eDisable;
                break;
            case BDSP_DecodeMode_eMax:
            default:
                BDBG_ERR(("INVALID DECODE MODE"));
                err = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto end;
            }
        }
        else if (pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_ePostProc)
        {
            sCommand.uCommand.sStartTask.eTaskAlgoType = BDSP_Raaga_P_AlgoType_eDecode; 
            sCommand.uCommand.sStartTask.ePPMCorrEnable = BDSP_AF_P_eDisable;

        }
        BDBG_MSG(("PPMCorrection > %d", sCommand.uCommand.sStartTask.ePPMCorrEnable));

        /* Enable mute frame rendering at start based on the flag openGateAtStart */
        if (pRaagaTask->settings.openGateAtStart)
        {
            sCommand.uCommand.sStartTask.eOpenGateAtStart = BDSP_AF_P_eEnable;
        }
        else
        {
            sCommand.uCommand.sStartTask.eOpenGateAtStart = BDSP_AF_P_eDisable;
        }

        /* Fill up start task parameters */
        pTaskParams = (BDSP_Raaga_P_TaskParamInfo *)(pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr);
        (void)BMEM_ConvertAddressToCached(pDevice->memHandle, pTaskParams, &pCached);
        pTaskParams = pCached;

        if (pRaagaTask->settings.realtimeMode== BDSP_TaskRealtimeMode_eNonRealTime)
        {
            sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = 
                BDSP_Raaga_P_DeadLineComputeFuncType_eNonRealtimeDecode;        
            sCommand.uCommand.sStartTask.eTaskType = BDSP_Raaga_P_TaskType_eNonRealtime;

            pTaskParams->sNonRealTimeTaskParams.ui32MaxTimeSlice = 1350;
            pTaskParams->sNonRealTimeTaskParams.ui32MinTimeThreshold = 45;                
        }
        else
        {
            sCommand.uCommand.sStartTask.eTaskType = BDSP_Raaga_P_TaskType_eRealtime;
            sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = 
                BDSP_Raaga_P_DeadLineComputeFuncType_eRealtimeDecode;            
        }
        
        if (pRaagaTask->settings.schedulingMode == BDSP_TaskSchedulingMode_eSlave)
        {
            sCommand.uCommand.sStartTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eSlave;
        }
        else
        {
            sCommand.uCommand.sStartTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eMaster;
        }

        if ((pRaagaTask->settings.schedulingMode== BDSP_TaskSchedulingMode_eSlave)&&(pRaagaTask->masterTaskId != BDSP_RAAGA_P_INVALID_TASK_ID))
        {
            sCommand.uCommand.sStartTask.ui32MasterTaskId = pRaagaTask->masterTaskId;            
        }
        else
        {
            sCommand.uCommand.sStartTask.ui32MasterTaskId = BDSP_RAAGA_P_INVALID_TASK_ID;                        
        }

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = ui32PhysAddr;

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = ui32PhysAddr;
        sCommand.uCommand.sStartTask.ui32SyncQueueId = pRaagaTask->hSyncMsgQueue->ui32FifoId;
        sCommand.uCommand.sStartTask.ui32AsyncQueueId = pRaagaTask->hAsyncMsgQueue->ui32FifoId;

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->citOutput.sStackSwapBuff.ui32DramBufferAddress),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = ui32PhysAddr;
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = pRaagaTask->citOutput.sStackSwapBuff.ui32BufferSizeInBytes;

        pTaskParams->ui32SamplingFrequency = 48000;
        BDBG_MSG(("pTaskParams->ui32SamplingFrequency > %x", pTaskParams->ui32SamplingFrequency));

        pTaskParams->ui32Threshold = pRaagaTask->ctbOutput.ui32Threshold;
        pTaskParams->ui32BlockTime =pRaagaTask->ctbOutput.ui32BlockTime; 

        /*TODO : ui32FrameSize is not required anymore by FW. remove this parameter */
        pTaskParams->ui32FrameSize = 1536;
        pTaskParams->eBufferType = BDSP_AF_P_BufferType_eFMM;


        if (pRaagaTask->schedulingMode == BDSP_TaskSchedulingMode_eSlave)
        {
            err = BDSP_Raaga_P_FindSlaveTaskSchedulingBuffer(&pRaagaTask->settings, &pTaskParams->sDspSchedulingBuffInfo.ui32DramSchedulingBuffCfgAddr, &pTaskParams->eBufferType);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("Unable to find scheduling buffer for the slave task : %d", pRaagaTask->taskId));
                err = BERR_TRACE(err);
                goto end;
            }
        }
        else
        {
            BDSP_CIT_P_FwStgSrcDstType    eSchedulingBufferType;
            BDSP_AF_P_sDRAM_CIRCULAR_BUFFER    sBufferPtr;
            BKNI_Memset(&sBufferPtr,0,sizeof(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER));

            eSchedulingBufferType = BDSP_CIT_P_FwStgSrcDstType_eInvalid;
            
            err = BDSP_Raaga_P_FindSchedulingBuffer(&pRaagaTask->settings,&eSchedulingBufferType, &uiSchedulingBufId, &sBufferPtr,&uiSchedulingBufDelay, &schedulingBufferRate);
            if ( (BERR_SUCCESS != err) || (eSchedulingBufferType == BDSP_CIT_P_FwStgSrcDstType_eInvalid) )
            {
                BDBG_ERR(("Unable to find scheduling buffer"));
                err=BERR_TRACE(err);
                goto end;
            }
            if (eSchedulingBufferType == BDSP_CIT_P_FwStgSrcDstType_eFMMBuf)
            {
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR
                /* Traditional RDB naming conventions */
                ui32RbufOffset = ((BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR - BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR) * uiSchedulingBufId) + BCHP_PHYSICAL_OFFSET;

                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32EndAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32ReadAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WriteAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WrapAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + ui32RbufOffset;
#else
                /* Newer 7429-style naming */
                ui32RbufOffset = ((BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_BASEADDR - BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_BASEADDR) * uiSchedulingBufId) + BCHP_PHYSICAL_OFFSET;

                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_BASEADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32EndAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32ReadAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WriteAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR + ui32RbufOffset;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WrapAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR + ui32RbufOffset;
#endif
            }
            else if (eSchedulingBufferType == BDSP_CIT_P_FwStgSrcDstType_eRaveBuf)
            {
                pTaskParams->eBufferType = BDSP_AF_P_BufferType_eRAVE;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr = sBufferPtr.ui32BaseAddr;                                
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32EndAddr = sBufferPtr.ui32EndAddr;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32ReadAddr = sBufferPtr.ui32ReadAddr;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WriteAddr = sBufferPtr.ui32WriteAddr;
                pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WrapAddr = sBufferPtr.ui32WrapAddr;
            }
            BDBG_MSG(("pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr > %x", pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr));            
            pTaskParams->ui32IndepDelay = uiSchedulingBufDelay;            


        }

        /* For slave task ui32IndepDelay and ui32MaxIndepDelay should be 0*/
        if (sCommand.uCommand.sStartTask.eSchedulingMode == BDSP_Raaga_P_SchedulingMode_eSlave)
        {
            pTaskParams->ui32IndepDelay = 0;    
            pTaskParams->ui32MaxIndepDelay = 0;         
        }
        else /* For Master task*/
        {
            pTaskParams->ui32MaxIndepDelay = pRaagaTask->settings.maxIndependentDelay;         
        }
        pTaskParams->eFmmDstFsRate = schedulingBufferRate;

        if ((pRaagaTask->schedulingMode== BDSP_TaskSchedulingMode_eMaster)
            ||(pRaagaTask->schedulingMode == BDSP_TaskSchedulingMode_eSlave))
        {
            BMEM_ConvertAddressToOffset(pDevice->memHandle,
                (void *)(pRaagaTask->pFeedbackBuffer),
                &(pTaskParams->ui32MasterTaskFeedbackBuffCfgAddr));        
            pTaskParams->ui32MasterTaskFeedbackBuffValid = 1;  
            BDBG_MSG(("%s: pTaskParams->ui32MasterTaskFeedbackBuffCfgAddr = 0x%x",(sCommand.uCommand.sStartTask.eSchedulingMode == BDSP_Raaga_P_SchedulingMode_eSlave)?"slave":"master", pTaskParams->ui32MasterTaskFeedbackBuffCfgAddr));                
        }
        /* Feedback buffer setting */
        else
        {
            pTaskParams->ui32MasterTaskFeedbackBuffCfgAddr = 0;
            pTaskParams->ui32MasterTaskFeedbackBuffValid = 0;
        }

        (void)BMEM_FlushCache(pDevice->memHandle, pTaskParams, sizeof(BDSP_Raaga_P_TaskParamInfo));
        sCommand.uCommand.sStartTask.ui32EventEnableMask = pRaagaTask->eventEnabledMask ;

        pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;    

        BKNI_ResetEvent(pRaagaTask->hEvent);

        BDBG_MSG(("\n"));
        BDBG_MSG(("===========Command information============="));
        BDBG_MSG(("==========================================="));        
        BDBG_MSG(("sCommand.uCommand.sStartTask.eTaskAlgoType = %d",sCommand.uCommand.sStartTask.eTaskAlgoType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eTaskType = %d",sCommand.uCommand.sStartTask.eTaskType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eSchedulingMode = %d",sCommand.uCommand.sStartTask.eSchedulingMode));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32MasterTaskId = %d",sCommand.uCommand.sStartTask.ui32MasterTaskId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = %d",sCommand.uCommand.sStartTask.eDeadlineComputationFuncType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = 0x%x",sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = 0x%x",sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32SyncQueueId = %d",sCommand.uCommand.sStartTask.ui32SyncQueueId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32AsyncQueueId = %d",sCommand.uCommand.sStartTask.ui32AsyncQueueId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = 0x%x",sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress));
        BDBG_MSG(("sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = %d",sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32EventEnableMask = %d",sCommand.uCommand.sStartTask.ui32EventEnableMask));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ePPMCorrEnable = %d\n\n",sCommand.uCommand.sStartTask.ePPMCorrEnable));        

        err = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);
        /*Accept the other Commands , After posting Start task Command */


        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK creation failed!"));
            err = BERR_TRACE(err);
            goto end;
        }

        /* Wait for Ack_Response_Received event w/ timeout */
        err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS);

        if (BERR_TIMEOUT == err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK ACK timeout! -- Interrupts to be tested"));
            err = BERR_TRACE(err);
            goto end;
        }

        /* Send command for the task , Only if the ack for the Start of the task is recieved */
        pRaagaTask->isStopped = false;            

        eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
        err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: Unable to read ACK!"));
            err = BERR_TRACE(err);
            goto end;
        }

        if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_START_TASK_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
        {

            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
            err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            goto end;
        }
    }
    else if (pRaagaContext->settings.contextType  == BDSP_ContextType_eVideo)
    {
        /* Send Start Task Command */
        sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_START_TASK_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = 0;
        sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
        sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
        sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);            

        sCommand.uCommand.sStartTask.eTaskAlgoType = BDSP_Raaga_P_AlgoType_eVideoDecode; 
        sCommand.uCommand.sStartTask.eDeadlineComputationFuncType =  BDSP_Raaga_P_DeadLineComputeFuncType_eNonRealtimeDecode;

        if (pRaagaTask->settings.schedulingMode == BDSP_TaskSchedulingMode_eSlave)
        {
            sCommand.uCommand.sStartTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eSlave;
        }
        else
        {
            sCommand.uCommand.sStartTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eMaster;
        }

        sCommand.uCommand.sStartTask.eTaskType = BDSP_Raaga_P_TaskType_eSoftRealtime;

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = ui32PhysAddr;


        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = ui32PhysAddr;
        sCommand.uCommand.sStartTask.ui32SyncQueueId = pRaagaTask->hSyncMsgQueue->ui32FifoId;
        sCommand.uCommand.sStartTask.ui32AsyncQueueId = pRaagaTask->hAsyncMsgQueue->ui32FifoId;

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->videoCitOutput.sStackSwapBuff.ui32DramBufferAddress),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = ui32PhysAddr;
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = pRaagaTask->videoCitOutput.sStackSwapBuff.ui32BufferSizeInBytes;

        /* Fill up start task parameters */
        pTaskParams = (BDSP_Raaga_P_TaskParamInfo *)(pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr);
        (void)BMEM_ConvertAddressToCached(pDevice->memHandle, pTaskParams, &pCached);
        pTaskParams = pCached;

        pTaskParams->ui32Threshold = ui32Threshold;
        pTaskParams->ui32BlockTime = ui32BlockTime; 

        pTaskParams->sNonRealTimeTaskParams.ui32MaxTimeSlice = 1350;
        pTaskParams->sNonRealTimeTaskParams.ui32MinTimeThreshold = 45;                

        /*TODO : ui32FrameSize is not required anymore by FW. remove this parameter */
        pTaskParams->ui32FrameSize = 1536;
        pTaskParams->eBufferType = BDSP_AF_P_BufferType_eFMM;

        ui32RbufOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
            BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

        ui32RbufOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
            (ui32RbufOffset * BDSP_RAAGA_DSP_P_FIFO_PDQ) + \
            + BCHP_PHYSICAL_OFFSET;

        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;;
        BDBG_MSG(("pTaskParams->sCircBuffer.ui32BaseAddr > %x", pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr));

        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32EndAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32ReadAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WriteAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WrapAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

        (void)BMEM_FlushCache(pDevice->memHandle, pTaskParams, sizeof(BDSP_Raaga_P_TaskParamInfo));
        sCommand.uCommand.sStartTask.ui32EventEnableMask = pRaagaTask->eventEnabledMask ;

        pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;    

        BKNI_ResetEvent(pRaagaTask->hEvent);

        BDBG_MSG(("\n"));
        BDBG_MSG(("===========Command information============="));
        BDBG_MSG(("==========================================="));        
        BDBG_MSG(("sCommand.uCommand.sStartTask.eTaskAlgoType = %d",sCommand.uCommand.sStartTask.eTaskAlgoType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eTaskType = %d",sCommand.uCommand.sStartTask.eTaskType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32MasterTaskId = %d",sCommand.uCommand.sStartTask.ui32MasterTaskId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = %d",sCommand.uCommand.sStartTask.eDeadlineComputationFuncType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = 0x%x",sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = 0x%x",sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32SyncQueueId = %d",sCommand.uCommand.sStartTask.ui32SyncQueueId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32AsyncQueueId = %d",sCommand.uCommand.sStartTask.ui32AsyncQueueId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = 0x%x",sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress));
        BDBG_MSG(("sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = %d",sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32EventEnableMask = %d",sCommand.uCommand.sStartTask.ui32EventEnableMask));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ePPMCorrEnable = %d\n\n",sCommand.uCommand.sStartTask.ePPMCorrEnable));        
        BDBG_MSG(("sCommand.uCommand.sStartTask.eOpenGateAtStart = %d\n\n",sCommand.uCommand.sStartTask.eOpenGateAtStart));        


        err = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);
        /*Accept the other Commands , After posting Start task Command */
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK creation failed!"));
            err = BERR_TRACE(err);
            goto end;
        }

        /* Wait for Ack_Response_Received event w/ timeout */
        err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS);
        if (BERR_TIMEOUT == err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK ACK timeout! -- Interrupts to be tested"));
            err = BERR_TRACE(err);
            goto end;
        }

        /* Send command for the task , Only if the ack for the Start of the task is recieved */
        pRaagaTask->isStopped = false;            

        eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
        err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: Unable to read ACK!"));
            err = BERR_TRACE(err);
            goto end;
        }

        if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_START_TASK_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
        {

            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
            err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            goto end;
        }
    }
	else if (BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType )
    {
        /* Send Start Task Command */
        sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_START_TASK_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = 0;
        sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
        sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
        sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);            

        sCommand.uCommand.sStartTask.eTaskAlgoType = BDSP_Raaga_P_AlgoType_eVideoEncode; 
        sCommand.uCommand.sStartTask.eDeadlineComputationFuncType =  BDSP_Raaga_P_DeadLineComputeFuncType_eNonRealtimeDecode;

        sCommand.uCommand.sStartTask.eTaskType = BDSP_Raaga_P_TaskType_eSoftRealtime;

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = ui32PhysAddr;


        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = ui32PhysAddr;
        sCommand.uCommand.sStartTask.ui32SyncQueueId = pRaagaTask->hSyncMsgQueue->ui32FifoId;
        sCommand.uCommand.sStartTask.ui32AsyncQueueId = pRaagaTask->hAsyncMsgQueue->ui32FifoId;

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->videoCitOutput.sStackSwapBuff.ui32DramBufferAddress),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = ui32PhysAddr;
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = pRaagaTask->videoCitOutput.sStackSwapBuff.ui32BufferSizeInBytes;

        /* Fill up start task parameters */
        pTaskParams = (BDSP_Raaga_P_TaskParamInfo *)(pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr);
        (void)BMEM_ConvertAddressToCached(pDevice->memHandle, pTaskParams, &pCached);
        pTaskParams = pCached;

        pTaskParams->ui32Threshold = ui32Threshold;
        pTaskParams->ui32BlockTime = ui32BlockTime; 

        pTaskParams->sNonRealTimeTaskParams.ui32MaxTimeSlice = 1350;
        pTaskParams->sNonRealTimeTaskParams.ui32MinTimeThreshold = 45;                

        /*TODO : ui32FrameSize is not required anymore by FW. remove this parameter */
        pTaskParams->ui32FrameSize = 1536;
        pTaskParams->eBufferType = BDSP_AF_P_BufferType_eRDB;

        ui32RbufOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
            BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

        ui32RbufOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
            (ui32RbufOffset * BDSP_RAAGA_DSP_P_FIFO_PDQ) + \
            + BCHP_PHYSICAL_OFFSET;

        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;;
        BDBG_MSG(("pTaskParams->sCircBuffer.ui32BaseAddr > %x", pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr));

        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32EndAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32ReadAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WriteAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WrapAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

        (void)BMEM_FlushCache(pDevice->memHandle, pTaskParams, sizeof(BDSP_Raaga_P_TaskParamInfo));
        sCommand.uCommand.sStartTask.ui32EventEnableMask = pRaagaTask->eventEnabledMask ;

        pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;    

        BKNI_ResetEvent(pRaagaTask->hEvent);

        BDBG_MSG(("\n"));
        BDBG_MSG(("===========Command information============="));
        BDBG_MSG(("==========================================="));        
        BDBG_MSG(("sCommand.uCommand.sStartTask.eTaskAlgoType = %d",sCommand.uCommand.sStartTask.eTaskAlgoType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eTaskType = %d",sCommand.uCommand.sStartTask.eTaskType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32MasterTaskId = %d",sCommand.uCommand.sStartTask.ui32MasterTaskId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = %d",sCommand.uCommand.sStartTask.eDeadlineComputationFuncType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = 0x%x",sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = 0x%x",sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32SyncQueueId = %d",sCommand.uCommand.sStartTask.ui32SyncQueueId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32AsyncQueueId = %d",sCommand.uCommand.sStartTask.ui32AsyncQueueId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = 0x%x",sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress));
        BDBG_MSG(("sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = %d",sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32EventEnableMask = %d",sCommand.uCommand.sStartTask.ui32EventEnableMask));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ePPMCorrEnable = %d\n\n",sCommand.uCommand.sStartTask.ePPMCorrEnable));        
        BDBG_MSG(("sCommand.uCommand.sStartTask.eOpenGateAtStart = %d\n\n",sCommand.uCommand.sStartTask.eOpenGateAtStart));        

        err = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);
        /*Accept the other Commands , After posting Start task Command */
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK creation failed!"));
            err = BERR_TRACE(err);
            goto end;
        }

        /* Wait for Ack_Response_Received event w/ timeout */
        err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS);
        if (BERR_TIMEOUT == err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK ACK timeout! -- Interrupts to be tested"));
            err = BERR_TRACE(err);
            goto end;
        }

        /* Send command for the task , Only if the ack for the Start of the task is recieved */
        pRaagaTask->isStopped = false;            

        eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
        err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: Unable to read ACK!"));
            err = BERR_TRACE(err);
            goto end;
        }

        if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_START_TASK_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
        {

            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
            err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            goto end;
        }
		/* Unmask any interrupt that task might be using */
		if( true == pRaagaTask->settings.extInterruptConfig.enableInterrupts)
		{
			/* for all the enabled interrupts */
			for(i = 0; i < pRaagaTask->settings.extInterruptConfig.numInterrupts; i++)
			{
				BDSP_Write32(pDevice->regHandle, \
					BCHP_RAAGA_DSP_ESR_SI_MASK_CLEAR + pDevice->dspOffset[pRaagaTask->settings.dspIndex], \
					(1 << pRaagaTask->settings.extInterruptConfig.interruptInfo[i].interruptBit));
			}
		}
    }
	else if (BDSP_ContextType_eScm == pRaagaContext->settings.contextType )
    {
        /* Send Start Task Command */
        sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_START_TASK_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = 0;
        sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
        sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
        sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);            

        sCommand.uCommand.sStartTask.eTaskAlgoType = BDSP_Raaga_P_AlgoType_eScm; 
        sCommand.uCommand.sStartTask.eDeadlineComputationFuncType =  BDSP_Raaga_P_DeadLineComputeFuncType_eScmTask;

        sCommand.uCommand.sStartTask.eTaskType = BDSP_Raaga_P_TaskType_eRealtime;

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = ui32PhysAddr;


        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = ui32PhysAddr;
        sCommand.uCommand.sStartTask.ui32SyncQueueId = pRaagaTask->hSyncMsgQueue->ui32FifoId;
        sCommand.uCommand.sStartTask.ui32AsyncQueueId = pRaagaTask->hAsyncMsgQueue->ui32FifoId;

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->scmCitOutput.sStackSwapBuff.ui32DramBufferAddress),
            &ui32PhysAddr);
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = ui32PhysAddr;
        sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = pRaagaTask->scmCitOutput.sStackSwapBuff.ui32BufferSizeInBytes;

        /* Fill up start task parameters */
        pTaskParams = (BDSP_Raaga_P_TaskParamInfo *)(pRaagaTask->taskMemGrants.sTaskInfo.ui32BaseAddr);

        pTaskParams->ui32Threshold = ui32Threshold;
        pTaskParams->ui32BlockTime = ui32BlockTime; 

        /*TODO : ui32FrameSize is not required anymore by FW. remove this parameter */
        pTaskParams->ui32FrameSize = 1536;
        pTaskParams->eBufferType = BDSP_AF_P_BufferType_eRDB;

        ui32RbufOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
            BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

        ui32RbufOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
            (ui32RbufOffset * BDSP_RAAGA_DSP_P_FIFO_PDQ) + \
            + BCHP_PHYSICAL_OFFSET;

        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;;
        BDBG_MSG(("pTaskParams->sCircBuffer.ui32BaseAddr > %x", pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32BaseAddr));

        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32EndAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32ReadAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WriteAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
        pTaskParams->sDspSchedulingBuffInfo.sRdbBasedSchedulingBuffer.ui32WrapAddr = ui32RbufOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

        sCommand.uCommand.sStartTask.ui32EventEnableMask = pRaagaTask->eventEnabledMask ;

        pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;    

        BKNI_ResetEvent(pRaagaTask->hEvent);

        BDBG_MSG(("\n"));
        BDBG_MSG(("===========Command information============="));
        BDBG_MSG(("==========================================="));        
        BDBG_MSG(("sCommand.uCommand.sStartTask.eTaskAlgoType = %d",sCommand.uCommand.sStartTask.eTaskAlgoType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eTaskType = %d",sCommand.uCommand.sStartTask.eTaskType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32MasterTaskId = %d",sCommand.uCommand.sStartTask.ui32MasterTaskId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.eDeadlineComputationFuncType = %d",sCommand.uCommand.sStartTask.eDeadlineComputationFuncType));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr = 0x%x",sCommand.uCommand.sStartTask.ui32DramDeadlineConfigStructAddr));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr = 0x%x",sCommand.uCommand.sStartTask.ui32DramTaskConfigAddr));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32SyncQueueId = %d",sCommand.uCommand.sStartTask.ui32SyncQueueId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32AsyncQueueId = %d",sCommand.uCommand.sStartTask.ui32AsyncQueueId));
        BDBG_MSG(("sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress = 0x%x",sCommand.uCommand.sStartTask.sDramStackBuffer.ui32DramBufferAddress));
        BDBG_MSG(("sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes = %d",sCommand.uCommand.sStartTask.sDramStackBuffer.ui32BufferSizeInBytes));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ui32EventEnableMask = %d",sCommand.uCommand.sStartTask.ui32EventEnableMask));
        BDBG_MSG(("sCommand.uCommand.sStartTask.ePPMCorrEnable = %d\n\n",sCommand.uCommand.sStartTask.ePPMCorrEnable));        


        err = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);
        /*Accept the other Commands , After posting Start task Command */
#if 1      
  if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK creation failed!"));
            err = BERR_TRACE(err);
            goto end;
        }

        /* Wait for Ack_Response_Received event w/ timeout */
        err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS);
        if (BERR_TIMEOUT == err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK ACK timeout! -- Interrupts to be tested"));
            err = BERR_TRACE(err);
            goto end;
        }

        /* Send command for the task , Only if the ack for the Start of the task is recieved */
        pRaagaTask->isStopped = false;            

        eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
        err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_StartTask: Unable to read ACK!"));
            err = BERR_TRACE(err);
            goto end;
        }
        if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_START_TASK_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
        {

            BDBG_ERR(("BDSP_Raaga_P_StartTask: START_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
            err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            goto end;
        }
#endif
    }
    else if (pRaagaContext->settings.contextType  == BDSP_ContextType_eGraphics)
    {

    }
    else if (pRaagaContext->settings.contextType  == BDSP_ContextType_eSecurity)
    {

    }
    end:

    return err;

}

BERR_Code BDSP_Raaga_P_StopTask(
    void *pTaskHandle
    )
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;    
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;    
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BDSP_Raaga_P_MsgType      eMsgType;
    unsigned uiDspOffset, i = 0;
    BERR_Code   err=BERR_SUCCESS;


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    BKNI_Memset(&sCommand,0,sizeof(sCommand));
    BKNI_Memset(&sRsp,0,sizeof(sRsp));    
    uiDspOffset = pDevice->dspOffset[pRaagaTask->settings.dspIndex];

    if (pRaagaContext->settings.contextType  == BDSP_ContextType_eAudio)
    {
        BDSP_RaagaCapture *pRaagaCapture;
        /* Disable all captures */
        for ( pRaagaCapture = BLST_S_FIRST(&pRaagaTask->captureList);
            pRaagaCapture != NULL;
            pRaagaCapture = BLST_S_NEXT(pRaagaCapture, node) )
        {
            pRaagaCapture->enabled = false;
        }
        
        if (pRaagaContext->contextWatchdogFlag== false)
        {

            /* Send stop Task Command */
            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_STOP_TASK_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    


            if (pRaagaTask->masterTaskId != BDSP_RAAGA_P_INVALID_TASK_ID)
            {
                sCommand.uCommand.sStopTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eSlave;
                sCommand.uCommand.sStopTask.ui32MasterTaskId = pRaagaTask->masterTaskId;            
            }
            else
            {
                sCommand.uCommand.sStopTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eMaster;
                sCommand.uCommand.sStopTask.ui32MasterTaskId = BDSP_RAAGA_P_INVALID_TASK_ID;                        
            }

            if (pRaagaTask->settings.realtimeMode == BDSP_TaskRealtimeMode_eNonRealTime)
            {
                sCommand.uCommand.sStopTask.eTaskType = BDSP_Raaga_P_TaskType_eNonRealtime;
            }
            else
            {
                sCommand.uCommand.sStopTask.eTaskType = BDSP_Raaga_P_TaskType_eRealtime;
            }

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;    

            BKNI_ResetEvent(pRaagaTask->hEvent);

            BKNI_EnterCriticalSection();  
            err = BDSP_Raaga_P_SendCommand_isr(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);
            /*Accept the other Commands , After posting Start task Command */
            pRaagaTask->isStopped = true;            
            BKNI_LeaveCriticalSection();  


            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK creation failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK ACK timeout! -- Interrupts to be tested"));
                err = BERR_TRACE(err);
                goto end;
            }

            /* Send command for the task , Only if the ack for the Start of the task is recieved */

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_StopTask: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_STOP_TASK_RESPONSE_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {

                BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                    sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;
            }
        }
        else
        {
            pRaagaTask->isStopped = true;                        
        }
    }
    else if (pRaagaContext->settings.contextType  == BDSP_ContextType_eVideo)
    {
        if (pRaagaContext->contextWatchdogFlag == false)
        {

            /* Send stop Task Command */
            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_STOP_TASK_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    


            if (pRaagaTask->settings.realtimeMode == BDSP_TaskRealtimeMode_eNonRealTime)
            {
                sCommand.uCommand.sStopTask.eTaskType = BDSP_Raaga_P_TaskType_eNonRealtime;
            }
            else
            {
                sCommand.uCommand.sStopTask.eTaskType = BDSP_Raaga_P_TaskType_eRealtime;
            }
            
            if (pRaagaTask->masterTaskId != BDSP_RAAGA_P_INVALID_TASK_ID)
            {
                sCommand.uCommand.sStopTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eSlave;
                sCommand.uCommand.sStopTask.ui32MasterTaskId = pRaagaTask->masterTaskId;            
            }
            else
            {
                sCommand.uCommand.sStopTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eMaster;
                sCommand.uCommand.sStopTask.ui32MasterTaskId = BDSP_RAAGA_P_INVALID_TASK_ID;                        
            }

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;    

            BKNI_ResetEvent(pRaagaTask->hEvent);

            BKNI_EnterCriticalSection();  
            err = BDSP_Raaga_P_SendCommand_isr(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);
            /*Accept the other Commands , After posting Start task Command */
            pRaagaTask->isStopped = true;            
            BKNI_LeaveCriticalSection();  


            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK creation failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK ACK timeout! -- Interrupts to be tested"));
                err = BERR_TRACE(err);
                goto end;
            }

            /* Send command for the task , Only if the ack for the Start of the task is recieved */

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_StopTask: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_STOP_TASK_RESPONSE_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {

                BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                    sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;
            }
        }
        else
        {
            pRaagaTask->isStopped = true;                        
        }
    }
	else if (pRaagaContext->settings.contextType  == BDSP_ContextType_eVideoEncode)
	{
		if (pRaagaContext->contextWatchdogFlag == false)
		{

			/* Mask any interrupt that task might be using */
			if( true == pRaagaTask->settings.extInterruptConfig.enableInterrupts)
			{
				/* for all the enabled interrupts */
				for(i = 0; i < pRaagaTask->settings.extInterruptConfig.numInterrupts; i++)
				{
					BDSP_Write32(pDevice->regHandle, \
						BCHP_RAAGA_DSP_ESR_SI_MASK_SET + pDevice->dspOffset[pRaagaTask->settings.dspIndex], \
						(1 << pRaagaTask->settings.extInterruptConfig.interruptInfo[i].interruptBit));
				}
			}
			
			/* Send stop Task Command */
			sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_STOP_TASK_COMMAND_ID;
			sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
			sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
			sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
			sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    


			if (pRaagaTask->settings.realtimeMode == BDSP_TaskRealtimeMode_eNonRealTime)
			{
				sCommand.uCommand.sStopTask.eTaskType = BDSP_Raaga_P_TaskType_eNonRealtime;
			}
			else
			{
				sCommand.uCommand.sStopTask.eTaskType = BDSP_Raaga_P_TaskType_eRealtime;
			}
			
			if (pRaagaTask->masterTaskId != BDSP_RAAGA_P_INVALID_TASK_ID)
			{
				sCommand.uCommand.sStopTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eSlave;
				sCommand.uCommand.sStopTask.ui32MasterTaskId = pRaagaTask->masterTaskId;			
			}
			else
			{
				sCommand.uCommand.sStopTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eMaster;
				sCommand.uCommand.sStopTask.ui32MasterTaskId = BDSP_RAAGA_P_INVALID_TASK_ID;						
			}

			pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;	  

			BKNI_ResetEvent(pRaagaTask->hEvent);

			BKNI_EnterCriticalSection();  
			err = BDSP_Raaga_P_SendCommand_isr(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);
			/*Accept the other Commands , After posting Start task Command */
			pRaagaTask->isStopped = true;			 
			BKNI_LeaveCriticalSection();  


			if (BERR_SUCCESS != err)
			{
				BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK creation failed!"));
				err = BERR_TRACE(err);
				goto end;
			}
			/* Wait for Ack_Response_Received event w/ timeout */
			err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS);
			if (BERR_TIMEOUT == err)
			{
				BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK ACK timeout! -- Interrupts to be tested"));
				err = BERR_TRACE(err);
				goto end;
			}

			/* Send command for the task , Only if the ack for the Start of the task is recieved */

			eMsgType = BDSP_Raaga_P_MsgType_eSyn;	 
			err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

			if (BERR_SUCCESS != err)
			{
				BDBG_ERR(("BDSP_Raaga_P_StopTask: Unable to read ACK!"));
				err = BERR_TRACE(err);
				goto end;
			}

			if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
				(sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_STOP_TASK_RESPONSE_ID)||
				(sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
			{

				BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
					sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
				err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
				goto end;
			}
		}
		else
		{
			pRaagaTask->isStopped = true;						 
		}
	}
    else if (pRaagaContext->settings.contextType  == BDSP_ContextType_eGraphics)
    {

    }
    else if (pRaagaContext->settings.contextType  == BDSP_ContextType_eSecurity)
    {

    }
	else if (pRaagaContext->settings.contextType  == BDSP_ContextType_eScm)
	{
		if (pRaagaContext->contextWatchdogFlag == false)
		{

			/* Send stop Task Command */
			sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_STOP_TASK_COMMAND_ID;
			sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
			sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
			sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
			sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    


			if (pRaagaTask->settings.realtimeMode == BDSP_TaskRealtimeMode_eNonRealTime)
			{
				sCommand.uCommand.sStopTask.eTaskType = BDSP_Raaga_P_TaskType_eNonRealtime;
			}
			else
			{
				sCommand.uCommand.sStopTask.eTaskType = BDSP_Raaga_P_TaskType_eRealtime;
			}
			
			if (pRaagaTask->masterTaskId != BDSP_RAAGA_P_INVALID_TASK_ID)
			{
				sCommand.uCommand.sStopTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eSlave;
				sCommand.uCommand.sStopTask.ui32MasterTaskId = pRaagaTask->masterTaskId;			
			}
			else
			{
				sCommand.uCommand.sStopTask.eSchedulingMode = BDSP_Raaga_P_SchedulingMode_eMaster;
				sCommand.uCommand.sStopTask.ui32MasterTaskId = BDSP_RAAGA_P_INVALID_TASK_ID;						
			}

			pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;	  

			BKNI_ResetEvent(pRaagaTask->hEvent);

			BKNI_EnterCriticalSection();  
			err = BDSP_Raaga_P_SendCommand_isr(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);
			/*Accept the other Commands , After posting Start task Command */
			pRaagaTask->isStopped = true;			 
			BKNI_LeaveCriticalSection();  


			if (BERR_SUCCESS != err)
			{
				BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK creation failed!"));
				err = BERR_TRACE(err);
				goto end;
			}
			/* Wait for Ack_Response_Received event w/ timeout */
			err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_START_STOP_EVENT_TIMEOUT_IN_MS);
			if (BERR_TIMEOUT == err)
			{
				BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK ACK timeout! -- Interrupts to be tested"));
				err = BERR_TRACE(err);
				goto end;
			}

			/* Send command for the task , Only if the ack for the Start of the task is recieved */

			eMsgType = BDSP_Raaga_P_MsgType_eSyn;	 
			err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

			if (BERR_SUCCESS != err)
			{
				BDBG_ERR(("BDSP_Raaga_P_StopTask: Unable to read ACK!"));
				err = BERR_TRACE(err);
				goto end;
			}

			if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
				(sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_STOP_TASK_RESPONSE_ID)||
				(sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
			{

				BDBG_ERR(("BDSP_Raaga_P_StopTask: STOP_TASK ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
					sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
				err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
				goto end;
			}
		}
		else
		{
			pRaagaTask->isStopped = true;						 
		}
	}
    pRaagaTask->commandCounter     = 0;
    end:

    return err;

}

BERR_Code BDSP_Raaga_P_CreateTask(
    void *pContextHandle, 
    const BDSP_TaskCreateSettings *pSettings, 
    BDSP_TaskHandle *pTask
    )
{
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;
    BDSP_RaagaTask *pRaagaTask;    
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BERR_Code   err=BERR_SUCCESS;
    BDSP_CIT_P_InputInfo        *pCitInput=NULL;
    BDSP_VF_P_sVDecodeBuffCfg   *psVDecodeBuffCfg=NULL;     
	BDSP_VF_P_sVEncodeConfig	*psVEncoderBufCfg=NULL;
    uint32_t ui32NumBranchesValid = 0, ui32NumStagesValid = 0, i=0;  
    unsigned int uiBranchId = 0, uiStageId = 0, uiConfigBufAddr, uiConfigBufSize, uiStartNodeIndex,ui32StatusBufAdr;
    BDSP_CIT_P_OpStgInfo *psOpStgInfo = NULL;
    bool bPpConfigDone;
    void *pCached;
    void *pSettingsBuffer = NULL;
    size_t settingsBufferSize=0;
    BDSP_Raaga_Audio_FrameSyncTsmConfigParams sTsmConfig;
	BDSP_VideoEncodeTaskDatasyncSettings	sVideoFrameSyncConfig;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);

    *pTask = NULL;

    /* Find a free task */
    pRaagaTask = BLST_S_FIRST(&pRaagaContext->freeTaskList);
    if ( NULL == pRaagaTask )
    {
        BDBG_ERR(("No more tasks are available for this context.  Please increase the value of BDSP_ContextCreateSettings.maxTasks (currently %u).",
            pRaagaContext->settings.maxTasks));
        err = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_getfree_task;
    }

    err =BDSP_Raaga_P_AllocateAndInitTask(pContextHandle,(void *)pRaagaTask,pSettings);
    if ( BERR_SUCCESS!=err )
    {
        err = BERR_TRACE(err);    
        goto err_init_task;
    }

    err = BDSP_Raaga_P_InterruptInstall((void *)pRaagaTask);
    if ( BERR_SUCCESS!=err )
    {
        err = BERR_TRACE(err);    
        goto err_interrupt_install;
    }

    if (false == pDevice->settings.preloadImages)
    {
        /* Download the firmware binaries required by the complete network */
        err = BDSP_Raaga_P_DownloadStartTimeFWExec((void*)pContextHandle, pSettings);
        if ( BERR_SUCCESS !=err )
        {
            err = BERR_TRACE(BDSP_ERR_DOWNLOAD_FAILED);        
            goto err_download_fw;
        }
    }

    pCitInput = (BDSP_CIT_P_InputInfo *)BKNI_Malloc(sizeof(BDSP_CIT_P_InputInfo));
    if (NULL==pCitInput)
    {
        BDBG_ERR(("Out of System memory"));
        err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc_citinput;        
    }
    BKNI_Memset( pCitInput, 0, sizeof(BDSP_CIT_P_InputInfo) );    

    if (pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
    {
        err = BDSP_Raaga_P_GenerateCITInput((void *)pRaagaTask,pSettings,pCitInput);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Generate CIT input %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;        
        }
		/* Pass the memory handle to cit for its purpose. SW7346-598 */
		pCitInput->memInfo.psCitDataStructure = pDevice->sCitBuffers.psCitDataStructure;
		for(i=0; i < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
		{
			pCitInput->memInfo.psFwBrnachInfo[i] = pDevice->sCitBuffers.psCitFwBranchInfo[i];
		}
		pCitInput->memInfo.psCitPortInfo	= pDevice->sCitBuffers.psCitPortCfg;
		pCitInput->memInfo.psCitTaskIsInfo	= pDevice->sCitBuffers.psCitTaskIsBuffCfg;

        err =BDSP_P_GenCit(pDevice->memHandle, pDevice->regHandle, pCitInput, &(pRaagaTask->citOutput));
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Cit module %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;
        }

        BDSP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset(
            (pSettings->audioTaskDelayMode == BDSP_AudioTaskDelayMode_eDefault)?BDSP_CIT_P_LowDelayMode_eDisabled:BDSP_CIT_P_LowDelayMode_eEnabled, 
            pCitInput, &(pRaagaTask->ctbOutput));   

        BDSP_P_AnalyseCit(pDevice->memHandle, &(pRaagaTask->citOutput.sCit));

        /* Download CIT structure into DRAM */
        err = BDSP_Raaga_P_CopyDataToDram(
            pDevice->memHandle, 
            (uint32_t *)(&(pRaagaTask->citOutput.sCit)),      
            pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr,
            pRaagaTask->taskMemGrants.sCitStruct.ui32Size
            );

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in Copying data to DRAM"));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }

        /* Initialize interframe buffers for all the nodes */

        err = BDSP_DSP_P_InitInterframeBuffer((void *)pRaagaTask);  
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in initializing Interframe buffers for Task id %d",pRaagaTask->taskId));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }


        /* Initialize configuration parameters for all the nodes */

        for (uiBranchId = 0; uiBranchId < pRaagaTask->citOutput.ui32NumBranches ; uiBranchId++)
        {
            /* Get the configuration buffer address for this stage */
            for (uiStageId = 0; uiStageId < pRaagaTask->citOutput.sCitBranchInfo[uiBranchId].ui32NumStages; uiStageId++)
            {
                bPpConfigDone = false;
                psOpStgInfo = &(pRaagaTask->citOutput.sCitBranchInfo[uiBranchId].sCitStgInfo[uiStageId]);
                uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
                uiConfigBufAddr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufSize;

                /* Check if this is a decode stage */
                if (BDSP_CIT_P_StageType_eDecode== psOpStgInfo->eStageType    /*psAudProcNetwork->sAudProcessingStage[uiBranchId][uiStageId].bDecoderStage*/)
                {
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)uiConfigBufAddr) ||
                        (0 == uiConfigBufSize)) )
                    {
                        if (psOpStgInfo->uAudioMode.eDecAudioMode== BDSP_DecodeMode_ePassThru)
                        {
                            pSettingsBuffer = (void *)&(BDSP_sDefaultPassthruSettings);
                            settingsBufferSize = sizeof(BDSP_Raaga_Audio_PassthruConfigParams);
                        }
                        else if (psOpStgInfo->uAudioMode.eDecAudioMode == BDSP_DecodeMode_eDecode)
                        {
                            pSettingsBuffer = (void *)BDSP_sAudioCodecDetails[psOpStgInfo->uAlgorithm.eDecAudioAlgo]->pDefaultUserConfig;
                            settingsBufferSize = BDSP_sAudioCodecDetails[psOpStgInfo->uAlgorithm.eDecAudioAlgo]->userConfigSize;                                   
                        }
                        err = BDSP_Raaga_P_SetAudioDecodeStageSettings(
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->uAlgorithm.eDecAudioAlgo,
                            psOpStgInfo->uAudioMode.eDecAudioMode,
                            uiConfigBufAddr, uiConfigBufSize,pSettingsBuffer,settingsBufferSize);
                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }

                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr) ||
                        (0 ==  psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize)) )
                    {
                        BDBG_MSG (("Set Frame Sync Config Params"));

                        sTsmConfig = (BDSP_sDefaultFrameSyncTsmSettings);

                        sTsmConfig.sTsmConfigParams.ui32AudioOffset = pRaagaTask->ctbOutput.ui32AudOffset*45;

                        err = BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams (
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr, 
                            psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize,
                            &sTsmConfig);                    
                            
                        /* Write the settings into the host buffer as well - 
                                                                So that it can be used instead of the DSP's buffer when GetTsmSettings 
                                                                is called, to avoid any race conditions */
                        err = BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams (
                            pRaagaTask->pContext->pDevice->memHandle,
                            pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr,
                            pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size,
                            &sTsmConfig);

                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }
                }
                else if ((BDSP_CIT_P_StageType_ePostProc == psOpStgInfo->eStageType))
                {
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)uiConfigBufAddr) ||
                        (0 == uiConfigBufSize)) )
                    {
                        pSettingsBuffer = (void *)BDSP_sAudioProcessingDetails[psOpStgInfo->uAlgorithm.eProcAudioAlgo]->pDefaultUserConfig;
                        settingsBufferSize = BDSP_sAudioProcessingDetails[psOpStgInfo->uAlgorithm.eProcAudioAlgo]->userConfigSize;

                        err = BDSP_Raaga_P_SetAudioProcessingStageSettings(
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->uAlgorithm.eProcAudioAlgo,
                            psOpStgInfo->uAudioMode.eProcessingAudioMode,
                            uiConfigBufAddr, uiConfigBufSize,pSettingsBuffer,settingsBufferSize);
                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }
                }
                else if ((BDSP_CIT_P_StageType_eEncode== psOpStgInfo->eStageType))
                {
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)uiConfigBufAddr) ||
                        (0 == uiConfigBufSize)) )
                    {
                        pSettingsBuffer = (void *)BDSP_sAudioEncodeDetails[psOpStgInfo->uAlgorithm.eEncAudioAlgo]->pDefaultUserConfig;
                        settingsBufferSize = BDSP_sAudioEncodeDetails[psOpStgInfo->uAlgorithm.eEncAudioAlgo]->userConfigSize;

                        err = BDSP_Raaga_P_SetAudioEncodeStageSettings(
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->uAlgorithm.eEncAudioAlgo,
                            psOpStgInfo->uAudioMode.eEncAudioMode,
                            uiConfigBufAddr, uiConfigBufSize,pSettingsBuffer,settingsBufferSize);
                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }
                }


                /* Status Buffer */
                ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufAdr; 
                if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32StatusBufAdr)) &&
                    (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize))
                {
                    (void)BMEM_ConvertAddressToCached(pDevice->memHandle, (void *)ui32StatusBufAdr, &pCached);
                    BKNI_Memset(pCached,0xFF,psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize);
                    (void)BMEM_FlushCache(pDevice->memHandle, pCached, psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize);
                }

                if (BDSP_CIT_P_StageType_eDecode== psOpStgInfo->eStageType)
                {
                    {
                        /* Initialize TSM status Buffer */
                        ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufAdr; 
                        if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32StatusBufAdr)) &&
                            (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize))
                        {
                            (void)BMEM_ConvertAddressToCached(pDevice->memHandle, (void *)ui32StatusBufAdr, &pCached);
                            BKNI_Memset(pCached,0XFF,psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufSize);
                            (void)BMEM_FlushCache(pDevice->memHandle, pCached, psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufSize);
                    }
                }
            }         
        }
        }

        /* Enable required asynchronous event notification */        
        pRaagaTask->eventEnabledMask |= BDSP_RAAGA_DEFAULT_EVENTS_ENABLE_MASK;
#if 0                                                                  
        pRaagaTask->eventEnabledMask |= (
            BDSP_FWIF_P_EventIdMask_eSampleRateChange   
            |BDSP_FWIF_P_EventIdMask_eFirstPTS_Received 
            |BDSP_FWIF_P_EventIdMask_eStreamInfoAvail
            |BDSP_FWIF_P_EventIdMask_eUnlicensedAlgo                                     
            |BDSP_FWIF_P_EventIdMask_eTSM_Lock 
            |BDSP_FWIF_P_EventIdMask_eFrameSyncLock 
            |BDSP_FWIF_P_EventIdMask_eFrameSyncLockLost
            |BDSP_FWIF_P_EventIdMask_eStartOnPTS
            |BDSP_FWIF_P_EventIdMask_eStopOnPTS
            |BDSP_FWIF_P_EventIdMask_eAstmTsmPass
            |BDSP_FWIF_P_EventIdMask_eCdbItbOverflow
            |BDSP_FWIF_P_EventIdMask_eCdbItbUnderflow
            |BDSP_FWIF_P_EventIdMask_eTsmFail                                 
            );  
#endif

#if 0                                                                  
        if (pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eDecAudioMode == BDSP_DecodeMode_eDecode)
        {
            pRaagaTask->eventEnabledMask  |= BDSP_FWIF_P_EventIdMask_eRampEnable;
        }
#endif

#if 0
#if BCHP_VCXO_MISC_AUDIO_MODE_CTRL
        unsigned regVal;
        /*
        Due to the default value of AUDIO PLL being set to select XTAL - the
        audio was not coming out on I2S. Ideally this should be part of Firmware
        along with PLL programming. JIRA FW35230-34 has been raised and assigned 
        to the Firmware team. This hardcoding should be removed once the Firmware 
        changes are done.
        */        
        regVal = BDSP_Read32 (pDevice->regHandle, BCHP_VCXO_MISC_AUDIO_MODE_CTRL);
        regVal &= ~ (BCHP_MASK (VCXO_MISC_AUDIO_MODE_CTRL, AUDIO0_REFCLK_SEL));
        regVal |= (BCHP_FIELD_ENUM (VCXO_MISC_AUDIO_MODE_CTRL,  AUDIO0_REFCLK_SEL, VCXO0_SEL));
        BDSP_Write32 (pDevice->regHandle, BCHP_VCXO_MISC_AUDIO_MODE_CTRL, regVal);
#endif
#endif
       

        /* Success */
        *pTask = &pRaagaTask->task;

        goto create_task_success;
    }
    else if (pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        psVDecodeBuffCfg = (BDSP_VF_P_sVDecodeBuffCfg *)BKNI_Malloc(sizeof(BDSP_VF_P_sVDecodeBuffCfg));
        if (NULL==psVDecodeBuffCfg)
        {
            BDBG_ERR(("Out of System memory"));
            err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto err_malloc_videobuffinput;
        }
        BKNI_Memset( psVDecodeBuffCfg, 0, sizeof(BDSP_VF_P_sVDecodeBuffCfg) );    


        err = BDSP_Raaga_P_GenerateVideoCITInput((void *)pRaagaTask,pSettings,pCitInput,psVDecodeBuffCfg);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Generate CIT input %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;        
        }

		/* Pass the memory handle to cit for its purpose. SW7346-598 */
		pCitInput->memInfo.psCitDataStructure = pDevice->sCitBuffers.psCitDataStructure;
		for(i=0; i < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
		{
			pCitInput->memInfo.psFwBrnachInfo[i] = pDevice->sCitBuffers.psCitFwBranchInfo[i];
		}
		pCitInput->memInfo.psCitPortInfo	= pDevice->sCitBuffers.psCitPortCfg;
		pCitInput->memInfo.psCitTaskIsInfo	= pDevice->sCitBuffers.psCitTaskIsBuffCfg;

        err =BDSP_P_VideoGenCit(pDevice->memHandle, pDevice->regHandle, pCitInput, (void *)psVDecodeBuffCfg, &(pRaagaTask->videoCitOutput), BDSP_CIT_P_StageType_eVideoDecode);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Cit module %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;
        }

        BDBG_MSG(("Video GenCit Done"));

        /*	Analyze the CIT generated : First level Information */
        BDSP_P_AnalyseVideoCit(pDevice->memHandle, &(pRaagaTask->videoCitOutput.uVideoCit), BDSP_CIT_P_StageType_eVideoDecode);
        BDBG_MSG(("Analyse cit DONE"));

        /* Download CIT structure into DRAM */
        err = BDSP_Raaga_P_CopyDataToDram(
            pDevice->memHandle, 
            (uint32_t *)(&(pRaagaTask->videoCitOutput.uVideoCit.sVideoDecTaskConfig)),      
            pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr,
            pRaagaTask->taskMemGrants.sCitStruct.ui32Size
            );
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in Copying data to DRAM"));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }

        /* Initialize interframe buffers for all the nodes */
        err = BDSP_VID_P_InitInterframeBuffer((void *)pRaagaTask);  
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in initializing Interframe buffers for Task id %d",pRaagaTask->taskId));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }

        /* Initialize configuration parameters for all the nodes */
        ui32NumBranchesValid = pRaagaTask->videoCitOutput.ui32NumBranches;
        BDBG_MSG(("ui32NumBranchesValid = %x\n",ui32NumBranchesValid));
        for (uiBranchId = 0; uiBranchId < ui32NumBranchesValid ; uiBranchId++)
        {
            /* Get the configuration buffer address for this stage */
            ui32NumStagesValid = pRaagaTask->videoCitOutput.sCitBranchInfo[uiBranchId].ui32NumStages;
            BDBG_MSG(("ui32NumStagesValid = %x\n",ui32NumStagesValid));
            for (uiStageId = 0; uiStageId < ui32NumStagesValid; uiStageId++)
            {
                bPpConfigDone = false;
                psOpStgInfo = &(pRaagaTask->videoCitOutput.sCitBranchInfo[uiBranchId].sCitStgInfo[uiStageId]);
                uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
                uiConfigBufAddr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufSize;

                /* Check if this is a decode stage */
                if (BDSP_CIT_P_StageType_eVideoDecode== psOpStgInfo->eStageType)
                {
                    BDBG_MSG (("BDSP_Raaga_P_CreateTask : BuffAdr[%d] BufSize[%d]",
                        psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr,
                        psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize));
                    /* Decode stage. Program the decoder configuration parameters stored in
                    channel handle. */                        
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)uiConfigBufAddr) ||
                        (0 == uiConfigBufSize)) )
                    {
                        pSettingsBuffer = (void *)BDSP_sVideoCodecDetails[psOpStgInfo->uAlgorithm.eVideoAlgo]->pDefaultUserConfig;
                        settingsBufferSize = BDSP_sVideoCodecDetails[psOpStgInfo->uAlgorithm.eVideoAlgo]->userConfigSize;                                   

                        err = BDSP_Raaga_P_SetVideoDecodeStageSettings(
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->uAlgorithm.eDecAudioAlgo,
                            psOpStgInfo->uAudioMode.eDecAudioMode,                        
                            uiConfigBufAddr, uiConfigBufSize,pSettingsBuffer,settingsBufferSize);
                        if ( BERR_SUCCESS!=err )
                        {
                            BDBG_MSG(("Error in BDSP_Raaga_P_SetVideoDecodeStageSettings\n"));
                            goto err_gen_citinput;
                        }
                    }

                    BDBG_MSG (("BDSP_Raaga_P_CreateTask : BuffAdr[%d] BufSize[%d]",
                        psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr,
                        psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize));   

                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr) ||
                        (0 ==  psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize)))
                    {
                        BDBG_MSG (("Set Frame Sync Config Params"));

                        sTsmConfig = (BDSP_sDefaultFrameSyncTsmSettings);

                        sTsmConfig.sTsmConfigParams.ui32AudioOffset = pRaagaTask->ctbOutput.ui32AudOffset*45;

                        err = BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams (
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr, 
                            psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize,
                            &sTsmConfig);                    
                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }
                }
                /* Status Buffer */
                ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufAdr; 
                if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32StatusBufAdr)) &&
                    (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize))
                {
                    (void)BMEM_ConvertAddressToCached(pDevice->memHandle, (void *)ui32StatusBufAdr, &pCached);
                    BKNI_Memset(pCached,0xFF,psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize);
                    (void)BMEM_FlushCache(pDevice->memHandle, pCached, psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize);
                }

                if (BDSP_CIT_P_StageType_eVideoDecode== psOpStgInfo->eStageType)
                {
                    /* Initialize TSM status Buffer */
                    ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufAdr; 
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32StatusBufAdr)) &&
                        (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize))
                    {
                        (void)BMEM_ConvertAddressToCached(pDevice->memHandle, (void *)ui32StatusBufAdr, &pCached);
                        BKNI_Memset(pCached,0XFF,psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufSize);
                        (void)BMEM_FlushCache(pDevice->memHandle, pCached, psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufSize);
                    }
                }

            }        
        }       

        /* Enable required asynchronous event notification */        
        pRaagaTask->eventEnabledMask |= BDSP_RAAGA_DEFAULT_EVENTS_ENABLE_MASK;

        /* Success */
        *pTask = &pRaagaTask->task;

        goto create_task_success;
    }
	else if (pRaagaContext->settings.contextType == BDSP_ContextType_eScm)
    {
        err = BDSP_Raaga_P_GenerateScmCITInput((void *)pRaagaTask,pSettings,pCitInput);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Generate CIT input %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;        
        }

		/* Pass the memory handle to cit for its purpose. SW7346-598 */
		pCitInput->memInfo.psCitDataStructure = pDevice->sCitBuffers.psCitDataStructure;
		for(i=0; i < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
		{
			pCitInput->memInfo.psFwBrnachInfo[i] = pDevice->sCitBuffers.psCitFwBranchInfo[i];
		}
		pCitInput->memInfo.psCitPortInfo	= pDevice->sCitBuffers.psCitPortCfg;
		pCitInput->memInfo.psCitTaskIsInfo	= pDevice->sCitBuffers.psCitTaskIsBuffCfg;


        err =BDSP_P_GenScmCit(pDevice->memHandle, pDevice->regHandle, pCitInput, &(pRaagaTask->scmCitOutput));
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Cit module %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;
        }

        BDSP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset(
            (pSettings->audioTaskDelayMode == BDSP_AudioTaskDelayMode_eDefault)?BDSP_CIT_P_LowDelayMode_eDisabled:BDSP_CIT_P_LowDelayMode_eEnabled, 
            pCitInput, &(pRaagaTask->ctbOutput));   

        BDSP_P_AnalyseScmCit(pDevice->memHandle, &(pRaagaTask->scmCitOutput.sScmCit));

        /* Download CIT structure into DRAM */
        err = BDSP_Raaga_P_CopyDataToDram(
            pDevice->memHandle, 
            (uint32_t *)(&(pRaagaTask->scmCitOutput.sScmCit)),      
            pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr,
            pRaagaTask->taskMemGrants.sCitStruct.ui32Size
            );

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in Copying data to DRAM"));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }

        /* Initialize interframe buffers for all the nodes */

        err = BDSP_DSP_P_InitInterframeBuffer((void *)pRaagaTask);  
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in initializing Interframe buffers for Task id %d",pRaagaTask->taskId));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }       

        /* Enable required asynchronous event notification */        
        pRaagaTask->eventEnabledMask |= BDSP_RAAGA_DEFAULT_EVENTS_ENABLE_MASK;

        /* Success */
        *pTask = &pRaagaTask->task;

        goto create_task_success;
    }
	else if (pRaagaContext->settings.contextType == BDSP_ContextType_eScm)
    {
        err = BDSP_Raaga_P_GenerateScmCITInput((void *)pRaagaTask,pSettings,pCitInput);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Generate CIT input %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;        
        }

		/* Pass the memory handle to cit for its purpose. SW7346-598 */
		pCitInput->memInfo.psCitDataStructure = pDevice->sCitBuffers.psCitDataStructure;
		for(i=0; i < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
		{
			pCitInput->memInfo.psFwBrnachInfo[i] = pDevice->sCitBuffers.psCitFwBranchInfo[i];
		}
		pCitInput->memInfo.psCitPortInfo	= pDevice->sCitBuffers.psCitPortCfg;
		pCitInput->memInfo.psCitTaskIsInfo	= pDevice->sCitBuffers.psCitTaskIsBuffCfg;


        err =BDSP_P_GenScmCit(pDevice->memHandle, pDevice->regHandle, pCitInput, &(pRaagaTask->scmCitOutput));
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Cit module %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;
        }

        BDSP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset(
            (pSettings->audioTaskDelayMode == BDSP_AudioTaskDelayMode_eDefault)?BDSP_CIT_P_LowDelayMode_eDisabled:BDSP_CIT_P_LowDelayMode_eEnabled, 
            pCitInput, &(pRaagaTask->ctbOutput));   

        BDSP_P_AnalyseScmCit(pDevice->memHandle, &(pRaagaTask->scmCitOutput.sScmCit));

        /* Download CIT structure into DRAM */
        err = BDSP_Raaga_P_CopyDataToDram(
            pDevice->memHandle, 
            (uint32_t *)(&(pRaagaTask->scmCitOutput.sScmCit)),      
            pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr,
            pRaagaTask->taskMemGrants.sCitStruct.ui32Size
            );

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in Copying data to DRAM"));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }

        /* Initialize interframe buffers for all the nodes */

        err = BDSP_DSP_P_InitInterframeBuffer((void *)pRaagaTask);  
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in initializing Interframe buffers for Task id %d",pRaagaTask->taskId));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }

#if 0 /* TODO: Vijay No configuration parameters are needed and hence commenting out. Need confirmation. */
        /* Initialize configuration parameters for all the nodes */

        for (uiBranchId = 0; uiBranchId < pRaagaTask->citOutput.ui32NumBranches ; uiBranchId++)
        {
            /* Get the configuration buffer address for this stage */
            for (uiStageId = 0; uiStageId < pRaagaTask->citOutput.sCitBranchInfo[uiBranchId].ui32NumStages; uiStageId++)
            {
                bPpConfigDone = false;
                psOpStgInfo = &(pRaagaTask->citOutput.sCitBranchInfo[uiBranchId].sCitStgInfo[uiStageId]);
                uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
                uiConfigBufAddr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufSize;

                /* Check if this is a decode stage */
                if (BDSP_CIT_P_StageType_eDecode== psOpStgInfo->eStageType    /*psAudProcNetwork->sAudProcessingStage[uiBranchId][uiStageId].bDecoderStage*/)
                {
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)uiConfigBufAddr) ||
                        (0 == uiConfigBufSize)) )
                    {
                        if (psOpStgInfo->uAudioMode.eDecAudioMode== BDSP_DecodeMode_ePassThru)
                        {
                            pSettingsBuffer = (void *)&(BDSP_sDefaultPassthruSettings);
                            settingsBufferSize = sizeof(BDSP_Raaga_Audio_PassthruConfigParams);
                        }
                        else if (psOpStgInfo->uAudioMode.eDecAudioMode == BDSP_DecodeMode_eDecode)
                        {
                            pSettingsBuffer = (void *)BDSP_sAudioCodecDetails[psOpStgInfo->uAlgorithm.eDecAudioAlgo]->pDefaultUserConfig;
                            settingsBufferSize = BDSP_sAudioCodecDetails[psOpStgInfo->uAlgorithm.eDecAudioAlgo]->userConfigSize;                                   
                        }
                        err = BDSP_Raaga_P_SetAudioDecodeStageSettings(
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->uAlgorithm.eDecAudioAlgo,
                            psOpStgInfo->uAudioMode.eDecAudioMode,
                            uiConfigBufAddr, uiConfigBufSize,pSettingsBuffer,settingsBufferSize);
                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }

                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr) ||
                        (0 ==  psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize)) )
                    {
                        BDBG_MSG (("Set Frame Sync Config Params"));

                        sTsmConfig = (BDSP_sDefaultFrameSyncTsmSettings);

                        sTsmConfig.sTsmConfigParams.ui32AudioOffset = pRaagaTask->ctbOutput.ui32AudOffset*45;

                        err = BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams (
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr, 
                            psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize,
                            &sTsmConfig);                    
                            
                        /* Write the settings into the host buffer as well - 
                                                                So that it can be used instead of the DSP's buffer when GetTsmSettings 
                                                                is called, to avoid any race conditions */
                        err = BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams (
                            pRaagaTask->pContext->pDevice->memHandle,
                            pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr,
                            pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size,
                            &sTsmConfig);

                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }
                }
                else if ((BDSP_CIT_P_StageType_ePostProc == psOpStgInfo->eStageType))
                {
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)uiConfigBufAddr) ||
                        (0 == uiConfigBufSize)) )
                    {
                        pSettingsBuffer = (void *)BDSP_sAudioProcessingDetails[psOpStgInfo->uAlgorithm.eProcAudioAlgo]->pDefaultUserConfig;
                        settingsBufferSize = BDSP_sAudioProcessingDetails[psOpStgInfo->uAlgorithm.eProcAudioAlgo]->userConfigSize;

                        err = BDSP_Raaga_P_SetAudioProcessingStageSettings(
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->uAlgorithm.eProcAudioAlgo,
                            psOpStgInfo->uAudioMode.eProcessingAudioMode,
                            uiConfigBufAddr, uiConfigBufSize,pSettingsBuffer,settingsBufferSize);
                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }
                }
                else if ((BDSP_CIT_P_StageType_eEncode== psOpStgInfo->eStageType))
                {
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)uiConfigBufAddr) ||
                        (0 == uiConfigBufSize)) )
                    {
                        pSettingsBuffer = (void *)BDSP_sAudioEncodeDetails[psOpStgInfo->uAlgorithm.eEncAudioAlgo]->pDefaultUserConfig;
                        settingsBufferSize = BDSP_sAudioEncodeDetails[psOpStgInfo->uAlgorithm.eEncAudioAlgo]->userConfigSize;

                        err = BDSP_Raaga_P_SetAudioEncodeStageSettings(
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->uAlgorithm.eEncAudioAlgo,
                            psOpStgInfo->uAudioMode.eEncAudioMode,
                            uiConfigBufAddr, uiConfigBufSize,pSettingsBuffer,settingsBufferSize);
                        if ( BERR_SUCCESS!=err )
                        {
                            goto err_gen_citinput;
                        }
                    }
                }


                /* Status Buffer */
                ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufAdr; 
                if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32StatusBufAdr)) &&
                    (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize))
                {
                    BKNI_Memset((void *)ui32StatusBufAdr,0xFF,psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize);
                }

                if (BDSP_CIT_P_StageType_eDecode== psOpStgInfo->eStageType)
                {
                    {
                        /* Initialize TSM status Buffer */
                        ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufAdr; 
                        if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32StatusBufAdr)) &&
                            (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize))
                        {
                            BKNI_Memset((void *)ui32StatusBufAdr,0XFF,psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufSize);
                        }
                    }
                }
            }         
        }
#endif
        /* Enable required asynchronous event notification */        
        pRaagaTask->eventEnabledMask |= BDSP_RAAGA_DEFAULT_EVENTS_ENABLE_MASK;
#if 0                                                                  
        pRaagaTask->eventEnabledMask |= (
            BDSP_FWIF_P_EventIdMask_eSampleRateChange   
            |BDSP_FWIF_P_EventIdMask_eFirstPTS_Received 
            |BDSP_FWIF_P_EventIdMask_eStreamInfoAvail
            |BDSP_FWIF_P_EventIdMask_eUnlicensedAlgo                                     
            |BDSP_FWIF_P_EventIdMask_eTSM_Lock 
            |BDSP_FWIF_P_EventIdMask_eFrameSyncLock 
            |BDSP_FWIF_P_EventIdMask_eFrameSyncLockLost
            |BDSP_FWIF_P_EventIdMask_eStartOnPTS
            |BDSP_FWIF_P_EventIdMask_eStopOnPTS
            |BDSP_FWIF_P_EventIdMask_eAstmTsmPass
            |BDSP_FWIF_P_EventIdMask_eCdbItbOverflow
            |BDSP_FWIF_P_EventIdMask_eCdbItbUnderflow
            |BDSP_FWIF_P_EventIdMask_eTsmFail                                 
            );  
#endif

#if 0                                                                  
        if (pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eDecAudioMode == BDSP_DecodeMode_eDecode)
        {
            pRaagaTask->eventEnabledMask  |= BDSP_FWIF_P_EventIdMask_eRampEnable;
        }
#endif

#if 0
#if BCHP_VCXO_MISC_AUDIO_MODE_CTRL
        unsigned regVal;
        /*
        Due to the default value of AUDIO PLL being set to select XTAL - the
        audio was not coming out on I2S. Ideally this should be part of Firmware
        along with PLL programming. JIRA FW35230-34 has been raised and assigned 
        to the Firmware team. This hardcoding should be removed once the Firmware 
        changes are done.
        */        
        regVal = BDSP_Read32 (pDevice->regHandle, BCHP_VCXO_MISC_AUDIO_MODE_CTRL);
        regVal &= ~ (BCHP_MASK (VCXO_MISC_AUDIO_MODE_CTRL, AUDIO0_REFCLK_SEL));
        regVal |= (BCHP_FIELD_ENUM (VCXO_MISC_AUDIO_MODE_CTRL,  AUDIO0_REFCLK_SEL, VCXO0_SEL));
        BDSP_Write32 (pDevice->regHandle, BCHP_VCXO_MISC_AUDIO_MODE_CTRL, regVal);
#endif
#endif

        /* Success */
        *pTask = &pRaagaTask->task;

        goto create_task_success;
    }
	else if (BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType )
    {
        psVEncoderBufCfg = (BDSP_VF_P_sVEncodeConfig *)BKNI_Malloc(sizeof(BDSP_VF_P_sVEncodeConfig));
        if (NULL==psVEncoderBufCfg)
        {
            BDBG_ERR(("Out of System memory"));
            err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto err_malloc_videobuffinput;
        }
        BKNI_Memset( psVEncoderBufCfg, 0, sizeof(BDSP_VF_P_sVEncoderConfig) );    

        err = BDSP_Raaga_P_GenerateVideoEncodeCITInput((void *)pRaagaTask, pSettings, pCitInput, psVEncoderBufCfg);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Generate Video Encode CIT input %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;        
        }

		/* Pass the memory handle to cit for its purpose. SW7346-598 */
		pCitInput->memInfo.psCitDataStructure = pDevice->sCitBuffers.psCitDataStructure;
		for(i=0; i < BDSP_P_MAX_FW_BRANCH_PER_FW_TASK; i++)
		{
			pCitInput->memInfo.psFwBrnachInfo[i] = pDevice->sCitBuffers.psCitFwBranchInfo[i];
		}
		pCitInput->memInfo.psCitPortInfo	= pDevice->sCitBuffers.psCitPortCfg;
		pCitInput->memInfo.psCitTaskIsInfo	= pDevice->sCitBuffers.psCitTaskIsBuffCfg;

        err =BDSP_P_VideoGenCit(pDevice->memHandle, pDevice->regHandle, pCitInput, (void *)psVEncoderBufCfg, &(pRaagaTask->videoCitOutput), BDSP_CIT_P_StageType_eVideoEncode);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("ERROR returned from Cit module %d!",err));
            err =BERR_TRACE(err);
            goto err_gen_citinput;
        }

        BDBG_MSG(("Video Encode GenCit Done"));

        /*	Analyze the CIT generated : First level Information */
        BDSP_P_AnalyseVideoCit(pDevice->memHandle, &(pRaagaTask->videoCitOutput.uVideoCit), BDSP_CIT_P_StageType_eVideoEncode);
        BDBG_MSG(("Analyse cit DONE"));

        /* Download CIT structure into DRAM */
        err = BDSP_Raaga_P_CopyDataToDram(
            pDevice->memHandle, 
            (uint32_t *)(&(pRaagaTask->videoCitOutput.uVideoCit.sVideoEncTaskConfig)),      
            pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr,
            pRaagaTask->taskMemGrants.sCitStruct.ui32Size
            );
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in Copying data to DRAM"));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }

        /* Initialize interframe buffers for all the nodes */
        err = BDSP_VID_P_InitInterframeBuffer((void *)pRaagaTask);  
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("Error in initializing Interframe buffers for Task id %d",pRaagaTask->taskId));
            err = BERR_TRACE(err);
            goto err_gen_citinput;
        }

        /* Initialize configuration parameters for all the nodes */
        ui32NumBranchesValid = pRaagaTask->videoCitOutput.ui32NumBranches;
        BDBG_MSG(("ui32NumBranchesValid = %x\n",ui32NumBranchesValid));
        for (uiBranchId = 0; uiBranchId < ui32NumBranchesValid ; uiBranchId++)
        {
            /* Get the configuration buffer address for this stage */
            ui32NumStagesValid = pRaagaTask->videoCitOutput.sCitBranchInfo[uiBranchId].ui32NumStages;
            BDBG_MSG(("ui32NumStagesValid = %x\n",ui32NumStagesValid));
            for (uiStageId = 0; uiStageId < ui32NumStagesValid; uiStageId++)
            {
                bPpConfigDone = false;
                psOpStgInfo = &(pRaagaTask->videoCitOutput.sCitBranchInfo[uiBranchId].sCitStgInfo[uiStageId]);
                uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
                uiConfigBufAddr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32UserParamBufSize;

                /* Check if this is a decode stage */
                if (BDSP_CIT_P_StageType_eVideoEncode== psOpStgInfo->eStageType)
                {
                    BDBG_MSG (("BDSP_Raaga_P_CreateTask : BuffAdr[%d] BufSize[%d]",
                        psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr,
                        psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize));
                    /* Decode stage. Program the decoder configuration parameters stored in
                    channel handle. */                        
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)uiConfigBufAddr) ||
                        (0 == uiConfigBufSize)) )
                    {
                        pSettingsBuffer = (void *)BDSP_sVideoEncodeCodecDetails[psOpStgInfo->uAlgorithm.eVideoEncAlgo]->pDefaultUserConfig;
                        settingsBufferSize = BDSP_sVideoEncodeCodecDetails[psOpStgInfo->uAlgorithm.eVideoEncAlgo]->userConfigSize;                                   

                        err = BDSP_Raaga_P_SetVideoEncodeStageSettings(
                            pRaagaTask->pContext->pDevice->memHandle,
                            psOpStgInfo->uAlgorithm.eVideoEncAlgo,
                            BDSP_EncodeMode_eRealtime,                        
                            uiConfigBufAddr, uiConfigBufSize,pSettingsBuffer,settingsBufferSize);
                        if ( BERR_SUCCESS!=err )
                        {
                            BDBG_ERR(("Error in BDSP_Raaga_P_SetVideoEncodeStageSettings\n"));
                            goto err_gen_citinput;
                        }
                    }

                    BDBG_MSG (("BDSP_Raaga_P_CreateTask : BuffAdr[%d] BufSize[%d]",
                        psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr,
                        psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize));   
                }
				
				if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr) ||
					(0 ==  psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize)) )
				{
					BDBG_MSG (("Set Video Encode Frame Sync Config Params"));
				
					sVideoFrameSyncConfig = (BDSP_sDefaultVideoEncodeFrameSyncSettings);
				
					err = BDSP_Raaga_P_SetVideoEncodeFrameSyncStageConfigParams (
						pRaagaTask->pContext->pDevice->memHandle,
						psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufAdr, 
						psOpStgInfo->sFwOpNodeInfo[0].ui32UserParamBufSize,
						&sVideoFrameSyncConfig);					 
						
					/* Write the settings into the host buffer as well - 
															So that it can be used instead of the DSP's buffer when GetTsmSettings 
															is called, to avoid any race conditions */
					err = BDSP_Raaga_P_SetVideoEncodeFrameSyncStageConfigParams (
						pRaagaTask->pContext->pDevice->memHandle,
						pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr,
						pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size,
						&sVideoFrameSyncConfig);
				
					if ( BERR_SUCCESS!=err )
					{
						goto err_gen_citinput;
					}
				}
                /* Status Buffer */
                ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufAdr; 
                if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32StatusBufAdr)) &&
                    (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize))
                {
                    (void)BMEM_ConvertAddressToCached(pDevice->memHandle, (void *)ui32StatusBufAdr, &pCached);
                    BKNI_Memset(pCached,0xFF,psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize);
                    (void)BMEM_FlushCache(pDevice->memHandle, pCached, psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize);
                }

                if (BDSP_CIT_P_StageType_eVideoDecode== psOpStgInfo->eStageType)
                {
                    /* Initialize TSM status Buffer */
                    ui32StatusBufAdr = psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufAdr; 
                    if (!(BDSP_Raaga_P_CheckIfPtrInvalid((void *)ui32StatusBufAdr)) &&
                        (0 != psOpStgInfo->sFwOpNodeInfo[uiStartNodeIndex].ui32StatusBufSize))
                    {
                        (void)BMEM_ConvertAddressToCached(pDevice->memHandle, (void *)ui32StatusBufAdr, &pCached);
                        BKNI_Memset(pCached,0XFF,psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufSize);
                        (void)BMEM_FlushCache(pDevice->memHandle, pCached, psOpStgInfo->sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufSize);
                    }
                }

            }        
        }       

        /* Enable required asynchronous event notification */        
        pRaagaTask->eventEnabledMask |= BDSP_RAAGA_DEFAULT_EVENTS_ENABLE_MASK;

        /* Success */
        *pTask = &pRaagaTask->task;

        goto create_task_success;
    }


    err_gen_citinput:

    err_malloc_videobuffinput:

    err_malloc_citinput:

    BDSP_Raaga_P_FreeStartTimeFWCtxt((void *)pRaagaTask);


    err_download_fw:

    BDSP_Raaga_P_InterruptUninstall((void *)pRaagaTask);


    err_interrupt_install:

    BDSP_Raaga_P_FreeAndInvalidateTask((void *)pRaagaTask);

    err_init_task:
    err_getfree_task:    
    create_task_success:    
    if (pCitInput)
        BKNI_Free(pCitInput);
    if (psVDecodeBuffCfg)
        BKNI_Free(psVDecodeBuffCfg);
	if(psVEncoderBufCfg)
		BKNI_Free(psVEncoderBufCfg);
    return err;
}



void BDSP_Raaga_P_DestroyTask(
    void *pTaskHandle
    )
{
    BDSP_RaagaTask *pTask = (BDSP_RaagaTask *)pTaskHandle;    
    BERR_Code err;  


    BDBG_OBJECT_ASSERT(pTask, BDSP_RaagaTask);
    BDBG_ASSERT(pTask->allocated);

    if (pTask->isStopped == false)
    {
        BDBG_WRN(("BDSP_Raaga_P_DestroyTask: Task is till in Start state. Stopping it."));
        BDSP_Raaga_P_StopTask(pTaskHandle);
    }

    err = BDSP_Raaga_P_FreeStartTimeFWCtxt(pTaskHandle);
    if ( BERR_SUCCESS!=err )
    {
        err = BERR_TRACE(err);
    }

    err = BDSP_Raaga_P_InterruptUninstall(pTaskHandle);
    if ( BERR_SUCCESS!=err )
    {
        err = BERR_TRACE(err);
    }

    err =BDSP_Raaga_P_FreeAndInvalidateTask(pTaskHandle);
    if ( BERR_SUCCESS!=err )
    {
        err = BERR_TRACE(err);
    }

    return;
}

BERR_Code BDSP_Raaga_P_GetFWSize (void *pDeviceHandle, BDSP_Img_Id firmware_id, uint32_t *size)
{
    void *image;    
    BERR_Code rc;   
    const void *data;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    const BIMG_Interface *iface;
    void *context;
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    

    iface = pDevice->settings.pImageInterface;  
    context = pDevice->settings.pImageContext;

    rc = iface->open (context, &image, (uint32_t)firmware_id);

    if (rc != BERR_SUCCESS)
    {
        BDBG_ERR(("\nError in Opening the Image Interface for FW_ID =%d \n",firmware_id));; 
        iface->close(image);
        return BERR_TRACE(rc);
    }

    rc = iface->next(image, 0, &data, 8);

    if (rc != BERR_SUCCESS)
    {
        BDBG_ERR(("\nError in fetching next chunk in Image Interface\n"));; 
        iface->close(image);
        return BERR_TRACE(rc);
    }

    *size =((uint32_t *) data)[0];

    iface->close(image);
    return rc;

}


static BERR_Code BDSP_DSP_P_DmaIn(  void *pDeviceHandle,
    uint8_t dataType,
    uint8_t swapType,
    uint32_t size,
    uint32_t src_addr,
    uint32_t dst_addr,
    unsigned uiDspOffset)
{
    uint32_t regVal, dmaCount = 0;
    BERR_Code err = BERR_SUCCESS;
    uint32_t tempsrc, tempdst, tempsize=0;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

    BDBG_MSG (("DMA In SRC = 0x%x, DST = 0x%x, SIZE = 0x%x", src_addr, dst_addr, size));

    tempsrc = src_addr;
    tempdst = dst_addr;
    tempsize = 0;

    while (size != 0)
    {
        tempsrc = tempsrc + tempsize;
        tempdst = tempdst + tempsize;

        if (size > BDSP_RAAGA_DMA_SIZE_PER_TRANSFER)
            tempsize = BDSP_RAAGA_DMA_SIZE_PER_TRANSFER;
        else
            tempsize = size;

        size = size - tempsize;

        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_DMA_SRC_ADDR_Q0 + uiDspOffset,tempsrc);
        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_DMA_DEST_ADDR_Q0 + uiDspOffset,tempdst);

        regVal = 0x0;
        regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_DMA_TRANSFER_Q0,SWAP_TYPE)))|
            (BCHP_FIELD_DATA(RAAGA_DSP_DMA_TRANSFER_Q0,SWAP_TYPE,swapType));
        regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_DMA_TRANSFER_Q0,DATA_TYPE)))|
            (BCHP_FIELD_DATA(RAAGA_DSP_DMA_TRANSFER_Q0,DATA_TYPE,dataType));
        regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_DMA_TRANSFER_Q0,TRANSFER_TYPE)))|
            (BCHP_FIELD_ENUM(RAAGA_DSP_DMA_TRANSFER_Q0,TRANSFER_TYPE,DMA_READ));
        regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_DMA_TRANSFER_Q0,TRANSFER_SIZE)))|
            (BCHP_FIELD_DATA(RAAGA_DSP_DMA_TRANSFER_Q0,TRANSFER_SIZE,tempsize));
        BDBG_MSG(("\n**** DMA TRANSFER0 = 0x%x", regVal));
        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_DMA_TRANSFER_Q0 + uiDspOffset,regVal);
        BDBG_MSG(("Waiting for DMA to complete..."));   

        regVal = BDSP_RAAGA_DMA_BUSY;
        while (regVal)
        {
            regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_DMA_STATUS + uiDspOffset);
            regVal = BCHP_GET_FIELD_DATA(regVal, RAAGA_DSP_DMA_STATUS, Q0_BUSY);
            dmaCount++;
            if (dmaCount==BDSP_RAAGA_DMA_TIMEOUT_COUNT)
            {
                BDBG_ERR(("DMA timeout: SRC Adr = 0x%x, DST Adr = 0x%x, "
                    "size = 0x%x", tempsrc, tempdst, tempsize));
                return BERR_TRACE(BERR_TIMEOUT);
            }
            BKNI_Delay(10);
        }

        BDBG_MSG(("DMA Done! for TempSRC Adr = 0x%x, TempDST Adr = 0x%x, "
            "Tempsize = 0x%x", tempsrc, tempdst, tempsize));
    }

    /* Clearing the Token ID for Queue 0/1/2/3. This needs to be done for other
        queues if they are used above. Clearning the token bits directly 
        since the Q0_BUSY is dealt with above. */
#ifdef FIREPATH_BM
    regVal = 0;
#else
    regVal = BDSP_Read32 (pDevice->regHandle,
        BCHP_RAAGA_DSP_DMA_TOKEN_ID_CLR_Q0 + uiDspOffset);

    regVal &= ~(BCHP_MASK (RAAGA_DSP_DMA_TOKEN_ID_CLR_Q0,TOKEN_ID_BITS));
#endif
    regVal |= BCHP_FIELD_DATA(RAAGA_DSP_DMA_TOKEN_ID_CLR_Q0,TOKEN_ID_BITS,0xFFFFFFFF); 

    BDSP_Write32 (pDevice->regHandle,
        BCHP_RAAGA_DSP_DMA_TOKEN_ID_CLR_Q0 + uiDspOffset,regVal);

#ifdef FIREPATH_BM
    regVal = 0;
#else
    regVal = BDSP_Read32 (pDevice->regHandle,
        BCHP_RAAGA_DSP_DMA_TOKEN_ID_CLR_Q1 + uiDspOffset);

    regVal &= ~(BCHP_MASK (RAAGA_DSP_DMA_TOKEN_ID_CLR_Q1,TOKEN_ID_BITS));
#endif
    regVal |= BCHP_FIELD_DATA(RAAGA_DSP_DMA_TOKEN_ID_CLR_Q1,TOKEN_ID_BITS,0xFFFFFFFF); 

    BDSP_Write32 (pDevice->regHandle,
        BCHP_RAAGA_DSP_DMA_TOKEN_ID_CLR_Q1 + uiDspOffset,regVal);

#ifdef FIREPATH_BM
    regVal = 0;
#else
    regVal = BDSP_Read32 (pDevice->regHandle,
        BCHP_RAAGA_DSP_DMA_TOKEN_ID_CLR_Q2 + uiDspOffset);

    regVal &= ~(BCHP_MASK (RAAGA_DSP_DMA_TOKEN_ID_CLR_Q2,TOKEN_ID_BITS));
#endif
    regVal |= BCHP_FIELD_DATA(RAAGA_DSP_DMA_TOKEN_ID_CLR_Q2,TOKEN_ID_BITS,0xFFFFFFFF); 

    BDSP_Write32 (pDevice->regHandle,
        BCHP_RAAGA_DSP_DMA_TOKEN_ID_CLR_Q2 + uiDspOffset,regVal);

#ifdef FIREPATH_BM
    regVal = 0;
#else
    regVal = BDSP_Read32 (pDevice->regHandle,
        BCHP_RAAGA_DSP_DMA_TOKEN_ID_CLR_Q3 + uiDspOffset);

    regVal &= ~(BCHP_MASK (RAAGA_DSP_DMA_TOKEN_ID_CLR_Q3,TOKEN_ID_BITS));
#endif
    regVal |= BCHP_FIELD_DATA(RAAGA_DSP_DMA_TOKEN_ID_CLR_Q3,TOKEN_ID_BITS,0xFFFFFFFF); 
    BDSP_Write32 (pDevice->regHandle,
        BCHP_RAAGA_DSP_DMA_TOKEN_ID_CLR_Q3 + uiDspOffset,regVal);    


    return err;
}




static void BDSP_P_InitializeMit(BDSP_Raaga_P_Mit    *pMit)
{
    uint32_t    i=0;
    for (i = 0; i < BDSP_AF_P_AlgoId_eMax; i++)
    {
        pMit->sAlgoDwnldDetail[i].ui32AlgoCodePtr = 0xffffffff;
        pMit->sAlgoDwnldDetail[i].ui32Algoui32CodeSize= 0;
        pMit->sAlgoDwnldDetail[i].ui32AlgoDataPtr = 0xffffffff;
        pMit->sAlgoDwnldDetail[i].ui32AlgoDataSize= 0;      
        pMit->isExecutablesDownloaded[i] =false;
    }
}


static void BDSP_P_MapImgIdWithExecId(void *pDeviceHandle,BDSP_Raaga_P_ImgIdMappingArrays *pimgIdMappingArrays)
{
    unsigned count = 0, size = 0,i=0;
    BDSP_Img_Id imgId=BDSP_Img_Id_eInvalid;
    BDSP_AF_P_AlgoId algoId = BDSP_AF_P_AlgoId_eInvalid;


    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    

    BDBG_ENTER(BDSP_MapImgIdWithExecId);

    for (count = 0; count < BDSP_AF_P_AlgoId_eMax; count++)
    {
        pimgIdMappingArrays->codeImgId[count] = BDSP_Img_Id_eInvalid;
        pimgIdMappingArrays->tableImgId[count] = BDSP_Img_Id_eInvalid;  
        pimgIdMappingArrays->ui32CodeSize[count] = 0;
        pimgIdMappingArrays->tableSize[count] = 0;      
        pimgIdMappingArrays->interframeImgId[count]= BDSP_Img_Id_eInvalid;
        pimgIdMappingArrays->interframeSize[count]= 0;      
    }

    /* Mapping ids for Passthru */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_ePassThru] = BDSP_Img_Id_ePassthruCode;
    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_ePassThru] = BDSP_Img_Id_ePassthruTable;

    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePassthruCode, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_ePassThru] = BDSP_RAAGA_ALIGN(size, 2);

    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePassthruTable, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_ePassThru] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_ePassThru] = BDSP_Img_Id_ePassthruInterFrame;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePassthruInterFrame, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_ePassThru]= BDSP_RAAGA_ALIGN(size, 2);     

    for (count =0 ;count < BDSP_AudioType_eMax;count++)
    {
        if (BDSP_Raaga_P_IsAudioTypeSupported(count))
        {
            if (BDSP_sAudioCodecDetails[count]->audioType != count)
            {
                BDBG_ERR(("BDSP_sAudioCodecDetails array not populated correctly for Algo ID =%d",count));
                BDBG_ASSERT(0);
            }

            for (i =0 ;i < BDSP_sDecAlgoExecInfo[count][BDSP_DecodeMode_eDecode].NumNodes;i++)
            {
                algoId = BDSP_sDecAlgoExecInfo[count][BDSP_DecodeMode_eDecode].eAlgoIds[i];  
                if (algoId != BDSP_AF_P_AlgoId_eInvalid)
                {
                    imgId = BDSP_sAudioCodecDetails[count]->codeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->codeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->ui32CodeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }

                    imgId = BDSP_sAudioCodecDetails[count]->tableImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->tableImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->tableSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }

                    imgId = BDSP_sAudioCodecDetails[count]->interframeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->interframeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->interframeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                }
            }
        }
        else if (BDSP_Raaga_P_IsPassthruSupportedWithoutLicense(count))
        {
            algoId = BDSP_sDecAlgoExecInfo[count][BDSP_DecodeMode_ePassThru].eAlgoIds[0];  
            if (algoId != BDSP_AF_P_AlgoId_eInvalid)
            {
                imgId = BDSP_sAudioCodecDetails[count]->passthruIdsCode;
                if (imgId != BDSP_Img_Id_eInvalid)
                {
                    pimgIdMappingArrays->codeImgId[algoId] = imgId;
                    BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                    pimgIdMappingArrays->ui32CodeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                }

                imgId = BDSP_sAudioCodecDetails[count]->passthruIdsInterframe;
                if (imgId != BDSP_Img_Id_eInvalid)
                {
                    pimgIdMappingArrays->interframeImgId[algoId] = imgId;
                    BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                    pimgIdMappingArrays->interframeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                }
            }
        }
        
        /* This is being added to initialize the img arrays for Passthru for those codecs which do not use the Generic Passthru module */
        if (BDSP_sAudioCodecDetails[count]->pCodecSpecPtDetails != NULL)
        {
            /* The  IDS image id gets populated in the above if / else if case - Only PT Img Ids need to be populated here */
            algoId = BDSP_sDecAlgoExecInfo[count][BDSP_DecodeMode_ePassThru].eAlgoIds[1];  
            if (algoId != BDSP_AF_P_AlgoId_eInvalid)
            {
                imgId = BDSP_sAudioCodecDetails[count]->pCodecSpecPtDetails->passthrucodeImgId;
                if (imgId != BDSP_Img_Id_eInvalid)
                {
                    pimgIdMappingArrays->codeImgId[algoId] = imgId;
                    BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                    pimgIdMappingArrays->ui32CodeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                }

                imgId = BDSP_sAudioCodecDetails[count]->pCodecSpecPtDetails->passthrutableImgId;
                if (imgId != BDSP_Img_Id_eInvalid)
                {
                    pimgIdMappingArrays->tableImgId[algoId] = imgId;
                    BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                    pimgIdMappingArrays->tableSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                }

                imgId = BDSP_sAudioCodecDetails[count]->pCodecSpecPtDetails->passthruinterframeImgId;
                if (imgId != BDSP_Img_Id_eInvalid)
                {
                    pimgIdMappingArrays->interframeImgId[algoId] = imgId;
                    BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                    pimgIdMappingArrays->interframeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                }
            }
        }
    }

    for (count =0 ;count < BDSP_AudioEncode_eMax;count++)
    {
        if (BDSP_Raaga_P_IsAudioEncodeSupported(count))
        {
            if (BDSP_sAudioEncodeDetails[count]->encodeType != count)
            {
                BDBG_ERR(("BDSP_sAudioEncodeDetails array not populated correctly for Algo ID =%d",count));
                BDBG_ASSERT(0);
            }
            for (i =0 ;i < BDSP_sEncAlgoExecInfo[count][0].NumNodes;i++)
            {
                algoId = BDSP_sEncAlgoExecInfo[count][0].eAlgoIds[i];  
                if (algoId != BDSP_AF_P_AlgoId_eInvalid)
                {
                    imgId = BDSP_sAudioEncodeDetails[count]->codeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->codeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->ui32CodeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                    imgId = BDSP_sAudioEncodeDetails[count]->tableImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->tableImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->tableSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                    imgId = BDSP_sAudioEncodeDetails[count]->interframeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->interframeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->interframeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                }
            }
        }
    }

    for (count =0 ;count < BDSP_VideoType_eMax;count++)
    {
        if (BDSP_Raaga_P_IsVideoTypeSupported(count))
        {
            if (BDSP_sVideoCodecDetails[count]->videoType != count)
            {
                BDBG_ERR(("BDSP_sVideoCodecDetails array not populated correctly for Algo ID =%d",count));
                BDBG_ASSERT(0);
            }
            for (i =0 ;i < BDSP_sVideoDecAlgoExecInfo[count][0].NumNodes;i++)
            {
                algoId = BDSP_sVideoDecAlgoExecInfo[count][0].eAlgoIds[i];  
                if (algoId != BDSP_AF_P_AlgoId_eInvalid)
                {
                    imgId = BDSP_sVideoCodecDetails[count]->codeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->codeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->ui32CodeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                    imgId = BDSP_sVideoCodecDetails[count]->tableImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->tableImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->tableSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                    imgId = BDSP_sVideoCodecDetails[count]->interframeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->interframeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->interframeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                }
            }
        }
    }
	for (count =0 ;count < BDSP_ScmType_eMax; count++)
    {
        if (BDSP_Raaga_P_IsScmTypeSupported(count))
        {
            if (BDSP_sScmCodecDetails[count]->scmType != count)
            {
                BDBG_ERR(("BDSP_sScmCodecDetails array not populated correctly for BDSP Algo ID =%d",count));
                BDBG_ASSERT(0);
            }
            for (i =0 ;i < BDSP_sScmAlgoExecInfo[count].NumNodes;i++)
            {
                algoId = BDSP_sScmAlgoExecInfo[count].eAlgoIds[i];  
                if (algoId != BDSP_AF_P_AlgoId_eInvalid)
                {
                    imgId = BDSP_sScmCodecDetails[count]->codeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->codeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->ui32CodeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                    imgId = BDSP_sScmCodecDetails[count]->tableImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->tableImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->tableSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                    imgId = BDSP_sScmCodecDetails[count]->interframeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->interframeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->interframeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                }
            }
        }
    }
	/* map video encode IDs */
	for (count =0 ;count < BDSP_VideoEncode_eMax;count++)
    {
        if (BDSP_Raaga_P_IsVideoEncodeTypeSupported(count))
        {
            if (BDSP_sVideoEncodeCodecDetails[count]->encodeType != count)
            {
                BDBG_ERR(("BDSP_sVideoEncodeCodecDetails array not populated correctly for BDSP Algo ID =%d",count));
                BDBG_ASSERT(0);
            }
            for (i =0 ;i < BDSP_sVideoEncAlgoExecInfo[count][0].NumNodes;i++)
            {
                algoId = BDSP_sVideoEncAlgoExecInfo[count][0].eAlgoIds[i];  
                if (algoId != BDSP_AF_P_AlgoId_eInvalid)
                {
                    imgId = BDSP_sVideoEncodeCodecDetails[count]->codeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->codeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->ui32CodeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                    imgId = BDSP_sVideoEncodeCodecDetails[count]->tableImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->tableImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->tableSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                    imgId = BDSP_sVideoEncodeCodecDetails[count]->interframeImgId[i];
                    if (imgId != BDSP_Img_Id_eInvalid)
                    {
                        pimgIdMappingArrays->interframeImgId[algoId] = imgId;
                        BDSP_Raaga_P_GetFWSize (pDeviceHandle, imgId, &size);
                        pimgIdMappingArrays->interframeSize[algoId] = BDSP_RAAGA_ALIGN(size, 2);                                
                    }
                }
            }
        }
    }


#ifdef BDSP_WMAPROPASSTHRU_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eWMAPassThrough] = (uint32_t)BDSP_Img_Id_eWmaPro_Passthru_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eWmaPro_Passthru_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eWMAPassThrough] = BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_AVL_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAvlPostProc] = (uint32_t)BDSP_Img_Id_eAvl_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAvl_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAvlPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eAvlPostProc] = (uint32_t)BDSP_Img_Id_eAvl_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAvl_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eAvlPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAvlPostProc] = BDSP_Img_Id_eAvl_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAvl_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAvlPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_PL2_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_ePl2PostProc] = (uint32_t)BDSP_Img_Id_ePL2_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePL2_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_ePl2PostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_ePl2PostProc] = (uint32_t)BDSP_Img_Id_ePL2_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePL2_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_ePl2PostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_ePl2PostProc] = BDSP_Img_Id_ePL2_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePL2_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_ePl2PostProc]= BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef 	BDSP_SRSXT_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc] = (uint32_t)BDSP_Img_Id_eSrsXt_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsXt_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc] = (uint32_t)BDSP_Img_Id_eSrsXt_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsXt_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc] = BDSP_Img_Id_eSrsXt_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsXt_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eSrsTruSurroundPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_SRSHD_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = (uint32_t)BDSP_Img_Id_eSrsHd_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsHd_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = (uint32_t)BDSP_Img_Id_eSrsHd_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsHd_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc] = BDSP_Img_Id_eSrsHd_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsHd_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eSrsTruSurroundHDPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_SRSTRUVOL_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eSrsTruVolumePostProc] = (uint32_t)BDSP_Img_Id_eSrsTruVolume_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsTruVolume_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eSrsTruVolumePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eSrsTruVolumePostProc] = (uint32_t)BDSP_Img_Id_eSrsTruVolume_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsTruVolume_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eSrsTruVolumePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eSrsTruVolumePostProc] = BDSP_Img_Id_eSrsTruVolume_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsTruVolume_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eSrsTruVolumePostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif


#ifdef BDSP_CUSTOMVOICE_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eCustomVoicePostProc] = (uint32_t)BDSP_Img_Id_eCustomVoice_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomVoice_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eCustomVoicePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eCustomVoicePostProc] = (uint32_t)BDSP_Img_Id_eCustomVoice_Tables;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomVoice_Tables, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eCustomVoicePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eCustomVoicePostProc] = BDSP_Img_Id_eCustomVoice_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomVoice_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eCustomVoicePostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef BDSP_CUSTOMSURROUND_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eCustomSurroundPostProc] = (uint32_t)BDSP_Img_Id_eCustomSurround_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomSurround_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eCustomSurroundPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eCustomSurroundPostProc] = (uint32_t)BDSP_Img_Id_eCustomSurround_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomSurround_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eCustomSurroundPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eCustomSurroundPostProc] = BDSP_Img_Id_eCustomSurround_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomSurround_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eCustomSurroundPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef BDSP_CUSTOMBASS_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eCustomBassPostProc] = (uint32_t)BDSP_Img_Id_eCustomBass_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomBass_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eCustomBassPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eCustomBassPostProc] = (uint32_t)BDSP_Img_Id_eCustomBass_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomBass_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eCustomBassPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eCustomBassPostProc] = BDSP_Img_Id_eCustomBass_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomBass_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eCustomBassPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef BDSP_SRC_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eSrcPostProc] = (uint32_t)BDSP_Img_Id_eSrc_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrc_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eSrcPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eSrcPostProc] = (uint32_t)BDSP_Img_Id_eSrc_Tables;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrc_Tables, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eSrcPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eSrcPostProc] = BDSP_Img_Id_eSrc_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrc_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eSrcPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef BDSP_AUDIODESC_SUPPORT
    /* Fade Control */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAudioDescriptorFadePostProc] = (uint32_t)BDSP_Img_Id_eFadeControl_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eFadeControl_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAudioDescriptorFadePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /* Pan Control */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = (uint32_t)BDSP_Img_Id_ePanControl_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePanControl_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = (uint32_t)BDSP_Img_Id_ePanControl_Tables;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePanControl_Tables, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc] = BDSP_Img_Id_ePanControl_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePanControl_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAudioDescriptorPanPostProc]= BDSP_RAAGA_ALIGN(size, 2);    
#endif

#ifdef BDSP_PCM_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_ePesFrameSync] = (uint32_t)BDSP_Img_Id_ePcm_Ids;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePcm_Ids, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_ePesFrameSync] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_ePesFrameSync] = BDSP_Img_Id_ePcm_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePcm_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_ePesFrameSync]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef BDSP_PCMROUTER_SUPPORT
    /* PCM Router */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_ePCMRouterPostProc] = (uint32_t)BDSP_Img_Id_ePCMRouter_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_ePCMRouter_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_ePCMRouterPostProc] = BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef 	BDSP_DTSENC_SUPPORT
    /* DTS Encode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eDtsEncode] = (uint32_t)BDSP_Img_Id_eDts_Encode_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDts_Encode_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eDtsEncode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eDtsEncode] = (uint32_t)BDSP_Img_Id_eDts_Encode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDts_Encode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eDtsEncode] = BDSP_RAAGA_ALIGN(size, 2);

    /* DTS Encode Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eDtsEncode] = BDSP_Img_Id_eDts_Encode_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDts_Encode_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eDtsEncode]= BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef BDSP_MS10_SUPPORT
    /*  Dolby Transcode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eMs10DDTranscode] = (uint32_t)BDSP_Img_Id_eDolbyTranscode_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDolbyTranscode_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eMs10DDTranscode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eMs10DDTranscode] = (uint32_t)BDSP_Img_Id_eDolbyTranscode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDolbyTranscode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eMs10DDTranscode] = BDSP_RAAGA_ALIGN(size, 2);

    /*  Dolby Transcode Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eMs10DDTranscode] = BDSP_Img_Id_eDolbyTranscode_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDolbyTranscode_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eMs10DDTranscode]= BDSP_RAAGA_ALIGN(size, 2);
#else    
#ifdef 	BDSP_AC3ENC_SUPPORT

    /* Ac3 Encode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAc3Encode] = (uint32_t)BDSP_Img_Id_eAc3_Encode_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAc3_Encode_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAc3Encode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eAc3Encode] = (uint32_t)BDSP_Img_Id_eAc3_Encode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAc3_Encode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eAc3Encode] = BDSP_RAAGA_ALIGN(size, 2);

    /* Ac3 Encode Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAc3Encode] = BDSP_Img_Id_eAc3_Encode_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAc3_Encode_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAc3Encode]= BDSP_RAAGA_ALIGN(size, 2);

#endif
#endif

#ifdef 	BDSP_DSOLA_SUPPORT

    /* DSOLA Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eDsolaPostProc] = (uint32_t)BDSP_Img_Id_eDsola_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDsola_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eDsolaPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eDsolaPostProc] = (uint32_t)BDSP_Img_Id_eDsola_Tables;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDsola_Tables, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eDsolaPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /* DSOLA Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eDsolaPostProc] = BDSP_Img_Id_eDsola_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDsola_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eDsolaPostProc]= BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef 	BDSP_DOLBYVOL_SUPPORT

    /* Dolby Vol Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eDolbyVolumePostProc] = (uint32_t)BDSP_Img_Id_eDolbyVol_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDolbyVol_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eDolbyVolumePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eDolbyVolumePostProc] = (uint32_t)BDSP_Img_Id_eDolbyVol_Tables;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDolbyVol_Tables, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eDolbyVolumePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /* Dolby Vol Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eDolbyVolumePostProc] = BDSP_Img_Id_eDolbyVol_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDolbyVol_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eDolbyVolumePostProc]= BDSP_RAAGA_ALIGN(size, 2);

#endif



#ifdef BDSP_MP3ENC_SUPPORT
    /*MP3 Encode Decode Code*/

    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eMpegL3Encode] = (uint32_t)BDSP_Img_Id_eMp3Enc_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eMp3Enc_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eMpegL3Encode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eMpegL3Encode] = (uint32_t)BDSP_Img_Id_eMp3Enc_Code_Tables;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eMp3Enc_Code_Tables, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eMpegL3Encode] = BDSP_RAAGA_ALIGN(size, 2);    

    /* MP3 Encode Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eMpegL3Encode] = BDSP_Img_Id_eMp3Enc_Code_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eMp3Enc_Code_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eMpegL3Encode]= BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef BDSP_AMR_SUPPORT
    /* AMR FrameSync */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAmrFrameSync] = (uint32_t)BDSP_Img_Id_eAMR_Ids;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMR_Ids, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAmrFrameSync] = BDSP_RAAGA_ALIGN(size, 2);

    /* AMR Decode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAmrDecode] = (uint32_t)BDSP_Img_Id_eAMR_Decode;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMR_Decode, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAmrDecode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eAmrDecode] = (uint32_t)BDSP_Img_Id_eAMR_Decode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMR_Decode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eAmrDecode] = BDSP_RAAGA_ALIGN(size, 2);    

    /* AMR Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAmrDecode] = BDSP_Img_Id_eAMR_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMR_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAmrDecode]= BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAmrFrameSync] = BDSP_Img_Id_eAMR_Ids_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMR_Ids_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAmrFrameSync]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef BDSP_AMRWB_SUPPORT
    /* AMRWB FrameSync */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAmrWbFrameSync] = (uint32_t)BDSP_Img_Id_eAMRWB_Ids;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMRWB_Ids, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAmrWbFrameSync] = BDSP_RAAGA_ALIGN(size, 2);

    /* AMRWB Decode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAmrWbDecode] = (uint32_t)BDSP_Img_Id_eAMRWB_Decode;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMRWB_Decode, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAmrWbDecode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eAmrWbDecode] = (uint32_t)BDSP_Img_Id_eAMRWB_Decode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMRWB_Decode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eAmrWbDecode] = BDSP_RAAGA_ALIGN(size, 2);    

    /* AMRWB Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAmrWbDecode] = BDSP_Img_Id_eAMRWB_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMRWB_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAmrWbDecode]= BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAmrWbFrameSync] = BDSP_Img_Id_eAMRWB_Ids_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAMRWB_Ids_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAmrWbFrameSync]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef BDSP_ILBC_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eiLBCFrameSync] = (uint32_t)BDSP_Img_Id_eiLBC_Ids;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiLBC_Ids, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eiLBCFrameSync] = BDSP_RAAGA_ALIGN(size, 2);
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eiLBCDecode] = (uint32_t)BDSP_Img_Id_eiLBC_Decode;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiLBC_Decode, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eiLBCDecode] = BDSP_RAAGA_ALIGN(size, 2);
    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eiLBCDecode] = (uint32_t)BDSP_Img_Id_eiLBC_Decode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiLBC_Decode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eiLBCDecode] = BDSP_RAAGA_ALIGN(size, 2);    
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eiLBCDecode] = BDSP_Img_Id_eiLBC_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiLBC_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eiLBCDecode]= BDSP_RAAGA_ALIGN(size, 2);
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eiLBCFrameSync] = BDSP_Img_Id_eiLBC_Ids_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiLBC_Ids_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eiLBCFrameSync]= BDSP_RAAGA_ALIGN(size, 2);
#endif


#ifdef BDSP_ISAC_SUPPORT
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eiSACFrameSync] = (uint32_t)BDSP_Img_Id_eiSAC_Ids;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiSAC_Ids, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eiSACFrameSync] = BDSP_RAAGA_ALIGN(size, 2);
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eiSACDecode] = (uint32_t)BDSP_Img_Id_eiSAC_Decode;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiSAC_Decode, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eiSACDecode] = BDSP_RAAGA_ALIGN(size, 2);
    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eiSACDecode] = (uint32_t)BDSP_Img_Id_eiSAC_Decode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiSAC_Decode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eiSACDecode] = BDSP_RAAGA_ALIGN(size, 2);    
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eiSACDecode] = BDSP_Img_Id_eiSAC_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiSAC_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eiSACDecode]= BDSP_RAAGA_ALIGN(size, 2);
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eiSACFrameSync] = BDSP_Img_Id_eiSAC_Ids_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiSAC_Ids_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eiSACFrameSync]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_SBCENC_SUPPORT
    /*Sbc Encode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eSbcEncode] = (uint32_t)BDSP_Img_Id_eSbc_Encode_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSbc_Encode_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eSbcEncode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eSbcEncode] = (uint32_t)BDSP_Img_Id_eSbc_Encode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSbc_Encode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eSbcEncode] = BDSP_RAAGA_ALIGN(size, 2);

    /* Sbc Encode Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eSbcEncode] = BDSP_Img_Id_eSbc_Encode_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSbc_Encode_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eSbcEncode]= BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef 	BDSP_ILBCENC_SUPPORT
    /*iLBC Encode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eiLBCEncode] = (uint32_t)BDSP_Img_Id_eiLBC_Encode_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiLBC_Encode_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eiLBCEncode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eiLBCEncode] = (uint32_t)BDSP_Img_Id_eiLBC_Encode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiLBC_Encode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eiLBCEncode] = BDSP_RAAGA_ALIGN(size, 2);

    /* Sbc Encode Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eiLBCEncode] = BDSP_Img_Id_eiLBC_Encode_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiLBC_Encode_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eiLBCEncode]= BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef 	BDSP_ISACENC_SUPPORT
    /*iLBC Encode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eiSACEncode] = (uint32_t)BDSP_Img_Id_eiSAC_Encode_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiSAC_Encode_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eiSACEncode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eiSACEncode] = (uint32_t)BDSP_Img_Id_eiSAC_Encode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiSAC_Encode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eiSACEncode] = BDSP_RAAGA_ALIGN(size, 2);

    /* Sbc Encode Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eiSACEncode] = BDSP_Img_Id_eiSAC_Encode_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eiSAC_Encode_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eiSACEncode]= BDSP_RAAGA_ALIGN(size, 2);

#endif



#ifdef  BDSP_REALAUDIOLBR_SUPPORT    
    /* RA LBR Decode Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eRealAudioLbrDecode] = (uint32_t)BDSP_Img_Id_eRealAudioLbr_Decode_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eRealAudioLbr_Decode_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eRealAudioLbrDecode] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eRealAudioLbrDecode] = (uint32_t)BDSP_Img_Id_eRealAudioLbr_Decode_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eRealAudioLbr_Decode_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eRealAudioLbrDecode] = BDSP_RAAGA_ALIGN(size, 2);    

    /*  RA LBR Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eRealAudioLbrDecode] = BDSP_Img_Id_eRealAudioLbr_Decode_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eRealAudioLbr_Decode_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eRealAudioLbrDecode]= BDSP_RAAGA_ALIGN(size, 2);   
#endif

#ifdef 	BDSP_ADV_SUPPORT
    /*  Audyssey Volume Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAudysseyVolumePostProc] = (uint32_t)BDSP_Img_Id_eAvd_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAvd_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAudysseyVolumePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eAudysseyVolumePostProc] = (uint32_t)BDSP_Img_Id_eAvd_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAvd_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eAudysseyVolumePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  Audyssey Volume Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAudysseyVolumePostProc] = BDSP_Img_Id_eAvd_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAvd_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAudysseyVolumePostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_ABX_SUPPORT
    /*  Audyssey ABX Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eAudysseyABXPostProc] = (uint32_t)BDSP_Img_Id_eAbx_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAbx_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eAudysseyABXPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eAudysseyABXPostProc] = (uint32_t)BDSP_Img_Id_eAbx_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAbx_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eAudysseyABXPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  Audyssey ABX Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eAudysseyABXPostProc] = BDSP_Img_Id_eAbx_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eAbx_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eAudysseyABXPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_BRCM3DSURROUND_SUPPORT
    /*  Brcm 3D Surround Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = (uint32_t)BDSP_Img_Id_eBrcm3DSurround_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eBrcm3DSurround_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = (uint32_t)BDSP_Img_Id_eBrcm3DSurround_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eBrcm3DSurround_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  Brcm 3D Surround Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc] = BDSP_Img_Id_eBrcm3DSurround_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eBrcm3DSurround_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eBrcm3DSurroundPostProc]= BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef 	BDSP_MONODOWNMIX_SUPPORT
    /*  Mono Downmix Code */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eMonoDownMixPostProc] = (uint32_t)BDSP_Img_Id_eMonoDownmix_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eMonoDownmix_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eMonoDownMixPostProc] = BDSP_RAAGA_ALIGN(size, 2);
#endif


#ifdef 	BDSP_FWMIXER_SUPPORT
    /* Firmware Mixer FrameSync*/
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eMixerFrameSync] = (uint32_t)BDSP_Img_Id_eMixer_Ids;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eMixer_Ids, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eMixerFrameSync] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eMixerFrameSync] = (uint32_t)BDSP_Img_Id_eMixer_Ids_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eMixer_Ids_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eMixerFrameSync]= BDSP_RAAGA_ALIGN(size, 2);

    /*  Firmware Mixer Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eFWMixerPostProc] = (uint32_t)BDSP_Img_Id_eFwMixer_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eFwMixer_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eFWMixerPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eFWMixerPostProc] = (uint32_t)BDSP_Img_Id_eFwMixer_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eFwMixer_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eFWMixerPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  Firmware Mixer Surround Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eFWMixerPostProc] = BDSP_Img_Id_eFwMixer_Inter_frame;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eFwMixer_Inter_frame, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eFWMixerPostProc]= BDSP_RAAGA_ALIGN(size, 2);

#endif

#ifdef 	BDSP_SRSCSTD_SUPPORT
    /*  SRS Circle-Surround and Tru-Dialogue Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc] = (uint32_t)BDSP_Img_Id_eSrsCsTd_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsCsTd_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc] = (uint32_t)BDSP_Img_Id_eSrsCsTd_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsCsTd_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  SRS Circle-Surround and Tru-Dialogue Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc] = BDSP_Img_Id_eSrsCsTd_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsCsTd_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eSrsCircleSurrPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_SRSEQHL_SUPPORT
    /*  SRS Equalizer and Hard Limiter Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eSrsEqualizerPostProc] = (uint32_t)BDSP_Img_Id_eSrsEqHl_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsEqHl_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eSrsEqualizerPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eSrsEqualizerPostProc] = (uint32_t)BDSP_Img_Id_eSrsEqHl_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsEqHl_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eSrsEqualizerPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  SRS Equalizer and Hard Limiter Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eSrsEqualizerPostProc] = BDSP_Img_Id_eSrsEqHl_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSrsEqHl_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eSrsEqualizerPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_CUSTOMDBE_SUPPORT
    /*  Dynamic Bass Enhancement Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eCustomDbePostProc] = (uint32_t)BDSP_Img_Id_eCustomDbe_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomDbe_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eCustomDbePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eCustomDbePostProc] = (uint32_t)BDSP_Img_Id_eCustomDbe_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomDbe_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eCustomDbePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  Dynamic Bass Enhancement Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eCustomDbePostProc] = BDSP_Img_Id_eCustomDbe_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomDbe_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eCustomDbePostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_CUSTOMAVLP_SUPPORT
    /*  AVL+ Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eCustomAvlpPostProc] = (uint32_t)BDSP_Img_Id_eCustomAvlp_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomAvlp_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eCustomAvlpPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eCustomAvlpPostProc] = (uint32_t)BDSP_Img_Id_eCustomAvlp_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomAvlp_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eCustomAvlpPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  AVL+ Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eCustomAvlpPostProc] = BDSP_Img_Id_eCustomAvlp_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomAvlp_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eCustomAvlpPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_CUSTOMACF_SUPPORT
    /*  AVL+ Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eCustomAcfPostProc] = (uint32_t)BDSP_Img_Id_eCustomAcf_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomAcf_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eCustomAcfPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eCustomAcfPostProc] = (uint32_t)BDSP_Img_Id_eCustomAcf_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomAcf_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eCustomAcfPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  AVL+ Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eCustomAcfPostProc] = BDSP_Img_Id_eCustomAcf_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eCustomAcf_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eCustomAcfPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_GENCDBITB_SUPPORT
    /*  GenCdbITB Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eGenCdbItbPostProc] = (uint32_t)BDSP_Img_Id_eGenItbCdb_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eGenItbCdb_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eGenCdbItbPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eGenCdbItbPostProc] = (uint32_t)BDSP_Img_Id_eGenItbCdb_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eGenItbCdb_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eGenCdbItbPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  GenCdbITB Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eGenCdbItbPostProc] = BDSP_Img_Id_eGenItbCdb_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eGenItbCdb_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eGenCdbItbPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_BTSCENC_SUPPORT
    /*  BtscEncoder Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eBtscEncoderPostProc] = (uint32_t)BDSP_Img_Id_eBtscEncoder_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eBtscEncoder_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eBtscEncoderPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eBtscEncoderPostProc] = (uint32_t)BDSP_Img_Id_eBtscEncoder_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eBtscEncoder_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eBtscEncoderPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*  BtscEncoder Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eBtscEncoderPostProc] = BDSP_Img_Id_eBtscEncoder_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eBtscEncoder_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eBtscEncoderPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_DV258_SUPPORT
    /*  DV258 Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eDv258PostProc] = (uint32_t)BDSP_Img_Id_eDV258_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDV258_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eDv258PostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eDv258PostProc] = (uint32_t)BDSP_Img_Id_eDV258_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDV258_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eDv258PostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*   DV258  Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eDv258PostProc] = BDSP_Img_Id_eDV258_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDV258_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eDv258PostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif


#ifdef 	BDSP_DDRE_SUPPORT
    /*  DDRE Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eDdrePostProc] = (uint32_t)BDSP_Img_Id_eDDRE_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDDRE_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eDdrePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eDdrePostProc] = (uint32_t)BDSP_Img_Id_eDDRE_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDDRE_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eDdrePostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*   DDRE  Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eDdrePostProc] = BDSP_Img_Id_eDDRE_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eDDRE_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eDdrePostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

#ifdef 	BDSP_SPEEXAEC_SUPPORT
    /*  SPEEX  Aec Code and Table */
    pimgIdMappingArrays->codeImgId[BDSP_AF_P_AlgoId_eSpeexAECPostProc] = (uint32_t)BDSP_Img_Id_eSpeexAec_Code;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSpeexAec_Code, &size);
    pimgIdMappingArrays->ui32CodeSize[BDSP_AF_P_AlgoId_eSpeexAECPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    pimgIdMappingArrays->tableImgId[BDSP_AF_P_AlgoId_eSpeexAECPostProc] = (uint32_t)BDSP_Img_Id_eSpeexAec_Table;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSpeexAec_Table, &size);
    pimgIdMappingArrays->tableSize[BDSP_AF_P_AlgoId_eSpeexAECPostProc] = BDSP_RAAGA_ALIGN(size, 2);

    /*   SPEEX Aec  Interframe */
    pimgIdMappingArrays->interframeImgId[BDSP_AF_P_AlgoId_eSpeexAECPostProc] = BDSP_Img_Id_eSpeexAec_Interframe;
    BDSP_Raaga_P_GetFWSize (pDeviceHandle, BDSP_Img_Id_eSpeexAec_Interframe, &size);
    pimgIdMappingArrays->interframeSize[BDSP_AF_P_AlgoId_eSpeexAECPostProc]= BDSP_RAAGA_ALIGN(size, 2);
#endif

    BDBG_LEAVE(BDSP_MapImgIdWithExecId);
    return;
}

/**********************************************************
Function: BDSP_Raaga_P_ResetHardware

Description : Reset's Raaga core and restores interrupts 
				to the state it was before the reset

**********************************************************/
uint32_t BDSP_Raaga_P_ResetHardware(
    void 		*pDeviceHandle,
    uint32_t	uiDspIndex)
{

	BDSP_Raaga_P_ResetRaagaCore(pDeviceHandle, uiDspIndex );

	BKNI_EnterCriticalSection();	
	BDSP_Raaga_P_RestoreInterrupts_isr(pDeviceHandle, uiDspIndex ); 
	BKNI_LeaveCriticalSection();	
    
	return BERR_SUCCESS;
}

/**********************************************************
Function: BDSP_Raaga_P_ResetHardware_isr

Description : To be called inside an isr-
				Reset's Raaga core and restores interrupts 
				to the state it was before the reset

**********************************************************/
uint32_t BDSP_Raaga_P_ResetHardware_isr(
    void 		*pDeviceHandle,
    uint32_t	uiDspIndex)
{

	BDSP_Raaga_P_ResetRaagaCore_isr(pDeviceHandle, uiDspIndex );
    
	BDSP_Raaga_P_RestoreInterrupts_isr(pDeviceHandle, uiDspIndex ); 

    return BERR_SUCCESS;
}

/**********************************************************
Function: BDSP_Raaga_P_ResetRaagaCore

Description : Reset's Raaga core.
				It masks watchdog interrupt too. 

**********************************************************/
BERR_Code BDSP_Raaga_P_ResetRaagaCore(
    void                *pDeviceHandle,
    uint32_t    uiDspIndex)
{

	uint32_t regVal,uiOffset;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

	
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

    /* Reset DSP processor and its peripherals */
    regVal = 0;
	uiOffset = pDevice->dspOffset[uiDspIndex]; 

#if BCHP_RAAGA_DSP_MISC_SOFT_RESET    
    regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset) ;
    regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_RESET,RESET_DMA_B)))|
        (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_RESET,RESET_DMA_B,0));
    regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_RESET,RESET_PROC_B)))|
        (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_RESET,RESET_PROC_B,0));
    regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_RESET,RESET_B)))|
        (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_RESET,RESET_B,0));
    BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset, regVal);

#elif BCHP_RAAGA_DSP_MISC_1_REG_START
	

	regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset) ;
	regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_INIT, DO_SW_INIT)))|
	 (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_INIT, DO_SW_INIT,1));
	BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset, regVal);

	regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_REVISION + uiOffset) ;
	regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_REVISION + uiOffset) ;

	BDBG_MSG(("REV ID VAL = 0x%x", regVal));

	regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset) ;
	regVal &=  ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_INIT,INIT_PROC_B));
	BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset, regVal);

	if( 0 == uiDspIndex )
	{
		/*RDB says no need of Read modify write.*/
		regVal = 0;
		regVal = (BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_SET, 	 raaga0_sw_init,1));
		BDSP_Write32(pDevice->regHandle,BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, regVal);	
	
		BKNI_Delay(2);
	
		/*RDB says no need of Read modify write.*/
		regVal = 0;
		regVal = (BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_CLEAR, raaga0_sw_init,1));
		BDSP_Write32(pDevice->regHandle,BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR, regVal);	

	}
	else if ( 1 == uiDspIndex )
	{
		/*RDB says no need of Read modify write.*/
		regVal = 0;
		regVal = (BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_SET, 	 raaga1_sw_init,1));
		BDSP_Write32(pDevice->regHandle,BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, regVal);	
		
		BKNI_Delay(2);
	
		/*RDB says no need of Read modify write.*/
		regVal = 0;
		regVal = (BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_CLEAR, raaga1_sw_init,1));
		BDSP_Write32(pDevice->regHandle,BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR, regVal);	
	}

#elif BCHP_RAAGA_DSP_MISC_SOFT_INIT    
	regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset) ;
    regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_INIT, DO_SW_INIT)))|
       (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_INIT, DO_SW_INIT,1));
    BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset, regVal);

    regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_REVISION + uiOffset) ;
    regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_REVISION + uiOffset) ;
    BDBG_MSG((" BDSP REV ID value = 0x%x", regVal));


    regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset) ;
    regVal &=  ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_INIT,INIT_PROC_B));
    BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset, regVal);

        /*RDB says no need of Read modify write.*/
    regVal = 0;
    regVal = (BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_SET, raaga_sw_init,1));
    BDSP_Write32(pDevice->regHandle,BCHP_SUN_TOP_CTRL_SW_INIT_0_SET + uiOffset, regVal);
		
	BKNI_Delay(2); 
        
	/*RDB says no need of Read modify write.*/
    regVal = 0;
    regVal = (BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_CLEAR, raaga_sw_init,1));
    BDSP_Write32(pDevice->regHandle,BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR + uiOffset, regVal);

#else
#error
#endif

return BERR_SUCCESS;
}
#if 0
BERR_Code BDSP_Raaga_P_LoadScm(
    void *pDeviceHandle,
    unsigned uiDspIndex,
	BDSP_ScmType eScmIndex)
{
    BERR_Code err = BERR_SUCCESS;

    uint32_t    size;
    uint8_t     dmaDataType, swapType;
    uint32_t    physAddress;
    uint32_t    ui32ImemAddr = 0;
    unsigned	uiOffset;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;    
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

    BDBG_ENTER(BDSP_Raaga_P_LoadScm);
    /* BSTD_UNUSED(uiScmIndex); */
    {
        /*For Multi DSP Chips, uiOffset needs to be initialized as per RDB address.*/
        uiOffset = 0*uiDspIndex;

        /* Boot SCM Specifics */
        dmaDataType = 2;
        swapType = 0;

        /* DMA SCM Task code into SRAM */
        ui32ImemAddr = (184*1024);

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Addr), &physAddress);
        size = pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Size;
        err = BDSP_DSP_P_DmaIn(pDeviceHandle, dmaDataType, swapType, size,  physAddress, ui32ImemAddr,uiOffset);


		if (eScmIndex == BDSP_ScmType_eScm1)
		{
	       /* DMA SCM1 Digest into SRAM */
	        ui32ImemAddr += (size - 20);

	        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
	            (void *)(pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Addr), &physAddress);
	        size = pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Size;
	        err = BDSP_DSP_P_DmaIn(pDeviceHandle, dmaDataType, swapType, size,  physAddress, ui32ImemAddr,uiOffset);			

		}
		else if (eScmIndex == BDSP_ScmType_eScm2)
		{
			/* DMA SCM2 Digest into SRAM */
			 ui32ImemAddr += (size -20);
			
			 BMEM_ConvertAddressToOffset(pDevice->memHandle, 
				 (void *)(pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Addr), &physAddress);
			 size = pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Size;
			 err = BDSP_DSP_P_DmaIn(pDeviceHandle, dmaDataType, swapType, size,  physAddress, ui32ImemAddr,uiOffset);

		}
		else
			BDBG_ERR(("SCM Digest not loaded as it is not required"));

    }
    BDBG_LEAVE(BDSP_Raaga_P_LoadScm);
    return err;


}
#endif
BERR_Code BDSP_Raaga_P_InitRaagaCore (void *pDeviceHandle,
					unsigned uiDspIndex)
{

	unsigned uiOffset;
	BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;    
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
	
	uiOffset = pDevice->dspOffset[uiDspIndex]; 

	BDSP_Write32(pDevice->regHandle, BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q0 + uiOffset, 
					BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q0_BURST_16JW);
	BDSP_Write32(pDevice->regHandle, BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q1 + uiOffset, 
					BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q0_BURST_16JW);
	BDSP_Write32(pDevice->regHandle, BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q2 + uiOffset, 
					BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q0_BURST_16JW);
	BDSP_Write32(pDevice->regHandle, BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q3 + uiOffset, 
					BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q0_BURST_16JW);
    
#if BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_VQ4
	BDSP_Write32(pDevice->regHandle, BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_VQ4 + uiOffset, 
					BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q0_BURST_16JW);
#endif

#if BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_VQ5
	BDSP_Write32(pDevice->regHandle, BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_VQ5 + uiOffset, 
					BCHP_RAAGA_DSP_DMA_MAX_SCB_BURST_SIZE_Q0_BURST_16JW);
#endif

return BERR_SUCCESS;
}

BERR_Code BDSP_Raaga_P_UnresetRaagaCore(void *pDeviceHandle,
					unsigned uiDspIndex)
{

	unsigned uiOffset;
	uint32_t    regVal = 0;
	BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;    
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

	uiOffset = pDevice->dspOffset[uiDspIndex]; 
    /* Withdraw processor resets */
#if BCHP_RAAGA_DSP_MISC_SOFT_RESET
        regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset);
        regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_RESET,RESET_PROC_B)))|
            (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_RESET,RESET_PROC_B,1));
        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset, regVal);


#elif BCHP_RAAGA_DSP_MISC_1_REG_START

	/*  uiOffset will decide whether dsp0 is being reset or dsp1  */
	regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset);
        regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_INIT,INIT_PROC_B)))|
            (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_INIT,INIT_PROC_B,1));
        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset, regVal);


#elif BCHP_RAAGA_DSP_MISC_SOFT_INIT   
        regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset);
        regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_INIT,INIT_PROC_B)))|
            (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_INIT,INIT_PROC_B,1));
        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_INIT + uiOffset, regVal);
#else
#error
#endif

return BERR_SUCCESS;

}

BERR_Code BDSP_Raaga_P_InitHardware(
    void *pDeviceHandle,
    unsigned uiDspIndex)
{

    BERR_Code err = BERR_SUCCESS;

    uint32_t    regVal = 0, size;
    uint8_t     dmaDataType, swapType;
    uint32_t    physAddress, retVal=0;
    uint32_t    ui32ImemAddr = 0;
    unsigned uiOffset;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;    
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

    BDBG_ENTER(BDSP_Raaga_P_InitHardware);

    {
        /*For Multi DSP Chips, uiOffset needs to be initialized as per RDB address.*/
#if defined BCHP_RAAGA_DSP_RGR_1_REG_START
		uiOffset = (BCHP_RAAGA_DSP_RGR_1_REG_START - BCHP_RAAGA_DSP_RGR_REG_START) * uiDspIndex;
#else
        uiOffset = 0*uiDspIndex;
#endif
        if (false == pDevice->settings.authenticationEnabled)
        {
            /* Reset DSP processor and its peripherals */
            regVal=BDSP_Raaga_P_ResetHardware(pDeviceHandle, uiDspIndex);

            /* Withdraw only RESET0_B reset */
#if BCHP_RAAGA_DSP_MISC_SOFT_RESET    
            regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_RESET,RESET_B)))|
                (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_RESET,RESET_B,1));

            BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset, regVal);

            regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset);
            regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_RESET,RESET_DMA_B)))|
                (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_RESET,RESET_DMA_B,1));

            BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset, regVal);
#endif
        }
        /* Initialize Mailbox5 register to zero */
        regVal = 0;
        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_PERI_SW_MAILBOX5 + uiOffset,
            BDSP_RAAGA_PREBOOT_MAILBOX_PATTERN);
        regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_PERI_SW_MAILBOX5 + uiOffset);

        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_PERI_SW_MAILBOX0 + uiOffset,
            BDSP_RAAGA_PREBOOT_MAILBOX_PATTERN);
        regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_PERI_SW_MAILBOX0 + uiOffset);


        /* Boot the processor */
        dmaDataType = 2;
        swapType = 0;

        /* DMA System code into SRAM */
        ui32ImemAddr = 0x0;    
        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Addr), &physAddress);
        size = pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Size;
        err = BDSP_DSP_P_DmaIn(pDeviceHandle, dmaDataType, swapType, size,  physAddress, ui32ImemAddr,uiOffset);

        /* DMA System data into SRAM */
        ui32ImemAddr = 0x40000000;    
        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Addr, &physAddress);
        size = pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Size;
        err = BDSP_DSP_P_DmaIn(pDeviceHandle, dmaDataType, swapType, size, physAddress, ui32ImemAddr,uiOffset);

        /* DMA RDB Vars into SRAM */
        ui32ImemAddr += size;
        BMEM_ConvertAddressToOffset(pDevice->memHandle,
            (void *)pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Addr, &physAddress);    
        size = pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Size;
        err = BDSP_DSP_P_DmaIn(pDeviceHandle, dmaDataType, swapType, size, physAddress, ui32ImemAddr,uiOffset);

        /* DMA CommonIds code into SRAM */
        ui32ImemAddr += size;
        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Addr), &physAddress);
        size = pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Size;
        err = BDSP_DSP_P_DmaIn(pDeviceHandle, dmaDataType, swapType, size,  physAddress, ui32ImemAddr,uiOffset);

	/* Both the functions return success all the time. If there is a way to cross-check we can use the retVal */	
	retVal = BDSP_Raaga_P_InitRaagaCore (pDeviceHandle, uiDspIndex);
	retVal = BDSP_Raaga_P_UnresetRaagaCore (pDeviceHandle,uiDspIndex);	
    }
    BDBG_LEAVE(BDSP_Raaga_P_InitHardware);
    return err;
}

BERR_Code BDSP_Raaga_P_Open(
    void *pDeviceHandle
    )
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned uiOffset=0, index=0,dspCfgRamSize=0,uiPhysicalAddr=0,ui32FifoPtr=0,uiDspIndex=0,i=0;    
    uint32_t    regVal =0,ui32DebugFifoOffset=0,ui32DebugType=0;
    BDSP_Status sStatus;
    BDSP_Raaga_P_MsgQueueHandle hMsgQueue;
	BDSP_Raaga_P_Command sCommand;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);


    if (pDevice->deviceWatchdogFlag == false)
    {
        if (pDevice->settings.authenticationEnabled == true)
            pDevice->settings.preloadImages = true;

        BKNI_CreateMutex(&(pDevice->taskDetails.taskIdMutex));
        BKNI_CreateMutex(&(pDevice->captureMutex));

        for (i=0 ; i < BDSP_RAAGA_MAX_DSP; i++)
        {
            for (index=0 ; index< BDSP_RAAGA_MAX_FW_TASK_PER_DSP; index++)
            {
                pDevice->taskDetails.taskId[i][index] =true;    
                pDevice->taskDetails.pRaagaTask[i][index] =NULL;
            }

            for (index=0;index<BDSP_RAAGA_NUM_FIFOS;index++)
            {
                pDevice->dspFifo[i][index] = false;
            }              

#if defined BCHP_RAAGA_DSP_RGR_1_REG_START
			pDevice->dspOffset[i] = i * (BCHP_RAAGA_DSP_RGR_1_REVISION - BCHP_RAAGA_DSP_RGR_REVISION);
#else
            pDevice->dspOffset[i] = i * 0 /*(BCHP_RAAGA_DSP1_RGR_REVISION - BCHP_RAAGA_DSP0_RGR_REVISION)*/;        
#endif
            BKNI_CreateMutex(&(pDevice->fifoIdMutex[i]));        

			/* Initialize (free) all interrupts to dsp */
			for (index=0;index<BDSP_RAAGA_MAX_INTERRUPTS_PER_DSP;index++)
			{
				pDevice->dspInterrupts[i][index] = false;
			}
            BKNI_CreateMutex(&(pDevice->dspInterruptMutex[i])); 
			/* Initialize (free) all RDB of DSP */			
			for (index=0;index<BDSP_RAAGA_RDB_REGISTERS;index++)
			{
				pDevice->rdbRegisters[i][index] = false;
			}
			BKNI_CreateMutex(&(pDevice->rdbRegistersMutex[i])); 
        }
        BKNI_CreateMutex(&(pDevice->watchdogMutex));                         
        BDSP_P_InitializeMit(&(pDevice->mit));

        BDSP_P_MapImgIdWithExecId(pDeviceHandle,&(pDevice->imgIdMappingArrays));

        ret = BDSP_Raaga_P_AllocateInitMemory (pDeviceHandle);
        if (ret != BERR_SUCCESS)
        {
            ret = BERR_TRACE(ret);        
            goto err_allocate_initmem;
        }
    }

    pDevice->device.getStatus(pDeviceHandle,&sStatus);

    /* If Fw verification is enabled, then don't download the Executable again 
    in watchdog recovery */
    if (!((pDevice->settings.authenticationEnabled == true)
        && (pDevice->deviceWatchdogFlag == true)))
    {
        ret = BDSP_Raaga_P_DownloadFwExec(pDeviceHandle);
        if (ret != BERR_SUCCESS)
        {
            ret = BERR_TRACE(BDSP_ERR_DOWNLOAD_FAILED);
            goto err_downloadfw;
        }
    }

    for (uiDspIndex =0 ; uiDspIndex < sStatus.numDsp ; uiDspIndex++)
    {
        /*For Multi DSP Chips, uiOffset needs to be initialized as per RDB address.*/
        uiOffset = pDevice->dspOffset[uiDspIndex] ;

        /* Initialize entire DSP configuration RAM to zero */
        /* Skip overwriting 17 as that is used for FW dbg buffer */
        dspCfgRamSize = BCHP_RAAGA_DSP_FW_CFG_READ_PTR_FIFO_17 - RAAGA_DSP_FW_CFG_PAGE_START;
        for (index = 0; index < dspCfgRamSize; index+=4)
        {
            BDSP_Write32(pDevice->regHandle, RAAGA_DSP_FW_CFG_PAGE_START + 
                uiOffset + index, 0x0);
        }

        BMEM_ConvertAddressToOffset(pDevice->memHandle,pDevice->memInfo.pDSPFifoAddrStruct[uiDspIndex], &uiPhysicalAddr);

        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_FW_CFG_CONTROL_REGISTER0_CXT0 + 
            uiOffset, uiPhysicalAddr + (BDSP_RAAGA_NUM_EXTRA_FIFOS*sizeof(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER)));
        /* Fill up this structure */
        uiPhysicalAddr = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + uiOffset + BCHP_PHYSICAL_OFFSET + BDSP_RAAGA_FIFO_0_INDEX*(BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1-BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0);
        ui32FifoPtr = (uint32_t)pDevice->memInfo.pDSPFifoAddrStruct[uiDspIndex];
        for (i=0;i<BDSP_RAAGA_NUM_FIFOS;i++)
        {
            BDSP_Raaga_P_MemWrite32(pDevice->memHandle,ui32FifoPtr,uiPhysicalAddr); /* base */
            ui32FifoPtr += 4;
            BDSP_Raaga_P_MemWrite32(pDevice->memHandle,ui32FifoPtr,uiPhysicalAddr + 4); /* end */
            ui32FifoPtr += 4;
            BDSP_Raaga_P_MemWrite32(pDevice->memHandle,ui32FifoPtr,uiPhysicalAddr + 12); /* read */
            ui32FifoPtr += 4;
            BDSP_Raaga_P_MemWrite32(pDevice->memHandle,ui32FifoPtr,uiPhysicalAddr + 8); /* write */
            ui32FifoPtr += 4;
            BDSP_Raaga_P_MemWrite32(pDevice->memHandle,ui32FifoPtr,uiPhysicalAddr + 4); /* wrap */
            ui32FifoPtr += 4;
            uiPhysicalAddr += 4*(BDSP_RAAGA_NUM_PTR_PER_FIFO-1);
        }

		for (ui32DebugType = 0; ui32DebugType < BDSP_Raaga_DebugType_eLast; ui32DebugType++)
		{
			ui32DebugFifoOffset = (BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0)*(BDSP_RAAGA_DEBUG_FIFO_START_INDEX + ui32DebugType);
	        BMEM_ConvertAddressToOffset(pDevice->memHandle,(void *)pDevice->memInfo.pFwDebugBuf[uiDspIndex][ui32DebugType].ui32Addr, &uiPhysicalAddr);       

			/*BDBG_ERR(("uiVirtualAddr%d = %x\n\n", ui32DebugType, pDevice->memInfo.pFwDebugBuf[uiDspIndex][ui32DebugType].ui32Addr));
			BDBG_ERR(("uiPhysicalAddr%d = %x\n\n", ui32DebugType, uiPhysicalAddr));
			BDBG_ERR(("FIFO BASE%d = %x\n\n", ui32DebugType, BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + ui32DebugFifoOffset +uiOffset));*/
	        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + ui32DebugFifoOffset +
	            uiOffset, uiPhysicalAddr);
	        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_FW_CFG_READ_PTR_FIFO_0 + ui32DebugFifoOffset +
	            uiOffset, uiPhysicalAddr);
	        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_FW_CFG_WRITE_PTR_FIFO_0 + ui32DebugFifoOffset +
	            uiOffset, uiPhysicalAddr);
	        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
	            (void *)(pDevice->memInfo.pFwDebugBuf[uiDspIndex][ui32DebugType].ui32Addr + 
	            pDevice->memInfo.pFwDebugBuf[uiDspIndex][ui32DebugType].ui32Size), &uiPhysicalAddr);
	        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_FW_CFG_END_ADDR_FIFO_0 + ui32DebugFifoOffset +
	            uiOffset, uiPhysicalAddr);
		}

        if (pDevice->deviceWatchdogFlag == true)
        {
            hMsgQueue =  pDevice->hCmdQueue[uiDspIndex];
        }
        /*Command Queue*/
        ret = BDSP_Raaga_P_CreateMsgQueue(&(pDevice->memInfo.cmdQueueParams[uiDspIndex]), pDevice->memHandle, 
            pDevice->regHandle, uiOffset,&hMsgQueue,pDevice->deviceWatchdogFlag);
        if (BERR_SUCCESS != ret)
        {
            BDBG_ERR(("BDSP_RAAGA_P_Open: Cmd queue creation failed!"));
            goto err_create_cmdqueue;

        }
        if (pDevice->deviceWatchdogFlag == false)
        {
            pDevice->hCmdQueue[uiDspIndex] = hMsgQueue;
        }

        if (pDevice->deviceWatchdogFlag == true)
        {
            hMsgQueue = pDevice->hGenRspQueue[uiDspIndex] ;
        }
        /*Generic response Queue*/
        ret = BDSP_Raaga_P_CreateMsgQueue(&(pDevice->memInfo.genRspQueueParams[uiDspIndex]), pDevice->memHandle, 
            pDevice->regHandle, uiOffset,&hMsgQueue,pDevice->deviceWatchdogFlag);
        if (BERR_SUCCESS != ret)
        {
            BDBG_ERR(("BDSP_RAAGA_P_Open: Generic Response queue creation failed!"));
            goto err_create_genqueue;
        }
        if (pDevice->deviceWatchdogFlag == false)
        {
            pDevice->hGenRspQueue[uiDspIndex] = hMsgQueue;
        }

        if ((pDevice->settings.authenticationEnabled==true) 
            && (pDevice->deviceWatchdogFlag == false))
        {
            /* Reset DSP processor and its peripherals */
            regVal = BDSP_Raaga_P_ResetHardware(pDeviceHandle, uiDspIndex);
#if BCHP_RAAGA_DSP_MISC_SOFT_RESET        
            /* Withdraw only RESET_B reset */
            regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_RESET,RESET_B)))|
                (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_RESET,RESET_B,1));
            BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset, regVal);
            BDBG_MSG(("\n**** Soft reset: reset0_b withdrawn = 0x%x",regVal));

            regVal = BDSP_Read32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset);
            regVal = (regVal & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_RESET,RESET_DMA_B)))|
                (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_RESET,RESET_DMA_B,1));
            BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_MISC_SOFT_RESET + uiOffset, regVal);
            BDBG_MSG(("\n**** Soft reset: SCBDMA withdrawn = 0x%x ****",regVal));
#endif
        }
        else
        {
            ret = BDSP_Raaga_P_InitHardware(pDeviceHandle,uiDspIndex);
            if (ret!=BERR_SUCCESS)
            {
                BDBG_MSG(("BDSP_Raaga_P_InitHardware::Error returned %x!",ret));
                ret= BERR_TRACE(ret);
                goto err_init_hardware;
            }
        }

        /* Program command queue & generic response queue */
        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_FW_CFG_CONTROL_REGISTER1_CXT0 + 
            uiOffset, BDSP_RAAGA_FIFO_CMD);
        BDSP_Write32(pDevice->regHandle,BCHP_RAAGA_DSP_FW_CFG_CONTROL_REGISTER2_CXT0 + 
            uiOffset, BDSP_RAAGA_FIFO_GENERIC_RSP);

        if (pDevice->deviceWatchdogFlag == false)
        {
            for (i=0;i<BDSP_RAAGA_NUM_FIFOS;i++)
            {
                pDevice->dspFifo[uiDspIndex][i] = false;
            }            
            pDevice->dspFifo[uiDspIndex][BDSP_RAAGA_FIFO_CMD-BDSP_RAAGA_FIFO_0_INDEX] = true;
            pDevice->dspFifo[uiDspIndex][BDSP_RAAGA_FIFO_GENERIC_RSP-BDSP_RAAGA_FIFO_0_INDEX] = true;
            pDevice->dspFifo[uiDspIndex][BDSP_RAAGA_FIFO_DEBUG] = true;
        }

        ret = BDSP_Raaga_P_AckInstall(pDeviceHandle);
        if (ret!=BERR_SUCCESS)
        {
            BDBG_MSG(("BDSP_Raaga_P_InitHardware::Error returned %x!",ret));
            ret= BERR_TRACE(ret);
            goto err_ack_install;
        }
		/* Need to send a command to send the Swap Memory Address */
		sCommand.sCommandHeader.ui32CommandID			= BDSP_RAAGA_GET_SYSTEM_SWAP_MEMORY_COMMAND_ID;
		sCommand.sCommandHeader.ui32CommandCounter		= 0;
		sCommand.sCommandHeader.ui32TaskID				= 0;
		sCommand.sCommandHeader.eResponseType			= BDSP_Raaga_P_ResponseType_eNone;
		sCommand.sCommandHeader.ui32CommandSizeInBytes	=  sizeof(BDSP_Raaga_P_Command);

	    BMEM_ConvertAddressToOffset(pDevice->memHandle,(void *)pDevice->memInfo.sRaagaSwapMemoryBuf[uiDspIndex].ui32Addr, &uiPhysicalAddr);
		sCommand.uCommand.sSystemSwapMemCommand.ui32SystemSwapDramMemAddr			= uiPhysicalAddr;
		
		BDBG_MSG(("System Swap memory DRAM address is 0x%x", uiPhysicalAddr));
		ret = BDSP_Raaga_P_SendCommand(
						pDevice->hCmdQueue[uiDspIndex], &sCommand,(void *)NULL);

    }    

    goto open_success;

    err_ack_install:
	for(uiDspIndex = 0; uiDspIndex < BDSP_RAAGA_MAX_DSP; uiDspIndex++)
	{
    	BDSP_Raaga_P_ResetHardware(pDeviceHandle, uiDspIndex);
	}

    err_init_hardware:
    for (uiDspIndex =0 ; uiDspIndex < sStatus.numDsp ; uiDspIndex++)
    {
        BDSP_Raaga_P_DestroyMsgQueue(pDevice->hGenRspQueue[uiDspIndex],
            pDevice->regHandle, pDevice->dspOffset[uiDspIndex]);
    }


    err_create_genqueue:


    for (uiDspIndex =0 ; uiDspIndex < sStatus.numDsp ; uiDspIndex++)
    {
        BDSP_Raaga_P_DestroyMsgQueue(pDevice->hCmdQueue[uiDspIndex], 
            pDevice->regHandle, pDevice->dspOffset[uiDspIndex]);
    }

    err_create_cmdqueue:    
    err_downloadfw:

    BDSP_Raaga_P_FreeInitMemory(pDeviceHandle);

    err_allocate_initmem:

	if (pDevice->deviceWatchdogFlag == false)
	{
	    BKNI_DestroyMutex(pDevice->taskDetails.taskIdMutex);
	    BKNI_DestroyMutex(pDevice->captureMutex);
	    for (i=0 ; i < BDSP_RAAGA_MAX_DSP; i++)
	    {
	        BKNI_DestroyMutex(pDevice->fifoIdMutex[i]);
			BKNI_DestroyMutex(pDevice->dspInterruptMutex[i]);
			BKNI_DestroyMutex(pDevice->rdbRegistersMutex[i]);
	    }
	    BKNI_DestroyMutex(pDevice->watchdogMutex);    
	}


    open_success:
    return ret; 
}

void BDSP_Raaga_P_UpdateMitEntry(
    void        *pDeviceHandle,
    BDSP_AF_P_AlgoId    eExecId,
    uint32_t        codePtr,
    uint32_t        ui32CodeSize,
    uint32_t        tblPtr,     
    uint32_t        tblSize             
    )
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_ENTER(BDSP_Raaga_P_UpdateMitEntry);
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    

    pDevice->mit.sAlgoDwnldDetail[eExecId].ui32AlgoCodePtr = codePtr;
    pDevice->mit.sAlgoDwnldDetail[eExecId].ui32Algoui32CodeSize= ui32CodeSize;
    pDevice->mit.sAlgoDwnldDetail[eExecId].ui32AlgoDataPtr = tblPtr;
    pDevice->mit.sAlgoDwnldDetail[eExecId].ui32AlgoDataSize= tblSize;

    BDBG_LEAVE(BDSP_Raaga_P_UpdateMitEntry);
    return;
}


BERR_Code BDSP_Raaga_P_GetStageSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    void *pSettingsBuffer, 
    size_t settingsSize)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;
    BDSP_CIT_P_OpStgInfo    *pStageInfo = NULL;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettingsBuffer);

    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }
        else
        {
            pStageInfo = (BDSP_CIT_P_OpStgInfo    *)BKNI_Malloc(sizeof(BDSP_CIT_P_OpStgInfo));
            if (pStageInfo == NULL)
            {
                BDBG_ERR(("Out of System memory"));
                err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                goto end;
            }
            BKNI_Memset(pStageInfo,0,sizeof(BDSP_CIT_P_OpStgInfo));

            *pStageInfo = pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId];
        }
        switch (pStageInfo->eStageType)
        {
        case BDSP_CIT_P_StageType_eDecode:
            BDSP_Raaga_P_GetAudioDecodeStageSettings
                (pRaagaTask->pContext->pDevice->memHandle,
                pStageInfo->uAlgorithm.eDecAudioAlgo
                ,pStageInfo->uAudioMode.eDecAudioMode
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize
                ,pSettingsBuffer
                ,settingsSize);
            break;
        case BDSP_CIT_P_StageType_eEncode:
            BDSP_Raaga_P_GetAudioEncodeStageSettings
                (pRaagaTask->pContext->pDevice->memHandle,
                pStageInfo->uAlgorithm.eEncAudioAlgo
                ,pStageInfo->uAudioMode.eEncAudioMode
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize
                ,pSettingsBuffer
                ,settingsSize);                                
            break;
        case BDSP_CIT_P_StageType_ePostProc:
            BDSP_Raaga_P_GetAudioProcessingStageSettings
                (pRaagaTask->pContext->pDevice->memHandle,
                pStageInfo->uAlgorithm.eProcAudioAlgo
                ,pStageInfo->uAudioMode.eProcessingAudioMode
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize
                ,pSettingsBuffer
                ,settingsSize);                
            break;
        default:
            BDBG_ERR(("Invalid Stage Type %d",pStageInfo->eStageType));
            BDBG_ASSERT(0);
            break;                
        }
    }
    end:
    if (pStageInfo)
        BKNI_Free(pStageInfo);
    return err;
}

BERR_Code BDSP_Raaga_P_GetVideoStageSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    void *pSettingsBuffer, 
    size_t settingsSize)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;
    BDSP_CIT_P_OpStgInfo    *pStageInfo = NULL;
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettingsBuffer);

    if (pRaagaTask->videoCitOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->videoCitOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;
    }
    else
    {
        if (pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }
        else
        {
            pStageInfo = (BDSP_CIT_P_OpStgInfo    *)BKNI_Malloc(sizeof(BDSP_CIT_P_OpStgInfo));
            if (pStageInfo == NULL)
            {
                BDBG_ERR(("Out of System memory"));
                err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                goto end;
            }
            BKNI_Memset(pStageInfo,0,sizeof(BDSP_CIT_P_OpStgInfo));

            *pStageInfo = pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId];
        }
        switch (pStageInfo->eStageType)
        {
        case BDSP_CIT_P_StageType_eVideoDecode:
            BDSP_Raaga_P_GetVideoDecodeStageSettings
                (pRaagaTask->pContext->pDevice->memHandle,
                pStageInfo->uAlgorithm.eVideoAlgo
                ,pStageInfo->uAudioMode.eDecAudioMode
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize
                ,pSettingsBuffer
                ,settingsSize);
            break;
        case BDSP_CIT_P_StageType_eVideoEncode:
            BDSP_Raaga_P_GetVideoEncodeStageSettings
                (pRaagaTask->pContext->pDevice->memHandle,
                pStageInfo->uAlgorithm.eVideoEncAlgo
                ,pStageInfo->uAudioMode.eEncAudioMode
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize
                ,pSettingsBuffer
                ,settingsSize);                                
            break;
        default:
            BDBG_ERR(("Invalid Stage Type %d",pStageInfo->eStageType));
            BDBG_ASSERT(0);
            break;                
        }
    }
    end:
    if (pStageInfo)
        BKNI_Free(pStageInfo);
    return err;
}

BERR_Code BDSP_Raaga_P_SetStageSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const void *pSettingsBuffer, 
    size_t settingsSize)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    BDSP_CIT_P_OpStgInfo    *pStageInfo = NULL;
    unsigned int            uiConfigBufAddr =0;    
    unsigned int            uiConfigBufSize;      
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;   
    BDSP_Raaga_P_FwBuffer  *psFwBufInfo;
    BDSP_Raaga_P_MsgType      eMsgType;


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettingsBuffer);


    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }
        else
        {
            pStageInfo = (BDSP_CIT_P_OpStgInfo    *)BKNI_Malloc(sizeof(BDSP_CIT_P_OpStgInfo));
            if (pStageInfo == NULL)
            {
                BDBG_ERR(("Out of System memory"));
                err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                goto end;
            }
            BKNI_Memset(pStageInfo,0,sizeof(BDSP_CIT_P_OpStgInfo));

            *pStageInfo = pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId];
        }
        switch (pStageInfo->eStageType)
        {
        case BDSP_CIT_P_StageType_eDecode:        
            if (pRaagaTask->isStopped == false)
            {
                uiConfigBufAddr = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32BaseAddr;
                uiConfigBufSize = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32Size;                    
            }
            else
            {
                uiConfigBufAddr = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize;  
            }
            BDSP_Raaga_P_SetAudioDecodeStageSettings(
                pDevice->memHandle,
                pStageInfo->uAlgorithm.eDecAudioAlgo,
                pStageInfo->uAudioMode.eDecAudioMode,
                uiConfigBufAddr,
                uiConfigBufSize,
                pSettingsBuffer,
                settingsSize);
            break;
        case BDSP_CIT_P_StageType_eEncode:
            if (pRaagaTask->isStopped == false)
            {
                uiConfigBufAddr = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32BaseAddr;
                uiConfigBufSize = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32Size;                    
            }
            else
            {
                uiConfigBufAddr = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize;  
            }                
            BDSP_Raaga_P_SetAudioEncodeStageSettings
                (pDevice->memHandle,
                pStageInfo->uAlgorithm.eEncAudioAlgo
                ,pStageInfo->uAudioMode.eEncAudioMode
                ,uiConfigBufAddr
                ,uiConfigBufSize
                ,pSettingsBuffer
                ,settingsSize);                
            break;
        case BDSP_CIT_P_StageType_ePostProc:
            if (pRaagaTask->isStopped == false)
            {
                uiConfigBufAddr = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32BaseAddr;
                uiConfigBufSize = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32Size;                    
            }
            else
            {
                uiConfigBufAddr = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize;  
            }                
            BDSP_Raaga_P_SetAudioProcessingStageSettings
                (pDevice->memHandle,
                pStageInfo->uAlgorithm.eProcAudioAlgo
                ,pStageInfo->uAudioMode.eProcessingAudioMode
                ,uiConfigBufAddr
                ,uiConfigBufSize
                ,pSettingsBuffer
                ,settingsSize);                
            break;
        default:
            BDBG_ERR(("Invalid Stage Type %d",pStageInfo->eStageType));
            BDBG_ASSERT(0);
            break;                
        }

        if (pRaagaTask->isStopped == false)
        {
            /*Send CFG Change Command to FW*/
            psFwBufInfo = &(pRaagaContext->contextMemInfo.sConfigParamBufInfo);            
            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(psFwBufInfo ->ui32BaseAddr), 
                &(sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr));   
            BMEM_ConvertAddressToOffset(pDevice->memHandle, 
                (void *)(pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr), 
                &(sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr));    
            sCommand.uCommand.sCfgChange.ui32SizeInBytes = settingsSize;

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;
            BKNI_ResetEvent(pRaagaTask->hEvent);
            err = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command failed! -- Interrupts to be tested"));
                err = BERR_TRACE(err);           
                goto end;
            }

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_ALGO_PARAMS_CFG_RESPONSE_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {

                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                    sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;
            }
        }
    }
    end:
    if (pStageInfo)
        BKNI_Free(pStageInfo);
    return err;
}


BERR_Code BDSP_Raaga_P_SetVideoStageSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const void *pSettingsBuffer, 
    size_t settingsSize)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    BDSP_CIT_P_OpStgInfo    *pStageInfo = NULL;
    unsigned int            uiConfigBufAddr =0;    
    unsigned int            uiConfigBufSize;      
    BDSP_Raaga_P_Command 	sCommand;
    BDSP_Raaga_P_Response 	sRsp;   
    BDSP_Raaga_P_FwBuffer   *psFwBufInfo;
    BDSP_Raaga_P_MsgType    eMsgType;


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettingsBuffer);


    if (pRaagaTask->videoCitOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->videoCitOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;        
    }
    else
    {
        if (pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }
        else
        {
            pStageInfo = (BDSP_CIT_P_OpStgInfo    *)BKNI_Malloc(sizeof(BDSP_CIT_P_OpStgInfo));
            if (pStageInfo == NULL)
            {
                BDBG_ERR(("Out of System memory"));
                err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                goto end;
            }
            BKNI_Memset(pStageInfo,0,sizeof(BDSP_CIT_P_OpStgInfo));

            *pStageInfo = pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId];
        }
        switch (pStageInfo->eStageType)
        {
        case BDSP_CIT_P_StageType_eVideoDecode:        
            if (pRaagaTask->isStopped == false)
            {
                uiConfigBufAddr = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32BaseAddr;
                uiConfigBufSize = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32Size;                    
            }
            else
            {
                uiConfigBufAddr = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize;  
            }
            BDSP_Raaga_P_SetVideoDecodeStageSettings
                (pDevice->memHandle,
                pStageInfo->uAlgorithm.eDecAudioAlgo
                ,pStageInfo->uAudioMode.eDecAudioMode
                ,uiConfigBufAddr
                ,uiConfigBufSize
                ,pSettingsBuffer
                ,settingsSize);
            break;
        case BDSP_CIT_P_StageType_eVideoEncode:
            if (pRaagaTask->isStopped == false)
            {
                uiConfigBufAddr = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32BaseAddr;
                uiConfigBufSize = pRaagaContext->contextMemInfo.sConfigParamBufInfo.ui32Size;                    
            }
            else
            {
                uiConfigBufAddr = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr;
                uiConfigBufSize = pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufSize;  
            }                
            BDSP_Raaga_P_SetVideoEncodeStageSettings
                (pDevice->memHandle,
                pStageInfo->uAlgorithm.eEncAudioAlgo
                ,pStageInfo->uAudioMode.eEncAudioMode
                ,uiConfigBufAddr
                ,uiConfigBufSize
                ,pSettingsBuffer
                ,settingsSize);                
            break;
        default:
            BDBG_ERR(("Invalid Stage Type %d",pStageInfo->eStageType));
            BDBG_ASSERT(0);
            break;                
        }

        if (pRaagaTask->isStopped == false)
        {
            /*Send CFG Change Command to FW*/
            psFwBufInfo = &(pRaagaContext->contextMemInfo.sConfigParamBufInfo);            
            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(psFwBufInfo ->ui32BaseAddr), 
                &(sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr));   
            BMEM_ConvertAddressToOffset(pDevice->memHandle, 
                (void *)(pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32UserParamBufAdr), 
                &(sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr));    
            sCommand.uCommand.sCfgChange.ui32SizeInBytes = settingsSize;

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;
            BKNI_ResetEvent(pRaagaTask->hEvent);
            err = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command failed! -- Interrupts to be tested"));
                err = BERR_TRACE(err);           
                goto end;
            }

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_ALGO_PARAMS_CFG_RESPONSE_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {

                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                    sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;
            }
        }
    }
    end:
    if (pStageInfo)
        BKNI_Free(pStageInfo);
    return err;
}

BERR_Code BDSP_Raaga_P_AddStageInput(
    void *pTaskHandle, 
    const BDSP_TaskAddStageInputSettings *pSettings,
    BDSP_TaskStageInputHandle *pHandle)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BDSP_RaagaContext *pRaagaContext;
    BDSP_Raaga  *pDevice;
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;   
    BDSP_Raaga_P_MsgType      eMsgType;
    BDSP_SIPS_InputInfo     *psSipsIp = NULL;
    BDSP_SIPS_OutputInfo    sSipsOp;
    void *pCached;
    unsigned branchId = 0;
    unsigned stageId =0;

    BDSP_TaskStageInput *stageInput=NULL;
    BDSP_RaagaTaskStageInput *raagaStageInput = NULL;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pHandle);

    pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    pDevice= pRaagaContext->pDevice;

    branchId = pSettings->branchId;
    stageId = pSettings->stageId;

    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }

        psSipsIp = (BDSP_SIPS_InputInfo *)BKNI_Malloc(sizeof(BDSP_SIPS_InputInfo));
        if (NULL==psSipsIp)
        {
            BDBG_ERR(("Out of System memory"));
            err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto end;
        }
        BKNI_Memset(psSipsIp, 0, sizeof(BDSP_SIPS_InputInfo));

        raagaStageInput = (BDSP_RaagaTaskStageInput *)BKNI_Malloc(sizeof(BDSP_RaagaTaskStageInput));
        if ( NULL == raagaStageInput )
        {
            BDBG_ERR(("Out of System memory"));
            err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto end;
        }
        BKNI_Memset(raagaStageInput, 0, sizeof(BDSP_RaagaTaskStageInput));

        BDBG_OBJECT_SET(raagaStageInput, BDSP_RaagaTaskStageInput);


        raagaStageInput->branchId = branchId;
        raagaStageInput->stageId = stageId;
        raagaStageInput->inputType = pSettings->sourceDetails.eType;        

        BDBG_OBJECT_SET(&(raagaStageInput->stageInput), BDSP_TaskStageInput);

        raagaStageInput->stageInput.pStageInputHandle= (void *)raagaStageInput;        
        raagaStageInput->stageInput.task= &(pRaagaTask->task);

        stageInput = &(raagaStageInput->stageInput);

        BKNI_Memset(&sRsp,0,sizeof(BDSP_Raaga_P_Response));
        BKNI_Memset(&sCommand,0,sizeof(BDSP_Raaga_P_Command));        

        psSipsIp->ui32CitDataStructureDramAddr = pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr;
        psSipsIp->sRdbCfg.ui32RdbBaseOffset = 0;
        psSipsIp->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;
        psSipsIp->eAddRemoveIpPort = BDSP_SIPS_P_AddRemoveIpPort_eAddPort;
        psSipsIp->sInputSrcDetails.eType = pSettings->sourceDetails.eType;

        if (pSettings->sourceDetails.eType == BDSP_CIT_P_FwStgSrcDstType_eFMMBuf)
        {
            psSipsIp->sInputSrcDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId
                = pSettings->sourceDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId;

            raagaStageInput->pIoBuffer = (BDSP_AF_P_sIO_BUFFER *)BMEM_AllocAligned(pDevice->memHandle, sizeof(BDSP_AF_P_sIO_BUFFER) ,5, 0);    /* 32 bit aligned*/
            if ( NULL == (void *)raagaStageInput->pIoBuffer)
            {
                BDBG_ERR(("Unable to Allocate memory for task IO Buffer handle !"));
                err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto end;
            }
            (void)BMEM_ConvertAddressToCached(pDevice->memHandle, raagaStageInput->pIoBuffer, &pCached);
            BKNI_Memset((BDSP_AF_P_sIO_BUFFER *)pCached, 0, sizeof(BDSP_AF_P_sIO_BUFFER));
            (void)BMEM_FlushCache(pDevice->memHandle, pCached, sizeof(BDSP_AF_P_sIO_BUFFER));
            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(raagaStageInput->pIoBuffer), 
                &(psSipsIp->sInputSrcDetails.sIoBuffAddrs.ui32IoBuffCfgAddr));
#if 0            
            psSipsIp->sInputSrcDetails.sIoBuffAddrs.ui32IoBuffCfgAddr
                = (uint32_t)raagaStageInput->pIoBuffer;
#endif


            raagaStageInput->pIoGenericBuffer = (BDSP_AF_P_sIO_GENERIC_BUFFER *)BMEM_AllocAligned(pDevice->memHandle, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER) ,5, 0);    /* 32 bit aligned*/
            if ( NULL == (void *)raagaStageInput->pIoGenericBuffer)
            {
                BDBG_ERR(("Unable to Allocate memory for task IO generic Buffer handle !"));
                err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto end;                
            }
            (void)BMEM_ConvertAddressToCached(pDevice->memHandle, raagaStageInput->pIoGenericBuffer, &pCached);
            BKNI_Memset((BDSP_AF_P_sIO_GENERIC_BUFFER *)pCached, 0, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER));    
            (void)BMEM_FlushCache(pDevice->memHandle, &pCached, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER));

            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(raagaStageInput->pIoGenericBuffer), 
                &(psSipsIp->sInputSrcDetails.sIoBuffAddrs.ui32IoGenericBuffCfgAddr));   
#if 0            
            psSipsIp->sInputSrcDetails.sIoBuffAddrs.ui32IoGenericBuffCfgAddr
                = (uint32_t)raagaStageInput->pIoGenericBuffer;
#endif
        }
        else if (pSettings->sourceDetails.eType == BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf)
        {
            psSipsIp->sInputSrcDetails.sIoBuffAddrs.ui32IoBuffCfgAddr
                = pSettings->sourceDetails.uDetails.sInterTaskDramBuffCfg.ui32IoBuffCfgAddr;

            psSipsIp->sInputSrcDetails.sIoBuffAddrs.ui32IoGenericBuffCfgAddr
                = pSettings->sourceDetails.uDetails.sInterTaskDramBuffCfg.ui32IoGenericBuffCfgAddr;
        }

        psSipsIp->ui32WorkingCitDataStructureDramAddr = (uint32_t)BMEM_AllocAligned(pDevice->memHandle, sizeof(BDSP_AF_P_sTASK_CONFIG) ,5, 0);    /* 32 bit aligned*/
        if ( NULL == (void *)psSipsIp->ui32WorkingCitDataStructureDramAddr)
        {
            BDBG_ERR(("Unable to Allocate memory for ui32WorkingCitDataStructureDramAddr CIT output !"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;            
        }
        (void)BMEM_ConvertAddressToCached(pDevice->memHandle, (void *)psSipsIp->ui32WorkingCitDataStructureDramAddr, &pCached);
        BKNI_Memset(pCached, 0, sizeof(BDSP_AF_P_sTASK_CONFIG));                                
        BMEM_FlushCache(pDevice->memHandle, pCached, sizeof(BDSP_AF_P_sTASK_CONFIG));

        err = BDSP_SIPS_P_SeamlessIpPortSwitchAPI(pDevice->memHandle, psSipsIp, &sSipsOp);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_SIPS_P_SeamlessIpPortSwitchAPI returned error!"));
            err =BERR_TRACE(err);
            goto end;
        }

        raagaStageInput->inputIndex = sSipsOp.ui32InputPortIndexAdded;

        /*	Analyze the Reconfigured CIT : First level Information */
        BDSP_P_AnalyseCit(pDevice->memHandle, (BDSP_AF_P_sTASK_CONFIG *)pCached);        

        /*Prepare command to stop the task */
        sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_RECONFIGURATION_COMMAND_ID;
        sCommand.sCommandHeader.ui32CommandCounter = 0;
        sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
        sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
        sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

        BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(psSipsIp->ui32WorkingCitDataStructureDramAddr), 
            &(sCommand.uCommand.sCitReconfigCommand.ui32ModifiedCitAddr));

        BMEM_ConvertAddressToOffset(pDevice->memHandle, 
            (void *)(pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr), 
            &(sCommand.uCommand.sCitReconfigCommand.ui32RunningTaskCitAddr));   

        sCommand.uCommand.sCitReconfigCommand.ui32SizeInBytes = 
            pRaagaTask->taskMemGrants.sCitStruct.ui32Size;                           

        pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;
        BKNI_ResetEvent(pRaagaTask->hEvent);
        err = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_AddStageInput: CFG_Command failed!"));
            err = BERR_TRACE(err);
            goto end;
        }
        /* Wait for Ack_Response_Received event w/ timeout */
        err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
        if (BERR_TIMEOUT == err)
        {
            BDBG_ERR(("BDSP_Raaga_P_AddStageInput: CFG_Command failed! -- Interrupts to be tested"));
            err = BERR_TRACE(err);           
            goto end;
        }

        eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
        err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_Raaga_P_AddStageInput: Unable to read ACK!"));
            err = BERR_TRACE(err);
            goto end;
        }

        if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_RECONFIGURATION_RESPONSE_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
        {

            BDBG_ERR(("BDSP_Raaga_P_AddStageInput: CFG_Command ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
            err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            goto end;
        }
    }

    *pHandle = stageInput;

    end:
    if (psSipsIp)
    {
        if ((psSipsIp->ui32WorkingCitDataStructureDramAddr != (uint32_t)NULL))
        {
            BDBG_MSG(("Freeing psSipsIp->ui32WorkingCitDataStructureDramAddr = 0x%x",psSipsIp->ui32WorkingCitDataStructureDramAddr));
            BMEM_Free(pDevice->memHandle, (void *)psSipsIp->ui32WorkingCitDataStructureDramAddr);        
        }

        BKNI_Free(psSipsIp);
    }

    if (err !=BERR_SUCCESS)
    {
        if (raagaStageInput)
        {
            if (pSettings->sourceDetails.eType == BDSP_CIT_P_FwStgSrcDstType_eFMMBuf)
            {
                if (raagaStageInput->pIoBuffer)
                    BMEM_Free(pDevice->memHandle, (void *)raagaStageInput->pIoBuffer);
                if (raagaStageInput->pIoGenericBuffer)
                    BMEM_Free(pDevice->memHandle, (void *)raagaStageInput->pIoGenericBuffer);
            }

            BDBG_OBJECT_DESTROY(&(raagaStageInput->stageInput), BDSP_TaskStageInput);
            BDBG_OBJECT_DESTROY(raagaStageInput, BDSP_RaagaTaskStageInput);        

            BKNI_Free(raagaStageInput);
        }

        *pHandle = NULL;
    }

    return err;
}

BERR_Code BDSP_Raaga_P_GetStageInputIndex(
    void *pStageInputHandle,
    unsigned *inputIndex)
{
    BDSP_RaagaTaskStageInput *raagaStageInput = (BDSP_RaagaTaskStageInput *)pStageInputHandle;
    BERR_Code err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(raagaStageInput, BDSP_RaagaTaskStageInput);

    *inputIndex = raagaStageInput->inputIndex;

    return err;
}    

BERR_Code BDSP_Raaga_P_RemoveStageInput(void *pStageInputHandle)
{
    BDSP_RaagaTask *pRaagaTask = NULL;  
    BDSP_RaagaContext *pRaagaContext = NULL;
    BDSP_Raaga  *pDevice= NULL;
    BDSP_RaagaTaskStageInput *raagaStageInput = (BDSP_RaagaTaskStageInput *)pStageInputHandle;
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;   
    BDSP_Raaga_P_MsgType      eMsgType;
    BDSP_SIPS_InputInfo     *psSipsIp = NULL;
    BDSP_SIPS_OutputInfo    sSipsOp;   
    unsigned branchId,stageId;
    void *pCached;

    BDBG_ASSERT(raagaStageInput);
    BDBG_OBJECT_ASSERT(raagaStageInput, BDSP_RaagaTaskStageInput);

    pRaagaTask = (BDSP_RaagaTask *) raagaStageInput->stageInput.task->pTaskHandle;
    pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    pDevice= pRaagaContext->pDevice;
    branchId = raagaStageInput->branchId;
    stageId = raagaStageInput->stageId;


    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }

        psSipsIp = (BDSP_SIPS_InputInfo *)BKNI_Malloc(sizeof(BDSP_SIPS_InputInfo));
        if (NULL==psSipsIp)
        {
            BDBG_ERR(("Out of System memory"));
            err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto end;
        }
        BKNI_Memset(psSipsIp,0,sizeof(BDSP_SIPS_InputInfo));

        psSipsIp->sRdbCfg.ui32RdbBaseOffset = 0;
        psSipsIp->sRdbCfg.ui32RdbDspOffset = BCHP_PHYSICAL_OFFSET;
        psSipsIp->eAddRemoveIpPort = BDSP_SIPS_P_AddRemoveIpPort_eRemovePort;

        psSipsIp->ui32InputPortIndexToRemove = raagaStageInput->inputIndex;
        psSipsIp->ui32CitDataStructureDramAddr = pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr;



        psSipsIp->ui32WorkingCitDataStructureDramAddr = (uint32_t)BMEM_AllocAligned(pDevice->memHandle, sizeof(BDSP_AF_P_sTASK_CONFIG) ,5, 0);    /* 32 bit aligned*/
        if ( NULL == (void *)psSipsIp->ui32WorkingCitDataStructureDramAddr)
        {
            BDBG_ERR(("Unable to Allocate memory for ui32WorkingCitDataStructureDramAddr CIT output !"));
            err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
        }
        (void)BMEM_ConvertAddressToCached(pDevice->memHandle, (void *)psSipsIp->ui32WorkingCitDataStructureDramAddr, &pCached);
        BKNI_Memset(pCached, 0, sizeof(BDSP_AF_P_sTASK_CONFIG));                                
        BMEM_FlushCache(pDevice->memHandle, pCached, sizeof(BDSP_AF_P_sTASK_CONFIG));

        err = BDSP_SIPS_P_SeamlessIpPortSwitchAPI(pDevice->memHandle, psSipsIp, &sSipsOp);
        if (BERR_SUCCESS != err)
        {
            BDBG_ERR(("BDSP_SIPS_P_SeamlessIpPortSwitchAPI returned error!"));
            err =BERR_TRACE(err);
            goto end;
        }

        /*	Analyze the Reconfigured CIT : First level Information */
        BDSP_P_AnalyseCit(pDevice->memHandle, (BDSP_AF_P_sTASK_CONFIG *)pCached);        

        if (pRaagaContext->contextWatchdogFlag== false)
        {
            /*Prepare command to stop the task */
            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_RECONFIGURATION_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter = 0;
            sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(psSipsIp->ui32WorkingCitDataStructureDramAddr), 
                &(sCommand.uCommand.sCitReconfigCommand.ui32ModifiedCitAddr));

            BMEM_ConvertAddressToOffset(pDevice->memHandle, 
                (void *)(pRaagaTask->taskMemGrants.sCitStruct.ui32BaseAddr), 
                &(sCommand.uCommand.sCitReconfigCommand.ui32RunningTaskCitAddr));   

            sCommand.uCommand.sCitReconfigCommand.ui32SizeInBytes = 
                pRaagaTask->taskMemGrants.sCitStruct.ui32Size;                           

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;
            BKNI_ResetEvent(pRaagaTask->hEvent);
            err = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_RemoveStageInput: CFG_Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_RemoveStageInput: CFG_Command failed! -- Interrupts to be tested"));
                err = BERR_TRACE(err);           
                goto end;
            }

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;    
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_RemoveStageInput: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_RECONFIGURATION_RESPONSE_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {

                BDBG_ERR(("BDSP_Raaga_P_RemoveStageInput: CFG_Command ACK not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
                    sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;
            }
        }
    }

    if (raagaStageInput->inputType== BDSP_CIT_P_FwStgSrcDstType_eFMMBuf)
    {
        if ((raagaStageInput->pIoBuffer != NULL))
        {
            BDBG_MSG(("Freeing pRaagaTask->pIoBuffer = 0x%x",raagaStageInput->pIoBuffer));
            err = BMEM_Free(pDevice->memHandle, (void *)raagaStageInput->pIoBuffer);  
        }
        if (raagaStageInput->pIoGenericBuffer!= NULL)
        {
            BDBG_MSG(("Freeing pRaagaTask->pIoGenericBuffer = 0x%x",raagaStageInput->pIoGenericBuffer));
            err = BMEM_Free(pDevice->memHandle, (void *)raagaStageInput->pIoGenericBuffer);                        
        }
    }

    BDBG_OBJECT_DESTROY(&(raagaStageInput->stageInput), BDSP_TaskStageInput);
    BDBG_OBJECT_DESTROY(raagaStageInput, BDSP_RaagaTaskStageInput);

    if (raagaStageInput)
        BKNI_Free(raagaStageInput);


    end:
    if (psSipsIp)
    {
        if ((psSipsIp->ui32WorkingCitDataStructureDramAddr != (uint32_t)NULL))
        {
            BDBG_MSG(("Freeing psSipsIp->ui32WorkingCitDataStructureDramAddr = 0x%x",psSipsIp->ui32WorkingCitDataStructureDramAddr));
            err = BMEM_Free(pDevice->memHandle, (void *)psSipsIp->ui32WorkingCitDataStructureDramAddr);        
        }

        BKNI_Free(psSipsIp);
    }

    return err;
}


BERR_Code BDSP_Raaga_P_GetDatasyncSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskDatasyncSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;    
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);

    BKNI_EnterCriticalSection();
    err =BDSP_Raaga_P_GetDatasyncSettings_isr(pTaskHandle,branchId,stageId,pSettings);    
    BKNI_LeaveCriticalSection();
    return err;
}

BERR_Code BDSP_Raaga_P_SetDatasyncSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_AudioTaskDatasyncSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;    
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);    
    BDBG_ASSERT(pSettings);

    BKNI_EnterCriticalSection();
    err =BDSP_Raaga_P_SetDatasyncSettings_isr(pTaskHandle,branchId,stageId,pSettings);    
    BKNI_LeaveCriticalSection();
    return err;
}



BERR_Code BDSP_Raaga_P_GetDatasyncSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskDatasyncSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_Audio_FrameSyncTsmConfigParams   sFrameSyncTsmConfigParams; 

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);

    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;            
        }

        BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams_isr
            (pRaagaTask->pContext->pDevice->memHandle,
            pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr
            ,pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufSize
            ,(void *)&sFrameSyncTsmConfigParams);

        BKNI_Memcpy((void *)(volatile void*)pSettings
            ,(void *)&(sFrameSyncTsmConfigParams.sFrameSyncConfigParams),sizeof(BDSP_AudioTaskDatasyncSettings));
    }
    end:
    return err;
}

BERR_Code BDSP_Raaga_P_SetDatasyncSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_AudioTaskDatasyncSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    unsigned int            uiConfigBufAddr =0;    
    unsigned int            uiConfigBufSize=0;   
    BDSP_Raaga_P_FwBuffer  *psFwBufInfo;    
    BDSP_Raaga_P_Command sCommand;
#if 0    
    BDSP_Raaga_P_Response sRsp;   
    BDSP_Raaga_P_MsgType      eMsgType;
#endif    
    BDSP_Raaga_Audio_FrameSyncTsmConfigParams   sFrameSyncTsmConfigParams;     


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);

    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;            
        }

        BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams_isr
            (pRaagaTask->pContext->pDevice->memHandle,
            pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr
            ,pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufSize
            ,(void *)&sFrameSyncTsmConfigParams);

        BKNI_Memcpy((void *)(volatile void*)&(sFrameSyncTsmConfigParams.sFrameSyncConfigParams)
            ,(void *)pSettings
            ,sizeof(BDSP_AudioTaskDatasyncSettings));        

        if (pRaagaTask->isStopped == false)
        {
            uiConfigBufAddr = pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr;
            uiConfigBufSize = pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size;
        }
        else
        {
            uiConfigBufAddr = pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr;
            uiConfigBufSize = pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufSize;  
        }


        BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams_isr
            (pDevice->memHandle,
            uiConfigBufAddr
            ,uiConfigBufSize
            ,( void *)&sFrameSyncTsmConfigParams);      

        if (pRaagaTask->isStopped == false)
        {
            /*Send CFG Change Command to FW*/
            psFwBufInfo = &( pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo);            
            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eNone;
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            BMEM_ConvertAddressToOffset_isr(pDevice->memHandle, (void *)(psFwBufInfo ->ui32BaseAddr), 
                &(sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr));   
            BMEM_ConvertAddressToOffset_isr(pDevice->memHandle, 
                (void *)(pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr), 
                &(sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr));    

            sCommand.uCommand.sCfgChange.ui32SizeInBytes = sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams);

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;
#if 0                        
            BKNI_ResetEvent(pRaagaTask->hEvent);
#endif
            err = BDSP_Raaga_P_SendCommand_isr(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
        }
    }
    end:

    return err;
}

BERR_Code BDSP_Raaga_P_GetTsmSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskTsmSettings    *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_Audio_FrameSyncTsmConfigParams   sFrameSyncTsmConfigParams; 

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);

    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;            
        }

        /* Read the settings from the host buffer to avoid race conditions */
        BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams_isr
            (pRaagaTask->pContext->pDevice->memHandle,
            pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr,
            pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size
            ,(void *)&sFrameSyncTsmConfigParams);

        BKNI_Memcpy((void *)(volatile void*)pSettings
            ,(void *)&(sFrameSyncTsmConfigParams.sTsmConfigParams),sizeof(BDSP_AudioTaskTsmSettings));
    }
    end:

    return err;
}

BERR_Code BDSP_Raaga_P_SetTsmSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_AudioTaskTsmSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    unsigned int            uiConfigBufAddr =0;    
    unsigned int            uiConfigBufSize=0;      
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_FwBuffer  *psFwBufInfo;
#if 0    
    BDSP_Raaga_P_Response sRsp;   
    BDSP_Raaga_P_MsgType      eMsgType;
#endif    
    BDSP_Raaga_Audio_FrameSyncTsmConfigParams   sFrameSyncTsmConfigParams;     


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);

    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;            
        }

        BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams_isr
            (pDevice->memHandle,
            pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr
            ,pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufSize
            ,(void *)&sFrameSyncTsmConfigParams);

        BKNI_Memcpy((void *)(volatile void*)&(sFrameSyncTsmConfigParams.sTsmConfigParams)
            ,(void *)pSettings
            ,sizeof(BDSP_AudioTaskTsmSettings));        

        if (pRaagaTask->isStopped == false)
        {
            uiConfigBufAddr = pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr;
            uiConfigBufSize = pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size;
        }
        else
        {

            /* If the task is stopped, write the settings into the host buffer as well - 
                                  So that it can be used instead of the DSP's buffer when GetTsmSettings 
                                  is called, to avoid any race conditions */
            BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams_isr
                (pDevice->memHandle,
                pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr
                ,pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size
                ,( void *)&sFrameSyncTsmConfigParams);

            uiConfigBufAddr = pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr;
            uiConfigBufSize = pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufSize;  
        }


        BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams_isr
            (pDevice->memHandle,
            uiConfigBufAddr
            ,uiConfigBufSize
            ,( void *)&sFrameSyncTsmConfigParams);

        if (pRaagaTask->isStopped == false)
        {
            /*Send CFG Change Command to FW*/
            psFwBufInfo = &( pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo);            
            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eNone;
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            BMEM_ConvertAddressToOffset_isr(pDevice->memHandle, (void *)(psFwBufInfo ->ui32BaseAddr), 
                &(sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr));   
            BMEM_ConvertAddressToOffset_isr(pDevice->memHandle, 
                (void *)(pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr), 
                &(sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr));    

            sCommand.uCommand.sCfgChange.ui32SizeInBytes = sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams);

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;
#if 0                        
            BKNI_ResetEvent(pRaagaTask->hEvent);
#endif
            err = BDSP_Raaga_P_SendCommand_isr(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }

        }
    }
    end:

    return err;
}

/* Handle video encoder datasync cases here */
BERR_Code BDSP_Raaga_P_GetVideoEncodeDatasyncSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_VideoEncodeTaskDatasyncSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;    
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);

    BKNI_EnterCriticalSection();
    err =BDSP_Raaga_P_GetVideoEncodeDatasyncSettings_isr(pTaskHandle,branchId,stageId,pSettings);    
    BKNI_LeaveCriticalSection();
    return err;
}

BERR_Code BDSP_Raaga_P_SetVideoEncodeDatasyncSettings(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_VideoEncodeTaskDatasyncSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;    
    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);    
    BDBG_ASSERT(pSettings);

    BKNI_EnterCriticalSection();
    err =BDSP_Raaga_P_SetVideoEncodeDatasyncSettings_isr(pTaskHandle,branchId,stageId,pSettings);    
    BKNI_LeaveCriticalSection();
    return err;
}



BERR_Code BDSP_Raaga_P_GetVideoEncodeDatasyncSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_VideoEncodeTaskDatasyncSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;
    BDSP_VideoEncodeTaskDatasyncSettings   sFrameSyncConfigParams; 

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);

    if (pRaagaTask->videoCitOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->videoCitOutput.ui32NumBranches-1));
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;
    }
    else
    {
        if (pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;            
        }

        BDSP_Raaga_P_GetVideoEncodeFrameSyncStageConfigParams_isr
            (pRaagaTask->pContext->pDevice->memHandle,
            pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr
            ,pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufSize
            ,(void *)&sFrameSyncConfigParams);

        BKNI_Memcpy((void *)(volatile void*)pSettings
            ,(void *)&(sFrameSyncConfigParams),sizeof(BDSP_VideoEncodeTaskDatasyncSettings));
    }
    end:
    return err;
}

BERR_Code BDSP_Raaga_P_SetVideoEncodeDatasyncSettings_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    const BDSP_VideoEncodeTaskDatasyncSettings *pSettings)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    unsigned int            uiConfigBufAddr =0;    
    unsigned int            uiConfigBufSize=0;   
    BDSP_Raaga_P_FwBuffer  *psFwBufInfo;    
    BDSP_Raaga_P_Command sCommand;
#if 0    
    BDSP_Raaga_P_Response sRsp;   
    BDSP_Raaga_P_MsgType      eMsgType;
#endif    
    BDSP_VideoEncodeTaskDatasyncSettings   sFrameSyncConfigParams;     


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pSettings);

    if (pRaagaTask->videoCitOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->videoCitOutput.ui32NumBranches-1));
        err = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto end;        
    }
    else
    {
        if (pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id for video encode(%d), It cannot exceed %d",branchId,pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto end;            
        }

        BDSP_Raaga_P_GetVideoEncodeFrameSyncStageConfigParams_isr
            (pRaagaTask->pContext->pDevice->memHandle,
            pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr
            ,pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufSize
            ,(void *)&sFrameSyncConfigParams);

        BKNI_Memcpy((void *)(volatile void*)&(sFrameSyncConfigParams)
            ,(void *)pSettings
            ,sizeof(BDSP_VideoEncodeTaskDatasyncSettings));        

        if (pRaagaTask->isStopped == false)
        {
            uiConfigBufAddr = pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32BaseAddr;
            uiConfigBufSize = pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo.ui32Size;
        }
        else
        {
            uiConfigBufAddr = pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr;
            uiConfigBufSize = pRaagaTask->videoCitOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufSize;  
        }


        BDSP_Raaga_P_SetVideoEncodeFrameSyncStageConfigParams_isr
            (pDevice->memHandle,
            uiConfigBufAddr
            ,uiConfigBufSize
            ,( void *)&sFrameSyncConfigParams);      

        if (pRaagaTask->isStopped == false)
        {
            /*Send CFG Change Command to FW*/
            psFwBufInfo = &( pRaagaTask->taskMemGrants.sFrameSyncTsmConfigParamBufInfo);            
            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID = pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eNone;
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            BMEM_ConvertAddressToOffset_isr(pDevice->memHandle, (void *)(psFwBufInfo ->ui32BaseAddr), 
                &(sCommand.uCommand.sCfgChange.ui32HostConfigParamBuffAddr));   
            BMEM_ConvertAddressToOffset_isr(pDevice->memHandle, 
                (void *)(pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32UserParamBufAdr), 
                &(sCommand.uCommand.sCfgChange.ui32DspConfigParamBuffAddr));    

            sCommand.uCommand.sCfgChange.ui32SizeInBytes = sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams);

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;
#if 0                        
            BKNI_ResetEvent(pRaagaTask->hEvent);
#endif
            err = BDSP_Raaga_P_SendCommand_isr(pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], &sCommand,(void *)pRaagaTask);

            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_SetStageSettings: CFG_Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
        }
    }
    end:

    return err;
}

BERR_Code BDSP_Raaga_P_GetStageStatus(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    void *pStatusBuffer, 
    size_t statusSize)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;
    BDSP_CIT_P_OpStgInfo    *pStageInfo = NULL;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pStatusBuffer);


    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }
        else
        {
            pStageInfo = (BDSP_CIT_P_OpStgInfo    *)BKNI_Malloc(sizeof(BDSP_CIT_P_OpStgInfo));
            if (pStageInfo == NULL)
            {
                BDBG_ERR(("Out of System memory"));
                err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                goto end;
            }
            BKNI_Memset(pStageInfo,0,sizeof(BDSP_CIT_P_OpStgInfo));

            *pStageInfo = pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId];
        }

        switch (pStageInfo->eStageType)
        {
        case BDSP_CIT_P_StageType_eDecode:
            err = BDSP_Raaga_P_GetAudioDecodeStageStatus
                (pRaagaTask->pContext->pDevice->memHandle,
                pStageInfo->uAlgorithm.eDecAudioAlgo
                ,pStageInfo->uAudioMode.eDecAudioMode
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32StatusBufAdr
                ,pStageInfo->sFwOpNodeInfo[pStageInfo->ui32StartNodeIndex].ui32StatusBufSize
                ,pStatusBuffer
                ,statusSize);
            if (err != BERR_SUCCESS)
            {
                goto end;
            }
            break;
        case BDSP_CIT_P_StageType_eEncode:
            break;
        case BDSP_CIT_P_StageType_ePostProc:
            break;
        default:
            BDBG_ERR(("Invalid Stage Type %d",pStageInfo->eStageType));
            BDBG_ASSERT(0);
            break;                
        }
    }
    end:
    if (pStageInfo)
        BKNI_Free(pStageInfo);
    return err;

}

BERR_Code BDSP_Raaga_P_GetDatasyncStatus(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskDatasyncStatus *pStatusBuffer)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pStatusBuffer);

    BSTD_UNUSED(pStatusBuffer);

    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }
        BDBG_WRN(("Status for DataSync is not defined by current FW"));
    }
    end:
    return err;

}

BERR_Code BDSP_Raaga_P_GetTsmStatus_isr(
    void *pTaskHandle, 
    unsigned branchId, 
    unsigned stageId, 
    BDSP_AudioTaskTsmStatus *pStatusBuffer)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_Audio_IdsTsmInfo  sIdsTsmInfo;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
    BDBG_ASSERT(pStatusBuffer);

    if (pRaagaTask->citOutput.ui32NumBranches  <= branchId)
    {
        BDBG_ERR(("Invalid Branch Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.ui32NumBranches-1));
        err =BERR_INVALID_PARAMETER;
        goto end;        
    }
    else
    {
        if (pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages <= stageId)
        {
            BDBG_ERR(("Invalid Stage Id (%d), It cannot exceed %d",branchId,pRaagaTask->citOutput.sCitBranchInfo[branchId].ui32NumStages-1));
            err =BERR_INVALID_PARAMETER;
            goto end;            
        }

        BKNI_Memcpy((void *)(volatile void*)&sIdsTsmInfo,(void *)pRaagaTask->citOutput.sCitBranchInfo[branchId].sCitStgInfo[stageId].sFwOpNodeInfo[FRAMESYNC_TSM_NODE_INDEX].ui32StatusBufAdr,sizeof(BDSP_Raaga_Audio_IdsTsmInfo));                 
        BKNI_Memcpy((void *)(volatile void*)pStatusBuffer,(void *)&(sIdsTsmInfo.sTsmInfo),sizeof(BDSP_AudioTaskTsmStatus)); 

        if (0 != ((BDSP_AudioTaskTsmStatus *)pStatusBuffer)->ui32StatusValid)
        {
            BDBG_MSG(("BDSP_Raaga_P_GetTsmStatus_isr: TSM Status buffer is not in valid status"));
            err = (BDSP_ERR_BAD_DEVICE_STATE);
            goto end;
        }
    }
    end:
    return err;

}

BERR_Code BDSP_Raaga_P_Pause(void *pTaskHandle)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BDSP_Raaga  *pDevice= pRaagaTask->pContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_P_MsgType      eMsgType;
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_WRN(("Task is not started yet. Ignoring the Pause command"));
        goto end;
    }
    else
    {
        if (pRaagaTask->paused == true)
        {
            BDBG_WRN(("Task is already in Pause state. Ignoring the Pause command"));
            goto end;            
        }
        else
        {
            /* Create Pause command */
            BKNI_Memset((void *)&sRsp,0,sizeof(BDSP_Raaga_P_Response));
            BKNI_Memset((void *)&sCommand,0,sizeof(BDSP_Raaga_P_Command));

            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_PAUSE_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter =  pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID =  pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eAckRequired;   
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;                    
            BKNI_ResetEvent(pRaagaTask->hEvent);

            err = BDSP_Raaga_P_SendCommand(
                pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], 
                &sCommand,pRaagaTask);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Pause: Pause Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Pause: Pause Command ACK timeout! Triggering Watchdog"));
                err = BERR_TRACE(err);           
                goto end;
            }

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp, eMsgType);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Pause: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_PAUSE_ACK_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {
                BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                BDBG_ERR(("BDSP_Raaga_P_Pause: Pause Command response not received successfully!"));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;                    
            }
            pRaagaTask->paused = true;
        }
    }
    end:
    return err;
}

BERR_Code BDSP_Raaga_P_Resume(void *pTaskHandle)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BDSP_Raaga  *pDevice= pRaagaTask->pContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_P_MsgType      eMsgType;
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_WRN(("Task is not started yet. Ignoring the Resume command"));    
        goto end;
    }
    else
    {
        if (pRaagaTask->paused == false)
        {
            BDBG_WRN(("Task is already in Resume state. Ignoring the Resume command"));
            goto end;            
        }
        else
        {
            /* Create Resume command */
            BKNI_Memset((void *)&sRsp,0,sizeof(BDSP_Raaga_P_Response));
            BKNI_Memset((void *)&sCommand,0,sizeof(BDSP_Raaga_P_Command));

            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_RESUME_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter =  pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID =  pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eAckRequired;   
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;                    
            BKNI_ResetEvent(pRaagaTask->hEvent);

            err = BDSP_Raaga_P_SendCommand(
                pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], 
                &sCommand,pRaagaTask);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Resume: Resume Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Resume: Resume Command ACK timeout! Triggering Watchdog"));
                err = BERR_TRACE(err);           
                goto end;
            }

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp, eMsgType);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Resume: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_RESUME_ACK_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {
                BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                BDBG_ERR(("BDSP_Raaga_P_Resume: Resume Command response not received successfully!"));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;                    
            }
            pRaagaTask->paused = false;
        }
    }
    end:
    return err;
}

BERR_Code BDSP_Raaga_P_Advance(
    void *pTaskHandle, 
    unsigned ms)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BDSP_Raaga  *pDevice= pRaagaTask->pContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_P_MsgType      eMsgType;
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_WRN(("Task is not started yet. Ignoring the Advance command"));    
        goto end;
    }
    else
    {
        if (pRaagaTask->paused == false)
        {
            BDBG_WRN(("Task %#x is not in Pause state. Ignoring the Advance command", pRaagaTask));
            goto end;            
        }
        else
        {
            /* Create Resume command */
            BKNI_Memset((void *)&sRsp,0,sizeof(BDSP_Raaga_P_Response));
            BKNI_Memset((void *)&sCommand,0,sizeof(BDSP_Raaga_P_Command));

            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_FRAME_ADVANCE_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter =  pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID =  pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eAckRequired;   
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            sCommand.uCommand.sFrameAdvance.ui32DurationOfFrameAdv = ms * 45;               

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;                    
            BKNI_ResetEvent(pRaagaTask->hEvent);

            err = BDSP_Raaga_P_SendCommand(
                pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], 
                &sCommand,pRaagaTask);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Advance: Advance Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Advance: Advance Command ACK timeout! Triggering Watchdog"));
                err = BERR_TRACE(err);           
                goto end;
            }

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp, eMsgType);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_Advance: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_FRAME_ADVANCE_ACK_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {
                BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                BDBG_ERR(("BDSP_Raaga_P_Advance: Advance Command response not received successfully!"));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;                    
            }
        }
    }
    end:
    return err;
}

BERR_Code BDSP_Raaga_P_GetPictureQueueCount_isr(
    void *pTaskHandle,
    unsigned *pPictureCount,
    BDSP_Raaga_P_MsgQueueHandle      hPictureQueue
    )
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BERR_Code err=BERR_SUCCESS;
    uint32_t dramReadPtr=0;
    uint32_t dramWritePtr=0;
    uint32_t maskReadPtr=0;
    uint32_t maskWritePtr=0; 


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_GetPictureCount_isr : Task is not started yet."));
        goto end;
    }

    dramReadPtr=BDSP_Read32_isr(hPictureQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * hPictureQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_READ_OFFSET + hPictureQueue->ui32DspOffset);

    dramWritePtr=BDSP_Read32_isr(hPictureQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * hPictureQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_WRITE_OFFSET + hPictureQueue->ui32DspOffset);

    BDBG_MSG(("dramReadPtr > %x", dramReadPtr));
    BDBG_MSG(("dramWritePtr > %x", dramWritePtr));

    maskReadPtr=dramReadPtr;
    maskWritePtr=dramWritePtr;

    /*Sanity check*/
    /*Checking boundness of read pointer- if((readptr>endaddr) OR (readptr<baseaddr)) read ptr not within bound*/
    if ((maskReadPtr>hPictureQueue->uiEndAddr)||(maskReadPtr<hPictureQueue->uiBaseAddr))
    {
        BDBG_ERR(("Read pointer not within bounds in Message Queue"));
        goto end;
    }

    /*Checking boundness of write pointer - if((writeptr>endaddr) OR (writeptr<baseaddr))  write ptr not within bound*/
    if ((maskWritePtr>hPictureQueue->uiEndAddr)||(maskWritePtr<hPictureQueue->uiBaseAddr))
    {
        BDBG_ERR(("Write pointer not within bounds in Message Queue"));
        goto end;
    }

    /* checking write ptrs */
    if (hPictureQueue->uiReadPtr!=maskReadPtr)
    {
        BDBG_ERR(("Read pointer corrupted in the Message Queue"));
        err =  BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);   
        goto end;
    }
    /* End of Sanity Check */


    /*Calc Picture count in the message queue*/

    /* Case1:if(maskwriteptr>maskreadptr)*/
    if (maskWritePtr > maskReadPtr)
    {
        *pPictureCount = (maskWritePtr - maskReadPtr)>>2;/* Diving by 4*/
    }


    /* Case2:if(maskreadptr>maskwriteptr) */
    if (maskReadPtr>maskWritePtr)
    {
        *pPictureCount = ((hPictureQueue->uiEndAddr-maskReadPtr)+
            (maskWritePtr-hPictureQueue->uiBaseAddr))>>2;
    }


    /* Case3:if(maskreadptr==maskwriteptr) */
    if (maskReadPtr==maskWritePtr)
    {
        /* The buffer is empty */
        *pPictureCount = 0x0;
    }

    end:
    return err;
}

BERR_Code BDSP_Raaga_P_GetPictureCount_isr(
    void *pTaskHandle,
    unsigned *pPictureCount)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BERR_Code err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_GetPictureCount_isr : Task is not started yet."));
        goto end;
    }

    err = BDSP_Raaga_P_GetPictureQueueCount_isr(pTaskHandle,pPictureCount,pRaagaTask->hPDQueue);

    end:
    return err;
}


BERR_Code BDSP_Raaga_P_PeekAtPicture_isr(
    void *pTaskHandle, 
    unsigned index,
    BXDM_Picture **pUnifiedPicture)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BERR_Code err=BERR_SUCCESS;
    uint32_t uiUPBPhysicalAddr=0;
    uint32_t ui32dramReadPtr=0;
    uint32_t ui32dramWritePtr=0;
    uint32_t ui32maskReadPtr=0;
    uint32_t ui32maskWritePtr=0;
    int32_t i32BytesToBeRead=0;
    void *pvMsgQueueReadAddr=NULL;


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);   
    BSTD_UNUSED( index );

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_PeekAtPicture_isr: Task is not started yet. "));
        goto end;
    }



    ui32dramReadPtr=BDSP_Read32_isr(pRaagaTask->hPDQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hPDQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_READ_OFFSET + pRaagaTask->hPDQueue->ui32DspOffset);

    ui32dramWritePtr=BDSP_Read32_isr(pRaagaTask->hPDQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hPDQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_WRITE_OFFSET + pRaagaTask->hPDQueue->ui32DspOffset);

    BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
    BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));
    ui32maskReadPtr=ui32dramReadPtr;
    ui32maskWritePtr=ui32dramWritePtr;

    /*Sanity check*/
    /* checking write ptr boundness- if((writeptr>endaddr)|(writeptr<baseaddr)) write ptr is not within bound*/ 
    if ((ui32maskWritePtr>pRaagaTask->hPDQueue->uiEndAddr)||(ui32maskWritePtr<pRaagaTask->hPDQueue->uiBaseAddr))
    {
        BDBG_ERR(("Write pointer not within bounds in Message Queue"));
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;       
    }


    /* checking read ptr boundness- if((readptr>endaddr)|(readptr<baseaddr)) read ptr is not within bound*/
    if ((ui32maskReadPtr>pRaagaTask->hPDQueue->uiEndAddr)||(ui32maskReadPtr<pRaagaTask->hPDQueue->uiBaseAddr))
    {
        BDBG_ERR(("Read pointer not within bounds in Message Queue"));
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;       
    }

    /*Updating write ptr */
    pRaagaTask->hPDQueue->uiWritePtr= ui32maskWritePtr;

    /* checking read ptrs to see if they are the same */
    if ((pRaagaTask->hPDQueue->uiReadPtr)!=ui32maskReadPtr)
    {
        BDBG_ERR(("Read pointer corrupted in the Message Queue"));
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE );      
        goto end;       
    }

    /* End of Sanity Check */

    /*******************************************************************
   Different cases: 
 
    If maskreadptr>maskwriteptr
     ReadPtrMSB   WritePtrMSB       
        0                   0                   This condn. is not possible
        0                   1                   (end-read)+(write-base)
        1                   0                     (end-read)+(write-base)
        1                   1                      this condn. is not possible
 
  If maskwriteptr>maskreadptr
   ReadptrMSB   WritePtrMSB  
    0                   0               write-read
    0                   1                  this condn. not possible
    1                   0                  this condn. not possible
    1                   1                  write-read
 
  If maskreadptr==maskwriteptr
  If the toggle bits are the same,no message to read
  If the toggle bits are different all the messages have to be read
   
 
 ***********************************************************************/
    /*Condn. for reading messages from the message queue into the message buffer*/
    /* If no msg is to be read, generate a BRAP_ERR_BUFFER_EMPTY error(new error defined in brap.h)*/

    /* Checking if a msg is present */

    /* Case1:if(readptr>writeptr)*/
    if (ui32maskReadPtr > ui32maskWritePtr)
    {
        i32BytesToBeRead=(pRaagaTask->hPDQueue->uiEndAddr-ui32maskReadPtr)+
            (ui32maskWritePtr-pRaagaTask->hPDQueue->uiBaseAddr);
    }

    /* Case2:if(writeptr>readptr) */
    if (ui32maskWritePtr>ui32maskReadPtr)
    {
        i32BytesToBeRead=ui32maskWritePtr-ui32maskReadPtr;
    }

    /*Case 3:if readptr == writeptr */
    if (ui32maskWritePtr ==ui32maskReadPtr)
    {
        /*All messages have to be read*/
        i32BytesToBeRead=pRaagaTask->hPDQueue->uiEndAddr-pRaagaTask->hPDQueue->uiBaseAddr;
    }
    if (i32BytesToBeRead < 0)
    {
        BDBG_ERR(("The Message Queue is empty.No message is present."));
        err = BERR_TRACE(BDSP_ERR_BUFFER_EMPTY);
        goto end;       
    }

    BDBG_MSG(("ui32maskReadPtr = 0x%x",ui32maskReadPtr));

/*This conversion is done to pass a virtual address to BDSP_Raaga_P_MemRead32_isr*/
    if (BERR_SUCCESS!=(err=BMEM_ConvertOffsetToAddress_isr(pRaagaTask->hPDQueue->hHeap,ui32maskReadPtr,(void **)&pvMsgQueueReadAddr)))
    {
        err = BERR_TRACE(err);    
        goto end;
    }

    /*Reading Message from the message queue into the message buffer*/
    uiUPBPhysicalAddr=BDSP_Raaga_P_MemRead32_isr(pRaagaTask->hPDQueue->hHeap,(uint32_t )pvMsgQueueReadAddr);

    BDBG_MSG(("uiUPBPhysicalAddr = 0x%x",uiUPBPhysicalAddr));

    if (BERR_SUCCESS!=(err=BMEM_ConvertOffsetToAddress_isr(pRaagaTask->hPDQueue->hHeap,uiUPBPhysicalAddr,(void **)&pvMsgQueueReadAddr)))
    {
        err = BERR_TRACE(err);
        goto end;
    }


    *pUnifiedPicture = pvMsgQueueReadAddr;

    BDBG_MSG(("*pUnifiedPicture = 0x%x",*pUnifiedPicture));    

    end:
    return err;
}

BERR_Code BDSP_Raaga_P_GetNextPicture_isr(
    void *pTaskHandle,
    BXDM_Picture **pUnifiedPicture)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BERR_Code err=BERR_SUCCESS;
    bool bReadUpdate = true; 
    uint32_t uiUPBPhysicalAddr=0;
    uint32_t ui32dramReadPtr=0;
    uint32_t ui32dramWritePtr=0;
    uint32_t ui32maskReadPtr=0;
    uint32_t ui32maskWritePtr=0;
    int32_t i32BytesToBeRead=0;
    void *pvMsgQueueReadAddr=NULL;       

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_GetNextPicture_isr : Task is not started yet. "));
        goto end;
    }


    ui32dramReadPtr=BDSP_Read32_isr(pRaagaTask->hPDQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hPDQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_READ_OFFSET + pRaagaTask->hPDQueue->ui32DspOffset);

    ui32dramWritePtr=BDSP_Read32_isr(pRaagaTask->hPDQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hPDQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_WRITE_OFFSET + pRaagaTask->hPDQueue->ui32DspOffset);

    BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
    BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));
    ui32maskReadPtr=ui32dramReadPtr;
    ui32maskWritePtr=ui32dramWritePtr;

    /*Sanity check*/
    /* checking write ptr boundness- if((writeptr>endaddr)|(writeptr<baseaddr)) write ptr is not within bound*/ 
    if ((ui32maskWritePtr>pRaagaTask->hPDQueue->uiEndAddr)||(ui32maskWritePtr<pRaagaTask->hPDQueue->uiBaseAddr))
    {
        BDBG_ERR(("Write pointer not within bounds in Message Queue"));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;
    }


    /* checking read ptr boundness- if((readptr>endaddr)|(readptr<baseaddr)) read ptr is not within bound*/
    if ((ui32maskReadPtr>pRaagaTask->hPDQueue->uiEndAddr)||(ui32maskReadPtr<pRaagaTask->hPDQueue->uiBaseAddr))
    {
        BDBG_ERR(("Read pointer not within bounds in Message Queue"));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;       
    }

    /*Updating write ptr */
    pRaagaTask->hPDQueue->uiWritePtr= ui32maskWritePtr;

    /* checking read ptrs to see if they are the same */
    if ((pRaagaTask->hPDQueue->uiReadPtr)!=ui32maskReadPtr)
    {
        BDBG_ERR(("Read pointer corrupted in the Message Queue"));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE );      
        goto end;       
    }

    /* End of Sanity Check */

    /*******************************************************************
    Different cases: 

    If maskreadptr>maskwriteptr
    ReadPtrMSB   WritePtrMSB       
       0                   0                   This condn. is not possible
       0                   1                   (end-read)+(write-base)
       1                   0                     (end-read)+(write-base)
       1                   1                      this condn. is not possible

    If maskwriteptr>maskreadptr
    ReadptrMSB   WritePtrMSB  
    0                   0               write-read
    0                   1                  this condn. not possible
    1                   0                  this condn. not possible
    1                   1                  write-read

    If maskreadptr==maskwriteptr
    If the toggle bits are the same,no message to read
    If the toggle bits are different all the messages have to be read


    ***********************************************************************/
    /*Condn. for reading messages from the message queue into the message buffer*/
    /* If no msg is to be read, generate a BRAP_ERR_BUFFER_EMPTY error(new error defined in brap.h)*/

    /* Checking if a msg is present */

    /* Case1:if(readptr>writeptr)*/
    if (ui32maskReadPtr > ui32maskWritePtr)
    {
        i32BytesToBeRead=(pRaagaTask->hPDQueue->uiEndAddr-ui32maskReadPtr)+
            (ui32maskWritePtr-pRaagaTask->hPDQueue->uiBaseAddr);
    }

    /* Case2:if(writeptr>readptr) */
    if (ui32maskWritePtr>ui32maskReadPtr)
    {
        i32BytesToBeRead=ui32maskWritePtr-ui32maskReadPtr;
    }

    /*Case 3:if readptr == writeptr */
    if (ui32maskWritePtr ==ui32maskReadPtr)
    {
        /*All messages have to be read*/
        i32BytesToBeRead=pRaagaTask->hPDQueue->uiEndAddr-pRaagaTask->hPDQueue->uiBaseAddr;
    }
    if (i32BytesToBeRead < 0)
    {
        BDBG_ERR(("The Message Queue is empty.No message is present."));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BUFFER_EMPTY);
        goto end;       
    }


/*This conversion is done to pass a virtual address to BDSP_Raaga_P_MemRead32_isr*/
    if (BERR_SUCCESS!=(err=BMEM_ConvertOffsetToAddress_isr(pRaagaTask->hPDQueue->hHeap,ui32maskReadPtr,(void **)&pvMsgQueueReadAddr)))
    {
        err = BERR_TRACE(err);    
        goto end;
    }

    /*Reading Message from the message queue into the message buffer*/
    uiUPBPhysicalAddr=BDSP_Raaga_P_MemRead32_isr(pRaagaTask->hPDQueue->hHeap,(uint32_t )pvMsgQueueReadAddr);

    BDBG_MSG(("In BRAP_VID_GetNextPicture_isr uiUPBPhysicalAddr = 0x%x",uiUPBPhysicalAddr));

    if (BERR_SUCCESS!=(err=BMEM_ConvertOffsetToAddress_isr(pRaagaTask->hPDQueue->hHeap,uiUPBPhysicalAddr,(void **)&pvMsgQueueReadAddr)))
    {
        err = BERR_TRACE(err);
        goto end;        
    }

    *pUnifiedPicture = pvMsgQueueReadAddr;

    BDBG_MSG(("In BDSP_Raaga_P_GetNextPicture_isr *pUnifiedPicture = 0x%x",*pUnifiedPicture));

    if (bReadUpdate == true)
    {
        ui32dramReadPtr=ui32dramReadPtr+4;    
        if (ui32dramReadPtr==pRaagaTask->hPDQueue->uiEndAddr)
            ui32dramReadPtr=pRaagaTask->hPDQueue->uiBaseAddr;

        /*updating read ptr in the Queue Attribute Structure*/
        BDSP_Write32_isr(pRaagaTask->hPDQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hPDQueue->ui32FifoId + 
            BDSP_RAAGA_P_FIFO_READ_OFFSET + pRaagaTask->hPDQueue->ui32DspOffset, ui32dramReadPtr); 
        BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
        BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));

        /*updating read ptr in the handle*/
        pRaagaTask->hPDQueue->uiReadPtr = ui32dramReadPtr;
    }

    end:
    return err;
}

BERR_Code BDSP_Raaga_P_ReleasePicture_isr(
    void *pTaskHandle,
    BXDM_Picture *pUnifiedPicture)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    unsigned int    uiBuffSize = 4;
    uint32_t    temp = (uint32_t)pUnifiedPicture;
    BERR_Code err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_ReleasePicture_isr : Task is not started yet. "));
        goto end;
    }

    BDBG_MSG(("In BDSP_Raaga_P_ReleasePicture_isr pUnifiedPicture = 0x%x",pUnifiedPicture));

    err = BDSP_Raaga_P_WriteVideoMsg_isr(pRaagaTask->hDSQueue, (void *)&temp,uiBuffSize);

    end:
    return err;
}

BERR_Code BDSP_Raaga_P_GetPictureDropPendingCount_isr(
    void *pTaskHandle, 
    unsigned *pPictureDropPendingCount)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BERR_Code err=BERR_SUCCESS;
    uint32_t ui32NumBranchesValid = 0, ui32NumStagesValid = 0;
    unsigned int uiBranchId = 0, uiStageId = 0,uiStatusBufAddr, uiStatusBufSize,uiStartNodeIndex;
    BDSP_CIT_P_OpStgInfo *psOpStgInfo = NULL;


    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_GetPictureDropPendingCount_isr: Task is not started yet. "));
        *pPictureDropPendingCount = 0;
        goto end;
    }

    ui32NumBranchesValid = pRaagaTask->videoCitOutput.ui32NumBranches;

    for (uiBranchId = 0; uiBranchId < ui32NumBranchesValid; uiBranchId++)
    {
        /* Get the status buffer address for this stage */
        ui32NumStagesValid = pRaagaTask->videoCitOutput.sCitBranchInfo[uiBranchId].ui32NumStages;                
        for (uiStageId = 0; uiStageId < ui32NumStagesValid; uiStageId++)
        {
            psOpStgInfo = &(pRaagaTask->videoCitOutput.sCitBranchInfo[uiBranchId].sCitStgInfo[uiStageId]);
            uiStartNodeIndex = psOpStgInfo->ui32StartNodeIndex;
            /* use docode node status buffer */
            /* TODO: Replace this hardcoding with proper define */            
            uiStatusBufAddr = psOpStgInfo->sFwOpNodeInfo[1].ui32StatusBufAdr;
            uiStatusBufSize = psOpStgInfo->sFwOpNodeInfo[1].ui32StatusBufSize;

            /* Check if this is a decode stage */
            if (BDSP_CIT_P_StageType_eVideoDecode== psOpStgInfo->eStageType)
            {
                *pPictureDropPendingCount = *(unsigned *)(uiStatusBufAddr);
            }
        }        
    }       

    end:
    return err;
}

BERR_Code BDSP_Raaga_P_RequestPictureDrop_isr(
    void *pTaskHandle,
    unsigned *pPictureDropRequestCount)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BDSP_Raaga  *pDevice= pRaagaTask->pContext->pDevice;    
    BERR_Code err=BERR_SUCCESS;
    BDSP_Raaga_P_Command sCommand;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_RequestPictureDrop_isr: Task is not started yet. "));
        goto end;
    }

    /* Create Pause command */
    sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_NUM_PIC_TO_DROP_COMMAND_ID;
    /* TODO: Need to check the use of it!!! */
    sCommand.sCommandHeader.ui32CommandCounter =  pRaagaTask->commandCounter++;
    sCommand.sCommandHeader.ui32TaskID =  pRaagaTask->taskId;
    sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eNone;   
    sCommand.uCommand.sNumPicToDropCommand.ui32NumPicToDrop = *pPictureDropRequestCount;

    pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;                    

    err = BDSP_Raaga_P_SendCommand_isr(
        pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], 
        &sCommand,pRaagaTask);
    if (BERR_SUCCESS != err)
    {
        BDBG_ERR(("BDSP_Raaga_P_RequestPictureDrop_isr: Request picture drop Command failed!"));
        err = BERR_TRACE(err);
        goto end;
    }

    end:
    return err;
}

BERR_Code BDSP_Raaga_P_DisplayInterruptEvent_isr(
    void *pTaskHandle)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BERR_Code err=BERR_SUCCESS;
    bool bRealUpdate = true;
    unsigned int    uiBuffSize = 4; 
    uint32_t puiPictureCount=0;
    uint32_t pGetUPB=0;
    uint32_t    physaddr=0;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_DisplayInterruptEvent_isr : Task is not started yet. "));
        goto end;
    }

    err = BDSP_Raaga_P_GetPictureQueueCount_isr(pTaskHandle,&puiPictureCount,pRaagaTask->hDSQueue);

    if (puiPictureCount != 0)
    {
        err = BDSP_Raaga_P_GetVideoMsg_isr(pRaagaTask->hDSQueue,&pGetUPB,bRealUpdate);          
        BMEM_ConvertAddressToOffset(pRaagaTask->hDSQueue->hHeap,(void *)pGetUPB,&physaddr);

        BDBG_MSG(("pGetUPB = 0x%x physaddr = 0x%x\n",pGetUPB,physaddr));

        err  = BDSP_Raaga_P_WriteVideoMsg_isr(pRaagaTask->hPRQueue, &physaddr,uiBuffSize);
    }

    end:
    return err;
}


BERR_Code BDSP_Raaga_P_GetPictureBuffer_isr(
    void *pTaskHandle,
    BVENC_VF_sPicParamBuff **pPictureParmBuf)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BERR_Code err=BERR_SUCCESS;
    bool bReadUpdate = true; 
    uint32_t uiPPBPhysicalAddr=0;
    uint32_t ui32dramReadPtr=0;
    uint32_t ui32dramWritePtr=0;
    uint32_t ui32maskReadPtr=0;
    uint32_t ui32maskWritePtr=0;
    int32_t i32BytesToBeRead=0;
    void *pvMsgQueueReadAddr=NULL;       

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_MSG(("BDSP_Raaga_P_GetPictureBuffer_isr : Task is not started yet. "));
        goto end;
    }


    ui32dramReadPtr=BDSP_Read32_isr(pRaagaTask->hRRQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hRRQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_READ_OFFSET + pRaagaTask->hRRQueue->ui32DspOffset);

    ui32dramWritePtr=BDSP_Read32_isr(pRaagaTask->hRRQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hRRQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_WRITE_OFFSET + pRaagaTask->hRRQueue->ui32DspOffset);

    BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
    BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));
    ui32maskReadPtr=ui32dramReadPtr;
    ui32maskWritePtr=ui32dramWritePtr;

    /*Sanity check*/
    /* checking write ptr boundness- if((writeptr>endaddr)|(writeptr<baseaddr)) write ptr is not within bound*/ 
    if ((ui32maskWritePtr>pRaagaTask->hRRQueue->uiEndAddr)||(ui32maskWritePtr<pRaagaTask->hRRQueue->uiBaseAddr))
    {
        BDBG_ERR(("Write pointer not within bounds in Message Queue"));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;
    }


    /* checking read ptr boundness- if((readptr>endaddr)|(readptr<baseaddr)) read ptr is not within bound*/
    if ((ui32maskReadPtr>pRaagaTask->hRRQueue->uiEndAddr)||(ui32maskReadPtr<pRaagaTask->hRRQueue->uiBaseAddr))
    {
        BDBG_ERR(("Read pointer not within bounds in Message Queue"));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;       
    }

    /*Updating write ptr */
    pRaagaTask->hRRQueue->uiWritePtr= ui32maskWritePtr;

    /* checking read ptrs to see if they are the same */
    if ((pRaagaTask->hRRQueue->uiReadPtr)!=ui32maskReadPtr)
    {
        BDBG_ERR(("Read pointer corrupted in the Message Queue"));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE );      
        goto end;       
    }

    /* End of Sanity Check */

    /*******************************************************************
    Different cases: 

    If maskreadptr>maskwriteptr
    ReadPtrMSB   WritePtrMSB       
       0                   0                   This condn. is not possible
       0                   1                   (end-read)+(write-base)
       1                   0                     (end-read)+(write-base)
       1                   1                      this condn. is not possible

    If maskwriteptr>maskreadptr
    ReadptrMSB   WritePtrMSB  
    0                   0               write-read
    0                   1                  this condn. not possible
    1                   0                  this condn. not possible
    1                   1                  write-read

    If maskreadptr==maskwriteptr
    If the toggle bits are the same,no message to read
    If the toggle bits are different all the messages have to be read


    ***********************************************************************/
    /*Condn. for reading messages from the message queue into the message buffer*/
    /* If no msg is to be read, generate a BRAP_ERR_BUFFER_EMPTY error(new error defined in brap.h)*/

    /* Checking if a msg is present */

    /* Case1:if(readptr>writeptr)*/
    if (ui32maskReadPtr > ui32maskWritePtr)
    {
        i32BytesToBeRead=(pRaagaTask->hRRQueue->uiEndAddr-ui32maskReadPtr)+
            (ui32maskWritePtr-pRaagaTask->hRRQueue->uiBaseAddr);
    }

    /* Case2:if(writeptr>readptr) */
    if (ui32maskWritePtr>ui32maskReadPtr)
    {
        i32BytesToBeRead=ui32maskWritePtr-ui32maskReadPtr;
    }

    /*Case 3:if readptr == writeptr. Queue is empty in this case. */
    if (ui32maskWritePtr ==ui32maskReadPtr)
    {
        i32BytesToBeRead=0;
    }
    if (i32BytesToBeRead <= 0)
    {
	    *pPictureParmBuf = 0;
		/*
	        BDBG_ERR(("The PRQ Queue is empty. No buffers available."));
	        BDBG_ASSERT(0);        
	        err = BERR_TRACE(BDSP_ERR_BUFFER_EMPTY);
	       */
        goto end;       
    }


	/* This conversion is done to pass a virtual address to BDSP_Raaga_P_MemRead32_isr */
    if (BERR_SUCCESS!=(err=BMEM_ConvertOffsetToAddress_isr(pRaagaTask->hRRQueue->hHeap, ui32maskReadPtr,(void **)&pvMsgQueueReadAddr)))
    {
        err = BERR_TRACE(err);    
        goto end;
    }

    /*Reading Message from the message queue into the message buffer*/
    uiPPBPhysicalAddr=BDSP_Raaga_P_MemRead32_isr(pRaagaTask->hRRQueue->hHeap,(uint32_t )pvMsgQueueReadAddr);

    if (BERR_SUCCESS!=(err=BMEM_ConvertOffsetToAddress_isr(pRaagaTask->hRRQueue->hHeap, uiPPBPhysicalAddr,(void **)&pvMsgQueueReadAddr)))
    {
        err = BERR_TRACE(err);
        goto end;        
    }

    *pPictureParmBuf = pvMsgQueueReadAddr;

    if (bReadUpdate == true)
    {
        ui32dramReadPtr=ui32dramReadPtr+4;    
        if (ui32dramReadPtr==pRaagaTask->hRRQueue->uiEndAddr)
            ui32dramReadPtr=pRaagaTask->hRRQueue->uiBaseAddr;

        /*updating read ptr in the Queue Attribute Structure*/
        BDSP_Write32_isr(pRaagaTask->hRRQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hRRQueue->ui32FifoId + 
            BDSP_RAAGA_P_FIFO_READ_OFFSET + pRaagaTask->hRRQueue->ui32DspOffset, ui32dramReadPtr); 
        BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
        BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));

        /*updating read ptr in the handle*/
        pRaagaTask->hRRQueue->uiReadPtr = ui32dramReadPtr;
    }

    end:
    return err;
}

/* This one is not needed as BVN will scale and write the picture in DRAM buffer and directly interrupt DSP which in turn will update RDQ */
BERR_Code BDSP_Raaga_P_PutPicture_isr(
    void 		*pTaskHandle,
    uint32_t	ui32pPPBAddress
    )
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BERR_Code err=BERR_SUCCESS;
    bool bWriteUpdate = true; 
    uint32_t ui32dramReadPtr=0;
    uint32_t ui32dramWritePtr=0;
    uint32_t ui32maskReadPtr=0;
    uint32_t ui32maskWritePtr=0;
    int32_t i32BytesToBeRead=0;
	uint32_t ui32PPBPhysicalAddr = 0;
#if 0
	void *pvWriteAddr = NULL;
#endif 

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_ERR(("BDSP_Raaga_P_PutPicture_isr : Task is not started yet. "));
        goto end;
    }


    ui32dramReadPtr=BDSP_Read32_isr(pRaagaTask->hRDQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hRDQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_READ_OFFSET + pRaagaTask->hRDQueue->ui32DspOffset);

    ui32dramWritePtr=BDSP_Read32_isr(pRaagaTask->hRDQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hRDQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_WRITE_OFFSET + pRaagaTask->hRDQueue->ui32DspOffset);

    BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
    BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));
    ui32maskReadPtr=ui32dramReadPtr;
    ui32maskWritePtr=ui32dramWritePtr;

    /*Sanity check*/
    /* checking write ptr boundness- if((writeptr>endaddr)|(writeptr<baseaddr)) write ptr is not within bound*/ 
    if ((ui32maskWritePtr>pRaagaTask->hRDQueue->uiEndAddr)||(ui32maskWritePtr<pRaagaTask->hRDQueue->uiBaseAddr))
    {
        BDBG_ERR(("Write pointer not within bounds in Message Queue"));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;
    }


    /* checking read ptr boundness- if((readptr>endaddr)|(readptr<baseaddr)) read ptr is not within bound*/
    if ((ui32maskReadPtr>pRaagaTask->hRDQueue->uiEndAddr)||(ui32maskReadPtr<pRaagaTask->hRDQueue->uiBaseAddr))
    {
        BDBG_ERR(("Read pointer not within bounds in Message Queue"));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;       
    }

    /*Updating write ptr */
    pRaagaTask->hRDQueue->uiWritePtr= ui32maskWritePtr;

    /* End of Sanity Check */

    /*******************************************************************
    Different cases: 

    If maskreadptr>maskwriteptr
    ReadPtrMSB   WritePtrMSB       
       0                   0                   This condn. is not possible
       0                   1                   (end-read)+(write-base)
       1                   0                     (end-read)+(write-base)
       1                   1                      this condn. is not possible

    If maskwriteptr>maskreadptr
    ReadptrMSB   WritePtrMSB  
    0                   0               write-read
    0                   1                  this condn. not possible
    1                   0                  this condn. not possible
    1                   1                  write-read

    If maskreadptr==maskwriteptr
    If the toggle bits are the same,no message to read
    If the toggle bits are different all the messages have to be read


    ***********************************************************************/
    /*Condn. for reading messages from the message queue into the message buffer*/
    /* If no msg is to be read, generate a BRAP_ERR_BUFFER_EMPTY error(new error defined in brap.h)*/

    /* Checking if a msg is present */

    /* Case1:if(readptr>writeptr)*/
    if (ui32maskReadPtr > ui32maskWritePtr)
    {
        i32BytesToBeRead=(pRaagaTask->hRDQueue->uiEndAddr-ui32maskReadPtr)+
            (ui32maskWritePtr-pRaagaTask->hRDQueue->uiBaseAddr);
    }

    /* Case2:if(writeptr>readptr) */
    if (ui32maskWritePtr>ui32maskReadPtr)
    {
        i32BytesToBeRead=ui32maskWritePtr-ui32maskReadPtr;
    }

    /*Case 3:if readptr == writeptr */
    if (ui32maskWritePtr ==ui32maskReadPtr)
    {
        /* The queue is empty */
        i32BytesToBeRead = 0;
    }
	/*
    if (i32BytesToBeRead == 4)
    {
        BDBG_ERR(("The PDQ Queue is empty. No buffers available."));
        BDBG_ASSERT(0);        
        err = BERR_TRACE(BDSP_ERR_BUFFER_EMPTY);
        goto end;       
    }
	*/
	if (i32BytesToBeRead == (int32_t)((pRaagaTask->hRDQueue->uiEndAddr - pRaagaTask->hRDQueue->uiBaseAddr)-4) )
	{
		BDBG_ERR(("RDQ is full and RRQ available. Encoder slow???"));
		goto end;
	}

	/* Put the buffer in PDQ */
	err = BMEM_ConvertAddressToOffset_isr(pRaagaTask->hRDQueue->hHeap, (void *) ui32pPPBAddress, &ui32PPBPhysicalAddr);

	/* Now ui32dramWritePtr is NOT a dram pointer but an RDB address */
	BDSP_Write32_isr(pRaagaTask->hRDQueue->hRegister, (ui32dramWritePtr-BCHP_PHYSICAL_OFFSET), ui32PPBPhysicalAddr); 
#if 0
	BMEM_ConvertOffsetToAddress_isr(pRaagaTask->hRDQueue->hHeap, ui32dramWritePtr, &pvWriteAddr);
	BDSP_Raaga_P_MemWrite32_isr(pRaagaTask->hRDQueue->hHeap, (uint32_t) pvWriteAddr, ui32PPBPhysicalAddr);
#endif 

	/* Update the write pointer of PDQ */
    if (bWriteUpdate == true)
    {
	    ui32dramWritePtr = ui32dramWritePtr + 4;
        if (ui32dramWritePtr==pRaagaTask->hRDQueue->uiEndAddr)
            ui32dramWritePtr=pRaagaTask->hRDQueue->uiBaseAddr;

        /*updating write ptr in the Queue Attribute Structure*/
        BDSP_Write32_isr(pRaagaTask->hRDQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * pRaagaTask->hRDQueue->ui32FifoId + 
            BDSP_RAAGA_P_FIFO_WRITE_OFFSET + pRaagaTask->hRDQueue->ui32DspOffset, ui32dramWritePtr); 
        BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
        BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));

        /*updating read ptr in the handle*/
        pRaagaTask->hRDQueue->uiReadPtr = ui32dramReadPtr;
    }

    end:
    return err;
}


BERR_Code BDSP_Raaga_P_PowerStandby(
    void *pDeviceHandle,
    BDSP_StandbySettings    *pSettings)
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;    
    BERR_Code err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

    if (pSettings)
        BSTD_UNUSED(pSettings);


#ifdef BCHP_PWR_RESOURCE_RAAGA
#ifdef BCHP_PWR_RESOURCE_RAAGA_SRAM
    if (!pDevice->powerStandby)
    {
        BDSP_RaagaContext *pRaagaContext=NULL;    
        BDSP_RaagaTask  *pRaagaTask=NULL;
		uint32_t			ui32DspIndex;

        for ( pRaagaContext = BLST_S_FIRST(&pDevice->contextList);
            pRaagaContext != NULL;
            pRaagaContext = BLST_S_NEXT(pRaagaContext, node) )
        {
            for ( pRaagaTask = BLST_S_FIRST(&pRaagaContext->allocTaskList);
                pRaagaTask != NULL;
                pRaagaTask = BLST_S_NEXT(pRaagaTask, node) )
            {
                if (pRaagaTask->isStopped == false)
                {
                    BDBG_ERR(("Task %x is not stopped. Can not go in standby",pRaagaTask));
                    err = BERR_INVALID_PARAMETER;
                    goto end;
                }
            }
        }
        /*Disable watchdog*/              
        err = BDSP_Raaga_P_EnableDspWatchdogTimer(pDeviceHandle,false);
        if (err!=BERR_SUCCESS)
        {
            BDBG_MSG(("BDSP_Raaga_P_EnableDspWatchdogTimer::Error returned %x!",err));
            err= BERR_TRACE(err);
            goto end;
        }

        /*Reset DSP/S*/
		for(ui32DspIndex = 0; ui32DspIndex < BDSP_RAAGA_MAX_DSP; ui32DspIndex++)
		{
	        BDSP_Raaga_P_ResetHardware(pDeviceHandle, ui32DspIndex);
		}

        /*Release resources*/
        BCHP_PWR_ReleaseResource(pDevice->chpHandle, BCHP_PWR_RESOURCE_RAAGA);
        BCHP_PWR_ReleaseResource(pDevice->chpHandle, BCHP_PWR_RESOURCE_RAAGA_SRAM);        

        pDevice->powerStandby = true;

    }
    else
    {
        BDBG_WRN(("Already in standby mode"));
        err = BERR_INVALID_PARAMETER;       
        goto end;
    }

    end:        
#endif    
#endif


    return err;
}    
BERR_Code BDSP_Raaga_P_PowerResume(
    void *pDeviceHandle)
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;
    BERR_Code err=BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);

#ifdef BCHP_PWR_RESOURCE_RAAGA
#ifdef BCHP_PWR_RESOURCE_RAAGA_SRAM 
    /* if we reach here, then no channels are active. we can power down */
    if (pDevice->powerStandby)
    {
        /*Acquire Resources*/
        BCHP_PWR_AcquireResource(pDevice->chpHandle, BCHP_PWR_RESOURCE_RAAGA);
        BCHP_PWR_AcquireResource(pDevice->chpHandle, BCHP_PWR_RESOURCE_RAAGA_SRAM);          

        /*Reboot and initialize DSP and its configurations*/
        pDevice->deviceWatchdogFlag = true;                
        BDSP_Raaga_P_Open(pDevice);    
        pDevice->deviceWatchdogFlag = false;

        /*Enable Watchdog*/
        err = BDSP_Raaga_P_EnableDspWatchdogTimer(pDeviceHandle,true);
        if (err!=BERR_SUCCESS)
        {
            BDBG_MSG(("BDSP_Raaga_P_EnableDspWatchdogTimer::Error returned %x!",err));
            err= BERR_TRACE(err);
            goto end;
        }

        pDevice->powerStandby = false;

    }
    else
    {
        BDBG_WRN(("Not in standby mode"));
        err = BERR_INVALID_PARAMETER;            
        goto end;
    }
    end:              
#endif    
#endif


    return err;
}    

/**************************************************************************************
This command puts FW in zero fill mode. This is needed to handle NRT Xcode case. In NRT xcode, decoder
has to trigger STC to move it forward and if there is no data in the input CDB, it is supposed to do nothing
so if there is an audio gap, audio stalls and does not trigger STC which in turn stall video also by virtue of
AV_WINDOW. This command will be called by upper layer when it detects such kind of gap in stream and 
this will make sure aduio fills zeroes and STC moves forward avoiding the deadlock.
***************************************************************************************/
BERR_Code BDSP_Raaga_P_AudioGapFill(void *pTaskHandle)
{
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;      
    BDSP_Raaga  *pDevice= pRaagaTask->pContext->pDevice;
    BERR_Code err=BERR_SUCCESS;
#if 0
    BDSP_Raaga_P_MsgType      eMsgType;
#endif 
    BDSP_Raaga_P_Command sCommand;
    BDSP_Raaga_P_Response sRsp;

    BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);

    if (pRaagaTask->isStopped == true)
    {
        BDBG_WRN(("Task is not started yet. Ignoring the Audio Gap Fill command"));
        goto end;
    }
    else
    {
        if (pRaagaTask->paused == true)
        {
            BDBG_WRN(("Task is already in Pause state. Ignoring the Audio Gap Fill command"));
            goto end;            
        }
        else
        {
            /* Create Pause command */
            BKNI_Memset((void *)&sRsp,0,sizeof(BDSP_Raaga_P_Response));
            BKNI_Memset((void *)&sCommand,0,sizeof(BDSP_Raaga_P_Command));

            sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_AUDIO_GAP_FILL_COMMAND_ID;
            sCommand.sCommandHeader.ui32CommandCounter =  pRaagaTask->commandCounter++;
            sCommand.sCommandHeader.ui32TaskID =  pRaagaTask->taskId;
            sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eNone;   
            sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command);    

            pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;                    
            BKNI_ResetEvent(pRaagaTask->hEvent);

            err = BDSP_Raaga_P_SendCommand(
                pDevice->hCmdQueue[pRaagaTask->settings.dspIndex], 
                &sCommand,pRaagaTask);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_AudioGapFill: Audio Gap Fill Command failed!"));
                err = BERR_TRACE(err);
                goto end;
            }
            /* Wait for Ack_Response_Received event w/ timeout */
			/* As of now, we do not need any ACK or RESPONSE for this command */
#if 0
            err = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
            if (BERR_TIMEOUT == err)
            {
                BDBG_ERR(("BDSP_Raaga_P_AudioGapFill: Audio Gap Fill Command ACK timeout!"));
                err = BERR_TRACE(err);           
                goto end;
            }

            eMsgType = BDSP_Raaga_P_MsgType_eSyn;
            err = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp, eMsgType);
            if (BERR_SUCCESS != err)
            {
                BDBG_ERR(("BDSP_Raaga_P_AudioGapFill: Unable to read ACK!"));
                err = BERR_TRACE(err);
                goto end;
            }

            if ((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
                (sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_PAUSE_ACK_ID)||
                (sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
            {
                BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                BDBG_ERR(("BDSP_Raaga_P_Pause: Pause Command response not received successfully!"));
                err = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
                goto end;                    
            }
#endif 
        }
    }
    end:
    return err;
}


BERR_Code BDSP_Raaga_P_AllocateExternalInterrupt(void *pDeviceHandle, uint32_t dspIndex, BDSP_ExternalInterruptHandle *pInterruptHandle)
{
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;    
    BERR_Code err=BERR_SUCCESS;
	BDSP_RaagaExternalInterrupt *pRaagaExtInterrupt;
	uint32_t	j;

	BDBG_ENTER(BDSP_Raaga_P_AllocateExternalInterrupt);

	BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
	/* Allocate the InterruptHandle first */
    pRaagaExtInterrupt = BKNI_Malloc(sizeof(BDSP_RaagaExternalInterrupt));
    if ( NULL == pRaagaExtInterrupt ) 
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(pRaagaExtInterrupt, 0, sizeof(BDSP_RaagaExternalInterrupt));

    /* Success, add to device's interrupt list */
    BLST_S_INSERT_HEAD(&pDevice->interruptList, pRaagaExtInterrupt, node);
	*pInterruptHandle = &pRaagaExtInterrupt->extInterrupt;
	
	/* Find a free interrupt */
	BKNI_AcquireMutex(pDevice->dspInterruptMutex[dspIndex]);
	for (j=0; j<BDSP_RAAGA_MAX_INTERRUPTS_PER_DSP; j++)
    {
        if (false == pDevice->dspInterrupts[dspIndex][j])
            break;
    }
	
    if (j >= BDSP_RAAGA_MAX_INTERRUPTS_PER_DSP)
    {
        BKNI_ReleaseMutex(pDevice->dspInterruptMutex[dspIndex]);
        BDBG_ERR(("BDSP_Raaga_P_AllocateExternalInterrupt: Unable to find free interrupt to dsp!"));
        err = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_interrupt_alloc_fail;           
    }
    else
    {
        pDevice->dspInterrupts[dspIndex][j] = true;
    }
	BKNI_ReleaseMutex(pDevice->dspInterruptMutex[dspIndex]);
	
	/* Got the interrupt bit. Populate interrupt handle now */
	BDBG_MSG(("Found free interrupt bit number %d of ESR_SI for dsp %d", j, dspIndex));
	pRaagaExtInterrupt->InterruptInfo.bitNum = j;
	pRaagaExtInterrupt->InterruptInfo.address = BCHP_RAAGA_DSP_ESR_SI_INT_SET + pDevice->dspOffset[dspIndex];
	pRaagaExtInterrupt->dspIndex = dspIndex;
	pRaagaExtInterrupt->extInterrupt.hDsp = &pDevice->device;
	pRaagaExtInterrupt->extInterrupt.pExtInterruptHandle = (void *)pRaagaExtInterrupt;
	pRaagaExtInterrupt->pDevice = pDevice;
	
    /* Done, mark objects as valid */
	BDBG_OBJECT_SET(pRaagaExtInterrupt, BDSP_RaagaExternalInterrupt);
    BDBG_OBJECT_SET(&pRaagaExtInterrupt->extInterrupt, BDSP_ExternalInterrupt);
	goto end;

err_interrupt_alloc_fail:	
	*pInterruptHandle = NULL;
    BLST_S_REMOVE(&pDevice->interruptList, pRaagaExtInterrupt, BDSP_RaagaExternalInterrupt, node);
    BKNI_Free(pRaagaExtInterrupt);
	
end:
	BDBG_LEAVE(BDSP_Raaga_P_AllocateExternalInterrupt);
	return err;
}

BERR_Code BDSP_Raaga_P_FreeExternalInterrupt(void *pInterruptHandle)
{
	BERR_Code err=BERR_SUCCESS;
	BDSP_RaagaExternalInterrupt *pExtInterrupt = (BDSP_RaagaExternalInterrupt *)pInterruptHandle;
	BDSP_Raaga *pRaaga = pExtInterrupt->pDevice;
	BDBG_ENTER(BDSP_Raaga_P_FreeExternalInterrupt);

	BDBG_OBJECT_ASSERT(pExtInterrupt, BDSP_RaagaExternalInterrupt);
	
	/* Free-up the interrupt bit */
	BDBG_MSG(("Freeing up interrupt bit number %d of ESR_SI for dsp %d", pExtInterrupt->InterruptInfo.bitNum, pExtInterrupt->dspIndex));
	pRaaga->dspInterrupts[pExtInterrupt->dspIndex][pExtInterrupt->InterruptInfo.bitNum] = false;

	/* Remove the handle from device's list */
	BLST_S_REMOVE(&pRaaga->interruptList, pExtInterrupt, BDSP_RaagaExternalInterrupt, node);
	BKNI_Free(pExtInterrupt);

	BDBG_LEAVE(BDSP_Raaga_P_FreeExternalInterrupt);

	return err;

}
BERR_Code BDSP_Raaga_P_AudioCaptureCreate(
    void *pContextHandle, 
    const BDSP_AudioCaptureCreateSettings *pCaptureCreateSettings,
    BDSP_AudioCaptureHandle *pCapture)
{
	BERR_Code errCode = BERR_SUCCESS;
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;
    BDSP_RaagaCapture *pRaagaCapture;
    
    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    *pCapture = NULL;

    /* Allocate capture structure and add it to the task's capture list */
	pRaagaCapture = (BDSP_RaagaCapture *)BKNI_Malloc(sizeof(BDSP_RaagaCapture));
    if ( NULL == pRaagaCapture )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc_stage;
    }
    
    /* Update the capture information using the capture create settings */
    BKNI_Memset(pRaagaCapture, 0, sizeof(*pRaagaCapture));
    pRaagaCapture->capture.pCapHandle = pRaagaCapture;
    /* Use the heap provided */
    if (pCaptureCreateSettings->hHeap)
    {
        pRaagaCapture->hHeap = pCaptureCreateSettings->hHeap;
    }
    else
    {
        pRaagaCapture->hHeap = pRaagaContext->pDevice->memHandle;
    }
    
    pRaagaCapture->pDevice = pRaagaContext->pDevice;
    
    errCode = BDSP_Raaga_P_InitAudioCaptureInfo(pRaagaCapture, pCaptureCreateSettings);

    if (errCode != BERR_SUCCESS)
    {
    	goto err_capture_init;        
    }
    
    pRaagaCapture->capture.destroy = BDSP_Raaga_P_AudioCaptureDestroy;
    pRaagaCapture->capture.add = BDSP_Raaga_P_AudioCaptureAdd;
    pRaagaCapture->capture.remove = BDSP_Raaga_P_AudioCaptureRemove;
    pRaagaCapture->capture.getBuffer = BDSP_Raaga_P_AudioCaptureGetBuffer;
    pRaagaCapture->capture.consumeData = BDSP_Raaga_P_AudioCaptureConsumeData;
    
	BDBG_OBJECT_SET(pRaagaCapture, BDSP_RaagaCapture);
    
    *pCapture = &pRaagaCapture->capture;
    
err_malloc_stage:

err_capture_init:

	return errCode;
}

void BDSP_Raaga_P_AudioCaptureDestroy (
    void *pCapHandle
    )
{
    BDSP_RaagaCapture *pRaagaCapture = pCapHandle;
    
    BDBG_OBJECT_ASSERT(pRaagaCapture, BDSP_RaagaCapture);

	if ( NULL != pRaagaCapture->pTask )
	{
		BDBG_ERR(("Please call BDSP_AudioTask_RemoveCapture() before calling BDSP_AudioCapture_Destroy()"));
		BDBG_ASSERT(NULL == pRaagaCapture->pTask);
	}	
    
    pRaagaCapture->enabled = false;
    BMEM_Free(pRaagaCapture->hHeap, (void *)pRaagaCapture->capPtrs[0].captureBufferPtr.ui32BaseAddr);
    BKNI_Free(pRaagaCapture);
}

BERR_Code BDSP_Raaga_P_AudioCaptureAdd(
    void *pCapHandle,
    void *pTaskHandle, 
    const BDSP_AudioTaskAddCaptureSettings *pSettings
    )
{
    BERR_Code err = BERR_SUCCESS;
    int j;
    BDSP_RaagaCapture *pRaagaCapture = pCapHandle;
    BDSP_RaagaTask *pRaagaTask = pTaskHandle;
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer;
    BDSP_AF_P_DistinctOpType eOpType;
    uint8_t stageId, branchId, portNum, distinctOpIndex;
    BDSP_CIT_P_FwStgSrcDstDetails *pDstDetails;

    BDBG_ASSERT(NULL != pRaagaCapture);
    
    BKNI_AcquireMutex(pRaagaCapture->pDevice->captureMutex);
    BLST_S_INSERT_HEAD(&pRaagaTask->captureList, pRaagaCapture, node);
    BKNI_ReleaseMutex(pRaagaCapture->pDevice->captureMutex);

    pRaagaCapture->pTask = pRaagaTask;
    
    /* Fill in the pointers and buffer type for the output buffers into capture info structure */
    eOpType = pSettings->eOpType;
    stageId = pSettings->stageId;
    branchId = pSettings->branchId;
    
    distinctOpIndex = 0;
    for (j = 0; j < (int)BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS; j++)
    {
        if (eOpType == pRaagaCapture->pTask->settings.pBranchInfo[branchId]->sFwStgInfo[stageId].sStgConnectivity.eDistinctOpType[j])
        {
            distinctOpIndex = j;
            break;
        }
    }
    portNum = pSettings->outputId;

    /* We will get a stage handle instead of the branchid, stageid once the cit refactoring changes are complete */
    pDstDetails = &pRaagaCapture->pTask->settings.pBranchInfo[branchId]->sFwStgInfo[stageId].sStgConnectivity.sDstDetails[distinctOpIndex][portNum];

    /* Populate the output buffer type, num buffers and the output buffer pointers */
    err = BDSP_Raaga_P_GetAudioOutputPointers(pDstDetails, pRaagaCapture);
    if (err != BERR_SUCCESS)
    {
        BLST_S_REMOVE(&pRaagaTask->captureList, pRaagaCapture, BDSP_RaagaCapture, node);
        return BERR_TRACE(err);
    }
    pRaagaCapture->updateRead = pSettings->updateRead;
    
    /* Initialize the read/write pointers to base */
    for (j = 0; j < (int)BDSP_AF_P_MAX_CHANNELS; j++)
    {
        pBuffer = &pRaagaCapture->capPtrs[j].captureBufferPtr;
        pBuffer->ui32ReadAddr = pBuffer->ui32BaseAddr;
        pBuffer->ui32WriteAddr = pBuffer->ui32BaseAddr;
    }
    
    /* Set the shadow read pointers and last write pointer */
    switch (pRaagaCapture->eBuffType)
    {
        case BDSP_AF_P_BufferType_eFMM:
        case BDSP_AF_P_BufferType_eRDB:
        case BDSP_AF_P_BufferType_eRAVE:
            for (j = 0; j < pRaagaCapture->numBuffers; j++)
            {
                pRaagaCapture->capPtrs[j].shadowRead = BREG_Read32(
                    pRaagaCapture->pTask->pContext->pDevice->regHandle, 
                    pRaagaCapture->capPtrs[j].outputBufferPtr.ui32BaseAddr);
                pRaagaCapture->capPtrs[j].lastWrite = BREG_Read32(
                    pRaagaCapture->pTask->pContext->pDevice->regHandle, 
                    pRaagaCapture->capPtrs[j].outputBufferPtr.ui32WriteAddr);
            }
            break;
        case BDSP_AF_P_BufferType_eDRAM:
        default:
            for (j = 0; j < pRaagaCapture->numBuffers; j++)
            {
                pRaagaCapture->capPtrs[j].shadowRead = pRaagaCapture->capPtrs[j].outputBufferPtr.ui32BaseAddr;
                pRaagaCapture->capPtrs[j].lastWrite = pRaagaCapture->capPtrs[j].outputBufferPtr.ui32WriteAddr;
            }
            break;
    }
    
    /* Enable the capture */
    pRaagaCapture->enabled = true;
    
    return BERR_SUCCESS;
}

void BDSP_Raaga_P_AudioCaptureRemove(
    void *pCapHandle,
    void *pTaskHandle
    )
{
    BDSP_RaagaCapture *pRaagaCapture = pCapHandle;
    BDSP_RaagaTask *pRaagaTask = pTaskHandle;

    BDBG_OBJECT_ASSERT(pRaagaCapture, BDSP_RaagaCapture);
	BDBG_OBJECT_ASSERT(pRaagaTask, BDSP_RaagaTask);
	BDBG_ASSERT(pRaagaTask == pRaagaCapture->pTask);
    
    BKNI_AcquireMutex(pRaagaCapture->pDevice->captureMutex);
    BLST_S_REMOVE(&pRaagaTask->captureList, pRaagaCapture, BDSP_RaagaCapture, node);
    BKNI_ReleaseMutex(pRaagaCapture->pDevice->captureMutex);
    
    pRaagaCapture->enabled = false;
    pRaagaCapture->pTask = NULL;
}


BERR_Code BDSP_Raaga_P_ProcessAudioCapture(
    void *pDevice /* [in] device handle */
    )
{
    int j;
    BERR_Code err = BERR_SUCCESS;

    BDSP_Raaga *pRaagaDevice = (BDSP_Raaga *)pDevice;
    BDSP_RaagaTask *pRaagaTask = NULL;
    BDSP_RaagaContext *pRaagaContext = NULL;
    BDSP_RaagaCapture *pRaagaCapture = NULL;

    bool retval;
    uint32_t opBuffDepth, capBuffSpace, bytesToCopy, 
             captureWrite, captureRead, captureBase;
    
    void *readAddr;
    
    BDBG_ASSERT(NULL != pRaagaDevice);

    /* Need to iterate through all contexts -> tasks -> captures and 
    copy the data from output buffers to capture buffers */

    BKNI_AcquireMutex(pRaagaDevice->captureMutex);
    for ( pRaagaContext = BLST_S_FIRST(&pRaagaDevice->contextList);
        pRaagaContext != NULL;
        pRaagaContext = BLST_S_NEXT(pRaagaContext, node) )
    {
        for ( pRaagaTask = BLST_S_FIRST(&pRaagaContext->allocTaskList);
            pRaagaTask != NULL;
            pRaagaTask = BLST_S_NEXT(pRaagaTask, node) )
        {
            for ( pRaagaCapture = BLST_S_FIRST(&pRaagaTask->captureList);
                pRaagaCapture != NULL;
                pRaagaCapture = BLST_S_NEXT(pRaagaCapture, node) )
            {
                if (pRaagaCapture->enabled)
                {
                    for (j = 0; j < pRaagaCapture->numBuffers; j++)
                    {
                        /* Detect read crossing shadow read and print debug error */
                        retval = BDSP_Raaga_P_DetectAudioCaptureError(&pRaagaCapture->capPtrs[j].outputBufferPtr, 
                                                         pRaagaCapture->capPtrs[j].shadowRead,
                                                         pRaagaCapture->capPtrs[j].lastWrite,
                                                         pRaagaCapture->eBuffType,
                                                         pRaagaCapture->pTask->pContext->pDevice->regHandle);
                                                         
                        if (retval)
                        {
                            BDBG_ERR(("!!! Capture error detected for buffer [%d] in capture %x", j, pRaagaCapture));
                            continue;
                        }
                        
                        /* GetBufferDepth of output buffer */
                        opBuffDepth = BDSP_Raaga_P_GetAudioBufferDepthLinear(&pRaagaCapture->capPtrs[j].outputBufferPtr,
                                                                pRaagaCapture->capPtrs[j].shadowRead,
                                                                pRaagaCapture->eBuffType,
                                                                pRaagaCapture->pTask->pContext->pDevice->regHandle);

                        /* GetFreeSpace to end in the capture buffer */
                        capBuffSpace = BDSP_Raaga_P_GetAudioCaptureBufferFreeSpaceLinear(&pRaagaCapture->capPtrs[j].captureBufferPtr);

                        /* Copy the minimum */
                        bytesToCopy = (opBuffDepth < capBuffSpace) ? opBuffDepth : capBuffSpace;
                        
                        /* Copy the data from the output buffers to the capture buffers */
                        captureWrite = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32WriteAddr;

                        /* Convert physical offset to virtual address */
                        if (pRaagaCapture->eBuffType == BDSP_AF_P_BufferType_eFMM)
                        {
                            /* Clear the wrap bit while copying the data from FMM buffers */
                            BMEM_ConvertOffsetToAddress(pRaagaTask->pContext->pDevice->memHandle, 
                                                        (pRaagaCapture->capPtrs[j].shadowRead & 0x7FFFFFFF),
                                                        &readAddr);
                        }
                        else
                        {
                            BMEM_ConvertOffsetToAddress(pRaagaCapture->pTask->pContext->pDevice->memHandle, 
                                                        pRaagaCapture->capPtrs[j].shadowRead, 
                                                        &readAddr);
                        }

                        BKNI_Memcpy((void *)captureWrite, readAddr, bytesToCopy);
                        
                        /* Update the capture buffer write pointer and detect capture buffer overflow */
                        captureWrite += bytesToCopy;
                        captureBase = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32BaseAddr;
                        captureRead = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32ReadAddr;
                        if (captureWrite > pRaagaCapture->capPtrs[j].captureBufferPtr.ui32EndAddr)
                        {
                            BDBG_ERR(("!!! Error in capture logic: non-contiguous capture detected"));
                        }
                        
                        if (captureWrite == pRaagaCapture->capPtrs[j].captureBufferPtr.ui32EndAddr)
                        {
                            if (captureBase == captureRead)
                            {
                                BDBG_ERR(("!!! Capture buffer overflow for buffer [%d] for capture %x", j, pRaagaCapture));
                                continue;
                            }
                            else
                            {
                                captureWrite = captureBase;
                            }
                        }

                        pRaagaCapture->capPtrs[j].captureBufferPtr.ui32WriteAddr = captureWrite;
                        
                        /* Update the shadow read pointer and last write pointer */
                        BDSP_Raaga_P_GetUpdatedShadowReadAndLastWrite(&pRaagaCapture->capPtrs[j].outputBufferPtr, 
                                                              &pRaagaCapture->capPtrs[j].shadowRead,
                                                              &pRaagaCapture->capPtrs[j].lastWrite,
                                                              pRaagaCapture->eBuffType,
                                                              bytesToCopy,
                                                              pRaagaTask->pContext->pDevice->regHandle);

                        /* Update the output buffer read pointer in required */
                        if (pRaagaCapture->updateRead)
                        {
                            BDSP_Raaga_P_UpdateReadPointer(&pRaagaCapture->capPtrs[j].outputBufferPtr,
                                                   pRaagaCapture->eBuffType,
                                                   pRaagaCapture->capPtrs[j].shadowRead,
                                                   pRaagaTask->pContext->pDevice->regHandle);
                        }
                    }
                }
            }
        }
    }
    BKNI_ReleaseMutex(pRaagaDevice->captureMutex);
    
    return err;
}

BERR_Code BDSP_Raaga_P_AudioCaptureGetBuffer(
    void *pCapture,   /* [in] capture handle */
    BDSP_BufferDescriptor *pBuffers     /* [out] pointer to buffer descriptor */
    )
{
    int j;
    uint32_t address, size, minSize, chunk1 = 0;
    uint32_t base = 0, read = 0, end = 0, write = 0;
    BDSP_RaagaCapture *pRaagaCapture;
    BERR_Code err = BERR_SUCCESS;

    pRaagaCapture = (BDSP_RaagaCapture *)pCapture;
    
    BDBG_ASSERT(NULL != pRaagaCapture);
    
    pBuffers->interleaved = false;
    pBuffers->numBuffers = pRaagaCapture->numBuffers;
    
    if (pRaagaCapture->enabled)
    {
        minSize = 0xFFFFFFFF;
        for (j = 0; j < pRaagaCapture->numBuffers; j++)
        {
            base = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32BaseAddr;
            end = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32EndAddr;
            read = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32ReadAddr;
            write = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32WriteAddr;
            
            address = read;
            size = write - read;
            chunk1 = size;
            if (read > write)
            {
                chunk1 = end - read; 
                size = chunk1 + (write - base);
            }
            
            if (size < minSize)
            {
                minSize = size;
            }
            pBuffers->buffers[j].pBuffer = (void *)read;
            pBuffers->buffers[j].pWrapBuffer = (void *)base;
        }
        
        /* Use chunk1 size from the last buffer as this 
        is expected to be the same for all buffers */
        if (minSize <= chunk1)
        {
            /* Atleast one buffer did not wrap around */
            for (j = 0; j < pRaagaCapture->numBuffers; j++)
            {
                pBuffers->buffers[j].pWrapBuffer = NULL;
            }
            pBuffers->bufferSize = minSize;
            pBuffers->wrapBufferSize = 0;
        }
        else
        {
            pBuffers->bufferSize = chunk1;
            pBuffers->wrapBufferSize = minSize - pBuffers->bufferSize;
        }
    }
    else
    {
        pBuffers->bufferSize = 0;
        pBuffers->wrapBufferSize = 0;
    }
    
    return err;
}

BERR_Code BDSP_Raaga_P_AudioCaptureConsumeData(
    void *pCapture, /* [in] capture handle */
    uint32_t numBytes /* [in] sizes of data read from each intermediate buffer */
    )
{
    int j;
    uint32_t captureRead;
    BDSP_RaagaCapture *pRaagaCapture;
    BERR_Code err = BERR_SUCCESS;

    pRaagaCapture = (BDSP_RaagaCapture *)pCapture;
    
    BDBG_ASSERT(NULL != pRaagaCapture);

    if (pRaagaCapture->enabled)
    {
        for (j = 0; j < pRaagaCapture->numBuffers; j++)
        {
            captureRead = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32ReadAddr;
            captureRead += numBytes;
            if (captureRead >= pRaagaCapture->capPtrs[j].captureBufferPtr.ui32EndAddr)
            {
                captureRead = pRaagaCapture->capPtrs[j].captureBufferPtr.ui32BaseAddr +
                               (captureRead - pRaagaCapture->capPtrs[j].captureBufferPtr.ui32EndAddr);
            }
            pRaagaCapture->capPtrs[j].captureBufferPtr.ui32ReadAddr = captureRead;
        }
    }
    
    return err;
}

uint32_t BDSP_Raaga_P_GetAudioBufferDepthLinear(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, 
    uint32_t ui32ShadowRead, 
    BDSP_AF_P_BufferType eType, 
    BREG_Handle hReg)
{
    uint32_t depth, ui32BaseAddr, ui32EndAddr, ui32ReadAddr, ui32WriteAddr, ui32WrapAddr;
    
    switch (eType)
    {
        case BDSP_AF_P_BufferType_eRAVE:
            ui32BaseAddr = BREG_Read32(hReg, pBuffer->ui32BaseAddr);
            ui32EndAddr = BREG_Read32(hReg, pBuffer->ui32EndAddr);
            ui32ReadAddr = ui32ShadowRead;
            ui32WriteAddr = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            ui32WrapAddr = BREG_Read32(hReg, pBuffer->ui32WrapAddr);
            
            if (ui32WrapAddr == 0)
                ui32WrapAddr = ui32EndAddr;
            
            depth = ui32WriteAddr - ui32ReadAddr;
            if (ui32WriteAddr < ui32ReadAddr)
            {
                /* Return only the contiguous chunk size */
                depth = (ui32WrapAddr - ui32ReadAddr) + 1;
            }
            break;
            
        case BDSP_AF_P_BufferType_eFMM:
            ui32BaseAddr = BREG_Read32(hReg, pBuffer->ui32BaseAddr);
            ui32EndAddr = BREG_Read32(hReg, pBuffer->ui32EndAddr);
            ui32WriteAddr = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            ui32ReadAddr = ui32ShadowRead;
            
            /* Buffer full condition */
            if ((ui32ReadAddr ^ ui32WriteAddr) == 0x80000000)
            {
                depth = (ui32EndAddr - ui32BaseAddr) + 1;
            }
            else
            {
                ui32ReadAddr &= 0x7FFFFFFF;
                ui32WriteAddr &= 0x7FFFFFFF;
                
                depth = ui32WriteAddr - ui32ReadAddr;
                if (ui32WriteAddr < ui32ReadAddr)
                {
                    /* Return only the contiguous chunk size */
                    depth = (ui32EndAddr - ui32ReadAddr) + 1;
                }
            }
            
            break;
            
        case BDSP_AF_P_BufferType_eRDB:
            ui32BaseAddr = BREG_Read32(hReg, pBuffer->ui32BaseAddr);
            ui32EndAddr = BREG_Read32(hReg, pBuffer->ui32EndAddr);
            ui32ReadAddr = ui32ShadowRead;
            ui32WriteAddr = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            
            depth = ui32WriteAddr - ui32ReadAddr;
            if (ui32WriteAddr < ui32ReadAddr)
            {
                /* Return only the contiguous chunk size */
                depth = (ui32EndAddr - ui32ReadAddr);
            }
            break;
            
        case BDSP_AF_P_BufferType_eDRAM:
        default:
            ui32BaseAddr = pBuffer->ui32BaseAddr;
            ui32EndAddr = pBuffer->ui32EndAddr;
            ui32ReadAddr = ui32ShadowRead;
            ui32WriteAddr = pBuffer->ui32WriteAddr;
            
            depth = ui32WriteAddr - ui32ReadAddr;
            if (ui32WriteAddr < ui32ReadAddr)
            {
                /* Return only the contiguous chunk size */
                depth = (ui32EndAddr - ui32ReadAddr);
            }
            break;
    }
    
    return depth;
}

uint32_t BDSP_Raaga_P_GetAudioCaptureBufferFreeSpaceLinear(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer)
{
    uint32_t space, ui32BaseAddr, ui32EndAddr, ui32ReadAddr, ui32WriteAddr;

    ui32BaseAddr = pBuffer->ui32BaseAddr;
    ui32EndAddr = pBuffer->ui32EndAddr;
    ui32ReadAddr = pBuffer->ui32ReadAddr;
    ui32WriteAddr = pBuffer->ui32WriteAddr;
    
    space = ui32ReadAddr - ui32WriteAddr;
    if (ui32WriteAddr >= ui32ReadAddr)
    {
        space = ui32EndAddr - ui32WriteAddr;
    }
    
    return space;
}

bool BDSP_Raaga_P_DetectAudioCaptureError(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, 
    uint32_t ui32ShadowRead,
    uint32_t ui32LastWrite,
    BDSP_AF_P_BufferType eType, 
    BREG_Handle hReg)
{
    bool retval = false;
    uint32_t ui32WriteAddr;
    
    switch (eType)
    {
        case BDSP_AF_P_BufferType_eRAVE:
            ui32WriteAddr = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            break;
        case BDSP_AF_P_BufferType_eFMM:
            ui32WriteAddr = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            /* Clear the wrap bit */
            ui32WriteAddr &= 0x7FFFFFFF;
            ui32ShadowRead &= 0x7FFFFFFF;
            break;
        case BDSP_AF_P_BufferType_eRDB:
            ui32WriteAddr = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            break;
        case BDSP_AF_P_BufferType_eDRAM:
        default:
            ui32WriteAddr = pBuffer->ui32WriteAddr;
            break;
    }
    
    /* It is not possible to detect a capture error if shadow read and last write are the same */
    if (ui32ShadowRead == ui32LastWrite)
    {
        retval = false;
    }
    else
    {
        if (ui32ShadowRead > ui32LastWrite)
        {
            if ((ui32WriteAddr > ui32ShadowRead)
                || (ui32WriteAddr < ui32LastWrite))
            {
                retval = true;
            }
        }
        else
        {
            if ((ui32WriteAddr > ui32ShadowRead) 
                && (ui32WriteAddr < ui32LastWrite))
            {
                retval = true;
            }
        }
    }
    
    if (retval)
    {
        BDBG_MSG(("shadow = %x : write = %x : last wr = %x", ui32ShadowRead, ui32WriteAddr, ui32LastWrite));
    }
    
    return retval;
}


void BDSP_Raaga_P_GetUpdatedShadowReadAndLastWrite(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, 
    uint32_t *pShadowRead,
    uint32_t *pLastWrite,
    BDSP_AF_P_BufferType eType,
    uint32_t bytesRead,
    BREG_Handle hReg)
{
    uint32_t shadowRead;
    uint32_t base, end;
    
    shadowRead = *pShadowRead + bytesRead;
    
    switch (eType)
    {
        case BDSP_AF_P_BufferType_eRAVE:
            *pLastWrite = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            base = BREG_Read32(hReg, pBuffer->ui32BaseAddr);
            end = BREG_Read32(hReg, pBuffer->ui32WrapAddr);
            if (end == 0)
            {
                end = BREG_Read32(hReg, pBuffer->ui32EndAddr);
            }
            
            /* TBD: Need to verify if this is correct. Code borrowed from raaga test */
            if (shadowRead >= end)
            {
                shadowRead = base + (shadowRead - end);
            }
            break;
        case BDSP_AF_P_BufferType_eFMM:
            *pLastWrite = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            /* Clear the wrap bit */
            *pLastWrite &= 0x7FFFFFFF;
            base = BREG_Read32(hReg, pBuffer->ui32BaseAddr);
            end = BREG_Read32(hReg, pBuffer->ui32EndAddr);
            
            if ((shadowRead & 0x7FFFFFFF) > end)
            {
                shadowRead = base + (shadowRead - end) - 1;
                /* Flip bit 31 on a wrap */
                shadowRead ^= 0x80000000;
            }
            break;
        case BDSP_AF_P_BufferType_eRDB:
            *pLastWrite = BREG_Read32(hReg, pBuffer->ui32WriteAddr);
            base = BREG_Read32(hReg, pBuffer->ui32BaseAddr);
            end = BREG_Read32(hReg, pBuffer->ui32EndAddr);
            
            if (shadowRead >= end)
            {
                shadowRead = base + (shadowRead - end);
            }
            break;
        case BDSP_AF_P_BufferType_eDRAM:
        default:
            *pLastWrite = pBuffer->ui32WriteAddr;
            base = pBuffer->ui32BaseAddr;
            end = pBuffer->ui32EndAddr;
            if (shadowRead >= end)
            {
                shadowRead = base + (shadowRead - end);
            }
            break;
    }
    
    *pShadowRead = shadowRead;
}

void BDSP_Raaga_P_UpdateReadPointer(
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, 
    BDSP_AF_P_BufferType eType,
    uint32_t ui32ReadAddr,
    BREG_Handle hReg)
{
    switch (eType)
    {
        case BDSP_AF_P_BufferType_eRAVE:
        case BDSP_AF_P_BufferType_eFMM:
        case BDSP_AF_P_BufferType_eRDB:
            BREG_Write32(hReg, pBuffer->ui32ReadAddr, ui32ReadAddr);
            break;
        case BDSP_AF_P_BufferType_eDRAM:
        default:
            pBuffer->ui32ReadAddr = ui32ReadAddr;
            break;
    }
}


BERR_Code BDSP_Raaga_P_GetExternalInterruptInfo(void *pInterruptHandle, BDSP_ExternalInterruptInfo **pInfo)
{
	BERR_Code err=BERR_SUCCESS;
	BDSP_RaagaExternalInterrupt *pRaagaExtInterrupt = (BDSP_RaagaExternalInterrupt *)pInterruptHandle;
	BDBG_ENTER(BDSP_Raaga_P_GetExternalInterruptInfo);

	BDBG_OBJECT_ASSERT(pRaagaExtInterrupt, BDSP_RaagaExternalInterrupt);

	*pInfo = &pRaagaExtInterrupt->InterruptInfo;
	
	BDBG_LEAVE(BDSP_Raaga_P_GetExternalInterruptInfo);
	return err;
}
	
BERR_Code BDSP_Raaga_P_AllocateRdbRegisters(
		void *pDeviceHandle, 
		uint32_t dspIndex, 
		uint32_t numRegs, 
		BDSP_RdbRegisterHandle *pRdbRegisterHandle /* out */
		)
{
	BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;	  
	BERR_Code err=BERR_SUCCESS;
	BDSP_RaagaRdbRegister *pRaagaRdbRegister;
	int32_t	j, freeRegs = 0;

	BDBG_ENTER(BDSP_Raaga_P_AllocateRdbRegisters);

	BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);
	/* Allocate the RDB register handle first */
	pRaagaRdbRegister = BKNI_Malloc(sizeof(BDSP_RaagaRdbRegister));
	if ( NULL == pRaagaRdbRegister ) 
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BKNI_Memset(pRaagaRdbRegister, 0, sizeof(BDSP_RaagaRdbRegister));

	/* Success, add to device's rdb register list */
	BLST_S_INSERT_HEAD(&pDevice->rdbRegisterList, pRaagaRdbRegister, node);
	*pRdbRegisterHandle = &pRaagaRdbRegister->rdbRegister;
	
	/* Find a chunk of free Rdb Registers */
	BKNI_AcquireMutex(pDevice->rdbRegistersMutex[dspIndex]);
	for (j=0; j<BDSP_RAAGA_RDB_REGISTERS; j++)
	{
		/* Find a continuous spread of Rdb Registers */
		if (false == pDevice->rdbRegisters[dspIndex][j])
		{
			freeRegs++;
			if(freeRegs == (int32_t)numRegs)
				break;
		}
		else
		{
			freeRegs = 0;
		}
	}
	/* Find free register start index */
	freeRegs = j - numRegs + 1;
	if (j >= BDSP_RAAGA_RDB_REGISTERS)
	{
		BKNI_ReleaseMutex(pDevice->rdbRegistersMutex[dspIndex]);
		BDBG_ERR(("BDSP_Raaga_P_AllocateRdbRegisters: Unable to find free Rdb registers in dsp page!"));
		err = BERR_TRACE(BERR_NOT_SUPPORTED);
		goto err_rdb_alloc_fail;			 
	}
	else
	{
		for(;j >= freeRegs; j--)
		{
			pDevice->rdbRegisters[dspIndex][j] = true;
		}
	}
	BKNI_ReleaseMutex(pDevice->rdbRegistersMutex[dspIndex]);
	
	/* Got the interrupt bit. Populate interrupt handle now */
	BDBG_MSG(("BDSP_Raaga_P_AllocateRdbRegisters: Found free rdb register at index %x ", freeRegs));
	pRaagaRdbRegister->RdbRegisterInfo.addressOfBaseRdb = RAAGA_DSP_FW_CFG_PAGE_START + (freeRegs*4) + pDevice->dspOffset[dspIndex];
	pRaagaRdbRegister->dspIndex = dspIndex;
	pRaagaRdbRegister->numRegisters = numRegs;
	pRaagaRdbRegister->startIndex = freeRegs;
	pRaagaRdbRegister->rdbRegister.hDsp = &pDevice->device;
	pRaagaRdbRegister->rdbRegister.pRdbRegisterHandle = (void *)pRaagaRdbRegister;
	pRaagaRdbRegister->pDevice = pDevice;
	
	/* Done, mark objects as valid */
	BDBG_OBJECT_SET(pRaagaRdbRegister, BDSP_RaagaRdbRegister);
	BDBG_OBJECT_SET(&pRaagaRdbRegister->rdbRegister, BDSP_RdbRegister);
	goto end;

err_rdb_alloc_fail:	
	*pRdbRegisterHandle = NULL;
	BLST_S_REMOVE(&pDevice->rdbRegisterList, pRaagaRdbRegister, BDSP_RaagaRdbRegister, node);
	BKNI_Free(pRaagaRdbRegister);
	
end:
	BDBG_LEAVE(BDSP_Raaga_P_AllocateRdbRegisters);
	return err;

}

BERR_Code BDSP_Raaga_P_FreeRdbRegisters( 
	void  *pRdbRegisterHandle
	)
{
	BERR_Code err=BERR_SUCCESS;
	int32_t	j;
	BDSP_RaagaRdbRegister *pRaagaRdbRegister = (BDSP_RaagaRdbRegister *)pRdbRegisterHandle;
	BDSP_Raaga *pRaaga = pRaagaRdbRegister->pDevice;
	BDBG_ENTER(BDSP_Raaga_P_FreeRdbRegisters);

	BDBG_OBJECT_ASSERT(pRaagaRdbRegister, BDSP_RaagaRdbRegister);
	
	/* Free-up the allocated RDB */
	BDBG_MSG(("Freeing up %d registers from index %d", pRaagaRdbRegister->numRegisters, pRaagaRdbRegister->startIndex));
	for(j = 0; j < (int32_t)pRaagaRdbRegister->numRegisters; j++)
	{
		pRaaga->rdbRegisters[pRaagaRdbRegister->dspIndex][pRaagaRdbRegister->startIndex+j] = false;
	}

	/* Remove the handle from device's list */
	BLST_S_REMOVE(&pRaaga->rdbRegisterList, pRaagaRdbRegister, BDSP_RaagaRdbRegister, node);
	BKNI_Free(pRaagaRdbRegister);

	BDBG_LEAVE(BDSP_Raaga_P_FreeRdbRegisters);

	return err;
}


BERR_Code BDSP_Raaga_P_GetRdbRegistersInfo(
	void *pRdbRegisterHandle, 
	BDSP_RdbRegisterInfo **pInfo	/* out */
	)
{
	BERR_Code err=BERR_SUCCESS;
	BDSP_RaagaRdbRegister *pRaagaRdbRegister = (BDSP_RaagaRdbRegister *)pRdbRegisterHandle;
	BDBG_ENTER(BDSP_Raaga_P_GetRdbRegistersInfo);

	BDBG_OBJECT_ASSERT(pRaagaRdbRegister, BDSP_RaagaRdbRegister);

	*pInfo = &pRaagaRdbRegister->RdbRegisterInfo;
	
	BDBG_LEAVE(BDSP_Raaga_P_GetRdbRegistersInfo);
	return err;

}
BERR_Code BDSP_Raaga_P_SendScmCommand(	  
													void *pTaskHandle,	  
													BDSP_Raaga_P_SCM_CmdPayload *pScmCmdPayload
													)
{
		BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;  
		BERR_Code ret = BERR_SUCCESS;
		BDSP_Raaga_P_Command sCommand;
		BDSP_Raaga_P_Response sRsp;	  
		BDSP_RaagaContext *pRaagaContext = pRaagaTask->pContext;
		BDSP_Raaga *pDevice = pRaagaContext->pDevice;
		unsigned int uiResponseId;
		BDSP_Raaga_P_MsgType eMsgType;
		uint32_t i;
		bool anyScmSupported = false;
		
		BDBG_ENTER(BDSP_Raaga_P_SendScmCommand);
		BDBG_ASSERT(pTaskHandle);
	
		for (i=0; i<BDSP_ScmType_eMax; i++)
		{
			if (true == BDSP_Raaga_P_IsScmTypeSupported(i))
			{
				anyScmSupported = true;
				break;
			}
		}

		if (false == anyScmSupported)
		{
			BDBG_ERR(("Please export RAP_SCMx_SUPPORT=y at the compile time for appropriate SCM"));
			ret = BERR_TRACE(BERR_NOT_SUPPORTED);
			goto end;
		}	 
		
		BKNI_Memset(&sCommand,0,sizeof(BDSP_Raaga_P_Command));
	
		if (true == pRaagaTask->isStopped)
		{
			BDBG_ERR(("BDSP_Raaga_P_SendScmCommand: SCM Task = %p is not started", pRaagaTask));	
			ret = BERR_TRACE(BERR_INVALID_PARAMETER);
			goto end;
		}
		else
		{
			BKNI_Memset((void *)&sRsp,0,sizeof(BDSP_Raaga_P_Response));
			/*	BKNI_Memset((void *)&sScmStatusBuf,0xFF,sizeof(BRAP_FWIF_P_ScmStageStatus));	*/
	
			/* Create SCM FW command */
			sCommand.sCommandHeader.ui32CommandID = BDSP_RAAGA_BSP_SCM_COMMAND_ID;
			sCommand.sCommandHeader.ui32CommandCounter = pRaagaTask->commandCounter++;
			sCommand.sCommandHeader.ui32TaskID =  pRaagaTask->taskId;
			sCommand.sCommandHeader.eResponseType = BDSP_Raaga_P_ResponseType_eResponseRequired;
			uiResponseId = BDSP_RAAGA_BSP_SCM_COMMAND_RESPONSE_ID; 		   
		    sCommand.sCommandHeader.ui32CommandSizeInBytes =  sizeof(BDSP_Raaga_P_Command); 	
			/*One command is going to come at a time...App is doing the buffering if even it comes*/
	
			sCommand.uCommand.sScmCmd.sCmdBufferInfo.ui32DramCmdBufAddress = pScmCmdPayload->ui32DramCmdBufAddress;
			sCommand.uCommand.sScmCmd.sCmdBufferInfo.ui32DramRespBufAddress = pScmCmdPayload->ui32DramRespBufAddress;
			sCommand.uCommand.sScmCmd.sCmdBufferInfo.ui32DramCmdBufSize 	= pScmCmdPayload->ui32DramCmdBufSize;
			sCommand.uCommand.sScmCmd.sCmdBufferInfo.ui32DramRespBufSize	= pScmCmdPayload->ui32DramRespBufSize;
	
			pRaagaTask->lastEventType = sCommand.sCommandHeader.ui32CommandID;


			ret = BDSP_Raaga_P_SendCommand(pDevice->hCmdQueue[0], &sCommand, (void *)pRaagaTask);

			if(BERR_SUCCESS != ret)
			{
				if((pRaagaContext->contextWatchdogFlag == false)
				&&(pRaagaTask->isStopped == false))
				{
					BDBG_ERR(("BDSP_Raaga_P_SendScmCommand: Send_Command failed!"));
					ret = BERR_TRACE(ret);
					goto end;
				}
				else
					ret = BERR_SUCCESS;    
			}
	
			/* Wait for Response_Received event with timeout */
			ret = BKNI_WaitForEvent(pRaagaTask->hEvent, BDSP_RAAGA_EVENT_TIMEOUT_IN_MS);
					/* TODO: Ashok Add watchdog enable write above !! */

			if (BERR_TIMEOUT == ret)
			{
				
				if((pRaagaContext->contextWatchdogFlag == false ))
				{
					/* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
							code which needs to be debugged. However Watchdog is being triggered to recover the system*/			
					BDBG_WRN(("BDSP_Raaga_P_SendScmCommand: Triggering Watchdog"));
	
					/* BDSP_Write32(pDevice->regHandle, BCHP_AUD_DSP_INTH0_R5F_SET+ hDsp->ui32Offset,0x1); */
					/* TODO: Vijay Add watchdog enable write above !! */
					pRaagaContext->contextWatchdogFlag = true;
					ret = BERR_SUCCESS; 			 
				}
				else
					ret = BERR_SUCCESS; 			 
			}


					/* TODO: Ashok watchdog enable write above !! */

			eMsgType = BDSP_Raaga_P_MsgType_eSyn;
			
			if(pRaagaContext->contextWatchdogFlag == false)
			{
				ret = BDSP_Raaga_P_GetMsg(pRaagaTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);
			}
	
			if(BERR_SUCCESS != ret)
			{
				if((pRaagaContext->contextWatchdogFlag == false ))
				{
					BDBG_ERR(("BDSP_Raaga_P_SendScmCommand: Unable to read Response!"));
					ret = BERR_TRACE(ret);
					goto end;
				}
				else
					ret = BERR_SUCCESS; 			 
			}
			if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
				(sRsp.sCommonAckResponseHeader.ui32ResponseID != BDSP_RAAGA_BSP_SCM_COMMAND_RESPONSE_ID)||
				(sRsp.sCommonAckResponseHeader.ui32TaskID != pRaagaTask->taskId))
			{
				if((pRaagaContext->contextWatchdogFlag == false))
				{
				   BDBG_ERR(("BDSP_Raaga_P_SendScmCommand: RESPONSE not received successfully!eStatus = %d , ui32ResponseID = %d , ui32TaskID %d ",
						sRsp.sCommonAckResponseHeader.eStatus,sRsp.sCommonAckResponseHeader.ui32ResponseID,sRsp.sCommonAckResponseHeader.ui32TaskID));
					ret = BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
					goto end;
				}
				else
					ret = BERR_SUCCESS; 	  
			}	   

		
		}
	
	end:  
	
		BDBG_LEAVE(BDSP_Raaga_P_SendScmCommand);    
		return ret;

}

BERR_Code BDSP_Raaga_P_InitAudioCaptureInfo(BDSP_RaagaCapture *pRaagaCapture, const BDSP_AudioCaptureCreateSettings *pSettings)
{
    unsigned j;
    uint32_t memRequired;
	BERR_Code err = BERR_SUCCESS;
    void *captureMem;
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer;
    
	BDBG_ENTER(BDSP_Raaga_P_InitAudioCaptureInfo);
    
    BDBG_ASSERT(NULL != pRaagaCapture);
    
    pRaagaCapture->enabled = false;
    pRaagaCapture->maxBuffers = pSettings->maxChannels;

    /* Allocate memory for the worst case output */
    memRequired = pSettings->maxChannels*pSettings->channelBufferSize;
    
    /* Allocate the intermediate capture buffers */
    captureMem = BMEM_AllocAligned(pRaagaCapture->hHeap, (memRequired + 32), 5, 0);
    if (NULL == captureMem)
    {
        BDBG_ERR(("Unable to Allocate memory for Capture buffers !"));
        err = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        return err;
    }

    /* Split the allocated space into individual capture buffers */
    for (j = 0; j < pSettings->maxChannels; j++)
    {
        pBuffer = &pRaagaCapture->capPtrs[j].captureBufferPtr;
        pBuffer->ui32BaseAddr = (uint32_t)captureMem + j*pSettings->channelBufferSize;
        pBuffer->ui32ReadAddr = (uint32_t)captureMem + j*pSettings->channelBufferSize;
        pBuffer->ui32WriteAddr = (uint32_t)captureMem + j*pSettings->channelBufferSize;
        pBuffer->ui32EndAddr = (uint32_t)captureMem + (j+1)*pSettings->channelBufferSize;
        pBuffer->ui32WrapAddr = (uint32_t)captureMem + (j+1)*pSettings->channelBufferSize;
    }
    
    /* Reset the pointers for the rest of the buffers */
    for ( ; j < BDSP_AF_P_MAX_CHANNELS; j++)
    {
        pBuffer = &pRaagaCapture->capPtrs[j].captureBufferPtr;
        pBuffer->ui32BaseAddr = 0;
        pBuffer->ui32ReadAddr = 0;
        pBuffer->ui32WriteAddr = 0;
        pBuffer->ui32EndAddr = 0;
        pBuffer->ui32WrapAddr = 0;
    }
    
	BDBG_LEAVE(BDSP_Raaga_P_InitAudioCaptureInfo);
	return err;
}

BERR_Code BDSP_Raaga_P_GetAudioOutputPointers(BDSP_CIT_P_FwStgSrcDstDetails *pDstDetails, BDSP_RaagaCapture *pRaagaCapture)
{
    int i, numBuffers;
    uint32_t ui32Buff0AddrStart, ui32Buff2BuffOffset, ui32BuffId;
	BERR_Code err = BERR_SUCCESS;
    BDSP_AF_P_sIO_BUFFER *pIoBuff;
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *psCircBuffer;
   
	BDBG_ENTER(BDSP_Raaga_P_GetAudioOutputPointers);
    
    switch (pDstDetails->eType)
    {
		case BDSP_CIT_P_FwStgSrcDstType_eFMMBuf:
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
        	ui32Buff0AddrStart = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR;
        	ui32Buff2BuffOffset = BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR - BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR;
#else
            ui32Buff0AddrStart = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR;
            ui32Buff2BuffOffset = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_RDADDR - BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR;
#endif
            numBuffers = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers;
        	for(i = 0; i < numBuffers; i++)
        	{
        		ui32BuffId = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[i];
        		psCircBuffer = &pRaagaCapture->capPtrs[i].outputBufferPtr;
        		
        		psCircBuffer->ui32ReadAddr	= ui32Buff0AddrStart + ui32BuffId * ui32Buff2BuffOffset;
        		psCircBuffer->ui32WriteAddr = psCircBuffer->ui32ReadAddr + 4;
        		psCircBuffer->ui32BaseAddr	= psCircBuffer->ui32ReadAddr + 8;
        		psCircBuffer->ui32EndAddr	= psCircBuffer->ui32ReadAddr + 12;
        		psCircBuffer->ui32WrapAddr = psCircBuffer->ui32EndAddr;
        	}
            pRaagaCapture->eBuffType = BDSP_AF_P_BufferType_eFMM;
            break;
        
        case BDSP_CIT_P_FwStgSrcDstType_eRDB:
        	ui32Buff0AddrStart = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;
            ui32Buff2BuffOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;
            
            numBuffers = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers;
        	for(i = 0; i < numBuffers; i++)
        	{
        		ui32BuffId = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[i];
        		psCircBuffer = &pRaagaCapture->capPtrs[i].outputBufferPtr;
        		
        		psCircBuffer->ui32BaseAddr = ui32Buff0AddrStart + ui32BuffId * ui32Buff2BuffOffset;
        		psCircBuffer->ui32EndAddr = psCircBuffer->ui32BaseAddr + 4;
        		psCircBuffer->ui32WriteAddr = psCircBuffer->ui32BaseAddr + 8;
        		psCircBuffer->ui32ReadAddr = psCircBuffer->ui32BaseAddr + 12;
        		psCircBuffer->ui32WrapAddr = psCircBuffer->ui32EndAddr;
        	}
            pRaagaCapture->eBuffType = BDSP_AF_P_BufferType_eRDB;
            break;
            
        case BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf:
            BMEM_ConvertOffsetToAddress(pRaagaCapture->hHeap, pDstDetails->uDetails.sInterTaskDramBuffCfg.ui32IoBuffCfgAddr, (void **)&pIoBuff);
                
            numBuffers = pIoBuff->ui32NumBuffers;
        	for(i = 0; i < numBuffers; i++)
            {
        		psCircBuffer = &pRaagaCapture->capPtrs[i].outputBufferPtr;
                *psCircBuffer = pIoBuff->sCircBuffer[i];
            }

            pRaagaCapture->eBuffType = BDSP_AF_P_BufferType_eDRAM;
            break;
            
        case BDSP_CIT_P_FwStgSrcDstType_eRaveBuf:
            numBuffers = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers;
            
            for (i = 0; i < numBuffers; i++)
            {
        		psCircBuffer = &pRaagaCapture->capPtrs[i].outputBufferPtr;
                /* The rave pointers are programmed with the chip physical offset added. Masking the same */
                psCircBuffer->ui32BaseAddr = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32BaseAddr & 0x0FFFFFFF;
                psCircBuffer->ui32ReadAddr = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32ReadAddr & 0x0FFFFFFF;
                psCircBuffer->ui32WriteAddr = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32WriteAddr & 0x0FFFFFFF;
                psCircBuffer->ui32EndAddr = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32EndAddr & 0x0FFFFFFF;
                psCircBuffer->ui32WrapAddr = pDstDetails->uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[i].ui32WrapAddr & 0x0FFFFFFF;
            }
            
            pRaagaCapture->eBuffType = BDSP_AF_P_BufferType_eRAVE;
            break;
        default:
            BDBG_ERR(("Output Capture not supported for buffer type (%d)", pDstDetails->eType));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
            break;
    }
    
    pRaagaCapture->numBuffers = numBuffers;

    BDBG_LEAVE(BDSP_Raaga_P_GetAudioOutputPointers);

	return err;
}

