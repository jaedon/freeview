/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	di_mheg_ics_iptuner.h File Name을 적습니다.
	@brief	  	Support MHEG IC & ICS Media	
	@Author		kimjg@humaxdigital.com (Media Client)
	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MHEG_ICS_IPTUNER_H__
#define	__MHEG_ICS_IPTUNER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "htype.h"
#include "di_media.h"

#include "vfio.h"

#include "di_iptuner.h"
#include "di_iptuner_config.h"
#include "di_iptuner_dnp.h"
#include "stream_global.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


typedef struct ProgressiveDownload_Info_s
{
	/* PROGRESSIVE DOWNLOAD GLOBAL VARIABLES */
	/* all of main data structure use 32bit variable for atomic set/fetch */

	HINT32 	nchunkoffPrdnEof;	
	HINT32 	nchunkoffOldPrdnRequestedSeekPoint;
	HINT32 	nchunkoffPrdnRequestedSeekPoint;
	HUINT8 	*pucChunkDownloadStatusArray;
	//HUINT32 nPausePosition;
	HINT32	nchunkoffForcedStop;

	HUINT64	ullDataReadOffset;
	HBOOL	bReadCompleted;
	HINT32	nDownloadBytesPerSecond;
	HINT32	nArrayDownloadSpeed[DNP_DOWNLOAD_SPEED_COUNT];
	HINT32	nDownloadSpeedIndex;

	HBOOL 	bForwardPlay;
#if 1	/* JWKIM, workaround: for Maxdome Service */
	HBOOL 	bPlayFFTrick;
	HINT32	nIsPrebufferingCompleted;
#endif
	HBOOL 	bPrebufferingRequested;
	HBOOL 	bDownCompleted;
	HINT32	nchunkoffIsCompletedStart; /* avoid recheck valid region */

	/* for PRDN INFO */
	HINT32 	nchunkoffCurrentReadPoint;
	HINT32 	nchunkoffLastPrebufferIdx;
	HINT32 	nBufferingPercent;
	HINT32 	nBufferingLeftTime; /* left sec that user must wait */
	HUINT32 nBufferingTime; /* content milisec to be buffered */
	HINT32 	nDownloadChunkPerSecSpeed; /* chunk per sec */
	HINT32 	nDownloadedChunkCount;
	HINT32 	nBufferSizeInChunkCount;	
	HUINT32 nCBulDeviceId;
	HULONG 	nLastReadyStateSentTime;

	HINT64  nSumLength;

	fn_readystate_callback pReadyStateCallbackFunc;
}DI_MHEG_ICS_Prdn_Info_t;


typedef struct DI_MHEG_ICS_IPTUNER_s
{
	/* common parameters for all iptuner types */
	DI_IPTUNER_t				stIptuner;

	HINT8						*pSaveFileName;	
	HINT32						nWfd;
	HINT64						nFileLength;
	HINT64						nEndPosition;

	DI_IPTUNER_Index_e 			nTunerIndex;
		
	VFIO_Handle_t 				hVFIO;

	HULONG						ulHttpSemId;
	STREAM_ACCESS_t				hHttpAccess;
	HBOOL						bHttpStopReceiving;
	HBOOL						bHttpPauseReceiving;
	
	HBOOL						bDiscarded;

	/* download Task Control Parameters */
	HULONG						ulDNPTaskId;
	HULONG						ulDelayedStartTaskId;
	HULONG						ulDNPSema;
	HULONG						ulDNPReadSema;
	HULONG						ulDNPMsgQId;
	HULONG						ulDelayedStartMsgQId;	
	HBOOL						bIsInitialized;
	HBOOL						bDNPTaskFinished;
	HBOOL						bDNPTaskQuit;
	HBOOL						bDelayedStartTaskFinished;
	HBOOL						bIsPlaybackStop;
	HBOOL						bExitDownload;	/* if true, a receiving function will be escape the loop */ 
	HBOOL						bExitRead;	
	HBOOL						bExitIndexRead;	
	HBOOL						bPauseDownload; /* if true, a receiving function will be sleep until the flag is disabled.*/
	HBOOL						bProbeCancel; /* if true, a receiving function will be slept until the flag is disabled.*/
	HBOOL						bUnderflow;
	
	HULONG						ulTunerEventID;
	HBOOL						bIsEvtSend;

	DI_MHEG_ICS_Prdn_Info_t			stPrdnInfo;	
}DI_MHEG_ICS_IPTUNER_t;

typedef struct MHEG_DNP_USER_DATA_s
{
	HUINT32				ulHandle;
	DNP_USER_DATA_t		stData;	
} MHEG_DNP_USER_DATA_t;




/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/



#endif /* !__MHEG_ICS_IPTUNER_H__ */

