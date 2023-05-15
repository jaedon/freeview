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
 * $brcm_Workfile: nexus_security_types.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/27/12 2:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/core/include/nexus_security_types.h $
 * 
 * 3   7/27/12 2:14p jgarrett
 * SW7425-3281: Merge to main branch
 * 
 * SW7425-3281/1   7/8/12 4:54p piyushg
 * SW7425-3281: Added support for playready licensce acquisition and Nexus
 *  fileio
 * 
 * 2   5/9/11 6:55p mphillip
 * SW7422-430: Merge API change to main
 * 
 * SW7422-430/2   4/25/11 11:55a mphillip
 * SW7422-430: Update key type enum from API review
 * 
 * SW7422-430/1   4/20/11 7:04p mphillip
 * SW7422-430: Add support for specifying keyslot type when allocating a
 *  keyslot
 * 
 * 1   8/4/10 3:07p mphillip
 * SW7405-4325: Merge keyslot refactoring to main
 * 
 * SW7405-4325/1   8/4/10 2:31p mphillip
 * SW7405-4325: Move keyslot memory management into core
 * 
 * 
 *
 **************************************************************************/
#ifndef NEXUS_SECURITY_TYPES_H__
#define NEXUS_SECURITY_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Security keyslot handle for conditional access and cryptography

Description:
See NEXUS_Security_AllocateKeySlot
**/
typedef struct NEXUS_KeySlot *NEXUS_KeySlotHandle;

/**
Summary:
This enum defines the crypto engines.

Description:
The security engine chosen when allocating a keyslot determines what the expected use of the
key will be.  Nexus tracks this information to validate key usage and for proper allocation and
de-allocation.
**/
typedef enum NEXUS_SecurityEngine
{
    NEXUS_SecurityEngine_eCa,
    NEXUS_SecurityEngine_eM2m,
    NEXUS_SecurityEngine_eCp,
    NEXUS_SecurityEngine_eCaCp,
    NEXUS_SecurityEngine_eRmx,
    NEXUS_SecurityEngine_eGeneric, /* Specifies that the key slot will be used only
                                      to pass a drm context to the playpump. */

    NEXUS_SecurityEngine_eMax
} NEXUS_SecurityEngine;

/**
Summary:
This enum defines the supported key slot types.

Description:
This selects the specific keyslot type when allocating a keyslot.

The default setting is NEXUS_SecurityKeySlotType_eAuto, which does not need to be changed.
When set to NEXUS_SecurityKeySlotType_eAuto, the Nexus security module will manage the actual
keyslot type when the keyslot is allocated via NEXUS_Security_AllocateKeySlot.
**/
typedef enum NEXUS_SecurityKeySlotType
{
    NEXUS_SecurityKeySlotType_eAuto,
    NEXUS_SecurityKeySlotType_eType0,
    NEXUS_SecurityKeySlotType_eType1,
    NEXUS_SecurityKeySlotType_eType2,
    NEXUS_SecurityKeySlotType_eType3,
    NEXUS_SecurityKeySlotType_eType4,
    NEXUS_SecurityKeySlotType_eType5,
    NEXUS_SecurityKeySlotType_eType6,
    NEXUS_SecurityKeySlotType_eType7,

    /* Add new key entry type definition before this line */
    NEXUS_SecurityKeySlotType_eMax
}   NEXUS_SecurityKeySlotType;

/**
Summary:
**/
typedef unsigned int NEXUS_SecurityKeySlotNumber;

/**
Summary:
This structure provides information about a keyslot handle.

Description:
See Also:
NEXUS_Security_GetKeySlotInfo
**/
typedef struct NEXUS_SecurityKeySlotInfo
{
    NEXUS_SecurityKeySlotNumber keySlotNumber;
    NEXUS_SecurityEngine        keySlotEngine;
    NEXUS_SecurityKeySlotType   keySlotType;
} NEXUS_SecurityKeySlotInfo;


#ifdef __cplusplus
}
#endif

#endif
