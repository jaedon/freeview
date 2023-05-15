/*
 * oapi_tyeps.c
 *
 *  Created on: 2012. 6. 7.
 */

#include "fsat_types.h"
#include "fsat_types_dec.c"
#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"FSATTYPES"

#define	__log		printf
#define	return_if_void(expr)	do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);goto pos;}}while(0)

#define	OTYPE_PRINT_INT(field)		fprintf(fp, "%30s: %d\n", #field, field)
#define	OTYPE_PRINT_HEX(field)		fprintf(fp, "%30s: 0x%X\n", #field, field)
#define OTYPE_PRINT_STR(field)		fprintf(fp, "%30s: %s\n", #field, field)
#define OTYPE_PRINT_BOOL(field)		fprintf(fp, "%30s: %s\n", #field, field ? "true" : "false")
#define OTYPE_PRINT_ENUM(field)		fprintf(fp, "%30s: enum %d\n", #field, field)
#define OTYPE_PRINT_INT_ARRAY(field, n)		\
	do {\
		HINT32	__i;\
		fprintf(fp, "%30s: [", #field);\
		for (__i = 0 ; __i < n ; __i++)\
			fprintf(fp, "%c%d", __i == 0 ? ' ' : ',', field[__i]);\
		fprintf(fp, "]\n");\
	} while (0)

#define OTYPE_PRINT_HEX_ARRAY(field, n)		\
	do {\
		HINT32	__i;\
		fprintf(fp, "%30s: [", #field);\
		for (__i = 0 ; __i < n ; __i++)\
			fprintf(fp, "%c%X", __i == 0 ? ' ' : ',', field[__i]);\
		fprintf(fp, "]\n");\
	} while (0)

#define OTYPE_PRINT_BYTE_DUMP(field, size) 	\
	do {\
		const HUINT8 *b = (const HUINT8 *)&(field);\
		HUINT32	s = size;\
		fprintf(fp, "%30s: ", #field);\
		while (s--) fprintf(fp, "%02X", *b++);\
		fprintf(fp, "\n");\
	} while (0)


#define DXCRCSTRMAX 1024
#define  	DXCRCSUM(s,m)     \
    if(HxSTD_StrLen(CRC)  +HxSTD_StrLen(#m)  < DXCRCSTRMAX){ \
        HxSTD_StrNCat(CRC,#m,12);   \
        HLIB_STD_FmtString(temp,12,"%d",offsetof(s,m)); \
        HxSTD_StrNCat(CRC,temp,12);}

#define  	DXCRCSUM_T(s)     \
        if(HxSTD_StrLen(CRC) +sizeof(s)  < DXCRCSTRMAX){ \
            HLIB_STD_FmtString(temp,12,"%d",sizeof(s)); \
            HxSTD_StrNCat(CRC,temp,12);}


#if defined(CONFIG_JSONBIN)

#define _START_CRC________________________
static HUINT32 local_ServiceGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
     HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(fsatDxService_t,uid);
        DXCRCSUM(fsatDxService_t,version);
        DXCRCSUM(fsatDxService_t,tsuid);
        DXCRCSUM(fsatDxService_t,prvuid);
        DXCRCSUM(fsatDxService_t,grpuids);
        DXCRCSUM(fsatDxService_t,bqtuids);

        DXCRCSUM(fsatDxService_t,svcid);
        DXCRCSUM(fsatDxService_t,tsid);
        DXCRCSUM(fsatDxService_t,onid);

        DXCRCSUM(fsatDxService_t,lcn);
        DXCRCSUM(fsatDxService_t,name);
        DXCRCSUM(fsatDxService_t,svcType);
        DXCRCSUM(fsatDxService_t,orgSvcType);
        DXCRCSUM(fsatDxService_t,optype);
        DXCRCSUM(fsatDxService_t,deliType);
        DXCRCSUM(fsatDxService_t,satType);
        DXCRCSUM(fsatDxService_t,visibleFlag);
        DXCRCSUM(fsatDxService_t,locked);
        DXCRCSUM(fsatDxService_t,removed);
        DXCRCSUM(fsatDxService_t,pmtPid);
        DXCRCSUM(fsatDxService_t,pcrPid);
        DXCRCSUM(fsatDxService_t,videoPid);
        DXCRCSUM(fsatDxService_t,audioPid);
        DXCRCSUM(fsatDxService_t,ttxPid);
        DXCRCSUM(fsatDxService_t,audioAuxPid);
        DXCRCSUM(fsatDxService_t,dolbyPid);

        DXCRCSUM(fsatDxService_t,removed);
        DXCRCSUM(fsatDxService_t,pmtPid);
        DXCRCSUM(fsatDxService_t,highDefinition);
        DXCRCSUM(fsatDxService_t,casType);
        DXCRCSUM(fsatDxService_t,dolbyFlag);
        DXCRCSUM(fsatDxService_t,videoCodec);

        DXCRCSUM(fsatDxService_t,audioCodec);
        DXCRCSUM(fsatDxService_t,audioAuxCodec);
        DXCRCSUM(fsatDxService_t,dolbyCodec);
        DXCRCSUM(fsatDxService_t,lcnFlag);
        DXCRCSUM(fsatDxService_t,audioConfig);
        DXCRCSUM(fsatDxService_t,updateSvcFlag);
        DXCRCSUM(fsatDxService_t,ex);
#ifdef	CONFIG_OP_FREESAT
        DXCRCSUM(fsatDxService_t,ex.freesat);
        DXCRCSUM(fsatDxService_t,ex.freesat.svcId);
        DXCRCSUM(fsatDxService_t,ex.freesat.regionId);
        DXCRCSUM(fsatDxService_t,ex.freesat.defaultAuth);
        DXCRCSUM(fsatDxService_t,ex.freesat.contentMgr);
        DXCRCSUM(fsatDxService_t,ex.freesat.szShortName);
        DXCRCSUM(fsatDxService_t,ex.freesat.ucUserCustomizable);
        DXCRCSUM(fsatDxService_t,ex.freesat.ucUserSelectable);
        DXCRCSUM(fsatDxService_t,ex.freesat.ucAdultSvc);
        DXCRCSUM(fsatDxService_t,ex.freesat.ucVideoCodecH264);
        DXCRCSUM(fsatDxService_t,ex.freesat.ucGuidanceType);
        DXCRCSUM(fsatDxService_t,ex.freesat.ucGuidanceMode);
        DXCRCSUM(fsatDxService_t,ex.freesat.szGuidanceStr);
#endif
        DXCRCSUM_T(fsatDxService_t);
        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}

static HUINT32 local_EventGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
     HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(fsatDxEvent_t,refCount);
        DXCRCSUM(fsatDxEvent_t,svcuid);
        DXCRCSUM(fsatDxEvent_t,eventId);
        DXCRCSUM(fsatDxEvent_t,startTime);
        DXCRCSUM(fsatDxEvent_t,freeCaMode);

        DXCRCSUM(fsatDxEvent_t,name);
        DXCRCSUM(fsatDxEvent_t,text);
        DXCRCSUM(fsatDxEvent_t,parentalRating);
        DXCRCSUM(fsatDxEvent_t,content);
        DXCRCSUM(fsatDxEvent_t,longText);
        DXCRCSUM(fsatDxEvent_t,item);
        DXCRCSUM(fsatDxEvent_t,linkage);
        DXCRCSUM(fsatDxEvent_t,component);
        DXCRCSUM(fsatDxEvent_t,crid);
        DXCRCSUM(fsatDxEvent_t,descriptor);
        DXCRCSUM(fsatDxEvent_t,extension);

        DXCRCSUM(fsatDxEvent_t,pstCollector);
        DXCRCSUM_T(fsatDxEvent_t);
        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}

static HUINT32 local_RECGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
    HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(fsatDxRecListData_t,ulContentId);
        DXCRCSUM(fsatDxRecListData_t,szUrl);
        DXCRCSUM(fsatDxRecListData_t,szName);
        DXCRCSUM(fsatDxRecListData_t,szThumbnailPath);
        DXCRCSUM(fsatDxRecListData_t,ulRecStartTime);
        DXCRCSUM(fsatDxRecListData_t,ulRecDuration);
        DXCRCSUM(fsatDxRecListData_t,ulRecOrgDuration);
        DXCRCSUM(fsatDxRecListData_t,eRecStatus);
        DXCRCSUM(fsatDxRecListData_t,eRecEndReason);
        DXCRCSUM(fsatDxRecListData_t,bScrambled);
        DXCRCSUM(fsatDxRecListData_t,bEncrypted);
        DXCRCSUM(fsatDxRecListData_t,bEventBased);
        DXCRCSUM(fsatDxRecListData_t,usEventId);
        DXCRCSUM(fsatDxRecListData_t,bSeriesRec);
        DXCRCSUM(fsatDxRecListData_t,ulLastViewTime);
        DXCRCSUM(fsatDxRecListData_t,bUserLock);
        DXCRCSUM(fsatDxRecListData_t,bWillBeDeleted);
        DXCRCSUM(fsatDxRecListData_t,bAudoDeleted);
        DXCRCSUM(fsatDxRecListData_t,bPlayed);
        DXCRCSUM(fsatDxRecListData_t,bBlocked);
        DXCRCSUM(fsatDxRecListData_t,usNumOfBookmark);
        DXCRCSUM(fsatDxRecListData_t,szSvcName);
        DXCRCSUM(fsatDxRecListData_t,nSvcNo);
        DXCRCSUM(fsatDxRecListData_t,svcUId);

        DXCRCSUM(fsatDxRecListData_t,eSvcType);
        DXCRCSUM(fsatDxRecListData_t,eVideoType);
        DXCRCSUM(fsatDxRecListData_t,eCasType);

        DXCRCSUM(fsatDxRecListData_t,stEvt);
        DXCRCSUM(fsatDxRecListData_t,ulExtEvtCnt);

        DXCRCSUM(fsatDxRecListData_t,astBookmarks);
        DXCRCSUM(fsatDxRecListData_t,pstExtEvts);
        DXCRCSUM(fsatDxRecListData_t,aucExtText);
#ifdef	CONFIG_OP_FREESAT
        DXCRCSUM(fsatDxRecListData_t,extension.freesat);
        DXCRCSUM(fsatDxRecListData_t,extension.freesat.stEvt);
#endif
        DXCRCSUM_T(fsatDxRecListData_t);
        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}



static HUINT32 local_ScheduleGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
    HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(DxSCHEDULE_t,uid);
        DXCRCSUM(DxSCHEDULE_t,schedId);
        DXCRCSUM(DxSCHEDULE_t,eventBase);
        DXCRCSUM(DxSCHEDULE_t,eventId);
        DXCRCSUM(DxSCHEDULE_t,startTime);
        DXCRCSUM(DxSCHEDULE_t,duration);
        DXCRCSUM(DxSCHEDULE_t,svcUId);
        DXCRCSUM(DxSCHEDULE_t,preStart);

        DXCRCSUM(DxSCHEDULE_t,action);
        DXCRCSUM(DxSCHEDULE_t,repeat);

        DXCRCSUM(DxSCHEDULE_t,param);
        DXCRCSUM(DxSCHEDULE_t,name);
        DXCRCSUM(DxSCHEDULE_t,description);
        DXCRCSUM(DxSCHEDULE_t,longDescription);

        DXCRCSUM_T(DxSCHEDULE_t);
        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}

static HUINT32 local_TransponderGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
    HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(fsatDxTransponder_t,uid);

        DXCRCSUM(fsatDxTransponder_t,version);
        DXCRCSUM(fsatDxTransponder_t,netuid);
        DXCRCSUM(fsatDxTransponder_t,tsid);
        DXCRCSUM(fsatDxTransponder_t,onid);
        DXCRCSUM(fsatDxTransponder_t,tunerid);


        DXCRCSUM(fsatDxTransponder_t,deliType);
        DXCRCSUM(fsatDxTransponder_t,tuningParam);
#ifdef	CONFIG_OP_FREESAT
        DXCRCSUM(fsatDxTransponder_t,ex.freesat);
        DXCRCSUM(fsatDxTransponder_t,ex.freesat.defaultAuth);
#endif
        DXCRCSUM_T(fsatDxTransponder_t);
        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}





static HUINT32 local_ProviderGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
    HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(fsatDxProvider_t,uid);

        DXCRCSUM(fsatDxProvider_t,version);
        DXCRCSUM(fsatDxProvider_t,name);
        DXCRCSUM_T(fsatDxProvider_t);
        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}


static HUINT32 local_GroupGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
    HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(fsatDxGroup_t,uid);

        DXCRCSUM(fsatDxGroup_t,version);
        DXCRCSUM(fsatDxGroup_t,id);

        DXCRCSUM(fsatDxGroup_t,name);
        DXCRCSUM(fsatDxGroup_t,id);


#ifdef	CONFIG_OP_FREESAT
        DXCRCSUM(fsatDxGroup_t,ex.freesat);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.ucGroupType);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.ucNondestructiveTuneFlag);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.ucReturnChannelAccessFlag);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.szIso639LangCode);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.ucG2ExtensionFlag);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.ucDefaultVisableFlag);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.ucAdultChannelsGroupFlag);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.ucIpChannelsGroupFlag);
        DXCRCSUM(fsatDxGroup_t,ex.freesat.ucCaChannelsGroupFlag);

#endif
        DXCRCSUM_T(fsatDxGroup_t);
        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}


static HUINT32 local_BouquetGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
    HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(fsatDxBouquet_t,uid);

        DXCRCSUM(fsatDxBouquet_t,version);
        DXCRCSUM(fsatDxBouquet_t,id);
        DXCRCSUM(fsatDxBouquet_t,name);
        DXCRCSUM_T(fsatDxBouquet_t);
        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}




static HUINT32 local_NetworkGetCRC(void)
{
    HUINT8  CRC[DXCRCSTRMAX] ="";
    HUINT8 temp[10];
    static HUINT32 crcvalue = 0;

    if(crcvalue ==0)
    {
        DXCRCSUM(fsatDxNetwork_t,uid);

        DXCRCSUM(fsatDxNetwork_t,version);
        DXCRCSUM(fsatDxNetwork_t,onid);
        DXCRCSUM(fsatDxNetwork_t,name);
        DXCRCSUM(fsatDxNetwork_t,deliType);

#ifdef	CONFIG_OP_FREESAT
        DXCRCSUM(fsatDxNetwork_t,ex.freesat);
        DXCRCSUM(fsatDxNetwork_t,ex.freesat.defaultAuth);
#endif
        DXCRCSUM_T(fsatDxNetwork_t);

        crcvalue =HLIB_MATH_GetCrc32(CRC,HxSTD_StrLen(CRC));
        HxLOG_Print("[%s,%d]CRC[%x]\n",__FUNCTION__,__LINE__,crcvalue);

    }
    return crcvalue;
}

#define _END__CRC________________________


//1 fsatDxEvent_t
static void *	local_event_FreeItem (void *item)
{
	if (item)
		HLIB_STD_MemFree(item);
	return NULL;
}

static void 	local_event_DecodeEx (HUINT8 *collector, fsatDxEvent_t *data, const HCHAR *key, const void *bin, HUINT32 size, void *userdata)
{
	const HUINT8	*byte = (const HUINT8 *)bin;
	HUINT32	i, n;
	HUINT32	ulCollectorAddr = (HUINT32)collector;

#define	__DECODE_ARRAY(type,vector,func)	\
	do {\
		type	*item;								\
		return_if_void (size % sizeof(type) != 0);	\
		n = size / sizeof(type);					\
		vector = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))local_event_FreeItem, NULL);	\
		return_if_void (vector == NULL);			\
		for (i = 0 ; i < n ; i++, byte += sizeof(type)) {		\
			item = (type *)HLIB_STD_MemDup(byte, sizeof(type));	\
			func;												\
			if (item) HLIB_VECTOR_Add(vector, item);				\
		}														\
		if (HLIB_VECTOR_Length(vector) == 0) {			\
			HLIB_VECTOR_Delete(vector);				\
			vector = NULL;							\
		}											\
	} while(0)

	if (strcmp(key, "component") == 0)
	{
		__DECODE_ARRAY(fsatDxEventComponent_t, data->component, {
			item->text = (HCHAR*)(ulCollectorAddr + (HUINT32)item->text);
		});
	}
	else if (strcmp(key, "crid") == 0)
	{
		__DECODE_ARRAY(fsatDxEventCRID_t, data->crid, {
			item->crid = (HCHAR*)(ulCollectorAddr + (HUINT32)item->crid);
		});
	}
	else if (strcmp(key, "descriptor") == 0)
	{
		HUINT8 *desc;
		HINT32	len = (HINT32)size;

		data->descriptor = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))local_event_FreeItem, NULL);
		return_if_void (data->descriptor  == NULL);

		for (; byte[0] != 0xFF && len > 0 ; byte += n, len -= (HINT32)n)
		{
			n    = byte[1] + 2;
			desc = (HUINT8 *)HLIB_STD_MemDup(byte, n);
			if (desc)
			{
				HLIB_VECTOR_Add(data->descriptor, desc);
			}
		}
		if (HLIB_VECTOR_Length(data->descriptor) == 0)
		{
			HLIB_VECTOR_Delete(data->descriptor);
			data->descriptor = NULL;
		}
	}
	else if (strcmp(key, "linkage") == 0)
	{
		__DECODE_ARRAY(fsatDxEventLinkage_t, data->linkage, {
			item->privateData = (HUINT8*)(ulCollectorAddr + (HUINT32)item->privateData);
		});
	}
	else if (strcmp(key, "longtext") == 0)
	{
		data->longText = HLIB_STD_StrDup(bin);
	}
#ifdef	CONFIG_OP_FREESAT
	else if (strcmp(key, "fsatextdes") == 0)
	{
		HUINT8 *desc;
		HINT32	len = (HINT32)size;

		data->extension.fsateventinfo.ext_descriptor = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))local_event_FreeItem, NULL);
		return_if_void (data->extension.fsateventinfo.ext_descriptor  == NULL);

		for (; byte[0] != 0xFF && len > 0 ; byte += n, len -= (HINT32)n)
		{
			n    = byte[1] + 2;
			desc = (HUINT8 *)HLIB_STD_MemDup(byte, n);
			if (desc)
			{
				HLIB_VECTOR_Add(data->extension.fsateventinfo.ext_descriptor, desc);
			}
		}
		if (HLIB_VECTOR_Length(data->extension.fsateventinfo.ext_descriptor) == 0)
		{
			HLIB_VECTOR_Delete(data->extension.fsateventinfo.ext_descriptor);
			data->extension.fsateventinfo.ext_descriptor = NULL;
		}
	}
	else if (strcmp(key, "medialocator") == 0)
	{
		__DECODE_ARRAY(fsatDxEventMediaLocatorInfo_t, data->extension.fsateventinfo.fsatMediaLocatorInfo, {
			item->URI_char = (HCHAR*)(ulCollectorAddr + (HUINT32)item->URI_char);
		});
	}


#endif
	else if (strcmp(key, "items") == 0)
	{
		__DECODE_ARRAY(fsatDxEventItem_t, data->item, {
			item->item = (HCHAR*)((HUINT32)item->item + ulCollectorAddr);
			item->description = (HCHAR*)((HUINT32)item->description + ulCollectorAddr);
		});
	}
	else if (strcmp(key, "collector") == 0)
	{
	}
	else
	{
		HxLOG_Critical("%s(Unknown key:%s)\n", __FUNCTION__, key);
	}
#undef	__DECODE_ARRAY
}

static void	local_event_DecodeBase(HUINT8 *collector, HUINT32 ulCollectorSize, fsatDxEvent_t *data)
{
	data->pstCollector = (fsatDxDataCollector_t*)HLIB_STD_MemCalloc(sizeof(fsatDxDataCollector_t));
	data->pstCollector->refCount = 1;
	data->pstCollector->size = ulCollectorSize;
	data->pstCollector->pucCollector = collector;

	data->name = (HUINT32)collector + data->name;
	data->text = (HUINT32)collector + data->text;

#ifdef	CONFIG_OP_FREESAT
	data->extension.fsateventinfo.fsatGuidance.guidancedata =
		(HUINT32)collector + data->extension.fsateventinfo.fsatGuidance.guidancedata;
	data->extension.fsateventinfo.fsatShortEventPromo.szShortEventPromo =
		(HUINT32)collector + data->extension.fsateventinfo.fsatShortEventPromo.szShortEventPromo;
#endif
}


#define DX_BOOKMARK_KEY		"bookmark"
#define DX_BOOKMARK_KEY_LEN	8

#define DX_EXT_TXT_KEY		"extTxt"
#define DX_EXT_TXT_KEY_LEN	6

#define DX_EXT_EVT_KEY		"extEvt"
#define DX_EXT_EVT_KEY_LEN	6


#define DX_EXT_ITEM_KEY		"extItem"
#define DX_EXT_ITEM_KEY_LEN	7


static void	local_RecData_DecodeEx(void *collector, void *data, const HCHAR *key, const void *bin, HUINT32 size, void *userdata)
{
	fsatDxRecListData_t	*recData= (fsatDxRecListData_t*)data;
	HINT32			nKeyLen = 0;

	if (NULL == key)
	{
		HxLOG_Assert(key);
		return;
	}
	nKeyLen = HxSTD_StrLen(key);

	if (NULL == recData->pstExtEvts && recData->ulExtEvtCnt)
	{	// not allocated -> alloc !
		recData->pstExtEvts = (fsatDxRecList_ExtEvt_t*)HLIB_STD_MemCalloc(sizeof(fsatDxRecList_ExtEvt_t) * recData->ulExtEvtCnt);
	}

	if (0 == HxSTD_StrNCmp(key, DX_BOOKMARK_KEY, DX_BOOKMARK_KEY_LEN) && DX_BOOKMARK_KEY_LEN + 2 <= nKeyLen)
	{
		HUINT32	firstDecimal = key[DX_BOOKMARK_KEY_LEN] - '0', secondDecimal = key[DX_BOOKMARK_KEY_LEN+1] - '0';
		HUINT32	bookmarkIdx = (firstDecimal*10) + secondDecimal;
		fsatDxRecList_Bookmark_t	*pstBookmark = NULL;

		if (fsatDxRECLIST_NUM_BOOKMARK <= bookmarkIdx)
		{	// invalid case
			HxLOG_Assert(bookmarkIdx < fsatDxRECLIST_NUM_BOOKMARK);
			return;
		}
		pstBookmark = &recData->astBookmarks[bookmarkIdx];
		HxSTD_MemCopy(pstBookmark, (fsatDxRecList_Bookmark_t*)bin, size);
	}
	else if (0 == HxSTD_StrCmp(key, DX_EXT_TXT_KEY))
	{
		HxSTD_MemCopy(recData->aucExtText, (HCHAR*)bin, size);
		HxLOG_Print("[%s:%d] extText=(%s)\n", __FUNCTION__, __LINE__, recData->aucExtText);
	}
	else if (0 == HxSTD_StrNCmp(key, DX_EXT_EVT_KEY, DX_EXT_EVT_KEY_LEN) && DX_EXT_EVT_KEY_LEN + 2 <= nKeyLen)
	{
		HUINT32	firstDecimal = key[DX_EXT_EVT_KEY_LEN] - '0', secondDecimal = key[DX_EXT_EVT_KEY_LEN+1] - '0';
		HUINT32	extEvtIdx = (firstDecimal*10) + secondDecimal;
		fsatDxRecList_ExtEvt_t	*pstExtEvt = NULL;

		if (recData->ulExtEvtCnt <= extEvtIdx)
		{	// invalid case
			HxLOG_Assert(extEvtIdx < recData->ulExtEvtCnt);
			return;
		}

		if (NULL == recData->pstExtEvts)
		{
			HxLOG_Assert(recData->pstExtEvts);
			return;
		}

		pstExtEvt = &recData->pstExtEvts[extEvtIdx];
		HxLOG_Assert(pstExtEvt);
		HxSTD_MemCopy(pstExtEvt, (fsatDxRecList_ExtEvt_t*)bin, size);
	}
	else if (0 == HxSTD_StrNCmp(key, DX_EXT_ITEM_KEY, DX_EXT_ITEM_KEY_LEN) && DX_EXT_ITEM_KEY_LEN + 4 <= nKeyLen)
	{
		HUINT32	firstDecimal = key[DX_EXT_ITEM_KEY_LEN] - '0', secondDecimal = key[DX_EXT_ITEM_KEY_LEN+1] - '0';
		HUINT32	extEvtIdx	 = (firstDecimal*10) + secondDecimal;

		HUINT32	itemFirstDecimal = key[DX_EXT_ITEM_KEY_LEN+2] - '0', itemSecondDecimal = key[DX_EXT_ITEM_KEY_LEN+3] - '0';
		HUINT32	extEvtItemIdx = (itemFirstDecimal*10) + itemSecondDecimal;

		fsatDxRecList_ExtEvt_t		*pstExtEvt 		= NULL;
		fsatDxRecList_Ext_ItemData_t*pstExtEvtItem 	= NULL;

		if (recData->ulExtEvtCnt <= extEvtIdx)
		{	// invalid case
			HxLOG_Assert(extEvtIdx < recData->ulExtEvtCnt);
			return;
		}

		if (NULL == recData->pstExtEvts)
		{
			HxLOG_Assert(recData->pstExtEvts);
			return;
		}

		pstExtEvt = &recData->pstExtEvts[extEvtIdx];
		HxLOG_Assert(pstExtEvt);

		if (fsatDxPVR_EXT_ITEM_MAX <= extEvtItemIdx)
		{	// invalid case
			HxLOG_Assert(extEvtItemIdx < fsatDxPVR_EXT_ITEM_MAX);
			return;
		}
		pstExtEvtItem = &pstExtEvt->astItem[extEvtItemIdx];
		HxLOG_Assert(pstExtEvtItem);

		HxSTD_MemCopy(pstExtEvtItem, (fsatDxRecList_Ext_ItemData_t*)bin, size);

		HxLOG_Print("[%s:%d] item (%d)(%d) itemIdx - desc:(%s) item:(%s)\n"
            , __FUNCTION__, __LINE__, extEvtIdx, extEvtItemIdx
			, pstExtEvtItem->aucDescription, pstExtEvtItem->aucItem);
	}

#ifdef	CONFIG_OP_FREESAT
	else if (0 == HxSTD_StrCmp(key, "freesat"))
	{
		HxSTD_MemCopy(&recData->extension.freesat, (fsatDxFreesatRecListData_t*)bin, size);
	}
#else	// other op ?
#endif // #ifdef	CONFIG_OP_FREESAT

}
fsatDxEvent_t *	fsatDxEvent_Decode (HxJSONBIN_t decoder, fsatDxEvent_t *type, HxJSON_t json)
{
	type = (fsatDxEvent_t *)HLIB_JSONBIN_DECODER_Decode(
								  decoder
								, type
								, sizeof(fsatDxEvent_t)
								, json
								, (HxJSONBIN_DecodeEx_t)local_event_DecodeEx
								, (HxJSONBIN_DecodeBase_t)local_event_DecodeBase
								, NULL
							);


	if (type)
	{
		type->refCount = 1;
	}
	return type;
}


fsatDxRecListData_t *	fsatDxRecData_DecodeBin (fsatDxRecListData_t *type, HxJSON_t json)
{
	HxJSONBIN_t	dec = HLIB_JSONBIN_DECODER_Open();
	fsatDxRecListData_t	*decodedRecData;

	decodedRecData = (fsatDxRecListData_t *)HLIB_JSONBIN_DECODER_Decode(dec, type, sizeof(fsatDxRecListData_t), json, local_RecData_DecodeEx, NULL, NULL);
	HLIB_JSONBIN_DECODER_Close(dec);
    HxLOG_Print("[%s:%d]saved crc[%x]CRC[%x]\n", __FUNCTION__, __LINE__, decodedRecData->__CRC__, local_RECGetCRC() );

    if(decodedRecData->__CRC__ == local_RECGetCRC()) return decodedRecData;
    HxLOG_Critical("[%s:%d]CRC is a mismatch [%x][%x]\n", __FUNCTION__, __LINE__, decodedRecData->__CRC__, local_RECGetCRC() );
    return NULL;
}



const HCHAR *		fsatDxRecData_EncodeBin (HxJSONBIN_t enc, const fsatDxRecListData_t *recData)
{
	HINT32	i, k;
	HUINT32	ulRecDataBaseSize		= sizeof(fsatDxRecListBasic_t);
	HUINT32	ulExtEvtItemSize		= sizeof(fsatDxRecList_Ext_ItemData_t);
	HUINT32	ulExtEvtSizeExceptItem	= sizeof(fsatDxRecList_ExtEvt_t) - (ulExtEvtItemSize * fsatDxPVR_EXT_ITEM_MAX);
	HUINT32	ulBookmarkSize			= sizeof(fsatDxRecList_Bookmark_t);
	HCHAR	szKey[64+1] = {0};
    fsatDxRecListData_t data;

    HxSTD_MemCopy(&data,recData,sizeof(fsatDxRecListData_t));
    data.__CRC__= local_RECGetCRC();
    HxLOG_Print("[%s:%d]CRC[%x]\n",__FUNCTION__,__LINE__,data.__CRC__ );

	HLIB_JSONBIN_ENCODER_Clear(enc);
	return_if (!HLIB_JSONBIN_ENCODER_SetBinary(enc, &data, ulRecDataBaseSize), NULL);

	for (i = 0; i < recData->usNumOfBookmark; ++i)
	{	// max 32
		fsatDxRecList_Bookmark_t	*pstBookmark = (fsatDxRecList_Bookmark_t *)&recData->astBookmarks[i];
		HUINT32	firstDecimal = i/10, secondDecimal = i%10;

		HxSTD_PrintToStrN(szKey, 64, "%s%d%d", DX_BOOKMARK_KEY, firstDecimal, secondDecimal);
		HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, szKey, pstBookmark, ulBookmarkSize);
	}

	if (recData->ulExtEvtCnt)
	{
		HUINT32	ulExtTxtLen = HxSTD_StrLen(recData->aucExtText);

		HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, DX_EXT_TXT_KEY, recData->aucExtText, ulExtTxtLen);
	}

	for (i = 0; i < recData->ulExtEvtCnt; ++i)
 	{	// max 16
		fsatDxRecList_ExtEvt_t	*pstExtEvt = &recData->pstExtEvts[i];
		HUINT32	firstDecimal = i/10, secondDecimal = i%10;

		HxSTD_PrintToStrN(szKey, 64, "%s%d%d", DX_EXT_EVT_KEY, firstDecimal, secondDecimal);

		HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, szKey, pstExtEvt, ulExtEvtSizeExceptItem);
		for (k = 0; k < pstExtEvt->ucItemNum; ++k)
	 	{	// max 16
		 	HUINT32	itemFirstDecimal = k/10, itemSecondDecimal = k%10;

			HxSTD_PrintToStrN(szKey, 64, "%s%d%d%d%d", DX_EXT_ITEM_KEY, firstDecimal, secondDecimal, itemFirstDecimal, itemSecondDecimal);
			HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, szKey, &pstExtEvt->astItem[k], ulExtEvtItemSize);
		}
	}

#ifdef	CONFIG_OP_FREESAT
	{
		HUINT32	ulFreeSatDataSize = sizeof(fsatDxFreesatRecListData_t);

		HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, "freesat", &recData->extension.freesat, ulFreeSatDataSize);
	}
#else	// other op ?
#endif // #ifdef	CONFIG_OP_FREESAT
	return HLIB_JSONBIN_ENCODER_Encode(enc);
}


static void local_shcedule_DecodeEx(void *collector, void *data, const HCHAR *key, const void *bin, HUINT32 size, void *userdata)
{
	DxSCHEDULE_t	*schedule	= (DxSCHEDULE_t*)data;

	if (strcmp(key, "name") == 0 && 0 != size)
	{
		schedule->name = HLIB_STD_StrDup(bin);
	}
	else if (strcmp(key, "description") == 0 && 0 != size)
	{
		schedule->description = HLIB_STD_StrDup(bin);
	}
	else if (strcmp(key, "longDescription") == 0 && 0 != size)
	{
		schedule->longDescription = HLIB_STD_StrDup(bin);
	}
	else if (strcmp(key, "storageID") == 0 && 0 != size)
	{
		schedule->storageID = HLIB_STD_StrDup(bin);
	}
}


DxSCHEDULE_t *	fsatDxSchedule_DecodeBin (DxSCHEDULE_t *shcedule, HxJSON_t json)
{
	HxJSONBIN_t	dec = HLIB_JSONBIN_DECODER_Open();
	DxSCHEDULE_t	*decodedData;

	decodedData = (DxSCHEDULE_t *)HLIB_JSONBIN_DECODER_Decode(dec, shcedule, sizeof(DxSCHEDULE_t), json, local_shcedule_DecodeEx, NULL, NULL);
	HLIB_JSONBIN_DECODER_Close(dec);

    HxLOG_Print("[%s:%d]saved crc[%x]CRC[%x]\n",__FUNCTION__,__LINE__,decodedData->__CRC__,local_ScheduleGetCRC() );
    if(decodedData->__CRC__  == local_ScheduleGetCRC())
        return decodedData;
    HxLOG_Critical("[%s:%d]CRC is not match [%x][%x]\n",__FUNCTION__,__LINE__,decodedData->__CRC__,local_ScheduleGetCRC() );
    return NULL;
}

const HCHAR *		fsatDxSchedule_EncodeBin (HxJSONBIN_t enc, const DxSCHEDULE_t *shcedule)
{
    DxSCHEDULE_t data;
    HxSTD_MemCopy(&data,shcedule,sizeof(DxSCHEDULE_t));
    data.__CRC__= local_ScheduleGetCRC();
    HxLOG_Print("[%s:%d]CRC[%x]\n",__FUNCTION__,__LINE__,data.__CRC__ );

	// just encode whole & encode the pointers -> except pointers (due to pointer clearance)
	HLIB_JSONBIN_ENCODER_Clear(enc);

	return_if (!HLIB_JSONBIN_ENCODER_SetBinary(enc, &data, sizeof(DxSCHEDULE_ExcepPointers_t)), NULL);

	if (shcedule->name)
	{
		HUINT32	ulLen = HxSTD_StrLen(shcedule->name);
		if (ulLen)
			HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, "name", shcedule->name, ulLen);
	}

	if (shcedule->description)
	{
		HUINT32	ulLen = HxSTD_StrLen(shcedule->description);
		if (ulLen)
			HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, "description", shcedule->description, ulLen);
	}

	if (shcedule->longDescription)
	{
		HUINT32	ulLen = HxSTD_StrLen(shcedule->longDescription);
		if (ulLen)
			HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, "longDescription", shcedule->longDescription, ulLen);
	}

	if (shcedule->storageID)
	{
		HUINT32	ulLen = HxSTD_StrLen(shcedule->storageID);
		if (ulLen)
			HLIB_JSONBIN_ENCODER_SetBinaryEx(enc, "storageID", shcedule->storageID, ulLen);
	}

	return HLIB_JSONBIN_ENCODER_Encode(enc);
}



fsatDxService_t *	fsatDxService_DecodeBin (fsatDxService_t *type, HxJSON_t json)
{
    fsatDxService_t *service;

    service =  (fsatDxService_t *)HLIB_JSONBIN_DECODER_Decode(NULL, type, sizeof(fsatDxService_t), json, NULL, NULL, NULL);
    HxLOG_Print("[%s:%d]saved crc[%x]CRC[%x]\n",__FUNCTION__,__LINE__,service->__CRC__,local_ServiceGetCRC() );
    if(service->__CRC__ == local_ServiceGetCRC())
        return service;

    HxLOG_Critical("[%s:%d]CRC is a mismatch [%x][%x]\n",__FUNCTION__,__LINE__,service->__CRC__,local_RECGetCRC() );
    return NULL;
}

const HCHAR *		fsatDxService_EncodeBin (HxJSONBIN_t enc, const fsatDxService_t *type)
{
    fsatDxService_t data;

    HxSTD_MemCopy(&data,type,sizeof(fsatDxService_t));
    data.__CRC__ = local_ServiceGetCRC();
    HxLOG_Print("[%s:%d]CRC[%x]\n",__FUNCTION__,__LINE__,data.__CRC__ );
	HLIB_JSONBIN_ENCODER_Clear(enc);
	return_if (!HLIB_JSONBIN_ENCODER_SetBinary(enc, &data, sizeof(fsatDxService_t)), NULL);

    return HLIB_JSONBIN_ENCODER_Encode(enc);
}



void				fsatDxService_Print (FILE *fp, const fsatDxService_t *t)
{
	fprintf(fp, "SERVICE[%d] %d.%s\n"
		, t->uid
		, t->lcn
		, t->name
	);
	OTYPE_PRINT_INT(t->version);
	OTYPE_PRINT_INT(t->tsuid);
	OTYPE_PRINT_INT(t->prvuid);
	OTYPE_PRINT_INT(t->antuid);
	OTYPE_PRINT_INT_ARRAY(t->grpuids, fsatDxGROUP_LEN);
	OTYPE_PRINT_INT_ARRAY(t->bqtuids, fsatDxBOUQUET_LEN);
	OTYPE_PRINT_HEX(t->svcid);
	OTYPE_PRINT_HEX(t->tsid);
	OTYPE_PRINT_HEX(t->onid);
	OTYPE_PRINT_ENUM(t->svcType);
	OTYPE_PRINT_ENUM(t->orgSvcType);
	OTYPE_PRINT_INT(t->optype);
	OTYPE_PRINT_ENUM(t->deliType);
	OTYPE_PRINT_STR(t->satType);
	OTYPE_PRINT_BOOL(t->visibleFlag);
	OTYPE_PRINT_BOOL(t->locked);
	OTYPE_PRINT_BOOL(t->removed);
	OTYPE_PRINT_HEX(t->pmtPid);
	OTYPE_PRINT_HEX(t->pcrPid);
	OTYPE_PRINT_HEX(t->videoPid);
	OTYPE_PRINT_HEX(t->audioPid);
	OTYPE_PRINT_HEX(t->ttxPid);
	OTYPE_PRINT_HEX(t->audioAuxPid);
	OTYPE_PRINT_HEX(t->dolbyPid);
	OTYPE_PRINT_ENUM(t->highDefinition);
	OTYPE_PRINT_ENUM(t->casType);
	OTYPE_PRINT_BOOL(t->dolbyFlag);
	OTYPE_PRINT_ENUM(t->videoCodec);
	OTYPE_PRINT_ENUM(t->audioCodec);
	OTYPE_PRINT_ENUM(t->audioAuxCodec);
	OTYPE_PRINT_ENUM(t->dolbyCodec);
	OTYPE_PRINT_BOOL(t->lcnFlag);
	OTYPE_PRINT_ENUM(t->audioConfig);
	OTYPE_PRINT_ENUM(t->updateSvcFlag);

#ifdef	CONFIG_OP_FREESAT
	{
		const fsatDxFreesatService_t	*freesat = &(t->ex.freesat);
		OTYPE_PRINT_HEX(freesat->svcId);
		OTYPE_PRINT_HEX(freesat->regionId);
		OTYPE_PRINT_STR(freesat->defaultAuth);
		OTYPE_PRINT_HEX(freesat->contentMgr);
		OTYPE_PRINT_STR(freesat->szShortName);
		OTYPE_PRINT_HEX(freesat->ucUserCustomizable);
		OTYPE_PRINT_HEX(freesat->ucUserSelectable);
		OTYPE_PRINT_HEX(freesat->ucAdultSvc);
		OTYPE_PRINT_HEX(freesat->ucVideoCodecH264);
		OTYPE_PRINT_HEX(freesat->ucGuidanceType);
		OTYPE_PRINT_HEX(freesat->ucGuidanceMode);
		OTYPE_PRINT_STR(freesat->szGuidanceStr);
	}
#endif
}

fsatDxTransponder_t *	fsatDxTransponder_DecodeBin (fsatDxTransponder_t *type, HxJSON_t json)
{
    fsatDxTransponder_t *transponder;

	transponder = (fsatDxTransponder_t *)HLIB_JSONBIN_DECODER_Decode(NULL, type, sizeof(fsatDxTransponder_t), json, NULL, NULL, NULL);

    HxLOG_Print("[%s:%d]saved crc[%x]CRC[%x]\n",__FUNCTION__,__LINE__,transponder->__CRC__,local_TransponderGetCRC() );
    if(transponder->__CRC__ == local_TransponderGetCRC())
        return transponder;

    HxLOG_Critical("[%s:%d]CRC is a mismatch [%x][%x]\n",__FUNCTION__,__LINE__,transponder->__CRC__,local_RECGetCRC() );
    return NULL;
}

const HCHAR *		fsatDxTransponder_EncodeBin (HxJSONBIN_t enc, const fsatDxTransponder_t *type)
{
    fsatDxTransponder_t data;
    HxSTD_MemCopy(&data,type,sizeof(fsatDxTransponder_t));
    data.__CRC__= local_TransponderGetCRC();

	HLIB_JSONBIN_ENCODER_Clear(enc);

    HxLOG_Print("[%s:%d]CRC[%x]\n",__FUNCTION__,__LINE__,data.__CRC__ );
	return_if (!HLIB_JSONBIN_ENCODER_SetBinary(enc, &data, sizeof(fsatDxTransponder_t)), NULL);
	return HLIB_JSONBIN_ENCODER_Encode(enc);
}

void				fsatDxTransponder_Print (FILE *fp, const fsatDxTransponder_t *t)
{
	fprintf(fp, "TRANSPONDER[%d]\n"
		, t->uid
	);
	OTYPE_PRINT_INT(t->version);
	OTYPE_PRINT_INT(t->netuid);
	OTYPE_PRINT_HEX(t->tsid);
	OTYPE_PRINT_HEX(t->onid);
	OTYPE_PRINT_HEX(t->tunerid);
	OTYPE_PRINT_ENUM(t->deliType);
	OTYPE_PRINT_BYTE_DUMP(t->tuningParam, sizeof(fsatDxTransponder_t));

#ifdef	CONFIG_OP_FREESAT
	OTYPE_PRINT_STR(t->ex.freesat.defaultAuth);
#endif
}

fsatDxProvider_t *	fsatDxProvider_DecodeBin (fsatDxProvider_t *type, HxJSON_t json)
{
    fsatDxProvider_t *provider;

	provider = (fsatDxProvider_t *)HLIB_JSONBIN_DECODER_Decode(NULL, type, sizeof(fsatDxProvider_t), json, NULL, NULL, NULL);

    HxLOG_Print("[%s:%d]saved crc[%x]CRC[%x]\n",__FUNCTION__,__LINE__,provider->__CRC__,local_ProviderGetCRC() );
    if(provider->__CRC__ == local_ProviderGetCRC())
        return provider;
    HxLOG_Critical("[%s:%d]CRC is a mismatch[%x][%x]\n",__FUNCTION__,__LINE__,provider->__CRC__,local_RECGetCRC() );
    return NULL;
}

const HCHAR *		fsatDxProvider_EncodeBin (HxJSONBIN_t enc, const fsatDxProvider_t *type)
{
    fsatDxProvider_t data;
    HxSTD_MemCopy(&data,type,sizeof(fsatDxProvider_t));
    data.__CRC__= local_ProviderGetCRC();
    HxLOG_Print("[%s:%d]CRC[%x]\n",__FUNCTION__,__LINE__,data.__CRC__ );

	HLIB_JSONBIN_ENCODER_Clear(enc);
	return_if (!HLIB_JSONBIN_ENCODER_SetBinary(enc, &data, sizeof(fsatDxProvider_t)), NULL);
	return HLIB_JSONBIN_ENCODER_Encode(enc);
}


void				fsatDxProvider_Print (FILE *fp, const fsatDxProvider_t *t)
{
	fprintf(fp, "PROVIDER[%d] %s\n"
		, t->uid, t->name
	);
	OTYPE_PRINT_INT(t->version);
}

fsatDxGroup_t *			fsatDxGroup_DecodeBin (fsatDxGroup_t *type, HxJSON_t json)
{
    fsatDxGroup_t *group;

	group =  (fsatDxGroup_t *)HLIB_JSONBIN_DECODER_Decode(NULL, type, sizeof(fsatDxGroup_t), json, NULL, NULL, NULL);

    HxLOG_Print("[%s:%d]saved crc[%x]CRC[%x]\n",__FUNCTION__,__LINE__,group->__CRC__,local_GroupGetCRC() );

    if(group->__CRC__ == local_GroupGetCRC())
        return group;
    HxLOG_Critical("[%s:%d]CRC is a mismatch [%x][%x]\n",__FUNCTION__,__LINE__,group->__CRC__,local_RECGetCRC() );
    return NULL;
}
const HCHAR *			fsatDxGroup_EncodeBin (HxJSONBIN_t enc, const fsatDxGroup_t *type)
{
    fsatDxGroup_t data;
    HxSTD_MemCopy(&data,type,sizeof(fsatDxGroup_t));
    data.__CRC__= local_GroupGetCRC();
    HxLOG_Print("[%s:%d]CRC[%x]\n",__FUNCTION__,__LINE__,data.__CRC__ );

	HLIB_JSONBIN_ENCODER_Clear(enc);
    return_if (!HLIB_JSONBIN_ENCODER_SetBinary(enc, &data, sizeof(fsatDxGroup_t)), NULL);
	return HLIB_JSONBIN_ENCODER_Encode(enc);
}



void				fsatDxGroup_Print (FILE *fp, const fsatDxGroup_t *t)
{
	fprintf(fp, "GROUP[%d] id:%d %s\n"
		, t->uid
		, t->id
		, t->name
	);
	OTYPE_PRINT_INT(t->version);

#ifdef	CONFIG_OP_FREESAT
	{
		const fsatDxFreesatGroup_t *freesat = &(t->ex.freesat);

		OTYPE_PRINT_HEX(freesat->ucGroupType);
		OTYPE_PRINT_HEX(freesat->ucNondestructiveTuneFlag);
		OTYPE_PRINT_HEX(freesat->ucReturnChannelAccessFlag);
		OTYPE_PRINT_STR(freesat->szIso639LangCode);
		OTYPE_PRINT_HEX(freesat->ucG2ExtensionFlag);
		OTYPE_PRINT_HEX(freesat->ucDefaultVisableFlag);
		OTYPE_PRINT_HEX(freesat->ucAdultChannelsGroupFlag);
		OTYPE_PRINT_HEX(freesat->ucIpChannelsGroupFlag);
		OTYPE_PRINT_HEX(freesat->ucCaChannelsGroupFlag);
	}
#endif
}


fsatDxBouquet_t *		fsatDxBouquet_DecodeBin (fsatDxBouquet_t *type, HxJSON_t json)
{
    fsatDxBouquet_t *bouquet;

	bouquet =  (fsatDxBouquet_t *)HLIB_JSONBIN_DECODER_Decode(NULL, type, sizeof(fsatDxBouquet_t), json, NULL, NULL, NULL);

    HxLOG_Print("[%s:%d]saved crc[%x]CRC[%x]\n",__FUNCTION__,__LINE__,bouquet->__CRC__,local_BouquetGetCRC() );
    if(bouquet->__CRC__ == local_BouquetGetCRC())
        return bouquet;
    HxLOG_Critical("[%s:%d]CRC is a mismatch [%x][%x]\n",__FUNCTION__,__LINE__,bouquet->__CRC__,local_RECGetCRC() );
    return NULL;
}
const HCHAR *			fsatDxBouquet_EncodeBin (HxJSONBIN_t enc, const fsatDxBouquet_t *type)
{
    fsatDxBouquet_t data;
    HxSTD_MemCopy(&data,type,sizeof(fsatDxBouquet_t));
    data.__CRC__= local_BouquetGetCRC();
    HxLOG_Print("[%s:%d]CRC[%x]\n",__FUNCTION__,__LINE__,data.__CRC__ );

	HLIB_JSONBIN_ENCODER_Clear(enc);
   	return_if (!HLIB_JSONBIN_ENCODER_SetBinary(enc, &data, sizeof(fsatDxBouquet_t)), NULL);
	return HLIB_JSONBIN_ENCODER_Encode(enc);
}


void				fsatDxBouquet_Print (FILE *fp, const fsatDxBouquet_t *t)
{
	fprintf(fp, "BOUQUET[%d] id:%d %s\n"
		, t->uid
		, t->id
		, t->name
	);
	OTYPE_PRINT_INT(t->version);
}


fsatDxNetwork_t *		fsatDxNetwork_DecodeBin (fsatDxNetwork_t *type, HxJSON_t json)
{
    fsatDxNetwork_t *network;

	network = (fsatDxNetwork_t *)HLIB_JSONBIN_DECODER_Decode(NULL, type, sizeof(fsatDxNetwork_t), json, NULL, NULL, NULL);

    HxLOG_Print("[%s:%d]saved crc[%x]CRC[%x]\n",__FUNCTION__,__LINE__,network->__CRC__,local_NetworkGetCRC() );
    if(network->__CRC__ == local_NetworkGetCRC())
        return    network;
    HxLOG_Critical("[%s:%d]CRC is a mismatch [%x][%x]\n",__FUNCTION__,__LINE__,network->__CRC__,local_RECGetCRC() );
    return NULL;
}
const HCHAR *			fsatDxNetwork_EncodeBin (HxJSONBIN_t enc, const fsatDxNetwork_t *type)
{
    fsatDxNetwork_t data;
    HxSTD_MemCopy(&data,type,sizeof(fsatDxNetwork_t));
    data.__CRC__= local_NetworkGetCRC();
    HxLOG_Print("[%s:%d]CRC[%x]\n",__FUNCTION__,__LINE__,data.__CRC__ );

	HLIB_JSONBIN_ENCODER_Clear(enc);

    return_if (!HLIB_JSONBIN_ENCODER_SetBinary(enc, &data, sizeof(fsatDxNetwork_t)), NULL);

	return HLIB_JSONBIN_ENCODER_Encode(enc);
}

void				fsatDxNetwork_Print (FILE *fp, const fsatDxNetwork_t *t)
{
	fprintf(fp, "NETWORK[%d] %s\n"
		, t->uid
		, t->name
	);
	OTYPE_PRINT_INT(t->version);
	OTYPE_PRINT_HEX(t->onid);
	OTYPE_PRINT_ENUM(t->deliType);

#ifdef	CONFIG_OP_FREESAT
	OTYPE_PRINT_STR(t->ex.freesat.defaultAuth);
#endif
}





#endif


fsatDxService_t *	        fsatDxService_Decode(fsatDxService_t *service, HxJSON_t json)
{
    local_op_service_Decode(service ,json); //op
    return (fsatDxService_t *)local_metasvc_DecodeService((void *)service,json);
}

HCHAR*            fsatDxService_Encode(const fsatDxService_t *pvData)
{
   HCHAR *extention,*result;

   extention = local_op_service_Encode(pvData); //op
   result = local_metasvc_EncodeService( (const void*)pvData,extention);

   HLIB_STD_MemFree(extention);
   return result;
}

fsatDxTransponder_t *	fsatDxTransponder_Decode (fsatDxTransponder_t *transponder, HxJSON_t json)
{
    local_op_Transponder_Decode(transponder ,json); //op
    return local_metasvc_DecodeTransponder((void *)transponder,json);
}

HCHAR* fsatDxTransponder_Encode(const fsatDxTransponder_t *transponder)
{
    HCHAR *extention,*result;

    extention = local_op_Transponder_Encode(transponder); //op
    result =  local_metasvc_EncodeTransponder( (const void*)transponder,extention);
    HLIB_STD_MemFree(extention);
    return result;
}

fsatDxProvider_t *			fsatDxProvider_Decode (fsatDxProvider_t *provider, HxJSON_t json)
{
    return local_metasvc_DecodeProvider((void *)provider,json);
}
HCHAR *			fsatDxProvider_Encode (const fsatDxProvider_t *provider)
{
    return  local_metasvc_EncodeProvider( (const void*)provider,NULL);
}

fsatDxGroup_t *				fsatDxGroup_Decode (fsatDxGroup_t *group, HxJSON_t json)
{
    local_op_Group_Decode(group ,json); //op
    return local_metasvc_DecodeGroup((void *)group,json);
}
HCHAR *			fsatDxGroup_Encode (const fsatDxGroup_t *group)
{
    HCHAR *extention,*result;

    extention = local_op_Group_Encode(group); //op
    result =  local_metasvc_EncodeGroup( (const void*)group,extention);
    HLIB_STD_MemFree(extention);
    return result;
}


fsatDxBouquet_t *			fsatDxBouquet_Decode(fsatDxBouquet_t *bouquet, HxJSON_t json)
{
    return local_metasvc_DecodeBouquet((void *)bouquet,json);

}
HCHAR *			fsatDxBouquet_Encode(const fsatDxBouquet_t *bouquet)
{
    return  local_metasvc_EncodeBouquet( (const void*)bouquet,NULL);
}

fsatDxNetwork_t *			fsatDxNetwork_Decode (fsatDxNetwork_t *network, HxJSON_t json)
{
    local_op_Network_Decode(network ,json); //op
    return local_metasvc_DecodeNetwork((void *)network,json);
}
HCHAR *			fsatDxNetwork_Encode (const fsatDxNetwork_t *network)
{
    HCHAR *extention,*result;
    extention = local_op_Network_Encode(network); //op
    result =  local_metasvc_EncodeNetwork( (const void*)network,extention);

    HLIB_STD_MemFree(extention);
    return result;
}


HERROR              	fsatDxSchedule_Decode (DxSCHEDULE_t *shcedule, HxJSON_t json)
{
    return local_metaSched_Decode(shcedule,json);
}
HCHAR *			fsatDxSchedule_Encode (const DxSCHEDULE_t *shcedule)
{
    return local_metaSched_Encode((const void*)shcedule);
}

HERROR		            fsatDxRecData_Decode (fsatDxRecListData_t *Rec, HxJSON_t json)
{
    return local_metaRec_Decode(Rec,json);

}
HCHAR *			fsatDxRecData_Encode (const fsatDxRecListData_t *Rec)
{
    return local_metaRec_Encode(Rec);
}

#ifdef	SUPPORT_DENC_FILE

//#define fsatDxEDPRINT
void PrintDataEncode(HCHAR *name, void* data)
{
#ifdef  fsatDxEDPRINT
    FILE *fp;
    fp = fopen(name,"a+");
    if(strcmp(name,"fsatDxService_t")==0 || strcmp(name,"fsatDxService_tDec")==0)
    {
            fsatDxService_Print(fp,(fsatDxService_t*)data);
    }
    else if(strcmp(name,"fsatDxTransponder_t")==0|| strcmp(name,"fsatDxTransponder_tDec")==0)
    {
             fsatDxTransponder_Print(fp,(fsatDxTransponder_t*)data);
    }
    else if(strcmp(name,"fsatDxProvider_t")==0|| strcmp(name,"fsatDxProvider_tDec")==0)
    {
             fsatDxProvider_Print(fp,(fsatDxProvider_t*)data);
    }
    else if(strcmp(name,"fsatDxNetwork_t")==0|| strcmp(name,"fsatDxNetwork_tDec")==0)
    {
             fsatDxNetwork_Print(fp,(fsatDxNetwork_t*)data);
    }
    else if(strcmp(name,"fsatDxGroup_t")==0|| strcmp(name,"fsatDxGroup_tDec")==0)
    {
             fsatDxGroup_Print(fp,(fsatDxGroup_t*)data);
    }
    else if(strcmp(name,"fsatDxBouquet_t")==0|| strcmp(name,"fsatDxBouquet_tDec")==0)
    {
            fsatDxBouquet_Print(fp,(fsatDxBouquet_t*)data);
    }

     fclose(fp);
 #endif
}

void PrintDataDecode(HCHAR *name,const HCHAR* str)
{
#ifdef   fsatDxEDPRINT
    FILE *fp;
    fp = fopen(name,"a+");
    fprintf(fp,"\nDecodeing- %s\n",str);
    fclose(fp);
#endif
}

HCHAR*					fsatDxService_EncodeAll(HxList_t *list)
{
    fsatDxService_t *data;
    HINT32 sd,i=0;
    HCHAR* json =NULL;
    HxList_t *tmp_list = list;
    sd = HLIB_RWSTREAM_Open(NULL, 0);
    if (sd)
    {
        HLIB_RWSTREAM_Print(sd, "{ \"service\": [");
        while (list)
        {
            data = (fsatDxService_t *)HLIB_LIST_Data(list);
            json = fsatDxService_Encode(data);
            PrintDataEncode(fsatDxNAMEOF(fsatDxService_t),(void*)data);
            if(i++ == 0)
                HLIB_RWSTREAM_Print(sd," %s", json);
            else
                HLIB_RWSTREAM_Print(sd,", %s", json);
            list = list->next;

            if(data) HLIB_STD_MemFree(data);
            if(json) HLIB_STD_MemFree(json); json =NULL;
        }
        HLIB_RWSTREAM_Print(sd, "]}");
        HLIB_LIST_RemoveAll(tmp_list);
        return (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
    }

    while (list)
    {
        data = (fsatDxService_t *)HLIB_LIST_Data(list);
        list = list->next;

        if(data) HLIB_STD_MemFree(data);
    }
    HLIB_LIST_RemoveAll(tmp_list);
    return NULL;
}

void*			fsatDxService_DecodeAll(HCHAR * json, HINT32 *size)
{
    HxJSON_t   list ,item,innerjson;
    fsatDxService_t service;
    void * result =NULL;
    HINT32 i,cnt;
    PrintDataDecode(fsatDxNAMEOF(fsatDxService_t),json);
    list = HLIB_JSON_Decode (json);
    if(list ==NULL) return NULL;
    item = HxJSON_Object_Get(list, "service");
    if(HxJSON_IsArray(item)==TRUE)
    {
        cnt = HLIB_JSON_GetSize(item);
        result = HLIB_STD_MemCalloc(sizeof(fsatDxService_t) *cnt);
        for (i = 0; i < cnt; i++)
        {
            innerjson = HxJSON_Array_Get(item, i);
            HxSTD_MemSet(&service,0,sizeof(fsatDxService_t));
            HxSTD_MemCopy(result + sizeof(fsatDxService_t)*i,fsatDxService_Decode(&service,innerjson),sizeof(fsatDxService_t));
            PrintDataEncode("fsatDxService_tDec", &service);
        }
        *size = sizeof(fsatDxService_t) *cnt;
    }
    HLIB_JSON_Delete(list);


    return result;
}

HCHAR*					fsatDxTransponder_EncodeAll(HxList_t *list)
{
    fsatDxTransponder_t *data;
    HINT32 sd,i=0;
    HCHAR* json =NULL;
    HxList_t *tmp_list = list;
    sd = HLIB_RWSTREAM_Open(NULL, 0);
    if (sd)
    {
        HLIB_RWSTREAM_Print(sd, "{ \"transponder\": [");
        while (list)
        {
            data = (fsatDxTransponder_t *)HLIB_LIST_Data(list);
            PrintDataEncode(fsatDxNAMEOF(fsatDxTransponder_t),(void*)data);
            json = fsatDxTransponder_Encode(data);
            if(i++ ==0)
                HLIB_RWSTREAM_Print(sd,"%s" ,json);
            else
                HLIB_RWSTREAM_Print(sd,", %s" ,json);
            list = list->next;

            if(data) HLIB_STD_MemFree(data);
            if(json) HLIB_STD_MemFree(json); json =NULL;
        }
        HLIB_RWSTREAM_Print(sd, "]}");
        HLIB_LIST_RemoveAll(tmp_list);
        return  (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
    }

    while (list)
    {
        data = (fsatDxTransponder_t *)HLIB_LIST_Data(list);
        list = list->next;

        if(data) HLIB_STD_MemFree(data);
    }
    HLIB_LIST_RemoveAll(tmp_list);
    return NULL;
}

void*		fsatDxTransponder_DecodeAll(HCHAR * json, HINT32 *size)
{
    HxJSON_t   list ,item,innerjson;
    fsatDxTransponder_t trans;
    void * result =NULL;
    HINT32 i,cnt;
    PrintDataDecode(fsatDxNAMEOF(fsatDxTransponder_t),json);
    list = HLIB_JSON_Decode (json);
    if(list ==NULL) return NULL;
    item = HxJSON_Object_Get(list, "transponder");
    if(HxJSON_IsArray(item)==TRUE)
    {
        cnt = HLIB_JSON_GetSize(item);
        result = HLIB_STD_MemCalloc(sizeof(fsatDxTransponder_t) *cnt);
        for (i = 0; i < cnt; i++)
        {
            innerjson = HxJSON_Array_Get(item, i);
            HxSTD_MemSet(&trans,0,sizeof(fsatDxTransponder_t));
            HxSTD_MemCopy(result + sizeof(fsatDxTransponder_t)*i,fsatDxTransponder_Decode(&trans,innerjson),sizeof(fsatDxTransponder_t));
            PrintDataEncode("fsatDxTransponder_tDec", &trans);

        }
        *size = sizeof(fsatDxTransponder_t) *cnt;
    }

    HLIB_JSON_Delete(list);
    return result;
}

HCHAR*					fsatDxProvider_EncodeAll(HxList_t *list)
{
    fsatDxProvider_t *data;
    HINT32 sd,i=0;
    HCHAR* json =NULL;
    HxList_t *tmp_list = list;
    sd = HLIB_RWSTREAM_Open(NULL, 0);
    if (sd)
    {
        HLIB_RWSTREAM_Print(sd, "{ \"provider\": [");
        while (list)
        {
            data = (fsatDxProvider_t *)HLIB_LIST_Data(list);
            json = fsatDxProvider_Encode(data);
            PrintDataEncode(fsatDxNAMEOF(fsatDxProvider_t),(void*)data);
            if(i++ ==0)
                HLIB_RWSTREAM_Print(sd,"%s" ,json);
            else
                HLIB_RWSTREAM_Print(sd,", %s",json);
            list = list->next;

            if(data) HLIB_STD_MemFree(data);
            if(json) HLIB_STD_MemFree(json); json =NULL;
        }
        HLIB_RWSTREAM_Print(sd, "]}");
        HLIB_LIST_RemoveAll(tmp_list);
        return  (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
    }

    while (list)
    {
        data = (fsatDxProvider_t *)HLIB_LIST_Data(list);
        list = list->next;

        if(data) HLIB_STD_MemFree(data);
    }
    HLIB_LIST_RemoveAll(tmp_list);
    return NULL;
}

void*			fsatDxProvider_DecodeAll(HCHAR * json, HINT32 *size)
{
    HxJSON_t   list ,item,innerjson;
    fsatDxProvider_t data;
    void * result =NULL;
    HINT32 i,cnt;
    PrintDataDecode(fsatDxNAMEOF(fsatDxProvider_t),json);
    list = HLIB_JSON_Decode (json);
    if(list ==NULL) return NULL;
    item = HxJSON_Object_Get(list, "provider");
    if(HxJSON_IsArray(item)==TRUE)
    {
        cnt = HLIB_JSON_GetSize(item);
        result = HLIB_STD_MemCalloc(sizeof(fsatDxProvider_t) *cnt);
        for (i = 0; i < cnt; i++)
        {
            innerjson = HxJSON_Array_Get(item, i);
            HxSTD_MemSet(&data,0,sizeof(fsatDxProvider_t));
            HxSTD_MemCopy(result + sizeof(fsatDxProvider_t)*i,fsatDxProvider_Decode(&data,innerjson),sizeof(fsatDxProvider_t));
            PrintDataEncode("fsatDxProvider_tDec", &data);
        }
        *size = sizeof(fsatDxProvider_t) *cnt;
    }

    HLIB_JSON_Delete(list);
    return result;
}

HCHAR*					fsatDxNetwork_EncodeAll(HxList_t *list)
{
    fsatDxNetwork_t *data;
    HINT32 sd,i=0;
    HCHAR* json =NULL;
    HxList_t *tmp_list = list;
    sd = HLIB_RWSTREAM_Open(NULL, 0);
    if (sd)
    {
        HLIB_RWSTREAM_Print(sd, "{ \"network\": [");
        while (list)
        {
            data = (fsatDxNetwork_t *)HLIB_LIST_Data(list);
            json = fsatDxNetwork_Encode(data);
            PrintDataEncode(fsatDxNAMEOF(fsatDxNetwork_t),(void*)data);
            if(i++ ==0)
                HLIB_RWSTREAM_Print(sd,"%s" ,json);
            else
                HLIB_RWSTREAM_Print(sd,", %s",json);
            list = list->next;

            if(data) HLIB_STD_MemFree(data);
            if(json) HLIB_STD_MemFree(json); json =NULL;
        }
        HLIB_RWSTREAM_Print(sd, "]}");
        HLIB_LIST_RemoveAll(tmp_list);
        return  (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
    }

    while (list)
    {
        data = (fsatDxNetwork_t *)HLIB_LIST_Data(list);
        list = list->next;

        if(data) HLIB_STD_MemFree(data);
    }
    HLIB_LIST_RemoveAll(tmp_list);
    return NULL;
}

void*			fsatDxNetwork_DecodeAll(HCHAR * json, HINT32 *size)
{
    HxJSON_t   list ,item,innerjson;
    fsatDxNetwork_t data;
    void * result =NULL;
    HINT32 i,cnt;
    PrintDataDecode(fsatDxNAMEOF(fsatDxNetwork_t),json);
    list = HLIB_JSON_Decode (json);
    if(list ==NULL) return NULL;
    item = HxJSON_Object_Get(list, "network");
    if(HxJSON_IsArray(item)==TRUE)
    {
        cnt = HLIB_JSON_GetSize(item);
        result = HLIB_STD_MemCalloc(sizeof(fsatDxNetwork_t) *cnt);
        for (i = 0; i < cnt; i++)
        {
            innerjson = HxJSON_Array_Get(item, i);
            HxSTD_MemSet(&data,0,sizeof(fsatDxNetwork_t));
            HxSTD_MemCopy(result + sizeof(fsatDxNetwork_t)*i,fsatDxNetwork_Decode(&data,innerjson),sizeof(fsatDxNetwork_t));
            PrintDataEncode("fsatDxNetwork_tDec", &data);
        }
        *size = sizeof(fsatDxNetwork_t) *cnt;
    }

    HLIB_JSON_Delete(list);
    return result;
}

HCHAR*					fsatDxGroup_EncodeAll(HxList_t *list)
{
    fsatDxGroup_t *data;
    HINT32 sd,i=0;
    HCHAR* json =NULL;
    HxList_t *tmp_list = list;
    sd = HLIB_RWSTREAM_Open(NULL, 0);

    if (sd)
    {
        HLIB_RWSTREAM_Print(sd, "{ \"group\": [");
        while (list)
        {
            data = (fsatDxGroup_t *)HLIB_LIST_Data(list);
            json = fsatDxGroup_Encode(data);
            PrintDataEncode(fsatDxNAMEOF(fsatDxGroup_t),(void*)data);
            if(i++ ==0)
                HLIB_RWSTREAM_Print(sd,"%s" , json);
            else
                HLIB_RWSTREAM_Print(sd,", %s", json);
            list = list->next;

            if(data) HLIB_STD_MemFree(data);
            if(json) HLIB_STD_MemFree(json); json =NULL;
        }
        HLIB_RWSTREAM_Print(sd, "]}");
        HLIB_LIST_RemoveAll(tmp_list);
        return (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
    }

    while (list)
    {
        data = (fsatDxGroup_t *)HLIB_LIST_Data(list);
        list = list->next;

        if(data) HLIB_STD_MemFree(data);
    }
    HLIB_LIST_RemoveAll(tmp_list);
    return NULL;
}

void*				fsatDxGroup_DecodeAll(HCHAR * json, HINT32 *size)
{
    HxJSON_t   list ,item,innerjson;
    fsatDxGroup_t data;
    void * result =NULL;
    HINT32 i,cnt;
    PrintDataDecode(fsatDxNAMEOF(fsatDxGroup_t),json);
    list = HLIB_JSON_Decode (json);
    if(list ==NULL) return NULL;
    item = HxJSON_Object_Get(list, "group");
    if(HxJSON_IsArray(item)==TRUE)
    {
        cnt = HLIB_JSON_GetSize(item);
        result = HLIB_STD_MemCalloc(sizeof(fsatDxGroup_t) *cnt);
        for (i = 0; i < cnt; i++)
        {
            innerjson = HxJSON_Array_Get(item, i);
            HxSTD_MemSet(&data,0,sizeof(fsatDxGroup_t));
            HxSTD_MemCopy(result + sizeof(fsatDxGroup_t)*i,fsatDxGroup_Decode(&data,innerjson),sizeof(fsatDxGroup_t));
            PrintDataEncode("fsatDxGroup_tDec", &data);
        }
        *size = sizeof(fsatDxGroup_t) *cnt;
    }

    HLIB_JSON_Delete(list);
    return result;
}

HCHAR*					fsatDxBouquet_EncodeAll(HxList_t *list)
{
    fsatDxBouquet_t *data;
    HINT32 sd,i=0;
    HCHAR* json =NULL;
    HxList_t *tmp_list = list;
    sd = HLIB_RWSTREAM_Open(NULL, 0);

    if (sd)
    {
        HLIB_RWSTREAM_Print(sd, "{ \"bouquet\": [");
        while (list)
        {
            data = (fsatDxBouquet_t *)HLIB_LIST_Data(list);
            json = fsatDxBouquet_Encode(data);
            PrintDataEncode(fsatDxNAMEOF(fsatDxBouquet_t),(void*)data);
            if(i++ ==0)
                HLIB_RWSTREAM_Print(sd,"%s" ,json );
            else
                HLIB_RWSTREAM_Print(sd,", %s" ,json);
            list = list->next;

            if(data) HLIB_STD_MemFree(data);
            if(json) HLIB_STD_MemFree(json); json =NULL;
        }
        HLIB_RWSTREAM_Print(sd, "]}");
        HLIB_LIST_RemoveAll(tmp_list);
        return  (HCHAR *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
    }

    while (list)
    {
        data = (fsatDxBouquet_t *)HLIB_LIST_Data(list);
        list = list->next;

        if(data) HLIB_STD_MemFree(data);
    }
    HLIB_LIST_RemoveAll(tmp_list);
    return NULL;
}

void*			fsatDxBouquet_DecodeAll(HCHAR * json, HINT32 *size)
{
    HxJSON_t   list ,item,innerjson;
    fsatDxBouquet_t data;
    void * result =NULL;
    HINT32 i,cnt;
    PrintDataDecode(fsatDxNAMEOF(fsatDxBouquet_t),json);
    list = HLIB_JSON_Decode (json);
    if(list ==NULL) return NULL;
    item = HxJSON_Object_Get(list, "bouquet");
    if(HxJSON_IsArray(item)==TRUE)
    {
        cnt = HLIB_JSON_GetSize(item);
        result = HLIB_STD_MemCalloc(sizeof(fsatDxBouquet_t) *cnt);
        for (i = 0; i < cnt; i++)
        {
            innerjson = HxJSON_Array_Get(item, i);
            HxSTD_MemSet(&data,0,sizeof(fsatDxBouquet_t));
            HxSTD_MemCopy(result + sizeof(fsatDxBouquet_t)*i,fsatDxBouquet_Decode(&data,innerjson),sizeof(fsatDxBouquet_t));
            PrintDataEncode("fsatDxBouquet_tDec", &data);
        }
         *size = sizeof(fsatDxBouquet_t) *cnt;
    }
    HLIB_JSON_Delete(list);
    return result;
}

typedef struct
{
	HCHAR	*pszName;
	HUINT32	nSize;
	HCHAR*	(*encode)(HxList_t *);		//	fsatDxData --> JSON TEXT
	void*	(*decode)(HCHAR * ,HINT32 *);	//	JSON TEXT --> fsatDxData
} fsatDxCodecInfo_t;

static fsatDxCodecInfo_t	s_pstCodecInfos[] =
{
	{fsatDxNAMEOF(fsatDxService_t),		sizeof(fsatDxService_t),		fsatDxService_EncodeAll,		fsatDxService_DecodeAll},
	{fsatDxNAMEOF(fsatDxTransponder_t),	sizeof(fsatDxTransponder_t),	fsatDxTransponder_EncodeAll,	fsatDxTransponder_DecodeAll},
	{fsatDxNAMEOF(fsatDxProvider_t),	sizeof(fsatDxProvider_t),		fsatDxProvider_EncodeAll,		fsatDxProvider_DecodeAll},
	{fsatDxNAMEOF(fsatDxNetwork_t),		sizeof(fsatDxNetwork_t),		fsatDxNetwork_EncodeAll,		fsatDxNetwork_DecodeAll},
	{fsatDxNAMEOF(fsatDxGroup_t),		sizeof(fsatDxGroup_t),			fsatDxGroup_EncodeAll,			fsatDxGroup_DecodeAll},
	{fsatDxNAMEOF(fsatDxBouquet_t),		sizeof(fsatDxBouquet_t),		fsatDxBouquet_EncodeAll,		fsatDxBouquet_DecodeAll}
};

static fsatDxCodecInfo_t*	local_codec_findInfo(HCHAR *objectName)
{
	HINT32	i, tot;

	tot = sizeof(s_pstCodecInfos) / sizeof(fsatDxCodecInfo_t);

	for (i = 0; i < tot; i++)
	{
		if (HxSTD_StrCmp(objectName, s_pstCodecInfos[i].pszName) == 0)
			return &(s_pstCodecInfos[i]);
	}

	return NULL;
}

HCHAR*	fsatDxXXX_EncodeFile(HCHAR *objectName, void * data,HINT32 size)
{
	fsatDxCodecInfo_t	*codecInfo;
	HxList_t        *list=NULL;
	HINT32          i,loof;
	void*           item;

	codecInfo = local_codec_findInfo(objectName);
	if (codecInfo == NULL || data == NULL)
		return NULL;
	if(size % codecInfo->nSize)
	{
		HxLOG_Critical("[%s:%d]",__FUNCTION__,__LINE__);
		return NULL;
	}
	loof = size / codecInfo->nSize;

	for(i=0;i<loof;i++)
	{
		item = HLIB_STD_MemAlloc(codecInfo->nSize);
		HxSTD_MemCopy(item, data + codecInfo->nSize * i, codecInfo->nSize);
		list = HLIB_LIST_Append(list, item);
	}

	if(list == NULL)
		return NULL;

	return codecInfo->encode(HLIB_LIST_First(list));
}

void*	fsatDxXXX_DecodeFile(HCHAR *objectName,  void * data ,HINT32 *size)
{
	fsatDxCodecInfo_t	*codecInfo;

	codecInfo = local_codec_findInfo(objectName);
	if (codecInfo == NULL || data ==NULL)
		return NULL;

	return codecInfo->decode((HCHAR*)data,size);
}

#endif

