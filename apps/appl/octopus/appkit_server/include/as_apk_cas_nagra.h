#ifndef ___AS_APK_CAS_NAGRA_H___
#define ___AS_APK_CAS_NAGRA_H___

#include <as_apk_cas_nagra_adaptor.h>

class cApkCasNagra
  : public Octopus::Appkit::Cas::Nagra_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_CAS_NAGRA;
	static char const * const BUSPATH_APK_CAS_NAGRA;

public:
	cApkCasNagra(DBus::Connection &connection);

    bool IsCWE();
    bool IsJTAG();
    bool IsSBOOT();
    bool IsOTP();
    std::string GetNUID();
    std::string GetCASN();
    ::DBus::Struct< bool, bool, bool, bool > GetFusingInfo();
    bool IsSCInserted();
    bool VerifyPincode(const std::string& pinStr);
    ::DBus::Struct< uint32_t, uint32_t, uint32_t, bool > GetAccessibleState();
    ::DBus::Struct< uint32_t, uint32_t, uint32_t, bool > VerifyPincodeGetAccessibleState(const std::string& pinStr);
    bool SetPincode(const std::string& oldPinStr, const std::string& newPinStr);

private:

	static void callback_Event (HBOOL *pbRet);

/*
 * APK_CAS_NAGRA�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
 */
 	static cApkCasNagra*	pMainThis;


};

#endif
