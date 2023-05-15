#if defined OIPF && defined JLABS
#ifndef __JLABS_MISCSETTING
#define __JLABS_MISCSETTING

#include "jsplugin.h"
#include "ObjectInstance.h"

namespace Ooif
{
	class jlabsMiscSetting : public ObjectInstance
	{
	public:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		// JS functions
		static int get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int set(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	};
} /* namespace Ooif */

#endif // __JLABS_MISCSETTING
#endif // #if defined OIPF && defined JLABS
