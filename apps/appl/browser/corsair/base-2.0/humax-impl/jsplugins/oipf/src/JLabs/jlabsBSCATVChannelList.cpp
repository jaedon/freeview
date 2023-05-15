
#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"
#include "Exception.h"
#include "WrapperObject.h"
#include "Programme.h"

#include "ChannelList.h"
#include "NativeJlabsChannel.h"
#include "jlabsBSCATVChannelList.h"

using namespace Ooif;

jlabsBSCATVChannelList::jlabsBSCATVChannelList(int count, void* handle):ChannelList(NONE), WrapperObject<jlabsBSCATVChannelList>(handle)
{
	ENTRY;
	classType = CLASS_JLABS_BS_CATV_CHANNEL_LIST;

	jlabsChannelHandle *handles = (jlabsChannelHandle *)handle;
	for (int i=0; i<count; i++)
	{
		addItem(new jlabsBSCATVChannel(handles[i]));
	}
}

jlabsBSCATVChannelList::jlabsBSCATVChannelList(jlabsBSCATVChannelList *original):ChannelList(NONE), WrapperObject<jlabsBSCATVChannelList>(original->identifier)
{
	ENTRY;
	classType = CLASS_JLABS_BS_CATV_CHANNEL_LIST;
	jlabsBSCATVChannel *channel = NULL;
	for (int i=0; i<original->getSize(); i++)
	{
		channel = (jlabsBSCATVChannel *)(original->internalGetItem(i));
 		addItem(new jlabsBSCATVChannel(*channel));
 	}
}

jlabsBSCATVChannelList::~jlabsBSCATVChannelList()
{
	ENTRY;
}

int jlabsBSCATVChannelList::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_STRING("signalLevel", native_jlabsChannelListGetSignalLevel(getIdentifier(), getSize(),&RETVAL), JSP_GET_VALUE_CACHE);
#if defined(JLABS_JCOM)
	NATIVE_GET_NUMBER("agc", native_jlabsChannelListGetAgc(getIdentifier(), getSize(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("ber", native_jlabsChannelListGetBer(getIdentifier(), getSize(), &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("snr", native_jlabsChannelListGetSnr(getIdentifier(), getSize(), &RETVAL), JSP_GET_VALUE);
#endif

	GET_FUNCTION(getChannel, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getChannelByTriplet, obj, "n-n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(item, obj, "n", JSP_GET_VALUE_CACHE);

// TODO: getChannelBySorceId  // THROW_WRONG_ARG_ERR

	return ChannelList::getter(obj, name, result);
}

int jlabsBSCATVChannelList::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	//NATIVE_SET("interfaceType", native_jlabsChannelListSetOneTouch(NUMBER_VALUE(value[0])));

	return JSP_PUT_NOTFOUND;
}

//static
int jlabsBSCATVChannelList::getChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	jlabsBSCATVChannelList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsBSCATVChannelList);
	jlabsBSCATVChannel *channel = NULL;
	// Alright let's search through the list for this ChannelList
	for (int i = 0; i < instance->getSize(); i++) {
		channel = (jlabsBSCATVChannel *)(instance->internalGetItem(i));
		if (channel->data_wrapper)
		{
			// TODO: serviceId로 해도 되는가? OIPF channelList에서는 ccid를 사용하나 ccid는 구분자가 된다.
			if (NUMBER_VALUE(argv[0]) == channel->data_wrapper->data.sid)
			{
				RETURN_OBJECT(this_obj, new jlabsBSCATVChannel(*channel), result, JSP_CALL_VALUE);
			}
		}
	}

	RETURN_NULL(JSP_CALL_VALUE);
}

//static
int jlabsBSCATVChannelList::getChannelByTriplet(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(3);

	jlabsBSCATVChannelList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsBSCATVChannelList);
	jlabsBSCATVChannel *channel = NULL;
	// Alright let's search through the list for this ChannelList
	for (int i = 0; i < instance->getSize(); i++) {
		channel = (jlabsBSCATVChannel *)(instance->internalGetItem(i));
		if (channel->data_wrapper)
		{
			if (NUMBER_VALUE(argv[0]) == channel->data_wrapper->data.onid &&
					(!TYPE_NUMBER(argv[1]) || NUMBER_VALUE(argv[1]) == channel->data_wrapper->data.tsid)  &&
					NUMBER_VALUE(argv[2]) == channel->data_wrapper->data.sid)
			{
				RETURN_OBJECT(this_obj, new jlabsBSCATVChannel(*channel), result, JSP_CALL_VALUE);
			}
		}
	}

	RETURN_NULL(JSP_CALL_VALUE);
}

int jlabsBSCATVChannelList::item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);

	jlabsBSCATVChannelList *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, jlabsBSCATVChannelList);
	OOIFNumber index = NUMBER_VALUE(argv[0]);
	jlabsBSCATVChannel *channel = (jlabsBSCATVChannel *)instance->internalGetItem(index);

	if ((index > instance->getSize()) || (index < 0))
	{
		RETURN_NULL(JSP_CALL_VALUE);
	}

	RETURN_OBJECT(this_obj, new jlabsBSCATVChannel(*channel), result, JSP_CALL_VALUE);
}

void jlabsBSCATVChannelList::releaseNative(jlabsChannelListHandle id)
{
	NATIVE_WARN_NOK(native_jlabsChannelListReleaseChListHandle((jlabsChannelHandle*)id));
}

#endif
