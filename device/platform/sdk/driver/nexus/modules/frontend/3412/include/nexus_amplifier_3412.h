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
* $brcm_Workfile: nexus_amplifier_3412.h $
* $brcm_Revision: 1 $
* $brcm_Date: 11/16/10 3:51p $
*
* API Description:
*   API name: Frontend 3412
*    APIs to open, close, and setup initial settings for a BCM3412 
*    Low-Noise Amplifier Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/3412/include/nexus_amplifier_3412.h $
* 
* 1   11/16/10 3:51p erickson
* SW7125-738: add 3412
* 
* SW7125-738/1   11/15/10 3:34p mward
* SW7125-729: Add 3412 (which also handles 3410).
* 
***************************************************************************/
#ifndef NEXUS_AMPLIFIER_3412_H__
#define NEXUS_AMPLIFIER_3412_H__

#include "nexus_frontend.h"
#include "nexus_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 Summary:
    Settings for a BCM3412 Device
 ***************************************************************************/
typedef struct NEXUS_3412Settings
{
    NEXUS_I2cHandle i2cDevice;  /* I2C device to use */
    uint16_t i2cAddr;           /* Device I2C Address */
    bool skipInit;              /* skip initialization that might disrupt ongoing reception */
} NEXUS_3412Settings;

/***************************************************************************
Summary:
    Get the default settings for a BCM3412 amplifier
 ***************************************************************************/
void NEXUS_Amplifier_GetDefault3412Settings(
    NEXUS_3412Settings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Open a handle to a BCM 3412 device.
 ***************************************************************************/
NEXUS_AmplifierHandle NEXUS_Amplifier_Open3412( /* attr{destructor=NEXUS_Amplifier_Close} */
    const NEXUS_3412Settings *pSettings
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AMPLIFIER_3412_H__ */

