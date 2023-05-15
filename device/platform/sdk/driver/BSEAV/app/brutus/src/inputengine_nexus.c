/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: inputengine_nexus.c $
 * $brcm_Revision: 19 $
 * $brcm_Date: 3/26/12 3:18p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/inputengine_nexus.c $
 * 
 * 19   3/26/12 3:18p vle
 * SW7425-2515: Merge to mainline
 * 
 * SW7425-2515/1   3/23/12 6:19p vle
 * SW7425-2515: Update Brutus to use new NEXUS_Cec module. Remove all CEC
 * related code out of hdmi_output module
 * 
 * 18   3/6/12 10:44a jessem
 * SW7425-2560: Fixed coverity issue 36963.
 *
 * 17   9/14/10 11:45a calvinho
 * SW7400-2906: Fix Coverity Defect ID 19389, mark as intentional
 * 
 * 16   8/26/10 4:02p dlwin
 * SW7125-607: Coverity Defect ID:20398
 * 
 * 15   8/26/10 3:44p dlwin
 * SW7125-606: For Coverity Defect ID:20341
 * 
 * 14   6/17/10 12:47p vle
 * SW3548-2952: Add hdmi debug command to start (hdm_packet_start) and
 * stop (hdm_packet_stop) the transmission of selected hdmi packets
 * 
 * 13   6/16/10 2:40p mphillip
 * SW7405-4502: Fix warnings with DEBUG=n
 * 
 * 12   5/14/10 12:29p jtna
 * SW7125-277: Coverity Defect ID:21062 STACK_USE
 * 
 * 11   5/10/10 11:33a calvinho
 * SW7125-275: Address coverity issues 20490, 20479, 20398, 20342, and
 * 20341
 * 
 * 10   4/12/10 1:12p jgarrett
 * SW7405-3972: Adding new versioning scheme for DTS and other algorithms
 * 
 * 9   3/5/10 3:27p gracew
 * SW7420-580: Added ICAM extension.
 * 
 * 8   1/25/10 11:42a erickson
 * SW7550-210: fix non-7550
 * 
 * 7   1/22/10 2:57p erickson
 * SW7550-210: refactor BCHP_CHIP lists
 *
 * 6   12/31/09 11:12a jgarrett
 * SW7405-3158: Renaming DTS config variables
 *
 * 5   12/2/09 7:32p katrep
 * SW7405-3158:support for DTS certification app
 *
 * 4   11/2/09 4:59p vle
 * SW7405-2885: add HDMI debug functions.
 *
 * SW7405-3158/2   10/13/09 12:22p mward
 * SW7405-3158:  Get the DTS version as a (read-only) codec setting,
 * not a codec status.  Status gets called too frequently.
 *
 * SW7405-3158/1   10/7/09 7:04p mward
 * SW7405-3158: Add commands for DTS certification testing.
 *
 * 3   9/16/09 1:59p erickson
 * SW7405-2885: fix DTV platforms (no hdmi output)
 *
 * 2   9/14/09 2:44p vle
 * SW7405-2885: Add HDMI commands
 *
 * 1   9/4/09 12:45p erickson
 * SW7405-2885: add inputengine_nexus
 *
 **************************************************************************/
#include <string.h>

#include "bstd.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(inputengine_nexus);

/***************
 * This file allows script commands to be written for directly Nexus access.
 * This does not allow for Magnum access.
 ***************/

/* Brutus implements thumbnail by going around the Settop API shim directly to Nexus.
This allows the bthumbnail_extractor library to be written with Nexus types, not Settop API types. */
#include "bsettop_types.h"
#define BSETTOP_NO_API_THUNKS
#include "bsettop_impl.h"
#include "nexus_video_adj.h"

#if B_N_DVI_OUTPUTS > 0
#include "nexus_hdmi_output_hdcp.h"
#include "nexus_hdmi_output_extra.h"
#endif

#if BCHP_CHIP != 7550
/* This is a temporary #define so that latest brutus can build against 7550's current RAP PI */
#define B_USE_RAP_1 0
#else
#define B_USE_RAP_1 1
#endif

#include "nexus_audio_decoder.h"

uint8_t DesResult[ 8 ];

uint8_t caProcDecKey[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 } ;
uint8_t caProcDecData[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
uint8_t caProcEncKey[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 } ;
uint8_t caProcEncData[] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

uint8_t ExpectedDecryptResults[] = { 0x28, 0x6E, 0x1C, 0x57, 0xFF, 0x19, 0xAA, 0xC2 };
uint8_t ExpectedEncryptResults[] = { 0xED, 0x29, 0xB5, 0x80, 0xCF, 0xD5, 0x55, 0x3D };

/*
Execute a script command.
You can use extern to get internal handles from the Settop API.
Return 0 if you have consumed the command.
*/
int inputengine_ext(const char *cmd)
{
#if B_N_DVI_OUTPUTS > 0
    static char EDID_MODULE[] = "BHDM_EDID" ;
#endif
    char *param = strchr(cmd, '(');
    if (param) param++;

    if (strstr(cmd, "vdc_mad(") == cmd) {
        NEXUS_Error rc=0;
        bool enabled = (param)? atoi(param) : 0;
        NEXUS_VideoWindowMadSettings madSettings;
        NEXUS_VideoWindow_GetMadSettings(g_display[0].window[0].nWindow, &madSettings);
        madSettings.deinterlace = enabled;
        rc = NEXUS_VideoWindow_SetMadSettings(g_display[0].window[0].nWindow, &madSettings);
		if(rc)
		{
			printf("NEXUS_VideoWindow_SetMadSettings() returned error=%d\n", rc);
		}
        return 0;
    }
#if B_N_DVI_OUTPUTS > 0
    else if (strstr(cmd, "hdmi_help") == cmd)
    {
        printf("************************************\n");
        printf("**** Available HDMI commands: ******\n");
        printf("hdm_mute(boolean) \n");
        printf("    1 - AVMute ON; 0 - AVMute OFF \n");
        printf("hdm_edid\n");
        printf("    Review Rx EDID info.\n");
        printf("hdm_pcm_support\n");
        printf("    Get Supported PCM audio channels\n");
        printf("hdm_format(nth_Detail_Timing_Block) - parameter type int \n");
        printf("    Get BCM_VideoFmt from the nth Detail Timing block\n");
        printf("hdm_rx_info \n");
        printf("    Report current status/info of the receiver\n");
        printf("hdm_color(colorspace) - parameter type int \n");
        printf("    Modify the colorspace indicated in the AVI Infoframe. While this modifies the AVI Infoframe, \n");
        printf("    it does not modify the actual encoding of the pixels\n");
        printf("        0 - Disable Data to HDMI \n");
        printf("        1 - Digital RGB \n");
        printf("        2 - HD YCbCr \n");
        printf("        3 - SD YCbCr \n");
        printf("hdm_avi_color(colorimetry) - parameter type int \n");
        printf("    Modify the colorimetry indicated in the AVI Infoframe\n");
        printf("        0 - Disable Data to HDMI \n");
        printf("        1 - Digital RGB \n");
        printf("        2 - HD YCbCr \n");
        printf("        3 - SD YCbCr \n");
        printf("hdm_avi_aspect(aspectratio) - parameter type int \n");
        printf("    Modify the picture aspect ratio indicated in the AVI Infoframe\n");
        printf("        0 - 16:9 Aspect Ratio\n") ;
        printf("        1 - 4:3 Aspect Ratio\n") ;
        printf("hdm_packet_start(packetChoice) - parameter type int \n");
        printf("    Enable transmission of specific info frame packet\n");
        printf("        0) AVI InfoFrame \n");
        printf("        1) Vendor Specific InfoFrame \n");
        printf("        2) Audio InfoFrame \n");
        printf("        3) Source Product Description InfoFrame \n");
        printf("hdm_packet_stop(packetChoice) - parameter type int \n");
        printf("    Disable transmission of specific info frame packet\n");
        printf("        0) AVI InfoFrame \n");
        printf("        1) Vendor Specific InfoFrame \n");
        printf("        2) Audio InfoFrame \n");
        printf("        3) Source Product Description InfoFrame \n");
        printf("hdcp_pj(boolean)\n");
        printf("    1 - enable Pj checking; 0 - disable Pj checking \n");
        printf("hdcp_force(pixelValue) - parameter type int \n");
        printf("    0 - Force video pixel to all 0s; 1 - Force video pixel to all 1s \n");
        printf("hdcp_enable \n");
        printf("    Enable HDCP \n");
        printf("hdcp_disable \n");
        printf("    Disable HDCP \n");
        printf("cec_send(device_logical_address,cec_msg_opcode) \n");
        printf("    Send a no-parameter CEC message to a device at specified logical address \n");
        printf("cec_send_param(device_logical_address,cec_msg_opcode,parameter_length,parameter1,parameter2,...) \n");
        printf("    Send a CEC message to a device at specified logical address. CEC message has to contain at least 1 parameter \n");
        printf("cec_bcast(cec_msg_opcode) \n");
        printf("    Broadcast a CEC message \n");
        printf("cec_reset() \n");
        printf("    Reset/disable the HDMI CEC core \n");
        printf("cec_dereset()\n");
        printf("    De-reset/Enable the HDMI CEC core \n");
        printf("********************************************\n");
        return 0;
    }

    else if (strstr(cmd, "hdm_mute(") == cmd) {
        bool mute = atoi(&cmd[9]);
        printf("HDMI Av Mute: %d\n", mute) ;
        NEXUS_HdmiOutput_SetAVMute(g_hdmi[0].handle, mute);
        return 0;
    }
    else if (strstr(cmd, "hdm_edid") == cmd) {

    NEXUS_HdmiOutput_DisplayRxEdid(g_hdmi[0].handle) ;
        return 0 ;
    }
    else if (strstr(cmd, "hdm_pcm_support") == cmd)
    {
        NEXUS_HdmiOutputStatus status;
        NEXUS_HdmiOutput_GetStatus(g_hdmi[0].handle, &status);
        BDBG_WRN(("PCM Audio Up to %d Channels Supported", status.maxAudioPcmChannels));
        return 0 ;
    }
    else if (strstr(cmd, "hdm_rx_info") == cmd) {
        NEXUS_HdmiOutputStatus hdmiStatus ;

        NEXUS_HdmiOutput_GetStatus( g_hdmi[0].handle, &hdmiStatus) ;
        if (!hdmiStatus.connected)
        {
         BDBG_WRN(("No Device is Connected")) ;
         return 0 ;
        }

        BDBG_WRN(("Monitor Name: %s", hdmiStatus.monitorName)) ;
     BDBG_WRN(("HDMI Mode: d", hdmiStatus.hdmiDevice)) ;
     if (hdmiStatus.hdmiDevice)
     {
        BDBG_WRN(("Audio Format: %d", hdmiStatus.audioFormat)) ;
        BDBG_WRN(("Audio Channels: %d", hdmiStatus.audioChannelCount)) ;
        BDBG_WRN(("Audio Sampe Rate %dHx", hdmiStatus.audioSamplingRate)) ;
     }

#if 0
        NEXUS_HdmiOutput_DisplayRxInfo(g_hdmi[0].handle) ;
#endif
        return 0 ;
    }

    else if (strstr(cmd, "hdm_format(") == cmd) {
        int format_number ;
        format_number = (param)? atoi(param): 0;

        NEXUS_HdmiOutput_GetVideoFormatFromDetailTiming(g_hdmi[0].handle, (uint8_t) format_number);
        return 0 ;
    }
    else if (strstr(cmd, "hdm_avi_color(") == cmd) {
        int colorimetry = (param)? atoi(param): 0;

        printf("0) BAVC_MatrixCoefficients_eUnknown      - Disable data to HDMI\n") ;
        printf("1) BAVC_MatrixCoefficients_eHdmi_RGB     - Digital RGB\n") ;
        printf("2) BAVC_MatrixCoefficients_eItu_R_BT_709 - HD YCbCr\n") ;
        printf("3) BAVC_MatrixCoefficients_eSmpte_170M   - SD YCbCr\n") ;

        NEXUS_HdmiOutput_SetAviInfoFrameColorimetry(g_hdmi[0].handle, (uint8_t) colorimetry);
        return 0 ;
    }
    else if (strstr(cmd, "hdm_avi_aspect(") == cmd) {
        int aspectRatio = (param)? atoi(param): 0;

        printf("0) BFMT_AspectRatio_e16_9      - 16:9 Aspect Ratio\n") ;
        printf("1) BFMT_AspectRatio_e4_3        - 4:3 Aspect Ratio\n") ;

        NEXUS_HdmiOutput_SetAviInfoFrameAspectRatio(g_hdmi[0].handle, (uint8_t) aspectRatio);
        return 0 ;
    }

    else if (strstr(cmd, "hdm_packet_start(") == cmd) {
        int packetChoice = (param)? atoi(param): 0;
    
        printf("0) AVI InfoFrame \n");
        printf("1) Vendor Specific InfoFrame \n");
        printf("2) Audio InfoFrame \n");
        printf("3) Source Product Description InfoFrame \n");
    
        NEXUS_HdmiOutput_EnablePacketTransmission(g_hdmi[0].handle, (uint8_t) packetChoice);
        return 0;
    }

    else if (strstr(cmd, "hdm_packet_stop(") == cmd) {
        int packetChoice = (param)? atoi(param): 0;

        printf("0) AVI InfoFrame \n");
        printf("1) Vendor Specific InfoFrame \n");
        printf("2) Audio InfoFrame \n");
        printf("3) Source Product Description InfoFrame \n");

        NEXUS_HdmiOutput_DisablePacketTransmission(g_hdmi[0].handle, (uint8_t) packetChoice);
        return 0;
    }

#if NEXUS_HAS_CEC
    /*******************************************/
    /**** HDMI CEC Related commands            ******/
    else if (strstr(cmd, "cec_send(") == cmd)           /* Send a CEC message to a device at specific address, no parameter in the CEC msg  */
    {
        int addr;
        unsigned int opcode;
        NEXUS_CecMessage stCecMessage;
        NEXUS_CecStatus stCecStatus;

        /* coverity[secure_coding] */
        if (param && (sscanf(param, "%16d,%16x", &addr,  &opcode) == 2))
        {
            printf("Send CEC msg opcode %#x to logical address %d \n", opcode, addr);
            stCecMessage.destinationAddr = (uint8_t) addr;
            stCecMessage.buffer[0] = opcode;
            stCecMessage.length = 1;

            NEXUS_Cec_TransmitMessage(g_hdmi[0].cecHandle, &stCecMessage);
            NEXUS_Cec_GetStatus(g_hdmi[0].cecHandle, &stCecStatus);

            BDBG_WRN(("My Logical Address: %d", stCecStatus.logicalAddress)) ;
            BDBG_WRN(("My Logical Address: : %02X %02X", stCecStatus.physicalAddress[0],stCecStatus.physicalAddress[1])) ;
        }
        else
            BDBG_ERR(("ERROR: Incorrect usage of cec_send(address,opcode) command. Use 'hdmi_help' for more details"));

        return 0;
    }

    /************************************************
      Command syntax:
        send_cec_param(logical address, opcode, plength, p1, p2,...)

      Example:
        send_cec_param(1, n, 3, 02, 03, 04)
    *************************************************/
    else if (strstr(cmd, "cec_send_param(") == cmd)
    {
#define MAX_MSG_LENGTH 16
        int addr;                   /* device's logical address to send the msg */
        unsigned int opcode;             /* CEC message opcode   */
        int plength=1;          /* number of paramaters, minimum is 1 */
        int cnt=0;
        NEXUS_CecMessage stCecMessage;
        NEXUS_CecStatus stCecStatus;

        stCecMessage.destinationAddr = (uint8_t) atoi(strtok(param, ","));
        if (sscanf(strtok(NULL, ","), "%x", &opcode) == 1)
        {
            plength = atoi(strtok(NULL, ","));

        if (plength<1)
        {
            printf("Need to have at least 1 paramater when using this command \n");
            return 0;
        }
        else if (plength+1 > MAX_MSG_LENGTH)
        {
            printf("CEC Msg exceeds message buffer \n");
            return 0;
        }

            stCecMessage.length = plength + 1;
            /* create and initialize msg buffer  */
            for (cnt=0; cnt <= plength+1; cnt++)    {
                stCecMessage.buffer[cnt]=0;
            }

        cnt = 0;
            stCecMessage.buffer[cnt++]=opcode;
        while (cnt <= plength)
        {
                if (sscanf(strtok(NULL, ","), "%x",  &opcode ) != 1) {
                    return 0;
                }
                stCecMessage.buffer[cnt++] = opcode ;
            }

            printf("Send CEC msg opcode %#x to logical address %d with %d parameters \n", stCecMessage.buffer[0],
                stCecMessage.destinationAddr, plength);
            NEXUS_Cec_TransmitMessage(g_hdmi[0].cecHandle, &stCecMessage);

            NEXUS_Cec_GetStatus(g_hdmi[0].cecHandle, &stCecStatus);
            BDBG_WRN(("My Logical Address: %d", stCecStatus.logicalAddress)) ;
            BDBG_WRN(("My Physical Address: : %02X %02X", stCecStatus.physicalAddress[0],stCecStatus.physicalAddress[1])) ;
            return 0;
        }
    }
    else if (strstr(cmd, "cec_bcast(") == cmd)
    {
        int opcode ;
        NEXUS_CecMessage stCecMessage;
        NEXUS_CecStatus stCecStatus;

        if (param && sscanf(param, "%x", (unsigned int *) &opcode) == 1)
        {
            stCecMessage.buffer[0] = opcode ;
            stCecMessage.destinationAddr = 0xF; /* broadcast address */
            stCecMessage.length = 1;
            printf("OpCode: %#x\n", opcode) ;

            NEXUS_Cec_TransmitMessage(g_hdmi[0].cecHandle, &stCecMessage);

            NEXUS_Cec_GetStatus(g_hdmi[0].cecHandle, &stCecStatus);
            BDBG_WRN(("My Logical Address: %d", stCecStatus.logicalAddress)) ;
            BDBG_WRN(("My Physical Address: : %02X %02X", stCecStatus.physicalAddress[0],stCecStatus.physicalAddress[1])) ;
            return 0 ;
        }
    }
    /* Putting CEC core into reset */
    else if (strstr(cmd, "cec_reset(") == cmd)
    {
        NEXUS_CecSettings stCecSettings;

        BDBG_WRN(("Putting CEC core into reset"));
        NEXUS_Cec_GetSettings(g_hdmi[0].cecHandle, &stCecSettings);
        stCecSettings.enabled = false;
        NEXUS_Cec_SetSettings(g_hdmi[0].cecHandle, &stCecSettings);
        return 0;
    }
    /* Take CEC core out of reset */
    else if (strstr(cmd, "cec_dereset(") == cmd)           /* Send a CEC message to a device at specific address, no parameter in the CEC msg  */
    {
        NEXUS_CecSettings stCecSettings;

        BDBG_WRN(("Bring CEC core out of reset"));
        NEXUS_Cec_GetSettings(g_hdmi[0].cecHandle, &stCecSettings);
        stCecSettings.enabled = true;
        NEXUS_Cec_SetSettings(g_hdmi[0].cecHandle, &stCecSettings);
        return 0;
    }
#endif

    /***************************************/
    /**** HDCP Related commands            ******/
    else if (strstr(cmd, "hdcp_pj(") == cmd)
    {
        NEXUS_HdmiOutput_EnablePjChecking(g_hdmi[0].handle, (bool) ((param)? atoi(param): 0));
        return 0;
    }
#if BDBG_DEBUG_BUILD
    else if ((strstr(cmd, "hdcp_force(") == cmd) )
    {
        int pixelValue = (param)? atoi(param): 0;
        BDBG_WRN(("Force: %d", pixelValue)) ;
        NEXUS_HdmiOutput_ForceVideoPixel(g_hdmi[0].handle, (uint8_t) pixelValue);
        return 0 ;
    }
#endif
    #ifdef BICM_ICAM2
    else if (strstr(cmd, "icam2") == cmd) {
        BICM_CLI_main( (void *)NULL);
        return 0;

    }
#endif
    else if ((strstr(cmd, "hdcp_status") == cmd))
    {
        /* coverity[stack_use_local_overflow] */
        NEXUS_HdmiOutputHdcpSettings hdcpSettings ;
        NEXUS_HdmiOutputHdcpStatus hdcpStatus ;

        NEXUS_HdmiOutput_GetHdcpStatus(g_hdmi[0].handle, &hdcpStatus) ;

        /* display Attached Device KSV */
        BDBG_WRN(("")) ;
        BDBG_WRN(("Attached Rx Bksv = %02x %02x %02x %02x %02x",
                hdcpStatus.bksv.data[4], hdcpStatus.bksv.data[3], hdcpStatus.bksv.data[2],
                hdcpStatus.bksv.data[1], hdcpStatus.bksv.data[0]));

        BDBG_WRN(("HDCP State: %d", hdcpStatus.hdcpState)) ;
        BDBG_WRN(("")) ;
        BDBG_WRN(("    HDCP Repeater:    %d", hdcpStatus.isHdcpRepeater)) ;
        BDBG_WRN(("    KSV Fifo Rdy:     %d", hdcpStatus.ksvFifoReady)) ;
        BDBG_WRN(("    HDCP Fast 400kHz: %d", hdcpStatus.i2c400Support)) ;
        BDBG_WRN(("    HDCP 1.1 Support: %d", hdcpStatus.hdcp1_1Features)) ;
        BDBG_WRN(("    HDCP Fast Reauth: %d", hdcpStatus.fastReauthentication)) ;


        NEXUS_HdmiOutput_GetHdcpSettings(g_hdmi[0].handle, &hdcpSettings) ;

        /* review use of transmitEncrypted */
#if 0
        BDBG_WRN(("HDCP Encrypted Frames: %d", hdcpSettings.transmitEncrypted)) ;
        if (hdcpSettings.transmitEncrypted)
        {
            BDBG_WRN((" HDCP Pj Checking: %d", hdcpSettings.pjCheckEnabled)) ;
        }
#endif
        return 0 ;
    }

    else if ( (strstr(cmd, "hdcp_enable") == cmd) || (strstr(cmd, "he") == cmd) )
    {
        boutput_hdmi_settings hdmi_settings;
        BERR_Code brc = BDBG_SetModuleLevel("BHDM_HDCP", BDBG_eMsg) ;

        boutput_hdmi_get(&g_hdmi[0], &hdmi_settings);
        hdmi_settings.hdcp = true;
        boutput_hdmi_set(&g_hdmi[0], &hdmi_settings);

        NEXUS_HdmiOutput_StartHdcpAuthentication(g_hdmi[0].handle);
        return 0 ;
    }

    /* "hd" command should be last.. otherwise other hdm* commands interpretted as hd */
    else if ((strstr(cmd, "hdcp_disable") == cmd) || (strstr(cmd, "hd") == cmd) )
    {
        boutput_hdmi_settings hdmi_settings;
        BERR_Code brc = BDBG_SetModuleLevel("BHDM_HDCP", BDBG_eWrn) ;

        boutput_hdmi_get(&g_hdmi[0], &hdmi_settings);
        hdmi_settings.hdcp = true;
        boutput_hdmi_set(&g_hdmi[0], &hdmi_settings);

        NEXUS_HdmiOutput_DisableHdcpAuthentication(g_hdmi[0].handle);
        return 0 ;
    }

#if 0
    else if (strstr(cmd, "hdm_rx_info") == cmd) {
        BDBG_Level level ;
        BERR_Code rc ;
        uint8_t i, IsRepeater;

        uint16_t BStatus ;
        uint16_t DeviceCount, uiRxKsvIndex ;
        uint8_t RxMonitorName[BHDM_EDID_DESC_ASCII_STRING_LEN] ;
        uint8_t AttachedKsv[BHDM_HDCP_KSV_LENGTH] ;
        uint8_t *DownstreamKsvsBuffer ;
        uint16_t DownstreamKsvsBufferSize ;
        uint8_t RxSense ;
        uint8_t Attached  ;

        static char HDCP_MODULE[] = "BHDM_HDCP" ;

        BHDM_RxDeviceAttached(b_root.video.hdmi_tx->hdm, &Attached) ;
        printf("Device Attached: %d\n", Attached) ;
        if (!Attached)
            return 0 ;

        BHDM_GetReceiverSense(b_root.video.hdmi_tx->hdm, &RxSense) ;
        printf("Device RxSense: %d\n", RxSense) ;

        if (RxSense != 0xF)
        {
            printf("Rx not powered (%d) ...", RxSense) ;
            return 0 ;
        }

        /* save the current debug level */
        BDBG_GetModuleLevel(HDCP_MODULE, &level) ;

        BDBG_SetModuleLevel(HDCP_MODULE, BDBG_eMsg) ;

        BKNI_Memset(RxMonitorName, 0, BHDM_EDID_DESC_ASCII_STRING_LEN) ;
        BHDM_EDID_GetMonitorName(b_root.video.hdmi_tx->hdm, RxMonitorName) ;
        printf("Monitor '%*s':\n",
            BHDM_EDID_DESC_ASCII_STRING_LEN, RxMonitorName) ;

        /*get/display information for BStatus */
        BHDM_HDCP_GetRxStatus(b_root.video.hdmi_tx->hdm, &BStatus) ;

        /* retrieve KSV Information */
        rc = BHDM_HDCP_GetRxKsv(b_root.video.hdmi_tx->hdm, AttachedKsv) ;
        if (rc != BERR_SUCCESS)
        {
            BDBG_WRN(("Error Retrieving Bksv")) ;
            return 0 ;
        }

        /* display Attached Device KSV */
        printf("Attached Rx Bksv = %02x %02x %02x %02x %02x\n",
                AttachedKsv[4], AttachedKsv[3], AttachedKsv[2],
                AttachedKsv[1], AttachedKsv[0]);

        /* switch back to Wrn to remove extra messages when checking for Repeater */
        BDBG_SetModuleLevel(HDCP_MODULE, BDBG_eWrn) ;
        BHDM_HDCP_CheckForRepeater(b_root.video.hdmi_tx->hdm, &IsRepeater) ;
        BDBG_SetModuleLevel(HDCP_MODULE, BDBG_eMsg) ;
        if (IsRepeater)
        {
            /* retrieve Repeater Ksvs if available */
            DeviceCount = BStatus & 0x7F ;

            /* allocate space for a copy of downstream devices */
            DownstreamKsvsBufferSize  = DeviceCount * BHDM_HDCP_KSV_LENGTH ;
            DownstreamKsvsBuffer = (uint8_t *) BKNI_Malloc(sizeof(uint8_t) * DownstreamKsvsBufferSize) ;

            rc = BHDM_HDCP_GetRepeaterKsvFifo(b_root.video.hdmi_tx->hdm,
                DownstreamKsvsBuffer, DownstreamKsvsBufferSize) ;
            if (rc != BERR_SUCCESS)
            {
                BDBG_WRN(("Error w/ Repeater/Downstream Devices")) ;
            }
            else
            {
                printf("Downstream Devices: %d\n", DeviceCount) ;
                for (i = 0 ; i < DeviceCount ; i++)                  /* for each RxKsv */
                {
                    uiRxKsvIndex = i * BHDM_HDCP_KSV_LENGTH ;

                    /* display each KSV to check debugging */
                    printf("Downstream Device %d: %02x %02x %02x %02x %02x\n",
                        i, DownstreamKsvsBuffer[uiRxKsvIndex + 4],
                        DownstreamKsvsBuffer[uiRxKsvIndex + 3], DownstreamKsvsBuffer[uiRxKsvIndex + 2],
                        DownstreamKsvsBuffer[uiRxKsvIndex + 1], DownstreamKsvsBuffer[uiRxKsvIndex + 0]);
                }
            }
        }

        /* restore the debug level */
        BDBG_SetModuleLevel(HDCP_MODULE, level) ;

        return 0 ;
    }
    else if (strstr(cmd, "hdm_color(") == cmd) {
        int ColorSpace = (param)? atoi(param): 0;
        BAVC_HDMI_AviInfoFrame stAviInfo ;
        BAVC_MatrixCoefficients  ulHdmi ;

        BHDM_GetAVIInfoFramePacket(b_root.video.hdmi_tx->hdm, &stAviInfo) ;
        stAviInfo.LeftBarEndPixelNumber = 32;
        BHDM_SetAVIInfoFramePacket(b_root.video.hdmi_tx->hdm, &stAviInfo) ;


        /* test to check results of HDMI AVI InfoFrame
        **
        ** since Info Frame is not changed here; the color on the display should
        ** look incorrect when opposite the AVI Info Frame (RGB or YCrCb)
        ** Default setup for HDMI is RGB
        */

        printf("0) BAVC_MatrixCoefficients_eUnknown      - Disable data to HDMI\n") ;
        printf("1) BAVC_MatrixCoefficients_eHdmi_RGB     - Digital RGB\n") ;
        printf("2) BAVC_MatrixCoefficients_eItu_R_BT_709 - HD YCbCr\n") ;
        printf("3) BAVC_MatrixCoefficients_eSmpte_170M   - SD YCbCr\n") ;

        switch (ColorSpace)
        {
        default :
        case 1 : ulHdmi = BAVC_MatrixCoefficients_eHdmi_RGB ; break ;
        case 2 : ulHdmi = BAVC_MatrixCoefficients_eItu_R_BT_709 ; break ;
        case 3 : ulHdmi = BAVC_MatrixCoefficients_eSmpte_170M   ; break ;
        case 0 : ulHdmi = BAVC_MatrixCoefficients_eUnknown; break ;
        }

        /* assuming display 0 */
        /* HDMI_TODO should actually go through bdisplay_set */
        BVDC_Display_SetHdmiConfiguration(
            b_root.video.display[0].vdc_display, BVDC_Hdmi_0, ulHdmi) ;

        BVDC_ApplyChanges(b_root.video.vdc);

        return 0 ;
    }
#endif /* #if 0 */
#endif /* B_N_DVI_OUTPUTS > 0 */
    /* Query and set */
#if B_USE_RAP_1
    else if (strstr(cmd, "rap_ui32MixLFE2Primary(") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;
        uint32_t ui32MixLFE2Primary = (param)? atoi(param): 0;
        NEXUS_Error rc=0;

        NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);
        BDBG_WRN(("ui32MixLFE2Primary %u -> %u",nCodecSettings.codecSettings.dts.mixLfeToPrimary,ui32MixLFE2Primary));
        nCodecSettings.codecSettings.dts.mixLfeToPrimary = ui32MixLFE2Primary;
        rc = NEXUS_AudioDecoder_SetCodecSettings(g_audio_decode[0].nAudioDecoder, &nCodecSettings);
        if (rc) rc=BSETTOP_ERROR(rc);

        return 0 ;
    }
#endif
    else if (strstr(cmd, "rap_ui32OutMode(") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;
        NEXUS_AudioDecoderSettings decoderSettings;
        uint32_t ui32OutMode = (param)? atoi(param): 0;
        NEXUS_Error rc=0;

        NEXUS_AudioDecoder_Stop(g_audio_decode[0].nAudioDecoder);

        NEXUS_AudioDecoder_GetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);
        /* NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);*/
        BDBG_WRN(("ui32OutMode %u -> %u",decoderSettings.outputMode,ui32OutMode));
        decoderSettings.outputMode = ui32OutMode;
        /*rc = NEXUS_AudioDecoder_SetCodecSettings(g_audio_decode[0].nAudioDecoder, &nCodecSettings);*/
        rc = NEXUS_AudioDecoder_SetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);
        if (rc) rc=BSETTOP_ERROR(rc);
        rc = NEXUS_AudioDecoder_Start(g_audio_decode[0].nAudioDecoder,&g_audio_decode[0].restartSettings);
        if (rc) rc=BSETTOP_ERROR(rc);

        return 0 ;
    }
    else if (strstr(cmd, "rap_ui32OutLfe(") == cmd) {
        /*NEXUS_AudioDecoderCodecSettings nCodecSettings;*/
        NEXUS_AudioDecoderSettings decoderSettings;
        uint32_t ui32OutLfe = (param)? atoi(param): 0;
        NEXUS_Error rc=0;

        NEXUS_AudioDecoder_Stop(g_audio_decode[0].nAudioDecoder);
        /*NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);*/
        NEXUS_AudioDecoder_GetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);
        BDBG_WRN(("ui32OutLfe %u -> %u",decoderSettings.outputLfeMode,ui32OutLfe));
        decoderSettings.outputLfeMode= ui32OutLfe;
        /*rc = NEXUS_AudioDecoder_SetCodecSettings(g_audio_decode[0].nAudioDecoder, &nCodecSettings);*/
        rc = NEXUS_AudioDecoder_SetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);
        if (rc) rc=BSETTOP_ERROR(rc);
        rc = NEXUS_AudioDecoder_Start(g_audio_decode[0].nAudioDecoder,&g_audio_decode[0].restartSettings);
        if (rc) rc=BSETTOP_ERROR(rc);

        return 0 ;
    }
    else if (strstr(cmd, "rap_ui32StereoMode(") == cmd) {
        /*NEXUS_AudioDecoderCodecSettings nCodecSettings;*/
        NEXUS_AudioDecoderSettings decoderSettings;
        uint32_t ui32StereoMode = (param)? atoi(param): 0;
        NEXUS_Error rc=0;

        NEXUS_AudioDecoder_Stop(g_audio_decode[0].nAudioDecoder);
        /*NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);*/
        NEXUS_AudioDecoder_GetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);
        BDBG_WRN(("ui32StereoMode %u -> %u",decoderSettings.dualMonoMode,ui32StereoMode));
        decoderSettings.dualMonoMode = ui32StereoMode;
        /*rc = NEXUS_AudioDecoder_SetCodecSettings(g_audio_decode[0].nAudioDecoder, &nCodecSettings);*/
        rc = NEXUS_AudioDecoder_SetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);
        if (rc) rc=BSETTOP_ERROR(rc);
        rc = NEXUS_AudioDecoder_Start(g_audio_decode[0].nAudioDecoder,&g_audio_decode[0].restartSettings);
        if (rc) rc=BSETTOP_ERROR(rc);

        return 0 ;
    }
#if B_USE_RAP_1
    else if (strstr(cmd, "rap_i32UserDRCFlag(") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;
        int32_t i32UserDRCFlag = (param)? atoi(param): 0;
        NEXUS_Error rc=0;

        NEXUS_AudioDecoder_Stop(g_audio_decode[0].nAudioDecoder);
        NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);
        BDBG_WRN(("i32UserDRCFlag %u -> %u",nCodecSettings.codecSettings.dts.enableDrc,i32UserDRCFlag));
        nCodecSettings.codecSettings.dts.enableDrc = i32UserDRCFlag;
        rc = NEXUS_AudioDecoder_SetCodecSettings(g_audio_decode[0].nAudioDecoder, &nCodecSettings);
        if (rc) rc=BSETTOP_ERROR(rc);
        rc = NEXUS_AudioDecoder_Start(g_audio_decode[0].nAudioDecoder,&g_audio_decode[0].restartSettings);
        if (rc) rc=BSETTOP_ERROR(rc);

        return 0 ;
    }
#endif
    else if (strstr(cmd, "rap_i32DynScaleHigh(") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;
        int32_t i32DynScaleHigh = (param)? atoi(param): 0;
        NEXUS_Error rc=0;

        NEXUS_AudioDecoder_Stop(g_audio_decode[0].nAudioDecoder);
        NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);
        BDBG_WRN(("i32DynScaleHigh %u -> %u",nCodecSettings.codecSettings.dts.boost,i32DynScaleHigh));
        nCodecSettings.codecSettings.dts.boost = i32DynScaleHigh;
        rc = NEXUS_AudioDecoder_SetCodecSettings(g_audio_decode[0].nAudioDecoder, &nCodecSettings);
        if (rc) rc=BSETTOP_ERROR(rc);
        rc = NEXUS_AudioDecoder_Start(g_audio_decode[0].nAudioDecoder,&g_audio_decode[0].restartSettings);
        if (rc) rc=BSETTOP_ERROR(rc);

        return 0 ;
    }
    else if (strstr(cmd, "rap_i32DynScaleLow(") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;
        int32_t i32DynScaleLow = (param)? atoi(param): 0;
        NEXUS_Error rc=0;

        NEXUS_AudioDecoder_Stop(g_audio_decode[0].nAudioDecoder);
        NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);
        BDBG_WRN(("i32DynScaleLow %u -> %u",nCodecSettings.codecSettings.dts.cut,i32DynScaleLow));
        nCodecSettings.codecSettings.dts.cut = i32DynScaleLow;
        rc = NEXUS_AudioDecoder_SetCodecSettings(g_audio_decode[0].nAudioDecoder, &nCodecSettings);
        if (rc) rc=BSETTOP_ERROR(rc);
        rc = NEXUS_AudioDecoder_Start(g_audio_decode[0].nAudioDecoder,&g_audio_decode[0].restartSettings);
        if (rc) rc=BSETTOP_ERROR(rc);

        return 0 ;
    }
#if B_USE_RAP_1
    /* Query only */
    else if (strstr(cmd, "rap_ui32MixLFE2Primary") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;

        NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);
        BDBG_WRN(("ui32MixLFE2Primary %u",nCodecSettings.codecSettings.dts.mixLfeToPrimary));

        return 0 ;
    }
#endif
    else if (strstr(cmd, "rap_ui32OutMode") == cmd) {
        /*NEXUS_AudioDecoderCodecSettings nCodecSettings;*/
        NEXUS_AudioDecoderSettings decoderSettings;
        NEXUS_AudioDecoder_GetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);

        /*NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);*/

        BDBG_WRN(("ui32OutMode %u",decoderSettings.outputMode));

        return 0 ;
    }
    else if (strstr(cmd, "rap_ui32OutLfe") == cmd) {
        /*NEXUS_AudioDecoderCodecSettings nCodecSettings;*/
        NEXUS_AudioDecoderSettings decoderSettings;
        NEXUS_AudioDecoder_GetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);

        /*NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);*/
        BDBG_WRN(("ui32OutLfe %u",decoderSettings.outputLfeMode));

        return 0 ;
    }
    else if (strstr(cmd, "rap_ui32StereoMode") == cmd) {
        /*NEXUS_AudioDecoderCodecSettings nCodecSettings;*/
        NEXUS_AudioDecoderSettings decoderSettings;
        NEXUS_AudioDecoder_GetSettings(g_audio_decode[0].nAudioDecoder,&decoderSettings);

        /*NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);*/
        BDBG_WRN(("ui32StereoMode %u",decoderSettings.dualMonoMode));
        return 0 ;
    }
#if B_USE_RAP_1
    else if (strstr(cmd, "rap_i32UserDRCFlag") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;

        NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);
        BDBG_WRN(("i32UserDRCFlag %u",nCodecSettings.codecSettings.dts.enableDrc));

        return 0 ;
    }
#endif
    else if (strstr(cmd, "rap_i32DynScaleHigh") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;

        NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);
        BDBG_WRN(("i32DynScaleHigh %u",nCodecSettings.codecSettings.dts.boost));

        return 0 ;
    }
    else if (strstr(cmd, "rap_i32DynScaleLow") == cmd) {
        NEXUS_AudioDecoderCodecSettings nCodecSettings;

        NEXUS_AudioDecoder_GetCodecSettings(g_audio_decode[0].nAudioDecoder, NEXUS_AudioCodec_eDts, &nCodecSettings);
        BDBG_WRN(("i32DynScaleLow %u",nCodecSettings.codecSettings.dts.cut));

        return 0 ;
    }
    else if (strstr(cmd, "rap_DTSver") == cmd) {
        NEXUS_AudioDecoderStatus decstatus;
        NEXUS_Error rc;
        rc = NEXUS_AudioDecoder_GetStatus(g_audio_decode[0].nAudioDecoder,&decstatus);

        if ( rc == NEXUS_SUCCESS && ((decstatus.codec == NEXUS_AudioCodec_eDts) || (decstatus.codec == NEXUS_AudioCodec_eDtsHd)))
        {
            BDBG_WRN(("DTS algo profile: %u algo ver %u.%u.%u.%u fw ver %u", decstatus.algorithmProfileId, 
                      decstatus.algorithmVersionId[0], decstatus.algorithmVersionId[1], decstatus.algorithmVersionId[2], decstatus.algorithmVersionId[3],
                      decstatus.firmwareVersionId));
        }
        else
        {
            BDBG_ERR(("rap_DTSver works while decoding DTS."));
        }

        return 0 ;
    }


    return -1;
}
