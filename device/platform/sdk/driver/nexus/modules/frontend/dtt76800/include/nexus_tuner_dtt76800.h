/***************************************************************************
*     (c)2004-2008 Broadcom Corporation
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
* $brcm_Workfile: nexus_tuner_dtt76800.h $
* $brcm_Revision: 1 $
* $brcm_Date: 5/1/08 11:27a $
*
* API Description:
*   API name: Frontend Dtt76800
*    APIs to open, close, and setup initial settings for a Thompson DTT76800
*    Tuner Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/dtt76800/include/nexus_tuner_dtt76800.h $
* 
* 1   5/1/08 11:27a jgarrett
* PR 41689: Adding dtt76800 support
* 
* 3   2/7/08 5:36p jgarrett
* PR 39407: Converting to common close functions
* 
* 2   2/7/08 1:04p vsilyaev
* PR 38682: Added attributes for the linux user/kernel proxy environment
* 
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/1   10/4/07 2:41p jgarrett
* PR 35551: Adding initial version
* 
***************************************************************************/
#ifndef NEXUS_TUNER_DTT76800_H__
#define NEXUS_TUNER_DTT76800_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nexus_frontend.h"
#include "nexus_i2c.h"

/***************************************************************************
Summary:
    Settings for a DTT76800 Device
 ***************************************************************************/
typedef struct NEXUS_Dtt76800Settings
{
    NEXUS_I2cHandle i2cDevice;      /* I2C device to use */
    uint16_t i2cAddr;               /* Device I2C Address */
    unsigned long ifFrequency;		/* in Hertz */
} NEXUS_Dtt76800Settings;

/***************************************************************************
Summary:
	Get the default settings for a Dtt76800 tuner
 ***************************************************************************/
void NEXUS_Tuner_GetDefaultDtt76800Settings(
    NEXUS_Dtt76800Settings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
	Open a handle to a Dtt76800 device.
 ***************************************************************************/
NEXUS_TunerHandle NEXUS_Tuner_OpenDtt76800( /* attr{destructor=NEXUS_Tuner_Close} */
    const NEXUS_Dtt76800Settings *pSettings
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_TUNER_DTT76800_H__ */

