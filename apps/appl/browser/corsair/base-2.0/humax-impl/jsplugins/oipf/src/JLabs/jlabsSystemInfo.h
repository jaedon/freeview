#if defined OIPF && defined JLABS
#ifndef __JLABS_SYSTEMINFO
#define __JLABS_SYSTEMINFO

#include "jsplugin.h"
#include "ObjectInstance.h"

namespace Ooif
{
	class jlabsSystemInfo : public ObjectInstance
	{
	public:
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		// JS functions

		static int get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int startSettingInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setDocsisDownStreamFrequency(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	};
} /* namespace Ooif */

#endif // __JLABS_SYSTEMINFO
#endif // #if defined OIPF && defined JLABS
