/**************************************************************************************/
/**
 * @file bplchannel.c
 *
 * Defines the channel structure.
 *
 **************************************************************************************
 **/

#include "oplchannel.h"
#include "oplfavourite.h"

#include <hlib.h>
#include "oapi.h"
#include <apk.h>
#include <thapi.h>

#ifndef USE_NEW_FAV

//#define	_DLOG					printf
#define	_DLOG					HxLOG_Debug
#define	ASSERT(expr)			HxLOG_Assert(expr)
#define	TRACE()					do{_DLOG("OPLFAV::TRACE [%s:%d]\n", __FUNCTION__, __LINE__);}while(0)
#define	RETURN_IF_VOID(expr)	do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	RETURN_IF(expr, err)	do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)
#define	OPL_MACRO_GET_JAPAN_AREA_ID(x)					( ((x) & 0xFC00) >> 10 )

#define		OPL_JAPAN_LOGO_ROOT_PATH		"/var/lib/humaxtv_backup/channel_logo/"


typedef struct
{
	HxVector_t	*list;	/// [ DxService_t * ]
	HxTREE_t	*map;	/// [ int uid, DxService_t * ]
	HINT32		lastVersion;
	OPL_ChannelListener_t	onEvent;
} ChannelManager_t;

#define	____OPL_CHANNEL_PRIVATE_____________________________________________________________________

static HBOOL	opl_channel_FilterByUID (const DxService_t *pstService, HINT32 nUid)
{
	if (pstService->uid == nUid)
		return TRUE;

	return FALSE;
}

static HBOOL	opl_channel_FilterByTriplet (const DxService_t *pstService, HINT32 *triplet)
{
	if (pstService->onid != *triplet++)
		return FALSE;
	if (pstService->tsid != *triplet++)
		return FALSE;
	if (pstService->svcid != *triplet++)
		return FALSE;

	return TRUE;
}

static HBOOL	opl_channel_FilterByNumber (const DxService_t *pstService, HINT32 lcn)
{
	if (pstService->lcn == lcn)
		return TRUE;

	return FALSE;
}

static HINT32		opl_channel_SortByUID (const void *pvArg1, const void *pvArg2)
{
	const DxService_t *pstService1 = *(const DxService_t **)pvArg1;
	const DxService_t *pstService2 = *(const DxService_t **)pvArg2;

	return (pstService1->uid - pstService2->uid);
}

static HINT32		opl_channel_SortByName (const void *pvArg1, const void *pvArg2)
{
	const DxService_t *pstService1 = *(const DxService_t **)pvArg1;
	const DxService_t *pstService2 = *(const DxService_t **)pvArg2;

	return (HxSTD_StrCmp(pstService1->name, pstService2->name));
}


static HINT32		opl_channel_SortByLCN (const void *pvArg1, const void *pvArg2)
{
	const DxService_t *pstService1 = *(const DxService_t **)pvArg1;
	const DxService_t *pstService2 = *(const DxService_t **)pvArg2;

	return (pstService1->lcn - pstService2->lcn);
}

static int	opl_channel_GetConstantValue (const char *value)
{
	static const struct constants
	{
		const char *name;
		int			value;
	} s_constants[] = {
		  {"TYPE_TV", eOPL_ChannelType_TV}
		, {"TYPE_RADIO", eOPL_ChannelType_RADIO}
		, {"TYPE_OTHER", eOPL_ChannelType_OTHER}
		, {"ID_ANALOG", eOPL_ChannelID_ANALOG}
		, {"ID_DVB_C", eOPL_ChannelID_DVB_C}
		, {"ID_DVB_S", eOPL_ChannelID_DVB_S}
		, {"ID_DVB_T", eOPL_ChannelID_DVB_T}
		, {"ID_DVB_SI_DIRECT", eOPL_ChannelID_DVB_SI_DIRECT}
		, {"ID_DVB_C2", eOPL_ChannelID_DVB_C2}
		, {"ID_DVB_S2", eOPL_ChannelID_DVB_S2}
		, {"ID_DVB_T2", eOPL_ChannelID_DVB_T2}
		, {"ID_ISDB_C", eOPL_ChannelID_ISDB_C}
		, {"ID_ISDB_S", eOPL_ChannelID_ISDB_S}
		, {"ID_ISDB_T", eOPL_ChannelID_ISDB_T}
		, {"ID_ATSC_T", eOPL_ChannelID_ATSC_T}
		, {"ID_IPTV_SDS", eOPL_ChannelID_IPTV_SDS}
		, {"ID_IPTV_URI", eOPL_ChannelID_IPTV_URI}
		, {"CAS_FTA", eOPL_ChannelCasID_FTA}
		, {"CAS_NAGRA", eOPL_ChannelCasID_NAGRA}
		, {"CAS_IRDETO", eOPL_ChannelCasID_IRDETO}
		, {"CAS_NDS", eOPL_ChannelCasID_NDS}
		, {"CAS_VIACCESS", eOPL_ChannelCasID_VIACCESS}
		, {"CAS_CONAX", eOPL_ChannelCasID_CONAX}
		, {"CAS_MEDIAGUARD", eOPL_ChannelCasID_MEDIAGUARD}
		, {"CAS_CRYPTOWORKS", eOPL_ChannelCasID_CRYPTOWORKS}
		, {"CAS_JPBCAS", eOPL_ChannelCasID_JPBCAS}
		, {"CAS_JPCCAS", eOPL_ChannelCasID_JPCCAS}
		, {"CAS_ALLCAS", eOPL_ChannelCasID_ALLCAS}
	};
	int i, n;

	n = sizeof(s_constants) / sizeof(s_constants[0]);
	for (i = 0 ; i < n ; i++)
	{
		if (strcmp(s_constants[i].name, value) == 0)
			return s_constants[i].value;
	}
	return -1;
}

static int	opl_channel_GetCasIDs (DxService_t *service, OPL_ChannelCasID_e *id)
{
	int		n;

	HxLOG_Trace();
	HxLOG_Assert(service);
	n = 0;
	if (service->casType & eDxCAS_TYPE_NAGRA)
		id[n++] = eOPL_ChannelCasID_NAGRA;
	if (service->casType & eDxCAS_TYPE_IRDETO)
		id[n++] = eOPL_ChannelCasID_IRDETO;
	if (service->casType & eDxCAS_TYPE_NDS)
		id[n++] = eOPL_ChannelCasID_NDS;
	if (service->casType & eDxCAS_TYPE_VIACCESS)
		id[n++] = eOPL_ChannelCasID_VIACCESS;
	if (service->casType & eDxCAS_TYPE_CONAX)
		id[n++] = eOPL_ChannelCasID_CONAX;
	if (service->casType & eDxCAS_TYPE_MEDIAGUARD)
		id[n++] = eOPL_ChannelCasID_MEDIAGUARD;
	if (service->casType & eDxCAS_TYPE_CRYPTOWORKS)
		id[n++] = eOPL_ChannelCasID_CRYPTOWORKS;
	if (service->casType & eDxCAS_TYPE_JPBCAS)
		id[n++] = eOPL_ChannelCasID_JPBCAS;
	if (service->casType & eDxCAS_TYPE_ALLCAS)
		id[n++] = eOPL_ChannelCasID_ALLCAS;
	if (service->casType & eDxCAS_TYPE_VERIMATRIX)
		id[n++] = eOPL_ChannelCasID_VERIMATRIX;
	return n;
}

static HCHAR *opl_channel_GetStrCodeRate(DxSat_CodeRate_e eCodeRate)
{
	switch(eCodeRate)
	{
		case eDxSAT_CODERATE_AUTO: return "AUTO"; break;
		case eDxSAT_CODERATE_NONE: return "NONE"; break;
		case eDxSAT_CODERATE_1_2: return "1/2"; break;
		case eDxSAT_CODERATE_2_3: return "2/3"; break;
		case eDxSAT_CODERATE_3_4: return "3/4"; break;
		case eDxSAT_CODERATE_5_6: return "5/6"; break;
		case eDxSAT_CODERATE_7_8: return "7/8"; break;
		case eDxSAT_CODERATE_3_5: return "3/5"; break;
		case eDxSAT_CODERATE_4_5: return "4/5"; break;
		case eDxSAT_CODERATE_5_11: return "5/11"; break;
		case eDxSAT_CODERATE_6_7: return "6/7"; break;
		case eDxSAT_CODERATE_8_9: return "8/9"; break;
		case eDxSAT_CODERATE_9_10: return "9/10"; break;
		default: break;
	}

	return ("Unknown");
}

static HCHAR *opl_channel_GetStrPolar(DxSat_Polarization_e ePolar)
{
	switch(ePolar)
	{
		case eDxSAT_POLAR_HOR: return "H"; break;
		case eDxSAT_POLAR_VER: return "V"; break;
		case eDxSAT_POLAR_LEFT: return "L"; break;
		case eDxSAT_POLAR_RIGHT: return "R"; break;
		default: break;
	}

	return ("AUTO");
}

static HCHAR *opl_channel_GetStrDVBTSystem(DxTer_System_e eDvbTSystem)
{
	switch(eDvbTSystem)
	{
		case eDxTER_DVB_T: return "DVB-T"; break;
		case eDxTER_DVB_T2: return "DVB-T2"; break;

		default: break;
	}
	return ("AUTO");
}

static HCHAR *opl_channel_GetStrTBandWidth(DxTer_BandWidth_e eBandWidth)
{
	switch(eBandWidth)
	{
		case eDxTER_BANDWIDTH_8M: return "8M"; break;
		case eDxTER_BANDWIDTH_7M: return "7M"; break;
		case eDxTER_BANDWIDTH_6M: return "6M"; break;
		case eDxTER_T2BANDWIDTH_5MHZ: return "T2_5M"; break;
		case eDxTER_T2BANDWIDTH_10MHZ: return "T2_10M"; break;
		case eDxTER_T2BANDWIDTH_1P7MHZ: return "T2_1P7M"; break;

		default: break;
	}
	return ("AUTO");
}

static HCHAR *opl_channel_GetStrTTransmode(DxTer_TransMode_e eTransMode)
{
	switch(eTransMode)
	{
		case eDxTER_TRANSMODE_AUTO: return "AUTO"; break;
		case eDxTER_TRANSMODE_2K: return "2K"; break;
		case eDxTER_TRANSMODE_8K: return "8K"; break;
		case eDxTER_T2TRANSMODE_1K: return "T2_1K"; break;
		case eDxTER_T2TRANSMODE_4K: return "T2_4K"; break;
		case eDxTER_T2TRANSMODE_16K: return "T2_16K"; break;
		case eDxTER_T2TRANSMODE_32K: return "T2_32K"; break;

		default: break;
	}
	return ("AUTO");
}

static HCHAR *opl_channel_GetStrTGuardInterval(DxTer_GuardInterval_e eGuardInterval)
{
	switch(eGuardInterval)
	{
		case eDxTER_GUARDINTERVAL_AUTO: return "AUTO"; break;
		case eDxTER_GUARDINTERVAL_1_4: return "1/4"; break;
		case eDxTER_GUARDINTERVAL_1_8: return "1/8"; break;
		case eDxTER_GUARDINTERVAL_1_16: return "1/16"; break;
		case eDxTER_GUARDINTERVAL_1_32: return "1/32"; break;
		case eDxTER_T2GUARDINTERVAL_1_128: return "T2_1/128"; break;
		case eDxTER_T2GUARDINTERVAL_19_128: return "T2_19/128"; break;
		case eDxTER_T2GUARDINTERVAL_19_256: return "T2_19/256"; break;

		default: break;
	}
	return ("AUTO");
}

static HINT32
	opl_channel_CompareHINT32 (const void *arg1, const void *arg2)
{
	return (HINT32)arg1 - (HINT32)arg2;
}


static ChannelManager_t *
	opl_channel_LoadList (ChannelManager_t *manager, HBOOL force)
{
	HxVector_t		*list;
	HxTREE_t		*uidMap;
	DxService_t	*service;

	if (manager->list && !force)
		return manager;

	HxLOG_Trace();

	list = APK_META_SVC_GetServiceList();
	if (list == NULL)
	{
		HxLOG_Debug("Cannot get channel list from appkit\n");
#ifdef CONFIG_OP_JAPAN
		if(manager->list)
		{
			HLIB_VECTOR_RemoveAll(manager->list);
		}
		if(manager->map)
		{
			HLIB_TREE_RemoveAll(manager->map);
		}
#endif
		return manager;
	}
	uidMap = HLIB_TREE_New(opl_channel_CompareHINT32);
	if (uidMap == NULL)
	{
		HLIB_VECTOR_Delete(list);
		HxLOG_Error("Out of memory!!!\n");
		return NULL;
	}
	HxVECTOR_EACH(list, DxService_t *, service, {
		HLIB_TREE_Insert(uidMap, (void *)service->uid, (void *)service);
	});

	if (manager->list)
		HLIB_VECTOR_Delete(manager->list);
	if (manager->map)
		HLIB_TREE_Destroy(manager->map);

	(void) HLIB_VECTOR_Sort(list, opl_channel_SortByLCN);

	manager->list = list;
	manager->map  = uidMap;

#if DONOTSAVE
	OPL_Favourite_Update();
#endif

	return manager;
}

static ChannelManager_t *
	opl_channel_GetManager (OPL_ChannelManager_t managerHandle)
{
	ChannelManager_t	*manager = (ChannelManager_t *)managerHandle;

	if (manager == NULL)
	{
		static ChannelManager_t	s_manager;

		manager = &s_manager;
	}
	return opl_channel_LoadList(manager, FALSE/*NOT forced*/);
}

static OPL_Collection_t *
	opl_channel_GetChannelListByVersion (ChannelManager_t	*instance, HINT32 version)
{
	DxService_t		*service;
	void 				*clone;
	OPL_Collection_t	*list;

	HxLOG_Trace();

	RETURN_IF(instance->list == NULL, NULL);

	list = OPL_Collection_NewEx(APK_META_SVC_Delete, NULL);
	RETURN_IF(list == NULL, NULL);

	HxVECTOR_EACH(instance->list, DxService_t *, service, {
		if (service && service->version == version)
		{
			clone = APK_META_SVC_Clone(service);
			if (clone)
				OPL_Collection_Add(list, clone);
		}
	});
	if (OPL_Collection_Length(list) == 0)
	{
		OPL_Collection_Delete(list);
		return NULL;
	}
	return list;
}


static void
	opl_channel_EventListener (HINT32 event, const HINT32 *args, void *userdata)
{
	ChannelManager_t    *manager;

	manager = (ChannelManager_t *)userdata;

	switch(event)
	{
	case 0:	// dynamic update!! (async)
	case 1: // loading (from octo, sync)
		opl_channel_LoadList(manager, TRUE);
		if (args)
		{
			manager->lastVersion = args[0];
		}


		#if 0
		printf("[%s][%d] ########### OPL_Favourite_Update START ###########\n",__func__,__LINE__);
		// update favourite list
		#if defined(CONFIG_PROD_HMS1000S)
		OPL_Favourite_Update();
		#endif
		printf("[%s][%d] ########### OPL_Favourite_Update END ##########\n",__func__,__LINE__);
		#endif

		if (manager->onEvent)
		{
			OPL_Collection_t *updated = opl_channel_GetChannelListByVersion(manager, manager->lastVersion);

			manager->onEvent((OPL_ChannelManager_t)manager, eOPL_CHANNEL_STATE_UPDATED, updated);

			if (updated)
				OPL_Collection_Delete(updated);
		}
		break;

	default:
		break;
	}
}

#if defined(CONFIG_OP_FREESAT)
static HBOOL
	opl_channel_FreesatUpdateChecker (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated)
{
#if 0	// TODO: remove for HDR-1000S Phase 2 - mhoh
	DxFreesatGroup_t *extOld, *extNew;

	if(NULL == updated)
		return FALSE;

	*updated = FALSE;

	//2 Updated
	if (_old && _new) // memcmp(_old, _new) != 0
	{
		if (strcmp(DxNAMEOF(DxGroup_t), typeName) == 0)
		{
			const DxGroup_t *oldGroup = (const DxGroup_t *)_old;
			const DxGroup_t *newGroup = (const DxGroup_t *)_new;

			if (strcmp(oldGroup->name, newGroup->name) != 0)
			{
				HxLOG_Print(">>>>> [opl_channel_FreesatUpdateChecker] group Update 1 \n");
				*updated = TRUE;
				return TRUE;	// HANDLED
			}

			extOld = &oldGroup->ex.freesat;
			extNew = &newGroup->ex.freesat;

			if(NULL == extOld || NULL == extNew)
			{
				return FALSE;
			}

			if(strcmp(extOld->szIso639LangCode, extNew->szIso639LangCode) != 0 ||
				extOld->ucGroupType != extNew->ucGroupType ||
				extOld->ucNondestructiveTuneFlag != extNew->ucNondestructiveTuneFlag ||
				extOld->ucReturnChannelAccessFlag != extNew->ucReturnChannelAccessFlag ||
				extOld->ucG2ExtensionFlag != extNew->ucG2ExtensionFlag ||
				extOld->ucDefaultVisableFlag != extNew->ucDefaultVisableFlag ||
				extOld->ucAdultChannelsGroupFlag != extNew->ucAdultChannelsGroupFlag ||
				extOld->ucIpChannelsGroupFlag != extNew->ucIpChannelsGroupFlag ||
				extOld->ucGroupType != extNew->ucGroupType ||
				extOld->ucCaChannelsGroupFlag != extNew->ucCaChannelsGroupFlag)
			{
				HxLOG_Print(">>>>> [opl_channel_FreesatUpdateChecker] group Update 2 \n");
				*updated = TRUE;
				return TRUE;	// HANDLED
			}
		}
	}
	//2 Removed
	else if (_old)
	{
	}
	//2 Added
	else if (_new)
	{
	}
	else
	{
	}

#endif
	return FALSE;	// NOT HANDLED
}
#elif defined(CONFIG_OP_JAPAN)
#define	OPL_CHANNEL_USERFLAG_UPDATE_CHECK(x, y)	((x &(eDxSvcUserFlag_24_TempSvc|eDxSvcUserFlag_22_SkipSvcByUser)) != (y & (eDxSvcUserFlag_24_TempSvc|eDxSvcUserFlag_22_SkipSvcByUser)))

static HBOOL	opl_channel_JapanUpdateChecker (const HCHAR *typeName,
																const void *_old,
																const void *_new,
																HBOOL *updated)
{
	*updated = FALSE;

	// TODO: transponder, network, provider도 필요한가.

	if (_old && _new)	// updated
	{
		if (strcmp(typeName, DxNAMEOF(DxService_t)) == 0)
		{
			const DxService_t *oldTable = (DxService_t*)_old;
			const DxService_t *newTable = (DxService_t*)_new;
			DxJapanService_t jpOldTable;
			DxJapanService_t jpNewTable;

			memcpy(&jpOldTable, &oldTable->ex.japan, sizeof(DxJapanService_t));
			memcpy(&jpNewTable, &newTable->ex.japan, sizeof(DxJapanService_t));

			if((oldTable->uid != newTable->uid)
				||(oldTable->tsuid != newTable->tsuid)
				||(oldTable->svcid != newTable->svcid)
				||(oldTable->tsid != newTable->tsid)
				||(oldTable->onid != newTable->onid)
				||(oldTable->lcn != newTable->lcn)
				||(strncmp(oldTable->name, newTable->name, DxNAME_LEN) != 0)
				||(oldTable->locked != newTable->locked)
				|| (jpOldTable.nBranchNum != jpNewTable.nBranchNum)
				|| (jpOldTable.nOnetouchNum != jpNewTable.nOnetouchNum)
				|| (jpOldTable.usLogoId != jpNewTable.usLogoId)
				|| (jpOldTable.ucFreeCAMode != jpNewTable.ucFreeCAMode)
				|| (jpOldTable.ucIsExistingSdtCAD != jpNewTable.ucIsExistingSdtCAD)
				|| (jpOldTable.usSubscriptionRes != jpNewTable.usSubscriptionRes)
				|| OPL_CHANNEL_USERFLAG_UPDATE_CHECK(jpOldTable.eUserFlag, jpNewTable.eUserFlag)
				)
			{
				*updated = TRUE;
			}
		}
	}
	else if (_old)		// removed
	{
		if (strcmp(typeName, DxNAMEOF(DxService_t)) == 0)
			*updated = TRUE;
	}
	else if (_new)		// added
	{
		if (strcmp(typeName, DxNAMEOF(DxService_t)) == 0)
			*updated = TRUE;
	}
	else
	{
		HxLOG_Critical("[%s] WTF!!!!!!\n", __FUNCTION__);
	}

	return TRUE;
}
#endif

#if defined(CONFIG_OP_JAPAN)
OPL_ResultCreateMaskListener_t onResultCreateMask;
static void 	opl_channel_ResultCreateMaskListener (HUINT32 event, HUINT32 arg1, HUINT32 arg2);
OPL_AribLogoType_e opl_channel_GetLogoType(int width, int height)
{
	int heightRange=0, widthRange=0;
	OPL_AribLogoType_e type[5][3] = {
		{eOPL_AribLogo_SD169_SMALL, eOPL_AribLogo_SD169_SMALL, eOPL_AribLogo_SD169_SMALL},
		{eOPL_AribLogo_SD43_SMALL, eOPL_AribLogo_HD_SMALL, eOPL_AribLogo_HD_SMALL},
		{eOPL_AribLogo_SD43_SMALL, eOPL_AribLogo_HD_SMALL, eOPL_AribLogo_SD169_LARGE},
		{eOPL_AribLogo_SD43_SMALL, eOPL_AribLogo_HD_SMALL, eOPL_AribLogo_HD_LARGE},
		{eOPL_AribLogo_SD43_SMALL, eOPL_AribLogo_HD_SMALL, eOPL_AribLogo_SD43_LARGE}};

	if((width < 36) ||  (height < 24))
		return eOPL_AribLogo_Unkown;

	if((36<=width) && (width < 48))
		widthRange = 0;
	else if((48<=width) && (width < 54))
		widthRange = 1;
	else if((54<=width) && (width < 64))
		widthRange = 2;
	else if((64<=width) && (width < 72))
		widthRange = 3;
	else
		widthRange = 4;

	if((24 <=height) && (height <27))
		heightRange = 0;
	else if((27 <=height) && (height < 36))
		heightRange = 1;
	else
		heightRange = 2;

	return type[widthRange][heightRange];

}


static void 	opl_channel_NotifyResultCreateMask(HUINT32 P1, HUINT32 P2)
{
	if (onResultCreateMask)
	{
		onResultCreateMask(P1);
		APK_SQCSCENARIOJAPAN_UnRegisterNotifier(opl_channel_ResultCreateMaskListener);
		onResultCreateMask = NULL;
	}
}

static void opl_channel_SyncedResultCreateMaskListener (int argc, void **argv)
{
	HxLOG_Assert(argc == 2);
	opl_channel_NotifyResultCreateMask((HUINT32)argv[0], (HUINT32)argv[1]);
}

static void 	opl_channel_ResultCreateMaskListener (HUINT32 event, HUINT32 arg1, HUINT32 arg2)
{
	if(event != eSysCtrl_Event_ChannelMaskDone)
	{
		return;
	}
	opl_channel_NotifyResultCreateMask(arg1, arg2);
}



#endif

static void	opl_channel_UpdateChannel (OPL_Channel_t channel)
{
	DxService_t			*newSvc;
	DxService_t			*oldSvc;
	ChannelManager_t	*manager;

	newSvc  = (DxService_t *)channel;

	manager = opl_channel_GetManager(NULL);
	if (manager == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return;
	}

	oldSvc = (DxService_t *)HLIB_TREE_Lookup(manager->map, (void *)newSvc->uid);
	if (oldSvc)
	{
		memcpy(oldSvc, newSvc, sizeof(DxService_t));
	}
	APK_META_SVC_SetService(newSvc);
}



#define	____OPL_CHANNEL_PUBLIC______________________________________________________________________

OPL_ChannelManager_t
		OPL_Channel_GetManager (void)
{
	(void)opl_channel_GetConstantValue;

	return (OPL_ChannelManager_t)opl_channel_GetManager(NULL);
}

void	OPL_Channel_AddListener (OPL_ChannelManager_t manager, OPL_ChannelListener_t listener)
{
	ChannelManager_t	*instance;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return;
	}

	if (listener)
	{
#if defined(CONFIG_OP_FREESAT)
		APK_META_SVC_SetUpdateChecker(opl_channel_FreesatUpdateChecker);
#elif defined(CONFIG_OP_JAPAN)
		APK_META_SVC_SetUpdateChecker(opl_channel_JapanUpdateChecker);
#endif
		APK_META_SVC_SetListener(opl_channel_EventListener, (void *)instance);
	}
	else
	{
		APK_META_SVC_SetUpdateChecker(NULL);
		APK_META_SVC_SetListener(NULL, NULL);
	}
	instance->onEvent = listener;
}

#if 0
// TODO: 삭제!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void
		OPL_Channel_ReloadAllList ()
{
	ChannelManager_t	*pManager;

	pManager = (ChannelManager_t*)OPL_Channel_GetManager();

	OPL_Channel_LoadList((OPL_ChannelManager_t)pManager);
}

#endif

OPL_ChannelManager_t
		OPL_Channel_LoadList (OPL_ChannelManager_t manager)
{
	ChannelManager_t	*instance;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return NULL;
	}
	return (OPL_ChannelManager_t)opl_channel_LoadList(instance, TRUE);
}


int		OPL_Channel_GetFreesatGenre (OPL_ChannelManager_t manager, char ***genre)
{
#if defined(CONFIG_OP_FREESAT)
	HCHAR	iso639[4];
	DxOperator_e	opType;
	HxVector_t			*list;
	DxGroup_t		*group;
	int					i, n;

	(void)manager;

	if (APK_SCENARIO_GetOperatorType(&opType) != ERR_OK || opType != eDxOPERATOR_Freesat)
	{
		HxLOG_Warning("%s() only available in freesat mode!!\n", __FUNCTION__);
		return 0;
	}

	if (APK_SCENARIO_GetMainLanguage(iso639) != ERR_OK)
	{
		HxLOG_Error("%s() Cannot get main language!!\n", __FUNCTION__);
		return 0;
	}

	list = APK_METASVC_Freesat_GetGroupList(HxLANGUAGE_639ToId(iso639));
	if (list == NULL)
	{
		HxLOG_Debug("%s() There is no group list\n", __FUNCTION__);
		return 0;
	}

	n = (int)HLIB_VECTOR_Length(list);
	if (genre == NULL)
		return n;

	*genre = (char **)HLIB_STD_MemAlloc(sizeof(char *) * n);
	if (*genre == NULL)
	{
		HLIB_VECTOR_Delete(list);

		HxLOG_Error("%s() Out of memory!!\n", __FUNCTION__);
		return 0;
	}

	i = 0;
	HxVECTOR_EACH(list, DxGroup_t *, group, {
		(*genre)[i] = HLIB_STD_StrDup(group->name);
		if ((*genre)[i])
			i++;
	});
	HLIB_VECTOR_Delete(list);
	if (i == 0)
		HLIB_STD_MemFree(*genre);
	return i;
#else
	(void)manager;
	(void)genre;
	return 0;
#endif
}

int		OPL_Channel_GetFreesatGenreExt (OPL_ChannelManager_t manager, char ***genreExt)
{
#if defined(CONFIG_OP_FREESAT)
	HCHAR	iso639[4], szGenreExt[512];
	DxOperator_e	opType;
	HxVector_t			*list;
	DxGroup_t			*group;
	DxFreesatGroup_t	*pFsatExt = NULL;
	int					i, n;

	(void)manager;

	if (APK_SCENARIO_GetOperatorType(&opType) != ERR_OK || opType != eDxOPERATOR_Freesat)
	{
		HxLOG_Warning("%s() only available in freesat mode!!\n", __FUNCTION__);
		return 0;
	}

	if (APK_SCENARIO_GetMainLanguage(iso639) != ERR_OK)
	{
		HxLOG_Error("%s() Cannot get main language!!\n", __FUNCTION__);
		return 0;
	}

	list = APK_METASVC_Freesat_GetGroupList(HxLANGUAGE_639ToId(iso639));
	if (list == NULL)
	{
		HxLOG_Debug("%s() There is no group list\n", __FUNCTION__);
		return 0;
	}

	n = (int)HLIB_VECTOR_Length(list);
	if (genreExt == NULL)
		return n;

	*genreExt = (char **)HLIB_STD_MemAlloc(sizeof(char *) * n);
	if (*genreExt == NULL)
	{
		HLIB_VECTOR_Delete(list);

		HxLOG_Error("%s() Out of memory!!\n", __FUNCTION__);
		return 0;
	}

	i = 0;
	HxVECTOR_EACH(list, DxGroup_t *, group, {
		pFsatExt = (DxFreesatGroup_t*)&(group->ex.freesat);
		snprintf(szGenreExt, sizeof(szGenreExt),
			OPL_FAVORITE_GROUPEXT_FORMAT,
			group->id,
			pFsatExt->ucGroupType,
			pFsatExt->ucNondestructiveTuneFlag,
			pFsatExt->ucReturnChannelAccessFlag,
			pFsatExt->ucG2ExtensionFlag,
			pFsatExt->ucDefaultVisableFlag,
			pFsatExt->ucAdultChannelsGroupFlag,
			pFsatExt->ucIpChannelsGroupFlag,
			pFsatExt->ucCaChannelsGroupFlag);

		(*genreExt)[i] = HLIB_STD_StrDup(szGenreExt);
		if ((*genreExt)[i])
			i++;
	});
	HLIB_VECTOR_Delete(list);
	if (i == 0)
		HLIB_STD_MemFree(*genreExt);
	return i;
#else
	(void)manager;
	(void)genreExt;
	return 0;
#endif
}

OPL_Channel_t	OPL_Channel_FindChannelByUID (OPL_ChannelManager_t manager, int uid)
{
	ChannelManager_t *instance;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return (OPL_Channel_t)NULL;
	}
	return (OPL_Channel_t)HLIB_TREE_Lookup(instance->map, (void *)uid);
}

int		OPL_Channel_GetChannelList (OPL_ChannelManager_t manager, OPL_Channel_t **channelList)
{
	HxLOG_Print("##### [%s] #####\n", __FUNCTION__);
	HxLOG_Trace();
	RETURN_IF(channelList == NULL, 0);

	return OPL_Channel_GetFilteredChannelList(manager, channelList, NULL, NULL);
}


int		OPL_Channel_GetFilteredChannelList (OPL_ChannelManager_t manager, OPL_Channel_t **channelList, OPL_BOOL (*filter) (OPL_Channel_t, void *), void *userdata)
{
	HxLOG_Print("##### [%s] #####\n", __FUNCTION__);
	ChannelManager_t	*instance;
	DxService_t			*service;
	OPL_Channel_t		*list;
	int					i, n, count;

	HxLOG_Trace();
	HxLOG_Assert(channelList);

	instance = opl_channel_GetManager(manager);
	if (instance == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return 0;
	}

	if (instance->list == NULL
		|| (n = HLIB_VECTOR_Length(instance->list)) == 0)
	{
		HxLOG_Error("Channel manager does not initialized!\n");
		return 0;
	}

	list = (OPL_Channel_t *)HLIB_STD_MemAlloc(sizeof(OPL_Channel_t) * n);
	if (list == NULL)
	{
		HxLOG_Error("Out of memory!!\n");
		return 0;
	}

	for (i = 0, count = 0 ; i < n ; i++)
	{
		service = (DxService_t *)HLIB_VECTOR_ItemAt(instance->list, i);
		if (service == NULL)
			continue;

		if (filter && !filter((OPL_Channel_t)service, userdata))
			continue;

		list[count] = (OPL_Channel_t)APK_META_SVC_Clone(service);
		if (list[count])
			count++;
	}

	if (count == 0)
	{
		HLIB_STD_MemFree(list);
		list = NULL;
	}
	*channelList = list;
	return count;
}


int		OPL_Channel_ForEach (OPL_ChannelManager_t manager, OPL_BOOL (*filter) (OPL_Channel_t, void *), void *userdata)
{
	ChannelManager_t	*instance;
	OPL_Channel_t		channel;
	int					i, n;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return 0;
	}

	if (instance->list == NULL
		|| (n = HLIB_VECTOR_Length(instance->list)) == 0)
	{
		HxLOG_Error("Channel manager does not initialized!\n");
		return 0;
	}

	for (i = 0 ; i < n ; i++)
	{
		channel = (OPL_Channel_t)HLIB_VECTOR_ItemAt(instance->list, i);
		if (channel == NULL)
			continue;

		if (filter && filter(channel, userdata))
			return i;
	}
	return i;
}


int			OPL_Channel_IndexOf (OPL_ChannelManager_t manager, OPL_Channel_t channel)
{
	ChannelManager_t	*instance;
	int					index;

	HxLOG_Trace();
	RETURN_IF(!channel, -1);

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return -1;
	}

	index = HLIB_VECTOR_ForEach(instance->list
				, (HBOOL (*)(void *, void *))opl_channel_FilterByUID
				, (void *)(((DxService_t *)channel)->uid)
			);
	if (index == HLIB_VECTOR_Length(instance->list))
	{
		HxLOG_Warning("Cannot find index of UID(%d)\n", channel);
		return -1;
	}
	return index;
}


OPL_Channel_t		OPL_Channel_FindChannel (OPL_ChannelManager_t manager, const char *ccid)
{
	HxLOG_Trace();
	HxLOG_Assert(ccid);

	return OPL_Channel_FindChannelByUID(manager, OPL_Channel_CCID2UniqueID(ccid));
}


OPL_Channel_t		OPL_Channel_FindChannelByTriplet (OPL_ChannelManager_t manager, int onid, int tsid, int sid)
{
	ChannelManager_t	*instance;
	DxService_t		*service;
	int					triplet[3];

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return NULL;
	}

	triplet[0] = onid;
	triplet[1] = tsid;
	triplet[2] = sid;

	service = (DxService_t *)HLIB_VECTOR_Find(instance->list, (HBOOL (*)(void *, void *))opl_channel_FilterByTriplet, (void *)triplet);
	if (service == NULL)
	{
		HxLOG_Warning("There is no channel matched with triplet(%d,%d,%d)\n", onid, tsid, sid);
		return NULL;
	}
	return (OPL_Channel_t)service;
}

OPL_Channel_t		OPL_Channel_FindChannelByNumber (OPL_ChannelManager_t manager, int major, int minor)
{
	ChannelManager_t	*instance;
	DxService_t		*service;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return NULL;
	}

	(void)minor;

	service = (DxService_t *)HLIB_VECTOR_Find(instance->list, (HBOOL (*)(void *, void *))opl_channel_FilterByNumber, (void *)major);
	if (service == NULL)
	{
		HxLOG_Warning("There is no channel matched with number(%d/%d)\n", major, minor);
		return NULL;
	}
	return (OPL_Channel_t)service;
}

OPL_Channel_t		OPL_Channel_FindChannelAt (OPL_ChannelManager_t manager, int index)
{
	ChannelManager_t	*instance;
	DxService_t		*service;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return NULL;
	}

	service = (DxService_t *)HLIB_VECTOR_ItemAt(instance->list, index);
	if (service == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return NULL;
	}
	return (OPL_Channel_t)service;
}

OPL_BOOL 			OPL_Channel_IsPossibleToTuneByUpDown(OPL_Channel_t channel, int tuningRange)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = NULL;
	DxUserFlag_t	flag = eDxSvcUserFlag_All;
	service = (DxService_t *)channel;
	if (service)
	{
		/*svc type이 TV, Radio, Data 셋중 하나가 아니라면 선국할 수 없는 채널이다 */
		if(! (service->svcType & (eDxSVC_TYPE_TV|eDxSVC_TYPE_RADIO|eDxSVC_TYPE_DATA)))
		{
			return FALSE;
		}
		switch(tuningRange)
		{
			case eTuningRange_TV:
				/*선국대상이 TV인데 svc type이 TV가 아니라면 선국할 수 없다.*/
				if(!(service->svcType & eDxSVC_TYPE_TV))
				{
					return FALSE;
				}
				break;
			case eTuningRange_Radio_Data:
				/*선국대상이 RADIO_DATA인데 svc type이 radio 또는 data 가 아니라면 선국할 수 없다.*/
				if(!(service->svcType & (eDxSVC_TYPE_RADIO|eDxSVC_TYPE_DATA)))
				{
					return FALSE;
				}
				break;
				/*선국대상이 all이라면 세 타입 모두 선국 가능*/
			case eTuningRange_All:
				break;
				/*선국대상이 그 외라면(불가능) 오동작을 막기위해 선국을 막는다*/
			default:
				return FALSE;
		}
		flag = service->ex.japan.eUserFlag;
		/*ch mask, temp service, skip설정, skipchupdown이 고려되어야 함.*/
		/*JCOM향에는 skip채널이 없기 때문에 on/off를 볼 필요 없음.
		skip채널 자체를 구현할 필요는 없으나, jlabs spec의 invisible flag때문에 동작 구현은 해놓은 상태*/
		if((flag &  (eDxSvcUserFlag_22_SkipSvcByUser|
				 eDxSvcUserFlag_25_MaskCh|
				 eDxSvcUserFlag_07_SkipChUpDown|
				 eDxSvcUserFlag_24_TempSvc)))
		{
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
#else
	return FALSE;
#endif
}

int		OPL_Channel_Count (OPL_ChannelManager_t manager)
{
	ChannelManager_t	*instance;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return 0;
	}
	return (int)HLIB_VECTOR_Length(instance->list);
}

void	OPL_Channel_AddChannel (OPL_ChannelManager_t manager, OPL_Channel_t channel)
{
	ChannelManager_t	*instance;
	OPL_Channel_t	newSvc;

	HxLOG_Trace();
	RETURN_IF_VOID(channel == NULL);

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return;
	}

	if (APK_META_SVC_AddService((DxService_t*)channel) != ERR_OK)
	{
		HxLOG_Warning("Failed to APK:AddService()\n");
		return;
	}

	HxLOG_Debug("Channel added(%d:%s)\n", channel, ((DxService_t*)channel)->name);
	printf ("Channel added(%d:%s, 0x%x)\n", channel, ((DxService_t*)channel)->name, ((DxService_t*)channel)->uid);

	newSvc = OPL_Channel_Clone(channel);
	if (newSvc != NULL)
	{
		HLIB_TREE_Insert(instance->map, (void *)((DxService_t*)newSvc)->uid, (void *)newSvc);
		HLIB_VECTOR_Add(instance->list, (void *)newSvc);
	}

	// update favourite list
	#if 0
	#if defined(CONFIG_PROD_HMS1000S)
	OPL_Favourite_Update();
	#endif
	#endif
}

void	OPL_Channel_RemoveChannel (OPL_ChannelManager_t manager, OPL_Channel_t channel)
{
	ChannelManager_t	*instance;
	DxService_t		*service;

	HxLOG_Trace();
	RETURN_IF_VOID(channel == NULL);

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return;
	}
	service  = (DxService_t *)channel;

	if (APK_META_SVC_RemoveService(service) != ERR_OK)
	{
		HxLOG_Warning("Failed to APK:RemoveService()\n");
		return;
	}

	HxLOG_Debug("Channel removed(%d:%s)\n", channel, service->name);

	HLIB_TREE_Remove(instance->map, (void *)service->uid);
	HLIB_VECTOR_Remove(instance->list, (HINT32)OPL_Channel_IndexOf(manager, channel));

	// update favourite list
	#if 0
	#if defined(CONFIG_PROD_HMS1000S)
	OPL_Favourite_Update();
	#endif
	#endif
}

void	OPL_Channel_RemoveChannelList (OPL_ChannelManager_t manager)
{
	ChannelManager_t	*instance;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return;
	}

	APK_META_SVC_Reset();

	// update favourite list
	#if 0
	#if defined(CONFIG_PROD_HMS1000S)
	OPL_Favourite_Update();
	#endif
	#endif
}

int	OPL_Channel_GetNetworkIDList(OPL_NetworkType_e netType, OPL_ChannelManager_t manager, int ** ppNetworkList)
{
	ChannelManager_t		*instance;
	DxService_t		*service;
	HxVector_t			*list;
	int					i, n, count;
	int					onId = 0;
	int					*pDtNetworkIdList = NULL;
	HBOOL 				bMatched = FALSE;

	instance = opl_channel_GetManager(manager);

	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return 0;
	}
	if(netType <= eOPL_NETWORK_START || netType >= eOPL_NETWORK_END)
	{
		HxLOG_Error("netType(%d) out of bound \n",netType);
		return 0;
	}
	if(ppNetworkList == NULL)
	{
		HxLOG_Error("ppNetworkList is NULL!!\n");
		return 0;
	}

	*ppNetworkList = NULL;
	if(HLIB_VECTOR_Length(instance->list) == 0)
	{
		HxLOG_Error("Channel manager does not initialized!\n");
		return 0;
	}

	list = HLIB_VECTOR_New(NULL, (HINT32 (*)(const void *, const void *))HLIB_VECTOR_CompareHINT32);
	if(list == NULL)
	{
		HxLOG_Error("new network list fail!\n");
		return 0;
	}
	HxVECTOR_EACH(instance->list, DxService_t *, service, {
		if(service != NULL)
		{
			switch(netType)
			{
				case eOPL_DTT_NETWORK:
					bMatched = OPL_MACRO_IS_JAPAN_DT_NETWORK(service->onid);
					break;
				case eOPL_BS_NETWORK:
					bMatched = OPL_MACRO_IS_JAPAN_BS_NETWORK(service->onid);
					break;
				case eOPL_CATV_NETWORK:
					bMatched = OPL_MACRO_IS_JAPAN_CATV_NETWORK(service->onid);
					break;
				case eOPL_CS1_NETWORK:
					bMatched = OPL_MACRO_IS_JAPAN_CS1_NETWORK(service->onid);
					break;
				case eOPL_CS2_NETWORK:
					bMatched = OPL_MACRO_IS_JAPAN_CS2_NETWORK(service->onid);
					break;
				case eOPL_JCHITS_NETWORK:
					bMatched = OPL_MACRO_IS_JAPAN_JCHITS_NETWORK(service->onid);
					break;
				default:
					break;
			}
			if(bMatched == TRUE)
			{
				if(HLIB_VECTOR_IndexOf(list, (void*)service->onid) == -1)
				{
					HLIB_VECTOR_Add(list, (void*)service->onid);
				}
				bMatched = FALSE;
			}
			service = NULL;
		}
	});

	if((n = HLIB_VECTOR_Length(list)) <= 0)
	{
		HxLOG_Error("no DT network! \n");
		return 0;
	}
	*ppNetworkList = HLIB_STD_MemAlloc(sizeof(int)*n);
	if(*ppNetworkList == NULL)
	{
		HxLOG_Error("Out of memory!!\n");
		return 0;
	}
	pDtNetworkIdList = *ppNetworkList;
	for (i = 0, count = 0 ; i < n ; i++)
	{
		onId = (int)HLIB_VECTOR_ItemAt(list, i);
		if (onId == 0)
			continue;
		pDtNetworkIdList[count] = onId;
		count++;
	}
	HLIB_VECTOR_Delete(list);
	list = NULL;
	return count;
}

void		OPL_Channel_FreeNetworkIDList(int *pNetworkList)
{
	if(pNetworkList != NULL)
	{
		HLIB_STD_MemFree(pNetworkList);
	}
}

OPL_NetworkType_e	OPL_Channel_GetNetTypeByOnID(int onId)
{
	OPL_NetworkType_e netType = eOPL_NETWORK_START;
	if(OPL_MACRO_IS_JAPAN_DT_NETWORK(onId))
	{
		netType = eOPL_DTT_NETWORK;
	}
	else if(OPL_MACRO_IS_JAPAN_BS_NETWORK(onId))
	{
		netType = eOPL_BS_NETWORK;
	}
	else if(OPL_MACRO_IS_JAPAN_CATV_NETWORK(onId))
	{
		netType = eOPL_CATV_NETWORK;
	}
	else if(OPL_MACRO_IS_JAPAN_CS1_NETWORK(onId))
	{
		netType = eOPL_CS1_NETWORK;
	}
	else if(OPL_MACRO_IS_JAPAN_CS2_NETWORK(onId))
	{
		netType = eOPL_CS2_NETWORK;
	}
	else if(OPL_MACRO_IS_JAPAN_JCHITS_NETWORK(onId))
	{
		netType = eOPL_JCHITS_NETWORK;
	}
	return netType;
}

int		OPL_Channel_GetChannelListByOnID (int onId, OPL_ChannelManager_t manager, OPL_Channel_t **channelList)
{
	ChannelManager_t	*instance;
	DxService_t		*service;
	OPL_Channel_t		*list;
	int					i, n, count;

	HxLOG_Trace();
	RETURN_IF(channelList == NULL, 0);

	instance = opl_channel_GetManager(manager);
	if (instance == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return 0;
	}

	if (instance->list == NULL
		|| (n = HLIB_VECTOR_Length(instance->list)) == 0)
	{
		HxLOG_Error("Channel manager does not initialized!\n");
		return 0;
	}

	if (channelList == NULL)
		return n;

	list = (OPL_Channel_t *)HLIB_STD_MemAlloc(sizeof(OPL_Channel_t) * n);
	if (list == NULL)
	{
		HxLOG_Error("Out of memory!!\n");
		return 0;
	}

	for (i = 0, count = 0 ; i < n ; i++)
	{
		service = (DxService_t *)HLIB_VECTOR_ItemAt(instance->list, i);
		if (service == NULL)
			continue;
		if(onId == service->onid)
		{
			list[count] = (OPL_Channel_t)APK_META_SVC_Clone(service);
			if (list[count])
				count++;
		}
	}

	if (count == 0)
	{
		HLIB_STD_MemFree(list);
		list = NULL;
	}
	*channelList = list;
	return count;
}

void		OPL_Channel_FreeChannelList(OPL_Channel_t *channelList)
{
	if(channelList != NULL)
	{
		HLIB_STD_MemFree(channelList);
	}
}
#if 0
int		OPL_Channel_CreateFilteredList (OPL_ChannelManager_t manager, int **uidList, const char *field, const char *value, const char *favID)
{
#define VALUE_INT_WITH_CONVERT(v,constant)	\
	do{\
		if (HLIB_STD_IsInteger(v))\
			value_int = HLIB_STD_StrToINT32(v);\
		else {\
			value_int = constant(v);\
			if(value_int < 0) goto ERROR;\
		}\
	} while(0)
#define VALUE_INT(v)	\
	do{\
		if (HLIB_STD_IsInteger(v))\
			value_int = HLIB_STD_StrToINT32(v);\
		else goto ERROR;\
	} while(0)
#define VALUE_STR(v)	\
	do{value_str = v;} while(0)
#define VALUE_BOOL(v)	\
	do{\
		if (strcmp(v, "true") == 0) value_bool = OPL_TRUE;\
		else if (strcmp(v, "false") == 0) value_bool = OPL_FALSE;\
		else goto ERROR;\
	} while(0)

	const char		*value_str;
	int				value_int;
	OPL_BOOL		value_bool;

	char			buf[256];
	HxVector_t		*list, *garbage;
	HxVector_t		*filtered;
	DxService_t	*service;
	int 			i, count;
	ChannelManager_t	*instance;

	HxLOG_Trace();
	HxLOG_Assert(uidList && field && value);

	printf("%s(%s, %s)\n", __FUNCTION__, field, value);

	instance = opl_channel_GetManager(manager);
	if (instance == NULL || instance->list == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return 0;
	}

	filtered = HLIB_VECTOR_New(NULL, NULL);
	if (filtered == NULL)
	{
		HxLOG_Error("Out of memory!!\n");
		return 0;
	}

	garbage = NULL;
	if (favID)
	{
		list = opl_channel_GetFavouriteList(instance, favID);
		if (list == NULL)
			goto ERROR;
		garbage = list;
	}
	else
	{
		list = instance->list;
	}

	if (strcmp(field, "channelType") == 0)
	{
		VALUE_INT_WITH_CONVERT(value, opl_channel_GetConstantValue);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && (int)OPL_Channel_GetChannelType((OPL_Channel_t)service) == value_int)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "idType") == 0)
	{
		VALUE_INT_WITH_CONVERT(value, opl_channel_GetConstantValue);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && (int)OPL_Channel_GetIdType((OPL_Channel_t)service) == value_int)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "ccid") == 0)
	{
		VALUE_STR(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service
				&& strcmp(OPL_Channel_GetCCID((OPL_Channel_t)service, buf, 256), value_str) == 0)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "tunerID") == 0)
	{
		VALUE_STR(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service
				&& strcmp(OPL_Channel_GetTunerID((OPL_Channel_t)service, buf, 256), value_str) == 0)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "onid") == 0)
	{
		VALUE_INT(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && (int)service->onid == value_int)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "tsid") == 0)
	{
		VALUE_INT(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && (int)service->tsid == value_int)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "sid") == 0)
	{
		VALUE_INT(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && (int)service->svcid == value_int)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "locked") == 0)	// ??
	{
		VALUE_BOOL(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && (int)!!service->locked == (int)!!value_bool)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "manualBlock") == 0)
	{
		VALUE_BOOL(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && (int)!!service->locked == (int)!!value_bool)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	/*else if (strcmp(field, "authorized") == 0)
	{
		VALUE_BOOL(value);

	}*/
	else if (strcmp(field, "hidden") == 0)
	{
		VALUE_BOOL(value);
		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && (int)!!service->visibleFlag == (int)!!value_bool)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "favourite") == 0)
	{
		OPL_BOOL	favourite;
		VALUE_BOOL(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service == NULL)
				continue;

			favourite = OPL_Channel_GetFavouriteIDs((OPL_Channel_t)service, NULL) ? OPL_TRUE : OPL_FALSE;
			if (favourite == value_bool)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "favIDs") == 0)
	{
		OPL_Favourite_t	hFav;

		VALUE_STR(value);

		hFav = OPL_Favourite_FindByID(value_str);
		if (!hFav)
			goto ERROR;

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service == NULL)
				continue;

			if (OPL_Favourite_HasChannel(hFav, (int)service->uid))
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "genre") == 0)
	{
		VALUE_STR(value);

		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service == NULL)
				continue;

			if (OPL_Channel_HasGenre((OPL_Channel_t)service, value_str))
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "casIDs") == 0)
	{
		VALUE_INT_WITH_CONVERT(value, opl_channel_GetConstantValue);
		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service == NULL)
				continue;

			if (OPL_Channel_HasCasID((OPL_Channel_t)service, (OPL_ChannelCasID_e)value_int))
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else if (strcmp(field, "operatorId") == 0)
	{
		VALUE_INT(value);
		HxVECTOR_EACH(list, DxService_t *, service, {
			if (service && service->optype == value_int)
				HLIB_VECTOR_Add(filtered, (void *)service->uid);
		});
	}
	else
	{
		printf("%s(Unknown Field:%s)\n", __FUNCTION__, field);
	}

	count = (int)HLIB_VECTOR_Length(filtered);
	if (count == 0)
		goto ERROR;

	*uidList = (int *)HLIB_STD_MemAlloc(sizeof(int) * count);
	if (*uidList == NULL)
		goto ERROR;

	// sort by LCN
	HLIB_VECTOR_SortEx(filtered, opl_channel_SortByLCN, (void *)instance);
	for (i = 0 ; i < count ; i++)
	{
		(*uidList)[i] = (int)HLIB_VECTOR_ItemAt(filtered, i);
	}

	if (garbage)
		HLIB_VECTOR_Delete(garbage);
	if (filtered)
		HLIB_VECTOR_Delete(filtered);
	return count;

ERROR:
	*uidList = NULL;
	if (garbage)
		HLIB_VECTOR_Delete(garbage);
	if (filtered)
		HLIB_VECTOR_Delete(filtered);
	printf("%s(field:%s, value:%s, fav:%s) return error!\n", __FUNCTION__, field, value, favID);
	return 0;
#undef	VALUE_INT_WITH_CONVERT
#undef	VALUE_INT
#undef	VALUE_STR
#undef	VALUE_BOOL
}
#endif

#define	____OPL_CHANNEL_PUBLIC_ATTRIBUTES____________________________________________________________

OPL_Channel_t	OPL_Channel_Clone (OPL_Channel_t channel)
{
	return (OPL_Channel_t)APK_META_SVC_Clone((void *)channel);
}

void			OPL_Channel_Delete (OPL_Channel_t channel)
{
	APK_META_SVC_Delete((void *)channel);
}

int				OPL_Channel_GetUniqueID (OPL_Channel_t channel)
{
	if (channel)
	{
		return (int)(((DxService_t *)channel)->uid);
	}
	return 0;
}


char *		OPL_Channel_MakeCCID (char *buf, unsigned int n, int uid, int onid, int tsid, int sid)
{
	HxLOG_Trace();

	if (buf == NULL || n == 0)
	{
		return NULL;
	}
	HxSTD_PrintToStrN(buf, n, "ccid:%d.%d.%d.%d", uid, onid, tsid, sid);
	return buf;
}

int			OPL_Channel_CCID2UniqueID (const char *ccid)
{
	HCHAR	buf[64];
	HCHAR	*token, *save;

	HxLOG_Trace();
	HxLOG_Assert(ccid);

	if (!HLIB_STD_StrStartWith((const HCHAR *)ccid, "ccid:"))
		return 0;

	HLIB_STD_StrNCpySafe(buf, (const HCHAR *)ccid + 5, 64);

	token = HxSTD_StrTok(buf, ".", &save);
	if (token == NULL)
		return 0;

	if (!HLIB_STD_IsInteger(token))
		return 0;

	return (int)HLIB_STD_StrToINT32(token);
}

OPL_BOOL	OPL_Channel_IsSupportedChannelID (OPL_ChannelID_e id)
{
	HxLOG_Trace();
/*
	DxOperator_e  opType;

	if (APK_SCENARIO_GetOperatorType(&opType) != ERR_OK)
		return OPL_FALSE;

	switch(opType)
	{
	case eDxOPERATOR_Freesat:	// dynamic update!!
		break;

	default;
		break;
	}
*/
	// 아 몰라 일단 이렇게 하자
	switch(id)
	{
	case eOPL_ChannelID_DVB_C:
	case eOPL_ChannelID_DVB_S:
	case eOPL_ChannelID_DVB_T:
	case eOPL_ChannelID_DVB_C2:
	case eOPL_ChannelID_DVB_S2:
	case eOPL_ChannelID_DVB_T2:
		return OPL_TRUE;
	default:
		break;
	}
	return OPL_FALSE;
}

OPL_ChannelType_e	OPL_Channel_GetChannelType (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	switch (service->svcType)
	{
	case eDxSVC_TYPE_TV:
		return eOPL_ChannelType_TV;

	case eDxSVC_TYPE_RADIO:
		return	eOPL_ChannelType_RADIO;

	default:
		break;
	}
	return eOPL_ChannelType_OTHER;
}


int			OPL_Channel_GetOrgSvcType(OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;
	DxOrgSvcType_e svcType;
	HxLOG_Trace();
	HxLOG_Assert(service);
	svcType = service->orgSvcType;
#if defined(CONFIG_OP_JAPAN)
	switch(service->orgSvcType)
	{
		case eDxOrgSVC_TYPE_ARIB_TEMP_VIDEO:
			if((service->ex.japan.eUserFlag& eDxSvcUserFlag_24_TempSvc) == 0)
			{
				svcType = eDxOrgSVC_TYPE_DIGITAL_TV;
			}
			break;
		case eDxOrgSVC_TYPE_ARIB_TEMP_AUDIO:
			if((service->ex.japan.eUserFlag & eDxSvcUserFlag_24_TempSvc) == 0)
			{
				svcType = eDxOrgSVC_TYPE_DIGITAL_RADIO;
			}
			break;
		case eDxOrgSVC_TYPE_ARIB_TEMP_DATA:
			if((service->ex.japan.eUserFlag & eDxSvcUserFlag_24_TempSvc) == 0)
			{
				svcType = eDxOrgSVC_TYPE_DATA;
			}
			break;
		default:
			break;
	}
#endif
	return svcType;
}

OPL_ChannelID_e	OPL_Channel_GetIdType (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;
	DxTransponder_t		*ts;
	OPL_ChannelID_e		id;

	HxLOG_Trace();
	HxLOG_Assert(service);

	switch (service->deliType)
	{
	case eDxDELIVERY_TYPE_SAT:	//
		ts = APK_META_SVC_GetTransponder(service->tsuid);
		if (ts && ts->tuningParam.sat.tuningInfo.transSpec == eDxSAT_TRANS_DVBS2)
		{
			id = eOPL_ChannelID_DVB_S2;
		}
		else
		{
			id = eOPL_ChannelID_DVB_S;
		}
		APK_META_SVC_Delete(ts);
		return id;

		case eDxDELIVERY_TYPE_CAB: //
			return eOPL_ChannelID_DVB_C;
		case eDxDELIVERY_TYPE_TER: //
		ts = APK_META_SVC_GetTransponder(service->tsuid);
		if (ts && ts->tuningParam.ter.system == eDxTER_DVB_T2)
		{
			id = eOPL_ChannelID_DVB_T2;
		}
		else
		{
			id = eOPL_ChannelID_DVB_T;
		}
		APK_META_SVC_Delete(ts);
		return id;
	case eDxDELIVERY_TYPE_ANALOG:  //
		return eOPL_ChannelID_ANALOG; //
	case eDxDELIVERY_TYPE_MULTICAST:  //
	default:
		break;
	}
	HxLOG_Warning("[%s:%d] delivery type -> oipf.id faild\n", __HxFILE__, __HxLINE__);
	return eOPL_ChannelID_ANALOG;
}

OPL_BOOL	OPL_Channel_HasCasID (OPL_Channel_t channel, OPL_ChannelCasID_e id)
{
	DxService_t	*service = (DxService_t *)channel;
	int		n;
	OPL_ChannelCasID_e	ids[32];

	HxLOG_Trace();
	HxLOG_Assert(service);

	n = opl_channel_GetCasIDs(service, ids);
	while (n--)
	{
		if (ids[n] == id)
			return OPL_TRUE;
	}
	return OPL_FALSE;
}


int		OPL_Channel_GetCasIDs (OPL_Channel_t channel, OPL_ChannelCasID_e **ids)
{
	DxService_t	*service = (DxService_t *)channel;
	int		n;
	OPL_ChannelCasID_e	id[32];

	HxLOG_Trace();
	HxLOG_Assert(service);

	n = opl_channel_GetCasIDs(service, id);
	if (ids && n > 0)
	{
		*ids = (OPL_ChannelCasID_e *)HLIB_STD_MemAlloc(sizeof(OPL_ChannelCasID_e) * n);
		if (*ids)
		{
			memcpy(*ids, id, sizeof(OPL_ChannelCasID_e) * n);
		}
	}
	return n;
}

char *	OPL_Channel_GetCCID (OPL_Channel_t channel, char *buf, unsigned int n)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return OPL_Channel_MakeCCID(buf, n, service->uid, service->onid, service->tsid, service->svcid);
}

BPLStatus		OPL_Channel_GetIntTunerID (OPL_Channel_t channel , HINT32 *tunerId)
{
	DxService_t	*service = (DxService_t *)channel;
	DxTransponder_t	*ts;

	HxLOG_Trace();
	HxLOG_Assert(service && tunerId);

	ts = APK_META_SVC_GetTransponder(service->tsuid);
	if (ts == NULL)
		return BPL_STATUS_ERROR;

	*tunerId = ts->tunerid;

	APK_META_SVC_Delete(ts);

	return BPL_STATUS_OK;
}

char *	OPL_Channel_GetTunerID (OPL_Channel_t channel, char *buf, unsigned int n)
{
	DxService_t	*service = (DxService_t *)channel;
	DxTransponder_t	*ts;

	HxLOG_Trace();
	HxLOG_Assert(service && buf && n);

	ts = APK_META_SVC_GetTransponder(service->tsuid);
	if (ts == NULL)
		return NULL;

	HxSTD_PrintToStrN(buf, n, "%d", ts->tunerid);
	APK_META_SVC_Delete(ts);

	return buf;
}

char *	OPL_Channel_GetTuningInfo (OPL_Channel_t channel, char *buf, unsigned int n)
{
	DxService_t	*service = (DxService_t *)channel;
	DxTransponder_t	*ts;
	DxAntInfo_t		*pstAntInfo;

	HxLOG_Trace();
	HxLOG_Assert(service && buf && n);

	ts = APK_META_SVC_GetTransponder(service->tsuid);
	if (ts == NULL)
		return NULL;

#if defined(CONFIG_OP_UK_DTT)
	HxSTD_snprintf(buf,n,"%s, %dkHz, %s, %s, %s",
							opl_channel_GetStrDVBTSystem(ts->tuningParam.ter.eSystem),
							ts->tuningParam.ter.ulFrequency,
							opl_channel_GetStrTBandWidth(ts->tuningParam.ter.eBandWidth),
							opl_channel_GetStrTTransmode(ts->tuningParam.ter.eTransMode),
							opl_channel_GetStrTGuardInterval(ts->tuningParam.ter.eGuardInterval));
#else
	pstAntInfo = APK_META_ANTENNA_GetInfo(ts->tuningParam.sat.antInfo.uid);

	HxSTD_snprintf(buf,n,"%s, %dMhz, %s, %d, %s",
							pstAntInfo->satName,
							ts->tuningParam.sat.tuningInfo.ulFrequency,
							opl_channel_GetStrPolar(ts->tuningParam.sat.tuningInfo.ePolarization),
							ts->tuningParam.sat.tuningInfo.ulSymbolRate,
							opl_channel_GetStrCodeRate(ts->tuningParam.sat.tuningInfo.eFecCodeRate));
#endif

	APK_META_SVC_Delete(ts);

	return buf;
}

int		OPL_Channel_GetOrgNetworkID (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return (int)service->onid;
}

int		OPL_Channel_GetTransponderID (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return (int)service->tsid;
}

int		OPL_Channel_GetServiceID (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return (int)service->svcid;
}


int		OPL_Channel_GetSourceID (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	HxLOG_Message("[%s] Not implemented yet...\n");

	return 0;
}


int		OPL_Channel_GetFrequency (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;
	DxTransponder_t *transponder = NULL;
	int			nFrequency = 0;
	HxLOG_Trace();
	RETURN_IF(service == NULL, 0);
	transponder = APK_META_SVC_GetTransponder(service->tsuid);
	RETURN_IF(transponder == NULL, 0);
	HxLOG_Message("[%s] eDeliType %d\n",__FUNCTION__,transponder->tuningParam.eDeliType);
	switch(transponder->tuningParam.eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			nFrequency = (int)transponder->tuningParam.sat.tuningInfo.ulFrequency;
			break;
		case eDxDELIVERY_TYPE_TER:
			nFrequency = (int)transponder->tuningParam.ter.ulFrequency;
			break;
		case eDxDELIVERY_TYPE_CAB:
			nFrequency = (int)transponder->tuningParam.cab.ulFrequency;
			break;
		default:
			break;
	}
	HxLOG_Message("[%s] nFrequency %d\n",__FUNCTION__,nFrequency);
	if(transponder != NULL)
	{
		APK_META_SVC_Delete(transponder);
	}
	return nFrequency;
}


int		OPL_Channel_GetCNI (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	HxLOG_Message("[%s] Not implemented yet...\n");

	return 0;
}

const char *	OPL_Channel_GetName (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return (char *)service->name;
}

void			OPL_Channel_SetName (OPL_Channel_t channel, const char* chname)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service && chname);

	if (strcmp(service->name, chname) != 0)
	{
		HLIB_STD_StrNCpySafe(service->name, chname, DxNAME_LEN );
		service->renamed = TRUE;

		opl_channel_UpdateChannel(channel);
	}
}

int		OPL_Channel_GetMajorChannel (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return service->lcn;
}

int		OPL_Channel_GetMinorChannel (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	// ????
	return 0;
}


int		OPL_Channel_GetDeliverySystemDescriptor (OPL_Channel_t channel, unsigned char **buf)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	HxLOG_Message("[%s] Not implemented yet...\n");

	*buf = (unsigned char *)HLIB_STD_StrDup(" ");

	return 0;
}

int		OPL_Channel_GetFavouriteIDs (OPL_Channel_t channel, char ***favIDs)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(channel);

	return (int)OPL_Favourite_GetFavouriteIDs((int)service->uid, favIDs);
}


OPL_BOOL		OPL_Channel_IsHD (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return service->highDefinition == eDxVIDEO_TYPE_HD ? OPL_TRUE : OPL_FALSE;
}

OPL_BOOL		OPL_Channel_IsLocked (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	// TODO: parental rating에 물어봐야 함...
	return service->locked ? OPL_TRUE : OPL_FALSE;
}


OPL_BOOL		OPL_Channel_IsManualBlocked (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return service->locked ? OPL_TRUE : OPL_FALSE;
}


void			OPL_Channel_SetManualBlock (OPL_Channel_t channel, OPL_BOOL block)
{
	DxService_t	*service = (DxService_t *)channel;
	HBOOL			locked;

	HxLOG_Trace();
	HxLOG_Assert(service);

	locked = (block) ? TRUE : FALSE;
	if (service->locked == locked)
		return;

	service->locked = locked;
//	printf("jhlee4: %s|%s.manualBlock=%s\n", __FUNCTION__, service->name, block ? "true" : "false");

	opl_channel_UpdateChannel(channel);
}

void			OPL_Channel_SetMajorChannel (OPL_Channel_t channel, int majorChannel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	if (service->lcn == majorChannel)
		return;

	service->lcn = majorChannel;
//	printf("jhlee4: %s|%s.manualBlock=%s\n", __FUNCTION__, service->name, block ? "true" : "false");
	opl_channel_UpdateChannel(channel);
}

OPL_BOOL	OPL_Channel_IsRemoved (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return service->removed ? OPL_TRUE : OPL_FALSE;
}


void			OPL_Channel_SetRemovedFlag (OPL_Channel_t channel, OPL_BOOL removed)
{
	DxService_t	*service = (DxService_t *)channel;
	HBOOL			flag;

	HxLOG_Trace();
	HxLOG_Assert(service);

	flag = (removed) ? TRUE : FALSE;
	if (service->removed == flag)
		return;

	service->removed = flag;
//	printf("jhlee4: %s|%s.removed=%s\n", __FUNCTION__, service->name, removed ? "true" : "false");

	opl_channel_UpdateChannel(channel);
}

OPL_BOOL	OPL_Channel_IsNewChannel(OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return (service->updateSvcFlag & eDxSVC_UPDATE_NEWCHANNEL) ? OPL_TRUE : OPL_FALSE;
}

void		OPL_Channel_SetNewChannelFlag(OPL_Channel_t channel, OPL_BOOL newChannel)
{
	DxService_t	*service = (DxService_t *)channel;
	HBOOL			flag;

	HxLOG_Trace();
	HxLOG_Assert(service);

	flag = (newChannel) ? TRUE : FALSE;

	if (flag)
		service->updateSvcFlag = HxMACRO_BITSET(service->updateSvcFlag, eDxSVC_UPDATE_NEWCHANNEL);
	else
		service->updateSvcFlag = HxMACRO_BITCLR(service->updateSvcFlag, eDxSVC_UPDATE_NEWCHANNEL);

	opl_channel_UpdateChannel(channel);
}

char *		OPL_Channel_GetIPBroadcastID (OPL_Channel_t channel, char *buf, unsigned int n)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service && buf && n);

	HxLOG_Message("[%s] Not implemented yet...\n");
	snprintf(buf, n, " ");
	return buf;
}


int			OPL_Channel_GetChannelMaxBitRate (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	HxLOG_Message("[%s] Not implemented yet...\n");

	return 0;
}

int			OPL_Channel_GetChannelTTR (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	HxLOG_Message("[%s] Not implemented yet...\n");

	return 0;
}


const char *		OPL_Channel_GetLongName (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

#if defined( CONFIG_OP_JAPAN )
{
	/* jlabs 스펙에 의해서 일본향은 provider name 으로.. */
	static char		ProvName[256];

	HxSTD_MemSet( ProvName, 0x00, 256 );
	OPL_Channel_GetProviderName( channel , ProvName );
	return ProvName;
}
#else
	HxLOG_Message("[%s] Not implemented yet...\n");
#endif

	return NULL;
}

const char *		OPL_Channel_GetDescription (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;
	char			*retVal = NULL;
	HxLOG_Trace();
	HxLOG_Assert(service);

	HxLOG_Message("[%s] Not implemented yet...\n");
	retVal = " ";

	return retVal;
}


OPL_BOOL	OPL_Channel_IsAuthorised (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	HxLOG_Message("[%s] Not implemented yet...\n");

	return OPL_TRUE;
}

OPL_BOOL	OPL_Channel_HasGenre (OPL_Channel_t channel, const char *genre)
{
	DxService_t	*service = (DxService_t *)channel;
	DxGroup_t	*group;
	HINT32			i;

	HxLOG_Trace();
	HxLOG_Assert(service);

	if (genre == NULL)
	{
		return OPL_Channel_GetGenre(channel, NULL) ? OPL_TRUE : OPL_FALSE;
	}

	for (i = 0 ; i < DxGROUP_LEN ; i++)
	{
		if (service->grpuids[i] == 0)
			break;

		group = APK_META_SVC_GetGroup(service->grpuids[i]);
		if (group == NULL)
			continue;

		if (strcmp(group->name, genre) == 0)
		{
			APK_META_SVC_Delete(group);
			return OPL_TRUE;
		}
		APK_META_SVC_Delete(group);
	}
	return OPL_FALSE;
}

int			OPL_Channel_GetGenre (OPL_Channel_t channel, char ***genre)
{
	DxService_t	*service = (DxService_t *)channel;
	DxGroup_t	*group;
	HINT32			i, n;
	char 			*genreBuf[DxGROUP_LEN];

	HxLOG_Trace();
	HxLOG_Assert(service);

	memset(genreBuf, 0, sizeof(char *) * DxGROUP_LEN);
	for (i = 0, n = 0 ; i < DxGROUP_LEN ; i++)
	{
		if (service->grpuids[i] == 0)
			break;

		group = APK_META_SVC_GetGroup(service->grpuids[i]);
		if (group == NULL)
			continue;

		if (genre)
		{
			genreBuf[n] = HLIB_STD_StrDup(group->name);
		}
		n++;
		APK_META_SVC_Delete(group);
	}

	if (n == 0)
		return 0;

	if (genre)
	{
		*genre = (char **)HLIB_STD_MemAlloc(sizeof(char *) * n);
		if (*genre)
		{
			for (i = 0 ; i < n ; i++)
				(*genre)[i] = genreBuf[i];
		}
		else
		{
			for (i = 0 ; i < n ; i++)
				HLIB_STD_MemFree(genreBuf[i]);
			return 0;
		}
	}
	return (int)n;
}


OPL_BOOL	OPL_Channel_IsHidden (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return service->visibleFlag ? OPL_FALSE : OPL_TRUE;
}


const char *	OPL_Channel_GetLogoURL (OPL_Channel_t channel)
{
	HxLOG_Trace();

#ifndef	CONFIG_OP_JAPAN
	DxService_t	*service = (DxService_t *)channel;
	// it have to be careful... but... i am so lazy.. :-). (STATIC!!!!)
	STATIC DxChannelLogo_t stLogo;
	HINT32			sd;
	const char		*empty = "";

	HxLOG_Assert(service);

	APK_META_SVC_GetLogoUrl(service->uid, &stLogo);

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc, HLIB_STD_MemFree);
	if(sd == 0)
		return empty;
	{
		HCHAR	*p;
#if defined(CONFIG_SUPPORT_IPEPG_CH_LOGO_STORE_HDD)
		p = (HCHAR*)THAPI_GetThumbnailURL(stLogo.ucServerLogoUrl, 0, "/mnt/hd1", FALSE);
#else
		p = (HCHAR*)THAPI_GetThumbnailURL(stLogo.ucServerLogoUrl, 0, "/var/lib/humaxtv_user", FALSE);
#endif
		if (p)
		{
			#if defined (CONFIG_OP_AUS_DTT) || defined(CONFIG_OP_UK_DTT)
			HCHAR				*pszEncodedTempUrl = NULL;

			// web에서 encoding이 필요한 cache 파일명을 위해
			pszEncodedTempUrl = thapi_getStringBuffer(1024);
   			HLIB_STD_URLEncodeExceptX (pszEncodedTempUrl, p, 1024, '/');
			HxLOG_Debug("[%s:%d] Already Cached pszTempUrl(%s) pszEncodedTempUrl(%s)\n", __FUNCTION__, __LINE__, pszTempUrl, pszEncodedTempUrl);
			p = pszEncodedTempUrl;
			#endif

			HLIB_RWSTREAM_Print(sd, "file://%s", p);
			return HLIB_RWSTREAM_CloseWithoutBuf(sd);
		}

		return empty;
	}
#elif defined(CONFIG_OP_JAPAN)
	// ARIB logo type 0 = 48x24  (SD 4:3 small)
	return OPL_Channel_GetLogo(channel, 48, 24);
#else
	//	TODO:
	return empty;
#endif
}

const char *		OPL_Channel_GetSvcOpTypeString (OPL_Channel_t channel)
{
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	switch(service->optype)
	{
		case	eDxOPTYPE_FREESAT:
			return "freesat";
		default:
			break;
	}

	return "none";
}

char *		OPL_Channel_GetField (OPL_Channel_t channel, const char *field)
{
	DxService_t	*service = (DxService_t *)channel;
	const HCHAR	*value = NULL;
#ifdef	CONFIG_OP_FREESAT
	HCHAR		temp[32] = {0,};
#endif

	HxLOG_Trace();
	HxLOG_Assert(service && field);

#ifdef	CONFIG_OP_FREESAT
	if (strcmp(field, "selectable") == 0)
	{
		// TODO:
		value = service->ex.freesat.ucUserSelectable ? "true" : "false";
	}
	else if (strcmp(field, "operatorId") == 0)
	{
		switch (service->optype)
		{
		case eDxOPTYPE_FREESAT:
			value = "8";//eDxOPERATOR_Freesat;
			break;

		default:
			value = "1";
			break;
		}
	}
	else if (strcmp(field, "shortname") == 0)
	{
		if(service->optype == eDxOPTYPE_FREESAT)
		{
			value = service->ex.freesat.szShortName;
		}
	}
	else if(strcmp(field, "fsatsvcId") == 0)
	{
		if(service->optype == eDxOPTYPE_FREESAT)
		{
			snprintf(temp, 32, "%d", service->ex.freesat.svcId);
			value = temp;
		}
	}
	else if (strcmp(field, "guidancesdttext") == 0)
	{

		if(service->optype == eDxOPTYPE_FREESAT)
		{
			value = (HCHAR *)(service->ex.freesat.szGuidanceStr);
		}
	}
	else if (strcmp(field, "guidancesdttype") == 0)
	{

		if(service->optype == eDxOPTYPE_FREESAT)
		{
			snprintf(temp, 32, "%d", service->ex.freesat.ucGuidanceType);
			value = temp;
		}
	}

	else
	{
		HxLOG_Warning("Unknown field(%s)!!\n", field);
		return NULL;
	}
#endif

	if (value)
	{
		return HLIB_STD_StrDup(value);
	}
	return NULL;
}

int		OPL_Channel_GetFreesatServiceID (OPL_Channel_t channel)
{
#ifdef	CONFIG_OP_FREESAT
	DxService_t	*service = (DxService_t *)channel;

	HxLOG_Trace();
	HxLOG_Assert(service);

	return (int)service->ex.freesat.svcId;
#else
	return 0;
#endif
}

int	OPL_Channel_GetFsatSvcIdByUID (OPL_ChannelManager_t manager, int uid)
{
#ifdef	CONFIG_OP_FREESAT
	ChannelManager_t *instance;
	DxService_t	*service;
	DxFreesatService_t *fsatservice;

	HxLOG_Trace();

	instance = opl_channel_GetManager(manager);
	if (instance == NULL)
	{
		HxLOG_Error("There is no channel manager!!\n");
		return 0;
	}

	service = (DxService_t *)HLIB_TREE_Lookup(instance->map, (void *)uid);
	if(service == NULL)
	{
		return 0;
	}
	fsatservice = &(service->ex.freesat);

	if(fsatservice == NULL)
	{
		return 0;
	}

	return fsatservice->svcId;
#else
	return 0;
#endif
}


OPL_Channel_t OPL_Channel_GetRepresentativeChannelInChannelList(OPL_Channel_t *pChannels, int length)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t	*service = NULL;
	OPL_NetworkType_e eNetType = eOPL_NETWORK_START;
	int i = 0;

	RETURN_IF((pChannels== NULL ||length <= 0), NULL);
	service = pChannels[0];
	RETURN_IF(service== NULL, NULL);

	eNetType = OPL_Channel_GetNetTypeByOnID(service->onid);
	switch(eNetType)
	{
		case eOPL_DTT_NETWORK:
			for(i = 0; i < length ; i++)
			{
				service = pChannels[i];
				if(service != NULL)
				{
					// TODO: 대표채널 구분하는 방법이 맞는지 확인 필요
					if((service->svcid & 0x07) == 0)
					{
						return pChannels[i];
					}
				}
			}
			break;
		case eOPL_BS_NETWORK:
		case eOPL_CATV_NETWORK:
		case eOPL_CS1_NETWORK:
		case eOPL_CS2_NETWORK:
		case eOPL_JCHITS_NETWORK:
			return pChannels[0];
			break;
		default:
			break;
	}

#endif
	return NULL;
}
int		OPL_Channel_GetOneTouch(OPL_Channel_t channel)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;
	HxLOG_Trace();
	RETURN_IF(service == NULL, -1);
	japan = &(service->ex.japan);
	RETURN_IF(japan == NULL, -1);
	// TODO: spec에서 의도한 값이 맞는지 확인 필요
	return japan->nOnetouchNum;
#else
	return -1;
#endif

}

void		OPL_Channel_SetOneTouch(OPL_Channel_t channel, int oneTouch)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;
	HxLOG_Trace();
	RETURN_IF_VOID(service == NULL);
	japan = &(service->ex.japan);
	RETURN_IF_VOID(japan == NULL);
	japan->nOnetouchNum = oneTouch;
	opl_channel_UpdateChannel(channel);
#else
		return;
#endif
}

int		OPL_Channel_GetAreacode(OPL_Channel_t channel)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	HxLOG_Trace();
	RETURN_IF(service == NULL, -1);
	return OPL_MACRO_GET_JAPAN_AREA_ID(service->svcid);
#else
	return -1;
#endif

}

BPLStatus		OPL_Channel_GetSignalLevel(OPL_Channel_t channel, unsigned int *pulAgc, unsigned int *pulBer, unsigned int*pulSnr, int*pnStrength)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxTransponder_t *transponder = NULL;
	DxNetwork_t *network = NULL;
	DxJapanNet_t *japan = NULL;
	BPLStatus 	ret = BPL_STATUS_ERROR;
	HxLOG_Trace();
	RETURN_IF(((service == NULL)||(pulAgc == NULL) ||(pulBer == NULL)||(pulSnr == NULL)||(pnStrength == NULL)), BPL_STATUS_ERROR);

	transponder = APK_META_SVC_GetTransponder(service->tsuid);
	if(transponder == NULL)
	{
		goto funcExit;
	}
	network = APK_META_SVC_GetNetwork(transponder->netuid);
	if(network == NULL)
	{
		goto funcExit;
	}
	japan = &(network->ex.japan);
	if(japan == NULL)
	{
		goto funcExit;
	}
	*pulAgc = japan->ulAgc;
	*pulBer = japan->ulBer;
	*pulSnr = japan->ulSnr;
	*pnStrength = japan->nStrength;
	ret = BPL_STATUS_OK;
funcExit:
	if(network != NULL)
	{
		APK_META_SVC_Delete(network);
	}
	if(transponder != NULL)
	{
		APK_META_SVC_Delete(transponder);
	}
	return ret;
#else
	return BPL_STATUS_ERROR;
#endif

}



int		OPL_Channel_GetBranchNum(OPL_Channel_t channel)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;
	HxLOG_Trace();
	RETURN_IF(service == NULL, -1);
	japan = &(service->ex.japan);
	RETURN_IF(japan == NULL, -1);
	// TODO: spec에서 의도한 값이 맞는지 확인 필요
	return japan->nBranchNum;
#else
	return -1;
#endif

}

BPLStatus	OPL_Channel_GetTsName(OPL_Channel_t channel, char *tsName, unsigned int length)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxTransponder_t *transponder = NULL;
	DxJapanTs_t *japan = NULL;
	unsigned int tsNameLength = 0;
	HxLOG_Trace();
	RETURN_IF(service == NULL, BPL_STATUS_ERROR);
	RETURN_IF(tsName == NULL, BPL_STATUS_ERROR);
	transponder = APK_META_SVC_GetTransponder(service->tsuid);
	RETURN_IF(transponder == NULL, BPL_STATUS_ERROR);
	japan = &(transponder->ex.japan);
	RETURN_IF(japan == NULL, BPL_STATUS_ERROR);
	tsNameLength = strlen((char *)japan->aucTsName);
	tsNameLength = (length <= tsNameLength ? length : tsNameLength);
	// TODO: spec에서 의도한 값이 맞는지 확인 필요
	HLIB_STD_StrUtf8NCpy(tsName, (char*)japan->aucTsName, DxJP_MAX_TS_NAME_LEN);
	APK_META_SVC_Delete(transponder);
	return BPL_STATUS_OK;
#else
	return BPL_STATUS_ERROR;
#endif
}


OPL_BOOL	OPL_Channel_IsSkipChannel (OPL_Channel_t channel)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t	*service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;
	HxLOG_Trace();
	RETURN_IF(service == NULL, OPL_FALSE);
	japan = &(service->ex.japan);
	RETURN_IF(japan == NULL, OPL_FALSE);
	return (japan->eUserFlag&eDxSvcUserFlag_22_SkipSvcByUser) ? OPL_TRUE : OPL_FALSE;
#else
	return OPL_FALSE;
#endif
}

void			OPL_Channel_SetSkipChannel (OPL_Channel_t channel, OPL_BOOL bSkip)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;
	HxLOG_Trace();
	RETURN_IF_VOID(service == NULL);
	japan = &(service->ex.japan);
	RETURN_IF_VOID(japan == NULL);

	if(bSkip == OPL_TRUE)
	{
		if(japan->eUserFlag & eDxSvcUserFlag_22_SkipSvcByUser)
			return;
		japan->eUserFlag = eDxSvcUserFlag_22_SkipSvcByUser;
	}
	else
	{
		if(!(japan->eUserFlag&eDxSvcUserFlag_22_SkipSvcByUser))
			return;
		japan->eUserFlag &= ~eDxSvcUserFlag_22_SkipSvcByUser;
	}
	//	printf("%s.userflag=0x%x  [%s] \n", service->name, service->userflag,__FUNCTION__);
	opl_channel_UpdateChannel(channel);
#endif
}

BPLStatus	OPL_Channel_GetLastUpdateDate(OPL_Channel_t channel, int *lastUpdateDate)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxTransponder_t *transponder = NULL;
	DxNetwork_t *network = NULL;
	DxJapanNet_t *japan = NULL;
	BPLStatus ret = BPL_STATUS_ERROR;
	HxLOG_Trace();
	RETURN_IF(service == NULL, BPL_STATUS_ERROR);
	transponder = APK_META_SVC_GetTransponder(service->tsuid);
	if(transponder == NULL)
	{
		goto funcExit;
	}

	network = APK_META_SVC_GetNetwork(transponder->netuid);
	if(network == NULL)
	{
		goto funcExit;
	}

	japan = &(network->ex.japan);
	if(japan == NULL)
	{
		goto funcExit;
	}

	*lastUpdateDate = japan->ulLastUpdateTime;
	ret = BPL_STATUS_OK;

funcExit:
	if(network != NULL)
	{
		APK_META_SVC_Delete(network);
	}
	if(transponder != NULL)
	{
		APK_META_SVC_Delete(transponder);
	}
	return ret;
#else
	return BPL_STATUS_ERROR;
#endif
}

char *		OPL_Channel_GetLogo (OPL_Channel_t channel, int width, int height)
{
	HCHAR	*value = NULL;
	char 	*empty = "";

#if defined(CONFIG_OP_JAPAN)
	HxFILE_t	file;
	static HUINT8		logoURI[64] = {0,};
	OPL_AribLogoType_e logoType;
	HCHAR		*logoTypeName[6] = {"HdLarge", "HdSmall", "Sd43Large", "Sd43Small", "Sd169Large", "Sd169Small"};
	DxService_t *service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;

	if(service == NULL)
		return empty;

	japan = &(service->ex.japan);
	if(japan == NULL)
		return empty;

	//printf("OPL_Channel_GetLogo: service=0x%x, lcn = %d, usLogoId=0x%x\n", service->svcid, service->lcn, japan->usLogoId);

	if(japan->usLogoId == 0xFFFF)
		return empty;

	logoType = opl_channel_GetLogoType(width, height);
	if(logoType == eOPL_AribLogo_Unkown)
		return empty;

	snprintf((HCHAR *)logoURI,64 , "%s%.4x/%.4x_%s.png",
		OPL_JAPAN_LOGO_ROOT_PATH, service->onid, japan->usLogoId, logoTypeName[logoType]);

	//printf("OPL_Channel_GetLogo: logoURI=%s\n", logoURI);

	file = HLIB_FILE_Open(logoURI, (const HUINT8 *)"rb");
	if (file != NULL)
	{
		value = (HCHAR *)logoURI;
		HLIB_FILE_Close(file);
	}
#endif

	if (value)
	{
		HxLOG_Print("%s \n",value);
		return value;
	}

	return empty;

}

OPL_BOOL	OPL_Channel_IsSameNetwork(OPL_Channel_t hChannel, OPL_Channel_t hCmpChannel)
{
	DxService_t *service  = hChannel;
	DxService_t *cmdService = hCmpChannel;
	RETURN_IF(service == NULL, FALSE);
	RETURN_IF(cmdService == NULL, FALSE);
#if defined(CONFIG_OP_JAPAN)
	if((OPL_MACRO_IS_JAPAN_DT_NETWORK(service->onid) && OPL_MACRO_IS_JAPAN_DT_NETWORK(cmdService->onid) )
	||(OPL_MACRO_IS_JAPAN_BS_NETWORK(service->onid) && OPL_MACRO_IS_JAPAN_BS_NETWORK(cmdService->onid) )
	||(OPL_MACRO_IS_JAPAN_CATV_NETWORK(service->onid) && OPL_MACRO_IS_JAPAN_CATV_NETWORK(cmdService->onid) )
	||(OPL_MACRO_IS_JAPAN_CS1_NETWORK(service->onid) && OPL_MACRO_IS_JAPAN_CS1_NETWORK(cmdService->onid) )
	||(OPL_MACRO_IS_JAPAN_CS2_NETWORK(service->onid) && OPL_MACRO_IS_JAPAN_CS2_NETWORK(cmdService->onid) )
	||(OPL_MACRO_IS_JAPAN_JCHITS_NETWORK(service->onid) && OPL_MACRO_IS_JAPAN_JCHITS_NETWORK(cmdService->onid) ))
	{
		return TRUE;
	}
#endif
	return FALSE;
}

BPLStatus OPL_Channel_GetProviderName(OPL_Channel_t channel, char *provName)
{
	DxService_t 	*service = (DxService_t *)channel;
	DxProvider_t	*provider = NULL;

	HxLOG_Trace();

	RETURN_IF(service == NULL, BPL_STATUS_ERROR);

	provider = APK_META_SVC_GetProvider( service->prvuid );

	RETURN_IF(provider == NULL, BPL_STATUS_ERROR);

	HLIB_STD_StrNCpySafe( provName, provider->name, DxNAME_LEN );

	APK_META_SVC_Delete(provider);
	return BPL_STATUS_OK;
}

int	OPL_Channel_GetFreeCAMode(OPL_Channel_t channel)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;

	if(service == NULL)
		return 0;

	japan = &(service->ex.japan);
	if(japan == NULL)
		return 0;

	return japan->ucFreeCAMode;
#else
	return 0;
#endif
}

OPL_BOOL OPL_Channel_GetExistingSdtCAD(OPL_Channel_t channel)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;

	if(service == NULL)
		return 0;

	japan = &(service->ex.japan);
	if(japan == NULL)
		return 0;

	return japan->ucIsExistingSdtCAD ? TRUE:FALSE;

#else
	return FALSE;
#endif
}

char * OPL_Channel_GetSubscriptionResponse(OPL_Channel_t channel)
{
#if defined(CONFIG_OP_JAPAN)
	DxService_t *service = (DxService_t *)channel;
	DxJapanService_t *japan = NULL;
	HCHAR	temp[8] = {0,};

	if(service == NULL)
		return 0;

	japan = &(service->ex.japan);
	if(japan == NULL)
		return 0;

	snprintf(temp, 8, "%04X", japan->usSubscriptionRes);
	//printf("OPL_Channel_GetSubscriptionResponse: service=0x%x, lcn = %d, usSubscriptionRes=%s\n", service->svcid, service->lcn, temp);
	return HLIB_STD_StrDup(temp);

#else
	return NULL;
#endif
}


void	OPL_Channel_SetResultCreateMaskListener (OPL_ResultCreateMaskListener_t listener)
{
#if defined(CONFIG_OP_JAPAN)
	onResultCreateMask = listener;
	APK_SQCSCENARIOJAPAN_RegisterNotifier(opl_channel_ResultCreateMaskListener);
#endif
}


#endif
