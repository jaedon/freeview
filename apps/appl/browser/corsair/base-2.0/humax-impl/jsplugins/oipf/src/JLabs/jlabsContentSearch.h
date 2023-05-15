#ifndef __JLABSCONTENTSEARCH__
#define __JLABSCONTENTSEARCH__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeJlabsContentSearch.h"
#include "NativeBookmark.h"
#include "Collection.h"
#include "Query.h"

namespace Ooif
{

	class jlabsContentSearch: public ObjectInstance, public WrapperObject<jlabsContentSearch>
	{

		private :
		public:
			~jlabsContentSearch();
			jlabsContentSearch(jlabsContentSearchHandle i);
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

			static int setQuery(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int orderBy(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int createQuery(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

			static void releaseNative(jlabsContentSearchHandle id);
			static const ClassType class_name = CLASS_JLABS_CONTENTSEARCH;
	};
} /* namespace Ooif */

#endif // __JLABSCONTENTSEARCH__
