#include <apk.h>
#include <oapi_media_rec.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_media_record.h>

char const * const cApkMediaRecord::BUSNAME_APK_MEDIA_RECORD = "Octopus.Appkit.Media.Record";
char const * const cApkMediaRecord::BUSPATH_APK_MEDIA_RECORD = "/Octopus/Appkit/Media/Record";


STATIC const char * _apk_GetEventString (OxMediaRec_Event_e eEvent)
{
	switch (eEvent)
	{
		ENUM_TO_STR (eOxMR_EVENT_REC_STARTED)
		ENUM_TO_STR (eOxMR_EVENT_REC_STOPPED)
		ENUM_TO_STR (eOxMR_EVENT_REC_TIME_CHANGED)
		ENUM_TO_STR (eOxMR_EVENT_REC_STATE_CHANGED)
		ENUM_TO_STR (eOxMR_EVENT_REC_CONTENTS_CHANGED)
		ENUM_TO_STR (eOxMR_EVENT_REC_TSRBUFFER_COPYEND)

	default:
		break;
	}

	return "Unknown";
}


void cApkMediaRecord::callback_Event (HUINT32 ulSvcUid, HUINT32 ulSlot, OxMediaRec_Event_e eEvent, OxMediaRec_NotifierData_t *pstNotifyData, void *pvUserData)
{
	cApkMediaRecord *pThis = (cApkMediaRecord *)pvUserData;
	std::string	eventStr(_apk_GetEventString(eEvent));

	uint32_t param1=0, param2=0, param3=0;

	/* emit signal */
	if (NULL != pstNotifyData)
	{
//		param1 = (uint32_t)pstNotifyData->stRecStarted.ulParam1;
//		param2 = (uint32_t)pstNotifyData->stRecStarted.ulParam2;
//		param3 = (uint32_t)pstNotifyData->stRecStarted.ulParam3;
	}

	pThis->event ((uint32_t)eEvent, eventStr, param1, param2, param3);					
}

cApkMediaRecord::cApkMediaRecord(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_MEDIA_RECORD)
{
	/* apkit media record */
    {
		APK_MEDIA_RECORD_Init();
    }
}

bool cApkMediaRecord::IsRecording(const uint32_t& svcUid)
{
	HERROR hErr;

	HBOOL isRecording = FALSE;

	hErr= APK_MEDIA_RECORD_IsRecording ((HUINT32)svcUid, &isRecording);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_IsRecording : Error");
	}

	return isRecording;
}

uint32_t cApkMediaRecord::GetCID(const uint32_t& svcUid)
{
	HERROR hErr;

	HUINT32 cid;

	hErr= APK_MEDIA_RECORD_GetCID ((HUINT32)svcUid, &cid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_GetCID : Error");
	}

	return (uint32_t)cid;
}

uint32_t cApkMediaRecord::Start(const ::DBus::Struct< uint32_t, uint32_t, uint32_t, int32_t, uint32_t, uint32_t, uint32_t, std::vector< uint8_t >, int32_t, std::string >& startInfo)
{
	HERROR hErr;

	OxMediaRec_StartInfo_t stStart;
	HUINT32 ulSessionId;
	HCHAR szRecordID[80];

	{	
		HxSTD_memset (&stStart, 0, sizeof(OxMediaRec_StartInfo_t));

		stStart.ulMasterSvcUid	=	startInfo._1;
		stStart.ulSuppleSvcUid	=	startInfo._2;
		stStart.eSuppSvcType	=	(OxMediaPlay_SuppleSvcType_e)startInfo._3;
		stStart.nStartOffset	=	startInfo._4;	

		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime((UNIXTIME)startInfo._5, &(stStart.stStartTime));

		stStart.ulDuration		=	startInfo._6;
		stStart.eStorageType	=	(OxMediaRec_TargetStorage_e)startInfo._7;

		if (startInfo._8.size() > 0)
			HLIB_STD_StrNCpySafe(stStart.szStorageId, (HCHAR*)&startInfo._8[0], sizeof(stStart.szStorageId)-1);

		stStart.nEvtId			=	startInfo._9;
		
		HLIB_STD_StrNCpySafe(stStart.szName, (HCHAR*)&startInfo._10[0], sizeof(stStart.szName)-1);	
	}

	hErr= APK_MEDIA_RECORD_Start (&stStart, &ulSessionId, szRecordID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_Start : Error");
	}
	else
	{
		APK_MEDIA_RECORD_RegisterNotifier (stStart.ulMasterSvcUid, cApkMediaRecord::callback_Event, this);
	}

	return (uint32_t)ulSessionId;

}

void cApkMediaRecord::Stop(const uint32_t& sessionId)
{
	HERROR hErr;

	hErr= APK_MEDIA_RECORD_Stop ((HUINT32)sessionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_Stop : Error");
	}
	else
	{
		// sessionId to svcId « ø‰«‘.
		//APK_MEDIA_RECORD_UnregisterNotifier (stStart.ulMasterSvcUid, cApkMediaRecord::callback_Event, this);
	}

	return ;

}

void cApkMediaRecord::StopByUID(const uint32_t& svcUid)
{
	HERROR hErr;

	hErr= APK_MEDIA_RECORD_StopByUID ((HUINT32)svcUid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_StopByUID : Error");
	}

	return ;

}

uint32_t cApkMediaRecord::GetRecTime(const uint32_t& sessionId)
{
	HERROR hErr;
	
	HUINT32 ulRecTime;

	hErr= APK_MEDIA_RECORD_GetRecTime ((HUINT32)sessionId, &ulRecTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_GetRecTime : Error");
	}

	return (uint32_t)ulRecTime;

}

uint32_t cApkMediaRecord::GetStartTime(const uint32_t& sessionId)
{
	HERROR hErr;

	HUINT32 ulStartTime;

	hErr= APK_MEDIA_RECORD_GetStartTime ((HUINT32)sessionId, &ulStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_GetStartTime : Error");
	}

	return (uint32_t)ulStartTime;

}

std::string cApkMediaRecord::GetURL(const uint32_t& sessionId)
{
	HERROR hErr;
	std::string reval;

	HCHAR szRecordID[80];
		
	hErr= APK_MEDIA_RECORD_GetURL((HUINT32)sessionId, szRecordID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_GetURL : Error");
	}

	return reval;
}

uint32_t cApkMediaRecord::GetRsvSlotByUID(const uint32_t& svcUid)
{
	HERROR hErr;

	HUINT32 ulRsvSlot;

	hErr= APK_MEDIA_RECORD_GetRsvSlotByUID ((HUINT32)svcUid, &ulRsvSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_RECORD_GetRsvSlotByUID : Error");
	}

	return (uint32_t)ulRsvSlot;
}
