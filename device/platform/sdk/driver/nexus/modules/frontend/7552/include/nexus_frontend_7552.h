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
* $brcm_Workfile: nexus_frontend_7552.h $
* $brcm_Revision: 7 $
* $brcm_Date: 1/11/12 5:32p $
*
* API Description:
*   API name: Frontend 7552
*    APIs to open, close, and setup initial settings for a BCM7552
*    Demodulator Device.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/7552/include/nexus_frontend_7552.h $
* 
* 7   1/11/12 5:32p mphillip
* SW7552-147: Merge changes to main
* 
* NEXUS_AOB_7552/3   1/11/12 5:26p vishk
* SW7552-147: 7552B0: Add Nexus OOB support undo #if 0
* 
* NEXUS_AOB_7552/2   1/11/12 5:05p vishk
* SW7552-147: 7552B0: Add Nexus OOB support
* 
* 6   8/29/11 6:41p bandrews
* SW7552-112: add changes for loopthru and rf input selection; pre-CR
*  check-in so development can continue
* 
* 5   6/8/11 2:03p jtna
* SW7552-35: update
* 
* 5   6/8/11 2:01p jtna
* SW7552-35: update
* 
* 4   5/31/11 5:01p jtna
* SW7552-35: update
* 
* 3   5/31/11 3:02p jtna
* SW7552-35: update
* 
* 2   5/27/11 8:16p jtna
* SW7552-35: update
* 
* 1   5/26/11 7:17p jtna
* SW7552-35: initial branch-out
* 
***************************************************************************/
#ifndef NEXUS_FRONTEND_7552_H__
#define NEXUS_FRONTEND_7552_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nexus_frontend.h"

/**
7552 has a single tuner for OFDM and QAM, but separate demods that connect to different input bands. 
The app or platform code should open one OFDM handle and one QAM handle, but can only use one at a time. */
typedef enum NEXUS_7552ChannelType
{
    NEXUS_7552ChannelType_eOfdm,
    NEXUS_7552ChannelType_eQam,
    NEXUS_7552ChannelType_eOutOfBand,
    NEXUS_7552ChannelType_eMax
} NEXUS_7552ChannelType;

/***************************************************************************
Summary:
Settings for a BCM7552 OFDM Device
***************************************************************************/
typedef struct NEXUS_7552FrontendSettings
{
    NEXUS_7552ChannelType type;
    bool supportIsdbt;      /* allocate memory for ISDB-T if true, set at open-time only */
} NEXUS_7552FrontendSettings;

/***************************************************************************
Summary:
    Get the default settings for a BCM7552 OFDM demodulator
***************************************************************************/
void NEXUS_Frontend_GetDefault7552Settings(
    NEXUS_7552FrontendSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Open a handle to a BCM7552 OFDM frontend device.
***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open7552(  /* attr{destructor=NEXUS_Frontend_Close} */
    const NEXUS_7552FrontendSettings *pSettings
    );

#define NEXUS_7552QamFrontendSettings NEXUS_7552FrontendSettings
#define NEXUS_7552OfdmFrontendSettings NEXUS_7552FrontendSettings
#define NEXUS_Frontend_GetDefault7552QamSettings NEXUS_Frontend_GetDefault7552Settings
#define NEXUS_Frontend_GetDefault7552OfdmSettings NEXUS_Frontend_GetDefault7552Settings
#define NEXUS_Frontend_Open7552Qam NEXUS_Frontend_Open7552
#define NEXUS_Frontend_Open7552Ofdm NEXUS_Frontend_Open7552

/***************************************************************************
Summary:
Power mode configuration of the 7552's wide band tuner.
***************************************************************************/
typedef enum NEXUS_7552TunerRfInput
{
    NEXUS_7552TunerRfInput_eOff,         /* Tuner is off. */
    NEXUS_7552TunerRfInput_eExternalLna, /* Tuner Rf input through UHF path. This Rf path does not use internal LNA. */
    NEXUS_7552TunerRfInput_eInternalLna, /* Tuner Rf input through UHF path. This Rf path does uses internal LNA. */
    NEXUS_7552TunerRfInput_eStandardIf,  /* Tuner input is not RF. Insted a (36 or 44MHz) standard IF signal is input.  */
    NEXUS_7552TunerRfInput_eLowIf,       /* Tuner input is not RF. Insted a (4 to 5MHz) Low IF signal is input.  */
    NEXUS_7552TunerRfInput_eBaseband,    /* Tuner input is baseband.  */
    NEXUS_7552TunerRfInput_eMax
} NEXUS_7552TunerRfInput;

/***************************************************************************
Summary:
Config settings for a BCM7552 Device Channel
***************************************************************************/
typedef struct NEXUS_7552ConfigSettings
{
    NEXUS_7552TunerRfInput rfInput;        /* Determines how Rf is input to the tuner. */
    bool enableRfLoopThrough;              /* True = Enables RF loop through. */
} NEXUS_7552ConfigSettings;

/***************************************************************************
Summary:
Get the default config settings to a BCM7552 device.
***************************************************************************/
void NEXUS_Frontend_GetDefault7552ConfigSettings(
    NEXUS_7552ConfigSettings *pConfigSettings   /* [out] */
    );

/***************************************************************************
Summary:
Get the config settings to a BCM7552 device.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_7552_GetConfigSettings(
    NEXUS_FrontendHandle handle,                 
    NEXUS_7552ConfigSettings *pConfigSettings    /* [out] */
    );

/***************************************************************************
Summary:
Set the config settings to a BCM7552 device.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_7552_SetConfigSettings(
    NEXUS_FrontendHandle handle,                 
    const NEXUS_7552ConfigSettings *pConfigSettings    
    );  


#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_7552_H__ */

