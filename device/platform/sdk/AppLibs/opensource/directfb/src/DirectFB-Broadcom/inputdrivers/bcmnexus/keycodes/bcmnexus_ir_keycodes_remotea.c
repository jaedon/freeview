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
 * $brcm_Workfile: bcmnexus_ir_keycodes_remotea.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 *
 * [File Description:]
 *
 * Revision History:
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/inputdrivers/bcmnexus/keycodes/bcmnexus_ir_keycodes_remotea.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 6:10p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/2   3/14/11 4:55p robertwm
 * SW3556-1224: DirectFB: Cannot build IR driver for 3556 platform.
 * SW3556-1225: DirectFB: DirectFB-1.4.1 Phase 3.0 PreRel fails to build.
 * SW35125-128: DirectFB: IR remote protocol default is wrong and causes
 *  compilation error.
 * SW7420-1654: DirectFB: Add run-time IR protocol and keymap selection.
 * 
 * 
 * This code implements a DirectFB keycodes mapping module for the IR receiver.
 * It was shaken out using the "One For All" remote; it will need additional work
 * to support other devices.  It will also need additional work to support
 * simultaneous IR devices.
 ***************************************************************************/
#include "bcmnexus_ir_keycodes_remotea.h"

DFB_BCMNEXUS_IR_KEYCODES(remotea)

static int bcmnexus_ir_get_basic_keycodes_size(void)
{
    return sizeof( IrBasicCodeMap ) / sizeof ( KeyMapElement );
}

static int bcmnexus_ir_get_advan_keycodes_size(void)
{
    return sizeof( IrAdvanCodeMap ) / sizeof ( KeyMapElement );
}

static void bcmnexus_ir_get_basic_keycodes(KeyMapElement * pIrBasicKeycodes)
{
    int num = sizeof( IrBasicCodeMap ) / sizeof ( KeyMapElement );
    for (int i = 0; i < num ; i ++)
    {
        pIrBasicKeycodes[i].dfbKeySymbol = IrBasicCodeMap[i].dfbKeySymbol;
        pIrBasicKeycodes[i].hwEventCode = IrBasicCodeMap[i].hwEventCode;
    }     
}

static void bcmnexus_ir_get_advance_keycodes(KeyMapElement * pIrAdvanKeycodes)
{
    int num = sizeof( IrAdvanCodeMap ) / sizeof ( KeyMapElement );
    for (int i = 0; i < num ; i ++)
    {
        pIrAdvanKeycodes[i].dfbKeySymbol = IrAdvanCodeMap[i].dfbKeySymbol;
        pIrAdvanKeycodes[i].hwEventCode = IrAdvanCodeMap[i].hwEventCode;		 	
    }
}

