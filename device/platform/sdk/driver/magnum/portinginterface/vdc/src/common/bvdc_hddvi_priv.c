/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_hddvi_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/278 $
 * $brcm_Date: 8/13/12 3:08p $
 *
 * Module Description:
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_hddvi_priv.c $
 * 
 * Hydra_Software_Devel/278   8/13/12 3:08p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 * 
 * Hydra_Software_Devel/277   8/10/12 3:27p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/276   11/23/11 11:31a tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/275   11/10/11 10:09a yuxiaz
 * SW7231-387: Fixed MUX_DATA_32 setting.
 *
 * Hydra_Software_Devel/274   11/10/11 10:03a yuxiaz
 * SW7231-387: Factor out Input timing and Deskew clk settings.
 *
 * Hydra_Software_Devel/273   11/8/11 3:26p yuxiaz
 * SW7231-387: Bring up HD_DVI using external HDMI nxp receiver.
 *
 * Hydra_Software_Devel/272   9/9/11 10:37a pntruong
 * SW7425-31: Initialized format detection placement with backward
 * compatible value.
 *
 * Hydra_Software_Devel/271   9/9/11 10:28a pntruong
 * SW7425-31: Adapted input select value changes in version 8.
 *
 * Hydra_Software_Devel/270   2/11/11 11:08a yuxiaz
 * SW7420-1178: Merge loopback support to mainline.
 *
 * Hydra_Software_Devel/269   12/21/10 3:04p yuxiaz
 * SW7422-28: Adjust to 3D format size change for HD_DVI.
 *
 * Hydra_Software_Devel/268   12/21/10 11:05a tdo
 * SW7422-28: Adjust to 3D format size change
 *
 * Hydra_Software_Devel/267   12/16/10 11:40p pntruong
 * SW3548-2987: Added 1080p@30hz 3DOU_AS hdmi input support.
 *
 * Hydra_Software_Devel/266   12/16/10 2:23p yuxiaz
 * SW7422-35: Move drain to BVDC_P_Drain_BuildFormatRul_isr
 *
 * Hydra_Software_Devel/265   12/16/10 10:52a yuxiaz
 * SW7422-35: Bring up 3d OU HD_DVI input.
 *
 * Hydra_Software_Devel/264   11/23/10 2:20p yuxiaz
 * SW7422-35: Added 3D support in HD_DVI.
 *
 * Hydra_Software_Devel/263   11/15/10 6:00p pntruong
 * SW7231-2: Initial support to make 7231 compile.
 *
 * Hydra_Software_Devel/262   11/12/10 3:56p pntruong
 * SW7425-31: Takes bvn yuv into account for hddvi input.  Fixed bad debug
 * register read on non-mfd source.  Updated scratch registers
 * availabliity.
 *
 * Hydra_Software_Devel/261   11/11/10 7:28p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/260   10/6/10 1:05p pntruong
 * SW7425-31: Initial support for hddvi.
 *
 * Hydra_Software_Devel/259   9/14/10 3:44p yuxiaz
 * SW7422-28: Use new 3D formats
 *
 * Hydra_Software_Devel/258   8/27/10 4:47p pntruong
 * SW3548-3070, SW3548-2795: Removed unnecessary 'resume' when resetting
 * the bridge error.
 *
 * Hydra_Software_Devel/257   7/30/10 11:10a yuxiaz
 * SW3548-2795: Reset HD_DVI for bridge error.
 *
 * Hydra_Software_Devel/256   7/13/10 11:28a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/255   5/7/10 7:10p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/254   4/19/10 10:15p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/253   4/7/10 11:30a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/252   4/5/10 4:07p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/251   3/17/10 4:17p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/250   2/23/10 3:49p pntruong
 * SW3548-2764: Allows 2x oversample 480p/576p for hdmi input.
 *
 * Hydra_Software_Devel/249   2/9/10 3:55p pntruong
 * SW3548-2764: DVI1440*480p@60 can't be supported.
 *
 * Hydra_Software_Devel/248   2/1/10 6:42p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/247   2/1/10 5:47p pntruong
 * SW3556-992: Improved dvi mode detection.
 *
 * Hydra_Software_Devel/246   1/14/10 2:19p pntruong
 * SW3556-992: Improved logic for detection of pixel replication for dvi
 * mode.
 *
 * Hydra_Software_Devel/BCM3D/4   1/29/10 6:56p hongtaoz
 * SW3548-2680, SW3548-2681: removed 1470p custom formats since they are
 * the same as 720p3D formats; added macro to wrap the 3548 specific
 * legacy 3DTV format; reduced window buffer allocation needed for
 * pulldown case; disable hddvi DE if invalid HVstart;
 *
 * Hydra_Software_Devel/BCM3D/3   1/29/10 2:17p hongtaoz
 * SW3548-2680, SW3548-2681: not to reset HVstart override for 1080p3D
 * input;
 *
 * Hydra_Software_Devel/BCM3D/2   1/15/10 4:53p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format 720p50 input support;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:09p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/245   10/30/09 11:55a pntruong
 * SW7420-417, SW3548-2581, SW7400-2584: HD-DVI, The rdb description for
 * MAX_PICTURE_LINES states that this should be programmed to one greater
 * than the desired value, but it should be programmed to the desired
 * value.
 *
 * Hydra_Software_Devel/244   5/28/09 2:18p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/243   5/22/09 11:33a tdo
 * PR55364: Disable dithering when any test feature is enabled
 *
 * Hydra_Software_Devel/242   4/30/09 1:40p pntruong
 * PR53960: Removed coverity warnings of unused local var.
 *
 * Hydra_Software_Devel/241   4/17/09 6:58p pntruong
 * PR53960: Inform HDDVI of HDMI Pixel Decimation and HdmiMode via the
 * HDDVI picture struct.
 *
 * Hydra_Software_Devel/240   4/17/09 4:32p yuxiaz
 * PR54135: When change window size from full screen, soucepending
 * callback won't fire.
 *
 * Hydra_Software_Devel/239   4/17/09 11:57a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/238   4/10/09 6:33p hongtaoz
 * PR54064: source pending callback must fire after window shutdown;
 *
 * Hydra_Software_Devel/237   4/3/09 4:33p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/236   4/1/09 12:04p pntruong
 * PR53756: No video for 1360x765, 1064x598.5, and 848x477 HDMI PC format.
 *
 * Hydra_Software_Devel/235   3/27/09 6:09p pntruong
 * PR53550: More PC format problems.  Relaxed condition for detecting
 * format not to use horizontal blanking.
 *
 * Hydra_Software_Devel/234   3/25/09 5:54p pntruong
 * PR52524: Used the index determined by upstream block.
 *
 * Hydra_Software_Devel/233   3/18/09 11:03p pntruong
 * PR53408: [BBTV]"No signal" in 1920x1080@60i with IPTV settop Box HDMI
 * input.  Restricted decimation to sd formats.
 *
 * Hydra_Software_Devel/232   3/9/09 1:28p pntruong
 * PR52524: Added support for hdmi rgb quantization of full range and
 * limited range.
 *
 * Hydra_Software_Devel/231   3/4/09 7:03p pntruong
 * PR52482: Vnet cannot be rebuilt when CVBS video in freezing then
 * unplug/plug the input cable.
 *
 * Hydra_Software_Devel/230   3/4/09 11:19a pntruong
 * PR52568: Customer not needed the custom 1340x725p@60hz.
 *
 * Hydra_Software_Devel/229   3/2/09 3:46p yuxiaz
 * PR52287: More update on 422-444 and 444-422 converters.
 *
 * Hydra_Software_Devel/228   3/2/09 3:05p pntruong
 * PR52568: Added support for custom format 1340x725p@60hz.
 *
 * Hydra_Software_Devel/227   2/26/09 10:01p pntruong
 * PR52568: [GDVR] Invalid format at HDMI 1080i.  Added support for detect
 * famous devices.
 *
 * Hydra_Software_Devel/226   2/26/09 6:10p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/225   2/7/09 4:44p pntruong
 * PR51810: Used the same round off as the one in multibuffering.
 *
 * Hydra_Software_Devel/224   2/4/09 10:04p pntruong
 * PR51810: HDDVI: Input from unstable source that produce 61hz result in
 * additional buffer.
 *
 * Hydra_Software_Devel/223   1/21/09 7:43p pntruong
 * PR51418: Fixed HDMI AvMute Processing.
 *
 * Hydra_Software_Devel/222   1/19/09 6:45p pntruong
 * PR51304: Improved format detection for formats with pixel decimation.
 *
 * Hydra_Software_Devel/221   12/18/08 8:45p pntruong
 * PR50502: Used correct display/source rate for computing the number of
 * buffers.
 *
 * Hydra_Software_Devel/220   12/3/08 7:54p pntruong
 * PR45817: Added hddvi support.
 *
 * Hydra_Software_Devel/219   11/6/08 7:07p pntruong
 * PR48680: Allowed adjusting of csc in ycrcb mode.
 *
 * Hydra_Software_Devel/218   10/21/08 1:03p pntruong
 * PR47156, PR47970, PR47658: Fixed typo.
 *
 * Hydra_Software_Devel/217   10/20/08 10:16p pntruong
 * PR47156, PR47970, PR47658: Bypassed scanwidth check for interlaced
 * formats.
 *
 * Hydra_Software_Devel/216   10/3/08 12:20p syang
 * pr 47129, pr 45402 :  1). improve  the VDC internally used videoFmtCode
 * approximate; 2). return w, h, frmRate and bInterlaced with src-pending
 * callback for non-enumerated mpeg-src video format
 *
 * Hydra_Software_Devel/215   9/29/08 7:59p pntruong
 * PR47156: Added more debug messages for unknown formats.
 *
 * Hydra_Software_Devel/214   9/23/08 7:56p pntruong
 * PR47149: Fixed warnings.
 *
 * Hydra_Software_Devel/213   9/22/08 1:35p pntruong
 * PR47126, PR46798: Make sure to build muxes on input color space change.
 * Use correct mask color for rgb/ycrcb.
 *
 * Hydra_Software_Devel/212   9/20/08 12:49p pntruong
 * PR47149: Adapted to invalid format info returns from bfmt.
 *
 * Hydra_Software_Devel/211   9/20/08 10:42a pntruong
 * PR47156: Improved detections of reduce blanking formats.
 *
 * Hydra_Software_Devel/210   9/15/08 11:52p pntruong
 * PR46798: Video judder when setting source color matrix for HDMI.
 *
 * Hydra_Software_Devel/209   9/2/08 6:05p pntruong
 * PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
 * alloc debug messages.
 *
 * Hydra_Software_Devel/208   8/8/08 3:28p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/207   8/6/08 3:00p yuxiaz
 * PR45367: Enable Dering in VDC.
 *
 * Hydra_Software_Devel/206   8/5/08 9:22p pntruong
 * PR44362, PR44807: Better tuned bvb reset states.
 *
 * Hydra_Software_Devel/205   6/26/08 9:57p pntruong
 * PR44238: Turn off fast pad as it currently affect the format detection.
 *
 * Hydra_Software_Devel/204   6/25/08 9:08p pntruong
 * PR39163: Added support xvYcc transfer characteristic from hddvi source.
 *
 * Hydra_Software_Devel/203   6/18/08 3:05p syang
 * PR35549: 1). src-pending when new win is created; 2). don't do
 * BufferHeap_AllocateBuffers as bReConfigSrc
 *
 * Hydra_Software_Devel/202   5/28/08 3:35p pntruong
 * PR42072: Make sure all windows get reconfigure on startfeed change.
 *
 * Hydra_Software_Devel/201   5/8/08 5:53p pntruong
 * PR35549: Added more preparations for source pending.
 *
 * Hydra_Software_Devel/200   5/7/08 11:28a pntruong
 * PR35549: Hook up the source callback event first so that app can start
 * using it while adding the internal pending.
 *
 * Hydra_Software_Devel/199   4/29/08 7:48p pntruong
 * PR35549: Make room for source pending dirty bit.
 *
 * Hydra_Software_Devel/198   4/2/08 11:21a pntruong
 * PR40742: No input status change also trigger generic callback.
 *
 * Hydra_Software_Devel/197   3/27/08 9:46a yuxiaz
 * PR40974: Need to check if the Hstart and Vstart setting from user is
 * valid or not  when format changes. Use the default delay of the format
 * from the table when user setting is out of range.
 *
 * Hydra_Software_Devel/196   3/26/08 1:14p hongtaoz
 * PR35398, PR40895: added ulVsyncRate and removed bGameModeDelayLags in
 * the window lipsync callback structure; added BFMT_FREQ_FACTOR to unify
 * Vsync rate and pixel frequency calculation;
 *
 * Hydra_Software_Devel/195   3/25/08 3:17p syang
 * PR 40431: add complete assert for critical section protection among src
 * _isr, dsp _isr, and ApplyChanges
 *
 * Hydra_Software_Devel/194   3/19/08 9:51a pntruong
 * PR40371: Fixed mux settings for Ycbcr422 10bit and 12bit.
 *
 * Hydra_Software_Devel/193   3/14/08 3:52p pntruong
 * PR40456: Preserved bits that are set by other core.
 *
 * Hydra_Software_Devel/192   3/12/08 5:37p pntruong
 * PR40455: [HD_DVI] Need to bring up hd_dvi_0 first to make hd_dvi_1 work
 * for external HD_DVI input.
 *
 * Hydra_Software_Devel/191   2/29/08 4:18p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/190   2/22/08 2:32p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/189   2/21/08 5:00p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/188   2/14/08 6:06p pntruong
 * PR39321: Implement hardware changes for HD_DVI.
 *
 * Hydra_Software_Devel/187   2/4/08 12:55p pntruong
 * PR39294: Memory not enough when switch from hdmi720p 60hz to hdmi 1080p
 * 50hz.
 *
 * Hydra_Software_Devel/186   1/17/08 11:39a yuxiaz
 * PR38528: Reset BVB bridge when video detection status changes.
 *
 * Hydra_Software_Devel/185   1/14/08 2:36p yuxiaz
 * PR38528: Read PCTR error bits in RUL.
 *
 * Hydra_Software_Devel/184   11/9/07 2:54p pntruong
 * PR36786: Also return the original format prior to remapped.
 *
 * Hydra_Software_Devel/183   10/30/07 7:19p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/182   10/26/07 5:46p pntruong
 * PR36469: Relax tolerance to pickup 480p@60Hz from Master Pattern
 * Generator, as other leading could also detect and display it.
 *
 * Hydra_Software_Devel/181   10/11/07 2:08p albertl
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/180   10/2/07 11:24a pntruong
 * PR35136: Some cleanup of unused code.
 *
 * Hydra_Software_Devel/179   9/20/07 3:29p pntruong
 * PR35135: Fixed build errors from csc refactoring.
 *
 * Hydra_Software_Devel/178   9/17/07 5:46p pntruong
 * PR34817: SD format(480X480) decoding problem.  Updated hddvi code to
 * use the h/v size from hdr/xvd.
 *
 * Hydra_Software_Devel/177   9/5/07 3:47p pntruong
 * PR34632: [HD_DVI] Incorrect reset bit got written.  Also reduce the
 * reset to one only, there is no need to reset 3.
 *
 * Hydra_Software_Devel/176   9/4/07 3:13p pntruong
 * PR34566: Delay framerate detection until video is locked.
 *
 * Hydra_Software_Devel/175   8/20/07 11:25a pntruong
 * PR33847: Apply workaround for all interlace formats.
 *
 * Hydra_Software_Devel/174   8/17/07 10:00a pntruong
 * PR34059: Need to fix FMT name for 1024x768i_43Hz.
 *
 * Hydra_Software_Devel/PR34059/1   8/17/07 11:41a gorgon
 * PR34059: Need to fix FMT name for 1024x768i_43Hz
 *
 * Hydra_Software_Devel/173   8/15/07 10:30a pntruong
 * PR33847:  Use top and bottom vertical blank to detect Marantz DVD
 * player field inversion issue.
 *
 * Hydra_Software_Devel/PR33847/1   8/15/07 9:18p honglang
 * PR33847:  Use top and bottom vertical blank to detect Marantz DVD
 * player field inversion issue.
 *
 * Hydra_Software_Devel/172   8/14/07 4:50p pntruong
 * PR31680: [TTA ,HDMI, C0] HDMI no video when set YCbCr422 Color space
 * after power on.
 *
 * Hydra_Software_Devel/171   7/27/07 4:16p yuxiaz
 * PR29940: Use bInterlaced from format.
 *
 * Hydra_Software_Devel/170   7/26/07 9:10a yuxiaz
 * PR29940: VER_DELAY2 should be VER_DELAY1 + 1 for interlaced input..
 *
 * Hydra_Software_Devel/169   7/25/07 8:49p pntruong
 * PR33411: Frequency needs to be detect as it is also a parameter to be
 * use for format identifications.
 *
 * Hydra_Software_Devel/168   7/25/07 7:56p pntruong
 * PR33411: Source frequence change when change vnet mode with ANR and DNR
 * enable.
 *
 * Hydra_Software_Devel/167   7/25/07 12:14p yuxiaz
 * PR32483: Flicker is appeared on the screen after the Samsung BD-P1000
 * player is turned to stand-by mode in HDMI input.
 *
 * Hydra_Software_Devel/166   7/9/07 2:22p pntruong
 * PR31628: HDDVI get real aspect ratio from HDR instead of FMT table,
 * because HDR can get the real value from AVI info frame.
 *
 * Hydra_Software_Devel/165   7/6/07 1:59p yuxiaz
 * PR32495: Only set YCbCr 30bit 422 double clock mode for external
 * HD_DVI.
 *
 * Hydra_Software_Devel/164   6/26/07 5:36p pntruong
 * PR32402: TV display "unsupport format" and "no signal" OSD for
 * componet/hdmi input.
 *
 * Hydra_Software_Devel/163   6/26/07 3:48p yuxiaz
 * PR32495: Added YCbCr 30bit 422 double clock mode for HD_DVI.
 *
 * Hydra_Software_Devel/162   6/26/07 2:39p pntruong
 * PR32241: modify (BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VSYNC_CNT_FIELD0,
 * VSYNC_CNT) != 0xffffff) to 0xfffffff( 7 F)
 * PR32241: use eColorSpace to distinguish CSC instead of colorimetry
 *
 * Hydra_Software_Devel/161   6/21/07 3:38p syang
 * PR 32440: clean up inefficient "if" statement
 *
 * Hydra_Software_Devel/160   6/21/07 2:16p yuxiaz
 * PR32023: Added video format tolerance to HDDVI.
 *
 * Hydra_Software_Devel/159   6/19/07 1:53p yuxiaz
 * PR32157: 480p HDMI input can't be locked with turning on ANR
 *
 * Hydra_Software_Devel/158   6/15/07 1:51p yuxiaz
 * PR29940: Added tolerance when trying to lock HD_DVI format in non-auto
 * format mode.
 *
 * Hydra_Software_Devel/157   6/8/07 12:37p pntruong
 * PR29940: Updated internal DE config table. Double HOR_PIXELS when
 * PIXEL_DECIMATION is n.
 *
 * Hydra_Software_Devel/156   6/7/07 2:03p pntruong
 * PR31813: Temporary put back capture reset.  Place trigger relative to
 * percentage of vbi area instead of absolute line numbers.
 *
 * Hydra_Software_Devel/155   6/5/07 9:52a yuxiaz
 * PR29940: Fxied BVDC_P_HdDvi_ValidateChanges when front porch and back
 * porch are zero with no signal.
 *
 * Hydra_Software_Devel/154   5/29/07 4:59p yuxiaz
 * PR30334, PR31640: Merge in changes from branch. Use pCurInfo-
 * >ulInputPort to configure HDDVI CHANNEL_CNTRL register MODESEL bit to
 * support extern daught card. Format detection sometimes failed because
 * of wrong pixel decimate value
 *
 * Hydra_Software_Devel/153   5/23/07 1:27p yuxiaz
 * PR29980: Added HD_DVI mux settings for different input color space.
 *
 * Hydra_Software_Devel/152   5/22/07 5:24p yuxiaz
 * PR29980: new mux settings for HD_DVI
 *
 * Hydra_Software_Devel/151   5/21/07 2:42p yuxiaz
 * PR29940: Added more formats in internal DE config table.
 *
 * Hydra_Software_Devel/150   5/21/07 1:33p pntruong
 * PR29980: C0: Handle new MUX settings for HD_DVI.
 *
 * Hydra_Software_Devel/149   5/18/07 2:51p yuxiaz
 * PR28810: add format 832x624 support. Merge from PR28810 branch.
 *
 * Hydra_Software_Devel/148   5/16/07 10:29a yuxiaz
 * PR29940: Updated internal DE config table. Double HOR_PIXELS when
 * PIXEL_DECIMATION is 1.
 *
 * Hydra_Software_Devel/147   5/15/07 11:46a yuxiaz
 * PR29940: Add support to set and get Hstart and Vstart for HDDVI.
 *
 * Hydra_Software_Devel/146   5/10/07 3:22p syang
 * PR 30617: add bStartFeed to src for muting writer
 *
 * Hydra_Software_Devel/145   4/23/07 5:33p pntruong
 * PR29980: C0: Handle new MUX settings for HD_DVI.
 *
 * Hydra_Software_Devel/144   4/17/07 4:34p pntruong
 * PR27209: Improved trigger swapped work-around.
 *
 * Hydra_Software_Devel/143   4/17/07 11:19a pntruong
 * PR28679, PR29204: HD_DVI: Video format detection is unstable.
 *
 * Hydra_Software_Devel/142   4/13/07 10:40a yuxiaz
 * PR28350: Added bVsyncDetected in BVDC_Source_InputStatus.
 *
 * Hydra_Software_Devel/141   4/9/07 11:25a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/140   4/5/07 11:49a yuxiaz
 * PR 28391: Updated DE_GEN_CONFIG1.VER_LINES settings.
 *
 * Hydra_Software_Devel/139   4/4/07 8:32p pntruong
 * PR27913: Fixed chroma rollover in BVN, only for 3563Ax, Bx chips.
 *
 * Hydra_Software_Devel/138   4/4/07 4:44p yuxiaz
 * PR 28391: Added DE settings for more formats.
 *
 * Hydra_Software_Devel/137   4/3/07 2:03p yuxiaz
 * PR 28391: Both HD_DVI core are combined together for external HD_DVI 24
 * bit and 30 bit mode.
 *
 * Hydra_Software_Devel/136   3/30/07 10:40a yuxiaz
 * PR28350, PR27180: Added BVDC_Source_GetInputStatus to get input status
 * for VDEC and HD_DVI.
 *
 * Hydra_Software_Devel/135   3/28/07 11:14p pntruong
 * PR28395: Provide API to customize VDEC video quality.  Cleanup dirty
 * meanings.
 *
 * Hydra_Software_Devel/134   3/28/07 8:07p pntruong
 * PR28391: Conserved source dirty as it about running out.
 *
 * Hydra_Software_Devel/133   3/28/07 6:35p yuxiaz
 * PR 28391: Support connecting external ADC to HD_DVI
 *
 * Hydra_Software_Devel/132   3/27/07 8:27p pntruong
 * PR28761: Rebuild colorspace upon callback installation to reflect new
 * colorspace.  Back-to-back interupt service is causing the isr to
 * swapped out the un-executed rul.
 *
 * Hydra_Software_Devel/131   3/23/07 11:19a tdo
 * PR 28392: Provide API to customize source CSC
 *
 * Hydra_Software_Devel/130   3/22/07 10:14p pntruong
 * PR28761: RGB CSC is abnormal after board powerup and input RGB first.
 * Used colorspace from callback if it is available.
 *
 * Hydra_Software_Devel/129   3/12/07 1:29p yuxiaz
 * PR 28590: Enable / disable HD_DVI format detection in VDC.
 *
 * Hydra_Software_Devel/128   3/7/07 1:23p pntruong
 * PR28478: Display abnormally while input YCbCr444 video at the first.
 *
 * Hydra_Software_Devel/127   2/17/07 9:36a pntruong
 * PR23482: Fine tuned framerate detection, handle case for pc input.
 *
 * Hydra_Software_Devel/126   2/15/07 4:04p pntruong
 * PR25235: Need to use actual pixel frequency and vertical frequency
 * fields in BFMT instead of the MASK.
 *
 * Hydra_Software_Devel/125   2/8/07 3:38p yuxiaz
 * PR 27209: Check HD_DVI_x_VID_FORMAT_UPDATE_STATUS for stable input.
 *
 * Hydra_Software_Devel/124   2/7/07 5:33p yuxiaz
 * PR 27209: Added HD_DVI trigger mismatch workaround for 3563.
 *
 * Hydra_Software_Devel/123   2/5/07 4:46p pntruong
 * PR26677, PR23236:  Restore some settings from previous hddvi code, to
 * use picture clean up to discard unused pixels.  Also enabled shutoff
 * end of picture.
 *
 * Hydra_Software_Devel/122   2/3/07 12:19p pntruong
 * PR23674: Updated the current ycrcb and colorsparce from callback.
 *
 * Hydra_Software_Devel/121   2/2/07 7:37p pntruong
 * PR23482: Added VDEC/HDDVI Frame Rate Detection in VDC.  Also refactored
 * detection to use in both hddvi and vdec.  Added support for all
 * framerate code suported in bavc.h.
 *
 * Hydra_Software_Devel/120   2/1/07 1:53p pntruong
 * PR26677, PR23236: Enhanced autoformat detection, added 1080p support.
 *
 * Hydra_Software_Devel/119   1/31/07 8:22p pntruong
 * PR23674: Support CSC auto detection in HDMI.
 *
 * Hydra_Software_Devel/118   1/23/07 5:35p yuxiaz
 * PR 27209: Reset HD_DVI when new format is detected.
 *
 * Hydra_Software_Devel/117   1/23/07 4:07p pntruong
 * PR23225: VDEC - PC input.  Updated values for FD detect for PC mode.
 *
 * Hydra_Software_Devel/116   1/22/07 7:51p pntruong
 * PR22579: Bringup of HD_DVI input (dual core).  Fixed colorspace.
 *
 * Hydra_Software_Devel/115   1/22/07 11:12a pntruong
 * PR22579: Bringup of HD_DVI input (dual core).  Redo hddvi code.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"
#include "bvdc_priv.h"
#include "bvdc_hddvi_priv.h"


/***************************************************************************/
/* Has some support for at least one HD_DVI? */
#if (BVDC_P_SUPPORT_HDDVI)
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bchp_hd_dvi_0.h"

#if (BVDC_P_SUPPORT_HDDVI > 1)
#include "bchp_hd_dvi_1.h"
#endif

BDBG_MODULE(BVDC_HDDVI);
BDBG_OBJECT_ID(BVDC_DVI);

/* Dither settings for HD_DVI */
#define BVDC_P_DITHER_HDDVI_LFSR_VALUE                 (0xC18F9)
#define BVDC_P_DITHER_HDDVI0_LFSR_CTRL_T0              (0x4)
#define BVDC_P_DITHER_HDDVI0_LFSR_CTRL_T1              (0x6)
#define BVDC_P_DITHER_HDDVI0_LFSR_CTRL_T2              (0x5)
#define BVDC_P_DITHER_HDDVI1_LFSR_CTRL_T0              (0x0)
#define BVDC_P_DITHER_HDDVI1_LFSR_CTRL_T1              (0x6)
#define BVDC_P_DITHER_HDDVI1_LFSR_CTRL_T2              (0x5)

/* Testing the new format */
#define BVDC_P_HDDVI_TEST_NEW_FORMAT                   (0)

/* External input type */
#define BVDC_P_HDDVI_EXT_INPUT_TYPE_NONE               (0)
#define BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK           (1)
#define BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP                (2)

/* External HDDVI configuration */
#if (BCHP_CHIP == 7420)
#define BVDC_P_HDDVI_EXT_INPUT_TYPE     (BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
#elif (BCHP_CHIP == 7231)
#define BVDC_P_HDDVI_EXT_INPUT_TYPE     (BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
#else
#define BVDC_P_HDDVI_EXT_INPUT_TYPE     (BVDC_P_HDDVI_EXT_INPUT_TYPE_NONE)
#endif

/****************************************************************************/
/* Number of entries use this to loop                                       */
/****************************************************************************/
#define BVDC_P_DE_TABLE_COUNT \
	(sizeof(s_aDeCfgTbl)/sizeof(BVDC_P_HdDvi_DeConfig))

/****************************************************************************/
/* Making an entry for internal DE configurations                           */
/****************************************************************************/
#define BVDC_P_HDDVI_MAKE_DE(fmt, ulHorzDelay, ulVertDelay)                 \
{                                                                           \
	(fmt),                                                                  \
	(ulHorzDelay),                                                          \
	(ulVertDelay)                                                           \
}

/* Internal De config table. */
static const BVDC_P_HdDvi_DeConfig s_aDeCfgTbl[] =
{
	/*                   fmt                      ulHorzDelay  ulVertDelay */
	/* Video format */
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e240p_60Hz,           122,         12),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eNTSC,                122,         12),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e720x482_NTSC,        122,         12),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_ePAL_G,               132,         12),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e480p,                122,         22),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e720x483p,            122,         22),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e576p_50Hz,           132,         23),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e720p,                300,         25),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e720p_60Hz_3DOU_AS,   300,         25),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e720p_50Hz_3DOU_AS,   260,         25),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e720p_50Hz,           300,         25),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080i,               235,         21),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080i_50Hz,          235,         21),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080p,               235,         41),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080p_50Hz,          235,         41),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080p_30Hz,          235,         41),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080p_25Hz,          235,         41),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080p_24Hz,          235,         41),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080p_24Hz_3DOU_AS,  192,         41),
	/* todo: Need to adjust when input format is available */
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e1080p_30Hz_3DOU_AS,  192,         41),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_e288p_50Hz,           132,         14),

	/* PC format */
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_640x350p_85Hz,   160,         63),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_640x400p_85Hz,   160,         44),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_640x480p,        140,         35),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_640x480p_72Hz,   168,         31),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_640x480p_75Hz,   184,         20),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_640x480p_85Hz,   150,         30),

	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_720x400p_60Hz,   180,         45),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_720x400p_70Hz,   180,         45),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_720x400p_85Hz,   180,         45),

	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_800x600p,        216,         30),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_800x600p_56Hz,   200,         25),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_800x600p_72Hz,   184,         30),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_800x600p_75Hz,   240,         25),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_800x600p_85Hz,   220,         30),

	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1024x768p,       296,         35),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1024x768i_87Hz,  232,         20),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1024x768p_70Hz,  280,         35),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1024x768p_75Hz,  280,         30),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1024x768p_85Hz,  314,         40),

	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1152x864p_75Hz,  384,         35),

	/* (1280x768/60-R  68.25MHz) or (1280x768/60 79.5Mhz) */
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x768p,       112,         20),
	/* BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x768p,    320,         30), */
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x768p_75Hz,  336,         35),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x768p_85Hz,  352,         40),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x960p_60Hz,  424,         40),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x960p_85Hz,  384,         50),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x1024p_60Hz, 360,         40),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x1024p_75Hz, 392,         40),
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1280x1024p_85Hz, 384,         50),

	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_832x624p_75Hz,   392,         40),

	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1360x768p_60Hz,  368,         25),

	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1400x1050p_60Hz, 110,         25),
	/* BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1400x1050p_60Hz, 376,      35), */
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1400x1050p_75Hz, 392,         45),

	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eDVI_1600x1200p_60Hz, 500,         50),

	/* Last */
	BVDC_P_HDDVI_MAKE_DE(BFMT_VideoFmt_eMaxCount,            0,           0 )
};

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_3)
#define BVDC_P_HDDVI_MUX_COUNT                  (36)

static const uint8_t s_aaucMux[][BVDC_P_HDDVI_MUX_COUNT] =
{
	/* Mux table for R12:G12:B12: input */
	{
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
		62, 62,  0,  1,          /* MUX_DATA_0  - MUX_DATA_3  */
		 2,  3,  4,  5,          /* MUX_DATA_4  - MUX_DATA_7  */
		 6,  7,  8,  9,          /* MUX_DATA_8  - MUX_DATA_11  */
		62, 62, 10, 11,          /* MUX_DATA_12 - MUX_DATA_15  */
		12, 13, 14, 15,          /* MUX_DATA_16 - MUX_DATA_19  */
		16, 17, 18, 19,          /* MUX_DATA_20 - MUX_DATA_23  */
		62, 62, 20, 21,          /* MUX_DATA_24 - MUX_DATA_27  */
		22, 23, 24, 25,          /* MUX_DATA_28 - MUX_DATA_31  */
		26, 27, 28, 29           /* MUX_DATA_32 - MUX_DATA_35  */
#else
		 0,  1,  2,  3,          /* MUX_DATA_0  - MUX_DATA_3  */
		 4,  5,  6,  7,          /* MUX_DATA_4  - MUX_DATA_7  */
		 8,  9, 10, 11,          /* MUX_DATA_8  - MUX_DATA_11  */
		12, 13, 14, 15,          /* MUX_DATA_12 - MUX_DATA_15  */
		16, 17, 18, 19,          /* MUX_DATA_16 - MUX_DATA_19  */
		20, 21, 22, 23,          /* MUX_DATA_20 - MUX_DATA_23  */
		24, 25, 26, 27,          /* MUX_DATA_24 - MUX_DATA_27  */
		28, 29, 30, 31,          /* MUX_DATA_28 - MUX_DATA_31  */
		32, 33, 34, 35           /* MUX_DATA_32 - MUX_DATA_35  */
#endif
	},
	/* Mux table for Y12:Cb12:Cr12:444 input */
	{
		 0,  1,  2,  3,          /* MUX_DATA_0  - MUX_DATA_3  */
		 4,  5,  6,  7,          /* MUX_DATA_4  - MUX_DATA_7  */
		 8,  9, 10, 11,          /* MUX_DATA_8  - MUX_DATA_11  */
		12, 13, 14, 15,          /* MUX_DATA_12 - MUX_DATA_15  */
		16, 17, 18, 19,          /* MUX_DATA_16 - MUX_DATA_19  */
		20, 21, 22, 23,          /* MUX_DATA_20 - MUX_DATA_23  */
		24, 25, 26, 27,          /* MUX_DATA_24 - MUX_DATA_27  */
		28, 29, 30, 31,          /* MUX_DATA_28 - MUX_DATA_31  */
		32, 33, 34, 35           /* MUX_DATA_32 - MUX_DATA_35  */
	},
	/* Mux table for Y8:Cb8:Cr8:422 input */
	{
		 8,  9, 10, 11,          /* MUX_DATA_0  - MUX_DATA_3  */
		28, 29, 30, 31,          /* MUX_DATA_4  - MUX_DATA_7  */
		32, 33, 34, 35,          /* MUX_DATA_8  - MUX_DATA_11  */
		 4,  5,  6,  7,          /* MUX_DATA_12 - MUX_DATA_15  */
		16, 17, 18, 19,          /* MUX_DATA_16 - MUX_DATA_19  */
		20, 21, 22, 23,          /* MUX_DATA_20 - MUX_DATA_23  */
		62, 62, 62, 62,          /* MUX_DATA_24 - MUX_DATA_27  */
		62, 62, 62, 62,          /* MUX_DATA_28 - MUX_DATA_31  */
		62, 62, 62, 62           /* MUX_DATA_32 - MUX_DATA_35  */
	},
	/* Mux table for YCbCr422 double clock input */
	{
		62, 62, 62, 62,          /* MUX_DATA_0  - MUX_DATA_3  */
		 7,  8,  9, 10,          /* MUX_DATA_4  - MUX_DATA_7  */
		11, 12, 13, 14,          /* MUX_DATA_8  - MUX_DATA_11  */
		62, 62, 62, 62,          /* MUX_DATA_12 - MUX_DATA_15  */
		22, 23, 24, 25,          /* MUX_DATA_16 - MUX_DATA_19  */
		26, 27, 28, 29,          /* MUX_DATA_20 - MUX_DATA_23  */
		62, 62, 62, 62,          /* MUX_DATA_24 - MUX_DATA_27  */
		62, 62, 62, 62,          /* MUX_DATA_28 - MUX_DATA_31  */
		62, 62, 62, 62           /* MUX_DATA_32 - MUX_DATA_35  */
	},
	/* Mux table for 7420 DVO->HDDVI loopback */
/* MUX: Mapping
MUX_DATA[index]
[35][34][33][32][31][30][29][28][27][26][25][24]-[23][22][21][20][19][18][17][16][15][14][13][12]-[11][10][09][08][07][06][05][04][03][02][01][00]

INPUT_DATA[]
[xx][xx][xx][xx][xx][xx][xx][xx][xx][23][22][21]-[20][19][18][17][16][15][14][13][12][xx][xx][xx]-[11][10][09][08][07][06][05][04][03][02][01][00]

OUTPUT_DATA[]
[23][22][21][20][19][18][17][16][xx][xx][xx][xx]-[15][14][13][12][11][10][09][08][xx][xx][xx][xx]-[07][06][05][04][03][02][01][00][xx][xx][xx][xx]

MUX_DATA[]
[26][25][24][23][22][21][20][19][xx][xx][xx][xx]-[18][17][16][15][11][10][09][08][xx][xx][xx][xx]-[07][06][05][04][03][02][01][00][xx][xx][xx][xx]
*/
	{
		62, 62, 62, 62,          /* MUX_DATA_0  - MUX_DATA_3  */
		 0,  1,  2,  3,          /* MUX_DATA_4  - MUX_DATA_7  */
		 4,  5,  6,  7,          /* MUX_DATA_8  - MUX_DATA_11  */
		62, 62, 62, 62,          /* MUX_DATA_12 - MUX_DATA_15  */
		 8,  9, 10, 11,          /* MUX_DATA_16 - MUX_DATA_19  */
		15, 16, 17, 18,          /* MUX_DATA_20 - MUX_DATA_23  */
		62, 62, 62, 62,          /* MUX_DATA_24 - MUX_DATA_27  */
		19, 20, 21, 22,          /* MUX_DATA_28 - MUX_DATA_31  */
		23, 24, 25, 26           /* MUX_DATA_32 - MUX_DATA_35  */
	}

};
#endif

/* Table for Input timing and Deskew Clk */
#define BVDC_P_HDDVI_INPUT_TIMING_ADJUST_COUNT  (39)
#define BVDC_P_HDDVI_DESKEW_CLK_COUNT           (5)

static const uint8_t s_aucInputTimingMsw[BVDC_P_HDDVI_INPUT_TIMING_ADJUST_COUNT] =
{
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
	0, 0, 2, 4,                 /* DATA_MSW_0  - DATA_MSW_3     */
	0, 2, 2, 2,                 /* DATA_MSW_4  - DATA_MSW_7     */
	3, 0, 1, 0,                 /* DATA_MSW_8  - DATA_MSW_11    */
	0, 0, 0, 0,                 /* DATA_MSW_12 - DATA_MSW_15    */
	8, 8, 8, 8,                 /* DATA_MSW_16 - DATA_MSW_19    */
	8, 8, 8, 8,                 /* DATA_MSW_20 - DATA_MSW_23    */
	8, 8, 8, 8,                 /* DATA_MSW_24 - DATA_MSW_27    */
	8, 8, 8, 8,                 /* DATA_MSW_28 - DATA_MSW_31    */
	8, 8, 8, 8,                 /* DATA_MSW_32 - DATA_MSW_35    */
	0, 1, 1                     /* HSYNC_MSW, VSYNC_MSW, DE_MSW */
#elif (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
	8, 7, 6, 8,                 /* DATA_MSW_0  - DATA_MSW_3     */
	6, 8, 8, 7,                 /* DATA_MSW_4  - DATA_MSW_7     */
	7, 7, 8, 8,                 /* DATA_MSW_8  - DATA_MSW_11    */
	8, 6, 6, 8,                 /* DATA_MSW_12 - DATA_MSW_15    */
	7, 8, 8, 9,                 /* DATA_MSW_16 - DATA_MSW_19    */
	8, 9, 8, 8,                 /* DATA_MSW_20 - DATA_MSW_23    */
	8, 8, 8, 8,                 /* DATA_MSW_24 - DATA_MSW_27    */
	8, 8, 8, 8,                 /* DATA_MSW_28 - DATA_MSW_31    */
	8, 8, 8, 8,                 /* DATA_MSW_32 - DATA_MSW_35    */
	8, 8, 8                     /* HSYNC_MSW, VSYNC_MSW, DE_MSW */
#else
	8, 8, 8, 8,                 /* DATA_MSW_0  - DATA_MSW_3     */
	8, 8, 8, 8,                 /* DATA_MSW_4  - DATA_MSW_7     */
	8, 8, 8, 8,                 /* DATA_MSW_8  - DATA_MSW_11    */
	8, 8, 8, 8,                 /* DATA_MSW_12 - DATA_MSW_15    */
	8, 8, 8, 8,                 /* DATA_MSW_16 - DATA_MSW_19    */
	8, 8, 8, 8,                 /* DATA_MSW_20 - DATA_MSW_23    */
	8, 8, 8, 8,                 /* DATA_MSW_24 - DATA_MSW_27    */
	8, 8, 8, 8,                 /* DATA_MSW_28 - DATA_MSW_31    */
	8, 8, 8, 8,                 /* DATA_MSW_32 - DATA_MSW_35    */
	8, 8, 8                     /* HSYNC_MSW, VSYNC_MSW, DE_MSW */
#endif
};

static const uint8_t s_aucInputTimingLsw[BVDC_P_HDDVI_INPUT_TIMING_ADJUST_COUNT] =
{
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
	1, 0, 2, 4,                 /* DATA_LSW_0  - DATA_LSW_3     */
	0, 3, 2, 3,                 /* DATA_LSW_4  - DATA_LSW_7     */
	3, 0, 2, 0,                 /* DATA_LSW_8  - DATA_LSW_11    */
	0, 0, 0, 0,                 /* DATA_LSW_12 - DATA_LSW_15    */
	8, 8, 8, 8,                 /* DATA_LSW_16 - DATA_LSW_19    */
	8, 8, 8, 8,                 /* DATA_LSW_20 - DATA_LSW_23    */
	8, 8, 8, 8,                 /* DATA_LSW_24 - DATA_LSW_27    */
	8, 8, 8, 8,                 /* DATA_LSW_28 - DATA_LSW_31    */
	8, 8, 8, 8,                 /* DATA_LSW_32 - DATA_LSW_35    */
	0, 1, 2                     /* HSYNC_LSW, VSYNC_LSW, DE_LSW */
#elif (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
	5, 2, 0, 4,                 /* DATA_LSW_0  - DATA_LSW_3     */
	4, 2, 4, 2,                 /* DATA_LSW_4  - DATA_LSW_7     */
	2, 4, 6, 5,                 /* DATA_LSW_8  - DATA_LSW_11    */
	4, 0, 0, 4,                 /* DATA_LSW_12 - DATA_LSW_15    */
	2, 4, 4, 5,                 /* DATA_LSW_16 - DATA_LSW_19    */
	5, 6, 6, 5,                 /* DATA_LSW_20 - DATA_LSW_23    */
	5, 6, 5, 4,                 /* DATA_LSW_24 - DATA_LSW_27    */
	5, 5, 8, 8,                 /* DATA_LSW_28 - DATA_LSW_31    */
	8, 8, 8, 8,                 /* DATA_LSW_32 - DATA_LSW_35    */
	5, 5, 5                     /* HSYNC_LSW, VSYNC_LSW, DE_LSW */
#else
	8, 8, 8, 8,                 /* DATA_LSW_0  - DATA_LSW_3     */
	8, 8, 8, 8,                 /* DATA_LSW_4  - DATA_LSW_7     */
	8, 8, 8, 8,                 /* DATA_LSW_8  - DATA_LSW_11    */
	8, 8, 8, 8,                 /* DATA_LSW_12 - DATA_LSW_15    */
	8, 8, 8, 8,                 /* DATA_LSW_16 - DATA_LSW_19    */
	8, 8, 8, 8,                 /* DATA_LSW_20 - DATA_LSW_23    */
	8, 8, 8, 8,                 /* DATA_LSW_24 - DATA_LSW_27    */
	8, 8, 8, 8,                 /* DATA_LSW_28 - DATA_LSW_31    */
	8, 8, 8, 8,                 /* DATA_LSW_32 - DATA_LSW_35    */
	8, 8, 8                     /* HSYNC_LSW, VSYNC_LSW, DE_LSW */
#endif
};

static const uint8_t s_aucDeskewClkMsw[BVDC_P_HDDVI_DESKEW_CLK_COUNT] =
{
#if ((BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK) || \
     (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP))
	 0,                 /* DELAY_ONE_CTL_CLK_MSW   */
	 0,                 /* DELAY_TWO_CTL_CLK_MSW   */
	 0,                 /* DELAY_THREE_CTL_CLK_MSW */
	 0,                 /* DELAY_FOUR_CTL_CLK_MSW  */
	 0                  /* DELAY_SELECTION_CLK_MSW */
#else
	15,                 /* DELAY_ONE_CTL_CLK_MSW   */
	 3,                 /* DELAY_TWO_CTL_CLK_MSW   */
	 0,                 /* DELAY_THREE_CTL_CLK_MSW */
	 0,                 /* DELAY_FOUR_CTL_CLK_MSW  */
	 2                  /* DELAY_SELECTION_CLK_MSW */
#endif
};

static const uint8_t s_aucDeskewClkLsw[BVDC_P_HDDVI_DESKEW_CLK_COUNT] =
{
#if ((BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK) || \
     (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP))
	 0,                 /* DELAY_ONE_CTL_CLK_LSW   */
	 0,                 /* DELAY_TWO_CTL_CLK_LSW   */
	 0,                 /* DELAY_THREE_CTL_CLK_LSW */
	 0,                 /* DELAY_FOUR_CTL_CLK_LSW  */
	 0                  /* DELAY_SELECTION_CLK_LSW */
#else
	15,                 /* DELAY_ONE_CTL_CLK_LSW   */
	 3,                 /* DELAY_TWO_CTL_CLK_LSW   */
	 0,                 /* DELAY_THREE_CTL_CLK_LSW */
	 0,                 /* DELAY_FOUR_CTL_CLK_LSW  */
	 2                  /* DELAY_SELECTION_CLK_LSW */
#endif
};


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_HdDvi_Create
	( BVDC_P_HdDvi_Handle             *phHdDvi,
	  BVDC_P_HdDviId                   eHdDviId,
	  BREG_Handle                      hReg,
	  BVDC_Source_Handle               hSource )
{
	BVDC_P_HdDviContext *pHdDvi;

	BDBG_ENTER(BVDC_P_HdDvi_Create);
	BDBG_ASSERT(phHdDvi);
	BDBG_ASSERT(hReg);
	BDBG_ASSERT(hSource);

	/* BDBG_SetModuleLevel("BVDC_HDDVI", BDBG_eMsg); */

	/* Alloc the context. */
	pHdDvi = (BVDC_P_HdDviContext*)(BKNI_Malloc(sizeof(BVDC_P_HdDviContext)));
	if(!pHdDvi)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pHdDvi, 0x0, sizeof(BVDC_P_HdDviContext));
	BDBG_OBJECT_SET(pHdDvi, BVDC_DVI);

	pHdDvi->eId          = eHdDviId;
	pHdDvi->hReg         = hReg;
	pHdDvi->hSource      = hSource;

	/* Allocate scratch register */
	pHdDvi->ulBridgeErrRegAddr = BRDC_AllocScratchReg(pHdDvi->hSource->hVdc->hRdc);
	if(!pHdDvi->ulBridgeErrRegAddr)
	{
		BDBG_ERR(("Not enough scratch registers for error scratch!"));
		BDBG_OBJECT_DESTROY(pHdDvi, BVDC_DVI);
		BKNI_Free((void*)pHdDvi);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Allocate scratch register */
	pHdDvi->ulPctrErrRegAddr = BRDC_AllocScratchReg(pHdDvi->hSource->hVdc->hRdc);
	if(!pHdDvi->ulPctrErrRegAddr)
	{
		BDBG_ERR(("Not enough scratch registers for error scratch!"));
		BDBG_OBJECT_DESTROY(pHdDvi, BVDC_DVI);
		BKNI_Free((void*)pHdDvi);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

#if ((BVDC_P_SUPPORT_HDDVI > 1) && \
	 (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_1))  /* 2 new hddvi's */
	/* Regigister offset from HD_DVI_0. */
	pHdDvi->ulOffset     = BVDC_P_HDDVI_GET_REG_OFFSET(eHdDviId,
		BCHP_HD_DVI_0_REG_START, BCHP_HD_DVI_1_REG_START);
#endif

	BVDC_P_HdDvi_Init(pHdDvi);
	BDBG_MSG(("Create HdDvi source %d", eHdDviId));

	/* All done. now return the new fresh context to user. */
	*phHdDvi = (BVDC_P_HdDvi_Handle)pHdDvi;

	BDBG_LEAVE(BVDC_P_HdDvi_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_HdDvi_Destroy
	( BVDC_P_HdDvi_Handle              hHdDvi )
{
	BDBG_ENTER(BVDC_P_HdDvi_Destroy);
	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);

	/* Free scratch register */
	BRDC_FreeScratchReg(hHdDvi->hSource->hVdc->hRdc, hHdDvi->ulBridgeErrRegAddr);
	BRDC_FreeScratchReg(hHdDvi->hSource->hVdc->hRdc, hHdDvi->ulPctrErrRegAddr);

	BDBG_OBJECT_DESTROY(hHdDvi, BVDC_DVI);
	/* Release context in system memory */
	BKNI_Free((void*)hHdDvi);
	BDBG_LEAVE(BVDC_P_HdDvi_Destroy);
	return;
}


/***************************************************************************
 *
 */
void BVDC_P_HdDvi_Init
	( BVDC_P_HdDvi_Handle              hHdDvi )
{
	BAVC_VDC_HdDvi_Picture *pCurPic;
	const BFMT_VideoInfo *pFmtInfo;
	uint32_t  ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	BDBG_OBJECT_ASSERT(hHdDvi->hSource, BVDC_SRC);

	/* Initial format! */
	pFmtInfo = hHdDvi->hSource->stCurInfo.pFmtInfo
		? hHdDvi->hSource->stCurInfo.pFmtInfo
		: BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_eNTSC);

	/* Initial CSC!  It uses the same Csc as compositor's csc. */
	BVDC_P_Csc_GetHdDviTable_isr(&hHdDvi->stCsc, hHdDvi->stXvdField.eCscMode);

	/* For 3563 the HDR input is connected thru AUX port! */
	hHdDvi->b24BitsMode          = false;
	hHdDvi->ulPixelDecimate      = 0; /* No decimation. */
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
	hHdDvi->eInputColorSpace     = BVDC_P_HdDvi_InputColorSpace_eRGB;
#else
	hHdDvi->eInputColorSpace     = BVDC_P_HdDvi_InputColorSpace_eYCbCr444;
#endif
	hHdDvi->ulVertFreqTolerance  = BVDC_P_HDDVI_VERTFREQ_TOLER;
	hHdDvi->ulScanWidthTolerance = BVDC_P_HDDVI_SCANLINE_TOLER;
	hHdDvi->hSource->bSrcIs444   =
		BVDC_P_HDDVI_INPUT_RGB(hHdDvi->eInputColorSpace) ||
		BVDC_P_HDDVI_INPUT_444(hHdDvi->eInputColorSpace);

	/* Auto format detection. */
	hHdDvi->bVideoDetected     = false;
	hHdDvi->bResetFormatDetect = true;
#if (BVDC_P_HDDVI_SW_RESET)
	hHdDvi->bSwReset           = false;
#endif
	hHdDvi->ulPixelDecimateCnt = 0;
	hHdDvi->bReverseTrigger    = false;

	hHdDvi->stDnSampler.eFilterType = BVDC_444To422Filter_eStandard;
	hHdDvi->stDnSampler.eRingRemoval = BVDC_RingSuppressionMode_eNormal;

	hHdDvi->stUpSampler.bUnbiasedRound = true;
	hHdDvi->stUpSampler.eRingRemoval   = BVDC_RingSuppressionMode_eDisable;
	hHdDvi->stUpSampler.eFilterType = BVDC_422To444Filter_eTenTaps;

	/* Dither */
	if(hHdDvi->eId == BVDC_P_HdDviId_eHdDvi1)
	{
		ulLfsrCtrlT0 = BVDC_P_DITHER_HDDVI1_LFSR_CTRL_T0;
		ulLfsrCtrlT1 = BVDC_P_DITHER_HDDVI1_LFSR_CTRL_T1;
		ulLfsrCtrlT2 = BVDC_P_DITHER_HDDVI1_LFSR_CTRL_T2;
	}
	else
	{
		ulLfsrCtrlT0 = BVDC_P_DITHER_HDDVI0_LFSR_CTRL_T0;
		ulLfsrCtrlT1 = BVDC_P_DITHER_HDDVI0_LFSR_CTRL_T1;
		ulLfsrCtrlT2 = BVDC_P_DITHER_HDDVI0_LFSR_CTRL_T2;
	}
	ulLfsrValue  = BVDC_P_DITHER_HDDVI_LFSR_VALUE;

	BVDC_P_Dither_Init(&hHdDvi->stDither,
		ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	/* Handed the downstream BVN with the local filled in.   This is if
	 * there is no callback to tell us what the decode frame should be. */
	pCurPic  = &hHdDvi->stXvdField;

	/* Init current field. */
	pCurPic->bMute                    = false;
	pCurPic->eSourcePolarity          = BVDC_P_NEXT_POLARITY(hHdDvi->hSource->eNextFieldId);
	pCurPic->eAspectRatio             = pFmtInfo->eAspectRatio;
	pCurPic->eCscMode                 = BAVC_CscMode_e709RgbFullRange;
	pCurPic->eColorSpace              = BAVC_Colorspace_eYCbCr444;
	pCurPic->eMatrixCoefficients      = BAVC_MatrixCoefficients_eItu_R_BT_709;
	pCurPic->eTransferCharacteristics = BAVC_TransferCharacteristics_eItu_R_BT_709;
	pCurPic->ulSourceHorizontalSize   = pFmtInfo->ulDigitalWidth;
	pCurPic->ulSourceVerticalSize     = pFmtInfo->ulDigitalHeight;

	return;
}


/***************************************************************************
 * Validate the settings in pNewInfo set by user!
 *
 */
BERR_Code BVDC_P_HdDvi_ValidateChanges
	( BVDC_P_HdDvi_Handle               hHdDvi )
{
	BVDC_P_Source_Info *pNewInfo;
	BVDC_HdDvi_Settings  *pNewSettings;
	BVDC_HdDvi_FormatTolerance *pNewTolerance;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	BDBG_OBJECT_ASSERT(hHdDvi->hSource, BVDC_SRC);

	/* New Information from user, and derived information. */
	pNewInfo  = &hHdDvi->hSource->stNewInfo;
	pNewSettings = &pNewInfo->stHdDviSetting;
	pNewTolerance = &pNewSettings->stFmtTolerence;

	/* Get configuration for PC Input if any. */
	if(pNewInfo->stDirty.stBits.bInputFormat)
	{
		BDBG_MSG(("User set format for HD_DVI[%d] = %s", hHdDvi->eId,
			pNewInfo->pFmtInfo->pchFormatStr));
	}

	/* Auto Format detection. */
	pNewInfo->bAutoFmtDetection = pNewInfo->bAutoDetect;

	/* Check format tolerance */
	if((pNewTolerance->ulWidth > BVDC_P_HDDVI_FORMAT_TOLER_MAX_WIDTH) ||
	   (pNewTolerance->ulHeight > BVDC_P_HDDVI_FORMAT_TOLER_MAX_HEIGHT))
	{
		BDBG_ERR(("Invalidate format tolerance settings"));
		return BERR_TRACE(BVDC_ERR_INVALID_TOLERANCE);
	}

	return BERR_SUCCESS;
}


/***************************************************************************
 * Read HD_DVI input status!  Helper function!
 *
 */
static void BVDC_P_HdDvi_ReadHwStatus_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_HdDviInput               *pStatus )
{
	uint32_t ulReg;
	uint32_t ulAux;
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;

	/* Error flag: HD_DVI_0_PCTR_ERROR_STATUS */
	ulReg = BREG_Read32_isr(hHdDvi->hReg, hHdDvi->ulPctrErrRegAddr);
	hHdDvi->bPctrErr =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_PCTR_ERROR_STATUS, INPUT_STALLED      ) ||
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_PCTR_ERROR_STATUS, INPUT_STALLED_ERROR);

	hHdDvi->bBvbErr =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_PCTR_ERROR_STATUS, LONG_PICTURE_ERROR ) ||
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_PCTR_ERROR_STATUS, SHORT_PICTURE_ERROR) ||
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_PCTR_ERROR_STATUS, LONG_LINE_ERROR    ) ||
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_PCTR_ERROR_STATUS, SHORT_LINE_ERROR   );

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_4)
	hHdDvi->bFifoErr =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_PCTR_ERROR_STATUS, DROPPED_DATA_ERROR);
#else
	/* HD_DVI_0_BUFFER_FIFO */
	ulReg   = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_BUFFER_FIFO + ulOffset);
	hHdDvi->bFifoErr =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_BUFFER_FIFO, OVERFLOW);
#endif

	/* HD_DVI_0_BRIDGE_ERRORS_RDB_CLR */
	ulReg = BREG_Read32_isr(hHdDvi->hReg, hHdDvi->ulBridgeErrRegAddr);
	hHdDvi->bBridgeErr  =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_BRIDGE_ERRORS_RDB_CLR, HOST_ERROR_RDB_CLR          ) ||
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_BRIDGE_ERRORS_RDB_CLR, MISSED_PICTURE_ERROR_RDB_CLR);

	/* VID_FORMAT_1 & VID_FORMAT_2 */
	ulReg    = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_1 + ulOffset);
	ulAux    = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_2 + ulOffset);
	pStatus->ulHPolarity =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_1, UUT_HPOL);

	pStatus->ulHFrontPorch =
		BCHP_GET_FIELD_DATA(ulAux, HD_DVI_0_VID_FORMAT_2, UUT_HFP);
	pStatus->ulHBackPorch =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_1, UUT_HBP);

	pStatus->ulHBlank = (
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_1, UUT_HSP) +
		pStatus->ulHBackPorch + pStatus->ulHFrontPorch);

	pStatus->ulAvWidth  =
		BCHP_GET_FIELD_DATA(ulAux, HD_DVI_0_VID_FORMAT_2, UUT_HAP);

	/* VID_FORMAT_3 & VID_FORMAT_4 */
	ulReg    = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_3 + ulOffset);
	ulAux    = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_4 + ulOffset);
	pStatus->ulVPolarity  =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_3, UUT_VPOL);

	pStatus->bInterlaced =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_3, INTERLACED);

	pStatus->ulVFrontPorch =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_3, UUT_VFP0);
	pStatus->ulVBackPorch =
		BCHP_GET_FIELD_DATA(ulAux, HD_DVI_0_VID_FORMAT_4, UUT_VBP0);

	pStatus->ulVBlank = (
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_3, UUT_VSP0) +
		pStatus->ulVFrontPorch + pStatus->ulVBackPorch);

	/* VID_FORMAT_5*/
	ulReg = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_5 + ulOffset);
	pStatus->ulBottomVFrontPorch =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_5, UUT_VFP1);

	/* VID_FORMAT_6*/
	ulReg = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_6 + ulOffset);
	pStatus->ulBottomVBackPorch =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_6, UUT_VBP1);

	/* VID_FORMAT_8 */
	ulReg    = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_8 + ulOffset);
	pStatus->ulAvHeight =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_8, UUT_VAL0);

	pStatus->ulBottomVBlank = (
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_8, UUT_VSP1) +
		pStatus->ulBottomVFrontPorch + pStatus->ulBottomVBackPorch);

	/* Adjust for interfaced/progressive */
	pStatus->ulVBlank   <<= pStatus->bInterlaced;
	pStatus->ulAvHeight <<= pStatus->bInterlaced;

	/* VSYNC_CNT_FIELDx */
	ulReg   = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VSYNC_CNT_FIELD0 + ulOffset);
	pStatus->ulClkPerVsync =
		BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VSYNC_CNT_FIELD0, VSYNC_CNT);

	/* Prevent dividing by zero */
	pStatus->ulClkPerVsync = BVDC_P_MAX(pStatus->ulClkPerVsync, BFMT_FREQ_FACTOR);

	/* Vertical frequency of this format!  Prevent dividing by zero when
	 *input is removed. */
	pStatus->ulVertFreq =
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
		BVDC_P_108_SYSCLK / (pStatus->ulClkPerVsync / BFMT_FREQ_FACTOR);
#else
		BVDC_P_216_SYSCLK / (pStatus->ulClkPerVsync / BFMT_FREQ_FACTOR);
#endif

	/* Pixel frequency */
	pStatus->ulPxlFreq =((pStatus->ulVertFreq) *
		 ((pStatus->ulAvWidth  + pStatus->ulHBlank) *
		  (pStatus->ulAvHeight + pStatus->ulVBlank) / BFMT_FREQ_FACTOR));
	pStatus->ulPxlFreq /= 10000; /* xx.yy Mhz */

	/* If a new Vsync signal comes, the register will be updated.
	 * If there's no Vsync siganl for a while, the register will be added
	 * continually up to 0xfffffff. */
	pStatus->bPllDetected = (
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
		(BVDC_P_108_SYSCLK != hHdDvi->stStatus.ulVertFreq) &&
#else
		(BVDC_P_216_SYSCLK != hHdDvi->stStatus.ulVertFreq) &&
#endif
		(BCHP_MASK(HD_DVI_0_VSYNC_CNT_FIELD0, VSYNC_CNT) != pStatus->ulClkPerVsync));

	return;
}


#if (BVDC_P_HDDVI_SW_RESET)
/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildResetSw_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;

#if BVDC_P_SUPPORT_NEW_SW_INIT
	/* HD_DVI_0_SW_INIT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_SW_INIT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_SW_INIT, CORE,          1 );

	/* HD_DVI_0_SW_INIT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_SW_INIT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_SW_INIT, CORE,          0 );
#else
	/* HD_DVI_0_SW_RESET_CNTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_SW_RESET_CNTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_SW_RESET_CNTRL, SW_RESET_HD_DVI, 1);

	/* HD_DVI_0_SW_RESET_CNTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_SW_RESET_CNTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_SW_RESET_CNTRL, SW_RESET_HD_DVI, 0);
#endif

	return;
}
#endif

/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildReset_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  bool                             bResetBvb )
{
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;

#if BVDC_P_SUPPORT_NEW_SW_INIT
	/* HD_DVI_0_SW_INIT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_SW_INIT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_SW_INIT, BVB_BRIDGE,
			bResetBvb                                     ) |
		BCHP_FIELD_DATA(HD_DVI_0_SW_INIT, FORMAT_DETECT,
			hHdDvi->bResetFormatDetect                    ) |
		BCHP_FIELD_DATA(HD_DVI_0_SW_INIT, CORE,         0 );
#else
	/* HD_DVI_0_SW_RESET_CNTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_SW_RESET_CNTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_SW_RESET_CNTRL, SW_RESET_BVB_BRIDGE,
			bResetBvb                                                    ) |
		BCHP_FIELD_DATA(HD_DVI_0_SW_RESET_CNTRL, SW_RESET_FORMAT_DETECT,
			hHdDvi->bResetFormatDetect                                   ) |
		BCHP_FIELD_DATA(HD_DVI_0_SW_RESET_CNTRL, SW_RESET_HD_DVI,      0 );
#endif

	/* Only reset format detect once */
	hHdDvi->bResetFormatDetect = false;

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildTopRul_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	BDBG_OBJECT_ASSERT(hHdDvi->hSource, BVDC_SRC);
	BSTD_UNUSED(pCurInfo);
	ulOffset = hHdDvi->ulOffset;

	/* Picture clean up cfg1 shadow */
	hHdDvi->ulPicCleanUpCfg1 =
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, BYPASS_THROTTLE,           0  ) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, BYPASS,                    0  ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, MASK_SYNC_ON_TIMING_ERROR, 1  ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, CLEAR_ERRORS,              1  ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, IGNORE_SYNCS,              1  ) |
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, INPUT_HSYNC_POLARITY,      1  ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, INPUT_VSYNC_POLARITY,      1  ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, MASK_PIXEL_VALUE,     0xA00000);
#else
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, INPUT_HSYNC_POLARITY,      0  ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, INPUT_VSYNC_POLARITY,      0  ) |
		(BVDC_P_HDDVI_INPUT_RGB(hHdDvi->eInputColorSpace)
		? BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, MASK_PIXEL_VALUE, 0x000000)
		: BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, MASK_PIXEL_VALUE, 0x801080) ); /* CrYCb */
#endif

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
	/* HD_DVI_0_INPUT_TIMING_ADJUST_MSW_X (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_MSW_X + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_X, DATA_MSW_35, s_aucInputTimingMsw[35]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_X, DATA_MSW_34, s_aucInputTimingMsw[34]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_X, DATA_MSW_33, s_aucInputTimingMsw[33]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_X, DATA_MSW_32, s_aucInputTimingMsw[32]);

	/* HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y, DATA_MSW_31, s_aucInputTimingMsw[31]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y, DATA_MSW_30, s_aucInputTimingMsw[30]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y, DATA_MSW_29, s_aucInputTimingMsw[29]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y, DATA_MSW_28, s_aucInputTimingMsw[28]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y, DATA_MSW_27, s_aucInputTimingMsw[27]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y, DATA_MSW_26, s_aucInputTimingMsw[26]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y, DATA_MSW_25, s_aucInputTimingMsw[25]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Y, DATA_MSW_24, s_aucInputTimingMsw[24]);

	/* HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z, DATA_MSW_23, s_aucInputTimingMsw[23]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z, DATA_MSW_22, s_aucInputTimingMsw[22]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z, DATA_MSW_21, s_aucInputTimingMsw[21]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z, DATA_MSW_20, s_aucInputTimingMsw[20]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z, DATA_MSW_19, s_aucInputTimingMsw[19]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z, DATA_MSW_18, s_aucInputTimingMsw[18]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z, DATA_MSW_17, s_aucInputTimingMsw[17]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_Z, DATA_MSW_16, s_aucInputTimingMsw[16]);
#endif

	/* HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A, DATA_MSW_15, s_aucInputTimingMsw[15]) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A, DATA_MSW_14, s_aucInputTimingMsw[14]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A, DATA_MSW_13, s_aucInputTimingMsw[13]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A, DATA_MSW_12, s_aucInputTimingMsw[12]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A, DATA_MSW_11, s_aucInputTimingMsw[11]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A, DATA_MSW_10, s_aucInputTimingMsw[10]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A, DATA_MSW_9,  s_aucInputTimingMsw[9] ) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_A, DATA_MSW_8,  s_aucInputTimingMsw[8]);

	/* HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B, DATA_MSW_7, s_aucInputTimingMsw[7]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B, DATA_MSW_6, s_aucInputTimingMsw[6]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B, DATA_MSW_5, s_aucInputTimingMsw[5]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B, DATA_MSW_4, s_aucInputTimingMsw[4]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B, DATA_MSW_3, s_aucInputTimingMsw[3]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B, DATA_MSW_2, s_aucInputTimingMsw[2]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B, DATA_MSW_1, s_aucInputTimingMsw[1]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_B, DATA_MSW_0, s_aucInputTimingMsw[0]);

	/* HD_DVI_0_INPUT_TIMING_ADJUST_MSW_C (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_MSW_C + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_C, DE_MSW,    s_aucInputTimingMsw[38]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_C, VSYNC_MSW, s_aucInputTimingMsw[37]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_MSW_C, HSYNC_MSW, s_aucInputTimingMsw[36]);

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
	/* HD_DVI_0_INPUT_TIMING_ADJUST_LSW_X (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_LSW_X + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_X, DATA_LSW_35, s_aucInputTimingLsw[35]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_X, DATA_LSW_34, s_aucInputTimingLsw[34]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_X, DATA_LSW_33, s_aucInputTimingLsw[33]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_X, DATA_LSW_32, s_aucInputTimingLsw[32]);

	/* HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y, DATA_LSW_31, s_aucInputTimingLsw[31]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y, DATA_LSW_30, s_aucInputTimingLsw[30]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y, DATA_LSW_29, s_aucInputTimingLsw[29]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y, DATA_LSW_28, s_aucInputTimingLsw[28]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y, DATA_LSW_27, s_aucInputTimingLsw[27]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y, DATA_LSW_26, s_aucInputTimingLsw[26]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y, DATA_LSW_25, s_aucInputTimingLsw[25]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Y, DATA_LSW_24, s_aucInputTimingLsw[24]);

	/* HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z, DATA_LSW_23, s_aucInputTimingLsw[23]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z, DATA_LSW_22, s_aucInputTimingLsw[22]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z, DATA_LSW_21, s_aucInputTimingLsw[21]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z, DATA_LSW_20, s_aucInputTimingLsw[20]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z, DATA_LSW_19, s_aucInputTimingLsw[19]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z, DATA_LSW_18, s_aucInputTimingLsw[18]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z, DATA_LSW_17, s_aucInputTimingLsw[17]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_Z, DATA_LSW_16, s_aucInputTimingLsw[16]);
#endif

	/* HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A, DATA_LSW_15, s_aucInputTimingLsw[15]) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A, DATA_LSW_14, s_aucInputTimingLsw[14]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A, DATA_LSW_13, s_aucInputTimingLsw[13]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A, DATA_LSW_12, s_aucInputTimingLsw[12]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A, DATA_LSW_11, s_aucInputTimingLsw[11]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A, DATA_LSW_10, s_aucInputTimingLsw[10]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A, DATA_LSW_9,  s_aucInputTimingLsw[9] ) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_A, DATA_LSW_8,  s_aucInputTimingLsw[8] );

	/* HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B, DATA_LSW_7, s_aucInputTimingLsw[7]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B, DATA_LSW_6, s_aucInputTimingLsw[6]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B, DATA_LSW_5, s_aucInputTimingLsw[5]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B, DATA_LSW_4, s_aucInputTimingLsw[4]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B, DATA_LSW_3, s_aucInputTimingLsw[3]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B, DATA_LSW_2, s_aucInputTimingLsw[2]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B, DATA_LSW_1, s_aucInputTimingLsw[1]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_B, DATA_LSW_0, s_aucInputTimingLsw[0]);

	/* HD_DVI_0_INPUT_TIMING_ADJUST_LSW_C (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_TIMING_ADJUST_LSW_C + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_C, DE_LSW,    s_aucInputTimingLsw[38]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_C, VSYNC_LSW, s_aucInputTimingLsw[37]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_TIMING_ADJUST_LSW_C, HSYNC_LSW, s_aucInputTimingLsw[36]);

	/* HD_DVI_0_DESKEW_CLK_MSW (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DESKEW_CLK_MSW + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_SELECTION_CLK_MSW, s_aucDeskewClkMsw[4]) |
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_FOUR_CTL_CLK_MSW,  s_aucDeskewClkMsw[3]) |
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_THREE_CTL_CLK_MSW, s_aucDeskewClkMsw[2]) |
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_TWO_CTL_CLK_MSW,   s_aucDeskewClkMsw[1]) |
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_ONE_CTL_CLK_MSW,   s_aucDeskewClkMsw[0]);

	/* HD_DVI_0_DESKEW_CLK_LSW (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DESKEW_CLK_LSW + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_SELECTION_CLK_LSW, s_aucDeskewClkLsw[4]) |
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_FOUR_CTL_CLK_LSW,  s_aucDeskewClkLsw[3]) |
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_THREE_CTL_CLK_LSW, s_aucDeskewClkLsw[2]) |
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_TWO_CTL_CLK_LSW,   s_aucDeskewClkLsw[1]) |
		BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_ONE_CTL_CLK_LSW,   s_aucDeskewClkLsw[0]);

	/* HD_DVI_0_BVB_CONVERT (RW) */
	/* HD_DVI_0_DMA_TRIG_CNTRL (RW) */
	/* HD_DVI_0_HOST_LEGACY (RW) */

#if (BVDC_P_SUPPORT_HDDVI_VER < BVDC_P_HDDVI_NEW_VER_9)
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HOST_LEGACY + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER < BVDC_P_HDDVI_NEW_VER_7)
		BCHP_FIELD_DATA(HD_DVI_0_HOST_LEGACY, USE_LEGACY_TRIGGER,          0 ) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_HOST_LEGACY, USE_HOST_EN_DIS,             1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_HOST_LEGACY, USE_BRIDGE_DISCARD_PICTURE,  1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_HOST_LEGACY, USE_PCTR_CU_DISCARD_PICTURE, 1 );
#endif

	/* HD_DVI_0_AUTO_REPEAT (RW) */
	/* HD_DVI_0_PICTURE_DELAY (RW) */

	/* KLUDGE: This hardware block require a pulse of 1->0 in order for it
	 * to generate a subsequence triggers otherwise we can't get the system
	 * rolling. */
	/* HD_DVI_0_HOST_ENABLE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HOST_ENABLE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HOST_ENABLE, HOST_ENABLE, 1 );

	/* HD_DVI_0_HOST_ENABLE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HOST_ENABLE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HOST_ENABLE, HOST_ENABLE, 0 );

	/* HD_DVI_0_HOST_CONTROL_BITS (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HOST_CONTROL_BITS + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, HOST_DEBUG,   0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, HOST_DISABLE, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, CLEAR_ERRORS, 0 );

	/* HD_DVI_0_VID_FORMAT_UPDATE_CLEAR (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_VID_FORMAT_UPDATE_CLEAR + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_4)
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_SATURATED_VSYNC_PERIOD, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_SATURATED_DE,           0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_SATURATED_HSYNC,        0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_SATURATED_VSYNC,        0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_SATURATED_VAL,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_SATURATED_VFP,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_SATURATED_VBP,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_SATURATED_HSYNC_DELAY,  0 ) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_HPOL,           0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_HAP,            0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_HFP,            0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_HSP,            0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_HBP,            0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VPOL,           0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VAL1,           0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VFP_1,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VSP_1,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VBP_1,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VSPO_1,         0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VAL2,           0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VFP_2,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VSP_2,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VBP_2,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_VSPO_2,         0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_PROGRESSIVE,    0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_UPDATE_CLEAR, CLEAR_UPDATED_INTERLACED,     0 );

	/* HD_DVI_0_PCTR_CLEAN_CFG_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_1 + ulOffset);
	*pList->pulCurrent++ = hHdDvi->ulPicCleanUpCfg1;

	/* HD_DVI_0_PCTR_CLEAN_CFG_2 (RW) */
	/* HD_DVI_0_PCTR_CLEAN_CFG_3 (RW) */
	/* HD_DVI_0_PCTR_CLEAN_CFG_4 (RW) */

	/* HD_DVI_0_VID_FORMAT_CFG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_VID_FORMAT_CFG + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_4)
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, VBLANK_FIELD_DET_MODE,              1 ) |
#endif
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_6)
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, BLOCK_HSYNC_RISE_COUNT_ROLLOVER,    1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, BLOCK_HSYNC_FALL_COUNT_ROLLOVER,    1 ) |
#endif
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, VSYNC_AVERAGING,                    1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, BLOCK_V_FRONT_PORCH_COUNT_ROLLOVER, 1 ) |
#else
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, BLOCK_V_FRONT_PROCH_COUNT_ROLLOVER, 1 ) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, BLOCK_V_ACTIVE_LINE_COUNT_ROLLOVER, 1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, BLOCK_DE_COUNT_ROLLOVER,            1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, BLOCK_HSYNC_COUNT_ROLLOVER,         1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_VID_FORMAT_CFG, BLOCK_VSYNC_COUNT_ROLLOVER,         1 );

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
	/* HD_DVI_0_SIDEBAND_CFG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_SIDEBAND_CFG + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_SIDEBAND_CFG, USE_EXT_FIELD,       0                 ) |
		BCHP_FIELD_ENUM(HD_DVI_0_SIDEBAND_CFG, CONFIG_SAMPLE_POINT, LAST_BUT_ONE_LINE );
#endif

	/* HD_DVI_0_VID_FORMAT_1 (RO) */
	/* HD_DVI_0_VID_FORMAT_2 (RO) */
	/* HD_DVI_0_VID_FORMAT_3 (RO) */
	/* HD_DVI_0_VID_FORMAT_4 (RO) */
	/* HD_DVI_0_VID_FORMAT_5 (RO) */
	/* HD_DVI_0_VID_FORMAT_6 (RO) */
	/* HD_DVI_0_VID_FORMAT_7 (RO) */
	/* HD_DVI_0_VID_FORMAT_8 (RO) */
	/* HD_DVI_0_VSYNC_CNT_FIELD0 (RO) */
	/* HD_DVI_0_VSYNC_CNT_FIELD1 (RO) */
	/* HD_DVI_0_COUNT_AT_FIRST_PIXEL (RO) */
	/* HD_DVI_0_VID_FORMAT_UPDATE_STATUS (RO) */
	/* HD_DVI_0_PCTR_ERROR_STATUS (RO) */
	/* HD_DVI_0_BRIDGE_ERRORS_AUTO_CLR (RO) */
	/* HD_DVI_0_BRIDGE_ERRORS_RDB_CLR (RO) */
	/* HD_DVI_0_BUFFER_FIFO (RW) */

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_BUFFER_FIFO + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER <= BVDC_P_HDDVI_NEW_VER_3)
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, LOAD_WR,      0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, HOLD_WR,      0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, FULL_MINUS,   0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, ALMOST_EMPTY, 1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, UNDERFLOW,    1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, HOLD_RD,      1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, EMPTY_MINUS,  0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, WR_ADDR_7_0,  0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, RD_ADDR_7_0,  0 ) |
#else
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, CLEAR_STATUS, 1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, WR_ADDR,      1 ) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, ALMOST_FULL,  0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_BUFFER_FIFO, OVERFLOW,     0 );

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_4)
	/* HD_DVI_0_REGISTER_METHOD (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_REGISTER_METHOD + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_REGISTER_METHOD, DLY_COUNTER_INIT,       15 ) |
		BCHP_FIELD_DATA(HD_DVI_0_REGISTER_METHOD, WAIT_COUNTER_INIT,      15 ) |
		BCHP_FIELD_DATA(HD_DVI_0_REGISTER_METHOD, BYPASS_TRIGGER_GEN,     0  ) |
		BCHP_FIELD_DATA(HD_DVI_0_REGISTER_METHOD, CLEAR_ERROR_BY_PICTURE, 0  ) |
		BCHP_FIELD_DATA(HD_DVI_0_REGISTER_METHOD, UPDATE_RDB_BY_PICTURE,  0  );

	/* HD_DVI_0_RDB_TRIGGER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_RDB_TRIGGER + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_RDB_TRIGGER, RDB_TRIGGER, 0 );

	/* HD_DVI_0_PCTR_CLEAN_CFG_5 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_5 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_5, USE_DE_WATCHDOG,  0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_5, PICTURE_FINISH,   0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_5, PICTURE_GENERATE, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_5, SINK_DATA,        1 ) |
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_5, FAST_PAD,         1 );
#else
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_5, FAST_PAD,         0 );
#endif

	/* HD_DVI_0_PCTR_CLEAN_CFG_7 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_7 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_7, DE_WATCHDOG, 0 );

	/* HD_DVI_0_PCTR_CLEAN_CFG_8 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_8 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_8, TIME_OUT_LINE_ADJ, 4095 );

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
	/* HD_DVI_0_PCTR_CLEAN_CFG_9 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_9 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_9, PAD_NCO_STEP_SIZE, 92274688 );

	/* HD_DVI_0_POSTING_MASTER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_POSTING_MASTER + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_POSTING_MASTER, POSTING_COUNT, 511 );

	/* HD_DVI_0_CRC_CHECK_CFG_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_CRC_CHECK_CFG_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CFG_0, MASK_SYNCS,           0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CFG_0, CRC_AUTO_CLEAR_ERROR, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CFG_0, CRC_INIT_MODE,        0 );

	/* HD_DVI_0_CRC_CHECK_CFG_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_CRC_CHECK_CFG_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CFG_1, CRC_EXPECTED_VALUE, 1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CFG_1, CRC_INITIAL_VALUE,  1 );

	/* HD_DVI_0_CRC_CHECK_CTL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_CRC_CHECK_CTL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CTL, CRC_CLEAR_ERROR,   0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CTL, CRC_CHECK_ERROR,   0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CTL, INIT_AND_SAVE_CRC, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CTL, SAVE_CRC,          0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_CRC_CHECK_CTL, ENABLE_CRC,        1 );

	/* HD_DVI_0_CRC_CHECK_STATUS_0 (RO) */
	/* HD_DVI_0_CRC_CHECK_STATUS_1 (RO) */
#endif

	/* HD_DVI_0_DITHER_LFSR_INIT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DITHER_LFSR_INIT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_LFSR_INIT, SEQ,   hHdDvi->stDither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_LFSR_INIT, VALUE, hHdDvi->stDither.ulLfsrValue);

	/* HD_DVI_0_DITHER_LFSR_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DITHER_LFSR_CTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_LFSR_CTRL, T0, hHdDvi->stDither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_LFSR_CTRL, T1, hHdDvi->stDither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_LFSR_CTRL, T2, hHdDvi->stDither.ulLfsrCtrlT2);

	/* HD_DVI_0_DITHER_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DITHER_CTRL + ulOffset);
	*pList->pulCurrent++ =
		((hHdDvi->hSource->bTestFeature1)
		? BCHP_FIELD_DATA(HD_DVI_0_DITHER_CTRL, MODE,     0                          )
		: BCHP_FIELD_DATA(HD_DVI_0_DITHER_CTRL, MODE,     hHdDvi->stDither.ulMode    )) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_CTRL, OFFSET_CH2, hHdDvi->stDither.ulCh2Offset) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_CTRL, SCALE_CH2,  hHdDvi->stDither.ulCh2Scale ) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_CTRL, OFFSET_CH1, hHdDvi->stDither.ulCh1Offset) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_CTRL, SCALE_CH1,  hHdDvi->stDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_CTRL, OFFSET_CH0, hHdDvi->stDither.ulCh0Offset) |
		BCHP_FIELD_DATA(HD_DVI_0_DITHER_CTRL, SCALE_CH0,  hHdDvi->stDither.ulCh0Scale );
#endif
	/* HD_DVI_0_TEST_CONFIG (RW) */
	/* HD_DVI_0_SPARE_REG2 (RW) */

	return;
}

/***************************************************************************
 * Build RUL related to input format: size, triggers etc
 *
 */
static void BVDC_P_HdDvi_BuildFormatRul_Isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulHeight;
	uint32_t ulTrig0, ulTrig1;
	uint32_t ulAutoRepeat, ulPicDelay;
	const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;
	uint32_t ulOffset;
	uint32_t  ulActiveSpace;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;

	/* Set trigger to be BVDC_P_HDDVI_TRIGGER_OFFSET lines after end of picture
	 * to get both triggers */
	ulHeight = pFmtInfo->ulDigitalHeight >> pFmtInfo->bInterlaced;
	ulActiveSpace = pFmtInfo->ulActiveSpace;
	if(pFmtInfo->eOrientation == BFMT_Orientation_e3D_OverUnder)
	{
		ulHeight = 2 * ulHeight + ulActiveSpace;
	}

#if BFMT_LEGACY_3DTV_SUPPORT
	if(BFMT_IS_3D_MODE(pFmtInfo->eVideoFmt))
	{
		if(pCurInfo->bHVStartOverride &&
		   pCurInfo->stHdDviSetting.bEnableDe)
		{
			ulHeight = pFmtInfo->ulDigitalHeight - pCurInfo->ulVstart + pFmtInfo->ulTopActive - 1;
		}
		else
		{
			ulHeight = VIDEO_FORMAT_IS_1080P_3DOU_AS(pFmtInfo->eVideoFmt)
				? BFMT_1080P_HEIGHT : pFmtInfo->ulDigitalHeight;
		}
	}
#endif

	/* Where to generate the triggers? */
	ulTrig0 = ulHeight + BVDC_P_HDDVI_TRIGGER_OFFSET;
	ulTrig1 = (pFmtInfo->bInterlaced) ? ulTrig0 : 0;

	/* New trigger mode.  How often to repeat the trigger in sysclk cycle unit */
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
	ulAutoRepeat = (BVDC_P_108_SYSCLK / pFmtInfo->ulVertFreq) * BFMT_FREQ_FACTOR;
#else
	ulAutoRepeat = (BVDC_P_216_SYSCLK / pFmtInfo->ulVertFreq) * BFMT_FREQ_FACTOR;
#endif

	ulPicDelay   = (ulAutoRepeat / pFmtInfo->ulScanHeight);      /* Single line delay */
	ulPicDelay  *= BVDC_P_TRIGGER_OFFSET(pFmtInfo->ulTopActive); /* X% of Vertical Blanking Area */

	/* Saved the SysClk per vsync to determined 1/1000 part difference. */
	hHdDvi->ulClkPerVsyncDelta = ulAutoRepeat * 1 / 10000;    /*   0.01% */

	/* HD_DVI_0_BVB_CONVERT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_BVB_CONVERT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_BVB_CONVERT, DATA_IS_C_Y,   0        ) |
		BCHP_FIELD_DATA(HD_DVI_0_BVB_CONVERT, LAST_LINE_CNT, ulHeight );

#if (BVDC_P_SUPPORT_HDDVI_VER < BVDC_P_HDDVI_NEW_VER_7)
	/* HD_DVI_0_DMA_TRIG_CNTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DMA_TRIG_CNTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_DMA_TRIG_CNTRL, LINE_CNT_1, ulTrig1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_DMA_TRIG_CNTRL, LINE_CNT_0, ulTrig0 );

	/* HD_DVI_0_PCTR_CLEAN_CFG_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_2, MAX_PICTURE_PIXELS,
			pFmtInfo->ulDigitalWidth ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_2, MIN_PICTURE_PIXELS,
			pFmtInfo->ulDigitalWidth );

	/* HD_DVI_0_PCTR_CLEAN_CFG_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_3, MAX_PICTURE_LINES,
			ulHeight                                                  ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_3, MIN_PICTURE_LINES,
			ulHeight                                                  );
#else
	/* HD_DVI_0_PCTR_CLEAN_CFG_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_2, PICTURE_PIXELS,
			pFmtInfo->ulDigitalWidth                                  );

	/* HD_DVI_0_PCTR_CLEAN_CFG_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_3, PICTURE_LINES,
			ulHeight                                                  );

	if(pFmtInfo->eOrientation == BFMT_Orientation_e3D_OverUnder)
	{
		/* HD_DVI_0_ASR (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_ASR + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_ASR, TRIGGER_SWAP, 0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_ASR, SEPARATE,     0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_ASR, OFFSET, pFmtInfo->ulDigitalHeight >> pFmtInfo->bInterlaced) |
			BCHP_FIELD_DATA(HD_DVI_0_ASR, LENGTH,       ulActiveSpace );
	}
	else
	{
		/* HD_DVI_0_ASR (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_ASR + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_ASR, TRIGGER_SWAP, 0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_ASR, SEPARATE,     0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_ASR, OFFSET,       0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_ASR, LENGTH,       0 );
	}
#endif

	/* HD_DVI_0_PCTR_CLEAN_CFG_4 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_4 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_4, PAD_LINE_HBLANK_LENGTH,
			pFmtInfo->ulScanWidth - pFmtInfo->ulDigitalWidth              ) |
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_4, MAX_HORIZONTAL_LINE_LENGTH,
			pFmtInfo->ulScanWidth                                         );

	/* HD_DVI_0_AUTO_REPEAT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_AUTO_REPEAT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_AUTO_REPEAT, AUTO_REPEAT,
			ulAutoRepeat * BVDC_P_FIELD_PER_FRAME                         );

	/* HD_DVI_0_PICTURE_DELAY (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PICTURE_DELAY + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PICTURE_DELAY, PICTURE_DELAY, ulPicDelay );

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_4)
	/* HD_DVI_0_PCTR_CLEAN_CFG_6 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_6 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_6, PICTURE_TIMEOUT, ulAutoRepeat );
#endif

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildCscRul_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset;
	BVDC_P_CscCoeffs *pCsc;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;
	pCsc = &hHdDvi->stCsc;

	if(pCurInfo->bUserCsc)
	{
		BVDC_P_Csc_FromMatrix_isr(pCsc,
			pCurInfo->pl32_Matrix, pCurInfo->ulUserShift);
	}
	else
	{
		BVDC_P_Csc_GetHdDviTable_isr(pCsc, hHdDvi->stXvdField.eCscMode);
	}

	/* HD_DVI_0_V0_COEFF_C01_C00 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_V0_COEFF_C01_C00 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C01_C00, COEFF_C0, pCsc->usY0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C01_C00, COEFF_C1, pCsc->usY1 );

	/* HD_DVI_0_V0_COEFF_C03_C02 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_V0_COEFF_C03_C02 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C03_C02, COEFF_C2, pCsc->usY2 ) |
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C03_C02, COEFF_C3, pCsc->usYOffset );

	/* HD_DVI_0_V0_COEFF_C11_C10 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_V0_COEFF_C11_C10 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C11_C10, COEFF_C0, pCsc->usCb0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C11_C10, COEFF_C1, pCsc->usCb1 );

	/* HD_DVI_0_V0_COEFF_C13_C12 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_V0_COEFF_C13_C12 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C13_C12, COEFF_C2, pCsc->usCb2 ) |
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C13_C12, COEFF_C3, pCsc->usCbOffset );

	/* HD_DVI_0_V0_COEFF_C21_C20 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_V0_COEFF_C21_C20 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C21_C20, COEFF_C0, pCsc->usCr0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C21_C20, COEFF_C1, pCsc->usCr1 );

	/* HD_DVI_0_V0_COEFF_C23_C22 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_V0_COEFF_C23_C22 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C23_C22, COEFF_C2, pCsc->usCr2 ) |
		BCHP_FIELD_DATA(HD_DVI_0_V0_COEFF_C23_C22, COEFF_C3, pCsc->usCrOffset );

	return;
}

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_3)
/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildMuxRul_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	const uint8_t *pMuxTable;
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;

	/* Not use at the moment. */
	BSTD_UNUSED(pCurInfo);

#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
	pMuxTable = s_aaucMux[BVDC_P_HdDvi_InputColorSpace_eYCbCr422_DoubleClock+1];
#else
	pMuxTable = s_aaucMux[hHdDvi->eInputColorSpace];
#endif

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
	/* HD_DVI_0_INPUT_MUX_SELECT_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_0, MUX_DATA_3, pMuxTable[3]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_0, MUX_DATA_2, pMuxTable[2]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_0, MUX_DATA_1, pMuxTable[1]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_0, MUX_DATA_0, pMuxTable[0]);

	/* HD_DVI_0_INPUT_MUX_SELECT_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_1, MUX_DATA_7, pMuxTable[7]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_1, MUX_DATA_6, pMuxTable[6]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_1, MUX_DATA_5, pMuxTable[5]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_1, MUX_DATA_4, pMuxTable[4]);

	/* HD_DVI_0_INPUT_MUX_SELECT_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_2, MUX_DATA_11, pMuxTable[11]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_2, MUX_DATA_10, pMuxTable[10]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_2, MUX_DATA_9,  pMuxTable[9 ]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_2, MUX_DATA_8,  pMuxTable[8 ]);

	/* HD_DVI_0_INPUT_MUX_SELECT_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_3, MUX_DATA_15, pMuxTable[15]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_3, MUX_DATA_14, pMuxTable[14]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_3, MUX_DATA_13, pMuxTable[13]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_3, MUX_DATA_12, pMuxTable[12]);

	/* HD_DVI_0_INPUT_MUX_SELECT_4 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_4 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_4, MUX_DATA_19, pMuxTable[19]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_4, MUX_DATA_18, pMuxTable[18]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_4, MUX_DATA_17, pMuxTable[17]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_4, MUX_DATA_16, pMuxTable[16]);

	/* HD_DVI_0_INPUT_MUX_SELECT_5 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_5 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_5, MUX_DATA_23, pMuxTable[23]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_5, MUX_DATA_22, pMuxTable[22]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_5, MUX_DATA_21, pMuxTable[21]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_5, MUX_DATA_20, pMuxTable[20]);

	/* HD_DVI_0_INPUT_MUX_SELECT_6 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_6 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_6, MUX_DATA_27, pMuxTable[27]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_6, MUX_DATA_26, pMuxTable[26]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_6, MUX_DATA_25, pMuxTable[25]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_6, MUX_DATA_24, pMuxTable[24]);

	/* HD_DVI_0_INPUT_MUX_SELECT_7 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_7 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_7, MUX_DATA_31, pMuxTable[31]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_7, MUX_DATA_30, pMuxTable[30]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_7, MUX_DATA_29, pMuxTable[29]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_7, MUX_DATA_28, pMuxTable[28]);

	/* HD_DVI_0_INPUT_MUX_SELECT_8 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_8 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_8, MUX_DATA_35, pMuxTable[35]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_8, MUX_DATA_34, pMuxTable[34]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_8, MUX_DATA_33, pMuxTable[33]) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_8, MUX_DATA_32, pMuxTable[32]);

	/* HD_DVI_0_INPUT_MUX_SELECT_9 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_MUX_SELECT_9 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_9, MUX_VSYNC, 0x26 ) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_9, MUX_HSYNC, 0x25 ) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_MUX_SELECT_9, MUX_DE,    0x24 );

#else
	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_0, MUX_DATA_3, pMuxTable[3]) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_0, MUX_DATA_2, pMuxTable[2]) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_0, MUX_DATA_1, pMuxTable[1]) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_0, MUX_DATA_0, pMuxTable[0]);

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_1, MUX_DATA_7, pMuxTable[7] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_1, MUX_DATA_6, pMuxTable[6] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_1, MUX_DATA_5, pMuxTable[5] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_1, MUX_DATA_4, pMuxTable[4] );

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_2, MUX_DATA_11, pMuxTable[11] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_2, MUX_DATA_10, pMuxTable[10] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_2, MUX_DATA_9,  pMuxTable[ 9] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_2, MUX_DATA_8,  pMuxTable[ 8] );

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_3, MUX_DATA_15, pMuxTable[15] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_3, MUX_DATA_14, pMuxTable[14] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_3, MUX_DATA_13, pMuxTable[13] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_3, MUX_DATA_12, pMuxTable[12] );

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_4 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_4 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_4, MUX_DATA_19, pMuxTable[19] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_4, MUX_DATA_18, pMuxTable[18] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_4, MUX_DATA_17, pMuxTable[17] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_4, MUX_DATA_16, pMuxTable[16] );

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_5 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_5 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_5, MUX_DATA_23, pMuxTable[23] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_5, MUX_DATA_22, pMuxTable[22] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_5, MUX_DATA_21, pMuxTable[21] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_5, MUX_DATA_20, pMuxTable[20] );

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_6 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_6 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_6, MUX_DATA_27, pMuxTable[27] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_6, MUX_DATA_26, pMuxTable[26] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_6, MUX_DATA_25, pMuxTable[25] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_6, MUX_DATA_24, pMuxTable[24] );

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_7 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_7 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_7, MUX_DATA_31, pMuxTable[31] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_7, MUX_DATA_30, pMuxTable[30] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_7, MUX_DATA_29, pMuxTable[29] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_7, MUX_DATA_28, pMuxTable[28] );

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_8 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_8 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_8, MUX_DATA_35, pMuxTable[35] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_8, MUX_DATA_34, pMuxTable[34] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_8, MUX_DATA_33, pMuxTable[33] ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_8, MUX_DATA_32, pMuxTable[32] );

	/* HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_9 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_9 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_9, MUX_VSYNC, 0x26 ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_9, MUX_HSYNC, 0x25 ) |
		BCHP_FIELD_DATA(HD_DVI_0_HD_DVI_INPUT_MUX_SELECT_9, MUX_DE,    0x24 );
#endif

	return;
}
#endif

/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildControlRul_Isr
	( const BVDC_P_HdDvi_Handle        hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE != BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
	const BVDC_HdDvi_Settings *pSettings = &pCurInfo->stHdDviSetting;
#endif
	uint32_t ulAutoClkRate = (pCurInfo->pFmtInfo->ulPxlFreq * 16/216)/BFMT_FREQ_FACTOR;
	uint32_t ulOffset, ulDigitalHeight;
	uint32_t ulInputPort = pCurInfo->ulInputPort;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_8)
	if(ulInputPort)
	{
		ulInputPort = (BVDC_P_HdDviId_eHdDvi0 == hHdDvi->eId)
			? BCHP_HD_DVI_0_INPUT_CNTRL_SELECT_INPUT_HDMI_0
			: BCHP_HD_DVI_0_INPUT_CNTRL_SELECT_INPUT_HDMI_1;
	}
#endif

#if ((BVDC_P_SUPPORT_HDDVI > 1) && \
	 (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_1))  /* 2 new hddvi's */
	/* For external HD_DVI (24 bit or 30 bit mode), both HD_DVI core are
	 * combined together. HD_DVI_x provides data path and control, the other
	 * HD_DVI core provides only data path.
	 * Need to program both core */
	if((BVDC_HdDviInput_Ext == pCurInfo->ulInputPort) &&
	   (BVDC_HdDvi_InputDataMode_e36Bit != pSettings->eInputDataMode))
	{
		uint32_t ulCtrlOrMask;
		uint32_t ulCtrlAndMask;
		uint32_t ulOtherCoreOffset;
		BVDC_P_HdDviId eOtherCoreId;

		eOtherCoreId = (hHdDvi->eId == BVDC_P_HdDviId_eHdDvi0)
			? BVDC_P_HdDviId_eHdDvi1 : BVDC_P_HdDviId_eHdDvi0;
		ulOtherCoreOffset = BVDC_P_HDDVI_GET_REG_OFFSET(eOtherCoreId,
			BCHP_HD_DVI_0_REG_START, BCHP_HD_DVI_1_REG_START);

		/* Update other but preserved other bits that important to other core.
		 *    HD_DVI_0_INPUT_CNTRL, USE_RGB_TO_YCRCB
		 *    HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_MSW
		 *    HD_DVI_0_INPUT_CNTRL, USE_RNG_SUPPRESSION
		 *    HD_DVI_0_INPUT_CNTRL, FILTER_MODE_444_TO_422 */
		ulCtrlAndMask = ~(
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, RGB_CONVERT_MODE             ) |
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, DOUBLE_PUMPED_24             ) |
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, SELECT_DE                    ) |
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, SELECT_VSYNC                 ) |
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, SELECT_HSYNC                 ) |
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_MSW              ) |
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_LSW              ) |
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, USE_PORT_0_CLOCKS_FOR_PORT_1 ) |
			BCHP_MASK(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_LSW             ));

		ulCtrlOrMask  =  (
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
			BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, RGB_CONVERT_MODE,             1                            ) |
#else
			BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, RGB_CONVERT_MODE,             3                            ) |
#endif
			BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, DOUBLE_PUMPED_24,             SINGLE_PUMPED                ) |
			BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DE,                    lsw_of_port_0_at_stage_a2    ) |
			BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_VSYNC,                 lsw_of_port_0_at_stage_a2    ) |
			BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_HSYNC,                 lsw_of_port_0_at_stage_a2    ) |
			BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_MSW,              lsw_of_port_1_at_stage_a2    ) |
			BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_LSW,              lsw_of_port_0_at_stage_a2    ) |
			BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_PORT_0_CLOCKS_FOR_PORT_1, 1                            ) |
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
			BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_LSW,             rising_edge_of_HD_DVI0_CLK_N ));
#else
			BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_LSW,             rising_edge_of_HD_DVI0_CLK_P ));
#endif

		/* HD_DVI_1_INPUT_CNTRL (RW) */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ulCtrlAndMask, ulCtrlOrMask,
			BCHP_HD_DVI_0_INPUT_CNTRL + ulOtherCoreOffset);

		/* HD_DVI_0_DESKEW_CLK_MSW (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DESKEW_CLK_MSW + ulOtherCoreOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_SELECTION_CLK_MSW, 2  ) |
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_FOUR_CTL_CLK_MSW,  0  ) |
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_THREE_CTL_CLK_MSW, 0  ) |
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_TWO_CTL_CLK_MSW,   3  ) |
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_MSW, DELAY_ONE_CTL_CLK_MSW,   15 );

		/* HD_DVI_0_DESKEW_CLK_LSW (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DESKEW_CLK_LSW + ulOtherCoreOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_SELECTION_CLK_LSW, 2  ) |
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_FOUR_CTL_CLK_LSW,  0  ) |
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_THREE_CTL_CLK_LSW, 0  ) |
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_TWO_CTL_CLK_LSW,   3  ) |
			BCHP_FIELD_DATA(HD_DVI_0_DESKEW_CLK_LSW, DELAY_ONE_CTL_CLK_LSW,   15 );
	}
#endif

	/* HD_DVI_0_INPUT_CNTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_INPUT_CNTRL + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_4)
#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_444_TO_422,               1                            ) |
#else
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_444_TO_422,
			((BVDC_P_HDDVI_INPUT_444(hHdDvi->eInputColorSpace) ||
			  BVDC_P_HDDVI_INPUT_RGB(hHdDvi->eInputColorSpace)) && (!BVDC_P_SUPPORT_BVN_444))            ) |
#endif
		((BVDC_RingSuppressionMode_eDisable == hHdDvi->stDnSampler.eRingRemoval)
		? BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_RNG_SUPPRESSION,        0  )
		: BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_RNG_SUPPRESSION,        1  )                         ) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, FILTER_MODE_444_TO_422, hHdDvi->stDnSampler.eFilterType    ) |
#endif

#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK)
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_RGB_TO_YCRCB,             1                            ) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, RGB_CONVERT_MODE,             1                            ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DE,                    msw_of_port_0_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_VSYNC,                 msw_of_port_0_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_HSYNC,                 msw_of_port_0_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_MSW,              lsw_of_port_0_at_stage_a2    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_LSW,              msw_of_port_0_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_MSW,             rising_edge_of_HD_DVI0_CLK_P ) |
#elif (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_RGB_TO_YCRCB,
			BVDC_P_HDDVI_INPUT_RGB(hHdDvi->eInputColorSpace) || pCurInfo->bUserCsc                       ) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, RGB_CONVERT_MODE,             3                            ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DE,                    lsw_of_port_0_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_VSYNC,                 lsw_of_port_0_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_HSYNC,                 lsw_of_port_0_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_MSW,              lsw_of_port_1_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_LSW,              lsw_of_port_0_at_stage_a1    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_MSW,             rising_edge_of_HD_DVI0_CLK_P ) |
#else
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_RGB_TO_YCRCB,
			BVDC_P_HDDVI_INPUT_RGB(hHdDvi->eInputColorSpace) || pCurInfo->bUserCsc                       ) |
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, RGB_CONVERT_MODE,             3                            ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DE,                    lsw_of_port_0_at_stage_a2    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_VSYNC,                 lsw_of_port_0_at_stage_a2    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_HSYNC,                 lsw_of_port_0_at_stage_a2    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_MSW,              lsw_of_port_1_at_stage_a2    ) |
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_DATA_LSW,              lsw_of_port_0_at_stage_a2    ) |
		((BVDC_HdDvi_InputDataMode_e36Bit == pSettings->eInputDataMode)
		? BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_MSW,         rising_edge_of_HD_DVI0_CLK_N )
		: BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_MSW,         rising_edge_of_HD_DVI0_CLK_P ) ) |
#endif
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, DOUBLE_PUMPED_24,             SINGLE_PUMPED                ) |
#if (BVDC_P_SUPPORT_HDDVI_VER < BVDC_P_HDDVI_NEW_VER_7)
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, SELECT_AUX_INPUT,             ulInputPort                  ) |
#else
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, SELECT_INPUT,                 ulInputPort                  ) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_INPUT_CNTRL, USE_PORT_0_CLOCKS_FOR_PORT_1, 1                            ) |

#if (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP)
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_LSW,           rising_edge_of_HD_DVI0_CLK_N   );
#else
		BCHP_FIELD_ENUM(HD_DVI_0_INPUT_CNTRL, SELECT_CLOCK_LSW,           rising_edge_of_HD_DVI0_CLK_P   );
#endif

	/* HD_DVI_0_CHANNEL_CNTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_CHANNEL_CNTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_CHANNEL_CNTRL, SPARE_31_17,           0                       ) |
#if (BVDC_P_SUPPORT_HDDVI_VER == BVDC_P_HDDVI_NEW_VER_2) /* Fix #12, chroma rollover in BVN */
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, OUTPUT_CH_1_CLIP_MODE, Clip_at_01_to_255       ) |
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, OUTPUT_CH_2_CLIP_MODE, Clip_at_01_to_255       ) |
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, OUTPUT_CH_3_CLIP_MODE, Clip_at_01_to_255       ) |
#else
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, OUTPUT_CH_1_CLIP_MODE, No_Clipping             ) |
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, OUTPUT_CH_2_CLIP_MODE, No_Clipping             ) |
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, OUTPUT_CH_3_CLIP_MODE, No_Clipping             ) |
#endif

#if (BVDC_P_SUPPORT_HDDVI_VER < BVDC_P_HDDVI_NEW_VER_7)
		((BVDC_HdDvi_InputDataMode_e24Bit == pSettings->eInputDataMode)
		? BCHP_FIELD_DATA(HD_DVI_0_CHANNEL_CNTRL, USE_30_BIT_MODE,     0 )
		: BCHP_FIELD_DATA(HD_DVI_0_CHANNEL_CNTRL, USE_30_BIT_MODE,     1 )                     ) |
#endif

#if (BVDC_P_SUPPORT_HDDVI_VER == BVDC_P_HDDVI_NEW_VER_2)
		BCHP_FIELD_DATA(HD_DVI_0_CHANNEL_CNTRL, ROTATE,                0                       ) |
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, ZERO_FILL,             twelve_bit_color_source ) |
#else

#if ((BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_LOOPBACK) || \
	 (BVDC_P_HDDVI_EXT_INPUT_TYPE == BVDC_P_HDDVI_EXT_INPUT_TYPE_NXP))
		BCHP_FIELD_DATA(HD_DVI_0_CHANNEL_CNTRL, MODESEL,               1                       ) |
#else
		BCHP_FIELD_DATA(HD_DVI_0_CHANNEL_CNTRL, MODESEL,               pCurInfo->ulInputPort   ) |
#endif

#if (BVDC_P_SUPPORT_HDDVI_VER < BVDC_P_HDDVI_NEW_VER_7)
		BCHP_FIELD_DATA(HD_DVI_0_CHANNEL_CNTRL, USE_DDR12_MODE,        0                       ) |
#endif
#endif
		BCHP_FIELD_DATA(HD_DVI_0_CHANNEL_CNTRL, CHANNEL_MAP_SELECT,    hHdDvi->eInputColorSpace) |
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, CHANNEL_1_CLIP_MODE,   No_Clipping             ) |
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, CHANNEL_2_CLIP_MODE,   No_Clipping             ) |
		BCHP_FIELD_ENUM(HD_DVI_0_CHANNEL_CNTRL, CHANNEL_3_CLIP_MODE,   No_Clipping             );

	/* HD_DVI_0_US_422_TO_444_CONV (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_US_422_TO_444_CONV + ulOffset);
	if((hHdDvi->stUpSampler.eFilterType == BVDC_422To444Filter_eTenTaps) ||
	   (hHdDvi->stUpSampler.eFilterType == BVDC_422To444Filter_eSixTaps))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(HD_DVI_0_US_422_TO_444_CONV, RING_SUPPION_MODE, DOUBLE) |
			BCHP_FIELD_ENUM(HD_DVI_0_US_422_TO_444_CONV, RING_SUPPION,      ENABLE) |
			BCHP_FIELD_DATA(HD_DVI_0_US_422_TO_444_CONV, UNBIASED_ROUND_ENABLE,
				hHdDvi->stUpSampler.bUnbiasedRound) |
			BCHP_FIELD_DATA(HD_DVI_0_US_422_TO_444_CONV, FILT_CTRL, hHdDvi->stUpSampler.eFilterType);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(HD_DVI_0_US_422_TO_444_CONV, RING_SUPPION_MODE, NORMAL)  |
			BCHP_FIELD_ENUM(HD_DVI_0_US_422_TO_444_CONV, RING_SUPPION,      DISABLE) |
			BCHP_FIELD_DATA(HD_DVI_0_US_422_TO_444_CONV, UNBIASED_ROUND_ENABLE,
				hHdDvi->stUpSampler.bUnbiasedRound) |
			BCHP_FIELD_DATA(HD_DVI_0_US_422_TO_444_CONV, FILT_CTRL, hHdDvi->stUpSampler.eFilterType);
	}

	/* HD_DVI_0_PORT_CONFIG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PORT_CONFIG + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
		BCHP_FIELD_DATA(HD_DVI_0_PORT_CONFIG, EXTEND_TIMEOUT,     0 ) |
#endif
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_8)
		BCHP_FIELD_DATA(HD_DVI_0_PORT_CONFIG, FD_STAGE_SEL,       1 ) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_PORT_CONFIG, AUTO_CLK_RATE,      ulAutoClkRate ) |
		BCHP_FIELD_DATA(HD_DVI_0_PORT_CONFIG, USE_EMBEDDED_SYNCS, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_PORT_CONFIG, USE_10_BIT_CODES,   0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_PORT_CONFIG, ITU_SYNC_TYPE,      1 ) |
		BCHP_FIELD_DATA(HD_DVI_0_PORT_CONFIG, PIXEL_DECIMATION,
			hHdDvi->ulPixelDecimate                                 );

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_7)
	/* HD_DVI_0_PIXEL_DECIMATION (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PIXEL_DECIMATION + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_PIXEL_DECIMATION, UNDECIMATED_LINE_PIXELS, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_PIXEL_DECIMATION, PIXEL_SELECT,            0 );
#endif

	ulDigitalHeight = pCurInfo->pFmtInfo->ulDigitalHeight >> pCurInfo->pFmtInfo->bInterlaced;
	if(pCurInfo->pFmtInfo->eOrientation == BFMT_Orientation_e3D_OverUnder)
	{
		ulDigitalHeight = 2 * ulDigitalHeight + pCurInfo->pFmtInfo->ulActiveSpace;
	}

	/* HD_DVI_0_DE_GEN_CONFIG1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DE_GEN_CONFIG1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG1, VSYNC_POLARITY, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG1, HSYNC_POLARITY, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG1, HOR_PIXELS,
			(pCurInfo->pFmtInfo->ulDigitalWidth * (hHdDvi->ulPixelDecimate + 1))   ) |
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG1, VER_LINES, ulDigitalHeight);

#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_3)
	/* HD_DVI_0_DE_GEN_CONFIG3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DE_GEN_CONFIG3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG3, AUTO_POL_SELECT, 0   );
#endif

	/* SW7425-31: TODO: Adding hw mute support with new input */
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_6)
	/* HD_DVI_0_FORMAT_THRESHOLD_CFG_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_FORMAT_THRESHOLD_CFG_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_FORMAT_THRESHOLD_CFG_1, MIN_VSYNC_CNT, 0 );

	/* HD_DVI_0_FORMAT_THRESHOLD_CFG_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_FORMAT_THRESHOLD_CFG_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_FORMAT_THRESHOLD_CFG_2, MAX_VSYNC_CNT, 0 );

	/* HD_DVI_0_FORMAT_THRESHOLD_CFG_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_FORMAT_THRESHOLD_CFG_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_FORMAT_THRESHOLD_CFG_3, MIN_HSYNC_CNT, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_FORMAT_THRESHOLD_CFG_3, MAX_HSYNC_CNT, 0 );

	/* HD_DVI_0_FORMAT_THRESHOLD_CFG_4 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_FORMAT_THRESHOLD_CFG_4 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_FORMAT_THRESHOLD_CFG_4, MIN_VAL_CNT, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_FORMAT_THRESHOLD_CFG_4, MAX_VAL_CNT, 0 );

	/* HD_DVI_0_FORMAT_THRESHOLD_CFG_5 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_FORMAT_THRESHOLD_CFG_5 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_FORMAT_THRESHOLD_CFG_5, MIN_HAP_CNT, 0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_FORMAT_THRESHOLD_CFG_5, MAX_HAP_CNT, 0 );

	/* HD_DVI_0_MUTE_CFG_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_MUTE_CFG_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HD_DVI_0_MUTE_CFG_1, MUTE_ENABLE,     0 ) |
		BCHP_FIELD_DATA(HD_DVI_0_MUTE_CFG_1, MUTE_PIXEL_DATA, 0 );

	/* HD_DVI_0_MUTE_CFG_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_MUTE_CFG_2 + ulOffset);
	*pList->pulCurrent++ = 0xffffffff;

	/* HD_DVI_0_MUTE_CFG_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_MUTE_CFG_3 + ulOffset);
	*pList->pulCurrent++ = 0xffffffff;
#endif

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildHVStartRul_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	bool bEnableDe, bValidUserHVStart = false;
	uint32_t ulHStart, ulVStart, ulVStart2;
	const BVDC_HdDvi_Settings *pSettings = &pCurInfo->stHdDviSetting;
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;

	/* Enable DE (Data Enable); if external and user enable. */
	bEnableDe = (
		(pSettings->bEnableDe)/* &&
		(BVDC_HdDviInput_Ext == pCurInfo->ulInputPort)*/);

	/* Check user HVStart settings */
	if(pCurInfo->bHVStartOverride)
	{
		uint32_t   ulHStart, ulVStart;
		uint32_t   ulHFP, ulHBP, ulVFP, ulVBP;
		int32_t   ulHMin, ulHMax, ulVMin, ulVMax, ulTemp;

		ulHStart = hHdDvi->ulHorzDelay;
		ulVStart = hHdDvi->ulVertDelay;
		ulHFP    = hHdDvi->stStatus.ulHFrontPorch;
		ulHBP    = hHdDvi->stStatus.ulHBackPorch;
		ulVFP    = hHdDvi->stStatus.ulVFrontPorch;
		ulVBP    = hHdDvi->stStatus.ulVBackPorch;

		ulHMin = ulHStart - ulHBP + 1;
		ulHMax = ulHStart + ulHFP - 1;
		ulVMin = ulVStart - ulVBP + 1;
		ulVMax = ulVStart + ulVFP - 1;

		/* ulHBP, ulHFP can be 0 with no signal */
		if(ulHMin > ulHMax)
		{
			ulTemp = ulHMin;
			ulHMin = ulHMax;
			ulHMax = ulTemp;
		}

		/* ulVBP, ulVFP can be 0 with no signal */
		if(ulVMin > ulVMax)
		{
			ulTemp = ulVMin;
			ulVMin = ulVMax;
			ulVMax = ulTemp;
		}

		if((((int32_t)(pCurInfo->ulHstart) < ulHMin) ||
		    ((int32_t)(pCurInfo->ulHstart) > ulHMax) ||
		    ((int32_t)(pCurInfo->ulVstart) < ulVMin) ||
		    ((int32_t)(pCurInfo->ulVstart) > ulVMax))
#if BFMT_LEGACY_3DTV_SUPPORT
			&& BFMT_IS_3D_MODE(pCurInfo->pFmtInfo->eVideoFmt)
#endif
			)
		{
			BDBG_WRN(("Invalidate HVStart settings"));
			bValidUserHVStart = false;
		}
		else
		{
			bValidUserHVStart = true;
		}
	}

	/* position adjustment */
	if(bValidUserHVStart)
	{
		ulHStart = pCurInfo->ulHstart;
		ulVStart = pCurInfo->ulVstart;
	}
	else /* restore the norminal values */
	{
		ulHStart = hHdDvi->ulHorzDelay;
		ulVStart = hHdDvi->ulVertDelay;
	}

	if(pCurInfo->pFmtInfo->bInterlaced)
	{
		ulVStart2 = ulVStart + 1;
	}
	else
	{
		ulVStart2 = ulVStart;
	}

	/*
	BDBG_MSG(("\t+++ bValidUserHVStart=%d, bEnableDE=%d, ulVStart=%d, bHVStartOverride=%d",
		bValidUserHVStart, bEnableDe, ulVStart, pCurInfo->bHVStartOverride));
	*/

	/* HD_DVI_0_DE_GEN_CONFIG2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DE_GEN_CONFIG2 + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_HDDVI_VER < BVDC_P_HDDVI_NEW_VER_7)
#if (BVDC_P_HDDVI_SW_RESET)
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG2, DE_SELECT,  bEnableDe && bValidUserHVStart && hHdDvi->bVideoDetected) |
#else
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG2, DE_SELECT,  bEnableDe && bValidUserHVStart) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG2, HOR_DELAY,  ulHStart ) |
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG2, AUTO_POL_DETECT, 1   ) |
#endif
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG2, VER_DELAY1, ulVStart ) |
		BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG2, VER_DELAY2, ulVStart2);

#if BFMT_LEGACY_3DTV_SUPPORT
	if(BFMT_IS_3D_MODE(pCurInfo->pFmtInfo->eVideoFmt) &&
	   bEnableDe && bValidUserHVStart)
	{
		/* HD_DVI_0_DE_GEN_CONFIG1 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_DE_GEN_CONFIG1 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG1, VSYNC_POLARITY, 0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG1, HSYNC_POLARITY, 0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG1, HOR_PIXELS,
				(pCurInfo->pFmtInfo->ulDigitalWidth * (hHdDvi->ulPixelDecimate + 1))   ) |
			BCHP_FIELD_DATA(HD_DVI_0_DE_GEN_CONFIG1, VER_LINES,
				(pCurInfo->pFmtInfo->ulDigitalHeight - pCurInfo->ulVstart + pCurInfo->pFmtInfo->ulTopActive - 1));
	}
#endif
	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildVsyncRul_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	ulOffset = hHdDvi->ulOffset;

	BSTD_UNUSED(pCurInfo);

	/* Read and store error flags. Need to read before HOST_ENABLE is set */
	*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_BRIDGE_ERRORS_RDB_CLR + ulOffset);
	*pList->pulCurrent++ = BRDC_REGISTER(hHdDvi->ulBridgeErrRegAddr);

	*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_ERROR_STATUS + ulOffset);
	*pList->pulCurrent++ = BRDC_REGISTER(hHdDvi->ulPctrErrRegAddr);

	/* clear error flags. Need before HOST_ENABLE is set */
	if(hHdDvi->bPctrErr)
	{
		/* CLEAR_ERRORS = 0 */
		hHdDvi->ulPicCleanUpCfg1 &= ~(
			BCHP_MASK(HD_DVI_0_PCTR_CLEAN_CFG_1, CLEAR_ERRORS));
		hHdDvi->ulPicCleanUpCfg1 |=  (
			BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, CLEAR_ERRORS, 0));

		/* HD_DVI_0_PCTR_CLEAN_CFG_1 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_1 + ulOffset);
		*pList->pulCurrent++ = hHdDvi->ulPicCleanUpCfg1;

		/* CLEAR_ERRORS = 1 */
		hHdDvi->ulPicCleanUpCfg1 &= ~(
			BCHP_MASK(HD_DVI_0_PCTR_CLEAN_CFG_1, CLEAR_ERRORS));
		hHdDvi->ulPicCleanUpCfg1 |=  (
			BCHP_FIELD_DATA(HD_DVI_0_PCTR_CLEAN_CFG_1, CLEAR_ERRORS, 1));

		/* HD_DVI_0_PCTR_CLEAN_CFG_1 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_PCTR_CLEAN_CFG_1 + ulOffset);
		*pList->pulCurrent++ = hHdDvi->ulPicCleanUpCfg1;
	}

	if(hHdDvi->bBridgeErr)
	{
		/* HD_DVI_0_HOST_CONTROL_BITS (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HOST_CONTROL_BITS + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, HOST_DEBUG,   0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, HOST_DISABLE, 0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, CLEAR_ERRORS, 1 );

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HOST_CONTROL_BITS + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, HOST_DEBUG,   0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, HOST_DISABLE, 0 ) |
			BCHP_FIELD_DATA(HD_DVI_0_HOST_CONTROL_BITS, CLEAR_ERRORS, 0 );
	}

	/* This register is a level trigger. (Toggle to START_FEED). */
	/* HD_DVI_0_HOST_ENABLE (RW) */
	if(hHdDvi->bVideoDetected)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HOST_ENABLE + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_HOST_ENABLE, HOST_ENABLE, 1 );

		/* HD_DVI_0_HOST_ENABLE (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HD_DVI_0_HOST_ENABLE + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_DVI_0_HOST_ENABLE, HOST_ENABLE, 0 );
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_UpdateFramerate_isr
	( const BVDC_P_HdDvi_Handle        hHdDvi,
	  BVDC_Source_Handle               hSource,
	  const BVDC_P_Source_Info        *pCurInfo,
	  BAVC_FrameRateCode              *peFrameRate )
{
	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	hSource->ulVertFreq    = hHdDvi->stStatus.ulVertFreq;
	hSource->ulClkPerVsync = hHdDvi->stStatus.ulClkPerVsync;

	/* Frequency is out of bound to be meaningful; use standard vertical
	 * frequency instead.  See also BVDC_P_ROUND_OFF() */
	if(!BVDC_P_EQ_DELTA(hSource->ulVertFreq,
	    pCurInfo->pFmtInfo->ulVertFreq, (BFMT_FREQ_FACTOR/2)-1))
	{
		hSource->ulVertFreq = pCurInfo->pFmtInfo->ulVertFreq;
	}

	BVDC_P_Source_UpdateFrameRate_isr(pCurInfo->pFmtInfo, hSource->ulClkPerVsync,
		hHdDvi->ulClkPerVsyncDelta, peFrameRate);

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_UpdateAutoFormat_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  const BVDC_Source_Handle         hSource,
	  const BFMT_VideoInfo           **ppNewFmtInfo,
	  uint32_t                        *pulPixelDecimate,
	  bool                            *pbVideoDetected )
{
	uint32_t ulStatusScanWidth, ulStatusScanHeight, i;
	uint32_t ulDigitalHeight;
	const BFMT_VideoInfo *pFmtInfo;
	const BVDC_P_HdDviInput *pStatus;
	const BVDC_P_Source_Info *pCurInfo;
	const BVDC_HdDvi_Settings *pSettings;
	const BVDC_HdDvi_FormatTolerance *pTolerance;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pStatus = &hHdDvi->stStatus;
	pCurInfo = &hSource->stCurInfo;
	pSettings = &pCurInfo->stHdDviSetting;
	pTolerance = &pSettings->stFmtTolerence;

	/* Initialized */
	*pbVideoDetected   = false;
	ulStatusScanWidth  = pStatus->ulHBlank + pStatus->ulAvWidth;
	ulStatusScanHeight = pStatus->ulVBlank + pStatus->ulAvHeight;

	/* FORMAT DETECTION: status */
	for(i = 0; i < BFMT_VideoFmt_eCustom0; i++)
	{
		/* TODO: Refactor this portion of the code with videodetect */
		uint32_t ulTolWidth, ulTolHeight;
		pFmtInfo = BFMT_GetVideoFormatInfoPtr((BFMT_VideoFmt)i);

		ulDigitalHeight = pFmtInfo->ulDigitalHeight;
		if(pFmtInfo->eOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			ulDigitalHeight = 2 * ulDigitalHeight + pFmtInfo->ulActiveSpace;
		}

		/* Here, when we're in DVI mode we assume that thing will get decimates
		 * if there are preference for doing non decimate we'll might need to
		 * open up that option.  In the case of HDMI mode the AviInfoFrame
		 * indicate the option.  Default decimate. */
		if((!hHdDvi->stXvdField.bHdmiMode) &&
		   ((BFMT_VideoFmt_eCUSTOM_1440x240p_60Hz == pFmtInfo->eVideoFmt) ||
		    (BFMT_VideoFmt_eCUSTOM_1440x288p_50Hz == pFmtInfo->eVideoFmt) ||
		    (BFMT_VideoFmt_e1440x480p_60Hz == pFmtInfo->eVideoFmt) ||
		    (BFMT_VideoFmt_e1440x576p_50Hz == pFmtInfo->eVideoFmt)))
		{
			continue;
		}

		/* PR52568: special 1090i */
		if(BFMT_VideoFmt_e1080i == pFmtInfo->eVideoFmt)
		{
			ulTolWidth  = pTolerance->ulWidth;
			ulTolHeight = BVDC_P_MAX(pTolerance->ulHeight, BVDC_P_HDDVI_1090i_MIN_TOLER_HEIGHT);
		}
		else if (BFMT_VideoFmt_e720p == pFmtInfo->eVideoFmt)
		{
			ulTolWidth  = BVDC_P_MAX(pTolerance->ulWidth,  BVDC_P_HDDVI_725p_MIN_TOLER_WIDTH);
			ulTolHeight = BVDC_P_MAX(pTolerance->ulHeight, BVDC_P_HDDVI_725p_MIN_TOLER_HEIGHT);
		}
		else
		{
			ulTolWidth  = pTolerance->ulWidth;
			ulTolHeight = pTolerance->ulHeight;
		}

		/* Check to see if we have a match? */
		if((pFmtInfo->bInterlaced == pStatus->bInterlaced) &&
		   (BVDC_P_EQ_DELTA(pStatus->ulAvWidth, pFmtInfo->ulDigitalWidth, ulTolWidth)) &&
		   (BVDC_P_EQ_DELTA(pStatus->ulAvHeight, ulDigitalHeight, ulTolHeight)) &&
		   (BVDC_P_EQ_DELTA(pStatus->ulVertFreq, pFmtInfo->ulVertFreq, hHdDvi->ulVertFreqTolerance)))
		{
#if (!BVDC_P_HDDVI_TEST_NEW_FORMAT)
			if((BFMT_VideoFmt_e1080p == pFmtInfo->eVideoFmt) ||
			   (BFMT_VideoFmt_eDVI_1400x1050p_60Hz == pFmtInfo->eVideoFmt) ||
			   (BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red == pFmtInfo->eVideoFmt) ||
			   (BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red == pFmtInfo->eVideoFmt))
			{
				if(BVDC_P_EQ_DELTA(ulStatusScanWidth, pFmtInfo->ulScanWidth, hHdDvi->ulScanWidthTolerance))
				{
					*pbVideoDetected  = true;
					*ppNewFmtInfo = pFmtInfo;
					break;
				}
			}
			else
#endif
			{
				*pbVideoDetected  = true;
				*ppNewFmtInfo = pFmtInfo;
				break;
			}

			/* See what it would be */
#if (BVDC_P_HDDVI_TEST_NEW_FORMAT)
			if((ulStatusScanWidth != pFmtInfo->ulScanWidth) ||
			   (ulStatusScanHeight != pFmtInfo->ulScanHeight))
			{
				BDBG_ERR(("V/H Blank status vs nominal:H: %d vs %d, V: %d vs %d, %s",
					ulStatusScanWidth, pFmtInfo->ulScanWidth,
					ulStatusScanHeight, pFmtInfo->ulScanHeight,
					pFmtInfo->pchFormatStr));
			}
#endif
		}
		else if((pStatus->ulAvWidth > pFmtInfo->ulDigitalWidth) &&
		        (pStatus->bInterlaced == pFmtInfo->bInterlaced) &&
		        (pStatus->ulAvHeight == ulDigitalHeight) &&
		        (!(pStatus->ulAvWidth % pFmtInfo->ulDigitalWidth)))
		{
			*pulPixelDecimate = (pStatus->ulAvWidth / pFmtInfo->ulDigitalWidth);
			if(*pulPixelDecimate)
			{
				*pulPixelDecimate -= 1;
			}
		}
#if BFMT_LEGACY_3DTV_SUPPORT
		else if((BFMT_IS_3D_MODE(pFmtInfo->eVideoFmt) &&
		         pCurInfo->bHVStartOverride && pCurInfo->stHdDviSetting.bEnableDe) &&
		        (BVDC_P_EQ_DELTA(pStatus->ulAvWidth, pFmtInfo->ulDigitalWidth, ulTolWidth)) &&
		        (BVDC_P_EQ_DELTA(pStatus->ulAvHeight, ulDigitalHeight - pCurInfo->ulVstart + pFmtInfo->ulTopActive - 1, ulTolHeight)) &&
		        (BVDC_P_EQ_DELTA(pStatus->ulVertFreq, pFmtInfo->ulVertFreq, hHdDvi->ulVertFreqTolerance)))
		{
			*pbVideoDetected  = true;
		}
#endif
		else if (hHdDvi->ulPixelDecimate)
		{
			/* Reset the decimation value. */
			if(pFmtInfo->ulDigitalWidth == (pStatus->ulAvWidth << hHdDvi->ulPixelDecimate))
			{
				*pulPixelDecimate = 0;
			}
		}
	}

	/* If not found we may need to mute the video and program the correct
	 * BVB_CONVERT and/or the PIXEL_DECIMATION */

	return;
}


#if (BVFD_P_HDDVI_TRIGGER_WORKAROUND)
/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_DetectTrigger_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  const BFMT_VideoInfo            *pFmtInfo,
	  int                              iFieldId )
{
	uint32_t ulReg;
	uint32_t ulField0, ulField1;
	bool bField0, bField1;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);

	/* Count */
	hHdDvi->ulDetectTrig++;

	/* Output status */
	ulReg = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_8 + hHdDvi->ulOffset);
	ulField0 = BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_8, UUT_VAL0);

	ulReg = BREG_Read32_isr(hHdDvi->hReg, BCHP_HD_DVI_0_VID_FORMAT_5 + hHdDvi->ulOffset);
	ulField1 = BCHP_GET_FIELD_DATA(ulReg, HD_DVI_0_VID_FORMAT_5, UUT_VAL1);

	BDBG_MSG(("[%2d] id=%d, f0=%4d, f1=%4d", hHdDvi->ulDetectTrig,
		iFieldId, ulField0, ulField1));

	/* Any field reach stable stage */
	bField0 = BVDC_P_EQ_DELTA(ulField0,
		pFmtInfo->ulDigitalHeight / BVDC_P_FIELD_PER_FRAME, 3);
	bField1 = BVDC_P_EQ_DELTA(ulField1,
		pFmtInfo->ulDigitalHeight / BVDC_P_FIELD_PER_FRAME, 3);

	/* Need to only detect the first field that reach the expected height
	 * compare that with the interrupt id to figure out if it's swapped or
	 * not.  If both field reach we can't tell. */
	if(bField0 && bField1)
	{
		/* Not able to detect, don't swap anything.  May need to restart the
		 * detection process. */
		hHdDvi->bReverseTrigger = false;
	}
	else
	{
		if(bField0)
		{
			hHdDvi->bReverseTrigger = iFieldId ? false : true;
		}
		else if(bField1)
		{
			hHdDvi->bReverseTrigger = iFieldId ? true : false;
		}
		hHdDvi->ulDetectTrig = BVDC_P_HDDVI_DETECT_TRIGGER_SWAP;
		BDBG_MSG(("HD_DVI[%d] Triggers are %s", hHdDvi->eId,
			hHdDvi->bReverseTrigger ? "SWAPPED" : "NORMAL"));
	}

	return;
}
#endif


/***************************************************************************
 *
 */
static const BVDC_P_HdDvi_DeConfig* BVDC_P_HdDvi_GetDeCfg_isr
	( const BFMT_VideoInfo            *pFmtInfo )
{
	uint32_t i;
	const BVDC_P_HdDvi_DeConfig *pDeCfg = NULL;

	for(i = 0; i < BVDC_P_DE_TABLE_COUNT; i++)
	{
		if(s_aDeCfgTbl[i].eVideoFmt == pFmtInfo->eVideoFmt)
		{
			pDeCfg = &s_aDeCfgTbl[i];
			break;
		}
	}

	return pDeCfg;
}


/***************************************************************************
 * HD_DVI state machine!
 *
 */
static void BVDC_P_HdDvi_UpdateStatus_isr
	( BVDC_P_HdDvi_Handle              hHdDvi )
{
	bool bVideoDetected;
	uint32_t ulPixelDecimate;

	uint32_t ulDigitalHeight;
	BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_DirtyBits *pCurDirty;
	BAVC_VDC_HdDvi_Picture *pCurPic;
	const BFMT_VideoInfo *pNewFmtInfo;
	BAVC_FrameRateCode eFrameRateCode;

	/* To be updated by following function from hw status */
	BDBG_OBJECT_ASSERT(hHdDvi->hSource, BVDC_SRC);
	pCurPic   = &hHdDvi->stXvdField;
	pCurInfo  = &hHdDvi->hSource->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;

	/* Look for these changes. */
	bVideoDetected  = hHdDvi->bVideoDetected;
	ulPixelDecimate = hHdDvi->ulPixelDecimate;
	eFrameRateCode  = hHdDvi->hSource->eFrameRateCode;
	pNewFmtInfo     = pCurInfo->pFmtInfo;

	/* Look at hw status and program feedback!  These process to be move
	 * down to hardware when possible. */
	BVDC_P_HdDvi_ReadHwStatus_isr(hHdDvi, &hHdDvi->stStatus);

	/* Do auto detection and video detection. */
	if(pCurInfo->bAutoFmtDetection)
	{
		BVDC_P_HdDvi_UpdateAutoFormat_isr(hHdDvi, hHdDvi->hSource, &pNewFmtInfo,
			&ulPixelDecimate, &bVideoDetected);
		ulDigitalHeight = pNewFmtInfo->ulDigitalHeight;
		if(pNewFmtInfo->eOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			ulDigitalHeight = 2 * ulDigitalHeight + pNewFmtInfo->ulActiveSpace;
		}
	}
	else
	{
		/* TODO: Refactor this portion of the code with videodetect */
		uint32_t ulTolWidth, ulTolHeight;
		const BVDC_P_HdDviInput *pStatus = &hHdDvi->stStatus;
		const BVDC_HdDvi_Settings *pSettings = &pCurInfo->stHdDviSetting;
		const BVDC_HdDvi_FormatTolerance *pTolerance = &pSettings->stFmtTolerence;
		uint32_t ulStatusScanWidth = pStatus->ulHBlank + pStatus->ulAvWidth;

		ulDigitalHeight = pNewFmtInfo->ulDigitalHeight;
		if(pNewFmtInfo->eOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			ulDigitalHeight = 2 * ulDigitalHeight + pNewFmtInfo->ulActiveSpace;
		}

		/* PR52568: special 1090i */
		if(BFMT_VideoFmt_e1080i == pNewFmtInfo->eVideoFmt)
		{
			ulTolWidth  = pTolerance->ulWidth;
			ulTolHeight = BVDC_P_MAX(pTolerance->ulHeight, BVDC_P_HDDVI_1090i_MIN_TOLER_HEIGHT);
		}
		else if (BFMT_VideoFmt_e720p == pNewFmtInfo->eVideoFmt)
		{
			ulTolWidth  = BVDC_P_MAX(pTolerance->ulWidth,  BVDC_P_HDDVI_725p_MIN_TOLER_WIDTH);
			ulTolHeight = BVDC_P_MAX(pTolerance->ulHeight, BVDC_P_HDDVI_725p_MIN_TOLER_HEIGHT);
		}
		else
		{
			ulTolWidth  = pTolerance->ulWidth;
			ulTolHeight = pTolerance->ulHeight;
		}

		if((pNewFmtInfo->bInterlaced == pStatus->bInterlaced) &&
		   (BVDC_P_EQ_DELTA(pStatus->ulAvWidth, pNewFmtInfo->ulDigitalWidth, ulTolWidth)) &&
		   (BVDC_P_EQ_DELTA(pStatus->ulAvHeight, ulDigitalHeight, ulTolHeight)) &&
		   (BVDC_P_EQ_DELTA(pStatus->ulVertFreq, pNewFmtInfo->ulVertFreq, hHdDvi->ulVertFreqTolerance)))
		{
#if (!BVDC_P_HDDVI_TEST_NEW_FORMAT)
			if((BFMT_VideoFmt_e1080p == pNewFmtInfo->eVideoFmt) ||
			   (BFMT_VideoFmt_eDVI_1400x1050p_60Hz == pNewFmtInfo->eVideoFmt) ||
			   (BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red == pNewFmtInfo->eVideoFmt) ||
			   (BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red == pNewFmtInfo->eVideoFmt))
			{
				if(BVDC_P_EQ_DELTA(ulStatusScanWidth, pNewFmtInfo->ulScanWidth, hHdDvi->ulScanWidthTolerance))
				{
					bVideoDetected = true;
				}
			}
			else
#endif
			{
				bVideoDetected = true;
			}
		}
		else if(pCurPic->bHdmiMode)
		{
			bVideoDetected = true;
		}
		else if((ulDigitalHeight == pStatus->ulAvHeight) &&
		        (pStatus->bInterlaced == pNewFmtInfo->bInterlaced) &&
		        (pStatus->ulAvWidth > pNewFmtInfo->ulDigitalWidth) &&
		        (!(pStatus->ulAvWidth % pNewFmtInfo->ulDigitalWidth)))
		{
			bVideoDetected   = false;
			ulPixelDecimate  = (pStatus->ulAvWidth / pNewFmtInfo->ulDigitalWidth);
			ulPixelDecimate -= 1;
		}
		else
		{
#if BFMT_LEGACY_3DTV_SUPPORT
			if(BFMT_IS_3D_MODE(pNewFmtInfo->eVideoFmt) &&
			   pCurInfo->bHVStartOverride && pCurInfo->stHdDviSetting.bEnableDe &&
			   (BVDC_P_EQ_DELTA(pStatus->ulAvWidth, pNewFmtInfo->ulDigitalWidth, ulTolWidth)) &&
			   (BVDC_P_EQ_DELTA(pStatus->ulAvHeight, ulDigitalHeight - pCurInfo->ulVstart + pNewFmtInfo->ulTopActive - 1, ulTolHeight)) &&
			   (BVDC_P_EQ_DELTA(pStatus->ulVertFreq, pNewFmtInfo->ulVertFreq, hHdDvi->ulVertFreqTolerance)))
			{
				bVideoDetected = true;
			}
			else
#endif
			{
				bVideoDetected  = false;
				ulPixelDecimate = 0;
			}
		}
	}

	/* FIFO overflow? */
	if(hHdDvi->bFifoErr)
	{
		if(!pCurPic->bHdmiMode)
		{
			ulPixelDecimate = 0;
		}

		bVideoDetected       = false;
		hHdDvi->ulDetectTrig = 0;
		hHdDvi->bResetFormatDetect = true;
		BDBG_WRN(("HD_DVI[%d] Fifo overflow!", hHdDvi->eId));
	}

	/* Detected video or not */
	bVideoDetected &= (
		!hHdDvi->bPctrErr &&
		!hHdDvi->bBridgeErr);

	/* Change in status? */
	if(hHdDvi->bVideoDetected != bVideoDetected)
	{
		hHdDvi->bVideoDetected      = bVideoDetected;
		hHdDvi->hSource->bStartFeed = bVideoDetected;
		pCurDirty->stBits.bVideoDetected   = BVDC_P_DIRTY;

		/* Reset the ulPixelDecimate on lost of video detection. */
		if(!bVideoDetected)
		{
			if(!pCurPic->bHdmiMode)
			{
				ulPixelDecimate = 0;
			}

			hHdDvi->ulDetectTrig       = 0;
			hHdDvi->bResetFormatDetect = true;
		}
		else
		{
			BDBG_MSG(("HdDvi[%d] video detected format %s", hHdDvi->eId,
				pNewFmtInfo->pchFormatStr));
		}
	}

	/* If hmdi mode use from aviInfo frame */
	if(pCurPic->bHdmiMode)
	{
		ulPixelDecimate = pCurPic->ulPixelRepitionFactor;
	}

	/* Need to wait!  Notice here we know the format, but would like to
	 * correct the pixel decimation, and not reset bResetFormatDetect circuit. */
	if(hHdDvi->ulPixelDecimate != ulPixelDecimate)
	{
		/* If pixel value equal 0, ignore it, otherwise it's not 0, wait pixel
		 * decimate value becoming a stable value because auto format detection
		 * probably gets a wrong decimate value during format changing */
		if((!ulPixelDecimate) ||
		   (hHdDvi->ulPixelDecimateCnt >= BVDC_P_HDDVI_PIXEL_DECIMATE_COUNT))
		{
			hHdDvi->ulDetectTrig        = 0;
			hHdDvi->ulPixelDecimate     = ulPixelDecimate;
			hHdDvi->ulPixelDecimateCnt  = 0;
			pCurDirty->stBits.bFvFhShift       = BVDC_P_DIRTY;
		}
		else
		{
			/* When pixel decimate value changing,  set decimate bit to 0
			to prevent none-zero decimate bit value from affecting format detection */
			ulPixelDecimate             = 0;
			hHdDvi->ulPixelDecimateCnt += 1;
			pCurDirty->stBits.bFvFhShift       = BVDC_P_DIRTY;
		}
	}

	if(pCurInfo->pFmtInfo != pNewFmtInfo)
	{
		BVDC_P_Source_Info *pNewInfo = &hHdDvi->hSource->stNewInfo;

		/* HdDvi configurating to new format! */
		BVDC_P_BUF_MSG(("HdDvi[%d] Format change %s -> %s", hHdDvi->eId,
			pCurInfo->pFmtInfo->pchFormatStr, pNewFmtInfo->pchFormatStr));

		/* Raster type changed? */
		if(pCurInfo->pFmtInfo->bInterlaced != pNewFmtInfo->bInterlaced)
		{
			hHdDvi->hSource->bRasterChanged = true;
		}

		/* Start the new format */
		hHdDvi->hSource->bPictureChanged = true;
		hHdDvi->hSource->ulVertFreq      = pNewFmtInfo->ulVertFreq;
		pCurInfo->pFmtInfo               = pNewInfo->pFmtInfo = pNewFmtInfo;
		pCurInfo->eAspectRatio           = pCurInfo->pFmtInfo->eAspectRatio;

		/* Get vdc base fmt information */
		pCurInfo->pVdcFmt        = pNewInfo->pVdcFmt  =
			BVDC_P_GetFormatInfoPtr_isr(pNewFmtInfo);

		/* Format changes, so set dirty to rebuild RUL. */
		pCurDirty->stBits.bInputFormat  = BVDC_P_DIRTY;

		/* inform next ApplyChanges to copy activated isr setting into new info */
		hHdDvi->hSource->stIsrInfo.stActivated.stBits.bInputFormat = BVDC_P_DIRTY;
		hHdDvi->hSource->stIsrInfo.stActivated.stBits.bAspectRatio = BVDC_P_DIRTY;
	}

#if (BVDC_P_HDDVI_SW_RESET)
	if(hHdDvi->bBridgeErr)
	{
		bool bSavedResume;
		BDBG_MSG(("HdDvi[%d] bSwReset", hHdDvi->eId));
		hHdDvi->bSwReset = true;
		bSavedResume = pCurDirty->stBits.bResume;
		BVDC_P_SET_ALL_DIRTY(pCurDirty);

		/* Don't inadvertly insert a "resume" if vdc was in src pending mode */
		pCurDirty->stBits.bResume = bSavedResume;
	}
#endif
	/* Don't update status until video locked and status is clean;. */
	if(BVDC_P_IS_CLEAN(pCurDirty))
	{
		BVDC_P_HdDvi_UpdateFramerate_isr(hHdDvi, hHdDvi->hSource, pCurInfo, &eFrameRateCode);
	}

	/* HW detects new frame change. */
	if(hHdDvi->hSource->eFrameRateCode != eFrameRateCode)
	{
		hHdDvi->hSource->eFrameRateCode   = eFrameRateCode;
		pCurDirty->stBits.bFrameRateCode = BVDC_P_DIRTY;
	}

	/* Need to update anything due dirty got set? */
	if(BVDC_P_IS_DIRTY(pCurDirty))
	{
		/* Update the hddvi context for window.  Note it could be updated by
		 * HW auto format detection or by format can be updated by user. */
		if(pCurDirty->stBits.bInputFormat)
		{
			BVDC_P_Source_Info *pNewInfo = &hHdDvi->hSource->stNewInfo;
			if(pCurInfo->bAutoFmtDetection)
			{
				/* reset position on a new format. */
#if BFMT_LEGACY_3DTV_SUPPORT
				/* if 1080p3D, we'd like to keep the DE crop when detecting format */
				if(!VIDEO_FORMAT_IS_CUSTOM_1080P3D(pNewFmtInfo->eVideoFmt))
#endif
				{
					pNewInfo->bHVStartOverride = false;
					pCurInfo->bHVStartOverride = false;
				}
			}

			/* Handed the downstream BVN with the local filled in.   This is if
			 * there is no callback to tell us what the decode frame should be. */
			pNewFmtInfo = pCurInfo->pFmtInfo;

			/* Init current field. */
			pCurPic->eAspectRatio            = pNewFmtInfo->eAspectRatio;
			pCurPic->ulSourceHorizontalSize  = pNewFmtInfo->ulDigitalWidth;
			pCurPic->ulSourceVerticalSize    = ulDigitalHeight;

			/* These will be fill-in by call back. */
			if(!pCurInfo->pfPicCallbackFunc)
			{
				pCurPic->eCscMode                 = BAVC_CscMode_e709RgbFullRange;
				pCurPic->eColorSpace              = BAVC_Colorspace_eYCbCr444;
				pCurPic->eMatrixCoefficients      = BAVC_MatrixCoefficients_eItu_R_BT_709;
				pCurPic->eTransferCharacteristics = BAVC_TransferCharacteristics_eItu_R_BT_709;
			}
		}

		/* recompute h/v delay for DE. */
		if((pCurDirty->stBits.bMiscCtrl) ||
		   (pCurDirty->stBits.bInputFormat))
		{
			const BVDC_P_HdDvi_DeConfig *pDeCfg;

			pDeCfg = BVDC_P_HdDvi_GetDeCfg_isr(pCurInfo->pFmtInfo );
			if(pDeCfg)
			{
				hHdDvi->ulHorzDelay = pDeCfg->ulHorzDelay;
				hHdDvi->ulVertDelay = pDeCfg->ulVertDelay;
			}
			else
			{
				hHdDvi->ulHorzDelay = pCurInfo->pFmtInfo->ulScanWidth  -
					pCurInfo->pFmtInfo->ulDigitalWidth;
				hHdDvi->ulVertDelay = pCurInfo->pFmtInfo->ulScanHeight -
					pCurInfo->pFmtInfo->ulDigitalHeight;
			}
		}
	}

	/* Keeping track of what happen each vsync. */
	hHdDvi->ulVsyncCnt++;

	return;
}


/***************************************************************************
 *
 */
void BVDC_P_HdDvi_GetStatus_isr
	( const BVDC_P_HdDvi_Handle        hHdDvi,
	  bool                            *pbVideoDetected )
{
	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);

	if(pbVideoDetected)
	{
		*pbVideoDetected = hHdDvi->bVideoDetected;
	}

	return;
}

/***************************************************************************
 *
*/
void BVDC_P_HdDvi_GetInputStatus
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_Source_InputStatus         *pInputStatus )
{
	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	BDBG_OBJECT_ASSERT(hHdDvi->hSource, BVDC_SRC);

	BKNI_EnterCriticalSection();
	pInputStatus->bInterlaced    = hHdDvi->stStatus.bInterlaced;
	pInputStatus->ulHPolarity    = hHdDvi->stStatus.ulHPolarity;
	pInputStatus->ulVPolarity    = hHdDvi->stStatus.ulVPolarity;
	pInputStatus->ulAvWidth      = hHdDvi->stStatus.ulAvWidth;
	pInputStatus->ulAvHeight     = hHdDvi->stStatus.ulAvHeight;
	pInputStatus->ulHBlank       = hHdDvi->stStatus.ulHBlank;
	pInputStatus->ulVBlank       = hHdDvi->stStatus.ulVBlank;
	pInputStatus->ulVertFreq     = hHdDvi->stStatus.ulVertFreq;
	pInputStatus->bVsyncDetected = hHdDvi->stStatus.bPllDetected;
	BKNI_LeaveCriticalSection();

	/* No input if vsync_cnt saturated! */
	if(!pInputStatus->bVsyncDetected)
	{
		pInputStatus->pFmtInfo  = NULL;
		pInputStatus->bNoSignal = true;
	}
	else
	{
		pInputStatus->bNoSignal = false;

		/* There is signal, but need to check if format is known/unknown */
		if(hHdDvi->bVideoDetected)
		{
			pInputStatus->pFmtInfo = hHdDvi->hSource->stCurInfo.pFmtInfo;
		}
		else
		{
			pInputStatus->pFmtInfo = NULL;
		}
	}

	/* no remapped just same as original */
	pInputStatus->pOriFmtInfo = pInputStatus->pFmtInfo;

	return;
}

/***************************************************************************
 *
 */
void BVDC_P_HdDvi_Bringup_isr
	( BVDC_P_HdDvi_Handle              hHdDvi )
{
	uint32_t i;
	BRDC_List_Handle hList;
	BRDC_Slot_Handle hSlot;
	BVDC_P_ListInfo stList, *pList;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	BDBG_OBJECT_ASSERT(hHdDvi->hSource, BVDC_SRC);

	/* Just build on the top field only!  This is a force execution. */
	BVDC_P_SRC_NEXT_RUL(hHdDvi->hSource, BAVC_Polarity_eBotField);
	hSlot = BVDC_P_SRC_GET_SLOT(hHdDvi->hSource, BAVC_Polarity_eBotField);
	hList = BVDC_P_SRC_GET_LIST(hHdDvi->hSource, BAVC_Polarity_eBotField);

	/* This will arm all the slot with trigger executions. */
	for(i = 0; i < hHdDvi->hSource->ulSlotUsed; i++)
	{
		BRDC_Slot_ExecuteOnTrigger_isr(hHdDvi->hSource->ahSlot[i],
			hHdDvi->hSource->aeTrigger[i], true);
	}

	/* Get the list infos */
	pList = &stList;
	BVDC_P_ReadListInfo_isr(pList, hList);

	/* Build drian RUL.  Only need to build it once. */
	BVDC_P_Drain_BuildRul_isr(&hHdDvi->hSource->stDrain, pList);

#if (BVDC_P_HDDVI_SW_RESET)
	if(hHdDvi->bSwReset)
	{
		BVDC_P_HdDvi_BuildResetSw_isr(hHdDvi, pList);
		hHdDvi->bSwReset = false;
	}
#endif
	/* Build Hddvi block */
	BVDC_P_HdDvi_BuildReset_isr(hHdDvi, pList, true);
	BVDC_P_HdDvi_BuildTopRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
	BVDC_P_HdDvi_BuildFormatRul_Isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
	BVDC_P_HdDvi_BuildControlRul_Isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
	BVDC_P_HdDvi_BuildHVStartRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
	BVDC_P_HdDvi_BuildCscRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_3)
	BVDC_P_HdDvi_BuildMuxRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
#endif
	BVDC_P_HdDvi_BuildReset_isr(hHdDvi, pList, false);

	/* Update the list */
	BVDC_P_WriteListInfo_isr(pList, hList);

	/* Initial bringup list */
	BRDC_Slot_SetList_isr(hSlot, hList);
	BRDC_Slot_Execute_isr(hSlot);

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_HdDvi_BuildRul_isr
	( BVDC_P_HdDvi_Handle              hHdDvi,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId )
{
	BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_DirtyBits *pCurDirty, *pOldDirty;

	BDBG_OBJECT_ASSERT(hHdDvi, BVDC_DVI);
	BDBG_OBJECT_ASSERT(hHdDvi->hSource, BVDC_SRC);

	pOldDirty = &hHdDvi->hSource->astOldDirty[eFieldId];
	pCurInfo  = &hHdDvi->hSource->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;

	/* Clear old dirty bits. */
	if(BVDC_P_IS_DIRTY(pOldDirty))
	{
		if(!pList->bLastExecuted)
		{
			BVDC_P_OR_ALL_DIRTY(pCurDirty, pOldDirty);
		}
		else
		{
			BVDC_P_CLEAN_ALL_DIRTY(pOldDirty);
		}
	}

	/* Let's see what we need to build. */
	if(BVDC_P_IS_DIRTY(pCurDirty) || hHdDvi->hSource->bDeferSrcPendingCb)
	{
#if (BDBG_DEBUG_BUILD)
		/* Let's why we're not detecting/detecting */
		if(pCurDirty->stBits.bVideoDetected)
		{
			const BVDC_P_HdDviInput *pStatus = &hHdDvi->stStatus;
			BDBG_MSG(("bHdmiMode               = %d", hHdDvi->stXvdField.bHdmiMode));
			BDBG_MSG(("pStatus->bInterlaced    = %d", pStatus->bInterlaced   ));
			BDBG_MSG(("pStatus->ulPxlFreq      = %d", pStatus->ulPxlFreq     ));
			BDBG_MSG(("pStatus->ulVertFreq     = %d", pStatus->ulVertFreq    ));
			BDBG_MSG(("pStatus->ulClkPerVsync  = %d", pStatus->ulClkPerVsync ));
			BDBG_MSG(("pStatus->ulVBlank       = %d", pStatus->ulVBlank      ));
			BDBG_MSG(("pStatus->ulHBlank       = %d", pStatus->ulHBlank      ));
			BDBG_MSG(("pStatus->ulAvWidth      = %d", pStatus->ulAvWidth     ));
			BDBG_MSG(("pStatus->ulAvHeight     = %d", pStatus->ulAvHeight    ));
			BDBG_MSG(("pStatus->ulVPolarity    = %d", pStatus->ulVPolarity   ));
			BDBG_MSG(("pStatus->ulHPolarity    = %d", pStatus->ulHPolarity   ));
			BDBG_MSG(("pHdDvi->ulPixelDecimate = %d", hHdDvi->ulPixelDecimate));
			BDBG_MSG(("---------------------------"));
		}
#endif

		BDBG_MSG(("pCurDirty       = 0x%08x", pCurDirty->aulInts[0]));
		BDBG_MSG(("pfGenCallback   = %d, dirty = %d",
			pCurInfo->pfGenericCallback, pCurDirty->stBits.bGenCallback));
		BDBG_MSG(("bVideoDetected  = %d, dirty = %d",
			hHdDvi->bVideoDetected, pCurDirty->stBits.bVideoDetected));
		BDBG_MSG(("ulPixelDecimate = %d, dirty = %d",
			hHdDvi->ulPixelDecimate, pCurDirty->stBits.bFvFhShift));
		BDBG_MSG(("eFrameRateCode  = %d, dirty = %d",
			hHdDvi->hSource->eFrameRateCode, pCurDirty->stBits.bFrameRateCode));
		BDBG_MSG(("AutoDetect      = %d, dirty = %d",
			pCurInfo->bAutoFmtDetection, pCurDirty->stBits.bAutoDetectFmt));
		BDBG_MSG(("bDeEnable       = %d, dirty = %d, datamode = %d",
			pCurInfo->stHdDviSetting.bEnableDe, pCurDirty->stBits.bMiscCtrl,
			pCurInfo->stHdDviSetting.eInputDataMode));
		BDBG_MSG(("eVideoFmt       = %s, dirty = %d",
			pCurInfo->pFmtInfo->pchFormatStr, pCurDirty->stBits.bInputFormat));
		BDBG_MSG(("bUserCsc        = %d, dirty = %d, colorspace = %d",
			pCurInfo->bUserCsc, pCurDirty->stBits.bColorspace, hHdDvi->eInputColorSpace));
		BDBG_MSG(("------------------------------intP%d", eFieldId));

#if (BVDC_P_HDDVI_SW_RESET)
		if(hHdDvi->bSwReset)
		{
			BVDC_P_HdDvi_BuildResetSw_isr(hHdDvi, pList);
			hHdDvi->bSwReset = false;
		}
#endif

		if((pCurDirty->stBits.bMiscCtrl) ||
		   (pCurDirty->stBits.bWinChanges) ||
		   (pCurDirty->stBits.bFvFhShift) ||
		   (pCurDirty->stBits.bManualPos) ||
		   (pCurDirty->stBits.bGenCallback) ||
		   (pCurDirty->stBits.bPicCallback) ||
		   (pCurDirty->stBits.bInputFormat) ||
		   (pCurDirty->stBits.bVideoDetected))
		{
			BVDC_P_HdDvi_BuildReset_isr(hHdDvi, pList, true);
			BVDC_P_HdDvi_BuildTopRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
			BVDC_P_HdDvi_BuildFormatRul_Isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
			BVDC_P_HdDvi_BuildControlRul_Isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
			BVDC_P_HdDvi_BuildHVStartRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
			BVDC_P_HdDvi_BuildCscRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
#if (BVDC_P_SUPPORT_HDDVI_VER >= BVDC_P_HDDVI_NEW_VER_3)
			BVDC_P_HdDvi_BuildMuxRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
#endif
			BVDC_P_Drain_BuildFormatRul_isr(&hHdDvi->hSource->stDrain,
				&hHdDvi->hSource->stScanOut, pCurInfo->pFmtInfo, pList);
			BVDC_P_HdDvi_BuildReset_isr(hHdDvi, pList, false);
		}
		if(pCurDirty->stBits.bColorspace)
		{
			BVDC_P_HdDvi_BuildControlRul_Isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
			BVDC_P_HdDvi_BuildCscRul_isr(hHdDvi, pList, &hHdDvi->hSource->stCurInfo);
		}

		/* Callback when following things change status */
		if((pCurInfo->pfGenericCallback) &&
		   ((pCurDirty->stBits.bGenCallback)   ||
		    (pCurDirty->stBits.bInputFormat)   ||
		    (pCurDirty->stBits.bVideoDetected) ||
		    (pCurDirty->stBits.bFrameRateCode) ||
		    (hHdDvi->hSource->bDeferSrcPendingCb) ||
		    (pCurDirty->stBits.bAddWin && pCurInfo->eResumeMode)))
		{
			BVDC_Source_CallbackData *pCbData = &hHdDvi->hSource->stSourceCbData;
			BVDC_Source_DirtyBits *pCbDirty = &pCbData->stDirty;

			/* Clear dirty bits */
			BVDC_P_CB_CLEAN_ALL_DIRTY(pCbDirty);

			/* Issue src pending call back when shutdown BVN completed. */
			if(hHdDvi->hSource->bDeferSrcPendingCb)
			{
				BVDC_P_Source_CheckAndIssueCallback(hHdDvi->hSource, pCbDirty);
			}

			/* Make sure the callback happen at least once, on first
			 * installation of callback to report the current status. */
			if(pCurDirty->stBits.bGenCallback)
			{
				pCbDirty->bActive     = BVDC_P_DIRTY;
				pCbDirty->bFmtInfo    = BVDC_P_DIRTY;
				pCbDirty->bFrameRate  = BVDC_P_DIRTY;
				hHdDvi->hSource->bDeferSrcPendingCb = true;
			}
			else
			{
				/* Which one triggers callback? */
				pCbDirty->bActive     = pCurDirty->stBits.bVideoDetected;
				pCbDirty->bFmtInfo    = pCurDirty->stBits.bInputFormat;
				pCbDirty->bFrameRate  = pCurDirty->stBits.bFrameRateCode;

				/* defer source pending callback until all its windows are shutdown! */
				if((pCurDirty->stBits.bInputFormat || pCurDirty->stBits.bAddWin) &&
				   (pCurInfo->eResumeMode))
				{
					hHdDvi->hSource->bDeferSrcPendingCb = true;
				}
			}

			/* callback only if something changed */
			if(BVDC_P_CB_IS_DIRTY(pCbDirty))
			{
				/* Update Callback data */
				pCbData->bActive  = hHdDvi->bVideoDetected;
				pCbData->pFmtInfo = pCurInfo->pFmtInfo;
				pCbData->eFrameRateCode = hHdDvi->hSource->eFrameRateCode;

				/* Callback application with the above data */
				pCurInfo->pfGenericCallback(pCurInfo->pvGenericParm1,
					pCurInfo->iGenericParm2, (void*)pCbData);
			}
		}

		/* Clear dirty bits. */
		*pOldDirty = *pCurDirty;
		BVDC_P_CLEAN_ALL_DIRTY(pCurDirty);
	}

	/* These entries always get build vsync. */
	BVDC_P_HdDvi_BuildVsyncRul_isr(hHdDvi, pList, pCurInfo);

	return;
}


/***************************************************************************
 * This function get register when we do open.
 *
 * User need not to access this functions.
 */
void BVDC_P_Source_HdDviDataReady_isr
	( void                            *pvSourceHandle,
	  int                              iParam2 )
{
	int i;
	BVDC_P_ListInfo stList;
	BVDC_Source_Handle hSource = (BVDC_Source_Handle)pvSourceHandle;
	BVDC_P_Source_Info *pCurInfo;
	BRDC_Slot_Handle hSlot, hOtherSlot;
	BRDC_List_Handle hList;
	BAVC_VDC_HdDvi_Picture *pCurPic;

	BDBG_ENTER(BVDC_P_Source_HdDviDataReady_isr);

	/* Get Source context */
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hHdDvi, BVDC_DVI);

	/* Get current settings */
	pCurInfo = &hSource->stCurInfo;

	/* We should not even get other source here. */
	BDBG_ASSERT(BVDC_P_SRC_IS_HDDVI(hSource->eId));

	/* HD_DVI source is always sync-slip with a compositor/vec. */
	BDBG_ASSERT(!hSource->hSyncLockCompositor);

	/* Make sure the BKNI enter/leave critical section works. */
	BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
	hSource->hVdc->ulInsideCs ++;

	/* Update source user info */
	BVDC_P_Source_UpdateSrcState_isr(hSource);

	/* Update source info; and state for sw work-around */
	BVDC_P_HdDvi_UpdateStatus_isr(hSource->hHdDvi);

#if (BVFD_P_HDDVI_TRIGGER_WORKAROUND)
	if(BVDC_P_HDDVI_DETECT_TRIGGER_SWAP > hSource->hHdDvi->ulDetectTrig)
	{
		BVDC_P_HdDvi_DetectTrigger_isr(hSource->hHdDvi, pCurInfo->pFmtInfo, iParam2);
	}
	else if((hSource->hHdDvi->bReverseTrigger) &&
	        (pCurInfo->pFmtInfo->bInterlaced))
	{
		iParam2 = BVDC_P_NEXT_POLARITY(iParam2);
	}
#endif

#if BVFD_P_HDDVI_FIELD_INVERSION_WORKAROUND
	if((pCurInfo->pFmtInfo->bInterlaced) &&
	   ((hSource->hHdDvi->stStatus.ulVBlank / BVDC_P_FIELD_PER_FRAME) > hSource->hHdDvi->stStatus.ulBottomVBlank))
	{
		iParam2 = BVDC_P_NEXT_POLARITY(iParam2);
	}
#endif

	/* for progressive src format, we always expect Top slot interrupt */
	hSource->eNextFieldIntP = iParam2;
	hSource->eNextFieldId   = pCurInfo->pFmtInfo->bInterlaced
		? BVDC_P_NEXT_POLARITY(iParam2) : BAVC_Polarity_eFrame; /* This programs the next slot */

	/* Get the approriate slot/list for building RUL. */
	BVDC_P_SRC_NEXT_RUL(hSource, hSource->eNextFieldIntP);
	hSlot = BVDC_P_SRC_GET_SLOT(hSource, hSource->eNextFieldIntP);
	hList = BVDC_P_SRC_GET_LIST(hSource, hSource->eNextFieldIntP);

	/* Update the status of last executed RUL. */
	BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, true);

	/* Assert: list is not connected to any slot */
	BRDC_List_SetNumEntries_isr(hList, 0);

	/* Get current pointer to RUL and info. */
	BVDC_P_ReadListInfo_isr(&stList, hList);
	stList.bMasterList = true;

	/* Use interrupt polarity as source polarity. */
	pCurPic = &hSource->hHdDvi->stXvdField;
	pCurPic->eSourcePolarity = BVDC_P_NEXT_POLARITY(hSource->eNextFieldId);

	/* Check if we need to change colorspace base on report from external
	 * device (e.g. HDR, 7411, etc). */
	if(pCurInfo->pfPicCallbackFunc)
	{
		void *pvNewPic; /* hush gcc warning strict typing */
		BAVC_VDC_HdDvi_Picture *pNewPic = &hSource->stNewXvdField;

		/* Extract the information from callback. */
		pCurInfo->pfPicCallbackFunc(pCurInfo->pvParm1, pCurInfo->iParm2,
		hSource->eNextFieldId, BAVC_SourceState_eActive, &pvNewPic);
		*pNewPic = *((BAVC_VDC_HdDvi_Picture*)pvNewPic);

		/* If there is a change?  Let's get the new colorspace? */
		if(BVDC_P_FIELD_DIFF(pCurPic, pNewPic, eCscMode) ||
		   BVDC_P_FIELD_DIFF(pCurPic, pNewPic, eColorSpace) ||
		   BVDC_P_FIELD_DIFF(pCurPic, pNewPic, eMatrixCoefficients) ||
		   BVDC_P_FIELD_DIFF(pCurPic, pNewPic, eTransferCharacteristics))
		{
			const BVDC_HdDvi_Settings *pSettings = &pCurInfo->stHdDviSetting;

			BDBG_MSG(("eCscMode                [%d->%d]", pCurPic->eCscMode, pNewPic->eCscMode));
			BDBG_MSG(("eColorSpace             [%d->%d]", pCurPic->eColorSpace, pNewPic->eColorSpace));
			BDBG_MSG(("eAspectRatio            [%d->%d]", pCurPic->eAspectRatio, pNewPic->eAspectRatio));
			BDBG_MSG(("eMatrixCoefficients     [%d->%d]", pCurPic->eMatrixCoefficients, pNewPic->eMatrixCoefficients));
			BDBG_MSG(("eTransferCharacteristics[%d->%d]", pCurPic->eTransferCharacteristics, pNewPic->eTransferCharacteristics));

			/* Copy the ext to the our current */
			pCurPic->eCscMode                 = pNewPic->eCscMode;
			pCurPic->eColorSpace              = pNewPic->eColorSpace;
			pCurPic->eMatrixCoefficients      = pNewPic->eMatrixCoefficients;
			pCurPic->eTransferCharacteristics = pNewPic->eTransferCharacteristics;

			/* Default detction tolerance */
			hSource->hHdDvi->ulVertFreqTolerance = BVDC_P_HDDVI_VERTFREQ_TOLER;

			/* HD_DVI converts to BAVC_MatrixCoefficients_eItu_R_BT_709
			 * for downstream BVN module if it's RGB input color space. */
			if(BAVC_Colorspace_eRGB == pCurPic->eColorSpace)
			{
				hSource->hHdDvi->eInputColorSpace = BVDC_P_HdDvi_InputColorSpace_eRGB;
			}
			else
			{
				if(BAVC_Colorspace_eYCbCr422 == pCurPic->eColorSpace)
				{
					if((BVDC_HdDvi_InputDataMode_e30Bit == pSettings->eInputDataMode) &&
					   (BVDC_HdDviInput_Ext == pCurInfo->ulInputPort))
					{
						hSource->hHdDvi->eInputColorSpace = BVDC_P_HdDvi_InputColorSpace_eYCbCr422_DoubleClock;
						hSource->hHdDvi->ulVertFreqTolerance = BVDC_P_HDDVI_DOUBLECLOCK_VERTFREQ_TOLER;
					}
					else
					{
						hSource->hHdDvi->eInputColorSpace = BVDC_P_HdDvi_InputColorSpace_eYCbCr422;
					}
				}
				else
				{
					hSource->hHdDvi->eInputColorSpace = BVDC_P_HdDvi_InputColorSpace_eYCbCr444;
				}
			}

			hSource->bSrcIs444 = BVDC_P_HDDVI_INPUT_RGB(hSource->hHdDvi->eInputColorSpace) ||
				BVDC_P_HDDVI_INPUT_444(hSource->hHdDvi->eInputColorSpace);

			/* Indicate source dirty bits, re-program colorspace converstion. */
			hSource->eMatrixCoefficients  = pCurPic->eMatrixCoefficients;
			pCurInfo->stDirty.stBits.bMiscCtrl   = BVDC_P_DIRTY; /* need to rebuild the muxes */
			pCurInfo->stDirty.stBits.bColorspace = BVDC_P_DIRTY; /* and the CSC */
		}

		/* (2) If there any change in ulpixel decimate and hdmi mode. */
		if(BVDC_P_FIELD_DIFF(pCurPic, pNewPic, bHdmiMode) ||
		   BVDC_P_FIELD_DIFF(pCurPic, pNewPic, ulPixelRepitionFactor))
		{
			BDBG_MSG(("bHdmiMode               [%d->%d]", pCurPic->bHdmiMode, pNewPic->bHdmiMode));
			BDBG_MSG(("ulPixelRepitionFactor   [%d->%d]", pCurPic->ulPixelRepitionFactor, pNewPic->ulPixelRepitionFactor));

			/* Copy the ext to the our current */
			pCurPic->bHdmiMode             = pNewPic->bHdmiMode;
			pCurPic->ulPixelRepitionFactor = pNewPic->ulPixelRepitionFactor;
			pCurInfo->stDirty.stBits.bFvFhShift   = BVDC_P_DIRTY;
		}

		/* (3) If aspectRatio from HDR is invalid(e.g.  it's DVI mode),
		 * HDDVI still uses the aspectRatio from FMT table. */
		/* Check for picture change parameters */
		if((BVDC_P_FIELD_DIFF(pCurPic, pNewPic, eAspectRatio)) &&
		   (BFMT_AspectRatio_eUnknown != pNewPic->eAspectRatio))
		{
			hSource->bPictureChanged = true;
			/* Inform next ApplyChanges to copy activated isr setting into new info */
			pCurPic->eAspectRatio  = pNewPic->eAspectRatio;
			pCurInfo->eAspectRatio = pNewPic->eAspectRatio;
			hSource->stIsrInfo.stActivated.stBits.bAspectRatio = BVDC_P_DIRTY;
		}

		if(BVDC_P_IS_CLEAN(&pCurInfo->stDirty))
		{
			/* (1) Source/display size are invalid.  Corrected them here. */
			if((pNewPic->ulSourceHorizontalSize  < BVDC_P_SRC_INPUT_H_MIN) ||
			   (pNewPic->ulSourceVerticalSize    < BVDC_P_SRC_INPUT_V_MIN))
			{
				pNewPic->ulSourceHorizontalSize  = pCurInfo->pFmtInfo->ulDigitalWidth;
				pNewPic->ulSourceVerticalSize    = pCurInfo->pFmtInfo->ulDigitalHeight;
			}

			/* (2) If we receiving 1088i stream we will assume it contains the
			 * non-active video in the last 8 lines.  We'll drop them.  PR10698.  Or
			 * any value that might break the VDC. */
			if((pNewPic->ulSourceVerticalSize    > pCurInfo->pFmtInfo->ulDigitalHeight) ||
			   (pNewPic->ulSourceHorizontalSize  > pCurInfo->pFmtInfo->ulDigitalWidth))
			{
				pNewPic->ulSourceVerticalSize    =
					BVDC_P_MIN(pNewPic->ulSourceVerticalSize, pCurInfo->pFmtInfo->ulDigitalHeight);
				pNewPic->ulSourceHorizontalSize  =
					BVDC_P_MIN(pNewPic->ulSourceHorizontalSize, pCurInfo->pFmtInfo->ulDigitalWidth);
			}

			/* Check if there are general changes from previous pictures. */
			if(BVDC_P_FIELD_DIFF(pCurPic, pNewPic, ulSourceHorizontalSize) ||
			   BVDC_P_FIELD_DIFF(pCurPic, pNewPic, ulSourceVerticalSize))
			{
				hSource->bPictureChanged        = true;
				pCurPic->ulSourceHorizontalSize = pNewPic->ulSourceHorizontalSize;
				pCurPic->ulSourceVerticalSize   = pNewPic->ulSourceVerticalSize;

				/* for BVDC_Source_GetSize support: need original size  */
				hSource->stExtVideoFmtInfo.ulWidth  = pNewPic->ulSourceHorizontalSize;
				hSource->stExtVideoFmtInfo.ulHeight = pNewPic->ulSourceVerticalSize;
			}
		}

		/* (7) Check if there are general changes from previous pictures. */
		if(BVDC_P_FIELD_DIFF(pCurPic, pNewPic, bMute))
		{
			BDBG_MSG(("bMute                   [%d->%d]", pCurPic->bMute, pNewPic->bMute));
			pCurPic->bMute = pNewPic->bMute;
		}
	}

	/* update windows that connected to this source, including user info,
	 * destroy related state and disconnecting from source */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hSource->ahWindow[i] ||
		   BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
		   BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

#if (BVDC_P_CLEANUP_VNET)
		if(BVDC_MuteMode_eRepeat != pCurInfo->eMuteMode)
		{
			/* Needed to initiate window shutdown when video detect
			 * status toggles to drain VNET switches and reset BVN blocks to clean
			 * up BVB error;  Resetting vnet has side effects
			 * blank screen with background color mix with source mute color. */
			if((hSource->bStartFeed != hSource->bPrevStartFeed))
			{
				if(hSource->bStartFeed)
				{
					BDBG_MSG(("Source[%d] Reconfigs vnet!", hSource->eId));
					BVDC_P_Window_SetReconfiguring_isr(hSource->ahWindow[i], false, true);
				}
			}
		}
#endif
		BVDC_P_Window_AdjustRectangles_isr(hSource->ahWindow[i], NULL,
			&hSource->hHdDvi->stXvdField, 0);
	}

	/* Defer update to make sure all windows get a chance of reconfiguring */
#if (BVDC_P_CLEANUP_VNET)
	if(BVDC_MuteMode_eRepeat != pCurInfo->eMuteMode)
	{
		hSource->bPrevStartFeed = hSource->bStartFeed;
	}
#endif

	/* Get the source scan out rectangle. Combine the user pan-scan info
	 * from all the window that uses this source;
	 * Note: pMvdField, pXvdField = NULL for analog video source. */
	if(pCurInfo->stDirty.stBits.bRecAdjust)
	{
		BVDC_P_Source_GetScanOutRect_isr(hSource, NULL,
			&hSource->hHdDvi->stXvdField, &hSource->stScanOut);
	}

	/* For each window using this source do the following. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hSource->ahWindow[i] ||
		   BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
		   BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

		if(hSource->hHdDvi->bFifoErr)
		{
			BVDC_P_Window_SetReconfiguring_isr(hSource->ahWindow[i], false, true);
		}

		BVDC_P_Window_Writer_isr(hSource->ahWindow[i], NULL,
			&hSource->hHdDvi->stXvdField, hSource->eNextFieldId, &stList, 0);

		/* skip window RUL if shutdown state */
		if(BVDC_P_State_eShutDown != hSource->ahWindow[i]->stCurInfo.eWriterState)
		{
			BVDC_P_Window_BuildRul_isr(hSource->ahWindow[i], &stList, hSource->eNextFieldId,
				true,  /* writer*/
				false, /* reader */
				false); /* canvas */
		}
	}

	if(pCurInfo->stDirty.stBits.bRecAdjust || hSource->bWait4ReconfigVnet)
	{
		/* Gather window information after vnetmode is determined */
		BVDC_P_Source_GetWindowVnetmodeInfo_isr(hSource);
	}

	/* Now build the HD_DVI block. */
	BVDC_P_HdDvi_BuildRul_isr(hSource->hHdDvi, &stList, hSource->eNextFieldId);

	/* UPdate current pictures, the eSourcePolarity must update every field. */
	hSource->bPictureChanged = false;
	hSource->bRasterChanged = false;

	/* Update entries count */
	BVDC_P_WriteListInfo_isr(&stList, hList);

	/* This programs the slot after next. This is needed in conjunction with
	 * the programming of the next slot above and for accommodating same field
	 * polarity sources. Moreover, this is so to make the 4 buffer algorithm
	 * work correctly. */
	hOtherSlot = BVDC_P_SRC_GET_SLOT(hSource,
		BVDC_P_NEXT_POLARITY(hSource->eNextFieldIntP));

	/* Always assign single RUL to two slots to avoid uncovered transition
	 * from dual to one; */
	/* Note: to flush the cached RUL only once, call the Dual function
	   instead of two individual slot functions; */
	BRDC_Slot_SetListDual_isr(hSlot, hOtherSlot, hList);
	hSource->hVdc->ulInsideCs --;
	BDBG_LEAVE(BVDC_P_Source_HdDviDataReady_isr);

	return;
}

/***************************************************************************/
/* No support for any HD_DVI */
#else
void BVDC_P_Source_HdDviDataReady_isr
	( void                            *pvSourceHandle,
	  int                              iParam2 )
{
	BDBG_ASSERT(0);
	BSTD_UNUSED(iParam2);
	BSTD_UNUSED(pvSourceHandle);
	return;
}

BERR_Code BVDC_P_HdDvi_ValidateChanges
	( BVDC_P_HdDvi_Handle               hHdDvi )
{
	BDBG_ASSERT(0);
	BSTD_UNUSED(hHdDvi);
	return BERR_NOT_SUPPORTED;
}
#endif

/* End of File */
