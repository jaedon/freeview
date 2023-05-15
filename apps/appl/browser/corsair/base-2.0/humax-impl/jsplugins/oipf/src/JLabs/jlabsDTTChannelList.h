
#ifndef __JLABS_DTTCHANNELLIST
#define __JLABS_DTTCHANNELLIST

#if defined OIPF && defined JLABS

#include "jsplugin.h"
#include "macros.h"
#include "Collection.h"
#include "WrapperObject.h"
#include "jlabsDTTChannel.h"
#include "NativeJlabsChannel.h"
#include "ChannelList.h"


namespace Ooif
{
	class jlabsDTTChannelList : public ChannelList, public WrapperObject<jlabsDTTChannelList>
	{
	public:
		jlabsDTTChannelList(int count, void* handle);
		jlabsDTTChannelList(jlabsDTTChannelList *original);

		virtual ~jlabsDTTChannelList();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		// Javascript functions
		static int getChannel(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getChannelByTriplet(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int item(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static void releaseNative(jlabsChannelListHandle id);

		static const ClassType class_name = CLASS_JLABS_DTT_CHANNEL_LIST;
	};
} /* namespace Ooif */

#endif	/* OIPF && JLABS */
#endif	/* __JLABS_DTTCHANNELLIST */
