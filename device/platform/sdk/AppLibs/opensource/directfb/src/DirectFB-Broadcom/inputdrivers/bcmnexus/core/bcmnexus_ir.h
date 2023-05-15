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
 * $brcm_Workfile: bcmnexus_ir.h $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/inputdrivers/bcmnexus/core/bcmnexus_ir.h $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 6:09p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/2   3/14/11 4:42p robertwm
 * SW3556-1224: DirectFB: Cannot build IR driver for 3556 platform.
 * SW3556-1225: DirectFB: DirectFB-1.4.1 Phase 3.0 PreRel fails to build.
 * SW35125-128: DirectFB: IR remote protocol default is wrong and causes
 *  compilation error.
 * SW7420-1654: DirectFB: Add run-time IR protocol and keymap selection.
 * 
 * DirectFB_1_4_Port/1   3/2/11 12:08p christ
 * SW7420-1558: DirectFB: Support runtime switching of IR codes for using
 *  different remotes.
 * 
 * DirectFB_1_4_Port/2   9/24/10 4:43p kcoyle
 * SW7420-1118: Add/fix remote codes in DirectFB
 * 
 * DirectFB_1_4_Port/1   8/19/10 4:27p robertwm
 * SW35230-1060: DirectFB: Add support for building DFB with different IR
 *  keycode tables.
 ***************************************************************************/
#ifndef __bcmnexus_ir_H__
#define __bcmnexus_ir_H__


#include <pthread.h>
#include <directfb.h>

#include <direct/modules.h>

#include <fusion/reactor.h>

#include <core/coretypes.h>

#define DFB_BCMNEXUS_IR_KEYCODES_ABI_VERSION 1

DECLARE_MODULE_DIRECTORY( dfb_bcmnexus_ir_keycodes );

/*
** For mapping hardware codes to DirectFB key codes.
*/
typedef struct {

    DFBInputDeviceKeySymbol        dfbKeySymbol;
    uint32_t                       hwEventCode;
} KeyMapElement;

typedef struct {
     int  (*GetBasicKeycodesSize) (void);
     int  (*GetAdvanKeycodesSize) (void);     
     void (*GetBasicKeycodes)     (KeyMapElement *);
     void (*GetAdvanKeycodes)     (KeyMapElement *);
} IrKeycodesMapFuncs;

typedef struct {
     DirectModuleEntry            *module;
     IrKeycodesMapFuncs           *funcs;
}bcmnexus_IR_keycodes;

#endif
