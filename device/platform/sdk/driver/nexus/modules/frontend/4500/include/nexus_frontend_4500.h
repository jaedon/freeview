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
* $brcm_Workfile: nexus_frontend_4500.h $
* $brcm_Revision: 3 $
* $brcm_Date: 2/26/10 4:04p $
*
* API Description:
*   API name: Frontend 4500
*    APIs for a BCM4500 Satellite Demodulator Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/4500/include/nexus_frontend_4500.h $
* 
* 3   2/26/10 4:04p jgarrett
* SW7400-2673: Adding diseqc settings
* 
* SW7400-2673/1   2/24/10 3:32p mward
* SW7400-2673: Adding Nexus support for BCM4500.
* 
* 2   2/7/08 5:34p jgarrett
* PR 39407: Converting to common close functions
* 
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/1   10/4/07 2:41p jgarrett
* PR 35551: Adding initial version
* 
***************************************************************************/
#ifndef NEXUS_FRONTEND_4500_H__
#define NEXUS_FRONTEND_4500_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nexus_frontend.h"
#include "nexus_i2c.h"

/***************************************************************************
Summary:
    Settings for a BCM4500 Device
 ***************************************************************************/
typedef struct NEXUS_4500Settings
{
    /* The following parameters identify the device to be used. */
    NEXUS_I2cHandle i2cDevice;      /* I2C device to use */
    uint16_t i2cAddr;               /* Device I2C Address */
    uint16_t diseqcI2cAddr;         /* DiSEqC device I2C Address */
    uint16_t diseqcChip;            /* Use 6425 for ISL6425 on 97400 board, 
                                       0 for an externally controlled voltage regulator */
    unsigned isrNumber;             /* L1 interrupt number (typically 0..63)*/

    bool bitWideSync;
} NEXUS_4500Settings;

/***************************************************************************
Summary:
	Get the default settings for a BCM4500 tuner
See Also:
    NEXUS_Frontend_Open4500
 ***************************************************************************/
void NEXUS_Frontend_GetDefault4500Settings(
    NEXUS_4500Settings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
	Open a handle to a BCM4500 device.
See Also:
    NEXUS_Frontend_Close4500
 ***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open4500(  /* attr{destructor=NEXUS_Frontend_Close} */
    const NEXUS_4500Settings *pSettings
    );

/***************************************************************************
Summary:
	Results of BCM4500 device discovery
 ***************************************************************************/
typedef struct NEXUS_4500ProbeResults
{
    uint8_t revision;
} NEXUS_4500ProbeResults;

/***************************************************************************
Summary:
	Probe to see if a BCM4500 device exists with the specified settings
See Also:
    NEXUS_Frontend_Open4500
 ***************************************************************************/
NEXUS_Error NEXUS_Frontend_Probe4500(
    const NEXUS_4500Settings *pSettings,
    NEXUS_4500ProbeResults *pResults        /* [out] */
    );

NEXUS_Error NEXUS_Frontend_Read4500HostRegister(
    NEXUS_FrontendHandle handle,
    uint8_t address,
    uint32_t *pData
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_4500_H__ */

