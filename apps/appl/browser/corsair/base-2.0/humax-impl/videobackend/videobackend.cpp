/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software ASA.  All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Opera's
 * separate written consent.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define FAKE_DURATION 100
#define FIRE_PLAY_START 1000

#include <iostream>

#include "localdata.h"
#include "videobackend_list.h"
#include "videobackend_utils.h"

#include "include/operavideobackend_api.h"
#include "oplvod_html5.h"


using namespace std;

static char g_mimetype[] = { "audio/mpeg;audio/mp3;audio/mp4;audio/x-wav;audio/wma;video/H264;video/mp4;video/mpeg;video/mpeg4;video/x-ms-wmv;video/x-ms-asf;text/xml" };

static OperaVideoFuncs gOperaVideoFuncs;

static pthread_t		s_vbackend_thread;
static pthread_mutex_t 	s_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t 	s_thread_cond = PTHREAD_COND_INITIALIZER;
static bool				s_quitTask = false;

static void MyVideoBackendCallback(void *handle, eOplVodEventType type)
{
	OplVodHtmlFive *vodObj = static_cast<OplVodHtmlFive *>(handle);
	if ( vodObj == NULL ) return;
	if ( gOperaVideoFuncs.event == NULL ) return;

	OVB instance = (OVB)vodObj->getUserData();
	if ( instance == NULL ) return;

	LocalData *ptr = (LocalData *)(instance->bdata);
	if ( ptr == NULL ) return;

	switch ( type )
	{
	case OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED:
	{
		eOplVodPlayState state = vodObj->getPlayState();
		switch ( state )
		{
		case OPLVOD_PLAY_STATE_PLAYING:
			ptr->got_playing_event = 1;
			break;
		case OPLVOD_PLAY_STATE_STOPPED:
			ptr->is_playing = 0;
			break;
		case OPLVOD_PLAY_STATE_PAUSED:
			break;
		case OPLVOD_PLAY_STATE_CONNECTING:
			fprintf(stderr,"VideoBacnend: Callback: OPLVOD_PLAY_STATE_CONNECTING\n");
			//ptr->is_playing = 0;
			break;
		case OPLVOD_PLAY_STATE_BUFFERING:
		{
			fprintf(stderr,"VideoBacnend: Callback: OPLVOD_PLAY_STATE_BUFFERING\n");

			fprintf(stderr, "Window Setting: x:%d, y:%d, w:%d, h:%d\n", ptr->latest_rect.x,
				ptr->latest_rect.y, ptr->latest_rect.width, ptr->latest_rect.height);

			if ( !vodObj->setWindow(ptr->latest_rect.x, ptr->latest_rect.y,
					ptr->latest_rect.width, ptr->latest_rect.height) )
			{
				cerr << "Videobackend: Failed to set display rect" << endl;
			}


			/* if the "autoplay" attribute is TRUE, the playback should be started */
			if ( ptr->autoplay == 1 )
			{
				if( !vodObj->play() )
				{
					cerr << "Videobackend: Failed to play" << endl;
				}
			}

#if defined(CONFIG_TIMESTAMP_64)
			unsigned long long duration = vodObj->getDuration();
#else
			unsigned int duration = vodObj->getDuration();
#endif
			ptr->duration = duration/1000;
			ptr->know_duration = 1;
			ptr->is_playing = 1;
			ptr->buffer_finished = 1;

			OVB_Event event;
			event.type = DurationChange;
			gOperaVideoFuncs.event(instance, event);

		}
			break;
		case OPLVOD_PLAY_STATE_FINISHED:
		{
			ptr->has_ended = 1;
			ptr->is_playing = 0;
			
			OVB_Event event;
			event.user_data = 0;
			event.type = PlaybackEnd;
			gOperaVideoFuncs.event(instance, event);
			break;
		}
		case OPLVOD_PLAY_STATE_PROBE_FAILED:
		case OPLVOD_PLAY_STATE_ERROR:
		{
			ptr->is_playing = 0;
			
			OVB_Event event;
			event.user_data = 0;
			event.type = DecodeError;
			gOperaVideoFuncs.event(instance, event);
			break;
		}
		default:
			break;
		}
	}
		break;
	case OPLVOD_EVENT_TYPE_PLAY_SPEED_CHANGED:
	{
		break;
	}
	case OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED:
	{
		if ( ptr->got_playing_event == 0 ) return;

		unsigned int playPosition = vodObj->getPlayPosition();
		ptr->pos = playPosition/1000;

		OVB_Event event;
		event.user_data = 0;
		event.type = Progress;
		gOperaVideoFuncs.event(instance, event);
		break;
	}
	case OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED:
		break;
	default:
		break;
	}
}

static OVB_STATUS real_my_new(OVB instance)
{
	LocalData *ptr = (LocalData*)(instance->bdata);

	OplVodHtmlFive *vodObj = new OplVodHtmlFive();
	if ( !vodObj )
	{	
		cout << "Videobackend: Failed to initialize OplVodHtmlFive" << endl;
		return OVB_Status::ERROR;
	}

	vodObj->setUrl(ptr->url);
	vodObj->setListener(MyVideoBackendCallback);
	vodObj->setUserData((void *)instance);

	ptr->id = vodObj;

	OVB pre_playing_instance = vblist_get_playing_instance();
	if ( pre_playing_instance )
	{
		cout << "The previous playback will be stopped." << endl;
		LocalData *ptr = (LocalData*)(pre_playing_instance->bdata);
		OplVodHtmlFive *vodObj = ptr->id;
		if ( vodObj ) vodObj->stop();
	}
	
	if( !vodObj->probe() )
	{
		cerr << "Videobackend: Failed to play" << endl;
		return OVB_Status::ERROR;
	}

	ptr->is_started = 1;

	return OVB_Status::OK;
}

static void MyProbe()
{
	if ( vblist_empty() )
	{ 
		fprintf(stdout, "All instances are destroyed.\n");
		fprintf(stdout, "This thread SHALL be destroyed.\n");
	}

	OVB instance = vblist_get_latest_instance();
	if ( instance ) real_my_new(instance);
}

static void *MyVideoBackendTask(void *args)
{
	while ( !s_quitTask )
	{
		pthread_mutex_lock(&s_mutex);
		pthread_cond_wait(&s_thread_cond, &s_mutex);
		MyProbe();
		pthread_mutex_unlock(&s_mutex);
	}
}

static void my_new_alarm(int arg)
{
	pthread_cond_signal(&s_thread_cond);
}

OVB_STATUS MyNew(OVB instance, int argc, char* argn[], char* argv[]);
OVB_STATUS MyDestroy(OVB instance);
OVB_STATUS MyPlay(OVB instance);
OVB_STATUS MyPause(OVB instance);
OVB_STATUS MyNeedData(OVB instance, long long *offset, int *size, void *user_data);
OVB_STATUS MyWrite(OVB instance, void *buffer, int size, long long offset, OVB_LOAD_STATUS status, void *user_data, int *more);
OVB_STATUS MyGetPosition(OVB instance, double *pos);
OVB_STATUS MySetPosition(OVB instance, double pos);
OVB_STATUS MyGetDuration(OVB instance, double *pos);
OVB_STATUS MyGetNativeSize(OVB instance, int* width, int* height, double *pixel_aspect);
OVB_STATUS MySetDisplayRect(OVB instance, OVB_Rect rect, OVB_Rect clip, void *user_data);
OVB_STATUS MySetVolume(OVB instance, double vol);
OVB_STATUS MySetPlaybackRate(OVB instance, double rate);
OVB_STATUS MyGetFrame(OVB instance, void *data, int height, int width, int bpp);
OVB_STATUS MyGetBufferedTimeRanges(OVB instance, int *num, double start[], double end[]);
OVB_STATUS MyGetSeekableTimeRanges(OVB instance, int *num, double start[], double end[]);
OVB_STATUS MyGetPreloadSize(OVB instance, double preload_duration, long long *size);
OVB_STATUS VideoInitialize(OperaVideoFuncs* nsTable, VideoBackendFuncs* backendFuncs)
{
	fprintf(stderr, "##VideoInitialize\n");

	if ((nsTable == NULL) || (backendFuncs == NULL))
		return OVB_Status::ERROR;

	gOperaVideoFuncs.size = nsTable->size;
	gOperaVideoFuncs.version = nsTable->version;
	gOperaVideoFuncs.event = nsTable->event;
	
	backendFuncs->version = (OVB_VERSION_MAJOR << 8) + OVB_VERSION_MINOR;
	backendFuncs->size    = sizeof(VideoBackendFuncs);
	backendFuncs->newinstance = MyNew;
	backendFuncs->destroy = MyDestroy;
	backendFuncs->play    = MyPlay;
	backendFuncs->pause   = MyPause;
	backendFuncs->needdata = MyNeedData;
	backendFuncs->write = MyWrite;
	backendFuncs->getposition = MyGetPosition;
	backendFuncs->setposition = MySetPosition;
	backendFuncs->getduration = MyGetDuration;
	backendFuncs->getnativesize = MyGetNativeSize;
	backendFuncs->setdisplayrect = MySetDisplayRect;
	backendFuncs->setvolume = MySetVolume;
	backendFuncs->setplaybackrate = MySetPlaybackRate;		
	backendFuncs->getframe = MyGetFrame;
	backendFuncs->getbufferedtimeranges = MyGetBufferedTimeRanges;
	backendFuncs->getseekabletimeranges = MyGetSeekableTimeRanges;
	backendFuncs->getpreloadsize = MyGetPreloadSize;
	
	fprintf(stderr, "Version used in browser: %i.%i\n",
			nsTable->version>>8,
			nsTable->version&0xff);	
	fprintf(stderr, "Sizes: %i %i\n", nsTable->size, backendFuncs->size);

	fprintf(stderr, "Version used by backend: %i.%i\n",
			backendFuncs->version>>8,
			backendFuncs->version&0xff);	

	// Use the same version on both sides
	if (backendFuncs->version != nsTable->version)
	{
		fprintf(stderr, "Different versions, bailing out\n");
		return OVB_Status::ERROR;
	}

	int r = pthread_create(&s_vbackend_thread, NULL, MyVideoBackendTask, NULL);
	if ( r != 0 )
	{
		fprintf(stderr, "Cannot create MyProbeProcess task.\n");
		return OVB_Status::ERROR;
	}
	
	return OVB_Status::OK;
}

OVB_STATUS VideoShutdown(void)
{
	// Shut down the backend here
	fprintf(stderr, "VideoShutdown\n");

	pthread_mutex_destroy(&s_mutex);
	pthread_cond_destroy(&s_thread_cond);
	s_quitTask = true;

	return OVB_Status::OK;
}

bool SupportedMimetype(const char* type)
{
	char* haystack = const_cast<char*>(g_mimetype);
	char* result = NULL;

	while (result = strstr(haystack, ";"))
	{
		if (strncmp(type, haystack, result-haystack) == 0)			
			return true;
		
		haystack = result+1;
	} 

	// Handle first and last mimetype
	if (strcmp(type, haystack) == 0)
		return true;
	
	return 0;
}

OVB_STATUS VideoCanPlayType(const char* type, int argc, const char* codecs[])
{
#ifdef DEBUG
	fprintf(stderr, "##CanPlayType: %s codecs=", type);
	for (int i = 0; i < argc; i++)
		fprintf(stderr, "%s ", codecs[i]);
	fprintf(stderr, "\n");
#endif	

	if (SupportedMimetype(type))
		return OVB_Status::OK;
	else
		return OVB_Status::ERROR;
}

OVB_STATUS VideoCanPlayUrl(const char* url)
{
#ifdef DEBUG
	fprintf(stderr, "##CanPlayUrl: %s\n", url);
#endif
	
	return OVB_Status::OK;
}

OVB_STATUS MyNew(OVB instance, int argc, char* argn[], char* argv[])
{
	int content_size = 0;

	// Init instance
	fprintf(stderr, "##Mynew: argc=%i\n", argc);
	for (int i = 0; i < argc; i++) {
		fprintf(stderr, "*** %s = %s ***\n", argn[i], argv[i]);

		// extract content size
		if (strncmp(argn[i], "Content-Size", strlen("Content-Size")) == 0)
			content_size = atoi(argv[i]);
	}

	if ( argv[0] == NULL )
	{
		return OVB_Status::ERROR;
	}

	LocalData *ptr = (LocalData*)malloc(sizeof(LocalData));
	memset(ptr, 0, sizeof(LocalData));
	instance->bdata = ptr;

	// We don't know the duration yet
	ptr->duration = OVB_INF;
	ptr->is_playing = 0;
	ptr->is_started = 0;
	ptr->got_playing_event = 0;
	ptr->autoplay = 0;
	ptr->buffer_finished = 0;

	// Save the content size for later use
	ptr->content_size = content_size;
	memset(&(ptr->latest_rect), 0, sizeof(OVB_Rect));
	ptr->thread = NULL;
	ptr->url = strdup(argv[0]);

	vblist_add_instance(instance);

	signal(SIGALRM, my_new_alarm);
	alarm(1);

	return OVB_Status::OK;
}

OVB_STATUS MyDestroy(OVB instance)
{
#ifdef DEBUG
	fprintf(stderr, "##MyDestroy\n");
#endif	

	int iret;
	fprintf(stderr, "MyDestroy: instance=%x\n", (int)instance);
	if (!instance->bdata)
	{
		return OVB_Status::OK;
	}

	LocalData *ptr = (LocalData*)(instance->bdata);
	if ( ptr ) 
	{
		OplVodHtmlFive *vodObj = ptr->id;
		if ( vodObj ) delete vodObj;
		if ( ptr->url ) free(ptr->url);
		free(ptr);
	}

	vblist_remove_instance(instance);

	return OVB_Status::OK;
}

OVB_STATUS MyPlay(OVB instance)
{
#ifdef DEBUG
	fprintf(stderr, "##MyPlay\n");
#endif	

	/* Start a thread and send a message to the main thread */
	LocalData *ptr = (LocalData*)(instance->bdata);

	// so return duration OVB_INF
	//ptr->duration = OVB_INF;

	if ( ptr->buffer_finished == 0 )
	{
		/* the autoplay is true if MyPlay function is called before buffer finished event. */
		ptr->autoplay = 1;
		return OVB_Status::OK;
	}

	OplVodHtmlFive *vodObj = ptr->id;
	if ( !vodObj ) return  OVB_Status::ERROR;

	if( !vodObj->play() )
	{
		cerr << "Videobackend: Failed to play" << endl;
		return OVB_Status::ERROR;
	}

	ptr->is_playing = 1;

#if 0
	// Notify the browser that we now think we know a new
	// duration
	OVB_Event event;
	event.type = DurationChange;
	gOperaVideoFuncs.event(instance, event);
#endif

	return OVB_Status::OK;
}

OVB_STATUS MyPause(OVB instance)
{
#ifdef DEBUG
	fprintf(stderr, "##MyPause\n");
#endif	
	if (!instance->bdata)
		return OVB_Status::ERROR;

	LocalData *ptr = (LocalData*)(instance->bdata);

	if ( ptr->is_playing == 0 ) return OVB_Status::OK;

	OplVodHtmlFive *vodObj = ptr->id;
	if ( !vodObj ) return  OVB_Status::ERROR;

	if ( !vodObj->setPlaySpeed(0) )
	{
		cerr << "Videobackend: Failed to pause" << endl;
		return OVB_Status::ERROR;
	}
	ptr->is_playing = 0;

	return OVB_Status::OK;
}

OVB_STATUS MyNeedData(OVB instance, long long *offset, int *size, void *user_data)
{
	LocalData *ptr = (LocalData*)(instance->bdata);
#ifdef DEBUG
	fprintf(stderr, "MyNeedData: offset=%llu\n", ptr->offset);
#endif	
	*offset = ptr->offset;
	*size = (1<<16);
	
	return OVB_Status::OK;
}

OVB_STATUS MyWrite(OVB instance, void *buffer, int size, long long offset, OVB_LOAD_STATUS status, void *user_data, int *more)
{
#if 0
	LocalData *ptr = (LocalData*)(instance->bdata);	
#ifdef DEBUG	
	fprintf(stderr, "MyWrite: status=%i size=%i FILE=%x\n", status, size, (int)ptr->fp);
#endif

	int written = 0;
	if (status == OVB_LoadStatus::EOS) {
#ifdef DEBUG		
		fprintf(stderr, "Got End-Of-Stream load status\n");
#endif		
		ptr->load_eos = 1;
	}
	else if (status == OVB_LoadStatus::OK)
	{
		written = fwrite(buffer, size, 1, ptr->fp);
		if (!written)
		{
			perror("MyWrite:");
			OVB_Event event;
			event.user_data = 0;
			event.type = Error;
			gOperaVideoFuncs.event(instance, event);
			return OVB_Status::ERROR;
		}
		ptr->offset += size;

		// Consume everything
	}
	else
		fprintf(stderr, "Got an error: %i\n", status);
	
	if (!ptr->know_duration)
	{
		// Fake that we know the real duration as we
		// got some data that could be parsed
		OVB_Event event;
		event.type = DurationChange;
		gOperaVideoFuncs.event(instance, event);

		// Also say that we know the new size	
		event.type = VideoResize;
		gOperaVideoFuncs.event(instance, event);
		
		// Make sure we don't spam the browser
		// only need to send it again if it change
		ptr->know_duration = 1;
	}
#endif
	
	return OVB_Status::OK;
}

OVB_STATUS MyGetPosition(OVB instance, double *pos)
{
#ifdef DEBUG	
//	fprintf(stderr, "MyGetPosition\n");
#endif
	LocalData *ptr = (LocalData*)(instance->bdata);

	*pos = ptr->pos;

#ifdef DEBUG	
//	cout << *pos << endl;
#endif
	
	return OVB_Status::OK;
}


OVB_STATUS MySetPosition(OVB instance, double pos)
{
#ifdef DEBUG	
	fprintf(stderr, "MySetPostion: %f\n", pos);
#endif
	LocalData *ptr = (LocalData*)(instance->bdata);

	if ( ptr->buffer_finished == 0 )
	{
		if (pos == 0 && !ptr->know_duration)
		{
			OVB_Event event;
			event.type = VideoResize;
			gOperaVideoFuncs.event(instance, event);
		}
		return OVB_Status::OK;
	}

	OplVodHtmlFive *vodObj = ptr->id;
	if ( !vodObj ) return  OVB_Status::ERROR;

	if ( !vodObj->seek(pos * 1000))
	{
		cerr << "Videobackend: Failed to set position" << endl;
		return OVB_Status::ERROR;
	}

	ptr->pos = pos;
	// We done a seek so we are not playback ended anymore
	ptr->has_ended = 0;

	// If seek is possible we need to tell Opera when the seek finished
	// else it will 
	OVB_Event event;
	event.user_data = 0;
	event.type = SeekComplete;
	gOperaVideoFuncs.event(instance, event);	
	
#if 0
	// Fake that we are getting meta data
	if (pos == 0 && !ptr->know_duration)
	{
		// Say that we know the duration (default OVB_NAN = unknown)
		OVB_Event event;
		event.type = DurationChange;
		gOperaVideoFuncs.event(instance, event);

		// Also say that we know the new size	
		event.type = VideoResize;
		gOperaVideoFuncs.event(instance, event);
		
		// Only send this info once
		ptr->know_duration = 1;
	}
#endif
	
	return OVB_Status::OK;
}


OVB_STATUS MyGetDuration(OVB instance, double *pos)
{
#ifdef DEBUG
//	fprintf(stderr, "##MyGetDuration\n");
#endif
	LocalData *ptr = (LocalData*)(instance->bdata);

	*pos = ptr->duration;

#ifdef DEBUG	
//	cout << *pos << endl;
#endif
	
	return OVB_Status::OK;
}


OVB_STATUS MyGetNativeSize(OVB instance, int *width, int *height, double *pixel_aspect)
{
#ifdef DEBUG
	fprintf(stderr, "##MyGetDuration\n");
#endif

	*width=OSD_WIDTH;
	*height=OSD_HEIGHT;
	*pixel_aspect=1.0;
	return OVB_Status::OK;
}

OVB_STATUS MySetDisplayRect(OVB instance, OVB_Rect rect, OVB_Rect clip, void *user_data)
{
#ifdef DEBUG	
	//fprintf(stderr, "##MySetDisplayRect: x: %i y: %i   width: %i height: %i\n", rect.x,rect.y, rect.width, rect.height);
	//fprintf(stderr, "Cliprect: x: %i y: %i   width: %i height: %i\n", clip.x,clip.y, clip.width, clip.height);
	//fprintf(stderr, "User Data: %x\n", (int)user_data);
#endif	
	LocalData *ptr = (LocalData*)(instance->bdata);

	if ( ptr->latest_rect.x == rect.x && ptr->latest_rect.y == rect.y
			&&  ptr->latest_rect.width == rect.width &&  ptr->latest_rect.height == rect.height )
	{
		return OVB_Status::OK;
	}

	if ( ptr->buffer_finished == 1 )
	{
		OplVodHtmlFive *vodObj = static_cast<OplVodHtmlFive *>(ptr->id);	
		if ( !vodObj ) return  OVB_Status::ERROR;

		if ( !vodObj->setWindow(rect.x, rect.y, rect.width, rect.height) )
		{
			cerr << "Videobackend: Failed to set display rect" << endl;
			return OVB_Status::ERROR;
		}
	}

	memcpy(&(ptr->latest_rect), &rect, sizeof(OVB_Rect));

	return OVB_Status::OK;
}

OVB_STATUS MySetVolume(OVB instance, double vol)
{
#ifdef DEBUG
	fprintf(stderr, "##MySetVolume: %f\n", vol);
#endif	
	return OVB_Status::OK;
}

OVB_STATUS MySetPlaybackRate(OVB instance, double rate)
{
#ifdef DEBUG
	fprintf(stderr, "##MySetPlaybackRate: %f\n", rate);
#endif	
	LocalData *ptr = (LocalData*)(instance->bdata);

	if ( ptr->buffer_finished == 0 ) return OVB_Status::OK;

	OplVodHtmlFive *vodObj = ptr->id;
	if ( !vodObj ) return  OVB_Status::ERROR;

	if ( vodObj->setPlaySpeed(rate*100) == false )
	{
		return OVB_Status::ERROR;
	}

	return OVB_Status::OK;
}

OVB_STATUS MyGetFrame(OVB instance, void *data, int height, int width, int bpp)
{
#ifdef DEBUG
	fprintf(stderr, "##MyGetFrame: %x %i:%i%i\n", (int)data, height, width, bpp);
#endif
	// Don't support GetFrame in this example
	return OVB_Status::ERROR;
}

OVB_STATUS MyGetBufferedTimeRanges(OVB instance, int *num, double start[], double end[])
{
	LocalData *ptr = (LocalData*)(instance->bdata);

#if 0
	OplVodHtmlFive *vodObj = ptr->id;
	if ( !vodObj ) return OVB_Status::ERROR;

	unsigned int *pullStartOffsets, *pullEndOffsets;
	int count = vodObj->getBufferingInfo(&pullStartOffsets, &pullEndOffsets);
	if ( count <= 0 )
	{
		return OVB_Status::ERROR;
	}

	for(int i=0; i<count; i++)
	{
#ifdef DEBUG
//		fprintf(stderr,"## MyGetBufferedTimeRanges Chunks[%f] start(%f) end(%f) \n", i, pullStartOffsets[i]/1000, pullEndOffsets[i]/1000);
#endif
		start[i] = (double) (pullStartOffsets[i])/1000;
		end[i] = (double) (pullEndOffsets[i]/1000);
#ifdef DEBUG
//		fprintf(stderr,"## MyGetBufferedTimeRanges Chunks[%f] start(%f) end(%f) \n", i, start[i], end[i]);
#endif
	}

	if ( num ) *num = count;
#endif

	return OVB_Status::OK;
}

OVB_STATUS MyGetSeekableTimeRanges(OVB instance, int *num, double start[], double end[])
{
	LocalData *ptr = (LocalData*)(instance->bdata);	

	// always allow seeking to any position
	if (ptr && ptr->know_duration) {
		*num = 1;
		start[0] = 0.0;
		end[0] = ptr->duration;
	}
	else {
		*num = 0;
	}

	return OVB_Status::OK;
}

OVB_STATUS MyGetPreloadSize(OVB instance, double preload_duration, long long *size)
{
#if DEBUG
	fprintf(stderr, "##MyGetPreloadSize duration=%f\n", preload_duration);
#endif
	LocalData *ptr = (LocalData*)(instance->bdata);

	// always allow seeking to any position
	if (ptr && ptr->know_duration && ptr->content_size && !ovb_isnan(ptr->duration) && ovb_isfinite(ptr->duration)) {

		// Just assume constant bitrate
		// should get this information from the video player
		*size = ptr->content_size / ptr->duration;
#if DEBUG
		fprintf(stderr, "preload size=%llu content_size=%llu duration=%f\n", *size, ptr->content_size, ptr->duration);
#endif
		return OVB_Status::OK;
	}
	else
		// Opera will try to calculate preload size from duration and its
		// knowledge about the content size in it's HTTP Stack
		return OVB_Status::ERROR;
}
