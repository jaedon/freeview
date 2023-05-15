#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_cas_mmi.h>

char const * const cApkCasMmi::BUSNAME_APK_CAS_MMI = "Octopus.Appkit.Cas.Mmi";
char const * const cApkCasMmi::BUSPATH_APK_CAS_MMI = "/Octopus/Appkit/Cas/Mmi";

cApkCasMmi * cApkCasMmi::pMainThis	=	NULL;

void cApkCasMmi::callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t param1=0, param2=0, param3=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;
	param3 = (uint32_t)ulParam3;

	pMainThis->event (param1, param2, param3);					
}

cApkCasMmi::cApkCasMmi(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_CAS_MMI)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit cas mmi */
    {	
		APK_CAS_MMI_Init();

//		void APK_CAS_MMI_UnRegisterEventNotifier(HUINT32 objId);	
    }
}

DbtMMI_Event_t cApkCasMmi::GetEvtInfo()
{
	DbtMMI_Event_t reval;

	DxCAS_MMI_Event_t *pstEvent = NULL;

	APK_CAS_MMI_GetEvtInfo (&pstEvent);
	if (NULL != pstEvent)
	{
		reval = cDbusUtilCvt::C2DxMMI_Event (pstEvent);			
		APK_CAS_MMI_RemoveEvtInfo (pstEvent);
	}

	return reval;
}

DbtMMI_Event_t cApkCasMmi::GetUiEvent(const uint32_t& sessionType, const uint32_t& handleSession)
{
	DbtMMI_Event_t	reval;

	DxCAS_MMI_Event_t *pstEvent = NULL;

	APK_CAS_MMI_GetUiEvent ((DxCAS_MmiSessionType_e)sessionType, (Handle_t)handleSession, &pstEvent);
	if (NULL != pstEvent)
	{
		reval = cDbusUtilCvt::C2DxMMI_Event (pstEvent);			
		APK_CAS_MMI_RemoveUiEvent ((DxCAS_MmiSessionType_e)sessionType, pstEvent);
	}

	return reval;
}

void cApkCasMmi::SendUiResponse(const uint32_t& groupId, const uint32_t& slot, const uint32_t& sessionType, const uint32_t& handleSession, const std::vector< uint8_t >& inputData)
{
	APK_CAS_MMI_SendUiResponse ((DxCAS_GroupId_e)groupId, (HUINT32)slot, (DxCAS_MmiSessionType_e)sessionType, (Handle_t)handleSession, (const HCHAR *)&inputData[0]);
}

void cApkCasMmi::CloseSession(const uint32_t& groupId, const uint32_t& slot, const uint32_t& sessionType, const uint32_t& handleSession)
{
	APK_CAS_MMI_UnRegisterSessionNotifier ((DxCAS_MmiSessionType_e)sessionType);

	APK_CAS_MMI_CloseSession ((DxCAS_GroupId_e)groupId, (HUINT32)slot, (DxCAS_MmiSessionType_e)sessionType, (Handle_t)handleSession);
}

uint32_t cApkCasMmi::OpenSession(const uint32_t& groupId, const uint32_t& slot, const uint32_t& sessionType, const std::vector< uint8_t >& paramData)
{
	Handle_t handle;
	handle = APK_CAS_MMI_OpenSession ((DxCAS_GroupId_e)groupId, (HUINT32)slot, (DxCAS_MmiSessionType_e)sessionType, (const char *)&paramData[0]);

	APK_CAS_MMI_RegisterSessionNotifier ((DxCAS_MmiSessionType_e)sessionType, cApkCasMmi::callback_Event);
	
	return (uint32_t)handle;
}
