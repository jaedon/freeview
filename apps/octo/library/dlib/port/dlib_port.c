
#include "../_dlib.h"
#include "_dlib_port.h"

#include "antinfo/_dlib_port_antinfo.h"
#include "bouquet/_dlib_port_bouquet.h"
#include "network/_dlib_port_network.h"
#include "provider/_dlib_port_provider.h"
#include "tuneparam/_dlib_port_tuneparam.h"
#include "group/_dlib_port_group.h"
#include "transponder/_dlib_port_transponder.h"
#include "service/_dlib_port_service.h"
#include "epg_rf/_dlib_port_rf_epg.h"
#include "epg_humax/_dlib_port_humax_epg.h"
#include "epg_humax/_dlib_port_humax_svc.h"
#include "schedule/_dlib_port_schedule.h"
#include "recording/_dlib_port_recording.h"
#include "dbsetup/_dlib_port_dbsetup.h"
#include "channel_logo/_dlib_port_channel_logo.h"
#include "fileinfo/_dlib_port_fileinfo.h"
#include "grouplist/_dlib_port_grouplist.h"
#include "udbmodule/_dlib_port_udbmodule.h"
#include "epg_tvtv/_dlib_port_tvtv_epg.h"
#include "epg_tvtv/_dlib_port_tvtv_svc.h"
#if defined(CONFIG_OP_UK_DTT)
#include "fvc_expiry/_dlib_port_fvc_expiry.h"
#endif

static HUINT32 dlib_port_DefaultSyncTime(void)
{
	return 10;	// 10 sec
}

/**
 *
 * @param	obj		object class
 * @param	name	object name
 * @param	tags	object tags (separated with ';', ex "tag1;tag2;...;tagN")
 *						"DaMa" tag를 넣으면, DaMa에서 자동으로 loading 해 감
 * @return	initialized object class
 */
static DxObject_t *	dlib_port_InitObject (DxObject_t *obj, const HCHAR *name, const HCHAR *tags)
{
	memset(obj, 0, sizeof(DxObject_t));
	HLIB_STD_StrNCpySafe(obj->name, name, DxLIB_MAX_NAME);
	if (tags)
		HLIB_STD_StrNCpySafe(obj->tags, tags, DxLIB_MAX_NAME);
	return obj;
}

HERROR	DxLIB_PORT_Init (void)
{
	DxObject_t	object;
	DxObject_t	*o;

	HxLOG_Trace();

	o = dlib_port_InitObject(&object, DxNAMEOF(DxAntInfo_t), "DaMa;antenna");
	if (o)
	{
		o->init = DLIB_ANTINFO_Init;
		o->getSyncTime = dlib_port_DefaultSyncTime;
		o->getFieldCount = DLIB_ANTINFO_GetFieldCount;
		o->getField = DLIB_ANTINFO_GetField;
		o->encode = DLIB_ANTINFO_Encode;
		o->decode = DLIB_ANTINFO_Decode;
		o->print = DLIB_ANTINFO_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxBouquet_t), "DaMa;SVC;RF");
	if (o)
	{
		o->init = DLIB_BOUQUET_Init;
		o->getFieldCount = DLIB_BOUQUET_GetFieldCount;
		o->getField = DLIB_BOUQUET_GetField;
		o->encode = DLIB_BOUQUET_Encode;
		o->decode = DLIB_BOUQUET_Decode;
		o->makeType = DLIB_BOUQUET_MakeType;
		o->freeType = DLIB_BOUQUET_FreeType;
		o->print = (void (*) (FILE *, const void *))DLIB_BOUQUET_Print;
		dlib_RegisterObject(o);
	}
	o = dlib_port_InitObject(&object, DxNAMEOF(DxNetwork_t), "DaMa;SVC;RF");
	if (o)
	{
		o->init = DLIB_NETWORK_Init;
		o->getFieldCount = DLIB_NETWORK_GetFieldCount;
		o->getField = DLIB_NETWORK_GetField;
		o->encode = DLIB_NETWORK_Encode;
		o->decode = DLIB_NETWORK_Decode;
		o->makeType = DLIB_NETWORK_MakeType;
		o->freeType = DLIB_NETWORK_FreeType;
		o->print = (void (*) (FILE *, const void *))DLIB_NETWORK_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxProvider_t), "DaMa;SVC;RF");
	if (o)
	{
		o->getFieldCount = DLIB_PROVIDER_GetFieldCount;
		o->getField = DLIB_PROVIDER_GetField;
		o->encode = DLIB_PROVIDER_Encode;
		o->decode = DLIB_PROVIDER_Decode;
		o->makeType = DLIB_PROVIDER_MakeType;
		o->freeType = DLIB_PROVIDER_FreeType;
		o->print = (void (*) (FILE *, const void *))DLIB_PROVIDER_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxTuneParam_t), "SVC;RF");
	if (o)
	{
		o->init = DLIB_TUNEPARAM_Init;
		o->encode = DLIB_TUNEPARAM_Encode;
		o->decode = DLIB_TUNEPARAM_Decode;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxGroup_t), "DaMa;SVC;RF");
	if (o)
	{
		o->init = DLIB_GROUP_Init;
		o->getFieldCount = DLIB_GROUP_GetFieldCount;
		o->getField = DLIB_GROUP_GetField;
		o->encode = DLIB_GROUP_Encode;
		o->decode = DLIB_GROUP_Decode;
		o->makeType = DLIB_GROUP_MakeType;
		o->freeType = DLIB_GROUP_FreeType;
		o->print = (void (*) (FILE *, const void *))DLIB_GROUP_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxTransponder_t), "DaMa;SVC;RF");
	if (o)
	{
		o->init = DLIB_TRANSPONDER_Init;
		o->getFieldCount = DLIB_TRANSPONDER_GetFieldCount;
		o->getField = DLIB_TRANSPONDER_GetField;
		o->encode = DLIB_TRANSPONDER_Encode;
		o->decode = DLIB_TRANSPONDER_Decode;
		o->makeType = DLIB_TRANSPONDER_MakeType;
		o->freeType = DLIB_TRANSPONDER_FreeType;
		o->print = (void (*) (FILE *, const void *))DLIB_TRANSPONDER_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxService_t), "DaMa;SVC;RF;CACHE;ASYNC");
	if (o)
	{
		o->init = DLIB_SERVICE_Init;
		o->getFieldCount = DLIB_SERVICE_GetFieldCount;
		o->getField = DLIB_SERVICE_GetField;
		o->encode = DLIB_SERVICE_Encode;
		o->decode = DLIB_SERVICE_Decode;
		o->makeType = DLIB_SERVICE_MakeType;
		o->freeType = DLIB_SERVICE_FreeType;
		o->print = (void (*) (FILE *, const void *))DLIB_SERVICE_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(eDxEPG_TYPE_PF), "DaMa;EPG;RF");
	if (o)
	{
		o->init = DLIB_EPG_RF_Init;
		o->getPushCycle = DLIB_EPG_PF_GetPushCycle;
		o->getThreshold = DLIB_EPG_PF_GetThreshold;
		o->getDBFullName = DLIB_EPG_PF_GetDBFullName;
		o->getFieldCount = DLIB_EPG_RF_GetFieldCount;
		o->getField = DLIB_EPG_RF_GetField;
		o->makeType = DLIB_EPG_RF_MakeType;
		o->freeType = DLIB_EPG_RF_FreeType;
		o->print = DLIB_EPG_RF_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(eDxEPG_TYPE_SC), "DaMa;EPG;RF");
	if (o)
	{
		o->init = DLIB_EPG_RF_Init;
		o->getThreshold = DLIB_EPG_SC_GetThreshold;
		o->getDBFullName = DLIB_EPG_SC_GetDBFullName;
		o->getFieldCount = DLIB_EPG_RF_GetFieldCount;
		o->getField = DLIB_EPG_RF_GetField;
		o->makeType = DLIB_EPG_RF_MakeType;
		o->freeType = DLIB_EPG_RF_FreeType;
		o->print = DLIB_EPG_RF_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(eDxEPG_TYPE_IP), "DaMa;EPG;IP;ASYNC;SLOWINIT");
	if (o)
	{
		o->init = DLIB_EPG_HUMAX_Init;
		o->getDBFullName = DLIB_EPG_HUMAX_GetDBFullName;
		o->getFieldCount = DLIB_EPG_HUMAX_GetFieldCount;
		o->getField = DLIB_EPG_HUMAX_GetField;
		o->getDataPath = DLIB_EPG_HUMAX_GetRepositoryPath;
		o->getThreshold = DLIB_EPG_HUMAX_GetThreshold;
		o->getThresholdSize = DLIB_EPG_HUMAX_GetThresholdSize;
		o->encode = DLIB_EPG_HUMAX_Encode;
		o->decode = DLIB_EPG_HUMAX_Decode;
		o->makeType = DLIB_EPG_HUMAX_MakeType;
		o->freeType = DLIB_EPG_HUMAX_FreeType;
		o->readData = DLIB_EPG_HUMAX_ReadData;
		o->writeData = DLIB_EPG_HUMAX_WriteData;
		o->getLinkName = DLIB_EPG_HUMAX_LinkName;
		o->print = DLIB_EPG_HUMAX_Print_Event;
		o->syncData = DLIB_EPG_HUMAX_SyncData;
		o->onStorageAdded = DLIB_EPG_HUMAX_OnStorageAdded;
		o->onStorageRemoved = DLIB_EPG_HUMAX_OnStorageRemoved;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxIpService_t), "DaMa;SVC;IP");
	if (o)
	{
		o->getDBFullName = DLIB_SVC_HUMAX_GetDBFullName;
		o->getSyncTime = DLIB_SVC_HUMAX_GetSyncTime;
		o->getFieldCount = DLIB_SVC_HUMAX_GetFieldCount;
		o->getField = DLIB_SVC_HUMAX_GetField;
		o->makeType = DLIB_SVC_HUMAX_MakeType;
		o->freeType = DLIB_SVC_HUMAX_FreeType;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(eDxEPG_TYPE_TVTV), "DaMa;EPG;TVTV;ASYNC;SLOWINIT");
	if (o)
	{
		o->init = DLIB_EPG_TVTV_Init;
		o->getDBFullName = DLIB_EPG_TVTV_GetDBFullName;
		o->getFieldCount = DLIB_EPG_TVTV_GetFieldCount;
		o->getField = DLIB_EPG_TVTV_GetField;
		o->getDataPath = DLIB_EPG_TVTV_GetRepositoryPath;
		o->encode = DLIB_EPG_TVTV_Encode;
		o->decode = DLIB_EPG_TVTV_Decode;
		o->makeType = DLIB_EPG_TVTV_MakeType;
		o->freeType = DLIB_EPG_TVTV_FreeType;
		o->readData = DLIB_EPG_TVTV_ReadData;
		o->writeData = DLIB_EPG_TVTV_WriteData;
		o->getLinkName = DLIB_EPG_TVTV_LinkName;
		o->print = DLIB_EPG_TVTV_Print_Event;
		o->syncData = DLIB_EPG_TVTV_SyncData;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxTvtvService_t), "DaMa;SVC;TVTV");
	if (o)
	{
		o->getDBFullName = DLIB_SVC_TVTV_GetDBFullName;
		o->getSyncTime = DLIB_SVC_TVTV_GetSyncTime;
		o->getFieldCount = DLIB_SVC_TVTV_GetFieldCount;
		o->getField = DLIB_SVC_TVTV_GetField;
		o->makeType = DLIB_SVC_TVTV_MakeType;
		o->freeType = DLIB_SVC_TVTV_FreeType;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxSchedule_t), "DaMa");
	if (o)
	{
		o->init = DLIB_SCHEDULE_Init;
		o->getSyncTime = dlib_port_DefaultSyncTime;
		o->getFieldCount = DLIB_SCHEDULE_GetFieldCount;
		o->getField = DLIB_SCHEDULE_GetField;
		o->encode = DLIB_SCHEDULE_Encode;
		o->decode = DLIB_SCHEDULE_Decode;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxDBSETUP_SYSTEM, "Obama");
	if (o)
	{
		o->init = DLIB_DBSETUP_Init;
		o->getSyncTime = dlib_port_DefaultSyncTime;
		o->getFieldCount = DLIB_DBSETUP_GetFieldCount;
		o->getField = DLIB_DBSETUP_GetField;
		o->encode = DLIB_DBSETUP_Encode;
		o->decode = DLIB_DBSETUP_Decode;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxRecListData_t), "DaMa;BLOB");
	if (o)
	{
		o->init = DLIB_RECORDING_Init;
		o->getDBFullName = DLIB_RECORDING_GetDBFullName;
		o->getSyncTime = dlib_port_DefaultSyncTime;
		o->getFieldCount = DLIB_RECORDING_GetFieldCount;
		o->getField = DLIB_RECORDING_GetField;
		o->encode = DLIB_RECORDING_Encode;	///< DxRecListData는 BLOB 태그가 있어서, db에 저장할 때
		o->decode = DLIB_RECORDING_Decode;	///< codec을 사용하지 않는다.
		o->cloneType = DLIB_RECORDING_CloneType;
		o->makeType = DLIB_RECORDING_MakeType;
		o->freeType = DLIB_RECORDING_FreeType;
		o->print    = (void (*) (FILE *, const void *))DLIB_RECORDING_Print;
		dlib_RegisterObject(o);
	}
	o = dlib_port_InitObject(&object, DxNAMEOF(DxChannelLogo_t), "DaMa");
	if (o)
	{
		o->getSyncTime = dlib_port_DefaultSyncTime;
		o->getDBFullName = DLIB_CHANNEL_LOGO_GetDBFullName;
		o->getFieldCount = DLIB_CHANNEL_LOGO_GetFieldCount;
		o->getField = DLIB_CHANNEL_LOGO_GetField;
		o->encode = DLIB_CHANNEL_LOGO_Encode;
		o->decode = DLIB_CHANNEL_LOGO_Decode;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object,"UDBMODULE", "DaMa");
	if (o)
	{
		o->getSyncTime = dlib_port_DefaultSyncTime;
		o->init = DLIB_UDBMODULE_Init;
		o->encode = DLIB_UDBMODULE_Encode;
		o->decode = DLIB_UDBMODULE_Decode;

		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxFileInfo_t), "DaMa");
	if (o)
	{
		o->getFieldCount = DLIB_FILEINFO_GetFieldCount;
		o->getDBFullName = DLIB_FILEINFO_GetDBFullName;
		o->getField      = DLIB_FILEINFO_GetField;
		o->encode		 = DLIB_FILEINFO_Encode;
		o->decode		 = DLIB_FILEINFO_Decode;
		o->serialize	 = DLIB_FILEINFO_Serialize;
		o->deserialize	 = DLIB_FILEINFO_Deserialize;
		o->print         = DLIB_FILEINFO_Print;
		dlib_RegisterObject(o);
	}

	o = dlib_port_InitObject(&object, DxNAMEOF(DxGroupList_t), "DaMa");
	if (o)
	{
		o->getFieldCount	= DLIB_GROUPLIST_GetFieldCount;
		o->getField			= DLIB_GROUPLIST_GetField;
		o->getSyncTime		= dlib_port_DefaultSyncTime;

		dlib_RegisterObject(o);
	}
#if defined (CONFIG_MW_SATRECORD)
	o = dlib_port_InitObject(&object, "DxDB_SatRecord", "DaMa");
	if (o)
	{
		o->init = DLIB_SCHEDULE_Init;
		o->getSyncTime = dlib_port_DefaultSyncTime;
		o->getFieldCount = DLIB_SCHEDULE_GetFieldCount;
		o->getField = DLIB_SCHEDULE_GetField;
		o->encode = DLIB_SCHEDULE_Encode;
		o->decode = DLIB_SCHEDULE_Decode;
		dlib_RegisterObject(o);
	}
#endif

#if defined (CONFIG_OP_UK_DTT)
	o = dlib_port_InitObject(&object, DxNAMEOF(DxFvcExpiry_t), "DaMa");
	if (o)
	{
		o->init = DLIB_FVC_EXPIRY_Init;
		o->getSyncTime = dlib_port_DefaultSyncTime;
		o->getDBFullName = DLIB_FVC_EXPIRY_GetDBFullName;
		o->makeType = DLIB_FVC_EXPIRY_MakeType;
		o->getFieldCount = DLIB_FVC_EXPIRY_GetFieldCount;
		o->getField = DLIB_FVC_EXPIRY_GetField;
		o->encode = DLIB_FVC_EXPIRY_Encode;
		o->decode = DLIB_FVC_EXPIRY_Decode;
		dlib_RegisterObject(o);
	}
#endif

	return ERR_OK;
}

