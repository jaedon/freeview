/***************************************************************************
 *     (c)2007-2010 Broadcom Corporation
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
 * $brcm_Workfile: nexus_iplicensing.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/3/10 11:04a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/7400/iplicensing_ext/include/nexus_iplicensing.h $
 * 
 * 1   8/3/10 11:04a erickson
 * SW7405-4696: add support for IP Licensing
 *
 **************************************************************************/
#ifndef NEXUS_SECURITYIPLICENSING_H__
#define NEXUS_SECURITYIPLICENSING_H__

#include "nexus_security_datatypes.h"
#include "nexus_security.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NEXUS_SECURITY_IPLICENSING_BUF_LEN       304
/**
Summary:
This enum defines the DS2 data type for the IP Licensing op

Description:
See Also:
NEXUS_SecurityIpLicensing
**/
typedef enum NEXUS_SecurityIpLicensingDS2KeyType
{
    NEXUS_SecurityIpLicensingDS2KeyType_eLSB,
    NEXUS_SecurityIpLicensingDS2KeyType_eMSB,
    NEXUS_SecurityIpLicensingDS2KeyType_eMax
} NEXUS_SecurityIpLicensingDS2KeyType;

/**
Summary:
This structure provides the IP Licensing data operation settings

Description:
See Also:
NEXUS_Security_PerformIpLicensing
**/
typedef struct NEXUS_SecurityIpLicensingSettings
{
    NEXUS_SecurityIpLicensingDS2KeyType ds2KeyType;
    uint32_t                            dataSize;
    uint8_t                             inputBuf[NEXUS_SECURITY_IPLICENSING_BUF_LEN];
} NEXUS_SecurityIpLicensingSettings;



/**
Summary:
Get default NEXUS_SecurityIpLicensingSettings
**/
void NEXUS_Security_GetDefaultIpLicensingSettings(
    NEXUS_SecurityIpLicensingSettings *pSettings /* [out] */
    );

/**
Summary:
This function performs IP Licensing operation

Description:
This function encrypts/decrypts data based on the AES key
**/
NEXUS_Error NEXUS_Security_PerformIpLicensing(
    const NEXUS_SecurityIpLicensingSettings *pSettings
    );

#ifdef __cplusplus
}
#endif

#endif
