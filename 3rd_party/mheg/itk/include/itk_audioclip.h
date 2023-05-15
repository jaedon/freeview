/*
 * Copyright (c) 2007 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_AUDIOCLIP_H__
#define ITK_AUDIOCLIP_H__
/**
 * @file
 * Audio Clip Interface
 */
#include "itk_avstream.h"

/**
 * @defgroup audioclip Audio Clips
 * @{
 *
 */
struct itk_audioclip_t;

/**
 * Prototype for an Audio Clip callback function
 *
 * @param[in] context The context given when the callback was registered
 * @param[in] ac The audio clip object that generated the callback
 * @param[in] ev The event type
 */
typedef void (*itk_audioclip_callback_t)(void *context, struct itk_audioclip_t *ac, itk_media_event_t ev);

/**
 * Audio Clip jump table
 */
typedef struct itk_audioclip_t
{
    /**
     * Releases an audio clip
     *
     * On return from this call the pointer to the buffer containing the
     * audio sample is invalid.
     *
     * @param[in] thiz The audio clip
     */
    void (*release)(struct itk_audioclip_t *thiz);

    /**
     * Starts presentation of an audio clip
     *
     * Presentation of the audio clip continues from the current playback cursor
     * (which is initially 0 but increments as the sample is presented).
     * Starting up presentation is asynchronous. The clip must call the listener
     * callback function with an ITK_MEDIA_PLAYING message when the presentation
     * starts and the clip was previously stopped (not paused).
     *
     * @param[in] thiz The audio clip
     */
    void (*play)(struct itk_audioclip_t *thiz);

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
    void (*stop)(struct itk_audioclip_t *thiz);

    /**
     * Pauses presentation of an audio clip
     *
     * The presentation of the sample is stopped as earliest possible point. The playback
     * cursor and loop count values are not affected. A subsequent call to play() will
     * continue presentation from the point reached at the time of the call to pause().
     *
     * @param[in] thiz The audio clip
     */
    void (*pause)(struct itk_audioclip_t *thiz);
} itk_audioclip_t;
/**
 * @}
 */

#endif
