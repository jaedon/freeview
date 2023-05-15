#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_media_airplay.h>

char const * const cApkMediaAirplay::BUSNAME_APK_MEDIA_AIRPLAY = "Octopus.Appkit.Media.Airplay";
char const * const cApkMediaAirplay::BUSPATH_APK_MEDIA_AIRPLAY = "/Octopus/Appkit/Media/Airplay";

cApkMediaAirplay * cApkMediaAirplay::pMainThis	=	NULL;

STATIC const char * _apk_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
		ENUM_TO_STR (eAPK_AIRPLAY_Unknown)
		ENUM_TO_STR (eAPK_AIRPLAY_RequestPlay)
		ENUM_TO_STR (eAPK_AIRPLAY_RequestStop)
		ENUM_TO_STR (eAPK_AIRPLAY_RequestShowPhoto)
		ENUM_TO_STR (eAPK_AIRPLAY_RequestSetRate)
		ENUM_TO_STR (eAPK_AIRPLAY_RequestSetPosition)
		ENUM_TO_STR (eAPK_AIRPLAY_RequestGetPlaybackInfo)
		ENUM_TO_STR (eAPK_AIRTUNES_RequestPlay)
		ENUM_TO_STR (eAPK_AIRTUNES_RequestStop)
		ENUM_TO_STR (eAPK_AIRTUNES_RequestSetAlbumart)
		ENUM_TO_STR (eAPK_AIRTUNES_RequestSetMetadata)
		ENUM_TO_STR (eAPK_AIRTUNES_RequestFlush)
		ENUM_TO_STR (eAPK_AIRTUNES_RequestGetPlaybackInfo)

	default:
		break;
	}

	return "Unknown";
}

void cApkMediaAirplay::callback_Event (HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->event ((uint32_t)eEvent, eventStr, param1, param2);					
}

cApkMediaAirplay::cApkMediaAirplay(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_MEDIA_AIRPLAY)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit media airplay */
    {	
		APK_AIRPLAY_Init ();
		//void APK_AIRPLAY_DeInit( void );

		APK_AIRPLAY_RegisterNotifier (cApkMediaAirplay::callback_Event);
		//void APK_AIRPLAY_UnRegisterNotifier( APKS_AIRPLAY_CbHandler pfnObjectHandler );
    }
}

void cApkMediaAirplay::Start()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_AIRPLAY_Start ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_AIRPLAY_Start Error : %d", hErr);
	}

	return ;
}

void cApkMediaAirplay::Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_AIRPLAY_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_AIRPLAY_Stop Error : %d", hErr);
	}

	return ;
}

void cApkMediaAirplay::SetPlaybackInfo(const ::DBus::Struct< uint32_t, int64_t, int64_t, int64_t, int64_t, int64_t, int32_t >& playbackInfo)
{
	HERROR hErr = ERR_FAIL;
	APKS_AIRPLAY_PlaybackInfo_t stPlaybackInfo;

	{
		stPlaybackInfo.ePlaybackStatus		=	(APKE_AIRPLAY_PBStatus_e)playbackInfo._1;
		stPlaybackInfo.ullDuration			=	(HINT64)playbackInfo._2;
		stPlaybackInfo.ullStartPosition		=	(HINT64)playbackInfo._3;
		stPlaybackInfo.ullPosition			=	(HINT64)playbackInfo._4;
		stPlaybackInfo.ullBufferedRange		=	(HINT64)playbackInfo._5;
		stPlaybackInfo.ullSeekableRange		=	(HINT64)playbackInfo._6;
		stPlaybackInfo.unRate				=	(HINT32)playbackInfo._7;
	}
		
	hErr = APK_AIRPLAY_SetPlaybackInfo (stPlaybackInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_AIRPLAY_SetPlaybackInfo Error : %d", hErr);
	}

	return ;
}
