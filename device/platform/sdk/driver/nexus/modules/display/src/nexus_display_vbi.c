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
 * $brcm_Workfile: nexus_display_vbi.c $
 * $brcm_Revision: 58 $
 * $brcm_Date: 7/23/12 10:20a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_display_vbi.c $
 * 
 * 58   7/23/12 10:20a erickson
 * SW7425-3545: move BVBI_TT_Line array off of stack, defer alloc until
 *  first use
 * 
 * 57   4/24/12 3:05p erickson
 * SW7405-5643: add param to BVBI_Encode_Create
 * 
 * 56   3/29/12 3:40p erickson
 * SW7435-77: add NEXUS_DisplayModuleSettings.vbi.allowVps
 * 
 * 55   3/27/12 11:59a erickson
 * SW7435-77: add NEXUS_DisplayModuleSettings.vbi.allowTeletext
 * 
 * 54   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 53   2/20/12 11:59a erickson
 * SW7425-2434: add NEXUS_VideoFormat_ePal60hz to VBI encoding "not
 *  supported" list in nexus
 * 
 * 52   1/23/12 1:38p erickson
 * SW7335-1391: disable CGMS A and B
 * 
 * 51   1/13/12 4:52p erickson
 * SW7125-1200: change to BVDC_Display_SetDcsMode, add
 *  NEXUS_Display_DcsLock provisionally
 * 
 * 50   10/6/11 3:42p erickson
 * SW7420-2070: add NEXUS_VBI_ENCODER_QUEUE_SIZE macro to control vbilib
 *  allocations using (NEXUS_VBI_ENCODER_QUEUE_SIZE+1)*NEXUS_NUM_DISPLAYS
 *  formula.
 * 
 * 49   2/25/11 4:37p erickson
 * SW7422-255: add AMOL support
 *
 * 48   1/3/11 11:29a erickson
 * SW7420-1336: use BVDC_Display_GetInterruptName for VBI encode
 *
 * 47   12/23/10 12:50p erickson
 * SW7425-34: use BAVC_VbiPath_eUnknown to determine if display is VBI
 *  capable
 *
 * 46   12/1/10 10:31a erickson
 * SW7405-5028: fix BVBI_Field_SetPolarity_isr for progressive displays
 *
 * 45   11/30/10 2:07p VISHK
 * SW7208-114: "Unable to set HD path display settings" if building with
 *  MACROVISION ON
 *
 * 44   10/18/10 2:12p VISHK
 * SW7401-4457: Settop API shim causes crash when macrovision is enabled
 *  for component via Brutus
 *
 * 43   10/18/10 11:21a katrep
 * SW7405-4950:add check to see if cgms-b in enabled in platform setting
 *
 * 42   10/5/10 4:47p erickson
 * SW7405-4916: CGMS A/B not supported for PAL
 *
 * 41   5/20/10 5:03p mphillip
 * SW7335-666: Enable CGMSA and CGMSB to work together
 *
 * 40   5/11/10 5:03p jtna
 * SW7125-307: Coverity - check for null pointer arguments before
 *  dereferencing them
 *
 * 39   5/4/10 10:12a erickson
 * SW7335-666: fix bottom field polarity in NEXUS_Display_P_DequeVbi_isr
 *
 * 38   4/6/10 10:17a erickson
 * SW7405-4171: revise NEXUS_DisplayDcsType. add eOn1, eOn2, eOn2. fix
 *  naming convention issues.
 *
 * 37   3/5/10 4:03p erickson
 * SW3556-1043: check in CASSERT that works with dtv and settop
 *
 * 36   3/5/10 12:35p erickson
 * SW3556-1043: clarify logic in NEXUS_Display_WriteTeletext, avoid VBI
 *  line 16 collisions with VPS, clarify mapping of SW arrays to VBI
 *  lines, note possible future options for lineNumber, avoid BKNI_Memcpy
 *  for BVBI_TT_INVALID_FRAMING_CODE lines
 *
 * 35   1/8/10 3:18p erickson
 * SW3556-979: rework VPS, CGMS, CGMS-B and WSS encoding. instead of
 *  trying to get into the VBIlib encoder queue, we just intercept the
 *  queue in the VEC isr. this removes the BKNI_Sleep and possible Set
 *  failure.
 *
 * 34   11/23/09 11:56a erickson
 * SW7550-29: replace BCHP_CHIP list with a test for an RDB define
 *
 * 33   11/19/09 4:21p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 32   11/19/09 3:27p erickson
 * SW7468-24: BVBI_Encode_SetGemstarOptions change
 *
 * 31   8/26/09 6:59p mphillip
 * SW7405-2810: Clear framing code when resetting to a new set of input
 *  teletext lines
 *
 * 30   8/19/09 11:14a mward
 * PR55545: Add 7125 support.
 *
 * 29   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 28   8/5/09 4:55p jrubio
 * PR55232: add 7342/7340 support
 *
 * 27   7/8/09 10:28a mward
 * PR 56640: Compiler warning for unused variable 'video'.
 *
 * 26   5/15/09 1:06p erickson
 * PR55187: set either top or bottom in NEXUS_Display_WriteClosedCaption,
 *  not either
 *
 * 25   3/10/09 10:58a erickson
 * PR51253: added NEXUS_TeletextLine.topField
 *
 * 24   3/9/09 10:11a erickson
 * PR51253: fix line_offset bug with NEXUS_Display_WriteTeletext
 *
 * 23   2/27/09 10:11a erickson
 * PR52115: convert BERR_TRACE to more clear BDBG_WRN. this is normal for
 *  bad video inputs.
 *
 * 22   1/15/09 9:51a katrep
 * PR47111: Fixed compiler warning
 *
 * 21   12/24/08 12:40p jgarrett
 * PR 50703: Disabling VBI for 7420
 *
 * 20   12/9/08 6:27p nickh
 * PR48963: Add support for 7420 VBI INT_ID name changes
 *
 * 19   12/8/08 5:04p erickson
 * PR50058: rework internal VideoInput vbi accounting, don't store refcnt
 *
 * 18   11/17/08 12:28p erickson
 * PR49203: ensure there are no unnecessary unguarded BVDC_ApplyChanges
 *  calls in Nexus
 *
 * 17   10/24/08 4:06p erickson
 * PR48340: don't call VBI PI with NULL handle on DTV platform
 *
 * 16   10/16/08 1:56p erickson
 * PR47892: add VBI PI calls for gemstar
 *
 * 15   10/16/08 12:39p jtna
 * PR47892: Added GemStar encoding interface
 *
 * 14   9/18/08 4:19p erickson
 * PR47111: fix warnings
 *
 * 13   7/17/08 6:36p jgarrett
 * PR 44919: Only enabling SEC_VEC interrupt for platforms that have one
 *
 * 12   7/17/08 4:17p erickson
 * PR44919: one display still requires SEC VBI
 *
 * 11   7/15/08 5:21p erickson
 * PR38679: don't fail display SetSettings if the display format doesn't
 *  support VBI
 *
 * 10   6/3/08 9:39a ahulse
 * PR43266: Add support for DCS in nexus
 *
 * 9   4/30/08 11:14a erickson
 * PR41371: add CGMS-B support
 *
 * 8   3/26/08 1:22p vsilyaev
 * PR 40862: Fixed support for 1080p formats
 *
 * 7   2/7/08 2:31p erickson
 * PR38701: added macrovision implementation with appropriate #deifne
 *
 * 6   2/7/08 12:10p erickson
 * PR38679: adds comments and DBG
 *
 * 5   2/5/08 10:12a erickson
 * PR38679: impl WriteTeletext, update SetWss, change to SetVps
 *
 * 4   2/4/08 10:40a erickson
 * PR38679: added NEXUS_Display_WriteClosedCaption
 *
 * 3   2/4/08 9:35a erickson
 * PR38679: impl SetWss and SetCgms
 *
 * 2   2/1/08 3:20p erickson
 * PR38679: add vbi support
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/12   1/15/08 10:51a vsilyaev
 * PR 38701: Added macrovision API
 *
 * Nexus_Devel/11   1/11/08 1:51p erickson
 * PR38679: add vbi read/write api's
 *
 * Nexus_Devel/10   12/28/07 5:19p erickson
 * PR38470: move conversion functions to core
 *
 * Nexus_Devel/9   11/6/07 1:13p erickson
 * PR36570: merge 3563
 *
 * Nexus_Devel/8   11/5/07 3:46p vsilyaev
 * PR 36696: Fixed user data handling
 *
 * Nexus_Devel/7   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 *
 * Nexus_Devel/6   10/17/07 3:20p vsilyaev
 * PR 34662: Used number of display from the platform layer
 *
 * Nexus_Devel/5   10/8/07 12:11p vsilyaev
 * PR 34662: PR34926: reworked VBI interface
 *
 * Nexus_Devel/4   10/4/07 6:44p vsilyaev
 * PR 34662: Use bitfield for the field polarity, disable VBI on the
 * format change
 *
 * Nexus_Devel/3   10/4/07 4:01p vsilyaev
 * PR 34662: Activated closed captioning
 *
 * Nexus_Devel/2   10/4/07 3:05p vsilyaev
 * PR 34662: Fixed typo
 *
 * Nexus_Devel/1   10/4/07 12:21p vsilyaev
 * PR 34662: VBI support
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_display_vbi);

#if B_HAS_VBI
#if B_HAS_VBI_ENCODE

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

static void
NEXUS_Display_P_DequeVbi_isr(void* parm1, int parm2)
{
    NEXUS_DisplayHandle display = parm1;
    BERR_Code rc;
    BAVC_Polarity polarity = (BAVC_Polarity)parm2;
    BVBI_Field_Handle field;
    bool isTop, isBottom;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_ASSERT(display->vbi.enc);

    /* PR 21710 - for progressive displays, pass vbilib eFrame */
    polarity = display->vbi.progressive ? BAVC_Polarity_eFrame  : (BAVC_Polarity)parm2;
    isTop = (polarity == BAVC_Polarity_eTopField) || (polarity == BAVC_Polarity_eFrame);
    isBottom = (polarity == BAVC_Polarity_eBotField) || (polarity == BAVC_Polarity_eFrame);

    rc = BVBIlib_Encode_GetOldestDatum_isr(display->vbi.enc, &field);
    if (!rc) {
        bool somethingSet =
            display->vbi.pending.wssSet ||
            display->vbi.pending.vpsSet ||
            display->vbi.pending.cgmsTopSet ||
            display->vbi.pending.cgmsBottomSet ||
            display->vbi.pending.cgmsBTopSet ||
            display->vbi.pending.cgmsBBottomSet;
        bool obtained = false;

        if (!field && somethingSet) {
            rc = BVBIlib_List_Obtain_isr(g_NEXUS_DisplayModule_State.vbilist, &field);
            if (rc) {field = NULL; rc = BERR_TRACE(rc);}
            /* if we can't get one, just let the data stay pending.
            we still need to fall through and allow VBIlib to empty its queue. */

            if (field) {
                unsigned polarityMask;

                obtained = true;

                if (polarity == BAVC_Polarity_eFrame) {
                    polarityMask = (1<<BAVC_Polarity_eTopField)|(1<<BAVC_Polarity_eBotField);
                }
                else {
                    polarityMask = 1<<polarity;
                }
                (void)BVBI_Field_SetPolarity_isr(field, polarityMask);
            }

        }
        if (field) {
            /* set non-streaming data. these types of VBI just require the last value set to be
            applied. this code bypassing the queue logic needed for cc, tt and gs. */
            if (display->vbi.pending.wssSet && isTop) {
                rc = BVBI_Field_SetWSSData_isr(field, display->vbi.pending.wssData);
                if (!rc) {
                    BDBG_MSG_TRACE(("wss %x set", display->vbi.pending.wssData));
                    display->vbi.pending.wssSet = false;
                }
            }
            if (display->vbi.pending.vpsSet && isTop) {
                rc = BVBI_Field_SetVPSData_isr(field, &display->vbi.pending.vpsData);
                if (!rc) {
                    BDBG_MSG_TRACE(("vps set"));
                    display->vbi.pending.vpsSet = false;
                }
            }
            if (display->vbi.pending.cgmsTopSet && isTop) {
                rc = BVBI_Field_SetCGMSData_isr(field, display->vbi.pending.cgmsData);
                if (!rc) {
                    BDBG_MSG_TRACE(("cgms top %x set", display->vbi.pending.cgmsData));
                    display->vbi.pending.cgmsTopSet = false;
                }
            }
            if (display->vbi.pending.cgmsBottomSet && isBottom) {
                rc = BVBI_Field_SetCGMSData_isr(field, display->vbi.pending.cgmsData);
                if (!rc) {
                    BDBG_MSG_TRACE(("cgms bottom %x set", display->vbi.pending.cgmsData));
                    display->vbi.pending.cgmsBottomSet = false;
                }
            }
            if (display->vbi.pending.cgmsBTopSet && isTop) {
                rc = BVBI_Field_SetCGMSBData_isr(field, (BVBI_CGMSB_Datum*)display->vbi.pending.cgmsBData);
                if (!rc) {
                    BDBG_MSG_TRACE(("cgmsB top set"));
                    display->vbi.pending.cgmsBTopSet = false;
                }
            }
            if (display->vbi.pending.cgmsBBottomSet && isBottom) {
                rc = BVBI_Field_SetCGMSBData_isr(field, (BVBI_CGMSB_Datum*)display->vbi.pending.cgmsBData);
                if (!rc) {
                    BDBG_MSG_TRACE(("cgmsB bottom set"));
                    display->vbi.pending.cgmsBBottomSet = false;
                }
            }
        }
        if (obtained) {
            rc = BVBIlib_Encode_Enqueue_isr(display->vbi.enc, field);
            if (rc) {
                BVBIlib_List_Return_isr(g_NEXUS_DisplayModule_State.vbilist, field);
            }
        }
    }

    rc = BVBIlib_Encode_Data_isr(display->vbi.enc, polarity);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
}


BERR_Code
NEXUS_Display_P_ConnectVbi(NEXUS_DisplayHandle display)
{
    BAVC_VbiPath vbi_path;
    BERR_Code rc;
    BERR_Code cleanup_rc; /* preserve original error code */
    BINT_Id tf_isr, bf_isr;
    const NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_ARIB_VIDEO)
	BVBI_Encode_Settings  encode_settings;
#endif

    BDBG_MSG(("connect display%d to vbi", display->index));

    rc = BVDC_Display_GetVbiPath(display->displayVdc, &vbi_path);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbi_path;}

    if (vbi_path == BAVC_VbiPath_eUnknown) {
        /* if this display doesn't support VBI, it is not a failure. return success. */
        /* make sure this is null so that NEXUS_Display_P_DisconnectVbi will short circuit as well */
        BDBG_ASSERT(!display->vbi.enc_core);
        return 0;
    }

    rc = BVDC_Display_GetInterruptName(display->displayVdc, BAVC_Polarity_eTopField, &tf_isr);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbi_path;}
    rc = BVDC_Display_GetInterruptName(display->displayVdc, BAVC_Polarity_eBotField, &bf_isr);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbi_path;}

    display->vbi.progressive = false;

    /* unfortunately we have to recreate everything because the vbi_path might change. */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_ARIB_VIDEO)
	encode_settings.bArib480p = true;
	rc = BVBI_Encode_Create(video->vbi, vbi_path, &encode_settings, &display->vbi.enc_core);
#else	
    rc = BVBI_Encode_Create(video->vbi, vbi_path, NULL, &display->vbi.enc_core);
#endif
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbi_encode;}

    rc = BVBIlib_Encode_Create(video->vbilib, video->vbilist, display->vbi.enc_core,
        NEXUS_VBI_ENCODER_QUEUE_SIZE,
        &display->vbi.enc);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbilib_encode;}

    /* we are using top ISR to feed bottom field data and bottom field isr to feed top field data */
    rc = BINT_CreateCallback( &display->vbi.tf_isr, g_pCoreHandles->bint, tf_isr, NEXUS_Display_P_DequeVbi_isr, display, BAVC_Polarity_eBotField);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_tf_isr;}

    rc = BINT_CreateCallback( &display->vbi.bf_isr, g_pCoreHandles->bint, bf_isr, NEXUS_Display_P_DequeVbi_isr, display, BAVC_Polarity_eTopField);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_bf_isr;}

    rc = BINT_EnableCallback(display->vbi.tf_isr);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_isr_cfg;}

    rc = BINT_EnableCallback(display->vbi.bf_isr);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_isr_cfg;}

    rc = NEXUS_Display_P_EnableVbi(display, display->cfg.format);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_enable_vbi;}

    BDBG_ASSERT(display->vbi.tf_isr && display->vbi.bf_isr);
    return BERR_SUCCESS;

err_enable_vbi:
err_isr_cfg:
    cleanup_rc = BINT_DestroyCallback(display->vbi.bf_isr);
    if (cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc); }
err_bf_isr:
    cleanup_rc = BINT_DestroyCallback(display->vbi.tf_isr);
    if (cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc); }
err_tf_isr:
    cleanup_rc = BVBIlib_Encode_Destroy(display->vbi.enc);
    if (cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc); }
    display->vbi.enc = NULL;
err_vbilib_encode:
    cleanup_rc = BVBI_Encode_Destroy(display->vbi.enc_core);
    if (cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc); }
    display->vbi.enc_core = NULL;
err_vbi_encode:
err_vbi_path:
    return rc;
}

void
NEXUS_Display_P_DisconnectVbi(NEXUS_DisplayHandle display)
{
    BERR_Code rc;

    BDBG_MSG(("disconnect display%d from vbi", display->index));

    if (display->vbi.enc_core) {
        NEXUS_Display_P_DisableVbi(display);

        rc = BINT_DestroyCallback(display->vbi.bf_isr);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
        rc = BINT_DestroyCallback(display->vbi.tf_isr);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
        rc = BVBIlib_Encode_Destroy(display->vbi.enc);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
        display->vbi.enc = NULL;
        rc = BVBI_Encode_Destroy(display->vbi.enc_core);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
        display->vbi.enc_core = NULL;
    }

    return;
}

BERR_Code
NEXUS_Display_P_VbiData_isr(NEXUS_DisplayHandle display, BVBI_Field_Handle vbiData)
{
    BERR_Code rc;

    if (!display->vbi.enc_core) {
        /* we did not consume BVBI_Field_Handle, so we can't return 0. */
        return -1;
    }
    /* NOTE: display->vbi.enc_core != NULL is the test for VBI capability. display->vbi.enc should track with it. */
    BDBG_ASSERT(display->vbi.enc);

    rc = BVBIlib_Encode_Enqueue_isr(display->vbi.enc, vbiData);
    if (rc) {
        BDBG_WRN(("Flushing VBI encoder queue because of overflow."));
        /* If we can't enqueue, the buffer is probably full. Try to recover by flushing. */
        BVBIlib_Encode_Flush_isr(display->vbi.enc);
    }
    return rc;
}

void
NEXUS_Display_P_DisableVbi(NEXUS_DisplayHandle display)
{
    BERR_Code rc;

    if (!display->vbi.enc_core) {
        return;
    }

    rc = BVBI_Encode_SetCC(display->vbi.enc_core, false);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    rc = BVBI_Encode_SetTeletext(display->vbi.enc_core, false);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    rc = BVBI_Encode_SetWSS(display->vbi.enc_core, false);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    rc = BVBI_Encode_SetCGMSA(display->vbi.enc_core, false);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    rc = BVBI_Encode_SetCGMSB(display->vbi.enc_core, false);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    rc = BVBI_Encode_SetVPS(display->vbi.enc_core, false);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    rc = BVBI_Encode_SetGemstar(display->vbi.enc_core, false);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    rc = BVBI_Encode_SetAMOL(display->vbi.enc_core, false);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    rc = BVBI_Encode_ApplyChanges(display->vbi.enc_core);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    BKNI_EnterCriticalSection();
    BVBIlib_Encode_Flush_isr(display->vbi.enc);
    BKNI_LeaveCriticalSection();

    return ;
}

BERR_Code
NEXUS_Display_P_EnableVbi(NEXUS_DisplayHandle display, NEXUS_VideoFormat format)
{
    BERR_Code rc;
    bool isSd = NEXUS_P_VideoFormat_IsSd(format);
    BFMT_VideoFmt formatVdc;

    if (!display->vbi.enc_core) {
        BDBG_MSG(("Display does not support VBI encoding."));
        return 0;
    }

    /* for formats that don't support VBI, print a WRN but succeed. this will allow the other display settings to be applied */
    switch (format) {
    case NEXUS_VideoFormat_e1080p24hz:
    case NEXUS_VideoFormat_e1080p25hz:
    case NEXUS_VideoFormat_e1080p30hz:
    case NEXUS_VideoFormat_e1080p50hz:
    case NEXUS_VideoFormat_e1080p60hz:
    case NEXUS_VideoFormat_ePal60hz:
        BDBG_MSG(("Display VideoFormat %d does not support VBI encoding.", format));
        return 0;
    default:
        break;
    }

    /* No VESA modes */
    if (format >= NEXUS_VideoFormat_eVesa640x480p60hz)  {
        BDBG_MSG(("VESA display formats (%d) do not support VBI encoding.", format));
        return 0;
    }

    display->vbi.progressive = !NEXUS_P_VideoFormat_IsInterlaced(format);

    rc = NEXUS_P_VideoFormat_ToMagnum(format, &formatVdc);
    if (rc) {return BERR_TRACE(rc);}

    rc = BVBI_Encode_SetVideoFormat(display->vbi.enc_core,  formatVdc);
    if (rc) {
        BDBG_WRN(("NEXUS_Display_P_EnableVbi: %#lx VBI not supported for %u", (unsigned long)display, (unsigned)format));
        return BERR_TRACE(rc);
    }

    rc = BVBI_Encode_SetCC(display->vbi.enc_core, isSd && display->vbi.settings.closedCaptionEnabled);
    if (rc) {
        BDBG_WRN(("ClosedCaption is not supported for this display format"));
        return BERR_TRACE(rc);
    }

    rc = BVBI_Encode_SetTeletext(display->vbi.enc_core, isSd && display->vbi.settings.teletextEnabled);
    if (rc) {
        BDBG_WRN(("Teletext is not supported for this display format"));
        return BERR_TRACE(rc);
    }

    rc = BVBI_Encode_SetWSS(display->vbi.enc_core, display->vbi.settings.wssEnabled);
    if (rc) {
        BDBG_WRN(("WSS is not supported for this display format"));
        return BERR_TRACE(rc);
    }

    if(g_NEXUS_DisplayModule_State.moduleSettings.vbi.allowCgmsB){
        /* CGMS is supported for NTSC and 50/60Hz HD, but not PAL. */
        rc = BVBI_Encode_SetCGMSB(display->vbi.enc_core, (format != NEXUS_VideoFormat_ePal) && display->vbi.settings.cgmsEnabled);
        if (rc) {
            BDBG_WRN(("CGMS B is not supported for this display format"));
            return BERR_TRACE(rc);
        }
    }

    /* CGMS is supported for NTSC and 50/60Hz HD, but not PAL. */
    rc = BVBI_Encode_SetCGMSA(display->vbi.enc_core, (format != NEXUS_VideoFormat_ePal) && display->vbi.settings.cgmsEnabled);
    if (rc) {
        BDBG_WRN(("CGMS A is not supported for this display format"));
        return BERR_TRACE(rc);
    }

    rc = BVBI_Encode_SetVPS(display->vbi.enc_core, display->vbi.settings.vpsEnabled);
    if (rc) {
        BDBG_WRN(("VPS is not supported for this display format"));
        return BERR_TRACE(rc);
    }

    if (display->vbi.settings.gemStarEnabled) {
        BVBI_GSOptions gsOptions;
        (void)BVBI_Encode_GetGemstarOptions(display->vbi.enc_core, &gsOptions);
        gsOptions.baseline_top = display->vbi.settings.gemStar.baseLineTop;
        gsOptions.linemask_top = display->vbi.settings.gemStar.lineMaskTop;
        gsOptions.baseline_bot = display->vbi.settings.gemStar.baseLineBottom;
        gsOptions.linemask_bot = display->vbi.settings.gemStar.lineMaskBottom;
        rc = BVBI_Encode_SetGemstarOptions(display->vbi.enc_core, &gsOptions);
        if (rc) return BERR_TRACE(rc);
    }

    rc = BVBI_Encode_SetGemstar(display->vbi.enc_core, display->vbi.settings.gemStarEnabled);
    if (rc) {
        BDBG_WRN(("GemStar is not supported for this display format"));
        return BERR_TRACE(rc);
    }

    if (display->vbi.settings.amolEnabled) {
        switch (display->vbi.settings.amol.type) {
        case NEXUS_AmolType_eI:
            display->vbi.amolType = BVBI_AMOL_Type_I; break;
        case NEXUS_AmolType_eII_Lowrate:
            display->vbi.amolType = BVBI_AMOL_Type_II_Lowrate; break;
        case NEXUS_AmolType_eII_Highrate:
            display->vbi.amolType = BVBI_AMOL_Type_II_Highrate; break;
        default:
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        rc = BVBI_Encode_SetAMOLOptions(display->vbi.enc_core, display->vbi.amolType);
        if (rc) return BERR_TRACE(rc);
    }

    rc = BVBI_Encode_SetAMOL(display->vbi.enc_core,
        /* AMOL only supported for NTSC and NTSC-J */
        (format == NEXUS_VideoFormat_eNtsc || format == NEXUS_VideoFormat_eNtscJapan) && display->vbi.settings.amolEnabled);
    if (rc) {
        BDBG_WRN(("AMOL is not supported"));
        return BERR_TRACE(rc);
    }


    rc = BVBI_Encode_ApplyChanges(display->vbi.enc_core);
    if (rc) {return BERR_TRACE(rc);}

    return BERR_SUCCESS;
}

#endif /* B_HAS_VBI_ENCODE */

void NEXUS_Display_GetVbiSettings(NEXUS_DisplayHandle display, NEXUS_DisplayVbiSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    *pSettings = display->vbi.settings;
}

NEXUS_Error NEXUS_Display_SetVbiSettings(NEXUS_DisplayHandle display, const NEXUS_DisplayVbiSettings *pSettings)
{
    NEXUS_Error rc;
    NEXUS_VideoInput prevInput;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    
    if (pSettings->amolEnabled && !g_NEXUS_DisplayModule_State.moduleSettings.vbi.allowAmol) {
        BDBG_WRN(("cannot enable amol because NEXUS_DisplayModuleSettings.vbi.allowAmol is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (pSettings->gemStarEnabled && !g_NEXUS_DisplayModule_State.moduleSettings.vbi.allowGemStar) {
        BDBG_WRN(("cannot enable gemstar because NEXUS_DisplayModuleSettings.vbi.allowGemStar is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (pSettings->teletextEnabled && !g_NEXUS_DisplayModule_State.moduleSettings.vbi.allowTeletext) {
        BDBG_WRN(("cannot enable teletext because NEXUS_DisplayModuleSettings.vbi.allowTeletext is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (pSettings->vpsEnabled && !g_NEXUS_DisplayModule_State.moduleSettings.vbi.allowVps) {
        BDBG_WRN(("cannot enable vps because NEXUS_DisplayModuleSettings.vbi.allowVps is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    prevInput = display->vbi.settings.vbiSource;

    display->vbi.settings = *pSettings;

    if (prevInput) {
        NEXUS_VideoInput_P_SetVbiState(prevInput);
    }
    if (pSettings->vbiSource && pSettings->vbiSource != prevInput) {
        NEXUS_VideoInput_P_SetVbiState(pSettings->vbiSource);
    }

    rc = NEXUS_Display_P_EnableVbi(display, display->cfg.format);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

static BERR_Code NEXUS_Display_P_SendTeletextField(NEXUS_DisplayHandle display, BVBI_TT_Line *ttLines, unsigned ttLineCount, bool topField)
{
    BERR_Code rc;
    BVBI_Field_Handle field = NULL;
    const NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;
    BFMT_VideoFmt formatVdc;

    rc = NEXUS_P_VideoFormat_ToMagnum(display->cfg.format, &formatVdc);
    if (rc) {return BERR_TRACE(rc);}

    BKNI_EnterCriticalSection();
    /* Set up the field. If we fail, make sure to exit the critical section
    and free memory */
    /* VBI PI memory allocation for TT was already done at BVBIlist create time. */
    rc = BVBIlib_List_Obtain_isr(video->vbilist, &field);
    if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}

    rc = BVBI_Field_SetTTData_isr(field, formatVdc, ttLineCount, ttLines);
    if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}

    /* We are encoding on either field. I don't believe it matters if the data is transmitted top or bottom.
    If it does, we will need to modify. */
    rc = BVBI_Field_SetPolarity_isr(field, topField ? (1<<BAVC_Polarity_eTopField) : (1<<BAVC_Polarity_eBotField));
    if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}

    /* This actually sends it to the VEC and consumes the field */
    if (BVBIlib_Encode_Enqueue_isr(display->vbi.enc, field)) {
        /* if it fails, assume it's a queue overflow and we're done */
        BVBIlib_List_Return_isr(video->vbilist, field);
        rc = -1;
    }

done_critsec:
    BKNI_LeaveCriticalSection();
    return rc;
}

/**
The following comments apply to NEXUS_Display_WriteTeletext and the PAL display format.

Teletext goes out on VBI lines 5..24. This is 20 lines.
The VBI PI has a hardcoded BVBI_TT_MAX_LINES, which is 18. Nexus uses this number.
The VBI PI's BVBI_TT_Line[0] corresponds to VBI line 5, BVBI_TT_Line[19] correponds to VBI line 24.
VBI line 5 must be skipped because the HW does not support it. TT_START_OFFSET specifies that 1 must be skipped.
    Apparently 5,6&7 had to be skipped on a 3563.
If VPS is enabled, VBI line 16 must be skipped.
If CC is enabled, VBI line 22 must be skipped.
If WSS is enabled, VBI line 23 must be skipped.
NEXUS_Display_WriteTeletext can skip these lines by setting an invalid framing code.
Therefore, if neither VPS, CC or WSS are used, the max # of TT lines/field is 19. If all three are in use, the max is 16.

TODO: if a user needs exactly lineNumber control in the application, I recommend we add bool NEXUS_DisplayVbiSettings.teletextLinenumEnabled
to enable this. it would default to false so this code remains backward compatible.
we might also want an option to just ignore lineNumber completely. In that case, NEXUS_DisplayVbiSettings.teletextLinenumMode w/ a tristate enum.
**/
#define MAX_TT_LINES_PER_FIELD (BVBI_TT_MAX_LINES)
#define TT_VBI_LINE_OFFSET 5
#if BCHP_CHIP == 3563
#define TT_START_OFFSET 3
#else
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#define TT_START_OFFSET 2
#else
#define TT_START_OFFSET 1
#endif
#endif

NEXUS_Error NEXUS_Display_WriteTeletext(NEXUS_DisplayHandle display, const NEXUS_TeletextLine *pLines,
    size_t numLines, size_t *pNumLinesWritten)
{
    BERR_Code rc = 0;
    unsigned i;

    if (numLines && !pLines) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    *pNumLinesWritten = 0; /* in case of error, assigned early */
    if (!display->vbi.settings.teletextEnabled) {
        BDBG_WRN(("NEXUS_DisplayVbiSettings.teletextEnabled is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    
    /* malloc space when needed. free on module shutdown. */
    if (!g_NEXUS_DisplayModule_State.ttLines) {
        g_NEXUS_DisplayModule_State.ttLines = BKNI_Malloc(sizeof(g_NEXUS_DisplayModule_State.ttLines[0]) * MAX_TT_LINES_PER_FIELD);
        if (!g_NEXUS_DisplayModule_State.ttLines) {
            return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        }
    }

    for (i=0;i<numLines;) {
        /* prep ttLines[] */
        BVBI_TT_Line *ttLines = g_NEXUS_DisplayModule_State.ttLines;
        unsigned ttLineCount = 0;
        unsigned prevLineNumber = 0;
        unsigned j;

        for (j=0;j<MAX_TT_LINES_PER_FIELD;j++) {
            ttLines[j].ucFramingCode = BVBI_TT_INVALID_FRAMING_CODE;
        }

        /* skip the initial number of lines that can't be output */
        ttLineCount += TT_START_OFFSET;

        for (;i<numLines && ttLineCount<MAX_TT_LINES_PER_FIELD;i++,ttLineCount++) {
            if (pLines[i].lineNumber < prevLineNumber) {
                /* the app wants a field break here */
                break;
            }

            /* skip lines which are used by other VBI standards */
            if ((ttLineCount == 16 - TT_VBI_LINE_OFFSET) && display->vbi.settings.vpsEnabled) ttLineCount++;
            /* NOTE: these conflicts aren't even possible if TT_VBI_LINE_OFFSET==1 and MAX_TT_LINES_PER_FIELD==18, but they are here for completeness. */
            if ((ttLineCount == 22 - TT_VBI_LINE_OFFSET) && display->vbi.settings.closedCaptionEnabled) ttLineCount++;
            if ((ttLineCount == 23 - TT_VBI_LINE_OFFSET) && display->vbi.settings.wssEnabled) ttLineCount++;

            if (ttLineCount >= MAX_TT_LINES_PER_FIELD) {
                /* need a field break here */
                break;
            }

            /* only do work if not BVBI_TT_INVALID_FRAMING_CODE. but always consume the pLines[i] line. */
            if (pLines[i].framingCode != BVBI_TT_INVALID_FRAMING_CODE) {
                ttLines[ttLineCount].ucFramingCode = pLines[i].framingCode;
                BDBG_CASSERT(NEXUS_TELETEXT_LINESIZE == sizeof(ttLines[0].aucData));
                BKNI_Memcpy(ttLines[ttLineCount].aucData, pLines[i].data, NEXUS_TELETEXT_LINESIZE);
                BDBG_MSG_TRACE(("copying line[%d] to ttLines[%d]: %02x", i, ttLineCount, ttLines[ttLineCount].ucFramingCode));
            }

            prevLineNumber = pLines[i].lineNumber;
        }

        rc = NEXUS_Display_P_SendTeletextField(display, ttLines, ttLineCount, pLines->topField);
        if (rc) {return 0;} /* Failure is normal for flow control. */

        BDBG_MSG_TRACE(("NEXUS_Display_WriteTeletext: sent %d VBI lines, %d of %d user lines consumed", ttLineCount, i, numLines));
        (*pNumLinesWritten) = i; /* total actually consumed */
    }

    BDBG_ASSERT(*pNumLinesWritten <= i);

    return rc;
}

NEXUS_Error NEXUS_Display_WriteClosedCaption(NEXUS_DisplayHandle display, const NEXUS_ClosedCaptionData *pEntries,
    size_t numEntries, size_t *pNumEntriesWritten)
{
    BVBI_Field_Handle field = NULL;
    BERR_Code rc = 0;
    unsigned i;
    int full = 0;
    const NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;

    *pNumEntriesWritten = 0; /* in case of error, assigned early */
    if (!display->vbi.settings.closedCaptionEnabled) {
        BDBG_WRN(("NEXUS_DisplayVbiSettings.closedCaptionEnabled is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    for (i=0;i<numEntries;i++) {
        uint32_t polarityMask;

        BKNI_EnterCriticalSection();

        /* Set up the field. If we fail, make sure to exit the critical section
        and free memory */
        rc = BVBIlib_List_Obtain_isr(video->vbilist, &field);
        if (rc) {rc = 0;goto done_critsec;} /* running out of fields is normal flow control */

        rc = BVBI_Field_SetCCData_isr(field, pEntries[i].data[0], pEntries[i].data[1]);
        if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}

        switch (pEntries[i].field) {
        case 0: polarityMask = (1<<BAVC_Polarity_eTopField); break;
        case 1: polarityMask = (1<<BAVC_Polarity_eBotField); break;
        default: rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto done_critsec;
        }
        rc = BVBI_Field_SetPolarity_isr(field, polarityMask);
        if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}

        /* This actually sends it to the VEC and consumes the field */
        if (BVBIlib_Encode_Enqueue_isr(display->vbi.enc, field)) {
            /* if it fails, assume it's a queue overflow and we're done */
            full = 1;
        }
        else {
            field = NULL;
        }

done_critsec:
        if (field) {
            BVBIlib_List_Return_isr(video->vbilist, field);
        }
        BKNI_LeaveCriticalSection();

        if (full || rc)
            break;

        field = NULL; /* consumed */
        (*pNumEntriesWritten)++;
    }

    return rc;
}

NEXUS_Error NEXUS_Display_WriteGemStar(NEXUS_DisplayHandle display, const NEXUS_GemStarData *pEntries,
    size_t numEntries, size_t *pNumEntriesWritten)
{

    BVBI_Field_Handle field = NULL;
    BERR_Code rc = 0;
    unsigned i;
    int full = 0;
    const NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;

    *pNumEntriesWritten = 0; /* in case of error, assigned early */

    if (!display->vbi.settings.gemStarEnabled) {
        BDBG_WRN(("NEXUS_DisplayVbiSettings.gemStarEnabled is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    for (i=0;i<numEntries;i++) {
        /* Convert from Nexus to Magnum */
        BVBI_GSData gsData;
        gsData.ulDataLines = pEntries[i].lineMask;
        gsData.ulErrorLines = 0;
        BDBG_CASSERT(sizeof(pEntries[0].data) == sizeof(gsData.ulData));
        BKNI_Memcpy(gsData.ulData, pEntries[i].data, sizeof(pEntries[i].data));

        BKNI_EnterCriticalSection();

        /* Set up the field. If we fail, make sure to exit the critical section
        and free memory */
        rc = BVBIlib_List_Obtain_isr(video->vbilist, &field);
        if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}
        rc = BVBI_Field_SetGSData_isr(field, &gsData);
        if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}

        if (pEntries[i].topField) {
            rc = BVBI_Field_SetPolarity_isr(field, 1<<BAVC_Polarity_eTopField);
            if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}
        }
        else {
            rc = BVBI_Field_SetPolarity_isr(field, 1<<BAVC_Polarity_eBotField);
            if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}
        }

        /* This actually sends it to the VEC and consumes the field */
        if (BVBIlib_Encode_Enqueue_isr(display->vbi.enc, field)) {
            /* if it fails, assume it's a queue overflow and we're done */
            full = 1;
        }
        else {
            field = NULL;
        }

done_critsec:
        if (field) {
            BVBIlib_List_Return_isr(video->vbilist, field);
        }
        BKNI_LeaveCriticalSection();

        if (full || rc)
            break;

        field = NULL; /* consumed */
        (*pNumEntriesWritten)++;
    }

    return rc;
}

NEXUS_Error NEXUS_Display_SetWss(NEXUS_DisplayHandle display, uint16_t wssData)
{
    if (!display->vbi.settings.wssEnabled) {
        BDBG_WRN(("NEXUS_DisplayVbiSettings.wssEnabled is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    BKNI_EnterCriticalSection();
    display->vbi.pending.wssSet = true;
    display->vbi.pending.wssData = wssData;
    BKNI_LeaveCriticalSection();

    return 0;
}

NEXUS_Error NEXUS_Display_SetCgms(NEXUS_DisplayHandle display, uint32_t cgmsData)
{
    if (!display->vbi.settings.cgmsEnabled) {
        BDBG_WRN(("NEXUS_DisplayVbiSettings.cgmsEnabled is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    BKNI_EnterCriticalSection();
    /* must queue CGMS value for both top & bottom fields */
    display->vbi.pending.cgmsTopSet = true;
    display->vbi.pending.cgmsBottomSet = true;
    display->vbi.pending.cgmsData = cgmsData;
    BKNI_LeaveCriticalSection();

    return 0;
}

NEXUS_Error NEXUS_Display_SetCgmsB(NEXUS_DisplayHandle display, const uint32_t *pCgmsData, unsigned size )
{
    if (!pCgmsData) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    if (!g_NEXUS_DisplayModule_State.moduleSettings.vbi.allowCgmsB) {
        BDBG_WRN(("NEXUS_DisplayModuleSettings.vbi.allowCgmsB is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (!display->vbi.settings.cgmsEnabled) {
        BDBG_WRN(("NEXUS_DisplayVbiSettings.cgmsEnabled is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (size < 5) {
        BDBG_WRN(("CGMS-B requires an array of 5 uint32_t's"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    BKNI_EnterCriticalSection();
    /* must queue CGMS-B value for both top & bottom fields */
    display->vbi.pending.cgmsBTopSet = true;
    display->vbi.pending.cgmsBBottomSet = true;
    BKNI_Memcpy(display->vbi.pending.cgmsBData, pCgmsData, sizeof(uint32_t)*5);
    BKNI_LeaveCriticalSection();

    return 0;
}

NEXUS_Error NEXUS_Display_SetVps(NEXUS_DisplayHandle display, const NEXUS_VpsData *pData)
{
    if (!display->vbi.settings.vpsEnabled) {
        BDBG_WRN(("NEXUS_DisplayVbiSettings.vpsEnabled is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    /* only enqueue top field for VPS */
    BKNI_EnterCriticalSection();
    display->vbi.pending.vpsSet = true;
    display->vbi.pending.vpsData.ucByte05 = pData->byte05;
    display->vbi.pending.vpsData.ucByte11 = pData->byte11;
    display->vbi.pending.vpsData.ucByte12 = pData->byte12;
    display->vbi.pending.vpsData.ucByte13 = pData->byte13;
    display->vbi.pending.vpsData.ucByte14 = pData->byte14;
    display->vbi.pending.vpsData.ucByte15 = pData->byte15;
    BKNI_LeaveCriticalSection();

    return 0;
}

/* bvdc_nomacrovision_priv.c does not provide stubs for public API, so we must #if */
#ifdef MACROVISION_SUPPORT
#include "bvdc_macrovision.h"
#endif

NEXUS_Error NEXUS_Display_SetMacrovision( NEXUS_DisplayHandle display, NEXUS_DisplayMacrovisionType type, const NEXUS_DisplayMacrovisionTables *pTable)
{
#ifdef MACROVISION_SUPPORT
    BERR_Code rc;
    BVDC_MacrovisionType mv_type;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    switch (type) {
    case NEXUS_DisplayMacrovisionType_eAgcOnly:
        mv_type = BVDC_MacrovisionType_eAgcOnly;
        break;
    case NEXUS_DisplayMacrovisionType_eAgc2Lines:
        mv_type = BVDC_MacrovisionType_eAgc2Lines;
        break;
    case NEXUS_DisplayMacrovisionType_eAgc4Lines:
        mv_type = BVDC_MacrovisionType_eAgc4Lines;
        break;
    case NEXUS_DisplayMacrovisionType_eAgcOnlyRgb:
        mv_type = BVDC_MacrovisionType_eAgcOnly_Rgb;
        break;
    case NEXUS_DisplayMacrovisionType_eAgc2LinesRgb:
        mv_type = BVDC_MacrovisionType_eAgc2Lines_Rgb;
        break;
    case NEXUS_DisplayMacrovisionType_eAgc4LinesRgb:
        mv_type = BVDC_MacrovisionType_eAgc4Lines_Rgb;
        break;
    case NEXUS_DisplayMacrovisionType_eTest01:
        mv_type = BVDC_MacrovisionType_eTest01;
        break;
    case NEXUS_DisplayMacrovisionType_eTest02:
        mv_type = BVDC_MacrovisionType_eTest02;
        break;
    case NEXUS_DisplayMacrovisionType_eCustom:
        mv_type = BVDC_MacrovisionType_eCustomized;
        break;
    default:
    case NEXUS_DisplayMacrovisionType_eNone:
        mv_type = BVDC_MacrovisionType_eNoProtection;
        break;
    }

    rc = BVDC_Display_SetMacrovisionType(display->displayVdc, mv_type);
    if (rc) return BERR_TRACE(rc);

    if (type == NEXUS_DisplayMacrovisionType_eCustom && pTable) {
        rc = BVDC_Display_SetMacrovisionTable(display->displayVdc, pTable->cpcTable, pTable->cpsTable);
        if (rc) return BERR_TRACE(rc);
    }
    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);
    return 0;
#else
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(type);
    BSTD_UNUSED(pTable);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

/* bvdc_nodcs_priv.c does not provide stubs for public API, so we must #if */
#ifdef DCS_SUPPORT
#include "bvdc_dcs.h"
#endif

NEXUS_Error NEXUS_Display_SetDcs( NEXUS_DisplayHandle display, NEXUS_DisplayDcsType type )
{
#ifdef DCS_SUPPORT
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    BDBG_CASSERT(BVDC_DCS_Mode_eUndefined == (BVDC_DCS_Mode)NEXUS_DisplayDcsType_eMax);
    rc = BVDC_Display_SetDcsMode( display->displayVdc, (BVDC_DCS_Mode)type);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);
    return 0;
#else
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(type);

    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_Display_DcsLock( NEXUS_DisplayHandle display )
{
#ifdef DCS_SUPPORT
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    rc = BVDC_Display_DcsLock(display->displayVdc);
    if (rc) return BERR_TRACE(rc);
    return 0;
#else
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_Display_WriteAmol( NEXUS_DisplayHandle display, const NEXUS_AmolData *pEntries, size_t numEntries, size_t *pNumEntriesWritten )
{
    BVBI_Field_Handle field = NULL;
    BERR_Code rc = 0;
    unsigned i;
    int full = 0;
    const NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    *pNumEntriesWritten = 0; /* in case of error, assigned early */
    if (!display->vbi.settings.amolEnabled) {
        BDBG_WRN(("NEXUS_DisplayVbiSettings.amolEnabled is false"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    for (i=0;i<numEntries;i++) {
        uint32_t polarityMask;

        BKNI_EnterCriticalSection();

        /* Set up the field. If we fail, make sure to exit the critical section
        and free memory */
        rc = BVBIlib_List_Obtain_isr(video->vbilist, &field);
        if (rc) {rc = 0;goto done_critsec;} /* running out of fields is normal flow control */

        rc = BVBI_Field_SetAMOLData_isr(field, display->vbi.amolType, (uint8_t*)pEntries[i].data, pEntries[i].length);
        if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}

        polarityMask = pEntries[i].topField ? (1<<BAVC_Polarity_eTopField) : (1<<BAVC_Polarity_eBotField);
        rc = BVBI_Field_SetPolarity_isr(field, polarityMask);
        if (rc) {rc = BERR_TRACE(rc);goto done_critsec;}

        /* This actually sends it to the VEC and consumes the field */
        if (BVBIlib_Encode_Enqueue_isr(display->vbi.enc, field)) {
            /* if it fails, assume it's a queue overflow and we're done */
            full = 1;
        }
        else {
            field = NULL;
        }

done_critsec:
        if (field) {
            BVBIlib_List_Return_isr(video->vbilist, field);
        }
        BKNI_LeaveCriticalSection();

        if (full || rc)
            break;

        field = NULL; /* consumed */
        (*pNumEntriesWritten)++;
    }
    return rc;
}

#else /* B_HAS_VBI */

BERR_Code
NEXUS_Display_P_ConnectVbi(NEXUS_DisplayHandle display)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    return 0;
}

void
NEXUS_Display_P_DisconnectVbi(NEXUS_DisplayHandle display)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
}

BERR_Code
NEXUS_Display_P_VbiData_isr(NEXUS_DisplayHandle display, BVBI_Field_Handle vbiData)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(vbiData);
    return 0;
}

void
NEXUS_Display_P_DisableVbi(NEXUS_DisplayHandle display)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
}

BERR_Code
NEXUS_Display_P_EnableVbi(NEXUS_DisplayHandle display, NEXUS_VideoFormat format)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(format);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

void NEXUS_Display_GetVbiSettings(NEXUS_DisplayHandle display, NEXUS_DisplayVbiSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_Display_SetVbiSettings(NEXUS_DisplayHandle display, const NEXUS_DisplayVbiSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(pSettings);
    return BERR_SUCCESS;
}

BERR_Code NEXUS_Display_P_SendTeletextField(NEXUS_DisplayHandle display, BVBI_TT_Line *ttLines, unsigned ttLineCount)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(ttLines);
    BSTD_UNUSED(ttLineCount);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_WriteTeletext(NEXUS_DisplayHandle display, const NEXUS_TeletextLine *pLines,
    size_t numLines, size_t *pNumLinesWritten)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(pLines);
    BSTD_UNUSED(numLines);
    BSTD_UNUSED(pNumLinesWritten);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_WriteClosedCaption(NEXUS_DisplayHandle display, const NEXUS_ClosedCaptionData *pEntries,
    size_t numEntries, size_t *pNumEntriesWritten)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(pEntries);
    BSTD_UNUSED(numEntries);
    BSTD_UNUSED(pNumEntriesWritten);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_WriteGemStar(NEXUS_DisplayHandle display, const NEXUS_GemStarData *pEntries,
    size_t numEntries, size_t *pNumEntriesWritten)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(pEntries);
    BSTD_UNUSED(numEntries);
    BSTD_UNUSED(pNumEntriesWritten);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}


NEXUS_Error NEXUS_Display_SetWss(NEXUS_DisplayHandle display, uint16_t wssData)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(wssData);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_SetCgms(NEXUS_DisplayHandle display, uint32_t cgmsData)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(cgmsData);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_SetCgmsB(NEXUS_DisplayHandle display, const uint32_t *pCgmsData, unsigned size )
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(pCgmsData);
    BSTD_UNUSED(size);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_SetVps(NEXUS_DisplayHandle display, const NEXUS_VpsData *pData)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(pData);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_SetMacrovision( NEXUS_DisplayHandle display, NEXUS_DisplayMacrovisionType type, const NEXUS_DisplayMacrovisionTables *pTable)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(type);
    BSTD_UNUSED(pTable);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_SetDcs( NEXUS_DisplayHandle display, NEXUS_DisplayDcsType type )
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(type);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_Display_WriteAmol( NEXUS_DisplayHandle display, const NEXUS_AmolData *pEntries, size_t numEntries, size_t *pNumEntriesWritten )
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BSTD_UNUSED(pEntries);
    BSTD_UNUSED(numEntries);
    BSTD_UNUSED(pNumEntriesWritten);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

#endif /* B_HAS_VBI */
