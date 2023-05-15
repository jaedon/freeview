#if defined OIPF && defined JLABS
#ifndef __JLABS_PPVHISTROY
#define __JLABS_PPVHISTROY

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "NativeJlabsPPVHistory.h"
#include "WrapperObject.h"

namespace Ooif
{
	class jlabsPPVHistory : public ObjectInstance, public WrapperObject<jlabsPPVHistory>
	{
	private:
		void init();
	
	public:
		jlabsPPVHistory(jlabsPPVHistoryHandle i);
		jlabsPPVHistory(const jlabsPPVHistory &original);
		virtual ~jlabsPPVHistory();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		static int get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int set(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void releaseNative(jlabsPPVHistoryHandle id);	
		// JS functions
	};
} /* namespace Ooif */

#endif // __JLABS_PPVHISTROY
#endif // #if defined OIPF && defined JLABS
