#ifndef ___AS_APK_CAS_IRDETO_H___
#define ___AS_APK_CAS_IRDETO_H___

#include <as_apk_cas_irdeto_adaptor.h>

class cApkCasIrdeto
  : public Octopus::Appkit::Cas::Irdeto_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_CAS_IRDETO;
	static char const * const BUSPATH_APK_CAS_IRDETO;

public:
	cApkCasIrdeto(DBus::Connection &connection);

    void SetVirtualSecureCwMode(const bool& virtualSecureCwMode);
    bool GetVirtualSecureCwMode();
    std::string GetIrdetoClientVersionName();
    bool IsCWE();
    bool IsJTAG();
    bool IsSBOOT();
    bool IsOTP();
    std::string GetIrdetoManCode();
    std::string GetIrdetoHwCode();
    std::string GetIrdetoVariant();
    std::string GetIrdetoSysId();
    std::string GetIrdetoKeyVer();
    std::string GetIrdetoSigVer();
    uint32_t GetControlByte();

private:
	static void callback_Event (void);


/*
 * APK_CAS_IRDETO�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
 */
 	static cApkCasIrdeto*	pMainThis;

};

#endif
