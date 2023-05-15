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
 * $brcm_Workfile: nexus_video_output_impl.h $
 * $brcm_Revision: 24 $
 * $brcm_Date: 8/1/12 10:22a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_video_output_impl.h $
 * 
 * 24   8/1/12 10:22a erickson
 * SW7552-307: back out change
 * 
 * 22   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 21   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 20   7/22/11 9:03a erickson
 * SW7425-745: add NEXUS_VideoOutputSettings
 * 
 * 19   7/19/11 11:14a erickson
 * SW7408-291: add NEXUS_VideoOutput_SetVfFilter, requires reverse lookup
 *  of DAC assignments
 * 
 * 18   3/28/11 3:45p vsilyaev
 * SW7335-1214: Added NEXUS_CallbackHandler framework
 * 
 * 17   9/17/09 4:11p erickson
 * SW7405-3033: back out last change. fix datatype instead.
 * 
 * 16   9/17/09 3:01p erickson
 * SW3548-2348: remove unused member
 *
 * 15   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 97401_nexus/1   6/26/09 11:20a gmohile
 * PR 56400 : Add 7401 nexus support
 *
 * 14   5/20/09 3:29p erickson
 * PR54880: keep track of whether VDC heaps were created and only destroy
 *  what was created
 *
 * 13   4/1/09 12:16p erickson
 * PR53623: merge 656 output support
 *
 * PR53623/1   3/31/09 10:08a mward
 * PR 53623: 656 output support.
 *
 * 12   2/20/09 11:25a erickson
 * PR52347: remove special case for component
 *
 * 11   12/17/08 11:40p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * 10   8/19/08 12:22p erickson
 * PR45794: clarify internal function names. add ASSERT's and comments.
 *
 * 9   7/7/08 3:19p erickson
 * PR44452: closing video outputs should automatically remove them from
 *  displays
 *
 * 8   6/17/08 4:04p vsilyaev
 * PR 40921: Added output portion of scart inputs
 *
 * 7   5/21/08 10:33a erickson
 * PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
 *
 * 6   5/12/08 4:09p erickson
 * PR42628: free NEXUS_VideoOutput_P_Link when DisplayModule closes
 *
 * 5   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 4   4/3/08 12:11p erickson
 * PR41302: add MPAA decimation
 *
 * 3   2/1/08 5:25p jgarrett
 * PR 39017: Adding HdmiOutput
 *
 * 2   1/31/08 9:55a erickson
 * PR36808: added SyncChannel interface
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/6   1/3/08 8:43p erickson
 * PR34662: refactor into smaller files
 *
 * Nexus_Devel/5   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 *
 * Nexus_Devel/4   10/4/07 1:09p vsilyaev
 * PR 34662: Removed VideoOutputDesc
 *
 * Nexus_Devel/3   9/25/07 3:12p vsilyaev
 * PR 34662: Fixed video output
 *
 * Nexus_Devel/2   9/25/07 1:02p vsilyaev
 * PR 34662: Fixed file names
 *
 * Nexus_Devel/1   9/25/07 12:49p vsilyaev
 * PR 34662: Internal API for display outputs
 *
 **************************************************************************/
/* this file shall be included only from nexus_display_module.h */
#ifndef NEXUS_VIDEO_OUTPUT_IMPL_H__
#define NEXUS_VIDEO_OUTPUT_IMPL_H__

/**
This section contains internal API's for the generic NEXUS_VideoOutput connector.
**/

BDBG_OBJECT_ID_DECLARE(NEXUS_VideoOutput_P_Link);

struct NEXUS_VideoOutput_P_Link;

typedef struct NEXUS_VideoOutput_P_Iface {
    BERR_Code (*connect)(void *output,  NEXUS_DisplayHandle display);
    BERR_Code (*disconnect)(void *output,  NEXUS_DisplayHandle display);
    BERR_Code (*formatChange)(void *output, NEXUS_DisplayHandle display, NEXUS_VideoFormat format, NEXUS_DisplayAspectRatio aspectRatio);
} NEXUS_VideoOutput_P_Iface;

typedef struct NEXUS_VideoOutput_P_Link {
    BDBG_OBJECT(NEXUS_VideoOutput_P_Link)
    BLST_D_ENTRY(NEXUS_VideoOutput_P_Link) link; /* list of what's displayed */
    NEXUS_DisplayHandle display; /* display that output is connected to */
    NEXUS_VideoOutput output; /* handle of the specific video output */
    NEXUS_VideoOutput_P_Iface iface;
    bool sdOnly;
    BVDC_DisplayOutput displayOutput;
    BVDC_DacOutput channel[NEXUS_VideoDac_eMax];
    NEXUS_VideoOutputSettings settings;
} NEXUS_VideoOutput_P_Link;

NEXUS_VideoOutput_P_Link *
NEXUS_P_VideoOutput_Link(NEXUS_VideoOutput output);

NEXUS_VideoOutput_P_Link *
NEXUS_VideoOutput_P_CreateLink(NEXUS_VideoOutput output, const NEXUS_VideoOutput_P_Iface *iface, bool sdOnly);

void
NEXUS_VideoOutput_P_DestroyLink(NEXUS_VideoOutput_P_Link *);

NEXUS_DisplayHandle
NEXUS_VideoOutput_P_GetDisplay(NEXUS_VideoOutput output);

/**
This section contains internal API's for specific video outputs.
**/

struct NEXUS_ComponentOutput {
    NEXUS_OBJECT(NEXUS_ComponentOutput);
    NEXUS_ComponentOutputSettings cfg;
    bool opened;
    NEXUS_VideoOutputObject output;
    unsigned index;
};

struct NEXUS_CompositeOutput {
    NEXUS_OBJECT(NEXUS_CompositeOutput);
    NEXUS_CompositeOutputSettings cfg;
    bool opened;
    NEXUS_VideoOutputObject output;
    BVDC_DacOutput dacOutputType; /* only used for 3548/3556 for AnalogVideoDecoder bypass */
};

struct NEXUS_SvideoOutput {
    NEXUS_OBJECT(NEXUS_SvideoOutput);
    NEXUS_SvideoOutputSettings cfg;
    bool opened;
    NEXUS_VideoOutputObject output;
};

struct NEXUS_Ccir656Output {
    NEXUS_OBJECT(NEXUS_Ccir656Output);
    NEXUS_Ccir656OutputSettings cfg;
    bool opened;
    NEXUS_VideoOutputObject output;
};

void NEXUS_VideoOutputs_P_Init(void);
BERR_Code NEXUS_VideoOutput_P_SetDac(NEXUS_VideoOutput_P_Link *link, BVDC_Display_Handle display, NEXUS_VideoDac dac, BVDC_DacOutput type);
void NEXUS_VideoOutput_P_PostSetHdmiFormat(void);

#if NEXUS_NUM_SCART_INPUTS
NEXUS_VideoOutput_P_Link * NEXUS_VideoOutput_P_OpenScartInput(NEXUS_VideoOutput output);
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
void NEXUS_VideoOutput_P_SetHdmiSettings(void *context);
#endif

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_ComponentOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_SvideoOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_CompositeOutput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Ccir656Output);

#endif /* NEXUS_VIDEO_OUTPUT_IMPL_H__ */


