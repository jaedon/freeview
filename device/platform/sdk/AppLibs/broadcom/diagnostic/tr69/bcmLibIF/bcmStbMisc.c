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

/****************************************/
/* STBService Object                    */
/****************************************/
TRX_STATUS getSTBServiceEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup("1");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/****************************************/
/* STBService.AVStreams.AVStream Object */
/****************************************/
TRX_STATUS getAVStreamsAVStreamStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamName(char **value)
{
#if TR135_STUB==1
	*value = strdup("Broadcom TR69 AVS Stream");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamPVRState(char **value)
{
#if TR135_STUB==1
	*value = strdup("Disabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamFrontEnd(char **value)
{
#if TR135_STUB==1
	*value = strdup("Components.Frontend.2");   
	*value = strdup("Broadcom TR69 AVS Stream");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamInbound(char **value)
{
#if TR135_STUB==1
	*value = strdup("Components.Frontend.2.Inbound.3");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamOutbound(char **value)
{
#if TR135_STUB==1
	*value = strdup("Components.Frontend.2.Outbound.3");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamAudioDecoder(char **value)
{
#if TR135_STUB==1
	*value = strdup("Components.AudioDecoder.s");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamVideoDecoder(char **value)
{
#if TR135_STUB==1
	*value = strdup("Components.VideoDecoder.s");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamCA(char **value)
{
#if TR135_STUB==1
	*value = strdup("Components.CA.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsAVStreamDRM(char **value)
{
#if TR135_STUB==1
	*value = strdup("Components.DRM.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*************************************************/
/* STBService.Components Object                  */
/*************************************************/
TRX_STATUS getAVStreamsActiveAVStreams(char **value)
{
	uint32 num_active_streams=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_active_streams));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVStreamsNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/****************************************/
/* STBService.AVPlayers.AVPLayer Object */
/****************************************/
TRX_STATUS setAVPlayersAVPlayerEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerName(char **value)
{
#if TR135_STUB==1
	*value = strdup("Broadcom TR69 AVPlayer");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerAudioLanguage(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerSubtitlingStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerSubtitlingLanguage(char **value)
{
#if TR135_STUB==1
	*value = strdup("English");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerAudioOutputs(char **value)
{
#if TR135_STUB==1
	*value = strdup(".Components.Audiooutput.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerVideoOutputs(char **value)
{
#if TR135_STUB==1
	*value = strdup(".Components.VideoOutput.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerMainStream(char **value)
{
#if TR135_STUB==1
	*value = strdup(".AVStreams.AVStream.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersAVPlayerPIPStreams(char **value)
{
#if TR135_STUB==1
	*value = strdup(".AVStreams.AVStream.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/****************************************/
/* STBService.AVPlayers.AVPLayer Object */
/****************************************/
TRX_STATUS getAVPlayersActiveAVPlayer(char **value)
{
	uint32 num_active_avplayer=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_active_avplayer));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersPreferredAudioLanguage(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getAVPlayersPreferredSubtitlingLanguage(char **value)
{
#if TR135_STUB==1
	*value = strdup("English");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}


/********************************************************/
/* STBService.Applications.AudienceStats.Channel Object */
/********************************************************/
TRX_STATUS getApplicationsAudienceStatsChannelName(char **value)
{
#if TR135_STUB==1
	*value = strdup("Broadcom TR69 Audience Channel Name");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getApplicationsAudienceStatsChannelDuration(char **value)
{
	uint32 channel_duration=1;

#if TR135_STUB==1
	*value = strdup(itoa(channel_duration));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/****************************************/
/* STBService.AVPlayers.AVPLayer Object */
/****************************************/
TRX_STATUS setApplicationsAudienceStatsEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getApplicationsAudienceStatsEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setApplicationsAudienceStatsReset(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getApplicationsAudienceStatsReset(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getApplicationsAudienceStatsResetTime(char **value)
{
	uint32 reset_time=1;

#if TR135_STUB==1
	*value = strdup(itoa(reset_time));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getApplicationsAudienceStatsChannelNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}



