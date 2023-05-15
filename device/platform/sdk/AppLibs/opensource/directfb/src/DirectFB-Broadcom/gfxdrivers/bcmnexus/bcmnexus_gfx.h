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
 * $brcm_Workfile: bcmnexus_gfx.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/11 $
 *
 * $brcm_Date: 2/29/12 12:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/gfxdrivers/bcmnexus/bcmnexus_gfx.h $
 *
 * DirectFB_1_4_15_Port/11   2/29/12 12:17p robertwm
 * SW7420-2268: DirectFB-1.4.15: Require pre-allocated memory for
 *  intermediate surface buffer used in 2 pass d'scale.
 *
 * DirectFB_1_4_15_Port/10   2/14/12 2:15p christ
 * SW7231-652: DirectFB-1.4.15: Add chip defines for all AppLibs 3.0
 *  supported platforms.
 *
 * DirectFB_1_4_15_Port/9   1/10/12 12:34p christ
 * SW7231-580: DirectFB: Graphics card version checking / code cleanup
 *
 * DirectFB_1_4_15_Port/8   12/16/11 3:30p robertwm
 * SW7425-1987: DirectFB: Need to support 7425B2 version.
 *
 * DirectFB_1_4_15_Port/7   12/15/11 10:33a scchang
 * SW7425-1963: DirectFB: Hardware dithering support
 *
 * DirectFB_1_4_15_Port/6   10/25/11 5:09p christ
 * SW7231-416: DirectFB: Support latest 40nm parts required for Applibs
 *  2.2 release.
 *
 * DirectFB_1_4_15_Port/5   10/25/11 4:07p scchang
 * SW7420-2097: DirectFB: Add fixed scaling support to default when doing
 *  stretch blit.
 *
 * DirectFB_1_4_15_Port/4   10/7/11 4:30p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.15 release.
 *
 * DirectFB_1_4_5_Port/7   10/6/11 3:26p robertwm
 * SW7420-2079: DirectFB: MRC errors during stretch blit df_dok tests.
 *
 * DirectFB_1_4_5_Port/6   10/6/11 5:39p scchang
 * SW7420-2079: DirectFB: MRC errors during stretch blit df_dok tests
 *
 * DirectFB_1_4_14_Port/3   9/12/11 2:27p christ
 * SW7425-1272: DirectFB: Add support for 7346 / 7418 / 7425 B0 parts
 *
 * DirectFB_1_4_14_Port/2   8/19/11 5:04p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/5   9/12/11 2:25p christ
 * SW7425-1272: DirectFB: Add support for 7346 / 7418 / 7425 B0 parts
 *
 * DirectFB_1_4_5_Port/4   7/28/11 2:59p jackf
 * Updated to support 935230C1/935123C1.
 *
 * DirectFB_1_4_5_Port/4   7/28/11 2:49p jackf
 * Updated to support 935230C1/935123C1.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:20a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/3   7/21/11 5:26p christ
 * SW7552-84: DirectFB: Add support for 7552 SOC
 *
 * DirectFB_1_4_5_Port/1   6/16/11 3:08p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/40   6/10/11 5:39p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/7   4/29/11 1:49p tross
 * Rename blitBcc to use Bcc.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/6   4/13/11 5:01p tross
 * Correct typo.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/5   4/13/11 4:59p tross
 * Added 7640 chip info.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/4   4/11/11 7:07p tross
 * Merge changes to get STB to compile.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   4/7/11 8:03p tross
 * Merge from latest.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/28/11 11:00a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/39   5/5/11 11:24a robertwm
 * SW7358-42: Support DirectFB build for 7358.
 *
 * DirectFB_1_4_Port/38   3/30/11 6:20p christ
 * SW7425-254: DirectFB: Improve performance by replacing linux-fusion
 *  calls with pthreads
 *
 * DirectFB_1_4_Port/37   3/21/11 4:58p robertwm
 * SW7422-393: DirectFB: Hang in graphics driver due to serialisation of
 *  graphics packet-buffer.
 *
 * DirectFB_1_4_Port/36   3/17/11 12:43p robertwm
 * SW7422-384: DirectFB: Multi-application packet-buffer graphics fails
 *  with assertion.
 *
 * DirectFB_1_4_Port/35   3/8/11 6:33p apandav
 * SW35125-124: DirectFB Build fails on BCM35125
 *
 * DirectFB_1_4_Port/34   3/5/11 1:32a robertwm
 * SW7405-5179: DirectFB: NEXUS_GRAPHICS2D_QUEUE_FULL messages when
 *  running df_dok with Phase 7.5 refsw.
 *
 * DirectFB_1_4_Port/33   3/3/11 3:09p christ
 * SW7231-44: DirectFB: Add support for BCM7231A0 chip type.
 *
 * DirectFB_1_4_Port/32   2/24/11 8:16p raywu
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs
 *
 * DirectFB_1_4_Port/31   2/20/11 7:29p robertwm
 * SW7420-1525: DirectFB: Cannot build DirectFB with packet-buffer support
 *  on Phase 5.0.
 *
 * DirectFB_1_4_Port/30   2/18/11 9:47p robertwm
 * SW7422-279: DirectFB: Cannot see apps when pixelformat is YUY2 or UYVY.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:02p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/29   1/25/11 11:43a christ
 * SW7420-1411: DirectFB: Merge of features required for DirectFB 1.4.5
 *  phase 1.5 release - Merge in packet blit driver.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/20   1/4/11 4:25p raywu
 * SW7420-1346: DirectFB: Added dual source batch blit supported
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/19   1/3/11 4:42p scchang
 * SW7420-1338: Add A8 and other pixel formats support for FillTrapezoid()
 *  for M2MC core
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/18   12/23/10 11:42a christ
 * SW7425-61: DirectFB: Add support for BCM7425A0 chip type.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/17   12/16/10 7:08p robertwm
 * SW7344-17: DirectFB: Require support for BCM7344A0 chip.
 * SW7346-6: DirectFB: Require support for the BCM7346A0 chip.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/16   12/16/10 4:17p robertwm
 * SW7208-145: DirectFB: Need to support BCM7208 B0.
 * SW7340-240: DirectFB: Require support for BCM7340 variants.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/15   12/10/10 10:57a robertwm
 * SW7420-1309: DirectFB: Fix S/T co-ordinates for TextureTriangles using
 *  PX3D
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/14   12/8/10 6:17p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/13   12/5/10 4:31p robertwm
 * SW35230-2493: DirectFB does not compile for C0 chip version.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/12   12/3/10 8:37p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/11   11/25/10 8:36p raywu
 * SW7420-1134: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/10   11/19/10 5:49p raywu
 * SW7420-1251: Add Directfb packet buffer, multi-app, Kernel mode
 *  supported
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/9   11/17/10 9:40p raywu
 * SW7420-1134: DirectFB: Add packet buffer (blit) support for gfxdriver;
 *  Multi-App Kernel mode Supported
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/8   11/15/10 3:14p robertwm
 * SW7420-1241: DirectFB: Add support for FillTrapezoid() to the driver
 *  and core.
 * SW7420-1242: DirectFB: Add support for SetMatrix() function for both
 *  M2MC and PX3D cores.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/7   11/3/10 7:43p raywu
 * SW7420-1134 : DirectFB: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/6   11/2/10 7:30p raywu
 * SW7420-1134: DirectFB: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/5   10/29/10 6:50p raywu
 * SW7420-1134: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/4   10/28/10 8:13p scchang
 * SW7420-1134: Fix the drawing icon issue on web browser
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/3   10/27/10 5:44p robertwm
 * SW7420-1214: DirectFB: Add support for "EmitCommands" to our packet-
 *  blit version of the gfxdriver.
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/2   10/8/10 12:44p raywu
 * SW7420-1134: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/DirectFB_JIRA_SW7420_1134/1   10/4/10 8:20p raywu
 * SW7420-1134 : DirectFB: Add packet buffer (blit) support for gfxdriver
 *
 * DirectFB_1_4_Port/25   8/20/10 3:35p robertwm
 * SW7405-4791: DirectFB: DirectFB-1.4.1 Phase 2.5 fails to build with
 *  Nexus Phase 5.0 reference software.
 *
 * DirectFB_1_4_Port/24   8/11/10 4:58p robertwm
 * SW35230-957: DirectFB: platform_init fails to build on 35230 with
 *  recent 3D changes.
 *
 * DirectFB_1_4_Port/23   8/9/10 8:42p robertwm
 * SW7405-4183: DirectFB: Add support for Texture Triangles in the
 *  graphics driver.
 * SW7405-4734: DirectFB: Add hardware support for FillTriangle().
 *
 * DirectFB_1_4_Port/22   7/22/10 4:13p robertwm
 * SW7405-4649: DirectFB: Optimise surface locking and wait idle syncing.
 *
 * DirectFB_1_4_Port/21   7/15/10 11:05a robertwm
 * SW7342-230: DirectFB: Support DirectFB on BCM7342.
 *
 * DirectFB_1_4_Port/20   6/8/10 1:05p robertwm
 * SW7405-4468: DirectFB: Graphics driver fails to build if Nexus doesn't
 *  support source alpha premultiplication.
 * SW7468-252: DirectFB: Need to support 7468B0 chip revision.
 * SW7422-6: DirectFB: Need to support 7422A0 chip revision.
 * SW7420-816: DirectFB: Need to support 7420C1 chip revision.
 * SW35230-276: DirectFB: Support 35230A0 and B0 chip revision checking.
 *
 * DirectFB_1_4_Port/19   4/9/10 4:21p robertwm
 * SW7405-4183: DirectFB: Add support for Texture Triangles in the
 *  graphics driver.
 *
 * DirectFB_1_4_Port/18   3/2/10 10:37a robertwm
 * SW3556-1069: DirectFB: Run-time dynamic linking between gfxdriver and
 *  system drivers not working.
 *
 * DirectFB_1_4_Port/17   12/23/09 4:34p robertwm
 * SW7468-57: DirectFB: Support DirectFB on BCM7208 and BCM7468.
 * SW7405-3668: DirectFB: Support DirectFB on BCM7019.
 *
 * DirectFB_1_4_Port/16   12/23/09 2:19p robertwm
 * SW7550-51: Support DirectFB on 7550.
 *
 * DirectFB_1_4_Port/SW7550-51/1   12/22/09 3:22p mphillip
 * SW7550-51: Add 7550 build support (on branch)
 *
 * DirectFB_1_4_Port/15   12/4/09 9:19p robertwm
 * SW7405-3544: DirectFB: Maximum Vertical Downscale check is incorrect.
 * SW7405-3543: DirectFB: Use NEXUS_Graphics2D_FastBlit() for performing
 *  Copy Blits.
 *
 * DirectFB_1_4_Port/14   12/1/09 1:00p gmohile
 * SW7408-30 : Add 7125 and 7408 support
 *
 * DirectFB_1_4_Port/13   11/30/09 3:52p robertwm
 * SW7405-3471: Nexus needs to support additional pixel format for
 *  DirectFB.
 *
 * DirectFB_1_4_Port/12   11/27/09 2:03p robertwm
 * SW7405-3504: DirectFB: Add accelerated graphics support for
 *  DrawRectangle().
 * SW7405-3503: DirectFB: Destination color keying does not work properly
 *  when alpha premult block is unavailable.
 * SW7405-3502: Poor graphics performance of 1.3.x and 1.4.x compared with
 *  1.0.0.
 *
 * DirectFB_1_4_Port/11   11/25/09 10:52a robertwm
 * SW7405-3471: Nexus needs to support additional pixel formats for
 *  DirectFB.
 * SW7405-3475: Nexus does not perform source color keying correctly if
 *  any form of blending is required.
 * SW7405-3438: DirectFB: Palette based blits do not work correctly.
 * SW7405-3336: DirectFB: A8 blits are NOT displayed correctly.
 * SW7405-3481: DSRO_SMOOTH_UPSCALE not implemented.
 * SW7405-3122: DirectFB: Support different blending combinations,
 *  premultiply alpha ,XOR and mirorr/rotation.
 *
 * DirectFB_1_4_Port/10   10/30/09 2:49p robertwm
 * SW7400-2588: DFB: DirectFB FillRect renders differently with hardware
 *  and software surfaces.
 *
 * DirectFB_1_4_Port/9   10/12/09 6:34p robertwm
 * SW7405-3189: DirectFB: Disable alpha premult by default.
 *
 * DirectFB_1_4_Port/8   10/2/09 10:49a robertwm
 * SW7405-3122: DirectFB: Support different blending combinations,
 *  premultiply alpha ,XOR and mirorr/rotation.
 *
 * DirectFB_1_4_Port/7   8/27/09 6:35p ismailk
 * SW3556-800 : DeviceIds for 93556 added to bcmnexus_gfx.h
 *
 * DirectFB_1_4_Port/6   8/25/09 9:23p robertwm
 * SW7405-2935: DirectFB 1.4.1 introduced a bug when displaying bitmaps
 *  with an alpha channel.
 *
 * DirectFB_1_4_Port/5   8/7/09 3:56p robertwm
 * PR57500: Add DirectFB support for 7413 platform.
 *
 * DirectFB_1_4_Port/4   7/17/09 6:24p robertwm
 * PR56900: Possible font caching problem.
 *
 * DirectFB_1_4_Port/3   7/10/09 1:39p gmohile
 * PR 56731 : Add 7401 chip versions
 *
 * DirectFB_1_4_Port/2   7/9/09 1:31p mphillip
 * PR56658: Add 7403 defines
 *
 * DirectFB_1_4_Port/1   7/8/09 3:18p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 *
 * 9   3/16/09 4:43p mphillip
 * PR52806: Add support for DSBLIT_BLEND_COLORALPHA combined with
 *  DSBLIT_BLEND_ALPHACHANNEL
 *
 * 8   2/20/09 11:09a gmohile
 * PR 51629 : Add 7335 support
 *
 * 7   2/4/09 10:43a jrubio
 * PR51629: add 7336 support
 *
 * DirectFB_1_4_Port/4   6/28/09 9:05p robertwm
 * Add support for 7400 E0.
 *
 * DirectFB_1_4_Port/3   6/28/09 8:57p robertwm
 * Add support for 7420 A0,A1 and B0.
 *
 * DirectFB_1_4_Port/2   6/18/09 12:29p robertwm
 * Add in support for 7413A0,B0,B1 devices.
 *
 * 6   1/23/09 4:55p mphillip
 * PR50924: Enable premultiplying alpha blending options
 *
 * 5   12/10/08 1:47p gmohile
 * PR 50167 : Add 3458 B1/B2 support
 *
 * 4   11/6/08 8:23a gmohile
 * PR 45243 : Add 3548 B0 support
 *
 *
 ***************************************************************************/

#ifndef __bcmnexus_gfx_H__
#define __bcmnexus_gfx_H__

#include "bcmnexus.h"
#include "bcmnexus_utils.h"
#ifdef BCMNEXUS_GFX_HAS_P3D
#include <bp3d.h>
#include <nexus_graphics3d_init.h>
#include <nexus_graphics3d_priv.h>
#define NEXUS_THUNK_LAYER
#include <nexus_graphics2d_module.h>
#include <nexus_surface_priv.h>
#endif

#include "bcmnexus_gfx_platforms.h"

/* If updating any of the APIs or making any radical changes please rev the major version number.
   Any functional changes or bug fixes please upadte the minor version number */

#define BCMNEXUS_GFX_MAJOR_VERSION 4
#define BCMNEXUS_GFX_MINOR_VERSION 3

typedef enum {
     BCMNEXUS_GFX_IDLE,
     BCMNEXUS_GFX_2D,
     BCMNEXUS_GFX_3D
} bcmnexus_gfxMode;

typedef enum {
     m_None             = 0x0000,
     m_Source           = 0x0001,
     m_Destination      = 0x0002,
     m_Output           = 0x0004,
     m_SrcColorKey      = 0x0008,
     m_DstColorKey      = 0x0010,
     m_BlendEq          = 0x0020,
     m_Filter           = 0x0040,
     m_Matrix           = 0x0080,
     m_ColorMatrix      = 0x0100,
     m_AlphaPremult     = 0x0200,
     m_Mirror           = 0x0400,
     m_Rop              = 0x0800,
     m_All              = 0x0FFF

} BcmnexusStateBits;

#ifndef BCMNEXUS_GFX_PACKET_BUFFER

#define BCMNEXUS_VALIDATE(b)
#define BCMNEXUS_INVALIDATE(b)
#define BCMNEXUS_IS_VALID(b)      (false)
#define BCMNEXUS_ARE_VALID(b)     (false)

#else

#define BCMNEXUS_VALIDATE(b)      (pBdev->stateValidFlags |= (b))
#define BCMNEXUS_INVALIDATE(b)    (pBdev->stateValidFlags &= ~(b))
#define BCMNEXUS_IS_VALID(b)      (pBdev->stateValidFlags & (b))
#define BCMNEXUS_ARE_VALID(b)     (!((pBdev->stateValidFlags & (b)) ^ (b)))

typedef struct bcmnexus_gfxPacketHWStates_tag
{
    // Source Feeder
    BM2MC_PACKET_Plane                      packetSrcPlane;
    uint32_t                                packetSrcColor;
    bool                                    packetSrcNone;

    // Destination Feeder
    BM2MC_PACKET_Plane                      packetDstPlane;
    uint32_t                                packetDstColor;
    bool                                    packetDstNone;

    // Output Feeder
    BM2MC_PACKET_Plane                      packetOutPlane;

    // Blend Equation
    BM2MC_PACKET_Blend                      packetColorBlendOp;
    BM2MC_PACKET_Blend                      packetAlphaBlendOp;
    uint32_t                                packetBlendColor;

    // ROP
    uint32_t                                packetRop;

    // Source Color Key
    uint32_t                                packetSrcColorkey;
    int                                     packetSrcColorkeyEnable;

    // Destination Color Key
    uint32_t                                packetDstColorkey;
    int                                     packetDstColorkeyEnable;

    // Filter
    NEXUS_Graphics2DFilterCoeffs            packetHorzFilter;
    NEXUS_Graphics2DFilterCoeffs            packetVertFilter;
    BM2MC_PACKET_Rectangle                  packetFilterSrcRect;
    BM2MC_PACKET_Rectangle                  packetFilterOutRect;
    BM2MC_PACKET_FilterCoeffs               packetFilterHorzCoeffs;
    BM2MC_PACKET_FilterCoeffs               packetFilterVertCoeffs;
    int                                     packetFilterEnable;

    // Color Matrix
    NEXUS_Graphics2DColorMatrix             packetColorMatrix;
    int                                     packetColorMatrixEnable;

    // Alpha Premultiply
    int                                     packetAlphaPremultiply;

    // Mirror
    int                                     packetMirrorSrcHorz;
    int                                     packetMirrorSrcVert;
    int                                     packetMirrorDstHorz;
    int                                     packetMirrorDstVert;
    int                                     packetMirrorOutHorz;
    int                                     packetMirrorOutVert;

    // FixedScale
    int32_t                                 packetHorPhase;
    int32_t                                 packetVerPhase;
    uint32_t                                packetHorStep;
    uint32_t                                packetVerStep;

    // Dither
    bool                                    packetDither;
    bool                                    packetChromaFilter;

} bcmnexus_gfxPacketHWStates;

typedef struct bcmnexus_gfxPacketSettings_tag
{
    void *                                  packetBufStartPtr;
    void *                                  packetBufNextPtr;
    size_t                                  packetBufSize;

    NEXUS_Graphics2DHandle                  hGfx2D;

    uint32_t                                packetConstantColor;
    NEXUS_Graphics2DFilterCoeffs            packetHorzFilter;
    NEXUS_Graphics2DFilterCoeffs            packetVertFilter;
    BM2MC_PACKET_Rectangle                  packetSrcRect;
    BM2MC_PACKET_Rectangle                  packetOutRect;
    BM2MC_PACKET_Rectangle                  packetDstRect;

    BM2MC_PACKET_Plane                      packetSrcPlane;
    BM2MC_PACKET_Plane                      packetDstPlane;
    BM2MC_PACKET_Plane                      packetOutPlane;

    int32_t                                 packetHorPhase;
    int32_t                                 packetVerPhase;
    uint32_t                                packetHorStep;
    uint32_t                                packetVerStep;

    bool                                    packetSetRGBYUVColorMatrix;

}bcmnexus_gfxPacketSettings;
#endif

typedef struct
{
    pthread_mutex_t                         syncLock;
    pthread_cond_t                          syncCond;
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    pthread_cond_t                          packetSpaceAvailCond;
    pthread_mutex_t                         packetSpaceAvailLock;
    bcmnexus_gfxPacketHWStates              packetHWStates;
    DFBSurfaceDrawingFlags                  drawingflags;
    DFBSurfaceBlittingFlags                 blittingflags;
    DFBAccelerationMask                     accel;
    DFBSurfaceRenderOptions                 renderOptions;
    BcmnexusStateBits                       stateValidFlags;
#endif
    bcmnexus_gfxMode                        mode;
    unsigned int                            count;
#ifndef BCMNEXUS_CHECKPOINT_IN_SETTINGS_SUPPORTED
    void (*callback) (void *context, int param);
#endif
} bcmnexus_gfxDeviceData;

typedef struct
{
    NEXUS_Graphics2DHandle                  hGfx2D;
    DFBBCMNEXUS                            *pBrcmDfb;               /* pointer to brcm system data structure - needed for surf cache */
#ifndef BCMNEXUS_ALPHA_PREMULT_SUPPORTED
    bool                                    blitSingle;             /* set to false when 2 passes are required in the M2MC */
#endif
    bool                                    blitDest;               /* set to true when "destination" block in M2MC is enabled */
    bool                                    blitFast;               /* set to true when we need to do a fast blit */
    bool                                    odd_shift_enable;       /* set to true if checking for odd alignment in x or w is required */
    NEXUS_Pixel                             sourcePixelFormat;      /* Source surface Nexus pixel format */
    NEXUS_Graphics2DBlitSettings            blitSettings;
    NEXUS_Graphics2DFillSettings            fillSettings;
#ifdef BCMNEXUS_PORTERDUFF_FILL_SUPPORTED
    NEXUS_Graphics2DPorterDuffFillSettings  PorterDuffFillSettings;
#endif
#ifdef BCMNEXUS_GFX_PACKET_BUFFER
    bcmnexus_gfxPacketSettings              packetSettings;
#endif
    size_t                                  intermediateSurfaceSize;
    void                                   *intermediateSurfaceAddr;
    NEXUS_SurfaceCreateSettings             intermediateSurfaceSettings;
    NEXUS_SurfaceHandle                     hIntermediateSurface;   /* required when alpha blending AND colour blending */
#ifdef BCMNEXUS_GFX_HAS_P3D
    BP3D_Handle                             hP3d;
    BP3D_Context                            hContext;
    uint32_t                                diffuseColor;           /* diffuse color in ARGB8888 */
    uint32_t                                specularColor;          /* specular color in FRGB888, F=Fog */
    float                                   fSrcWidth;
    float                                   fSrcHeight;
    float                                   fSrcS;
    float                                   fSrcT;
#ifdef BCMNEXUS_SOFT3D_MATRIX_SUPPORTED
    bool                                    matrix_3d;
#endif
    bool                                    gfx_3d_support;
#endif
    s32                                     matrix[9];
    bool                                    matrix_affine;
    bool                                    matrix_enabled;
    bool                                    matrix_scales;
    bool                                    matrix_flips_x;
    bool                                    matrix_flips_y;
    DFBRegion                               dst_clip;

} bcmnexus_gfxDriverData;

#ifdef BCMNEXUS_PIXELFORMAT_R8_G8_B8_SUPPORTED
#define NEXUS_PIXEL_FORMAT_ALPHA_NONE(f)    \
(                                           \
    ((f) == NEXUS_PixelFormat_eR5_G6_B5) || \
    ((f) == NEXUS_PixelFormat_eR8_G8_B8)    \
)
#else
#define NEXUS_PIXEL_FORMAT_ALPHA_NONE(f)    \
(                                           \
    ((f) == NEXUS_PixelFormat_eR5_G6_B5)    \
)
#endif

#ifdef BCMNEXUS_GFX_PACKET_BUFFER
#define bcmnexus_gfx_SUPPORTED_DRAWINGFLAGS \
               (DSDRAW_BLEND | DSDRAW_DST_COLORKEY | DSDRAW_XOR |DSDRAW_SRC_PREMULTIPLY)
#else
#define bcmnexus_gfx_SUPPORTED_DRAWINGFLAGS \
               (DSDRAW_BLEND | DSDRAW_SRC_PREMULTIPLY)
#endif

#ifdef BCMNEXUS_TRAPEZOID_SUPPORTED
#define bcmnexus_gfx_SUPPORTED_DRAWINGFUNCTIONS \
               (DFXL_FILLRECTANGLE | DFXL_DRAWRECTANGLE | DFXL_FILLTRAPEZOID)
#else
#define bcmnexus_gfx_SUPPORTED_DRAWINGFUNCTIONS \
               (DFXL_FILLRECTANGLE | DFXL_DRAWRECTANGLE)
#endif

/* ZSP graphics support */
#if NEXUS_HAS_ZSP_GRAPHICS
#define bcmnexus_gfx_SUPPORTED_BLITTINGFLAGS  \
              ( DSBLIT_BLEND_ALPHACHANNEL   | \
                DSBLIT_BLEND_COLORALPHA     | \
                DSBLIT_COLORIZE             | \
                DSBLIT_SRC_PREMULTCOLOR     | \
                DSBLIT_XOR )

#define bcmnexus_gfx_SUPPORTED_BLITTINGFUNCTIONS \
              ( DFXL_BLIT )
#else
/* M2MC graphics support */
#define bcmnexus_gfx_SUPPORTED_BLITTINGFLAGS  \
              ( DSBLIT_BLEND_ALPHACHANNEL   | \
                DSBLIT_BLEND_COLORALPHA     | \
                DSBLIT_COLORIZE             | \
                DSBLIT_SRC_PREMULTIPLY      | \
                DSBLIT_SRC_COLORKEY         | \
                DSBLIT_DST_COLORKEY         | \
                DSBLIT_SRC_PREMULTCOLOR     | \
                DSBLIT_XOR                  | \
                DSBLIT_ROTATE180            | \
                DSBLIT_FLIP_HORIZONTAL      | \
                DSBLIT_FLIP_VERTICAL )

#define bcmnexus_gfx_SUPPORTED_BLITTINGFUNCTIONS \
              ( DFXL_BLIT                   |    \
                DFXL_BLIT2                  |    \
                DFXL_STRETCHBLIT )
#endif

/* PX3D graphics support */
#define bcmnexus_gfx_p3d_SUPPORTED_DRAWINGFLAGS \
              ( DSDRAW_BLEND                |   \
                DSDRAW_SRC_PREMULTIPLY )

#define bcmnexus_gfx_p3d_SUPPORTED_DRAWINGFUNCTIONS \
              ( DFXL_FILLRECTANGLE          |       \
                DFXL_DRAWRECTANGLE          |       \
                DFXL_FILLTRIANGLE           |       \
                DFXL_FILLTRIANGLES          |       \
                DFXL_FILLTRAPEZOID )

#define bcmnexus_gfx_p3d_SUPPORTED_BLITTINGFLAGS \
              ( DSBLIT_SRC_COLORKEY         |    \
                DSBLIT_BLEND_COLORALPHA     |    \
                DSBLIT_BLEND_ALPHACHANNEL   |    \
                DSBLIT_ROTATE180            |    \
                DSBLIT_FLIP_HORIZONTAL      |    \
                DSBLIT_FLIP_VERTICAL )

#define bcmnexus_gfx_p3d_SUPPORTED_BLITTINGFUNCTIONS \
              ( DFXL_BLIT                   |        \
                DFXL_STRETCHBLIT            |        \
                DFXL_TEXTRIANGLES )

/* Event timeout in milliseconds to wait for graphics to complete */
#define bcmnexus_gfx_EVENT_TIMEOUT 5000

/* Define Graphics 2D flush counts and increments */
#if defined(BCMNEXUS_GFX_PACKET_BUFFER)
#define BCMNEXUS_GRAPHICS2D_FLUSH_COUNT     4000
#define BCMNEXUS_GRAPHICS2D_DRAW_RECT_INC   1
#else
#define BCMNEXUS_GRAPHICS2D_FLUSH_COUNT     400
#define BCMNEXUS_GRAPHICS2D_DRAW_RECT_INC   4
#endif

#define BCMNEXUS_GRAPHICS2D_FILL_RECT_INC   1
#define BCMNEXUS_GRAPHICS2D_FILL_TRAP_INC   1
#define BCMNEXUS_GRAPHICS2D_BLIT_INC        1

/* Define Graphics 3D flush counts and increments */
#define BCMNEXUS_GRAPHICS3D_FLUSH_COUNT     400
#define BCMNEXUS_GRAPHICS3D_DRAW_RECT_INC   4
#define BCMNEXUS_GRAPHICS3D_DRAW_LINE_INC   1
#define BCMNEXUS_GRAPHICS3D_FILL_RECT_INC   4
#define BCMNEXUS_GRAPHICS3D_FILL_TRAP_INC   6
#define BCMNEXUS_GRAPHICS3D_FILL_TRI_INC    3
#define BCMNEXUS_GRAPHICS3D_TEXT_TRI_INC    3
#define BCMNEXUS_GRAPHICS3D_BLIT_INC        6

#define ARGB_MASK   0xFFFFFFFF
#define RGB_MASK    0x00FFFFFF
#define ROP_XOR     0x66
#define ROP_SOURCE  0xCC

/* Color conversion coefficient matrix helpers */
#define MAKE_CSC_C(c, s)      ((int32_t)((c) * (1 << (s))))

#define MAKE_CSC_MATRIX(s, p0c0, p0c1, p0c2, p0a, p0off,            \
                           p1c0, p1c1, p1c2, p1a, p1off,            \
                           p2c0, p2c1, p2c2, p2a, p2off,            \
                           ac0,  ac1,  ac2,  aa,  aoff)             \
{                                                                   \
    s,                                                              \
    {                                                               \
        MAKE_CSC_C(p0c0, s),                                        \
        MAKE_CSC_C(p0c1, s),                                        \
        MAKE_CSC_C(p0c2, s),                                        \
        MAKE_CSC_C(p0a, s),                                         \
        MAKE_CSC_C(p0off, s),                                       \
        MAKE_CSC_C(p1c0, s),                                        \
        MAKE_CSC_C(p1c1, s),                                        \
        MAKE_CSC_C(p1c2, s),                                        \
        MAKE_CSC_C(p1a, s),                                         \
        MAKE_CSC_C(p1off, s),                                       \
        MAKE_CSC_C(p2c0, s),                                        \
        MAKE_CSC_C(p2c1, s),                                        \
        MAKE_CSC_C(p2c2, s),                                        \
        MAKE_CSC_C(p2a, s),                                         \
        MAKE_CSC_C(p2off, s),                                       \
        MAKE_CSC_C(ac0, s),                                         \
        MAKE_CSC_C(ac1, s),                                         \
        MAKE_CSC_C(ac2, s),                                         \
        MAKE_CSC_C(aa, s),                                          \
        MAKE_CSC_C(aoff, s)                                         \
    }                                                               \
}

#endif
