#ifdef OIPF
#ifndef __HMX_AUDIO
#define __HMX_AUDIO

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeHConfiguration.h"

namespace Ooif
{
	class HMX_Audio : public ObjectInstance, public WrapperObject<HMX_Audio>
	{

	public:
		HMX_Audio(HMX_AudioHandle i);
		HMX_Audio(const HMX_Audio &original);
		~HMX_Audio();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative(HMX_AudioHandle id);
	};

} /* namespace Ooif */

#endif
#endif // OIPF


