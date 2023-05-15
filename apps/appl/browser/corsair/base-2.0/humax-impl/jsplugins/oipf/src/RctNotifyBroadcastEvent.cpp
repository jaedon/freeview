// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "RctNotifyBroadcastEvent.h"
#include "oplbroadcast.h"

#include <stdio.h>
#include <stdlib.h>


#include "macros.h"


#include <sstream>  

#define GET_STRING_FROM_INT(identifier, value, returntype) \
	do { \
        if ( !value ) { \
            this->m_buff.clear(); \
            std::stringstream ss; \
            ss << value; \
            this->m_buff = ss.str(); \
            GET_STRING("startDate",this->m_buff.c_str(), JSP_GET_VALUE_CACHE); \
        }  \
	} while (0)

using namespace Ooif;
/*
RctNotifyBroadcastEvent::RctNotifyBroadcastEvent(jsplugin_obj *c, OOIFBoolean _display, const char *_rctList )
  : 	Event(c, "RctInfoChanged")


{
	ENTRY;


	display = _display;
	if(_rctList)
		rctList = strdup(_rctList);
	else
		rctList = NULL;
}
*/
RctNotifyBroadcastEvent::RctNotifyBroadcastEvent(jsplugin_obj *c, void *aRctInfo)
  : 	Event(c, "RctInfoChanged")


{
	ENTRY;

	if(aRctInfo == NULL)
	{
		mRctInfo = NULL;
		mLinkCount = 0;
//		OOIF_LOG_PRINT("\n\n##### sh ##### [%s][%d] mRctInfo == NULL mLinkCount=[%d]\n\n",__FUNCTION__,__LINE__,mLinkCount);
	}
	else
	{
		mRctInfo = (DxService_RctInfo_t *)aRctInfo;
		mLinkCount = mRctInfo->ulTotalLinkCnt;
//		OOIF_LOG_PRINT("\n\n##### sh ##### [%s][%d] mLinkCount=[%d]\n\n",__FUNCTION__,__LINE__,mLinkCount);
	}
}

RctNotifyBroadcastEvent::~RctNotifyBroadcastEvent()
{
	ENTRY;
	/*
	if(rctList != NULL)
	{
		free(rctList);
	}
	*/
}

int RctNotifyBroadcastEvent::getter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *result)
{
	ENTRY;

	/*

	GET_FUNCTION(valueOf, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(toString, obj, "", JSP_GET_VALUE_CACHE);
	GET_BOOLEAN("display", display, JSP_GET_VALUE_CACHE);
	GET_STRING("rctList", rctList, JSP_GET_VALUE_CACHE);
	GET_NUMBER("linkCount", mLinkCount, JSP_GET_VALUE_CACHE);
	if (strcmp(name, "rctList") == 0)
	{
		if (mRctInfo)
		{
			RETURN_OBJECT(obj, new RctInfoCollection(mRctInfo), result, JSP_GET_VALUE_CACHE);
		}
		else
		{
			RETURN_NULL(JSP_GET_VALUE_CACHE);
		}
	}
*/

	return JSP_GET_NOTFOUND;
}

int RctNotifyBroadcastEvent::setter(jsplugin_obj *obj, const char *name,
								  jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

//virtual
int RctNotifyBroadcastEvent::serialize(jsplugin_value **argv)
{
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	if (mRctInfo)
	{
		result[0].type = JSP_TYPE_NUMBER;
		result[0].u.number = mLinkCount;

		SET_SERIALIZED_OBJECT( new RctInfoCollection(mRctInfo), 1);
	}
	else
	{
		result[0].type = JSP_TYPE_NUMBER;
		result[0].u.number = mLinkCount;
		result[1].type = JSP_TYPE_NULL;
	}

	return 2;
/*
	*argv = new jsplugin_value[2];

	jsplugin_value *result = *argv;

	result[0].type = JSP_TYPE_BOOLEAN;
	result[0].u.boolean = display;

	result[1].type = JSP_TYPE_STRING;
	result[1].u.string = rctList;

	return 2;
*/
}

RctImageIconInfo::RctImageIconInfo(DxService_RctImageIconInfo_t* aRctImageIconInfo)
{
	ENTRY;
	classType = CLASS_RCT_IMAGE_ICON_INFO;

	memcpy(&mRctImageIconInfo, aRctImageIconInfo, sizeof(DxService_RctImageIconInfo_t));
}
RctImageIconInfo::~RctImageIconInfo()
{
	ENTRY;
}
int RctImageIconInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_NUMBER("iconId", mRctImageIconInfo.ucIconId, JSP_GET_VALUE_CACHE);
	GET_NUMBER("transPortMode", mRctImageIconInfo.ucTransPortMode, JSP_GET_VALUE_CACHE);
	GET_NUMBER("positionFlag", mRctImageIconInfo.ucPositionFlag, JSP_GET_VALUE_CACHE);
	GET_NUMBER("coordinateSystem", mRctImageIconInfo.ucCoordinateSystem, JSP_GET_VALUE_CACHE);
	GET_NUMBER("horizontalOrigin", mRctImageIconInfo.usHorizontalOrigin, JSP_GET_VALUE_CACHE);
	GET_NUMBER("verticalOrigin", mRctImageIconInfo.usVerticalOrigin, JSP_GET_VALUE_CACHE);
	GET_STRING("iconType", (char *)mRctImageIconInfo.szIconTypeChar, JSP_GET_VALUE_CACHE);
	GET_NUMBER("dataByteSize", mRctImageIconInfo.ulDataByteSize, JSP_GET_VALUE_CACHE);
	GET_STRING("dataByte", (char *)(mRctImageIconInfo.pucDatabyte), JSP_GET_VALUE_CACHE);
	GET_STRING("iconUrl", (char *)mRctImageIconInfo.szIconUrl, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}
int RctImageIconInfo::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}



RctDvbBinaryLocator::RctDvbBinaryLocator(DxService_RctDvbBinaryLocator_t aRctDvbBinaryLocator)
{
	ENTRY;
	classType = CLASS_RCT_DVB_BINARY_LOCATOR;
	mRctDvbBinaryLocator = aRctDvbBinaryLocator;
}
RctDvbBinaryLocator::~RctDvbBinaryLocator()
{
	ENTRY;
}

int RctDvbBinaryLocator::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_NUMBER("identifierType", mRctDvbBinaryLocator.ucIdentifierType, JSP_GET_VALUE_CACHE);
	GET_NUMBER("scheTimeReliability", mRctDvbBinaryLocator.ucScheTimeReliability, JSP_GET_VALUE_CACHE);
	GET_NUMBER("inlineSvc", mRctDvbBinaryLocator.ucInlineSvc, JSP_GET_VALUE_CACHE);
    GET_STRING_FROM_INT("startDate", mRctDvbBinaryLocator.usStartDate, JSP_GET_VALUE_CACHE);
    GET_STRING_FROM_INT("dvbSvcTripletId", mRctDvbBinaryLocator.usDvbSvcTripletId, JSP_GET_VALUE_CACHE);
	GET_STRING_FROM_INT("tsId", mRctDvbBinaryLocator.usTsId, JSP_GET_VALUE_CACHE);
	GET_STRING_FROM_INT("onId", mRctDvbBinaryLocator.usOnId, JSP_GET_VALUE_CACHE);
	GET_STRING_FROM_INT("svcId", mRctDvbBinaryLocator.usSvcId, JSP_GET_VALUE_CACHE);
	GET_STRING_FROM_INT("startTime", mRctDvbBinaryLocator.usStartTime, JSP_GET_VALUE_CACHE);
	GET_STRING_FROM_INT("duration", mRctDvbBinaryLocator.usDuration, JSP_GET_VALUE_CACHE);
	GET_STRING_FROM_INT("evtId", mRctDvbBinaryLocator.usEvtId, JSP_GET_VALUE_CACHE);
	GET_STRING_FROM_INT("tvaId", mRctDvbBinaryLocator.usTvaId, JSP_GET_VALUE_CACHE);
	GET_NUMBER("component", mRctDvbBinaryLocator.ucComponent, JSP_GET_VALUE_CACHE);
	GET_NUMBER("earlyStartWin", mRctDvbBinaryLocator.ucEarlyStartWin, JSP_GET_VALUE_CACHE);
	GET_NUMBER("lateEndWin", mRctDvbBinaryLocator.ucLateEndWin, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int RctDvbBinaryLocator::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

RctLinkInfo::RctLinkInfo(DxService_RctLinkInfo_t* aRctLinkInfo)
{
	ENTRY;
	classType = CLASS_RCT_INFO;

	memcpy(&mRctLinkInfo, aRctLinkInfo, sizeof(DxService_RctLinkInfo_t));
}
RctLinkInfo::~RctLinkInfo()
{
	ENTRY;
}

int RctLinkInfo::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_NUMBER("linkType", mRctLinkInfo.ucLinkType, JSP_GET_VALUE_CACHE);
	GET_NUMBER("howRelatedSchemId", mRctLinkInfo.ucHowRelatedSchemeId, JSP_GET_VALUE_CACHE);
	GET_NUMBER("termId", mRctLinkInfo.usTermId, JSP_GET_VALUE_CACHE);
	GET_NUMBER("groupId", mRctLinkInfo.ucGroupId, JSP_GET_VALUE_CACHE);
	GET_NUMBER("precedence", mRctLinkInfo.ucPrecedence, JSP_GET_VALUE_CACHE);
	GET_STRING("mediaUri", (char *)mRctLinkInfo.stMediaUri.szMediaUri, JSP_GET_VALUE_CACHE);
	GET_OBJECT("dvbBinLocator", obj, new RctDvbBinaryLocator(mRctLinkInfo.stDvbBinLocator), JSP_GET_VALUE_CACHE);
	GET_NUMBER("proTextNum", mRctLinkInfo.ucProTextNum, JSP_GET_VALUE_CACHE);
	GET_STRING("promotionalText", (char *)mRctLinkInfo.stPromotionalText.szPromotionalText, JSP_GET_VALUE_CACHE);
	GET_STRING("promotionalTextLangCode",(char *) mRctLinkInfo.stPromotionalText.szLangCode, JSP_GET_VALUE_CACHE);
	GET_STRING("eventName",(char *) mRctLinkInfo.stShortEvtInfoInLink.szEventName, JSP_GET_VALUE_CACHE);
	GET_BOOLEAN("isDefaultIcon", mRctLinkInfo.bDefaultIcon, JSP_GET_VALUE_CACHE);
	GET_NUMBER("iconId", mRctLinkInfo.ucIconId, JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int RctLinkInfo::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;
	return JSP_PUT_NOTFOUND;
}

RctInfoCollection::RctInfoCollection(DxService_RctInfo_t *aRctInfo)
{
	ENTRY;
	classType = CLASS_RCT_INFO_COLLECTION;

	mRctInfo = aRctInfo;

	int	count = aRctInfo->ulTotalLinkCnt;
	for (int i = 0; i < count; i++)
	{
//		OOIF_LOG_PRINT("##### sh ##### [%s][%d] aRctInfo->pstLinkInfoList[%d]=[%d]\n",__FUNCTION__,__LINE__,i,aRctInfo->pstLinkInfoList[i].usTermId);
		addItem(new RctLinkInfo(&(aRctInfo->pstLinkInfoList[i])));
	}

}

RctInfoCollection::~RctInfoCollection()
{
	ENTRY;
//	OOIF_LOG_PRINT("\n\n##### sh ##### [%s][%d] \n\n",__FUNCTION__,__LINE__);
	if(mRctInfo)
	{
		if(OPL_Broadcast_ReleaseRctInfo(mRctInfo))
		{
			OOIF_LOG_ERROR("[%s] OPL_Broadcast_ReleaseRctInfo returned false\n",__FUNCTION__);
		}
	}
}

int RctInfoCollection::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	int ret = Collection<RctLinkInfo>::getter(obj, name, result);
	if (ret != JSP_GET_NOTFOUND)
		return ret;
	GET_OBJECT("imageIconInfo", obj, new RctImageIconInfo(&(mRctInfo->stImageIconInfo)), JSP_GET_VALUE_CACHE);
	GET_NUMBER("totalLinkCnt", mRctInfo->ulTotalLinkCnt, JSP_GET_VALUE_CACHE);
	GET_NUMBER("serviceId", mRctInfo->usServiceId, JSP_GET_VALUE_CACHE);
	GET_NUMBER("yearOffset", mRctInfo->usYearOffset, JSP_GET_VALUE_CACHE);
	return JSP_GET_NOTFOUND;
}




/* EOF */

