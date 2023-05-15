/**
 * @note
 * Copyright (C) 2001-2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute
 * this software except in compliance with the terms and conditions
 * of any applicable license agreement in writing between Humax
 * Corporation and you.
 */

/**
 * @defgroup DLNA_MMI DLNA Player
 *
 * ¿Ã ∏µ‚¿∫ Broadcom DLNA MMI ∞¸∑√ API ∏¶ ¿ÃøÎ«— DLNA Player interface ∏¶ ¡¶∞¯«—¥Ÿ.
 *
 */

/**
 * @author huyang@humaxdigital.com
 */

#if 0

/*@{*/

/**
 * @file DI_dlna_mmi.c
 *
 */

#include "di_dlna.h"

/**
 * @b Function @b Description <br>
 * ¿Ã «‘ºˆ¥¬ «ÿ¥Á URL ∞˙ Language ∑Œ Play∏¶ ¡ÿ∫Ò«œµµ∑œ «œ¥¬ «‘ºˆ ¿‘¥œ¥Ÿ.
 *  DI_DLNA_play_start »£√‚ ¿¸ø° »£√‚µ«æÓæﬂ «’¥œ¥Ÿ.
 *
 * The DI_DLNA_play_prefetch function prepares playing the media on the given url.
 * The function allocates resources like a video plane and a decoder to play
 * the given media url. The function needs to acquire resources as much as
 * possible to reduce the start latency by the DI_DLNA_play_start function call.
 *
 * @param[in] deviceId The device ID of the target video plane
 * @param[in] urls The media urls to play.
 * @param[in] defaultAudioLangLength  - count of default audio
 * @param[in] defaultAudioLang  - value of default audio
 *
 * @return  DI_DLNA_PLAY_SUCCESS: Success <br>
 *          DI_DLNA_PLAY_NOT_SUPPORTED: The format of the media
 *                                  is not supportable.<br>
 *          DI_DLNA_PLAY_CONNECTION_ERROR: Accessing the media is not possible <br>
 *          DI_DLNA_PLAY_RESOURCE_UNAVAILABLE: Acquiring resources to render
 *                                        the media is not possible <br>
 *          DI_DLNA_PLAY_FAIL: Failure.
 */
DI_DLNA_PLAY_ERR DI_DLNA_MEDIA_Setup(HUINT32 deviceId, HUINT8* urls, HUINT32 defaultAudioLangLength, HUINT32* defaultAudioLang)
{
	DI_DLNA_PLAY_ERR nDiRet = DI_DLNA_PLAY_SUCCESS;

	PRINT_FUNC(("[++HAAL_DLNA_MEDIA_Prefetch]\n"));
	PRINT_DATA(("[HAAL_DLNA_MEDIA_Prefetch] Url-(%s)\n",pcUrls));
#if 0 /* TEST */
	//HUINT8 Url_temp[100];
	bzero(Url_temp, 100);
	VK_sprintf(Url_temp, "http://%s\0", pcUrls);
	bzero(Protocol_temp, 100);
	VK_sprintf(Protocol_temp, "http-get:*:video/mpeg:*");

	PRINT_DATA(("[HAAL_DLNA_MEDIA_Prefetch] TEST URL, TYPE - (%s)(%s)\n", Url_temp, Protocol_temp));

	BDlna_Mmi_SetUri(g_hMmi, Url_temp, Protocol_temp);
#else
	unsigned char Protocol_temp[100];
	bzero(Protocol_temp, 100);
	VK_sprintf(Protocol_temp, "http-get:*:video/mpeg:*");
	//PRINT_DATA(("[HAAL_DLNA_MEDIA_Prefetch] TEST URL, TYPE - (%s)(%s)\n", pcUrls, HAAL_DLNA_GetProtocol(pcUrls)));
	BDlna_Mmi_SetUri(g_hMmi, pcUrls, Protocol_temp);
#endif
	s_unDeviceId = unDeviceId;
	s_fCurnRate = 1.0;
	s_ullDuration 	= ullDuration;
	PRINT_FUNC(("[--HAAL_DLNA_MEDIA_Prefetch]\n"));
	return HAAL_DLNA_MEDIA_SUCCESS;











	return DI_DLNA_PLAY_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * ???®Ïàò??DI_DLNA_play_prefetch ?®Ïàò Ï§ÄÎπÑÎêú Contents ???Ä??Play Start ?©Îãà??
 *
 * The DI_DLNA_play_start function starts playing the media.
 * The fuction starts playing the media prepared at the DI_DLNA_play_prefetch.
 * @param[in] deviceId The device ID of the target video plane.
 * @return  DI_DLNA_PLAY_SUCCESS: Success. <br>
 *          DI_DLNA_PLAY_CONNECTION_ERROR: Accessing the media is not possible.<br>
 *          DI_DLNA_PLAY_FAIL: Failure.
 */
DI_DLNA_PLAY_ERR DI_DLNA_MEDIA_Start(HUINT32 deviceId)
{
	return DI_DLNA_PLAY_SUCCESS;
}


/**
 * @b Function @b Description <br>
 * Player Î•?Stop ?©Îãà??
 *
 * The DI_DLNA_play_stop function stops playing the media.
 * The function stops playing the media. The function does not change the
 * current media position. And The function maintains the last frame of the
 * media on the video plane.
 * The DI_DLNA_play_stop function can be called in other thread while DI_DLNA_play_start
 * is in operation. Then the operation of DI_DLNA_play_start should be stopped as
 * soon as possible, and return DI_DLNA_PLAY_FAIL. In that case, DI_DLNA_PLAY_stop would
 * return DI_DLNA_PLAY_SUCCESS.
 * @param[in] deviceId The device ID of the target video plane.
 * @return  DI_DLNA_PLAY_SUCCESS: Success. <br>
 *          DI_DLNA_PLAY_FAIL: Failure.
 */
DI_DLNA_PLAY_ERR DI_DLNA_MEDIA_Stop(HUINT32 deviceId)
{
	return DI_DLNA_PLAY_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * Player ??Î∞∞ÏÜç ?§Ï†ï???©Îãà??
 *
 * The DI_DLNA_play_setRate function changes the playback rate.
 * The playback rate is expressed as a ratio of the normal speed. Thus,
 * 1.0 is normal playback speed, 0.5 is half speed, 2.0 is twice speed,
 * and -1.0 is backward speed.
 * It changes the playback rate of the media as similar as it can and returns.
 * It would change the value of rate parameter to the actual rate value setted.
 * OCTO never calls DI_DLNA_play_setRate while the platform is playing the
 * media. For example, when OCTO needs to change the playback rate of
 * the media while it was playing, OCTO would call DI_DLNA_play_stop first,
 * and call DI_DLNA_play_setRate, and then call DI_DLNA_play_start finally.
 * @param[in] deviceId The device ID of the target video plane.
 * @param[in,out] rate The playback rate.
 * @return  DI_DLNA_PLAY_SUCCESS: Success. <br>
 *          DI_DLNA_PLAY_FAIL: Failure or if the media can't support a rate changing.
 */
DI_DLNA_PLAY_ERR DI_DLNA_MEDIA_SetSpeed(HUINT32 deviceId, HFLOAT32* rate)
{
	return DI_DLNA_PLAY_SUCCESS;
}

DI_DLNA_PLAY_ERR DI_DLNA_MEDIA_GetSpeed(HUINT32 deviceId, HFLOAT32* rate)
{
	return DI_DLNA_PLAY_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * Player ??Pregress position Set ?©Îãà??
 *
 * The DI_DLNA_play_setPosition function changes the current position of the media.
 * The function changes the current position of the media. The position
 * means that the media time from 0 to the duration of the media in
 * millisecond unit. OCTO never calls DI_DLNA_play_setPosition while the
 * platform is playing the media.
 * @param[in] deviceId The device ID of the target video plane.
 * @param[in] time The media time.
 * @return  DI_DLNA_PLAY_SUCCESS: Success. <br>
 *          DI_DLNA_PLAY_FAIL: If the media can't support a positioning.
 * @remarks The HUINT64 type is used to represent time in millisecond unit
 *          in this function. Please check whether your platform supports
 *          64bit type atomically or not, and implement carefully.
 */
DI_DLNA_PLAY_ERR DI_DLNA_MEDIA_SetPosition(HUINT32 deviceId, HUINT64 time)
{
	return DI_DLNA_PLAY_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * Player ??Pregress position time Í∞íÏùÑ Í∞Ä???µÎãà??
 *
 * The DI_DLNA_play_getPosition function returns the current position of the media.
 * The function returns the current position of the media in millisecond unit.
 *
 * @param[in] deviceId The device ID of the target video plane.
 * @param[out] time The media time.
 * @return  DI_DLNA_PLAY_SUCCESS: Success. <br>
 *          DI_DLNA_PLAY_FAIL: If the media doesn't have a time information.
 * @remarks The HUINT64 type is used to represent time in millisecond unit
 *          in this function. Please check whether your platform supports
 *          64bit type atomically or not, and implement carefully.
 */
DI_DLNA_PLAY_ERR DI_DLNA_MEDIA_GetPosition(HUINT32 deviceId, HUINT64 *time)
{
	return DI_DLNA_PLAY_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * ?ÑÏû¨ Play Ï§ëÏù∏ Contents ??Duration time Í∞íÏùÑ Í∞Ä???µÎãà??
 *
 * The DI_DLNA_play_getDuration function returns the duration of the media.
 * The function returns the duration of the media at normal playback speed in
 * millisecond unit. Changing the playback rate does not affect the duration.
 * The function can be called after not only the DI_DLNA_play_start function call
 * but also the DI_DLNA_play_prefetch call. If the platform can't find the duration
 * information after the DI_DLNA_play_prefetch call, the platform can return
 * DI_DLNA_PLAY_DURATION_UNKNOWN. But, If the platform can't find the duration
 * information after the DI_DLNA_play_start function call, it should return
 * DI_DLNA_PLAY_FAIL or DI_DLNA_PLAY_DURATION_UNBOUND.
 *
 * @param[in] deviceId The device ID of the target video plane.
 * @param[out] time The duration of the current media.
 * @return  DI_DLNA_PLAY_SUCCESS: Success. <br>
 *          DI_DLNA_PLAY_DURATION_UNKNOWN: The duration information is not
                                 currently available.<br>
 *          DI_DLNA_PLAY_DURATION_UNBOUND: The media source does not have a defined
                                 duration as in case of a live broadcast.<br>
 *          DI_DLNA_PLAY_FAIL:
 * @remarks The HUINT64 type is used to represent time in millisecond unit
 *          in this function. Please check whether your platform supports
 *          64bit type atomically or not, and implement carefully.
 */
DI_DLNA_PLAY_ERR DI_DLNA_MEDIA_GetDuration(HUINT32 deviceId, HUINT64 *time)
{
	return DI_DLNA_PLAY_SUCCESS;
}

/**
 * @b Function @b Description <br>
 * ???®Ïàò??Player Callback ???±Î°ù?òÎäî ?®Ïàò?ÖÎãà??
 * Player Í∞Ä Íµ¨Îèô?òÎäî Ï§ëÏóê Play Í¥Ä???¥Î≤§?∏Î? ?±Î°ù???®ÏàòÎ•??∏Ï∂ú?òÏó¨,
 * APP ?êÏÑú ?¥Î≤§?∏Î? Î∞õÏùÑ ???àÎèÑÎ°??©Îãà??
 *
 *  The DI_DLNA_play_registerEventHandler function registers the event handler.
 *  The function registers the event handler for DI_DLNA_PLAY_EVENT. The function
 *  would be called only once.
 *  @param[in] handler Player Event handler function.
 */
void DI_DLNA_MEDIA_RegisterEventHandler(DI_DLNA_PLAY_EventCallBack handler)
{
}

#endif

/*@}*/
