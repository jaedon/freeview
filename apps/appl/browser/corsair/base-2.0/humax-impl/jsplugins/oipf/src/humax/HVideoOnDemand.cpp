
#include <string.h>
#include "macros.h"

#include "HVideoOnDemand.h"
#include "EmptyEvent.h"
#include "oplvodavchandle.h"

namespace Ooif
{

HVideoOnDemand* HVideoOnDemand::create()
{
	OplVodHandle* handle = OplVodAvcHandle::create(OplVodHandle::EHandleOwnerTypeSystem);
	HVideoOnDemand* ret = new HVideoOnDemand(handle);
	handle->setAudioPathChangeListener(ret);
	return ret;
}

HVideoOnDemand::HVideoOnDemand(VodHandle i)
	: VideoOnDemand(i)
{
}

HVideoOnDemand::~HVideoOnDemand()
{
}

int HVideoOnDemand::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	GET_FUNCTION(requestAudioPath, obj, "", JSP_GET_VALUE);
	GET_FUNCTION(hasAudioPath, obj, "", JSP_GET_VALUE);
	INTRINSIC_EVENT_GET(AudioPathAcquired);
	INTRINSIC_EVENT_GET(AudioPathReleased);
	return VideoOnDemand::getter(obj, name, result);
}

int HVideoOnDemand::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	INTRINSIC_EVENT(AudioPathAcquired);
	INTRINSIC_EVENT(AudioPathReleased);
	return VideoOnDemand::setter(obj, name, value);
}

int HVideoOnDemand::requestAudioPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	HVideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HVideoOnDemand);
	OplVideoHandle* handle = reinterpret_cast<OplVideoHandle*>(instance->getIdentifier());
	handle->requestAudioPath();
	return JSP_CALL_NO_VALUE;
}

int HVideoOnDemand::hasAudioPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(0);
	HVideoOnDemand *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, HVideoOnDemand);
	OplVideoHandle* handle = reinterpret_cast<OplVideoHandle*>(instance->getIdentifier());
	OOIFBoolean ret = handle->hasAudioPath();
	RETURN_BOOLEAN(ret, JSP_CALL_VALUE);
}

void HVideoOnDemand::audioPathChanged(bool acquired)
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
