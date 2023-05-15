/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_dash.h  $
 * Version:			$Revision:   $
 * Original Author:	
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     DASH - VFIO Controller Header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup DASH_VFIO_CONTROLLER
 * @brief DASH VFIO Controller. This is interlocked with Nexus playback.
 */

/**
 * @author Jeehune Park (parkjh4@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file di_iptuner_dash.h
 */

#ifndef	__DI_IPTUNER_DASH_H__
#define	__DI_IPTUNER_DASH_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */

#include "di_iptuner_config.h"

#include "htype.h"
#include "di_err.h"
#include "di_media20.h"
#include "dash_main.h"
#include "di_iptuner20.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */

/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/* End typedef */

/*******************************************************************/
/******************************  const  ****************************/
/*******************************************************************/
static const HUINT32 DASH_PRECONNECTOR_MAXPOOL = 1;		/* if this value is too large, representation change will be slow */
static const HUINT32 DASH_BUFFERING_MAXIMUM_BUFFEREDSEGMENT = 2;
static const HUINT32 DASH_MAXTRY_MPDUPDATE_FOR_UNKNOWNENDTIME = 10;
static const HUINT32 DASH_MAXIMUM_CONNECT_TRY = 10;
static const HINT32 DASH_MPD_TASK_SLEEP_INTERVAL = 100;
static const HUINT32 DASH_TEMPORAL_RECV_BUFFER_SIZE = 6*1024;
static const HINT32 DASH_PRECONNECTOR_TASK_SLEEP = 100;
static const HINT32 DASH_BUFFERING_TASK_SLEEP = 100;
static const HINT32 DASH_SCHEDULER_TASK_SLEEP = 100;
static const HINT32 DASH_BUFFERMANAGER_TEMPORAL_BUFFERSIZE = 1 * 1024 * 1024;
static const HINT32 DASH_BUFFERMANAGER_BLOCKSIZE = 1024 * 10;
static const HINT32 DASH_DEFAULT_TIMESHIFTBUFFER_SIZE = 20 * 1024 * 1024;
static const HUINT32 DASH_SEGMENT_RECV_SIZE	= 1024 * 60;
static const HUINT32 DASH_MAX_CONSECUTIVE_SKIPPED_SEGMENT = 10;
static const HUINT32 DASH_VIDEO_STREAM_ID = 0x200;
static const HUINT32 DASH_AUDIO_STREAM_ID = 0x100;
static const HUINT32 DASH_RETAIN_THRESHOLD = 80;  		 /* percentage of total buffer size*/
static const HUINT32 DASH_RETAIN_BLOCK_AT_ONCE = 10; /* percentage of the number of blocks */
static const HUINT32 DASH_MP4BOX_BUFFERSIZE = (1024 * 4);
static const HUINT32 DASH_DOWNLOAD_SIZE_FOR_PROBING = 100 * 1024 * 1024;	/* minimum download size for probing */
static const HUINT32 DASH_MP4CACHE_DEFAULT_ELEMENT_COUNT = 10;	/* the number of MP4 cache element for initialisation*/
static const HBOOL	 DASH_DEFAULT_HEURISTICS_MODE = TRUE;		/* default heuristics mode : TRUE(on)/FALSE(off) */
#define DASH_CODECPRIVDATA_BUFFER (512)
#define DASH_INTRET_ERROR 		-1
#define DASH_INTRET_ERROR_EOF 	-2

typedef struct mem_file_s {
	HBOOL bIsOpened;
    HUINT8 *buf;
    size_t buf_len;
    size_t length;
    size_t offset;
	HBOOL bBoxing;
} DASH_MEM_FILE_t;

typedef struct mp4_cache_element_s {
	HINT32 nCacheID;
	HINT32 nElementType;
	UTIL_MP4_PARSER_t* mp4buf;
	HINT32 szBuf;
} DASH_MP4_CACHE_ELEMENT_t;

typedef struct mp4_cache_s {
	UTIL_DLL_t cache;
	HUINT32 szUsed;
	HUINT32 szUnused;
} DASH_MP4_CACHE_t;

/** 
 * Main state of DASH
 */
typedef enum {
	DASH_STATE_INITIALIZING,	/**< initialize state */
	DASH_STATE_READY,			/**< ready to work state */
	DASH_STATE_PREBUFFERING,	/**< prebuffering state */
	DASH_STATE_BUFFERING,		/**< buffering state */ 
	DASH_STATE_PAUSE,			/**< pause state */
	DASH_STATE_ERROR,			/**< error state */
	DASH_STATE_FINISH,			/**< job completion state : for scheduler */
	DASH_STATE_LIVESTANDBY,		/**< waiting for live start (Availability start time in MPD) */
	DASH_STATE_WAITFORNEWMPD,	/**< waiting for up to date MPD */
	DASH_STATE_FINALIZE,		/**< destructor state */
	DASH_STATE_FINALIZEOK,		/**< destruction complete state */	
	DASH_STATE_RESET,			/**< reset state */
} DASH_PROCESS_STATE_t;

/** 
 * DASH Buffer command definition, this is also used as state
 */
typedef enum
{
	DASH_BUFFERCMD_INVALID = -1,	/**< invalid state */
	DASH_BUFFERCMD_NONE,			/**< default state */
	DASH_BUFFERCMD_STOP,			/**< stop comand */
	DASH_BUFFERCMD_RUN,				/**< run command */
	DASH_BUFFERCMD_PAUSE,			/**< pause command */
	DASH_BUFFERCMD_FLUSH,			/**< flush command */
	DASH_BUFFERCMD_QUIT,			/**< quit command */
	DASH_BUFFERCMD_QUITOK			/**< quit state */
} DASH_BUFFERCMD_e;

/** 
 * DASH Preconnector command definition, this is also used as state
 */
typedef enum
{
	DASH_PRECONCMD_INVALID = -1,	/**< invalid state */
	DASH_PRECONCMD_NONE,			/**< default state */
	DASH_PRECONCMD_STOP,			/**< stop command */
	DASH_PRECONCMD_RUN,				/**< run command */
	DASH_PRECONCMD_PAUSE,			/**< pause command */
	DASH_PRECONCMD_FLUSH,			/**< flush command */
	DASH_PRECONCMD_QUIT,			/**< quit command */
	DASH_PRECONCMD_QUITOK			/**< quit state */
} DASH_PRECONCMD_e;

/** 
 * DASH post user event definition
 */
typedef enum
{
	DASH_EVENT_RESULT_NONE,				/**< No need to process */
	DASH_EVENT_RESULT_ERROR,			/**< Error */
	DASH_EVENT_RESULT_EOF,				/**< Seek position is EOF */
	DASH_EVENT_RESULT_SEEK_BUFREGION,	/**< If seek pos is buffered region */
	DASH_EVENT_RESULT_SEEK_OUTBUF		/**< If seek pos is out of buffered region */
} DASH_EVENT_RESULT_e;

/** 
 * for seperation of probing and playing state
 * 
 */
typedef enum
{
	DASH_IO_TYPE_NONE,
	DASH_IO_TYPE_PROBE,
	DASH_IO_TYPE_PLAYBACK,
} DASH_IO_TYPE_e;

/**
 * DI_STREAM_DASH Handle
 */
typedef struct DASH_s 
{
	DASH_MASTER_HANDLE_t hDASH;				/**< DASH master handle */
	DASH_BUFFERCMD_e buffercmd;				/**< Buffer command */
	DASH_PRECONCMD_e preconcmd;				/**< Preconnector command */ 
	DASH_EVENT_RESULT_e	eventResult;		/**< event result */
	DASH_MP4_CACHE_t* InitData;				/**< Initialisation segment data cache */
	DASH_MEM_FILE_t*	mp4box;				/**< temporal MP4BOX buffer */
	DASH_PROCESS_STATE_t state;				/**< DASH state */
	DASH_PROCESS_STATE_t prevstate;			/**< previous DASH state */
	DASH_IO_TYPE_e IOmode;					/**< DASH VFIO mode : probing or playback */
	off_t ullVirtureOffset;					/**< Fake */
	HULONG ulSema;
} DASH_t;
typedef DASH_t* MEDIA_DASH_HANDLE_t;


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

/*! 
 *	\brief Create DASH data structures, and register handles
 *	\param pUrl : MPD URL
 *	\return Media DASH handle
 *	\see 
 */
MEDIA_DASH_HANDLE_t DI_STREAM_DASH_OpenController(HINT8 * pUrl);

/*! 
 *	\brief Destroy DASH data structures
 *	\param pMDASH : Media DASH handle
 *	\return Error code
 *	\see 
 */
DI_ERR_CODE DI_STREAM_DASH_CloseController(MEDIA_DASH_HANDLE_t pMDASH);

/*! 
 *	\brief External user event receiver (such as seek)
 *	\param pMDASH : Media DASH handle
 *	\param cmd : Control command
 *	\param val : Control value
 *	\param val2 : unused param
 *	\return Error code
 *	\see 
 */
DI_ERR_CODE DI_STREAM_DASH_Control (MEDIA_DASH_HANDLE_t pMDASH, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);

/*! 
 *	\brief Download MP4 header(initialisation segment or partial media segment) and parsing then cache it
 *	\param pDASH : Media DASH handle
 *	\param eStreamType : adaptation type(AUDIO or VIDEO)
 *	\prarm nBandwidth : base bandwidth
 *	\param tTime : scaled time (related to MPD)
 *	\return Error code
 *	\see 
 */
DI_ERR_CODE DI_STREAM_DASH_MP4Preprocess (MEDIA_DASH_HANDLE_t pMDASH, DASH_ADAPTATIONSET_TYPE_e eStreamType, DASH_BANDWIDTH_t nBandwidth, DASH_TIME_t tTime);

/*! 
 *	\brief Create scheduler task
 *	\param pHSession : session handle
 *	\return Error code
 *	\see 
 */
DI_ERR_CODE DI_STREAM_DASH_CreateSchedulerTask (SESSION_HANDLE pHSession);

/*! 
 *	\brief Create MPD task
 *	\param pHSession : session handle
 *	\return Error code
 *	\see 
 */
DI_ERR_CODE DI_STREAM_DASH_CreateMPDTask (SESSION_HANDLE pHSession);

/*! 
 *	\brief DASH VFIO callback install
 *	\param pUrl : MPD URL
 *	\return void
 *	\see 
 */
void DI_STREAM_DASH_RegisterCallback (const char *pUrl);

/*! 
 *	\brief DASH VFIO callback uninstall
 *	\param pUrl : MPD URL
 *	\return void
 *	\see 
 */
void DI_STREAM_DASH_UnregisterCallback (const char *pUrl);

/*! 
 *	\brief Get media format information from DASH to determine DASH works as TS or MP4
 *	\param pMDASH : Media DASH handle
 *	\param format : segment format(return)
 *	\return Error code
 *	\see 
 */
DI_ERR_CODE DI_STREAM_DASH_GetMediaFormat (MEDIA_DASH_HANDLE_t pMDASH, DASH_SEGMENT_FORMAT_e *format);

/*! 
 *	\brief Construct media information(pstMediaInfo) manually
 *	\param pMDASH : Media DASH handle
 *	\param pstMediaInfo : Media info structure(return)
 *	\return Error code
 *	\see 
 */
DI_ERR_CODE DI_STREAM_DASH_GetMediaInfo (MEDIA_DASH_HANDLE_t pMDASH, DI_MEDIA_INFO_t *pstMediaInfo);

/*! 
 *	\brief Set main state of DASH
 *	\param pMDASH : Media DASH handle
 *	\param eState : DASH State
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e DI_STREAM_DASH_SetState(MEDIA_DASH_HANDLE_t pMDASH, DASH_PROCESS_STATE_t eState);

/*! 
 *	\brief Set main state of DASH
 *	\param pMDASH : Media DASH handle
 *	\return DASH state
 *	\see 
 */
DASH_PROCESS_STATE_t DI_STREAM_DASH_GetState(MEDIA_DASH_HANDLE_t pMDASH);

/*! 
 *	\brief Waiting for given state
 *	\param pMDASH : Media DASH handle
 *	\param eState : state which caller will be waiting for
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e DI_STREAM_DASH_WaitState(MEDIA_DASH_HANDLE_t pMDASH, DASH_PROCESS_STATE_t eState);

/*! 
 *	\brief Save current state of DASH
 *	\param pMDASH : Media DASH handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e DI_STREAM_DASH_SaveState(MEDIA_DASH_HANDLE_t pMDASH);

/*! 
 *	\brief Restore previous state of DASH
 *	\param pMDASH : Media DASH handle
 *	\return Error code
 *	\see 
 */
 DASH_ERROR_e DI_STREAM_DASH_RestoreState(MEDIA_DASH_HANDLE_t pMDASH);

/* End global function prototypes */

#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_INETRADIO_H__ */
