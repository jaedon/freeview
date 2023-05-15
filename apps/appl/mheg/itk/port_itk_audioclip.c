/**
	@file     port_itk_audioclip.c
	@brief    ITK Host audio Clip 모듈.

	Description: ITK Host Audio Clip Interface.	\n
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

#include <itk.h>

#include "port_itk_main.h"
#include "port_itk_display.h"
#include "port_itk_audioclip.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/** @brief Display audioclip handle type */

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
void  	*g_pCurAudioClip = NULL;
void  	*g_pStoppedAudioClip = NULL; /* SVC_AV_StopAudioClip 함수가 호출되면 eSEVT_AV_AUDIO_CLIP_COMPLETED 메시지가 Mheg mgr로 전달된다.
										이 메시지가 전달되기 전에 new audio clip이 play(생성)가 되면 즉시 stop 되는 문제가 발생(sync문제) 하여
										stop된 audio clip의 Object를 저장했다가 이 메시지가 전달되면 new audio clip을 stop하지 않도록 하고
										이전에 stop된 audio clip의 callback을 호출하도록 하기 위한 변수*/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

#define ___STATIC_APIS__________________________________________________________

/**
 * Releases an audio clip
 *
 * On return from this call the pointer to the buffer containing the
 * audio sample is invalid.
 *
 * @param[in] thiz The audio clip
 */
static void mheg_itk_port_audioClipRelease(struct itk_audioclip_t *thiz)
{
	ITK_AudioclipHandle_t *ac = NULL;

	HxLOG_Trace();

	ac = (ITK_AudioclipHandle_t *)thiz;
	if (ac == NULL)
	{
		HxLOG_Error("input parameter is invalid NULL\r\n");
		return;
	}

	if (ac->ucAudioEv != eITK_DISPLAY_EVENT_STOP)
	{
		//stop audio clip
		APK_MEDIA_PLAY_StopAudioClip(0);
		ac->ucAudioEv = eITK_DISPLAY_EVENT_STOP;

		g_pStoppedAudioClip = (void*)ac;
	}

	if (ac->content != NULL)
	{
		HLIB_STD_MemFree(ac->content);
		ac->content = NULL;
	}
	HLIB_STD_MemFree(ac);
	ac = NULL;
}

/**
 * Starts presentation of an audio clip
 *
 * Presentation of the audio clip continues from the current playback cursor
 * (which is initially 0 but increments as the sample is presented).
 * Starting up presentation is asynchronous. The clip must call the listener
 * callback function with a ITK_MEDIA_PLAYING message when the presentation
 * starts and the clip was previously stopped (not paused).
 *
 * @param[in] thiz The audio clip
 */
static void mheg_itk_port_audioClipPlay(struct itk_audioclip_t *thiz)
{
	ITK_AudioclipHandle_t *ac = NULL;

	ac = (ITK_AudioclipHandle_t *)thiz;
	if (thiz == NULL)
	{
		HxLOG_Error("input parameter is invalid NULL\r\n");
		return;
	}

	HxLOG_Debug("audioclip play (%p), ac->ucAudioEv (%d)\n", ac, ac->ucAudioEv);

	if (ac->ucAudioEv == eITK_DISPLAY_EVENT_STOP)
	{
		if ((ac->content == NULL) || (ac->content_size == 0))
		{
			HxLOG_Error(" audio content or content_size is NULL\r\n");
			return;
		}

		//play audio clip
		APK_MEDIA_PLAY_StartAudioClip(0, ac->content, ac->content_size, eDxAUDIO_CODEC_MPEG, ac->presentation_count);
		ac->ucAudioEv = eITK_DISPLAY_EVENT_PLAY;
		ac->callback(ac->callback_context, (itk_audioclip_t *)ac, ITK_MEDIA_PLAYING);
		g_pCurAudioClip = ac;
	}
	else if (ac->ucAudioEv == eITK_DISPLAY_EVENT_PAUSE)
	{
		//resume audio clip
		APK_MEDIA_PLAY_ResumeAudioClip(0);
		ac->ucAudioEv = eITK_DISPLAY_EVENT_PLAY;
	}
	else
	{
		HxLOG_Debug(" audio clip is already playing\n");
		return;
	}

	HxLOG_Debug(" audio clip played\n");
}

/**
 * Stops presentation of an audio clip
 *
 * Stopping presentation is synchronous. The player must not call the listener
 * callback function when presentation ends.
 *
 * Stopping a clip resets the loop count and playback cursor. A subsequent call to
 * play() will start presentation from the beginning of the sample.
 *
 * @param[in] thiz The audio clip
 */
static void mheg_itk_port_audioClipStop(struct itk_audioclip_t *thiz)
{
	ITK_AudioclipHandle_t *ac = NULL;

	ac = (ITK_AudioclipHandle_t *)thiz;
	if (ac == NULL)
	{
		HxLOG_Error("input parameter is invalid NULL\r\n");
		return;
	}
	HxLOG_Debug("audioclip stop...\r\n");

	if (ac->ucAudioEv != eITK_DISPLAY_EVENT_STOP)
	{
		//stop audio clip
		APK_MEDIA_PLAY_StopAudioClip(0);
		ac->ucAudioEv = eITK_DISPLAY_EVENT_STOP;

		g_pStoppedAudioClip = (void*)ac;
	}
	else
	{
		HxLOG_Debug(" audio clip is already stopped.\n");
	}
	HxLOG_Debug(" audio clip stopped\n");
}

/**
 * Pauses presentation of an audio clip
 *
 * The presentation of the sample is stopped as earliest possible point. The playback
 * cursor and loop count values are not affected. A subsequent call to play() will
 * continue presentation from the point reached at the time of the call to pause().
 *
 * @param[in] thiz The audio clip
 */
static void mheg_itk_port_audioClipPause(struct itk_audioclip_t *thiz)
{
	ITK_AudioclipHandle_t *ac = NULL;

	ac = (ITK_AudioclipHandle_t *)thiz;
	if (thiz == NULL)
	{
		HxLOG_Error("input parameter is invalid NULL\r\n");
		return;
	}

	HxLOG_Debug("audioclip pause...\r\n");

	if (ac->ucAudioEv == eITK_DISPLAY_EVENT_PLAY)
	{
		//pause audio clip
		APK_MEDIA_PLAY_PauseAudioClip(0);
		ac->ucAudioEv = eITK_DISPLAY_EVENT_PAUSE;
	}
	else
	{
		HxLOG_Error(" can't pause audio clip.\n");
		return;
	}

	HxLOG_Debug(" audio clip is paused.\n");
}

#define	___PUBLIC_INTERNAL_FUNCTION____

/**
 * Returns an object that can be used to present an audio clip.
 *
 * @see audioclip
 *
 * @note Unlike struct itk_display_t::newImage(), the content buffer referenced
 * in the call remains valid for the lifetime of the itk_audioclip_t (i.e.
 * until it is released). This means that the host does not need to make
 * a local copy of the audio sample (although it is free to do so if this
 * is required).
 *
 * @param[in] thiz The display
 * @param[in] content A pointer to a buffer that holds the content to present
 * @param[in] contentSize The nmber of bytes of content in the buffer
 * @param[in] presentationCount The number of times to present the sample, 0
 *            indicates that the sample should be presented continuously until
 *            it is stopped with itk_audioclip_t::stop().
 * @param[in] callback A pointer to a callback function to be invoked when
 *            playback begins or the final presentation is made
 * @param[in] callbackContext Context data to send to the callback when it is invoked
 *
 * @return NULL if the media player can not be created
 * @return An audioclip for the content
 */
itk_audioclip_t	*port_itk_audioclip_NewAudioClip(struct itk_display_t *thiz, const void *content, size_t content_size,
												 itk_int32_t presentation_count, itk_audioclip_callback_t callback, void *callback_context)
{
	ITK_AudioclipHandle_t *ac = NULL;

	if (thiz == NULL)
	{
		HxLOG_Error("input parameter is invalid NULL\r\n");
		return (itk_audioclip_t *)NULL;
	}

	if (content == NULL || content_size == 0)
	{
		HxLOG_Error("content or content_size is NULL\r\n");
		return (itk_audioclip_t *)NULL;
	}

	ac = (ITK_AudioclipHandle_t *)HLIB_STD_MemAlloc(sizeof(ITK_AudioclipHandle_t));

	/**
	 * failed to allocate audio clip.
	 */
	if (ac == NULL)
	{
		HxLOG_Error("audio clip is NULL\r\n");
		return (itk_audioclip_t *)NULL;
	}
	else
	{
		memset(ac, 0, sizeof(itk_audioclip_t));
	}

	/**
	 * 1. link audio clip handling functions.
	 * 2. content
	 */
	ac->vtbl.release = mheg_itk_port_audioClipRelease;
	ac->vtbl.play = mheg_itk_port_audioClipPlay;
	ac->vtbl.stop = mheg_itk_port_audioClipStop;
	ac->vtbl.pause = mheg_itk_port_audioClipPause;
	ac->content = (void *)HLIB_STD_MemAlloc(content_size);

	/**
	 * failed to allocate audio clip content.
	 */
	if (ac->content == NULL)
	{
		HxLOG_Error("ac->content allocation failed.\r\n");
		goto _host_display_err;
	}
	memcpy(ac->content, content, content_size);

	/**
	 * 3. content_size
	 * 4. presentation_count
	 * 5. callback
	 * 6. callback_context
	 * 7. audio id ???
	 * 8. audio event (무조건 STOP으로 설정)
	 */
	ac->content_size = content_size;
	ac->presentation_count = presentation_count;
	ac->callback = callback;
	ac->callback_context = callback_context;
	ac->ucAudioEv = eITK_DISPLAY_EVENT_STOP;

	HxLOG_Debug("ac created. content_size (%d), presentation_count (%ld), audioev (%d)\n",
						ac->content_size, ac->presentation_count, ac->ucAudioEv);

	return (itk_audioclip_t *)ac;

_host_display_err:
	if (ac->content != NULL)
	{
		HLIB_STD_MemFree(ac->content);
		ac->content = NULL;
	}
	HLIB_STD_MemFree(ac);
	ac = NULL;

	HxLOG_Error("failed to create an ac.\r\n");
	return (itk_audioclip_t *)NULL;
}

#define	___PUBLIC_FUNCTION____
void	*PORT_ITK_AUDIOCLIP_getCurHandle(void)
{
	return g_pCurAudioClip;
}

void	PORT_ITK_AUDIOCLIP_setCurHandle(void *pCurAudioClip)
{
	g_pCurAudioClip = pCurAudioClip;
}

void	*PORT_ITK_AUDIOCLIP_getStoppedHandle(void)
{
	return g_pStoppedAudioClip;
}

void	PORT_ITK_AUDIOCLIP_setStoppedHandle(void *pStoppedAudioClip)
{
	g_pStoppedAudioClip = pStoppedAudioClip;
}

void	PORT_ITK_AUDIOCLIP_Restore(void)
{
	HxLOG_Debug("restore Audio Clip (%p)\n");
	APK_MEDIA_PLAY_StopAudioClip(0);
	g_pCurAudioClip = NULL;
	g_pStoppedAudioClip = NULL;
}

