#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_util_drmagent.h>

char const * const cApkUtilDrmagent::BUSNAME_APK_UTIL_DRMAGENT = "Octopus.Appkit.Util.Drmagent";
char const * const cApkUtilDrmagent::BUSPATH_APK_UTIL_DRMAGENT = "/Octopus/Appkit/Util/Drmagent";

cApkUtilDrmagent * cApkUtilDrmagent::pMainThis	=	NULL;

void cApkUtilDrmagent::msg_callback_event(const HCHAR *pzResultMsg, HUINT16 usMsgId, HUINT32 ulResultCode)
{
	if (NULL == pMainThis)
		return;

	pMainThis->event (std::string(pzResultMsg), (uint32_t)usMsgId, (uint32_t)ulResultCode);	
}

void cApkUtilDrmagent::status_callback_event(const HCHAR *pzDrmSystemId)
{
	if (NULL == pMainThis)
		return;

	pMainThis->status_event (std::string(pzDrmSystemId));	
}


cApkUtilDrmagent::cApkUtilDrmagent(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_DRMAGENT)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit util drmagent */
    {	
    	HERROR hErr = ERR_FAIL;
		APK_DRMAGENT_Init ();

		hErr = APK_DRMAGENT_RegisterDrmMessageNotifier (cApkUtilDrmagent::msg_callback_event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_DRMAGENT_RegisterDrmMessageNotifier Error : %d", hErr);
		}

		hErr = APK_DRMAGENT_RegisterDrmStatusNotifier (cApkUtilDrmagent::status_callback_event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_DRMAGENT_RegisterDrmStatusNotifier Error : %d", hErr);
		}
	}
}

void cApkUtilDrmagent::SendDrmMessage(const std::string& msgType, const std::string& message, const std::string& drmSystemId, const uint32_t& msgId)
{
	APK_DRMAGENT_SendDrmMessage ((const HCHAR *)&msgType[0], (const HCHAR *)&message[0], (const HCHAR *)&drmSystemId[0], (HUINT16)msgId);
}

void cApkUtilDrmagent::SendDrmSystemStatusRequest(const std::string& drmSystemId)
{
	APK_DRMAGENT_SendDrmSystemStatusRequest ((const HCHAR *)&drmSystemId[0]);
}
