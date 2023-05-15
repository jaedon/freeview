#if defined OIPF && defined JLABS

#ifndef __JLABS_PPVHISTORYCOLLECTION
#define __JLABS_PPVHISTORYCOLLECTION

#include "jsplugin.h"
#include <vector>
#include "macros.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "jlabsPPVHistory.h"

namespace Ooif
{
	class jlabsPPVHistoryCollection : public EventTarget
	{
	private:
		vector<jlabsPPVHistory*> items;
		void init();
		jlabsPPVHistory* internalGetItem(int index);
		static list<jlabsPPVHistoryCollection*> instances;
	public:
		jlabsPPVHistoryCollection();
		~jlabsPPVHistoryCollection();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		int getSize();
		void addItem(jlabsPPVHistory* item);
		int getItem(jsplugin_obj *this_obj, jsplugin_value *result, int index, int return_type);

		// JS functions
		static int item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int uploadPPVViewlogData(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int deleteAllPPVPrivateData(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getPPVPurchaseHistory(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int cancelPPVViewlogDataUpload(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void onUploadPPVViewlogDataEvent(int result);

		static const ClassType class_name = CLASS_JLABS_PPV_HISTORY_COLLECTION;
	};

} /* namespace Ooif */

#endif // __JLABS_PPVHISTORYCOLLECTION
#endif // #if defined OIPF && defined JLABS
