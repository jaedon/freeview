/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   dash_controller.h  $
 * Version:			$Revision:   1.0 $
 * Original Author:	
 * Current Author:	$Author: parkjh4@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     Dash controller module
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup DASH_CONTROLLER
 */

/**
 * @author 
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
 * @file dash_controller.h
 */


#ifndef __DASH_CONTROLLER_H__
#define __DASH_CONTROLLER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "util_dllist.h"
#include "util_mp4header_parser.h"
#include "dash_mpdparser.h"
#include <sys/time.h> 	 /** for checking delay */

/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */

/*******************************************************************/
/************************ Constant Definition *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/** 
 * DASH error code
 */
typedef enum
{
	DASH_ERR_OK,
	DASH_ERR_ERROR,
	DASH_ERR_EOF,
	DASH_ERR_INFO_EMPTY,
	DASH_ERR_STOP_BY_FLUSH,
	DASH_ERR_STOP_FORCE,
	DASH_ERR_STOP_BY_QUITCMD,
	DASH_ERR_STOP_BY_USEREVENT,
	DASH_ERR_INITSEGMENT_NONE,
	DASH_ERR_INDEXSEGMENT_NONE,
	DASH_ERR_POOL_EMPTY,
	DASH_ERR_LIST_EMPTY,
	DASH_ERR_EXCEED_MAXIMUM_CONNECT_TRY
} DASH_ERROR_e;

/** 
 * Probing method selection(by initialisation or by real media segment)
 */
typedef enum
{
	DASH_PROBE_NONE,
	DASH_PROBE_INITIALISATION,
	DASH_PROBE_REALSEGMENT
} DASH_PROBE_METHOD_e;

/** 
 * DASH User event definition
 */
typedef enum
{
	DASH_USER_EVENT_NONE,
	DASH_USER_EVENT_SEEK,
	DASH_USER_EVENT_STOP,
	DASH_USER_EVENT_PAUSE,
	DASH_USER_EVENT_START,
	DASH_USER_EVENT_TRICK,
	DASH_USER_EVENT_QUIT,
	DASH_USER_EVENT_MPDUPDATE
} DASH_USER_EVENT_e;

/** 
 * DASH time base
 */
typedef enum
{
	DASH_TIME_BASE_CONNECTING,	/**< Connecting base time */
	DASH_TIME_BASE_BUFFERING	/**< Buffering base time */
} DASH_TIME_BASE_e;

/** 
 * DASH Connecting time base
 */
typedef enum
{
	DASH_CONNECTTIME_VIDEO,	/**< Video based connecting time */
	DASH_CONNECTTIME_AUDIO	/**< Audio based connecting time */
} DASH_CONNECTTIME_e;

/** 
 * DASH Buffering mode (currently, not used)
 */
typedef enum
{
	DASH_BUFFERING_MODE_PRE,		/**< prebuffering mode */
	DASH_BUFFERING_MODE_NORMAL		/**< normal buffering mode */
} DASH_BUFFERING_MODE_e;

/** 
 * DASH Preconnector item definition
 */
typedef struct DASH_PRECONNECTOR_ITEM_s
{
	HUINT32 		nID;			/**< Item ID (not used) */
	HINT8			*pucURL;		/**< Item URL */
	HINT8			*pSegBuffer;	/**< Item Data buffer */
	HUINT64			ullSegmentsize;	/**< Item Size : set by preconnector */
	HINT64			ullStarttime;	/**< Item start time */
	HINT64			ullDuration;	/**< Item duration */
	HINT64			ullRequestedPos;/**< Item requested pos (when seek position is inner segment) */
	DASH_BANDWIDTH_t nBandwidth;	/**< item bandwidth */
	DASH_SEGMENT_e	eSegmentType;	/**< Item type (init segment, index segment, media */
	DASH_ADAPTATIONSET_TYPE_e eStreamType;	/**< Item stream type (audio, video) for organizing MP4 box */
	DASH_CODEC_e	eCodec;
	HINT64			ullStartOffset;	/**< Item start offset (for byte range addressing) */
	HINT64			ullEndOffset;	/**< Item end offset (for byte range addressing) */
	HBOOL			bIsLastSegment;	/**< Last segment indicator */
	void			*pAccess;		/**< Stream Access struct */
} DASH_PRECONNECTOR_ITEM_t;

/** 
 * DASH Preconnector structure(handle)
 */
typedef struct DASH_PRECONNECTOR_s
{
	HUINT32		nMaxPoolElements;	/**< Pool size */
	UTIL_DLL_t 	ConnectedList;		/**< Connected item list */
	UTIL_DLL_t 	NotConnectedList;	/**< Not connected item list */
	UTIL_DLL_t 	Pool;				/**< Free list */
	HULONG		ulSema;				/**< Semaphore */
} DASH_PRECONNECTOR_t;
typedef DASH_PRECONNECTOR_t* DASH_PRECONNECTOR_HANDLE_t;

/** 
 * DASH Controller structure(handle)
 */
typedef struct DASH_CONTROLLER_s
{
	HUINT8* MPDUrl;							/**< MPD url */
	DASH_PROBE_METHOD_e probemethod;		/**< Probe method */
	DASH_SEGMENT_FORMAT_e format;			/**< Segment format : TS or MP4 */
	DASH_MPDPARSER_HANDLE_t mpdparser;		/**< MPD parser handle */
	HULONG ulCmdlock;						/**< Task command lock */
	DASH_TIME_t CurVideoConnectTime;		/**< Current Video connecting time */
	DASH_TIME_t CurAudioConnectTime;		/**< Current Audio connecting time */
	DASH_TIME_t CurrentBufferingTime;		/**< Current Buffering time */
	DASH_LOCALE_TYPE_e CurrentLocale;		/**< Current Locale */
	DASH_BANDWIDTH_t CurrentBandwidth;		/**< Current Bandwidth */
	DASH_ADAPTATIONSET_TYPE_e CurrentSelectedAV;	/**< Current selected stream : video or audio or AV*/
	DASH_TIME_t TimeScale;							/**< Time scale */
	DASH_BUFFERING_MODE_e BufferingMode;			/**< Buffering mode : prebuffering or normal buffering*/
	DASH_PRECONNECTOR_ITEM_t* inBufferingItem;		/**< Current buffering item */
	HBOOL bIsPrebufferd;							/**< prebuffering completion flag */
	HBOOL bIsEndofStream;							/**< end of stream flag */
	HBOOL bIsReadCompletion;						/**< read sync flag */
	HBOOL bHeuristicsOn;							/**< heuristics switch */
	HULONG ulSema;									/**< controller lock */
} DASH_CONTROLLER_t;
typedef DASH_CONTROLLER_t*	 DASH_CONTROLLER_HANDLE_t;

typedef UTIL_DLLNode_t*		 DASH_PRECONNECTOR_CONNECTED_t;
typedef UTIL_DLLNode_t*		 DASH_PRECONNECTOR_NOTCONNECTED_t;
typedef UTIL_DLLNode_t*		 DASH_PRECONNECTOR_POOL_t;

/** 
 * DASH Immediate segment indicator for retrieving segment
 */
typedef enum
{
	DASH_LAST_ELEMENT_NONE,
	DASH_LAST_ELEMENT_REPRESENTATION,	/**< Last element is representation */
	DASH_LAST_ELEMENT_ADAPTATION,		/**< Last element is adaptation */
	DASH_LAST_ELEMENT_PERIOD,			/**< Last element is period */
	DASH_LAST_ELEMENT_MPD				/**< Last element is MPD */
} DASH_LAST_ELEMENT_e;

/* when we call function that looking for current segment, 
   the function returns the result in below structure */
typedef struct DASH_IMMEDIATE_ELEMENT_s
{
	DASH_MPD_t* mpd;						/* which MPD belong to */
	DASH_Period_t* period;					/* which Period belong to */
	DASH_AdaptationSet_t* adaptation;		/* which Adaptationset belong to */
	DASH_Representation_t* representation;	/* which Representation belong to */
	DASH_SegmentBase_t* segmentbase;		/* which segment base belong to */
	DASH_SegmentList_t* segmentlist;		/* which segment list belong to */
	DASH_SegmentTemplate_t* segmenttemplate;/* which segment template belong to */
	DASH_LAST_ELEMENT_e selectedElement;	/* where the segment is*/
	DASH_LAST_ELEMENT_e whereSegmentlist;	/* where the segment list is */
	DASH_LAST_ELEMENT_e whereSegmentbase;	/* where the segment base is */
	DASH_LAST_ELEMENT_e whereSegmenttemplate; /* where the segment tempalte is */
	HINT8 derivedURL[DASH_MAX_URL_LEN];		/* derived URL */
	HUINT32 segment_starttime;				/* segment starttime */
	HUINT32	segment_duration;				/* segment duration */
} DASH_IMMEDIATE_ELEMENT_t;


/*******************************************************
*         Controller APIs
*******************************************************/
/*! 
 *	\brief Create controller
 *	\return controller handle
 *	\see 
 */
DASH_CONTROLLER_HANDLE_t DASH_CTRL_Create(void);

/*! 
 *	\brief Destroy controller
 *	\param pController : controller handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_Destroy(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Set MPD Url
 *	\param pController : controller handle
 *	\param stURL : url string
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 		DASH_CTRL_SetMPDUrl(DASH_CONTROLLER_HANDLE_t pController, HINT8* stURL);

/*! 
 *	\brief Set MPD Parser
 *	\param pController : controller handle
 *	\param pMPD : mpd parser handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 		DASH_CTRL_SetMPDParser(DASH_CONTROLLER_HANDLE_t pController, DASH_MPDPARSER_HANDLE_t pMPD);

/*! 
 *	\brief Set probing method (this is needed by TS probing)
 *	\param pController : controller handle
 *	\param eMethod : probing method
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_SetProbeMethod(DASH_CONTROLLER_HANDLE_t pController, DASH_PROBE_METHOD_e eMethod);

/*! 
 *	\brief Get current probing method (this is needed by TS probing)
 *	\param pController : controller handle
 *	\return probe method value
 *	\see 
 */
DASH_PROBE_METHOD_e	DASH_CTRL_GetProbeMethod(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Find segment information based on segment type, track type, time
 *	\param pController : controller handle
 *	\param eSegType : segment type
 * 	\param eAdaptationType : Adaptation(track) type
 *	\param nBandwidth : bandwidth
 *	\param nTime : time (converted to MPD time scale)
 *	\param pSeg : Segment information
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e DASH_CTRL_FindSegmentInfo (DASH_CONTROLLER_HANDLE_t pController, DASH_SEGMENT_e eSegType, DASH_ADAPTATIONSET_TYPE_e eAdaptaionType, 
 DASH_BANDWIDTH_t nBandwidth, DASH_TIME_t tTime,DASH_SEGMENTINFO_t* pSeg);

/*! 
 *	\brief Get current segment information based on segment type, track type
 *	\param pController : controller handle
 *	\param eSegType : segment type
 * 	\param eAdaptationType : Adaptation(track) type
 *	\param pSeg : Segment information
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_GetSegmentInfo(DASH_CONTROLLER_HANDLE_t pController, DASH_SEGMENT_e eSegType, DASH_ADAPTATIONSET_TYPE_e eAdaptationType, DASH_SEGMENTINFO_t* pSeg);

/*! 
 *	\brief Set connecting time which will be based of getting segment
 *	\param pController : controller handle
 *	\param tTime : specific time(in ms)
 * 	\param eTimebase : timebase (Audio or Video)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 		DASH_CTRL_SetCurrentConnectingTime(DASH_CONTROLLER_HANDLE_t pController, DASH_TIME_t tTime, DASH_CONNECTTIME_e eTimebase);

/*! 
 *	\brief Get current connecting time
 *	\param pController : controller handle
 * 	\param eTimebase : timebase (Audio or Video)
 *	\return connecting time
 *	\see 
 */
DASH_TIME_t 		DASH_CTRL_GetCurrentConnectingTime(DASH_CONTROLLER_HANDLE_t pController, DASH_CONNECTTIME_e eTimebase);

/*! 
 *	\brief Set buffering time which will be based of buffering
 *	\param pController : controller handle
 * 	\param eTimebase : specific time(in ms)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_SetCurrentBufferingTime(DASH_CONTROLLER_HANDLE_t pController, DASH_TIME_t etime);

/*! 
 *	\brief Get current buffering time
 *	\param pController : controller handle
 *	\return buffering time
 *	\see 
 */
DASH_TIME_t			DASH_CTRL_GetCurrentBufferingTime(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Get current buffering time
 *	\param pController : controller handle
 *	\param eLocale : language information for audio
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 		DASH_CTRL_SetCurrentLocale(DASH_CONTROLLER_HANDLE_t pController, DASH_LOCALE_TYPE_e eLocale);

/*! 
 *	\brief Set buffering mode (prebuffering or normal)
 *	\param pController : controller handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_SetBufferingMode(DASH_CONTROLLER_HANDLE_t pController, DASH_BUFFERING_MODE_e eBufmode);

/*! 
 *	\brief Get current buffering mode (prebuffering or normal)
 *	\param pController : controller handle
 *	\return bufering mode value
 *	\see 
 */
DASH_BUFFERING_MODE_e	DASH_CTRL_GetBufferingMode(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Set current bandwidth
 *	\param pController : controller handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 		DASH_CTRL_SetCurrentBandwidth(DASH_CONTROLLER_HANDLE_t pController, DASH_BANDWIDTH_t tBandwidth);

/*! 
 *	\brief Get maximum bandwidth in representations of MPD based on given time
 *	\param pController : controller handle
 *	\param tTimebase   : time base (connecting , buffering)
 *	\return maximum bandwidth
 *	\see 
 */
HINT32				DASH_CTRL_GetMaxBandwidth(DASH_CONTROLLER_HANDLE_t pController, DASH_TIME_BASE_e eTimebase);

/*! 
 *	\brief Get current bandwidth of selected representation
 *	\param pController : controller handle
 *	\return current bandwidth
 *	\see 
 */
HUINT32				DASH_CTRL_GetCurrentBandwidth(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Get duration of current MPD
 *	\param pController : controller handle
 *	\return MPD duration
 *	\see 
 */
HINT64 				DASH_CTRL_GetMediaDuration(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Get the number of tracks in MPD
 *	\param pController : controller handle
 *	\return the number of tracks
 *	\see 
 */
HINT32				DASH_CTRL_GetTotalTracks(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Get end of stream flag (this is a notifier for read function from scheduler)
 *	\param pController : controller handle
 *	\return BOOL
 *	\see 
 */
HBOOL				DASH_CTRL_GetIsEndOfStream(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Set end of stream flag
 *	\param pController : controller handle
 *	\param bVal : true or not
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_SetIsEndOfStream(DASH_CONTROLLER_HANDLE_t pController, HBOOL bVal);

/*! 
 *	\brief Get read completion flag (this is a notifier for scheduler from read function)
 *	\param pController : controller handle
 *	\return BOOL
 *	\see 
 */
HBOOL				DASH_CTRL_GetIsReadCompletion(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Set read completion flag
 *	\param pController : controller handle
 *	\param bVal : true or not
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_SetIsReadCompletion(DASH_CONTROLLER_HANDLE_t pController, HBOOL bVal);

/*! 
 *	\brief wait for read is complete
 *	\param pController : controller handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_WaitForReadCompletion(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Get minbuffertime attribute value in MPD
 *	\param pController : controller handle
 *	\return minbuffertime in ms
 *	\see 
 */
HUINT32 			DASH_CTRL_GetMinBufferTime(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Check whether MPD is updatable or not
 *	\param pController : controller handle
 *	\return BOOL
 *	\see 
 */
HBOOL 				DASH_CTRL_GetIsLiveUpdate(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Get segment format in MPD (MP4 or TS)
 *	\param pController : controller handle
 *	\return segment format value
 *	\see 
 */
DASH_SEGMENT_FORMAT_e DASH_CTRL_GetSegmentFormat(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Get timescale in MPD
 *	\param pController : controller handle
 *	\return timescale value
 *	\see 
 */
HINT64				DASH_CTRL_GetTimeScale(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Convert time in MS to Non-scaled time(orginal value)
 *	\param pController : controller handle
 *	\param val : time value(MS)
 *	\return non-scaled time
 *	\see 
 */
HINT64 				DASH_CTRL_ConvertMStoNonScaledTime(DASH_CONTROLLER_HANDLE_t pController, HINT64 val);

/*! 
 *	\brief Convert Non-scaled time(original value) to time in MS
 *	\param pController : controller handle
 *	\param val : non-scaled time
 *	\return time value(MS)
 *	\see 
 */
HINT64 				DASH_CTRL_ConvertNonScaledTimetoMs(DASH_CONTROLLER_HANDLE_t controller, HINT64 val);

/*! 
 *	\brief Set current buffering item
 *	\param pController : controller handle
 *	\param pItem : preconnector item
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_SetCurrentBufferingItem(DASH_CONTROLLER_HANDLE_t pController, DASH_PRECONNECTOR_ITEM_t* pItem);

/*! 
 *	\brief Get current buffering item (this is required for heuristics to get current segment duration)
 *	\param pController : controller handle
 *	\param pItem : preconnector item
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_GetCurrentBufferingItem(DASH_CONTROLLER_HANDLE_t pController, DASH_PRECONNECTOR_ITEM_t* pItem);

/*! 
 *	\brief Get type of track
 *	\param pController : controller handle
 *	\param nTrack : track number
 *	\return track type value
 *	\see 
 */
DASH_ADAPTATIONSET_TYPE_e DASH_CTRL_GetTrackType(DASH_CONTROLLER_HANDLE_t pController, HUINT32 nTrack);

/*! 
 *	\brief Get codec of track
 *	\param pController : controller handle
 *	\param nTrack : track number
 *	\return codec value
 *	\see 
 */
DASH_CODEC_e 		DASH_CTRL_GetTrackCodec(DASH_CONTROLLER_HANDLE_t pController, HUINT32 nTrack);

/*! 
 *	\brief Get language of track
 *	\param pController : controller handle
 *	\param nTrack : track number
 *	\return language value
 *	\see 
 */
DASH_STRING_t 		DASH_CTRL_GetTrackLang(DASH_CONTROLLER_HANDLE_t pController, HUINT32 nTrack);

/*! 
 *	\brief Get minimum update period
 *	\param pController : controller handle
 *	\return time value in ms
 *	\see 
 */
HINT32				DASH_CTRL_GetUpdatePeriod(DASH_CONTROLLER_HANDLE_t controller);

/*! 
 *	\brief Set current selected stream type
 *	\param pController : controller handle
 *	\param type : stream type
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_SetCurrentAVType(DASH_CONTROLLER_HANDLE_t pController, DASH_ADAPTATIONSET_TYPE_e type);

/*! 
 *	\brief Get current selected stream type
 *	\param pController : controller handle
 *	\return stream type value
 *	\see 
 */
DASH_ADAPTATIONSET_TYPE_e	DASH_CTRL_GetCurrentAVType(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Toggle stream type between audio and video
 *	\param pController : controller handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_ToggleCurrentAVType(DASH_CONTROLLER_HANDLE_t pController);

/*! 
 *	\brief Set heuristics switch
 *	\param pController : controller handle
 *	\param onoff	: switch on/off
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e		DASH_CTRL_SetHeuristicsSwitch(DASH_CONTROLLER_HANDLE_t pController, HBOOL onoff);

/*! 
 *	\brief Get heuristics switch
 *	\param pController : controller handle
 *	\return heuristics switch value
 *	\see 
 */
HBOOL				DASH_CTRL_GetHeuristicsSwitch(DASH_CONTROLLER_HANDLE_t pController);

/* CB for Heuristics */
HUINT32				_CB_DASH_CTRL_GetCurDuration(void* controller);
HUINT32				_CB_DASH_CTRL_GetCurBitrate(void* controller);
HBOOL 				_CB_DASH_CTRL_GetIsProtected(void* controller);
HBOOL				_CB_DASH_CTRL_SetBandwidthStepDown(void* controller);
HBOOL				_CB_DASH_CTRL_SetBandwidthStepUp(void* controller);
HBOOL				_CB_DASH_CTRL_SetManualBandwidth(void* controller, HUINT32 ulBitrate);
HBOOL				_CB_DASH_CTRL_SetLowestBandwidth(void* controller);
HBOOL				_CB_DASH_CTRL_SetHighestBandwidth(void* controller);

/*******************************************************
*         Preconnector APIs
*******************************************************/
/*! 
 *	\brief Create preconnector
 *	\return preconnector handle
 *	\see 
 */
DASH_PRECONNECTOR_HANDLE_t DASH_PRECON_Create(void);

/*! 
 *	\brief Destroy preconnector
 *	\param hPrecon : preconnector handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_Destroy(DASH_PRECONNECTOR_HANDLE_t hPrecon);

/*! 
 *	\brief Create pool with certain number
 *	\param hPrecon : preconnector handle
 *	\param nMaxElement : the number of pool element
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_InitPool(DASH_PRECONNECTOR_HANDLE_t hPrecon, HUINT32 nMaxElement);

/*! 
 *	\brief attach detach connected item to pool
 *	\param hPrecon : preconnector handle
 *	\param node : connected item
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_Free(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_CONNECTED_t node);

/*! 
 *	\brief Flush connected list and not connected list (attach them to pool)
 *	\param hPrecon : preconnector handle
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_Flush(DASH_PRECONNECTOR_HANDLE_t hPrecon);

/*! 
 *	\brief Insert item to connected list
 *	\param hPrecon : preconnector handle
 *	\param pItem : preconnector item
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_Insert(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_ITEM_t* pItem);

/*! 
 *	\brief Get pool element
 *	\param hPrecon : preconnector handle
 *	\param node : list node (from pool)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_GetPool(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_POOL_t* node);

/*! 
 *	\brief Get not connected item from list
 *	\param hPrecon : preconnector handle
 *	\param node : list node (from not connected list)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_GetNotConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_NOTCONNECTED_t* node);

/*! 
 *	\brief Get connected item from list
 *	\param hPrecon : preconnector handle
 *	\param node : list node (from connected list)
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_GetConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_CONNECTED_t *node);

/*! 
 *	\brief Move given list node to not connected list
 *	\param hPrecon : preconnector handle
 *	\param node : list node
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_MovetoNotConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_POOL_t node);

/*! 
 *	\brief Move given list node to connected list
 *	\param hPrecon : preconnector handle
 *	\param node : list node
 *	\return Error code
 *	\see 
 */
DASH_ERROR_e 	DASH_PRECON_MovetoConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon, DASH_PRECONNECTOR_NOTCONNECTED_t node);

/*! 
 *	\brief Get the number of not connected item
 *	\param hPrecon : preconnector handle
 *	\return the number of not connected item
 *	\see 
 */
HINT32 			DASH_PRECON_GetCntNotConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon);

/*! 
 *	\brief Get the number of pool
 *	\param hPrecon : preconnector handle
 *	\return the number of pool
 *	\see 
 */
HINT32 			DASH_PRECON_GetCntPool(DASH_PRECONNECTOR_HANDLE_t hPrecon);

/*! 
 *	\brief Get the number of connected item
 *	\param hPrecon : preconnector handle
 *	\return the number of connected item
 *	\see 
 */
HINT32		 	DASH_PRECON_GetCntConnected(DASH_PRECONNECTOR_HANDLE_t hPrecon);

#endif
