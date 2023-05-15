// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "Collection.h"

#include "jsplugin.h"
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
#include "Download.h"
#endif // HBBTV_DOWNLOAD || OIPF
#ifdef OIPF
#include "Application.h"
#endif
#include "ParentalRatingScheme.h"
#include "ParentalRating.h"
#if defined(HBBTV_PVR) || defined(OIPF)
#include "ScheduledRecording.h"
#include "Recording.h"
#endif // HBBTV_PVR || OIPF
#ifdef OIPF
#include "Bookmark.h"
#endif // OIPF
#include "Channel.h"
#ifdef OIPF
#include "FavouriteList.h"
#endif // OIPF
#include "Exception.h"
#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
#include "DRMControlInformation.h"
#endif // HBBTV_DOWNLOAD || OIPF
#ifdef OIPF
#include "NetworkInterface.h"
#include "AVOutput.h"
#include "Tuner.h"
#include "WidgetDescriptor.h"
#endif // OIPF

#if defined(HMX_WEBUI)
#include "humax/HMX_Output.h"
#include "humax/HMX_Audio.h"
#include "humax/HNetworkInterface.h"
#include "humax/HMXPhysicalStorage.h"
#include "humax/HMXLogicalStorage.h"
#include "AntennaInfoCollection.h"
#include "humax/ChannelScanParameters.h"
#include "humax/HMX_Dlna.h"
//#include "humax/HMX_MediaScan.h"

#include "humax/HDlna.h"
#include "humax/HContentManager.h"
#include "humax/HWoon.h"
#include "humax/HSamba.h"
#include "humax/HFtpManager.h"
#include "humax/HBluetoothManager.h"
#include "RctNotifyBroadcastEvent.h"
#endif // HMX_WEBUI

#include "AVComponent.h"
#include "Programme.h"
#include "NativeOOIFTypes.h"
#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

#ifdef CONFIG_OP_FREESAT
#include "humax/HMXOperatorFreesat.h"
#endif

#ifdef JLABS
#include "jlabsEmmMessage.h"
#include "jlabsBoardMessage.h"
#include "jlabsCDSRecording.h"
#include "jlabsPPVHistory.h"
#include "jlabsDMS.h"
#include "jlabsScanning.h"
#include "jlabsNetworkInterface.h"
#include "jlabsNetworkService.h"
#include "jlabsWiFiAccessPoint.h"
#include "jlabsDTTChannel.h"
#include "jlabsBSCATVChannel.h"
#include "ChannelList.h"
#include "jlabsChannelListCollection.h"
#include "jlabsDataContentComponent.h"
#include "DiscInfo.h"
#include "HDiscInfo.h"
#include "jlabsRecordDestination.h"
#include "jlabsMultiViewComponent.h"
#endif // JLABS

#include "macros.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>

namespace Ooif
{

template <class T>
T BaseCollection<T>::internalGetItem(int index)
{
	return items[index];
}

template <class T>
int BaseCollection<T>::getSize()
{
	return items.size();
}

template <class T>
void BaseCollection<T>::addItem(T item)
{
	items.push_back(item);
}

template <class T>
void BaseCollection<T>::internalInsertAt(T item, int index)
{
	items.insert(items.begin()+index, item);
}

template <class T>
void BaseCollection<T>::internalRemoveIndex(int index)
{
	items.erase(items.begin() + index);
}

template <class T>
void BaseCollection<T>::internalRemoveItem(T item)
{
	items.erase(find(items.begin(), items.end(), item));
}

//static
template <class T>
int BaseCollection<T>::item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	CHECK_ARGUMENT(0, JSP_TYPE_NUMBER);

	BaseCollection<T> *instance = static_cast<BaseCollection<T>*>(EXTRACT_OBJECT<ObjectInstance>(this_obj));
	if (!instance)
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);

	if (instance->getSize() > NUMBER_VALUE(argv[0])  && NUMBER_VALUE(argv[0]) >= 0)
	{
		return instance->getItem(this_obj, result, NUMBER_VALUE(argv[0]), JSP_CALL_VALUE);
	}
	else
	{
		RETURN_UNDEFINED(JSP_CALL_VALUE);
	}
}

template <class T>
int BaseCollection<T>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	GET_NUMBER("length", this->getSize(), JSP_GET_VALUE);
	GET_FUNCTION(item, obj, "n", JSP_GET_VALUE_CACHE);

	// Array indexing (eg myCollection[0])
	int index = atoi(name);
	if (index > 0 || name[0] == '0')
	{
		if ( (unsigned int)index >= this->items.size())
			return JSP_GET_NOTFOUND;
		return getItem(obj, result, index, JSP_GET_VALUE);
	}

	return JSP_GET_NOTFOUND;
}

template <class T>
Collection<T>::~Collection()
{
	ENTRY;
	while (!this->items.empty())
	{
		delete this->items.back();
		this->items.pop_back();
	}
}

template <class T>
void Collection<T>::internalRemoveIndex(int index)
{
    delete this->items[index];
    this->items.erase(this->items.begin() + index);
}

template <class T>
int Collection<T>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	RETURN_OBJECT(this_obj, new T(*(this->internalGetItem(index))), result, return_type);
}

int NumberCollection::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	RETURN_NUMBER( this->internalGetItem(index), return_type);
}

int StringCollection::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	RETURN_STRING(this->internalGetItem(index), return_type);
}

StringCollection::~StringCollection()
{
	ENTRY;
	while (!this->items.empty())
	{
		HLIB_STD_MemFree((void*)this->items.back());
		this->items.pop_back();
	}
}

StringWithLength::StringWithLength(char* string, int string_len) : str(string), length(string_len)
{
	if(length < 0)
		length = strlen(string);
}

StringWithLength::~StringWithLength()
{
	HLIB_STD_MemFree(str);
}

template <>
int Collection<StringWithLength>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	StringWithLength *item = this->internalGetItem(index);
	RETURN_STRING_WITH_LENGTH(item->str, item->length, return_type);
}

#ifdef OIPF
ApplicationCollection::~ApplicationCollection()
{
	proteges.clear();
}

int ApplicationCollection::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	result->type = JSP_TYPE_OBJECT;
	result->u.object = internalGetItem(index)->getHost();
	return return_type;
}

void ApplicationCollection::addItem(Application* app)
{
	BaseCollection<Application*>::addItem(app);
	addProtege(app->getHost());
}
#endif // OIPF


#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
template <>
int Collection<ScheduledRecording>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif //HBBTV_SECURITY
	return BaseCollection<ScheduledRecording*>::getter(obj, name, result);
}

template <>
int Collection<ScheduledRecording>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // #ifdef HBBTV_SECURITY
    ScheduledRecording *scheduledRecording = this->internalGetItem(index);
    Recording *recording = dynamic_cast<Recording*>(scheduledRecording);

    if (recording)
    {
        RETURN_OBJECT(this_obj, new Recording(*recording), result, return_type);
    }
    else
    {
        RETURN_OBJECT(this_obj, new ScheduledRecording(*scheduledRecording), result, return_type);
    }
}

template <>
int Collection<Recording>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif //HBBTV_SECURITY
	return BaseCollection<Recording*>::getter(obj, name, result);
}

template <>
int Collection<Recording>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new Recording(*(this->internalGetItem(index))), result, return_type);
}
#endif //defined(HBBTV_DOWNLOAD) || defined(OIPF)

//#ifdef HBBTV_SECURITY
template <class T>
int Collection<T>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	return BaseCollection<T*>::getter(obj, name, result);
}

template <>
int Collection<Programme>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	return BaseCollection<Programme*>::getter(obj, name, result);
}

template <>
int Collection<Programme>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
	RETURN_OBJECT(this_obj, new Programme(*(this->internalGetItem(index))), result, return_type);
}

#ifdef HBBTV_DOWNLOAD
template <>
int Collection<Download>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif
	return BaseCollection<Download*>::getter(obj, name, result);
}

template <>
int Collection<Download>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif
	RETURN_OBJECT(this_obj, new Download(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<DRMControlInformation>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif
	return BaseCollection<DRMControlInformation*>::getter(obj, name, result);
}

template <>
int Collection<DRMControlInformation>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif
	RETURN_OBJECT(this_obj, new DRMControlInformation(*(this->internalGetItem(index))), result, return_type);
}
#endif // HBBTV_DOWNLOAD
//#endif // HBBTV_SECURITY

#if defined(HMX_WEBUI)
template <>
int Collection<HMX_Output>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HMX_Output(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HMX_Audio>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HMX_Audio(*(this->internalGetItem(index))), result, return_type);
}

/*******************************************************************/
/**********    		MediaList Collections   ***********************/
/*******************************************************************/

template <>
int Collection<HDlnaCds>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<HDlnaCds*>::getter(obj, name, result);
}

template <>
int Collection<HDlnaCds>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HDlnaCds(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HDlnaCdsContent>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<HDlnaCdsContent*>::getter(obj, name, result);
}

template <>
int Collection<HDlnaCdsContent>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HDlnaCdsContent(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HContent>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<HContent*>::getter(obj, name, result);
}

template <>
int Collection<HContent>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HContent(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HSambaSearchedServer>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<HSambaSearchedServer*>::getter(obj, name, result);
}

template <>
int Collection<HSambaSearchedServer>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HSambaSearchedServer(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HSmbsShareFolder>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<HSmbsShareFolder*>::getter(obj, name, result);
}

template <>
int Collection<HSmbsShareFolder>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HSmbsShareFolder(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HFtpsShareFolder>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<HFtpsShareFolder*>::getter(obj, name, result);
}

template <>
int Collection<HFtpsShareFolder>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HFtpsShareFolder(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HWoonFriend>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<HWoonFriend*>::getter(obj, name, result);
}

template <>
int Collection<HWoonFriend>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HWoonFriend(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HWoonContent>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<HWoonContent*>::getter(obj, name, result);
}

template <>
int Collection<HWoonContent>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new HWoonContent(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<HBluetoothDevice>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
    GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
    return BaseCollection<HBluetoothDevice*>::getter(obj, name, result);
}

template <>
int Collection<HBluetoothDevice>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
    CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
    RETURN_OBJECT(this_obj, new HBluetoothDevice(*(this->internalGetItem(index))), result, return_type);
}
#endif // #if defined(HMX_WEBUI)

#ifdef JLABS

template <>
int Collection<jlabsBoardMessage>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<jlabsBoardMessage*>::getter(obj, name, result);
}

template <>
int Collection<jlabsBoardMessage>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new jlabsBoardMessage(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<jlabsCDSRecording>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<jlabsCDSRecording*>::getter(obj, name, result);
}

template <>
int Collection<jlabsCDSRecording>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new jlabsCDSRecording(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<jlabsRecordDestination>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<jlabsRecordDestination*>::getter(obj, name, result);
}

template <>
int Collection<jlabsRecordDestination>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new jlabsRecordDestination(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<jlabsDMS>::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	GET_SECURITY_CHECK(CHECK_TRUSTED);
#endif // HBBTV_SECURITY
	return BaseCollection<jlabsDMS*>::getter(obj, name, result);
}

template <>
int Collection<jlabsDMS>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY
	RETURN_OBJECT(this_obj, new jlabsDMS(*(this->internalGetItem(index))), result, return_type);
}

template <>
int Collection<AVComponent>::getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_TRUSTED, result);
#endif // HBBTV_SECURITY

	AVComponent *obj = this->internalGetItem(index);

	if (obj->getType() == CLASS_VIDEOCOMPONENT)
	{
		VideoComponent *videoComponent = (VideoComponent *)obj;
		RETURN_OBJECT(this_obj, new VideoComponent(*(videoComponent)), result, return_type);
	}
	else if (obj->getType() == CLASS_AUDIOCOMPONENT)
	{
		AudioComponent *audioComponent = (AudioComponent *)obj;
		RETURN_OBJECT(this_obj, new AudioComponent(*(audioComponent)), result, return_type);
	}
	else if (obj->getType() == CLASS_SUBTITLECOMPONENT)
	{
		SubtitleComponent *subtitleComponent = (SubtitleComponent *)obj;
		RETURN_OBJECT(this_obj, new SubtitleComponent(*(subtitleComponent)), result, return_type);
	}
	else if (obj->getType() == CLASS_JLABS_DATA_CONTENT_COMPONENT)
	{
		jlabsDataContentComponent*dataContentComponent = (jlabsDataContentComponent *)obj;
		RETURN_OBJECT(this_obj, new jlabsDataContentComponent(*(dataContentComponent)), result, return_type);
	}

	return JSP_GET_ERROR;
}
#endif //JLABS

template class BaseCollection<OOIFNumber>;
template class BaseCollection<const char*>;
#define INSTANTIATE_COLLECTION(element) template class BaseCollection<element*>; template class Collection<element>

#ifdef OIPF
INSTANTIATE_COLLECTION(Application);
#endif // OIPF

#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
INSTANTIATE_COLLECTION(Download);
#endif // HBBTV_DOWNLOAD || OIPF

#if defined(HBBTV_PVR) || defined(OIPF)
INSTANTIATE_COLLECTION(ScheduledRecording);
INSTANTIATE_COLLECTION(Recording);
#endif // HBBTV_PVR || OIPF

INSTANTIATE_COLLECTION(StringWithLength);

#if defined(HBBTV_DOWNLOAD) || defined(OIPF)
INSTANTIATE_COLLECTION(DRMControlInformation);
#endif // HBBTV_DOWNLOAD || OIPF

#ifdef OIPF
INSTANTIATE_COLLECTION(NetworkInterface);
INSTANTIATE_COLLECTION(AVOutput);
INSTANTIATE_COLLECTION(Tuner);
#endif // OIPF

INSTANTIATE_COLLECTION(AVComponent);
INSTANTIATE_COLLECTION(Programme);
INSTANTIATE_COLLECTION(ParentalRatingScheme);
INSTANTIATE_COLLECTION(ParentalRating);
INSTANTIATE_COLLECTION(Channel);

#ifdef OIPF
INSTANTIATE_COLLECTION(Bookmark);
INSTANTIATE_COLLECTION(FavouriteList);
INSTANTIATE_COLLECTION(WidgetDescriptor);
#endif // OIPF

#if defined(HMX_WEBUI)
INSTANTIATE_COLLECTION(HMX_Output);
INSTANTIATE_COLLECTION(HMX_Audio);
INSTANTIATE_COLLECTION(HNetworkInterface);
INSTANTIATE_COLLECTION(HMXPhysicalStorage);
INSTANTIATE_COLLECTION(HMXLogicalStorage);
INSTANTIATE_COLLECTION(HMX_DLNAContent);
//INSTANTIATE_COLLECTION(HMX_MediaContent);

INSTANTIATE_COLLECTION(AntennaInfo);
INSTANTIATE_COLLECTION(ChannelScanParameters);

INSTANTIATE_COLLECTION(HContent);
INSTANTIATE_COLLECTION(HDlnaCds);
INSTANTIATE_COLLECTION(HDlnaCdsContent);
INSTANTIATE_COLLECTION(HWoonFriend);
INSTANTIATE_COLLECTION(HWoonContent);
INSTANTIATE_COLLECTION(HSambaSearchedServer);
INSTANTIATE_COLLECTION(HSmbsShareFolder);
INSTANTIATE_COLLECTION(HFtpsShareFolder);
INSTANTIATE_COLLECTION(HBluetoothDevice);

INSTANTIATE_COLLECTION(RctLinkInfo);
// End of KTH
#endif

#ifdef CONFIG_OP_FREESAT
INSTANTIATE_COLLECTION(HMXOperatorFreesat);
#endif

#ifdef JLABS
INSTANTIATE_COLLECTION(jlabsMultiViewComponent);
INSTANTIATE_COLLECTION(jlabsEmmMessage);
INSTANTIATE_COLLECTION(jlabsBoardMessage);
INSTANTIATE_COLLECTION(jlabsCDSRecording);
INSTANTIATE_COLLECTION(jlabsPPVHistory);
INSTANTIATE_COLLECTION(jlabsDTTChannel);
INSTANTIATE_COLLECTION(jlabsBSCATVChannel);
INSTANTIATE_COLLECTION(jlabsDMS);
INSTANTIATE_COLLECTION(jlabsNetworkInterface);
INSTANTIATE_COLLECTION(jlabsNetworkService);
INSTANTIATE_COLLECTION(jlabsWiFiAccessPoint);
INSTANTIATE_COLLECTION(jlabsScanning);
INSTANTIATE_COLLECTION(ChannelList);
INSTANTIATE_COLLECTION(jlabsChannelListCollection);
INSTANTIATE_COLLECTION(HDiscInfo);
INSTANTIATE_COLLECTION(jlabsRecordDestination);
#endif // JLABS

#undef INSTANTIATE_COLLECTION
} /*namespace Ooif */
