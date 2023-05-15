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

/**************************************************************************/
/* STBService.ServiceMonitoring.MainSStream.TotalAudioDecoderStats Object */
/**************************************************************************/
TRX_STATUS getSMMSTotalAudioDecoderStatsDecodedFrames(char **value)
{
	uint32 num_decoded_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_decoded_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalAudioDecoderStatsDecodingErrors(char **value)
{
	uint32 num_decoding_errors=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_decoding_errors));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/**************************************************************************/
/* STBService.ServiceMonitoring.MainSStream.TotalVideoDecoderStats Object */
/**************************************************************************/
TRX_STATUS getSMMSTotalVideoDecoderStatsFrameRate(char **value)
{
	uint32 frame_rate=1;

#if TR135_STUB==1
	*value = strdup(itoa(frame_rate));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsDecodedFrames(char **value)
{
	uint32 num_decoded_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_decoded_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsLostFrames(char **value)
{
	uint32 num_lost_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_lost_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsConcealedFrames(char **value)
{
	uint32 num_concealed_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_concealed_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsIDecodedFrames(char **value)
{
	uint32 num_decoded_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_decoded_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsILostFrames(char **value)
{
	uint32 num_lost_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_lost_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsIConcealedFrames(char **value)
{
	uint32 num_concealed_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_concealed_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsPDecodedFrames(char **value)
{
	uint32 num_decoded_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_decoded_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsPLostFrames(char **value)
{
	uint32 num_lost_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_lost_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsPConcealedFrames(char **value)
{
	uint32 num_concealed_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_concealed_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsBDecodedFrames(char **value)
{
	uint32 num_decoded_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_decoded_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsBLostFrames(char **value)
{
	uint32 num_lost_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_lost_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsBConcealedFrames(char **value)
{
	uint32 num_concealed_frames=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_concealed_frames));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalVideoDecoderStatsAVResynchCounter(char **value)
{
	uint32 num_resync_counter=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_resync_counter));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/**************************************************************************/
/* STBService.ServiceMonitoring.MainSStream.TotalMPEG2TSStats             */
/**************************************************************************/
TRX_STATUS getSMMSTotalMPEG2TSStatsTSPacketsReceived(char **value)
{
	uint32 num_ts_packets_received=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_ts_packets_received));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalMPEG2TSStatsTSSyncByteErrorCount(char **value)
{
	uint32 num_ts_sync_byte_error_count=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_ts_sync_byte_error_count));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalMPEG2TSStatsTSSyncLossCount(char **value)
{
	uint32 num_ts_sync_loss_count=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_ts_sync_loss_count));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalMPEG2TSStatsPacketDiscontinuityCounter(char **value)
{
	uint32 num_packets_discontinuity_count=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_packets_discontinuity_count));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalMPEG2TSStatsPacketDiscontinuityCounterBeforeCA(char **value)
{
	uint32 num_packets_discontinuity_count_before_ca=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_packets_discontinuity_count_before_ca));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/**************************************************************************/
/*  STBService.ServiceMonitoring.MainSStream.TotalDejitteringStats        */
/**************************************************************************/
TRX_STATUS getSMMSTotalDejitteringStatsOverruns(char **value)
{
	uint32 num_overruns=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_overruns));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSTotalDejitteringStatsUnderruns(char **value)
{
	uint32 num_underruns=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_underruns));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/*********************************************************/
/* STBService.ServiceMonitoring.MainSStream.Total Object */ 
/*********************************************************/
TRX_STATUS setSMMSTotalReset(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getSMMSTotalReset(char **value)
{
#if TR135_STUB==1
	*value = strdup("1");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMMSTotalResetTime(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getSMMSTotalResetTime(char **value)
{
	uint32 reset_time=1;


#if TR135_STUB==1
	*value = strdup(itoa(reset_time));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/******************************************************************************/
/* STBService.ServiceMonitoring.MainSStream.SampleHighLevelMetricStats Object */ 
/*******************************************************************************/
TRX_STATUS getSMMSSampleHighLevelMetricStatsMetricName(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMMSSampleHighLevelMetricStatsMetric1Threshold(char *value)
{
	uint32 threshold;

	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	threshold = atoi(value);
	return TRX_OK;
}

TRX_STATUS getSMMSSampleHighLevelMetricStatsMetric1Threshold(char **value)
{
	uint32 threshold=1;

#if TR135_STUB==1
	*value = strdup(itoa(threshold));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMMSSampleHighLevelMetricStatsMetric2Threshold(char *value)
{
	uint32 threshold;

	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	threshold = atoi(value);
	return TRX_OK;
}

TRX_STATUS getSMMSSampleHighLevelMetricStatsMetric2Threshold(char **value)
{
	uint32 threshold=1;

#if TR135_STUB==1
	*value = strdup(itoa(threshold));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleHighLevelMetricStatsSampleSeconds(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleHighLevelMetricStatsMetric1(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleHighLevelMetricStatsMetric2(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleHighLevelMetricStatsMetric1Failures(char **value)
{
	uint32 metric_failure=1;

#if TR135_STUB==1
	*value = strdup(itoa(metric_failure));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleHighLevelMetricStatsMetric2Failures(char **value)
{
	uint32 metric_failure=1;

#if TR135_STUB==1
	*value = strdup(itoa(metric_failure));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/******************************************************************************/
/* STBService.ServiceMonitoring.MainSStream.SampleVideoResponseStats Object   */ 
/******************************************************************************/
TRX_STATUS getSMMSSampleVideoResponseStatsSampleSeconds(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleVideoResponseStatsAverageVideoSystemResponse(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleVideoResponseStatsMinimumVideoSystemResponse(char **value)
{
	uint32 min_video_resp=1;

#if TR135_STUB==1
	*value = strdup(itoa(min_video_resp));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleVideoResponseStatsMaximumVideoSystemResponse(char **value)
{
	uint32 max_video_resp=1;

#if TR135_STUB==1
	*value = strdup(itoa(max_video_resp));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/******************************************************************************/
/* STBService.ServiceMonitoring.MainSStream.Sample Object                     */ 
/******************************************************************************/
TRX_STATUS getSMMSSampleSampleSeconds(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleSignificantChanges(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSamplePVRTimeShift(char **value)
{
#if TR135_STUB==1
	*value = strdup("Temp");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSSampleHighLevelMetricStatsNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_entries));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/**********************************************************/
/* STBService.ServiceMonitoring.MainSStream.Sample Object */ 
/**********************************************************/
TRX_STATUS setSMMSEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getSMMSEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMainStatus(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMMSServiceType(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getSMMSServiceType(char **value)
{
#if TR135_STUB==1
	*value = strdup("IPTV");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMSAVStream(char **value)
{
#if TR135_STUB==1
	*value = strdup(".AVStreams.AVStream.2");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMMSGmin(char *value)
{
	uint32 gmin;
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	gmin = atoi(value);

	return TRX_OK;
}

TRX_STATUS getSMMSGmin(char **value)
{
	uint32 gmin=1;

#if TR135_STUB==1
	*value = strdup(itoa(gmin));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMMSSevereLossMinDistance(char *value)
{
	uint32 min_dist_severe_loss;

	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	min_dist_severe_loss = atoi(value);

	return TRX_OK;
}

TRX_STATUS getSMMSSevereLossMinDistance(char **value)
{
	uint32 min_dist_severe_loss=1;

#if TR135_STUB==1
	*value = strdup(itoa(min_dist_severe_loss));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMMSSevereLossMinLength(char *value)
{
	uint32 min_len_severe_loss;

	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	min_len_severe_loss = atoi(value);

	return TRX_OK;
}

TRX_STATUS getSMMSSevereLossMinLength(char **value)
{
	uint32 min_len_severe_loss=1;

#if TR135_STUB==1
	*value = strdup(itoa(min_len_severe_loss));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

/**********************************************************/
/* STBService.ServiceMonitoring Object                    */ 
/**********************************************************/
TRX_STATUS setSMSampleEnable(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getSMSampleEnable(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMSampleState(char **value)
{
#if TR135_STUB==1
	*value = strdup("Enabled");   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMSampleInterval(char *value)
{
	uint32 smaple_interval;

	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	smaple_interval = atoi(value);

	return TRX_OK;
}

TRX_STATUS getSMSampleInterval(char **value)
{
	uint32 smaple_interval=1;

#if TR135_STUB==1
	*value = strdup(itoa(smaple_interval));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMReportSamples(char *value)
{
	uint32 num_report_smaple;

	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	num_report_smaple = atoi(value);

	return TRX_OK;
}

TRX_STATUS getSMReportSamples(char **value)
{
	uint32 num_report_smaple=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_report_smaple));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMFetchSamples(char *value)
{
	uint32 num_fetch_smaple;

	if ( value == NULL ) return TRX_ERR;

	/* Store the value */
	num_fetch_smaple = atoi(value);

	return TRX_OK;
}

TRX_STATUS getSMFetchSamples(char **value)
{
	uint32 num_fetch_smaple=1;

#if TR135_STUB==1
	*value = strdup(itoa(num_fetch_smaple));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}


/*!!! JK !!!*/
TRX_STATUS setSMTimeReference(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

/*!!! JK !!!*/
TRX_STATUS getSMTimeReference(char **value)
{
#if TR135_STUB==1
	*value = strdup ("Temp");
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS setSMForceSample(char *value)
{
	if ( value == NULL ) return TRX_ERR;

	/* Store the value */

	return TRX_OK;
}

TRX_STATUS getSMForceSample(char **value)
{
#if TR135_STUB==1
	*value = strdup(("1"));   
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMReportStartTime(char **value)
{
#if TR135_STUB==1
	*value = strdup ("Temp");
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMReportEndTime(char **value)
{
#if TR135_STUB==1
	*value = strdup ("Temp");
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}

TRX_STATUS getSMMainStreamNumberOfEntries(char **value)
{
	uint32 num_entries=1;

#if TR135_STUB==1
	*value = strdup (itoa(num_entries));
#else
	/* Get the value from storage */
#endif
	return TRX_OK;
}




