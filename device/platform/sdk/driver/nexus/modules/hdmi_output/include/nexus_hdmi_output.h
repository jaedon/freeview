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
 * $brcm_Workfile: nexus_hdmi_output.h $
 * $brcm_Revision: 40 $
 * $brcm_Date: 10/19/12 2:19p $
 *
 * Module Description:
 *                      HdmiOutput: Specific interfaces for an HDMI/DVI output.
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_output/include/nexus_hdmi_output.h $
 * 
 * 40   10/19/12 2:19p jgarrett
 * SW7358-417: Merge to main branch
 * 
 * SW7358-417/1   10/3/12 12:21p mpeteri
 * SW7358-417: Changes to allow NEXUS to configure HDMI audio burst
 *  settings.
 * 
 * 39   9/13/12 11:54a erickson
 * SW7425-3904: merge
 * 
 * SW7425-3904/2   9/13/12 11:13a rgreen
 * SW7425-3904: API revew changes
 * 
 * SW7425-3904/1   9/10/12 6:13p rgreen
 * SW7425-3904: Make RxSense retry and polling interval configurable
 * 
 * 38   6/8/12 1:21p erickson
 * SW7425-3159,SW7346-849: merge
 * 
 * SW7346-849/1   6/4/12 5:55p rgreen
 * SW7425-3159,SW7346-849:  Add rxPowered to HdmiOutputStatus
 * 
 * 37   5/31/12 7:53p bandrews
 * SW7425-3152: Fix comment for HdmiOutput status 'connected' member
 * 
 * 36   4/13/12 4:58p erickson
 * SW7346-737: add NEXUS_HdmiOutput_GetCrcData
 * 
 * 35   7/12/11 3:01p erickson
 * SW7420-1979: add shutdown attr to GetVideoConnector and
 *  GetAudioConnector
 * 
 * 34   4/29/11 12:06p erickson
 * SW7125-798: revise NEXUS_HdmiOutput_SetPreEmphasisConfiguration
 * 
 * 33   4/22/11 11:09a erickson
 * SW7125-798: merge
 * 
 * SW7125-798/1   2/3/11 11:27a vle
 * SW7125-798: Add NEXUS API to allow customer to control pre-emphasis
 *  settings
 * 
 * 32   2/17/11 5:31p erickson
 * SW7420-1289: merge
 *
 * SW7420-1289/2   2/17/11 10:03a vle
 * SW7420-1289: Add NEXUS APIs to get/set HDMI AVI InfoFrame, HDMI Audio
 *  Infoframe. Share structures for hdmi_input and hdmi_output
 *
 * SW7420-1289/1   1/27/11 2:35p vle
 * SW7420-1289: Add NEXUS APIs to get/set HDMI AVI InfoFrame, HDMI Audio
 *  Infoframe
 *
 * 31   11/11/10 4:00p erickson
 * SW7420-184__SW7400-2956: add NEXUS_HdmiOutputAutoLipsyncInfo and
 *  NEXUS_HdmiOutputMonitorRange
 *
 * 30   10/22/10 10:52a erickson
 * SW7422-23: update ieeeRegId field
 *
 * 29   10/8/10 4:30p erickson
 * SW7405-4782: merge
 *
 * SW7405-4782/2   10/7/10 12:04p vle
 * SW7405-4782: Changing aspect ratio does not require HDCP re-
 *  authentication - TAKE 2. First implementation attempt has a potential
 *  race condition.
 *
 * 28   10/7/10 11:04a erickson
 * SW7420-1148: add destructor hint
 *
 * 27   9/22/10 11:24a erickson
 * SW7405-4782: merge
 *
 * 26   6/4/10 4:07p erickson
 * SW7405-3994: refactor HDMI 3D Structures to report array of video
 *  formats
 *
 * SW7405-3994/3   6/4/10 2:34p vle
 * SW7405-3994: Add HDMI 3D Structures defines in NEXUS
 *
 * SW7405-3994/2   5/14/10 6:12p vle
 * SW7405-3994: Add support to check for supported 3D formats.
 *
 * SW7405-3994/1   5/11/10 7:11p vle
 * SW7405-3994: report supported 3D formats to NEXUS.
 *
 * 24   4/23/10 9:50a erickson
 * SW7420-676: merge
 *
 * SW7420-676/2   4/22/10 4:54p vle
 * SW7420-676: Fix incorrect programming in return size.
 *
 * SW7420-676/1   4/21/10 2:27p vle
 * SW7420-676: Add API to return supported video info
 *
 * 23   4/5/10 10:05a erickson
 * SW7405-3997: use nexus_hdmi_types.h for VSI enums
 *
 * 22   3/17/10 11:10a erickson
 * SW3548-2820: update NEXUS_HdmiOutputVendorSpecificInfoFrame
 *
 * 21   1/25/10 9:34a erickson
 * SW7405-3486: added NEXUS_HdmiOutputSettings.syncOnly
 *
 * 20   1/14/10 5:31p vsilyaev
 * SW7401-4355: Add HDMI Vendor Specific InfoFrame support
 *
 * 19   1/12/10 6:47p jgarrett
 * SW7335-654: Adding dither option for HDMI outputs
 *
 * 18   12/21/09 4:00p erickson
 * SW3556-935: modify spread spectrum variables
 *
 * 17   8/25/09 3:18p erickson
 * SW7405-2670: add NEXUS_HdmiOutput_SetTmdsSignal and
 *  NEXUS_HdmiOutput_GetTmdsSignal
 *
 * 16   7/21/09 9:42a erickson
 * PR54888: merged additional NEXUS_HdmiOutputStatus
 *
 * PR54888/1   7/14/09 7:15p vle
 * PR54888: Add support to retrieve audio/video property from HDMI.
 *
 * 15   7/15/09 1:40p erickson
 * PR56609: merged in NEXUS_HdmiOutputSettings.spreadSpectrum settings
 *
 * PR56609/1   7/15/09 11:33a rgreen
 * PR56609: Add HDMI Spread Spectrum support
 *
 * 14   6/22/09 4:04p erickson
 * PR56132: added NEXUS_HdmiOutput_SetVendorSpecificInfoFrame
 *
 * 13   5/11/09 1:48p erickson
 * PR53373: remove colorDepth from NEXUS_HdmiOutputStatus
 *
 * 12   5/4/09 3:40p erickson
 * PR53373: merge HDMI 1.3a support
 *
 * PR53373_DeepColor/1   4/16/09 3:58p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 11   3/16/09 4:37p erickson
 * PR53271: added manual colorSpace option to NEXUS_HdmiOutputSettings
 *
 * 10   12/17/08 11:38p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * 9   12/16/08 2:55p erickson
 * PR48987: merge
 *
 * HDMI_TX_Plugfest11_200811/3   12/5/08 4:34p vle
 * PR 48987: Update to use better variable name.
 *
 * HDMI_TX_Plugfest11_200811/2   11/13/08 7:34p vle
 * PR 48987: Add run time option to enable I2C bit bang mode.
 *
 * HDMI_TX_Plugfest11_200811/1   11/10/08 7:13p vle
 * PR48842: Fix HDCP in BRUTUS. Make sure to report the status to Brutus
 *  and restart HDCP authentication process on each format change.
 *
 * 8   8/1/08 4:26p erickson
 * PR39979: add CEC support
 *
 * 7   6/27/08 9:30a erickson
 * PR43653: remove TODO. VESA supported with separate NEXUS_VideoFormat
 *  enums.
 *
 * 6   6/25/08 11:43a erickson
 * PR39453: moved xvYccEnabled to NEXUS_DisplaySettings
 *
 * 5   5/12/08 4:55p erickson
 * PR39453: add NEXUS_HdmiOutputSettings.xvYccEnabled
 *
 * 4   4/3/08 12:11p erickson
 * PR41302: add MPAA decimation
 *
 * 3   4/2/08 6:46p jgarrett
 * PR 41233: Moving PJ checking to HDCP
 *
 * 2   2/21/08 5:58p jgarrett
 * PR 39017: Separating HDCP into a separate header file
 *
 * 1   2/1/08 5:25p jgarrett
 * PR 39017: Adding hdmi_output
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_OUTPUT_H__
#define NEXUS_HDMI_OUTPUT_H__

#include "nexus_i2c.h"
#include "nexus_hdmi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for the HDMI output interface.
**/
typedef struct NEXUS_HdmiOutput *NEXUS_HdmiOutputHandle;

/**
Summary:
Enumeration for SPD (Source Product Description) Device Types
**/
typedef enum NEXUS_HdmiSpdSourceDeviceType
{
    NEXUS_HdmiSpdSourceDeviceType_eUnknown,
    NEXUS_HdmiSpdSourceDeviceType_eDigitalStb,
    NEXUS_HdmiSpdSourceDeviceType_eDvd,
    NEXUS_HdmiSpdSourceDeviceType_eDvhs,
    NEXUS_HdmiSpdSourceDeviceType_eHddVideo,
    NEXUS_HdmiSpdSourceDeviceType_eDvc,
    NEXUS_HdmiSpdSourceDeviceType_eDsc,
    NEXUS_HdmiSpdSourceDeviceType_eVideoCd,
    NEXUS_HdmiSpdSourceDeviceType_eGame,
    NEXUS_HdmiSpdSourceDeviceType_ePc,
    NEXUS_HdmiSpdSourceDeviceType_eMax
} NEXUS_HdmiSpdSourceDeviceType;

#define NEXUS_HDMI_SPD_VENDOR_NAME_MAX 8
#define NEXUS_HDMI_SPD_DESCRIPTION_MAX 16
/**
Summary:
Settings to open the HDMI output interface
**/
typedef struct NEXUS_HdmiOutputOpenSettings
{
    NEXUS_I2cHandle i2c;        /* TMDS I2C Bus Handle */

    struct
    {
        NEXUS_HdmiSpdSourceDeviceType deviceType;
        char vendorName[NEXUS_HDMI_SPD_VENDOR_NAME_MAX+1];
        char description[NEXUS_HDMI_SPD_DESCRIPTION_MAX+1];
    } spd;  /* Source Product Description */

    bool bypassEdidChecking;    /* DEBUG/TEST only, bypasses EDID checking if true */
    bool i2cSoftwareMode;       /* If true, enable I2C Bit Bang Mode */

    unsigned maxEdidRetries;        /* Maximum number of retries to read EDID from the receiver */
    unsigned powerPollingInterval;  /* Polling interval for receiver power changes in ms */

    unsigned maxRxSenseRetries;  /* Maximum number of retries to check if attached Rx is powered */
    unsigned rxSenseInterval;    /* time between checks for Rx Sense in milliseconds */
		
} NEXUS_HdmiOutputOpenSettings;


/** 
Summary:
Pre-emphasic configurations settings 
**/
typedef struct NEXUS_HdmiOutputPreEmphasisConfiguration
{
    uint16_t preEmphasis_CK;
    uint16_t preEmphasis_Ch0;
    uint16_t preEmphasis_Ch1;
    uint16_t preEmphasis_Ch2; 

    /* The following fields are valid for 65nm platforms only */
    uint8_t driverAmp;
    uint8_t refCntl;
    uint8_t termR;

    /* The following fields are valid for 40nm platforms only */
    uint8_t  hfEn;
    uint16_t currentRatioSel;
    uint8_t  kP;
    uint8_t  kI;
    uint8_t  kA;    
} NEXUS_HdmiOutputPreEmphasisConfiguration;


/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
*/
void NEXUS_HdmiOutput_GetDefaultOpenSettings(
    NEXUS_HdmiOutputOpenSettings *pSettings    /* [out] Settings */
    );

/**
Summary:
Open an HDMI output interface
**/
NEXUS_HdmiOutputHandle NEXUS_HdmiOutput_Open( /* attr{destructor=NEXUS_HdmiOutput_Close} */
    unsigned index,
    const NEXUS_HdmiOutputOpenSettings *pSettings    /* settings */
    );

/**
Summary:
Close the HdmiOutput interface
**/
void NEXUS_HdmiOutput_Close(
    NEXUS_HdmiOutputHandle output
    );

/**
Summary:
Settings to configure the HDMI output interface
**/
typedef struct NEXUS_HdmiOutputSettings
{
    bool preemphasisEnabled;    /* If true, preemphasis will be enabled.  This can be used with long cables */
    unsigned preFormatChangeAvMuteDelay;   /* Delay (in msec) between sending AVMute packet and changing display format */
    unsigned postFormatChangeAvMuteDelay;  /* Delay (in msec) between changing display format and sending AVUnmute packet */

    NEXUS_CallbackDesc hotplugCallback; /* Callback will be called when a hotplug event occurs */
    NEXUS_CallbackDesc cecCallback;     /* Callback will be called when a CEC message is sent or received. See NEXUS_HdmiOutputCecStatus. */

    struct
    {
        bool           professionalMode;    /* [0:0] The professional mode flag.
                                                TRUE: Professional mode. Other user
                                                options will not be considered.
                                                FALSE: Consumer mode.*/
        bool           swCopyRight;         /* [2:2] Software CopyRight assert.
                                                TRUE: CopyRight is asserted
                                                FALSE: CopyRight is not asserted */
        uint16_t       categoryCode;        /* [8:15] Category Code */
        uint16_t       clockAccuracy;       /* [28:29] Clock Accuracy */
        bool           separateLRChanNum;   /* TRUE:  Left channel num = 0000
                                                      Right Channel Num = 0000
                                               FALSE: Left channel num = 1000
                                                      Right Channel Num = 0100 */
    } audioChannelStatusInfo;

    bool audioDitherEnabled;    /* If true, a dither signal will be sent out when
                                   there is no audio data on this output in PCM mode. */
    NEXUS_SpdifOutputBurstType  audioBurstType; /* burst type when the HDMI compressed audio
                                                   transmission is inactive (decoder underflow, mute, etc) */

    bool mpaaDecimationEnabled; /* When MPAA decimation is enabled, a significant number of pixels in the output video
                                   signal are replaced with values interpolated from neighboring pixels.
                                   Therefore, video quality is lowered. */

    bool autoColorSpace; /* if autoColorSpace is true, then whatever color space reported by the HDMI receiver is used (default is true) */
    NEXUS_ColorSpace colorSpace; /* if autoColorSpace is false, then this value is used instead of the value reported by the HDMI receiver */
    NEXUS_HdmiColorDepth colorDepth; /* default is 8bit standard color depth. 10bit & 12bit deep color are supported with HDMI 1.3 platform only */

    /* Spread Spectrum configuration */
    struct
    {
        bool                     enable;  /* enable Spread Spectrum */
        uint32_t                 frequency;
        uint32_t                 delta;
    } spreadSpectrum;

    bool syncOnly;
    
    unsigned crcQueueSize; /* if non-zero, CRC capture is enabled. use NEXUS_HdmiOutput_GetCrcData to retrieve data. */
} NEXUS_HdmiOutputSettings;

/**
Summary:
Get current settings
**/
void NEXUS_HdmiOutput_GetSettings(
    NEXUS_HdmiOutputHandle output,
    NEXUS_HdmiOutputSettings *pSettings    /* [out] Settings */
    );

/**
Summary:
Apply new settings
**/
NEXUS_Error NEXUS_HdmiOutput_SetSettings(
    NEXUS_HdmiOutputHandle output,
    const NEXUS_HdmiOutputSettings *pSettings
    );


/**
Summary:
Macros for HDMI 3D Structures bit-fields, used in NEXUS_HdmiOutputStatus.hdmi3DFormatsSupported[]
**/
#define NEXUS_HdmiOutput_3DStructure_FramePacking               0x0001
#define NEXUS_HdmiOutput_3DStructure_FieldAlternative           0x0002
#define NEXUS_HdmiOutput_3DStructure_LineAlternative            0x0004
#define NEXUS_HdmiOutput_3DStructure_SideBySideFull             0x0008

#define NEXUS_HdmiOutput_3DStructure_LDepth                     0x0010
#define NEXUS_HdmiOutput_3DStructure_LDepthGraphics             0x0020
#define NEXUS_HdmiOutput_3DStructure_TopAndBottom               0x0040
#define NEXUS_HdmiOutput_3DStructure_Reserved07                 0x0080

#define NEXUS_HdmiOutput_3DStructure_SideBySideHalfHorizontal   0x0100
#define NEXUS_HdmiOutput_3DStructure_Reserved09                 0x0200
#define NEXUS_HdmiOutput_3DStructure_Reserved10                 0x0400
#define NEXUS_HdmiOutput_3DStructure_Reserved11                 0x0800

#define NEXUS_HdmiOutput_3DStructure_Reserved12                 0x1000
#define NEXUS_HdmiOutput_3DStructure_Reserved13                 0x2000
#define NEXUS_HdmiOutput_3DStructure_Reserved14                 0x4000
#define NEXUS_HdmiOutput_3DStructure_SideBySideHalfQuincunx     0x8000



/****
Summary:
Data structure containing audio and video latency information for auto lipsync feature
support by the HDMI receiver. These information are located in the receiver's
EDID PROM
******/
typedef struct NEXUS_HdmiOutputAutoLipsyncInfo
{
    uint8_t videoLatency;   /* video latency for progressive video formats (ms) */
    uint8_t audioLatency;   /* audio latency for progressive video formats (ms) */
    uint8_t interlacedVideoLatency;     /* video latency for interlaced video format (ms) */
    uint8_t interlacedAudioLatency;     /* audio lantecy for interfaced video format (ms) */
} NEXUS_HdmiOutputAutoLipsyncInfo;



/****
Summary:
Data structure containing Monitor Ranges data contained in a receiver's EDID
PROM
******/
typedef struct NEXUS_HdmiOutputMonitorRange
{
    uint16_t minVertical;     /* Minimum Vertical Rate (Hz) */
    uint16_t maxVertical;     /* Maximum Vertical Rate (Hz) */
    uint16_t minHorizontal;   /* Minimum Horizontal Rate (kHz) */
    uint16_t maxHorizontal;   /* Maximum Horizontal Rate (kHz) */
    uint16_t maxPixelClock;   /* Maximum Supported Pixel Clock Rate (MHz) */
    uint8_t secondaryTiming;  /* Secondary Timing Formula (if supported) */
    uint8_t secondaryTimingParameters[7];  /* Secondary Timing Formula Params */
} NEXUS_HdmiOutputMonitorRange;


/**
Summary:
Status for the HDMI interface
**/
typedef struct NEXUS_HdmiOutputStatus
{
    bool connected;     /* True if a receiver device is connected; device may be ON or OFF */
    bool rxPowered;    /* True if receiver device is powered ON, false OFF */
    bool hdmiDevice;    /* True if HDMI, false if DVI */

    char monitorName[14];                               /* NULL-terminated string for the monitor name from EDID data */
    NEXUS_HdmiOutputMonitorRange monitorRange;
    NEXUS_VideoFormat preferredVideoFormat;

    bool videoFormatSupported[NEXUS_VideoFormat_eMax];
    uint16_t hdmi3DFormatsSupported[NEXUS_VideoFormat_eMax]; /* This is a bit-field value. Use NEXUS_HdmiOutput_3DStructure_xxx macros
                                                                to determine the supported 3D Structures for a particular video format. */
    bool audioCodecSupported[NEXUS_AudioCodec_eMax];
    unsigned maxAudioPcmChannels;                       /* The maximum number of PCM audio channels supported by the receiver -
                                                           2 for stereo, 6 or 8 typically for decoded multichannel data. */

    NEXUS_VideoFormat videoFormat;
    NEXUS_AspectRatio aspectRatio;
    NEXUS_ColorSpace colorSpace;

    NEXUS_AudioCodec audioFormat;
    unsigned audioSamplingRate; /* in units of Hz */
    unsigned audioSamplingSize;
    unsigned audioChannelCount;

    /* Please see HDMI Specification 1.3a under Section 8.6 for details of Physical Addres.
     * Each node (A..D) represent an attached HDMI device on HDMI device tree. */
    uint8_t physicalAddressA;               /* Physical Address for HDMI node A */
    uint8_t physicalAddressB;               /* Physical Address for HDMI node B */
    uint8_t physicalAddressC;               /* Physical Address for HDMI node C */
    uint8_t physicalAddressD;               /* Physical Address for HDMI node D */

    NEXUS_HdmiOutputAutoLipsyncInfo autoLipsyncInfo;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	struct
	{
		bool bSupported;
		unsigned uSamplingRate;
	}audioInfo[NEXUS_AudioCodec_eMax];
#endif	
} NEXUS_HdmiOutputStatus;

/**
Summary:
Get current status of the HDMI transmitter
**/
NEXUS_Error NEXUS_HdmiOutput_GetStatus(
    NEXUS_HdmiOutputHandle output,
    NEXUS_HdmiOutputStatus *pStatus    /* [out] Status */
    );

/**
Summary:
Basic EDID data from the receiver device
**/
typedef struct NEXUS_HdmiOutputBasicEdidData
{
   uint8_t vendorID[2];  /* Vendor ID from Microsoft; compressed ASCII */
   uint8_t productID[2]; /* Product ID assigned by Vendor */
   uint8_t serialNum[4]; /* Serial # assigned by Vendor; may be undefined */
   uint8_t manufWeek;    /* Week of Manufture (1..53)          */
   uint16_t manufYear;   /* Year of Manufacture + 1990         */
   uint8_t edidVersion;  /* Version of Edid (version 1 or 2)   */
   uint8_t edidRevision; /* Revision of Edid 1.3 or 2.0        */
   uint8_t maxHorizSize; /* Max Horizontal Image size in cm.   */
   uint8_t maxVertSize;  /* Max Vertical Image size in cm.     */
   uint8_t extensions;   /* Number of 128 byte EDID extensions */

   uint8_t features;      /* Features are not supported; standby,suspend, etc */

} NEXUS_HdmiOutputBasicEdidData;

/**
Summary:
Vendor Specific Infoframe for HDMI output
**/
typedef struct NEXUS_HdmiOutputVendorSpecificInfoFrame
{
    uint8_t ieeeRegId[3];
    NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat hdmiVideoFormat;
    NEXUS_HdmiVendorSpecificInfoFrame_HDMIVIC hdmiVIC;
    NEXUS_HdmiVendorSpecificInfoFrame_3DStructure hdmi3DStructure;
    NEXUS_HdmiVendorSpecificInfoFrame_3DExtData hdmi3DExtData;
} NEXUS_HdmiOutputVendorSpecificInfoFrame;


/**
Summary:
Video information supported by the attached HDMI receiver
**/
typedef struct NEXUS_HdmiOutputSupportedVideoInfo
{
    uint8_t supportedVideoIDCode[64];
    uint8_t numSupportedVideoDescriptors;
} NEXUS_HdmiOutputSupportedVideoInfo;


/**
Summary:
Get basic EDID data from the receiver device
**/
NEXUS_Error NEXUS_HdmiOutput_GetBasicEdidData(
    NEXUS_HdmiOutputHandle output,
    NEXUS_HdmiOutputBasicEdidData *pData    /* [out] Basic Data */
    );

/**
Summary:
Container for a block of EDID data from the receiver device
**/
typedef struct NEXUS_HdmiOutputEdidBlock
{
    uint8_t data[128];
} NEXUS_HdmiOutputEdidBlock;

/**
Summary:
Get a block of EDID data from the receiver device
**/
NEXUS_Error NEXUS_HdmiOutput_GetEdidBlock(
    NEXUS_HdmiOutputHandle output,
    unsigned blockNum,
    NEXUS_HdmiOutputEdidBlock *pBlock    /* [out] Block of raw EDID data */
    );

/**
Summary:
Returns the abstract NEXUS_VideoOutput connector for this Interface.
The NEXUS_VideoOutput connector is added to a Display in order to route that Display's video to the output.

Description:
Used in NEXUS_Display_AddOutput
**/
NEXUS_VideoOutput NEXUS_HdmiOutput_GetVideoConnector( /* attr{shutdown=NEXUS_VideoOutput_Shutdown} */
    NEXUS_HdmiOutputHandle output
    );

/**
Summary:
Returns the abstract NEXUS_AudioOutput connector for this Interface.
The NEXUS_AudioOutput connector is added to an audio filter graph in order to route audio from that path to the output.

Description:
Used in NEXUS_AudioOutput_AddInput
**/
NEXUS_AudioOutput NEXUS_HdmiOutput_GetAudioConnector( /* attr{shutdown=NEXUS_AudioOutput_Shutdown} */
    NEXUS_HdmiOutputHandle output
    );


/**
Summary:
Send AVMute/AVUnMute to attached receivers to minimize video flashes, audio pops, etc.
during format changes, color space change, etc.

Description:
Use when changing video format
**/
NEXUS_Error NEXUS_HdmiOutput_SetAVMute(
    NEXUS_HdmiOutputHandle handle,
    bool mute
    );

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
NEXUS_Error NEXUS_HdmiOutput_SetAudioMute(NEXUS_HdmiOutputHandle hdmiOutput, bool mute);

/**
Summary:
Send AVBlank/AVUnBlank to attached receivers to minimize video flashes, audio pops, etc.
during format changes, color space change, etc.

Description:
Use when blocking av
**/
NEXUS_Error NEXUS_HdmiOutput_SetAVBlank(
	NEXUS_HdmiOutputHandle hdmiOutput,
	bool mute
	);
#endif

#if defined(HUMAX_PLATFORM_BASE)
NEXUS_Error NEXUS_HdmiOutput_SetPowerState(NEXUS_HdmiOutputHandle hdmiOutput, bool bPowerOn);
#endif


/**
Summary:
Get current Vendor Specific Info Frame
**/
void NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputVendorSpecificInfoFrame *pVendorSpecificInfoFrame /* [out] */
    );

/**
Summary:
Set up the Vendor Specific Info Frame

Description:
Used to send Vendor Specific Information
**/
NEXUS_Error NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputVendorSpecificInfoFrame *pVendorSpecificInfoFrame
    );


/**
Summary:
Get current AVI Info Frame
**/
NEXUS_Error NEXUS_HdmiOutput_GetAviInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiAviInfoFrame *pAviInfoFrame
    );

/**
Summary:
Set up the Auxiliary Video Information(AVI) InfoFrame

Description:
Used to send AVI Information
**/
NEXUS_Error NEXUS_HdmiOutput_SetAviInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiAviInfoFrame *pAviInfoFrame
    );

/**
Summary:
Get current Audio Info Frame
**/
NEXUS_Error NEXUS_HdmiOutput_GetAudioInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiAudioInfoFrame *pAudioInfoFrame
    );

/**
Summary:
Set up the Audio InfoFrame

Description:
Used to send Audio Information
**/
NEXUS_Error NEXUS_HdmiOutput_SetAudioInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiAudioInfoFrame *pAudioInfoFrame
    );


/**
Summary:
Enable/Disable the TMDS lines

Description:
Turn on/off the TMDS lines
**/
NEXUS_Error NEXUS_HdmiOutput_SetTmdsSignal(
    NEXUS_HdmiOutputHandle handle,
    bool tmdsEnable
    );

/**
Summary:
Get the current TMDS signal setting

Description:
This will return the last value passed into NEXUS_HdmiOutput_SetTmdsSignal
**/
void NEXUS_HdmiOutput_GetTmdsSignal(
    NEXUS_HdmiOutputHandle handle,
    bool *pTmdsEnable /* [out] */
    );


/**
Summary:
Get the video information supported by the current attached HDMI receiver

Description:
This will return the video information supported by the attached HDMI TV/monitors
**/
NEXUS_Error NEXUS_HdmiOutput_GetSupportedVideoInfo(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputSupportedVideoInfo *pSupportedVideoInfo /* [out] */
    );

#if defined(HUMAX_PLATFORM_BASE)
void NEXUS_HdmiOutput_HotplugCallback(void *pContext);
#endif

/**
Summary:
Set the current AV Info Frame 
**/
NEXUS_Error NEXUS_HdmiOutput_SetAVInfoFrame(
    NEXUS_HdmiOutputHandle handle,
    unsigned int AspectRatio,
    unsigned int ActiveFormat);
/**
Summary:
Get the current pre emphasis configuration 
**/
NEXUS_Error NEXUS_HdmiOutput_GetPreEmphasisConfiguration(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputPreEmphasisConfiguration *pPreEmphasisConfiguration /* [out] */
    );


/**
Summary:
Set new pre emphasis configurations
**/
NEXUS_Error NEXUS_HdmiOutput_SetPreEmphasisConfiguration(
    NEXUS_HdmiOutputHandle handle,
    const NEXUS_HdmiOutputPreEmphasisConfiguration *pPreEmphasisConfiguration
    );

/**
Summary:
HDMI Tx CRC data
**/
typedef struct NEXUS_HdmiOutputCrcData
{
    uint16_t crc;
} NEXUS_HdmiOutputCrcData;

/**
Summary:
Get HDMI Tx CRC data

Description:
You must set NEXUS_HdmiOutputSettings.crcQueueSize to a non-zero value (for example, 30) to capture data.
**/
NEXUS_Error NEXUS_HdmiOutput_GetCrcData(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiOutputCrcData *pData, /* attr{nelem=numEntries;nelem_out=pNumEntriesReturned} [out] array of crc data structures */
    unsigned numEntries,
    unsigned *pNumEntriesReturned /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_HDMI_OUTPUT_H__ */

