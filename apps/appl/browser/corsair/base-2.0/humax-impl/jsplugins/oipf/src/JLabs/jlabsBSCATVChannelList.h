
#ifndef __JLABS_BSCATVCHANNELLIST
#define __JLABS_BSCATVCHANNELLIST

#if defined OIPF && defined JLABS

#include "jsplugin.h"
#include "macros.h"
#include "Collection.h"
#include "jlabsBSCATVChannel.h"
#include "NativeJlabsChannel.h"
#include "NativeOOIFTypes.h"
#include "ChannelList.h"


namespace Ooif
{
	class jlabsBSCATVChannelList : public ChannelList, public WrapperObject<jlabsBSCATVChannelList>
	{
	public:
		jlabsBSCATVChannelList(int count, void* handle);
		jlabsBSCATVChannelList(jlabsBSCATVChannelList *original);
//		jlabsBSCATVChannelList(OOIFNumber network);
		virtual ~jlabsBSCATVChannelList();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		// Javascript functions
		static int getChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getChannelByTriplet(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void releaseNative(jlabsChannelListHandle id);

		static const ClassType class_name = CLASS_JLABS_BS_CATV_CHANNEL_LIST;
	};
} /* namespace Ooif */

#endif	/* OIPF && JLABS */
#endif	/* __JLABS_BSCATVCHANNELLIST */
