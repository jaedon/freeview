/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdm_edid.c $
 * $brcm_Revision: Hydra_Software_Devel/169 $
 * $brcm_Date: 9/11/12 4:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/src/common/bhdm_edid.c $
 * 
 * Hydra_Software_Devel/169   9/11/12 4:16p rgreen
 * SW7435-306: Make the compilation of the new analog alternative BFMT
 * formats configurable.  Will allow compilation of latest HDM with older
 * reference code.  By default the formats are enabled
 * 
 * Hydra_Software_Devel/168   8/15/12 1:09p rgreen
 * SW7425-3735: Resolve coverity issues
 * 
 * Hydra_Software_Devel/167   8/14/12 6:00p rgreen
 * SW7435-306: Account for alternative NTSC,480p formats (480+ lines) to
 * digital formats (exactly 480 lines).
 * 
 * Hydra_Software_Devel/166   7/18/12 4:48p rgreen
 * SW7425-3480: when overriding 1360x768 formats, check support for 720p
 * 50 and 60.  Downgrade override warning to a message.
 * 
 * Hydra_Software_Devel/165   5/25/12 1:32p rgreen
 * SW7425-3126: Add EDID Video Data Block parsing support for 720p 24, 25,
 * and 30 Hz
 * 
 * Hydra_Software_Devel/164   4/30/12 3:24p rgreen
 * SW7405-5557: Report HDMI Rx does not support a particular supported
 * BcmFormat only once per EDID initialization
 * 
 * Hydra_Software_Devel/163   4/10/12 5:07p rgreen
 * SW7425-2707: Force MonitorRange Settings for 60Hz if MonitorRange
 * descriptor is not found
 * 
 * Hydra_Software_Devel/162   3/23/12 10:30a rgreen
 * SW7346-745: check scan width for all EDID versions to determine if
 * detailed timing support is for 50 vs 60hz
 * 
 * Hydra_Software_Devel/161   3/19/12 11:34a rgreen
 * SW7425-2650: Fix memory leak in HDM PI; Delcare/store supported Video
 * ID Codes in hdm handlle vs mallocing to build list each time
 * 
 * Hydra_Software_Devel/160   3/14/12 1:22p rgreen
 * SW7420-2277: Fix incorrectly reported 1080p 24 or 1080p 30 support from
 * BHDM_EDID_GetVideoInfo function
 * 
 * Hydra_Software_Devel/159   11/22/11 6:02p vle
 * SW7425-1140: Merge to mainline.  Remove all CEC functionality out of
 * HDM PI.
 * 
 * Hydra_Software_Devel/SW7425-1140/2   11/22/11 5:48p vle
 * SW7425-1140: Add BHDM_CONFIG_CEC_LEGACY_SUPPORT for backward compatible
 * for CEC legacy platforms.
 * 
 * Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:16p vle
 * SW7425-1140: Remove all CEC functionalities out of HDM PI
 * 
 * Hydra_Software_Devel/158   10/18/11 5:16p vle
 * SW7125-1091: should not stop parsing DTD in EDID extension even if
 * there's no NativeFormatDescriptors
 * 
 * Hydra_Software_Devel/157   9/15/11 6:07p vle
 * SW7420-2056: merge to mainline
 * 
 * Hydra_Software_Devel/SW7420-2056/1   9/15/11 3:51p vle
 * SW7420-2056: Fix incorrect Hdmi 3D formats support from parsing EDID
 * 
 * Hydra_Software_Devel/156   6/24/11 5:11p rgreen
 * SW7405-5358, SW7405-5312: always indicate 3D formats have been checked
 * whenever HDMI VSDB is parsed
 * 
 * Hydra_Software_Devel/155   6/17/11 5:36p vle
 * SW7405-5318: Fix bug where variable was used before being populated.
 * 
 * Hydra_Software_Devel/154   6/17/11 1:42p vle
 * SW7405-5358, SW7405-5312: Separate 3D support from EDID parser to allow
 * easier back port
 * 
 * Hydra_Software_Devel/153   5/4/11 1:03p vle
 * SW7405-5208: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7405-5208/1   4/5/11 6:48p vle
 * SW7405-5208: Fix incorrect parsing of VSDB in some cases.
 * 
 * Hydra_Software_Devel/152   1/7/11 3:54p vle
 * SW7405-5076: merge to mainline
 * 
 * Hydra_Software_Devel/SW7405-5076/1   1/4/11 4:48p vle
 * SW7405-5076: Fix EDID parser when HDMI_3D_Present bit set to 0.
 * 
 * Hydra_Software_Devel/150   10/14/10 5:34p rgreen
 * SW7125-659: Indicate HDMI support only if VSDB has IEEE Registration ID
 * 0x000C03.  Clear RxVSDB in IsRxDeviceHdmi;  Report 3d Formats for HDMI
 * devices only Force Full Range RGB for supported Colorimetry for DVI
 * formats
 * 
 * Hydra_Software_Devel/149   9/3/10 4:08p vle
 * SW7400-2868: Fix potential bug from previous implementation
 * 
 * Hydra_Software_Devel/148   8/27/10 7:48p vle
 * SW7400-2868: Fix potential EDID parser issue when parsing 3D supports
 * on the first 16 video descriptor
 * 
 * Hydra_Software_Devel/147   8/19/10 6:28p vle
 * SW7335-815: Fix error passing bool by value instead of reference
 * 
 * Hydra_Software_Devel/146   7/15/10 1:37p erickson
 * SW7400-2855: init BCM_VideoFmt out param in BHDM_EDID_GetDetailTiming
 * for error path
 *
 * Hydra_Software_Devel/145   6/22/10 6:57p vle
 * SW7405-3994: Add support to parse all Shorthand and additional 3D
 * Timing/Structure support indication in HDMI 1.4a
 *
 * Hydra_Software_Devel/144   6/7/10 7:16p vle
 * SW7405-3994: Make sure no misleading error messages are printed for
 * non-3D TV EDID.
 *
 * Hydra_Software_Devel/143   6/4/10 6:09p vle
 * SW7405-3994: Merge to main branch
 *
 * Hydra_Software_Devel/SW7405-3994/2   6/4/10 2:37p vle
 * SW7405-3994: Need to check monitor vertical range, not max vertical
 * rate, for 50/60Hz support.
 *
 * Hydra_Software_Devel/SW7405-3994/1   5/14/10 6:12p vle
 * SW7405-3994: Add support to check for supported 3D formats.
 *
 * Hydra_Software_Devel/142   5/27/10 11:37a vle
 * SW7550-438: Fix Coverity issue.
 *
 * Hydra_Software_Devel/141   5/11/10 7:14p vle
 * SW7405-3994: Add topAndBottom 3D Structure
 *
 * Hydra_Software_Devel/140   5/10/10 10:58a erickson
 * SW7405-3994: fix compilation error
 *
 * Hydra_Software_Devel/139   5/7/10 5:45p rgreen
 * SW7405-3994: Update EDID parsing for 3D structure
 *
 * Hydra_Software_Devel/138   4/23/10 10:43a vle
 * SW7420-676: merge to main branch
 *
 * Hydra_Software_Devel/SW7420-676/2   4/22/10 4:54p vle
 * SW7420-676: Fix incorrect programming in return size.
 *
 * Hydra_Software_Devel/SW7420-676/1   4/21/10 2:27p vle
 * SW7420-676: Add API to return supported video info
 *
 * Hydra_Software_Devel/137   4/16/10 1:37p rgreen
 * SW7405-3994: Fix overwriting of RxSupported features when parsing the
 * VSDB
 *
 * Hydra_Software_Devel/136   4/5/10 10:12a rgreen
 * SW7405-3994: Merge support for parsing HDMI 1.4 EDID VSDB
 *
 * Hydra_Software_Devel/SW7401-4363/3   3/5/10 4:11p rgreen
 * JIRA:SW7405-3994: Update HDMI 1.4 parsing for 3D Structure fields in
 * the VSDB
 *
 * Hydra_Software_Devel/SW7401-4363/2   3/1/10 7:23p rgreen
 * SW7401-4363: Update the EDID parsing of the Vendor Specific Data Block
 *
 * Hydra_Software_Devel/SW7401-4363/1   3/1/10 3:35p rgreen
 * SW7401-4363: Add Support to parse 3D information in the VSDB portion of the EDID
 *
 * Hydra_Software_Devel/135   3/1/10 11:32a rgreen
 * SW7420-579: Rename DetailedTiming to SupportedDetailTiming to eliminate
 * confusion when reading code
 *
 * Hydra_Software_Devel/134   2/26/10 5:18p vle
 * SW7405-3016: Remove software i2c settings from HDM PI. This mode is
 * configure in I2C module
 *
 * Hydra_Software_Devel/133   9/24/09 3:40p vle
 * SW7405-2871: Reduce HDMI I2C failure retry count from 10 to 1 since
 * each retry can wait up to 30seconds.
 *
 * Hydra_Software_Devel/132   9/23/09 11:15a vle
 * SW7601-168: Fix mistake on merging changes from bdvd_v3.0 branch.
 *
 * Hydra_Software_Devel/131   9/22/09 1:37p vle
 * SW7601-168: Merge in changes from bdvd_v3.0 branch.
 *
 * Hydra_Software_Devel/bdvd_v3.0/4   9/17/09 1:57p rbshah
 * PR16614[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Add
 * missing check for 4x pixel repetition capability in the
 * VSDB block parsing. This would prevent 480i,p or 576i,p from
 * being detected if 1x or 2x repetition was not supported by
 * the receiver.
 *
 * Reviewed by: Eric Trudeau
 *
 * Hydra_Software_Devel/bdvd_v3.0/3   9/17/09 10:25a rbshah
 * PR16611[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Enhance EDID parsing of detailed timing descriptor so we can
 * differentiate between 24Hz, 25/50Hz, and 30/60Hz refresh rates
 * properly. This is really needed for v1/v2 CEA Extensions because
 * explicit video codes are not used as they are in v3.
 *
 * Reviewed by: Eric Trudeau
 *
 * Hydra_Software_Devel/130   9/15/09 5:59p vle
 * SW7601-168: Merge fixes from bdvd_v3.0 branch.
 *
 * Hydra_Software_Devel/bdvd_v3.0/2   9/2/09 3:31p rbshah
 * PR16468[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Merged with the latest portinginterface/hdm and syslib/hdcplib files.
 *
 * Hydra_Software_Devel/bdvd_v2.0/11   6/29/09 11:19a rbshah
 * PR_15307[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Fix
 * problem in parsing EDID Extension version 3. The Data Block collection
 * was being dropped completely. Re-worked the logic so things are
 * processed in order.
 *
 * Hydra_Software_Devel/129   9/1/09 3:51p vle
 * SW7403-870: Fix build issues for 7403/7401.
 *
 * Hydra_Software_Devel/128   8/28/09 3:59p vle
 * SW7403-870: 720p 25/30Hz are not supported by 7401/7403
 *
 * Hydra_Software_Devel/127   6/23/09 5:08p rgreen
 * PR56323: Compile in debug EDID based on
 * BHDM_CONFIG_DEBUG_EDID_PROCESSING only
 *
 * Hydra_Software_Devel/126   6/23/09 3:34p vle
 * PR55934: Merge bdvd fixes into main branch.
 *
 * Hydra_Software_Devel/125   5/19/09 7:29p vle
 * PR 55080: EDID parsing should not stop if an error was found in a
 * detail timing block.
 *
 * Hydra_Software_Devel/124   5/15/09 4:58p rgreen
 * PR54128: Update EDID processing to add DDP audio passthrough support
 *
 * Hydra_Software_Devel/123   5/6/09 1:57p vle
 * PR 54882: Fix EDID parser bug in parsing Vendor Specific Data Block
 * (VSDB)
 *
 * Hydra_Software_Devel/bdvd_v2.0/10   6/10/09 4:30p rbshah
 * PR_14564[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Bring forward fix from v1.4 & Hydra into v2.* branches.
 *
 * Hydra_Software_Devel/bdvd_v2.0/9   6/8/09 12:44p rbshah
 * PR_14946[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Add
 * missing code in the EDID parser to stash away the Product ID.
 *
 * Code reviewer: Ashish Koul.
 *
 * Hydra_Software_Devel/bdvd_v2.0/8   6/4/09 11:36a rbshah
 * PR_14898[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Fix
 * EDID parsing logic so it doesn't pick up 25Hz, 30Hz, and 50Hz
 * refresh rates unless these are explicitly indicated as being supported
 * by the receiver. These rates were considered valid by the old code if
 * the TV supported 24Hz (because they fell in the 24-60Hz range).
 *
 * Reviewed by: Eric Trudeau.
 *
 * Hydra_Software_Devel/bdvd_v2.0/7   5/5/09 4:07p rbshah
 * PR_14433[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Fix
 * bug in parsing the Vendor Specific Data Block (VSDB). The size of
 * this block wasn't being taken into account correctly all the time and
 * a random byte was getting used to determine whether or not the TV
 * supports Deep Color.
 *
 * Hydra_Software_Devel/122   3/23/09 7:30p vle
 * PR52390:Merge to main branch
 * PAL/576p format uses ITU_R_601 colorimetry.
 *
 * Hydra_Software_Devel/121   3/9/09 3:20p vle
 * PR50570, PR50918, PR49277, PR49652, PR52873:
 * Add API to mute/unmute audio, update pixel repitition support, add
 * SetPixelDataOverride API for transmission of black video. Merge
 * changes/updates from bdvd_v2.0 to main branch.
 *
 * Hydra_Software_Devel/PR52390/2   3/10/09 3:32p vle
 * PR52390: 576p format also uses ITU_R_601 colorimetry.
 *
 * Hydra_Software_Devel/PR52390/1   3/4/09 8:29p vle
 * PR52390: PAL formats uses ITU_R_601 colorimetry.
 *
 * Hydra_Software_Devel/120   2/27/09 8:56p vle
 * PR52505: Fix non-const global data in HDM and HDCPLIB
 *
 * Hydra_Software_Devel/119   2/2/09 7:18p vle
 * PR51682: Add BREG_I2C_ReadSwEDDC support for EDID read.
 *
 * Hydra_Software_Devel/bdvd_v2.0/2   1/30/09 1:04p rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Fix bug
 * in BHDM-EDID: get a false positive for pixel repetition on
 * DVI monitors. This is in response to CSP 206270 (No HDCP event
 * with DVI TV/Monitor).
 *
 * Also turned all BDBG_ERR and BDBG_WRN messages to BKNI_Prints in
 * bdvd-hdmi for now.
 *
 * Hydra_Software_Devel/bdvd_v2.0/1   1/19/09 3:35p rbshah
 * PR_12621 [ see HiDef-DVD bug tracking system for more info ].
 * BHDM_EDID_CheckRxHdmiVideoSupport() was reading EDID over DDC even
 * when
 * there was a cached copy.
 *
 * Hydra_Software_Devel/118   12/23/08 7:57p vle
 * PR50081, PR49866: Merge to main branch
 * Add new API allowing application to get the device physical address
 * even without CEC support.
 * Add deep color support.
 *
 * Hydra_Software_Devel/PR50081/2   12/11/08 2:24p vle
 * PR49866: Add new API allowing application to get the device physical
 * address even without CEC support.
 *
 * Hydra_Software_Devel/PR50081/1   12/8/08 5:29p vle
 * PR50081: add deep color support.
 *
 * Hydra_Software_Devel/117   10/30/08 5:51p vle
 * PR47447: Merge to main branch.
 * Add new GetReceivedMessage API. Use same CEC configuration for
 * transmitter and receiver.
 *
 * Hydra_Software_Devel/116   10/24/08 5:56p vle
 * PR 44535: Merge from bdvd v2.0 branch
 *
 * Hydra_Software_Devel/115   10/17/08 12:33p vle
 * PR 47470: Check for the returned format pointer info. If a custom
 * format is used, there is no information available from BFMT
 *
 * Hydra_Software_Devel/114   10/10/08 6:29p vle
 * PR 47470: Update to fix build issues with older, legacy platforms.
 *
 * Hydra_Software_Devel/113   10/8/08 6:29p vle
 * PR 47470: Check for the returned format pointer info. If a custom
 * format is used, there is no information available from BFMT
 *
 * Hydra_Software_Devel/PR47447/1   9/30/08 7:04p vle
 * PR47447: Add new GetReceivedMessage API. Use same CEC configuration for
 * transmitter and receiver.
 *
 * Hydra_Software_Devel/112   9/23/08 4:56p rgreen
 * PR47281: Enable WMA Support
 *
 * Hydra_Software_Devel/111   7/9/08 3:31p vishk
 * PR 43983: KLOCWORK: bhdm_edid.c, buffer overflow.
 *
 * Hydra_Software_Devel/110   7/7/08 5:52p vle
 * PR44482, PR43424: set default device (HDMI vs DVI) and colorimetry when
 * no EDID found.
 *
 * Hydra_Software_Devel/109   6/6/08 6:24p vle
 * PR 43424: Do not ignore EDID Checksum error.
 *
 * Hydra_Software_Devel/108   4/15/08 2:37p vishk
 * PR 41193: BHDM_EDID_DumpRawEDID can't dump all blocks when there is
 * blockmap in EDID
 *
 * Hydra_Software_Devel/107   4/12/08 5:08p vishk
 * PR 41193: BHDM_EDID_DumpRawEDID can't dump all blocks when there is
 * blockmap in EDID
 *
 * Hydra_Software_Devel/106   4/2/08 10:18a mward
 * PR40681: null version of BHDM_EDID_P_PrintEdid(),
 * BHDM_EDID_DumpRawEDID() for DEBUG=n.
 *
 * Hydra_Software_Devel/105   3/27/08 4:22p vishk
 * PR 40681: Added support to parse extended EDID.
 *
 * Hydra_Software_Devel/104   3/27/08 11:24a vishk
 * PR 40681: Added support to parse extended EDID.
 *
 * Hydra_Software_Devel/103   3/18/08 2:34p rgreen
 * PR39705: Support reading CEA Timing Extenstion V4 as V3
 *
 * Hydra_Software_Devel/102   3/13/08 4:44p rgreen
 * PR39988: Fix compilation error for 7401,7038
 *
 * Hydra_Software_Devel/101   2/27/08 2:16p rgreen
 * PR40063,PR24461: Support parsing DVI V3 Timing Extensions
 * Specify HDMI Audio Support only VSDB Audio Bit is set
 *
 * Hydra_Software_Devel/100   2/11/08 11:56a rgreen
 * PR39232: Merge changes
 *
 * Hydra_Software_Devel/bdvd_v10/4   2/7/08 12:53p rbshah
 * PR_7515 [ Use latest hdm and hdmlib porting interfaces (for HDCP
 * related fixes) ].
 *
 * Hydra_Software_Devel/bdvd_v10/3   2/1/08 11:13a mpeteri
 * PR_7394 [ Export setting required to choose between reencode or pcm ].
 * Fixes for PR7394 and PR6652 - this checkin is built upon 1/31 RAP
 * label and requires that the RAP label be below v10 latest rule in the
 * config spec.  An updated config spec will be sent out to demonstrate
 * this requirement.  NOTE this is temporary and will go back to normal
 * when RAP label picks up the branched changes.
 * RAP label to use - RAP_7440_FW_Phase2D_20080131
 *
 * Hydra_Software_Devel/99   1/28/08 2:39p vle
 * PR 26877: Force to alternate preferred format when DVI 1360x768p 60Hz
 * or DVI 1366x768p 60Hz is detected.
 *
 * Hydra_Software_Devel/98   1/22/08 7:02p rgreen
 * PR38489: Force supported formats to remember the highest number of
 * supported audio channels.  Add additional debug messages for Sample
 * Rate.
 *
 * Hydra_Software_Devel/97   11/9/07 4:48p mward
 * PR36915: Eliminate compiler warning for DEBUG=n.
 *
 * Hydra_Software_Devel/96   10/9/07 6:43p rgreen
 * PR35740: fix C99 declaration compilation error for xvd_vdc_test
 *
 * Hydra_Software_Devel/95   10/9/07 4:21p vle
 * PR 35085: HDM-Coverity (CID 371): NO_EFFECT. Fix coverity issue for
 * comparing array against NULL
 *
 * Hydra_Software_Devel/94   10/4/07 1:30p rgreen
 * PR35740: Add support to parse new audio formats in CEA861C and 861D.
 * Update debug messages
 *
 * Hydra_Software_Devel/93   10/2/07 11:22a vle
 * PR 35085: Fix coverity issue for comparing array against NULL
 *
 * Hydra_Software_Devel/92   9/24/07 6:25p rgreen
 * PR35326:  Check/report for 1080p 60Hz support based on platform
 *
 * Hydra_Software_Devel/91   9/18/07 5:33p rgreen
 * PR27746,PR31853:
 * fix debug bug to print last line of Raw EDID dump
 * fix bug to correctly interpret the number of bytes in the Vendor
 * Specific DB
 * Display HDMI 1.3 EDID Info only when it exists
 *
 * Hydra_Software_Devel/90   8/23/07 4:56p vle
 * PR 34249: Check BCM supported format for 24, 25, and 30Hz formats.
 *
 * Hydra_Software_Devel/89   8/3/07 4:17p vle
 * PR 33783: Fix incorrectly stored audio sampling size in the audio
 * descriptor (3rd byte)
 *
 * Hydra_Software_Devel/88   8/1/07 8:04p vle
 * PR 33721: VGA format (640x480p) should be supported by all HDMI
 * receivers.
 *
 * Hydra_Software_Devel/87   7/19/07 7:57p vle
 * PR 28667: Verify buffer size before memcpy
 *
 * Hydra_Software_Devel/86   6/6/07 3:29p vle
 * PR 31853: Update array range of EDID data array
 *
 * Hydra_Software_Devel/85   5/24/07 12:15p vle
 * PR 31570: Add capability to parse CEA Colorimetry Data Block
 *
 * Hydra_Software_Devel/84   5/14/07 2:02p vle
 * PR 30730: Modify EDID parser to parse all EDID blocks including those
 * V3 Timing Extension blocks
 *
 * Hydra_Software_Devel/83   2/15/07 2:47p vle
 * PR 27746: Added Support to EDID parser to process VSDB fields contained
 * in HDMI 1.3
 *
 * Hydra_Software_Devel/82   2/8/07 12:16p rgreen
 * PR27746: Add 3563 EDID for debugging EDIDs containing HDMI 1.3 support
 *
 * Hydra_Software_Devel/81   1/29/07 6:52p vle
 * PR 26912: Ensure a valid supported video format is returned for
 * corrupted or badly formed EDID.
 *
 * Hydra_Software_Devel/80   1/17/07 8:07p vle
 * PR 20872: Abort parsing invalid EDID info and check for validation of
 * EDID before accessing EDID info.
 *
 * Hydra_Software_Devel/79   1/11/07 6:36p vle
 * PR 26877:
 * Force to alternate preferred format when 1366x768 is detected.
 * Added Dummy descriptor tag.
 *
 * Hydra_Software_Devel/78   12/21/06 2:59p rgreen
 * PR26531: Should use #if instead of #ifdef for compile flag
 * BDBG_DEBUG_BUILD
 *
 * Hydra_Software_Devel/77   10/20/06 11:28a rgreen
 * PR20526: Fix release code warning
 *
 * Hydra_Software_Devel/76   10/2/06 7:29p rgreen
 * PR24461: Always report PCM Audio Support for HDMI Rx
 *
 * Hydra_Software_Devel/75   9/20/06 9:53p rgreen
 * PR24304: Clean up debug output for EDID dump. Fix mislabeled vertical
 * parameters (lines vs pixels)
 *
 * Hydra_Software_Devel/74   9/14/06 8:01p rgreen
 * PR24291: Do not check for 1080p 60Hz Support
 *
 * Hydra_Software_Devel/73   7/13/06 11:26a rgreen
 * PR22588: Copy additional fields to Basic EDID data structure: Vendor
 * ID, Manufacturer week/year, feature support, and Serial Number
 *
 * Hydra_Software_Devel/72   6/22/06 2:34p rgreen
 * PR22104: Add BHDM_EDID_GetSupportedColorimetry function to return
 * preferred *and* supported format.  Use instead of BHDM_GetColorimetry
 *
 * Hydra_Software_Devel/71   6/12/06 5:14p rgreen
 * PR22104: Force HDMI RGB when YCrCb not supported; Update BHDM_EDID
 * debug messages
 *
 * Hydra_Software_Devel/70   5/26/06 4:16p rgreen
 * PR21621: enhance audio format debug messages to show format names vs
 * codes
 *
 * Hydra_Software_Devel/69   5/10/06 9:57a rgreen
 * PR21039: Use magnum naming conventions for macros BHDM_CEC_SUPPORT vs
 * HDMI_CEC_SUPPORT
 *
 * Hydra_Software_Devel/68   5/2/06 6:33p rgreen
 * PR21039: Update macros to magnum naming conventions. Fix CEC support
 * and DVO support macros
 *
 * Hydra_Software_Devel/67   4/21/06 4:23p rgreen
 * PR20871: Fix HDMI EDID Parsing for 1080p formats (various Hz)
 * Add debug mode for checking EDIDs
 *
 * Hydra_Software_Devel/66   4/7/06 5:50p rgreen
 * PR20721: Copy Cached EDID Block for return in BHDM_EDID_GetNthBlock
 *
 * Hydra_Software_Devel/65   3/23/06 7:55p rgreen
 * PR20327:
 * Add 7438 Compilation Support
 *
 * Add new compilation configuration macros to remove BCHP_XXX references
 * from source files
 * BHDM_CONFIG_DVO_7038BX_VERSION compilation macro
 * BHDM_CONFIG_88_2__176_4__192_SAMPLE_RATES
 *
 * Hydra_Software_Devel/64   2/28/06 3:30p rgreen
 * PR18147: Add support for HDMI 1080p/30 Hz
 *
 * Hydra_Software_Devel/63   2/8/06 5:00p rgreen
 * PR8896: Fix compilation warnings.
 *
 * Hydra_Software_Devel/62   2/3/06 4:08p rgreen
 * PR17778: Account for new frequencies for PC formats
 *
 * Hydra_Software_Devel/61   2/2/06 5:22p rgreen
 * PR19283,PR8896: Copy physical address to hdmi handle
 * Add debug info for PC formats used when searching EDIDs
 *
 * Hydra_Software_Devel/60   1/10/06 12:11p rgreen
 * PR16966: Fix compilation warning
 *
 * Hydra_Software_Devel/59   1/9/06 3:19p rgreen
 * PR 18790: Update HDM with C3 defines using new version method
 * PR 18394: Fix PC format found table for 1024x768 and 800x600
 *
 * Hydra_Software_Devel/58   12/5/05 10:37a rgreen
 * PR18394: HDMI connection fails with Sagem HD-L32 TV (PAL) monitor.
 * Indicate supported video formats have been checked if a) Detail Timing
 * is found, b) Established Timing is found, or c) CEA Version 3 Video
 * Block is found
 *
 * Hydra_Software_Devel/57   11/29/05 2:38p rgreen
 * PR16966: Fix "if" comparison when checking for video formats
 *
 * Hydra_Software_Devel/56   9/23/05 4:59p rgreen
 * PR17297: Relax EDID checking;
 *
 * Hydra_Software_Devel/55   9/14/05 6:33p rgreen
 * PR16538,PR12828: Prevent failure of bdisplay_set when display format
 * not supported by attached DVI/HDMI monitor.
 *
 * Implement HDMI AvMute in SettopAPI
 *
 * Hydra_Software_Devel/54   9/1/05 5:34p rgreen
 * PR16966: Relax EDID checking; fixes problems with EDIDs specified with
 * 50Hz formats and monitor vertical ranges of 59-61
 *
 ***************************************************************************/
#include "breg_endian.h"

#include "bhdm.h"
#include "bhdm_edid.h"
#include "bhdm_priv.h"
#include "bavc.h"

#if BHDM_CONFIG_HDMI_3D_SUPPORT
#include "bhdm_edid_3d.h"
#endif


/*=************************ Module Overview ********************************
  The EDID (Enhanced Display Identification Data) functions provide support
  for reading/interpretting the EDID prom contained in the DVI/HDMI Receiver.

  The PROM has an I2C address of 0xA0.  These functions support the Enhanced
  DDC protocol as needed.
***************************************************************************/


BDBG_MODULE(BHDM_EDID) ;


#define	BHDM_CHECK_RC( rc, func )	              \
do                                                \
{										          \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										      \
		goto done;							      \
	}										      \
} while(0)



/******************************************************************************
Summary:
Enumeration containing Audio Formats specified in CEA Short Audio Descriptors
*******************************************************************************/
typedef enum BHDM_EDID_P_AudioFormat
{
	BHDM_EDID_P_AudioFormat_eReserved,
	BHDM_EDID_P_AudioFormat_ePCM,
	BHDM_EDID_P_AudioFormat_eAC3,
	BHDM_EDID_P_AudioFormat_eMPEG1,
	BHDM_EDID_P_AudioFormat_eMP3,
	BHDM_EDID_P_AudioFormat_eMPEG2,
	BHDM_EDID_P_AudioFormat_eAAC,
	BHDM_EDID_P_AudioFormat_eDTS,
	BHDM_EDID_P_AudioFormat_eATRAC,
	BHDM_EDID_P_AudioFormat_eOneBit,
	BHDM_EDID_P_AudioFormat_eDDPlus,
	BHDM_EDID_P_AudioFormat_eDTSHD,
	BHDM_EDID_P_AudioFormat_eMATMLP,
	BHDM_EDID_P_AudioFormat_eDST,
	BHDM_EDID_P_AudioFormat_eWMAPro,
	BHDM_EDID_P_AudioFormat_eMaxCount
} BHDM_EDID_P_AudioFormat ;



/******************************************************************************
Summary:
Enumeration containing Sample Rates specified in CEA Short Audio Descriptors
*******************************************************************************/
typedef enum BHDM_EDID_P_AudioSampleRate
{
	BHDM_EDID_P_AudioSampleRate_e32KHz  =  1,
	BHDM_EDID_P_AudioSampleRate_e44KHz  =  2,
	BHDM_EDID_P_AudioSampleRate_e48KHz  =  4,
	BHDM_EDID_P_AudioSampleRate_e88KHz  =  8,
	BHDM_EDID_P_AudioSampleRate_e96KHz  = 16,
	BHDM_EDID_P_AudioSampleRate_e176KHz = 32,
	BHDM_EDID_P_AudioSampleRate_e192KHz = 64
} BHDM_EDID_P_AudioSampleRate;



typedef struct _BHDM_EDID_P_CEA_861B_VIDEO_FORMAT_
{
	uint8_t            CeaVideoCode ;      /* Short Descriptor */
	uint16_t           HorizontalPixels ;  /* Horiz Active Pixels */
	uint16_t           VerticalPixels ;	  /* Vertical Active Pixels */
	BAVC_ScanType      eScanType ;         /* Progressive, Interlaced */
	BAVC_FrameRateCode eFrameRateCode ;    /* Vertical Frequency */
	BFMT_AspectRatio   eAspectRatio ;      /* Horiz  to Vertical Ratio */
} BHDM_EDID_P_CEA_861B_VIDEO_FORMAT ;


const BHDM_EDID_P_CEA_861B_VIDEO_FORMAT BHDM_EDID_P_Cea861bFormats[] =
{
/* Video	Horizontal	Vertical */
/* Codes	(pixels)	(pixels)		  i/p	                Vertical Freq				Aspect Ratio			Remark */

	{ 1,	640,		480, 	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e4_3}, 	/* Default format */
	{ 2,	720,		480, 	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e4_3}, 	/* EDTV */
	{ 3,	720,		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* EDTV */
	{ 4,	1280,		720,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* HDTV */
	{ 5,	1920,		1080,	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* HDTV */
	{ 6,    /*720*/1440,	480,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e4_3}, 	/* Optional Double clock for 720x480i */
	{ 7, 	/*720*/1440,	480,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* Optional Double clock for 720x480i */
	{10,	2880,		480,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e4_3},		/* Game Console */
	{11,	2880,		480,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* Game Console */
	{14,	1440,		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e4_3},		/* high-end DVD */
	{15,	1440,		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* high-end DVD */
	{16,	1920,		1080,	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* Optional HDTV */

	{17,	720,		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e4_3},		/* EDTV */
	{18,	720,		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* EDTV */
	{19,	1280,		720,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* HDTV */
	{20,	1920,		1080,	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* HDTV */
	{21,	/*720*/1440,	576,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e4_3},		/* Optional Double clock for 720x576i */
	{22,	/*720*/1440,	576,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* Optional Double clock for 720x576i */
	{25,	2880,		576,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e4_3},		/* Game Console */
	{26,	2880,		576,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* Game Console */
	{29,	1440,		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e4_3},		/* high-end DVD */
	{30,	1440,		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* high-end DVD */
	{31,	1920,		1080,	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* Optional HDTV */

	{32,	1920,		1080,	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e23_976, BFMT_AspectRatio_e16_9},	/* Optional HDTV */
	{33,	1920,		1080,	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e25,     BFMT_AspectRatio_e16_9},	/* Optional HDTV */
	{34,	1920,		1080,	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e30,	BFMT_AspectRatio_e16_9},	       /* Optional HDTV */
	{35,	2880,		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e4_3},		/* 4x Pixel Rep */
	{36,	2880,		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* 4x Pixel Rep */
	{37,	2880,		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,   	BFMT_AspectRatio_e4_3},		/* 4x Pixel Rep */
	{38,	2880,		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,   	BFMT_AspectRatio_e16_9},	/* 4x Pixel Rep */

	{60,	1280,		720,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e24,   	BFMT_AspectRatio_e16_9},	
	{61,	1280,		720,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e25,   	BFMT_AspectRatio_e16_9},	
	{62,	1280,		720,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e30,   	BFMT_AspectRatio_e16_9},	

#ifdef UNSUPPORTED
	{ 8,	720(1440),	240,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e4_3},		/* Double clock for 720x240p */
	{ 9,	720(1440),	240,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* Double clock for 720x240p */
	{12,	(2880),		240,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e4_3},		/* Game Console */
	{13,	(2880),		240,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e59_94,	BFMT_AspectRatio_e16_9},	/* Game Console */
	{23,	720(1440),	288,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e4_3},		/* Double clock for 720x288p */
	{24,	720(1440),	288,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* Double clock for 720x288p */
	{27,	(2880),		288,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e4_3},		/* Game Console */
	{28,	(2880),		288,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e50,		BFMT_AspectRatio_e16_9},	/* Game Console */

	/* UnSupported Frame Rates */
	{39, 1920, 		1080, /*(1250 total)*/BAVC_ScanType_eInterlaced, BAVC_FrameRateCode_e50 BFMT_AspectRatio_e16_9},
	{40, 1920, 		1080,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e100, BFMT_AspectRatio_e16_9},
	{41, 1280, 		720,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e100, BFMT_AspectRatio_e16_9},
	{42, 720, 		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e100, BFMT_AspectRatio_e4_3},
	{43, 720, 		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e100, BFMT_AspectRatio_e16_9},
	{44, 720(1440), 	576,  	BAVC_ScanType_eInterlaced, 	BAVC_FrameRateCode_e100, BFMT_AspectRatio_e4_3}, 
	{45, 720(1440), 	576,  	BAVC_ScanType_eInterlaced, 	BAVC_FrameRateCode_e100, BFMT_AspectRatio_e16_9},
	{46, 1920, 		1080,  	BAVC_ScanType_eInterlaced, 	BAVC_FrameRateCode_e120, BFMT_AspectRatio_e16_9},
	{47, 1280, 		720,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e120, BFMT_AspectRatio_e16_9},
	{48, 720, 		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e120, BFMT_AspectRatio_e4_3},
	{49, 720, 		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e120, BFMT_AspectRatio_e16_9},
	{50, 720(1440), 	480,  	BAVC_ScanType_eInterlaced, 	BAVC_FrameRateCode_e120, BFMT_AspectRatio_e4_3},
	{51, 720(1440), 	480,  	BAVC_ScanType_eInterlaced, 	BAVC_FrameRateCode_e120, BFMT_AspectRatio_e16_9},
	{52, 720, 		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e200,  BFMT_AspectRatio_e4_3}, 
	{53, 720, 		576,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e200,  BFMT_AspectRatio_e16_9},
	{54, 720(1440), 	576,  	BAVC_ScanType_eInterlaced, 	BAVC_FrameRateCode_e200,  BFMT_AspectRatio_e4_3}, 
	{55, 720(1440), 	576,  	BAVC_ScanType_eInterlaced, 	BAVC_FrameRateCode_e200,  BFMT_AspectRatio_e16_9},
	{56, 720, 		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e240, BFMT_AspectRatio_e4_3}, 
	{57, 720, 		480,  	BAVC_ScanType_eProgressive,	BAVC_FrameRateCode_e240, BFMT_AspectRatio_e16_9},
	{58, 720(1440), 	480,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e240, BFMT_AspectRatio_e4_3}, 
	{59, 720(1440), 	480,  	BAVC_ScanType_eInterlaced,	BAVC_FrameRateCode_e240, BFMT_AspectRatio_e16_9},
	{63, 1920, 		1080,  	BAVC_ScanType_eProgressive, 	BAVC_FrameRateCode_e120, BFMT_AspectRatio_e16_9},
	{64, 1920, 		1080 	BAVC_ScanType_eProgressive, 	BAVC_FrameRateCode_e100 BFMT_AspectRatio_e16_9}, 
		
#endif

/*  0 No Video Code Available (Used with AVI InfoFrame only) */
/*	{35,-127 Reserved for the Future */
} ;


typedef struct _BHDM_EDID_P_ALT_PREFERRED_VIDEOFMT
{
	BFMT_VideoFmt format;
	char *formattext;
}BHDM_EDID_P_ALT_PREFERRED_VIDEOFMT;

const BHDM_EDID_P_ALT_PREFERRED_VIDEOFMT BHDM_EDID_P_AltPreferredFormats[] = {
	{BFMT_VideoFmt_e720p, "BFMT_VideoFmt_e720p"},
	{BFMT_VideoFmt_e1080i, "BFMT_VideoFmt_e1080i"},
	{BFMT_VideoFmt_e480p, "BFMT_VideoFmt_e480p"},
	{BFMT_VideoFmt_eNTSC, "BFMT_VideoFmt_eNTSC"},
	{BFMT_VideoFmt_eDVI_1024x768p, "BFMT_VideoFmt_eDVI_1024x768p"},
	{BFMT_VideoFmt_eDVI_640x480p, "BFMT_VideoFmt_eDVI_640x480p"}
};

#define BHDM_EDID_P_ALT_PREFERRED_VIDEOFMT_MAX \
	(sizeof(BHDM_EDID_P_AltPreferredFormats) / sizeof(BHDM_EDID_P_ALT_PREFERRED_VIDEOFMT))

#if BDBG_DEBUG_BUILD
	static const char Mode[3] = "pi"  ; /* 1080i 480P etc. for debug msg only */
#endif

#define BHDM_EDID_P_BCM_VIDEO_FORMATS_MAX \
	(sizeof(BHDM_EDID_P_Cea861bFormats) / sizeof(BHDM_EDID_P_CEA_861B_VIDEO_FORMAT))


static uint8_t BHDM_EDID_P_EdidCheckSum(uint8_t *pEDID) ;


static BERR_Code BHDM_EDID_P_ParseVideoDB(
	BHDM_Handle hHDMI,                   /* [in] HDMI handle  */
	uint8_t DataBlockIndex,              /* [in] start offset of Video Data Block */
	uint8_t DataBlockLength              /* [in] length (number) of Video ID codes */
) ;

static BERR_Code BHDM_EDID_P_ParseAudioDB(
	BHDM_Handle hHDMI,                   /* [in] HDMI handle  */
	uint8_t DataBlockIndex,              /* [in] start offset of Video Data Block */
	uint8_t DataBlockLength             /* [in] length (number) of Video ID codes */
) ;

static BERR_Code BHDM_EDID_P_ParseEstablishedTimingFormats(
	BHDM_Handle hHDMI) ;                  /* [in] HDMI handle  */

static BERR_Code BHDM_EDID_P_ParseV1V2TimingExtension(BHDM_Handle hHDMI) ;

static BERR_Code BHDM_EDID_P_ParseV3TimingExtension(BHDM_Handle hHDMI);

static BERR_Code BHDM_EDID_P_GetVerticalFrequency(
	uint32_t ulVertFreqMask,            /* [in] Vertical Frequency Mask (bfmt) */
	uint16_t *uiVerticalFrequency)  ;   /* [out] Vertical Frequency value */

static BERR_Code BHDM_EDID_P_ParseMonitorRange(
	BHDM_Handle hHDMI, uint8_t offset) ;

static BERR_Code BHDM_EDID_P_DetailTiming2VideoFmt(
	BHDM_Handle hHDMI,
	BHDM_EDID_DetailTiming *pBHDM_EDID_DetailTiming,
	BFMT_VideoFmt *Detail_VideoFmt) ;


#if BDBG_DEBUG_BUILD
static void BHDM_EDID_P_PrintEdid(uint8_t *pEDID) ;


static int _strlen(const char *s) {
	int i=0;
	while (*s++) i++;
	return i;
}

static const char *CeaTagName[] =
{
	"Reserved0  ",
	"Audio    DB",
	"Video    DB",
	"VendorSp DB",
	"Speaker  DB",
	"Reserved5  ",
	"Reserved6  ",
	"Extended DB"
} ;

static const char *g_status[] = {"No", "Yes"} ;

static const unsigned char EDIDByPassedText[] = "BYPASSED EDID" ;

#endif

#define BHDM_EDID_USE_DEBUG_EDID 0

#if BHDM_EDID_USE_DEBUG_EDID
#undef BHDM_CONFIG_DEBUG_EDID_PROCESSING
#define BHDM_CONFIG_DEBUG_EDID_PROCESSING 1
#endif

#if BHDM_CONFIG_DEBUG_EDID_PROCESSING
/*
the following EDID can be used to debug any Rx EDID that a BCMxxxx chip may have problems parsing.
a copy of the EDID must first be obtained from the Receiever and copied here;
Use BHDM_EDID_DumpRawEDID to get a copy of the EDID

NOTE: This following EDID is used for DEBUGGING ONLY! i.e. debugging the parsing of an EDID which
you may not have the HDMI Rx.  Presumeably the EDID was provided by the user with the TV.

*/

#if 1
uint8_t DebugRxEdid[] = /* BCM97403 Extended-EDID test blocks*/
{
	
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x48, 0x56, 0xA0, 0xA0, 0xA0, 0xA0,
	0x14, 0x14, 0x01, 0x03, 0x81, 0x5D, 0x34, 0x78, 0x0A, 0x55, 0x50, 0xA7, 0x55, 0x46, 0x99, 0x24,
	0x12, 0x49, 0x4B, 0x21, 0x08, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xE4, 0x57, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xA2, 0x08, 0x32, 0x00, 0x00, 0x1E, 0xC0, 0x2B, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20,
	0x6E, 0x28, 0x55, 0x00, 0xA2, 0x08, 0x32, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
	0x43, 0x4D, 0x33, 0x35, 0x34, 0x38, 0x5F, 0x35, 0x36, 0x20, 0x33, 0x44, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x32, 0x4B, 0x0F, 0x50, 0x17, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x25,
	
	0x02, 0x03, 0x3D, 0x71, 0x78, 0x03, 0x0C, 0x00, 0x10, 0x00, 0xB8, 0x2D, 0x20, 0xC0, 0x0E, 0x01,
	0x01, 0x0F, 0x3F, 0x08, 0x00, 0x20, 0x40, 0x56, 0x16, 0x90, 0x88, 0x00, 0xE6, 0x52, 0x05, 0x04,
	0x40, 0x02, 0x22, 0x20, 0x21, 0x1F, 0x13, 0x11, 0x14, 0x18, 0x15, 0x16, 0x1A, 0x07, 0x06, 0x01,
	0x29, 0x09, 0x07, 0x07, 0x15, 0x50, 0xFF, 0x19, 0x50, 0xFF, 0x82, 0x01, 0x00, 0xE4, 0x57, 0x80,
	0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0x13, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE
} ;


#else
uint8_t DebugRxEdid[] = /* BCM93563 EDID */
{

	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x63, 0x35, 0x01, 0x01, 0x01, 0x01,
	0x14, 0x11, 0x01, 0x03, 0x81, 0x5D, 0x34, 0x78, 0x0A, 0x55, 0x50, 0xA7, 0x55, 0x46, 0x99, 0x24,
	0x12, 0x49, 0x4B, 0x21, 0x08, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xA2, 0x08, 0x32, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20,
	0x6E, 0x28, 0x55, 0x00, 0xA2, 0x08, 0x32, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x39,
	0x33, 0x35, 0x36, 0x33, 0x5F, 0x31, 0x30, 0x38, 0x30, 0x70, 0x0A, 0x20, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x32, 0x4B, 0x0F, 0x50, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xB9,

	0x02, 0x03, 0x29, 0x71, 0x50, 0x90, 0x04, 0x05, 0x03, 0x20, 0x21, 0x22, 0x06, 0x13, 0x14, 0x11,
	0x15, 0x02, 0x07, 0x12, 0x16, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03, 0x0C,
	0x00, 0x10, 0x00, 0xB8, 0x2D, 0xE3, 0x05, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79

} ;
#endif


#endif
static const struct  {
	BHDM_EDID_P_AudioFormat EdidAudioFormat ;
	BAVC_AudioFormat BcmAudioFormat ;
} BHDM_EDID_P_BcmSupportedFormats [] =
{
	{BHDM_EDID_P_AudioFormat_ePCM,   BAVC_AudioFormat_ePCM},
	{BHDM_EDID_P_AudioFormat_eAC3,   BAVC_AudioFormat_eAC3},
	{BHDM_EDID_P_AudioFormat_eMPEG1, BAVC_AudioFormat_eMPEG1},
	{BHDM_EDID_P_AudioFormat_eMP3,   BAVC_AudioFormat_eMP3},
	{BHDM_EDID_P_AudioFormat_eMPEG2, BAVC_AudioFormat_eMPEG2},
	{BHDM_EDID_P_AudioFormat_eAAC,   BAVC_AudioFormat_eAAC},
	{BHDM_EDID_P_AudioFormat_eDTS,   BAVC_AudioFormat_eDTS}

#if BHDM_CONFIG_AUDIO_SUPPORT_DDP
	, {BHDM_EDID_P_AudioFormat_eDDPlus, BAVC_AudioFormat_eDDPlus}
#endif

#if BHDM_CONFIG_AUDIO_SUPPORT_DTSHD
	, {BHDM_EDID_P_AudioFormat_eDTSHD, BAVC_AudioFormat_eDTSHD}
#endif

#if BHDM_CONFIG_AUDIO_SUPPORT_MATMLP
	, {BHDM_EDID_P_AudioFormat_eMATMLP, BAVC_AudioFormat_eMATMLP}
#endif

#if BHDM_CONFIG_AUDIO_SUPPORT_WMAPRO
	, {BHDM_EDID_P_AudioFormat_eWMAPro, BAVC_AudioFormat_eWMAPro}
#endif

#if 0
	, {BHDM_EDID_P_AudioFormat_eATRAC, BAVC_AudioFormat_eATRAC}
	, {BHDM_EDID_P_AudioFormat_eOneBit, BAVC_AudioFormat_eOneBit}
	, {BHDM_EDID_P_AudioFormat_eDST,	 BAVC_AudioFormat_eDST}
#endif

 } ;

static const struct {
	BHDM_EDID_P_AudioSampleRate EdidAudioSampleRate ;
	BAVC_AudioSamplingRate BcmAudioSampleRate ;
} EdidAudioSampleRateTable[] =
{

	{BHDM_EDID_P_AudioSampleRate_e32KHz, BAVC_AudioSamplingRate_e32k},
	{BHDM_EDID_P_AudioSampleRate_e44KHz, BAVC_AudioSamplingRate_e44_1k},
	{BHDM_EDID_P_AudioSampleRate_e48KHz, BAVC_AudioSamplingRate_e48k},
	{BHDM_EDID_P_AudioSampleRate_e96KHz, BAVC_AudioSamplingRate_e96k}

#if BHDM_CONFIG_88_2KHZ_AUDIO_SUPPORT
	, {BHDM_EDID_P_AudioSampleRate_e88KHz,  BAVC_AudioSamplingRate_e88_2k}
#endif

#if BHDM_CONFIG_176_4KHZ_AUDIO_SUPPORT
	, {BHDM_EDID_P_AudioSampleRate_e176KHz, BAVC_AudioSamplingRate_e176_4k}
#endif

#if BHDM_CONFIG_192KHZ_AUDIO_SUPPORT
	, {BHDM_EDID_P_AudioSampleRate_e192KHz, BAVC_AudioSamplingRate_e192k}
#endif

} ;

#if BDBG_DEBUG_BUILD
static char *CeaAudioSampleRateTypeText[] =
{
	"32 kHz",
	"44.1 kHz (CD)",
	"48 kHz",
	"96 kHz"
#if BHDM_CONFIG_88_2KHZ_AUDIO_SUPPORT
	, "88.2 kHz"
#endif

#if BHDM_CONFIG_176_4KHZ_AUDIO_SUPPORT
	, "176.4 kHz"
#endif

#if BHDM_CONFIG_192KHZ_AUDIO_SUPPORT
	, "192 kHz"
#endif
} ;
#endif

/******************************************************************************
uint8_t BHDM_EDID_P_EdidCheckSum
Summary:Verify the checksum on an EDID block
*******************************************************************************/
uint8_t BHDM_EDID_P_EdidCheckSum(uint8_t *pEDID)
{
	uint8_t i ;
	uint8_t checksum = 0 ;


	for (i = 0 ; i < BHDM_EDID_BLOCKSIZE ; i++)
		checksum = checksum + (uint8_t) pEDID[i]  ;

#if BDBG_DEBUG_BUILD
	/*
	-- Determine Checksum if an error (non-zero)
	-- correct value can be inserted above if desired
	*/
	if (checksum % 256)
	{
		BDBG_WRN(("Checksum:      %#X", checksum)) ;
		BDBG_WRN(("Invalid Checksum Byte: %#02X", (unsigned char) pEDID[BHDM_EDID_CHECKSUM])) ;

		/* determine the correct checksum value */
		BDBG_WRN(("Correct Checksum Byte = %#02X",
			(uint8_t) (256 - (checksum - (unsigned int) pEDID[BHDM_EDID_CHECKSUM])))) ;
	}
#endif

	return ( (checksum % 256) ? 0 : 1) ;
} /* end BHDM_EDID_P_EdidCheckSum */



/******************************************************************************
BERR_Code BHDM_EDID_GetNthBlock
Summary:Retrieve the Nth 128-byte EDID block
*******************************************************************************/
BERR_Code BHDM_EDID_GetNthBlock(
   BHDM_Handle hHDMI,   /* [in] HDMI handle */
   uint8_t BlockNumber, /* [in] EDID Block Number to read */
   uint8_t *pBuffer,    /* [out] pointer to input buffer */
   uint16_t uiBufSize    /* [in] Size of input buffer to write EDID to */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t uiSegment ;
#if ! BHDM_CONFIG_DEBUG_EDID_PROCESSING
	uint8_t timeoutMs ;
#endif
	uint8_t RxDeviceAttached ;


	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		return BHDM_NO_RX_DEVICE ;
	}

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		return BHDM_EDID_NOT_FOUND;
	}

	/* check if the requested block already cached */
	if ((BlockNumber == hHDMI->AttachedEDID.CachedBlock)
	&& (hHDMI->edidStatus == BHDM_EDID_STATE_eOK))
	{
		BDBG_MSG(("Skip reading EDID; Block %d already cached", BlockNumber)) ;

		/* copy the cached EDID block to the user buffer */
		BKNI_Memcpy(pBuffer, hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE) ;

		rc = BERR_SUCCESS ;
		goto done ;
	}

	if (uiBufSize <  BHDM_EDID_BLOCKSIZE)
	{
		BDBG_ERR(("Incorrect Specified EDID Block Size: %d; expecting %d",
			uiBufSize, BHDM_EDID_BLOCKSIZE)) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}


	/* Use E-DDC protocol for reading the EDID... */

	/* each segment can hold two EDID blocks of 128 bytes each */
	/* determine which segment the requested block lies in */
	uiSegment = BlockNumber / 2 ;


#if BHDM_CONFIG_DEBUG_EDID_PROCESSING
	/* copy the EDID from the debugging EDID instead of the connected monitor */

	BDBG_WRN(("<$$$ BHDM_CONFIG_DEBUG_EDID_PROCESSING  $$$>")) ;
	BDBG_WRN(("<$$$ Using EDID declared in bdhm_edid.c $$$>")) ;
	
	BKNI_Memcpy(pBuffer,
		DebugRxEdid + (uint8_t) (BHDM_EDID_BLOCKSIZE * (BlockNumber % 2)) + (BHDM_EDID_BLOCKSIZE * 2 * uiSegment),
		BHDM_EDID_BLOCKSIZE) ;
#else
	/* Read the EDID block; sometimes the EDID block is not ready/available */
	/* try again if a READ failure occurs */

	
#if defined(HUMAX_PLATFORM_BASE)
	timeoutMs = 10 ;
#else
	timeoutMs = 1 ;
#endif
	
	do
	{
		rc = BREG_I2C_ReadEDDC(hHDMI->hI2cRegHandle, (uint8_t) BHDM_EDID_I2C_ADDR,
			(uint8_t) uiSegment,
			(uint8_t) (BHDM_EDID_BLOCKSIZE * (BlockNumber % 2)), /* offset */
			pBuffer, (uint16_t) BHDM_EDID_BLOCKSIZE) ;	/* storage & length  */

		if (rc == BERR_SUCCESS)
			break ;

		BDBG_WRN(("Error reading EDID Block; Attempt to re-read...")) ;
#if defined(HUMAX_PLATFORM_BASE)
		BKNI_Sleep(100) ;
#else
		BKNI_Sleep(1) ;
#endif

	} while ( timeoutMs-- ) ;

	if (rc != BERR_SUCCESS)
	{
		BDBG_ERR(("Can't find/read Rx EDID device")) ;
		rc = BHDM_NO_RX_DEVICE ;
		goto done ;
	}
#endif

	/* copy the EDID block to the EDID handle */
	BKNI_Memcpy(hHDMI->AttachedEDID.Block, pBuffer, BHDM_EDID_BLOCKSIZE) ;

	/* check for a valid checksum */
	if (!BHDM_EDID_P_EdidCheckSum(hHDMI->AttachedEDID.Block))
	{
		BDBG_WRN(("Checksum Error for EDID Block #%d Ignored", BlockNumber)) ;
#if 0
		rc = BHDM_EDID_CHECKSUM_ERROR ;
		goto done ;
#endif
	}

	hHDMI->AttachedEDID.CachedBlock = BlockNumber ;
	hHDMI->edidStatus = BHDM_EDID_STATE_eOK;

done:
	return rc ;
} /* end BHDM_EDID_GetNthBlock */


#if BDBG_DEBUG_BUILD
/******************************************************************************
BERR_Code BHDM_EDID_P_PrintEdid
Summary: Print EDID Messages
*******************************************************************************/
void BHDM_EDID_P_PrintEdid(uint8_t *pEDID)
{
	uint8_t i ;
	uint8_t display_string[128] = "" ;
	uint8_t *ptr ;

	i = 0 ;
	do
	{
		if ((i % BHDM_EDID_BLOCKSIZE == 0) /* add a blank line between blocks*/
		&&  (i != 0)
		&&  (i != BHDM_EDID_BLOCKSIZE))
			BDBG_MSG(("")) ;

		if ((i % 16 == 0) )                /* 16 bytes per line */
		{
			BDBG_MSG(("%s", display_string)) ;
			BKNI_Snprintf((char *) display_string, BHDM_EDID_BLOCKSIZE, "%04X> ", i) ;
		}

		ptr = display_string + _strlen((char *) display_string) ;
		BKNI_Snprintf((char *) ptr, BHDM_EDID_BLOCKSIZE, "%02X ", pEDID[i]) ;
		i++ ;
	} while (i < BHDM_EDID_BLOCKSIZE) ;

	/* pirint last line */
	BDBG_MSG(("%s", display_string)) ;


	BDBG_MSG(("")) ;
}



/******************************************************************************
BERR_Code BHDM_EDID_DumpRawEDID
Summary: Dump EDID data
*******************************************************************************/
BERR_Code BHDM_EDID_DumpRawEDID(BHDM_Handle hHDMI)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t NumExtensions ;
	uint8_t i ;

	/* read the 1st EDID Block */
	BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI,
		0, hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;

	BDBG_MSG(("RAW EDID DUMP: ")) ;

	BHDM_EDID_P_PrintEdid( hHDMI->AttachedEDID.Block ) ;

	NumExtensions = hHDMI->AttachedEDID.BasicData.Extensions;
	/* Search EDID Extension blocks for additional timing descriptors */
	for (i = 1 ; i <= NumExtensions; i++)
	{
		BDBG_MSG(("")) ;
		BDBG_MSG(("EXTENSION BLOCK %d", i)) ;

		/* read the next 128 Byte EDID block */
		BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI, i,
			hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;

		BHDM_EDID_P_PrintEdid( hHDMI->AttachedEDID.Block ) ;
	}

done :
	return rc ;
}
#else
void BHDM_EDID_P_PrintEdid(uint8_t *pEDID)
{
	BSTD_UNUSED(pEDID);
}
BERR_Code BHDM_EDID_DumpRawEDID(BHDM_Handle hHDMI)
{
	BSTD_UNUSED(hHDMI);
	return BERR_SUCCESS;
}
#endif




/******************************************************************************
BERR_Code BHDM_EDID_GetBasicData
Summary: Retrieve the basic EDID data
*******************************************************************************/
BERR_Code BHDM_EDID_GetBasicData(
   BHDM_Handle hHDMI,               /* [in] HDMI handle */
   BHDM_EDID_BasicData *pMonitorData /* [out] pointer to structure to hold Basic
                                            EDID Data */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t RxDeviceAttached ;

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		return BHDM_NO_RX_DEVICE ;
	}

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		return BHDM_EDID_NOT_FOUND;
	}

	/* copy the EDID Basic Data */
	BKNI_Memcpy(pMonitorData, (void *) &(hHDMI->AttachedEDID.BasicData),
		sizeof(BHDM_EDID_BasicData)) ;

done:
	return rc ;
}/* end BHDM_EDID_GetBasicData */



/******************************************************************************
static void BHDM_EDID_ParseDetailedTimingDescriptor
Summary: Parse a Detailed Timing Descriptor Block
*******************************************************************************/
static void BHDM_EDID_ParseDetailedTimingDescriptor
	(unsigned char *pDescriptor, BHDM_EDID_DetailTiming *DetailedTiming)
{

	BDBG_ENTER(BHDM_EDID_ParseDetailedTimingDescriptor) ;

	DetailedTiming->PixelClock =
    	( ((pDescriptor[BHDM_EDID_DESC_PIXEL_CLOCK_HI]) << 8)
		| ((pDescriptor[BHDM_EDID_DESC_PIXEL_CLOCK_LO]))) / 100 ;


	DetailedTiming->HorizActivePixels =
		    pDescriptor[BHDM_EDID_DESC_HACTIVE_PIXELS_LSB]
		| ((pDescriptor[BHDM_EDID_DESC_HACTIVE_PIXELS_UN_F0] & 0xF0) << 4) ;

	DetailedTiming->HorizBlankingPixels =
		pDescriptor[ BHDM_EDID_DESC_HBLANK_PIXELS_LSB]
		| (((pDescriptor[BHDM_EDID_DESC_HBLANK_PIXELS_UN_0F] & 0x0F) << 8)) ;



	DetailedTiming->VerticalActiveLines =
		    pDescriptor[BHDM_EDID_DESC_VACTIVE_LINES_LSB]
		| ((pDescriptor[BHDM_EDID_DESC_VACTIVE_LINES_UN_F0] & 0xF0) << 4) ;

	DetailedTiming->VerticalBlankingLines =
		pDescriptor[BHDM_EDID_DESC_VBLANK_LINES_LSB]
		| (((pDescriptor[BHDM_EDID_DESC_VBLANK_LINES_UN_0F] & 0x0F) << 8)) ;

	/* Mode */
	DetailedTiming->Mode =
		(pDescriptor[BHDM_EDID_DESC_PREFERRED_FLAGS] & 0x80) ? 1 : 0 ;


	/* HSync Offset / Width */
	DetailedTiming->HSyncOffset =
		   pDescriptor[BHDM_EDID_DESC_HSYNC_OFFSET_LSB]
		| (pDescriptor[BHDM_EDID_DESC_HSYNC_OFFSET_U2_C0] & 0xC0) << 2 ;

	DetailedTiming->HSyncWidth =
		   pDescriptor[BHDM_EDID_DESC_HSYNC_WIDTH_LSB]
		| (pDescriptor[BHDM_EDID_DESC_HSYNC_WIDTH_U2_30] & 0x30) << 4 ;


	/* VSync Offset / Width */
	DetailedTiming->VSyncOffset =
		  ((pDescriptor[BHDM_EDID_DESC_VSYNC_OFFSET_LN_F0] & 0xF0) >> 4)
		| ((pDescriptor[BHDM_EDID_DESC_VSYNC_OFFSET_U2_0C] & 0x0C) << 2) ;

	DetailedTiming->VSyncWidth =
		   (pDescriptor[BHDM_EDID_DESC_VSYNC_WIDTH_LN_0F] & 0x0F)
		| ((pDescriptor[BHDM_EDID_DESC_VSYNC_WIDTH_U2_03] & 0x03) << 4) ;

	/*Screen Size */
	DetailedTiming->HSize_mm =
		    pDescriptor[BHDM_EDID_DESC_HSIZE_MM_LSB]
		| ((pDescriptor[BHDM_EDID_DESC_HSIZE_UN_F0] & 0xF0) << 4) ;

	DetailedTiming->VSize_mm =
		    pDescriptor[BHDM_EDID_DESC_VSIZE_MM_LSB]
		| ((pDescriptor[BHDM_EDID_DESC_VSIZE_UN_0F] & 0x0F) << 8) ;

	DetailedTiming->HorizBorderPixels = pDescriptor[BHDM_EDID_DESC_HBORDER_PIXELS] ;
	DetailedTiming->VerticalBorderLines = pDescriptor[BHDM_EDID_DESC_VBORDER_LINES] ;

	BDBG_LEAVE(BHDM_EDID_ParseDetailedTimingDescriptor) ;
	return ;
} /* END BHDM_EDID_ParseDetailedTimingDescriptor */



/******************************************************************************
BERR_Code BHDM_EDID_P_DetailTiming2VideoFmt(
Summary: Determine the BFMT_VideoFmt for the associated Detailed Timing Format
*******************************************************************************/
static BERR_Code BHDM_EDID_P_DetailTiming2VideoFmt(
	BHDM_Handle hHDMI,
	BHDM_EDID_DetailTiming *pBHDM_EDID_DetailTiming,
	BFMT_VideoFmt *Detail_VideoFmt
)
{
	BERR_Code rc = BHDM_EDID_DETAILTIMING_NOT_SUPPORTED ;
	const BFMT_VideoInfo *pVideoFormatInfo ;
	BFMT_VideoFmt eVideoFmt ;

	uint8_t i ;
	bool Interlaced ;
	uint16_t
		uiVerticalFrequency ;
	uint16_t AdjustedHorizPixels ;
	uint16_t AdjustedVerticalLines ;

	/* default to NTSC format */
	*Detail_VideoFmt = BFMT_VideoFmt_eDVI_640x480p  ;

	/* adjust Detailed Timing vertical parameters for interlaced formats */
	/* Do not adjust the original number of pixels in the Detail Timing  */
	Interlaced = pBHDM_EDID_DetailTiming->Mode ? true : false ;
	if (Interlaced)
		AdjustedVerticalLines = pBHDM_EDID_DetailTiming->VerticalActiveLines * 2 ;
	else
		AdjustedVerticalLines = pBHDM_EDID_DetailTiming->VerticalActiveLines ;

	/* adjust any pixel repititions to match the formats */
	/* as specified in BFMT (e.g 1440 -> 720) */

	if ((pBHDM_EDID_DetailTiming->HorizActivePixels == 1440)
	||  (pBHDM_EDID_DetailTiming->HorizActivePixels == 2880))
		AdjustedHorizPixels = 720 ;
	else
		AdjustedHorizPixels = pBHDM_EDID_DetailTiming->HorizActivePixels ;


	for (i = 0; i < BFMT_VideoFmt_eMaxCount; i++)
	{
		eVideoFmt = (BFMT_VideoFmt) i ;

		pVideoFormatInfo = BFMT_GetVideoFormatInfoPtr(eVideoFmt) ;
		if (!pVideoFormatInfo)
			continue;

		/* check if the specified parameters match the requested formats */

		/* 1st, Check if Pixel Format matches */
		if ((pVideoFormatInfo->ulDigitalWidth != AdjustedHorizPixels)
		|| (pVideoFormatInfo->ulDigitalHeight != AdjustedVerticalLines))
			continue ;

		/* 2nd, Check Scan Type (i/p) */
		if (pVideoFormatInfo->bInterlaced != Interlaced)
			continue ;

		
		/* 3rd, for all 720p and 1080i,p formats, make another
		check to help differentiate the 24, 50, and 60Hz formats.
		This is needed for Detailed Timing Descriptors because the 
		format width is the same so the blanking time must also be considered 
		when matching formats */

		if ((pVideoFormatInfo->ulDigitalWidth == BFMT_1080I_WIDTH) 
		|| (pVideoFormatInfo->ulDigitalWidth == BFMT_1080P_WIDTH) 
		|| (pVideoFormatInfo->ulDigitalWidth == BFMT_720P_WIDTH))
		{
			uint16_t ulHorizontalScanWidth ;

			ulHorizontalScanWidth =
				   pBHDM_EDID_DetailTiming->HorizActivePixels 
				+ pBHDM_EDID_DetailTiming->HorizBlankingPixels ;

			if (pVideoFormatInfo->ulScanWidth != ulHorizontalScanWidth)
				continue ;
		}

		/* use the BFMT freqency parameter to get the Vertical Frequency */
		/* Ignore rc; default of 60Hz will be used for unknown Frequency */
		BHDM_EDID_P_GetVerticalFrequency(
			pVideoFormatInfo->ulVertFreqMask, &uiVerticalFrequency) ;

#if !defined(BHDM_CONFIG_1080P_5060HZ_SUPPORT)
		if ((eVideoFmt == BFMT_VideoFmt_e1080p)
		&& (uiVerticalFrequency >= 50))
		{
			/* 1080p 60hz */
			/* assign format but return unsupported */

			*Detail_VideoFmt = eVideoFmt ;
			rc = BHDM_EDID_DETAILTIMING_NOT_SUPPORTED ;
			break ;
		}
#endif

		/* 3rd check the vertical frequency range */
		if  ((uiVerticalFrequency >= hHDMI->AttachedEDID.MonitorRange.MinVertical)
		&&  (uiVerticalFrequency <= hHDMI->AttachedEDID.MonitorRange.MaxVertical))
		{
#if 0
			/* debug Vertical Frequency */
			BDBG_MSG(("Vertical Frequency %d <= %d <=%d",
				hHDMI->AttachedEDID.MonitorRange.MinVertical,
				uiVerticalFrequency,
				hHDMI->AttachedEDID.MonitorRange.MaxVertical));
#endif
			BDBG_MSG(("   %s (#%d) %4d x %4d %c %dHz",
				pVideoFormatInfo->pchFormatStr, i,
				pVideoFormatInfo->ulDigitalWidth, pVideoFormatInfo->ulDigitalHeight,
				Mode[pVideoFormatInfo->bInterlaced], uiVerticalFrequency)) ;

			/* found associated BFMT_eVideoFmt */
			*Detail_VideoFmt = eVideoFmt ;
			rc = BERR_SUCCESS ;
			break ;
		}
		else
		{
			BDBG_MSG(("   Format %s does not match ", pVideoFormatInfo->pchFormatStr)) ;
		}
	}

	return rc ;
}


/******************************************************************************
BERR_Code BHDM_EDID_GetDetailTiming
Summary: Retrieve EDID Detail Timing Data
*******************************************************************************/
BERR_Code BHDM_EDID_GetDetailTiming(
	BHDM_Handle hHDMI,			/* [in] HDMI handle */
	uint8_t NthTimingRequested, /* [in] number of the Detail Block to get */
	BHDM_EDID_DetailTiming
		*pBHDM_EDID_DetailTiming, /* [out] pointer to a BHDM_EDID_DetailTiming
								   structure to hold the data */
	BFMT_VideoFmt *BCM_VideoFmt
)
{
	/* the first 128 bytes of the EDID should contain the preferred
	 * timing block.  It should be in the first Descriptor Block...
	 */

	/*
	 *	LISTED HERE FOR DOCUMENTATION PURPOSES ONLY...
	 *
	 *  By searching for the following DESCRIPTOR BLOCKS
	 *  EDID information (Name, S/N etc) can be gathered from the
	 *  Display Device; the search and retrieval of such data
	 *  could be implemented in this function
	 *
	 *	static DESCRIPTOR_BLOCK TimingDescriptors[] =
	 *	{
	 *		{0x00, 0x00, 0x00, 0xFF, 0x00}, -- Monitor Serial Number
	 *		{0x00, 0x00, 0x00, 0xFE, 0x00}, -- ASCII String
	 *		{0x00, 0x00, 0x00, 0xFD, 0x00}, -- Monitor Range Limits
	 *		{0x00, 0x00, 0x00, 0xFC, 0x00}, -- Monitor Name (ASCII)
	 *		{0x00, 0x00, 0x00, 0xFB, 0x00}, -- Color Point Data
	 *		{0x00, 0x00, 0x00, 0xFA, 0x00}, -- Additional Standard Timings
	 *				                        -- F9 - 11 Unused
	 *			                            -- 0F - 00 Manufacturer Descriptor
	 *		{0x00, 0x00, 0x00, 0x10, 0x00}, -- Dummy Descriptor
	 *	} ;
	 */

	BERR_Code rc = BERR_SUCCESS ;

	uint8_t i, j ;
	uint8_t offset ;
	uint8_t MaxDescriptors ;
	uint8_t extensions, DataOffset ;
	uint8_t NumDetailedTimingsFound = 0 ;
	uint8_t RxDeviceAttached ;

	BDBG_ENTER(BHDM_EDID_GetDetailTiming) ;

	*BCM_VideoFmt = BFMT_VideoFmt_eNTSC; /* initialize the out param for error path */

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
		return BHDM_NO_RX_DEVICE ;

#if BDBG_DEBUG_BUILD
	if (hHDMI->DeviceSettings.BypassEDIDChecking)
	{
		BDBG_WRN(("EDID is ByPassed; All formats will attempt to display")) ;
		goto done ;
	}
#endif

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid)
	{
		rc = BHDM_EDID_NOT_FOUND;
		goto done;
	}


	if (!NthTimingRequested)
	{
		rc = BERR_INVALID_PARAMETER ;
		BDBG_ERR(("Incorrectly specified Detail Timing: %d", NthTimingRequested)) ;
		goto done ;
	}

	/* set Number of Detailed Timings Found to 2; number in 1st Block */
	NumDetailedTimingsFound = hHDMI->AttachedEDID.SupportedDetailTimingsIn1stBlock ;

	/* up to two preferred formats should be stored in handle; if requested copy and exit */
	if ((NumDetailedTimingsFound) && (NthTimingRequested <= NumDetailedTimingsFound))
	{
		BKNI_Memcpy(pBHDM_EDID_DetailTiming,
			&(hHDMI->AttachedEDID.SupportedDetailTimings[NthTimingRequested - 1]),
			sizeof(BHDM_EDID_DetailTiming)) ;

		/* function below displays just the BFMT name; parse/display done at Initialize */
		rc = BHDM_EDID_P_DetailTiming2VideoFmt(hHDMI, pBHDM_EDID_DetailTiming, BCM_VideoFmt);
		if (rc == BERR_SUCCESS) {
			goto BcmSupportedFormatFound;
		}
		else {
			goto BcmSupportedFormatNotFound;
		}
	}

	/* otherwise search the extensions for the requested format */
	extensions = hHDMI->AttachedEDID.BasicData.Extensions ;
	if (!extensions)
		goto BcmSupportedFormatNotFound;

	/* Search EDID Extension blocks for additional timing descriptors */
	for (i = 1 ; i <= extensions; i++)
	{
		/* read the next 128 Byte EDID block */
		BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI, i, hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;

		/* check Extension Tag type for Timing Data */
		offset = 0 ;
		if (hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_TAG] != BHDM_EDID_EXT_TIMING_DATA)
			continue ;

		/* check if data blocks exist before the 18 Byte Detailed Timing data */
		DataOffset = hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_DATA_OFFSET] ;
		if (DataOffset == 0) /* no timing info in the block */
		{
			BDBG_MSG(("EDID Ext contains no Detail Timing Descriptors")) ;
			continue ;          /* continue to the next Timing Extension */
		}

		/* determine the number of timing data */
		switch (hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_VERSION])
		{
		case 0x01 :  /* Version 1 See CEA-861        */
			MaxDescriptors = 6 ;
			break ;

		case 0x02 :  /* Version 2 See CEA-861 A Spec */
		case 0x03 :  /* Version 3 See CEA-861 B Spec */
			MaxDescriptors = hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_MONITOR_SUPPORT] ;
			MaxDescriptors = MaxDescriptors & 0x0F ;
			MaxDescriptors = MaxDescriptors - NumDetailedTimingsFound ;

			break ;

		default :
			/* Check all blocks regardless of */
			/* the version of the EDID Extension */
			BDBG_MSG(("Timing Extension Version '%d' Not Supported",
				hHDMI->AttachedEDID.Block[offset+1])) ;
			rc = BHDM_EDID_EXT_VERSION_NOT_SUPPORTED ;
			goto BcmSupportedFormatNotFound;
		}

		BDBG_MSG(("EDID Extension #%d contains additional Timing Data", i)) ;


		/* skip start of EDID Extension Block */
		offset = offset + hHDMI->AttachedEDID.Block[offset + 2] ;

		for (j = 0 ; j < MaxDescriptors; j++)
		{
			/* advance to next descriptor in the extension */
			offset = offset + 18 * j ;

			/* skip any Detailed Timing blocks used as descriptors */
			/* Descriptor Blocks begin with 0x00 0x00 0x00...      */
			/* Timing Blocks do not...					           */

#ifdef DEBUG_DESCRIPTOR
			BDBG_MSG(("COMPARE %02X %02X %02X %02X %02X ",
				hHDMI->AttachedEDID.Block[offset],
				hHDMI->AttachedEDID.Block[offset+1],hHDMI->AttachedEDID.Block[offset+2],
				hHDMI->AttachedEDID.Block[offset+3], hHDMI->AttachedEDID.Block[offset+4])) ;
			BDBG_MSG(("COMPARE %02X %02X %02X %02X %02X ",
				hHDMI->AttachedEDID.DescriptorHeader[0],
				hHDMI->AttachedEDID.DescriptorHeader[1], hHDMI->AttachedEDID.DescriptorHeader[2],
				hHDMI->AttachedEDID.DescriptorHeader[3], hHDMI->AttachedEDID.DescriptorHeader[4])) ;

			BDBG_MSG(("Check Timing Block #%d of possible %d ", j+1, MaxDescriptors)) ;
			BDBG_MSG(("DescriptorHeader %02X %02X %02X %02X %02X",
				hHDMI->AttachedEDID.Block[offset],
				hHDMI->AttachedEDID.Block[offset+1], 	hHDMI->AttachedEDID.Block[offset+2],
				hHDMI->AttachedEDID.Block[offset+3], 	hHDMI->AttachedEDID.Block[offset+4])) ;
#endif

			/* skip EDID descriptor blocks */
			if (BKNI_Memcmp(&hHDMI->AttachedEDID.Block[offset], (void *) &hHDMI->AttachedEDID.DescriptorHeader, 3) == 0)
				continue ;


			/* stop searching if the Requested Timing is found */
			if (++NumDetailedTimingsFound == NthTimingRequested)
			{
				BHDM_EDID_ParseDetailedTimingDescriptor(
					&hHDMI->AttachedEDID.Block[offset], pBHDM_EDID_DetailTiming ) ;

				BDBG_MSG(("Detailed Timing #%d found in EDID Extension #%d, Descriptor Block #%d",
					NthTimingRequested, i, j+1)) ;

				rc = BHDM_EDID_P_DetailTiming2VideoFmt(hHDMI, pBHDM_EDID_DetailTiming, BCM_VideoFmt);
				if (rc != BERR_SUCCESS)
					goto BcmSupportedFormatNotFound;

				break ;  /* falls to BcmSupportedFormatFound: label */
			}
		}
	}



BcmSupportedFormatFound:
	if (*BCM_VideoFmt == BFMT_VideoFmt_eCUSTOM_1366x768p
	|| *BCM_VideoFmt == BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz
#if BHDM_CONFIG_1366_FORMAT_CHECK
	|| *BCM_VideoFmt == BFMT_VideoFmt_eDVI_1366x768p_60Hz
	|| *BCM_VideoFmt == BFMT_VideoFmt_eDVI_1360x768p_60Hz
#endif
	)
	{
		BFMT_VideoFmt eVideoFmt ;
		const BFMT_VideoInfo *pVideoFormatInfo  ;

#define BHDM_EDID_P_OVERRIDE_FORMATS 2		
		BFMT_VideoFmt OverrideFormats[BHDM_EDID_P_OVERRIDE_FORMATS] =
		{
			BFMT_VideoFmt_e720p, 
			BFMT_VideoFmt_e720p_50Hz
		} ;

 		for (i = 0 ; i < BHDM_EDID_P_OVERRIDE_FORMATS ; i++)
 		{
			eVideoFmt = OverrideFormats[i] ;

			if (hHDMI->AttachedEDID.BcmSupportedVideoFormats[eVideoFmt])
			{
				pVideoFormatInfo = BFMT_GetVideoFormatInfoPtr(eVideoFmt) ;
				BDBG_MSG(("Overriding DetailTiming #%d (1366x768p/1360x768p) to %s",
					NthTimingRequested, pVideoFormatInfo->pchFormatStr));
				BSTD_UNUSED(pVideoFormatInfo) ; /* supress coverity message for non-debug builds */
				*BCM_VideoFmt = eVideoFmt ;
				goto done ;
			}
 		}

		/*	else fall to BcmSupportedFormatNotFound: label	*/
		goto BcmSupportedFormatNotFound;
	}
	else if (BFMT_IS_VESA_MODE(*BCM_VideoFmt))
	{
		*BCM_VideoFmt = BFMT_VideoFmt_eDVI_640x480p ;
		BDBG_WRN(("Requested Detail Timing #%d is a PC format; Override with HDMI supported VGA\n\n",
			NthTimingRequested)) ;
	}
	
	goto done ;


BcmSupportedFormatNotFound:
	rc = BHDM_EDID_DETAILTIMING_NOT_SUPPORTED;
	/* Detailed Timing Request Not Found */
	BDBG_WRN(("Requested Detailed Timing %d NOT SUPPORTED - Detailed Timings found: %d",
		NthTimingRequested, NumDetailedTimingsFound)) ;
	for (i=0; i<BHDM_EDID_P_ALT_PREFERRED_VIDEOFMT_MAX; i++) {
		if (hHDMI->AttachedEDID.BcmSupportedVideoFormats[BHDM_EDID_P_AltPreferredFormats[i].format])
		{
			BDBG_WRN(("Overriding to %s format", BHDM_EDID_P_AltPreferredFormats[i].formattext));
			*BCM_VideoFmt = BHDM_EDID_P_AltPreferredFormats[i].format;
			break;
		}
	}


done:
	BDBG_LEAVE(BHDM_EDID_GetDetailTiming) ;

	return rc ;
}


/******************************************************************************
BERR_Code BHDM_EDID_GetVideoDescriptor
Summary: Retrieve EDID Detail Timing Data
*******************************************************************************/
BERR_Code BHDM_EDID_GetVideoDescriptor(
	BHDM_Handle hHDMI,			 /* [in] HDMI handle */
	uint8_t NthIdCode,           /* [in] number of the Video ID Code to get */
	uint8_t *VideoIdCode,        /* [out] 861B Video ID Code  */
	BFMT_VideoFmt *BCM_VideoFmt, /* [out] associated BFMT */
	uint8_t *NativeFormat        /* Native Monitor Format */
)
{
	BERR_Code rc = BERR_SUCCESS ;

	uint8_t i ;
	uint8_t RxDeviceAttached ;
	BHDM_EDID_P_VideoDescriptor  *pVideoDescriptor ;

	BDBG_ENTER(BHDM_EDID_GetVideoDescriptor) ;

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
		return BHDM_NO_RX_DEVICE ;

#if BDBG_DEBUG_BUILD
	if (hHDMI->DeviceSettings.BypassEDIDChecking)
	{
		BDBG_WRN(("EDID is ByPassed; All formats will attempt to display")) ;
		goto done ;
	}
#endif

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid)
	{
		rc = BHDM_EDID_NOT_FOUND;
		goto done;
	}

	if (!hHDMI->AttachedEDID.RxHasHdmiSupport)
	{
		rc = BHDM_EDID_HDMI_NOT_SUPPORTED ;
		goto done ;
	}

	if (!NthIdCode)
	{
		rc = BERR_INVALID_PARAMETER ;
		BDBG_ERR(("Incorrectly specified Id Code: %d", NthIdCode)) ;
		goto done ;
	}

	i = 1 ;
	for( pVideoDescriptor = BLST_Q_FIRST(&hHDMI->AttachedEDID.VideoDescriptorList);
			pVideoDescriptor ; pVideoDescriptor = BLST_Q_NEXT(pVideoDescriptor, link))
	{
		if (i++ == NthIdCode)
		{
			*VideoIdCode  = (uint8_t) pVideoDescriptor->VideoIdCode ;
			*BCM_VideoFmt = (BFMT_VideoFmt) pVideoDescriptor->eVideoFmt  ;
			*NativeFormat = (uint8_t) pVideoDescriptor->NativeFormat ;

			BDBG_MSG(("VICn: %d; BCM_Fmt: %d; Native Fmt: %d",
				*VideoIdCode, *BCM_VideoFmt, *NativeFormat)) ;
			goto done ;
		}
	}

done:
	BDBG_LEAVE(BHDM_EDID_GetVideoDescriptor) ;

	return rc ;
}
/*
*** BASE_HDMI: Attached Monitor VESA Formats Only
--- BHDM_EDID: 2 VICn: 6; BCM_Fmt: 26; Monitor Native Format: 0
*/

/******************************************************************************
BERR_Code BHDM_EDID_GetDescriptor
Summary: Retrieve a specified EDID descriptor
*******************************************************************************/
BERR_Code BHDM_EDID_GetDescriptor(
   BHDM_Handle hHDMI, /* [in] HDMI handle */
   BHDM_EDID_Tag tag, /* [in] id of the descriptor tag to retrieve */
   uint8_t *pDescriptorText, /* [out] pointer to memory to hold retrieved tag data */
   uint8_t uiBufSize         /* [in ] mem size in bytes of pDescriptorText */
)
{
	uint8_t TagId ;
	uint8_t i, j ;
	uint8_t offset ;
	uint8_t extensions ;
	uint8_t MaxDescriptors ;
	uint8_t RxDeviceAttached ;

	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDM_EDID_GetDescriptor) ;

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		return BHDM_NO_RX_DEVICE ;
	}

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid)
	{
		rc = BHDM_EDID_NOT_FOUND;
		goto done;
	}


#if BDBG_DEBUG_BUILD
	if (hHDMI->DeviceSettings.BypassEDIDChecking)
	{
		BKNI_Memcpy(pDescriptorText, EDIDByPassedText,
			BHDM_EDID_MONITOR_DESC_SIZE - BHDM_EDID_DESC_HEADER_LEN) ;
		goto done ;
	}
#endif

	if ((!uiBufSize)
	||  (uiBufSize > BHDM_EDID_MONITOR_DESC_SIZE)
	||  (uiBufSize < BHDM_EDID_MONITOR_DESC_SIZE - BHDM_EDID_DESC_HEADER_LEN))
	{
		BDBG_ERR(("Incorrect Specified Descriptor Length: %d", uiBufSize)) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}


	/* check for valid tag */
	switch (tag)
	{
	case BHDM_EDID_Tag_eMONITOR_NAME  :
		/* monitor name should have been read at EDID_Initialize */
		if (hHDMI->AttachedEDID.MonitorName[0] == 0x00)
		{
			BKNI_Memcpy(pDescriptorText, &hHDMI->AttachedEDID.MonitorName,
				BHDM_EDID_MONITOR_DESC_SIZE - BHDM_EDID_DESC_HEADER_LEN) ;

			rc = BERR_SUCCESS ;  /* Descriptor Found and Copied */
			goto done ;
		}
		TagId = BHDM_EDID_TAG_MONITOR_NAME ;
		break ;

	case BHDM_EDID_Tag_eMONITOR_ASCII :
		TagId = BHDM_EDID_TAG_MONITOR_ASCII ;
		break ;

	case BHDM_EDID_Tag_eMONITOR_SN    :
		TagId = BHDM_EDID_TAG_MONITOR_SN ;
		break ;

	default :
		BDBG_ERR(("Invalid Descriptor Tag: %d", tag)) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}

	/* Insert the Tag we are searching for in the Descriptor Header */
	BKNI_Memset((void *) &hHDMI->AttachedEDID.DescriptorHeader, 0x0, BHDM_EDID_DESC_HEADER_LEN);
	hHDMI->AttachedEDID.DescriptorHeader[BHDM_EDID_DESC_TAG] = TagId ;

	/* read the 1st EDID Block */
	BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI,
		0, hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;

	/* Check the four Descriptor Blocks in the initial 128 EDID  bytes */
	for (i = 0 ; i < 4; i++)   /* 1-4 Detailed Timing Descriptor */
	{
		offset = BHDM_EDID_MONITOR_DESC_1 + BHDM_EDID_MONITOR_DESC_SIZE * i ;

		/*
		** Check if we've found the Descriptor tag we're looking for
		** Descriptor Blocks begin with 0x00 0x00 0x00 <tag> 0x00
		** Detailed Timings do not...
		*/
		if (BKNI_Memcmp(&hHDMI->AttachedEDID.Block[offset], (void *) &hHDMI->AttachedEDID.DescriptorHeader,
			BHDM_EDID_DESC_HEADER_LEN) == 0)
		{
			BKNI_Memcpy(pDescriptorText, &hHDMI->AttachedEDID.Block[offset + BHDM_EDID_DESC_DATA],
				BHDM_EDID_MONITOR_DESC_SIZE - BHDM_EDID_DESC_HEADER_LEN) ;

			rc = BERR_SUCCESS ;  /* Descriptor Found and Copied */
			goto done ;
		}
	}

	/* Descriptor Not Found...check extension blocks */
	extensions = hHDMI->AttachedEDID.BasicData.Extensions;
	if (!extensions)
	{
		rc = BHDM_EDID_DESCRIPTOR_NOT_FOUND ;
		goto done ;
	}


	/* Search EDID Extension blocks for additional descriptors */
	for (i = 1 ; i <= extensions; i++)
	{
		/* read the next 128 Byte EDID block */
		BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI,
			i, hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;


		/* check Extension Tag type for Timing Data */
		offset = 0 ;
		if (hHDMI->AttachedEDID.Block[offset] != BHDM_EDID_EXT_TIMING_DATA)
			continue ;

		/* determine the number of Detailed Timing Descripors */
		switch (hHDMI->AttachedEDID.Block[offset+1])
		{
		case 0x01 :  /* Check all blocks regardless of */
			MaxDescriptors = 6 ;   /* (128 - 6) / 18 */
			break ;

		case 0x02 :  /* the version of the EDID Extension */
		case 0x03 :  /* See EA-861 B Spec */
			MaxDescriptors = hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_MONITOR_SUPPORT] ;
			MaxDescriptors = MaxDescriptors & 0x0F ;
			break ;

		default :
			BDBG_ERR(("Timing Extension Version '%d' Not Supported",
				hHDMI->AttachedEDID.Block[offset+1])) ;
			rc = BHDM_EDID_EXT_VERSION_NOT_SUPPORTED ;
			goto done ;
		}


		/* skip start of EDID Extension Block */
		offset = offset + hHDMI->AttachedEDID.Block[offset + 2] ;

		for (j = 0 ; j < MaxDescriptors; j++)
		{
			if (BKNI_Memcmp(&hHDMI->AttachedEDID.Block[offset + BHDM_EDID_MONITOR_DESC_SIZE*j],
				       (void *) &hHDMI->AttachedEDID.DescriptorHeader, BHDM_EDID_DESC_HEADER_LEN) == 0)
			{
				BKNI_Memcpy(pDescriptorText, &hHDMI->AttachedEDID.Block[offset + BHDM_EDID_DESC_DATA],
					BHDM_EDID_MONITOR_DESC_SIZE - BHDM_EDID_DESC_HEADER_LEN) ;
				rc = BERR_SUCCESS ;  /* Descriptor Found and Copied */
				goto done ;
			}
		}
	}

done:
	BDBG_LEAVE(BHDM_EDID_GetDescriptor) ;
	return rc ;
} /* end BHDM_EDID_GetDescriptor */



BERR_Code BHDM_EDID_P_ParseMonitorRange(BHDM_Handle hHDMI, uint8_t offset)
{
	BERR_Code rc = BERR_SUCCESS ;

	/* indicate a Monitor Range descriptor has been parsed */
	hHDMI->AttachedEDID.BcmMonitorRangeParsed = true ;
	
	hHDMI->AttachedEDID.MonitorRange.MinVertical =
		hHDMI->AttachedEDID.Block[offset + BHDM_EDID_DESC_RANGE_MIN_V_RATE] ;

	hHDMI->AttachedEDID.MonitorRange.MaxVertical =
		hHDMI->AttachedEDID.Block[offset +  BHDM_EDID_DESC_RANGE_MAX_V_RATE] ;

	hHDMI->AttachedEDID.MonitorRange.MinHorizontal =
		hHDMI->AttachedEDID.Block[offset +  BHDM_EDID_DESC_RANGE_MIN_H_RATE] ;

	hHDMI->AttachedEDID.MonitorRange.MaxHorizontal =
		hHDMI->AttachedEDID.Block[offset +  BHDM_EDID_DESC_RANGE_MAX_H_RATE] ;

	hHDMI->AttachedEDID.MonitorRange.MaxPixelClock
		= hHDMI->AttachedEDID.Block[offset + BHDM_EDID_DESC_RANGE_MAX_PCLOCK] * 10 ;

	/*
	** check for secondary timing formula but don't decode it...
	** make the bytes available for decoding by caller..
	*/
	hHDMI->AttachedEDID.MonitorRange.SecondaryTiming =
		hHDMI->AttachedEDID.Block[offset + EDID_DESC_TIMING_FORMULA] ;

	BKNI_Memcpy(hHDMI->AttachedEDID.MonitorRange.SecondaryTimingParameters,
		   &hHDMI->AttachedEDID.Block[offset + EDID_DESC_TIMING_FORMULA + 1], 7) ;

#if BDBG_DEBUG_BUILD
	/* Display DEBUG Messages */
	BDBG_MSG(("Min - Max Vertical:   %d - %d Hz",
		hHDMI->AttachedEDID.MonitorRange.MinVertical, 	hHDMI->AttachedEDID.MonitorRange.MaxVertical)) ;

	BDBG_MSG(("Min - Max Horizontal: %d - %d kHz",
		hHDMI->AttachedEDID.MonitorRange.MinHorizontal, hHDMI->AttachedEDID.MonitorRange.MaxHorizontal)) ;

	BDBG_MSG(("Max Pixel Clock %d MHz", hHDMI->AttachedEDID.MonitorRange.MaxPixelClock)) ;

#if 0
	/* Display More DEBUG messages for Timing Support */
	switch (hHDMI->AttachedEDID.MonitorRange.SecondaryTiming)
	{
	case 0x00 : BDBG_MSG(("No Timing Formula Support")) ;     break ;
	case 0x02 : BDBG_MSG(("Secondary GTF curve supported")) ; break ;
	default :   BDBG_MSG(("Other secondary timing formula")) ;
	}
#endif
	BDBG_MSG(("[END Monitor Range Descriptor]")) ; /* add a blank line */
#endif

	return rc ;
}



/******************************************************************************
BERR_Code BHDM_EDID_GetMonitorRange
Summary: Retrieve the monitor ranges supported as specified in the EDID
*******************************************************************************/
BERR_Code BHDM_EDID_GetMonitorRange(
   BHDM_Handle hHDMI,                    /* [in] HDMI handle */
   BHDM_EDID_MonitorRange *pMonitorRange /* [out] pointer to BHDM_EDID_MonitorRange
										    to hold the retrieved data */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t RxDeviceAttached ;

	BDBG_ENTER(BHDM_EDID_GetMonitorRange) ;

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		return BHDM_NO_RX_DEVICE ;
	}

	if (!hHDMI->AttachedEDID.BcmMonitorRangeParsed)
	{
		pMonitorRange->MinVertical = 60 ;
		pMonitorRange->MaxVertical = 60 ;
		
		pMonitorRange->MinHorizontal = 26 ;
		pMonitorRange->MaxHorizontal = 68 ;

		pMonitorRange->MaxPixelClock = 80 ;

		BDBG_WRN(("Required EDID Monitor Range Descriptor Not Parsed/Found")) ;
		BDBG_WRN(("Forcing 60Hz monitor type monitor")) ;
		BDBG_WRN(("   Vertical Refresh: %d", pMonitorRange->MinVertical)) ;
		BDBG_WRN(("   Horizontal Refresh %d - %d ",
			pMonitorRange->MinHorizontal, pMonitorRange->MaxHorizontal)) ;
		
		goto done ;
	}
	
	if (hHDMI->AttachedEDID.MonitorRange.MinVertical == 0)
	{
		rc = BHDM_EDID_NOT_IMPLEMENTED ;
		goto done ;
	}

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) 
	{
		rc = BHDM_EDID_NOT_FOUND;
		goto done ;
	}

	BKNI_Memcpy(pMonitorRange, &hHDMI->AttachedEDID.MonitorRange, sizeof(BHDM_EDID_MonitorRange)) ;

done:
	BDBG_LEAVE(BHDM_EDID_GetMonitorRange) ;
	return rc ;
} /* BHDM_EDID_GetMonitorRange */



/******************************************************************************
BERR_Code BHDM_EDID_IsRxDeviceHdmi
Summary: Retrieve the Vendor Specific Data Block from the first Version 3 Timing
Extension in the EDID
*******************************************************************************/
BERR_Code BHDM_EDID_IsRxDeviceHdmi(
   BHDM_Handle hHDMI,                  /* [in] HDMI handle */
   BHDM_EDID_RxVendorSpecificDB *RxVSDB,  /* [out] pointer to Vendor Specific Data
                                            Block to hold the retrieved data */
   bool *bHdmiDevice
)
{
	uint8_t RxDeviceAttached ;
	BERR_Code rc = BERR_SUCCESS ;


	BDBG_ENTER(BHDM_EDID_IsRxDeviceHdmi) ;

	BKNI_Memset(RxVSDB, 0, sizeof(BHDM_EDID_RxVendorSpecificDB)) ;
		

	*bHdmiDevice = false ;  /* assume device is not HDMI */

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		rc = BHDM_NO_RX_DEVICE ;
		goto done ;
	}

#if BDBG_DEBUG_BUILD
	if (hHDMI->DeviceSettings.BypassEDIDChecking)
	{
		BDBG_WRN(("EDID is ByPassed; Assuming DVI monitor attached")) ;
		goto done ;
	}
#endif

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		BDBG_WRN(("No Valid EDID Found. Default to DVI Device"));
		goto done;
	}

	*bHdmiDevice = hHDMI->AttachedEDID.RxHasHdmiSupport ;
	if (!*bHdmiDevice)
		goto done ;

	/* EDID processed at initialization; copy information */
	BKNI_Memcpy(RxVSDB,  &hHDMI->AttachedEDID.RxVSDB, sizeof(BHDM_EDID_RxVendorSpecificDB)) ;


#if BDBG_DEBUG_BUILD
	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInitialize)
	{
		BDBG_MSG(("HDMI Rx Supported Features (%#x):", RxVSDB)) ;
		BDBG_MSG(("\tUnderscan:    %s", g_status[RxVSDB->Underscan ? 1 : 0])) ;
		BDBG_MSG(("\tAudio Caps:   %s", g_status[RxVSDB->Audio ? 1 : 0])) ;
		BDBG_MSG(("\tYCbCr: 4:2:2 %s   4:4:4 %s",
			g_status[RxVSDB->YCbCr422 ? 1 : 0],
			g_status[RxVSDB->YCbCr444 ? 1 : 0])) ;

		BDBG_MSG(("\tNative Formats in Descriptors: %d",
			RxVSDB->NativeFormatsInDescriptors)) ;

		BDBG_MSG(("END HDMI Rx Supported Features")) ;
		hHDMI->edidStatus = BHDM_EDID_STATE_eProcessing ;
	}
#endif


done:
	BDBG_LEAVE(BHDM_EDID_IsRxDeviceHdmi) ;
	return 	rc ;
} /* BHDM_EDID_IsRxDeviceHdmi */



/******************************************************************************
BERR_Code BHDM_EDID_CheckRxHdmiAudioSupport
Summary: Check if the input Audio Format is supported by the attached HDMI
Receiver
*******************************************************************************/
BERR_Code BHDM_EDID_CheckRxHdmiAudioSupport(
   BHDM_Handle hHDMI,                         /* [in] HDMI handle  */
   BAVC_AudioFormat       eAudioFormat,       /* [in] Audio Format */
   BAVC_AudioSamplingRate eAudioSamplingRate, /* [in] Audio Rate to check for */
   BAVC_AudioBits         eAudioBits,         /* [in] Quantization Bits to search for */
   uint16_t               iCompressedBitRate, /* [in] Bit Rate if Compressed Audio */
   uint8_t                *iSupported         /* [out] audio format is supported */
)
{
	BERR_Code rc = BERR_SUCCESS ;

	uint8_t
		i,
		FormatFound,
		EdidAudioSamplingRate,
		EdidAudioBits,
		EdidMaxCompressedBitRate ;

	uint8_t RxDeviceAttached ;

	BDBG_ENTER(BHDM_EDID_CheckRxHdmiAudioSupport) ;

	*iSupported = 0 ;

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		return BHDM_NO_RX_DEVICE ;
	}

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		return BHDM_EDID_NOT_FOUND;
	}

	/* 1st check for requested format */
	if(!hHDMI->AttachedEDID.BcmSupportedAudioFormats[eAudioFormat].Supported)
		goto done ;

	/**********************************************/
	/* 2nd, Check for requested Audio Sample Rate */
	/* convert the BAVC_AudioSampleRate to EdidAudioSampleRate */

	EdidAudioSamplingRate = BAVC_AudioSamplingRate_eUnknown  ;
	for (i = 0; i < sizeof(EdidAudioSampleRateTable) / sizeof(*EdidAudioSampleRateTable) ; i++)
		if (eAudioSamplingRate == EdidAudioSampleRateTable[i].BcmAudioSampleRate)
		{
			EdidAudioSamplingRate = EdidAudioSampleRateTable[i].EdidAudioSampleRate ;
			break ;
		}

	if (EdidAudioSamplingRate == BAVC_AudioSamplingRate_eUnknown)
		goto done ;

	if (!(EdidAudioSamplingRate
		& hHDMI->AttachedEDID.BcmSupportedAudioFormats[i].ucCeaSampleRates))
		goto done ;


	/********************************************************/
	/* 3rd, Get the number of Audio Bits (quantization) the */
	/* monitor supports for the requested Audio Format etc  */


	EdidAudioBits =
		hHDMI->AttachedEDID.BcmSupportedAudioFormats[i].ucCeaNBits_BitRate ;

	FormatFound = 0 ;
	/* get the number of bits supported by this format */
	if (eAudioFormat != BAVC_AudioFormat_ePCM) /* compressed */
	{										   /*  formats   */
		/* Max Bit Rate = EdidAudioBits * 8 */
		EdidMaxCompressedBitRate = EdidAudioBits * 8 ;
		if (iCompressedBitRate  <= EdidMaxCompressedBitRate)
		{
			BDBG_MSG(("<%.*s> Max Bit Rate Supported: %d",
					BHDM_EDID_DESC_ASCII_STRING_LEN, hHDMI->AttachedEDID.MonitorName,
					EdidMaxCompressedBitRate)) ;
			FormatFound = 1 ;
		}
	} /* if compressed formats */
	else									 /* uncompressed */
	{										 /*     PCM      */
		if (EdidAudioBits & 0x01)           EdidAudioBits = 16 ;
		else if (EdidAudioBits & 0x02)		EdidAudioBits = 20 ;
		else if (EdidAudioBits & 0x04)		EdidAudioBits = 24 ;
		else
		{
			BDBG_WRN(("Unknown Supported Bit Rate")) ;
			rc = BHDM_EDID_HDMI_UNKNOWN_BIT_RATE ;
			goto done ;
		}

		/* check if the number of bits matches the requested value */
		if (eAudioBits == BAVC_AudioBits_e16)		FormatFound = (EdidAudioBits == 16) ;
		else if (eAudioBits == BAVC_AudioBits_e20)	FormatFound = (EdidAudioBits == 20) ;
		else if (eAudioBits == BAVC_AudioBits_e24)	FormatFound = (EdidAudioBits == 24) ;
	} /* else uncompressed formats */


	if (FormatFound)
		*iSupported = 1 ;

done:
	BDBG_LEAVE(BHDM_EDID_CheckRxHdmiAudioSupport) ;
	return 	rc ;
} /* BHDM_EDID_CheckRxHdmiAudioSupport */



/******************************************************************************
BERR_Code BHDM_EDID_CheckRxHdmiVideoSupport
Summary: Check if the input Audio Format is supported by the attached HDMI
Receiver
*******************************************************************************/
BERR_Code BHDM_EDID_CheckRxHdmiVideoSupport(
	BHDM_Handle hHDMI,                    /* [in] HDMI handle  */
	uint16_t           HorizontalPixels,  /* [in] Horiz Active Pixels */
	uint16_t           VerticalPixels,    /* [in] Vertical Active Pixels */
	BAVC_ScanType      eScanType,         /* [in] Progressive, Interlaced */
	BAVC_FrameRateCode eFrameRateCode,    /* [in] Vertical Frequency */
	BFMT_AspectRatio   eAspectRatio,      /* [in] Horiz to Vertical Ratio */
	uint8_t            *pNativeFormat	  /* [out] Requested format is a
										      native format to the monitor */
)
{
	BERR_Code rc = BHDM_EDID_HDMI_VIDEO_FORMAT_UNSUPPORTED ;

	uint8_t
		i, j, k, /* indexes */
		extensions,
		DataOffset,
		DataBlockIndex,
		DataBlockTag,
		DataBlockLength,
		FormatFound,
		NumVideoDescriptors,
		EdidVideoIDCode ;

	uint8_t RxDeviceAttached ;

	BDBG_ENTER(BHDM_EDID_CheckRxHdmiVideoSupport) ;


	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		return BHDM_NO_RX_DEVICE ;
	}

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		return BHDM_EDID_NOT_FOUND;
	}

	/* first get the number of extensions in the EDID */
	/* audio support is always in first V3 Timing Extension; never in block 0 */
	extensions = hHDMI->AttachedEDID.BasicData.Extensions ;

	if (!extensions)
	{
		BDBG_WRN(("No EDID Extensions Found... No HDMI Support")) ;
		rc = BHDM_EDID_HDMI_VIDEO_FORMAT_UNSUPPORTED ;
		goto done ;
	}

	/* check ALL extensions for Version 3 Timing Extensions */
	for (i = 1 ; i <= extensions; i++)
	{
		if (hHDMI->edidStatus != BHDM_EDID_STATE_eOK)
		{
			BHDM_CHECK_RC(rc,
				BHDM_EDID_GetNthBlock(hHDMI, i, (uint8_t *) &hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;
		}

		/* check for Timing Data Extension */
		if (hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_TAG] != BHDM_EDID_EXT_TIMING_DATA)
			continue ;

		/* check for Version 3 Timing Data Extension */
		if (hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_VERSION] != BHDM_EDID_TIMING_VERSION_3)
			continue ;


		BDBG_MSG(("V3 Timing Extension found in EDID Extension #%d", i)) ;

		/* check if data blocks exist before the 18 Byte Detailed Timing data */
		DataOffset = hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_DATA_OFFSET] ;
		if ((DataOffset == 0)
		||  (DataOffset == 4)) /* no Reserved Data is Available */
		{
			BDBG_MSG(("-----V3 Timing Extension contains no CEA Data Blocks")) ;
			continue ;          /* continue to the next Timing Extension */
		}
		BDBG_MSG(("-----CEA Data Blocks found in V3 Timing Extension")) ;


		/* set the index to the start of Data Blocks */
		DataBlockIndex = BHDM_EDID_EXT_DATA_BLOCK_COLLECTION ;

		/* scan through the data blocks and retrieve the necessary information */
		while (DataBlockIndex < DataOffset)
		{
			/* get the Data Block type */
			DataBlockTag =
				hHDMI->AttachedEDID.Block[DataBlockIndex] >> 5 ;

			/* get the Data Block length */
			DataBlockLength =
				hHDMI->AttachedEDID.Block[DataBlockIndex] & 0x1F ;

			BDBG_MSG(("\n[%02X] CEA-861 %s (0x%02x) found; %d bytes",
				hHDMI->AttachedEDID.Block[DataBlockIndex],
				CeaTagName[DataBlockTag], DataBlockTag, DataBlockLength)) ;

			switch (DataBlockTag)
			{

			/* return error on unknown Tags */
			default :
				BDBG_WRN(("\nCEA-861 Data Block Tag Code <%d> is not supported",
					DataBlockTag)) ;
				rc = BHDM_EDID_HDMI_UNKNOWN_CEA_TAG ;
				goto done ;

			/* skip Block Tags that are of no interest to this function */
			case BHDM_EDID_CeaDataBlockTag_eVSDB :      /* Vendor Specific DB */
			case BHDM_EDID_CeaDataBlockTag_eAudioDB :   /* Audio DB */
			case BHDM_EDID_CeaDataBlockTag_eSpeakerDB : /* Speaker Allocation DB */
			case BHDM_EDID_CeaDataBlockTag_eReserved0 :
			case BHDM_EDID_CeaDataBlockTag_eReserved5 :
			case BHDM_EDID_CeaDataBlockTag_eReserved6 :
			case BHDM_EDID_CeaDataBlockTag_eExtendedDB:
				break ;

			case BHDM_EDID_CeaDataBlockTag_eVideoDB :   /* Video DB */
				/* check each video descriptor for requested video support */

				FormatFound = 0 ;
				NumVideoDescriptors = DataBlockLength ;

				/* for each CEA Video ID Code Found */
				for (j = 0 ; j < NumVideoDescriptors && !FormatFound ; j++ )
				{
					/* get the supported Video Code ID; check if a native format */
					EdidVideoIDCode = hHDMI->AttachedEDID.Block[DataBlockIndex + j + 1] ;
					EdidVideoIDCode = EdidVideoIDCode  & 0x7F ;

					*pNativeFormat = EdidVideoIDCode & 0x80 ;

					BDBG_MSG(("Find CEA Video ID Code %02d parameters...",
						EdidVideoIDCode)) ;

					/* search BCM 861-B supported formats for format found in EDID */
					for (k = 0 ; k < BHDM_EDID_P_BCM_VIDEO_FORMATS_MAX ; k++)
					{
						if (EdidVideoIDCode != BHDM_EDID_P_Cea861bFormats[k].CeaVideoCode)
							continue ;

						BDBG_MSG(("Found supported CEA Video ID Code: %02d (%d x %d)",
							EdidVideoIDCode,
							BHDM_EDID_P_Cea861bFormats[k].HorizontalPixels,
							BHDM_EDID_P_Cea861bFormats[k].VerticalPixels)) ;

						/* check if the specified parameters match the requested formats */
						/* 1st, Check if Pixel Format matches */
						if ((HorizontalPixels != BHDM_EDID_P_Cea861bFormats[k].HorizontalPixels)
						||	(VerticalPixels != BHDM_EDID_P_Cea861bFormats[k].VerticalPixels))
							break  ;
						BDBG_MSG(("Pixel Format Match")) ;


						/* 2nd, Check Scan Type (i/p) */
						if (eScanType != BHDM_EDID_P_Cea861bFormats[k].eScanType)
							break  ;
						BDBG_MSG(("Scan Type Match..")) ;


						/* 3rd, Check Vertical Frequency */
						if (eFrameRateCode != BHDM_EDID_P_Cea861bFormats[k].eFrameRateCode)
							break ;
						BDBG_MSG(("Frame Rate Match..")) ;


						/* 4th  Check Aspect Ratio (4:3, 16:9) etc. */
						if (eAspectRatio != BHDM_EDID_P_Cea861bFormats[k].eAspectRatio)
							break ;

						BDBG_MSG(("Requested format is supported..")) ;
						rc = BERR_SUCCESS ;
						goto done ;
					} /* for each BCM Supported CEA Video ID Code */
				} /* for each Supported CEA Video ID Code */
			} /* for each CEA Video ID Code found */


			DataBlockIndex += DataBlockLength + 1;

		} /* while DataBlockIndex < DataOffset */

	} /* for each extension */

	rc = BHDM_EDID_HDMI_VIDEO_FORMAT_UNSUPPORTED ;

done:
	BDBG_LEAVE(BHDM_EDID_CheckRxHdmiVideoSupport) ;
	return 	rc ;
} /* BHDM_EDID_CheckRxHdmiVideoSupport */



/******************************************************************************
BERR_Code BHDM_EDID_VideoFmtSupported
Summary: Check if the requested VideoFmt is supported by the attached HDMI Rx
*******************************************************************************/
BERR_Code BHDM_EDID_VideoFmtSupported(
	BHDM_Handle hHDMI,        /* [in] HDMI handle  */
	BFMT_VideoFmt eVideoFmt,  /* requested video format */
	uint8_t *Supported        /* [out] true/false Requested format is
							    supported by the monitor */
)
{
	BERR_Code rc = BERR_SUCCESS  ;
	const BFMT_VideoInfo *pVideoFormatInfo ;
	uint8_t RxDeviceAttached ;

	BDBG_ENTER(BHDM_EDID_VideoFmtSupported) ;

	/* default to format not supported */
	*Supported = 0 ;

	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
		goto done ;

#if BDBG_DEBUG_BUILD
	if (hHDMI->DeviceSettings.BypassEDIDChecking)
	{
		*Supported = 1 ;
		BDBG_WRN(("EDID is ByPassed; all formats will attempt to display")) ;
		goto done ;
	}
#endif

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		rc = BHDM_EDID_NOT_FOUND;
		goto error;
	}

	if (hHDMI->AttachedEDID.BcmVideoFormatsChecked == 0)
	{
		rc = BHDM_EDID_VIDEO_FORMATS_UNAVAILABLE ;
		goto error ;
	}

	if (hHDMI->AttachedEDID.BcmSupportedVideoFormats[eVideoFmt])
	{
		*Supported = 1 ;
		goto done;
	}
	else
	{
		/* Warn the BCM supported format is not supported by the Rx only once */
		if (!hHDMI->AttachedEDID.UnsupportedVideoFormatReported[eVideoFmt])
		{
			pVideoFormatInfo = BFMT_GetVideoFormatInfoPtr(eVideoFmt) ;
			
			BDBG_WRN(("%-30s %4d x %4d %c NOT SUPPORTED by attached <%.13s> receiver",
				pVideoFormatInfo->pchFormatStr,
				pVideoFormatInfo->ulDigitalWidth, pVideoFormatInfo->ulDigitalHeight,
				Mode[pVideoFormatInfo->bInterlaced],
				hHDMI->AttachedEDID.MonitorName)) ;
			BSTD_UNUSED(pVideoFormatInfo) ; /* supress coverity message for non-debug builds */
			hHDMI->AttachedEDID.UnsupportedVideoFormatReported[eVideoFmt] = true ;
		}			

		goto done;
	}

error:
	if (eVideoFmt == BFMT_VideoFmt_eDVI_640x480p) {

		BDBG_MSG(("Can't find/read EDID. Assume HDMI receiver supports VGA (640x480p)"));
		*Supported = 1;
		rc = BERR_SUCCESS;
	}

done:
	BDBG_LEAVE(BHDM_EDID_VideoFmtSupported) ;
	return 	rc ;
} /* BHDM_EDID_VideoFmtSupported */


BERR_Code BHDM_EDID_P_GetVerticalFrequency(
	uint32_t ulVertFreqMask, uint16_t *uiVerticalFrequency)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t i ;

static const struct {
	uint32_t ulVertFreqMask ;
	uint32_t uiVerticalFrequency ;
	} VerticalFrequencies[] =
	{
		{BFMT_VERT_50Hz,        50},
		{BFMT_VERT_59_94Hz,   59},
		{BFMT_VERT_60Hz,        60},

	 	{BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,        60},
		{BFMT_VERT_50Hz | BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,        60},

		{BFMT_VERT_66Hz,        66},
		{BFMT_VERT_70Hz,        70},
		{BFMT_VERT_72Hz,        72},
		{BFMT_VERT_75Hz,        75},
		{BFMT_VERT_85Hz,        85},
			
		{BFMT_VERT_23_976Hz, 23},
		{BFMT_VERT_24Hz,        24},
		{BFMT_VERT_23_976Hz  | BFMT_VERT_24Hz, 24},

		{BFMT_VERT_25Hz,        25},

		{BFMT_VERT_29_97Hz,   29},
		{BFMT_VERT_30Hz,        30},
		{BFMT_VERT_29_97Hz  | BFMT_VERT_30Hz, 30},			
	};

	for (i=0; i < sizeof(VerticalFrequencies) / sizeof(*VerticalFrequencies); i++)
	{
		if (ulVertFreqMask & VerticalFrequencies[i].ulVertFreqMask)
		{
			*uiVerticalFrequency = VerticalFrequencies[i].uiVerticalFrequency ;
			return rc ;
		}
	}

	BDBG_WRN(("Unknown Vertical Frequency Mask %#08X; using 60", ulVertFreqMask)) ;
	*uiVerticalFrequency = 60 ;

	return rc ;
}


/******************************************************************************
Summary:
Parse the Established Timings to check for CEA 861 B video formats
*******************************************************************************/
static BERR_Code BHDM_EDID_P_ParseEstablishedTimingFormats(
	BHDM_Handle hHDMI                  /* [in] HDMI handle  */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t EstablishedTimings ;

#if BDBG_DEBUG_BUILD
 	uint8_t i ;
	 static char *EstablishedTimingsIText[8] =
	{
		 "800 x 600 @ 60Hz VESA",
		 "800 x 600 @ 56Hz VESA",
		 "640 x 480 @ 75Hz VESA",
		 "640 x 480 @ 72Hz VESA",
		 "640 x 480 @ 67Hz Apple, Mac II",
		 "640 x 480 @ 60Hz IBM, VGA",
		 "720 x 400 @ 88Hz IBM, XGA2",
		 "720 x 400 @ 70Hz IBM, VGA"
	 } ;

	 static char *EstablishedTimingsIIText[8] =
	{
		 "1280 x 1024 @ 75Hz VESA",
		 "1024 x 768 @ 75Hz VESA",
		 "1024 x 768 @ 70Hz VESA",
		 "1024 x 768 @ 60Hz VESA",
		 "1024 x 768 @ 87Hz(I) IBM",
		 "832 x 624 @ 75Hz Apple, Mac II",
		 "800 x 600 @ 75Hz VESA",
		"800 x 600 @ 72Hz VESA"
	 } ;
#endif

	/* assumes current block is 0 */

	/* ESTABLISHED #1 */


	EstablishedTimings = hHDMI->AttachedEDID.Block[BHDM_EDID_ESTABLISHED_TIMINGS1] ;
#if BDBG_DEBUG_BUILD
      if (!EstablishedTimings)
	  	goto CheckEstablishTiming2 ;

	BDBG_MSG(("Monitor Established Timings  I (%#02x)", EstablishedTimings)) ;
	for (i = 0 ;i  < 8; i++)
	{
		if (EstablishedTimings & (1 << i))
			BDBG_MSG(("%s", EstablishedTimingsIText[i])) ;
	}
#endif

	if (EstablishedTimings & BHDM_EDID_ESTABLISHED_TIMINGS_1_640x480_60HZ)
	{
		BDBG_MSG(("Found BCM Supported 640x480p")) ;
		hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_eDVI_640x480p] = true ;
	}

	if (EstablishedTimings & BHDM_EDID_ESTABLISHED_TIMINGS_1_800x600_60HZ)
	{
		BDBG_MSG(("Found BCM Supported 800x600p")) ;
		hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_eDVI_800x600p] = true ;
	}


#if BDBG_DEBUG_BUILD
CheckEstablishTiming2 :
#endif
	/* ESTABLISHED #2 */
	EstablishedTimings = hHDMI->AttachedEDID.Block[BHDM_EDID_ESTABLISHED_TIMINGS2] ;
      if (!EstablishedTimings)
	  	goto done ;
#if BDBG_DEBUG_BUILD
	BDBG_MSG(("Monitor Established Timings II (%#02x)", EstablishedTimings)) ;
	for (i = 0 ;i  < 8; i++)
	{
		if (EstablishedTimings & (1 << i))
			BDBG_MSG(("%s", EstablishedTimingsIIText[i])) ;
	}
#endif

	if (EstablishedTimings & BHDM_EDID_ESTABLISHED_TIMINGS_2_1024x768_60HZ)
	{
		BDBG_MSG(("Found BCM Supported 1024x768p")) ;
		hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_eDVI_1024x768p] = true ;
	}

done:
	/* indicate formats have been checked */
	hHDMI->AttachedEDID.BcmVideoFormatsChecked = 1 ;
	return rc ;
}


/******************************************************************************
Summary:
Return all CEA 861 B video formats supported by the attached monitor as
sepecified in the V3 Timing Ext Video Data Block.
*******************************************************************************/
BERR_Code BHDM_EDID_P_ParseVideoDB(
	BHDM_Handle hHDMI,              /* [in] HDMI handle  */
	uint8_t DataBlockIndex,         /* [in] start offset of Video Data Block */
	uint8_t DataBlockLength         /* [in] length (number) of Video ID codes */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	const BFMT_VideoInfo *pVideoFormatInfo ;
	uint8_t
		j, k, l, /* indexes */
		NumVideoDescriptors,
		EdidVideoIDCode,
		NativeFormat,
		LastIdAdded,
		SupportedIdCount ;
	
	BFMT_VideoFmt eVideoFmt ;
	uint16_t temp ;
	uint16_t tempH;
	BHDM_EDID_P_VideoDescriptor 	*pVideoDescriptor = NULL ;


	/* indicate formats have been checked */
	hHDMI->AttachedEDID.BcmVideoFormatsChecked = 1 ;

	/* set the number of video descriptors to look at  */
	NumVideoDescriptors = DataBlockLength ;

	LastIdAdded = 0 ;
	SupportedIdCount = 0 ;
	
#if BHDM_CONFIG_HDMI_3D_SUPPORT	
	hHDMI->AttachedEDID.First16VideoDescriptorsMask = 0;
#endif

	/* for each CEA Video ID Code Found */
	BDBG_MSG(("<%s> CEA-861 Supported Video Formats (%d):",
		hHDMI->AttachedEDID.MonitorName, NumVideoDescriptors)) ;

	for (j = 0 ; j < NumVideoDescriptors ; j++ )
	{
		/* get the supported Video Code ID; check if a native format */
		EdidVideoIDCode = hHDMI->AttachedEDID.Block[DataBlockIndex + j + 1] ;
		NativeFormat = (EdidVideoIDCode & 0x80) ? 1 : 0 ;
		EdidVideoIDCode = EdidVideoIDCode  & 0x7F ;


		/* search BCM 861-B supported formats for format found in EDID */
		for (k = 0 ; k < BHDM_EDID_P_BCM_VIDEO_FORMATS_MAX ; k++)
		{
			/* find the retrieved Video ID Code in our table */
			if (EdidVideoIDCode != BHDM_EDID_P_Cea861bFormats[k].CeaVideoCode)
				continue ;

			/* a BCM supported Video ID code has been found; add it to our list */
			/* since this is a loop make sure it is added only once */

#if BHDM_CONFIG_HDMI_3D_SUPPORT
			BDBG_MSG(("Found BCM supported CEA-861 Video ID: %02d (%s)",
				EdidVideoIDCode,
				(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr(EdidVideoIDCode))) ;

			/* Set the support mask for the first 16 video descriptors for later use on parsing 3D support */
			if (j < 16)
				hHDMI->AttachedEDID.First16VideoDescriptorsMask |= (uint16_t) (1 << j);
#endif

			
			if (LastIdAdded != EdidVideoIDCode)
			{
				pVideoDescriptor = (BHDM_EDID_P_VideoDescriptor  *)
					BKNI_Malloc(sizeof(BHDM_EDID_P_VideoDescriptor )) ;
				if (pVideoDescriptor == NULL)
				{
					rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					return rc;
				}
				pVideoDescriptor->VideoIdCode  = EdidVideoIDCode ;
				pVideoDescriptor->NativeFormat = NativeFormat ;

				BLST_Q_INSERT_TAIL(&hHDMI->AttachedEDID.VideoDescriptorList, pVideoDescriptor, link) ;

				/* add Video Id code to supported list for GetVideoInfo */
				hHDMI->AttachedEDID.BcmSupportedVideoIdCodes[SupportedIdCount++] = EdidVideoIDCode ;
				
#if BHDM_CONFIG_HDMI_3D_SUPPORT
				hHDMI->AttachedEDID.NumBcmSupportedVideoDescriptors++;
#endif
			}

			/* adjust pixel repeat formats if necessary */
			if ((BHDM_EDID_P_Cea861bFormats[k].HorizontalPixels == 1440) ||
				(BHDM_EDID_P_Cea861bFormats[k].HorizontalPixels == 2880))
				tempH = 720 ;
			else
				tempH = BHDM_EDID_P_Cea861bFormats[k].HorizontalPixels ;

			/* set all matching BFMT_VideoFmts as supported */
			for (l = 0; l < BFMT_VideoFmt_eMaxCount; l++)
			{
				/* no longer skip the already supported formats; since the corresponding
				   BCM_VideoFmt has to also be added to the VideoIdCode structure */

				eVideoFmt = (BFMT_VideoFmt) l ;
				pVideoFormatInfo = BFMT_GetVideoFormatInfoPtr(eVideoFmt) ;
				if (!pVideoFormatInfo)
					continue;

				/* Skip 3D formats */
				if (BFMT_IS_3D_MODE(eVideoFmt)) {
					continue;
				}

				/* check if the specified parameters match the requested formats */
				/* 1st, Check if Pixel Format matches */

				if ((pVideoFormatInfo->ulDigitalWidth != tempH)
				||	(pVideoFormatInfo->ulDigitalHeight != BHDM_EDID_P_Cea861bFormats[k].VerticalPixels))
				{
#if 0
					BDBG_MSG(("BFMT%d %d x %d <> %d x %d", l,
						pVideoFormatInfo->ulDigitalWidth, pVideoFormatInfo->ulDigitalHeight,
						tempH, BHDM_EDID_P_Cea861bFormats[k].VerticalPixels)) ;
#endif
					continue ;
				}

				/* 2nd, Check Scan Type (i/p) */
				if (pVideoFormatInfo->bInterlaced !=
						(BAVC_ScanType_eInterlaced == BHDM_EDID_P_Cea861bFormats[k].eScanType))
					continue  ;

				/* use the BFMT frequency parameter to get the Vertical Frequency */
				/* Ignore rc; default of 60Hz will be used for unknown Frequency */
				BHDM_EDID_P_GetVerticalFrequency(
					pVideoFormatInfo->ulVertFreqMask, &temp) ;

				/* 3rd check the 861 vertical frequency matches the BCM format  */
				if ((temp == 59) || (temp == 60))
				{
					if ((BHDM_EDID_P_Cea861bFormats[k].eFrameRateCode != BAVC_FrameRateCode_e59_94)
					&&  (BHDM_EDID_P_Cea861bFormats[k].eFrameRateCode != BAVC_FrameRateCode_e60))
						continue ;
				}
				else if  ((temp == 50)
				&&   (BHDM_EDID_P_Cea861bFormats[k].eFrameRateCode != BAVC_FrameRateCode_e50))
					continue ;

				else if (((temp == 23) || (temp == 24))
				&&   (BHDM_EDID_P_Cea861bFormats[k].eFrameRateCode != BAVC_FrameRateCode_e23_976)
				&&   (BHDM_EDID_P_Cea861bFormats[k].eFrameRateCode != BAVC_FrameRateCode_e24))
					continue;

				else if ((temp == 25)
				&&   (BHDM_EDID_P_Cea861bFormats[k].eFrameRateCode != BAVC_FrameRateCode_e25))
					continue;

				else if (((temp == 29) || (temp == 30))
				&&   (BHDM_EDID_P_Cea861bFormats[k].eFrameRateCode != BAVC_FrameRateCode_e30))
					continue;


				/* format matches */
#if 0
				BDBG_WRN(("        %s is supported", pVideoFormatInfo->pchFormatStr)) ;
#endif
				hHDMI->AttachedEDID.BcmSupportedVideoFormats[l] = true ;
				if ((LastIdAdded != EdidVideoIDCode) && (pVideoDescriptor))
				{
					pVideoDescriptor->eVideoFmt    = eVideoFmt ;
					LastIdAdded = EdidVideoIDCode ;
				}
			} /* for each BCM BFMT_VideoFmt */
		} /* for each BCM Supported CEA-861-B Video ID Code */
	} /* for each CEA-861-B Video Descriptor */


	return rc ;
}



/******************************************************************************
Summary:
Return all CEA 861 B video formats supported by the attached monitor as
sepecified in the V3 Timing Ext Video Data Block.
*******************************************************************************/
BERR_Code BHDM_EDID_P_ParseAudioDB(
	BHDM_Handle hHDMI,             /* [in] HDMI handle  */
	uint8_t DataBlockIndex,        /* [in] start offset of Video Data Block */
	uint8_t DataBlockLength        /* [in] length (number) of Video ID codes */
)
{

	BERR_Code rc = BERR_SUCCESS ;

#if BDBG_DEBUG_BUILD
	static char *CeaAudioTypeText[] =
	{
		"Reserved",	"PCM",
		"AC3",		"MPEG1",
		"MP3",		"MPEG2",
		"AAC",		"DTS",
		"ATRAC",		"One Bit Audio",
		"DDP",		"DTS-HD",
		"MAT (MLP)",	"DST",
		"WMA Pro",	"Reserved15"
	} ;
#endif


	uint8_t
		i, j, /* indexes */
		NumAudioDescriptors,
		BcmAudioFormat,
		SampleRateFound,
		EdidAudioFormat,
		EdidAudioMaxChannels,
		EdidAudioSampleRate,
		EdidAudioBits ;


	NumAudioDescriptors = DataBlockLength / 3 ;

	/* for each CEA Audio Format Code Found */
	BDBG_MSG(("<%s> CEA-861 Supported audio format:",
		hHDMI->AttachedEDID.MonitorName)) ;

	for (j = 0 ; j < NumAudioDescriptors ; j++)
	{
		EdidAudioFormat        = hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 1] ;
		EdidAudioSampleRate = hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 2] ;
		EdidAudioBits             = hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 3] ;

		EdidAudioFormat = EdidAudioFormat & 0x7F ; /* clear reserved bit */
		EdidAudioMaxChannels = (EdidAudioFormat & 0x07) + 1 ; /* max channels */
		EdidAudioFormat = EdidAudioFormat >> 3 ;

		/************************************************/
		/* 1st check if format is supported  */
		BcmAudioFormat = BAVC_AudioFormat_eMaxCount ;
		for (i = 0; i < sizeof(BHDM_EDID_P_BcmSupportedFormats) / sizeof(*BHDM_EDID_P_BcmSupportedFormats) ; i++)
			if (EdidAudioFormat == BHDM_EDID_P_BcmSupportedFormats[i].EdidAudioFormat)
			{
				BcmAudioFormat = BHDM_EDID_P_BcmSupportedFormats[i].BcmAudioFormat;
				break ;
			}

		if (BcmAudioFormat == BAVC_AudioFormat_eMaxCount)
		{
			BDBG_WRN(("%s - **** NOT Implemented/Supported by BCM%d ; [%02X %02X %02X]",
				CeaAudioTypeText[EdidAudioFormat],  BCHP_CHIP,
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 1],
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 2],
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 3])) ;

			continue ;
		}

		hHDMI->AttachedEDID.BcmAudioFormatsChecked = 1 ;

		/**********************************************/
		/* 2nd, Check for supported Audio Sample Rate */
		EdidAudioSampleRate &= 0x7F ; /* clear reserved bit */

		SampleRateFound = 0 ;
		for (i = 0; i < sizeof(EdidAudioSampleRateTable) / sizeof(*EdidAudioSampleRateTable); i++)
			/* check that at least one sample rate is supported */
			if (EdidAudioSampleRate & EdidAudioSampleRateTable[i].EdidAudioSampleRate)
			{
				SampleRateFound = 1 ;
				break ;
			}

		if (!SampleRateFound)
			continue ;

		/********************************************************/
		/* 3rd, Determine the number of Audio Bits (quantization) the */
		/* monitor supports for the requested Audio Format etc  */

		/* get the number of bits supported by this format */
		if (EdidAudioFormat != BHDM_EDID_P_AudioFormat_ePCM) /* compressed */
		{										           /*  formats   */
			/* Max Bit Rate = EdidAudioBits * 8 */
			EdidAudioBits = EdidAudioBits /** 8*/ ;

			/* display debug information */
			BDBG_MSG(("Found BCM supported CEA-861 Audio: %s - %d Ch [%d max bit rate] ; [%02X %02X %02X]",
				CeaAudioTypeText[EdidAudioFormat],
				EdidAudioMaxChannels, EdidAudioBits *8,
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 1],
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 2],
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 3])) ;

#if BDBG_DEBUG_BUILD
			/* show the supported sample rates */
			for (i = 0; i < sizeof(EdidAudioSampleRateTable) / sizeof(*EdidAudioSampleRateTable); i++)
				/* check that at least one sample rate is supported */
				if (EdidAudioSampleRate & EdidAudioSampleRateTable[i].EdidAudioSampleRate)
				{
					BDBG_MSG(("\tSample Rate %s", CeaAudioSampleRateTypeText[i])) ;
				}
#endif
		} /* END if compressed formats */
		else									 /* uncompressed */
		{										 /*     PCM      */
			uint8_t uiAudioSampleSize ;

			if (EdidAudioBits & 0x04)       uiAudioSampleSize = 24 ;
			else if (EdidAudioBits & 0x02)	uiAudioSampleSize = 20 ;
			else if (EdidAudioBits & 0x01)	uiAudioSampleSize = 16 ;
			else
			{
				BDBG_WRN(("Unknown/Un-Supported Bit Rate")) ;
				rc = BHDM_EDID_HDMI_UNKNOWN_BIT_RATE ;
				continue ;
			}

			/* display debug information */
			BDBG_MSG(("Found BCM supported CEA-861 Audio: %s - %d Ch [up to %d bits] ; [%02X %02X %02X]",
				CeaAudioTypeText[EdidAudioFormat],
				EdidAudioMaxChannels, uiAudioSampleSize,
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 1],
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 2],
				hHDMI->AttachedEDID.Block[DataBlockIndex+ j*3 + 3])) ;

#if BDBG_DEBUG_BUILD
			/* show the supported sample rates */
			for (i = 0; i < sizeof(EdidAudioSampleRateTable) / sizeof(*EdidAudioSampleRateTable); i++)
				/* check that at least one sample rate is supported */
				if (EdidAudioSampleRate & EdidAudioSampleRateTable[i].EdidAudioSampleRate)
				{
					BDBG_MSG(("\tSample Rate %s", CeaAudioSampleRateTypeText[i])) ;
				}
#endif
		} /* END ELSE uncompressed formats */

		/* update audio supported information */

		hHDMI->AttachedEDID.BcmSupportedAudioFormats[BcmAudioFormat].Supported = 1 ;

		/* update supported format only if descriptor contains a larger number of Audio Channels */
		if (EdidAudioMaxChannels > hHDMI->AttachedEDID.BcmSupportedAudioFormats[BcmAudioFormat].AudioChannels )
		{
			hHDMI->AttachedEDID.BcmSupportedAudioFormats[BcmAudioFormat].AudioChannels
				= EdidAudioMaxChannels ;
		}

		hHDMI->AttachedEDID.BcmSupportedAudioFormats[BcmAudioFormat].ucCeaSampleRates
			= EdidAudioSampleRate ;

		hHDMI->AttachedEDID.BcmSupportedAudioFormats[BcmAudioFormat].ucCeaNBits_BitRate
			= EdidAudioBits ;
	} /* for each CEA Audio Format Code Found in EDID */

	return rc ;
}


/******************************************************************************
Summary:
Set all BCM_VideoFmts that match eVideoFmt as being supported.
*******************************************************************************/
void BHDM_EDID_P_SetSupportedMatchingFmts(
	BHDM_Handle hHDMI, BFMT_VideoFmt eVideoFmt)
{
	uint8_t i ;

	uint16_t
		uiVerticalFrequency ,
		uiSupportedVerticalFrequency ;

	BFMT_VideoInfo
		*pVideoFormatInfo,
		*pSupportedVideoFormatInfo ;

	pSupportedVideoFormatInfo = (BFMT_VideoInfo *) BFMT_GetVideoFormatInfoPtr(eVideoFmt) ;
	if (!pSupportedVideoFormatInfo)
	{
		/* BFMT_VideoFmt_eCustom2 is used. BFMT does not have any
				information on this format so it returns NULL.
				HDMI does not support this custom format */

		BDBG_WRN(("No support for custom user defined format"));
		return;
	}

	BHDM_EDID_P_GetVerticalFrequency(pSupportedVideoFormatInfo->ulVertFreqMask,
			 &uiSupportedVerticalFrequency) ;

	hHDMI->AttachedEDID.BcmSupportedVideoFormats[eVideoFmt] = true ;

	for (i = 0 ; i < BFMT_VideoFmt_eMaxCount ; i++)
	{
		/* skip if already listed as supported */
		if (hHDMI->AttachedEDID.BcmSupportedVideoFormats[i])
			continue ;

		pVideoFormatInfo = (BFMT_VideoInfo *) BFMT_GetVideoFormatInfoPtr((BFMT_VideoFmt) i) ;
		if (!pVideoFormatInfo)
			continue;

		/* 1st, Check if Pixel Format matches */
		/* check the width/height, and interlace parameters */
		if ((pVideoFormatInfo->ulDigitalWidth != pSupportedVideoFormatInfo->ulDigitalWidth)
		||  (pVideoFormatInfo->ulDigitalHeight != pSupportedVideoFormatInfo->ulDigitalHeight)
		||  (pVideoFormatInfo->bInterlaced != pSupportedVideoFormatInfo->bInterlaced))
			continue ;

		/* 2nd make sure the Video Format's frequency is supported by the monitor */
		BHDM_EDID_P_GetVerticalFrequency(
			pVideoFormatInfo->ulVertFreqMask, &uiVerticalFrequency) ;

		/* if the frequency does not fall into the range the monitor supports */
		/* the format cannot be supported */
		if  ((uiVerticalFrequency < hHDMI->AttachedEDID.MonitorRange.MinVertical)
		||   (uiVerticalFrequency > hHDMI->AttachedEDID.MonitorRange.MaxVertical))
			continue ;

		/* Finally, we don't want to falsely conclude that
		   25Hz, 30Hz, and 50Hz are supported if 24Hz is
		   supported (by virtue of the above range check).
		   This is only pertinent for 720p and 1080i,p. */
		if ((pVideoFormatInfo->ulDigitalWidth == BFMT_1080I_WIDTH) 
		|| (pVideoFormatInfo->ulDigitalWidth == BFMT_1080P_WIDTH) 
		|| (pVideoFormatInfo->ulDigitalWidth == BFMT_720P_WIDTH))
		{
			if (uiVerticalFrequency != uiSupportedVerticalFrequency)
			        continue;
 		}

		/* set as supported */
		hHDMI->AttachedEDID.BcmSupportedVideoFormats[i] = true ;
	}

}  /* BHDM_EDID_P_SetSupportedMatchingFmts */


/******************************************************************************
Summary:
Return all video formats supported by the attached monitor.
*******************************************************************************/
BERR_Code BHDM_EDID_GetSupportedVideoFormats(
	BHDM_Handle hHDMI,                   /* [in] HDMI handle  */
	bool *VideoFormats                   /* [out] supported true/false */
)
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDM_EDID_GetSupportedVideoFormats) ;

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		return BHDM_EDID_NOT_FOUND;
	}

	if (hHDMI->AttachedEDID.BcmVideoFormatsChecked == 0)
		return BHDM_EDID_VIDEO_FORMATS_UNAVAILABLE ;

	BKNI_Memcpy(VideoFormats, &hHDMI->AttachedEDID.BcmSupportedVideoFormats,
		sizeof(hHDMI->AttachedEDID.BcmSupportedVideoFormats)) ;

	BDBG_LEAVE(BHDM_EDID_GetSupportedVideoFormats) ;
	return rc ;

} /* BHDM_EDID_GetSupportedVideoFormats */


#if BHDM_CONFIG_HDMI_3D_SUPPORT
/******************************************************************************
Summary:
Return all video information (video formats, video ID code) supported by the attached monitor.
*******************************************************************************/
BERR_Code BHDM_EDID_GetSupportedVideoInfo(
	BHDM_Handle hHDMI,					 /* [in] HDMI handle  */
	BHDM_EDID_VideoDescriptorInfo *stSupportedVideoInfo       /* [out] supported true/false */
)
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDM_EDID_GetSupportedVideoInfo) ;

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		rc = BHDM_EDID_NOT_FOUND;
		goto done;
	}

	if (hHDMI->AttachedEDID.BcmVideoFormatsChecked == 0) {
		rc = BHDM_EDID_VIDEO_FORMATS_UNAVAILABLE ;
		goto done;
	}

	/* zero out return structure */
	BKNI_Memset(stSupportedVideoInfo, 0, sizeof(BHDM_EDID_VideoDescriptorInfo));


	stSupportedVideoInfo->numDescriptors
		= hHDMI->AttachedEDID.NumBcmSupportedVideoDescriptors ;

	BDBG_MSG(("Num Supported Descriptors: %d", 
		hHDMI->AttachedEDID.NumBcmSupportedVideoDescriptors)) ;

	BKNI_Memcpy(
		stSupportedVideoInfo->VideoIDCode, hHDMI->AttachedEDID.BcmSupportedVideoIdCodes, 
		hHDMI->AttachedEDID.NumBcmSupportedVideoDescriptors) ;

done:

	BDBG_LEAVE(BHDM_EDID_GetSupportedVideoInfo) ;
	return rc ;
	
}
#endif


/******************************************************************************
Summary:
Return all audio formats supported by the attached monitor.
*******************************************************************************/
BERR_Code BHDM_EDID_GetSupportedAudioFormats(
	BHDM_Handle hHDMI,                          /* [in] HDMI handle  */
	BHDM_EDID_AudioDescriptor *BcmAudioFormats  /* [out] supported formats */
)
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDM_EDID_GetSupportedAudioFormats) ;

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		return BHDM_EDID_NOT_FOUND;
	}

	BKNI_Memset(BcmAudioFormats, 0, sizeof(hHDMI->AttachedEDID.BcmSupportedAudioFormats)) ;

	if (hHDMI->AttachedEDID.BcmAudioFormatsChecked == 0)
		return BHDM_EDID_AUDIO_FORMATS_UNAVAILABLE ;

	BKNI_Memcpy(BcmAudioFormats, &hHDMI->AttachedEDID.BcmSupportedAudioFormats,
		sizeof(hHDMI->AttachedEDID.BcmSupportedAudioFormats)) ;

	BDBG_LEAVE(BHDM_EDID_GetSupportedAudioFormats) ;
	return rc ;

} /* BHDM_EDID_GetSupportedAudioFormats */

void BHDM_EDID_P_ParseExtendedColorimetryDB(BHDM_Handle hHDMI, uint8_t DataBlockIndex)
{
	/* Check for xvYCC support	*/
	hHDMI->AttachedEDID.ColorimetryData.bxvYCC601= (hHDMI->AttachedEDID.Block[DataBlockIndex+2] & 0x01);
	hHDMI->AttachedEDID.ColorimetryData.bxvYCC709= (hHDMI->AttachedEDID.Block[DataBlockIndex+2] & 0x02) >> 1;

	/* Check for metadata support in colorimetry block	*/
	hHDMI->AttachedEDID.ColorimetryData.bMetadataProfile0= (hHDMI->AttachedEDID.Block[DataBlockIndex+3] & 0x01);
	hHDMI->AttachedEDID.ColorimetryData.bMetadataProfile1 = (hHDMI->AttachedEDID.Block[DataBlockIndex+3] & 0x02) >> 1;
	hHDMI->AttachedEDID.ColorimetryData.bMetadataProfile2 = (hHDMI->AttachedEDID.Block[DataBlockIndex+3] & 0x04) >> 2;

#if BDBG_DEBUG_BUILD
	BDBG_MSG(("Support for xvYCC601: %s,  xvYCC709: %s",
		g_status[hHDMI->AttachedEDID.ColorimetryData.bxvYCC601 ? 1 : 0],
		g_status[hHDMI->AttachedEDID.ColorimetryData.bxvYCC709 ? 1 : 0])) ;

	BDBG_MSG(("Gamut Metadata profile support: MD0: %s, MD1: %s, MD2: %s",
		g_status[hHDMI->AttachedEDID.ColorimetryData.bMetadataProfile0 ? 1 : 0],
		g_status[hHDMI->AttachedEDID.ColorimetryData.bMetadataProfile1 ? 1 : 0],
		g_status[hHDMI->AttachedEDID.ColorimetryData.bMetadataProfile2 ? 1 : 0])) ;
#endif
}


void BHDM_EDID_P_ParseVendorSpecificDB(BHDM_Handle hHDMI, uint8_t DataBlockIndex, uint8_t DataBlockLength)
{
	static const uint8_t ucpIEEE_RegId[3] = {0x03, 0x0C, 0x00} ; /* LSB.. MSB */
	uint8_t offset ;
	uint8_t offsetByte;

	/* once a HDMI VSDB has been found; no need to parse additional VSDBs */
	/* NOTE: All EDID variables are cleared at BHDM_EDID_Initialize */
	if (hHDMI->AttachedEDID.RxHasHdmiSupport)
		return ;

	BKNI_Memset(&hHDMI->AttachedEDID.RxVSDB, 0, sizeof(BHDM_EDID_RxVendorSpecificDB)) ;

	/* get the 24 bit IEEE Registration Identifier */
	BKNI_Memcpy(&hHDMI->AttachedEDID.RxVSDB.ucpIEEE_RegId, &hHDMI->AttachedEDID.Block[DataBlockIndex+1], 3) ;

#if BHDM_CONFIG_HDMI_3D_SUPPORT
	/* indicate 3D Formats checked even if we do not proces due to DVI device (no HDMI) or no 3D Support */
	hHDMI->AttachedEDID.Bcm3DFormatsChecked = true;
#endif

	/* make sure it is the correct IEEE Registration ID */
	if (BKNI_Memcmp(hHDMI->AttachedEDID.RxVSDB.ucpIEEE_RegId, ucpIEEE_RegId, 3) != 0)
	{
		BDBG_ERR(("VSDB IEEE Reg ID <%02X%02X%02X> != HDMI Reg ID <%02X%02X%02X>",
			hHDMI->AttachedEDID.RxVSDB.ucpIEEE_RegId[2], hHDMI->AttachedEDID.RxVSDB.ucpIEEE_RegId[1],
			hHDMI->AttachedEDID.RxVSDB.ucpIEEE_RegId[0],
			ucpIEEE_RegId[2], ucpIEEE_RegId[1], ucpIEEE_RegId[0])) ;
		
		BDBG_ERR(("Rx Device will be treated as a DVI device...")) ;
		hHDMI->AttachedEDID.RxHasHdmiSupport = 0 ;
		return ;
	}
	else
	{
		BDBG_MSG(("HDMI VSDB IEEE Registration ID %02X%02X%02X Found",
			hHDMI->AttachedEDID.RxVSDB.ucpIEEE_RegId[2],
			hHDMI->AttachedEDID.RxVSDB.ucpIEEE_RegId[1],
			hHDMI->AttachedEDID.RxVSDB.ucpIEEE_RegId[0])) ;
		hHDMI->AttachedEDID.RxHasHdmiSupport = 1 ;
		
	}

	/* get the components of My Source Physical Address */
	hHDMI->AttachedEDID.RxVSDB.PhysAddr_A = (hHDMI->AttachedEDID.Block[DataBlockIndex+4] & 0xF0) >> 4 ;
	hHDMI->AttachedEDID.RxVSDB.PhysAddr_B = (hHDMI->AttachedEDID.Block[DataBlockIndex+4] & 0x0F) ;
	hHDMI->AttachedEDID.RxVSDB.PhysAddr_C = (hHDMI->AttachedEDID.Block[DataBlockIndex+5] & 0xF0) >> 4 ;
	hHDMI->AttachedEDID.RxVSDB.PhysAddr_D = (hHDMI->AttachedEDID.Block[DataBlockIndex+5] & 0x0F) ;
	BDBG_MSG(("my_address (CEC physical) = (%d.%d.%d.%d)",
		hHDMI->AttachedEDID.RxVSDB.PhysAddr_A, hHDMI->AttachedEDID.RxVSDB.PhysAddr_B,
		hHDMI->AttachedEDID.RxVSDB.PhysAddr_C, hHDMI->AttachedEDID.RxVSDB.PhysAddr_D)) ;

#if BHDM_CONFIG_CEC_LEGACY_SUPPORT && BHDM_CEC_SUPPORT
	BKNI_Memcpy(hHDMI->cecConfiguration.CecPhysicalAddr,
		&hHDMI->AttachedEDID.Block[DataBlockIndex+4], 2) ;
#endif

	/* DataBlockLength is N (which is 1 less than actual length). */
	if (DataBlockLength >= 6)
	{
		/* for HDMI 1.1 Check if Rx supports ACP or ISRC packets */
		hHDMI->AttachedEDID.RxVSDB.SupportsAI = (hHDMI->AttachedEDID.Block[DataBlockIndex+6] & 0x80) >> 7 ;
		BDBG_MSG(("Supports AI = %d", hHDMI->AttachedEDID.RxVSDB.SupportsAI)) ;

		/* retrieve support for Deep Color modes	*/
		hHDMI->AttachedEDID.RxVSDB.DeepColor_48bit = (hHDMI->AttachedEDID.Block[DataBlockIndex+6] & 0x40) >> 6;
		hHDMI->AttachedEDID.RxVSDB.DeepColor_36bit = (hHDMI->AttachedEDID.Block[DataBlockIndex+6] & 0x20) >> 5;
		hHDMI->AttachedEDID.RxVSDB.DeepColor_30bit = (hHDMI->AttachedEDID.Block[DataBlockIndex+6] & 0x10) >> 4;
		hHDMI->AttachedEDID.RxVSDB.DeepColor_Y444 = (hHDMI->AttachedEDID.Block[DataBlockIndex+6] & 0x08) >> 3;
		hHDMI->AttachedEDID.RxVSDB.DVI_Dual = (hHDMI->AttachedEDID.Block[DataBlockIndex+6] & 0x01);
	}
	else
	{
		hHDMI->AttachedEDID.RxVSDB.SupportsAI = false;
		hHDMI->AttachedEDID.RxVSDB.DeepColor_48bit = false;
		hHDMI->AttachedEDID.RxVSDB.DeepColor_36bit = false;
		hHDMI->AttachedEDID.RxVSDB.DeepColor_30bit = false;
		hHDMI->AttachedEDID.RxVSDB.DeepColor_Y444 = false;
		hHDMI->AttachedEDID.RxVSDB.DVI_Dual = false;
	}

	if (DataBlockLength >= 7)
	{
		/* get the maximum TMDS clock rate supported	*/
		hHDMI->AttachedEDID.RxVSDB.Max_TMDS_Clock_Rate =
			hHDMI->AttachedEDID.Block[DataBlockIndex+7] * 5;
	}

	/* determine present/non present fields */
	if (DataBlockLength >= 8)
	{
		offsetByte = hHDMI->AttachedEDID.Block[DataBlockIndex+8] ;

		/* get lipsync-related fields	*/
		hHDMI->AttachedEDID.RxVSDB.Latency_Fields_Present =
			offsetByte >> 7 ;

		hHDMI->AttachedEDID.RxVSDB.Interlaced_Latency_Fields_Present =
			(offsetByte & 0x40) >> 6 ;

#if BHDM_CONFIG_HDMI_3D_SUPPORT
		/* HDMI Video Present */
		hHDMI->AttachedEDID.RxVSDB.HDMI_Video_Present=
			(offsetByte & 0x20) >> 5 ;


		/* get Content Type related fields */
		hHDMI->AttachedEDID.RxVSDB.SupportedContentTypeGraphicsText =
			offsetByte & 0x01 ;

		hHDMI->AttachedEDID.RxVSDB.SupportedContentTypePhoto =
			offsetByte & 0x02 ;

		hHDMI->AttachedEDID.RxVSDB.SupportedContentTypeCinema =
			offsetByte & 0x04 ;

		hHDMI->AttachedEDID.RxVSDB.SupportedContentTypeGame =
			offsetByte & 0x08 ;
#endif		
	}


	offset = 9 ;  /* earliest byte where optional data can be located */
	if (offset > DataBlockLength)  /* no more optional data */
		goto done ;

	/* retrieve Video and Audio Latency if available	*/
	if (hHDMI->AttachedEDID.RxVSDB.Latency_Fields_Present)
	{
		hHDMI->AttachedEDID.RxVSDB.Video_Latency =
				hHDMI->AttachedEDID.Block[DataBlockIndex + offset] ;
		offset++  ;

		hHDMI->AttachedEDID.RxVSDB.Audio_Latency =
				hHDMI->AttachedEDID.Block[DataBlockIndex + offset] ;
		offset++ ;
	}
	else
	{
		/* force Interlaced Latency Fields Present to false when no Progressive Latency fields */
		/* cannot have Interlaced Latency Fields without Progressive Latency Fields */
		hHDMI->AttachedEDID.RxVSDB.Interlaced_Latency_Fields_Present = false ;
	}

	/* retrieve Video and Audio Latency for interlaced formats if available	*/
	if (hHDMI->AttachedEDID.RxVSDB.Interlaced_Latency_Fields_Present)
	{
		hHDMI->AttachedEDID.RxVSDB.Interlaced_Video_Latency =
				hHDMI->AttachedEDID.Block[DataBlockIndex + offset] ;
		offset++ ;

		hHDMI->AttachedEDID.RxVSDB.Interlaced_Audio_Latency =
				hHDMI->AttachedEDID.Block[DataBlockIndex + offset] ;
		offset++ ;
	}

	/* AV and interlaced AV latencies have been retrieved */
	if (offset > DataBlockLength)
		goto done ;  /* no more optional data */


#if BHDM_CONFIG_HDMI_3D_SUPPORT	
	/* Now check for HDMI VICs and 3D support info */
	BHDM_EDID_P_ParseVSDB3D(hHDMI, DataBlockIndex, &offset, DataBlockLength);
#endif


done:
#if BHDM_CONFIG_DEBUG_EDID_VSDB
	if ((DataBlockLength < 7)
	&& (hHDMI->AttachedEDID.RxVSDB.DeepColor_48bit
	||  hHDMI->AttachedEDID.RxVSDB.DeepColor_36bit
	||  hHDMI->AttachedEDID.RxVSDB.DeepColor_30bit
	||  hHDMI->AttachedEDID.RxVSDB.DeepColor_Y444
	||  hHDMI->AttachedEDID.RxVSDB.DVI_Dual))
	{
		BDBG_WRN(("Found HDMI 1.3 Features")) ;
		BDBG_WRN(("Deep Color Support")) ;
		BDBG_WRN(("\t 48bit: %s, 36bit: %s, 30bit: %s,  Y444: %s",
			g_status[hHDMI->AttachedEDID.RxVSDB.DeepColor_48bit ? 1 : 0],
			g_status[hHDMI->AttachedEDID.RxVSDB.DeepColor_36bit ? 1 : 0],
			g_status[hHDMI->AttachedEDID.RxVSDB.DeepColor_30bit ? 1 : 0],
			g_status[hHDMI->AttachedEDID.RxVSDB.DeepColor_Y444 ? 1 : 0])) ;

		BDBG_WRN(("DVI Dual Link Support: %s",
			g_status[hHDMI->AttachedEDID.RxVSDB.DVI_Dual ? 1 : 0]));
		return ;
	}

	BDBG_WRN(("Max TMDS Clock Rate: %d",
			hHDMI->AttachedEDID.RxVSDB.Max_TMDS_Clock_Rate));

	BDBG_WRN(("Video Latency Data Present:            %s",
		g_status[hHDMI->AttachedEDID.RxVSDB.Latency_Fields_Present ? 1 : 0])) ;
	if (hHDMI->AttachedEDID.RxVSDB.Latency_Fields_Present)
	{
		BDBG_WRN(("\tVideo: %d, Audio: %d",
			hHDMI->AttachedEDID.RxVSDB.Video_Latency,
			hHDMI->AttachedEDID.RxVSDB.Audio_Latency)) ;
	}

	BDBG_WRN(("Interlaced Video Latency Data Present: %s",
		g_status[hHDMI->AttachedEDID.RxVSDB.Interlaced_Latency_Fields_Present ? 1 : 0])) ;
	if (hHDMI->AttachedEDID.RxVSDB.Interlaced_Latency_Fields_Present)
	{
		BDBG_WRN(("\tVideo: %d, Audio: %d",
			hHDMI->AttachedEDID.RxVSDB.Interlaced_Video_Latency,
			hHDMI->AttachedEDID.RxVSDB.Interlaced_Audio_Latency));
	}

	BDBG_WRN(("HDMI Video Present: %s",
		g_status[hHDMI->AttachedEDID.RxVSDB.HDMI_Video_Present ? 1 : 0])) ;

#if BHDM_CONFIG_HDMI_3D_SUPPORT
	if (hHDMI->AttachedEDID.RxVSDB.HDMI_Video_Present)
	{
		BDBG_WRN(("Found HDMI 1.4 Features")) ;
		BDBG_WRN(("Content Types Supported")) ;
		BDBG_WRN(("\tGraphics (Text): %s",
			g_status[hHDMI->AttachedEDID.RxVSDB.SupportedContentTypeGraphicsText ? 1 : 0])) ;
		BDBG_WRN(("\tCinema: %s",
			g_status[hHDMI->AttachedEDID.RxVSDB.SupportedContentTypeCinema ? 1 : 0])) ;
		BDBG_WRN(("\tPhoto: %s",
			g_status[hHDMI->AttachedEDID.RxVSDB.SupportedContentTypePhoto ? 1 : 0])) ;
		BDBG_WRN(("\tGame: %s",
			g_status[hHDMI->AttachedEDID.RxVSDB.SupportedContentTypeGame ? 1 : 0])) ;

		BDBG_WRN(("HDMI 3D Present: %s",
			g_status[hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Present ? 1 : 0])) ;

		BDBG_WRN(("HDMI Multi 3D Present (Optional): %#x",
			hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Multi_Present)) ;


		BDBG_WRN(("HDMI VIC LEN: %d",
			hHDMI->AttachedEDID.RxVSDB.HDMI_VIC_Len)) ;

		BDBG_WRN(("HDMI  3D LEN: %d",
			hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Len)) ;

		BDBG_WRN(("Image Size: %d",
			hHDMI->AttachedEDID.RxVSDB.HDMI_Image_Size)) ;


		if (hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Present)
		{
			uint8_t i;
			BDBG_WRN(("Broadcom 3D Formats Supported:")) ;
			for (i = 0 ; i < BFMT_VideoFmt_eMaxCount; i++)
			{
				const BFMT_VideoInfo *pVideoFormatInfo ;
				pVideoFormatInfo = (BFMT_VideoInfo *) BFMT_GetVideoFormatInfoPtr((BFMT_VideoFmt) i) ;

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FRAME_PACKING) {
					BDBG_WRN(("\t%s Frame Packing ", pVideoFormatInfo->pchFormatStr)) ;
				}

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FIELD_ALTERNATIVE) {
					BDBG_WRN(("\t%s FieldAlternative ",	pVideoFormatInfo->pchFormatStr)) ;
				}

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LINE_ALTERNATIVE) {
					BDBG_WRN(("\t%s LineAlternative ", pVideoFormatInfo->pchFormatStr)) ;
				}

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LDEPTH) {
					BDBG_WRN(("\t%s LDepth ", pVideoFormatInfo->pchFormatStr)) ;
				}

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LDEPTH_GFX)	{
					BDBG_WRN(("\t%s LDepth+Graphics ", pVideoFormatInfo->pchFormatStr)) ;
				}

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_TOP_BOTTOM) {
					BDBG_WRN(("\t%s TopAndBottom ", pVideoFormatInfo->pchFormatStr)) ;
				}

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_FULL) {
					BDBG_WRN(("\t%s SideBySide_Full ", pVideoFormatInfo->pchFormatStr)) ;
				}

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_HALF_HORIZ) {
					BDBG_WRN(("\t%s SideBySide_Half_Horizontal ", pVideoFormatInfo->pchFormatStr)) ;
				}

				if (hHDMI->AttachedEDID.BcmSupported3DFormats[i] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_HALF_QUINC) {
					BDBG_WRN(("\t%s SideBySide_Half_QuincunxMatrix ", pVideoFormatInfo->pchFormatStr)) ;
				}
			}
		}
	}
#endif	
#endif

	return;

}

BERR_Code BHDM_EDID_P_ParseV1V2TimingExtension(BHDM_Handle hHDMI)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t
		i, offset,
		DataOffset ;
	BFMT_VideoFmt eVideoFmt ;
	BHDM_EDID_DetailTiming DetailTimingBlock ;

#if BDBG_DEBUG_BUILD
	uint8_t TimingsFound = 0 ;
#endif


	/* check if data blocks exist before the detailed timing data */
	DataOffset = hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_DATA_OFFSET] ;
	if (DataOffset == 0)  /* no detailed timing descriptors */
		return rc ;

	i = 0 ;
	offset = DataOffset ;

	BKNI_Memset((void *) &hHDMI->AttachedEDID.DescriptorHeader, 0x0, BHDM_EDID_DESC_HEADER_LEN) ;


	while (offset < BHDM_EDID_BLOCKSIZE)
	{
		if (BKNI_Memcmp(&hHDMI->AttachedEDID.Block[offset], hHDMI->AttachedEDID.DescriptorHeader,
			BHDM_EDID_DESC_HEADER_LEN) == 0)
		{
			break ;
		}

		/* skip EDID descriptor blocks */
		if (BKNI_Memcmp(&hHDMI->AttachedEDID.Block[offset], (void *) &hHDMI->AttachedEDID.DescriptorHeader, 3) == 0)
		{
			offset = DataOffset + BHDM_EDID_MONITOR_DESC_SIZE * i++ ;
			continue ;
		}

		BHDM_EDID_ParseDetailedTimingDescriptor(
			&hHDMI->AttachedEDID.Block[offset], &DetailTimingBlock) ;

		/* convert the DetailTiming to BFMT_eVideoFmt type */
		if (BHDM_EDID_P_DetailTiming2VideoFmt(hHDMI, &DetailTimingBlock, &eVideoFmt)
		== BERR_SUCCESS)
		{
			/* set BFMTs that match this Detailed Timing Format as being supported */
			BHDM_EDID_P_SetSupportedMatchingFmts(hHDMI, eVideoFmt) ;

#if BDBG_DEBUG_BUILD
			BDBG_MSG(("Preferred Timing #%d found in V1/2 Timing Ext block", ++TimingsFound)) ;
#endif
		}

		offset = DataOffset + BHDM_EDID_MONITOR_DESC_SIZE * i++ ;
	}
	return rc ;
}


BERR_Code BHDM_EDID_P_ParseV3TimingExtension (BHDM_Handle hHDMI)
{
	BERR_Code rc = BERR_SUCCESS;
	uint8_t
		DataOffset,
		DataBlockIndex,
		DataBlockTag,
		DataBlockLength,
		MonitorSupport,
		ExtendedTagCode;

	DataOffset = hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_DATA_OFFSET] ;

	/* check if data blocks exist before the detailed timing data */
	if ((DataOffset != 0) &&
		(DataOffset != 4)) /* yes, data blocks exist. */
	{
		/* Look for Video Data Block and parse it first
		Set the index to the start of Data Blocks */
		DataBlockIndex = BHDM_EDID_EXT_DATA_BLOCK_COLLECTION ;
		
		while (DataBlockIndex < DataOffset)
		{
			/* get the Data Block type */
			DataBlockTag =
					  hHDMI->AttachedEDID.Block[DataBlockIndex] >> 5 ;
		
			/* get the Data Block length */
			DataBlockLength =
					  hHDMI->AttachedEDID.Block[DataBlockIndex] & 0x1F ;

			if (DataBlockTag == BHDM_EDID_CeaDataBlockTag_eVideoDB)
			{			
				BDBG_MSG(("\n[%#02X] CEA-861 %s (%#02x) found; %d bytes",
						  hHDMI->AttachedEDID.Block[DataBlockIndex],
						  CeaTagName[DataBlockTag], DataBlockTag, DataBlockLength)) ;

				/* adds to supported BCM video formats */
				BHDM_EDID_P_ParseVideoDB(hHDMI, DataBlockIndex, DataBlockLength) ;
				break ;
			}	
			DataBlockIndex += DataBlockLength + 1;
		} /* while DataBlockIndex < DataOffset */


		/* scan through the data blocks once again and retrieve all other the necessary information.
		Set the index to the start of Data Blocks */
		DataBlockIndex = BHDM_EDID_EXT_DATA_BLOCK_COLLECTION ;
		
		while (DataBlockIndex < DataOffset)
		{
			/* get the Data Block type */
			DataBlockTag =
					  hHDMI->AttachedEDID.Block[DataBlockIndex] >> 5 ;

			/* get the Data Block length */
			DataBlockLength =
					  hHDMI->AttachedEDID.Block[DataBlockIndex] & 0x1F ;

			BDBG_MSG(("\n[%#02X] CEA-861 %s (%#02x) found; %d bytes",
					  hHDMI->AttachedEDID.Block[DataBlockIndex],
					  CeaTagName[DataBlockTag], DataBlockTag, DataBlockLength)) ;

			switch (DataBlockTag)
			{
			case BHDM_EDID_CeaDataBlockTag_eAudioDB :	/* Audio DB */
				/* adds to supported BCM audio formats */
				BHDM_EDID_P_ParseAudioDB(hHDMI, DataBlockIndex, DataBlockLength) ;
				break ;

			case BHDM_EDID_CeaDataBlockTag_eVideoDB :	/* Video DB */
				/* Video Data Block already parse at this point */
				break ;

			case BHDM_EDID_CeaDataBlockTag_eVSDB :		/* Vendor Specific DB */
				/* populates RxVSDB */
				BHDM_EDID_P_ParseVendorSpecificDB(hHDMI, DataBlockIndex, DataBlockLength) ;
				break ;

			case BHDM_EDID_CeaDataBlockTag_eSpeakerDB : /* Speaker Allocation DB */
				break ;

			case BHDM_EDID_CeaDataBlockTag_eExtendedDB:
				ExtendedTagCode = hHDMI->AttachedEDID.Block[DataBlockIndex+1];
				if (ExtendedTagCode == 5)
					BHDM_EDID_P_ParseExtendedColorimetryDB(hHDMI, DataBlockIndex) ;
				else
				{
					BDBG_MSG((" Extended Tag Code <%d> is not supported",
							ExtendedTagCode)) ;
				}
				break;

			case BHDM_EDID_CeaDataBlockTag_eReserved0 :
			case BHDM_EDID_CeaDataBlockTag_eReserved5 :
			case BHDM_EDID_CeaDataBlockTag_eReserved6 :


			default : /* note any Unknown/Unsupported Tags */
				BDBG_WRN(("CEA Data Block Tag Code %d is not supported",
					   DataBlockTag)) ;
				break ;
			}

			DataBlockIndex += DataBlockLength + 1;

		} /* while DataBlockIndex < DataOffset */
	}


	/* check what the monitor supports */
	MonitorSupport = hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_MONITOR_SUPPORT] ;
	
	hHDMI->AttachedEDID.RxVSDB.Underscan = MonitorSupport & 0x80 ;
	hHDMI->AttachedEDID.RxVSDB.Audio	 = MonitorSupport & 0x40 ;
	hHDMI->AttachedEDID.RxVSDB.YCbCr444  = MonitorSupport & 0x20 ;
	hHDMI->AttachedEDID.RxVSDB.YCbCr422  = MonitorSupport & 0x10 ;
	hHDMI->AttachedEDID.RxVSDB.NativeFormatsInDescriptors = MonitorSupport & 0xF ;

	/* check if there are any detailed timing descriptors. */
	if (DataOffset > 0)
	{
		uint16_t offset = DataOffset ;
		BHDM_EDID_DetailTiming DetailTimingBlock ;
		BFMT_VideoFmt eVideoFmt ;

		BKNI_Memset((void *) &hHDMI->AttachedEDID.DescriptorHeader, 0x0, BHDM_EDID_DESC_HEADER_LEN);

		while ((offset > 0) && (offset < BHDM_EDID_BLOCKSIZE)
			&& BKNI_Memcmp( /* detailed timing descriptors contain non-zeros */
				&hHDMI->AttachedEDID.Block[offset],
				&hHDMI->AttachedEDID.DescriptorHeader, 3))
		{
			BHDM_EDID_ParseDetailedTimingDescriptor(
				&hHDMI->AttachedEDID.Block[offset], &DetailTimingBlock) ;

			/* convert the DetailTiming to BFMT_eVideoFmt type */
			rc = BHDM_EDID_P_DetailTiming2VideoFmt(hHDMI, &DetailTimingBlock, &eVideoFmt) ;
			if (rc == BERR_SUCCESS)
			{
				/* set BFMTs that match this Detailed Timing Format as being supported */
				BHDM_EDID_P_SetSupportedMatchingFmts(hHDMI, eVideoFmt) ;
			}

			/* skip to next descriptor */
			offset = offset + BHDM_EDID_MONITOR_DESC_SIZE ;
		}
	}

	/* RxVSDB has been  parsed at this point load additional values found in the V3 timing extension */
	/* all HDMI Rx that set VSDB.Audio must support Basic Audio (i.e 2 Channel PCM)
		 although not all devices explicitly specify it in Audio Descriptors */
	if (!hHDMI->AttachedEDID.RxVSDB.Audio)
	{
		BDBG_WRN(("Attached HDMI device '%s' does not support audio",
			hHDMI->AttachedEDID.MonitorName)) ;
	}
	else if (!hHDMI->AttachedEDID.BcmSupportedAudioFormats[BAVC_AudioFormat_ePCM].Supported)
	{
		BDBG_WRN(("VSDB indicates audio support, but no PCM Audio Descriptors found!")) ;
		BDBG_WRN(("Assuming 2 channel PCM is supported")) ;

		/* all HDMI Rxs are required to support PCM Audio; list 48KHz PCM as supported */
		hHDMI->AttachedEDID.BcmSupportedAudioFormats[BAVC_AudioFormat_ePCM].Supported = 1 ;
		hHDMI->AttachedEDID.BcmSupportedAudioFormats[BAVC_AudioFormat_ePCM].AudioChannels = 2 ;
		hHDMI->AttachedEDID.BcmSupportedAudioFormats[BAVC_AudioFormat_ePCM].ucCeaSampleRates
			= BAVC_AudioSamplingRate_e48k ;
		hHDMI->AttachedEDID.BcmSupportedAudioFormats[BAVC_AudioFormat_ePCM].ucCeaNBits_BitRate
			= 1 ;	/* 16 bits */
		hHDMI->AttachedEDID.BcmAudioFormatsChecked = 1 ;
	}

	return rc;

}


BERR_Code BHDM_EDID_P_ProcessTimingExtension (BHDM_Handle hHDMI)
{
	BERR_Code rc = BERR_SUCCESS;

	/* check for Version 1/2/3 Timing Data Extensions */
	switch (hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_VERSION])
	{
	case BHDM_EDID_TIMING_VERSION_1 :
	case BHDM_EDID_TIMING_VERSION_2 :
		BDBG_MSG(("Parse Version 1/2 Timing Extension ")) ;
		BHDM_EDID_P_ParseV1V2TimingExtension(hHDMI) ;
		break  ;

	case BHDM_EDID_TIMING_VERSION_3 :
		BDBG_MSG(("V3 Timing Extension Found in EDID Extension.")) ;
		BHDM_EDID_P_ParseV3TimingExtension(hHDMI) ;
		break ;

	case BHDM_EDID_TIMING_VERSION_4 :
		BDBG_WRN(("Parsing Timing Extension Version 4 as Version 3")) ;
		BHDM_EDID_P_ParseV3TimingExtension(hHDMI) ;
		break ;

	default :
		BDBG_WRN(("Uknown/Unsupported Timing Extension Version %d",
			hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_VERSION])) ;
	}

	return rc;
}

BERR_Code BHDM_EDID_P_ProcessExtensionBlock (BHDM_Handle hHDMI)
{
	BERR_Code rc = BERR_SUCCESS;

	/* check for Extension type */
	switch (hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_TAG])
	{
	case BHDM_EDID_EXT_TIMING_DATA :
		BDBG_MSG(("Timing Extension Block Found in EDID Extension Blocks")) ;
		BHDM_EDID_P_ProcessTimingExtension(hHDMI) ;
		break;

	case BHDM_EDID_EXT_LCD_TIMINGS :
	case BHDM_EDID_EXT_EDID_2_0 :
	case BHDM_EDID_EXT_COLOR_INFO :
	case BHDM_EDID_EXT_DVI_FEATURE :
	case BHDM_EDID_EXT_TOUCH_SCREEN :
	case BHDM_EDID_EXT_MANUFACTURER :
	default :
		BDBG_WRN(("Uknown/Unsupported Extension  %d",
			hHDMI->AttachedEDID.Block[BHDM_EDID_EXT_TAG])) ;
	}

	return rc;
}

BERR_Code BHDM_EDID_P_ProcessBlockMap (BHDM_Handle hHDMI)
{
	BERR_Code rc = BERR_SUCCESS;
	uint8_t block_map[BHDM_EDID_BLOCKSIZE];
	uint8_t i, index;

	index = hHDMI->AttachedEDID.CachedBlock;

	/* Copy the block map from the hHDMI handle */
	for(i=0; i< BHDM_EDID_BLOCKSIZE; i++){
		block_map[i]=hHDMI->AttachedEDID.Block[i];
	}

	i = 1;
	while((i < (BHDM_EDID_BLOCKSIZE-1)) && (block_map[i])){
		BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI, (i + index), hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;
		BHDM_EDID_P_ProcessExtensionBlock (hHDMI);
		i++;
	}

	if(hHDMI->AttachedEDID.BasicData.Extensions < BHDM_EDID_BLOCKSIZE) {
		if(hHDMI->AttachedEDID.BasicData.Extensions != i) {
			hHDMI->AttachedEDID.BasicData.Extensions = i;
		}
	}
	else {
		if(hHDMI->AttachedEDID.BasicData.Extensions != (i+index)) {
			hHDMI->AttachedEDID.BasicData.Extensions = i;
		}
	}

done:
	return rc;
}

/******************************************************************************
Summary:
Notify the EDID Block to re-read the EDID; initializing the values read from the EDID
*******************************************************************************/
BERR_Code BHDM_EDID_Initialize(
	BHDM_Handle hHDMI                    /* [in] HDMI handle  */
)
{
	BERR_Code rc ;
	uint8_t
		offset,
		Tag ;
	uint8_t SupportedTimingsFound = 0 ;
	BFMT_VideoFmt eVideoFmt ;

	uint8_t i; /* indexes */

	uint8_t RxDeviceAttached ;
	static const uint8_t ucEdidHeader[BHDM_EDID_HEADER_SIZE] =
		{ 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00} ;

	BHDM_EDID_DetailTiming DetailTiming ;
	BHDM_EDID_P_VideoDescriptor *pVideoDescriptor ;


	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		return BHDM_NO_RX_DEVICE ;
	}

	/* dont bother reading if EDID has been bypassed (DEBUG mode only) */
#if BDBG_DEBUG_BUILD
	if (hHDMI->DeviceSettings.BypassEDIDChecking)
	{
		BDBG_WRN(("EDID is ByPassed; all formats will attempt to display")) ;

		for (i = 0 ; i < BFMT_VideoFmt_eMaxCount ; i++)
			hHDMI->AttachedEDID.BcmSupportedVideoFormats[i] = true ; /* set as supported */
		hHDMI->AttachedEDID.BcmVideoFormatsChecked = 1 ;

		goto done ;
	}
#endif

	/* delete previous video descriptors if they exist */
	if (!BLST_Q_EMPTY(&hHDMI->AttachedEDID.VideoDescriptorList))
	{
		for (pVideoDescriptor=BLST_Q_FIRST(&hHDMI->AttachedEDID.VideoDescriptorList) ;
			pVideoDescriptor ;
			pVideoDescriptor=BLST_Q_FIRST(&hHDMI->AttachedEDID.VideoDescriptorList))
		{
			BLST_Q_REMOVE_HEAD(&hHDMI->AttachedEDID.VideoDescriptorList, link);
			BKNI_Free(pVideoDescriptor); /* free memory */
		}
	}
	/* clear all current EDID information */
  	BKNI_Memset((void *) &hHDMI->AttachedEDID, 0, sizeof(BHDM_EDID_DATA)) ;

	BDBG_MSG(("Initializing/Reading EDID Information")) ;

	/* read the 1st EDID Block and parse the information of interest it contains */

	/* during InitializeEDID, always force the reading of EDID block 0 */
	/* incorrectly implemented Hot Plug signals sometimes cause a problem */
	hHDMI->edidStatus = BHDM_EDID_STATE_eInitialize;
	BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI, 0, hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;

	/**************************************
	 Parse Basic EDID Data
	**************************************/

	if (BKNI_Memcmp(&ucEdidHeader[0], &hHDMI->AttachedEDID.Block[0],
		BHDM_EDID_HEADER_SIZE))
	{
		BDBG_WRN(("Invalid/Missing EDID Header %02X %02X %02X %02X %02X %02X %02X %02X... ignoring",
			hHDMI->AttachedEDID.Block[0], hHDMI->AttachedEDID.Block[1],
			hHDMI->AttachedEDID.Block[2], hHDMI->AttachedEDID.Block[3],
			hHDMI->AttachedEDID.Block[4], hHDMI->AttachedEDID.Block[5],
			hHDMI->AttachedEDID.Block[6], hHDMI->AttachedEDID.Block[7])) ;
	}

	BKNI_Memcpy(&hHDMI->AttachedEDID.BasicData.VendorID,
		&hHDMI->AttachedEDID.Block[BHDM_EDID_VENDOR_ID], 2) ;

	BKNI_Memcpy(&hHDMI->AttachedEDID.BasicData.ProductID,
		&hHDMI->AttachedEDID.Block[BHDM_EDID_PRODUCT_ID], 2) ;

	BKNI_Memcpy(&hHDMI->AttachedEDID.BasicData.SerialNum,
		&hHDMI->AttachedEDID.Block[BHDM_EDID_SERIAL_NO], 4) ;

	 hHDMI->AttachedEDID.BasicData.ManufWeek=
	 	hHDMI->AttachedEDID.Block[BHDM_EDID_MANUFACTURE_WEEK] ;
	 hHDMI->AttachedEDID.BasicData.ManufYear =
	 	hHDMI->AttachedEDID.Block[BHDM_EDID_MANUFACTURE_YEAR] ;
	 hHDMI->AttachedEDID.BasicData.features =
	 	hHDMI->AttachedEDID.Block[BHDM_EDID_FEATURE_SUPPORT] ;


	hHDMI->AttachedEDID.BasicData.EdidVersion = hHDMI->AttachedEDID.Block[BHDM_EDID_VERSION] ;
	hHDMI->AttachedEDID.BasicData.EdidRevision = hHDMI->AttachedEDID.Block[BHDM_EDID_REVISION] ;
	hHDMI->AttachedEDID.BasicData.Extensions = hHDMI->AttachedEDID.Block[BHDM_EDID_EXTENSION] ;

	if ((hHDMI->AttachedEDID.BasicData.EdidVersion == 0xFF)
	||  (hHDMI->AttachedEDID.BasicData.EdidRevision	== 0xFF)
	||  (hHDMI->AttachedEDID.BasicData.Extensions == 0xFF)
	|| ( (hHDMI->AttachedEDID.BasicData.EdidVersion == 0x00) &&
		(hHDMI->AttachedEDID.BasicData.EdidRevision == 0x00) &&
		(hHDMI->AttachedEDID.BasicData.Extensions == 0x00) ))
	{
		/* probably read all 0xFF or 0x00; invalid EDID */
		BDBG_ERR(("EDID returns all 0xFF or 0x00 values. Invalid EDID information"));
		hHDMI->edidStatus = BHDM_EDID_STATE_eInvalid;
		return BHDM_EDID_NOT_FOUND;
	}

	BDBG_MSG(("Version %d.%d  Number of Extensions: %d",
		hHDMI->AttachedEDID.BasicData.EdidVersion,
		hHDMI->AttachedEDID.BasicData.EdidRevision, hHDMI->AttachedEDID.BasicData.Extensions)) ;

	hHDMI->AttachedEDID.BasicData.MaxHorizSize = hHDMI->AttachedEDID.Block[BHDM_EDID_MAX_HORIZ_SIZE] ;
	hHDMI->AttachedEDID.BasicData.MaxVertSize  = hHDMI->AttachedEDID.Block[BHDM_EDID_MAX_VERT_SIZE] ;
	BDBG_MSG(("H x V Size (cm): %d x %d",
		hHDMI->AttachedEDID.BasicData.MaxHorizSize,
		hHDMI->AttachedEDID.BasicData.MaxVertSize)) ;

	/*
	** Additional Information for Display Purposes only
	** Add to EDID Data structure if desired...
	*/
	BDBG_MSG(("Manufacture Week / Year: %d / %d",
		hHDMI->AttachedEDID.Block[BHDM_EDID_MANUFACTURE_WEEK],
		hHDMI->AttachedEDID.Block[BHDM_EDID_MANUFACTURE_YEAR] + 1990)) ;

	/* Serial Number may also be placed in a Descriptor Tag */
	BDBG_MSG(("Serial Number: %02X %02X %02X %02X",
		hHDMI->AttachedEDID.Block[BHDM_EDID_SERIAL_NO], hHDMI->AttachedEDID.Block[BHDM_EDID_SERIAL_NO+1],
		hHDMI->AttachedEDID.Block[BHDM_EDID_SERIAL_NO+2], hHDMI->AttachedEDID.Block[BHDM_EDID_SERIAL_NO+3])) ;

	{
         #if BDBG_DEBUG_BUILD
		 static const char *DisplayType[] =
		 {
		 	"Monochrome Display",
		 	"RGB Display",
		 	"Non-RGB Display",
		 	"Undefined"
		 } ;
		 #endif
		 BDBG_MSG(("Feature Support: <%#x>", hHDMI->AttachedEDID.BasicData.features)) ;
		 BDBG_MSG(("\tStandby Supported: %s",
		 	g_status[(hHDMI->AttachedEDID.BasicData.features & 0x80) ? 1 : 0])) ;
		 BDBG_MSG(("\tSuspend Supported: %s",
			 g_status[(hHDMI->AttachedEDID.BasicData.features & 0x40) ? 1 : 0])) ;
		 BDBG_MSG(("\tActive Off Supported: %s",
			 g_status[(hHDMI->AttachedEDID.BasicData.features & 0x20) ? 1 : 0])) ;
		 BDBG_MSG(("\tDisplay Type: %s",
		 	DisplayType[ (hHDMI->AttachedEDID.BasicData.features & 0x18) >> 3])) ;
		 BDBG_MSG(("\tsRGB Colorspace Supported: %s",
			 g_status[(hHDMI->AttachedEDID.BasicData.features & 0x04) ? 1 : 0])) ;
		 BDBG_MSG(("\tPreferred Timing in Block 1 (must always be set): %s",
			 g_status[(hHDMI->AttachedEDID.BasicData.features & 0x02) ? 1 : 0])) ;
		 BDBG_MSG(("\tDefault GTF Supported: %s",
			 g_status[(hHDMI->AttachedEDID.BasicData.features & 0x01) ? 1 : 0])) ;
	}

	/*************************************************************************
	 Parse the 4 Detailed Timing Blocks contained in this EDID Block.
	 Should contain:
	 	 1 Monitor Name,
	 	 1 Monitor Range,
	 	 and 1 to 2 Detailed Timing Descriptors
	 EDID Version 1.3 and up
	**************************************************************************/

	BKNI_Memset((void *) &hHDMI->AttachedEDID.DescriptorHeader, 0x0, BHDM_EDID_DESC_HEADER_LEN);

	/* Check the four Descriptor Blocks in the initial 128 EDID  bytes */
	/* search for non-timing desriptors first.  Especially the Monitor Range */
	for (i = 0 ; i < 4 ; i++)
	{
		offset = BHDM_EDID_MONITOR_DESC_1 + BHDM_EDID_MONITOR_DESC_SIZE * i ;

		/* Non-timing descriptors start with 0x00, 0x00, 0x00, <tag>, 0x00 */
		if (BKNI_Memcmp(&hHDMI->AttachedEDID.Block[offset],
			(void *) &hHDMI->AttachedEDID.DescriptorHeader, 3) == 0)
		{
			Tag = hHDMI->AttachedEDID.Block[offset+3] ;

			/* Check which descriptor tag we found */
			switch (Tag)
			{
			case BHDM_EDID_TAG_MONITOR_RANGE : 	/* Range Limits found */
				BDBG_MSG(("Monitor Range found in Descriptor %d", i + 1)) ;
				BHDM_EDID_P_ParseMonitorRange(hHDMI, offset) ;
				break ;

			case BHDM_EDID_TAG_MONITOR_NAME : /* Monitor Name found */
				BKNI_Memcpy(
					hHDMI->AttachedEDID.MonitorName, &hHDMI->AttachedEDID.Block[offset + BHDM_EDID_DESC_DATA],
					BHDM_EDID_MONITOR_DESC_SIZE - BHDM_EDID_DESC_HEADER_LEN) ;

#if BDBG_DEBUG_BUILD
				/* replace linefeed with NULL; use offset variable as index */
				for (offset = 0 ; offset < BHDM_EDID_DESC_ASCII_STRING_LEN; offset++)
				{
					if (hHDMI->AttachedEDID.MonitorName[offset] == '\n')
					{
						hHDMI->AttachedEDID.MonitorName[offset] = '\0' ;
						break ;
					}
				}
#endif

				BDBG_MSG(("Monitor Name  found in Descriptor %d", i + 1)) ;
				BDBG_MSG(("   %s ", hHDMI->AttachedEDID.MonitorName)) ;
				break ;

			case BHDM_EDID_TAG_MONITOR_SN :
				BDBG_MSG(("Monitor S/N   found in Descriptor %d", i + 1)) ;
				break ;

			case BHDM_EDID_TAG_MONITOR_ASCII :
				/* no support using this in EDID structure */
				break ;

			case BHDM_EDID_TAG_DUMMY_DESC:
				/* descriptor space is unsed. */
				break;

			default :
				/* unknown; continue processing anyway */
				BDBG_WRN(("Unknown Tag <%X> found in Descriptor %d; continue...",
					Tag, i+1)) ;
			}
		}
	}


	/* now process the timing descriptors */
	BDBG_MSG(("Detailed Timing (Preferred) Formats:")) ;
	BDBG_MSG(("Format             HBlnk HOfst HWidth  VBlnk VOfst HWidth  PxlClk  ScrSz")) ;

	for (i = 0 ; i < 4 ; i++)
	{
		offset = BHDM_EDID_MONITOR_DESC_1 + BHDM_EDID_MONITOR_DESC_SIZE * i ;

		if (BKNI_Memcmp(&hHDMI->AttachedEDID.Block[offset],
			(void *) &hHDMI->AttachedEDID.DescriptorHeader, 3) != 0)
		{

			/* at least one Detail Timing Format has been found; */
			/* indicate formats have been checked */
			hHDMI->AttachedEDID.BcmVideoFormatsChecked = 1 ;

			BHDM_EDID_ParseDetailedTimingDescriptor(
				&hHDMI->AttachedEDID.Block[offset], &DetailTiming) ;

			BDBG_MSG(("%4d x %d (%4d%c)  %3d   %3d   %3d      %2d   %2d    %2d      %dMHz   %dx%d",
				DetailTiming.HorizActivePixels, DetailTiming.VerticalActiveLines,
				DetailTiming.Mode ?
					DetailTiming.VerticalActiveLines * 2 : DetailTiming.VerticalActiveLines, Mode[DetailTiming.Mode],
				DetailTiming.HorizBlankingPixels, DetailTiming.HSyncOffset, DetailTiming.HSyncWidth,
				DetailTiming.VerticalBlankingLines, DetailTiming.VSyncOffset, DetailTiming.VSyncWidth,
				DetailTiming.PixelClock, DetailTiming.HSize_mm, DetailTiming.VSize_mm)) ;

			/* convert the DetailTiming to BFMT_eVideoFmt type; */
			if (BHDM_EDID_P_DetailTiming2VideoFmt(hHDMI, &DetailTiming, &eVideoFmt)
			!= BERR_SUCCESS)
			{
				BDBG_WRN(("Unknown/Unsupported Detailed Timing Format %4d x %d (%4d%c)\n",
					DetailTiming.HorizActivePixels, DetailTiming.VerticalActiveLines,
					DetailTiming.Mode ?
						DetailTiming.VerticalActiveLines * 2 : DetailTiming.VerticalActiveLines,
					Mode[DetailTiming.Mode])) ;
				continue /* if not a valid format */ ;
			}
			BDBG_MSG(("\n")) ;

			/* keep a copy of first two supported detailed timings for quick retrieval */
			if (SupportedTimingsFound < 2)
			{
				BKNI_Memcpy(
					&(hHDMI->AttachedEDID.SupportedDetailTimings[SupportedTimingsFound]),
					&DetailTiming, sizeof(BHDM_EDID_DetailTiming)) ;
			}

			/* set BFMTs that match this Detailed Timing Format as being supported */
			BHDM_EDID_P_SetSupportedMatchingFmts(hHDMI, eVideoFmt) ;

			SupportedTimingsFound++ ;

		}
	}

	BHDM_EDID_P_ParseEstablishedTimingFormats(hHDMI) ;

	hHDMI->AttachedEDID.SupportedDetailTimingsIn1stBlock = SupportedTimingsFound ;

	/******************************************************************
	 Parse Extension Blocks - primarily interested in Timing Extensions
	******************************************************************/

	if (!hHDMI->AttachedEDID.BasicData.Extensions)
	{
		hHDMI->AttachedEDID.RxHasHdmiSupport = 0 ;
		/* Its not mandatory to have extension blocks. so, i dont think this is an Error. It should just spit out a message and continue. */
		BDBG_ERR(("No EDID Extensions Found... No HDMI Support")) ;
		goto done ;
	}

	/* Read the 1st EDID Block and parse to see if its a block map or an extension block(extensions count should be 1). */
	BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI, 1, hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE)) ;

	if(hHDMI->AttachedEDID.BasicData.Extensions == 1) {
		rc = BHDM_EDID_P_ProcessExtensionBlock(hHDMI);
	}
	else if(hHDMI->AttachedEDID.Block[0] == BHDM_EDID_EXT_BLOCK_MAP){
		rc = BHDM_EDID_P_ProcessBlockMap(hHDMI);

		/* If the number of extension blocks are more than 127, the standard needs  one more block map to accomodate their extension tags. */
		if(hHDMI->AttachedEDID.BasicData.Extensions >= BHDM_EDID_BLOCKSIZE){
			/* Read the 128th EDID Block which should be a block map. */
			BHDM_CHECK_RC(rc, BHDM_EDID_GetNthBlock(hHDMI, 128, hHDMI->AttachedEDID.Block, BHDM_EDID_BLOCKSIZE));

			if(hHDMI->AttachedEDID.Block[0] == BHDM_EDID_EXT_BLOCK_MAP){
				rc = BHDM_EDID_P_ProcessBlockMap(hHDMI);
			}
			else {
				BDBG_ERR(("Wrong EDID extension tag of %d found in Block 128(Block Map); should be 0xF0",hHDMI->AttachedEDID.Block[0])) ;
			}
		}
	}
	else {
		BDBG_ERR(("Wrong coding of the EDID Extension count in Block 0...")) ;
		goto done ;
	}

#if BDBG_DEBUG_BUILD
		/* print raw EDID info */
		BHDM_EDID_DumpRawEDID(hHDMI);
#endif

done:
	hHDMI->edidStatus = BHDM_EDID_STATE_eOK;

	return rc ;
} /* BHDM_EDID_Initialize */


/******************************************************************************
BERR_Code BHDM_EDID_GetMonitorName
Summary: Retrieve a copy of the Monitor Name stored in the EDID
*******************************************************************************/
BERR_Code BHDM_EDID_GetMonitorName(
   BHDM_Handle hHDMI, /* [in] HDMI handle */
   uint8_t *pDescriptorText /* [out] pointer to memory to hold retrieved name */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t RxDeviceAttached ;

	BDBG_ENTER(BHDM_EDID_GetMonitorName) ;

	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		return BHDM_NO_RX_DEVICE ;
	}

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		return BHDM_EDID_NOT_FOUND;
	}

#if BDBG_DEBUG_BUILD
	if (hHDMI->DeviceSettings.BypassEDIDChecking)
	{
		BKNI_Memcpy(pDescriptorText, EDIDByPassedText,
			BHDM_EDID_MONITOR_DESC_SIZE - BHDM_EDID_DESC_HEADER_LEN) ;
		goto done ;
	}
#endif

	BKNI_Memcpy(pDescriptorText, hHDMI->AttachedEDID.MonitorName, BHDM_EDID_DESC_ASCII_STRING_LEN) ;

done:
#if BDBG_DEBUG_BUILD
	BDBG_MSG(("Monitor Name: <%.*s>",
		BHDM_EDID_DESC_ASCII_STRING_LEN, pDescriptorText)) ;
#endif

	BDBG_LEAVE(BHDM_EDID_GetMonitorName) ;
	return rc ;
} /* end BHDM_EDID_GetMonitorName */


BERR_Code BHDM_EDID_GetSupportedColorimetry(
	BHDM_Handle hHDMI, BHDM_OutputFormat eOutputFormat,
	BFMT_VideoFmt eVideoFmt, BAVC_MatrixCoefficients *eColorimetry)
{
	BERR_Code 	rc = BERR_SUCCESS ;

	*eColorimetry = BAVC_MatrixCoefficients_eDvi_Full_Range_RGB;
	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		/* Default to Full range RGB */
		rc = BHDM_EDID_NOT_FOUND;
		goto done ;
	}

	if (!hHDMI->AttachedEDID.RxHasHdmiSupport)
	{
		goto done ;
	}

	/* default to Limited Range RGB */
	*eColorimetry = BAVC_MatrixCoefficients_eHdmi_RGB ;

	/* use YPrPb for HDMI monitors */
	if ((eOutputFormat == BHDM_OutputFormat_eHDMIMode)
	&& (hHDMI->AttachedEDID.RxVSDB.YCbCr444))  /* if supported by attached monitor */
	{
		switch (eVideoFmt)
		{
		case BFMT_VideoFmt_eDVI_640x480p  :  /* except for VGA mode */
			BDBG_MSG(("Use Full Range RGB")) ;
			*eColorimetry = BAVC_MatrixCoefficients_eDvi_Full_Range_RGB ;
			break ;

		case BFMT_VideoFmt_eNTSC :
		case BFMT_VideoFmt_eNTSC_J :

#if BHDM_CONFIG_ANALOG_TRANSLATION_SUPPORT
		case BFMT_VideoFmt_e720x482_NTSC :	   /* 720x482i NSTC-M for North America */
		case BFMT_VideoFmt_e720x482_NTSC_J:    /* 720x482i Japan */
		case BFMT_VideoFmt_e720x483p :
#endif

			
		case BFMT_VideoFmt_e480p :

		case BFMT_VideoFmt_ePAL_B :
		case BFMT_VideoFmt_ePAL_B1 :
		case BFMT_VideoFmt_ePAL_D :
		case BFMT_VideoFmt_ePAL_D1 :
		case BFMT_VideoFmt_ePAL_G :
		case BFMT_VideoFmt_ePAL_H :
		case BFMT_VideoFmt_ePAL_K :
		case BFMT_VideoFmt_ePAL_I :
		case BFMT_VideoFmt_ePAL_M :
		case BFMT_VideoFmt_ePAL_N :
		case BFMT_VideoFmt_ePAL_NC :
		case BFMT_VideoFmt_ePAL_60 :
		case BFMT_VideoFmt_eSECAM_L :
		case BFMT_VideoFmt_eSECAM_B :
		case BFMT_VideoFmt_eSECAM_G :
		case BFMT_VideoFmt_eSECAM_D :
		case BFMT_VideoFmt_eSECAM_K :
		case BFMT_VideoFmt_eSECAM_H :
		case BFMT_VideoFmt_e576p_50Hz :
			BDBG_MSG(("Use YCrCb SD")) ;
			*eColorimetry = BAVC_MatrixCoefficients_eSmpte_170M ;   /* SD Colorspace */
			break ;

		default : /* all others */
			BDBG_MSG(("Use YCrCb HD")) ;
			*eColorimetry = BAVC_MatrixCoefficients_eItu_R_BT_709 ; /* HD Colorspace */
		}
	}
#if BDBG_DEBUG_BUILD
	else
		BDBG_MSG(("DVI Monitors/Mode support RGB Only")) ;
#endif


	/* use full range rgb for DVI and HDMI 640x480 */
	if (*eColorimetry == BAVC_MatrixCoefficients_eHdmi_RGB)
	{
		switch (eVideoFmt)
		{
		default :
			BDBG_MSG(("Use Limited Range RGB")) ;
			/* keep limited range RGB */
			break ;

		case BFMT_VideoFmt_eDVI_640x480p  :
		case BFMT_VideoFmt_eDVI_800x600p  :
		case BFMT_VideoFmt_eDVI_1024x768p :
		case BFMT_VideoFmt_eDVI_1280x768p :
		case BFMT_VideoFmt_eDVI_1280x720p_50Hz :
		case BFMT_VideoFmt_eDVI_1280x720p :
		case BFMT_VideoFmt_eDVI_1280x720p_ReducedBlank :
			BDBG_MSG(("Use Full Range RGB")) ;
			*eColorimetry = BAVC_MatrixCoefficients_eDvi_Full_Range_RGB ;
			break ;
		}
	}

done:
	return rc  ;
}


/******************************************************************************
BERR_Code BHDM_EDID_GetExtendedColorimetry
Summary: Retrieve xvYCC colorimetry info stored in the EDID
*******************************************************************************/
BERR_Code BHDM_EDID_GetExtendedColorimetry(
	BHDM_Handle hHDMI,
	BHDM_EDID_ExtendedColorimetry *pExtendedColorimetry)
{
	BERR_Code rc = BERR_SUCCESS ;

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid)
	{
		return BHDM_EDID_NOT_FOUND;
	}

	pExtendedColorimetry->bxvYCC601 = hHDMI->AttachedEDID.ColorimetryData.bxvYCC601 ? true : false ;
	pExtendedColorimetry->bxvYCC709 = hHDMI->AttachedEDID.ColorimetryData.bxvYCC709 ? true : false ;

	return rc  ;
}

/******************************************************************************
BERR_Code BHDM_EDID_GetSupportedColorDepth
Summary: Retrieve a copy of the Monitor Name stored in the EDID
*******************************************************************************/
BERR_Code BHDM_EDID_GetSupportedColorDepth(
	BHDM_Handle hHDMI,
	BHDM_EDID_ColorDepth *stSuppotedColorDepth,	/* [out] */
	bool *bYCbCrPixelEncoding 	/* [out] */
)
{
	BERR_Code rc = BERR_SUCCESS;

	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid)
	{
		return BHDM_EDID_NOT_FOUND;
	}

	stSuppotedColorDepth->bColorDepth24bit = true; /* standard color depth, always supported */
	stSuppotedColorDepth->bColorDepth30bit =
		hHDMI->AttachedEDID.RxVSDB.DeepColor_30bit ? true : false;
	stSuppotedColorDepth->bColorDepth36bit =
		hHDMI->AttachedEDID.RxVSDB.DeepColor_36bit ? true : false;
	stSuppotedColorDepth->bColorDepth48bit =
		hHDMI->AttachedEDID.RxVSDB.DeepColor_48bit ? true : false;

	*bYCbCrPixelEncoding = hHDMI->AttachedEDID.RxVSDB.DeepColor_Y444 ? true : false;

	return rc;
}



/******************************************************************************
BERR_Code BHDM_EDID_GetMyCecPhysicalAddr
Summary: Retrieve CEC Physical info stored in the EDID
*******************************************************************************/
BERR_Code BHDM_EDID_GetMyCecPhysicalAddr(
	BHDM_Handle hHDMI,
	uint8_t *pMyPhysicalAddr)
{
	BERR_Code rc = BERR_SUCCESS ;

	if (hHDMI->edidStatus != BHDM_EDID_STATE_eOK)
	{
		return BHDM_EDID_NOT_FOUND;
	}

	pMyPhysicalAddr[0] = hHDMI->AttachedEDID.RxVSDB.PhysAddr_A;
	pMyPhysicalAddr[0] <<= 4;
	pMyPhysicalAddr[0] |= hHDMI->AttachedEDID.RxVSDB.PhysAddr_B;

	pMyPhysicalAddr[1] = hHDMI->AttachedEDID.RxVSDB.PhysAddr_C;
	pMyPhysicalAddr[1] <<= 4;
	pMyPhysicalAddr[1] |= hHDMI->AttachedEDID.RxVSDB.PhysAddr_D;

	return rc  ;
}


