/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bcmnexus_gfx.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/22 $
 *
 * $brcm_Date: 3/7/12 5:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/gfxdrivers/bcmnexus/bcmnexus_gfx.c $
 *
 * DirectFB_1_4_15_Port/22   3/7/12 5:25p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/21   2/29/12 12:18p robertwm
 * SW7420-2268: DirectFB-1.4.15: Require pre-allocated memory for
 *  intermediate surface buffer used in 2 pass d'scale.
 *
 * DirectFB_1_4_15_Port/20   2/6/12 4:24p scchang
 * SW7420-2229: DirectFB: failure of handling blitter downscaling limits
 *
 * DirectFB_1_4_15_Port/19   1/10/12 12:34p christ
 * SW7231-580: DirectFB: Graphics card version checking / code cleanup
 *
 * DirectFB_1_4_15_Port/18   1/6/12 2:42p robertwm
 * SW7420-2210: DirectFB: Need to support SMF_SOURCE2 flag in graphics
 *  driver and core code.
 *
 * DirectFB_1_4_15_Port/17   12/15/11 10:33a scchang
 * SW7425-1963: DirectFB: Hardware dithering support
 *
 * DirectFB_1_4_15_Port/16   12/12/11 3:54p robertwm
 * SW7420-2191: DirectFB: Stretch-blit tests are failing with Phase 7.0
 *  refSW on BCM97420C
 *
 * DirectFB_1_4_15_Port/15   12/7/11 12:11p apandav
 * SW7420-2179: DirectFB: Change default M2MC filtering to anisotropic
 *  when doing stretched blits
 *
 * DirectFB_1_4_15_Port/14   12/6/11 4:38p apandav
 * SW7420-2179: DirectFB: Change default M2MC filtering to anisotropic
 *  when doing stretched blits
 *
 * DirectFB_1_4_15_Port/13   11/30/11 7:48p scchang
 * SW7420-2158: DirectFB: Consider setting pthread mutex robust attribute
 *
 * DirectFB_1_4_15_Port/12   11/11/11 3:37p christ
 * SW7420-2113: DirectFB: Print out whether packet-buffer is enabled or
 *  not during df_dok test.
 *
 * DirectFB_1_4_15_Port/11   11/8/11 9:29p scchang
 * SW7420-2123: DirectFB: memory corrupted when no packet submitted to
 *  M2MC, then call packet buffer write completed
 *
 * DirectFB_1_4_15_Port/10   10/28/11 4:47p christ
 * SW7420-2113: DirectFB: Print out whether packet-buffer is enabled or
 *  not during df_dok test
 *
 * DirectFB_1_4_15_Port/9   10/25/11 4:07p scchang
 * SW7420-2097: DirectFB: Add fixed scaling support to default when doing
 *  stretch blit.
 *
 * DirectFB_1_4_15_Port/8   10/21/11 12:21p scchang
 * SW7420-2097: DirectFB: Disable FixedScale temporarily to avoid deadlock
 *  when running Webkit
 *
 * DirectFB_1_4_15_Port/7   10/12/11 4:08p scchang
 * SW7420-2082: DirectFB: System crash when running df_dok in non-packet
 *  buffer mode
 *
 * DirectFB_1_4_15_Port/6   10/7/11 4:29p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.15 release.
 *
 * DirectFB_1_4_15_Port/5   10/5/11 12:11p robertwm
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 *
 * DirectFB_1_4_15_Port/4   9/29/11 2:10p christ
 * SW7425-1360: DirectFB: Merge in upstream DirectFB 1.4.15 changes.
 *
 * DirectFB_1_4_14_Port/3   8/19/11 5:04p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_14_Port/2   8/12/11 4:44p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:20a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/19   10/12/11 3:45p scchang
 * SW7420-2082: DirectFB: System crash when running df_dok in non-packet
 *  buffer mode
 *
 * DirectFB_1_4_5_Port/18   10/6/11 5:36p scchang
 * SW7420-2079: DirectFB: MRC errors during stretch blit df_dok tests
 *
 * DirectFB_1_4_5_Port/17   9/22/11 2:54p robertwm
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 *
 * DirectFB_1_4_5_Port/17   9/22/11 2:26p robertwm
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 *
 * DirectFB_1_4_5_Port/16   9/6/11 2:46p tross
 * SWBLURAY-26917:
 * Changed implicit casting to explicit to work around compiler bug.
 * Reverted dfb_convert_colorspace back to inline.
 * Corrected compiler warnings.
 *
 * DirectFB_1_4_5_Port/15   9/2/11 12:30p tross
 * Converted scale/phase calculations to fixed-point to remove
 * requirement for math library linking and eliminate linux
 * floating emulation slowdown on chips without FPU.
 *
 * DirectFB_1_4_5_Port/14   9/1/11 4:21p tross
 * Add fixed scaling to bcmnexus_gfxStretchBlit to correct
 * scaling inaccuracies leading to significant pixel misalignment.
 *
 * DirectFB_1_4_5_Port/13   8/16/11 10:04p tross
 * Disable BCC temporarily. It was causing problems with
 * subtitles not showing up. Will reenable when root cause
 * is understood.
 *
 * DirectFB_1_4_5_Port/13   8/16/11 10:03p tross
 * Disable BCC temporarily. It was causing problems with
 * subtitles not showing up. Will reenable when root cause
 * is understood.
 *
 * DirectFB_1_4_5_Port/12   8/15/11 12:47p robertwm
 * SW7420-2031: DirectFB-1.4.5: Segfault when running DFB applications
 *  from limited build in different NEXUS_MODE.
 *
 * DirectFB_1_4_5_Port/11   8/3/11 6:33p scchang
 * SW7405-5451: IDirectFBSurface::SetClip followed by
 *  IDirectFBSurface::StretchBlit fails to draw anything
 *
 * DirectFB_1_4_5_Port/10   8/2/11 2:39p scchang
 * SW7420-2018: System crash in packet buffer 2D graphics driver for
 *  multi-application mode
 *
 * DirectFB_1_4_5_Port/9   7/27/11 5:30p apandav
 * SWDTV-8107: DirectFB: Blit with Horizontal Flip fails in Non Packet
 *  buffer mode
 *
 * DirectFB_1_4_5_Port/8   7/26/11 9:27p scchang
 * SW7420-2003: support >8x downscaling
 *
 * DirectFB_1_4_5_Port/7   7/21/11 7:36p scchang
 * SW7420-2003: support >8x downscaling
 *
 * DirectFB_1_4_5_Port/6   7/7/11 4:37p scchang
 * SW7420-1977: Inconsistent DirectFB Blend Behavior
 *
 * DirectFB_1_4_5_Port/5   7/5/11 7:54p scchang
 * SW7420-1104: Image colors are not matched between SW and HW functions
 *
 * DirectFB_1_4_5_Port/4   6/27/11 2:46p robertwm
 * SW7405-5390: DirectFB: PX3D proxy code does not relinquish resources
 *  when app is closed.
 *
 * DirectFB_1_4_5_Port/3   6/24/11 3:13p christ
 * SW7420-1962: DirectFB: Improve debug for heap usage
 *
 * DirectFB_1_4_5_Port/2   6/16/11 4:56p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_5_Port/1   6/16/11 3:08p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/62   6/16/11 4:13p robertwm
 * SW7405-5373: DirectFB: Cannot build gfxdriver due to undefined
 *  references to BCC h/w.
 *
 * DirectFB_1_4_Port/61   6/16/11 3:40p scchang
 * SW7425-737: system is hung up when running multi-app mode with 2.6.37
 *  kernel
 *
 * DirectFB_1_4_Port/60   6/13/11 9:10p scchang
 * SW7425-612: DrawString() color not correct when preceded by
 *  DrawString() call with different color, when packet blit is enabled
 *
 * DirectFB_1_4_Port/59   6/10/11 5:46p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/8   5/6/11 9:48a tross
 * Correct BCC use.
 * Flush surfaces prior to calling BCC.
 * Add BCC fill support.
 * Correct qualification of using BCC for PG layer.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/7   4/29/11 1:51p tross
 * Correct deadlock caused by qualification for BCC blit.
 * Reintegrate use of BCC with packet functionality.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/6   4/7/11 8:03p tross
 * Merge from latest.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/5   4/5/11 2:40p tross
 * Correct shifting problems in colormatrix support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/4   3/30/11 4:33p tross
 * Corrected colorspace conversion in xxxSetSourceColorMatrix.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   3/28/11 11:00a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/11/11 3:34p tross
 * Added check to ensure source and destination surfaces are identical
 *  configuration.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:02p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/58   3/30/11 6:20p christ
 * SW7425-254: DirectFB: Improve performance by replacing linux-fusion
 *  calls with pthreads
 *
 * DirectFB_1_4_Port/57   3/29/11 7:11p scchang
 * SW7422-278: DirectFB: reset color matrix for RGB to YUV format
 *  conversion when fast blit to non-fast blit
 *
 * DirectFB_1_4_Port/56   3/24/11 9:46a robertwm
 * SW7405-5205: DirectFB: Need to clear graphics callbacks when closing
 *  down gfxdriver.
 *
 * DirectFB_1_4_Port/55   3/21/11 4:59p robertwm
 * SW7422-393: DirectFB: Hang in graphics driver due to serialisation of
 *  graphics packet-buffer.
 *
 * DirectFB_1_4_Port/54   3/17/11 12:44p robertwm
 * SW7422-385: DirectFB: Using Image Provider results in some images being
 *  displayed incorrectly.
 * SW7422-384: DirectFB: Multi-application packet-buffer graphics fails
 *  with assertion.
 *
 * DirectFB_1_4_Port/53   3/12/11 4:24p robertwm
 * SW7420-1641: DirectFB: transformation matrix takes no effect after
 *  setting RenderOptions to None.
 *
 * DirectFB_1_4_Port/52   3/11/11 8:16p scchang
 * SW7420-1641: DirectFB: transformation matrix takes no effect after
 *  setting RenderOptions to None
 *
 * DirectFB_1_4_Port/51   3/3/11 8:16p scchang
 * SW7420-1394: DirectFB: Add support for colorizing on YUY2 format
 *
 * DirectFB_1_4_Port/50   3/1/11 6:58p raywu
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs
 *
 * DirectFB_1_4_Port/49   3/1/11 2:55p raywu
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs
 *
 * DirectFB_1_4_Port/48   2/25/11 7:39p raywu
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs
 *
 * DirectFB_1_4_Port/47   2/25/11 6:40p scchang
 * SW7422-279: DirectFB: Add colorMatrix_RGBToYCbCr and
 *  colorMatrix_YCbCrToRGB declaration in packet and non-packet buffer
 *  modes
 *
 * DirectFB_1_4_Port/46   2/25/11 6:22p scchang
 * SW7422-279: DirectFB: Add support for color change with RGB to YUV
 *  format conversion
 *
 * DirectFB_1_4_Port/45   2/24/11 8:11p raywu
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs
 *
 * DirectFB_1_4_Port/44   2/24/11 9:09a raywu
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs
 *
 * DirectFB_1_4_Port/43   2/23/11 6:04p christ
 * SW7425-108: DirectFB: Insignia LUT8 tests failing on 7425 packet blit
 *  build.
 *
 * DirectFB_1_4_Port/42   2/22/11 5:15p raywu
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs
 *
 * DirectFB_1_4_Port/41   2/21/11 3:03p christ
 * SW7425-108: DirectFB: Insignia LUT8 tests failing on 7425 packet blit
 *  build.
 *
 * DirectFB_1_4_Port/40   2/20/11 7:29p robertwm
 * SW7420-1525: DirectFB: Cannot build DirectFB with packet-buffer support
 *  on Phase 5.0.
 *
 * DirectFB_1_4_Port/39   2/18/11 7:45p robertwm
 * SW7422-279: DirectFB: Cannot see apps when pixelformat is YUY2 or UYVY.
 *
 * DirectFB_1_4_Port/38   2/17/11 10:50p robertwm
 * SW7420-1510: DirectFB: Reduce number of IPC calls for gfx operations
 *  using packet-blit interface.
 *
 * DirectFB_1_4_Port/37   2/11/11 6:22p scchang
 * SW7420-1486: fix SetMatrix() function for translation issue
 *
 * DirectFB_1_4_Port/36   2/10/11 5:31p christ
 * SW7420-1479: DirectFB: Correct Nexus 2d graphics filter coefficients
 *  usage in gfx driver.
 *
 * DirectFB_1_4_Port/35   2/8/11 4:56p robertwm
 * SW7420-1470: DirectFB: Ensure width/height/pitch of PX3D texture
 *  surfaces is a power of 2.
 *
 * DirectFB_1_4_Port/34   1/25/11 12:39p christ
 * SW7420-1411: DirectFB: Merge of features required for DirectFB 1.4.5
 *  phase 1.5 release - fix automerge compile error.
 *
 * DirectFB_1_4_Port/33   1/25/11 12:11p christ
 *  SW7420-1411: DirectFB: Merge of features required for DirectFB 1.4.5
 *  phase 1.5 release - Merge in packet blit driver.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/27   1/14/11 2:03p raywu
 * SW7420-1384: DirectFB: Cannot run stretch-blit df_dok tests with 480i
 *  resolution with packet-blit gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/26   1/10/11 7:31p robertwm
 * SW7420-1373: DirectFB: Destination surface not setup during
 *  NEXUS_Graphics2D_Blit on packet-blit branch.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/25   1/5/11 11:27a robertwm
 * SW7420-1346: DirectFB: Added dual source batch blit supported.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/24   1/4/11 4:24p raywu
 * SW7420-1346: DirectFB: Added dual source batch blit supported
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/23   1/4/11 2:28p scchang
 * SW7420-1338: Fixed compiled error when not building PX3D support
 *  enabled
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/22   1/3/11 4:43p scchang
 * SW7420-1338: Add A8 and other pixel formats support for FillTrapezoid()
 *  for M2MC core
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/21   12/27/10 2:30p scchang
 * SW7420-1335: Add support for overlapping blit on same surface
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/20   12/15/10 11:35a robertwm
 * SW7420-1317: DirectFB: Missing dst_buffer in
 *  bcmnexus_gfxSetStateDraw3D.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/19   12/10/10 10:56a robertwm
 * SW7420-1309: DirectFB: Fix S/T co-ordinates for TextureTriangles using
 *  PX3D
 * SW7420-1310: DirectFB: Ensure that the maximum surface size for PX3D is
 *  2048 and is a power of 2
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/18   12/8/10 6:11p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/17   12/3/10 8:37p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/16   11/25/10 8:36p raywu
 * SW7420-1134: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/15   11/19/10 6:33p raywu
 * SW7420-1251: Add Directfb packet buffer, multi-app, Kernel mode
 *  supported.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/14   11/19/10 5:49p raywu
 * SW7420-1251: Add Directfb packet buffer, multi-app, Kernel mode
 *  supported
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/13   11/17/10 9:40p raywu
 * SW7420-1134: DirectFB: Add packet buffer (blit) support for gfxdriver;
 *  Multi-App Kernel mode Supported
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/12   11/15/10 3:13p robertwm
 * SW7420-1241: DirectFB: Add support for FillTrapezoid() to the driver
 *  and core.
 * SW7420-1242: DirectFB: Add support for SetMatrix() function for both
 *  M2MC and PX3D cores.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/11   11/9/10 6:37p robertwm
 * SW7420-1236: DirectFB: Colorkeying with packet-buffer not always
 *  working.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/10   11/3/10 7:43p raywu
 * SW7420-1134 : DirectFB: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/9   11/2/10 7:29p raywu
 * SW7420-1134: DirectFB: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/8   10/29/10 6:49p raywu
 * SW7420-1134: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/7   10/28/10 8:13p scchang
 * SW7420-1134: Fix the drawing icon issue on web browser
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/6   10/27/10 5:44p robertwm
 * SW7420-1214: DirectFB: Add support for "EmitCommands" to our packet-
 *  blit version of the gfxdriver.
 *
 * DirectFB_1_4_Port/32   10/21/10 4:56p raywu
 * SW7420-1154: Hybrid mode graphic driver for stagecraft 1.2 & 2.0
 *
 * DirectFB_1_4_Port/31   10/20/10 8:07p raywu
 * SW7420-1154: Hybrid mode graphic driver for stagecraft 1.2 & 2.0
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/5   10/19/10 10:16p scchang
 * SW7420-1134: Fix some artifact issues when running packet blit with
 *  browser
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/4   10/12/10 6:31p robertwm
 * SW7420-1162: DirectFB: packet-blit gfxdriver does not compile with non-
 *  pb Nexus.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/3   10/12/10 8:58p raywu
 * SW7420-1134: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/2   10/8/10 12:43p raywu
 * SW7420-1134: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/1   10/4/10 8:15p raywu
 * SW7420-1134: DirectFB: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/32   10/21/10 4:56p raywu
 * SW7420-1154: Hybrid mode graphic driver for stagecraft 1.2 & 2.0
 *
 * DirectFB_1_4_Port/31   10/20/10 8:07p raywu
 * SW7420-1154: Hybrid mode graphic driver for stagecraft 1.2 & 2.0
 *
 * DirectFB_1_4_Port/30   9/23/10 8:00a robertwm
 * SW7405-4895: DirectFB: Improve tolerance of certain blitting flag
 *  combinations between h/w and s/w.
 * SW7405-4894: DirectFB: Revert from NEXUS_Graphics2D_FastBlit() to
 *  NEXUS_Graphics2D_Blit().
 * SW7405-4891: DirectFB: When running multiple instances of df_andi, a
 *  deadlock can occur.
 *
 * DirectFB_1_4_Port/29   9/10/10 1:30p robertwm
 * SW7405-4863: DirectFB: Incorrect constant alpha multiplication factor.
 *
 * DirectFB_1_4_Port/28   8/20/10 3:34p robertwm
 * SW35230-1074: DirectFB: Debug in graphics driver does not display
 *  correct surface pixel format.
 * SW7405-4791: DirectFB: DirectFB-1.4.1 Phase 2.5 fails to build with
 *  Nexus Phase 5.0 reference software.
 *
 * DirectFB_1_4_Port/27   8/20/10 9:44a robertwm
 * SW35230-1074: DirectFB: Debug in graphics driver does not display
 *  correct surface pixel format.
 *
 * DirectFB_1_4_Port/26   8/16/10 11:13p robertwm
 * SW7405-4753: DirectFB: Ensure the destination surface palette is loaded
 *  if an indexed surface.
 *
 * DirectFB_1_4_Port/25   8/11/10 4:58p robertwm
 * SW35230-957: DirectFB: platform_init fails to build on 35230 with
 *  recent 3D changes.
 *
 * DirectFB_1_4_Port/24   8/9/10 8:41p robertwm
 * SW7405-4183: DirectFB: Add support for Texture Triangles in the
 *  graphics driver.
 * SW7405-4734: DirectFB: Add hardware support for FillTriangle().
 *
 * DirectFB_1_4_Port/23   7/26/10 5:31p jackf
 * SW35230-771: Added include for shim_nexus.h.
 *
 * DirectFB_1_4_Port/22   7/22/10 4:10p robertwm
 * SW7405-4649: DirectFB: Optimise surface locking and wait idle syncing.
 *
 * DirectFB_1_4_Port/21   6/8/10 1:11p robertwm
 * SW7405-4468: DirectFB: Graphics driver fails to build if Nexus doesn't
 *  support source alpha premultiplication.
 * SW7420-817: DirectFB: Performing a source premultiply with color alpha
 *  blending results in wrong output.
 *
 * DirectFB_1_4_Port/20   5/27/10 8:50p robertwm
 * SW7420-783: DirectFB: Should use Point Sample filtering when performing
 *  a stretch with no smooth scaling.
 * SW7420-784: DirectFB: SetPalette on a non-layer surface doesn't work.
 *
 * DirectFB_1_4_Port/19   5/26/10 12:11p robertwm
 * SW7420-773: DirectFB: Assertion if source surface's palette is NULL and
 *  destination surface is palettised.
 *
 * DirectFB_1_4_Port/18   4/14/10 11:43a robertwm
 * SW7405-4208: DirectFB: Running df_andi with different pixelformats
 *  results in grreen squares around penguins.
 * SW7405-4209: DirectFB: Wrong surface format displayed when debugging is
 *  enabled.
 *
 * DirectFB_1_4_Port/17   4/9/10 4:21p robertwm
 * SW7405-4183: DirectFB: Add support for Texture Triangles in the
 *  graphics driver.
 *
 ***************************************************************************/

#include <core/state.h>
#include <core/gfxcard.h>
#include <core/system.h>

#include <gfx/clip.h>
#include <gfx/convert.h>
#include <gfx/util.h>
#include <misc/conf.h>

#include <fusion/conf.h>

#include <core/core.h>
#include <core/palette.h>
#include <core/graphics_driver.h>
#include <core/layer_context.h>
#include <core/layer_control.h>
#include <core/layer_region.h>

extern void (*nexus_graphics2d_sync)(void);

#include "bcmnexus_gfx.h"
#include "bcmnexus_pool.h"

#ifdef __GNUC__
#define UNUSED_ __attribute__ ((unused))
#else
#define UNUSED_
#endif

D_DEBUG_DOMAIN( bcmnexusGraphics,  "bcmNexusGfx/Graphics",  "Broadcom NEXUS Graphics Acceleration driver" );
D_DEBUG_DOMAIN( bcmnexusGraphicsX, "bcmNexusGfx/GraphicsX", "Extended Debug for Broadcom NEXUS Graphics Acceleration driver" );
D_DEBUG_DOMAIN( bcmnexusGraphicsPB, "bcmNexusGfx/GraphicsPB", "Packet Blit Debug for Broadcom NEXUS Graphics Acceleration driver" );
D_DEBUG_DOMAIN( bcmnexusGraphicsQ,  "bcmNexusGfx/GraphicsQ",  "Extended Queue Debug for Broadcom NEXUS Graphics Acceleration driver" );

DFB_GRAPHICS_DRIVER( bcmnexus_gfx )

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
#define BCM_DRIVER_NAME "Bcmnexus Packet Buffer GFX Driver"
#else
#define BCM_DRIVER_NAME "Bcmnexus Nexus 2D GFX Driver"
#endif

#define BCM_VENDOR_NAME "Broadcom Corporation"

/*
 * Color space conversion matricies.
 */
#define BCMNEXUS_COLOR_MATRIX_SHIFT     10

/* RGB -> BT.601 */
/* Y  = R *  0.257 + G *  0.504 + B *  0.098 + 16  */
/* Cb = R * -0.148 + G * -0.291 + B *  0.439 + 128 */
/* Cr = R *  0.439 + G * -0.368 + B * -0.071 + 128 */
static NEXUS_Graphics2DColorMatrix RGB_to_BT601 =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         0.257,  0.504,  0.098,  0.000,    16.000,
        -0.148, -0.291,  0.439,  0.000,   128.000,
         0.439, -0.368, -0.071,  0.000,   128.000,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* RGB -> BT.709 */
/* Y  = R *  0.184 + G *  0.614 + B *  0.061 + 16  */
/* Cb = R * -0.101 + G * -0.339 + B *  0.440 + 128 */
/* Cr = R *  0.440 + G * -0.400 + B * -0.040 + 128 */
static NEXUS_Graphics2DColorMatrix RGB_to_BT709 =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         0.184,  0.614,  0.061,  0.000,    16.000,
        -0.101, -0.339,  0.440,  0.000,   128.000,
         0.440, -0.400, -0.040,  0.000,   128.000,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* BT.601 -> RGB */
/* R = Y * 1.164 + Cb * 0.000 + Cr * 1.596 - 223 */
/* G = Y * 1.164 - Cb * 0.391 - Cr * 0.813 + 135 */
/* B = Y * 1.164 + Cb * 2.018 + Cr * 0.000 - 277 */
static NEXUS_Graphics2DColorMatrix BT601_to_RGB =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         1.164,  0.000,  1.596,  0.000,  -223.000,
         1.164, -0.391, -0.813,  0.000,   135.000,
         1.164,  2.018,  0.000,  0.000,  -277.000,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* BT.709 -> RGB */
/* R = Y * 1.164 + Cb * 0.000 + Cr * 1.787 - 247 */
/* G = Y * 1.164 - Cb * 0.531 - Cr * 0.222 + 76 */
/* B = Y * 1.164 + Cb * 2.112 + Cr * 0.000 - 289 */
static NEXUS_Graphics2DColorMatrix BT709_to_RGB =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         1.164,  0.000,  1.787,  0.000,  -247.000,
         1.164, -0.531, -0.222,  0.000,    76.000,
         1.164,  2.112,  0.000,  0.000,  -289.000,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* BT.601 -> BT.709 */
static NEXUS_Graphics2DColorMatrix BT601_to_BT709 =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         1.000, -0.117, -0.206,  0.000,    40.961,
         0.000,  1.020,  0.114,  0.000,   -17.122,
         0.000,  0.076,  1.027,  0.000,   -13.040,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* BT.709 -> BT.601 */
static NEXUS_Graphics2DColorMatrix BT709_to_BT601 =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         1.000, -0.061,  0.347,  0.000,   -37.497,
         0.000,  1.082, -0.200,  0.000,    15.569,
         0.000,  0.045,  0.866,  0.000,    12.118,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* BT.709 -> to Full Range BT.601 */
static NEXUS_Graphics2DColorMatrix BT709_to_BT601FR =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         1.164, -0.071,  0.404,  0.000,   -62.187,
         0.000,  1.232, -0.229,  0.000,     0.087,
         0.000,  0.051,  0.987,  0.000,    -3.935,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* BT.601 -> Full Range BT.601 */
static NEXUS_Graphics2DColorMatrix BT601_to_BT601FR =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         1.164,  0.000,  0.000,  0.000,   -19.010,
         0.000,  1.138,  0.000,  0.000,   -17.498,
         0.000,  0.000,  1.139,  0.000,   -17.628,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* Full Range BT.601 -> to BT.709 */
static NEXUS_Graphics2DColorMatrix BT601FR_to_BT709 =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         0.859, -0.103, -0.180,  0.000,    52.317,
         0.000,  0.896,  0.100,  0.000,     0.405,
         0.000,  0.067,  0.902,  0.000,     3.928,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* Full Range BT.601 -> BT.601 */
static NEXUS_Graphics2DColorMatrix BT601FR_to_BT601 =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         0.859,  0.000,  0.000,  0.000,    16.000,
         0.000,  0.878,  0.000,  0.000,    16.000,
         0.000,  0.000,  0.878,  0.000,    16.000,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* Full Range BT.601 -> RGB */
static NEXUS_Graphics2DColorMatrix BT601FR_to_RGB =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         1.000,  0.000,  1.402,  0.000,  -179.456,
         1.000, -0.344, -0.714,  0.000,   135.460,
         1.000,  1.772,  0.000,  0.000,  -226.816,
         0.000,  0.000,  0.000,  1.000,     0.000 );

/* RGB -> Full Range BT.601 */
static NEXUS_Graphics2DColorMatrix RGB_to_BT601FR =
    MAKE_CSC_MATRIX(BCMNEXUS_COLOR_MATRIX_SHIFT,
         0.299,  0.587,  0.114,  0.000,     0.000,
        -0.169, -0.331,  0.500,  0.000,   128.000,
         0.500, -0.419, -0.081,  0.000,   128.000,
         0.000,  0.000,  0.000,  1.000,     0.000 );


/***************************************************************************
Description:
    This matrix is used to select which color conversion matrix should be
    used to convert from the input surface color space to the output surface
    color space.
****************************************************************************/
typedef enum {
    CS_UNKNOWN          = 0,    /* Indicies to matrix below */
    CS_BT601            = 1,
    CS_BT709            = 2,
    CS_BT601_FULLRANGE  = 3,
    CS_RGB              = 4,
    CS_LAST             = CS_RGB
}bcmnexus_ColorSpace;

#define NUM_COLORSPACES (CS_LAST - CS_UNKNOWN + 1)

static NEXUS_Graphics2DColorMatrix *bcmnexus_CSCMatrix[NUM_COLORSPACES][NUM_COLORSPACES] =
{
    {NULL,  NULL,                   NULL,                   NULL,                   NULL                },
    {NULL,  NULL,                   &BT601_to_BT709,        &BT601_to_BT601FR,      &BT601_to_RGB       },
    {NULL,  &BT709_to_BT601,        NULL,                   &BT709_to_BT601FR,      &BT709_to_RGB       },
    {NULL,  &BT601FR_to_BT601,      &BT601FR_to_BT709,      NULL,                   &BT601FR_to_RGB     },
    {NULL,  &RGB_to_BT601,          &RGB_to_BT709,          &RGB_to_BT601FR,        NULL                }
};

static __inline__ bcmnexus_ColorSpace
cs_to_index(DFBSurfaceColorSpace ColorSpace)
{
    switch (ColorSpace)
    {
        case DSCS_UNKNOWN:
            return CS_UNKNOWN;
        case DSCS_BT601:
            return CS_BT601;
        case DSCS_BT709:
            return CS_BT709;
        case DSCS_BT601_FULLRANGE:
            return CS_BT601_FULLRANGE;
        case DSCS_RGB:
            return CS_RGB;
        default:
            return CS_UNKNOWN;
    }
}

static __inline__ void
dfb_convert_colorspace(
    DFBSurfaceColorSpace fromCS, u16 C2, u16 C1, u16 C0,
    DFBSurfaceColorSpace toCS, u16 *pC2, u16 *pC1, u16 *pC0)
{
    NEXUS_Graphics2DColorMatrix *M;
    bcmnexus_ColorSpace from, to;
    u32 c2, c1, c0;

    from = cs_to_index(fromCS);
    to = cs_to_index(toCS);

    M = bcmnexus_CSCMatrix[from][to];
    if (M == NULL)
    {
        *pC2 = C2;
        *pC1 = C1;
        *pC0 = C0;
        return;
    }

    c2 = (M->coeffMatrix[0]*C2  + M->coeffMatrix[1]*C1  + M->coeffMatrix[2]*C0  + M->coeffMatrix[4])  >> M->shift;
    c1 = (M->coeffMatrix[5]*C2  + M->coeffMatrix[6]*C1  + M->coeffMatrix[7]*C0  + M->coeffMatrix[9])  >> M->shift;
    c0 = (M->coeffMatrix[10]*C2 + M->coeffMatrix[11]*C1 + M->coeffMatrix[12]*C0 + M->coeffMatrix[14]) >> M->shift;

    *pC2 = c2 > 0x100 ? 0x100 : (u16)c2;
    *pC1 = c1 > 0x100 ? 0x100 : (u16)c1;
    *pC0 = c0 > 0x100 ? 0x100 : (u16)c0;
}


/******************* macros for matrix calculations ****************/

#define DFB_TRANSFORM(x, y, m, affine) \
do { \
     s32 _x, _y, _w; \
     if (affine) { \
          _x = ((x) * (m)[0] + (y) * (m)[1] + (m)[2] + 0x8000) >> 16; \
          _y = ((x) * (m)[3] + (y) * (m)[4] + (m)[5] + 0x8000) >> 16; \
     } \
     else { \
          _x = ((x) * (m)[0] + (y) * (m)[1] + (m)[2]); \
          _y = ((x) * (m)[3] + (y) * (m)[4] + (m)[5]); \
          _w = ((x) * (m)[6] + (y) * (m)[7] + (m)[8]); \
          if (!_w) { \
               _x = (_x < 0) ? -0x7fffffff : 0x7fffffff; \
               _y = (_y < 0) ? -0x7fffffff : 0x7fffffff; \
          } \
          else { \
               _x /= _w; \
               _y /= _w; \
          } \
     } \
     (x) = _x; \
     (y) = _y; \
} while (0)


#define AFFINE_TRANSFORM(x, y, m) \
do { \
     s32 _x, _y; \
     _x = ((x) * (m)[0] + (y) * (m)[1] + (m)[2] + 0x8000) >> 16; \
     _y = ((x) * (m)[3] + (y) * (m)[4] + (m)[5] + 0x8000) >> 16; \
     (x) = _x; \
     (y) = _y; \
} while (0)


#define APPLY_MATRIX_RECT(x, y, w , h, m) \
do { \
     int _x2, _y2; \
     _x2 = (x) + (w); \
     _y2 = (y) + (h); \
     AFFINE_TRANSFORM((x),(y),(m)); \
     AFFINE_TRANSFORM(_x2,_y2,(m)); \
     \
     if ((x) < _x2) { \
         (w) = (_x2-(x)); \
     } else { \
         (w) = ((x)-_x2); \
         (x) = _x2; \
     } \
     if ((y) < _y2) { \
         (h) = (_y2-(y)); \
     } \
     else { \
        (h) = ((y)-_y2); \
        (y) = _y2; \
     } \
} while (0)

#ifdef BCMNEXUS_TRAPEZOID_SUPPORTED
#define APPLY_MATRIX_TRAP(x1, y1, w1 ,x2, y2, w2, m) \
do { \
     int _x3, _x4, _y1, _y2; \
     _x3 = (x1) + (w1); \
     _x4 = (x2) + (w2); \
     _y1 = (y1); \
     _y2 = (y2); \
     AFFINE_TRANSFORM((x1),(y1),(m)); \
     AFFINE_TRANSFORM((x2),(y2),(m)); \
     AFFINE_TRANSFORM(_x3, _y1,(m)); \
     AFFINE_TRANSFORM(_x4, _y2,(m)); \
     \
     if ((x1) < _x3) { \
         (w1) = (_x3-(x1)); \
     } else { \
         (w1) = ((x1)-_x3); \
         (x1) = _x3; \
     } \
     if ((x2) < _x4) { \
         (w2) = (_x4-(x2)); \
     } else { \
         (w2) = ((x2)-_x4); \
         (x2) = _x4; \
     } \
     if (y2 < y1) \
     { \
          int temp = x1; \
          x1 = x2; \
          x2 = temp; \
          temp = y1; \
          y1 = y2; \
          y2 = temp; \
          temp = w1; \
          w1 = w2; \
          w2 = temp; \
     } \
} while (0)
#endif


static DFBResult
bcmnexus_gfxEnterMode( bcmnexus_gfxDriverData *pBdriver,
                       bcmnexus_gfxDeviceData *pBdevice,
                       bcmnexus_gfxMode        mode,
                       int                     inc );

/**************** functions for clipping *************************************/

static void
soft_clip_blit_flip( DFBRectangle           *srect,
                     DFBRectangle           *drect,
                     const DFBRegion        *clip,
                     DFBSurfaceBlittingFlags flags )
{
     DFBRegion dest    = DFB_REGION_INIT_FROM_RECTANGLE( drect );
     DFBRegion clipped = dest;

     int xfixup;
     int yfixup;

     dfb_region_region_intersect( &clipped, clip );
     dfb_rectangle_from_region( drect, &clipped );

     if (flags & DSBLIT_FLIP_HORIZONTAL)
         xfixup = dest.x2 - clipped.x2;
     else
         xfixup = clipped.x1 - dest.x1;

     if (flags & DSBLIT_FLIP_VERTICAL)
         yfixup = dest.y2 - clipped.y2;
     else
         yfixup = clipped.y1 - dest.y1;

     srect->x += xfixup;
     srect->y += yfixup;
     srect->w = drect->w;
     srect->h = drect->h;
}

static void
soft_clip_stretchblit_flip( DFBRectangle           *srect,
                            DFBRectangle           *drect,
                            const DFBRegion        *clip,
                            DFBSurfaceBlittingFlags flags,
                            uint32_t               *fix12HScale,
                            uint32_t               *fix12VScale,
                            int32_t                *fix12HPhase,
                            int32_t                *fix12VPhase )
{
     DFBRegion        dest = DFB_REGION_INIT_FROM_RECTANGLE( drect );
     DFBRectangle orig_dst = *drect;
     DFBRegion     clipped = dest;

     int    xfixup;
     int    yfixup;

#if defined(CONFIG_NETFLIX_SUPPORT) /* NETFLIX PATCH */
     if(dest.x1 == clip->x1 && dest.x2 == clip->x2 &&
        dest.y1 == clip->y1 && dest.y2 == clip->y2)
     {
        *fix12HScale = 0;
        *fix12VScale = 0;
        *fix12HPhase = 0;
        *fix12VPhase = 0;
        return;
     }
#endif

     dfb_region_region_intersect( &clipped, clip );
     dfb_rectangle_from_region( drect, &clipped );

     *fix12HScale = (srect->w << 12) / orig_dst.w;
     *fix12VScale = (srect->h << 12) / orig_dst.h;

     if (flags & DSBLIT_FLIP_HORIZONTAL)
     {
         xfixup =  (int)((*fix12HScale * (dest.x2 - clipped.x2)) >> 12);
         *fix12HPhase = (*fix12HScale * (dest.x2 - clipped.x2)) & 0xfff;
     }
     else
     {
         xfixup =  (int)((*fix12HScale * (clipped.x1 - dest.x1)) >> 12);
         *fix12HPhase = (*fix12HScale * (clipped.x1 - dest.x1)) & 0xfff;
     }

     if (flags & DSBLIT_FLIP_VERTICAL)
     {
         yfixup =  (int)((*fix12VScale * (dest.y2 - clipped.y2)) >> 12);
         *fix12VPhase = (*fix12VScale * (dest.y2 - clipped.y2)) & 0xfff;
     }
     else
     {
         yfixup =  (int)((*fix12VScale * (clipped.y1 - dest.y1)) >> 12);
         *fix12VPhase = (*fix12VScale * (clipped.y1 - dest.y1)) & 0xfff;
     }

     srect->x += xfixup;
     srect->y += yfixup;

     if (srect->w > 0)
     {
         srect->w = (int)((*fix12HScale * drect->w + *fix12HPhase + 0xfff) >> 12);
         /* The minimum w after source clipping should be one pixel at least, otherwise
            there is nothing to be drawn on clipped destination */
         if (srect->w == 0) srect->w = 1;
     }
     if (srect->h > 0)
     {
         srect->h = (int)((*fix12VScale * drect->h + *fix12VPhase + 0xfff) >> 12);

         /* The minimum h after source clipping should be one pixel at least. */
         if (srect->h == 0) srect->h = 1;
     }
}

/*********************************************************************************/

static __inline__ bool bcmnexus_isAlphaOnly(NEXUS_PixelFormat pixelFormat)
{
    return (pixelFormat == NEXUS_PixelFormat_eA8)
#ifdef BCMNEXUS_PIXELFORMAT_A4_SUPPORTED
        || (pixelFormat == NEXUS_PixelFormat_eA4)
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A2_SUPPORTED
        || (pixelFormat == NEXUS_PixelFormat_eA2)
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A1_SUPPORTED
        || (pixelFormat == NEXUS_PixelFormat_eA1)
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A0_SUPPORTED
        || (pixelFormat == NEXUS_PixelFormat_eA0)
#endif
    ;
}

#ifndef BCMNEXUS_GFX_PACKET_BUFFER
#if DIRECT_BUILD_DEBUG  /* Build with debug support? */
static char *filter_to_string[] =
{
    [NEXUS_Graphics2DFilterCoeffs_ePointSample]         = "Point Sample",
    [NEXUS_Graphics2DFilterCoeffs_eBilinear]            = "Bilinear",
    [NEXUS_Graphics2DFilterCoeffs_eAnisotropic]         = "Anisotropic",
    [NEXUS_Graphics2DFilterCoeffs_eSharp]               = "Sharp",
    [NEXUS_Graphics2DFilterCoeffs_eSharper]             = "Sharper",
    [NEXUS_Graphics2DFilterCoeffs_eBlurry]              = "Blurry",
    [NEXUS_Graphics2DFilterCoeffs_eAntiFlutter]         = "Anti-Flutter",
    [NEXUS_Graphics2DFilterCoeffs_eAntiFlutterBlurry]   = "Anti-Flutter Blurry",
    [NEXUS_Graphics2DFilterCoeffs_eAntiFlutterSharp]    = "Anti-Flutter Sharp",
};

static char *pixelformat_to_string[] =
{
    [NEXUS_PixelFormat_eUnknown]            = "Unknown",
    [NEXUS_PixelFormat_eR5_G6_B5]           = "R5_G6_B5",
#ifdef BCMNEXUS_PIXELFORMAT_B5_G6_R5_SUPPORTED
    [NEXUS_PixelFormat_eB5_G6_R5]           = "B5_G6_R5",
#endif
    [NEXUS_PixelFormat_eA1_R5_G5_B5]        = "A1_R5_G5_B5",
#ifdef BCMNEXUS_PIXELFORMAT_X1_R5_G5_B5_SUPPORTED
    [NEXUS_PixelFormat_eX1_R5_G5_B5]        = "X1_R5_G5_B5",
#endif
#ifdef BCMNEXUS_PIXELFORMAT_X1_B5_G5_R5_SUPPORTED
    [NEXUS_PixelFormat_eX1_B5_G5_R5]        = "X1_B5_G5_R5",
#endif
    [NEXUS_PixelFormat_eR5_G5_B5_A1]        = "R5_G5_B5_A1",
    [NEXUS_PixelFormat_eA4_R4_G4_B4]        = "A4_R4_G4_B4",
#ifdef BCMNEXUS_PIXELFORMAT_X4_R4_G4_B4_SUPPORTED
    [NEXUS_PixelFormat_eX4_R4_G4_B4]        = "X4_R4_G4_B4",
#endif
    [NEXUS_PixelFormat_eR4_G4_B4_A4]        = "R4_G4_B4_A4",
#ifdef BCMNEXUS_PIXELFORMAT_R8_G8_B8_SUPPORTED
    [NEXUS_PixelFormat_eR8_G8_B8]           = "R8_G8_B8",
#endif
    [NEXUS_PixelFormat_eA8_R8_G8_B8]        = "A8_R8_G8_B8",
#ifdef BCMNEXUS_PIXELFORMAT_X8_R8_G8_B8_SUPPORTED
    [NEXUS_PixelFormat_eX8_R8_G8_B8]        = "X8_R8_G8_B8",
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A8_B8_G8_R8_SUPPORTED
    [NEXUS_PixelFormat_eR8_G8_B8_A8]        = "R8_G8_B8_A8",
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A8_B8_G8_R8_SUPPORTED
    [NEXUS_PixelFormat_eA8_B8_G8_R8]        = "A8_B8_G8_R8",
#endif
    [NEXUS_PixelFormat_ePalette2]           = "Palette2",
    [NEXUS_PixelFormat_ePalette4]           = "Palette4",
    [NEXUS_PixelFormat_ePalette8]           = "Palette8",
#ifdef BCMNEXUS_PIXELFORMAT_A0_SUPPORTED
    [NEXUS_PixelFormat_eA0]                 = "A0",
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A1_SUPPORTED
    [NEXUS_PixelFormat_eA1]                 = "A1",
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A2_SUPPORTED
    [NEXUS_PixelFormat_eA2]                 = "A2",
#endif
#ifdef BCMNEXUS_PIXELFORMAT_A4_SUPPORTED
    [NEXUS_PixelFormat_eA4]                 = "A4",
#endif
    [NEXUS_PixelFormat_eA8]                 = "A8",
    [NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08]    = "Cr8_Y18_Cb8_Y08",
    [NEXUS_PixelFormat_eY18_Cr8_Y08_Cb8]    = "Y18_Cr8_Y08_Cb8",
};

static char *colorop_to_string[] =
{
    [NEXUS_BlitColorOp_eCopySource]                 = "NEXUS_BlitColorOp_eCopySource",
    [NEXUS_BlitColorOp_eUseConstantAlpha]           = "NEXUS_BlitColorOp_eUseConstantAlpha",
    [NEXUS_BlitColorOp_eUseSourceAlpha]             = "NEXUS_BlitColorOp_eUseSourceAlpha",
    [NEXUS_BlitColorOp_eUseDestAlpha]               = "NEXUS_BlitColorOp_eUseDestAlpha",
    [NEXUS_BlitColorOp_eSelectPaletteWithColorkey]  = "NEXUS_BlitColorOp_eSelectPaletteWithColorkey",
    [NEXUS_BlitColorOp_eAdd]                        = "NEXUS_BlitColorOp_eAdd",
    [NEXUS_BlitColorOp_eUseBlendEquation]           = "NEXUS_BlitColorOp_eUseBlendEquation",
};

static char *alphaop_to_string[] =
{
    [NEXUS_BlitAlphaOp_eCopySource]                 = "NEXUS_BlitAlphaOp_eCopySource",
    [NEXUS_BlitAlphaOp_eCopyDest]                   = "NEXUS_BlitAlphaOp_eCopyDest",
    [NEXUS_BlitAlphaOp_eCopyConstant]               = "NEXUS_BlitAlphaOp_eCopyConstant",
    [NEXUS_BlitAlphaOp_eCombine]                    = "NEXUS_BlitAlphaOp_eCombine",
    [NEXUS_BlitAlphaOp_eEmulateTransparentVideo]    = "NEXUS_BlitAlphaOp_eEmulateTransparentVideo",
    [NEXUS_BlitAlphaOp_eAdd]                        = "NEXUS_BlitAlphaOp_eAdd",
    [NEXUS_BlitAlphaOp_eUseBlendEquation]           = "NEXUS_BlitAlphaOp_eUseBlendEquation",
};

static void
bf_to_string(NEXUS_BlendFactor bf,
             char            **ppBfString)
{
    switch (bf)
    {
        case NEXUS_BlendFactor_eZero:
            strcpy(*ppBfString, "0");
            *ppBfString += strlen("0");
            break;
        case NEXUS_BlendFactor_eHalf:
            strcpy(*ppBfString, "1/2");
            *ppBfString += strlen("1/2");
            break;
        case NEXUS_BlendFactor_eOne:
            strcpy(*ppBfString, "1");
            *ppBfString += strlen("1");
            break;
        case NEXUS_BlendFactor_eSourceColor:
            strcpy(*ppBfString, "Sc");
            *ppBfString += strlen("Sc");
            break;
        case NEXUS_BlendFactor_eInverseSourceColor:
            strcpy(*ppBfString, "(1-Sc)");
            *ppBfString += strlen("(1-Sc)");
            break;
        case NEXUS_BlendFactor_eSourceAlpha:
            strcpy(*ppBfString, "Sa");
            *ppBfString += strlen("Sa");
            break;
        case NEXUS_BlendFactor_eInverseSourceAlpha:
            strcpy(*ppBfString, "(1-Sa)");
            *ppBfString += strlen("(1-Sa)");
            break;
        case NEXUS_BlendFactor_eDestinationColor:
            strcpy(*ppBfString, "Dc");
            *ppBfString += strlen("Dc");
            break;
        case NEXUS_BlendFactor_eInverseDestinationColor:
            strcpy(*ppBfString, "(1-Dc)");
            *ppBfString += strlen("(1-Dc)");
            break;
        case NEXUS_BlendFactor_eDestinationAlpha:
            strcpy(*ppBfString, "Da");
            *ppBfString += strlen("Da");
            break;
        case NEXUS_BlendFactor_eInverseDestinationAlpha:
            strcpy(*ppBfString, "(1-Da)");
            *ppBfString += strlen("(1-Da)");
            break;
        case NEXUS_BlendFactor_eConstantColor:
            strcpy(*ppBfString, "Cc");
            *ppBfString += strlen("Cc");
            break;
        case NEXUS_BlendFactor_eInverseConstantColor:
            strcpy(*ppBfString, "(1-Cc)");
            *ppBfString += strlen("(1-Cc)");
            break;
        case NEXUS_BlendFactor_eConstantAlpha:
            strcpy(*ppBfString, "Ca");
            *ppBfString += strlen("Ca");
            break;
        case NEXUS_BlendFactor_eInverseConstantAlpha:
            strcpy(*ppBfString, "(1-Ca)");
            *ppBfString += strlen("(1-Ca)");
            break;
        default:
            break;
    }
}

static char *
be_to_string(NEXUS_BlendEquation *pBe,
             char                *pBeString)
{
    char *origString = pBeString;
    char  tmp_buf[2];

    bf_to_string(pBe->a, &pBeString);
    strcat(pBeString, ".");
    pBeString++;
    bf_to_string(pBe->b, &pBeString);
    sprintf(tmp_buf, "%s", pBe->subtract_cd ? "-" : "+");
    strcat(pBeString, tmp_buf);
    pBeString++;
    bf_to_string(pBe->c, &pBeString);
    strcat(pBeString, ".");
    pBeString++;
    bf_to_string(pBe->d, &pBeString);
    sprintf(tmp_buf, "%s", pBe->subtract_e ? "-" : "+");
    strcat(pBeString, tmp_buf);
    pBeString++;
    bf_to_string(pBe->e, &pBeString);
    return origString;
}

static void bcmnexus_debugBlit(char                         *hdrMsg,
                               bcmnexus_gfxDriverData       *pBdrv,
                               NEXUS_Graphics2DBlitSettings *pBlitSettings)
{
#ifndef D_CHECK_DOMAIN
#define D_CHECK_DOMAIN(d) direct_debug_check_domain( &d )
#endif

    char blitString[80];

    if (D_CHECK_DOMAIN(bcmnexusGraphicsX))
    {
        D_DEBUG_AT( bcmnexusGraphicsX, "===== %s =====\n", hdrMsg );
        D_DEBUG_AT( bcmnexusGraphicsX, "Source Surface Format = %s\n", pixelformat_to_string[pBdrv->sourcePixelFormat] );
        D_DEBUG_AT( bcmnexusGraphicsX, "ColorOp=%s, AlphaOp=%s\n",
                    colorop_to_string[pBlitSettings->colorOp], alphaop_to_string[pBlitSettings->alphaOp]);
        D_DEBUG_AT( bcmnexusGraphicsX, "constantColor=0x%08x\n", pBlitSettings->constantColor );
        D_DEBUG_AT( bcmnexusGraphicsX, "Source Color Key %s, color(ARGB)=0x%08x\n",
                pBlitSettings->colorKey.source.enabled ? "On" : "Off", pBlitSettings->colorKey.source.lower);
        D_DEBUG_AT( bcmnexusGraphicsX, "Destination Color Key %s, color(ARGB)=0x%08x\n",
                pBlitSettings->colorKey.dest.enabled ? "On" : "Off", pBlitSettings->colorKey.dest.lower);
        D_DEBUG_AT( bcmnexusGraphicsX, "Horizontal/Vertical Filters=%s/%s\n",
                filter_to_string[pBlitSettings->horizontalFilter],
                filter_to_string[pBlitSettings->verticalFilter] );
#ifdef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
        D_DEBUG_AT( bcmnexusGraphicsX, "alphaPremultEnabled=%d\n", pBlitSettings->alphaPremultiplySourceEnabled );
#endif
#ifdef BCMNEXUS_ROP_SUPPORTED
        D_DEBUG_AT( bcmnexusGraphicsX, "Pattern enable=%d, vector=0x%02x\n",
                pBlitSettings->patternSettingsEnabled, pBlitSettings->patternSettings.ropVector );
#endif
        D_DEBUG_AT( bcmnexusGraphicsX, "conversionYMatrixEnabled=%d\n", pBlitSettings->conversionMatrixEnabled );
        D_DEBUG_AT( bcmnexusGraphicsX, "conversionMatrix: A[0]=0x%03x, A[6]=0x%03x, A[12]=0x%03x, A[18]=0x%03x\n",
            pBlitSettings->conversionMatrix.coeffMatrix[0],
            pBlitSettings->conversionMatrix.coeffMatrix[6],
            pBlitSettings->conversionMatrix.coeffMatrix[12],
            pBlitSettings->conversionMatrix.coeffMatrix[18]
            );
        D_DEBUG_AT( bcmnexusGraphicsX, "conversionMatrix: A[3]=0x%03x, A[8]=0x%03x, A[13]=0x%03x, A[19]=0x%03x\n",
            pBlitSettings->conversionMatrix.coeffMatrix[3],
            pBlitSettings->conversionMatrix.coeffMatrix[8],
            pBlitSettings->conversionMatrix.coeffMatrix[13],
            pBlitSettings->conversionMatrix.coeffMatrix[19]
            );
        if (pBlitSettings->colorOp == NEXUS_BlitColorOp_eUseBlendEquation) {
            D_DEBUG_AT( bcmnexusGraphicsX, "Color BE=%s\n", be_to_string(&pBlitSettings->colorBlend, &blitString[0] ));
        }
        if (pBlitSettings->alphaOp == NEXUS_BlitAlphaOp_eUseBlendEquation) {
            D_DEBUG_AT( bcmnexusGraphicsX, "Alpha BE=%s\n", be_to_string(&pBlitSettings->alphaBlend, &blitString[0] ));
        }
        D_DEBUG_AT( bcmnexusGraphicsX, "===== Blitting Done =====\n");
    }
}
#else

static void bcmnexus_debugBlit(UNUSED_ char                         *hdrMsg,
                               UNUSED_ bcmnexus_gfxDriverData       *pBdrv,
                               UNUSED_ NEXUS_Graphics2DBlitSettings *pBlitSettings)
{
}
#endif
#endif

static struct {
        const NEXUS_BlendFactor alpha, color;
} BlendFunctionTable[12] =
{
    /* DSBF_UNKNOWN */      { NEXUS_BlendFactor_eZero,                      NEXUS_BlendFactor_eZero },
    /* DSBF_ZERO */         { NEXUS_BlendFactor_eZero,                      NEXUS_BlendFactor_eZero },
    /* DSBF_ONE */          { NEXUS_BlendFactor_eOne,                       NEXUS_BlendFactor_eOne },
    /* DSBF_SRCCOLOR */     { NEXUS_BlendFactor_eSourceAlpha,               NEXUS_BlendFactor_eSourceColor },
    /* DSBF_INVSRCCOLOR */  { NEXUS_BlendFactor_eInverseSourceAlpha,        NEXUS_BlendFactor_eInverseSourceColor },
    /* DSBF_SRCALPHA */     { NEXUS_BlendFactor_eSourceAlpha,               NEXUS_BlendFactor_eSourceAlpha },
    /* DSBF_INVSRCALPHA */  { NEXUS_BlendFactor_eInverseSourceAlpha,        NEXUS_BlendFactor_eInverseSourceAlpha },
    /* DSBF_DESTALPHA */    { NEXUS_BlendFactor_eDestinationAlpha,          NEXUS_BlendFactor_eDestinationAlpha },
    /* DSBF_INVDESTALPHA */ { NEXUS_BlendFactor_eInverseDestinationAlpha,   NEXUS_BlendFactor_eInverseDestinationAlpha },
    /* DSBF_DESTCOLOR */    { NEXUS_BlendFactor_eDestinationAlpha,          NEXUS_BlendFactor_eDestinationColor },
    /* DSBF_INVDESTCOLOR */ { NEXUS_BlendFactor_eInverseDestinationAlpha,   NEXUS_BlendFactor_eInverseDestinationColor },
    /* DSBF_SRCALPHASAT? */ { NEXUS_BlendFactor_eOne,                       NEXUS_BlendFactor_eOne },
};

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
static struct {
    const NEXUS_BlendFactor alpha, color;
} BlendFillFunctionTable[12] =
{
    /* DSBF_UNKNOWN */      { NEXUS_BlendFactor_eZero,                      NEXUS_BlendFactor_eZero },
    /* DSBF_ZERO */         { NEXUS_BlendFactor_eZero,                      NEXUS_BlendFactor_eZero },
    /* DSBF_ONE */          { NEXUS_BlendFactor_eOne,                       NEXUS_BlendFactor_eOne },
    /* DSBF_SRCCOLOR */     { NEXUS_BlendFactor_eConstantAlpha,             NEXUS_BlendFactor_eConstantColor },
    /* DSBF_INVSRCCOLOR */  { NEXUS_BlendFactor_eInverseConstantAlpha,      NEXUS_BlendFactor_eInverseConstantColor },
    /* DSBF_SRCALPHA */     { NEXUS_BlendFactor_eConstantAlpha,             NEXUS_BlendFactor_eConstantAlpha },
    /* DSBF_INVSRCALPHA */  { NEXUS_BlendFactor_eInverseConstantAlpha,      NEXUS_BlendFactor_eInverseConstantAlpha },

    /* DSBF_DESTALPHA */    { NEXUS_BlendFactor_eDestinationAlpha,          NEXUS_BlendFactor_eDestinationAlpha },
    /* DSBF_INVDESTALPHA */ { NEXUS_BlendFactor_eInverseDestinationAlpha,   NEXUS_BlendFactor_eInverseDestinationAlpha },
    /* DSBF_DESTCOLOR */    { NEXUS_BlendFactor_eDestinationAlpha,          NEXUS_BlendFactor_eDestinationColor },
    /* DSBF_INVDESTCOLOR */ { NEXUS_BlendFactor_eInverseDestinationAlpha,   NEXUS_BlendFactor_eInverseDestinationColor },
    /* DSBF_SRCALPHASAT? */ { NEXUS_BlendFactor_eOne,                       NEXUS_BlendFactor_eOne },
};

#define GFX_FULL_PACKET_SIZE (2 * \
    sizeof (BM2MC_PACKET_PacketSourceFeeder) + \
    sizeof (BM2MC_PACKET_PacketDestinationFeeder) + \
    sizeof (BM2MC_PACKET_PacketOutputFeeder) + \
    sizeof (BM2MC_PACKET_PacketOutputControl) + \
    sizeof (BM2MC_PACKET_PacketBlend) + \
    sizeof (BM2MC_PACKET_PacketRop) + \
    sizeof (BM2MC_PACKET_PacketSourceColorkey) * 2 + \
    sizeof (BM2MC_PACKET_PacketSourceColorkeyEnable) * 2 + \
    sizeof (BM2MC_PACKET_PacketFilter) + \
    sizeof (BM2MC_PACKET_PacketFilterEnable) + \
    sizeof (BM2MC_PACKET_PacketSourceColorMatrix) + \
    sizeof (BM2MC_PACKET_PacketSourceColorMatrixEnable) + \
    sizeof (BM2MC_PACKET_PacketSourcePalette) + \
    sizeof (BM2MC_PACKET_PacketAlphaPremultiply) + \
    sizeof (BM2MC_PACKET_PacketMirror) + \
    sizeof (BM2MC_PACKET_PacketScaleBlendBlit))

#define GFX_FILL_PACKET_SIZE   sizeof(BM2MC_PACKET_PacketFillBlit)

#define GFX_DRAW_PACKET_SIZE 4*sizeof(BM2MC_PACKET_PacketFillBlit)


#define GFX_BLIT_PACKET_SIZE   sizeof(BM2MC_PACKET_PacketScaleBlendBlit) + \
                               sizeof(BM2MC_PACKET_PacketFilter) + \
                               sizeof(BM2MC_PACKET_PacketFilterEnable)

#define GFX_TWO_PASSES_BLIT_PACKET_SIZE ( \
    sizeof(BM2MC_PACKET_PacketSourceFeeder) * 2 + \
    sizeof(BM2MC_PACKET_PacketOutputFeeder) * 2 + \
    sizeof(BM2MC_PACKET_PacketRop) * 2 + \
    sizeof(BM2MC_PACKET_PacketSourceColorkeyEnable) * 2 + \
    sizeof(BM2MC_PACKET_PacketDestinationColorkeyEnable) * 2 + \
    sizeof(BM2MC_PACKET_PacketSourceColorMatrixEnable) * 2 + \
    sizeof(BM2MC_PACKET_PacketAlphaPremultiply) * 2 + \
    sizeof(BM2MC_PACKET_PacketMirror) * 2 + \
    sizeof(BM2MC_PACKET_PacketFilter) + \
    sizeof(BM2MC_PACKET_PacketFilterEnable) * 2 + \
    sizeof(BM2MC_PACKET_PacketScaleBlendBlit) * 2)

static const BM2MC_PACKET_Blend colorEquation_FillOp[] = {
    { BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0,BM2MC_PACKET_BlendFactor_eZero }
};

static const BM2MC_PACKET_Blend alphaEquation_FillOp[] = {
    { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero }
};

static const BM2MC_PACKET_Blend colorEquation_BlitOp[] = {
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero }
};

static const BM2MC_PACKET_Blend alphaEquation_BlitOp[] = {
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, BM2MC_PACKET_BlendFactor_eDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero }
};


static inline void bcmnexus_gfxPktSetSourceFeederNonePacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                             bcmnexus_gfxDeviceData     *pBdev )
{
    if (pBdev->packetHWStates.packetSrcNone != true)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s()\n", __FUNCTION__);
        BM2MC_PACKET_WRITE_SourceNone(pPktSettings->packetBufNextPtr, false);
        pBdev->packetHWStates.packetSrcNone = true;
    }
}

static inline void bcmnexus_gfxPktSetSourceFeederPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                         bcmnexus_gfxDeviceData     *pBdev,
                                                         bcmAllocationData          *ad,
                                                         uint32_t                    color )
{
    if (ad)
    {
        if ((pBdev->packetHWStates.packetSrcPlane.address != ad->hwaddress)   ||
            (pBdev->packetHWStates.packetSrcPlane.pitch   != ad->pitch)       ||
            (pBdev->packetHWStates.packetSrcPlane.format  != ad->pixelformat) ||
            (pBdev->packetHWStates.packetSrcColor         != color)           ||
            (pBdev->packetHWStates.packetSrcNone))
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): sur=%p handle=%p vaddr=%p hwaddr=0x%08x color=0x%08x\n",
                       __FUNCTION__, (void *)ad->allocation->surface, (void *)ad->bcmSurfaceHandle, ad->address, ad->hwaddress, color);

            pPktSettings->packetSrcPlane.address = ad->hwaddress;
            pPktSettings->packetSrcPlane.pitch   = ad->pitch;
            pPktSettings->packetSrcPlane.format  = ad->pixelformat;
            BM2MC_PACKET_WRITE_SourceFeeder(pPktSettings->packetBufNextPtr, pPktSettings->packetSrcPlane, color, false);
            pBdev->packetHWStates.packetSrcPlane = pPktSettings->packetSrcPlane;
            pBdev->packetHWStates.packetSrcColor = color;
            pBdev->packetHWStates.packetSrcNone  = false;
        }
    }
    else
    {
        if ((pBdev->packetHWStates.packetSrcColor != color) ||
            (pBdev->packetHWStates.packetSrcNone))
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): color=0x%08x\n", __FUNCTION__, color);
            BM2MC_PACKET_WRITE_SourceColor(pPktSettings->packetBufNextPtr, color, false);
            pBdev->packetHWStates.packetSrcColor = color;
            pBdev->packetHWStates.packetSrcNone = false;
        }
        pPktSettings->packetSrcPlane.address = 0;
        pPktSettings->packetSrcPlane.pitch = 0;
        pBdev->packetHWStates.packetSrcPlane = pPktSettings->packetSrcPlane;

        pPktSettings->packetSrcRect.x      = 0;
        pPktSettings->packetSrcRect.y      = 0;
        pPktSettings->packetSrcRect.width  = 0;
        pPktSettings->packetSrcRect.height = 0;
    }
}

static inline void bcmnexus_gfxPktSetDestinationFeederNonePacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                                  bcmnexus_gfxDeviceData     *pBdev )
{
    if (pBdev->packetHWStates.packetDstNone != true)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s()\n", __FUNCTION__);
        BM2MC_PACKET_WRITE_DestinationNone(pPktSettings->packetBufNextPtr, false);
        pPktSettings->packetDstPlane.address = 0;
        pBdev->packetHWStates.packetDstNone = true;
    }
}

static inline void bcmnexus_gfxPktSetDestinationFeederPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                              bcmnexus_gfxDeviceData     *pBdev,
                                                              bcmAllocationData          *ad,
                                                              uint32_t                    color )
{
    if (ad)
    {
        if ((pBdev->packetHWStates.packetDstPlane.address != ad->hwaddress)   ||
            (pBdev->packetHWStates.packetDstPlane.pitch   != ad->pitch)       ||
            (pBdev->packetHWStates.packetDstPlane.format  != ad->pixelformat) ||
            (pBdev->packetHWStates.packetDstColor         != color)           ||
            (pBdev->packetHWStates.packetDstNone))
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): sur=%p handle=%p vaddr=0x%p hwaddr=0x%08x color=0x%08x\n",
                       __FUNCTION__, (void *)ad->allocation->surface, (void *)ad->bcmSurfaceHandle, ad->address, ad->hwaddress, color);

            pPktSettings->packetDstPlane.address = ad->hwaddress;
            pPktSettings->packetDstPlane.pitch   = ad->pitch;
            pPktSettings->packetDstPlane.format  = ad->pixelformat;
            BM2MC_PACKET_WRITE_DestinationFeeder(pPktSettings->packetBufNextPtr, pPktSettings->packetDstPlane, color, false);
            pBdev->packetHWStates.packetDstPlane = pPktSettings->packetDstPlane;
            pBdev->packetHWStates.packetDstColor = color;
            pBdev->packetHWStates.packetDstNone  = false;
        }
    }
    else
    {
        if ((pBdev->packetHWStates.packetDstColor != color) ||
            (pBdev->packetHWStates.packetDstNone))
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): color=0x%08x\n", __FUNCTION__, color);
            BM2MC_PACKET_WRITE_DestinationColor(pPktSettings->packetBufNextPtr, color, false);
            pBdev->packetHWStates.packetDstColor = color;
            pBdev->packetHWStates.packetDstNone = false;
        }
        pPktSettings->packetDstPlane.address = 0;
        pPktSettings->packetDstPlane.pitch = 0;
        pBdev->packetHWStates.packetDstPlane = pPktSettings->packetDstPlane;
    }
}

static inline void bcmnexus_gfxPktSetOutputFeederPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                         bcmnexus_gfxDeviceData     *pBdev,
                                                         bcmAllocationData          *ad )
{
    if((pBdev->packetHWStates.packetOutPlane.address != ad->hwaddress) ||
        (pBdev->packetHWStates.packetOutPlane.pitch  != ad->pitch)     ||
        (pBdev->packetHWStates.packetOutPlane.format != ad->pixelformat))
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): sur=%p handle=%p vaddr=0x%p hwaddr=0x%08x\n",
                   __FUNCTION__, (void *)ad->allocation->surface, (void *)ad->bcmSurfaceHandle, ad->address, ad->hwaddress);

        pPktSettings->packetOutPlane.address = ad->hwaddress;
        pPktSettings->packetOutPlane.pitch   = ad->pitch;
        pPktSettings->packetOutPlane.format  = ad->pixelformat;
        BM2MC_PACKET_WRITE_OutputFeeder(pPktSettings->packetBufNextPtr, pPktSettings->packetOutPlane,false);
        pBdev->packetHWStates.packetOutPlane = pPktSettings->packetOutPlane;
    }
}

static bool bcmnexus_gfxPktCheckBlendEq( const BM2MC_PACKET_Blend *pBlendEqA,
                                         const BM2MC_PACKET_Blend *pBlendEqB )
{
    return ( pBlendEqA->a      != pBlendEqB->a      ||
             pBlendEqA->b      != pBlendEqB->b      ||
             pBlendEqA->sub_cd != pBlendEqB->sub_cd ||
             pBlendEqA->c      != pBlendEqB->c      ||
             pBlendEqA->d      != pBlendEqB->d      ||
             pBlendEqA->sub_e  != pBlendEqB->sub_e  ||
             pBlendEqA->e      != pBlendEqB->e
           );
}

static inline void bcmnexus_gfxPktSetBlendPacket( bcmnexus_gfxDriverData   *pBdrv,
                                                  bcmnexus_gfxDeviceData   *pBdev,
                                                  const BM2MC_PACKET_Blend *pColorBlend,
                                                  const BM2MC_PACKET_Blend *pAlphaBlend )
{
    bcmnexus_gfxPacketSettings *pPktSettings = &pBdrv->packetSettings;

    if (bcmnexus_gfxPktCheckBlendEq(&pBdev->packetHWStates.packetColorBlendOp, pColorBlend) ||
        bcmnexus_gfxPktCheckBlendEq(&pBdev->packetHWStates.packetAlphaBlendOp, pAlphaBlend))
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): Color=[%dx%d(%d)%dx%d(%d)%d] Alpha=[%dx%d(%d)%dx%d(%d)%d] color=0x%08x\n", __FUNCTION__,
                pColorBlend->a, pColorBlend->b, pColorBlend->sub_cd, pColorBlend->c,pColorBlend->d,pColorBlend->sub_e,pColorBlend->e,
                pAlphaBlend->a, pAlphaBlend->b, pAlphaBlend->sub_cd, pAlphaBlend->c, pAlphaBlend->d, pAlphaBlend->sub_e, pAlphaBlend->e,
                pPktSettings->packetConstantColor);

        BM2MC_PACKET_WRITE_Blend(pPktSettings->packetBufNextPtr, *pColorBlend, *pAlphaBlend, pPktSettings->packetConstantColor, false);

        memcpy(&pBdev->packetHWStates.packetColorBlendOp, pColorBlend, sizeof(BM2MC_PACKET_Blend));
        memcpy(&pBdev->packetHWStates.packetAlphaBlendOp, pAlphaBlend, sizeof(BM2MC_PACKET_Blend));
        pBdev->packetHWStates.packetBlendColor = pPktSettings->packetConstantColor;
    }
    else if (pBdev->packetHWStates.packetBlendColor != pPktSettings->packetConstantColor)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): color=0x%08x\n", __FUNCTION__, pPktSettings->packetConstantColor);

        BM2MC_PACKET_WRITE_BlendColor(pPktSettings->packetBufNextPtr, pPktSettings->packetConstantColor, false);

        pBdev->packetHWStates.packetBlendColor = pPktSettings->packetConstantColor;
    }
}

static inline void bcmnexus_gfxPktSetRopPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                bcmnexus_gfxDeviceData     *pBdev,
                                                uint32_t                    rop )
{
    if (pBdev->packetHWStates.packetRop != rop)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): rop=0x%x\n", __FUNCTION__, rop);

        BM2MC_PACKET_WRITE_Rop(pPktSettings->packetBufNextPtr, rop, 0, 0, 0, 0, false);

        pBdev->packetHWStates.packetRop = rop;
    }
}

static inline void bcmnexus_gfxPktSetSourceColorkeyPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                           bcmnexus_gfxDeviceData     *pBdev,
                                                           bool                        enable,
                                                           uint32_t                    colorkey )
{
    if (enable)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): Colorkey=%x\n",__FUNCTION__, colorkey);

//        if (pBdev->packetHWStates.packetSrcColorkey != colorkey)
            BM2MC_PACKET_WRITE_SourceColorkey(pPktSettings->packetBufNextPtr, colorkey, colorkey, RGB_MASK, 0, 0, false, false);

//        if (pBdev->packetHWStates.packetSrcColorkeyEnable <= 0)
            BM2MC_PACKET_WRITE_SourceColorkeyEnable(pPktSettings->packetBufNextPtr, enable, false);

        pBdev->packetHWStates.packetSrcColorkey = colorkey;
    }
    else //if (pBdev->packetHWStates.packetSrcColorkeyEnable != 0)
    {
        BM2MC_PACKET_WRITE_SourceColorkeyEnable(pPktSettings->packetBufNextPtr, enable, false);
    }
    pBdev->packetHWStates.packetSrcColorkeyEnable = enable;
}

static inline void bcmnexus_gfxPktSetDestinationColorkeyPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                                bcmnexus_gfxDeviceData     *pBdev,
                                                                bool                        enable,
                                                                uint32_t                    colorkey )
{
    if(enable)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): Colorkey=%x\n",__FUNCTION__, colorkey);

//        if (pBdev->packetHWStates.packetDstColorkey != colorkey)
            BM2MC_PACKET_WRITE_DestinationColorkey(pPktSettings->packetBufNextPtr, colorkey, colorkey, RGB_MASK, 0, 0, false, false);

//        if (pBdev->packetHWStates.packetDstColorkeyEnable <= 0)
            BM2MC_PACKET_WRITE_DestinationColorkeyEnable(pPktSettings->packetBufNextPtr, enable, false);

        pBdev->packetHWStates.packetDstColorkey = colorkey;
    }
    else //if (pBdev->packetHWStates.packetDstColorkeyEnable != 0)
    {
        BM2MC_PACKET_WRITE_DestinationColorkeyEnable(pPktSettings->packetBufNextPtr, enable, false);
    }
    pBdev->packetHWStates.packetDstColorkeyEnable = enable;
}

static inline void bcmnexus_gfxPktSetFilterPacket( bcmnexus_gfxPacketSettings  *pPktSettings,
                                                   bcmnexus_gfxDeviceData      *pBdev,
                                                   NEXUS_Graphics2DFilterCoeffs horz,
                                                   NEXUS_Graphics2DFilterCoeffs vert )
{
    bool enable;

    if (BCMNEXUS_IS_VALID( m_Filter ))
        return;

    enable = (horz != NEXUS_Graphics2DFilterCoeffs_ePointSample) || (vert != NEXUS_Graphics2DFilterCoeffs_ePointSample);

    if (enable)
    {
        bool writePacket = false;
        BM2MC_PACKET_FilterCoeffs horCoeffs;
        BM2MC_PACKET_FilterCoeffs verCoeffs;

        if ((pBdev->packetHWStates.packetHorzFilter != horz) ||
            ((horz == NEXUS_Graphics2DFilterCoeffs_eAnisotropic) &&
             ((pBdev->packetHWStates.packetFilterSrcRect.width != pPktSettings->packetSrcRect.width) ||
              (pBdev->packetHWStates.packetFilterOutRect.width != pPktSettings->packetOutRect.width))))
        {
            NEXUS_Graphics2D_ConvertFilter(horz, pPktSettings->packetSrcRect.width, pPktSettings->packetOutRect.width, &horCoeffs);
            pBdev->packetHWStates.packetHorzFilter = horz;

            if (memcmp(&horCoeffs, &pBdev->packetHWStates.packetFilterHorzCoeffs, sizeof(BM2MC_PACKET_FilterCoeffs)))
            {
                pBdev->packetHWStates.packetFilterHorzCoeffs = horCoeffs;
                writePacket = true;
            }
        }

        if ((pBdev->packetHWStates.packetVertFilter != vert) ||
            ((vert == NEXUS_Graphics2DFilterCoeffs_eAnisotropic) &&
             ((pBdev->packetHWStates.packetFilterSrcRect.height != pPktSettings->packetSrcRect.height) ||
              (pBdev->packetHWStates.packetFilterOutRect.height != pPktSettings->packetOutRect.height))))
        {
            NEXUS_Graphics2D_ConvertFilter(vert, pPktSettings->packetSrcRect.height, pPktSettings->packetOutRect.height, &verCoeffs);
            pBdev->packetHWStates.packetVertFilter = vert;

            if (!writePacket && memcmp(&verCoeffs, &pBdev->packetHWStates.packetFilterVertCoeffs, sizeof(BM2MC_PACKET_FilterCoeffs)))
            {
                writePacket = true;
            }
            pBdev->packetHWStates.packetFilterVertCoeffs = verCoeffs;
        }

        if (writePacket)
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): HorzFilter=[%d], VertFilter=[%d]\n", __FUNCTION__, horz, vert);
            BM2MC_PACKET_WRITE_Filter(pPktSettings->packetBufNextPtr, pBdev->packetHWStates.packetFilterHorzCoeffs,
                                      pBdev->packetHWStates.packetFilterVertCoeffs, false);
        }

        pBdev->packetHWStates.packetFilterSrcRect = pPktSettings->packetSrcRect;
        pBdev->packetHWStates.packetFilterOutRect = pPktSettings->packetOutRect;

        if(pBdev->packetHWStates.packetFilterEnable <= 0)
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): Enable Filter\n", __FUNCTION__);
            BM2MC_PACKET_WRITE_FilterEnable(pPktSettings->packetBufNextPtr, enable, false);
        }

    }
    else if(pBdev->packetHWStates.packetFilterEnable != 0)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): Disable Filter\n", __FUNCTION__);
        BM2MC_PACKET_WRITE_FilterEnable(pPktSettings->packetBufNextPtr, enable, false);
    }

    pBdev->packetHWStates.packetFilterEnable = enable;

    BCMNEXUS_VALIDATE( m_Filter );
}

static inline bool bcmnexus_gfxPktCheckColorMatrix( int32_t *pCoeffMatrixA,
                                                    int32_t *pCoeffMatrixB )
{
    return (pCoeffMatrixA[0]  != pCoeffMatrixB[0])  ||
           (pCoeffMatrixA[3]  != pCoeffMatrixB[3])  ||
           (pCoeffMatrixA[6]  != pCoeffMatrixB[6])  ||
           (pCoeffMatrixA[8]  != pCoeffMatrixB[8])  ||
           (pCoeffMatrixA[12] != pCoeffMatrixB[12]) ||
           (pCoeffMatrixA[13] != pCoeffMatrixB[13]) ||
           (pCoeffMatrixA[18] != pCoeffMatrixB[18]) ||
           (pCoeffMatrixA[19] != pCoeffMatrixB[19]);
}

static inline void bcmnexus_gfxPktSetColorMatrixPacket( bcmnexus_gfxPacketSettings  *pPktSettings,
                                                        bcmnexus_gfxDeviceData      *pBdev,
                                                        NEXUS_Graphics2DColorMatrix *pColorMatrix )
{
    if(pColorMatrix)
    {
        if (bcmnexus_gfxPktCheckColorMatrix(&pColorMatrix->coeffMatrix[0], &pBdev->packetHWStates.packetColorMatrix.coeffMatrix[0]))

        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "SetColorMatrixPacket() Matrix Set\n");

            BM2MC_PACKET_ColorMatrix packetColorMatrix;

            NEXUS_Graphics2D_ConvertColorMatrix(pColorMatrix, &packetColorMatrix);
            BM2MC_PACKET_WRITE_SourceColorMatrix(pPktSettings->packetBufNextPtr, packetColorMatrix, false);

            memcpy(&pBdev->packetHWStates.packetColorMatrix, pColorMatrix, sizeof (NEXUS_Graphics2DColorMatrix));

        }
        if(pBdev->packetHWStates.packetColorMatrixEnable <= 0)
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "SetColorMatrixPacket() Enable\n");
            BM2MC_PACKET_WRITE_SourceColorMatrixEnable(pPktSettings->packetBufNextPtr, true, false);
            pBdev->packetHWStates.packetColorMatrixEnable = true;
        }
    }
    else if(pBdev->packetHWStates.packetColorMatrixEnable != 0)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "SetColorMatrixPacket() Disable\n");
        BM2MC_PACKET_WRITE_SourceColorMatrixEnable(pPktSettings->packetBufNextPtr, false, false);
        pBdev->packetHWStates.packetColorMatrixEnable = false;
    }
}

static inline void bcmnexus_gfxPktSetSourcePalettePacket (UNUSED_ bcmnexus_gfxPacketSettings *pPktSettings,
                                                                  bcmAllocationData          *ad )
{
    if (ad)
    {
        if (NEXUS_PIXEL_FORMAT_IS_PALETTE(ad->pixelformat))
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): sur=%p handle=%p vaddr=0x%p hwaddr=0x%08x pixel format=%d\n",
                       __FUNCTION__, (void *)ad->allocation->surface, (void *)ad->bcmSurfaceHandle, ad->address, ad->hwaddress, ad->pixelformat);

            BM2MC_PACKET_WRITE_SourcePalette(pPktSettings->packetBufNextPtr, ad->hwpalette, false);
        }
    }
}

static inline void bcmnexus_gfxPktSetAlphaPremultiplyPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                             bcmnexus_gfxDeviceData     *pBdev,
                                                             bool                        enable )
{
    if (pBdev->packetHWStates.packetAlphaPremultiply != enable)
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): alpha premultiply enable=%d\n", __FUNCTION__, enable);

        BM2MC_PACKET_WRITE_AlphaPremultiply(pPktSettings->packetBufNextPtr, enable, false);

        pBdev->packetHWStates.packetAlphaPremultiply = enable;
    }
}

static inline void bcmnexus_gfxPktSetMirrorPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                   bcmnexus_gfxDeviceData     *pBdev,
                                                   bool                        srcHorz,
                                                   bool                        srcVert,
                                                   bool                        dstHorz,
                                                   bool                        dstVert )
{
    if ((pBdev->packetHWStates.packetMirrorSrcHorz != srcHorz) || (pBdev->packetHWStates.packetMirrorSrcVert != srcVert) ||
        (pBdev->packetHWStates.packetMirrorDstHorz != dstHorz) || (pBdev->packetHWStates.packetMirrorDstVert != dstVert))
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): srcHorz=%d, srcVert=%d, dstHorz=%d, dstVert=%d\n",
                   __FUNCTION__, srcHorz, srcVert, dstHorz, dstVert);

        BM2MC_PACKET_WRITE_Mirror(pPktSettings->packetBufNextPtr, srcHorz, srcVert, dstHorz, dstVert, false, false, false);

        pBdev->packetHWStates.packetMirrorSrcHorz = srcHorz;
        pBdev->packetHWStates.packetMirrorSrcVert = srcVert;
        pBdev->packetHWStates.packetMirrorDstHorz = dstHorz;
        pBdev->packetHWStates.packetMirrorDstVert = dstVert;
    }
}

#ifdef BCMNEXUS_FIXEDSCALE_SUPPORTED
static inline void bcmnexus_gfxPktSetFixedScalePacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                       bcmnexus_gfxDeviceData     *pBdev,
                                                       int32_t                     horPhase,
                                                       int32_t                     verPhase,
                                                       uint32_t                    horStep,
                                                       uint32_t                    verStep)
{
    if ((pBdev->packetHWStates.packetHorPhase != horPhase) || (pBdev->packetHWStates.packetVerPhase != verPhase) ||
        (pBdev->packetHWStates.packetHorStep != horStep) || (pBdev->packetHWStates.packetVerStep != verStep))
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): horPhase=0x%x, verPhase=0x%x, horStep=0x%x, verStep=0x%x\n",
                   __FUNCTION__, horPhase, verPhase, horStep, verStep);

        BM2MC_PACKET_WRITE_FixedScale(pPktSettings->packetBufNextPtr, horPhase, verPhase, horStep, verStep, 12, false);

        pBdev->packetHWStates.packetHorPhase = horPhase;
        pBdev->packetHWStates.packetVerPhase = verPhase;
        pBdev->packetHWStates.packetHorStep  = horStep;
        pBdev->packetHWStates.packetVerStep  = verStep;
    }
}
#endif

static inline void bcmnexus_gfxPktSetOutputControlPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                          bcmnexus_gfxDeviceData     *pBdev,
                                                          bool                        dither,
                                                          bool                        chromaFilter )
{
    if ((pBdev->packetHWStates.packetDither != dither) || (pBdev->packetHWStates.packetChromaFilter != chromaFilter))
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "%s(): dither=%d, chromaFilter=%d\n", __FUNCTION__, dither, chromaFilter);

//        BM2MC_PACKET_WRITE_OutputControl(pPktSettings->packetBufNextPtr, dither, chromaFilter, false);
        BM2MC_PACKET_WRITE_OutputControl(pPktSettings->packetBufNextPtr, dither, ((dither)?false:chromaFilter), false);

        pBdev->packetHWStates.packetDither       = dither;
        pBdev->packetHWStates.packetChromaFilter = chromaFilter;
    }
}

static inline void bcmnexus_gfxPktSetFillPacket( bcmnexus_gfxPacketSettings *pPktSettings,
                                                 BM2MC_PACKET_Rectangle     *pRect )
{
    D_DEBUG_AT(bcmnexusGraphicsPB, "FillPacket... rect[%d, %d, %d, %d]\n", pRect->x, pRect->y, pRect->width, pRect->height);
    BM2MC_PACKET_WRITE_FillBlit(pPktSettings->packetBufNextPtr, *pRect, true);
}

static inline void bcmnexus_gfxPktSetBlitPacket( bcmnexus_gfxPacketSettings *pPktSettings, bcmnexus_gfxDeviceData *pBdev)
{
    BM2MC_PACKET_Point  tPointOut, tPointDst;
    bool enableHorzMirror = false, enableVertMirror = false;

    /* Check source/output overlap */
    if (pPktSettings->packetSrcPlane.address == pPktSettings->packetOutPlane.address)
    {
        DFBRectangle r1 = { pPktSettings->packetSrcRect.x, pPktSettings->packetSrcRect.y,
                            pPktSettings->packetSrcRect.width, pPktSettings->packetSrcRect.height };
        DFBRectangle r2 = { pPktSettings->packetOutRect.x, pPktSettings->packetOutRect.y,
                            pPktSettings->packetOutRect.width, pPktSettings->packetOutRect.height };
        r1.w++; r1.h++;
        r2.w++; r2.h++;
        if (dfb_rectangle_intersect(&r1,&r2))
        {
            if (pPktSettings->packetSrcRect.x+pPktSettings->packetSrcRect.width < pPktSettings->packetOutRect.x+pPktSettings->packetOutRect.width)
                enableHorzMirror = true;

            if (pPktSettings->packetSrcRect.y+pPktSettings->packetSrcRect.height < pPktSettings->packetOutRect.y+pPktSettings->packetOutRect.height)
                enableVertMirror = true;
        }
    }

    if (pBdev->packetHWStates.packetMirrorOutHorz != enableHorzMirror ||
        pBdev->packetHWStates.packetMirrorOutVert != enableVertMirror)
    {
        BM2MC_PACKET_WRITE_Mirror(pPktSettings->packetBufNextPtr,
                                  pBdev->packetHWStates.packetMirrorSrcHorz || enableHorzMirror,
                                  pBdev->packetHWStates.packetMirrorSrcVert || enableVertMirror,
                                  pBdev->packetHWStates.packetMirrorDstHorz || enableHorzMirror,
                                  pBdev->packetHWStates.packetMirrorDstVert || enableVertMirror,
                                  enableHorzMirror,
                                  enableVertMirror,
                                  false);
        pBdev->packetHWStates.packetMirrorOutHorz = enableHorzMirror;
        pBdev->packetHWStates.packetMirrorOutVert = enableVertMirror;
    }

    if ((pPktSettings->packetSrcRect.width  == pPktSettings->packetOutRect.width) &&
        (pPktSettings->packetSrcRect.height == pPktSettings->packetOutRect.height))
    {
        tPointOut.x = pPktSettings->packetOutRect.x;
        tPointOut.y = pPktSettings->packetOutRect.y;
        tPointDst.x = pPktSettings->packetDstRect.x;
        tPointDst.y = pPktSettings->packetDstRect.y;
        if (pPktSettings->packetDstPlane.address)
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "BlendBlitPacket... src[%d, %d, %d, %d], out[%dx, %dy], dst[%dx, %dy]\n",
                       pPktSettings->packetSrcRect.x, pPktSettings->packetSrcRect.y, pPktSettings->packetSrcRect.width, pPktSettings->packetSrcRect.height,
                       pPktSettings->packetOutRect.x, pPktSettings->packetOutRect.y, pPktSettings->packetDstRect.x, pPktSettings->packetDstRect.y);

            BM2MC_PACKET_WRITE_BlendBlit(pPktSettings->packetBufNextPtr, pPktSettings->packetSrcRect, tPointOut, tPointDst, true);
        }
        else
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "CopyBlitPacket... src[%d, %d, %d, %d], out[%dx, %dy]\n",
                       pPktSettings->packetSrcRect.x, pPktSettings->packetSrcRect.y, pPktSettings->packetSrcRect.width, pPktSettings->packetSrcRect.height,
                       pPktSettings->packetOutRect.x, pPktSettings->packetOutRect.y);

            BM2MC_PACKET_WRITE_CopyBlit(pPktSettings->packetBufNextPtr, pPktSettings->packetSrcRect, tPointOut, true);
        }
    }
    else
    {
        if (pPktSettings->packetDstPlane.address)
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "ScaleBlendBlitPacket... src[%d, %d, %d, %d], out[%d, %d, %d, %d], dst[%dx, %dy]\n",
                       pPktSettings->packetSrcRect.x, pPktSettings->packetSrcRect.y, pPktSettings->packetSrcRect.width, pPktSettings->packetSrcRect.height,
                       pPktSettings->packetOutRect.x, pPktSettings->packetOutRect.y, pPktSettings->packetOutRect.width, pPktSettings->packetOutRect.height,
                       pPktSettings->packetDstRect.x, pPktSettings->packetDstRect.y);

            tPointDst.x = pPktSettings->packetDstRect.x;
            tPointDst.y = pPktSettings->packetDstRect.y;
            BM2MC_PACKET_WRITE_ScaleBlendBlit(pPktSettings->packetBufNextPtr, pPktSettings->packetSrcRect, pPktSettings->packetOutRect, tPointDst, true);
        }
        else
        {
            D_DEBUG_AT(bcmnexusGraphicsPB, "ScaleBlitPacket... src[%d, %d, %d, %d], out[%d, %d, %d, %d]\n",
                       pPktSettings->packetSrcRect.x, pPktSettings->packetSrcRect.y, pPktSettings->packetSrcRect.width, pPktSettings->packetSrcRect.height,
                       pPktSettings->packetOutRect.x, pPktSettings->packetOutRect.y, pPktSettings->packetOutRect.width, pPktSettings->packetOutRect.height);

            BM2MC_PACKET_WRITE_ScaleBlit(pPktSettings->packetBufNextPtr, pPktSettings->packetSrcRect, pPktSettings->packetOutRect, true);
        }
    }
}

static inline NEXUS_Error bcmnexus_gfxPktWritePacketComplete( bcmnexus_gfxDriverData *pDrv )
{
    NEXUS_Error rc = NEXUS_SUCCESS;


    if (pDrv->packetSettings.packetBufStartPtr != NULL &&
        (((uint8_t*)pDrv->packetSettings.packetBufNextPtr - (uint8_t*)pDrv->packetSettings.packetBufStartPtr) >= 0))
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "WritePacketComplete from EngineSync--> bufStart=%p, usedSize=%d\n",
                   pDrv->packetSettings.packetBufStartPtr,
                   ((uint8_t*)pDrv->packetSettings.packetBufNextPtr - (uint8_t*)pDrv->packetSettings.packetBufStartPtr));

        rc = NEXUS_Graphics2D_PacketWriteComplete(pDrv->packetSettings.hGfx2D,
                                                  (uint8_t*)pDrv->packetSettings.packetBufNextPtr - (uint8_t*)pDrv->packetSettings.packetBufStartPtr);
        BDBG_ASSERT(!rc);

        pDrv->packetSettings.packetBufStartPtr = pDrv->packetSettings.packetBufNextPtr = NULL;
        pDrv->packetSettings.packetBufSize = 0;
    }
    return rc;
}

static inline NEXUS_Error bcmnexus_gfxPktGetPacketBuffer( bcmnexus_gfxDriverData *pDrv,
                                                          bcmnexus_gfxDeviceData *pDev,
                                                          size_t                  size )
{
    bcmnexus_gfxPacketSettings *pPktSettings = &pDrv->packetSettings;
    NEXUS_Error rc = NEXUS_SUCCESS;

    /* submit all previous packet commands while the packet buffer is not enough for next packet command  */
    if (pPktSettings->packetBufStartPtr != NULL &&
        (((uint8_t*)pPktSettings->packetBufNextPtr - (uint8_t*)pPktSettings->packetBufStartPtr + size) > pPktSettings->packetBufSize))
    {
        D_DEBUG_AT(bcmnexusGraphicsPB, "WritePacketComplete--> requiredSize=%d\n",
                   ((uint8_t*)pPktSettings->packetBufNextPtr - (uint8_t*)pPktSettings->packetBufStartPtr + size));
        rc = bcmnexus_gfxPktWritePacketComplete( pDrv );
    }

    /* when buffer is empty, request a new buffer */
    if (pPktSettings->packetBufStartPtr == NULL && pPktSettings->packetBufSize == 0)
    {
        PTHREAD_ROBUST_MUTEX_LOCK(&pDev->packetSpaceAvailLock);

        while(1)
        {
            rc = NEXUS_Graphics2D_GetPacketBuffer(pPktSettings->hGfx2D,
                                                  &pPktSettings->packetBufStartPtr,
                                                  &pPktSettings->packetBufSize,
                                                  GFX_FULL_PACKET_SIZE);
            BDBG_ASSERT(!rc);
            if (!pPktSettings->packetBufSize) {
                pthread_cond_wait(&pDev->packetSpaceAvailCond,&pDev->packetSpaceAvailLock);
                continue;
            }
            D_DEBUG_AT(bcmnexusGraphicsPB, "GetPacketBuffer--> bufStart=%p, size=%d\n", pPktSettings->packetBufStartPtr, pPktSettings->packetBufSize);

            pPktSettings->packetBufNextPtr = pPktSettings->packetBufStartPtr;
            break;
        }
        pthread_mutex_unlock(&pDev->packetSpaceAvailLock);
    }
    return rc;
}

#endif

/* The following routine checks the alpha/blend related blitting flags, determines if they
   are supported, and if so returns true */
static bool bcmnexus_gfxCheckStateBlitBlend(UNUSED_ bcmnexus_gfxDriverData *pBdrv,
                                                    CardState              *pState)
{
    if ((pState->src_blend == DSBF_SRCALPHASAT) || (pState->dst_blend == DSBF_SRCALPHASAT))
    {
        return false; /* exit with op still set to OpUnsupported - AlphaSat not handled */
    }
    return true;
}

/* Check whether hardware can support the requested acceleration and if so, set the accel mask */
static void bcmnexus_gfxCheckState(         void               *pDrv,
                                    UNUSED_ void               *pDev,
                                            CardState          *pState,
                                            DFBAccelerationMask accel )
{
    bcmnexus_gfxDriverData* pBdrv = (bcmnexus_gfxDriverData*) pDrv;
    bool gfx3d = false;

    D_DEBUG_AT( bcmnexusGraphics, "%s() drawing flags=0x%08x blitting flags=0x%08x accel=0x%08x src_blend=0x%08x dst_blend=0x%08x\n", __FUNCTION__,
                pState->drawingflags, pState->blittingflags, accel, pState->src_blend, pState->dst_blend );

    /* Check Destination surface to ensure it is valid */
    if (bcmnexus_getNexusPixelFormat(pState->destination->config.format) == NEXUS_PixelFormat_eUnknown)
    {
        return;
    }

    if (pState->render_options & DSRO_MATRIX) {
        /* check if the matrix is affine and only does things we can do */
        if (pState->matrix[1] || pState->matrix[3] || !pState->affine_matrix)
#ifndef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
            return;
#else
            gfx3d = true;
#endif
    }

    /* Check to see whether function will use 3d core... */
#ifdef BCMNEXUS_GFX_HAS_P3D
#ifdef BCMNEXUS_TRAPEZOID_SUPPORTED
    if (accel == DFXL_FILLTRAPEZOID || accel == DFXL_DRAWLINE || accel == DFXL_FILLTRIANGLE  || accel == DFXL_FILLTRIANGLES || accel == DFXL_TEXTRIANGLES)
#else
    if (accel == DFXL_DRAWLINE || accel == DFXL_FILLTRIANGLE  || accel == DFXL_FILLTRIANGLES || accel == DFXL_TEXTRIANGLES)
#endif
        gfx3d = true;
#endif

    /*** 2D Functions Supported? ***/
    if (gfx3d == false)
    {
        /* Drawing Functions Supported? */
#ifdef BCMNEXUS_TRAPEZOID_SUPPORTED
        if (accel == DFXL_FILLRECTANGLE || accel == DFXL_DRAWRECTANGLE || accel == DFXL_FILLTRAPEZOID)
#else
        if (accel == DFXL_FILLRECTANGLE || accel == DFXL_DRAWRECTANGLE)
#endif
        {
            if (pState->drawingflags & ~bcmnexus_gfx_SUPPORTED_DRAWINGFLAGS)
            { /* there is some flag we do not support */
                return;
            }

            if (pState->drawingflags & DSDRAW_BLEND)
            {
#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
#ifndef BCMNEXUS_GFX_PACKET_BUFFER
                if ((pState->src_blend == DSBF_ONE) && (pState->dst_blend == DSBF_ONE))
                { /* Nexus does NOT DirectFB Porter-Duff Add operation */
                    return;
                }
#endif
#else
                if ((pState->src_blend != DSBF_SRCALPHA) || (pState->dst_blend != DSBF_INVSRCALPHA))
                { /* Nexus does NOT support any other blending mode for fill than Cc*Sa + (1-Sa)*Dc */
                    return;
                }
#endif
            }
            /* We don't support any drawing flags other than NOFX and DST_COLORKEY when destination
               surface is indexed. */
            if (DFB_PIXELFORMAT_IS_INDEXED(pState->destination->config.format) &&
                (pState->drawingflags != DSDRAW_DST_COLORKEY) && (pState->drawingflags != DSDRAW_NOFX))
            {
                    D_WARN("bcmNexus/Graphics: Cannot perform desired drawing function on a indexed destination surface.");
                return;
            }

            pState->accel |= bcmnexus_gfx_SUPPORTED_DRAWINGFUNCTIONS;
        }
        /* Blit Functions Supported? */
        else if (accel == DFXL_BLIT || accel == DFXL_BLIT2 || accel == DFXL_STRETCHBLIT)
        {
            if (pState->blittingflags & ~bcmnexus_gfx_SUPPORTED_BLITTINGFLAGS)
            { /* there is some flag we do not support */
                return;
            }

            /* Check source surface to ensure it is valid */
            if (bcmnexus_getNexusPixelFormat(pState->source->config.format) == NEXUS_PixelFormat_eUnknown)
            {
                return;
            }

            /* We only support blitting from source to destination when palette's are the same.  All other
               blending operations are not supported in the h/w. We don't support any stretch blits either! */
            if (DFB_PIXELFORMAT_IS_INDEXED(pState->destination->config.format) &&
                ((accel == DFXL_STRETCHBLIT) || (pState->blittingflags &
                ~(DSBLIT_ROTATE180 | DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL)) ||
                (pState->source->palette && !dfb_palette_equal(pState->source->palette, pState->destination->palette))))
            {
                return;
            }

            if (bcmnexus_gfxCheckStateBlitBlend(pBdrv, pState) == true)
            {
                pState->accel |= bcmnexus_gfx_SUPPORTED_BLITTINGFUNCTIONS;
            }
        }
    }
#ifdef BCMNEXUS_GFX_HAS_P3D
    /*** 3D Functions Supported? ***/
    else
    {
        /* Drawing Functions Supported? */
#ifdef BCMNEXUS_TRAPEZOID_SUPPORTED
        if (accel == DFXL_FILLRECTANGLE || accel == DFXL_DRAWRECTANGLE || accel == DFXL_FILLTRAPEZOID ||
            accel == DFXL_DRAWLINE      || accel == DFXL_FILLTRIANGLE  || accel == DFXL_FILLTRIANGLES)
#else
        if (accel == DFXL_FILLRECTANGLE || accel == DFXL_DRAWRECTANGLE ||
            accel == DFXL_DRAWLINE      || accel == DFXL_FILLTRIANGLE  || accel == DFXL_FILLTRIANGLES)
#endif
        {
            /* We need to fall back to using M2MC if the destination format is not supported by PX3D. */
            switch (pState->destination->config.format) {
                case DSPF_ARGB:
                case DSPF_RGB32:
                case DSPF_RGB16:
                case DSPF_ARGB4444:
                case DSPF_ARGB1555:
                    pBdrv->gfx_3d_support = true;
                    break;

                default:
                    if (pState->drawingflags & DSDRAW_BLEND)
                    {
#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
#ifndef BCMNEXUS_GFX_PACKET_BUFFER
                        if ((pState->src_blend == DSBF_ONE) && (pState->dst_blend == DSBF_ONE))
                        { /* Nexus does NOT DirectFB Porter-Duff Add operation */
                            return;
                        }
#endif
#else
                        if ((pState->src_blend != DSBF_SRCALPHA) || (pState->dst_blend != DSBF_INVSRCALPHA))
                        { /* Nexus does NOT support any other blending mode for fill than Cc*Sa + (1-Sa)*Dc */
                            return;
                        }
#endif
                    }
                    /* We don't support any drawing flags other than NOFX and DST_COLORKEY when destination
                       surface is indexed. */
                    if (DFB_PIXELFORMAT_IS_INDEXED(pState->destination->config.format) &&
                        (pState->drawingflags != DSDRAW_DST_COLORKEY) && (pState->drawingflags != DSDRAW_NOFX))
                    {
                        D_WARN("bcmNexus/Graphics: Cannot perform desired drawing function on a indexed destination surface.");
                        return;
                    }

                    /* PX3D doesn't support as many pixel formats as M2MC, so that we use the M2MC core
                       instead of the PX3D core in this case */
                    pBdrv->gfx_3d_support = false;
                    break;
            }
            /* Return if unsupported blitting flags are set. */
            if (pState->drawingflags & ~bcmnexus_gfx_p3d_SUPPORTED_DRAWINGFLAGS) {
                D_WARN("%s() 3D h/w cannot support accel=0x%08x, drawingflags=0x%08x", __FUNCTION__, accel, pState->drawingflags);
                return;
            }

            /* Enable acceleration of the function. */
            pState->accel |= accel;
        }
        /* Blit Functions Supported? */
        else if (accel == DFXL_BLIT || accel == DFXL_STRETCHBLIT || accel == DFXL_TEXTRIANGLES)
        {
          /* Return if the source format is not supported. */
          switch (pState->source->config.format) {
               case DSPF_ARGB:
               case DSPF_RGB32:
               case DSPF_RGB16:
               case DSPF_ARGB4444:
               case DSPF_ARGB1555:
                    break;

               default:
                    D_WARN("%s() 3D h/w cannot support %s source pixel format!", __FUNCTION__,
                            dfb_pixelformat_name(pState->source->config.format));
                    return;
          }

          /* Return if the destination format is not supported. */
          switch (pState->destination->config.format) {
               case DSPF_ARGB:
               case DSPF_RGB32:
               case DSPF_RGB16:
               case DSPF_ARGB4444:
               case DSPF_ARGB1555:
                    break;

               default:
                    D_WARN("%s() 3D h/w cannot support %s destination pixel format!", __FUNCTION__,
                            dfb_pixelformat_name(pState->destination->config.format));
                    return;
          }

          /* Return if unsupported blitting flags are set. */
            if (pState->blittingflags & ~bcmnexus_gfx_p3d_SUPPORTED_BLITTINGFLAGS) {
                //D_WARN("%s() 3D h/w cannot support accel=0x%08x, blittingflags=0x%08x", __FUNCTION__, accel, pState->blittingflags);
                return;
            }

            /* Enable acceleration of the function. */
            pState->accel |= accel;
        }
     }
#endif
}

/* Setup the source color key block */
static __inline__ void bcmnexus_gfxSetStateM2mcSourceColorkey(         bcmnexus_gfxDriverData *pBdrv,
                                                               UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                                       CardState              *pState)
{
    NEXUS_Pixel color;
    unsigned    index;
    u16          a, src_c2, src_c1, src_c0, dst_c2, dst_c1, dst_c0;

    if (BCMNEXUS_IS_VALID( m_SrcColorKey ))
        return;

    /* set the destination colorkey for fill rect and blit */
    /* We know we are setting up for BLIT or StretchBLIT */
    if (pState->blittingflags & DSBLIT_SRC_COLORKEY)
    {
        /* Disable Source Color Keying if surface format is A8, A0, etc. */
        if ((pState->source != NULL) && DFB_COLOR_BITS_PER_PIXEL(pState->source->config.format))
        {
            if (!DFB_PIXELFORMAT_IS_INDEXED(pState->source->config.format))
            {
                /* Convert colorkey from src colorspace to dest colorspace and make an RGB or YUV
                   pixel according to the destination colorspace. */
                a = src_c2 = src_c1 = src_c0 = 0;
                dfb_pixel_to_components(pState->source->config.format, pState->src_colorkey,
                    (u8*)&a, (u8*)&src_c2, (u8*)&src_c1, (u8*)&src_c0);
                dfb_convert_colorspace(pState->source->config.colorspace, src_c2, src_c1, src_c0,
                    pState->destination->config.colorspace, &dst_c2, &dst_c1, &dst_c0);
                if (DFB_COLOR_IS_YUV(pState->destination->config.format))
                    color = PIXEL_AYUV(0xff, dst_c2, dst_c1, dst_c0);
                else
                    color = PIXEL_ARGB(0xff, dst_c2, dst_c1, dst_c0);
            }
            else
            {
                if (!DFB_PIXELFORMAT_IS_INDEXED(pState->destination->config.format))
                {
                    /* If source surface format is indexed and destination surface format isn't, then we need to look
                       up the color in the palette in order for the source color key operation to work properly.
                    */
                    CorePalette *palette = pState->source->palette;

                    index = (unsigned) pState->src_colorkey;
                    color = PIXEL_ARGB( palette->entries[index].a, palette->entries[index].r,
                        palette->entries[index].g, palette->entries[index].b);
                }
                else
                {
                    /* If both source and destination surfaces are indexed, then we need to get the source key
                       color in the source palette and use this to find the index in the destination palette
                       in order for the source color key operation to work properly.
                    */
                    CorePalette *src_palette = pState->source->palette;
                    CorePalette *dst_palette = pState->destination->palette;

                    index = (unsigned) pState->src_colorkey;
                    color = dfb_palette_search(dst_palette, src_palette->entries[index].r, src_palette->entries[index].g,
                        src_palette->entries[index].b, src_palette->entries[index].a);
                }
            }
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
            bcmnexus_gfxPktSetSourceColorkeyPacket(&pBdrv->packetSettings, pBdev, true, color);
#else
            pBdrv->blitSettings.colorKey.source.lower =
            pBdrv->blitSettings.colorKey.source.upper = color;
            pBdrv->blitSettings.colorKey.source.mask = RGB_MASK;
            pBdrv->blitSettings.colorKey.source.replace = 0;
            pBdrv->blitSettings.colorKey.source.replaceMask = 0;
            pBdrv->blitSettings.colorKey.source.enabled = true;
#endif
        }
    }
    else
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetSourceColorkeyPacket(&pBdrv->packetSettings, pBdev, false, 0);
#else
        pBdrv->blitSettings.colorKey.source.enabled = false;
#endif
    }
    BCMNEXUS_VALIDATE( m_SrcColorKey );
}

/* Setup the destination color key block */
static __inline__ void bcmnexus_gfxSetStateM2mcDestColorkey(         bcmnexus_gfxDriverData *pBdrv,
                                                             UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                                     CardState              *pState)
{
    NEXUS_Pixel color;
    u8          a, c2, c1, c0;

    if (BCMNEXUS_IS_VALID( m_DstColorKey ))
        return;

    /* set the destination colorkey for fill rect and blit */
    if (pState->blittingflags & DSBLIT_DST_COLORKEY)
    {
        if (!DFB_PIXELFORMAT_IS_INDEXED(pState->destination->config.format))
        {
            dfb_pixel_to_components(pState->destination->config.format, pState->dst_colorkey, &a, &c2, &c1, &c0);
            if (DFB_COLOR_IS_YUV(pState->destination->config.format))
                color = PIXEL_AYUV(0xff, c2, c1, c0);
            else
                color = PIXEL_ARGB(0xff, c2, c1, c0);
        }
        else
            color = pState->dst_colorkey;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetDestinationColorkeyPacket(&pBdrv->packetSettings, pBdev, true, color);
#else
        pBdrv->blitSettings.colorKey.dest.enabled = true;
        pBdrv->blitSettings.colorKey.dest.lower =
        pBdrv->blitSettings.colorKey.dest.upper = color;
        pBdrv->blitSettings.colorKey.dest.mask = RGB_MASK;
        pBdrv->blitSettings.colorKey.dest.replace = 0;
        pBdrv->blitSettings.colorKey.dest.replaceMask = 0;
#endif
    }
    else
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetDestinationColorkeyPacket(&pBdrv->packetSettings, pBdev, false, 0);
#else
        pBdrv->blitSettings.colorKey.dest.enabled = false;
#endif
    }
    BCMNEXUS_VALIDATE( m_DstColorKey );
}

/* Setup the source alpha premultiplication scaler block if available */
static __inline__ void bcmnexus_gfxSetStateM2mcSourcePremultiply( UNUSED_ bcmnexus_gfxDriverData *pBdrv,
                                                                  UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                                  UNUSED_ CardState              *pState )
{
#ifdef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
    if (BCMNEXUS_IS_VALID( m_AlphaPremult ))
        return;

    /* Setup Alpha Premultiplication State
       NOTE: DSBLIT_SRC_PREMULTIPLY|DSBLIT_BLEND_COLORALPHA blending equation is:
             Oc=(Sc.Ca).Ca + (1 - Ca).Dc
             Oa=Ca.Ca + (1 - Ca).Da
             in this case we don't use the alpha premultiply block in the scaler, but instead use the color matrix. */
    bool bEnable = (pState->blittingflags & DSBLIT_SRC_PREMULTIPLY) &&
        ((pState->blittingflags & (DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL)) != DSBLIT_BLEND_COLORALPHA);
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    bcmnexus_gfxPktSetAlphaPremultiplyPacket(&pBdrv->packetSettings, pBdev, bEnable);
#else
    pBdrv->blitSettings.alphaPremultiplySourceEnabled = bEnable;
#endif
    BCMNEXUS_VALIDATE( m_AlphaPremult );
#endif
}

/* Setup the blend block */
static __inline__ void bcmnexus_gfxSetStateM2mcBlend(         bcmnexus_gfxDriverData *pBdrv,
                                                      UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                      UNUSED_ CardState              *pState )
{
    if (BCMNEXUS_IS_VALID( m_BlendEq ))
        return;

    if (pState->blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA))
    {
        /* Blending Formula is:
           color[result] = color[SRC]' * alpha[SRC]' + color[DST]' * (1 - alpha[SRC]')
           alpha[result] = alpha[SRC]' * alpha[SRC]' + alpha[DST]' * (1 - alpha[SRC]')
        */
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        BM2MC_PACKET_Blend ColorBlendEq ={BlendFunctionTable[pState->dst_blend].color, NEXUS_BlendFactor_eDestinationColor, 0,
                                          BlendFunctionTable[pState->src_blend].color, NEXUS_BlendFactor_eSourceColor, 0,
                                          NEXUS_BlendFactor_eZero};
        BM2MC_PACKET_Blend AlphaBlendEq ={BlendFunctionTable[pState->dst_blend].alpha, NEXUS_BlendFactor_eDestinationAlpha, 0,
                                          BlendFunctionTable[pState->src_blend].alpha, NEXUS_BlendFactor_eSourceAlpha, 0,
                                          NEXUS_BlendFactor_eZero};
        bcmnexus_gfxPktSetBlendPacket(pBdrv, pBdev, &ColorBlendEq, &AlphaBlendEq);

#else
        struct NEXUS_BlendEquation be;  /* Blend Equation */

        pBdrv->blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
        be.a = BlendFunctionTable[pState->dst_blend].color;
        be.b = NEXUS_BlendFactor_eDestinationColor;
        be.subtract_cd = 0;
        be.c = BlendFunctionTable[pState->src_blend].color;
        be.d = NEXUS_BlendFactor_eSourceColor;
        be.subtract_e = 0;
        be.e = NEXUS_BlendFactor_eZero;
        pBdrv->blitSettings.colorBlend = be;

        pBdrv->blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;
        be.a = BlendFunctionTable[pState->dst_blend].alpha;
        be.b = NEXUS_BlendFactor_eDestinationAlpha;
        be.subtract_cd = 0;
        be.c = BlendFunctionTable[pState->src_blend].alpha;
        be.d = NEXUS_BlendFactor_eSourceAlpha;
        be.subtract_e = 0;
        be.e = NEXUS_BlendFactor_eZero;
        pBdrv->blitSettings.alphaBlend = be;
#endif
    }
#ifndef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
    /* If not using the blend block already and we need to do an alpha premultiplication
       then we can use the blend block for this (premultiply done in color matrix for A8)
    */
    else if ((pState->blittingflags & DSBLIT_SRC_PREMULTIPLY) && !bcmnexus_isAlphaOnly(pBdrv->sourcePixelFormat))
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        BM2MC_PACKET_Blend ColorBlendEq ={NEXUS_BlendFactor_eSourceAlpha, NEXUS_BlendFactor_eSourceColor, 0,
                                          NEXUS_BlendFactor_eZero, NEXUS_BlendFactor_eZero, 0,
                                          NEXUS_BlendFactor_eZero};
        NEXUS_BlitAlphaOp alphaOp = NEXUS_BlitAlphaOp_eCopySource;

        if (!DFB_PIXELFORMAT_HAS_ALPHA(pState->source->config.format) &&
            !DFB_PIXELFORMAT_IS_INDEXED(pState->source->config.format))
        {
            pBdrv->packetSettings.packetConstantColor = PIXEL_ARGB(0xFF, 0x00, 0x00, 0x00);
            alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
        }

        bcmnexus_gfxPktSetBlendPacket(pBdrv, pBdev, &ColorBlendEq, &alphaEquation_BlitOp[alphaOp]);

#else
        struct NEXUS_BlendEquation be;  /* Blend Equation */

        pBdrv->blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
        be.a = NEXUS_BlendFactor_eSourceAlpha;
        be.b = NEXUS_BlendFactor_eSourceColor;
        be.subtract_cd = 0;
        be.c = NEXUS_BlendFactor_eZero;
        be.d = NEXUS_BlendFactor_eZero;
        be.subtract_e = 0;
        be.e = NEXUS_BlendFactor_eZero;
        pBdrv->blitSettings.colorBlend = be;
#endif
    }
#endif
    else
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        NEXUS_BlitAlphaOp alphaOp = NEXUS_BlitAlphaOp_eCopySource;

        if (!DFB_PIXELFORMAT_HAS_ALPHA(pState->source->config.format) &&
            !DFB_PIXELFORMAT_IS_INDEXED(pState->source->config.format))
        {
            pBdrv->packetSettings.packetConstantColor = PIXEL_ARGB(0xFF, 0x00, 0x00, 0x00);
            alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
        }
        bcmnexus_gfxPktSetBlendPacket(pBdrv, pBdev, &colorEquation_BlitOp[NEXUS_BlitColorOp_eCopySource],
                                                    &alphaEquation_BlitOp[alphaOp]);

#else
        /*** Setup Blend Block ***/
        pBdrv->blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        pBdrv->blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
#endif
    }
    BCMNEXUS_VALIDATE( m_BlendEq );
}

/* Setup Constant Color in Source Feeder Block */
static __inline__ void bcmnexus_gfxSetStateM2mcConstantColor(bcmnexus_gfxDriverData *pBdrv,
                                                             CardState              *pState)
{
    pBdrv->sourcePixelFormat = bcmnexus_getNexusPixelFormat(pState->source->config.format);
    if (pState->mod_hw & SMF_COLOR)
    {
        /* For formats that don't have an alpha value, ensure the constant colour block provides the fixed
           alpha value. */
        if (!DFB_PIXELFORMAT_HAS_ALPHA(pState->source->config.format) )
        {
            pState->color.a = 0xff;
        }
        /* set the blend color */
        switch (pState->source->config.format) {
            case DSPF_AYUV:
            case DSPF_YUY2:
            case DSPF_UYVY:
            {
                int y,cb,cr;

                RGB_TO_YCBCR( 0xFF, 0xFF, 0xFF, y, cb, cr );
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
                pBdrv->packetSettings.packetConstantColor = PIXEL_AYUV(pState->color.a, y, cb, cr);
#else
                pBdrv->blitSettings.constantColor = PIXEL_AYUV(pState->color.a, y, cb, cr);
#endif
                break;
            }

            default:
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
                pBdrv->packetSettings.packetConstantColor = PIXEL_ARGB(pState->color.a, 0xFF, 0xFF, 0xFF);
#else
                pBdrv->blitSettings.constantColor = PIXEL_ARGB(pState->color.a, 0xFF, 0xFF, 0xFF);
#endif
                break;
        }
    }
}

/* Setup the Source color matrix block */
static __inline__ void bcmnexus_gfxSetStateM2mcSourceColorMatrix(         bcmnexus_gfxDriverData *pBdrv,
                                                                  UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                                          CardState              *pState)
{
    u32      a, ca, A, C2, C1, C0;
    u16      c2, c1, c0;
    bool     constColorAlpha;
    s32     *pCoeffMatrix;
    bool     colorspacesDiffer;
    bcmnexus_ColorSpace sourceColorSpace, destColorSpace;

    if (BCMNEXUS_IS_VALID( m_ColorMatrix ))
        return;

    /* setup matrix */
    constColorAlpha = false;

    pCoeffMatrix = &pBdrv->blitSettings.conversionMatrix.coeffMatrix[0];
    sourceColorSpace = cs_to_index(pState->source->config.colorspace);
    destColorSpace = cs_to_index(pState->destination->config.colorspace);
    colorspacesDiffer = (sourceColorSpace != destColorSpace) &&
                        (bcmnexus_CSCMatrix[sourceColorSpace][destColorSpace] != NULL);
    if (colorspacesDiffer)
    {
        memcpy(pCoeffMatrix, bcmnexus_CSCMatrix[sourceColorSpace][destColorSpace]->coeffMatrix,
            sizeof(pBdrv->blitSettings.conversionMatrix.coeffMatrix));
    }
    else {
        memset(pCoeffMatrix, 0, sizeof(pBdrv->blitSettings.conversionMatrix.coeffMatrix));
    }
    pBdrv->blitSettings.conversionMatrix.shift = BCMNEXUS_COLOR_MATRIX_SHIFT;

    A  = 256;
    C2 = 256;
    C1 = 256;
    C0 = 256;

    a = pState->color.a;
    ca = a + 1;

    if (pState->blittingflags & DSBLIT_COLORIZE)
    {
        dfb_convert_colorspace(DSCS_RGB, (u16)(pState->color.r+1), (u16)(pState->color.g+1), (u16)(pState->color.b+1),
            pState->source->config.colorspace, &c2, &c1, &c0);
        C2 = c2;
        C1 = c1;
        C0 = c0;
    }

    if (pState->blittingflags & DSBLIT_SRC_PREMULTCOLOR)
    {
        C2 = (C2 * ca) >> 8;
        C1 = (C1 * ca) >> 8;
        C0 = (C0 * ca) >> 8;
    }

    if (pState->blittingflags & DSBLIT_BLEND_COLORALPHA)
    {
        uint ca2 = ca;

        if ((pState->blittingflags & DSBLIT_BLEND_ALPHACHANNEL) == 0)
        {
            constColorAlpha = true;
        } else if (bcmnexus_isAlphaOnly(pBdrv->sourcePixelFormat)) {
            ca2 = MIN(ca + 1, 256);   /* To match s/w */
        }

        A = ca;
        if (pState->blittingflags & DSBLIT_SRC_PREMULTIPLY)
        {
            C2 = (C2 * ca2) >> 8;
            C1 = (C1 * ca2) >> 8;
            C0 = (C0 * ca2) >> 8;
        }
    }

#ifndef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
    /* For A8 surfaces, we can use the Alpha Input channel to perform the Alpha Premultiplication. */
    if ((pState->blittingflags & DSBLIT_SRC_PREMULTIPLY) && bcmnexus_isAlphaOnly(pBdrv->sourcePixelFormat))
    {
        pCoeffMatrix[3]  = (C2 << pBdrv->blitSettings.conversionMatrix.shift) >> 8;
        pCoeffMatrix[8]  = (C1 << pBdrv->blitSettings.conversionMatrix.shift) >> 8;
        pCoeffMatrix[13] = (C0 << pBdrv->blitSettings.conversionMatrix.shift) >> 8;
    }
    else
#endif
    {
        if (colorspacesDiffer)
        {
            pCoeffMatrix[0]  = (pCoeffMatrix[0]  * C2) >> 8;
            pCoeffMatrix[1]  = (pCoeffMatrix[1]  * C1) >> 8;
            pCoeffMatrix[2]  = (pCoeffMatrix[2]  * C0) >> 8;
            pCoeffMatrix[5]  = (pCoeffMatrix[5]  * C2) >> 8;
            pCoeffMatrix[6]  = (pCoeffMatrix[6]  * C1) >> 8;
            pCoeffMatrix[7]  = (pCoeffMatrix[7]  * C0) >> 8;
            pCoeffMatrix[10] = (pCoeffMatrix[10] * C2) >> 8;
            pCoeffMatrix[11] = (pCoeffMatrix[11] * C1) >> 8;
            pCoeffMatrix[12] = (pCoeffMatrix[12] * C0) >> 8;
            pCoeffMatrix[18] = (pCoeffMatrix[18] * A) >> 8;
        }
        else {
            pCoeffMatrix[0]  = (C2 << pBdrv->blitSettings.conversionMatrix.shift) >> 8;
            pCoeffMatrix[6]  = (C1 << pBdrv->blitSettings.conversionMatrix.shift) >> 8;
            pCoeffMatrix[12] = (C0 << pBdrv->blitSettings.conversionMatrix.shift) >> 8;
            pCoeffMatrix[18] = (A  << pBdrv->blitSettings.conversionMatrix.shift) >> 8;
        }
    }
    if (constColorAlpha)
    {
        pCoeffMatrix[18] = 0;
        pCoeffMatrix[19] = A << pBdrv->blitSettings.conversionMatrix.shift;  /* Shift left by conversion matrix shift amount */
    }

    if (A != 256 || C2 != 256 || C1 != 256 || C0 != 256 || constColorAlpha || colorspacesDiffer)
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        pBdrv->packetSettings.packetSetRGBYUVColorMatrix = true;
        bcmnexus_gfxPktSetColorMatrixPacket(&pBdrv->packetSettings, pBdev, &pBdrv->blitSettings.conversionMatrix);
#else
        pBdrv->blitSettings.conversionMatrixEnabled = true;
#endif
    }
    else
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetColorMatrixPacket(&pBdrv->packetSettings, pBdev, 0);
        pBdrv->packetSettings.packetSetRGBYUVColorMatrix = false;
#else
        pBdrv->blitSettings.conversionMatrixEnabled = false;
#endif
    }
    BCMNEXUS_VALIDATE( m_ColorMatrix );
}

/* set the source surface. */
static __inline__ void bcmnexus_gfxSetStateM2mcSource(         bcmnexus_gfxDriverData *pBdrv,
                                                       UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                               CardState              *pState)
{
    NEXUS_SurfaceHandle hSur;

    if (BCMNEXUS_IS_VALID( m_Source ))
        return;

    hSur = pState->src.handle;

    if (!hSur)
    {  /* this is always an error */
        D_DEBUG_AT( bcmnexusGraphics, "%s() NO SURFACE\n", __FUNCTION__ );
        return;
    }

    /* Setup the palette if the source is palettised... */
    if (DFB_PIXELFORMAT_IS_INDEXED(pState->source->config.format) && pState->source->palette)
    {
        copy_palette(pState->source, hSur);
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetSourcePalettePacket(&pBdrv->packetSettings, pState->src.allocation->data);
#endif
    }
    /* set the source surface */
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    bcmnexus_gfxPktSetSourceFeederPacket(&pBdrv->packetSettings, pBdev, pState->src.allocation->data, pBdrv->packetSettings.packetConstantColor);
#else
    pBdrv->blitSettings.source.surface = hSur;
#endif
    BCMNEXUS_VALIDATE( m_Source );
}

/* Setup the destination and output surfaces */
static __inline__ void bcmnexus_gfxSetStateM2mcDestination(         bcmnexus_gfxDriverData *pBdrv,
                                                            UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                                    CardState              *pState,
                                                                    DFBAccelerationMask     accel )
{
    bool blitDest, dithering = false;

    /* set the destination surface. If bOutputOnly is set, we will not set the
       destination surface in the hGrc */
    NEXUS_SurfaceHandle hSur;

    /* lookup surface - this should mark it as recently used */
    hSur = pState->dst.handle;
    if (!hSur)
    {  /* this is always an error */
        return;
    }

    if (!BCMNEXUS_IS_VALID( m_Destination ))
    {
        /* Setup the palette if the destination is palettised... */
        if (DFB_PIXELFORMAT_IS_INDEXED(pState->destination->config.format) && pState->destination->palette)
        {
            copy_palette(pState->destination, hSur);
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
            bcmnexus_gfxPktSetSourcePalettePacket(&pBdrv->packetSettings, pState->dst.allocation->data);
#endif
        }

        /* setup the destination/output surface */
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        blitDest = ( accel == DFXL_BLIT2                           ||
                     pBdev->packetHWStates.packetSrcColorkeyEnable ||
                     pBdev->packetHWStates.packetDstColorkeyEnable ||
                     pBdev->packetHWStates.packetRop == 0x66       ||
                     (pState->blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)) ||
                     ((accel == DFXL_STRETCHBLIT) && (pState->blittingflags & (DSBLIT_ROTATE180 | DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL))) );

        if (blitDest)
            bcmnexus_gfxPktSetDestinationFeederPacket(&pBdrv->packetSettings,pBdev,
                                                      (accel == DFXL_BLIT2) ? pState->src2.allocation->data : pState->dst.allocation->data,
                                                      pBdrv->packetSettings.packetConstantColor);
        else
            bcmnexus_gfxPktSetDestinationFeederNonePacket(&pBdrv->packetSettings, pBdev);
#else
        blitDest = (pBdrv->blitSettings.colorKey.source.enabled ||
                    pBdrv->blitSettings.colorKey.dest.enabled   ||
                    pBdrv->blitSettings.patternSettingsEnabled  ||
                    (pState->blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)) );

        pBdrv->blitSettings.dest.surface = (blitDest) ? ((accel == DFXL_BLIT2) ? pState->src2.handle : hSur) : NULL;
#endif
        BCMNEXUS_VALIDATE( m_Destination );
    }

    if (!BCMNEXUS_IS_VALID( m_Output ))
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetOutputFeederPacket(&pBdrv->packetSettings, pBdev, pState->dst.allocation->data);
#else
        pBdrv->blitSettings.output.surface = hSur;
#endif
#if BCMNEXUS_HW_DITHERING
        /* dithering enabled as blitting ARGB8888 to ARGB4444 or RGB565 */
        dithering = ( (pState->destination->config.format == DSPF_ARGB4444 ||
                       pState->destination->config.format == DSPF_RGB16) &&
                       pState->source->config.format == DSPF_ARGB );
#endif

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetOutputControlPacket(&pBdrv->packetSettings, pBdev, dithering, true);
#endif

        BCMNEXUS_VALIDATE( m_Output );
    }
}

/* Setup the pattern block to perform XOR raster operation */
static __inline__ void bcmnexus_gfxSetStateM2mcXOR( UNUSED_ bcmnexus_gfxDriverData *pBdrv,
                                                    UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                    UNUSED_ CardState              *pState )
{
#ifdef BCMNEXUS_ROP_SUPPORTED
    if (BCMNEXUS_IS_VALID( m_Rop ))
        return;

    pBdrv->blitSettings.patternSettingsEnabled = false;

    /* setup the destination/output surface */
    if (pState->blittingflags & DSBLIT_XOR)
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetRopPacket(&pBdrv->packetSettings, pBdev, 0x66);
#else
        pBdrv->blitSettings.patternSettingsEnabled = true;
        pBdrv->blitSettings.patternSettings.ropVector = 0x66;    /* SRC INVERT ROP */
#endif
    }
    else
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        bcmnexus_gfxPktSetRopPacket(&pBdrv->packetSettings, pBdev, 0xCC);
#else
        pBdrv->blitSettings.patternSettingsEnabled = false;
#endif
    }
    BCMNEXUS_VALIDATE( m_Rop );
#endif
}

/* Setup the mirror block to perform rotation and flips */
static __inline__ void bcmnexus_gfxSetStateMirror( UNUSED_ bcmnexus_gfxDriverData *pBdrv,
                                                   UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                   UNUSED_ CardState              *pState )
{
#ifdef BCMNEXUS_MIRRORING_SUPPORTED
    bool bMirrorSrcVertical;
    bool bMirrorSrcHorizontal;
    bool bMirrorDstVertical;
    bool bMirrorDstHorizontal;

    if (BCMNEXUS_IS_VALID( m_Mirror ))
        return;

    bMirrorSrcVertical   = false;
    bMirrorSrcHorizontal = false;
    bMirrorDstVertical   = false;
    bMirrorDstHorizontal = false;

    if (pState->blittingflags & DSBLIT_ROTATE180)
    {
        bMirrorSrcVertical   = true;
        bMirrorSrcHorizontal = true;
    } else {
        if (pState->blittingflags & DSBLIT_FLIP_HORIZONTAL)
            bMirrorSrcHorizontal = true;

        if (pState->blittingflags & DSBLIT_FLIP_VERTICAL)
            bMirrorSrcVertical = true;
    }

    if (pBdrv->matrix_flips_x)
        bMirrorSrcHorizontal = !bMirrorSrcHorizontal;

    if (pBdrv->matrix_flips_y)
        bMirrorSrcVertical = !bMirrorSrcVertical;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    bcmnexus_gfxPktSetMirrorPacket(&pBdrv->packetSettings, pBdev, bMirrorSrcHorizontal, bMirrorSrcVertical,bMirrorDstHorizontal, bMirrorDstVertical);
#else
    pBdrv->blitSettings.mirrorSourceVertically = bMirrorSrcVertical;
    pBdrv->blitSettings.mirrorSourceHorizontally = bMirrorSrcHorizontal;
    pBdrv->blitSettings.mirrorDestVertically = bMirrorDstVertical;
    pBdrv->blitSettings.mirrorDestHorizontally = bMirrorDstHorizontal;
#endif
    BCMNEXUS_VALIDATE( m_Mirror );
#endif
}

/* Setup the filter in the source scaler block */
static __inline__ void bcmnexus_gfxSetStateRenderOptions(         bcmnexus_gfxDriverData *pBdrv,
                                                          UNUSED_ CardState              *pState,
                                                          UNUSED_ bool                    stretch )
{
#if defined(CONFIG_NETFLIX_SUPPORT) /* NETFLIX PATCH */
    NEXUS_Graphics2DFilterCoeffs horizontalFilter;
    NEXUS_Graphics2DFilterCoeffs verticalFilter;

    horizontalFilter = NEXUS_Graphics2DFilterCoeffs_ePointSample;
    verticalFilter   = NEXUS_Graphics2DFilterCoeffs_ePointSample;

    /* Always use point-sample filtering for LUT based pixel formats and
       when not wanting smooth scaling */
    if (!DFB_PIXELFORMAT_IS_INDEXED(pState->source->config.format))
    {
        if (stretch)
        {
            if (pState->render_options & (DSRO_SMOOTH_UPSCALE|DSRO_SMOOTH_DOWNSCALE))
            {
                horizontalFilter = NEXUS_Graphics2DFilterCoeffs_eBilinear;
                verticalFilter   = NEXUS_Graphics2DFilterCoeffs_eBilinear;
            }
        }
        else
        {
            horizontalFilter = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
            verticalFilter   = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
        }
    }

#else

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SDOUT_NTSC_JAPAN)
    NEXUS_Graphics2DFilterCoeffs horizontalFilter = NEXUS_Graphics2DFilterCoeffs_ePointSample;
    NEXUS_Graphics2DFilterCoeffs verticalFilter   = NEXUS_Graphics2DFilterCoeffs_ePointSample;
#elif defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SDOUT_PAL)
    NEXUS_Graphics2DFilterCoeffs horizontalFilter = NEXUS_Graphics2DFilterCoeffs_eAntiFlutterSharp;
    NEXUS_Graphics2DFilterCoeffs verticalFilter   = NEXUS_Graphics2DFilterCoeffs_eAntiFlutterSharp;
#else
    NEXUS_Graphics2DFilterCoeffs horizontalFilter;
    NEXUS_Graphics2DFilterCoeffs verticalFilter;

    horizontalFilter = NEXUS_Graphics2DFilterCoeffs_ePointSample;
    verticalFilter   = NEXUS_Graphics2DFilterCoeffs_ePointSample;

    /* Always use point-sample filtering for LUT based pixel formats and
       when not wanting smooth scaling */
    if (!DFB_PIXELFORMAT_IS_INDEXED(pState->source->config.format))
    {
        if (stretch)
        {
            if (pState->render_options & (DSRO_SMOOTH_UPSCALE|DSRO_SMOOTH_DOWNSCALE))
            {
                horizontalFilter = NEXUS_Graphics2DFilterCoeffs_eBilinear;
                verticalFilter   = NEXUS_Graphics2DFilterCoeffs_eBilinear;
            }
        }
        else
        {
            horizontalFilter = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
            verticalFilter   = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
        }
    }
#endif
#endif

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    pBdrv->packetSettings.packetHorzFilter = horizontalFilter;
    pBdrv->packetSettings.packetVertFilter = verticalFilter;
#else
    pBdrv->blitSettings.horizontalFilter = horizontalFilter;
    pBdrv->blitSettings.verticalFilter   = verticalFilter;
#endif
}



static DFBResult bcmnexus_gfxSetStateBlit( bcmnexus_gfxDriverData *pBdrv,
                                           bcmnexus_gfxDeviceData *pBdev,
                                           CardState              *pState,
                                           bool                    bStretch,
                                           DFBAccelerationMask     accel )
{
    D_DEBUG_AT( bcmnexusGraphics, "%s()\n", __FUNCTION__ );

    /**********************************************************************\
     * finish setup for BLIT and STRETCHBLIT                                *
    \**********************************************************************/

    /* Don't want to do NEXUS fast blit */
    pBdrv->blitFast = false;

#ifndef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
    /* Setup single/dual passes through the M2MC.
       If we need to use the blender for both alpha premultiplication and alpha blending, then
       we can only do this using 2 passes in the M2MC. */
    pBdrv->blitSingle = !((pState->blittingflags & DSBLIT_SRC_PREMULTIPLY) &&
                          (pState->blittingflags & DSBLIT_BLEND_ALPHACHANNEL) &&
                          !(pState->blittingflags & (DSBLIT_SRC_MASK_ALPHA | DSBLIT_SRC_MASK_COLOR)) &&
                          !bcmnexus_isAlphaOnly(pBdrv->sourcePixelFormat));
#endif

    /* If all state flags are valid, then there's nothing to do to setup the state */
    if (BCMNEXUS_ARE_VALID( m_All ))
        return DFB_OK;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    bcmnexus_gfxEnterMode(pBdrv, pBdev, BCMNEXUS_GFX_2D, 0);

    bcmnexus_gfxPktGetPacketBuffer(pBdrv, pBdev, GFX_FULL_PACKET_SIZE);
#endif
    /*** Setup The Constant Color Blend Block ***/
    bcmnexus_gfxSetStateM2mcConstantColor(pBdrv, pState);

    /*** Setup Blend Block ***/
    bcmnexus_gfxSetStateM2mcBlend(pBdrv, pBdev, pState);

    /*** Setup Source ***/
    bcmnexus_gfxSetStateM2mcSource(pBdrv, pBdev, pState);

    /*** Setup Source and Destination Color Keying ***/
    bcmnexus_gfxSetStateM2mcSourceColorkey(pBdrv, pBdev, pState);
    bcmnexus_gfxSetStateM2mcDestColorkey(pBdrv, pBdev, pState);

    /*** Setup Pattern Block ***/
    bcmnexus_gfxSetStateM2mcXOR(pBdrv, pBdev, pState);

    /*** Setup Source Color Matrix ***/
    bcmnexus_gfxSetStateM2mcSourceColorMatrix(pBdrv, pBdev, pState);

    /*** Setup Source Alpha Premultiplication ***/
    bcmnexus_gfxSetStateM2mcSourcePremultiply(pBdrv, pBdev, pState);

    /*** Setup Mirroring ***/
    bcmnexus_gfxSetStateMirror(pBdrv, pBdev, pState);

    /*** Ensure Render Options are Set ***/
    bcmnexus_gfxSetStateRenderOptions(pBdrv, pState, bStretch);

    /*** Setup Destination ***/
    bcmnexus_gfxSetStateM2mcDestination(pBdrv, pBdev, pState, accel);
    return DFB_OK;
}

static DFBResult bcmnexus_gfxSetStateFastBlit( bcmnexus_gfxDriverData *pBdrv,
                                               bcmnexus_gfxDeviceData *pBdev,
                                               CardState              *pState,
                                               DFBAccelerationMask     accel )
{
    D_DEBUG_AT( bcmnexusGraphics, "%s()\n", __FUNCTION__ );

    /* Want to do Nexus Fast blit */
    pBdrv->blitFast = true;

    /* If all state flags are valid, then there's nothing to do to setup the state */
    if (BCMNEXUS_ARE_VALID( m_All ))
        return DFB_OK;


    /*** Setup The Constant Color Blend Block ***/
    bcmnexus_gfxSetStateM2mcConstantColor(pBdrv, pState);

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    bcmnexus_gfxEnterMode(pBdrv, pBdev, BCMNEXUS_GFX_2D, 0);

    bcmnexus_gfxPktGetPacketBuffer(pBdrv, pBdev, GFX_FULL_PACKET_SIZE);
    bcmnexus_gfxPacketSettings *pPktSettings = &pBdrv->packetSettings;
    if (!BCMNEXUS_IS_VALID( m_BlendEq ))
    {
        NEXUS_BlitAlphaOp alphaOp = NEXUS_BlitAlphaOp_eCopySource;

        if (!DFB_PIXELFORMAT_HAS_ALPHA(pState->source->config.format) &&
            !DFB_PIXELFORMAT_IS_INDEXED(pState->source->config.format))
        {
            pBdrv->packetSettings.packetConstantColor = PIXEL_ARGB(0xFF, 0x00, 0x00, 0x00);
            alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
        }

        bcmnexus_gfxPktSetBlendPacket(pBdrv, pBdev, &colorEquation_BlitOp[NEXUS_BlitColorOp_eCopySource],
                                                    &alphaEquation_BlitOp[alphaOp]);
        BCMNEXUS_VALIDATE( m_BlendEq );
    }


    if (!BCMNEXUS_IS_VALID( m_Rop ))
    {
        bcmnexus_gfxPktSetRopPacket(pPktSettings, pBdev, 0xCC);
        BCMNEXUS_VALIDATE( m_Rop );
    }
    if (!BCMNEXUS_IS_VALID( m_SrcColorKey ))
    {
        bcmnexus_gfxPktSetSourceColorkeyPacket(pPktSettings, pBdev, false, 0);
        BCMNEXUS_VALIDATE( m_SrcColorKey );
    }
    if (!BCMNEXUS_IS_VALID( m_DstColorKey ))
    {
        bcmnexus_gfxPktSetDestinationColorkeyPacket(pPktSettings, pBdev, false, 0);
        BCMNEXUS_VALIDATE( m_DstColorKey );
    }
    if (!BCMNEXUS_IS_VALID( m_ColorMatrix ))
    {
        DFBSurfaceColorSpace src_cs = pState->source->config.colorspace;
        DFBSurfaceColorSpace dst_cs = pState->destination->config.colorspace;

        if (src_cs != dst_cs)
            bcmnexus_gfxPktSetColorMatrixPacket(pPktSettings, pBdev,
                bcmnexus_CSCMatrix[cs_to_index(src_cs)][cs_to_index(dst_cs)]);
        else
            bcmnexus_gfxPktSetColorMatrixPacket(pPktSettings, pBdev, 0);

        BCMNEXUS_VALIDATE( m_ColorMatrix );
    }
    if (!BCMNEXUS_IS_VALID( m_AlphaPremult ))
    {
        bcmnexus_gfxPktSetAlphaPremultiplyPacket(pPktSettings, pBdev, false);
        BCMNEXUS_VALIDATE( m_AlphaPremult );
    }
    if (!BCMNEXUS_IS_VALID( m_Mirror ))
    {
        bcmnexus_gfxPktSetMirrorPacket(pPktSettings, pBdev, false, false, false, false);
        BCMNEXUS_VALIDATE( m_Mirror );
    }
#endif
    /*** Setup Source ***/
    bcmnexus_gfxSetStateM2mcSource(pBdrv, pBdev, pState);

    /*** Ensure Render Options are Set ***/
    bcmnexus_gfxSetStateRenderOptions(pBdrv, pState, false);

    /*** Setup Destination Last ***/
    bcmnexus_gfxSetStateM2mcDestination(pBdrv, pBdev, pState, accel);

    return DFB_OK;
}

#if defined(BCMNEXUS_PORTERDUFF_FILL_SUPPORTED) && !defined(BCMNEXUS_GFX_PACKET_BUFFER)
static NEXUS_PorterDuffOp getNexusPorterDuffOperation(NEXUS_BlendFactor src,
                                                      NEXUS_BlendFactor dst)
{
    NEXUS_PorterDuffOp op = NEXUS_PorterDuffOp_eMax;

    switch (src)
    {
        case NEXUS_BlendFactor_eZero:
        {
            switch (dst)
            {
                case NEXUS_BlendFactor_eZero:
                    op=NEXUS_PorterDuffOp_eClear;
                    break;
                case NEXUS_BlendFactor_eOne:
                    op=NEXUS_PorterDuffOp_eDst;
                    break;
                case NEXUS_BlendFactor_eSourceAlpha:
                    op=NEXUS_PorterDuffOp_eDstIn;
                    break;
                case NEXUS_BlendFactor_eInverseSourceAlpha:
                    op=NEXUS_PorterDuffOp_eDstOut;
                    break;
                default:
                    break;
            }
            break;
        }

        case NEXUS_BlendFactor_eOne:
        {
            switch (dst)
            {
                case NEXUS_BlendFactor_eZero:
                    op=NEXUS_PorterDuffOp_eSrc;
                    break;
                case NEXUS_BlendFactor_eInverseSourceAlpha:
                    op=NEXUS_PorterDuffOp_eSrcOver;
                    break;
                default:
                    break;
            }
            break;
        }

        case NEXUS_BlendFactor_eInverseDestinationAlpha:
        {
            switch (dst)
            {
                case NEXUS_BlendFactor_eZero:
                    op=NEXUS_PorterDuffOp_eSrcOut;
                    break;
                case NEXUS_BlendFactor_eOne:
                    op=NEXUS_PorterDuffOp_eDstOver;
                    break;
                case NEXUS_BlendFactor_eSourceAlpha:
                    op=NEXUS_PorterDuffOp_eDstAtop;
                    break;
                case NEXUS_BlendFactor_eInverseSourceAlpha:
                    op=NEXUS_PorterDuffOp_eXor;
                    break;
                default:
                    break;
            }
            break;
        }

        case NEXUS_BlendFactor_eDestinationAlpha:
        {
            switch (dst)
            {
                case NEXUS_BlendFactor_eZero:
                    op=NEXUS_PorterDuffOp_eSrcIn;
                    break;
                case NEXUS_BlendFactor_eInverseSourceAlpha:
                    op=NEXUS_PorterDuffOp_eSrcAtop;
                    break;
                default:
                    break;
            }
            break;
        }

        default:
            break;
    }
    return op;
}
#endif

static void bcmnexus_gfxSetStateRectangle(         bcmnexus_gfxDriverData *pBdrv,
                                           UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                                   CardState              *pState)
{
    NEXUS_SurfaceHandle                     hSur;
    NEXUS_Graphics2DFillSettings           *pFillSettings;
    uint16_t                                c2, c1, c0;
    uint                                    a, C2, C1, C0;
    uint                                    ca;
    uint                                    fillColor;

    /* If these state flags are valid, then there's nothing to do to setup the state */
    if (BCMNEXUS_ARE_VALID( m_Source | m_Destination | m_Output | m_BlendEq | m_SrcColorKey | m_DstColorKey | m_Rop))
        return;

    /**********************************************************************\
     * finish setup for FILL RECTANGLE                                      *
    \**********************************************************************/

    /* lookup output surface - this should mark it as recently used */
    hSur = pState->dst.handle;
    if (!hSur)
    {  /* this is always an error */
        return;
    }

    if (pState->destination->palette != NULL)
    {
        fillColor = pState->color_index;
    } else {
        a  = pState->color.a;
        dfb_convert_colorspace(DSCS_RGB, (u16)pState->color.r, (u16)pState->color.g, (u16)pState->color.b,
            pState->destination->config.colorspace, &c2, &c1, &c0);
        C2 = c2;
        C1 = c1;
        C0 = c0;

        /* check for premultiply */
        if (pState->drawingflags & DSDRAW_SRC_PREMULTIPLY)
        {
            /* setup the premultiplied the color */
            ca = a + 1;
            C2 =(C2*ca)>>8;
            C1 =(C1*ca)>>8;
            C0 =(C0*ca)>>8;
        }

        if (DFB_COLOR_IS_YUV(pState->destination->config.format))
            fillColor = PIXEL_AYUV(a, C2, C1, C0);
        else
            fillColor = PIXEL_ARGB(a, C2, C1, C0);
    }

    /* Set these up to help with debug */
    pFillSettings = &pBdrv->fillSettings;
    pFillSettings->color = fillColor;
    pFillSettings->surface = hSur;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    bcmnexus_gfxPacketSettings *pPktSettings = &pBdrv->packetSettings;

    bcmnexus_gfxEnterMode(pBdrv, pBdev, BCMNEXUS_GFX_2D, 0);

    bcmnexus_gfxPktGetPacketBuffer(pBdrv, pBdev, GFX_FULL_PACKET_SIZE);
    pPktSettings->packetConstantColor = fillColor;
    if (!BCMNEXUS_IS_VALID( m_Source ))
    {
        bcmnexus_gfxPktSetSourceFeederNonePacket(pPktSettings, pBdev); /* We use the destination feeder as the source */
        BCMNEXUS_VALIDATE( m_Source );
    }

    if (pState->drawingflags & DSDRAW_BLEND)
    {
        if (!BCMNEXUS_IS_VALID( m_Destination ))
        {
            bcmnexus_gfxPktSetDestinationFeederPacket(pPktSettings, pBdev, pState->dst.allocation->data, fillColor);
            BCMNEXUS_VALIDATE( m_Destination );
        }
        if (!BCMNEXUS_IS_VALID( m_BlendEq ))
        {
            BM2MC_PACKET_Blend ColorBlendEq = {BlendFillFunctionTable[pState->dst_blend].color, NEXUS_BlendFactor_eDestinationColor, 0,
                                               BlendFillFunctionTable[pState->src_blend].color, NEXUS_BlendFactor_eConstantColor, 0,
                                               NEXUS_BlendFactor_eZero};
            BM2MC_PACKET_Blend AlphaBlendEq = {BlendFillFunctionTable[pState->dst_blend].alpha, NEXUS_BlendFactor_eDestinationAlpha, 0,
                                               BlendFillFunctionTable[pState->src_blend].alpha, NEXUS_BlendFactor_eConstantAlpha, 0,
                                               NEXUS_BlendFactor_eZero};
            bcmnexus_gfxPktSetBlendPacket(pBdrv, pBdev, &ColorBlendEq, &AlphaBlendEq);
            BCMNEXUS_VALIDATE( m_BlendEq );
        }
    }
    else
    {
        if (!BCMNEXUS_IS_VALID( m_Destination ))
        {
            bcmnexus_gfxPktSetDestinationFeederNonePacket(pPktSettings, pBdev);
            BCMNEXUS_VALIDATE( m_Destination );
        }
        if (!BCMNEXUS_IS_VALID( m_BlendEq ))
        {
            bcmnexus_gfxPktSetBlendPacket(pBdrv, pBdev, &colorEquation_FillOp[1], &alphaEquation_FillOp[1]);
            BCMNEXUS_VALIDATE( m_BlendEq );
        }
    }
    if (!BCMNEXUS_IS_VALID( m_Output ))
    {
        bcmnexus_gfxPktSetOutputFeederPacket(pPktSettings, pBdev, pState->dst.allocation->data);
        BCMNEXUS_VALIDATE( m_Output );
    }
    if (!BCMNEXUS_IS_VALID( m_SrcColorKey ))
    {
        bcmnexus_gfxPktSetSourceColorkeyPacket(pPktSettings, pBdev, false, 0);
        BCMNEXUS_VALIDATE( m_SrcColorKey );
    }
    if (!BCMNEXUS_IS_VALID( m_DstColorKey ))
    {
        if(pState->drawingflags & DSDRAW_DST_COLORKEY)
        {
            NEXUS_Pixel color;

            if (!DFB_PIXELFORMAT_IS_INDEXED(pState->destination->config.format))
            {
                u8 A, C2, C1, C0;

                dfb_pixel_to_components(pState->destination->config.format, pState->dst_colorkey,
                                        &A, &C2, &C1, &C0 );
                if (DFB_COLOR_IS_YUV(pState->destination->config.format))
                    color = PIXEL_AYUV(0xff, C2, C1, C0);
                else
                    color = PIXEL_ARGB(0xff, C2, C1, C0);
            }
            else
                color = pState->dst_colorkey;

            bcmnexus_gfxPktSetDestinationColorkeyPacket(pPktSettings, pBdev, true, color);
        }
        else
        {
            bcmnexus_gfxPktSetDestinationColorkeyPacket(pPktSettings, pBdev, false, 0);
        }
        BCMNEXUS_VALIDATE( m_DstColorKey );
    }

    if (!BCMNEXUS_IS_VALID( m_Rop ))
    {
        if (pState->drawingflags & DSDRAW_XOR)
            bcmnexus_gfxPktSetRopPacket(pPktSettings, pBdev, 0x66);
        else
            bcmnexus_gfxPktSetRopPacket(pPktSettings, pBdev,0xCC);
        BCMNEXUS_VALIDATE( m_Rop );
    }
#else
#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
    NEXUS_Graphics2DPorterDuffFillSettings *pPDFillSettings = &pBdrv->PorterDuffFillSettings;
#endif

    pFillSettings->colorOp = NEXUS_FillOp_eCopy;
    pFillSettings->alphaOp = NEXUS_FillOp_eCopy;

    if (pState->drawingflags & DSDRAW_BLEND)
    {
        pFillSettings->colorOp = NEXUS_FillOp_eBlend;
        pFillSettings->alphaOp = NEXUS_FillOp_eBlend;
#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
        pPDFillSettings->color = pFillSettings->color;
        pPDFillSettings->surface = hSur;
        pPDFillSettings->operation = getNexusPorterDuffOperation(BlendFunctionTable[pState->src_blend].alpha,
                                                                 BlendFunctionTable[pState->dst_blend].alpha);
#endif
    }
#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
    else
    {
        pPDFillSettings->operation = NEXUS_PorterDuffOp_eMax;   /* Signal invalid PD fill operation */
    }
#endif
#endif
}

#ifdef BCMNEXUS_GFX_HAS_P3D
static const BP3D_Blend blend[] = {
     [DSBF_ZERO]             = BP3D_Blend_Zero,        /* cf:    0           af:    0 */
     [DSBF_ONE]              = BP3D_Blend_One,         /* cf:    1           af:    1 */
     [DSBF_SRCCOLOR]         = BP3D_Blend_SrcColor,    /* cf:   sc           af:   sa */
     [DSBF_INVSRCCOLOR]      = BP3D_Blend_InvSrcColor, /* cf: 1-sc           af: 1-sa */
     [DSBF_SRCALPHA]         = BP3D_Blend_SrcAlpha,    /* cf:   sa           af:   sa */
     [DSBF_INVSRCALPHA]      = BP3D_Blend_InvSrcAlpha, /* cf: 1-sa           af: 1-sa */
     [DSBF_DESTALPHA]        = BP3D_Blend_DstAlpha,    /* cf:   da           af:   da */
     [DSBF_INVDESTALPHA]     = BP3D_Blend_InvDstAlpha, /* cf: 1-da           af: 1-da */
     [DSBF_DESTCOLOR]        = BP3D_Blend_DstColor,    /* cf:   dc           af:   da */
     [DSBF_INVDESTCOLOR]     = BP3D_Blend_InvDstColor, /* cf: 1-dc           af: 1-da */
     [DSBF_SRCALPHASAT]      = BP3D_Blend_SrcAlphaSat, /* cf: min(sa, 1-da)  af:    1 */
};

static void
bcmnexus_gfxSetStateDraw3D( bcmnexus_gfxDriverData *pBdriver,
                            CardState              *pState)
{
     DFBSurfaceDrawingFlags   flags      = pState->drawingflags;
     CoreSurfaceBuffer       *dst_buffer = pState->dst.buffer;
     BP3D_Blend               src_blend  = BP3D_Blend_One;
     BP3D_Blend               dst_blend  = BP3D_Blend_Zero;
     BSUR_Surface_Handle      dst;
     uint                     a, r, g, b;
     uint                     ca;

     D_DEBUG_AT( bcmnexusGraphics, "%s( %p, %p ) <- flags: 0x%08x (%dx%d)\n", __FUNCTION__, (void*) pBdriver, (void*) pState, flags,
                 dst_buffer->surface->config.size.w, dst_buffer->surface->config.size.h );

     DFB_Platform_P_Graphics3D_GetBSUR(pState->dst.handle, &dst);

     BP3D_Context_SetCurrent( pBdriver->hP3d, pBdriver->hContext );

     /* destination */
     BP3D_SetDstColorSurface( pBdriver->hP3d, dst );
     BP3D_SetSrcColorSurface( pBdriver->hP3d, dst );
     BP3D_SetViewport( pBdriver->hP3d, pState->clip.x1, pState->clip.y1,
                       pState->clip.x2 - pState->clip.x1 + 1,
                       pState->clip.y2 - pState->clip.y1 + 1,
                       pState->clip.x1, pState->clip.y1 );
     BP3D_SetTextureSurface( pBdriver->hP3d, NULL );

     /* color */
     a = pState->color.a;
     r = pState->color.r;
     g = pState->color.g;
     b = pState->color.b;

     if (pState->drawingflags & DSDRAW_SRC_PREMULTIPLY)
     {
        /* setup the premultiplied the color */
        ca = a + 1;
        r=(r*ca)>>8;
        g=(g*ca)>>8;
        b=(b*ca)>>8;
     }
     pBdriver->diffuseColor = PIXEL_ARGB( a, r, g, b );

     /* blend functions */
     if (flags & DSDRAW_BLEND) {
          src_blend = blend[pState->src_blend];
          dst_blend = blend[pState->dst_blend];
     }

     BP3D_SetSrcBlend( pBdriver->hP3d, src_blend );
     BP3D_SetDstBlend( pBdriver->hP3d, dst_blend );
}

static void
bcmnexus_gfxSetStateBlit3D( bcmnexus_gfxDriverData *pBdriver,
                            CardState              *pState)
{
     DFBSurfaceBlittingFlags  flags      = pState->blittingflags;
     CoreSurfaceBuffer       *src_buffer = pState->src.buffer;
     BP3D_Blend               src_blend  = BP3D_Blend_One;
     BP3D_Blend               dst_blend  = BP3D_Blend_Zero;
     BSUR_Surface_Handle      src;
     BSUR_Surface_Handle      dst;

     D_DEBUG_AT( bcmnexusGraphics, "%s( %p, %p ) <- flags: 0x%08x\n", __FUNCTION__, (void*) pBdriver, (void*) pState, flags );

     DFB_Platform_P_Graphics3D_GetBSUR(pState->src.handle, &src);
     DFB_Platform_P_Graphics3D_GetBSUR(pState->dst.handle, &dst);

     BP3D_Context_SetCurrent( pBdriver->hP3d, pBdriver->hContext );

     /* destination */
     BP3D_SetDstColorSurface( pBdriver->hP3d, dst );
     BP3D_SetSrcColorSurface( pBdriver->hP3d, dst );
     BP3D_SetViewport( pBdriver->hP3d, pState->clip.x1, pState->clip.y1,
                       pState->clip.x2 - pState->clip.x1 + 1,
                       pState->clip.y2 - pState->clip.y1 + 1,
                       pState->clip.x1, pState->clip.y1 );

     /* color */
     pBdriver->diffuseColor = PIXEL_ARGB( pState->color.a, pState->color.r, pState->color.g, pState->color.b );

     /* blend functions */
     if (flags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)) {
          src_blend = blend[pState->src_blend];
          dst_blend = blend[pState->dst_blend];
     }

     BP3D_SetSrcBlend( pBdriver->hP3d, src_blend );
     BP3D_SetDstBlend( pBdriver->hP3d, dst_blend );

     BP3D_SetTextureSurface( pBdriver->hP3d, src );
     BP3D_SetTextureFilter( pBdriver->hP3d, BP3D_TextureFilter_Bilinear );
     BP3D_SetTextureWrap( pBdriver->hP3d, BP3D_TextureWrap_Clamp );

     pBdriver->fSrcWidth  = (float)(1 << direct_log2( pState->source->config.size.w ));
     pBdriver->fSrcHeight = (float)(1 << direct_log2( pState->source->config.size.h ));

     pBdriver->fSrcS      = pState->source->config.size.w / pBdriver->fSrcWidth;
     pBdriver->fSrcT      = pState->source->config.size.h / pBdriver->fSrcHeight;

     /* color key */
     if (flags & DSBLIT_SRC_COLORKEY) {
          u32 colorkey;

          switch (src_buffer->format) {
               case DSPF_ARGB:
               case DSPF_RGB32:
                    colorkey = pState->src_colorkey & 0xffffff;
                    break;

               case DSPF_RGB16:
                    colorkey = RGB16_TO_RGB32( pState->src_colorkey );
                    break;

               default:
                    D_BUG( "unexpected pixelformat" );
                    return;
          }

          BP3D_SetTextureColorKey( pBdriver->hP3d, true, colorkey );
     }
     else
          BP3D_SetTextureColorKey( pBdriver->hP3d, false, 0 );
}
#endif

static void
bcmnexus_gfxSetStateMatrix(         bcmnexus_gfxDriverData *pBdrv,
                            UNUSED_ bcmnexus_gfxDeviceData *pBdev,
                                    CardState              *pState )

{
    if (BCMNEXUS_IS_VALID( m_Matrix ))
        return;

    D_ASSERT( sizeof(pState->matrix) == sizeof(pBdrv->matrix) );

    direct_memcpy( pBdrv->matrix, pState->matrix, sizeof(pState->matrix) );

    pBdrv->matrix_affine = pState->affine_matrix;

    /* set information about the matrix type in driver struct */

    /* is matrix enabled? */
    if (pState->render_options & DSRO_MATRIX) {
        pBdrv->matrix_enabled = true;

        /* check if we need the 3D core */
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        if (pState->matrix[1] || pState->matrix[3] || !pState->affine_matrix)
        {
            pBdrv->matrix_3d = true;
        }
        else
#endif
        {
            /* check if it scales */
            if (((pBdrv->matrix[0] == 0x10000) || (pBdrv->matrix[0] == -0x10000)) &&
                ((pBdrv->matrix[4] == 0x10000) || (pBdrv->matrix[4] == -0x10000)))
                pBdrv->matrix_scales = false;
            else
                pBdrv->matrix_scales = true;

            /* check if flips horizontally */
            if (pBdrv->matrix[0] < 0)
                pBdrv->matrix_flips_x = true;
            else
                pBdrv->matrix_flips_x = false;

            /* check if flips vertically */
            if (pBdrv->matrix[4] < 0)
                pBdrv->matrix_flips_y = true;
            else
                pBdrv->matrix_flips_y = false;
        }
    }
    else {
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        pBdrv->matrix_3d      = false;
#endif
        pBdrv->matrix_enabled = false;
        pBdrv->matrix_scales  = false;
        pBdrv->matrix_flips_x = false;
        pBdrv->matrix_flips_y = false;
    }

    BCMNEXUS_VALIDATE(m_Matrix);
}

static void bcmnexus_gfxSetState(         void                *pDrv,
                                  UNUSED_ void                *pDev,
                                  UNUSED_ GraphicsDeviceFuncs *pFuncs,
                                          CardState           *pState,
                                          DFBAccelerationMask  accel )
{
    bcmnexus_gfxDriverData* pBdrv;
    bcmnexus_gfxDeviceData* pBdev;

    D_DEBUG_AT( bcmnexusGraphics, "%s() mod_hw=0x%08x accel=0x%08x blittingflags=0x%08x drawingflags=0x%08x\n", __FUNCTION__,
                pState->mod_hw, accel, pState->blittingflags, pState->drawingflags );

    pBdrv = (bcmnexus_gfxDriverData*) pDrv;
    pBdev = (bcmnexus_gfxDeviceData*) pDev;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    {
        BcmnexusStateBits       invFlags;
        StateModificationFlags  mod_hw;
        DFBAccelerationMask     past_accel;
        DFBSurfaceDrawingFlags  past_drawingflags;
        DFBSurfaceBlittingFlags past_blittingflags;
        DFBSurfaceDrawingFlags  future_drawingflags;
        DFBSurfaceBlittingFlags future_blittingflags;
        bool                    past_flip, future_flip;
        bool                    past_draw, future_draw;
        bool                    past_blit, future_blit;

        invFlags             = m_None;
        mod_hw               = pState->mod_hw;
        past_accel           = pBdev->accel;
        pBdev->accel         = accel;
        past_drawingflags    = pBdev->drawingflags;
        past_blittingflags   = pBdev->blittingflags;

        if (DFB_BLITTING_FUNCTION(accel)) {
            future_blittingflags = pState->blittingflags;
            pBdev->blittingflags = future_blittingflags;
            future_drawingflags  = DSDRAW_NOFX;
        } else {
            future_drawingflags  = pState->drawingflags;
            pBdev->drawingflags  = future_drawingflags;
            future_blittingflags = DSBLIT_NOFX;
        }

        /* check SMF state and set corresponding state in bcmnexus_gfx internal state(s) */
        if (mod_hw == SMF_ALL)
            invFlags = (BcmnexusStateBits)m_All;
        else
        {
            past_draw        = (past_accel & bcmnexus_gfx_SUPPORTED_DRAWINGFUNCTIONS);
            future_draw      = (accel      & bcmnexus_gfx_SUPPORTED_DRAWINGFUNCTIONS);

            past_blit        = (past_accel & bcmnexus_gfx_SUPPORTED_BLITTINGFUNCTIONS);
            future_blit      = (accel      & bcmnexus_gfx_SUPPORTED_BLITTINGFUNCTIONS);

            past_flip        = (pBdrv->matrix_flips_x || pBdrv->matrix_flips_y);
            future_flip      = (pState->render_options & DSRO_MATRIX) && ((pState->matrix[0] < 0) || (pState->matrix[4] < 0));

            /* We disable the source feeeder when doing a draw operation.  We
               need to re-enable it if doing back to a blit operation.  The destination
               feeder may have been disabled during a draw operation (non-blend) and
               we may need to enable it for a non-fast blit.  Alternatively, the
               drawing may have had the destination feeder enabled (blend) and we are
               going to a fast-blit where the destination feeder needs to be disabled.
            */
            if (past_draw && future_blit)
            {
                bool past_draw_dst_colorkey;
                bool future_blit_dst_colorkey;
                bool past_draw_xor;
                bool future_blit_xor;

                /* Re-enable the source feeder please and invalidate the destination and blend blocks */
                invFlags |= (BcmnexusStateBits)(m_Source | m_Destination | m_BlendEq);

#ifdef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
                /* Re-enable the alpha premult block only if source pre-multiplication is enabled */
                if (future_blittingflags & DSBLIT_SRC_PREMULTIPLY)
                    invFlags |= (BcmnexusStateBits)m_AlphaPremult;
#endif

                /* The source color-key block is disabled during a draw operation.  Do we need
                   to re-enable it for the the blit operation? */
                if (future_blittingflags & DSBLIT_SRC_COLORKEY)
                    invFlags |= (BcmnexusStateBits)m_SrcColorKey;

                /* If the destination color-keying state is different between a draw and a blit
                   then we need to invalidate the destination color-keying block */
                past_draw_dst_colorkey   = !!(past_drawingflags    & DSDRAW_DST_COLORKEY);
                future_blit_dst_colorkey = !!(future_blittingflags & DSBLIT_DST_COLORKEY);

                if (past_draw_dst_colorkey ^ future_blit_dst_colorkey)
                    invFlags |= (BcmnexusStateBits)m_DstColorKey;

                /* If the ROP block state for XOR has changed between a draw and a blit operation,
                   then we need to invalidate it. */
                past_draw_xor   = !!(past_drawingflags    & DSDRAW_XOR);
                future_blit_xor = !!(future_blittingflags & DSBLIT_XOR);

                if (past_draw_xor ^ future_blit_xor)
                    invFlags |= (BcmnexusStateBits)m_Rop;

                past_flip   |= !!(past_blittingflags   & (DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL | DSBLIT_ROTATE180));
                future_flip |= !!(future_blittingflags & (DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL | DSBLIT_ROTATE180));
            }

            /* The core DFB code does not set the SMF_SOURCE flag if going to a drawing
               function.  We disable the source feeeder when doing a draw operation.
               Also, we must disable the source color key for draw ops, otherwise the
               destination feeder does not work.  The destination feeder may have been
               disabled during a fast-blit operation and we need to enable it again for
               a blended draw operation.  Alternatively, the destination feeder may have
               been enabled for a non-fast blit and we need to disable it for a
               non-blended draw operation.
            */
            else if (past_blit && future_draw)
            {
                bool future_draw_dst_colorkey;
                bool past_blit_dst_colorkey;
                bool future_draw_xor;
                bool past_blit_xor;

                /* We always invalidate the source, destination, source colorkey alpha premult and blend M2MC blend blocks */
                invFlags |= (BcmnexusStateBits)(m_Source | m_Destination | m_SrcColorKey | m_BlendEq | m_AlphaPremult) ;

                /* If the destination color-keying state is different between a draw and a blit
                   then we need to invalidate the destination color-keying block */
                future_draw_dst_colorkey = !!(future_drawingflags & DSDRAW_DST_COLORKEY);
                past_blit_dst_colorkey   = !!(past_blittingflags  & DSBLIT_DST_COLORKEY);

                if (future_draw_dst_colorkey ^ past_blit_dst_colorkey)
                    invFlags |= (BcmnexusStateBits)m_DstColorKey;

                /* If the ROP block state for XOR has changed between a draw and a blit operation,
                   then we need to invalidate it. */
                future_draw_xor = !!(future_drawingflags & DSDRAW_XOR);
                past_blit_xor   = !!(past_blittingflags  & DSBLIT_XOR);

                if (future_draw_xor ^ past_blit_xor)
                    invFlags |= (BcmnexusStateBits)m_Rop;
            }
            else if (past_draw && future_draw)
            {
                /* If the blending state has changed, then we need to check whether to disable
                   or enable the destination feeder. */
                if ((past_drawingflags & DSDRAW_BLEND) ^ (future_drawingflags & DSDRAW_BLEND))
                    invFlags |= (BcmnexusStateBits)m_Destination;

                if ((past_drawingflags   & (DSDRAW_BLEND | DSDRAW_SRC_PREMULTIPLY)) ^
                    (future_drawingflags & (DSDRAW_BLEND | DSDRAW_SRC_PREMULTIPLY)))
                    invFlags |= (BcmnexusStateBits)m_BlendEq;

                if ((past_drawingflags   & DSDRAW_DST_COLORKEY) ^
                    (future_drawingflags & DSDRAW_DST_COLORKEY))
                    invFlags |= (BcmnexusStateBits)m_DstColorKey;

                if ((past_drawingflags & DSDRAW_XOR) ^ (future_drawingflags & DSDRAW_XOR))
                    invFlags |= (BcmnexusStateBits)m_Rop;
            }
            else if (past_blit && future_blit)
            {
                past_flip   |= !!(past_blittingflags   & (DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL | DSBLIT_ROTATE180));
                future_flip |= !!(future_blittingflags & (DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL | DSBLIT_ROTATE180));

                /* If we were doing a fast blit or we are about to to a fast blit, then we need to invalidate
                   the destination feeder. */
                if ((past_blittingflags == DSBLIT_NOFX) || (future_blittingflags == DSBLIT_NOFX))
                    invFlags |= (BcmnexusStateBits)m_Destination;

                if ((past_blittingflags & DSBLIT_SRC_COLORKEY) ^ (future_blittingflags & DSBLIT_SRC_COLORKEY))
                    invFlags |= (BcmnexusStateBits)m_SrcColorKey;

                if ((past_blittingflags & DSBLIT_DST_COLORKEY) ^ (future_blittingflags & DSBLIT_DST_COLORKEY))
                    invFlags |= (BcmnexusStateBits)m_DstColorKey;

#ifdef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
                /* Does the source alpha premultiplication block need to be invalidated? */
                if ((past_blittingflags   & (DSBLIT_SRC_PREMULTIPLY | DSBLIT_BLEND_COLORALPHA)) ^
                    (future_blittingflags & (DSBLIT_SRC_PREMULTIPLY | DSBLIT_BLEND_COLORALPHA)))
                    invFlags |= (BcmnexusStateBits)m_AlphaPremult;

                /* Has Blend Block changed? */
                if ((past_blittingflags   & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)) ^
                    (future_blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)))
                    invFlags |= (BcmnexusStateBits)m_BlendEq;
#else
                /* Has Blend Block changed? */
                if ((past_blittingflags   & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY)) ^
                    (future_blittingflags & (DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY)))
                    invFlags |= (BcmnexusStateBits)m_BlendEq;
#endif
                /* Has Color Matrix changed? */
                if ((past_blittingflags   & (DSBLIT_COLORIZE | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_SRC_PREMULTIPLY |
                                             DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)) ^
                    (future_blittingflags & (DSBLIT_COLORIZE | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_SRC_PREMULTIPLY |
                                             DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA)))
                    invFlags |= (BcmnexusStateBits)m_ColorMatrix;

                if ((past_blittingflags & DSBLIT_XOR) ^ (future_blittingflags & DSBLIT_XOR))
                    invFlags |= (BcmnexusStateBits)m_Rop;

                /* If Color is changed for drawstring, we need to invalidate the source feeder */
                if (mod_hw & SMF_COLOR)
                    if ((future_blittingflags & DSBLIT_SRC_PREMULTIPLY) && (future_blittingflags & DSBLIT_COLORIZE) && (future_blittingflags & DSBLIT_BLEND_ALPHACHANNEL))
                        invFlags |= (BcmnexusStateBits)m_Source;
            }

            /* If the source has changed, then we also need to invalidate the
               source color-key and filter co-efficients. */
            if (mod_hw & SMF_SOURCE)
                invFlags |= (BcmnexusStateBits)(m_Source | m_SrcColorKey | m_Filter);

            /* If the destination or source2 has changed, then we also need to invalidate
               the destination color-keying and filter co-efficients. */
            if (mod_hw & (SMF_DESTINATION | SMF_SOURCE2))
                invFlags |= (BcmnexusStateBits)(m_Destination | m_Output | m_DstColorKey | m_Filter);

            if (mod_hw & SMF_SRC_COLORKEY )
                invFlags |= (BcmnexusStateBits)m_SrcColorKey;

            if (mod_hw & SMF_DST_COLORKEY)
                invFlags |= (BcmnexusStateBits)m_DstColorKey;

            if (mod_hw & SMF_COLOR)
                invFlags |= (BcmnexusStateBits)(m_BlendEq | m_ColorMatrix);

            if (mod_hw & (SMF_SRC_BLEND | SMF_DST_BLEND))
                invFlags |= (BcmnexusStateBits)m_BlendEq;

            if (mod_hw & SMF_RENDER_OPTIONS)
            {
                if ((pBdev->renderOptions   & (DSRO_SMOOTH_UPSCALE | DSRO_SMOOTH_DOWNSCALE)) ^
                    (pState->render_options & (DSRO_SMOOTH_UPSCALE | DSRO_SMOOTH_DOWNSCALE)))
                    invFlags |= (BcmnexusStateBits)m_Filter;

                if ((pBdev->renderOptions & DSRO_MATRIX) ^ (pState->render_options & DSRO_MATRIX))
                    invFlags |= (BcmnexusStateBits)m_Matrix;
            }

            if (mod_hw & SMF_MATRIX)
                invFlags |= (BcmnexusStateBits)m_Matrix;

            /* If we need to do flips, then we need to use the source feeder for mirroring */
            if (past_flip ^ future_flip)
                invFlags |= (BcmnexusStateBits)m_Mirror;
            if (future_blit)
            {
                if ( (pState->source->config.colorspace != pState->destination->config.colorspace) ||
                     pBdrv->packetSettings.packetSetRGBYUVColorMatrix)
                    invFlags |= (BcmnexusStateBits)(m_ColorMatrix | m_BlendEq);
            }
        }
        /* Remember render options state */
        if (mod_hw & SMF_RENDER_OPTIONS)
            pBdev->renderOptions = pState->render_options;

        D_DEBUG_AT(bcmnexusGraphics, "%s(): Invalid Flags=0x%08x\n", __FUNCTION__, invFlags);

        /* Now really invalidate the flags... */
        BCMNEXUS_INVALIDATE( invFlags );

    }
#endif
    bcmnexus_gfxSetStateMatrix( pBdrv, pBdev, pState );

    /* To avoid rectangle x or width is odd for YCbCr422 format, because this's not supported as two pixels share the same chroma (CbCr) values */
    if (pState->destination->config.format == DSPF_YUY2 || pState->destination->config.format == DSPF_UYVY)
        pBdrv->odd_shift_enable = true;
    else
        pBdrv->odd_shift_enable = false;

    switch (accel) {
    case DFXL_FILLRECTANGLE:
    case DFXL_DRAWRECTANGLE:
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        if (pBdrv->matrix_3d)
            bcmnexus_gfxSetStateDraw3D(pBdrv, pState);
        else
#endif

            bcmnexus_gfxSetStateRectangle(pBdrv, pBdev, pState);

        pState->set = (DFXL_FILLRECTANGLE | DFXL_DRAWRECTANGLE);
        break;

#ifdef BCMNEXUS_TRAPEZOID_SUPPORTED
    case DFXL_FILLTRAPEZOID:
#ifdef BCMNEXUS_GFX_HAS_P3D
        if (pBdrv->gfx_3d_support)
            bcmnexus_gfxSetStateDraw3D(pBdrv, pState);
        else
            bcmnexus_gfxSetStateRectangle(pBdrv, pBdev, pState);

#else
        bcmnexus_gfxSetStateRectangle(pBdrv, pBdev, pState);
#endif
        pState->set |= DFXL_FILLTRAPEZOID;
        break;
#endif

    case DFXL_BLIT:
    case DFXL_BLIT2:
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        if (pBdrv->matrix_3d) {
            bcmnexus_gfxSetStateBlit3D(pBdrv, pState);
        } else
#endif
        if (pBdrv->matrix_scales) {
               /* due to matrix type, blit turns into a strechblit! */
               bcmnexus_gfxSetStateBlit(pBdrv, pBdev, pState, true, accel);
        } else
        if (pState->blittingflags == DSBLIT_NOFX)
        {
            bcmnexus_gfxSetStateFastBlit(pBdrv, pBdev, pState, accel);
        }
        else
        {
            bcmnexus_gfxSetStateBlit(pBdrv, pBdev, pState, false, accel);
        }
        pState->set = DFXL_BLIT;
        break;

    case DFXL_STRETCHBLIT:
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        if (pBdrv->matrix_3d) {
            bcmnexus_gfxSetStateBlit3D(pBdrv, pState);
        } else
#endif
            bcmnexus_gfxSetStateBlit(pBdrv, pBdev, pState, true, accel);
        pState->set = DFXL_STRETCHBLIT;
        break;

#ifdef BCMNEXUS_GFX_HAS_P3D
    case DFXL_DRAWLINE:
    case DFXL_FILLTRIANGLE:
    case DFXL_FILLTRIANGLES:
        bcmnexus_gfxSetStateDraw3D(pBdrv, pState);
        pState->set = (DFXL_DRAWLINE | DFXL_FILLTRIANGLE | DFXL_FILLTRIANGLES);
        break;
    case DFXL_TEXTRIANGLES:
        bcmnexus_gfxSetStateBlit3D(pBdrv, pState);
        pState->set = DFXL_TEXTRIANGLES;
        break;
#endif
    default: /* should not get here */
        break;
    }

    pBdrv->dst_clip = pState->clip;
    pState->mod_hw = 0;
    return; /* done ! */
}

static void
checkpointCallback(         void *data,
                    UNUSED_ int   param )
{
    bcmnexus_gfxDeviceData *pBdevice = data;

    pthread_mutex_t *pLock = &pBdevice->syncLock;
    pthread_cond_t  *pCond = &pBdevice->syncCond;

    D_DEBUG_AT( bcmnexusGraphicsX, "synchronisation callback handled with %p\n", data );

    PTHREAD_ROBUST_MUTEX_LOCK( pLock );
    pthread_cond_signal( pCond );
    pthread_mutex_unlock( pLock );

}

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
static void
packetSpaceAvailableCallback(         void *data,
                              UNUSED_ int   param )
{
    bcmnexus_gfxDeviceData *pBdevice = data;

    pthread_mutex_t *pLock = &pBdevice->packetSpaceAvailLock;
    pthread_cond_t  *pCond =  &pBdevice->packetSpaceAvailCond;

    D_DEBUG_AT( bcmnexusGraphicsPB, "synchronisation packet callback handled with %p\n", data );

    PTHREAD_ROBUST_MUTEX_LOCK( pLock );
    pthread_cond_signal( pCond );
    pthread_mutex_unlock( pLock );

}
#endif

static DFBEnumerationResult
bcmnexus_gfxSurfaceCallback(         CoreSurfaceAllocation *alloc,
                             UNUSED_ void                  *ctx )
{
    struct bcmAllocationData *ad;

    ad = alloc->data;
    ad->syncIssued = true;
    return DFENUM_OK;
}

static DFBResult
bcmnexus_gfxWaitComplete( bcmnexus_gfxDriverData *pBdriver,
                          bcmnexus_gfxDeviceData *pBdevice )
{
#ifndef BCMNEXUS_CHECKPOINT_IN_SETTINGS_SUPPORTED
     NEXUS_CallbackDesc  callback;
#endif
     NEXUS_Error         error;
     CoreSurfacePool    *pool;

     D_DEBUG_AT( bcmnexusGraphicsX, "%s()\n", __FUNCTION__ );

     /* Do we really need to do a Graphics2D Checkpoint/Sync? */
     if (GET_GRAPHICS2D_SYNC(pBdriver->pBrcmDfb)) {
          PTHREAD_ROBUST_MUTEX_LOCK( &pBdevice->syncLock );

#ifndef BCMNEXUS_CHECKPOINT_IN_SETTINGS_SUPPORTED
     if (!pBdevice->callback)
          pBdevice->callback = checkpointCallback;

          callback.callback = pBdevice->callback;
          callback.context  = pBdevice;

          error = NEXUS_Graphics2D_Checkpoint(pBdriver->hGfx2D, &callback);
#else
          /* if Checkpoint returns NEXUS_SUCCESS, no wait was needed, and no callback is called */
          error = NEXUS_Graphics2D_Checkpoint(pBdriver->hGfx2D, 0);

          if (error != NEXUS_SUCCESS)
#endif
          {
               D_DEBUG_AT( bcmnexusGraphicsX, "Checkpoint: wait for callback\n" );
              pthread_cond_wait(&pBdevice->syncCond,&pBdevice->syncLock);
          }

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
          if (pBdriver->packetSettings.packetBufStartPtr != NULL &&
              (((uint8_t*)pBdriver->packetSettings.packetBufNextPtr - (uint8_t*)pBdriver->packetSettings.packetBufStartPtr) > 0))
          {
               pBdriver->packetSettings.packetBufStartPtr = pBdriver->packetSettings.packetBufNextPtr = NULL;
               pBdriver->packetSettings.packetBufSize = 0;
          }
#endif
          pool = GET_POOL_HND(pBdriver->pBrcmDfb);
          if (pool) {
              dfb_surface_pool_enumerate( pool, bcmnexus_gfxSurfaceCallback, NULL );
          }
          SET_GRAPHICS2D_SYNC(pBdriver->pBrcmDfb, false);
          pthread_mutex_unlock( &pBdevice->syncLock );
     }

     D_DEBUG_AT( bcmnexusGraphicsX, "%s() synced\n", __FUNCTION__ );

     return DFB_OK;
}

static DFBResult
bcmnexus_gfxFlushMode( bcmnexus_gfxDriverData *pBdriver,
                       bcmnexus_gfxDeviceData *pBdevice )
{
    D_DEBUG_AT( bcmnexusGraphicsQ, "%s() <- count %d\n", __FUNCTION__, pBdevice->count );

    if (pBdevice->count) {
        pBdevice->count = 0;

        switch (pBdevice->mode) {
            case BCMNEXUS_GFX_IDLE:
                D_DEBUG_AT( bcmnexusGraphicsQ, "  -> IDLE!\n" );
                break;

            case BCMNEXUS_GFX_2D:
                D_DEBUG_AT( bcmnexusGraphicsQ, "  -> flushing 2D...\n" );
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
                bcmnexus_gfxPktWritePacketComplete(pBdriver);
#else
                bcmnexus_gfxWaitComplete( pBdriver, pBdevice );
#endif
                break;

            case BCMNEXUS_GFX_3D:
#ifdef BCMNEXUS_GFX_HAS_P3D
                D_DEBUG_AT( bcmnexusGraphicsQ, "  -> flushing 3D...\n" );
                BP3D_IssueRender( pBdriver->hP3d, NULL, NULL, 0 );
#endif
                break;
        }
        /* Once we've flushed, we are effectively in IDLE mode */
        pBdevice->mode = BCMNEXUS_GFX_IDLE;
    }

    return DFB_OK;
}

static DFBResult
bcmnexus_gfxEnterMode( bcmnexus_gfxDriverData *pBdriver,
                       bcmnexus_gfxDeviceData *pBdevice,
                       bcmnexus_gfxMode        mode,
                       int                     inc )
{
    unsigned limit;

    D_DEBUG_AT( bcmnexusGraphicsQ, "%s( %s )\n", __FUNCTION__, mode ? (mode == BCMNEXUS_GFX_2D ? "2D" : "3D") : "IDLE" );

    if (pBdevice->mode == mode) {

        limit = (mode == BCMNEXUS_GFX_2D) ? BCMNEXUS_GRAPHICS2D_FLUSH_COUNT : BCMNEXUS_GRAPHICS3D_FLUSH_COUNT;

        if ((pBdevice->count+inc) >= limit) {
            bcmnexus_gfxFlushMode( pBdriver, pBdevice );
        }

        pBdevice->count += inc;
        return DFB_OK;
    }

    /* Leaving... */
    switch (pBdevice->mode) {
        case BCMNEXUS_GFX_IDLE:
            D_DEBUG_AT( bcmnexusGraphicsQ, "  -> leaving IDLE...\n" );
            break;

        case BCMNEXUS_GFX_2D:
            D_DEBUG_AT( bcmnexusGraphicsQ, "  -> leaving 2D...\n" );
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
            bcmnexus_gfxPktWritePacketComplete(pBdriver);
#endif
            bcmnexus_gfxWaitComplete( pBdriver, pBdevice );
            break;

        case BCMNEXUS_GFX_3D:
#ifdef BCMNEXUS_GFX_HAS_P3D
            D_DEBUG_AT( bcmnexusGraphicsQ, "  -> leaving 3D...\n" );
            BP3D_IssueRenderAndWait( pBdriver->hP3d );
#endif
            break;
    }

    /* Entering... */
    switch (mode) {
        case BCMNEXUS_GFX_IDLE:
            D_DEBUG_AT( bcmnexusGraphicsQ, "  -> entering IDLE...\n" );
            break;

        case BCMNEXUS_GFX_2D:
            D_DEBUG_AT( bcmnexusGraphicsQ, "  -> entering 2D...\n" );
            SET_GRAPHICS2D_SYNC(pBdriver->pBrcmDfb, true);
            break;

        case BCMNEXUS_GFX_3D:
#ifdef BCMNEXUS_GFX_HAS_P3D
            D_DEBUG_AT( bcmnexusGraphicsQ, "  -> entering 3D...\n" );
            SET_GRAPHICS2D_SYNC(pBdriver->pBrcmDfb, true);
#endif
            break;
    }

    pBdevice->mode  = mode;
    pBdevice->count = 0;

    return DFB_OK;
}

static DFBResult bcmnexus_gfxEngineSync( void *pDrv,
                                         void *pDev )
{
    bcmnexus_gfxDriverData *pBdriver = pDrv;
    bcmnexus_gfxDeviceData *pBdevice = pDev;

    D_DEBUG_AT( bcmnexusGraphicsQ, "%s()\n", __FUNCTION__ );

    bcmnexus_gfxEnterMode( pBdriver, pBdevice, BCMNEXUS_GFX_IDLE, 0 );

    return DFB_OK;
}

static DFBEnumerationResult
bcmnexus_gfxCalcBufferSizeCallback( CoreSurfaceAllocation *alloc,
                                    void                  *ctx )
{
    struct bcmAllocationData *ad;

    ad = alloc->data;

    *(DFB_Platform_P_PoolHandle *)ctx = ad->gfx3DPoolHandle;
    return DFENUM_OK;
}

static DFBResult bcmnexus_gfxCalcBufferSize( void              *pDrv,
                                     UNUSED_ void              *pDev,
                                             CoreSurfaceBuffer *buffer,
                                             int               *ret_pitch,
                                             int               *ret_length )
{
    DFBResult                  ret = DFB_UNSUPPORTED;
    bcmnexus_gfxDriverData    *pBdriver = pDrv;
    CoreSurfacePool           *pool;
    DFB_Platform_P_PoolHandle  gfx3DPoolHandle;
    int                        width;
    int                        height;

    D_DEBUG_AT( bcmnexusGraphics, "%s()\n", __FUNCTION__ );

    /* If a 3D graphics surface, then get the buffer contraints from the platform layer... */
    if (buffer->surface->config.caps & DSCAPS_GL)
    {
        pool = GET_GFX3D_POOL_HND(pBdriver->pBrcmDfb);
        width  = buffer->surface->config.size.w;
        height = MAX(buffer->surface->config.size.h, buffer->surface->config.min_size.h);
        dfb_surface_pool_enumerate( pool, bcmnexus_gfxCalcBufferSizeCallback, &gfx3DPoolHandle );

        ret = (DFB_Platform_P_Graphics3D_GetBufferSize(gfx3DPoolHandle, width, height, bcmnexus_getDFBPlatformPixelFormat(buffer->format),
                                                       ret_pitch, ret_length) == DFB_PLATFORM_OK) ? DFB_OK : DFB_UNSUPPORTED;
    }
    return ret;
}

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
static void bcmnexus_gfxEmitCommands( void *pBdrv,
                                      void *pBdev )
{
    bcmnexus_gfxDeviceData *pBdevice = pBdev;

    D_DEBUG_AT( bcmnexusGraphicsPB, "%s()\n", __FUNCTION__ );

    switch (pBdevice->mode)
    {
        case BCMNEXUS_GFX_2D:
            bcmnexus_gfxPktWritePacketComplete(pBdrv);
            break;
        case BCMNEXUS_GFX_3D:
            break;
        default:
            break;
    }
}
#endif

static bool bcmnexus_gfxDrawRectangle(         void         *pDrv,
                                       UNUSED_ void         *pDev,
                                               DFBRectangle *pRect )
{
    NEXUS_Error                             rc = NEXUS_SUCCESS;
    bcmnexus_gfxDriverData                 *pBdrv;
    bcmnexus_gfxDeviceData                 *pBdevice = pDev;
    NEXUS_Graphics2DFillSettings           *pFillSettings;
    DFBRectangle                            rect;
    int                                     x, y, w, h;

    pBdrv = (bcmnexus_gfxDriverData *)pDrv;
    pFillSettings = &pBdrv->fillSettings;

    x = pRect->x;
    y = pRect->y;
    w = pRect->w;
    h = pRect->h;

    D_DEBUG_AT( bcmnexusGraphics, "%s() sur=%p x,y,w,h=%d,%d,%d,%d color(ARGB)=0x%08x\n", __FUNCTION__,
                (void *)pBdrv->fillSettings.surface, x, y, w, h, pFillSettings->color );

    DFBEdgeFlags edgeflags = DFEF_ALL;

    if (pBdrv->matrix_enabled) {
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        if (pBdrv->matrix_3d)
        {
            BP3D_Vertex V[4];
            DFBPoint    p[4] = {
                { x, y },
                { x + w, y },
                { x + w, y + h },
                { x, y + h }
            };

            DFB_TRANSFORM( p[0].x, p[0].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[1].x, p[1].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[2].x, p[2].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[3].x, p[3].y, pBdrv->matrix, pBdrv->matrix_affine );

            memset( V, 0, sizeof(V) );

            for (int i = 0; i < 4; i++) {
                V[i].fX        = p[i].x;
                V[i].sX        = p[i].x;
                V[i].fY        = p[i].y;
                V[i].sY        = p[i].y;
                V[i].ulDiffuse = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */
            }

            bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_3D, BCMNEXUS_GRAPHICS3D_DRAW_RECT_INC );

            BP3D_DrawLine( pBdrv->hP3d, &V[0], &V[1] );
            BP3D_DrawLine( pBdrv->hP3d, &V[1], &V[2] );
            BP3D_DrawLine( pBdrv->hP3d, &V[2], &V[3] );
            BP3D_DrawLine( pBdrv->hP3d, &V[3], &V[0] );

            return true;
        }
#endif
        APPLY_MATRIX_RECT(x,y,w,h,pBdrv->matrix);
    }

    bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_2D, BCMNEXUS_GRAPHICS2D_DRAW_RECT_INC);

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    edgeflags = dfb_clip_edges( &pBdrv->dst_clip, &rect );
    if (!edgeflags)
          return true; /* nothing to do here everything cut off */

    x = rect.x;
    y = rect.y;
    w = rect.w;
    h = rect.h;

    /* Check to make sure width/height is not zero */
    if (!w || !h)
    {
        return true;    /* nothing to do */
    }

    /* M2MC does NOT support the width=1 for YCbCr422 format */
    if (pBdrv->odd_shift_enable)
        return false;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    BM2MC_PACKET_Rectangle OutRect={x, y, w, h};

    bcmnexus_gfxPktGetPacketBuffer(pDrv, pDev, GFX_DRAW_PACKET_SIZE);

    OutRect.height = 1;

    if (edgeflags & DFEF_TOP)
        bcmnexus_gfxPktSetFillPacket(&pBdrv->packetSettings, &OutRect);

    OutRect.y = (y + h - 1);
    if (edgeflags & DFEF_BOTTOM)
        bcmnexus_gfxPktSetFillPacket(&pBdrv->packetSettings, &OutRect);

    OutRect.y = y;
    OutRect.width = 1;
    OutRect.height = h;

    if (edgeflags & DFEF_LEFT)
        bcmnexus_gfxPktSetFillPacket(&pBdrv->packetSettings, &OutRect);

    OutRect.x = (x + w - 1);
    if (edgeflags & DFEF_RIGHT)
        bcmnexus_gfxPktSetFillPacket(&pBdrv->packetSettings, &OutRect);

#else
#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
    NEXUS_Graphics2DPorterDuffFillSettings *pPDFillSettings = &pBdrv->PorterDuffFillSettings;
    if (pPDFillSettings->operation < NEXUS_PorterDuffOp_eMax)
    {
        if (edgeflags & DFEF_TOP) {
            /* Setup top edge of dest rectangle */
            pPDFillSettings->rect.x = x;
            pPDFillSettings->rect.y = y;
            pPDFillSettings->rect.width = w;
            pPDFillSettings->rect.height = 1;

            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_PorterDuffFill(pBdrv->hGfx2D, pPDFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                    continue;
                }
#endif
                break;
            } while(true);
        }

        if (edgeflags & DFEF_BOTTOM) {
            /* Setup bottom edge of dest rectangle */
            pPDFillSettings->rect.y = (y + h - 1);

            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_PorterDuffFill(pBdrv->hGfx2D, pPDFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                    continue;
                }
#endif
                break;
            } while(true);
        }

        if (edgeflags & DFEF_LEFT) {
            /* Setup left edge of dest rectangle */
            pPDFillSettings->rect.y = y;
            pPDFillSettings->rect.width = 1;
            pPDFillSettings->rect.height = h;
            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_PorterDuffFill(pBdrv->hGfx2D, pPDFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                    continue;
                }
#endif
                break;
            } while(true);
        }

        if (edgeflags & DFEF_RIGHT) {
            /* Setup right edge of dest rectangle */
            pPDFillSettings->rect.x = (x + w - 1);
            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_PorterDuffFill(pBdrv->hGfx2D, pPDFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                    continue;
                }
#endif
                break;
            } while(true);
        }
    }
    else
#endif
    {
        if (edgeflags & DFEF_TOP) {
            /* Setup top edge of dest rectangle */
            pFillSettings->rect.x = x;
            pFillSettings->rect.y = y;
            pFillSettings->rect.width = w;
            pFillSettings->rect.height = 1;

            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_Fill(pBdrv->hGfx2D, pFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                    continue;
                }
#endif
                break;
            } while(true);
        }

        if (edgeflags & DFEF_BOTTOM) {
            /* Setup bottom edge of dest rectangle */
            pFillSettings->rect.y = (y + h - 1);

            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_Fill(pBdrv->hGfx2D, pFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                    continue;
                }
#endif
                break;
            } while(true);
        }

        if (edgeflags & DFEF_LEFT) {
            /* Setup left edge of dest rectangle */
            pFillSettings->rect.y = y;
            pFillSettings->rect.width = 1;
            pFillSettings->rect.height = h;

            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_Fill(pBdrv->hGfx2D, pFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                    continue;
                }
#endif
                break;
            } while(true);
        }

        if (edgeflags & DFEF_RIGHT) {
            /* Setup right edge of dest rectangle */
            pFillSettings->rect.x = (x + w - 1);

            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_Fill(pBdrv->hGfx2D, pFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                    continue;
                }
#endif
                break;
            } while(true);
        }
    }
#endif
    return (rc==NEXUS_SUCCESS) ? true:false;
}


static bool bcmnexus_gfxFillRectangle(         void         *pDrv,
                                       UNUSED_ void         *pDev,
                                               DFBRectangle *pRect )
{
    bcmnexus_gfxDriverData                 *pBdrv;
    bcmnexus_gfxDeviceData                 *pBdevice      = pDev;
    NEXUS_Graphics2DFillSettings           *pFillSettings;
    DFBRectangle                            rect;
    int                                     x ,y, w, h;

    NEXUS_Error rc = NEXUS_SUCCESS;

    pBdrv = (bcmnexus_gfxDriverData *)pDrv;
    pFillSettings = &pBdrv->fillSettings;

    x = pRect->x;
    y = pRect->y;
    w = pRect->w;
    h = pRect->h;

    if (pBdrv->matrix_enabled) {
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        if (pBdrv->matrix_3d) {
            BP3D_Vertex V[4];
            DFBPoint p[4] = {
                { x, y },
                { x + w, y },
                { x + w, y + h },
                { x, y + h }
            };

            DFB_TRANSFORM( p[0].x, p[0].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[1].x, p[1].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[2].x, p[2].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[3].x, p[3].y, pBdrv->matrix, pBdrv->matrix_affine );

            memset( V, 0, sizeof(V) );

            for (int i = 0; i < 4; i++) {
                V[i].fX        = p[i].x;
                V[i].sX        = p[i].x;
                V[i].fY        = p[i].y;
                V[i].sY        = p[i].y;
                V[i].ulDiffuse = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */
            }

            bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_3D, BCMNEXUS_GRAPHICS3D_FILL_RECT_INC );

            if (BP3D_DrawTriangleFan( pBdrv->hP3d, V, 2 ))
                D_WARN("FillRectangle (3D)");

            return true;
        }
#endif

        APPLY_MATRIX_RECT(x,y,w,h,pBdrv->matrix);
    }

    bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_2D, BCMNEXUS_GRAPHICS2D_FILL_RECT_INC );

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    if (!dfb_clip_rectangle(&pBdrv->dst_clip, &rect))
          return true;   /* nothing to do - lies comletely out of destination */

    x = rect.x;
    y = rect.y;
    w = rect.w;
    h = rect.h;


    /* Check to make sure width/height is not zero */
    if (!w || !h)
    {
        return true;    /* nothing to do */
    }

    D_DEBUG_AT( bcmnexusGraphics, "%s() sur=%p x,y,w,h=%d,%d,%d,%d color(ARGB)=0x%08x", __FUNCTION__,
                (void *)pFillSettings->surface, x, y, w, h, pFillSettings->color);

    if (pBdrv->odd_shift_enable)
    {
        if (x & 1) x--;
        if (w & 1) w--;

        if (!w)
            return false;
    }

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    BM2MC_PACKET_Rectangle OutRect={x, y, w, h};

    bcmnexus_gfxPktGetPacketBuffer(pDrv, pDev, GFX_FILL_PACKET_SIZE);
    bcmnexus_gfxPktSetFillPacket(&pBdrv->packetSettings, &OutRect);

#else

#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
     NEXUS_Graphics2DPorterDuffFillSettings *pPDFillSettings = &pBdrv->PorterDuffFillSettings;
    if (pPDFillSettings->operation < NEXUS_PorterDuffOp_eMax)
    {
        /* Set dest rectangle  */
        pPDFillSettings->rect.x = x;
        pPDFillSettings->rect.y = y;
        pPDFillSettings->rect.width = w;
        pPDFillSettings->rect.height = h;

        do {
            SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
            rc = NEXUS_Graphics2D_PorterDuffFill(pBdrv->hGfx2D, pPDFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
            if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                continue;
            }
#endif
            break;
        } while(true);
    }
    else
#endif
    {
        /* Set dest rectangle  */
        pFillSettings->rect.x = x;
        pFillSettings->rect.y = y;
        pFillSettings->rect.width = w;
        pFillSettings->rect.height = h;


        do {
            SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
            rc = NEXUS_Graphics2D_Fill(pBdrv->hGfx2D, pFillSettings);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
            if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                bcmnexus_gfxWaitComplete( pBdrv, pBdevice );
                continue;
            }
#endif
            break;
        } while(true);

    }
#endif
    return (rc==NEXUS_SUCCESS) ? true:false;
}


#ifdef BCMNEXUS_TRAPEZOID_SUPPORTED
typedef struct {
   int xi;
   int xf;
   int mi;
   int mf;
   int _2dy;
} DDA;

#define SETUP_DDA(xs,ys,xe,ye,dda)         \
     do {                                  \
          int dx = (xe) - (xs);            \
          int dy = (ye) - (ys);            \
          dda.xi = (xs);                   \
          if (dy != 0) {                   \
               dda.mi = dx / dy;           \
               dda.mf = 2*(dx % dy);       \
               dda.xf = -dy;               \
               dda._2dy = 2 * dy;          \
               if (dda.mf < 0) {           \
                    dda.mf += 2 * ABS(dy); \
                    dda.mi--;              \
               }                           \
          }                                \
          else {                           \
               dda.mi = 0;                 \
               dda.mf = 0;                 \
               dda.xf = 0;                 \
               dda._2dy = 0;               \
          }                                \
     } while (0)


#define INC_DDA(dda)                       \
     do {                                  \
          dda.xi += dda.mi;                \
          dda.xf += dda.mf;                \
          if (dda.xf > 0) {                \
               dda.xi++;                   \
               dda.xf -= dda._2dy;         \
          }                                \
     } while (0)

static bool bcmnexus_gfxFillTrapezoid(         void         *pDrv,
                                       UNUSED_ void         *pDev,
                                               DFBTrapezoid *pTrap )
{
    bcmnexus_gfxDriverData                 *pBdrv;
    bcmnexus_gfxDeviceData                 *pBdevice = pDev;
    NEXUS_Graphics2DFillSettings           *pFillSettings;
    int                                     x1, y1, w1, x2 , y2, w2;
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    pBdrv = (bcmnexus_gfxDriverData *)pDrv;
    pFillSettings = &pBdrv->fillSettings;

    x1 = pTrap->x1;
    y1 = pTrap->y1;
    w1 = pTrap->w1;
    x2 = pTrap->x2;
    y2 = pTrap->y2;
    w2 = pTrap->w2;

    D_DEBUG_AT( bcmnexusGraphics, "%s() sur=%p x1,y1,w1,x2,y2,w2=%d,%d,%d,%d,%d,%d color(ARGB)=0x%08x\n", __FUNCTION__,
                (void *)pBdrv->fillSettings.surface, x1, y1, w1, x2, y2, w2, pFillSettings->color );

#ifdef BCMNEXUS_GFX_HAS_P3D
    if (pBdrv->gfx_3d_support) {
        BP3D_Vertex V[4];
        DFBPoint p[4] = {
            { x1, y1 },
            { x1 + w1, y1 },
            { x2 + w2, y2 },
            { x2, y2 }
        };

        if (pBdrv->matrix_enabled) {
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
            if (pBdrv->matrix_3d) {
                DFB_TRANSFORM( p[0].x, p[0].y, pBdrv->matrix, pBdrv->matrix_affine );
                DFB_TRANSFORM( p[1].x, p[1].y, pBdrv->matrix, pBdrv->matrix_affine );
                DFB_TRANSFORM( p[2].x, p[2].y, pBdrv->matrix, pBdrv->matrix_affine );
                DFB_TRANSFORM( p[3].x, p[3].y, pBdrv->matrix, pBdrv->matrix_affine );
            }
            else
#endif
            {
                APPLY_MATRIX_TRAP(x1,y1,w1,x2,y2,w2,pBdrv->matrix);
                p[0].x = x1;
                p[0].y = y1;
                p[1].x = x1 + w1;
                p[1].y = y1;
                p[2].x = x2 + w2;
                p[2].y = y2;
                p[3].x = x2;
                p[3].y = y2;
            }
        }

        memset( V, 0, sizeof(V) );

        for (int i = 0; i < 4; i++) {
            V[i].fX        = p[i].x;
            V[i].sX        = p[i].x;
            V[i].fY        = p[i].y;
            V[i].sY        = p[i].y;
            V[i].ulDiffuse = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */
        }

        bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_3D, BCMNEXUS_GRAPHICS3D_FILL_TRAP_INC );

        if (BP3D_DrawTriangleFan( pBdrv->hP3d, V, 2 ))
            D_WARN("FillRectangle (3D)");
    }
    else
#endif
    {
        bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_2D, BCMNEXUS_GRAPHICS2D_FILL_TRAP_INC );

        if (pBdrv->matrix_enabled) {
            APPLY_MATRIX_TRAP(x1,y1,w1,x2,y2,w2,pBdrv->matrix);
        }

        /* Check to make sure width/height is not zero */
        if ((!w1 && !w2) || y2 == y1) {
            return true;    /* nothing to do */
        }

        /* M2MC does NOT support the width=1 for YCbCr422 format */
        if (pBdrv->odd_shift_enable)
            return false;

        /* draw spans */
        DDA dda1 = { .xi = 0 }, dda2 = { .xi = 0 };
        int clip_x1 = pBdrv->dst_clip.x1;
        int clip_x2 = pBdrv->dst_clip.x2;
        int y = y1;
        int yend = y2;
        int pkt_space_avail = 0;

        if (yend > pBdrv->dst_clip.y2)
             yend = pBdrv->dst_clip.y2;

        /* top left to bottom left */
        SETUP_DDA(x1,          y1, x2,          y2, dda1);
        /* top right to bottom right */
        SETUP_DDA(x1 + w1 - 1, y1, x2 + w2 - 1, y2, dda2);

        while (y <= yend) {
             int x     = dda1.xi;
             int width = dda2.xi - dda1.xi;

             if (clip_x2 < x + width - 1 )
                  width = clip_x2 - x + 1;

             if (width > 0) {
                  if (clip_x1 > x) {
                       width -= (clip_x1 - x);
                       x = clip_x1;
                  }

                  if (width > 0 && y >= pBdrv->dst_clip.y1) {
                      if (!pkt_space_avail) { /* we must request more space in the package buffer */
                          int spans_left = yend - y + 1;

                          pkt_space_avail = MIN(spans_left, 20);
                          bcmnexus_gfxPktGetPacketBuffer(pDrv, pDev, GFX_FILL_PACKET_SIZE * pkt_space_avail);
                      }

                      pkt_space_avail--;

                      BM2MC_PACKET_Rectangle OutRect = { x, y, width, 1 };

                      bcmnexus_gfxPktSetFillPacket( &pBdrv->packetSettings, &OutRect );
                  }
             }

             INC_DDA(dda1);
             INC_DDA(dda2);

             y++;
        }
    }

    return true;
#else
    return false;
#endif /* BCMNEXUS_GFX_PACKET_BUFFER */
}
#endif


static bool bcmnexus_gfxBlitCommon(        bcmnexus_gfxDriverData *pBdrv,
                                   UNUSED_ bcmnexus_gfxDeviceData *pBdev)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER

#ifndef BCMNEXUS_FIXEDSCALE_SUPPORTED
    bcmnexus_gfxPktGetPacketBuffer(pBdrv, pBdev, GFX_BLIT_PACKET_SIZE);
#else
    bcmnexus_gfxPktGetPacketBuffer(pBdrv, pBdev, GFX_BLIT_PACKET_SIZE + sizeof(BM2MC_PACKET_PacketFixedScale));
    bcmnexus_gfxPktSetFixedScalePacket(&pBdrv->packetSettings, pBdev, pBdrv->packetSettings.packetHorPhase, pBdrv->packetSettings.packetVerPhase, pBdrv->packetSettings.packetHorStep, pBdrv->packetSettings.packetVerStep);
#endif
    bcmnexus_gfxPktSetFilterPacket(&pBdrv->packetSettings, pBdev, pBdrv->packetSettings.packetHorzFilter, pBdrv->packetSettings.packetVertFilter);
    bcmnexus_gfxPktSetBlitPacket(&pBdrv->packetSettings, pBdev);

#else
    if (pBdrv->blitFast == true)
    {
        NEXUS_Graphics2DBlitSettings bset;

        NEXUS_Graphics2D_GetDefaultBlitSettings(&bset);
        bset.constantColor    = pBdrv->blitSettings.constantColor;
        bset.source.surface   = pBdrv->blitSettings.source.surface;
        bset.source.rect      = pBdrv->blitSettings.source.rect;
        bset.output.surface   = pBdrv->blitSettings.output.surface;
        bset.output.rect      = pBdrv->blitSettings.output.rect;
        bset.colorOp          = NEXUS_BlitColorOp_eCopySource;
        bset.alphaOp          = NEXUS_BlitAlphaOp_eCopySource;
        bset.horizontalFilter = pBdrv->blitSettings.horizontalFilter;
        bset.verticalFilter   = pBdrv->blitSettings.verticalFilter;

        bcmnexus_debugBlit("FastBlit Start", pBdrv, &bset);
        do {
            SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
            rc = NEXUS_Graphics2D_Blit(pBdrv->hGfx2D, &bset);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
            if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                bcmnexus_gfxWaitComplete( pBdrv, pBdev);
                continue;
            }
#endif
            break;
        } while(true);
    }
    else
    {
#ifndef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
        /* If the alpha premultiplication block is NOT enabled in the source scaler, then we need to do
           2 passes if using the blend block already to do an alpha or color blend. */
        if (!pBdrv->blitSingle)
        {
            struct NEXUS_BlendEquation be;
            NEXUS_Graphics2DBlitSettings bset;
            NEXUS_SurfaceCreateSettings  cset;
            NEXUS_SurfaceHandle h = pBdrv->hIntermediateSurface;

            /* We cannot support Source Color Keying in this mode, as the first pass in the M2MC
               will subtitute the source pixels with the destination pixels, but then the second pass will
               treat the destination pixels as source pixels and apply blending to them which is wrong!
            */
            if (pBdrv->blitSettings.colorKey.source.enabled == true)
            {
                return false;
            }

            /* create a temporary surface. It would be quicker to do this more statically */
            NEXUS_Surface_GetCreateSettings( pBdrv->blitSettings.source.surface, &cset );
            cset.heap = cset.pPaletteMemory = cset.pMemory = NULL;
            cset.alignment = 6; /* Ensure alignment is on a 64-byte boundary */

            /* We cannot do any pixel format changes when going from a palettised source
               to a palettised output surface */
            if (NEXUS_PIXEL_FORMAT_IS_PALETTE(cset.pixelFormat))
            {
                cset.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
                cset.pitch = 0;
            }

            /* if the temporary surface is unequal or too small, we ditch it */
            if (h)
            {
                NEXUS_SurfaceCreateSettings *t = &pBdrv->intermediateSurfaceSettings;

                if(   (t->pixelFormat != cset.pixelFormat)
                  ||  (t->width       <  cset.width)
                  ||  (t->height      <  cset.height)
                  ||  (t->alignment   != cset.alignment)
                  ||  (t->pitch       != cset.pitch)
                  ||  (t->heap        != cset.heap)
                  ||  (    NEXUS_PIXEL_FORMAT_IS_PALETTE(t->pixelFormat)
                        && (t->palettePixelFormat != cset.palettePixelFormat) ) )
                {

                    D_DEBUG_AT( bcmnexusGraphicsX, "NEXUS_Surface_Destroy() intermediate surface\n" );
                    NEXUS_Surface_Destroy( h );
                    pBdrv->hIntermediateSurface = h = 0;
                }
            }

            if (!h)
            {
                D_DEBUG_AT( bcmnexusGraphicsX, "NEXUS_Surface_Create() intermediate surface\n" );
                h = NEXUS_Surface_Create( &cset );
                if (!h) {
                    D_ERROR( "bcmNexus/Gfx: NEXUS_Surface_Create failed\n" );
                    return false;
                }
                pBdrv->intermediateSurfaceSettings = cset;
                pBdrv->hIntermediateSurface = h;
            }


            /* the first blit creates a temporary surface with alpha blending equations:
               Tc = Sc x Ta
               Ta = Sa x Ca
            */
            bset = pBdrv->blitSettings;
            bset.dest.surface   = NULL;
            bset.output.surface = h;
            bset.output.rect    = pBdrv->blitSettings.source.rect;
            bset.colorOp        = NEXUS_BlitColorOp_eUseBlendEquation;
            bset.alphaOp        = NEXUS_BlitAlphaOp_eCopySource;

            be.a = NEXUS_BlendFactor_eSourceAlpha;
            be.b = NEXUS_BlendFactor_eSourceColor;
            be.subtract_cd = 0;
            be.c = NEXUS_BlendFactor_eZero;
            be.d = NEXUS_BlendFactor_eZero;
            be.subtract_e = 0;
            be.e = NEXUS_BlendFactor_eZero;
            bset.colorBlend = be;

            bset.conversionMatrixEnabled = false;   /* Disable any conversion matrix on first blit */
#ifdef BCMNEXUS_ROP_SUPPORTED
            bset.patternSettingsEnabled = false;    /* Disable XOR on first blit */
#endif
            bset.colorKey.dest.enabled = false;     /* Disable Destination Color Keying on first blit */

            /* Disable any upscale/downscale filtering as we are not performing any scaling
               in the first blit. */
            bset.horizontalFilter = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
            bset.verticalFilter = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;

            bcmnexus_debugBlit("Blitting Start - First Pass", pBdrv, &bset);
            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_Blit(pBdrv->hGfx2D, &bset);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdev );
                    continue;
                }
#endif
                break;
            } while(true);

            if (rc == NEXUS_SUCCESS)
            {
                /* The second blit uses the following blending equations:
                   Oc = Tc * Ta + (1 - Ta) * Dc
                   Oa = Ta * Ta + (1 - Ta) * Da
                */
                bset = pBdrv->blitSettings;
                bset.source.surface = h;

                bcmnexus_debugBlit("Blitting Start - Second Pass", pBdrv, &bset);
                do {
                    SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                    rc = NEXUS_Graphics2D_Blit(pBdrv->hGfx2D, &bset);
#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                    if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                        bcmnexus_gfxWaitComplete( pBdrv, pBdev );
                        continue;
                    }
#endif
                    break;
                } while(true);
            }
        }
        else
#endif /* BCMNEXUS_ALPHA_PREMULT_SUPPORTED */
        {
            bcmnexus_debugBlit("Blitting Start", pBdrv, &pBdrv->blitSettings);
            do {
                SET_GRAPHICS2D_SYNC(pBdrv->pBrcmDfb, true);
                rc = NEXUS_Graphics2D_Blit(pBdrv->hGfx2D, &pBdrv->blitSettings);

#ifdef NEXUS_GRAPHICS2D_QUEUE_FULL
                if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                    bcmnexus_gfxWaitComplete( pBdrv, pBdev );
                    continue;
                }
#endif
                break;
            } while(true);
        }
    }
#endif
    return (rc == NEXUS_SUCCESS) ? true : false;
}

static bool bcmnexus_gfxStretchBlit( UNUSED_ void         *pDrv,
                                     UNUSED_ void         *pDev,
                                     UNUSED_ DFBRectangle *pRectSrc,
                                     UNUSED_ DFBRectangle *pRectDest )
{
    bcmnexus_gfxDriverData      *pBdrv    = (bcmnexus_gfxDriverData *)pDrv;
    bcmnexus_gfxDeviceData      *pBdevice = pDev;
    int                          x, y, w, h ,dx, dy, dw, dh, iw, ih;
    bool                         ok;
    bool                         twoPassesDownscale = false;
    NEXUS_SurfaceCreateSettings  createSurfaceSettings;
    DFBSurfacePixelFormat        pixelFormat;
    NEXUS_SurfaceHandle          hSur = pBdrv->hIntermediateSurface;
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    BM2MC_PACKET_Plane           packetOrigSrcPlane;
    BM2MC_PACKET_Plane           packetOrigDstPlane;
    BM2MC_PACKET_Plane           packetOrigOutPlane;
    BM2MC_PACKET_Plane           packetMediOutPlane;
    NEXUS_SurfaceMemory          memory;
#ifdef NEXUS_MAX_HEAPS
    NEXUS_MemoryStatus           memoryStatus;
#endif
    bool                         blendEquationChange = false;
#else
    NEXUS_Graphics2DBlitSettings origBlitSettings;
#endif
    int32_t                      fix12HorPhase, fix12VerPhase;
    uint32_t                     fix12HorScale, fix12VerScale;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    D_DEBUG_AT( bcmnexusGraphics, "%s() src addr=%p, dst addr=%p, out addr=%p [%dx,%dy,%dw,%dh]->[%dx,%dy,%dw,%dh]\n", __FUNCTION__,
                (void *)pBdrv->packetSettings.packetSrcPlane.address,
                pBdrv->packetSettings.packetDstPlane.address ? (void *)pBdrv->packetSettings.packetDstPlane.address : NULL,
                (void *)pBdrv->packetSettings.packetOutPlane.address,
                pRectSrc->x,  pRectSrc->y,  pRectSrc->w,  pRectSrc->h,
                pRectDest->x, pRectDest->y, pRectDest->w, pRectDest->h
              );
#else
    D_DEBUG_AT( bcmnexusGraphics, "%s() src sur=%p, dst sur=%p, out sur=%p [%dx,%dy,%dw,%dh]->[%dx,%dy,%dw,%dh]\n", __FUNCTION__,
                (void *)pBdrv->blitSettings.source.surface,
                pBdrv->blitSettings.dest.surface ? (void *)pBdrv->blitSettings.dest.surface : NULL,
                (void *)pBdrv->blitSettings.output.surface,
                pRectSrc->x,  pRectSrc->y,  pRectSrc->w,  pRectSrc->h,
                pRectDest->x, pRectDest->y, pRectDest->w, pRectDest->h
              );
#endif

     x = pRectSrc->x;
     y = pRectSrc->y;
     w = pRectSrc->w;
     h = pRectSrc->h;

     dx = pRectDest->x;
     dy = pRectDest->y;
     dw = pRectDest->w;
     dh = pRectDest->h;

     DFBRectangle srect = { x, y, w, h };
     DFBRectangle drect = { dx, dy, dw, dh };

    if (pBdrv->matrix_enabled) {
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        if (pBdrv->matrix_3d) {
            BP3D_Vertex V[4];
            DFBPoint p[4] = {
                { dx, dy },
                { dx + dw, dy },
                { dx + dw, dy + dh },
                { dx, dy + dh }
            };

            DFB_TRANSFORM( p[0].x, p[0].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[1].x, p[1].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[2].x, p[2].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[3].x, p[3].y, pBdrv->matrix, pBdrv->matrix_affine );

            memset( V, 0, sizeof(V) );

            for (int i = 0; i < 4; i++) {
                V[i].fX         = p[i].x;
                V[i].sX         = p[i].x;
                V[i].fY         = p[i].y;
                V[i].sY         = p[i].y;
                V[i].ulDiffuse  = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */
                V[i].ulSpecular = pBdrv->specularColor;    /* specular color in FRGB888, F=Fog */
                V[i].fW         = 1.0f;                    /* w value */
                V[i].fInvW      = 1.0f;                    /* inverse w value */
            }

            float s1 = pRectSrc->x / pBdrv->fSrcWidth;
            float t1 = pRectSrc->y / pBdrv->fSrcHeight;

            float s2 = (pRectSrc->x + pRectSrc->w) / pBdrv->fSrcWidth;
            float t2 = (pRectSrc->y + pRectSrc->h) / pBdrv->fSrcHeight;

            V[0].fS = s1;
            V[0].fT = t1;
            V[1].fS = s2;
            V[1].fT = t1;
            V[2].fS = s2;
            V[2].fT = t2;
            V[3].fS = s1;
            V[3].fT = t2;

            bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_3D, BCMNEXUS_GRAPHICS3D_BLIT_INC );

            if (BP3D_DrawTriangleFan( pBdrv->hP3d, V, 2 ))
                D_WARN("StretchBlit (3D)");

            return true;
        }
#endif
        bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_2D, BCMNEXUS_GRAPHICS2D_BLIT_INC );

        APPLY_MATRIX_RECT(dx,dy,dw,dh,pBdrv->matrix);
        drect.x = dx;
        drect.y = dy;
        drect.w = dw;
        drect.h = dh;

        /* nothing do - lies comletely out of destination */
        if (!dfb_clip_blit_precheck(&pBdrv->dst_clip, dw, dh, dx, dy))
            return true;
    }
    else
    {
        bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_2D, BCMNEXUS_GRAPHICS2D_BLIT_INC );
    }


    DFBSurfaceBlittingFlags sbf = DSBLIT_NOFX;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    if (pBdevice->packetHWStates.packetMirrorSrcVert)
        sbf |= DSBLIT_FLIP_VERTICAL;

    if (pBdevice->packetHWStates.packetMirrorSrcHorz)
        sbf |= DSBLIT_FLIP_HORIZONTAL;
#else
    if (pBdrv->blitSettings.mirrorSourceVertically)
        sbf |= DSBLIT_FLIP_VERTICAL;

    if (pBdrv->blitSettings.mirrorSourceHorizontally)
        sbf |= DSBLIT_FLIP_HORIZONTAL;
#endif

    soft_clip_stretchblit_flip( &srect, &drect, &pBdrv->dst_clip, sbf,
                                &fix12HorScale, &fix12VerScale, &fix12HorPhase, &fix12VerPhase );

    dx = drect.x;
    dy = drect.y;
    dw = drect.w;
    dh = drect.h;

    x = srect.x;
    y = srect.y;
    w = srect.w;
    h = srect.h;

    D_DEBUG_AT( bcmnexusGraphics, "%s() src: [%dx,%dy,%dh,%dw] -> dst: [%dx,%dy,%dw,%dh]\n", __FUNCTION__,
            x, y, w, h, dx, dy, dw, dh );

    /* Check to make sure width/height of source is not zero */
    if (!w || !h)
    {
        return true;    /* nothing to do */
    }

    /* Check to make sure width/height of destination is not zero */
    if (!dw || !dh)
    {
        return true;    /* nothing to do */
    }

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    /*SW7420-2179: Change default M2MC filtering to anisotropic when doing stretched blits downscale >2x */
    if(( w/dw > 2) || (h/dh > 2))
    {
        pBdevice->packetHWStates.packetVertFilter = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
        pBdevice->packetHWStates.packetHorzFilter = NEXUS_Graphics2DFilterCoeffs_eAnisotropic;
    }
#endif

    /* Check width/height scale limit */
    if ((w > dw * BCMNEXUS_GFX_SCALE_DOWN_MAX_X) || (h > dh * BCMNEXUS_GFX_SCALE_DOWN_MAX_Y))
    {
        /* Ensure that with 2 passes we don't exceed the maximum scale down.  If we do, then we have to fall
           back to using the generic software implementation instead... */
        if ((w <= dw * BCMNEXUS_GFX_SCALE_DOWN_MAX_X * BCMNEXUS_GFX_SCALE_DOWN_MAX_X) &&
            (h <= dh * BCMNEXUS_GFX_SCALE_DOWN_MAX_Y * BCMNEXUS_GFX_SCALE_DOWN_MAX_Y))
        {
            /* If we exceed maximum vertical or horizontal downscale, need to perform two passes using
               an intermediate buffer */
            twoPassesDownscale = true;
        }
        else
            return false;   /* let s/w do the blit */
    }

    /* Check to see if we are also doing any form of Horizontal mirroring.  If we are, then
       make sure that neither the source nor the destination/output horizontal number of pixels
       is greater than 128 (BGRC_P_STRIP_WIDTH_MAX).  If it is, then the hardware cannot do this
       in a single stripe and we will have to resort to using the s/w mirroring/rotation capability.
    */
#ifdef BCMNEXUS_MIRRORING_SUPPORTED
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    if (pBdevice->packetHWStates.packetMirrorSrcHorz == true)
#else
    if (pBdrv->blitSettings.mirrorSourceHorizontally == true)
#endif
    {
        if ((w > BCMNEXUS_GFX_STRIP_WIDTH_MAX) || (dw > BCMNEXUS_GFX_STRIP_WIDTH_MAX))
        {
            return false;   /* let s/w do the blit */
        }
    }
#endif

    /* Pre-check source/output overlap */
#ifndef BCMNEXUS_GFX_PACKET_BUFFER
    if (pBdrv->blitSettings.source.surface == pBdrv->blitSettings.output.surface)
    {
        DFBRectangle r1,r2;
        r1 = srect;
        r2 = drect;
        r1.w++; r1.h++;
        r2.w++; r2.h++;
        if (dfb_rectangle_intersect(&r1,&r2)) {
            D_WARN( "%s: rectangle intersect failed!\n", __FUNCTION__ );
            return false;  /* let s/w do the blit */
        }
    }
#endif

    if (pBdrv->odd_shift_enable)
    {
        if (dx & 1) dx--;
        if (dw & 1) dw--;

        if (!dw)
            return false;
    }

    if (twoPassesDownscale)
    {
        /* calculate the minimum width and height of intermediate surface */
        iw = (w > dw * BCMNEXUS_GFX_SCALE_DOWN_MAX_X) ? ((w + BCMNEXUS_GFX_SCALE_DOWN_MAX_X - 1) / BCMNEXUS_GFX_SCALE_DOWN_MAX_X) : dw;
        ih = (h > dh * BCMNEXUS_GFX_SCALE_DOWN_MAX_Y) ? ((h + BCMNEXUS_GFX_SCALE_DOWN_MAX_Y - 1) / BCMNEXUS_GFX_SCALE_DOWN_MAX_Y) : dh;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        pixelFormat = bcmnexus_getDFBSurfacePixelFormat(pBdrv->packetSettings.packetSrcPlane.format);
        NEXUS_Surface_GetDefaultCreateSettings(&createSurfaceSettings);
        createSurfaceSettings.pixelFormat = pBdrv->packetSettings.packetSrcPlane.format;
        createSurfaceSettings.alignment   = 12; /* Ensure surface is aligned on 4096Kbyte boundary to help prevent "cache crosstalk" */
        createSurfaceSettings.heap        = GET_OFFSCREEN_HEAP_HND(pBdrv->pBrcmDfb);
#else
        NEXUS_Surface_GetCreateSettings(pBdrv->blitSettings.source.surface, &createSurfaceSettings);
        pixelFormat = bcmnexus_getDFBSurfacePixelFormat(createSurfaceSettings.pixelFormat);
#endif
        if (pixelFormat == DSPF_YUY2 || pixelFormat == DSPF_UYVY)
            /* align size of YCbCr422 to 2 pixels */
            createSurfaceSettings.pitch   = ((iw + 1) & (~1)) * 2;
        else
            createSurfaceSettings.pitch   = DFB_BYTES_PER_LINE(pixelFormat, iw);

        createSurfaceSettings.width       = iw;
        createSurfaceSettings.height      = ih;

        if (ih * createSurfaceSettings.pitch <= pBdrv->intermediateSurfaceSize) {
            D_DEBUG_AT( bcmnexusGraphics, "  -> using preallocated memory for intermediate surface (%d/%d)\n",
                        ih * createSurfaceSettings.pitch, pBdrv->intermediateSurfaceSize );

            createSurfaceSettings.pMemory = pBdrv->intermediateSurfaceAddr;
        }
        else
            D_DEBUG_AT( bcmnexusGraphics, "  -> using new memory for intermediate surface (%d > %d)!!!\n",
                        ih * createSurfaceSettings.pitch, pBdrv->intermediateSurfaceSize );

        if (hSur)
        {
            NEXUS_SurfaceCreateSettings *t = &pBdrv->intermediateSurfaceSettings;

            if((t->pixelFormat != createSurfaceSettings.pixelFormat) ||
               (t->width       <  createSurfaceSettings.width)       ||
               (t->height      <  createSurfaceSettings.height)      ||
               (t->alignment   != createSurfaceSettings.alignment)   ||
               (t->pitch       != createSurfaceSettings.pitch)       ||
               (NEXUS_PIXEL_FORMAT_IS_PALETTE(t->pixelFormat) && (t->palettePixelFormat != createSurfaceSettings.palettePixelFormat)))
            {
                D_DEBUG_AT( bcmnexusGraphics, "%s() destroy intermediate surface=%p\n", __FUNCTION__, (void *)hSur );

                /* wait for M2MC to complete all remaining graphics 2D operations */
                bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_IDLE, 0 );

                NEXUS_Surface_Destroy(hSur);
                pBdrv->hIntermediateSurface = hSur = NULL;
            }
        }

        if (!hSur)
        {
            hSur = NEXUS_Surface_Create(&createSurfaceSettings);
            if (!hSur) {
                D_ERROR( "%s() create intermediate surface failed\n", __FUNCTION__ );
                return false;
            }

            bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_2D, BCMNEXUS_GRAPHICS2D_BLIT_INC );

            pBdrv->intermediateSurfaceSettings = createSurfaceSettings;
            pBdrv->hIntermediateSurface = hSur;

            D_DEBUG_AT( bcmnexusGraphics, "%s() create intermediate surface=%p w=%d h=%d pitch=%d\n", __FUNCTION__,
                        (void *)hSur, createSurfaceSettings.width, createSurfaceSettings.height, createSurfaceSettings.pitch );
        }

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        NEXUS_Surface_GetMemory(hSur, &memory);
        NEXUS_Heap_GetStatus(createSurfaceSettings.heap, &memoryStatus);

        packetMediOutPlane.address = memoryStatus.offset + (uint8_t*)memory.buffer - (uint8_t*)memoryStatus.addr;
        packetMediOutPlane.format  = createSurfaceSettings.pixelFormat;
        packetMediOutPlane.pitch   = createSurfaceSettings.pitch;

#ifndef BCMNEXUS_FIXEDSCALE_SUPPORTED
        bcmnexus_gfxPktGetPacketBuffer(pBdrv, pBdevice, GFX_TWO_PASSES_BLIT_PACKET_SIZE);
#else
        bcmnexus_gfxPktGetPacketBuffer(pBdrv, pBdevice, GFX_TWO_PASSES_BLIT_PACKET_SIZE + sizeof(BM2MC_PACKET_PacketFixedScale));
#endif

        /* record original configuration of output feeder, then assign intermediate surface to it */
        packetOrigOutPlane = pBdrv->packetSettings.packetOutPlane;
        pBdrv->packetSettings.packetOutPlane = packetMediOutPlane;
        BM2MC_PACKET_WRITE_OutputFeeder(pBdrv->packetSettings.packetBufNextPtr, pBdrv->packetSettings.packetOutPlane, false);

        packetOrigDstPlane.address = 0;
        if (pBdrv->packetSettings.packetDstPlane.address)
        {
            packetOrigDstPlane = pBdrv->packetSettings.packetDstPlane;
            pBdrv->packetSettings.packetDstPlane.address = 0;

            /* We don't need to submit DestinationNone packet, as destination feeder is not supported when doing ScaleBlit.
               ScaleBlit will still do a blend, but without the software striping, so we should set blending equation to
               just copy the pixels from source to destination.
            */
            if (bcmnexus_gfxPktCheckBlendEq(&pBdevice->packetHWStates.packetColorBlendOp, &colorEquation_BlitOp[NEXUS_BlitColorOp_eCopySource]) ||
                bcmnexus_gfxPktCheckBlendEq(&pBdevice->packetHWStates.packetAlphaBlendOp, &alphaEquation_BlitOp[NEXUS_BlitAlphaOp_eCopySource]))
            {
                /* Copy the source color & alpha to output with no blending. */
                BM2MC_PACKET_WRITE_Blend(pBdrv->packetSettings.packetBufNextPtr,
                                         colorEquation_BlitOp[NEXUS_BlitColorOp_eCopySource],
                                         alphaEquation_BlitOp[NEXUS_BlitAlphaOp_eCopySource],
                                         0,
                                         false);
                blendEquationChange = true;
            }
        }

#ifdef BCMNEXUS_FIXEDSCALE_SUPPORTED
        /* phase and step are set to 0 to have GRC calculate them for two passes downscale */
        bcmnexus_gfxPktSetFixedScalePacket(&pBdrv->packetSettings, pBdevice, 0, 0, 0, 0);
#endif

        if (pBdevice->packetHWStates.packetRop != 0xCC)
        {
            /* disable XOR raster operation */
            BM2MC_PACKET_WRITE_Rop(pBdrv->packetSettings.packetBufNextPtr, 0xCC, 0, 0, 0, 0, false);
        }

        if (pBdevice->packetHWStates.packetSrcColorkeyEnable)
        {
            /* disable source color key */
            BM2MC_PACKET_WRITE_SourceColorkeyEnable(pBdrv->packetSettings.packetBufNextPtr, false, false);
        }

        if (pBdevice->packetHWStates.packetDstColorkeyEnable)
        {
            /* disable destination color key */
            BM2MC_PACKET_WRITE_DestinationColorkeyEnable(pBdrv->packetSettings.packetBufNextPtr, false, false);
        }

        if (pBdevice->packetHWStates.packetColorMatrixEnable)
        {
            /* disable color matrix */
            BM2MC_PACKET_WRITE_SourceColorMatrixEnable(pBdrv->packetSettings.packetBufNextPtr, false, false);
        }

        if (pBdevice->packetHWStates.packetAlphaPremultiply)
        {
            /* disable source alpha premultiplication */
            BM2MC_PACKET_WRITE_AlphaPremultiply(pBdrv->packetSettings.packetBufNextPtr, false, false);
        }

        if (pBdevice->packetHWStates.packetMirrorSrcHorz || pBdevice->packetHWStates.packetMirrorSrcVert ||
            pBdevice->packetHWStates.packetMirrorDstHorz || pBdevice->packetHWStates.packetMirrorDstVert)
        {
            /* disable mirror block to avoid rotation and flips */
            BM2MC_PACKET_WRITE_Mirror(pBdrv->packetSettings.packetBufNextPtr, false, false, false, false, false, false, false);
        }

        if (pBdevice->packetHWStates.packetFilterEnable != 0)
        {
            /* disable filter */
            BM2MC_PACKET_WRITE_FilterEnable(pBdrv->packetSettings.packetBufNextPtr, false, false);
        }

        /* blit: source -> output(intermediate surface) */
        pBdrv->packetSettings.packetSrcRect.x = x;
        pBdrv->packetSettings.packetSrcRect.y = y;
        pBdrv->packetSettings.packetSrcRect.width  = w;
        pBdrv->packetSettings.packetSrcRect.height = h;

        pBdrv->packetSettings.packetOutRect.x = 0;
        pBdrv->packetSettings.packetOutRect.y = 0;
        pBdrv->packetSettings.packetOutRect.width  = iw;
        pBdrv->packetSettings.packetOutRect.height = ih;

        D_DEBUG_AT( bcmnexusGraphics, "%s() Downscale(using intermediate buffer) [%dx,%dy,%dw,%dh]->[0,0,%dw,%dh]\n", __FUNCTION__, x, y, w, h, iw, ih );

        bcmnexus_gfxPktSetBlitPacket(&pBdrv->packetSettings, pBdevice);

        /* record original configuration of source feeder, then assign intermediate surface to it */
        packetOrigSrcPlane = pBdrv->packetSettings.packetSrcPlane;
        pBdrv->packetSettings.packetSrcPlane = packetMediOutPlane;
        BM2MC_PACKET_WRITE_SourceFeeder(pBdrv->packetSettings.packetBufNextPtr, pBdrv->packetSettings.packetSrcPlane, pBdrv->packetSettings.packetConstantColor, false);

        /* get original configuration of output feeder back */
        pBdrv->packetSettings.packetOutPlane = packetOrigOutPlane;
        BM2MC_PACKET_WRITE_OutputFeeder(pBdrv->packetSettings.packetBufNextPtr, pBdrv->packetSettings.packetOutPlane, false);

        if (packetOrigDstPlane.address)
        {
            pBdrv->packetSettings.packetDstPlane = packetOrigDstPlane;

            if (blendEquationChange)
            {
                /* as blending equation is set to copy pixels only for previous ScaleBlit, we need to
                   recover blending for upcoming ScaleBlendBlit. */
                BM2MC_PACKET_WRITE_Blend(pBdrv->packetSettings.packetBufNextPtr,
                                         pBdevice->packetHWStates.packetColorBlendOp,
                                         pBdevice->packetHWStates.packetAlphaBlendOp,
                                         pBdevice->packetHWStates.packetBlendColor,
                                         false);
            }
        }

        if (pBdevice->packetHWStates.packetRop != 0xCC)
        {
            /* re-enable XOR raster operation */
            BM2MC_PACKET_WRITE_Rop(pBdrv->packetSettings.packetBufNextPtr, pBdevice->packetHWStates.packetRop, 0, 0, 0, 0, false);
        }

        if (pBdevice->packetHWStates.packetSrcColorkeyEnable)
        {
            /* re-enable source color key */
            BM2MC_PACKET_WRITE_SourceColorkeyEnable(pBdrv->packetSettings.packetBufNextPtr, true, false);
        }

        if (pBdevice->packetHWStates.packetDstColorkeyEnable)
        {
            /* re-enable destination color key */
            BM2MC_PACKET_WRITE_DestinationColorkeyEnable(pBdrv->packetSettings.packetBufNextPtr, true, false);
        }

        if (pBdevice->packetHWStates.packetColorMatrixEnable)
        {
            /* re-enable color matrix */
            BM2MC_PACKET_WRITE_SourceColorMatrixEnable(pBdrv->packetSettings.packetBufNextPtr, true, false);
        }

        if (pBdevice->packetHWStates.packetAlphaPremultiply)
        {
            /* re-enable source alpha premultiplication */
            BM2MC_PACKET_WRITE_AlphaPremultiply(pBdrv->packetSettings.packetBufNextPtr, true, false);
        }

        if (pBdevice->packetHWStates.packetMirrorSrcHorz || pBdevice->packetHWStates.packetMirrorSrcVert ||
            pBdevice->packetHWStates.packetMirrorDstHorz || pBdevice->packetHWStates.packetMirrorDstVert)
        {
            /* re-enable mirror block to perform rotation and flips */
            BM2MC_PACKET_WRITE_Mirror(pBdrv->packetSettings.packetBufNextPtr,
                                      pBdevice->packetHWStates.packetMirrorSrcHorz,
                                      pBdevice->packetHWStates.packetMirrorSrcVert,
                                      pBdevice->packetHWStates.packetMirrorDstHorz,
                                      pBdevice->packetHWStates.packetMirrorDstVert,
                                      false, false, false);
        }

        if (pBdevice->packetHWStates.packetFilterEnable != 0)
        {
            /* re-enable filter */
            BM2MC_PACKET_WRITE_FilterEnable(pBdrv->packetSettings.packetBufNextPtr, true, false);
        }

        pBdrv->packetSettings.packetHorPhase = 0;
        pBdrv->packetSettings.packetVerPhase = 0;
        pBdrv->packetSettings.packetHorStep  = 0;
        pBdrv->packetSettings.packetVerStep  = 0;

        /* blit: source(intermediate surface) -> output */
        pBdrv->packetSettings.packetSrcRect.x = 0;
        pBdrv->packetSettings.packetSrcRect.y = 0;
        pBdrv->packetSettings.packetSrcRect.width  = iw;
        pBdrv->packetSettings.packetSrcRect.height = ih;

        pBdrv->packetSettings.packetOutRect.x = dx;
        pBdrv->packetSettings.packetOutRect.y = dy;
        pBdrv->packetSettings.packetOutRect.width  = dw;
        pBdrv->packetSettings.packetOutRect.height = dh;

        if (packetOrigDstPlane.address)
        {
            pBdrv->packetSettings.packetDstRect.x = dx;
            pBdrv->packetSettings.packetDstRect.y = dy;
            pBdrv->packetSettings.packetDstRect.width  = dw;
            pBdrv->packetSettings.packetDstRect.height = dh;
        }
#else
        /* We don't force a specific scale factor and initial phase for two passes downscale */
        pBdrv->blitSettings.scalingControlSettingsEnabled = false;

        origBlitSettings = pBdrv->blitSettings;

        /* blit: source -> output(intermediate surface) */
        pBdrv->blitSettings.output.surface = hSur;
        pBdrv->blitSettings.dest.surface = NULL;

        pBdrv->blitSettings.patternSettingsEnabled        = false;
        pBdrv->blitSettings.colorKey.source.enabled       = false;
        pBdrv->blitSettings.colorKey.dest.enabled         = false;
        pBdrv->blitSettings.conversionMatrixEnabled       = false;
        pBdrv->blitSettings.mirrorSourceVertically        = false;
        pBdrv->blitSettings.mirrorSourceHorizontally      = false;
        pBdrv->blitSettings.mirrorDestVertically          = false;
        pBdrv->blitSettings.mirrorDestHorizontally        = false;
        pBdrv->blitSettings.alphaPremultiplySourceEnabled = false;

        pBdrv->blitSettings.source.rect.x = x;
        pBdrv->blitSettings.source.rect.y = y;
        pBdrv->blitSettings.source.rect.width  = w;
        pBdrv->blitSettings.source.rect.height = h;

        pBdrv->blitSettings.output.rect.x = 0;
        pBdrv->blitSettings.output.rect.y = 0;
        pBdrv->blitSettings.output.rect.width  = iw;
        pBdrv->blitSettings.output.rect.height = ih;

        D_DEBUG_AT( bcmnexusGraphics, "%s() Downscale(using intermediate buffer) [%dx,%dy,%dw,%dh]->[0,0,%dw,%dh]\n", __FUNCTION__,
                    x, y, w, h, iw, ih );

        ok = bcmnexus_gfxBlitCommon(pBdrv, pBdevice);
        if (ok == false) {
            D_ERROR("%s: Blitting failed!\n", __FUNCTION__);
            return false;
        }

        pBdrv->blitSettings = origBlitSettings;

        /* blit: source(intermediate surface) -> output */
        pBdrv->blitSettings.source.surface = hSur;

        pBdrv->blitSettings.source.rect.x = 0;
        pBdrv->blitSettings.source.rect.y = 0;
        pBdrv->blitSettings.source.rect.width  = iw;
        pBdrv->blitSettings.source.rect.height = ih;

        pBdrv->blitSettings.output.rect.x = dx;
        pBdrv->blitSettings.output.rect.y = dy;
        pBdrv->blitSettings.output.rect.width  = dw;
        pBdrv->blitSettings.output.rect.height = dh;

        if (pBdrv->blitSettings.dest.surface != NULL)
        {
            pBdrv->blitSettings.dest.rect.x = dx;
            pBdrv->blitSettings.dest.rect.y = dy;
            pBdrv->blitSettings.dest.rect.width  = dw;
            pBdrv->blitSettings.dest.rect.height = dh;
        }
#endif

        ok = bcmnexus_gfxBlitCommon(pBdrv, pBdevice);

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        /* get original configuration of source feeder back */
        pBdrv->packetSettings.packetSrcPlane = packetOrigSrcPlane;
        BM2MC_PACKET_WRITE_SourceFeeder(pBdrv->packetSettings.packetBufNextPtr, pBdrv->packetSettings.packetSrcPlane, pBdrv->packetSettings.packetConstantColor, false);
#else
        pBdrv->blitSettings.source.surface = origBlitSettings.source.surface;
#endif
    }
    else
    {
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
        /* Use fixed scaling with our modified rectangles to achieve fractional pixel accuracy. */
        pBdrv->packetSettings.packetHorPhase = fix12HorPhase;
        pBdrv->packetSettings.packetVerPhase = fix12VerPhase;
        pBdrv->packetSettings.packetHorStep  = fix12HorScale;
        pBdrv->packetSettings.packetVerStep  = fix12VerScale;

        pBdrv->packetSettings.packetSrcRect.x = x;
        pBdrv->packetSettings.packetSrcRect.y = y;
        pBdrv->packetSettings.packetSrcRect.width  = w;
        pBdrv->packetSettings.packetSrcRect.height = h;

        pBdrv->packetSettings.packetOutRect.x = dx;
        pBdrv->packetSettings.packetOutRect.y = dy;
        pBdrv->packetSettings.packetOutRect.width  = dw;
        pBdrv->packetSettings.packetOutRect.height = dh;

        if (pBdrv->packetSettings.packetDstPlane.address)
        {
            pBdrv->packetSettings.packetDstRect.x = dx;
            pBdrv->packetSettings.packetDstRect.y = dy;
            pBdrv->packetSettings.packetDstRect.width  = dw;
            pBdrv->packetSettings.packetDstRect.height = dh;
        }
#else
        /* To disable fixed scaling, GRC will then calculate the proper phase and scale values. */
        pBdrv->blitSettings.scalingControlSettingsEnabled = false;

        /* Set source rectangle.  Source surface is setup in SetState */
        pBdrv->blitSettings.source.rect.x = x;
        pBdrv->blitSettings.source.rect.y = y;
        pBdrv->blitSettings.source.rect.width  = w;
        pBdrv->blitSettings.source.rect.height = h;

        /* setup the output rectangle */
        pBdrv->blitSettings.output.rect.x = dx;
        pBdrv->blitSettings.output.rect.y = dy;
        pBdrv->blitSettings.output.rect.width  = dw;
        pBdrv->blitSettings.output.rect.height = dh;

        /* setup the destination rectangle if req'd */
        if (pBdrv->blitSettings.dest.surface != NULL)
        {
            pBdrv->blitSettings.dest.rect.x = dx;
            pBdrv->blitSettings.dest.rect.y = dy;
            pBdrv->blitSettings.dest.rect.width  = dw;
            pBdrv->blitSettings.dest.rect.height = dh;
        }
#endif

        ok = bcmnexus_gfxBlitCommon(pBdrv, pBdevice);
    }

    return ok;
}

static bool bcmnexus_gfxBlit2( void         *pDrv,
                               void         *pDev,
                               DFBRectangle *pRect,
                               int           dx,
                               int           dy,
                               int           sx2,
                               int           sy2 )
{
    bcmnexus_gfxDriverData *pBdrv    = (bcmnexus_gfxDriverData *)pDrv;
    bcmnexus_gfxDeviceData *pBdevice = pDev;
    int                     x, y, w, h;
    bool                    ok;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    D_DEBUG_AT( bcmnexusGraphics, "%s( %dx,%dy,%dw,%dh -> %dx,%dy,%dw,%dh ) src addr=%p, dst addr=%p, out addr=%p\n", __FUNCTION__,
                pRect->x, pRect->y, pRect->w, pRect->h, dx, dy, pRect->w, pRect->h,
                (void *)pBdrv->packetSettings.packetSrcPlane.address,
                pBdrv->packetSettings.packetDstPlane.address ? (void *)pBdrv->packetSettings.packetDstPlane.address : NULL,
                (void *)pBdrv->packetSettings.packetOutPlane.address );
#else
    D_DEBUG_AT( bcmnexusGraphics, "%s( %dx,%dy,%dw,%dh -> %dx,%dy,%dw,%dh ) src sur=%p, dst sur=%p, out sur=%p\n", __FUNCTION__,
                pRect->x, pRect->y, pRect->w, pRect->h, dx, dy, pRect->w, pRect->h,
                (void *)pBdrv->blitSettings.source.surface,
                pBdrv->blitSettings.dest.surface ? (void *)pBdrv->blitSettings.dest.surface : NULL,
                (void *)pBdrv->blitSettings.output.surface );
#endif

    DFB_RECTANGLES_DEBUG_AT( bcmnexusGraphics, pRect, 1 );

    x = pRect->x;
    y = pRect->y;
    w = pRect->w;
    h = pRect->h;


    DFBRectangle srect = { x, y, w, h };
    DFBRectangle drect = { dx, dy, w, h };

    if (pBdrv->matrix_enabled) {
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
        if (pBdrv->matrix_3d) {
            BP3D_Vertex V[4];
            DFBPoint p[4] = {
                { dx, dy },
                { dx + w, dy },
                { dx + w, dy + h },
                { dx, dy + h }
            };

            DFB_TRANSFORM( p[0].x, p[0].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[1].x, p[1].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[2].x, p[2].y, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( p[3].x, p[3].y, pBdrv->matrix, pBdrv->matrix_affine );

            memset( V, 0, sizeof(V) );

            for (int i = 0; i < 4; i++) {
                V[i].fX         = p[i].x;
                V[i].sX         = p[i].x;
                V[i].fY         = p[i].y;
                V[i].sY         = p[i].y;
                V[i].ulDiffuse  = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */
                V[i].ulSpecular = pBdrv->specularColor;    /* specular color in FRGB888, F=Fog */
                V[i].fW         = 1.0f;                    /* w value */
                V[i].fInvW      = 1.0f;                    /* inverse w value */
            }

            float s1 = pRect->x / pBdrv->fSrcWidth;
            float t1 = pRect->y / pBdrv->fSrcHeight;

            float s2 = (pRect->x + pRect->w) / pBdrv->fSrcWidth;
            float t2 = (pRect->y + pRect->h) / pBdrv->fSrcHeight;

            V[0].fS = s1;
            V[0].fT = t1;
            V[1].fS = s2;
            V[1].fT = t1;
            V[2].fS = s2;
            V[2].fT = t2;
            V[3].fS = s1;
            V[3].fT = t2;

            bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_3D, BCMNEXUS_GRAPHICS3D_BLIT_INC );

            if (BP3D_DrawTriangleFan( pBdrv->hP3d, V, 2 ))
                D_WARN("Blit (3D)");

            return true;
        }
#endif

        if (pBdrv->matrix_scales) {
            /* we have a scaled blit */
            return (bcmnexus_gfxStretchBlit(pBdrv, pBdevice, pRect, &drect));
        }

        bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_2D, BCMNEXUS_GRAPHICS2D_BLIT_INC );

        APPLY_MATRIX_RECT(dx,dy,w,h,pBdrv->matrix);
        drect.x = dx;
        drect.y = dy;

        if (!dfb_clip_blit_precheck(&pBdrv->dst_clip, w, h, dx, dy))
            return true;   /* nothing to do - lies comletely out of destination */
    }
    else
    {
        bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_2D, BCMNEXUS_GRAPHICS2D_BLIT_INC );
    }

    DFBSurfaceBlittingFlags sbf = DSBLIT_NOFX;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    if (pBdevice->packetHWStates.packetMirrorSrcVert)
        sbf |= DSBLIT_FLIP_VERTICAL;

    if (pBdevice->packetHWStates.packetMirrorSrcHorz)
        sbf |= DSBLIT_FLIP_HORIZONTAL;
#else
    if (pBdrv->blitSettings.mirrorSourceVertically)
        sbf |= DSBLIT_FLIP_VERTICAL;

    if (pBdrv->blitSettings.mirrorSourceHorizontally)
    {
        if (pBdrv->blitSettings.source.rect.width > BCMNEXUS_GFX_STRIP_WIDTH_MAX)
            return false; /*let software do the blit */

        sbf |= DSBLIT_FLIP_HORIZONTAL;
    }
#endif


    soft_clip_blit_flip( &srect, &drect, &pBdrv->dst_clip, sbf);

    sx2 += drect.x - dx;
    sy2 += drect.y - dy;

    dx = drect.x;
    dy = drect.y;

    x = srect.x;
    y = srect.y;
    w = srect.w;
    h = srect.h;


    /* Check to make sure width/height of source is not zero */
    if (!w || !h)
    {
        return true;    /* let s/w do the blit */
    }

    /* Pre-check source/output overlap */
#ifndef BCMNEXUS_GFX_PACKET_BUFFER
    if(pBdrv->blitSettings.source.surface==pBdrv->blitSettings.output.surface)
    {
        DFBRectangle r1,r2;
        r1 = r2 = srect;
        r2.x = dx;
        r2.y = dy;
        r1.w++; r1.h++;
        r2.w++; r2.h++;
        if(dfb_rectangle_intersect(&r1,&r2)) return false;    /* let s/w do the blit */
    }
#endif

    if (pBdrv->odd_shift_enable)
    {
        if (dx & 1) dx--;
        if (sx2 & 1) sx2--;
        if (w & 1) w--;

        if (!w)
            return false;
    }

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    pBdrv->packetSettings.packetHorPhase = 0;
    pBdrv->packetSettings.packetVerPhase = 0;
    pBdrv->packetSettings.packetHorStep  = 0;
    pBdrv->packetSettings.packetVerStep  = 0;

    pBdrv->packetSettings.packetSrcRect.x = x;
    pBdrv->packetSettings.packetSrcRect.y = y;
    pBdrv->packetSettings.packetSrcRect.width= w;
    pBdrv->packetSettings.packetSrcRect.height= h;

    pBdrv->packetSettings.packetOutRect.x = dx;
    pBdrv->packetSettings.packetOutRect.y = dy;
    pBdrv->packetSettings.packetOutRect.width= w;
    pBdrv->packetSettings.packetOutRect.height= h;

    if (pBdrv->matrix_enabled) {
        pBdrv->packetSettings.packetDstRect.x = dx;
        pBdrv->packetSettings.packetDstRect.y = dy;
    }
    else {
        pBdrv->packetSettings.packetDstRect.x = sx2;
        pBdrv->packetSettings.packetDstRect.y = sy2;
    }
    pBdrv->packetSettings.packetDstRect.width= w;
    pBdrv->packetSettings.packetDstRect.height= h;
#else
    pBdrv->blitSettings.scalingControlSettingsEnabled = false;

    /* Set source rectangle.  Source surface is setup in SetState */
    pBdrv->blitSettings.source.rect.x = x;
    pBdrv->blitSettings.source.rect.y = y;
    pBdrv->blitSettings.source.rect.width = w;
    pBdrv->blitSettings.source.rect.height = h;

    /* setup the output rectangle */
    pBdrv->blitSettings.output.rect.x = dx;
    pBdrv->blitSettings.output.rect.y = dy;
    pBdrv->blitSettings.output.rect.width = w;
    pBdrv->blitSettings.output.rect.height = h;

    /* setup the destination rectangle if req'd */
    if (pBdrv->blitSettings.dest.surface != NULL)
    {
        if (pBdrv->matrix_enabled) {
            pBdrv->blitSettings.dest.rect.x = dx;
            pBdrv->blitSettings.dest.rect.y = dy;
        }
        else {
            pBdrv->blitSettings.dest.rect.x = sx2;
            pBdrv->blitSettings.dest.rect.y = sy2;
        }
        pBdrv->blitSettings.dest.rect.width = w;
        pBdrv->blitSettings.dest.rect.height = h;
    }
#endif
    ok = bcmnexus_gfxBlitCommon(pBdrv, pBdevice);

    return ok;
}

static bool bcmnexus_gfxBlit( void         *pDrv,
                              void         *pDev,
                              DFBRectangle *pRect,
                              int           dx,
                              int           dy )
{
    return bcmnexus_gfxBlit2( pDrv, pDev, pRect, dx, dy, dx, dy );
}

#ifdef BCMNEXUS_GFX_HAS_P3D
static bool bcmnexus_gfxFillTriangles(         void        *pDrv,
                                      UNUSED_  void        *pDev,
                                               DFBTriangle *pTri,
                                               int          num )
{
    bool                    success  = false;
    bcmnexus_gfxDriverData *pBdrv    = (bcmnexus_gfxDriverData *)pDrv;
    BP3D_Vertex            *pV;
    BP3D_Vertex            *pFirstVertex;
    int                     i;

    D_DEBUG_AT( bcmnexusGraphics, "%s() dst sur=%p, out sur=%p, num=%d\n", __FUNCTION__,
                pBdrv->blitSettings.dest.surface   ? (void *)pBdrv->blitSettings.dest.surface   : NULL,
                (void *)pBdrv->blitSettings.output.surface, num
              );

    if (num > 500)
          pV = malloc( sizeof(BP3D_Vertex) * num * 3 );
    else
          pV = alloca( sizeof(BP3D_Vertex) * num * 3 );

    pFirstVertex = pV;

    memset(pFirstVertex, 0, sizeof(*pFirstVertex)*num*3);

    for (i = 0; i < num; i++)
    {
        if (pBdrv->matrix_enabled) {
            DFB_TRANSFORM( pTri->x1, pTri->y1, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( pTri->x2, pTri->y2, pBdrv->matrix, pBdrv->matrix_affine );
            DFB_TRANSFORM( pTri->x3, pTri->y3, pBdrv->matrix, pBdrv->matrix_affine );
        }

        pV->fX        = pTri->x1;
        pV->sX        = pTri->x1;
        pV->fY        = pTri->y1;
        pV->sY        = pTri->y1;
        pV->ulDiffuse = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */
        pV++;

        pV->fX        = pTri->x2;
        pV->sX        = pTri->x2;
        pV->fY        = pTri->y2;
        pV->sY        = pTri->y2;
        pV->ulDiffuse = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */
        pV++;

        pV->fX        = pTri->x3;
        pV->sX        = pTri->x3;
        pV->fY        = pTri->y3;
        pV->sY        = pTri->y3;
        pV->ulDiffuse = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */
        pV++;

        pTri++;
    }

    bcmnexus_gfxEnterMode( pBdrv, pDev, BCMNEXUS_GFX_3D, num * BCMNEXUS_GRAPHICS3D_FILL_TRI_INC );

    success = !BP3D_DrawTriangleList( pBdrv->hP3d, pFirstVertex, num );

    if (!success)
        D_WARN("FillTriangle(s)");

    if (num > 500)
         free( pFirstVertex );

    return success;
}

static bool bcmnexus_gfxFillTriangle(          void        *pDrv,
                                      UNUSED_  void        *pDev,
                                               DFBTriangle *pTri )
{
    UNUSED_ bcmnexus_gfxDriverData *pBdrv    = (bcmnexus_gfxDriverData *)pDrv;

    D_DEBUG_AT( bcmnexusGraphics, "%s() src sur=%p, dst sur=%p, out sur=%p [%dx1,%dy1,%dx2,%dy2,%dx3,%dy3]\n", __FUNCTION__,
                pBdrv->blitSettings.source.surface ? (void *)pBdrv->blitSettings.source.surface : NULL,
                pBdrv->blitSettings.dest.surface   ? (void *)pBdrv->blitSettings.dest.surface   : NULL,
                (void *)pBdrv->blitSettings.output.surface,
                pTri->x1, pTri->y1, pTri->x2, pTri->y2, pTri->x3, pTri->y3
              );

    return bcmnexus_gfxFillTriangles(pDrv, pDev, pTri, 1);
}

static bool bcmnexus_gfxDrawLine(          void      *pDrv,
                                  UNUSED_  void      *pDev,
                                           DFBRegion *pLine )
{
    bcmnexus_gfxDriverData *pBdrv   = (bcmnexus_gfxDriverData *)pDrv;
    bool                    success = false;
    BP3D_Vertex             V[2];
    DFBRegion               line;

    D_DEBUG_AT( bcmnexusGraphics, "%s() src sur=%p, dst sur=%p, out sur=%p [%dx1,%dy1,%dx2,%dy2]\n", __FUNCTION__,
                pBdrv->blitSettings.source.surface ? (void *)pBdrv->blitSettings.source.surface : NULL,
                pBdrv->blitSettings.dest.surface   ? (void *)pBdrv->blitSettings.dest.surface   : NULL,
                (void *)pBdrv->blitSettings.output.surface,
                pLine->x1, pLine->y1, pLine->x2, pLine->y2
              );

    line = *pLine;

    if (pBdrv->matrix_enabled) {
        DFB_TRANSFORM( line.x1, line.y1, pBdrv->matrix, pBdrv->matrix_affine );
        DFB_TRANSFORM( line.x2, line.y2, pBdrv->matrix, pBdrv->matrix_affine );
    }

    memset( V, 0, sizeof(V) );

    V[0].fX        = line.x1;
    V[0].sX        = line.x1;
    V[0].fY        = line.y1;
    V[0].sY        = line.y1;
    V[0].ulDiffuse = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */

    V[1].fX        = line.x2;
    V[1].sX        = line.x2;
    V[1].fY        = line.y2;
    V[1].sY        = line.y2;
    V[1].ulDiffuse = pBdrv->diffuseColor;     /* diffuse color in ARGB8888 */

    bcmnexus_gfxEnterMode( pBdrv, pDev, BCMNEXUS_GFX_3D, BCMNEXUS_GRAPHICS3D_DRAW_LINE_INC );

    success = !BP3D_DrawLine( pBdrv->hP3d, &V[0], &V[1] );

    if (!success)
        D_WARN("DrawLine(s)");

    return success;
}

static bool
bcmnexus_gfxTextureTriangles( void                *pDrv,
                              void                *pDev,
                              DFBVertex           *vertices,
                              int                  num,
                              DFBTriangleFormation formation )
{
     bool                    success  = false;
     bcmnexus_gfxDriverData *pBdrv    = (bcmnexus_gfxDriverData *)pDrv;
     bcmnexus_gfxDeviceData *pBdevice = pDev;
     int                     i;
     BP3D_Vertex            *v;

     D_DEBUG_AT( bcmnexusGraphics, "%s() src sur=%p, dst sur=%p, out sur=%p [%fx,%fy,%fz,%fw] [%fs %ft] num=%d formatation=%d\n", __FUNCTION__,
                 (void *)pBdrv->blitSettings.source.surface,
                 pBdrv->blitSettings.dest.surface ? (void *)pBdrv->blitSettings.dest.surface : NULL,
                 (void *)pBdrv->blitSettings.output.surface,
                 vertices->x, vertices->y, vertices->z, vertices->w, vertices->s, vertices->t,
                 num, formation
              );

     if (num > 500)
          v = malloc( sizeof(BP3D_Vertex) * num );
     else
          v = alloca( sizeof(BP3D_Vertex) * num );

     for (i=0; i<num; i++) {
          v[i].fX         = vertices[i].x;                  /* x coordinate */
          v[i].fY         = vertices[i].y;                  /* y coordinate */
          v[i].fZ         = vertices[i].z;                  /* z coordinate */
          v[i].fW         = vertices[i].w;                  /* w value */
          v[i].ulDiffuse  = pBdrv->diffuseColor;            /* diffuse color in ARGB8888 */
          v[i].ulSpecular = pBdrv->specularColor;           /* specular color in FRGB888, F=Fog */
          v[i].fInvW      = 1.0f / v[i].fW;                 /* inverse w value */
          v[i].sX         = vertices[i].x;                  /* integer x coordinate */
          v[i].sY         = vertices[i].y;                  /* integer y coordinate */
     }

     float fs = pBdrv->fSrcS;
     float ft = pBdrv->fSrcT;

     if (fs != 1.0f) {
         if (ft != 1.0f) {
             for (i=0; i<num; i++) {
                  v[i].fS         = vertices[i].s * fs;                  /* s texture coordinate */
                  v[i].fT         = vertices[i].t * ft;                  /* t texture coordinate */
             }
         }
         else {
             for (i=0; i<num; i++) {
                  v[i].fS         = vertices[i].s * fs;                  /* s texture coordinate */
                  v[i].fT         = vertices[i].t;                       /* t texture coordinate */
             }
         }
     }
     else {
         if (ft != 1.0f) {
             for (i=0; i<num; i++) {
                  v[i].fS         = vertices[i].s;                       /* s texture coordinate */
                  v[i].fT         = vertices[i].t * ft;                  /* t texture coordinate */
             }
         }
         else {
             for (i=0; i<num; i++) {
                  v[i].fS         = vertices[i].s;                       /* s texture coordinate */
                  v[i].fT         = vertices[i].t;                       /* t texture coordinate */
             }
         }
     }

     bcmnexus_gfxEnterMode( pBdrv, pBdevice, BCMNEXUS_GFX_3D, num * BCMNEXUS_GRAPHICS3D_TEXT_TRI_INC );

     switch (formation) {
          case DTTF_LIST:
               success = !BP3D_DrawTriangleList( pBdrv->hP3d, v, num / 3 );
               break;

          case DTTF_STRIP:
               success = !BP3D_DrawTriangleStrip( pBdrv->hP3d, v, (num - 1) / 2 );
               break;

          case DTTF_FAN:
               success = !BP3D_DrawTriangleFan( pBdrv->hP3d, v, num - 2 );
               break;
     }

     if (!success)
         D_WARN("TextureTriangle(s)!");


     if (num > 500)
          free( v );

     return success;
}
#endif

/* exported symbols for a graphics driver */

static int
driver_probe( UNUSED_ CoreGraphicsDevice *pDevice )
{

    D_DEBUG_AT( bcmnexusGraphics, "%s()\n", __FUNCTION__ );
    return 1;
}

static void
driver_get_info( UNUSED_ CoreGraphicsDevice *pDevice,
                         GraphicsDriverInfo *pInfo )
{
    /* fill driver info structure */
    snprintf( pInfo->name,
              DFB_GRAPHICS_DRIVER_INFO_NAME_LENGTH,
              BCM_DRIVER_NAME );

    snprintf( pInfo->vendor,
          DFB_GRAPHICS_DRIVER_INFO_VENDOR_LENGTH,
          BCM_VENDOR_NAME );

    pInfo->version.major = BCMNEXUS_GFX_MAJOR_VERSION;
    pInfo->version.minor = BCMNEXUS_GFX_MINOR_VERSION;

    pInfo->driver_data_size = sizeof (bcmnexus_gfxDriverData);
    pInfo->device_data_size = sizeof (bcmnexus_gfxDeviceData);
}

/* This get called by each DFB application before driver_init_device.
   NOTE: device data is shared, but driver data isn't.
 */
static DFBResult
driver_init_driver( UNUSED_ CoreGraphicsDevice  *device,
                            GraphicsDeviceFuncs *funcs,
                            void                *driver_data,
                    UNUSED_ void                *device_data,
                    UNUSED_ CoreDFB             *core )
{
    bcmnexus_gfxDriverData *pBdriver;
    bcmnexus_gfxDeviceData *pBdevice;
    DFBBCMNEXUS            *pBrcmDfb;           /*Brcm system data structure maintained by system module */
#ifdef BCMNEXUS_GFX_HAS_P3D
    int                     ret;
#endif

    D_DEBUG_AT( bcmnexusGraphics, "%s()\n", __FUNCTION__ );

    pBdriver = driver_data;
    pBdevice = device_data;

    /* Get the initialization information from the system */
    pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    pBdriver->pBrcmDfb=pBrcmDfb;

    pBdriver->hGfx2D = GET_GRAPHICS2D_HND(pBrcmDfb);

#ifdef BCMNEXUS_GFX_HAS_P3D
    DFB_Platform_P_Graphics3D_OpenP3D(&pBdriver->hP3d);

    if (!pBdriver->hP3d) {
          D_ERROR( "Broadcom/P3D: Could not get 3D handle!\n" );
          return DFB_INIT;
    }

    ret = BP3D_Context_Create( pBdriver->hP3d, &pBdriver->hContext );
    if (ret) {
          D_ERROR( "Broadcom/P3D: BP3D_Context_Create() failed with result 0x%04x\n", ret );
          return DFB_INIT;
    }

    BP3D_Context_SetCurrent( pBdriver->hP3d, pBdriver->hContext );

    BP3D_SetDepthCompare( pBdriver->hP3d, BP3D_Compare_Always );
    BP3D_SetShade( pBdriver->hP3d, BP3D_Shade_Flat );
    BP3D_SetTextureWrap( pBdriver->hP3d, BP3D_TextureWrap_Clamp );

    funcs->DrawLine         = bcmnexus_gfxDrawLine;
    funcs->FillTriangle     = bcmnexus_gfxFillTriangle;
    funcs->FillTriangles    = bcmnexus_gfxFillTriangles;
    funcs->TextureTriangles = bcmnexus_gfxTextureTriangles;
#endif

    /* Fill in function pointers */
    funcs->CheckState       = bcmnexus_gfxCheckState;
    funcs->SetState         = bcmnexus_gfxSetState;
    funcs->EngineSync       = bcmnexus_gfxEngineSync;
    funcs->CalcBufferSize   = bcmnexus_gfxCalcBufferSize;

    /* Fill in driver functions as implemented */
    funcs->DrawRectangle    = bcmnexus_gfxDrawRectangle;
    funcs->FillRectangle    = bcmnexus_gfxFillRectangle;
#ifdef BCMNEXUS_TRAPEZOID_SUPPORTED
    funcs->FillTrapezoid    = bcmnexus_gfxFillTrapezoid;
#endif
    funcs->Blit             = bcmnexus_gfxBlit;
    funcs->Blit2            = bcmnexus_gfxBlit2;
    funcs->StretchBlit      = bcmnexus_gfxStretchBlit;

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    funcs->EmitCommands     = bcmnexus_gfxEmitCommands;
#endif


    /* Secure fusion slaves shouldn't start GFX hardware */
    if (!fusion_config->secure_fusion || dfb_core_is_master(core))
    {
    /* Init the Fill and Blit setting once */
    NEXUS_Graphics2D_GetDefaultFillSettings(&pBdriver->fillSettings);
#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
    NEXUS_Graphics2D_GetDefaultPorterDuffFillSettings(&pBdriver->PorterDuffFillSettings);
#endif
    NEXUS_Graphics2D_GetDefaultBlitSettings(&pBdriver->blitSettings);
    pBdriver->blitSettings.conversionMatrix.shift = 8;
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    memset(&pBdriver->packetSettings, 0, sizeof(struct bcmnexus_gfxPacketSettings_tag));
    pBdriver->packetSettings.hGfx2D = GET_GRAPHICS2D_HND(pBrcmDfb);
    pBdriver->packetSettings.packetBufStartPtr = NULL;
    pBdriver->packetSettings.packetBufNextPtr = NULL;
#endif

    }

    return DFB_OK;
}


/* This get called only once by the master DFB application after driver_init_driver.
   NOTE: device data is shared, but driver data isn't.
 */
static DFBResult
driver_init_device( UNUSED_ CoreGraphicsDevice *pDevice,
                            GraphicsDeviceInfo *pDevice_info,
                            void               *pDriver_data,
                            void               *pDevice_data )
{
    NEXUS_Graphics2DSettings       settings;
    bcmnexus_gfxDriverData        *pBdriver = pDriver_data;
    bcmnexus_gfxDeviceData        *pBdevice = pDevice_data;
    pthread_mutexattr_t            mutexAttr;
    pthread_condattr_t             condAttr;
    NEXUS_MemoryAllocationSettings surfaceMemorySettings;

    D_DEBUG_AT( bcmnexusGraphics, "%s()\n", __FUNCTION__ );

    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_init(&condAttr);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

    /* Create the events and data for GFX EngineSync */
    pthread_cond_init(&pBdevice->syncCond, &condAttr);
    pthread_mutex_init(&pBdevice->syncLock, &mutexAttr);

#ifndef BCM740X_DEVICE_ID
    D_ERROR("bcmNexus/Graphics: Unknown chip %d or revision %0x - you may experience unexpected behavior\n", BCHP_CHIP , BCHP_VER);
#endif

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    /* Create the events and data for GFX PacketBlit Sync */
    pthread_cond_init(&pBdevice->packetSpaceAvailCond, &condAttr);
    pthread_mutex_init(&pBdevice->packetSpaceAvailLock, &mutexAttr);
#endif

    pthread_mutexattr_destroy(&mutexAttr);
    pthread_condattr_destroy(&condAttr);

    /* initialise the callback, and make sure it is part of the Master */
    NEXUS_Graphics2D_GetSettings(pBdriver->hGfx2D, &settings);
    settings.blockedSync       = false;
#ifdef BCMNEXUS_CHECKPOINT_IN_SETTINGS_SUPPORTED
    settings.pollingCheckpoint = false;
    settings.checkpointCallback.callback = checkpointCallback;
    settings.checkpointCallback.context  = pBdevice;
#else
    pBdevice->callback = 0;
#endif
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    settings.packetSpaceAvailable.callback = packetSpaceAvailableCallback;
    settings.packetSpaceAvailable.context = pBdevice;
#endif
    NEXUS_Graphics2D_SetSettings(pBdriver->hGfx2D, &settings);
    SET_GRAPHICS2D_SYNC(pBdriver->pBrcmDfb, true);

    /* fill device info */
    snprintf( pDevice_info->name,
          DFB_GRAPHICS_DEVICE_INFO_NAME_LENGTH, BCM_DRIVER_NAME );

    snprintf( pDevice_info->vendor,
          DFB_GRAPHICS_DEVICE_INFO_VENDOR_LENGTH, BCM_VENDOR_NAME );

    pDevice_info->caps.flags    = CCF_NOTRIEMU | CCF_RENDEROPTS | CCF_CLIPPING;
    pDevice_info->caps.accel    = bcmnexus_gfx_SUPPORTED_DRAWINGFUNCTIONS |
                                  bcmnexus_gfx_SUPPORTED_BLITTINGFUNCTIONS;
    pDevice_info->caps.clip     = DFXL_NONE;
#ifdef BCMNEXUS_GFX_HAS_P3D
    pDevice_info->caps.accel   |= bcmnexus_gfx_p3d_SUPPORTED_DRAWINGFUNCTIONS |
                                  bcmnexus_gfx_p3d_SUPPORTED_BLITTINGFUNCTIONS;
    pDevice_info->caps.clip    |= DFXL_FILLTRIANGLE | DFXL_FILLTRIANGLES | DFXL_TEXTRIANGLES;
#endif
    pDevice_info->caps.drawing  = bcmnexus_gfx_SUPPORTED_DRAWINGFLAGS;
    pDevice_info->caps.blitting = bcmnexus_gfx_SUPPORTED_BLITTINGFLAGS;

    /* align to longword boundry */
    pDevice_info->limits.surface_bytepitch_alignment  = 4;
    pDevice_info->limits.surface_byteoffset_alignment = 4;

#ifdef BCMNEXUS_GFX_HAS_P3D
    pDevice_info->limits.surface_max_power_of_two_pixelpitch = 2048;
    pDevice_info->limits.surface_max_power_of_two_height     = 2048;
#endif

    /* Ensure that the size of the destination surface is 1920x1080 or less */
    pDevice_info->limits.dst_max.w = BCMNEXUS_MAX_HD_DISPLAY_WIDTH;
    pDevice_info->limits.dst_max.h = BCMNEXUS_MAX_HD_DISPLAY_HEIGHT;



    /* Preallocate memory for intermediate surface for two pass down scaling */
    pBdriver->intermediateSurfaceSize = ((4096 + BCMNEXUS_GFX_SCALE_DOWN_MAX_X - 1) / BCMNEXUS_GFX_SCALE_DOWN_MAX_X) *
                                        ((2048 + BCMNEXUS_GFX_SCALE_DOWN_MAX_Y - 1) / BCMNEXUS_GFX_SCALE_DOWN_MAX_Y) * 4;

    NEXUS_Memory_GetDefaultAllocationSettings(&surfaceMemorySettings);
    surfaceMemorySettings.alignment = 4096;  /* Surface needs to be aligned on a 4K boundary */
    surfaceMemorySettings.heap = GET_OFFSCREEN_HEAP_HND(pBdriver->pBrcmDfb);

    if (NEXUS_Memory_Allocate((size_t)pBdriver->intermediateSurfaceSize, &surfaceMemorySettings, &pBdriver->intermediateSurfaceAddr) != NEXUS_SUCCESS)
    {
        D_ERROR( "bcmNexus/Pool: Insufficient memory to preallocate intermediate surface memory (%d bytes)\n", pBdriver->intermediateSurfaceSize);
        return DFB_NOVIDEOMEMORY;
    }

    return DFB_OK;
}

static void
driver_close_device( UNUSED_ CoreGraphicsDevice *pDevice,
                             void               *pDriver_data,
                             void               *pDevice_data )
{
    NEXUS_Graphics2DSettings settings;
    bcmnexus_gfxDriverData  *pBdriver = pDriver_data;
    bcmnexus_gfxDeviceData  *pBdevice = pDevice_data;

    D_DEBUG_AT( bcmnexusGraphics, "%s()\n", __FUNCTION__ );

    /* wait for the M2MC to complete any pending ops */
    bcmnexus_gfxEnterMode( pDriver_data, pBdevice, BCMNEXUS_GFX_IDLE, 0 );

    /* Free preallocated memory for intermediate surface for two pass down scaling */
    NEXUS_Memory_Free(pBdriver->intermediateSurfaceAddr);

    /* Ensure the callbacks are cleared */
    NEXUS_Graphics2D_GetSettings(pBdriver->hGfx2D, &settings);
#ifdef BCMNEXUS_CHECKPOINT_IN_SETTINGS_SUPPORTED
    settings.checkpointCallback.callback = NULL;
    settings.checkpointCallback.context  = NULL;
#endif
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    settings.packetSpaceAvailable.callback = NULL;
    settings.packetSpaceAvailable.context  = NULL;
#endif
    NEXUS_Graphics2D_SetSettings(pBdriver->hGfx2D, &settings);

    pthread_mutex_destroy( &pBdevice->syncLock );
    pthread_cond_destroy( &pBdevice->syncCond );

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    pthread_cond_destroy( &pBdevice->packetSpaceAvailCond );
    pthread_mutex_destroy( &pBdevice->packetSpaceAvailLock );
#endif
}

static void
driver_close_driver( UNUSED_ CoreGraphicsDevice *pDevice,
                     UNUSED_ void               *pDriver_data )
{
    bcmnexus_gfxDriverData *pBdriver = pDriver_data;

    D_DEBUG_AT( bcmnexusGraphics, "%s()\n", __FUNCTION__ );

    if (pBdriver->hIntermediateSurface)
    {
        D_DEBUG_AT( bcmnexusGraphics, "%s() destroy intermediate surface\n", __FUNCTION__ );
        NEXUS_Surface_Destroy(pBdriver->hIntermediateSurface);
        pBdriver->hIntermediateSurface = NULL;
    }

#ifdef BCMNEXUS_GFX_HAS_P3D
    BP3D_Context_Destroy( pBdriver->hP3d, pBdriver->hContext );
    DFB_Platform_P_Graphics3D_CloseP3D(pBdriver->hP3d);
#endif
}

