#ifndef	__AIRPLAY_INT_H__
#define	__AIRPLAY_INT_H__
#include <hlib.h>
#include <apk_deprecated.h>
#include <apk_network.h>

typedef enum
{
	eAPK_AIRPLAY_Unknown	= 0,
	eAPK_AIRPLAY_RequestPlay,
	eAPK_AIRPLAY_RequestStop,
	eAPK_AIRPLAY_RequestShowPhoto,
	eAPK_AIRPLAY_RequestSetRate,
	eAPK_AIRPLAY_RequestSetPosition,
	eAPK_AIRPLAY_RequestGetPlaybackInfo,
	eAPK_AIRTUNES_RequestPlay,
	eAPK_AIRTUNES_RequestStop,
	eAPK_AIRTUNES_RequestSetAlbumart,
	eAPK_AIRTUNES_RequestSetMetadata,
	eAPK_AIRTUNES_RequestFlush,
	eAPK_AIRTUNES_RequestGetPlaybackInfo
} APKE_AIRPLAY_Property_e;

typedef enum
{
	eAPK_AIRPLAY_PBSTATUS_Unknown,
	eAPK_AIRPLAY_PBSTATUS_Pause,
	eAPK_AIRPLAY_PBSTATUS_Loading,
	eAPK_AIRPLAY_PBSTATUS_Playing,
	eAPK_AIRPLAY_PBSTATUS_ReachedEOF,
	eAPK_AIRPLAY_PBSTATUS_Stopped,
	eAPK_AIRPLAY_PBSTATUS_Error
} APKE_AIRPLAY_PBStatus_e;

typedef struct
{
	HCHAR	contentsURL[2048];
	HINT64	nPlayPosition;
} APKS_AIRPLAY_ReqPlayParam_t;

typedef struct
{
	HINT64	nPlayPosition;
} APKS_AIRPLAY_ReqSetPositionParam_t;

typedef struct
{
	HINT32	nPlaySpeed;
} APKS_AIRPLAY_ReqSetRateParam_t;

typedef struct {
	HINT8	contentURL[2048];
}APKS_AIRTUNES_ReqPlay_t;

typedef struct {
	HCHAR szTitle[128];
	HCHAR szAlbum[128];
	HCHAR szArtist[128];
}APKS_AIRTUNES_ReqSetMetadata_t;

typedef struct {
	HCHAR szAlbumArtPath[512];
	HINT32 unAlbumArtSize;
}APKS_AIRTUNES_ReqSetAlbumart_t;


typedef struct
{
	APKE_AIRPLAY_PBStatus_e ePlaybackStatus;
	HINT64 ullDuration;
	HINT64 ullStartPosition;
	HINT64 ullPosition;
	HINT64 ullBufferedRange;
	HINT64 ullSeekableRange;
	HINT32 unRate;
} APKS_AIRPLAY_PlaybackInfo_t;

typedef void 	(*APKS_AIRPLAY_CbHandler) (HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);

extern void		APK_AIRPLAY_Init( void );
extern void		APK_AIRPLAY_DeInit( void );
extern void		APK_AIRPLAY_RegisterNotifier( APKS_AIRPLAY_CbHandler pfnObjectHandler );
extern void		APK_AIRPLAY_UnRegisterNotifier( APKS_AIRPLAY_CbHandler pfnObjectHandler );
extern HERROR	APK_AIRPLAY_Start( void );
extern HERROR	APK_AIRPLAY_Stop( void );
extern HERROR	APK_AIRPLAY_SetPlaybackInfo(APKS_AIRPLAY_PlaybackInfo_t tPlaybackInfo);

#endif
