/**************************************************************************************/
/**
 * @file opldlna.h
 *
 * dlna/dmp Interfaces
 *
 * @author  Munho Park(mhpar3@humaxdigital.com)
 * @date    2011/12/08
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLAIRPLAY_H__
#define __OPLAIRPLAY_H__

#include	"bpltype.h"

typedef enum
{
	eOPL_AIRPLAY_REQ_PLAY,
	eOPL_AIRPLAY_REQ_STOP,
	eOPL_AIRPLAY_REQ_SHOWPHOTO,
	eOPL_AIRPLAY_REQ_SETPOSITION,
	eOPL_AIRPLAY_REQ_SETRATE,
	eOPL_AIRPLAY_REQ_GET_PLAYBACK_INFO,
	eOPL_AIRTUNES_REQ_PLAY,
	eOPL_AIRTUNES_REQ_STOP,
	eOPL_AIRTUNES_REQ_FLUSH,
	eOPL_AIRTUNES_REQ_SETALBUMART,
	eOPL_AIRTUNESY_REQ_SETMETADATA,
	eOPL_AIRPLAY_REQ_UNKNOWN
} Opl_AIRPLAY_Request_e;

typedef enum
{
	eOPL_AIRPLAY_PBSTATUS_Unknown,
	eOPL_AIRPLAY_PBSTATUS_Pause,
	eOPL_AIRPLAY_PBSTATUS_Loading,
	eOPL_AIRPLAY_PBSTATUS_Playing,
	eOPL_AIRPLAY_PBSTATUS_ReachedEOF,
	eOPL_AIRPLAY_PBSTATUS_Stopped,
	eOPL_AIRPLAY_PBSTATUS_Error
} Opl_AIRPLAY_Pbstatus_e;

typedef struct
{
	Opl_AIRPLAY_Pbstatus_e ePlaybackStatus;
	HINT64 ullduration;
	HINT64 ullStartPosition;
	HINT64 ullPosition;
	HINT64 ullBufferedRange;
	HINT64 ullSeekableRange;
	HINT32 unRate;
} Opl_AIRPLAY_PlaybackInfo_t;

/******************************************************************
	Typedef
******************************************************************/
typedef	void	(*OPL_AIRPLAY_EventCallback)(unsigned int event, unsigned long long decimal, char* pUrl, char *title, char * album, char * artist, char * path);

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_AIRPLAY_Start( void );
BPLStatus	OPL_AIRPLAY_Stop( void );
BPLStatus	OPL_AIRPLAY_RegisterListener( OPL_AIRPLAY_EventCallback listener );
BPLStatus	OPL_AIRPLAY_UnRegisterListener( void );
BPLStatus	OPL_AIRPLAY_SetPlaybackInfo( Opl_AIRPLAY_PlaybackInfo_t tPbinfo );
BPLStatus 	OPL_AIRPLAY_IsActivated(HBOOL *bEnable);

BPLStatus  OPL_AIRPLAY_SetPlaybackStatusInfo(const char* status);
BPLStatus  OPL_AIRPLAY_SetBufferedRangeInfo(long long int buffered_range);
BPLStatus  OPL_AIRPLAY_SetSeekableRangeInfo(long long int seekable_range);
BPLStatus  OPL_AIRPLAY_SetDurationInfo(long long int duration);
BPLStatus  OPL_AIRPLAY_SetPositionInfo(long long int position);
BPLStatus  OPL_AIRPLAY_SetStartPositionInfo(long long int start_position);
BPLStatus  OPL_AIRPLAY_SetRateInfo(int rate);


#ifdef __cplusplus
};

/******************************************************************
	class 
******************************************************************/



#endif // __cplusplus
#endif // __OPLDMP_H__
