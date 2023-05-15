/**
	@file     port_itk_avstream.c
	@brief    ITK Host AV Stream 모듈.

	Description: ITK Host AV Stream Interface.	\n
	Module: mw/mheg/itk												\n

	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include "port_itk_main.h"
#include "port_itk_display.h"
#include "port_itk_avstream.h"
#include "port_itk_service.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL_AVS		HxSEMT_Get(s_ulAvstreamSemId)
#define	LEAVE_CRITICAL_AVS		HxSEMT_Release(s_ulAvstreamSemId)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HUINT32 					s_ulAvstreamSemId = 0;
static ITK_AvstreamHandle_t 	*s_pHeadAvstream = NULL;
static ITK_AvstreamHandle_t		*s_pTailAvstream = NULL;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
#if 0
//static void 	mheg_itk_port_avsRelease(struct itk_avstream_t *thiz);
//static void 	mheg_itk_port_avsPreload(struct itk_avstream_t *thiz);
//static void 	mheg_itk_port_avsPlay(struct itk_avstream_t *thiz);
//static void 	mheg_itk_port_avsStop(struct itk_avstream_t *thiz);
static void 	mheg_itk_port_avsSelectAudioComponent(struct itk_avstream_t *thiz, itk_int32_t component);
static void 	mheg_itk_port_avsSelectVideoComponent(struct itk_avstream_t *thiz, itk_int32_t component);
static void 	mheg_itk_port_avsDeSelectAudioComponent(struct itk_avstream_t *thiz);
static void 	mheg_itk_port_avsDeSelectVideoComponent(struct itk_avstream_t *thiz);
static void 	mheg_itk_port_avsSetSpeed(struct itk_avstream_t *thiz, itk_int32_t numerator, itk_int32_t denominator);
static void 	mheg_itk_port_avsSetTrigger(struct itk_avstream_t *thiz, itk_int32_t trigger, itk_int32_t trigger_value);
static void 	mheg_itk_port_avsClearTrigger(struct itk_avstream_t *thiz, itk_int32_t trigger);
static void 	mheg_itk_port_avsSetPosition(struct itk_avstream_t *thiz, itk_int32_t position);
static void 	mheg_itk_port_avsSetEndPosition(struct itk_avstream_t *thiz, itk_int32_t end_position);
static void		mheg_itk_port_avsAppendHandle(struct ITK_AvstreamHandle_t *pstStreamHandler);
static void		mheg_itk_port_avsRemoveHandle(struct ITK_AvstreamHandle_t *pstStreamHandler);
//static void		local_itk_Display_AVS_termSem(void);		// not used
#endif

#define _____STATIC_APIs_____________________________________________________

static void mheg_itk_port_avsRemoveHandle(struct ITK_AvstreamHandle_t *pstStreamHandler)
{
	ENTER_CRITICAL_AVS;

	HxLOG_Trace();

	if (pstStreamHandler->pPrev == NULL)
	{
		s_pHeadAvstream = pstStreamHandler->pNext;
	}
	else
	{
		pstStreamHandler->pPrev->pNext = pstStreamHandler->pNext;
	}
	if (pstStreamHandler->pNext == NULL)
	{
		s_pTailAvstream = pstStreamHandler->pPrev;
	}
	else
	{
		pstStreamHandler->pNext->pPrev = pstStreamHandler->pPrev;
	}

	LEAVE_CRITICAL_AVS;
}


/**
 * Releases an avstream
 *
 * Releasing a stream *does not* stop presentation of the media. It indicates
 * that the client no longer wishes to affect which audio and video streams
 * are presented to the viewer.
 *
 * @param[in] thiz The avstream
 */
static void mheg_itk_port_avsRelease(struct itk_avstream_t *thiz)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	mheg_itk_port_avsRemoveHandle(pstStreamHandler);

	PORT_ITK_PostMsg(eMHEG_AVSTREAM_RELEASE, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, pstStreamHandler->eStreamRef, 0, sizeof(ITK_AvstreamHandle_t));

	//mheg_itk_avsReleaseStream 에서 pstStreamHandler Free 함.
	//HLIB_STD_MemFree(pstStreamHandler);
}

/**
 * Preloads an avstream
 *
 * Preloading causes the host to put the stream in a suitable state for "immediate"
 * presentation. This may mean connecting to a media server and buffering the stream
 * or dereferencing the stream reference to a stream object. When the preload is
 * complete the host will generate a ITK_MEDIA_PRELOAD event with eventData = ITK_PRESENT for
 * a successful preload and eventData = ITK_NOT_PRESENT for a failure to obtain the stream.
 *
 * @param[in] thiz The avstream
 */
static void mheg_itk_port_avsPreload(struct itk_avstream_t *thiz)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	PORT_ITK_PostMsg(eMHEG_AVSTREAM_PRELOAD, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, 0, 0, sizeof(ITK_AvstreamHandle_t));
}

/**
 * Starts presentation of an avstream
 *
 * Starting up presentation is asynchronous. The player must call the listener
 * callback function with a ITK_MEDIA_PLAYING message when the presentation starts,
 * i.e. video or audio is being presented.
 *
 * Presentation begins from the current playback marker (the beginning of the
 * media content for a new player). For continuous media this has no meaning.
 *
 * @param[in] thiz The avstream
 */
static void mheg_itk_port_avsPlay(struct itk_avstream_t *thiz)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	// TODO: 확인 필요. ( PORT_ITK_GetMhegRunningStatus ? PORT_ITK_GetMhegInternalRunningStatus?)
 	//if(PORT_ITK_GetMhegInternalRunningStatus() == FALSE) {
 	if(PORT_ITK_GetMhegRunningStatus() == FALSE) {
		HxLOG_Error("Redkey Engine should not be call this function - ignore (%s)\n", __FUNCTION__);
		return;
	}

	HxLOG_Debug("avStream(%p) Ref(%d), V(0x%x), A(0x%X) P(0x%x) \r\n",
				pstStreamHandler, pstStreamHandler->eStreamRef,	pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid);

	switch(pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_DVB:
		case eITK_REFSVC_CUR:
		case eITK_REFSVC_DEF:
		case eITK_REFSVC_LCN:
		case eITK_REFSVC_FSI:
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
#endif
			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_AVSTREAM_PLAY, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, 0, 0, sizeof(ITK_AvstreamHandle_t));
			//HLIB_STD_MemFree(pstStreamHandler);
			break;

		case eITK_REFSVC_PREV:
		case eITK_REFSVC_PVR:
			HxLOG_Debug("unsupported streamRef (0x%x) \r\n", pstStreamHandler->eStreamRef);
			break;

		case eITK_REFSVC_UNKNOWN:
		default:
			HxLOG_Warning("unknown Stream ref type \r\n");
			break;
	}
}

/**
 * Stops presentation of a media object.
 *
 * Stopping presentation is synchronous. The player must not call the listener
 * callback function when presentation ends.
 *
 * Stopping an object resets the playback cursor to the beginning of the sample
 * and resets the loop count to 0.
 *
 * @param[in] thiz The avstream
 */
static void mheg_itk_port_avsStop(struct itk_avstream_t *thiz)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(0);
		return;
	}

	switch(pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_DVB:
		case eITK_REFSVC_CUR:
		case eITK_REFSVC_DEF:
		case eITK_REFSVC_LCN:
		case eITK_REFSVC_FSI:
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
#endif
			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_AVSTREAM_STOP, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, 0, 0, sizeof(ITK_AvstreamHandle_t));
			//HLIB_STD_MemFree(pstStreamHandler);
			break;

		case eITK_REFSVC_PREV:
		case eITK_REFSVC_PVR:
			HxLOG_Debug("unsupported streamRef (0x%x) \r\n", pstStreamHandler->eStreamRef);
			break;

		case eITK_REFSVC_UNKNOWN:
		default:
			HxLOG_Debug("unknown Stream ref type \r\n");
			break;
	}
	// <---- check point #42
}

/**
 * Indicates how the player is to handle termination of video presentation.
 *
 * The possible solutions are to blank the video (term = ITK_MEDIA_BLANK) or to
 * keep the last "presentable" frame on display (blank = ITK_MEDIA_KEEP).
 * The default termination behaviour is to blank.
 *
 * @param[in] thiz The avstream
 * @param[in] term Control
 */
static void mheg_itk_port_avsSetTerminationMode(struct itk_avstream_t *thiz, itk_media_termination_t term)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if (pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	pstStreamHandler->mediaTerm = term;
}

/**
 * Selects the audio 'track' to play from the media. A component value of '-1' indicates
 * the default track. For audio this will be dependent on the chosen user language.
 *
 * An avstream can have only one active audio component. If the avstream has a selected
 * component already then this is replaced by the new component.
 *
 * If the avstream is running then the change of component is effective immediately.
 *
 * @param[in] thiz The avstream
 * @param[in] component Audio component to select
 */
static void mheg_itk_port_avsSelectAudioComponent(struct itk_avstream_t *thiz, itk_int32_t component)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if (pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	switch(pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_DVB:
		case eITK_REFSVC_CUR:
		case eITK_REFSVC_DEF:
		case eITK_REFSVC_LCN:
		case eITK_REFSVC_FSI:
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
#endif
			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_AVSTREAM_SELECT_AUDIO, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, component, 0, sizeof(ITK_AvstreamHandle_t));
			//HLIB_STD_MemFree(pstStreamHandler);
			break;

		case eITK_REFSVC_PREV:
		case eITK_REFSVC_PVR:
			HxLOG_Debug("unsupported streamRef (0x%x) \r\n", pstStreamHandler->eStreamRef);
			break;

		case eITK_REFSVC_UNKNOWN:
		default:
			HxLOG_Error("unknown Stream ref type \r\n");
			break;
	}
}

/**
 * Selects the video 'track' to play from the media. A component value of '-1' indicates
 * the default track. For video there is often only a single track
 *
 * An avstream can have only one active video component. If the avstream has a selected
 * component already then this is replaced by the new component.
 *
 * If the avstream is running then the change of component is effective immediately.
 *
 * @param[in] thiz The avstream
 * @param[in] component Video component to select
 */
static void mheg_itk_port_avsSelectVideoComponent(struct itk_avstream_t *thiz, itk_int32_t component)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	switch(pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_DVB:
		case eITK_REFSVC_CUR:
		case eITK_REFSVC_DEF:
		case eITK_REFSVC_LCN:
		case eITK_REFSVC_FSI:
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
#endif
			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_AVSTREAM_SELECT_VIDEO, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, component, 0, sizeof(ITK_AvstreamHandle_t));
			//HLIB_STD_MemFree(pstStreamHandler);
			break;

		case eITK_REFSVC_PREV:
		case eITK_REFSVC_PVR:
			HxLOG_Debug("unsupported streamRef (0x%x) \r\n", pstStreamHandler->eStreamRef);
			break;

		case eITK_REFSVC_UNKNOWN:
		default:
			HxLOG_Error("unknown Stream ref type \r\n");
			break;
	}
}

/**
 * Deselects the audio 'track' to play from the media. No audio component is
 * selected in its place
 *
 * If the avstream is running then the change of component is effective immediately.
 *
 * @param[in] thiz The avstream
 */
static void mheg_itk_port_avsDeSelectAudioComponent(struct itk_avstream_t *thiz)
{
	ITK_AvstreamHandle_t*	pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if (pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	switch(pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_DVB:
		case eITK_REFSVC_CUR:
		case eITK_REFSVC_DEF:
		case eITK_REFSVC_LCN:
		case eITK_REFSVC_FSI:
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
#endif

			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_AVSTREAM_DESELECT_AUDIO, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, (HINT32)pstStreamHandler->audioComponent, 0, sizeof(ITK_AvstreamHandle_t));
			//HLIB_STD_MemFree(pstStreamHandler);
			break;

		case eITK_REFSVC_PREV:
		case eITK_REFSVC_PVR:
			HxLOG_Debug("unsupported streamRef (0x%x) \r\n", pstStreamHandler->eStreamRef);
			break;

		case eITK_REFSVC_UNKNOWN:
		default:
			HxLOG_Error("unknown Stream ref type \r\n");
			break;
	}
}


/**
 * Deselects the video 'track' to play from the media. No video component is
 * selected in its place
 *
 * If the avstream is running then the change of component is effective immediately.
 *
 * @param[in] thiz The avstream
 */
static void mheg_itk_port_avsDeSelectVideoComponent(struct itk_avstream_t *thiz)
{
	ITK_AvstreamHandle_t*	pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	HxLOG_Trace();

	if (pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	switch(pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_DVB:
		case eITK_REFSVC_CUR:
		case eITK_REFSVC_DEF:
		case eITK_REFSVC_LCN:
		case eITK_REFSVC_FSI:
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
#endif
			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_AVSTREAM_DESELECT_VIDEO, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, (HINT32)pstStreamHandler->videoComponent, 0, sizeof(ITK_AvstreamHandle_t));
			//HLIB_STD_MemFree(pstStreamHandler);
			break;

		case eITK_REFSVC_PREV:
		case eITK_REFSVC_PVR:
			HxLOG_Debug("unsupported streamRef (0x%x) \r\n", pstStreamHandler->eStreamRef);
			break;

		case eITK_REFSVC_UNKNOWN:
		default:
			HxLOG_Error("unknown Stream ref type \r\n");
			break;
	}
}


/**
 * Sets the playback speed for the stream
 *
 * The default playback speed is 1/1. The speed can be changed when the media
 * is playing and when it is stopped. Speeds other than 1/1 and 0/1 (pause)
 * are not supported by broadcast streams.
 *
 * @param[in] thiz The avstream
 * @param[in] numerator Numerator
 * @param[in] denominator Denominator
 */
static void mheg_itk_port_avsSetSpeed(struct itk_avstream_t *thiz,
													itk_int32_t numerator,
													itk_int32_t denominator)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	HxLOG_Debug("numerator, denominator (%ld, %ld) \r\n", numerator, denominator);

	// TODO: check memory leak.
	PORT_ITK_PostMsg(eMHEG_AVSTREAM_SET_SPEED, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, numerator, denominator, sizeof(ITK_AvstreamHandle_t));
	//HLIB_STD_MemFree(pstStreamHandler);
}

/**
 * Sets a trigger to generate an event at a specified point in the presentation
 * of a stream.
 *
 * The semantics of the trigger value are dependent on the media format.
 *
 * @note The call is not supported for all media types.
 *
 * @param[in] thiz The avstream
 * @param[in] trigger Integer trigger identifier
 * @param[in] triggerValue Context specific trigger value
 */
static void mheg_itk_port_avsSetTrigger(struct itk_avstream_t *thiz,
																	itk_int32_t trigger,
																	itk_int32_t trigger_value)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	HxLOG_Debug("AVStream(%p) - trigger, value (%ld, %ld)\n", pstStreamHandler, trigger, trigger_value);

	// TODO: check memory leak.
	PORT_ITK_PostMsg(eMHEG_AVSTREAM_SET_TRIGGER, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, trigger, trigger_value, sizeof(ITK_AvstreamHandle_t));
	//HLIB_STD_MemFree(pstStreamHandler);
}

/**
 * Clears a trigger defined for a stream.
 *
 * The semantics of the trigger are dependent on the media format.
 *
 * @note The call is not supported for all media types.
 *
 * @param[in] thiz The avstream
 * @param[in] trigger Trigger identifier
 */
static void mheg_itk_port_avsClearTrigger(struct itk_avstream_t *thiz, itk_int32_t trigger)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	HxLOG_Debug("AVStream(%p) - trigger (%ld)\n", pstStreamHandler, trigger);

	// TODO: check memory leak.
	PORT_ITK_PostMsg(eMHEG_AVSTREAM_CLEAR_TRIGGER, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, trigger, 0, sizeof(ITK_AvstreamHandle_t));
	//HLIB_STD_MemFree(pstStreamHandler);
}

/**
 * Sets the playback position (the point in the stream timeline) to
 * commence playback of a stream.
 *
 * @note The call is not supported for all media types.
 *
 * @param[in] thiz The avstream
 * @param[in] position Context specific playback position identification
 */
static void mheg_itk_port_avsSetPosition(struct itk_avstream_t *thiz, itk_int32_t position)
{
	ITK_AvstreamHandle_t*		pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	switch (pstStreamHandler->eStreamRef)
	{
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			pstStreamHandler->position = position;
			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_AVSTREAM_SET_POSITION, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, position, 0, sizeof(ITK_AvstreamHandle_t));
			//HLIB_STD_MemFree(pstStreamHandler);

			HxLOG_Debug("AVStream(%p) - position : %ld\n", pstStreamHandler, position);
			break;
#endif
		default:
			HxLOG_Error("unknown or not supported reference ! [%p] \n", pstStreamHandler);
			break;
	}
}

/**
 * Sets the position (the point in the stream timeline) to
 * play to in a stream. When the position is reached presentation is finished
 * and the media generates a ITK_MEDIA_STOPPED event.
 *
 * @note The call is not supported for all media types.
 *
 * @param[in] thiz The avstream
 * @param[in] endPosition Context specific playback end position identification
 */
static void mheg_itk_port_avsSetEndPosition(struct itk_avstream_t *thiz, itk_int32_t end_position)
{
	ITK_AvstreamHandle_t*	pstStreamHandler = (ITK_AvstreamHandle_t*)thiz;

	if(pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return;
	}

	HxLOG_Debug("AVStream(%p) - end_position : %ld\n", pstStreamHandler, end_position);

	// TODO: check memory leak.
	PORT_ITK_PostMsg(eMHEG_AVSTREAM_SET_END_POSITION, pstStreamHandler->ulViewId, (HINT32)pstStreamHandler, end_position, 0, sizeof(ITK_AvstreamHandle_t));
	//HLIB_STD_MemFree(pstStreamHandler);
}

static itk_int32_t mheg_itk_port_avsGetPosition(struct itk_avstream_t *thiz)
{
	ITK_AvstreamHandle_t*	pstStreamHandler = (ITK_AvstreamHandle_t *)thiz;
	itk_int32_t		position = 0;
#if 0
	HUINT8			avStatus;
#endif

	if (pstStreamHandler == NULL)
	{
		HxLOG_Assert(pstStreamHandler);
		return 0;
	}

	/**
	 * Gets the playback position of a stream.
	 */
	switch (pstStreamHandler->eStreamRef)
	{
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			/*
				<ICS009>
				At the end of the stream playout (30 seconds) the progress bar must aso display that it has reached its completion.

				position을 av 상태에 관계없이 가져올 수 있도록 추후 변경 사항에 따라 수정되어야 할 것.
			*/
			#if 1
			position = pstStreamHandler->position;
			#else
			if( (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING )
			{
				PORT_ITK_GetMastMhegAvStatus(&avStatus);

				if (avStatus & ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING)
				{
					position = pstStreamHandler->position;
				}

			}
			#endif
			break;
#endif
		default:
			HxLOG_Error("unknown or not supported reference ! [%p] \n", pstStreamHandler);
			return 0;
	}

 	HxLOG_Debug("[AVstream(%p)]"C_CODE_F_BLACK_B_YELLOW"position(%ld)"C_CODE_RESET", V/A/PCR PID(0x%x/0x%x/0x%x) mediaStatus(0x%x) avStatus(%d/%d) avComp(%ld/%ld) eStreamRef(%d) (%x/%x)!\n",
		pstStreamHandler, position, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid,
		pstStreamHandler->mediaStatus, pstStreamHandler->audioStatus, pstStreamHandler->videoStatus, pstStreamHandler->audioComponent, pstStreamHandler->videoComponent, pstStreamHandler->eStreamRef,
		pstStreamHandler->svcUid, pstStreamHandler->serviceId);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HxLOG_Debug("avMode(%d)\n", pstStreamHandler->avMode);
#endif

	return position;
}

/**
* @brief : ICS mode와 Live 모드간 전환을 위한 sync를 맞추기 위해서 주로 사용하고 AV stream handle 존재여부 체크.
* @retval : void
*/
static void mheg_itk_port_avsAppendHandle(struct ITK_AvstreamHandle_t *pstStreamHandler)
{
	ENTER_CRITICAL_AVS;

	HxLOG_Trace();

	if (s_pHeadAvstream == NULL)
	{
		s_pHeadAvstream = pstStreamHandler;
		pstStreamHandler->pPrev = NULL;
	}
	else
	{
		s_pTailAvstream->pNext = pstStreamHandler;
		pstStreamHandler->pPrev = s_pTailAvstream;
	}
	s_pTailAvstream = pstStreamHandler;
	pstStreamHandler->pNext = NULL;

	LEAVE_CRITICAL_AVS;

}

#define _____PUBLIC_APIs_____________________________________________________



/**
 * Returns an object that can be used to control avstream presentation for
 * content sourced and presented by the host.
 *
 * @see avstream
 *
 * @param[in] thiz The display
 * @param[in] reference A content reference string
 * @param[in] presentationCount The number of times to present the stream. A value
 *            of 0 indicates that the sample should be presented continuously until
 *            it is stopped with itk_avstream_t::stop().
 * @param[in] callback A pointer to a callback function
 * @param[in] callbackContext Context data to send to the callback when it is invoked
 *
 * @return NULL if the avstream can not be created
 * @return A avstream for the content
 */
itk_avstream_t *port_itk_avs_NewAvStream(struct itk_display_t *thiz,
										 const char *reference,
										 itk_int32_t presentation_count,
										 itk_media_buffer_priority_t bufferPriority,
										 itk_avstream_callback_t callback,
										 void *callback_context)
{
	ITK_AvstreamHandle_t*	pstStreamHandler = NULL;
	HUINT32					ulViewId;
	HINT32					nOrgNetId = ITK_SVC_INVALID_ID, nTsId = ITK_SVC_INVALID_ID, nSvcId = ITK_SVC_INVALID_ID, nLcn = 0;
	HUINT32					ulFsi = 0;
	HERROR					ulErr = ERR_OK;
	HINT32					nDefSvcUid = 0, nCurSvcUid = 0, svcUid = 0;
	DxService_t				*pstSvcInfo;

	HxLOG_Trace();

	if (thiz == NULL)
	{
		HxLOG_Assert(thiz);
		return (itk_avstream_t *)NULL;
	}

	if (reference == NULL)
	{
		HxLOG_Assert(reference);

		return (itk_avstream_t *)NULL;
	}

	pstStreamHandler = (ITK_AvstreamHandle_t *)HLIB_STD_MemAlloc(sizeof(ITK_AvstreamHandle_t));
	if( pstStreamHandler == NULL)
	{
		HxLOG_Assert(0);
		return (itk_avstream_t *)NULL;
	}

	HxSTD_memset(pstStreamHandler, 0, sizeof(ITK_AvstreamHandle_t));

	/**
	* 1. link av stream handling functions.
	*/
	pstStreamHandler->vtbl.release 					= mheg_itk_port_avsRelease;
	pstStreamHandler->vtbl.preload 					= mheg_itk_port_avsPreload;
	pstStreamHandler->vtbl.play 						= mheg_itk_port_avsPlay;
	pstStreamHandler->vtbl.stop 						= mheg_itk_port_avsStop;
	pstStreamHandler->vtbl.setTerminationMode 		= mheg_itk_port_avsSetTerminationMode;
	pstStreamHandler->vtbl.selectAudioComponent 		= mheg_itk_port_avsSelectAudioComponent;
	pstStreamHandler->vtbl.selectVideoComponent 		= mheg_itk_port_avsSelectVideoComponent;
	pstStreamHandler->vtbl.deselectAudioComponent 	= mheg_itk_port_avsDeSelectAudioComponent;
	pstStreamHandler->vtbl.deselectVideoComponent 	= mheg_itk_port_avsDeSelectVideoComponent;
	pstStreamHandler->vtbl.setSpeed 					= mheg_itk_port_avsSetSpeed;
	pstStreamHandler->vtbl.setTrigger 				= mheg_itk_port_avsSetTrigger;
	pstStreamHandler->vtbl.clearTrigger 				= mheg_itk_port_avsClearTrigger;
	pstStreamHandler->vtbl.setPosition 				= mheg_itk_port_avsSetPosition;
	pstStreamHandler->vtbl.setEndPosition 			= mheg_itk_port_avsSetEndPosition;
	pstStreamHandler->vtbl.getPosition				= mheg_itk_port_avsGetPosition;


	pstStreamHandler->presentation_count 	= presentation_count;
	pstStreamHandler->bufferPriority			= bufferPriority;
	pstStreamHandler->callback 				= callback;
	pstStreamHandler->callback_context 		= callback_context;

	PORT_ITK_GetMastViewId(&ulViewId);
	pstStreamHandler->ulViewId		= ulViewId;
	pstStreamHandler->svcUid 		= 0;
	pstStreamHandler->serviceId 	= ITK_SVC_INVALID_ID;
	pstStreamHandler->audioPid 		= PID_NULL;
	pstStreamHandler->videoPid 		= PID_NULL;
	pstStreamHandler->pcrPid 		= PID_NULL;

	/** 생성시 audio, video 컴포넌트는 deselect 상태로 가정한다. */
	pstStreamHandler->audioComponent = 0;
	pstStreamHandler->videoComponent = 0;

	pstStreamHandler->mediaStatus 	= ITK_DISPLAY_AV_STATUS_CREATED;

	pstStreamHandler->bAudioFreeze = FALSE;
	pstStreamHandler->bVideoFreeze = FALSE;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	strncpy(pstStreamHandler->reference, reference, ITK_DISPLAY_REF_MAX_LEN - 1);
	pstStreamHandler->reference[ITK_DISPLAY_REF_MAX_LEN - 1] = '\0';
#endif
	/**
	* This can best be thought of by the host having an avstream object created when we call newAvstream.
	* This object reflects the settings we wish to control on the stream and the current state of playout.
	* When it is created the object is populated with the current state of the stream.
	* For most cases this means the stream is stopped with no components selected.
	* This is not the case for the avstream "rec://svc/cur".
	* In this case the initial state would be playing and the components would represent the current video
	* and audio components for the current stream.
	*/
	pstStreamHandler->eStreamRef = port_itk_svc_getReferenceService(reference, &nOrgNetId, &nTsId, &nSvcId, &nLcn, &ulFsi);
	switch(pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_DVB:
			/* get service handle by triple Ids */
			if(port_itk_svc_getSvcHandleByTripleIds(nOrgNetId, nTsId, nSvcId, &svcUid) == ERR_OK)
			{
				pstStreamHandler->svcUid = svcUid;

				pstSvcInfo = APK_META_SVC_GetService(svcUid);
				if (pstSvcInfo)
				{
					pstStreamHandler->serviceId 	= pstSvcInfo->svcid;
					pstStreamHandler->videoPid  	= pstSvcInfo->videoPid;
					pstStreamHandler->audioPid  	= pstSvcInfo->audioPid;
					pstStreamHandler->pcrPid    		= pstSvcInfo->pcrPid;
					pstStreamHandler->videoType 	= pstSvcInfo->videoCodec;
					pstStreamHandler->audioType 	= pstSvcInfo->audioCodec;

					if(pstStreamHandler->pcrPid >= PID_NULL)
					{
						/* in case of invalid pcr pid */
						if(pstStreamHandler->videoPid < PID_NULL)
							pstStreamHandler->pcrPid = pstStreamHandler->videoPid;
						else
							pstStreamHandler->pcrPid = pstStreamHandler->audioPid;
					}
					APK_META_SVC_Delete(pstSvcInfo);
				}
			}
			else
			{
				HxLOG_Error(" fail to retrieve svc handle by OrgNetId(0x%x), tsid(0x%x), svcId(0x%x) \r\n",nOrgNetId, nTsId, nSvcId);
				ulErr = ERR_FAIL;
				goto _host_display_err;
			}
			break;

		case eITK_REFSVC_DEF:

			PORT_ITK_GetSvcUid(&nDefSvcUid, &nCurSvcUid);

			/* set default service handle */
			pstStreamHandler->svcUid = nDefSvcUid;

			pstSvcInfo = APK_META_SVC_GetService(nDefSvcUid);
			if (pstSvcInfo)
			{
				pstStreamHandler->serviceId = pstSvcInfo->svcid;
				pstStreamHandler->videoPid  = pstSvcInfo->videoPid;
				pstStreamHandler->audioPid  = pstSvcInfo->audioPid;
				pstStreamHandler->pcrPid    = pstSvcInfo->pcrPid;
				pstStreamHandler->videoType = pstSvcInfo->videoCodec;
				pstStreamHandler->audioType = pstSvcInfo->audioCodec;
				if(pstStreamHandler->pcrPid >= PID_NULL)
				{
					/* in case of invalid pcr pid */
					if(pstStreamHandler->videoPid < PID_NULL)
						pstStreamHandler->pcrPid = pstStreamHandler->videoPid;
					else
						pstStreamHandler->pcrPid = pstStreamHandler->audioPid;
				}
				APK_META_SVC_Delete(pstSvcInfo);
			}
			else
			{
				goto _host_display_err;
			}
			break;

		case eITK_REFSVC_CUR:
			{
				/* get mast context informaton and set current service handle */
				PORT_ITK_GetSvcUid(&nDefSvcUid, &nCurSvcUid);

				pstStreamHandler->svcUid	= nCurSvcUid;
				pstSvcInfo = APK_META_SVC_GetService(nCurSvcUid);
				if (pstSvcInfo)
				{
					pstStreamHandler->serviceId = pstSvcInfo->svcid;
					APK_META_SVC_Delete(pstSvcInfo);
				}

				PORT_ITK_GetMasterMhegPesInfo(&pstStreamHandler->videoPid, &pstStreamHandler->audioPid, &pstStreamHandler->pcrPid, &pstStreamHandler->videoType, &pstStreamHandler->audioType);

				if(pstStreamHandler->pcrPid >= PID_NULL)
				{
					/* in case of invalid pcr pid */
					if(pstStreamHandler->videoPid < PID_NULL)
						pstStreamHandler->pcrPid = pstStreamHandler->videoPid;
					else
						pstStreamHandler->pcrPid = pstStreamHandler->audioPid;
				}

				/*  이곳에서 AV 상태를 저장 관리할 필요를 확인 해야 함.
				    OCTO는 모든것을 Watch TV가 관리하는 구조임. */
				/* set default component */
				pstStreamHandler->videoComponent 	= -1;
				pstStreamHandler->audioComponent	= -1;
			}
			break;

		case eITK_REFSVC_LCN:
			{
				HxVector_t		*pstSvcList;

				/* get delivery system from current service */
				pstSvcList = APK_META_SVC_FindServiceByNumber(nLcn);
				if (pstSvcList == NULL)
					goto _host_display_err;

				pstSvcInfo = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, 0);
				if( pstSvcInfo == NULL)
						goto _host_display_err;

				pstStreamHandler->svcUid	= pstSvcInfo->uid;
				pstStreamHandler->serviceId = pstSvcInfo->svcid;
				pstStreamHandler->videoPid  = pstSvcInfo->videoPid;
				pstStreamHandler->audioPid  = pstSvcInfo->audioPid;
				pstStreamHandler->pcrPid    = pstSvcInfo->pcrPid;
				pstStreamHandler->videoType = pstSvcInfo->videoCodec;
				pstStreamHandler->audioType = pstSvcInfo->audioCodec;

				if(pstStreamHandler->pcrPid >= PID_NULL)
				{
					/* in case of invalid pcr pid */
					if(pstStreamHandler->videoPid < PID_NULL)
						pstStreamHandler->pcrPid = pstStreamHandler->videoPid;
					else
						pstStreamHandler->pcrPid = pstStreamHandler->audioPid;
				}
				HLIB_VECTOR_Delete(pstSvcList);
			}
			break;

		case eITK_REFSVC_FSI:
#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
#if	1
			MHEG_NOT_IMPLEMENTED;
#else
			{
				HUINT32 ulSvcCount = 0;
				Handle_t *phSvcList = NULL;

				ulErr = XDB_SVC_FindServiceListByFsatIdf_Freesat(eDxDELIVERY_TYPE_SAT, eSvcType_All, eLcnVisibleSvc_FindAll, (HUINT16)ulFsi, &ulSvcCount, &phSvcList);
				if ((ulErr != ERR_OK) || (ulSvcCount == 0) || (phSvcList == NULL))
				{
					ItkAssert(("Cannot find the Fsi(0x%x)\n", ulFsi));
					if (phSvcList)
					{
						DB_SVC_FreeServiceList (phSvcList);
					}
					goto _host_display_err;
				}

				ItkTrace(("\t\t\t fsi(0x%x) SvcCount(%d)\n", ulFsi, ulSvcCount));

				/*
					freesat service id는 unique하지만 LCN중복인 서비스들이 존재하여 여러개의 service handle이 검색될수 있다
					제일 첫번째놈 사용
				*/
				pstStreamHandler->svcHandle = phSvcList[0];
				DB_SVC_GetServiceIdFromSvcHandle(pstStreamHandler->svcHandle, &pstStreamHandler->serviceId);

				/*
					db의 정보만으로 pid를 취하는데 문제 없는지 확인 필요
				*/
				ulErr = mheg_itk_port_avsGetPesInfoFromDb(pstStreamHandler->svcHandle,
										&pstStreamHandler->videoPid, &pstStreamHandler->audioPid, &pstStreamHandler->pcrPid,
										&pstStreamHandler->videoType, &pstStreamHandler->audioType);

				if(ulErr == ERR_OK)
				{
					if(pstStreamHandler->pcrPid >= PID_NULL)
					{
						/* in case of invalid pcr pid */
						if(pstStreamHandler->videoPid < PID_NULL)
							pstStreamHandler->pcrPid = pstStreamHandler->videoPid;
						else
							pstStreamHandler->pcrPid = pstStreamHandler->audioPid;
					}
				}
				else
				{
					if (phSvcList)
					{
						DB_SVC_FreeServiceList (phSvcList);
					}
					goto _host_display_err;
				}

				if (phSvcList)
				{
					DB_SVC_FreeServiceList (phSvcList);
				}
			}
#endif
#endif
			break;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			// TODO: pstStreamHandler->svcHandle = ? 필요에 따라 넣어야 할지도 ...
			pstStreamHandler->avMode = MHEG_AV_MODE_IPSTREAM;
			break;
#endif
		default:
			pstStreamHandler->eStreamRef = eITK_REFSVC_UNKNOWN;
			goto _host_display_err;

	}

	mheg_itk_port_avsAppendHandle(pstStreamHandler);

	HxLOG_Debug(C_CODE_F_BLACK_B_YELLOW"avStream(%p) ref(%s) v(0x%x) a(0x%x) pcr(0x%x) SvcId(0x%x) svcHandle(0x%x) vType(%d) aType(%d)"C_CODE_RESET"\n",
			pstStreamHandler, reference, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid,
			nSvcId, pstStreamHandler->svcUid, pstStreamHandler->videoType, pstStreamHandler->audioType);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HxLOG_Debug("avMode(%d)\n", pstStreamHandler->avMode);
#endif

	return (itk_avstream_t *)pstStreamHandler;

_host_display_err:
	if(pstStreamHandler != NULL)
	{
		HLIB_STD_MemFree(pstStreamHandler);
		pstStreamHandler = NULL;
	}

	HxLOG_Error("failed to create an avs."C_CODE_F_WHITE_B_RED"ref(%s) OrgNet(0x%x) TsId(0x%x) SvcId(0x%x) Lcn(%d) Fsi(0x%x)"C_CODE_RESET"\n",
			reference, nOrgNetId, nTsId, nSvcId, nLcn, ulFsi);

	UNUSED(ulErr);

	return (itk_avstream_t *)NULL;

}

ITK_AvstreamHandle_t* port_itk_avs_GetHandle(struct ITK_AvstreamHandle_t *pstStreamHandler)
{
	ITK_AvstreamHandle_t *pstStreamHandlerList = NULL;

	if (pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null\n");
		return NULL;
	}

	ENTER_CRITICAL_AVS;

	HxLOG_Debug("Stream handler (%p)\n", pstStreamHandler);

	pstStreamHandlerList = s_pHeadAvstream;

	while (pstStreamHandlerList != NULL)
	{
		if (pstStreamHandlerList == pstStreamHandler)
		{
			break;
		}

		pstStreamHandlerList = pstStreamHandlerList->pNext;
	}

	HxLOG_Debug("Stream handler (%p)\n", pstStreamHandlerList);

	LEAVE_CRITICAL_AVS;

	return pstStreamHandlerList;
}

void	port_itk_avs_init(void)
{
	HxSEMT_Create(&s_ulAvstreamSemId, "MhegMgrAV", HxSEMT_FIFO);
}

