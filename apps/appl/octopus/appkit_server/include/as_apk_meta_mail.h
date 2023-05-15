#ifndef ___AS_APK_META_MAIL_H___
#define ___AS_APK_META_MAIL_H___

#include <as_apk_meta_mail_adaptor.h>

class cApkMetaMail
  : public Octopus::Appkit::Meta::Mail_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_META_MAIL;
	static char const * const BUSPATH_APK_META_MAIL;

public:
	cApkMetaMail(DBus::Connection &connection);

	int32_t Foo(const uint32_t& in_param);

private:


};

#endif
