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
* $brcm_Workfile: nexus_core_utils.h $
* $brcm_Revision: 13 $
* $brcm_Date: 10/19/12 5:31p $
*
* API Description:
*   API name: Core Features
*       This file sets up chip-specific definitions for the core module
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/include/nexus_core_utils.h $
* 
* 13   10/19/12 5:31p erickson
* SW7425-3782: allow NEXUS_FrontendConnector to be acquired with Frontend
* 
* 12   10/8/12 1:03p erickson
* SW7425-3782: add NEXUS_FrontendConnector for frontend -> transport
*  connections
* 
* 11   9/4/12 1:11p vsilyaev
* SW7420-1004: Removed NEXUS_GetLastError
* 
* 10   8/2/12 4:51p erickson
* SW7435-276: clarify NEXUS_VideoFormatInfo width and height fields
* 
* 9   7/9/12 11:29a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
* 
* 8   3/4/11 5:46p jtna
* SW7422-221: add NEXUS_VideoFormatInfo.isFullRes3d
* 
* 7   9/16/10 10:21a jtna
* SW7420-1004: added #include "berr.h"
* 
* 6   9/15/10 3:44p jtna
* SW7420-1004: added BERR_StoreError and NEXUS_GetLastError_Os
* 
* 5   2/11/09 10:08a erickson
* PR52007: added NEXUS_LookupFrameRate
*
* 4   6/10/08 3:29p vsilyaev
* PR 43184: Added support for Start/Stop callbacks in linux kernel/user
*  proxy mode
*
* 3   4/8/08 10:40a vsilyaev
* PR 41083: Added bprofile support
*
* 2   2/28/08 9:42p vsilyaev
* PR 40103: Added NEXUS_Stop/StartCallback functions
*
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/1   12/28/07 5:05p erickson
* PR38470: add NEXUS_VideoFormat_GetInfo to public API in
* nexus_core_utils.h
*
***************************************************************************/
#ifndef NEXUS_CORE_UTILS_H__
#define NEXUS_CORE_UTILS_H__

#include "nexus_types.h"
#include "berr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Information about a NEXUS_VideoFormat.

Description:
See NEXUS_VideoFormat_GetInfo
**/
typedef struct NEXUS_VideoFormatInfo
{
    uint32_t  width;                /* width in pixels */
    uint32_t  height;               /* height in pixels */
    uint32_t  digitalWidth;         /* digital width (in practice, always same as width) */
    uint32_t  digitalHeight;        /* digital height (in practice, same as height until this is an analog format of height 482 or 483) */
    uint32_t  scanWidth;            /* rasterization width */
    uint32_t  scanHeight;           /* rasterization height */
    uint32_t  topActive;            /* the start active video line of the top field or frame */
    uint32_t  topMaxVbiPassThru;    /* Maximum VBI Pass Through lines at the top field or frame */
    uint32_t  bottomMaxVbiPassThru; /* Maximum VBI Pass Through lines at the bottom field */
    uint32_t  verticalFreq;         /* picture vertical frequency, or refresh rate in units of 1/100th Hz.  For example 60.00Hz would be
                                       6000, and 59.94Hz would be 5994. */
    uint32_t  pixelFreq;            /* Pixel frequencies in units of 1/100th Mhz.  For example 74.24Mhz would be 7425, and
                                       148.50Mhz would be 14850. */
    bool      interlaced;           /* interlaced or progressive mode */
    NEXUS_AspectRatio aspectRatio;  /* Default Aspect Ratio associated with this format. */
    bool      isFullRes3d;          /* full-resolution 3D format or 2D format */
} NEXUS_VideoFormatInfo;

/**
Summary:
Get information about a NEXUS_VideoFormat.
**/
void NEXUS_VideoFormat_GetInfo(
    NEXUS_VideoFormat videoFmt,
    NEXUS_VideoFormatInfo *pVideoFmtInfo    /* [out] */
    );

/**
Summary:
Test if NEXUS_TransportType is a DSS type.

Description:
DSS is a 130-byte packet transport format.
**/
#define NEXUS_IS_DSS_MODE(TRANSPORTTYPE) \
    ((TRANSPORTTYPE) == NEXUS_TransportType_eDssEs || (TRANSPORTTYPE) == NEXUS_TransportType_eDssPes)


/**
Summary:
Stop all callbacks associated with this interface handle.
Wait for any running callbacks to finish, and ensure that no other callbacks for this handle are fired.
**/
void NEXUS_StopCallbacks( /* attr{local=true} */
    void *interfaceHandle
    );

/**
Summary:
Re-enable all callbacks for this interface handle.
All interfaces default to being already started.
If you call StopCallbacks followed by StartCallbacks, no pending callbacks will be lost. <<maybe>>
**/
void NEXUS_StartCallbacks( /* attr{local=true} */
    void *interfaceHandle
    );

/**
Summary:
Look up Nexus framerate enum

Description:
frameRateInteger is in units of 1000 FPS. For example:
    59940 => NEXUS_VideoFrameRate_e59_94
    23976 => NEXUS_VideoFrameRate_e23_976
    60000 => NEXUS_VideoFrameRate_e60

pNexusFrameRate will be NEXUS_VideoFrameRate_eUnknown if there is no match.
**/
void NEXUS_LookupFrameRate(
    unsigned frameRateInteger, /* framerate in units of 1000 fps. */
    NEXUS_VideoFrameRate *pNexusFrameRate /* [out] */
    );

/**
Summary:
Handle obtained from NEXUS_Frontend_GetConnector to make connections to other modules, like transport.

Description:
See NEXUS_ParserBandSettings.sourceTypeSettings.mtsif and NEXUS_TsmfSettings.sourceTypeSettings.mtsif.
**/
NEXUS_FrontendConnectorHandle NEXUS_FrontendConnector_Create( /* attr{destructor=NEXUS_FrontendConnector_Destroy} */
    void);

NEXUS_FrontendConnectorHandle NEXUS_FrontendConnector_Acquire( /* attr{release=NEXUS_FrontendConnector_Release} */
    unsigned index
    );
    
void NEXUS_FrontendConnector_Release(
    NEXUS_FrontendConnectorHandle handle
    );
    
/**
Summary:
**/
void NEXUS_FrontendConnector_Destroy(
    NEXUS_FrontendConnectorHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_CORE_UTILS_H__ */

