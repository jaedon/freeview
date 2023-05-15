/**
	@file     port_itk_service.c
	@brief    ITK Host Service 모듈.

	Description: ITK Host Service Interface 이다.							\n
	Module: mw/mheg/itk												\n

	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>

#include "port_itk_main.h"
#include "port_itk_display.h"
#include "port_itk_service.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ITK_SVC_START_SVCLIST_IDX_NUM		1
#define ITK_SI_SVC_TYPE_TV					0x01
#define ITK_SI_SVC_TYPE_RADIO				0x02
#define ITK_SI_SVC_TYPE_DATA				0x0C
#define ITK_SVC_INVALID_SUBTITLE_TRACK		0xFFFF

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagUrl_Data
{
	HCHAR*		scheme;
	HCHAR*		host;
	HCHAR*		name;
	HCHAR*		value;
}ITK_UrlData_t;

typedef struct tagItkServiceList
{
	Handle_t					hSvc;
	HINT32						svcIndex;

	struct tagItkServiceList*	pNext;
}ITK_SvcIdxList_t;

typedef struct tagMhegServiceInfo
{
	redkey_profile_t	itkProfile;
	itk_service_t		object;
} MHEG_SERVICE_INFO_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
static MHEG_SERVICE_INFO_t			s_stMhegServiceInfo;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HUINT32						s_ulSvcIdxSema;
static itk_service_event_callback_t	s_cbSvcEvent;
static void*						s_pContextSvcEvent;


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
/*  Porting layer for Service Manager */
static itk_int32_t 		mheg_itk_port_svcFindSvc(struct itk_service_t* thiz, const char* service);
static itk_errcode_t 	mheg_itk_port_svcGetSvcLocator(struct itk_service_t* thiz, itk_int32_t index, itk_service_locator_t* loc);
static itk_errcode_t 	mheg_itk_port_svcGetSvcInfo(struct itk_service_t* thiz, itk_int32_t index, itk_service_info_t* info);
static itk_errcode_t 	mheg_itk_port_svcGetSvcEventInfo(struct itk_service_t* thiz, itk_int32_t index, itk_uint16_t eventIndex, itk_service_event_t* info);
static itk_errcode_t 	mheg_itk_port_svcStartRetune(struct itk_service_t* thiz, itk_int32_t index, itk_service_tune_flag_t flags);
static itk_errcode_t 	mheg_itk_port_svcCheckNonDestructiveTune(struct itk_service_t *thiz, itk_int32_t index1, itk_int32_t index2);
static itk_errcode_t 	mheg_itk_port_svcCheckMhegEnabled(struct itk_service_t *thiz, itk_int32_t index);
static itk_errcode_t 	mheg_itk_port_svcCheckMhegIC(struct itk_service_t *thiz, itk_int32_t index);
static itk_errcode_t 	mheg_itk_port_svcAddEventCallback(struct itk_service_t *thiz, itk_service_event_callback_t callback, void *context);
static void 			mheg_itk_port_svcRemoveEventCallback(struct itk_service_t *thiz);
static itk_int32_t 		mheg_itk_port_svcGetFreesatServiceIdentifier(struct itk_service_t * thiz, itk_int32_t service_index);
static itk_errcode_t 	mheg_itk_port_svcCheckGroupMembership(struct itk_service_t *thiz, itk_uint16_t freesat_service_index, itk_uint16_t freesat_service_group);
static size_t 			mheg_itk_port_svcGetSvcNvStorageAlloc(struct itk_service_t * thiz, itk_uint16_t freesat_service_index);
static size_t 			mheg_itk_port_svcGetGroupNvStorageAlloc(struct itk_service_t * thiz, itk_uint16_t freesat_service_group);
void 					mheg_itk_port_svcSetBroadcastInterruption(struct itk_service_t *thiz, const itk_control_t control);
itk_int32_t				mheg_itk_port_svcGetServiceCount(struct itk_service_t * thiz);
itk_int32_t				mheg_itk_port_svcGetNextServiceIndex(struct itk_service_t *thiz, itk_int32_t index, itk_int8_t updown);
itk_eits_iterator_t*	mheg_itk_port_svcNewEitsIterator(struct itk_service_t *thiz, itk_int32_t index, itk_int32_t date, itk_int32_t start);
itk_validation_t		mheg_itk_port_svcGetEventId(struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex, itk_uint16_t *eventId);
itk_uint16_t			mheg_itk_port_svcGetEitPfNumComponentDescs(struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex);
itk_validation_t		mheg_itk_port_svcGetEitPfComponentDescs(struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex, char *stream_content, itk_uint16_t scLength, char *component_type, itk_uint16_t ctLength);
itk_present_t			mheg_itk_port_svcGetEitPfComponentInfo(struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex, itk_component_t component);

/* inner MISC functions for Service Manager */
static HCHAR* 		mheg_itk_port_svcStr2Hex(HCHAR* str, HUINT32 *vp) ;
static HINT32 		mheg_itk_port_svcParseUrl(HCHAR* url, ITK_UrlData_t *uf);
static HCHAR* 		mheg_itk_port_svcStrGrab(HCHAR *src, HINT32 len);
static HCHAR* 		mheg_itk_port_svcStrScan(HCHAR *src, HCHAR *origsearch);
static void 		mheg_itk_port_svcFreeUrlData(ITK_UrlData_t *uf);
static HERROR 		mheg_itk_port_svcGetCurrentSvcUid(HINT32* pslSvcIndex);

static HERROR 		mheg_itk_port_svcGetSvcUidByLcn(HINT32 usLcn, Handle_t* phSvc);


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HCHAR* mheg_itk_port_svcStrGrab(HCHAR *src, HINT32 len)
{
	HCHAR *str = (HCHAR*)HLIB_STD_MemAlloc(len+1);

	HxSTD_memcpy(str, src, len);
	str[len] = 0;

	return str;
}

void mheg_itk_port_svcFreeUrlData(ITK_UrlData_t *uf)
{
	if (uf->scheme) HLIB_STD_MemFree(uf->scheme);
	if (uf->host) HLIB_STD_MemFree(uf->host);
	if (uf->name) HLIB_STD_MemFree(uf->name);
	if (uf->value) HLIB_STD_MemFree(uf->value);
}

HCHAR* mheg_itk_port_svcStrScan(HCHAR *src, HCHAR *origsearch)
{
	HCHAR *search;

	while (*src)
	{
		search = origsearch;
		while (*search) {
			if (*src == *search)
				return src;
			search++;
		}
		src++;
	}
	return src;
}

HINT32 mheg_itk_port_svcParseUrl(HCHAR* url, ITK_UrlData_t *uf)
{
	HCHAR *s = url, *e;

	HxSTD_memset(uf, 0, sizeof(ITK_UrlData_t));
	e = mheg_itk_port_svcStrScan(s, ":");
	if (e == s) goto err;
	uf->scheme = mheg_itk_port_svcStrGrab(s, e-s);
	s = e+1;
	if (*s == 0) goto err;
	if (!(*s == '/' && *(s+1) == '/')) goto err;
	s = s+2;
	if (*s == 0) goto err;
	e = mheg_itk_port_svcStrScan(s, "/");
	if (e == s) goto err;
	uf->host = mheg_itk_port_svcStrGrab(s, e-s);
	s = e;
	if (*s == 0) return 0;
	else s = s+1;
	e = mheg_itk_port_svcStrScan(s, "/");
	if (e == s) goto err;
	uf->name = mheg_itk_port_svcStrGrab(s, e-s);
	s = e;
	if (*s == 0) return 0;
	else s = s+1;
	e = mheg_itk_port_svcStrScan(s, "");
	if (e == s) goto err;
	uf->value = mheg_itk_port_svcStrGrab(s, e-s);
	return 0;
err:
	mheg_itk_port_svcFreeUrlData(uf);
	return -(e-url);
}

HCHAR* mheg_itk_port_svcStr2Hex(HCHAR *str, HUINT32 *vp)
{
	int v, c;

	if (*str == '0' && *(str+1) == 'x')
	{
		/*
		* DTG Test Suite 3.10.6.1 SI_GetServiceIndex [1of10]에서 '0x'를 Invalid Value로 처리한다.
		*/
		#if 0
		str += 2;
		#else
		return (str);
		#endif
	}

	for (v = 0;; v = v * 16 + c, str++)
	{
		c = (HUINT8)*str;
		if (c <= '9')
		{
			if ((c -= '0') < 0)
				break;
		}
		else if (c <= 'F')
		{
			if ((c -= 'A' - 10) < 10)
				break;
		}
		else if (c <= 'f')
		{
			if ((c -= 'a' - 10) < 10)
				break;
		}
		else
			break;
	}
	//ItkTrace(("str2hex: %x\n", v));
	*vp = v;
	return (str);
}

HERROR mheg_itk_port_svcGetSvcUidByLcn(HINT32 nLcn, Handle_t* phSvc)
{
	HxVector_t	*pstSvcList;
	DxService_t	*pstSvcInfo;

	pstSvcList = APK_META_SVC_FindServiceByNumber(nLcn);
	if (pstSvcList)
	{
		pstSvcInfo = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, 0);
		*phSvc = (Handle_t)pstSvcInfo->uid;

		HLIB_VECTOR_Delete(pstSvcList);

		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR mheg_itk_port_svcGetCurrentSvcUid(HINT32* pslSvcIndex)
{
	HINT32		nCurSvcUid;

	PORT_ITK_GetSvcUid(NULL, &nCurSvcUid);
	*pslSvcIndex = (HINT32)nCurSvcUid;

	if (nCurSvcUid == 0)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR mheg_itk_port_svcGetDefaultSvcUid(HINT32* pslSvcIndex)
{
	HINT32		nDefSvcUid;

	PORT_ITK_GetSvcUid(&nDefSvcUid, NULL);
	HxLOG_Debug("GetDefaultSVCUid : [%p]\n", nDefSvcUid);
	*pslSvcIndex = nDefSvcUid;

	if (nDefSvcUid == 0)
		return ERR_FAIL;

	return ERR_OK;
}

HUINT8 *local_itk_SVC_StrTrimCharcode(HUINT8 *pcSrc, HUINT32 unLen)
{
	HINT32	n = 0;

	if (pcSrc == NULL)
	{
		return NULL;
	}

	switch (*pcSrc)
	{
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x11:
			n = 1;
			break;

		case 0x10:
			n = 3;
			break;
/*
		case 0x11:
			n = 1;
			break;
*/
	}

	if (unLen < n)
	{
		n = 0;
	}

	return (pcSrc + n);
}

itk_errcode_t mheg_itk_port_svcGetSvcInfo(struct itk_service_t* thiz, itk_int32_t index, itk_service_info_t* info)
{
	Handle_t		hSvc;
	DxService_t		*pstSvcInfo;
	DxProvider_t	*pstPrvInfo;

	HxLOG_Assert(info);
	HxLOG_Debug(" Request service info of index(%ld) \n", index);
	if(thiz == NULL)
	{
		HxLOG_Assert(0);
		return ITKE_NOT_FOUND;
	}

	/* get service handle by service index retrived by function findService */
	hSvc = (Handle_t)index;

	pstSvcInfo = APK_META_SVC_GetService(hSvc);
	if (pstSvcInfo == NULL)
	{
		HxLOG_Error("fail to get svcinfo by uid %p\n", hSvc);
		return ITKE_NOT_FOUND;
	}

	HxSTD_memset(info->name, 0x00, ITK_SERVICE_INFO_MAX_STR);
	HLIB_STD_StrUtf8NCpy(info->name, pstSvcInfo->name, MIN(DxNAME_LEN, ITK_SERVICE_INFO_MAX_STR) - 1);

	switch(pstSvcInfo->svcType)
	{
		case eDxSVC_TYPE_TV:
			info->type	= ITK_SI_SVC_TYPE_TV;
			break;

		case eDxSVC_TYPE_RADIO:
			info->type	= ITK_SI_SVC_TYPE_RADIO;
			break;

		case eDxSVC_TYPE_DATA:
			info->type	= ITK_SI_SVC_TYPE_DATA;		/* <---  check Point #31 */
			break;

		default:
			HxLOG_Assert(0);
			info->type	= ITK_SI_SVC_TYPE_TV;
			break;
	}

	/* get provider information */
	HxSTD_memset(info->provider, 0x00, ITK_SERVICE_INFO_MAX_STR);
	pstPrvInfo = APK_META_SVC_GetProvider(pstSvcInfo->prvuid);
	if (pstPrvInfo)
	{
		HLIB_STD_StrUtf8NCpy(info->provider, pstPrvInfo->name, MIN(DxNAME_LEN, ITK_SERVICE_INFO_MAX_STR) - 1);
		APK_META_SVC_Delete(pstPrvInfo);
	}

	/* get subtitle track information */	/* <------- check point #32 */
	if (pstSvcInfo->user.subtitleTrack >= ITK_SVC_INVALID_SUBTITLE_TRACK)
	{
		info->subtitles = ITK_NOT_PRESENT;
	}
	else
	{
		info->subtitles = ITK_PRESENT;
	}


	APK_META_SVC_Delete(pstSvcInfo);

	return ITKE_OK;
}


/**
 * Obtains information on events available on the service.
 *
 * The event in question is defined by an eventIndex. Index = 0 indicates the current
 * event, index = 1 indicates the following event, and so on. The call is synchronous and
 * shall return with a FALSE value if the requested event is not yet available.
 *
 * @note For DVB EIT-based services it is expected that only EITp/f is monitored - i.e. the
 * maximum value of eventIndex required is 1. Support for additional values is optional.
 *
 * @note There is no requirement to ensure that event info is obtained from a coherent event
 * database. If the event data increments between two calls then the version returned must
 * be changed in line with the version of the data. If coherence is required by the client
 * then this will be managed above the API.
 *
 * @param[in]  thiz A pointer to a service database
 * @param[in]  index The service index
 * @param[in]  eventIndex A 0 based index for the event
 * @param[out] info A pointer to a itk_service_event_info_t structure to fill
 *
 * @return ITKE_OK if the event was found and populated
 * @return ITKE_NOT_FOUND if the index is not valid
 */
itk_errcode_t mheg_itk_port_svcGetSvcEventInfo(struct itk_service_t* thiz, itk_int32_t index, itk_uint16_t eventIndex, itk_service_event_t* info)
{
	itk_errcode_t			eErr = ITKE_OK;

#if defined(CONFIG_3RD_MHEG_AUSTRALIA_PROFILE)
#if 1
	MHEG_NOT_IMPLEMENTED;
#else
	Handle_t				hSvc = HANDLE_NULL;
	SvcEpg_FindParam_t	stEpgParam;
	POINTER_LIST_T		*pEpgList = NULL, *pHeadEpgList;
	SvcEpg_Event_t			*pstEvent = NULL, *pstPresent = NULL, *pstNext = NULL;
	HUINT8				*pucConvertString = NULL, *pucConvertString2 = NULL;
	unsigned long 			ulCurtime;
	HUINT8				aucStartTime[5];
	HINT32				offsetMinTime;

	ItkFatal(info != NULL);

	ItkTrace((C_CODE_F_WHITE_B_PURPLE" index(%ld) eventIndex(%d)"C_CODE_RESET"\n", index, eventIndex));

	if (thiz == NULL)
	{
		ItkFatal(0);
		return ITKE_NOT_FOUND;
	}

	VK_CLOCK_GetTime(&ulCurtime);

	if (eventIndex > 1)
	{
		ItkAssert((" Not support schedule event!(%d) \r\n", eventIndex));
		return ITKE_NOT_FOUND;
	}

	/* get service handle */
	if (local_itk_SVC_GetSvcHandleBySvcIndex(index, &hSvc) != ERR_OK)
	{
		ItkAssert((" fail to get service handle by SvcIndex(%ld) \r\n",index));
		return ITKE_NOT_FOUND;
	}

	/* make Epg parameter as input */
	HxSTD_memset(&stEpgParam, 0x00, sizeof(SvcEpg_FindParam_t));

	/* @note For DVB EIT-based services it is expected that only EITp/f is monitored */
	stEpgParam.ulFindMask 	= EPG_FIND_MASK_PF | EPG_FIND_MASK_TIME;
	stEpgParam.hSvcHandle	= hSvc;

	stEpgParam.ulStartTime = ulCurtime;
	stEpgParam.ulEndTime = 0xFFFFFFFF;

	/* get event list */
	// TODO: epg 소스 정리로 새로 구현하세요..
	//if (SVC_EPG_GetEventList(&stEpgParam, &pEpgList) == ERR_OK)
	{
		HINT32		lStrLen = 0;
		HUINT32		ulIndex = 0;

		/* initiate output variable */
		HxSTD_memset(info, 0x00, sizeof(itk_service_event_t));
		pHeadEpgList = pEpgList;
		while (pEpgList)
		{
			pstEvent = (SvcEpg_Event_t*)UTIL_LINKEDLIST_GetContents(pEpgList);

			if (ulIndex == 0)
			{
				pstPresent = pstEvent;
			}
			else if (ulIndex == 1)
			{
				pstNext = pstEvent;
			}
			else
			{
				break;
			}

			pEpgList = pEpgList->pNext;
			ulIndex++;
		}

		/*
			젠장.......
			P/F를 요청했는데 event가 1개 일 경우 이게 P인지 F인지 구분해야 한다
		*/
		if ((pstPresent != NULL) && (pstNext == NULL))
		{
			/* Case 1 : 현재 시간이 present시작시간 이전인 경우. present없고 follow있는걸로 간주 */
			if (pstEvent->ulStartTime > ulCurtime)
			{
				pstNext = pstPresent;
				pstPresent = NULL;
				ItkTrace((" No present event! eventIndex(%d)\n", eventIndex));
			}
			/* Case 2 : 현재 시간이 present에 속해 있는 경우. 당연히 present있고 follow없는걸로 간주 */
			else if ((pstEvent->ulStartTime <= ulCurtime) && (ulCurtime <= (pstEvent->ulStartTime + pstEvent->ulDuration)))
			{
				/* do nothing */
			}
			/* Case 3 : 현재 시간이 present끝난 이후 시간에 속하는 경우. present없고 follow있는걸로 간주 */
			else if (ulCurtime > (pstEvent->ulStartTime + pstEvent->ulDuration))
			{
				pstNext = pstPresent;
				pstPresent = NULL;
				ItkTrace((" No present event! eventIndex(%d)\n", eventIndex));
			}
		}

		if (eventIndex == 0)
		{
			pstEvent = pstPresent;
		}
		else
		{
			pstEvent = pstNext;
		}

		if (pstEvent != NULL)
		{
			/* current present event */
			info->version = pstEvent->stEventInfo.stEitEventInfo.ucVersion;
			info->type	= ITK_SERVICE_EV_EIT;

			// TODO: EPG 소스 정리로 인해 새로 구현하세요.
			//SVC_EPG_ConvertEpgTextToString(pstEvent->astShortText[0].pEventName, &pucConvertString);
			lStrLen = MWC_UTIL_DvbStrlen(pucConvertString);
			if (pstEvent->stEventInfo.stEitEventInfo.bwEitDesMask & eEPG_DES_SHORT_EVENT)
			{
				/* EVENT NAME */
				if (lStrLen > 0)
				{
					pucConvertString2 = local_itk_SVC_StrTrimCharcode(pucConvertString, lStrLen);
					/* See @ref strfmt for details of the format of the string */
					HLIB_STD_StrUtf8NCpy(info->event_info.eit.event_name, pucConvertString2, ITK_SERVICE_INFO_MAX_STR);
				}
				// TODO: EPG 소스 정리로 인해 새로 구현하세요.
//				SVC_EPG_ConvertEpgTextToString(pstEvent->astShortText[0].pEventText, &pucConvertString);
				lStrLen = MWC_UTIL_DvbStrlen(pucConvertString);
				/* SHORT EVENT DESCRIPTION */
				if (lStrLen > 0)
				{
					pucConvertString2 = local_itk_SVC_StrTrimCharcode(pucConvertString, lStrLen);
					/* See @ref strfmt for details of the format of the string */
					HLIB_STD_StrUtf8NCpy(info->event_info.eit.short_desc, pucConvertString2, ITK_SERVICE_INFO_MAX_STR);
				}
			}

			/* set content desciptor info */

			/* set time info */

			/* date in MJD */
			HLIB_DATETIME_ConvertUnixTimeToMJD(pstEvent->ulStartTime, (HUINT16*)&info->event_info.eit.date);

			HLIB_DATETIME_ConvertUnixTimeToMJDBcdTime(pstEvent->ulStartTime, aucStartTime);
			/* TIME만 */
			HLIB_DATETIME_ConvertBcdTimeToUnixTime(&aucStartTime[2], (UNIXTIME*)&info->event_info.eit.start);
			HLIB_DATETIME_GetTimeOffsetWithDst(&offsetMinTime);
			/*  Start time in seconds from midnight (UTC + local offset) */
			info->event_info.eit.start += (offsetMinTime * SECONDS_PER_MIN);

			/* duration in seconds */
			info->event_info.eit.duration = pstEvent->ulDuration;

			/* set parent rating !!! multiple rating일경우 고려 필요 */
			if (pstEvent->stEventInfo.stEitEventInfo.bwEitDesMask & eEPG_DES_PARENTAL_RATING)
			{
				info->event_info.eit.rating = pstEvent->stEventInfo.stEitEventInfo.astParentalRating[0].ucParentalRating;
			}

			/* set cas option */
			info->event_info.eit.encrypted = pstEvent->u1FreeCaMode ? ITK_NOT_PRESENT : ITK_PRESENT;

			/* content type */
			/* Australia는 content nibble level1만 사용 */
			info->event_info.eit.content_type = (pstEvent->aucContent[0] >> 4 ) & 0x0F;

			ItkTrace((" eventIndex(%d) eventName(%s)\n", eventIndex, info->event_info.eit.event_name));

			eErr = ITKE_OK;
		}
		else
		{
			eErr = ITKE_NOT_FOUND;
			ItkAssert((" NO P/F! eventIndex(%d), hSvc(0x%x)\n", eventIndex, hSvc));
		}

		SVC_EPG_ReleaseEventList(pHeadEpgList);
	}
	else
	{
		eErr = ITKE_NOT_FOUND;
		ItkAssert((" NO P/F! eventIndex(%d), hSvc(0x%x)\n", eventIndex, hSvc));
	}
#endif
#else
	HxLOG_Error("[%s:%d] Not Supported\n", __FUNCTION__, __LINE__);
	eErr = ITKE_NOT_FOUND;
#endif

	return eErr;
}

/*
 * The flag to display info indicates that the tuning should be performed with presentation
 * of all information so the viewer is aware of the change. This means that identification
 * should be displayed (banner on the new channel, service name, now/next, usually presented
 * during a standard service tune).
 */
itk_errcode_t mheg_itk_port_svcStartRetune(struct itk_service_t* thiz, itk_int32_t index, itk_service_tune_flag_t flags)
{
	if(thiz == NULL)
	{
		HxLOG_Error(" ERROR : input parameter is invalid !!! \n");
		return ITKE_NOT_FOUND;
	}

	if(ERR_OK != PORT_ITK_SVC_Retune(index, (HUINT32)flags))
	{
		return ITKE_NOT_FOUND;
	}

	return ITKE_OK;
}

/*
	DB field에 group string이 MAX 64로 정의 되었으며, 하나의 그룹은 4byte로 구성된다. 그래서 일단 group max를 16으로 한다
*/
#define ITK_FREESAT_MAX_GROUP_NUM	16

/*
* An application may use the return channel if the current service is a member of a service group
* with the return channel flag set. If the service is a member of several groups, only one group
* needs to have the flag set to allow use of the return channel.
*/
itk_errcode_t mheg_itk_port_svcCheckMhegIC(struct itk_service_t *thiz, itk_int32_t index)
{
#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
#if	1
	MHEG_NOT_IMPLEMENTED;
#else
	/* stack을 좀 많이 쓴다...... */
	Handle_t		hSvc = HANDLE_NULL,
				hAction;
	DbSvc_Info_t 	stSvcInfo;
	HUINT16		usTsUniqId;
	HBOOL		bFound = FALSE;
	HUINT16		groupList[ITK_FREESAT_MAX_GROUP_NUM];
	HUINT32		groupCnt;
	int			i = 0;
	DbSvc_GroupInfo_t	svcGroupInfo;
	HERROR		nErr;

	if (index == -1)
	{
		ItkAssert((" invalid service index(%ld)\n", index));
		return ITKE_NOT_FOUND;
	}

	nErr = local_itk_SVC_GetSvcHandleBySvcIndex(index, &hSvc);
	if (nErr != ERR_OK)
	{
		ItkAssert((" fail to retrieve Svc Handle by index(%ld)\n", index));
		return ITKE_NOT_FOUND;
	}

	nErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
	if (nErr != ERR_OK)
	{
		ItkAssert((" fail to retrieve SvcInfo By hSvc(0x%x)\n", hSvc));
		return ITKE_NOT_FOUND;
	}

	nErr = DB_SVC_GetUniqueIdFromSvcHandle(hSvc, &usTsUniqId);
	if(nErr != ERR_OK)
	{
		ItkAssert(("fail to retrieve TsInfo By hSvc(0x%x)\n", hSvc));
		return ITKE_NOT_FOUND;
	}

	/* get action handle */
	PORT_ITK_GetMastActionHandle(&hAction);

	HxSTD_memset(groupList, 0, sizeof(HUINT16) * ITK_FREESAT_MAX_GROUP_NUM);

	nErr = DB_SVC_GetGroupIndexArrayInSvcInfo(&stSvcInfo, ITK_FREESAT_MAX_GROUP_NUM, &groupCnt, groupList);
	if(nErr != ERR_OK)
	{
		ItkAssert(("fail to retrieve GroupList By hSvc(0x%x)\n", hSvc));
		return ITKE_NOT_FOUND;
	}

	for (i = 0; i < groupCnt && i < ITK_FREESAT_MAX_GROUP_NUM; i++)
	{
		nErr = DB_SVC_GetGroupInfo(groupList[i], &svcGroupInfo);
		if ((nErr == ERR_OK) && (DB_SVC_GroupGetReturnChannelAccessFlag(svcGroupInfo) == TRUE))
		{
			bFound = TRUE;
			break;
		}
	}

// rjlee. will be remove.
	ItkAssert((C_CODE_F_BLACK_B_YELLOW"\t index(%ld) bIC(%d)"C_CODE_RESET"\n", index, bFound));

	if (bFound == TRUE)
		return ITKE_OK;
	else
		return ITKE_FAIL;
#endif
#elif defined (CONFIG_3RD_MHEG_UK_PROFILE_HD) || defined (CONFIG_3RD_MHEG_AUSTRALIA_PROFILE)

	Handle_t	hSvc = HANDLE_NULL;
//	HERROR		nErr;

	if (thiz == NULL)
	{
		HxLOG_Error("ERROR : thiz is NULL !\n");
		return ITKE_FAIL;
	}

	if (index == -1)
	{
		HxLOG_Error(" invalid service index(%ld)\n", index);
		return ITKE_NOT_FOUND;
	}

	/* get service handle by service index retrived by function findService */
	hSvc = (Handle_t)index;

	return ITKE_OK;
#else
	HxLOG_Error("[%s:%d] Not Supported\n", __FUNCTION__, __LINE__);
	return ITKE_NOT_SUPPORTED;
#endif
}

itk_errcode_t mheg_itk_port_svcCheckNonDestructiveTune(struct itk_service_t *thiz, itk_int32_t index1, itk_int32_t index2)
{
#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
#if 1
	MHEG_NOT_IMPLEMENTED;
#else
	/* stack을 좀 많이 쓴다...... */
	Handle_t		hSvc1 = HANDLE_NULL,
				hSvc2 = HANDLE_NULL,
				hAction;
	DbSvc_Info_t 	stSvcInfo1,
				stSvcInfo2;
	DbSvc_GroupInfo_t	svcGroupInfo;
	HUINT16		usTsUniqId1,
				usTsUniqId2;
	HBOOL		bFound = FALSE;
	HUINT16		groupList1[ITK_FREESAT_MAX_GROUP_NUM],
				groupList2[ITK_FREESAT_MAX_GROUP_NUM];
	HUINT32		groupCnt1,
				groupCnt2;
	int			i = 0, j = 0;
	HERROR		nErr;

	ItkTrace((" index1(%ld), index2(%ld) \n", index1, index2));

	if (index1 == -1 || index2 == -1 || index1 == index2)
	{
		ItkAssert((" invalid service index1(%ld) index2(%ld)\n", index1, index2));
		return ITKE_NOT_FOUND;
	}

	nErr = local_itk_SVC_GetSvcHandleBySvcIndex(index1, &hSvc1);
	nErr |= local_itk_SVC_GetSvcHandleBySvcIndex(index2, &hSvc2);
	if (nErr != ERR_OK)
	{
		ItkAssert((" fail to retrieve Svc Handle by index1(%ld) index2(%ld)\n", index1, index2));
		return ITKE_NOT_FOUND;
	}

	nErr = DB_SVC_GetServiceInfo(hSvc1, &stSvcInfo1);
	nErr |= DB_SVC_GetServiceInfo(hSvc2, &stSvcInfo2);
	if (nErr != ERR_OK)
	{
		ItkAssert((" fail to retrieve SvcInfo By hSvc1(0x%x) hSvc2(0x%x)\n", hSvc1, hSvc2));
		return ITKE_NOT_FOUND;
	}

	nErr = DB_SVC_GetUniqueIdFromSvcHandle(hSvc1, &usTsUniqId1);
	nErr |= DB_SVC_GetUniqueIdFromSvcHandle(hSvc2, &usTsUniqId2);
	if(nErr != ERR_OK)
	{
		ItkAssert(("fail to retrieve TsInfo By hSvc1(0x%x) hSvc2(0x%x)\n", hSvc1, hSvc2));
		return ITKE_NOT_FOUND;
	}

	/* get action handle */
	PORT_ITK_GetMastActionHandle(&hAction);

	HxSTD_memset(groupList1, 0, sizeof(HUINT16) * ITK_FREESAT_MAX_GROUP_NUM);
	HxSTD_memset(groupList2, 0, sizeof(HUINT16) * ITK_FREESAT_MAX_GROUP_NUM);

	nErr = DB_SVC_GetGroupIndexArrayInSvcInfo(&stSvcInfo1, ITK_FREESAT_MAX_GROUP_NUM, &groupCnt1, groupList1);
	nErr |= DB_SVC_GetGroupIndexArrayInSvcInfo(&stSvcInfo2, ITK_FREESAT_MAX_GROUP_NUM, &groupCnt2, groupList2);
	if(nErr != ERR_OK)
	{
		ItkAssert(("fail to retrieve GroupList! groupCnt1(%d) groupCnt2(%d)\n", groupCnt1, groupCnt2));
		return ITKE_NOT_FOUND;
	}

	for (i = 0; i < groupCnt1 && i < ITK_FREESAT_MAX_GROUP_NUM; i++)
	{
		nErr = DB_SVC_GetGroupInfo(groupList1[i], &svcGroupInfo);
		if ((nErr == ERR_OK) && (DB_SVC_GroupGetNoneDestructiveTuneFalg(svcGroupInfo) == TRUE))
		{
			for (j = 0; j < groupCnt2 && j < ITK_FREESAT_MAX_GROUP_NUM; j++)
			{
				nErr = DB_SVC_GetGroupInfo(groupList2[j], &svcGroupInfo);
				if ((nErr == ERR_OK) && (DB_SVC_GroupGetNoneDestructiveTuneFalg(svcGroupInfo) == TRUE))
				{
					/* Group ID가 속한 Group Index가 동일하면 성공 */
					if (groupList1[i] == groupList2[j])
					{
						bFound = TRUE;
						break;
					}
				}
			}
		}

		if (bFound == TRUE)
			break;
	}

// rjlee. will be remove.
	ItkAssert((C_CODE_F_BLACK_B_YELLOW"\t index1(%ld) index2(%ld) bNonDestructive(%d)"C_CODE_RESET"\n", index1, index2, bFound));

	if (bFound == TRUE)
		return ITKE_OK;
	else
		return ITKE_FAIL;
#endif
#else
	HxLOG_Error("[%s:%d] Not Supported\n", __FUNCTION__, __LINE__);
	return ITKE_NOT_SUPPORTED;
#endif
}

itk_errcode_t mheg_itk_port_svcCheckMhegEnabled(struct itk_service_t *thiz, itk_int32_t index)
{
	/*
	* The private descriptor called the free satellite interactive restriction descriptor provides a
	* mechanism to disable use of the MHEG interactive features for a list of services. The descriptor
	* is only likely to be used in exceptional circumstance to protect the receiver population from a
	* faulty interactive MHEG application broadcast on a particular service. Receivers shall support
	* the use of this descriptor.
	*/

#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
#if	1
	MHEG_NOT_IMPLEMENTED;
#else
	Handle_t		hSvc = HANDLE_NULL, hAction;
	DbSvc_Info_t 	stSvcInfo;
	HUINT16		usTsUniqId;
	HBOOL		bRestrict = FALSE;
	HERROR		nErr;

	ItkTrace((" index (%ld) \n", index));

	nErr = local_itk_SVC_GetSvcHandleBySvcIndex(index, &hSvc);
	if (nErr != ERR_OK)
	{
		ItkAssert((" fail to retrieve Svc Handle by SvcIndex(%ld)\n",index));
		return ITKE_NOT_FOUND;
	}

	nErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
	if (nErr != ERR_OK)
	{
		ItkAssert((" fail to retrieve SvcInfo By hSvc(0x%x)\n", hSvc));
		return ITKE_NOT_FOUND;
	}

	nErr = DB_SVC_GetUniqueIdFromSvcHandle(hSvc, &usTsUniqId);
	if(nErr != ERR_OK)
	{
		ItkAssert(("fail to retrieve TsInfo By hSvc(0x%x)\n", hSvc));
		return ITKE_NOT_FOUND;
	}

	/* get action handle */
	PORT_ITK_GetMastActionHandle(&hAction);

	/*
		1. 지상파 일때 Action handle에 의해 SI module에서 return error일것이므로 별도 mode check불필요
		2. Freesat일때 BAT수신전이면 return error일 것이므로 무조건 ITKE_OK가 된다. 체크 필요.
		3. non-restrict 채널이면 return error이다
	*/
	nErr = XSVC_SI_MHEG_GetServiceInteractiveRestrictInfo(hAction, usTsUniqId, stSvcInfo.usOnId, stSvcInfo.usTsId, stSvcInfo.usSvcId, &bRestrict);

// rjlee. will be remove.
	ItkAssert((C_CODE_F_BLACK_B_YELLOW"\tbRestrict(%d) hSvc(0x%x) usTsUniqId(0x%x) onID(0x%x) tsID(0x%x) svcID(0x%x) group(%s) svcinfo size(%d)"C_CODE_RESET"\n",
			bRestrict,
			hSvc, usTsUniqId, stSvcInfo.usOnId, stSvcInfo.usTsId, stSvcInfo.usSvcId, stSvcInfo.szGroupList, sizeof(DbSvc_Info_t)));

	if ((nErr == ERR_OK) && (bRestrict == TRUE))
	{
		ItkAssert((" mheg restrict service! So mheg will not start!\n"));
		return ITKE_FAIL;
	}
	else
	{
		ItkData(("fail to retrieve Freesat restrict info(0x%x), but return value will be ok.\n", hSvc));
		return ITKE_OK;
	}
#endif
#else
	HxLOG_Error("[%s:%d] Not Supported\n", __FUNCTION__, __LINE__);
	return ITKE_OK;
#endif
}

/*
	service_index는 mheg의 service list의 index이다
*/
itk_int32_t mheg_itk_port_svcGetFreesatServiceIdentifier(struct itk_service_t * thiz, itk_int32_t service_index)
{
#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
	DxService_t		*pstSvcInfo;
	HINT32			nSvcId;

	pstSvcInfo = APK_META_SVC_GetService(service_index);
	if (pstSvcInfo == NULL)
	{
		HxLOG_Error("Invalid service index (%d)\n", service_index);
		return ITKE_NOT_FOUND;
	}

	svcId = pstSvcInfo->ex.freesat.svcId;

	APK_META_SVC_Delete(pstSvcInfo);

	return (itk_int32_t)nSvcId;
#else
	HxLOG_Error("[%s:%d] Not Supported\n", __FUNCTION__, __LINE__);
	return -1;
#endif
}

/*
	freesat_service_index는 freesat service identifier이다
*/
itk_errcode_t mheg_itk_port_svcCheckGroupMembership(struct itk_service_t *thiz, itk_uint16_t freesat_service_index, itk_uint16_t freesat_service_group)
{
#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
#if 1
	MHEG_NOT_IMPLEMENTED;
#else
	HERROR nErr;
	itk_errcode_t nResult = ITKE_FAIL;
	DbSvc_Info_t 	stSvcInfo;
	HUINT32 ulSvcCount = 0;
	Handle_t *phSvcList = NULL;
	HUINT16		groupList1[ITK_FREESAT_MAX_GROUP_NUM];
	HUINT32		groupCnt1;
	DbSvc_GroupInfo_t	svcGroupInfo;
	int i;

// rjlee. will be remove.
	ItkAssert((C_CODE_F_BLACK_B_YELLOW"\t freesat_service_index(0x%x) freesat_service_group(0x%x)"C_CODE_RESET"\n", freesat_service_index, freesat_service_group));

	nErr = XDB_SVC_FindServiceListByFsatIdf_Freesat(eDxDELIVERY_TYPE_SAT, eSvcType_All, eLcnVisibleSvc_FindAll, freesat_service_index, &ulSvcCount, &phSvcList);
	if ((nErr != ERR_OK) || (ulSvcCount == 0) || (phSvcList == NULL))
	{
		ItkAssert((" Cannot find the Fsi(0x%x)\n", freesat_service_index));
		goto _itk_svc_checkgroup_err;
	}

	/*
		freesat service id는 unique하지만 LCN중복인 서비스들이 존재하여 여러개의 service handle이 검색될수 있다
		제일 첫번째놈 사용
	*/
	nErr = DB_SVC_GetServiceInfo(phSvcList[0], &stSvcInfo);
	if (nErr != ERR_OK)
	{
		ItkAssert((" fail to retrieve SvcInfo By hSvc(0x%x)\n", phSvcList[0]));
		goto _itk_svc_checkgroup_err;
	}

	/*
		service info를 이용해서 group list를 구한다
	*/
	nErr = DB_SVC_GetGroupIndexArrayInSvcInfo(&stSvcInfo, ITK_FREESAT_MAX_GROUP_NUM, &groupCnt1, groupList1);
	if(nErr != ERR_OK)
	{
		ItkAssert(("fail to retrieve GroupList By hSvc1(0x%x)\n", phSvcList[0]));
		goto _itk_svc_checkgroup_err;
	}

	for (i = 0; i < groupCnt1; i++)
	{
		nErr = DB_SVC_GetGroupInfo(groupList1[i], &svcGroupInfo);
		if ((nErr == ERR_OK) && svcGroupInfo.ulGroupId == freesat_service_group)
		{
			nResult = ITKE_OK;
			break;
		}
	}

_itk_svc_checkgroup_err:

	if (phSvcList)
	{
		DB_SVC_FreeServiceList (phSvcList);
	}

	return nResult;
#endif
#else
	HxLOG_Error("[%s:%d] Not Supported\n", __FUNCTION__, __LINE__);
	return -1;
#endif
}

size_t mheg_itk_port_svcGetSvcNvStorageAlloc(struct itk_service_t * thiz, itk_uint16_t freesat_service_index)
{
#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
#if 1
	MHEG_NOT_IMPLEMENTED;
#else
	HERROR nErr;
	HUINT16 usTsUniqId;
	HUINT32 ulSvcCount = 0;
	Handle_t *phSvcList = NULL, hAction;
	HUINT32 unNvAllocSize = 0;

// rjlee. will be remove.
	ItkAssert((C_CODE_F_BLACK_B_YELLOW"\t freesat_service_index(0x%x)"C_CODE_RESET"\n", freesat_service_index));

	/*
		freesat identifier로 service handle을 구한다
	*/
	nErr = XDB_SVC_FindServiceListByFsatIdf_Freesat(eDxDELIVERY_TYPE_SAT, eSvcType_All, eLcnVisibleSvc_FindAll, freesat_service_index, &ulSvcCount, &phSvcList);
	if ((nErr != ERR_OK) || (ulSvcCount == 0) || (phSvcList == NULL))
	{
		ItkAssert((" Cannot find the Fsi(0x%x)\n", freesat_service_index));
		goto _itk_svc_storagealloc_err;
	}

	/*
		구해진 service handle로 ts unique id를 구한다
	*/
	nErr = DB_SVC_GetUniqueIdFromSvcHandle(phSvcList[0], &usTsUniqId);
	if(nErr != ERR_OK)
	{
		ItkAssert(("fail to retrieve TsInfo By hSvc(0x%x)\n", phSvcList[0]));
		goto _itk_svc_storagealloc_err;
	}

	/* get action handle */
	PORT_ITK_GetMastActionHandle(&hAction);

	/*
		size는 error return되면 0이므로 굳이 error check하지 않는다
	*/
	XSVC_SI_MHEG_GetInteractiveStorageAllocationBySvcId(hAction, usTsUniqId, freesat_service_index, &unNvAllocSize);
_itk_svc_storagealloc_err:

	if (phSvcList)
	{
		DB_SVC_FreeServiceList (phSvcList);
	}

	return unNvAllocSize;
#endif
#else
	HxLOG_Error("[%s:%d] Not Supported\n", __FUNCTION__, __LINE__);
	return -1;
#endif
}

size_t mheg_itk_port_svcGetGroupNvStorageAlloc(struct itk_service_t * thiz, itk_uint16_t freesat_service_group)
{
#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
#if	1
	MHEG_NOT_IMPLEMENTED;
#else
	Handle_t hAction;
	HUINT16 usTsUniqId;
	HUINT32 unNvAllocSize = 0;

// rjlee. will be remove.
	ItkAssert((C_CODE_F_BLACK_B_YELLOW"\t freesat_service_group(0x%x)"C_CODE_RESET"\n", freesat_service_group));

	/* get action handle */
	PORT_ITK_GetMastActionHandle(&hAction);

	/* group id만으로 TS unique id를 구하기 애매하다. 일단 현재는, 아래 api가 ts unique id를 구분하지 않으므로 0으로 들고간다 */
	usTsUniqId = 0;

	XSVC_SI_MHEG_GetInteractiveStorageAllocationByGroupId(hAction, usTsUniqId, freesat_service_group, &unNvAllocSize);

	return unNvAllocSize;
#endif
#else
	HxLOG_Error("[%s:%d] Not Supported\n", __FUNCTION__, __LINE__);
	return -1;
#endif
}

/**
	itk_service.h - void (*setBroadcastInterruption)(struct itk_service_t *thiz, const itk_control_t control);

	Implement Reference : ITK_User_Guide (pdf, version 0.26)
	   - 12.3 Service Changes
	   - 12.4 Broadcast interruption
	   Related Keyword : ITK_TUNE_KEEP_BROADCAST_INTERRUPT (flag on eMEVT_MHEG_SVC_RETUNE)
*/
void mheg_itk_port_svcSetBroadcastInterruption(struct itk_service_t *thiz, const itk_control_t control)
{
	HUINT32			ulViewId;

	HxLOG_Trace();
	PORT_ITK_GetMastViewId(&ulViewId);

	printf("\033[30;43m[%s : %d]  MHEG : BROADCAST_INTERRUPT : %s  \033[0m\n", __FUNCTION__, __LINE__, ( (ITK_DISABLE == control) ? "Disable" : "Enable" ));
	PORT_ITK_PostMsg(eMHEG_APK_SET_BROADCAST_INTERRUPTION, ulViewId, (HINT32)control, 0, 0, 0);
}

void mheg_itk_port_svcGetBroadcastLocation(struct itk_service_t *thiz, itk_uint8_t *loc, itk_uint32_t loc_size)
{
	HxLOG_Trace();

	// TODO: for AU profile
	// BAT 의 country_availablility_descriptor 에서 country_code를 가져옴
}

itk_errcode_t mheg_itk_port_svcGetSvcLocator(struct itk_service_t* thiz, itk_int32_t index, itk_service_locator_t* loc)
{
	HINT32			nDefSvcUid, nCurSvcUid;
	HINT32			nSvcUid;
	DxService_t		*pstSvcInfo= NULL;
	DxTransponder_t	*pstTsInfo = NULL;
	DxNetwork_t		*pstNetworkInfo = NULL;

	//HxLOG_Trace();

	if(thiz == NULL)
	{
		HxLOG_Assert(0);
		return ITKE_NOT_FOUND;
	}

	if(index == -1)
	{
		/* get default service locator */
		PORT_ITK_GetSvcUid(&nDefSvcUid, &nCurSvcUid);

		nSvcUid = nDefSvcUid;
		HxLOG_Debug("Get default service uid (%p)\n", nDefSvcUid);
	}
	else
	{
		/* get service handle by service index retrived by function findService */
		nSvcUid = (Handle_t)index;
	}

	pstSvcInfo = APK_META_SVC_GetService(nSvcUid);
	if (pstSvcInfo == NULL)
	{
		HxLOG_Error("Fail to get service info (%p)\n", nSvcUid);
		return ITKE_NOT_FOUND;
	}

	pstTsInfo = APK_META_SVC_GetTransponder(pstSvcInfo->tsuid);
	if (pstTsInfo == NULL)
	{
		if(pstSvcInfo != NULL)
			APK_META_SVC_Delete(pstSvcInfo);

		HxLOG_Error("fail to retrieve TsInfo By nSvcUid(0x%x) \r\n", nSvcUid);
		return ITKE_NOT_FOUND;
	}

	loc->type			= ITK_LOCATOR_BCAST;		/* all services in DTT are ITK_LOCATOR_BCAST */
	loc->locator.bcast.onid	= pstTsInfo->onid;
	loc->locator.bcast.tsid	= pstTsInfo->tsid;
	loc->locator.bcast.sid	= pstSvcInfo->svcid;

	//	OnId와 NetId를 구분하는 부분이 존재하나OCTO 2.0에서 사용 여부를 몰라, 그냥 onid매핑.
	pstNetworkInfo = APK_META_SVC_GetNetwork(pstTsInfo->netuid);
	if (pstNetworkInfo)
		loc->locator.bcast.nid = pstNetworkInfo->onid;
	else
		loc->locator.bcast.nid = pstTsInfo->onid;

	HxLOG_Debug("onid(0x%x), tsid(0x%x), sid(0x%x), nid(0x%x) \n", loc->locator.bcast.onid, loc->locator.bcast.tsid,
						loc->locator.bcast.sid, loc->locator.bcast.nid);

	if(pstSvcInfo != NULL)
		APK_META_SVC_Delete(pstSvcInfo);
	if(pstTsInfo != NULL)
		APK_META_SVC_Delete(pstTsInfo);
	if(pstNetworkInfo != NULL)
		APK_META_SVC_Delete(pstNetworkInfo);

	return ITKE_OK;
}

itk_int32_t mheg_itk_port_svcGetServiceCount(struct itk_service_t * thiz)
{
	HxLOG_Assert(thiz);

	// TODO: ZA Profile

	return 0;
}

itk_int32_t mheg_itk_port_svcGetNextServiceIndex(struct itk_service_t *thiz, itk_int32_t index, itk_int8_t updown)
{
	HxLOG_Assert(thiz);

	// TODO: ZA Profile

	return 0;
}

itk_eits_iterator_t *mheg_itk_port_svcNewEitsIterator(struct itk_service_t *thiz, itk_int32_t index, itk_int32_t date, itk_int32_t start)
{
	HxLOG_Assert(thiz);

	// TODO: ZA Profile

	return (itk_eits_iterator_t*)NULL;
}

itk_validation_t mheg_itk_port_svcGetEventId(struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex, itk_uint16_t *eventId)
{
	HxLOG_Assert(thiz);

	// TODO: ZA Profile (out param : eventId)

	return ITK_VALID;
}

itk_uint16_t mheg_itk_port_svcGetEitPfNumComponentDescs(struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex)
{
	HxLOG_Assert(thiz);

	// TODO: ZA Profile

	return 0;
}

itk_validation_t mheg_itk_port_svcGetEitPfComponentDescs(struct itk_service_t *thiz, itk_int32_t index, itk_uint16_t eventIndex,
				                		                char *stream_content, itk_uint16_t scLength,
                				        		        char *component_type, itk_uint16_t ctLength)
{
	HxLOG_Assert(thiz);

	// TODO: ZA Profile (out param : stream_content, component_type)

	return ITK_VALID;
}

itk_present_t mheg_itk_port_svcGetEitPfComponentInfo(struct itk_service_t *thiz, itk_int32_t index,
											        itk_uint16_t eventIndex, itk_component_t component)
{
	HxLOG_Assert(thiz);

	// TODO: ZA Profile

	return ITK_PRESENT;
}

itk_errcode_t mheg_itk_port_svcAddEventCallback(struct itk_service_t *thiz, itk_service_event_callback_t callback, void *context)
{
	HxLOG_Trace();

	if (thiz == NULL)
	{
		HxLOG_Error("thiz is NULL\n");
		return ITKE_FAIL;
	}

	if (callback == NULL)
	{
		HxLOG_Error("callback is NULL\n");
		return ITKE_FAIL;
	}

	s_cbSvcEvent 		= callback;
	s_pContextSvcEvent 	= context;

	return ITKE_OK;
}

void mheg_itk_port_svcRemoveEventCallback(struct itk_service_t *thiz)
{
	if (thiz == NULL)
	{
		HxLOG_Error("thiz is NULL\n");
		return;
	}

	if (s_cbSvcEvent == NULL)
		HxLOG_Debug("Already been freed status\n");

	s_cbSvcEvent		= NULL;
	s_pContextSvcEvent	= NULL;
}

/*
 	The locator format may depend on the service type. The returned value is unique for the service
	and is 0 or greater. If the service is not visible then the value -1 is returned
*/
itk_int32_t mheg_itk_port_svcFindSvc(struct itk_service_t* thiz, const char* service)
{
	HINT32			slServiceIndex = -1;
	HINT32			nOnId, nTsId, nServiceId, nLcn;
	HUINT32			ulFsi;
	ITK_RefSvc_t		eRefSvc = eITK_REFSVC_UNKNOWN;

	//HxLOG_Trace();

	/* check validation of parameters */
	if(thiz == NULL || service == NULL)
	{
		HxLOG_Assert(0);
		return -1;
	}

	/* initiate variables */
	nOnId = nTsId = nServiceId = nLcn = ITK_SVC_INVALID_ID;

	/* get service reference data */
	HxLOG_Debug("svcFindSvc : [%s]\n", service);
	eRefSvc = port_itk_svc_getReferenceService(service, &nOnId, &nTsId, &nServiceId, &nLcn, &ulFsi);
	switch(eRefSvc)
	{
		case eITK_REFSVC_DVB:
			{
				HINT32	nSvcUid = 0;

				/*
				 * DVB format:
				 * "dvb://{onid}.[{tsid}].{sid}"
				 *
				 * {onid} is the Original Network Id
				 * {tsid} is the (optional) Transport Stream Id
				 * {sid} is the Service Id
				 *
				 * Each of {onid}, {tsid} and {sid} are represented with a hexadecimal
				 * number (case insensitive).
				 */
				 if(port_itk_svc_getSvcHandleByTripleIds(nOnId, nTsId, nServiceId, &nSvcUid) == ERR_OK)
				 {
				 	slServiceIndex = nSvcUid;
				 	HxLOG_Debug("Find : netId(0x%x) ,tsid(0x%x),  svcId(0x%x) slServiceIndex(%d) \r\n",nOnId, nTsId, nServiceId, slServiceIndex);
				 }
				 else
				 {
				 	slServiceIndex = -1;
				 }
			}
			break;

		case eITK_REFSVC_DEF:
			/*
			 * Receiver format:
			 * "rec://svc/def"
			 *
			 * This string identifies the service currently tuned to. This ignores which current avstream
			 * is being decoded.
			 */
			if (mheg_itk_port_svcGetDefaultSvcUid(&slServiceIndex) != ERR_OK)
			{
				HxLOG_Error("Fail to get default service from default\n");
				slServiceIndex = -1;
			}
			break;

		case eITK_REFSVC_CUR:
			 /* "rec://svc/cur"
			 *
			 * This string identifies the service that the display is currently displaying (which may be
			 * different if the display is currently showing a different service).
			 */
			if(mheg_itk_port_svcGetCurrentSvcUid(&slServiceIndex) != ERR_OK)
			{
				HxLOG_Error("fail to get Cur service index from Default Service \r\n");
				slServiceIndex = -1;
			}
			break;

		case eITK_REFSVC_LCN:
			{
				/*
				 * LCN format:
				 * "rec://svc/lcn/{lcn}"
				 *
				 * {lcn} is the Logical Channel Number, as a decimal integer.
				 */
				 HINT32	nSvcUid = 0;

				 if(mheg_itk_port_svcGetSvcUidByLcn(nLcn, &nSvcUid) == ERR_OK)
				 {
				 	slServiceIndex = nSvcUid;
				 }
				 else
				 {
				 	HxLOG_Error("Error : by lcn(%d) \r\n",nLcn);
				 	slServiceIndex = -1;
				 }
			}
			break;

		case eITK_REFSVC_FSI:
#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
			{

				/*
				 * Additionaly the UK freesat profile defines the Freesat Service Identifier format and a "previous"
				 * service.
				 *
				 * FSI format:
				 * "rec://svc/fsi/{fsi}
				 *
				 * {fsi} is the Freesat Service Identifier, as a decimal integer.
				 *
				 * "rec://svc/prev"
				 *
				 * This string identifies the service that was being viewed before the current PVR event was
				 * watched. Note that it is required only for hosts implementing PVR extensions. If there
				 * was no previous service then the index value is -1.
				 *
				 * It is possible that a stream reference may not be found by the host (because the service
				 * is not visible in the geographical region). In this case the service index value is -1. Also
				 * some of the references may resolve to multiple services (for instance LCN references). In
				 * this case the choice of service index to return will vary depending on rules defined for the
				 * target market - consult receiver requirements documents for that market for more details.
				 */
				HxLOG_Critical("[%s:%d] Not implemented....\n", __FUNCTION__, __LINE__);
				HxLOG_Assert(0);
#if 0
				HERROR	nErr;
				HUINT32 ulSvcCount = 0;
				Handle_t *phSvcList = NULL;

				nErr = XDB_SVC_FindServiceListByFsatIdf_Freesat(eDxDELIVERY_TYPE_SAT, eSvcType_All, eLcnVisibleSvc_FindAll, (HUINT16)ulFsi, &ulSvcCount, &phSvcList);
				if ((nErr != ERR_OK) || (ulSvcCount == 0) || (phSvcList == NULL))
				{
					ItkAssert((" Cannot find the Fsi(0x%x)\n", ulFsi));
					slServiceIndex = -1;
				}
				else
				{
					slServiceIndex = local_itk_SVC_GetSvcIndexBySvcHandle(phSvcList[0]);
				}

				if (phSvcList)
				{
					DB_SVC_FreeServiceList (phSvcList);
				}
#endif
			}
#endif
			break;

		case eITK_REFSVC_PREV:
		case eITK_REFSVC_PVR:
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			HxLOG_Debug("do not support ref format(%d) \r\n", eRefSvc);
			break;

		case eITK_REFSVC_UNKNOWN:
			HxLOG_Debug("Unknown ref format(%d) \r\n", eRefSvc);
			//HxLOG_Assert(0);
		default:
			break;

	}

	HxLOG_Print(C_CODE_F_BLACK_B_CYAN" service(%s) index(%d)"C_CODE_RESET"\n", service, slServiceIndex);

	return slServiceIndex;
}

#define _____PUBLIC_APIs________________________________________________________________

void PORT_ITK_SVC_Init(void)
{
	HINT32		slErr = 0;

	s_cbSvcEvent 		= NULL;
	s_pContextSvcEvent = NULL;

	slErr = HxSEMT_Create(&s_ulSvcIdxSema, "ItvSvc1", HxSEMT_PRIORITY);
	HxLOG_Assert(slErr == ERR_OK);
}

void PORT_ITK_SVC_CreateObject(redkey_profile_t eItkProfile)
{
	HxLOG_Error(C_CODE_F_WHITE_B_BLUE"Service Object Profile (%d) !!!"C_CODE_RESET"\n", eItkProfile);

	s_stMhegServiceInfo.itkProfile = eItkProfile;

	s_stMhegServiceInfo.object.addEventCallback					= mheg_itk_port_svcAddEventCallback;
	s_stMhegServiceInfo.object.removeEventCallback				= mheg_itk_port_svcRemoveEventCallback;
	s_stMhegServiceInfo.object.findService						= mheg_itk_port_svcFindSvc;
	s_stMhegServiceInfo.object.getServiceLocator				= mheg_itk_port_svcGetSvcLocator;
	s_stMhegServiceInfo.object.getServiceInfo					= mheg_itk_port_svcGetSvcInfo;
	s_stMhegServiceInfo.object.getServiceEventInfo				= mheg_itk_port_svcGetSvcEventInfo;
	s_stMhegServiceInfo.object.startRetune						= mheg_itk_port_svcStartRetune;
	s_stMhegServiceInfo.object.checkNonDestructiveTuneAllowed	= mheg_itk_port_svcCheckNonDestructiveTune;
	s_stMhegServiceInfo.object.checkMhegICEnabled				= mheg_itk_port_svcCheckMhegIC;
	s_stMhegServiceInfo.object.checkMhegEnabled					= mheg_itk_port_svcCheckMhegEnabled;
	s_stMhegServiceInfo.object.getFreeSatServiceIdentifier		= mheg_itk_port_svcGetFreesatServiceIdentifier;
	s_stMhegServiceInfo.object.checkGroupMembership				= mheg_itk_port_svcCheckGroupMembership;
	s_stMhegServiceInfo.object.getServiceNvStorageAllocation		= mheg_itk_port_svcGetSvcNvStorageAlloc;
	s_stMhegServiceInfo.object.getGroupNvStorageAllocation		= mheg_itk_port_svcGetGroupNvStorageAlloc;
	s_stMhegServiceInfo.object.setBroadcastInterruption			= mheg_itk_port_svcSetBroadcastInterruption;
	s_stMhegServiceInfo.object.getBroadcastLocation				= mheg_itk_port_svcGetBroadcastLocation;
	s_stMhegServiceInfo.object.getServiceCount					= mheg_itk_port_svcGetServiceCount;
	s_stMhegServiceInfo.object.getNextServiceIndex				= mheg_itk_port_svcGetNextServiceIndex;
	s_stMhegServiceInfo.object.newEitsIterator					= mheg_itk_port_svcNewEitsIterator;
	s_stMhegServiceInfo.object.getEventId						= mheg_itk_port_svcGetEventId;
	s_stMhegServiceInfo.object.getEitpfNumComponentDescs		= mheg_itk_port_svcGetEitPfNumComponentDescs;
	s_stMhegServiceInfo.object.getEitpfComponentDescs			= mheg_itk_port_svcGetEitPfComponentDescs;
	s_stMhegServiceInfo.object.getEitpfComponentInfo			= mheg_itk_port_svcGetEitPfComponentInfo;

	// according to each profile, set NULL to usused function
	if ( (eItkProfile == REDKEY_PROFILE_UK_HD)
		|| (eItkProfile == REDKEY_PROFILE_INTERNAL5) )
	{
		s_stMhegServiceInfo.object.getServiceInfo					= NULL; //mheg_itk_port_svcGetSvcInfo;
		s_stMhegServiceInfo.object.getServiceEventInfo				= NULL; //mheg_itk_port_svcGetSvcEventInfo;
		s_stMhegServiceInfo.object.checkNonDestructiveTuneAllowed	= NULL; //mheg_itk_port_svcCheckNonDestructiveTune;
		s_stMhegServiceInfo.object.checkMhegEnabled					= NULL; //mheg_itk_port_svcCheckMhegEnabled;
		s_stMhegServiceInfo.object.getFreeSatServiceIdentifier		= NULL; //mheg_itk_port_svcGetFreesatServiceIdentifier;
		s_stMhegServiceInfo.object.checkGroupMembership				= NULL; //mheg_itk_port_svcCheckGroupMembership;
		s_stMhegServiceInfo.object.getServiceNvStorageAllocation		= NULL; //mheg_itk_port_svcGetSvcNvStorageAlloc;
		s_stMhegServiceInfo.object.getGroupNvStorageAllocation		= NULL; //mheg_itk_port_svcGetGroupNvStorageAlloc;
		s_stMhegServiceInfo.object.getBroadcastLocation				= NULL; //;
		s_stMhegServiceInfo.object.getServiceCount					= NULL;
		s_stMhegServiceInfo.object.getNextServiceIndex				= NULL;
		s_stMhegServiceInfo.object.newEitsIterator					= NULL;
		s_stMhegServiceInfo.object.getEventId						= NULL;
		s_stMhegServiceInfo.object.getEitpfNumComponentDescs			= NULL;
		s_stMhegServiceInfo.object.getEitpfComponentDescs			= NULL;
		s_stMhegServiceInfo.object.getEitpfComponentInfo				= NULL;
	}
	else if ( (eItkProfile == REDKEY_PROFILE_UK_FSAT)
			|| (eItkProfile == REDKEY_PROFILE_FREESATG2) )
	{
		s_stMhegServiceInfo.object.getServiceInfo					= NULL;
		s_stMhegServiceInfo.object.getServiceEventInfo				= NULL;
		s_stMhegServiceInfo.object.getBroadcastLocation				= NULL;
		s_stMhegServiceInfo.object.getServiceCount					= NULL;
		s_stMhegServiceInfo.object.getNextServiceIndex				= NULL;
		s_stMhegServiceInfo.object.newEitsIterator					= NULL;
		s_stMhegServiceInfo.object.getEventId						= NULL;
		s_stMhegServiceInfo.object.getEitpfNumComponentDescs			= NULL;
		s_stMhegServiceInfo.object.getEitpfComponentDescs			= NULL;
		s_stMhegServiceInfo.object.getEitpfComponentInfo				= NULL;
	}
	else if (eItkProfile == REDKEY_PROFILE_AUSTRALIA)
	{
		s_stMhegServiceInfo.object.checkNonDestructiveTuneAllowed	= NULL; //mheg_itk_port_svcCheckNonDestructiveTune;
		s_stMhegServiceInfo.object.checkMhegEnabled					= NULL; //mheg_itk_port_svcCheckMhegEnabled;
		s_stMhegServiceInfo.object.getFreeSatServiceIdentifier		= NULL; //mheg_itk_port_svcGetFreesatServiceIdentifier;
		s_stMhegServiceInfo.object.checkGroupMembership				= NULL; //mheg_itk_port_svcCheckGroupMembership;
		s_stMhegServiceInfo.object.getServiceNvStorageAllocation		= NULL; //mheg_itk_port_svcGetSvcNvStorageAlloc;
		s_stMhegServiceInfo.object.getGroupNvStorageAllocation		= NULL; //mheg_itk_port_svcGetGroupNvStorageAlloc;
		s_stMhegServiceInfo.object.setBroadcastInterruption			= NULL; //mheg_itk_port_svcSetBroadcastInterruption;
		s_stMhegServiceInfo.object.getBroadcastLocation				= NULL; //;
		s_stMhegServiceInfo.object.getServiceCount					= NULL;
		s_stMhegServiceInfo.object.getNextServiceIndex				= NULL;
		s_stMhegServiceInfo.object.newEitsIterator					= NULL;
		s_stMhegServiceInfo.object.getEventId						= NULL;
		s_stMhegServiceInfo.object.getEitpfNumComponentDescs			= NULL;
		s_stMhegServiceInfo.object.getEitpfComponentDescs			= NULL;
		s_stMhegServiceInfo.object.getEitpfComponentInfo				= NULL;
	}
	else if (eItkProfile == REDKEY_PROFILE_CIPLUS)
	{
		s_stMhegServiceInfo.object.getServiceInfo					= NULL;
		s_stMhegServiceInfo.object.getServiceEventInfo				= NULL;
		s_stMhegServiceInfo.object.startRetune						= NULL;
		s_stMhegServiceInfo.object.checkNonDestructiveTuneAllowed	= NULL;
		s_stMhegServiceInfo.object.checkMhegICEnabled				= NULL;
		s_stMhegServiceInfo.object.checkMhegEnabled					= NULL;
		s_stMhegServiceInfo.object.getFreeSatServiceIdentifier		= NULL;
		s_stMhegServiceInfo.object.checkGroupMembership				= NULL;
		s_stMhegServiceInfo.object.getServiceNvStorageAllocation		= NULL;
		s_stMhegServiceInfo.object.getGroupNvStorageAllocation		= NULL;
		s_stMhegServiceInfo.object.setBroadcastInterruption			= NULL;
		s_stMhegServiceInfo.object.getBroadcastLocation				= NULL;
		s_stMhegServiceInfo.object.getServiceCount					= NULL;
		s_stMhegServiceInfo.object.getNextServiceIndex				= NULL;
		s_stMhegServiceInfo.object.newEitsIterator					= NULL;
		s_stMhegServiceInfo.object.getEventId						= NULL;
		s_stMhegServiceInfo.object.getEitpfNumComponentDescs			= NULL;
		s_stMhegServiceInfo.object.getEitpfComponentDescs			= NULL;
		s_stMhegServiceInfo.object.getEitpfComponentInfo				= NULL;
	}
	else // TODO: for other profiles
	{

	}

	ITK_registerManager(ITK_SERVICE, (void*)&s_stMhegServiceInfo.object);
}

HERROR PORT_ITK_SVC_Start(void)
{
	HERROR		ulErr = ERR_OK;

	HxLOG_Debug(C_CODE_F_WHITE_B_BLUE" Start RED KEY !!!"C_CODE_RESET"\n");

	if(s_cbSvcEvent)
	{
		s_cbSvcEvent(s_pContextSvcEvent, &s_stMhegServiceInfo.object, ITK_SERVICE_TUNE_START);
	}
	else
	{
		HxLOG_Error(" CbSvcEvent has not been registered yet \r\n");
		ulErr = ERR_FAIL;
	}

	return ulErr;
}

HERROR PORT_ITK_SVC_Stop(itk_service_tune_event_t eTuneEvent)
{
	HERROR		ulErr = ERR_OK;

	HxLOG_Debug(C_CODE_F_WHITE_B_BLUE" Stop RED KEY !!!"C_CODE_RESET" eTuneEvent (%d)\n", eTuneEvent);

	if(s_cbSvcEvent)
	{
		s_cbSvcEvent(s_pContextSvcEvent, &s_stMhegServiceInfo.object, eTuneEvent);
	}
	else
	{
		HxLOG_Error(" CbSvcEvent has not been registered yet \r\n");
		ulErr = ERR_FAIL;
	}

	return ulErr;
}

ITK_RefSvc_t	port_itk_svc_getReferenceService(const HCHAR* pszSourceUrl, HINT32 *pnOnId, HINT32 *pnTsId, HINT32 *pnSvcId, HINT32 *pnLcn, HUINT32 *pulFsi)
{
	ITK_UrlData_t	stUrlField;
	ITK_RefSvc_t	eRefSvc = eITK_REFSVC_UNKNOWN;
	HINT32			slVal;

	if (HxSTD_StrEmpty(pszSourceUrl))
		return eRefSvc;

	if(mheg_itk_port_svcParseUrl((HCHAR*)pszSourceUrl, &stUrlField))
	{
		if (pszSourceUrl)
			HxLOG_Error(" invalid url (%s) \r\n", pszSourceUrl);
		return eRefSvc;
	}

	if (strncmp(stUrlField.scheme, "dvb", 3) == 0)
	{
		char *ptr;

		ptr = stUrlField.host;
		if (ptr == NULL)
		{
			return eRefSvc;
		}

		if (*ptr == '.')
		{
			return eRefSvc;
		}

		ptr = mheg_itk_port_svcStr2Hex(ptr, pnOnId);
		if (*ptr != '.')
		{
			return eRefSvc;
		}
		ptr += 1;

		//*pusOnid = 0x04d2;
		//*pusOnid = 0x003b;

		if (*ptr != '.')
		{
			ptr = mheg_itk_port_svcStr2Hex(ptr, pnTsId);
			if (*ptr != '.')
			{
				return eRefSvc;
			}
		}
		ptr += 1;

		//*pusTsid = 0x03e8;
		//*pusTsid = 0x090b;

		ptr = mheg_itk_port_svcStr2Hex(ptr, pnSvcId);
		if (*ptr != 0)
		{
			return eRefSvc;
		}

		eRefSvc = eITK_REFSVC_DVB;
	}
	else if (strncmp(stUrlField.scheme, "rec", 3) == 0)
	{
		if (strncmp(stUrlField.name, "def", 3) == 0)
		{
			eRefSvc = eITK_REFSVC_DEF;
		}
		else if (strncmp(stUrlField.name, "cur", 3) == 0)
		{
			eRefSvc = eITK_REFSVC_CUR;
		}
		else if (strncmp(stUrlField.name, "lcn", 3) == 0)
		{
			if (stUrlField.value == NULL)
			{
				return eRefSvc;
			}
			if (pnLcn)
			{
				slVal = atoi(stUrlField.value);
				if (slVal)
				{
					*pnLcn = slVal;
					eRefSvc = eITK_REFSVC_LCN;
				}
			}
		}
		else if (strncmp(stUrlField.name, "fsi", 3) == 0)
		{
			if (stUrlField.value == NULL)
			{
				return eRefSvc;
			}
			if (pulFsi)
			{
				slVal = atoi(stUrlField.value);
				if (slVal)
				{
					*pulFsi = slVal;
					eRefSvc = eITK_REFSVC_FSI;
				}
			}
		}
		else if (strncmp(stUrlField.name, "prev", 4) == 0)
		{
			eRefSvc = eITK_REFSVC_PREV;
		}
	}
	else if (strncmp(stUrlField.scheme, "PVR", 3) == 0)
	{
		eRefSvc = eITK_REFSVC_PVR;
	}
	else if (strncmp(stUrlField.scheme, "http", 4) == 0)
	{
		eRefSvc = eITK_REFSVC_HTTP;
	}
	else if (strncmp(stUrlField.scheme, "rtsp", 4) == 0)
	{
		eRefSvc = eITK_REFSVC_RTSP;
	}
	else if (strncmp(stUrlField.scheme, "https", 5) == 0)
	{
		eRefSvc = eITK_REFSVC_HTTP;
	}
	else if (strncmp(stUrlField.scheme, "rtsps", 5) == 0)
	{
		eRefSvc = eITK_REFSVC_RTSP;
	}

	mheg_itk_port_svcFreeUrlData(&stUrlField);

	return eRefSvc;
}

HERROR	port_itk_svc_getSvcHandleByTripleIds(HINT32 nOrgNetId, HINT32 nTsId, HINT32 nSvcId, HINT32 *pnSvcUid)
{
	HxVector_t	*pstSvcList;
	DxService_t	*pstSvcInfo;

	HxLOG_Debug("find svc by Tripleid (0x%x, 0x%x, 0x%x)\n", nOrgNetId, nTsId, nSvcId);
	pstSvcList = APK_META_SVC_FindServiceByTriplet(nOrgNetId, nTsId, nSvcId);
	if (pstSvcList)
	{
		pstSvcInfo = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, 0);
		*pnSvcUid = pstSvcInfo->uid;
		HLIB_VECTOR_Delete(pstSvcList);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR PORT_ITK_SVC_Retune(HINT32 slIndex, HUINT32 flag)
{
	HINT32			nSvcUid = slIndex;
	HINT32			nDefSvcUid, nCurSvcUid;
	HUINT32			ulViewId;
	DxService_t		*pstSvcInfo = NULL, *pstDefSvcInfo = NULL;
	HBOOL			bSameTs = TRUE;
	HUINT32			usDefTsIdx;

	PORT_ITK_GetMastViewId(&ulViewId);
	PORT_ITK_GetSvcUid(&nDefSvcUid, &nCurSvcUid);

	pstDefSvcInfo = APK_META_SVC_GetService(nDefSvcUid);
	if(pstDefSvcInfo == NULL)
	{
		HxLOG_Error("fail to get svcinfo by uid %p\n", nDefSvcUid);
		return ERR_FAIL;
	}
	usDefTsIdx = pstDefSvcInfo->tsuid;
	APK_META_SVC_Delete(pstDefSvcInfo);

	pstSvcInfo = APK_META_SVC_GetService(nSvcUid);
	if (pstSvcInfo == NULL)
	{
		HxLOG_Error("fail to get svcinfo by uid %p\n", nSvcUid);
		return ERR_FAIL;
	}

	if(pstSvcInfo->tsuid != usDefTsIdx)
	{
		bSameTs = FALSE;
	}

	if (pstSvcInfo->svcType == eDxSVC_TYPE_HIDDEN)
	{
		flag |= ITK_TUNE_KEEP_CONTEXT;			/* quiet tune */
	}
	APK_META_SVC_Delete(pstSvcInfo);

	HxLOG_Debug("Send eMHEG_SERVICE_CHANGE_SVC ======== \n");
	HxLOG_Debug("nSvcUid(0x%x), nDefSvcUid(0x%x) , nCurSvcUid(0x%x) , flag(%d) QuietTune(%d) \n",nSvcUid, nDefSvcUid, nCurSvcUid , flag, flag & ITK_TUNE_KEEP_CONTEXT);
	HxLOG_Debug("svcType(0x%x) V/A/P PID (0x%x, 0x%x, 0x%x), Ts idx/id (%d, 0x%x) Same Ts (%d) \n", pstSvcInfo->svcType, pstSvcInfo->videoPid, pstSvcInfo->audioPid, pstSvcInfo->pcrPid, pstSvcInfo->tsuid, pstSvcInfo->tsid, bSameTs);

	PORT_ITK_PostMsg(eMHEG_SERVICE_CHANGE_SVC, ulViewId, nSvcUid, flag, bSameTs, 0);

	return ERR_OK;
}

HINT32 PORT_ITK_SVC_ParseUrlUntilHost(HCHAR* url, ITK_UrlData_t *uf)
{
	HCHAR *s = url, *e;

	HxSTD_memset(uf, 0, sizeof(ITK_UrlData_t));
	e = mheg_itk_port_svcStrScan(s, ":");
	if (e == s) goto err;
	uf->scheme = mheg_itk_port_svcStrGrab(s, e-s);
	s = e+1;
	if (*s == 0) goto err;
	if (!(*s == '/' && *(s+1) == '/')) goto err;
	s = s+2;
	if (*s == 0) goto err;
	e = mheg_itk_port_svcStrScan(s, "/");
	if (e == s) goto err;
	uf->host = mheg_itk_port_svcStrGrab(s, e-s);
	return 0;
err:
	mheg_itk_port_svcFreeUrlData(uf);
	return -(e-url);
}

void PORT_ITK_SVC_FreeUrlData(ITK_UrlData_t *uf)
{
	if (uf->scheme) HLIB_STD_MemFree(uf->scheme);
	if (uf->host) HLIB_STD_MemFree(uf->host);
	if (uf->name) HLIB_STD_MemFree(uf->name);
	if (uf->value) HLIB_STD_MemFree(uf->value);
}


