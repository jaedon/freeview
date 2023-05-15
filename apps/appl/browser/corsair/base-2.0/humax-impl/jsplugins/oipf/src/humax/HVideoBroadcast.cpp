
#include "macros.h"

#include "HVideoBroadcast.h"
#include "EmptyEvent.h"

void native_BroadcastRegisterListener(OplVideoBroadcastHandle *vbcastObj);
namespace Ooif
{

HVideoBroadcast* HVideoBroadcast::create()
{
	OplVideoBroadcastHandle* handle = OplVideoBroadcastHandle::create(OplVideoHandle::EHandleOwnerTypeSystem);
	native_BroadcastRegisterListener(handle);
	HVideoBroadcast* ret = new HVideoBroadcast(handle);
	handle->setAudioPathChangeListener(ret);
	return ret;
}
	
HVideoBroadcast::HVideoBroadcast(VideoBroadcastHandle i)
	: VideoBroadcast(i)
{
}

HVideoBroadcast::~HVideoBroadcast()
{
}

int HVideoBroadcast::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(requestAudioPath, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(hasAudioPath, obj, "", JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(AudioPathAcquired);
	INTRINSIC_EVENT_GET(AudioPathReleased);
	return VideoBroadcast::getter(obj, name, result);
}

int HVideoBroadcast::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	INTRINSIC_EVENT(AudioPathAcquired);
	INTRINSIC_EVENT(AudioPathReleased);
	return VideoBroadcast::setter(obj, name, value);
}

int HVideoBroadcast::requestAudioPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	HVideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HVideoBroadcast);
	OplVideoBroadcastHandle* handle = reinterpret_cast<OplVideoBroadcastHandle*>(instance->getIdentifier());
	handle->requestAudioPath();
	return JSP_CALL_NO_VALUE;
}

int HVideoBroadcast::hasAudioPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	HVideoBroadcast *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HVideoBroadcast);
	OplVideoBroadcastHandle* handle = reinterpret_cast<OplVideoBroadcastHandle*>(instance->getIdentifier());
	OOIFBoolean ret = handle->hasAudioPath();
	RETURN_BOOLEAN(ret, JSP_CALL_VALUE);
}

void HVideoBroadcast::audioPathChanged(bool acquired)
{
	jsplugin_obj *context(getHost());
	Event *e(NULL);
	if (acquired)
	{
		e = new EmptyEvent(context, "AudioPathAcquired");
	}
	else
	{
		e = new EmptyEvent(context, "AudioPathReleased");
	}
	dispatchEvent(e);
}

}
