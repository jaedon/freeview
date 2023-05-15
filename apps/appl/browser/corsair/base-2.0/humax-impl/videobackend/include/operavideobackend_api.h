/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2010 Opera Software ASA.  All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Opera's
 * separate written consent.
 */

#ifndef __EXTERNAL_API_HH
#define __EXTERNAL_API_HH

/**
   @mainpage Opera Video Backend API (OVB)

   @section intro Introduction
   OVB is an interface between Opera and the native video
   backend to make it easier to integrate HTML5 video into
   the Opera browser.

   Opera expect the platform to draw the video in a video plane
   behind Opera and then blend Opera on-top of this video.
   Opera will make sure that the coordinates on the screen where
   the video is positioned is painted with a chroma key color,
   same as ATVEF. And by default this use the alpha channel to
   make a hole through Opera so the video is shown below.

*/

/**
 * @defgroup operavideobackend_api Opera Video Backend API
 *
 * @{
 */

/* Compile time version of the Opera Video Backend API. */

#define OVB_VERSION_MAJOR 1
#define OVB_VERSION_MINOR 2

/* Defining some OVB types */

#include <math.h>
#define OVB_NAN NAN
#define OVB_INF INFINITY
#define ovb_isnan(a) isnan(a)
#define ovb_isfinite(a) isfinite(a)

#define OVB_STATUS int
#define OVB_LOAD_STATUS int

typedef struct
{
	int x, y, width, height;
} OVB_Rect;

#define OVB_STATUS_OK 0
#define OVB_STATUS_ERROR -1
#define OVB_STATUS_EOS 1

#ifdef __cplusplus
/** Status used when reporting back to Opera */
namespace OVB_Status {
	enum {
		ERROR = OVB_STATUS_ERROR,
		OK = OVB_STATUS_OK
	};
}

/** Load status */ 
namespace OVB_LoadStatus {
	enum {
		/** There was an error while getting the requested data */
		ERROR = OVB_STATUS_ERROR,
		/** Data request was successfull */
		OK = OVB_STATUS_OK,
		/** Tried reading data past end of stream */
		EOS = OVB_STATUS_EOS
	};
}
#endif

/** 
 *  OVB is a plug-in's opaque instance handle
 *  for the Opera Video Backend.
 */
typedef struct _OVB
{
	/** backend private data */
	void *bdata;
	/** opera private data */
	void *odata;
} OVB_t;


typedef OVB_t*  OVB;

/**
 * OVB enum even types
 */
typedef enum {
	/** Loading progress event used with external streaming */
	Progress = 0,
	/** Loading stalled event used with external streaming */
	Stalled = 1,
	/** Loading error event used with external streaming */
	Error = 2,
	/** Loading idle event used with external streaming */
	Idle = 3,
	/** The length of the media resource has become known or changed. */
	DurationChange = 4,
	/** The size of the video has become known or changed. */
	VideoResize = 5,
	/** The media resource cannot be decoded. */
	DecodeError = 6,
	/** Playback has reached the end of the media resource. */
	PlaybackEnd = 7,
	/** Needs more data to buffers, only used with Opera streaming */
	NeedData = 8,
	/** Force pause */
	ForcePause = 9,
	/** Seeking completed, need to send this else GetPosition will never be called */
	SeekComplete = 10,
	/** Fire waiting DOM event cause the next frame is not available */
	Buffering = 11,
	/** Fire playing DOM event after delayed due to lack of media data, but only fire canplay DOM event if the media element is paused */
	Resume = 12
} OVB_EventType;

/**
 *  Event structure
 */
typedef struct
{
	/** Event type */
	OVB_EventType type;
	/** Associate user data with event, used response function calls from
		Opera to backend example @see NeedDataUPP */
	void *user_data;
} OVB_Event;

/**
 * New Instance
 *
 * Called once for each new <video> or <audio> element on a page
 *
 * @param instance The handle for the new instance
 * @param argc Number of arguments
 * @param argn Arguments
 * @param argv Values
 * @return OVB_Status::OK if everything is OK
 */
typedef OVB_STATUS (* NewUPP)(OVB instance, int argc, char* argn[], char* argv[]);

/**
 * Destroy instance
 *
 * @param instance The handle for the instance
 * @return OVB_Status::OK if everything is OK
 */
typedef OVB_STATUS (* DestroyUPP)(OVB instance);

/**
 * Play
 *
 * Start playing an instance
 *
 * @param instance The handle for the instance
 * @return OVB_Status::OK if everything is OK else ERROR
 */
typedef OVB_STATUS (* PlayUPP)(OVB instance);

/**
 * Pause
 *
 * Pause the current playback, can be called even if nothing is playing.
 *
 * @param instance The handle for the instance
 * @return OVB_Status::OK if everything OK else ERROR
 */
typedef OVB_STATUS (* PauseUPP)(OVB instance);

/**
 * Need Data
 *
 * Called from the browsers as respons to the NeedData event.
 * Called from browser thread to get offset and size that
 * will be sent into Write later.
 *
 * @param[in]  instance The handle for the instance
 * @param[out] offset The offset into the stream
 * @param[out] size The size of the receiving buffer
 * @param[int] user_data Private data for this call
 * @return OVB_Status::OK if everything is OK else ERROR.
 */
typedef OVB_STATUS (* NeedDataUPP)(OVB instance, long long *offset, int *size, void *user_data);

/**
 * Write
 *
 * Called after NeedData if browser managed to fetch data from the offset
 * given in NeedData.
 *
 * @param[in]  instance The handle of the instance
 * @param[in]  buffer Buffer containing the raw stream owened by Opera
 * @param[in]  size Size of the buffer
 * @param[in]  offset Offset into the stream
 * @param[in]  status Indicates if the data request went ok, or if end of stream was
 *                    encountered or if there was an error performing the data request.
 * @param[in]  user_data Private data for this call
 * @param[out] more Set this to 1 if more data is needed, this will trigger another call to NeedDataUPP.
 * @return OVB_Status::OK if everything is OK
 */
typedef OVB_STATUS (* WriteUPP)(OVB instance, void *buffer, int size, long long offset, OVB_LOAD_STATUS status, void *user_data, int *more);

/**
 * Set Position
 *
 * Set the current playback position in seconds.
 *
 * @param instance The handle to the instance
 * @param pos Position in seconds
 * @return OVB_Status::OK if everything OK
 */
typedef OVB_STATUS (* SetPositionUPP)(OVB instance, double pos);

/**
 * GetPostion
 *
 * Get the current playback position in seconds in the stream.
 *
 * @param instance The handle to the instance
 * @param pos Will contain the position or OVB_NAN if unknown.
 * @return OVB_Status::OK if everything OK else ERROR
 */
typedef OVB_STATUS (* GetPositionUPP)(OVB instance, double *pos);

/**
 * GetDuraction
 *
 * Get the current duration in seconds for the stream.
 * If duration is unknown OVB_NAN should be returned but
 * if we know it is a stream of unknown length then OVB_INF
 * should be returned.
 *
 * Can be called multiple times and is always called after
 * DurationChange event has been posted.
 *
 * @param instance The handle to the instance
 * @param pos Will contain the duration in seconds or OVB_NAN/OVB_INF.
 * @return OVB_Status::OK if everything OK else ERROR.
 */
typedef OVB_STATUS (* GetDurationUPP)(OVB instance, double *pos);

/**
 * Get Native Size
 *
 * Get the native size of the video stream.
 * Will be called for audio streams but can be ignored, return ERROR.
 *
 * @param instance The handle to the instance
 * @param width Width of the stream
 * @param height Height of the stream
 * @param pixel_aspect Pixel aspect ratio of the stream
 * @return OVB_Status::OK if everything OK
 */
typedef OVB_STATUS (* GetNativeSizeUPP)(OVB instance, int *width, int *height, double *pixel_aspect);

/**
 * SetDisplayRect
 *
 * Give information to the backend where on the screen the video should be
 * shown and if it's clipped.
 * x,y coordinates could be negative to indicate that the origin is off screen.
 *
 * @param instance The handle to the instance
 * @param rect The coordinates to where on the screen the video should be visible
 * @param clip The clipping rectangle that could be applied
 * @param user_data User data attached to the Screen that the backend is running in.
 * @return OVB_Status::OK if everything is OK
 */
typedef OVB_STATUS (* SetDisplayRectUPP)(OVB instance, OVB_Rect rect, OVB_Rect clip, void *user_data);

/**
 * Set Volume
 *
 * Set the volume for this specific instance.
 * Max value is 1 and min is 0
 *
 * @param instance The handle to the instance
 * @param vol The volume that we want to set
 * return OVB_Status::OK if everythingis OK
 */
typedef OVB_STATUS (* SetVolumeUPP)(OVB instance, double vol);

/**
 * Set Playback Rate
 *
 * @param instance The handle to the instance
 * @param rate The playback rate
 * @return OVB_Status::OK if everything is OK
 */
typedef OVB_STATUS (* SetPlaybackRateUPP)(OVB instance, double rate);

/**
 * Get Frame
 *
 * Gets a snapshot of the current video frame in a buffer supplied from Opera.
 * Size is request through GetNativeSizeUPP just before this call.
 *
 * @param instance The handle to the instance
 * @param data A pre allocated buffer where the data should be copied
 * @param height Height of the pre-allocated buffer
 * @param width Width of the pre-allocated buffer
 * @param bpp Bits Per Pixel of the allocated buffer
 * @return OVB_Status::OK if everything OK
 */
typedef OVB_STATUS (* GetFrameUPP)(OVB instance, void *data, int height, int width, int bpp);

/**
 * Get Buffered TimeRanges
 *
 * Get buffered time ranges.
 * Calls down with a preallocated number of arrays as num parameter
 * for the start, end paramaters.
 * Should return with num as the number of ranges populated.
 *
 * @param instance The handle to the instance
 * @param num Number of ranges
 * @param start Start positions pre-allocated array defined by num param
 * @param end End postions pre-allocate array defined by num param
 * @return OVB_Status::OK if everything OK
 */
typedef OVB_STATUS (* GetBufferedTimeRangesUPP)(OVB instance, int *num, double start[], double end[]);

/**
 * Get Seekable TimeRanges
 *
 * Seekable ranges, usualy the full length of the video.
 * Calls down with a preallocated number of arrays as num parameter
 * for the start, end paramaters.
 * Should return with num as the number of ranges populated. 
 *
 * @param instance The handle to the instance
 * @param instance The handle to the instance
 * @param num Number of ranges
 * @param start Start positions pre-allocated array defined by num param
 * @param end End postions pre-allocate array defined by num param
 * @return OVB_Status::OK if everything OK
 */
typedef OVB_STATUS (* GetSeekableTimeRangesUPP)(OVB instance, int *num, double start[], double end[]);

/**
 * Get and set Preload
 *
 * Get the size of the preload converted from seconds to bytes by the
 * current instance. Can be called many times.
 * Opera will fallback to calculating Duration/FileSize if available.
 *
 * @param instance The handle to the instance
 * @param preload_duration The time that we want to set
 * @param size The number of bytes Opera should preload into its cache
 * return OVB_Status::OK if everythingis OK
 */
typedef OVB_STATUS (* GetPreloadSizeUPP)(OVB instance, double preload_duration, long long *size);


/**
 * VideoBackend function pointer struct.
 * Used when Opera calls backend functions and initated
 * in the call to VideoInitialize.
 */
typedef struct _VideoBackendFuncs {
    uint version;
    uint size;

	NewUPP newinstance;
	DestroyUPP destroy;
	PlayUPP play;
	PauseUPP pause;

	NeedDataUPP needdata;
	WriteUPP write;

   	GetPositionUPP getposition;
   	SetPositionUPP setposition;
	GetDurationUPP getduration;

	GetNativeSizeUPP getnativesize;
	SetDisplayRectUPP setdisplayrect;

	SetVolumeUPP setvolume;
	SetPlaybackRateUPP setplaybackrate;
	GetFrameUPP getframe;

	GetBufferedTimeRangesUPP getbufferedtimeranges;
	GetSeekableTimeRangesUPP getseekabletimeranges;

	GetPreloadSizeUPP getpreloadsize;
} VideoBackendFuncs;

/**
 *	Handle Event
 *
 * Called from the backend to post events that the browser should
 * act on.
 * Thread and re-entrancy safe but there is a built in delay before
 * the event is handled.
 *
 * @param instance The handle of the instance
 * @param event The event that we want Opera to act on
 * @return OVB_Status::OK if everything was OK
 */
typedef OVB_STATUS (* HandleEventUPP)(OVB instance, OVB_Event event);

/**
 * Used to call into Opera from the backend
 */
typedef struct _OperaVideoFuncs {
	uint size;
	uint version;
	HandleEventUPP event;
} OperaVideoFuncs;


#ifdef __cplusplus
extern "C" {
#endif

/* plugin meta member functions */

/**
 * Video Initialize
 *
 * Singleton alive for the lifetime of the browser.
 * Sets up function pointers between backend and Opera.
 */
typedef OVB_STATUS (* VideoInitializeP)(OperaVideoFuncs*, VideoBackendFuncs*);

/**
 * Video Shutdown
 *
 * Time to take down the video backend.
 */
typedef OVB_STATUS (* VideoShutdownP)();

/**
 * Video Can Play Type
 *
 * Browser asks backend if it can play a specific type and codec.
 * Codecs is the additional information added in the <video/audio> tag.
 *
 * Need to answer OVB_Status::OK if the backend support the requested
 * type and codec and else OVB_Status::ERROR.
 * Will be called multiple times with different values if multiple sources.
 *
 * @param type Mimetype of the media source
 * @param argc Number of arguments in the array of codecs
 * @param codecs Array of codecs for this type
 * @return OVB_Status::OK if backend supports this type else ERROR
 */
typedef OVB_STATUS (* VideoCanPlayTypeP)(const char* type, int argc, const char* codecs[]);

/**
 * Video Can Play Url
 *
 * This function is called to ask if the backend implementation want
 * to download the content itself or leave it to Opera's HTTP stack.
 *
 * @param url UTF-8 encoded URL
 * @return OVB_Status::OK if backend supports this feature else ERROR
 */
typedef OVB_STATUS (* VideoCanPlayUrlP)(const char* url);

/**
 * VideoInitialize hook
 * @see VideoInitializeP
 */
OVB_STATUS VideoInitialize(OperaVideoFuncs*, VideoBackendFuncs*);

/**
 * VideoShutdown hook
 * @see VideoShutdownP
 */
OVB_STATUS VideoShutdown(void);
/**
 * VideoCanPlayType hook
 * @see VideoCanPlayTypeP
 */	
OVB_STATUS VideoCanPlayType(const char* type, int argc, const char* codecs[]);
/**
 * VideoCanPlayUrl hook
 * @see VideoCanPlayUrlP
 */	
OVB_STATUS VideoCanPlayUrl(const char* url);

#ifdef __cplusplus
}
#endif

#endif //  __EXTERNAL_API_HH
