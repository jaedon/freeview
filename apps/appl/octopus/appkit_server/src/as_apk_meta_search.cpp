#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_meta_search.h>

char const * const cApkMetaSearch::BUSNAME_APK_META_SEARCH = "Octopus.Appkit.Meta.Search";
char const * const cApkMetaSearch::BUSPATH_APK_META_SEARCH = "/Octopus/Appkit/Meta/Search";


cApkMetaSearch::cApkMetaSearch(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_META_SEARCH)
{
	/* apkit meta search */
    {	
		//APK_META_SEARCH_Init();
    }
}
