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

/************************************/
/* STBService.Components.DRM Object */
/************************************/
TRX_STATUS setCompDRMEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompDRMEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompDRMStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompDRMName(char **value)
{
#if TR135_STUB==1
	*value = strdup("Broadcom TR69 CA");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompDRMSmartCardReader(char **value)
{
#if TR135_STUB==1
	*value = strdup("");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/************************************/
/* STBService.Components.CA Object  */
/************************************/
TRX_STATUS setCompCAEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompCAEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompCAStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompCAName(char **value)
{
#if TR135_STUB==1
	*value = strdup("Broadcom TR69 CA");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompCASmartCardReader(char **value)
{
#if TR135_STUB==1
	*value = strdup("");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/**************************************/
/* STBService.Components.SCART Object */
/**************************************/
TRX_STATUS setCompSCARTEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompSCARTEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompSCARTStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompSCARTname(char **value)
{
#if TR135_STUB==1
	*value = strdup("Broadcom TR69 SCART");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompSCARTPresence(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompSCARTPresence(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/********************************************/
/* STBService.Components.VideoOutput Object */
/********************************************/
TRX_STATUS setCompVideoOutputEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompVideoOutputEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoOutputStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoOutputName(char **value)
{
#if TR135_STUB==1
	*value = strdup("Broadcom TR69 Video Output");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompVideoOutputVideoFormat(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompVideoOutputVideoFormat(char **value)
{
#if TR135_STUB==1
	*value = strdup("HDMI");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoOutputAspectRatioBehaviour(char **value)
{
#if TR135_STUB==1
	*value = strdup("Letterbox");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoOutputMacrovision(char **value)
{
#if TR135_STUB==1
	*value = strdup("v1.0");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoOutputHDCP(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoOutputSCARTs(char **value)
{
#if TR135_STUB==1
	*value = strdup("");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/********************************************/
/* STBService.Components.AudioOutput Object */
/********************************************/
TRX_STATUS setCompAudioOutputEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompAudioOutputEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioOutputStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioOutputName(char **value)
{
#if TR135_STUB==1
	*value = strdup("Broadcom TR69 Audio Output");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioOutputAudioFormat(char **value)
{
 #if TR135_STUB==1
	*value = strdup("DIGITAL-OPTICAL-SP/DIF");   
#else
	/* Get the value from storage */
#endif
return TRX_OK;
}

TRX_STATUS setCompAudioOutputAudioLevel(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompAudioOutputAudioLevel(char **value)
{
#if TR135_STUB==1
	*value = strdup("50");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompAudioOutputCancelMute(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompAudioOutputCancelMute(char **value)
{
#if TR135_STUB==1
	*value = strdup("EnABLE");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioOutputSCARTs(char **value)
{
#if TR135_STUB==1
	*value = strdup("");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************/
/* STBService.Components.VideoDecoder Object */
/*********************************************/
TRX_STATUS setCompVideoDecoderEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderName(char **value)
{
#if TR135_STUB==1
	*value = strdup("TR69 Component Video Decoder");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderMPEG2Part2(char **value)
{
#if TR135_STUB==1
	*value = strdup(".Capabilities.VideoDecoder.MPEG2PART2.ProfileLevel.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderMPEG4Part2(char **value)
{
#if TR135_STUB==1
	*value = strdup(".Capabilities.VideoDecoder.MPEG4PART2.ProfileLevel.2");   
	*value = strdup("");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderMPEG4Part10(char **value)
{
#if TR135_STUB==1
	*value = strdup(".Capabilities.VideoDecoder.MPEG4PART10.ProfileLevel.2");   
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderSMPTEVC1(char **value)
{
#if TR135_STUB==1
	*value = strdup(".Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderContentAspectRatio(char **value)
{
#if TR135_STUB==1
	*value = strdup("16:9");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************/
/* STBService.Components.AudioDecoder Object */
/*********************************************/
TRX_STATUS setCompAudioDecoderEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompAudioDecoderEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioDecoderStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioDecoderName(char **value)
{
#if TR135_STUB==1
	*value = strdup("TR69 Component Video Decoder");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioDecoerStandards(char **value)
{
#if TR135_STUB==1
	*value = strdup("TR69 Component Video Decoder");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************/
/* STBService.Components.PVR.Storage Object  */
/*********************************************/
TRX_STATUS getCompPVRStorageReference(char **value)
{
#if TR135_STUB==1
	*value = strdup("");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************/
/* STBService.Components.PVR Object          */
/*********************************************/
TRX_STATUS getCompPVRStorageNumberOfEntries(char **value)
{
	uint32 num_entries = 1;
#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/********************************************************/
/* STBService.Components.FrontEnd.IP.Dejittering Object */
/********************************************************/
TRX_STATUS setCompFEIPDejitteringBufferSize(char *value)
{
	uint32	buf_size;

	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	buf_size = atoi(value);

	return TRX_OK;
}

TRX_STATUS getCompFEIPDejitteringBufferSize(char **value)
{
	uint32	buf_size = 1;
#if TR135_STUB==1
	*value = strdup(itoa(buf_size));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompFEIPDejitteringBufferInitialLevel(char *value)
{
	uint32	buf_level;
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	buf_level = atoi(value);

	return TRX_OK;
}

TRX_STATUS getCompFEIPDejitteringBufferInitialLevel(char **value)
{
	uint32	buf_level = 1;
#if TR135_STUB==1
	*value = strdup(itoa(buf_level));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/********************************************************/
/* STBService.Components.FrontEnd.IP.Outbound Object    */
/********************************************************/
TRX_STATUS getCompFEIPOutboundStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPOutboundMultiplexType(char **value)
{
#if TR135_STUB==1
	*value = strdup("MPEG2-PS");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPOutboundURI(char **value)
{
#if TR135_STUB==1
	*value = strdup("UPnP AV URN");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/********************************************************/
/* STBService.Components.FrontEnd.IP.Inbound Object     */
/********************************************************/
TRX_STATUS getCompFEIPInboundStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundStreamingControlProtocol(char **value)
{
#if TR135_STUB==1
	*value = strdup("IGMP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundStreamingTransportProtocol(char **value)
{
#if TR135_STUB==1
	*value = strdup("TCP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundStreamingTransportControlProtocol(char **value)
{
#if TR135_STUB==1
	*value = strdup("RTCP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundMultiplexType(char **value)
{
#if TR135_STUB==1
	*value = strdup("MPEG2-TS");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundDownloadTransportProtocol(char **value)
{
#if TR135_STUB==1
	*value = strdup("HTTP");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundSourceAddress(char **value)
{
#if TR135_STUB==1
	*value = strdup("10.10.10.10");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundSourcePort(char **value)
{
#if TR135_STUB==1
	*value = strdup("100");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundDestinationAddress(char **value)
{
#if TR135_STUB==1
	*value = strdup("10.10.10.10");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundDestinationPort(char **value)
{
#if TR135_STUB==1
	*value = strdup("100");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPInboundURI(char **value)
{
#if TR135_STUB==1
	*value = strdup("UPnP AV URN");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/******************************************************************************/
/* STBService.Components.FrontEnd.IP.IGMP.ClientGroupStats.QuarterHour Object */
/******************************************************************************/
TRX_STATUS getCompFEIPIGMPClientGroupStatsQuarterHourNumberOfJoins(char **value)
{
	uint32	num_joins = 1;
#if TR135_STUB==1
	*value = strdup(itoa(num_joins));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsQuarterHourNumberOfLeavs(char **value)
{
	uint32	num_leaves = 1;
#if TR135_STUB==1
	*value = strdup(itoa(num_leaves));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsQuarterHourMaxJoinDelay(char **value)
{
	uint32	max_joins = 1;
#if TR135_STUB==1
	*value = strdup(itoa(max_joins));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/******************************************************************************/
/* STBService.Components.FrontEnd.IP.IGMP.ClientGroupStats.Current Object     */
/******************************************************************************/
TRX_STATUS getCompFEIPIGMPClientGroupStatsCurrentDayNumberOfJoins(char **value)
{
	uint32	num_joins = 1;
#if TR135_STUB==1
	*value = strdup(itoa(num_joins));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsCurrentDayNumberOfLeavs(char **value)
{
	uint32	num_leaves = 1;
#if TR135_STUB==1
	*value = strdup(itoa(num_leaves));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsCurrentDayMaxJoinDelay(char **value)
{
	uint32	max_joins = 1;
#if TR135_STUB==1
	*value = strdup(itoa(max_joins));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/******************************************************************************/
/* STBService.Components.FrontEnd.IP.IGMP.ClientGroupStats.Total Object       */
/******************************************************************************/
TRX_STATUS getCompFEIPIGMPClientGroupStatsTotalNumberOfJoins(char **value)
{
	uint32	num_joins = 1;
#if TR135_STUB==1
	*value = strdup(itoa(num_joins));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsTotalNumberOfLeavs(char **value)
{
	uint32	num_leaves = 1;
#if TR135_STUB==1
	*value = strdup(itoa(num_leaves));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsTotalMaxJoinDelay(char **value)
{
	uint32	max_joins = 1;
#if TR135_STUB==1
	*value = strdup(itoa(max_joins));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/******************************************************************************/
/* STBService.Components.FrontEnd.IP.IGMP.ClientGroupStats Object             */
/******************************************************************************/
TRX_STATUS getCompFEIPIGMPClientGroupStatsAddress(char **value)
{
#if TR135_STUB==1
	*value = strdup("10.10.0.0");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsTotalStart(char **value)
{
	uint32	total_start = 1;
#if TR135_STUB==1
	*value = strdup(itoa(total_start));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsCurrentDayStart(char **value)
{
	uint32	current_start = 1;
#if TR135_STUB==1
	*value = strdup(itoa(current_start));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupStatsQuartHourStart(char **value)
{
	uint32	quarter_hour_start = 1;
#if TR135_STUB==1
	*value = strdup(itoa(quarter_hour_start));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/******************************************************************************/
/* STBService.Components.FrontEnd.IP.IGMP.ClientGroup Object                  */
/******************************************************************************/
TRX_STATUS getCompFEIPIGMPClientGroupAddress(char **value)
{
#if TR135_STUB==1
	*value = strdup("10.10.0.0");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPClientGroupUpTime(char **value)
{
	uint32	up_time = 1;
#if TR135_STUB==1
	*value = strdup(itoa(up_time));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*************************************************/
/* STBService.Components.FrontEnd.IP.IGMP Object */
/*************************************************/
TRX_STATUS setCompFEIPIGMPEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup("1");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPMaximumNumberOfConcurrentGroups(char **value)
{
	uint32	max_con_group = 1;
#if TR135_STUB==1
	*value = strdup(itoa(max_con_group));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPMaximumNumberOfTrackedGroups(char **value)
{
	uint32	max_track_group = 1;
#if TR135_STUB==1
	*value = strdup(itoa(max_track_group));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompFEIPIGMPLoggingEnable(char **value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPLoggingEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup("1");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompFEIPIGMPDSCPMark(char **value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPDSCPMark(char **value)
{
	uint32 diff_serv=1;

#if TR135_STUB==1
	*value = strdup(itoa(diff_serv));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompFEIPIGMPVLANIDMark(char **value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPVLANIDMark(char **value)
{
	int vlan_id=1;

#if TR135_STUB==1
	*value = strdup(itoa(vlan_id));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompFEIPIGMPEthernetPriorityMark(char **value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMPEthernetPriorityMark(char **value)
{
	int eth_prio_mark=1;

#if TR135_STUB==1
	*value = strdup(itoa(eth_prio_mark));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMClientVersion(char **value)
{
#if TR135_STUB==1
	*value = strdup("v1");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompFEIPIGMClientRobustness(char **value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMClientRobustness(char **value)
{
	int robustness=1;

#if TR135_STUB==1
	*value = strdup(itoa(robustness));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setCompFEIPIGMClientUnsolicitedReportingInterval(char **value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMClientUnsolicitedReportingInterval(char **value)
{
	int reporting_interval=1;

#if TR135_STUB==1
	*value = strdup(itoa(reporting_interval));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMClientGroupNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPIGMClientGroupStatsNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*************************************************/
/* STBService.Components.FrontEnd.IP Object      */
/*************************************************/
TRX_STATUS getCompFEIPInboundNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPOutboundNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPActiveInboundIPStreams(char **value)
{
	uint32 num_inbound_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_inbound_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEIPActiveOutboundIPStreams(char **value)
{
	uint32 num_outbound_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_outbound_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*************************************************/
/* STBService.Components.FrontEnd Object         */
/*************************************************/
TRX_STATUS setCompFEEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getCompFEEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompFEName(char **value)
{
#if TR135_STUB==1
	*value = strdup("TR69 Component Froentend");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*************************************************/
/* STBService.Components Object                  */
/*************************************************/
TRX_STATUS getCompFENumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioDecoderNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoDecoderNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompAudioOutputNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompVideoOutputNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompSCARTNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompCANNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getCompDRMNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

