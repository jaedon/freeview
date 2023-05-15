/***************************************************************************
*     (c)2004-2013 Broadcom Corporation
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
* $brcm_Workfile: nexus_frontend.h $
* $brcm_Revision: PROD_HUMAX_URSR/1 $
* $brcm_Date: 8/30/13 6:50p $
*
* API Description:
*   API name: Frontend
*    Generic Frontend APIs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/include/nexus_frontend.h $
*
* PROD_HUMAX_URSR/1   8/30/13 6:50p scho
* PROD_HUMAX_URSR: Make NEXUS_Platform_Init() asynchronous. Make
*  FrontendDeviceOpen asynchrnous. Use NEXUS_FrontendDevice_GetStatus()
*  to determine if the frontend device is successfully opened or if it
*  failed. Also implement parallel download of firmware to frontend
*  devices controlled by seperate I2C controllers. The same feature also
*  implemented for resume from S3 standby.
* 
* 49   10/19/12 11:20a jtna
* SW7425-4037: add NEXUS_Frontend_TransmitDebugPacket
*
* 48   10/18/12 3:41p erickson
* SW3461-206: add NEXUS_FrontendDevice api
*
* 47   10/18/12 3:37p erickson
* SW3461-270: add more version and bondout info
*
* 46   10/8/12 2:52p erickson
* SW7435-372: add NEXUS_Frontend_ReadSoftDecisions
*
* 45   10/8/12 1:03p erickson
* SW7425-3782: add NEXUS_FrontendConnector for frontend -> transport
*  connections
*
* 44   10/3/12 5:00p jtna
* SW7425-3782: refactor nexus tsmf impl
*
* 43   9/5/12 6:08p jtna
* SW7425-3782: rework host and demod TSMF impl. both cases now go through
*  the host API
*
* 42   8/7/12 3:08p jtna
* SW7231-924: Add oob and ifd capabilities support for
*  NEXUS_Frontend_Acquire() and NEXUS_FrontendAcquireSettings
*
* 41   7/26/12 3:08p jtna
* SW7425-3514: add frontend GetTemperature APIs
*
* 40   7/5/12 12:03p erickson
* SW3462-12: add DVB-C2 API
*
* 39   6/26/12 3:34p jtna
* SW7425-3062: add feature to propagate host parserband settings to
*  demod. deprecate NEXUS_Frontend_Get/SetTransportSettings
*
* 38   6/21/12 2:35p erickson
* SW7420-2078: add NEXUS_Frontend_OpenStub
*
* 37   6/21/12 2:01p erickson
* SW7420-2078: refactor NEXUS_FrontendAcquireSettings using mode enum
*
* 36   6/20/12 4:22p erickson
* SW7420-2078: add NEXUS_Frontend_Acquire/Release
*
* 35   6/8/12 3:27p jtna
* SW7425-3171: add NEXUS_Frontend_ReapplyTransportSettings
*
* 34   5/29/12 11:15a mphillip
* SW7344-327: Clarify comments for NEXUS_Frontend_Untune
*
* 33   5/15/12 2:48p erickson
* SW7552-269, SW3128-155, SW3461-198: add NEXUS_Frontend_ReadRegister and
*  NEXUS_Frontend_WriteRegister
*
* 32   5/10/12 6:17p jtna
* SW7425-2752: rework MTSIF-frontend <-> host interface
*
* 31   3/30/12 5:44p jtna
* SW3128-117: add comment about using NEXUS_Memory_Allocate for members
*  with attr{memory=cached}
*
* 30   2/3/12 3:32p erickson
* SW7552-198, SW7552-201: merge
*
* SCAN_FIX/1   2/2/12 6:43p vishk
* SW7552-198, SW7552-201: Lock/unlock callbacks fired are queued and are
*  fired back to back, hence making the sequence of event occurrence non-
*  deterministic. Scan for 7552 is broken.
*
* 29   1/25/12 3:03p erickson
* SW3461-51: add partial DVBT2 status
*
* 3461_PARTIAL_STATUS/1   1/23/12 4:08p vishk
* SW3461-51: Implement asynchronous nexus apis for retrieval of partial
*  DVBT2 status.
*
* 28   1/18/12 11:36a erickson
* SW3128-99: add NEXUS_Frontend_RequestSpectrumData
*
* 27   1/12/12 11:51a jtna
* SW3128-84: merge NEXUS_FrontendTransportSettings callbacks
*
* 26   11/17/11 4:33p erickson
* SW3461-96: Remove GetLockStatus and replace it with GetFastStatus
*
* 25   11/1/11 3:07p erickson
* SW3461-55: add NEXUS_Frontend_GetType, NEXUS_FrontendType,
*  NEXUS_FrontendChipType, and NEXUS_FrontendVersion
*
* 24   10/13/11 4:28p erickson
* SW3461-55: add comment to deprecate/freeze NEXUS_FrontendUserParameters
*
* 23   10/6/11 5:21p jtna
* SW7425-1327: add NEXUS_Frontend_Get/SetTransportSettings()
*
* 22   10/6/11 9:53a jtna
* SW7425-1189: change NEXUS_Frontend_GetTsmfSettings() to return void
*
* 21   10/5/11 12:23p jtna
* SW7425-1189: add transport and frontend TSMF support
*
* 20   9/21/11 5:25p bandrews
* SW3461-53: add common GetLockStatus
*
* 19   8/30/11 4:17p erickson
* SW7425-1129: add NEXUS_FrontendCapabilities.docsis
*
* 18   1/26/11 11:08a jgarrett
* SW7125-769: Merge to main branch
*
* SW7125-769/1   1/24/11 10:59a mward
* SW7125-769:  Add id, chipId, and chipRevision to
*  NEXUS_FrontendUserParameters.
*
* 17   1/6/11 5:20p jhaberf
* SW35230-2441: Moved channel scan API to platform specific location
*
* 16   9/10/10 4:21p erickson
* SW35230-1306: deprecate NEXUS_FrontendBandwidth. use unsigned integer
*  in units of Hz instead.
*
* 15   9/1/10 4:15p erickson
* SW7325-800: add NEXUS_FrontendCapabilities.diseqc and use for internal
*  configuration as well
*
* 14   9/1/10 3:48p jhaberf
* SW35230-1024: Merged SW35230-1024 branch into main line
*
* SW35230-1020/4   9/1/10 6:03p shyi
* SW35230-857: Adding ifFrequency to NEXUS_FrontendScanSettings to allow
*  applications to specify dynamically changing IF
*
* SW35230-1020/3   8/30/10 6:29p shyi
* SW35230-1020: Adding flexibility of specifying bandwidths to search
*
* SW35230-1020/2   8/26/10 12:01p shyi
* SW35230-1020: Added frequency scanning for DVB-T signal
*
* SW35230-1020/1   8/26/10 11:19a shyi
* SW35230-1020: Porting changes made under SW35230-900 to quickly prepare
*  release
*
* 12   7/21/09 10:00a erickson
* PR56958: fix NEXUS_Frontend_SetUserParameters, add comments to
*  NEXUS_Frontend_ScanFrequency
*
* 11   7/14/09 3:47p erickson
* PR56789: remove NEXUS_Frontend_SetPowerState. unimplemented and unused.
*
* 10   7/6/09 4:50p erickson
* PR56607: clarify comment on callbacks after NEXUS_Frontend_Untune
*
* 9   2/25/09 4:39p jgarrett
* PR 51839: Fix for kernel mode
*
* 8   8/5/08 11:25a erickson
* PR45427: add NEXUS_Frontend_Untune
*
* 7   3/14/08 12:55p jgarrett
* PR 40108: Adding NEXUS_Frontend_ScanFrequency
*
* 6   3/14/08 11:31a jgarrett
* PR 40109: Adding NEXUS_Frontend_ResetStatus
*
* 5   2/8/08 6:36p jgarrett
* PR 39408: Adding daughtercard support
*
* 4   2/7/08 5:35p jgarrett
* PR 39407: Converting to common close functions
*
* 3   1/23/08 8:37p vobadm
* PR35457: update docs
*
* 2   1/23/08 5:16p erickson
* PR35457: update docs
*
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/6   1/10/08 2:33p jgarrett
* PR 38653: Initial version
*
* Nexus_Devel/5   11/5/07 2:58p erickson
* PR36725: remove Get and GetCount
*
* Nexus_Devel/4   10/10/07 11:29a jgarrett
* PR 35551: Adding IFD
*
* Nexus_Devel/3   10/5/07 5:51p jgarrett
* PR 35551: successful build
*
* Nexus_Devel/2   10/4/07 2:51p jgarrett
* PR 35551: Removing Register and Configuration -- part of device
*
* Nexus_Devel/1   10/4/07 2:41p jgarrett
* PR 35551: Adding initial version
*
***************************************************************************/
#ifndef NEXUS_FRONTEND_H__
#define NEXUS_FRONTEND_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/**
Summary:
Handle for a generic frontend object
**/
typedef struct NEXUS_Frontend *NEXUS_FrontendHandle;
/**
Summary:
FrontendDevice is a single frontend chip which can have one or more channels
**/
typedef struct NEXUS_FrontendDevice *NEXUS_FrontendDeviceHandle;

#include "nexus_frontend_qam.h"
#include "nexus_frontend_vsb.h"
#include "nexus_frontend_satellite.h"
#include "nexus_frontend_ofdm.h"
#include "nexus_frontend_oob.h"
#include "nexus_frontend_upstream.h"
#include "nexus_frontend_analog.h"
#include "nexus_tuner.h"
#include "nexus_amplifier.h"
#include "nexus_ifd.h"
#include "nexus_frontend_card.h"
#include "nexus_frontend_dvb_t2.h"
#include "nexus_frontend_dvb_c2.h"

#include "nexus_i2c.h"
#include "nexus_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Frontend Temperature -- Allows frontend temperature to be queried by the application.
***************************************************************************/
typedef struct NEXUS_FrontendTemperature
{
    unsigned temperature;                           /* units of 1/100 degree celcius */
} NEXUS_FrontendTemperature;


/***************************************************************************
Summary:
Frontend Capabilities -- Allows capabilities of generic frontend objects to be queried by the application.
***************************************************************************/
typedef struct NEXUS_FrontendCapabilities
{
    bool powerDownSupported;                        /* If true, this tuner supports a power-down mode */
    bool analog;                                    /* If true, this tuner can support analog/NTSC tuning */
    bool ifd;                                       /* If true, this tuner can demodulate an analog signal with the IFD */
    bool qam;                                       /* If true, at least one QAM mode is supported */
    bool docsis;                                    /* If true, DOCSIS is supported */
    bool vsb;                                       /* If true, at least one VSB mode is supported */
    bool satellite;                                 /* If true, at least one satellite mode is supported */
    bool outOfBand;                                 /* If true, at least one out of band mode is supported */
    bool upstream;                                  /* If true, at least one upstream mode is supported */
    bool ofdm;                                      /* If true, OFDM modulation is supported */
    bool scan;                                      /* If true, NEXUS_Frontend_ScanFrequency is supported for this frontend */
    bool qamModes[NEXUS_FrontendQamMode_eMax];      /* For each QAM mode, the mode is supported if true and unsupported if false */
    bool vsbModes[NEXUS_FrontendVsbMode_eMax];      /* For each VSB mode, the mode is supported if true and unsupported if false */
    bool satelliteModes[NEXUS_FrontendSatelliteMode_eMax];  /* For each satellite mode, the mode is supported if true and unsupported if false */
    bool outOfBandModes[NEXUS_FrontendOutOfBandMode_eMax];  /* For each out-of-band mode, the mode is supported if true and unsupported if false */
    bool upstreamModes[NEXUS_FrontendUpstreamMode_eMax];    /* For each upstream mode, the mode is supported if true and unsupported if false */
    bool ofdmModes[NEXUS_FrontendOfdmMode_eMax];    /* For each OFDM mode, the mode is supported if true and unsupported if false */
    bool diseqc;
} NEXUS_FrontendCapabilities;

/***************************************************************************
Summary:
    Return a list of capabilities for a given frontend object.

Description:
    This call returns a list of capabilities for a frontend object.  Because
    many frontends support multiple types of modulation schemes, this
    allows the application to distinguish the capabilities of one tuner
    versus another.  If a tuner has multiple capabilities, only one of
    the modes may be used at any one time.
See Also:
    NEXUS_Frontend_Get
***************************************************************************/
void NEXUS_Frontend_GetCapabilities(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendCapabilities *pCapabilities   /* [out] */
    );

/***************************************************************************
Summary:
Frontend Soft Decisions
***************************************************************************/
typedef struct NEXUS_FrontendSoftDecision
{
    int16_t i; /* ranges from 32767 to -32768 for all devices, however precision may vary per device. */
    int16_t q; /* ranges from 32767 to -32768 for all devices, however precision may vary per device. */
} NEXUS_FrontendSoftDecision;

/***************************************************************************
Summary:
    Get a fixed-sized array of soft decisions for a constellation.

Description:
    Deprecated. Use NEXUS_Frontend_ReadSoftDecisions instead.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetSoftDecisions(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSoftDecision *pDecisions, /* attr{nelem=length;reserved=100} array of soft decisions */
    size_t length                           /* number of NEXUS_FrontendSoftDecision's to get */
    );

/***************************************************************************
Summary:
    Read an array of soft decisions for a constellation.

Description:
    Like NEXUS_Frontend_GetSoftDecisions, but will only do a single read and return pNunRead.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_ReadSoftDecisions(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendSoftDecision *pDecisions, /* attr{nelem=length;nelem_out=pNumRead;reserved=100} array of soft decisions */
    size_t length,                          /* maximum number of NEXUS_FrontendSoftDecision's to read */
    size_t *pNumRead                        /* actual number read */
    );

/***************************************************************************
Summary:
Frontend User Parameters -- Optional values that platform may bind to a
                            specific device/channel to pass information
                            to the application (such as input band)

This struct is deprecated. It should not be extended. If additional frontend
information is needed from the platform, the NEXUS_PlatformConfiguration struct should
be extended so the info can be communicated directly, not through a tunnel.
***************************************************************************/
typedef struct NEXUS_FrontendUserParameters
{
    int param1; /* if isMtsif==false, this is the SoC's input band number that the frontend is connected to.
                   if isMtsif==true, it's the demod's input band number. */
    void *pParam2;
    unsigned id; /* platform-assigned id for this frontend instance */
    uint16_t chipId; /* demod chip, in hex, e.g. 0x3117 */
    uint16_t chipRevision; /* demod chip revision, in hex, e.g. 0x0010 = A0, 0x0021 = B1
                              0 = "not needed" or "not available" */
    bool isMtsif; /* if true, uses MTSIF connection between demod and SoC. if false, legacy connection */
} NEXUS_FrontendUserParameters;

/***************************************************************************
Summary:
    Get user parameters provided by NEXUS_Frontend_SetUserParameters.

Description:
    This is an optional call that allows the platform layer to attach some
    user parameters to a frontend object.  The application can, in turn,
    retrieve these parameters to get some data from platform such as
    input band.
See Also:
    NEXUS_Frontend_SetUserParameters
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetUserParameters(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendUserParameters *pParams   /* [out] */
    );

/***************************************************************************
Summary:
    Set user parameters for a frontend.

Description:
    This is an optional call that allows the platform layer to attach some
    user parameters to a frontend object.  The application can, in turn,
    retrieve these parameters to get some data from platform such as
    input band.
See Also:
    NEXUS_Frontend_GetUserParameters
***************************************************************************/
NEXUS_Error NEXUS_Frontend_SetUserParameters(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendUserParameters *pParams
    );


/***************************************************************************
Summary:
Major/minor version code

Examples:
For silicon,
major=0x01 and minor=0x00, denotes A0.
major=0x01 and minor=0x01, denotes A1.
major=0x02 and minor=0x02, denotes B2.

For firmware:
major=0x00 and minor=0x05, denotes 0.5.
major=0x01 and minor=0x01, denotes 1.1.
major=0x02 and minor=0x02, denotes 2.2.
***************************************************************************/
typedef struct NEXUS_FrontendVersion
{
    unsigned major;
    unsigned minor;
    unsigned buildType; /* only for FW */
    unsigned buildId;   /* only for FW */
} NEXUS_FrontendVersion;

#define NEXUS_FRONTEND_MAX_BONDOUT_OPTIONS 2

/***************************************************************************
Summary: Frontend Chip Info.
***************************************************************************/
typedef struct NEXUS_FrontendChipType
{
    unsigned familyId; /* Chip's family Id. In hex, e.g. 0x3128. */
    unsigned id;       /* Chip's Id or Bondout Id. In hex, e.g. 0x3128/ 0x3124/ 0x3123. */
    NEXUS_FrontendVersion version;
    unsigned bondoutOptions[NEXUS_FRONTEND_MAX_BONDOUT_OPTIONS];
} NEXUS_FrontendChipType;

/***************************************************************************
Summary: Frontend Chip and Firmware Info.
***************************************************************************/
typedef struct NEXUS_FrontendType
{
    NEXUS_FrontendChipType chip;
    NEXUS_FrontendVersion firmwareVersion;
} NEXUS_FrontendType;

/***************************************************************************
Summary:
    Retrieve the chip family id, chip id, chip version and firmware version.
***************************************************************************/
void NEXUS_Frontend_GetType(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendType *pType
    );

/***************************************************************************
Summary:
Do not use this function. This is a stub function to create the Nexus
class for systems with frontend module, but no chip-specific open
***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_OpenStub( /* attr{destructor=NEXUS_Frontend_Close} */
    unsigned index
    );

/***************************************************************************
Summary:
    Close a frontend handle
***************************************************************************/
void NEXUS_Frontend_Close(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
    Reset state after a tune

Description:
    After this function is called, no new lock callback will be generated.
    It's possible that a pending lock callback will still be issued. If you cannot process another
    callback after NEXUS_Frontend_Untune, call NEXUS_StopCallbacks before calling NEXUS_Frontend_Untune and
    call NEXUS_StartCallbacks from any subsequent Tune function.

    The demodulator, tuner, and related cores (e.g. diseqc) may be reset or powered down as well.
    Since powering up may cause delays as the cores are restarted, a sequence of tune/acquisition should
    not call NEXUS_Frontend_Untune between tunes for performance reasons.
***************************************************************************/
void NEXUS_Frontend_Untune(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
    Reset accumulated status values for a frontend
***************************************************************************/
void NEXUS_Frontend_ResetStatus(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
Frontend Lock Status
****************************************************************************/
typedef enum NEXUS_FrontendLockStatus
{
    NEXUS_FrontendLockStatus_eUnknown,  /* no status available */
    NEXUS_FrontendLockStatus_eUnlocked, /* not locked, but there is a signal. */
    NEXUS_FrontendLockStatus_eLocked,   /* locked */
    NEXUS_FrontendLockStatus_eNoSignal, /* no signal; therefore not locked */
    NEXUS_FrontendLockStatus_eMax
} NEXUS_FrontendLockStatus;

/***************************************************************************
Summary:
Status returned by NEXUS_Frontend_GetFastStatus
***************************************************************************/
typedef struct NEXUS_FrontendFastStatus {
    NEXUS_FrontendLockStatus lockStatus;
    bool acquireInProgress; /* if true, you can wait for another lockCallback */
} NEXUS_FrontendFastStatus;

/***************************************************************************
Summary:
Get fast status for the frontend

Description:
Frontends may not populate all status.
Usually fast status happens from direct register reads from the SoC,
not FW transactions.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetFastStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendFastStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
Re-apply transport settings to frontend

Description:
MTSIF-based frontends are routed to host parserbands using NEXUS_ParserBandSourceType_eMtsif.
NEXUS_ParserBand_SetSettings must be called prior to calling the tune function.

NEXUS_Frontend_ReapplyTransportSettings re-applies the transport settings to the frontend
after the tune.

This function is called automatically by the tune function.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_ReapplyTransportSettings(
    NEXUS_FrontendHandle handle
    );

#if (defined(CONFIG_DEMOD_TSMF) || (defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)))
/***************************************************************************
Summary:
Get Frontend Parserband Path info

Description:
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetPathinfo(
    NEXUS_FrontendHandle handle
   );
#endif

/***************************************************************************
The NEXUS_FrontendBandwidth enum has been deprecated. It is an unsigned integer in units of Hz now.
The following #defines are for backward compatibility only.
***************************************************************************/
#define NEXUS_FrontendBandwidth_e5Mhz 5000000
#define NEXUS_FrontendBandwidth_e6Mhz 6000000
#define NEXUS_FrontendBandwidth_e7Mhz 7000000
#define NEXUS_FrontendBandwidth_e8Mhz 8000000
#define NEXUS_FrontendBandwidth_eMax  4

/***************************************************************************
Summary:
***************************************************************************/
typedef struct NEXUS_FrontendSpectrumSettings
{
    void *data;              /* attr{memory=cached} ptr to the data. You must allocate this buffer with NEXUS_Memory_Allocate */
    unsigned dataLength;     /* max data length in bytes, must be >= (numSamples*4) */
    unsigned startFrequency; /* in Hz */
    unsigned stopFrequency;  /* in Hz */
    unsigned binAverage;     /* Total number of samples to be averaged per bin. */
    unsigned numSamples;     /* Total number of 32-bit frequency values to return */
    unsigned fftSize;        /* Current supported FFT sizes are 64, 128, 256, 512, 1024 and 2048. */
    NEXUS_CallbackDesc dataReadyCallback;   /* Callback will be called when the spectrum data is available to be read. */
} NEXUS_FrontendSpectrumSettings;

#if defined(HUMAX_PLATFORM_BASE)
/***************************************************************************
Summary:
Re-apply TSMF settings to frontend

Description:
***************************************************************************/
NEXUS_Error NEXUS_Frontend_ReapplyTSMFSettings(
    NEXUS_FrontendHandle handle
    );
#endif

/***************************************************************************
Summary:
***************************************************************************/
void NEXUS_Frontend_GetDefaultSpectrumSettings(
    NEXUS_FrontendSpectrumSettings *pSettings /* [out] */
    );

/***************************************************************************
Summary:
***************************************************************************/
NEXUS_Error NEXUS_Frontend_RequestSpectrumData(
    NEXUS_FrontendHandle handle,
    const NEXUS_FrontendSpectrumSettings *pSettings  /* spectrum settings */
    );

/***************************************************************************
Summary:
Read a frontend register for debug
***************************************************************************/
NEXUS_Error NEXUS_Frontend_WriteRegister(
    NEXUS_FrontendHandle handle,
    unsigned address, /* offset from demod's register base address */
    uint32_t value
    );

/***************************************************************************
Summary:
Write a frontend register for debug

Description:
Be aware that direct register writes can cause serious system failures. This should
only be used for debug and only as directed.
***************************************************************************/
NEXUS_Error NEXUS_Frontend_ReadRegister(
    NEXUS_FrontendHandle handle,
    unsigned address, /* offset from demod's register base address */
    uint32_t *pValue
    );

/***************************************************************************
Summary:
Mode for NEXUS_FrontendAcquireSettings
***************************************************************************/
typedef enum NEXUS_FrontendAcquireMode
{
    NEXUS_FrontendAcquireMode_eByCapabilities,
    NEXUS_FrontendAcquireMode_eByIndex,
    NEXUS_FrontendAcquireMode_eMax
} NEXUS_FrontendAcquireMode;

/***************************************************************************
Summary:
Settings for NEXUS_Frontend_Acquire
***************************************************************************/
typedef struct NEXUS_FrontendAcquireSettings
{
    NEXUS_FrontendAcquireMode mode; /* this determines which of the following fields are used */
    struct {
        bool qam;
        bool vsb;
        bool ofdm;
        bool satellite;
        bool outOfBand;
        bool ifd;
    } capabilities; /* used if mode == eByCapabilities. at least one field must be set true.
        if more than one is true, then the frontend must meet all requested capabilities. */

    unsigned index; /* used if mode == eByIndex. this is a platform-wide index of frontend typically corresponding
        to the index of NEXUS_PlatformConfiguration.frontend[].
        it may differ if the application opens/closes frontends outside of platform. */
} NEXUS_FrontendAcquireSettings;

/***************************************************************************
Summary:
Get default acquire settings
***************************************************************************/
void NEXUS_Frontend_GetDefaultAcquireSettings(
    NEXUS_FrontendAcquireSettings *pSettings
    );

/***************************************************************************
Summary:
Find a frontend based on capabilities and register handle for protected client use
***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Acquire( /* attr{release=NEXUS_Frontend_Release} */
    const NEXUS_FrontendAcquireSettings *pSettings
    );

/***************************************************************************
Summary:
Release an acquired client
***************************************************************************/
void NEXUS_Frontend_Release(
    NEXUS_FrontendHandle handle
    );

/***************************************************************************
Summary:
Returns the frontend die temperature
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetTemperature(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendTemperature *pTemp
    );

/***************************************************************************
Summary:
Frontend debug packet type
***************************************************************************/
typedef enum NEXUS_FrontendDebugPacketType {
    NEXUS_FrontendDebugPacketType_eOob,
    NEXUS_FrontendDebugPacketType_eMax
} NEXUS_FrontendDebugPacketType;

/***************************************************************************
Summary:
Transmit a debug packet
***************************************************************************/
NEXUS_Error NEXUS_Frontend_TransmitDebugPacket(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendDebugPacketType type,
    const uint8_t *pBuffer,
    size_t size
    );

/***************************************************************************
Summary:
    Current BERT status

Description:
    This struct holds the BERT status data, and is used only with a special
pseudo-random bit stream test input.
    If NEXUS_FrontendBertStatus.enabled = false, the frontend was not tuned
with BERT enabled, and the rest of the values are invalid.

See also:
    NEXUS_Frontend_GetBertStatus
***************************************************************************/
typedef struct NEXUS_FrontendBertStatus {
    bool enabled;
    bool locked;          /* current BERT lock status */
    bool syncAcquired;    /* BERT acquired sync since last NEXUS_Frontend_GetBertStatus() or NEXUS_Frontend_ResetStatus() */
    bool syncLost;        /* BERT lost sync since last NEXUS_Frontend_GetBertStatus() or NEXUS_Frontend_ResetStatus() */
    uint64_t bitCount;    /* Cumulative bit error count since last NEXUS_Frontend_ResetStatus () */
    uint64_t errorCount;  /* Total bit count since last NEXUS_Frontend_ResetStatus() */
                          /* BER = (float) errorCount / bitCount. */
} NEXUS_FrontendBertStatus;

/***************************************************************************
Summary:
    Get the current BERT status

Description:
    This function returns the BERT status for a given frontend.  For the values
to be valid, the frontend needs to have been tuned with BERT enabled.
    This call was introduced for greater precision in the bit and error counts,
plus support for new fields.  As a separate call, frontends which have support
to provide just BERT data can also return the data faster than via the larger
get status structures.

See also:
    NEXUS_FrontendBertStatus,
    NEXUS_FrontendSettings
***************************************************************************/
NEXUS_Error NEXUS_Frontend_GetBertStatus(
    NEXUS_FrontendHandle handle,
    NEXUS_FrontendBertStatus *pStatus
    );

/***************************************************************************
Summary:
Returns connector for this frontend

Description:
There is no attr{shutdown} needed because the frontend connector is passive in the transport module.
***************************************************************************/
NEXUS_FrontendConnectorHandle NEXUS_Frontend_GetConnector(
    NEXUS_FrontendHandle frontend
    );

       
/**
Summary:
**/
void NEXUS_FrontendDevice_Close(
    NEXUS_FrontendDeviceHandle handle
    );


/**
Summary:
**/
typedef enum NEXUS_FrontendDeviceRfInput
{
    NEXUS_FrontendDeviceRfInput_eOff,         /* There is no RF connection between the devices. */
    NEXUS_FrontendDeviceRfInput_eDaisy,       /* Device Rf input through parent device's daisy path. */
    NEXUS_FrontendDeviceRfInput_eLoopThrough, /* Device Rf input through parent device's loop through path. */
    NEXUS_FrontendDeviceRfInput_eMax
} NEXUS_FrontendDeviceRfInput;

/**
Summary:
Settings used for NEXUS_FrontendDevice_Link
**/
typedef struct NEXUS_FrontendDeviceLinkSettings {
    NEXUS_FrontendDeviceRfInput rfInput;
} NEXUS_FrontendDeviceLinkSettings;

/**
Summary:
**/
void NEXUS_FrontendDevice_GetDefaultLinkSettings(
    NEXUS_FrontendDeviceLinkSettings *pSettings
    );

/**
Summary:
Make a link between two devices
**/
NEXUS_Error NEXUS_FrontendDevice_Link(
    NEXUS_FrontendDeviceHandle parentHandle,
    NEXUS_FrontendDeviceHandle childHandle,
    const NEXUS_FrontendDeviceLinkSettings *pSettings
    );
#if defined(HUMAX_PLATFORM_BASE)
/**
Summary:
Unlink a child or all children from the parent
**/
void NEXUS_FrontendDevice_Unlink(
    NEXUS_FrontendDeviceHandle parentHandle,
    NEXUS_FrontendDeviceHandle childHandle /* if NULL, unlink all children */
    );
#endif
/**
Summary:
Get the device for a channel
**/
NEXUS_FrontendDeviceHandle NEXUS_Frontend_GetDevice(
    NEXUS_FrontendHandle handle
    );

/**
Summary:
Make a link between a device and an amplifier
**/
NEXUS_Error NEXUS_FrontendDevice_LinkAmplifier(
    NEXUS_FrontendDeviceHandle handle,
    NEXUS_AmplifierHandle amplifierHandle
    );
#if defined(HUMAX_PLATFORM_BASE)
/**
Summary:
Frontend Device Status
**/
typedef struct NEXUS_FrontendDeviceStatus
{
    struct {
        bool enabled;     /* If true, AVS(Automatic Voltage Scaling) is enabled on the device. */
        unsigned voltage; /* chip voltage in units of millivolts. This is only valid if enabled= true. */
    } avs;
    int temperature;      /* chip temperature in units of 1/1000 degrees Celsius. */
    bool openPending; /* Set true if async device open/resume is pending */
    bool openFailed; /* Set true if async device open/resume has failed */
} NEXUS_FrontendDeviceStatus;

/**
Summary:
Get status for the device
**/
NEXUS_Error NEXUS_FrontendDevice_GetStatus(
    NEXUS_FrontendDeviceHandle handle,
    NEXUS_FrontendDeviceStatus *pStatus
    );
#endif

/**
Summary:
Get total number of tuners present on the frontend device.
**/
typedef struct NEXUS_FrontendDeviceCapabilities
{
    unsigned numTuners;
} NEXUS_FrontendDeviceCapabilities;

void NEXUS_FrontendDevice_GetCapabilities(
    NEXUS_FrontendDeviceHandle handle,
    NEXUS_FrontendDeviceCapabilities *pCapabilities
    );

/***************************************************************************
Summary:
Interrupt Mode -- Used to choose the notification method from frontend to the backedn.
***************************************************************************/
typedef enum NEXUS_FrontendInterruptMode
{
    NEXUS_FrontendInterruptMode_eInterrupt, /* default, use either gpioInterrupt or isrNumber */
    NEXUS_FrontendInterruptMode_ePolling,
    NEXUS_FrontendInterruptMode_eMax
} NEXUS_FrontendInterruptMode;

/***************************************************************************
Summary:
    MTSIF settings that can be configured by the frontend.

Description:
    Some frontends allow certain MTSIF configuration parameters to be changed.
This is a shared structure for NEXUS_Frontend structures and code to use.  It
is initially targeted at NEXUS_FrontendDevice_OpenXXXX calls for frontends
which support this.
    The details on how this maps to a given frontend's MTSIFx is detailed in
the individual frontend's header.
***************************************************************************/
typedef struct NEXUS_FrontendDeviceMtsifSettings {
    bool enabled;           /* If true, enable this MTSIF channel. Typically defaults to 'true', but can be set to 'false' to disable. */
    unsigned clockRate;     /* in Hz */
    unsigned driveStrength; /* in mA */
} NEXUS_FrontendDeviceMtsifSettings;

/***************************************************************************
Summary:
    Channel type for a frontend handle

See Also:
    NEXUS_Frontend_Open
***************************************************************************/
typedef enum NEXUS_FrontendChannelType
{
    NEXUS_FrontendChannelType_eCable,
    NEXUS_FrontendChannelType_eCableOutOfBand,
    NEXUS_FrontendChannelType_eTerrestrial,
    NEXUS_FrontendChannelType_eSatellite,
    NEXUS_FrontendChannelType_eMax
} NEXUS_FrontendChannelType;

/***************************************************************************
Summary:
Out of band settings for a frontend handle

See Also:
    NEXUS_FrontendDevice_Open
***************************************************************************/
typedef enum NEXUS_FrontendOutOfBandInput
{
    NEXUS_OutOfBandInput_eNarrowBandAtoD, /* out of band uses its own dedicated analog to digital converter. */
    NEXUS_OutOfBandInput_eWideBandAtoD,   /* out of band uses the wideband analog to digital converter. */
    NEXUS_OutOfBandInput_eBandPassFilteredNarrowBandAtoD, /* out of band uses its own dedicated analog to digital converter.  The signal is also band pass filtered
                                                                 to filter out frequencies <70Mhz as normal out of band range is from 70-130 Mhz. This should help save some power.*/
    NEXUS_OutOfBandInput_eMax
} NEXUS_FrontendOutOfBandInput;

/***************************************************************************
Summary:
Cable-specific settings for a frontend handle

Description:
    When opening a frontend device using the generic function NEXUS_FrontendDevice_Open,
this structure (in NEXUS_FrontendDeviceOpenSettings) contains the settings specific to
a frontend.
    There are also general settings in the main structure which can apply to any of the
frontend types, such as daisy chain configuration.

See Also:
    NEXUS_FrontendDevice_Open,
    NEXUS_FrontendDeviceOpenSettings
***************************************************************************/
typedef struct NEXUS_FrontendCableConfiguration
{
    bool enabled;

    struct {
        unsigned ifFrequency;       /* IF frequency used for OOB module. */
        NEXUS_FrontendOutOfBandInput useWidebandAtoD; /* Specify which A to D converter is used and how. See the enum for more comments. */
        NEXUS_FrontendOutOfBandNyquistFilter nyquist; /* Nyquist filter used for out of band. */
    } outOfBand;

    struct {
        bool enabled;               /* If true, set the pinmux registers withs values as explained in the specific frontend. */
        unsigned data[4];           /* pin mux data to be applied, data[n]=register, data[n+1]=value. */
    } pinmux;
} NEXUS_FrontendCableConfiguration;

/***************************************************************************
Summary:
Terrestrial-specific settings for a frontend handle

Description:
    When opening a frontend device using the generic function NEXUS_FrontendDevice_Open,
this structure (in NEXUS_FrontendDeviceOpenSettings) contains the settings specific to
a terrestrial frontend.
    There are also general settings in the main structure which can apply to any of the
frontend types, such as daisy chain configuration.

See Also:
    NEXUS_FrontendDevice_Open,
    NEXUS_FrontendDeviceOpenSettings
***************************************************************************/
typedef struct NEXUS_FrontendTerrestrialConfiguration
{
    bool enabled;
} NEXUS_FrontendTerrestrialConfiguration;

/***************************************************************************
Summary:
Terrestrial-specific settings for a frontend handle

Description:
    When opening a frontend device using the generic function NEXUS_FrontendDevice_Open,
this structure (in NEXUS_FrontendDeviceOpenSettings) contains the settings specific to
a satellite frontend.
    There are also general settings in the main structure which can apply to any of the
frontend types, such as daisy chain configuration.

See Also:
    NEXUS_FrontendDevice_Open,
    NEXUS_FrontendDeviceOpenSettings
***************************************************************************/
typedef struct NEXUS_FrontendSatelliteConfiguration
{
    bool enabled;

    struct {
        uint16_t i2cAddress;            /* I2C address to communicate with diseqc */
        NEXUS_I2cHandle i2cDevice;      /* I2C device to communicate with diseqc */
    } diseqc;
} NEXUS_FrontendSatelliteConfiguration;

/***************************************************************************
Summary:
    Settings for a frontend device

Description:
    This structure provides the settings when opening a generic frontend device. (A FrontendDeviceHandle
maps to the frontend chip, a FrontendHandle maps to a specific demod or tuner path.)
    The structure can be used for a frontend of any of the supported types (cable, terrestrial,
or satellite) with NEXUS_FrontendDevice_Open, and is required for frontends with multiple capability
(such as 7364).
    It is similar to chip-specific NEXUS_FrontendDevice_OpenXXXX calls.
    To configure the frontend device's interrupt:
        External interrupt: Set isrNumber
        GPIO interrupt: configure a NEXUS_GpioHandle and use gpioInterrupt
    To configure I2C communication:
        Set i2cDevice, i2cAddress.  If required by the platform, configure i2cSlaveAddr.
        Leave spiDevice NULL.
    To configure SPI communication:
        Set spiDevice.
        Leave i2cDevice NULL.
    For integrated frontends:
        Both spiDevice and i2cDevice should be set to NULL.

    The structure should be initialized with NEXUS_FrontendDevice_GetDefaultOpenSettings before
setting any member variables.

    Support for older frontends via the generic open is not guaranteed.

See Also:
    NEXUS_FrontendDevice_GetDefaultOpenSettings,
    NEXUS_FrontendDevice_Open,
    NEXUS_FrontendCableConfiguration,
    NEXUS_FrontendTerrestrialConfiguration,
    NEXUS_FrontendSatelliteConfiguration
 ***************************************************************************/
typedef struct NEXUS_FrontendDeviceOpenSettings
{
    /* General configuration options */
    NEXUS_FrontendInterruptMode interruptMode; /* Either GPIO or an L1 can be used for notification via interrupts from the frontend to the host.
                                                  If not, polling can be used instead to check the status of frontend. */
    NEXUS_GpioHandle gpioInterrupt; /* GPIO pin for interrupt. If not NULL, isrNumber is ignored. If NULL, isrNumber is used. */
    unsigned isrNumber;             /* L1 interrupt number.  See gpioInterrupt for other interrupt option. */

    /* either I2C or SPI is used for the host to control the frontend. */
    NEXUS_I2cHandle i2cDevice;      /* I2C device to use. spiDevice should be NULL and this set to use I2C. */
    uint16_t i2cAddress;            /* master device I2C Address */
    uint8_t i2cSlaveAddress;        /* slave device I2C Address */

    NEXUS_SpiHandle spiDevice;      /* SPI device to use. i2cDevice should be NULL and this set to use SPI. */

    bool loadAP;                    /* Load Acquisition Processor microcode when starting. */
    bool configureWatchdog;         /* Depending on the platform decide whether to configure watchdog or not. */

    struct {
        int bypassable;             /* in units of 1/100 db. */
        int total;                  /* includes the bypassable and all non-bypassable fixed gains before this device. in units of 1/100 db. */
    } externalFixedGain;

    struct {
        bool enableDaisyChain;      /* enable clock to the next frontend device. */
    } crystalSettings;

    NEXUS_FrontendDeviceMtsifSettings mtsif[NEXUS_MAX_MTSIF]; /* Configure MTSIF TX enable, rate, and drive strength at open time. If values are 0, defaults are used. */

    NEXUS_CallbackDesc updateGainCallback;  /* Callback will be called when the gain from the lna needs to be updated. */

    /* Cable/Terrestrial configuration options */
    NEXUS_FrontendCableConfiguration cable;

    /* Terrestrial configuration options */
    NEXUS_FrontendTerrestrialConfiguration terrestrial;

    /* Satellite configuration options */
    NEXUS_FrontendSatelliteConfiguration satellite;

} NEXUS_FrontendDeviceOpenSettings;


/***************************************************************************
Summary:
    Settings for a frontend channel

Description:
    These are the settings passed into NEXUS_Frontend_Open, and are used to configure
a specific demod or tuner path.  The device is required to be opened by an earlier
call.
    NEXUS_FrontendChannelType should be set to the appropriate frontend category,
and needs to match the portions of the frontend device which were opened.

See Also:
    NEXUS_FrontendDeviceOpenSettings
 ***************************************************************************/
typedef struct NEXUS_FrontendChannelSettings
{
    NEXUS_FrontendDeviceHandle device;  /* Previously opened device to use */
    NEXUS_FrontendChannelType type;     /* Channel type to open from this device */
    unsigned channelNumber;             /* Which channel of the specified type to open from this device */
} NEXUS_FrontendChannelSettings;

/***************************************************************************
Summary:
    Get the default settings for a frontend channel

Description:
See Also:
    NEXUS_Frontend_OpenChannel,
    NEXUS_FrontendChannelSettings
 ***************************************************************************/
void NEXUS_Frontend_GetDefaultOpenSettings(
    NEXUS_FrontendChannelSettings *pSettings   /* [out] */
    );

/***************************************************************************
Summary:
    Open a handle to a frontend device.

Description:
    This function opens a channel on a frontend device and returns a frontend handle.
The channel maps to an individual demod of the device (which may contain more than one
demod).  The total number of channels (demods) available can be retrieved with
NEXUS_FrontendDevice_GetCapabilities.

See Also:
    NEXUS_Frontend_Device_Open,
    NEXUS_Frontend_Close
 ***************************************************************************/
NEXUS_FrontendHandle NEXUS_Frontend_Open(  /* attr{destructor=NEXUS_Frontend_Close} */
    const NEXUS_FrontendChannelSettings *pSettings
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_H__ */

