
#ifndef __JLABS_BSCATVCHANNEL
#define __JLABS_BSCATVCHANNEL

#if defined OIPF && defined JLABS

#include "NativeJlabsChannel.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "Collection.h"
#include "Channel.h"

namespace Ooif
{
	class jlabsBSCATVChannel : public Channel
	{
	private:
		void init();

	public:
		jlabsBSCATVChannel(jlabsChannelHandle i);
		jlabsBSCATVChannel(const jlabsBSCATVChannel &original);
		~jlabsBSCATVChannel();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		// Specified functions
		static int getField(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int getLogo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		/* WrapperObject는 필요하다. */
		static void releaseNative(jlabsChannelHandle id);

		static const ClassType class_name = CLASS_JLABS_BS_CATV_CHANNEL;
	};
} /* namespace Ooif */

#endif	/* OIPF && JLABS */
#endif	/* __JLABS_BSCATVCHANNEL */
