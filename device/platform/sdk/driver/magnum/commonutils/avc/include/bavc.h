/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavc.h $
 * $brcm_Revision: Hydra_Software_Devel/229 $
 * $brcm_Date: 10/8/12 2:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/avc/include/bavc.h $
 * 
 * Hydra_Software_Devel/229   10/8/12 2:27p davidp
 * SW7445-16: Add H.265 video compression standard.
 * 
 * Hydra_Software_Devel/228   9/13/12 3:58p syang
 * SW7425-3896: remove right alpha surface, and add ulOrigPTS to
 * BAVC_Gfx_Picture
 * 
 * Hydra_Software_Devel/227   8/22/12 2:01p vanessah
 * SW7425-2172: add max sourceId
 * 
 * Hydra_Software_Devel/226   8/20/12 5:01p hongtaoz
 * SW7425-2172: added max mfd/vfd/gfd source IDs;
 * 
 * Hydra_Software_Devel/225   8/15/12 6:21p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/224   8/7/12 12:55p yuxiaz
 * SW7405-3984: Updated comments for bMute.
 *
 * Hydra_Software_Devel/223   6/29/12 1:28p vanessah
 * SW7425-3356: add FNRT flages to BAVC_MFD_Pictures
 *
 * Hydra_Software_Devel/222   5/22/12 1:33p pntruong
 * SW7425-3103: Added 15 and 20 fps frame rate support.
 *
 * Hydra_Software_Devel/221   5/7/12 11:44a pntruong
 * SW7429-143: Added new framerate for encoder.
 *
 * Hydra_Software_Devel/221   5/7/12 10:54a pntruong
 * SW7429-143: Added new framerate for encoder.
 *
 * Hydra_Software_Devel/220   4/3/12 5:40p jgarrett
 * SW7425-2378: Removing BAVC_AudioCompressionStd.  Now defined in
 * bavc_rap.h
 *
 * Hydra_Software_Devel/219   3/23/12 11:28a jgarrett
 * SW7425-2378: Adding macro to facilitate moving BAVC_AudioCompressionStd
 * to bavc_rap.h
 *
 * Hydra_Software_Devel/218   2/14/12 12:46p vsilyaev
 * SW7425-2258: Added Motion Jpeg codec type
 *
 * Hydra_Software_Devel/217   2/6/12 6:27p pntruong
 * SW7425-2181: VDC: Add support to interface with output from SID for
 * MJPEG usage mode.
 *
 * Hydra_Software_Devel/216   2/3/12 2:24p pntruong
 * SW7425-2327: Added bar data support.
 *
 * Hydra_Software_Devel/215   2/2/12 4:47p jgarrett
 * SW7425-2136: Adding FLAC codec type
 *
 * Hydra_Software_Devel/214   1/27/12 4:33p pntruong
 * SW7425-2239, SW7425-2253: Added bChannelChange to support Transcode
 * Fast Channel Change.
 *
 * Hydra_Software_Devel/213   1/25/12 4:52p jgarrett
 * SW7425-2268: Adding G.723.1 audio codec
 *
 * Hydra_Software_Devel/212   1/3/12 7:18p hongtaoz
 * SW7425-1001, SW7425-2083: added bLast flag to cleanly flush/terminate
 * NRT mode transcoder;
 *
 * Hydra_Software_Devel/211   12/15/11 5:26p jgarrett
 * SW7231-500: Adding Lpcm1394 codec
 *
 * Hydra_Software_Devel/210   11/23/11 4:25p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/209   11/17/11 2:04p pntruong
 * SW7425-1780, SW7425-1781: Initial support for userdata transcode.
 *
 * Hydra_Software_Devel/208   11/15/11 11:03a jgarrett
 * SW7425-1406: Adding additional audio codecs
 *
 * Hydra_Software_Devel/VORBIS_DEVEL/2   11/15/11 10:13a jgarrett
 * SW7425-1406: Adding additional audio codecs
 *
 * Hydra_Software_Devel/VORBIS_DEVEL/1   10/4/11 3:53p jgarrett
 * SW7425-1406: Adding Vorbis support
 *
 * Hydra_Software_Devel/207   7/20/11 12:33p jessem
 * SW7425-938: Added VP7, VP8, RV9, and SPARK codecs.
 *
 * Hydra_Software_Devel/206   6/25/11 7:10p vanessah
 * SW7425-686: change interruptrefreshrate defintion
 *
 * Hydra_Software_Devel/205   6/22/11 11:21a pntruong
 * SW7425-686: Added new veritical refresh rate enum for synchronizing
 * XVD/VDC state.  Also will be using this enum for replacing limited
 * fmt's veritical refresh rate mask.
 *
 * Hydra_Software_Devel/204   6/15/11 5:33p pntruong
 * SW7425-104: Added the flag that indicates if top/bot/frame repeat.
 * This flag can be use by BVN cores (e.g. mctf) to improve picture
 * quality.
 *
 * Hydra_Software_Devel/203   6/9/11 1:34p nilesh
 * SW7425-44: Moved bIgnorePicture and bStallStc fields to before the
 * bMute field in BAVC_MFD_Picture
 *
 * Hydra_Software_Devel/202   6/7/11 4:58p nilesh
 * SW7425-44: Added BAVC_MFD_Picture.bStallStc
 *
 * Hydra_Software_Devel/201   5/20/11 12:02p pntruong
 * SW7425-616: Added VP6 as an enum in BAVC_VideoCompressionStd.
 *
 * Hydra_Software_Devel/200   2/15/11 3:59p jgarrett
 * SW7422-146: Adding additional codecs
 *
 * Hydra_Software_Devel/199   2/8/11 12:34p nilesh
 * SW7425-1: Added BAVC_VideoCompressionProfile and
 * BAVC_VideoCompressionLevel enums
 *
 * Hydra_Software_Devel/198   2/7/11 11:52a vanessah
 * SW7425-32: undo the changes made in version 197
 *
 * Hydra_Software_Devel/197   2/7/11 10:48a vanessah
 * SW7425-32: fix 7420 compile error
 *
 * Hydra_Software_Devel/196   2/3/11 4:39p nilesh
 * SW7425-38,SW7425-1,SW7425-56: Removed bavc_enc.h
 *
 * Hydra_Software_Devel/195   2/1/11 4:56p nilesh
 * SW7425-38,SW7425-1,SW7425-56: Added protocol to encoder buffer status
 *
 * Hydra_Software_Devel/194   2/1/11 4:14p nilesh
 * SW7425-1,SW7425-38: Combined common portions of audio/video compressed
 * buffer descriptors
 *
 * Hydra_Software_Devel/193   11/18/10 4:08p nilesh
 * SW7425-38: Fixed compilation issue related to changed to bavc_rap.h and
 * bavc_vce.h
 *
 * Hydra_Software_Devel/192   11/3/10 2:31p hongtaoz
 * SW7425-43: changed picture coding type name space to be generic;
 * updated comments about picture coding type;
 *
 * Hydra_Software_Devel/191   10/25/10 7:23p vanessah
 * SW7422-43:  Handle GFD programming for 3D.
 *
 * Hydra_Software_Devel/190   10/22/10 6:59p vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/189   10/15/10 3:52p yuxiaz
 * SW7422-39: Added BAVC_SourceId_eMpeg2.
 *
 * Hydra_Software_Devel/188   10/11/10 3:17p hongtaoz
 * SW7425-38: added encoders common files;
 *
 * Hydra_Software_Devel/186   10/11/10 11:08a jessem
 * SW7420-173: Added Video Feeders 0, 1, 2, 3, and 4 to BAVC_SourceId.
 *
 * Hydra_Software_Devel/184   9/1/10 5:01p yuxiaz
 * SW7422-29: Update XVD to AVC structure with 3D requirements
 *
 * Hydra_Software_Devel/183   8/5/10 5:12p darnstein
 * SW7422-46: define second bypass path for VBI software. The 7422 chipset
 * is the first to require this.
 *
 * Hydra_Software_Devel/182   3/18/10 11:31a yuxiaz
 * SW7405-3954: Remove dependence of brdc.h
 *
 * Hydra_Software_Devel/181   3/17/10 3:32p yuxiaz
 * SW7405-3954: Use RDC reserved registers to coordinate programming of
 * VBI encoder control registers between BVBI and BVDC.
 *
 * Hydra_Software_Devel/180   1/6/10 10:27a btosi
 * SW7405-3137: added BAVC_VideoCompressionStd_eSVC to be compatible with
 * AVC
 *
 * Hydra_Software_Devel/179   12/21/09 11:11a jasonx
 * SW35230-32: HD_DVI changes for input control
 *
 * Hydra_Software_Devel/178   12/11/09 3:11p pntruong
 * SW7408-17: Adding initial APE support.
 *
 * Hydra_Software_Devel/NEXUS_SOFT_AUDIO/1   12/8/09 11:09a jgarrett
 * SW7408-17: Adding initial APE support
 *
 * Hydra_Software_Devel/177   10/16/09 5:21p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/176   10/15/09 4:53p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/175   10/6/09 2:41p davidp
 * SW7405-3137: Add MVC video compression standard
 *
 * Hydra_Software_Devel/174   8/3/09 1:58p pntruong
 * PR57256: In bavc.h, 7.943 frame rate should be 7.493.
 *
 * Hydra_Software_Devel/173   7/8/09 4:02p pntruong
 * PR42134: Added parameters for AFD value from the stream for Active
 * Format Descriptors.
 *
 * Hydra_Software_Devel/172   6/24/09 3:29p yuxiaz
 * PR50994: BAVC To Extend enums to Support Newer Chroma Sample Location
 * Types
 *
 * Hydra_Software_Devel/170   3/25/09 5:54p pntruong
 * PR52524: Used the index determined by upstream block.
 *
 * Hydra_Software_Devel/169   3/9/09 1:27p pntruong
 * PR52524: Added support for hdmi rgb quantization of full range and
 * limited range.
 *
 * Hydra_Software_Devel/168   1/7/09 10:46a yuxiaz
 * PR44579: Add 7.943 and 14.985 frame rates to bavc.h
 *
 * Hydra_Software_Devel/167   12/10/08 10:18a yuxiaz
 * PR49634: XVD to Tell VDC If Cadence Match Should be Ignored.
 *
 * Hydra_Software_Devel/166   12/1/08 3:03p yuxiaz
 * PR49664: PR49664 : Add AAC HE to the list of compression standards
 *
 * Hydra_Software_Devel/PR49664/1   11/25/08 5:21p shyam
 * PR49664 : Add AAC HE to the list of compression standards
 *
 * Hydra_Software_Devel/164   2/26/08 2:17p yuxiaz
 * PR38429: Put back BAVC_AudioFormat_eAVS,
 *
 * Hydra_Software_Devel/163   2/8/08 5:25p rgreen
 * PR39232:Merge Changes
 *
 * Hydra_Software_Devel/bdvd_v10/7   2/1/08 11:13a mpeteri
 * PR_7394 [ Export setting required to choose between reencode or pcm ].
 * Fixes for PR7394 and PR6652 - this checkin is built upon 1/31 RAP
 * label and requires that the RAP label be below v10 latest rule in the
 * config spec.  An updated config spec will be sent out to demonstrate
 * this requirement.  NOTE this is temporary and will go back to normal
 * when RAP label picks up the branched changes.
 * RAP label to use - RAP_7440_FW_Phase2D_20080131
 *
 * Hydra_Software_Devel/bdvd_v10/fix_bdvd_pr7394/1   1/30/08 3:40p mpeteri
 * update from 6652 branch
 *
 * Hydra_Software_Devel/bdvd_v10/fix_bdvd_pr6652/2   1/24/08 1:51p mpeteri
 * update from Hydra for VDC
 *
 * Hydra_Software_Devel/bdvd_v10/6   1/17/08 2:16p nprao
 * PR_7285 [ Add xvYcc support ].  Updated VDC for xvYcc support and added
 * API for the application to enable xvYcc.
 *
 * Hydra_Software_Devel/177   10/16/09 5:21p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/176   10/15/09 4:53p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/175   10/6/09 2:41p davidp
 * SW7405-3137: Add MVC video compression standard
 *
 * Hydra_Software_Devel/174   8/3/09 1:58p pntruong
 * PR57256: In bavc.h, 7.943 frame rate should be 7.493.
 *
 * Hydra_Software_Devel/173   7/8/09 4:02p pntruong
 * PR42134: Added parameters for AFD value from the stream for Active
 * Format Descriptors.
 *
 * Hydra_Software_Devel/172   6/24/09 3:29p yuxiaz
 * PR50994: BAVC To Extend enums to Support Newer Chroma Sample Location
 * Types
 *
 * Hydra_Software_Devel/170   3/25/09 5:54p pntruong
 * PR52524: Used the index determined by upstream block.
 *
 * Hydra_Software_Devel/169   3/9/09 1:27p pntruong
 * PR52524: Added support for hdmi rgb quantization of full range and
 * limited range.
 *
 * Hydra_Software_Devel/168   1/7/09 10:46a yuxiaz
 * PR44579: Add 7.943 and 14.985 frame rates to bavc.h
 *
 * Hydra_Software_Devel/167   12/10/08 10:18a yuxiaz
 * PR49634: XVD to Tell VDC If Cadence Match Should be Ignored.
 *
 * Hydra_Software_Devel/166   12/1/08 3:03p yuxiaz
 * PR49664: PR49664 : Add AAC HE to the list of compression standards
 *
 * Hydra_Software_Devel/PR49664/1   11/25/08 5:21p shyam
 * PR49664 : Add AAC HE to the list of compression standards
 *
 * Hydra_Software_Devel/164   2/26/08 2:17p yuxiaz
 * PR38429: Put back BAVC_AudioFormat_eAVS,
 *
 * Hydra_Software_Devel/163   2/8/08 5:25p rgreen
 * PR39232:Merge Changes
 *
 * Hydra_Software_Devel/162   1/25/08 3:22p katrep
 * PR38429: Added AVS audio format
 *
 * Hydra_Software_Devel/161   1/4/08 2:28p albertl
 * PR38150:  Added BAVC_TransferCharacteristics_eIec_61966_2_4;
 *
 * Hydra_Software_Devel/160   10/19/07 10:49a pntruong
 * PR35185: PCRlib needs BAVC_PTSInfo to carry additional information.
 *
 * Hydra_Software_Devel/159   7/23/07 7:31p pntruong
 * PR32948: Video Compression Standard in BXVD_StartDecode should be
 * MPEG2DTV only DSS_ES.
 *
 * Hydra_Software_Devel/158   7/16/07 5:27p albertl
 * PR31658:  Added FGT fields.
 *
 * Hydra_Software_Devel/157   6/26/07 2:35p pntruong
 * PR32241: add a new variable eColorSpace for HDMI callback structure
 * that uses the pixel encoding from hdmi.
 *
 * Hydra_Software_Devel/PR32241/1   6/20/07 6:30p honglang
 * PR32241: add a new variable eColorSpace for HDMI callback structure
 *
 * Hydra_Software_Devel/156   6/12/07 1:02p pntruong
 * PR32070: Add BAVC_AudioSamplingRate enums for 8kHz, 12kHz and
 * 11.025kHz.
 *
 * Hydra_Software_Devel/155   6/8/07 12:32p yuxiaz
 * PR30016: Added BAVC_StripeWidth_e256Byte in BAVC_StripeWidth.
 *
 * Hydra_Software_Devel/bdvd_v10/fix_bdvd_pr6652/1   1/22/08 2:15p carson
 * internal HDMI
 *
 * Hydra_Software_Devel/bdvd_v10/5   8/17/07 8:39p vijayk
 * PR_5077 [ Support for "bPictureRepeatFlag" ].  This is to inform any
 * consumers of DM isr that the DM is presenting the first field of a
 * picture
 *
 * Hydra_Software_Devel/bdvd_v10/5   8/17/07 8:37p vijayk
 * PR5077: Addition of picture repeat flag
 *
 * Hydra_Software_Devel/bdvd_v10/5   8/18/07 12:30a parijat
 * bFrameBufferRepeatFlag added
 *
 * Hydra_Software_Devel/bdvd_v10/4   6/6/07 3:58p therman
 * PR_4238 [ &nbsp ].  PR4238 - Fix BAVC sample rate enum for 11.025 kHz
 *
 * Hydra_Software_Devel/bdvd_v10/fix_bdvd_pr4238/1   6/6/07 3:43p therman
 * PR4238 - Fix BAVC sample rate enum for 11.025 kHz
 *
 * Hydra_Software_Devel/bdvd_v10/3   6/1/07 1:33p nprao
 * Increased BAVC_MOSAIC_MAX to 32 for 32 clear rectangles. This change
 * will also go in the mainline later.
 *
 * Hydra_Software_Devel/bdvd_v10/2   5/29/07 4:30p vijayk
 * PR31658: 7440B: Updates to bavc.h [Audio samplingrate updates and merge
 * from Hydra latest]
 *
 * Hydra_Software_Devel/154   5/24/07 7:57p hongtaoz
 * PR26255: updated comments for BAVC_MFD_Picture;
 *
 * Hydra_Software_Devel/153   4/24/07 1:29p yuxiaz
 * PR30016: Added stripe width in BAVC_MFD_Picture.
 *
 * Hydra_Software_Devel/bdvd_v10/1   4/24/07 1:13p nilesh
 * PR22766: Added FGT heap handle and data pointer to BAVC_MFD_Picture
 * struct
 *
 * Hydra_Software_Devel/152   2/13/07 6:32p shyam
 * PR 27542 : Added Audio compression standards for transcoder use into
 * AVC
 *
 * Hydra_Software_Devel/151   2/8/07 4:57p katrep
 * PR25430: Moved the xpt related defines from bavc.h to bavc_xpt.h
 *
 * Hydra_Software_Devel/150   2/5/07 7:24p katrep
 * PR27518: Added support mpeg audio 2.5
 *
 * Hydra_Software_Devel/149   12/18/06 11:15p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   12/14/06 11:23a pntruong
 * PR26322: Fixed build errors for single path.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   12/14/06 11:08a pntruong
 * PR26322:  Merged from mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   12/11/06 5:19p albertl
 * PR24587:  Added 3rd GFX Feeder source.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   12/6/06 4:17p pntruong
 * PR26255:  Added more descriptions and usages of bFrameProgressive,
 * bStreamProgressive, and eSourcePolarity.
 *
 * Hydra_Software_Devel/148   12/12/06 4:24p gmullen
 * PR24806: Explicit support for MP3.
 *
 * Hydra_Software_Devel/147   12/11/06 10:03p pntruong
 * PR26322: For modular VEC, BVDC needs to disclose choice of VEC to BVBI.
 *
 * Hydra_Software_Devel/146   12/8/06 3:13p pntruong
 * PR26322: For modular VEC, BVDC needs to disclose choice of VEC to BVBI.
 * Updated additional comments.
 *
 * Hydra_Software_Devel/145   12/6/06 4:07p pntruong
 * PR26255:  Added more descriptions and usages of bFrameProgressive,
 * bStreamProgressive, and eSourcePolarity.
 *
 * Hydra_Software_Devel/144   11/30/06 8:37p gaurava
 * PR26255: Added description of a few fields in the BAVC_MFD_Picture data
 * structure.
 *
 * Hydra_Software_Devel/143   11/29/06 4:44p pntruong
 * PR26255: Documentation for BAVC_MFD_Picture has gone out of date.
 *
 * Hydra_Software_Devel/142   11/28/06 5:05p pntruong
 * PR23189: Removed chip specific ifdefs.
 *
 * Hydra_Software_Devel/141   11/2/06 11:44a syang
 * PR 23331:  update desc for sending gfx surface to MFD
 *
 * Hydra_Software_Devel/140   11/2/06 8:39a gmullen
 * PR23189: Added PicIncDecCtrl to BAVC_XptContextMap.
 *
 * Hydra_Software_Devel/139   10/25/06 6:27p katrep
 * PR25179: Added support for SVP OTF.
 *
 * Hydra_Software_Devel/138   10/12/06 5:15p katrep
 * PR24899: Added itb es enum for MPEG1 video required for VCD playback.
 *
 * Hydra_Software_Devel/137   10/5/06 4:49p gmullen
 * PR24743: Added compression standard enum for MPEG4 part 2
 *
 * Hydra_Software_Devel/136   10/5/06 11:55a gmullen
 * PR 24743: Added ITB ES enum for MPEG4.
 *
 * Hydra_Software_Devel/135   9/26/06 11:20a hongtaoz
 * PR24539: added range remapping setting for VC1;
 *
 * Hydra_Software_Devel/134   9/12/06 2:00p gmullen
 * PR15309: Added VC1 simple/main and H263 support for RAVE.
 *
 * Hydra_Software_Devel/133   9/12/06 1:05p syang
 * PR 23331: add desc for sending gfx surface to MFD.
 *
 * Hydra_Software_Devel/132   8/4/06 5:21p gmullen
 * PR23158: Added WMA support.
 *
 * Hydra_Software_Devel/131   8/2/06 2:17p hongtaoz
 * PR18233: added channel index to the xvd_to_vdc structure and more
 * comments for mosaic mode;
 *
 * Hydra_Software_Devel/130   7/24/06 3:25p gmullen
 * PR20082: Added picture counter reg to BAVC_XptContextMap
 *
 * Hydra_Software_Devel/129   7/24/06 10:36a gmullen
 * PR20082: Added support for picture counters.
 *
 * Hydra_Software_Devel/128   7/18/06 10:31a maivu
 * PR 21107: Add support for VC1 simple main profile
 *
 * Hydra_Software_Devel/127   7/17/06 5:56p pntruong
 * PR18233: Moved MOSAIC_MAX to avc since other PIs need to access the
 * definition.
 *
 * Hydra_Software_Devel/126   6/19/06 10:21a pntruong
 * PR22095: Clarify range of idr_pic_id.
 *
 * Hydra_Software_Devel/125   6/7/06 12:46p pntruong
 * PR21975: Need to remove CHP_REV from code in AVC.
 *
 * Hydra_Software_Devel/124   5/9/06 5:39p hongtaoz
 * PR20750: fixed comments for color space enums; 480p/576p formats are
 * treated in SD color space;
 *
 * Hydra_Software_Devel/123   4/20/06 5:51p pntruong
 * PR20750: Please update bavc.h to reflect presence of third VEC.
 *
 * Hydra_Software_Devel/122   4/18/06 9:46a pntruong
 * PR20042:  Update panscan comment regardings clipped source.
 *
 * Hydra_Software_Devel/121   4/17/06 2:55p pntruong
 * PR20042: Added new source clipping for AVC decoding.
 *
 * Hydra_Software_Devel/120   4/11/06 4:11p pntruong
 * PR20750: Please update bavc.h to reflect presence of third VEC.
 *
 * Hydra_Software_Devel/117   4/4/06 1:34p hongtaoz
 * PR20403: add triple display support for 7400;
 *
 * Hydra_Software_Devel/116   3/31/06 12:13a ssavekar
 * PR13740: add MAD support for 7038 C0
 * Added code for setting of flag "bPictureRepeatFlag". This flag will be
 * set to true if DM is repeating a picture (frame/field). This flag will
 * not be set to true if DM is alternating between two fields of the same
 * frame.
 *
 * Hydra_Software_Devel/116   3/30/06 11:46p ssavekar
 * PR13740: add MAD support for 7038 C0
 * Added code for setting of flag "bPictureRepeatFlag". This flag will be
 * set to true if DM is repeating a picture (frame/field). This flag will
 * not be set to true if DM is alternating between two fields of the same
 * frame.
 *
 * Hydra_Software_Devel/115   3/7/06 5:00p bandrews
 * PR19664: Removed conditional compile in sampling rate enum
 *
 * Hydra_Software_Devel/114   2/7/06 2:48p pntruong
 * PR19106: Initial xvd decode of pid 0x1022 in
 * estarFeed_20051223_1.5min.mpg causes segfault in bxvd_dispmgr.c.
 * Added support for eSAR for mvd/avc on chip decoder.
 *
 * Hydra_Software_Devel/113   1/25/06 3:07p maivu
 * PR 19138, PR 19157: Add sample aspect ratio support for AVC
 *
 * Hydra_Software_Devel/112   1/25/06 10:06a maivu
 * PR 18545: Added userdata type for VC1
 *
 * Hydra_Software_Devel/111   1/12/06 12:09p hongtaoz
 * PR18233: added pNext to BAVC_MVD_Picture structure to support mosaic
 * mode;
 *
 * Hydra_Software_Devel/110   1/3/06 4:28p darnstein
 * PR18545: Eliminate userdata_type enum for DSS. Simplify remaining
 * enums. This was the consensus reached with David Erickson today, after
 * consulting with Bill Fassl.
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/2   12/23/05 10:19a hongtaoz
 * PR18233: merge change from main branch;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/1   12/8/05 6:49p hongtaoz
 * PR18233: add Mosaic mode;
 *
 * Hydra_Software_Devel/105   11/28/05 3:44p syang
 * PR 12762: separate little endian and big endian when gfx sur goes to
 * mgd; mute for bad gfx sur; add 4 more pxl fmts to fully use HW
 *
 * Hydra_Software_Devel/104   11/22/05 12:01p vijeth
 * PR 17568: Add "key CRC frames" boolean to XVD and handle in VDC.
 * Removed the Redundent flag bkeycrc
 *
 * Hydra_Software_Devel/103   11/18/05 8:12p ssavekar
 * PR 17811: Need idr_pic_id and PicOrderCnt with CRC values
 * Adding code and data structure fields for passing CRC related
 * parameters to VDC.
 *
 * Hydra_Software_Devel/102   11/10/05 9:08a pblanco
 * PR17568: Adding "key CRC frames" Boolean to XVD and handle in VDC.
 *
 * Hydra_Software_Devel/101   10/31/05 5:18p hongtaoz
 * PR17879, PR17514: added ulAdjQp to the picture structure to support DNR
 * programming;
 *
 * Hydra_Software_Devel/100   10/25/05 2:40p vsilyaev
 * PR 17727: Relaxed condition test (test now exclusive).
 *
 * Hydra_Software_Devel/99   10/20/05 12:43p syang
 * PR 12762: prepare to support feeding gfx surface by mfd
 *
 * Hydra_Software_Devel/98   10/19/05 11:41a pntruong
 * PR17568: Added "key CRC frames" boolean to XVD and handle in VDC.
 * Added bCrcCapture flag.
 *
 * Hydra_Software_Devel/97   10/11/05 6:29p rgreen
 * PR17542: Share HDMI declarations between HDM and HDR portinginterfaces
 *
 * Hydra_Software_Devel/96   9/21/05 3:00p jasonh
 * PR 17261: Added mem heap to MVD/XVD structure.
 *
 * Hydra_Software_Devel/95   9/21/05 2:12p gmullen
 * PR15309: Added enums for AAC HE and AC3+ audio support.
 *
 * Hydra_Software_Devel/94   9/1/05 8:01p gmullen
 * PR15309: Added BAVC_ItbEsType_eVc1Video to support VC1 video.
 *
 * Hydra_Software_Devel/93   9/1/05 6:23p jasonh
 * PR 16967: Removing XVD source
 *
 * Hydra_Software_Devel/92   9/1/05 2:07p pntruong
 * PR16676: Restored xvd picture name for legacy code.
 *
 * Hydra_Software_Devel/91   9/1/05 11:21a pntruong
 * PR16676, PR16941: Changed pan scan values in BAVC_MFD_Picture /
 * BAVC_VDC_HdDvi_Picture from 16 bit to 32 bit.
 *
 * Hydra_Software_Devel/91   9/1/05 11:20a pntruong
 * PR16676, PR16941: Changed pan scan values in BAVC_MFD_Picture /
 * BAVC_VDC_HdDvi_Picture from 16 bit to 32 bit.
 *
 * Hydra_Software_Devel/PR16676/1   9/1/05 3:39p shyam
 * PR 16676 : Change pan scan values in BAVC_MFD_Picture from 16 bit to 32
 * bit
 *
 * Hydra_Software_Devel/90   8/31/05 3:20p gmullen
 * PR15309: More changes for AVC support
 *
 * Hydra_Software_Devel/89   8/31/05 2:24p gmullen
 * PR15309: Changed value of BAVC_ItbEsType_eAvcVideo.
 *
 * Hydra_Software_Devel/88   8/29/05 5:59p pntruong
 * PR16824: Fixed previous check-in build errors.
 *
 * Hydra_Software_Devel/87   8/29/05 2:23p jasonh
 * PR 16824: Removed unneeded elements. Clarified comments for other
 * entries.
 *
 * Hydra_Software_Devel/86   8/24/05 2:56p gmullen
 * PR15309: Added comments for ContextIdx.
 *
 * Hydra_Software_Devel/85   8/24/05 2:53p gmullen
 * PR15309: Added ContextIdx to RAVE register mapping.
 *
 * Hydra_Software_Devel/84   8/24/05 10:02a maivu
 * PR 16400: Removed clipping flag since it is no longer needed
 *
 * Hydra_Software_Devel/83   8/22/05 3:17p maivu
 * PR 16400: Added flag to indicate whether clipping was applied for
 * panscan.
 *
 * Hydra_Software_Devel/82   8/12/05 8:55a gmullen
 * PR15309: Added endianess control to BAVC_CdbItbConfig.
 *
 * Hydra_Software_Devel/81   8/9/05 5:35p maivu
 * PR 16582: Added lPtsStcOffset to BAVC_VDC_HdDvi_Picture
 *
 * Hydra_Software_Devel/80   8/4/05 12:53p ebrakus
 * PR16052: Added data structure for communication of ITB/CDB sizes
 * between video decoder and XPT
 *
 * Hydra_Software_Devel/79   7/22/05 3:30p hongtaoz
 * PR15630, PR16218: removed chip-dependency from bavc.h;
 *
 * Hydra_Software_Devel/78   7/22/05 10:10a yuxiaz
 * PR16182: Change pan scan variables to 32-bit in BAVC_XVD_Picture.
 *
 * Hydra_Software_Devel/77   7/18/05 6:04p hongtaoz
 * PR15630, PR16218: sychronize VBI encoder control registers programming
 * through scratch registers between BVDC and BVBI;
 *
 * Hydra_Software_Devel/76   7/15/05 9:42a gmullen
 * PR15309: Added CDB and ITB buffer structs for RAVE.
 *
 * Hydra_Software_Devel/75   7/14/05 3:22p rgreen
 * PR15086: Add all HDMI RAM Packet Types; to be shared between HDM and
 * HDR modules.
 *
 * Hydra_Software_Devel/74   7/13/05 9:48a yuxiaz
 * PR16182: Remove #ifdef from BAVC_SourceId.
 *
 * Hydra_Software_Devel/73   7/12/05 7:19p rgreen
 * PR15086: Add HDMI specific enumerations for AVI Infoframe for sharing
 * between HDM and HDR. Ise BAVC_Hddvi_Picture for data transfer between
 * HDR and VDC blocks
 *
 * Hydra_Software_Devel/72   7/12/05 9:23a yuxiaz
 * PR16182: Added new BAVC_SourceId and BAVC_XVD_Picture for 7401 XVD
 * input.
 *
 * Hydra_Software_Devel/71   6/30/05 5:05p rgreen
 * PR14847: Modify HDMI PI / Settop API to detect/configure audio
 * configuration
 *
 * Hydra_Software_Devel/70   6/27/05 4:33p jasonh
 * PR 16013: Edits for 7401
 *
 * Hydra_Software_Devel/69   6/23/05 6:25p vsilyaev
 * PR 15967: Added common type for tagged PTS.
 *
 * Hydra_Software_Devel/68   5/31/05 1:38p darnstein
 * PR 15707: new typedef BAVC_USERDATA_info and two supporting typedefs.
 *
 * Hydra_Software_Devel/68   5/31/05 1:37p darnstein
 * PR 15707: new BAVC_USERDATA_info typedef and two supporting typedefs.
 *
 * Hydra_Software_Devel/67   5/18/05 11:40a agin
 * PR14720: B2, C1, C2 compilation support.
 *
 * Hydra_Software_Devel/67   5/18/05 11:40a agin
 * PR14720: B2, C1, C2 compilation support.
 *
 * Hydra_Software_Devel/66   5/3/05 1:46p hongtaoz
 * PR14976: added support for 0-255 full range RGB dvi output;
 *
 * Hydra_Software_Devel/65   5/3/05 1:42p maivu
 * PR 15086: Added bStreamProgressive field for hddvi source
 *
 * Hydra_Software_Devel/64   4/19/05 4:16p sri
 * PR 14858 : Adding Progressive Frame bit
 *
 * Hydra_Software_Devel/63   4/13/05 1:28p bandrews
 * PR13974: 3560 also has SPDIF receiver with many sample rates
 *
 * Hydra_Software_Devel/62   3/18/05 6:35p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/61   2/17/05 5:31p maivu
 * PR 13882: Added compressed multi-stream to BAVC_StreamType
 *
 * Hydra_Software_Devel/60   1/28/05 11:14a yuxiaz
 * PR13569: Change bResetHdDvi to bResetHdDviBegin and bResetHdDviEnd to
 * sync vdc with xvd.
 *
 * Hydra_Software_Devel/59   1/10/05 10:34a maivu
 * PR 13238: Rename bNeedSync to bResetHdDvi
 *
 * Hydra_Software_Devel/58   12/22/04 2:31p darnstein
 * PR 13631:  Implement callback for the unwanted interrupt.
 * PR 13365:  Support new AVC information (partially done).
 *
 * Hydra_Software_Devel/57   12/20/04 2:11p darnstein
 * PR 13506: Adapt to change BAVC_AspectRatio->BFMT_AspectRatio.
 *
 * Hydra_Software_Devel/56   12/9/04 4:20p maivu
 * PR 13238: Added more info to BAVC_VDC_HdDvi_Picture
 *
 * Hydra_Software_Devel/55   11/4/04 5:51p gmullen
 * PR11894: Added parser bands 6, 7, 8, and 9 to BAVC_XptOutputId .
 *
 * Hydra_Software_Devel/54   11/2/04 4:57p bandrews
 * PR12965: Added Audio_Info struct for uniformity between audio and video
 * rate change callbacks.
 *
 * Hydra_Software_Devel/53   10/19/04 4:52p pntruong
 * PR13044: Implement callback to notify external modules of VDC's rate
 * manager update.
 *
 * Hydra_Software_Devel/52   10/12/04 11:35a maivu
 * PR 11978: Renamed Windows media 9 to VC1
 *
 * Hydra_Software_Devel/51   9/28/04 9:18p shyam
 * PR 12588 : Provide field in BAVC_MFD_Picture struct to indicate whether
 * source is progressive or interlaced
 *
 * Hydra_Software_Devel/50   9/21/04 2:41p jasonh
 * PR 12728: Fixed -pdantic errors.
 *
 * Hydra_Software_Devel/49   8/26/04 6:36p syang
 * PR 11266: added src picture call back func for gfx;  added src state to
 * call back func for XVD sync;
 *
 * Hydra_Software_Devel/48   8/13/04 2:27p maivu
 * PR 11978: Added video compression enum for XVD
 *
 * Hydra_Software_Devel/47   7/16/04 1:20p aram
 * PR11788: added some more enums to SR
 *
 * Hydra_Software_Devel/46   6/28/04 3:17p hongtaoz
 * PR11613: merged HDMI-specific color space enums into avc matrix coeff
 * enums to be shared across vdc and hdmi porting interfaces;
 *
 * Hydra_Software_Devel/45   6/25/04 10:40a maivu
 * PR 11658: Rename BAVC_HdDvi_Picture to BAVC_VDC_HdDvi_Picture
 *
 * Hydra_Software_Devel/44   6/24/04 3:09p maivu
 * PR 11658: Rename BAVC_XVD_Field to BAVC_HdDvi_Picture
 *
 * Hydra_Software_Devel/43   5/24/04 3:14p jasonh
 * PR 11189: Merge down from B0 -> main line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/3   5/21/04 6:56p hongtaoz
 * PR11103: added user-configurable default HD/SD color matrix settings to
 * the BVDC default setting structure; updated comments for color space
 * enums;
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   5/19/04 10:08a yuxiaz
 * PR 10943: Merged in BAVC_XVD_Field.
 *
 * Hydra_Software_Devel/42   5/17/04 4:50p maivu
 * PR 10943: Added BAVC_XVD_Field
 *
 * Hydra_Software_Devel/41   4/30/04 5:02p shyam
 * PR 10286 : Added unknowns in enums for AspectRatio and FrameRateCode
 *
 * Hydra_Software_Devel/40   4/7/04 6:47p hongtaoz
 * PR10484: change AVC module's color space parameters as enums; added
 * some implementation comments.
 *
 * Hydra_Software_Devel/39   4/5/04 7:29p hongtaoz
 * PR10484: Change matrix coeffecient enums in AVC module to comply with
 * MPEG-2 video spec.
 *
 * Hydra_Software_Devel/38   4/2/04 3:26p gmullen
 * PR8852: Added BAVC_GetXptId().
 *
 * Hydra_Software_Devel/37   3/12/04 6:56p hongtaoz
 * PR10003: Change aspect ratio enums in AVC module; removed
 * ProgressiveSequence bool.
 *
 * Hydra_Software_Devel/36   12/29/03 5:05p aram
 * PR8967: added timebase 3
 *
 * Hydra_Software_Devel/35   12/19/03 4:01p jasonh
 * PR 9073: brought back scan type structure (used for HDMI)
 *
 * Hydra_Software_Devel/34   12/19/03 2:37p pntruong
 * PR 9073:  Renamed BAVC_Polarity enums.
 *
 * Hydra_Software_Devel/33   12/19/03 1:35p jasonh
 * PR 9073: Combined scan types and field polarity enums into one enum.
 * Removed bFormatChange element from structure.
 *
 * Hydra_Software_Devel/32   12/5/03 4:35p jasonh
 * PR 8841: Clarified units of ulStride.
 *
 * Hydra_Software_Devel/31   12/4/03 10:47a jasonh
 * PR 8841: Added virtual memory description to luma/chroma buffers.
 *
 * Hydra_Software_Devel/30   12/4/03 10:47p shyam
 * Changed the Buffer address from uint32 to void * pointers
 *
 * Hydra_Software_Devel/29   12/2/03 5:01p aram
 * fixed a typo
 *
 * Hydra_Software_Devel/28   12/2/03 11:58a vsilyaev
 * Added type for MPEG stream formats.
 *
 * Hydra_Software_Devel/28   12/2/03 11:55a vsilyaev
 * Added common enum for the MPEG stream formats.
 *
 * Hydra_Software_Devel/27   12/2/03 11:29a jasonh
 * Split out transfer characteristics and color primaries into their own
 * structure elements.
 *
 * Hydra_Software_Devel/26   11/24/03 1:54p jasonh
 * Returned end of file newline (warnings)
 *
 * Hydra_Software_Devel/25   11/14/03 5:30p rgreen
 * Add constants for Audio Bits and Audio Format.  Supported parameters
 * will be used to query HDMI Receivers for their supported
 * parameters.
 *
 * Hydra_Software_Devel/24   11/12/03 10:48a jasonh
 * PR 8606: Added interrupt field polarity to MVD_Field structure.
 *
 * Hydra_Software_Devel/23   11/10/03 4:04p yuxiaz
 * Change BAVC_InterpolationMode to match hardware values.
 *
 * Hydra_Software_Devel/22   10/31/03 4:42p aram
 * moving audio SR from baud_types to this header file
 *
 * Hydra_Software_Devel/20   10/16/03 3:45p pntruong
 * Updated source id to handle analog component inputs.
 *
 * Hydra_Software_Devel/19   10/9/03 6:07p darnstein
 * Undo my change to the field polarity enum, it broke the vdc pi module.
 * Also, consistently apply a starting value to each enum.  The latter is
 * a cosmetic issue.
 *
 * Hydra_Software_Devel/18   10/9/03 1:26p darnstein
 * Enum for field parity can now be used as a bitmask.
 *
 * Hydra_Software_Devel/17   10/2/03 4:02p yuxiaz
 * Change pan scan in BAVC_MFD_Picture from uint16_t to int16_t.
 *
 * Hydra_Software_Devel/16   9/25/03 12:45p jasonh
 * Fixed MPEG enumeration. Added 444 to YCbCr enum.
 *
 * Hydra_Software_Devel/16   9/25/03 12:07p jasonh
 * Fixed mpeg type enumeration values and added 444 to YCbCr type.
 *
 * Hydra_Software_Devel/15   9/25/03 10:14a aram
 * a field name change to eXptSourceId
 *
 * Hydra_Software_Devel/14   9/24/03 4:47p aram
 * added timebase
 *
 * Hydra_Software_Devel/13   9/24/03 9:29a jasonh
 * Changed enumeration values to match the ISO 13818 specification.
 *
 * Hydra_Software_Devel/12   9/23/03 2:32p yuxiaz
 * Added bFormatChange, ulRowStride and more comments.
 *
 * Hydra_Software_Devel/11   9/15/03 10:50a marcusk
 * Added standard transport output enum and structure.
 *
 * Hydra_Software_Devel/10   9/8/03 4:07p yuxiaz
 * Change unsigned int to uint32_t
 *
 * Hydra_Software_Devel/9   9/3/03 1:45p jasonh
 * Added bMute to MVD Field structure.
 *
 * Hydra_Software_Devel/7   9/2/03 2:38p pntruong
 * PR7854: 7038 VDC porting interface shall differentiate the IF Demod
 * analog video input with the line-in CVBS input.
 *
 * Hydra_Software_Devel/6   8/29/03 5:44p pntruong
 * Moved BVDC_SourceId to BAVC_SourceId.
 *
 * Hydra_Software_Devel/5   8/29/03 11:37a darnstein
 * Add enumeration BAVC_VbiPath.
 *
 * Hydra_Software_Devel/4   8/28/03 6:31p pntruong
 * Moved BVDC_SourceId here, as BVBI also needed the source id when vbi
 * BBVI_Decode_Create.
 *
 * Hydra_Software_Devel/3   8/11/03 9:02a jasonh
 * (PR 7736) Added BT.601 to matrix coefficient selection.
 *
 * Hydra_Software_Devel/2   8/4/03 2:20p jasonh
 * Initial shot at documentation comments.
 *
 * Hydra_Software_Devel/1   7/31/03 6:28p jasonh
 * Initial version.
 *
 ***************************************************************************/

#ifndef BAVC_H__
#define BAVC_H__

#include "bsur.h"
#include "bfmt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=Module Overview: ********************************************************
The purpose of this common utility is to create a location to contained
shared data structures used to pass data between the various audio/video
porting interface modules. This utility does not contain any functions.

The modules currently using this common utility are
   o MVD - MPEG Video Decoder
   o VDC - Video Display Control
   o XPT - Data Transport
   o AUD - Audio
   o XVD - X Video Decoder

See Also:
   BMVD_Open,
   BVDC_Open
****************************************************************************/
/* xpt related common defines */
#include "bavc_xpt.h"

/***************************************************************************
Summary:
    These defines describe the scratch registers that hold the current
    settings of the VBI Encoder control registers.

Description:
    Define scratch registers to coordinate programming of VBI encoder
    control registers between BVBI and BVDC. BDC_Open would reset those
    registers with the default setting, i.e. only VBI pass-through is
    enabled; the following BVBI_Encode_ApplyChanges would update the
    settings by read/modify/write those scratch registers; the following
    vec vsync isr would read back the scratch setting and build RUL
    to reprogram the corresponding VBI_ENC_xxx_Control register;

See Also:
    BVDC_Open
    BVBI_Encode_ApplyChanges
    BAVC_VbiPath
***************************************************************************/
/* this is the AVC wrapper of VBI_ENC_656_Ancil_Control register */
#ifdef BCHP_RDC_scratch_i_ARRAY_BASE
#define BAVC_VBI_ENC_BP_CTRL_SCRATCH     \
	(BCHP_RDC_scratch_i_ARRAY_BASE + BCHP_RDC_scratch_i_ARRAY_END * sizeof(uint32_t))
#define BAVC_VBI_ENC_0_CTRL_SCRATCH      \
	(BCHP_RDC_scratch_i_ARRAY_BASE + (BCHP_RDC_scratch_i_ARRAY_END - 1) * sizeof(uint32_t))
#define BAVC_VBI_ENC_1_CTRL_SCRATCH      \
	(BCHP_RDC_scratch_i_ARRAY_BASE + (BCHP_RDC_scratch_i_ARRAY_END - 2) * sizeof(uint32_t))
#define BAVC_VBI_ENC_2_CTRL_SCRATCH      \
	(BCHP_RDC_scratch_i_ARRAY_BASE + (BCHP_RDC_scratch_i_ARRAY_END - 3) * sizeof(uint32_t))
#endif

/***************************************************************************
Summary:
    This definition defines the maximum number of mosaic rectangless in a window.

Description:
    This is for video window mosaic mode usage only.  The XVD/MVD and XPT also
    needs this macro to allocate approriate number of channels.

See Also:
    BVDC_Window_SetMosaic, BVDC_Window_GetMosaicDstRects
***************************************************************************/
#define BAVC_MOSAIC_MAX                      (16)

/***************************************************************************
Summary:
    This enumeration represents the available video input source.

Description:
    BAVC_SourceId is an enumeration which represents the source devices to
    VDC module and VBI. It is needed by BVDC_Source_Create to create a
    source handle, and BVBI_Decode_Create.

    There are two major type of sources, the video source and graphics
    source. For example, Mpeg feeder, vdec and ITU-R-656 input are video
    sources, and graphics feeder is graphics source.

    Right after the pixel data is input to VDC module, it is automatically
    expanded to a 4:4:4:4 AYCrCb or ARGB format with 8 bits per component.
    Then the expanded color components are further processed in the VDC
    module.

    After some intra picture processing, pixels from several source could
    could be finally blended together in a compositor. Before the blending
    is performed, color space conversion would be done automatically to
    pixels from each source to match the main source.

    And before the color space conversion, a video input could involve
    dimension scaling / filtering, luma keying intra picture processing,
    and a graphics input could involve a series of operations in the order
    of color key, alpha pre-multiplying, horizontal up scaling, and gamma
    correction.

    The number of different sources (eMpeg0, eVdec0, and etc) might not be
    available on all chipset.  This will be chip specific configurations.
    Calling BVDC_Source_Create will return an error if the source is not
    available on that particular chip.

    There are two graphics feeders in the 7038 chip. Each of them fetches
    pixel data from the video memory and sends to the video compositors. A
    variety of YCrCb, ARGB and CLUT pixel formats are supported for both SD
    and HD applications. Please refer to BPXL module for the complete list.

    The eVfdn sources are used for graphics sources only. There are
    limitations to its use. The corresponding CAP block cannot be used
    because only 1 VFD can be used in the video path. Note that the use of
    CAP requires the use of its corresponding VFD. Only 1 window can be used
    with a VFD. All current window assignments with a particular VFD will
    remain. For example, VFD_0 will be used with the CMP0_V0 window.

See Also:
    BVDC_Source_Create, BVDC_Source_Destroy, BVDC_Source_Handle
***************************************************************************/
typedef enum BAVC_SourceId
{
	BAVC_SourceId_eMpeg0 = 0,        /* Mpeg feeder 0 */
	BAVC_SourceId_eMpeg1,            /* Mpeg feeder 1 */
	BAVC_SourceId_eMpeg2,            /* Mpeg feeder 2 */
	BAVC_SourceId_eMpeg3,            /* Mpeg feeder 3 */
	BAVC_SourceId_eMpeg4,            /* Mpeg feeder 4 */
	BAVC_SourceId_eMpeg5,            /* Mpeg feeder 5 */
	BAVC_SourceId_eMpegMax = BAVC_SourceId_eMpeg5,
	BAVC_SourceId_eVdec0,            /* Analog video source from vdec0 */
	BAVC_SourceId_eVdec1,            /* Analog video source from vdec1 */
	BAVC_SourceId_e656In0,           /* ITU-R-656 video source.0 */
	BAVC_SourceId_e656In1,           /* ITU-R-656 video source 1 */
	BAVC_SourceId_eGfx0,             /* Gfx feeder 0 */
	BAVC_SourceId_eGfx1,             /* Gfx feeder 1 */
	BAVC_SourceId_eGfx2,             /* Gfx feeder 2 */
	BAVC_SourceId_eGfx3,             /* Gfx feeder 3 */
	BAVC_SourceId_eGfx4,             /* Gfx feeder 4 */
	BAVC_SourceId_eGfx5,             /* Gfx feeder 5 */
	BAVC_SourceId_eGfx6,             /* Gfx feeder 6 */
	BAVC_SourceId_eGfxMax = BAVC_SourceId_eGfx6,
	BAVC_SourceId_eHdDvi0,           /* HD DVI Input 0 */
	BAVC_SourceId_eHdDvi1,           /* HD DVI Input 1 */
	BAVC_SourceId_eDs0,              /* DownStream Input 0 */
	BAVC_SourceId_eVfd0,             /* Video feeder 0 */
	BAVC_SourceId_eVfd1,             /* Video feeder 1 */
	BAVC_SourceId_eVfd2,             /* Video feeder 2 */
	BAVC_SourceId_eVfd3,             /* Video feeder 3 */
	BAVC_SourceId_eVfd4,             /* Video feeder 4 */
	BAVC_SourceId_eVfd5,             /* Video feeder 5 */
	BAVC_SourceId_eVfd6,             /* Video feeder 6 */
	BAVC_SourceId_eVfd7,              /* Video feeder 7 */
	BAVC_SourceId_eVfdMax = BAVC_SourceId_eVfd7,
	BAVC_SourceId_eMax               /* Max source  */

} BAVC_SourceId;

/***************************************************************************
Summary:
	This enumeration represents the different analog decoder.

Description:
	BAVC_DecoderId is an enumeration which represents the analog decoder
	that is repsonsible for decode analog input.

	BAVC_DecoderId_eVindeco_0 -
		This represent the analog decoder (VINDECO) for VGA or YPrPb input.

	BAVC_DecoderId_eVdec_0 -
		This represent the analog decoder (VDEC) for composite/SVIDEO/SCART/IF
		Demodulator input.

	BAVC_DecoderId_e656In_0 -
		This represent the 656 input. This generally means external analog
		decoder.

	BAVC_DecoderId_eMaxCount -
	    Counter. Do not use.

See Also:
	BANV_Source_GetDefaultSettings, BANV_Source_Create, BANV_Source_Destroy,
	BVDC_Source_SetDecoderID, BVDC_Source_GetDecoderID.
***************************************************************************/
typedef enum
{
	BAVC_DecoderId_eVindeco_0 = 0,
	BAVC_DecoderId_eVdec_0,
	BAVC_DecoderId_e656In_0,

	BAVC_DecoderId_eMaxCount

} BAVC_DecoderId;

/***************************************************************************
Summary:
    This enumeration describes the VBI paths within the Video Encoder block

Description:
    Contains the path taken inside the VEC, which is required by the VBI PI.
    BAVC_VbiPath_eVec0 - Represent the Primary VEC VBI encoder.
    BAVC_VbiPath_eVec1 - Represent the Secondary VEC VBI encoder.
    BAVC_VbiPath_eVec2 - Represent the Tertiary VEC VBI encoder.
    BAVC_VbiPath_eBypass0 - Represent the first 656 Bypass VEC VBI encoder.
    BAVC_VbiPath_eBypass1 - Represent the second 656 Bypass VEC VBI encoder.

See Also:
    BVDC_Display_GetVbiPath
***************************************************************************/
typedef enum BAVC_VbiPath
{
	BAVC_VbiPath_eVec0 = 0,            /* Primary VEC path */
	BAVC_VbiPath_eVec1,                /* Secondary VEC path */
	BAVC_VbiPath_eVec2,                /* Tertiary VEC path */
	BAVC_VbiPath_eBypass0,             /* 656 vec */
	BAVC_VbiPath_eBypass1,             /* 656 vec */

	BAVC_VbiPath_eUnknown              /* Unknown path */

} BAVC_VbiPath;

/***************************************************************************
Summary:
    Used to specify scan type.

Description:

See Also:
****************************************************************************/
typedef enum BAVC_ScanType
{
	BAVC_ScanType_eInterlaced = 0,   /* Interlaced */
	BAVC_ScanType_eProgressive       /* Progressive */

} BAVC_ScanType;


/***************************************************************************
Summary:
    Used to specify field polarity.

Description:
    The values are suitable for assignment to an integer bitmask.

See Also:
****************************************************************************/
typedef enum BAVC_Polarity
{
	BAVC_Polarity_eTopField = 0,       /* Top field */
	BAVC_Polarity_eBotField,           /* Bottom field */
	BAVC_Polarity_eFrame               /* Progressive frame */

} BAVC_Polarity;

/***************************************************************************
Summary:
    Used to specify interpolation mode.

Description:

See Also:
****************************************************************************/
typedef enum BAVC_InterpolationMode
{
	BAVC_InterpolationMode_eField = 0,  /* Field */
	BAVC_InterpolationMode_eFrame       /* Frame */

} BAVC_InterpolationMode;

/***************************************************************************
Summary:
    Used to specify Chroma location type.

Description:
    LT1-----C-----LT2
     |             |
     |             |
     A------B------|
     |             |
     |             |
    LB1-----D-----LB2

    Luma sample position:
        Top field:    LT1, LT2
        Bottom field: LB1, LB2

    Chroma sample position:
        BAVC_ChromaLocation_eType0: A
        BAVC_ChromaLocation_eType1: B
        BAVC_ChromaLocation_eType2: LT1
        BAVC_ChromaLocation_eType3: C
        BAVC_ChromaLocation_eType4: LB1
        BAVC_ChromaLocation_eType5: D
See Also:
****************************************************************************/
typedef enum BAVC_ChromaLocation
{
	BAVC_ChromaLocation_eType0 = 0,
	BAVC_ChromaLocation_eType1,
	BAVC_ChromaLocation_eType2,
	BAVC_ChromaLocation_eType3,
	BAVC_ChromaLocation_eType4,
	BAVC_ChromaLocation_eType5

} BAVC_ChromaLocation;

/* Backward compatibility */
#define BAVC_MpegType_eMpeg1     BAVC_ChromaLocation_eType1
#define BAVC_MpegType_eMpeg2     BAVC_ChromaLocation_eType0

/***************************************************************************
Summary:
    Used to specify YCbCr type.

Description:
    The values assigned to these enumerations should be kept in step with
    the ISO 13818-2 specification to minimize effort converting to these
    types.

See Also:
****************************************************************************/
typedef enum BAVC_YCbCrType
{
	BAVC_YCbCrType_e4_2_0 = 1,  /* 4:2:0 */
	BAVC_YCbCrType_e4_2_2    ,  /* 4:2:2 */
	BAVC_YCbCrType_e4_4_4       /* 4:4:4 (unsupported) */

} BAVC_YCbCrType;


/***************************************************************************
Summary:
    Use to specify what color space conversion matrix should be use by
    HD_DVI.

Description:
    BAVC_CscMode_e709RgbFullRange - Conversion from
       [Full Range RGB BT.709 / xvYCC 709] to [YCbCr BT.709 / xvYCC 709 ]

    BAVC_CscMode_e709RgbLimitedRange -  Conversion from
       [Limited Range RGB BT.709 / xvYCC 709] to [YCbCr BT.709 / xvYCC 709 ]

    BAVC_CscMode_e709YCbCr - Conversion from YCbCr to YCbCr, no conversion needed.

    BAVC_CscMode_e601RgbFullRange - Conversion from
       [Full Range RGB BT.601 / SMPTE 170M / xvYCC 601] to
       [YCbCr BT.601 / SMPTE 170M / xvYCC 601 ]

    BAVC_CscMode_e601RgbLimitedRange - Conversion from
       [Limited Range RGB BT.601 / SMPTE 170M / xvYCC 601] to
       [YCbCr BT.601 / SMPTE 170M / xvYCC 601 ]

    BAVC_CscMode_e601YCbCr - Conversion from YCbCr to YCbCr, no conversion needed.

See Also:
****************************************************************************/
typedef enum
{
	BAVC_CscMode_e709RgbFullRange = 0,
	BAVC_CscMode_e709RgbLimitedRange,
	BAVC_CscMode_e709YCbCr,

	BAVC_CscMode_e601RgbFullRange,
	BAVC_CscMode_e601RgbLimitedRange,
	BAVC_CscMode_e601YCbCr,

	/* Must be last */
	BAVC_CscMode_eMax

} BAVC_CscMode;



/***************************************************************************
Summary:
    Used to specify color space type.  This extracted out of the AVI info
    frame.  Y1Y0.

Description:

See Also:
****************************************************************************/
typedef enum
{
	BAVC_Colorspace_eRGB = 0,
	BAVC_Colorspace_eYCbCr422,
	BAVC_Colorspace_eYCbCr444,
	BAVC_Colorspace_eFuture

} BAVC_Colorspace;

/***************************************************************************
Summary:
    Used to specify the possible frame rates.

Description:
    The values assigned to these enumerations should be kept in step with
    the ISO 13818-2 specification to minimize effort converting to these
    types.

See Also:
****************************************************************************/
typedef enum BAVC_FrameRateCode
{
	BAVC_FrameRateCode_eUnknown = 0, /* Unknown */
	BAVC_FrameRateCode_e23_976 = 1,
	BAVC_FrameRateCode_e24,
	BAVC_FrameRateCode_e25,
	BAVC_FrameRateCode_e29_97,
	BAVC_FrameRateCode_e30,
	BAVC_FrameRateCode_e50,
	BAVC_FrameRateCode_e59_94,
	BAVC_FrameRateCode_e60,
	BAVC_FrameRateCode_e14_985,
	BAVC_FrameRateCode_e7_493,
	BAVC_FrameRateCode_e10,
	BAVC_FrameRateCode_e15,
	BAVC_FrameRateCode_e20,

	BAVC_FrameRateCode_eMax          /* Max Enum Value */

} BAVC_FrameRateCode;

/* TODO: to be removed shortly after xvd/synclib/etc adapt to new naming changes. */
#define BAVC_FrameRateCode_e7_943 BAVC_FrameRateCode_e7_493

/***************************************************************************
Summary:
    Used to specify the possible standard matrix coefficients types for
    YCbCr color space interpretation of the digital video data.

Description:
    This enum follows the MPEG-2 standard Video-spec except the RGB output
    enum(=3), which is intended for HDMI use only in case of RGB output to
    an HDMI Rx device to indicate the RGB color space interpretation of the
    digital video data.

See Also:
    BAVC_ColorPrimaries, BAVC_TransferCharacteristics.
****************************************************************************/
typedef enum BAVC_MatrixCoefficients
{
	/* Recommendation ITU-R BT.709;
	   (ATSC HD or PAL HD, same as SMPTE274 and EIA770.3);

	   note: it may also be used to indicate HD YCbCr output to an HDMI Rx
	   device. */
	BAVC_MatrixCoefficients_eItu_R_BT_709 = 1,

	/* FCC;
	   (NTSC SD 1953, not exactly the same as BT.601, but similar) */
	BAVC_MatrixCoefficients_eFCC = 4,

	/* Recommendation ITU-R BT.470-2 System B, G;
	   (PAL SD, same as BT.601) */
	BAVC_MatrixCoefficients_eItu_R_BT_470_2_BG = 5,

	/* SMPTE 170M;
	   (NTSC SD, same as BT.601; but color primaries are different
	   from BT.470-2 System B, G.);

	   note: it may also be used to indicate SD YCbCr output to an HDMI Rx
	   device. */
	BAVC_MatrixCoefficients_eSmpte_170M = 6,

	/* SMPTE 240M (1987);
	   (ATSC HD; color primaries are the same as SMPTE 170M.) */
	BAVC_MatrixCoefficients_eSmpte_240M = 7,

	/* HDMI 1.3;
	   (xvYCC709) */
	BAVC_MatrixCoefficients_eXvYCC_709 = 8,

	/* HDMI 1.3;
	   (xvYCC601) */
	BAVC_MatrixCoefficients_eXvYCC_601 = 9,

	/* Unspecified Video: Image characteristics are unknown;
	   VDC would handle 'Unknown' case as, i.e.
	   if the decoded picture is in HD format(size is larger than
	   720x576), then take default HD color matrix; else take default SD color
	   matrix.

	   Note: it may also be used to indicate the display doesn't output HDMI
	   video. */
	BAVC_MatrixCoefficients_eUnknown = 2,

	/* 0, 3,  8 - 255 reserved;
	   MVD should handle the error and provide 'Unknown' to VDC. */

	/***********************************************************
	 * NOTE: the following enum values are for HDMI/DVI display use only;
	 *           video decompressor should not pass these values to vdc via the
	 *           BAVC_MFD_Picture or BAVC_VDC_HdDvi_Picture data structure.
	 * TODO: what about hd-dvi input when hd-dvi input module behaves as
	 *           a stand-alone DVI Rx device?
	 */
	/* for HDMI use only, in case of 220 steps limited range(16-236) RGB
	    output to an HDMI Rx device */
	BAVC_MatrixCoefficients_eHdmi_RGB = 0,

	/* for DVI use only, in case of 8-bit full range(0-255) RGB output to a
	    DVI PC monitor */
	BAVC_MatrixCoefficients_eDvi_Full_Range_RGB = 3

 } BAVC_MatrixCoefficients;

/***************************************************************************
Summary:
    Used to specify the possible color primaries.

Description:
    This enum follows the MPEG-2 standard Video-spec.

See Also:
    BAVC_MatrixCoefficients, BAVC_TransferCharacteristics.
****************************************************************************/
typedef enum BAVC_ColorPrimaries
{
	/* Recommendation ITU-R BT.709;
	   (ATSC HD or PAL HD) */
	BAVC_ColorPrimaries_eItu_R_BT_709 = 1,

	/* Recommendation ITU-R BT.470-2 System M;
	   (NTSC SD 1953, not the same as PAL SD nor SMPTE170) */
	BAVC_ColorPrimaries_eItu_R_BT_470_2_M = 4,

	/* Recommendation ITU-R BT.470-2 System B, G;
	   (PAL SD, similar to SMPTE170) */
	BAVC_ColorPrimaries_eItu_R_BT_470_2_BG = 5,

	/* SMPTE 170M; (NTSC SD) */
	BAVC_ColorPrimaries_eSmpte_170M = 6,

	/* SMPTE 240M (1987);
	   (ATSC HD; same as SMPTE170) */
	BAVC_ColorPrimaries_eSmpte_240M = 7,

	/* Generic file
	   AVC specification ??? */
	BAVC_ColorPrimaries_eGenericFilm = 8,

	/* Unspecified Video: Image characteristics are unknown;
	   VDC would handle 'Unknown' case as follows, i.e.
	   if the decoded picture is in HD format(size is larger than
	   720x576), then take default HD color matrix; else take default SD color
	   matrix. */
	BAVC_ColorPrimaries_eUnknown = 2

	/* 0 - forbidden;
	   MVD should handle the error and provide 'Unknown' to VDC. */

	/* 3, 8 - 255 reserved;
	   MVD should handle the error and provide 'Unknown' to VDC. */

} BAVC_ColorPrimaries;

/***************************************************************************
Summary:
    Used to specify the possible transfer characteristics.

Description:
    This enum follows the MPEG-2 standard Video-spec.

See Also:
    BAVC_MatrixCoefficients, BAVC_ColorPrimaries.
****************************************************************************/
typedef enum BAVC_TransferCharacteristics
{
	/* Recommendation ITU-R BT.709;
	   (ATSC HD or PAL HD) */
	BAVC_TransferCharacteristics_eItu_R_BT_709 = 1,

	/* FCC, or Recommendation ITU-R BT.470-2 System M;
	   (NTSC SD 1953, assumed display gamma 2.2) */
	BAVC_TransferCharacteristics_eItu_R_BT_470_2_M = 4,

	/* Recommendation ITU-R BT.470-2 System B, G;
	   (PAL SD, assumed display gamma 2.8) */
	BAVC_TransferCharacteristics_eItu_R_BT_470_2_BG = 5,

	/* SMPTE 170M; (NTSC SD) */
	BAVC_TransferCharacteristics_eSmpte_170M = 6,

	/* SMPTE 240M (1987); (ATSC HD) */
	BAVC_TransferCharacteristics_eSmpte_240M = 7,

	/* Linear Transfer Characteristics */
	BAVC_TransferCharacteristics_eLinear = 8,

	/* Recommendation ITU-T H.262, H.264; (IEC 61966-2-4 gamma, xvYCC) */
	BAVC_TransferCharacteristics_eIec_61966_2_4 = 11,

	/* Unspecified Video: Image characteristics are unknown;
	   VDC would handle 'Unknown' case as follows, i.e.
	   if the decoded picture is in HD format(size is larger than
	   720x576), then take default HD color matrix; else take default
	   SD color. */
	BAVC_TransferCharacteristics_eUnknown = 2

	/* 0 - forbidden;
	   MVD should handle the error and provide 'Unknown' to VDC. */

	/* 3, 9 - 255 reserved;
	   MVD should handle the error and provide 'Unknown' to VDC. */

} BAVC_TransferCharacteristics;

/***************************************************************************
Summary:
    Used to specify the source state.

Description:
    When source picture call back function is used to pull the video/graphics
    field/frame produced by an upstream module to BVDC for display, the
    current state of the used BVDC source sub-source is passed from BVDC to
    user as a parameter of the call back function, in order to synchronize
    the upstream module and BVDC.

See Also:
    BVDC_Source_PictureCallback_isr, BVDC_Source_InstallPictureCallback
****************************************************************************/
typedef enum BAVC_SourceState
{
	BAVC_SourceState_eActive_Create, /* The source was just created. */
	BAVC_SourceState_eActive_Sync,   /* BVDC has recieved a sync pulse. */
	BAVC_SourceState_eActive_Start,  /* BVDC has recieved the first frame after the sync pulse. */
	BAVC_SourceState_eActive_Reset,  /* We need to restart the syncronization process. */
	BAVC_SourceState_eActive,        /* Simply active. Normal case */
	BAVC_SourceState_eActive_Last,   /* The source is about to be destroyed. The next callback will be the last. */
	BAVC_SourceState_eDestroy        /* The source has been destroyed and is no longer active, This callback is the last. */

} BAVC_SourceState;


/***************************************************************************
Summary:
    Defines the timebase.

Description:
    This enum defines the timebase or pcr to PLLs, RateMangers
    and audio/video decoders/DACs.

See Also:
****************************************************************************/
typedef enum BAVC_Timebase
{
	BAVC_Timebase_e0 = 0,   /* TimeBase/PCR 0 */
	BAVC_Timebase_e1,       /* TimeBase/PCR 1 */
	BAVC_Timebase_e2,       /* TimeBase/PCR 2 */
	BAVC_Timebase_e3        /* TimeBase/PCR 3 */

}BAVC_Timebase;


/***************************************************************************
Summary:
    Used to specify stripe width.

Description:
    The values defines stripe width of the decoder.

See Also:
****************************************************************************/
typedef enum BAVC_StripeWidth
{
	BAVC_StripeWidth_e64Byte = 0,
	BAVC_StripeWidth_e128Byte,
	BAVC_StripeWidth_e256Byte

} BAVC_StripeWidth;

/***************************************************************************
Summary:
    The picture coding type

Description:
    This enum value follows MPEG picture coding type for I, P or B picture.
    The AVC decoder fw sets picture coding type according to the slice type
    of the 1st slice in picture. THE VC-1 and MPEG4 decoder fw sets it according
    to the codec syntax definition for the picture coding type.
    For the User data the Picture Header it can be associated with different
    MPEG coding types like I, P or B frames. This type is enumerated here.

See Also:

****************************************************************************/
typedef enum BAVC_PictureCoding
{
	BAVC_PictureCoding_eUnknown=0,    /* Picture Coding Type Unknown */
	BAVC_PictureCoding_eI,             /* Picture Coding Type I */
	BAVC_PictureCoding_eP,             /* Picture Coding Type P */
	BAVC_PictureCoding_eB,             /* Picture Coding Type B */
	BAVC_PictureCoding_eMax

} BAVC_PictureCoding;

/* The following is to be backward compatible. However, it should be deprecated. */
#define BAVC_USERDATA_PictureCoding                     BAVC_PictureCoding
#define BAVC_USERDATA_PictureCoding_eUnknown  BAVC_PictureCoding_eUnknown
#define BAVC_USERDATA_PictureCoding_eI               BAVC_PictureCoding_eI
#define BAVC_USERDATA_PictureCoding_eP               BAVC_PictureCoding_eP
#define BAVC_USERDATA_PictureCoding_eB               BAVC_PictureCoding_eB
#define BAVC_USERDATA_PictureCoding_eMax           BAVC_PictureCoding_eMax


/***************************************************************************
Summary:
	The picture coding type

Description:
	BAR data.

See Also:

****************************************************************************/
typedef enum BAVC_BarDataType
{
	BAVC_BarDataType_eInvalid = 0,     /* No data */
	BAVC_BarDataType_eTopBottom,       /* Bar data defines top/bottom */
	BAVC_BarDataType_eLeftRight        /* Bar data defines left/right */

} BAVC_BarDataType;

/***************************************************************************
Summary:
    A structure representing a field/frame of data from the decoder module.

Description:
    This structure is typically used to pass data from the decoder module to
    the VDC module. This should be done every field so that the VDC can
    display the appropriate field with the appropriate parameters.

    The structure is for programming the MFD in BVN.  Notice that both the
    XVD and MVD could prepare the structure for VDC.

    This structure might also represent a graphics surface which is intended
    to be used as a video source to MFD.

See Also:
    BVDC_Source_MpegDataReady_isr
****************************************************************************/
typedef struct BAVC_MFD_Picture
{
	/*----------------------------------------------------------------------*/
	/* Used by all (MVD/XVD/Surface) */

	/* Contains the field polarity of the VDC interrupt driving the callback.
	 * The VDC module provides this interrupt which drives the firmware
	 * to produce field data. This enumeration contains the polarity offscreen
	 * that interrupt and represents what type of field the VDC would
	 * like to feed out of the MPEG feeder. */
	BAVC_Polarity            eInterruptPolarity;

	/* Contains the display veritical refresh rate of the VDC interrupt driving
	 * the callback.  * The VDC module provides this refresh rate which drives
	 * the firmware to produce field data. This enumeration contains the refresh
	 * rate that interrupt and represents what rate the VDC would like to feed
	 * out of the MPEG feeder. */
	BFMT_Vert                eInterruptRefreshRate;

	/* Fast non-real-time (FNRT) support flag, to seed/pre-charge VDC internal
	 * state for modules like deinterlacer. Usually it is set for the overlap part
	 * between chunks n and n+1, preparing for the start of chunk n+1.It is also
	 * meta data relay to ViCE through mailbox by VDC */
	bool                      bPreChargePicture;

	/* Fast non-real-time (FNRT) support flage, to indicate last picture inside
	 * a chunk data. It is also meta data relay to ViCE through mailbox by VDC */
	bool                      bEndOfChunk;

	/* unintentional picture repeat due to decoder underflow at non-real time
	 * transcode mode Encoder FW should simply drop this picture with IGNORE
	 * flag set. No encode time stamp to be adjusted. The flag is generated by
	 * DM software. Ignore should be zero for all other cases.
	 * source: display manager */
	bool                     bIgnorePicture;

	/* video decoder underflow in non-real time transcode mode.  The video
	 * decoder STC should not increment when this flag is true. The flag is
	 * generated by DM software. bStallStc should be false for all other cases.
	 * source: display manager */
	bool                     bStallStc;

	/* DM will set the bLast flag to mark that the last picture decoded from
	 * source has been presented by DM. This flag is used in non-realtime
	 * transcode mode to cleanly flush/terminate transcoder at the end of
	 * source file. When it's true, bIgnorePicture should be true too. */
	bool                     bLast;

	/* Determines if the field is to be muted. This is set when all values
	 * except eInterruptPolarity, bIgnorePicture, bStallStc, bLast and
	 * bChannelChange are invalid and undefined. This flag notifies the VDC
	 * that the feeder should be programmed to feed a fixed constant color
	 * instead of using the supplied buffers. */
	bool                     bMute;

	/* heap from which the frame buffers were allocated;
	 * Null means to use the same main heap of VDC; */
	BMEM_Handle              hHeap;

	/* Source polarity.  This indicate that the buffer need to be scan
	 * out as eSourcePolarity (T/B/F polairy).  As determined by the
	 * correct display logic of MVD.  Which could override the original
	 * stream polarity. */
	BAVC_Polarity            eSourcePolarity;

	/* stream panscan vector specifies the position of the centre of
	 * the reconstructed frame from the centre of the display rectangle. */
	/* Units of 1/16th of a pixel in two's complement. Bits 31-16 are
	 * a sign extension of Bit 15. Bit 15 is the sign bit. Bits 14:8
	 * are the macro block grid.Bits 7:4 are the pixel grid. Bits 3:0
	 * are 1/16 pixel grid. */
	int32_t                  i32_HorizontalPanScan;

	/* Units of 1/16th of a pixel in two's complement. Bits 31-16 are
	 * a sign extension of Bit 15. Bit 15 is the sign bit. Bits 14:8
	 * are the macro block grid.Bits 7:4 are the pixel grid. Bits 3:0
	 * are 1/16 pixel grid. */
	int32_t                  i32_VerticalPanScan;

	/* When pan-scan is enabled, this is the horizontal size of the
	 * displayed portion of the buffer. */
	uint32_t                 ulDisplayHorizontalSize;

	/* When pan-scan is enabled, this is the vertical size of the
	 * displayed portion of the buffer. */
	uint32_t                 ulDisplayVerticalSize;

	/* Matrix coefficients. */
	BAVC_MatrixCoefficients  eMatrixCoefficients;

	/* Aspect ratio of the rectangle defined by the display
	 * parameters (ulDisplayHorizontalSize and ulDisplayVerticalSize)
	 * if it's not BFMT_AspectRatio_eSAR; otherwise, look at
	 * uiSampleAspectRatioX/uiSampleAspectRatioY to derive the sample
	 * aspect ratio; */
	BFMT_AspectRatio         eAspectRatio;

	/* Frame rate code. */
	BAVC_FrameRateCode       eFrameRateCode;

	/* hSurface must be NULL for MVD and XVD cases. When it is a valid graphics
	 * surface handle, the surface is feeded as the video source to the video
	 * window. For MPEG Feeder cores with a minor version earlier than 0x60
	 * (see MFD_0_REVISION_ID) and Video Feeder cores with a minor version
	 * earlier than 0x50, the graphics surface for the video
	 * source must have a pixel format of either BPXL_eY18_Cr8_Y08_Cb8,
	 * BPXL_eCr8_Y18_Cb8_Y08, BPXL_eY18_Cb8_Y08_Cr8, or BPXL_eCb8_Y18_Cr8_Y08;
	 * otherwise pink and green video will result. Cores with minor version
	 * 0x60 and 0x50 (for MFD and VFD respectively) and later can support all
	 * formats. Refer to the description of BPXL_Format for details
	 * on the byte location with each pixel format.
	 *
	 * TO BE OBSOLETE: Please use ePxlFmt instead. */
	BSUR_Surface_Handle      hSurface;

	/* Pixel format of given decoded surface.
	 *   ePxlFmt == BPXL_eCr8_Y18_Cb8_Y08
	 *   ePxlFmt == BPXL_eY18_Cr8_Y08_Cb8
	 *   ePxlFmt == BPXL_eY08_Cb8_Y18_Cr8
	 *   ePxlFmt == BPXL_eCb8_Y08_Cr8_Y18

	 *   ePxlFmt == BPXL_eCb8_Y18_Cr8_Y08
	 *   ePxlFmt == BPXL_eY18_Cb8_Y08_Cr8
	 *   ePxlFmt == BPXL_eY08_Cr8_Y18_Cb8
	 *   ePxlFmt == BPXL_eCr8_Y08_Cb8_Y18

	 *  ePxlFmt == BPXL_INVALID be treated as default XVD 420 decoder format. */
	BPXL_Format              ePxlFmt;

	/*----------------------------------------------------------------------*/
	/* MVD and XVD only */

	/* This field gives the Progressive Sequence bit in the stream for MPEG
	 * streams that contain the progressive_sequence bit in the sequence
	 * header. For other protocols, the Display Manager (XVD) may choose to
	 * derive this bit from other parameters. Hence, the field is not
	 * dependable, e.g., in AVC, there is no concept of "prog_seq". It also
	 * can happen that streams marked interlaced are full of Progressive frames.
	 * This is informational only. VDC does not use this flag. */
	bool                     bStreamProgressive;

	/* This field gives the Progressive Frame bit in the stream.
	 * Progressive Frame bit arrives in the Picture header of MPEG
	 * stream and hence may change every frame. For other protocols,
	 * like AVC, it is derived from other coding tools like ct_type
	 * in picture_timing SEI and frame_mbs_only_flag.  This is informational
	 * only VDC does not use this flag. */
	bool                     bFrameProgressive;

	/* Chroma location type. Still use the variable name eMpegType for
	 * backward compatibility. This information is used by the MFD (Feeder)
	 * to decide where chroma position is related to luma position. Protocols
	 * supported include MPEG1, MPEG2, and AVC. */
	BAVC_ChromaLocation      eMpegType;

	/* YCbCr type: 4:2:0, 4:2:2 or 4:4:4. */
	BAVC_YCbCrType           eYCbCrType;

	/* Chrominance interpolation mode. */
	BAVC_InterpolationMode   eChrominanceInterpolationMode;

	/* color primaries */
	BAVC_ColorPrimaries      eColorPrimaries;

	/* transfer characteristics */
	BAVC_TransferCharacteristics eTransferCharacteristics;

	/* The actual horizontal size of the provided buffers (post-clipped). */
	uint32_t                 ulSourceHorizontalSize;

	/* The actual vertical size of the provided buffers (post-clipped). */
	uint32_t                 ulSourceVerticalSize;

	/* Luminance frame buffer virtual base address (original buffer address).
	 * Or surface address of surface when (ePxlFmt == 422 foramts)*/
	void                    *pLuminanceFrameBufferAddress;

	/* Chrominance frame buffer virtual base address (original buffer address). */
	void                    *pChrominanceFrameBufferAddress;

	/* Adjusted Quantization Parameter to be used for Digital Noise Reduction;
	 * default should be zero; */
	uint32_t                 ulAdjQp;

	/* If scanning out a top field, repeat is set if the previous top field is the same
	 * If scanning out a bottom field, repeat is set if the previous bottom field is the same
	 * If scanning out a progressive frame, repeat is set if the previous progressive frame is the same */
	bool                     bRepeatField;

	/* slow->fast(e.g. 50->60) or trick mode repeat;
	 * it should be set for any true repeat (the source polarity and frame
	 * buffer are the same as the previous picture). It helps VDC/MAD/Multi-buffer
	 * to display better. */
	bool                     bPictureRepeatFlag;

	/* When decoder is in trick mode (fast/pause/slow motion) and its scanout
	 * cadence is interlaced, notify VDC to ignore cadence match. The result
	 * will be  half of fields would be field-inverted by SCL and no more
	 * regular multi-buffer skip/repeat bouncing. When VDC sees the flag be set,
	 * its capture and playback cadence should ignore source cadence match so
	 * the sync-slipped multi-buffer reader/writer pointers would proceed normally
	 * without extra field repeat/skip; otherwise, VDC cadence match is done by
	 * best effort. */
	bool                     bIgnoreCadenceMatch;

	/*---------------------------------------------------------------------
	 * Mosaic Mode:
	 * ------------
	 * Mosaic mode is an option where an existing single decode/display path
	 * is being re-used to support multiple smaller decode/display operations
	 * at full frame-rate.
	 * For each active channel there will be one and only one entry added to
	 * a linked list. The linked list has the following properties:
	 *
	 * Each element in the linked list is a BAVC_XVD_Picture structure. The
	 * element 'pNext' was added to this existing structure to make it into
	 * a linked list. This is the same structure used for the single decode
	 * case so the single decode comes across as a 1-entry linked list.
	 *
	 * If a channel is not open, there will be no entry in the list and the
	 * VDC callback will not be made. If a channel is open (even if
	 * StartDecode? has not been called), there will always be an element on
	 * this list corresponding to this channel and the VDC callback will be
	 * made. Even if there are no buffers to decode, the list element will
	 * exist but bMute will be set to true. There will be no padding of
	 * additional linked list elements to make up for inactive channels. In
	 * other words, if you only have channels 3, 5, and 7 open, the XVD will
	 * not pass muted structures for channel 1, 2, 4, 6, or 8. Instead the
	 * linked list will only have elements for 3, 5, and 7.
	 *
	 * Elements in the list will be sorted in ascending order according to
	 * channel number. There will only be one element for each channel number.
	 */

	/* linked list of mosaic pictures; the last element of the list should
	 * set it to NULL to terminate; */
	void                    *pNext;

	/* channel index for mosaic mode linked list; the index is per decoder
	 * instance based, in other words, decoder 0 has active channels 1, 2, 3,
	 * while decoder 1 could also have different channels 1, 2, 3, which are
	 * independent of decoder 0's channels; */
	uint32_t                 ulChannelId;

	/*----------------------------------------------------------------------*/
	/* MVD or (ePxlFmt == 422 foramts) */

	/* The stride of the luminance and chrominance buffers in bytes measured
	 * from the start of a line's macroblock to the start of the next line's
	 * macroblock.
	 *
	 * Or surface stride of surface when (ePxlFmt == 422 foramts)*/
	uint32_t                 ulRowStride;

	/*----------------------------------------------------------------------*/
	/* XVD only */
	/* This field specifies that the feeder needs to capture the CRC of this
	 * frame, and send it back to the application if callback were registered.
	 * This is for feeder with the capability of doing CRC when feeding.  In
	 * addition if this flag is set by the deocode it also indication that
	 * correct display is not honor, and we may get incorrect display on the
	 * backend. */
	bool                     bCaptureCrc;

	/* Number of macroblocks size in the vertical direction for Luminance
	 * array of the picture to be displayed. */
	uint32_t                 ulLuminanceNMBY;

	/* Number of macroblocks size in the vertical direction for Chrominance
	 * array of the picture to be displayed. */
	uint32_t                 ulChrominanceNMBY;

	/* Stripe width, can be 64 byte or 128 byte */
	BAVC_StripeWidth         eStripeWidth;

	/* Valid when  eAspectRatio == eSAR. */
	uint16_t                 uiSampleAspectRatioX;
	uint16_t                 uiSampleAspectRatioY;

	/* PR 17811 idr_pic_id and PicOrderCnt values passed to VDC during CRC mode.
	 * It is set to Zero otherwise.  The range of int32_PicOrderCnt from
	 * decoder can in the full range of int32_t.  While ulIdrPicID is an
	 * uint32_t, but only 16 bits of unsinged value is used. */
	uint32_t                 ulIdrPicID;
	int32_t                  int32_PicOrderCnt;

	/* PR20042: Need the top/left coordinate of the clipping values from decoder
	 * to calculate new MSTART for MFD.  Unit is in pixel. */
	uint32_t                 ulSourceClipTop;
	uint32_t                 ulSourceClipLeft;

	/* VC1 range re-mapping setting:
	   These values are used for VC1 decode display; */
	uint32_t                 ulLumaRangeRemapping;
	uint32_t                 ulChromaRangeRemapping;

	/* H.264 FGT Info */
	BMEM_Handle              hFgtHeap; /* Heap from which FGT data structure is allocated */
	void                    *pFgtSeiBufferAddress; /* Pointer to the FGT SEI structure */

	/* This field indicate if valid AFD present present in the stream or not.
	 * if bValidAfd=true, then ulAfd can then be used. */
	bool                     bValidAfd;

	/* AFD values defined by User data registered by ITU-T Recommendation T.35
	 * SEI message (see ISO/IEC 14496-10:2005, Subclauses D.8.5 and D.9.5).
	 * The AFD value describe the area of interest of the stream. */
	uint32_t                 ulAfd;

	/* specify top/bottom or left/right data */
	BAVC_BarDataType         eBarDataType;

	/* either the top or left bar data value, as defined above */
	uint32_t                 ulTopLeftBarValue;

	/* either the bottom or right bar data value, as defined above */
	uint32_t                 ulBotRightBarValue;

	/* 3D orientation */
	BFMT_Orientation         eOrientation;

	/* Original PTS of the picture, relayed by STG to ViCE2 Mailbox.
	 * Note, this PTS value could be either original coded PTS or interolated
	 * PTS by DM. */
	uint32_t                 ulOrigPTS;

	/* Original input Picture type, relayed by STG to ViCE2 Mailbox.
	 * Decoder FW extracts the value and passes to DM via PPB structure.
	 * source: bxvd_vdec_info.h  structure BXVD_P_PPB */
	BAVC_PictureCoding       ePictureType;

	/* Should be set to "true" for all pictures sent to VDC before the first
	 * decoded picture is available after a channel is stopped (for both muted
	 * or repeated hold last pic frames).  See also SW7425-2239and SW7425-2253. */
	bool                     bChannelChange;

	/* Decoded picture id for userdata transcode */
	uint32_t                 ulDecodePictureId;

	/* Fast non-real-time (FNRT) support. 10-bit chunk index to indicate which
	 * chunk the picture belongs to. It is also meta data relay to ViCE by VDC */
	uint32_t                 ulChunkId;

	/* Pointer to BAVC_MFD_Picture specify pictures that must be processed at
	 * the same time to form a complete display */
	void                    *pEnhanced;

} BAVC_MFD_Picture;

typedef BAVC_MFD_Picture BAVC_XVD_Picture;   /* AVC decoder */
typedef BAVC_MFD_Picture BAVC_MVD_Picture;   /* MiniTitan decoder */

/* Backward compatibility, for legacy code. */
typedef BAVC_MFD_Picture BAVC_MVD_Field;      /* MiniTitan decoder */


/***************************************************************************
Summary:
    Contains the picture info for the HDDVI input port.

Description:
    This structure is used to pass data from the XVD module to the
    VDC module, through a call-back. Fields that are unused, simply
    means XVD doesn't have the data, and VDC doesn't expect any.
    Naming convention used: BAVC_To_Source_Name

See Also:
    BVDC_Source_PictureCallback_isr, BVDC_Source_InstallPictureCallback
****************************************************************************/
typedef struct BAVC_VDC_HdDvi_Picture
{
	/* Determines if the field is to be muted. This is set when all other
	   values in this structure are invalid and undefined. This flag
	   notifies the VDC that the feeder should be programmed to feed a
	   fixed constant color instead of using the supplied buffers. */
	bool                     bMute;

	/* This field gives the Progressive Sequence bit in the stream.
	   Progressive Sequence bit comes in the Sequence header of a
	   stream and does not change every frame. It also can happen
	   that streams marked interlaced are full of Progressive frames
	   This is _only_ for information.
	*/
	bool                     bStreamProgressive;

	/* XVD tells VDC to detect sync pulses */
	bool                     bResetHdDviBegin;
	bool                     bResetHdDviEnd;

	/* XVD tells VDC to change new hddvi format, if bNeedSync is true */
	/* TODO: Fix this to BFMT_VideoFmt later */
	uint32_t                 eHddviFormat;

	/* Difference between STC and PTS of a given frame at Vsync time */
	long                     lPtsStcOffset;

	/* Source polarity!  To be field by VDC!  Taken from slot interrupt.
	 * HDR or XVD does not need to initialized this field. */
	BAVC_Polarity            eSourcePolarity;

	/* Units of 1/16th of a pixel in two's complement. Bit 15 is the
	 * sign bit. Bits 14:8 are the macro block grid.Bits 7:4 are the
	 * pixel grid. Bits 3:0 are 1/16 pixel grid. */
	int32_t                  i32_HorizontalPanScan;

	/* Units of 1/16th of a pixel.in two's complement. Bit 15 is the
	 * sign bit. Bits 14:8 are the macro block grid. Bits 7:4 are the
	 * pixel grid. Bits 3:0 are 1/16 pixel grid. */
	int32_t                  i32_VerticalPanScan;

	/* Color space type: RGB, YCbCr444 and YCbCr422 */
	BAVC_Colorspace          eColorSpace;

	/* RGB Quantization */
	BAVC_CscMode             eCscMode;

	/* YCbCr type. */
	BAVC_YCbCrType           eYCbCrType;

	/* color primaries */
	BAVC_ColorPrimaries      eColorPrimaries;

	/* transfer characteristics */
	BAVC_TransferCharacteristics eTransferCharacteristics;

	/* Matrix coefficients. */
	BAVC_MatrixCoefficients  eMatrixCoefficients;

	/* Aspect ratio of the rectangle defined by the display
	 * parameters (ulDisplayHorizontalSize and ulDisplayVerticalSize) */
	BFMT_AspectRatio         eAspectRatio;

	/* Sample aspect ratio for AVC */
	uint16_t                 uiSampleAspectRatioX;
	uint16_t                 uiSampleAspectRatioY;

	/* Frame rate code. */
	BAVC_FrameRateCode       eFrameRateCode;

	/* When pan-scan is enabled, this is the horizontal size of the
	 * displayed portion of the buffer. */
	uint32_t                 ulDisplayHorizontalSize;

	/* When pan-scan is enabled, this is the vertical size of the
	 * displayed portion of the buffer. */
	uint32_t                 ulDisplayVerticalSize;

	/* The actual horizontal size of the provided buffers. */
	uint32_t                 ulSourceHorizontalSize;

	/* The actual vertical size of the provided buffers. */
	uint32_t                 ulSourceVerticalSize;

	/* For HDMI Mode, the contents of AVI Infoframe should be used to set
	 * the value of the pixel decimation circuit rather than have it calculated */
	bool                     bHdmiMode;
	uint32_t                 ulPixelRepitionFactor;

} BAVC_VDC_HdDvi_Picture;

/***************************************************************************
Summary:
    A structure representing a graphics frame.

Description:
    This structure is used to pass a graphics frame to BVDC for display, from
    BGRC or BP3D.

    If the optional alpha surface is not needed, hAlphaSurface should be set
    to NULL.

See Also:
    BVDC_Source_PictureCallback_isr, BVDC_Source_InstallPictureCallback
    BVDC_Source_SetSurface, BVDC_Source_SetAlphaSurface
****************************************************************************/
typedef struct BAVC_Gfx_Picture
{
	BSUR_Surface_Handle  hSurface;       /* main surface */
	BSUR_Surface_Handle  hRSurface;      /* main R surface */
	BSUR_Surface_Handle  hAlphaSurface;  /* optional separate alpha surface */
	uint8_t  ucW0Alpha; /* Used as pixel alpha when the pixel's W is 0. */
	uint8_t  ucW1Alpha; /* Used as pixel alpha when the pixel's W is 1. */

	/* 3D orientation */
	BFMT_Orientation     eInOrientation;

	/* Original PTS of the picture, relayed by STG to ViCE2 Mailbox. */
	uint32_t             ulOrigPTS;

} BAVC_Gfx_Picture;


/***************************************************************************
Summary:
    A structure contains information about the current display state, such
    as current refresh rate, pixel clock rate, and timebase used.

Description:
    This structure is used to pass a information about a display information
    to application that needs to know about the display information.   For
    example HDMI module needs to know the pixel clock rate, so it can match
    its output pixel clock rate as well.

See Also:
    BVDC_CallbackFunc_isr,
    BVDC_Display_InstallRateChangeCallback,
    BVDC_Window_SetMasterFrameRate.
****************************************************************************/
typedef struct BAVC_VdcDisplay_Info
{
	uint32_t                 ulVertRefreshRate;  /* defines in bfmt.h (Hz) */
	uint32_t                 ulPixelClkRate;     /* defines in bfmt.h (Mhz) */

} BAVC_VdcDisplay_Info;

/***************************************************************************
Summary:
    Defines audio sampling rate

Description:
    This enum defines audio sampling rate.

See Also:
****************************************************************************/
typedef enum BAVC_AudioSamplingRate
{
	BAVC_AudioSamplingRate_e32k=0,    /* 32K Sample rate */
	BAVC_AudioSamplingRate_e44_1k,    /* 44.1K Sample rate */
	BAVC_AudioSamplingRate_e48k,      /* 48K Sample rate */
	BAVC_AudioSamplingRate_e96k,      /* 96K Sample rate */
	BAVC_AudioSamplingRate_e16k,      /* 16K Sample rate */
	BAVC_AudioSamplingRate_e22_05k,   /* 22.05K Sample rate */
	BAVC_AudioSamplingRate_e24k,      /* 24K Sample rate */
	BAVC_AudioSamplingRate_e64k,      /* 64K Sample rate */
	BAVC_AudioSamplingRate_e88_2k,    /* 88.2K Sample rate */
	BAVC_AudioSamplingRate_e128k,     /* 12896K Sample rate */
	BAVC_AudioSamplingRate_e176_4k,   /* 176.4K Sample rate */
	BAVC_AudioSamplingRate_e192k,     /* 19296K Sample rate */
	BAVC_AudioSamplingRate_e8k,       /* 8K Sample rate */
	BAVC_AudioSamplingRate_e12k,      /* 12K Sample rate */
	BAVC_AudioSamplingRate_e11_025k,  /* 11.025K Sample rate */

	BAVC_AudioSamplingRate_eUnknown   /* Unknown sample rate */

}BAVC_AudioSamplingRate;


/***************************************************************************
Summary:
    Defines supported number of Audio bits

Description:
    This enum is used to query HDMI monitors for supported audio bits

See Also:
****************************************************************************/
typedef enum BAVC_AudioBits
{
	BAVC_AudioBits_e16=0,
	BAVC_AudioBits_e20,
	BAVC_AudioBits_e24

} BAVC_AudioBits ;


/***************************************************************************
Summary:
    Defines supported Audio Formats

Description:
    This enum is used to query HDMI monitors for supported Audio Formats

See Also:
****************************************************************************/
typedef enum BAVC_AudioFormat
{
	BAVC_AudioFormat_ePCM=0,
	BAVC_AudioFormat_eAC3,
	BAVC_AudioFormat_eMPEG1,
	BAVC_AudioFormat_eMP3,
	BAVC_AudioFormat_eMPEG2,
	BAVC_AudioFormat_eAAC,
	BAVC_AudioFormat_eDTS,
	BAVC_AudioFormat_eAVS,
	BAVC_AudioFormat_eATRAC,
	BAVC_AudioFormat_eOneBit,
	BAVC_AudioFormat_eDDPlus,
	BAVC_AudioFormat_eDTSHD,
	BAVC_AudioFormat_eMATMLP,
	BAVC_AudioFormat_eDST,
	BAVC_AudioFormat_eWMAPro,

	BAVC_AudioFormat_eMaxCount

} BAVC_AudioFormat ;

/***************************************************************************
Summary:
    A structure contains information about the current audio state, such
    as Sample Rate, etc.

Description:
    This structure is used to pass a information about audio to the
    application that needs to know about the audio.  For example, the HDMI
    module needs to know the Audio Sample Rate, so it can properly adjust
    its audio parameters for the current rate.

See Also:
    BAUD_EnableSampleRateChangeCb
****************************************************************************/
typedef struct BAVC_Audio_Info
{
	BAVC_AudioSamplingRate   eAudioSamplingRate ;

} BAVC_Audio_Info ;


/***************************************************************************
Summary:
    The User Data Type

Description:
    The User data can come in the Sequence Header, the GOP Header
    or the Picture Header. This type is enumerated here.

See Also:

****************************************************************************/
typedef enum BAVC_USERDATA_Type
{
	BAVC_USERDATA_Type_eSeq=1,       /* User data coming in the Sequence Header */
	BAVC_USERDATA_Type_eGOP,         /* User data coming in the Gop Header */
	BAVC_USERDATA_Type_ePicture,     /* User Data coming in Picture Header */
	BAVC_USERDATA_Type_eSEI,         /* User Data in H.264 */
	BAVC_USERDATA_Type_eEntryPoint, /* VC1 user data coming in entry point layer */
	BAVC_USERDATA_Type_eField,       /* VC1 user data coming in picture field layer */
	BAVC_USERDATA_Type_eFrame,       /* VC1 user data coming in picture frame layer */
	BAVC_USERDATA_Type_eSlice        /* VC1 user data coming in slice layer */

} BAVC_USERDATA_Type;




/***************************************************************************
Summary:
    User Data Info structure common to MVD (Mini-Titan) and XVD (AVC decoder).

Description:
    Interface structure between application and microcode. Microcode
    initializes the fields of this structure to inform application
    about the current status of user data.

See Also:

****************************************************************************/
typedef struct BAVC_USERDATA_info
{
	void            *pUserDataBuffer;   /* Pointer to the pre-allocated buffer */
	size_t          ui32UserDataBufSize;    /* Total buffer size */
	BAVC_USERDATA_Type eUserDataType ;  /* Type of the User Data */
	bool            bTopFieldFirst ;    /* MPEG Syntax flag TopFieldFirst */
	bool            bRepeatFirstField ; /* MPEG Syntax flag RepeatFirstField */
	BAVC_PictureCoding ePicCodingType ; /* I/P/B Picture Coding Type */
	uint32_t        ui32PTS ;           /* PTS value of the associated frame */
	bool            bPTSValid ;         /* PTS value of the associated frame */
	bool            bErrorBufferOverflow ;  /* To indicate buffer overflow */
	BAVC_Polarity   eSourcePolarity ;   /* Source Polarity if data found in
	                                       Picture Header */
	uint32_t        ui32PicCodExt[2] ;  /* Array of 2 32 bit variable for
	                                        storing Picture Coding Extension
	                                        All bits from the MPEG stream are
	                                        stored as they come in the earliest
	                                        going to the highest bit in the word
	                                        In word-0 bits 31-30 are unused
	                                        and in word-1 bits 31-20 are unused */
	uint32_t        ulDecodePictureId;  /* Decoded picture id for userdata transcode */

} BAVC_USERDATA_info;

/***************************************************************************
Summary:
    Enum for specifying the PTS type.

Description:
    This can be used by the SerialPCRLib to decide whether the PTS can
    be used to jam the STC or not. For example, a coded PTS might be given
    preference to a interpolated PTS.

See Also:

****************************************************************************/
typedef enum BAVC_PTSType
{
	BAVC_PTSType_eCoded,
	BAVC_PTSType_eInterpolatedFromValidPTS,
	BAVC_PTSType_eHostProgrammedPTS,
	BAVC_PTSType_eInterpolatedFromInvalidPTS

} BAVC_PTSType;

/***************************************************************************
Summary:
    This data structure is passed with the callbacks associated with
    the FirsPTSReady and FirsTSMPassed interrupts. The XXX_GetPTS
    funxtion also might use this structure.

Description:
    This structure consists of the 32 bit PTS value itself and a tag
    associated with it that tells whether this is a coded PTS, an
    Interpolated PTS from a previous Coded PTS, or PTS interpoleted from
    a junk PTS value.

****************************************************************************/
typedef struct BAVC_PTSInfo
{
	uint32_t        ui32CurrentPTS; /* Current PTS value */
	BAVC_PTSType    ePTSType;           /* The PTS type tag */

	/* needed for PCRLib */
	uint32_t uiDecodedFrameCount;
	uint32_t uiDroppedFrameCount;

} BAVC_PTSInfo;

/***************************************************************************
Summary:
    Defines Video compression standards

Description:
    This enum is used to set the video compression standard use in XVD PI.

See Also:
****************************************************************************/
typedef enum BAVC_VideoCompressionStd
{
	BAVC_VideoCompressionStd_eH264,           /* H.264 */
	BAVC_VideoCompressionStd_eMPEG2,          /* MPEG-2 */
	BAVC_VideoCompressionStd_eH261,           /* H.261 */
	BAVC_VideoCompressionStd_eH263,           /* H.263 */
	BAVC_VideoCompressionStd_eVC1,            /* VC1 Advanced profile */
	BAVC_VideoCompressionStd_eMPEG1,          /* MPEG-1 */
	BAVC_VideoCompressionStd_eMPEG2DTV,       /* MPEG-2 DirecTV DSS ES */
	BAVC_VideoCompressionStd_eVC1SimpleMain,  /* VC1 Simple & Main profile */
	BAVC_VideoCompressionStd_eMPEG4Part2,     /* MPEG 4, Part 2. */
	BAVC_VideoCompressionStd_eAVS,            /* AVS Jinzhun profile. */
	BAVC_VideoCompressionStd_eMPEG2_DSS_PES,  /* MPEG-2 DirecTV DSS PES */
	BAVC_VideoCompressionStd_eSVC,            /* Scalable Video Codec */
	BAVC_VideoCompressionStd_eSVC_BL,         /* Scalable Video Codec Base Layer */
	BAVC_VideoCompressionStd_eMVC,            /* MVC Multi View Coding */
	BAVC_VideoCompressionStd_eVP6,            /* VP6 */
	BAVC_VideoCompressionStd_eVP7,            /* VP7 */
	BAVC_VideoCompressionStd_eVP8,            /* VP8 */
	BAVC_VideoCompressionStd_eRV9,            /* Real Video 9 */
	BAVC_VideoCompressionStd_eSPARK,          /* Sorenson Spark */
	BAVC_VideoCompressionStd_eMOTION_JPEG,    /* Motion Jpeg */
	BAVC_VideoCompressionStd_eH265,           /* H.265 */
	BAVC_VideoCompressionStd_eMax

} BAVC_VideoCompressionStd;

/***************************************************************************
Summary:
    Defines Video compression profiles

Description:
    This enum is used to report the video compression profile by XVD PI.
    Also used to specify the video compression profile to VCE PI

    This enum is defined by AVD FW team

See Also:
****************************************************************************/
typedef enum BAVC_VideoCompressionProfile
{
	BAVC_VideoCompressionProfile_eUnknown = 0,
	BAVC_VideoCompressionProfile_eSimple,
	BAVC_VideoCompressionProfile_eMain,
	BAVC_VideoCompressionProfile_eHigh,
	BAVC_VideoCompressionProfile_eAdvanced,
	BAVC_VideoCompressionProfile_eJizhun,
	BAVC_VideoCompressionProfile_eSnrScalable,
	BAVC_VideoCompressionProfile_eSpatiallyScalable,
	BAVC_VideoCompressionProfile_eAdvancedSimple,
	BAVC_VideoCompressionProfile_eBaseline,

	BAVC_VideoCompressionProfile_eMax

} BAVC_VideoCompressionProfile;

/***************************************************************************
Summary:
    Defines Video compression levels

Description:
    This enum is used to report the video compression level by XVD PI.
    Also used to specify the video compression level to VCE PI

    This enum is defined by AVD FW team

See Also:
****************************************************************************/
typedef enum BAVC_VideoCompressionLevel
{
	BAVC_VideoCompressionLevel_eUnknown = 0,
	BAVC_VideoCompressionLevel_e00,
	BAVC_VideoCompressionLevel_e10,
	BAVC_VideoCompressionLevel_e1B,
	BAVC_VideoCompressionLevel_e11,
	BAVC_VideoCompressionLevel_e12,
	BAVC_VideoCompressionLevel_e13,
	BAVC_VideoCompressionLevel_e20,
	BAVC_VideoCompressionLevel_e21,
	BAVC_VideoCompressionLevel_e22,
	BAVC_VideoCompressionLevel_e30,
	BAVC_VideoCompressionLevel_e31,
	BAVC_VideoCompressionLevel_e32,
	BAVC_VideoCompressionLevel_e40,
	BAVC_VideoCompressionLevel_e41,
	BAVC_VideoCompressionLevel_e42,
	BAVC_VideoCompressionLevel_e50,
	BAVC_VideoCompressionLevel_e51,
	BAVC_VideoCompressionLevel_e60,
	BAVC_VideoCompressionLevel_e62,
	BAVC_VideoCompressionLevel_eLow,
	BAVC_VideoCompressionLevel_eMain,
	BAVC_VideoCompressionLevel_eHigh,
	BAVC_VideoCompressionLevel_eHigh1440,

	BAVC_VideoCompressionLevel_eMax

} BAVC_VideoCompressionLevel;

/* vce related common defines */
#include "bavc_vce.h"

/* rap related common defines */
#include "bavc_rap.h"


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BAVC_H__ */

/* end of file */
