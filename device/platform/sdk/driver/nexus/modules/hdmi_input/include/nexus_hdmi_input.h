/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_hdmi_input.h $
 * $brcm_Revision: 33 $
 * $brcm_Date: 10/12/12 3:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_input/include/nexus_hdmi_input.h $
 * 
 * 33   10/12/12 3:15p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle
 * 
 * 32   8/17/11 4:30p vsilyaev
 * SW7425-991: Add HDMI hardware passthrough support
 * 
 * 31   7/12/11 3:20p erickson
 * SW7420-1979: add shutdown attr to GetVideoConnector and
 *  GetAudioConnector
 * 
 * 30   4/18/11 12:22p erickson
 * SW7422-186: merge
 *
 * SW7422-186/3   4/15/11 2:55p rgreen
 * SW7422-186: API Review updates
 *
 * SW7422-186/2   4/11/11 2:55p rgreen
 * SW7422-186: Add NEXUS_HdmiInput_ToggleHotPlug function to support/force
 *  upstream HDCP reauthentication
 *
 * SW7422-186/1   3/28/11 5:37p rgreen
 * SW7422-186: Add HDCP Repeater Support
 *
 * 29   3/15/11 10:32a erickson
 * SW7425-163: add attr{null_allowed=y}
 *
 * 28   2/16/11 5:21p erickson
 * SW7422-129: merge
 *
 * SW7422-129/2   2/10/11 3:48p rgreen
 * SW7422-129: Remove hdcpSettings from hdmiInputSettings; create separate
 *  hdcpSettings
 *
 * SW7422-129/1   2/2/11 6:26p rgreen
 * SW7422-129: Add HdcpSettings to HdmiInputSettings
 *
 *
 * 27   2/3/11 3:23p vsilyaev
 * SW35230-2882: Restore access to gamut metadata packet
 *
 * 26   12/22/10 5:24p vsilyaev
 * SW35230-2374: Merged support for OTP HDCP Key verfication
 *
 * SW35230-2374/2   12/20/10 3:59p rgreen
 * SW35230-2374: Move HDMI HDCP Key Set verification functionality to
 *  separate file
 *
 * SW35230-2374/1   12/3/10 9:09p rgreen
 * SW35230-2374: Add support for OTP HDCP Key verfication
 *
 * 25   9/20/10 2:31p jhaberf
 * SW35230-993: Added adcCalibration for HDMI input self test mode
 *
 * 24   5/12/10 4:40p erickson
 * SW3548-2795: merge
 *
 * SW3548-2642/1   5/3/10 11:49a rgreen
 * SW3548-2975: Add NEXUS_HdmiInput_SetMaster function to configure the
 *  master/slave core; Remove masterHdmiInput from hdmiInputSettings
 *
 * BCM3D/1   1/19/10 4:48p erickson
 * SW3548-2727: add 3DTV support
 *
 * 22   8/21/09 3:39p erickson
 * PR57890: add setting for disconnected HPD signal
 *
 * 21   6/18/09 12:01p vsilyaev
 * PR 56097: Added validSpdifInfo into the status structure
 *
 * 20   5/27/09 4:11p erickson
 * PR51331: add NEXUS_HdmiInputSettings.powerDownUnusedPort
 *
 * 19   5/4/09 5:29p erickson
 * PR54810: remove gamutMetadataPacketChanged
 *
 * 18   4/9/09 3:58p erickson
 * PR52879: merge
 *
 * PR52879/3   4/9/09 3:24p rgreen
 * PR52879: EDID RAM Support; remove pointers from input settings
 *
 * PR52879/2   4/8/09 4:27p rgreen
 * PR52879: Add support for on-chip EDID RAM
 *
 * PR52879/1   3/23/09 2:00p rgreen
 * PR52879: Add EDID RAM Support
 *
 * 16   3/11/09 5:39p jgarrett
 * PR 53108: Extending forcePcFormat to include all 1080p formats
 *
 * 15   3/4/09 1:03p jgarrett
 * PR 52618: Adding forcePcFormat option to HdmiInput
 *
 * 14   2/19/09 1:53p erickson
 * PR51721: update
 *
 * 13   2/19/09 1:28p erickson
 * PR51752: merge
 *
 * branch/1   2/18/09 4:22p rgreen
 * PR51752: add validHdmiStatus to NEXUS_HdmiInputStatus structure to
 *  indicate validity of returned HdmiStatus
 *
 * 12   12/16/08 3:13p erickson
 * PR49943: merge
 *
 * PR49943/2   12/10/08 4:18p rgreen
 * PR49943: Rename enableI2cPullup to disableI2cPullup
 *
 * 11   12/5/08 2:31p erickson
 * PR49943: merge
 *
 * PR49943/1   12/5/08 1:46p rgreen
 * PR49943: Add support to enable/disable I2c pull up at initialization
 *
 * 10   12/1/08 11:03a erickson
 * PR49226: update comments on HdmiInputStatus
 *
 * PR49226/1   11/27/08 6:15p honglang
 * PR49226, PR49228: add comment for HDMI input status
 *
 * 9   11/26/08 2:32p erickson
 * PR49228: merge
 *
 * PR49228/1   11/25/08 6:49p honglang
 * PR49226, PR49228: add the real format information to HDMI input status
 *
 * 8   11/12/08 4:28p jgarrett
 * PR48741: Add setting for BHDR_FE_Open to indicate use of external
 *  switch
 *
 * PR48741/2   11/12/08 4:21p rgreen
 * PR48741: Add nexus setting to indicate use of external switch in front
 *  of the  HDMI Rx
 *
 * PR48741/1   11/11/08 1:53p honglang
 * PR48741: Add setting for BHDR_FE_Open to indicate use of external
 *  switch
 *
 * 7   11/3/08 2:27p erickson
 * PR47030: add HdmiInput heap
 *
 * 6   10/8/08 2:25a erickson
 * PR46176: Implement Power Management; move frontend inside of the
 *  hdmiInput object
 *
 * 5   9/16/08 9:09a erickson
 * PR46801: clarify comments on colorSpace settings and status
 *
 * 4   7/31/08 9:50a erickson
 * PR44523: merge
 *
 * PR44523/1   7/8/08 4:37p rgreen
 * PR44523: Add support for Vendor Specific Infoframe
 *
 * 3   5/19/08 1:38p erickson
 * PR39453: HdmiInput updates
 *
 * 2   5/12/08 11:07a erickson
 * PR39453: HdmiInputFrontend impl
 *
 * 1   5/6/08 4:44p erickson
 * PR39453: add separate impl for 3548/3556
 *
 * PR39009/1   4/17/08 12:32a honglang
 * PR39009: add more status to API
 *
 * 8   4/14/08 10:26a erickson
 * PR40780: merge
 *
 * PR40780/1   4/2/08 4:37p honglang
 * PR40780: Add audio info and pixel repetition factor to the get status
 *  function of HDMI module
 *
 * 7   3/26/08 11:48p vsilyaev
 * PR 40410: Added more video settings for HDMI Input interface
 *
 * 6   3/26/08 3:21p erickson
 * PR40949: rework NEXUS_HdmiInput_SetHdcpKeySet so that key data is
 *  passed from user to kernel mode
 *
 * 5   3/26/08 10:54a erickson
 * PR40742: added more HdmiInput which is passed in from VDC source
 *  changed callback
 *
 * 4   2/7/08 11:16p vsilyaev
 * PR 38682: Added linux kernel/proxy support for 93563 platform
 *
 * 3   1/23/08 8:37p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 5:16p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/10   1/9/08 12:31p erickson
 * PR36814: update comments, naming convention
 *
 * Nexus_Devel/9   1/9/08 6:20p honglang
 * PR36814: Add CSC and AR to get HDMI status,  and setHotplug function,
 * configAfterHotplug function, get packets raw function group.
 *
 * Nexus_Devel/8   12/14/07 5:15p ditian
 * PR37956: Porting BHM  to Nexus: analog video input.
 *
 * Nexus_Devel/7   11/30/07 11:12a erickson
 * PR35457: api update
 *
 * Nexus_Devel/6   11/28/07 10:37a erickson
 * PR35457: doc update
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_INPUT_H__
#define NEXUS_HDMI_INPUT_H__

/*=***********************************
HdmiInput is an HDMI receiver.
It routes audio and video data from an HDMI receiver port to the Display and Audio modules.
Each instance controls one HDMI receiver port.
*************************************/

#include "nexus_types.h"
#include "nexus_video_types.h"
#include "nexus_audio_types.h"
#include "nexus_spdif_types.h"
#include "nexus_hdmi_input_hdcp_types.h"
#include "nexus_timebase.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
Summary:
Handle for the HdmiInput receiver interface.

Description:
An HDMI frontend must be attached to an HDMI receiver in order to process audio and video.
The number of HDMI receivers is based on the number of simultaneous audio/video streams can be processed at a time (main and possibly PIP).
The number of HDMI frontends is based on the number of HDMI connections on the box.
**/
typedef struct NEXUS_HdmiInput *NEXUS_HdmiInputHandle;


/**
Summary:
Settings used to configure an HdmiInput receiver interface.
**/
typedef struct NEXUS_HdmiInputSettings
{
    NEXUS_Timebase timebase; /* The timebase which is locked to this HdmiInput.
                                When the video frame rate is discovered, it will be automatically configure the Timebase
                                if NEXUS_TimebaseSettings.sourceType == NEXUS_TimebaseSourceType_eHdDviIn for this timebase. */
    NEXUS_CallbackDesc avMuteChanged; /* called when NEXUS_HdmiInputStatus.avMute changes */
    NEXUS_CallbackDesc sourceChanged; /* called when the audio or video format changes. Call NEXUS_HdmiInput_GetStatus to get new format information. */
    bool forcePcFormat;  /* If true, a PC format will be used for the HDMI format if available.  Currently, this applies to 1080p inputs only.
                            This may be useful if capturing PC data over HDMI.  Default=false. */
    bool autoColorSpace; /* if autoColorSpace is true, then whatever color space reported by the HDMI receiver is used (default is true) */
    NEXUS_ColorSpace colorSpace; /* if autoColorSpace is false, then this value is used instead of the value reported by the HDMI receiver */
    bool useInternalEdid ; /* use when on-chip EDID RAM is to be enabled */
    bool powerDownUnusedPort ;  /* use if HDMI port will never be used in design */

    struct   {
        bool equalizationEnabled;
        bool externalSwitch;
        bool hpdDisconnected ;
        NEXUS_CallbackDesc cecCallback;
        NEXUS_CallbackDesc hotPlugCallback;
    } frontend;

    struct {
        bool parseAviInfoframe ;

        bool disableI2cSclPullUp ;
        bool disableI2cSdaPullUp ;

        bool enableHdmiHardwarePassthrough ;
    } hdr ;

    NEXUS_CallbackDesc aviInfoFrameChanged;
    NEXUS_CallbackDesc audioInfoFrameChanged;
    NEXUS_CallbackDesc spdInfoFrameChanged;
    NEXUS_CallbackDesc vendorSpecificInfoFrameChanged;
    NEXUS_CallbackDesc audioContentProtectionChanged;
    NEXUS_CallbackDesc gamutMetadataPacketChanged;
    NEXUS_HeapHandle heap;

    bool adcCalibration;
} NEXUS_HdmiInputSettings;


/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_HdmiInput_GetDefaultSettings(
    NEXUS_HdmiInputSettings *pSettings /* [out] */
    );

/**
Summary:
Open a new HdmiInput receiver interface
**/
NEXUS_HdmiInputHandle NEXUS_HdmiInput_Open( /* attr{destructor=NEXUS_HdmiInput_Close}  */
    unsigned index,
    const NEXUS_HdmiInputSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
Open a new HdmiInput receiver interface with EDID RAM support
**/
NEXUS_HdmiInputHandle NEXUS_HdmiInput_OpenWithEdid( /* attr{destructor=NEXUS_HdmiInput_Close}  */
    unsigned index,
    const NEXUS_HdmiInputSettings *pSettings, /* attr{null_allowed=y} */
    const uint8_t *edidData, /* attr{nelem=edidDataSize} */
    uint16_t edidDataSize
    );

/**
Summary:
Close an HdmiInput receiver interface
**/
void NEXUS_HdmiInput_Close(
    NEXUS_HdmiInputHandle handle
    );

/**
Summary:
Get current settings
**/
void NEXUS_HdmiInput_GetSettings(
    NEXUS_HdmiInputHandle    handle,
    NEXUS_HdmiInputSettings *pSettings /* [out] */
    );

/**
Summary:
Set new settings
**/
NEXUS_Error NEXUS_HdmiInput_SetSettings(
    NEXUS_HdmiInputHandle handle,
    const NEXUS_HdmiInputSettings *pSettings
    );

/**
Summary:
Status returned by NEXUS_HdmiInput_GetStatus
**/
typedef struct NEXUS_HdmiInputStatus
{
    /* From HDMI RX */
    bool                validHdmiStatus ;
    uint8_t             hdmiMode; /* 1 - HDMI mode ; 0 - DVI mode */
    uint8_t             deviceAttached; /* 1 - HDMI cable connected ; 0 - HDMI cable disconnected */
    uint8_t             pllLocked; /* 1 - HDMI PLL locked ; 0 - HDMI PLL unlocked */
    bool                packetErrors; /* true - HDMI packet error occurs ; false - no any packet error occurs */
    bool                avMute; /* true - the current avmute status is mute ; false - the current avmute status is unmute */
    bool                hdcpRiUpdating; /* true - HDCP Ri key is updating ; false - HDCP Ri key is not updating */
    bool                symbolLoss; /* true - symbol loss is detected ; false - symbol loss is not detected  */
    uint32_t            lineClock; /* HDMI RX physical line clock */
    NEXUS_ColorSpace    colorSpace; /* The color space repored by the HDMI receiver. If NEXUS_HdmiInputSettings.autoColorSpace is true, this value is being used.
                                       If NEXUS_HdmiInputSettings.autoColorSpace is false, this value is being ignored. */
    NEXUS_AspectRatio   aspectRatio; /* aspect ration from AVI info packet */
    uint32_t            width; /* the actual width in HDMI RX format detection */
    uint32_t            height; /* the actual height in HDMI RX format detection */

    /* From HDDVI */
    bool                interlaced; /* true - interlaced format ; false - progressive format. From the format data HDDVI locked to*/
    NEXUS_VideoFormat   originalFormat; /* which format HDDVI has been locked to */
    bool                noSignal; /* HDDVI detects no input signal */
    uint32_t            avWidth; /* the actual width in HDDVI format detection */
    uint32_t            avHeight; /* the actual height in HDDVI format detection */
    uint32_t            vertFreq; /* vertical frequency from the format data HDDVI locked to */
    NEXUS_VideoPolarity hPolarity; /* the actual horizontal polarity in HDDVI format detection */
    NEXUS_VideoPolarity vPolarity; /* the actual vertical polarity in HDDVI format detection */
    uint32_t            vBlank; /* the actual vertical blank in HDDVI format detection */
    uint32_t            hBlank; /* the actual horizontal blank in HDDVI format detection */

    uint8_t pixelRepeat; /* the actual pixel repeat status in HDDVI format detection */

    struct {
        uint16_t packets; /* The number of audio packet between two successive vsyncs */
        bool validSpdifInfo ; /* indicator that Stream Type, Word Length, and Sample Frequency are valid */
        NEXUS_SpdifStreamType streamType; /* audio format: PCM or not PCM */
        NEXUS_SpdifWordLength wordLength; /* audio word length, will be significant if it's PCM audio */
        NEXUS_SpdifSamplingFrequency sampleFreq; /* audio sample frequency, will be significant if it's PCM audio  */
    } audio; /* audio information */
} NEXUS_HdmiInputStatus;

/**
Summary:
Get status
**/
NEXUS_Error NEXUS_HdmiInput_GetStatus(
    NEXUS_HdmiInputHandle handle,
    NEXUS_HdmiInputStatus *pStatus /* [out] */
    );


/**
Summary:
Returns the abstract NEXUS_VideoInput connector for the HdmiInput.
This connector is passed to a VideoWindow to display the decoded video.

Description:
Returns:
NEXUS_VideoInput - abstract connector

See Also:
NEXUS_VideoWindow_AddInput
**/
NEXUS_VideoInputHandle NEXUS_HdmiInput_GetVideoConnector( /* attr{shutdown=NEXUS_VideoInput_Shutdown} */
    NEXUS_HdmiInputHandle handle
    );

/**
Summary:
Returns the abstract NEXUS_AudioInput connector for the HdmiInput.
This connector is passed to a Mixer or AudioOutput to route audio to an output.

Description:
Returns:
NEXUS_AudioInput - abstract connector

See Also:
NEXUS_Mixer_AddInput
**/
NEXUS_AudioInput NEXUS_HdmiInput_GetAudioConnector( /* attr{shutdown=NEXUS_AudioInput_Shutdown} */
    NEXUS_HdmiInputHandle handle
    );

/**
Summary:
Control the connection of HDMI by forcing the hotplug out pin to high or low.
**/
NEXUS_Error NEXUS_HdmiInput_SetHotPlug(
    NEXUS_HdmiInputHandle handle,
    bool status /* True - to disconnect, False - to connect */
    );

/**
Summary:
Manual hot plug configuration

Description:
Use this function if your board does not use our hotplug pin, but uses GPIO instead.
Invoke this function to make some necessary configuration changes when the connection status changes.
**/
NEXUS_Error NEXUS_HdmiInput_ConfigureAfterHotPlug(
    NEXUS_HdmiInputHandle handle,
    bool status /* True - connection up, False - connnection down */
    );

/**
Summary:
Use this function to load HDMI Rx EDID Data to on-chip EDID RAM
**/
NEXUS_Error NEXUS_HdmiInput_LoadEdidData(
    NEXUS_HdmiInputHandle handle,
    const uint8_t *dataBytes, /* attr{nelem=numBytes} */
    uint16_t numBytes
    );

/**
Summary:
Use this function to unload/free any resources used to load HDMI Rx EDID Data
**/
NEXUS_Error NEXUS_HdmiInput_UnloadEdidData(
    NEXUS_HdmiInputHandle handle
    );

/**
Summary:
Set the master HDMI frontend for 3DTV mode
This requires coordinated calls to both master and slave, and also required
NEXUS_VideoWindow_Add/RemoveInput calls to reinitialized the video pipeline.
See nexus/examples/3dtv/dtv for code.
**/
NEXUS_Error NEXUS_HdmiInput_SetMaster(
    NEXUS_HdmiInputHandle hdmiInput,
    NEXUS_HdmiInputHandle master
    );

/**
Summary:
Force a toggle of the Hot Plug on the hdmiInput
**/
void NEXUS_HdmiInput_ToggleHotPlug(
    NEXUS_HdmiInputHandle hdmiInput
    );

#ifdef __cplusplus
}
#endif

#endif

