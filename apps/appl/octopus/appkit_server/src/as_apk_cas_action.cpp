#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_cas_action.h>

#include <oapi_cas_action.h>

char const * const cApkCasAction::BUSNAME_APK_CAS_ACTION = "Octopus.Appkit.Cas.Action";
char const * const cApkCasAction::BUSPATH_APK_CAS_ACTION = "/Octopus/Appkit/Cas/Action";

cApkCasAction * cApkCasAction::pMainThis	=	NULL;

void cApkCasAction::callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t param1=0, param2=0, param3=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;
	param3 = (uint32_t)ulParam3;

	pMainThis->event (param1, param2, param3);					
}

cApkCasAction::cApkCasAction(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_CAS_ACTION)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit cas action */
    {	
		HERROR hErr = ERR_FAIL;
		
		hErr = APK_CAS_ACTION_Init();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_CAS_ACTION_Init Error : %d", hErr);
		}

		APK_CAS_ACTION_RegisterNotifier (cApkCasAction::callback_Event);
		//void APK_CAS_ACTION_UnRegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotifer);
	}
}

uint32_t cApkCasAction::GetActionType()
{
	HERROR hErr = ERR_FAIL;
	uint32_t	reval;

	HUINT8 ucActionType	=	0;
		
	hErr = APK_CAS_ACTION_GetActionType (&ucActionType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_ACTION_GetActionType Error : %d", hErr);
	}

	{
		reval	=	(uint32_t)ucActionType;
	}

	return reval;
}

uint32_t cApkCasAction::GetSlotId()
{
	HERROR hErr = ERR_FAIL;
	uint32_t	reval;

	HUINT8 ucSlotId	=	0;

	hErr = APK_CAS_ACTION_GetSlotId (&ucSlotId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_ACTION_GetSlotId Error : %d", hErr);
	}
	
	{
		reval	=	(uint32_t)ucSlotId;
	}

	return reval;
}

uint32_t cApkCasAction::GetCasId()
{
	HERROR hErr = ERR_FAIL;
	uint32_t	reval;

	HUINT8 ucCasId	=	0;

	hErr = APK_CAS_ACTION_GetCasId (&ucCasId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_ACTION_GetCasId Error : %d", hErr);
	}

	{
		reval	=	(uint32_t)ucCasId;
	}

	return reval;
}

std::vector< uint8_t > cApkCasAction::GetData()
{
	HERROR hErr = ERR_FAIL;
	std::vector< uint8_t >	reval;

	HUINT8 aucData[APKS_CAS_SUB_DATA_LENG];

	hErr = APK_CAS_ACTION_GetData(aucData);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_ACTION_GetData Error : %d", hErr);
	}

	{
		HINT32 i;
		for (i=0 ;i<APKS_CAS_SUB_DATA_LENG ; i++)
			reval.push_back(aucData[i]);
	}

	return reval;
}
