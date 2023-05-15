#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_util_datastreamer.h>

char const * const cApkUtilDatastreamer::BUSNAME_APK_UTIL_DATASTREAMER = "Octopus.Appkit.Util.Datastreamer";
char const * const cApkUtilDatastreamer::BUSPATH_APK_UTIL_DATASTREAMER = "/Octopus/Appkit/Util/Datastreamer";

cApkUtilDatastreamer * cApkUtilDatastreamer::pMainThis	=	NULL;

void cApkUtilDatastreamer::callback_Event (HINT32 size, void *pvData)
{
	if (NULL == pMainThis)
		return;

	std::vector< uint8_t > reval;
	HINT32 i;

	for (i=0 ; i<size ; i++)
		reval.push_back(((HUINT8*)pvData)[i]);
	
	pMainThis->event (reval);					
}

cApkUtilDatastreamer::cApkUtilDatastreamer(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_DATASTREAMER)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit util datastreamer */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_DATASTREAMER_InitDataStreamer (cApkUtilDatastreamer::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_DATASTREAMER_InitDataStreamer Error : %d", hErr);
		}
	}
}

void cApkUtilDatastreamer::ConnectDataStreamer(const std::string& dataType)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DATASTREAMER_ConnectDataStreamer ((HCHAR *)&dataType[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DATASTREAMER_ConnectDataStreamer Error : %d", hErr);
	}

	return ;
}

void cApkUtilDatastreamer::DisconnectDataStreamer(const std::string& dataType)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DATASTREAMER_DisconnectDataStreamer ((HCHAR *)&dataType[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DATASTREAMER_DisconnectDataStreamer Error : %d", hErr);
	}

	return ;
}
