/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_power_management.h $
* $brcm_Revision: 12 $
* $brcm_Date: 2/21/12 5:44p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/7425/include/nexus_power_management.h $
* 
* 12   2/21/12 5:44p jtna
* SW7425-2401: add NEXUS_PowerManagementCore_ePictureDecoderMemory
* 
* 11   8/30/11 7:37a jhaberf
* SWDTV-7671: power management 2.0 changes for DTV
* 
* 10   5/26/11 4:47p erickson
* SW7231-128: add more cores
* 
* 9   8/26/10 12:26p erickson
* SW7420-972: add NEXUS_PowerManagementCore_eRfm
* 
* 8   3/26/10 3:07p erickson
* SW7325-697: added NEXUS_PowerManagementCore_eImageInput. requires for
*  7325/7335 MFD dynamic power management.
*
* 7   6/25/09 2:56p erickson
* PR45902: add NEXUS_PowerManagementCore_eVideoDecoder
*
* 6   6/25/09 10:21a erickson
* PR55968: add ePictureDecoder
*
* 5   6/24/09 12:48p erickson
* PR55968: added Graphics2D and Graphics3D
*
* 4   12/11/08 4:05p erickson
* PR49556: split frontend PM into analog and digital enums
*
* 3   12/10/08 3:19p vsilyaev
* PR 48908: Added transport and DMA modules
*
* 2   10/2/08 11:12a erickson
* PR40799: remove GetCoreStatus. added eFrontend.
*
* 1   9/26/08 11:10a erickson
* PR40799: add internal power management API
*
***************************************************************************/
#ifndef NEXUS_POWER_MANAGEMENT_H
#define NEXUS_POWER_MANAGEMENT_H

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
Summary:
List of cores that can be powered up or down.

Description:
This list of cores is subject to change as new chips are designed and
new power management techniques are developed.
**/
typedef enum NEXUS_PowerManagementCore
{
    NEXUS_PowerManagementCore_eVbiDecoder,
    NEXUS_PowerManagementCore_eVdec, /* AnalogVideoDecoder */
    NEXUS_PowerManagementCore_eVideoDecoder, /* digital video decode */
    NEXUS_PowerManagementCore_eCcir656Input,
    NEXUS_PowerManagementCore_eHdDviInput,
    NEXUS_PowerManagementCore_eHdmiInput,
    NEXUS_PowerManagementCore_eAnalogFrontend, /* any internal IF demod */
    NEXUS_PowerManagementCore_eDigitalFrontend, /* any internal digital demod */
    NEXUS_PowerManagementCore_eTransport,
    NEXUS_PowerManagementCore_eDma,
    NEXUS_PowerManagementCore_eHsm,
    NEXUS_PowerManagementCore_eGraphics2D, /* M2MC */
    NEXUS_PowerManagementCore_eGraphics2DMemory, /* M2MC SRAM */
    NEXUS_PowerManagementCore_eGraphics3D, /* PX3D */
    NEXUS_PowerManagementCore_eGraphics3DMemory, /* Px3D/V3D SRAM */
    NEXUS_PowerManagementCore_ePictureDecoder, /* SID */
    NEXUS_PowerManagementCore_ePictureDecoderMemory, /* SID SRAM */
    NEXUS_PowerManagementCore_eImageInput, /* requires MFD HW */
    NEXUS_PowerManagementCore_eRfm,
    NEXUS_PowerManagementCore_eDfe,
    NEXUS_PowerManagementCore_eDs,
    NEXUS_PowerManagementCore_eThd,
    NEXUS_PowerManagementCore_eMax
} NEXUS_PowerManagementCore;

/**
Summary:
Set the state of a core to either powered up or down.

Description:
Internally, nexus will determine if the actual hardware can be powered up or down based
on a variety of chip-specific conditions. Setting a core to be powered down does not
guarantee that power will actually be turned off. It just instructs Nexus can power
can be turned off if possible.

This is a set function which does not fail. There is no need and no way for a module
to recover from the inability to power down. Inside, Nexus will just leave the core
powered up.
**/
void NEXUS_PowerManagement_SetCoreState(
    NEXUS_PowerManagementCore core,
    bool poweredUp
    );

/**
Summary:
Get the current setting
**/
void NEXUS_PowerManagement_GetCoreState(
    NEXUS_PowerManagementCore core,
    bool *pPoweredUp /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif

