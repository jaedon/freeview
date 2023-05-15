#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_filemgr_group.h>

char const * const cApkFilemgrGroup::BUSNAME_APK_FILEMGR_GROUP = "Octopus.Appkit.Filemgr.Group";
char const * const cApkFilemgrGroup::BUSPATH_APK_FILEMGR_GROUP = "/Octopus/Appkit/Filemgr/Group";


cApkFilemgrGroup::cApkFilemgrGroup(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_FILEMGR_GROUP)
{
	/* apkit filemgr group */
    {	

    }
}

uint32_t cApkFilemgrGroup::Create(const std::string& extFilter)
{
	HUINT32 ulReval;
	ulReval = APK_FILEMGR_GROUP_Create ((HCHAR *)&extFilter[0]);

	return ulReval;
}

void cApkFilemgrGroup::Release(const uint32_t& handle)
{
	HERROR	hErr = ERR_FAIL;
	hErr = APK_FILEMGR_GROUP_Release ((HUINT32)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GROUP_Release Error : %d", hErr);
	}		
	
	return ;
}

void cApkFilemgrGroup::AddPath(const uint32_t& handle, const std::string& path)
{
	HERROR	hErr = ERR_FAIL;
	hErr = APK_FILEMGR_GROUP_AddPath ((HUINT32)handle, (const HCHAR *)&path[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GROUP_AddPath Error : %d", hErr);
	}

	return ;
}

void cApkFilemgrGroup::RemovePath(const uint32_t& handle, const std::string& path)
{
	HERROR	hErr = ERR_FAIL;
	hErr = APK_FILEMGR_GROUP_RemovePath ((HUINT32)handle, (const HCHAR *)&path[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GROUP_RemovePath Error : %d", hErr);
	}

	return ;
}

void cApkFilemgrGroup::ExcludePath(const uint32_t& handle, const std::string& path)
{
	HERROR	hErr = ERR_FAIL;
	hErr = APK_FILEMGR_GROUP_ExcludePath ((HUINT32)handle, (const HCHAR *)&path[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GROUP_ExcludePath Error : %d", hErr);
	}

	return ;
}

uint32_t cApkFilemgrGroup::Copy(const uint32_t& handle, const std::string& path)
{
	HERROR	hErr = ERR_FAIL;
	HINT32 nReqId = 0;
		
	hErr = APK_FILEMGR_GROUP_Copy ((HUINT32)handle, (const HCHAR *)&path[0], &nReqId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GROUP_Copy Error : %d", hErr);
	}

	return (uint32_t)nReqId;
}

uint32_t cApkFilemgrGroup::Move(const uint32_t& handle, const std::string& path)
{
	HERROR	hErr = ERR_FAIL;
	HINT32 nReqId = 0;

	hErr = APK_FILEMGR_GROUP_Move ((HUINT32)handle, (const HCHAR *)&path[0], &nReqId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GROUP_Move Error : %d", hErr);
	}

	return (uint32_t)nReqId;
}

uint32_t cApkFilemgrGroup::Remove(const uint32_t& handle)
{
	HERROR	hErr = ERR_FAIL;
	HINT32 nReqId = 0;

	hErr = APK_FILEMGR_GROUP_Remove ((HUINT32)handle, &nReqId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GROUP_Remove Error : %d", hErr);
	}

	return (uint32_t)nReqId;
}

uint32_t cApkFilemgrGroup::Cancel(const uint32_t& handle)
{
	HERROR	hErr = ERR_FAIL;
	HINT32 nReqId = 0;

	hErr = APK_FILEMGR_GROUP_Cancel ((HUINT32)handle, &nReqId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GROUP_Cancel Error : %d", hErr);
	}

	return (uint32_t)nReqId;
}
