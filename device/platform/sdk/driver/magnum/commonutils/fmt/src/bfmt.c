/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfmt.c $
 * $brcm_Revision: Hydra_Software_Devel/108 $
 * $brcm_Date: 8/10/12 3:26p $
 *
 * Module Description:
 *   Video format module header file
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/fmt/bfmt.c $
 * 
 * Hydra_Software_Devel/108   8/10/12 3:26p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/107   10/7/11 4:09p yuxiaz
 * SW7425-974: Added new high pixel rate formats.
 *
 * Hydra_Software_Devel/106   3/17/11 6:09p tdo
 * SW7422-391: Incorrect VertRefreshRate return and add support for PAL 60
 * to allow multi rate
 *
 * Hydra_Software_Devel/105   12/21/10 11:03a tdo
 * SW7422-28: Change the size for 3D format to 1 eye window
 *
 * Hydra_Software_Devel/104   12/9/10 2:16p pntruong
 * SW35230-2250: Merged to mainline.
 *
 * Hydra_Software_Devel/103   9/7/10 3:03p yuxiaz
 * SW7422-28: Add 3D formats to FMT plus additional data structures
 *
 * Hydra_Software_Devel/SW7422-28/2   9/3/10 11:47a yuxiaz
 * SW7422-28: Rename BFMT_VideoFmt_e3D_1080p_30Hz  to
 * BFMT_VideoFmt_e1080p_30Hz_3DOU_AS.
 *
 * Hydra_Software_Devel/SW7422-28/1   9/2/10 4:08p yuxiaz
 * SW7422-28: Add 3D formats to FMT plus additional data structures
 *
 * Hydra_Software_Devel/102   9/7/10 5:38p zhang
 * SW35230-1039: Reverted check-in from Hydra. Will branch for 35230
 * instead.
 *
 * Hydra_Software_Devel/101   9/3/10 5:55p zhang
 * SW35230-1039: Added 3 custom 3D formats to support special timing for
 * output to BCM35425.
 *
 * Hydra_Software_Devel/100   7/13/10 11:27a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/99   5/20/10 5:47p darnstein
 * SW7125-326: add vertical frequency for 720P@30Hz video.
 *
 * Hydra_Software_Devel/98   5/20/10 4:29p darnstein
 * SW7125-326: Fix problem with pixel frequencies for 720P@50Hz.
 *
 * Hydra_Software_Devel/97   5/14/10 2:12p hongtaoz
 * SW3548-2680: fixed ulScanWidth of 2160i48;
 *
 * Hydra_Software_Devel/96   4/12/10 2:03p tdo
 * SW7420-673: add multi-frame rate support to SD
 *
 * Hydra_Software_Devel/95   4/9/10 3:20p tdo
 * SW7420-673: add multi-frame rate support to SD
 *
 * Hydra_Software_Devel/94   2/23/10 3:49p pntruong
 * SW3548-2764: Allows 2x oversample 480p/576p for hdmi input.
 *
 * Hydra_Software_Devel/93   2/10/10 1:25p pntruong
 * SW3548-2350: Updated line per frame for some of the format to match the
 * spreadsheet.
 *
 * Hydra_Software_Devel/92   2/1/10 6:34p hongtaoz
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
 * Hydra_Software_Devel/91   1/27/10 5:50p pntruong
 * SW3556-1027: Corrected the refresh pal_m, pal_60, and default blank.
 *
 * Hydra_Software_Devel/90   12/15/09 2:16p pntruong
 * SW3556-933: Display no signal when input signal is 1440x480@60Hz.
 *
 * Hydra_Software_Devel/89   10/16/09 5:21p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/88   10/15/09 5:37p darnstein
 * SW7405-3191: Previous check-in clobbered one of the pixel frequencies.
 *
 * Hydra_Software_Devel/87   10/15/09 4:53p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/86   10/12/09 3:40p tdo
 * SW7420-252: Fix PxlFreqMask for BFMT_VideoFmt_eDVI_1280x720p
 *
 * Hydra_Software_Devel/85   7/21/09 4:23p rpan
 * PR56037: Initial effort for analog VESA outputs.
 *
 * Hydra_Software_Devel/84   5/26/09 6:30p darnstein
 * PR48884: prevent crash when querying for properties of custom video
 * format.
 *
 * Hydra_Software_Devel/83   5/4/09 4:22p rpan
 * PR53106: Added new format 720p25/30Hz.
 *
 * Hydra_Software_Devel/82   3/31/09 8:22p pntruong
 * PR51930: Correct the pixel rate for 480p.
 *
 * Hydra_Software_Devel/81   3/18/09 6:04p pntruong
 * PR53234, PR53395: Added support for wvga, wsvga, sxga.
 *
 * Hydra_Software_Devel/80   3/4/09 11:19a pntruong
 * PR52568: Customer not needed the custom 1340x725p@60hz.
 *
 * Hydra_Software_Devel/79   3/2/09 3:05p pntruong
 * PR52568: Added support for custom format 1340x725p@60hz.
 *
 * Hydra_Software_Devel/78   2/13/09 2:27p pntruong
 * PR52089: Updated the data error in the format info for
 * BFMT_VideoFmt_eDVI_640x480p_66Hz.
 *
 * Hydra_Software_Devel/PR52089/1   2/13/09 4:03p gorgon
 * PR52089: Fix the data error in the format info
 *
 * Hydra_Software_Devel/77   10/9/08 10:43a pntruong
 * PR47156: Downgrade unknown format to just msg.
 *
 * Hydra_Software_Devel/76   9/29/08 7:55p pntruong
 * PR47156: Updated blanking values to match specs.
 *
 * Hydra_Software_Devel/75   9/20/08 10:56a pntruong
 * PR47149: Relax const to allow app to dynamically load of custom ucode.
 *
 * Hydra_Software_Devel/74   8/6/08 4:45p pntruong
 * PR45219:  Added new RGB formats.
 *
 * Hydra_Software_Devel/73   7/22/08 11:07a pntruong
 * PR45024: Make sure the ulHBlank is right in GetInputStatus for VGA
 * input.
 *
 * Hydra_Software_Devel/72   6/23/08 3:44p rpan
 * PR43939: Enabled 1080p@29.97.
 *
 * Hydra_Software_Devel/71   5/1/08 2:38p rpan
 * PR42109: Initial work for SECAM variants.
 *
 * Hydra_Software_Devel/70   3/26/08 10:24a hongtaoz
 * PR35398: added BFMT_FREQ_FACTOR to unify Vsync rate and pixel frequency
 * calculation;
 *
 * Hydra_Software_Devel/69   3/14/08 10:57a pntruong
 * PR40572: Fixed the standard raster line size for 576p.
 *
 * Hydra_Software_Devel/68   3/11/08 10:45p pntruong
 * PR40280: Added support for vdec PAL-60.
 *
 * Hydra_Software_Devel/PR40133/1   3/11/08 10:31a mlei
 * PR40280:need PAL-60 support on 3563
 *
 * Hydra_Software_Devel/67   12/3/07 11:43a pntruong
 * PR37790: [PC]: Remove two reduntant formats:
 * BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Cea; and
 * BFMT_VideoFmt_eDVI_1920x1080i_60Hz.
 *
 * Hydra_Software_Devel/gorgon_temp_20071203/1   12/3/07 2:34p gorgon
 * PR37790:[PC]: Remove two reduntant
 * formats:BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Cea&#12288;and
 * BFMT_VideoFmt_eDVI_1920x1080i_60Hz
 *
 * Hydra_Software_Devel/66   11/2/07 12:11p pntruong
 * PR34239: fixed segfault in hddvi auto detection.
 *
 * Hydra_Software_Devel/65   10/30/07 7:06p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/64   10/15/07 3:20p pntruong
 * PR35137: Add NTSC443 support for cvbs/Svideo.
 *
 * Hydra_Software_Devel/63   9/29/07 3:30p pntruong
 * PR33621: Fill in FMT entries for ulTopActive.
 *
 * Hydra_Software_Devel/PR33621/1   9/29/07 5:06p gorgon
 * PR33621:Fill in FMT entries for ulTopActive
 *
 * Hydra_Software_Devel/62   8/17/07 11:44a pntruong
 * PR34059: Need to fix FMT name for 1024x768i_43Hz.
 *
 * Hydra_Software_Devel/PR34059/1   8/17/07 11:40a gorgon
 * PR34059: Need to fix FMT name for 1024x768i_43Hz
 *
 * Hydra_Software_Devel/61   8/16/07 6:22p tdo
 * PR34007: [VDEC] Add support for 50Hz formats
 * (576p/720p/1080i/1080p/50Hz).
 *
 * Hydra_Software_Devel/60   7/31/07 2:45p pntruong
 * PR33182: Video abnormal when enable ANR for some format of
 * HDMI(640*350@60Hz/640*400@60Hz/70Hz).
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
 * Hydra_Software_Devel/57   7/3/07 2:09p pntruong
 * PR32080: Updated format info of vesa formats to relfect standard
 * values.
 *
 * Hydra_Software_Devel/56   6/27/07 6:25p pntruong
 * PR32553: Update line length for 1024x768i@43Hz.
 *
 * Hydra_Software_Devel/Custom_Hw_Issue/1   6/27/07 10:09p dyzhang
 * PR32553: Update line length for 1024x768i@43Hz
 *
 * Hydra_Software_Devel/55   6/18/07 6:20p pntruong
 * PR31619: Correct pixle rate setting for 1400x1050p_75hz.
 *
 * Hydra_Software_Devel/54   6/12/07 4:33p pntruong
 * PR32076, PR32078, PR32081: Updated pc-input pll settings.
 *
 * Hydra_Software_Devel/53   6/5/07 8:42p pntruong
 * PR31619: Add two new format: 1280x768p/60Hz reduced blank and
 * 1400x1050p/60Hz reduced blank.
 * PR31619: Update timing and PLL settings for 1280x768p/60Hz and
 * 1280x960p/60Hz
 * PR31619: Update timing and PLL settings for 1400x1050p/60Hz and
 * 1400x1050p/75Hz.
 * PR31619: Update ulScanWidth and PLL settings for 1280x1024p/60Hz,
 * 1280x1024p/75Hz and 1280x1024p/85Hz.
 * PR31619: Update ulScanWidth for 1024x768p/70Hz and 1024x768p/85Hz to
 * fix the waviness issue.
 * PR31619: Update 640x350p/85Hz VStart.
 *
 * Hydra_Software_Devel/52   5/23/07 9:13p pntruong
 * PR31558: Add support for new PC input format 1600x1200p@60Hz.
 *
 * Hydra_Software_Devel/51   5/18/07 2:46p yuxiaz
 * PR28810: Merge in 832x624 support from PR28810 branch.
 *
 * Hydra_Software_Devel/PR28810/1   5/16/07 11:16a honglang
 * PR28810: add format 832x624 support
 *
 * Hydra_Software_Devel/50   4/11/07 7:33p pntruong
 * PR29483: PC input. Add two supported formats:720x400@70Hz and
 * 640x350@70Hz.  Also not include the dummy adc/edsafe regs in rul.
 *
 * Hydra_Software_Devel/49   3/7/07 5:21p pntruong
 * PR28477: Added parameters for 1152x864p@75Hz, 1280x768p@75/85Hz,
 * 1280x960p@60/85Hz, 1280x1024p@60/75/85Hz.
 *
 * Hydra_Software_Devel/48   2/5/07 4:06p pntruong
 * PR26677, PR23236: Fixed vertical frequency to be more accurate for
 * 640x480@72Hz and 800x600@72Hz.
 *
 * Hydra_Software_Devel/47   2/1/07 1:54p pntruong
 * PR26677, PR23236: Enhanced autoformat detection, added 1080p support.
 *
 * Hydra_Software_Devel/46   1/23/07 4:05p pntruong
 * PR23225: VDEC - PC input.  Updated values for FD detect for PC mode.
 *
 * Hydra_Software_Devel/45   1/21/07 2:31p pntruong
 * PR23225: VDEC - PC input.  Fine tune the other parameters for pc
 * formats.
 *
 * Hydra_Software_Devel/44   12/18/06 11:26p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   10/23/06 4:41p hongtaoz
 * PR22568, PR23188: updated 1600x1200p format info;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/4/06 6:57p hongtaoz
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
 * Hydra_Software_Devel/43   9/6/06 4:16p hongtaoz
 * PR22568, PR23188: added 1080p60 format for 3563;
 *
 * Hydra_Software_Devel/42   8/23/06 5:11p pntruong
 * PR23836: Please fix compiler warnings with: VDC, RDC, and FMT.
 *
 * Hydra_Software_Devel/41   8/16/06 3:25p pntruong
 * PR23522: Add supporting hdmi pc input 800x600@56Hz and 800x600@85Hz
 *
 * Hydra_Software_Devel/40   8/7/06 11:58a pntruong
 * PR22494: Support HDDVI new trigger in 3560 B0 software.
 *
 * Hydra_Software_Devel/39   7/21/06 2:51p pntruong
 * PR20913: Need support for smaller formats to avoid clipping issues.
 * Need to use double the hblank as well.
 *
 * Hydra_Software_Devel/38   7/5/06 6:39p pntruong
 * PR20913: Need support for smaller formats to avoid clipping issues.
 * Made 240p and 288p a custom format for 7411 because of non-repeat
 * pixel support.
 *
 * Hydra_Software_Devel/37   6/19/06 4:49p pntruong
 * PR20913: Fixed the height.
 *
 * Hydra_Software_Devel/36   6/19/06 10:14a pntruong
 * PR20913: Need support for smaller formats to avoid clipping issues.
 * Added 240p60Hz and 288p50Hz hddvi input support.
 *
 * Hydra_Software_Devel/35   6/9/06 8:45a jessem
 * PR 19995: Added 720p 23.976/24Hz support.
 *
 * Hydra_Software_Devel/34   6/5/06 4:15p jessem
 * PR 19995: Added 1080p 24Hz support.
 *
 * Hydra_Software_Devel/33   4/19/06 1:47p hongtaoz
 * PR20938: redefined pixel frequency bit mask; updated some vesa mode
 * timing parameters to comply with DVT Format Support document, i.e.
 * reference software only supports 50/59.94/60 Hz trackable frame rate
 * for VESA formats;
 *
 * Hydra_Software_Devel/32   3/27/06 1:55p pntruong
 * PR20239: Added support for 1920x1080P@24Hz, 1920x1080P@30Hz formats on
 * 3560B0 on HD_DVI input.
 *
 * Hydra_Software_Devel/31   2/16/06 3:05p pntruong
 * PR17778: Update pixel clock rate for new formats.
 *
 * Hydra_Software_Devel/30   2/15/06 11:32a jessem
 * PR 17554: Added support for 1080p 30Hz.
 *
 * Hydra_Software_Devel/29   1/31/06 1:52p pntruong
 * PR17778: Support VESA input for HD_DVI, and merge in 3560B0 support.
 *
 * Hydra_Software_Devel/28   9/30/05 4:36p hongtaoz
 * PR12560: fixed a typo for BFMT_VideoFmt_eDVI_1280x720p_50Hz;
 *
 * Hydra_Software_Devel/27   9/22/05 2:40p hongtaoz
 * PR12388, PR15820, PR15563, PR15685: fixed eCutome_1366x768p format
 * raster sizes; fixed 480p/576p etc formats' pixel frequency values;
 *
 * Hydra_Software_Devel/26   9/21/05 7:56p albertl
 * PR12388:  Added 1366x768 50Hz and removed 50Hz clock from old 1366x768.
 *
 * Hydra_Software_Devel/25   6/22/05 2:45p pntruong
 * PR15820, PR15563, PR15685: Added pixel frequency to aid compute
 * bandwith equation.
 *
 * Hydra_Software_Devel/24   6/13/05 4:38p albertl
 * PR12560:  Changed DVI 1280x768 to 15:9 aspect ratio.
 *
 * Hydra_Software_Devel/23   2/14/05 1:56p rgreen
 * PR 13448: Modify DigitalHeight of PAL_M from 483 to 480
 *
 * Hydra_Software_Devel/22   2/9/05 2:52p albertl
 * PR12560: Added BFMT_VideoFmt_eDVI_1280x768p,
 * BFMT_VideoFmt_eDVI_1280x720p_50Hz, BFMT_VideoFmt_eDVI_1280x720p, and
 * BFMT_VideoFmt_eDVI_1280x720p_Red formats.
 *
 * Hydra_Software_Devel/21   12/20/04 2:11p darnstein
 * PR 13506: Adapt to change BAVC_AspectRatio->BFMT_AspectRatio.
 *
 * Hydra_Software_Devel/20   12/16/04 5:29p albertl
 * PR12388:  Added 50Hz clock to 1366x768.
 *
 * Hydra_Software_Devel/19   12/9/04 12:45p pntruong
 * PR 13448:  Fixed typo on previous checked in.
 *
 * Hydra_Software_Devel/18   12/3/04 10:06a jasonh
 * PR 13448: Added support of digital format width/height to FMT module.
 *
 * Hydra_Software_Devel/17   11/5/04 4:23p hongtaoz
 * PR13180: fixed PAL M format info errors;
 *
 * Hydra_Software_Devel/16   10/22/04 11:41a hongtaoz
 * PR12162: set the bottom max vbi lines the same as top for progressive
 * mode;
 *
 * Hydra_Software_Devel/15   10/13/04 7:55p hongtaoz
 * PR12301: added the maximum VBI pass-through lines for each format;
 * removed top/bottom/left/right porches, instead directly use scan width
 * and height to describe the format rasterization information;
 *
 * Hydra_Software_Devel/14   9/22/04 12:32p pntruong
 * PR12728: Fixed -pdantic build warnings and errors.
 *
 * Hydra_Software_Devel/13   9/15/04 4:33p albertl
 * PR 12388:  Added 1366x768 format, and integrated required settings.
 *
 * Hydra_Software_Devel/12   9/9/04 11:31a hongtaoz
 * PR11868, PR11870: added VESA modes 800x600p and 1024x768p;
 *
 * Hydra_Software_Devel/11   9/2/04 11:18a hongtaoz
 * PR10827: adding dvi safe mode(640x480p) support;
 *
 * Hydra_Software_Devel/10   7/8/04 5:57p hongtaoz
 * PR11794: added 720p_50Hz and 1250i_50Hz support;
 *
 * Hydra_Software_Devel/9   6/16/04 5:42p jasonh
 * PR 11562: Added support for test format.
 *
 * Hydra_Software_Devel/8   5/4/04 6:44p hongtaoz
 * PR10073: added refresh rate to the BFMT module;
 * PR10942: added pixel frequency bit mask into format info; only expose
 * necessary format macroes;
 *
 * Hydra_Software_Devel/6   2/19/04 10:38a maivu
 * PR 9493: Removed generic PAL. Replaced with specific PAL modes:
 * B,B1,D,D1,G,H,I,K
 *
 * Hydra_Software_Devel/5   12/17/03 3:47p pntruong
 * PR 9027: Added BFMT_GetVideoFormatInfoPtr to BFMT.
 *
 * Hydra_Software_Devel/4   12/1/03 11:29a pntruong
 * Removed unnecessary usage of BKNI_Memcpy.
 *
 * Hydra_Software_Devel/3   10/9/03 8:40a pntruong
 * Added some common use constants.
 *
 * Hydra_Software_Devel/2   8/14/03 5:50p pntruong
 * Fixed PAL_M entry.
 *
 * Hydra_Software_Devel/1   7/22/03 1:43p pntruong
 * Initial verstion.
 *
 ***************************************************************************/

/* user-defined formats and microcodes for both eCustom0, and eCustom1 */
#include "bstd.h"
#include "bdbg.h"
#include "bfmt.h"
#include "bfmt_custom.c"

BDBG_MODULE(BFMT);

#define BFMT_P_MAKE_FMT(fmt, width, height, raster_width, raster_height,       \
	top_active, top_max_vbi, bot_max_vbi, active_space, vert_freq_mask,        \
	vert_freq, pxl_freq_mask, pxl_freq, interlaced, aspect_ratio, orientation) \
{                     \
	fmt,              \
	width,            \
	height,           \
	width,            \
	height,           \
	raster_width,     \
	raster_height,    \
	top_active,       \
	top_max_vbi,      \
	bot_max_vbi,      \
	active_space,     \
	vert_freq_mask,   \
	vert_freq,        \
	pxl_freq_mask ,   \
	interlaced,       \
	aspect_ratio,     \
	orientation,      \
	pxl_freq,         \
	#fmt,             \
	NULL              \
}

/* Fill entries in with blank. */
#define BFMT_P_MAKE_BLANK(fmt)    \
	BFMT_P_MAKE_FMT(              \
		fmt,                      \
		BFMT_NTSC_WIDTH,          \
		BFMT_NTSC_HEIGHT,         \
		858,                      \
		525,                      \
		22,                       \
		12,                       \
		12,                       \
		0,                        \
		BFMT_VERT_60Hz,           \
		6000,                     \
		BFMT_PXL_27MHz,           \
		13.50 * BFMT_FREQ_FACTOR, \
		true,                     \
		BFMT_AspectRatio_e4_3,    \
		BFMT_Orientation_e2D)

/* Video Format Information!  There are a couple of tables needed to be
 * updated beside this one.  Check VDC module!
 * (1) BVDC_P_aFormatInfoTable in bvdc_displayfmt_priv.c
 * (2) s_aFormatDataTable in bvdc_displayfmt_priv.c
 * (3) s_astShaperSettings in bvdc_displayfmt_priv.c
 *
 * TODO: Additional update Pixel Frequency needed! */
static const BFMT_VideoInfo s_aVideoFmtInfoTbls[] =
{
	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eNTSC,
		BFMT_NTSC_WIDTH,
		BFMT_NTSC_HEIGHT,
		858,
		525,
		22,
		12,
		12,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz | BFMT_PXL_27MHz_MUL_1_001,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eNTSC_J,
		BFMT_NTSC_WIDTH,
		BFMT_NTSC_HEIGHT,
		858,
		525,
		22,
		12,
		12,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz | BFMT_PXL_27MHz_MUL_1_001,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eNTSC_443,
		BFMT_NTSC_WIDTH,
		BFMT_NTSC_HEIGHT,
		858,
		525,
		22,
		12,
		12,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz | BFMT_PXL_27MHz_MUL_1_001,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_B,
		BFMT_PAL_WIDTH,
		BFMT_PAL_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_B1,
		BFMT_PAL_WIDTH,
		BFMT_PAL_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_D,
		BFMT_PAL_WIDTH,
		BFMT_PAL_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_D1,
		BFMT_PAL_WIDTH,
		BFMT_PAL_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_G,
		BFMT_PAL_WIDTH,
		BFMT_PAL_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_H,
		BFMT_PAL_WIDTH,
		BFMT_PAL_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_K,
		BFMT_PAL_WIDTH,
		BFMT_PAL_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_I,
		BFMT_PAL_WIDTH,
		BFMT_PAL_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_M,
		BFMT_PAL_M_WIDTH,
		BFMT_PAL_M_HEIGHT,
		858,
		525,
		22,
		12,
		12,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz | BFMT_PXL_27MHz_MUL_1_001,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_N,
		BFMT_PAL_N_WIDTH,
		BFMT_PAL_N_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_NC,
		BFMT_PAL_NC_WIDTH,
		BFMT_PAL_NC_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_ePAL_60,
		BFMT_NTSC_WIDTH,
		BFMT_NTSC_HEIGHT,
		858,
		525,
		22,
		12,
		12,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz | BFMT_PXL_27MHz_MUL_1_001,
		true,
		13.50 * BFMT_FREQ_FACTOR,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eSECAM_L,
		BFMT_SECAM_WIDTH,
		BFMT_SECAM_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eSECAM_B,
		BFMT_SECAM_WIDTH,
		BFMT_SECAM_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eSECAM_G,
		BFMT_SECAM_WIDTH,
		BFMT_SECAM_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eSECAM_D,
		BFMT_SECAM_WIDTH,
		BFMT_SECAM_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eSECAM_K,
		BFMT_SECAM_WIDTH,
		BFMT_SECAM_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eSECAM_H,
		BFMT_SECAM_WIDTH,
		BFMT_SECAM_HEIGHT,
		864,
		625,
		23,
		17,
		18,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080i,
		BFMT_1080I_WIDTH,
		BFMT_1080I_HEIGHT,
		2200,
		1125,
		21,
		14,
		15,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz_DIV_1_001 | BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p,
		BFMT_1080I_WIDTH,
		BFMT_1080I_HEIGHT,
		2200,
		1125,
		42,
		36,
		36,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
		148.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		1650,
		750,
		26,
		20,
		20,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz_DIV_1_001 | BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p_60Hz_3DOU_AS,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		1650,
		750 * 2,
		26,
		20,
		20,
		30,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
		148.5 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e3D_OverUnder),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p_50Hz_3DOU_AS,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		1980,
		750 * 2,
		26,
		20,
		20,
		30,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz,
		148.5 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e3D_OverUnder),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p_30Hz_3DOU_AS,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		3300,
		750 * 2,
		30,
		20,
		20,
		30,
		BFMT_VERT_30Hz,
		30 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
		148.5 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e3D_OverUnder),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p_24Hz_3DOU_AS,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		4125,
		750 * 2,
		30,
		20,
		20,
		30,
		BFMT_VERT_24Hz | BFMT_VERT_23_976Hz,
		24 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
		148.5 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e3D_OverUnder),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e480p,
		BFMT_480P_WIDTH,
		BFMT_480P_HEIGHT,
		858,
		525,
		43,
		30,
		30,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz_MUL_1_001 | BFMT_PXL_27MHz,
		27 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080i_50Hz,
		BFMT_1080I_WIDTH,
		BFMT_1080I_HEIGHT,
		2640,
		1125,
		21,
		14,
		15,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p_24Hz_3DOU_AS,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT,
		2750,
		1125 * 2,
		42,
		41,
		4,
		45,
		BFMT_VERT_24Hz | BFMT_VERT_23_976Hz,
		24 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
		148.5 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e3D_OverUnder),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p_30Hz_3DOU_AS,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT,
		2200,
		1125 * 2,
		42,
		41,
		4,
		45,
		BFMT_VERT_30Hz,
		30 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
		148.5 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e3D_OverUnder),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p_60Hz_3DOU_AS,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT,
		2200,
		1125 * 2,
		42,
		41,
		4,
		45,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_297MHz_DIV_1_001 | BFMT_PXL_297MHz,
		297 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e3D_OverUnder),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p_60Hz_3DLR,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT,
		2200 * 2,
		1125,
		42,
		41,
		4,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_297MHz_DIV_1_001 | BFMT_PXL_297MHz,
		297 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e3D_LeftRight),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p_24Hz,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT,
		2750,
		1125,
		42,
		41,
		4,
		0,
		BFMT_VERT_24Hz | BFMT_VERT_23_976Hz,
		24 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz_DIV_1_001 | BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p_25Hz,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT,
		2640,
		1125,
		42,
		41,
		4,
		0,
		BFMT_VERT_25Hz,
		25 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p_30Hz,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT,
		2200,
		1125,
		42,
		41,
		4,
		0,
		BFMT_VERT_30Hz,
		30 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz_DIV_1_001 | BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1080p_50Hz,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT,
		2640,
		1125,
		42,
		41,
		4,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz,
		148.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1250i_50Hz,
		BFMT_1080I_WIDTH,
		BFMT_1080I_HEIGHT,
		2376,
		1250,
		81,
		79,
		79,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p_24Hz,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		4125,
		750,
		26,
		20,
		20,
		0,
		BFMT_VERT_24Hz | BFMT_VERT_23_976Hz,
		24 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz_DIV_1_001 | BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p_25Hz,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		3960,
		750,
		26,
		20,
		20,
		0,
		BFMT_VERT_25Hz,
		25 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p_30Hz,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		3300,
		750,
		26,
		20,
		20,
		0,
		BFMT_VERT_29_97Hz | BFMT_VERT_30Hz,
		30 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz_DIV_1_001 | BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720p_50Hz,
		BFMT_720P_WIDTH,
		BFMT_720P_HEIGHT,
		1980,
		750,
		26,
		20,
		20,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz,
		74.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e576p_50Hz,
		BFMT_576P_WIDTH,
		BFMT_576P_HEIGHT,
		864,
		625,
		45,
		39,
		39,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		27 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e240p_60Hz,
		720,
		240,
		858 * 2,
		263,
		22,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz,
		59.94 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		27 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e288p_50Hz,
		720,
		BFMT_PAL_HEIGHT/2,
		864 * 2,
		313,
		23,
		0,
		0,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		27 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1440x480p_60Hz,
		BFMT_480P_WIDTH * 2,
		480,
		858 * 2,
		525,
		43,
		30,
		30,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_54MHz_MUL_1_001 | BFMT_PXL_54MHz,
		54 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e1440x576p_50Hz,
		BFMT_576P_WIDTH * 2,
		BFMT_576P_HEIGHT,
		864 * 2,
		625,
		45,
		39,
		39,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_54MHz,
		54 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e3840x2160p_24Hz,
		3840,
		2160,
		5500,
		2250,
		83,
		0,
		0,
		0,
		BFMT_VERT_24Hz | BFMT_VERT_23_976Hz,
		24 * BFMT_FREQ_FACTOR,
		BFMT_PXL_297MHz_DIV_1_001 | BFMT_PXL_297MHz,
		297 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e4096x2160p_24Hz,
		4096,
		2160,
		5500,
		2250,
		83,
		0,
		0,
		0,
		BFMT_VERT_24Hz,
		24 * BFMT_FREQ_FACTOR,
		BFMT_PXL_297MHz_DIV_1_001 | BFMT_PXL_297MHz,
		297 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

#if BFMT_LEGACY_3DTV_SUPPORT
	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT * 2,
		2750,
		1125 * 2,
		42,
		41,
		4,
		0,
		BFMT_VERT_48Hz | BFMT_VERT_47_952Hz,
		48 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
		148.5 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eCUSTOM1920x2160i_60Hz,
		BFMT_1080P_WIDTH,
		BFMT_1080P_HEIGHT * 2,
		2200,
		1125 * 2,
		42,
		41,
		4,
		0,
		BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
		148.5 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),
#endif

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eCUSTOM_1440x240p_60Hz,
		1440,
		240,
		858 * 2,
		263,
		22,
		0,
		0,
		0,
		BFMT_VERT_60Hz | BFMT_VERT_59_94Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		27 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eCUSTOM_1440x288p_50Hz,
		1440,
		BFMT_PAL_HEIGHT/2,
		864 * 2,
		313,
		23,
		0,
		0,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz,
		27 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eCUSTOM_1366x768p,
		1366,
		768,
		1440,
		782,
		13,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_67_565MHz_DIV_1_001 | BFMT_PXL_67_565MHz,
		67.56 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz,
		1366,
		768,
		1440,
		782,
		13,
		0,
		0,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_56_304MHz,
		56.30 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x480p,
		BFMT_DVI_480P_WIDTH,
		BFMT_DVI_480P_HEIGHT,
		800,
		525,
		35,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x480p_CVT,
		BFMT_DVI_480P_WIDTH,
		BFMT_DVI_480P_HEIGHT,
		800,
		525,
		35,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_23_75MHz | BFMT_PXL_23_75MHz_DIV_1_001,
		23.75 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	/* The following VESA display formats complies with
	   http://www.sj.broadcom.com/projects/dvt/Chip_Architecture/Clocking/Released/DVT_format_support.doc;
	   Note: 59.94/60 Hz support frame rate tracking for TV formats input; */
	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_800x600p,
		BFMT_DVI_600P_WIDTH,
		BFMT_DVI_600P_HEIGHT,
		1056,
		628,
		27,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_39_79MHz | BFMT_PXL_39_79MHz_DIV_1_001,
		39.79 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1024x768p,
		BFMT_DVI_768P_WIDTH,
		BFMT_DVI_768P_HEIGHT,
		1344,
		806,
		35,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_65MHz | BFMT_PXL_65MHz_DIV_1_001,
		64.99 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x768p,
		1280,
		BFMT_DVI_768P_HEIGHT,
		1664,
		798,
		27,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_65_286MHz | BFMT_PXL_65_286MHz_DIV_1_001,
		79.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e15_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x768p_Red,
		1280,
		BFMT_DVI_768P_HEIGHT,
		1440,
		790,
		19,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_65_286MHz | BFMT_PXL_65_286MHz_DIV_1_001,
		68.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e15_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x720p_50Hz,
		BFMT_DVI_720P_WIDTH,
		BFMT_DVI_720P_HEIGHT,
		1632,
		741,
		35,
		0,
		0,
		0,
		BFMT_VERT_50Hz,
		50 * BFMT_FREQ_FACTOR,
		BFMT_PXL_60_4656MHz,
		60.47 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x720p,
		BFMT_DVI_720P_WIDTH,
		BFMT_DVI_720P_HEIGHT,
		1664,
		746,
		35,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_25MHz | BFMT_PXL_74_25MHz_DIV_1_001,
		74.48 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x720p_Red,
		BFMT_DVI_720P_WIDTH,
		BFMT_DVI_720P_HEIGHT,
		1440,
		741,
		35,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_64_022MHz | BFMT_PXL_64_022MHz_DIV_1_001,
		64.02 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x350p_60Hz,
		640,
		350,
		800,
		525,
		75,
		0,
		0,
		0,
		BFMT_VERT_60Hz | BFMT_VERT_59_94Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x350p_70Hz,
		640,
		350,
		800,
		450,
		62,
		0,
		0,
		0,
		BFMT_VERT_70Hz,
		70 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x350p_72Hz,
		640,
		350,
		832,
		445,
		63,
		0,
		0,
		0,
		BFMT_VERT_72Hz,
		72 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x350p_75Hz,
		640,
		350,
		832,
		445,
		63,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x350p_85Hz,
		640,
		350,
		832,
		445,
		63,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85 * BFMT_FREQ_FACTOR,
		BFMT_PXL_31_50MHz,
		31.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x400p_60Hz,
		640,
		400,
		800,
		525,
		50,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x400p_70Hz,
		640,
		400,
		800,
		450,
		12,
		0,
		0,
		0,
		BFMT_VERT_70Hz,
		70 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x400p_72Hz,
		640,
		400,
		832,
		425,
		12,
		0,
		0,
		0,
		BFMT_VERT_72Hz,
		72 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x400p_75Hz,
		640,
		400,
		832,
		425,
		12,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x400p_85Hz,
		640,
		400,
		832,
		445,
		44,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85 * BFMT_FREQ_FACTOR,
		BFMT_PXL_31_50MHz,
		31.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x480p_66Hz,
		640,
		480,
		864,
		525,
		42,
		0,
		0,
		0,
		BFMT_VERT_66Hz,
		66.67 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		30.24 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x480p_70Hz,
		640,
		480,
		832,
		503,
		23,
		0,
		0,
		0,
		BFMT_VERT_70Hz,
		70 * BFMT_FREQ_FACTOR,
		BFMT_PXL_25_2MHz_DIV_1_001 | BFMT_PXL_25_2MHz,
		25.20 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x480p_72Hz,
		640,
		480,
		832,
		520,
		31,
		0,
		0,
		0,
		BFMT_VERT_72Hz,
		72.81 * BFMT_FREQ_FACTOR,
		BFMT_PXL_31_50MHz,
		31.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x480p_75Hz,
		640,
		480,
		840,
		500,
		19,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75 * BFMT_FREQ_FACTOR,
		BFMT_PXL_31_50MHz,
		31.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_640x480p_85Hz,
		640,
		480,
		832,
		509,
		28,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		8500,
		BFMT_PXL_36_00MHz,
		36 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_720x400p_60Hz,
		720,
		400,
		900,
		525,
		19,
		0,
		0,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_40MHz,
		40 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_720x400p_70Hz,
		720,
		400,
		900,
		449,
		36,
		0,
		0,
		0,
		BFMT_VERT_70Hz,
		70 * BFMT_FREQ_FACTOR,
		BFMT_PXL_31_50MHz,
		28.32 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_720x400p_72Hz,
		720,
		400,
		936,
		525,
		20,
		0,
		0,
		0,
		BFMT_VERT_72Hz,
		72 * BFMT_FREQ_FACTOR,
		BFMT_PXL_40MHz,
		40 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_720x400p_75Hz,
		720,
		400,
		936,
		525,
		21,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75 * BFMT_FREQ_FACTOR,
		BFMT_PXL_40MHz,
		40 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_720x400p_85Hz,
		720,
		400,
		936,
		446,
		45,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85 * BFMT_FREQ_FACTOR,
		BFMT_PXL_35_50MHz,
		35.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_800x600p_56Hz,
		800,
		600,
		1024,
		625,
		24,
		0,
		0,
		0,
		BFMT_VERT_56Hz,
		56 * BFMT_FREQ_FACTOR,
		BFMT_PXL_36_00MHz,
		36 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_800x600p_59Hz_Red,
		800,
		600,
		1024,
		624,
		21,
		0,
		0,
		0,
		BFMT_VERT_60Hz |BFMT_VERT_59_94Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		38.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_800x600p_70Hz,
		800,
		400,
		1056,
		628,
		28,
		0,
		0,
		0,
		BFMT_VERT_70Hz,
		70 * BFMT_FREQ_FACTOR,
		BFMT_PXL_50_00MHz,
		50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_800x600p_72Hz,
		800,
		600,
		1040,
		666,
		29,
		0,
		0,
		0,
		BFMT_VERT_72Hz,
		72.19 * BFMT_FREQ_FACTOR,
		BFMT_PXL_50_00MHz,
		50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_800x600p_75Hz,
		800,
		600,
		1056,
		625,
		24,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75 * BFMT_FREQ_FACTOR,
		BFMT_PXL_49_50MHz,
		49.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_800x600p_85Hz,
		800,
		600,
		1048,
		631,
		30,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85 * BFMT_FREQ_FACTOR,
		BFMT_PXL_56_25MHz,
		56.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1024x768p_66Hz,
		BFMT_DVI_768P_WIDTH,
		BFMT_DVI_768P_HEIGHT,
		1328,
		816,
		40,
		0,
		0,
		0,
		BFMT_VERT_66Hz,
		66.11 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		71.64 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1024x768p_70Hz,
		BFMT_DVI_768P_WIDTH,
		BFMT_DVI_768P_HEIGHT,
		1328,
		806,
		35,
		0,
		0,
		0,
		BFMT_VERT_70Hz,
		70 * BFMT_FREQ_FACTOR,
		BFMT_PXL_75_00MHz,
		75 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1024x768p_72Hz,
		BFMT_DVI_768P_WIDTH,
		BFMT_DVI_768P_HEIGHT,
		1344,
		806,
		35,
		0,
		0,
		0,
		BFMT_VERT_72Hz,
		72 * BFMT_FREQ_FACTOR,
		BFMT_PXL_65MHz,
		65 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1024x768p_75Hz,
		BFMT_DVI_768P_WIDTH,
		BFMT_DVI_768P_HEIGHT,
		1312,
		800,
		31,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75 * BFMT_FREQ_FACTOR,
		BFMT_PXL_78_75MHz,
		78.75 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1024x768p_85Hz,
		BFMT_DVI_768P_WIDTH,
		BFMT_DVI_768P_HEIGHT,
		1376,
		808,
		39,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85 * BFMT_FREQ_FACTOR,
		BFMT_PXL_94_50MHz,
		94.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x720p_70Hz,
		BFMT_DVI_720P_WIDTH,
		BFMT_DVI_720P_HEIGHT,
		1664,
		746,
		32,
		0,
		0,
		0,
		BFMT_VERT_70Hz,
		70.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_375MHz,
		74.37 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x720p_72Hz,
		BFMT_DVI_720P_WIDTH,
		BFMT_DVI_720P_HEIGHT,
		1664,
		746,
		33,
		0,
		0,
		0,
		BFMT_VERT_72Hz,
		72.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_375MHz,
		74.37 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x720p_75Hz,
		BFMT_DVI_720P_WIDTH,
		BFMT_DVI_720P_HEIGHT,
		1664,
		746,
		35,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_375MHz,
		74.37 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x720p_85Hz,
		BFMT_DVI_720P_WIDTH,
		BFMT_DVI_720P_HEIGHT,
		1664,
		746,
		39,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_74_375MHz,
		74.37 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1024x768i_87Hz,
		1024,
		768,
		1264,
		817,
		24,
		0,
		0,
		0,
		BFMT_VERT_87Hz,
		87.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_44_900MHz,
		44.90 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1152x864p_75Hz,
		1152,
		864,
		1152+448,
		864+36,
		35,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		108.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x768p_75Hz,
		1280,
		768,
		1696,
		805,
		34,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		102.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e15_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x768p_85Hz,
		1280,
		768,
		1712,
		809,
		38,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		117.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e15_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x800p_60Hz,
		1280,
		800,
		1680,
		828,
		28,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_83_5MHz | BFMT_PXL_83_5MHz_DIV_1_001,
		false,
		83.50 * BFMT_FREQ_FACTOR,
		BFMT_AspectRatio_e15_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x960p_60Hz,
		1280,
		960,
		1800,
		1000,
		39,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		108.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x960p_85Hz,
		1280,
		960,
		1728,
		1011,
		50,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		148.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x1024p_60Hz,
		1280,
		1024,
		1712,
		1066,
		39,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_108MHz | BFMT_PXL_108MHz_DIV_1_001,
		108.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x1024p_69Hz,
		1280,
		1024,
		1680,
		1063,
		38,
		0,
		0,
		0,
		BFMT_VERT_70Hz,
		69.99 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		125.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x1024p_75Hz,
		1280,
		1024,
		1688,
		1066,
		41,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		135.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1280x1024p_85Hz,
		1280,
		1024,
		1728,
		1072,
		47,
		0,
		0,
		0,
		BFMT_VERT_85Hz,
		85.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		157.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_832x624p_75Hz,
		832,
		624,
		1152,
		667,
		35,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_56_25MHz,
		56.25 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1360x768p_60Hz,
		1360,
		BFMT_DVI_768P_HEIGHT,
		1792,
		795,
		24,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_85_5MHz,
		85.50 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1366x768p_60Hz,
		1366,
		BFMT_DVI_768P_HEIGHT,
		1528,
		790,
		20,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_85_5MHz | BFMT_PXL_85_5MHz_DIV_1_001,
		85.5 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1400x1050p_60Hz,
		1400,
		1050,
		1864,
		1089,
		36,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_121_75MHz,
		121.75 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red,
		1400,
		1050,
		1560,
		1080,
		27,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_101MHz,
		101.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1400x1050p_75Hz,
		1400,
		1050,
		1896,
		1099,
		46,
		0,
		0,
		0,
		BFMT_VERT_75Hz,
		75.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_156MHz,
		156.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1600x1200p_60Hz,
		1600,
		1200,
		2160,
		1250,
		49,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_162MHz,
		162.00 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red,
		1920,
		1080,
		2080,
		1111,
		29,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_138_625MHz,
		138.63 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_848x480p_60Hz,
		848,
		480,
		1056,
		500,
		20,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		59.61 * BFMT_FREQ_FACTOR,
		BFMT_PXL_31_50MHz,
		31.48 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1064x600p_60Hz,
		1064,
		600,
		1352,
		624,
		24,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		59.81 * BFMT_FREQ_FACTOR,
		BFMT_PXL_36_00MHz,
		37.32 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_eDVI_1440x900p_60Hz,
		1440,
		900,
		1904,
		932,
		32,
		0,
		0,
		0,
		BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_106_5MHz | BFMT_PXL_106_5MHz_DIV_1_001,
		106.5 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e16_9,
		BFMT_Orientation_e2D),

	/* SW7435-276: New format enums for 482/483 */
	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720x482_NTSC,
		BFMT_NTSC_WIDTH,
		482,
		858,
		525,
		22,
		12,
		12,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz | BFMT_PXL_27MHz_MUL_1_001,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720x482_NTSC_J,
		BFMT_NTSC_WIDTH,
		482,
		858,
		525,
		22,
		12,
		12,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz | BFMT_PXL_27MHz_MUL_1_001,
		13.50 * BFMT_FREQ_FACTOR,
		true,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	BFMT_P_MAKE_FMT(
		BFMT_VideoFmt_e720x483p,
		BFMT_480P_WIDTH,
		483,
		858,
		525,
		43,
		30,
		30,
		0,
		BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
		60.00 * BFMT_FREQ_FACTOR,
		BFMT_PXL_27MHz_MUL_1_001 | BFMT_PXL_27MHz,
		27 * BFMT_FREQ_FACTOR,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D),

	/* custom formats placeholders*/
	BFMT_P_MAKE_BLANK(BFMT_VideoFmt_eCustom0),
	BFMT_P_MAKE_BLANK(BFMT_VideoFmt_eCustom1),
	BFMT_P_MAKE_BLANK(BFMT_VideoFmt_eCustom2),

	/* Must be last */
	BFMT_P_MAKE_BLANK(BFMT_VideoFmt_eMaxCount)
};

/* For table size sanity check */
#define BVDC_P_FMT_INFO_COUNT \
	(sizeof(s_aVideoFmtInfoTbls) / sizeof(BFMT_VideoInfo))

/***************************************************************************
 *
 */
BERR_Code BFMT_GetVideoFormatInfo
	( BFMT_VideoFmt                      eVideoFmt,
	  BFMT_VideoInfo                    *pVideoFmtInfo )
{
	BDBG_ENTER(BFMT_GetVideoFormatInfo);

	if(pVideoFmtInfo)
	{
		const BFMT_VideoInfo* info = BFMT_GetVideoFormatInfoPtr(eVideoFmt);
		if (info == 0x0)
			return BERR_TRACE (BERR_INVALID_PARAMETER);
		*pVideoFmtInfo = *((const BFMT_VideoInfo*)
			BFMT_GetVideoFormatInfoPtr(eVideoFmt));
	}

	BDBG_LEAVE(BFMT_GetVideoFormatInfo);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
const BFMT_VideoInfo* BFMT_GetVideoFormatInfoPtr
	( BFMT_VideoFmt                      eVideoFmt )
{
	const BFMT_VideoInfo *pVideoInfo = NULL;
	BDBG_ENTER(BFMT_GetVideoFormatInfoPtr);

	/* Table size sanity check!  Just in case someone added new format in fmt,
	 * but forgot to add the new into these table. */
#if (BDBG_DEBUG_BUILD)
	if(BVDC_P_FMT_INFO_COUNT != BFMT_VideoFmt_eMaxCount+1)
	{
		BDBG_ERR(("Format Look Up Table out of sync!"));
		BDBG_ASSERT(false);
	}
#endif
	BDBG_ASSERT(eVideoFmt < BFMT_VideoFmt_eMaxCount);

	if(BFMT_VideoFmt_eCustom0 == eVideoFmt)
	{
		pVideoInfo = &s_stFormatInfoCustom0;
	}
	else if(BFMT_VideoFmt_eCustom1 == eVideoFmt)
	{
		pVideoInfo = &s_stFormatInfoCustom1;
	}
	else if(BFMT_VideoFmt_eCustom2 == eVideoFmt)
	{
		BDBG_MSG(("User defined, bfmt does not have information!"));
		pVideoInfo = 0x0;
	}
	else
	{
		pVideoInfo = &s_aVideoFmtInfoTbls[eVideoFmt];
	}

	BDBG_LEAVE(BFMT_GetVideoFormatInfoPtr);
	return pVideoInfo;
}

/* End of File */
