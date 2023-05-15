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
* $brcm_Workfile: nexus_frontend_ast.h $
* $brcm_Revision: 23 $
* $brcm_Date: 9/11/12 2:24p $
*
* API Description:
*   API name: Frontend AST
*    Generic routines to control any AST satellite frontend
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/src/nexus_frontend_ast.h $
* 
* 23   9/11/12 2:24p mphillip
* SWSATFE-214: Update nexus 73xx frontend code to accommodate the
*  reorganization
* 
* 22   7/17/12 5:03p mphillip
* SW7346-702: Restore internal LNA after powerup
* 
* 21   6/21/12 5:26p mphillip
* SW7358-304: Merge tone search to main
* 
* SW7358-304/2   6/21/12 5:13p mphillip
* SW7358-304: Place tone search min ratio numerator/denominator in a
*  minRatio struct in tone search settings
* 
* SW7358-304/1   6/20/12 6:32p mphillip
* SW7358-304: Implement tone search
* 
* 20   2/9/12 9:59a erickson
* SW7425-1786: verify chip id before typecasting to NEXUS_AstDevice
* 
* 19   6/27/11 10:54a vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent. Add support to include both 4501 and 4506 at the
*  same time.
* 
* 18   6/23/11 7:49p vishk
* SW7422-272: Add nexus gpio support for 4506 tuners
* 
* 17   6/23/11 3:09p vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent
* 
* 16   4/28/11 9:23a randyjew
* SW7344-47: For 7344/7346 we should not be using TEST_NETWORK_TUNER
*  code.
* 
* 15   1/19/11 3:47p xhuang
* SW7358-24: Add 7358 support
* 
* 14   12/8/10 9:48a jrubio
* SW7344-9: add G3 Support
* 
* 13   9/1/10 4:11p erickson
* SW7325-800: add NEXUS_FrontendCapabilities.diseqc and use for internal
*  configuration as wel
* 
* 12   4/16/10 10:24a erickson
* SW3556-1092: added frontend master I2C (MI2c) interface for 4501/4506
*
* 11   9/23/09 1:27p erickson
* SW7335-561: create FTM task callback per AST core
*
* 10   9/16/09 12:08p erickson
* SW7405-2980: support more variables in NEXUS_Frontend_SetDiseqcSettings
*  on 4501/4506
*
* 9   8/20/09 4:48p erickson
* PR57770: convert FTM extension into more generic customer extension
*
* 8   7/31/09 3:40p erickson
* PR57239: move NEXUS_Frontend_P_GetAstDevice and related functions into
*  nexus_frontend_ast.c
*
* 7   7/30/09 3:59p jtna
* PR42924: remove unneeded code
*
* 6   7/21/09 9:01a erickson
* PR56765: add ftm state and uninit
*
* 5   6/30/09 6:54p jtna
* PR56368: support NEXUS_MAX_FRONTENDS < num AST channels
*
* 4   6/9/09 2:25p erickson
* PR55699: move NEXUS_AstDevice definition to private header file to
*  enable AST extension
*
* 3   1/8/09 8:21p agin
* PR50937: Support BCM97335 V10 board.
*
* 2   5/22/08 5:55p jrubio
* PR42593: add multiple tuner support for 7325/7335 -external tuner does
*  not control the diseqc
*
* 1   3/13/08 7:42p jgarrett
* PR 39985: Adding AST support
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
#ifndef NEXUS_FRONTEND_AST_H__
#define NEXUS_FRONTEND_AST_H__

#include "nexus_frontend_module.h"
#include "bast.h"

#if BCHP_CHIP==7346
#include "bast_g3.h"
#endif

#ifdef NEXUS_FRONTEND_73XX
#if BCHP_CHIP==7344 || BCHP_CHIP==7358
#include "bast_g3.h"
#else
#include "bast_g2.h"
#endif
#if BCHP_CHIP!=7344 && BCHP_CHIP!=7346 
#define TEST_NETWORK_TUNER 1
#endif
#endif

#if defined NEXUS_FRONTEND_4506
#include "bast_4506.h"
#endif
#if defined NEXUS_FRONTEND_4501
#include "bast_4501.h"
#endif


/* AST requires that we open all channels, but NEXUS_MAX_FRONTENDS may be
   less than the total number of channels. This handles the mismatch */
#if BCHP_CHIP==7325
#define NEXUS_P_MAX_AST_CHANNELS 2
#elif BCHP_CHIP==7335
#define NEXUS_P_MAX_AST_CHANNELS 3
#else
#define NEXUS_P_MAX_AST_CHANNELS NEXUS_MAX_FRONTENDS
#endif

/***************************************************************************
Summary:
    AST Devices
***************************************************************************/
typedef struct NEXUS_FrontendAstDevices
{
    NEXUS_TunerHandle tuner;    /* Optional tuner -- used on 97335 and 97325 boards */
} NEXUS_FrontendAstDevices;

/***************************************************************************
Summary:
    AST Channel Settings
***************************************************************************/
typedef struct NEXUS_FrontendAstSettings
{
    BAST_Handle astHandle;
    BAST_ChannelHandle astChannel;
    unsigned astChip; /* unused */
    NEXUS_FrontendAstDevices devices;
    unsigned channelIndex;
    unsigned boardRev;
    void (*restoreInternalLnaFunction)(NEXUS_FrontendHandle handle, void *pParam);
    void *pRestoreParam;
    void (*closeFunction)(NEXUS_FrontendHandle handle, void *pParam);   /* Called after handle is closed */
    void *pCloseParam;
    NEXUS_FrontendCapabilities capabilities;
} NEXUS_FrontendAstSettings;

/***************************************************************************
Summary:
    Get Default AST Channel Settings
***************************************************************************/
void NEXUS_Frontend_P_Ast_GetDefaultSettings(
    NEXUS_FrontendAstSettings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
    Create an AST frontend handle
***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_P_Ast_Create(
    const NEXUS_FrontendAstSettings *pSettings
    );

/***************************************************************************
Summary:
    Set devices for an AST channel.
Description:
    May be needed if devices are connected to an on-chip I2C master to
    avoid a chicken and egg problem connecting devices.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_P_Ast_SetDevices(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendAstDevices *pDevices
    );

/***************************************************************************
Summary:
    Get the AST channel handle from a frontend handle
***************************************************************************/
BAST_ChannelHandle NEXUS_Frontend_P_Ast_GetChannel(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
    Internal structure for peakscan
***************************************************************************/
typedef struct NEXUS_SatellitePeakscanStatus {
    unsigned symRateCount;  /* consecutive scans with same symbol rate */
    uint32_t lastSymRate;   /* symbol rate from last scan */
    uint32_t maxPeakPower;  /* maximum of peakPower from scans matching the peak scan parameters */
    uint32_t maxPeakFreq;   /* tuner frequency corresponding to maxPeakPower */
    uint32_t minPeakPower;  /* minimum of peakPower from scans matching the peak scan parameters (only used for tone search) */
    uint32_t maxPeakIndex;  /* index of maximum peakPower (only used for tone search) */

    struct {
        unsigned numerator;
        unsigned denominator;
    } minRatio;

    /* for-loop book-keeping */
    uint32_t curFreq;
    uint32_t endFreq;
    uint32_t stepFreq;
    bool singleScan;
    int32_t binsize;

    bool scanFinished;
} NEXUS_SatellitePeakscanStatus;

BDBG_OBJECT_ID_DECLARE(NEXUS_AstDevice);

/***************************************************************************
Summary:
Internal structure for AST channels
Be aware that each NEXUS_AstDevice corresponds to one BAST_ChannelHandle, not one BAST_Handle.
There is a one-to-one mapping of astChannel to frontendHandle.
There is a one-to-many mapping of astHandle to astChannel.
***************************************************************************/
typedef struct NEXUS_AstDevice
{
    BDBG_OBJECT(NEXUS_AstDevice)

    NEXUS_FrontendAstSettings settings;

    BAST_Handle astHandle; /* copied for each BAST_ChannelHandle */
    BAST_ChannelHandle astChannel;
    NEXUS_I2cHandle deviceI2cHandle; /* For a possible master I2C controller */
    NEXUS_FrontendHandle frontendHandle;

    NEXUS_FrontendSatelliteSettings lastSettings;   /* Last tune settings, returned in status */
    NEXUS_FrontendDiseqcSettings diseqcSettings;

    BKNI_EventHandle lockEvent;
    NEXUS_EventCallbackHandle lockEventCallback;
    NEXUS_TaskCallbackHandle lockAppCallback;

    BKNI_EventHandle diseqcEvent;
    NEXUS_EventCallbackHandle diseqcEventCallback;
    NEXUS_TaskCallbackHandle diseqcAppCallback;

    /* This group of variables are used to do blind scans and tone searches via BAST Peak Scan */
    bool toneSearch;
    BKNI_EventHandle peakscanEvent;
    NEXUS_EventCallbackHandle peakscanEventCallback;
    NEXUS_TaskCallbackHandle peakscanAppCallback;
    NEXUS_SatellitePeakscanStatus peakscanStatus;

    NEXUS_TaskCallbackHandle ftmCallback; /* optionally set by customer extension. must be created here because it's
                                             based on a BAST_Handle event. */

#ifdef NEXUS_FRONTEND_CUSTOMER_EXTENSION
    void *customerData;
#endif

    NEXUS_FrontendCapabilities capabilities; /* copy of capabilites for use in nexus_frontend_ast.c */
    unsigned astChip;
} NEXUS_AstDevice;

/* only use this your chip's .c file. you must #define B_AST_CHIP in that .c file (never in a .h file). 
this will verify that the handle is correct before doing a potentially dangerous typecast. */
#define NEXUS_Frontend_P_GetAstDevice(frontendHandle) NEXUS_Frontend_P_GetAstDeviceByChip(frontendHandle, B_AST_CHIP)

NEXUS_AstDevice *NEXUS_Frontend_P_GetAstDeviceByChip( NEXUS_FrontendHandle handle, unsigned chipId );
    
#endif /* #ifndef NEXUS_FRONTEND_AST_H__ */

