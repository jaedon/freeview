/*
 *
 *
 *
 */

#ifndef __LOCALDATA_H__
#define __LOCALDATA_H__

#include <cstdlib>

#include <pthread.h>
#include "include/operavideobackend_api.h"
#include "oplvod_html5.h"

typedef struct {	
	pthread_t *thread;
	char *url;
	long long offset;    // Current offset in the file
	double pos;          // Current playback position
	double duration;     // Current known duration	
	char is_playing;     // Playing or Pause
	char is_started;     // Playback started
	char got_playing_event;     // if get the playing event
	char autoplay;     // Playing or Pause
	int load_eos;        // Load status End-Of-Stream
	char has_ended;      // Playback ended
	char buffer_finished;// initial buffering is finished
	char know_duration;  // Dummy flag to not spam the browser
	long long content_size; // Size of the video we are loading if not streaming
	OplVodHtmlFive *id;
	OVB_Rect latest_rect;
} LocalData; 


#endif
