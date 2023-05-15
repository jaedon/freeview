#ifndef ___AS_APK_CAS_MMI_H___
#define ___AS_APK_CAS_MMI_H___

#include <as_apk_cas_mmi_adaptor.h>

class cApkCasMmi
  : public Octopus::Appkit::Cas::Mmi_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_CAS_MMI;
	static char const * const BUSPATH_APK_CAS_MMI;

public:
	cApkCasMmi(DBus::Connection &connection);

    DbtMMI_Event_t GetEvtInfo();
    DbtMMI_Event_t GetUiEvent(const uint32_t& sessionType, const uint32_t& handleSession);
    void SendUiResponse(const uint32_t& groupId, const uint32_t& slot, const uint32_t& sessionType, const uint32_t& handleSession, const std::vector< uint8_t >& inputData);
    void CloseSession(const uint32_t& groupId, const uint32_t& slot, const uint32_t& sessionType, const uint32_t& handleSession);
    uint32_t OpenSession(const uint32_t& groupId, const uint32_t& slot, const uint32_t& sessionType, const std::vector< uint8_t >& paramData);

private:
	
	static void callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
	

	/*
	 * APK_CAS_MMI�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
	   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
	 */
	static cApkCasMmi*	pMainThis;


};

#endif
