/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_vdec_info.h $
 * $brcm_Revision: Hydra_Software_Devel/97 $
 * $brcm_Date: 10/8/12 4:58p $
 *
 * File Description: This file definitions for PPB data structure passed
 *                   from 7401 decode FW to DM.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_vdec_info.h $
 * 
 * Hydra_Software_Devel/97   10/8/12 4:58p davidp
 * SW7445-16: Add support for 7445 platform.
 * 
 * Hydra_Software_Devel/96   9/21/12 4:07p davidp
 * SW7346-963: Update FW major version number for Rev K and later
 * decoders.
 * 
 * Hydra_Software_Devel/95   6/13/12 11:15a btosi
 * SW7425-3177: added support frame rates e10,e15,e20.  Reintroduced
 * BXVD_P_PPB_Protocol since the AVD and bavc.h protocol enums have
 * diverged.
 * 
 * Hydra_Software_Devel/94   2/27/12 5:15p davidp
 * SW7425-1680: Add support for early picture delivery mode.
 * 
 * Hydra_Software_Devel/93   2/10/12 11:18a btosi
 * SW7425-2247: add support for bar data
 * 
 * Hydra_Software_Devel/92   1/10/12 11:39a btosi
 * SW7425-1001: added flags_ext0 and BXVD_P_PPB_EXT0_FLAG_LAST_PICTURE
 * 
 * Hydra_Software_Devel/91   12/16/11 8:32a btosi
 * SWDTV-8681: add support for VP8 display size
 * 
 * Hydra_Software_Devel/90   7/27/11 11:32a btosi
 * SWDTV-7795: added PPB extensions for MP4, VP78 and RV9
 * 
 * Hydra_Software_Devel/89   6/27/11 4:13p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/8   6/13/11 2:43p btosi
 * SW7425-542: added support for new PPB definition
 * 
 * Hydra_Software_Devel/SW7425-542/7   6/9/11 4:20p davidp
 * SW7425-542: Move BXVD_P_PPB_EXTENDED to top of file.
 * 
 * Hydra_Software_Devel/SW7425-542/6   6/9/11 4:15p davidp
 * SW7425-542: Increase context memory size for extended PPB.
 * 
 * Hydra_Software_Devel/SW7425-542/5   6/9/11 3:57p davidp
 * SW7425-542: Add AVD FW current major version symbolic constant.
 * 
 * Hydra_Software_Devel/SW7425-542/4   6/9/11 1:19p btosi
 * SW7425-542: added in support for the Host Memory Interface
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/7/11 5:47p davidp
 * SW7425-542: Add BXVD_P_HIM_MAJOR_VERSION to validate correct FW is
 * being loaded.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/1/11 6:45p davidp
 * SW7425-542: Add BXVD_P_STC_MAX macro.
 * 
 * Hydra_Software_Devel/SW7425-542/1   5/31/11 3:58p davidp
 * SW7425-542: Add support for Host Interface Memory (ARC Scratch memory)
 * FW API changes.
 * 
 * Hydra_Software_Devel/88   3/15/11 10:17a btosi
 * SW7425-160: updated to use Broadcom standard types
 * 
 * Hydra_Software_Devel/87   2/24/11 1:55p slauzon
 * SW7572-277: Used local variable to restore the pic_out_id after "
 * Backend_Reset". Changed video reference frame register initialization.
 * First picture video buffer is used for initial video reference frame
 * initialization.
 * FWAVD-40: AVD does not support D-type pictures in MPEG. Set the
 * "unsupported feature flag" in the AVD channel status.
 * FWAVD-80: Added error identification and mitigation logic in AVD OL
 * firmware.(SW35230-2647)
 * VDEC_REL_0_21_3_47
 * 
 * Hydra_Software_Devel/86   2/17/11 2:02p btosi
 * SW7405-5135: if the SEI arrangement_cancel flag is set, set both the
 * Unified and MFD picture orientations to 2D
 * 
 * Hydra_Software_Devel/85   1/25/11 3:56p davidp
 * SW7422-22: Add support for SVC Picture Order Count.
 * 
 * Hydra_Software_Devel/84   11/3/10 10:29a slauzon
 * SW7125-633: mmco1 error logic changed for start of stream for
 * ePrognostic mode
 * SW7405-4910: fix btp cleanup for mpeg size change
 * SW7405-4953: increment reference count when creating sei3d packet
 * VDEC_REL_0_21_3_44
 * 
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/12   10/21/10 1:11p slauzon
 * SWCHECKFRAME-80: upleveled from hydra latest
 * 
 * Hydra_Software_Devel/83   10/19/10 10:23a btosi
 * SW7422-72: added constants for parsing the SEI messages
 * 
 * Hydra_Software_Devel/82   10/19/10 9:08a delkert
 * SW7405-4044: Add support for
 * BAVC_TransferCharacteristics_eIec_61966_2_4 to support xvYCC
 *
 * Hydra_Software_Devel/81   8/30/10 2:02p delkert
 * SW7405-4827: Add PPB Flag definition for bit 20 (error concealment)
 *
 * Hydra_Software_Devel/80   8/27/10 11:45a slauzon
 * SW3548-3033: Added a file format support for MKV where for back to back
 * SPS, we retain valid Aspect Ratio if the 2nd SPS has invalid  AR. This
 * change introduces a new channel_mode bit to enable this feature. The
 * api document updated accordingly.
 * SWBLURAY-21421: Merged MVC error-handling code to the main-line. Files
 * affected are outer\mvc: picture.c, sequence.c, mvc.h,channel.c.
 * No-PR: Remove FGT from H264 and MVC protocol. No one uses it.
 * SW3548-2967: Fixed Pan-scan bug which was corrupting channel core lsram
 * by overwriting pass the lsram PPB structure. Also, added a check at
 * PPB delivery time to make sure PPB is within a valid range. Otherwise
 * a WD is forced.
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/11   8/3/10 2:27p slauzon
 * SWCHECKFRAME-42: upleveled from Hydra
 *
 * Hydra_Software_Devel/79   7/29/10 11:22a btosi
 * SW7405-4560: added support for the SEI "new message" flag.
 *
 * Hydra_Software_Devel/78   7/27/10 11:36a btosi
 * SW7401-4426: added support for the H264 timecode
 *
 * Hydra_Software_Devel/77   5/10/10 12:18p slauzon
 * SW7325-721: Fix avc AFD extraction
 * SW7405-3996:  Add SEI 3D parsed frame-packing mode. This reuses the
 * offset meta data pointer in ppb structure and changes the offset meta
 * data structure to be a union with sei data.  This will require a
 * parallel XVD change in order not to break DVD mvc playback- Ray's team
 * are aware.
 * No JIRA: Change H264 Direct Context size to be 4 MB align.
 * Brickstone VOB label=VDEC_REL_0_21_3_36
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/10   5/10/10 11:02a slauzon
 * SW7405-3996: added support for new SEI message format
 *
 * Hydra_Software_Devel/76   5/4/10 10:41a btosi
 * SW7405-3996: added support for new SEI message format
 *
 * Hydra_Software_Devel/SW7405-3996/2   4/26/10 4:24p btosi
 * SW7405-3996: added BXVD_P_PPB_SEI_MSG_MVC_GRAPHICS_OFFSET
 *
 * Hydra_Software_Devel/SW7405-3996/1   4/1/10 3:59p btosi
 * SW7405-3996: first cut at generating picture extension callback from
 * SEI data
 *
 * Hydra_Software_Devel/75   3/5/10 11:58a slauzon
 * SW7400-2685: When BTP commands are sent across pictures that have
 * different sizes (such as a scene change or commercial), we must
 * release a stored-reference picture of a size that is no longer valid
 * as predicting from that picture is not accurate.
 *
 * FWAVD-16: Merge IP licensing support; to avoid any backward
 * compatibility issue, add IP enablement supports for 7405 family (7405,
 * 7413, 7205, 7206) in this release.
 *
 * SW3556-1058 : AVC decoder functionality is modified to parameterize
 * "dpb_output_delay" processing. By default the functionality is
 * enabled.
 *
 * VDEC_REL_0_21_3_32
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/9   3/4/10 3:01p mdanke
 * SWCHECKFRAME-42: upleveled from Hydra
 *
 * Hydra_Software_Devel/74   3/3/10 5:28p davidp
 * SW7400-2704: Add SEI message frame packing support.
 *
 * Hydra_Software_Devel/73   1/19/10 12:40p slauzon
 * SW7405-3749: Add check for un-initialized pointer for abandon picture
 * else it could cause an memory corrupt. SW7400-2499: Fixed MPEG-2 P-
 * skip MB with no prior reference frame.
 * Release v0.21.3.29 (Brickston VOB label=VDEC_REL_0_21_3_29)."
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/8   1/18/10 2:40p slauzon
 * SWCHECKFRAME-42: rebase merge from Hydra_Software_Devel
 *
 * Hydra_Software_Devel/72   1/8/10 11:50a btosi
 * SW7405-3137: added support for extension data callback
 *
 * Hydra_Software_Devel/71   1/6/10 2:28p btosi
 * SW7405-3137: merged branch into mainline
 *
 * Hydra_Software_Devel/70   12/23/09 12:54p slauzon
 * SW7601-180 : Issue with Promise DVD. Pull down information was miscoded
 * in the stream. Passed the "repeat_first_flag" to the Display manager
 * through PPB_MPEG structure.
 * Add sleep command for Flea low-power mode.
 * Reset sparse mode picture processing variable correctly.
 * VDEC_REL_0_21_3_28.
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/7   12/16/09 4:46p slauzon
 * SWCHECKFRAME-42: upleveled dev branch from Hydra
 *
 * Hydra_Software_Devel/69   12/16/09 4:14p btosi
 * SW7601-180: added BXVD_MPEGPulldownOverride routines/logic to
 * conditionally override TB->TBT and BT->BTB
 *
 * Hydra_Software_Devel/68   11/18/09 10:40a slauzon
 * SW7400-2466: (DRAM logging)  Debug ArcPrintf in DRAM logging command
 * need to happen AFTER Dram logging setup because ArcPrintf uses DRAM
 * logging.
 *
 * Added support to output pictures in decode-order if the appropriate
 * flag is set in Channel_Start command. This mode should only be used
 * internally for testing. The following codecs were changed: MPEG2, VC1,
 * DivX and AVS.
 *
 * SW7405-3322: Fix IL code space corruption issue. In error concealment
 * code, FW intends to write to scratch memory to initialize the direct
 * context data content and accidently forget to add the IO_BASE addres
 * that cause data write into code space.
 *
 * SW7405-3035: Adding support in AVC to start decode on I picture when a
 * channel_mode bit is set.
 *
 * SW3556-836: Adding fixed frame rate flag in avc. Note that shared.h was
 * also changed, hence bxvd_vdec_info needs to add the new flag
 *
 * Patch last MB of MPEG4 I frame followed by AVC I frame in mosaic mode
 * cause decode error issue. Fix only is only enabled at rev J core and
 * below.
 *
 * SW7420-421: DNR error message while playback MP4. Current MP4 FW uses
 * only 16-bit to accumulate QP and overflow the data due to high QP and
 * MB count.
 *
 * SW7405-2844: XVD error handling issues. Additional checks for gap in
 * "frame_num" and errors in mmco commands were added and reported to
 * display manager for each picture delivered.
 *
 * SW7601-173: Macro-blocking on Sunshine BD. AVC error concealment is off
 * by default. Application can turn it on as required.
 *
 * VDEC_REL_0_21_3_26.
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/6   11/14/09 3:05p slauzon
 * SWCHECKFRAME-42: rebase devel branch
 *
 * Hydra_Software_Devel/67   11/11/09 3:35p btosi
 * SW3556-836: added support for returning the PPB fixed_frame_rate_flag
 * in the BXVD_PictureParameterInfo structure
 *
 * Hydra_Software_Devel/66   10/20/09 10:56a slauzon
 * SW7401-4223: Update the drop_count reported to XVD in the PPB structure
 * with the latched drop count only if the value is non-zero. We were
 * updating this value at every RAP and sometimes, the latched drop count
 * of zero was being reported to XVD causing XVD to repeatedly make drop
 * requests. This behaviour resulted in lipsync issues, especially in
 * streams with errors. Note that this behaviour was only seen with DM
 * v1, which handles drop requests differently than DM v2.
 *
 * No JIRA number: Add flea specific api to drop fields (flea does not
 * have VSync so current mechanism for passing drop count would not
 * work). API is not currently published to XVD and therefore does not
 * require an increase in minor release number.
 *
 * No JIRA number :: Added AVC in AVI container functionality.
 *
 * SW7405-3052: AVD inner loop watchdog popup with customer provided
 * stream. The inner watchdog is caused by the IL take too long during
 * error re-recovering; IL did not lock up in this case. I disable the IL
 * WD and see it running fine. This is a HD streams with 3 slice coded
 * per row, in case of error, FW need to do a start code search (byte
 * search) to locate the next slice boundary and resume decode, plus the
 * error recovering code some time it exceed the watchdog timer budget. A
 * suggest fix is to add WD reset during MPEG2 slice start code search.
 *
 * No PR:to be able to decode Base view of MVC streams, we need to chagne
 * the reserved zero flags in SPS to only parse out 3 bits instead of 4
 * bits.
 *
 * No PR: For DVD chips after Grain, the sixth bit used in the
 * picture/video ID was taken away to be used for motion-compensation
 * cache. As a result, video ID can only be in the range of 0-31. In
 * prior chips, the sixth bit was used by the CCB to determine whether
 * the video ID address was in memory bank A or B. Now, because there is
 * no sixth bit, CCB will not be able to "steer" the video ID to bank A
 * or B. Instead, all the picture buffers will either be in bank A or
 * bank B but not in both at the same time. To enable steering again,
 * hardware needs to assign the sixth bit back to video ID and we should
 * program video_id_offset = 32   In Grain, in order to use memory bank
 * B, we must set video_id_offset to 0 because a video ID > 32 will set
 * the sixth bit, which will cause the memory controller to interpret the
 *
 * SW7405-3005:  Add overflow bit to PPB flags and status.
 *
 * VDEC_REL_0_21_3_25.
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/5   10/15/09 10:10a slauzon
 * SW7405-3005: added "uiDecoderInputOverflow" i.e. reporting of CDB
 * overflow
 *
 * Hydra_Software_Devel/65   10/13/09 11:22a btosi
 * SW7405-3005: added "uiDecoderInputOverflow" i.e. reporting of CDB
 * overflow
 *
 * Hydra_Software_Devel/63   9/14/09 3:42p slauzon
 * SW7405-2844 :
 *
 *   No PR: for Cabac programc lip L0 and L1 count to 2. This is needed
 * for SVC bu
 * t to avoid a branch this cahnge can be applied to mainline with any
 * side effect
 *
 *   No PR: Misc. code changes. (1) Preserve the work around for the
 * possible top
 * context corruption in SD only box (w SD IL-ARC RTS setting).  This is a
 * HW bug a
 * nd is fixed in Rev J.  Due to the performance impact, this work around
 * is disabl
 * e for now. (2) Remove dummy instrcution in VC1 OL FW.
 *
 *   PR 56809:  Fixes for OL TPD and Output-N processing
 *
 *   PR 57490 :: Modifications to code to implement BXVD_ERROR_HANDLING
 * functional
 * ity appropriately, especially for "eprognostic" mode.
 *
 *   VDEC_REL_0_21_3_23.
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/4   9/1/09 12:55p slauzon
 * SW7405-2961: merged back from Hydra_Software_Devel to resolve build
 * issue on AVD_Qual apps.
 *
 * Hydra_Software_Devel/62   8/28/09 10:15a btosi
 * SW7405-2940: add BXVD_P_PPB_FLAG_DECODE_REF_ERROR
 *
 * Hydra_Software_Devel/61   7/31/09 11:04a nilesh
 * PR57256: Changed 7.943 frame rate to 7.493
 *
 * Hydra_Software_Devel/60   3/10/09 1:53p nilesh
 * PR52427: Removed unused MPEG PPB flag defines
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/2   3/4/09 1:27p mdanke
 * PR 48843: Added IL instruction cache miss variable to PPB.
 *
 * Hydra_Software_Devel/XVD_Multi_Firmware_Devel/1   2/11/09 6:05p gauravs
 * PR13053: Added overscan bits for AVC stream to vdec_info.h
 *
 * Hydra_Software_Devel/57   1/12/09 11:22a gaurava
 * PR 48843: Added a new variable for CABAC cycle count that would be
 * used for Rev J RTL and beyond for measuring hardware decode
 * performance for CABAC.
 *
 * Hydra_Software_Devel/56   11/25/08 1:48p gaurava
 * PR48843: moved the delta_pic_seen down by a word to leave
 * room for future inner-loop performance monitoring.
 *
 * Hydra_Software_Devel/55   11/21/08 11:50a btosi
 * PR48843: added in delta_pic_seen
 *
 * Hydra_Software_Devel/54   11/5/08 12:50p gaurava
 * PR 47494: Added a bit in ppb->flags to tell the DM whenever a picture
 * has a new picture_tag as opposed to a sticky picture tag.
 *
 * Hydra_Software_Devel/53   11/3/08 10:11a gaurava
 * PR 43785: Added data structure for inner-loop performance monitoring.
 *
 * Hydra_Software_Devel/52   10/20/08 5:14p davidp
 * PR46567: Extract profile and level data out of proper PPB field.
 *
 * Hydra_Software_Devel/51   10/16/08 3:19p gaurava
 * PR 43785: Modified the poc_top and poc_bottom to be signed. These can
 * be negative and there are some streams where they are.  We need to use
 * the lower of the two and a comparison of poc values would be incorrect
 * if declared as unsigned.
 *
 * Hydra_Software_Devel/50   10/6/08 11:49a btosi
 * PR46567: removed BXVD_P_PPB_Protocol, BXVD_P_PPB_Protocol_Level and
 * BXVD_P_PPB_Protocol_Profile
 *
 * Hydra_Software_Devel/49   10/3/08 3:47p btosi
 * PR46567: added BXVD_P_PPB_Protocol, BXVD_P_PPB_Protocol_Level and
 * BXVD_P_PPB_Protocol_Profile
 *
 * Hydra_Software_Devel/48   7/31/08 1:56p btosi
 * PR41123: added BXVD_P_PPB_FLAG_PICTURE_TYPE_MASK
 *
 * Hydra_Software_Devel/47   7/30/08 10:29a btosi
 * PR41123: added picture type constants
 *
 * Hydra_Software_Devel/46   6/4/08 3:18p gaurava
 * PR 42134: Added parameter for AFD value from the stream for Active
 * Format Descriptors
 *
 * Hydra_Software_Devel/45   5/27/08 11:12a gaurava
 * PR 43036: Added parameter in PPB to forward the inner-loop decode
 * time to the Host for gathering performance statistics
 *
 * Hydra_Software_Devel/44   3/17/08 5:28p davidp
 * PR40316: Add BXVD_P_PPB_FLAG_BUFFER_TYPE_MASK and
 * BXVD_P_PPB_FLAG_SOURCE_TYPE_MASK for access to these items in ppb-
 * >flags.
 *
 * Hydra_Software_Devel/43   1/11/08 2:03p gaurava
 * PR 37223: Added AVS Pan-Scan vectors
 *
 * Hydra_Software_Devel/42   1/10/08 5:31p otao
 * PR37223: Added panscan support in AVS.
 *
 * Hydra_Software_Devel/41   11/15/07 4:54p gaurava
 * PR 34981: Added PPB extensions for AVS
 *
 * Hydra_Software_Devel/40   10/22/07 2:23p gaurava
 * PR 35408: Added new parameters to PPB for DigitalTV customers.
 * Also added the absolute address for base of 8x8 block averages
 * buffer useful for FGT in BVN.
 *
 * Hydra_Software_Devel/39   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/8   7/25/07 1:37p nilesh
 * PR29915: Cleaned up bxvd_priv.h and bxvd_vdec_info.h constants to match
 * XVD coding style
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/6   7/10/07 10:41a nilesh
 * PR29915: Merged from mainline 07/10/2007
 *
 * Hydra_Software_Devel/37   6/28/07 10:32a gaurava
 * PR 32432: Added bit in ppb->flags to tell XVD if a picture was
 * decoded with errors.
 * Release v7.3.18
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/5   7/3/07 3:47p nilesh
 * PR29915: bxvd_priv.h cleanup
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/4   6/19/07 12:13p nilesh
 * PR29915: Merge from mainline
 *
 * Hydra_Software_Devel/36   5/22/07 1:27p gaurava
 * PR 29143: Added second stc snapshot and removed H263 specific
 * PPB.
 *
 * Hydra_Software_Devel/35   3/28/07 4:37p gaurava
 * PR 29143: Added flag to PPB for picture_tag validity
 *
 * Hydra_Software_Devel/34   3/19/07 12:18p gaurava
 * PR 28629: Added MPEG GopTimeCode to PPB
 * Also modified the FGT data structs
 *
 * Hydra_Software_Devel/33   1/2/07 2:24p gaurava
 * PR 26868: Added picture_tag support through BTP command. Also added ITB
 * termination
 * support for single-pass OTF.
 * Release v7.3.2
 *
 * Hydra_Software_Devel/32   12/12/06 3:03p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 *
 * Hydra_Software_Devel/31   11/8/06 4:58p gaurava
 * PR 22362: Added two new aspect ratio enum values as per new ISO
 * spec received in email from Wesley Li.
 *
 * Hydra_Software_Devel/30   10/17/06 3:07p gaurava
 * PR 24969: Modified the PPB definition to include a time_marker field
 * that will be used to send time information synchronous with the stream.
 *
 * Hydra_Software_Devel/29   10/11/06 6:20a gaurava
 * PR 24539: Added range remapping information in VC-1 PPB
 *
 * Hydra_Software_Devel/28   9/14/06 2:51p gaurava
 * PR 24258: Cleaned the AVD header file.
 *
 * Hydra_Software_Devel/27   9/5/06 3:05p davidp
 * PR22867: Merge secure heap (Cabac) support
 *
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/30/06 5:12p davidp
 * PR22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 *
 * Hydra_Software_Devel/26   8/23/06 3:09p gaurava
 * PR 23787: Added picture_tag to MPEG PPB for BUD trick modes.
 *
 * Hydra_Software_Devel/25   6/16/06 5:48p gaurava
 * PR 20017: PPB changes to align with new B0 firmware
 *
 * Hydra_Software_Devel/23   5/17/06 2:07p gaurava
 * PR 20595: Fixed MPEG brcm trick modes. Also fixed DNR parameter
 * extraction and a few H264 CRC failures related to RTS-limits on SI
 * coeff generator.
 *
 * Hydra_Software_Devel/22   5/3/06 10:46a gaurava
 * PR 17879: Changes for DNR feature-seq hdr and gop hdr.
 *
 * Hydra_Software_Devel/21   5/1/06 3:20p davidp
 * PR20353: Checkin AVD FW to get latest 97401 FW changes that fix last
 * slice not being decoded
 *
 * Hydra_Software_Devel/20   4/25/06 3:21p gaurava
 * PR 18710: Added drop-at-decode feature in firmware.
 *
 * Hydra_Software_Devel/19   4/24/06 4:37p davidp
 * PR18043: Add support for Sequence Header interrupt processing
 *
 * Hydra_Software_Devel/18   4/10/06 6:18p gaurava
 * PR 20647: Fixed the crash of MIPS after channel change by reducing size
 * of the PPB
 * that caused scratch mem of outer-loop to get corrupted.
 *
 * Hydra_Software_Devel/17   3/31/06 3:34p gaurava
 * PR 18749: Improvements in AVC RTS performance. PR 17879: Added DNR
 * parameter extraction for MPEG-2. Added little-endian support through
 * API, fixed PR 19485 for MPEG non-compliance stream
 *
 * Hydra_Software_Devel/16   3/15/06 5:15p gaurava
 * PR 19749: Added a flag from DM to DMS to inform if pictures are running
 * late.
 *
 * Hydra_Software_Devel/15   3/6/06 2:55p gaurava
 * PR 19436: Added new elements for picture coding type and sequence
 * header interrupt.
 *
 * Hydra_Software_Devel/14   2/15/06 1:56p pblanco
 * PR18545: Extended user data types for 7401/7400 in the same manner Mai
 * did for 7411.
 *
 * Hydra_Software_Devel/13   2/6/06 1:14p gaurava
 * PR 19106: Fixed the header file change for the extended aspect_ratio
 * that made the DM read the PPB incorrectly, leading to scaled video in
 * the top-left.
 *
 * Hydra_Software_Devel/12   2/3/06 10:33p vijeth
 * PR 19106 : Added the Extended A/R support in DM
 *
 * Hydra_Software_Devel/11   12/1/05 1:41p pblanco
 * PR16052: Changed definition of UD_HDR to make it look like
 * BXVD_P_Userdata.
 *
 * Hydra_Software_Devel/10   11/18/05 8:10p ssavekar
 * PR 17811: Need idr_pic_id and PicOrderCnt with CRC values
 * Adding code and data structure fields for passing CRC related
 * parameters to VDC.
 *
 * Hydra_Software_Devel/9   10/26/05 9:21a davidp
 * PR16052: Fix compiler warnings:
 *
 * Hydra_Software_Devel/8   10/20/05 5:26p pblanco
 * PR16052: Removed all C++ style comments
 *
 * Hydra_Software_Devel/7   10/14/05 7:00p ssavekar
 * PR 16052: New check-in for XVD
 * Changes to get the code in sync with latest "vdec_info.h" file.
 *
 * Hydra_Software_Devel/7   10/14/05 6:59p ssavekar
 * PR 16052: New check-in for XVD
 * Changes to get the code in sync with latest "vdec_info.h" file.
 *
 * Hydra_Software_Devel/6   10/7/05 3:31p pblanco
 * PR16052: Added definitions for more MPEG user data types.
 *
 * Hydra_Software_Devel/5   9/30/05 8:54p ssavekar
 * PR 16052: New check-in for XVD
 * Added a few elements in PPB data structures that were missing in the
 * "bxvd" directory because of which display horizontal and vertical
 * sizes were being read wrong from DM for MPEG.
 *
 * Hydra_Software_Devel/4   9/8/05 4:53p ssavekar
 * PR 16052: New check-in for XVD
 * Changes for DM integration with XVD and FW.
 *
 * Hydra_Software_Devel/3   8/2/05 5:24a ssavekar
 * PR 16052: New check-in for XVD
 * Changes after a code walk-through.
 *
 * Hydra_Software_Devel/1   7/29/05 9:05p ssavekar
 * PR 16052: New check-in for XVD
 * This file holds all the information about data structures passed from
 * 7401 decode FW to DM and the data structures passed from 7401 DM to
 * decode FW.
 *
 *
 ***************************************************************************/

#ifndef __INC_VDEC_INFO_H__
#define __INC_VDEC_INFO_H__

/* version information for PPB and other ancilliary data */
#define VDEC_INFO_VERSION               1.8

/* stripe-width used for output of decoded pixels */
#define VIDEO_STRIPE_WIDTH              64

/* buffer alignment required by hardware for luma, chroma in bytes */
#define FRAME_BUFFER_ALIGNMENT          (4 * 1024)


#if BXVD_P_FW_HIM_API

#define BXVD_P_PPB_EXTENDED 1

#endif

/*
 * SW7405-3996:: Constant and structure definitions for SEI messages.
 */

/*
 *  Types of SEI messages
 */
#define  BXVD_P_PPB_SEI_MSG_FRAMEPACKING           0x2D
#define  BXVD_P_PPB_SEI_MSG_MVC_GRAPHICS_OFFSET    0x80


/* Graphics offset message */

#define BXVD_P_MAX_MVC_OFFSET_META_SEQ 32

typedef struct
{
    int32_t         size;
    unsigned char   offset[BXVD_P_MAX_MVC_OFFSET_META_SEQ];
} BXVD_P_MVC_Offset_Meta;


/* Frame packing message */

typedef struct
{
   uint32_t flags;
   uint32_t frame_packing_arrangement_id;
   uint32_t frame_packing_arrangement_type;
   uint32_t content_interpretation_type;
   uint32_t frame0_grid_position_x;
   uint32_t frame0_grid_position_y;
   uint32_t frame1_grid_position_x;
   uint32_t frame1_grid_position_y;
   uint32_t frame_packing_arrangement_reserved_byte;
   uint32_t frame_packing_arrangement_repetition_period;

} BXVD_P_SEI_FramePacking;

/* Constants for interpreting the "flags" field in BXVD_P_SEI_FramePacking
 *
 * "arrangement_cancel":
 * The H264 specification was not completely rigorous in the definition of this bit.  We are assuming 
 * that if this bit is set, none of the other fields in the message are valid.   This then implies 
 * that the picture is a 2D picture.  The orientation will be set to "2D" in both the Unified and
 * MFD picture structures. (SW7405-5135)
 */
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_ARRANGEMENT_CANCEL          (0x00000001)      /* if set, implies 2D */
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_QUINCUNX_SAMPLING           (0x00000002)
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_SPATIAL_FLIPPING            (0x00000004)
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_FRAME0_FLIPPED              (0x00000008)
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_FIELD_VIEWS                 (0x00000010)
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_CURRENT_FRAME_IS_FRAME0     (0x00000020)
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_FRAME0_SELF_CONTAINED       (0x00000040)
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_FRAME1_SELF_CONTAINED       (0x00000080)
#define BXVD_P_PPB_FLAG_SEI_FRAMEPACK_ARRANGEMENT_EXTENSION       (0x00000100)


/* Constants for interpreting the "frame_packing_arrangement_type" field in BXVD_P_SEI_FramePacking
 */
#define BXVD_P_PPB_SEI_FRAMEPACK_TYPE_CHECKER        0
#define BXVD_P_PPB_SEI_FRAMEPACK_TYPE_COLUMN         1
#define BXVD_P_PPB_SEI_FRAMEPACK_TYPE_ROW            2
#define BXVD_P_PPB_SEI_FRAMEPACK_TYPE_SIDE_BY_SIDE   3
#define BXVD_P_PPB_SEI_FRAMEPACK_TYPE_OVER_UNDER     4
#define BXVD_P_PPB_SEI_FRAMEPACK_TYPE_ALTERNATING    5

/* Constants for interpreting the "content_interpretation_type" field in BXVD_P_SEI_FramePacking
 */
#define BXVD_P_PPB_SEI_FRAMEPACK_INTERPRET_UNSPECIFIED  0
#define BXVD_P_PPB_SEI_FRAMEPACK_INTERPRET_FRAME0_LEFT  1
#define BXVD_P_PPB_SEI_FRAMEPACK_INTERPRET_FRAME0_RIGHT 2

/* Generic wrapper for messages delivered by AVD */

typedef struct BXVD_P_SEI_Message
{
   uint32_t uiMsgType;

   /* Pointer to next message if sent. */

   struct BXVD_P_SEI_Message * pstNextSEIMsgOffset;

   union
   {
      BXVD_P_MVC_Offset_Meta  stOffsetMeta;
      BXVD_P_SEI_FramePacking stSEIFramePacking;

   } data;

}  BXVD_P_SEI_Message;

/*
 * end:: SW7405-3996 add SEI messages
 */

/* SW7425-2247: constants for interpreting the "bar_data" field
 * 
 * bits 13:0   top or left Bar Data
 * bits 15:14  unused
 * bits 29:16  bottom or right Bar Data
 * bit 30      1 means Bar data is Top_Bottom, 0 means Bar Data is Left_Right
 * bit 31      1 means Bar data is valid, 0 means invalid
 */
#define BXVD_P_PPB_BAR_DATA_TOP_LEFT_MASK    (0x00003FFF)
#define BXVD_P_PPB_BAR_DATA_TOP_LEFT_SHIFT   0

#define BXVD_P_PPB_BAR_DATA_BOT_RIGHT_MASK   (0x3FFF0000)
#define BXVD_P_PPB_BAR_DATA_BOT_RIGHT_SHIFT  16

#define BXVD_P_PPB_BAR_DATA_FLAG_TOP_BOTTOM  (0x40000000)
#define BXVD_P_PPB_BAR_DATA_FLAG_VALID       (0x80000000)


/* User Data Header */
typedef struct user_data
{
   struct user_data  *next;
   unsigned long      type;
   unsigned long      size;
} UD_HDR;

/*------------------------------------------------------*
 *    MPEG Extension to the BXVD_P_PPB                  *
 *------------------------------------------------------*/
#define  BXVD_P_PPB_MPEG_USERDATA_TYPE_SEQ           (1)
#define  BXVD_P_PPB_MPEG_USERDATA_TYPE_GOP           (2)
#define  BXVD_P_PPB_MPEG_USERDATA_TYPE_PIC           (4)
#define  BXVD_P_PPB_MPEG_USERDATA_TYPE_TOP           (8)
#define  BXVD_P_PPB_MPEG_USERDATA_TYPE_BOT           (16)
#define  BXVD_P_PPB_MPEG_USERDATA_TYPE_I             (32)
#define  BXVD_P_PPB_MPEG_USERDATA_TYPE_P             (64)
#define  BXVD_P_PPB_MPEG_USERDATA_TYPE_B             (128)

/* GOP time code field extraction */
#define BXVD_P_PPB_MPEG_GOP_HOUR_MASK    0xf80000
#define BXVD_P_PPB_MPEG_GOP_MINUTE_MASK  0x7e000
#define BXVD_P_PPB_MPEG_GOP_SECOND_MASK  0xfc0
#define BXVD_P_PPB_MPEG_GOP_PICTURE_MASK 0x3f
#define BXVD_P_PPB_MPEG_GOP_HOUR_SHIFT   0x13
#define BXVD_P_PPB_MPEG_GOP_MINUTE_SHIFT 0x0d
#define BXVD_P_PPB_MPEG_GOP_SECOND_SHIFT 0x06

typedef struct
{
   uint32_t           pictype;   /* pict_coding_type (I=1, P=2, B=3) */

   /* Always valid,  defaults to picture size if no
      sequence display extension in the stream. */
   uint32_t      display_horizontal_size;
   uint32_t      display_vertical_size;

   /* MPEG_VALID_PANSCAN
      Offsets are a copy values from the MPEG stream. */
   uint32_t      offset_count;
   int32_t       horizontal_offset[3];
   int32_t       vertical_offset[3];

   /* GOP time code [23:19]=hour, [18:13]=minute, [11:6]=sec, [5:0]=frame */
   uint32_t gop_time_code;

   uint32_t      bit_rate_value;         /* bit-rate from sequence header */
   uint32_t      low_delay_video_format; /* [17:16]=low_delay, [2:0]=video_format */
   uint32_t      frame_rate_extension;   /* [6:5]=frame_rate_extn_n, [4:0]=frame_rate_extn_d */
   unsigned char     repeat_first_field;    /*  MPEG-2 repeat first flag */
   unsigned char     reserved[3];

} BXVD_P_PPB_MPEG;


/*------------------------------------------------------*
 *    VC1 Extension to the PPB                          *
 *------------------------------------------------------*/
#define  BXVD_P_PPB_VC1_VALID_PANSCAN             (1)
#define  BXVD_P_PPB_VC1_VALID_USER_DATA           (2)
#define  BXVD_P_PPB_VC1_USER_DATA_OVERFLOW        (4)

#define  BXVD_P_PPB_VC1_USERDATA_TYPE_SEQ        (1)
#define  BXVD_P_PPB_VC1_USERDATA_TYPE_ENTRYPOINT (2)
#define  BXVD_P_PPB_VC1_USERDATA_TYPE_FRM        (4)
#define  BXVD_P_PPB_VC1_USERDATA_TYPE_FLD        (8)
#define  BXVD_P_PPB_VC1_USERDATA_TYPE_SLICE      (16)

typedef struct
{
   /* Always valid,  defaults to picture size if no
      sequence display extension in the stream. */
   uint32_t      display_horizontal_size;
   uint32_t      display_vertical_size;

  /* VC1 pan scan windows
   */
   uint32_t      num_panscan_windows;
   uint32_t      ps_horiz_offset_width[4]; /* bits[31:14]=horiz_offset, [13:0]=width */
   uint32_t      ps_vert_offset_height[4]; /* bits[31:14]=vert_offset, [13:0]=height */

   uint32_t      range_remapping_ratio;    /* expansion/reduction information */

} BXVD_P_PPB_VC1;


/*------------------------------------------------------*
 *    AVS Extension to the BXVD_P_PPB                   *
 *------------------------------------------------------*/
#define  BXVD_P_PPB_AVS_VALID_PANSCAN             (1)
#define  BXVD_P_PPB_AVS_VALID_USER_DATA           (2)

#define  BXVD_P_PPB_AVS_USER_DATA_TYPE_SEQ        (1)
#define  BXVD_P_PPB_AVS_USER_DATA_TYPE_FRM        (2)
#define  BXVD_P_PPB_AVS_USER_DATA_OVERFLOW        (4)

typedef struct
{
   /* Always valid,  defaults to picture size if no
      sequence display extension in the stream. */
   uint32_t      display_horizontal_size;
   uint32_t      display_vertical_size;

   /* AVS_VALID_PANSCAN
      Offsets are a copy values from the AVS stream. */
   uint32_t      offset_count;
   int32_t       horizontal_offset[3];
   int32_t       vertical_offset[3];

} BXVD_P_PPB_AVS;

/*------------------------------------------------------*/
/*    H.264 Extension to the BXVD_P_PPB                 */
/*------------------------------------------------------*/

/* userdata type flags */
#define BXVD_P_PPB_H264_USERDATA_TYPE_REGISTERED      4
#define BXVD_P_PPB_H264_USERDATA_TYPE_TOP             8
#define BXVD_P_PPB_H264_USERDATA_TYPE_BOT            16
#define BXVD_P_PPB_H264_USERDATA_TYPE_FRAME_PACK     45

/**
 * @brief Film grain SEI message.
 *
 * Content of the film grain SEI message.
 */
/*maximum number of model-values as for Thomson spec(standard says 5)*/
#define MAX_FGT_MODEL_VALUE         (3)
/* maximum number of intervals(as many as 256 intervals?) */
#define MAX_FGT_VALUE_INTERVAL      (256)

/* This is 3+3+(9*256)+(3*256)+(3*256) */
#define MAX_FGT_VALS       3846

/* Bit definitions for 'other.h264.valid' field */
#define  BXVD_P_PPB_H264_VALID_PANSCAN             (1)
#define  BXVD_P_PPB_H264_VALID_SPS_CROP            (2)
#define  BXVD_P_PPB_H264_VALID_VUI                 (4)

/* SW7401-4426: Bit defintions for the "time_code" field
 * [24:20]=hour, [19:14]=minute, [13:8]=sec, [7:0]=frame
 */
#define BXVD_P_PPB_H264_GOP_HOUR_MASK    0x01F00000
#define BXVD_P_PPB_H264_GOP_MINUTE_MASK  0x000FC000
#define BXVD_P_PPB_H264_GOP_SECOND_MASK  0x00003F00
#define BXVD_P_PPB_H264_GOP_PICTURE_MASK 0x000000FF
#define BXVD_P_PPB_H264_GOP_HOUR_SHIFT   20
#define BXVD_P_PPB_H264_GOP_MINUTE_SHIFT 14
#define BXVD_P_PPB_H264_GOP_SECOND_SHIFT 8

typedef struct
{
   /* 'valid' specifies which fields (or sets of
    * fields) below are valid.  If the corresponding
    * bit in 'valid' is NOT set then that field(s)
    * is (are) not initialized. */
   uint32_t      valid;

   int32_t        poc_top;
   int32_t        poc_bottom;
   uint32_t      idr_pic_id;

   /* H264_VALID_PANSCAN */
   uint32_t      pan_scan_count;
   uint32_t      pan_scan_horiz [3]; /* [31:16]=left, [15:0]=right */
   uint32_t      pan_scan_vert  [3]; /* [31:16]=top, [15:0]=bottom */

   /* H264_VALID_SPS_CROP */
   uint32_t      sps_crop_horiz;     /* [31:16]=left, [15:0]=right */
   uint32_t      sps_crop_vert;      /* [31:16]=top, [15:0]=bottom */

   /* H264_VALID_VUI */
   uint32_t      chroma_top;
   uint32_t      chroma_bottom;

   /* Offset to a BXVD_P_SEI_Message for the current frame */
   BXVD_P_SEI_Message * pstSEIMessageOffset;

} BXVD_P_PPB_H264;

/* Bit definitions for 'other.h264.valid' field */
#define  SVC_VALID_PANSCAN             (1)
#define  SVC_VALID_SPS_CROP            (2)
#define  SVC_VALID_VUI                 (4)

typedef struct
{
   /* 'valid' specifies which fields (or sets of
   * fields) below are valid.  If the corresponding
   * bit in 'valid' is NOT set then that field(s)
   * is (are) not initialized. */
   uint32_t     valid;

   int32_t       poc_top;
   int32_t       poc_bottom;
   uint32_t     idr_pic_id;

   /* SVC_VALID_PANSCAN */
   uint32_t     pan_scan_count;
   uint32_t     pan_scan_horiz[3];  /* [31:16]=left, [15:0]=right */
   uint32_t     pan_scan_vert[3];   /* [31:16]=top, [15:0]=bottom */

   /* SVC_VALID_SPS_CROP */
   uint32_t     sps_crop_horiz;     /* [31:16]=left, [15:0]=right */
   uint32_t     sps_crop_vert;      /* [31:16]=top, [15:0]=bottom */

   /* SVC_VALID_VUI */
   uint32_t     chroma_top;
   uint32_t     chroma_bottom;

} BXVD_P_PPB_SVC;


/*------------------------------------------------------*/
/*   MPEG-4 Part-2 Extension to the BXVD_P_PPB          */
/*------------------------------------------------------*/

typedef struct
{
   uint32_t pictype;   /* pict_coding_type (I=0, P=1, B=2, S=3) */

} BXVD_P_PPB_MP4;

/*------------------------------------------------------*/
/*    VP7/VP8 Extension to the BXVD_P_PPB               */
/*------------------------------------------------------*/

typedef struct
{
   /* 0: No upscaling (the most common case)
    * 1: Upscale by 5/4
    * 2: Upscale by 5/3
    * 3: Upscale by 2  
    */
   uint32_t    horiz_scale_mode;
   uint32_t    vert_scale_mode;

   /* SWDTV-8681: add support for VP8 display size */
   uint32_t display_horizontal_size;
   uint32_t display_vertical_size;

} BXVD_P_PPB_VP8;

/*------------------------------------------------------*/
/*    RV9 Extension to the BXVD_P_PPB                   */
/*------------------------------------------------------*/

typedef struct
{
   /* Always valid, defaults to picture size if no
    * sequence display extension in the stream. 
    */
    uint32_t   crop_horiz;     /* [31:16]=left, [15:0]=right */
    uint32_t   crop_vert;      /* [31:16]=top, [15:0]=bottom */
    
} BXVD_P_PPB_RV9;


/*------------------------------------------------------*
 *    Picture Parameter Block                           *
 *------------------------------------------------------*/

/* Bit definitions for 'flags' field */


#define  BXVD_P_PPB_FLAG_BUFFER_TYPE_MASK     (0x00000003)
#define  BXVD_P_PPB_FLAG_FRAME                (0x00000000)
#define  BXVD_P_PPB_FLAG_FIELDPAIR            (0x00000001)
#define  BXVD_P_PPB_FLAG_TOPFIELD             (0x00000002)
#define  BXVD_P_PPB_FLAG_BOTTOMFIELD          (0x00000003)

#define  BXVD_P_PPB_FLAG_SOURCE_TYPE_MASK     (0x0000000c)
#define  BXVD_P_PPB_FLAG_PROGRESSIVE_SRC      (0x00000000)
#define  BXVD_P_PPB_FLAG_INTERLACED_SRC       (0x00000004)
#define  BXVD_P_PPB_FLAG_UNKNOWN_SRC          (0x00000008)

#define  BXVD_P_PPB_FLAG_BOTTOM_FIRST         (0x00000010)

#define  BXVD_P_PPB_FLAG_PTS_PRESENT          (0x00000020)
#define  BXVD_P_PPB_FLAG_PCR_OFFSET_PRESENT   (0x00000040)
#define  BXVD_P_PPB_FLAG_DISCONT_PCR_OFFSET   (0x00000080)
#define  BXVD_P_PPB_FLAG_PICTURE_LESS_PPB     (0x00000100)
#define  BXVD_P_PPB_FLAG_PROG_SEQUENCE        (0x00000200)
#define  BXVD_P_PPB_FLAG_PIC_TAG_PRESENT      (0x00000400)
#define  BXVD_P_PPB_FLAG_DECODE_ERROR         (0x00000800)

#define  BXVD_P_PPB_FLAG_AFD_VALID            (0x00001000)

/* Bit 14:13 is a 2 bit enum for Picture type
 * 00 - Undefined (backward compatibility)
 * 01 - I Picture
 * 10 - P Picture
 * 11 - B Picture
 */
#define BXVD_P_PPB_FLAG_PICTURE_TYPE_MASK     (0x00006000)
#define BXVD_P_PPB_FLAG_I_PICTURE             (0x00002000)
#define BXVD_P_PPB_FLAG_P_PICTURE             (0x00004000)
#define BXVD_P_PPB_FLAG_B_PICTURE             (0x00006000)

#define BXVD_P_PPB_FLAG_REF_PICTURE           (0x00008000)
#define BXVD_P_PPB_FLAG_RAP_PICTURE           (0x00010000)

#define BXVD_P_PPB_NEW_PIC_TAG_AVAIL          (0x00020000)
#define BXVD_P_PPB_OVERSCAN_FLAG              (0x00040000)
#define BXVD_P_PPB_OVERSCAN_APPROPRIATE_FLAG  (0x00080000)
#define BXVD_P_PPB_FLAG_ERROR_CONCEAL         (0x00100000)
#define BXVD_P_PPB_FLAG_DECODE_REF_ERROR      (0x00200000)
#define BXVD_P_PPB_FLAG_INPUT_OVERFLOW        (0x00400000)

/*
 * Bits 26:23: ( a 4-bit value of 0 means 2D PPB)
 *  These bits (enum) are used to indicate which view component PPB belongs to
 * Bit 26: When set to 1, indicates MVC Base View component, when set to 0
 *     indicates dependent view component
 * Bit 25:23: This three bit number indicates the view index number for the
 *     dependent view. This ranges from 1 to 7. 0 is not used, as Base view is
 *     view index 0. This number is only valid when Bit 26 is set to 0.
 *
 */
#define BXVD_P_PPB_MULTIVIEW_FIELD_MASK       (0x07800000)
#define BXVD_P_PPB_MULTIVIEW_COUNT_MASK       (0x03800000)
#define BXVD_P_PPB_MULTIVIEW_COUNT_SHIFT      23
#define BXVD_P_PPB_MULTIVIEW_BASE_FLAG        (0x04000000)

#define BXVD_P_PPB_FLAG_FIXED_FRAME_RATE      (0x08000000)

/* SW7405-4560:: Indicates that this is the first time the message is being delivered.
 * The 3D framepacking messages can be repeated for a 'N' pictures, this
 * flag can be used to filter repeated messages.
 */
#define BXVD_P_PPB_FLAG_NEW_SEI_MSG           (0x10000000)


#if BXVD_P_PPB_EXTENDED

/* Bit definitions for the 'flags_ext0' field */

/* SW7425-1001: effectively an EOS flag.  Currently defined to 
 * only be delivered with a "picture-less" PPB. 
 */
#define BXVD_P_PPB_EXT0_FLAG_LAST_PICTURE    (0x00000001)

#endif


/* Values for 'pulldown' field.  '0' means no pulldown information
 * was present for this picture. */
typedef enum BXVD_P_PPB_PullDown
{
  BXVD_P_PPB_PullDown_eTop             = 1,
  BXVD_P_PPB_PullDown_eBottom          = 2,
  BXVD_P_PPB_PullDown_eTopBottom       = 3,
  BXVD_P_PPB_PullDown_eBottomTop       = 4,
  BXVD_P_PPB_PullDown_eTopBottomTop    = 5,
  BXVD_P_PPB_PullDown_eBottomTopBottom = 6,
  BXVD_P_PPB_PullDown_eFrameX2         = 7,
  BXVD_P_PPB_PullDown_eFrameX3         = 8,
  BXVD_P_PPB_PullDown_eFrameX1         = 9,
  BXVD_P_PPB_PullDown_eFrameX4         = 10
} BXVD_P_PPB_PullDown;

/* Values for the 'frame_rate' field. */
typedef enum BXVD_P_PPB_FrameRate
{
  BXVD_P_PPB_FrameRate_eUnknown = 0,
  BXVD_P_PPB_FrameRate_e23_97,
  BXVD_P_PPB_FrameRate_e24,
  BXVD_P_PPB_FrameRate_e25,
  BXVD_P_PPB_FrameRate_e29_97,
  BXVD_P_PPB_FrameRate_e30,
  BXVD_P_PPB_FrameRate_e50,
  BXVD_P_PPB_FrameRate_e59_94,
  BXVD_P_PPB_FrameRate_e60,
  BXVD_P_PPB_FrameRate_e14_985,
  BXVD_P_PPB_FrameRate_e7_493, 
  BXVD_P_PPB_FrameRate_e15,
  BXVD_P_PPB_FrameRate_e10,
  BXVD_P_PPB_FrameRate_e20,

  BXVD_P_PPB_FrameRate_eMax

} BXVD_P_PPB_FrameRate;

/* Values for the 'matrix_coeff' field. */
typedef enum BXVD_P_PPB_MatrixCoeff
{
  BXVD_P_PPB_MatrixCoeff_eUnknown   = 0,
  BXVD_P_PPB_MatrixCoeff_eBT709,
  BXVD_P_PPB_MatrixCoeff_eUnspecified,
  BXVD_P_PPB_MatrixCoeff_eReserved,
  BXVD_P_PPB_MatrixCoeff_eFCC       = 4,
  BXVD_P_PPB_MatrixCoeff_eBT740_2BG,
  BXVD_P_PPB_MatrixCoeff_eSMPTE170M,
  BXVD_P_PPB_MatrixCoeff_eSMPTE240M,
  BXVD_P_PPB_MatrixCoeff_eSMPTE293M
} BXVD_P_PPB_MatrixCoeff;

/* Values for the 'aspect_ratio' field. */
typedef enum BXVD_P_PPB_AspectRatio
{
  BXVD_P_PPB_AspectRatio_eUnknown = 0,
  BXVD_P_PPB_AspectRatio_eSquare,
  BXVD_P_PPB_AspectRatio_e12_11,
  BXVD_P_PPB_AspectRatio_e10_11,
  BXVD_P_PPB_AspectRatio_e16_11,
  BXVD_P_PPB_AspectRatio_e40_33,
  BXVD_P_PPB_AspectRatio_e24_11,
  BXVD_P_PPB_AspectRatio_e20_11,
  BXVD_P_PPB_AspectRatio_e32_11,
  BXVD_P_PPB_AspectRatio_e80_33,
  BXVD_P_PPB_AspectRatio_e18_11,
  BXVD_P_PPB_AspectRatio_e15_11,
  BXVD_P_PPB_AspectRatio_e64_33,
  BXVD_P_PPB_AspectRatio_e160_99,
  BXVD_P_PPB_AspectRatio_e4_3,
  BXVD_P_PPB_AspectRatio_e3_2,
  BXVD_P_PPB_AspectRatio_e2_1,
  BXVD_P_PPB_AspectRatio_e16_9,
  BXVD_P_PPB_AspectRatio_e221_1,
  BXVD_P_PPB_AspectRatio_eOther = 255
} BXVD_P_PPB_AspectRatio;

/* Values for the 'colour_primaries' field. */
typedef enum BXVD_P_PPB_ColorPrimaries
{
  BXVD_P_PPB_ColorPrimaries_eUnknown = 0,
  BXVD_P_PPB_ColorPrimaries_eBT709,
  BXVD_P_PPB_ColorPrimaries_eUnspecified,
  BXVD_P_PPB_ColorPrimaries_eReserved,
  BXVD_P_PPB_ColorPrimaries_eBT470_2M = 4,
  BXVD_P_PPB_ColorPrimaries_eBT470_2BG,
  BXVD_P_PPB_ColorPrimaries_eSMPTE170M,
  BXVD_P_PPB_ColorPrimaries_eSMPTE240M,
  BXVD_P_PPB_ColorPrimaries_eGenericFilm,
  BXVD_P_PPB_ColorPrimaries_eBT1361
} BXVD_P_PPB_ColorPrimaries;

/* Values for the 'transfer_char' field. */
typedef enum BXVD_P_PPB_TransferChar
{
  BXVD_P_PPB_TransferChar_eUnknown = 0,
  BXVD_P_PPB_TransferChar_eBT709,
  BXVD_P_PPB_TransferChar_eUnspecified,
  BXVD_P_PPB_TransferChar_eReserved,
  BXVD_P_PPB_TransferChar_eBT479_2M = 4,
  BXVD_P_PPB_TransferChar_eBT479_2BG,
  BXVD_P_PPB_TransferChar_eSMPTE170M,
  BXVD_P_PPB_TransferChar_eSMPTE240M,
  BXVD_P_PPB_TransferChar_eLinear,
  BXVD_P_PPB_TransferChar_eLog100_1,
  BXVD_P_PPB_TransferChar_eLog31622777_1,
  BXVD_P_PPB_TransferChar_eIec_61966_2_4
} BXVD_P_PPB_TransferChar;

/* SW7425-3177: Values for 'protocol' field.  These needed to be added back 
 * in since the enums defining the video protocol have diverged between AVD 
 * and bavc.h, i.e. BXVD_P_PPB_Protocol is no longer in sync with BAVC_VideoCompressionStd.
 */
typedef enum BXVD_P_PPB_Protocol
{   
   BXVD_P_PPB_Protocol_eH264 = 0,   
   BXVD_P_PPB_Protocol_eMPEG2,  
   BXVD_P_PPB_Protocol_eH261,  
   BXVD_P_PPB_Protocol_eH263,   
   BXVD_P_PPB_Protocol_eVC1,  
   BXVD_P_PPB_Protocol_eMPEG1,  
   BXVD_P_PPB_Protocol_eMPEG2DTV, 
   BXVD_P_PPB_Protocol_eVC1SimpleMain, 
   BXVD_P_PPB_Protocol_eMPEG4Part2,  
   BXVD_P_PPB_Protocol_eAVS,  
   BXVD_P_PPB_Protocol_eMPEG2_DSS_PES,
   BXVD_P_PPB_Protocol_eSVC,            /* Scalable Video Codec */
   BXVD_P_PPB_Protocol_eSVC_BL,         /* Scalable Video Codec Base Layer */
   BXVD_P_PPB_Protocol_eMVC,            /* MVC Multi View Coding */
   BXVD_P_PPB_Protocol_eVP6,            /* VP6 */
   BXVD_P_PPB_Protocol_eVP7,            /* VP7 */
   BXVD_P_PPB_Protocol_eVP8,            /* VP8 */
   BXVD_P_PPB_Protocol_eRV9,            /* Real Video 9 */
   BXVD_P_PPB_Protocol_eSPARK,          /* Sorenson Spark */
   BXVD_P_PPB_Protocol_eH265,

   BXVD_P_PPB_Protocol_eMax
   
} BXVD_P_PPB_Protocol;


/* Values for extracting the "protocol level" field from the PPB.
 * The "level" is the lower 16 bits.
 */
#define BXVD_P_PPB_PROTOCOL_LEVEL_MASK            0x0000FFFF
#define BXVD_P_PPB_PROTOCOL_LEVEL_SHIFT           0x0

/* Values for extracting the "protocol profile" field from the PPB.
 * The "profile" is the upper 16 bits.
 */
#define BXVD_P_PPB_PROTOCOL_PROFILE_MASK          0xFFFF0000
#define BXVD_P_PPB_PROTOCOL_PROFILE_SHIFT         0x10


#define BXVD_P_PPB_DNR_FLAG_REFPIC		  0x01
#define BXVD_P_PPB_DNR_FLAG_GOPHEADER		  0x02

typedef struct
{
   uint32_t      pic_flags;            /* Bit 2: GOP header seen. Bit 1: Sequence header seen Bit 0: reference pic  */
   uint32_t      sum_qp_cur;           /* sum of the quant params for this frame, or first field if interlaced */
   uint32_t      sum_qp_ref;           /* sum of the quant params for the reference pictures */
   uint32_t      num_mb_cur;           /* number of macroblocks included in sum_qp_cur */
   uint32_t      mb_count_intra_inter; /* number of  macroblocks in this picture: [31:16]=intra, [15:0]=non_intra */
   uint32_t      intra_qmat_sum_lo_hi; /* sum of intra quant matrix: [31:16]=first 36 entries, [15:0]=last 32 entries */
   uint32_t      inter_qmat_sum_lo_hi; /* sum of inter quant matrix: [31:16]=first 36 entries, [15:0]=last 32 entries */
   uint32_t      num_mb_ref;           /* number of macroblocks included in sum_qp_ref */
} BXVD_P_PPB_DNR;

/* data structure with key statistics collected by inner-loop for a picture */
/* some are available only for newer AVD core revisions */
typedef struct
{
   /* Number of cycles taken just by the inner-loop decode.
    * Does not include cabac or outer-loop processing times. */
   uint32_t      decode_cycle_count;

   /* the following are used/available for Rev J and later which
    * has a mocomp cache in there. will be 0 for earlier revisions. */
   uint32_t      pcache_hit;
   uint32_t      pcache_miss_1;
   uint32_t      pcache_miss_2;

   uint32_t      cabac_cylces;
   uint32_t      inst_cache_miss;
} BXVD_P_PPB_IL_DATA;

#if BXVD_P_PPB_EXTENDED

typedef struct
{
    uint32_t      crc_luma;            /* frame or top field of LUMA CRC */
    uint32_t      crc_cb;              /* frame or top field of CHROMA CB CRC */
    uint32_t      crc_cr;              /* frame or top field of CHROMA CR CRC */
    uint32_t      crc_luma_bot;        /* bot field of LUMA CRC */
    uint32_t      crc_cb_bot;          /* bot field of CHROMA CB CRC */
    uint32_t      crc_cr_bot;          /* bot field of CHROMA CR CRC */    
} BXVD_P_PPB_CRC;

/* Data structure with key statistics collected by outer-loop for a frame. */
/* For interlaced data,  statistics are accummulated over both fields-     */
/* statistics per field would go over 256 byte limit  */

typedef struct
{
    uint32_t      idle_time;                  /* IL idle time; stall time while waiting for IL decode command */
    unsigned short    num_decod_mbs;              /* (H264 only) number of macroblock are decoded */
    unsigned short    num_recov_mbs;              /* (H264 only) Number Of Recovered Macroblocks: number of macroblocks which where error corrected */
    uint32_t      totalDecodeCycles;          /* total IL frame  decode cycle  */
    uint32_t      totalCabacCycles;           /* total CABAC frame cycle for pair field picture */
    uint32_t      totalILBLDecodeCycles;      /* svc IL frame decode cycle count include wait for BL */
    uint32_t      totalFrm2FrmDecodeCycles;   /* IL frame cycle count from frame to frame (include wit for (BL), OL) */
    uint32_t      reserved[4]; 
} BXVD_P_PPB_IL_LOG_DATA;


typedef struct
{
    uint32_t      decode_cycle_count;
    uint32_t      inst_cache_miss;
    uint32_t      display_time;
    uint32_t      cdb_size;
    uint32_t      reserved[4];
} BXVD_P_PPB_OL_LOG_DATA;

typedef struct
{
    BXVD_P_PPB_IL_LOG_DATA il_log_data;
    BXVD_P_PPB_OL_LOG_DATA ol_log_data;
} BXVD_P_PPB_LOG_DATA;

#endif /* #if BXVD_P_PPB_EXTENDED */

typedef struct
{
   /* Common fields. */
   uint32_t      flags;            /* see above         */
   uint32_t      luma_video_address;    /* Address of picbuf Y     */
   uint32_t      chroma_video_address; /* Address of picbuf UV    */
   uint32_t      video_width;      /* Picbuf width      */
   uint32_t      video_height;     /* Picbuf height     */

   uint32_t      luma_stripe_height;
   uint32_t      chroma_stripe_height;
   uint32_t      pulldown;         /* see above         */
   uint32_t      protocol;         /* protocolXXX (above)     */

   uint32_t      frame_rate;       /* see above         */
   uint32_t      aspect_ratio;     /* see above         */
   uint32_t      custom_aspect_ratio_width_height; /* upper 16-bits is Y and lower 16-bits is X */
   uint32_t      display_info;     /* [31:24]=matrix_coeff, [23:16]=color_primaries, [15:8]=transfer_char */

   uint32_t      pts;              /* 32 LSBs of PTS    */
   uint32_t      pcr_offset;       /* 45kHz if PCR type; else 27MHz */
   uint32_t      n_drop;           /* Number of pictures to be dropped */

   /* User data is in the form of a linked list. */
   long               user_data_size;
   UD_HDR            *user_data;

   /* DNR parameters */
   BXVD_P_PPB_DNR     dnr;              /* DNR parameters for entire frame */

   uint32_t      timing_marker;
   uint32_t      picture_tag;      /* tag used during on-the-fly PVR, similar to PTS */

   /* SW7425-2247: support for bar data. This field was 
    * formerly fgt_block_avg_address 
    */
   uint32_t      bar_data;         
   uint32_t      profile_level;

   uint32_t      afd_val;         /* Active Format Descriptor */

   /* IL decode information */
   BXVD_P_PPB_IL_DATA       il_perf_data;

   uint32_t      delta_pic_seen;

   /* W7401-4426: H264 time code [24:20]=hour, [19:14]=minute, [13:8]=sec, [7:0]=frame */
   uint32_t     time_code;

   uint32_t      reserved[2];     /* may be removed if protocol-specific PPB requires more bytes */

   /* Protocol-specific extensions. */
   union
   {
      BXVD_P_PPB_H264        h264;
      BXVD_P_PPB_SVC         svc;
      BXVD_P_PPB_MPEG        mpeg;
      BXVD_P_PPB_VC1         vc1;
      BXVD_P_PPB_AVS         avs;
      BXVD_P_PPB_MP4         mp4;
      BXVD_P_PPB_VP8         vp8;
      BXVD_P_PPB_RV9         rv9;

#ifndef BXVD_P_PPB_EXTENDED

   } other;

#else
      uint32_t             stub[32];   /* to force this union to be a minimum of 32 words. */

   } other;

   BXVD_P_PPB_CRC          crc;
   uint32_t                flags_ext0;
   uint32_t                picture_latency;
   uint32_t                ppb_reserved[29];
   BXVD_P_PPB_LOG_DATA     data_log;

#endif

} BXVD_P_PPB;


/* Display Manager: STC and Parity Information in DRAM for host to use */

#if BXVD_P_FW_HIM_API

#if BXVD_P_HVD_PRESENT 
/* Hercules */
#define BXVD_P_CURRENT_MAJOR_VERSION 2

#else
/* Aphrodite */
#define BXVD_P_CURRENT_MAJOR_VERSION 5
#endif

#define BXVD_P_STC_MAX 8

typedef struct
{
    uint32_t stc_snapshot[BXVD_P_STC_MAX];
    uint32_t vsync_parity;
} BXVD_P_DisplayInfo;

#else

#define BXVD_P_CURRENT_MAJOR_VERSION 3

#define BXVD_P_STC_MAX 2

typedef struct
{
      uint32_t stc_snapshot;
      uint32_t vsync_parity;
      uint32_t stc1_snapshot;
} BXVD_P_DisplayInfo;

#endif   /* ~BXVD_P_FW_HIM_API */


#if BXVD_P_FW_HIM_API
#define BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE              64
#define BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE                0
#else
#define BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE              62
#define BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE                2
#endif

#define BXVD_P_DISP_FIFO_DEPTH   ( BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE + BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE )

#if 0
/* Parameters passed from Display Manager (host) to DMS */
typedef struct
{
   uint32_t write_offset;
   uint32_t drop_count;
} BXVD_P_DMS_Info;
#endif

typedef struct
{

#if !BXVD_P_FW_HIM_API
    uint32_t queue_read_offset; /* offset is w.r.t base of this data struct so value of 0-1 prohibited */
    uint32_t queue_write_offset; /* offset is w.r.t base of this data struct so value of 0-1 prohibited */
#endif

    /* queue if full if (write_offset+1 == read_offset) */
    /* write_offset modified by firmware and read_offset modified by Display Manager in host */
    BXVD_P_PPB *display_elements[BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE];

} BXVD_P_Avd_Queue_In_Memory;

#define BXVD_P_PictureDeliveryQueue BXVD_P_Avd_Queue_In_Memory
#define BXVD_P_PictureReleaseQueue BXVD_P_Avd_Queue_In_Memory

/* picture information block (PIB) */
typedef struct BXVD_P_PPB_PIB
{
	BXVD_P_PPB                sPPB;
	unsigned long      bFormatChange;
	unsigned long      ulResolution;
	unsigned long      ulChannelId;
	unsigned long      ppbPtr;

} BXVD_P_PPB_PIB;



/* The following two sections are mask and shift constant definitions based on FW team defined VDEC_FLAG above.
 * If the VDEC_FLAG bits change, these values will also need to change */

/* picture structure is in bits 0 & 1 */
#define BXVD_P_PPB_FLAG_EXTRACT_PICT_STRUCT_MASK         0x03
#define BXVD_P_PPB_FLAG_EXTRACT_PICT_STRUCT_SHIFT        0x0

/* progressive frame in bits 2 & 3 */
#define BXVD_P_PPB_FLAG_EXTRACT_SOURCE_FORMAT_MASK       0xc
#define BXVD_P_PPB_FLAG_EXTRACT_SOURCE_FORMAT_SHIFT      0x2

/* Paramaters passed from DMS to DM */
typedef struct BXVD_P_DMS2DMInfo
{
	BXVD_P_DisplayInfo *pDisplayInfo ;
	BXVD_P_PictureDeliveryQueue *pPictureDeliveryQueue ;
	BXVD_P_PictureReleaseQueue  *pPictureReleaseQueue ;
} BXVD_P_DMS2DMInfo ;

/* Parameters passed from DM to DMS */
typedef struct BXVD_P_DM2DMSInfo
{
   uint32_t pdq_write_offset ;
   uint32_t drop_count;
} BXVD_P_DM2DMSInfo ;

typedef struct BXVD_P_DisplayElement
{
	BXVD_P_PPB *pPPB ;
	BXVD_P_PPB *pPPBPhysical ;
	BAVC_Polarity ePPBPolarity;
} BXVD_P_DisplayElement ;

typedef struct BXVD_P_DisplayElementQueue
{
	BXVD_P_PPB *pPPBQArray[ BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE ] ;
	uint32_t read_pointer ;
	uint32_t write_pointer ;
    uint32_t depth_pointer ;
} BXVD_P_DisplayElementQueue ;
/************************************************************************/
/* ABOVE ARE THE STRUCTURES ADDED BY SSAVEKAR FOR XVD INTEGRATION       */
/************************************************************************/

#endif /* __INC_VDEC_INFO_H__ */


