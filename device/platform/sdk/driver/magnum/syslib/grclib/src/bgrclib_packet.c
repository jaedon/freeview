/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrclib_packet.c $
 * $brcm_Revision: Hydra_Software_Devel/50 $
 * $brcm_Date: 4/10/12 12:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/grclib/7425/bgrclib_packet.c $
 * 
 * Hydra_Software_Devel/50   4/10/12 12:35p jtna
 * SW7231-754: track src/out rects for changes in scaling factor
 * 
 * Hydra_Software_Devel/49   2/29/12 10:19a erickson
 * SW7420-2155: fix use of default mirror packet
 * 
 * Hydra_Software_Devel/48   2/16/12 3:45p nissen
 * SW7405-5427: Adding flags for output mirroring when blitting
 * 
 * Hydra_Software_Devel/47   12/8/11 10:57a erickson
 * SW7425-1921: note that BGRC_Output_ColorKeySelection is unused, but
 * extend BGRClib_BlitColorKeyParams for completeness
 * 
 * Hydra_Software_Devel/46   7/28/11 2:56p nissen
 * SW7405-5427: Modified to disable filter when not scaling in cases when
 * it is not required.
 * 
 * Hydra_Software_Devel/45   7/21/11 5:27p erickson
 * SW7420-1974: add separate dest and source constantColor
 * 
 * Hydra_Software_Devel/44   7/20/11 3:41p nissen
 * SW7405-5427: Added support for mirroring the output.
 * 
 * Hydra_Software_Devel/43   7/13/11 2:18p erickson
 * SW7420-1974: add BM2MC_PACKET_PacketDestinationColor back in
 * 
 * Hydra_Software_Devel/42   7/6/11 2:25p erickson
 * SW7420-1974: backing out use of BM2MC_PACKET_PacketDestinationColor
 * until fix for non-dest blits can be found
 * 
 * Hydra_Software_Devel/41   7/1/11 4:05p erickson
 * SW7420-1974: use BM2MC_PACKET_PacketDestinationColor if there's no
 * destination surface
 * 
 * Hydra_Software_Devel/40   5/31/11 1:16p erickson
 * SW7420-1200: remove unused functions, add missing const, clean up
 * comments
 * 
 * Hydra_Software_Devel/39   5/24/11 4:09p erickson
 * SW7346-149: remove unused functions
 * 
 * Hydra_Software_Devel/38   5/24/11 3:14p erickson
 * SW7420-1200: refactor grclib for optimal nexus use
 * 
 * Hydra_Software_Devel/37   5/23/11 4:50p erickson
 * SW7420-1200: refactor grclib for optimal nexus use
 * 
 * Hydra_Software_Devel/36   4/21/11 6:02p nissen
 * SW7335-1221: Fixed PorterDuff palette blit.
 * 
 * Hydra_Software_Devel/35   3/18/11 1:21p nissen
 * SW7420-1575: Added new width and height fields for surface planes.
 * 
 * Hydra_Software_Devel/34   1/6/11 5:04p erickson
 * SW7405-5090: fix possible div-by-zero
 * 
 * Hydra_Software_Devel/33   11/30/10 4:28p nissen
 * SW7405-3671: Fixed warnings.
 * 
 * Hydra_Software_Devel/32   11/29/10 1:50p nissen
 * SW7335-916: Fixed support for FixedScale packet.
 * 
 * Hydra_Software_Devel/31   11/16/10 2:54p erickson
 * SW7420-1200: when we run out of SW fifo memory for a blit, we should
 * fail with BERR_OUT_OF_DEVICE_MEMORY and no BERR_TRACE
 * 
 * Hydra_Software_Devel/30   11/11/10 1:35p nissen
 * SW7335-916: Added support for FixedScale packet.
 * 
 * Hydra_Software_Devel/29   9/20/10 4:33p nissen
 * SW7405-4869: Fixed Rop usage.
 * 
 * Hydra_Software_Devel/28   8/27/10 6:24p nissen
 * SW7405-3671: Added Porter Duff support.
 * 
 * Hydra_Software_Devel/27   8/5/10 6:04p nissen
 * SW7405-3671: Fixed output offset packet and warnings.
 *
 * Hydra_Software_Devel/26   7/28/10 11:53a erickson
 * SW7405-3671: add #if GRCPACKET_SUPPORT logic
 *
 * Hydra_Software_Devel/25   7/12/10 2:25p nissen
 * SW7405-3671: Updated to work with new packet api changes.
 *
 * Hydra_Software_Devel/24   6/30/10 8:18p nissen
 * SW7405-3671: Fixed support for pixel format.
 *
 * Hydra_Software_Devel/23   6/30/10 7:54p nissen
 * SW7405-3671: Added to support blending and pixel format enums for
 * packets.
 *
 * Hydra_Software_Devel/22   6/24/10 2:22p nissen
 * SW7405-3671: Added source feeder destripe packet function.
 *
 * Hydra_Software_Devel/21   6/21/10 12:45p nissen
 * SW7405-3671: Added context support. Switched to M2MC packets.
 *
 * Hydra_Software_Devel/20   3/19/10 2:24p nissen
 * SW7405-3671: Fixed colorkey.
 *
 * Hydra_Software_Devel/19   3/15/10 1:34p nissen
 * SW7405-3671: Removed unnecessary call to get palette.
 *
 * Hydra_Software_Devel/18   3/11/10 1:14p nissen
 * SW7405-3671: Added support for alpha premultiply.
 *
 * Hydra_Software_Devel/17   3/10/10 2:42p nissen
 * SW7405-3671: Added offset buffer pointers to batch packet blits.
 *
 * Hydra_Software_Devel/16   2/16/10 4:32p nissen
 * SW7405-3671: Fixed warnings.
 *
 * Hydra_Software_Devel/15   2/10/10 2:20p nissen
 * SW7405-3671: Fixed palette to palette blits.
 *
 * Hydra_Software_Devel/14   2/9/10 4:26p nissen
 * SW7405-3671: Updated batch blits.
 *
 * Hydra_Software_Devel/13   2/8/10 11:25p nissen
 * SW7405-3671: Added support for eUseBlendFactors.
 *
 * Hydra_Software_Devel/12   2/8/10 9:07p nissen
 * SW7405-3671: Added use of surface ID for identifying surfaces.
 *
 * Hydra_Software_Devel/11   2/8/10 6:04p nissen
 * SW7405-3671: Updated batching to submit as many blits as possible.
 *
 * Hydra_Software_Devel/10   2/5/10 8:24p nissen
 * SW7405-3671: Fixed palette to palette blits.
 *
 * Hydra_Software_Devel/9   2/5/10 4:35p nissen
 * SW7405-3671: Fixed problem with getting color.
 *
 * Hydra_Software_Devel/8   2/5/10 12:07p nissen
 * SW7405-3671: Fixed problem with init packet.
 *
 * Hydra_Software_Devel/7   2/5/10 10:51a nissen
 * SW7405-3671: Moved packet function.
 *
 * Hydra_Software_Devel/6   2/3/10 2:14p nissen
 * SW7405-3671: Added batch packets.
 *
 * Hydra_Software_Devel/5   1/29/10 7:30p nissen
 * SW7405-3671: Added support for using packets in any order.
 *
 * Hydra_Software_Devel/4   1/19/10 12:39p nissen
 * SW7405-3671: Added support for sync'ing and 420 destriping.
 *
 * Hydra_Software_Devel/3   1/8/10 2:23p nissen
 * SW7405-3671: Removed sync function.
 *
 * Hydra_Software_Devel/2   1/7/10 5:21p nissen
 * SW7405-3671: Fixed reset when syncing.
 *
 * Hydra_Software_Devel/1   1/6/10 10:29p nissen
 * SW7405-3671: Packet blits.
 *
 ***************************************************************************/

#include "bgrclib_packet.h"
#include "bstd.h"
#include "berr.h"
#include "berr_ids.h"
#include "bdbg.h"
#include "bkni.h"

#include "bm2mc_packet.h"
#include "bgrc_packet.h"
#include "bgrc_packet_write.h"

BDBG_MODULE(BGRClib);

BDBG_OBJECT_ID(GRClib);

typedef struct BGRClib_P_Data_tag
{
    BDBG_OBJECT(GRClib)
    BGRC_Handle hGRC;
    BGRC_PacketContext_Handle hContext;
    bool defaultRopPacket;
    bool defaultSourceColorkeyPacket;
    bool defaultDstColorkeyPacket;
    bool defaultBlitBlendPacket;
    bool defaultFillBlendPacket;
    bool defaultSourceFeederPacket;
    bool defaultDestinationFeederPacket;
    bool defaultColorMatrixPacket;
    bool defaultAlphaPremultiplyPacket;
    bool defaultMirrorPacket;
    bool defaultFixedScalePacket;
    struct {
        BGRC_FilterCoeffs vertCoeffs, horzCoeffs;
        struct {
            uint16_t width, height;
        } src, out;
    } filterPacket;
}
BGRClib_P_Data;

#define DEFAULT_COLOR (0xFF000000)

#if 0
/* some helpful debug code */
static const char *g_blendFactor[] = 
{
    "Zero",
    "Half",
    "One",
    "SourceColor",
    "InverseSourceColor",
    "SourceAlpha",
    "InverseSourceAlpha",
    "DestinationColor",
    "InverseDestinationColor",
    "DestinationAlpha",
    "InverseDestinationAlpha",
    "ConstantColor",
    "InverseConstantColor",
    "ConstantAlpha",
    "InverseConstantAlpha"
};

static void print_eq(const char *name, const BM2MC_PACKET_Blend *eq)
{
    BDBG_WRN(("%s blend: %s * %s %c %s * %s %c %s", name,
        g_blendFactor[eq->a], g_blendFactor[eq->b], eq->sub_cd?'-':'+', 
        g_blendFactor[eq->c], g_blendFactor[eq->d], eq->sub_e?'-':'+', 
        g_blendFactor[eq->e]));
}
#endif

/* for maximum perfornace, even in debug mode:
- do not use BDBG_ENTER/LEAVE. there are other ways to trace calls and we don't want to risk DBG overhead.
- do not BDBG_ASSERT on pointers. it's a marginal check anyway, and if it's NULL we're going to crash anyway.
*/

/*****************************************************************************/
static BM2MC_PACKET_Blend g_aGRClib_Packet_ColorEquation_FillOp[] = {
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0,BM2MC_PACKET_BlendFactor_eZero }
};
static BM2MC_PACKET_Blend g_aGRClib_Packet_AlphaEquation_FillOp[] = {
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero }
};
static BM2MC_PACKET_Blend g_aGRClib_Packet_ColorEquation_BlitOp[] = {
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero }
};
static BM2MC_PACKET_Blend g_aGRClib_Packet_AlphaEquation_BlitOp[] = {
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, BM2MC_PACKET_BlendFactor_eDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero }
};
static BM2MC_PACKET_Blend g_aGRClib_Packet_ColorEquation_PorterDuffFillOp[] = {
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0,  BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero }
};
static BM2MC_PACKET_Blend g_aGRClib_Packet_AlphaEquation_PorterDuffFillOp[] = {
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha,   BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseConstantAlpha, 0,  BM2MC_PACKET_BlendFactor_eZero }
};
static BM2MC_PACKET_Blend g_aGRClib_Packet_ColorEquation_PorterDuffBlitOp[] = {
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationColor, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero }
};
static BM2MC_PACKET_Blend g_aGRClib_Packet_AlphaEquation_PorterDuffBlitOp[] = {
    { BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, 0, BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, 0, BM2MC_PACKET_BlendFactor_eZero },
    { BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, 0, BM2MC_PACKET_BlendFactor_eDestinationAlpha, BM2MC_PACKET_BlendFactor_eInverseSourceAlpha, 0, BM2MC_PACKET_BlendFactor_eZero }
};

/*****************************************************************************/
#define GRCLIB_FULL_PACKET_SIZE (2 * \
    sizeof (BM2MC_PACKET_PacketSourceFeeder) + \
    sizeof (BM2MC_PACKET_PacketDestinationFeeder) + \
    sizeof (BM2MC_PACKET_PacketOutputFeeder) + \
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
    sizeof (BM2MC_PACKET_PacketDestripeBlit))

/*****************************************************************************/
static void BGRClib_P_GetPacketRectangle(
    const BM2MC_PACKET_Plane *pSurface,
    const BRect *pInRect,
    BM2MC_PACKET_Rectangle *pOutRect )
{
    if( pInRect && pInRect->width && pInRect->height)
    {
        pOutRect->x = pInRect->x;
        pOutRect->y = pInRect->y;
        pOutRect->width = pInRect->width;
        pOutRect->height = pInRect->height;
    }
    else {
        pOutRect->x = 0;
        pOutRect->y = 0;
        if ( pSurface ) {
            pOutRect->width = pSurface->width;
            pOutRect->height = pSurface->height;
        }
        else {
            pOutRect->width = 0;
            pOutRect->height = 0;
        }
    }
}

/*****************************************************************************/
static void BGRClib_P_GetBlendEquation( BM2MC_PACKET_Blend *pEquation, const BGRClib_BlendEquation *pBlend )
{
    if( pEquation && pBlend )
    {
        pEquation->a = pBlend->a;
        pEquation->b = pBlend->b;
        pEquation->sub_cd = pBlend->subcd;
        pEquation->c = pBlend->c;
        pEquation->d = pBlend->d;
        pEquation->sub_e = pBlend->sube;
        pEquation->e = pBlend->e;
    }
}

/*****************************************************************************/

static BERR_Code BGRC_Packet_SetSourceSurfacePacket2( BGRC_Handle hGrc, void **ppPacket, 
    const BM2MC_PACKET_Plane *pSrcSurface0, const BM2MC_PACKET_Plane *pSrcSurface1, uint32_t color )
{
    if( pSrcSurface0 && pSrcSurface1 )
    {
        BM2MC_PACKET_PacketSourceFeeders *pPacket = (BM2MC_PACKET_PacketSourceFeeders *) (*ppPacket);
        BM2MC_PACKET_INIT( pPacket, SourceFeeders, false );
        pPacket->plane0 = *pSrcSurface0;
        pPacket->plane1 = *pSrcSurface1;
        pPacket->color = color;
        BM2MC_PACKET_TERM( pPacket );
        *ppPacket = (void *) pPacket;
    }
    else if( pSrcSurface0 )
    {
        BM2MC_PACKET_PacketSourceFeeder *pPacket = (BM2MC_PACKET_PacketSourceFeeder *) (*ppPacket);
        BM2MC_PACKET_INIT( pPacket, SourceFeeder, false );
        pPacket->plane = *pSrcSurface0;
        pPacket->color = color;
        BM2MC_PACKET_TERM( pPacket );
        *ppPacket = (void *) pPacket;
    }
    else
    {
        BM2MC_PACKET_PacketSourceColor *pPacket = (BM2MC_PACKET_PacketSourceColor *) (*ppPacket);
        BM2MC_PACKET_INIT( pPacket, SourceColor, false );
        pPacket->color = color;
        BM2MC_PACKET_TERM( pPacket );
        *ppPacket = (void *) pPacket;
    }
    BSTD_UNUSED(hGrc);
    return BERR_SUCCESS;
}

#define BGRClib_P_SetDefaultSourceFeederPacket(grclib, pPacket) \
    ((grclib)->defaultSourceFeederPacket?(pPacket):BGRClib_P_SetSourceFeederPacket( (grclib), (pPacket), 0, DEFAULT_COLOR))
    
static uint32_t *BGRClib_P_SetSourceFeederPacket( BGRClib_Handle grclib,
    void *pPacket, const BM2MC_PACKET_Plane *pSrcSurface, uint32_t color )
{
    grclib->defaultSourceFeederPacket = (!pSrcSurface && color == DEFAULT_COLOR);
    BGRC_Packet_SetSourceSurfacePacket2( grclib->hGRC, &pPacket, pSrcSurface, 0, color );
    return pPacket;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_SetSourceDestripePacket( BGRClib_Handle grclib,
    void *pPacket, uint32_t lumaOffset, uint32_t chromaOffset, uint32_t pitch )
{
    /* BGRC_Packet_SetSourceDestripePacket uses the same packet as BGRC_Packet_SetSourceSurfacePacket2 */
    grclib->defaultSourceFeederPacket = false;
    BGRC_Packet_SetSourceDestripePacket( grclib->hGRC, &pPacket, lumaOffset, chromaOffset, pitch, DEFAULT_COLOR );
    return pPacket;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetDestinationSurfacePacket2( BGRC_Handle hGrc, void **ppPacket, 
    const BM2MC_PACKET_Plane *pSurface, uint32_t color )
{
    if( pSurface )
    {
        BM2MC_PACKET_PacketDestinationFeeder *pPacket = (BM2MC_PACKET_PacketDestinationFeeder *) (*ppPacket);
        BM2MC_PACKET_INIT( pPacket, DestinationFeeder, false );
        pPacket->plane = *pSurface;
        pPacket->color = color;
        BM2MC_PACKET_TERM( pPacket );
        *ppPacket = (void *) pPacket;
    }
    else
    {
        BM2MC_PACKET_PacketDestinationColor *pPacket = (BM2MC_PACKET_PacketDestinationColor *) (*ppPacket);
        BM2MC_PACKET_INIT( pPacket, DestinationColor, false );
        pPacket->color = color;
        BM2MC_PACKET_TERM( pPacket );
        *ppPacket = (void *) pPacket;
    }
    BSTD_UNUSED(hGrc);
    return BERR_SUCCESS;
}

#define BGRClib_P_SetDefaultDestinationFeederPacket(grclib, pPacket) \
    ((grclib)->defaultDestinationFeederPacket?(pPacket):BGRClib_P_SetDestinationFeederPacket( (grclib), (pPacket), NULL, DEFAULT_COLOR))
    
static uint32_t *BGRClib_P_SetDestinationFeederPacket( BGRClib_Handle grclib,
    void *pPacket, const BM2MC_PACKET_Plane *pSurface, uint32_t color )
{
    grclib->defaultDestinationFeederPacket = (!pSurface && color == DEFAULT_COLOR);
    BGRC_Packet_SetDestinationSurfacePacket2( grclib->hGRC, &pPacket, pSurface, color );
    return pPacket;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetOutputSurfacePacket2( BGRC_Handle hGrc, void **ppPacket, 
    const BM2MC_PACKET_Plane *pSurface )
{
    BM2MC_PACKET_PacketOutputFeeder *pPacket = (BM2MC_PACKET_PacketOutputFeeder *) (*ppPacket);
    BM2MC_PACKET_INIT( pPacket, OutputFeeder, false );
    pPacket->plane = *pSurface;
    BM2MC_PACKET_TERM( pPacket );
    *ppPacket = (void *) pPacket;
    BSTD_UNUSED(hGrc);
    return BERR_SUCCESS;
}

/* the only way to cache this without comparing everything is something like BSUR's SurfaceId to know if the surface has been recreated. */
static uint32_t *BGRClib_P_SetOutputFeederPacket( BGRClib_Handle grclib,
    void *pPacket, const BM2MC_PACKET_Plane *pSurface )
{
    BGRC_Packet_SetOutputSurfacePacket2( grclib->hGRC, &pPacket, pSurface );
    return pPacket;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_SetOutputOffsetPacket( 
    void *pPacket, BM2MC_PACKET_PixelFormat pixelFormat, uint32_t offset, uint32_t pitch )
{
    BM2MC_PACKET_PacketOutputFeeder *pFeeder = (BM2MC_PACKET_PacketOutputFeeder *) pPacket;
    BM2MC_PACKET_INIT( pFeeder, OutputFeeder, false );
    pFeeder->plane.address = offset;
    pFeeder->plane.pitch = (uint16_t) pitch;
    pFeeder->plane.format = pixelFormat;
    pFeeder->plane.width = 0;
    pFeeder->plane.height = 0;
    BM2MC_PACKET_TERM( pFeeder );
    pPacket = pFeeder;
    return pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultFillBlendPacket(grclib, pPacket) \
    ((grclib)->defaultFillBlendPacket?(pPacket):BGRClib_P_SetFillBlendPacket( (grclib), (pPacket), BGRCLib_FillOp_eCopy, BGRCLib_FillOp_eCopy, 0 ))
    
static uint32_t *BGRClib_P_SetFillBlendPacket( BGRClib_Handle grclib, void *pPacket,
    BGRCLib_FillOp colorOp, BGRCLib_FillOp alphaOp, uint32_t color )
{
    grclib->defaultBlitBlendPacket = false;
    grclib->defaultFillBlendPacket = (colorOp == BGRCLib_FillOp_eCopy && alphaOp == BGRCLib_FillOp_eCopy && !color);
    
    BGRC_Packet_SetBlendPacket( grclib->hGRC, &pPacket,
        &g_aGRClib_Packet_ColorEquation_FillOp[colorOp], &g_aGRClib_Packet_AlphaEquation_FillOp[alphaOp], color );
    return pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultBlitBlendPacket(grclib, pPacket) \
    ((grclib)->defaultBlitBlendPacket?(pPacket):BGRClib_P_SetBlitBlendPacket( (grclib), (pPacket), BGRCLib_BlitColorOp_eCopySource, BGRCLib_BlitAlphaOp_eCopySource, 0, 0, 0 ))
    
static uint32_t *BGRClib_P_SetBlitBlendPacket( BGRClib_Handle grclib,
    void *pPacket, BGRCLib_BlitColorOp colorOp, BGRCLib_BlitAlphaOp alphaOp,
    const BGRClib_BlendEquation *pColorBlend, const BGRClib_BlendEquation *pAlphaBlend, uint32_t color )
{
    BM2MC_PACKET_Blend colorEquation;
    BM2MC_PACKET_Blend alphaEquation;
    
    /* if both ops are eCopySource, the other params are ignored */
    grclib->defaultBlitBlendPacket = (colorOp == BGRCLib_BlitColorOp_eCopySource && alphaOp == BGRCLib_BlitAlphaOp_eCopySource);
    grclib->defaultFillBlendPacket = false;

    if( colorOp == BGRCLib_BlitColorOp_eUseBlendFactors )
        BGRClib_P_GetBlendEquation( &colorEquation, pColorBlend );
    if( alphaOp == BGRCLib_BlitAlphaOp_eUseBlendFactors )
        BGRClib_P_GetBlendEquation( &alphaEquation, pAlphaBlend );

    BGRC_Packet_SetBlendPacket( grclib->hGRC, &pPacket,
        (colorOp == BGRCLib_BlitColorOp_eUseBlendFactors) ? &colorEquation : &g_aGRClib_Packet_ColorEquation_BlitOp[colorOp],
        (alphaOp == BGRCLib_BlitAlphaOp_eUseBlendFactors) ? &alphaEquation : &g_aGRClib_Packet_AlphaEquation_BlitOp[alphaOp], color );
    return pPacket;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_SetPorterDuffFillBlendPacket( BGRClib_Handle grclib, void *pPacket,
    BGRCLib_PorterDuffOp pdOp, uint32_t color )
{
    /* no grclib defaults */
    grclib->defaultBlitBlendPacket = false;
    grclib->defaultFillBlendPacket = false;
    
    BGRC_Packet_SetBlendPacket( grclib->hGRC, &pPacket,
        &g_aGRClib_Packet_ColorEquation_PorterDuffFillOp[pdOp], &g_aGRClib_Packet_AlphaEquation_PorterDuffFillOp[pdOp], color );
    return pPacket;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_SetPorterDuffBlitBlendPacket( BGRClib_Handle grclib, void *pPacket,
    BGRCLib_PorterDuffOp pdOp, uint32_t color )
{
    /* no grclib defaults */
    grclib->defaultBlitBlendPacket = false;
    grclib->defaultFillBlendPacket = false;
    
    BGRC_Packet_SetBlendPacket( grclib->hGRC, &pPacket,
        &g_aGRClib_Packet_ColorEquation_PorterDuffBlitOp[pdOp], &g_aGRClib_Packet_AlphaEquation_PorterDuffBlitOp[pdOp], color );
    return pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultRopPacket(grclib, pPacket) \
    ((grclib)->defaultRopPacket?(pPacket):BGRClib_P_SetRopPacket( (grclib), (pPacket), 0xCC, NULL, 0, 0))
    
static uint32_t *BGRClib_P_SetRopPacket( BGRClib_Handle grclib, void *pPacket,
    uint8_t rop, uint32_t* pattern, uint32_t color0, uint32_t color1 )
{
    grclib->defaultRopPacket = (rop == 0xCC && pattern == NULL && color0==0 && color1==0);
    BGRC_Packet_SetRopPacket( grclib->hGRC, &pPacket, rop, pattern, color0, color1 );
    return pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultSourceColorkeyPacket(grclib, pPacket) \
    ((grclib)->defaultSourceColorkeyPacket?(pPacket):BGRClib_P_SetSourceColorkeyPacket( (grclib), (pPacket), false, 0, 0, 0, 0, 0))
    
static uint32_t *BGRClib_P_SetSourceColorkeyPacket( BGRClib_Handle grclib, void *pPacket,
    bool enable, uint32_t high, uint32_t low, uint32_t mask, uint32_t replacement, uint32_t replacement_mask )
{
    grclib->defaultSourceColorkeyPacket = (!enable && !high && !low && !mask && !replacement && !replacement_mask);
    BGRC_Packet_SetSourceColorkeyPacket( grclib->hGRC, &pPacket, enable,
        high, low, mask, replacement, replacement_mask );
    return pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultDestinationColorkeyPacket(grclib, pPacket) \
    ((grclib)->defaultDstColorkeyPacket?(pPacket):BGRClib_P_SetDestinationColorkeyPacket( (grclib), (pPacket), false, 0, 0, 0, 0, 0))
    
static uint32_t *BGRClib_P_SetDestinationColorkeyPacket( BGRClib_Handle grclib, void *pPacket,
    bool enable, uint32_t high, uint32_t low, uint32_t mask, uint32_t replacement, uint32_t replacement_mask )
{
    grclib->defaultDstColorkeyPacket = (!enable && !high && !low && !mask && !replacement && !replacement_mask);
    BGRC_Packet_SetDestinationColorkeyPacket( grclib->hGRC, &pPacket, enable,
        high, low, mask, replacement, replacement_mask );
    return pPacket;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_SetFilterPacket( BGRClib_Handle grclib, void *pPacket,
    BGRC_FilterCoeffs horz, BGRC_FilterCoeffs vert, BM2MC_PACKET_Rectangle *pSrcRect, BM2MC_PACKET_Rectangle *pOutRect )
{
    /* no default value, so we cache values. the src/out rects are used to determine changes in scaling factor */
    if (grclib->filterPacket.vertCoeffs != vert ||
        grclib->filterPacket.horzCoeffs != horz ||
        grclib->filterPacket.src.width != pSrcRect->width ||
        grclib->filterPacket.src.height != pSrcRect->height ||
        grclib->filterPacket.out.width != pOutRect->width ||
        grclib->filterPacket.out.height != pOutRect->height)
    {   
        BGRC_Packet_SetFilterPacket( grclib->hGRC, &pPacket, horz, vert, pSrcRect, pOutRect );
        
        grclib->filterPacket.vertCoeffs = vert;
        grclib->filterPacket.horzCoeffs = horz;
        grclib->filterPacket.src.width = pSrcRect->width;
        grclib->filterPacket.src.height = pSrcRect->height;
        grclib->filterPacket.out.width = pOutRect->width;
        grclib->filterPacket.out.height = pOutRect->height;
    }
    return pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultColorMatrixPacket(grclib, pPacket) \
    ((grclib)->defaultColorMatrixPacket?(pPacket):BGRClib_P_SetColorMatrixPacket( (grclib), (pPacket), NULL, 0))

static uint32_t *BGRClib_P_SetColorMatrixPacket( BGRClib_Handle grclib,
    void *pPacket, const int32_t matrix[], uint32_t shift )
{
    grclib->defaultColorMatrixPacket = (!matrix && !shift);
    BGRC_Packet_SetColorMatrixPacket( grclib->hGRC, &pPacket, matrix, shift );
    return pPacket;
}

#define BM2MC_IS_PALETTE_FORMAT(FORMAT) \
    ((FORMAT)>=BM2MC_PACKET_PixelFormat_eA8_P8 && (FORMAT)<=BM2MC_PACKET_PixelFormat_eY8_P8)
    
/*****************************************************************************/
static uint32_t *BGRClib_P_SetSourcePalette(void *pPacket_, unsigned paletteOffset)
{
    BM2MC_PACKET_PacketSourcePalette *pPacket = pPacket_;
    BM2MC_PACKET_INIT( pPacket, SourcePalette, false );
    pPacket->address = paletteOffset;
    BM2MC_PACKET_TERM( pPacket );
    return (uint32_t *)pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultAlphaPremultiplyPacket(grclib, pPacket) \
    ((grclib)->defaultAlphaPremultiplyPacket?(pPacket):BGRClib_P_SetAlphaPremultiplyPacket( (grclib), (pPacket), false))
static uint32_t *BGRClib_P_SetAlphaPremultiplyPacket( BGRClib_Handle grclib, void *pPacket, bool enable )
{
    grclib->defaultAlphaPremultiplyPacket = (!enable);
    BGRC_Packet_SetAlphaPremultiply( grclib->hGRC, &pPacket, enable );
    return pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultMirrorPacket(grclib, pPacket) \
    ((grclib)->defaultMirrorPacket?(pPacket):BGRClib_P_SetMirrorPacket( (grclib), (pPacket), false, false, false, false, false, false))
static uint32_t *BGRClib_P_SetMirrorPacket( BGRClib_Handle grclib, void *pPacket,
    uint16_t srcHorz, uint16_t srcVert, uint16_t dstHorz, uint16_t dstVert, uint16_t outHorz, uint16_t outVert )
{
    grclib->defaultMirrorPacket = (!srcHorz && !srcVert && !dstHorz && !dstVert && !outHorz && !outVert);
    BGRC_Packet_SetMirrorPacket( grclib->hGRC, &pPacket, 
        0!=srcHorz, 0!=srcVert, 0!=dstHorz, 0!=dstVert, 0!=outHorz, 0!=outVert );
    return pPacket;
}

/*****************************************************************************/
#define BGRClib_P_SetDefaultFixedScalePacket(grclib, pPacket) \
    ((grclib)->defaultFixedScalePacket?(pPacket):BGRClib_P_SetFixedScalePacket( (grclib), (pPacket), NULL))
static uint32_t *BGRClib_P_SetFixedScalePacket( BGRClib_Handle grclib, void *pPacket, const BGRClib_BlitScalingControlParams *params )
{
#if 0
    grclib->defaultFixedScalePacket = (!params);
    if( params )
    {
        BM2MC_PACKET_PacketFixedScale *pScale = (BM2MC_PACKET_PacketFixedScale *) pPacket;
        BM2MC_PACKET_INIT( pScale, FixedScale, false );
        pScale->shift = 20;
        pScale->hor_phase = params->iHorizontalPhase << (pScale->shift - params->ulPhaseFixedPointShift);
        pScale->ver_phase = params->iVerticalPhase << (pScale->shift - params->ulPhaseFixedPointShift);
        if (params->ulHorizontalDenominator) {
            pScale->hor_step = (params->ulHorizontalNumerator << pScale->shift) / params->ulHorizontalDenominator;
        }
        else {
            pScale->hor_step = 0;
        }
        if (params->ulVerticalDenominator) {
            pScale->ver_step = (params->ulVerticalNumerator << pScale->shift) / params->ulVerticalDenominator;
        }
        else {
            pScale->ver_step = 0;
        }
        BM2MC_PACKET_TERM( pScale );
        pPacket = pScale;
    }
    else
    {
        BM2MC_PACKET_PacketFixedScale *pScale = (BM2MC_PACKET_PacketFixedScale *) pPacket;
        BM2MC_PACKET_INIT( pScale, FixedScale, false );
        pScale->hor_phase = 0;
        pScale->ver_phase = 0;
        pScale->hor_step = 0;
        pScale->ver_step = 0;
        pScale->shift = 0;
        BM2MC_PACKET_TERM( pScale );
        pPacket = pScale;
    }
#else
	BSTD_UNUSED(grclib);
	BSTD_UNUSED(params);
#endif
    return pPacket;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_SetFillPacket( BGRClib_Handle grclib,
    void *pPacket, BM2MC_PACKET_Rectangle *pRect )
{
    BGRC_Packet_SetBlitPacket( grclib->hGRC, &pPacket, pRect, NULL, NULL );
    return pPacket;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_SetBlitPacket( BGRClib_Handle grclib, void *pPacket,
    BM2MC_PACKET_Rectangle *pSrcRect, BM2MC_PACKET_Rectangle *pDstRect, BM2MC_PACKET_Rectangle *pOutRect,
    BGRC_FilterCoeffs horz, BGRC_FilterCoeffs vert )
{
    if( (pSrcRect->width == pOutRect->width) && (pSrcRect->height == pOutRect->height) &&
        (horz <= BGRC_FilterCoeffs_eAnisotropic) && (vert <= BGRC_FilterCoeffs_eAnisotropic) )
    {
        BGRC_Packet_SetBlitPacket( grclib->hGRC, &pPacket,
            pSrcRect, (BM2MC_PACKET_Point *) pOutRect, (BM2MC_PACKET_Point *) pDstRect );
    }
    else
    {
        BGRC_Packet_SetScaleBlitPacket( grclib->hGRC, &pPacket,
            pSrcRect, pOutRect, (BM2MC_PACKET_Point *) pDstRect );
    }
    return pPacket;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_SetDestripeBlitPacket( BGRClib_Handle grclib, void *pPacket,
    BM2MC_PACKET_Rectangle *pSrcRect, BM2MC_PACKET_Rectangle *pOutRect,
    uint32_t stripeWidth, uint32_t lumaStripeHeight, uint32_t chromaStripeHeight )
{
    BGRC_Packet_SetDestripeBlitPacket( grclib->hGRC, &pPacket,
        pSrcRect, pOutRect, stripeWidth, lumaStripeHeight, chromaStripeHeight );
    return pPacket;
}

/*****************************************************************************/
static BERR_Code BGRClib_P_PacketsComplete( BGRClib_Handle grclib, uint32_t ulPacketSize )
{
    BERR_Code err = BGRC_Packet_SubmitPackets( grclib->hGRC, grclib->hContext, ulPacketSize );
    return (err == BGRC_PACKET_MSG_PACKETS_INCOMPLETE) ? BERR_SUCCESS : err;
}

/*****************************************************************************/
static uint32_t *BGRClib_P_GetPacketBuffer( BGRClib_Handle grclib, size_t size )
{
    void *pPacket;
    size_t iAllocSize = 0;

    BERR_Code err = BGRC_Packet_GetPacketMemory( grclib->hGRC, grclib->hContext, &pPacket, &iAllocSize, size );
    if( err != BERR_SUCCESS )
        return NULL;

    return pPacket;
}

/***************************************************************************/
BERR_Code BGRClib_Open( BGRClib_Handle *pgrclib, BGRC_Handle grcHandle, BGRC_PacketContext_Handle packetContext )
{
    BGRClib_Handle grclib;
    BERR_Code err = BERR_SUCCESS;

    BDBG_ASSERT(pgrclib);
    BDBG_ASSERT(grcHandle);

    grclib = (BGRClib_Handle) BKNI_Malloc( sizeof (BGRClib_P_Data) );
    if( !grclib )
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);

    BKNI_Memset( grclib, 0, sizeof(BGRClib_P_Data) );
    BDBG_OBJECT_SET(grclib, GRClib);

    grclib->hGRC = grcHandle;
    grclib->hContext = packetContext;
    *pgrclib = grclib;
    grclib->filterPacket.vertCoeffs = (BGRC_FilterCoeffs)-1; 
    grclib->filterPacket.horzCoeffs = (BGRC_FilterCoeffs)-1;

    return BERR_TRACE(err);
}

/***************************************************************************/
void BGRClib_Close( BGRClib_Handle grclib )
{
    BDBG_OBJECT_ASSERT(grclib, GRClib);
    BDBG_OBJECT_DESTROY(grclib, GRClib);
    BKNI_Free( (void*)grclib );
}

/*****************************************************************************/
BERR_Code BGRClib_Memset32( BGRClib_Handle grclib, uint32_t offset, uint32_t data, uint32_t count )
{
    BERR_Code err = BERR_SUCCESS;
    BM2MC_PACKET_Rectangle outRect = { 0, 0, 0, 0 };
    static uint32_t pitch = 4096;
    void *pPacket;
    void *pPacketStart;

    BDBG_OBJECT_ASSERT(grclib, GRClib);

    outRect.width = (uint16_t) (pitch / 4);
    outRect.height = (uint16_t) (count * 4 / pitch);

    pPacketStart = BGRClib_P_GetPacketBuffer( grclib, GRCLIB_FULL_PACKET_SIZE );
    if( pPacketStart == 0 )
    {
        return BERR_OUT_OF_DEVICE_MEMORY; /* this is normal for flow control */
    }

    pPacket = pPacketStart;
    pPacket = BGRClib_P_SetDefaultSourceFeederPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultDestinationFeederPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetOutputOffsetPacket( pPacket, BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8, offset, pitch );
    pPacket = BGRClib_P_SetFillBlendPacket( grclib, pPacket, BGRCLib_FillOp_eCopy, BGRCLib_FillOp_eCopy, data );
    pPacket = BGRClib_P_SetDefaultRopPacket(grclib, pPacket);

    pPacket = BGRClib_P_SetDefaultSourceColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultDestinationColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultColorMatrixPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultAlphaPremultiplyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultMirrorPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultFixedScalePacket(grclib, pPacket);
    pPacket = BGRClib_P_SetFillPacket( grclib, pPacket, &outRect );

    err = BGRClib_P_PacketsComplete( grclib, (uint32_t) pPacket - (uint32_t) pPacketStart );

    return BERR_TRACE(err);
}

/*****************************************************************************/
BERR_Code BGRClib_Destripe_Blit( BGRClib_Handle grclib, const BM2MC_PACKET_Plane *pOutSurface, const BRect *pOutRect,
    uint32_t lumaOffset, uint32_t chromaOffset, uint32_t pitch, uint32_t width, uint32_t height,
    uint32_t stripeWidth, uint32_t lumaStripeHeight, uint32_t chromaStripeHeight,
    const int32_t colorMatrix[], uint32_t matrixShift )
{
    BERR_Code err = BERR_SUCCESS;
    BM2MC_PACKET_Rectangle srcRect = { 0, 0, 0, 0 };
    BM2MC_PACKET_Rectangle outRect;
    void *pPacket;
    void *pPacketStart;

    BDBG_OBJECT_ASSERT(grclib, GRClib);

    srcRect.width = (uint16_t) width;
    srcRect.height = (uint16_t) height;
    BGRClib_P_GetPacketRectangle( pOutSurface, pOutRect, &outRect );

    pPacketStart = BGRClib_P_GetPacketBuffer( grclib, GRCLIB_FULL_PACKET_SIZE );
    if( pPacketStart == 0 )
    {
        return BERR_OUT_OF_DEVICE_MEMORY; /* this is normal for flow control */
    }

    pPacket = pPacketStart;
    pPacket = BGRClib_P_SetSourceDestripePacket( grclib, pPacket, lumaOffset, chromaOffset, pitch );
    pPacket = BGRClib_P_SetDefaultDestinationFeederPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetOutputFeederPacket( grclib, pPacket, pOutSurface );
    pPacket = BGRClib_P_SetDefaultBlitBlendPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultRopPacket(grclib, pPacket);

    pPacket = BGRClib_P_SetFilterPacket( grclib, pPacket,
        BGRC_FilterCoeffs_eAnisotropic, BGRC_FilterCoeffs_eAnisotropic, &srcRect, &outRect );

    if( colorMatrix )
        pPacket = BGRClib_P_SetColorMatrixPacket( grclib, pPacket, colorMatrix, matrixShift );
    else
        pPacket = BGRClib_P_SetDefaultColorMatrixPacket(grclib, pPacket);

    pPacket = BGRClib_P_SetDefaultSourceColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultDestinationColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultAlphaPremultiplyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultMirrorPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultFixedScalePacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDestripeBlitPacket( grclib, pPacket, &srcRect, &outRect, stripeWidth, lumaStripeHeight, chromaStripeHeight );

    err = BGRClib_P_PacketsComplete( grclib, (uint32_t) pPacket - (uint32_t) pPacketStart );

    return BERR_TRACE(err);
}

/*****************************************************************************/
BERR_Code BGRClib_Fill( BGRClib_Handle grclib, const BM2MC_PACKET_Plane *pSurface, uint32_t constantColor,
    const BRect *pRect )
{
    BERR_Code err;

    err = BGRClib_Blended_Fill( grclib, pSurface, constantColor, pRect, BGRCLib_FillOp_eCopy, BGRCLib_FillOp_eCopy );

    return BERR_TRACE(err);
}

/*****************************************************************************/
BERR_Code BGRClib_Blended_Fill( BGRClib_Handle grclib, const BM2MC_PACKET_Plane *pSurface, uint32_t constantColor,
    const BRect *pRect, BGRCLib_FillOp colorOp, BGRCLib_FillOp alphaOp )
{
    BERR_Code err = BERR_SUCCESS;
    BM2MC_PACKET_Rectangle outRect;
    void *pPacket;
    void *pPacketStart;

    BDBG_OBJECT_ASSERT(grclib, GRClib);

    BGRClib_P_GetPacketRectangle( pSurface, pRect, &outRect );

    pPacketStart = BGRClib_P_GetPacketBuffer( grclib, GRCLIB_FULL_PACKET_SIZE );
    if( pPacketStart == 0 )
    {
        return BERR_OUT_OF_DEVICE_MEMORY; /* this is normal for flow control */
    }

    pPacket = pPacketStart;
    if (colorOp == BGRCLib_FillOp_eCopy && alphaOp == BGRCLib_FillOp_eCopy) {
        pPacket = BGRClib_P_SetDefaultSourceFeederPacket(grclib, pPacket);
    }
    else {
        pPacket = BGRClib_P_SetSourceFeederPacket( grclib, pPacket, pSurface, DEFAULT_COLOR );
    }
    pPacket = BGRClib_P_SetDefaultDestinationFeederPacket( grclib, pPacket);
    pPacket = BGRClib_P_SetOutputFeederPacket( grclib, pPacket, pSurface );
    pPacket = BGRClib_P_SetFillBlendPacket( grclib, pPacket, colorOp, alphaOp, constantColor );
    pPacket = BGRClib_P_SetDefaultRopPacket(grclib, pPacket);

    pPacket = BGRClib_P_SetDefaultSourceColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultDestinationColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultColorMatrixPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultAlphaPremultiplyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultMirrorPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultFixedScalePacket(grclib, pPacket);
    pPacket = BGRClib_P_SetFillPacket( grclib, pPacket, &outRect );

    err = BGRClib_P_PacketsComplete( grclib, (uint32_t) pPacket - (uint32_t) pPacketStart );

    return BERR_TRACE(err);
}

/*****************************************************************************/
BERR_Code BGRClib_PorterDuffFill( BGRClib_Handle grclib, BGRCLib_PorterDuffOp pdOp,
    const BM2MC_PACKET_Plane *pSurface, uint32_t constantColor, 
    const BRect *pRect )
{
    BERR_Code err = BERR_SUCCESS;
    BM2MC_PACKET_Rectangle outRect;
    void *pPacket;
    void *pPacketStart;

    BDBG_OBJECT_ASSERT(grclib, GRClib);

    BGRClib_P_GetPacketRectangle( pSurface, pRect, &outRect );

    pPacketStart = BGRClib_P_GetPacketBuffer( grclib, GRCLIB_FULL_PACKET_SIZE );
    if( pPacketStart == 0 )
    {
        return BERR_OUT_OF_DEVICE_MEMORY; /* this is normal for flow control */
    }

    pPacket = pPacketStart;
    pPacket = BGRClib_P_SetSourceFeederPacket( grclib, pPacket, pSurface, DEFAULT_COLOR );
    pPacket = BGRClib_P_SetDefaultDestinationFeederPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetOutputFeederPacket( grclib, pPacket, pSurface );
    pPacket = BGRClib_P_SetPorterDuffFillBlendPacket( grclib, pPacket, pdOp, constantColor );
    pPacket = BGRClib_P_SetDefaultRopPacket(grclib, pPacket);

    pPacket = BGRClib_P_SetDefaultSourceColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultDestinationColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultColorMatrixPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultAlphaPremultiplyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultMirrorPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultFixedScalePacket(grclib, pPacket);
    pPacket = BGRClib_P_SetFillPacket( grclib, pPacket, &outRect );

    err = BGRClib_P_PacketsComplete( grclib, (uint32_t) pPacket - (uint32_t) pPacketStart );

    return BERR_TRACE(err);
}

/***************************************************************************/
BERR_Code BGRClib_PorterDuffBlit( BGRClib_Handle grclib, BGRCLib_PorterDuffOp pdOp,
    const BM2MC_PACKET_Plane *pSrcSurface, const BRect *pSrcRect, unsigned paletteOffset, const BM2MC_PACKET_Plane *pDstSurface, const BRect *pDstRect,
    const BM2MC_PACKET_Plane *pOutSurface, const BRect *pOutRect )
{
    BERR_Code err = BERR_SUCCESS;
    BM2MC_PACKET_Rectangle srcRect;
    BM2MC_PACKET_Rectangle dstRect;
    BM2MC_PACKET_Rectangle outRect;
    void *pPacket;
    void *pPacketStart;

    BDBG_OBJECT_ASSERT(grclib, GRClib);

    BGRClib_P_GetPacketRectangle( pSrcSurface, pSrcRect, &srcRect );
    BGRClib_P_GetPacketRectangle( pOutSurface, pDstRect, &dstRect ); /* default to output surface dimensions */
    BGRClib_P_GetPacketRectangle( pOutSurface, pOutRect, &outRect );
    if (!dstRect.width || !dstRect.height) {
        dstRect.width = outRect.width;
        dstRect.height = outRect.height;
    }

    pPacketStart = BGRClib_P_GetPacketBuffer( grclib, GRCLIB_FULL_PACKET_SIZE );
    if( pPacketStart == 0 )
    {
        return BERR_OUT_OF_DEVICE_MEMORY; /* this is normal for flow control */
    }

    pPacket = pPacketStart;
    pPacket = BGRClib_P_SetSourceFeederPacket( grclib, pPacket, pSrcSurface, DEFAULT_COLOR );
    if (pDstSurface) {
        pPacket = BGRClib_P_SetDestinationFeederPacket( grclib, pPacket, pDstSurface, DEFAULT_COLOR );
    }
    else {
        pPacket = BGRClib_P_SetDefaultDestinationFeederPacket(grclib, pPacket);
    }
    pPacket = BGRClib_P_SetOutputFeederPacket( grclib, pPacket, pOutSurface );
    pPacket = BGRClib_P_SetPorterDuffBlitBlendPacket( grclib, pPacket, pdOp, 0);
    pPacket = BGRClib_P_SetDefaultRopPacket(grclib, pPacket);

    pPacket = BGRClib_P_SetDefaultSourceColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultDestinationColorkeyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultColorMatrixPacket(grclib, pPacket);
    if (paletteOffset) {
        pPacket = BGRClib_P_SetSourcePalette(pPacket, paletteOffset );
    }
    pPacket = BGRClib_P_SetDefaultAlphaPremultiplyPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultMirrorPacket(grclib, pPacket);
    pPacket = BGRClib_P_SetDefaultFixedScalePacket(grclib, pPacket);
    pPacket = BGRClib_P_SetBlitPacket( grclib, pPacket, &srcRect, pDstSurface ? &dstRect : NULL, &outRect, 
        BGRC_FilterCoeffs_ePointSample, BGRC_FilterCoeffs_ePointSample );

    err = BGRClib_P_PacketsComplete( grclib, (uint32_t) pPacket - (uint32_t) pPacketStart );

    return BERR_TRACE(err);
}

/*****************************************************************************/
void BGRClib_GetDefaultBlitParams( BGRClib_BlitParams *params )
{
    BKNI_Memset( params, 0, sizeof(BGRClib_BlitParams) );
    /* already zero: params->colorOp = BGRCLib_BlitColorOp_eCopySource; */
    /* already zero: params->alphaOp = BGRCLib_BlitAlphaOp_eCopySource; */
    params->horzFilter = BGRC_FilterCoeffs_eAnisotropic;
    params->vertFilter = BGRC_FilterCoeffs_eAnisotropic;
    params->colorKeySelect = BGRC_Output_ColorKeySelection_eTakeBlend;
    params->constantColor = DEFAULT_COLOR;
}

/*****************************************************************************/
void BGRClib_GetDefaultPaletteBlitParams( BGRClib_BlitParams *params )
{
    BKNI_Memset( params, 0, sizeof(BGRClib_BlitParams) );
    /* already zero: params->horzFilter = BGRC_FilterCoeffs_ePointSample; */
    /* already zero: params->vertFilter = BGRC_FilterCoeffs_ePointSample; */
    params->colorKeySelect = BGRC_Output_ColorKeySelection_eTakeSource;
    params->constantColor = DEFAULT_COLOR;
}

/*****************************************************************************/
void BGRClib_GetDefaultColorKeyParams( BGRClib_BlitColorKeyParams *colorkeyparams )
{
    BKNI_Memset( colorkeyparams, 0, sizeof(BGRClib_BlitColorKeyParams) );
    
    /* TODO: these are unused. need to add packet if they become needed */
    colorkeyparams->cksOnlySrcColorKeyed = BGRC_Output_ColorKeySelection_eTakeDestination;
    colorkeyparams->cksOnlyDstColorKeyed = BGRC_Output_ColorKeySelection_eTakeSource;
    colorkeyparams->cksBothSrcDstColorKeyed = BGRC_Output_ColorKeySelection_eTakeDestination;
    colorkeyparams->cksNeitherSrcDstColorKeyed = BGRC_Output_ColorKeySelection_eTakeSource;
}


/*****************************************************************************/
void BGRClib_GetDefaultScalingControlParams( BGRClib_BlitScalingControlParams *scalingparams )
{
    BKNI_Memset( scalingparams, 0, sizeof(BGRClib_BlitScalingControlParams) );
    scalingparams->ulHorizontalNumerator = 1;
    scalingparams->ulHorizontalDenominator = 1;
    scalingparams->ulVerticalNumerator = 1;
    scalingparams->ulVerticalDenominator = 1;
}

/***************************************************************************/
BERR_Code BGRClib_Blit( BGRClib_Handle grclib, const BGRClib_BlitParams *params,
    const BGRClib_BlitColorKeyParams *colorkeyParams, const BGRClib_BlitMatrixParams *matrixParams,
    const BGRClib_BlitPatternParams *patternParams, const BGRClib_BlitScalingControlParams *scalingParams )
{
    BERR_Code err = BERR_SUCCESS;

    void *pPacket;
    void *pPacketStart;

    BM2MC_PACKET_Rectangle srcRect;
    BM2MC_PACKET_Rectangle dstRect;
    BM2MC_PACKET_Rectangle outRect;

    BDBG_OBJECT_ASSERT(grclib, GRClib);
    
    if (params->colorOp >= BGRCLib_BlitColorOp_Count || params->alphaOp >= BGRCLib_BlitAlphaOp_Count) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if (!params->outSurface) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BGRClib_P_GetPacketRectangle( params->srcSurface, params->srcRect, &srcRect );
    BGRClib_P_GetPacketRectangle( params->outSurface, params->dstRect, &dstRect ); /* default to output surface dimensions */
    BGRClib_P_GetPacketRectangle( params->outSurface, params->outRect, &outRect );
    if (!dstRect.width || !dstRect.height) {
        dstRect.width = outRect.width;
        dstRect.height = outRect.height;
    }

    pPacketStart = BGRClib_P_GetPacketBuffer( grclib, GRCLIB_FULL_PACKET_SIZE );
    if( pPacketStart == 0 )
    {
        return BERR_OUT_OF_DEVICE_MEMORY; /* this is normal for flow control */
    }

    pPacket = pPacketStart;
    pPacket = BGRClib_P_SetSourceFeederPacket( grclib, pPacket, params->srcSurface, params->sourceConstantColor );
    if (params->dstSurface || params->destConstantColor != DEFAULT_COLOR) {
        pPacket = BGRClib_P_SetDestinationFeederPacket( grclib, pPacket, params->dstSurface, params->destConstantColor );
    }
    else {
        pPacket = BGRClib_P_SetDefaultDestinationFeederPacket(grclib, pPacket);
    }
    pPacket = BGRClib_P_SetOutputFeederPacket( grclib, pPacket, params->outSurface );
    
    if (params->colorOp != BGRCLib_BlitColorOp_eCopySource || params->alphaOp != BGRCLib_BlitAlphaOp_eCopySource) {
        pPacket = BGRClib_P_SetBlitBlendPacket( grclib, pPacket, params->colorOp, params->alphaOp,
            &params->colorBlend, &params->alphaBlend, params->constantColor );
    }
    else {
        pPacket = BGRClib_P_SetDefaultBlitBlendPacket(grclib, pPacket);
    }    

    if( patternParams )
    {
        pPacket = BGRClib_P_SetRopPacket( grclib, pPacket, patternParams->ropVector,
            (uint32_t *) patternParams->pattern, patternParams->backColor, patternParams->foreColor );
    }
    else
    {
        pPacket = BGRClib_P_SetDefaultRopPacket(grclib, pPacket);
    }

    if( colorkeyParams )
    {
        pPacket = BGRClib_P_SetSourceColorkeyPacket( grclib, pPacket, colorkeyParams->enableColorKey,
            colorkeyParams->colorKeyUpper, colorkeyParams->colorKeyLower, colorkeyParams->colorKeyMask,
            colorkeyParams->colorKeyReplace, colorkeyParams->colorKeyRplMask );
        pPacket = BGRClib_P_SetDestinationColorkeyPacket( grclib, pPacket, colorkeyParams->enableDstColorKey,
            colorkeyParams->dstColorKeyUpper, colorkeyParams->dstColorKeyLower, colorkeyParams->dstColorKeyMask,
            colorkeyParams->dstColorKeyReplace, colorkeyParams->dstColorKeyRplMask );
    }
    else
    {
        pPacket = BGRClib_P_SetDefaultSourceColorkeyPacket(grclib, pPacket);
        pPacket = BGRClib_P_SetDefaultDestinationColorkeyPacket(grclib, pPacket);
    }

    pPacket = BGRClib_P_SetFilterPacket( grclib, pPacket, params->horzFilter, params->vertFilter, &srcRect, &outRect );

    if( matrixParams )
        pPacket = BGRClib_P_SetColorMatrixPacket( grclib, pPacket, matrixParams->conversionMatrix, matrixParams->matrixShift );
    else
        pPacket = BGRClib_P_SetDefaultColorMatrixPacket(grclib, pPacket);

    if (params->srcPaletteOffset) {    
        pPacket = BGRClib_P_SetSourcePalette(pPacket, params->srcPaletteOffset );
    }

    if (params->srcAlphaPremult) {
        pPacket = BGRClib_P_SetAlphaPremultiplyPacket( grclib, pPacket, params->srcAlphaPremult );
    }
    else {
        pPacket = BGRClib_P_SetDefaultAlphaPremultiplyPacket(grclib, pPacket);
    }

    if (params->mirrorSrcHorizontally || params->mirrorSrcVertically ||
        params->mirrorDstHorizontally || params->mirrorDstVertically ||
        params->mirrorOutHorizontally || params->mirrorOutVertically ) {
        pPacket = BGRClib_P_SetMirrorPacket( grclib, pPacket,
            params->mirrorSrcHorizontally, params->mirrorSrcVertically,
            params->mirrorDstHorizontally, params->mirrorDstVertically,
            params->mirrorOutHorizontally, params->mirrorOutVertically );
    }
    else {
        pPacket = BGRClib_P_SetDefaultMirrorPacket(grclib, pPacket);
    }    

    /* nexus will pass scalingParams == NULL if not used */
    if (scalingParams) {
        pPacket = BGRClib_P_SetFixedScalePacket( grclib, pPacket, scalingParams );
    }
    else {
        pPacket = BGRClib_P_SetDefaultFixedScalePacket(grclib, pPacket);
    }

    pPacket = BGRClib_P_SetBlitPacket( grclib, pPacket, &srcRect, params->dstSurface ? &dstRect : NULL, &outRect, params->horzFilter, params->vertFilter );

    err = BGRClib_P_PacketsComplete( grclib, (uint32_t) pPacket - (uint32_t) pPacketStart );

    return BERR_TRACE(err);
}

/* End of File */
