#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_meta_antenna.h>

char const * const cApkMetaAntenna::BUSNAME_APK_META_ANTENNA = "Octopus.Appkit.Meta.Antenna";
char const * const cApkMetaAntenna::BUSPATH_APK_META_ANTENNA = "/Octopus/Appkit/Meta/Antenna";


cApkMetaAntenna::cApkMetaAntenna(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_META_ANTENNA)
{
	/* apkit meta antenna */
    {	
		(HBOOL)APK_META_ANTENNA_Init (FALSE);
		//HBOOL APK_META_ANTENNA_DeInit(void);
    }
}

int32_t cApkMetaAntenna::Add(const DbtAntInfo_t& antInfo)
{
	DxAntInfo_t stAntenna;
	cDbusUtilCvt::C2DbtAntInfo (antInfo, &stAntenna);

	return (int32_t)APK_META_ANTENNA_Add (&stAntenna);

}

void cApkMetaAntenna::RemoveAll()
{
	return APK_META_ANTENNA_RemoveAll ();
}

uint32_t cApkMetaAntenna::Count()
{
	return (uint32_t)APK_META_ANTENNA_Count ();
}

int32_t cApkMetaAntenna::GetUidAt(const int32_t& index)
{
	return (int32_t)APK_META_ANTENNA_GetUidAt ((HINT32)index);
}

DbtAntInfo_t cApkMetaAntenna::GetInfo(const int32_t& uid)
{
	DbtAntInfo_t reval;
	DxAntInfo_t * pstAntInfo	=	APK_META_ANTENNA_GetInfo ((HINT32)uid);

	if (NULL != pstAntInfo)
	{
		reval = cDbusUtilCvt::C2DxAntInfo (pstAntInfo);
		APK_META_ANTENNA_InfoFree (pstAntInfo);
	}

	return reval;
}

void cApkMetaAntenna::SetInfo(const int32_t& uid, const DbtAntInfo_t& antInfo)
{
	DxAntInfo_t stAntenna;
	cDbusUtilCvt::C2DbtAntInfo (antInfo, &stAntenna);

	return APK_META_ANTENNA_SetInfo ((HINT32)uid, &stAntenna);
}

void cApkMetaAntenna::Load()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_ANTENNA_Load ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_ANTENNA_Load Error : %d", hErr);
	}

	return ;
}

void cApkMetaAntenna::Save()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_ANTENNA_Save ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_ANTENNA_Save Error : %d", hErr);
	}

	return ;
}
