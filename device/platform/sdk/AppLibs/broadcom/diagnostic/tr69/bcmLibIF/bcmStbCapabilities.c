 /*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : bcmSysIF.c
 *
 * Description: System object functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.2 $
 * $Id: bcmSysIF.c,v 1.2 2005/12/28 20:15:24 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#include "../main/utils.h"
#include "../SOAPParser/CPEframework.h"
#include "bcmSysIF.h"

#include "bcm_types.h"
#include "syscall.h"

#define TR135_STUB	1

/****************************************************/
/* STBService.Capabilities.AudienceStats Object     */
/****************************************************/
TRX_STATUS getCapASMaxAudienceStatsChannels(char **value)
{
	uint32 	channel=1;

#if TR135_STUB==1
	*value = strdup(itoa(channel));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/****************************************************/
/* STBService.Capabilities.ServiceMonitoring Object */
/****************************************************/
TRX_STATUS getCapSMServiceTypes(char **value)
{
#if TR135_STUB==1
	*value = strdup("IPTV, PVR");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapSMMaxActiveMainStreams(char **value)
{
	uint32 max_active_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_active_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapSMMinSampleInterval(char **value)
{
	uint32 min_sample_interval=1;

#if TR135_STUB==1
	*value = strdup(itoa(min_sample_interval));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapSMMaxReportSamples(char **value)
{
	uint32 max_report_samples=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_report_samples));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapSMHighLevelMetricNames(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/****************************************************/
/* STBService.Capabilities.DRM Object                */
/****************************************************/
TRX_STATUS getCapDRMDRMSystems(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/****************************************************/
/* STBService.Capabilities.VideoOutput Object       */
/****************************************************/
TRX_STATUS getCapVideoOutputCompositeVideoStandards(char **value)
{
#if TR135_STUB==1
	*value = strdup("NTSC-J");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoOutputVideoFormats(char **value)
{
#if TR135_STUB==1
	*value = strdup("CVBS, YPrPb, HDMI");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoOutputMacrovision(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoOutputHDCP(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/****************************************************/
/* STBService.Capabilities.AudioOutput Object       */
/****************************************************/
TRX_STATUS getCapAudioOutputAudioFormats(char **value)
{
#if TR135_STUB==1
	*value = strdup("DIGITAL-OPTICAL-SP/DIF");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel Object */
/*********************************************************************/
TRX_STATUS getCapVideoDecoderSMPTEVC1PLProfile(char **value)
{
#if TR135_STUB==1
	*value = strdup("MP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderSMPTEVC1PLLevel(char **value)
{
#if TR135_STUB==1
	*value = strdup("ML");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderSMPTEVC1PLMaximumDecodingCapability(char **value)
{
	uint32 max_decoding_cap=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_decoding_cap));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.VideoDecoder.SMPTEVC1 Object              */
/*********************************************************************/
TRX_STATUS getCapVideoDecoderSMPTEVC1AudioStandards(char **value)
{
#if TR135_STUB==1
	*value = strdup("MP3-Surround, MPEG2-AAC-LC, MPEG4-AAC-LC");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderSMPTEVC1ProfileLevelNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/************************************************************************/
/* STBService.Capabilities.VideoDecoder.MPEG4Part10.ProfileLevel Object */
/************************************************************************/
TRX_STATUS getCapVideoDecoderMPEG4Part10PLProfile(char **value)
{
#if TR135_STUB==1
	*value = strdup("MAIN");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG4Part10PLLevel(char **value)
{
#if TR135_STUB==1
	*value = strdup("4.1");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG4Part10PLMaximumDecodingCapability(char **value)
{
	uint32 max_decoding_cap=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_decoding_cap));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.VideoDecoder.MPEG4Part10 Object           */
/*********************************************************************/
TRX_STATUS getCapVideoDecoderMPEG4Part10AudioStandards(char **value)
{
#if TR135_STUB==1
	*value = strdup("MP3-Surround, MPEG2-AAC-LC, MPEG4-AAC-LC");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG4Part10ProfileLevelNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/************************************************************************/
/* STBService.Capabilities.VideoDecoder.MPEG4Part2.ProfileLevel Object  */
/************************************************************************/
TRX_STATUS getCapVideoDecoderMPEG4Part2PLProfile(char **value)
{
#if TR135_STUB==1
	*value = strdup("MAIN");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG4Part2PLLevel(char **value)
{
#if TR135_STUB==1
	*value = strdup("L5");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG4Part2PLMaximumDecodingCapability(char **value)
{
	uint32 max_decoding_cap=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_decoding_cap));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.VideoDecoder.MPEG4Part2 Object            */
/*********************************************************************/
TRX_STATUS getCapVideoDecoderMPEG4Part2AudioStandards(char **value)
{
#if TR135_STUB==1
	*value = strdup("MP3-Surround, MPEG2-AAC-LC, MPEG4-AAC-LC");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG4Part2ProfileLevelNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/************************************************************************/
/* STBService.Capabilities.VideoDecoder.MPEG2Part2.ProfileLevel Object  */
/************************************************************************/
TRX_STATUS getCapVideoDecoderMPEG2Part2PLProfile(char **value)
{
#if TR135_STUB==1
	*value = strdup("MP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG2Part2PLLevel(char **value)
{
#if TR135_STUB==1
	*value = strdup("ML");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG2Part2PLMaximumDecodingCapability(char **value)
{
	uint32 max_decoding_cap=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_decoding_cap));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.VideoDecoder.MPEG2Part2 Object            */
/*********************************************************************/
TRX_STATUS getCapVideoDecoderMPEG2Part2AudioStandards(char **value)
{
#if TR135_STUB==1
	*value = strdup("MP3-Surround, MPEG2-AAC-LC, MPEG2-AAC-LC");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapVideoDecoderMPEG2Part2ProfileLevelNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.VideoDecoder Object                       */
/*********************************************************************/
TRX_STATUS getCapVideoDecoderVideoStandards(char **value)
{
#if TR135_STUB==1
	*value = strdup("MPEG4-Part2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.AudioDecoder Object                       */
/*********************************************************************/
TRX_STATUS getCapAudioDecoderAudioStandards(char **value)
{
#if TR135_STUB==1
	*value = strdup("MP3-Surround, MPEG2-AAC-LC, MPEG2-AAC-LC");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.PVR Object                                */
/*********************************************************************/
TRX_STATUS getCapPVRMaxIOStreams(char **value)
{
	int max_io_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_io_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapPVRMaxRecordingStreams(char **value)
{
	int max_rec_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_rec_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapPVRMaxPlaybackStreams(char **value)
{
	int max_pb_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_pb_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapPVRMaxTimeDelay(char **value)
{
	int max_time_delay=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_time_delay));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities.FrontEnd.IP Object                        */
/*********************************************************************/
TRX_STATUS getCapFrontEndIPMaxActiveIPProtocols(char **value)
{
	int max_active_ip_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_active_ip_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapFrontEndIPMaxActiveInboundIPProtocols(char **value)
{
	int max_active_inbound_ip_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_active_inbound_ip_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapFrontEndIPMaxActiveOutboundIPProtocols(char **value)
{
	int max_active_outbound_ip_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_active_outbound_ip_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapFrontEndIPStreamingControlProtocols(char **value)
{
#if TR135_STUB==1
	*value = strdup("IGMP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapFrontEndIPStreamingTransportProtocols(char **value)
{
#if TR135_STUB==1
	*value = strdup("TCP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapFrontEndIPStreamingTransportControlProtocols(char **value)
{
#if TR135_STUB==1
	*value = strdup("RTCP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapFrontEndIPStreamingDownloadTransportProtocols(char **value)
{
#if TR135_STUB==1
	*value = strdup("HTTP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapFrontEndIPStreamingMultiplexTypes(char **value)
{
#if TR135_STUB==1
	*value = strdup("MPEG2-PS");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapFrontEndIPMaxDejitteringBufferSize(char **value)
{
	int max_buf_size=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_buf_size));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************************/
/* STBService.Capabilities Object                                    */
/*********************************************************************/
TRX_STATUS getCapMaxActiveAVStreams(char **value)
{
	int max_active_av_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_active_av_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCapMaxActiveAVPlayers(char **value)
{
	int max_active_av_players=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_active_av_players));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}



