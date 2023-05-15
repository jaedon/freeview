/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_abr.h  $
 * Version:			$Revision:   $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     HTTP-MPD Controller for Streaming Header
 * Module:
 * Remarks:	
 */	

/*******************************************************************/ 	
/* Copyright (c) 2011 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/

#ifndef __DI_IPTUNER_ABR_H__
#define __DI_IPTUNER_ABR_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "vfio20.h"
// #include "di_iptuner_config.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */
#if 0
/* CANVAS TODO: DRAGON> need to rename this structure */
typedef struct
{
	HINT32 	nPurpose;	  /* FOR PROBE? OR FOR NEXUS FILE PLAY? */
	HUINT32 uiMediaMaxBitrateInChunkPerSec;
	off_t	player_last_read_cursor;
	off_t	player_last_seek_cursor;
	off_t	align_buffer_start_index;
	off_t	align_buffer_end_index;
	HUINT8*	p_align_buffer;
	off_t	indexer_last_seek_cursor;
	size_t 	player_last_read_size;
	HBOOL 	bDNPNetworkError;
	HBOOL 	bProbeDone;
	HUINT32 uiContentDuration;
	HUINT32 unOpenFromNexusHandle;
	
} DI_STREAM_HTTP_USER_DATA_t;
#endif
/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
#define DI_IPTUNER_ABR_CONST_BOUNDS_PADDING (1)
#define DI_IPTUNER_ABR_CONFIG_POSIX_NOINDEX
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/** HTTP STREAM VFIO Register/Unregister */
void DI_STREAM_ABR_RegisterCallback (const char *pUrl);
void DI_STREAM_ABR_UnregisterCallback (const char *pUrl);

void DI_STREAM_ABR_RegisterProbeCallback (const char *pUrl);
void DI_STREAM_ABR_UnregisterProbeCallback (const char *pUrl);

/*
void DI_STREAM_ABR_MPD_HTTP_RegisterProtocolCB(DI_IPTUNER_t *pTuner);
void DI_STREAM_ABR_MPD_HTTP_UnregisterProtocolCB(DI_IPTUNER_t *pTuner);
*/

/* external API */
int DI_STREAM_ABR_SetTimeForSeek(DI_IPTUNER_t *pTunerHandle, HUINT64 ullTime, DI_MEDIA_TIME_TYPE eTimeType);
int DI_STREAM_ABR_SetABRStream(DI_IPTUNER_t *pTunerHandle, HINT32 nStreamIndex, HBOOL bDeferred);

#if 0

/** HTTP DNP Control Functions */
void DI_STREAM_HTTP_DnpStartReceiving (void);
void DI_STREAM_HTTP_DnpStopReceiving (void);
void DI_STREAM_HTTP_DnpPauseReceiving (void);
void DI_STREAM_HTTP_DnpResumeReceiving (void);
int DI_STREAM_HTTP_DnpReceive (DI_IPTUNER_t *pHTuner);

#endif

#endif /* ! __DI_IPTUNER_ABR_H__ */

