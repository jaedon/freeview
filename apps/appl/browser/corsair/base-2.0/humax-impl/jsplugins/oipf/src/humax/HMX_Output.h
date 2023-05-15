#ifdef OIPF
#ifndef __HMX_OUTPUT
#define __HMX_OUTPUT

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"

#include "NativeHConfiguration.h"

namespace Ooif
{
	class HMX_Output : public ObjectInstance, public WrapperObject<HMX_Output>
	{

	public:
		HMX_Output(HMX_OutputHandle i);
		HMX_Output(const HMX_Output &original);
		~HMX_Output();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static void releaseNative(HMX_OutputHandle id);
	};
} /* namespace Ooif */

#endif // __AVOUTPUT
#endif // OIPF

