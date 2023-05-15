#ifndef ___AS_APK_META_SEARCH_H___
#define ___AS_APK_META_SEARCH_H___

#include <as_apk_meta_search_adaptor.h>

class cApkMetaSearch
  : public Octopus::Appkit::Meta::Search_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_META_SEARCH;
	static char const * const BUSPATH_APK_META_SEARCH;

public:
	cApkMetaSearch(DBus::Connection &connection);

private:


};

#endif
