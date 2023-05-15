/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfmt.h $
 * $brcm_Revision: Hydra_Software_Devel/106 $
 * $brcm_Date: 8/10/12 3:25p $
 *
 * Module Description:
 *   Video format module header file
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/fmt/bfmt.h $
 * 
 * Hydra_Software_Devel/106   8/10/12 3:25p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/105   12/6/11 4:08p rgreen
 * SW7425-597: Add 1080p (60Hz) to BFMT_SUPPORT_HDMI macro
 *
 * Hydra_Software_Devel/104   10/7/11 4:09p yuxiaz
 * SW7425-974: Added new high pixel rate formats.
 *
 * Hydra_Software_Devel/103   9/16/11 3:05p pntruong
 * SW7420-2056: Merged.
 *
 * Hydra_Software_Devel/SW7420-2056/1   9/15/11 3:51p vle
 * SW7420-2056: Fix incorrect Hdmi 3D formats support from parsing EDID
 *
 * Hydra_Software_Devel/102   8/24/11 10:56a tdo
 * SW7425-610: Add support for VESA formats1280x1024
 *
 * Hydra_Software_Devel/101   8/8/11 2:27p yuxiaz
 * SW7425-966: Added BFMT_Orientation_eLeftRight_Enhanced for MRE modes.
 *
 * Hydra_Software_Devel/100   6/22/11 1:33p pntruong
 * SW7425-686: Added clarification regard deprecating of BFMT_VERT_* and
 * BFMT_PXL_*.
 *
 * Hydra_Software_Devel/99   6/22/11 11:22a pntruong
 * SW7425-686: Added new veritical refresh rate enum for synchronizing
 * XVD/VDC state.  Also will be using this enum for replacing limited
 * fmt's veritical refresh rate mask.
 *
 * Hydra_Software_Devel/98   12/9/10 2:16p pntruong
 * SW35230-2250: Merged to mainline.
 *
 * Hydra_Software_Devel/97   12/7/10 1:56p tdo
 * SW7422-44: Add the missing 3D format to BFMT_SUPPORT_HDMI
 *
 * Hydra_Software_Devel/96   12/1/10 6:07p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/95   9/30/10 5:01p yuxiaz
 * SW7422-28: Reorder BFMT_Orientation enums to match RDB.
 *
 * Hydra_Software_Devel/94   9/7/10 3:03p yuxiaz
 * SW7422-28: Add 3D formats to FMT plus additional data structures
 *
 * Hydra_Software_Devel/SW7422-28/3   9/3/10 3:21p yuxiaz
 * SW7422-28: Added more comment for ulActiveSpace
 *
 * Hydra_Software_Devel/SW7422-28/2   9/3/10 11:47a yuxiaz
 * SW7422-28: Rename BFMT_VideoFmt_e3D_1080p_30Hz  to
 * BFMT_VideoFmt_e1080p_30Hz_3DOU_AS.
 *
 * Hydra_Software_Devel/SW7422-28/1   9/2/10 4:08p yuxiaz
 * SW7422-28: Add 3D formats to FMT plus additional data structures
 *
 * Hydra_Software_Devel/92   9/1/10 4:53p yuxiaz
 * SW7422-29: Added BFMT_Orientation.
 *
 * Hydra_Software_Devel/91   7/13/10 11:26a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/90   5/20/10 5:47p darnstein
 * SW7125-326: add vertical frequency for 720P@30Hz video.
 *
 * Hydra_Software_Devel/89   3/31/10 4:16p pntruong
 * SW7405-4150: Fixed build errors.
 *
 * Hydra_Software_Devel/88   3/31/10 3:39p pntruong
 * SW7405-4150: Updated the list of HDMI format that matches with HDMI PI
 * implementation.
 *
 * Hydra_Software_Devel/87   2/23/10 3:48p pntruong
 * SW3548-2764: Allows 2x oversample 480p/576p for hdmi input.
 *
 * Hydra_Software_Devel/86   2/23/10 2:53p darnstein
 * SW7400-2620: add pixel frequency defintions for double-rate 480P.
 *
 * Hydra_Software_Devel/85   2/1/10 6:34p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/4   1/29/10 6:55p hongtaoz
 * SW3548-2680, SW3548-2681: removed 1470p custom formats since they are
 * the same as 720p3D formats; added macro to wrap the 3548 specific
 * legacy 3DTV format;
 *
 * Hydra_Software_Devel/BCM3D/3   1/15/10 6:33p hongtaoz
 * SW3548-2680, SW3548-2681: added custom formats to support HDMI 1.4 3D
 * 720p50/60 input passed-thru to external panel processor; reanmed 2160i
 * to custom DVI format;
 *
 * Hydra_Software_Devel/BCM3D/2   1/15/10 4:54p hongtaoz
 * SW3548-2680, SW3548-2681: added new HDMI1.4 3D formats 720p50 input
 * support; fixed pixel frequency value for 3D formats;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 5:55p hongtaoz
 * SW3548-2680, SW3548-2681: added new HDMI1.4 3D formats 720p60 and
 * 1080p24; added a special display format 2160i48 for 11-bit BVN;
 *
 * Hydra_Software_Devel/84   12/15/09 2:15p pntruong
 * SW3556-933: Display no signal when input signal is 1440x480@60Hz.
 *
 * Hydra_Software_Devel/83   12/2/09 8:24p pntruong
 * SW7420-447: HDMI: Incorrect preferred format display at hot plug if TV
 * has preferred format of 1080p/60Hz.
 *
 * Hydra_Software_Devel/82   10/16/09 5:20p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/81   10/15/09 4:47p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/80   10/1/09 7:44p darnstein
 * SW7405-2612: assert that 1360x768 is a VESA video format.
 *
 * Hydra_Software_Devel/79   7/31/09 6:05p rpan
 * PR56037: Change BFMT_PXL_85_5MHz value to avoid collision.
 *
 * Hydra_Software_Devel/78   7/21/09 4:22p rpan
 * PR56037: Initial effort for analog VESA outputs.
 *
 * Hydra_Software_Devel/77   5/4/09 4:22p rpan
 * PR53106: Added new format 720p25/30Hz.
 *
 * Hydra_Software_Devel/76   3/18/09 6:04p pntruong
 * PR53234, PR53395: Added support for wvga, wsvga, sxga.
 *
 * Hydra_Software_Devel/75   3/4/09 11:18a pntruong
 * PR52568: Customer not needed the custom 1340x725p@60hz.
 *
 * Hydra_Software_Devel/74   3/2/09 3:05p pntruong
 * PR52568: Added support for custom format 1340x725p@60hz.
 *
 * Hydra_Software_Devel/73   9/20/08 11:00a pntruong
 * PR47149: Relax const to allow app to dynamically load of custom ucode.
 *
 * Hydra_Software_Devel/72   9/20/08 10:55a pntruong
 * PR47149: Relax const to allow app to dynamically load of custom ucode.
 *
 * Hydra_Software_Devel/71   8/6/08 4:44p pntruong
 * PR45219:  Added new RGB formats.
 *
 * Hydra_Software_Devel/70   6/26/08 4:47p rpan
 * PR39529: Added BFMT_VERT_48Hz.
 *
 * Hydra_Software_Devel/69   6/9/08 1:57p rpan
 * PR39529: Back out FMT_VideoFmt_e1080p_48Hz change. Will use
 * FMT_Video_eCustom to test 48Hz panel.
 *
 * Hydra_Software_Devel/68   6/9/08 10:47a rpan
 * PR39529: Added new display format FMT_VideoFmt_e1080p_48Hz. uCode is
 * empty right now.
 *
 * Hydra_Software_Devel/67   5/1/08 2:37p rpan
 * PR42109: Initial work for SECAM variants.
 *
 * Hydra_Software_Devel/66   3/26/08 10:24a hongtaoz
 * PR35398: added BFMT_FREQ_FACTOR to unify Vsync rate and pixel frequency
 * calculation;
 *
 * Hydra_Software_Devel/65   3/11/08 10:45p pntruong
 * PR40280: Added support for vdec PAL-60.
 *
 * Hydra_Software_Devel/PR40133/1   3/11/08 10:32a mlei
 * PR40280:need PAL-60 support on 3563
 *
 * Hydra_Software_Devel/64   12/3/07 11:43a pntruong
 * PR37790: [PC]: Remove two reduntant formats:
 * BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Cea; and
 * BFMT_VideoFmt_eDVI_1920x1080i_60Hz.
 *
 * Hydra_Software_Devel/gorgon_temp_20071203/1   12/3/07 2:35p gorgon
 * PR37790:[PC]: Remove two reduntant
 * formats:BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Cea&#12288;and
 * BFMT_VideoFmt_eDVI_1920x1080i_60Hz
 *
 * Hydra_Software_Devel/63   11/1/07 10:00a pntruong
 * PR36487: Add HDMI support for 1080p_25Hz in FMT module.
 *
 * Hydra_Software_Devel/62   10/30/07 7:06p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/61   10/15/07 3:21p pntruong
 * PR35137: Add NTSC443 support for cvbs/Svideo.
 *
 * Hydra_Software_Devel/60   8/17/07 11:45a pntruong
 * PR34059: Need to fix FMT name for 1024x768i_43Hz.
 *
 * Hydra_Software_Devel/PR34059/1   8/17/07 11:40a gorgon
 * PR34059: Need to fix FMT name for 1024x768i_43Hz
 *
 * Hydra_Software_Devel/59   7/19/07 4:33p pntruong
 * PR33138: [VDEC][PC]:Support PC 1920x1080i_60Hz.
 *
 * Hydra_Software_Devel/Gorgon_20070719/1   7/19/07 3:38p gorgon
 * PR33138:[VDEC][PC]:Support PC 1920x1080i_60Hz.
 *
 * Hydra_Software_Devel/58   7/11/07 1:56p pntruong
 * PR32296, PR32913, PR32919: Added support for 1080p@60Hz_Cea,
 * 1366x768@60Hz.
 *
 * Hydra_Software_Devel/PR32296/2   7/11/07 9:34p gorgon
 * PR32919:[PC]: Support format 1366x768@60.
 *
 * Hydra_Software_Devel/PR32296/1   7/11/07 7:26p gorgon
 * PR32913:[PC]:Add a new format 1920x1080p@60Hz_Cea.
 *
 * Hydra_Software_Devel/57   6/12/07 4:32p pntruong
 * PR32076, PR32078, PR32081: Updated pc-input pll settings.
 *
 * Hydra_Software_Devel/56   6/5/07 8:41p pntruong
 * PR31619: Add two new format: 1280x768p/60Hz reduced blank and
 * 1400x1050p/60Hz reduced blank.
 * PR31619: Update timing and PLL settings for 1400x1050p/60Hz and
 * 1400x1050p/75Hz.
 *
 * Hydra_Software_Devel/55   5/18/07 2:47p yuxiaz
 * PR28810: Merge in 832x624 support from PR28810 branch.
 *
 * Hydra_Software_Devel/PR28810/1   5/16/07 11:18a honglang
 * PR28810: add format 832x624 support
 *
 * Hydra_Software_Devel/53   1/23/07 4:06p pntruong
 * PR23225: VDEC - PC input.  Updated values for FD detect for PC mode.
 *
 * Hydra_Software_Devel/52   12/18/06 11:16p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   10/4/06 6:57p hongtaoz
 * PR23204, PR23279, PR23280, PR24727: add user-defined custom formats;
 * removed CUSTOM 1600x1200p format;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/26/06 11:05a pntruong
 * PR23550: Add format support for 1360x768@60Hz, 1280x768@60Hz,
 * 1280x768@60Hz Reduced Blanking.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/25/06 2:05p pntruong
 * PR23222, PR23225: Added more entries in format detection table.
 *
 * Hydra_Software_Devel/51   9/6/06 4:16p hongtaoz
 * PR22568, PR23188: added 1080p60 format for 3563;
 *
 * Hydra_Software_Devel/50   8/16/06 3:26p pntruong
 * PR23522: Add supporting hdmi pc input 800x600@56Hz and 800x600@85Hz
 *
 * Hydra_Software_Devel/49   8/7/06 11:59a pntruong
 * PR22494: Support HDDVI new trigger in 3560 B0 software.
 *
 * Hydra_Software_Devel/48   7/5/06 6:39p pntruong
 * PR20913: Need support for smaller formats to avoid clipping issues.
 * Made 240p and 288p a custom format for 7411 because of non-repeat
 * pixel support.
 *
 * Hydra_Software_Devel/47   6/27/06 1:34p jessem
 * PR 19995: Added 720p 24Hz, 1080p 24Hz, and 1080p 30Hz to
 * BFMT_SUPPORT_HDMI(fmt).
 *
 * Hydra_Software_Devel/46   6/19/06 10:15a pntruong
 * PR20913: Need support for smaller formats to avoid clipping issues.
 * Added 240p60Hz and 288p50Hz hddvi input support.
 *
 * Hydra_Software_Devel/45   6/9/06 8:47a jessem
 * PR 19995, PR 20086: Added 720p 23.976/24Hz support. Added
 * BFMT_VERT_INVALID.
 *
 * Hydra_Software_Devel/44   4/19/06 1:53p hongtaoz
 * PR20938: redefined pixel frequency enums; updated some vesa mode timing
 * parameters to comply with DVT Format Support document, i.e. reference
 * software only supports 50/59.94/60 Hz VESA formats;
 *
 * Hydra_Software_Devel/43   3/27/06 1:55p pntruong
 * PR20239: Added support for 1920x1080P@24Hz, 1920x1080P@30Hz formats on
 * 3560B0 on HD_DVI input.
 *
 * Hydra_Software_Devel/42   3/22/06 5:37p jessem
 * PR 19517, PR 19995: Added BFMT_VERT_23_976Hz, BFMT_VERT_24Hz, and
 * BFMT_VERT_25Hz
 *
 * Hydra_Software_Devel/42   3/22/06 5:14p jessem
 * PR 19517, PR 19995: Added BFMT_VERT_23_976Hz, BFMT_VERT_24Hz, and
 * BFMT_VERT_25Hz.
 *
 * Hydra_Software_Devel/41   2/16/06 3:07p pntruong
 * PR17778: Update pixel clock rate for new formats.
 *
 * Hydra_Software_Devel/41   2/16/06 3:06p pntruong
 * PR17778: Update pixel clock rate for new formats.
 *
 * Hydra_Software_Devel/41   2/16/06 3:05p pntruong
 * PR17778: Update pixel clock rate for new formats.
 *
 * Hydra_Software_Devel/40   2/15/06 11:32a jessem
 * PR 17554: Added support for 1080p 30Hz.
 *
 * Hydra_Software_Devel/39   1/31/06 1:52p pntruong
 * PR17778: Support VESA input for HD_DVI, and merge in 3560B0 support.
 *
 * Hydra_Software_Devel/38   1/25/06 11:28a maivu
 * PR 19138, PR 19157: Added support for BFMT_AspectRatio_eSAR for AVC.
 * Refer to ulSampleAspectRatioX/ulSampleAspectRatioY if DAR is not
 * available.
 *
 * Hydra_Software_Devel/37   11/16/05 4:07p maivu
 * PR 18107: Removed unsupported aspect ratio previously added. Those are
 * actually SAR(for h.264) not display aspect ratio.
 *
 * Hydra_Software_Devel/36   9/21/05 7:56p albertl
 * PR12388:  Added 1366x768 50Hz and removed 50Hz clock from old 1366x768.
 *
 * Hydra_Software_Devel/35   6/22/05 2:45p pntruong
 * PR15820, PR15563, PR15685: Added pixel frequency to aid compute
 * bandwith equation.
 *
 * Hydra_Software_Devel/34   2/9/05 2:52p albertl
 * PR12560: Added BFMT_VideoFmt_eDVI_1280x768p,
 * BFMT_VideoFmt_eDVI_1280x720p_50Hz, BFMT_VideoFmt_eDVI_1280x720p, and
 * BFMT_VideoFmt_eDVI_1280x720p_ReducedBlank formats.
 *
 * Hydra_Software_Devel/33   12/22/04 2:31p darnstein
 * PR 13631:  Implement callback for the unwanted interrupt.
 * PR 13365:  Support new AVC information (partially done).
 *
 * Hydra_Software_Devel/32   12/20/04 2:10p darnstein
 * PR 13506: Adapt to change BAVC_AspectRatio->BFMT_AspectRatio.
 *
 * Hydra_Software_Devel/31   12/16/04 5:30p albertl
 * PR12388:  Added 50Hz clock to 1366x768.
 *
 * Hydra_Software_Devel/30   12/9/04 11:21a syang
 * PR 13092:  change BFMT and BVDC to use 483 lines for 480p
 *
 * Hydra_Software_Devel/29   12/3/04 10:06a jasonh
 * PR 13448: Added support of digital format width/height to FMT module.
 *
 * Hydra_Software_Devel/28   11/17/04 11:06a syang
 * PR12336: put back the fix again (increase ntsc height to 482)
 *
 * Hydra_Software_Devel/27   11/4/04 11:48a syang
 * PR12336: temp back off the 482 lines for ntsc
 *
 * Hydra_Software_Devel/26   11/2/04 4:26p syang
 * PR 12336: Trully increase NTSC line number to 482, and take off related
 * work-around in compositor
 *
 * Hydra_Software_Devel/25   10/18/04 5:05p albertl
 * PR12641:  Added PAL formats to HDMI macro.
 *
 * Hydra_Software_Devel/24   10/13/04 7:58p hongtaoz
 * PR12301: added the maximum VBI pass-through lines for each format;
 * directly use scan width and height to describe the format
 * rasterization info;
 *
 * Hydra_Software_Devel/23   9/27/04 4:05p hongtaoz
 * PR12641: updated description for video format HDMI support macro;
 *
 * Hydra_Software_Devel/22   9/27/04 3:13p hongtaoz
 * PR12641: added 576p 50Hz dvi support;
 *
 * Hydra_Software_Devel/21   9/22/04 12:32p pntruong
 * PR12728: Fixed -pdantic build warnings and errors.
 *
 * Hydra_Software_Devel/20   9/21/04 11:57a albertl
 * PR 11794: Added 720p 50Hz, 1080i 50Hz, and 1366x768 to
 * BFMT_SUPPORT_HDMI macro.
 *
 * Hydra_Software_Devel/19   9/15/04 4:31p albertl
 * PR 12388:  Added 1366x768 format, and integrated required settings.
 *
 * Hydra_Software_Devel/18   9/9/04 11:31a hongtaoz
 * PR11868, PR11870: added VESA modes 800x600p and 1024x768p;
 *
 * Hydra_Software_Devel/17   7/8/04 5:57p hongtaoz
 * PR11794: added 720p_50Hz and 1250i_50Hz support;
 *
 * Hydra_Software_Devel/16   6/16/04 5:42p jasonh
 * PR 11562: Added support for test format.
 *
 * Hydra_Software_Devel/15   5/4/04 6:44p hongtaoz
 * PR10073: added refresh rate to the BFMT module;
 * PR10942: added pixel frequency bit mask into format info; only expose
 * necessary format macroes;
 *
 * Hydra_Software_Devel/14   5/4/04 11:14a hongtaoz
 * PR10942: moved some useful video format macroes from
 * bvdc_display_priv.h to BFMT module.
 *
 * Hydra_Software_Devel/12   2/19/04 2:42p maivu
 * PR 9493: Fixed typo in comments.
 *
 * Hydra_Software_Devel/11   2/19/04 10:36a maivu
 * PR 9493: Removed generic PAL. Added specific PAL modes:
 * B,B1,D,D1,G,H,I,K.
 *
 * Hydra_Software_Devel/10   12/17/03 3:47p pntruong
 * PR 9027: Added BFMT_GetVideoFormatInfoPtr to BFMT.
 *
 * Hydra_Software_Devel/9   10/9/03 8:40a pntruong
 * Added some common use constants.
 *
 * Hydra_Software_Devel/8   7/22/03 1:44p pntruong
 * Added implementation.
 *
 * Hydra_Software_Devel/7   7/9/03 5:07p maivu
 * Added separate PAL-N to distinguish from PAL-NC. Renamed
 * BFMT_DisplayFmt_xyz to BFMT_VideoFmt_xyz.
 *
 * Hydra_Software_Devel/6   7/8/03 6:22p maivu
 * Rename BFMT_VideoFmt_ePAL_N to BFMT_VideoFmt_ePAL_NC (for N
 * combination), per Alek's request.
 *
 * Hydra_Software_Devel/5   3/19/03 5:54p maivu
 * Modifications based on feedback from initial internal review.
 *
 * Hydra_Software_Devel/4   3/10/03 6:02p pntruong
 * Fixed semicolon typo.
 *
 * Hydra_Software_Devel/3   3/4/03 3:16p maivu
 * Added total horizontal length, and total vertical height
 *
 * Hydra_Software_Devel/2   3/3/03 3:49p maivu
 * Added comma on last enum defs, so docJet can pick up the comment
 *
 * Hydra_Software_Devel/1   3/3/03 3:42p maivu
 * Initial revision
 *
 *
 ***************************************************************************/
#ifndef BFMT_H__
#define BFMT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=************************ Module Overview ********************************
The purpose of the VideoFormatModule is to provide common Video formats
information to other Software Modules. It does not require any chip specific
data, and therefore does not require a handle.

This module defines all possible Video formats, and allows the user to get
fixed information about the format such as: width, height, top porch, bottom
porch, front porch, back porch, whether the format is progressive, etc. These
sort of functions are required by most video applications and by graphics.
These values are constant from chip to chip. The included functions do no
access any memory or registers.
**************************************************************************=*/

/****************************************************************
 *  Macros
 ****************************************************************/

/* KLUDE: legacy 3DTV chipsets have picture size limitation, require special
   display format to support */
#if (3548 == BCHP_CHIP) || (3556 == BCHP_CHIP)
#define BFMT_LEGACY_3DTV_SUPPORT            (1)
#else
#define BFMT_LEGACY_3DTV_SUPPORT            (0)
#endif

/* The vertical refresh rate and pixel frequency scaling factor:
    For example, NTSC has vsync rate of 59.94Hz, its stored ulVertFreq value is
    	59.94 x BFMT_FREQ_FACTOR = 5994;
    720p has pixel frequency of 74.25MHz, its stored ulPxlFreq value is
    	74.25 x BFMT_FREQ_FACTOR = 7425; */
#define BFMT_FREQ_FACTOR        (100)

/* To be OBSOLETED: These masks are hitting the limitation of as new format
 * growth.  These are to be replace with BFMT_Vert_e*. */
/* The bit masks for picture vertical frequency, or refresh rate */
#define BFMT_VERT_INVALID            0x00000000
#define BFMT_VERT_50Hz               0x00000001
#define BFMT_VERT_59_94Hz            0x00000002
#define BFMT_VERT_60Hz               0x00000004
#define BFMT_VERT_70Hz               0x00000008
#define BFMT_VERT_72Hz               0x00000010
#define BFMT_VERT_75Hz               0x00000020
#define BFMT_VERT_85Hz               0x00000040
#define BFMT_VERT_23_976Hz           0x00000080
#define BFMT_VERT_24Hz               0x00000100
#define BFMT_VERT_25Hz               0x00000200
#define BFMT_VERT_29_97Hz            0x00000400
#define BFMT_VERT_30Hz               0x00000800
#define BFMT_VERT_56Hz               0x00001000
#define BFMT_VERT_87Hz               0x00002000
#define BFMT_VERT_48Hz               0x00004000
#define BFMT_VERT_47_952Hz           0x00008000
#define BFMT_VERT_66Hz               0x00010000

/* To be OBSOLETED: These masks are hitting the limitation of as new format
 * growth.  These are to be replace with BFMT_Pxl_e*. */
/* The bit masks for video pixel frequency, or sample rate */
/* Safe mode: 640x480p */
#define BFMT_PXL_25_2MHz             0x00000001
#define BFMT_PXL_25_2MHz_MUL_1_001   0x00000000 /* not used */
#define BFMT_PXL_25_2MHz_DIV_1_001   0x00000002

/* NTSC and 480p, NTSC is upsampled in VEC from 13.5MHz */
#define BFMT_PXL_27MHz               0x00000004
#define BFMT_PXL_27MHz_MUL_1_001     0x00000008 /* 480p @60Hz */
#define BFMT_PXL_27MHz_DIV_1_001     0x00000000 /* not used */

/* 1080i and 720p */
#define BFMT_PXL_74_25MHz            0x00000010
#define BFMT_PXL_74_25MHz_MUL_1_001  0x00000000 /* not used */
#define BFMT_PXL_74_25MHz_DIV_1_001  0x00000020

/* 1080p */
#define BFMT_PXL_148_5MHz            0x20000000 /* 60Hz */
#define BFMT_PXL_148_5MHz_DIV_1_001  0x40000000 /* 59.94 Hz */

/* 1080p60 3D
 * Reuse bits. BFMT_PXL_39_79MHz and BFMT_PXL_39_79MHz_DIV_1_001
 * are used for input only */
#define BFMT_PXL_297MHz              0x00000200 /* 60    Hz */
#define BFMT_PXL_297MHz_DIV_1_001    0x00000400 /* 59.94 Hz */

/* DTV: 1366x768p  */
#define	BFMT_PXL_56_304MHz           0x00000040 /* 50 Hz */
#define BFMT_PXL_67_565MHz           0x00000080 /* 60 Hz    */
#define BFMT_PXL_67_565MHz_MUL_1_001 0x00000000 /* not used */
#define BFMT_PXL_67_565MHz_DIV_1_001 0x00000100 /* 59.94 Hz */

/* VESA mode: 800x600p @ 60/59.94Hz */
#define BFMT_PXL_40MHz               0x00000200 /* obsoleted */
#define BFMT_PXL_39_79MHz            0x00000200 /* 60    Hz */
#define BFMT_PXL_39_79MHz_DIV_1_001  0x00000400 /* 59.94 Hz */

/* VESA mode: 1024x768p @ 60/59.94Hz */
#define BFMT_PXL_65MHz               0x00000800
#define BFMT_PXL_65MHz_DIV_1_001     0x00001000

/* VESA mode: 1280x768p @ 60/59.94Hz */
#define BFMT_PXL_65_286MHz           0x00002000
#define BFMT_PXL_65_286MHz_DIV_1_001 0x00004000

/* VESA mode: 1280x720p @ 50Hz */
#define BFMT_PXL_60_4656MHz          0x00008000
#define BFMT_PXL_60_375MHz           0x00008000 /* obsoleted */

/* VESA mode: 1280x720p @ 60/59.94Hz */
#define BFMT_PXL_74_375MHz           0x00010000 /* obsoleted */
#define BFMT_PXL_74_48MHz            0x00010000
#define BFMT_PXL_74_48MHz_DIV_1_001  0x00020000

/* VESA mode: 1280x720p @ 60/59.94Hz Reduced Blanking */
#define BFMT_PXL_64MHz               0x00040000 /* obsoleted */
#define BFMT_PXL_64_022MHz           0x00040000
#define BFMT_PXL_64_022MHz_DIV_1_001 0x00080000

/* More PC mode pixel rate */
#define BFMT_PXL_31_50MHz            0x00100000
#define BFMT_PXL_35_50MHz            0x00200000
#define BFMT_PXL_36_00MHz            0x00400000
#define BFMT_PXL_49_50MHz            0x00800000
#define BFMT_PXL_50_00MHz            0x01000000
#define BFMT_PXL_56_25MHz            0x02000000
#define BFMT_PXL_75_00MHz            0x04000000
#define BFMT_PXL_78_75MHz            0x08000000
#define BFMT_PXL_94_50MHz            0x10000000

#define BFMT_PXL_101MHz              0x20000000 /* 60Hz */
#define BFMT_PXL_121_75MHz           0x20000000 /* 60Hz */
#define BFMT_PXL_156MHz              0x40000000 /* 60Hz */
#define BFMT_PXL_162MHz              0x80000000 /* 60Hz */

/* VESA mode: 1360x768p @ 60Hz */
#define BFMT_PXL_85_5MHz             0x40000000
#define BFMT_PXL_85_5MHz_DIV_1_001   0x10000000

/* VESA mode: 1920x1080p@60Hz_Red */
#define BFMT_PXL_138_625MHz          0x20000000

/* 1366x768@60 */
#define BFMT_PXL_72_014MHz           0x80000000

/* 1024x768i@87 */
#define BFMT_PXL_44_900MHz           0x00000000/*not used*/

/* VESA mode: 640x480p_CVT @ 60Hz */
#define BFMT_PXL_23_75MHz            0x00100000
#define BFMT_PXL_23_75MHz_DIV_1_001  0x00200000

/* VESA mode: 1280x800p @ 60Hz */
#define BFMT_PXL_83_5MHz             0x00400000
#define BFMT_PXL_83_5MHz_DIV_1_001   0x00800000

/* VESA mode: 1280x1024p @ 60Hz */
#define BFMT_PXL_108MHz              0x01000000
#define BFMT_PXL_108MHz_DIV_1_001    0x02000000

/* VESA mode: 1440x900p @ 60Hz */
#define BFMT_PXL_106_5MHz            0x04000000
#define BFMT_PXL_106_5MHz_DIV_1_001  0x08000000

/* 480P output, with 54 MHz pixel sampling with pixel doubling */
#define BFMT_PXL_54MHz               0x00010000
#define BFMT_PXL_54MHz_MUL_1_001     0x00020000

/* Check with HDM PI owner before modifying BFMT_IS_VESA_MODE and
 * BFMT_SUPPORT_HDMI lists */
#define BFMT_IS_VESA_MODE(fmt) \
	((BFMT_VideoFmt_eDVI_640x480p == (fmt)) || \
	 (BFMT_VideoFmt_eDVI_800x600p == (fmt)) || \
	 (BFMT_VideoFmt_eDVI_1024x768p == (fmt)) || \
	 (BFMT_VideoFmt_eDVI_1280x768p == (fmt)) || \
	 (BFMT_VideoFmt_eDVI_1280x720p == (fmt)) || \
	 (BFMT_VideoFmt_eDVI_640x480p_CVT == (fmt)) || \
	 (BFMT_VideoFmt_eDVI_1280x1024p_60Hz == (fmt)) || \
	 (BFMT_VideoFmt_eDVI_1360x768p_60Hz == (fmt)) || \
	 (BFMT_VideoFmt_eDVI_1366x768p_60Hz == (fmt)))

#define BFMT_SUPPORT_HDMI(fmt) \
	((BFMT_VideoFmt_eNTSC == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_B == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_B1 == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_D == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_D1 == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_G == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_H == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_I == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_K == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_M == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_N == (fmt)) || \
	 (BFMT_VideoFmt_ePAL_NC == (fmt)) || \
	 (BFMT_VideoFmt_e1080i == (fmt)) || \
	 (BFMT_VideoFmt_e1080p == (fmt)) || \
	 (BFMT_VideoFmt_e720p == (fmt)) || \
	 (BFMT_VideoFmt_e720p_60Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e720p_50Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e480p == (fmt)) || \
	 (BFMT_VideoFmt_e576p_50Hz == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_24Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_30Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_60Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_60Hz_3DLR == (fmt)) || \
	 (BFMT_VideoFmt_e3840x2160p_24Hz == (fmt)) || \
	 (BFMT_VideoFmt_e4096x2160p_24Hz == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_24Hz == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_25Hz == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_30Hz == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_50Hz == (fmt)) || \
	 (BFMT_VideoFmt_e1080i_50Hz == (fmt)) || \
	 (BFMT_VideoFmt_e720p_24Hz == (fmt)) || \
	 (BFMT_VideoFmt_e720p_50Hz == (fmt)) || \
	 (BFMT_VideoFmt_eCUSTOM_1366x768p == (fmt)) || \
	 (BFMT_VideoFmt_e720x482_NTSC == (fmt)) || \
	 (BFMT_VideoFmt_e720x482_NTSC_J == (fmt)) || \
	 (BFMT_VideoFmt_e720x483p == (fmt)) || \
	 (BFMT_IS_VESA_MODE(fmt)))

#define BFMT_IS_3D_MODE(fmt) \
	((BFMT_VideoFmt_e720p_60Hz_3DOU_AS  == (fmt)) || \
	 (BFMT_VideoFmt_e720p_50Hz_3DOU_AS  == (fmt)) || \
	 (BFMT_VideoFmt_e720p_30Hz_3DOU_AS  == (fmt)) || \
	 (BFMT_VideoFmt_e720p_24Hz_3DOU_AS  == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_24Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_30Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_60Hz_3DOU_AS == (fmt)) || \
	 (BFMT_VideoFmt_e1080p_60Hz_3DLR == (fmt)))

/***************************************************************************
Summary:
	This macro are commonly used to described a format.

See Also:
	BFMT_GetVideoFormatInfoPtr
***************************************************************************/
#define BFMT_NTSC_WIDTH                (720)
#define BFMT_NTSC_HEIGHT               (480)

#define BFMT_PAL_WIDTH                 (720)
#define BFMT_PAL_HEIGHT                (576)

#define BFMT_PAL_M_WIDTH               BFMT_NTSC_WIDTH    /* Yes! */
#define BFMT_PAL_M_HEIGHT              BFMT_NTSC_HEIGHT

#define BFMT_PAL_N_WIDTH               BFMT_PAL_WIDTH
#define BFMT_PAL_N_HEIGHT              BFMT_PAL_HEIGHT

#define BFMT_PAL_NC_WIDTH              BFMT_PAL_WIDTH
#define BFMT_PAL_NC_HEIGHT             BFMT_PAL_HEIGHT

#define BFMT_SECAM_WIDTH               BFMT_PAL_WIDTH
#define BFMT_SECAM_HEIGHT              BFMT_PAL_HEIGHT

#define BFMT_DVI_480P_WIDTH            (640)
#define BFMT_DVI_480P_HEIGHT           (480)

#define BFMT_DVI_600P_WIDTH            (800)
#define BFMT_DVI_600P_HEIGHT           (600)

#define BFMT_DVI_768P_WIDTH            (1024)
#define BFMT_DVI_768P_HEIGHT           (768)

#define BFMT_DVI_720P_WIDTH            (1280)
#define BFMT_DVI_720P_HEIGHT           (720)

#define BFMT_480P_WIDTH                BFMT_NTSC_WIDTH
#define BFMT_480P_HEIGHT               (480)

#define BFMT_576P_WIDTH                BFMT_PAL_WIDTH
#define BFMT_576P_HEIGHT               BFMT_PAL_HEIGHT

#define BFMT_720P_WIDTH                (1280)
#define BFMT_720P_HEIGHT               (720)

#define BFMT_1080I_WIDTH               (1920)
#define BFMT_1080I_HEIGHT              (1080)

#define BFMT_1080P_WIDTH               BFMT_1080I_WIDTH
#define BFMT_1080P_HEIGHT              BFMT_1080I_HEIGHT

#define BFMT_1080P3D_HEIGHT            (1125+1080)
#define BFMT_720P3D_HEIGHT             (750+720)


/***************************************************************************
Summary:
	Used to specify the possible aspect ratios.

Description:
	The values assigned to these enumerations should be kept in step with
	the ISO 13818-2 specification to minimize effort converting to these
	types.

See Also:
****************************************************************************/
typedef enum
{
	BFMT_AspectRatio_eUnknown   = 0, /* Unkown/Reserved */
	BFMT_AspectRatio_eSquarePxl = 1, /* square pixel */
	BFMT_AspectRatio_e4_3          , /* 4:3 */
	BFMT_AspectRatio_e16_9         , /* 16:9 */
	BFMT_AspectRatio_e221_1        , /* 2.21:1 */
	BFMT_AspectRatio_e15_9,          /* 15:9 */
	BFMT_AspectRatio_eSAR            /* no DAR available, use SAR instead */

} BFMT_AspectRatio;

/***************************************************************************
Summary:
	Used to specify orientation of the format.

Description:
	BFMT_Orientation defines all possible orientation of a format.

See Also:
****************************************************************************/
typedef enum
{
	BFMT_Orientation_e2D = 0,         /* 2D */
	BFMT_Orientation_e3D_LeftRight,   /* 3D left right */
	BFMT_Orientation_e3D_OverUnder,   /* 3D over under */
	BFMT_Orientation_e3D_Left,        /* 3D left */
	BFMT_Orientation_e3D_Right,       /* 3D right */
	BFMT_Orientation_eLeftRight_Enhanced  /* multi-resolution 3D */

} BFMT_Orientation;

/***************************************************************************
Summary:
	Used to specify veritical refresh rate (vsync rate) of format.

Description:
	BFMT_Vert defines all possible refresh rate of a format.

See Also:
****************************************************************************/
typedef enum
{
	BFMT_Vert_eInvalid = 0,
	BFMT_Vert_e23_976Hz,
	BFMT_Vert_e24Hz,
	BFMT_Vert_e25Hz,
	BFMT_Vert_e29_97Hz,
	BFMT_Vert_e30Hz,
	BFMT_Vert_e48Hz,
	BFMT_Vert_e50Hz,
	BFMT_Vert_e59_94Hz,
	BFMT_Vert_e60Hz,
	BFMT_Vert_eLast

} BFMT_Vert;


/***************************************************************************
Summary:
	This enumeration contains the Video formats available

Description:
	BFMT_VideoFmt defines all possible standard Video formats for HD and
	SD modes. Modes that are supported for DVI outputs only, will have
	the DVI identification.

	BFMT_VideoFmt_e1080i will set 29.97 or 30 Hz, to match with the input
	source. BFMT_VideoFmt_e720p will set 59.94 or 60 Hz, depending on the
	input source.

See Also:
	BVDC_Display_SetVideoFormat, BVDC_Display_GetVideoFormat,
	BFMT_GetVideoFormatInfoPtr
***************************************************************************/
typedef enum
{
	BFMT_VideoFmt_eNTSC = 0,                   /* 480i, NTSC-M for North America */
	BFMT_VideoFmt_eNTSC_J,                     /* 480i (Japan) */
	BFMT_VideoFmt_eNTSC_443,                   /* NTSC-443 */
	BFMT_VideoFmt_ePAL_B,                      /* Australia */
	BFMT_VideoFmt_ePAL_B1,                     /* Hungary */
	BFMT_VideoFmt_ePAL_D,                      /* China */
	BFMT_VideoFmt_ePAL_D1,                     /* Poland */
	BFMT_VideoFmt_ePAL_G,                      /* Europe */
	BFMT_VideoFmt_ePAL_H,                      /* Europe */
	BFMT_VideoFmt_ePAL_K,                      /* Europe */
	BFMT_VideoFmt_ePAL_I,                      /* U.K. */
	BFMT_VideoFmt_ePAL_M,                      /* 525-lines (Brazil) */
	BFMT_VideoFmt_ePAL_N,                      /* Jamaica, Uruguay */
	BFMT_VideoFmt_ePAL_NC,                     /* N combination (Argentina) */
	BFMT_VideoFmt_ePAL_60,                     /* 60Hz PAL */
	BFMT_VideoFmt_eSECAM_L,                    /* France */
	BFMT_VideoFmt_eSECAM_B,                    /* Middle East */
	BFMT_VideoFmt_eSECAM_G,                    /* Middle East */
	BFMT_VideoFmt_eSECAM_D,                    /* Eastern Europe */
	BFMT_VideoFmt_eSECAM_K,                    /* Eastern Europe */
	BFMT_VideoFmt_eSECAM_H,                    /* Line SECAM */
	BFMT_VideoFmt_e1080i,                      /* HD 1080i */
	BFMT_VideoFmt_e1080p,                      /* HD 1080p 60/59.94Hz, SMPTE 274M-1998 */
	BFMT_VideoFmt_e720p,                       /* HD 720p */
	BFMT_VideoFmt_e720p_60Hz_3DOU_AS,          /* HD 3D 720p */
	BFMT_VideoFmt_e720p_50Hz_3DOU_AS,          /* HD 3D 720p50 */
	BFMT_VideoFmt_e720p_30Hz_3DOU_AS,          /* HD 3D 720p30 */
	BFMT_VideoFmt_e720p_24Hz_3DOU_AS,          /* HD 3D 720p24 */
	BFMT_VideoFmt_e480p,                       /* HD 480p */
	BFMT_VideoFmt_e1080i_50Hz,                 /* HD 1080i 50Hz, 1125 sample per line, SMPTE 274M */
	BFMT_VideoFmt_e1080p_24Hz_3DOU_AS,         /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
	BFMT_VideoFmt_e1080p_30Hz_3DOU_AS,         /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */
	BFMT_VideoFmt_e1080p_60Hz_3DOU_AS,         /* HD 1080p 60Hz, 2200 sample per line  */
	BFMT_VideoFmt_e1080p_60Hz_3DLR,            /* HD 1080p 60Hz, 4400 sample per line  */
	BFMT_VideoFmt_e1080p_24Hz,                 /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
	BFMT_VideoFmt_e1080p_25Hz,                 /* HD 1080p 25Hz, 2640 sample per line, SMPTE 274M-1998 */
	BFMT_VideoFmt_e1080p_30Hz,                 /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */
	BFMT_VideoFmt_e1080p_50Hz,                 /* HD 1080p 50Hz. */
	BFMT_VideoFmt_e1250i_50Hz,                 /* HD 1250i 50Hz, another 1080i_50hz standard SMPTE 295M */
	BFMT_VideoFmt_e720p_24Hz,                  /* HD 720p 23.976/24Hz, 750 line, SMPTE 296M */
	BFMT_VideoFmt_e720p_25Hz,                  /* HD 720p 25Hz, 750 line, SMPTE 296M */
	BFMT_VideoFmt_e720p_30Hz,                  /* HD 720p 30Hz, 750 line, SMPTE 296M */
	BFMT_VideoFmt_e720p_50Hz,                  /* HD 720p 50Hz (Australia) */
	BFMT_VideoFmt_e576p_50Hz,                  /* HD 576p 50Hz (Australia) */
	BFMT_VideoFmt_e240p_60Hz,                  /* NTSC 240p */
	BFMT_VideoFmt_e288p_50Hz,                  /* PAL 288p */
	BFMT_VideoFmt_e1440x480p_60Hz,             /* CEA861B */
	BFMT_VideoFmt_e1440x576p_50Hz,             /* CEA861B */
	BFMT_VideoFmt_e3840x2160p_24Hz,            /* 3840x2160 24Hz */
	BFMT_VideoFmt_e4096x2160p_24Hz,            /* 4096x2160 24Hz */
#if BFMT_LEGACY_3DTV_SUPPORT
	BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz,    /* 3548 LVDS output for legacy 3DTV support */
	BFMT_VideoFmt_eCUSTOM1920x2160i_60Hz,    /* 3548 LVDS output for legacy 3DTV support */
#endif
	BFMT_VideoFmt_eCUSTOM_1440x240p_60Hz,      /* 240p 60Hz 7411 custom format. */
	BFMT_VideoFmt_eCUSTOM_1440x288p_50Hz,      /* 288p 50Hz 7411 custom format. */
	BFMT_VideoFmt_eCUSTOM_1366x768p,           /* Custom 1366x768 mode */
	BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz,      /* Custom 1366x768 50Hz mode */
	BFMT_VideoFmt_eDVI_640x480p,               /* DVI Safe mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x480p_CVT,           /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_800x600p,               /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1024x768p,              /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x768p,              /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x768p_Red,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x720p_50Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x720p,              /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x720p_Red,          /* DVI VESA mode for computer monitors */

	/* Added for HDMI/HDDVI input support!  VEC does not support these timing format!
	 * Convention: BFMT_VideoFmt_eDVI_{av_width}x{av_height}{i/p}_{RefreshRateInHz}.
	 * Eventually VEC can output all these timing formats when we get the microcodes
	 * for it.  Currently there are no microcode for these yet. */
	BFMT_VideoFmt_eDVI_640x350p_60Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x350p_70Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x350p_72Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x350p_75Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x350p_85Hz,          /* DVI VESA mode for computer monitors */

	BFMT_VideoFmt_eDVI_640x400p_60Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x400p_70Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x400p_72Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x400p_75Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x400p_85Hz,          /* DVI VESA mode for computer monitors */

	BFMT_VideoFmt_eDVI_640x480p_66Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x480p_70Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x480p_72Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x480p_75Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_640x480p_85Hz,          /* DVI VESA mode for computer monitors */

	BFMT_VideoFmt_eDVI_720x400p_60Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_720x400p_70Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_720x400p_72Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_720x400p_75Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_720x400p_85Hz,          /* DVI VESA mode for computer monitors */

	BFMT_VideoFmt_eDVI_800x600p_56Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_800x600p_59Hz_Red,      /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_800x600p_70Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_800x600p_72Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_800x600p_75Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_800x600p_85Hz,          /* DVI VESA mode for computer monitors */

	BFMT_VideoFmt_eDVI_1024x768p_66Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1024x768p_70Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1024x768p_72Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1024x768p_75Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1024x768p_85Hz,         /* DVI VESA mode for computer monitors */

	BFMT_VideoFmt_eDVI_1280x720p_70Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x720p_72Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x720p_75Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x720p_85Hz,         /* DVI VESA mode for computer monitors */

	/* New DVI or PC vdec input support */
	BFMT_VideoFmt_eDVI_1024x768i_87Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1152x864p_75Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x768p_75Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x768p_85Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x800p_60Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x960p_60Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x960p_85Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x1024p_60Hz,        /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x1024p_69Hz,        /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x1024p_75Hz,        /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1280x1024p_85Hz,        /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_832x624p_75Hz,          /*   MAC-16 */
	BFMT_VideoFmt_eDVI_1360x768p_60Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1366x768p_60Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1400x1050p_60Hz,        /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red,    /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1400x1050p_75Hz,        /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1600x1200p_60Hz,        /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red,    /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_848x480p_60Hz,          /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1064x600p_60Hz,         /* DVI VESA mode for computer monitors */
	BFMT_VideoFmt_eDVI_1440x900p_60Hz,         /* DVI VESA mode for computer monitors */

	/* SW7435-276: New format enums for 482/483 */
	BFMT_VideoFmt_e720x482_NTSC,               /* 720x482i NSTC-M for North America */
	BFMT_VideoFmt_e720x482_NTSC_J,             /* 720x482i Japan */
	BFMT_VideoFmt_e720x483p,                   /* 720x483p */

	/* statics: custom formats */
	BFMT_VideoFmt_eCustom0,         /* 59.94/60 Hz */
	BFMT_VideoFmt_eCustom1,         /* 50 Hz */

	/* dynamics: custom format */
	BFMT_VideoFmt_eCustom2,         /* defined at run time by app */

	/* Must be last */
	BFMT_VideoFmt_eMaxCount         /* Counter. Do not use! */

} BFMT_VideoFmt;

/* NOTE: These names could go away anytime better change to use the real
 * one from the enumerations above. */
/* Define BFMT_VideoFmt_eSECAM for backwards compatibility */
#define BFMT_VideoFmt_eSECAM BFMT_VideoFmt_eSECAM_L
#define BFMT_VideoFmt_eDVI_1280x720p_ReducedBlank BFMT_VideoFmt_eDVI_1280x720p_Red

#define BFMT_VideoFmt_e3D_720p         BFMT_VideoFmt_e720p_60Hz_3DOU_AS
#define BFMT_VideoFmt_e3D_720p_50Hz    BFMT_VideoFmt_e720p_50Hz_3DOU_AS
#define BFMT_VideoFmt_e3D_720p_30Hz    BFMT_VideoFmt_e720p_30Hz_3DOU_AS
#define BFMT_VideoFmt_e3D_720p_24Hz    BFMT_VideoFmt_e720p_24Hz_3DOU_AS
#define BFMT_VideoFmt_e3D_1080p_24Hz   BFMT_VideoFmt_e1080p_24Hz_3DOU_AS
#define BFMT_VideoFmt_e3D_1080p_30Hz   BFMT_VideoFmt_e1080p_30Hz_3DOU_AS

/***************************************************************************
Summary:
	This structure describes a custom format tables to be used to program VEC
	and DVO.

Description:
	eVideoFmt has to be a custom format;
	for now, it only supports DVO master mode;
	a custom format cannot have both 50 and 60 Hz rate manager entries since
	they are different custom formats; a custom format only tracks either
	60/59.94Hz or 50Hz frame rate;

	pDvoMicrocodeTbl - Microcode for the custom format timing such as LCD
	panel.

	pDvoRmTbl0 - Rate manager setting for given refresh rate.  Such as 60.00Hz,
	50.00Hz, 120.00Hz, etc.

	pDvoRmTbl1 - Rate manager setting for given refresh rate.  Such as 59.94Hz,
	50.00Hz, 120.00Hz, etc.  But this is a frame drop version of pDvoRmTbl0.
	For example if pDvoRmTbl0 is 60.00Hz, then pDvoRmTbl1 is 60.00/1.001 Hz

See Also:
	BFMT_SetCustomFormatInfo
***************************************************************************/
typedef struct
{
	/* 64-dword array */
	void                               *pDvoMicrocodeTbl;
	void                               *pDvoRmTbl0;
	void                               *pDvoRmTbl1;

} BFMT_CustomFormatInfo;

/***************************************************************************
Summary:
	This structure contains the display format information

Description:
	BFMT_VideoInfo provides the Video data for a Video format, such
	as screen size, resolution, type, and name associated with the format.

	eVideoFmt            - This video format enumeration.  Should match
		with the enum that user pass in to query the information.

	ulWidth              - The active analog screen width.  With given
		eVideoFmt the VEC can output multiple paths.  For example with NTSC
		the analog height, and its counter part HDMI output have different size.

	ulHeight             -  The active analog screen width.  With given
		eVideoFmt the VEC can output multiple paths.  For example with NTSC
		the analog height, and its counter part HDMI output have different size.

	ulDigitalWidth       - The active digital screen width

	ulDigitalHeight      - The active digital screen height

	ulScanWidth          - The total rasterization width which include
		blanking and active video.

	ulScanHeight         - The rasterization height which include blanking and
		active video.

	ulTopActive          - The start active video line of the top field or frame.

	ulTopMaxVbiPassThru  - Maximum VBI Pass Through lines at the top field or frame

	ulBotMaxVbiPassThru  - Maximum VBI Pass Through lines at the bottom field

	ulActiveSpace        - Active space for the format. Specify number of pixels
		between the left and right buffers for 3D format. Should be 0 for all 2D
		formats.

	ulVertFreqMask       - To be obsoleted!  Use ulVertFreq.

	ulVertFreq           - To be obsoleted! picture vertical frequency, or
		refresh rate in units of 1/100th Hz.  For example 60.00Hz would be
		6000, and 59.94Hz would be 5994.

	ulPxlFreqMask        - To be obsoleted!  Use ulPxlFreq.

	bInterlaced          - Indicate if the format is interlaced or progressive
		mode.

	eAspectRatio         - Default Aspect Ratio associated with this format.

	eOrientation         - Default orientation associated with the format.

	ulPxlFreq            - To be obsoleted!  Pixel frequencies
		in units of 1/100th Mhz.  For example 74.24Mhz would be 7425, and
		148.50Mhz would be 14850.

	pchFormatStr         - Video format name.

	pCustomInfo          - Custom format info, including microcode/rm tables;
		NULL for non-custom formats;  This is mainly for DVO output.

See Also:
	BFMT_GetVideoFormatInfoPtr
***************************************************************************/
typedef struct
{
	BFMT_VideoFmt                      eVideoFmt;
	uint32_t                           ulWidth;
	uint32_t                           ulHeight;
	uint32_t                           ulDigitalWidth;
	uint32_t                           ulDigitalHeight;
	uint32_t                           ulScanWidth;
	uint32_t                           ulScanHeight;
	uint32_t                           ulTopActive;
	uint32_t                           ulTopMaxVbiPassThru;
	uint32_t                           ulBotMaxVbiPassThru;
	uint32_t                           ulActiveSpace;
	uint32_t                           ulVertFreqMask;
	uint32_t                           ulVertFreq; /* in 1 /BFMT_FREQ_FACTOR Hz */
	uint32_t                           ulPxlFreqMask;
	bool                               bInterlaced;
	BFMT_AspectRatio                   eAspectRatio;
	BFMT_Orientation                   eOrientation;
	uint32_t                           ulPxlFreq;  /* in 1 /BFMT_FREQ_FACTOR MHz */
	const char                        *pchFormatStr;
	BFMT_CustomFormatInfo             *pCustomInfo;

} BFMT_VideoInfo;

/***************************************************************************
Summary:
	This function queries the Video information for a specific Video
	format.

Description:
	Returns the pointer to video information for a specific Video format,
	such as width, height, top/bottom/back/front porch, interlaced or
	progressive mode, etc.  Users can query the information for any Video
	format, whether or not it is supported by a particular chipset.
	Used by applications, SysLib, and PortingInterface modules.

	NOTE: for BFMT_VideoFmt_eCustom2 BFMT does not have the information
	of this format.  It is just an enumeration to indicate that this format
	timing will be define at runtime.  It will be loaded thru VDC via
	BVDC_Display_SetCustomVideoFormat() API.  The format information of is
	kept in application, and also can be query from VDC API
	BVDC_Display_GetCustomVideoFormat().

Input:
	eVideoFmt - Video format

Returns:
	This function return a 'const BFMT_VideoInfo*'.  If eVideoFmt is an
	invalid video format it will return a NULL pointer.  Or passing in
	BFMT_VideoFmt_eCustom2.

See Also:
	BVDC_Display_SetCustomVideoFormat

**************************************************************************/
const BFMT_VideoInfo* BFMT_GetVideoFormatInfoPtr
	( BFMT_VideoFmt                      eVideoFmt );

/***************************************************************************
 * Obsoleted!  Should be using BFMT_GetVideoFormatInfoPtr()
 ***************************************************************************/
BERR_Code BFMT_GetVideoFormatInfo
	( BFMT_VideoFmt                    eVideoFmt,
	  BFMT_VideoInfo                  *pVideoFmtInfo );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BFMT_H__ */
/* End of File */
