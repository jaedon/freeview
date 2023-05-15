
#ifndef __HVIDEOBROADCAST_H__
#define __HVIDEOBROADCAST_H__

#include "VideoBroadcast.h"
#include <oplvideobroadcasthandle.h>

namespace Ooif
{
	class HVideoBroadcast: public VideoBroadcast, public OplVideoHandle::IAudioPathChangeListener
	{
	private:
		HVideoBroadcast(VideoBroadcastHandle i);
	public:
		static HVideoBroadcast* create();
		~HVideoBroadcast();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static int requestAudioPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int hasAudioPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	private:
		virtual void audioPathChanged(bool acquired);
	};
}

#endif // __HVIDEOBROADCAST_H__