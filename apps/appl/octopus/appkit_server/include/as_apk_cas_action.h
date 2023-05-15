#ifndef ___AS_APK_CAS_ACTION_H___
#define ___AS_APK_CAS_ACTION_H___

#include <as_apk_cas_action_adaptor.h>

class cApkCasAction
  : public Octopus::Appkit::Cas::Action_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_CAS_ACTION;
	static char const * const BUSPATH_APK_CAS_ACTION;

public:
	cApkCasAction(DBus::Connection &connection);

    uint32_t GetActionType();
    uint32_t GetSlotId();
    uint32_t GetCasId();
    std::vector< uint8_t > GetData();

private:

	static void callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

/*
 * APK_CAS_ACTION�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
 */
 	static cApkCasAction*	pMainThis;

};

#endif
