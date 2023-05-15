/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: nexus_frontend_extension.h $
* $brcm_Revision: 1 $
* $brcm_Date: 12/7/10 11:36a $
*
* API Description:
*   API name: 97346 Frontend Extension
*    Extension code to handle 97346 frontend board-specifics
*
* Revision History:
*
* $brcm_Log: /nexus/extensions/frontend/97346/include/nexus_frontend_extension.h $
* 
* 1   12/7/10 11:36a jrubio
* SW7344-9: initial version of extension
* 
* 1   8/6/09 2:12p jrubio
* PR55232: add 7346
* 
*
*
***************************************************************************/
#ifndef NEXUS_FRONTEND_EXTENSION_H__
#define NEXUS_FRONTEND_EXTENSION_H__

typedef struct NEXUS_97346FrontendExtensionSettings
{
    NEXUS_FrontendHandle primary;
    NEXUS_FrontendHandle secondary;
    NEXUS_FrontendHandle qpsk;
    NEXUS_TunerHandle qpskTuner;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
    NEXUS_I2cHandle allegroI2cDevice;
#else
    NEXUS_I2cHandle isl6423I2cDevice;
#endif
    NEXUS_I2cHandle fpgaI2cDevice;
    unsigned boardRev;
} NEXUS_97346FrontendExtensionSettings;

typedef struct NEXUS_97346FrontendExtensionHandles
{
    NEXUS_FrontendHandle primary;
    NEXUS_FrontendHandle secondary;
    NEXUS_FrontendHandle qpsk;
} NEXUS_97346FrontendExtensionHandles;

void NEXUS_Frontend_GetDefault97346ExtensionSettings(
    NEXUS_97346FrontendExtensionSettings *pSettings    /* [out] */
    );

NEXUS_Error NEXUS_Frontend_Create97346Extension(
    const NEXUS_97346FrontendExtensionSettings *pSettings,
    NEXUS_97346FrontendExtensionHandles *pHandles       /* [out] */
    );

#endif /* !defined NEXUS_FRONTEND_EXTENSION_H__ */
