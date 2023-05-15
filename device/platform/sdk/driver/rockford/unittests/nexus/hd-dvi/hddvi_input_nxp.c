/***************************************************************************
*     (c)2004-2009 Broadcom Corporation
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
* $brcm_Workfile: hddvi_input_nxp.c $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 1/10/12 4:48p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/hd-dvi/hddvi_input_nxp.c $
* 
* Hydra_Software_Devel/5   1/10/12 4:48p katrep
* SW7429-1:updated i2c channel for 7241
* 
* Hydra_Software_Devel/4   11/14/11 4:48p katrep
* SW7231-387:enabled hdmi input, connected hd-dvi input to window
* 
* Hydra_Software_Devel/3   10/21/11 7:25p katrep
* SW7231-387:bringup nxp drivers
* 
* Hydra_Software_Devel/2   9/29/11 2:24p katrep
* SW7231-387:add i2c
* 
* Hydra_Software_Devel/1   9/29/11 1:38p katrep
* SW7231-387:add hd-dvi with external nxp
* 
*
***************************************************************************/

//
//
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_hddvi_input.h"
#include "nexus_component_output.h"
#include "nexus_video_window.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include "nexus_i2c.h"
NEXUS_I2cHandle g_i2c= NULL;
#include "nxp/tmdlHdmiRx_Functions.h"
/* drivers includes */
#include "tmdlHdmiRx.h"
#include "tmdlHdmiRx_cfg.h"
#include "tmdlHdmiRx_IW.h"
#include "bkni.h"




BDBG_MODULE(hddvi_input);


/*============================================================================*/
/*                   PROJECT INCLUDE FILES                                    */
/*============================================================================*/


/* drivers infrastructure */
/* #include "I2C.h" */

/*============================================================================*/
/*                   INTERNAL TYPES                                           */
/*============================================================================*/

/* Enum listing the color spaces indicated in AVI infoframe (byte 1, field Y1:Y0) */
typedef enum
{
    TMEX_APPLI_COLORSPACE_RGB,        /* Color space is RGB */
    TMEX_APPLI_COLORSPACE_YCBCR_422,  /* Color space is YCbCr 4:2:2 */
    TMEX_APPLI_COLORSPACE_YCBCR_444   /* Color space is YCbCr 4:4:4 */
} tmExAppliColorSpace_t;

/* Enum listing the colorimetry indicated in AVI infoframe (byte 2, field C1:C0) */
typedef enum
{
    TMEX_APPLI_COLORIMETRY_NO_DATA,
    TMEX_APPLI_COLORIMETRY_SMPTE170_ITU601,
    TMEX_APPLI_COLORIMETRY_ITU709,
    TMEX_APPLI_COLORIMETRY_XVYCC
} tmExAppliColorimetry_t;

/* Enum listing the resolution types, it can be useful when colorimetry is not indicated in the AVI infoframe */
typedef enum
{
    TMEX_APPLI_SDTV_RESOLUTION,
    TMEX_APPLI_HDTV_RESOLUTION,
    TMEX_APPLI_PC_RESOLUTION
} tmExAppliResolutionType_t;

typedef struct
{
    tmExAppliColorSpace_t       colorSpace;
    tmExAppliColorimetry_t      colorimetry;
    tmExAppliResolutionType_t   resolutionType;
} tmExAppliColorSpaceManagement_t;

/*============================================================================*/
/*                   INTERNAL DEFINITIONS                                     */
/*============================================================================*/
/* Version of the SW driver */
#define VERSION_COMPATIBILITY 0
#define VERSION_MAJOR         3
#define VERSION_MINOR         3

/* Conversion matrices */
#define TMEX_APPLI_CONVERSIONMATRIX_RGBLimited_YCbCr601 TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_1
#define TMEX_APPLI_CONVERSIONMATRIX_YCbCr601_RGBLimited TMDL_HDMIRX_CONVERSIONMATRIX_PREDEFINED_2

tmdlHdmiRxColorMatrixCoefs_t TMEX_APPLI_CONVERSIONMATRIX_YCbCr709_RGBLimited = { -256, -2048, -2048,
                                                                                 4096, -1875,  -750,
                                                                                 4096,  6307,     0,
                                                                                 4096,     0,  7431,
                                                                                  256,   256,   256
                                                                               };

/* EDID */
/* 1080p60&50 xvYCC */
const UInt8 edid1080p6050xvYCC[256] =
{ /* Block 0 */
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3B, 0x10, 0x0A, 0x4E, 0x01, 0x00, 0x00, 0x00,
  0x26, 0x11, 0x01, 0x03, 0x80, 0x00, 0x00, 0x00, 0x1A, 0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26,
  0x0F, 0x50, 0x54, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28,
  0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x44, 0x45, 0x4D,
  0x4F, 0x20, 0x54, 0x44, 0x41, 0x31, 0x39, 0x39, 0x37, 0x38, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x32,
  0x3C, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFE,
  0x00, 0x31, 0x30, 0x38, 0x30, 0x70, 0x36, 0x30, 0x26, 0x35, 0x30, 0x78, 0x76, 0x43, 0x01, 0xA1,

                                        /* source physical address (2 bytes) */
                                        /* These bytes are replaced by sPAEdid[0] for input A, sPAEdid[1] for input B, */
                                        /* sPAEdid[2] for input C, sPAEdid[3] for input D */
                                        /* The position depends on the sPAOffsetEdid value */
  /* Block 1                                        ^     ^ */
  /*                                                |     | */
  0x02, 0x03, 0x2F, 0xF0, 0x6C, 0x03, 0x0C, 0x00, 0x10, 0x00, 0xB8, 0x2D, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x4B, 0x04, 0x13, 0x10, 0x1F, 0x03, 0x12, 0x05, 0x14, 0x07, 0x16, 0x01, 0x29, 0x0F, 0x17,
  0x07, 0x5F, 0x7F, 0x01, 0x67, 0x7F, 0x00, 0x83, 0x2F, 0x00, 0x00, 0xE3, 0x05, 0x03, 0x01, 0x01,
  0x1D, 0x00, 0xBC, 0x52, 0xD0, 0x1E, 0x20, 0xB8, 0x28, 0x55, 0x40, 0xC4, 0x8E, 0x21, 0x00, 0x00,
  0x1E, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0xC4, 0x8E, 0x21,
  0x00, 0x00, 0x1E, 0x02, 0x3A, 0x80, 0xD0, 0x72, 0x38, 0x2D, 0x40, 0x10, 0x2C, 0x45, 0x80, 0xC4,
  0x8E, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96,
  0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81
};

/* Source Physical Adresses for each input */
const UInt16 sPAEdid[4] = { 0x1000, 0x2000, 0x3000, 0x4000 };

const UInt8 sPAOffsetEdid = (UInt8) 0x08U;  /* Offset of the first SPA byte inside EDID block 1 */

/* Interrupt line */
#define IT_LINE_ACTIVE 0

/*============================================================================*/
/*                   EXTERNAL PROTOTYPE                                       */
/*============================================================================*/

/*============================================================================*/
/*                   INTERNAL PROTOTYPE                                       */
/*============================================================================*/

/* Callbacks */
static void digitalActivityCallback (tmdlHdmiRxEvent_t event,
                       tmdlHdmiRxInput_t input,
                       tmdlHdmiRxSyncType_t syncType);
static void infoFrameCallback(tmdlHdmiRxEvent_t  event,
                       UInt8             *data,
                       UInt8              size);
static void eventCallback (tmdlHdmiRxEvent_t  event);

/* Other */
tmErrorCode_t tmdlHdmiRxExampleAppliInit(void);

static void readPortInterrupt(int *GPIO_input);
static void storeResolution(tmdlHdmiRxResolutionID_t resolutionID);
static tmErrorCode_t tmExAppliConfigureConversion(void);



/* ARM7 initialization */
/* void Infra_Init(void); */
/*void Serial_Init(void);*/

/* Function that prints the returned error codes */
void PRINTIF(tmErrorCode_t errCode, Int lineNumber);


/*============================================================================*/
/*                   GLOBAL VARIABLES                                  */
/*============================================================================*/
/* Instance variable of the HdmiRx device library */
tmInstance_t   gDlHdmiRxInstance;

/* Instance setup variable of the HdmiRx device library */
tmdlHdmiRxInstanceSetupInfo_t gDlHdmiRxSetupInfo;

/* Blanking codes values, they depend on the output color space (RGB or YCbCr) */
tmdlHdmiRxBlankingCodes_t gRGBBlankingCode = {64,64,64};
tmdlHdmiRxBlankingCodes_t gYCbCrBlankingCode = {64,512,512};

/* Color space information from AVI infoframe must be memorized in order to choose the proper conversion */
tmExAppliColorSpaceManagement_t gColorSpaceManagement;

/*============================================================================*/
/**
    \brief        Initialize all the required components

    \param        None

    \return     - TM_OK  : the call was successful
                - Any others: failed

 ******************************************************************************/
tmErrorCode_t tmdlHdmiRxExampleAppliInit()
{   
    tmSWVersion_t   versionNr;

    versionNr.compatibilityNr = VERSION_COMPATIBILITY;
    versionNr.majorVersionNr = VERSION_MAJOR;
    versionNr.minorVersionNr = VERSION_MINOR;

    printf("Example application \t: %d.%02d\n\n",
            (int)versionNr.majorVersionNr,(int)versionNr.minorVersionNr);

    /* Init I2c for ARM7 */
    /* Init_i2c(); */
    printf("I2C initialized\n");

    /* Initialization of the variables */
    gColorSpaceManagement.colorSpace     = TMEX_APPLI_COLORSPACE_RGB;
    gColorSpaceManagement.colorimetry    = TMEX_APPLI_COLORIMETRY_NO_DATA;
    gColorSpaceManagement.resolutionType = TMEX_APPLI_SDTV_RESOLUTION;

    /*--------------------------------------------------------*/
    /* Initialize the tmdlHdmiRx component                    */
    /*--------------------------------------------------------*/

    /* Open an instance of HdmiRx driver and provides the instance number to the caller */
    /* It recovers the configuration of the device librray and sets the I2C read/write functions */
    /* from the CFG file. */
    PRINTIF(tmdlHdmiRxOpen(&gDlHdmiRxInstance), __LINE__);

    /* Register the callbacks */
    /* Three types of callbacks can be registered : */
    /*  - input activity related callback */
    /*  - data related callback (infoframes, packets, etc.) */
    /*  - general information callback */
    /* A null pointer means that no callback are registered */

    /* Parameters: */
    /* 1: Instance identifier */
    /* 2: pAnalogActivityCallback   Pointer to the callback function that will
                                    handle activity related events on analog
                                    inputs 
                                    => Not used on the TDA19978 because there is no analog input */
    /* 3: pDigitalActivityCallback  Pointer to the callback function that will
                                    handle activity related events on digital
                                    inputs */
    /* 4: pDataCallback             Pointer to the callback function that will
                                    handle data related events (typically infoframes) */
    /* 5: pInfoCallback             Pointer to the callback function that will
                                    handle new informative events (input locked ...)*/
    PRINTIF(tmdlHdmiRxRegisterCallbacks(gDlHdmiRxInstance,
                                        Null, 
                                        (ptmdlHdmiRxActivityCallback_t) digitalActivityCallback, 
                                        (ptmdlHdmiRxDataCallback_t)     infoFrameCallback,
                                        (ptmdlHdmiRxInfoCallback_t)     eventCallback 
                                       ), __LINE__);

    /* This function allows basic instance configuration like enabling HDCP, */
    /* choosing HDCP encryption mode or enabling HDCP repeater mode and internal EDID */
    gDlHdmiRxSetupInfo.hdcpEnable = True;
    gDlHdmiRxSetupInfo.hdcpMode = TMDL_HDMIRX_HDCPMODE_AUTO;           
    gDlHdmiRxSetupInfo.hdcpRepeaterEnable = False;
    gDlHdmiRxSetupInfo.internalEdid = True;

    PRINTIF(tmdlHdmiRxInstanceSetup(gDlHdmiRxInstance, &gDlHdmiRxSetupInfo), __LINE__);

    /* The internal EDIDs are enabled, so we can now load EDID data into embedded EDID memory */ 
    PRINTIF(tmdlHdmiRxLoadEdidData(gDlHdmiRxInstance, (UInt8 *) edid1080p6050xvYCC, (UInt16 *) sPAEdid, sPAOffsetEdid), __LINE__);

    /*-----------------------------------------------------------------------------*/
    /* Set default datapath                                                        */
    /*-----------------------------------------------------------------------------*/

    /* Select an input */
    /* Auto digital means that we use the automatic mode of chip, the input selection is based on a priority */
    /* input A > input B > input C > input D */
    PRINTIF(tmdlHdmiRxSelectInput(gDlHdmiRxInstance, TMDL_HDMIRX_INPUT_AUTO_DIGITAL), __LINE__);

    /* Set bypass of the color space matrix => no conversion */
    /* Blanking codes values depend on the output color space (RGB or YCbCr), if Null they are not set */
    PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance, TMDL_HDMIRX_CONVERSIONMATRIX_BYPASS, Null, Null), __LINE__);
    
    /* Set output mode to 4:4:4, blanking and timing code are not inserted, clock mode is single edge */
    PRINTIF(tmdlHdmiRxSetVideoOutputFormat(gDlHdmiRxInstance, TMDL_HDMIRX_OUTPUTFORMAT_444, False, False, TMDL_HDMIRX_OUTCLKMODE_SINGLE_EDGE), __LINE__);
    
    /* Route the video port according to board configuration */
    /* Video port resolution must always be 10 bits */
    /* Here, Blue is output on video port A, Green is output on video port B, Red is output on video port C */
    PRINTIF(tmdlHdmiRxSetVideoOutputRouting(gDlHdmiRxInstance, 
                                        TMDL_HDMIRX_VPRESOLUTION_10_BITS, 
                                        TMDL_HDMIRX_OUTPUTSIGNAL_B,
                                        TMDL_HDMIRX_OUTPUTSIGNAL_G,
                                        TMDL_HDMIRX_OUTPUTSIGNAL_R), __LINE__);

    /* Set audio to I2S 32 bits format and audio sysclk frequency to 256FS */
    PRINTIF(tmdlHdmiRxSetAudioOutputFormat(gDlHdmiRxInstance, 
                                           TMDL_HDMIRX_AUDIOFORMAT_I2S32,
                                           TMDL_HDMIRX_AUDIOSYSCLK_256FS), __LINE__);

    /* Enable detection of all the event */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_ACTIVITY_DETECTED), __LINE__);                 /* New activity has been detected */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_ACTIVITY_LOST), __LINE__);                     /* Activity has been lost */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_ISRC1_PACKET_RECEIVED), __LINE__);             /* ISRC1 packet has been received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_ISRC2_PACKET_RECEIVED), __LINE__);             /* ISRC2 packet has been received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_ACP_PACKET_RECEIVED), __LINE__);               /* Audio Content Protection packet has been received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED), __LINE__);            /* Auxiliary Video Information infoframe has been received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED), __LINE__);            /* Source product Description infoframe has been received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED), __LINE__);            /* Audio infoframe has been received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED), __LINE__);            /* MPEG Source infoframe has been received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_ENCRYPTION_DETECTED), __LINE__);               /* Indicates that the active digital input is receiving HDCP encrypted data */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_EESS_HDCP_DETECTED), __LINE__);                /* Indicates that the active digital input is receiving data in EESS mode */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_GBD_PACKET_RECEIVED), __LINE__);               /* Indicates a GAMUT packet has been received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AUDIO_SAMPLE_PACKET_DETECTED), __LINE__);      /* Indicates that audio samples packets are detected */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AUDIO_HBR_PACKET_DETECTED), __LINE__);         /* Indicates that audio HBR packets are detected */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AUDIO_OBA_PACKET_DETECTED), __LINE__);         /* Indicates that audio OBA packets are detected */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AUDIO_DST_PACKET_DETECTED), __LINE__);         /* Indicates that audio DST packets are detected */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_HDMI_DETECTED), __LINE__);                     /* Indicates that HDMI stream is detected */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_DVI_DETECTED), __LINE__);                      /* Indicates that DVI stream is detected */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_NO_ENCRYPTION_DETECTED), __LINE__);            /* Indicates the the active digital input is not receiving HDCP encrypted data */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_OESS_HDCP_DETECTED), __LINE__);                /* Indicates that the active digital input is receiving data in OESS mode */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AVI_AVMUTE_ACTIVE), __LINE__);                 /* AV mute active received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AVI_AVMUTE_INACTIVE), __LINE__);               /* AV mute inactive received */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_AUDIO_SAMPLE_FREQ_CHANGED), __LINE__);         /* Indicates that audio sampling frequency has changed */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_INPUT_LOCKED), __LINE__);                      /* Indicates that clocks are locked on the new active input */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_REPEATER), __LINE__);                          /* Indicates that the HDCP repeater authentication can start */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS), __LINE__);            /* Indicates that deep color mode is 24 bits */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS), __LINE__);            /* Indicates that deep color mode is 30 bits */
    PRINTIF(tmdlHdmiRxEnableEvent(gDlHdmiRxInstance, TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS), __LINE__);            /* Indicates that deep color mode is 36 bits */

    return TM_OK;
}

/*============================================================================*/
/**
    \brief          Callback on digital activity detection
                    Used to select the correct input and to display 
                    inforamtion on the terminal.

    \param event    Activities Event raised by the interrupts
    \param input    Input that raises the event
    \param syncType Type of synchronization used by the input
                    (not relevant in our case because HDMI always uses separated sync)

    \return         None

 ******************************************************************************/
static void digitalActivityCallback (tmdlHdmiRxEvent_t event,
                       tmdlHdmiRxInput_t input,
                       tmdlHdmiRxSyncType_t syncType)
{
    syncType=syncType;
    switch (input)
    {
        case TMDL_HDMIRX_INPUT_HDMI_A: /* New activity has been detected */
            printf("\n*** INPUT HDMI A: ");
            break;
        case TMDL_HDMIRX_INPUT_HDMI_B: /* New activity has been detected */
            printf("\n*** INPUT HDMI B: ");
            break;
        case TMDL_HDMIRX_INPUT_HDMI_C: /* New activity has been detected */
            printf("\n*** INPUT HDMI C: ");
            break;
        case TMDL_HDMIRX_INPUT_HDMI_D: /* New activity has been detected */
            printf("\n*** INPUT HDMI D: ");
            break;
        case TMDL_HDMIRX_INPUT_AUTO_ANALOG:
        case TMDL_HDMIRX_INPUT_AUTO_DIGITAL:
        case TMDL_HDMIRX_INPUT_ANALOG_1:
        case TMDL_HDMIRX_INPUT_ANALOG_2:
        case TMDL_HDMIRX_INPUT_ANALOG_3:
        case TMDL_HDMIRX_INPUT_TEST_480P:
        case TMDL_HDMIRX_INPUT_TEST_576P: 
        default:
        break;

    }

    if(event == TMDL_HDMIRX_ACTIVITY_DETECTED)
    {
        printf("Digital ACTIVITY DETECTED ***\n");
    }
    else if(event == TMDL_HDMIRX_ACTIVITY_LOST)
    {
        printf("Digital ACTIVITY LOST ***\n");
    }
}

/*============================================================================*/
/**
    \brief          Callback on infoframe reception
                    Used to display information on the terminal.

    \param  event   Event raised corresponding to infoframe type received
    \param  data    content of the infoframe received
    \param  size    size of the infoframe received

    \return         None

 ******************************************************************************/
static void infoFrameCallback(tmdlHdmiRxEvent_t  event,
                       UInt8             *intData,
                       UInt8              size)
{
    tmdlHdmiRxAVIInfoframe_t*   pAVIInfoFrame;
    size=size;
    switch (event)
    {
        case TMDL_HDMIRX_ISRC1_PACKET_RECEIVED:
            printf("\nINFOFRAME: ISRC1 received\n");
            break;

        case TMDL_HDMIRX_ISRC2_PACKET_RECEIVED: 
            printf("\nINFOFRAME: ISRC2 received\n");
            break;

        case TMDL_HDMIRX_ACP_PACKET_RECEIVED: 
            printf("\nINFOFRAME: ACP received\n");
            break;

        case TMDL_HDMIRX_AVI_INFOFRAME_RECEIVED:
            printf("\nINFOFRAME: AVI received\n");
            pAVIInfoFrame = (tmdlHdmiRxAVIInfoframe_t*) intData;

            if (pAVIInfoFrame->packetType != 0x82)
            {
                printf("\nINFOFRAME AVI: wrong packet type !!!\n");
            }
            
            /* Memorize the color indicator */
            switch (pAVIInfoFrame->colorIndicator)
            {
                case 0:
                    gColorSpaceManagement.colorSpace = TMEX_APPLI_COLORSPACE_RGB; 
                    printf("color space = RGB\n");
                    break;
                case 1:
                    gColorSpaceManagement.colorSpace = TMEX_APPLI_COLORSPACE_YCBCR_422;
                    printf("color space = YCbCr 422\n");
                    break;
                case 2:
                    gColorSpaceManagement.colorSpace = TMEX_APPLI_COLORSPACE_YCBCR_444;
                    printf("color space = YCbCr 444\n");
                    break;
                case 3:
                    gColorSpaceManagement.colorSpace = TMEX_APPLI_COLORSPACE_YCBCR_444;
                    printf("color space = Future\n");
                    break;
                default:
                    gColorSpaceManagement.colorSpace = TMEX_APPLI_COLORSPACE_RGB; 
                    printf("Wrong color indicator\n");
            }

            /* Memorize the colorimetry */
            switch (pAVIInfoFrame->colorimetry)
            {
                case 0:
                    gColorSpaceManagement.colorimetry = TMEX_APPLI_COLORIMETRY_NO_DATA;
                    printf("colorimetry: No data\n");
                    break;
                case 1:
                    gColorSpaceManagement.colorimetry = TMEX_APPLI_COLORIMETRY_SMPTE170_ITU601;
                    printf("colorimetry: SMPTE 170M / ITU601\n");
                    break;
                case 2:
                    gColorSpaceManagement.colorimetry = TMEX_APPLI_COLORIMETRY_ITU709;
                    printf("colorimetry: ITU709\n");
                    break;
                case 3:
                    gColorSpaceManagement.colorimetry = TMEX_APPLI_COLORIMETRY_XVYCC;
                    printf("colorimetry: extended (xvYCC)\n");
                    break;
                default:
                    gColorSpaceManagement.colorimetry = TMEX_APPLI_COLORIMETRY_NO_DATA;
                    printf("Wrong colorimetry\n");
            }

            /* Configure the receiver with the new color space */
            PRINTIF(tmExAppliConfigureConversion(), __LINE__);
            break;

        case TMDL_HDMIRX_SPD_INFOFRAME_RECEIVED:
            printf("\nINFOFRAME: SPD received\n");
            break;

        case TMDL_HDMIRX_AUD_INFOFRAME_RECEIVED:
            printf("\nINFOFRAME: AUD received\n");
            break;

        case TMDL_HDMIRX_MPS_INFOFRAME_RECEIVED:
            printf("\nINFOFRAME: MPEG received\n");
            break;

        case TMDL_HDMIRX_GBD_PACKET_RECEIVED:
            printf("\nGamut metadata packets received\n");
            break;

        default:
            printf("erroneous event for infoframe callback\n");
            break;
    }
}


/*============================================================================*/
/**
    \brief          Callback on event reception
                    Used to handle informative events.

    \param          event: event raised by the device library

    \return         None

 ******************************************************************************/
static void eventCallback (tmdlHdmiRxEvent_t  event)
{
    tmdlHdmiRxResolutionID_t     resolutionID = TMDL_HDMIRX_VIDEORES_UNKNOWN;
    tmdlHdmiRxAudioDstRate_t     dstRate;
    tmdlHdmiRxAudioSampleFreq_t  audioFreq;

    switch (event)
    {
        case TMDL_HDMIRX_INPUT_LOCKED:
            printf("Automatic INPUT LOCKED\n");

            /* Detect the video resolution */
            PRINTIF(tmdlHdmiRxDetectResolution(gDlHdmiRxInstance, &resolutionID), __LINE__);

            /* Display on the terminal the name of the detected resolution */
            printf("Automatic detection of resolution: ");
            storeResolution(resolutionID);
            printf("\n");

            /* Configure the active input to the given resolution */
            PRINTIF(tmdlHdmiRxConfigureInput(gDlHdmiRxInstance, resolutionID), __LINE__);

            /* On "input locked" event, RGB color space is forced (the AVI infoframe is not received yet at this moment) */
            /* If an AVI infoframe is received later, the color space will be re-configured in the AVI infoframe callback */
            gColorSpaceManagement.colorSpace  = TMEX_APPLI_COLORSPACE_RGB;
            gColorSpaceManagement.colorimetry = TMEX_APPLI_COLORIMETRY_NO_DATA;

            /* Configure the receiver with the new color space */
            PRINTIF(tmExAppliConfigureConversion(), __LINE__);
            break;

        case TMDL_HDMIRX_HDMI_DETECTED:
            printf("\nHDMI mode\n");
            break;

        case TMDL_HDMIRX_DVI_DETECTED:
            printf("\nDVI mode\n");
            /* RGB mandatory in DVI mode and there are no infoframes */
            gColorSpaceManagement.colorSpace  = TMEX_APPLI_COLORSPACE_RGB;
            gColorSpaceManagement.colorimetry = TMEX_APPLI_COLORIMETRY_NO_DATA;

            /* Configure the receiver with the new color space */
            PRINTIF(tmExAppliConfigureConversion(), __LINE__);
            break;

        case TMDL_HDMIRX_ENCRYPTION_DETECTED:
            printf("\nHDCP encrypted \n");
            break;

        case TMDL_HDMIRX_NO_ENCRYPTION_DETECTED:
            printf("\nnot HDCP encrypted \n");
            break;

        case TMDL_HDMIRX_EESS_HDCP_DETECTED:
            printf("EESS mode\n");
            break;

        case TMDL_HDMIRX_OESS_HDCP_DETECTED:
            printf("OESS mode\n");
            break;

        case TMDL_HDMIRX_AVI_AVMUTE_ACTIVE:
            printf("AV mute active\n\n");
            break;

        case TMDL_HDMIRX_AVI_AVMUTE_INACTIVE:
            printf("AV Mute inactive\n\n ");
            break;

        case TMDL_HDMIRX_DEEP_COLOR_MODE_24BITS:
            printf("Deep color mode: 24 bits\n");
            break;

        case TMDL_HDMIRX_DEEP_COLOR_MODE_30BITS:
            printf("Deep color mode: 30 bits\n");
            break;

        case TMDL_HDMIRX_DEEP_COLOR_MODE_36BITS:
            printf("Deep color mode: 36 bits\n");
            break;

        case TMDL_HDMIRX_AUDIO_SAMPLE_PACKET_DETECTED:
            printf("Audio sample packets\n");
            break;

        case TMDL_HDMIRX_AUDIO_HBR_PACKET_DETECTED:
            printf("Audio HBR packets\n");
            break;

        case TMDL_HDMIRX_AUDIO_OBA_PACKET_DETECTED:
            printf("Audio OBA packets\n");
            break;

        case TMDL_HDMIRX_AUDIO_DST_PACKET_DETECTED:
            printf("Audio DST packets\n");
            break;

        case TMDL_HDMIRX_AUDIO_SAMPLE_FREQ_CHANGED:
            /* Get the new audio sampling frequency */
            PRINTIF(tmdlHdmiRxGetAudioFrequency(gDlHdmiRxInstance, &dstRate, &audioFreq), __LINE__);

            switch(audioFreq)
            {
                case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_ERROR:
                    printf("calculation error\n");
                    break;
                case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_32_KHZ:
                    printf("Audio frequency: 32 kHz\n");
                    break;
                case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_44_KHZ:
                    printf("Audio frequency: : 44.1 kHz\n");
                    break;
                case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_48_KHZ:
                    printf("Audio frequency: : 48 kHz\n");
                    break;
                case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_88_KHZ:
                    printf("Audio frequency: : 88.2 kHz\n");
                    break;
                case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_96_KHZ:
                    printf("Audio frequency: : 96 kHz\n");
                    break;
                case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_176_KHZ:
                    printf("Audio frequency: : 176.4 kHz\n");
                    break;
                case TMDL_HDMIRX_AUDIO_SAMPLEFREQ_192_KHZ:
                    printf("Audio frequency: : 192 kHz\n");
                    break;
                default:
                    printf(": wrong value\n");
            }
            break;

        case TMDL_HDMIRX_REPEATER:
            printf("Repeater state 5 reached\n");
            break;

        default:
            printf("erroneous event for standard event callback\n");
            break;
    }
}

/*============================================================================*/
/**
    \brief        Read the Rx interrupt pin status on microcontroller
    
    \param        Integer to recover GPIO status

    \return       None

 ******************************************************************************/
void readPortInterrupt(int *GPIO_input)
{
    *GPIO_input = *GPIO_input ;
   /* *GPIO_input = (IO0PIN>>16) & 0x01; */
    
}

/*============================================================================*/
/**
    \brief        print resolution on the terminal

    \param        resolutionID: Resolution to display

    \return     - TM_OK  : the call was successful
                - Any others: failed

 ******************************************************************************/
static void storeResolution(tmdlHdmiRxResolutionID_t resolutionID)
{
    switch (resolutionID){
        /* 60 Hz TV*/
        case TMDL_HDMIRX_VIDEORES_720_480p_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_SDTV_RESOLUTION;
            printf("720x480p59/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1280_720p_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1280x720p59/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1920_1080i_60HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1920x1080i59/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_720_480i_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_SDTV_RESOLUTION;
            printf("720x480i59/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1920_1080p_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1920x1080p59/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1920_1080p_30HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1920x1080p29/30Hz");
            break;

        /* 50 Hz TV*/
        case TMDL_HDMIRX_VIDEORES_720_576p_50HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_SDTV_RESOLUTION;
            printf("720x576p/50Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1280_720p_50HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1280x720p/50Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1920_1080i_50HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1920x1080i/50Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_720_576i_50HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_SDTV_RESOLUTION;
            printf("720x576i/50Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1920_1080p_50HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1920x1080p/50Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1920_1080p_25HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1920x1080p/25Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1920_1080p_24HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_HDTV_RESOLUTION;
            printf("1920x1080p23/24Hz");
            break;

        /* 60 Hz PC*/
        case TMDL_HDMIRX_VIDEORES_640_480p_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("VGA 640x480p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_800_600p_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("SVGA 800x600p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1024_768p_60HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("XGA 1024x768p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1280_768p_60HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("WXGA 1280x768p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1280_960p_60HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("1280x960p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1280_1024p_60HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("SXGA 1280x1024p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1440_900p_60HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("1440x900p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1600_1200p_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("UGA 1600x1200p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1680_1050p_60HZ_RB:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("WSXA 1680x1050p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1920_1200p_60HZ_RB:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("WUXGA 1920x1200p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1360_768p_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("1360x768p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("1400x1050p/60Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1400_1050p_60HZ_RB:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("1400x1050p/60Hz reduced blanking");
            break;

        /* 70 Hz PC */
        case TMDL_HDMIRX_VIDEORES_1024_768p_70HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("XGA 1024x768p/70Hz");
            break;

        /* 72 Hz PC */
        case TMDL_HDMIRX_VIDEORES_640_480p_72HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("VGA 640x480p/72Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_800_600p_72HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("SVGA 800x600p/72Hz");
            break;

        /* 75 Hz PC*/
        case TMDL_HDMIRX_VIDEORES_640_480p_75HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("VGA 640x480p/75Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_800_600p_75HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("SVGA 800x600p/75Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1024_768p_75HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("XGA 1024x768p/75Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1280_1024p_75HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("SXGA 1280x1024p/75Hz");
            break;

        /* 85 HZ PC */
        case TMDL_HDMIRX_VIDEORES_640_350p_85HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("640x350p/85Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_640_400p_85HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("640x400p/85Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_720_400p_85HZ:
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("720x400p/85Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_640_480p_85HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("VGA 640x480p/85Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_800_600p_85HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf(" SVGA 800x600p/85Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1024_768p_85HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("XGA 1024x768p/85Hz");
            break;
        case TMDL_HDMIRX_VIDEORES_1280_1024p_85HZ :
            gColorSpaceManagement.resolutionType = TMEX_APPLI_PC_RESOLUTION;
            printf("SXGA 1280x1024p/85Hz");
            break;

        default:
            printf("Unknown resolution");
            break;
    }
}

/*============================================================================*/
/**
    \brief        Configure the conversion in the receiver

    \param        None

    \return     - TM_OK  : the call was successful
                - Any others: failed

 ******************************************************************************/
static tmErrorCode_t tmExAppliConfigureConversion(void)
{
    /* In this example application, the chosen output color space is RGB 4:4:4 */

    /* Choose the proper matrix depending on the input color space */
    switch (gColorSpaceManagement.colorSpace)
    {
        case TMEX_APPLI_COLORSPACE_RGB:
            /* Input color space is RGB */
            /* As output color space is also RGB => no conversion to do */
            /* The blanking codes depend on the output color space => it will always be RGB in our case */
            PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance,
                    TMDL_HDMIRX_CONVERSIONMATRIX_BYPASS,
                    Null,
                    &gRGBBlankingCode), __LINE__);

            if(gColorSpaceManagement.resolutionType == TMEX_APPLI_PC_RESOLUTION)
            {
                printf("PC format => RGB full range\n");
            }
            else
            {
                printf("TV format => RGB limited range\n");
            }
            break;

        case TMEX_APPLI_COLORSPACE_YCBCR_444:
        case TMEX_APPLI_COLORSPACE_YCBCR_422:
            /* Input color space is YCbCr */
            /* As output color space is RGB => a conversion must be made */
            /* The blanking codes depend on the output color space => it will always be RGB in our case */
            
            /* In this case, SW must check the colorimetry field of the AVI infoframe */
            switch(gColorSpaceManagement.colorimetry)
            {
                case TMEX_APPLI_COLORIMETRY_NO_DATA:
                case TMEX_APPLI_COLORIMETRY_XVYCC:
                    /* The colorimetry is not specify, the conversion depends on the resolution: */
                    /* YCbCr ITU709 for a HD resolution and YCbCr ITU601 for a SD resolution. */
                    /* This case should not happen for a PC resolution because a PC format should */
                    /* always be in RGB */ 
                    if(gColorSpaceManagement.resolutionType == TMEX_APPLI_HDTV_RESOLUTION)
                    {
                        /* YCbCr ITU709 is used */
                        PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance,
                                TMDL_HDMIRX_CONVERSIONMATRIX_CUSTOM,
                                &TMEX_APPLI_CONVERSIONMATRIX_YCbCr709_RGBLimited,
                                &gRGBBlankingCode), __LINE__);
                    }
                    else
                    {
                        /* YCbCr ITU601 is used */
                        PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance,
                                TMEX_APPLI_CONVERSIONMATRIX_YCbCr601_RGBLimited,
                                Null,
                                &gRGBBlankingCode), __LINE__);
                    }
                    break;

                case TMEX_APPLI_COLORIMETRY_SMPTE170_ITU601:
                    /* YCbCr ITU601 is used */
                    PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance,
                            TMEX_APPLI_CONVERSIONMATRIX_YCbCr601_RGBLimited,
                            Null,
                            &gRGBBlankingCode), __LINE__);
                    break;

                case TMEX_APPLI_COLORIMETRY_ITU709:
                    /* YCbCr ITU709 is used */
                    PRINTIF(tmdlHdmiRxSetColorConversionMatrix(gDlHdmiRxInstance,
                            TMDL_HDMIRX_CONVERSIONMATRIX_CUSTOM,
                            &TMEX_APPLI_CONVERSIONMATRIX_YCbCr709_RGBLimited,
                            &gRGBBlankingCode), __LINE__);
                    break;
                default:
                    printf("Swith problem\n");
            }
            break;
        default:
            printf("Swith problem\n");
    }

    return TM_OK;
}




/*============================================================================*/
/**
    \brief   Main of the application

    \param   None
                                
    \return  Integer

 ******************************************************************************/
#if 0
int main(void) 
{
    tmErrorCode_t errCode = TM_OK;
    int           interruption_port = 0;

    
    /* Initialize the application */
    errCode = tmdlHdmiRxExampleAppliInit();
    PRINTIF(errCode, __LINE__);

    while (1)
    {
        /* Check the status every 50 ms */
        PRINTIF(tmdlHdmiRxIWWait(50), __LINE__);
        

        /* Read the interruption status*/
        readPortInterrupt(&interruption_port);

        if (interruption_port == IT_LINE_ACTIVE)
        {
            /* Make device library handle an incoming interrupt. This function is
               used by application to tell the device library that the hardware
               sent an interrupt */
            errCode = tmdlHdmiRxHandleInterrupt(gDlHdmiRxInstance);
            PRINTIF(errCode, __LINE__);
        }

        errCode = tmdlHdmiRxHeartBeat(gDlHdmiRxInstance);
        PRINTIF(errCode, __LINE__);
    }

}/* End of Main */
#endif

/*============================================================================*/
/**
    \brief   Function that prints the returned error codes 

    \param   Error code and line number
                                
    \return  None

 ******************************************************************************/
void PRINTIF(tmErrorCode_t errCode, Int lineNumber)
{
    if(errCode != TM_OK)
    {
        printf("Error detected nb 0X%X at line %i of file %s\n",(unsigned int)errCode, lineNumber, __FILE__);
    }
}
#if PLATFORM == 97241
#define NEXUS_HDMI_RX_I2C  4
#else
#define NEXUS_HDMI_RX_I2C  2
#endif
#define NXP_HDMI_RX_ADDRESS 
int main(void)
{
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_HdDviInputHandle hdDviInput;
    NEXUS_HdDviInputSettings hdDviInputSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    NEXUS_I2cSettings i2cSettings;
    tmErrorCode_t errCode = TM_OK;
    int interruption_port = 0;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    platformSettings.openI2c = true;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if 0
    /* open i2c driver */
    NEXUS_I2c_GetDefaultSettings(&i2cSettings);
    g_i2c = NEXUS_I2c_Open(NEXUS_HDMI_RX_I2C, &i2cSettings);
    NEXUS_I2c_GetSettings(g_i2c, &i2cSettings);
    rc = NEXUS_I2c_SetSettings(g_i2c, &i2cSettings);
    BDBG_ASSERT(!rc);
#else
    g_i2c = platformConfig.i2c[NEXUS_HDMI_RX_I2C];
#endif


    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if(platformConfig.outputs.component[0]){
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        BDBG_ERR(("add component output"));
    }
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ERR(("add composite output"));
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    BDBG_ERR(("add hdmi output"));
#endif

    NEXUS_HdDviInput_GetDefaultSettings(&hdDviInputSettings);
    hdDviInput = NEXUS_HdDviInput_Open(0, &hdDviInputSettings);

    window = NEXUS_VideoWindow_Open(display, 0);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_HdDviInput_GetConnector(hdDviInput));
    BDBG_ASSERT(!rc);

    /* */
    /* Initialize the application */
    errCode = tmdlHdmiRxExampleAppliInit();
    PRINTIF(errCode,__LINE__);

    while (1)
    {
        /* Check the status every 50 ms */
        BKNI_Sleep(50);

        /* Read the interruption status*/
        readPortInterrupt(&interruption_port);

        if (interruption_port == IT_LINE_ACTIVE)
        {
            /* Make device library handle an incoming interrupt. This function is
               used by application to tell the device library that the hardware
               sent an interrupt */
            
            errCode = tmdlHdmiRxHandleInterrupt(gDlHdmiRxInstance);
            PRINTIF(errCode, __LINE__);
        }

        errCode = tmdlHdmiRxHeartBeat(gDlHdmiRxInstance);
        PRINTIF(errCode, __LINE__);
    }

    /* NEXUS_I2c_Close(g_i2c); */
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_HdDviInput_Close(hdDviInput);
    NEXUS_Platform_Uninit();

    return 0;
}


