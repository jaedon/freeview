#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_meta_mail.h>

char const * const cApkMetaMail::BUSNAME_APK_META_MAIL = "Octopus.Appkit.Meta.Mail";
char const * const cApkMetaMail::BUSPATH_APK_META_MAIL = "/Octopus/Appkit/Meta/Mail";


cApkMetaMail::cApkMetaMail(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_META_MAIL)
{
	/* apkit meta mail */
    {	
		//APK_META_MAIL_Init();
    }
}
