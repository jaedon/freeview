/***************************************************************************
*     (c)2010-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_core_security.h $
* $brcm_Revision: 4 $
* $brcm_Date: 7/27/12 2:14p $
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/include/priv/nexus_core_security.h $
* 
* 4   7/27/12 2:14p jgarrett
* SW7425-3281: Merge to main branch
* 
* SW7425-3281/1   7/8/12 4:54p piyushg
* SW7425-3281: Added support for playready licensce acquisition and Nexus
*  fileio
* 
* 3   6/19/12 6:21p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 2   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 1   8/4/10 3:07p mphillip
* SW7405-4325: Merge keyslot refactoring to main
* 
* SW7405-4325/1   8/4/10 2:31p mphillip
* SW7405-4325: Move keyslot memory management into core
* 
* 
*
***************************************************************************/
#ifndef NEXUS_CORE_SECURITY_H__
#define NEXUS_CORE_SECURITY_H__

#include "nexus_base.h"

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_KeySlot);

/**
Summary:
This struct defines the key handle structure.

Description:
This structure contains the key handle parameters which will be filled
in NEXUS_Security_AllocateKeySlot.
calls.

See Also:
NEXUS_Security_AllocateKeySlot
**/
typedef struct NEXUS_KeySlot
{
    NEXUS_OBJECT(NEXUS_KeySlot);
    unsigned int keySlotNumber;
    unsigned int keyslotType;
    unsigned int pidChannel; /* TODO: remove this once BHSM_FreeCAKeySlot is fixed on all chips. */
    NEXUS_SecurityEngine cryptoEngine;
    unsigned int drmContext; /* Generic context used to pass drm handle to the playpump. 
                                Note that in MP protected mode, a nexus module must own the drm context */
} NEXUS_KeySlotHandle_t;

/**
Summary:
This function retrieves keyslot information from a keyslot handle.

Description:
This function shall retrieve keyslot information from a keyslot handle.
The keyslot functions are here to remove a DMA dependency on Security.

See Also:
NEXUS_BaseKeySlotInfo
**/
void NEXUS_P_Core_GetKeySlotInfo(
    NEXUS_KeySlotHandle keyHandle,
    NEXUS_SecurityKeySlotInfo *pKeyslotInfo /* [out] */
    );

/**
Summary:
This function handles the allocation of memory for a keyslot handle.

Description:
This function allocates the memory for a keyslot handle.  It should not be called except by the security module.
The keyslot functions are here to remove a DMA dependency on Security.

See Also:
NEXUS_BaseKeySlotInfo
**/
NEXUS_KeySlotHandle
NEXUS_P_Core_AllocateKeySlotHandle(void);

/**
Summary:
This function handles freeing the memory for a keyslot handle.

Description:
This function frees the memory for a keyslot handle.  It should not be called except by the security module.
The keyslot functions are here to remove a DMA dependency on Security.

See Also:
NEXUS_BaseKeySlotInfo
**/
void
NEXUS_P_Core_FreeKeySlotHandle(NEXUS_KeySlotHandle keyHandle);


#endif /* #ifndef NEXUS_CORE_SECURITY_H__ */

