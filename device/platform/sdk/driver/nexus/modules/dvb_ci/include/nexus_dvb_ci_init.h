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
* $brcm_Workfile: nexus_dvb_ci_init.h $
* $brcm_Revision: 3 $
* $brcm_Date: 7/14/10 3:20p $
*
* API Description:
*   API name: DvbCi
*    Specific APIs related DVB Common Interface
*
* Revision History:
*
* $brcm_Log: /nexus/modules/dvb_ci/3563/include/nexus_dvb_ci_init.h $
* 
* 3   7/14/10 3:20p erickson
* SW7405-4621: change to nexus_types.h
* 
* 2   4/17/09 5:01p jgarrett
* PR 50410: Adding 97405 DVB-CI support
* 
* 1   2/19/08 3:32p jgarrett
* PR 39610: Adding DvbCi
*
***************************************************************************/

#include "nexus_types.h"

#ifndef NEXUS_DVB_CI_INIT_H__
#define NEXUS_DVB_CI_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    Module settings for DVB-CI
***************************************************************************/
typedef struct NEXUS_DvbCiModuleSettings
{
    struct
    {
        NEXUS_ModuleHandle gpio;
        NEXUS_ModuleHandle i2c;     /* Optional.  Only required if an I2C FPGA/CPLD is used. */
    } modules;
} NEXUS_DvbCiModuleSettings;

/***************************************************************************
Summary:
    Get default DVB-CI module settings
***************************************************************************/
void NEXUS_DvbCiModule_GetDefaultSettings(
    NEXUS_DvbCiModuleSettings *pSettings
    );

/***************************************************************************
Summary:
    Initialize the DVB-CI module
***************************************************************************/
NEXUS_ModuleHandle NEXUS_DvbCiModule_Init(
    const NEXUS_DvbCiModuleSettings *pSettings
    );

/***************************************************************************
Summary:
    Uninitialize the DVB-CI module
***************************************************************************/
void NEXUS_DvbCiModule_Uninit(
    void
    );

#ifdef __cplusplus
}
#endif


#endif /* #ifndef NEXUS_DVB_CI_INIT_H__ */

