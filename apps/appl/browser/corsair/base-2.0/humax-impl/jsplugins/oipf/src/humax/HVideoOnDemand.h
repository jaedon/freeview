
#ifndef __HVIDEOONDEMAND_H__
#define __HVIDEOONDEMAND_H__

#include "VideoOnDemand.h"
#include <oplvodhandle.h>

namespace Ooif
{
	class HVideoOnDemand: public VideoOnDemand, public OplVideoHandle::IAudioPathChangeListener
	{
	private:
		HVideoOnDemand(VodHandle i);
	public:
		static HVideoOnDemand* create();
		~HVideoOnDemand();
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		static int requestAudioPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int hasAudioPath(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
	private:
		virtual void audioPathChanged(bool acquired);
	};
}

#endif // __HVIDEOONDEMAND_H__